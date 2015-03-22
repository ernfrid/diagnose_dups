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

#include <fstream>
#include <iostream>

using namespace std;

int main(int argc, char** argv) {
    Options opts = Options(argc, argv);

    std::ofstream out;
    std::ostream* out_ptr = &std::cout;
    if (!opts.output_file.empty() && opts.output_file != "-") {
        out.open(opts.output_file.c_str());
        if (!out) {
            std::cerr << "Failed to open output file " << opts.output_file << "\n";
            return 1;
        }
        out_ptr = &out;
    }

    SamReader reader(opts.input_file.c_str(), "r");
    reader.required_flags(BAM_FPROPER_PAIR);
    reader.skip_flags(BAM_FSECONDARY | BAM_FQCFAIL | BAM_FREAD2 | BAM_FSUPPLEMENTARY);

    BamRecord record;
    SignatureBundle bundle;
    BundleProcessor proc;
    while (reader.next(record)) {
        int rv = bundle.add(record);
        if (rv == -1) {
            std::cerr << "Failed to parse bam record, name = " << bam_get_qname(record) << "\n";
            // XXX: would you rather abort?
            continue;
        }
        else if (rv == 0) {
            proc.process(bundle);
            bundle.clear();
        }
    }

    // don't forget the last bundle
    proc.process(bundle);
    proc.write_output(*out_ptr);

    return 0;
}
