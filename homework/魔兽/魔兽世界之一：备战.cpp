/*
描述
魔兽世界的西面是红魔军的司令部，东面是蓝魔军的司令部。两个司令部之间是依次排列的若干城市。
红司令部，City 1，City 2，……，City n，蓝司令部

两军的司令部都会制造武士。武士一共有 dragon 、ninja、iceman、lion、wolf 五种。每种武士都有编号、生命值、攻击力这三种属性。

双方的武士编号都是从1开始计算。红方制造出来的第n个武士，编号就是n。同样，蓝方制造出来的第n个武士，编号也是n。

武士在刚降生的时候有一个生命值。

在每个整点，双方的司令部中各有一个武士降生。

红方司令部按照iceman、lion、wolf、ninja、dragon的顺序循环制造武士。

蓝方司令部按照lion、dragon、ninja、iceman、wolf的顺序循环制造武士。

制造武士需要生命元。

制造一个初始生命值为m的武士，司令部中的生命元就要减少m个。

如果司令部中的生命元不足以制造某个按顺序应该制造的武士，那么司令部就试图制造下一个。如果所有武士都不能制造了，则司令部停止制造武士。

给定一个时间，和双方司令部的初始生命元数目，要求你将从0点0分开始到双方司令部停止制造武士为止的所有事件按顺序输出。
一共有两种事件，其对应的输出样例如下：

1) 武士降生
输出样例： 004 blue lion 5 born with strength 5,2 lion in red headquarter
表示在4点整，编号为5的蓝魔lion武士降生，它降生时生命值为5，降生后蓝魔司令部里共有2个lion武士。（为简单起见，不考虑单词的复数形式）注意，每制造出一个新的武士，都要输出此时司令部里共有多少个该种武士。

2) 司令部停止制造武士
输出样例： 010 red headquarter stops making warriors
表示在10点整，红方司令部停止制造武士

输出事件时：

首先按时间顺序输出；

同一时间发生的事件，先输出红司令部的，再输出蓝司令部的。

输入
第一行是一个整数，代表测试数据组数。

每组测试数据共两行。

第一行：一个整数M。其含义为， 每个司令部一开始都有M个生命元( 1 <= M <= 10000)。

第二行：五个整数，依次是 dragon 、ninja、iceman、lion、wolf 的初始生命值。它们都大于0小于等于10000。
输出
对每组测试数据，要求输出从0时0分开始，到双方司令部都停止制造武士为止的所有事件。
对每组测试数据，首先输出"Case:n" n是测试数据的编号，从1开始 。
接下来按恰当的顺序和格式输出所有事件。每个事件都以事件发生的时间开头，时间以小时为单位，有三位。
样例输入
1
20
3 4 5 6 7
样例输出
Case:1
000 red iceman 1 born with strength 5,1 iceman in red headquarter
000 blue lion 1 born with strength 6,1 lion in blue headquarter
001 red lion 2 born with strength 6,1 lion in red headquarter
001 blue dragon 2 born with strength 3,1 dragon in blue headquarter
002 red wolf 3 born with strength 7,1 wolf in red headquarter
002 blue ninja 3 born with strength 4,1 ninja in blue headquarter
003 red headquarter stops making warriors
003 blue iceman 4 born with strength 5,1 iceman in blue headquarter
004 blue headquarter stops making warriors
*/

#include <iostream>
#include <map>
using namespace std;

string red[5] = {"iceman", "lion", "wolf", "ninja", "dragon"};
string blue[5] = {"lion", "dragon", "ninja", "iceman", "wolf"};

int main() {
    map<string, int> a;
    int hp = 0;
    int t = 0;
    cin >> t;
    int m = 0;
    int r, b;
    int iii = t;
    while(iii--) {
        int nr[5] = {};
        int nb[5] = {}; 
        int lr = 0;
        int lb = 0;
        int tor = 1;
        int tob = 1;
        cin >> m; r = b = m;
        cin >> hp; a["dragon"] = hp;
        cin >> hp; a["ninja"] = hp;
        cin >> hp; a["iceman"] = hp;
        cin >> hp; a["lion"] = hp;
        cin >> hp; a["wolf"] = hp;
        cout << "Case:" << t-iii << endl;
        bool x = 1, y = 1;
        int time = 1;
        while(x || y) {
            
            int cnt = 0;
            while(cnt < 5 && x) {
                if(r >= a[red[lr]]) {
                    r -= a[red[lr]];
                    printf("%03d red %s %d born with strength %d,%d %s in red headquarter\n", 
                        time - 1, red[lr].c_str(), tor, a[red[lr]], ++nr[lr], red[lr].c_str());
                    ++lr;
                    ++tor;
                    lr %= 5;
                    break;    
                }
                else {
                    ++cnt;
                    ++lr;
                    lr %= 5;
                }
            }
            if(cnt == 5 && x) {
                printf("%03d red headquarter stops making warriors\n", time - 1);
                x = 0;
            }
            cnt = 0;
            while(cnt < 5 && y) {
                if(b >= a[blue[lb]]) {
                    b -= a[blue[lb]];
                    printf("%03d blue %s %d born with strength %d,%d %s in blue headquarter\n", 
                        time - 1, blue[lb].c_str(), tob, a[blue[lb]], ++nb[lb], blue[lb].c_str());
                    ++lb;
                    lb %= 5;
                    ++tob;
                    break;    
                }
                else {
                    ++cnt;
                    ++lb;
                    lb %= 5;
                }
            }
            if(cnt == 5 && y) {
                printf("%03d blue headquarter stops making warriors\n", time - 1);
                y = 0;
            }
            ++time;
        }
    }
    return 0;
}

/*
让AI用类重写了一遍，漂亮多了（
#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <map>

using namespace std;

// 全局配置：兵种属性
map<string, int> warriorCosts;
string redOrder[5] = {"iceman", "lion", "wolf", "ninja", "dragon"};
string blueOrder[5] = {"lion", "dragon", "ninja", "iceman", "wolf"};

class Headquarter {
private:
    string color;
    int health;
    int curIdx = 0;          // 当前准备制造的兵种下标
    int totalCount = 0;      // 总造兵数
    int counts[5] = {0};     // 每种兵的数量
    string* order;           // 指向出兵顺序数组
    bool stopped = false;

public:
    Headquarter(string c, int m, string* o) : color(c), health(m), order(o) {}

    bool isStopped() const { return stopped; }

    // 核心逻辑：尝试造兵
    void tryProduce(int time) {
        if (stopped) return;

        int searched = 0;
        while (searched < 5) {
            string name = order[curIdx];
            int cost = warriorCosts[name];

            if (health >= cost) {
                health -= cost;
                totalCount++;
                counts[curIdx]++;
                
                printf("%03d %s %s %d born with strength %d,%d %s in %s headquarter\n",
                       time, color.c_str(), name.c_str(), totalCount, cost, counts[curIdx], name.c_str(), color.c_str());
                
                curIdx = (curIdx + 1) % 5;
                return; // 造兵成功，结束本次任务
            }
            
            // 买不起，换下一个兵种试试
            curIdx = (curIdx + 1) % 5;
            searched++;
        }

        // 循环一遍都没钱买，停止生产
        stopped = true;
        printf("%03d %s headquarter stops making warriors\n", time, color.c_str());
    }
};

int main() {
    int t;
    cin >> t;
    for (int i = 1; i <= t; ++i) {
        int m;
        cin >> m;
        // 初始化兵种消耗（顺序：dragon 、ninja、iceman、lion、wolf）
        cin >> warriorCosts["dragon"] >> warriorCosts["ninja"] >> warriorCosts["iceman"] 
            >> warriorCosts["lion"] >> warriorCosts["wolf"];

        cout << "Case:" << i << endl;

        // 创建两个司令部对象
        Headquarter redHQ("red", m, redOrder);
        Headquarter blueHQ("blue", m, blueOrder);

        int time = 0;
        while (!redHQ.isStopped() || !blueHQ.isStopped()) {
            redHQ.tryProduce(time);
            blueHQ.tryProduce(time);
            time++;
        }
    }
    return 0;
}
*/