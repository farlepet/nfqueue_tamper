#include <iostream>
#include <iomanip>

#include "TamperMethods/TamperMethodReplace.hpp"

TamperMethodReplace::TamperMethodReplace(std::map<std::string, std::string> &_opts) :
TamperMethod(_opts) {
    if(!_opts.count("off")) {
        throw std::runtime_error("Missing offset for TamperMethodReplace!");
    }
    if(!_opts.count("data")) {
        throw std::runtime_error("Missing data for TamperMethodReplace!");
    }
    if(!_opts.count("seq")) {
        throw std::runtime_error("Missing sequence for TamperMethodReplace!");
    }
    
    this->offset = std::stoi(_opts["off"]);

    if(TamperMethod::parseHex(_opts["data"], this->data)) {
        throw std::runtime_error("Error while parsing data for TamperMethodReplace!");
    }
    if(TamperMethod::parseHex(_opts["seq"], this->sequence)) {
        throw std::runtime_error("Error while parsing sequence for TamperMethodReplace!");
    }

    if(_opts.count("multi") &&
       TamperMethod::parseBool(_opts["multi"], this->multi)) {
        throw std::runtime_error("Error while parsing multi for TamperMethodReplace!");
    }
}

int TamperMethodReplace::tamper(size_t len, uint8_t *data) {
    size_t search_sz = this->sequence.size();

    for(size_t i = 0; i < (len - search_sz); i++) {
        size_t j = 0;
        for(; j < search_sz; j++) {
            if(data[i+j] != this->sequence[j]) {
                break;
            }
        }
        if(j == search_sz) {
            /* Sequence match */
            size_t sz  = this->data.size();
            size_t off = i;
            size_t idx = 0;

            if(((ssize_t)off + this->offset) < 0) {
                idx = (size_t)-((ssize_t)off + this->offset);
                if(idx >= sz) {
                    /* Entire insertion is before the payload */
                    continue;
                }
                sz -= idx;
                off = 0;
            } else {
                off += this->offset;
            }

            if(off >= len) {
                break;
            } else if((off + sz) > len) {
                /* Insertion continues beyond end of payload */
                sz = len - off;
            }
    
            std::cerr << "  + TamperMethodReplace(off: " << off << ", sz: " << sz << "): ";
            std::cerr << std::setw(2) << std::setfill('0') << std::hex;
            for(size_t k = 0; k < sz; k++) {
                data[off + k] = this->data[idx + k];
                std::cerr << unsigned(data[off + k]);
            }
            std::cerr << std::endl << std::setw(0) << std::setfill(' ') << std::dec;

            if(!this->multi) {
                break;
            }

            i += (search_sz - 1);
        }
    }

    return 0;
}
