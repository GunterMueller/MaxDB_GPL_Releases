/*
  -----------------------------------------------------------------------------
  module: vls01.cpp
  -----------------------------------------------------------------------------
  responsible:  SteffenS and d033893

  special area: SAP DB LOADER

  description:  communication with client

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

#include "gls00.h"
#include "hls01.h"
#include "hls03.h"
#include "hls05.h"      // UPPER and lower letter conversion maps
#include "hls11.h"
#include "hls99.h"
#include "hls98msg.h"
#include "gsp09.h"

externPascal tin_global_in_vars i01g;

/* Lokale Funktionen: ******************************************************************/

/*!
  -----------------------------------------------------------------------------

  function:     ls01_InitDBInfo

  -----------------------------------------------------------------------------

  description:  Initializes the db info structure and parameters.

                SQLMODE             = INTERNAL
                Isolations Level    = 3
                date type           = Internal
                time type           = Internal
                stamp Typ           = Internal
                digit grouping symbol = none
                decimal symbol      = point
                string separator    = comma
                string delimiter    = QUOTE_LS00
                NULL string         = "?                   "    (20 zeichen)
                TRUE string         = "TRUE      "              (10 zeichen)
                FALSE string        = "FALSE     "              (10 zeichen)
                output language     = "DEU"
                code type           = ASCII
                DB encoding         = ASCII


  arguments:    DBInfo     [IN] - db info structure
                DBName     [IN] - db name

  returnvalue:  No
  -----------------------------------------------------------------------------
*/
void ls01_InitDBInfo(tls00_DBInfo *DBInfo, const char* DBName)
{
    ROUTINE_DBG_MEO00 ("ls01_InitDBInfo");

    i01g = new tin_global_in_record;

    /* Initialisierung */
    DBInfo->dbiCommand_ls00.strAddr_ls00 = NULL;
    DBInfo->dbiCommand_ls00.strLeng_ls00 = 0;


    DBInfo->dbiSqlRef_ls00      = 0;

    // Make the loader a user task
    DBInfo->dbiSqlService_ls00.becomes(srvUser_esp01);

    DBInfo->dbiSqlRc_ls00.becomes(commErrOk_esp01);
    DBInfo->dbiEncoding_ls00    = ctAscii_els00;
    DBInfo->dbiOI_Encoding_ls00 = ctAscii_els00;
    DBInfo->dbiPageSize_ls00    = 0;
    DBInfo->dbiPktCount_ls00    = 2;
    DBInfo->dbiPktSize_ls00     = 0;

    // ODBC Connection
    DBInfo->dbiOdbcConnectionPool   = new sapdbwa_DBCPool;

    // OI Connection
    RTESys_InitMicroSecTimer();
    DBInfo->dbiPktSndList_ls00[0]   = NULL;
    DBInfo->dbiPktSndList_ls00[1]   = NULL;
    DBInfo->dbiPktRcvList_ls00[0]   = NULL;
    DBInfo->dbiPktRcvList_ls00[1]   = NULL;

    DBInfo->dbiLoaderSessionIsoLevel_ls00 = 3;      // This is the predefined default that can be set with SET ISOLATION LEVEL

    // This is the kernel parameter DATE_TIME_FORMAT and set during user connect
    DBInfo->dbiDateTimeFormat_ls00  = dtf_normal;

    memset(DBInfo->dbiSqlTerm_ls00, BLANK_LS00, sizeof (tsp00_TermId));
    memset(DBInfo->dbiSqlErr_ls00,  BLANK_LS00, sizeof (tsp00_ErrText));

    DBInfo->dbiSession_ls00.siUserKey_ls00           = NULL;
    DBInfo->dbiSession_ls00.siSQLMode_ls00           = sp1sm_internal;
    DBInfo->dbiSession_ls00.siDBSessionIsoLevel_ls00 = DBInfo->dbiLoaderSessionIsoLevel_ls00;
    DBInfo->dbiSession_ls00.siBackupTool_ls00        = btTypeUnknown_els00;
    DBInfo->dbiSession_ls00.siConfigurationType_ls00 = ctExclude_els00;

    DBInfo->dbiSession_ls00.siDBConnection_ls00         = false;
    DBInfo->dbiSession_ls00.siSqlStat_ls00              = false;
    DBInfo->dbiSession_ls00.siAuthentication_ls00       = true;       // Use new password encryption/authentication as default
    DBInfo->dbiSession_ls00.siEncryptionProtocol_ls00   = epUnknown_els00;
    memset(&DBInfo->dbiSession_ls00.siReconnect_ls00, 0, sizeof(tls00_Reconnect));
    DBInfo->dbiSession_ls00.siAutoCommit_ls00           = true;
    DBInfo->dbiSession_ls00.siMessageOn_ls00            = false;
    DBInfo->dbiSession_ls00.siInstallOn_ls00            = false;
    DBInfo->dbiSession_ls00.siODBCTrace_ls00            = false;
    DBInfo->dbiSession_ls00.siCheckConstraint_ls00      = true;
    DBInfo->dbiSession_ls00.siSystemAttribute_ls00      = false;
    DBInfo->dbiSession_ls00.siNolog_ls00                = false;

    DBInfo->dbiSession_ls00.siFormat_ls00       = dtInternal_els00;
    DBInfo->dbiSession_ls00.siDateType_ls00     = dtInternal_els00;
    DBInfo->dbiSession_ls00.siTimeType_ls00     = dtInternal_els00;
    DBInfo->dbiSession_ls00.siStampType_ls00    = dtInternal_els00;
    DBInfo->dbiSession_ls00.siThsdChar_ls00     = NO_THSD_GROUPS_LS00;
    DBInfo->dbiSession_ls00.siDecPoint_ls00     = POINT_LS00;

    DBInfo->dbiSession_ls00.siSeperat_ls00[0]   = COMMA_LS00;
    DBInfo->dbiSession_ls00.siDelimit_ls00[0]   = QUOTE_LS00;
    DBInfo->dbiSession_ls00.siSeperat_ls00[1]   = '\0';
    DBInfo->dbiSession_ls00.siDelimit_ls00[1]   = '\0';

    DBInfo->dbiSession_ls00.siMaxAdmissibleErrors   = 1;

    DBInfo->dbiSession_ls00.siNullSpec_ls00.rawAssign("?                   ");

    DBInfo->dbiSession_ls00.siConfigurationType_ls00 = ctExclude_els00;

    // Boolwert fuer TRUE
    DBInfo->dbiSession_ls00.siTrueVal_ls00[0] = '\0';
    strcpy(DBInfo->dbiSession_ls00.siTrueVal_ls00,  "TRUE");

    DBInfo->dbiSession_ls00.siFalseVal_ls00[0] = '\0';
    strcpy(DBInfo->dbiSession_ls00.siFalseVal_ls00, "FALSE");

    SAPDB_memcpy (DBInfo->dbiSession_ls00.siLanguage_ls00, "DEU", sizeof (tsp00_C3));

    DBInfo->dbiSession_ls00.siDBName_ls00.Init();
    DBInfo->dbiSession_ls00.si_pDBNode_ls00 = NULL;
    DBInfo->dbiSession_ls00.siUserName_ls00.Init();

    memset(DBInfo->dbiSYSDBA_Name_ls00.asCharp(), 0, sizeof(tsp00_KnlIdentifierc));
    memset(DBInfo->dbiSession_ls00.siUserGroup_ls00.asCharp(), 0, sizeof(tsp00_KnlIdentifierc));
    memset(DBInfo->dbiSession_ls00.siUserCPwd_ls00, BLANK_LS00, sizeof(tsp00_CryptPw));
    memset(DBInfo->dbiSession_ls00.siUserPwd_ls00.asCharp(), BLANK_LS00, sizeof(tsp00_Namec));
	DBInfo->dbiSession_ls00.siSchemaName_ls00   [0] = '\0';
    DBInfo->dbiSession_ls00.siSchemaMapName_ls00[0] = '\0';

    // Codetypes, termcharsets and conversion tables
    DBInfo->dbiSession_ls00.siCodeType_ls00     = ctUnknown_els00;




    // Set default code page for ASCII to UCS2 conversion
    DBInfo->dbiSession_ls00.siCodePage_ls00.name.Init();
    DBInfo->dbiSession_ls00.siCodePage_ls00.name.rawAssign(pszDefaultCodePageName_ls00);
    SAPDB_memcpy(DBInfo->dbiSession_ls00.siCodePage_ls00.map, lDefaultCodePageMap_ls00, 256*sizeof(tsp00_Uint2));

    // Transaction size
    DBInfo->dbiSession_ls00.siTASize_ls00 = 0;

    if (DBName != NULL)
    {
        DBInfo->dbiSession_ls00.siDBName_ls00.rawAssign(DBName);
    }

    // Check swap behavior of machine
    union
    {
        tsp00_Int4 IntVal;
        char       CharVal[4];
    } uSwap;


    uSwap.IntVal = 1;
    if (1 == (int) uSwap.CharVal[3])
    {
        DBInfo->dbiOwnSwap_ls00.becomes(sw_normal);
    }
    else if (1 == (int) uSwap.CharVal[0])
    {
        DBInfo->dbiOwnSwap_ls00.becomes(sw_full_swapped);
    }
    else
    {
        DBInfo->dbiOwnSwap_ls00.becomes(sw_part_swapped);
    }

    // Initialize a global PASCAL (!) structure that is used for date, time and timestamp format conversion
    // http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124596
    SAPDB_memcpy(DBInfo->dbiSession_ls00.siConversionTables_ls00[0], REINTERPRET_CAST(unsigned char*, lLatin1_ToLowerMap_ls05), 256);
    i01g->i32.transtables[0] = DBInfo->dbiSession_ls00.siConversionTables_ls00[0];
    SAPDB_memcpy(DBInfo->dbiSession_ls00.siConversionTables_ls00[1], REINTERPRET_CAST(unsigned char*, lLatin1_ToUpperMap_ls05), 256);
    i01g->i32.transtables[1] = DBInfo->dbiSession_ls00.siConversionTables_ls00[1];
    i01g->i20.code_type = csp_ascii;
}

/********************************************************************************/
/* FUNCTION ls01_RefreshDBInfo - lokale funktion                                */
/*==============================================================================*/
/* PURPOSE                                                                      */
/*  Reinitialisiert werte der struktur DBInfo.                                  */
/*==============================================================================*/
/* PARAMETER        IN/OUT      DESCRIPTION                                     */
/*  *DBInfo         X           pointer to load session info structure          */
/*==============================================================================*/
/* RETURN CODE              DESCRIPTION                                         */
/********************************************************************************/
void ls01_RefreshDBInfo(tls00_DBInfo *DBInfo)
{
    DBInfo->dbiPktRc_ls00 = 0;
}

/* Globale Funktionen: *****************************************************************/


/*
  -----------------------------------------------------------------------------
  function:     ctrlservopen
  -----------------------------------------------------------------------------
*/
void ctrlservopen(void       **vcontrolData,
                  tsp09_cstr   dbname,      // const char*
                  tsp09_cstr   dbroot,      // const char*
                  BOOLEAN     *cancelAddr,
                  BOOLEAN      loggedOn,
                  BOOLEAN      couldLogOn,
                  int          callerId,
                  BOOLEAN      isLocalCommunication,
                  const char* clientMachineID,
                  SAPDB_Int8  clientPIDonClientMachine)
{
    ROUTINE_DBG_MEO00 ("ls01CtrlServOpen");



    // Handle local communication on UNIX - on NT the couldLogOn is always false
    tsp09_rte_xerror xerror;
    if (couldLogOn)
    {   // This indicates that the user may be logged on which is done here

        // The server is always started by vserver;
        // this means it runs first as root;
        // so if server and client run locally the server can log on the user who called
        // the client and than runs as this user
        sqlxidlogon (callerId, &xerror);
        if (xerror.xe_result == 0)
        {
            loggedOn = true;
        }
    }

    // Initialize log file - must happen prior to opening the trace as the trace uses the log file path
    LoaderMessages* MsgInstance = LoaderMessages::Instance();

    //*
    //*     Open trace file
    //*
    TRACE_INIT_MLS99("ls01ServOpen", "PROTOKOLL ANFANG", "loader.trc");

    // Only now trace user logon - this is necessary to make sure the trace file is opened after
    // the effective user has logged on
    if (couldLogOn)
    {
        TRACE_PRNF_MLS99(("ls01CtrlServOpen", "Logging on OS user (uid = %d) possible\n", callerId));
        if (xerror.xe_result == 0)
        {
            TRACE_PRNF_MLS99(("ls01CtrlServOpen", "OS user (uid = %d) logon successful\n", callerId));
        }
        else
        {
            TRACE_PRNF_MLS99(("ls01CtrlServOpen", "OS user (uid = %d) logon failed with error %d = %s\n",
                                                  callerId,
                                                  xerror.xe_native_error,
                                                  xerror.xe_text));
        }
    }
    else
    {
        // Either the user is already logged on locally (which means the euid and uid are the same
        // or this is a remote call
        if (loggedOn)
        {
            TRACE_PRNF_MLS99(("ls01CtrlServOpen", "OS user logon successful (euid = uid)\n"));
        }
        else
        {
            TRACE_PRNF_MLS99(("ls01CtrlServOpen", "OS user logon failed - remote call\n"));
        }
    }


    tls00_DBInfo*   DBInfo        = new tls00_DBInfo;
    tsp00_CompName  cname;

    ls01_InitDBInfo(DBInfo, dbname);        // Initializes dbinfo struct and adjusts parameter

    tsp00_Int4 rc = MsgInstance->ls98InitLogFile(dbname);

    SAPDB_memcpy(cname, COMP_NAME_LS00, 64);
    sqlinit(cname, NULL);

    // save cancel byte address
    DBInfo->dbiSession_ls00.sipCancelByte_ls00 = REINTERPRET_CAST(bool*, cancelAddr);

    *vcontrolData = DBInfo;


    // Initialize Runtime Statistics Singleton
    Loader_RTStatistics* pStatObj = Loader_RTStatistics::Instance();
}
// ctrlservopen()

/*
  -----------------------------------------------------------------------------
  function:     ctrlservclose
  -----------------------------------------------------------------------------
*/
void ctrlservclose(void*  SessionData)
{
    ROUTINE_DBG_MEO00 ("ls01CtrlServClose");

    tls00_DBInfo*   DBInfo = REINTERPRET_CAST (tls00_DBInfo*, SessionData);

    ls03SQLRelease (DBInfo);                // release db connection

    TRACE_ENDE_MLS99 ("ls01ServClose", "PROTOKOLL ENDE");

    if ( NULL != DBInfo->dbiOdbcConnectionPool )
    {
         delete DBInfo->dbiOdbcConnectionPool;
         DBInfo->dbiOdbcConnectionPool = NULL;
    }
    if ( NULL != DBInfo )
    {
        delete DBInfo;
        DBInfo = NULL;
    }
    delete i01g;

    // close log file
    delete LoaderMessages::Instance();
}
// ctrlservclose()


/*
  -----------------------------------------------------------------------------
  function:     ctrlservcommand
  -----------------------------------------------------------------------------
*/
int ctrlservcommand(void*       SessionData,
                    tsp00_Addr  RequestData,
                    tsp00_Int4  RequestLen,
                    tsp00_Addr  ReplyData,
                    tsp00_Int4* ReplyLen,
                    tsp00_Int4  ReplyLenMax)
{
    ROUTINE_DBG_MEO00 ("ls01CtrlServCmd");

    //*
    //*     Get session data
    //*
    tls00_DBInfo    *pDBInfo  = (tls00_DBInfo*) SessionData;



    // Check if the loader c/s protocol is used
    bool bCmdContinued = false;
    bool bStaticServer = false;
    bool bNoProtocol   = false;

    if (RequestData[0] > 9)
    {
        bNoProtocol = true;
    }
    else
    {
        if ('\x01' == RequestData[0])
        {
            bCmdContinued = true;
        }

        if ('\x01' == RequestData[1])
        {
            bStaticServer = true;
        }
    }

    //*
    //*     Initialize command structure thats used in the server
    //* ATTENTION: CmdScanRc is set here to srUnknown_ls00 !!!!
    //*
    tls00_Command *pSvrCmd  = new tls00_Command;
    memset (pSvrCmd, DEF_BYTE_LS00, sizeof (tls00_Command));

    //*
    //*     Make the answer string local and initialize functions reply string
    //* In case of statically linked client/server the string doesn't need to be
    //* allocated again. The client simply calls ctrlservcommand without any
    //* extra communication.
    //*
    tsp00_Int4 MyCmdReplyLen = 0;
    memset(ReplyData, DEF_BYTE_LS00, ReplyLenMax);
    if (true == bStaticServer)
    {
        pSvrCmd->cmdRpyDat_ls00 = ReplyData + ProtHeaderLength_ls00;    // skip the 4 Byte error protocol
    }
    else
    {
        pSvrCmd->cmdRpyDat_ls00 = new char[MAX_REPLY_LENGTH_LS00];
        memset(pSvrCmd->cmdRpyDat_ls00, DEF_BYTE_LS00, MAX_REPLY_LENGTH_LS00);
    }


    tsp00_Int4  CmdExecRc = errOK_els00;
    tsp00_Int4  ErrCode   = CtrlCmdOk_csp09;


    //*
    //*     Define string and string length used to process command arriving parts
    //*
    tsp00_Addr pszTmpStr = NULL;

    // only because of compatibility reason we allow for commands without protocol
    tsp00_Int4 lTmpStrLen = (true == bNoProtocol) ? RequestLen
                                                  : pDBInfo->dbiCommand_ls00.strLeng_ls00 + RequestLen - PROTOCOL_SIZE_LS00;
    //*
    //*     Command protocol processing (command in parts or not?)
    //*
    if (true == bCmdContinued)
    {
        pszTmpStr = new char[lTmpStrLen + 1];
        pszTmpStr[0] = 0;

        if (0 != pDBInfo->dbiCommand_ls00.strLeng_ls00)
        {
            SAPDB_memcpy(pszTmpStr, pDBInfo->dbiCommand_ls00.strAddr_ls00, pDBInfo->dbiCommand_ls00.strLeng_ls00);
            delete pDBInfo->dbiCommand_ls00.strAddr_ls00;
            pDBInfo->dbiCommand_ls00.strAddr_ls00 = NULL;
        }

        // second term (if it is the 2nd) copied
        SAPDB_memcpy(&pszTmpStr[pDBInfo->dbiCommand_ls00.strLeng_ls00], &RequestData[PROTOCOL_SIZE_LS00], (RequestLen - PROTOCOL_SIZE_LS00));

        // 'repointer'
        pDBInfo->dbiCommand_ls00.strAddr_ls00  = pszTmpStr;
        pDBInfo->dbiCommand_ls00.strLeng_ls00 += RequestLen - PROTOCOL_SIZE_LS00;

        pSvrCmd->cmdReqErr_ls00 = errOK_els00;
    }
    else
    {
        ls01_RefreshDBInfo(pDBInfo);

        //*
        //*     Either command could be received in its fullness or last part arrived
        //* Means: process the command
        //* In case of statically linked client/server the commands are NEVER continued
        //* but always given as parameter in its full length. Thus here is no special
        //* treatment for continued commands necessary.
        //*

        if (true == bStaticServer)
        {
            pSvrCmd->cmdReqDat_ls00 = RequestData + PROTOCOL_SIZE_LS00;     // skip the 4 Byte protocol
        }
        else
        {
            //*
            //*     Make the request string local
            //*
            pSvrCmd->cmdReqDat_ls00    = new char[lTmpStrLen + 1];
            pSvrCmd->cmdReqDat_ls00[0] = 0;

            //*
            //*     If there were parts of the commands send before build the whole command to process
            //*
            if (0 != pDBInfo->dbiCommand_ls00.strLeng_ls00)
            {
                SAPDB_memcpy(pSvrCmd->cmdReqDat_ls00, pDBInfo->dbiCommand_ls00.strAddr_ls00, pDBInfo->dbiCommand_ls00.strLeng_ls00);

                // Remove command string buffer - not longer necessary for this command
                // Length is reset a bit later
                delete pDBInfo->dbiCommand_ls00.strAddr_ls00;
                pDBInfo->dbiCommand_ls00.strAddr_ls00 = NULL;
            }

            if (true == bNoProtocol)        // Uuuhuuhuuu we try to be compatible to older version
            {
                SAPDB_memcpy(&pSvrCmd->cmdReqDat_ls00[pDBInfo->dbiCommand_ls00.strLeng_ls00], RequestData, RequestLen);
            }
            else
            {
                SAPDB_memcpy(&pSvrCmd->cmdReqDat_ls00[pDBInfo->dbiCommand_ls00.strLeng_ls00],
                             &RequestData[PROTOCOL_SIZE_LS00],
                             (RequestLen - PROTOCOL_SIZE_LS00));
            }
            pDBInfo->dbiCommand_ls00.strLeng_ls00 = 0;  // Reset length on session command string buffer
        }
        // Append end of string marker
        pSvrCmd->cmdReqDat_ls00[lTmpStrLen] = DEF_BYTE_LS00;

        // initialize logging behaviour
        pSvrCmd->cmdLogging_ls00  = SAPDB_TRUE ;
        pSvrCmd->cmdInternal_ls00 = SAPDB_FALSE;

        TRACE_PRNF_MLS99(("ls01CtrlServCmd", "Parse statement '%s'.\n", pSvrCmd->cmdReqDat_ls00));

        //*
        //*     Now we really process the REQUEST
        //*
        CmdExecRc     = ls11ParseDispatchLoadCommand (pSvrCmd, pDBInfo);

        MyCmdReplyLen = STATIC_CAST(tsp00_Int4, _tcslen(pSvrCmd->cmdRpyDat_ls00) );

    }   // end else of if (true == bCmdContinued)



    //*
    //*     Define return value
    //* This is 'success' even if the command fails. Only in case of an internal server
    //* error the return value is 'error' and in case of release command the return value is 'release'.
    //*
    if (errOK_els00 != CmdExecRc)           // internal error
    {
        strcpy(ReplyData, ProtErrHeader_ls00);

        ErrCode = CtrlCmdErr_csp09;
    }
    else
    {
        if (srRelease_els00 == pSvrCmd->cmdScanRC_ls00)  // release
        {
            ErrCode = CtrlRelease_csp09;
        }
        else
        {
            if (pSvrCmd->cmdReqErr_ls00 < errOK_els00) // error at processing command
            {
                SAPDB_memcpy(ReplyData, ProtErrHeader_ls00, ProtHeaderLength_ls00);
            }
            else
            {
                SAPDB_memcpy(ReplyData, ProtOkHeader_ls00, ProtHeaderLength_ls00);
            }
        }
    }

    if (0 != MyCmdReplyLen)     // there is some message from processing the command
    {
        if (true == bStaticServer)
        {
            *ReplyLen = MyCmdReplyLen + ProtHeaderLength_ls00;
        }
        else
        {
            if ( ReplyLenMax < (MyCmdReplyLen + ProtHeaderLength_ls00) )
            {
                SAPDB_memcpy(ReplyData + ProtHeaderLength_ls00, pSvrCmd->cmdRpyDat_ls00, (ReplyLenMax - ProtHeaderLength_ls00));
                *ReplyLen = ReplyLenMax;
            }
            else
            {
                SAPDB_memcpy(ReplyData + ProtHeaderLength_ls00, pSvrCmd->cmdRpyDat_ls00, MyCmdReplyLen);
                *ReplyLen = MyCmdReplyLen + ProtHeaderLength_ls00;
            }
        }
    }
    else                        // no message from processing
    {
        *ReplyLen = ProtHeaderLength_ls00;
    }


    if (false == bStaticServer)
    {
        delete pSvrCmd->cmdReqDat_ls00;
        delete pSvrCmd->cmdRpyDat_ls00;
    }
    pSvrCmd->cmdReqDat_ls00 = NULL; // remove at least the links
    pSvrCmd->cmdRpyDat_ls00 = NULL;

    delete pSvrCmd;
    pSvrCmd = NULL;

    return ErrCode;
}
