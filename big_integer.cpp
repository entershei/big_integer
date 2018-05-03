#include <cstring>
#include <stdexcept>
#include <limits>
#include <iostream>
#include <cassert>
#include <algorithm>

#include "big_integer.h"

//const uint32_t MAX_UINT_32 = std::numeric_limits<uint32_t>::max();
const uint32_t MAX_UINT_32 = 15;
const uint64_t BASE = static_cast<uint64_t>(MAX_UINT_32) + 1;
const uint32_t SHIFT_32 = 4;
//const uint32_t SHIFT_32 = 32;
//const uint32_t HALF_MAX = BASE / 2;

big_integer::big_integer() :
        number({0}),
        negative(false) {}

uint32_t big_integer::abs_for_int32(int32_t a) {
    if (a == std::numeric_limits<uint32_t>::min()) {
        return static_cast<uint32_t>(1) << static_cast<uint32_t>(31);
    }

    return static_cast<uint32_t>(a < 0 ? -a : a);
}

big_integer::big_integer(int32_t a) {
    number = {abs_for_int32(a)};
    negative = a < 0;
}

big_integer::big_integer(std::string const &str) {
    size_t start = 0;
    negative = false;
    number = {0};

    if (str[0] == '-') {
        negative = true;
        start = 1;
    } else if (str[0] == '+') {
        start = 1;
    }

    for (size_t i = start; i < str.size(); ++i) {
        *this = *this * 10 + static_cast<uint32_t>(str[i] - '0');
    }

    delete_zeroes();
}

big_integer::big_integer(std::vector<uint32_t> const &a) { //от дополнения до 2ки
    if (a.back() == 0) {
        number = a;
        negative = false;
    } else {
        number = negate(a);
        negative = true;
    }

    delete_zeroes();
}

void swap(big_integer &a, big_integer &b) {
    std::swap(a.number, b.number);
    std::swap(a.negative, b.negative);
}

big_integer &big_integer::operator=(big_integer other) {
    swap(*this, other);
    return *this;
}

void big_integer::small_add(size_t pos, uint64_t add, uint32_t &carry) {
    uint64_t res = static_cast<uint64_t>(this->number[pos] + add + carry);
    this->number[pos] = static_cast<uint32_t>(res & MAX_UINT_32);
    carry = static_cast<uint32_t>(res >> SHIFT_32);
}

big_integer &big_integer::add(big_integer const &rhs) { // Сложение беззнаковое
    number.resize(std::max(this->number.size(), rhs.number.size()), 0);
    uint32_t carry = 0;

    for (size_t i = 0; i < rhs.number.size(); ++i) {
        small_add(i, rhs.number[i], carry);
    }

    for (size_t i = rhs.number.size(); i < this->number.size(); ++i) {
        small_add(i, 0, carry);
    }

    if (carry > 0) {
        number.push_back(carry);
    }

    return *this;
}

big_integer &big_integer::subtraction_larger(big_integer const &rhs) { // Вычитание, |this| >= |rhs|
    int64_t borrow = 0;

    for (size_t i = 0; i < rhs.number.size(); ++i) {
        int64_t res = static_cast<int64_t >(this->number[i]) - borrow;

        if (res < rhs.number[i]) {
            res += BASE - rhs.number[i];
            borrow = 1;
        } else {
            res -= rhs.number[i];
            borrow = 0;
        }

        this->number[i] = static_cast<uint32_t >(res);
    }

    for (size_t i = rhs.number.size(); i < this->number.size(); ++i) {
        if (borrow == 0) {
            break;
        }

        if (this->number[i] == 0) {
            this->number[i] = MAX_UINT_32;
        } else {
            --this->number[i];
            break;
        }
    }

    delete_zeroes();

    return *this;
}

big_integer &big_integer::subtraction_less(big_integer const &rhs) { // Вычитание, |this| < |rhs|
    big_integer res = rhs;
    res.subtraction_larger(*this);
    *this = res;

    return *this;
}

bool more_or_equal(big_integer const &a, big_integer const &b) { // |a| >= |b|
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
        this->subtraction_larger(rhs);
    } else {
        this->subtraction_less(rhs);
        this->negative ^= 1;
    }

    return *this;
}

big_integer &big_integer::operator+=(big_integer const &rhs) { // Сложение
    if (this->negative == rhs.negative) {
        this->add(rhs);
    } else {
        this->subtraction(rhs);
    }

    return *this;
}

big_integer &big_integer::operator-=(big_integer const &rhs) { // Вычитание
    if (this->negative == rhs.negative) {
        this->subtraction(rhs);
    } else {
        this->add(rhs);
    }

    return *this;
}

uint32_t big_integer::insert_or_add(size_t pos, uint64_t x) {
    this->number.resize(std::max(this->number.size(), pos + 1), 0);

    uint64_t ret = this->number[pos] + x;
    this->number[pos] = static_cast<uint32_t >(ret & MAX_UINT_32);
    return static_cast<uint32_t>(ret >> SHIFT_32);
}

big_integer &big_integer::operator*=(big_integer const &rhs) {
    big_integer ret = 0;


    for (size_t i = 0; i < rhs.number.size(); ++i) {
        uint32_t carry = 0;
        for (size_t j = 0; j < this->number.size(); ++j) {
            uint64_t mul = static_cast<uint64_t >(rhs.number[i]) * this->number[j];
            carry = ret.insert_or_add(i + j, mul + carry);
        }
        if (carry != 0) {
            carry = ret.insert_or_add(i + number.size(), carry);
            assert(carry == 0);
        }
    }



    ret.negative = this->negative ^ rhs.negative;
    ret.delete_zeroes();

    *this = ret;

    return *this;
}

big_integer &big_integer::normalization(big_integer const &rhs) {
    big_integer new_rhs = rhs;
    int64_t multiplier = BASE / (rhs.number.back() + 1);

    *this *= multiplier;
    new_rhs *= multiplier;

    return new_rhs;
}

big_integer &big_integer::delete_zeroes() {
    while (this->number.size() > 1 && this->number.back() == 0) {
        this->number.pop_back();
    }

    if (this->number.size() == 1 && this->number.back() == 0) {
        this->negative = false;
    }

    return *this;
}

big_integer big_integer::quotient(uint32_t b) {
    big_integer ret = 0;
    ret.number.resize(this->number.size());
    uint32_t carry = 0;

    for (size_t i = this->number.size(); i-- > 0;) {
        uint64_t cur = carry * BASE + this->number[i];
        ret.number[i] = static_cast<uint32_t >(cur / b);
        carry = static_cast<uint32_t>(cur % b);
    }

    ret.delete_zeroes();
    return ret;
}

big_integer big_integer::remainder(uint32_t b) {
    big_integer ret = 0;
    uint64_t carry = 0;

    for (size_t i = this->number.size(); i-- > 0;) {
        carry = (carry * BASE + this->number[i]) % b;
    }

    ret = carry;
    return ret;
}

uint32_t big_integer::divide(uint32_t h, uint32_t &l, uint32_t b) {
    uint64_t a = (h << SHIFT_32) | l;
    uint32_t ret = a % b;
    l = a / b;

    return ret;
}

uint32_t trial(big_integer const &a, big_integer const &b, size_t pos) {
    if (more_or_equal(b, a)) {
        return 0;
    }

    big_integer a3(0);
    a3.number = {a.number[pos - 2], a.number[pos - 1], static_cast<uint32_t >(0)};

    if (pos != a.number.size()) {
        a3.number.back() = a.number[pos];
    }

    big_integer b2(0);
    b2.number = {b.number[pos - 1], b.number[pos]};
    uint32_t test;

    if (b2.number[1] > a3.number[2]) {
        uint32_t cur = a3.number[2];
        test = a3.number[1];
        cur = divide(cur, test, b2.number[1]);
    } else {
        test = MAX_UINT_32;
    }
    b2 *= test;
    if (a3 < b2) {
        --test;
    }

    return test;
}

bool big_integer::shift_larger(big_integer const &first, big_integer const &second, size_t shift) {
    if (first.number.size() < second.number.size() + shift) {
        return false;
    }

    if (first.number.size() > second.number.size() + shift) {
        return true;
    }

    for (size_t i = first.number.size(); i-- > 0;) {
        size_t pos = i - shift;
        uint32_t cur = 0;

        if (i >= 0) {
            cur = second.number[pos];
        }
        if (first.number[i] != cur) {
            return first.number[i] > cur;
        }
    }
}

big_integer &big_integer::shift_subtraction(big_integer const &rhs, size_t shift) {
    size_t finish = rhs.number.size();
    int64_t borrow = 0;

    for (size_t i = 0; i < finish; ++i) {
        int64_t res = static_cast<int64_t >(this->number[i]) - borrow;

        if (res < rhs.number[i + shift]) {
            res += BASE - rhs.number[i + shift];
            borrow = 1;
        } else {
            res -= rhs.number[i + shift];
            borrow = 0;
        }

        this->number[i] = static_cast<uint32_t >(res);
    }

    if (borrow > 0) {
        this->number.back() -= 1;
    }

    return *this;

}

std::pair<big_integer &, big_integer &> big_integer::long_division(big_integer rhs) {
    this->number.push_back(0);
    big_integer ret;
    ret.number.resize(this->number.size() - rhs.number.size(), 0);

    rhs = this->normalization(rhs);

    for (size_t i = ret.number.size(); i-- > 0;) {
        uint32_t cur = trial(*this, rhs, rhs.number.size() + i);
        big_integer test = rhs;
        test *= cur;

        if (shift_larger(*this, test, i)) {
            --cur;
            test -= rhs;
        }

        ret.number[i] = cur;
        this->shift_subtraction(test, i);

        if (this->number.empty()) {
            break;
        }
    }

    ret.delete_zeroes();

    return {ret, *this};
}

//uint32_t big_integer::bin_search(uint32_t a, uint32_t b, uint64_t& carry) {
//    if (b > a + carry) {
//        carry += a;
//        carry *= BASE;
//
//        return 0;
//    }
//
//    uint32_t ret = static_cast<uint32_t>((carry + a) / b);
//    carry = ((carry + a) % b) * BASE;
//
//    return ret;
//}

//std::pair<big_integer&, big_integer&> big_integer::long_division(big_integer const &rhs) {
//    big_integer ret = 0;
//    ret.number.resize(this->number.size() - rhs.number.size() + 1 , 0);
//
//    uint64_t carry = 0;
//    size_t ind_rhs = rhs.number.size() - 1;
//
//    for (size_t i = ret.number.size(); i-- > 0;) {
//        ret.number[i] = bin_search(this->number[i + rhs.number.size() - 1], rhs.number[ind_rhs], carry);
//
//        if (ret.number[i] != 0) {
//            ++ind_rhs;
//        }
//    }
//
//    if (more_or_equal(ret * rhs, *this)) {
//        --ret;
//    }
//
//    ret.delete_zeroes();
//    big_integer carry_big_int = carry;
//
//    return {ret, carry_big_int};
//}

std::pair<big_integer, big_integer> big_integer::division_with_remainder(big_integer const &rhs) {
    if (rhs.number.size() == 1) {
        assert(rhs.number.back() != 0);

        return {this->quotient(rhs.number.back()), this->remainder(rhs.number.back())};
    } else {
        if (this->number.size() < rhs.number.size()) {
            big_integer ret_f = big_integer(0);
            return {ret_f, *this};
        } else {
            return this->long_division(rhs);
        }
    }
}

big_integer &big_integer::operator/=(big_integer const &rhs) {
    std::pair<big_integer, big_integer> div_and_rem = division_with_remainder(rhs);

    *this = div_and_rem.first;
    this->negative = (this->negative != rhs.negative);
    return *this;
}

big_integer &big_integer::operator%=(big_integer const &rhs) {
    throw std::runtime_error("oh-oh");
}

std::vector<uint32_t> big_integer::get_twos_complement(size_t size) const {
    std::vector<uint32_t> ret(size, 0);

    if (!negative) {
        return number;
    }

    return negate(number);
}

big_integer &big_integer::operator&=(big_integer const &rhs) {
    size_t size = std::max(number.size(), rhs.number.size()) + 1;
    std::vector<uint32_t> a = get_twos_complement(size);
    std::vector<uint32_t> b = rhs.get_twos_complement(size);

    for (size_t i = 0; i < size; ++i) {
        a[i] &= b[i];
    }
    *this = big_integer(a);

    delete_zeroes();
    return *this;
}

big_integer &big_integer::operator|=(big_integer const &rhs) {
    size_t size = std::max(number.size(), rhs.number.size()) + 1;
    std::vector<uint32_t> a = get_twos_complement(size);
    std::vector<uint32_t> b = rhs.get_twos_complement(size);

    for (size_t i = 0; i < size; ++i) {
        a[i] |= b[i];
    }
    *this = big_integer(a);

    delete_zeroes();
    return *this;
}

big_integer &big_integer::operator^=(big_integer const &rhs) {
    size_t size = std::max(number.size(), rhs.number.size()) + 1;
    std::vector<uint32_t> a = get_twos_complement(size);
    std::vector<uint32_t> b = rhs.get_twos_complement(size);

    for (size_t i = 0; i < size; ++i) {
        a[i] ^= b[i];
    }
    *this = big_integer(a);

    delete_zeroes();
    return *this;
}

big_integer &big_integer::operator<<=(size_t shift) {
    !!!
    std::vector<uint32_t> a = get_twos_complement(number.size());

    std::vector ret(a.size() + shift, 0);
    for (size_t i = shift; i < ret.size(); ++i) {
        ret[i] = a[i - shift];
    }

    *this = big_integer(a);

    delete_zeroes();
    return *this;
}

big_integer &big_integer::operator>>=(size_t shift) {
    !!!
    std::vector<uint32_t> a = get_twos_complement(number.size());

    std::vector ret = a;
    ret.resize(a.size() + shift, 0);

    *this = big_integer(a);

    delete_zeroes();
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

big_integer big_integer::operator~() const {
    throw std::runtime_error("oh-oh");
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
    throw std::runtime_error("oh-oh");
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
    return more_or_equal(b, a) && a != b;
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

std::string to_string(big_integer const &a) {
    throw std::runtime_error("oh-oh");
}

std::ostream &operator<<(std::ostream &s, big_integer const &a) {
    return s << to_string(a);
}

void big_integer::inc(std::vector<uint32_t>& v) const {
    uint64_t carry = 1;

    for (size_t i = 0; i < v.size(); ++i) {
        uint64_t res = static_cast<uint64_t>(v[i]) + carry;
        v[i] = static_cast<uint32_t>(res & MAX_UINT_32);
        carry = static_cast<uint32_t>(res >> SHIFT_32);
    }
    assert(!carry);
}

std::vector<uint32_t> big_integer::negate(std::vector<uint32_t> const& v) const {
    std::vector<uint32_t> ret(v.size() + 1);

    for (size_t i = 0; i < v.size(); ++i) {
        ret[i] = ~v[i];
    }

    inc(ret);
    return ret;
}