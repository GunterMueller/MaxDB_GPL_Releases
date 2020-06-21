/*
  -----------------------------------------------------------------------------
  module: vls10.cpp
  -----------------------------------------------------------------------------

  responsible:  SteffenS and d033893

  special area: SAP DB LOADER

  description:  dispatcher

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
#include "heo01.h"
#include "ToolsCommon/Tools_DynamicUTF8String.hpp"
#include "hls03.h"
#include "hls05.h"
#include "hls11.h"
#include "hls13.h"
#include "hls15.h"
#include "hls20_dataload.h"
#include "hls21.h"
#include "hls26.h"
#include "hls27_dataextract.h"
#include "hls28_TLoad.h"
#include "hls29_dataupdate.h"
#include "hls31_catalogextract.h"
#include "hls98msg.h"
#include "hls99.h"
#include "hls10.h"

#include <sys/timeb.h>
#include <stdlib.h>

/*!
  -----------------------------------------------------------------------------
  Chapter: Local function definition
  -----------------------------------------------------------------------------
 */

#ifndef WIN32
#else
static void diffTime (const char *transport, struct _timeb timeStart, struct _timeb timeStop)
{
  long result;

  result = timeStop.time - timeStart.time;
  result *= 1000;
  result += timeStop.millitm - timeStart.millitm;

  printf ("%s: %ld ms\n", transport, result );

}
static void fileTime (const char *transport, const char *str, FILETIME *runTime)
{
  SYSTEMTIME time;

  FileTimeToSystemTime (runTime, &time);

  if (time.wMinute != 0)
      printf ("%s: %s\t%d min %d.%d sec\n", transport, str, time.wMinute, time.wSecond, time.wMilliseconds);
  else
      printf ("%s: %s\t%d.%d sec\n", transport, str, time.wSecond, time.wMilliseconds);
}
#endif


static void 
ls10_FreeSchemaMapStruct(tls00_MultSchemaSpec* pMultSchemaSpec);
/*!
  -----------------------------------------------------------------------------

  function:     ls10_FreeColumnStruct

  -----------------------------------------------------------------------------

  description:  Pseudo destructor.
                

  arguments:    pMultColSpec [IN]   - tls00_MultColSpec to destroy
                pFileFormat  [IN]   - tls00_FileFormat* to destroy

  returnvalue:  None
  -----------------------------------------------------------------------------
*/
static void
ls10_FreeColumnStruct(tls00_MultColSpec* pMultColSpec, tls00_FileFormat*  pFileFormat);

/*!
  -----------------------------------------------------------------------------
  EndChapter: Local function definition
  -----------------------------------------------------------------------------
 */

/*!
  -----------------------------------------------------------------------------
  Chapter: Global function implementation
  -----------------------------------------------------------------------------
 */

/*
  -----------------------------------------------------------------------------
  function:     ls10ExecChangeUser
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls10ExecChangeUser(tls00_DBInfo *DBInfo, tls00_SessionInfo *Session, tsp00_Addr ErrText)
{
    ROUTINE_DBG_MEO00 ("ls10ExecChangeUser");

    LoaderMessages*     pMsgObj             = LoaderMessages::Instance();  
    tsp00_Int4          rc                  = errOK_els00;
    tls00_SessionInfo*  pOldSession         = NULL;
    tls00_SessionInfo*  pWorkingSession     = &DBInfo->dbiSession_ls00;
    bool                ReconnectAfterError = false; 

    //* 0. User Key
    if (NULL != Session->siUserKey_ls00) 
    {
        char pszTmpText[sizeof(tsp00_ErrText) + 1];

        rc = ls15SQLGetUser(Session, pszTmpText);
        if (errOK_els00 != rc)
        {
            pMsgObj->ls98Msg(laySQLConnect_els98, ErrText, errSqlConnectUserkey_els98, Session->siUserKey_ls00, pszTmpText);
            return rc;
        }
        else
        {
            delete [] Session->siUserKey_ls00;
            Session->siUserKey_ls00 = NULL;
        }
    }// end if (NULL != Session->siUserKey_ls00)

    //*  check the following parameters for equality:
    //*  1. Isolation level
    //*  2. Database  node
    //*  3. Database  name
    //*  4. User      name
    //*  5. Users     password
    //*  6. Schema    name
    SAPDB_Bool bEqualIsoLevel  = SAPDB_FALSE;
    SAPDB_Bool bEqualNodes     = SAPDB_FALSE;
    SAPDB_Bool bExecuteConnect = SAPDB_TRUE;
    
    if ( (SAPDB_TRUE == DBInfo->dbiSession_ls00.siDBConnection_ls00) &&
         (SAPDB_TRUE == DBInfo->dbiSession_ls00.siSqlStat_ls00) )
    {
        //*  1. Isolation level
        if (DBInfo->dbiSession_ls00.siDBSessionIsoLevel_ls00 == Session->siDBSessionIsoLevel_ls00)
            bEqualIsoLevel = SAPDB_TRUE;
        //*  2. Database  node
        if (SAPDB_TRUE == bEqualIsoLevel)
        {
            if ( (NULL == DBInfo->dbiSession_ls00.si_pDBNode_ls00) && (NULL == Session->si_pDBNode_ls00) )
                 bEqualNodes = SAPDB_TRUE;   
            else
            {
                if ( (NULL != DBInfo->dbiSession_ls00.si_pDBNode_ls00) && (NULL != Session->si_pDBNode_ls00) )
                {
                    // Database  node is case insensitive
                    if (0 == _tcsicmp(DBInfo->dbiSession_ls00.si_pDBNode_ls00, Session->si_pDBNode_ls00))
                        bEqualNodes = SAPDB_TRUE;
                }
                else
                {
                    tsp00_NodeIdc  tmpDBHost_c;
                    tsp00_ErrText  aErrText;
                    aErrText.SetBlank();
                    rc = ls03GetHostname(tmpDBHost_c, aErrText);
                    if (errOK_els00 == rc)
                    {
                        char *p = (NULL == Session->si_pDBNode_ls00) ? DBInfo->dbiSession_ls00.si_pDBNode_ls00 :Session->si_pDBNode_ls00;
                        if (0 == _tcsicmp(tmpDBHost_c.asCharp(), p))
                            bEqualNodes = SAPDB_TRUE;
                    }
                }
            }
        }// if (SAPDB_TRUE == bEqualIsoLevel)

        //*  4. User   name 
        //*  5  User   password
        //*  6. Schema name
        if (SAPDB_TRUE == bEqualNodes)
        {
            // Database name is case insensitive
            if (0 == _tcsicmp(DBInfo->dbiSession_ls00.siDBName_ls00, Session->siDBName_ls00))
            {
                // User name is case sensitive
                if (0 == _tcscmp(DBInfo->dbiSession_ls00.siUserName_ls00, Session->siUserName_ls00))
                {
                    // User password
                    if (0 == DBInfo->dbiSession_ls00.siUserCPwd_ls00.compare(Session->siUserCPwd_ls00))
                    {
                        bExecuteConnect = SAPDB_FALSE;
                        // Schema name
                        if (0 != _tcscmp( DBInfo->dbiSession_ls00.siSchemaName_ls00, Session->siSchemaName_ls00) ) 
                        {
                            SAPDB_strcpy(DBInfo->dbiSession_ls00.siSchemaName_ls00,Session->siSchemaName_ls00);
                            rc = ls15SwitchSchema(DBInfo, REINTERPRET_CAST(SAPDB_UTF8*, Session->siSchemaName_ls00), ErrText);
                        }
                    }
                }// User name is case sensitive
            }// Database name is case insensitive
        }//if (SAPDB_TRUE == bEqualNodes)

    }// if (   SAPDB_TRUE == DBInfo->dbiSession_ls00.siDBConnection_ls00) 
    else
    {
        //   isolation level or schema name was set
        if ( (NULL == Session->si_pDBNode_ls00) &&
             (0    == Session->siDBName_ls00.length()) &&
             (0    == Session->siUserName_ls00.length()) )
        {
            bExecuteConnect = SAPDB_FALSE;
        }
    } // if ( (false == DBInfo->dbiSession_ls00.siDBConnection_ls00) 

    if (SAPDB_TRUE == bExecuteConnect)
    {
        DBInfo->dbiEncoding_ls00    = ctAscii_els00;
        DBInfo->dbiOI_Encoding_ls00 = ctAscii_els00;

        //*
        //*     Save old connection if existing
        //*
        if (errOK_els00 != (rc = ls15CommitUser (DBInfo, ErrText)) )
        {
            // If no connection (errConnectNoServerDB_els98) nothing to save
            if (errConnectNoServerDB_els98 != rc)
            {
                return rc;      // error text is logged by ls15CommitUser
            }
        }
        else
        {
            pOldSession = new tls00_SessionInfo;            // User 'committed': save old session structure
            SAPDB_memcpy(pOldSession, pWorkingSession, sizeof (tls00_SessionInfo));

            // new user may not have db connection and may not be logged on
            Session->siDBConnection_ls00 = false;
            Session->siSqlStat_ls00      = false;
            ReconnectAfterError          = true;                     // restore if connection fails
        }

        // Copy new session struct into 'old' DBInfo-struct and overwrite old session struct
        SAPDB_memcpy (pWorkingSession, Session, sizeof (tls00_SessionInfo));

        //*
        //*     New connection
        //* 1. db connection; 2. user logon
        //*
        rc = ls15Connect(DBInfo, ErrText);
        if (errOK_els00 != rc)       // no connection; error is already logged
        {
            if (true == ReconnectAfterError)
            {
                // restore old session structure
                SAPDB_memcpy(pWorkingSession, pOldSession, sizeof (tls00_SessionInfo));

                rc = ls15Connect(DBInfo, ErrText);     // try to connect 'old' user
                if (errOK_els00 == rc)                           // success restoring old connection
                {
                    pMsgObj->ls98Msg( layUserConnect_els98, ErrText, errPreviousUserRestored_els98,
                                                                    pOldSession->siUserName_ls00.asCharp());
                    rc = errPreviousUserRestored_els98;
                }
                // else --> caller logs error
            }
            // else --> caller logs error
        }
        else        // log connected user and server db
        {
            if (true == pWorkingSession->siNolog_ls00)
            {
                pMsgObj->ls98PrintMessage(layUserConnect_els98, msgUserConnectionLog_els98,
                                                                pWorkingSession->siUserName_ls00.asCharp(),
                                                                DBInfo->dbiSession_ls00.siDBName_ls00.asCharp(),
                                                                DBInfo->dbiSession_ls00.siDBSessionIsoLevel_ls00,
                                                                "NOLOG");
            }
            else
            {
                pMsgObj->ls98PrintMessage(layUserConnect_els98, msgUserConnectionLog_els98,
                                                                pWorkingSession->siUserName_ls00.asCharp(),
                                                                DBInfo->dbiSession_ls00.siDBName_ls00.asCharp(),
                                                                DBInfo->dbiSession_ls00.siDBSessionIsoLevel_ls00,
                                                                " ");
            }
        }

        if (errOK_els00 == rc)
            rc = ls15LoadCodePage(DBInfo, ErrText);

        if (NULL != pOldSession)
        {
            if ( (NULL != pOldSession->si_pDBNode_ls00) &&
                 (errPreviousUserRestored_els98 != rc) && 
                 (pOldSession->si_pDBNode_ls00  != DBInfo->dbiSession_ls00.si_pDBNode_ls00) )
            {
                delete pOldSession->si_pDBNode_ls00;
                pOldSession->si_pDBNode_ls00 = NULL;
            }
            delete pOldSession;
        }
    }// end if (SAPDB_TRUE == bExecuteConnect)
    return rc;
}
//  ls10ExecChangeUser()

/*
  -----------------------------------------------------------------------------
  function:     ls10ExecSQLStmt
  -----------------------------------------------------------------------------
*/
int ls10ExecSQLStmt(tls00_Command *pLoaderCmd, tls00_DBInfo *pDBInfo)
{
    ROUTINE_DBG_MEO00 ("ls10ExecSQLStmt");

    tsp00_Int4  rc = errOK_els00;

    rc = ls15CheckConnection(pDBInfo, pLoaderCmd->cmdRpyDat_ls00);
    if (errOK_els00 == rc) {
        rc = ls13ExecExtSQLStmt(pDBInfo,REINTERPRET_CAST(tls00_DBSqlCmd*, pLoaderCmd->cmdScnDat_ls00), pLoaderCmd->cmdRpyDat_ls00);
        if (errOK_els00 != rc)  // The function returns the true SQL return code (kernel return code like -4008 or 100);
        {                       // the Loader returns its code for SQL errors to caller
            rc = errSimpleSQL_els98;
        }
    }
    return rc;
}
//  ls10ExecSQLStmt()

/*
  -----------------------------------------------------------------------------
  function:     ExecuteTransformationPackage 
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ExecuteTransformationPackage(tls00_Command *pLoaderCmd, tls00_DBInfo *pDBInfo)
{
    ROUTINE_DBG_MEO00 ("ExecuteTransformationPackage");

    LoaderMessages*  MsgInstance  = LoaderMessages::Instance();

    tsp00_Int4  rc = errOK_els00;

    //*
    //*   connected to db
    //*
    pLoaderCmd->cmdReqErr_ls00 = ls15CheckConnection(pDBInfo, pLoaderCmd->cmdRpyDat_ls00);
    if (errOK_els00 == pLoaderCmd->cmdReqErr_ls00)
    {
        tls00_TransformationCmd  * pTransformationCmd = REINTERPRET_CAST(tls00_TransformationCmd*, pLoaderCmd->cmdScnDat_ls00);
        MetaDataDef *pMetaDataDef  =  new MetaDataDef( pTransformationCmd, pDBInfo );
        
        pLoaderCmd->cmdReqErr_ls00 = pMetaDataDef->initResultSets(pTransformationCmd, pDBInfo, pLoaderCmd->cmdRpyDat_ls00);

        // Transformation and Relational Model
        if ( errOK_els00 == pLoaderCmd->cmdReqErr_ls00 )
        {
            switch (pLoaderCmd->cmdScanRC_ls00)
            {
                case srFctCreateSchemaMap_els00:
                {
                    pLoaderCmd->cmdReqErr_ls00 = ImportCatalogSchemaMap(pMetaDataDef, pDBInfo, pLoaderCmd->cmdRpyDat_ls00);
                    break;
                }
                case srFctDropSchemaMap_els00:
                {
                    pLoaderCmd->cmdReqErr_ls00 = ExportCatalogSchemaMap(pMetaDataDef, pDBInfo, pLoaderCmd->cmdRpyDat_ls00);
                    break;
                }
                case srFctUseSchemaMap_els00:
                {
                    pLoaderCmd->cmdReqErr_ls00 = UseCatalogSchemaMap   (pMetaDataDef, pDBInfo, pLoaderCmd->cmdRpyDat_ls00);
                    break;
                }
                case srFctCatExtract_els00:   
                case srFctCatLoad_els00:      
                {
                    pLoaderCmd->cmdReqErr_ls00 = TransportPackageCatalog(pMetaDataDef, pDBInfo, pLoaderCmd->cmdRpyDat_ls00);
                    break; 
                }
                case srFctPartExtract_els00:   
                case srFctTabExtract_els00:    
                {
                    pLoaderCmd->cmdReqErr_ls00 = ExportPackageData      (pMetaDataDef, pDBInfo, pLoaderCmd->cmdRpyDat_ls00);
                    break;
                }
                case srFctPartLoad_els00:      
                case srFctTabLoad_els00:       
                {
                    pLoaderCmd->cmdReqErr_ls00 = ImportPackageData      (pMetaDataDef, pDBInfo, pLoaderCmd->cmdRpyDat_ls00);
                    break;
                }
                case srFctColExtract_els00:    
                {
                    pLoaderCmd->cmdReqErr_ls00 = ExportPackageDataRecord(pMetaDataDef, pDBInfo, pLoaderCmd->cmdRpyDat_ls00);
                    break;
                }
                case srFctColLoad_els00:       
                {
                    pLoaderCmd->cmdReqErr_ls00 = ImportPackageDataRecord(pMetaDataDef, pDBInfo, pLoaderCmd->cmdRpyDat_ls00);
                    break;
                }
                case srFctColUpdate_els00:     
                {
                    pLoaderCmd->cmdReqErr_ls00 = UpdatePackageDataRecord(pMetaDataDef, pDBInfo, pLoaderCmd->cmdRpyDat_ls00);
                    break;
                }
                default:
                {
                    MsgInstance->ls98Msg(layBasicConnect_els98, pLoaderCmd->cmdRpyDat_ls00,errNotImplemented_els98,"Unknown Function");
                    pLoaderCmd->cmdReqErr_ls00 = errNotImplemented_els98;
                    break;
                }
            }//switch (pLoaderCmd->cmdScanRC_ls00)
        }// if ( pMetaDataDef->initResultSets(pTransformationCmd, pDBInfo) )
        if ( NULL != pMetaDataDef )
        {
            delete pMetaDataDef;
            pMetaDataDef = NULL;
        }
    }
    return pLoaderCmd->cmdReqErr_ls00;
}
//  ExecuteTransformationPackage()

/*
  -----------------------------------------------------------------------------
  function:     ls10TransformationPackageId
  -----------------------------------------------------------------------------
*/
void
ls10TransformationPackageId(tls00_Command *&pLoaderCmd, tls00_DBInfo *pDBInfo)
{
    ROUTINE_DBG_MEO00 ("ls10TransformationPackageId");

    LoaderMessages *MsgInstance  = LoaderMessages::Instance();    // Error reporting objekt
    GUID_           *GuidInstance = GUID_::Instance();

    GUID_IdString    packageidString;
    GUID_Id          packageid;

    switch (pLoaderCmd->cmdScanRC_ls00)
    {
        case srFctPartExtract_els00:
        case srFctPartLoad_els00:
        case srFctCatExtract_els00:
        case srFctCatLoad_els00:
        case srFctTabExtract_els00: 
        case srFctTabLoad_els00:  
        case srFctColExtract_els00: 
        case srFctColLoad_els00:
        case srFctColUpdate_els00: 
        {
            tls00_TransformationCmd *pTransformationCmd = REINTERPRET_CAST(tls00_TransformationCmd*, pLoaderCmd->cmdScnDat_ls00);
            if ( true == pTransformationCmd->trRestart_ls00.rstRestart_ls00 )
            {
                 pTransformationCmd->trPackageid_ls00  = pTransformationCmd->trRestart_ls00.rstPackageid_ls00;
            }
            else if( false == pTransformationCmd->trInternal_ls00 ) 
            {
                 GuidInstance->IdGetNext (packageid);
                 GuidInstance->IdAsString(packageid, packageidString);
                 pTransformationCmd->trRestart_ls00.rstPackageid_ls00.ConvertFromASCII_Latin1(packageidString,packageidString+_tcslen(packageidString));
                 
                 pTransformationCmd->trPackageid_ls00  = pTransformationCmd->trRestart_ls00.rstPackageid_ls00;
            }
            break;
        }
        default:
        {
            break;
        }
    }   // end switch (pLoaderCmd->cmdScanRC_ls00)
}
//  ls10TransformationPackageId()

/*
  -----------------------------------------------------------------------------
  function:     ls10ExecRemark
  -----------------------------------------------------------------------------
*/
void
ls10ExecRemark( tls00_Command *pLoaderCmd, tls00_DBInfo *pDBInfo) 
{
    ROUTINE_DBG_MEO00 ("ls10ExecRemark");
    LoaderMessages *pMsgObj = LoaderMessages::Instance();

    tls00_RemarkCmd*         pRemarkCmd = REINTERPRET_CAST(tls00_RemarkCmd*, pLoaderCmd->cmdScnDat_ls00);
    Tools_DynamicUTF8String  remark;
    remark.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, pRemarkCmd->dscRemark_ls00));

    if ( remark.Length() <= 1023 )
         pMsgObj->ls98PrintMessage(layCatalogExtract_els98, msgRemark_els98, remark.StrPtr() );
    else
    {
         remark = remark.SubStr(0,1022);
         pMsgObj->ls98PrintMessage(layCatalogExtract_els98, msgRemark_els98, remark.StrPtr() );
    }
    ls10FreeTransformation(srFctRemark_els00, pLoaderCmd->cmdScnDat_ls00);
}
//  ls10ExecRemark

/*
  -----------------------------------------------------------------------------
  function:     ls10FreeTransformation
  -----------------------------------------------------------------------------
*/
void
ls10FreeTransformation( tls00_TransformationCmd* pTransformationCmd )
{
    ROUTINE_DBG_MEO00 ("ls10FreeTransformation");
    if ( NULL != pTransformationCmd )
     {
         int i = 0;
         // schemamap
         ls10_FreeSchemaMapStruct(&pTransformationCmd->trSchemaMap_ls00);
         // part
         if (NULL != pTransformationCmd->trPartSpec_ls00.psFullTabName_ls00)
         {
             delete [] pTransformationCmd->trPartSpec_ls00.psFullTabName_ls00;
             pTransformationCmd->trPartSpec_ls00.psFullTabName_ls00 = NULL;
         }
         if (NULL != pTransformationCmd->trTable_ls00.tsFullTabName_ls00)
         {
             delete [] pTransformationCmd->trTable_ls00.tsFullTabName_ls00;
             pTransformationCmd->trTable_ls00.tsFullTabName_ls00 = NULL;
         }
         // streams
         if (NULL != pTransformationCmd->trConfigurationFile_ls00.fsFileName_ls00)
         {
             delete [] pTransformationCmd->trConfigurationFile_ls00.fsFileName_ls00;
             pTransformationCmd->trConfigurationFile_ls00.fsFileName_ls00 = NULL;
         }
         if (NULL != pTransformationCmd->trCatalogFile_ls00.fsFileName_ls00)
         {
             delete [] pTransformationCmd->trCatalogFile_ls00.fsFileName_ls00;
             pTransformationCmd->trCatalogFile_ls00.fsFileName_ls00 = NULL;
         }
         if (NULL != pTransformationCmd->trDataFile_ls00.fsFileName_ls00)
         {
             delete [] pTransformationCmd->trDataFile_ls00.fsFileName_ls00;
             pTransformationCmd->trDataFile_ls00.fsFileName_ls00 = NULL;
         }
         if (NULL != pTransformationCmd->trPackageFile_ls00.fsFileName_ls00)
         {
             delete [] pTransformationCmd->trPackageFile_ls00.fsFileName_ls00;
             pTransformationCmd->trPackageFile_ls00.fsFileName_ls00 = NULL;
         }
         if (NULL != pTransformationCmd->trCmdFile_ls00.fsFileName_ls00)
         {
             delete [] pTransformationCmd->trCmdFile_ls00.fsFileName_ls00;
             pTransformationCmd->trCmdFile_ls00.fsFileName_ls00 = NULL;
         }
         if (NULL != pTransformationCmd->trStatisticFile_ls00)
         {
             if (NULL != pTransformationCmd->trStatisticFile_ls00->fsFileName_ls00)
             {
                 delete [] pTransformationCmd->trStatisticFile_ls00->fsFileName_ls00;
                 pTransformationCmd->trStatisticFile_ls00->fsFileName_ls00 = NULL;
             }
         }
         // columns
         ls10_FreeColumnStruct(&pTransformationCmd->trColSpec_ls00, &pTransformationCmd->trDataFile_ls00.fsFormat_ls00);
         if (NULL != pTransformationCmd->trTable_ls00.tsCondSpec_ls00)
         {
             if (NULL != pTransformationCmd->trTable_ls00.tsCondSpec_ls00->cosCond_ls00)
             {
                 delete pTransformationCmd->trTable_ls00.tsCondSpec_ls00->cosCond_ls00;
                 pTransformationCmd->trTable_ls00.tsCondSpec_ls00->cosCond_ls00 = NULL;
             }

             if (NULL != pTransformationCmd->trTable_ls00.tsCondSpec_ls00->cosLeft_ls00)
             {
                 if (NULL != pTransformationCmd->trTable_ls00.tsCondSpec_ls00->cosLeft_ls00->cosCond_ls00)
                 {
                     delete pTransformationCmd->trTable_ls00.tsCondSpec_ls00->cosLeft_ls00->cosCond_ls00;
                     pTransformationCmd->trTable_ls00.tsCondSpec_ls00->cosLeft_ls00->cosCond_ls00 = NULL;
                 }

                 delete pTransformationCmd->trTable_ls00.tsCondSpec_ls00->cosLeft_ls00;
                 pTransformationCmd->trTable_ls00.tsCondSpec_ls00->cosLeft_ls00 = NULL;
             }

             if (NULL != pTransformationCmd->trTable_ls00.tsCondSpec_ls00->cosRight_ls00)
             {
                 if (NULL != pTransformationCmd->trTable_ls00.tsCondSpec_ls00->cosRight_ls00->cosCond_ls00)
                 {
                     delete pTransformationCmd->trTable_ls00.tsCondSpec_ls00->cosRight_ls00->cosCond_ls00;
                     pTransformationCmd->trTable_ls00.tsCondSpec_ls00->cosRight_ls00->cosCond_ls00 = NULL;
                 }

                 delete pTransformationCmd->trTable_ls00.tsCondSpec_ls00->cosRight_ls00;
                 pTransformationCmd->trTable_ls00.tsCondSpec_ls00->cosRight_ls00 = NULL;
             }
         }
         if (0 != pTransformationCmd->trColLongSpec_ls00.mfsCount_ls00)
         {
             for (i=0; i < pTransformationCmd->trColLongSpec_ls00.mfsCount_ls00; ++i)
             {
                 if (NULL != pTransformationCmd->trColLongSpec_ls00.mfsFiles_ls00[i]->fsFileName_ls00)
                 {
                     delete [] pTransformationCmd->trColLongSpec_ls00.mfsFiles_ls00[i]->fsFileName_ls00;
                     pTransformationCmd->trColLongSpec_ls00.mfsFiles_ls00[i]->fsFileName_ls00 = NULL;
                 }
                 if (NULL != pTransformationCmd->trColLongSpec_ls00.mfsFiles_ls00[i]->fsLongColName_ls00)
                 {
                     delete [] pTransformationCmd->trColLongSpec_ls00.mfsFiles_ls00[i]->fsLongColName_ls00;
                     pTransformationCmd->trColLongSpec_ls00.mfsFiles_ls00[i]->fsLongColName_ls00 = NULL;
                 }
                 delete pTransformationCmd->trColLongSpec_ls00.mfsFiles_ls00[i];
             }
         }
         if (NULL != pTransformationCmd->trFullUserInput_ls00)
         {
             delete [] pTransformationCmd->trFullUserInput_ls00;
             pTransformationCmd->trFullUserInput_ls00 = NULL;
         }

         if (NULL != pTransformationCmd->trSelOrdStr_ls00)
         {
             delete [] pTransformationCmd->trSelOrdStr_ls00;
             pTransformationCmd->trSelOrdStr_ls00 = NULL;
         }
         // transformation
         delete pTransformationCmd;
         pTransformationCmd = NULL;

     }// if ( NULL != pTransformationCmd )
}// ls10FreeTransformation

/*
  -----------------------------------------------------------------------------
  function:     ls10FreeTransformation
  -----------------------------------------------------------------------------
*/
void
ls10FreeTransformation(tls00_ScanResult Type, char * &ppLoaderCmd)
{
    ROUTINE_DBG_MEO00 ("ls10FreeTransformation");

    if ( NULL != ppLoaderCmd )
    {
        switch (Type)
        {
            case srFctPartExtract_els00:
            case srFctPartLoad_els00:
            case srFctCatExtract_els00:
            case srFctCatLoad_els00:
            case srFctTabExtract_els00: 
            case srFctTabLoad_els00:  
            case srFctColExtract_els00: 
            case srFctColLoad_els00:
            case srFctColUpdate_els00: 
            {
                tls00_TransformationCmd* pTransformationCmd = REINTERPRET_CAST(tls00_TransformationCmd*, ppLoaderCmd);
                if ( NULL != pTransformationCmd ) {
                     ls10FreeTransformation( pTransformationCmd );
                }
                break;
            }
            case srFctRemark_els00:
            {
                tls00_RemarkCmd* pRemarkCmd = REINTERPRET_CAST(tls00_RemarkCmd*, ppLoaderCmd);

                if ( NULL != pRemarkCmd )
                {
                    if (NULL != pRemarkCmd->dscRemark_ls00)
                    {
                        delete [] pRemarkCmd->dscRemark_ls00;
                        pRemarkCmd->dscRemark_ls00 = NULL;
                    }

                    delete pRemarkCmd;
                    pRemarkCmd = NULL;
                }   // end if ( NULL != pRemark )
                break;
            }
        }   // end switch (Type)
    }   // end if ( NULL != ppLoaderCmd )
    return;
}
//  ls10FreeTransformation()

/*
  -----------------------------------------------------------------------------
  function:     ls10_FreeSchemaMapStruct
  -----------------------------------------------------------------------------
*/
static void
ls10_FreeSchemaMapStruct(tls00_MultSchemaSpec* pMultSchemaSpec)
{
    if (NULL != pMultSchemaSpec) {
        for(tsp00_Int4 i = 0; i < pMultSchemaSpec->msCount_ls00; ++i) {
            
            tls00_SchemaMap *pSchemaMap = pMultSchemaSpec->msSchemaMap_ls00[i];

            if ( NULL != pSchemaMap ) {
                 delete pSchemaMap;
                 pSchemaMap = NULL;
            }
        }
    }
}
/*
  -----------------------------------------------------------------------------
  function:     ls10_FreeColumnStruct
  -----------------------------------------------------------------------------
*/
static void
ls10_FreeColumnStruct(tls00_MultColSpec* pMultColSpec, tls00_FileFormat*  pFileFormat)
{
    if (NULL != pMultColSpec)
    {
        for (tsp00_Int4 i = 0; i < pMultColSpec->mcsCount_ls00; ++i)
        {
            tls00_Column *pCol = pMultColSpec->mcsColumn_ls00[i];

            if ( NULL != pCol )
            {
                // LONGFILE info in tls00_Column structure
                if ( (true  == pCol->colLongInfo.loiIsLong_ls00) &&
                     (false == pFileFormat->ffoCompact_ls00) )
                {
                    if (NULL != pCol->colLongInfo.loiFileName_ls00)
                    {
                        delete [] pCol->colLongInfo.loiFileName_ls00;
                        pCol->colLongInfo.loiFileName_ls00 = NULL;
                    }

                    if (NULL != pCol->colLongInfo.loiFileStruct_ls00)
                    {
                        delete pCol->colLongInfo.loiFileStruct_ls00;
                        pCol->colLongInfo.loiFileStruct_ls00 = NULL;
                    }
                }
                // Assignment values
                if (NULL != pCol->colAssign_ls00.fasAsgnVal_ls00)
                {
                    delete [] pCol->colAssign_ls00.fasAsgnVal_ls00;
                    pCol->colAssign_ls00.fasAsgnVal_ls00 = NULL;
                }

                if (NULL != pCol->colNullCond_ls00)        // RECURSION REQUIRED
                {
                    if (NULL != pCol->colNullCond_ls00->cosCond_ls00)
                    {
                        if (NULL != pCol->colNullCond_ls00->cosCond_ls00->conCompVal_ls00)
                        {
                            // in case of ascii or utf8 files the null spec pointer points to
                            // null spec in file format and MUST NOT BE DELETED
                            if (pFileFormat->ffoNullSpec_ls00 !=
                                pCol->colNullCond_ls00->cosCond_ls00->conCompVal_ls00)
                            {
                                delete [] pCol->colNullCond_ls00->cosCond_ls00->conCompVal_ls00;
                            }
                            pCol->colNullCond_ls00->cosCond_ls00->conCompVal_ls00 = NULL;
                        }
                        if (NULL != pCol->colNullCond_ls00->cosCond_ls00->conBinCompVal_ls00)
                        {
                            delete [] STATIC_CAST(char*, pCol->colNullCond_ls00->cosCond_ls00->conBinCompVal_ls00);
                            pCol->colNullCond_ls00->cosCond_ls00->conBinCompVal_ls00 = NULL;
                        }
                        pCol->colNullCond_ls00->cosCond_ls00->conValLen_ls00 = 0;


                        delete pCol->colNullCond_ls00->cosCond_ls00;
                        pCol->colNullCond_ls00->cosCond_ls00 = NULL;
                    }
                    delete pCol->colNullCond_ls00;
                    pCol->colNullCond_ls00 = NULL;
                }

                memset(pCol, 0, sizeof(tls00_Column));
                delete pCol;
                pCol = NULL;

            }   // end if ( NULL != pCol )
        }   // end for (tsp00_Int4 i = 0; i < pMultColSpec->mcsCount_ls00; ++i)
    }   // end if (NULL != pMultColSpec)
}
// ls10_FreeColumnStruct()

/*===========================================================================*
 *     END
 *===========================================================================*/

