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
    return *(data() + ind);
}

value_type &my_vector::operator[](size_t ind) {
    return *(data() + ind);
}

value_type &my_vector::back() {
    return *(data() + size_ - 1);
}

value_type my_vector::back() const {
    return *(data() + size_ - 1);
}

size_t my_vector::size() const {
    return size_;
}

void swap(my_vector &a, my_vector &b) {
    using std::swap;

    if (a.is_small && b.is_small) {
        swap(a.small_value, b.small_value);
    } else if (!a.is_small && !b.is_small) {
        a.large_value.swap(b.large_value);
    } else if (a.is_small && !b.is_small) {
        small_type last_small;
        std::copy(a.begin(), a.end(), last_small);
        new(&a.large_value) large_type(b.large_value);
        std::copy(last_small, last_small + a.size_, b.begin());
        a.is_small = b.is_small = false;
    } else {
        small_type last_small;
        std::copy(b.begin(), b.end(), last_small);
        new(&b.large_value) large_type(a.large_value);
        std::copy(last_small, last_small + b.size(), a.begin());
        a.is_small = b.is_small = false;
    }
    swap(a.size_, b.size_);
}

my_vector &my_vector::operator=(my_vector other) {
    swap(*this, other);
    return *this;
}

void my_vector::resize(size_t new_size, value_type val) {
    if (is_small) {
        if (new_size > size_small_type) {
            small_type last_data;
            std::copy(begin(), end(), last_data);
            is_small = false;
            new(&large_value) large_type(new_size, val);

            std::copy(last_data, last_data + size_, large_value.data);
        } else {
            if (new_size > size_) {
                std::fill_n(small_value + size_, new_size - size_, val);
            }
        }
    } else {
        make_unique();

        large_value.resize(new_size);
        std::fill_n(begin() + size_, std::max(new_size, size_) - size_, val);
    }

    size_ = new_size;
}

void my_vector::pop_back() {
    --size_;
}

void my_vector::push_back(value_type val) {
    resize(size_ + 1, val);
}

bool operator==(my_vector const &a, my_vector const &b) {
    return std::equal(a.begin(), a.end(), b.begin(), b.end());
}