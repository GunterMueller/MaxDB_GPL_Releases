/*!
  @file           IFRConversion_DateConverter.cpp
  @author         D039759
  @ingroup        IFR_DataConv
  @brief          Conversion from/to Date values
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
#include "Interfaces/Runtime/Conversion/IFRConversion_DateUCS2Converter.h"

#define DBUG_CLINK_METHOD_ENTER(x,y) DBUG_CONTEXT_METHOD_ENTER(x, y, &clink)

//----------------------------------------------------------------------
IFRConversion_DateUCS2Converter::IFRConversion_DateUCS2Converter(IFR_ShortInfo& shortinfo,
                                                                 SAPDBMem_IRawAllocator& allocator,
                                                                 IFR_Connection& connection)
:IFRConversion_UCS2CharDataConverter(shortinfo, allocator, false, false)
{
    m_flags.oraclenull = 0;
}

//----------------------------------------------------------------------
IFRConversion_DateUCS2Converter::~IFRConversion_DateUCS2Converter()
{}

//----------------------------------------------------------------------
IFR_Retcode 
IFRConversion_DateUCS2Converter::appendAsciiInput(IFRPacket_DataPart& datapart,
                                              char               *data,
                                              IFR_Length            datalength,
                                              IFR_Length*           lengthindicator,
                                              IFR_Bool              terminate,
                                              IFR_Bool              ascii7bit,
                                              IFR_ConnectionItem &clink,
                                              IFR_Length&         offset,
                                              IFRConversion_Putval *putval)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_DateUCS2Converter, appendAsciiInput);
    DBUG_PRINT(datalength);
    DBUG_PRINT(lengthindicator);
    DBUG_PRINT(offset);

    IFR_Length byteslength;
    if(IFRConversion_InputDataLength(datalength, lengthindicator, data, byteslength, terminate)) {
        clink.error().setRuntimeError(IFR_ERR_INVALID_LENGTHINDICATOR_I, (IFR_Int4)getIndex());
        DBUG_RETURN(IFR_NOT_OK);
    }
    
    // check for an ODBC date literal
    if(byteslength > 4) {
        if(data[0]=='{' &&
           data[1]=='d' &&
           data[2]==' ' &&
           data[byteslength-1]=='}') {
            data += 3;
            byteslength -=4;

            while(byteslength && data[0] == ' ') {
                ++data;
                --byteslength;
            }
            
            while(byteslength && data[byteslength - 1]==' ') {
                --byteslength;
            }
            
        }
    }
    
    DBUG_RETURN(IFRConversion_UCS2CharDataConverter::appendAsciiInput(datapart, data, datalength, &byteslength,
                                                                      false,
                                                                      ascii7bit, clink, offset, putval));
}

//----------------------------------------------------------------------
IFR_Retcode 
IFRConversion_DateUCS2Converter::appendUCS2Input(IFRPacket_DataPart&  datapart,
                                             char                *data,
                                             IFR_Bool            swapped,
                                             IFR_Length           datalength,
                                             IFR_Length*          lengthindicator,
                                             IFR_Bool             terminate,
                                             IFR_ConnectionItem  &clink,
                                             IFR_Length&          offset,
                                             IFRConversion_Putval *putval)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_UCS2CharDataConverter, appendUCS2Input);
    
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

    if(byteslength >= 8) {
        if(swapped) {
            if(data[1] == 0 && data[0]=='{' &&
               data[3] == 0 && data[2]=='d' &&
               data[5] == 0 && data[4]==' ' &&
               data[byteslength-1] == '}' &&
               data[byteslength-2] == 0) {
                data += 6;
                byteslength -= 8;

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
               data[2] == 0 && data[3]=='d' &&
               data[4] == 0 && data[5]==' ' &&
               data[byteslength-1] == '}' &&
               data[byteslength-2] == 0) {
                data += 6;
                byteslength -= 8;
                
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
    DBUG_RETURN(IFRConversion_UCS2CharDataConverter::appendUCS2Input(datapart, data, swapped, 
                                                                     datalength, &byteslength,
                                                                     false, // already handled
                                                                     clink, offset, putval));
}

//----------------------------------------------------------------------
IFR_Retcode 
IFRConversion_DateUCS2Converter::appendUTF8Input(IFRPacket_DataPart& datapart,
                                             char               *data,
                                             IFR_Length            datalength,
                                             IFR_Length*           lengthindicator,
                                             IFR_Bool              terminate,
                                             IFR_ConnectionItem &clink,
                                             IFR_Length&         offset,
                                             IFRConversion_Putval *putval)
{
    // identical for ASCII
    return appendAsciiInput(datapart, data, datalength, lengthindicator, terminate, false, clink, offset, putval);
}



//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_DateUCS2Converter::appendBinaryInput(IFRPacket_DataPart& datapart,
                                                    char               *data,
                                                    IFR_Length            datalength,
                                                    IFR_Length*           lengthindicator,
                                                    IFR_ConnectionItem & clink,
                                                    IFR_Length         &offset,
                                                    IFRConversion_Putval *putval)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_DateUCS2Converter, appendBinaryInput);
    DBUG_RETURN(IFRConversion_UCS2CharDataConverter::appendAsciiInput(datapart, data, datalength, lengthindicator, 
                                                                      true, // compatibility
                                                                      false, clink, offset, putval));
}

//----------------------------------------------------------------------
IFR_Retcode 
IFRConversion_DateUCS2Converter::appendBinaryOutput(IFRPacket_DataPart&   datapart,
                                                char                 *data,
                                                IFR_Length            datalength,
                                                IFR_Length*           lengthindicator,
                                                IFR_ConnectionItem   &clink,
                                                IFR_Length&           dataoffset,
                                                IFR_Length&           offset,
                                                IFRConversion_Getval *getval)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_DateUCS2Converter, appendBinaryOutput);
    DBUG_RETURN(IFRConversion_UCS2CharDataConverter::appendAsciiOutput(datapart, 
                                                                       data,
                                                                       datalength,
                                                                       lengthindicator,
                                                                       false,
                                                                       false,
                                                                       clink,
                                                                       dataoffset,
                                                                       offset,
                                                                       getval));
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_DateUCS2Converter::translateInput(IFRPacket_DataPart& part,
                                            SQL_TIME_STRUCT& data,
                                            IFR_Length *lengthindicator,
                                                IFR_ConnectionItem& clink,
                                                IFRConversion_Putval *pv)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_DateUCS2Converter, translateInput);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)getIndex());
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_DateUCS2Converter::translateInput(IFRPacket_DataPart& part,
                                                SQL_TIMESTAMP_STRUCT& data,
                                                IFR_Length *lengthindicator,
                                                IFR_ConnectionItem& clink,
                                                IFRConversion_Putval *pv)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_DateUCS2Converter, translateInput);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)getIndex());
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_DateUCS2Converter::translateOutput(IFRPacket_DataPart& part,
                                             SQL_TIME_STRUCT& data,
                                             IFR_Length *lengthindicator,
                                             IFR_ConnectionItem& clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_DateUCS2Converter, translateInput);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)getIndex());
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_DateUCS2Converter::translateOutput(IFRPacket_DataPart& part,
                                             SQL_TIMESTAMP_STRUCT& data,
                                             IFR_Length *lengthindicator,
                                             IFR_ConnectionItem& clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_DateUCS2Converter, translateInput);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)getIndex());
    DBUG_RETURN(IFR_NOT_OK);
}

#define TRANSLATE_METHOD_IMPL(xxtype)                                   \
IFR_Retcode                                                             \
IFRConversion_DateUCS2Converter::translateInput(IFRPacket_DataPart& part, \
                                            xxtype& data,               \
                                            IFR_Length *lengthindicator, \
                                            IFR_ConnectionItem& clink,  \
                                            IFRConversion_Putval *pv)   \
{                                                                       \
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)getIndex()); \
    return IFR_NOT_OK;                                                  \
}                                                                       \
IFR_Retcode                                                             \
IFRConversion_DateUCS2Converter::translateOutput(IFRPacket_DataPart& part, \
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

