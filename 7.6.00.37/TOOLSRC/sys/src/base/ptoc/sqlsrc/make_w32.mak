#  @(#)sqlsrc/make_w32.mak		2.0.5  1996-09-03  SQL DBS
#                       Makefile for 'PTOC' support modules
#                       Options etc. provided by '..\Makedefs'
#

!include ..\Makedefs.w32

CFLAGS	= $(CCFLAGS) $(OPTIONS)
LFLAGS	= $(LDFLAGS) $(OPTIONS)

.SUFFIXES :	.y .l .c .h .obj .lib
.y.c :
.l.c :
.c.obj :
	$(CC) $(CFLAGS) -c $<
.c.lib :


all :	ptc_delbody.exe ptc_ginit.exe opt_pt_c.exe

clean :
	del ptc_delbody.exe ptc_ginit.exe opt_pt_c.exe *.obj *.pdb

install :	ptc_delbody.exe ptc_ginit.exe opt_pt_c.exe
	copy ptc_delbody.exe $(TOOL)\pgm
	copy ptc_ginit.exe $(TOOL)\pgm
	copy opt_pt_c.exe $(TOOL)\bin

ptc_delbody.exe :	ptc_delbody.obj
	$(link) $(LFLAGS) ptc_delbody.obj -out:ptc_delbody.exe

ptc_ginit.exe :	ptc_ginit.obj
	$(link) $(LFLAGS) ptc_ginit.obj -out:ptc_ginit.exe

opt_pt_c.exe :	opt_pt_c.obj
	$(link) $(LFLAGS) opt_pt_c.obj -out:opt_pt_c.exe

