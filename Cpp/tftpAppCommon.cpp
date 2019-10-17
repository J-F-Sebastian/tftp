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

#include <stdexcept>

#include "tftp.h"
#include "tftpAppCommon.h"

TFTPAppCommon::TFTPAppCommon() : filesize(0), blockSize(TFTP_BLKSIZE), windowSize(TFTP_WINSIZE), timeout(TFTP_TIMEOUT), tftpSocket(INVALID_SOCKET)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		throw std::runtime_error("WinSock failed.");
	}

	if (LOBYTE(wsaData.wVersion) != 2 ||
	    HIBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();
		throw std::invalid_argument("Winsock version 2.2 not available.");
	}

	tftpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (tftpSocket == INVALID_SOCKET)
	{
		WSACleanup();
		throw std::runtime_error("Socket error.");
	}
}

TFTPAppCommon::~TFTPAppCommon()
{
	closesocket(tftpSocket);
	WSACleanup();
}

void TFTPAppCommon::setFilesize(uint64_t newval)
{
	filesize = newval;
}

void TFTPAppCommon::setBlockSize(uint16_t newval)
{
	if ((newval < TFTP_MIN_BLKSIZE) || (newval > TFTP_MAX_BLKSIZE))
		throw(std::invalid_argument("Block size out of range."));

	blockSize = newval;
}

void TFTPAppCommon::setWindowSize(uint16_t newval)
{
	if ((newval < TFTP_MIN_WINSIZE) || (newval > TFTP_MAX_WINSIZE))
		throw(std::invalid_argument("Window size out of range."));

	windowSize = newval;
}

void TFTPAppCommon::setTimeout(uint8_t newval)
{
	if (timeout == 0)
		throw(std::invalid_argument("Timeout out of range."));

	timeout = newval;
}

void TFTPAppCommon::sendError(enum TFTP_ERRORS error)
{
	char buf[64];
	TFTPPacket pkt(buf, sizeof(buf));
	TFTPPacketFactory::inst().createERROR(pkt, error);
	::send(tftpSocket, pkt.getBuffer(), (int)pkt.getBufferSize(), 0);
}