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

IFTools_Console.h inc=\
        :SAPDB/Interfaces/sapdbifr.h,\
        SQLDBC_ClientRuntime.h,\
        RTESys_IO.h

IFTools_Console.cpp inc=IFTools_Console.h,\
    RTEMem_RteAllocator.hpp,\
    SAPDBMem_Alloca.h,\
    RTE_IniFileHandling.h,\
    IFRUtil_Configuration.h,\
    :SAPDB/RunTime/InputOutput/RTEIO_DirectoryIterator.hpp,\
    IFRUtil_Vector.h


generrmsg.pl ->$OWN/tool/bin/ ascii setbuildinfo

setenv.sh ->$OWN/test/sqldbc/ ascii exec

IFTools_sqlcliMain.cpp       inc=IFTools_sqlcliDBAccess.h,IFTools_sqlcliInput.h setbuildinfo

IFTools_sqlcliProperties.h   inc=\
        :SAPDB/Interfaces/SQLDBC/SQLDBC_Types.h,\
        :SAPDB/Interfaces/SQLDBC/SQLDBC.h,\
        :SAPDB/Interfaces/SQLDBC/SQLDBC_ClientRuntime.h,\
        geo81.h 

IFTools_sqlcliProperties.cpp inc=IFTools_sqlcliProperties.h setbuildinfo

IFTools_sqlcliDBAccess.h     inc=IFTools_sqlcliProperties.h,IFTools_sqlcliOutput.h,IFTools_sqlcliInput.h
IFTools_sqlcliDBAccess.cpp   inc=IFTools_sqlcliDBAccess.h

IFTools_sqlcliOutput.h       inc=IFTools_sqlcliProperties.h
IFTools_sqlcliOutput.cpp     inc=IFTools_sqlcliOutput.h

IFTools_sqlcliInput.h        inc=IFTools_sqlcliProperties.h
IFTools_sqlcliInput.cpp      inc=IFTools_sqlcliInput.h,heo06.h  setbuildinfo

IFTools_Utils.cpp inc=IFTools_Utils.h,\
    RTEMem_Allocator.hpp,\
    Container_Vector.hpp,\
    hsp77.h,\
    hsp78_0.h,\
    hsp81.h

IFTools_Utils.h   inc=:SAPDB/Interfaces/SQLDBC/SQLDBC.h,\
    Container_Vector.hpp

IFTools_corrnumcol.cpp inc=IFTools_Utils.h,IFRUtil_Time.h -except
IFTools_corrlongcol.cpp inc=IFTools_Utils.h,IFRUtil_Time.h -except

sapsystems.pl ->$OWN/tool/bin/ ascii
