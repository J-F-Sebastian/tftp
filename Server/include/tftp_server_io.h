#ifndef TFTP_SERVER_IO_H_INCLUDED
#define TFTP_SERVER_IO_H_INCLUDED

#include "tftp_protocol.h"
#include "sock_utils.h"

/*
 * Send an error packet as defined by the TFTP protocol.
 * sock must be a bind socket, addr points to the destination
 * address.
 */
sock_errno_e send_error_un(SOCKET sock,
			   struct sockaddr_in *addr,
			   int addr_size,
			   enum TFTP_ERROR error);
/*
 * Send a data packet as defined by the TFTP protocol.
 * sock must be a bound socket, addr points to the destination
 * address.
 * buffer is a pointer to a full TFTP data packet.
 * buffersize is the size in octets of buffer.
 * blocknum is the blockid as defined by the TFTP protocol.
 * NOTE: buffer does not point to data only !!! the function expects
 * the buffer to be like this
 *
 * | 2 octets (type) | 2 octets (blockid) | ..... N octets (data) .....|
 */
sock_errno_e send_data(SOCKET sock,
		       char *buffer,
		       unsigned buffersize,
		       unsigned blocknum);

sock_errno_e send_oack(SOCKET sock,
		       char *buffer,
		       unsigned buffersize);

#endif // TFTP_SERVER_IO_H_INCLUDED
