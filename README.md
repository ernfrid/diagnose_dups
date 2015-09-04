[![Build Status](https://travis-ci.org/genome/diagnose_dups.svg?branch=master)](https://travis-ci.org/genome/diagnose_dups)
[![Coverage Status](https://coveralls.io/repos/genome/diagnose_dups/badge.svg)](https://coveralls.io/r/genome/diagnose_dups)

# Description
The purpose of this program is due examine characteristics of duplicate reads within a coordinate sorted BAM file. It is intended to be run by bioinformaticians interested in examining duplicate reads within a BAM file. Metrics are output as JSON files that can then be analyzed in R or the statistical package of your choice. Some R scripts are in the process of being written and exist [here](https://github.com/genome/diagnose_dups/tree/master/scripts]) but assume a relatively old version of ggplot.

# Methods
## Identifying duplicates
Clusters of duplicates are identified by examining the CIGAR string and mate cigar at a given genomic location. These are then expanded as in SAMBLASTER and metrics calculated for each duplicated fragment. Only properly paired reads are considered and this may result in slight changes in the percentage of duplicates for most experiments.

# Input
A BAM file containing reads for a single library is expected as duplication detection is not library aware. The BAM file must be coordinate sorted. Reads are expected to be named in the following convention `machine:run:flowcell:lane:tile:x_coord:y_coord` and this is used to determine the physical location of reads on the flow cell. Additionally, reads are expected to contain the `MC` tag indicating the mate cigar string of each read. This tag can be added with either Picard or SAMBLASTER.

# Output
Metrics are reported in JSON. An example is present in the [test-data](https://github.com/genome/diagnose_dups/blob/master/test-data/expected_data.json). Definitions of the metrics are below.

1. **summary** - Overall summary metrics for the data examined.
   1. **total_fragments** - The number of fragments examined.
   2. **total_duplicated_fragments** - The number of fragments that were duplicated. This differs from other calculations as all fragments are counted as duplicate (a single one is not chosen as unique).
   3. **total_duplicate_fragments** - The number of duplicate fragments. This is the typical value reported by other software for calculating duplication rate.
   3. **total_flowcell_duplicates** - The number of duplicate fragments arising between the same or adjacent tiles. These are assumed to be a result of duplication on the patterned flow cell.
   3.  **duplicate_on_same_strand(pairs)** - Both fragments arose from the same strand of the duplicated molecule (sense or antisense). Only calculated for fragments duplicated once. 
   4.  **duplicate_on_different_strand(pairs)** - The fragments arose from different strands of the duplicated molecule (one frome the sense strand, one from the antisense strand). Only calculated for fragments duplicated once.
   5.  **subtile_dup_rate_stdev** - The standard deviation of the duplicated fragment rate per subtile across all subtiles analyzed in the BAM. If the file contains data from different flowcells and lanes, then this value may not be informative. 
2. **distance** - Histogram of Euclidean distance within a flowcell tile between pairs of duplicated molecules.
   1. **intratile_distance** - The Euclidean distance between the duplicate fragments on the flow cell. Only calculated if the fragments are within the same tile.
   2. **count** - The number of fragments with the given intratile distance.
3. **insert_size** - Histogram of duplication rate relative to insert size.
   1. **insert_size** - The insert size in bp.
   2. **unique_count** - The number of unique fragments with this insert size.
   3. **duplicate_count** - The number of duplicated fragments with this insert size.
4. **per_tile_stats** - Information on the number of duplicates, broken down by flow cell and tile.
   1. **flowcell** - The flow cell name. Derived from the read names.
   2. **lane** - The lane within the flow cell. Derived from the read names.
   3. **tile** - The tile location within the lane. Derived from the read names.
   4. **subtile_x** - The subtile x-coordinate within the tile. Currently these are binned as x-location in read name divided by 1000.
   5. **subtile_y** - The subtile y-coordinate within the tile. Current these are binned as y-location in read name divided by 1000.
   6. **unique_count** - The number of unique fragments within this subtile.
   7. **duplicate_count** - The number of duplicated fragments within this subtile.

# Compilation and Installation
Currently, `diagnose_dups` must be compiled from source code.

## Dependencies

* git
* cmake 2.8+ ([cmake.org](http://cmake.org))
* boost1.48 or greater 
* zlib
* htslib1.2.1 or greater

## Build Instructions

### Installing dependencies

* For APT-based systems (Debian, Ubuntu), install the following packages:

```
sudo apt-get install build-essential git-core cmake zlib1g-dev libncurses-dev libboost-dev
```

* For htslib:
```
wget https://github.com/samtools/htslib/archive/1.2.1.tar.gz -O /tmp/htslib-1.2.1.tar.gz
tar -xvf /tmp/htslib-1.2.1.tar.gz
cd htslib-1.2.1/ && make && sudo make install
```

### Clone the repository

```
git clone git://github.com/genome/diagnose_dups.git
```

### Build the program

`diagnose_dups` does not support in-source builds. So create a subdirectory, enter it, build, and run tests:

```
mkdir diagnose_dups/build
cd diagnose_dups/build
cmake ..
make -j
make test
```
Tests should pass. The binary `diagnose_dups` can then be found under `diagnose_dups/build/bin`. If you have administrative rights, then run `sudo make install` to install the tool for all users under `/usr/bin`.

# Getting Help
Please open issues on the github [repository](https://github.com/genome/diagnose_dups/issues) to obtain help.

# Contributors
+ [David E Larson](https://github.com/ernfrid)
+ [Travis E Abbott](https://github.com/tabbott)
