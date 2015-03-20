#include "common/Utility.hpp"
#include "TestData.hpp"

#include <sam.h>
#include <gtest/gtest.h>
#include <stdexcept>
#include <iostream>

using namespace std;
using namespace cigar; //for testing cigar parsing functions

class TestUtility : public ::testing::Test {
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

bam1_t* TestUtility::record = NULL;
bam1_t* TestUtility::record2 = NULL;
bam_hdr_t* TestUtility::header = NULL;

TEST_F(TestUtility, cigar_right_offset_from_bam) {
    //21M1I65M64S
    //Offset to the right should be 21+65+64 or 150 - 1;
    ASSERT_EQ(149,calculate_right_offset(record));
    //151M
    ASSERT_EQ(150,calculate_right_offset(record2));
}

TEST_F(TestUtility, cigar_right_offset_from_bam_handles_no_cigar) {
    //21M1I65M64S
    //Offset to the right should be 21+65+64 or 150 - 1;
    size_t tmp = record->core.n_cigar;
    record->core.n_cigar = 0;
    ASSERT_EQ(0,calculate_right_offset(record));
    record->core.n_cigar = tmp;
}

TEST_F(TestUtility, cigar_left_offset_from_bam) {
    //21M1I65M64S
    //Offset to the right should be 21+65+64 or 150 - 1;
    ASSERT_EQ(0,calculate_left_offset(record));
    //151M
    ASSERT_EQ(0,calculate_left_offset(record2));
}

TEST_F(TestUtility, cigar_left_offset_from_bam_handles_no_cigar) {
    //21M1I65M64S
    //Offset to the right should be 21+65+64 or 150 - 1;
    size_t tmp = record->core.n_cigar;
    record->core.n_cigar = 0;
    ASSERT_EQ(0,calculate_left_offset(record));
    record->core.n_cigar = tmp;
}

TEST_F(TestUtility, cigar_right_offset_from_string) {
    ASSERT_EQ(149, calculate_right_offset("21M1I65M64S"));
    ASSERT_EQ(150, calculate_right_offset("151M"));
    ASSERT_EQ(130, calculate_right_offset("20H131M"));
    ASSERT_EQ(125, calculate_right_offset("20H5S126M"));
    ASSERT_EQ(125, calculate_right_offset("20H5S25X25=76M"));
    ASSERT_EQ(10, calculate_right_offset("140S11M"));
    ASSERT_EQ(150, calculate_right_offset("140M11S"));
    ASSERT_EQ(150, calculate_right_offset("140M11H"));
    ASSERT_EQ(150, calculate_right_offset("140M5S6H"));
    ASSERT_EQ(160, calculate_right_offset("100M10D51M"));
}


TEST_F(TestUtility, cigar_left_offset_from_string) {
    ASSERT_EQ(0, calculate_left_offset("151M"));
    ASSERT_EQ(-20, calculate_left_offset("20H131M"));
    ASSERT_EQ(-25, calculate_left_offset("20H5S126M"));
    ASSERT_EQ(-25, calculate_left_offset("20H5S25X25=26M"));
    ASSERT_EQ(-140, calculate_left_offset("140S11M"));
    ASSERT_EQ(0, calculate_left_offset("140M11S"));
    ASSERT_EQ(0, calculate_left_offset("140M11H"));
    ASSERT_EQ(0, calculate_left_offset("100M10D51M"));
}
