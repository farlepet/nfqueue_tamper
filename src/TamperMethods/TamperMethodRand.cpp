#include <iostream>

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
    int off = TamperMethod::randRange(this->offset_min, this->offset_max, len);
    int sz  = TamperMethod::randRange(this->size_min,   this->size_max,   len);
    
    std::cerr << "Off: " << off << ", Sz: " << sz << std::endl;

    std::uniform_int_distribution<uint8_t> rand_dist(0, 255);

    /* TODO: non-consecutive */
    for(int i = off; i < (off + sz); i++) {
        data[i] = rand_dist(this->rand_engine);
    }

    return 0;
}
