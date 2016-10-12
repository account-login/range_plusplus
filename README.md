# range++

Pythonic range() in C++

See examples:

```C++
#include <iostream>
#include <vector>
#include "range.hpp"


using namespace std;
// import range() from namespace tz
using namespace tz;


int main()
{
    cout << "Simple usages" << endl;

    // output: 12345
    for (int i : range(5)) {
        cout << i;
    }
    cout << endl;

    // output: 567
    for (int i : range(5, 8)) {
        cout << i;
    }
    cout << endl;

    // output: 8642
    for (int i : range(8, 0, -2)) {
        cout << i;
    }
    cout << endl;

    cout << "range() works with floats too" << endl;
    // output: 1 1.1 1.2 1.3 1.4 1.5 1.6 1.7 1.8 1.9
    for (double d : range(1.0, 1.99, 0.1)) {
        cout << d << " ";
    }
    cout << endl;

    cout << "Works beyond numbers" << endl;
    vector<int> vec = { 1, 2, 3, 4, 5 };

    cout << "Works with iterator" << endl;
    // output: 54321
    for (auto it : range(vec.rbegin(), vec.rend())) {
        cout << *it;
    }
    cout << endl;

    cout << "Works with pointer" << endl;
    const int *ptr = vec.data();
    // output: 53
    for (const int *p : range(ptr + vec.size() - 1, ptr, -2)) {
        cout << *p;
    }
    cout << endl;

    return 0;
}
```
