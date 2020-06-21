#       make_w32.mak
#

!include	..\Makedefs.w32

.SUFFIXES :	.y .l .c .h .obj .lib
.y.c :
.l.c :
.c.obj :
	$(CC) $(CFLAGS) -c $<
.c.lib :
.mf.p :
.p.c :
.p.obj :


all :		mfcline.exe
install :	$(TOOL)\pgm\mfcline.exe
clean :
	-$(RM) *.obj *.prt prot mfcline.exe

mfcline.exe :			mfcline.obj
	$(link) $(LFLAGS) -out:mfcline.exe  mfcline.obj
$(TOOL)\pgm\mfcline.exe :	mfcline.obj
	$(link) $(LFLAGS) -out:mfcline.exe  mfcline.obj
	-del $(TOOL)\pgm\mfcline.exe
	copy mfcline.exe $(TOOL)\pgm

