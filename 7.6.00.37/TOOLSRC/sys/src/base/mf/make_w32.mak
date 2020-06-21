#	make_w32.mak
#

!include	..\Makedefs.w32

CFLAGS	= $(ICFLAGS)
LFLAGS	= $(ILFLAGS)

.SUFFIXES :	.y .l .c .h .obj .lib
.y.c :
.l.c :
.c.obj :
	$(CC) $(CFLAGS) -c $<
.c.lib :


all :		\
		mfconcom.exe \
		mfspl.exe \
		mfdef.exe \
		mfuse.exe \
		mfcod.exe \
		mfglp.exe \
		mfexp.exe \
		mfstm.exe \
		mfchk.exe \
		mftrc.exe \
		mftop.exe \
		mfcat.exe 

install :	\
		$(TOOL)\pgm\mfconcom.exe \
		$(TOOL)\pgm\mfspl.exe \
		$(TOOL)\pgm\mfdef.exe \
		$(TOOL)\pgm\mfuse.exe \
		$(TOOL)\pgm\mfcod.exe \
		$(TOOL)\pgm\mfglp.exe \
		$(TOOL)\pgm\mfexp.exe \
		$(TOOL)\pgm\mfstm.exe \
		$(TOOL)\pgm\mfchk.exe \
		$(TOOL)\pgm\mftrc.exe \
		$(TOOL)\pgm\mftop.exe \
		$(TOOL)\pgm\mfcat.exe 

clean :
	-$(RM) *.obj *.prt prot $(PGMS)


COMOBJ	= mfoption.obj mfscan.obj debug.obj 

ityp00.obj :	ityp00.h
debug.obj :	debug.h
mfscan.obj :	mf.h debug.h
mfoption.obj :	mf.h debug.h
mfhexstr.obj :	mf.h debug.h
mfstrlen.obj :	mf.h debug.h
mfconcom.obj :	mf.h debug.h
mfspl.obj :	mf.h debug.h
mfdef.obj :	mf.h debug.h
mfuse.obj :	mf.h debug.h
mfcod.obj :	mf.h debug.h
mfglp.obj :	mf.h debug.h
mfexp.obj :	mf.h debug.h
mfstm.obj :	mf.h debug.h
mfchk.obj :	mf.h debug.h
mftrc.obj :	mf.h debug.h
mfclr.obj :	mf.h debug.h
mftop.obj :	mf.h debug.h
mfcat.obj :	mf.h debug.h

y_tab.obj :	mf.h debug.h
	$(CC) $(CFLAGS) -DIGNORECOMMENT -DIGNORECASE -DIGNOREENV -c y_tab.c

#y_tab.c :	concom.y
#	yacc concom.y
#	sed -e '\^#[ 	]*include[ 	]*<stdlib\.h>\d' \
#	    -e '\^#[ 	]*include[ 	]*<string\.h>\d' \
#	    -e '\^#[ 	]*include[ 	]*<memory\.h>\d' \
#	    -e '\^#[ 	]*include[ 	]*<malloc\.h>\d' \
#	    -e '\^#[ 	]*line[ 	]\d' \
#	    y.tab.c >y_tab.c
#	$(RM) y.tab.c


ityp00.exe :	ityp00.obj
	$(link) $(LFLAGS) -out:ityp00.exe ityp00.obj 

mfhexstr.exe :	mfhexstr.obj $(COMOBJ) 
	$(link) $(LFLAGS) -out:mfhexstr.exe mfhexstr.obj $(COMOBJ) 

mfstrlen.exe :	mfstrlen.obj $(COMOBJ) 
	$(link) $(LFLAGS) -out:mfstrlen.exe mfstrlen.obj $(COMOBJ) 

mfconcom.exe :	mfconcom.obj debug.obj y_tab.obj
	$(link) $(LFLAGS) -out:mfconcom.exe mfconcom.obj debug.obj y_tab.obj 
$(TOOL)\pgm\mfconcom.exe :	mfconcom.obj debug.obj y_tab.obj
	$(link) $(LFLAGS) -out:mfconcom.exe mfconcom.obj debug.obj y_tab.obj 
	-$(RM) $(TOOL)\pgm\mfconcom.exe
	$(MV) mfconcom.exe $(TOOL)\pgm\.

mfspl.exe :	mfspl.obj mfoption.obj debug.obj 
	$(link) $(LFLAGS) -out:mfspl.exe mfspl.obj mfoption.obj debug.obj 
$(TOOL)\pgm\mfspl.exe :	mfspl.obj mfoption.obj debug.obj 
	$(link) $(LFLAGS) -out:mfspl.exe mfspl.obj mfoption.obj debug.obj 
	-$(RM) $(TOOL)\pgm\mfspl.exe
	$(MV) mfspl.exe $(TOOL)\pgm\.

mfdef.exe :	mfdef.obj $(COMOBJ) 
	$(link) $(LFLAGS) -out:mfdef.exe mfdef.obj $(COMOBJ) 
$(TOOL)\pgm\mfdef.exe :	mfdef.obj $(COMOBJ) 
	$(link) $(LFLAGS) -out:mfdef.exe mfdef.obj $(COMOBJ) 
	-$(RM) $(TOOL)\pgm\mfdef.exe
	$(MV) mfdef.exe $(TOOL)\pgm\.

mfuse.exe :	mfuse.obj $(COMOBJ) 
	$(link) $(LFLAGS) -out:mfuse.exe mfuse.obj $(COMOBJ) 
$(TOOL)\pgm\mfuse.exe :	mfuse.obj $(COMOBJ) 
	$(link) $(LFLAGS) -out:mfuse.exe mfuse.obj $(COMOBJ) 
	-$(RM) $(TOOL)\pgm\mfuse.exe
	$(MV) mfuse.exe $(TOOL)\pgm\.

mfcod.exe :	mfcod.obj $(COMOBJ) 
	$(link) $(LFLAGS) -out:mfcod.exe mfcod.obj $(COMOBJ) 
$(TOOL)\pgm\mfcod.exe :	mfcod.obj $(COMOBJ) 
	$(link) $(LFLAGS) -out:mfcod.exe mfcod.obj $(COMOBJ) 
	-$(RM) $(TOOL)\pgm\mfcod.exe
	$(MV) mfcod.exe $(TOOL)\pgm\.

mfglp.exe :	mfglp.obj $(COMOBJ) 
	$(link) $(LFLAGS) -out:mfglp.exe mfglp.obj $(COMOBJ) 
$(TOOL)\pgm\mfglp.exe :	mfglp.obj $(COMOBJ) 
	$(link) $(LFLAGS) -out:mfglp.exe mfglp.obj $(COMOBJ) 
	-$(RM) $(TOOL)\pgm\mfglp.exe
	$(MV) mfglp.exe $(TOOL)\pgm\.

mfexp.exe :	mfexp.obj $(COMOBJ) 
	$(link) $(LFLAGS) -out:mfexp.exe mfexp.obj $(COMOBJ) 
$(TOOL)\pgm\mfexp.exe :	mfexp.obj $(COMOBJ) 
	$(link) $(LFLAGS) -out:mfexp.exe mfexp.obj $(COMOBJ) 
	-$(RM) $(TOOL)\pgm\mfexp.exe
	$(MV) mfexp.exe $(TOOL)\pgm\.

mfstm.exe :	mfstm.obj $(COMOBJ) 
	$(link) $(LFLAGS) -out:mfstm.exe mfstm.obj $(COMOBJ) 
$(TOOL)\pgm\mfstm.exe :	mfstm.obj $(COMOBJ) 
	$(link) $(LFLAGS) -out:mfstm.exe mfstm.obj $(COMOBJ) 
	-$(RM) $(TOOL)\pgm\mfstm.exe
	$(MV) mfstm.exe $(TOOL)\pgm\.

mfchk.exe :	mfchk.obj $(COMOBJ)
	$(link) $(LFLAGS) -out:mfchk.exe mfchk.obj $(COMOBJ) 
$(TOOL)\pgm\mfchk.exe :	mfchk.obj $(COMOBJ)
	$(link) $(LFLAGS) -out:mfchk.exe mfchk.obj $(COMOBJ) 
	-$(RM) $(TOOL)\pgm\mfchk.exe
	$(MV) mfchk.exe $(TOOL)\pgm\.

mftrc.exe :	mftrc.obj $(COMOBJ)
	$(link) $(LFLAGS) -out:mftrc.exe mftrc.obj $(COMOBJ) 
$(TOOL)\pgm\mftrc.exe :	mftrc.obj $(COMOBJ)
	$(link) $(LFLAGS) -out:mftrc.exe mftrc.obj $(COMOBJ) 
	-$(RM) $(TOOL)\pgm\mftrc.exe
	$(MV) mftrc.exe $(TOOL)\pgm\.

mfclr.exe :	mfclr.obj $(COMOBJ)
	$(link) $(LFLAGS) -out:mfclr.exe mfclr.obj $(COMOBJ) 
$(TOOL)\pgm\mfclr.exe :	mfclr.obj $(COMOBJ)
	$(link) $(LFLAGS) -out:mfclr.exe mfclr.obj $(COMOBJ) 
	-$(RM) $(TOOL)\pgm\mfclr.exe
	$(MV) mfclr.exe $(TOOL)\pgm\.

mftop.exe :	mftop.obj $(COMOBJ) 
	$(link) $(LFLAGS) -out:mftop.exe mftop.obj $(COMOBJ) 
$(TOOL)\pgm\mftop.exe :	mftop.obj $(COMOBJ) 
	$(link) $(LFLAGS) -out:mftop.exe mftop.obj $(COMOBJ) 
	-$(RM) $(TOOL)\pgm\mftop.exe
	$(MV) mftop.exe $(TOOL)\pgm\.

mfcat.exe :	mfcat.obj mfoption.obj debug.obj 
	$(link) $(LFLAGS) -out:mfcat.exe mfcat.obj mfoption.obj debug.obj 
$(TOOL)\pgm\mfcat.exe :	mfcat.obj mfoption.obj debug.obj 
	$(link) $(LFLAGS) -out:mfcat.exe mfcat.obj mfoption.obj debug.obj 
	-$(RM) $(TOOL)\pgm\mfcat.exe
	$(MV) mfcat.exe $(TOOL)\pgm\.

