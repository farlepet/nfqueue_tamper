#ifndef _PACKET_HANDLER_HPP_
#define _PACKET_HANDLER_HPP_

#include <vector>
#include <random>
#include <netinet/ip.h>

#include "TamperMethod.hpp"

class PacketHandler {
    private:
        std::vector<TamperMethod *> meths;
        
        std::random_device          rand_rd;
        std::default_random_engine  rand_engine;

        int handleTCPPacket(struct iphdr *_ip_head);
        int handleUDPPacket(struct iphdr *_ip_head);

        int doTamper(size_t len, uint8_t *data);

    public:
        PacketHandler();

        void addTamperMethod(TamperMethod *_meth);

        int handlePacket(size_t len, uint8_t *data);
};

#endif /* _PACKET_HANDLER_HPP_ */
