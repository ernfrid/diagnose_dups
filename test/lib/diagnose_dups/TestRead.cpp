#include "diagnose_dups/Read.hpp"
#include "testing/TestBamRecords.hpp"

#include <gtest/gtest.h>

class TestRead : public ::testing::Test {
    public:
        bam1_t *record;
        bam1_t *record2;
        TestBamRecords records;

        TestRead()  {
            record = records.record;
            record2 = records.record2;
        }
};

TEST_F(TestRead, construction_from_bam_record) {
    Read test_read;
    ASSERT_TRUE(parse_read(record, test_read));
    ASSERT_EQ(65, test_read.insert_size);
    ASSERT_EQ("H25NTCCXX", test_read.flowcell);
    ASSERT_EQ(6, test_read.lane);
    ASSERT_EQ(1101, test_read.tile);
    ASSERT_EQ(10206, test_read.x);
    ASSERT_EQ(3454, test_read.y);
}

TEST_F(TestRead, distance) {
    Read test_read;
    test_read.x = 50;
    test_read.y = 8;

    Read other_test_read;
    other_test_read.x = 25;
    other_test_read.y = 16;
    ASSERT_EQ(26u, euclidean_distance(test_read, other_test_read));

    other_test_read.y = 24;
    ASSERT_EQ(30u, euclidean_distance(test_read, other_test_read));
}

TEST_F(TestRead, is_on_same_tile) {
    //FIXME test all possible configurations of flow cell, lane, tile to make
    //sure this is working
    Read test_read;
    test_read.flowcell = "Saint Louis";
    test_read.lane = 1;
    test_read.tile = 1011;

    Read other_test_read;
    other_test_read.flowcell = "Saint Louis";
    other_test_read.lane = 1;
    other_test_read.tile = 1011;

    ASSERT_TRUE(is_on_same_tile(test_read, other_test_read));

    other_test_read.flowcell = "Indianapolis";
    ASSERT_FALSE(is_on_same_tile(test_read, other_test_read));

    other_test_read.flowcell = "Saint Louis";
    other_test_read.lane = 6;
    ASSERT_FALSE(is_on_same_tile(test_read, other_test_read));


    other_test_read.lane = 1;
    other_test_read.tile = 1012;
    ASSERT_FALSE(is_on_same_tile(test_read, other_test_read));
}
