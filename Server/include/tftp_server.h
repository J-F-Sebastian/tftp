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

#ifndef TFTP_SERVER_H_INCLUDED
#define TFTP_SERVER_H_INCLUDED

#include "sock_utils.h"

#define TFTP_SRV_DEFAULT_ROOT "c:\\temp\\"
#define TFTP_SRV_MAX_CLIENTS (4)
#define TFTP_SRV_BLKSIZE_MIN (TFTP_DEFAULT_DATA)
#define TFTP_SRV_BLKSIZE_MAX (TFTP_DEFAULT_DATA * 16)

sock_errno_e tftp_server(SOCKET sock);

#endif // TFTP_SERVER_H_INCLUDED
