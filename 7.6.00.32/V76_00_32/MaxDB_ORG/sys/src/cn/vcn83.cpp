/*! 
  -----------------------------------------------------------------------------
 
  module: vcn83.cpp
 
  -----------------------------------------------------------------------------
 
  responsible:  BerndV
 
  special area: DBMServer
 
  description:  implementation module for special sql access
                - index handling
                - update statistics
  
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

/* 
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_Session.hpp"

#include "hcn50.h"
#include "hcn90.h"

#include "hcn83.h"

/* 
  -----------------------------------------------------------------------------
  specification private types and macros
  -----------------------------------------------------------------------------
*/

#define SQL_COMMANDLENGTH      1024

#define SQL_BADINDEXBYNAME     "SELECT * FROM SYSDD.BAD_INDEXES WHERE SCHEMANAME='%s' AND TABLENAME='%s' AND INDEXNAME='%s'"

#define SQL_INDEXBYNAME        "SELECT * FROM SYSDD.INDEXES WHERE SCHEMANAME='%s' AND TABLENAME='%s' AND INDEXNAME='%s' ORDER BY COLUMNNO ASC"
#define SQL_INDEXCOUNT         "SELECT COUNT(*) FROM SYSDD.INDEXES WHERE SCHEMANAME='%s' AND TABLENAME='%s' AND INDEXNAME='%s'"

#define SQL_DROPINDEXBYNAME    "DROP INDEX \"%s\" ON \"%s\".\"%s\""

#define SQL_CREATEINDEXBYNAME1 "CREATE %s INDEX \"%s\" ON \"%s\".\"%s\" (%s) "
#define SQL_CREATEINDEXBYNAME2 "\"%s\" %s,"

#define SQL_RESTART            "CREATE INDEX RESTART"

#define SQLF_TYPE              "TYPE"
#define SQLF_SORT              "SORT"
#define SQLF_COLUMNNAME        "COLUMNNAME"

/* 
  -----------------------------------------------------------------------------
  private function:     cn83_RecreateIndex
  -----------------------------------------------------------------------------
 */
static tcn00_Error cn83_RecreateIndex
      ( const tsp00_DbNamec & pDBName,
        const char          * pScheme,
        const char          * pTable,
        const char          * pIndex,
        char                * replyData,
        int                 * replyLen,
        int                   replyLenMax)
{
  tcn00_Error             nFuncReturn    = OK_CN00;

  char                    szCommand     [SQL_COMMANDLENGTH];
  char                    szDropCommand [SQL_COMMANDLENGTH];
  char                  * pCreateCommand = NULL;
  char                  * pColumns       = NULL;
  char                  * pCurrent       = NULL;
  char                    szType  [10]   = "";
  char                    szSort  [10]   = "";
  tsp00_KnlIdentifierc    szColumn;
  tsp00_Int4              nColumns = 0;

  DBMSrvKnl_Session       oSession;
  DBMSrvMsg_Error         oSessionError;
    
  // connect as DBM
  cn50DBMUser oDBMUser   (pDBName, cn50DBMUser::getColdUser  (pDBName));
  if (!oDBMUser.lastEvent().IsEmpty()) {
    Msg_List oDBM = oDBMUser.lastEvent();
    oDBM.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_DBAUNKNOWN));
    nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oDBM);
  } else {
    if (!oSession.NormalConnect(pDBName.asCharp(),                                     
                                oDBMUser.getUserName().asCharp(),
                                oDBMUser.getClearMasterPwd().asCharp(),
                                oSessionError)) {
      nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oSessionError);
    } // end if
  } // end if

  // check sysdd.bad_indexes for existence and named property
  if (nFuncReturn == OK_CN00) {
    sprintf(szCommand, SQL_BADINDEXBYNAME, pScheme, pTable, pIndex);
    if (!oSession.Execute(szCommand, oSessionError)) {
      nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oSessionError);
    } // end if
  } // end if

  // generate drop command
  if (nFuncReturn == OK_CN00) {
    sprintf(szDropCommand, SQL_DROPINDEXBYNAME, pIndex, pScheme, pTable);

    // generate create command
    // check field count
    sprintf(szCommand, SQL_INDEXCOUNT, pScheme, pTable, pIndex);
    if (!oSession.Execute(szCommand, oSessionError)) {
      nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oSessionError);
    } else {
      DBMSrvKnl_SQLRecord & oRecord = oSession.GetRecordObject();
      nColumns = oRecord.fieldAsInt(1);
    } // end if
  } // end if

  // check sysdd.indexes
  if (nFuncReturn == OK_CN00) {
    sprintf(szCommand, SQL_INDEXBYNAME, pScheme, pTable, pIndex);
    if (!oSession.Execute(szCommand, oSessionError)) {
      nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oSessionError);
    } // end if
  } // end if

  if (nFuncReturn == OK_CN00) {
    DBMSrvKnl_SQLRecord & oRecord = oSession.GetRecordObject();
    bool             bEnd = false;

    // save type
    SAPDB_strcpy(szType, oRecord.fieldAsChar(SQLF_TYPE).CharPtr());

    // allocate memory for fieldlist
    pColumns  = new char [nColumns * (sizeof(tsp00_KnlIdentifier) + strlen(SQL_CREATEINDEXBYNAME2)) + 1];
    if (pColumns == NULL) {
      nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, DBMSrvMsg_Error(SDBMSG_DBMSRV_MEMORY));
    } else {
      *pColumns = CHAR_STRINGTERM_CN90;
      pCurrent  = pColumns;

      // create fieldlist
      while (!bEnd && oSessionError.IsEmpty()) {
        SAPDB_strcpy(szSort,   oRecord.fieldAsChar(SQLF_SORT).CharPtr());
        SAPDB_strcpy(szColumn, oRecord.fieldAsChar(SQLF_COLUMNNAME).CharPtr());
        sprintf(pCurrent, SQL_CREATEINDEXBYNAME2, szColumn.asCharp(), szSort);
        pCurrent = pCurrent + strlen(pCurrent);
    
        if (!oRecord.getNextRecord(bEnd, oSessionError)) {
          break;
        } // end if
      } // end while

      if (!oSessionError.IsEmpty()) {
        nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oSessionError);
      } else {
        --pCurrent;
        *pCurrent = CHAR_STRINGTERM_CN90;
        pCreateCommand = new char [SQL_COMMANDLENGTH + strlen(pColumns)];
        if (pCreateCommand != NULL) {
          sprintf(pCreateCommand, SQL_CREATEINDEXBYNAME1, szType, pIndex, pScheme, pTable, pColumns);
        } else {
          nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, DBMSrvMsg_Error(SDBMSG_DBMSRV_MEMORY));
        } // end if
      } // end if
    } // end if
  } // end if

  // drop index
  if (nFuncReturn == OK_CN00) {
    if (!oSession.Execute(szDropCommand, oSessionError)) {
      nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oSessionError);
    } // end if
  } // end if

  // drop index
  if (nFuncReturn == OK_CN00) {
    if (!oSession.Execute(pCreateCommand, oSessionError)) {
      nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oSessionError);
    } // end if
  } // end if

  if (nFuncReturn == OK_CN00) {
    cn90AnswerOK(replyData, replyLen, NULL);
  } // end if

  if (pColumns       != NULL) delete [] pColumns;
  if (pCreateCommand != NULL) delete [] pCreateCommand;

  return nFuncReturn;
} // end cn83_RecreateIndex

/* 
  -----------------------------------------------------------------------------
  private function:     cn83_RestartIndex
  -----------------------------------------------------------------------------
 */
static tcn00_Error cn83_RestartIndex
      ( const tsp00_DbNamec & pDBName,
        char                * replyData,
        int                 * replyLen,
        int                   replyLenMax )
{
  tcn00_Error             nFuncReturn    = OK_CN00;

  cn50DBMUser oDBAUser   (pDBName, cn50DBMUser::getSYSDBA  (pDBName));
  if (!oDBAUser.lastEvent().IsEmpty()) {
    Msg_List oDBM = oDBAUser.lastEvent();
    oDBM.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_DBAUNKNOWN));
    nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oDBM);
  } else {
    DBMSrvMsg_Error oError;
    if (!DBMSrvKnl_Session::Execute (pDBName.asCharp(),
                                     oDBAUser.getUserName().asCharp(),
                                     oDBAUser.getClearMasterPwd().asCharp(),
                                     SQL_RESTART,
                                     oError)) {
      nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oError);
    } // end if
  } // end if

  if (nFuncReturn == OK_CN00) {
    cn90AnswerOK(replyData, replyLen, NULL);
  } // end if

  return nFuncReturn;
} // end cn83_RestartIndex

/* 
  -----------------------------------------------------------------------------
  function:     cn83RecreateIndex
  -----------------------------------------------------------------------------
 */
tcn00_Error cn83RecreateIndex
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error         nFuncReturn   = OK_CN00;

  char               szIndexSpec[PARAMETER_MAXLEN_CN90];
  char             * pScheme       = NULL;
  char             * pTable        = NULL;
  char             * pIndex        = NULL;

  // analyze parameters
  if (cn90GetToken(command->args, szIndexSpec, 1, PARAMETER_MAXLEN_CN90)) {
    pScheme = &szIndexSpec[0];
    pTable  = strstr(szIndexSpec, ".");
    if (pTable == NULL) {
      nFuncReturn =  cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00);
    } else {
      *pTable = CHAR_STRINGTERM_CN90;
      ++pTable;
      pIndex  = strstr(pTable, ".");
      if (pIndex == NULL) {
        nFuncReturn =  cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00);
      } else {
        *pIndex = CHAR_STRINGTERM_CN90;
        ++pIndex;
      } // end if
    } // end if

    if (nFuncReturn == OK_CN00) {
      cn90StripLeadingChar(cn90StripTrailingChar(pScheme, '"'), '"');
      cn90StripLeadingChar(cn90StripTrailingChar(pTable,  '"'), '"');
      cn90StripLeadingChar(cn90StripTrailingChar(pIndex,  '"'), '"');

      nFuncReturn = cn83_RecreateIndex (vcontrol->dbname, pScheme, pTable, pIndex, replyData, replyLen, replyLenMax);

    } // end if

  } else {
    nFuncReturn = cn83_RestartIndex (vcontrol->dbname, replyData, replyLen, replyLenMax);
  } // end if

  return nFuncReturn;
} // end cn83RecreateIndex

