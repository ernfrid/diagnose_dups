#include "common/Histogram.hpp"
#include "common/Options.hpp"
#include "diagnose_dups/SignatureBuffer.hpp"
#include "diagnose_dups/BundleProcessor.hpp"
#include "diagnose_dups/Read.hpp"
#include "diagnose_dups/Signature.hpp"
#include "io/BamRecord.hpp"
#include "io/SamReader.hpp"

#include <sam.h>

#include <boost/format.hpp>
#include <boost/timer/timer.hpp>

#include <fstream>
#include <iostream>
#include <iomanip>
#include <stdexcept>

using namespace std;
using boost::format;

namespace {
    void run(Options const& opts) {
        boost::timer::auto_cpu_timer timer(std::cerr);
        std::ofstream out;
        std::ostream* out_ptr = &std::cout;
        if (!opts.output_file.empty() && opts.output_file != "-") {
            out.open(opts.output_file.c_str());
            if (!out) {
                throw std::runtime_error(str(format(
                    "Failed to open output file %1%"
                    ) % opts.output_file));
            }
            out_ptr = &out;
        }

        SamReader reader(opts.input_file.c_str(), "r");
        reader.required_flags(BAM_FPROPER_PAIR);
        reader.skip_flags(BAM_FSECONDARY | BAM_FQCFAIL | BAM_FREAD2 | BAM_FSUPPLEMENTARY);

        BamRecord record;
        BundleProcessor proc;
        SignatureBuffer buffer(1000, proc);
        std::size_t parse_failures = 0;
        while (reader.next(record)) {
            buffer.add(record);
            /*
            int rv = bundle.add(record);
            if (rv == -1) {
                if (++parse_failures <= 5) {
                    std::cerr << "Failed to parse bam record, name = "
                        << bam_get_qname(record) << "\n";
                    if (parse_failures == 5)
                        std::cerr << "max warning limit reached, disabling...\n";
                }

                // XXX: would you rather abort?
                continue;
            }
            else if (rv == 0) {
                proc.process(bundle);
                bundle.clear();
                //if rv was 0 then the attempt to previously add this read
                //failed and it needs to be re-added to this fresh bundle
                bundle.add(record);
            }
        */
        }
        if (parse_failures) {
            double pct = parse_failures * 100.0;
            pct /= reader.record_count();

            std::cerr << "\nWARNING: failed to parse read names for " << parse_failures
                << " / " << reader.record_count() << " records ("
                << fixed << setprecision(2) << pct << "%).\n\n";
        }

        // don't forget the rest of the buffer
        buffer.process_all();
        buffer.write_output(*out_ptr);
    }
}

int main(int argc, char** argv) {
    try {
        Options opts = Options(argc, argv);
        run(opts);
    }
    catch (std::exception const& ex) {
        std::cerr << "ERROR: " << ex.what() << "\n";
        return 1;
    }

    return 0;
}
