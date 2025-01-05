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

#ifndef SOCK_UTILS_H_INCLUDED
#define SOCK_UTILS_H_INCLUDED

#include <ws2tcpip.h>

#include "tftp_logger.h"

#define SOCK_STD_ERR(errcode)                                                 \
        tftp_log_message("%s failed: %d\n", __FUNCTION__, WSAGetLastError()); \
        return errcode;

typedef enum socket_errno
{
        SOCK_ERR_OK = 0,
        SOCK_ERR_FAIL = -1,
        SOCK_ERR_TIMEOUT = -2,
        SOCK_ERR_INVAL = -3,
        SOCK_ERR_CLOSED = -4
} sock_errno_e;

/*
 * Init the socket library.
 */
sock_errno_e sock_init(void);

/*
 * Store into result a pointer to a host-based IP address, to be used with UDP datagrams.
 */
sock_errno_e sock_resolve_addr(struct sockaddr *result, int *result_size);

/*
 * Configures and returns a socket running on UDP/69.
 * Bind is invoked here, and the socket is flagged as passive, i.e. it is to be used
 * to accept incoming connections only.
 */
sock_errno_e sock_server_setup(struct sockaddr *addr, int addr_size, SOCKET *sock);

/*
 * Configures and returns a socket running on UDP.
 * Connect is invoked here, the socket is bidirectional.
 */
sock_errno_e sock_client_setup(struct sockaddr *addr, int addr_size, SOCKET *sock);

/*
 * Create an unconnected, unboud socket on UDP layer
 */
sock_errno_e sock_create(SOCKET *sock);

/*
 * Call connect on the socket
 */
sock_errno_e sock_connect(SOCKET sock, struct sockaddr *addr, int addr_size);

/*
 * Configures timeout.
 */
sock_errno_e sock_set_timeout(SOCKET sock, unsigned msecs);

/*
 * Configures send and receive buffers.
 */
sock_errno_e sock_set_buffers(SOCKET sock, unsigned bytes);

/*
 * Deinit socket library. Release any memory retained by sock_init.
 */
void sock_done(void);

/*
 * Prints on stdout the error name.
 */
const char *sock_errno_to_string(sock_errno_e e);

/*
 * Converts WSA errors into sock_errno_e values
 */
sock_errno_e WSAError_to_sock_errno(int WSAError);

#endif // SOCK_UTILS_H_INCLUDED
