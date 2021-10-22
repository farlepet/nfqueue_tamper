#include <iostream>
#include <iomanip>

#include "TamperMethods/TamperMethodFixed.hpp"

TamperMethodFixed::TamperMethodFixed(std::map<std::string, std::string> &_opts) :
TamperMethod(_opts) {
    if(!_opts.count("off")) {
        throw std::runtime_error("Missing offset for TamperMethodFixed!");
    }
    if(!_opts.count("data")) {
        throw std::runtime_error("Missing data for TamperMethodFixed!");
    }
    
    this->offset = std::stoi(_opts["off"]);
    if(this->offset < 0) {
        throw std::runtime_error("offset<0 for TamperMethodFixed!");
    }

    if(TamperMethod::parseHex(_opts["data"], this->data)) {
        throw std::runtime_error("Error while parsing data for TamperMethodFixed!");
    }
}

int TamperMethodFixed::tamper(size_t len, uint8_t *data) {
    size_t sz  = this->data.size();
    size_t off = this->offset;
    
    if(off >= len) {
        return 0;
    }
    
    if((off + sz) > len) {
        sz = len - off;
    }

    std::cerr << "  + TamperMethodFixed(off: " << off << ", sz: " << sz << "): ";
    std::cerr << std::setw(2) << std::setfill('0') << std::hex;

    for(size_t i = 0; i < sz; i++) {
        data[off + i] = this->data[i];
        std::cerr << unsigned(data[off + i]);
    }
    
    std::cerr << std::endl << std::setw(0) << std::setfill(' ') << std::dec;

    return 0;
}
