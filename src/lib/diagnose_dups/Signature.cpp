#include "Signature.hpp"
#include "common/Utility.hpp"

Signature::Signature(bam1_t const* record) {
    parse(record);
}

void Signature::parse(bam1_t const* record) {
    tid = record->core.tid;
    mtid = record->core.mtid;
    reverse = bam_is_rev(record);
    mreverse = bam_is_mrev(record);
    pos = _calculate_position(record);
    mpos = _calculate_mate_position(record);
}

int32_t Signature::_calculate_position(bam1_t const* record) {
    int32_t tmp_pos = record->core.pos;
    if(reverse) {   //assume this has already been initialized through the initializer list
        tmp_pos += cigar::calculate_right_offset(record);
    }
    else {
        // must be forwards
        tmp_pos += cigar::calculate_left_offset(record);
    }
    return tmp_pos;
}


int32_t Signature::_calculate_mate_position(bam1_t const* record) {
    int32_t tmp_mpos = record->core.mpos;
    uint8_t *mate_cigar_tag_ptr = bam_aux_get(record, "MC");
    if(mate_cigar_tag_ptr != 0) {
        char const* mc_string = bam_aux2Z(mate_cigar_tag_ptr);
        if(mreverse) {
            tmp_mpos += cigar::calculate_right_offset(mc_string);
        }
        else {
            tmp_mpos += cigar::calculate_left_offset(mc_string);
        }

    }
    else {
        //no MC tag on this alignment...
        //what DO WE DO
        //htslib with no cigar assume pos+1 (also for unmapped)
    }
    return tmp_mpos;
}
