/*!
  @file           IFRConversion_DateTimeConverter.cpp
  @author         D039759
  @ingroup        IFR_DataConv
  @brief          Conversion from/to DateTime values
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
#include "Interfaces/Runtime/IFR_ErrorCode.h"
#include "Interfaces/Runtime/Conversion/IFRConversion_DateTimeConverter.h"

#define DBUG_CLINK_METHOD_ENTER(x,y) DBUG_CONTEXT_METHOD_ENTER(x, y, &clink)

//----------------------------------------------------------------------
IFRConversion_DateTimeConverter::IFRConversion_DateTimeConverter(IFR_ShortInfo& shortinfo,
                                                                 SAPDBMem_IRawAllocator& allocator,
                                                                 IFR_Connection& connection)
:IFRConversion_ByteCharDataConverter(shortinfo, allocator, false, false, false)
{
}

//----------------------------------------------------------------------
IFRConversion_DateTimeConverter::~IFRConversion_DateTimeConverter()
{}


//----------------------------------------------------------------------
IFR_Retcode 
IFRConversion_DateTimeConverter::appendAsciiInput(IFRPacket_DataPart& datapart,
                                                  char               *data,
                                                  IFR_Length            datalength,
                                                  IFR_Length*           lengthindicator,
                                                  IFR_Bool              terminate,
                                                  IFR_Bool              ascii7bit,
                                                  IFR_ConnectionItem &clink,
                                                  IFR_Length&         offset,
                                                  IFRConversion_Putval *putval)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_DateConverter, appendAsciiInput);
    DBUG_PRINT(datalength);
    DBUG_PRINT(lengthindicator);
    DBUG_PRINT(offset);

    IFR_Length byteslength;
    if(IFRConversion_InputDataLength(datalength, lengthindicator, data, byteslength, terminate)) {
        clink.error().setRuntimeError(IFR_ERR_INVALID_LENGTHINDICATOR_I, (IFR_Int4)getIndex());
        DBUG_RETURN(IFR_NOT_OK);
    }
    
    // check for an ODBC date literal
    if(byteslength > 5) {
        if(data[0]=='{' &&
           data[1]=='t' &&
           data[2]=='s' &&
           data[3]==' ' &&
           data[byteslength-1]=='}') {
            data += 4;
            byteslength -=5;

            while(byteslength && data[0] == ' ') {
                ++data;
                --byteslength;
            }
            while(byteslength && data[byteslength - 1] == ' ') {
                --byteslength;
            }
            
        }
    }
    
    DBUG_RETURN(IFRConversion_ByteCharDataConverter::appendAsciiInput(datapart, data, datalength, &byteslength, 
                                                                      false, 
                                                                      ascii7bit, clink, offset, putval));
}


//----------------------------------------------------------------------
IFR_Retcode 
IFRConversion_DateTimeConverter::appendUCS2Input(IFRPacket_DataPart&  datapart,
                                             char                *data,
                                             IFR_Bool            swapped,
                                             IFR_Length           datalength,
                                             IFR_Length*          lengthindicator,
                                                 IFR_Bool             terminate,
                                             IFR_ConnectionItem  &clink,
                                             IFR_Length&          offset,
                                             IFRConversion_Putval *putval)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_ByteCharDataConverter, appendUCS2Input);
    
    // Target encoding is ASCII, source encoding is some UCS2
    IFR_Length byteslength;
    if(IFRConversion_InputDataLengthUCS2(datalength, lengthindicator, data, byteslength, terminate)) {
        clink.error().setRuntimeError(IFR_ERR_INVALID_LENGTHINDICATOR_I, (IFR_Int4)getIndex());
        DBUG_RETURN(IFR_NOT_OK);
    }
    if(byteslength % 2) {
        clink.error().setRuntimeError(IFR_ERR_ODD_DATALENGTH_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    }

    if(byteslength >= 10) {
        if(swapped) {
            if(data[1] == 0 && data[0]=='{' &&
               data[3] == 0 && data[2]=='t' &&
               data[5] == 0 && data[4]=='s' &&
               data[7] == 0 && data[6]==' ' &&
               data[byteslength-1] == '}' &&
               data[byteslength-2] == 0) {
                data += 8;
                byteslength -= 10;

                while(byteslength >= 2 && data[1]==0 && data[0] == ' ') {
                    data += 2;
                    byteslength -= 2;
                }
                while(byteslength >= 2 && data[byteslength-2]==0 && data[byteslength-1]==' ') {
                    byteslength -= 2;
                }


            }
        } else {
            if(data[0] == 0 && data[1]=='{' &&
               data[2] == 0 && data[3]=='t' &&
               data[4] == 0 && data[5]=='s' &&
               data[6] == 0 && data[7]==' ' &&
               data[byteslength-1] == '}' &&
               data[byteslength-2] == 0) {
                data += 8;
                byteslength -= 10;

                while(byteslength >= 2 && data[0]==0 && data[1] == ' ') {
                    data += 2;
                    byteslength -= 2;
                }
                while(byteslength >= 2 && data[byteslength-1]==' ' && data[byteslength-2]==0) {
                    byteslength -= 2;
                }
                
            }
        }
    }
    DBUG_RETURN(IFRConversion_ByteCharDataConverter::appendUCS2Input(datapart, data, swapped, 
                                                                     datalength, &byteslength, 
                                                                     false,
                                                                     clink, offset, putval));
}

//----------------------------------------------------------------------
IFR_Retcode 
IFRConversion_DateTimeConverter::appendUTF8Input(IFRPacket_DataPart& datapart,
                                             char               *data,
                                             IFR_Length            datalength,
                                             IFR_Length*           lengthindicator,
                                                 IFR_Bool          terminate,
                                             IFR_ConnectionItem &clink,
                                             IFR_Length&         offset,
                                             IFRConversion_Putval *putval)
{
    // identical for ASCII
    return appendAsciiInput(datapart, data, datalength, lengthindicator, terminate, false, clink, offset, putval);
}


//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_DateTimeConverter::appendBinaryInput(IFRPacket_DataPart& datapart,
                                                    char               *data,
                                                    IFR_Length            datalength,
                                                    IFR_Length*           lengthindicator,
                                                    IFR_ConnectionItem & clink,
                                                    IFR_Length         &offset,
                                                    IFRConversion_Putval *putval)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_DateTimeConverter, appendBinaryInput);
    DBUG_RETURN(IFRConversion_ByteCharDataConverter::appendBinaryInput(datapart, data, datalength, lengthindicator, clink, offset, putval));
}

//----------------------------------------------------------------------
IFR_Retcode 
IFRConversion_DateTimeConverter::appendBinaryOutput(IFRPacket_DataPart&   datapart,
                                                char                 *data,
                                                IFR_Length            datalength,
                                                IFR_Length*           lengthindicator,
                                                IFR_ConnectionItem   &clink,
                                                IFR_Length&           dataoffset,
                                                IFR_Length&           offset,
                                                IFRConversion_Getval *getval)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_DateTimeConverter, appendBinaryOutput);
    DBUG_RETURN(IFRConversion_ByteCharDataConverter::appendBinaryOutput(datapart, 
                                                                        data,
                                                                        datalength,
                                                                        lengthindicator,
                                                                        clink,
                                                                        dataoffset,
                                                                        offset,
                                                                        getval));
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_DateTimeConverter::translateInput(IFRPacket_DataPart& part,
                                            SQL_TIME_STRUCT& data,
                                            IFR_Length *lengthindicator,
                                                IFR_ConnectionItem& clink,
                                                IFRConversion_Putval *pv)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_DateTimeConverter, translateInput);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED);
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_DateTimeConverter::translateInput(IFRPacket_DataPart& part,
                                                SQL_DATE_STRUCT& data,
                                                IFR_Length *lengthindicator,
                                                IFR_ConnectionItem& clink,
                                                IFRConversion_Putval *pv)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_DateTimeConverter, translateInput);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED);
    DBUG_RETURN(IFR_NOT_OK);
}


//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_DateTimeConverter::translateOutput(IFRPacket_DataPart& part,
                                                 SQL_TIME_STRUCT& data,
                                                 IFR_Length* lengthindicator,
                                                 IFR_ConnectionItem& clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_DateTimeConverter, translateOutput);
    SQL_TIMESTAMP_STRUCT tmp_timestamp;
    IFR_Length             tmp_lengthindicator;
    // MSVC requires special scope handling here
    IFR_Retcode rc = IFRConversion_ByteCharDataConverter::translateOutput
        (part, tmp_timestamp, &tmp_lengthindicator, clink);
    if(rc == IFR_OK) {
        data.hour=tmp_timestamp.hour;
        data.minute=tmp_timestamp.minute;
        data.second=tmp_timestamp.second;
        if(lengthindicator)
            *lengthindicator=sizeof(SQL_TIME_STRUCT);
    }
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_DateTimeConverter::translateOutput(IFRPacket_DataPart& part,
                                                 SQL_DATE_STRUCT& data,
                                                 IFR_Length*        lengthindicator,
                                                 IFR_ConnectionItem& clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_DateTimeConverter, translateOutput);
    SQL_TIMESTAMP_STRUCT tmp_timestamp;
    IFR_Length             tmp_lengthindicator;
    // MSVC requires special scope handling here
    IFR_Retcode rc=
        IFRConversion_ByteCharDataConverter::translateOutput(part, tmp_timestamp, &tmp_lengthindicator, clink);
    if(rc == IFR_OK) {
        data.year=tmp_timestamp.year;
        data.month=tmp_timestamp.month;
        data.day=tmp_timestamp.day;
        if(lengthindicator)
            *lengthindicator=sizeof(SQL_DATE_STRUCT);
    }
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
#define TRANSLATE_METHOD_IMPL(xxtype)                                   \
IFR_Retcode                                                             \
IFRConversion_DateTimeConverter::translateInput(IFRPacket_DataPart& part, \
                                            xxtype& data,               \
                                            IFR_Length *lengthindicator, \
                                                IFR_ConnectionItem& clink, \
                                            IFRConversion_Putval *pv)   \
{                                                                       \
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)getIndex()); \
    return IFR_NOT_OK;                                                  \
}                                                                       \
IFR_Retcode                                                             \
IFRConversion_DateTimeConverter::translateOutput(IFRPacket_DataPart& part, \
                                            xxtype& data,               \
                                            IFR_Length *lengthindicator, \
                                            IFR_ConnectionItem& clink)  \
{                                                                       \
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)getIndex()); \
    return IFR_NOT_OK;                                                  \
}

TRANSLATE_METHOD_IMPL(IFR_Int1)
TRANSLATE_METHOD_IMPL(IFR_Int2)
TRANSLATE_METHOD_IMPL(IFR_Int4)
TRANSLATE_METHOD_IMPL(IFR_Int8)
TRANSLATE_METHOD_IMPL(IFR_UInt1)
TRANSLATE_METHOD_IMPL(IFR_UInt2)
TRANSLATE_METHOD_IMPL(IFR_UInt4)
TRANSLATE_METHOD_IMPL(IFR_UInt8)
TRANSLATE_METHOD_IMPL(double)
TRANSLATE_METHOD_IMPL(float)
TRANSLATE_METHOD_IMPL(SQL_NUMERIC_STRUCT)
TRANSLATE_METHOD_IMPL(GUID)

