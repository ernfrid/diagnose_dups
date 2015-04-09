#include "Read.hpp"

#include "common/Parse.hpp"

#include <algorithm>

bool parse_read(bam1_t const* record, Read& read) {
    read.insert_size = record->core.isize;
    read.is_read1 = record->core.flag & BAM_FREAD1; //BAM_FREAD2 will not be examined explicitly

    char const* name = bam_get_qname(record);

    // l_qname includes the null terminator
    SimpleTokenizer tok(name, name + record->core.l_qname - 1, ':');

    if (tok.skip(2) != 2)
        return false;

    if (!tok.extract(read.tile.flowcell))
        return false;

    if (!tok.extract(read.tile.lane))
        return false;

    if (!tok.extract(read.tile.id))
        return false;

    if (!tok.extract(read.x))
        return false;

    if (!tok.extract(read.y))
        return false;

    //generate subtiles
    read.tile.subtile_x = read.x / 1000;
    read.tile.subtile_y = read.y / 1000;

    return true;
}
