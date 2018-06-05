//
// Created by Ira_F on 31.05.2018.
//
#include <cassert>
#include "my_vector.h"

my_vector::my_vector() : size_(0), is_small(true) {}

my_vector::my_vector(my_vector const &other) : size_(other.size_), is_small(other.is_small) {
    if (small()) {
        std::copy(other.begin(), other.end(), begin());
    } else {
        new (&large_value) large_type(other.large_value);
    }
}

my_vector::my_vector(std::initializer_list<value_type> other) : my_vector(other.size()) {
    std::copy(other.begin(), other.end(), data());
}

my_vector::my_vector(size_t size, value_type value) : my_vector() {
    resize(size, value);
}

my_vector::~my_vector() {
    if (!small()) {
        large_value.~large_type();
    }
}

value_type my_vector::operator[](size_t ind) const {
    assert(ind < size() && "[i], but i < size()");
    return *(data() + ind);
}

value_type &my_vector::operator[](size_t ind) {
    assert(ind < size() && "[i], but i < size()");
    return *(data() + ind);
}

value_type &my_vector::back() {
    return this->operator[](size() - 1);
}

value_type my_vector::back() const {
    return this->operator[](size() - 1);
}

size_t my_vector::size() const {
    return size_;
}

void my_vector::swap(my_vector &b) {
    using std::swap;

    if (small() && b.small()) {
        swap(small_value, b.small_value);
    } else if (!small() && !b.small()) {
        large_value.swap(b.large_value);
    } else if (small() && !b.small()) {
        small_type last_small;
        std::copy(begin(), end(), last_small);
        new(&large_value) large_type(b.large_value);
        std::copy(last_small, last_small + size(), b.begin());
        is_small = false;
        b.is_small = false;
    } else {
        small_type last_small;
        std::copy(b.begin(), b.end(), last_small);
        new(&b.large_value) large_type(large_value);
        std::copy(last_small, last_small + b.size(), begin());
        b.is_small = false;
        is_small = false;
    }
    swap(size_, b.size_);
}

my_vector &my_vector::operator=(my_vector other) {
    this->swap(other);
    return *this;
}

void my_vector::resize(size_t new_size, value_type val) {
    if (small()) {
        if (new_size > size_small_type) {
            small_type last_data;
            std::copy(begin(), end(), last_data);
            is_small = false;
            new(&large_value) large_type(new_size, val);

            std::copy(last_data, last_data + size(), large_value.data);
        } else {
            if (new_size > size()) {
                std::fill_n(small_value + size(), new_size - size(), val);
            }
        }
    } else {
        make_unique();
        large_value.resize(new_size);
        std::fill_n(begin() + size(), std::max(new_size, size()) - size(), val);
    }

    size_ = new_size;
}

value_type my_vector::pop_back() {
    assert(size() > 0 && "pop_back(), but size_ = 0");

    value_type res = back();

    resize(size() - 1);
    return res;
}

void my_vector::push_back(value_type val) {
    resize(size() + 1, val);
}

my_vector &my_vector::copy_of_std_vector(std::vector<value_type> const &other) {
    make_unique();
    resize(other.size());

    if (small()) {
        for (size_t i = 0; i < size(); ++i) {
            small_value[i] = other[i];
        }
    } else {
        for (size_t i = 0; i < size(); ++i) {
            large_value[i] = other[i];
        }
    }

    return *this;
}

bool operator==(my_vector const &a, my_vector const &b) {
    return std::equal(a.begin(), a.end(), b.begin(), b.end());
}
