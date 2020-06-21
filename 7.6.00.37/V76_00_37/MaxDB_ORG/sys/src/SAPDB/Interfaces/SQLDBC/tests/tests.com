# -----------------------------------------------------------------------------
#
# PURPOSE
# Handling Description for SAP Database Connectivty Library
#
# RELEASE:  SAP DB 8.0
#
# -----------------------------------------------------------------------------
#
#
#    ========== licence begin  GPL
#    Copyright (c) 2000-2005 SAP AG
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

* inc=:SAPDB/Interfaces/SQLDBC/tests/SQLDBCTestUtils.h

&ifdebug
* -DCATCH_ALL
&endif

&if $OS = WIN32
*-d -MD
&endif

&if $MACH in [ SUN ] || $OSSPEC == SOLARIS
regex(\w+\.cpp) -features=tmplife
&endif

SQLDBCTestUtils.h inc=\
        :SAPDB/Interfaces/SQLDBC/SQLDBC.h,\
        :SAPDB/Container/Container_Vector.hpp,\
        :SAPDB/Interfaces/SQLDBC/tests/SQLDBCTestMain.h


SQLDBCTestUtils*.cpp inc=\
        SQLDBCTestUtils.h,\
        :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp,\
        :SAPDB/Container/Container_Vector.hpp,\
        hsp77.h,\
        hsp81.h,\
        hsp78_0.h

#        SAPDB_ReuseLib.h


SQLDBCTestMain.h inc=

allsqldbc.h inc=+SAPDB_sprintf.h
allsqldbc.cpp inc=+SAPDB_sprintf.h

convertUCS2.cpp inc=+SAPDB_sprintf.h

unterm.cpp inc=+SAPDB_sprintf.h

piecewise.cpp inc=+SAPDB_sprintf.h

allrowset.cpp inc=+SAPDBMem_Alloca.h

blobsingleinsert.cpp inc=+SAPDBMem_Alloca.h

proxyconnect.cpp inc=:SAPDB/Interfaces/SQLDBC/SQLDBC.h,\
    :SAPDB/Interfaces/SQLDBC/SQLDBC_Proxy.h

clienttrace.cpp inc=+:SAPDB/Interfaces/SQLDBC/SQLDBC_ClientTrace.h

datatrunc.cpp inc=+hsp78_0.h

oomtest.cpp inc=+SQLDBC_IRuntime.h,SAPDBMem_IRawAllocator.hpp,:SAPDB/Interfaces/SQLDBC/tests/allsqldbc.h

blobsingleinsert.cpp inc=+SAPDBMem_Alloca.h

errorCheck.cpp inc=+SAPDB_sprintf.h,IFR_ErrorHndl.h

canceltest.cpp inc=+heo07.h,+heo02.h

allsqldbc_c.c inc=:SAPDB/Interfaces/SQLDBC/SQLDBC-sqldbc_c.h,\
                  :SAPDB/Interfaces/SQLDBC/SQLDBC_Types.h

paramtypes.cpp inc=+hsp81.h\
        ,:SAPDB/Interfaces/ODBC/sqldbc_sqltypes.h,hsp77.h

streamtest.cpp inc=+:precom/lcstream.h
lcsqldbctest.cpp inc=+:SAPDB/Interfaces/tests/PacketSpy.h \
           dep=OMS_LiveCacheTypes.xml

tp2sim.cpp -I$OWN/usr/sdk/sqldbc/incl inc=

mutruntime.cpp inc=+:SAPDB/Interfaces/SQLDBC/SQLDBC_ClientRuntime.h,\
                     :SAPDB/Interfaces/Runtime/IFR_ConnectionItem.h,\
                     :SAPDB/Interfaces/Runtime/IFR_Connection.h
                     
closehandlesonexit.cpp inc=+SQLDBC_IRuntime.h,SAPDBMem_IRawAllocator.hpp

typestest.cpp inc=+RTESys_MSWindows.h

#dummy.cpp inc=:SAPDB/Interfaces/Runtime/IFR_Environment.h

testbincompat.cpp -DVMAKE <-$WRK/src/SAPDB/Interfaces/SQLDBC/tests/ \
	inc=:SAPDB/Interfaces/SQLDBC/SQLDBC.h \
	dep=:SAPDB/Interfaces/SQLDBC/SQLDBC-testbincompat.h

#----------------------------------------------------------------------
SQLDBCTest_Storage.hpp inc=:SAPDB/Interfaces/SQLDBC/SQLDBC.h,\
	:SAPDB/Interfaces/SQLDBC/tests/SQLDBCTestUtils.h

# Force remake to always get current version, not only in scratch make.
SQLDBCTest_Storage.cpp inc=:SAPDB/Interfaces/SQLDBC/tests/SQLDBCTest_Storage.hpp,\
	hsp77.h,\
	RTEMem_Allocator.hpp,\
	:SAPDB/Interfaces/SQLDBC/tests/SQLDBCTest_Profile.hpp \
	setbuildinfo dep=:config/Buildinfo/VERSIONS

SQLDBCTest_Profile.hpp inc=:SAPDB/Interfaces/SQLDBC/SQLDBC.h,\
	Container_Vector.hpp

SQLDBCTest_Profile.cpp inc=:SAPDB/Interfaces/SQLDBC/tests/SQLDBCTest_Profile.hpp,\
	RTEMem_Allocator.hpp,\
	:SAPDB/Interfaces/SQLDBC/tests/SQLDBCTestUtils.h,\
  :buildtools/extern/expat/expat.h

#----------------------------------------------------------------------
&if $OS = WIN32
bincompat.mk NMAKE ascii concom=yes ->$OWN/test/sqldbc/bincompat.mk
&else
bincompat.mk ascii concom=yes ->$OWN/test/sqldbc/bincompat.mk
&endif

# Do not remove this marker, the test generator depends on it!
#!testbincompat
testbincompat_102723.cpp ascii ->$(OWN)/test/sqldbc/testbincompat_102723.cpp 
testbincompat_102710.cpp ascii ->$(OWN)/test/sqldbc/testbincompat_102710.cpp 
testbincompat_98781.cpp ascii ->$(OWN)/test/sqldbc/testbincompat_98781.cpp 
testbincompat_98304.cpp ascii ->$(OWN)/test/sqldbc/testbincompat_98304.cpp 
testbincompat_92146.cpp ascii ->$(OWN)/test/sqldbc/testbincompat_92146.cpp 
testbincompat_92742.cpp ascii ->$(OWN)/test/sqldbc/testbincompat_92742.cpp 
testbincompat_92168.cpp ascii ->$(OWN)/test/sqldbc/testbincompat_92168.cpp 
testbincompat_89660.cpp ascii ->$(OWN)/test/sqldbc/testbincompat_89660.cpp 

