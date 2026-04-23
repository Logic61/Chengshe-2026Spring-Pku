/*
总时间限制:
1000ms
内存限制:
65536kB
描述
Lisp语言中有高阶函数的概念，即函数可以作为函数的参数，也可以作为函数的返回值。例如:
(define (square x)  (* xx))   定义了一个求x的平方的函数
(define (inc x)  (+ x1))     定义了一个求x+1的函数
(define (combine f g)

     (lambda (x) (f (+ (fx) (g x)))))
(combine f g) 返回函数k , k(x) = f( f(x)+g(x))
因此

((combine square inc) 3) 

的返回值就是169
此处
f(x) = x*x

     g(x) = x + 1
k(x) = (x*x+(x+1)) ^2
((combine square inc) 3) 即是 k(3)
因此返回值为169



用C++实现类似的combine函数模板，使得下面的程序输出结果为
169
10.75
#include <iostream>
using namespace std;
// 在此处补充你的代码
int main()
{
    auto Square = [] (double a) { return a * a; };
    auto Inc = [] (double a) { return a + 1; };
    cout << combine<decltype(Square),decltype(Inc),int>(Square,Inc)(3) << endl;
    cout << combine<decltype(Inc),decltype(Square),double>(Inc,Square)(2.5) << endl;

    return 0;
}
输入
无
输出
169
10.75
样例输入
无
样例输出
169

10.75
提示
C++函数模板实例化时，也可以通过在<>中指定类型参数所对应的具体类型来实现。
来源
Guo Wei
查看
提交
统计
提问
URL: http://cxsjsx.openjudge.cn/hw202612/B/
--------------------
*/

#include <iostream>
using namespace std;

template<class T1, class T2, class T3>
auto combine(T1 f, T2 g) {
    return [f,g](T3 x) {
        return f(f(x)+g(x));
    };
}

int main()
{
    auto Square = [] (double a) { return a * a; };
    auto Inc = [] (double a) { return a + 1; };
    cout << combine<decltype(Square),decltype(Inc),int>(Square,Inc)(3) << endl;
    cout << combine<decltype(Inc),decltype(Square),double>(Inc,Square)(2.5) << endl;

    return 0;
}