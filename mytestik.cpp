#include <iostream>
#include <limits>
#include <algorithm>
#include "big_integer.h"

const uint32_t MAX_UINT_32 = std::numeric_limits<uint32_t>::max();
const uint64_t BASE = static_cast<uint64_t>(MAX_UINT_32) + 1;
const uint32_t number_of_multipliers = 7;

using namespace big_integer_ns;

int main() {
    std::vector<uint32_t> multy = {596516650, 1681692778, 1189641422, 1804289384, 1649760493, 424238336, 846930887};

    std::vector<big_integer> cur(number_of_multipliers);
    big_integer a = 1;
    for (size_t i = 0; i < number_of_multipliers; ++i) {
        a *= multy[i];
        cur[i] = a;
        std::cerr << i << ": " << cur[i] << "\n";
    }
}