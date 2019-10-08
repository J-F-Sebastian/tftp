#include <w32api.h>
#define WINVER WindowsXP
#include <stdio.h>

#include "sock_utils.h"
#include "tftp_protocol.h"
#include "tftp_logger.h"

static const char *errnos[] = {"OK", "FAILURE", "TIMEOUT", "INVALID", "CLOSED"};

sock_errno_e sock_init(void)
{
    WSADATA wsaData;
    // Initialize Winsock
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);

    if (iResult) {
        tftp_log_message("WSAStartup failed: %d\n", iResult);
        return SOCK_ERR_FAIL;
    }

    return SOCK_ERR_OK;
}

sock_errno_e sock_resolve_addr(struct sockaddr *result, int *result_size)
{
    int iResult;
    struct addrinfo hints;
    struct addrinfo *retval;
    char hostname[256];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the host name
    iResult = gethostname(hostname, sizeof(hostname));
    if (iResult) {
        tftp_log_message("gethostname failed: %d\n", iResult);
        return SOCK_ERR_FAIL;
    }

    // Resolve the server address and port
    iResult = getaddrinfo(hostname, TFTP_PORT_NAME, &hints, &retval);
    if (iResult) {
        tftp_log_message("getaddrinfo failed: %d\n", iResult);
        return SOCK_ERR_FAIL;
    }

    *result = *(retval->ai_addr);
    *result_size = retval->ai_addrlen;

    freeaddrinfo(retval);

    return SOCK_ERR_OK;
}

sock_errno_e sock_server_setup(struct sockaddr *addr, int addr_size, SOCKET *sock)
{
    SOCKET listen_socket;
    int iResult;

    // Create a SOCKET to establish a server connection
    listen_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (listen_socket == INVALID_SOCKET) {
        tftp_log_message("socket failed: %d\n", WSAGetLastError());
        return SOCK_ERR_FAIL;
    }

    // Bind the socket to the addrinfo provided by the caller
    iResult = bind(listen_socket, addr, addr_size);
    if (iResult == SOCKET_ERROR) {
        tftp_log_message("bind failed: %d\n", WSAGetLastError());
        closesocket(listen_socket);
        return SOCK_ERR_FAIL;
    }

    *sock = listen_socket;

    return SOCK_ERR_OK;
}

sock_errno_e sock_client_setup(struct sockaddr *addr, int addr_size, SOCKET *sock)
{
    SOCKET traffic_socket;
    int iResult;

    // Create a SOCKET to establish a server connection
    traffic_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (traffic_socket == INVALID_SOCKET) {
        tftp_log_message("socket failed: %d\n", WSAGetLastError());
        return SOCK_ERR_FAIL;
    }

    // Connect the socket to the addrinfo provided by the caller
    iResult = connect(traffic_socket, addr, addr_size);
    if (iResult == SOCKET_ERROR) {
        tftp_log_message("connect failed: %d\n", WSAGetLastError());
        closesocket(traffic_socket);
        return SOCK_ERR_FAIL;
    }

    *sock = traffic_socket;

    return SOCK_ERR_OK;
}

sock_errno_e sock_create(SOCKET *sock)
{
    SOCKET traffic_socket;
    int iResult;
    struct sockaddr_in src;

    src.sin_family = AF_INET;
    src.sin_port = 0;
    src.sin_addr.s_addr = INADDR_ANY;

    // Create a SOCKET to establish a server connection
    traffic_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (traffic_socket == INVALID_SOCKET) {
        tftp_log_message("socket failed: %d\n", WSAGetLastError());
        return SOCK_ERR_FAIL;
    }

    iResult = bind(traffic_socket, (struct sockaddr *)&src, sizeof(src));
    if (iResult) {
        tftp_log_message("bind failed: %d\n", WSAGetLastError());
        return SOCK_ERR_FAIL;
    }

    *sock = traffic_socket;

    return SOCK_ERR_OK;
}

sock_errno_e sock_connect(SOCKET sock, struct sockaddr *addr, int addr_size)
{
    int iResult;

    /*
     * Connect the socket to the addrinfo provided by the caller, note that socket
     * is not going to be closed.
     * Connect will simply re bind the socket.
     */
    iResult = connect(sock, addr, addr_size);
    if (iResult == SOCKET_ERROR) {
        tftp_log_message("reconnect failed: %d\n", WSAGetLastError());
        return SOCK_ERR_FAIL;
    }

    return SOCK_ERR_OK;
}

sock_errno_e sock_set_timeout(SOCKET sock, unsigned msecs)
{
    int iResult;

    // Since the code is dedicated to TFTP, it is assumed that the connection is
    // using UDP so we configure read (write?) timeouts
    iResult = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&msecs, sizeof(msecs));
    if (iResult == SOCKET_ERROR) {
        tftp_log_message("setsockopt SO_RCVTIMEO failed: %d\n", WSAGetLastError());
        return SOCK_ERR_FAIL;
    }

    return SOCK_ERR_OK;
}

sock_errno_e sock_set_buffers(SOCKET sock, unsigned bytes)
{
    int iResult;

    // Since the code is dedicated to TFTP, it is assumed that the connection is
    // using UDP so we configure read and write buffers
    iResult = setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char *)&bytes, sizeof(bytes));
    if (iResult == SOCKET_ERROR) {
        tftp_log_message("setsockopt SO_RCVBUF failed: %d\n", WSAGetLastError());
        return SOCK_ERR_FAIL;
    }
    iResult = setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char *)&bytes, sizeof(bytes));
    if (iResult == SOCKET_ERROR) {
        tftp_log_message("setsockopt SO_SNDBUF failed: %d\n", WSAGetLastError());
        return SOCK_ERR_FAIL;
    }
    return SOCK_ERR_OK;
}

void sock_done(void)
{
    WSACleanup();
}

const char *sock_errno_to_string(sock_errno_e e)
{
    switch (e) {
    case SOCK_ERR_OK:
        return errnos[0];
    case SOCK_ERR_FAIL:
        return errnos[1];
    case SOCK_ERR_TIMEOUT:
        return errnos[2];
    case SOCK_ERR_INVAL:
        return errnos[3];
    case SOCK_ERR_CLOSED:
        return errnos[4];
    default:
        return "N/A";
    }
}

sock_errno_e WSAError_to_sock_errno(int WSAError)
{
    switch (WSAError) {
    case WSAETIMEDOUT:
        return SOCK_ERR_TIMEOUT;
    case WSAECONNRESET:
        return SOCK_ERR_CLOSED;
    default:
        return SOCK_ERR_FAIL;
    }
}
