#pragma once

#include "TestData.hpp"

#include <sam.h>
#include <gtest/gtest.h>
#include <stdexcept>
#include <iostream>

using namespace std;

/* XXX Reevaluate this at a later time
 * 1) Is this appropriate as a base class or does it make more sense to be present in composition
 * 2) As a base class with static members, are we re-initializing those on every construction? 
 *    The answer should be yes and that seems bad.
 * FIXME Due to the above, likely want to pull this into its own class and then have interested 
 * tests use them as static variables that get initialized
 */

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
