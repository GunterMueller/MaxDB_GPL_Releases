#
# file   CppSQL.com
# author PeterG (GoertzP)
#
#
#    ========== licence begin  GPL
#    Copyright (c) 2003-2005 SAP AG
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

&ifdebug
*.c -DOMS_DEBUG -DOMS_IMPLEMENTATION -DSQLCLS_IMPL
*.cpp -DOMS_DEBUG -except -DOMS_IMPLEMENTATION -DSQLCLS_IMPL
&else
*.cpp -except -DOMS_IMPLEMENTATION -DSQLCLS_IMPL
&endif


SQL_.hpp                   unpack=no concom=yes \
                           inc= \
                           ->$INSTROOT/incl/CppSQL/SQL_.hpp
SQL_.cpp                   inc=geo00.h,OMS_Defines.h,LVC_IliveCacheSink.hpp,\
                           SQL_Handle.hpp,SQL_.hpp,SQL_SessionContext.hpp,\
                           SQL_Statement.hpp,SQL_Column.hpp \
                           -MD
SQL_-t.cpp                 -DOMSTST \
                           inc=geo00.h,OMS_Defines.h,LVC_IliveCacheSink.hpp,\
                           SQL_Handle.hpp,SQL_.hpp,SQL_SessionContext.hpp,\
                           SQL_Statement.hpp,SQL_Column.hpp \
                           -MD
SQL_-m.cpp                 -DEXT_CLIENT_SQL \
                           inc=geo00.h,OMS_Defines.h,LVC_IliveCacheSink.hpp,\
                           SQL_Handle.hpp,SQL_.hpp,SQL_SessionContext.hpp,\
                           SQL_Statement.hpp,SQL_Column.hpp

SQL_ClientInterface.hpp    inc=SQL_Option.hpp,SQL_Column.hpp,SQL_.hpp,\
                           SQL_Handle.hpp \
                           ->$INSTROOT/incl/livecachesql.h

SQL_Column.hpp             unpack=no concom=yes \
                           inc=OMS_Defines.h \
                           ->$INSTROOT/incl/CppSQL/SQL_Column.hpp
SQL_Column.cpp             inc=geo00.h,SQL_Column.hpp,SQL_ColumnDesc.hpp,\
                           SQL_SessionContext.hpp,\
                           ggg00,ggg01,OMS_DbpBase.hpp,OMS_Globals.hpp,\
                           LVC_IliveCacheSink.hpp,LVC_KernelInterface.hpp,\
                           SAPDBMem_IBlockAllocator.hpp,\
                           SAPDBMem_RawAllocator.hpp \
                           -MD
SQL_Column-m.cpp           -DEXT_CLIENT_SQL \
                           inc=geo00.h,SQL_Column.hpp,SQL_ColumnDesc.hpp,\
                           SQL_SessionContext.hpp,\
                           ggg00,ggg01,OMS_DbpBase.hpp,OMS_Globals.hpp,\
                           LVC_IliveCacheSink.hpp,LVC_KernelInterface.hpp,\
                           SAPDBMem_IBlockAllocator.hpp,\
                           SAPDBMem_RawAllocator.hpp \
                           -MD
SQL_Column-t.cpp           -DOMSTST \
                           inc=geo00.h,SQL_Column.hpp,SQL_ColumnDesc.hpp,\
                           SQL_SessionContext.hpp,\
                           ggg00,ggg01,OMS_DbpBase.hpp,OMS_Globals.hpp,\
                           LVC_IliveCacheSink.hpp,LVC_KernelInterface.hpp,\
                           SAPDBMem_IBlockAllocator.hpp,\
                           SAPDBMem_RawAllocator.hpp \
                           -MD

SQL_ColumnDesc.hpp         inc=vsp001
SQL_ColumnDesc.cpp         dep=OMS_LiveCacheTypes.xml \
                           inc=geo00.h,SQL_Column.hpp,SQL_ColumnDesc.hpp,\
                           SQL_SessionContext.hpp,\
                           :SAPDB/PacketInterface/PIn_Part.h,\
                           SQL_Statement.hpp,hsp40.h,hsp41.h,hsp81.h,hsp77.h,\
                           ggg00,OMS_DbpBase.hpp,OMS_Globals.hpp,\
                           LVC_KernelInterface.hpp,\
                           SAPDBMem_IBlockAllocator.hpp,\
                           SAPDBMem_RawAllocator.hpp,\
                           SAPDB_MemCopyMove.hpp,\
                           :SAPDB/Interfaces/Runtime/Util/IFRUtil_VDNNumber.h \
                           -MD
SQL_ColumnDesc-m.cpp       -DEXT_CLIENT_SQL \
                           dep=OMS_LiveCacheTypes.xml \
                           inc=geo00.h,SQL_Column.hpp,SQL_ColumnDesc.hpp,\
                           SQL_SessionContext.hpp,\
                           :SAPDB/PacketInterface/PIn_Part.h,\
                           SQL_Statement.hpp,hsp40.h,hsp41.h,hsp81.h,hsp77.h,\
                           ggg00,OMS_DbpBase.hpp,OMS_Globals.hpp,\
                           LVC_KernelInterface.hpp,\
                           SAPDBMem_IBlockAllocator.hpp,\
                           SAPDBMem_RawAllocator.hpp,\
                           SAPDB_MemCopyMove.hpp,\
                           :SAPDB/Interfaces/Runtime/Util/IFRUtil_VDNNumber.h \
                           -MD
SQL_ColumnDesc-t.cpp       -DOMSTST \
                           dep=OMS_LiveCacheTypes.xml \
                           inc=geo00.h,SQL_Column.hpp,SQL_ColumnDesc.hpp,\
                           SQL_SessionContext.hpp,\
                           :SAPDB/PacketInterface/PIn_Part.h,\
                           SQL_Statement.hpp,hsp40.h,hsp41.h,hsp81.h,hsp77.h,\
                           ggg00,OMS_DbpBase.hpp,OMS_Globals.hpp,\
                           LVC_KernelInterface.hpp,\
                           SAPDBMem_IBlockAllocator.hpp,\
                           SAPDBMem_RawAllocator.hpp,\
                           SAPDB_MemCopyMove.hpp,\
                           :SAPDB/Interfaces/Runtime/Util/IFRUtil_VDNNumber.h \
                           -MD

SQL_SQLDBCConnection.hpp   output=$INSTROOT/incl/livecachesqldbc.h \
                           dep=:SAPDB/Interfaces/SQLDBC/SQLDBC.h

SQL_SQLDBCConnection.cpp   inc=geo00.h,\
                           ggg01,\
                           OMS_Defines.h,\
                           OMS_Globals.hpp,\
                           OMS_DbpError.hpp,\
                           OMS_GlobalAllocatorWrapper.hpp,\
                           SQL_SQLDBCConnection.hpp,\
                           SQL_SQLDBCRuntime.hpp,\
                           LVC_IliveCacheSink.hpp,\
                           LVC_KernelInterface.hpp


SQL_ErrorHandler.hpp       inc=ggg00
SQL_ErrorHandler.cpp       inc=SQL_ErrorHandler.hpp,SQL_SessionContext.hpp \
                           -MD
SQL_ErrorHandler-m.cpp     -DEXT_CLIENT_SQL \
                           inc=SQL_ErrorHandler.hpp,SQL_SessionContext.hpp \
                           -MD

SQL_ExtClientInterface.hpp dep=SQL_ClientInterface.hpp \
                           inc=SQL_Session.hpp \
                           ->$INSTROOT/incl/sqlcppclass.h

SQL_Handle.hpp             unpack=no concom=yes \
                           inc=OMS_DbpBase.hpp,SQL_Option.hpp \
                           ->$INSTROOT/incl/CppSQL/SQL_Handle.hpp
SQL_Handle.cpp             inc=SQL_Handle.hpp,SQL_.hpp,\
                           SQL_SessionContext.hpp,hsp81.h \
                           -MD
SQL_Handle-m.cpp           -DEXT_CLIENT_SQL \
                           inc=SQL_Handle.hpp,SQL_.hpp,\
                           SQL_SessionContext.hpp,hsp81.h

SQL_Monitor.hpp            inc=gsp92
SQL_Monitor.cpp            inc=SQL_Monitor.hpp

SQL_Option.hpp             inc= \
                           ->$INSTROOT/incl/CppSQL/SQL_Option.hpp

SQL_ReplyParser.hpp        inc=:SAPDB/PacketInterface/PIn_ReplyParser.h
SQL_ReplyParser.cpp        inc=SQL_ReplyParser.hpp,SQL_SessionContext.hpp,\
                           SQL_Statement.hpp \
                           -MD
SQL_ReplyParser-m.cpp      -DEXT_CLIENT_SQL \
                           inc=SQL_ReplyParser.hpp,SQL_SessionContext.hpp,\
                           SQL_Statement.hpp

SQL_SQLDBCRuntime.hpp      inc=:SAPDB/Interfaces/SQLDBC/SQLDBC_IRuntime.h

SQL_SQLDBCRuntime.cpp      inc=SAPDBMem_NewDestroy.hpp,\
                           SQL_SQLDBCRuntime.hpp,\
                           LVC_IliveCacheSink.hpp,\
                           SAPDB_MemCopyMove.hpp

SQL_Session.hpp            inc=SQL_Handle.hpp \
                           ->$INSTROOT/incl/CppSQL/SQL_Session.hpp
SQL_Session-m.cpp          -DEXT_CLIENT_SQL \
                           inc=heo02.h,heo03.h,hsp02.h,OMS_Defines.h,\
                           SQL_Session.hpp,SQL_.hpp,SQL_SessionContext.hpp

SQL_SessionContext.hpp     inc=vsp001,ggg00,OMS_Defines.h,\
                           LVC_IliveCacheSink.hpp,ggg251.h,SQL_Option.hpp,\
                           SQL_ErrorHandler.hpp,hsp81.h,OMS_Globals.hpp,\
                           LVC_KernelInterface.hpp,\
                           SAPDBMem_IBlockAllocator.hpp,\
                           SAPDBMem_RawAllocator.hpp,SQL_Monitor.hpp,\
                           SQL_Handle.hpp,:SAPDB/PacketInterface/PIn.h
SQL_SessionContext.cpp     inc=geo00.h,OMS_Defines.h,LVC_IliveCacheSink.hpp,\
                           OMS_Globals.hpp,OMS_GlobalAllocatorWrapper.hpp,\
                           LVC_KernelInterface.hpp,\
                           SQL_.hpp,SQL_SessionContext.hpp,\
                           SQL_Statement.hpp,\
                           :SAPDB/PacketInterface/PIn_ReplyParser.h,\
                           :SAPDB/PacketInterface/PIn_RequestWriter.h,\
                           SQL_ReplyParser.hpp,gsp01.h,heo02.h,heo03.h,\
                           hsp02.h,hsp81.h,hsp100.h,SAPDB_MemCopyMove.hpp,hsp78_0.h \
                           -D_AFXDLL -MD
SQL_SessionContext-t.cpp   -DOMSTST \
                           inc=geo00.h,OMS_Defines.h,LVC_IliveCacheSink.hpp,\
                           OMS_Globals.hpp,OMS_GlobalAllocatorWrapper.hpp,\
                           LVC_KernelInterface.hpp,\
                           SQL_.hpp,SQL_SessionContext.hpp,\
                           SQL_Statement.hpp,\
                           :SAPDB/PacketInterface/PIn_ReplyParser.h,\
                           :SAPDB/PacketInterface/PIn_RequestWriter.h,\
                           SQL_ReplyParser.hpp,gsp01.h,heo02.h,heo03.h,\
                           hsp02.h,hsp81.h,hsp100.h,SAPDB_MemCopyMove.hpp,hsp78_0.h \
                           -D_AFXDLL -MD
SQL_SessionContext-m.cpp   -DEXT_CLIENT_SQL \
                           inc=geo00.h,OMS_Defines.h,LVC_IliveCacheSink.hpp,\
                           OMS_Globals.hpp,OMS_GlobalAllocatorWrapper.hpp,\
                           LVC_KernelInterface.hpp,\
                           SQL_.hpp,SQL_SessionContext.hpp,\
                           SQL_Statement.hpp,\
                           :SAPDB/PacketInterface/PIn_ReplyParser.h,\
                           :SAPDB/PacketInterface/PIn_RequestWriter.h,\
                           SQL_ReplyParser.hpp,gsp01.h,heo02.h,heo03.h,\
                           hsp02.h,hsp81.h,hsp100.h,SAPDB_MemCopyMove.hpp,hsp78_0.h \
                           -D_AFXDLL

SQL_Statement.hpp          inc=OMS_DbpBase.hpp,:SAPDB/PacketInterface/PIn.h,\
                           hsp81.h
SQL_Statement.cpp          inc=geo00.h,OMS_Defines.h,SQL_.hpp,\
                           SQL_Statement.hpp,SQL_SessionContext.hpp,\
                           SQL_Column.hpp,SQL_ColumnDesc.hpp,vsp001,\
                           :SAPDB/PacketInterface/PIn_Part.h,\
                           hsp81.h,hsp41.h,SAPDB_MemCopyMove.hpp,hsp77.h \
                           -MD
SQL_Statement-m.cpp        -DEXT_CLIENT_SQL \
                           inc=geo00.h,OMS_Defines.h,SQL_.hpp,\
                           SQL_Statement.hpp,SQL_SessionContext.hpp,\
                           SQL_Column.hpp,SQL_ColumnDesc.hpp,vsp001,\
                           :SAPDB/PacketInterface/PIn_Part.h,\
                           hsp81.h,hsp41.h,SAPDB_MemCopyMove.hpp,hsp77.h

SQL_Utilities.hpp          inc=
SQL_Utilities.cpp          inc=OMS_Defines.h,geo00.h,SQL_Utilities.hpp,\
                           hsp81.h,OMS_DbpError.hpp,hsp77.h,OMS_Oid.hpp \
                           -MD
SQL_Utilities-m.cpp        -DEXT_CLIENT_SQL \
                           dep=OMS_LiveCacheTypes.xml \
                           inc=OMS_Defines.h,geo00.h,SQL_Utilities.hpp,\
                           hsp81.h,OMS_DbpError.hpp,hsp77.h,OMS_Oid.hpp


