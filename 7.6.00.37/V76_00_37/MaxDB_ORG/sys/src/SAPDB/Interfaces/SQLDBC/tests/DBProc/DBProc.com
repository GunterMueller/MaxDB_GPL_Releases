# -----------------------------------------------------------------------------
#
# PURPOSE
# Handling Description for files related to the StreamTest tests
#
# RELEASE:  SAP DB 7.6
#
# CHANGES
# 04/3/2004 FF:  First time CheckIn

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

# MS 7.11.03
*.cpp -DLIVECACHE_INTGUIDS

*x -DIMF
SQLDBCPROC.exp ascii unpack=no
SQLDBCPROCObj.h  dep=OMS_ClientInterface.hpp,OMS_DbpGlobals.hpp,SQL_ClientInterface.hpp

SQLDBCPROC.xml lang=r dep=SQLDBCPROCObj.h obj=SQLDBCPROCIspc.o \
               exdep=$WRK/incl/SQLDBCPROC.h,$WRK/incl/SQLDBCPROC_i.c \
               unpack=no noversion \
               -xml2ispc -notdc
&if ($OSSPEC!=WIN32 or defined BIT64) and not ($MACH == SUN || $OSSPEC == SOLARIS)
SQLDBCPROC.xml -novalidate
&endif

SQLDBCPROCMain.cpp -except dep=SQLDBCPROC.xml	inc=:SAPDB/Interfaces/SQLDBC/tests/DBProc/SQLDBCPROCObj.h

SQLDBCPROCObj.cpp -except dep=SQLDBCPROC.xml	inc=:SAPDB/Interfaces/SQLDBC/tests/DBProc/SQLDBCPROCObj.h


streamtest.xml lang=r dep=:SAPDB/Interfaces/SQLDBC/tests/DBProc/streamtestObj.h obj=streamtestIspc.o \
               exdep=$WRK/incl/streamtest.h,$WRK/incl/streamtest_i.c \
               unpack=no noversion \
               -xml2ispc -notdc
&if ($OSSPEC!=WIN32 or defined BIT64) and not ($MACH == SUN || $OSSPEC == SOLARIS)
streamtest.xml -novalidate
&endif

streamtestMain.cpp -except dep=streamtest.xml	inc=:SAPDB/Interfaces/SQLDBC/tests/DBProc/streamtestObj.h
streamtestObj.cpp  -except dep=streamtest.xml	inc=:SAPDB/Interfaces/SQLDBC/tests/DBProc/streamtestObj.h

streamtestObj.h
