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

#include "common/Histogram.hpp"
#include "common/Options.hpp"
#include "diagnose_dups/BundleProcessor.hpp"
#include "diagnose_dups/Read.hpp"
#include "diagnose_dups/Signature.hpp"
#include "diagnose_dups/SignatureBundle.hpp"
#include "io/BamRecord.hpp"
#include "io/SamReader.hpp"

#include <sam.h>

#include <boost/format.hpp>
#include <boost/timer/timer.hpp>

#include <fstream>
#include <iostream>
#include <iomanip>
#include <stdexcept>

using namespace std;
using boost::format;

namespace {
    void run(Options const& opts) {
        boost::timer::auto_cpu_timer timer;
        std::ofstream out;
        std::ostream* out_ptr = &std::cout;
        if (!opts.output_file.empty() && opts.output_file != "-") {
            out.open(opts.output_file.c_str());
            if (!out) {
                throw std::runtime_error(str(format(
                    "Failed to open output file %1%"
                    ) % opts.output_file));
            }
            out_ptr = &out;
        }

        SamReader reader(opts.input_file.c_str(), "r");
        reader.required_flags(BAM_FPROPER_PAIR);
        reader.skip_flags(BAM_FSECONDARY | BAM_FQCFAIL | BAM_FREAD2 | BAM_FSUPPLEMENTARY);

        BamRecord record;
        SignatureBundle bundle;
        BundleProcessor proc;
        std::size_t parse_failures = 0;
        while (reader.next(record)) {
            int rv = bundle.add(record);
            if (rv == -1) {
                if (++parse_failures < 5) {
                    std::cerr << "Failed to parse bam record, name = "
                        << bam_get_qname(record) << "\n";
                }
                else if (parse_failures == 5) {
                    std::cerr << "Failed to parse bam record, name = "
                        << bam_get_qname(record)
                        << ", max warning limit reached, disabling...\n";
                }

                // XXX: would you rather abort?
                continue;
            }
            else if (rv == 0) {
                proc.process(bundle);
                bundle.clear();
            }
        }

        if (parse_failures) {
            double pct = parse_failures * 100.0;
            pct /= reader.record_count();

            std::cerr << "\nWARNING: failed to parse read names for " << parse_failures
                << " / " << reader.record_count() << " records ("
                << fixed << setprecision(2) << pct << "%).\n\n";
        }

        // don't forget the last bundle
        proc.process(bundle);
        proc.write_output(*out_ptr);
    }
}

int main(int argc, char** argv) {
    try {
        Options opts = Options(argc, argv);
        run(opts);
    }
    catch (std::exception const& ex) {
        std::cerr << "ERROR: " << ex.what() << "\n";
        return 1;
    }

    return 0;
}
