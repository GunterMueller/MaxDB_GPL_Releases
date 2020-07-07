/*!
  @file           IFRConversion_Getval.cpp
  @author         D030044
  @ingroup        IFR_ConvData
  @brief          Implements a class for long data conversion
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

#include "Interfaces/Runtime/Conversion/IFRConversion_Getval.h"
#include "Interfaces/Runtime/Conversion/IFRConversion_Converter.h"
#include "Interfaces/Runtime/Packet/IFRPacket_RequestPacket.h"
#include "Interfaces/Runtime/Packet/IFRPacket_ReplyPacket.h"
#include "Interfaces/Runtime/Packet/IFRPacket_RequestSegment.h"
#include "Interfaces/Runtime/Packet/IFRPacket_ReplySegment.h"
#include "Interfaces/Runtime/IFR_Connection.h"
#include "Interfaces/Runtime/Util/IFRUtil_BinaryToHex.h"
#include "Interfaces/Runtime/Conversion/IFRConversion_FromString.h"

IFR_BEGIN_NAMESPACE

//----------------------------------------------------------------------
IFRConversion_Getval::IFRConversion_Getval (IFR_Int2 column,
                                            IFR_Int4 row,
                                            IFR_Int2 length,
                                            IFR_Int2 iolength,
                                            IFR_Int1 definedbyte,
                                            IFR_SQLType datatype,
                                            IFR_ConnectionItem& clink,
                                            IFR_Bool bin2hex,
                                            IFR_StringEncoding packetEncoding)
:m_column(column),
 m_row(row),
 m_maxlength(-1),
 m_lastinternpos(1), // At creation, it is assumed the long will be read from start ...
 m_clink(&clink),
 m_datatype(datatype),
 m_length(length),
 m_iolength(iolength),
 m_definedbyte(definedbyte),
 m_dirty(true),
 m_readoffset(1),
 m_status(Normal_C),
 m_longdata(0),
 m_bin2hex(bin2hex)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Getval, IFRConversion_Getval, &clink);
    DBUG_PRINT(column);
    DBUG_PRINT(row);
    
    // initialise the long descriptor 
    memset((void*)&m_longdesc, 0, sizeof(IFRPacket_LongDescriptor));
    m_longdesc.internpos  = 1;
    m_longdesc.valmode    = IFRPacket_LongDescriptor::NoData_C;
    
    // Source encoding.
    if(m_datatype == IFR_SQLTYPE_LONGUNI || m_datatype == IFR_SQLTYPE_STRUNI) {
        m_encoding = IFR_StringEncodingUCS2Native;
    } else if (packetEncoding == IFR_StringEncodingAscii || m_datatype == IFR_SQLTYPE_STRB || m_datatype==IFR_SQLTYPE_LONGB){
        m_encoding = IFR_StringEncodingAscii;
    } else {
        m_encoding = IFR_StringEncodingUCS2Native;
    }
}

//----------------------------------------------------------------------
IFRConversion_Getval::~IFRConversion_Getval()
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Getval, ~IFRConversion_Getval, m_clink);
    clearLongData();
}

//----------------------------------------------------------------------
void
IFRConversion_Getval::putDescriptor(IFRPacket_DataPart& datapart)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Getval, putDescriptor, m_clink);
    
    m_lastinternpos = m_longdesc.internpos;

    // !!! NEVER send vm_datatrunc back to the kernel.
    //     The kernel will then set internpos to the length of the LONG + 1 and delivers 2 bytes
    //     of data.
    // !!!
    if(m_longdesc.valmode == IFRPacket_LongDescriptor::DataTrunc_C) {
        m_longdesc.valmode = IFRPacket_LongDescriptor::DataPart_C;
    }
    
    datapart.addDescriptor(m_longdesc);
}

//----------------------------------------------------------------------
void
IFRConversion_Getval::putDescriptorForGetLength(IFRPacket_DataPart& datapart)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Getval, putDescriptor, m_clink);

    setNoClose();
    clearLongData();

    m_lastinternpos = m_longdesc.internpos;

    // !!! NEVER send vm_datatrunc back to the kernel.
    //     The kernel will then set internpos to the length of the LONG + 1 and delivers 2 bytes
    //     of data.
    // !!!
    if(m_longdesc.valmode == IFRPacket_LongDescriptor::DataTrunc_C) {
        m_longdesc.valmode = IFRPacket_LongDescriptor::DataPart_C;
    }
    m_longdesc.vallen = 2;
    m_dirty           = true;
    datapart.addDescriptor(m_longdesc);
}


//----------------------------------------------------------------------
void
IFRConversion_Getval::updateDescriptor(IFRPacket_DataPart& datapart, 
                                       IFR_Int4 position)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Getval, getDescriptor, m_clink);
    DBUG_PRINT(position);
    
    // Save the valindex as this update may be the 1st, and valindex is not
    // set in data part.
    IFR_Int2 valindex = m_longdesc.valind;  
    
    updateDescriptor(datapart.getOutputData(position));
    
    if (m_longdesc.valind == 0) {
        m_longdesc.valind = valindex;
    } 
}

//----------------------------------------------------------------------
void
IFRConversion_Getval::updateDescriptor(char *descriptor)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Getval, updateDescriptor, m_clink);
    memcpy(&m_longdesc, descriptor, IFRPacket_LongDescriptor::Size);
    IFR_DEBUG_TRACE << "LONG DESCRIPTOR " << m_longdesc << endl;
    m_dirty = false;
    switch(m_longdesc.valmode) {
    case IFRPacket_LongDescriptor::DataTrunc_C:
    case IFRPacket_LongDescriptor::DataPart_C:
    case IFRPacket_LongDescriptor::LastData_C:
        m_status = NeedsClose_C;
        m_longdesc.internpos = m_lastinternpos + m_longdesc.vallen;
        if(m_maxlength == -1) {
            m_maxlength = m_longdesc.maxlen;
            IFR_DEBUG_TRACE << "maxlen = " << m_maxlength << endl;
        }
        break;
    case IFRPacket_LongDescriptor::AllData_C:
        m_longdesc.internpos = m_lastinternpos + m_longdesc.vallen;
        if(isNoClose()) {
            m_status = NeedsClose_C;
        }
        if(m_maxlength == -1) {
            m_maxlength = m_longdesc.maxlen;
            // And sometimes it's only in VALLEN
            if(m_maxlength < m_longdesc.vallen) {
                m_maxlength = m_longdesc.vallen;
            }
            IFR_DEBUG_TRACE << "maxlen = " << m_maxlength << endl;
        }
    }
}
//----------------------------------------------------------------------
void
IFRConversion_Getval::setNoClose()
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Getval, setNoClose, m_clink); 
    m_longdesc.infoset |= IFRPacket_LongDescriptor::NoClose_C;
}

//----------------------------------------------------------------------
IFR_Bool
IFRConversion_Getval::isNoClose() const
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Getval, setNoClose, m_clink);
    DBUG_RETURN((m_longdesc.infoset & IFRPacket_LongDescriptor::NoClose_C) != 0);
}


//----------------------------------------------------------------------
IFR_Int2
IFRConversion_Getval::getValIndex()
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Getval, getValIndex, m_clink);
    DBUG_RETURN(m_longdesc.valind);
}

//----------------------------------------------------------------------
void
IFRConversion_Getval::setValIndex(IFR_Int2 valindex)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Getval, getValIndex, m_clink);
    DBUG_PRINT(valindex);
    m_longdesc.valind = valindex;
    return;
}

//----------------------------------------------------------------------
IFR_StringEncoding
IFRConversion_Getval::getEncoding()
{
  DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Getval, getEncoding, m_clink);
  DBUG_RETURN(m_encoding);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_Getval::transferIntegral(IFRPacket_DataPart& datapart,
                                       IFR_HostType hosttype,
                                       void *data,
                                       IFR_Length *lengthindicator)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Getval, transferIntegral, m_clink);
    IFR_Int4 preferred_read_size = 1 ;

    if(m_datatype == IFR_SQLTYPE_STRB || m_datatype==IFR_SQLTYPE_LONGB) {
        m_clink->error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)getColumn());
        DBUG_RETURN(IFR_NOT_OK);
    }
    
    switch(hosttype) {
    case IFR_HOSTTYPE_DOUBLE:
        preferred_read_size = sizeof(IFRConversion_FromString<double>::BufferType);
        break;
    case IFR_HOSTTYPE_FLOAT:
        preferred_read_size = sizeof(IFRConversion_FromString<float>::BufferType);
        break;
    case IFR_HOSTTYPE_INT1:
        preferred_read_size = sizeof(IFRConversion_FromString<IFR_Int1>::BufferType);
        break;
    case IFR_HOSTTYPE_UINT1:
        preferred_read_size = sizeof(IFRConversion_FromString<IFR_UInt1>::BufferType);
        break;
    case IFR_HOSTTYPE_INT2:
        preferred_read_size = sizeof(IFRConversion_FromString<IFR_Int2>::BufferType);
        break;
    case IFR_HOSTTYPE_UINT2:
        preferred_read_size = sizeof(IFRConversion_FromString<IFR_UInt2>::BufferType);
        break;
    case IFR_HOSTTYPE_INT4:
        preferred_read_size = sizeof(IFRConversion_FromString<IFR_Int4>::BufferType);
        break;
    case IFR_HOSTTYPE_UINT4:
        preferred_read_size = sizeof(IFRConversion_FromString<IFR_UInt4>::BufferType);
        break;
    case IFR_HOSTTYPE_INT8:
        preferred_read_size = sizeof(IFRConversion_FromString<IFR_Int8>::BufferType);
        break;
    case IFR_HOSTTYPE_UINT8:
        preferred_read_size = sizeof(IFRConversion_FromString<IFR_UInt8>::BufferType);
        break;
    case IFR_HOSTTYPE_ODBCDATE:
        preferred_read_size = sizeof(IFRConversion_FromString<SQL_DATE_STRUCT>::BufferType);
        break;
    case IFR_HOSTTYPE_ODBCTIME:
        preferred_read_size = sizeof(IFRConversion_FromString<SQL_TIME_STRUCT>::BufferType);
        break;
    case IFR_HOSTTYPE_ODBCTIMESTAMP:
        preferred_read_size = sizeof(IFRConversion_FromString<SQL_TIMESTAMP_STRUCT>::BufferType);
        break;
    case IFR_HOSTTYPE_ODBCNUMERIC:
        preferred_read_size = sizeof(IFRConversion_FromString<SQL_NUMERIC_STRUCT>::BufferType);
        break;
    }
    IFR_StringEncoding encoding = IFR_StringEncodingAscii;
    switch(m_longdesc.valmode) {
    case IFRPacket_LongDescriptor::Error_C:
        m_clink->error().setRuntimeError(IFR_ERR_ERROR_LONGDESCRIPTOR);
        DBUG_RETURN(IFR_NOT_OK);
    case IFRPacket_LongDescriptor::StartposInvalid_C:
        m_clink->error().setRuntimeError(IFR_ERR_INVALID_STARTPOSITION_I, (IFR_Int4)m_longdesc.internpos);
        DBUG_RETURN(IFR_NOT_OK);
    case IFRPacket_LongDescriptor::NoData_C:
        requestLongChunk(m_readoffset, preferred_read_size, &encoding);
        DBUG_RETURN(IFR_NEED_DATA);
    }
    
    if(m_dirty) {
        m_clink->error().setRuntimeError(IFR_ERR_GETVAL_REQUIRED);
        DBUG_RETURN(IFR_NOT_OK);
    }
    
    if(m_maxlength > preferred_read_size) {
        m_clink->error().setRuntimeError(IFR_ERR_LONG_LONG_I, (IFR_Int4)getColumn());
        DBUG_RETURN(IFR_NOT_OK);
        
    }

    char *target_buffer = (char *)alloca(preferred_read_size + 1);        
    if(m_maxlength == 0) {
        *target_buffer = '\0';
    } else {
        // reduce size when there is less available
        if(preferred_read_size > m_maxlength) {
            preferred_read_size = m_maxlength;
        }
        
        // check start of data to be 1, and the length enough to do the thing in one.
        IFR_Length datastart = m_longdesc.internpos - m_longdesc.vallen;
        if(datastart != 1 || (m_longdesc.vallen < preferred_read_size)) {
            requestLongChunk(m_readoffset, preferred_read_size, &encoding);
            DBUG_RETURN(IFR_NEED_DATA);
        }
        
        // ok, enough bytes to read
        char *src = (char *) datapart.GetReadData(0) + m_longdesc.valpos - 1;
        tsp00_Uint4 bytes_written = 0;
        tsp00_Uint4 srcbytes_parsed = 0;
        tsp78ConversionResult conv_res;
        conv_res = sp78convertBuffer(IFR_ENCODING(IFR_StringEncodingAscii),
                                     target_buffer,
                                     preferred_read_size + 1,
                                     &bytes_written,
                                     IFR_ENCODING(m_encoding),
                                     src,
                                     m_longdesc.vallen,
                                     &srcbytes_parsed);
        switch(conv_res) {
        case sp78_TargetExhausted:
        case sp78_SourceExhausted:
        case sp78_Ok: {
            target_buffer[bytes_written] = 0;
            break;
        }
        default:
            m_clink->error().setRuntimeError(IFR_ERR_CORRUPTED_UCS2ASCIIDATA_I, (IFR_Int4)getColumn());
            DBUG_RETURN(IFR_NOT_OK);
        }
    }
    IFR_Retcode rc;
    switch(hosttype) {
    case IFR_HOSTTYPE_DOUBLE: {
        IFRConversion_FromString<double> cnv(getColumn());
        rc = cnv.convert(*(double *)data, lengthindicator, target_buffer, m_clink->error());
        break;
    }
    case IFR_HOSTTYPE_FLOAT: {
        IFRConversion_FromString<float> cnv(getColumn());
        rc = cnv.convert(*(float *)data, lengthindicator, target_buffer, m_clink->error());
        break;
    }
    case IFR_HOSTTYPE_INT1: {
        IFRConversion_FromString<IFR_Int1> cnv(getColumn());
        rc = cnv.convert(*(IFR_Int1 *)data, lengthindicator, target_buffer, m_clink->error());
        break;
    }
    case IFR_HOSTTYPE_INT2: {
        IFRConversion_FromString<IFR_Int2> cnv(getColumn());
        rc = cnv.convert(*(IFR_Int2 *)data, lengthindicator, target_buffer, m_clink->error());
        break;
    }
    case IFR_HOSTTYPE_INT4: {
        IFRConversion_FromString<IFR_Int4> cnv(getColumn());
        rc = cnv.convert(*(IFR_Int4 *)data, lengthindicator, target_buffer, m_clink->error());
        break;
    }
    case IFR_HOSTTYPE_INT8: {
        IFRConversion_FromString<IFR_Int8> cnv(getColumn());
        rc = cnv.convert(*(IFR_Int8 *)data, lengthindicator, target_buffer, m_clink->error());
        break;
    }
    case IFR_HOSTTYPE_UINT1: {
        IFRConversion_FromString<IFR_UInt1> cnv(getColumn());
        rc = cnv.convert(*(IFR_UInt1 *)data, lengthindicator, target_buffer, m_clink->error());
        break;
    }
    case IFR_HOSTTYPE_UINT2: {
        IFRConversion_FromString<IFR_UInt2> cnv(getColumn());
        rc = cnv.convert(*(IFR_UInt2 *)data, lengthindicator, target_buffer, m_clink->error());
        break;
    }
    case IFR_HOSTTYPE_UINT4: {
        IFRConversion_FromString<IFR_UInt4> cnv(getColumn());
        rc = cnv.convert(*(IFR_UInt4 *)data, lengthindicator, target_buffer, m_clink->error());
        break;
    }
    case IFR_HOSTTYPE_UINT8: {
        IFRConversion_FromString<IFR_UInt8> cnv(getColumn());
        rc = cnv.convert(*(IFR_UInt8 *)data, lengthindicator, target_buffer, m_clink->error());
        break;
    }
    case IFR_HOSTTYPE_ODBCDATE: {
        IFRConversion_FromString<SQL_DATE_STRUCT> cnv(getColumn(), m_clink->getConnection()->getDateTimeFormat());
        rc = cnv.convert(*(SQL_DATE_STRUCT *)data, lengthindicator, target_buffer, m_clink->error());
        break;
    }
    case IFR_HOSTTYPE_ODBCTIME: {
        IFRConversion_FromString<SQL_TIME_STRUCT> cnv(getColumn(), m_clink->getConnection()->getDateTimeFormat());
        rc = cnv.convert(*(SQL_TIME_STRUCT *)data, lengthindicator, target_buffer, m_clink->error());
        break;
    }
    case IFR_HOSTTYPE_ODBCTIMESTAMP: {
        IFRConversion_FromString<SQL_TIMESTAMP_STRUCT> cnv(getColumn(), m_clink->getConnection()->getDateTimeFormat());
        rc = cnv.convert(*(SQL_TIMESTAMP_STRUCT *)data, lengthindicator, target_buffer, m_clink->error());
        break;
    }
    case IFR_HOSTTYPE_ODBCNUMERIC: {
        IFRConversion_FromString<SQL_NUMERIC_STRUCT> cnv(getColumn());
        rc = cnv.convert(*(SQL_NUMERIC_STRUCT *)data, lengthindicator, target_buffer, m_clink->error());
        break;
    }
    default: {
        m_clink->error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)getColumn());
        rc = IFR_NOT_OK;
    }
    }
    DBUG_RETURN(checkStoreLongData(datapart, rc));
}
                                       

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_Getval::transferStream(IFRPacket_DataPart& datapart,        // data part to read from
                                     char               *data,            // target data
                                     IFR_Length          datalength,      // number of bytes to read
                                     IFR_Length         *lengthindicator, // length indicator                  (output)
                                     IFR_Length         &offset,          // offset IN TARGET
                                     IFR_StringEncoding  encoding,
                                     IFR_Bool            terminate,
                                     IFR_Bool            ascii7bit)          
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Getval, transferStream, m_clink);
    DBUG_PRINT(datalength);
    DBUG_PRINT(lengthindicator);
    DBUG_PRINT(offset);
    DBUG_PRINT(encoding);
    DBUG_PRINT(terminate);
    
    IFR_StringEncoding source_encoding = m_encoding;
    if(encoding == IFR_StringEncodingUTF8 && (m_datatype == IFR_SQLTYPE_STRB || m_datatype==IFR_SQLTYPE_LONGB) ) {
        source_encoding = IFR_StringEncodingUTF8;
    }

    int terminatorSize = 0;
    if(terminate) {
        switch(encoding) {
        case IFR_StringEncodingAscii:
        case IFR_StringEncodingUTF8:
            terminatorSize = 1;
            break;
        default:
            terminatorSize = 2;
        }
    }
    
    IFR_Length used_datalength = datalength - terminatorSize;
    // Someone bound a very short array ...
    if(datalength < terminatorSize) {
        used_datalength = 0;
    }
    switch(m_longdesc.valmode) {
    case IFRPacket_LongDescriptor::Error_C:
        m_clink->error().setRuntimeError(IFR_ERR_ERROR_LONGDESCRIPTOR);
        DBUG_RETURN(IFR_NOT_OK);
    case IFRPacket_LongDescriptor::StartposInvalid_C:
        m_clink->error().setRuntimeError(IFR_ERR_INVALID_STARTPOSITION_I, (IFR_Int4)m_longdesc.internpos);
        DBUG_RETURN(IFR_NOT_OK);
    case IFRPacket_LongDescriptor::NoData_C:
        IFR_DEBUG_TRACE << "NO DATA" << endl;
        // Update the descriptor such that 'datalength' bytes are requested 
        // next time, and from the offset that was requested.
        requestLongChunk(m_readoffset, used_datalength - offset, &encoding);
        DBUG_RETURN(IFR_NEED_DATA);
    default: {
        if(m_dirty) {
            m_clink->error().setRuntimeError(IFR_ERR_GETVAL_REQUIRED);
            DBUG_RETURN(IFR_NOT_OK);
        }
        
        // If there is no data, return OK/NO DATA FOUND depending on the
        // offset.
        if(m_maxlength == 0) {
            if(lengthindicator) {
                *lengthindicator = 0;
                if (terminate && (used_datalength >= terminatorSize)) {
                  memset(data + offset, 0, terminatorSize);
                }
            }
            DBUG_RETURN(IFR_OK);
        }
        
        IFR_Length datastart = m_longdesc.internpos - m_longdesc.vallen; // Smallest 1-based position
        IFR_Length dataend   = m_longdesc.internpos;
        
        DBUG_PRINT(datastart);
        DBUG_PRINT(dataend);
        
        // Look whether the data is in the current part (even partial).
        if(m_readoffset < datastart  || m_readoffset >= dataend) {
            // ... it is not, so look whether there is such data, anyway.
            if(m_readoffset > m_maxlength ) {
                // it is not, 
                if(lengthindicator) {
                    *lengthindicator = 0;
                }
                DBUG_RETURN(IFR_NO_DATA_FOUND);
            } else {
                requestLongChunk(m_readoffset, used_datalength - offset, &encoding) ;
                DBUG_RETURN(IFR_NEED_DATA);
            }
        } else {
            // OK, there's some work to do ...
            IFR_Retcode rc = IFR_OK;
            IFR_Length  required_datalength = used_datalength - offset; // we have already 'offset' bytes in the data.

            // ... or maybe not?
            if(required_datalength == 0) {
                if(lengthindicator) {
                    *lengthindicator = m_maxlength + 1 - m_readoffset;
                    if(m_bin2hex) {
                        (*lengthindicator) *= 2;
                    } else {
                        switch(encoding) {
                        case IFR_StringEncodingAscii:
                            if(m_datatype == IFR_SQLTYPE_STRUNI || m_datatype==IFR_SQLTYPE_LONGUNI) {
                                *lengthindicator /= 2;
                            }
                            break;
                        case IFR_StringEncodingUCS2:
                        case IFR_StringEncodingUCS2Swapped:
                            if(m_datatype != IFR_SQLTYPE_STRUNI && m_datatype != IFR_SQLTYPE_LONGUNI) {
                                *lengthindicator *= 2;
                            }
                            break;
                        case IFR_StringEncodingUTF8:  // In case of DATA_TRUNC, it will be later corrected
                            // to IFR_NO_TOTAL
                            *lengthindicator = 0;
                            break;
                        }
                    }
                }
                
                if(m_readoffset > m_maxlength) {
                    DBUG_RETURN(checkStoreLongData(datapart, IFR_OK));
                } else {
                    if(lengthindicator && encoding == IFR_StringEncodingUTF8 && !m_bin2hex) {
                        *lengthindicator = IFR_NO_TOTAL;
                    }
                    DBUG_RETURN(checkStoreLongData(datapart, IFR_DATA_TRUNC));
                }
            }


            IFR_Length  available_bytes = m_longdesc.vallen - m_readoffset + datastart; // the data length in bytes available
                  // in this chunk in case  the data is not read from the beginning of this long
            
            // get the pointer to the data
            char *src = (char *) datapart.GetReadData(0) 
                + m_longdesc.valpos - 1     // the position in the part
                + m_readoffset - datastart;    // the position within the long to read from (not in the chunk)
            char *dest = data + offset;  // The destination buffer.
            
            tsp00_Uint4 bytes_written = 0;
            tsp00_Uint4 srcbytes_parsed = 0;
            tsp78ConversionResult conv_res;
            if(m_bin2hex) {
                conv_res = IFRUtil_BinaryToHex(IFR_ENCODING(encoding), // target encoding
                                               dest,                   // target
                                               required_datalength,    // length of target buffer (incl. terminator)
                                               &bytes_written,         // bytes written (incl. terminator)
                                               src,                     // source
                                               available_bytes,         // bytes available in the source
                                               &srcbytes_parsed);
            } else {
                conv_res = sp78convertBuffer(IFR_ENCODING(encoding), // target encoding
                                             dest,                   // target
                                             required_datalength,    // length of target buffer (incl. terminator)
                                             &bytes_written,         // bytes written (incl. terminator)
                                             IFR_ENCODING(source_encoding), // source encoding
                                             src,                     // source
                                             available_bytes,         // bytes available in the source
                                             &srcbytes_parsed);       // bytes parsed in the source
            }
            
            switch(conv_res) {
            case sp78_TargetExhausted:
            case sp78_SourceExhausted:
            case sp78_Ok: {
                memset(dest + bytes_written, 0, terminatorSize); 
                
                // ascii check for oms type
                if(ascii7bit) {
                    if(!IFRConversion_StringIsAscii7((unsigned char *)dest, bytes_written)) {
                        m_clink->error().setRuntimeError(IFR_ERR_NOT_ASCII_CHARACTERS_I, (IFR_Int4)getColumn());
                        DBUG_RETURN(IFR_NOT_OK);
                    }
                }

                // The length indicator will only be updated if it hasn't been 
                // set before.
                if(lengthindicator && *lengthindicator==0) {
                    *lengthindicator = m_maxlength + 1 - m_readoffset;
                    if(m_bin2hex) {
                        (*lengthindicator) *= 2; 
                    } else {
                        switch(encoding) {
                        case IFR_StringEncodingAscii:
                            if(m_datatype == IFR_SQLTYPE_STRUNI || m_datatype==IFR_SQLTYPE_LONGUNI) {
                                *lengthindicator /= 2;
                            }
                            break;
                        case IFR_StringEncodingUCS2:
                        case IFR_StringEncodingUCS2Swapped:
                            if(m_datatype != IFR_SQLTYPE_STRUNI && m_datatype != IFR_SQLTYPE_LONGUNI) {
                                *lengthindicator *= 2;
                            }
                            break;
                        case IFR_StringEncodingUTF8:  // In case of DATA_TRUNC, it will be later corrected
                            // to IFR_NO_TOTAL
                            *lengthindicator = bytes_written;
                            break;
                        }
                    }
                }

                // The read offset must advance by the bytes parsed in the conversion.
                m_readoffset += srcbytes_parsed;
                // The write offset must advance by the bytes in the output buffer.
                offset       += bytes_written;

                // If we wrote less than the number of bytes 
                // that is in the output buffer ...
                if(((IFR_Length)bytes_written < required_datalength) && 
                   conv_res != sp78_TargetExhausted) {
                    // If we are simply at the end, return, 
                    if(dataend > m_maxlength) {
                        DBUG_RETURN(checkStoreLongData(datapart, IFR_OK));
                    } else {
                        // otherwise ask for more data. We start at read offset, 
                        requestLongChunk(m_readoffset, required_datalength - bytes_written, &encoding);
                        DBUG_RETURN(IFR_NEED_DATA);
                    }
                } else {
                    if(m_readoffset > m_maxlength) {
                        DBUG_RETURN(checkStoreLongData(datapart, IFR_OK));
                    } else {
                        if(lengthindicator && encoding == IFR_StringEncodingUTF8 && !m_bin2hex) {
                            *lengthindicator = IFR_NO_TOTAL;
                        }
                        DBUG_RETURN(checkStoreLongData(datapart, IFR_DATA_TRUNC));
                    }
                }
            }
            default:
                if(m_bin2hex) {
                    m_clink->error().setRuntimeError(IFR_ERR_BYTE_CONVERSION_FAILED_I, (IFR_Int4)getColumn());
                } else {
                    if(encoding == IFR_StringEncodingUTF8) {
                        m_clink->error().setRuntimeError(IFR_ERR_CORRUPTED_UTF8DATA_I, (IFR_Int4)getColumn());
                    } else {
                        m_clink->error().setRuntimeError(IFR_ERR_CORRUPTED_UCS2ASCIIDATA_I, (IFR_Int4)getColumn());
                    }
                }
            }
        }
           
    }
    }
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------
void
IFRConversion_Getval::requestLongChunk(IFR_Int4 start, IFR_Int4 length, IFR_StringEncoding *encoding)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Getval, requestLongChunk, m_clink);
    DBUG_PRINT(start);
    DBUG_PRINT(length);
    if(encoding) {
        DBUG_PRINT(*encoding);
    } 
    
    // Keep the LONG open after it has been read.
    setNoClose(); 
    // Erase stored LONG data, as we are about to fetch new data
    clearLongData();
    
    IFR_Int4 minlongfetchsize = m_clink->getConnection()->getMinimumLongFetchSize();
    
    m_longdesc.internpos = start;
    if(encoding == 0) {
        m_longdesc.vallen    = length;
    } else {
        switch(*encoding) {
        case IFR_StringEncodingAscii:
            if(m_datatype == IFR_SQLTYPE_LONGUNI || m_datatype == IFR_SQLTYPE_STRUNI) {
                // target ascii, database unicode, we must get double the bytes we
                // have at host
                length = length << 1;
                if(length < 0) { 
                    m_longdesc.vallen = MAX_IFR_INT4 - 1; // largest *EVEN* value possible.
                } else {
                    m_longdesc.vallen = length;
                }
            } else {
                // ascii to ascii
                m_longdesc.vallen = length;
            }
            break;
            
        case IFR_StringEncodingUCS2Swapped:
        case IFR_StringEncodingUCS2:
            if(m_datatype == IFR_SQLTYPE_LONGUNI || m_datatype == IFR_SQLTYPE_STRUNI) {
                // ucs2 to ucs2 (possibly with swapping). we have just to keep
                // an eye that the size is even.
                m_longdesc.vallen = length - (length % 2);
            } else {
                // target ucs2, source ascii
                // we must require only *half* the length from the LONG
                m_longdesc.vallen = length / 2;
            }
            break;
            
        case IFR_StringEncodingUTF8:    
            // oh no. this is something we really have no exact idea about.
            if(m_datatype == IFR_SQLTYPE_LONGUNI || m_datatype == IFR_SQLTYPE_STRUNI) {
                length = length << 1;
                if(length < 0) { 
                    m_longdesc.vallen = MAX_IFR_INT4 - 1; // largest *EVEN* value possible.
                } else {
                    m_longdesc.vallen = length;
                }
            } else {
                m_longdesc.vallen = length;
            }
        }
    }

    if(m_longdesc.vallen < minlongfetchsize) {
        m_longdesc.vallen = minlongfetchsize;
    }

    m_dirty = true;
}


//----------------------------------------------------------------------
void 
IFRConversion_Getval::clearLongData()
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Getval, clearLongData, m_clink);
    if(m_longdata) {
        IFRUtil_Delete(m_longdata, m_longdata->allocator);
        m_longdata = 0;
    }
    return;
}

//----------------------------------------------------------------------
void
IFRConversion_Getval::setLongData(IFRPacket_DataPart& longdatapart, 
                                  SAPDBMem_IRawAllocator& allocator, 
                                  IFR_Bool& memory_ok)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Getval, setLongData, m_clink);
    if(memory_ok) {
        clearLongData();
        IFR_size_t copysize = m_longdesc.vallen + sizeof(tsp1_part_header);
        IFR_Byte  *source   = (IFR_Byte *) longdatapart.GetRawPart();
        IFR_Byte  *source2  = (IFR_Byte *) longdatapart.GetReadData() + m_longdesc.valpos - 1;
        IFR_Byte  *dest     = (IFR_Byte *) IFR_ALLOCATOR(allocator).Allocate(copysize);
        if(dest == 0) {
            memory_ok = false;
            return;
        }
        memcpy(dest, source, sizeof(tsp1_part_header));
        memcpy(dest + sizeof(tsp1_part_header), source2, m_longdesc.vallen);
        m_longdesc.valpos = 1;
        m_longdata = new IFR_ALLOCATOR(allocator) IFRConversion_GetvalDataPart(dest,
                                                                               allocator,
                                                                               longdatapart.getEncoding());
        if(m_longdata == 0) {
            IFR_ALLOCATOR(allocator).Deallocate(dest);
            memory_ok = false;
            return;
        }
    }
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_Getval::checkStoreLongData(IFRPacket_DataPart& datapart,
                                         IFR_Retcode originalRetcode)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Getval, checkStoreLongData, m_clink);
    DBUG_PRINT(originalRetcode);
    // Check if we have to keep the old data.
    if(&datapart != m_longdata) {
        if(datapart.getPartKind() == IFRPacket_PartKind::Longdata_C) {
            IFR_Bool memory_ok=true;
            setLongData(datapart, m_clink->getConnection()->allocator, memory_ok);
            if(!memory_ok) {
                m_clink->error().setMemoryAllocationFailed();
                DBUG_RETURN(IFR_NOT_OK);
            }
        }
    }
    DBUG_RETURN(originalRetcode);
    
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_Getval::close()
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Getval, close, m_clink);
    if(m_status == NeedsClose_C) {
        m_status = Closed_C;
        IFR_Connection *c = m_clink->getConnection();
        m_longdesc.valmode = IFRPacket_LongDescriptor::Close_C;
        IFR_Bool memory_ok=true;
        c->dropLongDescriptor(m_longdesc, memory_ok);
        if(!memory_ok) {
            m_clink->error().setMemoryAllocationFailed();
            DBUG_RETURN(IFR_NOT_OK);
        } 
        DBUG_RETURN(IFR_OK);
    } else {
        DBUG_RETURN(IFR_OK);
    }
}
  

//----------------------------------------------------------------------
IFR_TraceStream&
operator << (IFR_TraceStream& s, const IFRConversion_Getval& p)
{
    char tmp[50];
    sp77sprintf(tmp, sizeof(tmp), "IFRConversion_Getval@%p", &p);
    return s << tmp;
}

IFR_END_NAMESPACE
