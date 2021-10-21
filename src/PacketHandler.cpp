#include <iostream>
#include <iomanip>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>

#include "PacketHandler.hpp"

PacketHandler::PacketHandler() :
rand_engine(rand_rd()) {

}

void PacketHandler::addTamperMethod(TamperMethod *_meth) {
    this->meths.push_back(_meth);
}

int PacketHandler::handlePacket(size_t len, uint8_t *data) {
    (void)len;

    struct iphdr *ip_head = (struct iphdr *)data;

    std::cerr << "  IPv" << ip_head->version;
    std::cerr << std::hex <<" Src: " << ip_head->saddr;
    std::cerr << " Dest: " << ip_head->daddr << std::endl << std::dec;
    std::cerr << "  Proto: " << ip_head->protocol << " Size: " << ntohs(ip_head->tot_len) << std::endl;

    switch(ip_head->protocol) {
        case IPPROTO_TCP:
            std::cerr << "  TCP Packet" << std::endl;
            this->handleTCPPacket(ip_head);
            break;
        case IPPROTO_UDP:
            std::cerr << "  UDP Packet" << std::endl;
            this->handleUDPPacket(ip_head);
            break;
        default:
            std::cerr << "  Unhandled packet type: " << ip_head->protocol << std::endl;
            break;
    }

    return 0;
}

int PacketHandler::handleTCPPacket(struct iphdr *_ip_head) {
    (void)_ip_head; /* TODO */

    return 0;
}

int PacketHandler::handleUDPPacket(struct iphdr *_ip_head) {
    struct udphdr *udp_head = (struct udphdr *)((uint8_t *)_ip_head + (_ip_head->ihl * 4));

    size_t   len  = ntohs(udp_head->len) - sizeof(struct udphdr);
    uint8_t *data = (uint8_t *)udp_head + sizeof(udp_head);

    int ret = this->doTamper(len, data);

    /* TODO: For now, we are just disabling the checksum. This will not work on
     * IPv6 packets. */
    udp_head->check = 0;

    return ret;
}

int PacketHandler::doTamper(size_t len, uint8_t *data) {
    std::uniform_real_distribution<double> rand_dist(0, 1);
    
    for(TamperMethod *meth : this->meths) {
        if(meth->getProbability() >= rand_dist(this->rand_engine)) {
            meth->tamper(len, data);
        }
    }

    return 0;
}
