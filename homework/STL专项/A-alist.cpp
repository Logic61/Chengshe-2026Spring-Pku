/*
总时间限制:
4000ms
内存限制:
65536kB
描述
写一个程序完成以下命令：
new id ——新建一个指定编号为id的序列(id < 10000)
add id num——向编号为id的序列加入整数
num merge id1 id2——如果id1等于id2,不做任何事，否则归并序列id1和id2中的数，并将id2清空
unique id——去掉序列id中重复的元素
out id ——从小到大输出编号为id的序列中的元素，以空格隔开
输入
第一行一个数n，表示有多少个命令( n＜＝２０００００)。以后n行每行一个命令。
输出
按题目要求输出。
样例输入
16

new 1

new 2

add 1 1

add 1 2

add 1 3

add 2 1

add 2 2

add 2 3

add 2 4

out 1

out 2

merge 1 2

out 1

out 2

unique 1

out 1
样例输出
1 2 3 

1 2 3 4

1 1 2 2 3 3 4



1 2 3 4
查看
提交
统计
提问
URL: http://cxsjsx.openjudge.cn/hw202610/A/
--------------------
*/

#include <iostream>
#include <set>
#include <map>
#include <algorithm>
using namespace std;

map<int, multiset<int> > all; 

void add(int id, int num) {
    all[id].insert(num);
}

void merge(int a, int b) {
    if(a == b) return;
    else {
        all[a].merge(all[b]);
        all[b].clear();
    }
}

void unique(int id) {
    auto& ms = all[id];
    for(auto it = ms.begin(); it != ms.end(); ) {
        auto next_it = ms.upper_bound(*it);
        ms.erase(next(it), next_it);
        it = next_it;
    }
}

void out(int id) {
    for(auto num : all[id]) cout << num << " ";
    cout << endl;
}

int main() {
    int n;
    cin >> n;
    while(n--) {
        string order;
        cin >> order;
        if(order == "new") {
            int id;
            cin >> id;
            continue;
        }
        else if(order == "add") {
            int id, num;
            cin >> id >> num;
            add(id, num);
        }
        else if(order == "merge") {
            int id1, id2;
            cin >> id1 >> id2;
            merge(id1, id2);
        }
        else if(order == "unique") {
            int id;
            cin >> id;
            unique(id);
        }
        else {
            int id;
            cin >> id;
            out(id);
        }
    }
    return 0;
}
