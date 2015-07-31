#pragma once

#include <sam.h>

#include <boost/format.hpp>

#include <stdexcept>

class SamReader {
public:
    SamReader(char const* path, char const* mode)
        : path_(path)
        , fp_(hts_open(path, mode))
        , required_flags_(0)
        , skip_flags_(0)
        , record_count_(0)
        , skipped_count_(0)
    {
        using boost::format;
        if (!fp_ || !(header_ = sam_hdr_read(fp_))) {
            throw std::runtime_error(str(format(
                "Failed to open input file %1%"
                ) % path));
        }
    }

    ~SamReader() {
        if (header_)
            bam_hdr_destroy(header_);

        if (fp_)
            hts_close(fp_);
    }

    void required_flags(uint32_t flags) {
        required_flags_ = flags;
    }

    void skip_flags(uint32_t flags) {
        skip_flags_ = flags;
    }

    bool want(uint32_t flag) const {
        return (flag & required_flags_) == required_flags_
            && (flag & skip_flags_) == 0;
    }

    bool next(bam1_t* record) {
        while (sam_read1(fp_, header_, record) > 0) {
            if (want(record->core.flag)) {
                ++record_count_;
                return true;
            }
            else {
                ++skipped_count_;
            }
        }
        return false;
    }

    std::size_t record_count() const {
        return record_count_;
    }

    std::size_t skipped_count() const {
        return skipped_count_;
    }

    char const* path() const {
        return path_;
    }

private:
    char const* path_;
    htsFile* fp_;
    bam_hdr_t* header_;
    uint32_t required_flags_;
    uint32_t skip_flags_;
    std::size_t record_count_;
    std::size_t skipped_count_;
};
