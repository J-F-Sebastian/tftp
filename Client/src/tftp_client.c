#include <stdio.h>

#include "tftp_client.h"
#include "tftp_client_state.h"
#include "tftp_common_io.h"
#include "tftp_client_io.h"

static BOOL
handle_rrq_timeout(tftp_client_state_t *state, const char *filename)
{
    state->state_flags |= TFTP_SRV_TIMEOUT;
    state->timeout_count += 1;
    /* We tried hard, but it is time to reset the session */
    if (state->timeout_count > TFTP_CLNT_RETRIES) {
        return FALSE;
    }

    if (SOCK_ERR_OK != send_rrq(state->client,
                                filename,
                                TFTP_MODE_BIN,
                                state->opt_blocksize,
                                (struct sockaddr *)&state->destination,
                                sizeof(state->destination))) {
        printf("Cannot connect to server.\n");
        return FALSE;
    }

    printf("Timeout %d blockid %d\n", state->timeout_count, state->blockid);

    return TRUE;
}

static BOOL
handle_timeout(tftp_client_state_t *state)
{
    state->state_flags |= TFTP_SRV_TIMEOUT;
    state->timeout_count += 1;
    /* We tried hard, but it is time to reset the session */
    if (state->timeout_count > TFTP_CLNT_RETRIES) {
        return FALSE;
    }

    send_ack(state->client, state->blockid);
    printf("Timeout %d blockid %d\n", state->timeout_count, state->blockid);

    return TRUE;
}

static BOOL
handle_data_receive(tftp_client_state_t *state)
{
    size_t retval;

    if (state->block_buffer_size < TFTP_HDR_SIZE) {
        return FALSE;
    }

    state->block_buffer_size -= TFTP_HDR_SIZE;
    /* FIXME error handling! */
    if (state->block_buffer_size) {
        retval = fwrite(state->block_buffer + TFTP_HDR_SIZE,
                        1,
                        state->block_buffer_size,
                        state->file);
        if (retval < state->block_buffer_size) {
            return FALSE;
        }
    }

    if (state->block_buffer_size < state->opt_blocksize) {
        state->state_flags |= TFTP_SRV_LASTACK;
    }

    /*
     * If the previous data packet timeout and an ack was received in time, clear the timeout
     * condition and reset counter.
     */
    if (state->state_flags & TFTP_SRV_TIMEOUT) {
        state->state_flags &= ~TFTP_SRV_TIMEOUT;
        state->timeout_count = 0;
    }

    //printf("Block ID %u\n", state->blockid);
    send_ack(state->client, state->blockid);
    state->blockid++;
    state->octets += state->block_buffer_size;
    state->block_buffer_size = state->opt_blocksize + TFTP_HDR_SIZE;

    return TRUE;
}

static BOOL
handle_oack_receive(tftp_client_state_t *state)
{
    if (state->block_buffer_size < 2) {
        return FALSE;
    }

    state->block_buffer_size -= 2;
    /* FIXME error handling! */
    if (state->block_buffer_size) {
        if (!strncasecmp(state->block_buffer + 2, TFTP_OPT_BLKSIZE, sizeof(TFTP_OPT_BLKSIZE))) {
            if (atoi(state->block_buffer + 2 + sizeof(TFTP_OPT_BLKSIZE)) <= state->opt_blocksize) {
                state->opt_blocksize = atoi(state->block_buffer + 2 + sizeof(TFTP_OPT_BLKSIZE));
                state->block_buffer_size = state->opt_blocksize + TFTP_HDR_SIZE;
                //printf("OACK blksize is %u\n",state->opt_blocksize);
                send_ack(state->client, state->blockid);
                state->blockid++;
                return TRUE;
            }
        }
    }

    send_error(state->client, TFTP_ERR_OPTION);
    return FALSE;
}

static BOOL handle_send_rrq(tftp_client_state_t *cli, const char *filename)
{
    cli->block_buffer = malloc(cli->opt_blocksize + TFTP_HDR_SIZE);
    if (!cli->block_buffer) {
        printf("Out of memory.\n");
        return FALSE;
    }
    cli->block_buffer_size = cli->opt_blocksize + TFTP_HDR_SIZE;

    cli->file = fopen(filename,"wb+");
    if (!cli->file) {
        printf("Cannot create local file %s.\n",filename);
        return FALSE;
    }

    if (SOCK_ERR_OK != send_rrq(cli->client,
                                filename,
                                TFTP_MODE_BIN,
                                cli->opt_blocksize,
                                (struct sockaddr *)&cli->destination,
                                sizeof(cli->destination))) {
        printf("Cannot connect to server.\n");
        return FALSE;
    }

    return TRUE;
}

static BOOL handle_rrq(tftp_client_state_t *cli, const char *filename)
{
    short pak_type;
    short *netbuf_shorts;
    sock_errno_e retcode;
    struct sockaddr addr;
    int addrsize = sizeof(addr);
    BOOL first_try = TRUE;

    if (!handle_send_rrq(cli, filename)) {
        return FALSE;
    }

    netbuf_shorts = (short *)cli->block_buffer;

    while (TRUE) {

        retcode = receive_packet_un(cli->client,
                                    cli->block_buffer,
                                    &cli->block_buffer_size,
                                    &addr,
                                    &addrsize,
                                    first_try);

        switch (retcode) {
        case SOCK_ERR_OK:
            break;
        case SOCK_ERR_TIMEOUT:
            if (handle_rrq_timeout(cli, filename)) {
                continue;
            } else {
                return FALSE;
            }
            break;

        default:
            return FALSE;
        }

        pak_type = ntohs(netbuf_shorts[0]);

        switch (pak_type) {
        case TFTP_OACK:
            /* FALLTHRU */
        case TFTP_DATA:
            first_try = FALSE;

            retcode = receive_packet_un(cli->client,
                                        cli->block_buffer,
                                        &cli->block_buffer_size,
                                        &addr,
                                        &addrsize,
                                        first_try);

            if ((SOCK_ERR_OK != retcode) ||
                    SOCK_ERR_OK != sock_connect(cli->client, &addr, addrsize) ||
                    SOCK_ERR_OK != sock_set_timeout(cli->client, TFTP_CLNT_TIMEOUT_MS)) {
                return FALSE;
            }

            if (TFTP_OACK == pak_type) {
                if (!handle_oack_receive(cli)) {
                    send_error(cli->client, TFTP_ERR_OPTION);
                    return FALSE;
                }
                return TRUE;

            } else {
                if (!handle_data_receive(cli)) {
                    send_error(cli->client, TFTP_ERR_DISKFULL);
                    return FALSE;
                }
                cli->opt_blocksize = TFTP_DEFAULT_DATA;
                cli->block_buffer_size = cli->opt_blocksize + TFTP_HDR_SIZE;
                send_ack(cli->client, cli->blockid);
                return TRUE;
            }

            break;

        case TFTP_ERROR:
            receive_error(cli->block_buffer);
            return FALSE;

        case TFTP_RRQ:
            /* FALLTHRU */
        case TFTP_WRQ:
            /* FALLTHRU */
        case TFTP_ACK:
            printf("Unhandled packet type %d in %s\n", pak_type, __FUNCTION__);
            break;

        default:
            printf("Unknown packet type %d in %s\n", pak_type, __FUNCTION__);
            break;
        }
    }
}

static void
handle_transfer(tftp_client_state_t *cli)
{
    short pak_type;
    short *netbuf_shorts = (short *)cli->block_buffer;
    sock_errno_e retcode;

    while (1) {

        retcode = receive_packet(cli->client,
                                 cli->block_buffer,
                                 &cli->block_buffer_size);

        switch (retcode) {
        case SOCK_ERR_OK:
            break;
        case SOCK_ERR_TIMEOUT:
            if (handle_timeout(cli)) {
                continue;
            } else {
                return;
            }
            break;

        default:
            return;
        }

        pak_type = ntohs(netbuf_shorts[0]);

        switch (pak_type) {
        case TFTP_RRQ:
            /* FALLTHRU */
        case TFTP_WRQ:
            /* FALLTHRU */
        case TFTP_ACK:
            /* FALLTHRU */
        case TFTP_OACK:
            printf("Unhandled packet type %d in %s\n", pak_type, __FUNCTION__);
            break;

        case TFTP_DATA:
            if (!handle_data_receive(cli)) {
                send_error(cli->client, TFTP_ERR_DISKFULL);
                return;
            }
            if (cli->state_flags & TFTP_SRV_LASTACK) {
                return;
            }
            break;

        case TFTP_ERROR:
            receive_error(cli->block_buffer);
            return;

        default:
            printf("Unknown packet type %d in %s\n", pak_type, __FUNCTION__);
            break;
        }
    }
}

sock_errno_e tftp_client(tftp_client_state_t *client, const char *filename)
{
    if (client->state_flags & TFTP_SRV_RCVING) {
        if (handle_rrq(client, filename)) {
            handle_transfer(client);
        } else {
            return SOCK_ERR_FAIL;
        }
    }

    return SOCK_ERR_OK;
}
