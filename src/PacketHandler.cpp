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

    std::cerr << "Received IPv" << ip_head->version << " packet of protocol "
              << unsigned(ip_head->protocol) << " and size " << ntohs(ip_head->tot_len) << std::endl;

    switch(ip_head->protocol) {
        case IPPROTO_TCP:
            this->handleTCPPacket(ip_head);
            break;
        case IPPROTO_UDP:
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

typedef struct {
    uint32_t src_addr;
    uint32_t dst_addr;
    uint16_t proto;
    uint16_t length;
} _udp_ipv4_pseudo_t;

__attribute__((__unused__))
static uint32_t _udpChecksum(size_t len, uint8_t *data) {
    uint32_t accum = 0;
    for(size_t i = 0; i < len; i++) {
        if(i & 1) {
            accum += (uint32_t)data[i];
        } else {
            accum += (uint32_t)data[i] << 8;
        }
    }
    return accum;
}

int PacketHandler::handleUDPPacket(struct iphdr *_ip_head) {
    struct udphdr *udp_head = (struct udphdr *)((uint8_t *)_ip_head + (_ip_head->ihl * 4));

    size_t   len  = ntohs(udp_head->len) - sizeof(struct udphdr);
    uint8_t *data = (uint8_t *)udp_head + sizeof(udp_head);

    int ret = this->doTamper(len, data);

    /* TODO: For now, we are just disabling the checksum. This will not work on
     * IPv6 packets. */
    udp_head->check = 0;

#if 0
    uint32_t chksum = 0;
    if(_ip_head->version == 4) {
        _udp_ipv4_pseudo_t ph = {
            .src_addr = _ip_head->saddr,
            .dst_addr = _ip_head->daddr,
            .proto    = _ip_head->protocol,
            .length   = udp_head->len
        };
        chksum += _udpChecksum(sizeof(_udp_ipv4_pseudo_t), (uint8_t *)&ph);
    } else {
        /* TODO */
    }
    chksum += _udpChecksum(ntohs(udp_head->len), (uint8_t *)udp_head);

    while(chksum & 0xFFFF0000) {
        chksum = (chksum & 0x0000FFFF) + (chksum >> 16);
    }
    if(chksum != 0xFFFF) {
        chksum = ~chksum;
    }

    udp_head->check = (uint16_t)chksum;
#endif

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
