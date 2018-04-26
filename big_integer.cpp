#include <cstring>
#include <stdexcept>
#include <limits>
#include <iostream>

#include "big_integer.h"

const uint32_t MAX_UINT_32 = std::numeric_limits<uint32_t>::max();
const uint64_t MAX_UINT_64 = std::numeric_limits<uint64_t>::max();
const uint32_t SHIFT_32 = 32;

big_integer::big_integer() :
        number{},
        negative(false) {}

uint32_t big_integer::abs_for_int32(int32_t a) {
    return static_cast<uint32_t>(a < 0 ? -a : a);
}

big_integer::big_integer(int32_t a) {
    number = {abs_for_int32(a)};
    negative = a < 0;
}

big_integer::big_integer(std::string const &str) {
    throw std::runtime_error("oh-oh");
}

void swap(big_integer &a, big_integer &b) {
    std::swap(a.number, b.number);
    std::swap(a.negative, b.negative);
}

big_integer &big_integer::operator=(big_integer other) {
    swap(*this, other);
    return *this;
}

void big_integer::small_add(size_t pos, uint64_t add, uint32_t *carry) {
    uint64_t res = static_cast<uint64_t>(this->number[pos] + add + *carry);
    this->number[pos] = static_cast<unsigned int>(res & MAX_UINT_32);
    *carry = static_cast<uint32_t>(res >> SHIFT_32);
}

big_integer &big_integer::add(big_integer const &rhs) { // Сложение беззнаковое
    this->number.resize(std::max(this->number.size(), rhs.number.size()), 0);
    size_t finish = rhs.number.size();
    uint32_t carry = 0;

    for (size_t i = 0; i < finish; ++i) {
        small_add(i, rhs.number[i], &carry);
    }

    for (size_t i = finish; i < this->number.size(); ++i) {
        small_add(i, 0, &carry);
    }

    if (carry > 0) {
        this->number.push_back(carry);
    }

    return *this;
}

big_integer &big_integer::subtraction_larger(big_integer const &rhs) { // Вычитание, |this| больше |rhs|
    size_t finish = rhs.number.size();
    int64_t borrow = 0;

    for (size_t i = 0; i < finish; ++i) {
        int64_t res = static_cast<int64_t >(this->number[i]) - borrow;

        if (res < rhs.number[i]) {
            res += MAX_UINT_32 - rhs.number[i];
            borrow = 1;
        } else {
            res -= rhs.number[i];
            borrow = 0;
        }

        this->number[i] = static_cast<uint32_t >(res);
    }

    if (borrow > 0) {
        this->number.back() -= 1;
    }

    return *this;
}

big_integer &big_integer::subtraction_less(big_integer const &rhs) { // Вычитание, |this| меньше |rhs|
    big_integer res = rhs;
    res.subtraction_larger(*this);
    *this = res;

    return *this;
}

bool larger(big_integer const &a, big_integer const &b) { // |a| >= |b|
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
    if (larger(*this, rhs)) {
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
    return static_cast<uint32_t>(ret << SHIFT_32);
}

big_integer &big_integer::operator*=(big_integer const &rhs) {
    big_integer ret = 0;
    uint32_t carry = 0;

    for (size_t i = 0; i < rhs.number.size(); ++i) {
        for (size_t j = 0; j < this->number.size(); ++j) {
            uint64_t mul = rhs.number[i] * this->number[j];
            carry = ret.big_integer::insert_or_add(i + j, mul + carry);
        }
    }

    if (carry != 0) {
        ret.number.push_back(carry);
    }

    ret.negative = this->negative ^ rhs.negative;

    *this = ret;

    return *this;
}

big_integer &big_integer::operator/=(big_integer const &rhs) {
    throw std::runtime_error("oh-oh");
}

big_integer &big_integer::operator%=(big_integer const &rhs) {
    throw std::runtime_error("oh-oh");
}

big_integer &big_integer::operator&=(big_integer const &rhs) {
    throw std::runtime_error("oh-oh");
}

big_integer &big_integer::operator|=(big_integer const &rhs) {
    throw std::runtime_error("oh-oh");
}

big_integer &big_integer::operator^=(big_integer const &rhs) {
    throw std::runtime_error("oh-oh");
}

big_integer &big_integer::operator<<=(int rhs) {
    throw std::runtime_error("oh-oh");
}

big_integer &big_integer::operator>>=(int rhs) {
    throw std::runtime_error("oh-oh");
}

big_integer big_integer::operator+() const {
    return *this;
}

big_integer big_integer::operator-() const {
    big_integer ret = *this;
    ret.negative = !ret.negative;

    return ret;
}

big_integer big_integer::operator~() const {
    throw std::runtime_error("oh-oh");
}

big_integer &big_integer::operator++() { // Инкремент префиксный
    *this += 1;
    return *this;
}

big_integer big_integer::operator++(int) { // Инкремент постфиксный
    big_integer ret = *this;
    ++*this;
    return ret;
}

big_integer &big_integer::operator--() { // Декремент префиксный
    *this -= 1;
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
    if (a.negative != b.negative) {
        return false;
    }
    return larger(a, b) == larger(b, a);
}

bool operator!=(big_integer const &a, big_integer const &b) {
    return !(a == b);
}

bool operator<(big_integer const &a, big_integer const &b) {
    if (a.negative != b.negative) {
        return a.negative == 1;
    }
    return larger(b, a) && a != b;
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
