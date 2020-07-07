# -----------------------------------------------------------------------------
# 
# PURPOSE
# Handling Description for Runtime Packet layer.
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

* -I$OWN/sys/wrk/sdk/incl -DBUILD_INTERFACE_RUNTIME 
*-nomt -I$OWN/sys/wrk/sdk/incl -DBUILD_INTERFACE_RUNTIME -DIFR_SINGLE_THREADED
*-kernel -DBUILD_INTERFACE_RUNTIME -DSQLDBC_FOR_KERNEL

IFRPacket_RequestPacket.h inc=IFR_Types.h,\
	IFR_String.h,\
	IFR_ErrorHndl.h,\
	IFRUtil_Vector.h,\
	IFRUtil_RuntimeItem.h

IFRPacket_ReplyPacket.h inc=:SAPDBCommon/SAPDB_ReuseLib.h,\
    IFR_Types.h,\
    IFRPacket_Lock.h,\
    IFR_String.h

IFRPacket_ReplyPacket*.cpp inc=\
    IFR_Common.h,\
    IFRPacket_ReplyPacket.h

IFRPacket_Segment.h inc=:SAPDBCommon/SAPDB_ReuseLib.h
IFRPacket_Segment*.cpp inc=IFRPacket_Segment.h
IFRPacket_RequestPacket*.cpp inc=IFRPacket_RequestPacket.h,\
    IFRPacket_Segment.h,\
    IFRUtil_VDNNumber.h,\
    IFR_Common.h,\
    IFRPacket_Lock.h,\
    IFR_ConnectionItem.h,\
    IFRPacket_Part.h,\
    IFRConversion_Converter.h

IFRPacket_ReplySegment.h inc=:SAPDBCommon/SAPDB_ReuseLib.h,IFR_ErrorHndl.h,IFR_String.h,IFRConversion_Converter.h,IFRPacket_Segment.h,IFRPacket_ReplyPacket.h,IFRUtil_Vector.h
IFRPacket_ReplySegment*.cpp inc=IFR_ErrorHndl.h,IFRPacket_ReplySegment.h,IFR_Common.h,\
IFRUtil_DefaultRawAllocator.h,IFRPacket_Part.h,IFRPacket_FunctionCode.h
IFRPacket_RequestSegment.h inc=IFRPacket_Part.h,IFR_ErrorHndl.h,IFR_String.h,IFRPacket_RequestPacket.h

IFRPacket_RequestSegment*.cpp inc=\
    IFR_ErrorHndl.h,\
    IFRPacket_RequestSegment.h,\
    IFR_Common.h,\
    IFR_Trace.h,\
    IFRPacket_RequestPacket.h

IFRPacket_PartEnum.h inc=\
IFRPacket_Part.h,\
IFRPacket_ReplySegment.h

IFRPacket_PartEnum*.cpp inc=\
IFR_Common.h,\
IFRPacket_PartEnum.h

IFRPacket_Part.h inc=\
    :SAPDBCommon/SAPDB_ReuseLib.h,\
    IFR_String.h,\
    IFR_Types.h,\
    IFR_ErrorHndl.h,\
    IFR_Datatypes.h,\
    IFRUtil_Vector.h,\
    IFR_ShortInfo.h

IFRPacket_Part*.cpp inc=\
    IFR_Common.h,\
    IFR_Environment.h,\
    IFR_Connection.h,\
    IFRPacket_Part.h,\
    IFRUtil_VDNNumber.h,\
    IFRUtil_BinaryToHex.h,\
    IFRConversion_Factory.h

IFRPacket_FunctionCode.h inc=\
:SAPDBCommon/SAPDB_ReuseLib.h,\
IFR_Types.h,\
IFR_Trace.h
 
IFRPacket_FunctionCode*.cpp inc=\
IFRPacket_FunctionCode.h

IFRPacket_Lock.h inc=IFRUtil_Vector.h,IFR_Types.h,SAPDBCommon/SAPDB_ReuseLib.h,IFRUtil_RuntimeItem.h
IFRPacket_Lock*.cpp inc=\
        IFRPacket_Lock.h,\
        IFR_Common.h,\
        IFRPacket_RequestPacket.h,\
        IFR_Connection.h

IFRPacket_LongDescriptor.h inc=\
	SAPDBCommon/SAPDB_ReuseLib.h,\
	IFR_Common.h

IFRPacket_LongDescriptor*.cpp inc=\
	IFRPacket_LongDescriptor.h
	


	

