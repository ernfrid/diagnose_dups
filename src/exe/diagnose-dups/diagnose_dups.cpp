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

int main(int argc, char** argv) {

    return 0;
}
