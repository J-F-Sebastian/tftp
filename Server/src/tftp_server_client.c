#include <windows.h>

#include "tftp_server.h"
#include "tftp_common_io.h"
#include "tftp_server_io.h"
#include "tftp_server_client.h"
#include "tftp_protocol.h"

static BOOL
handle_timeout(tftp_client_state_t *state)
{
    state->state_flags |= TFTP_SRV_TIMEOUT;
    state->timeout_count += 1;
    /* We tried hard, but it is time to reset the session */
    if (state->timeout_count > TFTP_SRV_RETRIES)
    {
        return FALSE;
    }

    send_data(state->client, state->block_buffer, state->block_buffer_size, state->blockid);

    return TRUE;
}

static void
handle_oack_transmit(tftp_client_state_t *state)
{
    int len;

    len = snprintf(state->block_buffer + 2, sizeof(TFTP_OPT_BLKSIZE) + 1, "%s", TFTP_OPT_BLKSIZE);
    len += snprintf(state->block_buffer + len + 3, 5 + 1, "%u", state->opt_blocksize);

    state->block_buffer_size = 2 + len + 2;

    send_oack(state->client, state->block_buffer, state->block_buffer_size);
}

static void
handle_data_transmit(tftp_client_state_t *state)
{
    state->blockid++;
    /* FIXME error handling! */
    state->block_buffer_size = fread(state->block_buffer + TFTP_HDR_SIZE,
                                     1,
                                     state->opt_blocksize,
                                     state->file);

    if (state->block_buffer_size < state->opt_blocksize)
    {
        state->state_flags |= TFTP_SRV_LASTACK;
    }

    state->octets += state->block_buffer_size;
    state->block_buffer_size += TFTP_HDR_SIZE;

    /*
     * If the previous data packet timeout and an ack was received in time, clear the timeout
     * condition and reset counter.
     */
    if (state->state_flags & TFTP_SRV_TIMEOUT)
    {
        state->state_flags &= ~TFTP_SRV_TIMEOUT;
        state->timeout_count = 0;
    }

    send_data(state->client, state->block_buffer, state->block_buffer_size, state->blockid);
}

static void handle_rrq(tftp_client_state_t *cli)
{
    short pak_type;
    short *netbuf_shorts = (short *)cli->block_buffer;
    sock_errno_e retcode;

    if (cli->state_flags & TFTP_SRV_BLKSIZE) {
        handle_oack_transmit(cli);
    } else {
        handle_data_transmit(cli);
    }

    while (1)
    {
        retcode = receive_packet(cli->client, cli->block_buffer, &cli->block_buffer_size);

        switch (retcode)
        {
        case SOCK_ERR_OK:
            break;
        case SOCK_ERR_TIMEOUT:
            if (handle_timeout(cli))
            {
                continue;
            }
            else
            {
                return;
            }
            break;

        default:
            return;
        }

        pak_type = ntohs(netbuf_shorts[0]);

        switch (pak_type)
        {
        case TFTP_ACK:
            if (SOCK_ERR_OK == receive_ack(cli->block_buffer, cli->blockid))
            {
                if (cli->state_flags & TFTP_SRV_LASTACK)
                {
                    return;
                }
                else
                {
                    handle_data_transmit(cli);
                }
            }
            break;

        case TFTP_ERROR:
            receive_error(cli->block_buffer);
            return;

        case TFTP_RRQ:
            /* FALLTHRU */
        case TFTP_WRQ:
            /* FALLTHRU */
        case TFTP_DATA:
            /* FALLTHRU */
        case TFTP_OACK:
            tftp_log_message("Unhandled packet type %d in %s\n", pak_type, __FUNCTION__);
            break;

        default:
            tftp_log_message("Unknown packet type %d in %s\n", pak_type, __FUNCTION__);
            break;
        }
    }
}

/*
 * PUBLIC SECTION
 */
DWORD WINAPI start_client(LPVOID lpParam)
{
    tftp_client_state_t *state = (tftp_client_state_t *)lpParam;

    if (state->state_flags & TFTP_SRV_SENDING)
    {
        handle_rrq(state);
    }

    reset_client_state(state);
    free(state);

    ExitThread(0);
    return 0;
}
