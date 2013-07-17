#include <stdio.h>

#include "tftp_protocol.h"
#include "tftp_server.h"

int main()
{
    SOCKET server_socket = INVALID_SOCKET;
    struct sockaddr result;
    int res_size;
    sock_errno_e retcode;

    printf(" TFTPServer version 1.0\n");

    SetConsoleTitle("TFTP Server");

    retcode = sock_init();
    if (SOCK_ERR_OK != retcode)
    {
        printf("sock_init failed: %s\n", sock_errno_to_string(retcode));
        return -1;
    }

    printf(" socket init\t\tOK\n");

    retcode = sock_resolve_addr(&result, &res_size);
    if (SOCK_ERR_OK != retcode)
    {
        printf("sock_resolve_addr failed: %s\n",sock_errno_to_string(retcode));
        sock_done();
        return -2;
    }

    printf(" address resolution\tOK\n");

    retcode = sock_server_setup(&result, res_size, &server_socket);
    if (SOCK_ERR_OK != retcode)
    {
        printf("sock_setup failed: %s\n", sock_errno_to_string(retcode));
        WSACleanup();
        return -3;
    }

    printf(" setup complete\t\tOK\n");
    printf("\n TFTPServer is running ...\n");

    retcode = tftp_server(server_socket);

    sock_done();

    printf(" shutting down\t\tOK\n");

    return 0;
}
