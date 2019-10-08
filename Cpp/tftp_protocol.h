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

#ifndef TFTP_PROTOCOL_H_
#define TFTP_PROTOCOL_H_

#define TFTP_PORT_NAME "69"
#define TFTP_PORT_NUM (69)

/* RFC 1350 */
#define TFTP_HDR_SIZE (4)
#define TFTP_BLKSIZE (512)
/* These are defaults, not specified by RFC 1350 */
#define TFTP_TIMEOUT (3)
#define TFTP_WINSIZE (1)

/* RFC 2348 */
#define TFTP_MIN_BLKSIZE (8)
#define TFTP_MAX_BLKSIZE (65454)

/* RFC 2349 */
#define TFTP_MIN_TIMEOUT (1)
#define TFTP_MAX_TIMEOUT (255)
#define TFTP_MIN_TSIZE (1)
#define TFTP_MAX_TSIZE (2 * 1024 * 1024 * 1024UL)

/* RFC 7440 */
#define TFTP_MIN_WINSIZE (1)
#define TFTP_MAX_WINSIZE (65534)

enum TFTP_PACKET_TYPE
{
	/* RFC 1350 */
	TFTP_RRQ = 1,
	TFTP_WRQ = 2,
	TFTP_DATA = 3,
	TFTP_ACK = 4,
	TFTP_ERROR = 5,
	/* RFC 2347 */
	TFTP_OACK = 6
};

enum TFTP_ERRORS
{
	/* RFC 1350 */
	/* Not defined, see error message (if any). */
	TFTP_ERR_UNDEF = 0,
	/* File not found. */
	TFTP_ERR_NOTFOUND,
	/* Access violation. */
	TFTP_ERR_ACCESS,
	/* Disk full or allocation exceeded. */
	TFTP_ERR_DISKFULL,
	/* Illegal TFTP operation. */
	TFTP_ERR_ILLEGAL,
	/* Unknown transfer ID. */
	TFTP_ERR_UNKNOWNID,
	/* File already exists. */
	TFTP_ERR_FILEEXIST,
	/* No such user. */
	TFTP_ERR_NOUSER,
	/* RFC 2347 */
	/* Unsupported option */
	TFTP_ERR_OPTION,
	/* Number of known errors */
	TFTP_ERR_MAX
};

/* RFC 1350 */
#define TFTP_MODE_ASCII "netascii"
#define TFTP_MODE_BIN "octet"
#define TFTP_MODE_MAIL "mail"

/* RFC 2348 */
#define TFTP_OPT_BLKSIZE "blksize"

/* RFC 2349 */
#define TFTP_OPT_TIMEOUT "timeout"
#define TFTP_OPT_TSIZE "tsize"

/* RFC 7440 */
#define TFTP_OPT_WINSIZE "windowsize"

#endif // TFTP_PROTOCOL_H_INCLUDED
