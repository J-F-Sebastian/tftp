#define DBG_MODULE_ON
#include <w32api.h>

#define WINVER WindowsXP

#include <stdio.h>
#include <debug_traces.h>

#include "sock_utils.h"
#include "tftp_protocol.h"

static const char *errnos[] = {"OK", "FAILURE", "TIMEOUT", "INVALID", "CLOSED"};

sock_errno_e sock_init()
{
    WSADATA wsaData;
    // Initialize Winsock
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);

    if (iResult) {
        printf("WSAStartup failed: %d\n", iResult);
        return SOCK_ERR_FAIL;
    }

    TRACE_BEGIN_MSG("Windows Sockets version %d.%d\n",wsaData.wHighVersion >> 8,
                    wsaData.wHighVersion & 0x00FF)
    TRACE_PRINT_MSG("System description: %s\n",wsaData.szDescription)
    TRACE_END_MSG("System status: %s\n",wsaData.szSystemStatus)

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
    iResult = gethostname(hostname, 256);
    if (iResult) {
        printf("gethostname failed: %d\n", iResult);
        return SOCK_ERR_FAIL;
    }

    // Resolve the server address and port
    iResult = getaddrinfo(hostname, TFTP_PORT_NAME, &hints, &retval);
    if (iResult) {
        printf("getaddrinfo failed: %d\n", iResult);
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
        printf("socket failed: %d\n", WSAGetLastError());
        return SOCK_ERR_FAIL;
    }

    // Bind the socket to the addrinfo provided by the caller
    iResult = bind(listen_socket, addr, addr_size);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed: %d\n", WSAGetLastError());
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
        printf("socket failed: %d\n", WSAGetLastError());
        return SOCK_ERR_FAIL;
    }

    // Connect the socket to the addrinfo provided by the caller
    iResult = connect(traffic_socket, addr, addr_size);
    if (iResult == SOCKET_ERROR) {
        printf("connect failed: %d\n", WSAGetLastError());
        closesocket(traffic_socket);
        return SOCK_ERR_FAIL;
    }

#ifdef DBG_MODULE
    unsigned retval;
    int size = 4;

    iResult = getsockopt(traffic_socket, SOL_SOCKET, SO_MAX_MSG_SIZE, (char *)&retval, &size);

    if (!iResult) {
        printf("Max message size is %u\n", retval);
    }

#endif

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
        printf("socket failed: %d\n", WSAGetLastError());
        return SOCK_ERR_FAIL;
    }

    iResult = bind(traffic_socket, (struct sockaddr *)&src, sizeof(src));
    if (iResult) {
        printf("bind failed: %d\n", WSAGetLastError());
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
        printf("reconnect failed: %d\n", WSAGetLastError());
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
        printf("setsockopt failed: %d\n", WSAGetLastError());
        return SOCK_ERR_FAIL;
    }

    return SOCK_ERR_OK;
}

void sock_done()
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
