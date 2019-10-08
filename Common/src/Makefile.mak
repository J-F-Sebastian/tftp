OPTS = -I..\include /Ox /c

all:
	cl $(OPTS) sock_utils.c
	cl $(OPTS) tftp_client_state.c
	cl $(OPTS) tftp_common_io.c
	cl $(OPTS) tftp_logger.c
	lib /OUT:Common.lib *.obj
