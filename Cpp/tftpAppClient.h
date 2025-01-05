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

#ifndef _TFTP_APP_CLIENT_H_
#define _TFTP_APP_CLIENT_H_

#include <string>

#include "tftp.h"
#include "tftpAppCommon.h"

class TFTPAppClient : public TFTPAppCommon
{
	std::string filename;
	SOCKADDR_IN srvAddress;
	bool readRequest;

	/*
	 * Create a RRQ or a WRQ request message, options are appended accordingly.
	 * Options are added if the blocksize, the timeout or the window size have non default values.
	 * The transfer size is appended anytime.
	 * Throws exceptions.
	 */
	unsigned createRequest(TFTPPacket &pkt);

	/*
	 * Parse a message (OACK) and decode the acknowledged options.
	 * Invalid options or invalid values throws exceptions.
	 */
	void parseOptions(TFTPPacketParser &parsed, unsigned &options);

	/*
	 * Run the client in read mode, session started with a RRQ packet to the remote host.
	 * This method is dedicated to implement the receive loop inside run().
	 * Throws exceptions.
	 */
	// void readLoop(void);

	/*
	 * Run the client in write mode, session started with a WRQ packet to the remote host.
	 * This method is dedicated to implement the send loop inside run().
	 * Throws exceptions.
	 */
	// void writeLoop(void);

public:
	TFTPAppClient(std::string &srvaddress, std::string &filename, bool readRequest = true);
	virtual ~TFTPAppClient();

	virtual bool initialize() override;
	virtual bool run() override;
	virtual bool stop() override;

	void configure();
};

#endif
