#pragma once

#include <sam.h>
#include <boost/functional/hash.hpp>

#include <stdint.h>

//the intent is for this class to store samblaster signatures
//they will be used as hash keys in an unordered map
//they will be created directly from a bam1_t record

class Signature {
    private:
    int32_t _calculate_position(bam1_t const* record);
    int32_t _calculate_mate_position(bam1_t const* record);

    inline int32_t _calculate_right_offset(bam1_t const* record);
    inline int32_t _calculate_right_offset(char const* cigar);
    inline int32_t _calculate_left_offset(bam1_t const* record);
    inline int32_t _calculate_left_offset(char const* cigar);

    public:
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

    bool operator==(Signature const& rhs) const {
        return tid == rhs.tid
            && mtid == rhs.mtid
            && pos == rhs.pos
            && mpos == rhs.mpos
            && reverse == rhs.reverse
            && mreverse == rhs.mreverse;
    }

    friend std::size_t hash_value(Signature const& sig) {
        std::size_t seed = 0;
        boost::hash_combine(seed, sig.tid);
        boost::hash_combine(seed, sig.mtid);
        boost::hash_combine(seed, sig.pos);
        boost::hash_combine(seed, sig.mpos);
        boost::hash_combine(seed, sig.reverse);
        boost::hash_combine(seed, sig.mreverse);
        return seed;
    }

};
