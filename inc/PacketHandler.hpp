#ifndef _PACKET_HANDLER_HPP_
#define _PACKET_HANDLER_HPP_

#include <vector>
#include <random>
#include <netinet/ip.h>

#include "TamperMethod.hpp"

/**
 * @brief Handles packets received by the NFQueue, and applies tampers
 */
class PacketHandler {
    private:
        std::vector<TamperMethod *> meths; /*!< List of configured tamper methods, in order they are to be applied */
        
        std::random_device          rand_rd;
        std::default_random_engine  rand_engine;

        int handleTCPPacket(struct iphdr *_ip_head);
        int handleUDPPacket(struct iphdr *_ip_head);

        int doTamper(size_t len, uint8_t *data);

    public:
        PacketHandler();

        /**
         * @brief Add tamper method to the end of the list
         * 
         * @param _meth Tamper method
         */
        void addTamperMethod(TamperMethod *_meth);

        /**
         * @brief Handle a packet
         * 
         * @param len Length of packet data
         * @param data pointer to packet data
         * 
         * @return <Return value currently unused >
         */
        int handlePacket(size_t len, uint8_t *data);
};

#endif /* _PACKET_HANDLER_HPP_ */
