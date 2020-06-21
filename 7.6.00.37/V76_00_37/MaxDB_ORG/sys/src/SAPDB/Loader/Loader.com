# -----------------------------------------------------------------------------
#
# PURPOSE
# Handling Description for MaxDB Fastloader API.
#
# RELEASE:  SAP DB 7.6
#
# -----------------------------------------------------------------------------
#
#
#    ========== licence begin  GPL
#    Copyright (c) 2000-2004 SAP AG
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
#

# die beiden folgenden zeilen bedeuten: es gilt fuer alle!!!
* trace=none shadow=yes unpack=no

# Compile all modules with exception handling enabled
regex(.*\.cpp$)    -except
regex(.*\.y$)      -except
regex(.*\.l$)      -except



Loader_Hauptprogramm.cpp

Loader_Types.hpp            output=$INSTROOT/sdk/fastload/incl/

Loader_Common.hpp

Loader_ClientRuntime.hpp
Loader_ClientRuntime.cpp

Loader_Loader*.hpp  concom=yes
Loader_Loader.cpp
Loader_Loader-sdk.hpp SDK output=$INSTROOT/sdk/fastload/incl/Loader_Loader.hpp

Loader_Connection.cpp

Loader_ConnectProperties.cpp

#Loader_FastloadCommand.hpp
Loader_FastloadCommand.cpp

Loader_PageBuilder.hpp
Loader_PageBuilder.cpp


Loader_StringConversion.hpp
Loader_StringConversion.cpp

Loader_SpecialConstants.hpp
Loader_SpecialConstants.cpp

Loader_VDNNumber.hpp
Loader_VDNNumber.cpp
#
# include dependencies (do not edit)
#
Loader.rc inc=gsp100.h
Loader_Beispielprogramm.cpp inc=:SAPDB/Interfaces/SQLDBC/SQLDBC.h,\
    :SAPDB/Loader/Loader_Loader.hpp
Loader_ClientRuntime.cpp inc=:SAPDB/Loader/Loader_ClientRuntime.hpp,\
    :SAPDB/PacketInterface/PIn_Packet.h,\
    :SAPDB/PacketInterface/PIn_SpecialParts.h,\
    :SAPDB/Loader/Loader_Types.hpp
Loader_ClientRuntime.hpp inc=:SAPDB/Interfaces/SQLDBC/SQLDBC_IRuntime.h,\
    :SAPDB/Interfaces/SQLDBC/SQLDBC_ClientRuntime.h,\
    :SAPDB/Loader/Loader_Types.hpp
Loader_Common.cpp inc=:SAPDB/Loader/Loader_Common.hpp
Loader_Common.hpp inc=:SAPDB/Loader/Loader_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_string.h,hsp77.h,vsp001,vin00
Loader_ConnectProperties.cpp inc=:SAPDB/Loader/Loader_ErrorCode.hpp,\
    :SAPDB/Loader/Loader_ErrorMsgHandler.hpp,\
    :SAPDB/Loader/Loader_Types.hpp,:SAPDB/Loader/Loader_Loader.hpp,\
    :SAPDB/Interfaces/SQLDBC/SQLDBC.h,:SAPDB/SAPDBCommon/SAPDB_string.h
Loader_Connection.cpp inc=:SAPDB/Interfaces/SQLDBC/SQLDBC_IRuntime.h,\
    :SAPDB/Loader/Loader_ErrorCode.hpp,\
    :SAPDB/Loader/Loader_ErrorMsgHandler.hpp,\
    :SAPDB/Loader/Loader_Types.hpp,:SAPDB/Loader/Loader_Loader.hpp,\
    :SAPDB/Interfaces/SQLDBC/SQLDBC.h,:SAPDB/SAPDBCommon/SAPDB_string.h
Loader_ErrorCode.hpp inc=
Loader_ErrorMsgHandler.cpp inc=:SAPDB/Loader/Loader_Types.hpp,\
    :SAPDB/Loader/Loader_ErrorCode.hpp,\
    :SAPDB/Loader/Loader_ErrorMsgHandler.hpp
Loader_ErrorMsgHandler.hpp inc=:SAPDB/Loader/Loader_Common.hpp,\
    :SAPDB/Loader/Loader_ErrorCode.hpp
Loader_FastloadCommand.cpp inc=:SAPDB/Loader/Loader_Common.hpp,\
    :SAPDB/Loader/Loader_Types.hpp,:SAPDB/Loader/Loader_ErrorCode.hpp,\
    :SAPDB/Loader/Loader_ErrorMsgHandler.hpp,\
    :SAPDB/Loader/Loader_Loader.hpp,\
    :SAPDB/PacketInterface/PIn_SpecialParts.h,\
    :SAPDB/Interfaces/SQLDBC/SQLDBC.h,\
    :SAPDB/Interfaces/Runtime/Util/IFRUtil_VDNNumber.h,\
    :SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp,\
    :SAPDB/Loader/Loader_StringConversion.hpp,\
    :SAPDB/Loader/Loader_PageBuilder.hpp,\
    :SAPDB/Loader/Loader_FastloadCommandSupport.hpp,\
    :SAPDB/PacketInterface/PIn_Packet.h,gsp100.h
Loader_FastloadCommandSupport.cpp inc=\
    :SAPDB/Loader/Loader_FastloadCommandSupport.hpp,\
    :SAPDB/Loader/Loader_Types.hpp,:SAPDB/Loader/Loader_Common.hpp
Loader_FastloadCommandSupport.hpp inc=:SAPDB/Loader/Loader_Types.hpp,\
    :SAPDB/Loader/Loader_Common.hpp,:SAPDB/Loader/Loader_ErrorCode.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_string.h,hsp77.h
Loader_Hauptprogramm.cpp inc=:SAPDB/Interfaces/SQLDBC/SQLDBC.h,\
    :SAPDB/Loader/Loader_Loader.hpp,hsp77.h
Loader_Loader.cpp inc=:SAPDB/Loader/Loader_ErrorCode.hpp,\
    :SAPDB/Loader/Loader_Common.hpp,\
    :SAPDB/Loader/Loader_ErrorMsgHandler.hpp,\
    :SAPDB/Interfaces/SQLDBC/SQLDBC.h,:SAPDB/Loader/Loader_Loader.hpp
Loader_Loader.hpp inc=:SAPDB/Interfaces/SQLDBC/SQLDBC.h
Loader_Loader*.hpp inc=:SAPDB/Interfaces/SQLDBC/SQLDBC.h
Loader_Loader-sdk.hpp inc=:SAPDB/Interfaces/SQLDBC/SQLDBC.h
Loader_PageBuilder.cpp inc=:SAPDB/Loader/Loader_ErrorCode.hpp,\
    :SAPDB/Loader/Loader_ErrorMsgHandler.hpp,\
    :SAPDB/Loader/Loader_Loader.hpp,:SAPDB/Loader/Loader_PageBuilder.hpp,\
    :SAPDB/Loader/Loader_Types.hpp,:SAPDB/Loader/Loader_Common.hpp,\
    :SAPDB/Loader/Loader_VDNNumber.hpp,\
    :SAPDB/Loader/Loader_StringConversion.hpp,\
    :SAPDB/Loader/Loader_SpecialConstants.hpp,gbd00,vin00,hsp30.h,\
    :SAPDB/Loader/Loader_ClientRuntime.hpp
Loader_PageBuilder.hpp inc=:SAPDB/Loader/Loader_Types.hpp,\
    :SAPDB/Loader/Loader_Common.hpp,:SAPDB/Loader/Loader_ErrorCode.hpp,\
    :SAPDB/Loader/Loader_Loader.hpp,\
    :SAPDB/Loader/Loader_StringConversion.hpp,gbd00
Loader_SpecialConstants.cpp inc=:SAPDB/Loader/Loader_Types.hpp,\
    :SAPDB/Loader/Loader_Common.hpp,:SAPDB/Loader/Loader_ErrorCode.hpp,\
    :SAPDB/Loader/Loader_SpecialConstants.hpp,\
    :SAPDB/Loader/Loader_StringConversion.hpp,heo02.h,vin00
Loader_SpecialConstants.hpp inc=:SAPDB/Loader/Loader_Types.hpp,\
    :SAPDB/Loader/Loader_Common.hpp
Loader_StringConversion.cpp inc=:SAPDB/Loader/Loader_Types.hpp,\
    :SAPDB/Loader/Loader_Common.hpp,:SAPDB/Loader/Loader_ErrorCode.hpp,\
    :SAPDB/Loader/Loader_StringConversion.hpp,hsp78_0.h,hsp81.h,\
    :SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp
Loader_StringConversion.hpp inc=:SAPDB/Loader/Loader_Types.hpp,\
    :SAPDB/Loader/Loader_Common.hpp,:SAPDB/Loader/Loader_ErrorCode.hpp,\
    :SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp
Loader_Types.hpp inc=
Loader_VDNNumber.cpp inc=:SAPDB/Loader/Loader_VDNNumber.hpp,\
    :SAPDB/Interfaces/Runtime/Util/IFRUtil_VDNNumber.h
Loader_VDNNumber.hpp inc=:SAPDB/Loader/Loader_Types.hpp,\
    :SAPDB/Loader/Loader_ErrorCode.hpp,\
    :SAPDB/Interfaces/Runtime/Util/IFRUtil_VDNNumber.h

