#pragma once

#include <sam.h> //from htslib

#include <math.h>
#include <stdint.h>
#include <string>

class Read {
    public:
        int32_t insert_size;
        std::string flowcell;
        int lane;
        int tile;
        int x;
        int y;

    Read(bam1_t const* record);
    Read()
        : insert_size(0)
        , flowcell()
        , lane(0)
        , tile(0)
        , x(0)
        , y(0)
    {}

    bool is_on_same_tile(Read const& read) const { 
        return
            flowcell == read.flowcell
            && lane == read.lane
            && tile == read.tile;
    }

    int distance(Read const& read) const {
        //FIXME it would be better to check the return value here for overflow
        return (int) (hypot( (double) (x - read.x), (double) (y - read.y)) + 0.5);
    }

    private:

    bool _parse_queryname(bam1_t const* record);
};


