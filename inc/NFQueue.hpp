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
        int                  _nfq_fd   = -1;

        int     queue; /*!< Queue ID */

        uint8_t buff[NFQUEUE_BUFF_SZ];       /*!< Buffer used to store packet information */
        uint8_t newPayload[NFQUEUE_BUFF_SZ]; /*!< Buffer used to store manipulated payload */

        static int handle_callback(struct nfq_q_handle *_queue, struct nfgenmsg *_nfmsg, struct nfq_data *_data, void *_class);

    public:
        /**
         * @brief Construct a new NFQueue object
         * 
         * @param _queue Queue ID we wish to connect to
         * @param _pHandler 
         */
        NFQueue(int _queue, PacketHandler &_pHandler);

        int callback(struct nfq_q_handle *_queue, struct nfgenmsg *_nfmsg, struct nfq_data *_data);

        /**
         * @brief Connect to the NF queue
         */
        void open(void);

        /**
         * @brief Start reading from the NF queue and handling packets.
         */
        void run(void);

        /**
         * @brief Release the NF queue
         */
        void close(void);
};

#endif /* _NFQUEUE_HPP_ */
