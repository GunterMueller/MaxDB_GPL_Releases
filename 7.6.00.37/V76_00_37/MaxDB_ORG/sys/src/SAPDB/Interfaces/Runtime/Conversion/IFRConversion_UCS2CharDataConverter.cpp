/*!
  @file           IFRConversion_UCS2CharDataConverter.cpp
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
#include "SAPDB/Interfaces/Runtime/IFR_Common.h"
#include "SAPDB/Interfaces/Runtime/IFR_Connection.h"
#include "SAPDB/Interfaces/Runtime/Conversion/IFRConversion_UCS2CharDataConverter.h"
#include "SAPDBCommon/SAPDB_ReuseLib.h"
#include "hsp83.h"

#include <stdlib.h>
#include <errno.h>
#include <ctype.h>

#define DBUG_CLINK_METHOD_ENTER(x,y) DBUG_CONTEXT_METHOD_ENTER(x, y, &clink)

//----------------------------------------------------------------------
IFRConversion_UCS2CharDataConverter
::IFRConversion_UCS2CharDataConverter(IFR_ShortInfo &shortinfo,
                                      SAPDBMem_IRawAllocator& allocator,
                                      IFR_Bool istrimming,
                                      IFR_Bool integerconversion,
                                      IFR_Connection& connection)
:IFRConversion_ByteCharDataConverter(shortinfo,
                                     allocator, 
                                     istrimming, 
                                     integerconversion,
                                     connection.getSQLMode() == IFR_ORACLE)
{
    m_partencoding = IFR_StringEncodingUCS2Native;
}

//----------------------------------------------------------------------
IFRConversion_UCS2CharDataConverter
::IFRConversion_UCS2CharDataConverter(IFR_ShortInfo &shortinfo,
                                      SAPDBMem_IRawAllocator& allocator,
                                      IFR_Bool istrimming,
                                      IFR_Bool integerconversion)
:IFRConversion_ByteCharDataConverter(shortinfo,
                                     allocator, 
                                     istrimming, 
                                     integerconversion,
                                     false)
{
    m_partencoding = IFR_StringEncodingUCS2Native;
}

//----------------------------------------------------------------------
IFRConversion_UCS2CharDataConverter::~IFRConversion_UCS2CharDataConverter()
{
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_UCS2CharDataConverter::translateBinaryInput(IFRPacket_DataPart& datapart,
                                                          char               *data,
                                                          IFR_Length          datalength,
                                                          IFR_Length*         lengthindicator,
                                                          IFR_ConnectionItem &clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_UCS2CharDataConverter, translateBinaryInput);
    DBUG_RETURN(IFRConversion_ByteCharDataConverter::translateBinaryInput(datapart, 
                                                                          data, 
                                                                          datalength, 
                                                                          lengthindicator, 
                                                                          clink));
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_UCS2CharDataConverter::appendBinaryInput(IFRPacket_DataPart&   datapart,
                                          char                 *data,
                                          IFR_Length            datalength,
                                          IFR_Length*           lengthindicator,
                                          IFR_ConnectionItem   &clink,
                                          IFR_Length&           offset,
                                          IFRConversion_Putval *putval)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_UCS2CharDataConverter, appendBinaryInput);
    DBUG_RETURN(IFRConversion_ByteCharDataConverter::appendBinaryInput(datapart, 
                                                                       data, 
                                                                       datalength, 
                                                                       lengthindicator, 
                                                                       clink, 
                                                                       offset, 
                                                                       putval));
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_UCS2CharDataConverter::translateAsciiInput(IFRPacket_DataPart& datapart,
                                                         char               *data,
                                                         IFR_Length          datalength,
                                                         IFR_Length*         lengthindicator,
                                                         IFR_Bool            terminate,
                                                         IFR_Bool            ascii7bit,
                                                         IFR_ConnectionItem &clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_UCS2CharDataConverter, translateAsciiInput);
    IFR_Length offset = 0;
    DBUG_RETURN(appendAsciiInput(datapart, data, datalength, lengthindicator, terminate, ascii7bit, clink, offset, 0));
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_UCS2CharDataConverter::appendAsciiInput(IFRPacket_DataPart& datapart,
                                                      char               *data,
                                                      IFR_Length          datalength,
                                                      IFR_Length*         lengthindicator,
                                                      IFR_Bool            terminate,
                                                      IFR_Bool            ascii7bit,
                                                      IFR_ConnectionItem &clink,
                                                      IFR_Length&         offset,
                                                      IFRConversion_Putval *putval)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_UCS2CharDataConverter, appendAsciiInput);
    // Look how much data is available.
    IFR_Length byteslength;
    if(IFRConversion_InputDataLength(datalength, lengthindicator, data, byteslength, terminate)) {
        clink.error().setRuntimeError(IFR_ERR_INVALID_LENGTHINDICATOR_I, (IFR_Int4)getIndex());
        DBUG_RETURN(IFR_NOT_OK);
    }

    if(byteslength == 0 && m_flags.oraclenull && offset == 0) {
        DBUG_RETURN(translateNullInput(datapart, clink));
    }


    IFR_Retcode rc = IFR_OK;
    IFR_Length  old_offset = offset;

    if(ascii7bit) {
        if(!IFRConversion_StringIsAscii7((unsigned char *)data, byteslength)) {
            clink.error().setRuntimeError(IFR_ERR_NOT_ASCII_CHARACTERS_I, (IFR_Int4)getIndex());
            DBUG_RETURN(IFR_NOT_OK);
        }
    }

    if(offset == 0) {
        offset = byteslength * 2; // ascii => ucs2native
        rc = datapart.addParameter(data,
                                   byteslength,
                                   IFR_StringEncodingAscii,
                                   IFR_StringEncodingUCS2Native,
                                   m_shortinfo,
                                   false);
    } else {
        rc = datapart.appendToParameter(data,
                                        byteslength,
                                        IFR_StringEncodingAscii,
                                        IFR_StringEncodingUCS2Native,
                                        m_shortinfo,
                                        offset,
                                        false);
    }

    switch(rc) {
    case IFR_NOT_OK: {
        clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)getIndex());
        DBUG_RETURN(rc);
    }
    case IFR_OK:
        break;
    case IFR_DATA_TRUNC:
        // need to divide by 2 because of UCS encoding in destination
        offset = (m_shortinfo.iolength - 1) / 2;
        if(m_flags.trimming) {
            // If we have now appended less or equal chars excluding padding
            // chars, but supplied padding chars, these are truncated silently.
            // in case it is a variadic char type
            if(offset >= old_offset + IFRConversion_StringPadLength(data, byteslength, ' ')) {
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
IFRConversion_UCS2CharDataConverter::translateUCS2Input(IFRPacket_DataPart& datapart,
                                                        char               *data,
                                                        IFR_Bool            swapped,
                                                        IFR_Length            datalength,
                                                        IFR_Length*           lengthindicator,
                                                        IFR_Bool              terminate,
                                                        IFR_ConnectionItem &clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_UCS2CharDataConverter, translateUCS2Input);
    IFR_Length offset = 0;
    DBUG_RETURN(appendUCS2Input(datapart, data, swapped, datalength, lengthindicator, terminate, clink, offset, 0));
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_UCS2CharDataConverter::appendUCS2Input(IFRPacket_DataPart& datapart,
                                                     char               *data,
                                                     IFR_Bool            swapped,
                                                     IFR_Length            datalength,
                                                     IFR_Length*           lengthindicator,
                                                     IFR_Bool              terminate,
                                                     IFR_ConnectionItem &clink,
                                                     IFR_Length&         offset,
                                                     IFRConversion_Putval *putval)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_UCS2CharDataConverter, appendUCS2Input);
    IFR_Length byteslength;
    if(IFRConversion_InputDataLengthUCS2(datalength, lengthindicator, data, byteslength, terminate)) {
        clink.error().setRuntimeError(IFR_ERR_INVALID_LENGTHINDICATOR_I, (IFR_Int4)getIndex());
        DBUG_RETURN(IFR_NOT_OK);
    }

    if(byteslength == 0 && m_flags.oraclenull && offset == 0) {
        DBUG_RETURN(translateNullInput(datapart, clink));
    }

    if(byteslength % 2) {
        clink.error().setRuntimeError(IFR_ERR_ODD_DATALENGTH_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    }
    IFR_Retcode rc = IFR_OK;
    IFR_Length old_offset = offset;
    if(offset == 0) {
        offset = byteslength;
        rc = datapart.addParameter(data,
                                   byteslength,
                                   swapped?IFR_StringEncodingUCS2Swapped:IFR_StringEncodingUCS2,
                                   IFR_StringEncodingUCS2Native,
                                   m_shortinfo,
                                   false);
    } else {
        rc = datapart.appendToParameter(data,
                                        byteslength,
                                        swapped?IFR_StringEncodingUCS2Swapped:IFR_StringEncodingUCS2,
                                        IFR_StringEncodingUCS2Native,
                                        m_shortinfo,
                                        offset,
                                        false);
    }
    switch(rc) {
    case IFR_NOT_OK:
        clink.error().setRuntimeError(IFR_ERR_CORRUPTED_UCS2ASCIIDATA_I, (IFR_Int4)getIndex());
        break;
    case IFR_OK:
        break;
    case IFR_DATA_TRUNC: 
        offset = m_shortinfo.iolength - 1;
        if(m_flags.trimming) {
            if(offset >= old_offset + IFRConversion_StringPadLengthUCS2(data, byteslength, ' ', swapped)) {
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
IFRConversion_UCS2CharDataConverter::translateUTF8Input(IFRPacket_DataPart& datapart,
                                                        char               *data,
                                                        IFR_Length          datalength,
                                                        IFR_Length*         lengthindicator,
                                                        IFR_Bool            terminate,
                                                        IFR_ConnectionItem &clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_UCS2CharDataConverter, translateUTF8Input);
    IFR_Length offset = 0;
    DBUG_RETURN(appendUTF8Input(datapart, data, datalength, lengthindicator, terminate, clink, offset, 0));
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_UCS2CharDataConverter::appendUTF8Input(IFRPacket_DataPart& datapart,
                                                     char               *data,
                                                     IFR_Length            datalength,
                                                     IFR_Length*           lengthindicator,
                                                     IFR_Bool terminate,
                                                     IFR_ConnectionItem   &clink,
                                                     IFR_Length&           offset,
                                                     IFRConversion_Putval *putval)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_UCS2CharDataConverter, appendUTF8Input);
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

    char *d = (char *) alloca(charcount * 2);
    tsp00_Byte const * unused_inputptr;
    tsp81_UCS2Char   * unused_destptr;
    tsp83UTF8_ConversionResult conv_res =
        sp83UTF8ConvertToUCS2((const tsp00_Byte *)data,
                              (const tsp00_Byte *)(data + byteslength),
                              &unused_inputptr,
                              (tsp81_UCS2Char *) d,
                              (tsp81_UCS2Char *) (d + charcount * 2),
                              &unused_destptr,
                              IFR_StringEncodingUCS2Native==IFR_StringEncodingUCS2Swapped);
    if(conv_res != sp83UTF8Convert_Success) {
        clink.error().setRuntimeError(IFR_ERR_CORRUPTED_UTF8DATA_I,
                                      (IFR_Int4)getIndex());
    }

    IFR_Length tmp_datalength = charcount * 2;
    IFR_Length tmp_lengthindicator = charcount * 2;

    IFR_Retcode rc = appendUCS2Input(datapart, d,
                                     IFR_StringEncodingUCS2Native==IFR_StringEncodingUCS2Swapped,
                                     tmp_datalength, &tmp_lengthindicator,
                                     false,
                                     clink, offset, putval);

    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_UCS2CharDataConverter::translateBinaryOutput(IFRPacket_DataPart& datapart,
                                                           char              * data,
                                                           IFR_Length          datalength,
                                                           IFR_Length        * lengthindicator,
                                                           IFR_ConnectionItem& clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_UCS2CharDataConverter, translateBinaryOutput);
    DBUG_RETURN(IFRConversion_ByteCharDataConverter::translateBinaryOutput(datapart, 
                                                                           data, 
                                                                           datalength, 
                                                                           lengthindicator, 
                                                                           clink));
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_UCS2CharDataConverter::appendBinaryOutput(IFRPacket_DataPart & datapart,
                                                        char               * data,
                                                        IFR_Length           datalength,
                                                        IFR_Length         * lengthindicator,
                                                        IFR_ConnectionItem & clink,
                                                        IFR_Length         & dataoffset,
                                                        IFR_Length         & offset,
                                                        IFRConversion_Getval* getval)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_UCS2CharDataConverter, appendBinaryOutput);

    if(dataoffset != 0) {
        clink.error().setRuntimeError(IFR_ERR_BINARY_TO_ENCODED_UNSUPPORTED_I, getIndex());
        DBUG_RETURN(IFR_NOT_OK);
    }

    char       *read_data   = datapart.getOutputData(m_shortinfo.pos.bufpos);
    IFR_Length  byteslength = m_shortinfo.iolength - 1;

//     if(m_flags.trimming) {
//         byteslength = IFRConversion_StringPadLength(read_data, byteslength, m_shortinfo.getPaddingCharacter());
//     }
    
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
IFRConversion_UCS2CharDataConverter::translateAsciiOutput(IFRPacket_DataPart& datapart,
                                                          char *data,
                                                          IFR_Length datalength,
                                                          IFR_Length *lengthindicator,
                                                          IFR_Bool    terminate,
                                                          IFR_Bool    ascii7bit,
                                                          IFR_ConnectionItem& clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_UCS2CharDataConverter, translateAsciiOutput);
    IFR_Length  offset     = 1;
    IFR_Length  dataoffset = 0;
    DBUG_RETURN(appendAsciiOutput(datapart, data, datalength, lengthindicator, terminate, ascii7bit, clink, dataoffset, offset, 0));
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_UCS2CharDataConverter::appendAsciiOutput(IFRPacket_DataPart&   datapart,
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
    DBUG_CLINK_METHOD_ENTER(IFRConversion_UCS2CharDataConverter, appendAsciiOutput);
    
    if(dataoffset != 0) {
        clink.error().setRuntimeError(IFR_ERR_NOT_IMPLEMENTED("output of character data at offset in output buffer"));
        DBUG_RETURN(IFR_NOT_OK);
    }


    char *read_data = datapart.getOutputData(m_shortinfo.pos.bufpos);
    IFR_Length byteslength = m_shortinfo.iolength - 1;
    IFR_Bool packet_swapped = (IFR_StringEncodingUCS2Swapped == IFR_StringEncodingUCS2Native);
    
    if(m_flags.trimming) {
        byteslength = IFRConversion_StringPadLengthUCS2(read_data, byteslength, ' ', packet_swapped);
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
    tsp78ConversionResult conv_rc = sp78convertString(IFR_ENCODING(IFR_StringEncodingAscii),
                                                      data,
                                                      datalength,
                                                      &destbyteswritten,
                                                      false,
                                                      IFR_ENCODING(IFR_StringEncodingUCS2Native),
                                                      read_data,
                                                      byteslength,
                                                      &srcbytesparsed);
    

    switch(conv_rc) {
    case sp78_Ok: {
        if(lengthindicator) {
            *lengthindicator = destbyteswritten;
        }
        if(terminate && !datalength_too_small) {
            data[destbyteswritten] = 0;
        }
        if(ascii7bit) {
            if(!IFRConversion_StringIsAscii7((unsigned char *)data, destbyteswritten)) {
                clink.error().setRuntimeError(IFR_ERR_NOT_ASCII_CHARACTERS_I, (IFR_Int4)getIndex());
                DBUG_RETURN(IFR_NOT_OK);
            }
        }
        offset += srcbytesparsed;
        rc = IFR_OK;
        break;
    }
    case (sp78_TargetExhausted): {
        if(lengthindicator) {
            *lengthindicator = byteslength / 2;
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
        clink.error().setRuntimeError(IFR_ERR_CORRUPTED_UCS2ASCIIDATA_I, (IFR_Int4)this->m_index);
        rc = IFR_NOT_OK;
        break;
    }
    }
    

    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_UCS2CharDataConverter::translateUCS2Output(IFRPacket_DataPart& datapart,
                                                         char              * data,
                                                         IFR_Bool swapped,
                                                         IFR_Length datalength,
                                                         IFR_Length*lengthindicator,
                                                         IFR_Bool   terminate,
                                                         IFR_ConnectionItem& clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_UCS2CharDataConverter, translateUCS2Output);
    IFR_Length  offset     = 1;
    IFR_Length  dataoffset = 0;
    DBUG_RETURN(appendUCS2Output(datapart, data, swapped, datalength, 
                                 lengthindicator, terminate, clink, dataoffset, offset, 0));
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_UCS2CharDataConverter::appendUCS2Output(IFRPacket_DataPart&   datapart,
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
    DBUG_CLINK_METHOD_ENTER(IFRConversion_UCS2CharDataConverter, appendUCS2Output);
    
    if(dataoffset != 0) {
        clink.error().setRuntimeError(IFR_ERR_NOT_IMPLEMENTED("output of character data at offset in output buffer"));
        DBUG_RETURN(IFR_NOT_OK);
    }

    char *read_data = datapart.getOutputData(m_shortinfo.pos.bufpos);
    IFR_Length byteslength = m_shortinfo.iolength - 1;
    IFR_Bool packet_swapped = (IFR_StringEncodingUCS2Swapped == IFR_StringEncodingUCS2Native);
    
    if(m_flags.trimming) {
        byteslength = IFRConversion_StringPadLengthUCS2(read_data, byteslength, ' ', packet_swapped);
    }
    
    IFR_Length z_offset = offset - 1;

    // no data found if not at begin and offset >= length
    if(z_offset && z_offset >= byteslength) {
        DBUG_RETURN(IFR_NO_DATA_FOUND);
    }
    
    read_data += z_offset;
    byteslength -= z_offset;
    
    IFR_Length copylength = byteslength > datalength ? datalength : byteslength;

    // if someone specified an odd data length ...
    if(copylength % 2) {
        --copylength;
    }

    memcpy(data, read_data, copylength);

    // Swap bytes if necessary, i.e. requested swapping != packet swapping
    if(packet_swapped != swapped) {
        for(int i=0; i<copylength-1; i+=2) {
            char tmp = data[i];
            data[i] = data[i+1];
            data[i+1] = tmp;
        }
    }
    
    if(terminate) {
        if(datalength <= 2) {
            memset(data, 0, datalength);
            copylength = 0;
        } else {
            if(copylength == datalength) {
                data[--copylength] = 0;
                data[--copylength] = 0;
            } else {
                data[copylength]     = 0;
                data[copylength + 1] = 0;
            }
        }
    }
    
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
IFRConversion_UCS2CharDataConverter::translateUTF8Output(IFRPacket_DataPart& datapart,
                                                         char *data,
                                                         IFR_Length datalength,
                                                         IFR_Length *lengthindicator,
                                                         IFR_Bool    terminate,
                                                         IFR_ConnectionItem& clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_UCS2CharDataConverter, translateUTF8Output);
    IFR_Length  offset     = 1;
    IFR_Length  dataoffset = 0;
    DBUG_RETURN(appendUTF8Output(datapart, data, datalength, lengthindicator, terminate, clink, dataoffset, offset, 0));
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_UCS2CharDataConverter::appendUTF8Output(IFRPacket_DataPart&   datapart,
                                                      char                 *data,
                                                      IFR_Length            datalength,
                                                      IFR_Length*           lengthindicator,
                                                      IFR_Bool              terminate,
                                                      IFR_ConnectionItem   &clink,
                                                      IFR_Length&           dataoffset,
                                                      IFR_Length&           offset,
                                                      IFRConversion_Getval *getval)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_UCS2CharDataConverter, appendUTF8Output);
    if(dataoffset != 0) {
        clink.error().setRuntimeError(IFR_ERR_NOT_IMPLEMENTED("output of character data at offset in output buffer"));
        DBUG_RETURN(IFR_NOT_OK);
    }

    char *read_data = datapart.getOutputData(m_shortinfo.pos.bufpos);
    IFR_Length byteslength = m_shortinfo.iolength - 1;
    IFR_Bool packet_swapped = (IFR_StringEncodingUCS2Swapped == IFR_StringEncodingUCS2Native);
    
    if(m_flags.trimming) {
        byteslength = IFRConversion_StringPadLengthUCS2(read_data, byteslength, ' ', packet_swapped);
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
    tsp78ConversionResult conv_rc = sp78convertString(IFR_ENCODING(IFR_StringEncodingUTF8),
                                                      data,
                                                      datalength,
                                                      &destbyteswritten,
                                                      false,
                                                      IFR_ENCODING(IFR_StringEncodingUCS2Native),
                                                      read_data,
                                                      byteslength,
                                                      &srcbytesparsed);
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
            *lengthindicator = IFRConversion_StringUCS2LengthAsUTF8((unsigned char *)read_data,
                                                                   byteslength,
                                                                   packet_swapped);
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
        clink.error().setRuntimeError(IFR_ERR_CORRUPTED_UTF8DATA_I, (IFR_Int4)getIndex());
        rc = IFR_NOT_OK;
        break;
    }
    }
    
    DBUG_RETURN(rc);
}
