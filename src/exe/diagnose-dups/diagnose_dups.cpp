/*
may need to code against htslib rather than samtools.

For each read in the BAM file:
    calculate the 5' ends of the fragment:
    if last 5' end seen is less than the current 5' end:
        flush buffers, gathering stats on duplicates
    if last 5' end seen is greater than the current 5' end BAM is unsorted and freak out
    create signature and add to buffer

on buffer flush:
    for each signatures:
        skip if count is 1
        add number of reads in signature to histogram of duplicate fragments
        sort into bins by tile
        for each tile, create distance matrix
            increment counts in distance histogram
    clear out signatures hash

for each read we need:
    read name
    chromosome
    position
    a populated mate cigar tag (MC:Z:)
    tlen

we need a method to parse out readnames
we need a method to determine how far apart two readnames are
we need a method to generate a signature
we need a method to expand the cigar strings for the signature
we need a histogram template

we need a small amount of test data

diagnose_dups -i bam_file -o dup_stats
*/

#include "common/Options.hpp"
#include "diagnose_dups/Read.hpp"
#include "diagnose_dups/Signature.hpp"
#include "diagnose_dups/SignatureBundle.hpp"
#include "io/BamRecord.hpp"
#include "io/SamReader.hpp"

#include <sam.h>

#include <boost/unordered_map.hpp>

#include <iostream>
#include <numeric>

using namespace std;

namespace {
    typedef boost::unordered_map<uint64_t, uint64_t> Histogram;
    typedef vector<Read> ReadVector;
    typedef boost::unordered_map<Signature, ReadVector> SignatureMap;

    struct BundleProcessor {
        Histogram dup_insert_sizes;
        Histogram nondup_insert_sizes;
        Histogram distances;
        Histogram number_of_dups;

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
                    ++number_of_dups[reads.size()];
                    update_distances(reads);
                }
            }
        }
    };
}

int main(int argc, char** argv) {
    Options opts = Options(argc, argv);



    SamReader reader(opts.vm["input"].as<string>().c_str(), "r");
    reader.required_flags(BAM_FPROPER_PAIR);
    reader.skip_flags(BAM_FSECONDARY | BAM_FQCFAIL | BAM_FREAD2 | BAM_FSUPPLEMENTARY);

    BamRecord record;
    SignatureBundle bundle;
    std::vector<std::size_t> sig_sizes;
    std::size_t n_bundles = 0;
    std::size_t bundle_size_sum = 0;
    BundleProcessor proc;
    while(reader.next(record)) {
        int rv = bundle.add(record);
        if (rv == -1) {
            std::cerr << "Failed to parse bam record, name = " << bam_get_qname(record) << "\n";
            // XXX: would you rather abort?
            continue;
        }
        else if (rv == 0) {
            bundle_size_sum += bundle.size();
            ++n_bundles;
            proc.process(bundle);
            bundle.clear();
        }
    }
    proc.process(bundle);
    bundle_size_sum += bundle.size();
    ++n_bundles;

    cout << "Inter-tile distance\tFrequency\n";
    for(Histogram::iterator i = proc.distances.begin(); i != proc.distances.end(); ++i) {
        cout << i->first << "\t" << i->second << "\n";
    }
    cout << "\n";

    cout << "Number of dups at location\tFrequency\n";
    for(Histogram::iterator i = proc.number_of_dups.begin(); i != proc.number_of_dups.end(); ++i) {
        cout << i->first << "\t" << i->second << "\n";
    }
    cout << "\n";

    cout << "Size\tUniq frequency\tDup Frequency\n";
    for(Histogram::iterator i = proc.nondup_insert_sizes.begin(); i != proc.nondup_insert_sizes.end(); ++i) {
        cout << i->first << "\t" << i->second << "\t" << proc.dup_insert_sizes[i->first] << "\n";
    }

    double mu = bundle_size_sum / double(n_bundles);
    std::cerr << n_bundles << " bundles.\n";
    std::cerr << "avg bundle size: " << mu << "\n";

    return 0;
}
