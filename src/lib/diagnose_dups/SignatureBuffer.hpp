#pragma once

#include "Signature.hpp"
#include "Read.hpp"
#include "BundleProcessor.hpp"
#include "io/BamRecord.hpp"

#include <map>
#include <vector>
#include <algorithm>

class SignatureBuffer {
    public:
        typedef std::map<Signature, std::vector<Read> > SigRead;
        typedef SigRead::iterator SigReadIter;

        SignatureBuffer(int32_t window_size, BundleProcessor processor) 
            : window_(window_size)
            , processor_(processor) {
            }

        void add(bam1_t const* record) {
            Signature sig(record);//TODO this might throw
            Read read;
            parse_read(record, read);//TODO this might throw?
            process(sig);
            buffer_[sig].push_back(read);
        }

        void process(Signature const& last_sig) {
            if(buffer_.empty()) {
                return;
            }
            SigReadIter i = buffer_.begin();
            //FIXME This should use < or > operator once we figure out how those work
            while (i != buffer_.end() && (i->first.tid < last_sig.tid || (i->first.tid == last_sig.tid && i->first.pos + window_ < last_sig.pos))) {
                processor_(i->second);
                i++;
            }
            //TODO Ensure that i is in fact the last thing we want to erase
            //Alternatively, could probably remove i - 1
            buffer_.erase(buffer_.begin(), i);
        }

        void process() {
            for (SigReadIter i = buffer_.begin(); i != buffer_.end(); ++i)
                processor_(i->second);
        }


        void write_output(std::ostream& os) {
            processor_.write_output(os);
        }

    private:
        int32_t window_;
        BundleProcessor processor_;
        std::map<Signature, std::vector<Read> > buffer_;
};
