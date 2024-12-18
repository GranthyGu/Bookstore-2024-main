#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;

int main() {
    freopen("in.txt", "w", stdout);  // 将输入写入out.dat文件中 
    cout << 2000 << endl;
    for (int i = 0; i < 2000; i++)
    {
        cout << "insert" << ' ' << "TestIndex" << " " << i + 1 << endl;
    }
    cout << "find TestIndex";
    return 0;
}