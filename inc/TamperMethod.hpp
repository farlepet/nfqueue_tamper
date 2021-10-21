#ifndef _TAMPER_METHOD_HPP_
#define _TAMPER_METHOD_HPP_

#include <string>
#include <map>

class TamperMethod {
    private:
        double probability = 1; /*!< Probability between 0 and 1 that this tamper method will act on a given packet */

    public:
        TamperMethod(std::map<std::string, std::string> &_opts);

        virtual int tamper(size_t len, uint8_t *data) = 0;

        double getProbability(void);

        static TamperMethod *create(std::string &_str);

        static int parseRange(std::string &_str, int &_min, int &_max);
        static int parseBool(std::string &_str, bool &_val);

        static int randRange(int _min, int _max, int _sz);
};

#endif /* _TAMPER_METHOD_HPP_ */
