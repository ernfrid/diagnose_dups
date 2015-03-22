#pragma once

#include <boost/spirit/include/qi_numeric.hpp>
#include <boost/spirit/include/qi_auto.hpp>
#include <boost/spirit/include/qi_parse.hpp>

// This is similar to the basic boost::spirit::qi parsing protocol.
//
// Given begin and end iterators and a value, attempts to parse it
// into value and updates the begin iterator to point to the
// character after the last part of the match.
//
// Returns true if a value was extracted, false otherwise. Note that
// it may be the case that a partial match happened, e.g.,:
//
// std:string data = "123abc";
// auto beg = data.begin();
// auto end = data.end();
// int x;
//
// bool rv = auto_parse(beg, end, x);
//
// In this case, rv == true, and beg points to 'a'. If you want to enforce
// a full match, you have to check that beg == end after calling.
template<typename Iter, typename T>
bool auto_parse(Iter& beg, Iter const& end, T& value) {
    namespace qi = boost::spirit::qi;
    return qi::parse(beg, end, qi::auto_, value);
}

template<typename Iter>
bool auto_parse(Iter& beg, Iter const& end, std::string& value) {
    value.assign(beg, end);
    beg = end;
    return true;
}

// Simple string tokenization
struct SimpleTokenizer {
    SimpleTokenizer(char const* beg, char const* end, char delim)
        : beg(beg)
        , end(end)
        , pos(beg)
        , delim(delim)
    {
        assert(end >= beg);
    }

    SimpleTokenizer(std::string const& s, char delim)
        : beg(s.data())
        , end(s.data() + s.size())
        , pos(s.data())
        , delim(delim)
    {
        assert(end >= beg);
    }

    char const* next_delim() const {
        assert(end >= beg);
        return std::find(beg, end, delim);
    }

    // skip 'count' fields
    // returns the number of fields actually skipped (may be less if
    // end of string is encountered)
    unsigned skip(unsigned count) {
        unsigned i;
        for (i = 0; i < count; ++i) {
            char const* ndelim = next_delim();
            if (ndelim == end)
                return i;
            beg = ndelim + 1;
        }
        return i;
    }

    template<typename T>
    bool extract(T& value) {
        if (beg == end)
            return false;

        char const* ndelim = next_delim();
        char const* save = beg;
        if (!auto_parse(beg, ndelim, value) || beg != ndelim) {
            beg = save;
            return false;
        }

        beg = ndelim;
        if (beg != end)
            ++beg;

        return true;
    }

private:
    char const* beg;
    char const* end;
    char const* pos;
    char delim;
};
