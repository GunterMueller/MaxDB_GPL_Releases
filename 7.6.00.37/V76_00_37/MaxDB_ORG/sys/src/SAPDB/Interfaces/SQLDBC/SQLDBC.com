# -----------------------------------------------------------------------------
#
# PURPOSE
# Handling Description for SAP Database Connectivity.
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

&if $OSSPEC in [ HPUX ]
* -I$OWN/sys/wrk/sdk/incl -DBUILD_INTERFACE_RUNTIME +W1039
&else
* -I$OWN/sys/wrk/sdk/incl -DBUILD_INTERFACE_RUNTIME
&endif
*-nomt -I$OWN/sys/wrk/sdk/incl -DBUILD_INTERFACE_RUNTIME -DIFR_SINGLE_THREADED
*-kernel -DSQLDBC_FOR_KERNEL -DBUILD_INTERFACE_RUNTIME

SQLDBCrc.rc
SQLDBC76rc.rc
SQLDBCrc-notrace.rc -DIFR_NO_TRACE
SQLDBC_Crc.rc
SQLDBC_Crc-notrace.rc -DIFR_NO_TRACE

SQLDBC.h output=$INSTROOT/sdk/sqldbc/incl/,\
                $WRK/incl/SAPDB/Interfaces/SQLDBC/,
                $INSTROOT/incl/SQLDBC/SQLDBC.h \
         inc= \
         dep=SQLDBC_Types.h,:config/Buildinfo/VERSIONS setbuildinfo \
         remake concom=yes

genSQLDBC_C.pl ascii concom=no noversion unpack=no exec ->$OWN/tool/bin/ suppress_uncond
genbincompattest.pl ascii concom=no noversion unpack=no exec ->$OWN/tool/bin/ suppress_uncond

SQLDBC-sqldbc_c.h  output=$INSTROOT/sdk/sqldbc/incl/SQLDBC_C.h,$WRK/incl/SAPDB/Interfaces/SQLDBC/SQLDBC_C.h \
                   inc= \
                   dep=SQLDBC_Types.h,SQLDBC.h,genSQLDBC_C.pl execscript=$OWN/tool/bin/genSQLDBC_C.pl \
                   setbuildinfo remake concom=yes SQLDBC_C \
                   exdep=$WRK/src/SAPDB/Interfaces/SQLDBC/SQLDBC_C.cpp

SQLDBC-testbincompat.h  dep=SQLDBC_Types.h,SQLDBC.h,genbincompattest.pl \
                   execscript=$OWN/tool/bin/genbincompattest.pl \
                   setbuildinfo remake concom=yes \
                   exdep=$WRK/src/SAPDB/Interfaces/SQLDBC/tests/testbincompat.cpp

SQLDBC_Types.h output=$INSTROOT/sdk/sqldbc/incl/,\
                      $WRK/incl/SAPDB/Interfaces/SQLDBC/,\
                      $INSTROOT/incl/SQLDBC/SQLDBC_Types.h \
               inc= \

SQLDBC_Proxy.h output=$INSTROOT/incl/,$WRK/incl/SAPDB/Interfaces/SQLDBC/ \
               inc= \

SQLDBC*.cpp -I$OWN/sys/wrk/sdk/incl -DBUILD_INTERFACE_RUNTIME \
        inc=SQLDBC.h,SQLDBC_IRuntime.h,\
        IFR_Common.h,\
        IFRUtil_LinkedList.h,\
        IFR_Environment.h,\
        IFR_Connection.h,\
        IFR_Statement.h,\
        IFR_PreparedStmt.h,\
        IFR_ResultSet.h,\
        IFR_RowSet.h,\
        IFR_UpdatableRowSet.h,\
        RTESys_SharedLib.h,\
        SQLDBC_ClientRuntime.h,\
        RTE_IInterface.hpp,\
        SQLDBC_StorageItems.h,\
        gsp100.h \
        dep=SQLDBC_Types.h,:config/Buildinfo/VERSIONS setbuildinfo \
        remake


SQLDBC_C.cpp -I$OWN/sys/wrk/incl <-$WRK/src/SAPDB/Interfaces/SQLDBC/ -DBUILD_INTERFACE_RUNTIME \
        dep=SQLDBC-sqldbc_c.h \
        inc=SQLDBC.h,\
        SQLDBC-sqldbc_c.h \

SQLDBC-crtedll.cpp -DCLIENTRUNTIME_DLL

SQLDBC_IRuntime.h inc=\
                      SAPDBCommon/SAPDB_ReuseLib.h,SQLDBC_Types.h,SAPDBMem_IRawAllocator.hpp,\
                      SAPDB/Interfaces/SQLDBC/SQLDBC_Internals.h,\
                      SAPDB_Types.h

SQLDBC_ClientRuntime.h inc=SQLDBC_IRuntime.h,\
                           :SAPDB/Interfaces/SQLDBC/SQLDBC.h,\
                           IFRUtil_DefaultRawAllocator.h,\
                           SAPDBCommon/SAPDB_ReuseLib.h,\
                           IFR_Common.h,\
                           IFR_Environment.h,\
                           RunTime/Synchronisation/RTESync_Spinlock.hpp,\
                           heo07.h,\
                           IFR_ConnectProperties.h,\
                           IFRUtil_TraceSharedMemory.h,\
                           RTESec_Authentication.hpp

SQLDBC_ClientRuntime.cpp inc=SQLDBC_ClientRuntime.h,\
    SAPDB/RunTime/Synchronisation/RTESync_Spinlock.hpp,\
    SAPDB/RunTime/Synchronisation/RTESync_InterlockedCounter.hpp,\
    SAPDB/RunTime/RTE_IInterface.hpp,\
    SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_Alloca.h,\
    IFR_Common.h,\
    IFR_ErrorHndl.h,\
    IFRUtil_Tls.h,\
    IFRUtil_Time.h,\
    IFRUtil_RuntimeItem.h,\
    IFRUtil_Configuration.h,\
    IFRUtil_LinkedList.h,\
    SAPDB/Interfaces/Runtime/Packet/IFRPacket_RequestPacket.h,\
    SAPDB/Interfaces/Runtime/Packet/IFRPacket_RequestSegment.h,\
    SAPDB/Interfaces/Runtime/Packet/IFRPacket_ReplyPacket.h,\
    SAPDB/Interfaces/Runtime/Packet/IFRPacket_ReplySegment.h,\
    IFR_Connection.h,\
    heo02.h,\
    heo13.h,\
    heo670.h,\
    SAPDB/Interfaces/Runtime/Packet/IFRPacket_Part.h,\
    hsp78_0.h,\
    :extern/zlib/zlib.h,\
    RTEComm_ParseURI.hpp \
    dep=:config/Buildinfo/VERSIONS setbuildinfo \
    remake

SQLDBC_IProxyRuntime.h inc=SQLDBC_IRuntime.h
SQLDBC_ProxyRuntime.h inc=\
    SQLDBC_ClientRuntime.h,\
    :SAPDB/Interfaces/SQLDBC/SQLDBC.h,\
    SQLDBC_IProxyRuntime.h,\
    SQLDBC_Proxy.h

SQLDBC_ProxyRuntime.cpp inc=\
    SQLDBC_ProxyRuntime.h,\
    IFRPacket_RequestPacket.h,\
    IFRPacket_RequestSegment.h,\
    IFRPacket_ReplyPacket.h,\
    IFRPacket_ReplySegment.h,\
    IFRPacket_Part.h,\
    Msg_List.hpp

SQLDBC_SQLCppRuntime.h inc=SQLDBC_IRuntime.h,:SAPDB/Interfaces/SQLDBC/SQLDBC.h
SQLDBC_SQLCppRuntime.cpp inc=SQLDBC_SQLCppRuntime.h,LVC_IliveCacheSink.hpp

SQLDBC_Catalog.h   inc=SQLDBC.h
SQLDBC_Catalog.cpp inc=SQLDBC_IRuntime.h,SQLDBC_ClientRuntime.h,SQLDBC_Catalog.h

SQLDBC_Internals.h     inc=SAPDBCommon/SAPDB_Types.h

SQLDBC_StorageItems.h  inc=SQLDBC.h,SQLDBC_Types.h,SQLDBC_IRuntime.h,IFRUtil_LinkedList.h

SQLDBC_StorageItems*.cpp  inc=SQLDBC_StorageItems.h,\
							IFR_Common.h,\
							IFR_Environment.h,\
							IFR_Connection.h,\
							IFR_Statement.h,\
							IFR_PreparedStmt.h,\
							IFR_ResultSet.h

SQLDBC_Internals.h     inc=SAPDBCommon/SAPDB_Types.h

SQLDBC_StorageItems.h  inc=SQLDBC.h,SQLDBC_Types.h,SQLDBC_IRuntime.h,IFRUtil_LinkedList.h

SQLDBC_StorageItems.cpp  inc=SQLDBC_StorageItems.h,\
							IFR_Common.h,\
							IFR_Environment.h,\
							IFR_Connection.h,\
							IFR_Statement.h,\
							IFR_PreparedStmt.h,\
							IFR_ResultSet.h

SQLDBC.doxygen concom=yes ascii \
        dep=:config/Buildinfo/VERSIONS setbuildinfo

SQLDBC-FLOSS.doxygen FLOSS concom=yes ascii \
        dep=:config/Buildinfo/VERSIONS setbuildinfo

main.dxy ascii \
        dep=:config/Buildinfo/VERSIONS setbuildinfo

install.dxy ascii \
        dep=:config/Buildinfo/VERSIONS setbuildinfo

license.dxy concom=yes ascii \
        dep=:config/Buildinfo/VERSIONS setbuildinfo

license-FLOSS.dxy concom=yes ascii \
        dep=:config/Buildinfo/VERSIONS setbuildinfo \
        FLOSS ->$WRK/fast/obj/SAPDB/Interfaces/SQLDBC/license.dxy
   
genSQLDBCdoc.pl ascii concom=no noversion unpack=no exec ->$OWN/tool/bin/ suppress_uncond 
  