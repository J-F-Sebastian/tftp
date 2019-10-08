OPTS = -I..\include -I..\..\Common\include /Ox /c

all:
	cl $(OPTS) main.c 
	cl $(OPTS) tftp_server.c
	cl $(OPTS) tftp_server_client.c
	cl $(OPTS) tftp_server_io.c
	cl /MD /Fe:tftpsrv.exe ws2_32.lib ..\..\Common\src\Common.lib *.obj
