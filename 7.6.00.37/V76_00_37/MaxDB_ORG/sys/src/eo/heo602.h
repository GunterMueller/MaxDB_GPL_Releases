/*!
  @file           heo602.h
  @author         JoegM
  @special area   Unicode Common LZU Extensions
  @brief          Contains all unicode extensions shared by kernel and client LZU
  @first created  000-08-04  20:08
  @see            example.html ...

\if EMIT_LICENCE

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



\endif
*/



#ifndef HEO602_H
#define HEO602_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "hsp78_0.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  STRUCTURES, TYPES, UNIONS ...                                            *
 *===========================================================================*/


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

/*!
   @Description    Convert a given PascalString into UTF8


  PascalString is padded with spaces up to msgTextSizeInBytes.
  The specified encoding is used, to select the conversion routine.
  Convertion result is returned. Even on error, the result buffer is
  always zero terminated!!!
   @param          msgEncoding [in] The given encoding on msgTextPascal
   @param          msgTextPascal [in] The space padded string (not zero terminated)
   @param          msgTextSizeInBytes [in] The number of bytes in msgTextPascal
   @param          bufferUTF8 [out] The target buffer
   @param          bufferSizeInBytes [in] The target buffer size
   @Return value   The convertion result

 */

externC tsp78ConversionResult eo602_ConvertPascalStringToUTF8Unicode (
         tsp00_Int4  msgEnconding ,
   const tsp00_Byte *msgTextPascal ,
         tsp00_Uint4 msgTextSizeInBytes ,
         tsp00_Byte *bufferUTF8 ,
         tsp00_Uint4 bufferSizeInBytes );

/*!
   @Description    Retrieve environment encoding


  This call returns the encoding. If local variable currentEncoding 
  was already set, it is simply returned. If not its value is taken
  from the result of a call to setlocale(LC_CTYPE, NULL).
  XXX This function assumes that the locale is setup correctly... XXX
  XXX On WindowsNT alwas sp77encodingUCS2Native is returned XXX
  XXX This function does not assume to be compiled in a Unicode style... XXX
   @Return value   The current encoding

 */

externC const tsp77encoding * eo602getConsoleEncoding();

/*!
   @Description    Set start locale to some standard value


  Tries to setup predefined UTF8 locale for UNIX and the english locale for NT.
  This call should be called during initialization sequence once.
  Subsequent calls to eo602setStartLocale have no effect.

 */

externC void eo602setStartLocale();

#endif  /* HEO602_H */
