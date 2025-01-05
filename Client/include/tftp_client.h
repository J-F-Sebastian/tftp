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

#ifndef TFTP_CLIENT_H_INCLUDED
#define TFTP_CLIENT_H_INCLUDED

#include "tftp_client_state.h"
#include "sock_utils.h"

#define TFTP_CLNT_FILENAME_MAX (128)
#define TFTP_CLNT_TIMEOUT_MS (1000)
#define TFTP_CLNT_RETRIES (5)

sock_errno_e tftp_client(tftp_client_state_t *client, const char *filename);

#endif // TFTP_CLIENT_H_INCLUDED
