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
        int len = 2;
        int iResult;
        char temp[6];

        shortbuf[0] = htons(TFTP_RRQ);
        strcpy(buffer + len, filename);
        len += strlen(filename) + 1;
        strcpy(buffer + len, mode);
        len += strlen(mode) + 1;
        strcpy(buffer + len, TFTP_OPT_BLKSIZE);
        len += sizeof(TFTP_OPT_BLKSIZE);
        snprintf(temp, sizeof(temp), "%d", blksize);
        strcpy(buffer + len, temp);
        len += strlen(temp) + 1;

        iResult = sendto(sock, buffer, len, 0, addr, addrsize);

        if (iResult == SOCKET_ERROR)
        {
                printf("%s failed: %d\n", __FUNCTION__, WSAGetLastError());
                return SOCK_ERR_FAIL;
        }

        return SOCK_ERR_OK;
}
