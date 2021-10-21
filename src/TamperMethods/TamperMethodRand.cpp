#include <iostream>
#include <iomanip>

#include "TamperMethods/TamperMethodRand.hpp"

TamperMethodRand::TamperMethodRand(std::map<std::string, std::string> &_opts) :
TamperMethod(_opts),
rand_engine(rand_rd()) {
    if(_opts.count("off") &&
       TamperMethod::parseRange(_opts["off"], this->offset_min, this->offset_max)) {
        throw std::runtime_error("Badly formatted range for off!");
    }
    if(_opts.count("sz") &&
       TamperMethod::parseRange(_opts["sz"], this->size_min, this->size_max)) {
        throw std::runtime_error("Badly formatted range for sz!");
    }
    if(_opts.count("con") &&
       TamperMethod::parseBool(_opts["con"], this->consecutive)) {
        throw std::runtime_error("Badly formatted boolean for con!");
    }
}

int TamperMethodRand::tamper(size_t len, uint8_t *data) {
    size_t off = TamperMethod::randRange(this->offset_min, this->offset_max, len-1);
    size_t sz  = TamperMethod::randRange(this->size_min,   this->size_max,   len);
    
    if(off + sz > len) {
        sz = len - off;
    }

    std::cerr << "  + TamperMethodRand(off: " << off << ", sz: " << sz << "): ";
    std::cerr << std::setw(2) << std::setfill('0') << std::hex;

    std::uniform_int_distribution<uint8_t> rand_dist(0, 255);

    /* TODO: non-consecutive */
    for(size_t i = off; i < (off + sz); i++) {
        data[i] = rand_dist(this->rand_engine);
        std::cerr << unsigned(data[i]);
    }
    
    std::cerr << std::endl << std::setw(0) << std::setfill(' ') << std::dec;

    return 0;
}
