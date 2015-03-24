#pragma once

#include <sam.h>

#include <boost/noncopyable.hpp>

struct BamRecord : public boost::noncopyable {
    BamRecord()
        : data(bam_init1())
    {}

    ~BamRecord() {
        bam_destroy1(data);
    }

    operator bam1_t*() {
        return data;
    }

    operator bam1_t const*() const{
        return data;
    }

    bam1_t const* operator->() const {
        return data;
    }

    bam1_t* operator->() {
        return data;
    }

    bam1_t* data;
};
