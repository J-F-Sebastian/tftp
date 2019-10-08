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

#include <exception>
#include <cstring>
#include <iostream>
#include <cstdint>

#include "tftpPacket.h"

TFTPPacket::TFTPPacket(char *ptr, unsigned size, bool releaseInDtor) : buffer(ptr), bufferSize(size), bufferTail(0), release(releaseInDtor)
{
	if (buffer && bufferSize)
	{
		std::cout << __func__ << " buffer " << std::hex << (uintptr_t)buffer << std::dec << " size " << bufferSize << std::endl;
	}
	else
	{
		std::invalid_argument e("null buffer or null size");
		throw e;
	}
}

TFTPPacket::~TFTPPacket()
{
	if (release && buffer)
		delete buffer;

	buffer = nullptr;
	bufferSize = bufferTail = 0;
	release = false;
}

char *TFTPPacket::getBuffer()
{
	return buffer;
}

char *TFTPPacket::getBufferAtTail()
{
	return (bufferTail != bufferSize) ? (buffer + bufferTail) : (nullptr);
}

bool TFTPPacket::addOctet(char val)
{
	if (bufferTail < bufferSize)
	{
		buffer[bufferTail++] = val;
		return true;
	}

	return false;
}

bool TFTPPacket::addShort(short val)
{
	const char *src = (const char *)&val;

	if (bufferTail + 1 < bufferSize)
	{
		buffer[bufferTail++] = *src++;
		buffer[bufferTail++] = *src++;
		return true;
	}

	return false;
}
bool TFTPPacket::addWord(int val)
{
	const char *src = (const char *)&val;

	if (bufferTail + 3 < bufferSize)
	{
		buffer[bufferTail++] = *src++;
		buffer[bufferTail++] = *src++;
		buffer[bufferTail++] = *src++;
		buffer[bufferTail++] = *src++;
		return true;
	}

	return false;
}

char TFTPPacket::getOctet(unsigned pos)
{
	if (pos < bufferTail)
	{
		return buffer[pos];
	}

	std::out_of_range e(__FUNCTION__);
	throw e;
}

short TFTPPacket::getShort(unsigned pos)
{
	if ((2 * pos) < bufferTail)
	{
		return *reinterpret_cast<short *>(buffer + 2 * pos);
	}

	std::out_of_range e(__FUNCTION__);
	throw e;
}

int TFTPPacket::getWord(unsigned pos)
{
	if ((4 * pos) < bufferTail)
	{
		return *reinterpret_cast<int *>(buffer + 4 * pos);
	}

	std::out_of_range e(__FUNCTION__);
	throw e;
}

bool TFTPPacket::addBuffer(const char *src, unsigned size)
{
	if (src && (bufferTail + size < bufferSize))
	{
		memcpy(buffer + bufferTail, src, size);
		bufferTail += size;
		return true;
	}

	return false;
}

unsigned TFTPPacket::getBufferSize()
{
	return bufferSize;
}

unsigned TFTPPacket::getBufferTail()
{
	return bufferTail;
}

void TFTPPacket::setBufferTail(unsigned val)
{
	if (val <= bufferSize)
		bufferTail = val;
	else
		bufferTail = bufferSize;
}

void TFTPPacket::incBufferTail(unsigned val)
{
	unsigned temp = bufferTail + val;

	if (temp > bufferSize)
		temp = bufferSize;

	bufferTail = temp;
}

unsigned TFTPPacket::getBufferFreeOctets()
{
	return (bufferSize - bufferTail);
}

void TFTPPacket::resetBuffer(bool zeroTheBuffer)
{
	if (zeroTheBuffer)
		memset(buffer, 0, bufferTail);

	setBufferTail(0);
}
