#ifndef _NFQUEUE_HPP_
#define _NFQUEUE_HPP_

#include <libnetfilter_queue/libnetfilter_queue.h>

#include "PacketHandler.hpp"

#define NFQUEUE_BUFF_SZ 4096

class NFQueue {
    private:
        PacketHandler       &pHandler;

        struct nfq_handle   *nfq_hand  = NULL;
        struct nfq_q_handle *nfq_queue = NULL;
        int                  _nfq_fd    = -1;

        int queue;

        uint8_t buff[NFQUEUE_BUFF_SZ];
        uint8_t newPayload[NFQUEUE_BUFF_SZ];

        static int handle_callback(struct nfq_q_handle *_queue, struct nfgenmsg *_nfmsg, struct nfq_data *_data, void *_class);

    public:
        NFQueue(int _queue, PacketHandler &_pHandler);

        int callback(struct nfq_q_handle *_queue, struct nfgenmsg *_nfmsg, struct nfq_data *_data);

        void open(void);

        void run(void);

        void close(void);
};

#endif /* _NFQUEUE_HPP_ */
