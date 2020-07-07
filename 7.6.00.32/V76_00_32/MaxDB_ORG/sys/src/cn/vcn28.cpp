
/*!
  -----------------------------------------------------------------------------

  module: vcn28.cpp

  -----------------------------------------------------------------------------

  responsible:  BerndV

  special area: DBMServer Parameter File Migration

  description:  DBMServer Parameter File Migration - Implementation

  version:      7.4.*

  -----------------------------------------------------------------------------

  Copyright (c) 2001-2005 SAP AG

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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
  -------------------------------------------------------------------------
  includes
  -------------------------------------------------------------------------
 */

#include <stdlib.h>

#include "hsp100.h"
#include "hcn28.h"
#include "hcn29.h"
#include "hcn30.h"
#include "hcn42.h"
#include "hcn51.h"
#include "hcn90.h"

#include "KernelCommon/ParameterNames/KernelParam_IOManager.hpp"
#include "KernelCommon/ParameterNames/KernelParam_Converter.hpp"
#include "KernelCommon/ParameterNames/KernelParam_SharedSQL.hpp"

#include "SAPDB/DBM/Srv/DBMSrv_Reply.hpp"
#include "DBM/Srv/Deprecated/DBMSrvDep_DirtyReply.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_RTEError.hpp"

/*
  -------------------------------------------------------------------------
  function:     cn28_FindParamNew
  purpose:      param search
  -------------------------------------------------------------------------
 */
static DBMSrvPar_Parameter * cn28_FindParamNew
      ( DBMSrvPar_Parameters * pXParamData,
        const char        * szNewName )
{
  tcn00_Error                  nFuncReturn = OK_CN00;
  DBMSrvPar_Parameter          * pParameter  = NULL;
  DBMSrvPar_Parameter          * pDynHelp    = NULL;

  pParameter = pXParamData->Get(szNewName);
  if (pParameter == NULL) {

    /* No match -> check for dynamic */
    pDynHelp = cn29SearchDynamic(pXParamData->pFirst, szNewName);
    if (pDynHelp != NULL) {

      pParameter = new DBMSrvPar_Parameter;
      if (pParameter != NULL) {
        /* copy data from dynamic record */
        nFuncReturn = cn29CopyParameter(pParameter, pDynHelp);
        /* clear the dynamic/intern property */
        pParameter->xpProperties[XPPropClassDynamic] = DBMSrvPar_Parameter::XPPropValueNo;
        pParameter->xpProperties[XPPropClassIntern] = DBMSrvPar_Parameter::XPPropValueNo;

       SAPDB_strcpy(pParameter->szParameterName, szNewName);

        /* list management (insert after dynamic) */
        pParameter->pNext = pDynHelp->pNext;
        pDynHelp->pNext   = pParameter;
        pXParamData->Put(pParameter);
      } // end if

    } // end if
  } // end if

  return pParameter;
} // end cn28_FindParamNew
 
/*
  -------------------------------------------------------------------------
  function:     cn28_CopyParamInteger
  purpose:      param copy
  -------------------------------------------------------------------------
 */
static tcn00_Error cn28_CopyParamInteger
      ( tsp00_DbNamec       szDbName,
        DBMSrvPar_Parameters * pXParamData,
        DBMSrvPar_RTEConfParameter & oConfigFile,
        const char        * szOldName,
        const char        * szNewName,
        char              * replyData,
        int               * replyLen,
        bool                bIgnoreNotFound)
{
  FUNCTION_DBG_MCN00_1("cn28_CopyParamInteger");

  tcn00_Error                  nFuncReturn = OK_CN00;
  DBMSrvPar_Parameter          * pParameter  = NULL;
  DBMSrvPar_RTEConfParameter::Integer   nValue      = 0;
  DBMSrvMsg_Error              oMsg;

  // 
  if (nFuncReturn == OK_CN00) {
    if (!oConfigFile.GetValue ((const SAPDB_UTF8 *) szOldName, nValue, oMsg)) {
      if (!bIgnoreNotFound) {
        nFuncReturn = DBMSrvDep_DirtyReply(replyData, replyLen).startWithMessageList(oMsg);
      } // end if
    } else {
      pParameter = cn28_FindParamNew(pXParamData, szNewName);
      if (pParameter != NULL) {
        pParameter->xpRecord.xpValue.xpValueInt = nValue;
        if (!oConfigFile.SetValue ((const SAPDB_UTF8 *) pParameter->szParameterName.asCharp(), 
                                   pParameter->xpRecord.xpValue.xpValueInt, 
                                   (pParameter->xpProperties[XPPropClassChange] == DBMSrvPar_Parameter::XPPropValueRunning), 
                                   true,
                                   oMsg)) {
          nFuncReturn = DBMSrvDep_DirtyReply(replyData, replyLen).startWithMessageList(oMsg);
        } else {
          cn29ParamValueToString(pParameter->szNewValue, &(pParameter->xpRecord));
          cn29WriteParamHistory(szDbName, pParameter, PAHI_CHANGE_CN002);
        } // end if
      } else {
        nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_XPNOTFOUND_CN00);
      } // end if
    } // end if
  } // end if

  return nFuncReturn;
} // end cn28_CopyParamInteger

/*
  -------------------------------------------------------------------------
  function:     cn28_CopyParamString
  purpose:      param copy
  -------------------------------------------------------------------------
 */
static tcn00_Error cn28_CopyParamString
      ( tsp00_DbNamec       szDbName,
        DBMSrvPar_Parameters * pXParamData,
        DBMSrvPar_RTEConfParameter & oConfigFile,
        const char        * szOldName,
        const char        * szNewName,
        char              * replyData,
        int               * replyLen,
        bool                bIgnoreNotFound)
{
  FUNCTION_DBG_MCN00_1("cn28_CopyParamString");

  tcn00_Error                  nFuncReturn = OK_CN00;
  DBMSrvPar_Parameter          * pParameter  = NULL;
  DBMSrvMsg_Error              oMsg;
  DBMSrvPar_RTEConfParameter::StringElement szValue[RTECONF_MAXSTRINGLENGTH + 1];

  // 
  if (nFuncReturn == OK_CN00) {
    if (!oConfigFile.GetValue ((const SAPDB_UTF8 *) szOldName, szValue, oMsg)) {
      if (!bIgnoreNotFound) {
        nFuncReturn = DBMSrvDep_DirtyReply(replyData, replyLen).startWithMessageList(oMsg);
      } // end if
    } else {
      pParameter = cn28_FindParamNew(pXParamData, szNewName);
      if (pParameter != NULL) {
        SAPDB_strcpy((char *) pParameter->xpRecord.xpValue.xpValueString, (char *) szValue);
        if (!oConfigFile.SetValue ((const SAPDB_UTF8 *) pParameter->szParameterName.asCharp(), 
                                   (DBMSrvPar_RTEConfParameter::String) pParameter->xpRecord.xpValue.xpValueString.asCharp(), 
                                   (pParameter->xpProperties[XPPropClassChange] == DBMSrvPar_Parameter::XPPropValueRunning), 
                                   true,
                                   oMsg)) {
          nFuncReturn = DBMSrvDep_DirtyReply(replyData, replyLen).startWithMessageList(oMsg);
        } else {
          cn29ParamValueToString(pParameter->szNewValue, &(pParameter->xpRecord));
          cn29WriteParamHistory(szDbName, pParameter, PAHI_CHANGE_CN002);
        } // end if
      } else {
        nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_XPNOTFOUND_CN00);
      } // end if
    } // end if
  } // end if

  return nFuncReturn;
} // end cn28_CopyParamString

/*
  -------------------------------------------------------------------------
  function:     cn28_MigrateFrom_7_6_00_016
  purpose:      migration from 7.6.00.016
  -------------------------------------------------------------------------
 */
static tcn00_Error cn28_MigrateFrom_7_6_00_016
      ( tsp00_DbNamec          szDbName,
        DBMSrvPar_Parameters * pXParamData,
        char                 * replyData,
        int                  * replyLen,
        int                    replyLenMax)

{
    tcn00_Error                nFuncReturn = OK_CN00;
    DBMSrvMsg_Error            oMsg;
    DBMSrvPar_RTEConfParameter oConfigFile(szDbName.asCharp(), 
                                           0, 
                                           RTECONF_MAXNAMELENGTH,
                                           RTECONF_MAXSTRINGLENGTH);

    if (!oConfigFile.Read(oMsg)) {
        nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(oMsg);
    } // end if

    DBMSrvPar_Parameter * pSharedSQL = NULL;
    DBMSrvPar_Parameter * pSharedSQL_CommandCache = NULL;

    // handle SHAREDSQL
    if (nFuncReturn == OK_CN00) {
        pSharedSQL = pXParamData->Get(KERNELPARAM_SHAREDSQL);
        if (pSharedSQL != NULL) {
            SAPDB_strcpy((char *) pSharedSQL->xpRecord.xpValue.xpValueString, "YES");
            if (!oConfigFile.SetValue ((const SAPDB_UTF8 *) pSharedSQL->szParameterName.asCharp(), 
                                    (DBMSrvPar_RTEConfParameter::String) pSharedSQL->xpRecord.xpValue.xpValueString.asCharp(), 
                                    (pSharedSQL->xpProperties[XPPropClassChange] == DBMSrvPar_Parameter::XPPropValueRunning), 
                                    true,
                                    oMsg)) {
                nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(oMsg);
            } else {
                cn29ParamValueToString(pSharedSQL->szNewValue, &(pSharedSQL->xpRecord));
                pSharedSQL->szComment.rawAssign("implicit change by migration");
            } // end if

        } // end if
    } // end if

    // handle SHAREDSQL_COMMANDCACHESIZE
    if (nFuncReturn == OK_CN00) {
        pSharedSQL_CommandCache = pXParamData->Get(KERNELPARAM_SHAREDSQL_COMMANDCACHESIZE);
        if (pSharedSQL_CommandCache != NULL) {
            DBMSrvMsg_Error oDummy;
            if (oConfigFile.GetValue ((const SAPDB_UTF8 *) pSharedSQL_CommandCache->szParameterName.asCharp(), pSharedSQL_CommandCache->xpRecord.xpValue.xpValueInt, oDummy) &&
                (pSharedSQL_CommandCache->xpRecord.xpValue.xpValueInt < 262144) ) {
                pSharedSQL_CommandCache->xpRecord.xpValue.xpValueInt = 262144;
                if (!oConfigFile.SetValue ((const SAPDB_UTF8 *) pSharedSQL_CommandCache->szParameterName.asCharp(), 
                                        pSharedSQL_CommandCache->xpRecord.xpValue.xpValueInt, 
                                        (pSharedSQL_CommandCache->xpProperties[XPPropClassChange] == DBMSrvPar_Parameter::XPPropValueRunning), 
                                        true,
                                        oMsg)) {
                    nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(oMsg);
                } else {
                    cn29ParamValueToString(pSharedSQL_CommandCache->szNewValue, &(pSharedSQL_CommandCache->xpRecord));
                    pSharedSQL_CommandCache->szComment.rawAssign("implicit change by migration");
                } // end if
            } else {
                pSharedSQL_CommandCache = NULL;
            } // end if
        } // end if
    } // end if

    // write changes to disk
    if (nFuncReturn == OK_CN00) {
        if (!oConfigFile.Write(oMsg)) {
            nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(oMsg);
        } else {
            if (pSharedSQL              != NULL) cn29WriteParamHistory(szDbName, pSharedSQL, PAHI_CHANGE_CN002);
            if (pSharedSQL_CommandCache != NULL) cn29WriteParamHistory(szDbName, pSharedSQL_CommandCache, PAHI_CHANGE_CN002);
        } // end if
    } // end if

    return nFuncReturn;
} // end cn28_MigrateFrom_7_6_00_016

/*
  -------------------------------------------------------------------------
  function:     cn28_MigrateFrom_7_6_00_010
  purpose:      migration from 7.6.00.010
  -------------------------------------------------------------------------
 */
static tcn00_Error cn28_MigrateFrom_7_6_00_010
      ( tsp00_DbNamec          szDbName,
        DBMSrvPar_Parameters * pXParamData,
        char                 * replyData,
        int                  * replyLen,
        int                    replyLenMax)

{
  cn42RemoveOneFile( szDbName, FOBS_KNLDIAGSTART_CN42       );
  cn42RemoveOneFile( szDbName, FOBS_KNLDIAGRUN_CN42         );
  cn42RemoveOneFile( szDbName, FOBS_KNLDIAGOLDSTART_CN42    );
  cn42RemoveOneFile( szDbName, FOBS_KNLDIAGOLDRUN_CN42      );
  cn42RemoveOneFile( szDbName, FOBS_KNLDIAGERRCOMP_CN42     );
  cn42RemoveOneFile( szDbName, FOBS_DBMUTLCOMP_CN42         );
  cn42RemoveOneFile( szDbName, FOBS_KNLDIAGCLASSIC_CN42     );
  cn42RemoveOneFile( szDbName, FOBS_KNLDIAGVERBOSE_CN42     );
  cn42RemoveOneFile( szDbName, FOBS_KNLDIAGOLDCLASSIC_CN42  );
  cn42RemoveOneFile( szDbName, FOBS_KNLDIAGOLDVERBOSE_CN42  );
  cn42RemoveOneFile( szDbName, FOBS_KNLDIAGERRCLASSIC_CN42  );
  cn42RemoveOneFile( szDbName, FOBS_KNLDIAGERRVERBOSE_CN42  );
  cn42RemoveOneFile( szDbName, FOBS_DBMUTLCLASSIC_CN42      );
  cn42RemoveOneFile( szDbName, FOBS_DBMUTLVERBOSE_CN42      );

  return OK_CN00;
} // end cn28_MigrateFrom_7_6_00_010

/*
  -------------------------------------------------------------------------
  function:     cn28_MigrateFrom_7_6_00_000
  purpose:      migration from 7.6.00.000
  -------------------------------------------------------------------------
 */
static tcn00_Error cn28_MigrateFrom_7_6_00_000
      ( tsp00_DbNamec          szDbName,
        DBMSrvPar_Parameters * pXParamData,
        char                 * replyData,
        int                  * replyLen,
        int                    replyLenMax)

{
  cn51DBMConfigValue::migrateFile(szDbName);
  return OK_CN00;
} // end cn28_MigrateFrom_7_6_00_000

/*
  -------------------------------------------------------------------------
  function:     cn28_MigrateFrom_7_5
  purpose:      volume size migration
  -------------------------------------------------------------------------
 */
static tcn00_Error cn28_MigrateFrom_7_5 
      ( tsp00_DbNamec       szDbName,
        DBMSrvPar_Parameters * pXParamData,
        char              * replyData,
        int               * replyLen,
        int                 replyLenMax)

{
  FUNCTION_DBG_MCN00_1("cn28_MigrateFrom_7_5");

  tcn00_Error          nFuncReturn = OK_CN00;
  DBMSrvMsg_Error      oMsg;
  DBMSrvPar_RTEConfParameter    oConfigFile(szDbName.asCharp(), 
                                   0, 
                                   RTECONF_MAXNAMELENGTH,
                                   RTECONF_MAXSTRINGLENGTH);

  DBMSrvPar_RTEConfParameter::Integer backupBlockCount;

  // open param file
  if (!oConfigFile.Read(oMsg)) {
    nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(oMsg);
  } else {
    DBMSrvPar_RTEConfParameter::Integer nVolumeSize;
    DBMSrvPar_RTEConfParameter::Integer nVolumes;
    int                        nVolumeIndex;
    tsp00_XpKeyTypec           szName;
    DBMSrvPar_Parameter        * pParameter = NULL;


    // DATA Volume count
    if (nFuncReturn == OK_CN00) {
      if (!oConfigFile.GetValue ((const SAPDB_UTF8 *) PAN_MAXDATADEVSPACES, nVolumes, oMsg)) {
        nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(oMsg);        
      } // end if
    } // end if

    // iterate through DATA volumes
    for (nVolumeIndex = 1; (nVolumeIndex <= nVolumes) && (nFuncReturn == OK_CN00); ++nVolumeIndex) {

        // read volume size
        sprintf(szName, PAN_DATADEV_N_SIZE"%04d", nVolumeIndex);
        if (oConfigFile.GetValue ((const SAPDB_UTF8 *) szName.asCharp(), nVolumeSize, oMsg)) {
            // write volume size
            pParameter = cn28_FindParamNew(pXParamData, szName);
            if (pParameter != NULL) {
                pParameter->xpRecord.xpValue.xpValueInt = nVolumeSize + 1;
                if (!oConfigFile.SetValue ((const SAPDB_UTF8 *) pParameter->szParameterName.asCharp(),
                            pParameter->xpRecord.xpValue.xpValueInt,
                            (pParameter->xpProperties[XPPropClassChange] == DBMSrvPar_Parameter::XPPropValueRunning),
                            true,
                            oMsg)) {
                    nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(oMsg);
                } else {
                    cn29ParamValueToString(pParameter->szNewValue, &(pParameter->xpRecord));
                    cn29WriteParamHistory(szDbName, pParameter, PAHI_CHANGE_CN002);
                } // end if
            } else {
                nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_XPNOTFOUND, Msg_Arg(SDBMSGTAG_DBMSRV_XPNOTFOUND__PARAMETER, szName)));
            } // end if
        } else {
            if (oMsg == DBMSrvMsg_Error::DBMSrv_XPNOTFOUND) {
                oMsg.ClearMessageList();
            } else {
                nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(oMsg);
            } // end if
        } // end if
    } // end for

    // LOG Volumes
    if (nFuncReturn == OK_CN00) {
      if (!oConfigFile.GetValue ((const SAPDB_UTF8 *) PAN_MAXARCHIVELOGS, nVolumes, oMsg)) {
        nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(oMsg);
      } // end if
    } // end if

    // iterate through LOG volumes
    for (nVolumeIndex = 1; (nVolumeIndex <= nVolumes) && (nFuncReturn == OK_CN00); ++nVolumeIndex) {

        // read log volume size
        sprintf(szName, PAN_ARCHIVE_LOG_N_SIZE"%03d", nVolumeIndex);
        if (oConfigFile.GetValue ((const SAPDB_UTF8 *) szName.asCharp(), nVolumeSize, oMsg)) {
            // write log volume size
            pParameter = cn28_FindParamNew(pXParamData, szName);
            if (pParameter != NULL) {
                pParameter->xpRecord.xpValue.xpValueInt = nVolumeSize + 1;
                if (!oConfigFile.SetValue ((const SAPDB_UTF8 *) pParameter->szParameterName.asCharp(),
                            pParameter->xpRecord.xpValue.xpValueInt,
                            (pParameter->xpProperties[XPPropClassChange] == DBMSrvPar_Parameter::XPPropValueRunning),
                            true,
                            oMsg)) {
                    nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(oMsg);
                } else {
                    cn29ParamValueToString(pParameter->szNewValue, &(pParameter->xpRecord));
                    cn29WriteParamHistory(szDbName, pParameter, PAHI_CHANGE_CN002);
                } // end if
            } else {
                nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_XPNOTFOUND, Msg_Arg(SDBMSGTAG_DBMSRV_XPNOTFOUND__PARAMETER, szName)));
            } // end if
        } else {
            if (oMsg == DBMSrvMsg_Error::DBMSrv_XPNOTFOUND) {
                oMsg.ClearMessageList();
            } else {
                nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(oMsg);
            } // end if
        } // end if
    } // end for

    // ready stamp kernelversion
    if (nFuncReturn == OK_CN00) {
      pParameter = pXParamData->Get(PAN_PARAM_KERNELVERSION);
      if (pParameter != NULL) {
        // get own version
        tsp00_Versionc      VersionString;
        sp100_GetVersionString(COMP_NAME_KERNEL_SP100, s100buildnumber, VersionString);
        
        cn29StringToParamValue(&(pParameter->xpRecord), VersionString.asCharp());

        DBMSrvPar_RTEConfParameter::String pString = (DBMSrvPar_RTEConfParameter::String) pParameter->xpRecord.xpValue.xpValueString.asCharp();

        if (!oConfigFile.SetValue ((const SAPDB_UTF8 *) pParameter->szParameterName.asCharp(), 
                                   pString, 
                                   (pParameter->xpProperties[XPPropClassChange] == DBMSrvPar_Parameter::XPPropValueRunning), 
                                   true,
                                   oMsg)) {
          nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(oMsg);
        } else {
          cn29ParamValueToString(pParameter->szNewValue, &(pParameter->xpRecord));
          cn29WriteParamHistory(szDbName, pParameter, PAHI_CHANGE_CN002);
        } // end if
      } else {
        nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_XPNOTFOUND_CN00);
      } // end if
    } // end if

    //get value for BACKUP_BLOCK_CNT
    if(nFuncReturn == OK_CN00 &&
       !oConfigFile.GetValue((const SAPDB_UTF8 *) PAN_BACKUP_BLOCK_CNT, backupBlockCount, oMsg))
    {
        nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(oMsg);
    }

    // close param file
    if (nFuncReturn == OK_CN00) {
      if (!oConfigFile.Write(oMsg)) {
        nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(oMsg);
      } else {
        cn51DBMConfigValue oMigration( szDbName, CFG_CFGMIGRVOL_CN51 );
        oMigration = 1;
        if (!oMigration.LastEvent().IsEmpty()) {
          nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oMigration.LastEvent());
        } // end if
      } // end if
    } // end if

  } // end if

  //take care of BACKUP_BLOCK_CNT, as it is ceasing to exist
  if(OK_CN00==nFuncReturn)
  {
      DBMSrv_Reply reply(replyData, replyLen, replyLenMax);
      nFuncReturn=cn30SetBackupBlockCountInMediaFile(SAPDB_ToString(backupBlockCount), reply);
  }

  return nFuncReturn;
} // end cn28_MigrateFrom_7_5

/*
  -------------------------------------------------------------------------
  function:     cn28_MigrateFrom_7_2_5_or_7_3_0
  purpose:      CACHE_SIZE handling
  -------------------------------------------------------------------------
 */
static tcn00_Error cn28_MigrateFrom_7_2_5_or_7_3_0 
      ( tsp00_DbNamec       szDbName,
        DBMSrvPar_Parameters * pXParamData,
        char              * replyData,
        int               * replyLen,
        int                 replyLenMax)

{
  FUNCTION_DBG_MCN00_1("cn28_MigrateFrom_7_2_5_or_7_3_0");

  tcn00_Error          nFuncReturn = OK_CN00;
  DBMSrvMsg_Error      oMsg;
  DBMSrvPar_RTEConfParameter    oConfigFile(szDbName.asCharp(), 
                                   0, 
                                   RTECONF_MAXNAMELENGTH,
                                   RTECONF_MAXSTRINGLENGTH);

  // open param file
  if (!oConfigFile.Read(oMsg)) {
    nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(oMsg);
  } else {

    // Parameter to write
    DBMSrvPar_Parameter * pParameter = NULL;
    
    // CACHE_SIZE = DATA_CACHE + CONVERTER_CACHE
    DBMSrvPar_RTEConfParameter::Integer nDatacache;
    DBMSrvPar_RTEConfParameter::Integer nConvertercache;

    // read DATA_CACHE
    if (nFuncReturn == OK_CN00) {
      if (!oConfigFile.GetValue ((const SAPDB_UTF8 *) PAN_DATA_CACHE_PAGS, nDatacache, oMsg)) {
        nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(oMsg);
      } // end if
    } // end if

    // read CONVERTER_CACHE
    if (nFuncReturn == OK_CN00) {
      if (!oConfigFile.GetValue ((const SAPDB_UTF8 *) PAN_CONV_CACHE_PAGES, nConvertercache, oMsg)) {
        nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(oMsg);
      } // end if
    } // end if

    // write CACHE_SIZE
    if (nFuncReturn == OK_CN00) {
      pParameter = pXParamData->Get(PAN_CACHE_SIZE);
      if (pParameter != NULL) {
        pParameter->xpRecord.xpValue.xpValueInt = nDatacache + nConvertercache;
        if (!oConfigFile.SetValue ((const SAPDB_UTF8 *) pParameter->szParameterName.asCharp(), 
                                   pParameter->xpRecord.xpValue.xpValueInt, 
                                   (pParameter->xpProperties[XPPropClassChange] == DBMSrvPar_Parameter::XPPropValueRunning), 
                                   true,
                                   oMsg)) {
          nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(oMsg);
        } else {
          cn29ParamValueToString(pParameter->szNewValue, &(pParameter->xpRecord));
          cn29WriteParamHistory(szDbName, pParameter, PAHI_CHANGE_CN002);
        } // end if
      } else {
        nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_XPNOTFOUND_CN00);
      } // end if
    } // end if

    // ready stamp kernelversion
    if (nFuncReturn == OK_CN00) {
      pParameter = pXParamData->Get(PAN_PARAM_KERNELVERSION);
      if (pParameter != NULL) {
        // get own version
        tsp00_Versionc      VersionString;
        sp100_GetVersionString(COMP_NAME_KERNEL_SP100, s100buildnumber, VersionString);
        
        cn29StringToParamValue(&(pParameter->xpRecord), VersionString.asCharp());

        DBMSrvPar_RTEConfParameter::String pString = (DBMSrvPar_RTEConfParameter::String) pParameter->xpRecord.xpValue.xpValueString.asCharp();

        if (!oConfigFile.SetValue ((const SAPDB_UTF8 *) pParameter->szParameterName.asCharp(), 
                                   pString, 
                                   (pParameter->xpProperties[XPPropClassChange] == DBMSrvPar_Parameter::XPPropValueRunning), 
                                   true,
                                   oMsg)) {
          nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(oMsg);
        } else {
          cn29ParamValueToString(pParameter->szNewValue, &(pParameter->xpRecord));
          cn29WriteParamHistory(szDbName, pParameter, PAHI_CHANGE_CN002);
        } // end if
      } else {
        nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_XPNOTFOUND_CN00);
      } // end if
    } // end if

    // close param file
    if (nFuncReturn == OK_CN00) {
      if (!oConfigFile.Write(oMsg)) {
        nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(oMsg);
      } else {
        cn51DBMConfigValue oMigration( szDbName, CFG_CFGMIGRDB_CN51 );
        oMigration = 1;
        if (!oMigration.LastEvent().IsEmpty()) {
          nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oMigration.LastEvent());
        } // end if
      } // end if
    } // end if

  } // end if
  
  return nFuncReturn;
} // end cn28_MigrateFrom_7_2_5_or_7_3_0
/*
  -------------------------------------------------------------------------
  function:     cn28_MigrateFrom_7_4_3_or_2_or_1
  purpose:      DEVSPACE_TO_VOLUME Handling and several renamings
  -------------------------------------------------------------------------
 */
static tcn00_Error cn28_MigrateFrom_7_4_3_or_2_or_1
      ( tsp00_DbNamec       szDbName,
        DBMSrvPar_Parameters * pXParamData,
        char              * replyData,
        int               * replyLen)

{
  FUNCTION_DBG_MCN00_1("cn28_MigrateFrom_7_4_3_or_2_or_1");

  tcn00_Error          nFuncReturn = OK_CN00;
  DBMSrvMsg_Error      oMsg;
  DBMSrvPar_RTEConfParameter    oConfigFile(szDbName.asCharp(), 
                                   0, 
                                   RTECONF_MAXNAMELENGTH,
                                   RTECONF_MAXSTRINGLENGTH);

  // open param file
  if (!oConfigFile.Read(oMsg)) {
    nFuncReturn = DBMSrvDep_DirtyReply(replyData, replyLen).startWithMessageList(oMsg);
  } else {

    // Parameter to write
    DBMSrvPar_Parameter * pParameter = NULL;

    // handle MAXDATAVOLUMES
    if (nFuncReturn == OK_CN00) {
      nFuncReturn = cn28_CopyParamInteger(szDbName, 
                                          pXParamData, 
                                          oConfigFile,
                                          PAN_MAXDATADEVSPACES_OLD, 
                                          PAN_MAXDATADEVSPACES, 
                                          replyData, 
                                          replyLen,
                                          false);
    } // end if
    // handle FBM_VOLUME_COMPRESSION
    if (nFuncReturn == OK_CN00) {
      nFuncReturn = cn28_CopyParamInteger(szDbName, 
                                          pXParamData, 
                                          oConfigFile,
                                          PAN_FBM_DEVSP_COMPR_OLD, 
                                          PAN_FBM_DEVSP_COMPR, 
                                          replyData, 
                                          replyLen,
                                          false);
    } // end if
    // handle FBM_VOLUME_BALANCE
    if (nFuncReturn == OK_CN00) {
      nFuncReturn = cn28_CopyParamInteger(szDbName, 
                                          pXParamData, 
                                          oConfigFile,
                                          PAN_FBM_DEVSP_BALANCE_OLD, 
                                          PAN_FBM_DEVSP_BALANCE, 
                                          replyData, 
                                          replyLen,
                                          false);
    } // end if
    // handle MAXVOLUMES
    if (nFuncReturn == OK_CN00) {
      nFuncReturn = cn28_CopyParamInteger(szDbName, 
                                          pXParamData, 
                                          oConfigFile,
                                          PAN_MAXDEVSPACES_OLD, 
                                          PAN_MAXDEVSPACES, 
                                          replyData, 
                                          replyLen,
                                          false);
    } // end if
    // handle MAXPAGER
    if (nFuncReturn == OK_CN00) {
      nFuncReturn = cn28_CopyParamInteger(szDbName, 
                                          pXParamData, 
                                          oConfigFile,
                                          PAN_MAXDATAWRITER_OLD, 
                                          PAN_MAXDATAWRITER, 
                                          replyData, 
                                          replyLen,
                                          false);
    } // end if
    // handle MAXLOGVOLUMES
    if (nFuncReturn == OK_CN00) {
      nFuncReturn = cn28_CopyParamInteger(szDbName, 
                                          pXParamData, 
                                          oConfigFile,
                                          PAN_MAXARCHIVELOGS_OLD, 
                                          PAN_MAXARCHIVELOGS, 
                                          replyData, 
                                          replyLen,
                                          false);
    } // end if
    // handle VOLUMENO_BIT_COUNT
    if (nFuncReturn == OK_CN00) {
      nFuncReturn = cn28_CopyParamInteger(szDbName, 
                                          pXParamData, 
                                          oConfigFile,
                                          PAN_DEVNO_BIT_COUNT_OLD, 
                                          PAN_DEVNO_BIT_COUNT, 
                                          replyData, 
                                          replyLen,
                                          true);
    } // end if
    // handle TRACE_PAGES_LW
    if (nFuncReturn == OK_CN00) {
      nFuncReturn = cn28_CopyParamInteger(szDbName, 
                                          pXParamData, 
                                          oConfigFile,
                                          PAN_TRACE_PAGES_AL_OLD, 
                                          PAN_TRACE_PAGES_AL, 
                                          replyData, 
                                          replyLen,
                                          false);
    } // end if
    // handle TRACE_PAGES_PG
    if (nFuncReturn == OK_CN00) {
      nFuncReturn = cn28_CopyParamInteger(szDbName, 
                                          pXParamData, 
                                          oConfigFile,
                                          PAN_TRACE_PAGES_DW_OLD, 
                                          PAN_TRACE_PAGES_DW, 
                                          replyData, 
                                          replyLen,
                                          false);
    } // end if
    // handle LOG_MODE/LOG_MIRRORED
    DBMSrvPar_RTEConfParameter::StringElement szLogMode[RTECONF_MAXSTRINGLENGTH + 1];
    bool  bDual = false;
    if (oConfigFile.GetValue (KERNELPARAM_LOG_MODE, szLogMode, oMsg)) {
      if (STRCMP_UTF8(szLogMode, "DUAL") == 0) {
        SAPDB_Bool bUpdateOnline = oConfigFile.UpdateOnlinePossible (KERNELPARAM_LOG_MODE);
        bDual = true;
        STRCPY_UTF8(szLogMode, "YES");
        if (!oConfigFile.SetValue (KERNELPARAM_LOG_MIRRORED, szLogMode, bUpdateOnline,  true, oMsg)) {
          nFuncReturn = DBMSrvDep_DirtyReply(replyData, replyLen).startWithMessageList(oMsg);
        }  // end if
      }  // end if
    } else {
        if (oMsg == DBMSrvMsg_Error::DBMSrv_XPNOTFOUND) {
            oMsg.ClearMessageList();
        } // end if
    } // end if

    // read DATA Volumes
    if (nFuncReturn == OK_CN00) {
      DBMSrvPar_RTEConfParameter::Integer nVolumes;
      int                        nVolumeIndex;
      tsp00_XpKeyTypec           szOld;
      tsp00_XpKeyTypec           szNew;
      
      if (!oConfigFile.GetValue ((const SAPDB_UTF8 *) PAN_MAXDATADEVSPACES, nVolumes, oMsg)) {
        nFuncReturn = DBMSrvDep_DirtyReply(replyData, replyLen).startWithMessageList(oMsg);
      } // end if

      for (nVolumeIndex = 1; (nVolumeIndex <= nVolumes) && (nFuncReturn == OK_CN00); ++nVolumeIndex) {
        sprintf(szOld, PAN_DATADEV_N_OLD"%04d", nVolumeIndex);
        sprintf(szNew, PAN_DATADEV_N"%04d"    , nVolumeIndex);
        nFuncReturn = cn28_CopyParamString(szDbName, 
                                           pXParamData, 
                                           oConfigFile,
                                           szOld, 
                                           szNew, 
                                           replyData, 
                                           replyLen,
                                           true);
        if (nFuncReturn == OK_CN00) {
          sprintf(szOld, PAN_DATADEV_N_SIZE_OLD"%04d", nVolumeIndex);
          sprintf(szNew, PAN_DATADEV_N_SIZE"%04d"    , nVolumeIndex);
          nFuncReturn = cn28_CopyParamInteger(szDbName, 
                                              pXParamData, 
                                              oConfigFile,
                                              szOld, 
                                              szNew, 
                                              replyData, 
                                              replyLen,
                                              true);
        } // end if
        if (nFuncReturn == OK_CN00) {
          sprintf(szOld, PAN_DATADEV_N_TYPE_OLD"%04d", nVolumeIndex);
          sprintf(szNew, PAN_DATADEV_N_TYPE"%04d"    , nVolumeIndex);
          nFuncReturn = cn28_CopyParamString(szDbName, 
                                             pXParamData, 
                                             oConfigFile,
                                             szOld, 
                                             szNew, 
                                             replyData, 
                                             replyLen,
                                             true);
        } // end if

      } // end for

    } // end if

    // read LOG Volumes
    if (nFuncReturn == OK_CN00) {
      DBMSrvPar_RTEConfParameter::Integer nVolumes;
      int                        nVolumeIndex;
      tsp00_XpKeyTypec           szOld;
      tsp00_XpKeyTypec           szNew;
      
      if (!oConfigFile.GetValue ((const SAPDB_UTF8 *) PAN_MAXARCHIVELOGS, nVolumes, oMsg)) {
        nFuncReturn = DBMSrvDep_DirtyReply(replyData, replyLen).startWithMessageList(oMsg);
      } // end if

      for (nVolumeIndex = 1; (nVolumeIndex <= nVolumes) && (nFuncReturn == OK_CN00); ++nVolumeIndex) {
        sprintf(szOld, PAN_ARCHIVE_LOG_N_OLD"%03d", nVolumeIndex);
        sprintf(szNew, PAN_ARCHIVE_LOG_N"%03d"    , nVolumeIndex);
        nFuncReturn = cn28_CopyParamString(szDbName, 
                                           pXParamData, 
                                           oConfigFile,
                                           szOld, 
                                           szNew, 
                                           replyData, 
                                           replyLen,
                                           true);
        if (nFuncReturn == OK_CN00) {
          sprintf(szOld, PAN_ARCHIVE_LOG_N_SIZE_OLD"%03d", nVolumeIndex);
          sprintf(szNew, PAN_ARCHIVE_LOG_N_SIZE"%03d"    , nVolumeIndex);
          nFuncReturn = cn28_CopyParamInteger(szDbName, 
                                              pXParamData, 
                                              oConfigFile,
                                              szOld, 
                                              szNew, 
                                              replyData, 
                                              replyLen,
                                              true);
          // Hack!!! 
          // http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1125994
          sprintf(szOld, PAN_ARCHIVE_LOG_N_SIZE_OLD"%02d", nVolumeIndex);
          sprintf(szNew, PAN_ARCHIVE_LOG_N_SIZE"%03d"    , nVolumeIndex);
          nFuncReturn = cn28_CopyParamInteger(szDbName, 
                                              pXParamData, 
                                              oConfigFile,
                                              szOld, 
                                              szNew, 
                                              replyData, 
                                              replyLen,
                                              true);
        } // end if
        if (nFuncReturn == OK_CN00) {
          sprintf(szOld, PAN_ARCHIVE_LOG_N_TYPE_OLD"%03d", nVolumeIndex);
          sprintf(szNew, PAN_ARCHIVE_LOG_N_TYPE"%03d"    , nVolumeIndex);
          nFuncReturn = cn28_CopyParamString(szDbName, 
                                             pXParamData, 
                                             oConfigFile,
                                             szOld, 
                                             szNew, 
                                             replyData, 
                                             replyLen,
                                             true);
        } // end if
        if (bDual) {
          sprintf(szOld, (SAPDB_Char *)KERNELPARAM_M_ARCHIVE_LOG_"%03d",     nVolumeIndex);
          sprintf(szNew, (SAPDB_Char *)KERNELPARAM_M_LOG_VOLUME_NAME_"%03d", nVolumeIndex);
          nFuncReturn = cn28_CopyParamString(szDbName, 
                                             pXParamData, 
                                             oConfigFile,
                                             szOld, 
                                             szNew, 
                                             replyData, 
                                             replyLen,
                                             true);
          if (nFuncReturn == OK_CN00) {
            sprintf(szOld, (SAPDB_Char *)KERNELPARAM_M_ARCHIVE_TYPE_"%03d",     nVolumeIndex);
            sprintf(szNew, (SAPDB_Char *)KERNELPARAM_M_LOG_VOLUME_TYPE_"%03d", nVolumeIndex);
            nFuncReturn = cn28_CopyParamString(szDbName, 
                                               pXParamData, 
                                               oConfigFile,
                                               szOld, 
                                               szNew, 
                                               replyData, 
                                               replyLen,
                                               true);
          } // end if
        } // end if

      } // end for

    } // end if

    // ready stamp kernelversion
    if (nFuncReturn == OK_CN00) {
      pParameter = pXParamData->Get(PAN_PARAM_KERNELVERSION);
      if (pParameter != NULL) {
        // get own version
        tsp00_Versionc      VersionString;
        sp100_GetVersionString(COMP_NAME_KERNEL_SP100, s100buildnumber, VersionString);
        
        cn29StringToParamValue(&(pParameter->xpRecord), VersionString.asCharp());

        DBMSrvPar_RTEConfParameter::String pString = (DBMSrvPar_RTEConfParameter::String) pParameter->xpRecord.xpValue.xpValueString.asCharp();

        if (!oConfigFile.SetValue ((const SAPDB_UTF8 *) pParameter->szParameterName.asCharp(), 
                                   pString, 
                                   (pParameter->xpProperties[XPPropClassChange] == DBMSrvPar_Parameter::XPPropValueRunning), 
                                   true,
                                   oMsg)) {
          nFuncReturn = DBMSrvDep_DirtyReply(replyData, replyLen).startWithMessageList(oMsg);
        } else {
          cn29ParamValueToString(pParameter->szNewValue, &(pParameter->xpRecord));
          cn29WriteParamHistory(szDbName, pParameter, PAHI_CHANGE_CN002);
        } // end if
      } else {
        nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_XPNOTFOUND_CN00);
      } // end if
    } // end if

    // close param file
    if (nFuncReturn == OK_CN00) {
      if (!oConfigFile.Write(oMsg)) {
        nFuncReturn = DBMSrvDep_DirtyReply(replyData, replyLen).startWithMessageList(oMsg);
      } // end if
    } // end if

  } // end if

  return nFuncReturn;
} // end cn28_MigrateFrom_7_4_3_or_2_or_1

/*
  -------------------------------------------------------------------------
  function:     cn28_SetVersion
  -------------------------------------------------------------------------
 */
static void cn28_SetVersion 
      ( tsp100_VersionID1  &  VersionID1,
        int                   MajorVersion,
        int                   MinorVersion,
        int                   CorrLevel,
        int                   BuildNumberPrefix )
{
  VersionID1.MajorVersion_sp100       = MajorVersion;
  VersionID1.MinorVersion_sp100       = MinorVersion;
  VersionID1.CorrLevel_sp100          = CorrLevel;
  VersionID1.BuildNumberPrefix_sp100  = BuildNumberPrefix;
} // end cn28_SetVersion

/*
  -------------------------------------------------------------------------
  function:     cn28_ReadVersionReached
  -------------------------------------------------------------------------
 */
static bool cn28_ReadVersionReached 
      ( tsp00_DbNamec         szDbName,
        tsp100_VersionID1  &  oVersionReached,
        tsp00_C256c        &  szVersionReached  )
{
  cn28_SetVersion(oVersionReached, 0, 0, 0, 0);
  szVersionReached.Init();

  // read the version from the param file
  DBMSrvMsg_Error               oMsg;
  DBMSrvPar_RTEConfParameter    oConfigFile(szDbName.asCharp(), 
                                   0, 
                                   RTECONF_MAXNAMELENGTH,
                                   RTECONF_MAXSTRINGLENGTH);
  DBMSrvPar_Parameter::XPRecord xpRecord;
  // open param file
  if (!oConfigFile.Read(oMsg)) {
    // key not found or paramfile not exist 
    // don't migrate but success
    return true;
  } else {
    if (!oConfigFile.GetValue((SAPDB_UTF8 *) PAN_PARAM_KERNELVERSION , (DBMSrvPar_RTEConfParameter::String)xpRecord.xpValue.xpValueString.asCharp(), oMsg)) {
      // key not found or paramfile not exist 
      // don't migrate but success
      return true;
    } else {
      szVersionReached.rawAssign((char *) xpRecord.xpValue.xpValueString);
    } // end if
  } // end if

  // convert version string to structure
  tsp00_C64c szVersion;
  tsp00_C64c szBuild;
  tsp00_C64c szTemp;
  char *     pBegin;
  char *     pEnd;
  
  // extract version information from string
  strncpy(szVersion, &szVersionReached[BEG_OF_1ST_VERS_SUBSTR_SP100], END_OF_1ST_VERS_SUBSTR_SP100 - BEG_OF_1ST_VERS_SUBSTR_SP100 + 1);
  szVersion[END_OF_1ST_VERS_SUBSTR_SP100 - BEG_OF_1ST_VERS_SUBSTR_SP100 + 1] = 0;

  // extract build information from string
  strncpy(szBuild, &szVersionReached[BEG_OF_2CND_VERS_SUBSTR_SP100], END_OF_2CND_VERS_SUBSTR_SP100 - BEG_OF_2CND_VERS_SUBSTR_SP100 + 1);
  szBuild[END_OF_2CND_VERS_SUBSTR_SP100 - BEG_OF_2CND_VERS_SUBSTR_SP100 + 1] = 0;

  // major version  
  pBegin = &szVersion[0];
  pEnd   = strchr(pBegin, '.');
  if (pBegin != NULL && pEnd != NULL) {
    strncpy(szTemp, pBegin, pEnd - pBegin);
    szTemp[(int) (pEnd - pBegin)] = 0;
    oVersionReached.MajorVersion_sp100 = (unsigned char) atoi(szTemp);
    pBegin = pEnd + 1;
    pEnd   = strchr(pBegin, '.');
  } else {
    return false;
  } // end if

  // minor version  
  if (pBegin != NULL && pEnd != NULL) {
    strncpy(szTemp, pBegin, pEnd - pBegin);
    szTemp[(int) (pEnd - pBegin)] = 0;
    oVersionReached.MinorVersion_sp100 = (unsigned char) atoi(szTemp);
    pBegin = pEnd + 1;
  } else {
    return false;
  } // end if

  // correction level
  if (pBegin != NULL) {
    oVersionReached.CorrLevel_sp100 = (unsigned char) atoi(pBegin);
  } else {
    return false;
  } // end if
  
  // build
  oVersionReached.BuildNumberPrefix_sp100 = (unsigned char) atoi(szBuild);

  return true;
} // end cn28_ReadVersionReached

/*
  -------------------------------------------------------------------------
  function:     cn28MigrateParamFile
  -------------------------------------------------------------------------
 */
tcn00_Error cn28MigrateParamFile 
      ( tsp00_DbNamec           szDbName,
        DBMSrvPar_Parameters * pXParamData,
        char                 * replyData,
        int                  * replyLen,
        int                    replyLenMax)
{
  tcn00_Error nFuncReturn = OK_CN00;

  tsp100_VersionID1   oVersionReached;
  tsp00_C256c         szVersionReached;
  if (!cn28_ReadVersionReached(szDbName, oVersionReached, szVersionReached)) {
    // error reading the version information
    return cn90AnswerEvent(replyData, replyLen, replyLenMax, DBMSrvMsg_Error(SDBMSG_DBMSRV_BADVERSION, Msg_Arg(SDBMSGTAG_DBMSRV_BADVERSION__DBMSRV_COMP,szVersionReached.asCharp())));
  } // end if
  if (oVersionReached.MajorVersion_sp100 == 0) {
    // no version information available, don't migrate but success
    return OK_CN00;
  } // end if

  // loop until a error or the "break" in the else branch occurs
  while (nFuncReturn == OK_CN00) {

    if ( ( (oVersionReached.MajorVersion_sp100        <   7)    ) ||

         ( (oVersionReached.MajorVersion_sp100        ==  7) &&
           (oVersionReached.MinorVersion_sp100        <   2)    ) ||

         ( (oVersionReached.MajorVersion_sp100        ==  7) &&
           (oVersionReached.MinorVersion_sp100        ==  2) &&
           (oVersionReached.CorrLevel_sp100           <   5)    ) ||

         ( (oVersionReached.MajorVersion_sp100        ==  7) &&
           (oVersionReached.MinorVersion_sp100        ==  2) &&
           (oVersionReached.CorrLevel_sp100           ==  5) &&
           (oVersionReached.BuildNumberPrefix_sp100   <  15)    ) ||

         ( (oVersionReached.MajorVersion_sp100        ==  7) &&
           (oVersionReached.MinorVersion_sp100        ==  3) &&
           (oVersionReached.CorrLevel_sp100           ==  0) &&
           (oVersionReached.BuildNumberPrefix_sp100   <  21)    )    ){
      // go away I don't like you
      nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, DBMSrvMsg_Error(SDBMSG_DBMSRV_MIGRATE, Msg_Arg(SDBMSGTAG_DBMSRV_MIGRATE__DBMSRV_COMP,szVersionReached.asCharp())));

    } else if ( ( (oVersionReached.MajorVersion_sp100    ==  7) &&
                  (oVersionReached.MinorVersion_sp100    <   4)    )    ) {
      // migrate from very old releases to 7.4.00.00
      nFuncReturn = cn28_MigrateFrom_7_2_5_or_7_3_0(szDbName, pXParamData, replyData, replyLen, replyLenMax);
      cn28_SetVersion(oVersionReached, 7, 4, 0, 0);

    } else if ( (oVersionReached.MajorVersion_sp100      ==  7) &&
                (oVersionReached.MinorVersion_sp100      <   5)    ) {
      // migrate 7.4 to 7.5
      nFuncReturn = cn28_MigrateFrom_7_4_3_or_2_or_1(szDbName, pXParamData, replyData, replyLen);
      cn28_SetVersion(oVersionReached, 7, 5, 0, 0);

    } else if ( (oVersionReached.MajorVersion_sp100      ==  7) &&
                (oVersionReached.MinorVersion_sp100      <   6)    ) {
      // from 7.5 to 7.6
      nFuncReturn = cn28_MigrateFrom_7_5(szDbName, pXParamData, replyData, replyLen, replyLenMax);
      cn28_SetVersion(oVersionReached, 7, 6, 0, 0);

    } else if ( (oVersionReached.MajorVersion_sp100      ==  7) &&
                (oVersionReached.MinorVersion_sp100      ==  6) &&
                (oVersionReached.CorrLevel_sp100         ==  0) &&
                (oVersionReached.BuildNumberPrefix_sp100 <  10)    ) {
      // implicit migration from 7.6.00 before build 10
      nFuncReturn = cn28_MigrateFrom_7_6_00_000(szDbName, pXParamData, replyData, replyLen, replyLenMax);
      cn28_SetVersion(oVersionReached, 7, 6, 0, 10);

    } else if ( (oVersionReached.MajorVersion_sp100      ==  7) &&
                (oVersionReached.MinorVersion_sp100      ==  6) &&
                (oVersionReached.CorrLevel_sp100         ==  0) &&
                (oVersionReached.BuildNumberPrefix_sp100 <  16)    ) {
      // implicit migration from 7.6.00 before build 16
      nFuncReturn = cn28_MigrateFrom_7_6_00_010(szDbName, pXParamData, replyData, replyLen, replyLenMax);
      cn28_SetVersion(oVersionReached, 7, 6, 0, 16);

    } else if ( (oVersionReached.MajorVersion_sp100      ==  7) &&
                (oVersionReached.MinorVersion_sp100      ==  6) &&
                (oVersionReached.CorrLevel_sp100         ==  0) &&
                (oVersionReached.BuildNumberPrefix_sp100 <  24)    ) {
      // implicit migration from 7.6.00 before build 24
      nFuncReturn = cn28_MigrateFrom_7_6_00_016(szDbName, pXParamData, replyData, replyLen, replyLenMax);
      cn28_SetVersion(oVersionReached, 7, 6, 0, 24);

    } else {
      // wow it looks like no migration necessary -> target version reached
      break;
    } // end if

  } // end while

  return nFuncReturn;
} // end cn28MigrateParamFile
