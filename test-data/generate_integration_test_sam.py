#!/usr/bin/env python

header = """@HD	VN:1.3	SO:coordinate
@SQ	SN:1	LN:249250621
@SQ	SN:2	LN:243199373"""

sam_lines = (
        header,
        #there are 4 dups in the block below
        #1 intertile, 1 clipped
        #should generate distances of 9 and 12, the other read is not on the same tile as the first in the file
        #should only generate a single insert size. 1 non dup @575, and 4 dups
        #there is one duplicated fragment and it is duplicated 4 times
        #3 paired comparisons are intertile
        "machine:1:flowcell:1:1102:1:1	163	1	15729	60	5M	=	16153	575	CAGGG	AAFFF	MC:Z:5M",
        "machine:1:flowcell:1:1101:1:1	99	1	15729	60	5M	=	16153	575	CAGGG	AAFFF	MC:Z:5M",
        "machine:1:flowcell:1:1101:1:10	99	1	15729	60	5M	=	16153	575	CAGGG	AAFFF	MC:Z:5M", # distance should be 9
        "machine:1:flowcell:1:1101:1:10	419	1	15729	60	5M	=	16153	575	CAGGG	AAFFF	MC:Z:5M", # this shouldn't be counted it isn't primary
        "machine:1:flowcell:1:1102:1:20	99	1	15730	60	4M	=	16153	575	AGGG	AFFF	MC:Z:5M", # isn't a dup

        "machine:1:flowcell:1:1102:1:13	99	1	15731	60	2S3M	=	16153	575	CAGGG	AAFFF	MC:Z:5M", # distance should be 12, this IS a dup

        # the following two reads are not duplicates, they add insert sizes of 375 and 475
        "machine:1:flowcell:2:1101:1:1	99	1	15750	60	5M	=	16153	375	GATCT	AAFFF	MC:Z:5M",
        "machine:1:flowcell:2:1102:1:1	99	1	15753	60	5M	=	16153	475	CTTGC	AAFFF	MC:Z:5M",

        #below are the mates from the first block. Ideally they are not counted since that would double count fragments
        "machine:1:flowcell:1:1101:1:1	147	1	16153	60	5M	=	15729	-575	GTGCA	AAFFF	MC:Z:5M",
        "machine:1:flowcell:1:1102:1:1	83	1	16153	60	5M	=	15729	-575	GTGCA	AAFFF	MC:Z:5M",
        "machine:1:flowcell:1:1101:1:10	147	1	16153	60	5M	=	15729	-575	GTGCA	AAFFF	MC:Z:5M", # distance should be 9
        "machine:1:flowcell:1:1102:1:20	147	1	16153	60	5M	=	15730	-575	GTGCA	AAFFF	MC:Z:4M",
        "machine:1:flowcell:1:1102:1:13	147	1	16153	60	5M	=	15731	-575	GTGCA	AAFFF	MC:Z:2S3M",

        #these are the mates for earlier reads. Ideally they are not counted.
        "machine:1:flowcell:2:1101:1:1	147	1	16153	60	5M	=	15750	-375	GTGCA	AAFFF	MC:Z:5M",
        "machine:1:flowcell:2:1102:1:1	147	1	16153	60	5M	=	15753	-475	GTGCA	AAFFF	MC:Z:5M",

        #These are duplicates on the reverse strand They are intertile and should not count to distances...
        #They should add 143 to insert sizes with 2 dups
        #should add 1 additional intertile comparison
        "machine:1:flowcell:2:1103:1:9	83	1	17000	60	5M	=	17153	-143	ACATC	AAFFF	MC:Z:5M",
        "machine:1:flowcell:2:1104:1:9	83	1	17000	60	5M	=	17153	-143	ACATC	AAFFF	MC:Z:5M",

        #This is a block of reads on a second chromosome to make sure we handle buffering correctly
        #It should add 1 nondup and 3 actual dups
        #1 insert size of 1000 (nondup) and 3 of 980
        #Distances are: 2 of 19, 1 of 27
        #Fragment is duplicated 3 times.
        "machine:1:flowcell:1:1104:1:9	83	2	17000	60	5M	=	18000	1000	GTTTT	AAFFF	MC:Z:5M",
        "machine:1:flowcell:1:1104:1:1	99	2	17020	60	5M	=	18000	980	TACAC	AAFFF	MC:Z:5M",
        "machine:1:flowcell:1:1104:1:20	99	2	17020	60	5M	=	18000	980	TACAC	AAFFF	MC:Z:5M",
        "machine:1:flowcell:1:1104:20:1	99	2	17020	60	5M	=	18000	980	TACAC	AAFFF	MC:Z:5M",

        #XXX getting lazy and not fixing the nucleotides below these are the mates for everything remaining.

        #These are the mates of the duplicates on the reverse strand. Ideally they shouldn't be counted.
        "machine:1:flowcell:2:1103:1:9	163	1	17153	60	5M	=	17000	143	ACATC	AAFFF	MC:Z:5M",
        "machine:1:flowcell:2:1104:1:9	163	1	17153	60	5M	=	17000	143	ACATC	AAFFF	MC:Z:5M",

        "machine:1:flowcell:1:1104:1:9	163	2	18000	60	5M	=	17000	-1000	GTTTT	AAFFF	MC:Z:5M",
        "machine:1:flowcell:1:1104:1:1	147	2	18000	60	5M	=	17020	-980	TACAC	AAFFF	MC:Z:5M",
        "machine:1:flowcell:1:1104:1:20	147	2	18000	60	5M	=	17020	-980	TACAC	AAFFF	MC:Z:5M",
        "machine:1:flowcell:1:1104:20:1	147	2	18000	60	5M	=	17020	-980	TACAC	AAFFF	MC:Z:5M",

        )

print "\n".join(sam_lines)
