#include <stdio.h>

#include "tftp_client.h"
#include "tftp_client_io.h"

sock_errno_e send_rrq(SOCKET sock,
                      const char *filename,
                      const char *mode,
                      unsigned blksize,
                      struct sockaddr *addr,
                      int addrsize)
{
    char buffer[TFTP_DEFAULT_DATA] = {0};
    short *shortbuf = (short *)buffer;
    int len;
    int iResult;

    shortbuf[0] = htons(TFTP_RRQ);
    len = snprintf(buffer + 2, TFTP_CLNT_FILENAME_MAX + 1,"%s", filename);
    len += snprintf(buffer + 3 + len, 8 + 1, "%s",mode);
    len += snprintf(buffer + 4 + len, sizeof(TFTP_OPT_BLKSIZE) + 1, "%s", TFTP_OPT_BLKSIZE);
    len += snprintf(buffer + 5 + len, 5 + 1, "%u", blksize);

    iResult = sendto(sock, buffer, 2 + len + 4, 0, addr, addrsize);

    if (iResult == SOCKET_ERROR) {
        printf("%s failed: %d\n", __FUNCTION__, WSAGetLastError());
        return SOCK_ERR_FAIL;
    }

    return SOCK_ERR_OK;
}
