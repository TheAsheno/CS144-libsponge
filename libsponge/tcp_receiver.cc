#include "tcp_receiver.hh"
#include<iostream>

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

bool TCPReceiver::segment_received(const TCPSegment &seg) {
    int length = seg.length_in_sequence_space();
    uint64_t abs_seqno = 0;
    if (seg.header().syn) {
        if (syn_flag)
            return false;
        syn_flag = true;
        isn = seg.header().seqno.raw_value();
        abs_seqno = 1;
        base = 1;
        if (length - 1 == 0) 
            return true;
    }
    else if (!syn_flag) {
        return false;
    }
    else {
        abs_seqno = unwrap(seg.header().seqno, WrappingInt32(isn), abs_seqno);
    }
    if (seg.header().fin) {
        if (fin_flag)
            return false;
        fin_flag = true;
    }
    if (length == 0) {
        return true;
    }
    else if (abs_seqno >= base + window_size() || abs_seqno + length <= base) {
        if (!seg.header().syn && !seg.header().fin)
            return false;
    }
    _reassembler.push_substring(seg.payload().copy(), abs_seqno - 1, seg.header().fin);
    base = _reassembler.get_head() + 1;
    if (_reassembler.input_ended())
        base++;
    return true;
}

optional<WrappingInt32> TCPReceiver::ackno() const {
    if (base > 0)
        return WrappingInt32(wrap(base, WrappingInt32(isn)));
    else
        return std::nullopt;

}

size_t TCPReceiver::window_size() const { return _capacity - _reassembler.stream_out().buffer_size(); }
