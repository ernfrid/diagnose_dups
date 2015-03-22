#pragma once

#include "Signature.hpp"
#include "Read.hpp"
#include "io/BamRecord.hpp"

#include <algorithm>

struct SigRead {
    Signature sig;
    Read read;
};

class SignatureBundle {
public:
    SignatureBundle()
        : tid_(-1)
        , max_pos_(-1)
    {
        data_.reserve(256);
    }

    int add(BamRecord const& record) {
        SigRead item;
        if (!parse_read(record, item.read))
            return -1;

        item.sig.parse(record);

        if (tid_ == -1) {
            tid_ = item.sig.tid;
            max_pos_ = item.sig.pos;
        }

        if (tid_ != record->core.tid || record->core.pos > max_pos_)
            return 0;

        max_pos_ = std::max(max_pos_, item.sig.pos);
        data_.push_back(item);

        return 1;
    }

    std::size_t size() const {
        return data_.size();
    }

    void clear() {
        data_.clear();
        tid_ = -1;
        max_pos_ = -1;
    }

    std::vector<SigRead> const& data() const {
        return data_;
    }

private:
    int32_t tid_;
    int32_t max_pos_;
    std::vector<SigRead> data_;
};
