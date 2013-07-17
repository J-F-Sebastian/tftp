#ifndef TFTP_SERVER_H_INCLUDED
#define TFTP_SERVER_H_INCLUDED

#include "sock_utils.h"

#define TFTP_SRV_DEFAULT_ROOT   "c:\\temp\\"
#define TFTP_SRV_FILENAME_MAX   (128)
#define TFTP_SRV_TIMEOUT_MS     (1000)
#define TFTP_SRV_RETRIES        (3)
#define TFTP_SRV_MAX_CLIENTS    (4)
#define TFTP_SRV_BLKSIZE_MIN    (TFTP_DEFAULT_DATA)
#define TFTP_SRV_BLKSIZE_MAX    (TFTP_DEFAULT_DATA*16)

sock_errno_e tftp_server(SOCKET sock);

#endif // TFTP_SERVER_H_INCLUDED
