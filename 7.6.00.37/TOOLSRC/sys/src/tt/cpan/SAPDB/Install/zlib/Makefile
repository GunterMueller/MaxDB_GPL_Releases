# Makefile for zlib

!include <ntwin32.mak>

CC=cl
LD=link
CFLAGS=-O -nologo
LDFLAGS=
O=.obj

# variables
OBJ1 = adler32$(O) compress$(O) crc32$(O) gzio$(O) uncompr$(O) deflate$(O) trees$(O)
OBJ2 = zutil$(O) inflate$(O) infblock$(O) inftrees$(O) infcodes$(O) infutil$(O) inffast$(O)

OBJS = $(OBJ1) $(OBJ2)

all:  zlib.ro

adler32.obj: adler32.c zutil.h zlib.h zconf.h
	$(CC) -c $(cvarsdll) $(CFLAGS) $*.c

compress.obj: compress.c zlib.h zconf.h
	$(CC) -c $(cvarsdll) $(CFLAGS) $*.c

crc32.obj: crc32.c zutil.h zlib.h zconf.h
	$(CC) -c $(cvarsdll) $(CFLAGS) $*.c

deflate.obj: deflate.c deflate.h zutil.h zlib.h zconf.h
	$(CC) -c $(cvarsdll) $(CFLAGS) $*.c

gzio.obj: gzio.c zutil.h zlib.h zconf.h
	$(CC) -c $(cvarsdll) $(CFLAGS) $*.c

infblock.obj: infblock.c zutil.h zlib.h zconf.h infblock.h inftrees.h\
   infcodes.h infutil.h
	$(CC) -c $(cvarsdll) $(CFLAGS) $*.c

infcodes.obj: infcodes.c zutil.h zlib.h zconf.h inftrees.h infutil.h\
   infcodes.h inffast.h
	$(CC) -c $(cvarsdll) $(CFLAGS) $*.c

inflate.obj: inflate.c zutil.h zlib.h zconf.h infblock.h
	$(CC) -c $(cvarsdll) $(CFLAGS) $*.c

inftrees.obj: inftrees.c zutil.h zlib.h zconf.h inftrees.h
	$(CC) -c $(cvarsdll) $(CFLAGS) $*.c

infutil.obj: infutil.c zutil.h zlib.h zconf.h inftrees.h infutil.h
	$(CC) -c $(cvarsdll) $(CFLAGS) $*.c

inffast.obj: inffast.c zutil.h zlib.h zconf.h inftrees.h infutil.h inffast.h
	$(CC) -c $(cvarsdll) $(CFLAGS) $*.c

trees.obj: trees.c deflate.h zutil.h zlib.h zconf.h
	$(CC) -c $(cvarsdll) $(CFLAGS) $*.c

uncompr.obj: uncompr.c zlib.h zconf.h
	$(CC) -c $(cvarsdll) $(CFLAGS) $*.c

zutil.obj: zutil.c zutil.h zlib.h zconf.h
	$(CC) -c $(cvars) $(CFLAGS) $*.c

zlib.ro: $(OBJS)
	lib -out:zlib.ro $(OBJS)

clean:
	-del $(OBJS)
