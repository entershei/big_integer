cmake_minimum_required(VERSION 2.8)

project(big-integer)

include_directories(.)

add_executable(Compression_of_Huffman main.cpp)
add_executable(Compression_of_Huffman_testing
        lib/compression.cpp
        lib/compression.h
        test/test.cpp
        gtest/gtest.h
        gtest/gtest-all.cc
        gtest/gtest_main.cc
        )

add_library(compression SHARED lib/compression.h lib/compression.cpp)

target_link_libraries(Compression_of_Huffman compression)
target_link_libraries(Compression_of_Huffman_testing compression -lpthread)

add_executable(big_integer_testing
        big_integer_testing.cpp
        big_integer.h
        big_integer.cpp
        gtest/gtest-all.cc
        gtest/gtest.h
        gtest/gtest_main.cc
        my_vector.cpp
        my_vector.h)

add_executable(MyIrinasSetup
        big_integer.h
        big_integer.cpp
        mytestik.cpp
        my_vector.cpp
        my_vector.h)


if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++17 -pedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")
endif ()

target_link_libraries(big_integer_testing -lgmpxx -lpthread)
