#Help
!message Building TFTP project
!message ---------------------------------------------
!message TARGETS
!message         compileonly -> target compiles but does not link
!message         all         -> compile and link
!message         clean       -> removes all derived objects
!message DEFINES
!message         DEBUG       -> will build for debug
!message ---------------------------------------------

#Global section
MYLIBSDIR = D:\sviluppo\tftp

#Release or debug section
!ifdef DEBUG
MYOBJCOMDIR = commondbg
MYOBJCLTDIR = clientdbg
MYOBJSRVDIR = serverdbg
!else
MYOBJCOMDIR = commonobj
MYOBJCLTDIR = clientobj
MYOBJSRVDIR = serverobj
!endif

MYOBJCOMS = $(MYOBJCOMDIR)\sock_utils.obj $(MYOBJCOMDIR)\tftp_client_state.obj $(MYOBJCOMDIR)\tftp_common_io.obj $(MYOBJCOMDIR)\tftp_logger.obj
MYOBJCLTS = $(MYOBJCLTDIR)\main.obj $(MYOBJCLTDIR)\tftp_client_io.obj $(MYOBJCLTDIR)\tftp_client.obj
MYOBJSRVS = $(MYOBJSRVDIR)\main.obj $(MYOBJSRVDIR)\tftp_server_client.obj $(MYOBJSRVDIR)\tftp_server_io.obj $(MYOBJSRVDIR)\tftp_server.obj

CPPFLAGS = /W4 /GA /std:c++20 /EHcs /MP /c

!ifdef DEBUG
CPPFLAGS = $(CPPFLAGS) /Zi /O1
!else
CPPFLAGS = $(CPPFLAGS) /O2 /arch:AVX
!endif

CPPFLAGS = $(CPPFLAGS) /I$(MYLIBSDIR)\Common\include
CPPFLAGS = $(CPPFLAGS) /I$(MYLIBSDIR)\Client\include
CPPFLAGS = $(CPPFLAGS) /I$(MYLIBSDIR)\Server\include

!ifdef DEBUG
CLTCPPFLAGS = $(CPPFLAGS) /Fdtftpclientdbg.pdb /Fetftpclientdbg.exe
SRVCPPFLAGS = $(CPPFLAGS) /Fdtftpserverdbg.pdb /Fetftpserverdbg.exe
!else
CLTCPPFLAGS = $(CPPFLAGS) /Fetftpclient.exe
SRVCPPFLAGS = $(CPPFLAGS) /Fetftpserver.exe
!endif

!ifdef DEBUG
LNFLAGS = /Zf /Zi /GA /MTd
CLTLNFLAGS = $(LNFLAGS) /Fdtftpclientdbg.pdb /Fetftpclientdbg.exe
SRVLNFLAGS = $(LNFLAGS) /Fdtftpserverdbg.pdb /Fetftpserverdbg.exe
!else
LNFLAGS = /GA /MT
CLTLNFLAGS = $(LNFLAGS) /Fetftpclient.exe
SRVLNFLAGS = $(LNFLAGS) /Fetftpserver.exe
!endif

compileonly : $(MYOBJCOMDIR) $(MYOBJCLTDIR) $(MYOBJSRVDIR) $(MYOBJCOMS) $(MYOBJCLTS) $(MYOBJSRVS)

all : $(MYOBJCOMDIR) $(MYOBJCLTDIR) $(MYOBJSRVDIR) $(MYOBJCOMS) $(MYOBJCLTS) $(MYOBJSRVS)
 $(CPP) $(CLTLNFLAGS) $(MYOBJCOMS) $(MYOBJCLTS) WS2_32.lib 
 $(CPP) $(SRVLNFLAGS) $(MYOBJCOMS) $(MYOBJSRVS) WS2_32.lib 

{Common\src}.c{$(MYOBJCOMDIR)}.obj::
 $(CPP) $(CPPFLAGS) /Fo$(MYOBJCOMDIR)\ $<

{Client\src}.c{$(MYOBJCLTDIR)}.obj::
 $(CPP) $(CLTCPPFLAGS) /Fo$(MYOBJCLTDIR)\ $<

{Server\src}.c{$(MYOBJSRVDIR)}.obj::
 $(CPP) $(SRVCPPFLAGS) /Fo$(MYOBJSRVDIR)\ $<

$(MYOBJCOMDIR) :
 -mkdir $@

$(MYOBJCLTDIR) :
 -mkdir $@

$(MYOBJSRVDIR) :
 -mkdir $@

clean :
 del /Q $(MYOBJCOMDIR)\*.*
 del /Q $(MYOBJCLTDIR)\*.*
 del /Q $(MYOBJSRVDIR)\*.*
!ifdef DEBUG
 del /Q *dbg.exe
 del /Q *dbg.pdb
 del /Q *dbg.ilk
!else
 del /Q *.exe
!endif

cleanall :
 del /Q commonobj\*.*
 del /Q clientobj\*.*
 del /Q serverobj\*.*
 del /Q commondbg\*.*
 del /Q clientdbg\*.*
 del /Q serverdbg\*.*
 del /Q tftpserverdbg.*
 del /Q tftpserver.exe
 del /Q tftpclientdbg.*
 del /Q tftpclient.exe
 del /Q *.pdb