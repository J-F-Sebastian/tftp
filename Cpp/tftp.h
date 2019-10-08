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

#ifndef _TFTP_H_
#define _TFTP_H_

#include <cstdint>
#include <string>
#include <map>

#include "tftp_protocol.h"
#include "tftpPacket.h"

class TFTPPacketFactory
{
    public:
        static TFTPPacketFactory &inst(void);

        bool createWRQ(TFTPPacket &pkt, std::string &filename);
        bool createRRQ(TFTPPacket &pkt, std::string &filename);
        bool createDATA(TFTPPacket &pkt, uint16_t blockNum, const char *data, size_t dataSize);
        bool createACK(TFTPPacket &pkt, uint16_t blockNum);
        bool createERROR(TFTPPacket &pkt, enum TFTP_ERROR error);
        bool createOACK(TFTPPacket &pkt);

        bool addOption(TFTPPacket &pkt, std::string &option, std::string &value);
};

class TFTPPacketParser
{
    public:
        TFTPPacketParser(TFTPPacket &pkt);
        ~TFTPPacketParser();

        enum TFTP_PAK_TYPE type;
        enum TFTP_ERROR error;
        uint16_t blockNum;
        size_t blockLen;
        const char *data;
        std::string errorMsg;
        std::map<std::string, std::string> packetStrings;
};

#endif