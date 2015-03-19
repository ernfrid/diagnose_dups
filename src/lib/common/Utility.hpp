#pragma once

#include <sam.h>

#include <stdint.h>

namespace cigar {

int32_t calculate_right_offset(bam1_t const* record);
int32_t calculate_right_offset(char const* cigar);
int32_t calculate_left_offset(bam1_t const* record);
int32_t calculate_left_offset(char const* cigar);

}
