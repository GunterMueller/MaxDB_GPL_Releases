/*!**************************************************************************

  module      : MyHeader.hpp

  -------------------------------------------------------------------------

  responsible : MarcoP

  special area:
  description : Conversion between character and various numerical representations.

  last changed: 2000-01-03
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

#ifndef HSP45_H
#define HSP45_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "gsp00.h" 
#include "hsp77.h" 

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
typedef const tsp77encoding* tsp45_StringEncoding; 

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/
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
void s45stoi2 (tsp00_Int2*    val, const char* buf, int pos, int len, char* res);

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
void s45stou2 (tsp00_Uint2*   val, const char* buf, int pos, int len, char* res);

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
void s45stoi4 (tsp00_Int4*    val, const char* buf, int pos, int len, char* res);

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
void s45stoi8 (tsp00_Longint* val, const char* buf, int pos, int len, char* res);

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
void s45stou4 (tsp00_Uint4*   val, const char* buf, int pos, int len, char* res);

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
void s45stor4 (float*       val, const char* buf, int pos, int len, char* res);

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
void s45stor8 (double*      val, const char* buf, int pos, int len, char* res);

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
void s45i2tos (tsp00_Int2     val, char* buf, int pos, int len, int* rlen, char* res);

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
void s45u2tos (tsp00_Uint2    val, char* buf, int pos, int len, int* rlen, char* res);

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
void s45i4tos (tsp00_Int4     val, char* buf, int pos, int len, int* rlen, char* res);

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
void s45i8tos (tsp00_Longint  val, char* buf, int pos, int len, int* rlen, char* res);

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
void s45u4tos (tsp00_Uint4    val, char* buf, int pos, int len, int* rlen, char* res);

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
void s45r4tos (double       val, char* buf, int pos, int len, int* rlen, char* res);

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
void s45r8tos (double       val, char* buf, int pos, int len, int* rlen, char* res);

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
void sp45cStringtoShortInt (tsp00_Int2 *val, const char *buf, int startpos, int len, char *res, tsp45_StringEncoding encType);

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
void sp45cStringtoInt (tsp00_Int4 *val, const char *buf, int startpos, int len, char *res, tsp45_StringEncoding encType);

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
void sp45cStringtoLong (tsp00_Longint *val, const char *buf, int startpos, int len, char *res, tsp45_StringEncoding encType);

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
void sp45cStringtoUShort (tsp00_Uint2 *val, const char *buf, int startpos, int len, char *res, tsp45_StringEncoding encType);

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
void sp45cStringtoUInt (tsp00_Uint4 *val, const char *buf, int startpos, int len, char *res, tsp45_StringEncoding encType);

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
void sp45cStringtoFloat (tsp00_Shortreal *val, const char *buf, int startpos, int len, char *res, tsp45_StringEncoding encType);

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
void sp45cStringtoDouble (tsp00_Longreal *val, const char *buf, int startpos, int len, char *res, tsp45_StringEncoding encType);

#endif  /* HSP45_H */
