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

#Release or debug section
!ifdef DEBUG
MYOBJDIR = builddbg
!else
MYOBJDIR = build
!endif

MYOBJS = $(MYOBJDIR)\tftpPacket.obj $(MYOBJDIR)\tftp.obj $(MYOBJDIR)\tftpAppCommon.obj $(MYOBJDIR)\tftpAppClient.obj $(MYOBJDIR)\main.obj

CPPFLAGS = /W4 /GA /std:c++20 /EHcs /MP /c

!ifdef DEBUG
CPPFLAGS = $(CPPFLAGS) /Zi /O1
!else
CPPFLAGS = $(CPPFLAGS) /O2 /arch:AVX
!endif

!ifdef DEBUG
CLTCPPFLAGS = $(CPPFLAGS) /Fdtftpclientdbg.pdb /Fetftpclientdbg.exe
!else
CLTCPPFLAGS = $(CPPFLAGS) /Fetftpclient.exe
!endif

!ifdef DEBUG
LNFLAGS = /Zf /Zi /GA /MTd
CLTLNFLAGS = $(LNFLAGS) /Fdtftpclientdbg.pdb /Fetftpclientdbg.exe
!else
LNFLAGS = /GA /MT
CLTLNFLAGS = $(LNFLAGS) /Fetftpclient.exe
!endif

compileonly : $(MYOBJDIR) $(MYOBJS)

all : compileonly
 $(CPP) $(CLTLNFLAGS) $(MYOBJS) WS2_32.lib

{}.cpp{$(MYOBJDIR)}.obj::
 $(CPP) $(CPPFLAGS) /Fo$(MYOBJDIR)\ $<

$(MYOBJDIR) :
 -mkdir $@

clean :
 del /Q $(MYOBJDIR)\*.*
!ifdef DEBUG
 del /Q *dbg.exe
 del /Q *dbg.pdb
 del /Q *dbg.ilk
!else
 del /Q *.exe
!endif

cleanall :
 del /Q build\*.*
 del /Q builddbg\*.*
  del /Q tftpclientdbg.*
 del /Q tftpclient.exe
 del /Q *.pdb
