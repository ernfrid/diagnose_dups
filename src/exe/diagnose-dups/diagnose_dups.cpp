#include "common/Histogram.hpp"
#include "common/Options.hpp"
#include "diagnose_dups/SignatureBuffer.hpp"
#include "diagnose_dups/BufferProcessor.hpp"
#include "diagnose_dups/Read.hpp"
#include "diagnose_dups/Signature.hpp"
#include "io/BamRecord.hpp"
#include "io/SamReader.hpp"
#include "io/SamProducer.hpp"

#include <sam.h>

#include <boost/format.hpp>
#include <boost/timer/timer.hpp>
#include <boost/scoped_ptr.hpp>

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
        reader.skip_flags(BAM_FSECONDARY | BAM_FQCFAIL | BAM_FSUPPLEMENTARY);

        boost::scoped_ptr<SamProducerBase> prod;
        if (opts.use_io_thread)
            prod.reset(new SamProducerThread(reader, opts.ring_buffer_size));
        else
            prod.reset(new SamProducer(reader));

        BufferProcessor proc;
        SignatureBuffer buffer(1000, proc);

        prod->for_each_record(buffer);
        prod.reset(); // will stop and join if using an io thread

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
