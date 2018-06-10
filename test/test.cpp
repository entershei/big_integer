//
// Created by Ira_F on 10.06.2018.
//

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <vector>
#include <utility>
#include <cstring>
#include <sstream>
#include "../lib/compression.h"
#include "../gtest/gtest.h"

bool encode_and_decode(std::string s) {
    std::istringstream encode_in(s);
    std::ostringstream encode_out;
    compression::encode(encode_in, encode_out);
    std::istringstream decode_in(encode_out.str());
    std::ostringstream decode_out;
    compression::decode(decode_in, decode_out);

    return (s == decode_out.str());
}

std::string random_string(size_t sz) {
    std::string ret;

    for (size_t i = 0; i < sz; ++i) {
        ret += char(rand() % 256);
    }

    return ret;
}

TEST(correctness, empty) {
    std::string s;
    EXPECT_EQ(encode_and_decode(s), true);
}

TEST(correctness, zero) {
    std::string s{'0'};
    EXPECT_EQ(encode_and_decode(s), true);
}

TEST(correctness, words) {
    std::string s("Home Work ");
    EXPECT_EQ(encode_and_decode(s), true);
}

TEST(correctness, large_string) {
    std::string s = random_string(100000);
    EXPECT_EQ(encode_and_decode(s), true);
}

TEST(correctness, big_random_test) {
    for (size_t k = 0; k < 10; ++k) {
        for (size_t len = 0; len < 1000; ++len) {
            std::string s = random_string(len);

            EXPECT_EQ(encode_and_decode(s), true);
        }
    }
}

