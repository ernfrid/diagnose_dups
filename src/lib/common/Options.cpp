#include "Options.hpp"

#include "version.h"

#include <iostream>
#include <cstdlib>

using namespace std;
namespace po = boost::program_options;


Options::Options(int argc, char** argv) {
    try {
        po::options_description desc = _options_description();
        po::positional_options_description p = _positional_description();

        po::store(po::command_line_parser(argc, argv).
                options(desc).positional(p).run(), vm);
        po::notify(vm);

        if (vm.count("help")) {
            cerr << desc << "\n";
            exit(0);
        }

        if (vm.count("version")) {
            cerr << "version: " << __g_prog_version
                << " (commit " << __g_commit_hash << ")\n";
            exit(0);
        }
    }
    catch(const std::exception& ex) {
        cerr << "Error: " << ex.what() << "\n";
        exit(1); //We had an error so die with an exit code
    }
}

po::options_description Options::_options_description() {
    po::options_description desc("Available options");
    desc.add_options()
        ("help,h", "produce this message")
        ("version,v", "output the version")
        ("input,i", po::value<>(&input_file)->default_value("-"), "the input file")
        ("output,o", po::value<>(&output_file)->default_value("-"), "the output file")
        ("thread,t", po::bool_switch(&use_io_thread)->default_value(false),
            "use a separate thread for bam io")
        ("buffer-size,b", po::value<>(&ring_buffer_size)->default_value(4096),
            "size of the ring buffer between io and processing thread (# bam records). "
            "ignored without -t/--thread")
        ;
    return desc;
}

po::positional_options_description Options::_positional_description() {
    //define positional arguments for convenience
    po::positional_options_description p;
    p.add("input", 1);
    p.add("output", 1);
    return p;
}


