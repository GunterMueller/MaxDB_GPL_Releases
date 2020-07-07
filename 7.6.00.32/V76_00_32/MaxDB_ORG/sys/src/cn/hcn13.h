/*


    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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




*/
#ifdef _WIN32
  #line 3 "hcn13.h"
#endif 
/*! 
  -----------------------------------------------------------------------------
 
  module: hcn13.h
 
  -----------------------------------------------------------------------------
 
  responsible:  

  special area: UTF8-Output
 
  description:  API for reading/writing UTF8 from/to stdin/stdout/stderr

  version:      7.3.

  -----------------------------------------------------------------------------
 
  Copyright (c) 1998-2005 SAP AG
 
  -----------------------------------------------------------------------------
*/

#ifndef _HCN13_H_
#define _HCN13_H_

#ifndef __cplusplus
  #error "This module defines only C++ functions"
#endif

/* 
  ------------------------------------------------------------------
  includes
  ------------------------------------------------------------------
 */
#include <stdio.h>

/*!
  -----------------------------------------------------------------------------
 
  function:     cn13fputs

  -----------------------------------------------------------------------------

  description:  implements a ANSI fputs

                If there nLength is specified, a null terminator is added to
                the buffer at position nLength.
  
  arguments:    fileHandle [IN] - filehandle
                pString    [IN] - The string in UTF8 encoding.
                bUnicode   [IN] - there is no conversion if this flag is false
                nLength    [IN] - The length of the string. If the string is 
                                  nullterminated you can specify -1 and the function 
                                  will compute the length by strlen.

  returnvalue:  > 0 on succes or EOF on error (see fputs)
  
  -----------------------------------------------------------------------------
*/
int cn13fputs
      ( FILE       *  fileHandle,
        const char *  pString,
        bool          bUnicode,
        int           nLength = -1 );

/*!
  -----------------------------------------------------------------------------
 
  function:     cn13fgets

  -----------------------------------------------------------------------------

  description:  implements a ANSI fgets

  arguments:    pBuffer    [IN] - the buffer for the string in UTF8 encoding
                nLength    [IN] - the length of the buffer
                fileHandle [IN] - filehandle
                bUnicode   [IN] - there is no conversion if this flag is false

  returnvalue:  pBuffer or null pointer (see fgets)

  -----------------------------------------------------------------------------
*/
char * cn13fgets
      ( char       *  pBuffer,
        int           nLength,
        FILE       *  fileHandle,
        bool          bUnicode );

/*!
  -----------------------------------------------------------------------------
 
  function:     cn13fgets

  -----------------------------------------------------------------------------

  description:  implements a ANSI fopen or at _WIN32 fwopen

  arguments:    pFile    [IN] - filename in UTF8 encoding (see fopen)
                pMode    [IN] - mode in UTF8 encondig (see fopen)

  returnvalue:  filehandle or null pointer (see fopen)

  -----------------------------------------------------------------------------
*/
FILE * cn13fopen
      ( const char * pFile,
        const char * pMode);

/*!
  -----------------------------------------------------------------------------
  function:     cn13MemcpyUTF8ToUCS2
  -----------------------------------------------------------------------------

  description:  converts a UTF8-encoded buffer to UCS2-encoding

  -----------------------------------------------------------------------------
 */
int cn13MemcpyUTF8ToUCS2(
    char       * pTarget, 
    const char * pSource, 
    int          nSource );

/*!
  -----------------------------------------------------------------------------
  function:     cn13MemcpyUCS2ToUTF8
  -----------------------------------------------------------------------------

  description:  converts a UCS2-encoded buffer to UTF8-encoding

  -----------------------------------------------------------------------------
 */
int cn13MemcpyUCS2ToUTF8(
    char       * pTarget, 
    const char * pSource, 
    int          nSource );

/*!
  -----------------------------------------------------------------------------
  function:     cn13MemcpyUTF8ToASCII
  -----------------------------------------------------------------------------

  description:  converts a UTF8-encoded buffer to ASCII-encoding

  -----------------------------------------------------------------------------
 */
int cn13MemcpyUTF8ToASCII(
    char       * pTarget, 
    const char * pSource, 
    int          nSource );

/*!
  -----------------------------------------------------------------------------
  function:     cn13MemcpyASCIIToUTF8
  -----------------------------------------------------------------------------

  description:  converts a ASCII-encoded buffer to UTF8-encoding

  -----------------------------------------------------------------------------
 */
int cn13MemcpyASCIIToUTF8(
    char       * pTarget, 
    const char * pSource, 
    int          nSource );

/*!
  -----------------------------------------------------------------------------
  function:     cn13MemcpyUTF8ToASCIIAcp
  -----------------------------------------------------------------------------

  description:  converts a UTF8-encoded buffer to ASCII-encoding

  -----------------------------------------------------------------------------
 */
void cn13MemcpyUTF8ToASCIIAcp(
    char       * pTarget, 
    const char * pSource );

#endif /* _HCN13_H_ */
