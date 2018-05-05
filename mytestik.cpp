#include <iostream>
#include "big_integer.h"

void print(big_integer const& a) {
    for (auto i : a.number) {
        std::cout << i << " ";
    }
    std::cout << "sign: " << a.negative <<"\n";
}

int main() {
    //540
    big_integer a("10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");
    big_integer b(                                                     "100000000000000000000000000000000000000");
    big_integer c("100000000000000000000000000000000000000000000000000000");

    if (a / b == c) {
        std::cout << "== c\n";
    }
}