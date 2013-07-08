#ifndef TFTP_COMMON_IO_H_INCLUDED
#define TFTP_COMMON_IO_H_INCLUDED

#include "tftp_protocol.h"
#include "sock_utils.h"

#define MAX_ERROR_STRING_LEN (76)

static const char *TFTP_ERRMSG[]=
{
    "Undefined error.",
    "File not found.",
    "Access violation.",
    "Disk full or allocation exceeded.",
    "Illegal TFTP operation.",
    "Unknown transfer ID.",
    "File already exists.",
    "No such user."
};

/*
 * Send an error packet as defined by the TFTP protocol.
 * sock must be a connected socket.
 */
sock_errno_e send_error(SOCKET sock,
                        enum TFTP_ERROR error);

/*
 * Send an acknowledge packet as defined by the TFTP protocol.
 * sock must be a bound socket, addr points to the destination
 * address.
 * blocknum is the blockid as defined by the TFTP protocol.
 */
sock_errno_e send_ack(SOCKET sock,
                      unsigned blocknum);

sock_errno_e receive_packet(SOCKET sock, char *buffer, unsigned *buffersize);

sock_errno_e receive_error(char *buffer);

sock_errno_e receive_ack(char *buffer, unsigned blockid);

sock_errno_e receive_packet_un(SOCKET sock,
                               char *buffer,
                               unsigned *buffersize,
                               struct sockaddr *addr,
                               int *addrsize,
                               BOOL peekonly);

#endif // TFTP_COMMON_IO_H_INCLUDED
