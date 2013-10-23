#ifndef SOCK_UTILS_H_INCLUDED
#define SOCK_UTILS_H_INCLUDED

#include <ws2tcpip.h>

#include "tftp_logger.h"

#define SOCK_STD_ERR(errcode) \
tftp_log_message("%s failed: %d\n", __FUNCTION__, WSAGetLastError());\
return errcode;

typedef enum socket_errno {
    SOCK_ERR_OK         = 0,
    SOCK_ERR_FAIL       = -1,
    SOCK_ERR_TIMEOUT    = -2,
    SOCK_ERR_INVAL      = -3,
    SOCK_ERR_CLOSED     = -4
} sock_errno_e;

/*
 * Init the socket library.
 */
sock_errno_e sock_init(void);

/*
 * Store into result a pointer to a host-based IP address, to be used with UDP datagrams.
 */
sock_errno_e sock_resolve_addr(struct sockaddr *result, int *result_size);

/*
 * Configures and returns a socket running on UDP/69.
 * Bind is invoked here, and the socket is flagged as passive, i.e. it is to be used
 * to accept incoming connections only.
 */
sock_errno_e sock_server_setup(struct sockaddr *addr, int addr_size, SOCKET *sock);

/*
 * Configures and returns a socket running on UDP.
 * Connect is invoked here, the socket is bidirectional.
 */
sock_errno_e sock_client_setup(struct sockaddr *addr, int addr_size, SOCKET *sock);

/*
 * Create an unconnected, unboud socket on UDP layer
 */
sock_errno_e sock_create(SOCKET *sock);

/*
 * Call connect on the socket
 */
sock_errno_e sock_connect(SOCKET sock, struct sockaddr *addr, int addr_size);

/*
 * Configures timeout.
 */
sock_errno_e sock_set_timeout(SOCKET sock, unsigned msecs);

/*
 * Deinit socket library. Release any memory retained by sock_init.
 */
void sock_done(void);

/*
 * Prints on stdout the error name.
 */
const char *sock_errno_to_string(sock_errno_e e);

/*
 * Converts WSA errors into sock_errno_e values
 */
sock_errno_e WSAError_to_sock_errno(int WSAError);

#endif // SOCK_UTILS_H_INCLUDED
