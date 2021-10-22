#ifndef _TAMPER_METHOD_HPP_
#define _TAMPER_METHOD_HPP_

#include <string>
#include <map>

/**
 * @brief Method used to tamper with packet data
 */
class TamperMethod {
    private:
        double probability = 1; /*!< Probability between 0 and 1 that this tamper method will act on a given packet */

    public:
        /**
         * @brief Initialize a new tamper method
         * 
         * @param _opts Map of supplied configuration options
         */
        TamperMethod(std::map<std::string, std::string> &_opts);

        /**
         * @brief Apply tampering on packet data
         * 
         * @param len Length of data
         * @param data Pointer to modifiable data
         * 
         * @return <Return value currently unused, returns 0>
         */
        virtual int tamper(size_t len, uint8_t *data) = 0;

        /**
         * @brief Get the configured execution probability
         */
        double getProbability(void);




        /**
         * @brief Create TamperMethod from configuration string
         * 
         * @param _str Configuration string
         */
        static TamperMethod *create(std::string &_str);

        /**
         * @brief Parse range string from configuration
         * 
         * The following formats are currently supported:
         *   <val>
         *   -1
         *   <min>:<max>
         *   <min>:-1
         * 
         * @param _str Configuration value string
         * @param _min Output minimum
         * @param _max Output maximum
         * 
         * @return 0 on success, < 0 on failure
         */
        static int parseRange(std::string &_str, int &_min, int &_max);
        
        /**
         * @brief Parse boolean string from configuration
         * 
         * The following formats are currently supported:
         *   0
         *   false
         *   1
         *   true
         * 
         * @param _str Configuration value string
         * @param _val Output boolean
         * 
         * @return 0 on success, < 0 on failure
         */
        static int parseBool(std::string &_str, bool &_val);

        /**
         * @brief Parse hex string from configuration
         * 
         * The format is byte-ordered groups of two hexadecimal characters:
         *   <byte0>[<byte1>[<byte2>[...]]]
         *   
         * Where each of <byteX> is a sequence of two characters, e.g.:
         *   C0FFEE1234
         * 
         * @param _str Configuration value string
         * @param _val Output byte vector
         * 
         * @return 0 on success, < 0 on failure
         */
        static int parseHex(std::string &_str, std::vector<uint8_t> &_val);

        /**
         * @brief Generates a random non-negative number in the given range
         * 
         * @param _min Minimum value, -1 = _sz
         * @param _max Maximum value, -1 = _sz
         * @param _sz Absolute limit
         * 
         * @return Random value
         */
        static size_t randRange(int _min, int _max, size_t _sz);
};

#endif /* _TAMPER_METHOD_HPP_ */
