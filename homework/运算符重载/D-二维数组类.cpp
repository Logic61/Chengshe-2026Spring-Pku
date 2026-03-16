/*
描述
写一个二维数组类 Array2,使得下面程序的输出结果是：

0,1,2,3,

4,5,6,7,

8,9,10,11,

next

0,1,2,3,

4,5,6,7,

8,9,10,11,

程序：

#include <iostream>
#include <cstring>
using namespace std;

class Array2 {
// 在此处补充你的代码
};

int main() {
    Array2 a(3,4);
    int i,j;
    for(  i = 0;i < 3; ++i )
        for(  j = 0; j < 4; j ++ )
            a[i][j] = i * 4 + j;
    for(  i = 0;i < 3; ++i ) {
        for(  j = 0; j < 4; j ++ ) {
            cout << a(i,j) << ",";
        }
        cout << endl;
    }
    cout << "next" << endl;
    Array2 b;     b = a;
    for(  i = 0;i < 3; ++i ) {
        for(  j = 0; j < 4; j ++ ) {
            cout << b[i][j] << ",";
        }
        cout << endl;
    }
    return 0;
}
输入
无
输出
0,1,2,3,
4,5,6,7,
8,9,10,11,
next
0,1,2,3,
4,5,6,7,
8,9,10,11,
样例输入
None
样例输出
0,1,2,3,
4,5,6,7,
8,9,10,11,
next
0,1,2,3,
4,5,6,7,
8,9,10,11,
*/

#include <iostream>
#include <cstring>
using namespace std;

class Array2 {
private:
    int rows, cols;
    int ** data;

public:
    Array2(int i, int j) : rows(i), cols(j) {
        data = new int*[i];
        for(int k = 0; k < i; ++k) data[k] = new int[j]{0};
    }
    class proxy {
        int* row_ptr;
    public:
        proxy(int* ptr) : row_ptr(ptr) {}
        int& operator[](int col) {
            return row_ptr[col];
        }
    };

    proxy operator[](int row) {
        return proxy(data[row]);
    }

    ~Array2() {
        for (int i = 0; i < rows; ++i) {
            delete[] data[i]; 
            data[i] = NULL; 
        }

        delete[] data;
    }

    int operator()(int x, int y) {
        return data[x][y];
    }

    Array2() {
        data = NULL;
        cols = rows = 0;
    }

    Array2(const Array2 & a) : rows(a.rows), cols(a.cols) {
        data = new int*[a.rows];
        for(int i = 0; i < rows; ++i) {
            data[i] = new int[a.cols];
            for(int j = 0; j < cols; ++j) {
                data[i][j] = a.data[i][j];
            }
        }
    }

    Array2& operator=(const Array2& a) {
        if (this == &a) return *this; 

        if (data != NULL) {
            for (int i = 0; i < rows; ++i) delete[] data[i];
            delete[] data;
        }

        rows = a.rows;
        cols = a.cols;

        data = new int*[rows];
        for (int i = 0; i < rows; ++i) {
            data[i] = new int[cols];
            for (int j = 0; j < cols; ++j) {
                data[i][j] = a.data[i][j];
            }
        }
        return *this; 
    }
};

int main() {
    Array2 a(3,4);
    int i,j;
    for(  i = 0;i < 3; ++i )
        for(  j = 0; j < 4; j ++ )
            a[i][j] = i * 4 + j;
    for(  i = 0;i < 3; ++i ) {
        for(  j = 0; j < 4; j ++ ) {
            cout << a(i,j) << ",";
        }
        cout << endl;
    }
    cout << "next" << endl;
    Array2 b;     b = a;
    for(  i = 0;i < 3; ++i ) {
        for(  j = 0; j < 4; j ++ ) {
            cout << b[i][j] << ",";
        }
        cout << endl;
    }
    return 0;
}