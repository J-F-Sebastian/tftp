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

#ifndef TFTP_COMMON_IO_H_INCLUDED
#define TFTP_COMMON_IO_H_INCLUDED

#include "tftp_protocol.h"
#include "sock_utils.h"

#define MAX_ERROR_STRING_LEN (76)

static const char *TFTP_ERRMSG[] = {
    "Undefined error.",
    "File not found.",
    "Access violation.",
    "Disk full or allocation exceeded.",
    "Illegal TFTP operation.",
    "Unknown transfer ID.",
    "File already exists.",
    "No such user."};

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
