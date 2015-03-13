#include <boost/program_options.hpp>

#include <string>

using namespace std;
namespace po = boost::program_options;


class Options {
    private:
        //TODO make certain that the following is eventually inline and is const correct and not copying objects.
        po::options_description _options_description();
        po::positional_options_description _positional_description();
    public:
        po::variables_map vm;

        Options(int argc, char** argv);
};




