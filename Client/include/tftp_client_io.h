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

// sock_errno_e receive_data(char *buffer);

#endif // TFTP_CLIENT_IO_H_INCLUDED
