#include <iostream>
#include "big_integer.h"

void print(big_integer const& a) {
    for (auto i : a.number) {
        std::cout << i << " ";
    }
    std::cout << "sign: " << a.negative <<"\n";
}

int main() {
    big_integer a = 4;
    big_integer b = 7;
    (a = b) = a;

    if (a == 7) {
        std::cout << "a == 7\n";
    }

    if (b == 7) {
        std::cout << "b == 7\n";
    }
}