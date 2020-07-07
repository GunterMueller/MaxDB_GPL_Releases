/*!**********************************************************************

  module: vpa05num.c

  -----------------------------------------------------------------------

  responsible:  

  special area: ODBC-Driver

  description:  Conversion for SQL_C_NUMERIC struct

  see also:     http://www.microsoft.com/data/odbc/

  -----------------------------------------------------------------------

  copyright:                Copyright (c) 1999-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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




************************************************************************/

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "hpa05num.h"

/*!**********************************************************************

  Chapter: prototypes for internal functions

************************************************************************/

/* MAX_NUMERIC_DIGITS + one for sign, '\0' and '.' */
#define MAX_NUMERIC_STRING_LEN  (MAX_NUMERIC_DIGITS_PA05*sizeof(UCHAR)\
                                 +4*sizeof(UCHAR))


/* convert unsigned integer val with valLen bytes to string.
 * *stringBufPos has to point to the last byte of the string buffer!
 * When the function returns API_OK *stringBufPos points to the first
 * byte of the string. When bufSize is too small the function returns
 * API_NOT_OK
 */
API_RETCODE pa05_Int2String( UCHAR   *val,
                             UDWORD   valLen,
                             UCHAR  **stringBufPos,
                             UDWORD   bufSize );

/* convert string to unsigned integer with max. valLen bytes.
 * When the integer does not fit into valLen bytes the function returns
 * API_NOT_OK.
 */
API_RETCODE pa05_String2Int( UCHAR  *string,
                             UCHAR  *val,
                             UCHAR   valLen );

/*!**********************************************************************

  EndChapter: prototypes for internal functions

************************************************************************/
/*!**********************************************************************

  Chapter: conversion functions for SQL_NUMERIC_STRUCT

************************************************************************/

API_RETCODE pa05Numeric2String( SQL_NUMERIC_STRUCT *numeric,
                                UWORD               numericScale,
                                UCHAR              *stringBuf,
                                UDWORD              bufSize )
{
    API_RETCODE apiRetcode = API_OK;
    
    UCHAR  buf[MAX_NUMERIC_STRING_LEN];   /* local buffer for out string */
    UCHAR *bufPos = &buf[MAX_NUMERIC_STRING_LEN-1];
    UWORD  nDigits = 0;                   /* number of calculated digits */

    /* get digits */
    apiRetcode = pa05_Int2String( numeric->val, MAX_NUMERIC_BYTES_PA05,
                                  &bufPos, sizeof(buf) );
    if (apiRetcode == API_OK) {
        nDigits = API_STRLEN(bufPos);
        if (numericScale > nDigits) {
            /* fill up with zeros */
            while (numericScale > nDigits && nDigits+1 < sizeof(buf)) {
                *--bufPos = '0';
                nDigits++;
            };
        }; /* if */
        if (bufSize > nDigits + 3*sizeof(UCHAR)) {
            /* stringBuf is big enough. copy into stringBuf */
            stringBuf[0] = (numeric->sign == NUMERIC_NEGATIVE_SIGN) ?
                '-' : '+';
            /* copy left digits */
            API_MEMCPY( &stringBuf[1], bufPos, nDigits-numericScale );
            if (numericScale > 0) {
                /* set decimal point and copy right digits */
                bufPos += nDigits-numericScale;
                stringBuf[nDigits-numericScale+1] = '.';
                API_MEMCPY( &stringBuf[nDigits-numericScale+2], bufPos,
                            numericScale );
                stringBuf[nDigits+2] = '\0';
            } else {
                stringBuf[nDigits+1] = '\0';
            }; /* else */
        } else {
            /* stringBuf is too small */
            apiRetcode = API_NOT_OK;
        }; /* else */
    }; /* if */

    return apiRetcode;
} /* pa05Numeric2String */


API_RETCODE pa05Double2Numeric( SDOUBLE             number,
                                SQL_NUMERIC_STRUCT *numeric )
{
    API_RETCODE apiRetcode = API_OK;
    UCHAR       buf[MAX_NUMERIC_STRING_LEN];
    UWORD       stringLen = 0;
    SDWORD      scale = 0;
    tsp00_Int4  dec;
    tsp00_Int4  sign;
    
    API_STRCPY( buf, API_ECVT( number, sizeof(buf)-1, &dec, &sign ) );
    sign = (sign == 0) ? NUMERIC_POSITIVE_SIGN : NUMERIC_NEGATIVE_SIGN;
    stringLen = API_STRLEN( buf );
    scale = stringLen-dec;
    /* cut trailing nulls */
    while (buf[dec+scale-1] == '0' && scale >0) { scale--; };
    buf[dec+scale] = '\0';
    /* convert buf to integer */
    apiRetcode = pa05_String2Int( buf, numeric->val, sizeof( numeric->val ) );
    if (apiRetcode = API_OK) {
        numeric->sign      = sign;
        numeric->scale     = scale;
        numeric->precision = API_STRLEN(buf);
    };

    return apiRetcode;
} /* pa05Double2Numeric */


API_RETCODE pa05String2Numeric( UCHAR              *string,
                                UWORD               numericScale,
                                SQL_NUMERIC_STRUCT *numeric )
{
    API_RETCODE apiRetcode = API_OK;
    UCHAR   buf[MAX_NUMERIC_STRING_LEN];  /* local buffer for integer string */
    UCHAR  *start     = string;
    UWORD   pointPos  = 0;
    UWORD   stringLen = 0;
    SCHAR   scale     = 0;
    SCHAR   sign      = 1;
    
    /* skip leading spaces */
    while (*start == ' ' ) { start++; };
    /* get sign */
    sign = (*start == '-') ? NUMERIC_NEGATIVE_SIGN : NUMERIC_POSITIVE_SIGN;
    /* skip sign */
    if (*start == '-' || *start == '+') {
        start++;
    }; /* if */
    /* skip leading zeros */
    while (*start == '0' && *(start+1)!='\0' && *(start+1)!='.' ) { start++; };
    stringLen = API_STRLEN(start);
    /* find decimal point and get scale */
    while (pointPos<stringLen && start[pointPos] != '.') { pointPos++; };
    scale = (pointPos+1 < stringLen) ? stringLen-pointPos-1 : 0;
    if (scale > 0) {
        /* skip trailing '0' */
        while (start[pointPos+scale] == '0' && scale > numericScale) {
            scale--;
        };
    };
    /* copy left digits */
    memcpy( buf, start, pointPos );
    /* copy right digits */
    memcpy( &buf[pointPos], &start[pointPos+1], scale );
    /* fill up zeros at the end to get desired numericScale */
    while (scale < numericScale) {
        scale++;
        buf[pointPos+scale-1] = '0';
    }; /* for */
    buf[pointPos+numericScale] = '\0';
    /* convert buf to integer */
    apiRetcode = pa05_String2Int( buf, numeric->val, sizeof( numeric->val ) );
    if (apiRetcode == API_OK) {
        if  (scale > numericScale) {
            /* we lost some digits */
            apiRetcode = API_TRUNCATE;
        }; /* if */
        numeric->scale     = numericScale;
        numeric->precision = API_STRLEN( buf );
        numeric->sign      = sign;
    }; /* if */
    
    return apiRetcode;
} /* pa05String2Numeric */


/*!**********************************************************************

  EndChapter: conversion functions for SQL_NUMERIC_STRUCT

************************************************************************/
/*!**********************************************************************

  Chapter: internal functions

************************************************************************/

#define MAX_FACTOR_DIGITS   (MAX_NUMERIC_DIGITS_PA05-1)

/* the digits of 256^0, 256^1, 256^2, ..., 256^15 in reverse order */
static unsigned char hexFactors[MAX_NUMERIC_BYTES_PA05][MAX_FACTOR_DIGITS] =
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
}; /* hexFactors */

/* the number of used digits (<>0) from factors */
static unsigned char usedHexDigits[MAX_NUMERIC_BYTES_PA05] =
{ 1, 3, 5, 8, 10, 13, 15, 17, 20, 22, 25, 27, 29, 32, 34, 37 };
    
#define	tochar(n)           ('0'+(n))

/* convert unsigned integer with valLen bytes to string */
API_RETCODE pa05_Int2String( UCHAR   *val,
                             UDWORD   valLen,
                             UCHAR  **stringBufPos,
                             UDWORD   bufSize )
{
    API_RETCODE apiRetcode = API_OK;
    
    UWORD  maxUsedByte = valLen;            /* the index of last byte <> 0 */
    UCHAR *bufPos      = *stringBufPos;     /* start with right most digit */
    UWORD  startByte   = 0;                 /* the lowest byte index with
                                             * factor != 0                 */
    UWORD  tmpRes = 0;                      /* result for actual digit     */
    UWORD  nDigit = 0;                      /* count for digit             */
    UWORD  nByte  = 0;                      /* index of current byte       */
    
    /* find index of highest byte <> 0 */
    while (maxUsedByte > 0 && val[--maxUsedByte] == 0) {};
    *bufPos = '\0';
    /* calculate digits */
    for ( nDigit = 0;
          (nDigit+1 < bufSize) && (startByte <= maxUsedByte || tmpRes != 0);
          nDigit++) {
        for (nByte = startByte; nByte <= maxUsedByte; nByte++) {
            tmpRes += hexFactors[nByte][nDigit] * val[nByte];
        }; /* for */
        *--bufPos = tochar( tmpRes % 10 );
        tmpRes /= 10;
        if (nDigit+1 >= usedHexDigits[startByte]) {
            /* all following factors are zero for startByte */
            startByte++;
        };
    }; /* for */
    if (nDigit+1 >= bufSize) {
        /* buffer was too small */
        apiRetcode = API_NOT_OK;
    } else {
        *stringBufPos = bufPos;
    }; /* else */
    
    return apiRetcode;
} /* pa05_Int2String */


/* one byte <=> one digit */
#define MAX_DECFACTOR_DIGITS  16

#define toint(c)  (c-'0')

/* bytes of 10^0, 10^1, ..., 10^38 in reverse order */
static UCHAR decFactors[MAX_NUMERIC_DIGITS_PA05][MAX_DECFACTOR_DIGITS] =
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
}; /* decFactors */


static UCHAR startDigits[MAX_DECFACTOR_DIGITS+2] =
{ 0, 3, 5, 8, 10, 13, 15, 17, 20, 22, 25, 27, 29, 32, 34, 37, 38, 38 }; /* last two are dummies */
                         
API_RETCODE pa05_String2Int( UCHAR  *string,
                             UCHAR  *val,
                             UCHAR   valLen )
{
    API_RETCODE apiRetcode = API_OK;
    UWORD  stringLen   = API_STRLEN(string);
    UCHAR *stringPos   = NULL;
    UCHAR *valPos      = val;
    UWORD  nByte       = 0;
    UWORD  nDigit      = 0;
    UWORD  startDigit  = startDigits[0];
    UWORD  tmpRes      = 0;
    
    if (stringLen > MAX_NUMERIC_STRING_LEN) {
        /* too long */
        apiRetcode = API_NOT_OK;
    } else {
        for (nByte = 0;
             (nByte < valLen) && (startDigit < stringLen || tmpRes != 0);
             nByte++) {
            stringPos = &string[stringLen-1-startDigit];
            for (nDigit = startDigit; nDigit < stringLen; nDigit++ ) {
                tmpRes += toint( *stringPos-- ) * decFactors[nDigit][nByte];
            };
            *valPos++ = tmpRes % 256;
            tmpRes /= 256;
            startDigit = startDigits[nByte+1];
        }; /* for */
    }; /* else */
    if (nByte > valLen) {
        /* valLen too short */
        apiRetcode = API_NOT_OK;
    } else {
        /* fill up with null bytes */
        for (; nByte<valLen; nByte++ ) {
            val[nByte] = 0;
        };
    }; /* else */
    
    return apiRetcode;
} /* pa05_String2Int */

/*!**********************************************************************

  EndChapter: internal functions

************************************************************************/
