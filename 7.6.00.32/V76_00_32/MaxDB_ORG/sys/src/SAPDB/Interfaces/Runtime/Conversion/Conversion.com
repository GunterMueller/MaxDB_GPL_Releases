# -----------------------------------------------------------------------------
#
# PURPOSE
# Handling Description for Runtime Conversion layer.
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

IFRConversion_Converter.h inc=\
    IFR_String.h,\
    IFR_ErrorHndl.h,\
    IFRPacket_Part.h,\
    IFR_Parameter.h,\
    IFR_Datatypes.h,\
    IFR_ShortInfo.h,\
    IFR_ConnectionItem.h,\
    IFR_Common.h,\
    IFRConversion_Putval.h,\
    IFRConversion_Getval.h,\
    SAPDBAlgo_QuickSort.hpp

IFRConversion_Converter*.cpp inc=\
    IFRConversion_Converter.h,\
    IFR_Connection.h,\
    IFR_Common.h

IFRConversion_CharAsciiConverter.h inc=IFRConversion_Converter.h,IFRConversion_ByteCharDataConverter.h
IFRConversion_CharAsciiConverter*.cpp inc=\
    IFRConversion_CharAsciiConverter.h,\
    IFR_Common.h

IFRConversion_Factory.h inc=\
    IFRConversion_Converter.h,\
    IFR_ShortInfo.h,\
    IFR_Common.h,\
    IFRUtil_RuntimeItem.h

IFRConversion_Factory*.cpp inc=IFRConversion_Factory.h,\
    IFR_Datatypes.h,\
    IFR_Common.h,\
    IFR_Connection.h,\
    IFRConversion_CharAsciiConverter.h,\
    IFRConversion_VarcharAsciiConverter.h,\
    IFRConversion_VarcharUCS2Converter.h,\
    IFRConversion_CharUCS2Converter.h,\
    IFRConversion_CharByteConverter.h,\
    IFRConversion_VarcharByteConverter.h,\
    IFRConversion_NumericConverter.h,\
    IFRConversion_DateConverter.h,\
    IFRConversion_BooleanConverter.h,\
    IFRConversion_TimeConverter.h,\
    IFRConversion_DateTimeConverter.h,\
    IFRConversion_StreamConverter.h,\
    IFRConversion_ABAPStreamConverter.h,\
    IFRConversion_TimeUCS2Converter.h,\
    IFRConversion_DateTimeUCS2Converter.h,\
    IFRConversion_DateUCS2Converter.h

IFRConversion_StreamConverter.h inc=\
    IFRConversion_Converter.h,\
    IFRConversion_ByteCharDataConverter.h

IFRConversion_StreamConverter*.cpp inc=\
    IFR_Common.h,\
    IFRConversion_StreamConverter.h,\
    IFRConversion_Putval.h,\
    IFR_Connection.h,\
    IFR_LOBHost.h

IFRConversion_Putval.h inc=\
    IFRPacket_Part.h,\
    IFRPacket_LongDescriptor.h,\
    IFR_ConnectionItem.h,\
    IFR_Parameter.h

IFRConversion_Putval*.cpp inc=\
    IFRConversion_Putval.h,\
    IFRConversion_Converter.h,\
    IFR_Common.h,\
    IFR_Connection.h,\
    IFRConversion_ToString.h

IFRConversion_Getval.h inc=\
    IFRPacket_Part.h,\
    IFRPacket_LongDescriptor.h,\
    IFR_ConnectionItem.h,\
    IFR_Parameter.h,\
    IFR_Common.h

IFRConversion_Getval*.cpp inc=\
    IFRConversion_Getval.h,\
    IFRConversion_Converter.h,\
    IFRPacket_RequestPacket.h,\
    IFRPacket_ReplyPacket.h,\
    IFRPacket_RequestSegment.h,\
    IFRPacket_ReplySegment.h,\
    IFR_Connection.h,\
    IFRUtil_BinaryToHex.h,\
    IFRConversion_FromString.h

IFRConversion_VarcharAsciiConverter.h inc=\
    IFRConversion_ByteCharDataConverter.h,\
    IFRConversion_Converter.h
IFRConversion_VarcharAsciiConverter*.cpp inc=\
    IFRConversion_VarcharAsciiConverter.h,IFR_Common.h

IFRConversion_NumericConverter.h inc=IFRConversion_Converter.h

&if $OSSPEC == OSF1
IFRConversion_NumericConverter*.cpp inc=IFRConversion_NumericConverter.h,IFR_Common.h,IFRUtil_VDNNumber.h,IFRUtil_SQLNumeric.h,IFR_Connection.h,hsp51.h -ieee
&else
IFRConversion_NumericConverter*.cpp inc=IFRConversion_NumericConverter.h,IFR_Common.h,IFRUtil_VDNNumber.h,IFRUtil_SQLNumeric.h,IFR_Connection.h,hsp51.h 
&endif

IFRConversion_BooleanConverter.h inc=IFRConversion_Converter.h
IFRConversion_BooleanConverter*.cpp inc=\
IFRConversion_BooleanConverter.h,\
IFR_Common.h


IFRConversion_DateConverter.h inc=IFRConversion_ByteCharDataConverter.h
IFRConversion_DateConverter*.cpp inc=IFRConversion_DateConverter.h,IFR_Common.h,IFR_ErrorCode.h

IFRConversion_TimeConverter.h inc=IFRConversion_ByteCharDataConverter.h
IFRConversion_TimeConverter*.cpp inc=IFRConversion_TimeConverter.h,IFR_Common.h,IFR_ErrorCode.h,RTESys_MSWindows.h

IFRConversion_DateTimeConverter.h inc=IFRConversion_ByteCharDataConverter.h
IFRConversion_DateTimeConverter*.cpp inc=IFRConversion_DateTimeConverter.h,IFR_Common.h,IFR_ErrorCode.h

IFRConversion_DateUCS2Converter.h inc=IFRConversion_UCS2CharDataConverter.h
IFRConversion_DateUCS2Converter*.cpp inc=IFRConversion_DateUCS2Converter.h,IFR_Common.h,IFR_ErrorCode.h

IFRConversion_TimeUCS2Converter.h inc=IFRConversion_UCS2CharDataConverter.h
IFRConversion_TimeUCS2Converter*.cpp inc=IFRConversion_TimeUCS2Converter.h,IFR_Common.h,IFR_ErrorCode.h

IFRConversion_DateTimeUCS2Converter.h inc=IFRConversion_UCS2CharDataConverter.h
IFRConversion_DateTimeUCS2Converter*.cpp inc=IFRConversion_DateTimeUCS2Converter.h,IFR_Common.h,IFR_ErrorCode.h


IFRConversion_ByteCharDataConverter.h inc=IFRConversion_Converter.h,IFR_Connection.h

IFRConversion_ByteCharDataConverter*.cpp inc=\
    IFRConversion_ByteCharDataConverter.h,\
    IFR_Common.h,\
    IFR_Connection.h,\
    hsp83.h,\
    SAPDBMem_Alloca.h,\
    IFRUtil_SQLNumeric.h,\
    IFRUtil_BinaryToHex.h,\
    IFRConversion_ToString.h,\
    IFRConversion_FromString.h,\
    IFRUtil_DateTime.h

IFRConversion_VarcharByteConverter.h inc=\
    IFRConversion_ByteCharDataConverter.h,\
    IFRConversion_Converter.h
IFRConversion_VarcharByteConverter*.cpp inc=\
    IFRConversion_VarcharByteConverter.h,IFR_Common.h

IFRConversion_CharByteConverter.h inc=\
    IFRConversion_Converter.h,\
    IFRConversion_ByteCharDataConverter.h

IFRConversion_CharByteConverter*.cpp inc=\
    IFRConversion_CharByteConverter.h,\
    IFR_Common.h

IFRConversion_UCS2CharDataConverter.h inc=\
    IFRConversion_Converter.h,\
    IFRConversion_ByteCharDataConverter.h

IFRConversion_UCS2CharDataConverter*.cpp inc=\
    IFRConversion_UCS2CharDataConverter.h,\
    IFR_Common.h,\
    :SAPDBCommon/SAPDB_ReuseLib.h,\
    hsp83.h,\
    IFR_Connection.h

IFRConversion_CharUCS2Converter.h inc=\
    IFRConversion_Converter.h,\
    IFRConversion_UCS2CharDataConverter.h

IFRConversion_CharUCS2Converter*.cpp inc=\
    IFRConversion_CharUCS2Converter.h,\
    IFR_Common.h

IFRConversion_VarcharUCS2Converter.h inc=\
    IFRConversion_Converter.h,\
    IFRConversion_UCS2CharDataConverter.h

IFRConversion_VarcharUCS2Converter*.cpp inc=\
    IFRConversion_VarcharUCS2Converter.h,\
    IFR_Common.h

IFRConversion_ABAPStreamConverter.h inc=\
    IFRConversion_Converter.h

IFRConversion_ABAPStreamConverter*.cpp \
    dep=OMS_LiveCacheTypes.xml \
    inc=\
    IFR_Common.h\
    ,IFR_Connection.h\
    ,IFRConversion_ABAPStreamConverter.h

IFRConversion_ToString.h inc=IFR_Types.h,IFR_ErrorHndl.h,hsp77.h,IFRUtil_DateTime.h,IFRUtil_SQLNumeric.h
IFRConversion_FromString.h inc=IFR_Types.h,IFR_ErrorHndl.h,IFRUtil_DateTime.h,IFRUtil_SQLNumeric.h