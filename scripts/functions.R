library(IRanges)

distance_summary <- function(x) {
    rle <- Rle(x$distance$intratile_distance, x$distance$count)
    x$summary$mean_distance <- mean(rle)
    x$summary$sd_distance <- sd(rle)
    x$summary$median_distance <- median(rle)
    x$summary$mad_distance <- mad(rle)
    invisible(x)
}

insert_size_summary <- function(x) {
    unique <- Rle(x$insert_size$insert_size, x$insert_size$unique_count)
    duplicate <- Rle(x$insert_size$insert_size, x$insert_size$duplicate_count)

    x$summary$mean_unique_insert_size <- mean(unique)
    x$summary$sd_unique_insert_size <- sd(unique)
    x$summary$median_unique_insert_size <- median(unique)
    x$summary$mad_unique_insert_size <- mad(unique)

    x$summary$mean_duplicate_insert_size <- mean(duplicate)
    x$summary$sd_duplicate_insert_size <- sd(duplicate)
    x$summary$median_duplicate_insert_size <- median(duplicate)
    x$summary$mad_duplicate_insert_size <- mad(duplicate)
    invisible(x)
}

num_times_duplicated_summary <- function(x) {
    num_times_duplicated <- Rle(x$num_times_duplicated$num_duplicates, x$num_times_duplicated$count)
    x$summary$mean_num_times_duplicated <- mean(num_times_duplicated)
    invisible(x)
}
