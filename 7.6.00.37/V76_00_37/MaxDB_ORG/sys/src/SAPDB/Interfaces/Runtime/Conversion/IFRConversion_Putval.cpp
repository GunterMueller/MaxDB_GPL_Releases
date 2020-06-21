/*!
  @file           IFRConversion_Putval.cpp
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

#include "Interfaces/Runtime/IFR_Common.h"
#include "Interfaces/Runtime/Conversion/IFRConversion_Putval.h"
#include "Interfaces/Runtime/Conversion/IFRConversion_Converter.h"
#include "Interfaces/Runtime/IFR_Connection.h"
#include "Interfaces/Runtime/Conversion/IFRConversion_ToString.h"

IFR_BEGIN_NAMESPACE

//----------------------------------------------------------------------
static IFR_size_t string_nlen(char *s, IFR_size_t max)
{
    for(IFR_size_t i=0; i<max; ++i) {
        if(*(s+i) == 0) {
            return i;
        }
    }
    return max;
}

//----------------------------------------------------------------------
static IFR_size_t ucs2string_nlen(char *s, IFR_size_t max)
{
    for(IFR_size_t i=0; i<max - max%2; i += 2) {
        if(s[i] == 0 && s[i+1]==0) {
            return i;
        }
    }
    return max;
}

//----------------------------------------------------------------------
static inline IFR_size_t compute_input_datalength(IFR_size_t datalength,
                                                  IFR_Length *lengthindicator,
                                                  IFR_HostType hosttype,
                                                  char      *data,
                                                  IFR_ConnectionItem& clink,
                                                  IFR_Int4            index,
                                                  IFR_Retcode&        rc)
{
    rc = IFR_OK;
    if(lengthindicator) {
        if(*lengthindicator==IFR_NTS) {
            if(datalength != 0) {
              if ((hosttype == IFR_HOSTTYPE_UCS2_NATIVE) ||
                  (hosttype == IFR_HOSTTYPE_UCS2) ||
                  (hosttype == IFR_HOSTTYPE_UCS2_SWAPPED)) {
                  return ucs2string_nlen(data, datalength);
              } else {
                  return string_nlen(data, datalength);
              }
            } else {
                return strlen(data);
            }
        } else if(*lengthindicator < 0) {
            clink.error().setRuntimeError(IFR_ERR_INVALID_LENGTHINDICATOR_I, index);
            rc = IFR_NOT_OK;
            return 0;
        } else if (*lengthindicator >= 0) {
            if(datalength != 0) {
                return  (IFR_Length) datalength < *lengthindicator ? datalength : *lengthindicator;
            } else {
                return *lengthindicator;
            }
        } 
    } 
    if(datalength != 0) {
        if ((hosttype == IFR_HOSTTYPE_UCS2_NATIVE) ||
            (hosttype == IFR_HOSTTYPE_UCS2) ||
            (hosttype == IFR_HOSTTYPE_UCS2_SWAPPED)) {
            return ucs2string_nlen(data, datalength);
        } else {
            return string_nlen(data, datalength);
        }
    } else {
        return strlen(data);
    }
}

//-------------------------------------------------------------------
IFRConversion_Putval::IFRConversion_Putval()
: m_hosttype(IFR_HOSTTYPE_PARAMETER_NOTSET),
  m_data(0),
  m_datalength(0),
  m_lengthindicator(0),
  m_clink(0),
  m_dataend(0),
  m_datapos(0),
  m_longdesc_in_dp(0),
  m_bin2hex(false),
  m_atend(false),
  m_dyn_data(0)
{
    // initialise the long descriptor 
    memset((void*)&m_longdesc, 0, sizeof(IFRPacket_LongDescriptor));
    m_longdesc.internpos  = 1;
    m_longdesc.valmode    = IFRPacket_LongDescriptor::NoData_C;
}

//-------------------------------------------------------------------
IFRConversion_Putval::IFRConversion_Putval(IFR_Int2 column,
                                           IFR_Int4 row,
                                           IFR_ShortInfo& shortinfo,
                                           IFR_HostType hosttype,
                                           char    * data,
                                           IFR_Length  datalength,
                                           IFR_Length* lengthindicator,
                                           IFR_ConnectionItem& clink,
                                           IFR_Bool bin2hex,
                                           IFR_StringEncoding packetEncoding)
:m_column(column),
 m_row(row),
 m_shortinfo(shortinfo),
 m_hosttype(hosttype),
 m_data(data),
 m_datalength(datalength),
 m_lengthindicator(lengthindicator),
 m_clink(&clink),
 m_dataend(0),
 m_datapos(0),
 m_longdesc_in_dp(0),
 m_bin2hex(bin2hex),
 m_atend(false),
 m_dyn_data(0)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Putval, IFRConversion_Putval, m_clink);
    DBUG_PRINT(datalength);
    DBUG_PRINT(lengthindicator);

    // initialise the long descriptor 
    memset((void*)&m_longdesc, 0, sizeof(IFRPacket_LongDescriptor));
    m_longdesc.internpos  = 1;
    m_longdesc.valmode    = IFRPacket_LongDescriptor::NoData_C;
    if(IFR_HostTypeIsLOB(m_hosttype)) {
        m_longdesc.infoset = IFRPacket_LongDescriptor::NoClose_C;
    }

    if(shortinfo.isUnicode()) {
        m_encoding = IFR_StringEncodingUCS2Native;
    } else {
        if (shortinfo.isBinary() || packetEncoding == IFR_StringEncodingAscii) {
          m_encoding = IFR_StringEncodingAscii;
        } else{
          m_encoding = IFR_StringEncodingUCS2Native;
        }
    }

    if(IFR_HostTypeIsIntegral(hosttype)) {
        convertDataToString();
        m_sourceencoding = IFR_StringEncodingAscii;
    } else {
        if (m_hosttype == SQLDBC_HOSTTYPE_BINARY ||
            m_hosttype == SQLDBC_HOSTTYPE_BLOB) {
            m_sourceencoding = m_encoding;
        } else {
            m_sourceencoding = IFR_Parameter::getEncoding(m_hosttype);
        }
    }
}


//----------------------------------------------------------------------
IFRConversion_Putval::~IFRConversion_Putval()
{
    if(m_dyn_data) {
        if(m_clink) {
            m_clink->getConnection()->allocator.Deallocate(m_dyn_data);
        }
    }
}


//----------------------------------------------------------------------
void
IFRConversion_Putval::putDescriptor(IFRPacket_DataPart& datapart)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Putval, putDescriptor, m_clink);

    m_longdesc_in_dp = datapart.getInputData(m_shortinfo);
    datapart.addBinaryParameter(&m_longdesc,
                                IFRPacket_LongDescriptor::Size,
                                m_shortinfo);
    
    DBUG_TRACE << "PUT DESCRIPTOR (DATA): " << m_longdesc << endl;
    
    return;
}

                 
//----------------------------------------------------------------------
void
IFRConversion_Putval::putDescriptor(IFRPacket_LongDataPart& datapart)

{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Putval, putDescriptor, m_clink);

    m_longdesc_in_dp = datapart.GetRawPart()->sp1p_buf().asCharp() 
        + datapart.GetRawPart()->sp1p_buf_len() + 1;
    datapart.addDescriptor(m_longdesc);
    DBUG_TRACE << "PUT DESCRIPTOR (LONGDATA): " << m_longdesc << endl;
    return;
}

//----------------------------------------------------------------------.
void
IFRConversion_Putval::setValIndex(IFR_Int2 valindex)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Putval, setValIndex, m_clink);
    DBUG_PRINT(valindex);
    
    m_longdesc.valind = valindex;
    if(m_longdesc_in_dp) {
        memcpy(m_longdesc_in_dp + IFRPacket_LongDescriptor::ValInd_O,
               &valindex,
               sizeof(IFR_Int2));
    }
    return;
}



//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_Putval::transferStream(IFRPacket_DataPart& datapart)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Putval, transferStream_DataPart, m_clink);
    
    // That method shall not be called if the data is provided at execution.
    if(IFR_Parameter::hasDataAtExecute(m_lengthindicator)) {
        m_clink->error().setRuntimeError(IFR_ERR_NOPIECEWISE_DATA_WRITE_I, 
                                         (IFR_Int4)getColumn());
        DBUG_RETURN(IFR_NOT_OK);
    }
    
    // compute m_dataend if necessary
    if(m_dataend == 0) {
        // dataend is always 0 if we have a real LOB type.
        // we have nothing to do in this case ...
        if(IFR_HostTypeIsLOB(m_hosttype)) {
            DBUG_RETURN(IFR_OK);
        }
    	if(computeDataEnd()) {
    		DBUG_RETURN(IFR_NOT_OK);
    	}
    }
    
    IFR_Retcode rc = IFR_OK;
    if(m_dataend == m_data) {
        datapart.addEmptyStream(m_longdesc_in_dp, false);
        m_atend = true;
        rc=IFR_OK;
    } else {
	    IFR_Bool alldata = m_datapos == m_data;    
      if(m_shortinfo.isBinary()) {
          rc=datapart.addStreamData(m_datapos, m_dataend, 
                                    m_longdesc_in_dp, 
                                    IFR_StringEncodingAscii, 
                                    IFR_StringEncodingAscii, 
                                    *m_clink, 
                                    m_bin2hex);
          
      } else {
          rc=datapart.addStreamData(m_datapos, m_dataend, 
                                    m_longdesc_in_dp, 
                                    m_sourceencoding, 
                                    m_encoding, 
                                    *m_clink, 
                                    m_bin2hex);
      }   
      if(rc == IFR_OK) {
    	    if(alldata) {
            	datapart.setStreamValMode(m_longdesc_in_dp,
                	                      IFRPacket_LongDescriptor::AllData_C);
        	} else {
            	datapart.setStreamValMode(m_longdesc_in_dp,
                	                      IFRPacket_LongDescriptor::LastData_C);
        	}
    	}
    }
    
//     IFRPacket_LongDescriptor ldtrace;
//     memcpy(&ldtrace, m_longdesc_in_dp, 40);
//     DBUG_TRACE << "Long Descriptor after adding data: " << ldtrace << endl;
    
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_Putval::putCloseDescriptor(IFRPacket_LongDataPart& longdata, IFR_Bool appendLastPutval)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Putval, putCloseDescriptor, m_clink);
    putDescriptor(longdata);
    longdata.addEmptyStream(m_longdesc_in_dp, true);
    
    if(appendLastPutval) {
        putDescriptor(longdata);
        longdata.setStreamValMode(m_longdesc_in_dp, IFRPacket_LongDescriptor::LastPutval_C);
    }
    
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_Putval::transferStream(IFRPacket_LongDataPart& longdatapart, IFR_Bool& empty)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Putval, transferStream_LongDataPart, m_clink);
    
    // That method shall not be called if the data is provided at execution.
    if(IFR_Parameter::hasDataAtExecute(m_lengthindicator)) {
        m_clink->error().setRuntimeError(IFR_ERR_NOPIECEWISE_DATA_WRITE_I, 
                                         (IFR_Int4) getColumn());
        DBUG_RETURN(IFR_NOT_OK);
    }

	// We will do this only if there is at least room for the 
	// descriptor and 1 data byte. 
	if((longdatapart.getRemainingBytes() & ~7) 
		 	<= IFRPacket_LongDescriptor::Size + 1) {
		DBUG_RETURN(IFR_DATA_TRUNC);
	}
    
    // compute m_dataend if necessary
    if(m_dataend == 0) {
    	if(computeDataEnd()) {
    		DBUG_RETURN(IFR_NOT_OK);
    	}
    }
    
	// we now have at first to put the descriptor, and update 
	// m_longdesc_in_dp. As we add a descriptor, we must add
    // also increment the number of part arguments
	putDescriptor(longdatapart);
	
	// Now continue similar to execute.
    IFR_Retcode rc=IFR_OK;
    if(m_dataend == m_data) {
        longdatapart.addEmptyStream(m_longdesc_in_dp, false);
        m_atend = empty = true;        
        rc=IFR_OK;
    } else {
	    IFR_Bool alldata = m_datapos == m_data;    
      if(m_shortinfo.isBinary()) {
          rc=longdatapart.addStreamData(m_datapos, m_dataend, m_longdesc_in_dp, IFR_StringEncodingAscii, IFR_StringEncodingAscii, 
                                        *m_clink, m_bin2hex);    
      } else {
          rc=longdatapart.addStreamData(m_datapos, m_dataend, m_longdesc_in_dp, m_sourceencoding, m_encoding, *m_clink, m_bin2hex);    
      }
    	// 
	    if(rc == IFR_OK) {
    	    if(alldata) {
            	longdatapart.setStreamValMode(m_longdesc_in_dp,
                	                      IFRPacket_LongDescriptor::AllData_C);
        	} else {
            	longdatapart.setStreamValMode(m_longdesc_in_dp,
                	                      IFRPacket_LongDescriptor::LastData_C);
        	}
    	}
    }

//     IFRPacket_LongDescriptor ldtrace;
//     memcpy(&ldtrace, m_longdesc_in_dp, 40);
//     DBUG_TRACE << "LONG descriptor after adding data for PUTVAL:" << ldtrace << endl;
    
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_Putval::transferStream(IFRPacket_DataPart& datapart,
                                     char               *data,
                                     IFR_Length            datalength,
                                     IFR_Length           *lengthindicator,
                                     IFR_Bool              ascii7bit,
                                     IFR_Length         &offset)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Putval, transferStream_DataPartRawData, m_clink);
    DBUG_PRINT(datalength);
    DBUG_PRINT(lengthindicator);
    DBUG_PRINT(offset);
    
    IFR_Retcode rc=IFR_OK;
    
    IFR_Bool isLongData = datapart.getPartKind() == IFRPacket_PartKind::Longdata_C;
    
    // The needed space depends on what kind of part it is, and whether
    // the descriptor needs to be put in.
    IFR_size_t remainingbytes=datapart.getRemainingBytes() & ~7;

    if(offset || !isLongData) {
        if(remainingbytes < 1) {
            DBUG_RETURN(IFR_DATA_TRUNC);
        } 
    } else {
        if(remainingbytes <= IFRPacket_LongDescriptor::Size + 1) {
            DBUG_RETURN(IFR_DATA_TRUNC);
        }
    }

    // If the offset has been resetted, and it is a longdata part,
    // the LONG descriptor must be inserted.
    if(offset == 0 && isLongData) {
        putDescriptor(static_cast<IFRPacket_LongDataPart&>(datapart));
    }
    
    // now compute the end of the data
    IFR_size_t used_datalength = compute_input_datalength(datalength,
                                                          lengthindicator,
                                                          m_hosttype,
                                                          data,
                                                          *m_clink,
                                                          getColumn(),
                                                          rc);
    char * dataend = data + used_datalength;
    char * datapos = data;

    if(rc) {
        DBUG_RETURN(IFR_NOT_OK);
    }

    if(ascii7bit) {
        if(!IFRConversion_StringIsAscii7((unsigned char *)data, used_datalength)) {
            m_clink->error().setRuntimeError(IFR_ERR_NOT_ASCII_CHARACTERS_I, (IFR_Int4)getColumn());
            DBUG_RETURN(IFR_NOT_OK);
        }
        
    }

    if(dataend == data) {
        datapart.addEmptyStream(m_longdesc_in_dp, false);
        rc = IFR_OK;
    } else {
        if(m_shortinfo.isBinary()) {
            rc = datapart.addStreamData(datapos, dataend, m_longdesc_in_dp, IFR_StringEncodingAscii, IFR_StringEncodingAscii, 
                                        *m_clink, m_bin2hex);
        } else {
            rc = datapart.addStreamData(datapos, dataend, m_longdesc_in_dp, m_sourceencoding, m_encoding, *m_clink, m_bin2hex);
        }
        offset += (datapos - data);
    }

//     IFRPacket_LongDescriptor ldtrace;
//     memcpy(&ldtrace, m_longdesc_in_dp, 40);
//     DBUG_TRACE << "LONG descriptor after adding data for PUTVAL:" << ldtrace << endl;
    
    DBUG_RETURN(rc);
}


//----------------------------------------------------------------------
void
IFRConversion_Putval::updateDescriptor(char *descriptor)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Putval, updateDescriptor, m_clink);
    memcpy(&m_longdesc, descriptor, IFRPacket_LongDescriptor::Size);
    // as this update is for a PUTVAL, the fields VALPOS and VALLEN must be
    // cleared, as the kernel 
    m_longdesc.valpos=0;
    m_longdesc.vallen=0;
    
    DBUG_TRACE << "LONG DESCRIPTOR UPDATED: " << m_longdesc << endl;
    
    return;
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_Putval::computeDataEnd()
{
	DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Putval, computeDataEnd, m_clink);
    IFR_Retcode  rc = IFR_OK;
    IFR_size_t   used_datalength=compute_input_datalength(m_datalength,
                                                          m_lengthindicator,
                                                          m_hosttype,
                                                          m_data,
                                                          *m_clink,
                                                          getColumn(),
                                                          rc);
    DBUG_PRINT(used_datalength);
    m_dataend = m_data + used_datalength;
    m_datapos = m_data;
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
void
IFRConversion_Putval::convertDataToString()
{
    IFR_size_t charsWritten;
    switch(m_hosttype) {
    case IFR_HOSTTYPE_UINT1: {
        IFR_UInt4 tmp_data = *(IFR_UInt1 *)m_data;
        m_dyn_data = (char *) m_clink->getConnection()->allocator.Allocate(sizeof(IFRConversion_ToString<IFR_UInt4>::BufferType));
        IFRConversion_ToString<IFR_UInt4> cnv;
        cnv.convert(tmp_data, m_dyn_data, charsWritten, m_clink->error());
        break;
    }
    case IFR_HOSTTYPE_INT1: {
        IFR_Int4 tmp_data = *(IFR_Int1 *)m_data;
        m_dyn_data = (char *) m_clink->getConnection()->allocator.Allocate(sizeof(IFRConversion_ToString<IFR_Int4>::BufferType));
        IFRConversion_ToString<IFR_Int4> cnv;
        cnv.convert(tmp_data, m_dyn_data, charsWritten, m_clink->error());
        break;
    }
    case IFR_HOSTTYPE_UINT2: {
        m_dyn_data = (char *) m_clink->getConnection()->allocator.Allocate(sizeof(IFRConversion_ToString<IFR_UInt2>::BufferType));
        IFRConversion_ToString<IFR_UInt2> cnv;
        cnv.convert(*(IFR_UInt2 *)m_data, m_dyn_data, charsWritten, m_clink->error());
        break;
    }
    case IFR_HOSTTYPE_INT2: {
        m_dyn_data = (char *) m_clink->getConnection()->allocator.Allocate(sizeof(IFRConversion_ToString<IFR_Int2>::BufferType));
        IFRConversion_ToString<IFR_Int2> cnv;
        cnv.convert(*(IFR_Int2 *)m_data, m_dyn_data, charsWritten, m_clink->error());
        break;
    }
    case IFR_HOSTTYPE_UINT4: {
        m_dyn_data = (char *) m_clink->getConnection()->allocator.Allocate(sizeof(IFRConversion_ToString<IFR_UInt4>::BufferType));
        IFRConversion_ToString<IFR_UInt4> cnv;
        cnv.convert(*(IFR_UInt4 *)m_data, m_dyn_data, charsWritten, m_clink->error());
        break;
    }
    case IFR_HOSTTYPE_INT4: {
        m_dyn_data = (char *) m_clink->getConnection()->allocator.Allocate(sizeof(IFRConversion_ToString<IFR_Int4>::BufferType));
        IFRConversion_ToString<IFR_Int4> cnv;
        cnv.convert(*(IFR_Int4 *)m_data, m_dyn_data, charsWritten, m_clink->error());
        break;
    }
    case IFR_HOSTTYPE_UINT8: {
        m_dyn_data = (char *) m_clink->getConnection()->allocator.Allocate(sizeof(IFRConversion_ToString<IFR_UInt8>::BufferType));
        IFRConversion_ToString<IFR_UInt8> cnv;
        cnv.convert(*(IFR_UInt8 *)m_data, m_dyn_data, charsWritten, m_clink->error());
        break;
    }
    case IFR_HOSTTYPE_INT8: {
        m_dyn_data = (char *) m_clink->getConnection()->allocator.Allocate(sizeof(IFRConversion_ToString<IFR_Int8>::BufferType));
        IFRConversion_ToString<IFR_Int8> cnv;
        cnv.convert(*(IFR_Int8 *)m_data, m_dyn_data, charsWritten, m_clink->error());
        break;
    }
    case IFR_HOSTTYPE_DOUBLE: {
        m_dyn_data = (char *) m_clink->getConnection()->allocator.Allocate(sizeof(IFRConversion_ToString<double>::BufferType));
        IFRConversion_ToString<double> cnv;
        cnv.convert(*((double *)m_data), m_dyn_data, charsWritten, m_clink->error());
        break;
    }
    case IFR_HOSTTYPE_FLOAT: {
        m_dyn_data = (char *) m_clink->getConnection()->allocator.Allocate(sizeof(IFRConversion_ToString<double>::BufferType));
        IFRConversion_ToString<double> cnv;
        double tmp_data = *(float *)m_data;
        cnv.convert(tmp_data, m_dyn_data, charsWritten, m_clink->error());
        break;
    }
    case IFR_HOSTTYPE_ODBCDATE: {
        m_dyn_data = (char *) m_clink->getConnection()->allocator.Allocate(sizeof(IFRConversion_ToString<SQL_DATE_STRUCT>::BufferType));
        IFRConversion_ToString<SQL_DATE_STRUCT> cnv(getColumn(), m_clink->getConnection()->getDateTimeFormat());
        cnv.convert(*(SQL_DATE_STRUCT *)m_data, m_dyn_data, charsWritten, m_clink->error());
        break;
    }
    case IFR_HOSTTYPE_ODBCTIME: {
        m_dyn_data = (char *) m_clink->getConnection()->allocator.Allocate(sizeof(IFRConversion_ToString<SQL_TIME_STRUCT>::BufferType));
        IFRConversion_ToString<SQL_TIME_STRUCT> cnv(getColumn(), m_clink->getConnection()->getDateTimeFormat());
        cnv.convert(*(SQL_TIME_STRUCT *)m_data, m_dyn_data, charsWritten, m_clink->error());
        break;
    }
    case IFR_HOSTTYPE_ODBCTIMESTAMP: {
        m_dyn_data = (char *) m_clink->getConnection()->allocator.Allocate(sizeof(IFRConversion_ToString<SQL_TIMESTAMP_STRUCT>::BufferType));
        IFRConversion_ToString<SQL_TIMESTAMP_STRUCT> cnv(getColumn(), m_clink->getConnection()->getDateTimeFormat());
        cnv.convert(*(SQL_TIMESTAMP_STRUCT *)m_data, m_dyn_data, charsWritten, m_clink->error());
        break;
    }
    case IFR_HOSTTYPE_ODBCNUMERIC: {
        m_dyn_data = (char *) m_clink->getConnection()
            ->allocator.Allocate(sizeof(IFRConversion_ToString<SQL_NUMERIC_STRUCT>::BufferType));
        IFRConversion_ToString<SQL_NUMERIC_STRUCT> cnv(getColumn());
        cnv.convert(*(SQL_NUMERIC_STRUCT *)m_data, m_dyn_data, charsWritten, m_clink->error());
        break;
    }
    default:
        m_clink->error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)m_column);
        return;
    }
    m_data = m_dyn_data;
    m_dataend = m_data + charsWritten;
    m_datapos = m_data;        
    return;
}

//----------------------------------------------------------------------
IFR_TraceStream&
operator << (IFR_TraceStream& s, const IFRConversion_Putval& p)
{
    char tmp[50];
    sp77sprintf(tmp, sizeof(tmp), "IFRConversion_Putval@%p", &p);
    return s << tmp;
}

IFR_END_NAMESPACE
