# -----------------------------------------------------------------------------
# 
# PURPOSE
# Compiling ODBC
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

#* -I$OWN/sys/src/SAPDB/Interfaces/ODBC -DBUILD_INTERFACE_RUNTIME

* -I$OWN/sys/wrk/sdk/incl -I$OWN/sys/src/SAPDB/Interfaces/ODBC -DBUILD_INTERFACE_RUNTIME

sqldbc_sql.h        inc=:SAPDB/Interfaces/ODBC/sqldbc_sqltypes.h
sqldbc_sqlext.h     inc=:SAPDB/Interfaces/ODBC/sqldbc_sql.h,:SAPDB/Interfaces/ODBC/sqldbc_sqlucode.h

ODBC.h                inc=:SAPDB/Interfaces/SQLDBC/SQLDBC.h,sqldbc_sql.h,sqldbc_sqlext.h,sqldbc_sqltypes.h,ODBC_Util.h

ODBC_Environment.h    inc=ODBC.h,ODBC_Diag.h,sqldbc_sqlext.h,:Interfaces/SQLDBC/SQLDBC_IRuntime.h,SAPDB/Interfaces/SQLDBC/SQLDBC_ClientRuntime.h
ODBC_Environment.cpp  inc=ODBC_Environment.h

ODBC_Connection.h   inc=ODBC_Environment.h,ODBC.h
ODBC_Connection.cpp inc=gsp100.h,:SAPDB/RunTime/RTE_IniFileHandling.h,:SAPDB/SAPDBCommon/SAPDB_Names.h,ODBC_Connection.h

ODBC_Statement.h    inc=sqldbc_sql.h,sqldbc_sqlext.h,ODBC.h,ODBC_Diag.h,ODBC_Connection.h,ODBC_Statement.h
ODBC_Statement.cpp  inc=hsp81.h,ODBC_Connection.h,ODBC_Statement.h

ODBC_Handle.h       inc=sqldbc_sql.h,ODBC.h
ODBC_Handle.cpp     inc=ODBC_Handle.h,ODBC_Environment.h,ODBC_Connection.h,ODBC_Statement.h

ODBC_Diag.h         inc=sqldbc_sqltypes.h,:SAPDB/Interfaces/Runtime/Util/IFRUtil_New.h,:SAPDB/Interfaces/Runtime/Util/IFRUtil_Vector.h,:SAPDB/Interfaces/SQLDBC/SQLDBC.h,:SAPDB/Interfaces/ODBC/ODBC.h
ODBC_Diag.cpp       inc=ODBC_Diag.h,ODBC_Statement.h,:Interfaces/SQLDBC/SQLDBC_IRuntime.h,SAPDB/Interfaces/SQLDBC/SQLDBC_ClientRuntime.h

ODBC_Catalog.cpp    inc=:SAPDB/Interfaces/ODBC/ODBC.h,:SAPDB/Interfaces/ODBC/ODBC_Statement.h,:SAPDB/Interfaces/SQLDBC/SQLDBC_Catalog.h

ODBC_Util.h         inc=hsp77.h,hsp78_0.h,:SAPDB/Interfaces/SQLDBC/SQLDBC.h
ODBC_Util.cpp       inc=ODBC.h,ODBC_Util.h
