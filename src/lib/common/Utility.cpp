#include "Utility.hpp"
#include "config.hpp"

#include <boost/format.hpp>
#include <boost/spirit/include/qi_numeric.hpp>
#include <boost/spirit/include/qi_parse.hpp>

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <vector>

using boost::format;
namespace qi = boost::spirit::qi;

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
            if (UNLIKELY(!qi::parse(beg, end, qi::uint_, oplen) || !valid_cigar_len(oplen))) {
                // do you want to get mad if this isn't a number?
                throw std::runtime_error(str(format(
                    "Error parsing cigar string %1%: expected number at position %2%"
                    ) % cigar_string % (beg - cigar_string)));
            }

            int op = opcode_for_char(*beg);

            if (UNLIKELY(op < 0)) {
                throw std::runtime_error(str(format(
                    "Error parsing cigar string %1%: invalid cigar op char at position %2%"
                    ) % cigar_string % (beg - cigar_string)));
            }

            cigar.push_back(bam_cigar_gen(oplen, op));
        }
        return cigar;
    }

    int32_t calculate_right_offset(std::vector<uint32_t> const& cigar) {
        //htslib doesn't quite do what we want here as softclipping doesn't consume the reference
        //also, samblaster considers leading soft or hardclips in unpacking the coords. But only the first one.
        //TODO What is valid here?
        int32_t offset = 0;
        typedef std::vector<uint32_t>::const_iterator iter;
        iter i = cigar.begin();
        while( (bam_cigar_type(bam_cigar_op(*i)) == 0x00 
                    || bam_cigar_op(*i) == BAM_CSOFT_CLIP) 
                && i != cigar.end()) {
            //NOTE that you could just increment in the while statement.
            //I think this is less prone to misinterpretation
            //This should strip any leading Hardclips or softclips. Note that in the pathological case of a read that is fully hard/softclipped this may not make sense
            ++i;
        }
        while(i != cigar.end()) {
            if(bam_cigar_type(bam_cigar_op(*i))&2 
                    || bam_cigar_op(*i) == BAM_CSOFT_CLIP
                    || bam_cigar_op(*i) == BAM_CHARD_CLIP) {
                offset += bam_cigar_oplen(*i);
            }
            ++i;
        }
        return offset - 1;
    }

    int32_t calculate_left_offset(std::vector<uint32_t> const& cigar) {
        int32_t offset = 0;
        typedef std::vector<uint32_t>::const_iterator iter;
        iter i = cigar.begin();

        //all we care about is if the start of this guy is softclipped
        //NOTE we are not going to check for properly constructed CIGAR strings
        while(bam_cigar_type(bam_cigar_op(*i)) == 0x00
                && bam_cigar_op(*i) != BAM_CHARD_CLIP
                && i != cigar.end()) {
            //NOTE that you could just increment in the while statement.
            //I think this is less prone to misinterpretation
            //This should strip any leading 
            ++i;
        }

        while(bam_cigar_op(*i) == BAM_CSOFT_CLIP
                || bam_cigar_op(*i) == BAM_CHARD_CLIP) {
            offset -= bam_cigar_oplen(*i);
            ++i;
        }
        return offset;
    }


    int32_t calculate_right_offset(bam1_t const* record) {
        if(record->core.n_cigar) {
            uint32_t *cigar = bam_get_cigar(record);
            std::vector<uint32_t> cigar_vec(cigar, cigar + record->core.n_cigar);

            return calculate_right_offset(cigar_vec);
        }
        else {
            return 0; // no position offset if not cigar. Makes sense since read not mapped.
        }
    }

    int32_t calculate_right_offset(char const* cigar) {
        std::vector<uint32_t> cigar_vec = parse_string_to_cigar_vector(cigar);

        return calculate_right_offset(cigar_vec);
    }

    int32_t calculate_left_offset(bam1_t const* record) {
        if(record->core.n_cigar) {
            uint32_t *cigar = bam_get_cigar(record);
            std::vector<uint32_t> cigar_vec(cigar, cigar + record->core.n_cigar);

            return calculate_left_offset(cigar_vec);
        }
        else {
            return 0;
        }
    }

    int32_t calculate_left_offset(char const* cigar) {
        std::vector<uint32_t> cigar_vec = parse_string_to_cigar_vector(cigar);

        return calculate_left_offset(cigar_vec);

    }

}
