#!/usr/bin/env Rscript

library(jsonlite)
library(IRanges) ## Use this for run length encoded data
library(ggplot2)
library(reshape2)
library(plyr)

args <- commandArgs(TRUE)

source("functions.R")
source("plot_flowcell.R")

x <- fromJSON(args[1])

x <- distance_summary(x)
x <- insert_size_summary(x)
x <- num_times_duplicated_summary(x)

write.table(x$summary, file="/dev/stdout", sep="\t", row.names=F, quote=F)

tile_stats <- x$per_tile_stats
tile_stats$dup_frac <- tile_stats$duplicate_count / (tile_stats$duplicate_count + tile_stats$unique_count)
tile_stats <- cbind(tile_stats, parse_tiles(tile_stats$tile))

yellow <- 0.10
red <- 0.15

tile_stats$tile_id = factor(tile_stats$tile_id, levels=sort(unique(tile_stats$tile_id)))
tile_stats$swath <- factor(tile_stats$swath, levels=sort(unique(tile_stats$swath)))
tile_stats$lane <- sprintf("Lane %d", tile_stats$lane)
tile_stats$side <- sprintf("Side %d", tile_stats$side)

pdf(paste(args[1],"out.pdf",sep="."), width=11, height=8)
df <- tile_stats
cbbPalette <- c("#000000", "#E69F00", "#56B4E9", "#009E73", "#F0E442", "#0072B2", "#D55E00", "#CC79A7")

for (fcid in unique(tile_stats$flowcell)) {
    x <- df[df$flowcell == fcid,]
    x$total <- x$unique_count + x$duplicate_count
    x$density <- x$total / max(x$total)
    
    x$graph_x <- x$subtile_x + (as.integer(x$swath) -1 ) * max(x$subtile_x)
    x$graph_y <- x$subtile_y + (as.integer(x$tile_id) - 1) * max(x$subtile_y) #+ (as.integer(x$tile_id) - 1) * 2) NOTE This didn't work as intended. doesn't look like separation, more like low values.

    # alternatively could try to get the following labeled better:
    # p <- ggplot(df, aes(subtile_x, subtile_y, fill=dup_frac)) + geom_tile() + facet_grid(side + tile_id ~ lane + swath, scales="free", space="free") + theme_bw() + scale_fill_gradient(low="#22FF00", high="#FF0000", name="Duplication") + opts(panel.margin = unit(0,"lines"))

    p <- ggplot(x, aes(graph_x, graph_y, fill=total)) +
        geom_tile() +
        facet_grid(side ~ lane) +
        theme_bw() +
        labs(x="Swath", y="Tile") +
        opts(title=sprintf("Per-tile read counts for flow cell %s", fcid)) +
        scale_fill_continuous(name="Read counts")
    print(p)

    p <- ggplot(x, aes(graph_x, graph_y, fill=dup_frac)) +
        geom_tile() +
        facet_grid(side ~ lane) +
        theme_bw() +
        labs(x="Swath", y="Tile") +
        opts(title=sprintf("Per-tile duplication for flow cell %s", fcid)) +
        scale_fill_gradient(low="#22FF00", high="#FF0000", name="Duplication")
            #breaks=c(0:10 / 10))
    print(p)

    p <- ggplot(x, aes(total, dup_frac, color=lane, shape=interaction(side, swath))) +
        geom_point() +
        theme_bw() +
        labs(x="Read count", y="Duplication") +
        opts(title=sprintf("Per-tile duplication by read count for flow cell %s", fcid)) +
        scale_color_manual(values=cbbPalette) +
        scale_shape_discrete(name="Side.Swath")
    print(p)
}

dev.off()

# calculate summary statistics and print out a line to stderr
# generate a multi-page PDF of possible plots
# plot insert size metrics
# plot distance graph
# plot flowcell graph(s)
