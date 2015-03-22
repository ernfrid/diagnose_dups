#include "Read.hpp"

#include "common/Parse.hpp"

#include <algorithm>

bool parse_read(bam1_t const* record, Read& read) {
    read.insert_size = record->core.isize;

    char const* name = bam_get_qname(record);

    // l_qname includes the null terminator
    SimpleTokenizer tok(name, name + record->core.l_qname - 1, ':');

    if (tok.skip(2) != 2)
        return false;

    if (!tok.extract(read.flowcell))
        return false;

    if (!tok.extract(read.lane))
        return false;

    if (!tok.extract(read.tile))
        return false;

    if (!tok.extract(read.x))
        return false;

    if (!tok.extract(read.y))
        return false;

    return true;
}
