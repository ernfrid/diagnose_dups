#pragma once

#include "SamReader.hpp"
#include "BamRecord.hpp"
#include "common/RingBuffer.hpp"

#include <boost/atomic.hpp>

#include <stdint.h>

class SamReaderThread {
public:
    SamReaderThread(SamReader& reader, uint32_t ring_size)
        : reader_(reader)
        , running_(true)
        , buf_(new BamRecord[ring_size])
        , ring_(buf_, ring_size)
    {
    }

    ~SamReaderThread() {
        stop();
        delete[] buf_;
    }

    void operator()() {
        BamRecord* ptr = 0;
        uint32_t n = 0;
        while (running_.load(boost::memory_order_acquire)) {

            if ((n = ring_.write_buffer(ptr)) == 0)
                continue; // ring is full

            assert(ptr);

            for (uint32_t i = 0; i < n; ++i) {
                if (!reader_.next(ptr[i])) {
                    stop();
                    break;
                }
                ring_.advance_write(1);
            }
        }
    }

    RingBuffer<BamRecord>& ring() {
        return ring_;
    }

    void stop() {
        running_.store(false, boost::memory_order_release);
    }

    bool running() const {
        return running_.load(boost::memory_order_acquire);
    }

private:
    SamReader& reader_;
    boost::atomic<bool> running_;
    BamRecord* buf_;
    RingBuffer<BamRecord> ring_;
};
