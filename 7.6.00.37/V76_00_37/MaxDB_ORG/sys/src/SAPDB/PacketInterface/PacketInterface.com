# -----------------------------------------------------------------------------
# 
# PURPOSE
# Handling Description for Packet Interface layer.
# 
# RELEASE:  SAP DB 
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

*-kernel -DBUILD_INTERFACE_RUNTIME -DIFR_NAMESPACE=SQLDBC_Kernel -DSQLDBC=SQLDBC_Kernel -DSQLDBC_RUNTIME_IN_NAMESPACE

# include dependencies (do not edit)
#
PIn.h
PIn_FileTraceStream.cpp
PIn_FileTraceStream.h
PIn_Packet.cpp
PIn_Packet.h
PIn_Part.cpp
PIn_Part.h
PIn_ReplyParser.cpp
PIn_ReplyParser.h
PIn_RequestWriter.cpp
PIn_RequestWriter.h
PIn_Segment.cpp
PIn_Segment.h
PIn_SimpleConnect.cpp
PIn_SimpleTraceStream.cpp
PIn_SimpleTraceStream.h
PIn_SpecialParts.cpp
PIn_SpecialParts.h
PIn_XUserConnect.cpp
PIn_XUserConnect.h

#
# include dependencies (do not edit)
#
PIn.h inc=vsp001,:SAPDB/PacketInterface/PIn_SimpleTraceStream.h
PIn_FileTraceStream.cpp inc=heo06.h,\
    :SAPDB/PacketInterface/PIn_FileTraceStream.h
PIn_FileTraceStream.h inc=:SAPDB/PacketInterface/PIn_SimpleTraceStream.h
PIn_Packet.cpp inc=hsp100.h,:SAPDB/PacketInterface/PIn_Packet.h
PIn_Packet.h inc=vsp001,:SAPDB/PacketInterface/PIn_Segment.h
PIn_Part.cpp inc=hsp77.h,:SAPDB/PacketInterface/PIn_Part.h,\
    :SAPDB/SAPDBCommon/SAPDB_UTF8Basis.hpp
PIn_Part.h inc=:SAPDB/PacketInterface/PIn.h,hsp81.h,\
    :SAPDB/PacketInterface/PIn_SimpleTraceStream.h
PIn_ReplyParser.cpp inc=:SAPDB/PacketInterface/PIn_ReplyParser.h,hsp40.h
PIn_ReplyParser.h inc=:SAPDB/PacketInterface/PIn_Packet.h
PIn_RequestWriter.cpp inc=:SAPDB/PacketInterface/PIn_RequestWriter.h,\
    hsp77.h
PIn_RequestWriter.h inc=:SAPDB/PacketInterface/PIn_Packet.h,hsp81.h
PIn_Segment.cpp inc=:SAPDB/PacketInterface/PIn_Segment.h
PIn_Segment.h inc=vsp001,:SAPDB/PacketInterface/PIn_Part.h
PIn_SimpleConnect.cpp inc=heo02.h,heo03.h,vsp004,hsp02.h,\
    :SAPDB/PacketInterface/PIn.h,\
    :SAPDB/PacketInterface/PIn_ReplyParser.h,\
    :SAPDB/PacketInterface/PIn_RequestWriter.h,\
    :SAPDB/PacketInterface/PIn_XUserConnect.h,\
    :SAPDB/PacketInterface/PIn_FileTraceStream.h
PIn_SimpleTraceStream.cpp inc=hsp77.h,\
    :SAPDB/PacketInterface/PIn_SimpleTraceStream.h
PIn_SimpleTraceStream.h inc=gsp12.h
PIn_SpecialParts*.cpp inc=\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageEventConverter.hpp,\
    :SAPDB/PacketInterface/PIn_SpecialParts.h,\
    :SAPDB/Interfaces/Runtime/Util/IFRUtil_VDNNumber.h,hsp40.h,hsp77.h
PIn_SpecialParts.h inc=:SAPDB/PacketInterface/PIn_Part.h,\
    :SAPDB/Messages/Msg_List.hpp
PIn_XUserConnect.cpp inc=heo02.h,heo03.h,gsp01.h,hsp02.h,\
    :SAPDB/PacketInterface/PIn_FileTraceStream.h,\
    :SAPDB/PacketInterface/PIn_RequestWriter.h,\
    :SAPDB/PacketInterface/PIn_XUserConnect.h
PIn_XUserConnect.h inc=vsp004,:SAPDB/PacketInterface/PIn_Packet.h

