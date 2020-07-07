/* 
  -----------------------------------------------------------------------------
  module: vls13.cpp
  -----------------------------------------------------------------------------
 
  responsible:  SteffenS and d033893

  special area: SAP DB LOADER
 
  description:  Implementation of support functions
                
  version:      7.6.
  -----------------------------------------------------------------------------

    ========== licence begin  GPL
    Copyright (c) 1997-2005 SAP AG

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
#include <ctype.h>
#include <assert.h>



#include "gls00.h"

#include "hls03.h"
#include "hls04.h"
#include "hls041.h"
#include "hls05.h"
#include "hls12.h"

#include "hls13.h"

#include "hls31_odbc_Result.h"      // MetaDataDef->Statement()
#undef sp1s_segm_len                // necessary because vpr00c redefines this function !!! and is included by various odbc files

#include "hls98msg.h"
#include "hls99.h"



#define _INTERNAL_FILE_ID_    _T("13")


/*  
  -----------------------------------------------------------------------------
  Chapter: Private functions declaration
  -----------------------------------------------------------------------------
*/
/*
  -----------------------------------------------------------------------------
  function:     ls13_GetRawDescription
  -----------------------------------------------------------------------------

  description:  Executes the cmd and returns a pointer to the data in receive
                packet.

  arguments:    DBInfo           [IN]  - structure with all info on the running
                                          session and the database connected to
                lPacketNo        [IN]  - packet number to use for request/receive
                pszCmd           [IN]  - command to execute (mainly DESCRIBE ...);
                                         must be UTF8 encoded
                lDescriptionLen  [OUT] - length of data pointed to
                pszDescription   [OUT] - pointer to data in receive packet
                pszErrtext       [OUT] - error text

  returnvalue:  errOK_els00
                errInternSQL_els98  - error executing statement: simple sql error message
                                      will be returned in pszErrtext in the following form
                                      "SQL-Error: <error number> = <error text>
                                        (error position: <position>)"
                errInternal_els98   - special error when retrieving the data from
                                      request packet (highly unlikely); no error
                                      text returned

                Function does not log error text to the log file.
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls13_GetRawDescription(tls00_DBInfo *DBInfo,
                       tsp00_Int4    lPacketNo,
                       tsp00_Addr    pszCmd,
                       tsp00_Int4   &lDescriptionLen,
                       tsp00_Addr   &pszDescription,
                       tsp00_Addr    pszErrText);

/*
  -----------------------------------------------------------------------------
  function:     ls13_PutCmd
  -----------------------------------------------------------------------------

  description:  Initializes a message packet and puts the command (first part)
                in. The command must be UTF8 encoded.
                Converts command to UNICODE if db is unicode.

  arguments:    DBInfo       [IN]  - structure with all info on the running
                                      session and the database connected to
                lPcktNo      [IN]  - packet number to use for init
                pszSqlStmt   [IN]  - command to put into packet (UTF8 encoded)
                pszErrText   [OUT] - error text
                CmdProducer  [IN]  - type of producer (defaults to sp1pr_internal_cmd)
                bWithInfo    [IN]  - with info flag (defaults to false)
                MessType     [IN]  - message type (defaults to sp1m_dbs)
                bMassCmd     [IN]  - mass command flag (defaults to false)

  returnvalue:  errOK_els00                  - no error
                errConvertingEncoding_els98  - error converting stmt text from
                                                UTF8 to UCS2
                errFinishPart_els98          - error finishing command part
                Logs the error to log file and to return string.
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls13_PutCmd(tls00_DBInfo            *pDBInfo,
            tsp00_Int4               lPcktNo,
            const char*              pszSqlStmt,
            tsp00_Addr               pszErrText,
            tsp1_producer_Enum       CmdProducer = sp1pr_internal_cmd,
            bool                     bWithInfo = false,
            tsp1_cmd_mess_type_Enum  MessType = sp1m_dbs,
            bool                     bMassCmd = false);

/*
  -----------------------------------------------------------------------------
  function:     ls13_GetCorrectErrorPos
  -----------------------------------------------------------------------------

  description:  Calculates accurate error position (line and col numbers) for
                failed [sql] commands.

  arguments:    pSqlCmd   [IN]  - command to parse
                lErrPos   [IN]  - reported error pos (by kernel upon execution)
                lRow      [OUT] - line number
                lCol      [OUT] - column number

  returnvalue:  None
  -----------------------------------------------------------------------------
*/
static void
ls13_GetCorrectErrorPos(const char*   pszSqlCmd,
                        tsp00_Int4    lErrPos,
                        tsp00_Int4   &lRow,
                        tsp00_Int4   &lCol);

/*  
  -----------------------------------------------------------------------------
  EndChapter: Private functions declaration
  -----------------------------------------------------------------------------
*/

/*  
  -----------------------------------------------------------------------------
  Chapter: Public functions implementation
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  function:     ls13ExecExtSQLStmt
  -----------------------------------------------------------------------------
*/
int ls13ExecExtSQLStmt(tls00_DBInfo *DBInfo, tls00_DBSqlCmd *SqlCmd, tsp00_Addr ErrText)
{
    ROUTINE_DBG_MEO00 ("ls13ExecExtSQLStmt");

    TRACE_PRNF_MLS99(("ls13ExecExtSQLStmt", "Executing external SQL statement '%s'",
                                            SqlCmd->dscSqlStmt_ls00));

    LoaderMessages *MsgInstance = LoaderMessages::Instance(); // error reporting obj

    tsp00_Int4 rc = errOK_els00;
    
    tsp00_Addr pszSqlStmt = SqlCmd->dscSqlStmt_ls00;
    size_t lStmtLen       = _tcslen(pszSqlStmt);
    
    //*
    //*     Initialize/fill packet for statement
    //*
    //* message type = sp1m_dbs (= default);  with info = false (= default);
    //* Mass command = false    (= default)
    //*
    //* sp1pr_user_cmd is the fundamental difference to executing internal commands
    rc = ls13_PutCmd(DBInfo, 0, pszSqlStmt, ErrText, sp1pr_user_cmd);
    if (errOK_els00 == rc)
    {
        //*
        //*     send SqlPacket, receive answer packet and analyse latter
        //*
        tsp00_Int4  lErrorPos = 0;
        _TCHAR     szErr[MAX_REPLY_LENGTH_LS00];
        szErr[0] = 0;

        rc = ls03ProcessSQLPacket(DBInfo, 0, lErrorPos, szErr);
        if (errOK_els00 != rc)
        {
            if (1 == lErrorPos)
            {
                MsgInstance->ls98Msg(laySQLInterface_els98, ErrText, errSimpleSQL_els98, szErr);
            }
            else
            {
                tsp00_Int4 lRow = 0;
                tsp00_Int4 lCol = 0;
                ls13_GetCorrectErrorPos(SqlCmd->dscSqlStmt_ls00, lErrorPos, lRow, lCol);
                // this comes always in form "error text ... (error position: <pos>)\n"
                // add some more accurate info about error location
                char*      _pbracket = strrchr(szErr, ')');
                tsp00_Int4 _len      = _pbracket - szErr;
                sp77sprintf(szErr + _len, MAX_REPLY_LENGTH_LS00 - _len,
                                           "[line: %d, col: %d])\n",
                                           lRow, lCol);
                MsgInstance->ls98Msg(laySQLInterface_els98, ErrText, errSimpleSQL_els98, szErr);
            }
        }
    }

    pszSqlStmt = NULL;

    return rc;
}
// ls13ExecExtSQLStmt()

/*
     -----------------------------------------------------------------------------
  function:     ls13ExecExtSQLStmt
  -----------------------------------------------------------------------------
*/
// Incoming sql command has to be UTF8 encoded
int ls13ExecExtSQLStmt(tls00_DBInfo *pDBInfo, const char* pszSqlCmd, tsp00_Addr ErrText)
{
    ROUTINE_DBG_MEO00 ("ls13ExecExtSQLStmt");

    TRACE_PRNF_MLS99(("ls13ExecExtSQLStmt", "Executing external SQL statement '%s'", pszSqlCmd));

    LoaderMessages *MsgInstance = LoaderMessages::Instance(); // error reporting obj

    tsp00_Int4 rc = errOK_els00;
    
    //*
    //*     Initialize/fill packet for statement
    //*
    //* message type = sp1m_dbs (= default);  with info = false (= default);
    //* Mass command = false    (= default)
    //*
    //* sp1pr_user_cmd is the fundamental difference to executing internal commands
    rc = ls13_PutCmd(pDBInfo, 0, pszSqlCmd, ErrText, sp1pr_user_cmd);
    if (errOK_els00 == rc)
    {
        //*
        //*     send SqlPacket, receive answer packet and analyse latter
        //*
        tsp00_Int4  lErrorPos = 0;
        _TCHAR     szErr[MAX_REPLY_LENGTH_LS00];
        szErr[0] = 0;

        rc = ls03ProcessSQLPacket(pDBInfo, 0, lErrorPos, szErr);
        if (errOK_els00 != rc)
        {
            if (1 == lErrorPos)
            {
                MsgInstance->ls98Msg(laySQLInterface_els98, ErrText, errSimpleSQL_els98, szErr);
            }
            else
            {
                tsp00_Int4 lRow = 0;
                tsp00_Int4 lCol = 0;
                ls13_GetCorrectErrorPos(pszSqlCmd, lErrorPos, lRow, lCol);
                // this comes always in form "error text ... (error position: <pos>)\n"
                // add some more accurate info about error location
                char*      _pbracket = strrchr(szErr, ')');
                tsp00_Int4 _len      = _pbracket - szErr;
                sp77sprintf(szErr + _len, MAX_REPLY_LENGTH_LS00 - _len,
                                           "[line: %d, col: %d])\n",
                                           lRow, lCol);
                MsgInstance->ls98Msg(laySQLInterface_els98, ErrText, errSimpleSQL_els98, szErr);
            }
        }
    }

    return rc;
}
// ls13ExecExtSQLStmt()


/*
  -----------------------------------------------------------------------------
  function:     ls13PingDB
  -----------------------------------------------------------------------------
*/
SAPDB_Int4
ls13PingDB(tls00_DBInfo* pDBInfo, SAPDB_Char* pszErrText)
{
    tsp1_packet*  pPacket = pDBInfo->dbiPktSndList_ls00[0];
    tsp1_segment* pSegm   = NULL;
    tsp1_part*    pPart   = NULL;
    SAPDB_Int4    lErrPos = 0;
    
    ls04FastInit(pPacket, &pSegm, &pPart, pDBInfo, sp1pr_internal_cmd);

    // This kind of packet is the only one that does not need a part at all
    // (for faster processing). Thus we initialize 'too much' with ls04FastInit
    // and have to adjust the packet to "no part inside":
    pSegm->sp1s_segm_len()               -= sizeof(tsp1_part_header);
    pSegm->sp1s_no_of_parts()             = 0;
    pPacket->sp1_header.sp1h_varpart_len -= sizeof(tsp1_part_header);

    // Set the message type HELLO which corresponds to ping
    ls04SetMsgType(pSegm, sp1m_hello);

    return ls03ProcessSQLPacket(pDBInfo, 0, lErrPos, pszErrText);
}
// ls13PingDB()


/*
  -----------------------------------------------------------------------------
  function:     ls13ExecIntSQLStmt      
  -----------------------------------------------------------------------------
*/
int
ls13ExecIntSQLStmt(tls00_DBInfo            *DBInfo,
                   tsp00_Int4               lPcktNo,
                   tsp00_Addr               pszSqlStmt,
                   tsp00_Addr               pszErrText,
                   tsp00_Int4              &lErrPos,
                   bool                     bWithInfo,  // = false
                   tsp1_cmd_mess_type_Enum  MessType,   // = sp1m_dbs
                   bool                     bMassCmd)   // = false
{
    ROUTINE_DBG_MEO00 ("ls13ExecIntSQLStmt");

    tsp00_Int4 rc = errOK_els00;

    rc = ls13ExecIntSQLStmtEx(DBInfo, lPcktNo, pszSqlStmt, NULL, 0, pszErrText, lErrPos,
                              sp1pk_command, bWithInfo, MessType, bMassCmd);
    return rc;
}
// ls13ExecIntSQLStmt()


/*
  -----------------------------------------------------------------------------
  function:     ls13ExecIntSQLStmtEx
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls13ExecIntSQLStmtEx(tls00_DBInfo            *DBInfo,
                     tsp00_Int4               lPcktNo,
                     tsp00_Addr               pszSqlStmt,
                     tsp00_Addr               pszData,    // always of type char*
                     tsp00_Int4               lDataLength,
                     tsp00_Addr               pszErrText,
                     tsp00_Int4              &lErrPos,
                     tsp1_part_kind_Enum      PartKind,
                     bool                     bWithInfo,  // = false
                     tsp1_cmd_mess_type_Enum  MessType,   // = sp1m_dbs
                     bool                     bMassCmd)   // = false
{
    ROUTINE_DBG_MEO00 ("ls13ExecIntSQLStmtEx");

    TRACE_PRNF_MLS99(("ls13ExecIntSQLStmt", "Executing internal SQL statement '%s'", pszSqlStmt));

    LoaderMessages *MsgInstance = LoaderMessages::Instance(); // error reporting obj

    tsp00_Int4 rc = errOK_els00;
    
    rc = ls13_PutCmd(DBInfo, lPcktNo, pszSqlStmt, pszErrText, sp1pr_internal_cmd, bWithInfo, MessType, bMassCmd);
    if (errOK_els00 == rc)
    {
        //*
        //*     2.part - data part
        //*
        //* In case lDataLength = 0 there is no 2nd part to fill
        if (0 != lDataLength)
        {
            tsp1_packet  *SqlPacket = DBInfo->dbiPktSndList_ls00[lPcktNo];
            tsp1_segment *SqlSegm   = ls04GetSegment(SqlPacket);
            tsp1_part    *SqlPart   = NULL;

            rc = ls04InitPart(SqlPacket, SqlSegm, &SqlPart, PartKind);
            if (errOK_els00 == rc)
            {
                ls04BufToPart(SqlPart, pszData, lDataLength);
    
                rc = ls04FinishPart(SqlPacket, SqlPart);
            }
        }

        //*
        //*     Send und receive sql packet
        //*
        if (errOK_els00 != rc)
        {
            MsgInstance->ls98Msg(laySQLInterface_els98, pszErrText, STATIC_CAST(ErrorNumbers,rc));
        }
        else
        {
            rc = ls03ProcessSQLPacket(DBInfo, lPcktNo, lErrPos, pszErrText);
        }
    }

    return rc;
}
// ls13ExecIntSQLStmtEx()


/*
  -----------------------------------------------------------------------------
  function:     ls13ExecIntSQLMultParts
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls13ExecIntSQLMultParts(tls00_DBInfo            *DBInfo,
                        tsp00_Int4               lPcktNo,
                        tls00_Parts4InternalSql &PartStruct,
                        tsp00_Addr               pszErrText,
                        tsp00_Int4              &lErrPos,
                        bool                     bWithInfo,  // = false
                        tsp1_cmd_mess_type_Enum  MessType,   // = sp1m_dbs
                        bool                     bMassCmd)   // = false
{
    ROUTINE_DBG_MEO00 ("ls13ExecIntSQLMultParts");

    TRACE_PRNF_MLS99(("ls13ExecIntSQLStmt", "Executing internal SQL statement"));

    LoaderMessages *MsgInstance = LoaderMessages::Instance(); // error reporting obj

    tsp00_Int4 rc = errOK_els00;

    // Check if first data is really a statement
    assert(sp1pk_command == PartStruct.pfiPartKind_ls00[0]);

    //*
    //*     Fill parts - first one MUST be a statement
    //*
    rc = ls13_PutCmd(DBInfo,
                     lPcktNo,
                     PartStruct.pfiData_ls00[0].strAddr_ls00,
                     pszErrText,
                     sp1pr_internal_cmd,
                     bWithInfo,
                     MessType,
                     bMassCmd);
    if (errOK_els00 == rc)
    {
        //*
        //*     Fill remaining parts
        //*
        if (PartStruct.pfiCount_ls00 > 1)
        {
            tsp1_packet  *SqlPacket = DBInfo->dbiPktSndList_ls00[lPcktNo];
            tsp1_segment *SqlSegm   = ls04GetSegment(SqlPacket);
            tsp1_part    *SqlPart   = NULL;

            int i = 1;
            for (i; i < PartStruct.pfiCount_ls00; ++i)
            {
                SqlPart = NULL;             // reinitialize

                rc = ls04InitPart(SqlPacket, SqlSegm, &SqlPart, PartStruct.pfiPartKind_ls00[i]);
                if (errOK_els00 != rc)
                {
                    break;
                }
                ls04BufToPart(SqlPart, PartStruct.pfiData_ls00[i].strAddr_ls00, PartStruct.pfiData_ls00[i].strLeng_ls00);

                rc = ls04FinishPart(SqlPacket, SqlPart);
                if (errOK_els00 != rc)
                {
                    break;
                }
            }
            
            if (errOK_els00 != rc)
            {
                MsgInstance->ls98Msg(laySQLInterface_els98, pszErrText, STATIC_CAST(ErrorNumbers, rc));
            }
        }

        //*
        //*     Send und receive sql packet
        //*
        rc = ls03ProcessSQLPacket(DBInfo, lPcktNo, lErrPos, pszErrText);
    }

    return rc;
}
// ls13ExecIntSQLMultParts()


/*
  -----------------------------------------------------------------------------
  function:     ls13Parse
  -----------------------------------------------------------------------------
*/
tsp00_Longint
ls13Parse(tsp00_Addr              pszCmd,
          tls00_DBInfo           *pDBInfo,
          char                   *pszParseID,
          tls00_TableDescription *pTableDesc,
          tsp00_Addr              pszErrText,
          bool                    bWithInfo,
          bool                    bMassCmd)
{
    ROUTINE_DBG_MEO00 ("ls13GetTableDescription");

    LoaderMessages *pMsgObj = LoaderMessages::Instance(); // error reporting obj
    tsp00_Longint   rc      = errOK_els00;

    char       szErr[MAX_REPLY_LENGTH_LS00];
    tsp00_Int4 ErrPos = 0;
    szErr[0]          = 0;
    rc = ls13ExecIntSQLStmt(pDBInfo, 0, pszCmd, szErr, ErrPos, bWithInfo, sp1m_parse, bMassCmd);
    if (errOK_els00 != rc)
    {
        pMsgObj->ls98Msg(laySQLInterface_els98, pszErrText, errSimpleSQL_els98, szErr);
        rc = errSimpleSQL_els98;
    }
    else
    {
        //*
        //*     Analyze answer packet
        //* Get parsid, shortinfo and parameter description
        //*
        tsp1_packet_ptr pSqlPacket = pDBInfo->dbiPktRcvList_ls00[0];
    
        rc = ls04GetParseID(pSqlPacket, pszParseID);
        if (errOK_els00 == rc)
        {
            rc = ls04GetShortInfo(pSqlPacket, csp_max_fields, pTableDesc->tdParamArray_ls00, pTableDesc->tdFieldCount_ls00);
        }
        if (errOK_els00 != rc)
        {
            pMsgObj->ls98Msg(laySQLInterface_els98, pszErrText, STATIC_CAST(ErrorNumbers, rc));
        }
        else
        {
            TRACE_TABPARAM_MLS99("ls13Parse", pTableDesc);
        }
    }

    return rc;
}
// ls13Parse()


/*
  -----------------------------------------------------------------------------
  function:     ls13GetTableDescription
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls13GetTableDescription(tls00_DBInfo            *pDBInfo,
                        tsp00_Int4               lPacketNo,
                        Tools_DynamicUTF8String  TableName,
                        tsp00_Int4              &lTabDescLen,
                        tsp00_Addr              &pszTabDesc,
                        tsp00_Addr               pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls13GetTableDescription");

    LoaderMessages *pMsgObj = LoaderMessages::Instance(); // error reporting obj
    tsp00_Int4       rc      = errOK_els00;

    char   szErr[MAX_REPLY_LENGTH_LS00];
    
    _TCHAR szCmd[256];
    szCmd[0] = '\0';
    sp77sprintf(szCmd, 256, DESCRIBE_TABLE_STMT_LS13, TableName.StrPtr());

    rc = ls13_GetRawDescription(pDBInfo, lPacketNo, szCmd, lTabDescLen, pszTabDesc, szErr);
    if (errOK_els00 != rc)
    {
        if (errInternal_els98 == rc)
        {
            pMsgObj->ls98Msg(layTableExtract_els98, pszErrText, errInternal_els98, __FILE__, __LINE__, rc);
        }
        else
        {
            pMsgObj->ls98Msg(layTableExtract_els98, pszErrText, errGetTableDefinition_els98, TableName.StrPtr(), szErr);
            rc = errGetTableDefinition_els98;
        }
    }
    return rc;
}
// ls13GetTableDescription()

/*
  -----------------------------------------------------------------------------
  function:     ls13GetIndexDescription
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls13GetIndexDescription(tls00_DBInfo            *pDBInfo,
                        tsp00_Int4               lPacketNo,
                        Tools_DynamicUTF8String  TableName,
                        tsp00_Int4              &lIndexDescLen,
                        tsp00_Addr              &pszIndexDesc,
                        tsp00_Addr               pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls13GetIndexDescription");

    LoaderMessages *pMsgObj = LoaderMessages::Instance(); // error reporting obj
    tsp00_Int4       rc      = errOK_els00;

    char   szErr[MAX_REPLY_LENGTH_LS00];

    _TCHAR szCmd[256];      // sizeof(tsp00_KnlIdentifierc) + sizeof(tsp00_Namec) + rest --> should be far enough
    szCmd[0] = '\0';
    sp77sprintf(szCmd, 256, DESCRIBE_INDEX_STMT_LS13, TableName.StrPtr());

    rc = ls13_GetRawDescription(pDBInfo, lPacketNo, szCmd, lIndexDescLen, pszIndexDesc, szErr);
    if (errOK_els00 != rc)
    {
        if (errInternal_els98 == rc)
        {
            pMsgObj->ls98Msg(layTableExtract_els98, pszErrText, errInternal_els98, __FILE__, __LINE__, rc);
        }
        else
        {
            pMsgObj->ls98Msg(layTableExtract_els98, pszErrText, errGetIndexDefinition_els98, TableName.StrPtr(), szErr);
            rc = errGetIndexDefinition_els98;
        }
    }
    return rc;
}
// ls13GetIndexDescription()


// Returns column names (in pTI) and count of column names (tdFieldCount in pTI)
SAPDB_Int4
ls13GetColumnNames(MetaDataDef*&            pMetaDataDef,
                   tsp00_Addr               pSchemaName,
                   tsp00_Addr               pTableName,
                   tls00_TableDescription*  pTI,
                   tsp00_Addr               pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls13GetColumnNames");
    
    tsp00_Int4      rc              = errOK_els00;
    bool            _bCursorOpened  = false;
    tls00_DBInfo    *pDBInfo        = pMetaDataDef->getConnection();
    SAPDB_Char      _szAuxErr[MAX_REPLY_LENGTH_LS00];
    _szAuxErr[0] = 0;

    //*
    //*     Build the command to select the column names from DOMAIN.COLUMNS
    //*
    // pszbuffer must store a cmd like
    // "SELECT COLUMNNAME FROM DOMAIN.COLUMNS WHERE SCHEMANAME = '<name>' AND TABLENAME = '<name>' ORDER BY POS"
    tsp00_KnlIdentifierc    _pLiteralTableName;
    StudioOAL_String        _stmt       = pMetaDataDef->Statement(sel_Relational_18);
    SAPDB_Char*             pszBuffer   = new char[256];
    pszBuffer[0] = '\0';

    ls05MakeDBIdentifierLiteral(pTableName, _tcslen(pTableName), _pLiteralTableName.asCharp());
    sprintf(pszBuffer, _stmt.CharPtr(), pSchemaName, _pLiteralTableName.asCharp());

    //*
    //*     Create result table name
    //*
    SAPDB_Char* _pszTmpResultTableName = new char[sizeof(tsp00_KnlIdentifierc)];
    ls12CreateResultTabName(_pszTmpResultTableName, sizeof(tsp00_KnlIdentifierc));

    //*
    //*     Declare named cursor
    //*
    rc = ls12DeclareCursor(*pDBInfo, _pszTmpResultTableName, pszBuffer, _szAuxErr); 
    if (errOK_els00 != rc)
    {
        if (dbRowNotFound_els00 == rc)
        {
            sprintf(_szAuxErr, "Unknown table name '%s.%s'", pSchemaName, _pLiteralTableName.asCharp());
            LoaderMessages::Instance()->ls98Msg(layTableLoad_els98, pszErrText, errSelectColumnNames_els98, _szAuxErr);
        }
        else
        {
            LoaderMessages::Instance()->ls98Msg(layTableLoad_els98, pszErrText, errSelectColumnNames_els98, _szAuxErr);
        }
        rc = errSelectColumnNames_els98;
    }
    else
    {
        //*
        //*     Fetch column names and assign to column name structure in pTI
        //*
        _bCursorOpened = true;

        SAPDB_Int4              lResultCount = 0;
        SAPDB_Int4              i            = 0;
        bool                    _bUnicode    = ls00UseUnicodeOI(pDBInfo);
        tls00_TableDescription  _pTmpTabDescription;
        memset(&_pTmpTabDescription, 0, sizeof(tls00_TableDescription));

        // Get count of column names and parameter description
        rc = ls04GetResultCount(pDBInfo->dbiPktRcvList_ls00[0], NULL, lResultCount);
        if (errOK_els00 == rc)
        {
            // Assign result count to fieldcount member
            pTI->tdFieldCount_ls00 = STATIC_CAST(SAPDB_Int2, lResultCount);

            // short info part
            rc = ls04GetShortInfo(pDBInfo->dbiPktRcvList_ls00[0], csp_max_fields,
                                  _pTmpTabDescription.tdParamArray_ls00,
                                  _pTmpTabDescription.tdFieldCount_ls00);
        

            // Allocate all column name pointers
            for (i = 0; i < lResultCount; ++i)
            {
                pTI->tdColName_ls00[i] = new tsp00_KnlIdentifierc;
            }

            // Attention: we have only ONE parameter --> tdParamArray_ls00[0] !!!
            tls00_FieldBasics*      _pBI            = &_pTmpTabDescription.tdParamArray_ls00[0]->piBasicInfo_ls00;
            SAPDB_Int4              _lColIdx        = 0;               // column name iterator index
            SAPDB_Int4              _lFetchCnt      = lResultCount;
            SAPDB_Int4              _lSumFetchCnt   = 0;
            SAPDB_Char*             _pPtrToData     = NULL;
            tsp00_KnlIdentifierc    _TmpId;
            SAPDB_Int4              _lNameLen       = _pBI->fbInOutLength_ls00 - sizeof(SAPDB_Char);      // decrease by size of defined Byte

            do
            {
                _lFetchCnt = lResultCount;            // Is reset by ls12FetchEx

                // Fetch column names
                SAPDB_Int4 _lAuxLen = 0;
                rc = ls12FetchEx(*pDBInfo, _pszTmpResultTableName, _lFetchCnt, true, _pPtrToData, _lAuxLen, _szAuxErr);
                if (errOK_els00 != rc)
                {
                    LoaderMessages::Instance()->ls98Msg(layTableLoad_els98, pszErrText, errInternSQL_els98, _szAuxErr);
                    rc = errInternSQL_els98;
                    break;
                }
                else
                {
                    // Assign fetched column names to pTI column name structure
                    for (SAPDB_Int4 j = 0; j < _lFetchCnt; ++j, ++_lColIdx)
                    {
                        SAPDB_memcpy(_TmpId.asCharp(), &_pPtrToData[j*_pBI->fbInOutLength_ls00] + sizeof(SAPDB_Char), _lNameLen);
                        ls05EscapeQuotesInDBIdentifier(_TmpId.asCharp(), _lNameLen, pTI->tdColName_ls00[_lColIdx]->asCharp(), _bUnicode, DOUBLE_QUOTE_LS00); ////SDC Migration _bUnicode=true
                    }   // end for (SAPDB_Int4 j = 0; j < _lFetchCnt; ++j, ++_lColIdx)

                    _lSumFetchCnt += _lFetchCnt;
                }
            } while (_lSumFetchCnt < lResultCount);

            // Free temporarily allocated (in ls04GetShortInfo) paraminfo
            for (i = 0; i < _pTmpTabDescription.tdFieldCount_ls00; ++i)
            {
                if (NULL != _pTmpTabDescription.tdParamArray_ls00[i])
                {
                    delete _pTmpTabDescription.tdParamArray_ls00[i];
                }
            }
        }   // end if if (errOK_els00 == rc)
    }   // end if (errOK_els00 != rc)

    if (true == _bCursorOpened)
    {
        char ErrTextIgnore[256];
        ls12CloseCursor(*pDBInfo, _pszTmpResultTableName, ErrTextIgnore);
    }

    delete [] _pszTmpResultTableName;
    _pszTmpResultTableName = NULL;

    if (NULL != pszBuffer)
    {
        delete [] pszBuffer;
        pszBuffer = NULL;
    }

    return errOK_els00;
}

/*
  -----------------------------------------------------------------------------
  function:     ls13GetColumnNamesInRecordOrder
  -----------------------------------------------------------------------------
*/
tsp00_Longint
ls13GetColumnNamesInRecordOrder(tsp7_table_description  *pTInfo,
                                Tools_DynamicUTF8String  pNameArray[],
                                tls00_CodeType           SrcEncoding,       // must give a hint on swap type in case of UCS2
                                tsp00_Addr               pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls13GetColumnNamesInRecordOrder");

    tsp00_Longint rc = errOK_els00;
    int m = 0;
    //*
    //*     Find column names in pTInfo structure.
    //*     Assign pointers to the names in order of value order in the record on page.
    //*
    //* The column names are located in the structure BEHIND the tsp7_ffield_infos. So first
    //* we move the pointer to the right place. The name buffer has the following structure:
    //* name length (1 byte)| name | name length (1 byte) | name | ...
    //*
    _TCHAR *pszColNameBuf = pTInfo->sp7t_colnames().asCharp();
    pszColNameBuf        += sizeof(tsp7_ffield_info) * pTInfo->sp7t_field_cnt();
    tsp00_Longint lLength = 0;

    for (m=0; m < pTInfo->sp7t_field_cnt(); ++m)
    {
        lLength       = *pszColNameBuf;    // name length
        pszColNameBuf += sizeof(_TCHAR);

        rc = ls05ConvertToUTF8(pNameArray[m], REINTERPRET_CAST(unsigned char*, pszColNameBuf),
                               lLength, SrcEncoding, pszErrText);

        pszColNameBuf += lLength;
    }

    return rc;
}
// ls13GetColumnNamesInRecordOrder()


/*
  -----------------------------------------------------------------------------
  function:     ls13GetColumnNamesInDefOrder
  -----------------------------------------------------------------------------
*/
tsp00_Longint
ls13GetColumnNamesInDefOrder(tsp7_table_description  *pTInfo,
                             Tools_DynamicUTF8String  pNameArray[],
                             tls00_CodeType           SrcEncoding,      // must give a hint on swap type in case of UCS2
                             tsp00_Addr               pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls13GetColumnNamesInDefOrder");

    tsp00_Longint rc = errOK_els00;

    int m = 0;
    //*
    //*     Find column names in pTInfo structure.
    //*     Assign pointers to the names in order of column definition numbers.
    //*
    //* The column names are located in the structure BEHIND the tsp7_ffield_infos. So first
    //* we move the pointer to the right place. The name buffer has the following structure:
    //* name length (1 byte)| name | name length (1 byte) | name | ...
    //*
    _TCHAR *pszColNameBuf = pTInfo->sp7t_colnames().asCharp();
    pszColNameBuf        += sizeof(tsp7_ffield_info) * pTInfo->sp7t_field_cnt();
    tsp00_Int4    colno   = 0;
    tsp00_Longint lLength = 0;

    for (m=0; m < pTInfo->sp7t_field_cnt(); ++m)
    {
        colno = pTInfo->sp7t_fields()[m].sp7fi_def_col_no;  // definition column number

        lLength        = *pszColNameBuf;    // name length
        pszColNameBuf += sizeof(_TCHAR);

        
        rc = ls05ConvertToUTF8(pNameArray[colno], REINTERPRET_CAST(unsigned char*, pszColNameBuf),
                               lLength, SrcEncoding, pszErrText);
        
        pszColNameBuf += lLength;
    }

    return rc;
}
// ls13GetColumnNamesInDefOrder()


/*
  -----------------------------------------------------------------------------
  function:     ls13RetrieveIndexNames
  -----------------------------------------------------------------------------
*/
tsp00_Longint
ls13RetrieveIndexNames(tls00_DBInfo            *pDBInfo,
                       tsp7_table_description  *pTInfo,
                       tsp7_index_description  *pIInfo,
                       Tools_DynamicUTF8String  TableName,
                       Tools_DynamicUTF8String  IndexNameArray[],
                       tsp00_Addr               pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls13RetrieveIndexNames");

    tsp00_Longint rc = errOK_els00;

    tsp00_Int4 lIndexCnt = pIInfo->sp7i_single_index_cnt + pIInfo->sp7i_mult_index_cnt;

    if (0 != lIndexCnt)
    {
        int        i     = 0;      // running index used when retrieving single AND multiple indexes
        tsp00_Int4 SICnt = pIInfo->sp7i_single_index_cnt;

        // variables used for unicode-ascii conversion
        tsp00_Longint lNameLen = 3*sizeof(tsp00_KnlIdentifierc);
        char          szName[3*sizeof(tsp00_KnlIdentifierc)];
        
        // We can use here the current database encoding to test for UNICODE encoding
        // of column names because source and target database MUST be of the same
        // encoding.
        tls00_CodeType SrcEncoding = ctAscii_els00;
        if (true == ls00UseUnicodeOI(pDBInfo))
        {
            SrcEncoding = ctUCS2_els00;     // NOT SWAPPED !!!
        }

        //*
        //*     Get single index names
        //* Single index names simply correspond to column names.
        //* Therfore we need to find column names in pTInfo structure.
        //* Assign pointers to the names in order of column definition numbers.
        //*
        //* The column names are located in the structure BEHIND the tsp7_ffield_infos. So first
        //* we move the pointer to the right place. The name buffer has the following structure:
        //* name length (1 byte)| name | name length (1 byte) | name | ...
        //*
        if (0 != SICnt)
        {
            Tools_DynamicUTF8String *pColNames = new Tools_DynamicUTF8String[pTInfo->sp7t_field_cnt() + 1];
            rc = ls13GetColumnNamesInDefOrder(pTInfo, pColNames, SrcEncoding, pszErrText);

            tsp7_info_single_index *pMySI = NULL;
            for (i = 0; ( (errOK_els00 == rc) && (i < SICnt) ); ++i)
            {
                pMySI = &(pIInfo->sp7i_single_index()[i]);

                // Format output
                szName[0] = 0;
                sp77sprintf(szName, STATIC_CAST(tsp00_Int4, lNameLen), SINGLE_INDEX_NAME_LS13, TableName.StrPtr(),
                                                                       pColNames[pMySI->sp7si_def_col_no].StrPtr());
                // Make output UTF8
                IndexNameArray[i] = STATIC_CAST(SAPDB_UTF8*, REINTERPRET_CAST(unsigned char*, (&szName[0])));

            }   // end for (i = 0; i < SICnt; ++i)

            delete [] pColNames;
        }   // end if (0 != SICnt)


        //*
        //*     Get multiple (composite) index names
        //*
        // Attention:
        // sizeof(tsp7_info_mult_index) (=56 byte) = 7 * sizeof(tsp7_info_single_index) (=8 byte)
        // Kernel fills single index structure always in steps of 7 indexes; that means that even in
        // case of only 8 single indixes on one table 14 single index_info-structures are 'used', so that
        // beginning at position idFirstMultIndex_ls00 = 3 multiple indexes may be inserted.
        //*
        //*     Find index names in pIInfo structure. ONLY MULTIPLE INDEXES HAVE NAMES!
        //*     Assign pointers to the names.
        //*
        //* The index names are located in the structure BEHIND the tsp7_info_mult_index. So first
        //* we move the pointer to the right place. The name buffer has the following structure:
        //* name length (1 byte)| name | name length (1 byte) | name | ...
        //*
        unsigned char *pszIndNameBuf = REINTERPRET_CAST(unsigned char*, pIInfo->sp7i_index_names().asCharp());
        pszIndNameBuf        += (pIInfo->sp7i_first_mult_index - 1 + pIInfo->sp7i_mult_index_cnt) * sizeof(tsp7_info_mult_index);
    
        tsp00_Longint           lIndexNameLen = 0;
        Tools_DynamicUTF8String NameUTF8;               // used to hold column and index names

        // We use here the i value that run the above for loop
        for (i; ( (errOK_els00 == rc) && (i < lIndexCnt) ); ++i)
        {
            lIndexNameLen  = *pszIndNameBuf;                   // name length
            pszIndNameBuf += sizeof(_TCHAR);

            rc = ls05ConvertToUTF8(NameUTF8, pszIndNameBuf, lIndexNameLen, SrcEncoding, pszErrText);
            if (errOK_els00 == rc)
            {
                sp77sprintf(szName, STATIC_CAST(tsp00_Int4, lNameLen), MULT_INDEX_NAME_LS13, NameUTF8.StrPtr());

                // Make output UTF8
                IndexNameArray[i] = STATIC_CAST(SAPDB_UTF8*, REINTERPRET_CAST(unsigned char*, (&szName[0])));

                // Move within buffer to next index name
                pszIndNameBuf += lIndexNameLen;
            }
        }   // end for (i; i < lIndexCnt; ++i)
    }

    return rc;
}
// ls13RetrieveIndexNames()


/*
  -----------------------------------------------------------------------------
  function:     ls13DropAllIndexesOnTable
  -----------------------------------------------------------------------------
*/
tsp00_Longint
ls13DropAllIndexesOnTable(tls00_DBInfo            *pDBInfo,
                          tsp7_table_description  *pTInfo,
                          tsp7_index_description  *pIInfo,
                          Tools_DynamicUTF8String  TableName,
                          tsp00_Addr               pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls13DropAllIndexesOnTable");

    tsp00_Longint rc = errOK_els00;

    tsp00_Int4 lIndexCnt = pIInfo->sp7i_single_index_cnt + pIInfo->sp7i_mult_index_cnt;
    if (0 != lIndexCnt)
    {
        Tools_DynamicUTF8String *pszNameArray = new Tools_DynamicUTF8String[lIndexCnt];

        rc = ls13RetrieveIndexNames(pDBInfo, pTInfo, pIInfo, TableName, pszNameArray, pszErrText);

        //*
        //*     drop indexes
        //*
        const tsp00_Int4 lStmtLen = (csp_max_indices + 2) * sizeof(tsp00_KnlIdentifierc) + 64;   // 64 extra bytes included
        char             szStmt[lStmtLen];
        _TCHAR           szErr[MAX_REPLY_LENGTH_LS00];
        tsp00_Int4       lErrPos  = 0;

        for (int i = 0; ( (i < lIndexCnt) && (errOK_els00 == rc) ); ++i)
        {
            // build drop stmts
            szStmt[0] = 0;
            if (i < pIInfo->sp7i_single_index_cnt)
            {
                sp77sprintf(szStmt, lStmtLen, DROP_SI_STMT_LS28, pszNameArray[i].StrPtr());
            }
            else
            {
                sp77sprintf(szStmt, lStmtLen, DROP_MI_STMT_LS28, pszNameArray[i].StrPtr(), TableName.StrPtr());
            }

            // genereated statement is already UTF8!

            // execute stmt
            rc = ls13ExecIntSQLStmt(pDBInfo, 0, szStmt, szErr, lErrPos);
            if (errOK_els00 != rc)
            {
                LoaderMessages::Instance()->ls98Msg(layTableLoad_els98, pszErrText, errInternSQL_els98,
                                                                                     szStmt, szErr);
                rc = errInternSQL_els98;
            }

        }   // end for (int i = 0; ( (i < lIndexCnt) && (errOK_els00 == rc) ); ++i)


        if (NULL != pszNameArray)
        {
            delete [] pszNameArray;
            pszNameArray = NULL;
        }

    }       // end if (0 != lIndexCnt)

    return rc;
}
// ls13DropAllIndexesOnTable()


/*
  -----------------------------------------------------------------------------
  function:     ls13CreateIndexes
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls13CreateIndexes(tls00_DBInfo            *pDBInfo,
                  tsp7_table_description  *pTInfo,
                  tsp7_index_description  *pIInfo,
                  Tools_DynamicUTF8String  TableName,
                  tsp00_Addr               pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls13CreateIndexes");

    tsp00_Longint rc = errOK_els00;

    tsp00_Int4 lIndexCnt = pIInfo->sp7i_single_index_cnt + pIInfo->sp7i_mult_index_cnt;

    //*
    //*     Create indexes
    //*
    if (0 != lIndexCnt)
    {
        tsp00_Int4 i       = 0;
        tsp00_Int4 lColCnt = pTInfo->sp7t_field_cnt() + 1;

        //*
        //*     Define encoding of source database
        //*
        //* We can use here the current database encoding to test for UNICODE encoding
        //* of column names because source and target database MUST be of the same encoding.
        //*
        tls00_CodeType SrcEncoding = ctAscii_els00;
        if (true == ls00UseUnicodeOI(pDBInfo))
        {
            SrcEncoding = ctUCS2_els00;
        }

        //*
        //*     Get column names needed to build index statements
        //*
        Tools_DynamicUTF8String *pColNameUTF8 = new Tools_DynamicUTF8String[lColCnt];
        Tools_DynamicUTF8String *pIndexName   = new Tools_DynamicUTF8String[lIndexCnt];
        
        rc = ls13GetColumnNamesInDefOrder(pTInfo, pColNameUTF8, SrcEncoding, pszErrText);
        if (errOK_els00 == rc)
        {
            //*
            //*     Get index names out of index description structure
            //*
            rc = ls13RetrieveIndexNames(pDBInfo, pTInfo, pIInfo, TableName, pIndexName, pszErrText);
        }

        _TCHAR     szErr[MAX_REPLY_LENGTH_LS00];
        tsp00_Int4 lErrPos = 0;

        const tsp00_Int4        MI_STMT_LEN          = (csp_max_indices + 2) * KNL_IDENTIFIER_SIZE + 64;   // 64 extra bytes included
        char                    szStmt[MI_STMT_LEN];
        tsp7_info_mult_index   *pMyMI                = NULL;
        tsp7_info_single_index *pMySI                = NULL;
        tsp00_Int4 lPos                              = 0;

        for (i=0; ( (i < lIndexCnt) && (errOK_els00 == rc) ); ++i)
        {
            szStmt[0] = 0;
            lPos      = 0;

            //*
            //*     Build CREATE INDEX statements
            //*
            if (i < pIInfo->sp7i_single_index_cnt)      // single indexes
            {
                pMySI = &(pIInfo->sp7i_single_index()[i]);

                if (1 == pIInfo->sp7i_single_index()[i].sp7si_unique)
                {
                    sp77sprintf(szStmt, MI_STMT_LEN, SINGLE_UNIQUE_INDEX_STMT_LS13, TableName.StrPtr(),
                                                                                    pColNameUTF8[pMySI->sp7si_def_col_no].StrPtr());
                }
                else
                {
                    sp77sprintf(szStmt, MI_STMT_LEN, SINGLE_INDEX_STMT_LS13, TableName.StrPtr(),
                                                                             pColNameUTF8[pMySI->sp7si_def_col_no].StrPtr());
                }
            }
            else                                        // multiple (composite) indexes
            {
                // Get the pointer to the structure of a single multiple index
                // - sp7i_first_mult_index in pIInfo gives the first multindex-struct to use;
                //   because this value is in PASCAL notation we have to substract 1
                // - to include the running index i we must substract the count of single indexes from i
                pMyMI = &(pIInfo->sp7i_mult_index()[(i - pIInfo->sp7i_single_index_cnt) + pIInfo->sp7i_first_mult_index - 1]);

                if (1 == pMyMI->sp7mi_unique)                                     // "CREATE UNIQUE INDEX ""
                {
                    lPos = sp77sprintf(szStmt, MI_STMT_LEN, MULT_UNIQUE_INDEX_STMT_LS13, pIndexName[i].StrPtr(),
                                                                                         TableName.StrPtr());
                }
                else
                {
                    lPos = sp77sprintf(szStmt, MI_STMT_LEN, MULT_INDEX_STMT_LS13, pIndexName[i].StrPtr(),
                                                                                  TableName.StrPtr());
                }

                for (int k = 0; k < pMyMI->sp7mi_col_cnt; ++k)
                {
                    lPos += sp77sprintf(szStmt + lPos, (MI_STMT_LEN - lPos), "\"%s\",",
                                                                             pColNameUTF8[pMyMI->sp7mi_def_col_no[k]].StrPtr());
                    if (1 == pMyMI->sp7mi_descending[k])
                    {
                        lPos += sp77sprintf((szStmt + lPos - 1), (MI_STMT_LEN - lPos + 1), " DESC,") - 1;
                    }
                }

                szStmt[lPos-1] = ')';     // terminated stmt with )
                // terminating Null inserted from sprintf
            }

            //*
            //*     Run CREATE INDEX statement
            //*
            rc = ls13ExecIntSQLStmt(pDBInfo, 0, szStmt, szErr, lErrPos);
            if (0 != rc)
            {
                LoaderMessages::Instance()->ls98Msg(layTableLoad_els98, pszErrText, errInternSQL_els98,
                                                                                     szStmt, szErr);
                rc = errInternSQL_els98;
            }

        }   // end for (i = 0; i < SICnt; ++i)


        if (NULL != pIndexName)
        {
            delete [] pIndexName;
            pIndexName = NULL;
        }

        if (NULL != pColNameUTF8)
        {
            delete [] pColNameUTF8;
            pColNameUTF8 = NULL;
        }
        
    }   // end if (0 != lIndexCnt)

    return STATIC_CAST(tsp00_Int4, rc);
}
// ls13CreateIndexes()


/*
  -----------------------------------------------------------------------------
  function:     ls13GetDefaultValue
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls13GetDefaultValue(tls00_DBInfo             *pDBInfo,
                    Tools_DynamicUTF8String  &TableName,
                    tsp00_Int2                lColumnNo,
                    _TCHAR                  *&pDefault,
                    tsp00_Int2               &lDefaultLength,
                    tsp00_Addr                pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls13GetDefaultValue");

    tsp00_Int4 rc = errOK_els00;

    const size_t max_cmd_len = 4*sizeof(tsp00_KnlIdentifier);
    _TCHAR szCmd[max_cmd_len];
    szCmd[0] = 0;
    sp77sprintf(szCmd, max_cmd_len, DESCRIBE_DEFAULT_STMT_LS13, TableName.StrPtr());

    if (errOK_els00 == rc)
    {
        tsp00_Int4 lErrorPos = 0;

        // For defining the default we send "DESCRIBE DEFAULT <table name>" plus column no
        // of column for which the default is defined
        rc = ls13ExecIntSQLStmtEx(pDBInfo, 0, szCmd, REINTERPRET_CAST(_TCHAR*, &lColumnNo),
                                  sizeof(tsp00_Int2), pszErrText, lErrorPos);
        if (errOK_els00 == rc)
        {
            tsp1_packet *pSqlPacket = pDBInfo->dbiPktRcvList_ls00[0];
            tsp1_part   *pSqlPart   = NULL;
            if ( false == ls04GetPart(pSqlPacket, sp1pk_data, pSqlPart) )
            {
                LoaderMessages::Instance()->ls98Msg(laySQLInterface_els98, pszErrText, errInternal_els98,
                                                                                        __FILE__,
                                                                                        __LINE__,
                                                                                        rc);
                rc = errInternal_els98;
            }
            else
            {
                // what does the value in answer packet look like
                //  kernel version 7.2.
                //      ------------------------------------------------------------
                //      | value length | default value with leading defined byte   |
                //      ------------------------------------------------------------
                //         1 byte           length byte
                //
                //  kernel version 7.3.
                //      ------------------------------------------------------------
                //      | value length | default value with leading defined byte   |
                //      ------------------------------------------------------------
                //         2 byte           length byte

                tsp00_Int4 lPartLen = 0;
                ls04GetRawDataPtr(pSqlPacket, pDefault, lPartLen);
            
                // Define length of value and move pointer to correct place
                if ( ('7' == pDBInfo->dbiDBVersion_ls00.MajorVersion_sp100) &&
                     ('2' == pDBInfo->dbiDBVersion_ls00.MinorVersion_sp100) )
                {
                    lDefaultLength =  pDefault[0];       // implicit cast to tsp00_Int2
                    pDefault       += 1;
                }
                else
                {
                    // assuming that kernel delivers 2 byte length value in swap of REPMServer              
                    lDefaultLength =  *(REINTERPRET_CAST(tsp00_Int2*, pDefault));
                    pDefault       += sizeof(tsp00_Int2);
                }
                
                TRACE_PRNF_MLS99(("ls13GetDefaultValue", "Column no: %i, default value '%.*s'",
                                                         lColumnNo,
                                                         lDefaultLength,
                                                         pDefault));

            }   // if ( false == ls04GetPart(pSqlPacket, sp1pk_data, pSqlPart) )
        }   // if (errOK_els00 == rc)
        else
        {
            LoaderMessages::Instance()->ls98Msg(laySQLInterface_els98, pszErrText, errInternal_els98,
                                                                                    __FILE__,
                                                                                    __LINE__,
                                                                                    rc);
            rc = errInternal_els98;
        }
    }   // end if (errOK_els00 == rc) for ls05ConvertToUTF8

    return rc;
}
// ls13GetDefaultValue()


/*
  -----------------------------------------------------------------------------
  function:     ls13CheckTableExistence
  -----------------------------------------------------------------------------
*/
tsp00_Longint
ls13CheckTableExistence(tls00_DBInfo            *pDBInfo,
                        Tools_DynamicUTF8String  TableOwner,
                        Tools_DynamicUTF8String  TableName,
                        bool                    &bTableExists,
                        tsp00_Addr               pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls13CheckTableExistence");

    tsp00_Longint rc = errOK_els00;

    //*
    //*     Check existence
    //* In case that table does not exist we get the error number -4004.
    //*
    bTableExists = false;

    _TCHAR szCmd[SHORT_STMT_LENGTH_LS00];
    szCmd[0] = 0;

    sp77sprintf(szCmd, SHORT_STMT_LENGTH_LS00, EXISTS_TABLE_STMT_LS00, TableOwner.StrPtr(), TableName.StrPtr());

    tsp00_Int4 lErrPos;
    _TCHAR     szErr[MAX_REPLY_LENGTH_LS00];

    rc = ls13ExecIntSQLStmt(pDBInfo, 0, szCmd, szErr, lErrPos);
    if (errOK_els00 != rc)
    {
        if (dbUnknownTableName_els00 == rc)
        {
            rc = errOK_els00;
        }
        else
        {
            LoaderMessages::Instance()->ls98Msg(layTableLoad_els98, pszErrText, errCheckTableExistence_els98,
                                                                                 TableName.StrPtr(), szErr);
            rc = errCheckTableExistence_els98;
        }
    }
    else
    {
        bTableExists = true;
    }

    return rc;
}
// ls13CheckTableExistence()


/*
  -----------------------------------------------------------------------------
  function:     ls13Prepare_FL_TL_Packet - useful only for fastload and dataload
  -----------------------------------------------------------------------------
*/
void
ls13Prepare_FL_TL_Packet(tls00_DBInfo  *pDBInfo,
                         tsp00_Int4     lPcktNo,
                         tsp1_packet  *&pSqlPacket,     // reference to a pointer !!!
                         tsp1_part    *&pSqlPart)       // reference to a pointer !!!
{
    ROUTINE_DBG_MEO00 ("ls13Prepare_FL_TL_Packet");

    tsp00_Int4    rc       = errOK_els00;
    tsp1_segment *pSqlSegm = NULL;

    pSqlPacket = pDBInfo->dbiPktSndList_ls00[lPcktNo];

    ls04FastInit(pSqlPacket, &pSqlSegm, &pSqlPart, pDBInfo);

    ls04SetMsgType(pSqlSegm, sp1m_load);
    
    ls04SetPartKind(pSqlPart, sp1pk_page);

    return;
}
// ls13Prepare_FL_TL_Packet


/*
  -----------------------------------------------------------------------------
  function:     ls13SendBdinfoPckt
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls13SendBdinfoPckt(tls00_DBInfo*  pDBInfo,
                   SAPDB_Int4     lPcktNo,
                   tls00_BDInfo*  pBdinfo,
                   tsp00_Addr     pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls13SendBdinfoPckt");
    
    tsp00_Int4 rc = errOK_els00;

    tsp1_packet  *pSqlPacket = pDBInfo->dbiPktSndList_ls00[lPcktNo];
    tsp1_segment *pSqlSegm   = ls04GetSegment(pSqlPacket);          // We need to get the pointer to segment
    tsp1_part    *pSqlPart   = NULL;                                // for subsequent actions

    //*     Add bdinfo part
    rc = ls04InitPart(pSqlPacket, pSqlSegm, &pSqlPart, sp1pk_bdinfo);
    ls04BufToPart(pSqlPart, pBdinfo->szbdiData_ls00, pBdinfo->bdiLength_ls00);

    rc = ls04FinishPart (pSqlPacket, pSqlPart);
    if (errOK_els00 == rc)
    {
        // Send packet
        rc = ls03SQLRequest (pDBInfo, lPcktNo, pszErrText);
        if (errOK_els00 == rc)
        {
            TRACE_PRNF_MLS99(("ls13SendBdinfoPckt", "Sent PacketNo: %i", lPcktNo));
        }
    }    
     return rc;
}

/*
  -----------------------------------------------------------------------------
  function:     ls13ReceiveBdinfoPckt
  -----------------------------------------------------------------------------
*/
SAPDB_Int4
ls13ReceiveBdinfoPckt(tls00_DBInfo*  pDBInfo,
                      SAPDB_Int4     lPcktNo,
                      tls00_BDInfo*  pBdInfo,
                      tsp00_Addr     pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls13ReceiveBdinfoPckt");

    tsp00_Int4  rc          = errOK_els00;
    tsp1_packet *pSqlPacket = NULL;
    
    //*
    //*     Get answer packet from kernel.
    //*
    rc = ls03SQLReceive (pDBInfo, lPcktNo, pszErrText);
    if (errOK_els00 == rc)
    {
        TRACE_PRNF_MLS99 (("ls13ReceiveBdinfoPckt", "Received packet No: %i", lPcktNo));
        
        pSqlPacket = pDBInfo->dbiPktRcvList_ls00[lPcktNo];      // Get ptr to answer packet

        rc = ls04RcFromPacket(pSqlPacket, pszErrText);          // Check SQL packet for errors from kernel
        if (rc >= errOK_els00)                                  // We don't analyse the error but simply give the
        {                                                       // return code back to caller
            //*
            //*     Now get dbinfo from packet
            //* Get dbinfo packet even in case of sql error; this is then handled by the caller
            //*
            tsp00_Int4 rc2 = ls04GetBdinfo(pSqlPacket, pBdInfo);
        }
    }
    return rc;
}

/*  
  -----------------------------------------------------------------------------
  EndChapter: Public functions implementation
  -----------------------------------------------------------------------------
*/


/*  
  -----------------------------------------------------------------------------
  Chapter: Private functions implementation
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  function:     ls13_GetRawDescription
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls13_GetRawDescription(tls00_DBInfo *DBInfo,
                      tsp00_Int4     lPacketNo,
                      tsp00_Addr     pszCmd,
                      tsp00_Int4    &lDescriptionLen,
                      tsp00_Addr    &pszDescription,
                      tsp00_Addr     pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls13_GetRawDescription");

    tsp00_Int4 rc       = errOK_els00;
    tsp00_Int4 ErrorPos = 0;

    rc = ls13ExecIntSQLStmt(DBInfo, lPacketNo, pszCmd, pszErrText, ErrorPos);
    if (errOK_els00 == rc)
    {
        rc = ls04GetRawDataPtr(DBInfo->dbiPktRcvList_ls00[lPacketNo], pszDescription, lDescriptionLen);
        if (errOK_els00 != rc)
        {
            rc = errInternal_els98;
        }
    }

    return rc;
} 
//  ls13_GetRawDescription()


/*
  -----------------------------------------------------------------------------
  function:     ls13_PutCmd
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls13_PutCmd(tls00_DBInfo            *pDBInfo,
            tsp00_Int4               lPcktNo,
            const char*              pszSqlStmt,
            tsp00_Addr               pszErrText,
            tsp1_producer_Enum       CmdProducer,
            bool                     bWithInfo,  // = false
            tsp1_cmd_mess_type_Enum  MessType,   // = sp1m_dbs
            bool                     bMassCmd)   // = false
{
    tsp00_Longint rc = errOK_els00;

    Tools_DynamicUTF8String SqlStmtUTF8(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, pszSqlStmt));

    tsp00_Longuint  lStmtLenUTF8 = SqlStmtUTF8.BasisSize();
    _TCHAR*         pszMySqlStmt = NULL;

    tsp00_Longuint lStmtLen = 0;
    tls00_CodeType DestCode = ctAscii_els00;

    if (true == ls00UseUnicodeOI(pDBInfo))       // Check for encoding of order interface to use
    {
        lStmtLen = 4*lStmtLenUTF8;
        DestCode = ctUCS2Native_els00;          // in case of commands the UCS2 encoded string must be in native swap
    }
    else
    {
        lStmtLen = lStmtLenUTF8;
    }

    pszMySqlStmt = new _TCHAR[lStmtLen];

    rc = ls05ConvertFromUTF8(REINTERPRET_CAST(unsigned char*, pszMySqlStmt),
                             lStmtLen,
                             SqlStmtUTF8,
                             DestCode,
                             pszErrText);
    if (errOK_els00 == rc)
    {
        tsp1_packet  *pPacket = pDBInfo->dbiPktSndList_ls00[lPcktNo];
        tsp1_segment *pSegm   = NULL;
        tsp1_part    *pPart   = NULL;
    
        //*
        //*     Fill packet with command (1. part)
        //*
        ls04FastInit(pPacket, &pSegm, &pPart, pDBInfo, CmdProducer);

        if (sp1m_dbs != MessType)
        {
            ls04SetMsgType(pSegm, MessType);
        }
    
        if (false != bWithInfo)
        {
            ls04SetWInfoFlag(pSegm, bWithInfo);
        }

        if (false != bMassCmd)
        {
            ls04SetMCFlag(pSegm, bMassCmd);
        }

        rc = ls04BufToPart(pPart, pszMySqlStmt, STATIC_CAST(tsp00_Int4, lStmtLen));
        if (errOK_els00 != rc)
        {
            LoaderMessages::Instance()->ls98Msg(laySQLInterface_els98, pszErrText, errSQLStmtTooLong_els98);
            rc = errSQLStmtTooLong_els98;
        }
        else
        {
            rc = ls04FinishPart(pPacket, pPart);
            if (errOK_els00 != rc)
            {
                LoaderMessages::Instance()->ls98Msg(laySQLInterface_els98, pszErrText,
                                                                            errInternal_els98,
                                                                            _INTERNAL_FILE_ID_,
                                                                            __LINE__,
                                                                            errFinishPart_els98);
                rc = errInternal_els98;
            }
        }
    }

    if (NULL != pszMySqlStmt)      // some cleaning work
    {
        delete [] pszMySqlStmt;
        pszMySqlStmt = NULL;
    }

    return STATIC_CAST(tsp00_Int4, rc);

}
// ls13_PutCmd()


/*
  -----------------------------------------------------------------------------
  function:     ls13_GetCorrectErrorPos
  -----------------------------------------------------------------------------
*/
static void
ls13_GetCorrectErrorPos(const char*  pszSqlCmd,
                        tsp00_Int4   lErrPos,
                        tsp00_Int4  &lRow,
                        tsp00_Int4  &lCol)
{
    ROUTINE_DBG_MEO00 ("ls13_GetCorrectErrorPos");

    lCol = lErrPos;
    lRow = 1;

    char*      pszSqlStmt = CONST_CAST(char*, pszSqlCmd);
    tsp00_Int4 lPos       = 0;
    char*      pdest      = pszSqlStmt;

    while ( NULL != (pdest = strchr(pszSqlStmt, '\n') ) )
    {
        lPos = pdest - pszSqlStmt + sizeof(char);
        if (lPos < lCol)
        {
            lRow       += 1;
            lCol       -= lPos;

            pszSqlStmt += lPos;
        }
        else
        {
            break;
        }
    }   // end while (lPos < lCol)
}
// ls13_GetCorrectErrorPos()

#define sp1s_segm_len       variant.C_1.sp1s_segm_len_F // redefine undefined value - see beginning of file
/*  
  -----------------------------------------------------------------------------
  EndChapter: Private functions implementation
  -----------------------------------------------------------------------------
*/
