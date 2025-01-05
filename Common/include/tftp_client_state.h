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

#ifndef TFTP_CLIENT_STATE_H_INCLUDED
#define TFTP_CLIENT_STATE_H_INCLUDED

#include <winsock2.h>
#include <stdio.h>
#include <types_common.h>

#include "tftp_common_values.h"

enum TFTP_SRV_STATE
{
        TFTP_SRV_WAITING = (1 << 0),
        TFTP_SRV_SENDING = (1 << 1),
        TFTP_SRV_RCVING = (1 << 2),
        TFTP_SRV_TIMEOUT = (1 << 3),
        TFTP_SRV_LASTACK = (1 << 4),
        TFTP_SRV_BLKSIZE = (1 << 5)
};

typedef struct tftp_client_state
{
        /* Support for RFC 1350 functionalities */
        SOCKET client;
        uint16_t blockid;
        uint16_t state_flags;
        FILE *file;
        char filename[TFTP_FILENAME_MAX + 1];
        char *block_buffer;
        unsigned block_buffer_size;
        unsigned timeout_count;
        struct sockaddr_in destination;
        uint32_t octets;
        /* Support for RFC 2347, 2348 functionalities*/
        uint16_t opt_blocksize;
} tftp_client_state_t;

void init_client_state(tftp_client_state_t *cli);

void reset_client_state(tftp_client_state_t *cli);

#endif // TFTP_CLIENT_STATE_H_INCLUDED
