#  @(#)ptoc/make_w32.mak		2.0.5  1996-09-03  SQL DBS
#

!include ..\Makedefs.w32

DEFS    = -DOPTIMIZING -DFLEX -DJOCHEN
CFLAGS  = $(CCFLAGS) $(DEFS) $(OPTIONS)
LFLAGS	= $(LDFLAGS) $(OPTIONS)
PFX	=

.SUFFIXES :	.y .l .c .h .obj .lib
.y.c :
.l.c :
.c.obj :
	$(CC) $(CFLAGS) -c $<
.c.lib :


all:	ptoc.exe

clean:
	del ptoc.exe *.obj *.pdb

install:	ptoc.exe
	copy ptoc.exe $(TOOL)\bin\ptoc.exe
	copy ..\include\ptoc.h $(TOOL)\incl\ptoc.h

ptoc.exe:		ptoc.obj Version.obj f_shell.obj
	$(link) $(LFLAGS) ptoc.obj Version.obj f_shell.obj -out:ptoc.exe

