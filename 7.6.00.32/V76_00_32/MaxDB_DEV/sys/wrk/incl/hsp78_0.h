/*!**************************************************************************

  module       : hsp78.h

  -------------------------------------------------------------------------

  responsible : JoergM

  special area: String Convertion
  description : Function accessing string convertion between Ascii/UCS2 and UTF8

  last changed: 2000-09-04  22:14
  first created:2000-08-23  18:38

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


#ifndef HSP78_H
#define HSP78_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "hsp77.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  STRUCTURES, TYPES, UNIONS ...                                            *
 *===========================================================================*/
/*!
  Declaration: tsp78ConversionResult
  Description: Conversion result codes

  sp78_Ok - Conversion sucessfull

  sp78_SourceExhausted - last character only partial in source buffer

  sp78_SourceCorrupted - source contains invalid chars or char sequences

  sp78_TargetExhausted - not enaugh room in target for conversion

  sp78_TargetNotTerminated - failed to add zero termination bytes

  sp78_BadSourceEncoding - unsupported source encoding

  sp78_BadTargetEncoding - unsupported destination encoding

  sp78_NullArgument - NULL pointer parsed
 */
typedef enum t_sp78ConversionResult
{
  sp78_Ok,              /* Conversion sucessfull */
  sp78_SourceExhausted, /* last character only partial in source buffer */
  sp78_SourceCorrupted, /* source contains invalid chars or char sequences */
  sp78_TargetExhausted, /* not enaugh room in target for conversion */
  sp78_TargetNotTerminated, /* failed to add zero termination bytes */
  sp78_BadSourceEncoding, /* unsupported source encoding */
  sp78_BadTargetEncoding, /* unsupported destination encoding */
  sp78_NullArgument       /* NULL pointer parsed */
} tsp78ConversionResult;
/*! EndDeclaration */

/*!
  Declaration: tsp78ConverterFunc
  Description: Conversion function pointer

  Converts srcBuffer into destBuffer unsig srcEncoding and destEncoding.
  Length of srcBufferInBytes is used ignoring any terminator characters in buffer.
  Length of destBufferInBytes is used to limit conversion if needed.
  The actual number of bytes parsed is returned in srcBytesParsed.
  The actual number of bytes written is returned in destBytesWritten.

  Possible results are sp78_Ok, sp78_SourceExhausted, sp78_SourceCorrupted and
  sp78_TargetExhausted. See tsp78ConversionResult for meaning of result codes.
 */
#if defined(__cplusplus)
extern "C" {
#endif
typedef tsp78ConversionResult (*tsp78ConvertFunc) (
 const tsp77encoding *srcEncoding,
 const void    *srcBuffer,
 tsp00_Uint4    srcLengthInBytes,
 tsp00_Uint4   *srcBytesParsed,
 const tsp77encoding *destEncoding,
 void *         destBuffer,
 tsp00_Uint4    destBufferLengthInBytes,
 tsp00_Uint4   *destBytesWritten);
/*! EndDeclaration */

/*!
  Declaration: sp78Converter
  Description: Array of converter function pointer

  First index is source , second index is destination.
 */
extern tsp78ConvertFunc sp78Converter[6][6];

#if defined(__cplusplus)
};
#endif
/*! EndDeclaration */

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/
/*!
  Function: sp78convertString

  Description: String conversion function

  This call should be used instead of 'sp77sprintfUnicode' whenever only a conversion is needed
  and the number of bytes to be converted is known exactly. It returns more explicit error return value.

  Termination characters in the input buffer are converted like all other characters.
  But if addZeroTerminator is true, zero termination character is added if needed (that means last
  character is not already a zero terminator) and destBytesWritten modified according to it.
  If zero termination was not possible sp78_TargetNotTerminated is returned.

  Arguments: destBuffer [out] Destination buffer
             destBufferLengthInBytes [in] Destination buffer length in bytes, not characters
             destBytesWritten [out] Number of destination bytes written (including optional zeros)
             addZeroTerminator [in] Flag to force zero terminated output
             srcEncoding [in] Encoding of source buffer
             srcBuffer [in] Source buffer
             srcBufferLengthInBytes [in] Source buffer length in bytes, not characters
             srcBytesParsed [out] Number of source bytes parsed

  Return value: Conversion result of type tsp78ConversionResult

  destBytesWritten and srcBytesParsed are valid even if sp78_Ok was not returned.
 */
externC tsp78ConversionResult sp78convertString(
 const tsp77encoding *destEncoding,
 void *         destBuffer,
 tsp00_Uint4    destBufferLengthInBytes,
 tsp00_Uint4   *destBytesWritten,
 tsp00_Bool     addZeroTerminator,
 const tsp77encoding *srcEncoding,
 const void    *srcBuffer,
 tsp00_Uint4    srcBufferLengthInBytes,
 tsp00_Uint4   *srcBytesParsed);

/*!
  Function: sp78convertBuffer

  Description: Buffer conversion function

  Identical to sp78convertString if addZeroTerminator is false.

  Arguments: destBuffer [out] Destination buffer
             destBufferLengthInBytes [in] Destination buffer length in bytes, not characters
             destBytesWritten [out] Number of destination bytes written (including optional zeros)
             srcEncoding [in] Encoding of source buffer
             srcBuffer [in] Source buffer
             srcBufferLengthInBytes [in] Source buffer length in bytes, not characters
             srcBytesParsed [out] Number of source bytes parsed

  Return value: Conversion result of type tsp78ConversionResult

  destBytesWritten and srcBytesParsed are valid even if sp78_Ok was not returned.
 */
externC tsp78ConversionResult sp78convertBuffer(
 const tsp77encoding *destEncoding,
 void *         destBuffer,
 tsp00_Uint4    destBufferLengthInBytes,
 tsp00_Uint4   *destBytesWritten,
 const tsp77encoding *srcEncoding,
 const void    *srcBuffer,
 tsp00_Uint4    srcBufferLengthInBytes,
 tsp00_Uint4   *srcBytesParsed);

/*!
  Function: sp78getConvertFuncIndex

  Description: Map encoding to convert function index

  Arguments: encoding [in] Encoding to use

  Return value: Index to be used for referencing convert function in sp78convert array
 */
externC int sp78getConvertFuncIndex( const tsp77encoding *encoding );

/*!
  Function: sp78errString

  returns: a string representation of a tsp78ConversionResult
 */
externC const char * sp78errString (tsp78ConversionResult convResult);

#define csp78_ucs4      23
#define csp78_ucs4_swap 24

#endif  /* HSP78_H */
