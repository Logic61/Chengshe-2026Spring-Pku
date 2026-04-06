#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <map>
#include <iomanip>
#include <algorithm>

using namespace std;

// 全局配置：兵种属性
map<string, int> warriorCosts;
map<string, int> warriorDamage;
string redOrder[5] = {"iceman", "lion", "wolf", "ninja", "dragon"};
string blueOrder[5] = {"lion", "dragon", "ninja", "iceman", "wolf"};
string Weaponname[3] = {"sword", "bomb", "arrow"};

//初始生命缘
int originalHp, cityNum, loyaltyLoss, t;

//武器
class Weapon {
protected:
    int id;
    int durability; 
public:
    Weapon(int _id) : id(_id) {}
    virtual ~Weapon() {}
    
    virtual string getName() = 0;
    int getId() const { return id; }
    virtual int getDurability() const { return durability; }
    virtual int getDamage(int userAttack) = 0; // 根据使用者攻击力计算伤害
    void use() { if(id != 0) durability--; }
};

// 具体武器子类
class Sword : public Weapon {
public:
int getDurability() const override { return durability; }
    Sword() : Weapon(0) { durability = 114514191; }
    string getName() override {return "sword";}
    int getDamage(int userAttack) override { return userAttack * 0.2; }
};

class Bomb : public Weapon {
public:
int getDurability() const override { return durability; }
    Bomb() : Weapon(1) { durability = 1; }
    string getName() override {return "bomb";}
    int getDamage(int userAttack) override { return userAttack * 0.4; }
};

class Arrow : public Weapon {
public:
int getDurability() const override { return durability; }
    Arrow() : Weapon(2) { durability = 2; }
    string getName() override {return "arrow";}
    int getDamage(int userAttack) override { return userAttack * 0.3; }
};

Weapon* createWeapon(int wId) {
    if (wId == 0) return new Sword();
    if (wId == 1) return new Bomb();
    if (wId == 2) return new Arrow();
    return NULL;
}

struct StealResult {
    string weaponName;
    int count;
    bool success; // 是否真的偷到了
};

//兵种
class Warrior {
private:
    string name;
protected:
    string color;
    int id;
    int hp;
    
    
    void addWeapon(int wId) {
        Weapon* w = createWeapon(wId);
        if (w) weapons.push_back(w);
    }
public:
    bool excape = 0; 
    vector<Weapon*> weapons;
    int order = 0;//武器用到哪个了
    bool attackable = 1;//能否攻击
    bool alive = 1;//是否还活着
    Warrior(int _id, int _hp, std::string _color, string _name) 
        : id(_id), hp(_hp), color(_color), name(_name) {}
    virtual ~Warrior() {
        for(auto w : weapons) {
            delete w;
        }
        weapons.clear();
    }
    virtual void report() = 0;
    virtual void announce() = 0;
    string getWeaponName(int idx) {
        if (idx < weapons.size()) return weapons[idx]->getName();
        return "no weapon";
    }
    void weaponSort() {
        //清理耐久为0的武器
        for (auto it = weapons.begin(); it != weapons.end(); ) {
            if ((*it)->getDurability() <= 0) {
                delete *it;            // 释放内存
                it = weapons.erase(it); // 迭代器安全删除
            } else {
                ++it;
            }
        }
        // 按照武器类型排序：剑、炸弹、箭，用过的箭排在前面(durability升序)
        sort(weapons.begin(), weapons.end(), [](Weapon* a, Weapon* b) {
            if (a->getId() == b->getId()) {
                return a->getDurability() < b->getDurability();
            }
            return a->getId() < b->getId();
        });
    }
    int getHp() const { return hp; }
    int getId() const { return id; }
    int getDamage() {
        if(weapons.empty()) {
            attackable = 0;
            return 0;
        }
        if(order >= (int)weapons.size()) {
            order = 0;
        }
        int n = (int)weapons.size();
        int checked = 0;
        while(checked < n && weapons[order]->getDurability() <= 0) {
            order = (order + 1) % n;
            checked++;
        }
        if(checked == n) {
            attackable = 0;
            return 0;
        }
        attackable = 1;
        Weapon* cur = weapons[order];
        int userAttack = warriorDamage[name]; // 使用者的攻击力
        int dmg = cur->getDamage(userAttack); // 根据使用者攻击力计算伤害
        if(cur->getName() == "bomb" && name != "ninja") {
            hp -= dmg / 2; // 炸弹反伤
        }
        cur->use();
        
        order = (order + 1) % n;
        return dmg;
    }
    void takeDamage(int dmg) { 
        hp -= dmg; 
        if (hp <= 0) {
            alive = 0;
        }
    }
    //缴获武器
    void captureWeapons(Warrior* defeated) {
        // 题目要求：按武器编号从小到大缴获，Arrow 优先拿没用过的
        // defeated->weaponSort(); // 败者先排好序，方便我们按规则拿 -> 这个排序是战斗用的，不符合缴获规则
        
        // 缴获规则：ID升序，Durability降序
        sort(defeated->weapons.begin(), defeated->weapons.end(), [](Weapon* a, Weapon* b) {
            if (a->getId() == b->getId()) {
                return a->getDurability() > b->getDurability(); // 降序
            }
            return a->getId() < b->getId();
        });

        for (auto w : defeated->weapons) {
            if (this->weapons.size() < 10) {
                this->weapons.push_back(w);
            } else {
                delete w; // 拿不下了，必须销毁，防止内存泄漏
            }
        }
        defeated->weapons.clear(); // 败者武器库清空
    }
    //快照
    int snapShot() const {
        int sum = hp;
        for (auto w : weapons) {
            sum += w->getDurability();
        }
        return sum;
    }
    virtual string getName() = 0;
    virtual StealResult TrySteal(Warrior* enemy) = 0;
    virtual void onMarch() = 0;
};

class Dragon : public Warrior {
private:
    double morale;
public:
    string getName() override { return "dragon"; }
    void announce() override {
        int numWeapons[3] = {};
        for(auto w : weapons) {
            if(w->getDurability() > 0) numWeapons[w->getId()]++;
        }
        cout << color << " " << "dragon" << " " << id << " has " 
        << numWeapons[0] << " sword " << numWeapons[1] << " bomb " 
        << numWeapons[2] << " arrow and " << hp << " elements" << endl;
    }
    Dragon(int _id, int _hp, std::string color, int left_hp) : Warrior(_id, _hp, color, "dragon") {
        morale = 1.0 * left_hp / warriorCosts["dragon"];
    
        addWeapon(_id % 3);
    }

    void report() override {

    }

    void onMarch() override {
        // 龙行军时的特殊行为
    }

    StealResult TrySteal(Warrior* enemy) override {
        return { "", 0, false }; // 龙没有偷窃能力
    }
};

class Ninja : public Warrior {
public:
    string getName() override { return "ninja"; }
    void announce() override {
        int numWeapons[3] = {};
        for(auto w : weapons) {
            if(w->getDurability() > 0) {
                numWeapons[w->getId()]++;
            }
        }
        cout << color << " " << "ninja" << " " << id << " has " 
        << numWeapons[0] << " sword " << numWeapons[1] << " bomb " 
        << numWeapons[2] << " arrow and " << hp << " elements" << endl;
    }
    Ninja(int _id, int _hp, std::string color, int left_hp) : Warrior(_id, _hp, color, "ninja") {
        addWeapon(_id % 3);
        addWeapon((_id + 1) % 3);
    }

    void report() override {
           
    }
    StealResult TrySteal(Warrior* enemy) override {
        return { "", 0, false }; // 忍者没有偷窃能力
    }
    void onMarch() override {
        // 忍者行军时的特殊行为
    }
};

class Iceman : public Warrior {
private:
    int stepCount = 0;
public:
string getName() override { return "iceman"; }
    void announce() override {
        int numWeapons[3] = {};
        for(auto w : weapons) {
            if(w->getDurability() > 0) {
                numWeapons[w->getId()]++;
            }   
        }
        cout << color << " " << "iceman" << " " << id << " has " 
        << numWeapons[0] << " sword " << numWeapons[1] << " bomb " 
        << numWeapons[2] << " arrow and " << hp << " elements" << endl;
    }
    Iceman(int _id, int _hp, std::string color, int left_hp) : Warrior(_id, _hp, color, "iceman") {
        addWeapon(_id % 3);
    }

    void report() override {
        
    }

    StealResult TrySteal(Warrior* enemy) override {
        return { "", 0, false }; // 冰人没有偷窃能力
    }
    void onMarch() override {
        stepCount++;
        hp -= hp / 10; // 每前进一步损失10%生命值
    }
};

class Lion : public Warrior {
private:
    int loyalty;
public:
    string getName() override { return "lion"; }
    void announce() override {
        int numWeapons[3] = {};
        for(auto w : weapons) {
            
            if(w->getDurability() > 0) {
                numWeapons[w->getId()]++;
            }
        }
        cout << color << " " << "lion" << " " << id << " has " 
        << numWeapons[0] << " sword " << numWeapons[1] << " bomb " 
        << numWeapons[2] << " arrow and " << hp << " elements" << endl;
    }
    Lion(int _id, int _hp, std::string color, int left_hp) : Warrior(_id, _hp, color, "lion") {
        loyalty = left_hp;
        addWeapon(_id % 3);
        if(loyalty <= 0) excape = 1;
    }

    void report() override {
        printf("Its loyalty is %d\n", loyalty);
    }
    StealResult TrySteal(Warrior* enemy) override {
        return { "", 0, false }; // 狮子没有偷窃能力
    }
    void onMarch() override {
        loyalty -= loyaltyLoss; // 每前进一步忠诚度下降
        if(loyalty <= 0) {
            excape = 1;
        }
    }
};

class Wolf : public Warrior {

public:
string getName() override { return "wolf"; }
void announce() override {
        int numWeapons[3] = {};
        for(auto w : weapons) {
            if(w->getDurability() > 0) {
                numWeapons[w->getId()]++;
            }
        }
        cout << color << " " << "wolf" << " " << id << " has " 
        << numWeapons[0] << " sword " << numWeapons[1] << " bomb " 
        << numWeapons[2] << " arrow and " << hp << " elements" << endl;
    }
    Wolf(int _id, int _hp, std::string color, int left_hp) : Warrior(_id, _hp, color, "wolf") {
    
    }

    void report() override {

    }

    StealResult TrySteal(Warrior* enemy) override {
        if (enemy->weapons.empty() || enemy->getName() == "wolf") return { "", 0, false }; // 没有武器可偷，或者敌人也是狼，无法偷窃

        // 1. 先排序：ID 升序 (0->1->2)，Durability 降序 (2->1)
        sort(enemy->weapons.begin(), enemy->weapons.end(), [](Weapon* a, Weapon* b) {
            if (a->getId() != b->getId()) return a->getId() < b->getId();
            return a->getDurability() > b->getDurability();
        });

        // 2. 确定要抢哪种武器
        int targetId = enemy->weapons.front()->getId();
        int cnt = 0;

        // 3. 尝试全部抢走
        auto it = enemy->weapons.begin();
        while (it != enemy->weapons.end() && (*it)->getId() == targetId) {
            if (this->weapons.size() < 10) {
                this->weapons.push_back(*it);
                it = enemy->weapons.erase(it);
                cnt++;
            } else {
                // 如果背包满了，后续同类武器也拿不下了，直接跳出
                break; 
            }
        }

        return { Weaponname[targetId], cnt, true };
    }

    void onMarch() override {
        // 狼行军时的特殊行为
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
    

public:
    vector<Warrior*> warriors;

    Headquarter(string c, int m, string* o) : color(c), health(m), order(o) {}

    bool isStopped() const { return stopped; }

    int getHealth() const { return health; }

    // 核心逻辑：尝试造兵
    void tryProduce(int time, int minute) {
        if (stopped) return;

        string name = order[curIdx % 5];
        int cost = warriorCosts[name];

        if (health >= cost) {
            health -= cost;
            totalCount++;
            counts[curIdx % 5]++;
                
            cout << setfill('0') << setw(3) << time << ":" << 
            setfill('0') << setw(2) << minute << " "
            << color << " " << name << " " << curIdx + 1
            << " born" << endl;
            //造兵
            warriors.push_back(create(name ,totalCount, warriorCosts[name], health, color));
            warriors.back()->report();

            curIdx = curIdx + 1;
            return; // 造兵成功，结束本次任务
        }
            
        // 买不起，不再造兵
        stopped = true;
    }

    ~Headquarter() {
        for(auto w : warriors) delete w;
    }
};

class City {
public:
    int id;
    int timing;
    Warrior* warrior[2] = {NULL}; // 0: red, 1: blue

    void battle(int hour, int minute) {
        if(warrior[0] && warrior[1]) {
            int turn = (id % 2 == 1) ? 0 : 1; // 奇数城市红先，偶数城市蓝先
            int winner = 2; //0:红赢，1:蓝赢，2:平局
            warrior[0]->weaponSort();
            warrior[1]->weaponSort();
            warrior[0]->order = 0;
            warrior[1]->order = 0;
            warrior[0]->attackable = 1;
            warrior[1]->attackable = 1;
            auto canAttack = [](Warrior* w) {
                for (auto wp : w->weapons) {
                    if (wp->getDurability() > 0) return true;
                }
                return false;
            };
            // 如果双方都没有武器，战斗视为平局
            int staleTurns = 0; // 连续回合数快照未变化
            if(warrior[0]->weapons.empty() && warrior[1]->weapons.empty()) {
                cout << setfill('0') << setw(3) << hour << ":" 
                     << setfill('0') << setw(2) << minute
                     << " both red " << warrior[0]->getName() << " " << warrior[0]->getId()
                     << " and blue " << warrior[1]->getName() << " " << warrior[1]->getId()
                     << " were alive in city " << id << endl;
                // 不要return，继续执行到yell判定
            } else {
            // 有武器的情况下才进行战斗循环
            while(warrior[0]->getHp() > 0 && warrior[1]->getHp() > 0) {
                // 检查双方是否都无法攻击
                bool canAtk0 = canAttack(warrior[0]);
                bool canAtk1 = canAttack(warrior[1]);
                if(!canAtk0 && !canAtk1) {
                    cout << setfill('0') << setw(3) << hour << ":" 
                    << setfill('0') << setw(2) << minute
                    << " both red " << warrior[0]->getName() << " " << warrior[0]->getId()
                    << " and blue " << warrior[1]->getName() << " " << warrior[1]->getId()
                    << " were alive in city " << id << endl;
                    break; // 双方都无法攻击，结束战斗
                }

                int snap = warrior[turn]->snapShot() + warrior[1 - turn]->snapShot();
                
                // 如果当前攻击方有武器，则攻击；否则跳过本轮
                if(canAttack(warrior[turn])) {
                    // 执行攻击（getDamage会处理bomb自伤）
                    int damage = warrior[turn]->getDamage();
                    warrior[1 - turn]->takeDamage(damage);
                }

                if(warrior[1 - turn]->getHp() <= 0 && warrior[turn]->getHp() <= 0) {
                    winner = 2;
                    warrior[0]->alive = 0;
                    warrior[1]->alive = 0;
                    cout << setfill('0') << setw(3) << hour << ":" 
                    << setfill('0') << setw(2) << minute
                    << " both red " << warrior[0]->getName() << " " << warrior[0]->getId()
                    << " and blue " << warrior[1]->getName() << " " << warrior[1]->getId()
                    << " died in city " << id << endl;
                    warrior[0] = warrior[1] = NULL;
                    break;
                }
                if(warrior[1 - turn]->getHp() <= 0 && warrior[turn]->getHp() > 0) {
                    warrior[1 - turn]->alive = 0;
                    cout << setfill('0') << setw(3) << hour << ":" 
                    << setfill('0') << setw(2) << minute
                    << " " << (turn == 0 ? "red" : "blue") << " " << warrior[turn]->getName() 
                    << " " << warrior[turn]->getId()
                    << " killed " << (turn == 0 ? "blue" : "red") << " "
                    << warrior[1 - turn]->getName() << " " << warrior[1 - turn]->getId()
                    << " in city " << id << " remaining " << warrior[turn]->getHp() 
                    << " elements" << endl;
                    winner = turn;
                    warrior[turn]->captureWeapons(warrior[1 - turn]);
                    warrior[1 - turn] = NULL;
                    break;
                }
                if(warrior[1 - turn]->getHp() > 0 && warrior[turn]->getHp() <= 0) {
                    warrior[turn]->alive = 0;
                    winner = 1 - turn;
                    cout << setfill('0') << setw(3) << hour << ":" 
                    << setfill('0') << setw(2) << minute
                    << " " << (winner == 0 ? "red" : "blue") << " " << warrior[winner]->getName() 
                    << " " << warrior[winner]->getId()
                    << " killed " << (winner == 0 ? "blue" : "red") << " "
                    << warrior[1 - winner]->getName() << " " << warrior[1 - winner]->getId()
                    << " in city " << id << " remaining " << warrior[winner]->getHp() 
                    << " elements" << endl;
                    warrior[1 - turn]->captureWeapons(warrior[turn]);
                    warrior[turn] = NULL;
                    break;
                }
                if(snap == warrior[0]->snapShot() + warrior[1]->snapShot()) {
                    staleTurns++;
                } else {
                    staleTurns = 0;
                }
                if(staleTurns >= 20) {
                    winner = 2; // 平局
                    cout << setfill('0') << setw(3) << hour << ":" 
                    << setfill('0') << setw(2) << minute
                    << " both red " << warrior[0]->getName() << " " << warrior[0]->getId()
                    << " and blue " << warrior[1]->getName() << " " << warrior[1]->getId()
                    << " were alive in city " << id << endl;
                    break;
                }
                turn = 1 - turn; // 轮流攻击 
            }
            } // 结束 else (有武器的战斗循环)
            // Dragon yell判定（无论是否有武器，只要还活着就会yell）
            if(warrior[0] && warrior[0]->alive && warrior[0]->getName() == "dragon") {
                cout << setfill('0') << setw(3) << hour << ":" << setfill('0') << setw(2) << minute << " "
                << "red dragon " << warrior[0]->getId() << " yelled in city " << id << endl;
            }
            if(warrior[1] && warrior[1]->alive && warrior[1]->getName() == "dragon") {
                cout << setfill('0') << setw(3) << hour << ":" << setfill('0') << setw(2) << minute << " "
                << "blue dragon " << warrior[1]->getId() << " yelled in city " << id << endl;
            }
        }
    }

    void printMarch(int hour, int minute, int colorNum) {
        if(!warrior[colorNum] || !warrior[colorNum]->alive) return;
        
        // 红方武士在0号位置（刚出生），蓝方武士在n+1号位置（刚出生），不输出行军
        if(colorNum == 0 && id == 0) return;
        if(colorNum == 1 && id == cityNum + 1) return;
        
        cout << setfill('0') << setw(3) << hour << ":" << setfill('0') << setw(2) 
        << minute << " " << (colorNum == 0 ? "red" : "blue") << " " 
        << warrior[colorNum]->getName() << " " << warrior[colorNum]->getId();
        
        // 判断是到达司令部还是普通城市
        if(id == 0 || id == cityNum + 1) {
            // 到达敌方司令部
            cout << " reached " << (id == 0 ? "red" : "blue") << " headquarter"
            << " with " << warrior[colorNum]->getHp()
        << " elements and force " << warriorDamage[warrior[colorNum]->getName()] << endl;
        cout << setfill('0') << setw(3) << hour << ":" << setfill('0') << setw(2) << minute << " "
                        << (id == 0 ? "red" : "blue") << " headquarter was taken" << endl;
        return;
        } else {
            // 行军到普通城市
            cout << " marched to city " << id;
        }
        
        cout << " with " << warrior[colorNum]->getHp()
        << " elements and force " << warriorDamage[warrior[colorNum]->getName()] << endl;
    }
};

int marchAll(City cities[], int hour, int minute) {
    // 临时存储下一时刻每个城市应该有的武士
    Warrior* nextRed[cityNum + 2] = {NULL};
    Warrior* nextBlue[cityNum + 2] = {NULL};
    bool redArrived[cityNum + 2] = {false};
    bool blueArrived[cityNum + 2] = {false};

    // 1. 物理位移：算出所有人下一步在哪
    for (int i = 0; i <= cityNum + 1; ++i) {
        if (cities[i].warrior[0] && cities[i].warrior[0]->alive) { // 红方
            if (i == cityNum + 1) {
                // 已经在敌方司令部的武士，不再移动，保持原位
                nextRed[i] = cities[i].warrior[0];
            } else {
                nextRed[i+1] = cities[i].warrior[0];
                redArrived[i + 1] = true;
            }
        }
        if (cities[i].warrior[1] && cities[i].warrior[1]->alive) { // 蓝方
            if (i == 0) {
                // 已经在敌方司令部的武士，不再移动，保持原位
                nextBlue[i] = cities[i].warrior[1];
            } else {
                nextBlue[i-1] = cities[i].warrior[1];
                blueArrived[i - 1] = true;
            }
        }
    }

    // 2. 逻辑更新：把临时指针塞回城市
    for (int i = 0; i <= cityNum + 1; ++i) {
        cities[i].warrior[0] = nextRed[i];
        cities[i].warrior[1] = nextBlue[i];
        
        // 在这里调用 Iceman,lion的机制
        if (cities[i].warrior[0] && cities[i].warrior[0]->alive) cities[i].warrior[0]->onMarch();
        if (cities[i].warrior[1] && cities[i].warrior[1]->alive) cities[i].warrior[1]->onMarch();
    }
int gameOver = 0;
    if(cities[0].warrior[1]) {
                        gameOver = true;
                    }
                    if(cities[cityNum + 1].warrior[0]) {
                        gameOver = true;
                    }
    
    // 输出行军信息：按城市从西到东依次输出，同城红方先、蓝方后
    for(int i = 0; i <= cityNum + 1; ++i) {
        if(redArrived[i]) cities[i].printMarch(hour, minute, 0);
        if(blueArrived[i]) cities[i].printMarch(hour, minute, 1);
    }

    return gameOver;
}

inline void test(int minute) {
    
}

int main() {
    int T;
    cin >> T;
    for (int i = 1; i <= T; ++i) {
        
        cin >> originalHp >> cityNum >> loyaltyLoss >> t;

        // 初始化兵种消耗（顺序：dragon 、ninja、iceman、lion、wolf）
        warriorCosts.clear();
        cin >> warriorCosts["dragon"] >> warriorCosts["ninja"] >> warriorCosts["iceman"] 
            >> warriorCosts["lion"] >> warriorCosts["wolf"];

        cin >> warriorDamage["dragon"] >> warriorDamage["ninja"] >> warriorDamage["iceman"] 
            >> warriorDamage["lion"] >> warriorDamage["wolf"];

        cout << "Case " << i << ":" << endl;

        // 创建两个司令部对象
        Headquarter redHQ("red", originalHp, redOrder);
        Headquarter blueHQ("blue", originalHp, blueOrder);
        City cities[cityNum + 2]; // 0号和n+1号是司令部
        for (int j = 0; j < cityNum + 2; ++j) {
            cities[j].id = j;
        }

        bool gameOver = false;
        for(int hour = 0; !gameOver; ++hour) {
            if(hour * 60 > t) break; // 超过时间限制，结束模拟
            for (int minute : {0, 5, 10, 35, 40, 50, 55}) {
                if (hour * 60 + minute > t) break; 
                
                if(minute == 0) {
                    test(minute);
                    int redBefore = (int)redHQ.warriors.size();
                    int blueBefore = (int)blueHQ.warriors.size();
                    redHQ.tryProduce(hour, minute);
                    blueHQ.tryProduce(hour, minute);
                    if((int)redHQ.warriors.size() > redBefore) {
                        cities[0].warrior[0] = redHQ.warriors.back();
                    }
                    if((int)blueHQ.warriors.size() > blueBefore) {
                        cities[cityNum + 1].warrior[1] = blueHQ.warriors.back();
                    }
                }
                else if(minute == 5) {
                    //lion逃跑
                    test(minute);
                    for(int i = 0; i < cityNum + 2; ++i) {
                        for(int c = 0; c < 2; ++c) {
                            if(cities[i].warrior[c] && cities[i].warrior[c]->getName() == "lion" && cities[i].warrior[c]->alive) {
                                // 位于敌人司令部的lion不会逃跑
                                if(c == 0 && i == cityNum + 1) continue; 
                                if(c == 1 && i == 0) continue; 
                                
                                if(cities[i].warrior[c]->excape) {
                                    cout << setfill('0') << setw(3) << hour << ":" << setfill('0') << setw(2) << minute << " "
                                    << (c == 0 ? "red" : "blue") << " lion " << cities[i].warrior[c]->getId() << " ran away" << endl;
                                    cities[i].warrior[c]->alive = 0; // 标记为不活跃，等会儿删除
                                    cities[i].warrior[c] = NULL;
                                }
                            }
                        }
                    }
                }
                else if(minute == 10) {
                    //武士前进
                    
                    
                    gameOver = marchAll(cities, hour, minute);
                    
                    if(gameOver) break;
                }
                else if(minute == 35) {
                    //抢武器
                    test(minute);
                    for(int i = 1; i <= cityNum; ++i) {
                        if(cities[i].warrior[0] && cities[i].warrior[1]) {
                            cities[i].warrior[0]->weaponSort();
                            cities[i].warrior[1]->weaponSort();
                            StealResult result0 = cities[i].warrior[0]->TrySteal(cities[i].warrior[1]);
                            StealResult result1 = cities[i].warrior[1]->TrySteal(cities[i].warrior[0]);
                            if(result0.success) {
                                //输出类似000:35 blue wolf 2 took 3 bomb from red dragon 2 in city 4
                                cout << setfill('0') << setw(3) << hour << ":" << setfill('0') << setw(2) << minute << " "
                                << "red " << cities[i].warrior[0]->getName() << " " << cities[i].warrior[0]->getId() << " took " << result0.count << " " << result0.weaponName << " from blue " << cities[i].warrior[1]->getName() << " "
                                << cities[i].warrior[1]->getId() << " in city " << i << endl;
                            }
                            if(result1.success) {
                                //输出类似000:35 blue wolf 2 took 3 bomb from red dragon 2 in city 4
                                cout << setfill('0') << setw(3) << hour << ":" << setfill('0') << setw(2) << minute << " "
                                << "blue " << cities[i].warrior[1]->getName() << " " << cities[i].warrior[1]->getId() << " took " << result1.count << " " << result1.weaponName << " from red " << cities[i].warrior[0]->getName() << " "
                                << cities[i].warrior[0]->getId() << " in city " << i << endl;
                            }
                        }
                    }
                }
                else if(minute == 40) {
                    //战斗
                    test(minute);
                    for(int i = 1; i <= cityNum; ++i) {
                        if(cities[i].warrior[0] && cities[i].warrior[1]) {
                            cities[i].battle(hour, minute);
                        }
                    }
                }
                else if(minute == 50) {
                    //司令部报告生命
                    test(minute);
                    cout << setfill('0') << setw(3) << hour << ":" << setfill('0') << setw(2) << minute << " "
                    << redHQ.getHealth() << " elements in red headquarter" << endl;
                    cout << setfill('0') << setw(3) << hour << ":" << setfill('0') << setw(2) << minute << " "
                    << blueHQ.getHealth() << " elements in blue headquarter" << endl;
                }
                else if(minute == 55) {
                    test(minute);
                    //武士报告生命值、攻击力、武器 - 按位置从西向东输出
                    for(int i = 0; i <= cityNum + 1; ++i) {
                        // 先输出红方武士
                        if(cities[i].warrior[0] && cities[i].warrior[0]->alive) {
                            cout << setfill('0') << setw(3) << hour << ":" << setfill('0') << setw(2) << minute << " ";
                            cities[i].warrior[0]->announce();
                        }
                        // 再输出蓝方武士
                        if(cities[i].warrior[1] && cities[i].warrior[1]->alive) {
                            cout << setfill('0') << setw(3) << hour << ":" << setfill('0') << setw(2) << minute << " ";
                            cities[i].warrior[1]->announce();
                        }
                    }
                }
            }
            // 如果游戏结束，立即退出小时循环
            if(gameOver) break;
        }
    }
    return 0;
}
