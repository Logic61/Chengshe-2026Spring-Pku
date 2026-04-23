/*
总时间限制:
2500ms
内存限制:
131072kB
描述
我们定义一个正整数a比正整数b优先的含义是：
*a的质因数数目（不包括自身）比b的质因数数目多；
*当两者质因数数目相等时，数值较大者优先级高。
现在给定一个容器，初始元素数目为0，之后每次往里面添加10个元素，每次添加之后，要求输出优先级最高与最低的元素，并把该两元素从容器中删除。
输入
第一行: num (添加元素次数，num <= 30)
下面10*num行，每行一个正整数n（n < 10000000).
输出
每次输入10个整数后，输出容器中优先级最高与最低的元素，两者用空格间隔。
样例输入
1
10 7 66 4 5 30 91 100 8 9
样例输出
66 5
查看
提交
统计
提问
URL: http://cxsjsx.openjudge.cn/hw202610/G/
--------------------
*/

#include <iostream>
#include <set>
using namespace std;

bool prime[10000001] = {};
int pf[10000001]{};

class cmp {
public:
    bool operator()(int a, int b) const {
        int ca = pf[a];
        int cb = pf[b];
        if(ca != cb) return ca < cb;
        return a < b;
    }
};

int main() {
    for(int i = 2; i < 10000000; ++i) {
        if(prime[i] == 0) {
            int ltp = i * 2;
            while(ltp < 10000000) {
                prime[ltp] = 1;
                pf[ltp] += 1;
                ltp += i;
            }
        }
    }

    multiset<int, cmp> ms;

    int n;
    cin >> n;
    while(n--) {
        int tmp;
        for(int i = 1; i <= 10; ++i) {
            cin >> tmp;
            ms.insert(tmp);
        }
        auto it_high = prev(ms.end()); 
        auto it_low = ms.begin();      
        cout << *it_high << " " << *it_low << endl;
        ms.erase(it_high);
        ms.erase(it_low);
    }

    return 0;
}
