/*
  -----------------------------------------------------------------------------
  module: vls15.cpp
  -----------------------------------------------------------------------------
  responsible:  SteffenS,d033893

  special area: Loader

  description:  user management

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
#include <assert.h>

#include "gls00.h"
#include "hls98msg.h"

#include "hls03.h"
#include "hls04.h"
#include "hls041.h"
#include "hls05.h"
#include "hls07.h"      // NumberToInt conversion
#include "hls12.h"      // for code page table - may be another place fits better?
#include "hls13.h"
#include "hwd07dbc.h"
#include "hls31_odbc_Result.h"
#include "hls15.h"
#include "hls99.h"

// codepage table loading at connect
#include "heo01.h"          // Working path
#include "hls30.h"

#include "heo13.h"
#include "RunTime/Security/RTESec_Authentication.hpp"
#include "SAPDBCommon/Fields/SAPDBFields_VarData.hpp"

externPascal tin_global_in_vars i01g;

#define CONNECT_STMT_LS15               _T("CONNECT \"%s\" IDENTIFIED BY :X ISOLATION LEVEL %d")
#define CONNECT_STMT_AUTH_LS15          _T("CONNECT \"%s\" AUTHENTICATION")

#define CREATE_CODEPAGE_TABLE_LS15      _T("CREATE TABLE %s.CODEPAGE (CODEPAGEID CHAR (32) NOT NULL, CODEPAGETABLE CHAR (512) BYTE NOT NULL, PRIMARY KEY (CODEPAGEID)) INTERNAL")
#define GRANT_CODEPAGE_TABLE_LS15       _T("GRANT  ALL ON %s.CODEPAGE TO PUBLIC")
#define INSERT_CODEPAGE_TABLE_LS15      _T("INSERT INTO %s.CODEPAGE VALUES('%s', x'%.*s')")
#define SELECT_CODEPAGE_TABLE_LS15      _T("SELECT CODEPAGETABLE FROM %s.CODEPAGE WHERE CODEPAGEID = '%s'")

#define SELECT_SESSIONTIMEOUT_76_LS15   _T("SELECT VALUE FROM SYSINFO.ACTIVECONFIGURATION WHERE PARAMETERNAME = 'SESSION_TIMEOUT'")
#define SELECT_SESSIONTIMEOUT_75_LS15   _T("SELECT VALUE FROM SYSDBA.DBPARAMETERS WHERE DESCRIPTION = 'SESSION_TIMEOUT'")

#define MAX_CONNECT_CMD_LENGTH_LS15     512


/* Lokale Funktionen: ******************************************************************/
/*!
  -----------------------------------------------------------------------------
  function:     ls15ConnectUser
  -----------------------------------------------------------------------------
  description:  connects a specified user to the database (ascii or unicode)
  
  arguments:    DBInfo   [IN]   - Structure with all information on the running
                                   session and the db connected to
                ErrText  [OUT]  - Error text

  returnvalue:  errOK_els00       - everything went fine
                errInternal_els98 - error processing 'after connect' information
                sql error         - error connecting user; delivered by kernel

                This function logs error messages only to the return string!
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls15ConnectUser(tls00_DBInfo*  DBInfo, tsp00_Addr  ErrText);


/*
  -----------------------------------------------------------------------------
  function:     ls15SwitchDB
  -----------------------------------------------------------------------------
*/
static SAPDB_Int4
ls15SwitchDB(tls00_DBInfo* pDBInfo, tsp00_Addr pszErrText);


/*
  -----------------------------------------------------------------------------
  function: ls15_UserConnect_MD5
  -----------------------------------------------------------------------------
*/
static SAPDB_Int4
ls15_UserConnect_MD5(tls00_DBInfo* pDBInfo, tsp00_Addr pszErrText);


/*
  -----------------------------------------------------------------------------
  function: ls15_UserConnect_Encrypt
  -----------------------------------------------------------------------------
*/
static SAPDB_Int4
ls15_UserConnect_Encrypt(tls00_DBInfo* pDBInfo, tsp00_Addr pszErrText);

/*
  -----------------------------------------------------------------------------
  function:     ls15_CheckConnectPacket - local function
  -----------------------------------------------------------------------------

  description:  Gets the following information from the reply packet of the
                logon request:
                 - page size
                 - conversion tables
                 - kernel version
                 - user group of connected user

  arguments:    pDBInfo  [IN]    - structure with all info on the running session
                                    and the db connected to.
                PacketNo [IN]    - Number of packet to be filled.
                pszErrText [OUT] - error text

  returnvalue:  errOK_els00        - everything went fine
                errInternal_els98  - error getting information; logs the 'real' error
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls15_CheckConnectPacket(tls00_DBInfo *pDBInfo, tsp00_Int4 PacketNo, tsp00_Addr pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls15_CheckConnectPacket");

    tsp00_Int4       rc         = errOK_els00;
    LoaderMessages  *pMsgInstance    = LoaderMessages::Instance();
    tsp1_packet     *pSqlPacket = pDBInfo->dbiPktRcvList_ls00[PacketNo];
    tsp1_part       *pSqlPart   = NULL;

    //*
    //*     Get page size, code type and kernel version
    //*
    if ( true == ls04GetPart(pSqlPacket, sp1pk_session_info_returned, pSqlPart) )
    {
        // Code type (check for UNICODE kernel)
        _TCHAR fUnicodeFlag;
        ls04BufFromPartEx(pSqlPart,0, 1, &fUnicodeFlag, 1);

        if (1 == fUnicodeFlag) //SDC Migration
        {
            pDBInfo->dbiEncoding_ls00 = ctUCS2_els00;             // Database is UNICODE
            pDBInfo->dbiOI_Encoding_ls00 = ctUCS2Native_els00;    // Order Interface set to UNICODE, too
        }
        
        // Page size
        // Convert size from internal format to readable format. In case the number
        // is truncated that's not an error - we proceed working.
        _TCHAR szPageSize[RESNUM_MXSP00] = {0};

        ls04BufFromPartEx(pSqlPart, 2193, 5, szPageSize, mxsp_resnum);

        rc = ls07NumberToInt(szPageSize, pDBInfo->dbiPageSize_ls00);
        if (errOK_els00 != rc)
        {
            if (errNumberTruncated_els98 != rc)
            {
                pMsgInstance->ls98MsgToString(layBasicConnect_els98, pszErrText, errInternal_els98,
                                                                            __FILE__,
                                                                            __LINE__, errConvertPageSize_els98);
                TRACE_BUFF_MLS99 ("ls15_CheckConnectPacket", "Page size(number format): %.*s\n",
                                                             RESNUM_MXSP00, szPageSize);
                rc = errInternal_els98;
            }
            else
            {
                rc = errOK_els00;
            }
        }
        if (errOK_els00 == rc)
        {
            TRACE_PRNF_MLS99 (("ls15_CheckConnectPacket", "Page size converted: %d\n", pDBInfo->dbiPageSize_ls00));
        }

        // Kernel version
        if (errOK_els00 == rc)
        {
            unsigned char szVersion[5] = {0};
            ls04BufFromPartEx(pSqlPart, 2200, 5, REINTERPRET_CAST(char*, szVersion), 5);

            pDBInfo->dbiDBVersion_ls00.MajorVersion_sp100 = szVersion[0] - '0';
            pDBInfo->dbiDBVersion_ls00.MinorVersion_sp100 = szVersion[2] - '0';
            pDBInfo->dbiDBVersion_ls00.CorrLevel_sp100    = szVersion[4] - '0';
            pDBInfo->dbiDBVersion_ls00.BuildNumberPrefix_sp100 = 0;
        }

        char cDateTimeFormat[2] = {0};
        ls04BufFromPartEx(pSqlPart, 169, 1, cDateTimeFormat, 2);
        pDBInfo->dbiDateTimeFormat_ls00 = STATIC_CAST(tgg00_DateTimeFormat_Enum, cDateTimeFormat[0]);

    }
    else
    {
        pMsgInstance->ls98MsgToString(layBasicConnect_els98, pszErrText, errInternal_els98,
                                                                    __FILE__,
                                                                    __LINE__, errUnknownPart_els98);
        rc = errInternal_els98;
    }

    //*
    //*     Get the conversion tables and put them into GLOBAL (!!!) structure i01g
    //*     Put the code type in i01g, too.
    //*
    if (errOK_els00 == rc)
    {
        pSqlPart = NULL;
        if ( true == ls04GetPart(pSqlPacket, sp1pk_conv_tables_returned, pSqlPart) )
        {
            tsp00_Int4 Len = ls04GetPartLength(pSqlPart);
            ls04BufFromPartEx(pSqlPart,
                              0,
                              Len,
                              REINTERPRET_CAST(char*, pDBInfo->dbiSession_ls00.siConversionTables_ls00),
                              sizeof(pDBInfo->dbiSession_ls00.siConversionTables_ls00));

            //  This is only for the conversion of date, time and timestamp values using the old
            //  vin - methods.
            for (int i = 0; i < (Len/STATIC_CAST(int, sizeof(tsp00_Ctable))); ++i)
            {
                i01g->i32.transtables[i] = pDBInfo->dbiSession_ls00.siConversionTables_ls00[i];
            }

            i01g->i20.code_type = pSqlPacket->sp1_header.sp1h_mess_code;
        }
        else
        {
            pMsgInstance->ls98MsgToString(layBasicConnect_els98, pszErrText, errInternal_els98,
                                                                        __FILE__,
                                                                        __LINE__, errUnknownPart_els98);
            rc = errInternal_els98;
        }
    }

    //*
    //*     Get some user related information: SYSDBA name, UserGroup name
    //*
    if (errOK_els00 == rc)
    {
        pSqlPart = NULL;
        if ( true == ls04GetPart(pSqlPacket, sp1pk_user_info_returned, pSqlPart) )
        {
            unsigned char  szAuxKnlID[sizeof(tsp00_KnlIdentifierc)];
            _TCHAR         szGroup[512];

            tsp00_Int4 Len = ls04GetPartLength(pSqlPart);
            ls04BufFromPartEx(pSqlPart, 0, Len, szGroup, sizeof(szGroup));

            //  Separate sysdba name and usergroup name out of the returned value.
            //  The value returned contains the
            //  following information (each info is preceeded by a byte given the length of info):
            //  - sysdba of serverdb to which current user is connected (max. 96 bytes)
            //  - sysdba of serverdb where the current user was created (max. 96 bytes)
            //  - user type (max. 16 bytes)
            //  - user group user belongs to (max. 96 bytes)
            short      count         = 0;

            //
            //  If the connection has been rebuild the DB might be a UNICODE DB. In this case the kernel
            //  returns the names of SYSDBA and some other user related information in UCS2Native. Thus they
            //  have to be converted to UTF8. In case of first time connect the data is always returned in ASCII.
            //
            memset(pDBInfo->dbiSYSDBA_Name_ls00, 0, sizeof(tsp00_KnlIdentifierc));
            
            SAPDB_memcpy(szAuxKnlID, &szGroup[count] + sizeof(char), szGroup[count]);

            if ( (SAPDB_FALSE == pDBInfo->dbiSession_ls00.siReconnect_ls00.rcoDoReconnect_ls00) ||
                 (SAPDB_FALSE == ls00IsDBUnicode(pDBInfo)))
            {
                SAPDB_memcpy(pDBInfo->dbiSYSDBA_Name_ls00, szAuxKnlID, szGroup[count]);
            }
            else
            {
                char szErrText[MAX_REPLY_LENGTH_LS00];
                SAPDB_Long lDestLen = sizeof(tsp00_KnlIdentifierc);

                SAPDB_Int4 rc = ls05ConvertFromUCS2Simple(REINTERPRET_CAST(unsigned char*, pDBInfo->dbiSYSDBA_Name_ls00.asCharp()),
                                                          szAuxKnlID,
                                                          lDestLen,
                                                          szGroup[count],
                                                          ctUTF8_els00,
                                                          szErrText);
            }

            // Group name - has been filled with zeroes upon initialization in ls01_InitDBInfo
            for (int j = 0; j < 3; ++j)     // only 3 loops because the first count is the init
            {
                count += szGroup[count] + 1;
            }
            if (0 != szGroup[count])
            {
                SAPDB_memcpy(szAuxKnlID, &szGroup[count] + sizeof(_TCHAR), szGroup[count]);

                memset(pDBInfo->dbiSession_ls00.siUserGroup_ls00.asCharp(), 0, sizeof(tsp00_KnlIdentifier));

                if ( (SAPDB_FALSE == pDBInfo->dbiSession_ls00.siReconnect_ls00.rcoDoReconnect_ls00) ||
                     (SAPDB_FALSE == ls00IsDBUnicode(pDBInfo)))
                {
                    SAPDB_memcpy(pDBInfo->dbiSession_ls00.siUserGroup_ls00.asCharp(), szAuxKnlID, szGroup[count]);
                }
                else
                {
                    char szErrText[MAX_REPLY_LENGTH_LS00];
                    SAPDB_Long lDestLen = sizeof(tsp00_KnlIdentifier);

                    SAPDB_Int4 rc = ls05ConvertFromUCS2Simple(REINTERPRET_CAST(unsigned char*, pDBInfo->dbiSession_ls00.siUserGroup_ls00.asCharp()),
                                                              szAuxKnlID,
                                                              lDestLen,
                                                              szGroup[count],
                                                              ctUTF8_els00,
                                                              szErrText);
                }
            }
            else
            {
                SAPDB_memcpy(pDBInfo->dbiSession_ls00.siUserGroup_ls00.asCharp(),
                             pDBInfo->dbiSession_ls00.siUserName_ls00.asCharp(),
                             pDBInfo->dbiSession_ls00.siUserName_ls00.length() );
            }
        }
        else
        {
            pMsgInstance->ls98MsgToString(layBasicConnect_els98, pszErrText, errInternal_els98,
                                                                        __FILE__,
                                                                        __LINE__, errUnknownPart_els98);
            rc = errInternal_els98;
        }
    }

    return rc;
}
// ls15_CheckConnectPacket()

/* Globale Funktionen: *****************************************************************/


/*
  -----------------------------------------------------------------------------
  function:     ls15CommitUser
  -----------------------------------------------------------------------------
*/
int ls15CommitUser(tls00_DBInfo* DBInfo, tsp00_Addr ErrText)
{
    ROUTINE_DBG_MEO00 ("ls15CommitUser");

    LoaderMessages*  MsgInstance = LoaderMessages::Instance();    // error reporting object

    tsp00_Int4  rc = errConnectNoServerDB_els98;                    // Initialize to error

    //*
    //*     Check for db connection and user connection to db
    //*     Notify caller if no connection to db established. Otherwise release connection.
    //*
    if ( (true == DBInfo->dbiSession_ls00.siSqlStat_ls00) && (true == DBInfo->dbiSession_ls00.siDBConnection_ls00) )
    {
        if (true == DBInfo->dbiSession_ls00.siAutoCommit_ls00)
        {
            MsgInstance->ls98PrintMessage(layBasicConnect_els98, msgReleaseUserComm_els98,
                                                                 DBInfo->dbiSession_ls00.siUserName_ls00.asCharp());
        }
        else
        {
            MsgInstance->ls98PrintMessage(layBasicConnect_els98, msgReleaseUserRollback_els98,
                                                                 DBInfo->dbiSession_ls00.siUserName_ls00.asCharp());
        }

        rc = ls15Rollback(DBInfo, ErrText, true);       // rollback work release
    //TODOTODO
        // Egal, ob das rollback durchging oder nicht, wir heben die db-verbindung auf.
        // Allerdings waere zu ueberlegen, ob eine spezielle meldung ausgegeben wird,
        // wenn das rollback schiefgeht.
        rc = errOK_els00;
    }

    if (true == DBInfo->dbiSession_ls00.siDBConnection_ls00)
    {
        ls03SQLRelease (DBInfo);    // release db connection
        rc = errOK_els00;
    }

    return rc;
}
// ls15CommitUser()



/*
  -----------------------------------------------------------------------------
  function:     ls15SQLGetUser
  -----------------------------------------------------------------------------
*/
int ls15SQLGetUser(tls00_SessionInfo* Session, tsp00_Addr pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls15SQLGetUser");

    tsp00_Int4         rc         = errSqlConnectUserkey_els98; // Initialize to error
    tsp00_Bool         ErrFlag;
    tsp00_ErrText      pszMyText;
    tsp00_Addr         pszUK      = Session->siUserKey_ls00;
    tsp4_xuser_record *UsrRecord  = new tsp4_xuser_record;

    size_t             lUKLength  = strlen(pszUK);
    pszMyText.SetBlank();

    // Initialize UsrRecord
    if (lUKLength >= sizeof (tsp4_xuserkey))
    {
        SAPDB_memcpy(&UsrRecord->xu_key[0], pszUK, sizeof (tsp4_xuserkey));
    }
    else
    {
        memset(&UsrRecord->xu_key[0], BLANK_LS00, sizeof (tsp4_xuserkey));
        SAPDB_memcpy(&UsrRecord->xu_key[0], pszUK, lUKLength);
    }

    // Get user information and store it in session structure.
    sqlgetuser(UsrRecord, NULL, pszMyText, &ErrFlag);
    if (0 != ErrFlag)
    {
        Session->siDBName_ls00.Init();
        Session->siDBName_ls00.p2c(UsrRecord->xu_serverdb);

        tsp00_Int4 len = UsrRecord->xu_servernode.length();
        Session->si_pDBNode_ls00 = new char[len + 1];
        SAPDB_memcpy(Session->si_pDBNode_ls00, UsrRecord->xu_servernode.asCharp(), len);
        Session->si_pDBNode_ls00[len] = 0;

        Session->siUserName_ls00.Init();
        Session->siUserName_ls00.rawAssign(UsrRecord->xu_user_61);

        SAPDB_memcpy(Session->siUserCPwd_ls00.asCharp(), &UsrRecord->xu_password[0], sizeof (tsp00_CryptPw));

        rc = errOK_els00;
    }
    else
    {   // make error text null terminated string
        strncpy(pszErrText, pszMyText.asCharp(), sizeof(tsp00_ErrText));
        pszErrText[sizeof(tsp00_ErrText)] = '\0';
        ls05StrTrimRightEx(pszErrText);
    }

    delete UsrRecord;

    return rc;
}
//  ls15SQLGetUser()


/*
  -----------------------------------------------------------------------------
  function:     ls15RSCommit
  -----------------------------------------------------------------------------
*/
tsp00_Int4 ls15RSCommit(tls00_DBInfo *DBInfo, tsp00_Addr ErrText, SAPDB_Int4 lPcktNo)
{
    ROUTINE_DBG_MEO00 ("ls15RSCommit");

    tsp00_Int4  rc = errOK_els00;

    if (true == DBInfo->dbiSession_ls00.siSqlStat_ls00)
    {
        if (true == DBInfo->dbiSession_ls00.siAutoCommit_ls00)
            rc = ls15Commit(DBInfo, ErrText, lPcktNo);
    }
    // No db connection no commit

    return rc;
}
//  ls15RSCommit()



/*
  -----------------------------------------------------------------------------
  function:     ls15Commit
  -----------------------------------------------------------------------------
*/
tsp00_Int4 ls15Commit(tls00_DBInfo *DBInfo, tsp00_Addr ErrText, SAPDB_Int4 lPcktNo)
{
    ROUTINE_DBG_MEO00 ("ls15Commit");

    tsp00_Int4  rc        = errOK_els00;
    tsp00_Int4  ErrPos    = 0;
    char        szCmd[64] = "COMMIT WORK";

    rc = ls13ExecIntSQLStmt(DBInfo, lPcktNo, szCmd, ErrText, ErrPos);
    if (errOK_els00 != rc)
    {
        // Here we get already an error text that is only to be printed. We'll use
        // the errSimpleSQL_els98-message to do it.
        LoaderMessages::Instance()->ls98PrintMessage(layBasicConnect_els98,
                                                      errSimpleSQL_els98,
                                                      ErrText);
    }

    return rc;
}
//  ls15Commit()


/*
  -----------------------------------------------------------------------------
  function:     ls15Rollback
  -----------------------------------------------------------------------------
*/
tsp00_Int4 ls15Rollback(tls00_DBInfo *DBInfo, tsp00_Addr ErrText, bool bRelease)
{
    ROUTINE_DBG_MEO00 ("ls15Rollback");

    tsp00_Int4  rc      = errOK_els00;
    tsp00_Int4  ErrPos  = 0;
    char        szCmd[64] = "ROLLBACK WORK";

    if (true == bRelease)
    {
        szCmd[0] = '\0';
        strcpy(szCmd, "ROLLBACK WORK RELEASE");
    }

    rc = ls13ExecIntSQLStmt(DBInfo, 0, szCmd, ErrText, ErrPos);
    if (errOK_els00 != rc)
    {
        // Here we get already an error text that is only to be printed. We'll use
        // the errSimpleSQL_els98-message to do it.
        LoaderMessages::Instance()->ls98PrintMessage(layBasicConnect_els98,
                                                      errSimpleSQL_els98,
                                                      ErrText);
    }

    return rc;
}
//  ls15Rollback()
/*
  -----------------------------------------------------------------------------
  function:     ls15SwitchDB
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls15SwitchDB(tls00_DBInfo* pDBInfo, tsp00_Addr pszErrText )
{
    ROUTINE_DBG_MEO00 ("ls15SwitchDB");

    LoaderMessages *pMsgInstance = LoaderMessages::Instance();

    tsp00_Int4 rc = errOK_els00;

    // Connect Attributes
    SAPDB_Char szDriver[16] = "SAP DB";

    //ServerNode
    if (NULL == pDBInfo->dbiSession_ls00.si_pDBNode_ls00)
    {
        tsp00_NodeId tmpDBHost_p;
        tmpDBHost_p.SetBlank();
        sqlhostname(tmpDBHost_p, pDBInfo->dbiSqlErr_ls00, pDBInfo->dbiSqlRc_ls00);
        if (0 != pDBInfo->dbiSqlRc_ls00)
            rc = errDbHost_els98;
        else
        {
            tsp00_NodeIdc tmpDBHost_c;
            tsp00_Int4    tmpDBHostLen = 0;
            tmpDBHost_c.Init();
            tmpDBHost_c.p2c(tmpDBHost_p);

            tmpDBHostLen = tmpDBHost_c.length();
            pDBInfo->dbiSession_ls00.si_pDBNode_ls00 = new char[tmpDBHostLen + 1];
            SAPDB_memcpy(pDBInfo->dbiSession_ls00.si_pDBNode_ls00, tmpDBHost_c.asCharp(), tmpDBHostLen);
            pDBInfo->dbiSession_ls00.si_pDBNode_ls00[tmpDBHostLen] = 0;
        }
    }
    // SQLSetConnectAttr

    // codepage support
    if ( pDBInfo->dbiSession_ls00.siCodeType_ls00 == ctAscii_els00 )
         pDBInfo->dbiOdbcConnection.AddAttr( SQL_ATTR_CODEPAGE, REINTERPRET_CAST( SQLPOINTER, pDBInfo->dbiSession_ls00.siCodePage_ls00.name.asCharp()), SQL_NTS );
    // privileges to execute used commands
    pDBInfo->dbiOdbcConnection.AddAttr( SQL_PRODUCER,  REINTERPRET_CAST( SQLPOINTER, sp1pr_internal_cmd ),SQL_IS_POINTER );
    pDBInfo->dbiOdbcConnection.AddAttr( SQL_COMPONENT, REINTERPRET_CAST( SQLPOINTER, sql_csp_comp_load  ),SQL_IS_POINTER );
    pDBInfo->dbiOdbcConnection.AddAttr( SQL_ATTR_SESSION_TIMEOUT, 0, SQL_IS_UINTEGER);

    // mapping of oi tsp1_sqlmode_Enum to odbc tpr00_SQLModeEnum
    switch(pDBInfo->dbiSession_ls00.siSQLMode_ls00)
    {
        case sp1sm_db2:
        {
            pDBInfo->dbiOdbcConnection.AddAttr( SQL_SQLMODE, REINTERPRET_CAST( SQLPOINTER, cpr_kind_db2 ),SQL_IS_POINTER);
            break;
        }
        case sp1sm_ansi:
        {
            pDBInfo->dbiOdbcConnection.AddAttr( SQL_SQLMODE, REINTERPRET_CAST( SQLPOINTER, cpr_kind_ansi ),SQL_IS_POINTER);
            break;
        }
        case sp1sm_oracle:
        {
            pDBInfo->dbiOdbcConnection.AddAttr( SQL_SQLMODE, REINTERPRET_CAST( SQLPOINTER, sp1sm_oracle ),SQL_IS_POINTER);
            break;
        }
        case sp1sm_internal:
        default:
        {
            pDBInfo->dbiOdbcConnection.AddAttr( SQL_SQLMODE, REINTERPRET_CAST( SQLPOINTER, cpr_kind_internal),SQL_IS_POINTER );
        }
    }
    switch(pDBInfo->dbiSession_ls00.siDBSessionIsoLevel_ls00)
    {
        case 0:
        {
            pDBInfo->dbiOdbcConnection.AddAttr(SQL_TXN_ISOLATION,
                                               REINTERPRET_CAST(SQLPOINTER, SQL_TXN_READ_UNCOMMITTED),
                                               SQL_IS_POINTER);
            break;
        }
        case 1:
        case 10:
        case 15:
        {
            pDBInfo->dbiOdbcConnection.AddAttr(SQL_TXN_ISOLATION,
                                               REINTERPRET_CAST(SQLPOINTER, SQL_TXN_READ_COMMITTED),
                                               SQL_IS_POINTER);
            break;
        }
        case 2:
        case 20:
        {
            pDBInfo->dbiOdbcConnection.AddAttr(SQL_TXN_ISOLATION,
                                               REINTERPRET_CAST(SQLPOINTER, SQL_TXN_REPEATABLE_READ),
                                               SQL_IS_POINTER);
            break;
        }
        default:        // case 3
        {
            pDBInfo->dbiOdbcConnection.AddAttr(SQL_TXN_ISOLATION,
                                               REINTERPRET_CAST(SQLPOINTER, SQL_TXN_READ_UNCOMMITTED),
                                               SQL_IS_POINTER);
        }
    } 

    //TraceFile
    tsp00_Pathc     siTraceFilePathNamec;
    SAPDB_Char      _szTraceFileName[2*PATH_MXSP00];
    SAPDB_Char      _szTraceFilePathName[PATH_MXSP00];
    tsp01_RteError  RteError;

    sqlGetIndependentWrkPath(siTraceFilePathNamec, TERM_WITH_DELIMITER_EO01, &RteError);
    
    _szTraceFileName[0]     = 0;
    _szTraceFilePathName[0] = 0;

    if ( NULL != getenv ("maxdbloaderserver") ) {
         sqlGetIndependentWrkPath(siTraceFilePathNamec, TERM_WITH_DELIMITER_EO01, &RteError);
         strcpy(_szTraceFilePathName, siTraceFilePathNamec.asCharp());
    }
    else {
        pMsgInstance->ls98GetLoaderLogPath(_szTraceFilePathName);
    }
    if (true == pDBInfo->dbiSession_ls00.siODBCTrace_ls00)
        sp77sprintf(_szTraceFileName, 2*PATH_MXSP00, "%sloader", _szTraceFilePathName);

    //Connection
    if ( !pDBInfo->dbiOdbcConnectionPool->ConnectWithServerDB(pDBInfo->dbiOdbcConnection,
                                                              "",
                                                              szDriver,
                                                              pDBInfo->dbiSession_ls00.si_pDBNode_ls00,
                                                              pDBInfo->dbiSession_ls00.siDBName_ls00.asCharp(),
                                                              pDBInfo->dbiSession_ls00.siUserName_ls00.asCharp(),
                                                              pDBInfo->dbiSession_ls00.siUserPwd_ls00.asCharp(),
                                                              _szTraceFileName,
                                                              0,0, sapdbwa_DBC_SHARED) )
    {
        sapdbwa_Err        Err    = pDBInfo->dbiOdbcConnection.GetErr();
        const SAPDB_Char*  pszErr = Err.GetErrMsg();

        TRACE_PRNF_MLS99(("ls15SwitchDB", "failed: %s\n",  pszErr ));
        pMsgInstance->ls98Msg(laySQLConnect_els98, pszErrText, errODBCErrors_els98, pszErr );

        rc = Err.GetNativeOdbcErr();
    }
    return rc;
}

/*
  -----------------------------------------------------------------------------
  function:     ls15SwitchUser
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls15SwitchUser(tls00_DBInfo            *pDBInfo,
               Tools_DynamicUTF8String  UserName,
               tsp00_Addr               ErrText)
{
    ROUTINE_DBG_MEO00 ("ls15SwitchUser");
    tsp00_Int4 rc = errOK_els00;

    _TCHAR szCmd[256];
    szCmd[0] = 0;
    sp77sprintf(szCmd, 256, "CURRENT USER \"%s\"", UserName.StrPtr());

     tsp00_Int4 lErrPos;
    _TCHAR      szErr[MAX_REPLY_LENGTH_LS00];
    rc = ls13ExecIntSQLStmt(pDBInfo, 0, szCmd, szErr, lErrPos);
    if (errOK_els00 != rc)
    {
        LoaderMessages::Instance()->ls98Msg(layTableExtract_els98, ErrText, errCurrentUserCmd_els98, UserName.StrPtr(), szErr);
        rc = errCurrentUserCmd_els98;
    }
    return rc;
}
// ls15SwitchUser()

/*
  -----------------------------------------------------------------------------
  function:     ls15SwitchUserSchema
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls15SwitchUserSchema(tls00_DBInfo           *pDBInfo,
                     Tools_DynamicUTF8String username,
                     Tools_DynamicUTF8String schemaname,
                     tsp00_Addr              ErrText)
{
    ROUTINE_DBG_MEO00 ("ls15SwitchUserSchema");
    tsp00_Int4 rc = errOK_els00;

    Tools_DynamicUTF8String statement;
    Tools_DynamicUTF8String sel_timeout_stmt(SELECT_SESSIONTIMEOUT_76_LS15);
    StudioOAL_String        record[1];
    tsp00_Int4              ErrPos;
    _TCHAR                  _ErrText[MAX_REPLY_LENGTH_LS00];
    _ErrText[0] = 0;

    ConnectionDef * pConnectionDef  = new ConnectionDef( pDBInfo->dbiOdbcConnection );
    tsp100_VersionResult vres = ls15CheckKernelVersion(pDBInfo, 7, 5, 0,0);

    if ( IsEqual_esp100 == vres || IsNewer_esp100 == vres )
    {
           SAPDB_Int4 build = 0;
           statement = "SELECT BUILD FROM SYSINFO.VERSION ";
           if( pConnectionDef != NULL &&
               pConnectionDef->executeSQL(statement) &&
               pConnectionDef->readResult( record  ) )
               build = AtoI( record[0] );
           pDBInfo->dbiDBVersion_ls00.BuildNumberPrefix_sp100 = build;
    }

    vres = ls15CheckKernelVersion(pDBInfo, 7, 6, 0,0);
    if ( IsEqual_esp100 == vres || IsNewer_esp100 == vres )
    {
        StudioOAL_String  where;

        if ( schemaname.Length() != 0 && 
             schemaname.Find ("SYS", 3) == StudioTD_String::NPos  )
        {
            SAPDB_Int4 exists = 0;

            statement = "SELECT 1 FROM DOMAIN.SCHEMAPRIVILEGES ";
            where     = " WHERE CREATEIN IN ('YES') AND GRANTEE IN ('PUBLIC','";
            where    += username;
            where    += "') AND SCHEMANAME = '";
            where    += schemaname;
            where    += "'";
            statement+= where;
            
            if( pConnectionDef != NULL &&
                pConnectionDef->executeSQL(statement) &&
                pConnectionDef->readResult( record  ) )
            {
                exists = AtoI( record[0] );
                if( exists == 1 )
                {
                    if ( !username.Empty() )
                    {
                        statement  = "CURRENT USER ";
                        statement += "\"";
                        statement += username;
                        statement += "\"";
                        pConnectionDef->executeSQL(statement);
                        rc = ls13ExecIntSQLStmt(pDBInfo, 0, CONST_CAST(SAPDB_Char*, statement.CharPtr()), ErrText, ErrPos);
                    }
                    if ( errOK_els00 == rc )
                    {
                        statement  = "SET CURRENT_SCHEMA=";
                        statement += "\"";
                        statement += schemaname;
                        statement += "\"";
                        pConnectionDef->executeSQL(statement);
                        rc = ls13ExecIntSQLStmt(pDBInfo, 0, CONST_CAST(SAPDB_Char*, statement.CharPtr()), ErrText, ErrPos);
                    }
                }// if( exists == 1 )
                else
                {
                    LoaderMessages::Instance()->ls98Msg(layTableExtract_els98, ErrText, errCurrentSchemaCmd_els98,
                                                                                        schemaname.StrPtr(), _ErrText);
                    rc = errCurrentSchemaCmd_els98;
                }
            }// if pConnectionDef->readResult( record  )
            else
            {
                LoaderMessages::Instance()->ls98Msg(layTableExtract_els98, ErrText, errCurrentSchemaCmd_els98, schemaname.StrPtr(), _ErrText);
                rc = errCurrentSchemaCmd_els98;
            }
        }
    }
    else
    {
        sel_timeout_stmt = Tools_DynamicUTF8String(SELECT_SESSIONTIMEOUT_75_LS15);

       if ( !username.Empty()  && 
             username.Length() != 0  && 
             username.Find ("SYS", 3) == StudioTD_String::NPos )
       {
            statement  = "CURRENT USER ";
            statement += "\"";
            statement += username;
            statement += "\"";
            pConnectionDef->executeSQL(statement);
            rc = ls13ExecIntSQLStmt(pDBInfo, 0, CONST_CAST(SAPDB_Char*, statement.CharPtr()), ErrText, ErrPos);
       }
    }
    //*
    //*     Set SESSION TIMEOUT values for reconnect
    //*
    if ( NULL != pConnectionDef && pConnectionDef->executeSQL(sel_timeout_stmt))
    {
        pConnectionDef->readResult(record);
        pDBInfo->dbiSession_ls00.siReconnect_ls00.rcoTimeOut_ls00 = atoi( REINTERPRET_CAST(char const*, record[0].StrPtr()));
    }
    else
    {
        pDBInfo->dbiSession_ls00.siReconnect_ls00.rcoTimeOut_ls00 = 900;
    }
    //*
    //*     Adjust timeout values if not set to 0; 0 means NO TIMEOUT
    //*
    if (0 == pDBInfo->dbiSession_ls00.siReconnect_ls00.rcoTimeOut_ls00)
    {
        pDBInfo->dbiSession_ls00.siReconnect_ls00.rcoThreshold_ls00 = 0;
    }
    else
    {
        // Adjust the value to microseconds as the timer works with microseconds
        pDBInfo->dbiSession_ls00.siReconnect_ls00.rcoTimeOut_ls00   *= 1000000;
        // The threshold that defines the time to refresh a connection is 90% of the timeout
        pDBInfo->dbiSession_ls00.siReconnect_ls00.rcoThreshold_ls00  =
            pDBInfo->dbiSession_ls00.siReconnect_ls00.rcoTimeOut_ls00/10 * 9;
    }

    //*
    //*     In case of reconnect we have to adjust some db parameters, too.
    //*     - DATE_TIME_FORMAT: Alterable default format of date, time and timestamp
    //*                         values for non-JDBC and non-ODBC applications
    //*
    if (SAPDB_TRUE == pDBInfo->dbiSession_ls00.siReconnect_ls00.rcoDoReconnect_ls00)
    {
        if ( (pDBInfo->dbiSession_ls00.siFormat_ls00 >= dtISO_els00) &&
             (dtInternal_els00 != pDBInfo->dbiSession_ls00.siFormat_ls00) )
        {
            char szCmd[MAX_CONNECT_CMD_LENGTH_LS15];
            szCmd[0] = 0;
            sp77sprintf(szCmd, MAX_CONNECT_CMD_LENGTH_LS15, "SET FORMAT %s ",
                                                            pszDateTimeName_ls00[pDBInfo->dbiSession_ls00.siFormat_ls00]);
            rc = ls13ExecIntSQLStmt(pDBInfo, 0, szCmd, _ErrText, ErrPos);
            if (errOK_els00 == rc)
            {
                pDBInfo->dbiSession_ls00.siReconnect_ls00.rcoDoReconnect_ls00 = SAPDB_FALSE;
            }
            else
            {
                LoaderMessages::Instance()->ls98Msg(layBasicConnect_els98, ErrText, errSimpleSQL_els98, _ErrText);
                rc = errSimpleSQL_els98;
            }
        }
    }   // end if (SAPDB_TRUE == pDBInfo->dbiSession_ls00.siReconnect_ls00.rcoDoReconnect_ls00)


    if (pConnectionDef != NULL )
    {
        delete pConnectionDef;
        pConnectionDef = NULL;
    }
    return rc;
}
// ls15SwitchUserSchema()


/*
  -----------------------------------------------------------------------------
  function:     ls15SwitchSchema
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls15SwitchSchema(tls00_DBInfo           *pDBInfo,
                 Tools_DynamicUTF8String schemaname,
                 tsp00_Addr              ErrText)
{
    ROUTINE_DBG_MEO00 ("ls15SwitchSchema");

    tsp00_Int4 rc = errOK_els00;

    tsp100_VersionResult vres = ls15CheckKernelVersion(pDBInfo, 7, 6, 0, 0);

    if ( (0 == schemaname.Length()) || (IsOlder_esp100 == vres))
    {
        // No schema to switch to in versions < 7.6
        return errOK_els00;
    }

    tsp00_Int4              lErrPos;
    
    Tools_DynamicUTF8String statement("SET CURRENT_SCHEMA=");
    ConnectionDef*          pConnectionDef  = NULL;
    statement += "\"";
    statement += schemaname;
    statement += "\"";

    //*
    //* Change schema on ODBC connection only if it is not the installation of system tables
    //*
    if( NULL == getenv ("maxdbloaderserver") )
    {
        pConnectionDef = new ConnectionDef( pDBInfo->dbiOdbcConnection );
        pConnectionDef->executeSQL(statement);
    }
    // we don't check for errors here as the next command will fail in this case surely too; the next
    // will then report the error to the caller

    //*
    //* Change schema on OI connection
    //*
    rc = ls13ExecIntSQLStmt(pDBInfo, 0, CONST_CAST(SAPDB_Char*, statement.CharPtr()), ErrText, lErrPos);
    if (errOK_els00 != rc)
    {
        LoaderMessages::Instance()->ls98Msg(layTableExtract_els98, ErrText, errCurrentSchemaCmd_els98,
                                                                            schemaname.StrPtr(), ErrText);
        rc = errCurrentSchemaCmd_els98;
    }

    if (pConnectionDef != NULL )
    {
        delete pConnectionDef;
        pConnectionDef = NULL;
    }
    return rc;
}
// ls15SwitchSchema()


/*
  -----------------------------------------------------------------------------
  function:     ls15SwitchFormat
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls15SwitchFormat(tls00_DBInfo*   pDBInfo,
                 tls00_DateTime  newFormat,
                 tsp00_Addr      pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls15SwitchFormat");

    tsp00_Int4 rc = errOK_els00;

    SAPDB_Int4 lErrPos = 0;
    _TCHAR     _ErrText[MAX_REPLY_LENGTH_LS00];
    _ErrText[0] = 0;

    Tools_DynamicUTF8String statement("SET FORMAT ");
    statement += pszDateTimeName_ls00[newFormat];

    rc = ls13ExecIntSQLStmt(pDBInfo, 0, CONST_CAST(SAPDB_Char*, statement.CharPtr()), _ErrText, lErrPos);
    if (errOK_els00 != rc)
    {
        LoaderMessages::Instance()->ls98Msg(layBasicConnect_els98, pszErrText, errSimpleSQL_els98, _ErrText);
        rc = errSimpleSQL_els98;
    }
    return rc;
}
// ls15SwitchFormat()


/*
  -----------------------------------------------------------------------------
  function: ls15_UserConnect_MD5
  -----------------------------------------------------------------------------
*/
static SAPDB_Int4
ls15_UserConnect_MD5(tls00_DBInfo* pDBInfo, tsp00_Addr pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls15_UserConnect_MD5");

    SAPDB_Int4 rc  = errOK_els00;

    tls00_SessionInfo*  pSession  = &pDBInfo->dbiSession_ls00;
    SAPDB_Int4          lDataLen  = MAX_CONNECT_CMD_LENGTH_LS15;        // Used to initialize _DataWriter
    char                szCmd[MAX_CONNECT_CMD_LENGTH_LS15];
    char                szData[MAX_CONNECT_CMD_LENGTH_LS15];
    
    szCmd[0]  = 0;
    szData[0] = 0;

    //*
    //*     Check if user name must be converted to UCS2
    //*
    tls00_String  _UserName;
    _UserName.strAddr_ls00 = new char[2*sizeof(tsp00_KnlIdentifierc)];
    _UserName.strLeng_ls00 = pSession->siUserName_ls00.length();        // Initialize to ASCII name

    if ( false == ls00IsDBUnicode(pDBInfo) )
    {   // nothing to do than to copy the user name
        SAPDB_memcpy(_UserName.strAddr_ls00, pSession->siUserName_ls00.asCharp(), _UserName.strLeng_ls00);
        _UserName.strAddr_ls00[_UserName.strLeng_ls00] = 0;
    }
    else
    {
        tsp00_Longuint _len = 2 * sizeof(tsp00_KnlIdentifierc);

        rc = ls05ConvertToUCS2Simple(REINTERPRET_CAST(unsigned char*, _UserName.strAddr_ls00),
                                     _len,
                                     REINTERPRET_CAST(unsigned char*, pSession->siUserName_ls00.asCharp()),
                                     _UserName.strLeng_ls00,
                                     ctAscii_els00,
                                     pszErrText);
        _UserName.strLeng_ls00 = STATIC_CAST(SAPDB_Int4, _len);
    }


    //*
    //*     Check if user password must be converted to UCS2
    //*
    tls00_String  _Password;
    if (errOK_els00 == rc)
    {
        _Password.strAddr_ls00 = new char[2 * sizeof(pSession->siUserPwd_ls00)];
        _Password.strLeng_ls00 = pSession->siUserPwd_ls00.length();     // Initialize to ASCII password

        if ( false == ls00IsDBUnicode(pDBInfo) )
        {   // nothing to do than to copy the password
            SAPDB_memcpy(_Password.strAddr_ls00, pSession->siUserPwd_ls00.asCharp(), _Password.strLeng_ls00);
            _Password.strAddr_ls00[_Password.strLeng_ls00] = 0;
        }
        else
        {
            tsp00_Longuint _len = 2 * sizeof(pSession->siUserPwd_ls00);

            rc = ls05ConvertToUCS2Simple(REINTERPRET_CAST(unsigned char*, _Password.strAddr_ls00),
                                        _len,
                                        REINTERPRET_CAST(unsigned char*, pSession->siUserPwd_ls00.asCharp()),
                                        _Password.strLeng_ls00,
                                        ctAscii_els00,
                                        pszErrText);
            _Password.strLeng_ls00 = STATIC_CAST(SAPDB_Int4, _len);
        }
    }   // end if (errOK_els00 == rc) for UCS2 conversion routines

    //*
    //*     Create salty key for password encrypting at connect
    //*
    if (errOK_els00 == rc)
    {
        RTESec_ClientAuthenticationBuffer  _stateBuffer;                       // holds state between REQUEST and RESPONSE
        const RTESec_Authentication*       _pAuthentication = NULL;            // authentication algorithm

        RTESec_Authentication::Iterator    _Iter;
        const void*                        _RequestData     = NULL;
        SAPDB_Int4                         _lRequestLen     = 0;
        SAPDB_Int4                         _lFieldCnt       = 0;
        SAPDB_Int4                         _lErrorPos       = 0;

        char*                              _pRawPartPtr     = NULL;
        SAPDB_Int4                         _lRawPartPtrLen  = 0;

        {
            // Write data in right form into data buffer
            SAPDBFields_VarData::Writer  _DataWriter(szData, MAX_CONNECT_CMD_LENGTH_LS15);
            while (_Iter.next(_pAuthentication))
            {
                _DataWriter.addField(_pAuthentication->Name(), _tcslen(_pAuthentication->Name()));
                _pAuthentication->DataForChallengeRequest(_stateBuffer,
                                                        _UserName.strAddr_ls00, _UserName.strLeng_ls00,
                                                        _RequestData, _lRequestLen);
                _DataWriter.addField(_RequestData, _lRequestLen);
            }
            _DataWriter.close(_lFieldCnt, lDataLen);                                // returns length of written data
        }
        _RequestData = NULL;                                                    // reinitialize


        //*
        //*     Command to get authentication algorithm
        //*
        sp77sprintf(szCmd, MAX_CONNECT_CMD_LENGTH_LS15, CONNECT_STMT_AUTH_LS15, pSession->siUserName_ls00.asCharp());

        //*
        //*     Check if kernel is willing to use this kind of authentication
        //* In case it is willing it responds with the challenge that must be used to crypt the password
        //*
        rc = ls13ExecIntSQLStmtEx(pDBInfo, 0, szCmd, szData, STATIC_CAST(tsp00_Int4, lDataLen), pszErrText, _lErrorPos);
        if (errOK_els00 == rc)
        {
            rc = ls04GetRawDataPtr(pDBInfo->dbiPktRcvList_ls00[0],
                                   _pRawPartPtr,
                                   _lRawPartPtrLen,
                                   STATIC_CAST(tsp1_part_kind_Enum, sp1pk_vardata));
            if (errOK_els00 == rc)
            {
                SAPDBFields_VarData::Reader  _DataReader(_pRawPartPtr);
                const void*                  _pAlgoName     = NULL;      // from field0 of CHALLENGE vardata part
                SAPDB_Int4                   _lAlgoNameLen = 0;         // from field0 of CHALLENGE vardata part
                const void*                  _Response;
                SAPDB_Int4                   _lResponseLen;
                SAPDB_Int4                   _lNotUsed     = 0;
                bool                         _bValid       = false;


                _bValid = _DataReader.next(_pAlgoName, _lAlgoNameLen, _lNotUsed);
                if (true == _bValid)
                {
                    _bValid = _DataReader.next(_RequestData, _lRequestLen, _lNotUsed);
                }

                _pAuthentication = RTESec_Authentication::FindByName (STATIC_CAST(const char*, _pAlgoName), _lAlgoNameLen);

                _pAuthentication->CreateResponse(_stateBuffer,
                                                 _Response, _lResponseLen,   // OUTPUT
                                                 _RequestData, _lRequestLen, // INPUT
                                                 _Password.strAddr_ls00, _Password.strLeng_ls00);// INPUT
                {
                    szData[0] = 0;
                    SAPDBFields_VarData::Writer  _DataWriter(szData, MAX_CONNECT_CMD_LENGTH_LS15);

                    _DataWriter.addField(_pAuthentication->Name(), _tcslen(_pAuthentication->Name()));
                    _DataWriter.addField(_Response, _lResponseLen);
                    _DataWriter.close(_lFieldCnt, lDataLen);
                }


                //*
                //*     At last process the CONNECT command
                //*
                szCmd[0] = 0;
                sp77sprintf(szCmd, MAX_CONNECT_CMD_LENGTH_LS15, CONNECT_STMT_LS15, pSession->siUserName_ls00.asCharp(),
                                                                                   pSession->siDBSessionIsoLevel_ls00);

                rc = ls13ExecIntSQLStmtEx(pDBInfo, 0, szCmd, szData, STATIC_CAST(tsp00_Int4, lDataLen), pszErrText, _lErrorPos, STATIC_CAST(tsp1_part_kind_Enum, sp1pk_vardata));

            }   // end else of if (errOK_els00 == rc ls04GetRawDataPtr(pDBInfo->dbiPktRcvList_ls00[0], ...

        }   // end else of if (errOK_els00 == rc = ls13ExecIntSQLStmtEx(pDBInfo, 0, szCmd, szData,...
        
    }   // end if (errOK_els00 == rc)


    if (NULL != _UserName.strAddr_ls00)
    {
        delete [] _UserName.strAddr_ls00;
        _UserName.strAddr_ls00 = NULL;
    }
    if (NULL != _Password.strAddr_ls00)
    {
        delete [] _Password.strAddr_ls00;
        _Password.strAddr_ls00 = NULL;
    }

    return rc;
}
// ls15_UserConnect_MD5() 

/*
  -----------------------------------------------------------------------------
  function: ls15_UserConnect_Encrypt
  -----------------------------------------------------------------------------
*/
static SAPDB_Int4
ls15_UserConnect_Encrypt(tls00_DBInfo* pDBInfo, tsp00_Addr pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls15_UserConnect_Encrypt");

    SAPDB_Int4 rc  = errOK_els00;

    tls00_SessionInfo*  pSession  = &pDBInfo->dbiSession_ls00;
    SAPDB_Int4          lDataLen  = MAX_CONNECT_CMD_LENGTH_LS15;        // Used to initialize _DataWriter
    tsp00_Int4          CPwdLen   = sizeof(tsp00_CryptPw);            // length of crypted pw stays the same for ascii and unicode
    char                szCmd[MAX_CONNECT_CMD_LENGTH_LS15];
    char                szData[MAX_CONNECT_CMD_LENGTH_LS15];
    
    //*
    //*     Connect cmd
    //*
    sp77sprintf(szCmd, MAX_CONNECT_CMD_LENGTH_LS15, CONNECT_STMT_LS15, pSession->siUserName_ls00.asCharp(),
                                                                       pSession->siDBSessionIsoLevel_ls00);
    if (true == pSession->siNolog_ls00)
        strcat(szCmd, " NOLOG");

    //*
    //* Build data string depending on encoding of db
    //*
    //* Data string looks like:
    //* ------------------------------------------------------------------------------
    //* def-byte |      crypted pw     | def-byte |            term-id               |
    //*-------------------------------------------------------------------------------
    //*

    if ( false == ls00IsDBUnicode(pDBInfo) )
    {
        // def-byte in front of pw and termid
        szData[0] = szData[sizeof(DEF_BYTE_ASCII_LS00) + CPwdLen] = DEF_BYTE_ASCII_LS00;
        lDataLen = sizeof(DEF_BYTE_ASCII_LS00);

        // put pw in
        SAPDB_memcpy(&szData[lDataLen], pSession->siUserCPwd_ls00.asCharp(), CPwdLen);
        lDataLen += sizeof(DEF_BYTE_ASCII_LS00) + CPwdLen;

        // put termid in
        SAPDB_memcpy(&szData[lDataLen], pDBInfo->dbiSqlTerm_ls00.asCharp(), TERMID_MXSP00);
        lDataLen += TERMID_MXSP00;
    }
    else
    {
        // def-byte in front of pw and termid
        szData[0] = szData[sizeof(DEF_BYTE_UNICODE_LS00) + CPwdLen] = DEF_BYTE_UNICODE_LS00;
        lDataLen  = sizeof(DEF_BYTE_UNICODE_LS00);

        // put pw in
        // in case of unicode pw needs to be stored as pascal string (unused chars filled with blank);
        // pascal string must be stored as UTF8, then converted to UCS2 and the crypted (uuuh)
        tsp00_Pw pszPwdP;
        pszPwdP.SetBlank();
        pszPwdP.c2p(pSession->siUserPwd_ls00.asCharp());

        tsp00_Pw       szUCS2Pwd;
        tsp00_Longuint lPWLen    = PW_MXSP00;

        rc = ls05ConvertToUCS2Simple(REINTERPRET_CAST(unsigned char*, szUCS2Pwd.asCharp()),
                                     lPWLen,
                                     REINTERPRET_CAST(unsigned char*, pszPwdP.asCharp()),
                                     PW_MXSP00/2,
                                     ctAscii_els00,
                                     pszErrText);
        if (errOK_els00 == rc)
        {
            ls15ApplEncrypt(szUCS2Pwd, pSession->siUserCPwd_ls00);

            SAPDB_memcpy(&szData[lDataLen], pSession->siUserCPwd_ls00.asCharp(), CPwdLen);
            lDataLen += sizeof(DEF_BYTE_UNICODE_LS00) + CPwdLen;

            // put termid in
            tsp00_Longuint lTermIDLen = 2*TERMID_MXSP00;

            rc = ls05ConvertToUCS2Simple(REINTERPRET_CAST(unsigned char*, &szData[lDataLen]),
                                         lTermIDLen,
                                         REINTERPRET_CAST(unsigned char*, pDBInfo->dbiSqlTerm_ls00.asCharp()),
                                         TERMID_MXSP00,
                                         ctAscii_els00,
                                         pszErrText);
            if (errOK_els00 == rc)      // function returns length on success in lTermIDLen
            {
                lDataLen += lTermIDLen;
            }
        }
        // convert function ls05ConvertToUCS2Simple returns length on success; in case of failure
        // the function logs the message and returns an error code

    }   // end else branch of if ( false == ls00IsDBUnicode(pDBInfo) )

    if (errOK_els00 == rc)
    {
        SAPDB_Int4 lErrorPos = 0;

        rc = ls13ExecIntSQLStmtEx(pDBInfo, 0, szCmd, szData, STATIC_CAST(tsp00_Int4, lDataLen), pszErrText, lErrorPos);
    }
    return rc;
}
// ls15_UserConnect_Encrypt()

/*
  -----------------------------------------------------------------------------
  function: ls15CheckUserMode
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls15CheckUserMode(tls00_DBInfo            *pDBInfo,
                  Tools_DynamicUTF8String  UserName,
                  tls00_DBUserMode         UserMode,
                  tsp00_Addr               pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls15CheckUserMode");
    tsp00_Int4 rc = errOK_els00;

    assert(dbuUnknown_els00 != UserMode);

    _TCHAR szCmd[256];
    _TCHAR szUserMode[32];
    szUserMode[0] = 0;
    szCmd[0]      = 0;

    switch (UserMode)
    {
        case dbuSysdba_els00:
        {
            _tcscpy(szUserMode, _T("SYSDBA"));
            break;
        }
        case dbuDba_els00:
        {
            _tcscpy(szUserMode, _T("DBA"));
            break;
        }
        case dbuResource_els00:
        {
            _tcscpy(szUserMode, _T("RESOURCE"));
            break;
        }
        default:    // dbuColdUser_els00
        {
            _tcscpy(szUserMode, _T("COLDUSER"));
        }
    }
    sp77sprintf(szCmd, 256, "SELECT * FROM DOMAIN.USERS WHERE USERNAME = '%s' AND USERMODE = '%s'",
                            UserName.StrPtr(), szUserMode);

    tsp00_Int4 lErrPos = 0;
    _TCHAR     szErr[MAX_REPLY_LENGTH_LS00];

    rc = ls13ExecIntSQLStmt(pDBInfo, 0, szCmd, szErr, lErrPos);
    if (errOK_els00 != rc)
    {
        if (dberrRowNotFound_els98 != rc)
        {
            LoaderMessages::Instance()->ls98Msg(layTableExtract_els98, pszErrText,errInternSQL_els98, szCmd, szErr);
            rc = errInternSQL_els98;
        }
    }
    ls15Commit(pDBInfo, szErr);
    return rc;
}
// ls15CheckUserMode()

/*
  -----------------------------------------------------------------------------
  function:     ls15ConnectUser
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls15ConnectUser(tls00_DBInfo* pDBInfo, tsp00_Addr ErrText)
{
    ROUTINE_DBG_MEO00 ("ls15ConnectUser");

    tsp00_Longint rc  = errOK_els00;
    SAPDB_Bool rcBool = errOK_els00;

    tls00_SessionInfo *pSession           = &pDBInfo->dbiSession_ls00;

    if (0 == pSession->siUserName_ls00.length())
        return STATIC_CAST(tsp00_Int4, rc);

    TRACE_PRNF_MLS99(("ls15ConnectUser", "Connecting user: %s at %s.\n",
                                         pSession->siUserName_ls00.asCharp(),
                                         pSession->siDBName_ls00.asCharp()));

    //*
    //*  User connect - Order Interface
    //*
    if (true == pSession->siAuthentication_ls00)        // Use new password encryption/authentication
    {
        rc = ls15_UserConnect_MD5(pDBInfo, ErrText);
    }
    else
    {
        rc = ls15_UserConnect_Encrypt(pDBInfo, ErrText);
    }

    if (errOK_els00 == rc)
    {
        SAPDB_Char  szCmd[MAX_CONNECT_CMD_LENGTH_LS15];
        tsp00_Int4 lErrorPos = 0;

        //* function ls15_CheckConnectPacket may only return errInternal_els98; the text is
        //* logged to return string and log file
        rc = ls15_CheckConnectPacket(pDBInfo, 0, ErrText);
        //*
        //*   Flag a successful connection and set DATE_TIME_FORMAT to INTERNAL
        //*
        if (errOK_els00 == rc)
        {
            pDBInfo->dbiSession_ls00.siSqlStat_ls00 = true;

            if (dtf_normal != pDBInfo->dbiDateTimeFormat_ls00)
            {
                char szErr[MAX_REPLY_LENGTH_LS00];

                szErr[0] = 0;
                szCmd[0] = 0;
                sp77sprintf(szCmd, 1024, "%s", "SET FORMAT INTERNAL");

                rc = ls13ExecIntSQLStmt(pDBInfo, 0, szCmd, szErr, lErrorPos);
                if (errOK_els00 != rc)
                {
                    LoaderMessages::Instance()->ls98PrintMessage(layBasicConnect_els98, errInternSQL_els98, szCmd, szErr);
                    rc = errOK_els00;
                }
            }
            // Connect DB - Open Database Connection
            if( NULL == getenv ("maxdbloaderserver") )
            {
                rc = ls15SwitchDB(pDBInfo, ErrText);
                if( errOK_els00 == rc ) {
                    rc = ls15SwitchUserSchema(pDBInfo,
                                              REINTERPRET_CAST(SAPDB_UTF8*, pDBInfo->dbiSession_ls00.siUserName_ls00.asCharp()),
                                              REINTERPRET_CAST(SAPDB_UTF8*, pDBInfo->dbiSession_ls00.siSchemaName_ls00),
                                              ErrText);
                }
            }
        }
    }// end if (errOK_els00 == rc)

    TRACE_USER_MLS99("ls15ConnectUser", pDBInfo);

    return STATIC_CAST(tsp00_Int4, rc);
}
// ls15ConnectUser()

/*
  -----------------------------------------------------------------------------
  function:     ls15Connect
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls15Connect(tls00_DBInfo *pDBInfo, tsp00_Addr ErrText)
{
    ROUTINE_DBG_MEO00 ("ls15Connect");

    tsp00_Int4  rc = errOK_els00;

    // Connect DB - Order Interface
    rc = ls03SQLConnect(pDBInfo, ErrText);
    if (errOK_els00 == rc)
    {
        _TCHAR aErrText[MAX_REPLY_LENGTH_LS00];

        rc = ls15ConnectUser(pDBInfo, aErrText);
        if (errOK_els00 != rc)
        {
            if ( (-5015 == rc) || (false == ls00IsDBUnicode(pDBInfo)) )
            {
                // If not yet happened try connecting using UNICODE;
                // to do so we need some manipulation
                if (-5015 == rc)
                {
                    pDBInfo->dbiSession_ls00.siAuthentication_ls00 = false;     // use old password encryption
                }
                else if ( false == ls00IsDBUnicode(pDBInfo) )
                {
                    // Set encoding to UCS2 for next try
                    pDBInfo->dbiEncoding_ls00     = ctUCS2_els00;
                    pDBInfo->dbiOI_Encoding_ls00  = ctUCS2Native_els00;
                }

                // Releasing db connection is possible only after check because the
                // connection is checked via assertion in ls00IsDBUnicode
                ls03SQLRelease(pDBInfo);                    // Error: release db connnection
                rc = errSQLConnectUser_els98;

                //*
                //*     Do it again and again and ...
                //*
                tsp00_Int4 rc2 = errOK_els00;
                rc2 = ls15Connect(pDBInfo, ErrText);        // recursive call
                if (errOK_els00 == rc2)
                {
                    rc = errOK_els00;
                }
                else
                {
                    // Reset encoding to old values
                    pDBInfo->dbiEncoding_ls00     = ctAscii_els00;
                    pDBInfo->dbiOI_Encoding_ls00  = ctAscii_els00;

                    if (dbNotLoadedCharset_els00 != rc2)
                    {
                        rc = rc2;
                    }
                    else
                    {
                        LoaderMessages::Instance()->ls98Msg(layBasicConnect_els98,ErrText, errSQLConnectUser_els98,
                                                            pDBInfo->dbiSession_ls00.siUserName_ls00.asCharp(),
                                                            pDBInfo->dbiSession_ls00.siDBName_ls00.asCharp(),
                                                            aErrText);
                    }
                }
            }   // if ( (-5015 == rc) || (false == ls00IsDBUnicode(pDBInfo)) )
            else
            {
                if (dbNotLoadedCharset_els00 != rc)
                {
                    LoaderMessages::Instance()->ls98Msg(layBasicConnect_els98,ErrText, errSQLConnectUser_els98,
                                                        pDBInfo->dbiSession_ls00.siUserName_ls00.asCharp(),
                                                        pDBInfo->dbiSession_ls00.siDBName_ls00.asCharp(),
                                                        aErrText);
                    rc = errSQLConnectUser_els98;
                }

                ls03SQLRelease(pDBInfo);                 // Error: release db connnection
            }
        }   // end of if (errOK_els00 != (rc = ls15ConnectUser(pDBInfo, aErrText)))

    }   // end of if (errOK_els00 == (rc = ls03SQLConnect(pDBInfo, ErrText)))

    return rc;
}
// ls15Connect()

/*
  -----------------------------------------------------------------------------
  function:     ls15CheckConnection
  -----------------------------------------------------------------------------
*/
SAPDB_Int4
ls15CheckConnection(tls00_DBInfo *pDBInfo, tsp00_Addr pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls15CheckConnection");

    LoaderMessages*  pMsgObj = LoaderMessages::Instance();
    SAPDB_Int4       rc      = errOK_els00;

    if (true != pDBInfo->dbiSession_ls00.siSqlStat_ls00)
    {
        pMsgObj->ls98Msg(layBasicConnect_els98, pszErrText, errNoUserConnection_els98,
                                                            pDBInfo->dbiSession_ls00.siDBName_ls00.asCharp());
        return errNoUserConnection_els98;
    }

    tls00_Reconnect*  pReconnect    = &pDBInfo->dbiSession_ls00.siReconnect_ls00;
    SAPDB_UInt8       elapsed_time  = RTESys_MicroSecTimer() - pReconnect->rcoLastSuccessfulKnlContact_ls00;
    pReconnect->rcoDoReconnect_ls00 = SAPDB_FALSE;

    if (0 != pReconnect->rcoTimeOut_ls00)
    {
        if (elapsed_time > pReconnect->rcoTimeOut_ls00)
        {
            if (true == pDBInfo->dbiSession_ls00.siAutoCommit_ls00) {
                pReconnect->rcoDoReconnect_ls00 = SAPDB_TRUE;
            }
        }
        else
        {
            if (elapsed_time > pReconnect->rcoThreshold_ls00)
            {
                rc = ls13PingDB(pDBInfo, pszErrText);
                if ( (errOK_els00 != rc) &&
                     ( (commErrShutdown_esp01 == pDBInfo->dbiSqlRc_ls00) ||     // database has been restarted
                     (commErrTimeout_esp01  == pDBInfo->dbiSqlRc_ls00)  ) )     // timeout should not happen but who knows
                {
                    // In case the db server has been down and is up again the Loader receives a special
                    // error code that is stored in pDBInfo->dbiSqlRc_ls00 by the called function; we have
                    // to check this and will try a reconnect in following situations:
                    if (true == pDBInfo->dbiSession_ls00.siAutoCommit_ls00)
                    {
                        pReconnect->rcoDoReconnect_ls00 = SAPDB_TRUE;
                    }
                }
            }   // end if (elapsed_time > pReconnect->rcoThreshold_ls00)
        }

        if (SAPDB_TRUE == pReconnect->rcoDoReconnect_ls00)
        {
            rc = ls15Connect(pDBInfo, pszErrText);
            if (errOK_els00 == rc)
            {   // If we have reconnected successfully we write a message into the protocol and initialize pszErrText
                 pszErrText[0] = 0;
                 pMsgObj->ls98PrintMessage(layBasicConnect_els98, msgReconnected_els98,
                                                                  pDBInfo->dbiSession_ls00.siUserName_ls00.asCharp());

                 pReconnect->rcoDoReconnect_ls00 = SAPDB_FALSE;
            }
        }
    }

    return rc;
}
// ls15CheckConnection()

/*
  -----------------------------------------------------------------------------
  function:     ls15LoadCodePage
  -----------------------------------------------------------------------------
*/
tsp00_Longint
ls15LoadCodePage(tls00_DBInfo *pDBInfo, tsp00_Addr pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls15LoadCodePage");

    // Check if there is a user connected; if not the function returns success
    // and the logon functions must load the code page values
    if (false == pDBInfo->dbiSession_ls00.siSqlStat_ls00)
    {
        TRACE_PRNF_MLS99(("ls15LoadCodePage", "No user connected -> code page loading reset until user logon"));
        return errOK_els00;
    }

    LoaderMessages *pMsgInstance = LoaderMessages::Instance();    
    tsp00_Longint    rc      = errOK_els00;

    short i = 0;
    tsp100_VersionResult vres = ls15CheckKernelVersion(pDBInfo, 7, 6, 0, 0);

    char szSchemaName[KNL_IDENTIFIER_SIZE + sizeof(char)];
    if (IsEqual_esp100 == vres || IsNewer_esp100 == vres )
    {
        sp77sprintf(szSchemaName, KNL_IDENTIFIER_SIZE, "SYSLOADER");
    }
    else
    {
        sp77sprintf(szSchemaName, KNL_IDENTIFIER_SIZE, "SYSDBA");
    }

    tsp81_CodePage&     CodePageStruct = pDBInfo->dbiSession_ls00.siCodePage_ls00;
    tls00_SessionInfo&  Session = pDBInfo->dbiSession_ls00;

    //*
    //*     Check if default code page is chosen and if yes fill it quickly and return
    //*
    if (0 == _tcsnicmp(CodePageStruct.name.asCharp(), pszDefaultCodePageName_ls00, CodePageStruct.name.length()))
    {
        SAPDB_memcpy(pDBInfo->dbiSession_ls00.siCodePage_ls00.map, lDefaultCodePageMap_ls00, 256*sizeof(tsp00_Uint2));
        return errOK_els00;
    }
    //*
    //*     Now handle other code pages
    //*
    tsp00_Int4 lErrPos = 0;
    char       szErr[MAX_REPLY_LENGTH_LS00];
    szErr[0] = 0;
    //*
    //*     Check whether system table SYSDBA.CODEPAGE exists and all rights are
    //*     granted to public (only then it's visible to an arbitrary user
    //*
    bool                     bCPTableExists       = false;
    bool                     bCPTableMustBeLoaded = true;
    Tools_DynamicUTF8String  SYSDBAUserName(szSchemaName);
    Tools_DynamicUTF8String  TableName("CODEPAGE");

    rc = ls13CheckTableExistence(pDBInfo, SYSDBAUserName, TableName, bCPTableExists, pszErrText);
    if (errOK_els00 == rc)
    {
        char szCmd[2048];
        szCmd[0] = 0;

        if (false == bCPTableExists)
        {   
            // Try to create the CODEPAGE table
            bCPTableMustBeLoaded = true;
            sp77sprintf(szCmd, 2048, CREATE_CODEPAGE_TABLE_LS15, szSchemaName);

            // set installation_on flag in DBInfo
            pDBInfo->dbiSession_ls00.siInstallOn_ls00 = true;

            rc = ls13ExecIntSQLStmt(pDBInfo, 0, szCmd, szErr, lErrPos, false, sp1m_dbs, false);
            if (errOK_els00 == rc)
            {
                szCmd[0] = 0;
                sp77sprintf(szCmd, 2048, GRANT_CODEPAGE_TABLE_LS15, szSchemaName);

                rc = ls13ExecIntSQLStmt(pDBInfo, 0, szCmd, szErr, lErrPos, false, sp1m_dbs, false);
            }
            if (errOK_els00 == rc)
            {
                ls15Commit(pDBInfo, pszErrText);
                bCPTableExists = true;
            }
            else
            {
                // Error creating/granting CODEPAGE table; we'll try to work
                // without this table after notifying the user in the logfile;
                // bCPTableExists is still set to false
                pMsgInstance->ls98PrintMessage(layBasicConnect_els98, errCreate_GrantCPTable_els98, szErr);

                // Reset error code to success to try reading code page file
                rc = errOK_els00;
            }
            // Reset installation_on flag
            pDBInfo->dbiSession_ls00.siInstallOn_ls00 = false;
        }
        else    // if (false == bCPTableExists)
        {   // Check if CODEPAGE table contains the right values

            sp77sprintf(szCmd, 2048, SELECT_CODEPAGE_TABLE_LS15, szSchemaName, CodePageStruct.name.asCharp());

            rc = ls13ExecIntSQLStmt(pDBInfo, 0, szCmd, szErr, lErrPos, false, sp1m_dbs, false);
            if (errOK_els00 != rc)
            {
                if (dbRowNotFound_els00 == rc)
                {
                    rc = errOK_els00;
                }
                else
                {
                    // Error reading code page table
                    pMsgInstance->ls98Msg(layBasicConnect_els98, pszErrText, errReadCPTable_els98, szErr);

                    bCPTableMustBeLoaded = false;

                    // Reset error code to success to try reading code page file
                    rc = errOK_els00;
                }
            }
            else
            {
                // Fetch the data
                // CODEPAGE table exists and contains the right mapping. Get the mapping by simply
                // memcpy-ing it to our internal structure. The swap will be corrected a bit later.
                tsp00_Addr pszBuffer = NULL;
                tsp00_Int4 BufLen    = 0;
                tsp00_Int4 RowCount  = 1;
                rc = ls12FetchEx(*pDBInfo, NULL, RowCount, false, pszBuffer, BufLen, pszErrText);
                if (errOK_els00 != rc)
                {
                    // Error reading code page table
                    pMsgInstance->ls98Msg(layBasicConnect_els98, pszErrText, errReadCPTable_els98, szErr);

                    // Reset error code to success to try reading code page file
                    rc = errOK_els00;
                }
                else
                {
                    SAPDB_memcpy(CodePageStruct.map, pszBuffer+sizeof(char), sizeof(CodePageStruct.map));

                }

                bCPTableMustBeLoaded = false;
            }
        }   // end if (false == bCPTableExists)


//TODOTODO: Wenn wir einfach nicht die system tabelle lesen konnten, sollten wir vielleicht trotzdem weitermachen
        // und nur intern das mapping verwenden ohne es in die system tabelle zu laden?


        if ( (errOK_els00 == rc) && ( (true == bCPTableMustBeLoaded) || (false == bCPTableExists) ) )
        {
            //
            //  User must be SYSDBA to execute this
            //
            rc = ls15CheckUserMode(pDBInfo, REINTERPRET_CAST(SAPDB_UTF8*, pDBInfo->dbiSession_ls00.siUserName_ls00.asCharp()),
                                   dbuSysdba_els00, pszErrText);
            if (dbRowNotFound_els00 == rc)
            {
                pMsgInstance->ls98MsgToString(layTableExtract_els98, pszErrText, errCodepageChangeOnlyBySysdba_els98,
                                                                                 CodePageStruct.name.asCharp(),
                                                                                 szSchemaName);
                return errCodepageChangeOnlyBySysdba_els98;
            }

            //*
            //* Load codepage table
            //* This table contains the mapping of the ASCII characters of a certain code page
            //* (up to now supported ISO-8859-1 - ISO-8859-16) to corresponding UCS2 characters.
            //* The UCS2 characters are stored as not swapped 2 Byte CHAR BYTE values in an 512 Byte array.
            //* At this point we know that the table SYSDBA.CODEPAGE exists in the db system but
            //* does not contain the mapping for the requested code page.
            //*
            //* The table will be loaded using files published by the UNICODE consortium at
            //* ftp://ftp.unicode.org/Public/MAPPINGS/.
            //* The file to be used must be located in directory /<dependend_path>/env and must be named
            //* 'ISO-8859-<codepage_number>.txt.
            //*
            //* Because certain code pages do not contain mappings for all 256 ASCII characters we'll
            //* assign the value 'FFFF' to those UCS2 characters that are not assigned.
            //*

            //*
            //* Build file name, find it in file system and open it in text mode
            //*
            SAPDB_Char     ConfigFilePath[PATH_MXSP00];
            ConfigFilePath[0] = '\0';
            tls00_VFile    CodePageFile;
            tsp00_Pathc    szPath;
            tsp01_RteError RteError;
            char           szFileName[1024];
            szFileName[0] = 0;

            if ( NULL != getenv ("maxdbloaderserver") )
            {
                 sqlGetDbrootEnvPath(szPath, TERM_WITH_DELIMITER_EO01, &RteError); 
                 strcat(ConfigFilePath,szPath.asCharp());
            }
            else
            {
                 pMsgInstance->ls98GetLoaderConfigPath(ConfigFilePath);
            }
            sp77sprintf(szFileName, 1024, "%s%s.txt", ConfigFilePath, CodePageStruct.name.asCharp());
            ls30VFInitialize(CodePageFile, szFileName, sp5vf_text);

            rc = ls30VFOpen(CodePageFile, pszErrText);
            if (errOK_els00 == rc)
            {
                //*
                //* File found and opened; now read it and fill UCS2 char array
                //*
                tsp00_Int4 rcRead = errOK_els00;

                // This array is used to compare the read ASCII characters with and
                // validate that the file is one of those we expect
                unsigned char szFullAsciiTable[MAX_UINT1_SP00 + 1];
                for (tsp00_Uint2 l = 0; l <= MAX_UINT1_SP00; szFullAsciiTable[l++] = STATIC_CAST(unsigned char, l))
                    ;

                // This array holds the read ASCII chars
                unsigned char szReadAsciiTable[MAX_UINT1_SP00 + 1];

                // Buffer for reading the file
                char szBuffer[MAX_UINT1_SP00 + 1];

                // Mapping table
                unsigned char szUCS2ValueTable[1024];
                memset(szUCS2ValueTable, 0, 1024);

                // Auxiliary variables for converting text characters to hex values
                unsigned char e[2];
                tsp00_Uint2   lValIndex     = 0;
                tsp00_Longint lByteValueLen = 1;

                do
                {
                    //*
                    //* Main read file routine where the mapping table and the ASCII table is filled
                    //*
                    rcRead = ls30VFReadLnEx(CodePageFile, szBuffer, pszErrText, CodePageFile.vfFileLen_ls00.lnuRemainder_ls00);
                    if (rcRead > 0)
                    {
                        if ('#' != szBuffer[0])     // the files contain comments that begin with #
                        {
                            // No comment: at position 3 starts the ASCII char that should be mapped
                            rc = ls07ConvertHexToByteEx(&szBuffer[2], 2, e, lByteValueLen);

                            // Mark unassigned UCS2 values and make sure not to exceed the table size
                            // of 1024 Bytes which could happen if the file is not the one we expect
                            if (rc >= errOK_els00)
                            {
                                while ( (e[0] > lValIndex) && (lValIndex <= MAX_UINT1_SP00) )
                                {
                                    SAPDB_memcpy(&szUCS2ValueTable[lValIndex*4], "FFFF", 4);
                                    szReadAsciiTable[lValIndex] = STATIC_CAST(unsigned char, lValIndex); // for validation

                                    lValIndex  += 1;
                                }

                                // Ok, found value that can be assigned: do it
                                // But make sure we have no endless loop and check if
                                // lValIndex is still within the limit
                                if (lValIndex <= MAX_UINT1_SP00)
                                {
                                    SAPDB_memcpy(&szUCS2ValueTable[lValIndex*4], &szBuffer[7], 4);
                                    szReadAsciiTable[lValIndex] = e[0];     // to validate it later on

                                    lValIndex  += 1;
                                }
                            }
                            else
                            {
                                // Error converting from text to hex: stop it
                                pMsgInstance->ls98Msg(layBasicConnect_els98, pszErrText, errInvalidHexInCPFile_els98,
                                                                                    2, &szBuffer[2]);
                                rc = errInvalidHexInCPFile_els98;
                            }   // end if (rc >= errOK_els00) for ls07ConvertHexToByteEx(...

                        }   // end if ('#' != szBuffer[0])

                        // Reinitialize read buffer
                        szBuffer[0] = 0;

                    }   // end if (rc > 0)
                    else
                    {
                        if (EOF_LS00 != rcRead)
                        {
                            // Read function has alredy logged the error message
                            rc = rcRead;
                        }
                    }
                }
                while ( (rcRead > 0) && (rc >= 0) && (lValIndex <= MAX_UINT1_SP00) );

                if (rc >= errOK_els00)
                {
                    rc = errOK_els00;

                    //*
                    //* No error reading the file and converting the text values to byte values
                    //* Check if there are some unassigned values left.
                    //* Check if the read ASCII values correspond to the expected (assended sorted) values
                    //* Insert values into db table.
                    //*

                    for (lValIndex; lValIndex <= MAX_UINT1_SP00; ++lValIndex)
                    {
                        SAPDB_memcpy(&szUCS2ValueTable[lValIndex*4], "FFFF", 4);
                        szReadAsciiTable[lValIndex] = STATIC_CAST(unsigned char, lValIndex);
                    }


                    // Check validity of read UCS2 values by checking the ascii values read
                    if (0 != memcmp(szReadAsciiTable, szFullAsciiTable, 256))
                    {
                        pMsgInstance->ls98Msg(layBasicConnect_els98, pszErrText, errInvalidASCIICPValues_els98);
                        rc = errInvalidASCIICPValues_els98;
                    }
                    else
                    {
                        // Simply memcpy-ing mapping to our internal structure. The swap will be corrected a bit later.
                        tsp00_Longint lCodePageTableBinLen = sizeof(CodePageStruct.map);

                        rc = ls07ConvertHexToByteEx(REINTERPRET_CAST(char*, szUCS2ValueTable),
                                                    (MAX_UINT1_SP00+1)*4,
                                                    REINTERPRET_CAST(unsigned char*, CodePageStruct.map),
                                                    lCodePageTableBinLen);
                        if (rc != errOK_els00)
                        {
                            // Error converting from text to hex: stop it

                            pMsgInstance->ls98Msg(layBasicConnect_els98, pszErrText, errInvalidHexInCPFile_els98,
                                                                                (MAX_UINT1_SP00+1)*4, szUCS2ValueTable);
                            rc = errInvalidHexInCPFile_els98;
                        }

                        // Insert values into table
                        
                        if ( (errOK_els00 == rc) && (true == bCPTableExists) )
                        {
                            //sp77sprintf(szCmd, 2048, REINTERPRET_CAST(SAPDB_Char *,(CONST_CAST (unsigned char* ,INSERT_CODEPAGE_TABLE_LS00.StrPtr()))), CodePageStruct.name.asCharp(),
                            //                                                       (MAX_UINT1_SP00+1)*4, szUCS2ValueTable);
                            sp77sprintf(szCmd, 2048, INSERT_CODEPAGE_TABLE_LS15, szSchemaName,
                                                                                 CodePageStruct.name.asCharp(),
                                                                                 (MAX_UINT1_SP00+1)*4, szUCS2ValueTable);

                            // set installation on in DBInfo
                            pDBInfo->dbiSession_ls00.siInstallOn_ls00 = true;
                            rc = ls13ExecIntSQLStmt(pDBInfo, 0, szCmd, pszErrText, lErrPos, false, sp1m_dbs, false);
                            pDBInfo->dbiSession_ls00.siInstallOn_ls00 = false;

                            if (errOK_els00 == rc)
                            {
                                ls15Commit(pDBInfo, pszErrText);
                            }
                        }   // end if (true == bCPTableExists)
                        
                    }
                }   // end if (rc >= errOK_els00)
                // else branch not needed because an error message is generated during processing
            }   // end if (errOK_els00 == (rc = ls30VFOpen(CodePageFile, pszErrText))
        }   // end if (true == bCPTableMustBeLoaded)
    }   // end if (errOK_els00 == rc) = ls13CheckTableExistence(pDBInfo, ...
    // else branch: function returns in case of error the error code and the error message and logs
    //              the message to the log file; this is a severe error so we leave here without doing anything

    // Codepage table either simply read or build; now check if the swap is right
    if (errOK_els00 == rc)
    {
        // Swap mapping table if necessary.
        if (sw_full_swapped == pDBInfo->dbiOwnSwap_ls00)
        {
            ls07SwapUCS2StringInPlace(REINTERPRET_CAST(char*, Session.siCodePage_ls00.map), MAX_UINT1_SP00 + 1);
        }
    }

    return rc;
}
// ls15LoadCodePage()