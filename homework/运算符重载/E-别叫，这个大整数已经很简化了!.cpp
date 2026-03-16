/*
描述
程序填空，输出指定结果

#include <iostream> 
#include <cstring> 
#include <cstdlib> 
#include <cstdio> 
using namespace std;
const int MAX = 110; 
class CHugeInt {
// 在此处补充你的代码
};
int  main() 
{ 
	char s[210];
	int n;

	while (cin >> s >> n) {
		CHugeInt a(s);
		CHugeInt b(n);

		cout << a + b << endl;
		cout << n + a << endl;
		cout << a + n << endl;
		b += n;
		cout  << ++ b << endl;
		cout << b++ << endl;
		cout << b << endl;
	}
	return 0;
}
输入
多组数据，每组数据是两个非负整数s和 n。s最多可能200位， n用int能表示
输出
对每组数据，输出6行，内容对应程序中6个输出语句
样例输入
99999999999999999999999999888888888888888812345678901234567789 12
6 6
样例输出
99999999999999999999999999888888888888888812345678901234567801
99999999999999999999999999888888888888888812345678901234567801
99999999999999999999999999888888888888888812345678901234567801
25
25
26
12
12
12
13
13
14
*/

#include <iostream> 
#include <cstring> 
#include <cstdlib> 
#include <cstdio> 
using namespace std;
const int MAX = 110; 
class CHugeInt {
private:
    int digit[210]{};
    int len = 0;
public:
    CHugeInt(int n) {
        int ltp = 200;
        while(n != 0) {
            digit[ltp] = n % 10;
            n /= 10;
            ++len;
            --ltp;
        }
    }

    CHugeInt(const char * c) {
        memset(digit, 0, sizeof(digit));
        len = strlen(c);
        for(int i = 0; i < len; ++i) {
            digit[200 - i] = c[len - i - 1] - '0';
        }
    }

    CHugeInt operator+(const CHugeInt & b) const {
        CHugeInt res(0);
        for(int i = 200; i >= 1; --i) {
            res.digit[i] += digit[i] + b.digit[i];
            if(res.digit[i] >= 10) {
                res.digit[i] %= 10;
                res.digit[i - 1]++;
            }
        }
        return res;
    }

    CHugeInt operator+(int n) {
        return *this + CHugeInt(n);
    }

    friend CHugeInt operator+(int n, const CHugeInt & b) {
        return CHugeInt(n) + b;
    }

    friend std::ostream & operator<<(std::ostream & os, const CHugeInt & b) {
        bool flag = 0;
        for(int i = 0; i <= 200; ++i) {
            if(!flag && b.digit[i] != 0) flag = 1;
            if(!flag) continue;
            else os << b.digit[i];
        }
        if(!flag) os << 0;
        return os;
    }

    void operator+=(int n) {
        *this = *this + n;
    }

    CHugeInt & operator++() {
        *this += 1 ;
        return *this;
    }

    CHugeInt operator++(int) {
        CHugeInt tmp = *this;
        *this += 1;
        return tmp;
    }
};
int  main() 
{ 
	char s[210];
	int n;

	while (cin >> s >> n) {
		CHugeInt a(s);
		CHugeInt b(n);

		cout << a + b << endl;
		cout << n + a << endl;
		cout << a + n << endl;
		b += n;
		cout  << ++ b << endl;
		cout << b++ << endl;
		cout << b << endl;
	}
	return 0;
}