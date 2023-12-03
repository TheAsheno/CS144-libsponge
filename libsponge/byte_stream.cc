#include "byte_stream.hh"

#include <algorithm>
#include <iterator>
#include <stdexcept>

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t capacity) : capacity_count(capacity) {}

size_t ByteStream::write(const string &data) {
    size_t len = data.length();
    if (len > capacity_count - buffer.size()) {
        len = capacity_count - buffer.size();
    }
    write_count += len;
    for (size_t i = 0; i < len; i++) {
        buffer.push_back(data[i]);
    }
    return len;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    size_t length = len;
    if (length > buffer.size()) {
        length = buffer.size();
    }
    return string().assign(buffer.begin(), buffer.begin() + length);
    return {};
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    size_t length = len;
    if (length > buffer.size()) {
        length = buffer.size();
    }
    read_count += length;
    while (length--) {
        buffer.pop_front();
    }
}

void ByteStream::end_input() {input_end_flag = true;}

bool ByteStream::input_ended() const { return input_end_flag; }

size_t ByteStream::buffer_size() const { return buffer.size(); }

bool ByteStream::buffer_empty() const { return buffer.empty(); }

bool ByteStream::eof() const { return buffer.empty() && input_ended(); }

size_t ByteStream::bytes_written() const { return write_count; }

size_t ByteStream::bytes_read() const { return read_count; }

size_t ByteStream::remaining_capacity() const { return capacity_count - buffer.size(); }
