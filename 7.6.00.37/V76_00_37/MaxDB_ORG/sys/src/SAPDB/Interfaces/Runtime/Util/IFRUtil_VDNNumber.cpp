/*!
  @file           IFRUtil_VDNNumber.cpp
  @author         D039759
  @ingroup        IFR_Util
  @brief          VDN Number conversions

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
#include "Interfaces/Runtime/Util/IFRUtil_VDNNumber.h"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_Alloca.h"
#include <stdio.h>
#include <ctype.h>

#include "hsp40.h"
#include "hsp41.h"

/*
 * due to a special feature of vdn numbers, we can check
 * with memcmp for limits. Thus, we need some integer limits
 * here.
 */

static unsigned char max_int8_vdnnumber[20] = { 0xd3, 0x92, 0x23, 0x37, 0x20, 0x36, 0x85, 0x47, 0x75, 0x80, 
                                                0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

static unsigned char min_int8_vdnnumber[20] = { 0x2d, 0x07, 0x76, 0x62, 0x79, 0x63, 0x14, 0x52, 0x24, 0x19, 
                                                0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

static unsigned char max_uint8_vdnnumber[20] = { 0xd4, 0x18, 0x44, 0x67, 0x44, 0x07, 0x37, 0x09, 0x55, 0x16, 
                                                 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
// 2147483647
static unsigned char max_int4_vdnnumber[20] = { 0xCA, 0x21, 0x47, 0x48, 0x36, 0x47, 0x00, 0x00, 0x00, 0x00, 
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
// -2147483648
static unsigned char min_int4_vdnnumber[20] = { 0x36, 0x78, 0x52, 0x51, 0x63, 0x52, 0x00, 0x00, 0x00, 0x00, 
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
// 0
static unsigned char zero_vdnnumber[20] = { 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
// 4294967295
static unsigned char max_uint4_vdnnumber[20] = { 0xca, 0x42, 0x94, 0x96, 0x72, 0x95, 0x00, 0x00, 0x00, 0x00, 
                                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
// -32768
static unsigned char min_int2_vdnnumber[20] = { 0x3b, 0x67, 0x23, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
// 32767
static unsigned char max_int2_vdnnumber[20] = { 0xc5, 0x32, 0x76, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
// 65535
static unsigned char max_uint2_vdnnumber[20] = { 0xc5, 0x65, 0x53, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
  
static unsigned char max_int1_vdnnumber[20] = { 0xc3, 0x12, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

static unsigned char min_int1_vdnnumber[20] = { 0x3d, 0x87, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };



static unsigned char max_uint1_vdnnumber[20] = { 0xc3, 0x25, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };


#define min_uint1_vdnnumber zero_vdnnumber
#define min_uint2_vdnnumber zero_vdnnumber
#define min_uint4_vdnnumber zero_vdnnumber
#define min_uint8_vdnnumber zero_vdnnumber

#define ZERO_EXPONENT 0x80

static inline void round_digits(IFR_Int4 *digits, 
                                IFR_Int4  round_length,
                                IFR_Int4  digit_length,
                                IFR_Int4& exponent)
{
    if(digits[round_length] >= 5) {
        for(IFR_Int4 i=round_length; i<digit_length; ++i) {
            digits[i] = 0;
        }
        IFR_Int4 k = round_length - 1;
        while(k >= 0) {
            digits[k]++;
            if(digits[k] == 10) {
                digits[k] = 0;
                --k;
                continue;
            }
            break;
        }
        if(k == -1) {
            digits[0]=1;
            ++exponent;
        }
    } else {
        for(IFR_Int4 i=round_length; i<digit_length; ++i) {
            digits[i] = 0;
        }
    }
}


//----------------------------------------------------------------------
// The individual conversions only differ in range checks,
// and return types. Thus there is a template for them.
template <class Integer> 
IFR_Retcode number_to_integervalue(unsigned char *number,       // source number
                                   Integer& result,             // destination number
                                   unsigned char *min_number,   // minimum valid value for the type
                                   unsigned char *max_number,   // maximum valid value for the type
                                   int iolength)                // iolength from shortinfo
{
    // comparision only with the iolength (length from 
    // short infos)
    if(memcmp(number, max_number, iolength-1) > 0) {
        return IFR_OVERFLOW;
    }
    if(memcmp(number, min_number, iolength-1) < 0) {
        return IFR_OVERFLOW;
    }
    
    if(number[0]==ZERO_EXPONENT) {
        result=0;
        return IFR_OK;
    }
    int exponent = (unsigned char) (number[0]);
    
    if(exponent < ZERO_EXPONENT) {
        exponent = - exponent + 64;
        if(exponent < 0) { // decimal places present
            result=0;
            return IFR_DATA_TRUNC;
        }
        
        if( (exponent+1)>>1 > iolength-2) {
            return IFR_NOT_OK;
        }

        result=0;
        for(int i=0; i<exponent; ++i) {
            unsigned int v = number[(i >> 1) + 1];
            if(i % 2 == 0) {
                v >>= 4;
            } else {
                v &= 0x0F;
            }
            result *=10;
            result += 9 - v; // tens complement
        }
        result = (Integer)-1 - result;
        // tens complement add one up
        // check that only zeros are following
        // and return IFR_DATA_TRUNC if this
        // is not the case
        for(int j=exponent+1; j<(iolength-2)*2; ++j) {
            unsigned int c=number[(j >> 1) + 1];
            if( j % 2 ) 
                c >>= 4;
            else 
                c &= 0xF;
            if(c)
                return IFR_DATA_TRUNC;
        }
        return IFR_OK;
    } else {
        exponent -= 192;
        if(exponent < 0) {
            result=0;
            return IFR_DATA_TRUNC;
        }
        
        // exponent is always < 38, 
        // because of the min/max check

        if( (exponent+1)>>1 > iolength-2) {
            return IFR_NOT_OK;
        }
        
        result=0;
        
        for(int i=0; i<exponent; ++i) {
            unsigned int v=number[(i >> 1) + 1];
            if(i % 2 == 0) {
                v >>= 4;
            } else {
                v &= 0x0F;
            }
            result *=10;
            result += v;
        }
        // check that only zeros are following
        // and return IFR_DATA_TRUNC if this
        // is not the case
        for(int j=exponent+1; j<(iolength-2)*2; ++j) {
            unsigned int c=number[(j >> 1) + 1];
            if( j % 2 ) 
                c >>= 4;
            else 
                c &= 0xF;
            if(c)
                return IFR_DATA_TRUNC;
        }
        return IFR_OK;
    }
}
    
    
//----------------------------------------------------------------------
IFR_Retcode
IFRUtil_VDNNumber::numberToInt4(unsigned char* number, IFR_Int4& result, int iolength)
{
    return number_to_integervalue(number, result, min_int4_vdnnumber, max_int4_vdnnumber, iolength);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRUtil_VDNNumber::numberToUInt4(unsigned char* number, IFR_UInt4& result, int iolength)
{
    return number_to_integervalue(number, result, zero_vdnnumber, max_uint4_vdnnumber, iolength);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRUtil_VDNNumber::numberToInt2(unsigned char* number, IFR_Int2& result, int iolength)
{
    return number_to_integervalue(number, result, min_int2_vdnnumber, max_int2_vdnnumber, iolength);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRUtil_VDNNumber::numberToUInt2(unsigned char* number, IFR_UInt2& result, int iolength)
{
    return number_to_integervalue(number, result, zero_vdnnumber, max_uint2_vdnnumber, iolength);
}


//----------------------------------------------------------------------
IFR_Retcode
IFRUtil_VDNNumber::numberToInt1(unsigned char* number, IFR_Int1& result, int iolength)
{
    return number_to_integervalue(number, result, min_int1_vdnnumber, max_int1_vdnnumber, iolength);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRUtil_VDNNumber::numberToUInt1(unsigned char* number, IFR_UInt1& result, int iolength)
{
    return number_to_integervalue(number, result, zero_vdnnumber, max_uint1_vdnnumber, iolength);
}


//----------------------------------------------------------------------
IFR_Retcode
IFRUtil_VDNNumber::numberToInt8(unsigned char* number, IFR_Int8& result, int iolength)
{
    return number_to_integervalue(number, result, min_int8_vdnnumber, max_int8_vdnnumber, iolength);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRUtil_VDNNumber::numberToUInt8(unsigned char* number, IFR_UInt8& result, int iolength)
{
    return number_to_integervalue(number, result, zero_vdnnumber, max_uint8_vdnnumber, iolength);
}

//----------------------------------------------------------------------
template <class Integer>
IFR_Retcode integer_to_number(Integer number, 
                              unsigned char *result, 
                              Integer min_number, 
                              unsigned char* min_vdn_number,
                              IFR_Int4 digits,
                              IFR_Bool isfloat)
{
    IFR_Int4 digits_bytelength=(digits+1)/2 + 1; // digits + exponent

    // simple case - zero
    if(number==0) {
        memcpy(result, zero_vdnnumber, digits_bytelength);
        return IFR_OK;
    }
    
    // if the number is the minumum number,
    // we cannot get the sign and make it positive, as 
    // two's complement is between 2**x-1 ... -2**x
    if(number == min_number) {
        if(digits<38 && min_vdn_number[digits_bytelength]!=0) {
            if(!isfloat) {
                return IFR_OVERFLOW;
            }
        }
        memcpy(result, min_vdn_number, digits_bytelength);
		return IFR_OK;		
    }


    
    int sgn=1;
    unsigned short zero = 0;
    if(number < zero) {
      sgn=0;
      number = (Integer)0 - number;
    }
    int exponent=0;
    unsigned char numvals[38];
    while(number != (Integer) 0) {
        unsigned int v = (unsigned int) (number % 10);
        numvals[exponent]=v;
        ++exponent;
        number /= 10;
    }
    if(!isfloat && (exponent > digits)) {
        return IFR_OVERFLOW;
    }
    memset(result, 0, digits_bytelength);
    // we now have the digits in numvals[0 ... exponent-1] 
    // they must be rotated and inserted.
    int idest=1;
    int isrc=exponent-1;
    int stopval=0;
    if(!sgn) {
        // If we are negative, we must obey the last zero 
        // as stop point for the 10's complement.
        while(stopval <= exponent-1 && numvals[stopval]==0) {
            ++stopval;
        }
        // Not all zeros, this can't happen here.
        if(stopval == exponent) {
            return IFR_NOT_OK;
        }
    }
    IFR_Retcode rc=IFR_OK;
    if(isfloat && (stopval < (exponent - digits))) {
        stopval = exponent - digits;
        rc = IFR_OK; // would be a truncation, but no interface wants
                     // it that way.
    }
    while(isrc>=stopval) {
        if(sgn) {
            result[idest]=numvals[isrc] << 4;
        } else {
            if(isrc==stopval) 
                result[idest]=(10 - numvals[isrc]) << 4;
            else 
                result[idest]=(9 - numvals[isrc]) << 4;
            
        }
        if(isrc==stopval) {
            ++idest;
            break;
        }
        --isrc;
        if(sgn) {
            result[idest]|=numvals[isrc];
        } else {
            if(isrc==stopval) 
                result[idest]|=(10 - numvals[isrc]);
            else 
                result[idest]|=(9 - numvals[isrc]);
        }
        ++idest;
        --isrc;
    }
    if(sgn) { // > 0
        result[0]=exponent + 192;
    } else {
        result[0]=64 - exponent;
    }
    return rc;
}

//----------------------------------------------------------------------
IFR_Retcode IFRUtil_VDNNumber::int1ToNumber(IFR_Int1 number, 
                                            unsigned char *result,
                                            IFR_Int4 digits,
                                            IFR_Bool isfloat)
{
    return integer_to_number(number, 
                             result, 
                             MIN_IFR_INT1, 
                             min_int1_vdnnumber,
                             digits,
                             isfloat);
}

//----------------------------------------------------------------------
IFR_Retcode IFRUtil_VDNNumber::uint1ToNumber(IFR_UInt1 number, 
                                             unsigned char *result,
                                             IFR_Int4 digits,
                                             IFR_Bool isfloat)
{
    return integer_to_number(number, result, 
                             MIN_IFR_UINT1, 
                             min_uint1_vdnnumber,
                             digits,
                             isfloat);
}

//----------------------------------------------------------------------
IFR_Retcode IFRUtil_VDNNumber::int2ToNumber(IFR_Int2 number, 
                                            unsigned char *result,
                                            IFR_Int4 digits,
                                            IFR_Bool isfloat)
{
    return integer_to_number(number, result, 
                             MIN_IFR_INT2, 
                             min_int2_vdnnumber,
                             digits,
                             isfloat);
}

//----------------------------------------------------------------------
IFR_Retcode IFRUtil_VDNNumber::uint2ToNumber(IFR_UInt2 number,
                                             unsigned char *result,
                                             IFR_Int4 digits,
                                             IFR_Bool isfloat)
{
    return integer_to_number(number, result, 
                             MIN_IFR_UINT2, 
                             min_uint2_vdnnumber,
                             digits,
                             isfloat);
}

//----------------------------------------------------------------------
IFR_Retcode IFRUtil_VDNNumber::int4ToNumber(IFR_Int4 number, 
                                            unsigned char *result,
                                            IFR_Int4 digits,
                                            IFR_Bool isfloat)
{
    return integer_to_number(number, result, 
                             MIN_IFR_INT4, 
                             min_int4_vdnnumber,
                             digits,
                             isfloat);
}

//----------------------------------------------------------------------
IFR_Retcode IFRUtil_VDNNumber::uint4ToNumber(IFR_UInt4 number, 
                                             unsigned char *result,
                                             IFR_Int4 digits,
                                             IFR_Bool isfloat)
{
    return integer_to_number(number, result, 
                             MIN_IFR_UINT4, 
                             min_uint4_vdnnumber,
                             digits,
                             isfloat);
}

//----------------------------------------------------------------------
IFR_Retcode IFRUtil_VDNNumber::int8ToNumber(IFR_Int8 number, 
                                            unsigned char *result,
                                            IFR_Int4 digits,
                                            IFR_Bool isfloat)
{
    return integer_to_number(number, 
                             result, 
                             MIN_IFR_INT8, 
                             min_int8_vdnnumber,
                             digits,
                             isfloat);
}

//----------------------------------------------------------------------
IFR_Retcode IFRUtil_VDNNumber::uint8ToNumber(IFR_UInt8 number, 
                                             unsigned char *result,
                                             IFR_Int4 digits,
                                             IFR_Bool isfloat)
{
    return integer_to_number(number, result, 
                             MIN_IFR_UINT8, 
                             min_uint8_vdnnumber,
                             digits,
                             isfloat);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRUtil_VDNNumber::isSmallInteger(unsigned char *number, IFR_Int4 length)
{
    if((memcmp(number, max_int2_vdnnumber, 
              length > sizeof(max_int2_vdnnumber) ? sizeof(max_int2_vdnnumber) : length) > 0)
       ||
       (memcmp(number, min_int2_vdnnumber,
               length > sizeof(min_int2_vdnnumber) ? sizeof(min_int2_vdnnumber) : length) < 0)) {
        return IFR_OVERFLOW;
    } else {
        return IFR_OK;
    }
}

//----------------------------------------------------------------------
IFR_Retcode
IFRUtil_VDNNumber::isInteger(unsigned char *number, IFR_Int4 length)
{
    if((memcmp(number, max_int4_vdnnumber, 
              length > sizeof(max_int4_vdnnumber) ? sizeof(max_int4_vdnnumber) : length) > 0)
       ||
       (memcmp(number, min_int4_vdnnumber,
               length > sizeof(min_int4_vdnnumber) ? sizeof(min_int4_vdnnumber) : length) < 0)) {
        return IFR_OVERFLOW;
    } else {
        return IFR_OK;
    }
}

//----------------------------------------------------------------------
IFR_Retcode
IFRUtil_VDNNumber::checkVDNNumber(unsigned char* number, int iolength)
{
    int exponent = *number;
    
    if(exponent == ZERO_EXPONENT) {
        return IFR_OK;
    } else {
        if(exponent > ZERO_EXPONENT) {
            exponent -= 0xC0;
        } else {
            exponent = - exponent + 64;            
        }
        if(exponent > 0) {
            if(exponent > 38) {
                exponent=38;
            }
            if( (exponent+1)>>1 > iolength-2) {
                return IFR_OVERFLOW;
            }
        } 
        
        for(int j=(iolength-2)*2 + 1; j<38; ++j) {
            unsigned int c=number[(j >> 1) + 1];
            if( j % 2 ) 
                c >>= 4;
            else 
                c &= 0xF;
            if(c) {
                return IFR_DATA_TRUNC;
            }
        }
    }
    return IFR_OK;
}

                                  
//----------------------------------------------------------------------
IFR_Retcode
IFRUtil_VDNNumber::numberToDouble(unsigned char *number,
                                  double& result,
                                  int     len)
{
    char res;
    s40glrel((void *)number, 1, len, result, (tsp00_NumError &)res);
    switch(res) {
    case (num_ok):
    case (num_trunc):
        return IFR_OK;
    case (num_overflow):
        return IFR_OVERFLOW;
    default:
        return IFR_NOT_OK;
    }
}

//----------------------------------------------------------------------
IFR_Retcode
IFRUtil_VDNNumber::numberToDecimal(unsigned char *number,
                                   int length,
                                   unsigned char *decimal,
                                   int decimalDigits,
                                   int decimalFraction)
{
    char res;
    s40gdec(number, 1, length, *((tsp00_Decimal *)decimal), decimalDigits, decimalFraction, (tsp00_NumError &)res);
    switch(res) {
    case (num_ok):
        return IFR_OK;
    case (num_trunc):
        return IFR_DATA_TRUNC;
    case (num_overflow):
        return IFR_OVERFLOW;
    default:
        return IFR_NOT_OK;
    }
}


//----------------------------------------------------------------------
IFR_Retcode
IFRUtil_VDNNumber::doubleToNumber(double number,
                                  unsigned char* result,
                                  int length,
                                  int precision)
    
{
     // catch the 'not a number' values
     if(ISNAN(number) || ISINF(number)) {
         return IFR_OVERFLOW;
     }
    
    
     // catch 0.0
     // values not representable are rounded without warning
     // to zero
     // this value is not dependent on the precision, as the
     // VDN Number is normalized, thus having .1e-63 as the 
     // minimum value
     if(fabs(number) < 0.5e-64) {
         memcpy(result, zero_vdnnumber, (length+1)/2 + 1);
     return IFR_OK;
     }
     
     // ok, go the old way.
     char res;
     s41plrel((char *)result, 1, length, precision, number, (tsp00_NumError&)res);
     switch(res) {
     case (num_ok):
         return IFR_OK;
     case (num_overflow):
         return IFR_OVERFLOW;
     case (num_trunc):
         return IFR_DATA_TRUNC;
     default:
         return IFR_NOT_OK;
     }
}

//----------------------------------------------------------------------
IFR_Retcode
IFRUtil_VDNNumber::decimalToNumber(unsigned char *decimal,
                                   int decimalLength,
                                   int decimalPrecision,
                                   unsigned char *result,
                                   int length,
                                   int precision)
{
    char res;
    s41pdec(result, 1, length, precision, *((tsp00_Decimal *)decimal), decimalLength, decimalPrecision, (tsp00_NumError &)res);
    switch(res) {
    case num_ok:
        return IFR_OK;
    case num_trunc:
        return IFR_DATA_TRUNC;
    case num_overflow:
        return IFR_OVERFLOW;
    default:
        return IFR_NOT_OK;
    }
}



//----------------------------------------------------------------------
IFR_Retcode
IFRUtil_VDNNumber::numberToString(unsigned char *number,
                                  char *buffer, 
                                  IFR_size_t bufferlength,
                                  IFR_StringEncoding encoding,
                                  IFR_Bool fixed,
                                  IFR_Int4 length,
                                  IFR_Int4 precision)
{
    if(encoding == IFR_StringEncodingAscii || encoding == IFR_StringEncodingUTF8) {
        return numberToStringAscii(number,
                                   buffer,
                                   bufferlength,
                                   fixed,
                                   length,
                                   precision);
    } else if(encoding==IFR_StringEncodingUCS2) {
        return numberToStringUCS2(number,
                                   buffer,
                                   bufferlength,
                                   fixed,
                                   length,
                                   precision);
    
    } else if(encoding==IFR_StringEncodingUCS2Swapped) {
        return numberToStringUCS2Swapped(number,
                                         buffer,
                                         bufferlength,
                                         fixed,
                                         length,
                                         precision);
        
    } else {
        return IFR_NOT_OK;
    }
}


//----------------------------------------------------------------------
static inline void 
number_get_digits(unsigned char *number,      // the number
                  char *digits,               // the destination digits
                  IFR_Int4& exponent,         // the exponent (out)
                  IFR_Bool& isnegative,       // negative? (out)
                  IFR_Int4& numberlength,     // length of number (out)   
                  IFR_Int4  length)           // shortinfo length of number
    
{
    numberlength=0;
    exponent=number[0];
    if(exponent < ZERO_EXPONENT) {
        isnegative = true;
        exponent = - exponent + 64;
        for(int i=0; i<length; ++i) {
            int v;
             if(i % 2 == 0) {
                 v = (number[1 + i/2] & 0xFF) >> 4;
             } else {
                 v = (number[1 + i/2] & 0xF);
             }
             if(v) {
                 numberlength=i;
            }
            digits[i] = (char) ((9 - v) + '0');
        }
        ++digits[numberlength];
    } else {
        isnegative=false;
        exponent -= 192;
        for(int i=0; i<length; ++i) {
            int v;
            if(i % 2 == 0) {
                v = (number[1 + i/2] & 0xFF) >> 4;
            } else {
                v = (number[1 + i/2] & 0xF);
            }
            if(v) {
                numberlength=i;
            }
            digits[i] = (char) (v + '0');
        }
    }
    numberlength++;
}


//----------------------------------------------------------------------
IFR_Retcode
IFRUtil_VDNNumber::numberToStringAscii(unsigned char *number,
                                       char *buffer,
                                       IFR_size_t bufferlength,
                                       IFR_Bool fixed,
                                       IFR_Int4 length,
                                       IFR_Int4 precision)
{
    // avoid stupid things
    if(bufferlength<1) {
        return IFR_NOT_OK;
    } else if(bufferlength==1) {
        *buffer='\0';
        return IFR_DATA_TRUNC;
    }
    
    char digits[38];
    IFR_Bool isnegative;
    IFR_Int4 numberlength;
    IFR_Int4 exponent;
    if(number[0]==ZERO_EXPONENT) {
        *buffer='0';
        if(fixed && precision > 0) {
            IFR_Int4 filllength = precision;
            if(filllength > IFR_Int4(bufferlength - 3)) {
                filllength = (IFR_Int4)(bufferlength - 3);
            }
            *(buffer + 1) = '.';
            memset(buffer + 2, '0', filllength);
            *(buffer + 2 + filllength) = '\0';
            return (IFR_size_t)precision > bufferlength - 3 ? IFR_DATA_TRUNC : IFR_OK;
        }
        *(buffer+1)='\0';
        return IFR_OK;
    }

    number_get_digits(number, digits, exponent, isnegative, numberlength, length);
    char *bufferend=buffer+bufferlength-1;
    char *p=buffer;
    
        
    if(isnegative) {
        *p='-';
        if(++p >= bufferend) {
            *bufferend='\0';
            return IFR_DATA_TRUNC;
        }
    }

    int i;
    // fprintf(stderr, "Number is %s\n", fixed?"fixed":"not fixed"); 
    if(fixed || (!fixed && (exponent>-3) && (exponent<7))) {
        // and now a lot and difficult cases.
        
        // if the exponent is <=0, this is a '0.xxx' number
        if(exponent <= 0) {
            // this must start with '0.'
            *p='0';
            if(++p >= bufferend) {
                *bufferend='\0';
                return IFR_DATA_TRUNC;
            }
            *p='.';
            if(++p >= bufferend) {
                *bufferend='\0';
                return IFR_DATA_TRUNC;
            }
            // we must keep track of the digits after the comma,
            // as for fixed type we will add as much as this value
            // to the value.
            int postcomma_digits=-exponent;
            // now add as manys zeros as the negative exponent has ...
            for(i=0; i < (-exponent); ++i) {
                *p='0';
                if(++p >= bufferend) {
                    *bufferend='\0';
                    return IFR_DATA_TRUNC;
                }
            }

            postcomma_digits += numberlength;
            for(i=0; i<numberlength; ++i) {
                *p=digits[i];
                if(++p >= bufferend) {
                    *bufferend='\0';
                    return IFR_DATA_TRUNC;
                }
            }
            for(i=postcomma_digits; i<precision; ++i) {
                *p='0';
                if(++p >= bufferend) {
                    *bufferend='\0';
                    return IFR_DATA_TRUNC;
                }
            }
        } else {
            // the exponent is > 0 
            // this means we have a part before and after the comma.
            
            // now to the part before the comma.
            for(i=0; i<exponent; ++i) {
                // if we have still stuff in the number,
                // we can add this ... otherwise we have
                // to use the 
                if(i<numberlength) {
                    *p = digits[i];
                } else {
                    *p = '0';
                }
                if(++p >= bufferend) {
                    *bufferend='\0';
                    return IFR_DATA_TRUNC;
                }
            }
            
            // now after the comma.
            
            // if precision is != 0 we have to add the post-comma-digits
            if(precision!=0) {
                *p = '.';
                if(++p >= bufferend) {
                    *bufferend='\0';
                    return IFR_DATA_TRUNC;
                }
                for(i=exponent; i<exponent+precision; ++i) {
                    if(i<numberlength) {
                        *p = digits[i];
                    } else {
                        *p = '0';
                    }
                    if(++p >= bufferend) {
                        *bufferend='\0';
                        return IFR_DATA_TRUNC;
                    }
                }
            } else if(exponent < numberlength) {  
                // also if we have still significant numbers remaining
                *p = '.';
                if(++p >= bufferend) {
                    *bufferend='\0';
                    return IFR_DATA_TRUNC;
                }
                for(i=exponent; i<numberlength; ++i) {
                    *p = digits[i];
                    if(++p >= bufferend) {
                        *bufferend='\0';
                        return IFR_DATA_TRUNC;
                    }
                }
            }
        }
        *p='\0';
        return IFR_OK;
    } else {
        // we will display the number in 'scientific' manner.
        exponent--; // we will use the a.bbbb format not the '0.abbbb' format
        *p=digits[0];
        if(++p >= bufferend) {
            *bufferend='\0';
            return IFR_DATA_TRUNC;
        }
        // check if we must add a decimal point
        if(numberlength > 1) {
            *p='.';
            if(++p >= bufferend) {
                *bufferend='\0';
                return IFR_DATA_TRUNC;
            }
        } 
        // add the remaining numbers
        for(i=1; i<numberlength; ++i) {
            *p=digits[i];
            if(++p >= bufferend) {
                *bufferend='\0';
                return IFR_DATA_TRUNC;
            }
        } 
        // add the 'E'
        *p='E';
        if(++p >= bufferend) {
            *bufferend='\0';
            return IFR_DATA_TRUNC;
        }
        // minus sign for exponent
        if(exponent < 0) {
            *p='-';
            if(++p >= bufferend) {
                *bufferend='\0';
                return IFR_DATA_TRUNC;
            }
            exponent = -exponent; 
        }
        // add exponent digits
        if(exponent > 10) {
            *p= exponent / 10 + '0';
            if(++p >= bufferend) {
                *bufferend='\0';
                return IFR_DATA_TRUNC;
            }
            *p= exponent % 10 + '0';
        } else {
            *p= exponent + '0';
        }
        ++p;
        *p='\0';
        
        return IFR_OK;
    }
}

//----------------------------------------------------------------------
IFR_Retcode
IFRUtil_VDNNumber::numberToStringUCS2(unsigned char *number,
                                      char *buffer,
                                      IFR_size_t bufferlength,
                                      IFR_Bool fixed,
                                      IFR_Int4 length,
                                      IFR_Int4 precision)
{
    // avoid stupid things
    if(bufferlength<2) {
        return IFR_NOT_OK;
    } else if(bufferlength<4) {
        *buffer='\0';
        *(buffer+1)='\0';
        return IFR_DATA_TRUNC;
    }
    
    char digits[38];
    IFR_Bool isnegative;
    IFR_Int4 numberlength;
    IFR_Int4 exponent;
    if(number[0]==ZERO_EXPONENT) {
        memcpy(buffer, "\0000\0\0", 4);
        return IFR_OK;
    }

    number_get_digits(number, digits, exponent, isnegative, numberlength, length);
    memset(buffer, 0, bufferlength);
    char *bufferend=buffer+bufferlength-2;
    char *p=buffer+1;
    
        
    if(isnegative) {
        *p='-';
        if((p+=2) >= bufferend) {
            *bufferend='\0';*(bufferend+1)='\0';
            return IFR_DATA_TRUNC;
        }
    }

    int i;
    // fprintf(stderr, "Number is %s\n", fixed?"fixed":"not fixed"); 
    if(fixed || (!fixed && (exponent>-3) && (exponent<7))) {
        // and now a lot and difficult cases.
        
        // if the exponent is <=0, this is a '0.xxx' number
        if(exponent <= 0) {
            // this must start with '0.'
            *p='0';
            if((p+=2) >= bufferend) {
                *bufferend='\0';*(bufferend+1)='\0';
                return IFR_DATA_TRUNC;
            }
            *p='.';
            if((p+=2) >= bufferend) {
                *bufferend='\0';*(bufferend+1)='\0';
                return IFR_DATA_TRUNC;
            }
            // we must keep track of the digits after the comma,
            // as for fixed type we will add as much as this value
            // to the value.
            int postcomma_digits=-exponent;
            // now add as manys zeros as the negative exponent has ...
            for(i=0; i < (-exponent); ++i) {
                *p='0';
                if((p+=2) >= bufferend) {
                    *bufferend='\0';*(bufferend+1)='\0';
                    return IFR_DATA_TRUNC;
                }
            }

            postcomma_digits += numberlength;
            for(i=0; i<numberlength; ++i) {
                *p=digits[i];
                if((p+=2) >= bufferend) {
                    *bufferend='\0';*(bufferend+1)='\0';
                    return IFR_DATA_TRUNC;
                }
            }
            for(i=postcomma_digits; i<precision; ++i) {
                *p='0';
                if((p+=2) >= bufferend) {
                    *bufferend='\0';*(bufferend+1)='\0';
                    return IFR_DATA_TRUNC;
                }
            }
        } else {
            // the exponent is > 0 
            // this means we have a part before and after the comma.
            
            // now to the part before the comma.
            for(i=0; i<exponent; ++i) {
                // if we have still stuff in the number,
                // we can add this ... otherwise we have
                // to use the 
                if(i<numberlength) {
                    *p = digits[i];
                } else {
                    *p = '0';
                }
                if((p+=2) >= bufferend) {
                    *bufferend='\0';*(bufferend+1)='\0';
                    return IFR_DATA_TRUNC;
                }
            }
            
            // now after the comma.
            
            // if precision is != 0 we have to add the post-comma-digits
            if(precision!=0) {
                *p = '.';
                if((p+=2) >= bufferend) {
                    *bufferend='\0';*(bufferend+1)='\0';
                    return IFR_DATA_TRUNC;
                }
                for(i=exponent; i<exponent+precision; ++i) {
                    if(i<numberlength) {
                        *p = digits[i];
                    } else {
                        *p = '0';
                    }
                    if((p+=2) >= bufferend) {
                        *bufferend='\0';*(bufferend+1)='\0';
                        return IFR_DATA_TRUNC;
                    }
                }
            } else if(exponent < numberlength) {  
                // also if we have still significant numbers remaining
                *p = '.';
                if((p+=2) >= bufferend) {
                    *bufferend='\0';*(bufferend+1)='\0';
                    return IFR_DATA_TRUNC;
                }
                for(i=exponent; i<numberlength; ++i) {
                    *p = digits[i];
                    if((p+=2) >= bufferend) {
                        *bufferend='\0';*(bufferend+1)='\0';
                        return IFR_DATA_TRUNC;
                    }
                }
            }
        }
        *p='\0'; *(p+1)='\0';
        return IFR_OK;
    } else {
        // we will display the number in 'scientific' manner.
        exponent--; // we will use the a.bbbb format not the '0.abbbb' format
        *p=digits[0];
        if((p+=2) >= bufferend) {
            *bufferend='\0';*(bufferend+1)='\0';
            return IFR_DATA_TRUNC;
        }
        // check if we must add a decimal point
        if(numberlength > 1) {
            *p='.';
            if((p+=2) >= bufferend) {
                *bufferend='\0';*(bufferend+1)='\0';
                return IFR_DATA_TRUNC;
            }
        } 
        // add the remaining numbers
        for(i=1; i<numberlength; ++i) {
            *p=digits[i];
            if((p+=2) >= bufferend) {
                *bufferend='\0';*(bufferend+1)='\0';
                return IFR_DATA_TRUNC;
            }
        } 
        // add the 'E'
        *p='E';
        if((p+=2) >= bufferend) {
            *bufferend='\0';*(bufferend+1)='\0';
            return IFR_DATA_TRUNC;
        }
        // minus sign for exponent
        if(exponent < 0) {
            *p='-';
            if((p+=2) >= bufferend) {
                *bufferend='\0';*(bufferend+1)='\0';
                return IFR_DATA_TRUNC;
            }
            exponent = -exponent; 
        }
        // add exponent digits
        if(exponent > 10) {
            *p= exponent / 10 + '0';
            if((p+=2) >= bufferend) {
                *bufferend='\0';*(bufferend+1)='\0';
                return IFR_DATA_TRUNC;
            }
            *p= exponent % 10 + '0';
        } else {
            *p= exponent + '0';
        }
        (p+=2);
        *p='\0'; *(p+1)='\0';
        
        return IFR_OK;
    }
}

//----------------------------------------------------------------------
IFR_Retcode
IFRUtil_VDNNumber::numberToStringUCS2Swapped(unsigned char *number,
                                             char *buffer,
                                             IFR_size_t bufferlength,
                                             IFR_Bool fixed,
                                             IFR_Int4 length,
                                             IFR_Int4 precision)
{
    // avoid stupid things
    if(bufferlength<2) {
        return IFR_NOT_OK;
    } else if(bufferlength<4) {
        *buffer='\0';
        *(buffer+1)='\0';
        return IFR_DATA_TRUNC;
    }
    
    char digits[38];
    IFR_Bool isnegative;
    IFR_Int4 numberlength;
    IFR_Int4 exponent;
    if(number[0]==ZERO_EXPONENT) {
        memcpy(buffer, "0\0\0\0", 4);
        return IFR_OK;
    }

    number_get_digits(number, digits, exponent, isnegative, numberlength, length);
    memset(buffer, 0, bufferlength);
    char *bufferend=buffer+bufferlength-2;
    char *p=buffer;
    
        
    if(isnegative) {
        *p='-';
        if((p+=2) >= bufferend) {
            *bufferend='\0';*(bufferend+1)='\0';
            return IFR_DATA_TRUNC;
        }
    }

    int i;
    // fprintf(stderr, "Number is %s\n", fixed?"fixed":"not fixed"); 
    if(fixed || (!fixed && (exponent>-3) && (exponent<7))) {
        // and now a lot and difficult cases.
        
        // if the exponent is <=0, this is a '0.xxx' number
        if(exponent <= 0) {
            // this must start with '0.'
            *p='0';
            if((p+=2) >= bufferend) {
                *bufferend='\0';*(bufferend+1)='\0';
                return IFR_DATA_TRUNC;
            }
            *p='.';
            if((p+=2) >= bufferend) {
                *bufferend='\0';*(bufferend+1)='\0';
                return IFR_DATA_TRUNC;
            }
            // we must keep track of the digits after the comma,
            // as for fixed type we will add as much as this value
            // to the value.
            int postcomma_digits=-exponent;
            // now add as manys zeros as the negative exponent has ...
            for(i=0; i < (-exponent); ++i) {
                *p='0';
                if((p+=2) >= bufferend) {
                    *bufferend='\0';*(bufferend+1)='\0';
                    return IFR_DATA_TRUNC;
                }
            }

            postcomma_digits += numberlength;
            for(i=0; i<numberlength; ++i) {
                *p=digits[i];
                if((p+=2) >= bufferend) {
                    *bufferend='\0';*(bufferend+1)='\0';
                    return IFR_DATA_TRUNC;
                }
            }
            for(i=postcomma_digits; i<precision; ++i) {
                *p='0';
                if((p+=2) >= bufferend) {
                    *bufferend='\0';*(bufferend+1)='\0';
                    return IFR_DATA_TRUNC;
                }
            }
        } else {
            // the exponent is > 0 
            // this means we have a part before and after the comma.
            
            // now to the part before the comma.
            for(i=0; i<exponent; ++i) {
                // if we have still stuff in the number,
                // we can add this ... otherwise we have
                // to use the 
                if(i<numberlength) {
                    *p = digits[i];
                } else {
                    *p = '0';
                }
                if((p+=2) >= bufferend) {
                    *bufferend='\0';*(bufferend+1)='\0';
                    return IFR_DATA_TRUNC;
                }
            }
            
            // now after the comma.
            
            // if precision is != 0 we have to add the post-comma-digits
            if(precision!=0) {
                *p = '.';
                if((p+=2) >= bufferend) {
                    *bufferend='\0';*(bufferend+1)='\0';
                    return IFR_DATA_TRUNC;
                }
                for(i=exponent; i<exponent+precision; ++i) {
                    if(i<numberlength) {
                        *p = digits[i];
                    } else {
                        *p = '0';
                    }
                    if((p+=2) >= bufferend) {
                        *bufferend='\0';*(bufferend+1)='\0';
                        return IFR_DATA_TRUNC;
                    }
                }
            } else if(exponent < numberlength) {  
                // also if we have still significant numbers remaining
                *p = '.';
                if((p+=2) >= bufferend) {
                    *bufferend='\0';*(bufferend+1)='\0';
                    return IFR_DATA_TRUNC;
                }
                for(i=exponent; i<numberlength; ++i) {
                    *p = digits[i];
                    if((p+=2) >= bufferend) {
                        *bufferend='\0';*(bufferend+1)='\0';
                        return IFR_DATA_TRUNC;
                    }
                }
            }
        }
        *p='\0'; *(p+1)='\0';
        return IFR_OK;
    } else {
        // we will display the number in 'scientific' manner.
        exponent--; // we will use the a.bbbb format not the '0.abbbb' format
        *p=digits[0];
        if((p+=2) >= bufferend) {
            *bufferend='\0';*(bufferend+1)='\0';
            return IFR_DATA_TRUNC;
        }
        // check if we must add a decimal point
        if(numberlength > 1) {
            *p='.';
            if((p+=2) >= bufferend) {
                *bufferend='\0';*(bufferend+1)='\0';
                return IFR_DATA_TRUNC;
            }
        } 
        // add the remaining numbers
        for(i=1; i<numberlength; ++i) {
            *p=digits[i];
            if((p+=2) >= bufferend) {
                *bufferend='\0';*(bufferend+1)='\0';
                return IFR_DATA_TRUNC;
            }
        } 
        // add the 'E'
        *p='E';
        if((p+=2) >= bufferend) {
            *bufferend='\0';*(bufferend+1)='\0';
            return IFR_DATA_TRUNC;
        }
        // minus sign for exponent
        if(exponent < 0) {
            *p='-';
            if((p+=2) >= bufferend) {
                *bufferend='\0';*(bufferend+1)='\0';
                return IFR_DATA_TRUNC;
            }
            exponent = -exponent; 
        }
        // add exponent digits
        if(exponent > 10) {
            *p= exponent / 10 + '0';
            if((p+=2) >= bufferend) {
                *bufferend='\0';*(bufferend+1)='\0';
                return IFR_DATA_TRUNC;
            }
            *p= exponent % 10 + '0';
        } else {
            *p= exponent + '0';
        }
        (p+=2);
        *p='\0'; *(p+1)='\0';
        
        return IFR_OK;
    }
}



//----------------------------------------------------------------------
IFR_Retcode
IFRUtil_VDNNumber::stringToNumber(char              *buffer,
                                  IFR_size_t         bufferlength,
                                  IFR_StringEncoding encoding,
                                  IFR_Bool          &nan,
                                  unsigned char     *number,
                                  IFR_Bool           fixed,
                                  IFR_Int4           length,
                                  IFR_Int4           precision)
{
    if(encoding == IFR_StringEncodingAscii || encoding == IFR_StringEncodingUTF8) {
        return (stringToNumberAscii(buffer, bufferlength, nan, number, fixed, length, precision));
    } else if(encoding == IFR_StringEncodingUCS2) {
        if(bufferlength % 2) {
            return IFR_NOT_OK;
        }
        IFR_size_t tmplen=bufferlength / 2;
        char *tmpbuffer = (char *)alloca(tmplen);
        for(IFR_size_t i=0; i<tmplen; ++i) {
            tmpbuffer[i] = buffer[i*2 +1];
            if(buffer[i*2]) {
                return IFR_NOT_OK;
            }
        }
        return stringToNumberAscii(tmpbuffer, tmplen, nan, number, fixed, length, precision);
    } else if(encoding == IFR_StringEncodingUCS2Swapped) {
        if(bufferlength % 2) {
            return IFR_NOT_OK;
        }
        IFR_size_t tmplen=bufferlength / 2;
        char *tmpbuffer = (char *)alloca(tmplen);
        for(IFR_size_t i=0; i<tmplen; ++i) {
            tmpbuffer[i] = buffer[i*2];
            if(buffer[i*2 + 1]) {
                return IFR_NOT_OK;
            }
        }
        return stringToNumberAscii(tmpbuffer, tmplen, nan, number, fixed, length, precision);
    }
    return IFR_NOT_OK;
}

//----------------------------------------------------------------------
IFR_Retcode
IFRUtil_VDNNumber::stringToNumberAscii(char              *buffer,
                                       IFR_size_t         bufferlength,
                                       IFR_Bool          &nan,
                                       unsigned char     *number,
                                       IFR_Bool           fixed,
                                       IFR_Int4           length,
                                       IFR_Int4           precision)
{
#define CHECK_REMAINING if(remaining == 0) return IFR_NOT_OK
    
    if(bufferlength == 0) {
        return IFR_NOT_OK;
    }
    nan = false;
    
    IFR_size_t remaining=bufferlength;
    IFR_Bool   positive=true;
    
    //eat leading blanks
    for (;(remaining>0 && isspace(buffer[0]));buffer++, remaining--){};    
    //eat trailing blanks
    for (;(remaining>0 && isspace(buffer[remaining-1]));--remaining){};
    CHECK_REMAINING;
    
    if(*buffer == '-') {
        positive = false;
        ++buffer;
        --remaining;
        CHECK_REMAINING;
    } else if(*buffer == '+') {
        ++buffer;
        --remaining;
        CHECK_REMAINING;
    } else if(remaining>=3 &&
              (buffer[0]=='N' || buffer[0]=='n') &&
              (buffer[1]=='A' || buffer[1]=='a') &&
              (buffer[2]=='N' || buffer[2]=='n')) {
        nan=true;
        return IFR_NOT_OK;
    } 

    if(remaining >=3 &&
       (buffer[0]=='I' || buffer[0]=='i') &&
       (buffer[1]=='N' || buffer[1]=='n') &&
       (buffer[2]=='F' || buffer[2]=='f') ) {
        nan = true;
        return IFR_NOT_OK;
    }
    
    IFR_Int4 digits_bytelength=(length + 1)/2 + 1; // digits + exponent
    
    if ((*buffer) == '0') {
        if (remaining > 1) {
            do {
                ++buffer;
                --remaining;
                if ((*buffer) != '0')
                    break;
            } while (remaining);        }
        // We have a zero.
        if(!remaining) {
            memcpy(number, zero_vdnnumber, digits_bytelength);
            return IFR_OK;
        }
    }
    
    IFR_Retcode rc=IFR_OK;
    IFR_Int4 digits[39];
    IFR_Int4  num_digits=0;
    IFR_Int4  frac_digits=0;
    IFR_Int4  exponent=0;
    IFR_Bool  frac_part=false;
    IFR_Bool  frac_significant = false;
    IFR_Int4  frac_zeros=0;

    do {
        if(*buffer == '.') {
            frac_part = true;
        } else  if((*buffer)>='0' && (*buffer)<='9') {
            if(!frac_part) {
                frac_significant = true; // after having a number before the '.', the
                // fractional part is always significant.
                if(num_digits >= 39) {
                    if(frac_part) {
                        rc = IFR_DATA_TRUNC;
                    } else {
                        return IFR_OVERFLOW;
                    }
                } else {
                    digits[num_digits] = *buffer - '0';
                    ++num_digits;
                    if(num_digits == 39) { 
                        return IFR_OVERFLOW; 
                    }
                    ++exponent;
                }
            } else {
                if(*buffer != '0' || frac_significant) {
                    frac_significant = true;
                    if(num_digits >= 39) {
                        if(frac_part) {
                            rc = IFR_DATA_TRUNC;
                        } else {
                            return IFR_OVERFLOW;
                        }
                    } else {
                        digits[num_digits] = *buffer - '0';
                        ++num_digits;
                        ++frac_digits;
                    }
                } else {
                    --exponent;
                    ++frac_zeros;
                    if(remaining==1) {
                        memcpy(number, zero_vdnnumber, digits_bytelength);
                        return IFR_OK;
                    }
                }
            }
        } else { 
            break;
        }
        ++buffer;
        --remaining;
    } while(remaining);
    
    if(num_digits == 0) {
        return IFR_NOT_OK;
    } 

    // possible exponent
    if(remaining) {
        if(*buffer=='E' || *buffer=='e') {
            ++buffer;
            --remaining;
            CHECK_REMAINING;
        }
        IFR_Int4 exponent_sign=1;
        IFR_Int4 explicit_exponent=0;
        if(*buffer ==  '+') {
            ++buffer;
            --remaining;
            CHECK_REMAINING;
        } else if(*buffer == '-') {
            exponent_sign = -1;
            ++buffer;
            --remaining;
            CHECK_REMAINING;
        }
        do {
            if((*buffer >= '0') && (*buffer <= '9')) {
                explicit_exponent *= 10;
                explicit_exponent += (*buffer - '0');
                if( abs((explicit_exponent * exponent_sign) + exponent) > 64) {
                    return IFR_OVERFLOW;
                }
            } else {
                break;
            }
            ++buffer;
            --remaining;
        } while(remaining);

        // trailing garbage
        if(remaining) {
            return IFR_NOT_OK;
        }
        // add the exponent to the exponent computed by the comma position
        exponent = explicit_exponent * exponent_sign + exponent;
    }
    
    // ok, now we have to find out, whether this is an overflow or truncated, or just ok.
    if(fixed) {
        if( (num_digits - frac_digits) > (length - precision) ) {
            return IFR_OVERFLOW;
        }
        if((frac_digits + frac_zeros) > precision) {
            rc = IFR_DATA_TRUNC;
        }
    }
    
    if(num_digits > length) {
        round_digits(digits, length, num_digits, exponent);
    }
    
    IFR_Int4 lastnonzero=0;
    for(IFR_Int4 k=0; k<num_digits; ++k) {
        if(digits[k]) {
            lastnonzero = k;
        }
    }
    
    if((exponent > 63) || (exponent < -64)) {
        return  IFR_OVERFLOW;
    }
    // and now insert it !

    if(lastnonzero == 0 && digits[0]==0) {
        memcpy(number, zero_vdnnumber, digits_bytelength);
        return IFR_OK;
    }
    memset(number, 0, digits_bytelength);
    if(positive) {
        number[0]=exponent + 192;
    } else {
        number[0]=64 - exponent;
    }
    IFR_Int4 idest=1;
    for(IFR_Int4 l=0; l<=lastnonzero; ++l) {
        if(positive) {
            if(!(l % 2)) {
                number[idest] = digits[l] << 4;
            } else {
                number[idest] |= digits[l];
                ++idest;
            }
        } else {
            if(!(l % 2)) {
                if(l == lastnonzero) {
                    number[idest] = (10 - digits[l]) << 4;
                } else {
                    number[idest] = (9 - digits[l]) << 4;
                }
            } else {
                if(l == lastnonzero) {
                    number[idest] |= (10 - digits[l]);
                } else {
                    number[idest] |= (9 - digits[l]);
                }
                ++idest;
            }
        }
    }
    return rc;
}

