#pragma once

#include "SignatureBundle.hpp"
#include "common/Histogram.hpp"

#include <cstddef>
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
        for (std::size_t i = 0; i < n - 1; ++i) {
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

    void process(SignatureBundle const& bundle) {
        std::vector<SigRead> const& sigreads = bundle.data();

        SignatureMap sigmap;
        for (std::size_t i = 0; i < sigreads.size(); ++i) {
            sigmap[sigreads[i].sig].push_back(sigreads[i].read);
        }

        for (SignatureMap::const_iterator i = sigmap.begin(); i != sigmap.end(); ++i) {
            ReadVector const& reads = i->second;
            if (reads.size() > 1) {
                ++total_dups;
                ++number_of_dups[reads.size()];
                update_distances(reads);
            }
        }
    }
};

