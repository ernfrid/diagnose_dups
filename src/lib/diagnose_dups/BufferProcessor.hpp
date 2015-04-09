#pragma once

#include "common/Histogram.hpp"
#include "Read.hpp"
#include "Tile.hpp"

#include <cstddef>
#include <iostream>
#include <stdint.h>

struct BufferProcessor {
    typedef vector<Read> ReadVector;
    typedef boost::unordered_map<Signature, ReadVector> SignatureMap;

    Histogram<uint64_t> dup_insert_sizes;
    Histogram<uint64_t> nondup_insert_sizes;
    Histogram<uint64_t> distances;
    Histogram<uint64_t> number_of_dups;
    Histogram<Tile> tile_duplicates;
    Histogram<Tile> tile_unique;

    std::size_t total_dups;
    std::size_t total_fragments;
    std::size_t dup_on_same_strand;
    std::size_t dup_on_different_strand;

    BufferProcessor()
        : total_dups(0)
        , total_fragments(0)
        , dup_on_same_strand(0)
        , dup_on_different_strand(0)
    {}

    void update_distances(ReadVector const& reads) {
        std::size_t n = reads.size();
        for (std::size_t i = 0; i < n; ++i) {
            ++dup_insert_sizes[abs(reads[i].insert_size)];
            nondup_insert_sizes[abs(reads[i].insert_size)] += 0;
            ++tile_duplicates[reads[i].tile];
            tile_unique[reads[i].tile] += 0;
            for (std::size_t j = i + 1; j < n; ++j) {
                if (is_on_same_tile(reads[i], reads[j])) {
                    uint64_t flow_cell_distance = euclidean_distance(reads[i], reads[j]);
                    ++distances[flow_cell_distance];
                }
                //Only do the below for paired duplicates. If more than 2 copies, this inflates with pairwise comparisons
                if (n == 2) {
                    if (is_on_same_strand(reads[i], reads[j])) {
                        ++dup_on_same_strand;
                    }
                    else {
                        ++dup_on_different_strand;
                    }
                }
            }
        }
    }
    
    void operator()(ReadVector const& reads) {
        total_fragments += reads.size();
        if (reads.size() > 1) {
            total_dups += reads.size();
            ++number_of_dups[reads.size()];
            update_distances(reads);
        }
        else {
            dup_insert_sizes[abs(reads[0].insert_size)] += 0;
            ++nondup_insert_sizes[abs(reads[0].insert_size)];

            ++tile_unique[reads[0].tile];
            tile_duplicates[reads[0].tile] += 0;
        }
    }

    void merge(BufferProcessor& x) {
        dup_insert_sizes.merge(x.dup_insert_sizes);
        nondup_insert_sizes.merge(x.nondup_insert_sizes);
        distances.merge(x.distances);
        number_of_dups.merge(x.number_of_dups);
        total_dups += x.total_dups;
    }

    //XXX This is terrible and should abstracted out
    void write_output(std::ostream& os) {
        os << "{\n"; //start json
        os << " \"summary\": [\n";
        os << "    { "
           << "\"total_fragments\": " << total_fragments
           << ", "
           << "\"total_duplicate_fragments\": " << total_dups
           << ", "
           << "\"duplicate_on_same_strand(pairs)\": " << dup_on_same_strand
           << ", "
           << "\"duplicate_on_different_strand(pairs)\": " << dup_on_different_strand
           << " }\n ],\n";
        typedef Histogram<uint64_t>::VectorType HVec;
        HVec dist = distances.as_sorted_vector();
        os << " \"distance\": [\n";
        for (HVec::const_iterator i = dist.begin(); i != dist.end(); ++i) {
            os << "    { "
               << "\"intratile_distance\": " << i->name
               << ", "
               << "\"count\": " << i->count
               << " }";
            if (i + 1 != dist.end()) {
                os << ",\n";
            }
        }
        os << "\n ],\n";

        os << " \"num_times_duplicated\": [\n";
        HVec ndup = number_of_dups.as_sorted_vector();
        for (HVec::const_iterator i = ndup.begin(); i != ndup.end(); ++i) {
            os << "    { "
               << "\"num_duplicates\": " << i->name
               << ", "
               << "\"count\": " << i->count
               << " }";
            if (i + 1 != ndup.end()) {
                os << ",\n";
            }
        }
        os << "\n ],\n";

        os << " \"insert_size\": [\n";
        HVec isizes = nondup_insert_sizes.as_sorted_vector();
        for (HVec::const_iterator i = isizes.begin(); i != isizes.end(); ++i) {
            os << "    { "
               << "\"insert_size\": " << i->name
               << ", "
               << "\"unique_count\": " << i->count
               << ", "
               << "\"duplicate_count\": " << dup_insert_sizes[i->name]
               << " }";
            if (i + 1 != isizes.end()) {
                os << ",\n";
            }
        }
        os << "\n ],\n";

        typedef Histogram<Tile>::VectorType TVec;
        os << " \"per_tile_stats\": [\n";
        TVec tiles = tile_unique.as_sorted_vector();
        for (TVec::const_iterator i = tiles.begin(); i != tiles.end(); ++i) {
            os << "    { "
               << "\"flowcell\": \"" << i->name.flowcell << "\""
               << ", "
               << "\"lane\": " << i->name.lane
               << ", "
               << "\"tile\": " << i->name.id
               << ", "
               << "\"subtile_x\": " << i->name.subtile_x
               << ", "
               << "\"subtile_y\": " << i->name.subtile_y
               << ", "
               << "\"unique_count\": " << i->count
               << ", "
               << "\"duplicate_count\": " << tile_duplicates[i->name]
               << " }";
            if (i + 1 != tiles.end()) {
                os << ",\n";
            }
        }
        
        os << "\n ]\n}\n";

        std::cerr << total_dups << " duplicates found out of " << total_fragments << ".\n";
    }
};
