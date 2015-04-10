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
