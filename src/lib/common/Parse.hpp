#pragma once

#include <boost/spirit/include/qi_numeric.hpp>
#include <boost/spirit/include/qi_string.hpp>
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
