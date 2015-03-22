#include "Read.hpp"

#include <boost/tokenizer.hpp>

#include<iostream>

Read::Read(bam1_t const* record)
    : insert_size(record->core.isize)
{
    _parse_queryname(record);
}

//This part taken almost directly from Travis' cellstats
inline
bool Read::_parse_queryname(bam1_t const* record) {
    std::string name = bam_get_qname(record);
    boost::char_separator<char> sep(":");
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    tokenizer tokens(name, sep);
    std::size_t count = 0;

    typedef tokenizer::iterator token_iter;
    for (token_iter i = tokens.begin(); i != tokens.end(); ++i, ++count) {
        if (count == 2)
            flowcell = *i;
        else if (count == 3)
            lane = atoi(i->c_str());
        else if (count == 4)
            tile = atoi(i->c_str());
        else if (count == 5)
            x = atoi(i->c_str());
        else if (count == 6) {
            y=atoi(i->c_str());
            return true;
        }
    }

    return false;
}
