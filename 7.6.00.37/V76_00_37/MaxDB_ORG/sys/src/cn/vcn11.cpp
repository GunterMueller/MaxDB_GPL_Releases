/*! 
  -----------------------------------------------------------------------------
 
  module: vcn11.h
 
  -----------------------------------------------------------------------------
 
  responsible:  BerndV
 
  special area: dbmgetf
 
  description:  implementation of command line DB-File-Get-Application

  version:      7.2. / 6.2 

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
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include <stdio.h>
#include <stdlib.h>

#include "hcn14.hpp"
#include "ToolsCommon/Tools_PipeCall.hpp"
#include "SAPDBCommon/SAPDB_string.h"

/* 
  -----------------------------------------------------------------------------
  something special for using the same code in 7.1 and 6.2
  -----------------------------------------------------------------------------
*/
#ifdef CN62

  /* bool for some compilers */
  #if defined (HP) || defined (AIX) || defined (OSF1) || defined (SUN)
    #define     bool    unsigned char
    #define     true    1
    #define     false   0
  #endif

#endif

/* 
  -----------------------------------------------------------------------------
  private declarations
  -----------------------------------------------------------------------------
*/
#define C_SWITCH              '-'
#define SW_NODE               "n"
#define SW_KEY                "k"
#define SW_FILE               "f"
#define SW_DATABASE           "d"
#define SW_QUIET              "q"
#define SW_USER               "u"
#define SW_USERKEY            "U"
#define SW_LIST               "l"
#define SW_PARAM              "p"
#define SW_OPERATION          "op"
#define SW_TPSYSID            "tpi"
#define SW_TPCONN             "tpc"
#define SW_TPPROF             "tpp"
#define SW_ENCRYPT            "e"
                       
#define PRG_FATAL             1001
#define PRG_VERYFATAL         1002
                       
#define DBM_OK                "OK"
#define DBM_END               "END"
#define DBM_CONTINUE          "CONTINUE"

#define ENCRYPT_TEXT_SSL      "SSL"
#define TXT_ERR_ENCMETHOD     "Error, Unrecognized encryption method!"
#define TXT_ERR_ENCNOHOST     "Error, Option -"SW_NODE" required for encryption!"
#define TXT_ERR_MEMORY        "Error, cannot allocate memory!"

enum tcn11EncryptionMethod {
    EncryptionMethodNone,
    EncryptionMethodSSL
};


typedef struct tcn11Options {
    const char  *   szApp;
    const char  *   szDB;
    const char  *   szUser;
    const char  *   szSysId;
    const char  *   szConn;
    const char  *   szProfile;
    const char  *   szNode;
    tcn11EncryptionMethod   encMethod;
    const char  *   szKey;
    const char  *   szFile;
    const char  *   szParam;
    const char  *   szOperation;
    int              nShow;
    bool       bQuiet;
    bool       bList;
    void   *   pSession;
    tsp00_Int4   nMaxData;
} tcn11Options;

/* 
  -----------------------------------------------------------------------------
  function: cn11FatalError
  -----------------------------------------------------------------------------
*/
static void cn11FatalError
      ( const char * errmsg,
        int          nExitCode )
{
    fprintf (stderr, "%s\n", errmsg);
    exit (nExitCode);
} // end cn11FatalError
static void cn11FatalError
      ( const char * errmsg1,
        const char * errmsg2,
        const char * errmsg3,
        const char * errmsg4,
        const char * errmsg5,
        const char * errmsg6,
        int          nExitCode )
{
    fprintf (stderr, "%s %s %s %s %s %s\n", errmsg1, errmsg2, errmsg3, errmsg4, errmsg5, errmsg6);
    exit (nExitCode);
} // end cn11FatalError

/* 
  -----------------------------------------------------------------------------
  function: cn11Usage
  -----------------------------------------------------------------------------
*/
static void cn11Usage 
      (tcn11Options * pOpt)
{
  fprintf (stderr, "usage: %s [<options>]\n", pOpt->szApp);
  fprintf (stderr, "  <options>:\n");
  fprintf (stderr, "    -%-4s dbname     (name of datbase)                \n", SW_DATABASE);
  fprintf (stderr, "    -%-4s user,pwd   (user for authorization)         \n", SW_USER);
  fprintf (stderr, "    -%-4s userkey    (userkey for authorization)      \n", SW_USERKEY);
  fprintf (stderr, "    -%-4s node       (name of servernode)             \n", SW_NODE);
  fprintf (stderr, "    -%-4s encryption (encryption method)              \n", SW_ENCRYPT);
  fprintf (stderr, "                     (use only with -n)               \n");
  fprintf (stderr, "                     (valid methods: '"ENCRYPT_TEXT_SSL"')\n");
  fprintf (stderr, "    -%-4s id         (id of database file)            \n", SW_KEY);
  fprintf (stderr, "    -%-4s file       (name for local file - optional) \n", SW_FILE);
  fprintf (stderr, "    -%-4s            (quiet mode)                     \n", SW_QUIET);
  fprintf (stderr, "    -%-4s            (list file id's)                 \n", SW_LIST);
  fprintf (stderr, "    -%-4s <param>    (DATE or LINE parameter)         \n", SW_PARAM);
  fprintf (stderr, "    -%-4s <opid>     (SHRINK or DELETE operation)     \n", SW_OPERATION);
  cn11FatalError("", PRG_FATAL);
} /* end cn11Usage */

/* 
  -----------------------------------------------------------------------------
  function: cn11ReadNextArg
  -----------------------------------------------------------------------------
*/
static char * cn11ReadNextArg
      ( char * szArgs[],
        int  & nCurArg,
        int    nMaxArgs,
        bool   bSwitch = false)
{
  int    nCurrent  = nCurArg + 1;
  char * pArgument = NULL;

  if (nCurrent < nMaxArgs) {
    if (bSwitch && szArgs[nCurrent][0] == C_SWITCH) {
      pArgument = &(szArgs[nCurrent][1]);
      nCurArg   = nCurrent;
    } else if (!bSwitch && szArgs[nCurrent][0] != C_SWITCH) {
      pArgument = szArgs[nCurrent];
      nCurArg   = nCurrent;
    } // end if
  } // end if

  return pArgument;
} // end cn11ReadNextArg

/* 
  -----------------------------------------------------------------------------
  function: cn11ParseOptions
  -----------------------------------------------------------------------------
*/
static void cn11ParseOptions 
      ( tcn11Options       * pOpt, 
        int                  argc, 
        char               * argv[] )
{
  int    argI      = 0;
  char * pSwitch   = NULL;
  char * pValue    = NULL;

  /* init options struct */
  pOpt->szApp      = argv[0];
  pOpt->szDB       = NULL;
  pOpt->szUser     = NULL;
  pOpt->szSysId    = NULL;
  pOpt->szConn     = NULL;
  pOpt->szProfile  = NULL;
  pOpt->szKey      = NULL;
  pOpt->szFile     = NULL;
  pOpt->szParam    = NULL;
  pOpt->szOperation= NULL;
  pOpt->nShow      = 0;
  pOpt->szNode     = "";
  pOpt->encMethod = EncryptionMethodNone;
  pOpt->bQuiet     = false;
  pOpt->bList      = false;

  /* read options form command line into struct */
  pSwitch = cn11ReadNextArg(argv, argI, argc, true);

  while (pSwitch != NULL) {

    // * Node -n ******************************
    if (strcmp(pSwitch, SW_NODE) == 0) {
      pOpt->szNode = cn11ReadNextArg(argv, argI, argc);
      if (pOpt->szNode == NULL) {
        cn11FatalError ("Error, missing node name!", PRG_VERYFATAL);
      } // end if

    // * encrpytion method -e **************************
    } else if (strcmp(pSwitch, SW_ENCRYPT) == 0) {
      const char* encryptMethod(cn11ReadNextArg(argv, argI, argc)); // we dont delete it
      if (encryptMethod == NULL || SAPDB_strcasecmp(ENCRYPT_TEXT_SSL,encryptMethod) != 0) {
        cn11FatalError (TXT_ERR_ENCMETHOD, PRG_VERYFATAL);
      }
      else {
          // the only method that is supported currently
          pOpt->encMethod = EncryptionMethodSSL;
      }

    // * Database -d **************************
    } else if (strcmp(pSwitch, SW_DATABASE) == 0) {
      pOpt->szDB = cn11ReadNextArg(argv, argI, argc);

    // * User -u ******************************
    } else if (strcmp(pSwitch, SW_USER) == 0) {
      pOpt->szUser = cn11ReadNextArg(argv, argI, argc);

    // * TP sysid -tpi ************************
    } else if (strcmp(pSwitch, SW_TPSYSID) == 0) {
      pOpt->szSysId = cn11ReadNextArg(argv, argI, argc);

    // * TP conn -tpc *************************
    } else if (strcmp(pSwitch, SW_TPCONN) == 0) {
      pOpt->szConn = cn11ReadNextArg(argv, argI, argc);

    // * TP prifile -tpp **********************
    } else if (strcmp(pSwitch, SW_TPPROF) == 0) {
      pOpt->szProfile = cn11ReadNextArg(argv, argI, argc);

    // * File -f ******************************
    } else if (strcmp(pSwitch, SW_FILE) == 0) {
      pOpt->szFile = cn11ReadNextArg(argv, argI, argc);

    // * Param -p *****************************
    } else if (strcmp(pSwitch, SW_PARAM) == 0) {
      pOpt->szParam = cn11ReadNextArg(argv, argI, argc);

    // * Operation -op ************************
    } else if (strcmp(pSwitch, SW_OPERATION) == 0) {
      pOpt->szOperation = cn11ReadNextArg(argv, argI, argc);

    // * Key -k *******************************
    } else if (strcmp(pSwitch, SW_KEY) == 0) {
      pOpt->szKey = cn11ReadNextArg(argv, argI, argc);

    // * Quiet -q *****************************
    } else if (strcmp(pSwitch, SW_QUIET) == 0) {
      pOpt->bQuiet = true;

    // * List -l ******************************
    } else if (strcmp(pSwitch, SW_LIST) == 0) {
      pOpt->bList = true;
      pValue = cn11ReadNextArg(argv, argI, argc);
      if (pValue != NULL) {
        pOpt->nShow = atoi(pValue);
      } // end if

    // ****************************************
    } else {
      cn11Usage (pOpt);
    } // end if

    pSwitch = cn11ReadNextArg(argv, argI, argc, true);
  } /* end while */


  if( strlen(pOpt->szNode)==0 && pOpt->encMethod!=EncryptionMethodNone ) {
    cn11FatalError (TXT_ERR_ENCNOHOST, PRG_VERYFATAL);
  }
  if (pOpt->szDB == NULL) {
    cn11FatalError ("Error, missing database name!", PRG_VERYFATAL);
  } // end if
  if (pOpt->szKey == NULL && !pOpt->bList) {
    cn11FatalError ("Error, missing file identifier!", PRG_VERYFATAL);
  } // end if
  if (pOpt->szKey != NULL && pOpt->bList) {
    cn11FatalError ("Error, don't specify -l with -k!", PRG_VERYFATAL);
  } // end if
  if (pOpt->szUser != NULL && (pOpt->szSysId != NULL || pOpt->szConn != NULL || pOpt->szProfile != NULL)) {
    cn11FatalError ("Error, don't specify -u with -tpi/-tpc/-tpp!", PRG_VERYFATAL);
  } // end if
  if ( ( pOpt->szConn != NULL || pOpt->szSysId != NULL || pOpt->szProfile != NULL ) &&
       ( pOpt->szConn == NULL || pOpt->szSysId == NULL || pOpt->szProfile == NULL )    ) {
    cn11FatalError ("Error, incomplete TP data (-tpi/-tpc/-tpp)!", PRG_VERYFATAL);
  } // end if

} // end cn11ParseOptions

/* 
  -----------------------------------------------------------------------------
  function: cn11CheckFile
  -----------------------------------------------------------------------------
*/
static void cn11CheckFile
      ( tcn11Options       * pOpt )
{
  FILE   * hFile;
  char     cAnswer;

  // only if quiet -q not specified
  if (!pOpt->bQuiet && pOpt->szFile != NULL) {
    // open for reading
    hFile = fopen(pOpt->szFile, "r");
    
    if (hFile != NULL) {
      // Ooops, file exists -> ask the user
      fclose(hFile);
      fprintf(stderr, "File %s exists! Overwrite (y/n)?\n", pOpt->szFile);
      cAnswer = getchar();
      switch (cAnswer) {
        case 'y':
        case 'Y':
          // ok, go on
          break;
        default:
          // in other case terminate
          cn11FatalError ("Execution aborted!", PRG_FATAL);
          break;
      } // end switch
    } // end if
  } // end if

} // end cn11CheckFile

/* 
  -----------------------------------------------------------------------------
  function: cn11Command 
  -----------------------------------------------------------------------------
*/
static char * cn11Command 
      ( tcn11Options       * pOpt,
        char               * pData,
        tsp00_Int4           * nReply)
{
  bool             bRc     = true;
  tsp00_ErrTextc   szErr;
  tsp00_Int4         commErr = DBMAPI_OK_CN14;
  long             nCmdLen       = 0;
  tsp00_Int4         nReplyLen     = 0;
  char           * pReply;

  // compute command len and reply position
  nCmdLen = (long) strlen (pData) + 1; /* null byte!!! */
  pReply = pData + nCmdLen;

  // execute  command
  nReplyLen = pOpt->nMaxData - nCmdLen;
  commErr = cn14cmdExecute (pOpt->pSession, pData, nCmdLen, pReply, &nReplyLen, szErr);

  // something wrong?
  if (commErr != DBMAPI_OK_CN14) {
    fprintf (stderr, "\nDBM-Api error (%d %s)\n", commErr, (char *) szErr);
    pReply = NULL;
  } // end if 

  *nReply = nReplyLen;

  return pReply;
} // end cn11Command 

/* 
  -----------------------------------------------------------------------------
  function: cn11CopyFile
  -----------------------------------------------------------------------------
*/
static int cn11CopyFile
      ( tcn11Options       * pOpt )
{
  char          * pData        = NULL;
  const char    * pReply       = NULL;
  long            nHandle      = 0;
  FILE          * hFile        = NULL;
  bool            bContinue    = true;
  bool            bTmpContinue = true;
  const char    * pNewLine     = NULL;
  long            nSize        = 0;
  long            nFullSize    = 0;
  long            nWritten     = 0;
  long            nPacketSize  = 0;
  int             nReturn      = 0;
  tsp00_Int4        nReplyLen    = 0; 
  const char    * pPayLoad     = NULL;
  tsp00_ErrTextc  szError;
  tsp00_Int4        nError       = 0;
  tsp00_Int4        nDummy1;
  tsp00_Int4        nDummy2;

  cn14packetstats(pOpt->pSession, &(pOpt->nMaxData), &nDummy1, &nDummy2);
  pData = new char[pOpt->nMaxData];

  if (pData != NULL) {

    if (pOpt->szFile != NULL) {
      hFile = fopen(pOpt->szFile, "wb");
    } else {
      hFile = stdout;
    } // end if

    if (hFile) {

      // DBMServer Command
      sprintf (pData, "file_getfirst %s %s", pOpt->szKey, pOpt->szParam == NULL ? "" : pOpt->szParam);
      pReply = cn11Command(pOpt, pData, &nReplyLen);

      // read DBMServer Reply
      while (pReply != NULL && bContinue) {

        bContinue    = false;
        bTmpContinue = false;

        if (cn14analyzeDbmAnswer (pOpt->pSession, 
                                  (const void **) &pPayLoad, 
                                  &nReplyLen, 
                                  &nError, 
                                  szError) != DBMAPI_OK_CN14) {
          fprintf(stderr, "\nError while receiving data! Operation aborted!\n");
          fprintf(stderr, "%ld, %s\n", (long) nError, (char *) szError);
          if (pPayLoad != NULL) {
            fprintf(stderr, "%.*s\n", (int) nReplyLen, (char *) pPayLoad);
          } // end if
          nReturn = PRG_FATAL;
        } else {
          // look for Handle
          if (pPayLoad != NULL) {
            pReply = pPayLoad;
            pNewLine = strstr(pReply, "\n");
            if (pNewLine != NULL) {
              nHandle = atol(pReply);
            
              // Look for end/continue
              pReply = pNewLine + 1;
              pNewLine = strstr(pReply, "\n");
              if (pNewLine != NULL) {
                if (strstr(pReply, DBM_CONTINUE) != NULL) {
                  bTmpContinue = true;
                } // end if

                // Look for packetsize
                pReply = pNewLine + 1;
                pNewLine = strstr(pReply, "\n");
                if (pNewLine != NULL) {
                  sscanf(pReply, "%ld %ld", &nSize, &nPacketSize);
                  nFullSize = nSize == 0 ? nFullSize : nSize;
                  // look for data
                  pReply = pNewLine + 1;
                  if (nPacketSize > 0) {
                    if (fwrite(pReply, nPacketSize, 1, hFile) == 1) {
                      bContinue = bTmpContinue;
                      nWritten = nWritten + nPacketSize;
                      if (!pOpt->bQuiet && pOpt->szFile != NULL) {
                        fprintf(stdout, "%10ld Bytes of %10ld Bytes received!\r", nWritten, nFullSize);
                      } // end if
                    } else {
                      fprintf(stderr, "\nError while writing file %s! Operation aborted!\n", pOpt->szFile);
                      nReturn = PRG_FATAL;
                    } // end if
                  } // end if
                } // end if
              } // end if
            } // end if
          } // end if

        } /* end if */
            
        if (bContinue) {
          // DBMServer Command
          sprintf (pData, "file_getnext %s %ld", pOpt->szKey, nHandle);
          pReply = cn11Command(pOpt, pData, &nReplyLen);
        } // end if
      } // end while

      // print statistic and close file
      if (!pOpt->bQuiet) {
        if (pOpt->szFile == NULL) {
          fprintf(stdout, "** EOF **\n");
        } // end if
        fprintf(stdout, "%10ld Bytes of %10ld Bytes received!\n", nWritten, nFullSize);
      } // end if
      if (pOpt->szFile != NULL) {
        fclose(hFile);
      } // end if
    } else {
      fprintf(stderr, "Error during open file %s! Operation aborted!\n", pOpt->szFile);
      nReturn = PRG_FATAL;
    } // end if

    delete pData;
  } else {
    fprintf(stderr, "Error during memory allocation! Operation aborted!\n");
    nReturn = PRG_FATAL;
  } // end if

  return nReturn;
} // end cn11CopyFile

/* 
  -----------------------------------------------------------------------------
  function: cn11ListIds
  -----------------------------------------------------------------------------
*/
static int cn11ListIds
      ( tcn11Options       * pOpt )
{
  char           * pData        = NULL;
  char           * pReply       = NULL;
  FILE           * hFile        = NULL;
  const char    * pLine        = NULL;
  const char    * pKomma       = NULL;
  int              nReturn      = 0;
  tsp00_Int4         nReplyLen    = 0; 
  const char     * pPayLoad     = NULL;
  tsp00_ErrTextc   szError;
  tsp00_Int4         nError       = 0;
  tsp00_Int4         nDummy1;
  tsp00_Int4         nDummy2;

  cn14packetstats(pOpt->pSession, &(pOpt->nMaxData), &nDummy1, &nDummy2);
  pData = new char[pOpt->nMaxData];

  if (pData != NULL) {

    if (pOpt->szFile != NULL) {
      hFile = fopen(pOpt->szFile, "wb");
    } else {
      hFile = stdout;
    } // end if

    if (hFile) {

      // DBMServer Command
      if (pOpt->nShow > 0) {
        sprintf (pData, "file_getlist %d", pOpt->nShow);
      } else {
        sprintf (pData, "file_getlist");
      } // end if
      pReply = cn11Command(pOpt, pData, &nReplyLen);

      // read DBMServer Reply
      if (cn14analyzeDbmAnswer (pOpt->pSession, 
                                (const void **) &pPayLoad, 
                                &nReplyLen, 
                                &nError, 
                                szError) != DBMAPI_OK_CN14) {
        if (nError ==  -24946) {
          sprintf (pData, "file_getlist");
          pReply = cn11Command(pOpt, pData, &nReplyLen);
          if (cn14analyzeDbmAnswer (pOpt->pSession, 
                                    (const void **) &pPayLoad, 
                                    &nReplyLen, 
                                    &nError, 
                                    szError) != DBMAPI_OK_CN14) {
            fprintf(stderr, "\nError while receiving data! Operation aborted!\n");
            fprintf(stderr, "%ld, %s\n", (long) nError, (char *) szError);
            nReturn = PRG_FATAL;
          } // end if
        } else {
          fprintf(stderr, "\nError while receiving data! Operation aborted!\n");
          fprintf(stderr, "%ld, %s\n", (long) nError, (char *) szError);
          nReturn = PRG_FATAL;
        } // end if
      } // end if

      if (nReturn != PRG_FATAL) {
        if (pPayLoad != NULL) {
          pLine = pPayLoad;
          pLine = strstr(pLine, "\n");

          while (pLine != NULL) {
            pLine++;
            pKomma = strstr(pLine, "\t"); 
            if (pKomma != NULL) {
              fprintf(hFile, "%.*s\n", (int) (pKomma - pLine), pLine);
            } // end if

            pLine = strstr(pLine, "\n");
          } // end while

        } // end if
      } // end if

      if (pOpt->szFile != NULL) {
        fclose(hFile);
      } // end if
    } else {
      fprintf(stderr, "Error during open file %s! Operation aborted!\n", pOpt->szFile);
      nReturn = PRG_FATAL;
    } // end if

    delete pData;
  } else {
    fprintf(stderr, "Error during memory allocation! Operation aborted!\n");
    nReturn = PRG_FATAL;
  } // end if

  return nReturn;

} // end cn11ListIds

/* 
  -----------------------------------------------------------------------------
  function: cn11ModifyFile
  -----------------------------------------------------------------------------
*/
static int cn11ModifyFile
      ( tcn11Options       * pOpt )
{
  char           * pData        = NULL;
  char           * pReply       = NULL;
  FILE           * hFile        = NULL;
  const char    * pLine        = NULL;
  const char    * pKomma       = NULL;
  int              nReturn      = 0;
  tsp00_Int4         nReplyLen    = 0; 
  const char     * pPayLoad     = NULL;
  tsp00_ErrTextc   szError;
  tsp00_Int4         nError       = 0;
  tsp00_Int4         nDummy1;
  tsp00_Int4         nDummy2;

  cn14packetstats(pOpt->pSession, &(pOpt->nMaxData), &nDummy1, &nDummy2);
  pData = new char[pOpt->nMaxData];

  if (pData != NULL) {

    if (pOpt->szFile != NULL) {
      hFile = fopen(pOpt->szFile, "wb");
    } else {
      hFile = stdout;
    } // end if

    if (hFile) {

      // DBMServer Command
      sprintf (pData, "file_operation %s OP=%s %s",  pOpt->szKey, pOpt->szOperation,
                                                     pOpt->szParam == NULL ? "" : pOpt->szParam);
      pReply = cn11Command(pOpt, pData, &nReplyLen);

      // read DBMServer Reply
      if (cn14analyzeDbmAnswer (pOpt->pSession, 
                                (const void **) &pPayLoad, 
                                &nReplyLen, 
                                &nError, 
                                szError) != DBMAPI_OK_CN14) {
        fprintf(stderr, "\nError while receiving data! Operation aborted!\n");
        fprintf(stderr, "%ld, %s\n", (long) nError, (char *) szError);
        nReturn = PRG_FATAL;
      } // end if

      if (nReturn != PRG_FATAL) {
        fprintf(hFile, "OK\n");
      } // end if

      if (pOpt->szFile != NULL) {
        fclose(hFile);
      } // end if
    } else {
      fprintf(stderr, "Error during open file %s! Operation aborted!\n", pOpt->szFile);
      nReturn = PRG_FATAL;
    } // end if

    delete pData;
  } else {
    fprintf(stderr, "Error during memory allocation! Operation aborted!\n");
    nReturn = PRG_FATAL;
  } // end if

  return nReturn;

} // end cn11ModifyFile

/* 
  -----------------------------------------------------------------------------
  function: main
  -----------------------------------------------------------------------------
*/
int main (int argcIN, char* argvIN[])
{
  int                 argc;
  char **             argv;

  if(Tools_PipeCall::ReadArguments(argcIN, argvIN, argc, argv, Tools_PipeCall::ReadXUser, "c") != 0) {
    cn11FatalError(Tools_PipeCall::ErrorReason(), PRG_FATAL);
  } // end if

  int                 nExit    = 0;
  tcn11Options        aOpt;
  tsp00_Int4          commErr(DBMAPI_OK_CN14);

  tsp00_ErrTextc            szErr;
  const char              * pError  = szErr.asCharp();
  Tools_DynamicUTF8String   oError;


  // read arguments
  cn11ParseOptions (&aOpt, argc, argv);

  // check the output file
  cn11CheckFile(&aOpt);

  char* dbmDynamicURI(NULL);
  const char* theURI(NULL);
  bool usingDynamicURI(false);
  if( aOpt.encMethod == EncryptionMethodSSL ) {
    // create URI for encryption
    tsp00_Int4 lenURI = cn14buildDBMURI(aOpt.szNode, aOpt.szDB, DBMAPI_USE_OUTURI_CN14_NO, dbmDynamicURI, szErr);
    if( lenURI <= 0)
      commErr = DBMAPI_NOT_OK_CN14;
    else {
      dbmDynamicURI = new char[lenURI];
      if( dbmDynamicURI == NULL ) {
        commErr = DBMAPI_NOT_OK_CN14;
        szErr.rawAssign(TXT_ERR_MEMORY);
      }
      else {
        lenURI = cn14buildDBMURI(aOpt.szNode, aOpt.szDB, DBMAPI_USE_OUTURI_CN14_YES, dbmDynamicURI, szErr);
        if( lenURI <=0 )
          commErr = DBMAPI_NOT_OK_CN14;
        else {
          // here we make sure we pass the URI of we have no error
          theURI = dbmDynamicURI;
        }
      }
    }
  }
  else
    theURI = aOpt.szNode;

  // connect  
  if( commErr == DBMAPI_OK_CN14 ) {
    if (aOpt.szUser != NULL) {
        commErr = cn14connectDBMUsr (theURI, 
                                    aOpt.szDB, 
                                    "", 
                                    aOpt.szUser, 
                                    &(aOpt.pSession), 
                                    oError);
    } else if (aOpt.szSysId != NULL) {
        commErr = cn14connectDBMTp (theURI, 
                                    aOpt.szDB, 
                                    "", 
                                    aOpt.szSysId, 
                                    aOpt.szConn, 
                                    aOpt.szProfile,  
                                    &(aOpt.pSession), 
                                    oError);
    } else {
        commErr = cn14connectDBM (theURI, 
                                aOpt.szDB, 
                                "", 
                                &(aOpt.pSession), 
                                oError);
    } // end if
    oError.Insert(0, "\n");
    pError = oError.CharPtr();
  }

  if( dbmDynamicURI != NULL )
      delete[] dbmDynamicURI;

  if (commErr != DBMAPI_OK_CN14) {
	  cn11FatalError("Connection failed to", DEFAULT_DBMPGM_CN14, "at", aOpt.szNode, ":", pError, PRG_FATAL);
  } /* end if */

  // copy the file
  if (aOpt.bList) {
    nExit = cn11ListIds(&aOpt);
  } else if (aOpt.szOperation != NULL) {
    nExit = cn11ModifyFile(&aOpt);
  } else {
    nExit = cn11CopyFile(&aOpt);
  } // end if

  // always disconnect 
  cn14release (&(aOpt.pSession));

  // exit program
  exit (nExit);

  return (nExit);
} // end main
