#include <bits/stdc++.h>

using namespace std;

struct Item {
int value;
uint32_t next_pointer;
};

int main() {
    ofstream out("out.txt", ios::binary);
    int n = 1;
    Item items[n];
    for (int i = 0; i < n; ++i) {
        items[i].value = 99;
        items[i].next_pointer = 0 * sizeof(Item);
    }

    out.write((char*)items, n * sizeof(Item));

    return 0;
}
