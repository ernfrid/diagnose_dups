#pragma once

#include "common/Histogram.hpp"

#include <cstddef>
#include <iostream>
#include <stdint.h>

struct BundleProcessor {
    typedef vector<Read> ReadVector;
    typedef boost::unordered_map<Signature, ReadVector> SignatureMap;

    Histogram<uint64_t> dup_insert_sizes;
    Histogram<uint64_t> nondup_insert_sizes;
    Histogram<uint64_t> distances;
    Histogram<uint64_t> number_of_dups;
    std::size_t total_dups;

    BundleProcessor()
        : total_dups(0)
    {}

    void update_distances(ReadVector const& reads) {
        std::size_t n = reads.size();
        for (std::size_t i = 0; i < n; ++i) {
            ++dup_insert_sizes[abs(reads[i].insert_size)];
            nondup_insert_sizes[abs(reads[i].insert_size)] += 0;
            for (std::size_t j = i + 1; j < n; ++j) {
                if (is_on_same_tile(reads[i], reads[j])) {
                    uint64_t flow_cell_distance = euclidean_distance(reads[i], reads[j]);
                    ++distances[flow_cell_distance];
                }
            }
        }
    }
    
    void operator()(ReadVector const& reads) {
        if (reads.size() > 1) {
            ++total_dups;
            ++number_of_dups[reads.size()];
            update_distances(reads);
        }
        else {
            dup_insert_sizes[abs(reads[0].insert_size)] += 0;
            ++nondup_insert_sizes[abs(reads[0].insert_size)];
        }
    }

    void merge(BundleProcessor& x) {
        dup_insert_sizes.merge(x.dup_insert_sizes);
        nondup_insert_sizes.merge(x.nondup_insert_sizes);
        distances.merge(x.distances);
        number_of_dups.merge(x.number_of_dups);
        total_dups += x.total_dups;
    }

    void write_output(std::ostream& os) {
        typedef Histogram<uint64_t>::VectorType HVec;
        HVec dist = distances.as_sorted_vector();
        os << "Inter-tile distance\tFrequency\n";
        for(HVec::const_iterator i = dist.begin(); i != dist.end(); ++i) {
            os << i->name << "\t" << i->count << "\n";
        }
        os << "\n";

        HVec ndup = number_of_dups.as_sorted_vector();
        os << "Number of dups at location\tFrequency\n";
        for(HVec::const_iterator i = ndup.begin(); i != ndup.end(); ++i) {
            os << i->name << "\t" << i->count << "\n";
        }
        os << "\n";

        HVec isizes = nondup_insert_sizes.as_sorted_vector();
        os << "Size\tUniq frequency\tDup Frequency\n";
        for(HVec::const_iterator i = isizes.begin(); i != isizes.end(); ++i) {
            os << i->name << "\t" << i->count << "\t" << dup_insert_sizes[i->name] << "\n";
        }

        std::cerr << total_dups << " duplicates found.\n";
    }
};
