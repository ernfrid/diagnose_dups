#pragma once

#include "BamRecord.hpp"
#include "SamReader.hpp"
#include "common/RingBuffer.hpp"

#include <boost/noncopyable.hpp>
#include <boost/ref.hpp>
#include <boost/thread.hpp>

#include <stdint.h>

#include <iostream>

class SamProducerBase {
public:
    virtual ~SamProducerBase() {}

    // records is set to an array of BamRecord objects
    // the return value is the length of the array
    virtual uint32_t next_chunk(BamRecord*& records) = 0;
    virtual bool eof() const = 0;

    virtual void advance(uint32_t) {}
    virtual void stop() {}

    template<typename Func>
    void for_each_record(Func& f) {
        BamRecord* records = 0;
        while (!eof()) {
            uint32_t n = next_chunk(records);
            for (uint32_t i = 0; i < n; ++i) {
                f(records[i]);
            }
            advance(n);
        }
    }
};

class SamProducer : public SamProducerBase {
public:
    SamProducer(SamReader& reader)
        : reader_(reader)
        , eof_(false)
    {}

    uint32_t next_chunk(BamRecord*& records) {
        if (reader_.next(record_)) {
            records = &record_;
            return 1;
        }
        else {
            records = 0;
            eof_ = true;
            return 0;
        }
    }

    void advance(uint32_t n) {}
    bool eof() const { return eof_; }

private:
    SamReader& reader_;
    bool eof_;
    BamRecord record_;
};

class SamProducerThread : public SamProducerBase, public boost::noncopyable {
public:
    SamProducerThread(SamReader& reader, uint32_t ring_size)
        : reader_(reader)
        , running_(true)
        , buf_(new BamRecord[ring_size])
        , ring_(buf_, ring_size)
        , busywaits_(0)
        , thread_(boost::ref(*this))
    {
    }

    ~SamProducerThread() {
        stop();
        thread_.join();
        delete[] buf_;
    }

    uint32_t next_chunk(BamRecord*& records) {
        return ring_.read_buffer(records);
    }

    void advance(uint32_t n) {
        ring_.advance_read(n);
    }

    bool eof() const {
        return !running() && ring_.empty();
    }

    void operator()() {
        BamRecord* ptr = 0;
        uint32_t n = 0;
        while (running_.load(boost::memory_order_acquire)) {

            if ((n = ring_.write_buffer(ptr)) == 0) {
                ++busywaits_;
                continue; // ring is full
            }

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

    uint64_t busywaits() const {
        return busywaits_;
    }

private:
    SamReader& reader_;
    boost::atomic<bool> running_;
    BamRecord* buf_;
    RingBuffer<BamRecord> ring_;
    uint64_t busywaits_;
    boost::thread thread_;
};
