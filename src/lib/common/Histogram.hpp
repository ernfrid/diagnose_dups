#pragma once

#include <boost/unordered_map.hpp>

#include <algorithm>
#include <stdint.h>
#include <vector>

template<typename T, typename CountType_ = uint64_t>
struct Histogram {
    typedef CountType_ CountType;
    typedef T KeyType;
    typedef boost::unordered_map<KeyType, CountType> Storage;
    typedef typename Storage::iterator iterator;
    typedef typename Storage::const_iterator const_iterator;

    struct Bin {
        KeyType name;
        CountType count;

        Bin()
            : name()
            , count()
        {}

        Bin(KeyType const& name, CountType count)
            : name(name)
            , count(count)
        {}

        bool operator<(Bin const& rhs) const {
            return name < rhs.name;
        }

        bool operator==(Bin const& rhs) const {
            return name == rhs.name && count == rhs.count;
        }
    };

    typedef std::vector<Bin> VectorType;

    CountType& operator[](KeyType const& key) {
        return data[key];
    }

    VectorType as_vector() const {
        VectorType rv;
        rv.reserve(size());
        for (const_iterator i = begin(); i != end(); ++i) {
            rv.push_back(Bin(i->first, i->second));
        }
        return rv;
    }

    VectorType as_sorted_vector() const {
        VectorType rv = as_vector();
        std::sort(rv.begin(), rv.end());
        return rv;
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
