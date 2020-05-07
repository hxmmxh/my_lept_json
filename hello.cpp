#include <iostream>
#include <string.h>

using namespace std;

int main()
{
    const char *p = "hi";
    const char *q = "hiasdsad";
    cout << memcmp(p, q, strlen(p));
    q += strlen(p);
    cout << *q;
}