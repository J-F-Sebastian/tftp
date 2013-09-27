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
    short netbuf[MAX_ERROR_STRING_LEN/sizeof(short) + 3];

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

