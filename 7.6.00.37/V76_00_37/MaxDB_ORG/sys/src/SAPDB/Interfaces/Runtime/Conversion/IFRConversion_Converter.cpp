/*!
  @file           IFRConversion_Converter.cpp
  @author         D030044
  @ingroup        IFR_DataConv
  @brief          Conversion of input and output data
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
#include "Interfaces/Runtime/Conversion/IFRConversion_Converter.h"
#include "Interfaces/Runtime/IFR_Connection.h"

IFR_BEGIN_NAMESPACE

//----------------------------------------------------------------------
IFRConversion_Converter::IFRConversion_Converter(IFR_ShortInfo& shortinfo,
                                                 SAPDBMem_IRawAllocator& allocator,
                                                 IFR_Int4 index)
:m_shortinfo(shortinfo),
 m_name(allocator),
 m_index(index)
{
}

//----------------------------------------------------------------------
IFRConversion_Converter::~IFRConversion_Converter()
{
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_Converter::translateInput(IFRPacket_DataPart& part,
                                        IFR_Parameter& parameter,
                                        IFR_ConnectionItem& clink,
                                        IFR_Int4 dataoffset,
                                        IFR_size_t rowsize)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateInput, &clink);

    IFR_HostType parameter_ht=parameter.getHostType();
    IFR_Bool parameter_term=parameter.shouldTerminated();
    // check that parameter is set
    if(parameter_ht == IFR_HOSTTYPE_PARAMETER_NOTSET) {
        clink.error().setRuntimeError(IFR_ERR_PARAMETER_NOT_SET_I, (IFR_Int4)getIndex());
        DBUG_RETURN(IFR_NOT_OK);
    }

    // get the length indicator, retrieve ...
    IFR_Length *parameter_lengthindicator=parameter.getLengthIndicator(dataoffset, rowsize);
    // ... whether we have a special value to insert
    if(parameter_lengthindicator) {
        if(*parameter_lengthindicator == IFR_NULL_DATA) {
            DBUG_RETURN(translateNullInput(part, clink));
        }
        if(*parameter_lengthindicator == IFR_DEFAULT_PARAM) {
            DBUG_RETURN(translateDefaultInput(parameter, part, clink, dataoffset, rowsize));
        }
    }

     // We must bail out if the data is 0 and not supplied at execute.
    if(parameter.data() == 0) {
        switch(parameter_ht) {
        case IFR_HOSTTYPE_BINARY:
        case IFR_HOSTTYPE_ASCII:
        case IFR_HOSTTYPE_OMS_ASCII:
        case IFR_HOSTTYPE_UCS2:
        case IFR_HOSTTYPE_UCS2_SWAPPED:
        case IFR_HOSTTYPE_UTF8:
            if((parameter.getBytesLength() == 0 ||
                (parameter_lengthindicator && *parameter_lengthindicator == 0))
               && !parameter.hasDataAtExecute(dataoffset, rowsize)) {
                break;
            }
        default:
            if(!parameter.hasDataAtExecute(dataoffset, rowsize)) {
                clink.error().setRuntimeError(IFR_ERR_NULL_PARAMETERADDR_I, (IFR_Int4)this->m_index);
                DBUG_RETURN(IFR_NOT_OK);
            }
        }
    }

    // otherwise, it is real data and we must do real work ...
    switch(parameter_ht) {
    case IFR_HOSTTYPE_BINARY:
        DBUG_RETURN(translateBinaryInput(part,
                                         parameter.asChar(dataoffset, rowsize),
                                         parameter.getBytesLength(),
                                         parameter_lengthindicator,
                                         clink));
    case IFR_HOSTTYPE_ASCII:
        DBUG_RETURN(translateAsciiInput(part,
                                        parameter.asChar(dataoffset, rowsize),
                                        parameter.getBytesLength(),
                                        parameter_lengthindicator,
                                        parameter_term,
                                        false,
                                        clink));
    case IFR_HOSTTYPE_OMS_ASCII:
        DBUG_RETURN(translateAsciiInput(part,
                                        parameter.asChar(dataoffset, rowsize),
                                        parameter.getBytesLength(),
                                        parameter_lengthindicator,
                                        parameter_term,
                                        clink.getConnection()->ascii7Bit(),
                                        clink));
    case IFR_HOSTTYPE_UCS2:
        DBUG_RETURN(translateUCS2Input(part,
                                       parameter.asChar(dataoffset, rowsize),
                                       false,
                                       parameter.getBytesLength(),
                                       parameter_lengthindicator,
                                       parameter_term,
                                       clink));
    case IFR_HOSTTYPE_UCS2_SWAPPED:
        DBUG_RETURN(translateUCS2Input(part,
                                       parameter.asChar(dataoffset, rowsize),
                                       true,
                                       parameter.getBytesLength(),
                                       parameter_lengthindicator,
                                       parameter_term,
                                       clink));
    case IFR_HOSTTYPE_UTF8:
        DBUG_RETURN(translateUTF8Input(part,
                                       parameter.asChar(dataoffset, rowsize),
                                       parameter.getBytesLength(),
                                       parameter_lengthindicator,
                                       parameter_term,
                                       clink));
    case IFR_HOSTTYPE_INT1:
        DBUG_RETURN(translateInput(part, parameter.asInt1(dataoffset, rowsize),
                                   parameter_lengthindicator, clink, 0));
    case IFR_HOSTTYPE_INT2:
        DBUG_RETURN(translateInput(part, parameter.asInt2(dataoffset, rowsize),
                                   parameter_lengthindicator, clink, 0));
    case IFR_HOSTTYPE_INT4:
        DBUG_RETURN(translateInput(part, parameter.asInt4(dataoffset, rowsize),
                                   parameter_lengthindicator, clink, 0));
    case IFR_HOSTTYPE_INT8:
        DBUG_RETURN(translateInput(part, parameter.asInt8(dataoffset, rowsize),
                                   parameter_lengthindicator, clink, 0));
    case IFR_HOSTTYPE_UINT1:
        DBUG_RETURN(translateInput(part, parameter.asUInt1(dataoffset, rowsize),
                                   parameter_lengthindicator, clink, 0));
    case IFR_HOSTTYPE_UINT2:
        DBUG_RETURN(translateInput(part, parameter.asUInt2(dataoffset, rowsize),
                                   parameter_lengthindicator, clink, 0));
    case IFR_HOSTTYPE_UINT4:
        DBUG_RETURN(translateInput(part, parameter.asUInt4(dataoffset, rowsize),
                                   parameter_lengthindicator, clink, 0));
    case IFR_HOSTTYPE_UINT8:
        DBUG_RETURN(translateInput(part, parameter.asUInt8(dataoffset, rowsize),
                                   parameter_lengthindicator, clink, 0));
    case IFR_HOSTTYPE_DOUBLE:
        DBUG_RETURN(translateInput(part, parameter.asDouble(dataoffset, rowsize),
                                   parameter_lengthindicator, clink, 0));
    case IFR_HOSTTYPE_FLOAT:
        DBUG_RETURN(translateInput(part, parameter.asFloat(dataoffset, rowsize),
                                   parameter_lengthindicator, clink, 0));
    case IFR_HOSTTYPE_ODBCDATE:
        DBUG_RETURN(translateInput(part, parameter.asSQLDate(dataoffset, rowsize),
                                   parameter_lengthindicator, clink, 0));
    case IFR_HOSTTYPE_ODBCTIME:
        DBUG_RETURN(translateInput(part, parameter.asSQLTime(dataoffset, rowsize),
                                   parameter_lengthindicator, clink, 0));
    case IFR_HOSTTYPE_ODBCTIMESTAMP:
        DBUG_RETURN(translateInput(part, parameter.asSQLTimestamp(dataoffset, rowsize),
                                   parameter_lengthindicator, clink, 0));
    case IFR_HOSTTYPE_ODBCNUMERIC:
        DBUG_RETURN(translateInput(part, parameter.asSQLNumeric(dataoffset, rowsize),
                                   parameter_lengthindicator, clink, 0));
    case IFR_HOSTTYPE_GUID:
        DBUG_RETURN(translateInput(part, parameter.asGUID(dataoffset, rowsize),
                                   parameter_lengthindicator, clink, 0));
    case IFR_HOSTTYPE_BLOB:
        DBUG_RETURN(translateBinaryLOBInput(part,
                                            parameter.asLOBData(dataoffset, rowsize),
                                            parameter_lengthindicator, dataoffset, clink));
    case IFR_HOSTTYPE_ASCII_LOB:
        DBUG_RETURN(translateAsciiLOBInput(part,
                                           parameter.asLOBData(dataoffset, rowsize),
                                           parameter_lengthindicator, dataoffset, clink));
    case IFR_HOSTTYPE_UCS2_LOB:
        DBUG_RETURN(translateUCS2LOBInput(part,
                                          parameter.asLOBData(dataoffset, rowsize),
                                          false,
                                          parameter_lengthindicator,
                                          dataoffset,
                                          clink));
    case IFR_HOSTTYPE_UCS2_SWAPPED_LOB:
        DBUG_RETURN(translateUCS2LOBInput(part,
                                          parameter.asLOBData(dataoffset, rowsize),
                                          true,
                                          parameter_lengthindicator,
                                          dataoffset,
                                          clink));
    case IFR_HOSTTYPE_UTF8_LOB:
        DBUG_RETURN(translateUTF8LOBInput(part,
                                          parameter.asLOBData(dataoffset, rowsize),
                                          parameter_lengthindicator,
                                          dataoffset,
                                          clink));
    case IFR_HOSTTYPE_STREAM:
        DBUG_RETURN(translateABAPStreamInput(part,
                                             parameter.asChar(dataoffset, rowsize),
                                             parameter.getBytesLength(),
                                             parameter_lengthindicator,
                                             dataoffset,
                                             clink));
    case IFR_HOSTTYPE_DECIMAL: 
        DBUG_RETURN(translateDecimalInput(part,
                                          parameter.asChar(dataoffset, rowsize),
                                          parameter.getBytesLength(),
                                          parameter_lengthindicator,
                                          clink,
                                          0));
    case IFR_HOSTTYPE_OMS_PACKED_8_3:
        DBUG_RETURN(translateOmsPacked_8_3_Input(part,
                                                 parameter.asChar(dataoffset, rowsize),
                                                 parameter.getBytesLength(),
                                                 parameter_lengthindicator,
                                                 clink,
                                                 0));
        
    case IFR_HOSTTYPE_OMS_PACKED_15_3:
        DBUG_RETURN(translateOmsPacked_15_3_Input(part,
                                                  parameter.asChar(dataoffset, rowsize),
                                                  parameter.getBytesLength(),
                                                  parameter_lengthindicator,
                                                  clink,
                                                  0));
    case IFR_HOSTTYPE_OMS_TIMESTAMP:
        DBUG_RETURN(translateOmsTimestampInput(part,
                                               parameter.asChar(dataoffset, rowsize),
                                               parameter.getBytesLength(),
                                               parameter_lengthindicator,
                                               clink, 
                                               0));
    default:
        clink.error().setRuntimeError(IFR_ERR_INVALID_HOSTTYPE_SI,
                                      IFR_HostTypeToString(parameter_ht),
                                      (IFR_Int4) getIndex());
        DBUG_RETURN(IFR_NOT_OK);
    }
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_Converter::translateNullInput(IFRPacket_DataPart& datapart,
                                            IFR_ConnectionItem& clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateNullInput, &clink);
    DBUG_RETURN(datapart.addNullInput(m_shortinfo));
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_Converter::translateDefaultInput(IFR_Parameter& parameter,
                                               IFRPacket_DataPart& datapart,
                                               IFR_ConnectionItem& clink,
                                               IFR_Int4 dataoffset,
                                               IFR_size_t rowsize)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateNullInput, &clink);
    DBUG_RETURN(datapart.addDefaultInput(m_shortinfo));
}


//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_Converter::translateBinaryInput(IFRPacket_DataPart& datapart,
                                              char               *data,
                                              IFR_Length            datalength,
                                              IFR_Length*           lengthindicator,
                                              IFR_ConnectionItem & clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateBinaryInput, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_Converter::translateAsciiInput(IFRPacket_DataPart&  datapart,
                                             char                *data,
                                             IFR_Length           datalength,
                                             IFR_Length*          lengthindicator,
                                             IFR_Bool             terminate,
                                             IFR_Bool             ascii7bit,
                                             IFR_ConnectionItem & clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateAsciiInput, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_Converter::translateUCS2Input(IFRPacket_DataPart& datapart,
                                            char               *data,
                                            IFR_Bool           swapped,
                                            IFR_Length            datalength,
                                            IFR_Length*           lengthindicator,
                                            IFR_Bool              terminate,
                                            IFR_ConnectionItem & clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateUCS2Input, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_Converter::translateUTF8Input(IFRPacket_DataPart& datapart,
                                            char               *data,
                                            IFR_Length            datalength,
                                            IFR_Length*           lengthindicator,
                                            IFR_Bool              terminate,
                                            IFR_ConnectionItem &  clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateUTF8Input, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------.
IFR_Retcode
IFRConversion_Converter::translateInput(IFRPacket_DataPart& datapart,
                                         IFR_Int1&           data,
                                        IFR_Length*           lengthindicator,
                                        IFR_ConnectionItem& clink,
                                        IFRConversion_Putval *pv)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateInput, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}
//----------------------------------------------------------------------.
IFR_Retcode
IFRConversion_Converter::translateInput(IFRPacket_DataPart& datapart,
                                         IFR_Int2&           data,
                                         IFR_Length*           lengthindicator,
                                        IFR_ConnectionItem& clink,
                                        IFRConversion_Putval *pv)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateInput, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}
//----------------------------------------------------------------------.
IFR_Retcode
IFRConversion_Converter::translateInput(IFRPacket_DataPart& datapart,
                                         IFR_Int4&           data,
                                         IFR_Length*           lengthindicator,
                                        IFR_ConnectionItem& clink,
                                        IFRConversion_Putval *pv)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateInput, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}
//----------------------------------------------------------------------.
IFR_Retcode
IFRConversion_Converter::translateInput(IFRPacket_DataPart& datapart,
                                         IFR_Int8&           data,
                                         IFR_Length*           lengthindicator,
                                        IFR_ConnectionItem& clink,
                                        IFRConversion_Putval *pv)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateInput, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}


//----------------------------------------------------------------------.
IFR_Retcode
IFRConversion_Converter::translateInput(IFRPacket_DataPart& datapart,
                                         IFR_UInt1&           data,
                                         IFR_Length*           lengthindicator,
                                        IFR_ConnectionItem& clink,
                                        IFRConversion_Putval *pv)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateInput, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}
//----------------------------------------------------------------------.
IFR_Retcode
IFRConversion_Converter::translateInput(IFRPacket_DataPart& datapart,
                                         IFR_UInt2&           data,
                                         IFR_Length*           lengthindicator,
                                        IFR_ConnectionItem& clink,
                                        IFRConversion_Putval *pv)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateInput, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}
//----------------------------------------------------------------------.
IFR_Retcode
IFRConversion_Converter::translateInput(IFRPacket_DataPart& datapart,
                                         IFR_UInt4&           data,
                                         IFR_Length*           lengthindicator,
                                        IFR_ConnectionItem& clink,
                                        IFRConversion_Putval *pv)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateInput, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}
//----------------------------------------------------------------------.
IFR_Retcode
IFRConversion_Converter::translateInput(IFRPacket_DataPart& datapart,
                                         IFR_UInt8&           data,
                                         IFR_Length*           lengthindicator,
                                        IFR_ConnectionItem& clink,
                                        IFRConversion_Putval *pv)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateInput, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}


//----------------------------------------------------------------------.
IFR_Retcode
IFRConversion_Converter::translateInput(IFRPacket_DataPart& datapart,
                                         double&           data,
                                         IFR_Length*           lengthindicator,
                                        IFR_ConnectionItem& clink,
                                        IFRConversion_Putval *pv)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateInput, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------.
IFR_Retcode
IFRConversion_Converter::translateInput(IFRPacket_DataPart& datapart,
                                         float&           data,
                                         IFR_Length*           lengthindicator,
                                        IFR_ConnectionItem& clink,
                                        IFRConversion_Putval *pv)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateInput, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------.
IFR_Retcode
IFRConversion_Converter::translateInput(IFRPacket_DataPart& datapart,
                                         SQL_DATE_STRUCT&    data,
                                         IFR_Length*           lengthindicator,
                                        IFR_ConnectionItem& clink,
                                        IFRConversion_Putval *pv)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateInput, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------.
IFR_Retcode
IFRConversion_Converter::translateInput(IFRPacket_DataPart& datapart,
                                         SQL_TIME_STRUCT&    data,
                                         IFR_Length*           lengthindicator,
                                        IFR_ConnectionItem& clink,
                                        IFRConversion_Putval *pv)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateInput, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------.
IFR_Retcode
IFRConversion_Converter::translateInput(IFRPacket_DataPart& datapart,
                                        SQL_TIMESTAMP_STRUCT& data,
                                        IFR_Length*           lengthindicator,
                                        IFR_ConnectionItem& clink,
                                        IFRConversion_Putval *pv)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateInput, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------.
IFR_Retcode
IFRConversion_Converter::translateInput(IFRPacket_DataPart& datapart,
                                        SQL_NUMERIC_STRUCT&    data,
                                        IFR_Length*           lengthindicator,
                                        IFR_ConnectionItem& clink,
                                        IFRConversion_Putval *pv)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateInput, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------.
IFR_Retcode
IFRConversion_Converter::translateInput(IFRPacket_DataPart& datapart,
                                         GUID&    data,
                                         IFR_Length*           lengthindicator,
                                        IFR_ConnectionItem& clink,
                                        IFRConversion_Putval *pv)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateInput, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------.
IFR_Retcode
IFRConversion_Converter::translateDecimalInput(IFRPacket_DataPart& datapart,
                                               char *       data,
                                               IFR_Length   datalength,
                                               IFR_Length*  lengthindicator,
                                               IFR_ConnectionItem& clink,
                                               IFRConversion_Putval *pv)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateDecimalInput, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------.
IFR_Retcode
IFRConversion_Converter::translateOmsPacked_8_3_Input(IFRPacket_DataPart& datapart,
                                               char *       data,
                                               IFR_Length   datalength,
                                               IFR_Length*  lengthindicator,
                                                      IFR_ConnectionItem& clink,
                                                      IFRConversion_Putval *pv)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateOmsPacked_8_3_Input, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------.
IFR_Retcode
IFRConversion_Converter::translateOmsPacked_15_3_Input(IFRPacket_DataPart& datapart,
                                               char *       data,
                                               IFR_Length   datalength,
                                               IFR_Length*  lengthindicator,
                                                       IFR_ConnectionItem& clink,
                                                       IFRConversion_Putval *pv)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateOmsPacked_15_3_Input, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------.
IFR_Retcode
IFRConversion_Converter::translateOmsTimestampInput(IFRPacket_DataPart& datapart,
                                               char *       data,
                                               IFR_Length   datalength,
                                               IFR_Length*  lengthindicator,
                                                    IFR_ConnectionItem& clink,
                                                    IFRConversion_Putval *pv)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateOmsTimestampInput, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}



//----------------------------------------------------------------------
IFR_Bool
IFRConversion_Converter::isNull(IFRPacket_DataPart& part,
                                IFR_ConnectionItem& clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, isNull, &clink);
    DBUG_PRINTS(isNull, *( (part.getOutputData(m_shortinfo.pos.bufpos) - 1) ));
    return *((part.getOutputData(m_shortinfo.pos.bufpos) - 1) ) == (IFR_Int1)csp_undef_byte;
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_Converter::translateOutput(IFRPacket_DataPart& part,
                                         IFR_Parameter& parameter,
                                         IFR_ConnectionItem& clink,
                                         IFR_Int4 dataoffset,
                                         IFR_size_t rowsize,
                                         IFR_Bool ignoreUnsetParameter)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateOutput, &clink);

    IFR_HostType parameter_ht=parameter.getHostType();
    IFR_Bool parameter_term=parameter.shouldTerminated();

    // check that parameter is set
    if(parameter_ht == IFR_HOSTTYPE_PARAMETER_NOTSET) {
        if(ignoreUnsetParameter) {
            DBUG_RETURN(IFR_OK);
        } else {
            clink.error().setRuntimeError(IFR_ERR_PARAMETER_NOT_SET_I, (IFR_Int4)getIndex());
            DBUG_RETURN(IFR_NOT_OK);
        }
    }

    // get the length indicator, retrieve ...
    IFR_Length *parameter_lengthindicator=parameter.getLengthIndicator(dataoffset, rowsize);
    // ... whether we have nothing to do
    if(parameter_lengthindicator &&  *parameter_lengthindicator == IFR_IGNORE) {
        DBUG_RETURN(IFR_OK);
    }
    // ... or wether there is nothing that we can further handle
    if(isNull(part, clink)) {
        if(parameter_lengthindicator) {
            *parameter_lengthindicator = IFR_NULL_DATA;
        }
        
        // ODBC: We will add a null-terminator into the string, if the result is of string type, 
        // and has the desired length
        if(parameter.data() != 0 && parameter_term) {
            switch(parameter_ht) {
            case IFR_HOSTTYPE_ASCII:
            case IFR_HOSTTYPE_OMS_ASCII:
            case IFR_HOSTTYPE_UTF8:
                if(parameter.getBytesLength() >= 1) {
                    *(parameter.asChar(dataoffset, rowsize)) = '\0';
                }
                break;
            case IFR_HOSTTYPE_UCS2:
            case IFR_HOSTTYPE_UCS2_SWAPPED:
                if(parameter.getBytesLength() >= 2) {
                    char *b = parameter.asChar(dataoffset, rowsize);
                    *(b)   = '\0';
                    *(b+1) = '\0';
                }
                break;
            }
        }
        
        
        DBUG_RETURN(IFR_OK);
    }

    if(parameter.data() == 0) {
        clink.error().setRuntimeError(IFR_ERR_NULL_PARAMETERADDR_I,
                                      (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    }

    // otherwise do conversion
    switch(parameter_ht) {
    case IFR_HOSTTYPE_BINARY:
        DBUG_RETURN(translateBinaryOutput(part,
                                          parameter.asChar(dataoffset, rowsize),
                                          parameter.getBytesLength(),
                                          parameter_lengthindicator,
                                          clink));
    case IFR_HOSTTYPE_ASCII:
        DBUG_RETURN(translateAsciiOutput(part,
                                         parameter.asChar(dataoffset, rowsize),
                                         parameter.getBytesLength(),
                                         parameter_lengthindicator,
                                         parameter_term,
                                         false,
                                         clink));
    case IFR_HOSTTYPE_OMS_ASCII:
        DBUG_RETURN(translateAsciiOutput(part,
                                         parameter.asChar(dataoffset, rowsize),
                                         parameter.getBytesLength(),
                                         parameter_lengthindicator,
                                         parameter_term,
                                         clink.getConnection()->ascii7Bit(),
                                         clink));
    case IFR_HOSTTYPE_RAWHEX:
        DBUG_RETURN(translateRawHexOutput(part,
                                          parameter.asChar(dataoffset, rowsize),
                                          parameter.getBytesLength(),
                                          parameter_lengthindicator,
                                          parameter_term,
                                          clink));
    case IFR_HOSTTYPE_UCS2:
        DBUG_RETURN(translateUCS2Output(part,
                                       parameter.asChar(dataoffset, rowsize),
                                       false,
                                       parameter.getBytesLength(),
                                       parameter_lengthindicator,
                                       parameter_term,
                                       clink));
    case IFR_HOSTTYPE_UCS2_SWAPPED:
        DBUG_RETURN(translateUCS2Output(part,
                                       parameter.asChar(dataoffset, rowsize),
                                       true,
                                       parameter.getBytesLength(),
                                       parameter_lengthindicator,
                                       parameter_term,
                                       clink));
    case IFR_HOSTTYPE_UTF8:
        DBUG_RETURN(translateUTF8Output(part,
                                       parameter.asChar(dataoffset, rowsize),
                                       parameter.getBytesLength(),
                                       parameter_lengthindicator,
                                       parameter_term,
                                       clink));
    case IFR_HOSTTYPE_INT1:
        DBUG_RETURN(translateOutput(part, parameter.asInt1(dataoffset, rowsize),
                                   parameter_lengthindicator, clink));
    case IFR_HOSTTYPE_INT2:
        DBUG_RETURN(translateOutput(part, parameter.asInt2(dataoffset, rowsize),
                                   parameter_lengthindicator, clink));
    case IFR_HOSTTYPE_INT4:
        DBUG_RETURN(translateOutput(part, parameter.asInt4(dataoffset, rowsize),
                                   parameter_lengthindicator, clink));
    case IFR_HOSTTYPE_INT8:
        DBUG_RETURN(translateOutput(part, parameter.asInt8(dataoffset, rowsize),
                                   parameter_lengthindicator, clink));
    case IFR_HOSTTYPE_UINT1:
        DBUG_RETURN(translateOutput(part, parameter.asUInt1(dataoffset, rowsize),
                                   parameter_lengthindicator, clink));
    case IFR_HOSTTYPE_UINT2:
        DBUG_RETURN(translateOutput(part, parameter.asUInt2(dataoffset, rowsize),
                                   parameter_lengthindicator, clink));
    case IFR_HOSTTYPE_UINT4:
        DBUG_RETURN(translateOutput(part, parameter.asUInt4(dataoffset, rowsize),
                                   parameter_lengthindicator, clink));
    case IFR_HOSTTYPE_UINT8:
        DBUG_RETURN(translateOutput(part, parameter.asUInt8(dataoffset, rowsize),
                                   parameter_lengthindicator, clink));
    case IFR_HOSTTYPE_DOUBLE:
        DBUG_RETURN(translateOutput(part, parameter.asDouble(dataoffset, rowsize),
                                   parameter_lengthindicator, clink));
    case IFR_HOSTTYPE_FLOAT:
        DBUG_RETURN(translateOutput(part, parameter.asFloat(dataoffset, rowsize),
                                   parameter_lengthindicator, clink));
    case IFR_HOSTTYPE_ODBCDATE:
        DBUG_RETURN(translateOutput(part, parameter.asSQLDate(dataoffset, rowsize),
                                   parameter_lengthindicator, clink));
    case IFR_HOSTTYPE_ODBCTIME:
        DBUG_RETURN(translateOutput(part, parameter.asSQLTime(dataoffset, rowsize),
                                   parameter_lengthindicator, clink));
    case IFR_HOSTTYPE_ODBCTIMESTAMP:
        DBUG_RETURN(translateOutput(part, parameter.asSQLTimestamp(dataoffset, rowsize),
                                   parameter_lengthindicator, clink));
    case IFR_HOSTTYPE_ODBCNUMERIC:
        DBUG_RETURN(translateOutput(part, parameter.asSQLNumeric(dataoffset, rowsize),
                                   parameter_lengthindicator, clink));
    case IFR_HOSTTYPE_GUID:
        DBUG_RETURN(translateOutput(part, parameter.asGUID(dataoffset, rowsize),
                                   parameter_lengthindicator, clink));
    case IFR_HOSTTYPE_BLOB:
        DBUG_RETURN(translateBinaryLOBOutput(part,
                                             parameter.asLOBData(dataoffset, rowsize),
                                             parameter_lengthindicator,
                                             dataoffset,
                                             clink));
    case IFR_HOSTTYPE_ASCII_LOB:
        DBUG_RETURN(translateAsciiLOBOutput(part,
                                            parameter.asLOBData(dataoffset, rowsize),
                                            parameter_lengthindicator,
                                            dataoffset,
                                            clink));
    case IFR_HOSTTYPE_UCS2_LOB:
        DBUG_RETURN(translateUCS2LOBOutput(part,
                                           parameter.asLOBData(dataoffset, rowsize),
                                           false,
                                           parameter_lengthindicator,
                                           dataoffset,
                                           clink));
    case IFR_HOSTTYPE_UCS2_SWAPPED_LOB:
        DBUG_RETURN(translateUCS2LOBOutput(part,
                                           parameter.asLOBData(dataoffset, rowsize),
                                           true,
                                           parameter_lengthindicator,
                                           dataoffset,
                                           clink));
    case IFR_HOSTTYPE_UTF8_LOB:
        DBUG_RETURN(translateUTF8LOBOutput(part,
                                           parameter.asLOBData(dataoffset, rowsize),
                                           parameter_lengthindicator,
                                           dataoffset,
                                           clink));

    case IFR_HOSTTYPE_DECIMAL:{
        parameter.setDecimalDigitsAndFraction(parameter_lengthindicator);
        DBUG_RETURN(translateDecimalOutput(part,
                                           parameter.asChar(dataoffset, rowsize),
                                           parameter.getBytesLength(),
                                           parameter_lengthindicator,
                                           clink));
    }                                       
    case IFR_HOSTTYPE_OMS_PACKED_8_3:
        DBUG_RETURN(translateOmsPacked_8_3_Output(part,
                                                  parameter.asChar(dataoffset, rowsize),
                                                  parameter.getBytesLength(),
                                                  parameter_lengthindicator,
                                                  clink));
    case IFR_HOSTTYPE_OMS_PACKED_15_3:
        DBUG_RETURN(translateOmsPacked_15_3_Output(part,
                                                   parameter.asChar(dataoffset, rowsize),
                                                   parameter.getBytesLength(),
                                                   parameter_lengthindicator,
                                                   clink));

    case IFR_HOSTTYPE_OMS_TIMESTAMP:
        DBUG_RETURN(translateOmsTimestampOutput(part,
                                                parameter.asChar(dataoffset, rowsize),
                                                parameter.getBytesLength(),
                                                parameter_lengthindicator,
                                                clink));
    default:
        clink.error().setRuntimeError(IFR_ERR_INVALID_HOSTTYPE_SI,
                                      IFR_HostTypeToString(parameter_ht),
                                      (IFR_Int4) getIndex());
        DBUG_RETURN(IFR_NOT_OK);
    }
}


//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_Converter::translateBinaryOutput(IFRPacket_DataPart& datapart,
                                               char               *data,
                                               IFR_Length            datalength,
                                               IFR_Length*           lengthindicator,
                                               IFR_ConnectionItem & clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateBinaryOutput, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_Converter::translateAsciiOutput(IFRPacket_DataPart& datapart,
                                              char               *data,
                                              IFR_Length            datalength,
                                              IFR_Length*           lengthindicator,
                                              IFR_Bool            terminate,
                                              IFR_Bool            ascii7bit,
                                              IFR_ConnectionItem & clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateAsciiOutput, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------
static inline char hexchar(IFR_UInt4 num)
{
    if(num >= 16) {
        return '?';
    } else {
        const char * p = "0123456789ABCDEF";
        return p[num];
    }
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_Converter::translateRawHexOutput(IFRPacket_DataPart& datapart,
                                               char               *data,
                                               IFR_Length          datalength,
                                               IFR_Length*         lengthindicator,
                                               IFR_Bool            terminate,
                                               IFR_ConnectionItem &clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateRawHexOutput, &clink);
    char *read_data        = datapart.getOutputData(m_shortinfo.pos.bufpos);
    IFR_Length byteslength = m_shortinfo.iolength - 1;
    if(datalength < (byteslength *2) + 3) {
        clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    }
    IFR_Length writedata = 0;
    char *p = data;
    *p = 'x'; ++writedata; ++p;
    *p = '\''; ++writedata; ++p;
    for(IFR_Length i=0; i<byteslength; ++i) {
        IFR_UInt4 upper = (read_data[i] & 0xF0) >> 4;
        IFR_UInt4 lower = (read_data[i] & 0x0F);
        *p = hexchar(upper); ++writedata; ++p;
        *p = hexchar(lower); ++writedata; ++p;
    }
    *p = '\''; ++writedata; ++p;
    if(terminate) {
        *p = '\0'; ++p;
    }
    if(lengthindicator) {
        *lengthindicator = writedata;
    }
    DBUG_RETURN(IFR_OK);
}
                                               

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_Converter::translateUCS2Output(IFRPacket_DataPart& datapart,
                                            char               *data,
                                            IFR_Bool            swapped,
                                            IFR_Length            datalength,
                                            IFR_Length*           lengthindicator,
                                            IFR_Bool            terminate,
                                            IFR_ConnectionItem & clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateUCS2Output, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_Converter::translateUTF8Output(IFRPacket_DataPart& datapart,
                                            char               *data,
                                            IFR_Length            datalength,
                                            IFR_Length*           lengthindicator,
                                            IFR_Bool            terminate,
                                            IFR_ConnectionItem & clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateUTF8Output, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------.
IFR_Retcode
IFRConversion_Converter::translateOutput(IFRPacket_DataPart& datapart,
                                         IFR_Int1&           data,
                                         IFR_Length*           lengthindicator,
                                         IFR_ConnectionItem& clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateOutput, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}
//----------------------------------------------------------------------.
IFR_Retcode
IFRConversion_Converter::translateOutput(IFRPacket_DataPart& datapart,
                                         IFR_Int2&           data,
                                         IFR_Length*           lengthindicator,
                                         IFR_ConnectionItem& clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateOutput, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}
//----------------------------------------------------------------------.
IFR_Retcode
IFRConversion_Converter::translateOutput(IFRPacket_DataPart& datapart,
                                         IFR_Int4&           data,
                                         IFR_Length*           lengthindicator,
                                         IFR_ConnectionItem& clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateOutput, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}
//----------------------------------------------------------------------.
IFR_Retcode
IFRConversion_Converter::translateOutput(IFRPacket_DataPart& datapart,
                                         IFR_Int8&           data,
                                         IFR_Length*           lengthindicator,
                                         IFR_ConnectionItem& clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateOutput, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}


//----------------------------------------------------------------------.
IFR_Retcode
IFRConversion_Converter::translateOutput(IFRPacket_DataPart& datapart,
                                         IFR_UInt1&           data,
                                         IFR_Length*           lengthindicator,
                                         IFR_ConnectionItem& clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateOutput, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}
//----------------------------------------------------------------------.
IFR_Retcode
IFRConversion_Converter::translateOutput(IFRPacket_DataPart& datapart,
                                         IFR_UInt2&           data,
                                         IFR_Length*           lengthindicator,
                                         IFR_ConnectionItem& clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateOutput, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}
//----------------------------------------------------------------------.
IFR_Retcode
IFRConversion_Converter::translateOutput(IFRPacket_DataPart& datapart,
                                         IFR_UInt4&           data,
                                         IFR_Length*           lengthindicator,
                                         IFR_ConnectionItem& clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateOutput, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}
//----------------------------------------------------------------------.
IFR_Retcode
IFRConversion_Converter::translateOutput(IFRPacket_DataPart& datapart,
                                         IFR_UInt8&           data,
                                         IFR_Length*           lengthindicator,
                                         IFR_ConnectionItem& clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateOutput, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}


//----------------------------------------------------------------------.
IFR_Retcode
IFRConversion_Converter::translateOutput(IFRPacket_DataPart& datapart,
                                         double&           data,
                                         IFR_Length*           lengthindicator,
                                         IFR_ConnectionItem& clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateOutput, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------.
IFR_Retcode
IFRConversion_Converter::translateOutput(IFRPacket_DataPart& datapart,
                                         float&           data,
                                         IFR_Length*           lengthindicator,
                                         IFR_ConnectionItem& clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateOutput, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------.
IFR_Retcode
IFRConversion_Converter::translateOutput(IFRPacket_DataPart& datapart,
                                         SQL_DATE_STRUCT&    data,
                                         IFR_Length*           lengthindicator,
                                         IFR_ConnectionItem& clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateOutput, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------.
IFR_Retcode
IFRConversion_Converter::translateOutput(IFRPacket_DataPart& datapart,
                                         SQL_TIME_STRUCT&    data,
                                         IFR_Length*           lengthindicator,
                                         IFR_ConnectionItem& clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateOutput, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------.
IFR_Retcode
IFRConversion_Converter::translateOutput(IFRPacket_DataPart& datapart,
                                         SQL_TIMESTAMP_STRUCT& data,
                                         IFR_Length*           lengthindicator,
                                         IFR_ConnectionItem& clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateOutput, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------.
IFR_Retcode
IFRConversion_Converter::translateOutput(IFRPacket_DataPart& datapart,
                                         SQL_NUMERIC_STRUCT&    data,
                                         IFR_Length*           lengthindicator,
                                         IFR_ConnectionItem& clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateOutput, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------.
IFR_Retcode
IFRConversion_Converter::translateOutput(IFRPacket_DataPart& datapart,
                                         GUID&    data,
                                         IFR_Length*           lengthindicator,
                                         IFR_ConnectionItem& clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateOutput, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_Converter::translateDecimalOutput(IFRPacket_DataPart& datapart,
                                                char *           data,
                                                IFR_Length       datalength,
                                                IFR_Length*           lengthindicator,
                                                IFR_ConnectionItem& clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateDecimalOutput, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_Converter::translateOmsPacked_8_3_Output(IFRPacket_DataPart& datapart,
                                                char *           data,
                                                IFR_Length       datalength,
                                                IFR_Length*           lengthindicator,
                                                IFR_ConnectionItem& clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateOmsPacked_8_3_Output, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_Converter::translateOmsPacked_15_3_Output(IFRPacket_DataPart& datapart,
                                                char *           data,
                                                IFR_Length       datalength,
                                                IFR_Length*           lengthindicator,
                                                IFR_ConnectionItem& clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateOmsPacked_15_3_Output, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_Converter::translateOmsTimestampOutput(IFRPacket_DataPart& datapart,
                                                     char *           data,
                                                     IFR_Length       datalength,
                                                     IFR_Length*      lengthindicator,
                                                     IFR_ConnectionItem& clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateOmsTimestampOutput, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_Converter::putData(IFRPacket_DataPart& datapart,
                                 void *data,
                                 IFR_Length *lengthindicator,
                                 IFR_Parameter& parameter,
                                 IFR_Length& offset,
                                 IFR_Bool &nullordefault,
                                 IFR_Bool &integral,
                                 IFR_ConnectionItem& clink,
                                 IFRConversion_Putval* putval)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, putData, &clink);
    DBUG_PRINT(lengthindicator);
    DBUG_PRINT(offset);
    DBUG_PRINT(nullordefault);
    DBUG_PRINT(integral);

    IFR_HostType parameter_ht = parameter.getHostType();
    IFR_Bool     parameter_term = parameter.shouldTerminated();
    
    // check that parameter is set
    if(parameter_ht == IFR_HOSTTYPE_PARAMETER_NOTSET) {
        clink.error().setRuntimeError(IFR_ERR_PARAMETER_NOT_SET_I, (IFR_Int4)getIndex());
        DBUG_RETURN(IFR_NOT_OK);
    }

    if(lengthindicator) {
        if(*lengthindicator == IFR_NULL_DATA) {
            // Setting LOB values to NULL or DEFAULT is not allowed after the EXECUTE
            if(parameter.isLOB()) {
                clink.error().setRuntimeError(IFR_ERR_LOB_NULLDEFAULT_I, (IFR_Int4)getIndex());
                DBUG_RETURN(IFR_NOT_OK);
            }
            // in case that there is an offset, there was a putdata before.
            // This operation so must not complete, but throw an error.
            if(offset != 0) {
                clink.error().setRuntimeError(IFR_ERR_NONULLDATAAPPEND_I, getIndex());
                DBUG_RETURN(IFR_NOT_OK);
            }
            nullordefault=true;
            DBUG_RETURN(translateNullInput(datapart, clink));
        } else if(*lengthindicator == IFR_DEFAULT_PARAM) {
            // Setting LOB values to NULL or DEFAULT is not allowed after the EXECUTE
            if(parameter.isLOB()) {
                clink.error().setRuntimeError(IFR_ERR_LOB_NULLDEFAULT_I, (IFR_Int4)getIndex());
                DBUG_RETURN(IFR_NOT_OK);
            }
            // in case that there is an offset, there was a putdata before.
            // This operation so must not complete, but throw an error.
            if(offset != 0) {
                clink.error().setRuntimeError(IFR_ERR_NONULLDATAAPPEND_I, (IFR_Int4) getIndex());
                DBUG_RETURN(IFR_NOT_OK);
            }
            nullordefault=true;
            DBUG_RETURN(translateDefaultInput(parameter, datapart, clink, 0, 0));
        }
    } else {
        clink.error().setRuntimeError(IFR_ERR_NULL_PARAMETERADDR_I, (IFR_Int4) getIndex());
        DBUG_RETURN(IFR_NOT_OK);
    }


    if (data == 0 && (lengthindicator && *lengthindicator!=0)) {
        clink.error().setRuntimeError(IFR_ERR_NULL_PARAMETERADDR_I, (IFR_Int4) getIndex());
        DBUG_RETURN(IFR_NOT_OK);
    }


    // if the data type is an integral type, the data must be delivered in
    // one piece.
    // this is also the case if the column type is CHAR/VARCHAR but
    // the host type is integral
    if(!parameter.isLOB() && (isIntegral() || (parameter.isIntegral()))) {
        integral=true;
        switch(parameter_ht) {
        case IFR_HOSTTYPE_BINARY:
            DBUG_RETURN(translateBinaryInput(datapart,
                                             (char *)data,
                                             0,
                                             lengthindicator,
                                             clink));
        case IFR_HOSTTYPE_ASCII:
            DBUG_RETURN(translateAsciiInput(datapart,
                                            (char *)data,
                                            0,
                                            lengthindicator,
                                            parameter_term,
                                            false,
                                            clink));
        case IFR_HOSTTYPE_OMS_ASCII:
            DBUG_RETURN(translateAsciiInput(datapart,
                                            (char *)data,
                                            0,
                                            lengthindicator,
                                            parameter_term,
                                            clink.getConnection()->ascii7Bit(),
                                            clink));
        case IFR_HOSTTYPE_UCS2:
            DBUG_RETURN(translateUCS2Input(datapart,
                                           (char *)data,
                                           false,
                                           0,
                                           lengthindicator,
                                           parameter_term,
                                           clink));
        case IFR_HOSTTYPE_UCS2_SWAPPED:
            DBUG_RETURN(translateUCS2Input(datapart,
                                           (char *)data,
                                           true,
                                           0,
                                           lengthindicator,
                                           parameter_term,
                                           clink));
        case IFR_HOSTTYPE_UTF8:
            DBUG_RETURN(translateUTF8Input(datapart,
                                           (char *)data,
                                           0,
                                           lengthindicator,
                                           parameter_term,
                                           clink));
#define TRANSLATE_AS_TYPE(type)                                         \
            {                                                           \
                type tmp;                                               \
                memcpy(&tmp, data, sizeof(type));                       \
                DBUG_RETURN(translateInput(datapart, tmp, lengthindicator, clink, putval)); \
            }
        case IFR_HOSTTYPE_UINT1:
            TRANSLATE_AS_TYPE(IFR_UInt1);
        case IFR_HOSTTYPE_UINT2:
            TRANSLATE_AS_TYPE(IFR_UInt2);
        case IFR_HOSTTYPE_UINT4:
            TRANSLATE_AS_TYPE(IFR_UInt4);
        case IFR_HOSTTYPE_UINT8:
            TRANSLATE_AS_TYPE(IFR_UInt8);
        case IFR_HOSTTYPE_INT1:
            TRANSLATE_AS_TYPE(IFR_Int1);
        case IFR_HOSTTYPE_INT2:
            TRANSLATE_AS_TYPE(IFR_Int2);
        case IFR_HOSTTYPE_INT4:
            TRANSLATE_AS_TYPE(IFR_Int4);
        case IFR_HOSTTYPE_INT8:
            TRANSLATE_AS_TYPE(IFR_Int8);
        case IFR_HOSTTYPE_DOUBLE:
            TRANSLATE_AS_TYPE(double);
        case IFR_HOSTTYPE_FLOAT:
            TRANSLATE_AS_TYPE(float);
        case IFR_HOSTTYPE_ODBCDATE:
            TRANSLATE_AS_TYPE(SQL_DATE_STRUCT);
        case IFR_HOSTTYPE_ODBCTIME:
            TRANSLATE_AS_TYPE(SQL_TIME_STRUCT);
        case IFR_HOSTTYPE_ODBCTIMESTAMP:
            TRANSLATE_AS_TYPE(SQL_TIMESTAMP_STRUCT);
        case IFR_HOSTTYPE_ODBCNUMERIC:
            TRANSLATE_AS_TYPE(SQL_NUMERIC_STRUCT);
        case IFR_HOSTTYPE_GUID:
            TRANSLATE_AS_TYPE(GUID);
        default:
            clink.error().setRuntimeError(IFR_ERR_INVALID_HOSTTYPE_SI,
                                        IFR_HostTypeToString(parameter_ht),
                                        (IFR_Int4)getIndex());
            DBUG_RETURN(IFR_NOT_OK);
        }
#undef TRANSLATE_AS_TYPE
    } else {
        switch(parameter_ht) {
        case IFR_HOSTTYPE_BINARY:
        case IFR_HOSTTYPE_BLOB:
            if(lengthindicator && *lengthindicator==0) {
                DBUG_RETURN(IFR_NO_DATA_FOUND);
            }
            DBUG_RETURN(appendBinaryInput(datapart,
                                          (char *)data,
                                          0,
                                          lengthindicator,
                                          clink,
                                          offset,
                                          putval));
        case IFR_HOSTTYPE_ASCII:
        case IFR_HOSTTYPE_ASCII_LOB:
            if(lengthindicator && *lengthindicator==0) {
                DBUG_RETURN(IFR_NO_DATA_FOUND);
            }
            DBUG_RETURN(appendAsciiInput(datapart,
                                         (char *)data,
                                         0,
                                         lengthindicator,
                                         parameter_term,
                                         false,
                                         clink,
                                         offset,
                                         putval));
        case IFR_HOSTTYPE_OMS_ASCII:
            if(lengthindicator && *lengthindicator==0) {
                DBUG_RETURN(IFR_NO_DATA_FOUND);
            }
            DBUG_RETURN(appendAsciiInput(datapart,
                                         (char *)data,
                                         0,
                                         lengthindicator,
                                         parameter_term,
                                         clink.getConnection()->ascii7Bit(),
                                         clink,
                                         offset,
                                         putval));
        case IFR_HOSTTYPE_UCS2:
        case IFR_HOSTTYPE_UCS2_LOB:
            if(lengthindicator && *lengthindicator==0) {
                DBUG_RETURN(IFR_NO_DATA_FOUND);
            }
            DBUG_RETURN(appendUCS2Input(datapart,
                                        (char *)data,
                                        false,
                                        0,
                                        lengthindicator,
                                        parameter_term,
                                        clink,
                                        offset,
                                        putval));
        case IFR_HOSTTYPE_UCS2_SWAPPED:
        case IFR_HOSTTYPE_UCS2_SWAPPED_LOB:
            if(lengthindicator && *lengthindicator==0) {
                DBUG_RETURN(IFR_NO_DATA_FOUND);
            }
            DBUG_RETURN(appendUCS2Input(datapart,
                                        (char *)data,
                                        true,
                                        0,
                                        lengthindicator,
                                        parameter_term,
                                        clink,
                                        offset,
                                        putval));
        case IFR_HOSTTYPE_UTF8:
        case IFR_HOSTTYPE_UTF8_LOB:
            if(lengthindicator && *lengthindicator==0) {
                DBUG_RETURN(IFR_NO_DATA_FOUND);
            }
            DBUG_RETURN(appendUTF8Input(datapart,
                                        (char *)data,
                                        0,
                                        lengthindicator,
                                        parameter_term,
                                        clink,
                                        offset,
                                        putval));
            
        default:
            clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
            DBUG_RETURN(IFR_NOT_OK);
        }
    }
}


//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_Converter::getData(IFRPacket_DataPart& datapart,
                                 void *data,
                                 IFR_Length *lengthindicator,
                                 IFR_Parameter& parameter,
                                 IFR_Length& dataoffset,
                                 IFR_Length& offset,
                                 IFR_ConnectionItem& clink,
                                 IFR_Length& datalength,
                                 IFRConversion_Getval* getval)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, getData, &clink);
    DBUG_PRINT(lengthindicator);
    DBUG_PRINT(offset);
    DBUG_PRINT(datalength);

    
    IFR_HostType parameter_ht = parameter.getHostType();
    IFR_Bool parameter_term=parameter.shouldTerminated();

    // check that parameter is set
    if(parameter_ht == IFR_HOSTTYPE_PARAMETER_NOTSET) {
        clink.error().setRuntimeError(IFR_ERR_PARAMETER_NOT_SET_I, (IFR_Int4)getIndex());
        DBUG_RETURN(IFR_NOT_OK);
    }

    if(datalength == -1) {
        datalength = parameter.getBytesLength();
    }
    // ... or wether there is nothing that we can further handle
    if (datapart.getPartKind () != IFRPacket_PartKind::Longdata_C) {
        if(isNull(datapart, clink)) {
            if(lengthindicator) {
                *lengthindicator = IFR_NULL_DATA;
            }
            
            // ODBC: We will add a null-terminator into the string, if the result is of string type, 
            // and has the desired length
            if(data != 0 && parameter_term) {
                switch(parameter_ht) {
                case IFR_HOSTTYPE_ASCII:
                case IFR_HOSTTYPE_OMS_ASCII:
                case IFR_HOSTTYPE_UTF8:
                    if(datalength >= 1) {
                        *((char *)data) = '\0';
                    }
                    break;
                case IFR_HOSTTYPE_UCS2:
                case IFR_HOSTTYPE_UCS2_SWAPPED:
                    if(datalength >= 2) {
                        *((char *)data)   = '\0';
                        *((char *)data+1) = '\0';
                    }
                    break;
                }
            }
            
            DBUG_RETURN(IFR_OK);
        }
    }


    if ((data == 0) && (datalength != 0)) {
        clink.error().setRuntimeError(IFR_ERR_NULL_PARAMETERADDR_I, (IFR_Int4) getIndex());
        DBUG_RETURN(IFR_NOT_OK);
    }

    // if the data type is an integral type, the data must be delivered in
    // one piece.
    // this is also the case if the column type is CHAR/VARCHAR but
    // the host type is integral
    // In this case, the OFFSET must be 0.
    // LONG data is always read using getData, so this does not apply.
    
    if(isIntegral() 
       || (!isLong() && parameter.isIntegral())
       || parameter.isLOB()
        ) {        
        if(offset != 1 && ! (offset == 0 && parameter.isLOB())) {
            clink.error().setRuntimeError(IFR_ERR_NOPIECEWISE_DATA_READ_I, (IFR_Int4)getIndex());
            DBUG_RETURN(IFR_NOT_OK);
        }
        switch(parameter_ht) {
        case IFR_HOSTTYPE_BINARY:
            DBUG_RETURN(translateBinaryOutput(datapart,
					      (char *)data,
					      datalength,
					      lengthindicator,
					      clink));
        case IFR_HOSTTYPE_ASCII:
            DBUG_RETURN(translateAsciiOutput(datapart,
                                             (char *)data,
                                             datalength,
                                             lengthindicator,
                                             parameter_term,
                                             false,
                                             clink));
        case IFR_HOSTTYPE_OMS_ASCII:
            DBUG_RETURN(translateAsciiOutput(datapart,
                                             (char *)data,
                                             datalength,
                                             lengthindicator,
                                             parameter_term,
                                             clink.getConnection()->ascii7Bit(),
                                             clink));
        case IFR_HOSTTYPE_RAWHEX:
            DBUG_RETURN(translateRawHexOutput(datapart,
                                              (char *)data,
                                              datalength,
                                              lengthindicator,
                                              parameter_term,
                                              clink));
        case IFR_HOSTTYPE_UCS2:
            DBUG_RETURN(translateUCS2Output(datapart,
					    (char *)data,
					    false,
					    datalength,
					    lengthindicator,
					    parameter_term,
					    clink));
        case IFR_HOSTTYPE_UCS2_SWAPPED:
            DBUG_RETURN(translateUCS2Output(datapart,
					    (char *)data,
					    true,
					    datalength,
					    lengthindicator,
					    parameter_term,
					    clink));
        case IFR_HOSTTYPE_UTF8:
            DBUG_RETURN(translateUTF8Output(datapart,
					    (char *)data,
					    datalength,
					    lengthindicator,
					    parameter_term,
					    clink));
        case IFR_HOSTTYPE_DECIMAL:
            DBUG_RETURN(translateDecimalOutput(datapart,
                                               (char *)data,
                                               datalength,
                                               lengthindicator,
                                               clink));
        case IFR_HOSTTYPE_OMS_PACKED_8_3:
            DBUG_RETURN(translateOmsPacked_8_3_Output(datapart,
                                                      (char *)data,
                                                      datalength,
                                                      lengthindicator,
                                                      clink));
        case IFR_HOSTTYPE_OMS_PACKED_15_3:
            DBUG_RETURN(translateOmsPacked_15_3_Output(datapart,
                                                       (char *)data,
                                                       datalength,
                                                       lengthindicator,
                                                       clink));
        
        case IFR_HOSTTYPE_OMS_TIMESTAMP:
            DBUG_RETURN(translateOmsTimestampOutput(datapart,
                                                    (char *)data,
                                                    datalength,
                                                    lengthindicator,
                                                    clink));
            
            
#define TRANSLATE_AS_TYPE(type)                                                     \
            {                                                                       \
                DBUG_RETURN(translateOutput(datapart, *((type *) data), lengthindicator, clink)); \
            }
        case IFR_HOSTTYPE_UINT1:
            TRANSLATE_AS_TYPE(IFR_UInt1);
        case IFR_HOSTTYPE_UINT2:
            TRANSLATE_AS_TYPE(IFR_UInt2);
        case IFR_HOSTTYPE_UINT4:
            TRANSLATE_AS_TYPE(IFR_UInt4);
        case IFR_HOSTTYPE_UINT8:
            TRANSLATE_AS_TYPE(IFR_UInt8);
        case IFR_HOSTTYPE_INT1:
            TRANSLATE_AS_TYPE(IFR_Int1);
        case IFR_HOSTTYPE_INT2:
            TRANSLATE_AS_TYPE(IFR_Int2);
        case IFR_HOSTTYPE_INT4:
            TRANSLATE_AS_TYPE(IFR_Int4);
        case IFR_HOSTTYPE_INT8:
            TRANSLATE_AS_TYPE(IFR_Int8);
        case IFR_HOSTTYPE_DOUBLE:
            TRANSLATE_AS_TYPE(double);
        case IFR_HOSTTYPE_FLOAT:
            TRANSLATE_AS_TYPE(float);
        case IFR_HOSTTYPE_ODBCDATE:
            TRANSLATE_AS_TYPE(SQL_DATE_STRUCT);
        case IFR_HOSTTYPE_ODBCTIME:
            TRANSLATE_AS_TYPE(SQL_TIME_STRUCT);
        case IFR_HOSTTYPE_ODBCTIMESTAMP:
            TRANSLATE_AS_TYPE(SQL_TIMESTAMP_STRUCT);
        case IFR_HOSTTYPE_ODBCNUMERIC:
            TRANSLATE_AS_TYPE(SQL_NUMERIC_STRUCT);
        case IFR_HOSTTYPE_GUID:
            TRANSLATE_AS_TYPE(GUID);
        case IFR_HOSTTYPE_BLOB:
            DBUG_RETURN(translateBinaryLOBOutput(datapart,
                                                 *((IFR_LOBData*)data),
                                                 lengthindicator,
                                                 dataoffset,
                                                 clink));
        case IFR_HOSTTYPE_ASCII_LOB:
            DBUG_RETURN(translateAsciiLOBOutput(datapart,
                                                 *((IFR_LOBData*)data),
                                                lengthindicator,
                                                dataoffset,
                                                clink));
        case IFR_HOSTTYPE_UCS2_LOB:
            DBUG_RETURN(translateUCS2LOBOutput(datapart,
                                                 *((IFR_LOBData*)data),
                                               false,
                                               lengthindicator,
                                               dataoffset,
                                               clink));
        case IFR_HOSTTYPE_UCS2_SWAPPED_LOB:
            DBUG_RETURN(translateUCS2LOBOutput(datapart,
                                               *((IFR_LOBData*)data),
                                               true,
                                               lengthindicator,
                                               dataoffset,
                                               clink));
        case IFR_HOSTTYPE_UTF8_LOB:
            DBUG_RETURN(translateUTF8LOBOutput(datapart,
                                               *((IFR_LOBData*)data),
                                               lengthindicator,
                                               dataoffset,
                                               clink));
            
        default:
            clink.error().setRuntimeError(IFR_ERR_INVALID_HOSTTYPE_SI,
                                          IFR_HostTypeToString(parameter_ht),
                                          (IFR_Int4)getIndex());
            DBUG_RETURN(IFR_NOT_OK);
        }
#undef TRANSLATE_AS_TYPE
    } else {
        switch(parameter_ht) {
        case IFR_HOSTTYPE_BINARY:
            DBUG_RETURN(appendBinaryOutput(datapart,
                                           (char *)data,
                                           datalength,
                                           lengthindicator,
                                           clink,
                                           dataoffset,
                                           offset,
                                           getval));
        case IFR_HOSTTYPE_ASCII:
            DBUG_RETURN(appendAsciiOutput(datapart,
                                          (char *)data,
                                          datalength,
                                          lengthindicator,
                                          parameter_term,
                                          false,
                                          clink,
                                          dataoffset,
                                          offset,
                                          getval));
        case IFR_HOSTTYPE_OMS_ASCII:
            DBUG_RETURN(appendAsciiOutput(datapart,
                                          (char *)data,
                                          datalength,
                                          lengthindicator,
                                          parameter_term,
                                          clink.getConnection()->ascii7Bit(),
                                          clink,
                                          dataoffset,
                                          offset,
                                          getval));
        case IFR_HOSTTYPE_UCS2:
            DBUG_RETURN(appendUCS2Output(datapart,
                                         (char *)data,
                                         false,
                                         parameter.getBytesLength(),
                                         lengthindicator,
                                         parameter_term,
                                         clink,
                                         dataoffset,
                                         offset,
                                         getval));
        case IFR_HOSTTYPE_UCS2_SWAPPED:
            DBUG_RETURN(appendUCS2Output(datapart,
                                         (char *)data,
                                         true,
                                         parameter.getBytesLength(),
                                         lengthindicator,
                                         parameter_term,
                                         clink,
                                         dataoffset,
                                         offset,
                                         getval));
        case IFR_HOSTTYPE_UTF8:
            DBUG_RETURN(appendUTF8Output(datapart,
                                         (char *)data,
                                         parameter.getBytesLength(),
                                         lengthindicator,
                                         parameter_term,
                                         clink,
                                         dataoffset,
                                         offset,
                                         getval));
        default:
            // The case for other values *should* be handled explicitely,
            // and not delegated, but this is the most convenient way to
            // handle the non-obvious conversions from LONG data to
            // numbers / date and time values.
            DBUG_RETURN(getIntegralData(datapart, data, lengthindicator, parameter, dataoffset, offset, clink, datalength, getval));
        }
    }
}

//----------------------------------------------------------------------
IFR_Retcode 
IFRConversion_Converter::getIntegralData(IFRPacket_DataPart& datapart,
                                         void *data,
                                         IFR_Length* lengthindicator,
                                         IFR_Parameter& parameter,
                                         IFR_Length& dataoffset,
                                         IFR_Length& offset,
                                         IFR_ConnectionItem& clink,
                                         IFR_Length&         datalength,
                                         IFRConversion_Getval *getval)
{
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    return IFR_NOT_OK;
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_Converter::appendBinaryInput(IFRPacket_DataPart& datapart,
                                           char               *data,
                                           IFR_Length            datalength,
                                           IFR_Length*           lengthindicator,
                                           IFR_ConnectionItem& clink,
                                           IFR_Length         &offset,
                                           IFRConversion_Putval *putval)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, appendBinaryInput, &clink);
    clink.error().setRuntimeError(IFR_ERR_NOPIECEWISE_DATA_WRITE_I, (IFR_Int4)getIndex());
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_Converter::appendAsciiInput(IFRPacket_DataPart& datapart,
                                          char               *data,
                                          IFR_Length            datalength,
                                          IFR_Length*           lengthindicator,
                                          IFR_Bool            terminate,
                                          IFR_Bool            ascii7bit,
                                          IFR_ConnectionItem& clink,
                                          IFR_Length         &offset,
                                          IFRConversion_Putval *putval)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, appendAsciiInput, &clink);
    clink.error().setRuntimeError(IFR_ERR_NOPIECEWISE_DATA_WRITE_I, (IFR_Int4)getIndex());
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_Converter::appendUCS2Input(IFRPacket_DataPart& datapart,
                                         char               *data,
                                         IFR_Bool            swapped,
                                         IFR_Length            datalength,
                                         IFR_Length*           lengthindicator,
                                         IFR_Bool            terminate,
                                         IFR_ConnectionItem& clink,
                                         IFR_Length         &offset,
                                         IFRConversion_Putval *putval)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, appendUCS2Input, &clink);
    clink.error().setRuntimeError(IFR_ERR_NOPIECEWISE_DATA_READ_I, (IFR_Int4)getIndex());
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_Converter::appendUTF8Input(IFRPacket_DataPart& datapart,
                                         char               *data,
                                         IFR_Length            datalength,
                                         IFR_Length*           lengthindicator,
                                         IFR_Bool              terminate,
                                         IFR_ConnectionItem& clink,
                                         IFR_Length         &offset,
                                         IFRConversion_Putval *putval)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, appendUTF8Input, &clink);
    clink.error().setRuntimeError(IFR_ERR_NOPIECEWISE_DATA_WRITE_I, (IFR_Int4)getIndex());
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_Converter::appendAsciiOutput(IFRPacket_DataPart& datapart,
                                           char               *data,
                                           IFR_Length          datalength,
                                           IFR_Length*           lengthindicator,
                                           IFR_Bool            terminate,
                                           IFR_Bool            ascii7bit,
                                           IFR_ConnectionItem& clink,
                                           IFR_Length         &dataoffset,
                                           IFR_Length         &offset,
                                           IFRConversion_Getval *getval)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, appendAsciiOutput, &clink);
    clink.error().setRuntimeError(IFR_ERR_NOPIECEWISE_DATA_READ_I, (IFR_Int4)getIndex());
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_Converter::appendBinaryOutput(IFRPacket_DataPart& datapart,
                                            char               *data,
                                            IFR_Length            datalength,
                                            IFR_Length*           lengthindicator,
                                            IFR_ConnectionItem& clink,
                                            IFR_Length         &dataoffset,
                                            IFR_Length         &offset,
                                            IFRConversion_Getval *getval)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, appendBinaryOutput, &clink);
    clink.error().setRuntimeError(IFR_ERR_NOPIECEWISE_DATA_READ_I, (IFR_Int4)getIndex());
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_Converter::appendUCS2Output(IFRPacket_DataPart& datapart,
                                          char               *data,
                                          IFR_Bool            swapped,
                                          IFR_Length            datalength,
                                          IFR_Length*           lengthindicator,
                                          IFR_Bool            terminate,
                                          IFR_ConnectionItem& clink,
                                          IFR_Length         &dataoffset,
                                          IFR_Length         &offset,
                                          IFRConversion_Getval *getval)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, appendUCS2Output, &clink);
    clink.error().setRuntimeError(IFR_ERR_NOPIECEWISE_DATA_READ_I, (IFR_Int4)getIndex());
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_Converter::appendUTF8Output(IFRPacket_DataPart& datapart,
                                          char               *data,
                                          IFR_Length            datalength,
                                          IFR_Length*           lengthindicator,
                                          IFR_Bool            terminate,
                                          IFR_ConnectionItem& clink,
                                          IFR_Length         &dataoffset,
                                          IFR_Length         &offset,
                                          IFRConversion_Getval *getval)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, appendUTF8Output, &clink);
    clink.error().setRuntimeError(IFR_ERR_NOPIECEWISE_DATA_READ_I, (IFR_Int4)getIndex());
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------
IFRConversion_Putval *
IFRConversion_Converter::createPutval(IFRPacket_DataPart& part,
                                      IFR_Parameter& parameter,
                                      IFR_ConnectionItem& clink,
                                      IFR_Int4 dataoffset,
                                      IFR_size_t rowsize)
{
	DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, createPutval, &clink);
	DBUG_RETURN((IFRConversion_Putval *)0);
}

//----------------------------------------------------------------------
IFRConversion_Getval *
IFRConversion_Converter::createGetval (IFRPacket_DataPart& part,
                                       IFR_Parameter* parameter,
                                       IFR_ConnectionItem& clink,
                                       IFR_Int4 row,
                                       IFR_Int4 dataoffset,
                                       IFR_size_t rowsize)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, createGetval, &clink);
    DBUG_RETURN((IFRConversion_Getval *)0);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_Converter::getName(char *buffer,
                                 IFR_StringEncoding encoding,
                                 IFR_Length bufferSize,
                                 IFR_Length *bufferLength) const
{
#if 0
    DBUG_METHOD_ENTER(IFRConversion_Converter, getName, 0);
    DBUG_PRINT(encoding);
    DBUG_PRINT(bufferLength);
    DBUG_RETURN(m_name.convertInto(buffer, encoding, bufferSize, bufferLength));
#endif
    return m_name.convertInto(buffer, encoding, bufferSize, bufferLength);
}

//----------------------------------------------------------------------.
IFR_Retcode
IFRConversion_Converter::translateBinaryLOBInput(IFRPacket_DataPart& datapart,
                                                 IFR_LOBData&          data,
                                                 IFR_Length*           lengthindicator,
                                                 IFR_Int4              dataoffset,
                                                 IFR_ConnectionItem& clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateInput, &clink);

    data.lobdata=0;
    data.connectionitem = &clink;

    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------.
IFR_Retcode
IFRConversion_Converter::translateAsciiLOBInput(IFRPacket_DataPart& datapart,
                                                 IFR_LOBData&         data,
                                                 IFR_Length*           lengthindicator,
                                                 IFR_Int4              dataoffset,
                                                 IFR_ConnectionItem& clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateInput, &clink);

    data.lobdata=0;
    data.connectionitem = &clink;

    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------.
IFR_Retcode
IFRConversion_Converter::translateUCS2LOBInput(IFRPacket_DataPart& datapart,
                                               IFR_LOBData&     data,
                                               IFR_Bool      swapped,
                                               IFR_Length*           lengthindicator,
                                               IFR_Int4              dataoffset,
                                               IFR_ConnectionItem& clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateInput, &clink);

    data.lobdata=0;
    data.connectionitem = &clink;

    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------.
IFR_Retcode
IFRConversion_Converter::translateUTF8LOBInput(IFRPacket_DataPart& datapart,
                                               IFR_LOBData&     data,
                                               IFR_Length*           lengthindicator,
                                               IFR_Int4              dataoffset,
                                               IFR_ConnectionItem& clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateUTF8LOBInput, &clink);

    data.lobdata=0;
    data.connectionitem = &clink;

    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}


//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_Converter::translateABAPStreamInput(IFRPacket_DataPart& datapart,
                                                  void       *streamDescriptor,
                                                  IFR_Length  datalength,
                                                  IFR_Length *lengthindicator,
                                                  IFR_Int4    dataoffset,
                                                  IFR_ConnectionItem& clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateABAPStreamInput, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_Converter::translateBinaryLOBOutput(IFRPacket_DataPart& datapart,
                                                  IFR_LOBData        &lob,
                                                  IFR_Length*         lengthindicator,
                                                  IFR_Int4            dataoffset,
                                                  IFR_ConnectionItem& clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateBinaryLOBOutput, &clink);

    lob.lobdata=0;
    lob.connectionitem = &clink;

    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_Converter::translateAsciiLOBOutput(IFRPacket_DataPart& datapart,
                                                 IFR_LOBData        &lob,
                                                 IFR_Length*         lengthindicator,
                                                 IFR_Int4            dataoffset,
                                                 IFR_ConnectionItem& clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateBinaryLOBOutput, &clink);

    lob.lobdata=0;
    lob.connectionitem = &clink;

    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_Converter::translateUCS2LOBOutput(IFRPacket_DataPart& datapart,
                                                IFR_LOBData        &lob,
                                                IFR_Bool swapped,
                                                IFR_Length*         lengthindicator,
                                                IFR_Int4            dataoffset,
                                                IFR_ConnectionItem& clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateBinaryLOBOutput, &clink);

    lob.lobdata=0;
    lob.connectionitem = &clink;

    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}

IFR_Retcode
IFRConversion_Converter::translateUTF8LOBOutput(IFRPacket_DataPart& datapart,
                                                IFR_LOBData        &lob,
                                                IFR_Length*         lengthindicator,
                                                IFR_Int4            dataoffset,
                                                IFR_ConnectionItem& clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateBinaryLOBOutput, &clink);

    lob.lobdata=0;
    lob.connectionitem = &clink;

    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_Converter::putABAPStream(IFR_Parameter& parameter,
                                       IFRPacket_DataPart& datapart,
                                       void *maskptr,
                                       IFR_Int4            masklength,
                                       IFR_Int4            requiredrows,
                                       IFR_Bool&           last,
                                       IFR_ConnectionItem& clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, putABAPStream, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}


//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_Converter::getABAPStream(IFR_Parameter& parameter,
                                       IFRPacket_DataPart& datapart,
                                       IFR_Int4 masklength,
                                       void *maskptr,
                                       IFR_Int4& abap_rc,
                                       IFR_ConnectionItem& clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, getABAPStream, &clink);
    clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    DBUG_RETURN(IFR_NOT_OK);
}


//----------------------------------------------------------------------
IFR_TraceStream&
operator << (IFR_TraceStream& s, const IFRConversion_Converter& c)
{
    return s << "IFRConversion_Converter[index="
             << c.m_index
             << ", name='"
             << c.m_name
             << "']";
}

//----------------------------------------------------------------------
void
IFRConversion_Converter::sqlTrace(IFR_TraceStream& s)
{

    char buffer[512];
    sp77sprintf(buffer, 512, "%-3.d ", m_index);
    s << buffer;
    char sqltypebuffer[32];
    IFR_SQLTypeToString((IFR_SQLType)m_shortinfo.datatype, sqltypebuffer, 32);
    sp77sprintf(buffer, 512, "%-15.15s", sqltypebuffer);
    s << buffer;
    sp77sprintf(buffer, 512, "%-4.1d", (IFR_Int4)m_shortinfo.length);
    s << buffer;
    sp77sprintf(buffer,512, " %-3.1d", (IFR_Int4)m_shortinfo.frac);
    s << buffer;
}

IFR_END_NAMESPACE
