all:
	cd Common\src
	nmake -f Makefile.mak
	cd ..\..\Client\src
	nmake -f Makefile.mak
	cd ..\..\Server\src
	nmake -f Makefile.mak
