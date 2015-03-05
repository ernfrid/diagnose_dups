# Purpose
The purpose of this program is due examine characteristics of duplicate reads within a coordinate sorted BAM file

# Metrics
It collects the following metrics:
1. Average number of duplicates of a particular fragment, given that the fragment is duplicated
2. A histogram of the distances between duplicates in a cluster of the same fragment. This would have to be generated within a tile.

# Identifying duplicates
Clusters of duplicates are identified by examining the CIGAR string and mate cigar at a given genomic location. These are then expanded as in SAMBLASTER and metrics calculated for each duplicated fragment.
