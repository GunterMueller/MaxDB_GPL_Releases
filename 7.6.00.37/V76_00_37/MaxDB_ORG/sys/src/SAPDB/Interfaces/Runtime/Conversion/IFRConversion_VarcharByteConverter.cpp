/*!
  @file           IFRConversion_VarcharByteConverter.cpp
  @author         D039759
  @ingroup        IFR_DataConv
  @brief
  @see

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    ========== licence end



\endif
*/
#include "Interfaces/Runtime/IFR_Common.h"
#include "Interfaces/Runtime/Conversion/IFRConversion_VarcharByteConverter.h"

#define DBUG_CLINK_METHOD_ENTER(x,y) DBUG_CONTEXT_METHOD_ENTER(x, y, &clink)

//----------------------------------------------------------------------
IFRConversion_VarcharByteConverter
::IFRConversion_VarcharByteConverter(IFR_ShortInfo &shortinfo,
                                     SAPDBMem_IRawAllocator& allocator,
                                     IFR_Connection& connection)
:IFRConversion_ByteCharDataConverter(shortinfo, allocator, true, false, 
                                     connection.getSQLMode() == IFR_ORACLE)
{
    m_flags.bin2hex           = connection.odbcBinToHex() ? 1 : 0;
}

//----------------------------------------------------------------------
IFRConversion_VarcharByteConverter
::~IFRConversion_VarcharByteConverter()
{}


//----------------------------------------------------------------------
IFR_Retcode 
IFRConversion_VarcharByteConverter::appendUCS2Input(IFRPacket_DataPart&  datapart,
                                                 char                *data,
                                                 IFR_Bool             swapped,
                                                 IFR_Length           datalength,
                                                 IFR_Length*          lengthindicator,
                                                 IFR_Bool terminate,
                                                 IFR_ConnectionItem  &clink,
                                                 IFR_Length&          offset,
                                                 IFRConversion_Putval *putval)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_VarcharByteConverter, appendUCS2Input);
    DBUG_RETURN(appendBinaryInput(datapart, data, datalength, lengthindicator, clink, offset, putval));
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_VarcharByteConverter::appendUTF8Input(IFRPacket_DataPart& datapart,
                                                 char               *data,
                                                 IFR_Length            datalength,
                                                 IFR_Length*           lengthindicator,
                                                 IFR_Bool terminate,
                                                 IFR_ConnectionItem &clink,
                                                 IFR_Length&         offset,
                                                 IFRConversion_Putval *putval)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_VarcharByteConverter, appendUTF8Input);
    DBUG_RETURN(appendBinaryInput(datapart, data, datalength, lengthindicator, clink, offset, putval));
}



//----------------------------------------------------------------------
IFR_Retcode 
IFRConversion_VarcharByteConverter::appendUCS2Output(IFRPacket_DataPart&   datapart,
                                                  char                 *data,
                                                  IFR_Bool              swapped,
                                                  IFR_Length            datalength,
                                                  IFR_Length*           lengthindicator,
                                                  IFR_Bool              terminate,
                                                  IFR_ConnectionItem   &clink,
                                                  IFR_Length&           dataoffset,
                                                  IFR_Length&           offset,
                                                  IFRConversion_Getval *getval)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_VarcharByteConverter, appendUCS2Output);
    if(terminate) {
        clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)getIndex());
        DBUG_RETURN(IFR_NOT_OK);
    }
    DBUG_RETURN(appendBinaryOutput(datapart, data, datalength, lengthindicator, clink, dataoffset, offset, getval));
}

//----------------------------------------------------------------------
IFR_Retcode 
IFRConversion_VarcharByteConverter::appendUTF8Output(IFRPacket_DataPart&   datapart,
                                                  char                 *data,
                                                  IFR_Length            datalength,
                                                  IFR_Length*           lengthindicator,
                                                  IFR_Bool              terminate,
                                                  IFR_ConnectionItem   &clink,
                                                  IFR_Length&           dataoffset,
                                                  IFR_Length&           offset,
                                                  IFRConversion_Getval *getval)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_VarcharByteConverter, appendUCS2Output);
    if(terminate) {
        clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)getIndex());
        DBUG_RETURN(IFR_NOT_OK);
    }
    DBUG_RETURN(appendBinaryOutput(datapart, data, datalength, lengthindicator, clink, dataoffset, offset, getval));
    
}


//----------------------------------------------------------------------
IFR_Retcode 
IFRConversion_VarcharByteConverter::translateInput(IFRPacket_DataPart& datapart,
                                                SQL_DATE_STRUCT&    data,
                                                IFR_Length*           lengthindicator,
                                                IFR_ConnectionItem& clink,
                                                IFRConversion_Putval *pv)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_VarcharByteConverter, translateInput_SQL_DATE_STRUCT);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)getIndex());
    DBUG_RETURN(IFR_NOT_OK);
}
    
//----------------------------------------------------------------------
IFR_Retcode 
IFRConversion_VarcharByteConverter::translateInput(IFRPacket_DataPart& datapart,
                                                SQL_TIME_STRUCT&    data,
                                                IFR_Length*           lengthindicator,
                                                IFR_ConnectionItem& clink,
                                                IFRConversion_Putval *pv)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_VarcharByteConverter, translateInput_SQL_TIME_STRUCT);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)getIndex());
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------
IFR_Retcode 
IFRConversion_VarcharByteConverter::translateInput(IFRPacket_DataPart& datapart,
                                                SQL_TIMESTAMP_STRUCT&    data,
                                                IFR_Length*           lengthindicator,
                                                IFR_ConnectionItem& clink,
                                                IFRConversion_Putval *pv)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_VarcharByteConverter, translateInput_SQL_TIMESTAMP_STRUCT);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)getIndex());
    DBUG_RETURN(IFR_NOT_OK);
}


//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_VarcharByteConverter::translateOutput(IFRPacket_DataPart&   datapart,
                                                 SQL_DATE_STRUCT&      data,
                                                 IFR_Length*             lengthindicator,
                                                 IFR_ConnectionItem&   clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_VarcharByteConverter, translateOutput_SQL_DATE_STRUCT);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)getIndex());
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_VarcharByteConverter::translateOutput(IFRPacket_DataPart&   datapart,
                                                 SQL_TIME_STRUCT&      data,
                                                 IFR_Length*             lengthindicator,
                                                 IFR_ConnectionItem&   clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_VarcharByteConverter, translateOutput_SQL_TIME_STRUCT);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)getIndex());
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_VarcharByteConverter::translateOutput(IFRPacket_DataPart&   datapart,
                                                 SQL_TIMESTAMP_STRUCT&      data,
                                                 IFR_Length*             lengthindicator,
                                                 IFR_ConnectionItem&   clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_VarcharByteConverter, translateOutput_SQL_TIMESTAMP_STRUCT);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)getIndex());
    DBUG_RETURN(IFR_NOT_OK);
}

