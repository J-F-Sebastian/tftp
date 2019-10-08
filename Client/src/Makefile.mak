OPTS = -I..\include -I..\..\Common\include /Ox /c

all:
	cl $(OPTS) main.c 
	cl $(OPTS) tftp_client.c
	cl $(OPTS) tftp_client_io.c
	cl /MD /Fe:tftpc.exe ws2_32.lib ..\..\Common\src\Common.lib *.obj
