#include "diagnose_dups/Read.hpp"
#include "TestData.hpp"

#include <sam.h>
#include <gtest/gtest.h>
#include <stdexcept>
#include <iostream>

using namespace std;

class TestRead : public ::testing::Test {
    public:
        static bam1_t *record;
        static bam1_t *record2;
        static bam_hdr_t *header;

        static void SetUpTestCase() {
            htsFile *fp = hts_open(TEST_BAMS[0].path.c_str(),"r");
            if(fp == 0) {
                cerr << "Unable to open " << TEST_BAMS[0].path << "\n";
                throw std::runtime_error("Error running tests");
            }
            record = bam_init1();
            record2 = bam_init1();
            header = sam_hdr_read(fp);
            if(sam_read1(fp, header, record) <= 0) {
                throw std::runtime_error("Error running tests");
            }
            if(sam_read1(fp, header, record2) <= 0) {
                throw std::runtime_error("Error running tests");
            }
            hts_close(fp);
        }

        static void TearDownTestCase() {
            bam_destroy1(record);
            bam_destroy1(record2);
            bam_hdr_destroy(header);
        }
};

bam1_t* TestRead::record = NULL;
bam1_t* TestRead::record2 = NULL;
bam_hdr_t* TestRead::header = NULL;

TEST_F(TestRead, construction_from_bam_record) {
    Read test_read(record);
    ASSERT_EQ(65, test_read.insert_size);
    ASSERT_EQ("H25NTCCXX", test_read.flowcell);
    ASSERT_EQ(6, test_read.lane);
    ASSERT_EQ(1101, test_read.tile);
    ASSERT_EQ(10206, test_read.x);
    ASSERT_EQ(3454, test_read.y);
}

TEST_F(TestRead, distance) {
    Read test_read(record);
    Read other_test_read(record2);
    ASSERT_EQ(13656,test_read.distance(other_test_read));
}

TEST_F(TestRead, is_on_same_tile) {
    Read test_read(record);
    Read other_test_read(record2);
    ASSERT_TRUE(test_read.is_on_same_tile(other_test_read));
}
