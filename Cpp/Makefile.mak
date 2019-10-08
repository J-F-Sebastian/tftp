#OPTIONS = -I. /O2 /EHsc /Wall /c
OPTIONS = -I. /EHsc /W3 /Zi /c

all:
	cl $(OPTIONS) main.cpp
	cl $(OPTIONS) tftpPacket.cpp
	cl $(OPTIONS) tftp.cpp
	cl /MD /Fe:tftpc.exe ws2_32.lib *.obj

clean:
	del *.exe *.obj *.pdb