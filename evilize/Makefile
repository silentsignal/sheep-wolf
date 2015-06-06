# Copyright (C) 2006 Peter Selinger. This file is distributed under
# the terms of the GNU General Public License. See the file COPYING
# for details.

CC=i686-pc-mingw32-gcc
CFLAGS=-O3 -Wall -DSTDC_HEADERS -g -DMD5COLL_VERSION=\"${MD5COLL_VERSION}\" -DVERSION=\"${VERSION}\"

PACKAGE=evilize
VERSION=0.2
MD5COLL_VERSION=0.1s

TOOLS=evilize md5coll 
EXAMPLES=hello-erase hello erase

tools: ${TOOLS}
examples: ${EXAMPLES}

evilize: evilize.o md5.o md5coll_lib.o
md5coll: md5coll.o md5coll_lib.o
hello-erase: hello-erase.o goodevil.o

hello erase: evilize hello-erase
	./evilize hello-erase -g hello -e erase

# ----------------------------------------------------------------------
# Distribution

DISTFILES=evilize.c md5coll.c md5coll_lib.c md5coll_lib.h crib.h md5.h	\
 md5.c goodevil.c hello-erase.c Makefile COPYING MBSD-LICENSE README ChangeLog AUTHORS

dist: ${DISTFILES}
	rm -rf ${PACKAGE}-${VERSION}
	mkdir ${PACKAGE}-${VERSION}
	cp ${DISTFILES} ${PACKAGE}-${VERSION}
	tar zcf ${PACKAGE}-${VERSION}.tar.gz ${PACKAGE}-${VERSION}

#----------------------------------------------------------------------
clean:
	rm -f *.o

cleanall: clean
	rm -f ${TOOLS} ${EXAMPLES}

#----------------------------------------------------------------------
# The following dependencies were generated with
# makedepend -DHAVE_CONFIG_H -I.. -f- -Y *.c

# DO NOT DELETE

evilize.o: md5.h md5coll_lib.h crib.h
goodevil.o: crib.h
md5.o: md5.h
md5coll.o: md5coll_lib.h
md5coll_lib.o: md5coll_lib.h
