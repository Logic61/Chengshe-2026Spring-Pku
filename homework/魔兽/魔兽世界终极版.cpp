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
    virtual int getDurability() const override { return (attack > 0) ? durability : 0; }
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
    
    
    void addWeapon(int wId) {
        Weapon* w = createWeapon(wId, warriorDamage[name] / 5);
        if (w) weapons.push_back(w);
    }
public:
    int damage;
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
    virtual void counterAttack(Warrior* enemy) {
        enemy->takeDamage(counterAttack_damage());
        auto it = find_if(weapons.begin(), weapons.end(), [](Weapon* w) {
            return w->getName() == "sword" && w->getDurability() > 0;
        });
        if (it != weapons.end()) {
            (*it) ->use(); // 反击后剑会衰减攻击力
        }
    }
    virtual int counterAttack_damage() {
        auto it = find_if(weapons.begin(), weapons.end(), [](Weapon* w) {
            return w->getName() == "sword" && w->getDurability() > 0;
        });
        int counterDamage = 0.5 * damage;
        if (it != weapons.end()) {
            counterDamage += (*it)->getDamage(); // 剑的反击加成
        }
        return counterDamage;
    }
    void announce() {
            cout << color << " " << name << " " << id << " has ";
            sort(weapons.begin(), weapons.end(), [](Weapon* a, Weapon* b) {
                return a->getId() > b->getId();
            });
            weapons.erase(std::remove_if(weapons.begin(), weapons.end(),
                [](Weapon* w) {
                    if (w->getDurability() <= 0) {
                        delete w;
                        return true;
                    }
                    return false;
                }), weapons.end());
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
    int getDamage_cost() {
        auto it = find_if(weapons.begin(), weapons.end(), [](Weapon* w) {
            return w->getName() == "sword" && w->getDurability() > 0;
        });
        int dmg = this->damage;
        if (it != weapons.end()) {
            dmg += (*it)->getDamage();
            (*it)->use(); // 使用剑会衰减攻击力
        }
        return dmg;
    }
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
            hp = 0;
        }
    }
    //缴获武器
    void deleteWeapons(Warrior* defeated) {
        
        for (auto w : defeated->weapons) {
            delete w; // 销毁，防止内存泄漏
        }
        defeated->weapons.clear(); // 败者武器库清空
    }
    virtual string getName() = 0;
    virtual StealResult TrySteal(Warrior* enemy) = 0;
    virtual void onMarch() = 0;
    int getDistanceToEnemyHQ() const {
        if (color == "blue") {
            return pos; 
        } else {
            return cityNum + 1 - pos; 
        }
    }
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
        cout << "Its morale is " << fixed << setprecision(2) << morale << endl;
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
    void counterAttack(Warrior* enemy) override {
        // 忍者没有反击能力
    }
    int counterAttack_damage() override {
        return 0; // 忍者没有反击伤害
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
        if (enemy->weapons.empty()) return { "", 0, false };
        int stolenCount = 0;
        vector<Weapon*> newWeapons;
        for(auto w : enemy->weapons) {
            auto it = std::find_if(weapons.begin(), weapons.end(), [w](Weapon* wp) {
                return wp->getName() == w->getName();
            });
            if (it == weapons.end() && weapons.size() + newWeapons.size() < 3) {
                newWeapons.push_back(w);
                stolenCount++;
            }
        }
        if (stolenCount > 0) {
            for(auto w : newWeapons) {
                weapons.push_back(w);
                enemy->weapons.erase(std::remove(enemy->weapons.begin(), enemy->weapons.end(), w), enemy->weapons.end());
            }
            return { "some", stolenCount, true };
        }
        return { "", 0, false };
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
    
    int curIdx = 0;          // 当前准备制造的兵种下标
    int totalCount = 0;      // 总造兵数
    int counts[5] = {0};     // 每种兵的数量
    string* order;           // 指向出兵顺序数组
    bool stopped = false;
    

public:
    vector<Warrior*> warriors;
    int health;              // 司令部剩余生命值
    int hp_get = 0; // 本轮获得的生命元
    Headquarter(string c, int m, string* o) : color(c), health(m), order(o) {}

    bool isStopped() const { return stopped; }

    int getHealth() const { return health; }

    vector<Warrior*> to_deliver; // 用于记录本轮要发出hp的士兵

    void deliverHp() {
        sort(to_deliver.begin(), to_deliver.end(), [](Warrior* a, Warrior* b) {
            return a->getDistanceToEnemyHQ() < b->getDistanceToEnemyHQ(); // 按离敌方司令部距离升序排序
        });
        for(auto it = to_deliver.begin(); it != to_deliver.end(); ++it) {
            if(health >= 8) {
                (*it)->getHp() += 8;
                health -= 8;
            } else {
                break; // hp发完了，退出循环
            }
        }
        to_deliver.clear();
    }

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
    int hp = 0; // 城市中间的生命元数量

    void getHp() {
        hp += 10; // 每小时增加10点生命元
    }

    void Quarter_getHp(int hour, int minute,Headquarter* red, Headquarter* blue) {
        if(warrior[0] && warrior[0]->alive && !warrior[1]) {
            red->hp_get += hp;
            
            //输出样例：001:40 blue dragon 2 earned 10 elements for his headquarter
            cout << setfill('0') << setw(3) << hour << ":"
            << setfill('0') << setw(2) << minute << " " << "red" << " "
            << warrior[0]->getName() << " " << warrior[0]->getId()
            << " earned " << hp << " elements for his headquarter" << endl;
            hp = 0;
        }
        else if(warrior[1] && warrior[1]->alive && !warrior[0]) {
            blue->hp_get += hp;
            
            //输出样例：001:40 blue dragon 2 earned 10 elements for his headquarter
            cout << setfill('0') << setw(3) << hour << ":"
            << setfill('0') << setw(2) << minute << " " << "blue" << " "
            << warrior[1]->getName() << " " << warrior[1]->getId()
            << " earned " << hp << " elements for his headquarter" << endl;
            hp = 0;
        }
    }

    void shoot(int hour, int minute, City* last_city, City* next_city) {
        for(int i = 0; i < 2; ++i) {
            if(warrior[i]) {
                auto arrowIt = find_if(warrior[i]->weapons.begin(), warrior[i]->weapons.end(), [](Weapon* w) {
                    return w->getName() == "arrow" && w->getDurability() > 0;
                });
                if(arrowIt != warrior[i]->weapons.end()) {
                    // 计算箭的目标
                    City* targetCity = (i == 0) ? next_city : last_city;
                    if(targetCity->id != 0 && targetCity->id != cityNum + 1 
                        && targetCity->warrior[1 - i] && targetCity->warrior[1 - i]->alive) {
                        // 发射箭
                        (*arrowIt)->use();
                        targetCity->warrior[1 - i]->takeDamage((*arrowIt)->getDamage());
                        if(targetCity->warrior[1 - i]->alive) {
                            cout << setfill('0') << setw(3) << hour << ":"
                            << setfill('0') << setw(2) << minute << " " << (i == 0 ? "red" : "blue") 
                            << " " << warrior[i]->getName() << " " << warrior[i]->getId() 
                            << " shot"<< endl;
                        } else {
                            cout << setfill('0') << setw(3) << hour << ":"
                            << setfill('0') << setw(2) << minute << " " << (i == 0 ? "red" : "blue") 
                            << " " << warrior[i]->getName() << " " << warrior[i]->getId() 
                            << " shot and killed " << (i == 0 ? "blue" : "red") << " "
                            << targetCity->warrior[1 - i]->getName() << " "
                            << targetCity->warrior[1 - i]->getId() << endl;
                        }
                    }
                }
            }
        }
    }

    void use_bomb(int hour, int minute) {
        if(!warrior[0] || !warrior[1] || !warrior[0]->alive || !warrior[1]->alive) return; // 没有双方都活着的战士了
        int turn = (id % 2 == 1) ? 0 : 1; // 奇数城市红先，偶数城市蓝先
        if(flag == 1) turn = 0; // 红旗红先
            else if(flag == 2) turn = 1; // 蓝旗蓝先
        int damage = warrior[turn]->getDamage();
        // 检测防守方有无bomb
                auto bombIt = find_if(warrior[1 - turn]->weapons.begin(), warrior[1 - turn]->weapons.end(), [](Weapon* w) {
                    return w->getName() == "bomb" && w->getDurability() > 0;
                });
                if(bombIt != warrior[1 - turn]->weapons.end()) {
                    if(damage >= warrior[1 - turn]->getHp()) {
                        //同归于尽
                        warrior[0]->alive = 0;
                        warrior[1]->alive = 0;
                        warrior[0]->getHp() = 0;
                        warrior[1]->getHp() = 0;
                        
                        //输出样例： 000:38 blue dragon 1 used a bomb and killed red lion 7
                        cout << setfill('0') << setw(3) << hour << ":"
                        << setfill('0') << setw(2) << minute << " " << (turn == 1 ? "red" : "blue")
                        << " " << warrior[1 - turn]->getName() << " " << warrior[1 - turn]->getId()
                        << " used a bomb and killed " << (turn == 1 ? "blue" : "red") << " "
                        << warrior[turn]->getName() << " " << warrior[turn]->getId() << endl;
                        warrior[0] = warrior[1] = NULL; // 双方都死了，城市里没有人了
                        return;
                    }
                }
                //进攻方有无bomb
                auto bombIt2 = find_if(warrior[turn]->weapons.begin(), warrior[turn]->weapons.end(), [](Weapon* w) {
                    return w->getName() == "bomb" && w->getDurability() > 0;
                });
                if(bombIt2 != warrior[turn]->weapons.end()) {
                    if(damage < warrior[1 - turn]->getHp()) {
                        if(warrior[1 - turn]->counterAttack_damage() >= warrior[turn]->getHp()) {
                            //同归于尽
                            warrior[0]->alive = 0;
                            warrior[1]->alive = 0;
                            warrior[0]->getHp() = 0;
                            warrior[1]->getHp() = 0;
                            //输出样例： 000:38 blue dragon 1 used a bomb and killed red lion 7
                            cout << setfill('0') << setw(3) << hour << ":"
                            << setfill('0') << setw(2) << minute << " " << (turn == 0 ? "red" : "blue")
                            << " " << warrior[turn]->getName() << " " << warrior[turn]->getId()
                            << " used a bomb and killed " << (turn == 0 ? "blue" : "red") << " "
                            << warrior[1 - turn]->getName() << " " << warrior[1 - turn]->getId() << endl;
                            warrior[0] = warrior[1] = NULL; // 双方都死了，城市里没有人了
                        }
                    }
                }
    }

    void battle(int hour, int minute, Headquarter* red, Headquarter* blue) {
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
                    
                int damage = warrior[turn]->getDamage_cost();
                
                warrior[1 - turn]->takeDamage(damage);
                //输出样例：000:40 red iceman 1 attacked blue lion 1 in city 1 with 20 elements and force 30
                cout << setfill('0') << setw(3) << hour << ":"
                << setfill('0') << setw(2) << minute << " " << (turn == 0 ? "red" : "blue") << " " << warrior[turn]->getName()
                << " " << warrior[turn]->getId() << " attacked " << (turn == 0 ? "blue" : "red") << " "
                << warrior[1 - turn]->getName() << " " << warrior[1 - turn]->getId() << " in city " << id
                << " with " << warrior[turn]->getHp() << " elements and force " << warrior[turn]->damage << endl;

                if(warrior[1 - turn]->alive && warrior[1 - turn]->getName() != "ninja") {
                    warrior[1 - turn]->counterAttack(warrior[turn]);
                    //输出样例：001:40 blue dragon 2 fought back against red lion 2 in city 1
                    cout << setfill('0') << setw(3) << hour << ":"
                    << setfill('0') << setw(2) << minute << " " << (turn == 0 ? "blue" : "red") << " " << warrior[1 - turn]->getName()
                    << " " << warrior[1 - turn]->getId() << " fought back against " << (turn == 0 ? "red" : "blue") << " "
                    << warrior[turn]->getName() << " " << warrior[turn]->getId() << " in city " << id << endl;
                }
                else if(warrior[1 - turn]->alive && warrior[1 - turn]->getName() == "ninja") {
                    // 忍者不反击
                    winner = 2;
                }
                else {
                    // 输出样例：001:40 red lion 2 was killed in city 1
                    winner = turn; // 进攻方赢了
                    cout << setfill('0') << setw(3) << hour << ":"
                    << setfill('0') << setw(2) << minute << " " << (turn == 1 ? "red" : "blue") << " " << warrior[1 - turn]->getName()
                    << " " << warrior[1 - turn]->getId() << " was killed in city " << id << endl;
                }

                if(warrior[turn]->alive == 0) {
                    winner = 1 - turn; // 防守方赢了
                    // 输出样例：001:40 blue lion 2 was killed in city 1
                    cout << setfill('0') << setw(3) << hour << ":"
                    << setfill('0') << setw(2) << minute << " " << (turn == 0 ? "red" : "blue") << " " << warrior[turn]->getName()
                    << " " << warrior[turn]->getId() << " was killed in city " << id << endl;
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
                //给获胜士兵发生命
                if(warrior[0] && warrior[0]->alive) {
                    red->to_deliver.push_back(warrior[0]);
                }
            } else if(winner == 1) {
                wincount = (wincount <= 0) ? wincount - 1 : -1; // 蓝方赢了，蓝方连胜-1，红方连胜重置
                //给获胜士兵发生命
                if(warrior[1] && warrior[1]->alive) {
                    blue->to_deliver.push_back(warrior[1]);
                }
            }
            else {
                wincount = 0; // 平局，连胜重置
            }
            // 没杀死敌人，Lion忠诚度-lossloyalty
            if(winner == 2 && warrior[1] && warrior[1]->alive && warrior[0]->getName() == "lion") {
                Lion* l = dynamic_cast<Lion*>(warrior[0]);
                if(l) l->loyalty -= loyaltyLoss;
                if(l->loyalty <= 0) l->excape = 1; // 忠诚度降到0或以下，狮子逃跑
            }
            if(winner == 2 && warrior[0] && warrior[0]->alive && warrior[1]->getName() == "lion") {
                Lion* l = dynamic_cast<Lion*>(warrior[1]);
                if(l) l->loyalty -= loyaltyLoss;
                if(l->loyalty <= 0) l->excape = 1; // 忠诚度降到0或以下，狮子逃跑
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
            //胜者获取生命元
            if(winner == 0 && warrior[0] && warrior[0]->alive) {
                red->hp_get += hp;
                //输出样例：001:40 blue dragon 2 earned 10 elements for his headquarter
                cout << setfill('0') << setw(3) << hour << ":"
                << setfill('0') << setw(2) << minute << " " << "red " << warrior[0]->getName() << " "
                << warrior[0]->getId() << " earned " << hp << " elements for his headquarter" << endl;
                hp = 0;
            }
            else if(winner == 1 && warrior[1] && warrior[1]->alive) {
                blue->hp_get += hp;
                //输出样例：001:40 blue dragon 2 earned 10 elements for his headquarter
                cout << setfill('0') << setw(3) << hour << ":"
                << setfill('0') << setw(2) << minute << " " << "blue " << warrior[1]->getName() << " "
                << warrior[1]->getId() << " earned " << hp << " elements for his headquarter" << endl;
                hp = 0;
            }
            // 升旗判定
            if(wincount >= 2 && flag != 1) {
                flag = 1; // 升红旗
                wincount = 0; // 连胜计数器重置
                //输出样例：004:40 blue flag raised in city 4
                cout << setfill('0') << setw(3) << hour << ":"
                << setfill('0') << setw(2) << minute << " " << "red flag raised in city " << id << endl;
            } else if(wincount <= -2 && flag != 2) {
                flag = 2; // 升蓝旗
                wincount = 0; // 连胜计数器重置
                //输出样例：004:40 blue flag raised in city 4
                cout << setfill('0') << setw(3) << hour << ":"
                << setfill('0') << setw(2) << minute << " " << "blue flag raised in city " << id << endl;
            }
            //清掉指针
            if(warrior[0] && !warrior[0]->alive) {
                warrior[0] = NULL;
            }
            if(warrior[1] && !warrior[1]->alive) {
                warrior[1] = NULL;
            }
            cerr << "City " << id << " winner=" << winner << " turn=" << turn 
            << " flag=" << flag << " wincount=" << wincount 
            << " redAlive=" << (warrior[0] ? warrior[0]->alive : -1)
            << " blueAlive=" << (warrior[1] ? warrior[1]->alive : -1) << endl;
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
            cout << " reached " << (id == 0 ? "red" : "blue") << " headquarter";
        } else {
            // 行军到普通城市
            cout << " marched to city " << id;
        }
        
        cout << " with " << warrior[colorNum]->getHp()
        << " elements and force " << warrior[colorNum]->damage << endl;
    }
};
int gameOver = 0;
int marchAll(City cities[], int hour, int minute) {
    vector<Warrior*> nextRed(cityNum + 2, nullptr);
    vector<Warrior*> nextBlue(cityNum + 2, nullptr);
    vector<bool> redArrived(cityNum + 2, false);
    vector<bool> blueArrived(cityNum + 2, false);
    
    int id = -1; // 0: red takes blue HQ, 1: blue takes red HQ

    // 1. 处理已在敌方司令部的武士，留在原处不动
    if (cities[cityNum + 1].warrior[0] && cities[cityNum + 1].warrior[0]->alive)
        nextRed[cityNum + 1] = cities[cityNum + 1].warrior[0];
    if (cities[0].warrior[1] && cities[0].warrior[1]->alive)
        nextBlue[0] = cities[0].warrior[1];

    // 2. 红方移动：从西向东，但用倒序避免覆盖
    // 先处理红司令部出生的武士 (i=0)
    if (cities[0].warrior[0] && cities[0].warrior[0]->alive) {
        nextRed[1] = cities[0].warrior[0];
        redArrived[1] = true;
    }
    // 然后是城市1到cityNum-1（如果是cityNum则需特殊判断司令部）
    for (int i = 1; i <= cityNum; ++i) {
        if (cities[i].warrior[0] && cities[i].warrior[0]->alive) {
            if (i == cityNum) {
                // 进入敌方司令部
                if (cities[cityNum + 1].warrior[0]) { // 已有红武士，游戏结束
                    gameOver = 1;
                    id = 0;
                }
                nextRed[cityNum + 1] = cities[i].warrior[0];
                redArrived[cityNum + 1] = true;
            } else {
                nextRed[i + 1] = cities[i].warrior[0];
                redArrived[i + 1] = true;
            }
        }
    }

    // 3. 蓝方移动：从东向西，同样分情况
    // 先处理蓝司令部出生的武士 (i=cityNum+1)
    if (cities[cityNum + 1].warrior[1] && cities[cityNum + 1].warrior[1]->alive) {
        nextBlue[cityNum] = cities[cityNum + 1].warrior[1];
        blueArrived[cityNum] = true;
    }
    // 再处理城市cityNum到1
    for (int i = cityNum; i >= 1; --i) {
        if (cities[i].warrior[1] && cities[i].warrior[1]->alive) {
            if (i == 1) {
                // 进入敌方司令部
                if (cities[0].warrior[1]) { // 已有蓝武士，游戏结束
                    gameOver = 1;
                    id = 1;
                    if(id == 0) id = 2;
                }
                nextBlue[0] = cities[i].warrior[1];
                blueArrived[0] = true;
            } else {
                nextBlue[i - 1] = cities[i].warrior[1];
                blueArrived[i - 1] = true;
            }
        }
    }

    // 4. 更新城市数组
    for (int i = 0; i <= cityNum + 1; ++i) {
        cities[i].warrior[0] = nextRed[i];
        cities[i].warrior[1] = nextBlue[i];
        if (cities[i].warrior[0] && cities[i].warrior[0]->alive)
            cities[i].warrior[0]->onMarch();
        if (cities[i].warrior[1] && cities[i].warrior[1]->alive)
            cities[i].warrior[1]->onMarch();
    }

    // 5. 输出行军事件（按从西向东，同城市红先蓝后）
    for (int i = 0; i <= cityNum + 1; ++i) {
        if (redArrived[i]) cities[i].printMarch(hour, minute, 0);
        if (blueArrived[i]) cities[i].printMarch(hour, minute, 1);
        if (gameOver) {
            if(id == 2) {
                if(i == 0) {
                    cout << setfill('0') << setw(3) << hour << ":" << setfill('0') << setw(2) << minute << " "
                    << "red headquarter was taken" << endl;
                }
                if(i == cityNum + 1) {
                    cout << setfill('0') << setw(3) << hour << ":" << setfill('0') << setw(2) << minute << " "
                    << "blue headquarter was taken" << endl;
                }
            }
            else if ((id == 1 && i == 0) || (id == 0 && i == cityNum + 1)) {
                cout << setfill('0') << setw(3) << hour << ":" << setfill('0') << setw(2) << minute << " "
                    << (id == 1 ? "red" : "blue") << " headquarter was taken" << endl;
            }
        }
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

        gameOver = 0;
        for(int hour = 0; !gameOver; ++hour) {
            if(hour * 60 > t) break; // 超过时间限制，结束模拟
            for (int minute : {0, 5, 10, 20, 30, 35, 38, 40, 50, 55}) {
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
                else if(minute == 20) {
                    //城市产出生命元
                    test(minute);
                    for(int i = 1; i <= cityNum; ++i) {
                        cities[i].getHp();
                    }
                }
                else if(minute == 30) {
                    test(minute);
                    for(int i = 1; i <= cityNum; ++i) {
                        cities[i].Quarter_getHp(hour, minute, &redHQ, &blueHQ);
                    }
                }
                else if(minute == 35) {
                    //射箭
                    test(minute);
                    for(int i = 1; i <= cityNum; ++i) {
                        cities[i].shoot(hour, minute, &cities[i-1], &cities[i+1]);
                    }
                }
                else if(minute == 38) {
                    //bomb
                    test(minute);
                    for(int i = 1; i <= cityNum; ++i) {
                        cities[i].use_bomb(hour, minute);
                    }
                }
                else if(minute == 40) {
                    //战斗
                    test(minute);
                    for(int i = 1; i <= cityNum; ++i) {
                        if(cities[i].warrior[0] && cities[i].warrior[1]) {
                            cities[i].battle(hour, minute, &redHQ, &blueHQ);
                        }
                    }
                    //发生命
                    redHQ.deliverHp();
                    blueHQ.deliverHp();
                    redHQ.health += redHQ.hp_get;
                    blueHQ.health += blueHQ.hp_get;
                    redHQ.hp_get = 0;
                    blueHQ.hp_get = 0;
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
                    }
                    for(int i = 0; i <= cityNum + 1; ++i) {
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
