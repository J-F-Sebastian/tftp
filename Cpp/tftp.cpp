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

#include <cstring>
#include <iostream>
#include <winsock2.h>

#include "tftp.h"

static const std::string TFTP_ERRMSG[] =
    {
	"Undefined error.",
	"File not found.",
	"Access violation.",
	"Disk full or allocation exceeded.",
	"Illegal TFTP operation.",
	"Unknown transfer ID.",
	"File already exists.",
	"No such user.",
	"Unsupported option.",
	"No error."};

/********************************************************************
 ***************** TFTPPacketFactory Class **************************
 ********************************************************************/

TFTPPacketFactory &TFTPPacketFactory::inst()
{
	static TFTPPacketFactory singleton;
	return singleton;
}

bool TFTPPacketFactory::createWRQ(TFTPPacket &pkt, std::string &filename)
{
	return (pkt.addShort(htons(TFTP_WRQ)) &&
		pkt.addBuffer(filename.c_str(), filename.size()) &&
		pkt.addOctet('\0') &&
		pkt.addBuffer(TFTP_MODE_BIN, sizeof(TFTP_MODE_BIN)))
		   ? true
		   : false;
}

bool TFTPPacketFactory::createRRQ(TFTPPacket &pkt, std::string &filename)
{
	return (pkt.addShort(htons(TFTP_RRQ)) &&
		pkt.addBuffer(filename.c_str(), filename.size()) &&
		pkt.addOctet('\0') &&
		pkt.addBuffer(TFTP_MODE_BIN, sizeof(TFTP_MODE_BIN)))
		   ? true
		   : false;
}

bool TFTPPacketFactory::createDATA(TFTPPacket &pkt, uint16_t blockNum, unsigned dataSize, const char *data)
{
	return (pkt.addShort(htons(TFTP_DATA)) &&
		pkt.addShort(htons(blockNum)) &&
		pkt.addBuffer(data, dataSize))
		   ? true
		   : false;
}

bool TFTPPacketFactory::createACK(TFTPPacket &pkt, uint16_t blockNum)
{
	return (pkt.addShort(htons(TFTP_ACK)) &&
		pkt.addShort(htons(blockNum)))
		   ? true
		   : false;
}

bool TFTPPacketFactory::createERROR(TFTPPacket &pkt, enum TFTP_ERRORS error)
{
	return (pkt.addShort(htons(TFTP_ERROR)) &&
		pkt.addShort((uint16_t)error) &&
		pkt.addBuffer(TFTP_ERRMSG[error].c_str(), TFTP_ERRMSG[error].length()) &&
		pkt.addOctet('\0'))
		   ? true
		   : false;
}

bool TFTPPacketFactory::createOACK(TFTPPacket &pkt)
{
	// TBD
	return (pkt.addShort(htons(TFTP_OACK)))
		   ? true
		   : false;
}

bool TFTPPacketFactory::addOption(TFTPPacket &pkt, const std::string &option, const std::string &value)
{
	if (pkt.getBufferFreeOctets() < option.length() + 1 + value.length() + 1)
	{
		return false;
	}

	switch (ntohs(pkt.getShort(0)))
	{
	case TFTP_WRQ:
	/* FALLTHRU */
	case TFTP_RRQ:
	/* FALLTHRU */
	case TFTP_OACK:
	{
		return (pkt.addBuffer(option.c_str(), option.length()) &&
			pkt.addOctet('\0') &&
			pkt.addBuffer(value.c_str(), value.length()) &&
			pkt.addOctet('\0'))
			   ? true
			   : false;
	}
	default:
		return false;
	}
}

/********************************************************************
 ****************** TFTPPacketParser Class **************************
 ********************************************************************/
TFTPPacketParser::TFTPPacketParser(TFTPPacket &pckt)
{
	unsigned pktlen = pckt.getBufferTail();
	unsigned bufpos = 0;

	if (!pckt.getBuffer() || !pktlen)
		throw std::invalid_argument("NULL packet");

	type = static_cast<enum TFTP_PACKET_TYPE>(ntohs(pckt.getShort(0)));
	if ((type < TFTP_RRQ) || (type > TFTP_OACK))
		throw std::range_error("unknown packet");

	if ((type == TFTP_DATA) || (type == TFTP_ACK))
	{
		blockNum = ntohs(pckt.getShort(1));
	}
	else
	{
		blockNum = 0;
	}

	if (type == TFTP_DATA)
	{
		data = pckt.getBuffer() + 4;
		blockLen = pktlen - 4;
	}
	else
	{
		data = 0;
		blockLen = 0;
	}

	if (type == TFTP_ERROR)
	{
		error = static_cast<enum TFTP_ERRORS>(ntohs(pckt.getShort(1)));
		errorMsg.assign(pckt.getBuffer() + 4);
	}
	else
	{
		error = TFTP_ERR_MAX;
		errorMsg.assign(TFTP_ERRMSG[error]);
	}

	if ((type == TFTP_RRQ) || (type == TFTP_WRQ) || (type == TFTP_OACK))
	{
		pktlen -= 2;
		bufpos = 2;
		std::string key, val;
		while (pktlen)
		{
			key.assign(pckt.getBuffer() + bufpos);
			if (pktlen > key.length())
			{
				pktlen -= key.length() + 1;
				bufpos += key.length() + 1;
			}
			else
			{
				break;
			}
			val.assign(pckt.getBuffer() + bufpos);
			if (pktlen > val.length())
			{
				pktlen -= val.length() + 1;
				bufpos += val.length() + 1;
			}
			else
			{
				break;
			}
			packetStrings.emplace(key, val);
			std::cout << "KEY : " << key << "\t VAL : " << val << std::endl;
		}
	}
}

TFTPPacketParser::~TFTPPacketParser()
{
	packetStrings.clear();
}