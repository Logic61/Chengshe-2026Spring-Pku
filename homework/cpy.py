import os
import requests
import re
import time
from bs4 import BeautifulSoup
from urllib.parse import urljoin

# --- 配置 ---
TARGET_DIR = r"C:\Users\32372\课程\Practice-on-Programming-Design-2026-Spring-PKU\homework\C++11及高级特性"
BASE_URL = "http://cxsjsx.openjudge.cn/hw202612/"

headers = {
    'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36'
}

def fetch_url(url, max_retries=3, delay=1):
    for attempt in range(max_retries):
        try:
            r = requests.get(url, headers=headers, timeout=5)
            r.encoding = 'utf-8'
            return r
        except Exception as e:
            print(f"请求 {url} 失败 (尝试 {attempt+1}/{max_retries}): {e}")
            if attempt < max_retries - 1:
                time.sleep(delay)
            else:
                raise
    raise Exception(f"无法获取 {url}")

def extract_problem_content(soup):
    """尝试多种方式提取题目描述内容"""
    # 方法1: class="problem-page"
    container = soup.find('div', class_='problem-page')
    if not container:
        # 方法2: class="problem-content"
        container = soup.find('div', class_='problem-content')
    if not container:
        # 方法3: id="problem-content"
        container = soup.find('div', id='problem-content')
    if not container:
        # 方法4: 直接找所有的dl.problem-content
        dl_list = soup.find_all('dl', class_='problem-content')
        if dl_list:
            parts = []
            for dl in dl_list:
                for child in dl.find_all(['dt', 'dd']):
                    parts.append(child.get_text("\n", strip=True))
            return "\n\n".join(parts)
        return ""
    
    # 对于找到的容器，提取文本
    # 去除脚本和样式
    for script in container(["script", "style"]):
        script.decompose()
    text = container.get_text("\n", strip=True)
    return text

def sync_problem(problem_url):
    try:
        # 跳过统计页面等非题目页面
        if '/statistics/' in problem_url or '/submit' in problem_url or '/rank' in problem_url:
            print(f"跳过非题目页面: {problem_url}")
            return

        r = fetch_url(problem_url)
        soup = BeautifulSoup(r.text, 'html.parser')

        # 1. 提取题目字母编号
        letter_match = re.search(r'/hw202612/([A-Z])/', problem_url)
        if not letter_match:
            print(f"警告: 无法从 {problem_url} 提取字母编号，跳过。")
            return
        letter = letter_match.group(1)

        # 2. 提取标题
        title_node = soup.find('div', id='pageTitle')
        if not title_node:
            print(f"警告: 无法获取 {problem_url} 的标题，跳过。")
            return
        raw_title = title_node.get_text(strip=True)

        # 3. 安全文件名
        safe_title = re.sub(r'\s+', '-', raw_title.lower())
        safe_title = re.sub(r'[^a-z0-9\-_]', '', safe_title)
        filename = f"{letter}-{safe_title}.cpp"

        # 4. 提取题目内容（用于注释）
        comment_body = extract_problem_content(soup)
        if not comment_body:
            print(f"警告: 未提取到题目内容 {problem_url}")

        # 5. 提取代码填空逻辑
        pre_tags = soup.find_all('pre')
        code_template = ""
        is_fill_blank = False
        fill_idx = -1
        for i, pre in enumerate(pre_tags):
            text = pre.get_text()
            if "在此处补充你的代码" in text or "Your Code Here" in text:
                fill_idx = i
                is_fill_blank = True
                break

        if is_fill_blank:
            before = pre_tags[fill_idx-1].get_text() if fill_idx > 0 else "// [Prefix Code]"
            hint = "// 在此处补充你的代码"
            after = pre_tags[fill_idx+1].get_text() if fill_idx < len(pre_tags)-1 else "// [Suffix Code]"
            code_template = f"{before}\n\n{hint}\n\n{after}"
        else:
            code_template = "#include <iostream>\nusing namespace std;\n\nint main() {\n    // TODO: Write your code here\n    return 0;\n}\n"

        # 6. 组装文件
        comment_footer = f"\nURL: {problem_url}\n--------------------\n"
        final_content = f"/*\n{comment_body}{comment_footer}*/\n\n{code_template}"

        # 7. 写入
        if not os.path.exists(TARGET_DIR):
            os.makedirs(TARGET_DIR)
        file_path = os.path.join(TARGET_DIR, filename)
        with open(file_path, "w", encoding="utf-8") as f:
            f.write(final_content)
        print(f"成功同步: {filename}")

    except Exception as e:
        print(f"解析 {problem_url} 失败: {e}")

def main():
    print("正在获取作业列表...")
    try:
        r = fetch_url(BASE_URL)
        soup = BeautifulSoup(r.text, 'html.parser')
        # 匹配 href 格式：/hw202612/A/  （结尾没有其他字符）
        links = soup.find_all('a', href=re.compile(r'/hw202612/[A-Z]/$'))
        urls = sorted(list(set([urljoin(BASE_URL, l['href']) for l in links])))
        print(f"共发现 {len(urls)} 道题目，准备下载...")
        for url in urls:
            sync_problem(url)
            time.sleep(1)
    except Exception as e:
        print(f"作业列表获取失败: {e}")

if __name__ == "__main__":
    main()