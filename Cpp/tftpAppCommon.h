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

#ifndef _TFTP_APP_COMMON_H_
#define _TFTP_APP_COMMON_H_

#include <cstdint>
#include <winsock2.h>

class TFTPAppCommon
{
protected:
    uint64_t filesize;
    uint16_t blockSize, windowSize;
    uint8_t timeout;
    SOCKET tftpSocket;

public:
    TFTPAppCommon();
    virtual ~TFTPAppCommon();

    void setFilesize(uint64_t newval);
    void setBlockSize(uint16_t newval);
    void setWindowSize(uint16_t newval);
    void setTimeout(uint8_t newval);

    virtual bool initialize() = 0;
    virtual bool run() = 0;
    virtual bool stop() = 0;
};

#endif
