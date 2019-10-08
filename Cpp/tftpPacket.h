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

#ifndef _TFTP_PACKET_H_
#define _TFTP_PACKET_H_

class TFTPPacket
{
    char *buffer;
    size_t bufferSize;

    public:
        TFTPPacket() : buffer(0), bufferSize(0)
        {
        }

        TFTPPacket(size_t bufSize) : bufferSize(bufSize)
        {
            buffer = new char[bufferSize];
        }

        ~TFTPPacket()
        {
            if (buffer)
                delete []buffer;
        }

        char *getBuffer() { return buffer; }
        size_t getBufferSize() { return bufferSize; }
        void resize(size_t newBufSize);
        void clear();
};

#endif