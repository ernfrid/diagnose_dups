#include "Signature.hpp"


inline
int32_t Signature::_calculate_position(bam1_t const* record) {
    int32_t tmp_pos = record->core.pos;
    if(reverse) {   //assume this has already been initialized through the initializer list
        tmp_pos += _calculate_right_offset(record);
    }
    else {
        // must be forwards
        tmp_pos += _calculate_left_offset(record);
    }
    return tmp_pos;
}
                

inline
int32_t Signature::_calculate_mate_position(bam1_t const* record) {
    int32_t tmp_mpos = record->core.mpos;
    uint8_t *mate_cigar_tag_ptr = bam_aux_get(record, "MC");
    if(mate_cigar_tag_ptr != 0) {
        char *mc_string = bam_aux2Z(mate_cigar_tag_ptr);
        if(reverse) {
            tmp_mpos += _calculate_right_offset(mc_string);
        }
        else {
            tmp_mpos += _calculate_left_offset(mc_string);
        }

    }
    else {
        //no MC tag on this alignment...
        //what DO WE DO
        //htslib with no cigar assume pos+1 (also for unmapped)
    }
    return tmp_mpos;
}

inline
int32_t Signature::_calculate_right_offset(bam1_t const* record) {
    //htslib doesn't quite do what we want here as softclipping doesn't consume the reference
    //also, samblaster considers leading soft or hardclips in unpacking the coords. But only the first one.
    //TODO What is valid here?
    int32_t offset = 0;
    //TODO handle if we don't have a cigar
    uint32_t *cigar = bam_get_cigar(record);

    int opindex = 0;
    while( (bam_cigar_type(bam_cigar_op(cigar[opindex])) == 0x00 
                || bam_cigar_op(cigar[opindex]) == BAM_CSOFT_CLIP) 
            && opindex < record->core.n_cigar) {
        //NOTE that you could just increment in the while statement.
        //I think this is less prone to misinterpretation
        //This should strip any leading Hardclips or softclips. Note that in the pathological case of a read that is fully hard/softclipped this may not make sense
        opindex++;
    }
    while( (bam_cigar_type(bam_cigar_op(cigar[opindex]))&2 
                || bam_cigar_op(cigar[opindex]) == BAM_CSOFT_CLIP) 
            && opindex < record->core.n_cigar) {
        offset += bam_cigar_oplen(cigar[opindex]);
        opindex++;
    }

    return offset - 1; //adjust here with subtraction or do it outside of here?
}

inline
int32_t Signature::_calculate_right_offset(char const* cigar) {
    int32_t offset = 0;
    char *op_ptr = (char *)cigar;
    long int oplen = -1;
    char opchr;
    bool first = true;
    while(op_ptr != '\0') {
        oplen = strtol(op_ptr, &op_ptr, 10);
        opchr = *op_ptr;
        op_ptr++;
        if(opchr == 'H' || opchr == 'S') {
            if(!first) {
                offset += oplen;
            }
        }
        else { 
            first = false;
            if(opchr == 'M' || opchr == '=' || opchr == 'X' || opchr == 'D') {
                offset += oplen;
            }
            else if(opchr == 'I') {
                //do nothing. We don't care about insertions here.
            }
            else {
                //blow up
            }
        }
    }
    return offset - 1;
}

inline
int32_t Signature::_calculate_left_offset(bam1_t const* record) {
    int32_t offset = 0;
    //TODO handle if we don't have a CIGAR
    uint32_t *cigar = bam_get_cigar(record);

    //all we care about is if the start of this guy is softclipped
    //NOTE we are not going to check for properly constructed CIGAR strings
    int opindex = 0;
    while(bam_cigar_type(bam_cigar_op(cigar[opindex])) == 0x00 
            && opindex < record->core.n_cigar) {
        //NOTE that you could just increment in the while statement.
        //I think this is less prone to misinterpretation
        //This should strip any leading 
        opindex++;
    }

    if(bam_cigar_op(cigar[opindex]) == BAM_CSOFT_CLIP) {
        offset -= bam_cigar_oplen(cigar[opindex]);
    }
    return offset;
}

inline
int32_t Signature::_calculate_left_offset(char const* cigar) {
    int32_t offset = 0;
    char *op_ptr = (char *)cigar;
    long int oplen = -1;
    char opchr;
    while(op_ptr != '\0') {
        oplen = strtol(op_ptr, &op_ptr, 10);
        opchr = *op_ptr;
        op_ptr++;
        if(opchr == 'H' || opchr == 'S') {
            offset += oplen;
        }
        else { 
            if(opchr == 'M' 
                    || opchr == '=' 
                    || opchr == 'X' 
                    || opchr == 'D'
                    || opchr == 'I') {
               return offset; 
            }
            else {
                //blow up
            }
        }
    }
}

