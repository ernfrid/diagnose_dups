#pragma once

#include <boost/functional/hash.hpp>

#include <string>

struct Tile {
    std::string flowcell;
    int lane;
    int id;

    Tile()
        : flowcell()
        , lane(0)
        , id(0)
    {}

   friend bool operator==(Tile const& lhs, Tile const& rhs) {
       return lhs.flowcell == rhs.flowcell
           && lhs.lane == rhs.lane
           && lhs.id == rhs.id;
   }

   friend bool operator<(Tile const& lhs, Tile const&rhs) {
       if (lhs.flowcell < rhs.flowcell) {
           return true;
       }
       else {
           if (lhs.flowcell == rhs.flowcell) {
               if (lhs.lane < rhs.lane) {
                   return true;
               }
               else {
                   if (lhs.lane == rhs.lane) {
                       return lhs.id < rhs.id;
                   }
               }
           }
       }
       return false;
   }

   friend std::size_t hash_value(Tile const& tile) {
       std::size_t seed = boost::hash_value(tile.flowcell);
       boost::hash_combine(seed, tile.lane);
       boost::hash_combine(seed, tile.id);
       return seed;
   }
};
