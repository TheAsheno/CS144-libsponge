#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) : _output(capacity), _capacity(capacity) {
    buffer.resize(capacity);
}

long StreamReassembler::merge_block(Node &elm1, const Node &elm2) {
    Node x, y;
    if (elm1.begin > elm2.begin) {
        x = elm2;
        y = elm1;
    }
    else {
        x = elm1;
        y = elm2;
    }
    if (x.begin + x.length < y.begin)
        return -1;
    else if (x.begin + x.length >= y.begin + y.length) {
        elm1 = x;
        return y.length;
    }
    else {
        elm1.begin = x.begin;
        elm1.data = x.data + y.data.substr(x.begin + x.length - y.begin);
        elm1.length = elm1.data.length();
        return x.begin + x.length - y.begin;
    }
}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    if (index > head_index + _capacity)
        return;
    Node elm;
    if (index + data.length() >= head_index) {

        if (index < head_index) {
            size_t offset = head_index - index;
            elm.data.assign(data.begin() + offset, data.end());
            elm.begin = head_index;
            elm.length = elm.data.length();
        }
        else {
            elm.data = data;
            elm.begin = index;
            elm.length = elm.data.length();
        }
        unassembled_byte += elm.length;
        long merged_bytes = 0;
        set<Node>::iterator it = blocks.lower_bound(elm);
        while (it != blocks.end() && (merged_bytes = merge_block(elm, *it)) >= 0) {
            unassembled_byte -= merged_bytes;
            blocks.erase(it);
            it = blocks.lower_bound(elm);
        }
        if (it != blocks.begin()) {
            it--;
            while ((merged_bytes = merge_block(elm, *it)) >= 0) {
                unassembled_byte -= merged_bytes;
                blocks.erase(it);
                it = blocks.lower_bound(elm);
                if (it == blocks.begin())
                    break;
                it --;
            }
        }
        blocks.insert(elm);
        if (!blocks.empty() && blocks.begin()->begin == head_index) {
            const Node head_block = *blocks.begin();
            size_t write_bytes = _output.write(head_block.data);
            head_index += write_bytes;
            unassembled_byte -= write_bytes;
            blocks.erase(blocks.begin());
        }
    }
    if (eof) {
        eof_flag = true;
    }
    if (eof_flag && empty()) {
        _output.end_input();
    }
}

size_t StreamReassembler::unassembled_bytes() const { return unassembled_byte; }

bool StreamReassembler::empty() const { return unassembled_byte == 0; }
