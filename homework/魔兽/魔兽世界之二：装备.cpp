#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <map>
#include <iomanip>

using namespace std;

// 全局配置：兵种属性
map<string, int> warriorCosts;
string redOrder[5] = {"iceman", "lion", "wolf", "ninja", "dragon"};
string blueOrder[5] = {"lion", "dragon", "ninja", "iceman", "wolf"};

//武器
class Weapon {
protected:
    int id;
    //int damage;
    // int durability; 
public:
    Weapon(int _id) : id(_id) {}
    virtual ~Weapon() {}
    
    virtual string getName() = 0;
    int getId() const { return id; }
};

// 具体武器子类
class Sword : public Weapon {
public:
    Sword() : Weapon(0) {}
    string getName() override {return "sword";}
};

class Bomb : public Weapon {
public:
    Bomb() : Weapon(1) {}
    string getName() override {return "bomb";}
};

class Arrow : public Weapon {
public:
    Arrow() : Weapon(2) {}
    string getName() override {return "arrow";}
};

Weapon* createWeapon(int wId) {
    if (wId == 0) return new Sword();
    if (wId == 1) return new Bomb();
    if (wId == 2) return new Arrow();
    return NULL;
}

//兵种
class Warrior {
protected:
    string color;
    int id;
    int hp;
    vector<Weapon*> weapons;
    void addWeapon(int wId) {
        Weapon* w = createWeapon(wId);
        if (w) weapons.push_back(w);
    }
public:
    Warrior(int _id, int _hp, std::string _color) 
        : id(_id), hp(_hp), color(_color) {}
    virtual ~Warrior() {
        for(auto w : weapons) {
            delete w;
        }
        weapons.clear();
    }
    virtual void report() = 0;
    string getWeaponName(int idx) {
        if (idx < weapons.size()) return weapons[idx]->getName();
        return "no weapon";
    }
};

class Dragon : public Warrior {
private:
    double morale;
public:
    Dragon(int _id, int _hp, std::string color, int left_hp) : Warrior(_id, _hp, color) {
        morale = 1.0 * left_hp / warriorCosts["dragon"];
    
        addWeapon(_id % 3);
    }

    void report() override {
        cout << "It has a " << getWeaponName(0) << ",and it's morale is " << 
        fixed << setprecision(2) << morale << endl;
    }
};

class Ninja : public Warrior {
public:
    Ninja(int _id, int _hp, std::string color, int left_hp) : Warrior(_id, _hp, color) {
        addWeapon(_id % 3);
        addWeapon((_id + 1) % 3);
    }

    void report() override {
        cout << "It has a " << getWeaponName(0) << " and a " << getWeaponName(1) << endl;
    }
};

class Iceman : public Warrior {
public:
    Iceman(int _id, int _hp, std::string color, int left_hp) : Warrior(_id, _hp, color) {
        addWeapon(_id % 3);
    }

    void report() override {
        cout << "It has a " << getWeaponName(0) << endl;
    }
};

class Lion : public Warrior {
private:
    int loyalty;
public:
    Lion(int _id, int _hp, std::string color, int left_hp) : Warrior(_id, _hp, color) {
        loyalty = left_hp;
    }

    void report() override {
        printf("It's loyalty is %d\n", loyalty);
    }
};

class Wolf : public Warrior {

public:
    Wolf(int _id, int _hp, std::string color, int left_hp) : Warrior(_id, _hp, color) {
    
    }

    void report() override {

    }
};

Warrior* create(string name, int _id, int _hp, int left_hp, string color) {
        if(name == "dragon") return new Dragon(_id, _hp, color, left_hp);
        if(name == "ninja") return new Ninja(_id, _hp, color, left_hp);
        if(name == "lion") return new Lion(_id, _hp, color, left_hp);
        if(name == "wolf") return new Wolf(_id, _hp, color, left_hp);
        if(name == "iceman") return new Iceman(_id, _hp, color, left_hp);
        return NULL;
    }

class Headquarter {
private:
    string color;
    int health;              // 司令部剩余生命值
    int curIdx = 0;          // 当前准备制造的兵种下标
    int totalCount = 0;      // 总造兵数
    int counts[5] = {0};     // 每种兵的数量
    string* order;           // 指向出兵顺序数组
    bool stopped = false;
    vector<Warrior*> warriors;

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
                
                cout << setfill('0') << setw(3) << time << " " 
                << color << " " << name << " " << totalCount 
                << " born with strength " << cost << "," << counts[curIdx] 
                << " " << name << " in " << color << " headquarter" << endl;
                //造兵，暂无hp，记作0
                warriors.push_back(create(name ,totalCount, 0, health, color));
                warriors.back()->report();

                curIdx = (curIdx + 1) % 5;
                return; // 造兵成功，结束本次任务
            }
            
            // 买不起，换下一个兵种试试
            curIdx = (curIdx + 1) % 5;
            searched++;
        }
 
        // 循环一遍都没钱买，停止生产
        stopped = true;
        cout << setfill('0') << setw(3) << time << " " 
         << color << " headquarter stops making warriors" << endl;
    }

    ~Headquarter() {
        for(auto w : warriors) delete w;
    }
};

int main() {
    int t;
    cin >> t;
    for (int i = 1; i <= t; ++i) {
        int m;
        cin >> m;
        // 初始化兵种消耗（顺序：dragon 、ninja、iceman、lion、wolf）
        warriorCosts.clear();
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