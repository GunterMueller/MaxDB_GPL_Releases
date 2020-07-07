/* @lastChanged: "1998-06-24  10:45"
 *
 * @filename:    vcn81.cpp
 * @purpose:     "show info views"
 * @release:     7.1.0.0
 * @see:         "-.-"
 *
 * @Copyright (c) 1998-2005 SAP AG"
 *
 * ==================================================================
 *
 * responsible:   Martin Reinecke (D024853 - Martin.Reinecke@SAP-AG.de)
 *                Bernd Vorsprach (D025588 - Bernd.Vorsprach@SAP-AG.de)
 *
 * created:       1998-12-04 by Martin Reinecke
 *
 * purpose:       show info views
 *
 * ==================================================================


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

/* ------------------------------------------------------------------
 * includes
 * ------------------------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "DBM/Srv/DBMSrv_Reply.hpp"
#include "SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_GlobalSessions.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_SQLError.hpp"

#include "hcn50.h"
#include "hcn90.h"

#include "hcn81.h"

/* ------------------------------------------------------------------
 * SPECIFICATION PRIVATE TYPES/MACROS
 * ------------------------------------------------------------------
 */
typedef struct InfoNameT {
    const char * szName;
    const char * szView;
    bool         bOneRowResult;
} InfoNameT;

static InfoNameT InfoNamesC[] = {
  {"INFOS"       , "SYSDBM.INFO_INFOS"               , false },
  {"STATE"       , "SYSDBM.INFO_STATE"               , true  },
  {"IO"          , "SYSDBM.INFO_IO_STATISTICS"       , false },
  {"CACHES"      , "SYSDBM.INFO_CACHE_STATISTICS"    , false },
  {"DEVSPACES"   , "SYSDBM.INFO_DEVSPACES"           , false },
  {"BADDEVSPACES", "SYSDBM.INFO_BAD_DEVSPACES"       , false },
  {"DATA"        , "SYSDBM.INFO_DATADEV_STATE"       , true  },
  {"LOG"         , "SYSDBM.INFO_LOG_STATE"           , true  },
  {"LOCKS"       , "SYSDBM.INFO_LOCKS"               , true  },
  {"USERS"       , "SYSDBM.INFO_CONNECTED_USERS"     , false },
  {"VERSIONS"    , "SYSDBM.INFO_VERSIONS"            , true  },
  {"TABLES"      , "SYSDBM.INFO_TABLES"              , false },
  {"COLUMNS"     , "SYSDBM.INFO_COLUMNS"             , false },
  {"UPDSTAT"     , "SYSDBM.INFO_UPDSTAT"             , false },
  {"UPDSTATW"    , "SYSDBM.INFO_UPDSTAT_WANTED"      , false },
  {"INDEXES"     , "SYSDBM.INFO_INDEXES"             , false },
  {"BADIDX"      , "SYSDBM.INFO_BAD_INDEXES"         , false },
  {"UNUSEDIDX"   , "SYSDBM.INFO_UNUSED_INDEXES"      , false },
  {"DISABLEDIDX" , "SYSDBM.INFO_DISABLED_INDEXES"    , false },
  {"PARAMS"      , "SYSDBM.INFO_PARAMETERS"          , false },
  {"OMSCACHES"   , "SYSDBM.INFO_OMS_CACHES"          , false },
  {"OMSMON"      , "SYSDBM.INFO_OMS_MONITOR"         , false },
  {"CLSCON"      , "SYSDBM.INFO_CLASSCONTAINERS"     , false },
  {"SYSDDSRVDB"  , "SYSDBM.INFO_SYSDD_SERVERDBSTATE" , true  },
  {"SYSDDCONFIG" , "SYSDBM.INFO_SYSDD_CONFIG"        , false },
  {"SYSDDMONITOR", "SYSDBM.INFO_SYSDD_MONITOR"       , false },
  {NULL          , NULL                              , false }
};

#define INFO_DESC_OPT     "-d"
#define INFO_WHERE_OPT    "-w"

#define CONTINUE_TXT   "CONTINUE\n"
#define END_TXT        "END     \n"
#define EMPTY_TXT      "        \n"

/* PRIVATE FUNCTION cn81_RemoveDoublequotes
 * ------------------------------------------------------------------
 * purpose: remove doublequotes from a string
 *
 */
void cn81_RemoveDoublequotes(
      char * szValue)
{
  long nLength = 0;

  nLength = (long)strlen(szValue);

  if (nLength > 1) {
    if ((szValue[0]           == '"') &&
        (szValue[nLength - 1] == '"')    ) {
      strncpy(szValue, szValue+1, nLength-2);
      szValue[nLength-2] = '\0';
    }
  }
} /* end cn81_RemoveDoublequotes */

/* PRIVATE FUNCTION cn81_InfoFetchNice
 * ------------------------------------------------------------------
 */
static tcn00_Error cn81_InfoFetchNice
  ( DBMSrvKnl_Session  & oSession,
    bool                 bDesc,
    char               * pCurrent,
    int                  replyLenMax,
    DBMSrvMsg_Error    & oSessionError )
{
  char             * pStart = pCurrent;
  DBMSrvKnl_SQLRecord & oRecord = oSession.GetRecordObject();
  int                nField         = 0;
  size_t             nMaxNameLength = 0;
  size_t             nRecord        = 0;
  bool               bEnd           = false;

  // add OK flag
  SAPDB_strcpy(pCurrent, ANSWER_OK_CN00);
  pCurrent += strlen(pCurrent);
  SAPDB_strcpy(pCurrent, LINE_SEPSTRING_CN00);
  pCurrent += strlen(pCurrent);

  // add END flag
  char * pContinue = pCurrent;
  SAPDB_strcpy(pCurrent, END_TXT);
  pCurrent += strlen(pCurrent);

  // add field types
  if (bDesc) {
    SAPDB_strcat(pCurrent, "SS");
    for (nField = 1; nField <= oRecord.fields(); ++nField) {
      switch (oRecord.fieldType(nField)) {
        case dfixed:    case dfloat:    case dvfloat:
        case dsmallint: case dinteger:
          SAPDB_strcat(pCurrent, "N");
          break;
        case dcha:      case dche:      case dchb:
        case dunicode:  case dvarchara: case dvarchare:
        case dvarcharb: case dstra:     case dstre:
        case dstrb:     case dstrdb:
          SAPDB_strcat(pCurrent, "S");
          break;
        case ddate:     case dtime:     case dtimestamp:
          SAPDB_strcat(pCurrent, "D");
          break;
        case dboolean:
          SAPDB_strcat(pCurrent, "B");
          break;
        default:
          SAPDB_strcat(pCurrent, "-");
          break;
      } // end switch
    } // end for
    SAPDB_strcat(pCurrent, LINE_SEPSTRING_CN00);
    pCurrent += strlen(pCurrent);
  } // end if

  // compute name length
  for (nField = 1; nField <= oRecord.fields(); ++nField) {
    nMaxNameLength = (nMaxNameLength > oRecord.fieldName(nField).Length())? nMaxNameLength : oRecord.fieldName(nField).Length();
    nRecord = nRecord + oRecord.maxSize(nField) + 5;
  } // end if
  nRecord = nRecord + (oRecord.fields() * nMaxNameLength);

  // print table header
  sprintf(pCurrent, "%-*s | %s%s%s", nMaxNameLength, "Name", "Value", LINE_SEPSTRING_CN00,LINE_SEPSTRING_CN00);
  pCurrent += strlen(pCurrent);

  while (!bEnd && ((int)(replyLenMax - (pCurrent - pStart)) > (int) nRecord) && oSessionError.IsEmpty()) {
    // print column lines
    for (nField = 1; nField <= oRecord.fields(); ++nField) {
      if ((int)(replyLenMax - (pCurrent - pStart)) > (int)(nMaxNameLength + 5 + oRecord.fieldAsChar(nField).Length())) {
        sprintf(pCurrent, "%-*s = %s%s", nMaxNameLength, oRecord.fieldName(nField).CharPtr(), oRecord.fieldAsChar(nField).CharPtr(), LINE_SEPSTRING_CN00);
        pCurrent += strlen(pCurrent);
      } // end if
    } // end for
    oRecord.getNextRecord(bEnd, oSessionError);
  } // end while

  if (!oSessionError.IsEmpty()) {
    return oSessionError.ID();
  } else {
    if (!bEnd) {
      SAPDB_strncpy(pContinue, CONTINUE_TXT, strlen(CONTINUE_TXT));

      // prepare next command
      cn00DBMServerData::vcontrol()->szNextCommand.rawAssign("info_next");
      cn00DBMServerData::vcontrol()->nNextCommandSkip = 3;
    } else {
      DBMSrvMsg_Error errOut;
      DBMSrvKnl_GlobalSessions::ReleaseSession(DBMSrvKnl_GlobalSessions::SessionInfo, errOut);
    } // end if
  } // end if

  return OK_CN00;
} // end cn81_InfoFetchNice

/* PRIVATE FUNCTION cn81_InfoFetch
 * ------------------------------------------------------------------
 */
static tcn00_Error cn81_InfoFetch
  ( DBMSrvKnl_Session  & oSession,
    bool                 bDesc,
    char               * pCurrent,
    int                  replyLenMax,
    DBMSrvMsg_Error    & oSessionError )
{
  char             * pStart = pCurrent;
  DBMSrvKnl_SQLRecord & oRecord = oSession.GetRecordObject();
  int                nField         = 0;
  size_t             nMaxNameLength = 0;
  size_t             nRecord        = 0;
  bool               bEnd           = false;

  // add OK flag
  SAPDB_strcpy(pCurrent, ANSWER_OK_CN00);
  pCurrent += strlen(pCurrent);
  SAPDB_strcpy(pCurrent, LINE_SEPSTRING_CN00);
  pCurrent += strlen(pCurrent);

  // add END flag
  char * pContinue = pCurrent;
  SAPDB_strcpy(pContinue, END_TXT);
  pCurrent += strlen(pCurrent);

  // add field types
  if (bDesc) {
    for (nField = 1; nField <= oRecord.fields(); ++nField) {
      switch (oRecord.fieldType(nField)) {
        case dfixed:    case dfloat:    case dvfloat:
        case dsmallint: case dinteger:
          SAPDB_strcat(pCurrent, "N");
          break;
        case dcha:      case dche:      case dchb:
        case dunicode:  case dvarchara: case dvarchare:
        case dvarcharb: case dstra:     case dstre:
        case dstrb:     case dstrdb:
          SAPDB_strcat(pCurrent, "S");
          break;
        case ddate:     case dtime:     case dtimestamp:
          SAPDB_strcat(pCurrent, "D");
          break;
        case dboolean:
          SAPDB_strcat(pCurrent, "B");
          break;
        default:
          SAPDB_strcat(pCurrent, "-");
          break;
      } // end switch
    } // end for
    SAPDB_strcat(pCurrent, LINE_SEPSTRING_CN00);
    pCurrent += strlen(pCurrent);
  } // end if

  // print table header
  for (nField = 1; nField <= oRecord.fields(); ++nField) {
    nRecord = nRecord + oRecord.maxSize(nField) + 3;
    sprintf(pCurrent, "%s%-*s | ", oRecord.fieldName(nField).CharPtr(), oRecord.maxSize(nField) - oRecord.fieldName(nField).Length(), "");
    pCurrent += strlen(pCurrent);
  } // end for
  pCurrent = pCurrent - 3;
  SAPDB_strcpy(pCurrent, LINE_SEPSTRING_CN00);
  SAPDB_strcat(pCurrent, LINE_SEPSTRING_CN00);
  pCurrent += strlen(pCurrent);

  // print column lines
  while (!bEnd && ((int)(replyLenMax - (pCurrent - pStart)) > (int) nRecord) && oSessionError.IsEmpty()) {
    for (nField = 1; nField <= oRecord.fields(); ++nField) {
      sprintf(pCurrent, "%s%-*s | ", oRecord.fieldAsChar(nField).CharPtr(), oRecord.maxSize(nField) - oRecord.fieldAsChar(nField).Length(), "");
      pCurrent += strlen(pCurrent);
    } // end for
    pCurrent = pCurrent - 3;
    SAPDB_strcpy(pCurrent, LINE_SEPSTRING_CN00);
    pCurrent += strlen(pCurrent);
    oRecord.getNextRecord(bEnd, oSessionError);
  } // end if

  if (!oSessionError.IsEmpty()) {
    return oSessionError.ID();
  } else {
    if (!bEnd) {
      SAPDB_strncpy(pContinue, CONTINUE_TXT, strlen(CONTINUE_TXT));

      // prepare next command
      cn00DBMServerData::vcontrol()->szNextCommand.rawAssign("info_next");
      cn00DBMServerData::vcontrol()->nNextCommandSkip = 3;
    } else {
      DBMSrvMsg_Error errOut;
      DBMSrvKnl_GlobalSessions::ReleaseSession(DBMSrvKnl_GlobalSessions::SessionInfo, errOut);
    } // end if
  } // end if

  return OK_CN00;
} // end cn81_InfoFetch

/* PRIVATE FUNCTION cn81_InfoView
 * ------------------------------------------------------------------
 */
tcn00_Error cn81_InfoView
    ( const tsp00_DbNamec &szDBName,
      int                  iView,
      char               * replyData,
      int                * replyLen,
      int                  replyLenMax,
      bool                 bDesc,
      const char         * pWhereOpt )
{
  tcn00_Error	        nFuncReturn = OK_CN00;
  char                szCommand[255];

  cn50DBMUser     oUser(szDBName, cn50DBMUser::getColdUser(szDBName));
  DBMSrvMsg_Error oSessionError;
  DBMSrvKnl_Session * pSession = DBMSrvKnl_GlobalSessions::CreateSession(DBMSrvKnl_GlobalSessions::SessionInfo,
                                                      szDBName.asCharp(),
                                                      oUser.getUserName().asCharp(),
                                                      oUser.getClearMasterPwd().asCharp(),
                                                      oSessionError);
  if (pSession == NULL) {
    nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oSessionError);
  } // end if

  if (nFuncReturn == OK_CN00) {

    SAPDB_strcpy(szCommand, "SELECT * FROM ");
    SAPDB_strcat(szCommand, InfoNamesC[iView].szView);
    if ((pWhereOpt != NULL) && (strlen(pWhereOpt) < 200 )) {
      SAPDB_strcat(szCommand, " WHERE ");
      SAPDB_strcat(szCommand, pWhereOpt);
    } // end if

    DBMSrvKnl_Session::ResultType oResultType = DBMSrvKnl_Session::resultUnknown;

    if (!pSession->Execute(szCommand, oResultType, oSessionError)) {
      if ((oSessionError == DBMSrvMsg_Error::DBMSrv_SQL) &&  
          (oSessionError.getTagValueAsInteger(SDBMSGTAG_DBMSRV_SQL__RETURNCODE) == 100)) {
        cn90AnswerOK(replyData, replyLen, NULL);
        DBMSrvKnl_GlobalSessions::ReleaseSession(DBMSrvKnl_GlobalSessions::SessionInfo, oSessionError);
      } else {
        nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oSessionError);
      } // end if
    } else {
      switch (oResultType) {
        case DBMSrvKnl_Session::resultSQL:
          if (InfoNamesC[iView].bOneRowResult) {
            if (cn81_InfoFetchNice(*pSession, bDesc, replyData, replyLenMax, oSessionError) != OK_CN00) {
              nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oSessionError);
            } // end if
          } else {
            if (cn81_InfoFetch(*pSession, bDesc, replyData, replyLenMax, oSessionError) != OK_CN00) {
              nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oSessionError);
            } // end if
          } // end if
          break;
        default:
          nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax)
                        .startWithMessageList(DBMSrvMsg_SQLError(-1, "unknown kernel answer"));
          break;
      } // end switch
    } // end if

  } // end if

  if (nFuncReturn != OK_CN00) {
    DBMSrvKnl_GlobalSessions::ReleaseSession(DBMSrvKnl_GlobalSessions::SessionInfo, oSessionError);
  } // end if

  *replyLen = (int)strlen(replyData);

  return nFuncReturn;
} /* cn81_InfoView */

/* SPECIFICATION PUBLIC FUNCTIONS
 * ------------------------------------------------------------------
 */

/* PUBLIC FUNCTION cn81Info
 * ------------------------------------------------------------------
 * purpose: show info view
 *
 */
tcn00_Error cn81Info(
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  tcn00_Error	         nFuncReturn = OK_CN00;
  char                 szName[255];
  char                 szParam[16];
  char               * pWhereOpt = NULL;
  int                  nParam = 1;
  int                  iView;
  bool                 bDesc = false;

  // get view name
  cn90GetToken(command->args, szName, nParam, 255);

  // get adtional options
  ++nParam;
  while (cn90GetToken(command->args, szParam, nParam, 16) && (nFuncReturn == OK_CN00))  {
    if (strcmp(szParam, INFO_DESC_OPT) == 0) {
      bDesc = true;
    } else  if (strcmp(szParam, INFO_WHERE_OPT) == 0) {
      pWhereOpt = (char *)strstr(command->args, INFO_WHERE_OPT) + strlen(INFO_WHERE_OPT) + 1;
      cn81_RemoveDoublequotes(pWhereOpt);
      break;;
    } else {
      nFuncReturn = cn90AnswerIError(replyData, replyLen,  ERR_PARAM_CN00);
    } // end if
    ++nParam;
  } // end while

  // check view name
  if (nFuncReturn == OK_CN00) {
    cn90Strip(szName);
    cn90StrUpperCopy(szName, szName, false);
    iView = 0;
    while (InfoNamesC[iView].szName != NULL)    {
      if (strcmp(szName, InfoNamesC[iView].szName) == 0) {
        break;
      } // end if
      ++iView;
    } // end while
    if (InfoNamesC[iView].szName == NULL) {
      nFuncReturn = cn90AnswerIError(replyData, replyLen,  ERR_PARAM_CN00);
    } // end if
  } // end if

  if (nFuncReturn == OK_CN00) {
    nFuncReturn = cn81_InfoView(vcontrol->dbname, iView, replyData, replyLen, replyLenMax, bDesc, pWhereOpt);
  } // end if

  return nFuncReturn;
} /* cn81Info*/

/* PUBLIC FUNCTION cn81InfoNext
 * ------------------------------------------------------------------
 * purpose: show next part from info view
 *
 */
tcn00_Error cn81InfoNext(
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax)
{
  tcn00_Error	 nFuncReturn = OK_CN00;
  DBMSrvKnl_Session * pSession = DBMSrvKnl_GlobalSessions::GetSession(DBMSrvKnl_GlobalSessions::SessionInfo);
  DBMSrvMsg_Error      oSessionError;

  if (pSession != NULL) {
    if (cn81_InfoFetch(*pSession, false, replyData, replyLenMax, oSessionError) != OK_CN00) {
      nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oSessionError);
    } // end if
  } else {
    cn90AnswerOK (replyData, replyLen, NULL);
  } // end if

  if (nFuncReturn != OK_CN00) {
    DBMSrvKnl_GlobalSessions::ReleaseSession(DBMSrvKnl_GlobalSessions::SessionInfo, oSessionError);
  } // end if

  *replyLen = (int)strlen(replyData);

  return nFuncReturn;
} /* cn81Info*/
