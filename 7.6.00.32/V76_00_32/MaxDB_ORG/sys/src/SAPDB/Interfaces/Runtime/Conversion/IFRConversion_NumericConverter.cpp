/*!
  @file           IFRConversion_NumericConverter.cpp
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
#include "Interfaces/Runtime/Conversion/IFRConversion_NumericConverter.h"
#include "Interfaces/Runtime/Util/IFRUtil_VDNNumber.h"
#include "Interfaces/Runtime/Util/IFRUtil_SQLNumeric.h"
#include "Interfaces/Runtime/IFR_Connection.h"
#include "hsp51.h"

#include <math.h>
#include <stdlib.h>

#ifdef OSF1
#ifndef _IEEE
#  ifndef _IEEE_FP
#    error This will only work with IEEE floating point arithmetic.
#  else
#    define _IEEE 1
#  endif
#endif
#include <nan.h>
#endif

#define DBUG_CLINK_METHOD_ENTER(x,y) DBUG_CONTEXT_METHOD_ENTER(x, y, &clink)

//----------------------------------------------------------------------
IFRConversion_NumericConverter::IFRConversion_NumericConverter(IFR_ShortInfo &shortinfo,
                                                               SAPDBMem_IRawAllocator& allocator,
                                                               IFR_Connection& connection)
:IFRConversion_Converter(shortinfo, allocator)
{
    m_noscientific = connection.numbersWithoutExponent();
    m_truncate     = connection.truncateNumbers();
}

//----------------------------------------------------------------------
IFRConversion_NumericConverter::~IFRConversion_NumericConverter()
{}


//----------------------------------------------------------------------
IFR_Retcode 
IFRConversion_NumericConverter::translateBinaryInput(IFRPacket_DataPart& datapart,
                                                     char               *data,
                                                     IFR_Length            datalength,
                                                     IFR_Length*           lengthindicator,
                                                     IFR_ConnectionItem& clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_NumericConverter, translateBinaryInput);
    IFR_Length used_datalength;
    if(IFRConversion_InputDataLength(datalength, lengthindicator, data, used_datalength, true /* force old behaviour */)) {
        clink.error().setRuntimeError(IFR_ERR_INVALID_LENGTHINDICATOR_I, (IFR_Int4)getIndex());
        DBUG_RETURN(IFR_NOT_OK);
    }
    if(used_datalength != m_shortinfo.iolength - 1) {
        clink.error().setRuntimeError(IFR_ERR_PARAM_CONVERSION_TRUNCATEDATA_I, (IFR_Int4)getIndex());
        DBUG_RETURN(IFR_NOT_OK);
    }
    unsigned char *num=(unsigned char*)datapart.getInputData(m_shortinfo);
    memcpy(num, data, used_datalength);
    datapart.finishData(m_shortinfo.iolength, m_shortinfo);
    DBUG_RETURN(IFR_OK);
}


//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_NumericConverter::translateAsciiInput(IFRPacket_DataPart& datapart,
                                                    char               *data,
                                                    IFR_Length          datalength,
                                                    IFR_Length*         lengthindicator,
                                                    IFR_Bool            terminate,
                                                    IFR_Bool            ascii7bit,  
                                                    IFR_ConnectionItem& clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_NumericConverter, translateAsciiInput);
    IFR_Length used_datalength;
    if(IFRConversion_InputDataLength(datalength, lengthindicator, data, used_datalength, terminate)) {
        clink.error().setRuntimeError(IFR_ERR_INVALID_LENGTHINDICATOR_I, (IFR_Int4)getIndex());
        DBUG_RETURN(IFR_NOT_OK);
    }
    
    if(ascii7bit) {
        if(!IFRConversion_StringIsAscii7((unsigned char *)data, used_datalength)) {
            clink.error().setRuntimeError(IFR_ERR_NOT_ASCII_CHARACTERS_I, (IFR_Int4)getIndex());
            DBUG_RETURN(IFR_NOT_OK);
        }
    }
    
    unsigned char *num=(unsigned char*)datapart.getInputData(m_shortinfo);
    IFR_Bool nan = false;
    IFR_Retcode rc=IFRUtil_VDNNumber::stringToNumber(data,
                                                     used_datalength,
                                                     IFR_StringEncodingAscii,
                                                     nan,
                                                     num,
                                                     !m_shortinfo.isFloat(),
                                                     m_shortinfo.length,
                                                     m_shortinfo.frac);
    
    // check overflow for integer types
    if(rc == IFR_OK || rc == IFR_DATA_TRUNC) {
        if(m_shortinfo.datatype == IFR_SQLTYPE_SMALLINT) {
            rc = ( IFRUtil_VDNNumber::isSmallInteger(num, m_shortinfo.iolength - 1) == IFR_OK) ? rc : IFR_OVERFLOW;
        } else if(m_shortinfo.datatype == IFR_SQLTYPE_INTEGER) {
            rc = ( IFRUtil_VDNNumber::isInteger(num, m_shortinfo.iolength - 1) == IFR_OK) ? rc : IFR_OVERFLOW;
        }
    }
    
    if(rc==IFR_OK || rc ==IFR_DATA_TRUNC) {
        datapart.finishData(m_shortinfo.iolength, m_shortinfo);
    } else if(rc==IFR_OVERFLOW) {
        clink.error().setRuntimeError(IFR_ERR_NUMERIC_OVERFLOW_I, (IFR_Int4)this->m_index);
    } else if(rc==IFR_NOT_OK) {
        clink.error().setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, (IFR_Int4)this->m_index);
    } else if(nan) {
        rc=IFR_NOT_OK;
        clink.error().setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, (IFR_Int4)this->m_index);
    }
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_NumericConverter::translateUCS2Input(IFRPacket_DataPart& datapart,
                                                   char               *data,
                                                   IFR_Bool            swapped,
                                                   IFR_Length          datalength,
                                                   IFR_Length*         lengthindicator,
                                                   IFR_Bool            terminate,
                                                   IFR_ConnectionItem& clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_NumericConverter, translateUCS2Input);
    IFR_Length  used_datalength;
    if(IFRConversion_InputDataLengthUCS2(datalength, lengthindicator, data, used_datalength, terminate)) {
        clink.error().setRuntimeError(IFR_ERR_INVALID_LENGTHINDICATOR_I, (IFR_Int4)getIndex());
        DBUG_RETURN(IFR_NOT_OK);
    }
    if(used_datalength % 2) {
        clink.error().setRuntimeError(IFR_ERR_ODD_DATALENGTH_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    }
    
    unsigned char *num=(unsigned char*)datapart.getInputData(m_shortinfo);
    IFR_Bool nan = false;
    IFR_Retcode rc=IFRUtil_VDNNumber::stringToNumber(data,
                                                     used_datalength,
                                                     swapped?IFR_StringEncodingUCS2Swapped:IFR_StringEncodingUCS2,
                                                     nan,
                                                     num,
                                                     !m_shortinfo.isFloat(),
                                                     m_shortinfo.length,
                                                     m_shortinfo.frac);
    // check overflow for integer types
    if(rc == IFR_OK || rc == IFR_DATA_TRUNC) {
        if(m_shortinfo.datatype == IFR_SQLTYPE_SMALLINT) {
            rc = ( IFRUtil_VDNNumber::isSmallInteger(num, m_shortinfo.iolength - 1) == IFR_OK) ? rc : IFR_OVERFLOW;
        } else if(m_shortinfo.datatype == IFR_SQLTYPE_INTEGER) {
            rc = ( IFRUtil_VDNNumber::isInteger(num, m_shortinfo.iolength - 1) == IFR_OK) ? rc : IFR_OVERFLOW;
        }
    }
    
    if(rc==IFR_OK || rc == IFR_DATA_TRUNC) {
        datapart.finishData(m_shortinfo.iolength, m_shortinfo);
    } else if(rc==IFR_OVERFLOW) {
        clink.error().setRuntimeError(IFR_ERR_NUMERIC_OVERFLOW_I, (IFR_Int4)this->m_index);
    } else if(rc==IFR_NOT_OK) {
        clink.error().setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, (IFR_Int4)this->m_index);
    } else if(nan) {
        rc=IFR_NOT_OK;
        clink.error().setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, (IFR_Int4)this->m_index);
    }
    DBUG_RETURN(rc);

}


//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_NumericConverter::translateUTF8Input(IFRPacket_DataPart& datapart,
                                                   char               *data,
                                                   IFR_Length          datalength,
                                                   IFR_Length*         lengthindicator,
                                                   IFR_Bool            terminate,  
                                                   IFR_ConnectionItem& clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_NumericConverter, translateUTF8Input);
    DBUG_RETURN(translateAsciiInput(datapart, data, datalength, lengthindicator, terminate, false, clink));
}




//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_NumericConverter::translateInput(IFRPacket_DataPart& datapart,
                                               double& data,
                                               IFR_Length *lengthindicator,
                                               IFR_ConnectionItem& clink,
                                               IFRConversion_Putval *pv)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_NumericConverter, translateInput_double);
    unsigned char num[20];
    IFR_Retcode conversion_rc;
    if(ISNAN(data)) {
        clink.error().setRuntimeError(IFR_ERR_NUMERIC_OVERFLOW_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    } 

    if(m_truncate && !m_shortinfo.isFloat()) {
        conversion_rc = IFRUtil_VDNNumber::doubleToNumber(data,
                                                          num, 
                                                          38, 
                                                          -1);
        if(conversion_rc == IFR_OK || conversion_rc == IFR_DATA_TRUNC) {
            tsp00_NumError numerror;
            unsigned char tmpnum[20];
            int resbytelen;
            memset(tmpnum, 0, sizeof(tmpnum));
            s51trunc(num, 1, 20, 
                     m_shortinfo.frac,
                     tmpnum,
                     1,
                     m_shortinfo.length,
                     m_shortinfo.frac,
                     resbytelen,
                     numerror);
            IFR_Int4 exp = (num[0] > 0x80) 
                ? num[0] - 192
                : (num[0] < 0x80 
                   ? - ((IFR_Int4)num[0]) + 64
                   : 0x80
                    );
            if(exp != 0x80) {
                IFR_Int4 lastdigit = 1;
                for(IFR_Int4 i=1; i<38; ++i) {
#define DIGIT(i) ((i%2) ? (num[1 + i/2] >> 4) : num[1 + i/2] & 0x0F)
                    if(DIGIT(i) != 0) {
                        lastdigit = i;
                    }
#undef  DIGIT                    
                }
                if((lastdigit - exp) > m_shortinfo.frac) {
                    numerror.becomes(num_trunc);
                }
            }
            memcpy(num, tmpnum, 20);
            switch(numerror) {
            case num_ok:
                conversion_rc = IFR_OK;
                break;
            case num_trunc:
                conversion_rc = IFR_DATA_TRUNC;
                break;
            case num_overflow:
                conversion_rc = IFR_OVERFLOW;
                break;
            default:
                conversion_rc = IFR_NOT_OK;
            }
        }
    } else {
        conversion_rc=IFRUtil_VDNNumber::doubleToNumber(data,
                                                        num, 
                                                        m_shortinfo.length, 
                                                        m_shortinfo.isFloat()?-1:m_shortinfo.frac);
    }
    // check overflow for integer types
    if(conversion_rc == IFR_OK || conversion_rc == IFR_DATA_TRUNC) {
        if(m_shortinfo.datatype == IFR_SQLTYPE_SMALLINT) {
            conversion_rc = ( IFRUtil_VDNNumber::isSmallInteger(num, m_shortinfo.iolength - 1) == IFR_OK) ? conversion_rc : IFR_OVERFLOW;
        } else if(m_shortinfo.datatype == IFR_SQLTYPE_INTEGER) {
            conversion_rc = ( IFRUtil_VDNNumber::isInteger(num, m_shortinfo.iolength - 1) == IFR_OK) ? conversion_rc : IFR_OVERFLOW;
        }
    }

    if(conversion_rc == IFR_OVERFLOW) {
        clink.error().setRuntimeError(IFR_ERR_NUMERIC_OVERFLOW_I, (IFR_Int4)this->m_index);
    } else if(conversion_rc == IFR_NOT_OK) {
        clink.error().setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, (IFR_Int4)this->m_index);
    } else if(conversion_rc==IFR_OK || conversion_rc==IFR_DATA_TRUNC) {
        datapart.addBinaryParameter(num, m_shortinfo.iolength-1, m_shortinfo);
    }
    DBUG_RETURN(conversion_rc);
}

//----------------------------------------------------------------------
IFR_Retcode 
IFRConversion_NumericConverter::translateDecimalInput(IFRPacket_DataPart& datapart,
                                                      char               *data,
                                                      IFR_Length          datalength,
                                                      IFR_Length         *length,
                                                      IFR_ConnectionItem& clink,
                                                      IFRConversion_Putval *pv)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_NumericConverter, translateDecimalInput);

    if(length == 0) {
        clink.error().setRuntimeError(IFR_ERR_NULL_DECIMAL_INDICATOR_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    } else {
        if((*length & 0xFFFF0000) != 0x40000000) {
            clink.error().setRuntimeError(IFR_ERR_DECIMAL_INDICATOR_CORRUPT_I, (IFR_Int4)this->m_index);
            DBUG_RETURN(IFR_NOT_OK);
        }
    }


    unsigned char num[20];
    IFR_Int4 digits    = SQLDBC_DECIMAL_DIGITS(*length);
    IFR_Int4 fraction  = SQLDBC_DECIMAL_FRACTION(*length);

    if(fraction > digits) {
        clink.error().setRuntimeError(IFR_ERR_INVALID_DECIMAL_SPECIFICATION_III, 
                                      (IFR_Int4)this->m_index,
                                      digits,
                                      fraction);
        DBUG_RETURN(IFR_NOT_OK);
    }

    if(datalength < SQLDBC_DECIMAL_LENGTH(digits, fraction)) {
        clink.error().setRuntimeError(IFR_ERR_DECIMAL_BUFFER_TOO_SHORT_I,
                                      (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    }
    IFR_Retcode rc = IFRUtil_VDNNumber::decimalToNumber((unsigned char *)data,
                                                        digits,
                                                        fraction,
                                                        num,
                                                        m_shortinfo.length,
                                                        m_shortinfo.isFloat()?-1:m_shortinfo.frac);

    // check overflow for integer types
    if(rc == IFR_OK || rc == IFR_DATA_TRUNC) {
        if(m_shortinfo.datatype == IFR_SQLTYPE_SMALLINT) {
            rc = ( IFRUtil_VDNNumber::isSmallInteger(num, m_shortinfo.iolength - 1) == IFR_OK) ? rc : IFR_OVERFLOW;
        } else if(m_shortinfo.datatype == IFR_SQLTYPE_INTEGER) {
            rc = ( IFRUtil_VDNNumber::isInteger(num, m_shortinfo.iolength - 1) == IFR_OK) ? rc : IFR_OVERFLOW;
        }
    }

    if(rc == IFR_OVERFLOW) {
        clink.error().setRuntimeError(IFR_ERR_NUMERIC_OVERFLOW_I, (IFR_Int4)this->m_index);
    } else if(rc == IFR_NOT_OK) {
        clink.error().setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, (IFR_Int4)this->m_index);
    } else if(rc==IFR_OK || rc==IFR_DATA_TRUNC) {
        datapart.addBinaryParameter(num, m_shortinfo.iolength-1, m_shortinfo);
    }
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode 
IFRConversion_NumericConverter::translateOmsPacked_8_3_Input(IFRPacket_DataPart& datapart,
                                                             char               *data,
                                                             IFR_Length          datalength,
                                                             IFR_Length         *length,
                                                             IFR_ConnectionItem& clink,
                                                             IFRConversion_Putval *pv)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_NumericConverter, translateOmsPacked_8_3_Input);
    // ? should we check datalength against too short data ?
    unsigned char num[20];
    IFR_Retcode rc = IFRUtil_VDNNumber::decimalToNumber((unsigned char *)data,
                                                        15,
                                                        3,
                                                        num,
                                                        m_shortinfo.length,
                                                        m_shortinfo.isFloat()?-1:m_shortinfo.frac);

    // check overflow for integer types
    if(rc == IFR_OK || rc == IFR_DATA_TRUNC) {
        if(m_shortinfo.datatype == IFR_SQLTYPE_SMALLINT) {
            rc = ( IFRUtil_VDNNumber::isSmallInteger(num, m_shortinfo.iolength - 1) == IFR_OK) ? rc : IFR_OVERFLOW;
        } else if(m_shortinfo.datatype == IFR_SQLTYPE_INTEGER) {
            rc = ( IFRUtil_VDNNumber::isInteger(num, m_shortinfo.iolength - 1) == IFR_OK) ? rc : IFR_OVERFLOW;
        }
    }

    if(rc == IFR_OVERFLOW) {
        clink.error().setRuntimeError(IFR_ERR_NUMERIC_OVERFLOW_I, (IFR_Int4)this->m_index);
    } else if(rc == IFR_NOT_OK) {
        clink.error().setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, (IFR_Int4)this->m_index);
    } else if(rc==IFR_OK || rc==IFR_DATA_TRUNC) {
        datapart.addBinaryParameter(num, m_shortinfo.iolength-1, m_shortinfo);
    }
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode 
IFRConversion_NumericConverter::translateOmsPacked_15_3_Input(IFRPacket_DataPart& datapart,
                                                             char               *data,
                                                             IFR_Length          datalength,
                                                             IFR_Length         *length,
                                                              IFR_ConnectionItem& clink,
                                                              IFRConversion_Putval *pv)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_NumericConverter, translateOmsPacked_15_3_Input);
    // ? should we check datalength against too short data ?
    unsigned char num[20];
    IFR_Retcode rc = IFRUtil_VDNNumber::decimalToNumber((unsigned char *)data,
                                                        29,
                                                        3,
                                                        num,
                                                        m_shortinfo.length,
                                                        m_shortinfo.isFloat()?-1:m_shortinfo.frac);
    // check overflow for integer types
    if(rc == IFR_OK || rc == IFR_DATA_TRUNC) {
        if(m_shortinfo.datatype == IFR_SQLTYPE_SMALLINT) {
            rc = ( IFRUtil_VDNNumber::isSmallInteger(num, m_shortinfo.iolength - 1) == IFR_OK) ? rc : IFR_OVERFLOW;
        } else if(m_shortinfo.datatype == IFR_SQLTYPE_INTEGER) {
            rc = ( IFRUtil_VDNNumber::isInteger(num, m_shortinfo.iolength - 1) == IFR_OK) ? rc : IFR_OVERFLOW;
        }
    }

    if(rc == IFR_OVERFLOW) {
        clink.error().setRuntimeError(IFR_ERR_NUMERIC_OVERFLOW_I, (IFR_Int4)this->m_index);
    } else if(rc == IFR_NOT_OK) {
        clink.error().setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, (IFR_Int4)this->m_index);
    } else if(rc==IFR_OK || rc==IFR_DATA_TRUNC) {
        datapart.addBinaryParameter(num, m_shortinfo.iolength-1, m_shortinfo);
    }
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode 
IFRConversion_NumericConverter::translateOmsTimestampInput(IFRPacket_DataPart& datapart,
                                                           char               *data,
                                                           IFR_Length          datalength,
                                                           IFR_Length         *length,
                                                           IFR_ConnectionItem& clink,
                                                           IFRConversion_Putval *pv)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_NumericConverter, translateOmsTimestampInput);
    // ? should we check datalength against too short data ?
    unsigned char num[20];
    IFR_Retcode rc = IFRUtil_VDNNumber::decimalToNumber((unsigned char *)data,
                                                        15,
                                                        0,
                                                        num,
                                                        m_shortinfo.length,
                                                        m_shortinfo.isFloat()?-1:m_shortinfo.frac);
    // check overflow for integer types
    if(rc == IFR_OK || rc == IFR_DATA_TRUNC) {
        if(m_shortinfo.datatype == IFR_SQLTYPE_SMALLINT) {
            rc = ( IFRUtil_VDNNumber::isSmallInteger(num, m_shortinfo.iolength - 1) == IFR_OK) ? rc : IFR_OVERFLOW;
        } else if(m_shortinfo.datatype == IFR_SQLTYPE_INTEGER) {
            rc = ( IFRUtil_VDNNumber::isInteger(num, m_shortinfo.iolength - 1) == IFR_OK) ? rc : IFR_OVERFLOW;
        }
    }

    if(rc == IFR_OVERFLOW) {
        clink.error().setRuntimeError(IFR_ERR_NUMERIC_OVERFLOW_I, (IFR_Int4)this->m_index);
    } else if(rc == IFR_NOT_OK) {
        clink.error().setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, (IFR_Int4)this->m_index);
    } else if(rc==IFR_OK || rc==IFR_DATA_TRUNC) {
        datapart.addBinaryParameter(num, m_shortinfo.iolength-1, m_shortinfo);
    }
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_NumericConverter::translateInput(IFRPacket_DataPart& datapart,
                                               float& data,
                                               IFR_Length *lengthindicator,
                                               IFR_ConnectionItem& clink,
                                               IFRConversion_Putval *pv)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_NumericConverter, translateInput_float);
    double d=data;
    DBUG_RETURN(translateInput(datapart, d, lengthindicator, clink, pv));
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_NumericConverter::translateInput(IFRPacket_DataPart& part,
                                               IFR_Int1& data,
                                               IFR_Length* lengthindicator,
                                               IFR_ConnectionItem& clink,
                                               IFRConversion_Putval *pv)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_NumericConverter, translateInput_Int1);
    unsigned char *num=(unsigned char*)part.getInputData(m_shortinfo);
    memset(num, 0, m_shortinfo.iolength);
    IFR_Retcode rc=IFRUtil_VDNNumber::int1ToNumber(data, num, m_shortinfo.length-m_shortinfo.frac, m_shortinfo.isFloat());

    // check overflow for integer types
    if(rc == IFR_OK || rc == IFR_DATA_TRUNC) {
        if(m_shortinfo.datatype == IFR_SQLTYPE_SMALLINT) {
            rc = ( IFRUtil_VDNNumber::isSmallInteger(num, m_shortinfo.iolength - 1) == IFR_OK) ? rc : IFR_OVERFLOW;
        } else if(m_shortinfo.datatype == IFR_SQLTYPE_INTEGER) {
            rc = ( IFRUtil_VDNNumber::isInteger(num, m_shortinfo.iolength - 1) == IFR_OK) ? rc : IFR_OVERFLOW;
        }
    }

    if(rc==IFR_OK) {
        part.finishData(m_shortinfo.iolength, m_shortinfo);
    } else if(rc==IFR_OVERFLOW) {
        clink.error().setRuntimeError(IFR_ERR_NUMERIC_OVERFLOW_I, (IFR_Int4)this->m_index);
    } else if(rc==IFR_NOT_OK) {
        clink.error().setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, (IFR_Int4)this->m_index);
    }
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_NumericConverter::translateInput(IFRPacket_DataPart& part,
                                               IFR_Int2& data,
                                               IFR_Length* lengthindicator,
                                               IFR_ConnectionItem& clink,
                                               IFRConversion_Putval *pv)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_NumericConverter, translateInput_Int2);
    unsigned char *num=(unsigned char*)part.getInputData(m_shortinfo);
    memset(num, 0, m_shortinfo.iolength);
    IFR_Retcode rc=IFRUtil_VDNNumber::int2ToNumber(data, num, m_shortinfo.length-m_shortinfo.frac, m_shortinfo.isFloat());

    // check overflow for integer types
    if(rc == IFR_OK || rc == IFR_DATA_TRUNC) {
        if(m_shortinfo.datatype == IFR_SQLTYPE_SMALLINT) {
            rc = ( IFRUtil_VDNNumber::isSmallInteger(num, m_shortinfo.iolength - 1) == IFR_OK) ? rc : IFR_OVERFLOW;
        } else if(m_shortinfo.datatype == IFR_SQLTYPE_INTEGER) {
            rc = ( IFRUtil_VDNNumber::isInteger(num, m_shortinfo.iolength - 1) == IFR_OK) ? rc : IFR_OVERFLOW;
        }
    }

    if(rc==IFR_OK) {
        part.finishData(m_shortinfo.iolength, m_shortinfo);
    } else if(rc==IFR_OVERFLOW) {
        clink.error().setRuntimeError(IFR_ERR_NUMERIC_OVERFLOW_I, (IFR_Int4)this->m_index);
    } else if(rc==IFR_NOT_OK) {
        clink.error().setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, (IFR_Int4)this->m_index);
    }
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_NumericConverter::translateInput(IFRPacket_DataPart& part,
                                               IFR_Int4& data,
                                               IFR_Length* lengthindicator,
                                               IFR_ConnectionItem& clink,
                                               IFRConversion_Putval *pv)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_NumericConverter, translateInput_Int4);
    DBUG_PRINT(data);
    unsigned char *num=(unsigned char*)part.getInputData(m_shortinfo);
    memset(num, 0, m_shortinfo.iolength);
    IFR_Retcode rc=IFRUtil_VDNNumber::int4ToNumber(data, num, m_shortinfo.length-m_shortinfo.frac, m_shortinfo.isFloat());

    // check overflow for integer types
    if(rc == IFR_OK || rc == IFR_DATA_TRUNC) {
        if(m_shortinfo.datatype == IFR_SQLTYPE_SMALLINT) {
            rc = ( IFRUtil_VDNNumber::isSmallInteger(num, m_shortinfo.iolength - 1) == IFR_OK) ? rc : IFR_OVERFLOW;
        } else if(m_shortinfo.datatype == IFR_SQLTYPE_INTEGER) {
            rc = ( IFRUtil_VDNNumber::isInteger(num, m_shortinfo.iolength - 1) == IFR_OK) ? rc : IFR_OVERFLOW;
        }
    }

    if(rc==IFR_OK) {
        part.finishData(m_shortinfo.iolength, m_shortinfo);
    } else if(rc==IFR_OVERFLOW) {
        clink.error().setRuntimeError(IFR_ERR_NUMERIC_OVERFLOW_I, (IFR_Int4)this->m_index);
    } else if(rc==IFR_NOT_OK) {
        clink.error().setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, (IFR_Int4)this->m_index);
    }
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_NumericConverter::translateInput(IFRPacket_DataPart& part,
                                               IFR_Int8 &data,
                                               IFR_Length* lengthindicator,
                                               IFR_ConnectionItem& clink,
                                               IFRConversion_Putval *pv)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_NumericConverter, translateInput_Int8);
    unsigned char *num=(unsigned char*)part.getInputData(m_shortinfo);
    memset(num, 0, m_shortinfo.iolength);
    IFR_Retcode rc=IFRUtil_VDNNumber::int8ToNumber(data, num, m_shortinfo.length-m_shortinfo.frac, m_shortinfo.isFloat());

    // check overflow for integer types
    if(rc == IFR_OK || rc == IFR_DATA_TRUNC) {
        if(m_shortinfo.datatype == IFR_SQLTYPE_SMALLINT) {
            rc = ( IFRUtil_VDNNumber::isSmallInteger(num, m_shortinfo.iolength - 1) == IFR_OK) ? rc : IFR_OVERFLOW;
        } else if(m_shortinfo.datatype == IFR_SQLTYPE_INTEGER) {
            rc = ( IFRUtil_VDNNumber::isInteger(num, m_shortinfo.iolength - 1) == IFR_OK) ? rc : IFR_OVERFLOW;
        }
    }

    if(rc==IFR_OK) {
        part.finishData(m_shortinfo.iolength, m_shortinfo);
    } else if(rc==IFR_OVERFLOW) {
        clink.error().setRuntimeError(IFR_ERR_NUMERIC_OVERFLOW_I, (IFR_Int4)this->m_index);
    } else if(rc==IFR_NOT_OK) {
        clink.error().setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, (IFR_Int4)this->m_index);
    }
    DBUG_RETURN(rc);
}


//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_NumericConverter::translateInput(IFRPacket_DataPart& part,
                                               IFR_UInt1& data,
                                               IFR_Length* lengthindicator,
                                               IFR_ConnectionItem& clink,
                                               IFRConversion_Putval *pv)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_NumericConverter, translateInput_UInt1);
    unsigned char *num=(unsigned char*)part.getInputData(m_shortinfo);
    memset(num, 0, m_shortinfo.iolength);
    IFR_Retcode rc=IFRUtil_VDNNumber::uint1ToNumber(data, num, m_shortinfo.length-m_shortinfo.frac, m_shortinfo.isFloat());

    // check overflow for integer types
    if(rc == IFR_OK || rc == IFR_DATA_TRUNC) {
        if(m_shortinfo.datatype == IFR_SQLTYPE_SMALLINT) {
            rc = ( IFRUtil_VDNNumber::isSmallInteger(num, m_shortinfo.iolength - 1) == IFR_OK) ? rc : IFR_OVERFLOW;
        } else if(m_shortinfo.datatype == IFR_SQLTYPE_INTEGER) {
            rc = ( IFRUtil_VDNNumber::isInteger(num, m_shortinfo.iolength - 1) == IFR_OK) ? rc : IFR_OVERFLOW;
        }
    }

    if(rc==IFR_OK) {
        part.finishData(m_shortinfo.iolength, m_shortinfo);
    } else if(rc==IFR_OVERFLOW) {
        clink.error().setRuntimeError(IFR_ERR_NUMERIC_OVERFLOW_I, (IFR_Int4)this->m_index);
    } else if(rc==IFR_NOT_OK) {
        clink.error().setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, (IFR_Int4)this->m_index);
    }
    DBUG_RETURN(rc);
}
//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_NumericConverter::translateInput(IFRPacket_DataPart& part,
                                               IFR_UInt2& data,
                                               IFR_Length* lengthindicator,
                                               IFR_ConnectionItem& clink,
                                               IFRConversion_Putval *pv)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_NumericConverter, translateInput_UInt2);
    unsigned char *num=(unsigned char*)part.getInputData(m_shortinfo);
    memset(num, 0, m_shortinfo.iolength);
    IFR_Retcode rc=IFRUtil_VDNNumber::uint2ToNumber(data, num, m_shortinfo.length-m_shortinfo.frac, m_shortinfo.isFloat());

    // check overflow for integer types
    if(rc == IFR_OK || rc == IFR_DATA_TRUNC) {
        if(m_shortinfo.datatype == IFR_SQLTYPE_SMALLINT) {
            rc = ( IFRUtil_VDNNumber::isSmallInteger(num, m_shortinfo.iolength - 1) == IFR_OK) ? rc : IFR_OVERFLOW;
        } else if(m_shortinfo.datatype == IFR_SQLTYPE_INTEGER) {
            rc = ( IFRUtil_VDNNumber::isInteger(num, m_shortinfo.iolength - 1) == IFR_OK) ? rc : IFR_OVERFLOW;
        }
    }

    if(rc==IFR_OK) {
        part.finishData(m_shortinfo.iolength, m_shortinfo);
    } else if(rc==IFR_OVERFLOW) {
        clink.error().setRuntimeError(IFR_ERR_NUMERIC_OVERFLOW_I, (IFR_Int4)this->m_index);
    } else if(rc==IFR_NOT_OK) {
        clink.error().setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, (IFR_Int4)this->m_index);
    }
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_NumericConverter::translateInput(IFRPacket_DataPart& part,
                                               IFR_UInt4& data,
                                               IFR_Length* lengthindicator,
                                               IFR_ConnectionItem& clink,
                                               IFRConversion_Putval *pv)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_NumericConverter, translateInput_UInt4);
    unsigned char *num=(unsigned char*)part.getInputData(m_shortinfo);
    memset(num, 0, m_shortinfo.iolength);
    IFR_Retcode rc=IFRUtil_VDNNumber::uint4ToNumber(data, num, m_shortinfo.length-m_shortinfo.frac, m_shortinfo.isFloat());

    // check overflow for integer types
    if(rc == IFR_OK || rc == IFR_DATA_TRUNC) {
        if(m_shortinfo.datatype == IFR_SQLTYPE_SMALLINT) {
            rc = ( IFRUtil_VDNNumber::isSmallInteger(num, m_shortinfo.iolength - 1) == IFR_OK) ? rc : IFR_OVERFLOW;
        } else if(m_shortinfo.datatype == IFR_SQLTYPE_INTEGER) {
            rc = ( IFRUtil_VDNNumber::isInteger(num, m_shortinfo.iolength - 1) == IFR_OK) ? rc : IFR_OVERFLOW;
        }
    }

    if(rc==IFR_OK) {
        part.finishData(m_shortinfo.iolength, m_shortinfo);
    } else if(rc==IFR_OVERFLOW) {
        clink.error().setRuntimeError(IFR_ERR_NUMERIC_OVERFLOW_I, (IFR_Int4)this->m_index);
    } else if(rc==IFR_NOT_OK) {
        clink.error().setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, (IFR_Int4)this->m_index);
    }
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_NumericConverter::translateInput(IFRPacket_DataPart& part,
                                               IFR_UInt8 &data,
                                               IFR_Length* lengthindicator,
                                               IFR_ConnectionItem& clink,
                                               IFRConversion_Putval *pv)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_NumericConverter, translateInput_UInt8);
    unsigned char *num=(unsigned char*)part.getInputData(m_shortinfo);
    memset(num, 0, m_shortinfo.iolength);
    IFR_Retcode rc=IFRUtil_VDNNumber::uint8ToNumber(data, num, m_shortinfo.length-m_shortinfo.frac, m_shortinfo.isFloat());

    // check overflow for integer types
    if(rc == IFR_OK || rc == IFR_DATA_TRUNC) {
        if(m_shortinfo.datatype == IFR_SQLTYPE_SMALLINT) {
            rc = ( IFRUtil_VDNNumber::isSmallInteger(num, m_shortinfo.iolength - 1) == IFR_OK) ? rc : IFR_OVERFLOW;
        } else if(m_shortinfo.datatype == IFR_SQLTYPE_INTEGER) {
            rc = ( IFRUtil_VDNNumber::isInteger(num, m_shortinfo.iolength - 1) == IFR_OK) ? rc : IFR_OVERFLOW;
        }
    }

    if(rc==IFR_OK) {
        part.finishData(m_shortinfo.iolength, m_shortinfo);
    } else if(rc==IFR_OVERFLOW) {
        clink.error().setRuntimeError(IFR_ERR_NUMERIC_OVERFLOW_I, (IFR_Int4)this->m_index);
    } else if(rc==IFR_NOT_OK) {
        clink.error().setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, (IFR_Int4)this->m_index);
    }
    DBUG_RETURN(rc);
}


//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_NumericConverter::translateOutput(IFRPacket_DataPart& datapart,
                                                IFR_Int1& data,
                                                IFR_Length *lengthindicator,
                                                IFR_ConnectionItem& clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_NumericConverter, translateOutput_Int1);
    unsigned char *vdn=(unsigned char *)datapart.getOutputData(m_shortinfo.pos.bufpos);        
    IFR_Retcode rc=IFRUtil_VDNNumber::numberToInt1(vdn, data, m_shortinfo.iolength);
    if(rc == IFR_NOT_OK) {
        clink.error().setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, (IFR_Int4)this->m_index);
    }
    if(rc==IFR_OVERFLOW) {
        clink.error().setRuntimeError(IFR_ERR_NUMERIC_OVERFLOW_I, (IFR_Int4)this->m_index);
    }
    if(lengthindicator) 
        *lengthindicator=sizeof(data);
    DBUG_PRINT(data);
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_NumericConverter::translateOutput(IFRPacket_DataPart& datapart,
                                                IFR_Int2& data,
                                                IFR_Length *lengthindicator,
                                                IFR_ConnectionItem& clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_NumericConverter, translateOutput_Int2);
    unsigned char *vdn=(unsigned char *)datapart.getOutputData(m_shortinfo.pos.bufpos);        
    IFR_Retcode rc=IFRUtil_VDNNumber::numberToInt2(vdn, data, m_shortinfo.iolength);
    if(rc == IFR_NOT_OK) {
        clink.error().setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, (IFR_Int4)this->m_index);
    }
    if(rc==IFR_OVERFLOW) {
        clink.error().setRuntimeError(IFR_ERR_NUMERIC_OVERFLOW_I, (IFR_Int4)this->m_index);
    }
    if(lengthindicator) 
        *lengthindicator=sizeof(data);
    DBUG_PRINT(data);
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_NumericConverter::translateOutput(IFRPacket_DataPart& datapart,
                                                IFR_Int4& data,
                                                IFR_Length *lengthindicator,
                                                IFR_ConnectionItem& clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_NumericConverter, translateOutput_Int4);
    unsigned char *vdn=(unsigned char *)datapart.getOutputData(m_shortinfo.pos.bufpos);        
    IFR_Retcode rc=IFRUtil_VDNNumber::numberToInt4(vdn, data, m_shortinfo.iolength);
    if(rc == IFR_NOT_OK) {
        clink.error().setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, (IFR_Int4)this->m_index);
    }
    if(rc==IFR_OVERFLOW) {
        clink.error().setRuntimeError(IFR_ERR_NUMERIC_OVERFLOW_I, (IFR_Int4)this->m_index);
    }
    if(lengthindicator) 
        *lengthindicator=sizeof(data);
    DBUG_PRINT(data);
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_NumericConverter::translateOutput(IFRPacket_DataPart& datapart,
                                                IFR_Int8& data,
                                                IFR_Length *lengthindicator,
                                                IFR_ConnectionItem& clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_NumericConverter, translateOutput_Int8);
    unsigned char *vdn=(unsigned char *)datapart.getOutputData(m_shortinfo.pos.bufpos);        
    IFR_Retcode rc=IFRUtil_VDNNumber::numberToInt8(vdn, data, m_shortinfo.iolength);
    if(rc == IFR_NOT_OK) {
        clink.error().setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, (IFR_Int4)this->m_index);
    }
    if(rc==IFR_OVERFLOW) {
        clink.error().setRuntimeError(IFR_ERR_NUMERIC_OVERFLOW_I, (IFR_Int4)this->m_index);
    }
    if(lengthindicator) 
        *lengthindicator=sizeof(data);
    DBUG_PRINT(data);
    DBUG_RETURN(rc);
}


//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_NumericConverter::translateOutput(IFRPacket_DataPart& datapart,
                                                IFR_UInt1& data,
                                                IFR_Length *lengthindicator,
                                                IFR_ConnectionItem& clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_NumericConverter, translateOutput_UInt1);
    unsigned char *vdn=(unsigned char *)datapart.getOutputData(m_shortinfo.pos.bufpos);        
    IFR_Retcode rc=IFRUtil_VDNNumber::numberToUInt1(vdn, data, m_shortinfo.iolength);
    if(rc == IFR_NOT_OK) {
        clink.error().setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, (IFR_Int4)this->m_index);
    }
    if(rc==IFR_OVERFLOW) {
        clink.error().setRuntimeError(IFR_ERR_NUMERIC_OVERFLOW_I, (IFR_Int4)this->m_index);
    }
    if(lengthindicator) 
        *lengthindicator=sizeof(data);
    DBUG_PRINT(data);
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_NumericConverter::translateOutput(IFRPacket_DataPart& datapart,
                                                IFR_UInt2& data,
                                                IFR_Length *lengthindicator,
                                                IFR_ConnectionItem& clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_NumericConverter, translateOutput_UInt2);
    unsigned char *vdn=(unsigned char *)datapart.getOutputData(m_shortinfo.pos.bufpos);        
    IFR_Retcode rc=IFRUtil_VDNNumber::numberToUInt2(vdn, data, m_shortinfo.iolength);
    if(rc == IFR_NOT_OK) {
        clink.error().setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, (IFR_Int4)this->m_index);
    }
    if(rc==IFR_OVERFLOW) {
        clink.error().setRuntimeError(IFR_ERR_NUMERIC_OVERFLOW_I, (IFR_Int4)this->m_index);
    }
    if(lengthindicator) 
        *lengthindicator=sizeof(data);
    DBUG_PRINT(data);
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_NumericConverter::translateOutput(IFRPacket_DataPart& datapart,
                                                IFR_UInt4& data,
                                                IFR_Length *lengthindicator,
                                                IFR_ConnectionItem& clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_NumericConverter, translateOutput_UInt4);
    unsigned char *vdn=(unsigned char *)datapart.getOutputData(m_shortinfo.pos.bufpos);        
    IFR_Retcode rc=IFRUtil_VDNNumber::numberToUInt4(vdn, data, m_shortinfo.iolength);
    if(rc == IFR_NOT_OK) {
        clink.error().setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, (IFR_Int4)this->m_index);
    }
    if(rc==IFR_OVERFLOW) {
        clink.error().setRuntimeError(IFR_ERR_NUMERIC_OVERFLOW_I, (IFR_Int4)this->m_index);
    }
    if(lengthindicator) 
        *lengthindicator=sizeof(data);
    DBUG_PRINT(data);
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_NumericConverter::translateOutput(IFRPacket_DataPart& datapart,
                                                IFR_UInt8& data,
                                                IFR_Length *lengthindicator,
                                                IFR_ConnectionItem& clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_NumericConverter, translateOutput_UInt8);
    unsigned char *vdn=(unsigned char *)datapart.getOutputData(m_shortinfo.pos.bufpos);        
    IFR_Retcode rc=IFRUtil_VDNNumber::numberToUInt8(vdn, data, m_shortinfo.iolength);
    if(rc == IFR_NOT_OK) {
        clink.error().setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, (IFR_Int4)this->m_index);
    }
    if(rc==IFR_OVERFLOW) {
        clink.error().setRuntimeError(IFR_ERR_NUMERIC_OVERFLOW_I, (IFR_Int4)this->m_index);
    }
    if(lengthindicator) 
        *lengthindicator=sizeof(data);
    DBUG_PRINT(data);
    DBUG_RETURN(rc);
}


//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_NumericConverter::translateOutput(IFRPacket_DataPart& datapart,
                                                double& data,
                                                IFR_Length *lengthindicator,
                                                IFR_ConnectionItem& clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_NumericConverter, translateOutput_double);
    unsigned char *vdn=(unsigned char *)datapart.getOutputData(m_shortinfo.pos.bufpos);        
    // Check whether there was a division by zero or similar harmful stuff
    if(*(vdn - 1) == csp_oflw_byte) {
#if defined(WIN32)
        data=sqrt(-1.0); errno=0;
#elif defined(OSF1)
        // There is no way to create a NaN by a function on the DEC.
        // So it must be done manually.
        memset(&data, 0, 8);
        dnan *np = (dnan *)&data;
        np->nan_parts.exponent     = 0x7FF;
        np->nan_parts.bits         = 1;
        np->nan_parts.fraction_low = 1;
#else
        data=strtod("nan", NULL);
#endif
        DBUG_RETURN(IFR_OK);
    }
    IFR_Retcode rc=IFRUtil_VDNNumber::numberToDouble(vdn, data, m_shortinfo.length);
    if(rc == IFR_NOT_OK) {
        clink.error().setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, (IFR_Int4)this->m_index);
    }
    if(rc==IFR_OVERFLOW) {
        clink.error().setRuntimeError(IFR_ERR_NUMERIC_OVERFLOW_I, (IFR_Int4)this->m_index);
    }
    if(lengthindicator) 
        *lengthindicator=sizeof(data);
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode 
IFRConversion_NumericConverter::translateDecimalOutput(IFRPacket_DataPart& datapart,
                                                       char               *data,
                                                       IFR_Length          datalength,
                                                       IFR_Length         *lengthindicator,
                                                       IFR_ConnectionItem& clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_NumericConverter, translateDecimalOutput);
    if(lengthindicator == 0) {
        clink.error().setRuntimeError(IFR_ERR_NULL_DECIMAL_INDICATOR_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    } else {
        if((*lengthindicator & 0xFFFF0000) != 0x40000000) {
            clink.error().setRuntimeError(IFR_ERR_DECIMAL_INDICATOR_CORRUPT_I, (IFR_Int4)this->m_index);
            DBUG_RETURN(IFR_NOT_OK);
        }
    }
    IFR_Int4 digits    = SQLDBC_DECIMAL_DIGITS(*lengthindicator);
    IFR_Int4 fraction  = SQLDBC_DECIMAL_FRACTION(*lengthindicator);
    if(datalength < SQLDBC_DECIMAL_LENGTH(digits, fraction)) {
        clink.error().setRuntimeError(IFR_ERR_DECIMAL_BUFFER_TOO_SHORT_I,
                                      (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    }
    
    unsigned char *vdn=(unsigned char *)datapart.getOutputData(m_shortinfo.pos.bufpos);        

    IFR_Retcode rc = IFRUtil_VDNNumber::numberToDecimal(vdn, m_shortinfo.length, (unsigned char *)data, digits, fraction);
    if(rc == IFR_NOT_OK) {
        clink.error().setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, (IFR_Int4)this->m_index);
    }
    if(rc==IFR_OVERFLOW) {
        clink.error().setRuntimeError(IFR_ERR_NUMERIC_OVERFLOW_I, (IFR_Int4)this->m_index);
    }
    if(lengthindicator) 
        *lengthindicator = SQLDBC_DECIMAL_LENGTH(digits, fraction);
    DBUG_RETURN(rc);
    
}

//----------------------------------------------------------------------
IFR_Retcode 
IFRConversion_NumericConverter::translateOmsPacked_8_3_Output(IFRPacket_DataPart& datapart,
                                                              char               *data,
                                                              IFR_Length          datalength,
                                                              IFR_Length         *lengthindicator,
                                                              IFR_ConnectionItem& clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_NumericConverter, translateOmsPacked_8_3_Output);
    unsigned char *vdn=(unsigned char *)datapart.getOutputData(m_shortinfo.pos.bufpos);        

    IFR_Retcode rc=IFRUtil_VDNNumber::numberToDecimal(vdn, m_shortinfo.length, (unsigned char *)data, 15, 3);
    if(rc == IFR_NOT_OK) {
        clink.error().setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, (IFR_Int4)this->m_index);
    }
    if(rc==IFR_OVERFLOW) {
        clink.error().setRuntimeError(IFR_ERR_NUMERIC_OVERFLOW_I, (IFR_Int4)this->m_index);
    }
    if(lengthindicator) 
        *lengthindicator = 8;
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode 
IFRConversion_NumericConverter::translateOmsPacked_15_3_Output(IFRPacket_DataPart& datapart,
                                                              char               *data,
                                                              IFR_Length          datalength,
                                                              IFR_Length         *lengthindicator,
                                                              IFR_ConnectionItem& clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_NumericConverter, translateOmsPacked_15_3_Output);
    unsigned char *vdn=(unsigned char *)datapart.getOutputData(m_shortinfo.pos.bufpos);        

    IFR_Retcode rc=IFRUtil_VDNNumber::numberToDecimal(vdn, m_shortinfo.length, (unsigned char *)data, 29, 3);
    if(rc == IFR_NOT_OK) {
        clink.error().setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, (IFR_Int4)this->m_index);
    }
    if(rc==IFR_OVERFLOW) {
        clink.error().setRuntimeError(IFR_ERR_NUMERIC_OVERFLOW_I, (IFR_Int4)this->m_index);
    }
    if(lengthindicator) 
        *lengthindicator = 15;
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode 
IFRConversion_NumericConverter::translateOmsTimestampOutput(IFRPacket_DataPart& datapart,
                                                            char               *data,
                                                            IFR_Length          datalength,
                                                            IFR_Length         *lengthindicator,
                                                            IFR_ConnectionItem& clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_NumericConverter, translateOmsTimestampOutput);
    unsigned char *vdn=(unsigned char *)datapart.getOutputData(m_shortinfo.pos.bufpos);        

    IFR_Retcode rc=IFRUtil_VDNNumber::numberToDecimal(vdn, m_shortinfo.length, (unsigned char *)data, 15, 0);
    if(rc == IFR_NOT_OK) {
        clink.error().setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, (IFR_Int4)this->m_index);
    }
    if(rc==IFR_OVERFLOW) {
        clink.error().setRuntimeError(IFR_ERR_NUMERIC_OVERFLOW_I, (IFR_Int4)this->m_index);
    }
    if(lengthindicator) 
        *lengthindicator = 8;
    DBUG_RETURN(rc);
}


//----------------------------------------------------------------------
IFR_Retcode 
IFRConversion_NumericConverter::translateOutput(IFRPacket_DataPart&   datapart,
                                                float&      data,
                                                IFR_Length*             lengthindicator,
                                                IFR_ConnectionItem&   clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_NumericConverter, translateOutput_float);
    double d;
    IFR_Retcode rc=translateOutput(datapart, d, lengthindicator, clink);
    if(rc == IFR_OK) {
        // NAN is different for float
        if(ISNAN(d)) {
            data = (float)d;
            DBUG_RETURN(IFR_OK);
        }
        // check the MAX_FLOAT limit
        if(d > 3.4028235e+38) {
            clink.error().setRuntimeError(IFR_ERR_NUMERIC_OVERFLOW_I, (IFR_Int4)this->m_index);
            DBUG_RETURN(IFR_NOT_OK);
        }
        data = (float)d; // the resulting precision loss is accepted here,
        // and inevitable. 
        if(lengthindicator) {
            *lengthindicator=sizeof(float);
        }
    }
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
inline IFR_size_t ucsx_bytelen(char *c)
{
    IFR_size_t result=0;
    while(*c || *(c+1)) {
        c+=2;
        result+=2;
    }
    return result;
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_NumericConverter::translateCharacterOutput(IFRPacket_DataPart& datapart,
                                                         char           *data,
                                                         IFR_Length        datalength,
                                                         IFR_Length*       lengthindicator,
                                                         IFR_ConnectionItem &clink,
                                                         IFR_StringEncoding encoding,
                                                         IFR_Bool terminate)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_NumericConverter, translateAsciiOutput);
    unsigned char *vdn=(unsigned char *)datapart.getOutputData(m_shortinfo.pos.bufpos);        

    // unfortunately, we must possibly report the original length to the
    // caller. So for now we will copy the result anyway ...
    char tmp_data[140]; // 64 digits (E64), '.' and '-', if no scientific ... makes 132 (plus some safety)
    const char *numstr=""; 
    IFR_size_t numlength=0;
    if(*(vdn - 1) == csp_oflw_byte) {
        if(encoding == IFR_StringEncodingAscii || encoding==IFR_StringEncodingUTF8) {
            numstr="NaN";
            numlength=3;
        } else if (encoding == IFR_StringEncodingUCS2) {
            numstr="\0N\0a\0N\0";
            numlength=6;
        } else if (encoding == IFR_StringEncodingUCS2Swapped) {
            numstr="N\0a\0N\0\0";
            numlength=6;
        }
    } else {
        IFR_Retcode rc=
            IFRUtil_VDNNumber::numberToString(vdn, 
                                              tmp_data,
                                              sizeof(tmp_data),
                                              encoding,
                                              m_noscientific || !m_shortinfo.isFloat(),
                                              m_shortinfo.length,
                                              m_shortinfo.frac);
        
        if(rc!=IFR_OK) {
            DBUG_RETURN(IFR_NOT_OK);
        }
        numstr=tmp_data;
        if(encoding == IFR_StringEncodingAscii || encoding == IFR_StringEncodingUTF8) {
            numlength=strlen(tmp_data);
        } else {
            numlength = ucsx_bytelen(tmp_data);
        }
    }
    IFR_Length terminatorSize=IFR_ENCODING(encoding)->terminatorSize;
    IFR_size_t copylength = (numlength < (IFR_size_t)datalength - ((terminate) ? terminatorSize : 0)) 
      ? numlength + ((terminate) ? terminatorSize : 0) 
      : datalength;

    memcpy(data, numstr, copylength);
    if(terminate) {
        memset(data+copylength-terminatorSize, 0, terminatorSize);
    }
    if(lengthindicator) {
        *lengthindicator=(IFR_Length)numlength;
    }
    
    if(numlength > (IFR_size_t)(datalength - ((terminate) ? terminatorSize : 0))) {
        DBUG_RETURN(IFR_DATA_TRUNC);
    } else {
        DBUG_RETURN(IFR_OK);
    }
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_NumericConverter::translateBinaryOutput(IFRPacket_DataPart& datapart,
                                                      char *data,
                                                      IFR_Length datalength,
                                                      IFR_Length *lengthindicator,
                                                      IFR_ConnectionItem& clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_NumericConverter, translateBinaryOutput);
    IFR_Length  offset    = 1;
    IFR_Length  dataoffset= 0;
    DBUG_RETURN(appendBinaryOutput(datapart, data, datalength, lengthindicator, clink, dataoffset, offset, 0));
}

//----------------------------------------------------------------------
IFR_Retcode 
IFRConversion_NumericConverter::appendBinaryOutput(IFRPacket_DataPart& datapart,
                                                   char               *data,
                                                   IFR_Length          datalength,
                                                   IFR_Length*         lengthindicator,
                                                   IFR_ConnectionItem& clink,
                                                   IFR_Length         &dataoffset,
                                                   IFR_Length         &offset,
                                                   IFRConversion_Getval *getval)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_NumericConverter, appendBinaryOutput);
    
    if(dataoffset != 0 || offset != 1) {
        clink.error().setRuntimeError(IFR_ERR_BINARY_APPEND_I, (IFR_Int4)getIndex());
        DBUG_RETURN(IFR_NOT_OK);
    }

    char       *read_data   = datapart.getOutputData(m_shortinfo.pos.bufpos);
    IFR_Length  byteslength = m_shortinfo.iolength - 1;
    
    if(lengthindicator) {
        *lengthindicator = byteslength;
    }
    
    if(datalength < byteslength) {
        char buffer[40];
        m_shortinfo.toString(buffer, sizeof(buffer));
        clink.error().setRuntimeError(IFR_ERR_BINARY_TRUNCATION_SI, buffer, (IFR_Int4)getIndex());
        DBUG_RETURN(IFR_NOT_OK);
    } else {
        memcpy(data, read_data, byteslength);
        DBUG_RETURN(IFR_OK);
    }    
}



//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_NumericConverter::translateUTF8Output(IFRPacket_DataPart& datapart,
                                                    char           *data,
                                                    IFR_Length        datalength,
                                                    IFR_Length*       lengthindicator,
                                                    IFR_Bool       terminate,
                                                    IFR_ConnectionItem &clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_NumericConverter, translateUTF8Output);
    DBUG_RETURN(translateCharacterOutput(datapart, data, datalength, lengthindicator, clink, IFR_StringEncodingUTF8, terminate));
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_NumericConverter::translateAsciiOutput(IFRPacket_DataPart& datapart,
                                                     char           *data,
                                                     IFR_Length     datalength,
                                                     IFR_Length*    lengthindicator,
                                                     IFR_Bool       terminate,
                                                     IFR_Bool       ascii7bit,
                                                     IFR_ConnectionItem &clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_NumericConverter, translateAsciiOutput);
    DBUG_RETURN(translateCharacterOutput(datapart, data, datalength, lengthindicator, clink, IFR_StringEncodingAscii, terminate));
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_NumericConverter::translateUCS2Output(IFRPacket_DataPart& datapart,
                                                    char           *data,
                                                    IFR_Bool        swapped,
                                                    IFR_Length        datalength,
                                                    IFR_Length*       lengthindicator,
                                                    IFR_Bool       terminate,
                                                    IFR_ConnectionItem &clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_NumericConverter, translateUCS2Output);
    IFR_StringEncoding encoding = (swapped) ? IFR_StringEncodingUCS2Swapped : IFR_StringEncodingUCS2;
    DBUG_RETURN(translateCharacterOutput(datapart, data, datalength, lengthindicator, clink, encoding, terminate));
}

//----------------------------------------------------------------------
IFR_Retcode 
IFRConversion_NumericConverter::translateInput(IFRPacket_DataPart&   datapart,
                                               SQL_NUMERIC_STRUCT&      data,
                                               IFR_Length*             lengthindicator,
                                               IFR_ConnectionItem&   clink,
                                               IFRConversion_Putval *pv)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_NumericConverter, translateInput_NUMERIC);
    unsigned char num[20];
    IFR_Retcode rc;
    if(m_truncate && !m_shortinfo.isFloat()) {
        rc = IFRUtil_SQLNumeric::numericToNumber(data, num, 38);
        if(rc == IFR_OK || rc == IFR_DATA_TRUNC) {
            tsp00_NumError numerror;
            unsigned char tmpnum[20];
            int resbytelen;
            memset(tmpnum, 0, sizeof(tmpnum));
            s51trunc(num, 1, 20, 
                     m_shortinfo.frac,
                     tmpnum,
                     1,
                     m_shortinfo.length,
                     m_shortinfo.frac,
                     resbytelen,
                     numerror);
            IFR_Int4 exp = (num[0] > 0x80) 
                ? num[0] - 192
                : (num[0] < 0x80 
                   ? - ((IFR_Int4)num[0]) + 64
                   : 0x80
                    );
            if(exp != 0x80) {
                IFR_Int4 lastdigit = 1;
                for(IFR_Int4 i=1; i<38; ++i) {
#define DIGIT(i) ((i%2) ? (num[1 + i/2] >> 4) : num[1 + i/2] & 0x0F)
                    if(DIGIT(i) != 0) {
                        lastdigit = i;
                    }
#undef  DIGIT                    
                }
                if((lastdigit - exp) > m_shortinfo.frac) {
                    numerror.becomes(num_trunc);
                }
            }
            memcpy(num, tmpnum, 20);
            switch(numerror) {
            case num_ok:
                rc = IFR_OK;
                break;
            case num_trunc:
                rc = IFR_DATA_TRUNC;
                break;
            case num_overflow:
                rc = IFR_OVERFLOW;
                break;
            default:
                rc = IFR_NOT_OK;
            }
        }
    } else {
        rc = IFRUtil_SQLNumeric::numericToNumber(data, num, m_shortinfo.length);
    }

    if(rc == IFR_OK || rc == IFR_DATA_TRUNC) {
        if(m_shortinfo.datatype == IFR_SQLTYPE_SMALLINT) {
            rc = ( IFRUtil_VDNNumber::isSmallInteger(num, m_shortinfo.iolength - 1) == IFR_OK) ? rc : IFR_OVERFLOW;
        } else if(m_shortinfo.datatype == IFR_SQLTYPE_INTEGER) {
            rc = ( IFRUtil_VDNNumber::isInteger(num, m_shortinfo.iolength - 1) == IFR_OK) ? rc : IFR_OVERFLOW;
        }
    }
    
    if(rc==IFR_OK || rc == IFR_DATA_TRUNC) {
        datapart.addBinaryParameter(num, m_shortinfo.iolength-1, m_shortinfo);
    } else if(rc==IFR_OVERFLOW) {
        clink.error().setRuntimeError(IFR_ERR_NUMERIC_OVERFLOW_I, (IFR_Int4)this->m_index);
    } else if(rc==IFR_NOT_OK) {
        clink.error().setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, (IFR_Int4)this->m_index);
    }
    DBUG_RETURN(rc);
}



//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_NumericConverter::translateOutput(IFRPacket_DataPart&   datapart,
                                               SQL_NUMERIC_STRUCT&    data,
                                               IFR_Length*            lengthindicator,
                                               IFR_ConnectionItem&    clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_NumericConverter, translateOutput_NUMERIC);
    unsigned char *vdn=(unsigned char *)datapart.getOutputData(m_shortinfo.pos.bufpos);
    if(lengthindicator) {
        *lengthindicator=sizeof(SQL_NUMERIC_STRUCT);
    }
    IFR_Retcode rc=IFRUtil_SQLNumeric::numberToNumeric(vdn, 
                                                       m_shortinfo.length,
                                                       data,
                                                       m_shortinfo.frac);
    DBUG_RETURN(rc);
}



