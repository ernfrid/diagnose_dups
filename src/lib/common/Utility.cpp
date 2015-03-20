#include "Utility.hpp"

#include <cstdlib>
#include <vector>


namespace cigar {
    int opcode_for_char(char const& code) {
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

    //the following is a direct copy from htslib
    //it's not a function which is unfortunate
    //https://github.com/samtools/htslib/blob/ef59ef2d6425985732d41bf7389df569a2a14c0a/sam.c#L776-L791
    std::vector<uint32_t> parse_string_to_cigar_vector(char const* string) {
        std::vector<uint32_t> cigar;
        char *op_ptr = (char *)string;
        while(*op_ptr != '\0') {
            uint32_t cigar_component = strtol(op_ptr, &op_ptr, 10)<<BAM_CIGAR_SHIFT;
            int op = (uint8_t)*op_ptr >= 128 ? -1 : opcode_for_char(*op_ptr);
            cigar_component |= op;
            cigar.push_back(cigar_component);
            op_ptr++;
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
        //TODO handle if we don't have a cigar
        uint32_t *cigar = bam_get_cigar(record);
        std::vector<uint32_t> cigar_vec(cigar, cigar + record->core.n_cigar);

        return calculate_right_offset(cigar_vec);
    }

    int32_t calculate_right_offset(char const* cigar) {
        std::vector<uint32_t> cigar_vec = parse_string_to_cigar_vector(cigar);

        return calculate_right_offset(cigar_vec);
    }

    int32_t calculate_left_offset(bam1_t const* record) {
        //TODO handle if we don't have a CIGAR
        uint32_t *cigar = bam_get_cigar(record);
        std::vector<uint32_t> cigar_vec(cigar, cigar + record->core.n_cigar);

        return calculate_left_offset(cigar_vec);
    }

    int32_t calculate_left_offset(char const* cigar) {
        std::vector<uint32_t> cigar_vec = parse_string_to_cigar_vector(cigar);

        return calculate_left_offset(cigar_vec);

    }

}
