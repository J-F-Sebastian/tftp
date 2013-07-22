#include <stdio.h>

#include "tftp_protocol.h"
#include "tftp_server.h"
#include "tftp_logger.h"

int main()
{
    SOCKET server_socket = INVALID_SOCKET;
    struct sockaddr result;
    int res_size;
    sock_errno_e retcode;

    tftp_log_message(" TFTPServer version 1.0\n");

    SetConsoleTitle("TFTP Server");

    retcode = sock_init();
    if (SOCK_ERR_OK != retcode)
    {
        printf("sock_init failed: %s\n", sock_errno_to_string(retcode));
        return -1;
    }

    tftp_log_message(" socket init\t\tOK");

    retcode = sock_resolve_addr(&result, &res_size);
    if (SOCK_ERR_OK != retcode)
    {
        printf("sock_resolve_addr failed: %s\n",sock_errno_to_string(retcode));
        sock_done();
        return -2;
    }

    tftp_log_message(" address resolution\tOK");

    retcode = sock_server_setup(&result, res_size, &server_socket);
    if (SOCK_ERR_OK != retcode)
    {
        printf("sock_setup failed: %s\n", sock_errno_to_string(retcode));
        WSACleanup();
        return -3;
    }

    tftp_log_message(" setup complete\t\tOK");
    tftp_log_message("\n TFTPServer is running ...\n");

    retcode = tftp_server(server_socket);

    sock_done();

    tftp_log_message(" shutting down\t\tOK\n");

    return 0;
}
