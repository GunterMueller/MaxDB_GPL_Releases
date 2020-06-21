#  @(#)ptocnt		2.10.0  
#                       Makefile for the 'PTOC' runtime library
#                       OS/2 and Windows NT only
#

!include ..\Makedefs.w32


COMP    = cl
AR      = lib /subsystem:console
LIBNAME =  ptocnt

!if "$(CPU)" == "IA64"
!ifdef DEBUG
CFLAGS  = $(cdebug) -c -nologo -Od -D_WIN32=1 -D_X86_=1 -DWIN32 $(cvarsmt) -D__PTOCLIB__
!else
CFLAGS  = $(cdebug) -c -nologo -Ogti -D_WIN32=1 -D_X86_=1 -DWIN32 $(cvarsmt) -D__PTOCLIB__
!endif
!else
CFLAGS  = $(cdebug) -c -nologo -G4 -Ogtyi -D_WIN32=1 -D_X86_=1 -DWIN32 $(cvarsmt) -D__PTOCLIB__
!endif

!if "$(CPU)" == "IA64"
OUT_DIR  =  .\common64
OUT_DIR1 =  .\rte64
OUT_DIR2 =  .\standard64
!else
OUT_DIR  =  .\common32
OUT_DIR1 =  .\rte32
OUT_DIR2 =  .\standard32
!endif

.SUFFIXES: .

INCLUDES =  H00VARS.H         \
            LIBPC.H           \
            WHOAMI.H


CFLAGS1  = $(CFLAGS) -DUSE_MSG_PRIO7
CFLAGS2  = $(CFLAGS)

!if "$(CPU)" == "IA64"
LIBNAME1 = $(LIBNAME)r64
LIBNAME2 = $(LIBNAME)s64
!else
LIBNAME1 = $(LIBNAME)r
LIBNAME2 = $(LIBNAME)s
!endif

OBJS     =  $(OUT_DIR)\sql__1r.obj       \
            $(OUT_DIR)\sql__atan.obj     \
            $(OUT_DIR)\sql__ca.obj       \
            $(OUT_DIR)\sql__caseerr.obj  \
            $(OUT_DIR)\sql__catchae.obj  \
#           $(OUT_DIR)\sql__ckl.obj      \
#           $(OUT_DIR)\sql__clock.obj    \
            $(OUT_DIR)\sql__close.obj    \
            $(OUT_DIR)\sql__closea.obj   \
            $(OUT_DIR)\sql__closep.obj   \
            $(OUT_DIR)\sql__compare.obj  \
            $(OUT_DIR)\sql__cos.obj      \
            $(OUT_DIR)\sql__date.obj     \
            $(OUT_DIR)\sql__del.obj      \
            $(OUT_DIR)\sql__dfn.obj      \
            $(OUT_DIR)\sql__dispse.obj   \
            $(OUT_DIR)\sql__endof.obj    \
            $(OUT_DIR)\sql__endol.obj    \
            $(OUT_DIR)\sql__env.obj      \
            $(OUT_DIR)\sql__er.obj       \
            $(OUT_DIR)\sql__erq.obj      \
            $(OUT_DIR)\sql__exit.obj     \
            $(OUT_DIR)\sql__exp.obj      \
            $(OUT_DIR)\sql__expp.obj     \
            $(OUT_DIR)\sql__fflush.obj   \
            $(OUT_DIR)\sql__flp.obj      \
            $(OUT_DIR)\sql__ger.obj      \
            $(OUT_DIR)\sql__get.obj      \
            $(OUT_DIR)\sql__gn.obj       \
            $(OUT_DIR)\sql__gtl.obj      \
            $(OUT_DIR)\sql__ic.obj       \
            $(OUT_DIR)\sql__intr.obj     \
            $(OUT_DIR)\sql__lel.obj      \
            $(OUT_DIR)\sql__lexe.obj     \
            $(OUT_DIR)\sql__ln.obj       \
            $(OUT_DIR)\sql__lor.obj      \
            $(OUT_DIR)\sql__ltl.obj      \
            $(OUT_DIR)\sql__max.obj      \
            $(OUT_DIR)\sql__multi.obj    \
            $(OUT_DIR)\sql__new.obj      \
            $(OUT_DIR)\sql__nil.obj      \
            $(OUT_DIR)\sql__nlf.obj      \
            $(OUT_DIR)\sql__nname.obj    \
            $(OUT_DIR)\sql__npa.obj      \
            $(OUT_DIR)\sql__nre.obj      \
            $(OUT_DIR)\sql__pack.obj     \
            $(OUT_DIR)\sql__pcnst.obj    \
            $(OUT_DIR)\sql__peer.obj     \
            $(OUT_DIR)\sql__pmain.obj    \
            $(OUT_DIR)\sql__pr.obj       \
            $(OUT_DIR)\sql__psetadd.obj  \
            $(OUT_DIR)\sql__psetco.obj   \
            $(OUT_DIR)\sql__psetge.obj   \
            $(OUT_DIR)\sql__psetgt.obj   \
            $(OUT_DIR)\sql__psetle.obj   \
            $(OUT_DIR)\sql__psetlt.obj   \
            $(OUT_DIR)\sql__psetm.obj    \
            $(OUT_DIR)\sql__pstrcat.obj  \
            $(OUT_DIR)\sql__pstrcom.obj  \
            $(OUT_DIR)\sql__pstrcpy.obj  \
            $(OUT_DIR)\sql__put.obj      \
#           $(OUT_DIR)\sql__pvars.obj    \
#           $(OUT_DIR)\sql__rd.obj       \
            $(OUT_DIR)\sql__readln.obj   \
            $(OUT_DIR)\sql__reset.obj    \
            $(OUT_DIR)\sql__rewrite.obj  \
            $(OUT_DIR)\sql__rgv.obj      \
            $(OUT_DIR)\sql__round.obj    \
            $(OUT_DIR)\sql__scnst.obj    \
            $(OUT_DIR)\sql__setadd.obj   \
            $(OUT_DIR)\sql__setco.obj    \
            $(OUT_DIR)\sql__setmin.obj   \
            $(OUT_DIR)\sql__setsub.obj   \
#           $(OUT_DIR)\sql__sin.obj      \
            $(OUT_DIR)\sql__sqrt.obj     \
#           $(OUT_DIR)\sql__srand.obj    \
            $(OUT_DIR)\sql__stopp.obj    \
            $(OUT_DIR)\sql__succ.obj     \
            $(OUT_DIR)\sql__sync.obj     \
            $(OUT_DIR)\sql__time.obj     \
#           $(OUT_DIR)\sql__trunc.obj    \
#           $(OUT_DIR)\sql__ucmp.obj     \
            $(OUT_DIR)\sql__uncs.obj     \
            $(OUT_DIR)\sql__unget.obj    \
            $(OUT_DIR)\sql__unpack.obj   \
            $(OUT_DIR)\sql__ut.obj       \
            $(OUT_DIR)\sql__writeln.obj  \
            $(OUT_DIR)\sql__pred.obj     \
            $(OUT_DIR)\version.obj       \
            $(OUT_DIR)\sql__strlen.obj

DIR1_OBJ  = $(OUT_DIR1)\sql__perrorp.obj  \
            $(OUT_DIR1)\sql__goto.obj

DIR2_OBJ  = $(OUT_DIR2)\sql__perrorp.obj  \
            $(OUT_DIR2)\sql__goto.obj

C_OBJS   =  $(OUT_DIR)\sql__setrang.obj

ASM_OBJS =  $(OUT_DIR)\sql__setrang.a.obj


!if "$(CPU)" == "IA64"
C_ASM_OBJS= C_OBJS
!else
C_ASM_OBJS= ASM_OBJS
!endif


all:  $(OUT_DIR1)\$(LIBNAME1).lib  $(OUT_DIR2)\$(LIBNAME2).lib

install:  $(OUT_DIR1)\$(LIBNAME1).lib  $(OUT_DIR2)\$(LIBNAME2).lib
	  copy $(OUT_DIR1)\$(LIBNAME1).lib $(TOOL)\lib\$(LIBNAME1).lib
	  copy $(OUT_DIR2)\$(LIBNAME2).lib $(TOOL)\lib\$(LIBNAME2).lib

clean :
	$(RM) $(OUT_DIR)\*.obj $(OUT_DIR1)\*.lib $(OUT_DIR2)\*.lib $(OUT_DIR2)\*.obj $(OUT_DIR1)\*.obj *.pdb

$(OUT_DIR) :
               if not exist $(OUT_DIR)\.   mkdir $(OUT_DIR)

$(OUT_DIR1) :
               if not exist $(OUT_DIR1)\.  mkdir $(OUT_DIR1)

$(OUT_DIR2) :
               if not exist $(OUT_DIR2)\.  mkdir $(OUT_DIR2)


$(OUT_DIR1)\$(LIBNAME1).lib:  $(OUT_DIR1) $(OUT_DIR) $(OBJS) $(C_OBJS) $(DIR1_OBJ)
               $(AR) -out:$(OUT_DIR1)\$(LIBNAME1).lib $(OUT_DIR)\*.obj $(OUT_DIR1)\*.obj

$(OUT_DIR2)\$(LIBNAME2).lib:  $(OUT_DIR2) $(OUT_DIR) $(OBJS) $(C_OBJS) $(DIR2_OBJ)
               $(AR) -out:$(OUT_DIR2)\$(LIBNAME2).lib $(OUT_DIR)\*.obj $(OUT_DIR2)\*.obj

.c{$(OUT_DIR)}.obj:
               $(COMP) $(CFLAGS) -V"@(#)PTOCLIB $< $(CFLAGS)" $< -Fo$*.obj

.c{$(OUT_DIR1)}.obj:
               $(COMP) $(CFLAGS1) -V"@(#)PTOCLIB $< $(CFLAGS1)" $< -Fo$*.obj

.c{$(OUT_DIR2)}.obj:
               $(COMP) $(CFLAGS2) -V"@(#)PTOCLIB $< $(CFLAGS2)" $< -Fo$*.obj

