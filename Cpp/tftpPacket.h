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

#ifndef _TFTP_PACKET_H_
#define _TFTP_PACKET_H_

class TFTPPacket
{
	/*
	 * X - - - - - - - - - - - - - - - - - - - - - - - X
	 * \                                                \
	 *  \                                                \
	 *   buffer                                           buffer + bufferSize - 1
	 *   |                                                |
	 *   |                                                |
	 *   O - - - - - - - (bufferPos) - - - - - - - - - - ->
	 *
	 * buffer points to a memory location of bufferSize octets size.
	 * bufferTail points to any location inside the buffer memory area,
	 * its values range from 0 (beginning of a void buffer) to bufferSize
	 * (beyond-the-end-of-buffer position).
	 * bufferTail must be updated when writing data, so that it points to the last
	 * valid octet available for writing.
	 * bufferTail equals the amount of octets written into the buffer.
	 * Available free (writable) octets are computed subtracting bufferTail from
	 * bufferSize.
	 * When bufferTail equals bufferSize the buffer is full and cannot be updated anymore.
	 * The buffer can be optionally released (deleted) in the constructor, but using this
	 * option is STRONGLY DISCOURAGED !!! Use static buffers and reuse them, this improves
	 * dramatically the performances of your code.
	 */

	/* Pointer to the buffer */
	char *buffer;
	/* Size of the buffer, total amount of bytes available */
	unsigned bufferSize;
	/* Tail of the buffer, total amount of used bytes */
	unsigned bufferTail;
	/* Should the buffer be deallocated or not */
	bool release;

public:
	TFTPPacket(char *ptr, unsigned size, bool releaseInDtor = false);
	~TFTPPacket();

	/*
	 * Return a pointer to the beginning of the internal buffer.
	 *
	 * RETURN
	 * a valid pointer
	 */
	char *getBuffer(void);

	/*
	 * Return a pointer to the tail of the internal buffer, i.e. the
	 * first unused byte in the buffer.
	 * If the buffer is full, the tail fall behind the allocated buffer,
	 * the returned value is nullptr.
	 *
	 * RETURN
	 * a valid pointer if the tail is lower than the buffer size.
	 * nullptr if the buffer is full.
	 */
	char *getBufferAtTail(void);

	/*
	 * Append an octet (byte) to the buffer, increasing the tail position.
	 * In case the buffer is full, the method will fail.
	 *
	 * PARAMETER IN
	 * char val - the octet to be set into the buffer
	 *
	 * RETURN
	 * true if the octet was appended.
	 * false if the buffer was full and the octet was not appended.
	 */
	bool addOctet(char val);
	/*
	 * Append a short word (2 bytes) to the buffer, increasing the tail position.
	 * In case the buffer is full, the method will fail.
	 *
	 * PARAMETER IN
	 * short val - the octets to be set into the buffer
	 *
	 * RETURN
	 * true if the octets were appended.
	 * false if the buffer was full and the octets were not appended.
	 */
	bool addShort(short val);
	/*
	 * Append a word (4 bytes) to the buffer, increasing the tail position.
	 * In case the buffer is full, the method will fail.
	 *
	 * PARAMETER IN
	 * int val - the octets to be set into the buffer
	 *
	 * RETURN
	 * true if the octets were appended.
	 * false if the buffer was full and the octets were not appended.
	 */
	bool addWord(int val);
	/*
	 * Append a string of octets to the buffer, increasing the tail position.
	 * In case the buffer is full, the method will fail.
	 *
	 * PARAMETER IN
	 * const char *src - pointer to a string of octets to be set into the buffer
	 * unsigned size / the size of the string of octets in bytes
	 *
	 * RETURN
	 * true if the octets were appended.
	 * false if the buffer was full and the octets were not appended.
	 */
	bool addBuffer(const char *src, unsigned size);

	/*
	 * Return an octet from the buffer at the specified position.
	 *
	 * PARAMETER IN
	 * unsigned pos - position of the octet in the buffer
	 *
	 * THROW
	 * std::out_of_range if pos is outside the buffer's boundaries
	 */
	char getOctet(unsigned pos);
	/*
	 * Return a short word (2 bytes) from the buffer at the specified position.
	 *
	 * PARAMETER IN
	 * unsigned pos - position of the octets in the buffer
	 *
	 * THROW
	 * std::out_of_range if pos is outside the buffer's boundaries
	 */
	short getShort(unsigned pos);
	/*
	 * Return a word (4 bytes) from the buffer at the specified position.
	 *
	 * PARAMETER IN
	 * unsigned pos - position of the octets in the buffer
	 *
	 * THROW
	 * std::out_of_range if pos is outside the buffer's boundaries
	 */
	int getWord(unsigned pos);

	/*
	 * Return the size of the buffer in octets
	 *
	 * RETURN
	 * number of allocated bytes for the buffer
	 */
	unsigned getBufferSize(void);
	/*
	 * Return the size of the buffer's tail in octets
	 *
	 * RETURN
	 * number of used bytes in the buffer
	 */
	unsigned getBufferTail(void);
	/*
	 * Set the position of the buffer's tail in octets.
	 * If the position is past the end of the buffer its value
	 * is set to the end of buffer.
	 *
	 * PARAMETER IN
	 * position of the tail in the buffer
	 */
	void setBufferTail(unsigned val);
	/*
	 * Increase the position of the buffer's tail in octets.
	 * If the position is past the end of the buffer its value
	 * is set to the end of buffer.
	 *
	 * PARAMETER IN
	 * delta octets to be added to the tail position
	 */
	void incBufferTail(unsigned val);
	/*
	 * Return the amount of unused octets in the buffer.
	 * Unused octets are past the tail, till the end of the buffer.
	 *
	 * RETURN
	 * number of octets not used in the buffer
	 */
	unsigned getBufferFreeOctets(void);

	/*
	 * Set the buffer tail to zero.
	 * Optionally sets the buffer contents to zero.
	 *
	 * PARAMETER IN
	 * bool zeroTheBuffer - wether the buffer is to be zeroed or not
	 */
	void resetBuffer(bool zeroTheBuffer = false);
};

#endif
