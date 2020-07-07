/*! 
  -----------------------------------------------------------------------------
 
  module: hcn90.h
 
  -----------------------------------------------------------------------------
 
  responsible:  BerndV
 
  special area: DBMServer
 
  description:  specification of useful utility functions
                for the cn (Control) layer

  version:      7.2. / 6.2 

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



  -----------------------------------------------------------------------------
*/
 

#ifndef _HCN90_H_
#define _HCN90_H_

/* 
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/

#include "gcn00.h"
#include "Messages/Msg_List.hpp"

#include "hcn90c.h"

/* 
  -----------------------------------------------------------------------------
  specification public macros
  -----------------------------------------------------------------------------
*/
#define CHAR_STRINGTERM_CN90     '\0'
#define CHAR_PASCALFILL_CN90     ' '
#define CHAR_SYNTAXSTRING_CN90   '"'
#define CHAR_SYNTAXFOLLOW_CN90   '\\'
#define CHAR_FIRSTPRINT_CN90     '\x20'
#define CHAR_LASTPRINT_CN90      '\x7E'
#define PARAMETER_MAXLEN_CN90    1024

#ifndef PATH_DELIMITER_EO01
  #if defined(_WIN32) || defined(OS2) 
    #define PATH_DELIMITER_CN90   '\\'
  #else
    #define PATH_DELIMITER_CN90   '/'
  #endif
#else
  #define PATH_DELIMITER_CN90   PATH_DELIMITER_EO01
#endif

/* forward declaration! this avoids unnecessary includes */
typedef struct tsp01_RteError tsp01_RteError;

#ifndef SERVICE_PARAMFILE_ID_EO01
  #define SERVICE_PARAMFILE_ID_OLD_CN90 "_"
#else
  #define SERVICE_PARAMFILE_ID_OLD_CN90 SERVICE_PARAMFILE_ID_EO01
#endif

#define SERVICE_PARAMFILE_ID_CN90 "."

/* 
  -----------------------------------------------------------------------------
  specification public functions
  -----------------------------------------------------------------------------
*/

/*! 
  -----------------------------------------------------------------------------
 
  function:     cn90StrUpperCopy

  -----------------------------------------------------------------------------

  description:  string copy with case conversion

                This function copies a zero terminated string from a sourcebuffer
                to a destination buffer. All letters will be converted to uppercase 
                except bKeepCase is true.

  arguments:    szDest    [OUT] pointer to destination buffer
                szSource  [IN]  pointer to source buffer with the zero terminated 
                                string
                bKeepCase [IN]  "keep case flag"

  return value: Pointer to the destination buffer (equal to szDest).
 
  -----------------------------------------------------------------------------
*/
char  * cn90StrUpperCopy 
    ( char              * szDest,
      const char        * szSource,
      const bool           bKeepCase );

/*!
  -----------------------------------------------------------------------------
 
  function:     cn90StringPtoC

  -----------------------------------------------------------------------------

  description:  string copy pascal to c 

                This function copies a string in pascal representation to a 
                buffer using C representation (zero terminated).

  arguments:    szDestC  [OUT] pointer to buffer for the zero terminated string
                pSourceP [IN]  pointer to the pascal representaion of string
                nLength  [IN]  max. number of characters to copy

  return value: Pointer to the destination buffer (equal to szDestC).
 
  -----------------------------------------------------------------------------
*/
char  * cn90StringPtoC
    ( char              * szDestC,
      const char        * pSourceP,
      const int            nLength );

/*!
  -----------------------------------------------------------------------------
 
  function:     cn90StringCtoP

  -----------------------------------------------------------------------------

  description:  string copy c to pascal

                This function copies a string in c representation (zero 
                terminated) to a buffer using pascal representation 

  arguments:    szDestP  [OUT]  pointer to buffer for the pascal string
                pSourceC [IN]   pointer to the zero terminated source string
                nLength  [IN]   max. number of characters to copy
 
  return value: Pointer to the destination buffer (equal to szDestP)
 
  -----------------------------------------------------------------------------
*/
char  * cn90StringCtoP  
    ( char              * pDestP,
      const char        * szSourceC,
      const int            nLength );

/*!
  -----------------------------------------------------------------------------
 
  function:     cn90StripLeadingChar

  -----------------------------------------------------------------------------

  description:  strips leading characters

                This function strips all leading characters equal to the specified
                character from the string.

  arguments:    szString [INOUT] Pointer to buffer with a zero terminated string.
                                 (contents will be modified)
                cStrip   [IN]    character to strip


  return value: Pointer to the string buffer (equal to szString).
 
  -----------------------------------------------------------------------------
*/
char  * cn90StripLeadingChar
    ( char        * szString,
      const char    cStrip );


/*!
  -----------------------------------------------------------------------------
 
  function:     cn90StripTrailingChar

  -----------------------------------------------------------------------------

  description:  strips trailing characters

                This function strips all trailing characters equal to the specified
                character from the string.

  arguments:    szString [INOUT] Pointer to buffer with a zero terminated string.
                                 (contents will be modified)
                cStrip   [IN]    character to strip


  return value: Pointer to the string buffer (equal to szString).
 
  -----------------------------------------------------------------------------
*/
char  * cn90StripTrailingChar
    ( char        * szString,
      const char    cStrip );

/*!
  -----------------------------------------------------------------------------
 
  function:     cn90Strip

  -----------------------------------------------------------------------------

  description:  strips leading and trailing whitespaces

                This function strips all leading and trailing whitespace characters
                (tabs and spaces) from the string.

  arguments:    szString [INOUT] Pointer to buffer with a zero terminated string.
                                 (contents will be modified)

  return value: Pointer to the string buffer (equal to szString).
 
  -----------------------------------------------------------------------------
*/
char  * cn90Strip
    ( char  * szString );

/*!
  -----------------------------------------------------------------------------
 
  function:     cn90StripQuotes

  -----------------------------------------------------------------------------

  description:  strips leading and trailing quotes 

                This function strips all leading and trailing quotes ( " and ' )
                from the string.

  arguments:    szString [INOUT] Pointer to buffer with a zero terminated string.
                                 (contents will be modified)

  return value: Pointer to the string buffer (equal to szString).
 
  -----------------------------------------------------------------------------
*/
char  * cn90StripQuotes
    ( char  * szString );

/*!
  -----------------------------------------------------------------------------
 
  function:     cn90Stricmp

  -----------------------------------------------------------------------------

  description:  caseinsensitive string compare
  
                This function provides a caseinsesitive compare of two zero
                terminated strings.

  arguments:    szString1 [IN]  pointer to the first string
                szString2 [IN]  pointer to the second string

  return value: - < 0 string 1 less than string 2
                - = 0   string 1 identical to string 2
                - > 0 string 1 greater than string 2
                - xxx
  -----------------------------------------------------------------------------
*/
int cn90Stricmp
    ( const char        * szString1,
      const char        * szString2 );

/*!
  -----------------------------------------------------------------------------
 
  function:     cn90StrNicmp

  -----------------------------------------------------------------------------

  description:  caseinsensitive string compare with maximal compare length
  
                This function provides a caseinsesitive compare of two zero
                terminated strings up to a maximal length (caseinsesitive strncmp). 

  arguments:    szString1 [IN]  pointer to the first string
                szString2 [IN]  pointer to the second string
                MaxLength [IN]  maximal number of characters used for the compare

  return value: - < 0 string 1 less than string 2
                - = 0   string 1 identical to string 2
                - > 0 string 1 greater than string 2

  -----------------------------------------------------------------------------
*/
int cn90StrNicmp(const char  * szString1,
                 const char  * szString2,
                 size_t         MaxLength);

/*!
  -----------------------------------------------------------------------------
 
  function:     cn90CheckKeyword

  -----------------------------------------------------------------------------

  description:  checks existence of a keyword in a string
  
                Fucntion check, wether the begin of a line identical to a keyword 
                and is it followed in the line by a whitespace or the termination 
                character or not.

  arguments:    szLine    [IN]  line to analyze
                szString2 [IN]  keyword to find
                bCaseSens [IN]  if true check casesensitive, otherwise ignore case

  return value: true if keyword found, otherwise false
  -----------------------------------------------------------------------------
*/
bool cn90CheckKeyword
    ( const char        * szLine,
      const char        * szKeyword,
      const bool           bCaseSens = true );

/*!
  -----------------------------------------------------------------------------

  function:     cn90SaveStrcpy

  -----------------------------------------------------------------------------

  description:  Provide a save-string-copy
  
                by copy max. n bytes and place a zero termination at the end of 
                destination buffer. The parameter nLength is at most the size of
                destination buffer. This means the maximum of byte to copy is 
                nLength - 1.

  arguments     szDest   [OUT]  pointer to destination buffer
                szSource [IN]   pointer to zero terminated source string
                nLength         max. length of source or destination

  return value: the destination buffer (equal to szDest)
  -----------------------------------------------------------------------------
*/
char  * cn90SaveStrcpy
    ( char              * szDest,
      const char        * szSource,
      int                  nLength );

/*!
  -----------------------------------------------------------------------------

  function:     cn90GetToken

  -----------------------------------------------------------------------------

  description:  provide the requested token from buffer

  arguments:    szBuffer    [IN]  zero terminated buffer with the line to analyze
                szToken     [OUT] pointer to buffer for the zero terminated token
                nRequest    [IN]  number of requested token (begins at 1)
                nLength     [IN]  the length of the providded buffer
                                  (-1 means the buffer is large enough)

  return value: true if requested token found, otherwise false
  -----------------------------------------------------------------------------
*/
bool cn90GetToken
    ( const char      * szBuffer,
      char            * szToken,
      const long         nRequest,
      const long         nLength  );

/*!
  -----------------------------------------------------------------------------

  function:     cn90GetStartAndEndOfToken

  -----------------------------------------------------------------------------

  description:  provides start and end of the requested token from buffer

  arguments:    szBuffer    [IN]  zero terminated buffer with the line to analyze
                TokenStart  [OUT] reference to pointer to start of the token in szBuffer
                TokenEnd    [OUT] reference to pointer to end of the token in szBuffer
                nRequest    [IN]  number of requested token (begins at 1)

  return value: true if requested token found, otherwise false
  -----------------------------------------------------------------------------
*/
bool cn90GetStartAndEndOfToken(const char  *   szBuffer,
                               const char  * & TokenStart,
                               const char  * & TokenEnd,
                               const long       nRequest);

/*!
  -----------------------------------------------------------------------------

  function:     cn90AnalyzeNumber

  -----------------------------------------------------------------------------

  description:  analyze end get a number from a buffer

  arguments:    szBuffer   [IN]  Zero terminated buffer to analyze. Contents of 
                                 szBuffer will be modified by stripping leading
                                 and trailing whitespaces.
                                  
                pValue     [OUT] pointer to long for the value

  return value: true if number found, otherwise false (syntax error)
  -----------------------------------------------------------------------------
*/

bool cn90AnalyzeNumber  
    ( char            * szBuffer,
      long             * pValue,
	  bool               bAllowNegatives=false);

/*!
  -----------------------------------------------------------------------------

  function:     cn90AnalyzeString

  -----------------------------------------------------------------------------

  description:  analyze end get a string from a buffer

  arguments:    szBuffer   [IN]  Zero terminated buffer to analyze (string in 
                                 buffer must be quoted ("example") (escapes not 
                                 supported). Contents of szBuffer will be 
                                 modified by stripping leading and trailing 
                                 whitespaces.
                szValue    [OUT] Pointer to buffer for the zero terminated string 
                                 without quotes.
                nMaxLen    [IN]  Max. length of result buffer (szValue).

  return value: true if string found, otherwise false (syntax error)
  -----------------------------------------------------------------------------
*/
bool cn90AnalyzeString  
    ( const char      * szBuffer,
      char            * szValue,
      const long        nMaxLen );

/*!
  -----------------------------------------------------------------------------

  function:     cn90AnswerIError

  -----------------------------------------------------------------------------

  description:  print a message for an internal error

  example:      
                ERR
                "errornumber","errortext"
                ["extnumber","exttext"]
                ["2ndextnumber","2ndexttext"]

  arguments:    szBuffer       [OUT] buffer for the the zero terminated string
                pnDataLen      [OUT] pointer to integer where the length of string is placed
                nError         [IN]  number of the internal error
                nExtError      [IN]  number of extended error
                pExtError      [IN]  text for extended error
                n2ndExtError   [IN]  number of 2nd extended error
                p2ndExtError   [IN]  text for 2nd extended error

  return value: the value of nError
  -----------------------------------------------------------------------------
*/
tcn00_Error cn90AnswerIError 
      ( char              * szBuffer,
        int               * pnDataLen,
        const tcn00_Error   nError,
        int                 nExtError    = 0,
        const char *        pExtError    = NULL,
        int                 n2ndExtError = 0,
        const char *        p2ndExtError = NULL);

/*!
  -----------------------------------------------------------------------------

  function:     cn90AnswerEvent

  -----------------------------------------------------------------------------

  description:  print a message for an error provide by Msg_List

  example:      
                ERR
                " 1st errornumber","1st errortext"
                " 2nd errornumber","2nd errortext"
                ...

  arguments:    szBuffer    [OUT] buffer for the the zero terminated string
                pnDataLen   [OUT] pointer to integer where the length of string is placed
                EvtLst      [IN]  eventlist to print

  return value: the value of nError
  -----------------------------------------------------------------------------
*/
Msg_List::MessageID cn90AnswerEvent 
      ( char           * szBuffer,
        int            * pnDataLen,
        int              nMaxDataLen,
        const Msg_List & EvtLst );

/*!
  -----------------------------------------------------------------------------

  function:     cn90AnswerErrnoError

  -----------------------------------------------------------------------------

  description:  print a message for an run-time-lib (errno) error

  example:      
                ERR
                "errornumber","errortext"
                "errno","errno-text"

  arguments:    szBuffer    [OUT] buffer for the the zero terminated string
                pnDataLen   [OUT] pointer to integer where the length of string is placed
                nError      [IN]  number of the internal error
                nErrno      [IN]  current errno
                szInfo      [IN]  more text to add

  return value: the value of nError
  -----------------------------------------------------------------------------
*/
tcn00_Error cn90AnswerErrnoError 
      ( char             * szBuffer,
        int               * pnDataLen,
        const tcn00_Error   nError,
        const int           nErrno,
        char             * szInfo = NULL);

/*!
  -----------------------------------------------------------------------------

  function:     cn90AnswerExecute

  -----------------------------------------------------------------------------

  description:  print a message after executing external program

                Print a message for error at execution of an external
                program. If szTmpFile is specified the contens of
                file will be supplied up to nMaxLen. This file should
                be used for output indirection on execution of
                the program. This function can be used both on error and
                on succes of execution (to provide the output)
  
  example:      

                ERR
                "errornumber","errortext"
                "external-errornumber"
                "external-output-line"
                "external-output-line"
                ...

  arguments:    szBuffer    [OUT] buffer for the the zero terminated string
                pnDataLen   [OUT] pointer to integer where the length of string is placed
                nMaxLen     [IN]  sizeof szBuffer
                nError      [IN]  number of the internal error
                nSysReturn  [IN]  return of the external program
                szCommand   [IN]  executed command
                szTmpFile   [IN]  name of the file 

  return value: the value of nError
  -----------------------------------------------------------------------------
*/
tcn00_Error cn90AnswerExecute
      ( char             * szBuffer,
        int               * pnDataLen,
        const int           nMaxLen,
        const tcn00_Error   nError,
        const int           nSysReturn,
        const char       * szCommand,
        const char       * szTmpFile );

/*!
  -----------------------------------------------------------------------------

  function:     cn90AnswerOK

  -----------------------------------------------------------------------------

  description:  print success message

  example:      

                OK[,"message"]

  arguments:    szBuffer    [OUT] buffer for the the zero terminated string
                pnDataLen   [OUT] pointer to integer where the length of string is placed
                stMsg       [IN]  additionaöl information to print

  return value: always OK_CN00
  -----------------------------------------------------------------------------
*/
tcn00_Error cn90AnswerOK 
      ( char        * szBuffer,
        int          * pnDataLen,
        const char  * szMsg );

/* SPECIFICATION PUBLIC FUNCTION cn90Trace
 * ------------------------------------------------------------------
 * purpose:
 *    print a trace message to the file %DBROOT%\wrk\cserver.trc
 *
 *    <timestamp><NL>
 *    <string1> :: <string2><NL>
 *
 * remark:
 *    For use of this function the modules should be compiled
 *    with macro BVMEMDEBUG. Other there is only a dummy macro
 *    for preventig compile time errors.
 *
 * parameters:
 *    char              * szString1
 *       - zero terminated string 1
 *    char              * szString2
 *       - zero terminated string 2
 */
#ifdef CNTRACE
  void cn90Trace 
        ( const char   * szString1,
          const char   * szString2 );
#else /* CNTRACE */
  #define cn90Trace(a, b)
#endif /* CNTRACE */

/* SEPCIFICATION PUBLIC FUNCTION cn90DirCreate
 * ------------------------------------------------------------------
 * purpose:
 *    create the specified dir, if necessary recursively create 
 *    subdirs
 *
 * parameters:
 *    char              * directory
 *       - create the specified directory
 *
 * return:
 *    tcn00_Error   - OK_CN00
 *                    ERR_RTEEXT_CN00
 *                    
 */
tcn00_Error cn90DirCreate
      ( char           * szDirectory,
        tsp01_RteError * pRteError );

/* SEPCIFICATION PUBLIC FUNCTION cn90ChangePathDelimiter
 * ------------------------------------------------------------------
 * purpose:
 *    change the all path delimiters to correct system path delimiter
 *    (macro PATH_DELIMITTER in veo00c) (Das Doppel-T ist nicht von mir!!!)
 *
 * parameters:
 *    char              * szPath
 *       - zero terminated buffer to analyze 
 *       - contents of szPath will be modified
 *
 * return:
 *    char * - the new path (equal to szPath)
 *                    
 */
char * cn90ChangePathDelimiter
      ( char          * szPath );

/* SPECIFICATION PUBLIC FUNCTION cn90GetCfgPath
 * ------------------------------------------------------------------
 * description:
 *    - supplies the config path
 *
 * arguments:
 *    szDBRoot     - the current DBRoot-Value
 *    szDir        - pointer to buffer for the zero terminated path
 *
 * return value:
 *    - the destination buffer (equal to szDir)
 */
char * cn90GetCfgPath
      ( tsp00_C256 & szDir);

/* SPECIFICATION PUBLIC FUNCTION cn90GetWrkPath
 * ------------------------------------------------------------------
 * description:
 *    - supplies the work path
 *
 * arguments:
 *    szDBRoot     - the current DBRoot-Value
 *    szDir        - pointer to buffer for the zero terminated path
 *
 * return value:
 *    - the destination buffer (equal to szDir)
 */
char * cn90GetWrkPath
      ( tsp00_C256 & szWrkDir);

/* SPECIFICATION PUBLIC FUNCTION cn90GetVersionEnvPath
 * ------------------------------------------------------------------
 * description:
 *    - supplies the env path of this cserver version
 *
 * arguments:
 *    szDir     - pointer to buffer for the zero terminated path
 *
 * return value:
 *    - the destination buffer (equal to szDir)
 */
char * cn90GetVersionEnvPath
      ( tsp00_C256 & szDir);

/* SPECIFICATION PUBLIC FUNCTION cn90GetVersionBinPath
 * ------------------------------------------------------------------
 * description:
 *    - supplies the bin path of this cserver version
 *
 * arguments:
 *    szDir     - pointer to buffer for the zero terminated path
 *
 * return value:
 *    - the destination buffer (equal to szDir)
 */
char * cn90GetVersionBinPath
      ( tsp00_C256 & szDir);

/*!
  -----------------------------------------------------------------------------
 
  function:     cn90DBExists

  -----------------------------------------------------------------------------

  description:  checks existence of db (register entry)

                For use of this functions you should include vsp009c.h
                above this header.

  arguments:    szDBName [IN]  - name of the database

  return value: exists or not
  -----------------------------------------------------------------------------
*/
bool cn90DBExists
      ( const tsp00_DbNamec szDbName);

/*!
  -----------------------------------------------------------------------------
 
  function:     cn90DBRoot

  -----------------------------------------------------------------------------

  description:  provide DBRoot of given Database
  
  arguments:    szDBName [IN]  - name of the database
                szDBRoot [OUT] - Buffer for DBRoot

  return value: szDBRoot.asCharp()

  -----------------------------------------------------------------------------
*/
char  * cn90DBRoot
      (const tsp00_DbNamec & szDbName, tsp00_Pathc & szDBRoot);

/*!
  -----------------------------------------------------------------------------
 
  function:     cn90ServiceDB

  -----------------------------------------------------------------------------

  description:  provide ServiceDB of given DBRoot
  
  arguments:    szDBName [OUT]  - name of the database
                szDBRoot [IN] - Buffer for DBRoot

  return value: szDBRoot.asCharp()

  -----------------------------------------------------------------------------
*/
char * cn90ServiceDB
      (tsp00_DbNamec & szDbName, const tsp00_Pathc & szDBRoot);

/*!
  -----------------------------------------------------------------------------
 
  function:     cn90Timestamp

  -----------------------------------------------------------------------------

  description:  provides a timestamp
  
  arguments:    szTimestamp [OUT]  - the timestamp

  -----------------------------------------------------------------------------
*/
const tsp00_Timestampc & cn90Timestamp
    ( tsp00_Timestampc & szTimestamp );

#endif /* _HCN90_H_ */
