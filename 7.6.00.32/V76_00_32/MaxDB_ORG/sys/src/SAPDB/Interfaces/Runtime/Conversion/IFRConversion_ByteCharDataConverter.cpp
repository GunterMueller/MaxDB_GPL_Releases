/*!
  @file           IFRConversion_ByteCharDataConverter.cpp
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
#include "Interfaces/Runtime/IFR_Connection.h"
#include "Interfaces/Runtime/Conversion/IFRConversion_ByteCharDataConverter.h"
#include "Interfaces/Runtime/Util/IFRUtil_SQLNumeric.h"
#include "Interfaces/Runtime/Util/IFRUtil_BinaryToHex.h"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_Alloca.h"
#include "Interfaces/Runtime/Conversion/IFRConversion_ToString.h"
#include "Interfaces/Runtime/Conversion/IFRConversion_FromString.h"
#include "Interfaces/Runtime/Util/IFRUtil_DateTime.h"
#include "hsp83.h"

#include <errno.h>
#include <ctype.h>
#include <stdlib.h>

#if defined(NMP) && !defined(BIT64)
extern "C" long long strtoll (const char *, char**, int);
extern "C" unsigned long long strtoull (const char *, char**, int);
#endif

#if defined(HPUX) && !defined(BIT64)
extern "C" intmax_t __strtoll (const char *, char**, int);
extern "C" uintmax_t __strtoull (const char *, char**, int);
#  define strtoull __strtoull
#  define strtoll  __strtoll
#endif

#define DBUG_CLINK_METHOD_ENTER(x,y) DBUG_CONTEXT_METHOD_ENTER(x, y, &clink)

//----------------------------------------------------------------------
IFRConversion_ByteCharDataConverter
::IFRConversion_ByteCharDataConverter(IFR_ShortInfo &shortinfo,
                                      SAPDBMem_IRawAllocator& allocator,
                                      IFR_Bool istrimming,
                                      IFR_Bool integerconversion,
                                      IFR_Bool oraclenull)
:IFRConversion_Converter(shortinfo, allocator)
{
    m_flags.trimming          = (istrimming) ? 1 : 0 ;
    m_flags.numbers           = (integerconversion) ? 1 : 0;
    m_flags.bin2hex           = 0;
    m_flags.oraclenull        = oraclenull;
    m_partencoding = IFR_StringEncodingAscii;
}

//----------------------------------------------------------------------
IFRConversion_ByteCharDataConverter::~IFRConversion_ByteCharDataConverter()
{
}


//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter
::translateBinaryInput(IFRPacket_DataPart& datapart,
                       char               *data,
                       IFR_Length            datalength,
                       IFR_Length*           lengthindicator,
                       IFR_ConnectionItem & clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_Converter, translateBinaryInput);
    IFR_Length offset = 0;
    DBUG_RETURN(appendBinaryInput(datapart, data, datalength, lengthindicator, clink, offset, 0));
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter
::appendBinaryInput(IFRPacket_DataPart& datapart,
                    char               *data,
                    IFR_Length            datalength,
                    IFR_Length*           lengthindicator,
                    IFR_ConnectionItem & clink,
                    IFR_Length         &offset,
                    IFRConversion_Putval *putval)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_ByteCharDataConverter, appendBinaryInput);

    IFR_Length byteslength;
    if(IFRConversion_InputDataLength(datalength, lengthindicator, data, byteslength, true /* old behaviour is 'true' */)) {
        clink.error().setRuntimeError(IFR_ERR_INVALID_LENGTHINDICATOR_I, (IFR_Int4)getIndex());
        DBUG_RETURN(IFR_NOT_OK);
    }

    IFR_Retcode rc;
    IFR_Length old_offset=offset;
    
    
    if(byteslength == 0 && m_flags.oraclenull && offset == 0) {
        DBUG_RETURN(translateNullInput(datapart, clink));
    }

    if(offset == 0) {
        offset=byteslength;
        rc = datapart.addBinaryParameter(data, byteslength, m_shortinfo);
    } else {
        rc = datapart.appendBinaryToParameter(data, byteslength, m_shortinfo, offset);
    }

    switch(rc) {
    case IFR_NOT_OK:
        clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)getIndex());
        DBUG_RETURN(rc);
    case IFR_OK:
        break;
    case IFR_DATA_TRUNC:
        offset = m_shortinfo.iolength - 1;
        if(m_flags.trimming) {
            if(offset >= old_offset + IFRConversion_StringPadLength(data, byteslength, m_shortinfo.getPaddingCharacter())) {
                rc = IFR_OK;
                break;
            }
        }
        rc = IFR_NOT_OK;
    default:
        clink.error().setRuntimeError(IFR_ERR_PARAM_CONVERSION_TRUNCATEDATA_I, (IFR_Int4)getIndex());
    }
    
    DBUG_RETURN(rc);
}




//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter
::translateAsciiInput(IFRPacket_DataPart& datapart,
                      char               *data,
                      IFR_Length            datalength,
                      IFR_Length*           lengthindicator,
                      IFR_Bool              terminate,
                      IFR_Bool              ascii7bit,
                      IFR_ConnectionItem &clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_ByteCharDataConverter, translateAsciiInput);
    IFR_Length  offset = 0;
    DBUG_RETURN(appendAsciiInput(datapart, data, datalength, lengthindicator, terminate, ascii7bit, clink, offset, 0));
}


//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter
::appendAsciiInput(IFRPacket_DataPart& datapart,
                   char               *data,
                   IFR_Length          datalength,
                   IFR_Length*         lengthindicator,
                   IFR_Bool            terminate,
                   IFR_Bool            ascii7bit,
                   IFR_ConnectionItem &clink,
                   IFR_Length&         offset,
                   IFRConversion_Putval *putval)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_ByteCharDataConverter, appendAsciiInput);
    DBUG_PRINT(datalength);
    DBUG_PRINT(lengthindicator);
    DBUG_PRINT(offset);
    DBUG_PRINT(terminate);

    IFR_Length byteslength;
    if(IFRConversion_InputDataLength(datalength, lengthindicator, data, byteslength, terminate)) {
        clink.error().setRuntimeError(IFR_ERR_INVALID_LENGTHINDICATOR_I, (IFR_Int4)getIndex());
        DBUG_RETURN(IFR_NOT_OK);
    }
    IFR_Retcode rc;
    IFR_Length old_offset=offset;
    
    if(ascii7bit) {
        if(!IFRConversion_StringIsAscii7((unsigned char *)data, byteslength)) {
            clink.error().setRuntimeError(IFR_ERR_NOT_ASCII_CHARACTERS_I, (IFR_Int4)getIndex());
            DBUG_RETURN(IFR_NOT_OK);
        }
    }

    if(byteslength == 0 && m_flags.oraclenull && offset == 0) {
        DBUG_RETURN(translateNullInput(datapart, clink));
    }
    
    if(offset == 0) {
        offset=byteslength;
        rc = datapart.addParameter(data, byteslength, IFR_StringEncodingAscii, IFR_StringEncodingAscii, m_shortinfo, 
                                   m_flags.bin2hex==1?true:false);
    } else {
        rc = datapart.appendToParameter(data, byteslength, IFR_StringEncodingAscii, IFR_StringEncodingAscii, 
                                        m_shortinfo, offset, 
                                        m_flags.bin2hex==1?true:false);
    }
    
        
    switch(rc) {
    case IFR_NOT_OK:
        clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)getIndex());
        DBUG_RETURN(rc);
    case IFR_OK:
        break;
    case IFR_DATA_TRUNC:
        offset = m_shortinfo.iolength - 1;
        if(m_flags.trimming) {
            if(offset >= old_offset + IFRConversion_StringPadLength(data, byteslength, m_shortinfo.getPaddingCharacter())) {
                rc = IFR_OK;
                break;
            }
        }
        rc = IFR_NOT_OK;
    default:
        clink.error().setRuntimeError(IFR_ERR_PARAM_CONVERSION_TRUNCATEDATA_I, (IFR_Int4)getIndex());
    }
    
    
    DBUG_RETURN(rc);
}



IFR_Retcode
IFRConversion_ByteCharDataConverter
::translateUCS2Input(IFRPacket_DataPart& datapart,
                     char               *data,
                     IFR_Bool            swapped,
                     IFR_Length            datalength,
                     IFR_Length*           lengthindicator,
                     IFR_Bool              terminate,
                     IFR_ConnectionItem &clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_ByteCharDataConverter, translateUCS2Input);
    IFR_Length offset = 0;
    DBUG_RETURN(appendUCS2Input(datapart, data, swapped, datalength, lengthindicator, terminate, clink, offset, 0));
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter
::appendUCS2Input(IFRPacket_DataPart& datapart,
                     char               *data,
                     IFR_Bool            swapped,
                     IFR_Length            datalength,
                     IFR_Length*           lengthindicator,
                     IFR_Bool              terminate,
                     IFR_ConnectionItem &clink,
                     IFR_Length&         offset,
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

    if(byteslength == 0 && m_flags.oraclenull && offset == 0) {
        DBUG_RETURN(translateNullInput(datapart, clink));
    }
    
    IFR_Retcode rc = IFR_OK;
    IFR_Length old_offset = offset;
    if(offset == 0) {
        offset = byteslength;
        rc = datapart.addParameter(data,
                                   byteslength,
                                   swapped?IFR_StringEncodingUCS2Swapped:IFR_StringEncodingUCS2,
                                   IFR_StringEncodingAscii,
                                   m_shortinfo,
                                   m_flags.bin2hex==1?true:false);
    } else {
        rc = datapart.appendToParameter(data,
                                        byteslength,
                                        swapped?IFR_StringEncodingUCS2Swapped:IFR_StringEncodingUCS2,
                                        IFR_StringEncodingAscii,
                                        m_shortinfo,
                                        offset,
                                        m_flags.bin2hex==1?true:false);
    }
    switch(rc) {
    case IFR_NOT_OK:
        clink.error().setRuntimeError(IFR_ERR_CORRUPTED_UCS2ASCIIDATA_I, (IFR_Int4)getIndex());
        break;
    case IFR_OK:
        break;
    case IFR_DATA_TRUNC: 
        offset = (m_shortinfo.iolength - 1) * 2; // iolength is bytes in ascii, needed is ucs2
        if(m_flags.trimming) {
            if(offset >= old_offset + IFRConversion_StringPadLengthUCS2(data, byteslength, 
                                                                        m_shortinfo.getPaddingCharacter(), swapped)) {
                rc = IFR_OK;
                break;
            }
        }
    default:
        rc = IFR_NOT_OK;
        clink.error().setRuntimeError(IFR_ERR_PARAM_CONVERSION_TRUNCATEDATA_I, (IFR_Int4)getIndex());
    }
    DBUG_RETURN(rc);
}


//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter
::translateUTF8Input(IFRPacket_DataPart& datapart,
                     char               *data,
                     IFR_Length            datalength,
                     IFR_Length*           lengthindicator,
                     IFR_Bool              terminate,
                     IFR_ConnectionItem &clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_ByteCharDataConverter, translateUTF8Input);
    IFR_Length offset = 0;
    DBUG_RETURN(appendUTF8Input(datapart, data, datalength, lengthindicator, terminate, clink, offset, 0));
}


//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter
::appendUTF8Input(IFRPacket_DataPart& datapart,
                  char               *data,
                  IFR_Length            datalength,
                  IFR_Length*           lengthindicator,
                  IFR_Bool              terminate,
                  IFR_ConnectionItem &clink,
                  IFR_Length&         offset,
                  IFRConversion_Putval *putval)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_ByteCharDataConverter, appendUTF8Input);
    
    IFR_Length byteslength;
    if(IFRConversion_InputDataLength(datalength, lengthindicator, data, byteslength, terminate)) {
        clink.error().setRuntimeError(IFR_ERR_INVALID_LENGTHINDICATOR_I, (IFR_Int4)getIndex());
        DBUG_RETURN(IFR_NOT_OK);
    }

    if(byteslength == 0 && m_flags.oraclenull && offset == 0) {
        DBUG_RETURN(translateNullInput(datapart, clink));
    }

    unsigned int bytecount = 0;
    unsigned int charcount = 0;
    int terminated = 0;
    int corrupted = 0;
    int exhausted = 0;
    sp83UTF8StringInfo(data,
                       byteslength,
                       1,
                       &charcount,
                       &bytecount,
                       &terminated,
                       &corrupted,
                       &exhausted);
    if(exhausted || corrupted) {
        clink.error().setRuntimeError(IFR_ERR_CORRUPTED_UTF8DATA_I, (IFR_Int4)getIndex());
        DBUG_RETURN(IFR_NOT_OK);
    }

    char *tmp_data = (char *)alloca(charcount + 1);
    unsigned int replacecount;
    unsigned int bytecount_ascii;
    unsigned int scrBytesParsed;
    tsp83UTF8_ConversionResult r=
        sp83UTF8toASCII((const unsigned char*)data,
                        charcount,
                        &scrBytesParsed,
                        tmp_data,
                        charcount,
                        '?',
                        &replacecount,
                        &bytecount_ascii);

    if(r != sp83UTF8Convert_Success) {
        clink.error().setRuntimeError(IFR_ERR_CORRUPTED_UTF8DATA_I, (IFR_Int4)getIndex());
        DBUG_RETURN(IFR_NOT_OK);
    }
    
    // and now the insert itself ...
    IFR_Length   tmp_datalength=charcount;
    IFR_Length tmp_lengthindicator=charcount;

    IFR_Retcode rc=appendAsciiInput(datapart, tmp_data, tmp_datalength,
                                    &tmp_lengthindicator, false, false, clink, offset, putval);
    //                                                    ^- length is already determined

    DBUG_RETURN(rc);
}


//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter::translateInput(IFRPacket_DataPart& datapart,
                                                    IFR_Int1&           data,
                                                    IFR_Length*           lengthindicator,
                                                    IFR_ConnectionItem& clink,
                                                    IFRConversion_Putval *pv)

{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_ByteCharDataConverter, translateInput_Int1);
    if(!m_flags.numbers) {
        clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    }
    
    IFR_Int4 tmp_data = (IFR_Int4)data;
    IFRConversion_ToString<IFR_Int4>::BufferType buffer;
    IFRConversion_ToString<IFR_Int4>  cnv;
    IFR_size_t charsWritten;
    IFR_Retcode rc = cnv.convert(tmp_data, buffer, charsWritten, clink.error());
    if(rc != IFR_OK) {
        DBUG_RETURN(rc);
    }
    DBUG_PRINTS(data(IFR_Int1), buffer);
    rc = moveDataToPart(datapart, buffer, charsWritten, clink.error());
    if(rc == IFR_DATA_TRUNC) {
      clink.error().setRuntimeError(IFR_ERR_PARAM_CONVERSION_LOOSEDIGITS_I, (IFR_Int4)this->m_index);
      DBUG_RETURN(IFR_NOT_OK);
    }
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter::translateInput(IFRPacket_DataPart& datapart,
                                                    IFR_UInt1&           data,
                                                    IFR_Length*           lengthindicator,
                                                    IFR_ConnectionItem& clink,
                                                    IFRConversion_Putval *pv)

{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_ByteCharDataConverter, translateInput_UInt1);

    if(!m_flags.numbers) {
        clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    }
    
    IFR_UInt4 tmp_data = (IFR_UInt4)data;
    IFRConversion_ToString<IFR_UInt4>::BufferType buffer;
    IFRConversion_ToString<IFR_UInt4>  cnv;
    IFR_size_t charsWritten;
    IFR_Retcode rc = cnv.convert(tmp_data, buffer, charsWritten, clink.error());
    if(rc != IFR_OK) {
        DBUG_RETURN(rc);
    }
    DBUG_PRINTS(data(IFR_UInt1), buffer);
    rc = moveDataToPart(datapart, buffer, charsWritten, clink.error());
    if(rc == IFR_DATA_TRUNC) {
        clink.error().setRuntimeError(IFR_ERR_PARAM_CONVERSION_LOOSEDIGITS_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    }

    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter::translateInput(IFRPacket_DataPart& datapart,
                                                    IFR_Int2&           data,
                                                    IFR_Length*           lengthindicator,
                                                    IFR_ConnectionItem& clink,
                                                    IFRConversion_Putval *pv)

{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_ByteCharDataConverter, translateInput_Int2);

    if(!m_flags.numbers) {
        clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    }

    IFRConversion_ToString<IFR_Int2>::BufferType buffer;
    IFRConversion_ToString<IFR_Int2>  cnv;
    IFR_size_t charsWritten;
    IFR_Retcode rc = cnv.convert(data, buffer, charsWritten, clink.error());
    if(rc != IFR_OK) {
        DBUG_RETURN(rc);
    }
    DBUG_PRINTS(data(IFR_Int2), buffer);
    rc = moveDataToPart(datapart, buffer, charsWritten, clink.error());
    if(rc == IFR_DATA_TRUNC) {
        clink.error().setRuntimeError(IFR_ERR_PARAM_CONVERSION_LOOSEDIGITS_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    }

    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter::translateInput(IFRPacket_DataPart& datapart,
                                                    IFR_UInt2&           data,
                                                    IFR_Length*           lengthindicator,
                                                    IFR_ConnectionItem& clink,
                                                    IFRConversion_Putval *pv)

{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_ByteCharDataConverter, translateInput_UInt2);

    if(!m_flags.numbers) {
        clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    }

    IFRConversion_ToString<IFR_UInt2>::BufferType buffer;
    IFRConversion_ToString<IFR_UInt2>  cnv;
    IFR_size_t charsWritten;
    IFR_Retcode rc = cnv.convert(data, buffer, charsWritten, clink.error());
    if(rc != IFR_OK) {
        DBUG_RETURN(rc);
    }
    DBUG_PRINTS(data(IFR_UInt2), buffer);
    rc = moveDataToPart(datapart, buffer, charsWritten, clink.error());
    if(rc == IFR_DATA_TRUNC) {
        clink.error().setRuntimeError(IFR_ERR_PARAM_CONVERSION_LOOSEDIGITS_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    }

    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter::translateInput(IFRPacket_DataPart& datapart,
                                                    IFR_Int4&           data,
                                                    IFR_Length*           lengthindicator,
                                                    IFR_ConnectionItem& clink,
                                                    IFRConversion_Putval *pv)

{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_ByteCharDataConverter, translateInput_Int4);

    if(!m_flags.numbers) {
        clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    }

    IFRConversion_ToString<IFR_Int4>::BufferType buffer;
    IFRConversion_ToString<IFR_Int4>  cnv;
    IFR_size_t charsWritten;
    IFR_Retcode rc = cnv.convert(data, buffer, charsWritten, clink.error());
    if(rc != IFR_OK) {
        DBUG_RETURN(rc);
    }
    DBUG_PRINTS(data(IFR_Int4), buffer);
    rc = moveDataToPart(datapart, buffer, charsWritten, clink.error());
    if(rc == IFR_DATA_TRUNC) {
        clink.error().setRuntimeError(IFR_ERR_PARAM_CONVERSION_LOOSEDIGITS_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    }

    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter::translateInput(IFRPacket_DataPart& datapart,
                                                    IFR_UInt4&           data,
                                                    IFR_Length*           lengthindicator,
                                                    IFR_ConnectionItem& clink,
                                                    IFRConversion_Putval *pv)

{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_ByteCharDataConverter, translateInput_UInt4);

    if(!m_flags.numbers) {
        clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    }

    IFRConversion_ToString<IFR_UInt4>::BufferType buffer;
    IFRConversion_ToString<IFR_UInt4>  cnv;
    IFR_size_t charsWritten;
    IFR_Retcode rc = cnv.convert(data, buffer, charsWritten, clink.error());
    if(rc != IFR_OK) {
        DBUG_RETURN(rc);
    }
    DBUG_PRINTS(data(IFR_UInt4), buffer);

    rc = moveDataToPart(datapart, buffer, charsWritten, clink.error());
    if(rc == IFR_DATA_TRUNC) {
        clink.error().setRuntimeError(IFR_ERR_PARAM_CONVERSION_LOOSEDIGITS_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    }

    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter::translateInput(IFRPacket_DataPart& datapart,
                                                    IFR_Int8&           data,
                                                    IFR_Length*           lengthindicator,
                                                    IFR_ConnectionItem& clink,
                                                    IFRConversion_Putval *pv)

{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_ByteCharDataConverter, translateInput_Int8);

    if(!m_flags.numbers) {
        clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    }


    IFRConversion_ToString<IFR_Int8>::BufferType buffer;
    IFRConversion_ToString<IFR_Int8>  cnv;
    IFR_size_t charsWritten;
    IFR_Retcode rc = cnv.convert(data, buffer, charsWritten, clink.error());
    if(rc != IFR_OK) {
        DBUG_RETURN(rc);
    }
    DBUG_PRINTS(data(IFR_Int8), buffer);
    rc = moveDataToPart(datapart, buffer, charsWritten, clink.error());

    if(rc == IFR_DATA_TRUNC) {
        clink.error().setRuntimeError(IFR_ERR_PARAM_CONVERSION_LOOSEDIGITS_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    }

    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter::translateInput(IFRPacket_DataPart& datapart,
                                                    IFR_UInt8&           data,
                                                    IFR_Length*           lengthindicator,
                                                    IFR_ConnectionItem& clink,
                                                    IFRConversion_Putval *pv)

{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_ByteCharDataConverter, translateInput_UInt8);

    if(!m_flags.numbers) {
        clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    }

    IFRConversion_ToString<IFR_UInt8>::BufferType buffer;
    IFRConversion_ToString<IFR_UInt8>  cnv;
    IFR_size_t charsWritten;
    IFR_Retcode rc = cnv.convert(data, buffer, charsWritten, clink.error());
    if(rc != IFR_OK) {
        DBUG_RETURN(rc);
    }

    DBUG_PRINTS(data(IFR_UInt8), buffer);
    rc = moveDataToPart(datapart, buffer, charsWritten, clink.error());
    if(rc == IFR_DATA_TRUNC) {
        clink.error().setRuntimeError(IFR_ERR_PARAM_CONVERSION_LOOSEDIGITS_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    }

    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter::moveDataToPart(IFRPacket_DataPart& datapart,
                                                    char *buffer,
                                                    IFR_size_t bufferlength,
                                                    IFR_ErrorHndl &error )
{
    // prevent an overflow in this case
    IFR_Int2 used_bufferlength=
        (bufferlength > MAX_IFR_INT2) ? MAX_IFR_INT2 : (IFR_Int2)bufferlength;
    IFR_Retcode rc = datapart.addParameter(buffer, used_bufferlength, IFR_StringEncodingAscii, m_partencoding, m_shortinfo, false);
    if (rc == IFR_NOT_OK)
        error.setRuntimeError(IFR_ERR_CORRUPTED_UCS2ASCIIDATA_I, (IFR_Int4)getIndex());
    return rc;
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter::translateInput(IFRPacket_DataPart& datapart,
                                                    double&             data,
                                                    IFR_Length*           lengthindicator,
                                                    IFR_ConnectionItem& clink,
                                                    IFRConversion_Putval *pv)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_ByteCharDataConverter, translateInput_double);
    
    if(!m_flags.numbers) {
        clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    }
    
    IFRConversion_ToString<double>::BufferType buf;
    IFRConversion_ToString<double>  cnv;
    IFR_size_t charsWritten;

    IFR_Retcode rc = cnv.convert(data, buf, charsWritten, clink.error());
    if(rc != IFR_OK) {
        DBUG_RETURN(rc);
    }
    rc = moveDataToPart(datapart, buf, charsWritten, clink.error());
    if(rc == IFR_DATA_TRUNC) {
        clink.error().setRuntimeError(IFR_ERR_PARAM_CONVERSION_LOOSEDIGITS_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    }
    
    
    
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter::translateInput(IFRPacket_DataPart& datapart,
                                                    float&             data,
                                                    IFR_Length*          lengthindicator,
                                                    IFR_ConnectionItem& clink,
                                                    IFRConversion_Putval *pv)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_ByteCharDataConverter, translateInput_float);
    double d_data = (double)data;
    DBUG_RETURN(translateInput(datapart, d_data, lengthindicator, clink, pv));
}


//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter::translateInput(IFRPacket_DataPart& part,
                                                    SQL_DATE_STRUCT&    data,
                                                    IFR_Length*           lengthindicator,
                                                    IFR_ConnectionItem& clink,
                                                    IFRConversion_Putval *pv)

{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_ByteCharDataConverter, translateInput_DATE);

    IFRConversion_ToString<SQL_DATE_STRUCT>::BufferType buffer;
    IFR_size_t charsWritten;
    IFRConversion_ToString<SQL_DATE_STRUCT> cnv(m_index, clink.getConnection()->getDateTimeFormat());

    IFR_Retcode rc = cnv.convert(data, buffer, charsWritten, clink.error()); 
    if(rc != IFR_OK) {
        DBUG_RETURN(rc);
    }
    if(moveDataToPart(part, buffer, charsWritten, clink.error()) == IFR_DATA_TRUNC) {
        clink.error().setRuntimeError(IFR_ERR_PARAM_CONVERSION_TRUNCATEDATA_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    }

    DBUG_RETURN(IFR_OK);
}


//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter::translateInput(IFRPacket_DataPart& part,
                                                    SQL_TIME_STRUCT&    data,
                                                    IFR_Length*           lengthindicator,
                                                    IFR_ConnectionItem& clink,
                                                    IFRConversion_Putval *pv)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_ByteCharDataConverter, translateInput_TIME);


    IFRConversion_ToString<SQL_TIME_STRUCT>::BufferType buffer;
    IFR_size_t charsWritten;
    IFRConversion_ToString<SQL_TIME_STRUCT> cnv(m_index, 
                                                clink.getConnection()->getDateTimeFormat());
    
    IFR_Retcode rc = cnv.convert(data, buffer, charsWritten, clink.error());
    if(rc != IFR_OK) {
        DBUG_RETURN(rc);
    }
    
    if(moveDataToPart(part, buffer, charsWritten, clink.error()) == IFR_DATA_TRUNC) {
        clink.error().setRuntimeError(IFR_ERR_PARAM_CONVERSION_TRUNCATEDATA_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    }
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter::translateInput(IFRPacket_DataPart&   part,
                                                    SQL_TIMESTAMP_STRUCT& data,
                                                    IFR_Length*             lengthindicator,
                                                    IFR_ConnectionItem&   clink,
                                                    IFRConversion_Putval *pv)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_ByteCharDataConverter, translateInput_TIMESTAMP);

    IFRConversion_ToString<SQL_TIMESTAMP_STRUCT>::BufferType buffer;
    IFR_size_t charsWritten;
    IFRConversion_ToString<SQL_TIMESTAMP_STRUCT> cnv(m_index, clink.getConnection()->getDateTimeFormat());
    
    IFR_Retcode rc = cnv.convert(data, buffer, charsWritten, clink.error());
    if(rc != IFR_OK) {
        DBUG_RETURN(rc);
    }
    if(moveDataToPart(part, buffer, charsWritten, clink.error()) == IFR_DATA_TRUNC) {
        clink.error().setRuntimeError(IFR_ERR_PARAM_CONVERSION_TRUNCATEDATA_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    }

    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter::translateInput(IFRPacket_DataPart&   part,
                                                    SQL_NUMERIC_STRUCT&   data,
                                                    IFR_Length*           lengthindicator,
                                                    IFR_ConnectionItem&   clink,
                                                    IFRConversion_Putval *pv)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_ByteCharDataConverter, translateInput_NUMERIC);

    if(!m_flags.numbers) {
        clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    }
    char buffer[132];
    IFR_size_t bufferlength=132;

    IFR_Retcode rc = IFRUtil_SQLNumeric::numericToAsciiString(data,
                                                              buffer,
                                                              bufferlength);
    if(rc != IFR_OK) {
        clink.error().setRuntimeError(IFR_ERR_PARAM_CONVERSION_TRUNCATEDATA_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(rc);
    }

    DBUG_PRINT_BUFFER(buffer, bufferlength, bufferlength, IFR_StringEncodingAscii);

    char * dotpos = (char *) memchr((void *)buffer, '.', bufferlength);
    if(dotpos == 0) {
        if(m_shortinfo.length < bufferlength) {
            clink.error().setRuntimeError(IFR_ERR_PARAM_CONVERSION_TRUNCATEDATA_I, (IFR_Int4)this->m_index);
            DBUG_RETURN(IFR_NOT_OK);
        }
    } else {
        if(m_shortinfo.length < (dotpos - buffer + 1)) {
            clink.error().setRuntimeError(IFR_ERR_PARAM_CONVERSION_TRUNCATEDATA_I, (IFR_Int4)this->m_index);
            DBUG_RETURN(IFR_NOT_OK);
        }
    }

    rc = moveDataToPart(part, buffer, bufferlength, clink.error());
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter::translateInput(IFRPacket_DataPart&   part,
                                                    GUID&                 data,
                                                    IFR_Length*             lengthindicator,
                                                    IFR_ConnectionItem&   clink,
                                                    IFRConversion_Putval *pv)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_ByteCharDataConverter, translateInput_GUID);

    if(m_shortinfo.length < sizeof(GUID)) {
        clink.error().setRuntimeError(IFR_ERR_PARAM_CONVERSION_TRUNCATEDATA_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    }
    IFR_Length tmp_lengthindicator=sizeof(GUID);
    DBUG_RETURN(translateBinaryInput(part, (char*)&data, sizeof(GUID), &tmp_lengthindicator, clink));
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter::translateAsciiOutput(IFRPacket_DataPart& datapart,
                                                          char *data,
                                                          IFR_Length datalength,
                                                          IFR_Length *lengthindicator,
                                                          IFR_Bool    terminate,
                                                          IFR_Bool    ascii7bit,
                                                          IFR_ConnectionItem& clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_ByteCharDataConverter, translateAsciiOutput);
    IFR_Length  offset     = 1;
    IFR_Length  dataoffset = 0;
    DBUG_RETURN(appendAsciiOutput(datapart, data, datalength, lengthindicator, terminate, ascii7bit, clink, dataoffset, offset, 0));
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter::appendAsciiOutput(IFRPacket_DataPart&   datapart,
                                                       char                 *data,
                                                       IFR_Length            datalength,
                                                       IFR_Length*           lengthindicator,
                                                       IFR_Bool              terminate,
                                                       IFR_Bool              ascii7bit,
                                                       IFR_ConnectionItem   &clink,
                                                       IFR_Length&           dataoffset,
                                                       IFR_Length&           offset,
                                                       IFRConversion_Getval *getval)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_ByteCharDataConverter, appendAsciiOutput);
    
    if(dataoffset != 0) {
        clink.error().setRuntimeError(IFR_ERR_NOT_IMPLEMENTED("output of character data at offset in output buffer"));
        DBUG_RETURN(IFR_NOT_OK);
    }

    char       *read_data   = datapart.getOutputData(m_shortinfo.pos.bufpos);
    IFR_Length  byteslength = m_shortinfo.iolength - 1;

    if(m_flags.trimming) {
        byteslength = IFRConversion_StringPadLength(read_data, byteslength, m_shortinfo.getPaddingCharacter());
    }

    IFR_Length z_offset = offset - 1;
    
    // no data found if not at begin and offset >= length
    if(z_offset && z_offset >= byteslength) {
        DBUG_RETURN(IFR_NO_DATA_FOUND);
    }

    read_data += z_offset;
    byteslength -= z_offset;
    
    if(m_flags.bin2hex) {
        IFR_UInt4 srcbytesparsed;
        IFR_UInt4 destbyteswritten;       
        tsp78ConversionResult convres = IFRUtil_BinaryToHex(IFR_ENCODING(IFR_StringEncodingAscii),
                                                            data,
                                                            datalength,
                                                            &destbyteswritten,
                                                            read_data,
                                                            byteslength,
                                                            &srcbytesparsed);
        
        if(convres != sp78_Ok && convres != sp78_TargetExhausted) {
            clink.error().setRuntimeError(IFR_ERR_BYTE_CONVERSION_FAILED_I, (IFR_Int4)getIndex());
            DBUG_RETURN(IFR_NOT_OK);
        }
        
        if(terminate && datalength) {
            if(destbyteswritten == datalength) {
                // need to subtract 2
                destbyteswritten -= 2;
                --srcbytesparsed;
                convres = sp78_TargetExhausted;
                data[datalength-2] = '\0';
            } else {
                data[destbyteswritten] = '\0';
            }
        }
        
        offset += srcbytesparsed;

        if(lengthindicator) {
            *lengthindicator = byteslength * 2;
        }
        
        if(convres == sp78_Ok) {
            DBUG_RETURN(IFR_OK);
        } else {
            DBUG_RETURN(IFR_DATA_TRUNC);
        }
        
    } else {
        IFR_Length copylength = byteslength > datalength ? datalength : byteslength;
        
        if(ascii7bit) {
            if(!IFRConversion_StringIsAscii7((unsigned char *)read_data, copylength)) {
                clink.error().setRuntimeError(IFR_ERR_NOT_ASCII_CHARACTERS_I, (IFR_Int4)getIndex());
                DBUG_RETURN(IFR_NOT_OK);
            }
        }
        
        
        memcpy(data, read_data, copylength); 
        
        if(terminate && datalength) {
            if(copylength == datalength) {
                --copylength;
                data[copylength] = 0;
            } else {
                data[byteslength] = 0;
            }
        }
        
        offset += copylength;

        if(lengthindicator) {
            *lengthindicator = byteslength;
        }
    
        if(copylength < byteslength) {
            DBUG_RETURN(IFR_DATA_TRUNC);
        } else {
            if(terminate && !datalength) {
                DBUG_RETURN(IFR_DATA_TRUNC);
            }
            DBUG_RETURN(IFR_OK);
        }
    }    
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter::translateBinaryOutput(IFRPacket_DataPart& datapart,
                                                           char *data,
                                                           IFR_Length datalength,
                                                           IFR_Length*lengthindicator,
                                                           IFR_ConnectionItem& clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_ByteCharDataConverter, translateBinaryOutput);
    IFR_Length  offset    = 1;
    IFR_Length  dataoffset= 0;
    DBUG_RETURN(appendBinaryOutput(datapart, data, datalength, lengthindicator, clink, dataoffset, offset, 0));
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter::appendBinaryOutput(IFRPacket_DataPart&   datapart,
                                                        char                 *data,
                                                        IFR_Length            datalength,
                                                        IFR_Length*           lengthindicator,
                                                        IFR_ConnectionItem   &clink,
                                                        IFR_Length&           dataoffset,
                                                        IFR_Length&           offset,
                                                        IFRConversion_Getval *getval)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_ByteCharDataConverter, appendBinaryOutput);

    if(dataoffset != 0) {
        clink.error().setRuntimeError(IFR_ERR_BINARY_TO_ENCODED_UNSUPPORTED_I, getIndex());
        DBUG_RETURN(IFR_NOT_OK);
    }

    char       *read_data   = datapart.getOutputData(m_shortinfo.pos.bufpos);
    IFR_Length  byteslength = m_shortinfo.iolength - 1;

    if(m_flags.trimming) {
        byteslength = IFRConversion_StringPadLength(read_data, byteslength, m_shortinfo.getPaddingCharacter());
    }

    IFR_Length z_offset = offset - 1;
    
    // no data found if not at begin and offset >= length
    if(z_offset && z_offset >= byteslength) {
        DBUG_RETURN(IFR_NO_DATA_FOUND);
    }

    read_data += z_offset;
    byteslength -= z_offset;
    
    IFR_Length copylength = byteslength > datalength ? datalength : byteslength;
    
    memcpy(data, read_data, copylength);
    
    offset += copylength;

    if(lengthindicator) {
        *lengthindicator = byteslength;
    }
    
    if(copylength < byteslength) {
        DBUG_RETURN(IFR_DATA_TRUNC);
    } else {
        DBUG_RETURN(IFR_OK);
    }    
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter::translateUCS2Output(IFRPacket_DataPart& datapart,
                                                         char *data,
                                                         IFR_Bool swapped,
                                                         IFR_Length datalength,
                                                         IFR_Length*lengthindicator,
                                                         IFR_Bool   terminate,
                                                         IFR_ConnectionItem& clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_ByteCharDataConverter, translateUCS2Output);
    IFR_Length  offset     = 1;
    IFR_Length  dataoffset = 0;
    DBUG_RETURN(appendUCS2Output(datapart, data, swapped, datalength, lengthindicator, terminate, clink, dataoffset, offset, 0));
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter::appendUCS2Output(IFRPacket_DataPart&   datapart,
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
    DBUG_CLINK_METHOD_ENTER(IFRConversion_ByteCharDataConverter, appendUCS2Output);
    
    if(dataoffset != 0) {
        clink.error().setRuntimeError(IFR_ERR_NOT_IMPLEMENTED("output of character data at offset in output buffer"));
        DBUG_RETURN(IFR_NOT_OK);
    }


    char *read_data = datapart.getOutputData(m_shortinfo.pos.bufpos);
    IFR_Length byteslength = m_shortinfo.iolength - 1;
    
    if(m_flags.trimming) {
        byteslength = IFRConversion_StringPadLength(read_data, byteslength, m_shortinfo.getPaddingCharacter());
    }
    
    IFR_Length z_offset = offset - 1;

    // no data found if not at begin and offset >= length
    if(z_offset && z_offset >= byteslength) {
        DBUG_RETURN(IFR_NO_DATA_FOUND);
    }
    
    read_data += z_offset;
    byteslength -= z_offset;

    IFR_Bool datalength_too_small = false;

    if(terminate) {
        if(datalength > 1) {
            datalength -= 2;
        } else {
            memset(data, 0, datalength);
            datalength = 0;
            datalength_too_small = true;
        }
    }
    IFR_UInt4 destbyteswritten;
    IFR_UInt4 srcbytesparsed;
    IFR_Retcode rc;
    tsp78ConversionResult conv_rc;
    
    if(m_flags.bin2hex) {
        conv_rc = IFRUtil_BinaryToHex(IFR_ENCODING(swapped?IFR_StringEncodingUCS2Swapped:IFR_StringEncodingUCS2),
                                      data, 
                                      datalength,
                                      &destbyteswritten,
                                      read_data,
                                      byteslength,
                                      &srcbytesparsed);
    } else {
        conv_rc = sp78convertBuffer(IFR_ENCODING(swapped?IFR_StringEncodingUCS2Swapped:IFR_StringEncodingUCS2),
                                    data,
                                    datalength,
                                    &destbyteswritten,
                                    IFR_ENCODING(IFR_StringEncodingAscii),
                                    read_data,
                                    byteslength,
                                    &srcbytesparsed);
    }
    

    switch(conv_rc) {
    case sp78_Ok: {
        if(lengthindicator) {
            *lengthindicator = destbyteswritten;
        }
        if(terminate && !datalength_too_small) {
            data[destbyteswritten] = 0;
            data[destbyteswritten+1] = 0;
        }
        offset += srcbytesparsed;
        rc = IFR_OK;
        break;
    }
    case (sp78_TargetExhausted): {
        if(lengthindicator) {
            if(m_flags.bin2hex) {
                *lengthindicator = byteslength * 4;
            } else {
                *lengthindicator = byteslength * 2;
            }
        }
        if(terminate && !datalength_too_small) {
            data[destbyteswritten] = 0;
            data[destbyteswritten+1] = 0;
        }
        offset += srcbytesparsed;
        rc = IFR_DATA_TRUNC;
        break;
    }
    case (sp78_SourceExhausted):
    case (sp78_SourceCorrupted):
    default: {
        clink.error().setRuntimeError(IFR_ERR_CORRUPTED_UCS2ASCIIDATA_I, (IFR_Int4)this->m_index);
        rc = IFR_NOT_OK;
        break;
    }
    }
    
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter::translateUTF8Output(IFRPacket_DataPart& datapart,
                                                         char *data,
                                                         IFR_Length datalength,
                                                         IFR_Length *lengthindicator,
                                                         IFR_Bool    terminate,
                                                         IFR_ConnectionItem& clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_ByteCharDataConverter, translateUTF8Output);
    IFR_Length  offset     = 1;
    IFR_Length  dataoffset = 0;
    DBUG_RETURN(appendUTF8Output(datapart, data, datalength, lengthindicator, terminate, clink, dataoffset, offset, 0));
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter::appendUTF8Output(IFRPacket_DataPart&   datapart,
                                                      char                 *data,
                                                      IFR_Length            datalength,
                                                      IFR_Length*           lengthindicator,
                                                      IFR_Bool              terminate,
                                                      IFR_ConnectionItem   &clink,
                                                      IFR_Length&           dataoffset,
                                                      IFR_Length&           offset,
                                                      IFRConversion_Getval *getval)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_ByteCharDataConverter, appendUTF8Output);
    if(dataoffset != 0) {
        clink.error().setRuntimeError(IFR_ERR_NOT_IMPLEMENTED("output of character data at offset in output buffer"));
        DBUG_RETURN(IFR_NOT_OK);
    }

    char *read_data = datapart.getOutputData(m_shortinfo.pos.bufpos);
    IFR_Length byteslength = m_shortinfo.iolength - 1;
    
    if(m_flags.trimming) {
        byteslength = IFRConversion_StringPadLength(read_data, byteslength, m_shortinfo.getPaddingCharacter());
    }

    IFR_Length z_offset = offset - 1;

    // no data found if not at begin and offset >= length
    if(z_offset && z_offset >= byteslength) {
        DBUG_RETURN(IFR_NO_DATA_FOUND);
    }
    
    read_data += z_offset;
    byteslength -= z_offset;

    IFR_Bool datalength_too_small = false;
    
    if(terminate) {
        if(datalength) {
            --datalength;
        } else {
            datalength_too_small = true;
        }
    }
    IFR_UInt4 destbyteswritten;
    IFR_UInt4 srcbytesparsed;
    IFR_Retcode rc;
    tsp78ConversionResult conv_rc;
    
    if(m_flags.bin2hex) {
        conv_rc = IFRUtil_BinaryToHex(IFR_ENCODING(IFR_StringEncodingUTF8),
                                    data,
                                    datalength,
                                    &destbyteswritten,
                                    read_data,
                                    byteslength,
                                    &srcbytesparsed);
        
    } else {
        conv_rc = sp78convertBuffer(IFR_ENCODING(IFR_StringEncodingUTF8),
                                    data,
                                    datalength,
                                    &destbyteswritten,
                                    IFR_ENCODING(IFR_StringEncodingAscii),
                                    read_data,
                                    byteslength,
                                    &srcbytesparsed);
    }
    switch(conv_rc) {
    case sp78_Ok: {
        if(lengthindicator) {
            *lengthindicator = destbyteswritten;
        }
        if(terminate && !datalength_too_small) {
            data[destbyteswritten] = 0;
        }
        offset += srcbytesparsed;
        rc = IFR_OK;
        break;
    }
    case (sp78_TargetExhausted): {
        if(lengthindicator) {
            if(m_flags.bin2hex) {
                *lengthindicator = byteslength * 2;
            } else {
                *lengthindicator = IFRConversion_StringAsciiLengthAsUTF8((unsigned char *)read_data,
                                                                         byteslength);
            }
        }
        if(terminate && !datalength_too_small) {
            data[destbyteswritten] = 0;
        }
        offset += srcbytesparsed;
        rc = IFR_DATA_TRUNC;
        break;
    }
    case (sp78_SourceExhausted):
    case (sp78_SourceCorrupted):
    default: {
        clink.error().setRuntimeError(IFR_ERR_CORRUPTED_UTF8DATA_I, (IFR_Int4)this->m_index);
        rc = IFR_NOT_OK;
        break;
    }
    }
    
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
template <class Integer>
IFR_Retcode
translate_integer_output(char *p,
                         Integer& result,
                         IFR_ConnectionItem& clink,
                         IFR_Int4 min_value,
                         IFR_Int4 max_value,
                         IFR_Length *lengthindicator,
                         IFR_Int4 paramindex)
{
    DBUG_CONTEXT_METHOD_ENTER(static, translate_integer_output, &clink);
    while(*p && isspace(*p)) {
         ++p;
     }
     if(*p == 0) {
         result = 0;
         if(lengthindicator)
             *lengthindicator = sizeof(Integer);
         DBUG_PRINT(lengthindicator);
         DBUG_RETURN(IFR_OK);
     }

     char *trail=0;
     long l=strtol(p, &trail, 10);
     DBUG_PRINT(l);

     // now we have the different things:
     // - there may be garbage
     // - there may be an overflow

     if( l < min_value || l > max_value) {
         clink.error().setRuntimeError(IFR_ERR_NUMERIC_OVERFLOW_I, paramindex);
         return IFR_NOT_OK;
     }
     result = (Integer) l;
     if(p == trail) {
         clink.error().setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, paramindex);
         return IFR_NOT_OK;
     }

     // trailing spaces are not treated as harmful
     while(*trail != '\0') {
         if(!isspace(*trail)) {
             return IFR_DATA_TRUNC;
         }
         ++trail;
     }

     if(lengthindicator)
         *lengthindicator = sizeof(Integer);

     DBUG_PRINT(lengthindicator);
     DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
void
IFRConversion_ByteCharDataConverter::moveDataToBuffer(IFRPacket_DataPart& datapart,
                                                      char *buffer)
{
    IFR_UInt4 destbyteswritten;
    IFR_UInt4 srcbytesparsed;
    tsp78ConversionResult rc =
        sp78convertString(IFR_ENCODING(IFR_StringEncodingAscii),
                          buffer,
                          m_shortinfo.length+1,
                          &destbyteswritten,
                          true,
                          IFR_ENCODING(m_partencoding),
                          datapart.getOutputData(m_shortinfo.pos.bufpos),
                          m_shortinfo.iolength-1,
                          &srcbytesparsed);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter::translateOutput(IFRPacket_DataPart& datapart,
                                                     IFR_Int1&             data,
                                                     IFR_Length*             lengthindicator,
                                                     IFR_ConnectionItem&   clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_ByteCharDataConverter, translateOutput_Int1);

    if(!m_flags.numbers) {
        clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    }

    char *buffer = (char *) alloca(m_shortinfo.length + 1);
    moveDataToBuffer(datapart, buffer);
    
    IFRConversion_FromString<IFR_Int1> cnv(m_index);
    DBUG_RETURN(cnv.convert(data, lengthindicator, buffer, clink.error()));
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter::translateOutput(IFRPacket_DataPart& datapart,
                                                     IFR_Int2&             data,
                                                     IFR_Length*             lengthindicator,
                                                     IFR_ConnectionItem&   clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_ByteCharDataConverter, translateOutput_Int2);

    if(!m_flags.numbers) {
        clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    }
    char *buffer = (char *) alloca(m_shortinfo.length + 1);
    moveDataToBuffer(datapart, buffer);

    IFRConversion_FromString<IFR_Int2> cnv(m_index);
    DBUG_RETURN(cnv.convert(data, lengthindicator, buffer, clink.error()));

}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter::translateOutput(IFRPacket_DataPart& datapart,
                                                     IFR_UInt1&             data,
                                                     IFR_Length*             lengthindicator,
                                                     IFR_ConnectionItem&   clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_ByteCharDataConverter, translateOutput_UInt1);

    if(!m_flags.numbers) {
        clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    }
    char *buffer = (char *) alloca(m_shortinfo.length + 1);
    moveDataToBuffer(datapart, buffer);
    IFRConversion_FromString<IFR_UInt1> cnv(m_index);
    DBUG_RETURN(cnv.convert(data, lengthindicator, buffer, clink.error()));
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter::translateOutput(IFRPacket_DataPart& datapart,
                                                     IFR_UInt2&             data,
                                                     IFR_Length*             lengthindicator,
                                                     IFR_ConnectionItem&   clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_ByteCharDataConverter, translateOutput_UInt2);

    if(!m_flags.numbers) {
        clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    }

    char *buffer = (char *) alloca(m_shortinfo.length + 1);
    moveDataToBuffer(datapart, buffer);
    IFRConversion_FromString<IFR_UInt2> cnv(m_index);
    DBUG_RETURN(cnv.convert(data, lengthindicator, buffer, clink.error()));
}


//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter::translateOutput(IFRPacket_DataPart& datapart,
                                                     IFR_UInt4& data,
                                                     IFR_Length*  lengthindicator,
                                                     IFR_ConnectionItem& clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_ByteCharDataConverter, translateOutput_UInt4);

    if(!m_flags.numbers) {
        clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    }

    char *buffer = (char *) alloca(m_shortinfo.length + 1);
    moveDataToBuffer(datapart, buffer);
    IFRConversion_FromString<IFR_UInt4> cnv(m_index);
    DBUG_RETURN(cnv.convert(data, lengthindicator, buffer, clink.error()));  
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter::translateOutput(IFRPacket_DataPart& datapart,
                                                     IFR_Int4& data,
                                                     IFR_Length*  lengthindicator,
                                                     IFR_ConnectionItem& clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_ByteCharDataConverter, translateOutput_Int4);

    if(!m_flags.numbers) {
        clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    }

    char *buffer = (char *) alloca(m_shortinfo.length + 1);
    moveDataToBuffer(datapart, buffer);
    IFRConversion_FromString<IFR_Int4> cnv(m_index);
    DBUG_RETURN(cnv.convert(data, lengthindicator, buffer, clink.error()));  
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter::translateOutput(IFRPacket_DataPart& datapart,
                                                          IFR_UInt8& data,
                                                          IFR_Length* lengthindicator,
                                                          IFR_ConnectionItem& clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_ByteCharDataConverter, translateOutput_UInt8);

    if(!m_flags.numbers) {
        clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    }

    char *buffer = (char *) alloca(m_shortinfo.length + 1);
    moveDataToBuffer(datapart, buffer);
    IFRConversion_FromString<IFR_UInt8> cnv(m_index);
    DBUG_RETURN(cnv.convert(data, lengthindicator, buffer, clink.error()));  
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter::translateOutput(IFRPacket_DataPart& datapart,
                                                          IFR_Int8& data,
                                                          IFR_Length* lengthindicator,
                                                          IFR_ConnectionItem& clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_ByteCharDataConverter, translateOutput_Int8);

    if(!m_flags.numbers) {
        clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    }

    char *buffer = (char *) alloca(m_shortinfo.length + 1);
    moveDataToBuffer(datapart, buffer);
    IFRConversion_FromString<IFR_Int8> cnv(m_index);
    DBUG_RETURN(cnv.convert(data, lengthindicator, buffer, clink.error()));  
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter
::translateOutput(IFRPacket_DataPart& datapart,
                  double&               data,
                  IFR_Length*           lengthindicator,
                  IFR_ConnectionItem &clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_ByteCharDataConverter, translateOutput_double);

    if(!m_flags.numbers) {
        clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    }
    
    char *buffer = (char *) alloca(m_shortinfo.length + 1);
    moveDataToBuffer(datapart, buffer);

    IFRConversion_FromString<double> cnv(m_index);
    DBUG_RETURN(cnv.convert(data, lengthindicator, buffer, clink.error()));
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter
::translateOutput(IFRPacket_DataPart& datapart,
                  float&               data,
                  IFR_Length*           lengthindicator,
                  IFR_ConnectionItem &clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_ByteCharDataConverter, translateOutput_float);
    
    if(!m_flags.numbers) {
        clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    }

    char *buffer = (char *) alloca(m_shortinfo.length + 1);
    moveDataToBuffer(datapart, buffer);

    IFRConversion_FromString<float> cnv(m_index);
    IFR_Retcode rc = cnv.convert(data, lengthindicator, buffer, clink.error());
    DBUG_RETURN(rc);
}



//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter::translateOutput(IFRPacket_DataPart& part,
                                                     SQL_DATE_STRUCT& data,
                                                     IFR_Length* lengthindicator,
                                                     IFR_ConnectionItem& clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_ByteCharDataConverter, translateOutput_DATE);
    
    char *buffer=(char *) alloca(m_shortinfo.length + 1);
    moveDataToBuffer(part, buffer);
    IFRConversion_FromString<SQL_DATE_STRUCT> cnv(m_index, clink.getConnection()->getDateTimeFormat());
    DBUG_RETURN(cnv.convert(data, lengthindicator, buffer, clink.error()));
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter::translateOutput(IFRPacket_DataPart& part,
                                                     SQL_TIME_STRUCT& data,
                                                     IFR_Length*        lengthindicator,
                                                     IFR_ConnectionItem& clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_ByteCharDataConverter, translateOutput_TIME);

    char *buffer=(char *) alloca(m_shortinfo.length + 1);
    moveDataToBuffer(part, buffer);
    IFRConversion_FromString<SQL_TIME_STRUCT> cnv(m_index, clink.getConnection()->getDateTimeFormat());
    DBUG_RETURN(cnv.convert(data, lengthindicator, buffer, clink.error()));
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter::translateOutput(IFRPacket_DataPart& part,
                                                     SQL_TIMESTAMP_STRUCT& data,
                                                     IFR_Length* lengthindicator,
                                                     IFR_ConnectionItem& clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_ByteCharDataConverter, translateOutput_TIMESTAMP);

    char *buffer=(char *) alloca(m_shortinfo.length + 1);
    moveDataToBuffer(part, buffer);
    IFRConversion_FromString<SQL_TIMESTAMP_STRUCT> cnv(m_index, clink.getConnection()->getDateTimeFormat());
    DBUG_RETURN(cnv.convert(data, lengthindicator, buffer, clink.error()));
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter
::translateOutput(IFRPacket_DataPart&      datapart,
                  SQL_NUMERIC_STRUCT&      data,
                  IFR_Length*                lengthindicator,
                  IFR_ConnectionItem      &clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_ByteCharDataConverter, translateOutput_NUMERIC);

    if(!m_flags.numbers) {
        clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    }
    
    char *buffer = (char *) alloca(m_shortinfo.length + 1);
    moveDataToBuffer(datapart, buffer);
    
    IFR_Retcode rc = IFRUtil_SQLNumeric::asciiStringToNumeric(buffer, data);
    switch(rc) {
    case IFR_NOT_OK:
        clink.error().setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, (IFR_Int4)this->m_index);
        break;
    case IFR_OVERFLOW:
        clink.error().setRuntimeError(IFR_ERR_NUMERIC_OVERFLOW_I, (IFR_Int4)this->m_index);
        break;
    default:
        if(lengthindicator) {
            *lengthindicator = sizeof(SQL_NUMERIC_STRUCT);
        }
    }
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ByteCharDataConverter
::translateOutput(IFRPacket_DataPart& datapart,
                  GUID&               data,
                  IFR_Length*           lengthindicator,
                  IFR_ConnectionItem &clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_ByteCharDataConverter, translateOutput_GUID);
    if(m_shortinfo.length < sizeof(GUID)) {
        clink.error().setRuntimeError(IFR_ERR_PARAM_CONVERSION_TRUNCATEDATA_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    }
    IFR_Retcode rc=translateBinaryOutput(datapart, (char*)&data, sizeof(GUID), lengthindicator, clink);
    if(rc != IFR_OK && rc!=IFR_DATA_TRUNC) {
        DBUG_RETURN(rc);
    } else {
        if(lengthindicator && *lengthindicator > 0) {
            *lengthindicator=sizeof(GUID);
        }
        DBUG_RETURN(IFR_OK);
    }
}

