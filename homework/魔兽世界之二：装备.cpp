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