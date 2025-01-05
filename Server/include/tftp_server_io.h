/*
 * TFTP Client and Server
 *
 * Copyright (C) 2013 - 2025 Diego Gallizioli
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
