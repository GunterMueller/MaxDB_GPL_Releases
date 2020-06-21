/*! ******************************************************************************

  module: vcn90.cpp

  ------------------------------------------------------------------------------

  responsible:  BerndV

  special area: Control-Server

  description:  implementation of useful utility functions
                for the cn (Control) layer

  version:      7.2. / 6.2

  ------------------------------------------------------------------------------

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


  *******************************************************************************/

/* ------------------------------------------------------------------
 * IMPORT
 * ------------------------------------------------------------------
 */
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>

#include "vsp001.h"
#include "vsp009c.h"
#include "heo06.h"
#include "heo02.h"
#include "hsp02.h"
#include "heo01.h"
#include "hcn90.h"

#include "RunTime/RTE_DBRegister.hpp"
#include "RunTime/RTE_ActiveInstallation.hpp"
#include "RunTime/RTE_ActiveDatabase.hpp"
#include "RunTime/RTE_GetDBRootOfDB.h"

#include "DBM/Srv/DBMSrv_Reply.hpp"
#include "DBM/Srv/Deprecated/DBMSrvDep_DirtyReply.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"

#include "geo00_2.h"

/* ------------------------------------------------------------------
 * SPECIFICATION PRIVATE MACROS
 * ------------------------------------------------------------------
 */

/* directories */
#define DB_CFG_DIR      "config"
#define DB_WRK_DIR      "wrk"
#define DB_ENV_DIR      "env"
#define DB_BIN_DIR      "bin"
#define ENV_DBROOT      "DBROOT"

/* so sorry, errno values aren't present on every system in the same matter, so
   that we include the usage of errno values in some ifdef's                   */
#ifdef E2BIG
  #define E2BIG_CN90     { E2BIG     , "E2BIG : argument list to long"             },
#else
  #define E2BIG_CN90
#endif

#ifdef EACCES
  #define EACCES_CN90    { EACCES    , "EACCES : permission denied"                },
#else
  #define EACCES_CN90
#endif

#ifdef EAGAIN
  #define EAGAIN_CN90    { EAGAIN    , "EAGAIN : No more processes or not enough memory or maximum nesting level reached" },
#else
  #define EAGAIN_CN90
#endif

#ifdef EBADF
  #define EBADF_CN90     { EBADF     , "EBADF : bad file number"                   },
#else
  #define EBADF_CN90
#endif

#ifdef ECHILD
  #define ECHILD_CN90    { ECHILD    , "ECHILD : no spawnde processes"             },
#else
  #define ECHILD_CN90
#endif

#ifdef EDEADLOCK
  #define EDEADLOCK_CN90 { EDEADLOCK , "EDEADLOCK : resource deadlock would error" },
#else
  #define EDEADLOCK_CN90
#endif

#ifdef EDOM
  #define EDOM_CN90      { EDOM      , "EDOM : math argument"                      },
#else
  #define EDOM_CN90
#endif

#ifdef EEXIST
  #define EEXIST_CN90    { EEXIST    , "EEXIST : file exist"                       },
#else
  #define EEXIST_CN90
#endif

#ifdef EINVAL
  #define EINVAL_CN90    { EINVAL    , "EIVAL : invalid argument"                  },
#else
  #define EINVAL_CN90
#endif

#ifdef EMFILE
  #define EMFILE_CN90    { EMFILE    , "EMFILE : too many open files"              },
#else
  #define EMFILE_CN90
#endif

#ifdef ENOENT
  #define ENOENT_CN90    { ENOENT    , "ENOENT : no such file or directory"        },
#else
  #define ENOENT_CN90
#endif

#ifdef ENOEXEC
  #define ENOEXEC_CN90   { ENOEXEC   , "ENOEXEC : exec format error"               },
#else
  #define ENOEXEC_CN90
#endif

#ifdef ENOMEM
  #define ENOMEM_CN90    { ENOMEM    , "ENOMEM : not enough memory"                },
#else
  #define ENOMEM_CN90
#endif

#ifdef ENOSPC
  #define ENOSPC_CN90    { ENOSPC    , "ENOSPC : no space left on device"          },
#else
  #define ENOSPC_CN90
#endif

#ifdef ERANGE
  #define ERANGE_CN90    { ERANGE    , "ERANGE : result too large"                 },
#else
  #define ERANGE_CN90
#endif

#ifdef EXDEV
  #define EXDEV_CN90     { EXDEV     , "EXDEV : cross-device link"                 },
#else
  #define EXDEV_CN90
#endif

#define ERRNO_TO_TEXT { \
   E2BIG_CN90           \
   EACCES_CN90          \
   EAGAIN_CN90          \
   EBADF_CN90           \
   ECHILD_CN90          \
   EDEADLOCK_CN90       \
   EDOM_CN90            \
   EEXIST_CN90          \
   EINVAL_CN90          \
   EMFILE_CN90          \
   ENOENT_CN90          \
   ENOEXEC_CN90         \
   ENOMEM_CN90          \
   ENOSPC_CN90          \
   ERANGE_CN90          \
   EXDEV_CN90           \
  { 0 , NULL       },   \
  { 0 , "unknown errno" }}

/* create dir */
#define CHAR_SLASH        '/'
#define CHAR_BACKSLASH    '\\'

/*
 * ==================================================================
 * START OF IMPLEMENTATION PUBLIC
 * ==================================================================
 */

/* PUBLIC FUNCTION cn90StrUpperCopy
 * ------------------------------------------------------------------
 */
char  * cn90StrUpperCopy
    ( char              * szDest,
      const char        * szSource,
      const bool           bKeepCase )
{
  long i = 0;

  for (i = 0; szSource[i] != CHAR_STRINGTERM_CN90; i++) {
    szDest[i] = bKeepCase ? szSource[i] : toupper(szSource[i]);
  } /* end for */

  szDest[i] = CHAR_STRINGTERM_CN90;

  return szDest;
} /* end cn90StrUpperCopy */

/* PUBLIC FUNCTION cn90StringPtoC
 * ------------------------------------------------------------------
 */
char  * cn90StringPtoC
    ( char              * szDestC,
      const char        * pSourceP,
      const int            nLength )
{
  int     i;
  bool bFound = false;

  // at first looking for zero termination in source
  for (i = 0; (i < (nLength - 1)) && (pSourceP[i] != CHAR_STRINGTERM_CN90); i++);

  if (pSourceP[i] == CHAR_STRINGTERM_CN90) {
    SAPDB_strcpy(szDestC, pSourceP);
  } else {
    szDestC[nLength] = CHAR_STRINGTERM_CN90;
    for (i = nLength - 1; i >= 0; i--) {
      if ((pSourceP[i] == CHAR_PASCALFILL_CN90) && !bFound) {
        szDestC[i] = CHAR_STRINGTERM_CN90;
      } else {
        bFound = true;
        szDestC[i] = pSourceP[i];
      } /* end if */
    } /* end while */
  } /* end while */

  return szDestC;
} /* end cn90StringPtoC */

/* PUBLIC FUNCTION cn90StringCtoP
 * ------------------------------------------------------------------
 */
char  * cn90StringCtoP
    ( char              * pDestP,
      const char        * szSourceC,
      const int            nLength )
{
  int i;

  memset(pDestP, CHAR_PASCALFILL_CN90, nLength);
  for (i = 0; ((szSourceC[i] != CHAR_STRINGTERM_CN90) && (i < nLength)); i++) {
    pDestP[i] = szSourceC[i];
  } /* end while */

  return pDestP;
} /* end cn90StringCtoP */

/* PUBLIC FUNCTION cn90StripLeadingChar
 * ------------------------------------------------------------------
 */
char  * cn90StripLeadingChar
    ( char      * szString,
      const char  cStrip )
{
  char * pSource      = szString;
  char * pDestination = szString;

  while ((*pSource == cStrip) && (*pSource != CHAR_STRINGTERM_CN90)) {
    pSource++;
  } /* end while */
  while (*pSource != CHAR_STRINGTERM_CN90) {
    *pDestination = *pSource;
    pDestination++;
    pSource++;
  } /* end while */

  *pDestination = CHAR_STRINGTERM_CN90;

  return szString;
} /* end cn90StripLeadingChar */

/* PUBLIC FUNCTION cn90StripTrailingChar
 * ------------------------------------------------------------------
 */
char  * cn90StripTrailingChar
    ( char        * szString,
      const char    cStrip )
{
  char * pSource      = &szString[strlen(szString) - 1];
  char * pDestination = &szString[strlen(szString) - 1];

  while ((*pSource == cStrip) && (pSource >= szString)) {
    *pDestination = CHAR_STRINGTERM_CN90;
    pSource--;
    pDestination--;
  } /* end while */

  return szString;
} /* end cn90StripTrailingChar */

/* PUBLIC FUNCTION cn90Strip
 * ------------------------------------------------------------------
 */
char  * cn90Strip
    ( char  * szString )
{
  return cn90StripLeadingChar(
             cn90StripTrailingChar(
                 cn90StripLeadingChar(
                     cn90StripTrailingChar(
                         szString,
                         '\t'),
                     '\t'),
                 ' '),
             ' ');
} /* end cn90Strip */

/* PUBLIC FUNCTION cn90StripQuotes
 * ------------------------------------------------------------------
 */
char  * cn90StripQuotes
    ( char  * szString )
{
  return cn90StripLeadingChar(
             cn90StripTrailingChar(
                 cn90StripLeadingChar(
                     cn90StripTrailingChar(
                         szString,
                         '\''),
                     '\''),
                 '"'),
             '"');
} /* end cn90StripQuotes */

/* PUBLIC FUNCTION cn90Stricmp
 * ------------------------------------------------------------------
 */
int  cn90Stricmp
    ( const char  * szString1,
      const char  * szString2 )
{
  int nReturn = 0;
  int nIndex  = -1;

  do {
    nIndex++;
    nReturn = toupper(szString1[nIndex]) - toupper(szString2[nIndex]);
  } while (szString1[nIndex] != CHAR_STRINGTERM_CN90 &&
           szString2[nIndex] != CHAR_STRINGTERM_CN90 &&
           nReturn           == 0);

  return nReturn;
} /* end cn90Stricmp */

/* PUBLIC FUNCTION cn90StrNicmp
 * ------------------------------------------------------------------
 */
int cn90StrNicmp(const char  * szString1,
                 const char  * szString2,
                 size_t         MaxLength)
{
    int    nReturn=0;
    size_t nIndex =0;

    while(0==nReturn &&     //not found any difference
          nIndex<MaxLength) //not reached maximal length
    {
        nReturn=toupper(szString1[nIndex]) - toupper(szString2[nIndex]);

        if(CHAR_STRINGTERM_CN90==szString1[nIndex] ||   //reached end of string1 
           CHAR_STRINGTERM_CN90==szString2[nIndex]   )  //reached end of string2
        {
            nIndex=MaxLength;   //end the loop (same as a break)
        }
        else
            nIndex++;
    }
    
    return nReturn;
} /* end cn90StrNicmp */

/* PUBLIC FUNCTION cn90CheckKeyword
 * ------------------------------------------------------------------
 */
bool cn90CheckKeyword
    ( const char  *     szLine,
      const char  *     szKeyword,
      const bool         bCaseSens)
{
  int nLength = 0;

  nLength = (int)strlen(szKeyword);

  return ( ( (bCaseSens ? strncmp(szLine,szKeyword, nLength)
                        : strnicmp(szLine,szKeyword, nLength)) == 0      ) &&
           ( ( isspace(szLine[nLength]) != 0                         ) ||
             ( szLine[nLength]          == CHAR_STRINGTERM_CN90)     )     );
} /* end cn90CheckKeyword */

/* PUBLIC FUNCTION cn90SaveStrcpy
 * ------------------------------------------------------------------
 */
char  * cn90SaveStrcpy
    ( char        * szDest,
      const char  * szSource,
      int            nLength )
{
  strncpy(szDest, szSource, nLength);
  szDest[nLength - 1] = CHAR_STRINGTERM_CN90;
  return szDest;
} /* end  cn90SaveStrcpy */

/* PRIVATE FUNCTION cn90GetToken
 * ------------------------------------------------------------------
 */
bool cn90GetToken
    ( const char        * szBuffer,
      char              * szToken,
      const long           nRequest,
      const long           nLength )
{
  long               nToken      = 0;
  long               i           = 0;
  long               j           = 0; //used for wired return statement (if somebody can explain it or even better assure, that the wired properties are not used, please call me ... TiloH)
  long               j1          = 0; //introduced for write access to szToken, we assure 0<=j1<nLength for all write operations
  bool            bOutside    = true;
  bool            bString     = false;

    if(0<nLength)        //if there is space for a terminating zero in szToken, make sure szToken is zero terminated
        szToken[nLength-1]='\0'; // all further write access is restricted to j1<nLength-1, so this zero is not overwritten

  /* is buffer valid */
  if (szBuffer != 0) {
    /* loop until we've reached the end of buffer or the requested token is complete, make sure terminating zero is not overwritten */
    for (i = 0; (szBuffer[i] != CHAR_STRINGTERM_CN90) && (nToken <= nRequest) && ((j < nLength) || (nLength == -1)); i++) {
      if (bOutside) {
        /* if we're outside of a token look out for a non white spaces */
        if (!isspace((unsigned char) szBuffer[i])) {
          bOutside = false;
          nToken++;
          /* copy the first char of the token */
          if ((szToken != NULL) && (nToken == nRequest)) { j++; if(j1<nLength-1) szToken[j1++] = szBuffer[i]; }
          /* set a flag for string handling */
          bString = szBuffer[i] == CHAR_SYNTAXSTRING_CN90;
        } /* end if */
      } else {
        if (bString) {
          /* copy the char */
          if ((szToken != NULL) && (nToken == nRequest)) { j++; if(j1<nLength-1) szToken[j1++] = szBuffer[i]; }
          /* look out for string terminator */
          if (szBuffer[i] == CHAR_SYNTAXSTRING_CN90 && isspace(szBuffer[i+1])) {
            bOutside = true;
            /* attach the null char, if there is space */
            if ((szToken != NULL) && (nToken == nRequest)) { j++; if(j1<nLength-1) szToken[j1++] = CHAR_STRINGTERM_CN90; }
          } /* end if */
        } else {
          /* look out for whitespaces */
          if (isspace((unsigned char) szBuffer[i])) {
            bOutside = true;
            /* attach the null char */
            if ((szToken != NULL) && (nToken == nRequest)) { j++; if(j1<nLength-1) szToken[j1++] = CHAR_STRINGTERM_CN90; }
          } else {
            /* copy the current char */
            if ((szToken != NULL) && (nToken == nRequest)) { j++; if(j1<nLength-1) szToken[j1++] = szBuffer[i]; }
            bString = szBuffer[i] == CHAR_SYNTAXSTRING_CN90;
          } /* end if */
        } /* end if */
      } /* end if */
    } /* end for */

    /* if the token was at the end of the buffer and there is space, attach a null char */
    if (szToken != NULL && szBuffer[i] == CHAR_STRINGTERM_CN90) { if(j1<nLength-1) szToken[j1++] = CHAR_STRINGTERM_CN90;}

  } /* end if */

  return ((nToken >= nRequest) && ((j <= nLength) ||(nLength == -1)));
} /* end cn90GetToken */

/* PRIVATE FUNCTION cn90GetStartAndEndOfToken
 * ------------------------------------------------------------------
 */
bool cn90GetStartAndEndOfToken(const char  *   szBuffer,
                               const char  * & TokenStart,
                               const char  * & TokenEnd,
                               const long       nRequest)
{
    long nToken  =0;
    long i       =0;
    bool bOutside=true;
    bool bString =false;

    TokenEnd=TokenStart=0;

    if(0!=szBuffer) // is buffer valid
    {
        while(CHAR_STRINGTERM_CN90!=szBuffer[i] && 0==TokenEnd)  //loop until we've reached the end of buffer or the requested token is complete
        {
            if(bOutside)    // if we're outside of a token ...
            {
                if(!isspace(szBuffer[i]))   //... look out for a non whitesapaces
                {
                    bOutside=false;
                    nToken++;

                    if(nToken==nRequest)
                        TokenStart=szBuffer+i;

                    bString=(CHAR_SYNTAXSTRING_CN90==szBuffer[i]);  // set a flag for string handling
                }
            }
            else            // if we're inside of a token ...
            {
                if(bString)
                {
                    if(CHAR_SYNTAXSTRING_CN90==szBuffer[i] && isspace(szBuffer[i+1]))   // look out for string terminator
                    {
                        bOutside=true;
                        i++;    //position on the space
                    }
                }
                else
                {
                    if(isspace(szBuffer[i]))
                        bOutside=true;
                    else
                        bString=(CHAR_SYNTAXSTRING_CN90==szBuffer[i]);  // set a flag for string handling
                }

                if(nToken==nRequest && bOutside)    //right token and szBuffer[i] is the first space after the token ...
                    TokenEnd=szBuffer+i;            //thats the end we looked for
            }

            i++;
        }
    }

    if(CHAR_STRINGTERM_CN90==szBuffer[i] && 0!=TokenStart && 0==TokenEnd)
        TokenEnd=szBuffer+i;

    return (0!=TokenStart && 0!=TokenEnd);
} // end cn90GetStartAndEndOfToken

/* PUBLIC FUNCTION cn90AnalyzeNumber
 * ------------------------------------------------------------------
 */
bool cn90AnalyzeNumber
    ( char            * szBuffer,
      long             * pValue,
	  bool               bAllowNegatives)
{
  long               i           = 0;
  bool            bValid      = true;

  cn90Strip(szBuffer);
  for (i = 0; szBuffer[i] != CHAR_STRINGTERM_CN90; i++) {
    bValid = (isdigit(szBuffer[i]) || ((i==0) && (szBuffer[i]=='-') && bAllowNegatives)) && bValid;
  } /* end if */

  *pValue = (bValid) ? atol(szBuffer) : 0;

  return bValid;
} /* end cn90AnalyzeNumber */



/* PUBLIC FUNCTION cn90AnalyzeString
 * ------------------------------------------------------------------
 */
bool cn90AnalyzeString
    ( const char      * szBuffer,
      char            * szValue,
      const long         nMaxLen )
{
  long               i           = 0;
  long               nLength     = 0;
  bool               bValid      = true;
  char *             pTemp       = 0;

  nLength = (long)strlen(szBuffer);
  pTemp = new char[nLength + 1];

  if (pTemp) {

    SAPDB_strcpy(pTemp, szBuffer);
    cn90Strip(pTemp);
    nLength = (long)strlen(pTemp);

    if (nLength > 1) {
      if ((pTemp[0]           == CHAR_SYNTAXSTRING_CN90) &&
          (pTemp[nLength - 1] == CHAR_SYNTAXSTRING_CN90)    ) {
        for (i = 1; (i < (nLength - 1)) && bValid; i++) {
          if ( (pTemp[i] == CHAR_SYNTAXSTRING_CN90   ) ||
//               (pTemp[i] == CHAR_SYNTAXFOLLOW_CN90   ) ||
               (pTemp[i] <  CHAR_FIRSTPRINT_CN90     ) ||
               (pTemp[i] >  CHAR_LASTPRINT_CN90      )    ) {
            bValid = false;
          } else {
            szValue[i - 1] = pTemp[i];
          } /* end if */
        } /* end for */

        if (i > (nMaxLen + 1)) {
          bValid = false;
        } /* end if */

      } else {
        bValid = false;
      } /* end if */
    } else {
      bValid = false;
    } /* end if */

    szValue[bValid ? i - 1 : 0] = CHAR_STRINGTERM_CN90;

    delete pTemp;
  } else {
    bValid = false;
  } // end if

  return bValid;
} /* end cn90AnalyzeString */

/* IMPLEMENTATION PUBLIC FUNCTION cn90AnswerIError
 * ------------------------------------------------------------------
 */
tcn00_Error cn90AnswerIError
      ( char              * szBuffer,
        int               * pnDataLen,
        const tcn00_Error   nError,
        int                 nExtError,
        const char *        pExtError,
        int                 n2ndExtError,
        const char *        p2ndExtError)

{
  int i = 0;
  struct {
    tcn00_Error   nValue;
    const char  * szText;
  }               errorArray[] = ERROR_TO_TEXT_CN00;


  while ((errorArray[i].nValue !=  ERR_UNKNOWN_CN00) && (errorArray[i].nValue !=  nError)) {
    i++;
  } /* end while */

  if (pExtError == NULL && p2ndExtError == NULL) {
    sprintf  (szBuffer, 
              "%s%s%d%s%s%s", 
              ((nError == OK_CN00) ? ANSWER_OK_CN00 : ANSWER_ERR_CN00),
              LINE_SEPSTRING_CN00,
              nError,
              TOKEN_SEPSTRING_CN00,
              errorArray[i].szText,
              LINE_SEPSTRING_CN00);
  } else if (p2ndExtError == NULL) {
    sprintf  (szBuffer, 
              "%s%s%d%s%s%s%d%s%s%s",  
              ((nError == OK_CN00) ? ANSWER_OK_CN00 : ANSWER_ERR_CN00),
              LINE_SEPSTRING_CN00,
              nError,
              TOKEN_SEPSTRING_CN00,
              errorArray[i].szText,
              LINE_SEPSTRING_CN00,
              nExtError,
              TOKEN_SEPSTRING_CN00,
              pExtError,
              LINE_SEPSTRING_CN00);
  } else {
    sprintf  (szBuffer, 
              "%s%s%d%s%s%s%d%s%s%s%d%s%s%s",  
              ((nError == OK_CN00) ? ANSWER_OK_CN00 : ANSWER_ERR_CN00),
              LINE_SEPSTRING_CN00,
              nError,
              TOKEN_SEPSTRING_CN00,
              errorArray[i].szText,
              LINE_SEPSTRING_CN00,
              nExtError,
              TOKEN_SEPSTRING_CN00,
              pExtError,
              LINE_SEPSTRING_CN00,
              n2ndExtError,
              TOKEN_SEPSTRING_CN00,
              p2ndExtError,
              LINE_SEPSTRING_CN00);
  } // end if
  *pnDataLen = (int)strlen(szBuffer);

  return nError;
} /* end cn90AnswerIError */

/* IMPLEMENTATION PUBLIC FUNCTION cn90AnswerEvent
 * ------------------------------------------------------------------
 * Attention: this method does not invert incoming message list in the reply!
 */
Msg_List::MessageID cn90AnswerEvent
      ( char           * szBuffer,
        int            * pnDataLen,
        int              nMaxDataLen,
        const Msg_List & EvtLst )
{
    DBMSrv_Reply localReply(szBuffer, pnDataLen, nMaxDataLen);
    return localReply.startWithMessageList( EvtLst);
} /* end cn90AnswerEvent */

/* IMPLEMENTATION PUBLIC FUNCTION cn90AnswerErrnoError
 * ------------------------------------------------------------------
 */
tcn00_Error cn90AnswerErrnoError
      ( char              * szBuffer,
        int               * pnDataLen,
        const tcn00_Error   nError,
        const int           nErrno,
        char              * szInfo)
{
  int j = 0;
  struct {
    int           nValue;
    const char * szText;
  }               errnoArray[] = ERRNO_TO_TEXT;

  while ((errnoArray[j].szText !=  NULL) && (errnoArray[j].nValue != nErrno)) {
    j++;
  } /* end while */
  if (errnoArray[j].szText ==  NULL) {
    j++;
  } /* end if */

  return cn90AnswerIError(szBuffer, pnDataLen, nError, nErrno, errnoArray[j].szText, 0, szInfo);

} /* end cn90AnswerErrnoError */

/* IMPLEMENTATION PUBLIC FUNCTION cn90AnswerExecute
 * ------------------------------------------------------------------
 */
tcn00_Error cn90AnswerExecute
      ( char             * szBuffer,
        int               * pnDataLen,
        const int           nMaxLen,
        const tcn00_Error   nError,
        const int           nSysReturn,
        const char       * szCommand,
        const char       * szTmpFile )
{
  int    i     = 0;
  FILE * hFile = NULL;
  char  szLine[PARAMETER_MAXLEN_CN90 + 1];

  struct {
    tcn00_Error    nValue;
    const char  * szText;
  }               errorArray[] = ERROR_TO_TEXT_CN00;

  while ((errorArray[i].nValue !=  ERR_UNKNOWN_CN00) && (errorArray[i].nValue !=  nError)) {
    i++;
  } /* end while */

  sprintf (szBuffer, "%s%s%d%s%s%s%d%s%s%s", ((nError == OK_CN00) ? ANSWER_OK_CN00 : ANSWER_ERR_CN00),
                                                LINE_SEPSTRING_CN00,
                                                nError,
                                                TOKEN_SEPSTRING_CN00,
                                                errorArray[i].szText,
                                                LINE_SEPSTRING_CN00,
                                                nSysReturn,
                                                TOKEN_SEPSTRING_CN00,
                                                szCommand,
                                                LINE_SEPSTRING_CN00);

  if (szTmpFile != NULL) {
    hFile = fopen(szTmpFile, "r");
    if (hFile) {
      while (fgets(szLine, PARAMETER_MAXLEN_CN90, hFile) != NULL && ((int) (strlen(szBuffer) + strlen(szLine) + 5) < nMaxLen)) {
        strcat(szBuffer, cn90StripTrailingChar(szLine, '\n'));
        strcat(szBuffer, LINE_SEPSTRING_CN00);
      } /* end while */
      fclose(hFile);
    } /* end if */
  } /* end if */

  *pnDataLen = (int)strlen(szBuffer);

  return nError;
} /* end cn90AnswerExecute */

/* IMPLEMENTATION PUBLIC FUNCTION cn90AnswerOK
 * ------------------------------------------------------------------
 */
tcn00_Error cn90AnswerOK
      ( char        * szBuffer,
        int         * pnDataLen,
        const char  * szMsg )
{
  if (szMsg == NULL) {
    sprintf (szBuffer, "%s%s", ANSWER_OK_CN00,
                                 LINE_SEPSTRING_CN00);
  } else {
    sprintf (szBuffer, "%s%s%s%s", ANSWER_OK_CN00,
                                     TOKEN_SEPSTRING_CN00,
                                     szMsg,
                                     LINE_SEPSTRING_CN00);
  } /* end if */

  *pnDataLen = (int)strlen (szBuffer);

  return OK_CN00;
} /* end cn90AnswerOK */

/* PUBLIC FUNCTION cn90Trace
 * ------------------------------------------------------------------
 */
#ifdef CNTRACE

#define TRACE_FILE_CN90     "dbmsrv.prt"
#define TRACE_MAX_CN90      800000

void cn90Trace
      ( const char   * szString1,
        const char   * szString2 )
{
  FILE          * hTrace;
  FILE          * hFile;
  tsp00_C256      szDir;
  char            szFilename[100];
  time_t          aTime;
  size_t          nSize = 0;
  void          * pBuffer = NULL;

  cn90GetWrkPath(szDir);
  SAPDB_strcpy(szFilename, szDir);
  strcat(szFilename, TRACE_FILE_CN90);

  time(&aTime);

  hTrace = fopen(szFilename  , "a+");
  if (hTrace) {
    fseek(hTrace, 0, SEEK_END);
    nSize = ftell(hTrace);
    if (nSize > TRACE_MAX_CN90) {
      fclose(hTrace);
      hTrace = NULL;
      hFile = fopen(szFilename , "rb");
      if (hFile) {
        fseek(hFile, TRACE_MAX_CN90 / 2, SEEK_SET);
        pBuffer = malloc(nSize - TRACE_MAX_CN90 / 2);
        if (pBuffer) {
          fread(pBuffer, nSize - TRACE_MAX_CN90 / 2, 1, hFile);
          fclose(hFile);
          hFile = fopen(szFilename , "wb");
          if (hFile) {
            fwrite(pBuffer, nSize - TRACE_MAX_CN90 / 2, 1, hFile);
            fclose(hFile);
            hTrace = fopen(szFilename  , "a");
          } /* end if */
        } else {
          fclose(hFile);
        } /* end if */
      } /* end if */
    } /* end if */
  } /* end if */

  if (hTrace) {
    fprintf (hTrace, "%s %s : %s\n", ctime(&aTime), szString1, szString2);
    fclose(hTrace);
  } /* end if */

} /* end cn90Trace */
#endif /* CNTRACE */

/* IMPLEMENTATION PUBLIC FUNCTION cn90DirCreate
 * ------------------------------------------------------------------
 */
tcn00_Error cn90DirCreate
      ( char           * szDirectory,
        tsp01_RteError * pRteError )
{
  tcn00_Error      nFuncReturn = OK_CN00;
  unsigned int     nIndex      = 0;
  tsp01_RteError   aRteError;

  /* change slashes */
  cn90ChangePathDelimiter(szDirectory);

  /* try to create directory */
  if (! sqlcreate_dirc (szDirectory, (pRteError == NULL) ? &aRteError : pRteError )) {
    nFuncReturn = DBMSrvMsg_Error::get_tcn00_Error(DBMSrvMsg_Error::DBMSrv_RTEEXT);
  } /* end if */

  return nFuncReturn;
} /* end cn90DirCreate */

/* IMPLEMENTATION PUBLIC FUNCTION cn90ChangePathDelimiter
 * ------------------------------------------------------------------
 */
char * cn90ChangePathDelimiter
      ( char          * szPath )
{
  unsigned long  nIndex      = 0;

  /* change backslash and slash to PATH_DELIMITTER */
  for (nIndex = 0; nIndex < strlen(szPath); nIndex++) {
    szPath[nIndex] = (szPath[nIndex] == CHAR_BACKSLASH) ? CHAR_SLASH          : szPath[nIndex];
    szPath[nIndex] = (szPath[nIndex] == CHAR_SLASH)     ? PATH_DELIMITER_CN90 : szPath[nIndex];
  } /* end for */

  return szPath;
} /* end cn90ChangePathDelimiter */

/* IMPLEMENTATION PUBLIC FUNCTION cn90GetCfgPath
 * ------------------------------------------------------------------
 */
char * cn90GetCfgPath
      ( tsp00_C256 & szDir)
{
  tsp01_RteError   aRteError;
  tsp00_Pathc      szLokDir;

  sqlGetIndependentConfigPath(szLokDir, TERM_WITH_DELIMITER_EO01, &aRteError);
  SAPDB_strcpy(szDir, szLokDir);

  return (char *) szDir;
} /* end cn90GetCfgPath */

/* IMPLEMENTATION PUBLIC FUNCTION cn90GetWrkPath
 * ------------------------------------------------------------------
 */
char * cn90GetWrkPath
      ( tsp00_C256 & szDir)
{
  tsp01_RteError  aRteError;
  tsp00_Pathc     szLokDir;

  sqlGetIndependentWrkPath(szLokDir, TERM_WITH_DELIMITER_EO01, &aRteError);
  SAPDB_strcpy(szDir, szLokDir);

  return (char *) szDir;
} /* end cn90GetWrkPath */

/* IMPLEMENTATION PUBLIC FUNCTION cn90GetVersionEnvPath
 * ------------------------------------------------------------------
 */
char * cn90GetVersionEnvPath
      ( tsp00_C256 & szDir)
{

  tsp01_RteError  aRteError;
  tsp00_Pathc      szLokDir;

  sqlGetDbrootEnvPath(szLokDir, TERM_WITH_DELIMITER_EO01, &aRteError);
  SAPDB_strcpy(szDir, szLokDir);

  return (char *) szDir;
} /* end cn90GetVersionEnvPath */

/* IMPLEMENTATION PUBLIC FUNCTION cn90GetVersionBinPath
 * ------------------------------------------------------------------
 */
char * cn90GetVersionBinPath
      ( tsp00_C256 & szDir)
{
  tsp01_RteError  aRteError;
  tsp00_Pathc     szLokDir;

  sqlGetDbrootBinPath(szLokDir, TERM_WITH_DELIMITER_EO01, &aRteError);
  SAPDB_strcpy(szDir, szLokDir);

  return (char *) szDir;
} /* end cn90GetVersionBinPath */

/*
  -----------------------------------------------------------------------------
  function cn90DBExists
  -----------------------------------------------------------------------------
*/
bool cn90DBExists
      (const tsp00_DbNamec szDbName)
{
    Msg_List oDummy;
    RTE_Database * pDatabase = RTE_DBRegister::Instance().GetDatabase(szDbName, oDummy);
    if (pDatabase) {
        delete pDatabase;
        return true;
    } // end if
    return false;
} // cn90DBExists

/*
  -----------------------------------------------------------------------------
  function cn90DBRoot
  -----------------------------------------------------------------------------
*/
char  * cn90DBRoot
      (const tsp00_DbNamec & szDbName, tsp00_Pathc & szDBRoot)
{
  // PTS 1109637, 1109638, 1109639
#if 1
  static tsp00_DbNamec oldDbName;
  static tsp00_Pathc oldDBRoot;

  if ( strcmp(oldDbName.asCharp(), szDbName.asCharp()) != 0 )
  {
    oldDbName = szDbName;
    if (!RTE_GetDBRootOfDB( oldDbName.asCharp(),
                            oldDBRoot.asCharp(),
                            oldDBRoot.size())) {
      oldDBRoot.Init();
    } // end if
  }

  szDBRoot = oldDBRoot;
#else
  if (!RTE_GetDBRootOfDB( szDbName.asCharp(), 
                          szDBRoot.asCharp(),
                          szDBRoot.size())) {
    szDBRoot.Init();
  } // end if
#endif
  return szDBRoot.asCharp();

} // cn90DBRoot

/*
  -----------------------------------------------------------------------------
  function cn90ServiceDB

  Todo:    
  -----------------------------------------------------------------------------
*/
char * cn90ServiceDB
      (tsp00_DbNamec & szDbName, const tsp00_Pathc & szDBRoot)
{
    szDbName.Init();

    Msg_List oRTEErr;        
    RTE_ActiveInstallation * pInstallation = (RTE_DBRegister::Instance()).GetActiveInstallationByDBRoot(szDBRoot, oRTEErr);
    if (pInstallation) {
        RTE_Database * pServiceDB = pInstallation->GetServiceDB(oRTEErr);
        if (pServiceDB) {
            szDbName.rawAssign(pServiceDB->GetDBName());
            delete pServiceDB;
        } // end if
        delete pInstallation;
    } // end if

    return szDbName.asCharp();
} // cn90ServiceDB

/* 
  -----------------------------------------------------------------------------
  function cn90Timestamp
  -----------------------------------------------------------------------------
*/
const tsp00_Timestampc & cn90Timestamp
    ( tsp00_Timestampc & szTimestamp )
{
  tsp00_Date  DateP;
  tsp00_Time  TimeP;
  tsp00_Datec DateC;
  tsp00_Timec TimeC;
    
  sqldattime(DateP, TimeP);
  DateC.p2c(DateP);
  TimeC.p2c(TimeP);

  sprintf(szTimestamp, "%.8s%.6s", DateC.asCharp(), (TimeC.asCharp() + 2));

  return szTimestamp;

} // end cn90Timestamp
