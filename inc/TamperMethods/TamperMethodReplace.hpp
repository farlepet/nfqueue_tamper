#ifndef _TAMPER_METHOD_REPLACE_HPP_
#define _TAMPER_METHOD_REPLACE_HPP_

#include <vector>

#include "TamperMethod.hpp"

/**
 * @brief Method used to tamper with packet data using search-and-replace with offset
 */
class TamperMethodReplace : public TamperMethod {
    private:
        int                  offset = 0;    /*!< Offset into sequence in which to insert data */
        bool                 multi = false; /*!< Whether to allow multiple replacements per packet */
        std::vector<uint8_t> data;          /*!< Data to insert */
        std::vector<uint8_t> sequence;      /*!< Sequence to search for */

    public:
        TamperMethodReplace(std::map<std::string, std::string> &_opts);

        int tamper(size_t len, uint8_t *data);
};

#endif /* _TAMPER_METHOD_REPLACE_HPP_ */
