#pragma once

#include "TestData.hpp"

#include <sam.h>
#include <gtest/gtest.h>
#include <stdexcept>
#include <iostream>

using namespace std;

struct TestBamRecords {
        bam1_t *record;
        bam1_t *record2;
        bam1_t *record3;
        bam_hdr_t *header;

        TestBamRecords() {
            htsFile *fp = hts_open(TEST_BAMS[0].path.c_str(),"r");
            if (fp == 0) {
                cerr << "Unable to open " << TEST_BAMS[0].path << "\n";
                throw std::runtime_error("Error running tests");
            }
            record = bam_init1();
            record2 = bam_init1();
            record3 = bam_init1();
            header = sam_hdr_read(fp);
            if (sam_read1(fp, header, record) <= 0) {
                throw std::runtime_error("Error parsing first test read");
            }
            if (sam_read1(fp, header, record2) <= 0) {
                throw std::runtime_error("Error parsing second test read");
            }
            if (sam_read1(fp, header, record3) <= 0) {
                throw std::runtime_error("Error parsing third test read");
            }
            hts_close(fp);
        }

        ~TestBamRecords() { 
            bam_destroy1(record);
            bam_destroy1(record2);
            bam_destroy1(record3);
            bam_hdr_destroy(header);
        }
};

