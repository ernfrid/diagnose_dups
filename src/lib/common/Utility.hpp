#pragma once

#include <sam.h>

#include <stdint.h>
#include <vector>

namespace cigar {

std::vector<uint32_t> parse_string_to_cigar_vector(char const* cigar_string);
int32_t calculate_right_offset(bam1_t const* record);
int32_t calculate_right_offset(char const* cigar);
int32_t calculate_left_offset(bam1_t const* record);
int32_t calculate_left_offset(char const* cigar);

}
