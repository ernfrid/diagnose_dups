#include "common/RingBuffer.hpp"

#include <boost/atomic.hpp>
#include <boost/ref.hpp>
#include <boost/thread.hpp>
#include <gtest/gtest.h>

#include <stdexcept>

TEST(TestRingBufferManually, read_buffer) {
    uint32_t const size = 32;
    std::vector<uint32_t> buf(size);
    RingBuffer<uint32_t> ring(&buf[0], size);
    uint32_t* ptr = 0;

    uint32_t n = ring.read_buffer(ptr);
    ASSERT_EQ(0u, n);
    ASSERT_EQ(buf.data(), ptr);

    // assertions about where the read/write indices are are tabbed
    // so they stick out
    ring.advance_write(1);
        ASSERT_EQ(1u, ring.write_index());
        ASSERT_EQ(0u, ring.read_index());
    n = ring.read_buffer(ptr);
    ASSERT_EQ(1u, n);
    ASSERT_EQ(buf.data(), ptr);

    ring.advance_write(2);
        ASSERT_EQ(3u, ring.write_index());
        ASSERT_EQ(0u, ring.read_index());
    n = ring.read_buffer(ptr);
    ASSERT_EQ(3u, n);
    ASSERT_EQ(buf.data(), ptr);

    ring.advance_write(size - 2 - 1);
        ASSERT_EQ(32u, ring.write_index());
        ASSERT_EQ(0u, ring.read_index());
    n = ring.read_buffer(ptr);
    ASSERT_EQ(size, n);
    ASSERT_EQ(buf.data(), ptr);

    ring.advance_read(1);
        ASSERT_EQ(32u, ring.write_index());
        ASSERT_EQ(1u, ring.read_index());
    n = ring.read_buffer(ptr);
    ASSERT_EQ(size - 1, n);
    ASSERT_EQ(buf.data() + 1, ptr);

    ring.advance_read(2);
        ASSERT_EQ(32u, ring.write_index());
        ASSERT_EQ(3u, ring.read_index());
    n = ring.read_buffer(ptr);
    ASSERT_EQ(size - 3, n);
    ASSERT_EQ(buf.data() + 3, ptr);

    n = ring.write_buffer(ptr);
    ASSERT_EQ(2u, n);
    // ^ We have 2 free slots at the beginning of the ring
    ASSERT_EQ(buf.data(), ptr);
    ring.advance_write(2);

    ring.advance_read(size - 3);
        ASSERT_EQ(2u, ring.write_index());
        ASSERT_EQ(32u, ring.read_index());
    n = ring.read_buffer(ptr);
        ASSERT_EQ(0u, ring.read_index()); // read wraps back to the beginning
    ASSERT_EQ(2u, n);
    ASSERT_EQ(buf.data(), ptr);

    n = ring.write_buffer(ptr);
    ASSERT_EQ(30u, n);
    ring.advance_write(30u);
        ASSERT_EQ(32u, ring.write_index());
        ASSERT_EQ(0u, ring.read_index());

    n = ring.read_buffer(ptr);
    ASSERT_EQ(32u, n);
    ring.advance_read(32u);
        ASSERT_EQ(32u, ring.write_index());
        ASSERT_EQ(32u, ring.read_index());
    ASSERT_TRUE(ring.empty());

    n = ring.write_buffer(ptr);
    ASSERT_EQ(31u, n);
    // ^ The read ptr hasn't wrapped yet. calling read_buffer() will wrap it
    // The motivation for doing things this way is efficiency:
    //  if we wrap the read/write ptrs in the advance functions, then we'd
    //  have to do atomic loads on both pointers. If we do it in the get buffer
    //  functions, we already have to do those loads there.
        ASSERT_EQ(0u, ring.write_index());
        ASSERT_EQ(32u, ring.read_index());

    n = ring.read_buffer(ptr);
        ASSERT_EQ(0u, ring.write_index());
        ASSERT_EQ(0u, ring.read_index());

    ring.advance_write(1u);
        ASSERT_EQ(1u, ring.write_index());
        ASSERT_EQ(0u, ring.read_index());
    n = ring.read_buffer(ptr);
    ASSERT_EQ(1u, n);
}


namespace {
template<typename RingType>
struct test_threads {
    typedef typename RingType::value_type value_type;
    typedef typename RingType::size_type size_type;

    struct producer_thread {
        producer_thread(RingType& ring)
            : ring_(ring)
            , size_idx_(0)
            , counter_(0)
        {}

        void iteration() {
            value_type* wptr;
            size_type size;
            while ((size = ring_.write_buffer(wptr)) == 0)
                ;

            for (size_type i = 0; i < size; ++i) {
                *wptr++ = counter_++;
                ring_.advance_write(1);
            }
        }

        void operator()(int iterations) {
            for (int i = 0; i < iterations; ++i) {
                iteration();
            }
        }

        RingType& ring_;
        std::size_t size_idx_;
        value_type counter_;
    };

    struct consumer_thread {
        consumer_thread(RingType& ring)
            : ring_(ring)
            , running_(true)
            , counter_(0)
        {
        }

        void stop() {
            running_ = false;
        }

        void drain_buffer() {
            value_type* rptr;
            size_type size = 0;

            while ((size = ring_.read_buffer(rptr)) > 0) {
                for (size_type i = 0; i < size; ++i, ++counter_, ++rptr) {
                    value_type actual = *rptr;
                    if (counter_ != actual) {
                        std::cerr << "Error: expected " << counter_ << ", got "
                            << actual << "\n";
                        ring_.dump_state(std::cerr);
                        throw std::runtime_error("bang");
                    }
                    ring_.advance_read(1);
                }
            }
        }

        void operator()() {
            while (running_) {
                drain_buffer();
            }
            drain_buffer();
        }

        RingType& ring_;
        boost::atomic<bool> running_;
        value_type counter_;
    };

};
}

struct TestRingBuffer : ::testing::TestWithParam<uint32_t> {
    TestRingBuffer()
        : size(GetParam())
        , buf(size)
        , ring(&buf[0], size)
        , ptr()
    {}

    uint32_t const size;
    std::vector<uint32_t> buf;
    RingBuffer<uint32_t> ring;
    uint32_t* ptr;
};


TEST_P(TestRingBuffer, prod_cons) {
    typedef test_threads<RingBuffer<uint32_t> > TT;
    TT::producer_thread prod(ring);
    TT::consumer_thread cons(ring);

    int iters = 100000;
    char* env_iters = getenv("DIAGNOSE_DUPS_RING_STRESS_ITERS");
    if (env_iters) {
        int x = atoi(env_iters);
        if (x > 0) {
            iters = x;
        }
    }
    std::cout << "Testing buffer size " << GetParam() << " for "
        << iters << " iterations.\n";

    boost::thread cons_thread(boost::ref(cons));
    prod(iters);
    cons.stop();
    cons_thread.join();

    std::cout << "prod counter: " << prod.counter_ << "\n";
    std::cout << "cons counter: " << cons.counter_ << "\n";
    ASSERT_EQ(prod.counter_, cons.counter_);
    ASSERT_TRUE(ring.empty());
    uint32_t* p = 0;
    ASSERT_EQ(0, ring.read_buffer(p));
}

INSTANTIATE_TEST_CASE_P(
      BufferSizes
    , TestRingBuffer
    , testing::Values(32, 33, 64, 77, 111)
    );
