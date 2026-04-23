#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <map>
#include <iomanip>
#include <algorithm>
#include <set>

using namespace std;

// 全局配置：兵种属性
map<string, int> warriorCosts;
map<string, int> warriorDamage;
string redOrder[5] = {"iceman", "lion", "wolf", "ninja", "dragon"};
string blueOrder[5] = {"lion", "dragon", "ninja", "iceman", "wolf"};
string Weaponname[3] = {"sword", "bomb", "arrow"};

//初始生命缘
int originalHp, cityNum, loyaltyLoss, t;
int arrow_hit;

//武器
class Weapon {
protected:
    int id;
    int durability; 
    
public:
    int attack;
    Weapon(int _id, int _attack) : id(_id), attack(_attack) {}
    virtual ~Weapon() {}
    
    virtual string getName() = 0;
    int getId() const { return id; }
    virtual int getDurability() const { return durability; }
    virtual void use() { if(id != 0) durability--; }
    virtual int getDamage() { return attack; }
    virtual bool announce() = 0;
};

// 具体武器子类
class Sword : public Weapon {
public:
int getDurability() const override { return durability; }
    Sword(int _attack) : Weapon(0, _attack) { durability = 114514191;}
    string getName() override {return "sword";}
    void use() override {
        attack *= 0.8; // 每次使用攻击力衰减20%
        if(attack == 0) durability = 0; // 攻击力为0时，耐久也视为0
    }
    bool announce() override {
        cout << getName() << "(" << attack << ")";
        return true;
    }
};

class Bomb : public Weapon {
public:
int getDurability() const override { return durability; }
    Bomb(int _attack) : Weapon(1, _attack) { durability = 1; }
    string getName() override {return "bomb";}
    bool announce() override {
        cout << getName();
        return true;
    }
};

class Arrow : public Weapon {
public:
int getDurability() const override { return durability; }
    Arrow(int _attack) : Weapon(2, _attack) { durability = 3; }
    string getName() override {return "arrow";}
    int getDamage() override { return arrow_hit; }
    bool announce() override {
        cout << getName() << "(" << durability << ")";
        return true;
    }
};

Weapon* createWeapon(int wId, int attack) {
    if (wId == 0) {
        if(attack == 0) return NULL; // 攻击力为0的剑不生成
        return new Sword(attack);
    }
    if (wId == 1) return new Bomb(attack);
    if (wId == 2) return new Arrow(attack);
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
    int damage;
    
    void addWeapon(int wId) {
        Weapon* w = createWeapon(wId, 0.2 * warriorDamage[name]);
        if (w) weapons.push_back(w);
    }
public:
    int pos;
    bool excape = 0; 
    vector<Weapon*> weapons;
    bool alive = 1;//是否还活着
    Warrior(int _id, int _hp, std::string _color, string _name) 
        : id(_id), hp(_hp), color(_color), name(_name), damage(warriorDamage[_name]) {}
    virtual ~Warrior() {
        for(auto w : weapons) {
            delete w;
        }
        weapons.clear();
    }
    virtual void report() = 0;
    void announce() {
            cout << color << " " << name << " " << id << " has ";
            sort(weapons.begin(), weapons.end(), [](Weapon* a, Weapon* b) {
                if (a->getId() != b->getId()) return a->getId() > b->getId();
            });
            for(size_t i = 0; i < weapons.size(); ++i) {
                if(weapons[i]->getDurability() <= 0) {
                    delete weapons[i]; // 释放内存
                    weapons.erase(weapons.begin() + i);
                    --i; // 调整索引，继续检查下一个
                }
            }
            if(weapons.empty()) {
                cout << "no weapon" << endl;
            } else {
                for(size_t i = 0; i < weapons.size(); ++i) {
                    if(weapons[i]->getDurability() > 0) {
                        weapons[i]->announce();
                        if(i != weapons.size() - 1) cout << ",";
                    }
                }
                cout << endl;
            }
    }
    string getWeaponName(int idx) {
        if (idx < weapons.size()) return weapons[idx]->getName();
        return "no weapon";
    }
    virtual double getMorale() { return 0.0; } // 默认士气为0，只有龙会重写这个方法
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
    int & getHp() { return this->hp; }
    int getId() const { return id; }
    int getDamage() {
        auto it = find_if(weapons.begin(), weapons.end(), [](Weapon* w) {
            return w->getName() == "sword" && w->getDurability() > 0;
        });
        int dmg = this->damage;
        if (it != weapons.end()) {
            dmg += (*it)->getDamage();
        }
        return dmg;
    }
    void takeDamage(int dmg) { 
        hp -= dmg; 
        if (hp <= 0) {
            alive = 0;
        }
    }
    //缴获武器
    void deleteWeapons(Warrior* defeated) {
        
        for (auto w : defeated->weapons) {
            delete w; // 销毁，防止内存泄漏
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
    
public:
    string getName() override { return "dragon"; }
    double morale;
    double getMorale() override { return morale; }
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
        if(stepCount % 2 == 0) {
            hp = max(1, hp - 9); // 每前进两步，生命值减少9点，但至少为1
            weapons[0]->attack += 20; // 每前进两步，攻击力增加20点
            damage += 20;
        }
    }
};

class Lion : public Warrior {
private:
    
public:
    string getName() override { return "lion"; }
    int loyalty;
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
        if(loyalty <= 0 && pos != (color == "blue" ? 0 : cityNum + 1)) {
            excape = 1;
        }
    }
};

class Wolf : public Warrior {

public:
string getName() override { return "wolf"; }
    Wolf(int _id, int _hp, std::string color, int left_hp) : Warrior(_id, _hp, color, "wolf") {
    
    }

    void report() override {

    }

    StealResult TrySteal(Warrior* enemy) override {
        if (enemy->weapons.empty() || this->weapons.size() == 3) return { "", 0, false }; // 没有武器可偷 or 武器满了

        //自己已有的武器就不缴获了。被缴获的武器当然不能算新的，已经被用到什么样了，就是什么样的。
        for(auto w : enemy->weapons) {
            auto it = std::find(weapons.begin(), weapons.end(), w);
            if (it != weapons.end()) {
                return { "", 0, false }; // 已经有这个武器了，不偷
            }
            else {
                if (weapons.size() < 3) {
                    weapons.push_back(w); // 偷到新武器
                    enemy->weapons.erase(std::remove(enemy->weapons.begin(), enemy->weapons.end(), w), enemy->weapons.end()); // 从敌人武器库移除
                    return { w->getName(), 1, true };
                } else {
                    return { "", 0, false }; // 武器满了，不能偷
                }
            }
        }

        return { 0, 0, true }; //历史遗留
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
            
        // 买不起，等
    }

    ~Headquarter() {
        for(auto w : warriors) delete w;
    }
};

class City {
public:
    int id;
    int timing;
    int flag = 0; // 0:无旗帜，1:红旗，2:蓝旗
    int wincount = 0; // 连续获胜次数，达到2次后会升红旗，-2次后会升蓝旗，然后归零
    Warrior* warrior[2] = {NULL}; // 0: red, 1: blue

    void battle(int hour, int minute) {
        if(warrior[0] && warrior[1]) {
            // 记录战前生命值
            int initialHp[2] = {warrior[0]->getHp(), warrior[1]->getHp()};
            warrior[0]->pos = id;
            warrior[1]->pos = id;
            int turn = (id % 2 == 1) ? 0 : 1; // 奇数城市红先，偶数城市蓝先
            if(flag == 1) turn = 0; // 红旗红先
            else if(flag == 2) turn = 1; // 蓝旗蓝先
            int winner = 2; //0:红赢，1:蓝赢，2:平局
            warrior[0]->weaponSort();
            warrior[1]->weaponSort();
                if(warrior[0]->alive == 0 && warrior[1]->alive == 1) {
                    winner = 1;
                }
                else if(warrior[0]->alive == 1 && warrior[1]->alive == 0) {
                    winner = 0;
                }
                else if(warrior[0]->getHp() > 0 && warrior[1]->getHp() > 0) {
                    
                    int damage = warrior[turn]->getDamage();
                    warrior[1 - turn]->takeDamage(damage);

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
                        warrior[turn]->deleteWeapons(warrior[1 - turn]);
                        warrior[1 - turn] = NULL;
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
                        warrior[1 - turn]->deleteWeapons(warrior[turn]);
                        warrior[turn] = NULL;
                    }

            } 
            //dragon每取得一次战斗的胜利(敌人被杀死)，士气就会增加0.2
            if(winner == 0 && warrior[0] && warrior[0]->alive && warrior[0]->getName() == "dragon") {
                Dragon* d = dynamic_cast<Dragon*>(warrior[0]);
                if(d) d->morale += 0.2;
            }
            if(winner == 1 && warrior[1] && warrior[1]->alive && warrior[1]->getName() == "dragon") {
                Dragon* d = dynamic_cast<Dragon*>(warrior[1]);
                if(d) d->morale += 0.2;
            }
            //dragon每经历一次未能获胜的战斗，士气值就会减少0.2。
            if(winner == 2 && warrior[0] && warrior[0]->alive && warrior[0]->getName() == "dragon") {
                Dragon* d = dynamic_cast<Dragon*>(warrior[0]);
                if(d) d->morale -= 0.2;
            }
            if(winner == 2 && warrior[1] && warrior[1]->alive && warrior[1]->getName() == "dragon") {
                Dragon* d = dynamic_cast<Dragon*>(warrior[1]);
                if(d) d->morale -= 0.2;
            }
            // Dragon yell判定（主动进攻，活着，morale>0.8，欢呼）
            if(turn == 0 && warrior[0]->alive && warrior[0]->getName() == "dragon" && warrior[0]->getMorale() > 0.8) {
                cout << setfill('0') << setw(3) << hour << ":" << setfill('0') << setw(2) << minute << " "
                << "red dragon " << warrior[0]->getId() << " yelled in city " << id << endl;
            }
            if(turn == 1 && warrior[1]->alive && warrior[1]->getName() == "dragon" && warrior[1]->getMorale() > 0.8) {
                cout << setfill('0') << setw(3) << hour << ":" << setfill('0') << setw(2) << minute << " "
                << "blue dragon " << warrior[1]->getId() << " yelled in city " << id << endl;
            }
            if(winner == 0) {
                wincount = (wincount >= 0) ? wincount + 1 : 1; // 红方赢了，红方连胜+1，蓝方连胜重置
            } else if(winner == 1) {
                wincount = (wincount <= 0) ? wincount - 1 : -1; // 蓝方赢了，蓝方连胜-1，红方连胜重置
            }
            // 没杀死敌人，Lion忠诚度-lossloyalty
            if(winner == 2 && warrior[1] && warrior[1]->alive && warrior[0]->getName() == "lion") {
                Lion* l = dynamic_cast<Lion*>(warrior[0]);
                if(l) l->loyalty -= loyaltyLoss;
            }
            if(winner == 2 && warrior[0] && warrior[0]->alive && warrior[1]->getName() == "lion") {
                Lion* l = dynamic_cast<Lion*>(warrior[1]);
                if(l) l->loyalty -= loyaltyLoss;
            }
            //杀死敌人，狼人偷武器
            if(winner == 0 && warrior[0] && warrior[0]->alive && warrior[0]->getName() == "wolf") {
                Wolf* w = dynamic_cast<Wolf*>(warrior[0]);
                if(w) {
                    w->TrySteal(warrior[1]);
                }
            }
            if(winner == 1 && warrior[1] && warrior[1]->alive && warrior[1]->getName() == "wolf") {
                Wolf* w = dynamic_cast<Wolf*>(warrior[1]);
                if(w) {
                    w->TrySteal(warrior[0]);
                }
            }
            // lion如果战死，战前生命值转移给对手
            if(winner == 0 && warrior[0] && warrior[0]->alive && warrior[1] && warrior[1]->getName() == "lion") {
                warrior[0]->getHp() += initialHp[1];
            }
            if(winner == 1 && warrior[1] && warrior[1]->alive && warrior[0] && warrior[0]->getName() == "lion") {
                warrior[1]->getHp() += initialHp[0];
            }
            // 升旗判定
            if(wincount >= 2 && flag != 1) {
                flag = 1; // 升红旗
                wincount = 0; // 连胜计数器重置
            } else if(wincount <= -2 && flag != 2) {
                flag = 2; // 升蓝旗
                wincount = 0; // 连胜计数器重置
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
    //cout << "----- Minute " << minute << " -----" << endl;
}

int main() {
    int T;
    cin >> T;
    for (int i = 1; i <= T; ++i) {
        
        cin >> originalHp >> cityNum >> arrow_hit >> loyaltyLoss >> t;

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
