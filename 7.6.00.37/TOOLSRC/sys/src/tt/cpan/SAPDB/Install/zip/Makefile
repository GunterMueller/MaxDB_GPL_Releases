# Makefile for zlib

!include <ntwin32.mak>

CC=cl
LD=link
CFLAGS=-I..\zlib -O -nologo
LDFLAGS=
O=.obj

# variables
OBJS = zip$(O) unzip$(O)

all:  ziplib.ro

zip.obj: zip.c zip.h
	$(CC) -c $(cvarsdll) $(CFLAGS) $*.c

unzip.obj: unzip.c unzip.h
	$(CC) -c $(cvarsdll) $(CFLAGS) $*.c

ziplib.ro: $(OBJS)
	lib -out:ziplib.ro $(OBJS)

clean:
	-del $(OBJS)
