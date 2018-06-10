#include <iostream>
#include <cstring>
#include <fstream>
#include <ctime>
#include <iomanip>
#include "lib/compression.h"

bool correct_arg(int argc, char **argv) {
    if (argc != 3 && argc != 4) {
        return false;
    }

    if (std::strcmp(argv[1], "-c") != 0 && std::strcmp(argv[1], "-d") != 0) {
        return false;
    }

    return true;
}

int main(int argc, char **argv) {
    std::clock_t time = clock();

    if (!correct_arg(argc, argv)) {
        std::cout << "Wrong arguments, correct usage is:\n" << argv[0] << " -c/-d input [output]\n";
        return 0;
    }

    std::ifstream input(argv[2], std::ios_base::binary | std::ios_base::in);
    std::ofstream output;
    if (argc == 4) {
        output = std::ofstream(argv[3], std::ios_base::binary | std::ios_base::out);
    } else {
        output = std::ofstream(std::string(argv[2]) + ".out", std::ios_base::binary | std::ios_base::out);
        std::cout << std::string(argv[2]) + ".out\n";
    }

    if (std::strcmp(argv[1], "-c") == 0) {
        compression::encode(input, output);
    } else if (std::strcmp(argv[1], "-d") == 0) {
        try {
            compression::decode(input, output);
        } catch (...) {
            std::cout << "Can't decode this file " << argv[3] << "\n";
            return 0;
        }
    }

    std::cerr << "time: " << std::setprecision(3) << double(clock() - time) / CLOCKS_PER_SEC << "s.\n";

    return 0;
}