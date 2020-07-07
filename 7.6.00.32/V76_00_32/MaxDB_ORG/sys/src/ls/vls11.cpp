/*
  -----------------------------------------------------------------------------
  module: vls11.cpp
  -----------------------------------------------------------------------------
  responsible:  SteffenS and d033893

  special area: SAP DB LOADER

  description:  Dispatcher module

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
#include "hls98msg.h"
#include "hls05.h"
#include "hls11.h"
#include "hls10.h"
#include "hls14_scan.h"     // parser and scanner
#include "hls14_parse.h"
#include "hls15.h"
#include "hls99.h"

/*
  -----------------------------------------------------------------------------
  function:     ls11ParseLoadCommand
  -----------------------------------------------------------------------------
*/
int ls11ParseLoadCommand(tls00_DBInfo  *pDBInfo,
                         tls00_Command *& SvrCmd  )
{
    ROUTINE_DBG_MEO00 ("ls11ParseLoadCommand");

    LoaderMessages *pMsgObj = LoaderMessages::Instance();

    tsp00_Int4  ErrCode = errOK_els00;
    
    //*
    //* Convert incoming statements to UTF8 if necessary
    //*
    char* pKeepPtrToRequestData = NULL;
    char* pNewCmdReqDat = NULL;

    unsigned int charLength       = 0;
    unsigned int byteLength       = 0;
    int          isTerminated     = 0;
    int          isCorrupted      = 0;
    int          isExhausted      = 0;
    int          isCorruptedUTF8  = 0;

    size_t lStmtLen = _tcslen(SvrCmd->cmdReqDat_ls00);

    isCorruptedUTF8 = sp77encodingUTF8->stringInfo(SvrCmd->cmdReqDat_ls00,
                                                   lStmtLen,
                                                   1,
                                                   &charLength,
                                                   &byteLength,
                                                   &isTerminated,
                                                   &isCorrupted,
                                                   &isExhausted);
    if (0 != isCorruptedUTF8)
    {
        char szErrText[MAX_REPLY_LENGTH_LS00];
        pKeepPtrToRequestData = SvrCmd->cmdReqDat_ls00;

        tsp00_Longuint  lStmtLenUTF8 = 6*lStmtLen;
        tls00_CodeType  DestCode    = ctAscii_els00;

        Tools_DynamicUTF8String s;
        pNewCmdReqDat = new _TCHAR[lStmtLenUTF8];

        SAPDB_Int4 rc = ls05ConvertToUTF8(s,
                                          REINTERPRET_CAST(unsigned char*, SvrCmd->cmdReqDat_ls00),
                                          lStmtLen,
                                          DestCode,
                                          szErrText);
        if (errOK_els00 == rc)
        {
            size_t len = s.BasisSize();
            SAPDB_memcpy(pNewCmdReqDat, s.StrPtr(), len);
            pNewCmdReqDat[len] = 0;
            SvrCmd->cmdReqDat_ls00 = pNewCmdReqDat;
        }
    }


    // initialize parser
    LoaderParser *Parser = new LoaderParser(SvrCmd, pDBInfo);
    if (Parser == NULL)
    {
        return errYaccAlloc_els00;
    }
    ErrCode = Parser->ls14GetErrorCode();
    if (errOK_els00 != ErrCode)
    {
        pMsgObj->ls98EFromRcToFile(layDispatch_els98, SvrCmd->cmdRpyDat_ls00,
                                                      STATIC_CAST(ErrorNumbers, ErrCode) );

        return ErrCode;
    }
    //*
    //* parse load command
    //*
    void *MyStruct = NULL;

    Parser->StartParsing(MyStruct);
    ErrCode = Parser->ls14GetErrorCode();

    delete Parser;

    // Delete conversion strings
	if (NULL != pKeepPtrToRequestData)
	{
		delete [] pNewCmdReqDat;
		pNewCmdReqDat = NULL;

		SvrCmd->cmdReqDat_ls00 = pKeepPtrToRequestData;
		pKeepPtrToRequestData = NULL;
	}


    if (errOK_els00 != ErrCode)
    {
        pMsgObj->ls98EFromRcToFile(layDispatch_els98, SvrCmd->cmdRpyDat_ls00,
                                                      STATIC_CAST(ErrorNumbers, ErrCode) );
        return ErrCode;
    }

    if (errOK_els00 != SvrCmd->cmdReqErr_ls00)
    {
        pMsgObj->ls98EFromRcToFile(layDispatch_els98, SvrCmd->cmdRpyDat_ls00,
                                                      STATIC_CAST(ErrorNumbers, SvrCmd->cmdReqErr_ls00) );

        ErrCode = SvrCmd->cmdReqErr_ls00;
    }

    return ErrCode;

}
// ls11ParseLoadCommand


 /*
  -----------------------------------------------------------------------------
  function:     ls11ParseDispatchLoadCommand - overloaded
  -----------------------------------------------------------------------------
*/
int
ls11ParseDispatchLoadCommand(Tools_DynamicUTF8String   SvrCmdString,
                             tls00_DBInfo             *pDBInfo,
                             tsp00_Addr                pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls11ParseDispatchLoadCommand I");

    LoaderMessages *pMsgObj = LoaderMessages::Instance();

    tsp00_Int4  rc = errOK_els00;

	if ( SvrCmdString.Empty() == SAPDB_FALSE )
	{
		// initialize command structure thats used in the server
		tls00_Command * pSvrCmd = new tls00_Command;
		memset (pSvrCmd, DEF_BYTE_LS00, sizeof  (tls00_Command));

		// initialize the request string

		// The following assignment places already a NULL at the end of the string
		pSvrCmd->cmdReqDat_ls00 = CONST_CAST(SAPDB_Char*, SvrCmdString.CharPtr());

		// initialize the answer string
		pSvrCmd->cmdRpyDat_ls00 = new char[MAX_REPLY_LENGTH_LS00];
		memset(pSvrCmd->cmdRpyDat_ls00, DEF_BYTE_LS00, MAX_REPLY_LENGTH_LS00);

		// initialize logging behaviour
		pSvrCmd->cmdLogging_ls00  = SAPDB_FALSE;
		pSvrCmd->cmdInternal_ls00 = SAPDB_TRUE;

		ls11ParseDispatchLoadCommand(pSvrCmd, pDBInfo);

		rc = pSvrCmd->cmdReqErr_ls00 ;

		if ( NULL != pSvrCmd->cmdRpyDat_ls00 )
		{
			// Before deleting any error text save it for returning
			size_t len = _tcslen(pSvrCmd->cmdRpyDat_ls00);
			if (0 != len)
			{
				pszErrText[0] = 0;      // initialize in any case
				_tcscpy(pszErrText, pSvrCmd->cmdRpyDat_ls00);
			}

			delete[] pSvrCmd->cmdRpyDat_ls00;
			pSvrCmd->cmdRpyDat_ls00 = NULL;
		}

		if ( NULL != pSvrCmd )
		{
			delete pSvrCmd;
			pSvrCmd = NULL;
		}
	}

    return rc;
}
// ls11ParseDispatchLoadCommand


/*
  -----------------------------------------------------------------------------
  function:     ls11ParseDispatchLoadCommand - overloaded
  -----------------------------------------------------------------------------
*/
int
ls11ParseDispatchLoadCommand(tls00_Command *&SvrCmd, tls00_DBInfo *DBInfo)
{
    ROUTINE_DBG_MEO00 ("ls11ParseDispatchLoadCommand II");

    LoaderMessages *pMsgObj = LoaderMessages::Instance();

    tsp00_Int4  ErrCode = errOK_els00;

    //*
    //* Convert incoming statements to UTF8 if necessary
    //*
    char* pKeepPtrToRequestData = NULL;
    char* pNewCmdReqDat = NULL;

    unsigned int charLength       = 0;
    unsigned int byteLength       = 0;
    int          isTerminated     = 0;
    int          isCorrupted      = 0;
    int          isExhausted      = 0;
    int          isCorruptedUTF8  = 0;

    size_t lStmtLen = _tcslen(SvrCmd->cmdReqDat_ls00);

    isCorruptedUTF8 = sp77encodingUTF8->stringInfo(SvrCmd->cmdReqDat_ls00,
                                                   lStmtLen,
                                                   1,
                                                   &charLength,
                                                   &byteLength,
                                                   &isTerminated,
                                                   &isCorrupted,
                                                   &isExhausted);
    if (0 != isCorruptedUTF8)
    {
        char szErrText[MAX_REPLY_LENGTH_LS00];
        pKeepPtrToRequestData = SvrCmd->cmdReqDat_ls00;

        tsp00_Longuint  lStmtLenUTF8 = 6*lStmtLen;
        tls00_CodeType  DestCode    = ctAscii_els00;

        Tools_DynamicUTF8String s;
        pNewCmdReqDat = new _TCHAR[lStmtLenUTF8];

        SAPDB_Int4 rc = ls05ConvertToUTF8(s,
                                          REINTERPRET_CAST(unsigned char*, SvrCmd->cmdReqDat_ls00),
                                          lStmtLen,
                                          DestCode,
                                          szErrText);
        if (errOK_els00 == rc)
        {
            size_t len = s.BasisSize();
            SAPDB_memcpy(pNewCmdReqDat, s.StrPtr(), len);
            pNewCmdReqDat[len] = 0;
            SvrCmd->cmdReqDat_ls00 = pNewCmdReqDat;
        }
    }

    // initialize parser
    LoaderParser *Parser = new LoaderParser(SvrCmd, DBInfo);
    if (Parser == NULL)
    {
        return errYaccAlloc_els00;
    }
    // parser generates scanner -> successful ?
    ErrCode = Parser->ls14GetErrorCode();
    if (errOK_els00 != ErrCode)
    {
        pMsgObj->ls98EFromRcToFile(layDispatch_els98, SvrCmd->cmdRpyDat_ls00,
                                                      STATIC_CAST(ErrorNumbers, ErrCode) );
    }

    //*
    //* parse load command
    //*
    //* startParsing will always return OK! One must check the error value using
    //* the Parser member function ls14GetErrorCode.
    //*
    void *ScanReturnType = NULL;

    Parser->StartParsing(ScanReturnType);
    ErrCode = Parser->ls14GetErrorCode();

    delete Parser;


    //*
    //*     Log command (excluding user user , remark command) and starting time
    //*
    if (sfRelease_els00   != SvrCmd->cmdSubFunc_ls00 &&
        srFctRemark_els00 != SvrCmd->cmdScanRC_ls00  &&
        SAPDB_FALSE       != SvrCmd->cmdLogging_ls00  )
    {
        tsp00_Date  Date;
        tsp00_Time  Time;
        sqldattime(Date, Time);
        tsp00_Datec szDate;
        tsp00_Timec szTime;
        szDate.p2c(Date);
        szTime.p2c(Time);

        pMsgObj->ls98PrintMessage(layDispatch_els98, msgStartCommand_els98, szDate.asCharp(), szTime.asCharp());
        pMsgObj->ls98PrintMessage(layDispatch_els98, msgSimpleMsg_els98, SvrCmd->cmdReqDat_ls00);
    }

    // Delete conversion strings
	if (NULL != pKeepPtrToRequestData)
	{
		delete [] pNewCmdReqDat;
		pNewCmdReqDat = NULL;

		SvrCmd->cmdReqDat_ls00 = pKeepPtrToRequestData;
		pKeepPtrToRequestData = NULL;
	}

    // Trace 'not loggable' statements at least
    if (SAPDB_FALSE == SvrCmd->cmdLogging_ls00)
    {
        TRACE_PRNF_MLS99(("ls11ParseDispatchLoadCommand II", "Parsed internal statement: '%s'\n", SvrCmd->cmdReqDat_ls00));
    }

    if (errOK_els00 != ErrCode)
    {
        pMsgObj->ls98EFromRcToFile(layDispatch_els98, SvrCmd->cmdRpyDat_ls00,
                                                      STATIC_CAST(ErrorNumbers, ErrCode) );
        SvrCmd->cmdReqErr_ls00 = ErrCode    ;
    }
    else
    {
        if (errOK_els00 > SvrCmd->cmdReqErr_ls00)
        {
            pMsgObj->ls98EFromRcToFile(layDispatch_els98, SvrCmd->cmdRpyDat_ls00,
                                                          STATIC_CAST(ErrorNumbers, SvrCmd->cmdReqErr_ls00) );
        }
        else
        {
            if (errOK_els00 < SvrCmd->cmdReqErr_ls00)
            {
                pMsgObj->ls98EFromRcToFile(layDispatch_els98, SvrCmd->cmdRpyDat_ls00,
                                                              STATIC_CAST(ErrorNumbers, SvrCmd->cmdReqErr_ls00) );

                SvrCmd->cmdReqErr_ls00 = errOK_els00;
                SvrCmd->cmdRpyDat_ls00[0] = 0;
            }
            //*
            //*  dispatcher
            //*
            switch (SvrCmd->cmdScanRC_ls00)
            {
                case srFctRemark_els00:
                {
                     ls10ExecRemark(SvrCmd, DBInfo);
                     break;
                }
                case srFctSqlStmt_els00:
                {
                    SvrCmd->cmdReqErr_ls00    = ls10ExecSQLStmt(SvrCmd, DBInfo);
                    break;
                }
                case srCmdSession_els00:
                {
                    if (NULL != ScanReturnType) 
                    {
                        SvrCmd->cmdReqErr_ls00 = ls10ExecChangeUser(DBInfo,
                                                                    STATIC_CAST(tls00_SessionInfo*, ScanReturnType),
                                                                    SvrCmd->cmdRpyDat_ls00);
                        delete STATIC_CAST(tls00_SessionInfo*, ScanReturnType);
                        ScanReturnType = NULL;
                    }

                    // In case of switching AUTOCOMMIT to ON we'll send 'COMMIT WORK' to the kernel
                    if (sfAutoCommitON_els00 == SvrCmd->cmdSubFunc_ls00)
                    {
                        ls15RSCommit(DBInfo, SvrCmd->cmdRpyDat_ls00);
                    }

                    break;
                }
                case srRelease_els00:
                {
                    ls15CommitUser(DBInfo, SvrCmd->cmdRpyDat_ls00);
                    break;
                }
                case srFctCreateSchemaMap_els00:
                case srFctUseSchemaMap_els00:
                case srFctDropSchemaMap_els00:
                case srFctPartExtract_els00 :
                case srFctPartLoad_els00 :
                case srFctDBExtr_els00 :
                case srFctDBLoad_els00 :
                case srFctCatExtract_els00:
                case srFctCatLoad_els00:
                case srFctTabExtract_els00:
                case srFctTabLoad_els00:
                case srFctColExtract_els00:
                case srFctColLoad_els00:
                case srFctColUpdate_els00 :
                {
                    SvrCmd->cmdReqErr_ls00 =  ExecuteTransformationPackage(SvrCmd, DBInfo); 
                    break;
                }
            }

            //*
            //*     Traceausgaben
            //*
            if (SvrCmd->cmdReqErr_ls00 != errOK_els00)
            {
                TRACE_PRNF_MLS99(("ls11ParseDispatchLoadCommand", "Error executing command '%s' (Error: %s).\n",
                                                                  SvrCmd->cmdReqDat_ls00,
                                                                  SvrCmd->cmdRpyDat_ls00));
            }
            else
            {
                TRACE_PRNF_MLS99(("ls11ParseDispatchLoadCommand", "Info: Successful execution of '%s'.\n",
                                                                  SvrCmd->cmdReqDat_ls00));
            }
        }
    }

    return ErrCode;
}
//  ls11ParseDispatchLoadCommand()

/*===========================================================================*
 *     END
 *===========================================================================*/
