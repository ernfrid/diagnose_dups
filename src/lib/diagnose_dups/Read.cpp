#include "Read.hpp"

#include "common/Parse.hpp"

#include <boost/format.hpp>

#include <algorithm>
#include <stdexcept>

using boost::format;

void parse_read(bam1_t const* record, Read& read) {
    read.insert_size = record->core.isize;
    read.is_read1 = record->core.flag & BAM_FREAD1; //BAM_FREAD2 will not be examined explicitly
    read.ignore = false;

    char const* name = bam_get_qname(record);

    // l_qname includes the null terminator
    SimpleTokenizer tok(name, name + record->core.l_qname - 1, ':');

    if (tok.skip(2) != 2)
        throw std::runtime_error(str(format(
                        "Error parsing read name %1%"
                        ) % name));

    if (!tok.extract(read.tile.flowcell))
        throw std::runtime_error(str(format(
                        "Error parsing read name %1%"
                        ) % name));

    if (!tok.extract(read.tile.lane))
        throw std::runtime_error(str(format(
                        "Error parsing read name %1%"
                        ) % name));

    if (!tok.extract(read.tile.id))
        throw std::runtime_error(str(format(
                        "Error parsing read name %1%"
                        ) % name));

    if (!tok.extract(read.x))
        throw std::runtime_error(str(format(
                        "Error parsing read name %1%"
                        ) % name));

    if (!tok.extract(read.y))
        throw std::runtime_error(str(format(
                        "Error parsing read name %1%"
                        ) % name));

    //generate subtiles
    read.tile.subtile_x = read.x / 1000;
    read.tile.subtile_y = read.y / 1000;

}
