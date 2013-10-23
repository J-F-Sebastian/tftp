#ifndef TFTP_CLIENT_STATE_H_INCLUDED
#define TFTP_CLIENT_STATE_H_INCLUDED

#include <winsock2.h>
#include <stdio.h>
#include <types_common.h>

enum TFTP_SRV_STATE {
    TFTP_SRV_WAITING = (1 << 0),
    TFTP_SRV_SENDING = (1 << 1),
    TFTP_SRV_RCVING  = (1 << 2),
    TFTP_SRV_TIMEOUT = (1 << 3),
    TFTP_SRV_LASTACK = (1 << 4),
    TFTP_SRV_BLKSIZE = (1 << 5)
};

typedef struct tftp_client_state {
    /* Support for RFC 1350 functionalities */
    SOCKET client;
    uint16_t blockid;
    uint16_t state_flags;
    FILE *file;
    char *block_buffer;
    unsigned block_buffer_size;
    unsigned timeout_count;
    struct sockaddr_in destination;
    uint32_t octets;
    /* Support for RFC 2347, 2348 functionalities*/
    uint16_t opt_blocksize;
} tftp_client_state_t;

void init_client_state(tftp_client_state_t *cli);

void reset_client_state(tftp_client_state_t *cli);

#endif // TFTP_CLIENT_STATE_H_INCLUDED
