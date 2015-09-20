#pragma once

#include "common/FusionBase.hpp"

#include <sam.h>
#include <boost/fusion/include/adapted.hpp>

#include <stdint.h>

//the intent is for this class to store samblaster signatures
//they will be created directly from a bam1_t record
BOOST_FUSION_DEFINE_STRUCT_INLINE(
    SignatureStorage,
    (int32_t, tid)
    (int32_t, mtid)
    (int32_t, pos)
    (int32_t, mpos)
    (bool, reverse)
    (bool, mreverse)
)

struct Signature : FusionBase<SignatureStorage, Signature> {
    Signature() {
        this->tid = -1;
        this->mtid = -1;
        this->pos = -1;
        this->mpos = -1;
        this->reverse = false;
        this->mreverse = false;
    }

    Signature(bam1_t const* record);

    void parse(bam1_t const* record);
    int32_t _calculate_position(bam1_t const* record);
    int32_t _calculate_mate_position(bam1_t const* record);

    //XXX If the two reads have the same orientation and position
    //then this could be invalid e.g. pass both through or pass only one through
    //Since we are selecting for proper pairs elsewhere
    //I think this will be ok as is
    bool is_for_rightmost_read() {
        return(tid > mtid || (tid == mtid && mpos < pos));
    }
};
