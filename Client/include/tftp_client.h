#ifndef TFTP_CLIENT_H_INCLUDED
#define TFTP_CLIENT_H_INCLUDED

#include "tftp_client_state.h"
#include "sock_utils.h"

#define TFTP_CLNT_FILENAME_MAX (128)
#define TFTP_CLNT_TIMEOUT_MS (1000)
#define TFTP_CLNT_RETRIES (5)

sock_errno_e tftp_client(tftp_client_state_t *client, const char *filename);

#endif // TFTP_CLIENT_H_INCLUDED
