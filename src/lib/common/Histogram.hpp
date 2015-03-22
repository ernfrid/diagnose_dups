#pragma once

#include <boost/unordered_map.hpp>

#include <stdint.h>

template<typename T, typename CountType_ = uint64_t>
struct Histogram {
    typedef CountType_ CountType;
    typedef T KeyType;
    typedef boost::unordered_map<KeyType, CountType> Storage;
    typedef typename Storage::iterator iterator;
    typedef typename Storage::const_iterator const_iterator;

    CountType& operator[](KeyType const& key) {
        return data[key];
    }

    iterator begin() {
        return data.begin();
    }

    iterator end() {
        return data.end();
    }

    const_iterator begin() const {
        return data.begin();
    }

    const_iterator end() const {
        return data.end();
    }

    void merge(Histogram const& other) {
        for (const_iterator i = other.begin(); i != other.end(); ++i) {
            data[i->first] += i->second;
        }
    }

    friend bool operator==(Histogram const& x, Histogram const& y) {
        return x.data == y.data;
    }

    std::size_t size() const {
        return data.size();
    }

    Storage data;
};
