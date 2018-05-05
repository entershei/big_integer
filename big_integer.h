#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <cstddef>
#include <iosfwd>
#include <vector>

struct big_integer
{
    big_integer();
    big_integer(big_integer const& other) = default;
    big_integer(int32_t a);
    big_integer(uint32_t a);
    explicit big_integer(std::string const& str);
    ~big_integer() = default;

    big_integer& operator=(big_integer const &other) = default;

    big_integer& operator+=(big_integer const& rhs);
    big_integer& operator-=(big_integer const& rhs);
    big_integer& operator*=(big_integer const& rhs);
    big_integer& operator/=(big_integer const& rhs);
    big_integer& operator%=(big_integer const& rhs);

    big_integer& operator&=(big_integer const& rhs);
    big_integer& operator|=(big_integer const& rhs);
    big_integer& operator^=(big_integer const& rhs);

    big_integer& operator<<=(size_t shift);
    big_integer& operator>>=(size_t shift);

    big_integer operator+() const;
    big_integer operator-() const;
    big_integer operator~() const;

    big_integer& operator++();
    big_integer operator++(int);

    big_integer& operator--();
    big_integer operator--(int);

    friend bool operator==(big_integer const& a, big_integer const& b);
    friend bool operator!=(big_integer const& a, big_integer const& b);
    friend bool operator<(big_integer const& a, big_integer const& b);
    friend bool operator>(big_integer const& a, big_integer const& b);
    friend bool operator<=(big_integer const& a, big_integer const& b);
    friend bool operator>=(big_integer const& a, big_integer const& b);

    friend std::string to_string(big_integer const& a);

    //0 - младший бит
    std::vector<uint32_t> number;
    bool negative;
private:
    big_integer(std::vector<uint32_t> const &a);
    big_integer& add(big_integer const& rhs);
    big_integer& subtraction_larger(big_integer const& rhs);
    big_integer& subtraction_less(big_integer const& rhs);
    big_integer& subtraction(big_integer const& rhs);
    //big_integer& division(big_integer const& rhs);
    big_integer normalization(big_integer const & a);
    big_integer quotient(uint32_t b);
    big_integer remainder(uint32_t rhs);
    big_integer& delete_zeroes();
    //static uint32_t divide(uint32_t h, uint32_t &l, uint32_t b);
    //bool shift_larger(big_integer const& first, big_integer const& second, size_t shift);
    //big_integer& shift_subtraction(big_integer const& rhs, size_t shift);
    static std::string complement_to_base(std::string s);

    //big_integer& reverse();
    uint32_t bin_search(big_integer const &rhs, size_t pos);
    std::pair<big_integer, big_integer> division_with_remainder(big_integer const& rhs);
    std::pair<big_integer, big_integer> long_division(big_integer const& rhs);
    uint32_t abs_for_int32(int32_t a);
    uint32_t insert_or_add(size_t pos, uint64_t x);
    std::vector<uint32_t> negate(std::vector<uint32_t> const& v) const;
    void inc(std::vector<uint32_t> & v) const;
    std::vector<uint32_t> get_twos_complement(size_t size) const;
    void shift_subtract(big_integer const &rhs, size_t pos);
    bool shift_leq(big_integer const& rhs, size_t pos);

    void small_add(size_t pos, uint64_t add, uint32_t& carry);
    friend bool more_or_equal(big_integer const &a, big_integer const &b);
    friend void swap(big_integer& a, big_integer& b);
    //friend uint32_t trial(big_integer const &a, big_integer const &b, size_t pos);
};

big_integer operator+(big_integer a, big_integer const& b);
big_integer operator-(big_integer a, big_integer const& b);
big_integer operator*(big_integer a, big_integer const& b);
big_integer operator/(big_integer a, big_integer const& b);
big_integer operator%(big_integer a, big_integer const& b);

big_integer operator&(big_integer a, big_integer const& b);
big_integer operator|(big_integer a, big_integer const& b);
big_integer operator^(big_integer a, big_integer const& b);

big_integer operator<<(big_integer a, int b);
big_integer operator>>(big_integer a, int b);

bool operator==(big_integer const& a, big_integer const& b);
bool operator!=(big_integer const& a, big_integer const& b);
bool operator<(big_integer const& a, big_integer const& b);
bool operator>(big_integer const& a, big_integer const& b);
bool operator<=(big_integer const& a, big_integer const& b);
bool operator>=(big_integer const& a, big_integer const& b);

std::string to_string(big_integer const& a);
std::ostream& operator<<(std::ostream& s, big_integer const& a);

#endif // BIG_INTEGER_H
