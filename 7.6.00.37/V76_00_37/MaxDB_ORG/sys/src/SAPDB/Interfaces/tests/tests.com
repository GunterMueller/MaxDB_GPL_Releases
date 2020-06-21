# -----------------------------------------------------------------------------
# 
# PURPOSE
# Handling Description for Interface tests
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

* -I$OWN/sys/wrk/sdk/incl
* inc=:SAPDB/Interfaces/sapdbifr.h,TestUtils.h
&ifdebug
* -DCATCH_ALL
&endif

&if $MACH in [ SUN ] || $OSSPEC == SOLARIS
regex(\w+\.cpp) -features=tmplife
&endif

runtests.pl ascii exec ->$INSTROOT/pgm/

CrashMe.h inc=
CrashMe.cpp inc=\
        CrashMe.h,\
        :SAPDBCommon/SAPDB_ReuseLib.h,\
        IFR_Datatypes.h,\
        IFR_PreparedStmt.h,\
        IFR_Connection.h

ReuseCon.cpp inc=\
	IFR_Environment.h,\
	IFR_Connection.h,\
	:SAPDB/Interfaces/SQLDBC/SQLDBC_ClientRuntime.h

mttest.cpp inc=+heo07.h

FailedStmt.cpp inc=+CrashMe.h,+PacketSpy.h

FailedPreparedStmt.cpp inc=+CrashMe.h

ReuseTrace.cpp inc=:SAPDBCommon/SAPDB_ReuseLib.h

TraceDemo.c inc=:SAPDB/Interfaces/IFR_dbug.h -DDBUG

Resultset.cpp inc=+Util.h

PacketSpy.h inc=\
        :SAPDBCommon/SAPDB_ReuseLib.h,\
        :SAPDB/Interfaces/SQLDBC/SQLDBC_ClientRuntime.h,\
        IFR_String.h

PacketSpy.cpp inc=\
        :SAPDB/Interfaces/tests/PacketSpy.h,\
        :SAPDB/RunTime/MemoryManagement/RTEMem_RteAllocator.hpp

pspytest.cpp inc=\
        +CrashMe.h,\
        +PacketSpy.h


Util.h inc=IFR_ErrorHndl.h
Util.cpp inc=\
        Util.h,\
        :SAPDBCommon/SAPDB_ReuseLib.h

TestUtils.h inc=:SAPDB/Interfaces/sapdbifr.h,Util.h,\
        :SAPDB/Interfaces/tests/TestMain.h,\
        :SAPDB/Interfaces/SQLDBC/SQLDBC_ClientRuntime.h

TestUtils.cpp inc=\
        TestUtils.h,\
        :SAPDBCommon/SAPDB_ReuseLib.h,\
        RTEMem_RawAllocator.hpp

TestMain.h inc=

FailedFetchTypes.cpp inc=+CrashMe.h

applparamtest.cpp

dataexecute.cpp 

&if OSSPEC == OSF1
typestest.cpp    -ieee
&endif


