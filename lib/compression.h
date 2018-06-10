#ifndef COMPRESSION_OF_HUFFMAN_COMPRESSION_H
#define COMPRESSION_OF_HUFFMAN_COMPRESSION_H

#include <istream>
#include <ostream>

namespace compression    {
    void encode(std::istream &in, std::ostream &out);

    void decode(std::istream &in, std::ostream &out);
}

#endif //COMPRESSION_OF_HUFFMAN_COMPRESSION_H
