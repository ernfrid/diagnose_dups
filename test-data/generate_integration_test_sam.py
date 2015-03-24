#!/usr/bin/env python

header = """@HD	VN:1.3	SO:coordinate
@SQ	SN:1	LN:249250621
@SQ	SN:2	LN:243199373"""

exemplar_read = """machine:1:flowcell:1:1101:1:1	99	1	15729	60	5M	=	16153	575	CAGGG	AAFFF	MC:Z:5M"""
outside_tile_dup = """machine:1:flowcell:1:1102:1:1	99	1	15729	60	5M	=	16153	575	CAGGG	AAFFF	MC:Z:5M"""
inside_tile_dup = """machine:1:flowcell:1:1101:1:10	99	1	15729	60	5M	=	16153	575	CAGGG	AAFFF	MC:Z:5M""" # distance should be 9
intervening_read = """machine:1:flowcell:1:1102:1:20	99	1	15730	60	4M	=	16153	575	AGGG	AFFF	MC:Z:5M"""
 
clipped_dup = """machine:1:flowcell:1:1102:1:13	99	1	15731	60	2S3M	=	16153	575	CAGGG	AAFFF	MC:Z:5M""" # distance should be 12

non_dup = """machine:1:flowcell:2:1101:1:1	99	1	15750	60	5M	=	16153	375	GATCT	AAFFF	MC:Z:5M"""
non_dup2 = """machine:1:flowcell:2:1102:1:1	99	1	15753	60	5M	=	16153	475	CTTGC	AAFFF	MC:Z:5M"""

reverse_strand_exemplar = """machine:1:flowcell:2:1103:1:9	83	1	17000	60	5M	=	17153	-143	ACATC	AAFFF	MC:Z:5M"""
reverse_strand_exemplar_dup = """machine:1:flowcell:2:1104:1:9	83	1	17000	60	5M	=	17153	-143	ACATC	AAFFF	MC:Z:5M"""

second_chr_nondup = """machine:1:flowcell:1:1104:1:9	83	2	17000	60	5M	=	18000	1000	GTTTT	AAFFF	MC:Z:5M"""
second_chr_dup = """machine:1:flowcell:1:1104:1:1	99	2	17020	60	5M	=	18000	980	TACAC	AAFFF	MC:Z:5M"""
second_chr_dup1 = """machine:1:flowcell:1:1104:1:20	99	2	17020	60	5M	=	18000	980	TACAC	AAFFF	MC:Z:5M"""
second_chr_dup2 = """machine:1:flowcell:1:1104:20:1	99	2	17020	60	5M	=	18000	980	TACAC	AAFFF	MC:Z:5M"""

print "\n".join((
    header, 
    exemplar_read,
    outside_tile_dup,
    inside_tile_dup,
    intervening_read,
    clipped_dup,
    non_dup,
    non_dup2,
    reverse_strand_exemplar,
    reverse_strand_exemplar_dup,
    second_chr_nondup,
    second_chr_dup,
    second_chr_dup1,
    second_chr_dup2
    ))

