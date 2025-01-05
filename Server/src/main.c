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

#include "tftp_protocol.h"
#include "tftp_server.h"
#include "tftp_logger.h"

static void on_server_exit(void)
{
        sock_done();

        tftp_log_message("shutting down\t\tOK\n");
        tftp_log_done();
}

int main()
{
        SOCKET server_socket = INVALID_SOCKET;
        struct sockaddr result;
        int res_size;
        sock_errno_e retcode;

        SetConsoleTitle("TFTP Server");
        atexit(on_server_exit);

        tftp_log_init(TFTP_LOG_USR | TFTP_LOG_FILE | TFTP_LOG_TIMESTAMP);
        tftp_log_message(" TFTPServer version 1.0\n");

        retcode = sock_init();
        if (SOCK_ERR_OK != retcode)
        {
                printf("sock_init failed: %s\n", sock_errno_to_string(retcode));
                return -1;
        }

        tftp_log_message("socket init\t\tOK");

        retcode = sock_resolve_addr(&result, &res_size);
        if (SOCK_ERR_OK != retcode)
        {
                printf("sock_resolve_addr failed: %s\n", sock_errno_to_string(retcode));
                sock_done();
                return -2;
        }

        tftp_log_message("address resolution\tOK");

        retcode = sock_server_setup(&result, res_size, &server_socket);
        if (SOCK_ERR_OK != retcode)
        {
                printf("sock_setup failed: %s\n", sock_errno_to_string(retcode));
                WSACleanup();
                return -3;
        }

        tftp_log_message("setup complete\t\tOK");
        tftp_log_message("TFTPServer is running ...\n");

        retcode = tftp_server(server_socket);

        return 0;
}
