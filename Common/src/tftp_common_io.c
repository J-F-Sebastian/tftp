#define DBG_MODULE_ON
#include <stdio.h>
#include <debug_traces.h>

#include "tftp_common_io.h"

sock_errno_e send_error(SOCKET sock, enum TFTP_ERROR error)
{
    int iResult;
    short netbuf[MAX_ERROR_STRING_LEN/sizeof(short) + 3];

    TRACE_FN_ENTRY()

    netbuf[0] = htons(TFTP_ERROR);
    netbuf[1] = htons(error);

    snprintf((char *)(netbuf + 2), MAX_ERROR_STRING_LEN + 1, "%s", TFTP_ERRMSG[error]);

    iResult = send(sock,
                   (char *)netbuf,
                   4 + strlen(TFTP_ERRMSG[error]) + 1,
                   0);

    if (iResult == SOCKET_ERROR)
    {
        SOCK_STD_ERR(SOCK_ERR_FAIL)
    }

    TRACE_FN_EXIT()
    return SOCK_ERR_OK;
}

sock_errno_e send_ack(SOCKET sock, unsigned blocknum)
{
    int iResult;
    short netbuf[2];

    TRACE_FN_ENTRY()

    netbuf[0] = htons(TFTP_ACK);
    netbuf[1] = htons(blocknum);

    iResult = send(sock,
                   (char *)netbuf,
                   4,
                   0);

    if (iResult == SOCKET_ERROR)
    {
       SOCK_STD_ERR(SOCK_ERR_FAIL)
    }

    TRACE_FN_EXIT()
    return SOCK_ERR_OK;
}

sock_errno_e receive_packet(SOCKET sock, char *buffer, unsigned *buffersize)
{
    int iResult;

    iResult = recv(sock, buffer, *buffersize, 0);

    if (iResult == SOCKET_ERROR)
    {
        iResult = WSAGetLastError();
        if (WSAETIMEDOUT == iResult)
        {
            return SOCK_ERR_TIMEOUT;
        }
        else if (WSAECONNRESET == iResult)
        {
            return SOCK_ERR_CLOSED;
        }
        else
        {
            SOCK_STD_ERR(SOCK_ERR_FAIL)
        }
    }
    if (iResult == 0)
    {
        printf("receive_packet ended session\n");
        return SOCK_ERR_CLOSED;
    }

    *buffersize = (unsigned)iResult;
    return SOCK_ERR_OK;
}

sock_errno_e receive_error(char *buffer)
{
    short *tmp = (short *)buffer;
    enum TFTP_ERROR error = ntohs(tmp[1]);
    char *cursor = buffer + 4;
    char description[MAX_ERROR_STRING_LEN + 1] = {0};

    if (error < TFTP_ERR_MAX)
    {
        printf("ERROR %d: %s\n", error, TFTP_ERRMSG[error]);
    }
    else
    {
        printf("UNKNOWN TFTP ERROR CODE %d\n", error);
    }

    strncpy(description, cursor, sizeof(description) - 1);

    printf("Error description: %s\n", description);

    return SOCK_ERR_OK;
}

sock_errno_e receive_ack(char *buffer, unsigned blockid)
{
    short *tmp = (short *)buffer;
    unsigned ackblockid = ntohs(tmp[1]);

    return (ackblockid == blockid) ? SOCK_ERR_OK : SOCK_ERR_FAIL;
}

sock_errno_e receive_packet_un(SOCKET sock,
                               char *buffer,
                               unsigned *buffersize,
                               struct sockaddr *addr,
                               int *addrsize,
                               BOOL peekonly)
{
    int iResult;

    if (peekonly) {
        iResult = recvfrom(sock, buffer, *buffersize, MSG_PEEK, addr, addrsize);
    } else {
        iResult = recvfrom(sock, buffer, *buffersize, 0, addr, addrsize);
    }

    if (iResult == SOCKET_ERROR)
    {
        iResult = WSAGetLastError();
        if (WSAETIMEDOUT == iResult)
        {
            return SOCK_ERR_TIMEOUT;
        }
        else if (WSAECONNRESET == iResult)
        {
            return SOCK_ERR_CLOSED;
        }
        else
        {
            SOCK_STD_ERR(SOCK_ERR_FAIL)
        }
    }
    if (iResult == 0)
    {
        printf("%s ended session\n",__FUNCTION__);
        return SOCK_ERR_CLOSED;
    }

    *buffersize = (unsigned)iResult;

    return SOCK_ERR_OK;
}
