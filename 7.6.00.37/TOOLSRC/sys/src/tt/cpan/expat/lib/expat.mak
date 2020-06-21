# Microsoft Developer Studio Generated NMAKE File, Based on expat.dsp
!IF "$(CFG)" == ""
CFG=expat - Win32 Release
!MESSAGE No configuration specified. Defaulting to expat - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "expat - Win32 Release" && "$(CFG)" != "expat - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "expat.mak" CFG="expat - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "expat - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "expat - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "expat - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\libexpat.dll"


CLEAN :
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\xmlparse.obj"
	-@erase "$(INTDIR)\xmlrole.obj"
	-@erase "$(INTDIR)\xmltok.obj"
	-@erase "$(OUTDIR)\libexpat.dll"
	-@erase "$(OUTDIR)\libexpat.exp"
	-@erase "$(OUTDIR)\libexpat.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "COMPILED_FROM_DSP" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /Wp64 /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\expat.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:none /machine:AMD64 /def:".\libexpat.def" /out:"$(OUTDIR)\libexpat.dll" /implib:"$(OUTDIR)\libexpat.lib" /machine:AMD64 
LINK32_OBJS= \
	"$(INTDIR)\xmlparse.obj" \
	"$(INTDIR)\xmlrole.obj" \
	"$(INTDIR)\xmltok.obj"

"$(OUTDIR)\libexpat.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "expat - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\libexpat.dll" "$(OUTDIR)\expat.bsc"


CLEAN :
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\xmlparse.obj"
	-@erase "$(INTDIR)\xmlparse.sbr"
	-@erase "$(INTDIR)\xmlrole.obj"
	-@erase "$(INTDIR)\xmlrole.sbr"
	-@erase "$(INTDIR)\xmltok.obj"
	-@erase "$(INTDIR)\xmltok.sbr"
	-@erase "$(OUTDIR)\expat.bsc"
	-@erase "$(OUTDIR)\libexpat.dll"
	-@erase "$(OUTDIR)\libexpat.exp"
	-@erase "$(OUTDIR)\libexpat.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /GX /ZI /Od /D "_DEBUG" /D "COMPILED_FROM_DSP" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\expat.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\xmlparse.sbr" \
	"$(INTDIR)\xmlrole.sbr" \
	"$(INTDIR)\xmltok.sbr"

"$(OUTDIR)\expat.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:none /debug /machine:I386 /def:".\libexpat.def" /out:"$(OUTDIR)\libexpat.dll" /implib:"$(OUTDIR)\libexpat.lib" 
DEF_FILE= \
	".\libexpat.def"
LINK32_OBJS= \
	"$(INTDIR)\xmlparse.obj" \
	"$(INTDIR)\xmlrole.obj" \
	"$(INTDIR)\xmltok.obj"

"$(OUTDIR)\libexpat.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("expat.dep")
!INCLUDE "expat.dep"
!ELSE 
!MESSAGE Warning: cannot find "expat.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "expat - Win32 Release" || "$(CFG)" == "expat - Win32 Debug"
SOURCE=.\xmlparse.c

!IF  "$(CFG)" == "expat - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "COMPILED_FROM_DSP" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /Wp64 /c 

"$(INTDIR)\xmlparse.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "expat - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /GX- /ZI /Od /D "_DEBUG" /D "COMPILED_FROM_DSP" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\xmlparse.obj"	"$(INTDIR)\xmlparse.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\xmlrole.c

!IF  "$(CFG)" == "expat - Win32 Release"


"$(INTDIR)\xmlrole.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "expat - Win32 Debug"


"$(INTDIR)\xmlrole.obj"	"$(INTDIR)\xmlrole.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\xmltok.c

!IF  "$(CFG)" == "expat - Win32 Release"


"$(INTDIR)\xmltok.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "expat - Win32 Debug"


"$(INTDIR)\xmltok.obj"	"$(INTDIR)\xmltok.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 


!ENDIF 

