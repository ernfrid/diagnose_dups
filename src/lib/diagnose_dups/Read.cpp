#include "Read.hpp"

#include "sam.h"

Read::Read(bam1_t const* record)
    : chromosome(record->core.tid)
    , position(record->core.pos)
    , reverse(record->core.flag & BAM_FREVERSE ? true : false)
    , insert_size(record->core.isize)
    , mate_chromosome(record->core.mtid)
    , mate_position(record->core.mpos)
    , mate_reverse(record->core.flag & BAM_FMREVERSE ? true : false)
{}
