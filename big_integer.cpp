#include <cstring>
#include <stdexcept>
#include <limits>
#include <iostream>
#include <cassert>
#include <algorithm>

#include "big_integer.h"

namespace big_integer_ns {
    //using namespace big_integer_h_ns;
    using std::uint32_t;
    using std::int32_t;
    using std::size_t;
    using std::uint64_t;
    using std::int64_t;

    const big_integer MAX_POW_10 = static_cast<uint32_t>(1e9);

    big_integer::big_integer() :
            number({0}),
            negative(false) {}

    uint32_t big_integer::abs_for_int32(int32_t a) {
        if (a == std::numeric_limits<int32_t>::min()) {
            return static_cast<uint32_t>(1) << static_cast<uint32_t>(31);
        }

        return static_cast<uint32_t>(a < 0 ? -a : a);
    }

    big_integer::big_integer(int32_t a) :
            negative(a < 0) {
        number = {abs_for_int32(a)};
    }

    big_integer::big_integer(uint32_t a) :
            number({a}),
            negative(false) {}

    big_integer::big_integer(std::string const &str) {
        size_t start = 0;
        negative = false;
        number = {0};

        bool this_negative = false;
        if (str[0] == '-') {
            this_negative = true;
            start = 1;
        } else if (str[0] == '+') {
            start = 1;
        }

        for (size_t i = start; i < str.size(); ++i) {
            operator*=(10);
            operator+=(static_cast<uint32_t>(str[i] - '0'));
        }

        negative = this_negative;
        delete_zeroes();
    }

    big_integer::big_integer(vector_t const &a) { //от дополнения до 2ки
        if (a.back() == 0) {
            number = a;
            negative = false;
        } else {
            number = negate(a);
            negative = true;
        }

        delete_zeroes();
    }

    void big_integer::small_add(size_t pos, uint64_t add, uint32_t &carry) {
        uint64_t res = static_cast<uint64_t>(number[pos] + add + carry);
        number[pos] = static_cast<uint32_t>(res & MAX_UINT_32);
        carry = static_cast<uint32_t>(res >> SHIFT_32);
    }

    big_integer &big_integer::add(big_integer const &rhs) { // Сложение беззнаковое
        number.resize(std::max(number.size(), rhs.number.size()), 0);
        uint32_t carry = 0;

        for (size_t i = 0; i < rhs.number.size(); ++i) {
            small_add(i, rhs.number[i], carry);
        }

        for (size_t i = rhs.number.size(); i < number.size(); ++i) {
            small_add(i, 0, carry);
        }

        if (carry > 0) {
            number.push_back(carry);
        }

        delete_zeroes();

        return *this;
    }

    big_integer &big_integer::subtraction_larger(big_integer const &rhs) { // Вычитание, |this| >= |rhs|
        int64_t borrow = 0;

        for (size_t i = 0; i < rhs.number.size(); ++i) {
            int64_t res = number[i];

            if (res < rhs.number[i] + borrow) {
                res += BASE - rhs.number[i] - borrow;
                borrow = 1;
            } else {
                res -= rhs.number[i] + borrow;
                borrow = 0;
            }

            number[i] = static_cast<uint32_t >(res);
        }

        for (size_t i = rhs.number.size(); i < number.size(); ++i) {
            if (borrow == 0) {
                break;
            }

            if (number[i] == 0) {
                number[i] = MAX_UINT_32;
            } else {
                --number[i];
                break;
            }
        }

        delete_zeroes();

        return *this;
    }

    big_integer &big_integer::subtraction_less(big_integer const &rhs) { // Вычитание, |this| < |rhs|
        big_integer res;
        res.number = rhs.number;
        res.subtraction_larger(*this);
        number = res.number;

        return *this;
    }

    bool big_integer::more_or_equal(big_integer const &a, big_integer const &b) { // |a| >= |b|
        if (a.number.size() != b.number.size()) {
            return a.number.size() > b.number.size();
        }

        for (size_t i = a.number.size(); i-- > 0;) {
            if (a.number[i] != b.number[i]) {
                return a.number[i] > b.number[i];
            }
        }

        return true;
    }

    big_integer &big_integer::subtraction(big_integer const &rhs) { // Вычитание беззнаковое (почти)
        if (more_or_equal(*this, rhs)) {
            subtraction_larger(rhs);
        } else {
            subtraction_less(rhs);
            negative ^= 1;
        }

        delete_zeroes();

        return *this;
    }

    big_integer &big_integer::operator+=(big_integer const &rhs) { // Сложение
        if (negative == rhs.negative) {
            add(rhs);
        } else {
            subtraction(rhs);
        }

        delete_zeroes();

        return *this;
    }

    big_integer &big_integer::operator-=(big_integer const &rhs) { // Вычитание
        if (negative == rhs.negative) {
            subtraction(rhs);
        } else {
            add(rhs);
        }

        return *this;
    }

    big_integer &big_integer::operator*=(int val) {
        negative ^= (val < 0);
        val = abs(val);
        uint32_t carry = 0;
        for (size_t i = 0; i < number.size(); ++i) {
            uint64_t mul = static_cast<uint64_t>(number[i]) * val + carry;
            number[i] = static_cast<uint32_t>(mul & MAX_UINT_32);
            carry = static_cast<uint32_t>(mul >> SHIFT_32);
        }
        if (carry) {
            number.push_back(carry);
        }
        delete_zeroes();
        return *this;
    }

    big_integer &big_integer::operator*=(big_integer const &rhs) {
        vector_t res(number.size() + rhs.number.size());
        for (size_t i = 0; i < rhs.number.size(); ++i) {
            uint32_t carry = 0;
            //todo вынести data
                    //
            for (size_t j = 0; j < number.size(); ++j) {
                uint64_t mul = static_cast<uint64_t>(rhs.number[i]) * number[j] + carry + res[i + j];
                res[i + j] = static_cast<uint32_t>(mul & MAX_UINT_32);
                carry = static_cast<uint32_t>(mul >> SHIFT_32);
            }
            if (carry != 0) {
                res[i + number.size()] += carry;
            }
        }

        negative ^= rhs.negative;
        swap(number, res);
        delete_zeroes();

        return *this;
    }

    big_integer &big_integer::delete_zeroes() {
        while (number.size() > 1 && number.back() == 0) {
            number.pop_back();
        }

        if (number.size() == 1 && number.back() == 0) {
            negative = false;
        }

        return *this;
    }

    big_integer big_integer::quotient(uint32_t b) {
        big_integer ret = 0;
        ret.number.resize(number.size());
        uint32_t carry = 0;

        for (size_t i = number.size(); i-- > 0;) {
            uint64_t cur = carry * BASE + number[i];
            ret.number[i] = static_cast<uint32_t >(cur / b);
            carry = static_cast<uint32_t>(cur % b);
        }

        ret.delete_zeroes();
        return ret;
    }

    big_integer big_integer::remainder(uint32_t b) {
        uint64_t carry = 0;

        for (size_t i = number.size(); i-- > 0;) {
            carry = (carry * BASE + number[i]) % b;
        }

        return static_cast<uint32_t>(carry);
    }

    void big_integer::shift_subtract(big_integer const &rhs, size_t pos) {
        uint64_t borrow = 0;
        size_t i;
        for (i = pos; i < std::min(number.size(), rhs.number.size() + pos); ++i) {
            uint32_t rhs_i = (i - pos < rhs.number.size()) ? rhs.number[i - pos] : 0;

            if (number[i] < rhs_i + borrow) {
                number[i] = static_cast<uint32_t>(BASE + number[i] - rhs_i - borrow);
                borrow = 1;
            } else {
                number[i] -= rhs_i + borrow;
                borrow = 0;
            }
        }

        while (borrow != 0) {
            if (number[i] != 0) {
                borrow = 0;
            }

            --number[i];
            ++i;
        }

        delete_zeroes();
    }

    bool big_integer::shift_leq(big_integer const &rhs, size_t pos) {
        if (pos + rhs.number.size() != number.size()) {
            return number.size() > pos + rhs.number.size();
        }

        for (size_t i = number.size(); i-- > pos;) {
            if (number[i] != rhs.number[i - pos]) {
                return number[i] > rhs.number[i - pos];
            }
        }

        return true;
    }

    uint32_t big_integer::bin_search(big_integer const &rhs, size_t pos) {
        uint64_t left = 0, right = BASE;

        while (right - left > 1) {
            uint32_t middle = static_cast<uint32_t>((left + right) / 2);

            big_integer debug = rhs * middle;

            if (shift_leq(rhs * middle, pos)) {
                left = middle;
            } else {
                right = middle;
            }
        }

        return static_cast<uint32_t>(left);
    }

    std::pair<big_integer, big_integer> big_integer::long_division(big_integer const &rhs) { //беззнаковое
        big_integer dividend = *this;
        big_integer ret = 0;
        ret.number.resize(number.size() - rhs.number.size() + 1, 0);

        for (size_t i = ret.number.size(); i-- > 0;) {
            ret.number[i] = dividend.bin_search(rhs, i);
            dividend.shift_subtract(ret.number[i] * rhs, i);
        }

        ret.delete_zeroes();

        return {ret, dividend};
    }

    std::pair<big_integer, big_integer> big_integer::division_with_remainder(big_integer const &rhs) {
        if (rhs.number.size() == 1) {
            //assert(rhs.number.back() != 0);

            return {quotient(rhs.number.back()), remainder(rhs.number.back())};
        } else {
            if (number.size() < rhs.number.size()) {
                big_integer ret_f = big_integer(0);
                return {ret_f, *this};
            } else {
                return long_division(rhs);
            }
        }
    }

    big_integer &big_integer::operator/=(big_integer const &rhs) {
        std::pair<big_integer, big_integer> div_and_rem = division_with_remainder(rhs);

        bool this_negative = negative;
        *this = div_and_rem.first;
        negative = (this_negative != rhs.negative);

        return *this;
    }

    big_integer &big_integer::operator%=(big_integer const &rhs) {
        std::pair<big_integer, big_integer> div_and_rem = division_with_remainder(rhs);

        bool this_negative = negative;
        *this = div_and_rem.second;
        negative = this_negative;

        delete_zeroes();
        return *this;
    }

    vector_t big_integer::get_twos_complement(size_t size) const {
        vector_t ret = number;
        ret.resize(size, 0);

        if (!negative) {
            return ret;
        }

        return negate(ret);
    }

    big_integer &big_integer::operator&=(big_integer const &rhs) {
        size_t size = std::max(number.size(), rhs.number.size()) + 1;
        vector_t a = get_twos_complement(size);
        vector_t b = rhs.get_twos_complement(size);

        for (size_t i = 0; i < size; ++i) {
            a[i] &= b[i];
        }
        *this = big_integer(a);

        return *this;
    }

    big_integer &big_integer::operator|=(big_integer const &rhs) {
        size_t size = std::max(number.size(), rhs.number.size()) + 1;
        vector_t a = get_twos_complement(size);
        vector_t b = rhs.get_twos_complement(size);

        for (size_t i = 0; i < size; ++i) {
            a[i] |= b[i];
        }
        *this = big_integer(a);

        return *this;
    }

    big_integer &big_integer::operator^=(big_integer const &rhs) {
        size_t size = std::max(number.size(), rhs.number.size()) + 1;
        vector_t a = get_twos_complement(size);
        vector_t b = rhs.get_twos_complement(size);

        for (size_t i = 0; i < size; ++i) {
            a[i] ^= b[i];
        }
        *this = big_integer(a);

        return *this;
    }

    big_integer big_integer::operator~() const {
        size_t size = number.size() + 1;
        vector_t a = get_twos_complement(size);

        for (size_t i = 0; i < a.size(); ++i) {
            a[i] = ~a[i];
        }

        return big_integer(a);
    }

    big_integer &big_integer::operator<<=(size_t shift) {
        vector_t a = get_twos_complement(number.size() + 1);

        vector_t ret(a.size() + (shift + SHIFT_32 - 1) / SHIFT_32, 0);
        for (size_t i = shift / SHIFT_32; i < ret.size(); ++i) {
            if (i - shift / SHIFT_32 < a.size()) {
                ret[i] = a[i - shift / SHIFT_32];
            }
        }

        shift %= SHIFT_32;

        for (size_t i = ret.size() - 1; i-- > 0;) {
            ret[i + 1] |= ret[i] >> (SHIFT_32 - shift);
            ret[i] <<= shift;
        }

        if (ret.back() != 0) {
            ret.back() = MAX_UINT_32;
        }

        *this = big_integer(ret);

        return *this;
    }

    big_integer &big_integer::operator>>=(size_t shift) {
        vector_t a = get_twos_complement(number.size() + 1);

        vector_t ret(a.size() - shift / SHIFT_32, 0);

        for (size_t i = ret.size(); i-- > 0;) {
            if (i + shift / SHIFT_32 < a.size()) {
                ret[i] = a[i + shift / SHIFT_32];
            }
        }

        shift %= SHIFT_32;

        for (size_t i = 0; i < ret.size(); ++i) {
            if (i != 0) {
                ret[i - 1] |= ret[i] << (SHIFT_32 - shift);
            }

            ret[i] >>= shift;
        }

        if (ret.back() != 0) {
            ret.back() = MAX_UINT_32;
        }

        *this = big_integer(ret);

        return *this;
    }

    big_integer big_integer::operator+() const {
        return *this;
    }

    big_integer big_integer::operator-() const {
        big_integer ret = *this;
        ret.negative = !ret.negative;
        ret.delete_zeroes();

        return ret;
    }

    big_integer &big_integer::operator++() { // Инкремент префиксный
        *this += static_cast<uint32_t>(1);
        return *this;
    }

    big_integer big_integer::operator++(int) { // Инкремент постфиксный
        big_integer ret = *this;
        ++*this;
        return ret;
    }

    big_integer &big_integer::operator--() { // Декремент префиксный
        *this -= static_cast<uint32_t>(1);
        return *this;
    }

    big_integer big_integer::operator--(int) { // Декремент постфиксный
        big_integer ret = *this;
        --*this;
        return ret;
    }


    big_integer operator+(big_integer a, big_integer const &b) {
        return a += b;
    }

    big_integer operator-(big_integer a, big_integer const &b) {
        return a -= b;
    }

    big_integer operator*(big_integer a, big_integer const &b) {
        return a *= b;
    }

    big_integer operator/(big_integer a, big_integer const &b) {
        return a /= b;
    }

    big_integer operator%(big_integer a, big_integer const &b) {
        return a %= b;
    }

    big_integer operator&(big_integer a, big_integer const &b) {
        return a &= b;
    }

    big_integer operator|(big_integer a, big_integer const &b) {
        return a |= b;
    }

    big_integer operator^(big_integer a, big_integer const &b) {
        return a ^= b;
    }

    big_integer operator<<(big_integer a, int b) {
        return a <<= b;
    }

    big_integer operator>>(big_integer a, int b) {
        return a >>= b;
    }

    bool operator==(big_integer const &a, big_integer const &b) {
        return a.negative == b.negative && a.number == b.number;
    }

    bool operator!=(big_integer const &a, big_integer const &b) {
        return !(a == b);
    }

    bool operator<(big_integer const &a, big_integer const &b) {
        if (a.negative != b.negative) {
            return a.negative;
        }
        return big_integer::more_or_equal(b, a) && a != b;
    }

    bool operator>(big_integer const &a, big_integer const &b) {
        return !(a < b) && a != b;
    }

    bool operator<=(big_integer const &a, big_integer const &b) {
        return a < b || a == b;
    }

    bool operator>=(big_integer const &a, big_integer const &b) {
        return a > b || a == b;
    }

    std::string big_integer::complement_to_base(std::string s) {
        return std::string(9 - s.size(), '0') + s;
    }

    std::string to_string(big_integer const &a) {
        if (a.number.size() == 1) {
            return (a.negative ? "-" : "") + std::to_string(a.number[0]);
        }

        big_integer dividend = a;
        dividend.negative = false;

        std::vector<std::string> ret;

        big_integer quotient;
        while (dividend > 0) {
            quotient = dividend % MAX_POW_10;
            dividend /= MAX_POW_10;
            ret.push_back(big_integer::complement_to_base(std::to_string(quotient.number[0])));
        }

        size_t pos0 = 0;
        for (size_t i = 0; i < ret.back().size(); ++i) {
            if (ret.back()[i] == '0') {
                pos0 = i + 1;
            } else {
                break;
            }
        }

        ret[ret.size() - 1] = ret[ret.size() - 1].substr(pos0);

        std::reverse(ret.begin(), ret.end());
        std::string string_of_big_integer;

        if (a.negative) {
            string_of_big_integer = "-";
        }


        for (auto i : ret) {
            string_of_big_integer += i;
        }

        return string_of_big_integer;
    }

    std::ostream &operator<<(std::ostream &s, big_integer const &a) {
        return s << to_string(a);
    }

    void big_integer::inc(vector_t &v) const {
        uint64_t carry = 1;

        for (size_t i = 0; i < v.size(); ++i) {
            uint64_t res = static_cast<uint64_t>(v[i]) + carry;
            v[i] = static_cast<uint32_t>(res & MAX_UINT_32);
            carry = static_cast<uint32_t>(res >> SHIFT_32);
        }
        //assert(!carry);
    }

    vector_t big_integer::negate(vector_t const &v) const {
        vector_t ret(v.size());

        for (size_t i = 0; i < v.size(); ++i) {
            ret[i] = ~v[i];
        }

        inc(ret);
        return ret;
    }
}