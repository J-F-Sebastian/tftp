#define DBG_MODULE_ON

#include <stdio.h>
#include <debug_traces.h>

#include "version.h"
#include "tftp_protocol.h"
#include "tftp_server.h"

int main()
{
    SOCKET server_socket = INVALID_SOCKET;
    struct sockaddr result;
    int res_size;
    sock_errno_e retcode;

    TRACE_FN_ENTRY()

    printf(" *** TFTPServer %s %s\n", TFTPSERVER_STATUS, TFTPSERVER_FULLVERSION_STRING);

    SetConsoleTitle("TFTP Server");

    retcode = sock_init();
    if (SOCK_ERR_OK != retcode)
    {
        printf("sock_init failed: %s\n", sock_errno_to_string(retcode));
        return -1;
    }

    retcode = sock_resolve_addr(&result, &res_size);
    if (SOCK_ERR_OK != retcode)
    {
        printf("sock_resolve_addr failed: %s\n",sock_errno_to_string(retcode));
        sock_done();
        return -2;
    }

    retcode = sock_server_setup(&result, res_size, &server_socket);
    if (SOCK_ERR_OK != retcode)
    {
        printf("sock_setup failed: %s\n", sock_errno_to_string(retcode));
        WSACleanup();
        return -3;
    }

    retcode = tftp_server(server_socket);

    sock_done();

    TRACE_FN_EXIT()

    return 0;
}
