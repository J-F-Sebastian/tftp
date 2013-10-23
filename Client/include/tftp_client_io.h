#ifndef TFTP_CLIENT_IO_H_INCLUDED
#define TFTP_CLIENT_IO_H_INCLUDED

#include "tftp_protocol.h"
#include "sock_utils.h"

/*
 * Send a Read ReQuest packet (RRQ) as defined by the TFTP protocol.
 * sock must be a connected socket.
 */
sock_errno_e send_rrq(SOCKET sock,
                      const char *filename,
                      const char *mode,
                      unsigned blksize,
                      struct sockaddr *addr,
                      int addrsize);

//sock_errno_e receive_data(char *buffer);

#endif // TFTP_CLIENT_IO_H_INCLUDED
