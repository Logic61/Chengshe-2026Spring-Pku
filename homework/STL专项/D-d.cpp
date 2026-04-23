/*
总时间限制:
1000ms
内存限制:
65536kB
描述
给定n个字符串（从1开始编号），每个字符串中的字符位置从0开始编号，长度为1-500，现有如下若干操作：
copy N X L：取出第N个字符串第X个字符开始的长度为L的字符串。
add S1 S2：判断S1，S2是否为0-99999之间的整数，若是则将其转化为整数做加法，若不是，则作字符串加法，返回的值为一字符串。
find S N：在第N个字符串中从左开始找寻S字符串，返回其第一次出现的位置，若没有找到，返回字符串的长度。
rfind S N：在第N个字符串中从右开始找寻S字符串，返回其第一次出现的位置，若没有找到，返回字符串的长度。
insert S N X：在第N个字符串的第X个字符位置中插入S字符串。
reset S N：将第N个字符串变为S。
print N：打印输出第N个字符串。
printall：打印输出所有字符串。
over：结束操作。
其中N，X，L可由find与rfind操作表达式构成，S，S1，S2可由copy与add操作表达式构成。
输入
第一行为一个整数n（n在1-20之间）
接下来n行为n个字符串，字符串不包含空格及操作命令等。
接下来若干行为一系列操作，直到over结束。
输出
根据操作提示输出对应字符串。
样例输入
3

329strjvc

Opadfk48

Ifjoqwoqejr

insert copy 1 find 2 1 2 2 2

print 2

reset add copy 1 find 3 1 3 copy 2 find 2 2 2 3

print 3

insert a 3 2

printall

over
样例输出
Op29adfk48

358

329strjvc

Op29adfk48

35a8
提示
推荐使用string类中的相关操作函数。
查看
提交
统计
提问
URL: http://cxsjsx.openjudge.cn/hw202610/D/
--------------------
*/

#include <iostream>
#include <vector>
#include <string>
#include <cctype>
using namespace std;

vector<string> strs(21);
int n;

bool isNum(const string &s) {
    if (s.empty() || s.length() > 5) return false;
    for (char c : s) if (!isdigit(c)) return false;
    return stoi(s) <= 99999;
}

string parse() {
    string tok;
    cin >> tok;
    
    if (tok == "copy") {
        string Ns = parse();
        string Xs = parse();
        string Ls = parse();
        int N = stoi(Ns), X = stoi(Xs), L = stoi(Ls);
        return strs[N].substr(X, L);
    }
    else if (tok == "add") {
        string s1 = parse(), s2 = parse();
        if (isNum(s1) && isNum(s2)) {
            return to_string(stoi(s1) + stoi(s2));
        } else {
            return s1 + s2;
        }
    }
    else if (tok == "find") {
        string S = parse(), Ns = parse();
        int N = stoi(Ns);
        size_t p = strs[N].find(S);
        if (p == string::npos) p = strs[N].length();
        return to_string(p);
    }
    else if (tok == "rfind") {
        string S = parse(), Ns = parse();
        int N = stoi(Ns);
        size_t p = strs[N].rfind(S);
        if (p == string::npos) p = strs[N].length();
        return to_string(p);
    }
    else {
        return tok; // 数字或普通字符串
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    cin >> n;
    for (int i = 1; i <= n; ++i)
        cin >> strs[i];
    
    string cmd;
    while (cin >> cmd) {
        if (cmd == "over") break;
        else if (cmd == "insert") {
            string S = parse();
            string Ns = parse();
            string Xs = parse();
            int N = stoi(Ns), X = stoi(Xs);
            strs[N].insert(X, S);
        }
        else if (cmd == "reset") {
            string S = parse();
            string Ns = parse();
            int N = stoi(Ns);
            strs[N] = S;
        }
        else if (cmd == "print") {
            string Ns = parse();
            cout << strs[stoi(Ns)] << '\n';
        }
        else if (cmd == "printall") {
            for (int i = 1; i <= n; ++i)
                cout << strs[i] << '\n';
        }
        // 若单独出现表达式（copy/add等）却不被命令使用，忽略，但题设不会出现。
    }
    return 0;
}