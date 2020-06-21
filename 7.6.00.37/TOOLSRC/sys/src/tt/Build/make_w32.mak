#   make_w32.mak
#

RM = del
MV = move


!IF "$(RELVER)" == "R72"
#CPCLIBS   = sqlcamt.lib libpcrs.lib (CTS 1109222)
!ERROR use 7.1.04 to create tools 
!ELSEIF "$(RELVER)" == "R71" 
# create pcr71smt-notcp.lib before (CTS 1109222)
CPCLIBS   = sqlcamt.lib pcr62smt-notcp.lib
!ELSE
!ERROR use 7.1.04 to create tools
!ENDIF

WIN32LIBS = kernel32.lib advapi32.lib gdi32.lib user32.lib wsock32.lib

CPCLNK = link.exe
CPCLIBPATH = /LIBPATH:$(INSTROOT)\lib /LIBPATH:$(WRK)\fast\obj 
CPCLNKFLAG = $(CPCLIBPATH) /nodefaultlib:libcmt largeint.lib $(WIN32LIBS) $(CPCLIBS)

!IFDEF DEBUG
CFLAGS = $(CFLAGS) /DDEBUG /Od
CPCLNKFLAG = $(CPCLNKFLAG) /debug
!ENDIF

.SUFFIXES : .cpc .c .h .obj .lib

.cpc.c :
     cpc.exe -S oracle -H nocheck -c $(<B)

.c.obj :
     $(CC) $(CFLAGS) -c $<

all :   GetBuildNr.exe \
        PutBuildPrt.exe \
        GetBuildPrt.exe \
        RegIPut.exe

install :   $(TOOL)\pgm\GetBuildNr.exe \
            $(TOOL)\pgm\PutBuildPrt.exe \
            $(TOOL)\pgm\GetBuildPrt.exe \
            $(TOOL)\pgm\RegIPut.exe 

clean :
    @-$(RM) *.obj *.prot *.pdb *.pct *.pcl GetBuildNr.exe PutBuildPrt.exe GetBuildPrt.exe RegIPut.exe \
    GetBuildNr.c PutBuildPrt.c GetBuildPrt.c RegIPut.c > nul 2>&1

AHuff.c:  $(@B).h
    
BitEA.c:  $(@B).h

GetBuildNr.c : $(@B).cpc

PutBuildPrt.c : $(@B).cpc AHuff.h BitEA.h

GetBuildPrt.c : $(@B).cpc AHuff.h BitEA.h

RegIPut.c : $(@B).cpc

AHuff.obj : AHuff.h BitEA.h AHuff.c

BitEA.obj :        BitEA.h BitEA.c

PutBuildPrt.obj :  AHuff.obj BitEA.obj

GetBuildPrt.obj :  AHuff.obj BitEA.obj

GetBuildNr.exe : GetBuildNr.obj
    $(CPCLNK) $(CPCLNKFLAG) -out:$@ $(**B) 

PutBuildPrt.exe : PutBuildPrt.obj AHuff.obj BitEA.obj
    $(CPCLNK) $(CPCLNKFLAG) -out:$@ $(**B) 

GetBuildPrt.exe : GetBuildPrt.obj AHuff.obj BitEA.obj
    $(CPCLNK) $(CPCLNKFLAG) -out:$@ $(**B)

RegIPut.exe : RegIPut.obj
    $(CPCLNK) $(CPCLNKFLAG) -out:$@ $(**B)

$(TOOL)\pgm\GetBuildNr.exe : GetBuildNr.obj
    $(CPCLNK) $(CPCLNKFLAG) -out:$(@F) $(**B)
    @-$(RM) $(TOOL)\pgm\$(@B).old > nul 2>&1
    @-$(MV) $(TOOL)\pgm\$(@F) $(TOOL)\pgm\$(@B).old > nul 2>&1
    $(MV) $(@F) $(TOOL)\pgm\.
    @echo : ------------------------------------------------------------ :

$(TOOL)\pgm\PutBuildPrt.exe : PutBuildPrt.obj AHuff.obj BitEA.obj
    $(CPCLNK) $(CPCLNKFLAG) -out:$(@F) $(**B)
    @-$(RM) $(TOOL)\pgm\$(@B).old > nul 2>&1
    @-$(MV) $(TOOL)\pgm\$(@F) $(TOOL)\pgm\$(@B).old > nul 2>&1
    $(MV) $(@F) $(TOOL)\pgm\.
    @echo : ------------------------------------------------------------ :

$(TOOL)\pgm\GetBuildPrt.exe : GetBuildPrt.obj AHuff.obj BitEA.obj
    $(CPCLNK) $(CPCLNKFLAG) -out:$(@F) $(**B)
    @-$(RM) $(TOOL)\pgm\$(@B).old > nul 2>&1
    @-$(MV) $(TOOL)\pgm\$(@F) $(TOOL)\pgm\$(@B).old > nul 2>&1
    $(MV) $(@F) $(TOOL)\pgm\.
    @echo : ------------------------------------------------------------ :

$(TOOL)\pgm\RegIPut.exe : RegIPut.obj
    $(CPCLNK) $(CPCLNKFLAG) -out:$(@F) $(**B)
    @-$(RM) $(TOOL)\pgm\$(@B).old > nul 2>&1
    @-$(MV) $(TOOL)\pgm\$(@F) $(TOOL)\pgm\$(@B).old > nul 2>&1
    $(MV) $(@F) $(TOOL)\pgm\.
    @echo : ------------------------------------------------------------ :

