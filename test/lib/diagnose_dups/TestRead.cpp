#include "diagnose_dups/Read.hpp"

#include<sam.h>

namespace {
    uint8_t data[23] ={
        0x6a, 0x75, 0x6e, 0x6b, 0x0, 0x20, 0x0, 0x0, 0x0, 0x28, 0x27, 0x21,
        0x52, 0x47, 0x5a, 0x72, 0x67, 0x33, 0x0, 0x41, 0x4d, 0x43, 0x25
    };
}

class TestRead : public ::testing::Test {
    public:
        void SetUp() {}
};

TEST_F(TestRead, construction_from_bam_record) {

}

TEST_F(TestRead, distance) {

}

TEST_F(TestRead, is_on_same_tile) {

}

TEST_F(TestRead, _parse_queryname) {

}
