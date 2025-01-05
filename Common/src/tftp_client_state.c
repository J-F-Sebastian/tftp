/*
 * TFTP Client and Server
 *
 * Copyright (C) 2013 - 2025 Diego Gallizioli
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
