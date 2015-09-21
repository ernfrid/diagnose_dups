#pragma once

// Wait free ring buffer implementation

#include "common/BranchPrediction.hpp"

#include <boost/atomic.hpp>

#include <cassert>
#include <stdint.h>

#define CACHE_LINE_SIZE 64

template<typename T, typename SizeType = uint32_t>
struct RingBuffer {
    typedef SizeType size_type;
    typedef T value_type;

    RingBuffer(value_type* buf, size_type size)
        : data_(buf)
        , size_(size)
        , read_idx_(0)
        , write_idx_(0)
    {}

    template<typename OS>
    void dump_state(OS& os) {
        os << "size: " << size_ << "\n";
        os << "read index: " << read_idx_ << "\n";
        os << "write index: " << write_idx_ << "\n";
    }

    // all internal callers of read_index/write_index will specifiy the
    // appropriate memory order.
    // we default to sequential consistency for external callers.
    size_type read_index(boost::memory_order order = boost::memory_order_seq_cst) const {
        return read_idx_.load(order);
    }

    size_type write_index(boost::memory_order order = boost::memory_order_seq_cst) const {
        return write_idx_.load(order);
    }

    size_type write_buffer(value_type*& ptr) {
        size_type ridx = read_index(boost::memory_order_acquire);
        size_type widx = write_index(boost::memory_order_relaxed);
        if (UNLIKELY(widx == size_ && ridx > 0 && ridx <= widx)) {
            write_idx_.store(widx = 0, boost::memory_order_release);
        }


        ptr = data_ + widx;
        size_type end = ridx <= widx ? size_ : ridx - 1;
        size_type rv = end - widx;

        return rv;
    }

    size_type read_buffer(value_type*& ptr) {
        size_type ridx = read_index(boost::memory_order_relaxed);
        size_type widx = write_index(boost::memory_order_acquire);
        if (UNLIKELY(ridx == size_ && widx >= 0 && widx < ridx)) {
            read_idx_.store(ridx = 0, boost::memory_order_release);
        }

        ptr = data_ + ridx;
        size_type end = ridx <= widx ? widx : size_;
        size_type rv = end - ridx;

        return rv;
    }

    bool empty() const {
        size_type ridx = read_index(boost::memory_order_acquire);
        size_type widx = write_index(boost::memory_order_acquire);
        return ridx == widx;
    }

    void advance_write(size_type n) {
        size_type widx = write_index(boost::memory_order_relaxed);
        size_type upd = widx + n;
        assert(upd <= size_);
        write_idx_.store(upd, boost::memory_order_release);
    }

    void advance_read(size_type n) {
        size_type ridx = read_index(boost::memory_order_relaxed);
        size_type upd = ridx + n;
        assert(upd <= size_);
        read_idx_.store(upd, boost::memory_order_release);
    }

private:
    value_type* data_;
    size_type size_;
    char rpad[CACHE_LINE_SIZE - sizeof(size_type)];
    boost::atomic<size_type> read_idx_;
    char wpad[CACHE_LINE_SIZE - sizeof(size_type)];
    boost::atomic<size_type> write_idx_;
};
