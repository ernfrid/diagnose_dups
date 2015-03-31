#pragma once

#include "TestData.hpp"

#include <sam.h>
#include <gtest/gtest.h>
#include <stdexcept>
#include <iostream>

using namespace std;

class TestWithBamRecord : public ::testing::Test {
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

bam1_t* TestWithBamRecord::record = NULL;
bam1_t* TestWithBamRecord::record2 = NULL;
bam_hdr_t* TestWithBamRecord::header = NULL;
