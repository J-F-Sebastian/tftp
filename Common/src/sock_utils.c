#define DBG_MODULE_ON

#include <stdio.h>
#include <debug_traces.h>

#include "sock_utils.h"
#include "tftp_protocol.h"

static const char *errnos[] = {"OK", "FAILURE", "TIMEOUT", "INVALID", "CLOSED"};

static void sock_dump_addr(struct addrinfo *addr)
{
    struct addrinfo *ptr = addr;
    struct sockaddr_in  *sockaddr_ipv4;
    LPSOCKADDR sockaddr_ip;
    char ipstringbuffer[46];
    DWORD ipbufferlength = 46;
    int iResult;

    while (ptr)
    {
        printf("\tFlags: 0x%x\n", ptr->ai_flags);
        printf("\tFamily: ");
        switch (ptr->ai_family)
        {
        case AF_UNSPEC:
            printf("Unspecified\n");
            break;
        case AF_INET:
            printf("IPv4\n");
            sockaddr_ipv4 = (struct sockaddr_in *) ptr->ai_addr;
            printf("\tAddress %s\n",
                   inet_ntoa(sockaddr_ipv4->sin_addr) );
            break;
        case AF_INET6:
            printf("IPv6\n");
            // the InetNtop function is available on Windows Vista and later
            // sockaddr_ipv6 = (struct sockaddr_in6 *) ptr->ai_addr;
            // printf("\tIPv6 address %s\n",
            //    InetNtop(AF_INET6, &sockaddr_ipv6->sin6_addr, ipstringbuffer, 46) );

            // We use WSAAddressToString since it is supported on Windows XP and later
            sockaddr_ip = (LPSOCKADDR) ptr->ai_addr;
            // The buffer length is changed by each call to WSAAddresstoString
            // So we need to set it for each iteration through the loop for safety
            ipbufferlength = 46;
            iResult = WSAAddressToString(sockaddr_ip, (DWORD) ptr->ai_addrlen, NULL,
                                         ipstringbuffer, &ipbufferlength );
            if (iResult)
                printf("\tAddress not available.\n");
            else
                printf("\tAddress %s\n", ipstringbuffer);
            break;
        case AF_NETBIOS:
            printf("NetBIOS\n");
            break;
        default:
            printf("Other %d\n", ptr->ai_family);
            break;
        }
        printf("\tSocket type: ");
        switch (ptr->ai_socktype)
        {
        case 0:
            printf("Unspecified\n");
            break;
        case SOCK_STREAM:
            printf("SOCK_STREAM (stream)\n");
            break;
        case SOCK_DGRAM:
            printf("SOCK_DGRAM (datagram) \n");
            break;
        case SOCK_RAW:
            printf("SOCK_RAW (raw) \n");
            break;
        case SOCK_RDM:
            printf("SOCK_RDM (reliable message datagram)\n");
            break;
        case SOCK_SEQPACKET:
            printf("SOCK_SEQPACKET (pseudo-stream packet)\n");
            break;
        default:
            printf("Other %d\n", ptr->ai_socktype);
            break;
        }
        printf("\tProtocol: ");
        switch (ptr->ai_protocol)
        {
        case 0:
            printf("Unspecified\n");
            break;
        case IPPROTO_TCP:
            printf("TCP\n");
            break;
        case IPPROTO_UDP:
            printf("UDP\n");
            break;
        default:
            printf("Other %d\n", ptr->ai_protocol);
            break;
        }
        printf("\tLength of this sockaddr: %d\n", ptr->ai_addrlen);
        printf("\tCanonical name: %s\n", ptr->ai_canonname);
        ptr = ptr->ai_next;
    }
}

sock_errno_e sock_init()
{
    WSADATA wsaData;
    // Initialize Winsock
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);

    if (iResult)
    {
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
    if (iResult)
    {
        printf("gethostname failed: %d\n", iResult);
        return SOCK_ERR_FAIL;
    }

    // Resolve the server address and port
    iResult = getaddrinfo(hostname, TFTP_PORT_NAME, &hints, &retval);
    if (iResult)
    {
        printf("getaddrinfo failed: %d\n", iResult);
        return SOCK_ERR_FAIL;
    }

    sock_dump_addr(retval);

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
    if (listen_socket == INVALID_SOCKET)
    {
        printf("socket failed: %d\n", WSAGetLastError());
        return SOCK_ERR_FAIL;
    }

    // Bind the socket to the addrinfo provided by the caller
    iResult = bind(listen_socket, addr, addr_size);
    if (iResult == SOCKET_ERROR)
    {
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
    if (traffic_socket == INVALID_SOCKET)
    {
        printf("socket failed: %d\n", WSAGetLastError());
        return SOCK_ERR_FAIL;
    }

    // Connect the socket to the addrinfo provided by the caller
    iResult = connect(traffic_socket, addr, addr_size);
    if (iResult == SOCKET_ERROR)
    {
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
    if (traffic_socket == INVALID_SOCKET)
    {
        printf("socket failed: %d\n", WSAGetLastError());
        return SOCK_ERR_FAIL;
    }

    iResult = bind(traffic_socket, (struct sockaddr *)&src, sizeof(src));
    if (iResult)
    {
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
    if (iResult == SOCKET_ERROR)
    {
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
    if (iResult == SOCKET_ERROR)
    {
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
    switch (e)
    {
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
