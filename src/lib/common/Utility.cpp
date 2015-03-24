#include "Utility.hpp"
#include "Parse.hpp"
#include "config.hpp"

#include <boost/format.hpp>

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <numeric>
#include <stdexcept>
#include <vector>

using boost::format;

namespace cigar {

    int opcode_for_char(char code) {
        //stupid, but easy
        //"MIDNSHP=XB"
        switch(code) {
            case 'M':
                return 0;
            case 'I':
                return 1;
            case 'D':
                return 2;
            case 'N':
                return 3;
            case 'S':
                return 4;
            case 'H':
                return 5;
            case 'P':
                return 6;
            case '=':
                return 7;
            case 'X':
                return 8;
            case 'B':
                return 9;
            default:
                return -1;
        }
    }

    bool valid_cigar_len(uint32_t len) {
        return len <= bam_cigar_oplen(~0u);
    }

    //the following is a translated from htslib
    //it's not a function which is unfortunate
    //https://github.com/samtools/htslib/blob/ef59ef2d6425985732d41bf7389df569a2a14c0a/sam.c#L776-L791
    std::vector<uint32_t> parse_string_to_cigar_vector(char const* cigar_string) {
        char const* beg = cigar_string;
        std::size_t len = std::strlen(beg);
        char const* end = beg + len;

        std::vector<uint32_t> cigar;
        // 2x performance increase
        cigar.reserve(len);

        for(; *beg != '\0'; ++beg) {
            uint32_t oplen = 0;
            // qi numeric parsing is much faster than strtoul
            // it takes beg by reference and moves it to the first non-numeric
            // character
            if(UNLIKELY(!auto_parse(beg, end, oplen) || !valid_cigar_len(oplen))) {
                // do you want to get mad if this isn't a number?
                throw std::runtime_error(str(format(
                    "Error parsing cigar string %1%: expected number at position %2%"
                    ) % cigar_string % (beg - cigar_string)));
            }

            int op = opcode_for_char(*beg);

            if(UNLIKELY(op < 0)) {
                throw std::runtime_error(str(format(
                    "Error parsing cigar string %1%: invalid cigar op char at position %2%"
                    ) % cigar_string % (beg - cigar_string)));
            }

            cigar.push_back(bam_cigar_gen(oplen, op));
        }
        return cigar;
    }

    bool affects_right_offset(uint32_t cigar) {
        uint32_t op = bam_cigar_op(cigar);
        return op != BAM_CSOFT_CLIP && bam_cigar_type(op) != 0;
    }

    uint32_t right_offset_add(int32_t current, uint32_t cigar) {
        uint32_t op = bam_cigar_op(cigar);
        if(bam_cigar_type(op) & 2 || op == BAM_CSOFT_CLIP || op == BAM_CHARD_CLIP) {
            current += bam_cigar_oplen(cigar);
        }
        return current;
    }

    int32_t calculate_right_offset(uint32_t const* beg, uint32_t const* end) {
        // HTSlib doesn't quite do what we want here as softclipping
        // doesn't consume the reference.
        // Also, samblaster considers leading soft or hardclips in
        // unpacking the coords. But only the first one.
        uint32_t const* first = std::find_if(beg, end, affects_right_offset);
        return std::accumulate(first, end, 0, right_offset_add) - 1;
    }

    bool affects_left_offset(uint32_t cigar) {
        uint32_t op = bam_cigar_op(cigar);
        return op == BAM_CHARD_CLIP || bam_cigar_type(op) != 0;
    }

    bool is_not_clipping(uint32_t cigar) {
        uint32_t op = bam_cigar_op(cigar);
        return op != BAM_CHARD_CLIP && op != BAM_CSOFT_CLIP;
    }

    int32_t left_offset_sub(int32_t current, uint32_t cigar) {
        uint32_t op = bam_cigar_op(cigar);
        if(op == BAM_CSOFT_CLIP || op == BAM_CHARD_CLIP) {
            current -= bam_cigar_oplen(cigar);
        }
        return current;
    }

    int32_t calculate_left_offset(uint32_t const* beg, uint32_t const* end) {
        uint32_t const* first = std::find_if(beg, end, affects_left_offset);
        uint32_t const* last = std::find_if(first, end, is_not_clipping);
        return std::accumulate(first, last, 0, left_offset_sub);
    }

    int32_t calculate_right_offset(bam1_t const* record) {
        if(record->core.n_cigar) {
            uint32_t *cigar = bam_get_cigar(record);
            return calculate_right_offset(cigar, cigar + record->core.n_cigar);
        }
        else {
            return 0; // no position offset if not cigar. Makes sense since read not mapped.
        }
    }

    int32_t calculate_right_offset(char const* cigar) {
        std::vector<uint32_t> cigar_vec = parse_string_to_cigar_vector(cigar);
        return calculate_right_offset(cigar_vec.data(), cigar_vec.data() + cigar_vec.size());
    }

    int32_t calculate_left_offset(bam1_t const* record) {
        if(record->core.n_cigar) {
            uint32_t *cigar = bam_get_cigar(record);
            return calculate_left_offset(cigar, cigar + record->core.n_cigar);
        }
        else {
            return 0;
        }
    }

    int32_t calculate_left_offset(char const* cigar) {
        std::vector<uint32_t> cigar_vec = parse_string_to_cigar_vector(cigar);
        return calculate_left_offset(cigar_vec.data(), cigar_vec.data() + cigar_vec.size());
    }
}
