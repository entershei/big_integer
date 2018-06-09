#ifndef BIGINT_MY_VECTOR_H
#define BIGINT_MY_VECTOR_H


#include <vector>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <cstring>
#include <iostream>

namespace {
    using value_type = std::uint32_t;
}

struct large_type {
    size_t *counter;
    value_type *data;
    size_t capacity;

    large_type() : counter(new size_t(1)), data(nullptr), capacity(0) {}

    explicit large_type(size_t capacity) : counter(new size_t(1)), capacity(capacity) {
        data = new value_type[capacity];
    }

    large_type(size_t capacity, value_type val) : large_type(capacity) {
        std::fill_n(data, capacity, val);
    }

    large_type(large_type const &other) : counter(other.counter), data(other.data), capacity(other.capacity) {
        ++(*counter);
    }

    large_type(value_type array[], size_t old_capacity, size_t new_capacity) : large_type(new_capacity) {
        std::copy(array, array + old_capacity, data);
    }

    large_type &operator=(large_type other) {
        swap(other);
        return *this;
    }

    ~large_type() {
        --(*counter);

        if (*counter == 0) {
            delete counter;
            delete[] data;
        }
    }

    bool unique() { return *counter == 1; }

    void swap(large_type &other) {
        using std::swap;

        swap(counter, other.counter);
        swap(data, other.data);
        swap(capacity, other.capacity);
    }

    value_type &operator[](size_t index) {
        return *(data + index);
    }

    value_type operator[](size_t index) const {
        return *(data + index);
    }

    void resize(size_t new_size) { //останется large_type
        if (new_size > capacity) {
            const size_t mul = 1;
            new_size = std::max(capacity * 2, new_size);
            auto *cur = new value_type[new_size * mul];
            std::copy(data, data + capacity, cur);
            capacity = new_size * mul;
            std::swap(data, cur);
            delete[] cur;
        }
    }
};

const size_t size_small_type = sizeof(large_type) / sizeof(value_type);
using small_type = value_type[size_small_type];

struct my_vector {
    using iterator  = value_type *;
    using const_iterator  = value_type const *;
    using pointer = value_type *;
    using reference = value_type &;
    using const_reference = value_type const &;
    using const_pointer = value_type const *;

    my_vector();

    my_vector(my_vector const &other);

    my_vector(std::initializer_list<value_type> other);

    explicit my_vector(size_t size, value_type value = 0);

    ~my_vector();

    void resize(size_t i, value_type val = 0);

    size_t size() const;

    value_type &operator[](size_t i);

    value_type operator[](size_t i) const;

    my_vector &operator=(my_vector other);

    value_type &back();

    value_type back() const;

    void pop_back();

    void push_back(value_type val);

    const_pointer data() const {
        if (small()) {
            return small_value;
        } else {
            return large_value.data;
        }
    }

    pointer data() {
        make_unique();

        if (small()) {
            return small_value;
        } else {
            return large_value.data;
        }
    }

    const_iterator begin() const {
        return data();
    }

    iterator begin() {
        return data();
    }

    const_iterator end() const {
        return begin() + size();
    }

    iterator end() {
        return begin() + size();
    }

    friend void swap(my_vector& a, my_vector& b);

private:
    union {
        small_type small_value;
        large_type large_value;
    };

    size_t size_;

    bool is_small;

    bool small() const {
        return is_small;
    }

    void make_unique() {
        if (!small()) {
            if (!large_value.unique()) {
                large_value = large_type(large_value.data, large_value.capacity, large_value.capacity);
            }
        }
    }
};

bool operator==(my_vector const &a, my_vector const &b);

void swap(my_vector& a, my_vector& b);

#endif //BIGINT_OPTVECTOR_H