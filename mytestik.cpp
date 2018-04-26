#include <iostream>
#include "big_integer.h"

void print(big_integer const& a) {
    for (auto i : a.number) {
        std::cout << i << " ";
    }
    std::cout << "sign: " << a.negative <<"\n";
}

int main() {
    big_integer a, b;
    a.number = {1, 2, 3};
    b.number = {5, 6};
    a.negative = false;
    b.negative = false;

    a *= b;

    print(a);
    print(b);
}