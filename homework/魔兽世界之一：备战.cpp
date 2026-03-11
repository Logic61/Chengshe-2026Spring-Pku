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