
#    ========== licence begin  GPL
#    Copyright (c) 2005 SAP AG
#
#    This program is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public License
#    as published by the Free Software Foundation; either version 2
#    of the License, or (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#    ========== licence end

&ifdef NMAKE
# Maxim L. (Intel): select Intel compiler if used for build
&if $COMPILER == ICC
CC = icl
&else
CC = cl
&endif

!IFDEF PROCESSOR_ARCHITEW6432
!IF "$(PROCESSOR_ARCHITEW6432)" == "AMD64"
MACHINE = AMD64
!ENDIF
!ENDIF

# Maxim L. (Intel): select Intel compiler if used for build
&if $COMPILER == ICC
LINK = xilink
&else
LINK = link
&endif

!IF "$(MACHINE)" == "AMD64"
LINK = $(LINK) bufferoverflowu.lib
!ENDIF

CPPLIBS = $(SQLDBCSDK)/sdk/sqldbc/lib/libSQLDBC.lib
CLIBS = $(SQLDBCSDK)/sdk/sqldbc/lib/libSQLDBC_C.lib

CPPFLAGS = -DWIN32 -I$(SQLDBCSDK)/sdk/sqldbc/incl
EXEEXT=.exe
EXEEXT=.obj
RM = del /f 
CPP_COMPILE_PREFIX = $(CC) /c $(CPPFLAGS)
CPP_COMPILE_SUFFIX = #
CPP_LINK_PREFIX = $(LINK) $(CPPLIBS)
CPP_LINK_SUFFIX = #

&else

CPPFLAGS = -I$(SQLDBCSDK)/sdk/sqldbc/incl
CPPLIBS     = -L$(SQLDBCSDK)/lib -lSQLDBC
CPPLIBS64   = -L$(SQLDBCSDK)/lib/lib64 -lSQLDBC
CLIBS     = -L$(SQLDBCSDK)/lib -lSQLDBC_C
CLIBS64   = -L$(SQLDBCSDK)/lib/lib64 -lSQLDBC_C

# Maxim L. (Intel): select Intel compiler if used for build
&if $COMPILER == ICC
CC = icc
CXX = icpc
&else
CC = cc
CXX = c++

&endif

EXEEXT=#

OBJEXT=.o

&if $MACH in [SUN] || $OSSPEC == SOLARIS
&  if $MACH in [SUN, SPARC ]
CPP_COMPILE_PREFIX = CC -c -xarch=v9 $(CPPFLAGS)
CPP_COMPILE_SUFFIX = #
CPP_LINK_PREFIX = CC -o $@ -xarch=v9 
CPP_LINK_SUFFIX = $(CPPLIBS64)
&  elif $MACH in [X86_64]
CPP_COMPILE_PREFIX = CC -c -xarch=amd64 $(CPPFLAGS)
CPP_COMPILE_SUFFIX = #
CPP_LINK_PREFIX = CC -o $@ -xarch=amd64 
CPP_LINK_SUFFIX = $(CPPLIBS64)
&  else
# 32-bit build...
CPP_COMPILE_PREFIX = CC -c $(CPPFLAGS)
CPP_COMPILE_SUFFIX = #
CPP_LINK_PREFIX = CC -o $@ 
CPP_LINK_SUFFIX = $(CPPLIBS)
&  endif
&endif

&if $OSSPEC = AIX
CPP_COMPILE_PREFIX = xlC -c -q64 $(CPPFLAGS)
CPP_COMPILE_SUFFIX = #
CPP_LINK_PREFIX = xlC_r -o $@ -q64 -brtl
CPP_LINK_SUFFIX = $(CPPLIBS64)
&endif

&if $OSSPEC = HPUX
&if $MACH = HP_IA64
CPP_COMPILE_PREFIX = aCC -c +DD64 $(CPPFLAGS)
CPP_COMPILE_SUFFIX = #
CPP_LINK_PREFIX = aCC -o $@ +DD64
CPP_LINK_SUFFIX = $(CPPLIBS) -lpthread
&else
CPP_COMPILE_PREFIX = aCC -c +DA2.0W +DS2.0 $(CPPFLAGS)
CPP_COMPILE_SUFFIX = #
CPP_LINK_PREFIX = aCC -o $@ +DA2.0W +DS2.0
CPP_LINK_SUFFIX = $(CPPLIBS64) 
&endif
&endif

&if $OSSPEC = OSF1
CPP_COMPILE_PREFIX = cxx -c $(CPPFLAGS)
CPP_COMPILE_SUFFIX = #
CPP_LINK_PREFIX = cxx -o $@ 
CPP_LINK_SUFFIX = $(CPPLIBS) 
&endif

&if $OSSPEC = LINUX
&if $MACH = SDBonPPC64
CPP_COMPILE_PREFIX = $(CXX) -c -m64 $(CPPFLAGS)
CPP_COMPILE_SUFFIX = #
CPP_LINK_PREFIX = $(CXX) -m64 -Wl,-melf64ppc -o $@ 
CPP_LINK_SUFFIX = $(CPPLIBS) 
&else
CPP_COMPILE_PREFIX = $(CXX) -c $(CPPFLAGS)
CPP_COMPILE_SUFFIX = #
CPP_LINK_PREFIX = $(CXX) -o $@ 
CPP_LINK_SUFFIX = $(CPPLIBS) 
&endif
&endif

RM = rm -f
&endif

#----------------------------------------------------------------------
# Automatic variables. Do not touch anything here.

#testbincompat-program
ALL_PROGRAMS =                 \
  testbincompat_89660$(EXEEXT) \
  testbincompat_92168$(EXEEXT) \
  testbincompat_92146$(EXEEXT) \
  testbincompat_98304$(EXEEXT) \
  testbincompat_98781$(EXEEXT) \
  testbincompat_102710$(EXEEXT) \
  testbincompat_102723$(EXEEXT) \
  testbincompat_92742$(EXEEXT) 

#testbincompat-object
TMP_ALL_OBJECTS =              \
  testbincompat_89660$(OBJEXT) \
  testbincompat_92168$(OBJEXT) \
  testbincompat_92146$(OBJEXT) \
  testbincompat_98304$(OBJEXT) \
  testbincompat_98781$(OBJEXT) \
  testbincompat_102710$(OBJEXT) \
  testbincompat_102723$(OBJEXT) \
  testbincompat_92742$(OBJEXT) 


testbincompat_89660$(OBJEXT) : testbincompat_89660.cpp
	 $(CPP_COMPILE_PREFIX) testbincompat_89660.cpp $(CPP_COMPILE_SUFFIX)

testbincompat_89660$(EXEEXT) : testbincompat_89660$(OBJEXT)
	$(CPP_LINK_PREFIX) testbincompat_89660$(OBJEXT) $(CPP_LINK_SUFFIX)

testbincompat_92168$(OBJEXT) : testbincompat_92168.cpp
	 $(CPP_COMPILE_PREFIX) testbincompat_92168.cpp $(CPP_COMPILE_SUFFIX)

testbincompat_92168$(EXEEXT) : testbincompat_92168$(OBJEXT)
	$(CPP_LINK_PREFIX) testbincompat_92168$(OBJEXT) $(CPP_LINK_SUFFIX)

testbincompat_92742$(OBJEXT) : testbincompat_92742.cpp
	 $(CPP_COMPILE_PREFIX) testbincompat_92742.cpp $(CPP_COMPILE_SUFFIX)

testbincompat_92742$(EXEEXT) : testbincompat_92742$(OBJEXT)
	$(CPP_LINK_PREFIX) testbincompat_92742$(OBJEXT) $(CPP_LINK_SUFFIX)

testbincompat_92146$(OBJEXT) : testbincompat_92146.cpp
	 $(CPP_COMPILE_PREFIX) testbincompat_92146.cpp $(CPP_COMPILE_SUFFIX)

testbincompat_92146$(EXEEXT) : testbincompat_92146$(OBJEXT)
	$(CPP_LINK_PREFIX) testbincompat_92146$(OBJEXT) $(CPP_LINK_SUFFIX)

testbincompat_98304$(OBJEXT) : testbincompat_98304.cpp
	 $(CPP_COMPILE_PREFIX) testbincompat_98304.cpp $(CPP_COMPILE_SUFFIX)

testbincompat_98304$(EXEEXT) : testbincompat_98304$(OBJEXT)
	$(CPP_LINK_PREFIX) testbincompat_98304$(OBJEXT) $(CPP_LINK_SUFFIX)

testbincompat_98781$(OBJEXT) : testbincompat_98781.cpp
	 $(CPP_COMPILE_PREFIX) testbincompat_98781.cpp $(CPP_COMPILE_SUFFIX)

testbincompat_98781$(EXEEXT) : testbincompat_98781$(OBJEXT)
	$(CPP_LINK_PREFIX) testbincompat_98781$(OBJEXT) $(CPP_LINK_SUFFIX)

testbincompat_102710$(OBJEXT) : testbincompat_102710.cpp
	 $(CPP_COMPILE_PREFIX) testbincompat_102710.cpp $(CPP_COMPILE_SUFFIX)

testbincompat_102710$(EXEEXT) : testbincompat_102710$(OBJEXT)
	$(CPP_LINK_PREFIX) testbincompat_102710$(OBJEXT) $(CPP_LINK_SUFFIX)

testbincompat_102723$(OBJEXT) : testbincompat_102723.cpp
	 $(CPP_COMPILE_PREFIX) testbincompat_102723.cpp $(CPP_COMPILE_SUFFIX)

testbincompat_102723$(EXEEXT) : testbincompat_102723$(OBJEXT)
	$(CPP_LINK_PREFIX) testbincompat_102723$(OBJEXT) $(CPP_LINK_SUFFIX)

#testbincompat-rule

#----------------------------------------------------------------------
# Rules section
all : $(ALL_PROGRAMS)

clean :
	-$(RM) $(ALL_PROGRAMS)
	-$(RM) $(ALL_OBJECTS)



