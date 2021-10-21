#ifndef _TAMPER_METHOD_FIXED_HPP_
#define _TAMPER_METHOD_FIXED_HPP_

#include <vector>

#include "TamperMethod.hpp"

/**
 * @brief Method used to tamper with packet data using fixed data manipulation
 */
class TamperMethodFixed : public TamperMethod {
    private:
        int                  offset = 0; /*!< Offset at which to insert data */
        std::vector<uint8_t> data;       /*!< Data to insert */

    public:
        TamperMethodFixed(std::map<std::string, std::string> &_opts);

        int tamper(size_t len, uint8_t *data);
};

#endif /* _TAMPER_METHOD_FIXED_HPP_ */
