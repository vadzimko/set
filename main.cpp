#include <iostream>
#include "Set.h"
#include <set>
#include <vector>
#include <algorithm>
#include <random>

using namespace std;

void print(Set<int> &s) {
    for (int a : s) {
        cerr << a <<" ";
    }
    cerr << "\n";
}
int main() {
    std::vector<int> k;
    for (int i = 0; i < 10; i++) k.push_back(i);

    std::shuffle(k.begin(), k.end(), std::default_random_engine());

    Set<int> v;
    for (int i : k) {
        v.insert(i);
    }

    print(v);

    auto it = v.begin();
    for (int i = 0; i < 10; i++) {
        cerr  << *it << "\n";
        cerr  << v.empty() << "\n";

        v.erase(it);
        it = v.begin();
    }


    return 0;
}