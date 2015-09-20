#pragma once

#include "common/FusionBase.hpp"

#include <boost/fusion/adapted.hpp>

#include <algorithm>
#include <string>

BOOST_FUSION_DEFINE_STRUCT_INLINE(
    TileStorage,
    (std::string, flowcell)
    (int, lane)
    (int, id)
    (int, subtile_x)
    (int, subtile_y)
)

struct Tile : FusionBase<TileStorage, Tile> {
    Tile()
    {
        lane = 0;
        id = 0;
        subtile_x = -1;
        subtile_y = -1;
    }

    friend bool same_tile(Tile const& lhs, Tile const& rhs) {
        return lhs.flowcell == rhs.flowcell
            && lhs.lane == rhs.lane
            && lhs.id == rhs.id;
    }

    friend bool adjacent_tile(Tile const& lhs, Tile const& rhs) {
        // FIXME: magic numbers o: what's 2000, whats 2? (rhetorical)
        return lhs.flowcell == rhs.flowcell
            && lhs.lane == rhs.lane
            && (lhs.id > 2000) == (rhs.id > 2000)
            && std::abs( int(lhs.id / 100) - int(rhs.id / 100) ) < 2
            && std::abs( int(lhs.id % 100) - int(rhs.id % 100) ) < 2
            ;
    }

};
