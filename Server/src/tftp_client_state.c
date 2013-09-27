#include <stdio.h>

#include "tftp_client_state.h"
#include "tftp_protocol.h"

void init_client_state(tftp_client_state_t *cli)
{
    cli->client = INVALID_SOCKET;
    cli->blockid = 0;
    cli->state_flags = TFTP_SRV_WAITING;
    cli->file = NULL;
    memset(cli->filename, 0, sizeof(cli->filename));
    cli->block_buffer = NULL;
    cli->block_buffer_size = 0;
    cli->timeout_count = 0;
    cli->octets = 0;
    cli->opt_blocksize = TFTP_DEFAULT_DATA;
    memset(&cli->destination, 0, sizeof(cli->destination));
}

void reset_client_state(tftp_client_state_t *cli)
{
    if (INVALID_SOCKET != cli->client)
    {
        shutdown(cli->client, SD_BOTH);
        closesocket(cli->client);
        cli->client = INVALID_SOCKET;
    }
    cli->blockid = 0;
    cli->state_flags = TFTP_SRV_WAITING;
    if (cli->file)
    {
        fclose(cli->file);
        cli->file = NULL;
    }
    memset(cli->filename, 0, sizeof(cli->filename));
    if (cli->block_buffer)
    {
        free(cli->block_buffer);
        cli->block_buffer = NULL;
    }
    cli->block_buffer_size = 0;
    cli->timeout_count = 0;
    cli->octets = 0;
    cli->opt_blocksize = TFTP_DEFAULT_DATA;
    memset(&cli->destination, 0, sizeof(cli->destination));
}
