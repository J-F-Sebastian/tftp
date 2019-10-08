#include <iostream>

#include "tftp.h"

int main()
{
    TFTPPacket a;
    TFTPPacket b(12);
    std::string filename("pippo is fa la pippa.AVI");

    TFTPPacketFactory::inst().createRRQ(a, filename);
    TFTPPacketFactory::inst().createWRQ(a, filename);
    TFTPPacketFactory::inst().createACK(b, 666);

    TFTPPacketParser prs(a.getBuffer(), a.getBufferSize());

    std::cout << prs.type << "   "  << prs.packetStrings[0] << "   " << prs.packetStrings[1] << std::endl;
    return 0;
}