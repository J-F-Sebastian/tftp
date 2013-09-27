#include <stdio.h>
#include <types_common.h>

#include "tftp_protocol.h"
#include "tftp_server.h"
#include "tftp_common_io.h"
#include "tftp_server_io.h"
#include "tftp_server_client.h"
#include "tftp_logger.h"

static HANDLE client_threads[TFTP_SRV_MAX_CLIENTS] = {NULL};

static inline HANDLE *get_client_thread()
{
    unsigned i;

    for (i = 0; i < NELEMENTS(client_threads); i++)
    {
        /* Position is allocated, see if it is signaled, if it is the case clear and return */
        if (NULL != client_threads[i])
        {
            if (WAIT_OBJECT_0 == WaitForSingleObject(client_threads[i], 0))
            {
                if (!CloseHandle(client_threads[i]))
                {
                    tftp_log_message("CloseHandle failed: %lu\n", GetLastError());
                }
                else
                {
                    client_threads[i] = NULL;
                    return &client_threads[i];
                }
            }
        }
        else
        {
            return &client_threads[i];
        }
    }

    return NULL;
}

static int request_supported(char *netbuf, BOOL read_request, tftp_client_state_t *cli)
{
    unsigned length = 0;
    char *cursor = netbuf + 2;
    /* filename accepts relative path */
    char filename[TFTP_SRV_FILENAME_MAX + 1] = {0};
    /* filename_start is the true file name */
    char *filename_start = NULL;
    unsigned blocksize;

    strncpy(filename, TFTP_SRV_DEFAULT_ROOT,sizeof(TFTP_SRV_DEFAULT_ROOT) - 1);
    length = sizeof(TFTP_SRV_DEFAULT_ROOT) - 1;

    /*
     * Since no standard filename length is given in the protocol,
     * we cannot assume that the filename is null terminated, nor
     * it can fit some predefined buffer; hence the copy & check
     * is performed here and the request is rejected if the filename
     * is too long (for us!).
     */
    while (length < TFTP_SRV_FILENAME_MAX)
    {
        if (*cursor)
        {
            filename[length] = *cursor++;
            ++length;
        }
        else
        {
            break;
        }
    }

    if (TFTP_SRV_FILENAME_MAX == length)
    {
        return -1;
    }

    filename_start = strrchr(filename, '\\');
    if (!filename_start) {
        filename_start = strrchr(filename, '/');
    }
    /* If no slashes are found, then use the filename buffer */
    if (!filename_start) {
        filename_start = filename;
    } else {
        filename_start++;
    }

    memset(cli->filename, 0, sizeof(cli->filename));
    strncpy(cli->filename, filename_start, sizeof(cli->filename) - 1);

    cursor++;

    /* netascii and mail are not supported */

    if (!strncasecmp(cursor, TFTP_MODE_ASCII,sizeof(TFTP_MODE_ASCII)) ||
            !strncasecmp(cursor, TFTP_MODE_MAIL,sizeof(TFTP_MODE_MAIL)))
    {
        return -2;
    }

    if (read_request)
    {
        cli->file = fopen(filename,"rb+");
    }
    else
    {
        cli->file = fopen(filename,"wb+");
    }

    if (!cli->file)
    {
        return -3;
    }

    /*
     * Advance cursor to the options, if any
     */
     while (*cursor++) {}

     if (*cursor) {
        if (!strncasecmp(cursor, TFTP_OPT_BLKSIZE, sizeof(TFTP_OPT_BLKSIZE))) {
            cursor += sizeof(TFTP_OPT_BLKSIZE);
            blocksize = atoi(cursor);
            if ((blocksize < TFTP_MIN_DATA) ||
                (blocksize > TFTP_MAX_DATA)) {
                    blocksize = TFTP_DEFAULT_DATA;
            }
                cli->opt_blocksize = blocksize;
                cli->state_flags |= TFTP_SRV_BLKSIZE;
        }
     }

    /*
     * Allocation use the max supported size, so the client thread does not need
     * to mess with allocs.
     */
    cli->block_buffer = malloc(cli->opt_blocksize + TFTP_HDR_SIZE);
    cli->block_buffer_size = cli->opt_blocksize + TFTP_HDR_SIZE;

    return 0;
}

static void
handle_read_request(SOCKET server,
                    struct sockaddr_in *client,
                    int client_size,
                    char *netbuf)
{
    int retval;
    tftp_client_state_t *temp_state;
    HANDLE *newthread;

    newthread = get_client_thread();
    if (!newthread)
    {
        send_error_un(server, client, client_size, TFTP_ERR_UNDEF);
        return;
    }

    temp_state = malloc(sizeof(*temp_state));
    if (!temp_state)
    {
        send_error_un(server, client, client_size, TFTP_ERR_UNDEF);
        return;
    }

    init_client_state(temp_state);
    temp_state->state_flags = TFTP_SRV_SENDING;
    temp_state->destination = *client;

    retval = request_supported(netbuf, TRUE, temp_state);

    switch (retval)
    {
    case 0 :
        if (SOCK_ERR_OK == sock_client_setup((struct sockaddr *)client,
                                             client_size,
                                             &temp_state->client) &&
                SOCK_ERR_OK == sock_set_timeout(temp_state->client, TFTP_SRV_TIMEOUT_MS))
        {
            *newthread = CreateThread(NULL, 4096, start_client, temp_state, 0, NULL);
            if (!*newthread)
            {
                tftp_log_message("Createthread failed: %lu\n", GetLastError());
                reset_client_state(temp_state);
                free(temp_state);
            }
            /* Let the client thread run, and enforce a delay between requests acceptance */
            Sleep(TFTP_SRV_TIMEOUT_MS);

        }
        break;

    case -1:
        /* Fallthru */
    case -3:
        send_error_un(server, client, client_size, TFTP_ERR_NOTFOUND);
        reset_client_state(temp_state);
        free(temp_state);
        break;

    case -2:
        send_error_un(server, client, client_size, TFTP_ERR_UNDEF);
        reset_client_state(temp_state);
        free(temp_state);
        break;
    }
}

static void
handle_write_request(SOCKET server,
                     struct sockaddr_in *client,
                     int client_size,
                     char *netbuf)
{
    send_error_un(server, client, client_size, TFTP_ERR_ILLEGAL);
}

/***************************************************************************/
/**************************** PUBLIC SECTION *******************************/
/***************************************************************************/

sock_errno_e tftp_server(SOCKET sock)
{
    short pak_type;
    struct sockaddr_in client;
    int client_size = sizeof(client);
    /*
     * All RFCs state that the RRQ and WRQ messages cannot be greater than
     * 512 bytes.
     */
    char netbuf[TFTP_DEFAULT_DATA + TFTP_HDR_SIZE]= {0};
    short *netbuf_shorts = (short *)netbuf;
    unsigned netbuf_size;
    sock_errno_e retcode;

    while (TRUE)
    {
        netbuf_size = sizeof(netbuf);

        retcode = receive_packet_un(sock,
                                    netbuf,
                                    &netbuf_size,
                                    (struct sockaddr *)&client,
                                    &client_size,
                                    FALSE);

        if (SOCK_ERR_OK != retcode) {
            return (retcode);
        }

        pak_type = ntohs(netbuf_shorts[0]);

        switch (pak_type)
        {
        case TFTP_RRQ:
            handle_read_request(sock, &client, client_size, netbuf);
            break;

        case TFTP_WRQ:
            handle_write_request(sock, &client, client_size, netbuf);
            break;

        case TFTP_DATA:
            /* FALLTHRU */
        case TFTP_ACK:
            /* FALLTHRU */
        case TFTP_ERROR:
            tftp_log_message("Unhandled packet type %d in %s\n", pak_type, __FUNCTION__);
            send_error_un(sock, &client, client_size, TFTP_ERR_UNKNOWNID);
            break;

        default:
            tftp_log_message("Unknown packet type %d in %s\n", pak_type, __FUNCTION__);
            send_error_un(sock, &client, client_size, TFTP_ERR_UNDEF);
            break;
        }

        /* Make the buffer clean */
        memset(netbuf, 0, sizeof(netbuf));
    }
}
