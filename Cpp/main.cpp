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

#include <iostream>

#include "tftpAppClient.h"

static void printHelp()
{
	std::cout << "Syntax: tftpc [get | put] host filename" << std::endl;
}

int main(int argc, char *argv[])
{
	/*
	 * Syntax:
	 * tftpc [get | put] address filename
	 */
	if (argc < 4)
	{
		printHelp();
		return -1;
	}

	std::string op(argv[1]);
	std::string remote(argv[2]);
	std::string filename(argv[3]);

	if ((op != "get") && (op != "put"))
	{
		std::cout << "ERROR: invalid syntax" << std::endl;
		return -2;
	}
	if (INADDR_NONE == inet_addr(remote.c_str()))
	{
		std::cout << "ERROR: invalid syntax" << std::endl;
		return -3;
	}

	std::cout << op << " address " << remote << std::endl;
	TFTPAppClient client(remote, filename, (op.compare(0, 3, "get") == 0) ? true : false);
	client.setBlockSize(1024);
	if (client.initialize())
	{
		client.run();
		client.stop();
	}

	return 0;
}