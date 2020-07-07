/*! 
  -----------------------------------------------------------------------------
 
  module: vcn13.cpp
 
  -----------------------------------------------------------------------------
 
  responsible:  

  special area: UTF8-Output
 
  description:  API for reading/writing UTF8 from/to stdin/stdout/stderr

  version:      7.3.

  -----------------------------------------------------------------------------
 
  Copyright (c) 1998-2005 SAP AG
 
  -----------------------------------------------------------------------------


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

/* 
  ------------------------------------------------------------------
  includes
  ------------------------------------------------------------------
 */
#include <string.h>
#include "SAPDBCommon/SAPDB_string.h"
#include "SAPDBCommon/SAPDB_UTF8Basis.hpp"
#include "hcn13.h"

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
    int          nSource )
{
  SAPDB_UTF8Basis::UTF8ConstPointer srcBeg  = (SAPDB_UTF8Basis::UTF8ConstPointer) pSource;
  SAPDB_UTF8Basis::UTF8ConstPointer srcEnd  = (SAPDB_UTF8Basis::UTF8ConstPointer) (pSource + nSource);
  SAPDB_UTF8Basis::UTF8ConstPointer srcAt   = NULL;
  SAPDB_UTF8Basis::UCS2Pointer      destBeg = (SAPDB_UTF8Basis::UCS2Pointer)      pTarget;
  SAPDB_UTF8Basis::UCS2ConstPointer destEnd = (SAPDB_UTF8Basis::UCS2Pointer)      (pTarget + (nSource * 2));
  SAPDB_UTF8Basis::UCS2Pointer      destAt  = NULL;

  SAPDB_UTF8Basis::ConvertToUCS2 (srcBeg, srcEnd, srcAt, destBeg, destEnd, destAt);

  pTarget[((char *) destAt) - pTarget]     = 0;
  pTarget[((char *) destAt) - pTarget + 1] = 0;

  return (int) ( ((char *) destAt) - pTarget);
} // end cn13MemcpyUTF8ToUCS2

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
    int          nSource )
{
  SAPDB_UTF8Basis::UTF8ConstPointer  srcBeg  = (SAPDB_UTF8Basis::UTF8ConstPointer) pSource;
  SAPDB_UTF8Basis::UTF8ConstPointer  srcEnd  = (SAPDB_UTF8Basis::UTF8ConstPointer) (pSource + nSource);
  SAPDB_UTF8Basis::UTF8ConstPointer  srcAt   = NULL;
  SAPDB_UTF8Basis::ASCIIPointer      destBeg = (SAPDB_UTF8Basis::ASCIIPointer)     pTarget;
  SAPDB_UTF8Basis::ASCIIConstPointer destEnd = (SAPDB_UTF8Basis::ASCIIPointer)     (pTarget + (nSource * 2));
  SAPDB_UTF8Basis::ASCIIPointer      destAt  = NULL;
  SAPDB_UInt                         nCount;

  SAPDB_UTF8Basis::ConvertToASCII (srcBeg, srcEnd, srcAt, destBeg, destEnd, destAt, '?', nCount);

  pTarget[((char *) destAt) - pTarget]     = 0;
  //pTarget[((char *) destAt) - pTarget + 1] = 0;

  return (int) ( ((char *) destAt) - pTarget);
} // end cn13MemcpyUTF8ToASCII

/*!
  -----------------------------------------------------------------------------
  function:     cn13MemcpyUTF8ToASCIIAcp
  -----------------------------------------------------------------------------

  description:  converts a UTF8-encoded buffer to ASCII-encoding

  -----------------------------------------------------------------------------
 */
void cn13MemcpyUTF8ToASCIIAcp(
    char       * pTarget, 
    const char * pSource )
{
#ifdef _WIN32

  wchar_t * pBufferU = new wchar_t[strlen(pSource) + 1];

  MultiByteToWideChar(CP_UTF8, 0, pSource, -1, pBufferU, ((int) strlen(pSource) + 1) * sizeof(wchar_t));
  WideCharToMultiByte(GetACP(), 0, pBufferU, -1, pTarget, (int) strlen(pSource) + 1, NULL, NULL);

  delete [] pBufferU;

#else

  SAPDB_strcpy(pTarget, pSource);

#endif

} // end cn13MemcpyUTF8ToASCIIAcp

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
    int          nSource )
{
  SAPDB_UTF8Basis::UCS2ConstPointer srcBeg  = (SAPDB_UTF8Basis::UCS2ConstPointer) pSource;
  SAPDB_UTF8Basis::UCS2ConstPointer srcEnd  = (SAPDB_UTF8Basis::UCS2ConstPointer) (pSource + nSource);
  SAPDB_UTF8Basis::UCS2ConstPointer srcAt   = NULL;
  SAPDB_UTF8Basis::UTF8Pointer      destBeg = (SAPDB_UTF8Basis::UTF8Pointer)      pTarget;
  SAPDB_UTF8Basis::UTF8ConstPointer destEnd = (SAPDB_UTF8Basis::UTF8ConstPointer) (pTarget + (nSource * 2));
  SAPDB_UTF8Basis::UTF8Pointer      destAt  = NULL;    

  SAPDB_UTF8Basis::ConvertFromUCS2(srcBeg, srcEnd, srcAt, destBeg, destEnd, destAt);

  pTarget[((char *) destAt) - pTarget]     = 0;

  return (int) ( ((char *) destAt) - pTarget);
} // end cn13MemcpyUCS2ToUTF8

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
    int          nSource )
{
  SAPDB_UTF8Basis::ASCIIConstPointer srcBeg  = (SAPDB_UTF8Basis::ASCIIConstPointer) pSource;
  SAPDB_UTF8Basis::ASCIIConstPointer srcEnd  = (SAPDB_UTF8Basis::ASCIIConstPointer) (pSource + nSource);
  SAPDB_UTF8Basis::ASCIIConstPointer srcAt   = NULL;
  SAPDB_UTF8Basis::UTF8Pointer      destBeg  = (SAPDB_UTF8Basis::UTF8Pointer)       pTarget;
  SAPDB_UTF8Basis::UTF8ConstPointer destEnd  = (SAPDB_UTF8Basis::UTF8ConstPointer)  (pTarget + (nSource * 2));
  SAPDB_UTF8Basis::UTF8Pointer      destAt   = NULL;    

  SAPDB_UTF8Basis::ConvertFromASCII(srcBeg, srcEnd, srcAt, destBeg, destEnd, destAt);

  pTarget[((char *) destAt) - pTarget]     = 0;

  return (int) ( ((char *) destAt) - pTarget);
} // end cn13MemcpyASCIIToUTF8

/* 
  -----------------------------------------------------------------------------
  function: cn13fputs
  -----------------------------------------------------------------------------
*/
int cn13fputs
      ( FILE       *  fileHandle,
        const char *  pString,
        bool          bUnicode,
        int           nLength )
{
  int    nReturn;
  char   szBuffer[1000];
  char * pBuffer = &szBuffer[0];
  bool   bNew    = false;

  // If nLength specified, we can't assume it is a null terminated string.
  // So we have to add the null byte.
  if (nLength == -1) {
    nLength = (int) strlen(pString);
  } // end if
  if ((nLength + 1) > 1000) {
    pBuffer = new char [nLength + 1];
    bNew = true;
  } // end if
  strncpy(pBuffer, pString, nLength);
  pBuffer[nLength] = 0;

#ifdef _WIN32
  
  if ((fileHandle == stdout || fileHandle == stderr) && bUnicode) {
    wchar_t * pBufferU = new wchar_t[strlen(pBuffer) + 1];

    cn13MemcpyUTF8ToUCS2((char *) pBufferU, pBuffer, (int) strlen(pBuffer));

    WideCharToMultiByte(GetConsoleCP(), 0, pBufferU, -1, pBuffer, (int) strlen(pBuffer) + 1, NULL, NULL);

    delete [] pBufferU;
  } else if ((fileHandle == stdout || fileHandle == stderr)) {

    wchar_t * pBufferU = new wchar_t[strlen(pBuffer) + 1];

    MultiByteToWideChar(GetACP(), 0, pBuffer, -1, pBufferU, ((int) strlen(pBuffer) + 1) * sizeof(wchar_t));
    WideCharToMultiByte(GetConsoleCP(), 0, pBufferU, -1, pBuffer, (int) strlen(pBuffer) + 1, NULL, NULL);

    delete [] pBufferU;
  } // end if

#else

  if ((fileHandle == stdout || fileHandle == stderr) && bUnicode) {
    char * pTemp = new char[strlen(pBuffer) + 1];
    cn13MemcpyUTF8ToASCII(pTemp, pBuffer, (int) strlen(pBuffer));
    SAPDB_strcpy(pBuffer, pTemp);
    delete [] pTemp;
  } // end if

#endif

  nReturn = fputs(pBuffer, fileHandle);
  fflush(fileHandle);

  if (bNew) {
    delete [] pBuffer;
  } // end if

  return nReturn;
} // end cn13fputs

/* 
  -----------------------------------------------------------------------------
  function: cn13fgets
  -----------------------------------------------------------------------------
*/
char * cn13fgets
      ( char       *  pBuffer,
        int           nLength,
        FILE       *  fileHandle,
        bool          bUnicode )
{
  char * pReturn = NULL;  

  pReturn = fgets(pBuffer, nLength, fileHandle);

#ifdef _WIN32

  if ((pReturn != NULL) && (fileHandle == stdin) && bUnicode) {
    wchar_t * pBufferU = new wchar_t[nLength + 1];

    MultiByteToWideChar(GetConsoleCP(), 0, pBuffer, -1, pBufferU, (nLength + 1) * sizeof(wchar_t));
    cn13MemcpyUCS2ToUTF8(pBuffer, (char *) pBufferU, (int) wcslen(pBufferU) * 2);

    delete [] pBufferU;
  } else if ((pReturn != NULL) && (fileHandle == stdin)) {

    wchar_t * pBufferU = new wchar_t[strlen(pBuffer) + 1];

    MultiByteToWideChar(GetConsoleCP(), 0, pBuffer, -1, pBufferU, ((int) strlen(pBuffer) + 1) * sizeof(wchar_t));
    WideCharToMultiByte(GetACP(), 0, pBufferU, -1, pBuffer, (int) strlen(pBuffer) + 1, NULL, NULL);

    delete [] pBufferU;
  } // end if

#else

  if ((pReturn != NULL) && (fileHandle == stdin) && bUnicode) {
    char * pTemp = new char[nLength + 1];
    cn13MemcpyASCIIToUTF8(pTemp, pBuffer, strlen(pBuffer));
    SAPDB_strcpy(pBuffer, pTemp);
    delete [] pTemp;
  } // end if

#endif

  return pReturn;
} // end cn13fgets

/* 
  -----------------------------------------------------------------------------
  function: cn13fopen
  -----------------------------------------------------------------------------
*/
FILE * cn13fopen
      ( const char * pFile,
        const char * pMode)
{
  FILE *  hFile; 

#ifdef _WIN32
  
  wchar_t * pFileU = new wchar_t[strlen(pFile) + 1];
  wchar_t * pModeU = new wchar_t[strlen(pMode)  + 1];

  cn13MemcpyUTF8ToUCS2((char *) pFileU, pFile, (int) strlen(pFile));
  cn13MemcpyUTF8ToUCS2((char *) pModeU, pMode, (int) strlen(pMode));

  hFile = _wfopen(pFileU, pModeU);

  delete [] pFileU;
  delete [] pModeU;

#else
  
  hFile = fopen(pFile, pMode);
  
#endif

  return hFile;
} // end cn13fopen

