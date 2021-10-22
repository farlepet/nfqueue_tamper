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


static uint32_t _checksum(size_t len, uint8_t *data) {
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

typedef struct {
    uint32_t src_addr;
    uint32_t dst_addr;
    uint8_t  zero;
    uint8_t  proto;
    uint16_t length;
} __attribute__((__packed__)) _pseudo_header_ipv4_t;

static uint16_t _calcChecksum(const struct iphdr *_ip_head, uint16_t _len) {
    uint32_t chksum = 0;
    
    if(_ip_head->version == 4) {
        _pseudo_header_ipv4_t ph = {
            .src_addr = _ip_head->saddr,
            .dst_addr = _ip_head->daddr,
            .zero     = 0,
            .proto    = _ip_head->protocol,
            .length   = htons(_len)
        };
        chksum += _checksum(sizeof(_pseudo_header_ipv4_t), (uint8_t *)&ph);
    } else {
        std::cerr << "  _calcChecksum: IPv" << (unsigned)_ip_head->version << " unsupported" << std::endl;
        return 0;
    }
    chksum += _checksum(_len, (uint8_t *)_ip_head + (_ip_head->ihl * 4));

    while(chksum & 0xFFFF0000) {
        chksum = (chksum & 0x0000FFFF) + (chksum >> 16);
    }
    if(chksum != 0xFFFF) {
        chksum = ~chksum;
    }

    return (uint16_t)chksum;
}

int PacketHandler::handleTCPPacket(struct iphdr *_ip_head) {
    struct tcphdr *tcp_head = (struct tcphdr *)((uint8_t *)_ip_head + (_ip_head->ihl * 4));
    
    size_t    len = ntohs(_ip_head->tot_len) - ((_ip_head->ihl + tcp_head->doff) * 4);
    uint8_t *data = (uint8_t *)tcp_head + (tcp_head->doff * 4);

    int ret = this->doTamper(len, data);

    tcp_head->check = 0;
    tcp_head->check = htons(_calcChecksum(_ip_head, len + (tcp_head->doff * 4)));

    return ret;
}

int PacketHandler::handleUDPPacket(struct iphdr *_ip_head) {
    struct udphdr *udp_head = (struct udphdr *)((uint8_t *)_ip_head + (_ip_head->ihl * 4));

    size_t   len  = ntohs(udp_head->len) - sizeof(struct udphdr);
    uint8_t *data = (uint8_t *)udp_head + sizeof(udp_head);

    int ret = this->doTamper(len, data);

    udp_head->check = 0;
    udp_head->check = htons(_calcChecksum(_ip_head, ntohs(udp_head->len)));

    return ret;
}

int PacketHandler::doTamper(size_t len, uint8_t *data) {
    if(len == 0) {
        return 0;
    }

    std::uniform_real_distribution<double> rand_dist(0, 0.999);
    
    for(TamperMethod *meth : this->meths) {
        if(meth->getProbability() >= rand_dist(this->rand_engine)) {
            meth->tamper(len, data);
        }
    }

    return 0;
}
