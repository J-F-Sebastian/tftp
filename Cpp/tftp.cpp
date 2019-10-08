/*
 * TFTP Client and Server
 *
 * Copyright (C) 2019 Diego Gallizioli
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

static const std::string TFTP_ERRMSG[] = {
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

static inline uint16_t *toWord(TFTPPacket &pkt)
{
    return reinterpret_cast<uint16_t *>(pkt.getBuffer());
}

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
    /*
     * TFTP_MODE_xxx is a string, so it is NULL terminated and sizeof() take
     * it inot account
     */
    try
    {
        pkt.resize(2 + filename.size() + 1 + sizeof(TFTP_MODE_BIN));
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
    toWord(pkt)[0] = static_cast<uint16_t>(htons(TFTP_WRQ));
    memcpy(pkt.getBuffer() + 2, filename.c_str(), filename.size());
    pkt.getBuffer()[2 + filename.size()] = '\0';
    memcpy(pkt.getBuffer() + 2 + filename.size() + 1, TFTP_MODE_BIN, sizeof(TFTP_MODE_BIN));
    return true;
}

bool TFTPPacketFactory::createRRQ(TFTPPacket &pkt, std::string &filename)
{
    /*
     * TFTP_MODE_xxx is a string, so it is NULL terminated and sizeof() take
     * it inot account
     */
    try
    {
        pkt.resize(2 + filename.size() + 1 + sizeof(TFTP_MODE_BIN));
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
    toWord(pkt)[0] = static_cast<uint16_t>(htons(TFTP_RRQ));
    memcpy(pkt.getBuffer() + 2, filename.c_str(), filename.length());
    pkt.getBuffer()[2 + filename.size()] = '\0';
    memcpy(pkt.getBuffer() + 2 + filename.length() + 1, TFTP_MODE_BIN, sizeof(TFTP_MODE_BIN));
    return true;
}

bool TFTPPacketFactory::createDATA(TFTPPacket &pkt, uint16_t blockNum, const char *data, size_t dataSize)
{
    try
    {
        pkt.resize(2 + 2 + dataSize);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
    toWord(pkt)[0] = static_cast<uint16_t>(htons(TFTP_DATA));
    toWord(pkt)[1] = htons(blockNum);
    memcpy(pkt.getBuffer() + 2 + 2, data, dataSize);
    return true;
}

bool TFTPPacketFactory::createACK(TFTPPacket &pkt, uint16_t blockNum)
{
    try
    {
        pkt.resize(2 + 2);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
    toWord(pkt)[0] = static_cast<uint16_t>(htons(TFTP_ACK));
    toWord(pkt)[1] = htons(blockNum);
    return true;
}

bool TFTPPacketFactory::createERROR(TFTPPacket &pkt, enum TFTP_ERROR error)
{
    try
    {
        pkt.resize(2 + 2 + TFTP_ERRMSG[error].size() + 1);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
    toWord(pkt)[0] = static_cast<uint16_t>(htons(TFTP_ERROR));
    toWord(pkt)[1] = static_cast<uint16_t>(htons(error));
    memcpy(pkt.getBuffer() + 2 + 2, TFTP_ERRMSG[error].c_str(), TFTP_ERRMSG[error].length());
    pkt.getBuffer()[2 + 2 + TFTP_ERRMSG[error].length()] = '\0';
    return true;
}
bool TFTPPacketFactory::createOACK(TFTPPacket &pkt)
{
    try
    {
        pkt.resize(2);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
    toWord(pkt)[0] = static_cast<uint16_t>(htons(TFTP_OACK));
    return true;
}

bool TFTPPacketFactory::addOption(TFTPPacket &pkt, std::string &option, std::string &value)
{
    if (pkt.getBufferSize() < 2)
    {
        return false;
    }

    switch (ntohs(toWord(pkt)[0]))
    {
    case TFTP_WRQ:
    /* FALLTHRU */
    case TFTP_RRQ:
    /* FALLTHRU */
    case TFTP_OACK:
    {
        size_t oldbufsize = pkt.getBufferSize();
        try
        {
            pkt.resize(pkt.getBufferSize() + option.length() + 1 + value.length() + 1);
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
            return false;
        }
        memcpy(pkt.getBuffer() + oldbufsize, option.c_str(), option.length());
        oldbufsize += option.size();
        pkt.getBuffer()[oldbufsize] = '\0';
        memcpy(pkt.getBuffer() + oldbufsize + 1, value.c_str(), value.length());
        oldbufsize += value.length() + 1;
        pkt.getBuffer()[oldbufsize] = '\0';
        break;
    }
    default:
        return false;
    }

    return true;
}

/********************************************************************
 ****************** TFTPPacketParser Class **************************
 ********************************************************************/
TFTPPacketParser::TFTPPacketParser(TFTPPacket &pckt)
{
    const char *pkt = pckt.getBuffer();
     size_t pktlen = pckt.getBufferSize();

    if (!pkt || !pktlen)
        throw std::invalid_argument("NULL packet");

    const uint16_t *pkt16 = reinterpret_cast<const uint16_t *>(pkt);
    type = static_cast<enum TFTP_PAK_TYPE>(ntohs(pkt16[0]));
    if ((type < TFTP_RRQ) || (type > TFTP_OACK))
        throw std::range_error("unknown packet");

    if ((type == TFTP_DATA) || (type == TFTP_ACK))
    {
        blockNum = ntohs(pkt16[1]);
    }
    else
    {
        blockNum = 0;
    }

    if (type == TFTP_DATA)
    {
        data = pkt + 4;
        blockLen = pktlen - 4;
    }
    else
    {
        data = 0;
        blockLen = 0;
    }

    if (type == TFTP_ERROR)
    {
        error = static_cast<enum TFTP_ERROR>(ntohs(pkt16[1]));
        errorMsg.assign(pkt + 4);
    }
    else
    {
        error = TFTP_ERR_MAX;
        errorMsg.assign(TFTP_ERRMSG[error]);
    }

    if ((type == TFTP_RRQ) || (type == TFTP_WRQ) || (type == TFTP_OACK))
    {
        pktlen -= 2;
        pkt += 2;
        std::string key,val;
        while (pktlen)
        {
            key.assign(pkt);
            if (pktlen > key.length())
            {
                pktlen -= key.length() + 1;
                pkt += key.length() + 1;
            }
            else
            {
                break;
            }
            val.assign(pkt);
            if (pktlen > val.length())
            {
                pktlen -= val.length() + 1;
                pkt += val.length() + 1;
            }
            else
            {
                break;
            }
            packetStrings.emplace(key, val);
        }
    }
}

TFTPPacketParser::~TFTPPacketParser()
{
    packetStrings.clear();
}