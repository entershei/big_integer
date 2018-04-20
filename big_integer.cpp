#include <cstring>
#include <stdexcept>
#include <limits>
#include <iostream>

#include "big_integer.h"

const uint32_t MAX_UINT = std::numeric_limits<uint32_t>::max();

big_integer::big_integer() // Конструктор по умолчанию, инициализирующий число нулем
{
    number.resize(1);
    number[0] = 0;
    negative = false;
}

/*
big_integer::big_integer(big_integer const& other) // Конструктор копирования, после которого исходный объект и копию можно изменять независимо.
{
    mpz_init_set(mpz, other.mpz);
}
*/

/*
big_integer::big_integer(int a) // Конструктор от int
{
    mpz_init_set_si(mpz, a);
}
*/

/*
big_integer::big_integer(std::string const& str) // Explicit конструктор от std::string
{
    if (mpz_init_set_str(mpz, str.c_str(), 10))
    {
        mpz_clear(mpz);
        throw std::runtime_error("invalid string");
    }
}
*/

/*
big_integer::~big_integer() // Деструктор
{
    mpz_clear(mpz);
}
 */

void swap(bool& a, bool& b) {
    bool c = a;
    a = b;
    b = c;
}

void swap(big_integer& a, big_integer& b)
{
    std::swap(a.number, b.number);
    swap(a.negative, b.negative);
}


big_integer& big_integer::operator=(big_integer other) // Оператор присваивания
{
    swap(*this, other);
    return *this;
}


big_integer& big_integer::add(big_integer const& rhs) // Сложение беззнаковое
{
    this->number.resize(std::max(this->number.size(), rhs.number.size()), 0);
    size_t finish = rhs.number.size();
    size_t carry = 0;

    for (size_t i = 0; i < finish; ++i)
    {
        uint64_t res = this->number[i] + rhs.number[i] + carry;
        this->number[i] = static_cast<unsigned int>(res & MAX_UINT);
        carry = static_cast<size_t>(res ^ MAX_UINT);
    }

    for (size_t i = finish; i < this->number.size(); ++i) {
        int64_t res = this->number[i] + carry;
        this->number[i] = static_cast<unsigned int>(res & MAX_UINT);
        carry = static_cast<size_t>(res ^ MAX_UINT);
    }

    return *this;
}

big_integer& big_integer::subtraction_larger(big_integer const& rhs) // Вычитание, |this| больше |rhs|
{
    size_t finish = rhs.number.size();
    int64_t borrow = 0;

    for (size_t i = 0; i < finish; ++i) {
        int64_t res = static_cast<int64_t >(this->number[i]) - borrow;

        if (res < rhs.number[i]) {
            res += MAX_UINT - rhs.number[i];
            borrow = 1;
        } else {
            res -= rhs.number[i];
            borrow = 0;
        }

        this->number[i] = static_cast<uint32_t >(res);
    }

    return *this;
}

big_integer& big_integer::subtraction_less(big_integer const& rhs) // Вычитание, |this| меньше |rhs|
{
    big_integer res = rhs;
    res.subtraction_larger(*this);

    *this = res;

    return *this;
}

bool larger(big_integer const& a, big_integer const& b) // a >= b беззнаково
{
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

big_integer& big_integer::subtraction(big_integer const& rhs) // Вычитание беззнаковое (почти)
{
    if (larger(*this, rhs)) {
        this->subtraction_larger(rhs);
    } else {
        this->subtraction_less(rhs);
        this->negative ^= 1;
    }

    return *this;
}

big_integer& big_integer::operator+=(big_integer const& rhs) // Сложение
{
    if (this->negative == rhs.negative) {
        this->add(rhs);
    } else {
        this->subtraction(rhs);
    }

    return *this;
}


big_integer& big_integer::operator-=(big_integer const& rhs) // Вычитание
{
    if (this->negative == rhs.negative) {
        this->subtraction(rhs);
    } else {
        this->add(rhs);
    }

    return *this;
}

/*
big_integer& big_integer::operator*=(big_integer const& rhs) // Умножение
{
    mpz_mul(mpz, mpz, rhs.mpz);
    return *this;
}
*/

/*
big_integer& big_integer::operator/=(big_integer const& rhs) // Деление
{
    mpz_tdiv_q(mpz, mpz, rhs.mpz);
    return *this;
}
*/

/*
big_integer& big_integer::operator%=(big_integer const& rhs) // Остаток от деления
{
    mpz_tdiv_r(mpz, mpz, rhs.mpz);
    return *this;
}
*/

/*
big_integer& big_integer::operator&=(big_integer const& rhs) // AND
{
    mpz_and(mpz, mpz, rhs.mpz);
    return *this;
}
*/

/*
big_integer& big_integer::operator|=(big_integer const& rhs) // OR
{
    mpz_ior(mpz, mpz, rhs.mpz);
    return *this;
}
*/

/*
big_integer& big_integer::operator^=(big_integer const& rhs) // XOR
{
    mpz_xor(mpz, mpz, rhs.mpz);
    return *this;
}
*/

/*
big_integer& big_integer::operator<<=(int rhs) // Побитовый сдвиг влево
{
    mpz_mul_2exp(mpz, mpz, rhs);
    return *this;
}
*/

/*
big_integer& big_integer::operator>>=(int rhs) // Побитовый сдвиг вправо
{
    mpz_div_2exp(mpz, mpz, rhs);
    return *this;
}
*/

/*
big_integer big_integer::operator+() const // Унарный плюс
{
    return *this;
}
*/

/*
big_integer big_integer::operator-() const // Унарный минус
{
    big_integer r;
    mpz_neg(r.mpz, mpz);
    return r;
}
*/

/*
big_integer big_integer::operator~() const // NOT
{
    big_integer r;
    mpz_com(r.mpz, mpz);
    return r;
}
*/

/*
big_integer& big_integer::operator++() // Инкремент префиксный
{
    mpz_add_ui(mpz, mpz, 1);
    return *this;
}
 */

/*
big_integer big_integer::operator++(int) // Инкремент постфиксный
{
    big_integer r = *this;
    ++*this;
    return r;
}
*/

/*
big_integer& big_integer::operator--() // Декремент префиксный
{
    mpz_sub_ui(mpz, mpz, 1);
    return *this;
}
 */

/*
big_integer big_integer::operator--(int) // Декремент постфиксный
{
    big_integer r = *this;
    --*this;
    return r;
}
*/

/*
big_integer operator+(big_integer a, big_integer const& b) // Бинарное сложение
{
    return a += b;
}
*/

/*
big_integer operator-(big_integer a, big_integer const& b) // Бинарное вычитание
{
    return a -= b;
}
*/

/*
big_integer operator*(big_integer a, big_integer const& b) // Бинарное умножение
{
    return a *= b;
}
*/

/*
big_integer operator/(big_integer a, big_integer const& b) // Бинарное деление
{
    return a /= b;
}
*/

/*
big_integer operator%(big_integer a, big_integer const& b) // Остаток от деления бинарный
{
    return a %= b;
}
*/

/*
big_integer operator&(big_integer a, big_integer const& b) // Бинарный AND
{
    return a &= b;
}
*/

/*
big_integer operator|(big_integer a, big_integer const& b) // Бинарный OR
{
    return a |= b;
}
*/

/*
big_integer operator^(big_integer a, big_integer const& b) // Бинарный XOR
{
    return a ^= b;
}
*/

/*
big_integer operator<<(big_integer a, int b)
{
    return a <<= b;
}
*/

/*
big_integer operator>>(big_integer a, int b)
{
    return a >>= b;
}
*/

/*
bool operator==(big_integer const& a, big_integer const& b)
{
    return mpz_cmp(a.mpz, b.mpz) == 0;
}
*/

/*
bool operator!=(big_integer const& a, big_integer const& b)
{
    return mpz_cmp(a.mpz, b.mpz) != 0;
}
*/

/*
bool operator<(big_integer const& a, big_integer const& b)
{
    return mpz_cmp(a.mpz, b.mpz) < 0;
}
*/

/*
bool operator>(big_integer const& a, big_integer const& b)
{
    return mpz_cmp(a.mpz, b.mpz) > 0;
}
*/

/*
bool operator<=(big_integer const& a, big_integer const& b)
{
    return mpz_cmp(a.mpz, b.mpz) <= 0;
}
*/

/*
bool operator>=(big_integer const& a, big_integer const& b)
{
    return mpz_cmp(a.mpz, b.mpz) >= 0;
}
*/

/*
std::string to_string(big_integer const& a)
{
    char* tmp = mpz_get_str(NULL, 10, a.mpz);
    std::string res = tmp;

    void (*freefunc)(void*, size_t);
    mp_get_memory_functions (NULL, NULL, &freefunc);

    freefunc(tmp, strlen(tmp) + 1);

    return res;
}
*/

/*
std::ostream& operator<<(std::ostream& s, big_integer const& a)
{
    return s << to_string(a);
}
*/

int main() {
    big_integer a, b;
    a.number = {1, 2, 3};
    b.number = {5, 6};
    a.negative = false;
    b.negative = false;

    a += b;

    std::cout << a.number[0] << " " << a.number[1] << " " << a.number[2] << " " << a.negative << std::endl;
    std::cout << b.number[0] << " " << b.number[1] << " " << b.number[2] << " " << b.negative << std::endl;
}