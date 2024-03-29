#ifndef _TAMPER_METHOD_RAND_HPP_
#define _TAMPER_METHOD_RAND_HPP_

#include <random>

#include "TamperMethod.hpp"

/**
 * @brief Method used to tamper with packet data using random data manipulation
 */
class TamperMethodRand : public TamperMethod {
    private:
        int offset_min = 0;       /*!< Minimum offset */
        int offset_max = -1;      /*!< Maximum offset */

        bool consecutive = false; /*!< Whether modified regions must be consecutive bytes */

        int size_min = 1;         /*!< Minimum size of modified region */
        int size_max = 1;         /*!< Maximum size of modified region */
    
        std::random_device rand_rd;
        std::mt19937       rand_engine;

    public:
        TamperMethodRand(std::map<std::string, std::string> &_opts);

        int tamper(size_t len, uint8_t *data);
};

#endif /* _TAMPER_METHOD_RAND_HPP_ */
