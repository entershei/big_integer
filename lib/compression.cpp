#include <vector>
#include <algorithm>
#include <iostream>
#include <set>
#include <cstring>
#include "compression.h"

using char_t = unsigned char;

const size_t ALPHABET = 256;

struct buffer_input_stream {
    static constexpr size_t BUFFER_SIZE = 1U << 15;
    char buffer[BUFFER_SIZE];
    size_t pos;
    std::istream &in;
    size_t size;
    size_t pos_in_char;
    uint64_t cnt_in_bit;

    explicit buffer_input_stream(std::istream &in) : in(in), pos_in_char(0), cnt_in_bit(0) {
        buf_update();
    }

    void buf_update() {
        pos = 0;
        in.read(buffer, BUFFER_SIZE);
        size = static_cast<size_t >(in.gcount());
        pos_in_char = 0;
    }

    bool can_read_char() {
        if (pos < size) { return true; }

        buf_update();
        return pos < size;
    }

    bool read_char(char_t &symbol) {
        if (!can_read_char()) { return false; }
        symbol = static_cast<char_t>(buffer[pos]);
        ++pos;
        pos_in_char = 0;

        return true;
    }

    bool read_bit(bool &bit) {
        if (pos == size) {
            buf_update();
        }
        if (pos == size) {
            return false;
        }

        bit = static_cast<bool>(static_cast<char_t>(buffer[pos]) & (1U << (7 - pos_in_char)));
        ++pos_in_char;

        if (pos_in_char == 8) {
            pos_in_char = 0;
            ++pos;
        }

        return true;
    }

    void refresh() {
        in.clear();
        in.seekg(0);
    }
};

struct buffer_output_stream {
    static constexpr size_t BUFFER_SIZE = 1U << 15;
    char buffer[BUFFER_SIZE];
    size_t pos;
    std::ostream &out;
    size_t pos_in_char;

    buffer_output_stream(std::ostream &out) : pos(0), out(out), pos_in_char(0) {
        memset(buffer, 0, sizeof buffer);
    }

    void write_from_buffer() {
        out.write(buffer, pos);
        memset(buffer, 0, sizeof buffer);
        pos = 0;
        pos_in_char = 0;
    }

    void write_char(char symbol) {
        if (pos == BUFFER_SIZE) {
            write_from_buffer();
        }

        buffer[pos] = symbol;
        ++pos;
        pos_in_char = 0;
    }

    void write_bit(bool bit) {
        if (pos == BUFFER_SIZE) {
            write_from_buffer();
        }

        buffer[pos] |= bit << (7 - pos_in_char);
        ++pos_in_char;

        if (pos_in_char == 8) {
            pos_in_char = 0;
            ++pos;
        }
    }

    ~buffer_output_stream() {
        if (pos_in_char) {
            ++pos;
        }
        write_from_buffer();
    }
};

std::vector<size_t> count_frequency(buffer_input_stream &bin) {
    std::vector<size_t> frequency(ALPHABET);
    char_t symbol;

    while (bin.read_char(symbol)) {
        ++frequency[symbol];
    }

    bin.refresh();

    return frequency;
}

struct node {
    int left;
    int right;

    node() : left(-1), right(-1) {}

    node(int left, int right) : left(left), right(right) {}
};

bool cmp(std::pair<size_t, size_t> const &a, std::pair<size_t, size_t> const &b) {
    return a.second < b.second;
}

std::vector<node> build_huffman_tree(std::vector<size_t> const &frequency) {
    std::vector<node> tree(ALPHABET * 2 - 1);
    std::set<std::pair<size_t, size_t> > freq; //второе - индекс в tree

    for (size_t i = 0; i < ALPHABET; ++i) {
        freq.insert({frequency[i], i});
    }

    size_t cnt_nodes = ALPHABET;
    while (freq.size() > 1) {
        auto node1 = *freq.begin();
        freq.erase(freq.begin());
        auto node2 = *freq.begin();
        freq.erase(freq.begin());
        tree[cnt_nodes] = {static_cast<int>(node1.second), static_cast<int>(node2.second)};
        freq.insert({node1.first + node2.first, cnt_nodes});
        ++cnt_nodes;
    }

    return tree;
}

void count_length(size_t pos, char_t depth, std::vector<node> const &tree, std::vector<char_t> &length) {
    if (tree[pos].left == -1 && tree[pos].right == -1) {
        length[pos] = depth;
        return;
    }

    if (tree[pos].left != -1) {
        count_length(static_cast<size_t>(tree[pos].left), depth + 1, tree, length);
    }
    if (tree[pos].right != -1) {
        count_length(static_cast<size_t>(tree[pos].right), depth + 1, tree, length);
    }
}

std::vector<size_t> count_key(std::vector<char_t> const &len) {
    std::vector<std::pair<size_t, size_t> > length(ALPHABET);

    for (size_t i = 0; i < ALPHABET; ++i) {
        length[i] = {i, len[i]};
    }

    sort(length.begin(), length.end(), cmp);

    std::vector<size_t> code(ALPHABET);
    size_t cur_code = 0;
    code[length[0].first] = 0;

    for (size_t i = 1; i < ALPHABET; ++i) {
        ++cur_code;

        if (length[i - 1].second < length[i].second) {
            cur_code <<= (length[i].second - length[i - 1].second);
        }

        code[length[i].first] = cur_code;
    }

    return code;
}

void write_cnt_bit(buffer_output_stream &out, uint64_t cnt) {
    const size_t BASE = (1U << 8) - 1;
    for (size_t i = 8; i-- > 0;) {
        out.write_char(static_cast<char_t>((cnt >> (i * 8)) & BASE));
    }
}

void write_length(buffer_output_stream &out, std::vector<char_t> const& length) {
    for (size_t i = 0; i < ALPHABET; ++i) {
        out.write_char(length[i]);
    }
}

void write_key(buffer_output_stream &out, std::vector<size_t> const& key, buffer_input_stream &in,
               std::vector<char_t> const &length) {
    char_t symbol;
    while (in.read_char(symbol)) {
        for (size_t j = length[symbol]; j-- > 0;) {
            out.write_bit(static_cast<bool>(key[symbol] & (1 << j)));
        }
    }
}

void compression::encode(std::istream &in, std::ostream &out) {
    buffer_input_stream buf_in(in);
    buffer_output_stream buf_out(out);

    std::vector<size_t> frequency = count_frequency(buf_in);

    std::vector<node> tree = build_huffman_tree(frequency);

    std::vector<char_t> length(ALPHABET);
    count_length(tree.size() - 1, 0, tree, length);

    uint64_t counter_bit = 0;
    for (size_t i = 0; i < length.size(); ++i) {
        counter_bit += length[i] * frequency[i];
    }

    std::vector<size_t> key = count_key(length);

    write_cnt_bit(buf_out, counter_bit);
    write_length(buf_out, length);
    write_key(buf_out, key, buf_in, length);
}

uint64_t read_cnt_bit( buffer_input_stream &buf_in) {
    uint64_t ret = 0;

    for (size_t i = 8; i-- > 0;) {
        char_t cur;
        if (!buf_in.read_char(cur)) {
            throw std::invalid_argument("can't decode this file\n");
        }
        ret |= static_cast<uint64_t>(cur) << (i * 8);
    }

    return ret;
}

std::vector<char_t> read_length(buffer_input_stream &buf_in) {
    std::vector<char_t> length(ALPHABET);
    for (size_t i = 0; i < ALPHABET; ++i) {
        if (!buf_in.read_char(length[i])) {
            throw std::invalid_argument("can't decode this file\n");
        }
    }

    return length;
}

struct node_for_bor {
    int go[2];
    bool term;
    char c;

    node_for_bor() : term(false), c(0) {
        go[0] = go[1] = -1;
    }

    node_for_bor(size_t l, size_t r) : term(false), c(0) {
        go[0] = static_cast<int>(l);
        go[1] = static_cast<int>(r);
    }

    node_for_bor(char c, size_t l, size_t r) : term(true), c(c) {
        go[0] = static_cast<int>(l);
        go[1] = static_cast<int>(r);
    }
};

std::vector<node_for_bor> build_bor(std::vector<size_t> &key, std::vector<char_t> &length) {
    std::vector<node_for_bor> bor(1);

    for (size_t i = 0; i < ALPHABET; ++i) {
        int pos = 0;
        for (size_t j = length[i]; j-- > 0;) {
            bool c = static_cast<bool>(key[i] & (1 << j));

            int to = bor[pos].go[c];

            if (to == -1) {
                to = static_cast<int>(bor.size());
                bor.push_back(node_for_bor());
                bor[pos].go[c] = to;
            }

            pos = to;
        }
        bor[pos].term = true;
        bor[pos].c = static_cast<char>(i);
    }

    return bor;
}

void compression::decode(std::istream &in, std::ostream &out) {
    buffer_input_stream buf_in(in);
    buffer_output_stream buf_out(out);

    uint64_t cnt_bit = read_cnt_bit(buf_in);
    std::vector<char_t> length = read_length(buf_in);

    std::vector<size_t> key = count_key(length);

    std::vector<node_for_bor> bor = build_bor(key, length);

    int pos_bor = 0;
    bool cur_bit;

    while (cnt_bit && buf_in.read_bit(cur_bit)) {
        --cnt_bit;
        pos_bor = bor[pos_bor].go[cur_bit];

        if (pos_bor == -1) {
            throw std::invalid_argument("can't decode this file\n");
        } else if (bor[pos_bor].term) {
            buf_out.write_char(bor[pos_bor].c);
            pos_bor = 0;
        }
    }
}