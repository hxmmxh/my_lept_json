#include <iostream>
#include <string.h>

using namespace std;



class A
{
    public:
    A()
    {
        cout << "init A" << endl;
    }
    A(const A&)
    {
        cout << "copy A" << endl;
    }
    ~A()
    {
        cout << "free A" << endl;
    }
};

void f(A& a)
{
    cout << "f" << endl;
    return;
}

void ff(A a)
{
    cout << "ff" << endl;
    return;
}



int main()
{
    A a;
    f(a);
    ff(a);
}