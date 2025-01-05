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

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <exception>

#include "tftpAppClient.h"

using namespace std::chrono;

#define UDPBUFSIZE 9500

static const std::string tftpOptBlkSize = TFTP_OPT_BLKSIZE;
static const std::string tftpOptWinSize = TFTP_OPT_WINSIZE;
static const std::string tftpOptTimeOut = TFTP_OPT_TIMEOUT;
static const std::string tftpOptTSize = TFTP_OPT_TSIZE;

unsigned TFTPAppClient::createRequest(TFTPPacket &pkt)
{
	unsigned options = 0;
	std::string temp;

	try
	{
		/* Create request */
		if (readRequest)
			TFTPPacketFactory::inst().createRRQ(pkt, filename);
		else
			TFTPPacketFactory::inst().createWRQ(pkt, filename);

		/* Add options if any */
		if (blockSize != TFTP_BLKSIZE)
		{
			temp = std::to_string(blockSize);
			TFTPPacketFactory::inst().addOption(pkt, tftpOptBlkSize, temp);
			options |= 1 << APP_OPT_BLKSIZE;
		}
		if (windowSize != TFTP_WINSIZE)
		{
			temp = std::to_string(windowSize);
			TFTPPacketFactory::inst().addOption(pkt, tftpOptWinSize, temp);
			options |= 1 << APP_OPT_WINSIZE;
		}
		if (timeout != TFTP_TIMEOUT)
		{
			temp = std::to_string(timeout);
			TFTPPacketFactory::inst().addOption(pkt, tftpOptTimeOut, temp);
			options |= 1 << APP_OPT_TIMEOUT;
		}
		if (filesize || readRequest)
		{
			temp = std::to_string(filesize);
			TFTPPacketFactory::inst().addOption(pkt, tftpOptTSize, temp);
			options |= 1 << APP_OPT_TSIZE;
		}
	}
	catch (const std::exception &e)
	{
		std::cerr << "EXCEPTION in " << __func__ << ", line " << __LINE__ << " : " << e.what() << std::endl;
		throw;
	}
	return options;
}

void TFTPAppClient::parseOptions(TFTPPacketParser &parsed, unsigned &options)
{
	std::map<std::string, std::string>::iterator opt;
	try
	{
		if (options & (1 << APP_OPT_BLKSIZE))
		{
			opt = parsed.packetStrings.find(tftpOptBlkSize);
			if (opt == parsed.packetStrings.end())
			{
				setBlockSize(TFTP_BLKSIZE);
				options &= ~(1U << 0);
			}
			else
				setBlockSize((uint16_t)atoi(opt->second.c_str()));
		}
		if (options & (1 << APP_OPT_WINSIZE))
		{
			opt = parsed.packetStrings.find(tftpOptWinSize);
			if (opt == parsed.packetStrings.end())
			{
				setWindowSize(TFTP_WINSIZE);
				options &= ~(1U << 1);
			}
			else
				setWindowSize((uint16_t)atoi(opt->second.c_str()));
		}
		if (options & (1 << APP_OPT_TIMEOUT))
		{
			opt = parsed.packetStrings.find(tftpOptTimeOut);
			if (opt == parsed.packetStrings.end())
			{
				setTimeout(TFTP_TIMEOUT);
				options &= ~(1U << 2);
			}
			else
				setTimeout((uint8_t)atoi(opt->second.c_str()));
		}
		if (options & (1 << APP_OPT_TSIZE))
		{
			opt = parsed.packetStrings.find(tftpOptTSize);
			if (opt == parsed.packetStrings.end())
			{
				setFilesize(0);
				options &= ~(1U << 3);
			}
			else
			{
				if (readRequest)
					setFilesize(atoll(opt->second.c_str()));
				else
				{
					if (std::to_string(filesize) != opt->second)
						throw(std::invalid_argument("Bad tsize value."));
				}
			}
		}
	}
	catch (const std::exception &e)
	{
		std::cerr << "EXCEPTION in " << __func__ << ", line " << __LINE__ << " : " << e.what() << std::endl;
		throw;
	}
}

TFTPAppClient::TFTPAppClient(std::string &srvaddress, std::string &filename, bool readreq) : TFTPAppCommon(), filename(filename), readRequest(readreq)
{
	if (inet_pton(AF_INET, srvaddress.c_str(), &srvAddress.sin_addr))
	{
		std::invalid_argument e(std::string("invalid address ").append(srvaddress));
		throw e;
	}

	srvAddress.sin_family = AF_INET;
	srvAddress.sin_port = htons(TFTP_PORT_NUM);
}

TFTPAppClient::~TFTPAppClient()
{
}

bool TFTPAppClient::initialize()
{
	SOCKADDR_IN service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = INADDR_ANY;
	service.sin_port = 0;
	DWORD tout = timeout * 1000;
	std::fstream fileTest(filename);

	/*
	 * Check file existance: must be present when sending (WRQ), must not when receiving (RRQ)
	 */
	if (readRequest && fileTest.good())
	{
		std::cerr << "ERROR: " << filename << " already exists." << std::endl;
		return false;
	}

	if (!readRequest && fileTest.bad())
	{
		std::cerr << "ERROR: " << filename << " does not exist." << std::endl;
		return false;
	}

	if (::setsockopt(tftpSocket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tout, sizeof(tout)))
	{
		std::cerr << "ERROR: could not set SO_RVCTIMEO." << std::endl;
		return false;
	}

	if (::bind(tftpSocket, (SOCKADDR *)&service, sizeof(service)))
	{
		std::cerr << "ERROR: could not bind socket." << std::endl;
		return false;
	}

	return true;
}

bool TFTPAppClient::run()
{
	char bufIn[UDPBUFSIZE], bufOut[UDPBUFSIZE];
	TFTPPacket pkt(bufIn, sizeof(bufIn)), answer(bufOut, sizeof(bufOut));
	unsigned options;
	SOCKADDR_IN peerAddress;
	int peerAddrSize = sizeof(peerAddress);
	int retcode = 0;
	uint16_t dataBlockNum = 0;
	std::ios_base::openmode mode = std::ios_base::binary;

	if (readRequest)
		mode |= std::ios_base::out;
	else
		mode |= std::ios_base::in;

	std::fstream diskFile(filename, mode);
	if (diskFile.bad())
	{
		std::cerr << "ERROR: could not open " << filename << std::endl;
		return false;
	}

	try
	{
		options = createRequest(pkt);
	}
	catch (std::exception &e)
	{
		std::cerr << "EXCEPTION in " << __func__ << ", line " << __LINE__ << " : " << e.what() << std::endl;
		return false;
	}

	unsigned retries = 0;
	while (retries++ < 3)
	{
		sendto(tftpSocket,
		       pkt.getBuffer(),
		       (int)pkt.getBufferTail(),
		       0,
		       reinterpret_cast<const SOCKADDR *>(&srvAddress),
		       sizeof(srvAddress));

		retcode = recvfrom(tftpSocket,
				   answer.getBuffer(),
				   (int)answer.getBufferSize(),
				   0,
				   reinterpret_cast<SOCKADDR *>(&peerAddress),
				   &peerAddrSize);

		if (retcode == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSAETIMEDOUT)
			{
				std::cout << "Server timed out on " << retries << " try." << std::endl;
				continue;
			}
		}
		else if (retcode <= 0)
			return false;
		else
			break;
	}
	if (retries > 3)
	{
		std::cout << "Server did not answer" << std::endl;
		return false;
	}

	pkt.resetBuffer(true);
	answer.setBufferTail((unsigned)retcode);
	TFTPPacketParser parsed(answer);
	if (parsed.type == TFTP_ERROR)
	{
		std::cout << "Server sent error " << parsed.error << " : " << parsed.errorMsg << std::endl;
		return false;
	}
	/*
	 * The first valid incoming packet is used to connect the socket
	 */
	if (connect(tftpSocket, reinterpret_cast<const SOCKADDR *>(&peerAddress), peerAddrSize))
	{
		return false;
	}

	/*
	 * Server answered with an acknowledge for options. We need to parse the message,
	 * validate options, eventually answer with an error and quit - or proceed with the transfer.
	 */
	if (parsed.type == TFTP_OACK)
	{
		if (!options)
		{
			sendError(TFTP_ERR_OPTION);
			return false;
		}
		try
		{
			parseOptions(parsed, options);
		}
		catch (std::exception &e)
		{
			std::cerr << "EXCEPTION in " << __func__ << ", line " << __LINE__ << " : " << e.what() << std::endl;
			sendError(TFTP_ERR_OPTION);
			return false;
		}
		if (readRequest)
		{
			TFTPPacketFactory::inst().createACK(pkt, dataBlockNum++);
			send(tftpSocket, pkt.getBuffer(), (int)pkt.getBufferSize(), 0);
		}
	}
	else if (((parsed.type == TFTP_DATA) && readRequest) ||
		 ((parsed.type == TFTP_ACK) && !readRequest))
	{
		setBlockSize(TFTP_BLKSIZE);
		setWindowSize(TFTP_WINSIZE);
		setTimeout(TFTP_TIMEOUT);
		setFilesize(0);
		options = 0;
		if (readRequest)
			diskFile.write(parsed.data, parsed.blockLen);
	}
	else
	{
		sendError(TFTP_ERR_ILLEGAL);
		return false;
	}

	answer.resetBuffer(true);
	DWORD tout = timeout * 1000;
	if (setsockopt(tftpSocket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tout, sizeof(tout)))
	{
		return false;
	}

	system_clock::time_point start = system_clock::now();

	if (readRequest)
	{
		char temp[16];
		std::string tempname;

		if (inet_ntop(AF_INET, &srvAddress.sin_addr, temp, sizeof(temp)))
			tempname.assign(temp);
		else
			tempname.assign("N/A");

		std::cout << "Retrieving " << filename << " from " << tempname << " with";
		if (options & (1 << APP_OPT_BLKSIZE))
		{
			std::cout << " block size " << blockSize;
		}
		if (options & (1 << APP_OPT_WINSIZE))
		{
			std::cout << " window size " << windowSize;
		}
		if (options & (1 << APP_OPT_TIMEOUT))
		{
			std::cout << " timeout " << (int)timeout;
		}
		if (options & (1 << APP_OPT_TSIZE))
		{
			std::cout << " transfer size " << filesize;
		}
		if (!options)
		{
			std::cout << " no options";
		}
		std::cout << std::endl;

		while (1)
		{
			pkt.resetBuffer();
			retries = 3;
			while (retries--)
			{
				retcode = recv(tftpSocket, pkt.getBuffer(), (int)pkt.getBufferSize(), 0);
				if (retcode == SOCKET_ERROR)
				{
					if (WSAGetLastError() == WSAETIMEDOUT)
						continue;
				}
				else if (retcode <= 0)
					return false;
				else
					break;
			}
			if (retries == UINT_MAX)
			{
				std::cout << "Server timed out" << std::endl;
				return false;
			}

			pkt.setBufferTail((unsigned)retcode);
			TFTPPacketParser parsedAnswer(pkt);
			/*
			 * RRQ receives DATA or ERROR packets and sends ACK.
			 * Oversized DATA packets are invalid and will stop the transfer.
			 * ERROR will stop the transfer.
			 */
			if (parsedAnswer.type == TFTP_ERROR)
			{
				std::cout << "Server sent error " << parsedAnswer.error << " : " << parsedAnswer.errorMsg << std::endl;
				return false;
			}
			if ((parsedAnswer.type != TFTP_DATA) || (parsedAnswer.blockLen > blockSize))
			{
				std::cout << "Server sent an invalid packet type " << parsedAnswer.type << std::endl;
				sendError(TFTP_ERR_ILLEGAL);
				return false;
			}
			/*
			 * If the DATA block ID is the last acknowledged, the server lost our ACK
			 * so send it again.
			 * If the data block ID is the current to be acknowledged, send an ACK.
			 * If the data block ID is unexpected send and ERROR and stop the transfer.
			 */
			if ((parsedAnswer.blockNum != dataBlockNum - 1) && (parsedAnswer.blockNum != dataBlockNum))
			{
				std::cout << "Server sent an out of sequence BlockID, expected " << dataBlockNum << " received " << parsedAnswer.blockNum << std::endl;
				sendError(TFTP_ERR_ILLEGAL);
				return false;
			}
			if (parsedAnswer.blockNum == dataBlockNum - 1)
				TFTPPacketFactory::inst().createACK(answer, dataBlockNum - 1);
			else
			{
				std::cout << "blockLen " << parsedAnswer.blockLen << std::endl;
				diskFile.write(parsedAnswer.data, parsedAnswer.blockLen);
				answer.resetBuffer();
				TFTPPacketFactory::inst().createACK(answer, dataBlockNum++);
			}

			send(tftpSocket, answer.getBuffer(), (int)answer.getBufferSize(), 0);

			/*
			 * If the DATA packet is smaller than blockSize (including 0 bytes !!!)
			 * then this is the last packet of the transfer, quit successfully !
			 */
			if (parsedAnswer.blockLen < blockSize)
				break;
		}
	}
	else
	{
		char temp[16];
		std::string tempname;

		if (inet_ntop(AF_INET, &srvAddress.sin_addr, temp, sizeof(temp)))
			tempname.assign(temp);
		else
			tempname.assign("N/A");

		std::cout << "Sending " << filename << " to " << tempname << " with";
		if (options & (1 << APP_OPT_BLKSIZE))
		{
			std::cout << " block size " << blockSize;
		}
		if (options & (1 << APP_OPT_WINSIZE))
		{
			std::cout << " window size " << windowSize;
		}
		if (options & (1 << APP_OPT_TIMEOUT))
		{
			std::cout << " timeout " << (int)timeout;
		}
		if (options & (1 << APP_OPT_TSIZE))
		{
			std::cout << " transfer size " << filesize;
		}
		if (!options)
		{
			std::cout << " no options";
		}
		std::cout << std::endl;

		while (1)
		{
			pkt.resetBuffer();
			retries = 3;
			TFTPPacketFactory::inst().createDATA(pkt, ++dataBlockNum, blockSize);
			diskFile.read(pkt.getBufferAtTail(), blockSize);
			pkt.incBufferTail(diskFile.gcount());
			while (retries--)
			{
				send(tftpSocket, pkt.getBuffer(), (int)pkt.getBufferTail(), 0);
				retcode = recv(tftpSocket, answer.getBuffer(), (int)answer.getBufferSize(), 0);
				if (retcode == SOCKET_ERROR)
				{
					if (WSAGetLastError() == WSAETIMEDOUT)
						continue;
				}
				else if (retcode <= 0)
					return false;
				else
					break;
			}
			if (retries == UINT_MAX)
			{
				std::cout << "Server timed out" << std::endl;
				return false;
			}

			answer.setBufferTail((unsigned)retcode);
			TFTPPacketParser parsedAnswer(answer);
			/*
			 * WRQ receives ACK or ERROR packets and sends DATA.
			 * ERROR will stop the transfer.
			 */
			if (parsedAnswer.type == TFTP_ERROR)
			{
				std::cout << "Server sent error " << parsedAnswer.error << " : " << parsedAnswer.errorMsg << std::endl;
				return false;
			}
			if (parsedAnswer.type != TFTP_ACK)
			{
				std::cout << "Server sent an invalid packet type " << parsedAnswer.type << std::endl;
				sendError(TFTP_ERR_ILLEGAL);
				return false;
			}
			/*
			 * If the data block ID is the current to be acknowledged, send an ACK.
			 * If the data block ID is unexpected send and ERROR and stop the transfer.
			 */
			if (parsedAnswer.blockNum != dataBlockNum)
			{
				std::cout << "Server sent an out of sequence BlockID" << std::endl;
				sendError(TFTP_ERR_ILLEGAL);
				return false;
			}

			if (diskFile.eof())
				break;
		}
	}

	std::fstream::pos_type fileLen;
	if (readRequest)
	{
		diskFile.flush();
		diskFile.seekp(diskFile.end);
		fileLen = diskFile.tellp();
	}
	else
	{
		diskFile.seekg(diskFile.end);
		fileLen = diskFile.tellg();
	}

	system_clock::time_point end = system_clock::now();
	std::chrono::duration<double> delta = end - start;
	std::cout << "Transfer completed in " << delta.count() << " secs, " << fileLen << " bytes." << std::endl;
	std::cout << "Transfer speed was " << (double)fileLen / delta.count() << " Bytes/s." << std::endl;
	return true;
}

bool TFTPAppClient::stop()
{
	return true;
}

void TFTPAppClient::configure()
{
}
