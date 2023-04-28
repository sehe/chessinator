#include <iostream>
#include <string>
#include <vector>
#include <set>

using namespace std;

int main(int argc, char *argv[])
{
    set<string> args{argv + 1, argv + argc};
    for (auto &arg : args)
        cout << arg << endl;

    string str;
    getline(cin, str);
    cout << str << endl;
}