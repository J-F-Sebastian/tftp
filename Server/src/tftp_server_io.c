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

#include <stdio.h>
#include <string.h>

#include "tftp_common_io.h"
#include "tftp_server_io.h"
#include "tftp_logger.h"

sock_errno_e send_error_un(SOCKET sock,
                           struct sockaddr_in *addr,
                           int addr_size,
                           enum TFTP_ERROR error)
{
        int iResult;
        short netbuf[MAX_ERROR_STRING_LEN / sizeof(short) + 3];

        tftp_log_message("Client %s %s (ERROR %d)",
                         inet_ntoa(addr->sin_addr),
                         TFTP_ERRMSG[error],
                         error);

        netbuf[0] = htons(TFTP_ERROR);
        netbuf[1] = htons(error);

        snprintf((char *)(netbuf + 2), MAX_ERROR_STRING_LEN + 1, "%s", TFTP_ERRMSG[error]);

        iResult = sendto(sock,
                         (char *)netbuf,
                         4 + strlen(TFTP_ERRMSG[error]) + 1,
                         0,
                         (struct sockaddr *)addr,
                         addr_size);

        if (iResult == SOCKET_ERROR)
        {
                SOCK_STD_ERR(SOCK_ERR_FAIL)
        }

        return SOCK_ERR_OK;
}

sock_errno_e send_data(SOCKET sock,
                       char *buffer,
                       unsigned buffersize,
                       unsigned blocknum)
{
        int iResult;
        short *netbuf = (short *)buffer;

        netbuf[0] = htons(TFTP_DATA);
        netbuf[1] = htons(blocknum);

        iResult = send(sock, buffer, buffersize, 0);

        if (iResult == SOCKET_ERROR)
        {
                SOCK_STD_ERR(SOCK_ERR_FAIL)
        }

        return SOCK_ERR_OK;
}

sock_errno_e send_oack(SOCKET sock,
                       char *buffer,
                       unsigned buffersize)
{
        int iResult;
        short *netbuf = (short *)buffer;

        netbuf[0] = htons(TFTP_OACK);

        iResult = send(sock, buffer, buffersize, 0);

        if (iResult == SOCKET_ERROR)
        {
                SOCK_STD_ERR(SOCK_ERR_FAIL)
        }

        return SOCK_ERR_OK;
}
