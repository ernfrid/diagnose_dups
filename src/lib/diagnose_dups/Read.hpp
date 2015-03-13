#pragma once

//TODO Is this needed?
extern "C" {

#include "sam.h" //from htslib

}

#include <stdint.h>

class Read {
    public:
        int32_t chromosome;
        int32_t position;
        bool reverse;
        int32_t insert_size;
        int32_t mate_chromosome;
        int32_t mate_position;
        bool mate_reverse;

    Read(bam1_t const* record);
    Read()
        : chromosome(-1)
        , position(-1)
        , reverse(false)
        , insert_size(0)
        , mate_chromosome(-1)
        , mate_position(-1)
        , mate_reverse(false)
    {}
};


