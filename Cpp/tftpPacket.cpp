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

#include <exception>
#include <cstring>

#include "tftpPacket.h"

void TFTPPacket::resize(size_t newBufSize)
{
    if (bufferSize)
    {
        if (bufferSize < newBufSize)
        {
            char *newbuffer;
            try
            {
                newbuffer = new char[newBufSize];
            }
            catch (std::exception &e)
            {
                throw e;
            }
            memcpy(newbuffer, buffer, bufferSize);
            delete[] buffer;
            buffer = newbuffer;
        }
        bufferSize = newBufSize;
    }
    else
    {
        bufferSize = newBufSize;
        buffer = new char[bufferSize];
    }
}

void TFTPPacket::clear()
{
    if (bufferSize)
    {
        memset(buffer, 0, bufferSize);
    }
}