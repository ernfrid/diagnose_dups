#include "Read.hpp"

#include <boost/tokenizer.hpp>

bool parse_read(bam1_t const* record, Read& read) {
    read.insert_size = record->core.isize;

    std::string name = bam_get_qname(record);
    boost::char_separator<char> sep(":");
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    tokenizer tokens(name, sep);
    std::size_t count = 0;

    typedef tokenizer::iterator token_iter;
    for (token_iter i = tokens.begin(); i != tokens.end(); ++i, ++count) {
        if (count == 2)
            read.flowcell = *i;
        else if (count == 3)
            read.lane = atoi(i->c_str());
        else if (count == 4)
            read.tile = atoi(i->c_str());
        else if (count == 5)
            read.x = atoi(i->c_str());
        else if (count == 6) {
            read.y = atoi(i->c_str());
            return true;
        }
    }

    return false;
}
