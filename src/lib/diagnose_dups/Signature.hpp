#pragma once

#include <sam.h>
#include <boost/functional/hash.hpp>

#include <stdint.h>

//the intent is for this class to store samblaster signatures
//they will be created directly from a bam1_t record

struct Signature {
    int32_t tid;
    int32_t mtid;
    int32_t pos;
    int32_t mpos;
    bool reverse;
    bool mreverse;

    Signature()
        : tid(-1)
        , mtid(-1)
        , pos(-1)
        , mpos(-1)
        , reverse(false)
        , mreverse(false)
    {}

    Signature(bam1_t const* record);

    void parse(bam1_t const* record);
    int32_t _calculate_position(bam1_t const* record);
    int32_t _calculate_mate_position(bam1_t const* record);

    friend bool operator==(Signature const& lhs, Signature const& rhs) {
        return lhs.tid == rhs.tid
            && lhs.mtid == rhs.mtid
            && lhs.pos == rhs.pos
            && lhs.mpos == rhs.mpos
            && lhs.reverse == rhs.reverse
            && lhs.mreverse == rhs.mreverse;
    }

    //FIXME The below is ugly. Think about a clear, but concise way to do this.
    friend bool operator<(Signature const& lhs, Signature const& rhs) {
        if (lhs.tid < rhs.tid) {
            return true;
        }
        else {
            if (lhs.tid == rhs.tid) {
                if (lhs.pos < rhs.pos) {
                    return true;
                }
                else if (lhs.pos == rhs.pos) {
                    if (lhs.mtid < rhs.mtid) {
                        return true;
                    }
                    else if (lhs.mtid == rhs.mtid) {
                        if (lhs.mpos < rhs.mpos) {
                            return true;
                        }
                        else if (lhs.mpos == rhs.mpos) {
                            //this should upcast the bools to int
                            //false will be 0
                            //true will be 1
                            return lhs.reverse < rhs.reverse
                                || lhs.mreverse < rhs.mreverse;
                        }
                    }
                }
            }
        }
        return false;
    }

    friend std::size_t hash_value(Signature const& sig) {
        std::size_t seed = boost::hash_value(sig.tid);
        boost::hash_combine(seed, sig.mtid);
        boost::hash_combine(seed, sig.pos);
        boost::hash_combine(seed, sig.mpos);
        boost::hash_combine(seed, sig.reverse);
        boost::hash_combine(seed, sig.mreverse);
        return seed;
    }

};
