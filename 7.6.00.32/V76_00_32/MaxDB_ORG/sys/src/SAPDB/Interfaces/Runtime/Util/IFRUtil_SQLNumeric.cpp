/*!
  @file           IFRUtil_SQLNumeric.cpp
  @author         D039759
  @ingroup        IFR_Util
  @brief          SQL Numeric datatype handling.


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
#include "Interfaces/Runtime/Util/IFRUtil_SQLNumeric.h"

#include <stdlib.h>
#include <ctype.h>

//----------------------------------------------------------------------
// Converts the SQL Numeric integer number into a string 
// (taken from vpa05num.c).
//
// val - the number to be converted.
// str - points to pointer TO THE END of the result string.
//
// *str is decremented so that it points to the beginning 
// of the result string
static unsigned char integer2string_hexfactors[SQL_MAX_NUMERIC_LENGTH][SQL_MAX_NUMERIC_DIGITS] =
{
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {6,5,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {6,3,5,5,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {6,1,2,7,7,7,6,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {6,9,2,7,6,9,4,9,2,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {6,7,7,7,2,6,1,1,5,9,9,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {6,5,6,0,1,7,6,7,9,4,7,4,1,8,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {6,3,9,7,2,9,7,3,0,4,9,5,7,5,0,2,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {6,1,6,1,5,5,9,0,7,3,7,0,4,4,7,6,4,4,8,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {6,9,6,3,1,2,5,4,6,9,6,8,2,8,4,6,6,3,2,2,7,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {6,7,1,6,0,7,4,7,1,9,2,6,4,1,6,9,1,8,5,2,9,8,0,2,1,0,0,0,0,0,0,0,0,0,0,0,0},
  {6,5,0,1,8,7,4,2,7,8,6,0,5,4,3,1,2,8,9,0,0,5,8,4,9,0,3,0,0,0,0,0,0,0,0,0,0},
  {6,3,3,0,5,9,3,4,5,3,9,5,7,3,3,4,6,2,4,1,5,2,6,1,8,2,2,9,7,0,0,0,0,0,0,0,0},
  {6,1,0,6,8,2,1,5,2,7,4,9,3,2,4,0,7,6,1,5,6,3,0,6,9,0,4,2,8,2,0,2,0,0,0,0,0},
  {6,9,0,0,2,2,9,2,3,6,9,4,0,3,5,8,2,6,7,2,8,4,3,5,8,5,8,6,9,2,2,9,1,5,0,0,0},
  {6,7,5,4,4,3,0,8,2,0,6,0,7,0,8,3,0,9,2,7,8,5,1,9,4,8,7,5,9,9,7,2,2,9,2,3,1}
}; /* integer2string_hexfactors */

/* the number of used digits (<>0) from factors */
static unsigned char integer2string_usedhexdigits[] =
{ 1, 3, 5, 8, 10, 13, 15, 17, 20, 22, 25, 27, 29, 32, 34, 37 };

static void integer2string(const unsigned char *val,
                           char         **str)
{
    IFR_UInt4 maxUsedByte = SQL_MAX_NUMERIC_LENGTH;
    
    while(maxUsedByte>0 && val[--maxUsedByte]==0)
        ;
    
    if(maxUsedByte==0 && val[0]==0) {
        *(--(*str)) = '0';
        return;
    }


    IFR_UInt4 nDigit;
    IFR_UInt4 nByte;
    IFR_UInt4 startByte=0; 
    IFR_UInt4 tmpRes  = 0; // temporary result and carry
    char     *bufpos  = *str;
    
    
    // foreach digit, until digit is >38, or there are no carry values remaining
    for(nDigit=0; 
        (nDigit<SQL_MAX_NUMERIC_DIGITS) && (startByte <= maxUsedByte || tmpRes!=0);
        ++nDigit) {

        
        // Idea:
        //  - number_value is val[0]*16^0 + val[1]*16^1 ... val[15]*16^15
        //  - the k-th digit of number_value is (number_value/10^k) % 10
        //  - 16^i can be written sum of powers of 10 multiplied with the respective digits,
        //    e.g. 256 is 2*10^2 + 5*10^1 + 6*10^0, which can be written
        //    16^i = p16[i][0] * 10^0 + p16[i][1] * 10^1 ...
        //  - the i-th digit is (carry[i-1] + val[0]*p16[0][i]*10^i + val[1]*p16[1][i]*10^i ...) % 10,
        //    carry[-1]=0, carry[i]=(carry[i-1] +  val[0]*p16[0][i]*10^i + val[1]*p16[1][i]*10^i ...) / 10
        for(nByte = startByte; nByte <= maxUsedByte; ++nByte) {
            tmpRes += integer2string_hexfactors[nByte][nDigit] * val[nByte]; 
            
        }

        // skip those number who will contain only zeros
        if(nDigit >= integer2string_usedhexdigits[startByte]) {
            ++startByte;
        }

        (*(--bufpos)) = '0' + (tmpRes % 10);
        
        

        tmpRes /= 10;
    }
    while((*bufpos == '0') && (bufpos < (*str - 1))) {
        ++bufpos;
    }
    *str = bufpos;
    return;
} 

//----------------------------------------------------------------------
static unsigned char zero_vdnnumber[20] = { 0x80, 0x00, 0x00, 0x00, 0x00, 
                                            0x00, 0x00, 0x00, 0x00, 0x00, 
                                            0x00, 0x00, 0x00, 0x00, 0x00, 
                                            0x00, 0x00, 0x00, 0x00, 0x00 
};

//----------------------------------------------------------------------
IFR_Retcode
IFRUtil_SQLNumeric::numericToNumber(const SQL_NUMERIC_STRUCT &number,
                                    unsigned char *vdnnumber,
                                    IFR_Int4 digits)
{
    // DBUG_METHOD_ENTER(IFRUtil_SQLNumeric, numericToNumber);
    char tmpNum[SQL_MAX_NUMERIC_DIGITS];
    // memset(tmpNum, 0, SQL_MAX_NUMERIC_DIGITS);
    
    char *p=tmpNum + SQL_MAX_NUMERIC_DIGITS;
    integer2string(number.val, &p);
    
    IFR_Int4 numdigits= SQL_MAX_NUMERIC_DIGITS - (p - tmpNum);
    
    if(numdigits > digits) {
        return IFR_OVERFLOW;
    }
    
    
    IFR_Int4 digits_bytelength=(digits+1)/2 + 1;
    if(numdigits==1 && *p=='0') {
        memcpy(vdnnumber, zero_vdnnumber, digits_bytelength+1);
        return IFR_OK;
    } else {
        memset(vdnnumber, 0, digits_bytelength);
    }

    char *l=p;                        // iterator for character string
    IFR_Int4 idest=1;                 // destination index
    IFR_Int4 vdnscale = -number.scale + numdigits; // vdn number scale
    
    if(abs(vdnscale) > 64) {
        return IFR_OVERFLOW;
    }
    
    if(number.sign == 0) { // negative 
        vdnnumber[0] = 64 - vdnscale;
        for(IFR_Int4 i=0; i<numdigits; ++i) {
            if(i & 1 ) {
                if(i == numdigits-1) {
                    vdnnumber[idest] |= 10 - (*l - '0');
                } else {
                    vdnnumber[idest] |= 9 - (*l - '0');
                }
                ++l;
                ++idest;
            } else {
                if(i == numdigits-1) {
                    vdnnumber[idest] = (10 - (*l  - '0')) << 4;
                } else {
                    vdnnumber[idest] = (9 - (*l  - '0')) << 4;
                }
                ++l;
            }
        }
    } else { // positive
        vdnnumber[0] = 192 + vdnscale;
        for(IFR_Int4 i=0; i<numdigits; ++i) {
            if(i & 1 ) {
                vdnnumber[idest] |= (*l - '0');
                ++l;
                ++idest;
            } else {
                vdnnumber[idest] = (*l  - '0') << 4;
                ++l;
            }
        }
    }
    return IFR_OK;
}

//----------------------------------------------------------------------
IFR_Retcode
IFRUtil_SQLNumeric::numericToAsciiString(const SQL_NUMERIC_STRUCT &number,
                                         char                     *buffer,
                                         IFR_size_t               &bufferlength)
{
    char tmpNum[SQL_MAX_NUMERIC_DIGITS];
    char *p=tmpNum + SQL_MAX_NUMERIC_DIGITS;
    integer2string(number.val, &p);
    IFR_Int4 numdigits = SQL_MAX_NUMERIC_DIGITS - (p - tmpNum);
    

    char         *b=buffer;
    IFR_size_t blen=bufferlength;
    IFR_Int4      binc=1;
    memset(b, 0, bufferlength);
    
    if(number.sign == 0) {
        *b='-';
        ++b;;
        --blen;
    }
    if(blen == 0) {
        return IFR_OVERFLOW;
    }
    
    IFR_Int4 srcindex=0;
    for(srcindex = 0; srcindex < numdigits - number.scale; ++srcindex) {
        if(blen == 0) {
            return IFR_OVERFLOW;
        }
        if(srcindex < numdigits) {
            *b=p[srcindex];
            --blen;
            ++b;;
        } else {
            *b='0';
            --blen;
            ++b;;
        }
    }
    if(srcindex == 0) {
        if(blen == 0) {
            return IFR_OVERFLOW;
        }
        *b='0';
        --blen;
        ++b;;
        srcindex = numdigits - number.scale;
    }
    if(srcindex < numdigits) {
        if(blen == 0) {
            return IFR_OVERFLOW;
        }
        *b='.';
        --blen;
        ++b;;
    }
    for(; srcindex < numdigits; ++srcindex) {
        if(blen == 0) {
            return IFR_DATA_TRUNC;
        }
        if(srcindex < 0) {
            *b = '0';
            --blen;
            ++b;;
        } else {
            *b = p[srcindex];
            --blen;
            ++b;;
        }
    }
    bufferlength -= blen;
    return IFR_OK;
}



//----------------------------------------------------------------------
static unsigned char string2integer_decfactors[38][16] =
{
    { 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0xe8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x10, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0xa0, 0x86, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x40, 0x42, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x80, 0x96, 0x98, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0xe1, 0xf5, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0xca, 0x9a, 0x3b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0xe4, 0x0b, 0x54, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0xE8, 0x76, 0x48, 0x17, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x10, 0xA5, 0xD4, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0xA0, 0x72, 0x4E, 0x18, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x40, 0x7A, 0x10, 0xF3, 0x5A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x80, 0xC6, 0xA4, 0x7E, 0x8D, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0xC1, 0x6F, 0xF2, 0x86, 0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x8A, 0x5D, 0x78, 0x45, 0x63, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x64, 0xA7, 0xB3, 0xB6, 0xE0, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0xE8, 0x89, 0x04, 0x23, 0xC7, 0x8A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x10, 0x63, 0x2D, 0x5E, 0xC7, 0x6B, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0xA0, 0xDE, 0xC5, 0xAD, 0xC9, 0x35, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x40, 0xB2, 0xBA, 0xC9, 0xE0, 0x19, 0x1E, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x80, 0xF6, 0x4A, 0xE1, 0xC7, 0x02, 0x2D, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0xA1, 0xED, 0xCC, 0xCE, 0x1B, 0xC2, 0xD3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x4A, 0x48, 0x01, 0x14, 0x16, 0x95, 0x45, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0xE4, 0xD2, 0x0C, 0xC8, 0xDC, 0xD2, 0xB7, 0x52, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0xE8, 0x3C, 0x80, 0xD0, 0x9F, 0x3C, 0x2E, 0x3B, 0x03, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x10, 0x61, 0x02, 0x25, 0x3E, 0x5E, 0xCE, 0x4F, 0x20, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0xA0, 0xCA, 0x17, 0x72, 0x6D, 0xAE, 0x0F, 0x1E, 0x43, 0x01, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x40, 0xEA, 0xED, 0x74, 0x46, 0xD0, 0x9C, 0x2C, 0x9F, 0x0C, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x80, 0x26, 0x4B, 0x91, 0xC0, 0x22, 0x20, 0xBE, 0x37, 0x7E, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x81, 0xEF, 0xAC, 0x85, 0x5B, 0x41, 0x6D, 0x2D, 0xEE, 0x04, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x0A, 0x5B, 0xC1, 0x38, 0x93, 0x8D, 0x44, 0xC6, 0x4D, 0x31, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x64, 0x8E, 0x8D, 0x37, 0xC0, 0x87, 0xAD, 0xBE, 0x09, 0xED, 0x01, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0xE8, 0x8F, 0x87, 0x2B, 0x82, 0x4D, 0xC7, 0x72, 0x61, 0x42, 0x13, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x10, 0x9F, 0x4B, 0xB3, 0x15, 0x07, 0xC9, 0x7B, 0xCE, 0x97, 0xC0, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0xA0, 0x36, 0xF4, 0x00, 0xD9, 0x46, 0xDA, 0xD5, 0x10, 0xEE, 0x85, 0x07 },
};

static int string2integer_startdigits[] =
{ 0, 3, 5, 8, 10, 13, 15, 17, 20, 22, 25, 27, 29, 32, 34, 37, 38, 38 };

#define GETDIGIT(string, i) (((string)[i] - '0'))


static void string2integer(unsigned char *string,
                           int            numdigits,
                           unsigned char *val)
{
    int startDigit = 0;
    unsigned int tmpRes = 0;

    
    for(int nByte=0; nByte<16 && (startDigit<numdigits || tmpRes!=0); ++nByte) {
        int pos = numdigits - 1 - startDigit;
        for(int nDigit = startDigit; nDigit < numdigits; ++nDigit) {
            tmpRes += GETDIGIT(string, pos) * string2integer_decfactors[nDigit][nByte];
            --pos;
        }
    }
}

//----------------------------------------------------------------------
// XXX -- add means to supply scale and precision
IFR_Retcode
IFRUtil_SQLNumeric::asciiStringToNumeric(const char *buffer,
                                         SQL_NUMERIC_STRUCT& number)
{
    memset(&number, 0, sizeof(SQL_NUMERIC_STRUCT));
    number.sign = 1; // assume number > 0
    number.precision = 38;
    
    char *p = (char *)buffer;
    while(isspace(*p) && *p) {
        ++p;
    }
    if(*p == '+') {
        ++p;
    } else if(*p == '-') {
        number.sign = 0;
        ++p;
    }
    if(!(*p)) {
        return IFR_NOT_OK;
    }
    // skip leading zeros
    while((*p) == '0') {
        ++p;
    }
    if(!(*p)) {
        return IFR_NOT_OK;
    }
    char digitbuffer[39];
    int  digits_in_buffer = 0;
    char *dotpos = strchr(p, '.');
    if(dotpos != 0 && (dotpos - p) > 38) {
        return IFR_OVERFLOW;
    }
    if(dotpos) {
        memcpy(digitbuffer, p, dotpos - p);
        digits_in_buffer = dotpos - p;
        p = dotpos + 1;
        int totaldigits = 0;
        int significantdigits = 0;
        while(*p) {
            if(!isdigit(*p)) {
                break;
            }
            if(*p == '0') {
                ++totaldigits;
            } else {
                ++totaldigits;
                significantdigits = totaldigits;
            }
            ++p;
        }
        
        if(*p) {
            while((*p) && isspace(*p)) {
                ++p;
            }
            if(*p) {
                return IFR_NOT_OK;
            }
        }
        // truncate 
        if(significantdigits > (38 - digits_in_buffer)) {
            significantdigits = 38 - digits_in_buffer;
        }
        
        number.scale = significantdigits;
        memcpy(digitbuffer + digits_in_buffer, dotpos + 1, significantdigits);
        digits_in_buffer += significantdigits;
        
    } else {
        char *digitstart = p;
        while(*p && isdigit(*p)) {
            ++p;
        }
        if(p - digitstart > 38) {
            return IFR_OVERFLOW;
        }
        memcpy(digitbuffer, digitstart, p-digitstart);
        digits_in_buffer = p - digitstart;
        while(*p) {
            if(!isspace(*p)) {
                return IFR_NOT_OK;
            }
            ++p;
        }
        number.scale=0;
    }
    IFR_Int4 startdigit = 0;
    IFR_UInt4 tmpres = 0;
    for(IFR_Int4 nbyte =0; nbyte < SQL_MAX_NUMERIC_LENGTH && (startdigit < digits_in_buffer || tmpres!=0); ++nbyte) {
        IFR_Int4 pos = digits_in_buffer - 1 - startdigit;
        for(IFR_Int4 ndigit = startdigit; ndigit < digits_in_buffer; ndigit ++) {
            IFR_Int4 currentdigit;
            if(!isdigit(digitbuffer[pos])) {
                return IFR_NOT_OK;
            }
            currentdigit = digitbuffer[pos] - '0';
            tmpres += currentdigit * string2integer_decfactors[ndigit][nbyte];
            --pos;
        }
        number.val[nbyte] = tmpres & 0xFF;
        tmpres >>= 8;
        startdigit = string2integer_startdigits[nbyte + 1];
    }
    
    return IFR_OK; 
}


//----------------------------------------------------------------------
IFR_Retcode
IFRUtil_SQLNumeric::numberToNumeric(const unsigned char *vdnnumber,
                                    IFR_Int4             length,
                                    SQL_NUMERIC_STRUCT&  number,
                                    IFR_UInt1            precision)
{
    memset(&number, 0, sizeof(SQL_NUMERIC_STRUCT));
    
    // at first we will check for zero
    if(*vdnnumber == 0x80) {
        // Create a +0 with the right precision.
        number.precision=precision;
        number.sign=1;
    }
    IFR_Int4 exponent = *vdnnumber;
    IFR_Int4 lastsignificant=0;
    // get the last zero
    for(IFR_Int4 i = 0; i<length; ++i) {
        if(i & 1) {
            if(vdnnumber[1 + i/2] & 0x0F) {
                lastsignificant = i;
            }
        } else {
            if(vdnnumber[1 + i/2] & 0xF0) {
                lastsignificant = i;
            }
        }
    }
    
    // we have handled up to lastsignificant + 1 digits 
    // the original number value was
    //
    //   value * pow(10, exponent); // 0 < value < 1
    // 
    // is same as
    //
    //   value * pow(10, numdigits) * pow(10, -numdigits) * pow(10, exponent)
    //
    // which eventually lead to setting the scale of the SQL_NUMERIC_STRUCT
    // to - (exponent - numdigits) = numdigits - exponents

    number.scale = lastsignificant + 1 - exponent;
    number.precision = precision;
    
    IFR_Int4   startDigit=0;
    IFR_UInt4  tmpRes=0;
    if(*vdnnumber < 0x80) {
        exponent = -exponent + 64;
               
        for(IFR_Int4 nByte=0; nByte<SQL_MAX_NUMERIC_LENGTH &&  (startDigit<length || tmpRes!=0); ++nByte) {
            int pos = length - 1 - startDigit;
            for(IFR_Int4 nDigit = startDigit; nDigit<length; nDigit ++) {
                IFR_Int4 currentDigit;
                if(pos < lastsignificant) {
                    if(pos & 1) {
                        currentDigit = 9 - (vdnnumber[1 + pos/2] & 0x0F);
                    } else {
                        currentDigit = 9 - (vdnnumber[1 + pos/2] >> 4);
                    }
                } else if(pos == lastsignificant) {
                    if(pos & 1) {
                        currentDigit = 10 - (vdnnumber[1 + pos/2] & 0x0F);
                    } else {
                        currentDigit = 10 - (vdnnumber[1 + pos/2] >> 4);
                    }
                } else { // behind the last significant number all
                    // is zero, we can stop
                    break;
                }
                tmpRes += currentDigit * string2integer_decfactors[nDigit][nByte];
                --pos;
            }
            number.val[nByte] = tmpRes & 0xFF;
            tmpRes >>= 8;
            startDigit = string2integer_startdigits[nByte+1];
        }
        
    } else {
        exponent -= 192;
        number.sign=1; // signal number > 0

        for(IFR_Int4 nByte=0; nByte<SQL_MAX_NUMERIC_LENGTH &&  (startDigit<length || tmpRes!=0); ++nByte) {
            int pos = length - 1 - startDigit;
            for(IFR_Int4 nDigit = startDigit; nDigit<length; nDigit ++) {
                IFR_Int4 currentDigit;
                if(pos <= lastsignificant) {
                    if(pos & 1) {
                        currentDigit = (vdnnumber[1 + pos/2] & 0x0F);
                    } else {
                        currentDigit = (vdnnumber[1 + pos/2] >> 4);
                    }
                } else { // behind the last significant number all
                    // is zero, we can stop
                    break;
                }
                tmpRes += currentDigit * string2integer_decfactors[nDigit][nByte];
                --pos;
            }
            number.val[nByte] = tmpRes & 0xFF;
            tmpRes >>= 8;
            startDigit = string2integer_startdigits[nByte+1];
        }
    }

    return IFR_OK;
}


