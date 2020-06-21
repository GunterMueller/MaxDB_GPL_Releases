/* ==================================================================
 *
 * responsible:   Bernd Vorsprach (D025588 - Bernd.Vorsprach@SAP-AG.de)
 *
 * created:       1998-02-06 by Bernd Vorsprach
 *
 * purpose:       - read and analyze old XParam help file
 *                - read and analyze new XParam help file
 *
 * ==================================================================
 *
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


 */

/* ------------------------------------------------------------------
 * IMPORT                                                                    
 * ------------------------------------------------------------------
 */
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include "heo02.h"
#include "heo06.h"

#include "hcn21.h"
#include "hcn29.h"
#include "hcn90.h"

#include "DBM/Srv/Message/DBMSrvMsg_RTEError.hpp"
#include "DBM/Srv/Deprecated/DBMSrvDep_DirtyReply.hpp"

/* ------------------------------------------------------------------
 * SPECIFICATION PRIVATE FUNCTION cn21_ReadNewHelpFileContents
 * ------------------------------------------------------------------
 * purpose: reading new helpfile
 */
static tcn00_Error cn21_ReadNewHelpFileContents (
      DBMSrvPar_Parameters * pXParamData,
      char              * replyData,
      int               * replyLen,
      char              * szFile,
      tsp00_Int4            hHelpFile );

/* ------------------------------------------------------------------
 * SPECIFICATION PRIVATE FUNCTION cn21_CheckLineSequence
 * ------------------------------------------------------------------
 * purpose: reading new helpfile
 */
static bool    cn21_CheckLineSequence (
      DBMSrvPar_XPLine::LineType currLine,
      DBMSrvPar_XPLine::LineType lastLine);

/* ------------------------------------------------------------------
 * SPECIFICATION PRIVATE FUNCTION cn21_GetSecondToken
 * ------------------------------------------------------------------
 * purpose: reading new helpfile
 */
static tcn00_Error cn21_GetSecondToken (
      char             * szBuffer,
      char            ** pValue,
      tcn002_XPError   * pSyntaxError);

/* ------------------------------------------------------------------
 * SPECIFICATION PRIVATE FUNCTION cn21_AddParameter
 * ------------------------------------------------------------------
 * purpose: reading new helpfile
 */
static tcn00_Error cn21_AddParameter (
      DBMSrvPar_Parameters *   pXParamData,
      char                 *   szBuffer,
      DBMSrvPar_Parameter  * & pParameter,
      tcn002_XPError       *   pSyntaxError);

/* ------------------------------------------------------------------
 * SPECIFICATION PRIVATE FUNCTION cn21_ReadType
 * ------------------------------------------------------------------
 * purpose: reading new helpfile
 */
static tcn00_Error cn21_ReadType (
      DBMSrvPar_Parameter * pParameter,
      char              * szBuffer,
      tcn002_XPError    * pSyntaxError);

/* ------------------------------------------------------------------
 * SPECIFICATION PRIVATE FUNCTION cn21_ReadDefault
 * ------------------------------------------------------------------
 * purpose: reading new helpfile
 */
static tcn00_Error cn21_ReadDefault (
      DBMSrvPar_Parameter * pParameter,
      char              * szBuffer,
      tcn002_XPError    * pSyntaxError);

/* ------------------------------------------------------------------
 * SPECIFICATION PRIVATE FUNCTION cn21_ReadDefaultFor
 * ------------------------------------------------------------------
 * purpose: reading new helpfile
 */
static tcn00_Error cn21_ReadDefaultFor (
      DBMSrvPar_Parameter * pParameter,
      char              * szBuffer,
      tcn002_XPError    * pSyntaxError);

/* ------------------------------------------------------------------
 * SPECIFICATION PRIVATE FUNCTION cn21_ReadProperty
 * ------------------------------------------------------------------
 * purpose: reading new helpfile
 */
static tcn00_Error cn21_ReadProperty (
      DBMSrvPar_Parameter * pParameter,
      char              * szBuffer,
      tcn002_XPError    * pSyntaxError);

/* ------------------------------------------------------------------
 * SPECIFICATION PRIVATE FUNCTION cn21_InitProperties
 * ------------------------------------------------------------------
 * purpose: reading new helpfile
 */
static void cn21_InitProperties (
      DBMSrvPar_Parameter * pParameter);

/* ------------------------------------------------------------------
 * SPECIFICATION PRIVATE FUNCTION cn21_AddLine
 * ------------------------------------------------------------------
 * purpose: reading new helpfile
 */
static tcn00_Error cn21_AddLine (
      DBMSrvPar_Parameter * pParameter,
      char              * szBuffer,
      DBMSrvPar_XPLine::LineType     xpLine,
      long                nNumber,
      DBMSrvPar_XPLine ** pOut);

/* ------------------------------------------------------------------
 * SPECIFICATION PRIVATE FUNCTION cn21_AddFollowLine
 * ------------------------------------------------------------------
 * purpose: reading new helpfile
 */
static tcn00_Error cn21_AddFollowLine (
      DBMSrvPar_XPLine  * pLine,
      char              * szBuffer);

/* ------------------------------------------------------------------
 * START OF IMPLEMENTATION
 * ------------------------------------------------------------------
 */

/* ------------------------------------------------------------------
 * SPECIFICATION PRIVATE FUNCTION cn21_SearchParameter
 * ------------------------------------------------------------------
 * purpose: search a parameter
 */
DBMSrvPar_Parameter * cn21_SearchParameter
      ( DBMSrvPar_Parameter * pParameter,
        const char        * szName );

/* ------------------------------------------------------------------
 * IMPLEMENATION PUBLIC FUNCTION cn21ReadHelpFile
 * ------------------------------------------------------------------
 */
tcn00_Error cn21ReadHelpFile (
      DBMSrvPar_Parameters * pXParamData,
      char              * replyData,
      int               * replyLen,
      char              * szFile,
      bool                bNewSyntax )
{
  tcn00_Error        nFuncStatus    = OK_CN00;
  tsp05_RteFileError rteFileErr;
  bool               bHelpFileOpen  = false;
  tsp00_Int4           hHelpFile;

  /* open XParam help file */
  if (nFuncStatus == OK_CN00) {
    sqlfopenc (szFile, sp5vf_text, sp5vf_read, sp5bk_buffered, &hHelpFile, &rteFileErr);
    if (rteFileErr.sp5fe_result != vf_ok) {
      nFuncStatus = DBMSrvDep_DirtyReply(replyData, replyLen).startWithMessageList(DBMSrvMsg_RTEError(rteFileErr));
    } else {
      bHelpFileOpen = true;
    } /* end if */
  } /* end if */

  /* File abarbeiten */
  if (nFuncStatus == OK_CN00) {
    if (bNewSyntax) {
      nFuncStatus = cn21_ReadNewHelpFileContents(pXParamData, replyData, replyLen, szFile, hHelpFile);
    } /* end if */
  } /* end if */

  /* close XParam help file (close always if open, independent from nFuncStatus) */
  if (bHelpFileOpen) {
    sqlfclosec ( hHelpFile, sp5vf_close_normal, &rteFileErr );
    /* set errortext only, if there no preceding error */
    if  ( (rteFileErr.sp5fe_result != vf_ok) && (nFuncStatus == OK_CN00) ) {
      nFuncStatus = DBMSrvDep_DirtyReply(replyData, replyLen).startWithMessageList(DBMSrvMsg_RTEError(rteFileErr));
    } /* end if */
  } /* end if */

  return nFuncStatus;

} /* cn21ReadHelpFile */

/* ------------------------------------------------------------------
 * IMPLEMENATION PRIVATE FUNCTION cn21_ReadNewHelpFileContents
 * ------------------------------------------------------------------
 * purpose: read and analyze the contents of new XParam help file
 *
 */
static tcn00_Error cn21_ReadNewHelpFileContents (
      DBMSrvPar_Parameters * pXParamData,
      char              * replyData,
      int               * replyLen,
      char              * szFile,
      tsp00_Int4            hHelpFile )
{
  tcn00_Error           nFuncStatus  = OK_CN00;
  tcn002_XPError        nSyntaxError = XPOK_CN002;
  tsp05_RteFileError    rteFileErr;
  tsp00_Longint           nRead;
  char                  szBuffer[XP_LINE_LENGTH_MXCN002 + 1];
  char                  szTemp[XP_LINE_LENGTH_MXCN002 + 1];
  DBMSrvPar_XPLine    * pLine    = NULL;
  DBMSrvPar_XPLine::LineType       currLine = DBMSrvPar_XPLine::XPLineUnknown;
  DBMSrvPar_XPLine::LineType       lastLine = DBMSrvPar_XPLine::XPLineBOF;      /* start at Begin Of File */
  bool                  bFollow  = false;
  bool                  bString  = false;
  bool                  bIfOpen  = false;
  long                  nLine    = 0;
  DBMSrvPar_Parameter * pCurrent = NULL;

  struct {
    DBMSrvPar_XPLine::LineType xpLineType;
    const char     * szKeyword;
  }                    LineToKeyTable[] = XP_KEYWORD_LINE_CN002;
  long                 i                = 0;


  /* looping around the help file */
  do {
    sqlfreadc (hHelpFile, szBuffer, XP_LINE_LENGTH_MXCN002, &nRead, &rteFileErr);

    currLine = DBMSrvPar_XPLine::XPLineUnknown;

    if  ( rteFileErr.sp5fe_result == vf_ok) {

      /* count of lines */
      nLine++;
      /* get a copy for analysis */
      SAPDB_strcpy(szTemp, szBuffer);
      /* remove tabs */
      char * pTab = &szTemp[0];
      while (*pTab != '\0') {
        *pTab = (*pTab == '\t') ? ' ' : *pTab;
        ++pTab;
      } // end while
      /* strip leading and trailing whitespaces */
      cn90Strip(szTemp);

      /* if in EXPLAIN or HELP check only for ENDEXPLAIN and ENDHELP */
      if ( ( (lastLine == DBMSrvPar_XPLine::XPLineEXPLAIN) || (lastLine == DBMSrvPar_XPLine::XPLineEXPLAINFollow) ) &&
             !cn90CheckKeyword(szTemp, XP_KEY_ENDEXPLAIN_CN002)                 ) {

        lastLine = DBMSrvPar_XPLine::XPLineEXPLAINFollow;
        nFuncStatus = cn21_AddLine(pCurrent,  szBuffer, DBMSrvPar_XPLine::XPLineEXPLAINFollow, nLine, &pLine);

      } else if ( ( (lastLine == DBMSrvPar_XPLine::XPLineHELP) || (lastLine == DBMSrvPar_XPLine::XPLineHELPFollow) ) &&
                     !cn90CheckKeyword(szTemp, XP_KEY_ENDHELP_CN002)         ) {

        lastLine = DBMSrvPar_XPLine::XPLineHELPFollow;
        nFuncStatus = cn21_AddLine(pCurrent,  szBuffer, DBMSrvPar_XPLine::XPLineHELPFollow, nLine, &pLine);

      } else {

        /* strip comment */
        i = 0;
        bString = false;
        while (szTemp[i] != CHAR_STRINGTERM_CN90) {
          if (szTemp[i] == XP_CHAR_STRING_CN002) {
            bString = !bString;
          } /* end if */
          if ((szTemp[i] == XP_CHAR_COMMENT_CN002) && !bString) {
            szTemp[i] = CHAR_STRINGTERM_CN90;
          } /* end if */
          i++;
        } /* end while */

        /* not empty? */
        if (strlen(szTemp) > 0) {

          /* its a follow line? */
          if (!bFollow) {

            /* detect line type by looping through the Keyword-To-Linetype-Table */
            i = 0;
            while ( (LineToKeyTable[i].xpLineType != DBMSrvPar_XPLine::XPLineUnknown) &&
                    !cn90CheckKeyword(szTemp, LineToKeyTable[i].szKeyword) ) {
              i++;
            } /* end while */
            currLine = LineToKeyTable[i].xpLineType;

            /* now we have a valid type of line or an syntax error */
            if (currLine == DBMSrvPar_XPLine::XPLineUnknown) {
              nFuncStatus  =  ERR_XPSYNTAX_CN00;
              nSyntaxError =  XPERR_NOTKEY_CN002;
            } else if (!cn21_CheckLineSequence(currLine, lastLine)) {
              nFuncStatus  =  ERR_XPSYNTAX_CN00;
              nSyntaxError =  XPERR_LINESEQ_CN002;
            } else {

              /* look for follow sign */
              if (szTemp[strlen(szTemp) - 1] == XP_CHAR_NEWFOLLOW_CN002) {
                szTemp[strlen(szTemp) - 1] = CHAR_STRINGTERM_CN90;

                /* its only valid for lines with expressions, otherwise error */
                switch (currLine) {
                  case DBMSrvPar_XPLine::XPLineCodeIF:
                  case DBMSrvPar_XPLine::XPLineCodeELSIF:
                  case DBMSrvPar_XPLine::XPLineCodeCALC:
                  case DBMSrvPar_XPLine::XPLineCodeCONSTRAINT:
                    bFollow = true;
                    break;
                  default:
                    nFuncStatus  =  ERR_XPSYNTAX_CN00;
                    nSyntaxError =  XPERR_INVFOLLOW_CN002;
                    break;
                } /* end switch */
              } else {
                bFollow = false;
              } /* end if */

              /* checking IF-ELSIF-ELSE */
              if (nFuncStatus == OK_CN00) {
                switch (currLine) {
                  case DBMSrvPar_XPLine::XPLineCodeIF:
                    bIfOpen = true;
                    break;
                  case DBMSrvPar_XPLine::XPLineCodeELSIF:
                    nFuncStatus  = bIfOpen ? nFuncStatus : ERR_XPSYNTAX_CN00;
                    nSyntaxError = bIfOpen ? nSyntaxError : XPERR_IFSEQ_CN002;
                    bIfOpen = true;
                    break;
                  case DBMSrvPar_XPLine::XPLineCodeELSE:
                    nFuncStatus  = bIfOpen ? nFuncStatus : ERR_XPSYNTAX_CN00;
                    nSyntaxError = bIfOpen ? nSyntaxError : XPERR_IFSEQ_CN002;
                    bIfOpen = false;
                    break;
                  case DBMSrvPar_XPLine::XPLineCodeCALC:
                  case DBMSrvPar_XPLine::XPLineCodeCONSTRAINT:
                    if ((lastLine == DBMSrvPar_XPLine::XPLineCodeCALC) || (lastLine == DBMSrvPar_XPLine::XPLineCodeCONSTRAINT)) {
                      bIfOpen = false;
                    } /* end if */
                    break;
                } /* end switch */
              } /* end if */

              if (nFuncStatus == OK_CN00) {
                /* do something with the line */
                switch (currLine) {
                  case DBMSrvPar_XPLine::XPLineID:
                    /* read identifier */
                    nFuncStatus = cn21_AddParameter(pXParamData, szTemp, pCurrent, &nSyntaxError);
                    break;
                  case DBMSrvPar_XPLine::XPLineTYPE:
                    /* read type */
                    nFuncStatus = cn21_ReadType(pCurrent, szTemp, &nSyntaxError);
                    break;
                  case DBMSrvPar_XPLine::XPLineDEFAULT:
                    /* read default */
                    nFuncStatus = cn21_ReadDefault(pCurrent, szTemp, &nSyntaxError);
                    break;
                  case DBMSrvPar_XPLine::XPLineDEFAULTFOR:
                    /* read default */
                    nFuncStatus = cn21_ReadDefaultFor(pCurrent, szTemp, &nSyntaxError);
                    break;
                  case DBMSrvPar_XPLine::XPLineProperty:
                    /* read property */
                    nFuncStatus = cn21_ReadProperty(pCurrent, szTemp, &nSyntaxError);
                  case DBMSrvPar_XPLine::XPLineCODE:
                  case DBMSrvPar_XPLine::XPLineENDCODE:
                  case DBMSrvPar_XPLine::XPLineEXPLAIN:
                  case DBMSrvPar_XPLine::XPLineENDEXPLAIN:
                  case DBMSrvPar_XPLine::XPLineHELP:
                  case DBMSrvPar_XPLine::XPLineENDHELP:
                    /* do nothing */
                    break;
                  case DBMSrvPar_XPLine::XPLineCodeIF:
                  case DBMSrvPar_XPLine::XPLineCodeELSIF:
                  case DBMSrvPar_XPLine::XPLineCodeELSE:
                  case DBMSrvPar_XPLine::XPLineCodeCALC:
                    /* add line */
                    nFuncStatus = cn21_AddLine(pCurrent,  szTemp, currLine, nLine, &pLine);
                    break;
                  case DBMSrvPar_XPLine::XPLineCodeCONSTRAINT:
                    if (pCurrent->pConstraint != NULL) {
                      nFuncStatus  =  ERR_XPSYNTAX_CN00;
                      nSyntaxError =  XPERR_DUPCONSTR_CN002;
                    } else {
                      /* add line */
                      nFuncStatus = cn21_AddLine(pCurrent,  szTemp, currLine, nLine, &pLine);
                    } // end if
                    break;
                  default:
                    nFuncStatus  =  ERR_XPSYNTAX_CN00;
                    nSyntaxError =  XPERR_LINEDET_CN002;
                    break;
                } /* end switch */
              } /* end if */
            } /* end if */

            lastLine = currLine;
          } else {
            /* look also for follow sign */
            if (szTemp[strlen(szTemp) - 1] == XP_CHAR_NEWFOLLOW_CN002) {
              szTemp[strlen(szTemp) - 1] = CHAR_STRINGTERM_CN90;
              bFollow = true;
            } else {
              bFollow = false;
            } /* end if */
            /* follow line - add to the previuos line */
            nFuncStatus = cn21_AddFollowLine(pLine, szTemp);
          } /* end if (bFollow) */
        } /* end if (empty line) */
      } /* end if (in HELP or EXPLAIN) */
    } /* end if (RTE error) */

  } while ( ( rteFileErr.sp5fe_result  == vf_ok             ) &&
            ( rteFileErr.sp5fe_warning == sp5vfw_no_warning ) &&
            ( nFuncStatus              == OK_CN00           )     );

  /* provide information about RTE error */
  if ( ( (rteFileErr.sp5fe_result != vf_ok)     &&
         (rteFileErr.sp5fe_result != vf_eof)        ) ||
     (rteFileErr.sp5fe_warning != sp5vfw_no_warning )    ) {
    nFuncStatus = DBMSrvDep_DirtyReply(replyData, replyLen).startWithMessageList(DBMSrvMsg_RTEError(rteFileErr));
  } /* end if */

  /* is EOF ok? */
  if (nFuncStatus == OK_CN00) {
    if (!cn21_CheckLineSequence(DBMSrvPar_XPLine::XPLineEOF, lastLine)) {
      nFuncStatus  =  ERR_XPSYNTAX_CN00;
      nSyntaxError =  XPERR_EOF_CN002;
    } /* end if */
  } /* end if */

  if ((nFuncStatus != OK_CN00) && (nFuncStatus != DBMSrvMsg_Error::get_tcn00_Error(DBMSrvMsg_Error::DBMSrv_RTE))) {
    /* internal error */
    cn90AnswerIError(replyData, replyLen, nFuncStatus);

    if (nFuncStatus == ERR_XPSYNTAX_CN00) {
      /* add information about syntax error*/
      cn29SyntaxError(replyData, replyLen, nSyntaxError, szBuffer, szFile, nLine);
    } /* end if */
  } /* end if */

  return nFuncStatus;

} /* end cn21_ReadNewHelpFileContents */

/* ------------------------------------------------------------------
 * IMPLEMENATION PRIVATE FUNCTION cn21_CheckLineSequence
 * ------------------------------------------------------------------
 * purpose: checks the sequence of line in help file using the
 *          transition table
 *
 */
static bool    cn21_CheckLineSequence (
      DBMSrvPar_XPLine::LineType currLine,
      DBMSrvPar_XPLine::LineType lastLine)
{
  bool    bValid = false;
  long    i      = 0;
  struct {
    DBMSrvPar_XPLine::LineType  xpLast;
    DBMSrvPar_XPLine::LineType  xpCurr;
  }       LineTable[] = XP_LINE_TRANSITIONS_CN002;

  while (!bValid && (LineTable[i].xpLast != DBMSrvPar_XPLine::XPLineUnknown)) {
    bValid = (LineTable[i].xpLast == lastLine) && (LineTable[i].xpCurr == currLine);
    i++;
  } /* end while */

  return bValid;
} /* end cn21_CheckLineSequence */

/* ------------------------------------------------------------------
 * IMPLEMENATION PRIVATE FUNCTION cn21_GetSecondToken
 * ------------------------------------------------------------------
 * purpose: looks for the second (and last!) token
 */
static tcn00_Error cn21_GetSecondToken (
      char             * szBuffer,
      char            ** pValue,
      tcn002_XPError    * pSyntaxError)
{
  tcn00_Error        nFuncStatus = OK_CN00;
  static char        szToken[XP_STRING_LENGTH_MXCN002 + 1];

  if (cn90GetToken(szBuffer, NULL, 3, -1)) {
    nFuncStatus = ERR_XPSYNTAX_CN00;
    *pSyntaxError = XPERR_TOKEN_CN002;
  } else if (cn90GetToken(szBuffer, szToken, 2, XP_STRING_LENGTH_MXCN002 + 1)) {
    if (strlen(szToken) == 0) {
      nFuncStatus = ERR_XPSYNTAX_CN00;
      *pSyntaxError = XPERR_INVVALUE_CN002;
    } /* end if */
  } else {
    nFuncStatus = ERR_XPSYNTAX_CN00;
    *pSyntaxError = XPERR_INVVALUE_CN002;
  } /* end if */

  if (nFuncStatus == OK_CN00) {
    *pValue = szToken;
  } /* end if */

  return nFuncStatus;
} /* end cn21_GetSecondToken */

/* ------------------------------------------------------------------
 * IMPLEMENATION PRIVATE FUNCTION cn21_AddParameter
 * ------------------------------------------------------------------
 * purpose: create a new Parameter struct an read indentifier
 */
static tcn00_Error cn21_AddParameter (
      DBMSrvPar_Parameters * pXParamData,
      char                 * szBuffer,
      DBMSrvPar_Parameter  * & pParameter,
      tcn002_XPError       * pSyntaxError)
{
  DBMSrvPar_Parameter  * pLast   = pParameter;
  tcn00_Error        nFuncStatus = OK_CN00;
  char             * pName       = NULL;

  pParameter = NULL;

  // read the name
  nFuncStatus = cn21_GetSecondToken(szBuffer, &pName, pSyntaxError);

  // check for duplicates
  if (nFuncStatus == OK_CN00) {
    if (cn21_SearchParameter(pXParamData->pFirst, pName) != NULL) {
      nFuncStatus = ERR_XPSYNTAX_CN00;
      *pSyntaxError = XPERR_DUPLICATE_CN002;
    } // end if
  } /* end if */
  
  // list handling
  if (nFuncStatus == OK_CN00) {
    pParameter = new DBMSrvPar_Parameter;
    if (pParameter == NULL) {
      nFuncStatus = ERR_MEM_CN00;
    } else {
      /* add the parameter in the list of parameters in the XParam section */
      if (pXParamData->pFirst == NULL) {
        pXParamData->pFirst = pParameter;
      } /* end if */
      if (pLast != NULL) {
        pLast->pNext = pParameter;
      } // end if
    } /* end if */
  } /* end if */

  // save data to item
  if (nFuncStatus == OK_CN00) {
    /* check length of identifier */
    if ((int) strlen(pName) > RTECONF_MAXNAMELENGTH ) {
      nFuncStatus = ERR_XPSYNTAX_CN00;
      *pSyntaxError = XPERR_LONGID_CN002;
    } else {
       if (isalpha(pName[0]) || (pName[0] == XP_CHAR_UNDERLINE_CN002) ) {
         /* copy identifier */
         cn90SaveStrcpy(pParameter->szParameterName, pName, sizeof(tsp00_XpKeyTypec));
        /* init properties */
         cn21_InitProperties (pParameter);
         /* init flags */
         pParameter->bHelpExist  = true;

         pXParamData->Put(pParameter);
      } else {
        nFuncStatus = ERR_XPSYNTAX_CN00;
        *pSyntaxError = XPERR_INVID_CN002;
      } /* end if */
    } /* end if */
  } /* end if */

  return nFuncStatus;
} /* end cn21_AddParameter */

/* ------------------------------------------------------------------
 * IMPLEMENATION PRIVATE FUNCTION cn21_ReadType
 * ------------------------------------------------------------------
 * purpose: analyze the type line
 */
static tcn00_Error cn21_ReadType (
      DBMSrvPar_Parameter * pParameter,
      char             * szBuffer,
      tcn002_XPError    * pSyntaxError)
{
  tcn00_Error        nFuncStatus = OK_CN00;
  char             * pType       = NULL;
  struct {
    DBMSrvPar_RTEConfParameter::Type   nValue;
    const char              * szText;
  }                 aTable[]    = XP_TYPES_CN002;
  long              i            = 0;

  nFuncStatus = cn21_GetSecondToken(szBuffer, &pType, pSyntaxError);

  if (nFuncStatus == OK_CN00) {
    /* run through the type table */
    while ( (strlen(aTable[i].szText) > 0) && (strcmp(pType, aTable[i].szText) != 0) ) {
      i++;
    } /* end while */

    if (strlen(aTable[i].szText) == 0) {
      nFuncStatus = ERR_XPSYNTAX_CN00;
      *pSyntaxError = XPERR_INVTYPE_CN002;
    } /* end if */

    pParameter->xpRecord.xpType = aTable[i].nValue;
  } /* end if */

  return nFuncStatus;
} /* end cn21_ReadType */

/* ------------------------------------------------------------------
 * IMPLEMENATION PRIVATE FUNCTION cn21_ReadDefault
 * ------------------------------------------------------------------
 * purpose: analyze the Default line
 */
static tcn00_Error cn21_ReadDefault (
      DBMSrvPar_Parameter * pParameter,
      char             * szBuffer,
      tcn002_XPError    * pSyntaxError)
{
  tcn00_Error        nFuncStatus  = OK_CN00;
  char             * pDefault     = NULL;
  long               nValue       = 0;
  bool               bValid       = false;

  nFuncStatus = cn21_GetSecondToken(szBuffer, &pDefault, pSyntaxError); 

  if (nFuncStatus == OK_CN00) {
    switch(pParameter->xpRecord.xpType) {
      case DBMSrvPar_RTEConfParameter::t_Integer:
        bValid = cn90AnalyzeNumber(pDefault, &nValue, true);
        if (bValid) {
          cn90SaveStrcpy(pParameter->szDefValue, pDefault, XP_STRING_LENGTH_MXCN002 + 1);
          cn90SaveStrcpy(pParameter->szNewValue, pDefault, XP_STRING_LENGTH_MXCN002 + 1);
        } /* end if */
        break ;
      case DBMSrvPar_RTEConfParameter::t_Real:
        bValid = false;
        break ;
      case DBMSrvPar_RTEConfParameter::t_CryptInfo:
        bValid = false;
        break;
      case DBMSrvPar_RTEConfParameter::t_String:
        bValid = cn90AnalyzeString(pDefault,   pParameter->szDefValue, XP_STRING_LENGTH_MXCN002);
        cn90SaveStrcpy(pParameter->szNewValue, pParameter->szDefValue, XP_STRING_LENGTH_MXCN002 + 1);
        break ;
    } /* end switch */

    if (!bValid) {
      nFuncStatus = ERR_XPSYNTAX_CN00;
      *pSyntaxError = XPERR_INVDEF_CN002;
    } /* end if */
  } /* end if */

  return nFuncStatus;
} /* end cn21_ReadDefault */

/* ------------------------------------------------------------------
 * IMPLEMENATION PRIVATE FUNCTION cn21_ReadDefaultFor
 * ------------------------------------------------------------------
 * purpose: analyze the Default line
 */
static tcn00_Error cn21_ReadDefaultFor (
      DBMSrvPar_Parameter * pParameter,
      char              * szBuffer,
      tcn002_XPError    * pSyntaxError)
{
  tcn00_Error        nFuncStatus  = OK_CN00;
  _TCHAR             szInstance[XP_STRING_LENGTH_MXCN002 + 1];
  _TCHAR             szValue[XP_STRING_LENGTH_MXCN002 + 1];
  int                nIndex = 0;

  long               nValue       = 0;
  bool               bValid       = false;

  // read token
  if (!cn90GetToken(szBuffer, szInstance, 2, XP_STRING_LENGTH_MXCN002 + 1) || !cn90GetToken(szBuffer, szValue, 3, XP_STRING_LENGTH_MXCN002 + 1)) {
    nFuncStatus  = ERR_XPSYNTAX_CN00;
    *pSyntaxError = XPERR_INVDEF_CN002;
  } // end if

  // check free DefaultFor-Entry
  if (nFuncStatus == OK_CN00) {
    for (nIndex = 0; nIndex < XP_DEFAULTFOR_MXCN002; ++nIndex) {
      if (_tcslen(pParameter->szDefaultFor[nIndex][XP_DEFAULTFORINSTANCE_CN002]) == 0) {
        break;
      } // end if
    } // end for
    if (nIndex == XP_DEFAULTFOR_MXCN002) {
      nFuncStatus  = ERR_XPSYNTAX_CN00;
      *pSyntaxError = XPERR_INVDEF_CN002;
    } // end if
  } // end if

  // copy Value
  if (nFuncStatus == OK_CN00) {
    switch(pParameter->xpRecord.xpType) {
      case DBMSrvPar_RTEConfParameter::t_Integer:
        bValid = cn90AnalyzeNumber(szValue, &nValue, true);
        if (bValid) {
          cn90SaveStrcpy(pParameter->szDefaultFor[nIndex][XP_DEFAULTFORVALUE_CN002], szValue, XP_STRING_LENGTH_MXCN002 + 1);
        } /* end if */
        break ;
      case DBMSrvPar_RTEConfParameter::t_Real:
        bValid = false;
        break ;
      case DBMSrvPar_RTEConfParameter::t_CryptInfo:
        bValid = false;
        break;
      case DBMSrvPar_RTEConfParameter::t_String:
        bValid = cn90AnalyzeString(szValue, pParameter->szDefaultFor[nIndex][XP_DEFAULTFORVALUE_CN002], XP_STRING_LENGTH_MXCN002);
        break ;
    } /* end switch */

    if (!bValid) {
      nFuncStatus = ERR_XPSYNTAX_CN00;
      *pSyntaxError = XPERR_INVDEF_CN002;
    } else {
      bValid = cn90AnalyzeString(szInstance, pParameter->szDefaultFor[nIndex][XP_DEFAULTFORINSTANCE_CN002], XP_STRING_LENGTH_MXCN002);
      if (!bValid) {
        nFuncStatus = ERR_XPSYNTAX_CN00;
        *pSyntaxError = XPERR_INVDEF_CN002;
      } /* end if */
    } /* end if */
  } /* end if */

  return nFuncStatus;
} /* end cn21_ReadDefaultFor */

/* ------------------------------------------------------------------
 * IMPLEMENATION PRIVATE FUNCTION cn21_ReadProperty
 * ------------------------------------------------------------------
 * purpose: analyze the property line
 */
static tcn00_Error cn21_ReadProperty (
      DBMSrvPar_Parameter * pParameter,
      char                * szBuffer,
      tcn002_XPError      * pSyntaxError)
{
  tcn00_Error           nFuncStatus    = OK_CN00;
  tsp00_C70c            szProperty;
  char                * pValue;
  DBMSrvPar_Parameter::XPPropValue     nValue        = DBMSrvPar_Parameter::XPPropValueUnknown;
  tcn002_XPPropClass     nClass        = XPPropClassUnknown;
  struct {
    DBMSrvPar_Parameter::XPPropValue   nValue;
    const char        * szText;
  }                     aValueTable[] = XP_PROP_VALUES_CN002;
  struct  {
    tcn002_XPPropClass   nClass;
    DBMSrvPar_Parameter::XPPropValue   nDefault;
    const char        * szText;
  }                     aClassTable[] = XP_PROP_CLASSES_CN002;
  struct {
    tcn002_XPPropClass   nClass;
    DBMSrvPar_Parameter::XPPropValue   nValue;
  }                     aValidTable[] = XP_PROP_VALID_CN002;
  long                  i             = 0;
  long                  j             = 0;
  long                  k             = 0;
  long                  l             = 0;

  /* knock down to tokens */
  szProperty.Init();
  cn90GetToken(szBuffer, szProperty, 1, szProperty.size());
  pValue = &szBuffer[strlen(szProperty)];
  cn90StripTrailingChar(pValue, '\n');
  cn90Strip(pValue);

  /* check existence of value */
  if (pValue == NULL) {
    nFuncStatus = ERR_XPSYNTAX_CN00;
    *pSyntaxError = XPERR_INVVALUE_CN002;
  } else if (strlen(pValue) == 0) {
    nFuncStatus = ERR_XPSYNTAX_CN00;
    *pSyntaxError = XPERR_INVVALUE_CN002;
  } else {

    /* search class */
    while ( (aClassTable[i].nClass != XPPropClassUnknown) && (strcmp(aClassTable[i].szText, szProperty) != 0) ) {
      i++;
    } /* end while */
    /* search value */
    while ( (aValueTable[j].nValue != DBMSrvPar_Parameter::XPPropValueUnknown) && (strcmp(aValueTable[j].szText, pValue) != 0) ) {
      j++;
    } /* end while */

    if (aClassTable[i].nClass == XPPropClassUnknown) {
      nFuncStatus = ERR_XPSYNTAX_CN00;
      *pSyntaxError = XPERR_INVPROP_CN002;
    } else {
      /* is combination valid */
      while (  ( aValidTable[k].nClass != XPPropClassUnknown           ) &&
              !( ( aValidTable[k].nClass == aClassTable[i].nClass ) &&
                  ( aValidTable[k].nValue == aValueTable[j].nValue )    )    ) {
        k++;
      } /* end while */

      if (aValidTable[k].nClass != XPPropClassUnknown) {
        pParameter->xpProperties[aClassTable[i].nClass] = aValueTable[j].nValue;
        SAPDB_strcpy(pParameter->szProperties[aClassTable[i].nClass], pValue);

        // handling for group property
        if (aClassTable[i].nClass == XPPropClassGroup) {
          struct {
            DBMSrvPar_Parameter::XPClassValue   nValue;
            const char        * szText;
          }                     aClassValueTable[] = XP_CLASS_VALUES_CN002;
          l = 0;
          /* search class value*/
          while ( (aClassValueTable[l].nValue != DBMSrvPar_Parameter::XPClassValueUnknown) && (strcmp(aClassValueTable[l].szText, pValue) != 0) ) {
            l++;
          } /* end while */
          if (aClassValueTable[l].nValue != DBMSrvPar_Parameter::XPClassValueUnknown) {
            pParameter->nClasses = pParameter->nClasses | aClassValueTable[l].nValue;
          } // end if
        } // end if

        // handling for class property
        if (aClassTable[i].nClass == XPPropClassClass) {
          tsp00_C70c szClass;
          int        nToken = 1;
          struct {
            DBMSrvPar_Parameter::XPClassValue   nValue;
            const char        * szText;
          }                     aClassValueTable[] = XP_CLASS_VALUES_CN002;

          while (cn90GetToken(pValue, szClass, nToken, szClass.size())) {
            l = 0;
            /* search class value*/
            while ( (aClassValueTable[l].nValue != DBMSrvPar_Parameter::XPClassValueUnknown) && (strcmp(aClassValueTable[l].szText, szClass) != 0) ) {
              l++;
            } /* end while */
            if (aClassValueTable[l].nValue == DBMSrvPar_Parameter::XPClassValueUnknown) {
              nFuncStatus = ERR_XPSYNTAX_CN00;
              *pSyntaxError = XPERR_INVPROP_CN002;
              break;
            } else {
              pParameter->nClasses = pParameter->nClasses | aClassValueTable[l].nValue;
            } // end if

            ++nToken;
          } // end while

          // simulate group values
          if (pParameter->xpProperties[XPPropClassGroup] == DBMSrvPar_Parameter::XPPropValueUnknown) {
            if (pParameter->nClasses | DBMSrvPar_Parameter::XPClassValueGeneral) {
              pParameter->xpProperties[XPPropClassGroup] = DBMSrvPar_Parameter::XPPropValueGeneral;
            } else if (pParameter->nClasses | DBMSrvPar_Parameter::XPClassValueExtended) {
              pParameter->xpProperties[XPPropClassGroup] = DBMSrvPar_Parameter::XPPropValueExtended;
            } else if (pParameter->nClasses | DBMSrvPar_Parameter::XPClassValueSupport) {
              pParameter->xpProperties[XPPropClassGroup] = DBMSrvPar_Parameter::XPPropValueSupport;
            } else if (pParameter->nClasses | DBMSrvPar_Parameter::XPClassValueTrace) {
              pParameter->xpProperties[XPPropClassGroup] = DBMSrvPar_Parameter::XPPropValueTrace;
            } else if (pParameter->nClasses | DBMSrvPar_Parameter::XPClassValueInfo) {
              pParameter->xpProperties[XPPropClassGroup] = DBMSrvPar_Parameter::XPPropValueInfo;
            } else if (pParameter->nClasses | DBMSrvPar_Parameter::XPClassValueNothing) {
              pParameter->xpProperties[XPPropClassGroup] = DBMSrvPar_Parameter::XPPropValueNo;
            } // end if
          } // end if

        } // end if

      } else {
        nFuncStatus = ERR_XPSYNTAX_CN00;
        *pSyntaxError = XPERR_INVPROP_CN002;
      } /* end if */
    } /* end if */
  } /* end if */

  return nFuncStatus;
} /* end cn21_ReadProperty */

/* ------------------------------------------------------------------
 * IMPLEMENATION PRIVATE FUNCTION cn21_InitProperties
 * ------------------------------------------------------------------
 * purpose: init properties of parameter
 */
static void cn21_InitProperties (
      DBMSrvPar_Parameter * pParameter)
{
  tcn00_Error           nFuncStatus      = OK_CN00;
  struct tcn002_PropClassTable {
    tcn002_XPPropClass   nClass;
    DBMSrvPar_Parameter::XPPropValue   nDefault;
    const char        * szText;
  }                     aClassTable[] = XP_PROP_CLASSES_CN002;
  long                  i             = 0;

  /* search class */
  for (i = 0; i < XPPropClassMAX; i++) {
    pParameter->xpProperties[i] = aClassTable[i].nDefault;
    SAPDB_strcpy(pParameter->szProperties[i], "");
  } /* end for */

} /* end cn21_InitProperties */

/* IMPLEMENATION PRIVATE FUNCTION cn21_AddLine
 * ------------------------------------------------------------------
 * purpose: add a line of specified type
 */
static tcn00_Error cn21_AddLine (
      DBMSrvPar_Parameter * pParameter,
      char              * szBuffer,
      DBMSrvPar_XPLine::LineType     xpLine,
      long                nNumber,
      DBMSrvPar_XPLine ** pOut)
{
  tcn00_Error        nFuncStatus = OK_CN00;
  DBMSrvPar_XPLine * pLine       = NULL;
  char             * pLineText   = NULL;
  bool               bMallocOK   = true;
  DBMSrvPar_XPLine * pCurrent    = NULL;

  pLine = new DBMSrvPar_XPLine(szBuffer, xpLine, nNumber);

  if (pLine == NULL || pLine->Line() == NULL) {
    if (pLine != NULL) {
      delete pLine;
    } // end if
    nFuncStatus = ERR_MEM_CN00;
  } else {
    *pOut = pLine;

    /* use the right list */
    switch (pLine->Type()) {
      case DBMSrvPar_XPLine::XPLineCodeCALC:
      case DBMSrvPar_XPLine::XPLineCodeIF:
      case DBMSrvPar_XPLine::XPLineCodeELSIF:
      case DBMSrvPar_XPLine::XPLineCodeELSE:
        pCurrent = pParameter->pCode;
        if (pCurrent == NULL) {
          pParameter->pCode = pLine;
        } /* end if */
        break;
      case DBMSrvPar_XPLine::XPLineCodeCONSTRAINT:
        pCurrent = pParameter->pConstraint;
        if (pCurrent == NULL) {
          pParameter->pConstraint = pLine;
        } /* end if */
        break;
      case DBMSrvPar_XPLine::XPLineEXPLAINFollow:
        pCurrent = pParameter->pExplain;
        if (pCurrent == NULL) {
          pParameter->pExplain = pLine;
        } /* end if */
        break;
      case DBMSrvPar_XPLine::XPLineHELPFollow:
        pCurrent = pParameter->pHelp;
        if (pCurrent == NULL) {
          pParameter->pHelp = pLine;
        } /* end if */
        break;
      default:
        /* line detection error */
        nFuncStatus = ERR_XPSYNTAX_CN00;
        break;
    } /* end switch */

    if (pCurrent != NULL) {
      while (pCurrent->Next() != NULL) {
        pCurrent = pCurrent->Next();
      } /* end while */
      pCurrent->SetNext(pLine);
    } /* end if */
  } /* end if */

  return nFuncStatus;
} /* end cn21_AddLine */

/* ------------------------------------------------------------------
 * IMPLEMENATION PRIVATE FUNCTION cn21_AddFollowLine
 * ------------------------------------------------------------------
 * purpose: add a line of specified type
 */
static tcn00_Error cn21_AddFollowLine (
      DBMSrvPar_XPLine * pLine,
      char             * szBuffer)
{
  tcn00_Error        nFuncStatus = OK_CN00;

  if (pLine != NULL) {
    if (pLine->Append(szBuffer) == NULL) {
      nFuncStatus = ERR_MEM_CN00;
    } /* end if */
  } else {
    nFuncStatus = ERR_MEM_CN00;
  } /* end if */

  return nFuncStatus;
} /* end cn21_AddFollowLine */

/* IMPLEMENTATION INTERN FUNCTION cn21_SearchParameter
 * ------------------------------------------------------------------
 */
DBMSrvPar_Parameter * cn21_SearchParameter
      ( DBMSrvPar_Parameter * pParameter,
        const char        * szName )
{
  bool                 bFound = false;

  /* search for dynamic match */
  bFound = false;
  while ((pParameter != NULL) && (bFound == false)) {
    if (strcmp(pParameter->szParameterName, szName) == 0) {
      bFound = true;
    } else {
      pParameter = pParameter->pNext;
    } /* end if */
  } /* end while */

  return pParameter;
} /* end cn21_SearchParameter */
