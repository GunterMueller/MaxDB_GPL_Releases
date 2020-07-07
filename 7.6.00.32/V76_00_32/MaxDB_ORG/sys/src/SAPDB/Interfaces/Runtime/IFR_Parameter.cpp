/*!
  @file           IFR_Parameter.cpp
  @author         D039759
  @ingroup        IFR_DataConv
  @brief          Handling of parameters and hostvars
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
//----------------------------------------------------------------------
#include "Interfaces/Runtime/IFR_Parameter.h"
#include "livecachetypes.h" /* no check */
#include "Interfaces/Runtime/Conversion/IFRConversion_Converter.h"

//----------------------------------------------------------------------
static inline IFR_size_t string_nlen(char *s, IFR_size_t max)
{
    for(IFR_size_t i=0; i<max; ++i) {
        if(*(s+i) == 0) {
            return i;
        }
    }
    return max;
}

//----------------------------------------------------------------------
const char*
IFR_Parameter::getHosttypeString() const
{
    return IFR_HostTypeToString(m_hosttype);
}

//----------------------------------------------------------------------
const IFR_Length
IFR_Parameter::getBytesLength() const
{
    switch(m_hosttype) {
    case IFR_HOSTTYPE_PARAMETER_NOTSET:
        return -1;
    case IFR_HOSTTYPE_BINARY:
    case IFR_HOSTTYPE_ASCII:
    case IFR_HOSTTYPE_UCS2_SWAPPED:
    case IFR_HOSTTYPE_UCS2:
    case IFR_HOSTTYPE_UTF8:
    case IFR_HOSTTYPE_STREAM:
        return m_byteslength;

    case IFR_HOSTTYPE_UINT1:
        return sizeof(IFR_UInt1);
    case IFR_HOSTTYPE_INT1:
        return sizeof(IFR_Int1);
    case IFR_HOSTTYPE_UINT2:
        return sizeof(IFR_UInt2);
    case IFR_HOSTTYPE_INT2:
        return sizeof(IFR_Int2);
    case IFR_HOSTTYPE_UINT4:
        return sizeof(IFR_UInt4);
    case IFR_HOSTTYPE_INT4:
        return sizeof(IFR_Int4);
    case IFR_HOSTTYPE_UINT8:
        return sizeof(IFR_UInt8);
    case IFR_HOSTTYPE_INT8:
        return sizeof(IFR_Int8);
    case IFR_HOSTTYPE_DOUBLE:
        return sizeof(double);
    case IFR_HOSTTYPE_FLOAT:
        return sizeof(float);
    case IFR_HOSTTYPE_ODBCDATE:
        return sizeof(SQL_DATE_STRUCT);
    case IFR_HOSTTYPE_ODBCTIME:
        return sizeof(SQL_TIME_STRUCT);
    case IFR_HOSTTYPE_ODBCTIMESTAMP:
        return sizeof(SQL_TIMESTAMP_STRUCT);

    case IFR_HOSTTYPE_OMS_PACKED_8_3:
        return 8;
    case IFR_HOSTTYPE_OMS_PACKED_15_3:
        return 15;
    case IFR_HOSTTYPE_OMS_TIMESTAMP:
        return 8;
    case IFR_HOSTTYPE_DECIMAL:
        return m_byteslength;

    case IFR_HOSTTYPE_GUID:
        return sizeof(GUID);
    default:
        return -1;
    }
}

//----------------------------------------------------------------------
IFR_SQLType
IFR_Parameter::getPreferredSQLType() const
{
    switch(m_hosttype) {
    case IFR_HOSTTYPE_BINARY:
        return IFR_SQLTYPE_CHB;
    case IFR_HOSTTYPE_ASCII:
        return IFR_SQLTYPE_CHA;
    case IFR_HOSTTYPE_UCS2_SWAPPED:
    case IFR_HOSTTYPE_UCS2:
    case IFR_HOSTTYPE_UTF8:
        return IFR_SQLTYPE_CHA;
    case IFR_HOSTTYPE_UINT1:
    case IFR_HOSTTYPE_INT1:
        return IFR_SQLTYPE_FIXED;
    case IFR_HOSTTYPE_UINT2:
        return IFR_SQLTYPE_FIXED;
    case IFR_HOSTTYPE_INT2:
        return IFR_SQLTYPE_SMALLINT;
    case IFR_HOSTTYPE_UINT4:
        return IFR_SQLTYPE_FIXED;
    case IFR_HOSTTYPE_INT4:
        return IFR_SQLTYPE_INTEGER;
    case IFR_HOSTTYPE_UINT8:
    case IFR_HOSTTYPE_INT8:
        return IFR_SQLTYPE_FIXED;
    case IFR_HOSTTYPE_DOUBLE:
        return IFR_SQLTYPE_FLOAT;
    case IFR_HOSTTYPE_FLOAT:
        return IFR_SQLTYPE_FLOAT;
    case IFR_HOSTTYPE_ODBCDATE:
        return IFR_SQLTYPE_DATE;
    case IFR_HOSTTYPE_ODBCTIME:
        return IFR_SQLTYPE_TIME;
    case IFR_HOSTTYPE_ODBCTIMESTAMP:
        return IFR_SQLTYPE_TIMESTAMP;
    case IFR_HOSTTYPE_ODBCNUMERIC:
        return IFR_SQLTYPE_FLOAT;
    case IFR_HOSTTYPE_GUID:
        return IFR_SQLTYPE_CHB;
    case IFR_HOSTTYPE_OMS_PACKED_8_3:
    case IFR_HOSTTYPE_OMS_PACKED_15_3:
    case IFR_HOSTTYPE_OMS_TIMESTAMP:
    case IFR_HOSTTYPE_DECIMAL:
        return IFR_SQLTYPE_FIXED;
    case IFR_HOSTTYPE_PARAMETER_NOTSET:
    default:
        return IFR_SQLTYPE_CHA;
    }
}

//----------------------------------------------------------------------
IFR_Bool
IFR_Parameter::isABAPTable(IFR_Int4 abapTabId)
{
    if(m_hosttype != IFR_HOSTTYPE_STREAM) {
        return false;
    }

    SQLStreamDesc *streamdesc = (SQLStreamDesc *) asChar();
    if(streamdesc && streamdesc->StreamParam->Stream.hStream.ABAPTabId == abapTabId) {
        return true;
    } else {
        return false;
    }
}

//----------------------------------------------------------------------
IFR_size_t
IFR_Parameter::getPreferredLength() const
{
    switch(m_hosttype) {
    // more than 8008 bytes are not supported, and may lead
    // to unexpected results.
    case IFR_HOSTTYPE_BINARY:
        return m_byteslength<=0 || m_byteslength>8008 ? 8008 : m_byteslength;
    case IFR_HOSTTYPE_ASCII:
        return m_byteslength<=0 || m_byteslength>8008 ? 8008 : m_byteslength;
    case IFR_HOSTTYPE_UCS2:
    case IFR_HOSTTYPE_UCS2_SWAPPED:
        return m_byteslength<=0 || m_byteslength>8008 ? 4004 : m_byteslength/2;
    case IFR_HOSTTYPE_UTF8:
        return m_byteslength<=0 || m_byteslength>8008 ? 4004 : m_byteslength;
    case IFR_HOSTTYPE_UINT1:
    case IFR_HOSTTYPE_INT1:
        return 3;
    case IFR_HOSTTYPE_UINT2:
        return 5;
    case IFR_HOSTTYPE_INT2:
        return 5;
    case IFR_HOSTTYPE_UINT4:
        return 10;
    case IFR_HOSTTYPE_INT4:
        return 10;
    case IFR_HOSTTYPE_UINT8:
        return 38;
    case IFR_HOSTTYPE_INT8:
        return 38;
    case IFR_HOSTTYPE_DOUBLE:
        return 9;
    case IFR_HOSTTYPE_FLOAT:
        return 16;
    /* !!! for these types, a length supplied is graceful ignored,
           if it is <> 0.
     */
    case IFR_HOSTTYPE_ODBCDATE:
    case IFR_HOSTTYPE_ODBCTIME:
    case IFR_HOSTTYPE_ODBCTIMESTAMP:
        return 50;
    case IFR_HOSTTYPE_ODBCNUMERIC:
        return 38;
    case IFR_HOSTTYPE_GUID:
        return 16;
    case IFR_HOSTTYPE_OMS_PACKED_8_3:
        return 15;
    case IFR_HOSTTYPE_OMS_PACKED_15_3:
        return 29;
    case IFR_HOSTTYPE_DECIMAL:
        return (m_byteslength * 2) - 1 > 38 ? 38 : (m_byteslength * 2) - 1;
    case IFR_HOSTTYPE_OMS_TIMESTAMP:
        return 15;
    case IFR_HOSTTYPE_PARAMETER_NOTSET:
    default:
        return 1;
    }

}

//----------------------------------------------------------------------
IFR_StringEncoding
IFR_Parameter::getEncoding(IFR_HostType hosttype)
{
    switch(hosttype) {
    case IFR_HOSTTYPE_BINARY:
    case IFR_HOSTTYPE_ASCII:
    case IFR_HOSTTYPE_BLOB:
    case IFR_HOSTTYPE_ASCII_LOB:
        return IFR_StringEncodingAscii;
    case IFR_HOSTTYPE_UCS2:
    case IFR_HOSTTYPE_UCS2_LOB:
        return IFR_StringEncodingUCS2;
    case IFR_HOSTTYPE_UCS2_SWAPPED:
    case IFR_HOSTTYPE_UCS2_SWAPPED_LOB:
        return IFR_StringEncodingUCS2Swapped;
    case IFR_HOSTTYPE_UTF8:
        return IFR_StringEncodingUTF8;
    default:
        return IFR_StringEncodingType::Unknown; // 0
    }
}

//----------------------------------------------------------------------
static inline void
convert_parameterindex(char *target, IFR_UInt2 parameterindex)
{
    if(parameterindex > 999) {
        memcpy(target, "###", 3);
        return;
    } else {
        IFR_UInt2 divisor = 100;
        IFR_Bool  started = false;
        while(divisor) {
            if(started) {
                *target = '0' + (parameterindex / divisor) % 10;
                ++target;
            } else if((parameterindex / divisor) % 10) {
                started = true;
                continue;
            }
            divisor = divisor / 10;
        }
    }
}

//----------------------------------------------------------------------
static inline void
convert_parameterlength(char *target, IFR_Length l)
{
    if(l >= 1000000) {
        IFR_Trace_sprintf(target, 64, "%-10d", l);
    } else {
        memset(target, ' ', 10);
        if(l==0) {
            *target = '0';
            return;
        }
        register IFR_Length divisor = 1000000;
        register IFR_Bool   started = false;
        while(divisor) {
            if(started) {
                *target = '0' + ((l / divisor) % 10);
                ++target;
            } else if((l / divisor) % 10) {
                started = true;
                continue;
            }
            divisor = divisor / 10;
        }
    }
}


//----------------------------------------------------------------------
static inline void
convert_pointer(char *target, void *p)
{
#ifdef BIT64
    IFR_UInt8 p_value = (IFR_UInt8)p;
    IFR_UInt8 p_mask  = 0xF000000000000000;
    IFR_UInt8 shift_val = 60;
    IFR_UInt8 c_val;

#else
    IFR_UInt4 p_value = (IFR_UInt4)p;
    IFR_UInt4 p_mask  = 0xF0000000;
    IFR_UInt4 shift_val = 28;
    IFR_UInt4 c_val;
#endif
    const char * hexdigit = "0123456789ABCDEF";
    while(p_mask != 0) {
        c_val = (p_value & p_mask) >> shift_val;
        *target = hexdigit[c_val];
        ++target;
        shift_val -= 4;
        p_mask >>= 4;
    }
    return;
}


//----------------------------------------------------------------------
IFR_Bool
IFR_Parameter::sqlTraceParameter(IFR_TraceStream& s, IFR_UInt2 parameterindex, IFR_Bool nodata)
{
    // Format is:                   0xFFFFFFFFFFFFFFFF 0xFFFFFFFFFFFFFFFF 0xFFFFFFFFFFFFFFFF
    // "I   T          AT L         I                  D                  P"
    // "I   T          AT L         I                  D                  P"
    if(m_hosttype == IFR_HOSTTYPE_PARAMETER_NOTSET) {
        char buffer[80];
        strcpy(buffer, "   *** NOT SET ***");
        convert_parameterindex(buffer, parameterindex);
        s.print(buffer);
        return false;
    } else {
        const char *hosttype_str = IFR_HostTypeToString(m_hosttype);
        IFR_Int4    hosttype_copylen = strlen(hosttype_str);
        if(hosttype_copylen > 10) {
            hosttype_copylen = 10;
        }
        char buffer[80];
        //                 1234567890123456789012345678
        // memcpy(buffer, "--- ---------- -- ----------", 28);
        memcpy(buffer, "                            ", 29);
        convert_parameterlength(&(buffer[18]), m_byteslength);
        convert_parameterindex(buffer, parameterindex);
        memcpy(&(buffer[4]), hosttype_str, hosttype_copylen);
        if(m_addrbound) {
            buffer[15]='A';
        }
        if(m_terminate) {
            buffer[16]='T';
        }
        s.print(buffer);
        if(!nodata) {
#ifndef BIT64
            strcpy(buffer, " 0xXXXXXXXX 0xXXXXXXXX 0xXXXXXXXX");
            convert_pointer(&(buffer[3]), (void *)m_lengthindicator);
            convert_pointer(&(buffer[14]), (void *)m_data);
            convert_pointer(&(buffer[25]), (void *)m_posindicator);
#else
            strcpy(buffer, " 0xXXXXXXXXXXXXXXXX 0xXXXXXXXXXXXXXXXX 0xXXXXXXXXXXXXXXXX");
            convert_pointer(&(buffer[3]), (void *)m_lengthindicator);
            convert_pointer(&(buffer[22]), (void *)m_data);
            convert_pointer(&(buffer[41]), (void *)m_posindicator);
#endif
            s.print(buffer);
        }
        return true;
    }
}

//----------------------------------------------------------------------
IFR_Bool
IFR_Parameter::sqlTraceParameter(IFR_TraceStream& s, const char * parametertag, IFR_Bool nodata)
{
    // Format is:               0xFFFFFFFFFFFFFFFF 0xFFFFFFFFFFFFFFFF 0xFFFFFFFFFFFFFFFF
    // "I   T          AT L     I                  D                  P"
    if(m_hosttype == IFR_HOSTTYPE_PARAMETER_NOTSET) {
        char buffer[80];
        IFR_Trace_sprintf(buffer, 80, "%-3.3s *** NOT SET ***", parametertag);
        s << buffer;
        return false;
    } else {
        const char *hosttype_str = IFR_HostTypeToString(m_hosttype);
        char buffer[80];
        IFR_Trace_sprintf(buffer, 80, "%-3.3s %-10.10s %c%c %-10d",
                    parametertag,
                    hosttype_str,
                    m_addrbound?'A':' ',
                    m_terminate?'T':' ',
                    m_byteslength);
        s.print(buffer);
        if(!nodata) {
#ifndef BIT64
            IFR_Trace_sprintf(buffer, 80, " 0x%-8.8p 0x%-8.8p 0x%-8.8p",
                        (void *)m_lengthindicator,
                        (void *)m_data,
                        (void *)m_posindicator);

#else
            IFR_Trace_sprintf(buffer, 80, " 0x%-16.16p 0x%-16.16p 0x%-16.16p" ,
                        (void *)m_lengthindicator,
                        (void *)m_data,
                        (void *)m_posindicator);

#endif
            s.print(buffer);
        }
        return true;
    }
}


//----------------------------------------------------------------------
static const char *get_abaptype(unsigned char abap_type)
{
    switch(abap_type) {
#define RETURN_TEXT(X) case X: return #X

        RETURN_TEXT(ABTYPC);
        RETURN_TEXT(ABTYPDATE);
        RETURN_TEXT(ABTYPP);
        RETURN_TEXT(ABTYPTIME);
        RETURN_TEXT(ABTYPX);
        RETURN_TEXT(ABTYPTABH);
        RETURN_TEXT(ABTYPNUM);
        RETURN_TEXT(ABTYPFLOAT);
        RETURN_TEXT(ABTYPINT);
        RETURN_TEXT(ABTYPINT2);
        RETURN_TEXT(ABTYPINT1);
        RETURN_TEXT(ABTYPW);
        RETURN_TEXT(ABTYP1);
        RETURN_TEXT(ABTYP2);
        RETURN_TEXT(ABTYPSTRUC1);
        RETURN_TEXT(ABTYPSTRUC2);
        RETURN_TEXT(ABTYPREF);
        RETURN_TEXT(ABTYPOBJ1);
        RETURN_TEXT(ABTYPOBJ2);
        RETURN_TEXT(ABTYPnull);
        RETURN_TEXT(ABTYPWYDE);
    default:
        return "***INVALID TYPE***";
    }
#undef RETURN_TEXT
}

#define DEC_DIGIT(b, i) ((i%2) ? (b[i/2] & 0xF) : (b[i/2] >> 4))


//----------------------------------------------------------------------
static void
trace_decimal(IFR_TraceStream& s, void *num_param, IFR_Int4 length, IFR_Int4 dec)
{
    unsigned char *num = (unsigned char *)num_param;
    IFR_Bool corrupt = false;
    if(length == -1 || dec == -1) {
        s << "*INVALID INDICATOR VALUE*";
    } else {
        char buffer[41];
        memset(buffer, 0, 41);
        char *p=&buffer[0];
        if(DEC_DIGIT(num, length) == 0xD) {
            *p='-';
            ++p;
        } else {
            *p='+';
            ++p;
        }
        for(IFR_Int4 i=0; i<length; ++i) {
            if(i == (length - dec)) {
                *p='.';
                ++p;
            }
            IFR_Int4 ix = DEC_DIGIT(num, i);
            if(ix > 9) {
                *p = '?';
                ++p;
                corrupt = true;
            } else {
                *p = ("0123456789")[ix];
                ++p;
            }
        }
        s << buffer;
        if(corrupt) {
            s << " *CORRUPT*";
        }
    }
}

//----------------------------------------------------------------------
static void
trace_decimal_digitsonly(IFR_TraceStream& s, void *num_data, IFR_Length* ind)
{
    if(ind == 0) {
        s << "*NO INDICATOR*";
    } else {
        unsigned char *num = (unsigned char *)num_data;
        unsigned int l = (*ind) * 2;
        if(DEC_DIGIT(num, l-1) > 9) {
            --l;
        }
        trace_decimal(s, num_data, l, 0);
        s << " (only digits)";
    }
}

//----------------------------------------------------------------------
static void
trace_stream(IFR_TraceStream& s,
             void *streamdata)
{
    const char *prefix = "                                         ";
    SQLStreamDesc *streamdesc = (SQLStreamDesc *) streamdata;
    SQL_LC_StreamParm *streamparam = streamdata ? streamdesc->StreamParam : 0;
    if(streamparam == 0) {
        s << "NULL STREAM PARAMETER";
    } else {
        OmsTypeABAPTabHandle *tabhandle = &(streamparam->ABAPTab.hABAPTab);
        if(tabhandle == 0) {
            s << "NULL ABAP TAB HANDLE";
        } else {
            s <<           "ROW SIZE  : " << tabhandle->rowSize << endl
              << prefix << "COL COUNT : " << tabhandle->colCount << endl
              << prefix << "ROW COUNT : " << tabhandle->rowCount << endl
              << prefix << "ABAP TABID: " << tabhandle->ABAPTabId << endl;
            for(int i=0; i<tabhandle->colCount; ++i) {
                s << prefix
                  << "COL[" << (i+1) << "]=(INOUT=" << (IFR_Int4)(tabhandle->colDesc[i].inout)
                  << ", TYPE=" << get_abaptype(tabhandle->colDesc[i].abap_type)
                  << ", DEC=" << tabhandle->colDesc[i].dec
                  << ", LENGTH=" << tabhandle->colDesc[i].length
                  << ", OFS=" << tabhandle->colDesc[i].offset
                  << ")";
                if(i != tabhandle->colCount-1) {
                    s << endl;
                }
            }
        }
    }
}


//----------------------------------------------------------------------.
void
IFR_Parameter:: sqlTraceParameterData(IFR_TraceStream& s,
                                      IFR_UInt2        index,
                                      IFR_Retcode      conversionRC,
                                      IFR_Int4         offset,
                                      IFR_Int4         rowsize,
                                      unsigned char   *parameterdata,
                                      IFR_Length      *lengthindicator,
                                      IFR_Bool         isinput)
{
    if(sqlTraceParameter(s, index, true)) {
        IFR_Bool nodatapresent=false;
        IFR_Length *used_lengthindicator = lengthindicator?lengthindicator:getLengthIndicator(offset, rowsize);
        if(used_lengthindicator) {
            if(m_hosttype != IFR_HOSTTYPE_DECIMAL || !isinput) {
                switch(*used_lengthindicator) {
                case IFR_DATA_AT_EXEC:
                    s.print(" DATA AT EXEC");
                    nodatapresent = true;
                    break;
                case IFR_NULL_DATA:
                    s.print(" NULL        ");
                    nodatapresent = true;
                    break;
                case IFR_NTS:
                    s.print(" NTS         ");
                    break;
                case IFR_DEFAULT_PARAM:
                    s.print(" DEFAULT     ");
                    nodatapresent = true;
                    break;
                case IFR_NO_TOTAL:
                    s.print(" NO TOTAL    ");
                    break;
                case IFR_IGNORE:
                    s.print(" IGNORE      ");
                    nodatapresent = true;
                    break;
                default: {
                	if(m_hosttype != IFR_HOSTTYPE_DECIMAL){
                    char buf[16];
                    IFR_Trace_sprintf(buf, 16, " %-10d  ", (IFR_Int4)(*used_lengthindicator));
                    s << buf;
                  }else{
		                char buf[16];
		                IFR_Trace_sprintf(buf, 16, "    (%2d,%2d)  ", m_digits, m_fraction);
		                s << buf;
                  } 
                }
                }
            } else {
                char buf[16];
                IFR_Trace_sprintf(buf, 16, "    (%2d,%2d)  ", m_digits, m_fraction);
                s << buf;
            }

        } else {
            s.print(" NULL PTR    ");
        }

        if((conversionRC==IFR_OK || conversionRC==IFR_DATA_TRUNC || isinput) && !nodatapresent) {
            IFR_Length byteslength=0;
            if (m_hosttype == IFR_HOSTTYPE_UCS2 ||
                m_hosttype == IFR_HOSTTYPE_UCS2_SWAPPED) {
              IFRConversion_InputDataLengthUCS2(m_byteslength, used_lengthindicator,
                                                (char *)data(offset, rowsize, parameterdata),
                                                byteslength, shouldTerminated());
            } else {
              IFRConversion_InputDataLength(m_byteslength, used_lengthindicator,
                                            (char *)data(offset, rowsize, parameterdata),
                                            byteslength, shouldTerminated());
            }
            IFR_Length tracelength = byteslength;
            if (conversionRC == IFR_DATA_TRUNC && !isinput) {
                tracelength = m_byteslength;
                if (shouldTerminated()) {
                    if (m_hosttype == IFR_HOSTTYPE_UCS2 ||
                        m_hosttype == IFR_HOSTTYPE_UCS2_SWAPPED && tracelength >= 2) {
                        tracelength-=2;
                    } else  if (tracelength > 0) {
                        tracelength--;
                    }
                }
            }
            switch(m_hosttype) {
            case IFR_HOSTTYPE_ASCII:
            case IFR_HOSTTYPE_RAWHEX:
                s << "'" << inputlength(MIN(IFR_MAX_TRACELENGTH, tracelength))
                  << inputencoding(IFR_StringEncodingAscii)
                  << data(offset, rowsize, parameterdata);
                if (tracelength > IFR_MAX_TRACELENGTH)
                    s << "...";
                else
                    s << "'";
                break;
            case IFR_HOSTTYPE_UTF8:
                s << "'" << inputlength(MIN(IFR_MAX_TRACELENGTH, tracelength)) << inputencoding(IFR_StringEncodingUTF8)
                  << data(offset, rowsize, parameterdata);
                if (tracelength > IFR_MAX_TRACELENGTH) s << "...";
                else s << "'";
                break;
            case IFR_HOSTTYPE_UCS2:
                s << "'" << inputlength(MIN(IFR_MAX_TRACELENGTH, tracelength)) << inputencoding(IFR_StringEncodingUCS2)
                  << data(offset, rowsize, parameterdata);
                if (tracelength > IFR_MAX_TRACELENGTH) s << "...";
                else s << "'";
                break;
            case IFR_HOSTTYPE_UCS2_SWAPPED:
                s << "'" << inputlength(MIN(IFR_MAX_TRACELENGTH, tracelength)) << inputencoding(IFR_StringEncodingUCS2Swapped)
                  << data(offset, rowsize, parameterdata);
                if (tracelength > IFR_MAX_TRACELENGTH) s << "...";
                else s << "'";
                break;
            case IFR_HOSTTYPE_BINARY:
                s << hex << inputlength(MIN(IFR_MAX_TRACELENGTH/2, tracelength)) << data(offset, rowsize, parameterdata);
                if (tracelength > IFR_MAX_TRACELENGTH/2)
                    s << "...";
                break;
            case IFR_HOSTTYPE_INT1:
                s << (IFR_Int4) asInt1(offset, rowsize, parameterdata);
                break;
            case IFR_HOSTTYPE_UINT1:
                s << (IFR_UInt4) asUInt1(offset, rowsize, parameterdata);
                break;
            case IFR_HOSTTYPE_INT2:
                s << asInt2(offset, rowsize, parameterdata);
                break;
            case IFR_HOSTTYPE_UINT2:
                s << asUInt2(offset, rowsize, parameterdata);
                break;
            case IFR_HOSTTYPE_INT4:
                s << asInt4(offset, rowsize, parameterdata);
                break;
            case IFR_HOSTTYPE_UINT4:
                s << asUInt4(offset, rowsize, parameterdata);
                break;
            case IFR_HOSTTYPE_INT8:
                s << asInt8(offset, rowsize, parameterdata);
                break;
            case IFR_HOSTTYPE_UINT8:
                s << asUInt8(offset, rowsize, parameterdata);
                break;
            case IFR_HOSTTYPE_DOUBLE:
                s << asDouble(offset, rowsize, parameterdata);
                break;
            case IFR_HOSTTYPE_FLOAT: {
                double d = asFloat(offset, rowsize, parameterdata);
                s << d;
                break;
            }
            case IFR_HOSTTYPE_ODBCDATE:
                s << asSQLDate(offset, rowsize, parameterdata);
                break;
            case IFR_HOSTTYPE_ODBCTIME:
                s << asSQLTime(offset, rowsize, parameterdata);
                break;
            case IFR_HOSTTYPE_ODBCTIMESTAMP:
                s << asSQLTimestamp(offset, rowsize, parameterdata);
                break;
            case IFR_HOSTTYPE_ODBCNUMERIC:
                s << asSQLNumeric(offset, rowsize, parameterdata);
                break;
            case IFR_HOSTTYPE_STREAM:
                trace_stream(s, (void *) data(offset, rowsize, parameterdata));
                break;
            case IFR_HOSTTYPE_BLOB:
            case IFR_HOSTTYPE_ASCII_LOB:
            case IFR_HOSTTYPE_UTF8_LOB:
            case IFR_HOSTTYPE_UCS2_LOB:
            case IFR_HOSTTYPE_UCS2_SWAPPED_LOB: {
                if(parameterdata && lengthindicator) {
                    // in this case we really have data for the parameter, we have to trace it
                    s << "*** DATA TRACE NOT IMPLEMENTED ***";
                } else {
                    IFR_LOB::sqlTrace(s, asLOBData(offset, rowsize));
                }
            }
                break;
            case IFR_HOSTTYPE_DECIMAL:
                if(!isinput && (m_digits == -1 && m_fraction==-1)) {
                    trace_decimal_digitsonly(s, (void *) data(offset, rowsize, parameterdata), used_lengthindicator);
                } else {
                    trace_decimal(s, (void *) data(offset, rowsize, parameterdata), m_digits, m_fraction);
                }
                break;
            case IFR_HOSTTYPE_OMS_PACKED_15_3:
                trace_decimal(s, (void *) data(offset, rowsize, parameterdata), 29, 3);
                break;
            case IFR_HOSTTYPE_OMS_PACKED_8_3:
                trace_decimal(s, (void *) data(offset, rowsize, parameterdata), 15, 3);
                break;
            case IFR_HOSTTYPE_OMS_TIMESTAMP:
                trace_decimal(s, (void *) data(offset, rowsize, parameterdata), 15, 0);
                break;
            default:
                s << "*** NOT IMPLEMENTED ***";
            }
        }
    }

    switch(conversionRC) {
    case IFR_DATA_TRUNC:
        s.print(" *** TRUNCATED ***");
        break;
    case IFR_OVERFLOW:
        s.print(" ***OVERFLOW ***");
    }

    return;
}



