#include <iostream>
#include <random>
#include <regex>

#include "TamperMethod.hpp"
#include "TamperMethods/TamperMethodRand.hpp"
#include "TamperMethods/TamperMethodFixed.hpp"
#include "TamperMethods/TamperMethodReplace.hpp"

TamperMethod::TamperMethod(std::map<std::string, std::string> &_opts) {
    if(_opts.count("chance")) {
        this->probability = std::stod(_opts["chance"]);
    }
}

double TamperMethod::getProbability(void) {
    return this->probability;
}

TamperMethod *TamperMethod::create(std::string &_str) {
    /* TODO: Handle escaped semicolons. */
    const std::regex sep(";");

    std::vector<std::string> vec = std::vector<std::string>(
        std::sregex_token_iterator{begin(_str), end(_str), sep, -1},
        std::sregex_token_iterator{}
    );

    std::string meth = vec[0];
    std::map<std::string, std::string> opts;

    for(size_t i = 1; i < vec.size(); i++) {
        size_t split = vec[i].find('=');
        if(split == std::string::npos) {
            /* TODO: Include more info in error. Also, supporting this might
             * be useful for default-false booleans. */
            throw std::runtime_error("Config option without matching value!");
        }
        
        opts.insert({vec[i].substr(0,split), vec[i].substr(split+1)});
    }

    if(meth == "rand") {
        return new TamperMethodRand(opts);
    } else if(meth == "fixed") {
        return new TamperMethodFixed(opts);
    } else if(meth == "replace") {
        return new TamperMethodReplace(opts);
    } else {
        throw std::runtime_error("Unhandled tamper method: " + meth);
    }

    return NULL;
}

int TamperMethod::parseRange(std::string &_str, int &_min, int &_max) {
    size_t colon = _str.find(':');
    if(colon == 0) {
        /* Bad formatting */
        return -1;
    } else if(colon != std::string::npos) {
        _min = std::stoi(_str.substr(0,colon));
        _max = std::stoi(_str.substr(colon+1));

        if((_min < 0) || (_max < -1) ||
           ((_max < _min) && (_max != -1))) {
            return -1;
        }
    } else {
        _min = _max = std::stoi(_str);
        if(_min < -1) {
            return -1;
        }
    }
    
    return 0;
}
        
int TamperMethod::parseBool(std::string &_str, bool &_val) {
    if(_str == "false" || _str == "0") {
        _val = false;
    } else if(_str == "true" || _str == "1") {
        _val = true;
    } else {
        return -1;
    }

    return 0;
}

int TamperMethod::parseHex(std::string &_str, std::vector<uint8_t> &_val) {
    if((_str.size() == 0) ||
       (_str.size() & 1)) {
        return -1;
    }
    for(size_t i = 0; i < _str.size(); i+=2) {
        uint8_t byte = std::stoi(_str.substr(i,2), NULL, 16);
        _val.push_back(byte);
    }

    return 0;
}


size_t TamperMethod::randRange(int _min, int _max, size_t _sz) {
    static std::random_device rand_rd;
    static std::mt19937       rand_engine(rand_rd());
    
    if(_max == -1 || (size_t)_max > _sz) {
        _max = _sz;
    }
    if(_min == -1 || (size_t)_min > _sz) {
        _min = _sz;
    }

    if(_min == _max) {
        return _min;
    }

    /* TODO: This is likely highly ineffecient */
    std::uniform_int_distribution<size_t> rand_dist(_min, _max);

    return rand_dist(rand_engine);
}
