#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <cstddef>
#include <iosfwd>
#include <vector>
#include "my_vector.h"

namespace big_integer_ns {
    //using vector = my_vector;
    using vector = std::vector<uint32_t>;

    const uint32_t MAX_UINT_32 = std::numeric_limits<uint32_t>::max();
    const uint64_t BASE = static_cast<uint64_t>(MAX_UINT_32) + 1;
    const uint32_t SHIFT_32 = 32;

    struct big_integer {
        //static const big_integer MAX_POW_10 = static_cast<uint32_t >(1e9);

        big_integer();

        big_integer(big_integer const &other) = default;

        big_integer(std::int32_t a);

        big_integer(std::uint32_t a);

        explicit big_integer(std::string const &str);

        ~big_integer() = default;

        big_integer &operator=(big_integer const &other) = default;

        big_integer &operator+=(big_integer const &rhs);

        big_integer &operator-=(big_integer const &rhs);

        big_integer &operator*=(big_integer const &rhs);

        big_integer &operator/=(big_integer const &rhs);

        big_integer &operator%=(big_integer const &rhs);

        big_integer &operator&=(big_integer const &rhs);

        big_integer &operator|=(big_integer const &rhs);

        big_integer &operator^=(big_integer const &rhs);

        big_integer &operator<<=(size_t shift);

        big_integer &operator>>=(size_t shift);

        big_integer operator+() const;

        big_integer operator-() const;

        big_integer operator~() const;

        big_integer &operator++();

        big_integer operator++(int);

        big_integer &operator--();

        big_integer operator--(int);

        friend bool operator==(big_integer const &a, big_integer const &b);

        friend bool operator!=(big_integer const &a, big_integer const &b);

        friend bool operator<(big_integer const &a, big_integer const &b);

        friend bool operator>(big_integer const &a, big_integer const &b);

        friend bool operator<=(big_integer const &a, big_integer const &b);

        friend bool operator>=(big_integer const &a, big_integer const &b);

        friend std::string to_string(big_integer const &a);

    private:
        vector number;
        bool negative;

        explicit big_integer(std::vector<std::uint32_t> const &a);

        big_integer &add(big_integer const &rhs);

        big_integer &subtraction_larger(big_integer const &rhs);

        big_integer &subtraction_less(big_integer const &rhs);

        big_integer &subtraction(big_integer const &rhs);

        big_integer quotient(std::uint32_t b);

        big_integer remainder(std::uint32_t rhs);

        big_integer &delete_zeroes();

        static std::string complement_to_base(std::string s);

        std::uint32_t bin_search(big_integer const &rhs, size_t pos);

        std::pair<big_integer, big_integer> division_with_remainder(big_integer const &rhs);

        std::pair<big_integer, big_integer> long_division(big_integer const &rhs);

        std::uint32_t abs_for_int32(std::int32_t a);

        std::uint32_t insert_or_add(size_t pos, uint64_t x);

        std::vector<std::uint32_t> negate(std::vector<std::uint32_t> const &v) const;

        void inc(std::vector<std::uint32_t> &v) const;

        std::vector<std::uint32_t> get_twos_complement(size_t size) const;

        void shift_subtract(big_integer const &rhs, size_t pos);

        bool shift_leq(big_integer const &rhs, size_t pos);

        void small_add(size_t pos, uint64_t add, std::uint32_t &carry);

        static bool more_or_equal(big_integer const &a, big_integer const &b);
    };

    big_integer operator+(big_integer a, big_integer const &b);

    big_integer operator-(big_integer a, big_integer const &b);

    big_integer operator*(big_integer a, big_integer const &b);

    big_integer operator/(big_integer a, big_integer const &b);

    big_integer operator%(big_integer a, big_integer const &b);

    big_integer operator&(big_integer a, big_integer const &b);

    big_integer operator|(big_integer a, big_integer const &b);

    big_integer operator^(big_integer a, big_integer const &b);

    big_integer operator<<(big_integer a, int b);

    big_integer operator>>(big_integer a, int b);

    bool operator==(big_integer const &a, big_integer const &b);

    bool operator!=(big_integer const &a, big_integer const &b);

    bool operator<(big_integer const &a, big_integer const &b);

    bool operator>(big_integer const &a, big_integer const &b);

    bool operator<=(big_integer const &a, big_integer const &b);

    bool operator>=(big_integer const &a, big_integer const &b);

    std::string to_string(big_integer const &a);

    std::ostream &operator<<(std::ostream &s, big_integer const &a);
}

#endif // BIG_INTEGER_H
