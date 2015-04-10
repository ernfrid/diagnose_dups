#pragma once

#include <boost/functional/hash.hpp>

#include <string>

struct Tile {
    std::string flowcell;
    int lane;
    int id;
    int subtile_x;
    int subtile_y;

    Tile()
        : flowcell()
        , lane(0)
        , id(0)
        , subtile_x(-1)
        , subtile_y(-1)
    {}

   friend bool operator==(Tile const& lhs, Tile const& rhs) {
       return same_tile(lhs, rhs)
           && lhs.subtile_x == rhs.subtile_x
           && lhs.subtile_y == rhs.subtile_y;
   }

   friend bool same_tile(Tile const& lhs, Tile const& rhs) {
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
                       if (lhs.id < rhs.id) {
                           return true;
                       }
                       else {
                           if (lhs.id == rhs.id) {
                               if (lhs.subtile_x < rhs.subtile_x) {
                                   return true;
                               }
                               else {
                                   if (lhs.subtile_x == rhs.subtile_x) {
                                       return lhs.subtile_y < rhs.subtile_y;
                                   }
                               }
                           }
                       }
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
       boost::hash_combine(seed, tile.subtile_x);
       boost::hash_combine(seed, tile.subtile_y);
       return seed;
   }

};
