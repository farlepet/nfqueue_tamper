#ifndef _TAMPER_METHOD_RAND_HPP_
#define _TAMPER_METHOD_RAND_HPP_

#include <random>

#include "TamperMethod.hpp"

class TamperMethodRand : public TamperMethod {
    private:
        int offset_min = 0;  /*!< Minimum offset */
        int offset_max = -1; /*!< Maximum offset */

        bool consecutive = false;

        int size_min = 1;
        int size_max = 1;
    
        std::random_device         rand_rd;
        std::default_random_engine rand_engine;

    public:
        TamperMethodRand(std::map<std::string, std::string> &_opts);

        int tamper(size_t len, uint8_t *data);
};

#endif /* _TAMPER_METHOD_RAND_HPP_ */
