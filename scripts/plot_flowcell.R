library(reshape2)

parse_tiles <- function(tiles) {
    side <- as.integer(tiles / 1000)
    if (any(side < 1 | side > 2)) {
        stop("Invalid side (must be 1 or 2)")
    }

    swath <- as.integer((tiles %% 1000) / 100)
    if (any(swath < 1 | swath > 3)) {
        stop("Invalid swath (must be 1, 2 or 3)")
    }

    tile <- tiles %% 100
    if (any(swath < 1 | swath > 24)) {
        stop("Invalid tile (must be in [1, ..., 24])")
    }

    data.frame(side=side, swath=swath, tile_id=tile)
}

aggregate_location_data <- function(...) {
    new_frame <- rbind.fill(...)
    ddply(new_frame, c("flowcell", "lane", "tile", "subtile_x", "subtile_y"), summarise, unique_count = sum(unique_count), duplicate_count = sum(duplicate_count))
}

aggregate_summary <- function(...) {
    new_frame <- rbind.fill(...)
    ddply(new_frame, c("total_fragments", "total_duplicate_fragments", "`duplicate_on_same_strand(pairs)`", "`duplicate_on_different_strand(pairs)`"), summarise, total_fragments = sum(total_fragments), total_duplicate_fragments = sum(total_duplicate_fragments), `duplicate_on_same_strand(pairs)` = sum(`duplicate_on_same_strand(pairs)`), `duplicate_on_different_strand(pairs)` = sum(`duplicate_on_different_strand(pairs)`))
}

aggregate_distance <- function(...) {
    new_frame <- rbind.fill(...)
    ddply(new_frame, c("intratile_distance"), summarise, count = sum(count))
}

aggregate_times_duplicated <- function(...) {
    new_frame <- rbind.fill(...)
    ddply(new_frame, c("num_duplicates"), summarise, count = sum(count))
}

aggregate_insert_size <- function(...) {
    new_frame <- rbind.fill(...)
    ddply(new_frame, c("insert_size"), summarise, unique_count = sum(unique_count), duplicate_count = sum(duplicate_count))
}

aggregate_output <- function(...) {
    json_data <- list(...)
    aggregated_list <- list()
    aggregated_list$summary <- aggregate_summary(lapply(json_data,"[[", "summary"))
    aggregated_list$distance <- aggregate_distance(lapply(json_data, "[[", "distance"))
    aggregated_list$num_times_duplicated <- aggregate_times_duplicated(lapply(json_data, "[[", "num_times_duplicated"))
    aggregated_list$insert_size <- aggregate_insert_size(lapply(json_data,"[[", "insert_size"))
    aggregated_list$per_tile_stats <- aggregate_location_data(lapply(json_data, "[[", "per_tile_stats"))
    aggregated_list
}

