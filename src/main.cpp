#include <iostream>
#include <string>
#include <vector>
#include <boost/program_options.hpp>

#include "NFQueue.hpp"
#include "TamperMethod.hpp"
#include "PacketHandler.hpp"

static struct {
    std::vector<std::string> tamperMethods;
    int                      queueNumber;
} _options;

static int _handleCmdLine(int, char **);

int main(int argc, char **argv) {
    int ret = _handleCmdLine(argc, argv);
    if(ret) {
        if (ret > 0) {
            /* Non-error exit case */
            return 0;
        } else {
            return 1;
        }
    }

    try {
        PacketHandler pHandler;

        for(std::string &meth : _options.tamperMethods) {
            pHandler.addTamperMethod(TamperMethod::create(meth));
        }

        NFQueue nfqueue(_options.queueNumber, pHandler);
        nfqueue.open();
        nfqueue.run();
        /* TODO: Run in a different thread, close on keyboard interrupt. */
        nfqueue.close();
    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    } catch(...) {
        std::cerr << "Unknown Exception" << std::endl;
    }

    return 0;
}

namespace po = boost::program_options;

static int _handleCmdLine(int argc, char **argv) {
    po::options_description desc("Program options");

    /* TODO: Clean this up if more options are added. */
    desc.add_options()
        ("help,h",   "Print this help message")
        ("tamper,t", po::value<std::vector<std::string>>(&_options.tamperMethods)->multitoken(), "Tamper method")
        ("queue,q", po::value<int>(&_options.queueNumber)->default_value(0), "Queue number");

    po::variables_map vmap;
    po::store(po::parse_command_line(argc, argv, desc), vmap);
    po::notify(vmap);

    if(vmap.count("help")) {
        std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
        std::cout << desc << std::endl;
        return 1;
    }

    return 0;
}
