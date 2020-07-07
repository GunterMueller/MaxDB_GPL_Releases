/*!**************************************************************************

  module      : vsp45.c

  -------------------------------------------------------------------------

  responsible : MarcoP

  special area:
  description : Conversion between character and various numerical representations.

  NOTE hsp45.h/vsp45.c replaces old vsp45c

  last changed: 2001-01-03  17:17
  see also    :

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG


    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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



  
*****************************************************************************/

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include <stdio.h>
#include <memory.h>
#include <ctype.h>
#include "hsp45.h"
#include "hsp78_0.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
#define INT4_DIG   12 /* max. no. of characters for tsp00_Int4 + 1 */
#define INT8_DIG   26 /* max. no. of characters for tsp00_Longint + 1(8byte) */
#define MAX_FLOAT 130 /* max. posible string length for strings which containing floats */
#define FLO8_DIG   26 /* max. no. of characters for tsp00_Longreal + 1
                        (g conversion) */
#define FLO4_PREC  7 /* precision of FLOAT4 */
#define FLO8_PREC 15 /* precision of tsp00_Longreal */

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/
#define SP45CIS_ASCII(buf, entype) ((entype) == sp77encodingAscii ? 1 : ((buf) == '\0'))

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/


/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/


/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

/*!
  Function:     sp45cSkipOverLeadingSpaces

  see also: 

  Description:  skips over leading blanks.
              
  Arguments:
    entype  [in]  encoding type of buf
    buf     [in]  string buffer
    posanf  [in]  begin position
    offset  [in|out] length of buf

  return value: position of first nonspace character 
 */
tsp00_Int4 sp45cSkipOverLeadingSpaces(tsp45_StringEncoding entype, const char* buf, tsp00_Int4 posanf, tsp00_Int4 *len)
{
  tsp00_Int4 pos, zeropos, offset;
  if (entype == sp77encodingAscii) {
    zeropos = 0;
    pos = posanf;
    offset = 1;
  } else if (entype == sp77encodingUCS2) {
    zeropos = -1;
    pos = posanf + 1;
    offset = 2;
  } else if (entype == sp77encodingUCS2Swapped) {
    zeropos = 1;
    pos = posanf;
    offset = 2;
  } else {
    zeropos = 0;
    pos = posanf;
    offset = 1;
  }
  
  /*eat leading spaces*/
  while (*len > 0 && isspace(buf[pos-1]) && 
         SP45CIS_ASCII (buf[pos-1+zeropos], entype)) {
     pos += offset;
     *len -= offset;
  }
  return (entype == sp77encodingUCS2)?pos-1:pos;
}/*-------------------------------------------------------------------*/

/*!
  Function:     sp45cStringtoShortInt

  see also: 

  Description:  Conversion from a character string representing a number 
  to its corresponding short integer representation depending on encoding type of
  character string.
    
  Arguments:      
     val [out] converted number of the specified type
     buf [in]  character buffer address
     pos [in]  byte count (pos = 1 means buf [0]) as starting position
               of the string
     len [in] maximum field length, the input string is stored
              between buf [pos - 1] and buf [pos + len - 2] with
              possibly preceding and trailing whitespace characters
 
     res [out] return status, num_ok indicates successful completion,
               num_invalid a conversion error returned by sscanf
     encType [in] string encoding of buf
  return value: void
*/ 
void sp45cStringtoShortInt (tsp00_Int2 *val, const char *buf, int startpos, int len, char *res, tsp45_StringEncoding encType)
{
    if (encType == sp77encodingAscii)
      s45stoi2 (val, buf, startpos, len, res);
      
    else if (   encType == sp77encodingUCS2
             || encType == sp77encodingUCS2Swapped){
                
      /*eat leading spaces*/
      tsp00_Int4 pos = sp45cSkipOverLeadingSpaces(encType, buf, startpos, &len);

      if (len > 0) {
          char       AsciiBuf[INT4_DIG+1];
          /*convert to ascii*/
          tsp00_Uint4   cbWrite;        /* number of bytes written */
          tsp00_Uint4   cbParsed;       /* number of bytes parsed */
          tsp78ConversionResult rc;

          rc = sp78convertString(sp77encodingAscii, AsciiBuf, INT4_DIG+1,
			                     &cbWrite, 1,
			                     encType, &buf[pos-1], 
			                     (INT4_DIG*2<=len)?INT4_DIG*2:len, 
			                     &cbParsed);
          /*call conversion method*/
          s45stoi2 (val, AsciiBuf, 1, INT4_DIG+1, res);
        }
        else {
          *val = 0;
          *res = 0; /* num_ok */
        }
    }    
    else {
      /*unsupported encoding*/  
      *val = 0;
      *res = 4; /* NUM_INCOMPATIBLE */
    }
}/*-------------------------------------------------------------------*/

/*!
  Function:     sp45cStringtoInt

  see also: 

  Description:  Conversion from a character string representing a number 
  to its corresponding integer representation depending on encoding type of
  character string.
    
  Arguments:      
     val [out] converted number of the specified type
     buf [in]  character buffer address
     pos [in]  byte count (pos = 1 means buf [0]) as starting position
               of the string
     len [in] maximum field length, the input string is stored
              between buf [pos - 1] and buf [pos + len - 2] with
              possibly preceding and trailing whitespace characters
 
     res [out] return status, num_ok indicates successful completion,
               num_invalid a conversion error returned by sscanf
     encType [in] string encoding of buf
  return value: void
*/ 
void sp45cStringtoInt (tsp00_Int4 *val, const char *buf, int startpos, int len, char *res, tsp45_StringEncoding encType)
{
    if (encType == sp77encodingAscii)
      s45stoi4 (val, buf, startpos, len, res);
      
    else if (   encType == sp77encodingUCS2
             || encType == sp77encodingUCS2Swapped){
      /*eat leading spaces*/
      tsp00_Int4 pos = sp45cSkipOverLeadingSpaces(encType, buf, startpos, &len);

      if (len > 0) {
          char       AsciiBuf[INT4_DIG+1];
          /*convert to ascii*/
          tsp00_Uint4   cbWrite;        /* number of bytes written */
          tsp00_Uint4   cbParsed;       /* number of bytes parsed */
          tsp78ConversionResult rc;

          rc = sp78convertString(sp77encodingAscii, AsciiBuf, INT4_DIG+1,
			                     &cbWrite, 1,
			                     encType, &buf[pos-1], 
			                     (INT4_DIG*2<=len)?INT4_DIG*2:len,
			                     &cbParsed);
          /*call conversion method*/
          s45stoi4 (val, AsciiBuf, 1, INT4_DIG+1, res);
      }
      else {
          *val = 0;
          *res = 0; /* num_ok */
      }
    }    
    else {
      /*unsupported encoding*/  
      *val = 0;
      *res = 4; /* NUM_INCOMPATIBLE */
    }
}

/*!
  Function:     sp45cStringtoLong

  see also: 

  Description:  Conversion from a character string representing a number 
  to its corresponding long integer representation depending on encoding type of
  character string.
    
  Arguments:      
     val [out] converted number of the specified type
     buf [in]  character buffer address
     pos [in]  byte count (pos = 1 means buf [0]) as starting position
               of the string
     len [in] maximum field length, the input string is stored
              between buf [pos - 1] and buf [pos + len - 2] with
              possibly preceding and trailing whitespace characters
 
     res [out] return status, num_ok indicates successful completion,
               num_invalid a conversion error returned by sscanf
     encType [in] string encoding of buf
  return value: void
*/ 
void sp45cStringtoLong (tsp00_Longint *val, const char *buf, int startpos, int len, char *res, tsp45_StringEncoding encType)
{
    if (encType == sp77encodingAscii)
      s45stoi8 (val, buf, startpos, len, res);
      
    else if (   encType == sp77encodingUCS2
             || encType == sp77encodingUCS2Swapped){
      /*eat leading spaces*/
      tsp00_Int4 pos = sp45cSkipOverLeadingSpaces(encType, buf, startpos, &len);

      if (len > 0) {
          char       AsciiBuf[INT8_DIG+1];
          /*convert to ascii*/
          tsp00_Uint4   cbWrite;        /* number of bytes written */
          tsp00_Uint4   cbParsed;       /* number of bytes parsed */
          tsp78ConversionResult rc;

          rc = sp78convertString(sp77encodingAscii, AsciiBuf, INT8_DIG+1,
			                     &cbWrite, 1,
			                     encType, &buf[pos-1], 
			                     (INT8_DIG*2<=len)?INT8_DIG*2:len,
			                     &cbParsed);
          /*call conversion method*/
          s45stoi8 (val, AsciiBuf, 1, INT8_DIG+1, res);
      }
      else {
          *val = 0;
          *res = 0; /* num_ok */
        }
    }    
    else {
      /*unsupported encoding*/  
      *val = 0;
      *res = 4; /* NUM_INCOMPATIBLE */
    }
}

/*!
  Function:     sp45cStringtoUShort

  see also: 

  Description:  Conversion from a character string representing a number 
  to its corresponding unsigned short integer representation depending on encoding type of
  character string.
    
  Arguments:      
     val [out] converted number of the specified type
     buf [in]  character buffer address
     pos [in]  byte count (pos = 1 means buf [0]) as starting position
               of the string
     len [in] maximum field length, the input string is stored
              between buf [pos - 1] and buf [pos + len - 2] with
              possibly preceding and trailing whitespace characters
 
     res [out] return status, num_ok indicates successful completion,
               num_invalid a conversion error returned by sscanf
     encType [in] string encoding of buf
  return value: void
*/ 
void sp45cStringtoUShort (tsp00_Uint2 *val, const char *buf, int startpos, int len, char *res, tsp45_StringEncoding encType)
{
    if (encType == sp77encodingAscii)
      s45stou2 (val, buf, startpos, len, res);
      
    else if (   encType == sp77encodingUCS2
             || encType == sp77encodingUCS2Swapped){
      /*eat leading spaces*/
      tsp00_Int4 pos = sp45cSkipOverLeadingSpaces(encType, buf, startpos, &len);

      if (len > 0) {
          char       AsciiBuf[INT4_DIG+1];
          /*convert to ascii*/
          tsp00_Uint4   cbWrite;        /* number of bytes written */
          tsp00_Uint4   cbParsed;       /* number of bytes parsed */
          tsp78ConversionResult rc;

          rc = sp78convertString(sp77encodingAscii, AsciiBuf, INT4_DIG+1,
			                     &cbWrite, 1,
			                     encType, &buf[pos-1], 
			                     (INT4_DIG*2<=len)?INT4_DIG*2:len,
			                     &cbParsed);
          /*call conversion method*/
          s45stou2 (val, AsciiBuf, 1, INT4_DIG+1, res);
      }
      else {
          *val = 0;
          *res = 0; /* num_ok */
        }
    }    
    else {
      /*unsupported encoding*/  
      *val = 0;
      *res = 4; /* NUM_INCOMPATIBLE */
    }
}

/*!
  Function:     sp45cStringtoUInt

  see also: 

  Description:  Conversion from a character string representing a number 
  to its corresponding unsigned integer representation depending on encoding type of
  character string.
    
  Arguments:      
     val [out] converted number of the specified type
     buf [in]  character buffer address
     pos [in]  byte count (pos = 1 means buf [0]) as starting position
               of the string
     len [in] maximum field length, the input string is stored
              between buf [pos - 1] and buf [pos + len - 2] with
              possibly preceding and trailing whitespace characters
 
     res [out] return status, num_ok indicates successful completion,
               num_invalid a conversion error returned by sscanf
     encType [in] string encoding of buf
  return value: void
*/ 
void sp45cStringtoUInt (tsp00_Uint4 *val, const char *buf, int startpos, int len, char *res, tsp45_StringEncoding encType)
{
    if (encType == sp77encodingAscii)
      s45stou4 (val, buf, startpos, len, res);
      
    else if (   encType == sp77encodingUCS2
             || encType == sp77encodingUCS2Swapped){
      /*eat leading spaces*/
      tsp00_Int4 pos = sp45cSkipOverLeadingSpaces(encType, buf, startpos, &len);

      if (len > 0) {
          char       AsciiBuf[INT4_DIG+1];
          /*convert to ascii*/
          tsp00_Uint4   cbWrite;        /* number of bytes written */
          tsp00_Uint4   cbParsed;       /* number of bytes parsed */
          tsp78ConversionResult rc;

          rc = sp78convertString(sp77encodingAscii, AsciiBuf, INT4_DIG+1,
			                     &cbWrite, 1,
			                     encType, &buf[pos-1], 
			                     (INT4_DIG*2<=len)?INT4_DIG*2:len,
			                     &cbParsed);
          /*call conversion method*/
          s45stou4 (val, AsciiBuf, 1, INT4_DIG+1, res);
      }
      else {
          *val = 0;
          *res = 0; /* num_ok */
        }
    }    
    else {
      /*unsupported encoding*/  
      *val = 0;
      *res = 4; /* NUM_INCOMPATIBLE */
    }
}

/*!
  Function:     sp45cStringtoFloat

  see also: 

  Description:  Conversion from a character string representing a number 
  to its corresponding float representation depending on encoding type of
  character string.
    
  Arguments:      
     val [out] converted number of the specified type
     buf [in]  character buffer address
     pos [in]  byte count (pos = 1 means buf [0]) as starting position
               of the string
     len [in] maximum field length, the input string is stored
              between buf [pos - 1] and buf [pos + len - 2] with
              possibly preceding and trailing whitespace characters
 
     res [out] return status, num_ok indicates successful completion,
               num_invalid a conversion error returned by sscanf
     encType [in] string encoding of buf
  return value: void
*/ 
void sp45cStringtoFloat (tsp00_Shortreal *val, const char *buf, int startpos, int len, char *res, tsp45_StringEncoding encType)
{
    if (encType == sp77encodingAscii)
      s45stor4 (val, buf, startpos, len, res);
      
    else if (   encType == sp77encodingUCS2
             || encType == sp77encodingUCS2Swapped){
      /*eat leading spaces*/
      tsp00_Int4 pos = sp45cSkipOverLeadingSpaces(encType, buf, startpos, &len);

      if (len > 0) {
          char       AsciiBuf[MAX_FLOAT+1];
          /*convert to ascii*/
          tsp00_Uint4   cbWrite;        /* number of bytes written */
          tsp00_Uint4   cbParsed;       /* number of bytes parsed */
          tsp78ConversionResult rc;

          rc = sp78convertString(sp77encodingAscii, AsciiBuf, MAX_FLOAT+1,
			                     &cbWrite, 1,
			                     encType, &buf[pos-1], 
			                     (MAX_FLOAT*2<=len)?MAX_FLOAT*2:len,
			                     &cbParsed);
          /*call conversion method*/
          s45stor4 (val, AsciiBuf, 1, MAX_FLOAT+1, res);
      }
      else {
          *val = (tsp00_Shortreal)0;
          *res = 0; /* num_ok */
        }
    }    
    else {
      /*unsupported encoding*/  
      *val = 0;
      *res = 4; /* NUM_INCOMPATIBLE */
    }
}

/*!
  Function:     sp45cStringtoDouble

  see also: 

  Description:  Conversion from a character string representing a number 
  to its corresponding double representation depending on encoding type of
  character string.
    
  Arguments:      
     val [out] converted number of the specified type
     buf [in]  character buffer address
     pos [in]  byte count (pos = 1 means buf [0]) as starting position
               of the string
     len [in] maximum field length, the input string is stored
              between buf [pos - 1] and buf [pos + len - 2] with
              possibly preceding and trailing whitespace characters
 
     res [out] return status, num_ok indicates successful completion,
               num_invalid a conversion error returned by sscanf
     encType [in] string encoding of buf
  return value: void
*/ 
void sp45cStringtoDouble (tsp00_Longreal *val, const char *buf, int startpos, int len, char *res, tsp45_StringEncoding encType)
{
    if (encType == sp77encodingAscii)
      s45stor8 (val, buf, startpos, len, res);
      
    else if (   encType == sp77encodingUCS2
             || encType == sp77encodingUCS2Swapped){
      /*eat leading spaces*/
      tsp00_Int4 pos = sp45cSkipOverLeadingSpaces(encType, buf, startpos, &len);

      if (len > 0) {
          char       AsciiBuf[MAX_FLOAT+1];
          /*convert to ascii*/
          tsp00_Uint4   cbWrite;        /* number of bytes written */
          tsp00_Uint4   cbParsed;       /* number of bytes parsed */
          tsp78ConversionResult rc;

          rc = sp78convertString(sp77encodingAscii, AsciiBuf, MAX_FLOAT+1,
			                     &cbWrite, 1,
			                     encType, &buf[pos-1], 
			                     (MAX_FLOAT*2<=len)?MAX_FLOAT*2:len,
			                     &cbParsed);
          /*call conversion method*/
          s45stor8 (val, AsciiBuf, 1, MAX_FLOAT+1, res);
      }
      else {
          *val = (tsp00_Longreal)0;
          *res = 0; /* num_ok */
        }
    }    
    else {
      /*unsupported encoding*/  
      *val = 0;
      *res = 4; /* NUM_INCOMPATIBLE */
    }
}

/*!
  Function:     s45i2tos

  see also: 

  Description:  Conversion from a short integer number to its
                character representation.
    
  Arguments:      
     val [in] number to be converted
     buf [in]  char buffer address for output of the resulting string
     pos [in]  byte count (pos = 1 means buf [0]) as starting position
               of the string
     len [in] output field length, the output string is stored left
              justified between buf [pos - 1] and buf [pos + len - 2]
 
     rlen[out] number of characters resulting from the conversion,
              rlen = len means success,
              rlen < 0 indicates a conversion error returned by
                      sprintf, buf is left unchanged, *res is set to
                      num_invalid,
              if rlen > len, only the leftmost len characters are
                      stored, res is set to num_overflow,
     res [out] return status, num_ok indicates successful completion,
               num_invalid a conversion error returned by sscanf

  return value: void
*/
void s45i2tos (
	      tsp00_Int2 val,
        char *buf,
        int pos, 
        int len,
        int *rlen,
        char *res)
{
	s45i4tos ((tsp00_Int4) val, buf, pos, len, rlen, res);
}

/*!
  Function:     s45u2tos

  see also: 

  Description:  Conversion from a unsigned short integer number to its
                character representation.
    
  Arguments:      
     val [in] number to be converted
     buf [in]  char buffer address for output of the resulting string
     pos [in]  byte count (pos = 1 means buf [0]) as starting position
               of the string
     len [in] output field length, the output string is stored left
              justified between buf [pos - 1] and buf [pos + len - 2]
 
     rlen[out] number of characters resulting from the conversion,
              rlen = len means success,
              rlen < 0 indicates a conversion error returned by
                      sprintf, buf is left unchanged, *res is set to
                      num_invalid,
              if rlen > len, only the leftmost len characters are
                      stored, res is set to num_overflow,
     res [out] return status, num_ok indicates successful completion,
               num_invalid a conversion error returned by sscanf

  return value: void
*/ 
void s45u2tos (
        tsp00_Uint2 val,
        char *buf,
        int pos,
        int len,
        int *rlen,
        char *res)
{
        s45u4tos ((tsp00_Uint4) val, buf, pos, len, rlen, res);
}

/*!
  Function:     s45i4tos

  see also: 

  Description:  Conversion from a integer number to its
                character representation.
    
  Arguments:      
     val [in] number to be converted
     buf [in]  char buffer address for output of the resulting string
     pos [in]  byte count (pos = 1 means buf [0]) as starting position
               of the string
     len [in] output field length, the output string is stored left
              justified between buf [pos - 1] and buf [pos + len - 2]
 
     rlen[out] number of characters resulting from the conversion,
              rlen = len means success,
              rlen < 0 indicates a conversion error returned by
                      sprintf, buf is left unchanged, *res is set to
                      num_invalid,
              if rlen > len, only the leftmost len characters are
                      stored, res is set to num_overflow,
     res [out] return status, num_ok indicates successful completion,
               num_invalid a conversion error returned by sscanf

  return value: void
*/ 
void s45i4tos (val, buf, pos, len, rlen, res)
	tsp00_Int4 val;
        char *buf;
        int pos, len;
        int *rlen;
        char *res;
{
        char tmp [INT4_DIG];
        int d;
 
        *rlen = sprintf (tmp, "%d", val);
 
        if (*rlen < 0)
                *res = 3; /* num_invalid */
        else
        {
                d = len - *rlen;
 
                if (d < 0)
                {
                        memcpy (&buf [pos - 1], tmp, len);
                        *res = 2; /* num_overflow */
                }
                else
                {
                        memcpy (&buf [pos - 1], tmp, *rlen);
                        *res =  0; /* num_ok */
 
                        if (d > 0)
                                memset (&buf [pos + *rlen - 1], ' ', d);
                }
        }
}
 
/*!
  Function:     s45i8tos

  see also: 

  Description:  Conversion from a long integer number to its
                character representation.
    
  Arguments:      
     val [in] number to be converted
     buf [in]  char buffer address for output of the resulting string
     pos [in]  byte count (pos = 1 means buf [0]) as starting position
               of the string
     len [in] output field length, the output string is stored left
              justified between buf [pos - 1] and buf [pos + len - 2]
 
     rlen[out] number of characters resulting from the conversion,
              rlen = len means success,
              rlen < 0 indicates a conversion error returned by
                      sprintf, buf is left unchanged, *res is set to
                      num_invalid,
              if rlen > len, only the leftmost len characters are
                      stored, res is set to num_overflow,
     res [out] return status, num_ok indicates successful completion,
               num_invalid a conversion error returned by sscanf

  return value: void
*/ 
void s45i8tos (val, buf, pos, len, rlen, res)
	tsp00_Longint val;
        char *buf;
        int pos, len;
        int *rlen;
        char *res;
{
        char tmp [INT8_DIG];
        int d;
 
        *rlen = sprintf (tmp, "%d", val);
 
        if (*rlen < 0)
                *res = 3; /* num_invalid */
        else
        {
                d = len - *rlen;
 
                if (d < 0)
                {
                        memcpy (&buf [pos - 1], tmp, len);
                        *res = 2; /* num_overflow */
                }
                else
                {
                        memcpy (&buf [pos - 1], tmp, *rlen);
                        *res =  0; /* num_ok */
 
                        if (d > 0)
                                memset (&buf [pos + *rlen - 1], ' ', d);
                }
        }
}

/*!
  Function:     s45u4tos

  see also: 

  Description:  Conversion from a unsigned integer number to its
                character representation.
    
  Arguments:      
     val [in] number to be converted
     buf [in]  char buffer address for output of the resulting string
     pos [in]  byte count (pos = 1 means buf [0]) as starting position
               of the string
     len [in] output field length, the output string is stored left
              justified between buf [pos - 1] and buf [pos + len - 2]
 
     rlen[out] number of characters resulting from the conversion,
              rlen = len means success,
              rlen < 0 indicates a conversion error returned by
                      sprintf, buf is left unchanged, *res is set to
                      num_invalid,
              if rlen > len, only the leftmost len characters are
                      stored, res is set to num_overflow,
     res [out] return status, num_ok indicates successful completion,
               num_invalid a conversion error returned by sscanf

  return value: void
*/
void s45u4tos (val, buf, pos, len, rlen, res)
        tsp00_Uint4 val;
        char *buf;
        int pos, len;
        int *rlen;
        char *res;
{
        char tmp [INT4_DIG];
        int d;
 
        *rlen = sprintf (tmp, "%u", val);
 
        if (*rlen < 0)
                *res = 3; /* num_invalid */
        else
        {
                d = len - *rlen;
 
                if (d < 0)
                {
                        memcpy (&buf [pos - 1], tmp, len);
                        *res = 2; /* num_overflow */
                }
                else
                {
                        memcpy (&buf [pos - 1], tmp, *rlen);
                        *res =  0; /* num_ok */
 
                        if (d > 0)
                                memset (&buf [pos + *rlen - 1], ' ', d);
                }
        }
}
/*!
  Function:     s45ftos

  see also: 

  Description:  Conversion from a float number to its
                character representation depending on given precision.
    
  Arguments:      
     val [in] number to be converted
     buf [in]  char buffer address for output of the resulting string
     pos [in]  byte count (pos = 1 means buf [0]) as starting position
               of the string
     len [in]  output field length, the output string is stored left
               justified between buf [pos - 1] and buf [pos + len - 2]
     prec[in]  precision of floating point number
     rlen[out] number of characters resulting from the conversion,
               rlen = len means success,
               rlen < 0 indicates a conversion error returned by
                      sprintf, buf is left unchanged, *res is set to
                      num_invalid,
               if rlen > len, only the leftmost len characters are
                      stored, res is set to num_overflow,
     res [out] return status, num_ok indicates successful completion,
               num_invalid a conversion error returned by sscanf

  return value: void
*/ 
void s45ftos (val, buf, pos, len, prec, rlen, res)
        tsp00_Longreal val;
        char *buf;
        int pos, len, prec;
        int *rlen;
        char *res;
{
        char tmp [FLO8_DIG];
        int d;
 
        *rlen = sprintf (tmp, "%.*g", prec, val);
 
        if (*rlen < 0)
                *res = 3; /* num_invalid */
        else
        {
                d = len - *rlen;
 
                if (d < 0)
                        *res = 2; /* num_overflow */
                else
                {
                        memcpy (&buf [pos - 1], tmp, *rlen);
                        *res =  0; /* num_ok */
 
                        if (d > 0)
                                memset (&buf [pos + *rlen - 1], ' ', d);
                }
        }
}

/*!
  Function:     s45r4tos

  see also: 

  Description:  Conversion from a float number to its character 
                representation.

  The conversion is performed using the g conversion specifier, i.e. the
  representation switches between f and e conversion depending on the
  specified precision and the value.
    
  Arguments:      
     val [in] number to be converted
     buf [in]  char buffer address for output of the resulting string
     pos [in]  byte count (pos = 1 means buf [0]) as starting position
               of the string
     len [in]  output field length, the output string is stored left
               justified between buf [pos - 1] and buf [pos + len - 2]
     rlen[out] number of characters resulting from the conversion,
               rlen = len means success,
               rlen < 0 indicates a conversion error returned by
                      sprintf, buf is left unchanged, *res is set to
                      num_invalid,
               if rlen > len, only the leftmost len characters are
                      stored, res is set to num_overflow,
     res [out] return status, num_ok indicates successful completion,
               num_invalid a conversion error returned by sscanf

  return value: void
*/ 
void s45r4tos (val, buf, pos, len, rlen, res)
        tsp00_Shortreal val;
        char *buf;
        int pos, len;
        int *rlen;
        char *res;
{
        s45ftos ((tsp00_Longreal) val, buf, pos, len, FLO4_PREC, rlen, res);
}

/*!
  Function:     s45r8tos

  see also: 

  Description:  Conversion from a double number to its character 
                representation.

  The conversion is performed using the g conversion specifier, i.e. the
  representation switches between f and e conversion depending on the
  specified precision and the value.
    
  Arguments:      
     val [in] number to be converted
     buf [in]  char buffer address for output of the resulting string
     pos [in]  byte count (pos = 1 means buf [0]) as starting position
               of the string
     len [in]  output field length, the output string is stored left
               justified between buf [pos - 1] and buf [pos + len - 2]
     rlen[out] number of characters resulting from the conversion,
               rlen = len means success,
               rlen < 0 indicates a conversion error returned by
                      sprintf, buf is left unchanged, *res is set to
                      num_invalid,
               if rlen > len, only the leftmost len characters are
                      stored, res is set to num_overflow,
     res [out] return status, num_ok indicates successful completion,
               num_invalid a conversion error returned by sscanf

  return value: void
*/ 
void s45r8tos (val, buf, pos, len, rlen, res)
        tsp00_Longreal val;
        char *buf;
        int pos, len;
        int *rlen;
        char *res;
{
        s45ftos (val, buf, pos, len, FLO8_PREC, rlen, res);
}

/*!
  Function:     s45stoi2

  see also: 

  Description:  Conversion from a character string representing a number to its
                corresponding short integer representation.

  Arguments:      
     val [out] converted number of the specified type
     buf [in]  character buffer address
     pos [in]  byte count (pos = 1 means buf [0]) as starting position
               of the string
     len [in]  maximum field length, the input string is stored
               between buf [pos - 1] and buf [pos + len - 2] with
               possibly preceding and trailing whitespace characters
     rlen[out] number of characters resulting from the conversion,
               rlen = len means success,
               rlen < 0 indicates a conversion error returned by
                      sprintf, buf is left unchanged, *res is set to
                      num_invalid,
               if rlen > len, only the leftmost len characters are
                      stored, res is set to num_overflow,
     res [out] return status, num_ok indicates successful completion,
               num_invalid a conversion error returned by sscanf

  return value: void
*/ 
void s45stoi2 (val, buf, pos, len, res)
	tsp00_Int2 *val;
        const char *buf;
        int pos, len;
        char *res;
{
        char fmt [INT4_DIG + 2];
        pos--;
 
        while ((len > 0) ? isspace (buf [pos]) : 0)
        {
                pos++;
                len--;
        }
        if (len > 0)
        {
                sprintf (fmt, "%c%dhd", '%', len);
                *res = (sscanf (&buf [pos], fmt, val) == 1) ?
                        0 /* num_ok */ : 3 /* num_invalid */;
        }
        else
        {
                *val = 0;
                *res = 0; /* num_ok */
        }
}

/*!
  Function:     s45stou2

  see also: 

  Description:  Conversion from a character string representing a number to its
                corresponding unsigned short integer representation.

  Arguments:      
     val [out] converted number of the specified type
     buf [in]  character buffer address
     pos [in]  byte count (pos = 1 means buf [0]) as starting position
               of the string
     len [in]  maximum field length, the input string is stored
               between buf [pos - 1] and buf [pos + len - 2] with
               possibly preceding and trailing whitespace characters
     rlen[out] number of characters resulting from the conversion,
               rlen = len means success,
               rlen < 0 indicates a conversion error returned by
                      sprintf, buf is left unchanged, *res is set to
                      num_invalid,
               if rlen > len, only the leftmost len characters are
                      stored, res is set to num_overflow,
     res [out] return status, num_ok indicates successful completion,
               num_invalid a conversion error returned by sscanf

  return value: void
*/ 
void s45stou2 (val, buf, pos, len, res)
        tsp00_Uint2 *val;
        const char *buf;
        int pos, len;
        char *res;
{
        char fmt [INT4_DIG + 2];
        pos--;
 
        while ((len > 0) ? isspace (buf [pos]) : 0)
        {
                pos++;
                len--;
        }
        if (len > 0)
        {
                sprintf (fmt, "%c%dhu", '%', len);
                *res = (sscanf (&buf [pos], fmt, val) == 1) ?
                        0 /* num_ok */ : 3 /* num_invalid */;
        }
        else
        {
                *val = 0;
                *res = 0; /* num_ok */
        }
}

/*!
  Function:     s45stoi4

  see also: 

  Description:  Conversion from a character string representing a number to its
                corresponding integer representation.

  Arguments:      
     val [out] converted number of the specified type
     buf [in]  character buffer address
     pos [in]  byte count (pos = 1 means buf [0]) as starting position
               of the string
     len [in]  maximum field length, the input string is stored
               between buf [pos - 1] and buf [pos + len - 2] with
               possibly preceding and trailing whitespace characters
     rlen[out] number of characters resulting from the conversion,
               rlen = len means success,
               rlen < 0 indicates a conversion error returned by
                      sprintf, buf is left unchanged, *res is set to
                      num_invalid,
               if rlen > len, only the leftmost len characters are
                      stored, res is set to num_overflow,
     res [out] return status, num_ok indicates successful completion,
               num_invalid a conversion error returned by sscanf

  return value: void
*/ 
void s45stoi4 (val, buf, pos, len, res)
	tsp00_Int4 *val;
        const char *buf;
        int pos, len;
        char *res;
{
        char fmt [INT4_DIG + 2];
        pos--;
 
        while ((len > 0) ? isspace (buf [pos]) : 0)
        {
                pos++;
                len--;
        }
        if (len > 0)
        {
                sprintf (fmt, "%c%dd", '%', len);
                *res = (sscanf (&buf [pos], fmt, val) == 1) ?
                        0 /* num_ok */ : 3 /* num_invalid */;
        }
        else
        {
                *val = 0;
                *res = 0; /* num_ok */
        }
}
 
/*!
  Function:     s45stoi8

  see also: 

  Description:  Conversion from a character string representing a number to its
                corresponding long integer representation.

  Arguments:      
     val [out] converted number of the specified type
     buf [in]  character buffer address
     pos [in]  byte count (pos = 1 means buf [0]) as starting position
               of the string
     len [in]  maximum field length, the input string is stored
               between buf [pos - 1] and buf [pos + len - 2] with
               possibly preceding and trailing whitespace characters
     rlen[out] number of characters resulting from the conversion,
               rlen = len means success,
               rlen < 0 indicates a conversion error returned by
                      sprintf, buf is left unchanged, *res is set to
                      num_invalid,
               if rlen > len, only the leftmost len characters are
                      stored, res is set to num_overflow,
     res [out] return status, num_ok indicates successful completion,
               num_invalid a conversion error returned by sscanf

  return value: void
*/ 
void s45stoi8 (val, buf, pos, len, res)
	tsp00_Longint *val;
        const char *buf;
        int pos, len;
        char *res;
{
        char fmt [INT8_DIG + 2];
        pos--;
 
        while ((len > 0) ? isspace (buf [pos]) : 0)
        {
                pos++;
                len--;
        }
        if (len > 0)
        {
                sprintf (fmt, "%c%dld", '%', len);
                *res = (sscanf (&buf [pos], fmt, val) == 1) ?
                        0 /* num_ok */ : 3 /* num_invalid */;
        }
        else
        {
                *val = 0;
                *res = 0; /* num_ok */
        }
}
 
/*!
  Function:     s45stou4

  see also: 

  Description:  Conversion from a character string representing a number to its
                corresponding unsigned integer representation.

  Arguments:      
     val [out] converted number of the specified type
     buf [in]  character buffer address
     pos [in]  byte count (pos = 1 means buf [0]) as starting position
               of the string
     len [in]  maximum field length, the input string is stored
               between buf [pos - 1] and buf [pos + len - 2] with
               possibly preceding and trailing whitespace characters
     rlen[out] number of characters resulting from the conversion,
               rlen = len means success,
               rlen < 0 indicates a conversion error returned by
                      sprintf, buf is left unchanged, *res is set to
                      num_invalid,
               if rlen > len, only the leftmost len characters are
                      stored, res is set to num_overflow,
     res [out] return status, num_ok indicates successful completion,
               num_invalid a conversion error returned by sscanf

  return value: void
*/ 
void s45stou4 (val, buf, pos, len, res)
        tsp00_Uint4 *val;
        const char *buf;
        int pos, len;
        char *res;
{
        char fmt [INT4_DIG + 2];
        pos--;
 
        while ((len > 0) ? isspace (buf [pos]) : 0)
        {
                pos++;
                len--;
        }
        if (len > 0)
        {
                sprintf (fmt, "%c%dlu", '%', len);
                *res = (sscanf (&buf [pos], fmt, val) == 1) ?
                        0 /* num_ok */ : 3 /* num_invalid */;
        }
        else
        {
                *val = 0;
                *res = 0; /* num_ok */
        }
}
 
/*!
  Function:     s45stor4

  see also: 

  Description:  Conversion from a character string representing a number to its
                corresponding float representation.

  Arguments:      
     val [out] converted number of the specified type
     buf [in]  character buffer address
     pos [in]  byte count (pos = 1 means buf [0]) as starting position
               of the string
     len [in]  maximum field length, the input string is stored
               between buf [pos - 1] and buf [pos + len - 2] with
               possibly preceding and trailing whitespace characters
     rlen[out] number of characters resulting from the conversion,
               rlen = len means success,
               rlen < 0 indicates a conversion error returned by
                      sprintf, buf is left unchanged, *res is set to
                      num_invalid,
               if rlen > len, only the leftmost len characters are
                      stored, res is set to num_overflow,
     res [out] return status, num_ok indicates successful completion,
               num_invalid a conversion error returned by sscanf

  return value: void
*/ 
void s45stor4 (val, buf, pos, len, res)
        tsp00_Shortreal *val;
        const char *buf;
        int pos, len;
        char *res;
{
        char fmt [INT4_DIG + 2];
        pos--;
 
        while ((len > 0) ? isspace (buf [pos]) : 0)
        {
                pos++;
                len--;
        }
        if (len > 0)
        {
                sprintf (fmt, "%c%df", '%', len);
                *res = (sscanf (&buf [pos], fmt, val) == 1) ?
                        0 /* num_ok */ : 3 /* num_invalid */;
        }
        else
        {
                *val = (tsp00_Shortreal)0;
                *res = 0; /* num_ok */
        }
}

/*!
  Function:     s45stor8

  see also: 

  Description:  Conversion from a character string representing a number to its
                corresponding double representation.

  Arguments:      
     val [out] converted number of the specified type
     buf [in]  character buffer address
     pos [in]  byte count (pos = 1 means buf [0]) as starting position
               of the string
     len [in]  maximum field length, the input string is stored
               between buf [pos - 1] and buf [pos + len - 2] with
               possibly preceding and trailing whitespace characters
     rlen[out] number of characters resulting from the conversion,
               rlen = len means success,
               rlen < 0 indicates a conversion error returned by
                      sprintf, buf is left unchanged, *res is set to
                      num_invalid,
               if rlen > len, only the leftmost len characters are
                      stored, res is set to num_overflow,
     res [out] return status, num_ok indicates successful completion,
               num_invalid a conversion error returned by sscanf

  return value: void
*/ 
void s45stor8 (val, buf, pos, len, res)
        tsp00_Longreal *val;
        const char *buf;
        int pos, len;
        char *res;
{
        char fmt [INT4_DIG + 2];
        pos--;
 
        while ((len > 0) ? isspace (buf [pos]) : 0)
        {
                pos++;
                len--;
        }
        if (len > 0)
        {
                sprintf (fmt, "%c%dlf", '%', len);
                *res = (sscanf (&buf [pos], fmt, val) == 1) ?
                        0 /* num_ok */ : 3 /* num_invalid */;
        }
        else
        {
                *val = 0;
                *res = 0; /* num_ok */
        }
}
 
/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
