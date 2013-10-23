#include <stdio.h>

#include "tftp_common_io.h"
#include "tftp_logger.h"

sock_errno_e send_error(SOCKET sock, enum TFTP_ERROR error)
{
    int iResult, msglen;
    short netbuf[MAX_ERROR_STRING_LEN/sizeof(short) + 3];

    netbuf[0] = htons(TFTP_ERROR);
    netbuf[1] = htons(error);

    msglen = snprintf((char *)(netbuf + 2),
                      MAX_ERROR_STRING_LEN + 1,
                      "%s",
                      TFTP_ERRMSG[error]);

    if (msglen < 0) {
        SOCK_STD_ERR(SOCK_ERR_FAIL)
    }

    iResult = send(sock,
                   (char *)netbuf,
                   4 + msglen + 1,
                   0);

    if (iResult == SOCKET_ERROR) {
        SOCK_STD_ERR(SOCK_ERR_FAIL)
    }

    return SOCK_ERR_OK;
}

sock_errno_e send_ack(SOCKET sock, unsigned blocknum)
{
    int iResult;
    short netbuf[2];

    netbuf[0] = htons(TFTP_ACK);
    netbuf[1] = htons(blocknum);

    iResult = send(sock, (char *)netbuf, 4, 0);

    if (iResult == SOCKET_ERROR) {
        SOCK_STD_ERR(SOCK_ERR_FAIL)
    }

    return SOCK_ERR_OK;
}

sock_errno_e receive_packet(SOCKET sock, char *buffer, unsigned *buffersize)
{
    int iResult;
    sock_errno_e retcode;

    iResult = recv(sock, buffer, *buffersize, 0);

    if (iResult == SOCKET_ERROR) {
        iResult = WSAGetLastError();
        retcode = WSAError_to_sock_errno(iResult);
        SOCK_STD_ERR(retcode)
    }
    if (iResult == 0) {
        tftp_log_message("receive_packet ended session\n");
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

    if (error < TFTP_ERR_MAX) {
        tftp_log_message("ERROR %d: %s\n", error, TFTP_ERRMSG[error]);
    } else {
        tftp_log_message("UNKNOWN TFTP ERROR CODE %d\n", error);
    }

    strncpy(description, cursor, sizeof(description) - 1);

    tftp_log_message("Error description: %s\n", description);

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
    sock_errno_e retcode;
    int flags = (peekonly) ? MSG_PEEK : 0;

    iResult = recvfrom(sock, buffer, *buffersize, flags, addr, addrsize);

    if (iResult == SOCKET_ERROR) {
        iResult = WSAGetLastError();
        retcode = WSAError_to_sock_errno(iResult);
        SOCK_STD_ERR(retcode)
    }
    if (iResult == 0) {
        tftp_log_message("%s ended session\n",__FUNCTION__);
        return SOCK_ERR_CLOSED;
    }

    *buffersize = (unsigned)iResult;

    return SOCK_ERR_OK;
}
