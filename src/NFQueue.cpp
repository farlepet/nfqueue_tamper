#include <iostream>
#include <iomanip>
#include <string.h>
#include <netinet/in.h>
#include <linux/netfilter.h>

#include "NFQueue.hpp"

int NFQueue::handle_callback(struct nfq_q_handle *_queue, struct nfgenmsg *_nfmsg, struct nfq_data *_data, void *_class) {
    return reinterpret_cast<NFQueue *>(_class)->callback(_queue, _nfmsg, _data);
}

NFQueue::NFQueue(int _queue, PacketHandler &_pHandler) :
pHandler(_pHandler) {
    this->queue = _queue;
}

void NFQueue::open(void) {
    this->nfq_hand = nfq_open();
    if(this->nfq_hand == NULL) {
        throw std::runtime_error("Could not open NFQ handle!");
    }

    if(nfq_unbind_pf(this->nfq_hand, AF_INET) < 0) {
        throw std::runtime_error("nfq_unbind_pf");
    }
    
    if(nfq_bind_pf(this->nfq_hand, AF_INET) < 0) {
        throw std::runtime_error("nfq_bind_pf");
    }
    
    this->nfq_queue = nfq_create_queue(this->nfq_hand, this->queue, NFQueue::handle_callback, this);
    if(this->nfq_queue == NULL) {
        throw std::runtime_error("nfq_create_queue");
    }
    
    if(nfq_set_mode(this->nfq_queue, NFQNL_COPY_PACKET, 0xFFFF) < 0) {
        throw std::runtime_error("nfq_set_mode");
    }

    this->_nfq_fd = nfq_fd(this->nfq_hand);
}

void NFQueue::run(void) {
    int len;
    while((len = recv(this->_nfq_fd, this->buff, NFQUEUE_BUFF_SZ, 0))) {
        nfq_handle_packet(this->nfq_hand, (char *)this->buff, len);
    }
}

void NFQueue::close(void) {
    if(nfq_destroy_queue(this->nfq_queue) < 0) {
        throw std::runtime_error("nfq_destroy_queue");
    }
    
    if(nfq_close(this->nfq_hand) < 0) {
        throw std::runtime_error("nfq_close");
    }
}


int NFQueue::callback(struct nfq_q_handle *_queue, struct nfgenmsg *_nfmsg, struct nfq_data *_data) {
    (void)_nfmsg;

    uint32_t                     id;
    struct nfqnl_msg_packet_hdr *head;
    uint8_t                     *payload;
    size_t                       payload_len;

    std::cerr << "---------------------------" << std::endl;

    head = nfq_get_msg_packet_hdr(_data);
    id   = ntohl(head->packet_id);

    /*std::cerr << "  Proto: " << std::hex << ntohs(head->hw_protocol) << std::dec;
    std::cerr << ", hook: " << head->hook << ", id: " << head->packet_id << std::endl;*/

    payload_len = nfq_get_payload(_data, &payload);

    /*std::cerr << "  Payload size: " << payload_len << std::endl;*/
    memcpy(this->newPayload, payload, payload_len);

    this->pHandler.handlePacket(payload_len, payload);
    /* TODO: Modify payload */

    /* TODO: Accept dropping packets */
    return nfq_set_verdict(_queue, id, NF_ACCEPT, payload_len, payload);
}