/* @lastChanged: "1998-06-08  9:33"
 * @filename:    vcn20.cpp
 * @purpose:     "param management"
 * @release:     7.2. / 6.2.
 * @see:         "-.-"
 *
 * @Copyright (c) 1998-2005 SAP AG"
 *
 * ==================================================================
 *
 * responsible:   Bernd Vorsprach (D025588 - Bernd.Vorsprach@SAP-AG.de)
 *
 * created:       1998-01-20 by Bernd Vorsprach
 *
 * purpose:       basic module for parameter management ("XParam")
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
 * IMPORT                                                                    
 * ------------------------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h> 
#include <sys/stat.h>

#include "heo02.h"
#include "heo06.h"
#include "heo11.h"
#include "hsp02.h"
#include "hsp100.h"

#include "hcn20.h"
#include "hcn23.h"
#include "hcn29.h"
#include "hcn42.h"
#include "hcn50.h"
#include "hcn51.h"
#include "hcn52.h"
#include "hcn90.h"

#include "DBM/Logging/DBMLog_Logger.hpp"
#include "KernelCommon/ParameterNames/KernelParam_IOManager.hpp"
#include "SAPDB/ToolsCommon/Tools_HexBuffer.hpp"
#include "DBM/Srv/KernelAccess/DBMSrvKnl_State.hpp"
#include "DBM/Srv/KernelAccess/DBMSrvKnl_Session.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_NewRTEError.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_RTEError.hpp"
#include "DBM/Srv/DBMSrv_Reply.hpp"

/* ------------------------------------------------------------------
 * specification private macros
 * ------------------------------------------------------------------
 */
#ifndef XP_PARAM_CHANGED
  #define XP_PARAM_CHANGED "__PARAM_CHANGED___"
#endif

#define PARAM_OPT_NOCLOSE      "NOCLOSE"

#define PARAM_OPT_ONLINE       "-running"
#define PARAM_OPT_PERMANENT    "-permanent"

/* ------------------------------------------------------------------
 * PUBLIC FUNCTION cn20FreeXParamSection
 * ------------------------------------------------------------------
 * purpose: frees the XParamSection from the VControlDataT structure
 *
 */
void cn20FreeXParamSection 
      ( VControlDataT * vcontrol )
{

  if (vcontrol != NULL) {
    DBMSrvPar_Parameters * pXParamData = STATIC_CAST(DBMSrvPar_Parameters *, vcontrol->pXParamSection);

    if (pXParamData != NULL) {
      delete pXParamData;
      pXParamData = NULL;
    } /* end if */

    vcontrol->pXParamSection = NULL;
  } /* end if */

} /* end cn20FreeXParamSection */

/* INTERN FUNCTION cn20XParamGetValue
 * ------------------------------------------------------------------
 *
 */
bool cn20XParamGetValue
      ( const char *               DBNameCIn,
        const char *               XpKeyCIn,
        tcn002_XpValueString     & XpValueC,
        DBMSrvMsg_Error          & oDBMError)
{
    DBMSrvPar_Parameter::XPRecord xpRecord;
    DBMSrvPar_RTEConfParameter    oConfigFile(DBNameCIn, 
                                              0, 
                                              RTECONF_MAXNAMELENGTH,
                                              RTECONF_MAXSTRINGLENGTH);

    // initialize return value
    XpValueC.Init();

    // check parameters
    if (strlen(DBNameCIn) == 0) {
        oDBMError = DBMSrvMsg_Error(SDBMSG_DBMSRV_NODBNAME);
        // ready with error
        return false;
    } // end if

    // special handling for rundirectory to make sure to always get a complete absolute path http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1141074
    if (strcmp(XpKeyCIn, PAN_RUNDIR) == 0) {
        if(!oConfigFile.GetRundirectoryAbsolutePath((DBMSrvPar_RTEConfParameter::String) xpRecord.xpValue.xpValueString.asCharp(), oDBMError)) {
            oDBMError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_XPNOTFOUND, Msg_Arg(SDBMSGTAG_DBMSRV_XPNOTFOUND__PARAMETER, PAN_RUNDIR)));
            return false;
        }
        // convert parameter
        xpRecord.xpType = DBMSrvPar_RTEConfParameter::t_String;
        cn29ParamValueToString(XpValueC, &xpRecord);

        // ready with success
        return true;
    }

    // read configuration
    if (!oConfigFile.Read(oDBMError)) {
        if (oDBMError == DBMSrvMsg_Error::DBMSrv_FILEOPEN) {
            oDBMError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_XPNOTFOUND, Msg_Arg(SDBMSGTAG_DBMSRV_XPNOTFOUND__PARAMETER, PAN_PARAM_KERNELVERSION)));
        } // end if
        return false;
    } // end if

    // special handling for change value
    if (strcmp(XpKeyCIn, PAN_XPARAM_CHANGED) == 0) {
        DBMSrvPar_RTEConfParameter::TimeStamp   aTime;
        XpValueC.rawAssign(oConfigFile.GetLastVerifyTime(aTime) ? "0" : "1");
        // ready with success
        return true;
    } // end if

    // special handling for modified value
    if (strcmp(XpKeyCIn, PAN_PARAM_VERSION_TIME) == 0) {
        DBMSrvPar_RTEConfParameter::TimeStamp   aTime;
        oConfigFile.GetLastVerifyTime(aTime);
        sprintf(XpValueC,
                "%04d-%02d-%02d %02d:%02d:%02d",
                (int) aTime.Year,
                (int) aTime.Month,
                (int) aTime.Day,
                (int) aTime.Hour,
                (int) aTime.Minute,
                (int) aTime.Second);
        // ready with success
        return true;
    } // end if

    // special handling for crahs flag
    if (strcmp(XpKeyCIn, PAN_CRASH_SEM) == 0) {
       XpValueC.rawAssign(oConfigFile.GetOriginalCrashFlag() ? "1" : "0");
       // ready with success
       return true;
    } // end if

    if (!oConfigFile.GetType ((const SAPDB_UTF8 *) XpKeyCIn, xpRecord.xpType , oDBMError)) {
        // ready with error
        return false;
    } // end if

    // read the parameter from the configuration
    switch (xpRecord.xpType) {
        case DBMSrvPar_RTEConfParameter::t_Integer:
            oConfigFile.GetValue ((const SAPDB_UTF8 *) XpKeyCIn, xpRecord.xpValue.xpValueInt, oDBMError);
            break;
        case DBMSrvPar_RTEConfParameter::t_Real:        
            oConfigFile.GetValue ((const SAPDB_UTF8 *) XpKeyCIn, xpRecord.xpValue.xpValueReal, oDBMError);
            break;
        case DBMSrvPar_RTEConfParameter::t_String:
            oConfigFile.GetValue ((const SAPDB_UTF8 *) XpKeyCIn, (DBMSrvPar_RTEConfParameter::String) xpRecord.xpValue.xpValueString.asCharp(), oDBMError);
            break;
        case DBMSrvPar_RTEConfParameter::t_CryptInfo:  
            oConfigFile.GetValue ((const SAPDB_UTF8 *) XpKeyCIn, xpRecord.xpValue.xpValueCrypt, oDBMError);
            break;
    } // end switch

    // check for error 
    if (!oDBMError.IsEmpty()) {
        // ready with error
        return false;
    } // end if

     // convert parameter
     cn29ParamValueToString(XpValueC, &xpRecord);

    // ready with success
    return true;
} /* end c20XParamGetValue */

tcn00_Error cn20XParamGetValue
      ( const char *               DBNameCIn,
        const char *               XpKeyCIn,
        tcn002_XpValueString     & XpValueC ) 
{
    DBMSrvMsg_Error oError;
    if (!cn20XParamGetValue(DBNameCIn, XpKeyCIn, XpValueC, oError)) {
        return oError.ID();
    } // end if
    return OK_CN00;
} // end cn20XParamGetValue


/* INTERN FUNCTION cn20XParamGetValue
 * ------------------------------------------------------------------
 *
 */
bool cn20XParamGetValue
      ( const char *               DBNameCIn,
        const char *               XpKeyCIn,
        tsp00_Int4               & XpNum,
        DBMSrvMsg_Error          & oDBMError )
{
    tcn002_XpValueString  XpValueC;

    XpNum = 0;

    if (!cn20XParamGetValue ( DBNameCIn, XpKeyCIn, XpValueC, oDBMError)) {
        return false;
    } // end if

    XpNum = atol(XpValueC);

    return true;
} /* end c20XParamGetValue */

tcn00_Error cn20XParamGetValue
      ( const char *               DBNameCIn,
        const char *               XpKeyCIn,
        tsp00_Int4               & XpNum )
{
    DBMSrvMsg_Error oError;
    if (!cn20XParamGetValue(DBNameCIn, XpKeyCIn, XpNum, oError)) {
        return oError.ID();
    } // end if
    return OK_CN00;
} // end cn20XParamGetValue


/* INTERN FUNCTION cn20XParamPutValue
 * ------------------------------------------------------------------
 *
 */
tcn00_Error cn20XParamPutValue
      ( const tsp00_DbNamec        & DBNameCIn,
        const tcn002_XpValueName   & XpKeyCIn,
        const tcn002_XpValueString & XpValueC )
{
  tcn00_Error          nFuncReturn = OK_CN00;
  DBMSrvMsg_Error      oMsg;
  DBMSrvPar_Parameter::XPRecord      xpRecord;
  DBMSrvPar_RTEConfParameter    oConfigFile(DBNameCIn.asCharp(), 
                                   0, 
                                   RTECONF_MAXNAMELENGTH,
                                   RTECONF_MAXSTRINGLENGTH);
  
  if (oConfigFile.Read(oMsg)) {
    if (oConfigFile.GetType ((const SAPDB_UTF8 *) XpKeyCIn.asCharp(), xpRecord.xpType , oMsg)) {
      SAPDB_Bool bUpdateOnline = oConfigFile.UpdateOnlinePossible ((const SAPDB_UTF8 *) XpKeyCIn.asCharp());

      cn29StringToParamValue(&xpRecord, XpValueC);

      switch (xpRecord.xpType) {
        case DBMSrvPar_RTEConfParameter::t_Integer:
          oConfigFile.SetValue ((const SAPDB_UTF8 *) XpKeyCIn.asCharp(), xpRecord.xpValue.xpValueInt, bUpdateOnline, true, oMsg);
          break;
        case DBMSrvPar_RTEConfParameter::t_Real:        
          oConfigFile.SetValue ((const SAPDB_UTF8 *) XpKeyCIn.asCharp(), xpRecord.xpValue.xpValueReal, bUpdateOnline, true, oMsg);
          break;
        case DBMSrvPar_RTEConfParameter::t_String:
          {
            DBMSrvPar_RTEConfParameter::String pString = (DBMSrvPar_RTEConfParameter::String) xpRecord.xpValue.xpValueString.asCharp();
            oConfigFile.SetValue ((const SAPDB_UTF8 *) XpKeyCIn.asCharp(), pString, bUpdateOnline, true, oMsg);
          }
          break;
        case DBMSrvPar_RTEConfParameter::t_CryptInfo:  
          oConfigFile.SetValue ((const SAPDB_UTF8 *) XpKeyCIn.asCharp(), xpRecord.xpValue.xpValueCrypt, bUpdateOnline, true, oMsg);
          break;
      } // end switch
    } // end if
  } // end if

  if (oMsg.IsEmpty()) {
    oConfigFile.Write(oMsg);
  } // end if
  if (!oMsg.IsEmpty()) {
    nFuncReturn = oMsg.ID();
  } // end if

  return nFuncReturn;
} /* end cn20XParamPutValue */

/* INTERN FUNCTION cn20XParamPutValue
 * ------------------------------------------------------------------
 *
 */
tcn00_Error cn20XParamPutValue
      ( const tsp00_DbNamec      & DBNameC,
        const tcn002_XpValueName & XpKeyC,
        const tsp00_Int4         & XpNum )
{
  tcn002_XpValueString XpValueC;

  _stprintf(XpValueC, "%d", XpNum);

  return cn20XParamPutValue ( DBNameC, XpKeyC, XpValueC);

} /* end cn20XParamPutValue */

/* INTERN FUNCTION cn20_XParamPutValue
 * ------------------------------------------------------------------
 *
 */
static tcn00_Error cn20_XParamPutValue
      ( const tsp00_DbNamec      & DBNameCIn,
        const tcn002_XpValueName & XpKeyCIn,
        DBMSrvPar_Parameter      * pParameter)
{
  tcn00_Error          nFuncReturn = OK_CN00;
  DBMSrvMsg_Error      oMsg;
  DBMSrvPar_RTEConfParameter    oConfigFile(DBNameCIn.asCharp(), 
                                   0, 
                                   RTECONF_MAXNAMELENGTH,
                                   RTECONF_MAXSTRINGLENGTH);
  
  if (oConfigFile.Read(oMsg)) {
    switch (pParameter->xpRecord.xpType) {
      case DBMSrvPar_RTEConfParameter::t_Integer:
        oConfigFile.SetValue ((const SAPDB_UTF8 *) XpKeyCIn.asCharp(), 
                              pParameter->xpRecord.xpValue.xpValueInt, 
                              (pParameter->xpProperties[XPPropClassChange] == DBMSrvPar_Parameter::XPPropValueRunning), 
                              true,
                              oMsg);
        break;
      case DBMSrvPar_RTEConfParameter::t_Real:        
        oConfigFile.SetValue ((const SAPDB_UTF8 *) XpKeyCIn.asCharp(), 
                              pParameter->xpRecord.xpValue.xpValueReal, 
                              (pParameter->xpProperties[XPPropClassChange] == DBMSrvPar_Parameter::XPPropValueRunning), 
                              true,
                              oMsg);
        break;
      case DBMSrvPar_RTEConfParameter::t_String:
        {
          DBMSrvPar_RTEConfParameter::String pString = (DBMSrvPar_RTEConfParameter::String) pParameter->xpRecord.xpValue.xpValueString.asCharp();
          oConfigFile.SetValue ((const SAPDB_UTF8 *) XpKeyCIn.asCharp(), 
                                pString, 
                                (pParameter->xpProperties[XPPropClassChange] == DBMSrvPar_Parameter::XPPropValueRunning), 
                                true,
                                oMsg);
        }
        break;
      case DBMSrvPar_RTEConfParameter::t_CryptInfo:  
        oConfigFile.SetValue ((const SAPDB_UTF8 *) XpKeyCIn.asCharp(), 
                              pParameter->xpRecord.xpValue.xpValueCrypt, 
                              (pParameter->xpProperties[XPPropClassChange] == DBMSrvPar_Parameter::XPPropValueRunning), 
                              true,
                              oMsg);
        break;
    } // end switch
  } // end if

  if (oMsg.IsEmpty()) {
    oConfigFile.Write(oMsg);
  } // end if

  if (!oMsg.IsEmpty()) {
      nFuncReturn = oMsg.ID();
  } else {
      cn29WriteParamHistory(DBNameCIn, pParameter, PAHI_CHANGE_CN002);
  } // end if

  return nFuncReturn;
} /* end cn20_XParamPutValue */

/* INTERN FUNCTION cn20XParamGetValue
 * ------------------------------------------------------------------
 *
 */
tcn00_Error cn20XParamGetValue
      ( VControlDataT * vcontrol,
        const char    * szParameter,
        char          * szValue,
        char          * szError)
{
  tcn00_Error   nFuncReturn = OK_CN00;
  char          szReply[256];
  char        * pReply = &szReply[0];
  char        * pValue = &szValue[0];
  int           nLen   = 0;

  nFuncReturn = cn29ParamGetIntern( vcontrol->dbname, STATIC_CAST(DBMSrvPar_Parameters *, vcontrol->pXParamSection), szParameter, szReply, &nLen, 256, XPRequestValueIntern);

  if (nFuncReturn == OK_CN00) {
    /* skip the OK line */
    while (*pReply != CHAR_STRINGTERM_CN90 && *pReply != '\n') {
      pReply++;
    } /* end while */
    if (*pReply != CHAR_STRINGTERM_CN90) {
      pReply++;
    } /* end if */
    while (*pReply != CHAR_STRINGTERM_CN90 && *pReply != '\n') {
      *pValue = *pReply;
      pValue++;
      pReply++;
    } /* end while */
    *pValue = CHAR_STRINGTERM_CN90;
  } else {
    if (szError != 0) {
      SAPDB_strcpy(szError, pReply);
    } /* end if */
  } /* end if */

  return nFuncReturn;

} /* end cn20XParamGetValue */

/* INTERN FUNCTION cn20XParamDelete
 * ------------------------------------------------------------------
 *
 */
tcn00_Error cn20XParamDelete
      ( const tsp00_DbNamec      & DBNameCIn,
        const tcn002_XpValueName & XpKeyCIn )
{
  tcn00_Error             nFuncReturn = OK_CN00;
  DBMSrvMsg_Error         oMsg;
  DBMSrvPar_RTEConfParameter::Type aType;
  DBMSrvPar_RTEConfParameter       oConfigFile(DBNameCIn.asCharp(), 
                                      0, 
                                      RTECONF_MAXNAMELENGTH,
                                      RTECONF_MAXSTRINGLENGTH);
  
  if (oConfigFile.Read(oMsg)) {
    if (oConfigFile.GetType ((const SAPDB_UTF8 *) XpKeyCIn.asCharp(), aType , oMsg)) {
      oConfigFile.Delete((const SAPDB_UTF8 *) XpKeyCIn.asCharp(), oMsg);
    } // end if
  } // end if

  if (oMsg.IsEmpty()) {
    oConfigFile.Write(oMsg);
  } // end if

  if (!oMsg.IsEmpty()) {
    nFuncReturn = oMsg.ID();
  } // end if

  return nFuncReturn;
} /* end cn20XParamDelete */

/* INTERN FUNCTION cn20XParamGetServiceValues
 * ------------------------------------------------------------------
 *
 */
tcn00_Error cn20XParamGetServiceValues
      ( tsp00_DbNamec     & szDBName,
        const tsp00_Pathc & szDbRoot)
{
  tcn00_UserNamec  szUserName;
  tsp00_Namec      szPassword;
  
   return cn20XParamGetServiceValues ( szDBName,
                                       szDbRoot,
                                       szUserName,
                                       szPassword );

} // end cn20XParamGetServiceValues

/* INTERN FUNCTION cn20XParamGetServiceValues
 * ------------------------------------------------------------------
 *
 */
tcn00_Error cn20XParamGetServiceValues
      ( tsp00_DbNamec     & szDBName,
        const tsp00_Pathc & szDbRoot,
        tcn00_UserNamec   & szUserName,
        tsp00_Namec       & szPassword )
{
  tcn00_Error          nFuncReturn = OK_CN00;
  DBMSrvMsg_Error      oMsg;
  DBMSrvPar_Parameter::XPRecord      xpRecord;
  tcn002_XpValueString szTemp;
  
  cn90ServiceDB(szDBName, szDbRoot);
  if (szDBName.length() == 0) {
    nFuncReturn = ERR_SERVICEDBNOTFOUND_CN00;
  } // end if

  if (nFuncReturn == OK_CN00) {
    DBMSrvPar_RTEConfParameter    oConfigFile(szDBName.asCharp(), 
                                     0, 
                                     RTECONF_MAXNAMELENGTH,
                                     RTECONF_MAXSTRINGLENGTH);

    if (oConfigFile.Read(oMsg)) {

      if (oConfigFile.GetType ((SAPDB_UTF8 *) PAN_CONTROLID, xpRecord.xpType , oMsg)) {
        if (oConfigFile.GetValue ((SAPDB_UTF8 *) PAN_CONTROLID, (DBMSrvPar_RTEConfParameter::String) xpRecord.xpValue.xpValueString.asCharp(), oMsg)) {
          cn29ParamValueToString(szTemp, &xpRecord);
          SAPDB_strcpy(szUserName, szTemp);
        } else {
          nFuncReturn = ERR_XPNOTFOUND_CN00;
        } // end if
      } else {
        nFuncReturn = ERR_XPNOTFOUND_CN00;
      } // end if

      if (oConfigFile.GetType ((SAPDB_UTF8 *) PAN_CONTROLPW, xpRecord.xpType , oMsg)) {
        if (oConfigFile.GetValue ((SAPDB_UTF8 *) PAN_CONTROLPW, xpRecord.xpValue.xpValueCrypt, oMsg)) {
          cn29ParamValueToString(szTemp, &xpRecord);
          SAPDB_strcpy(szPassword, szTemp);
        } else {
          nFuncReturn = ERR_XPNOTFOUND_CN00;
        } // end if
      } else {
        nFuncReturn = ERR_XPNOTFOUND_CN00;
      } // end if
    } else {
      nFuncReturn = DBMSrvMsg_Error::get_tcn00_Error(DBMSrvMsg_Error::DBMSrv_RTEEXT);
    } // end if
  } // end if

  return nFuncReturn;
} /* end cn20XParamGetServiceValues */


/* PUBLIC FUNCTION cn20ParamDirectGet
 * ------------------------------------------------------------------
 * purpose: get named parameter with value from param file
 *
 */
tcn00_Error cn20ParamDirectGet
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax )
{
  char szParameter[RTECONF_MAXNAMELENGTH + 1];

  cn90GetToken(command->args, szParameter, 1, RTECONF_MAXNAMELENGTH + 1);
  cn90StrUpperCopy(szParameter, szParameter, false);
  return cn29ParamGetIntern( vcontrol->dbname,STATIC_CAST(DBMSrvPar_Parameters *, vcontrol->pXParamSection), szParameter, replyData, replyLen, replyLenMax, XPRequestDirectGet);
} /* end cn20ParamDirectGet */

/* PUBLIC FUNCTION cn20ParamDirectGetAll
 * ------------------------------------------------------------------
 * purpose: get all parameters with values from param file
 *
 */
tcn00_Error cn20ParamDirectGetAll
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax )
{
  return cn29ParamGetAllIntern( vcontrol->dbname, STATIC_CAST(DBMSrvPar_Parameters *, vcontrol->pXParamSection), command->args, replyData, replyLen, replyLenMax, false, false);
} /* end cn20ParamDirectGetAll */

/* PUBLIC FUNCTION cn20ParamDirectGetAllNext
 * ------------------------------------------------------------------
 * purpose: get all parameters with values from param file
 *
 */
tcn00_Error cn20ParamDirectGetAllNext
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax )
{
  return cn29ParamGetAllIntern( vcontrol->dbname, STATIC_CAST(DBMSrvPar_Parameters *, vcontrol->pXParamSection), command->args, replyData, replyLen, replyLenMax, false, true);
} /* end cn20ParamDirectGetAllNext */

/* PUBLIC FUNCTION cn20_ParamDirectPut
 * ------------------------------------------------------------------
 * purpose: write one named paramter with value to param file
 *
 */
static tcn00_Error cn20_ParamDirectPut
      ( VControlDataT * vcontrol,
        const char    * szParameter,
        const char    * szValue,
        const char    * szComment,  
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error         nFuncReturn = OK_CN00;
  DBMSrvPar_Parameters * pXParamData = (DBMSrvPar_Parameters *) vcontrol->pXParamSection;
  DBMSrvPar_Parameter * pParameter  = NULL;
  DBMSrvPar_Parameter * pDynHelp    = NULL;
  DBMSrvPar_Parameter * pTemp       = NULL;
  bool                bInit  = false;

  /* init param management for commandline use or tcontrol or rcontrol */
  if (pXParamData == NULL) {
    nFuncReturn = cn29ParamStartSession( vcontrol->dbname, pXParamData, replyData, replyLen, replyLenMax);
    bInit = true;
  } /* end if */

  if (nFuncReturn == OK_CN00) {

    /* search parameter in List */
    pParameter = pXParamData->Get(szParameter);

    if (pParameter == NULL) {
      pDynHelp = cn29SearchDynamic(pXParamData->pFirst, szParameter);

      /* is there an dynamic match */
      if (pDynHelp != NULL) {
        pParameter = new DBMSrvPar_Parameter;
        if (pParameter == NULL) {
          nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_MEM_CN00);
        } else {
          /* copy data from dynamic record */
          nFuncReturn = cn29CopyParameter(pParameter, pDynHelp);
          cn90AnswerIError(replyData, replyLen, nFuncReturn);
          /* clear the dynamic/intern property */
          pParameter->xpProperties[XPPropClassDynamic] = DBMSrvPar_Parameter::XPPropValueNo;
          pParameter->xpProperties[XPPropClassIntern] = DBMSrvPar_Parameter::XPPropValueNo;
          /* set the real name */
          cn90SaveStrcpy(pParameter->szParameterName, szParameter, sizeof(tcn002_XpValueName));

          /* container management */
          pParameter->pNext = pDynHelp->pNext;
          pDynHelp->pNext   = pParameter;
          pXParamData->Put(pParameter);
        } /* end if */
      } /* end if */
    } /* end if */

    /* write the parameter */
    if (pParameter != NULL) {

      nFuncReturn = cn29CheckRange(pParameter, szValue);
      cn90AnswerIError(replyData, replyLen, nFuncReturn);

      if (nFuncReturn == OK_CN00) {
        DBMSrvMsg_Error oMsg;
        DBMSrvPar_RTEConfParameter    oConfigFile(vcontrol->dbname.asCharp(), 
                                         0, 
                                         RTECONF_MAXNAMELENGTH,
                                         RTECONF_MAXSTRINGLENGTH);

        if (!oConfigFile.Read(oMsg)) {
		    if (oMsg == DBMSrvMsg_Error::DBMSrv_FILEOPEN) {
			    oMsg.ClearMessageList();
		    } else {
			    nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(oMsg);
		    } // end if
        } // end if 

        if (nFuncReturn == OK_CN00) {
          cn90StrUpperCopy (pParameter->szNewValue, szValue,
                            (bool   ) (pParameter->xpProperties[XPPropClassCasesensitive] == DBMSrvPar_Parameter::XPPropValueYes));
          pParameter->szComment.rawAssign(szComment);
          pParameter->nCalcState = DBMSrvPar_Parameter::XPCalcStateUnknown; 
          cn29StringToParamValue(&(pParameter->xpRecord), pParameter->szNewValue);

          switch (pParameter->xpRecord.xpType) {
            case DBMSrvPar_RTEConfParameter::t_Integer:
              oConfigFile.SetValue ((const SAPDB_UTF8 *) pParameter->szParameterName.asCharp(), 
                                    pParameter->xpRecord.xpValue.xpValueInt, 
                                    (pParameter->xpProperties[XPPropClassChange] == DBMSrvPar_Parameter::XPPropValueRunning), 
                                    true,
                                    oMsg);
              break;
            case DBMSrvPar_RTEConfParameter::t_Real:        
              oConfigFile.SetValue ((const SAPDB_UTF8 *) pParameter->szParameterName.asCharp(), 
                                    pParameter->xpRecord.xpValue.xpValueReal, 
                                    (pParameter->xpProperties[XPPropClassChange] == DBMSrvPar_Parameter::XPPropValueRunning), 
                                    true,
                                    oMsg);
              break;
            case DBMSrvPar_RTEConfParameter::t_String:
              {
                DBMSrvPar_RTEConfParameter::String pString = (DBMSrvPar_RTEConfParameter::String) pParameter->xpRecord.xpValue.xpValueString.asCharp();
                oConfigFile.SetValue ((const SAPDB_UTF8 *) pParameter->szParameterName.asCharp(), 
                                      pString, 
                                      (pParameter->xpProperties[XPPropClassChange] == DBMSrvPar_Parameter::XPPropValueRunning), 
                                      true,
                                      oMsg);
              }
              break;
            case DBMSrvPar_RTEConfParameter::t_CryptInfo:  
              oConfigFile.SetValue ((const SAPDB_UTF8 *) pParameter->szParameterName.asCharp(), 
                                    pParameter->xpRecord.xpValue.xpValueCrypt, 
                                    (pParameter->xpProperties[XPPropClassChange] == DBMSrvPar_Parameter::XPPropValueRunning), 
                                    true,
                                    oMsg);
              break;
          } // end switch

          if (!oMsg.IsEmpty()) {
            nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(oMsg);
          } else {
            if (!oConfigFile.Write(oMsg)) {
              nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(oMsg);
            } else {
              cn29WriteParamHistory(vcontrol->dbname, pParameter, PAHI_CHANGE_CN002);
            } // end if
          } // end if

        } // end if

      } // end if

    } else {
      nFuncReturn = cn29AnswerParamNotFound(replyData, replyLen, szParameter, pXParamData->pFirst);
    } /* end if */

    if (nFuncReturn == OK_CN00) {
      if (strcmp(szParameter, PAN_CONTROLID) == 0) {
        tcn00_UserNamec szUser;
        szUser.rawAssign(szValue);
        cn50DBMUser  usrDBM(vcontrol->dbname, szUser);
        usrDBM.setColdUser();
        usrDBM.save();

      } // end if
      cn90AnswerOK(replyData, replyLen, NULL);
    } /* end if */
  } /* end if */

  /* if init then stop param management */
  if (bInit && (pXParamData != NULL)) {
    delete pXParamData;
    pXParamData = NULL;
  } /* end if */

  *replyLen = (int)strlen(replyData);

  return nFuncReturn;
} /* end cn20_ParamDirectPut */

/* PUBLIC FUNCTION cn20ParamDirectPut
 * ------------------------------------------------------------------
 * purpose: write one named paramter with value to param file
 *
 */
tcn00_Error cn20ParamDirectPut
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn002_XpValueName     szParameter;
  tcn002_XpValueString   szValue;
  tsp00_C80c             szComment;

  cn90GetToken(command->args, szParameter, 1, szParameter.size() + 1);
  cn90StrUpperCopy(szParameter, szParameter, false);
  cn90GetToken(command->args, szValue, 2, szValue.size() + 1);
  cn90StripQuotes(szValue);
  if (cn90GetToken(command->args, szComment, 3, szComment.size() + 1)) {
    cn90StripQuotes(szComment);
  } else {
    szComment.Init();
  } // end if

  return cn20_ParamDirectPut( vcontrol,
                              szParameter,
                              szValue,
                              szComment,
                              replyData,
                              replyLen,
                              replyLenMax);

} /* end cn20ParamDirectPut */

/* PUBLIC FUNCTION cn20_ParamDirectDel
 * ------------------------------------------------------------------
 * purpose: delete one named paramter from the param file
 */
static tcn00_Error cn20_ParamDirectDel
      ( VControlDataT * vcontrol,
        const char    * szParameter,
        const char    * szComment,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax,
        bool            parameterMustExist)
{
  tcn00_Error         nFuncReturn = OK_CN00;
  DBMSrvPar_Parameters * pXParamData = (DBMSrvPar_Parameters *) vcontrol->pXParamSection;
  DBMSrvPar_Parameter * pParameter  = NULL;
  DBMSrvPar_Parameter * pDynHelp    = NULL;
  DBMSrvPar_Parameter * pTemp       = NULL;
  DBMSrvPar_Parameter * pPrev       = NULL;
  
  /* init param management for commandline use or tcontrol or rcontrol */
  if (pXParamData == NULL) {
    nFuncReturn = cn29ParamStartSession( vcontrol->dbname, pXParamData, replyData, replyLen, replyLenMax);
  } else {
    nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_XPSESSION_CN00);
    pXParamData = NULL;
  } /* end if */

  if (nFuncReturn == OK_CN00) {

    /* search parameter in List */
    pParameter = pXParamData->Get(szParameter);

    // skip internal parameters
    if (pParameter != NULL) {
      pParameter = (pParameter->bParamExist) ? pParameter : NULL;
    } // end if

    /* delete the parameter */
    if (pParameter != NULL) {

      DBMSrvMsg_Error               oMsg;
      DBMSrvPar_RTEConfParameter    oConfigFile(vcontrol->dbname.asCharp(), 
                                       0, 
                                       RTECONF_MAXNAMELENGTH,
                                       RTECONF_MAXSTRINGLENGTH);

      if (oConfigFile.Read(oMsg)) {
        if (oConfigFile.Delete ((const SAPDB_UTF8 *) pParameter->szParameterName.asCharp(), oMsg)) {
           oConfigFile.Write (oMsg);
        } // end if
      } // end if

      if (!oMsg.IsEmpty()) {
        nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(oMsg);
      } else {
        pParameter->szComment.rawAssign(szComment);
        cn29WriteParamHistory(vcontrol->dbname, pParameter, PAHI_DELETE_CN002);
      } // end if

    } else
    {
        if(parameterMustExist)
            nFuncReturn=cn29AnswerParamNotFound(replyData, replyLen, szParameter, pXParamData->pFirst, true);
        else
            nFuncReturn=OK_CN00; //if the parameter must not exist before deletion an can not be found everything is OK
    } /* end if */

    if (nFuncReturn == OK_CN00) {
      cn90AnswerOK(replyData, replyLen, NULL);
    } /* end if */
  } /* end if */

  /* stop param management */
  if (pXParamData != NULL) {
    delete pXParamData;
    pXParamData = NULL;
  } /* end if */

  *replyLen = (int)strlen(replyData);

  return nFuncReturn;
} /* end  cn20_ParamDirectDel */

/* PUBLIC FUNCTION cn20ParamDirectDel
 * ------------------------------------------------------------------
 * purpose: delete one named paramter from the param file
 *
 */
tcn00_Error cn20ParamDirectDel
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn002_XpValueName     szParameter;
  tsp00_C80c             szComment;

  cn90GetToken(command->args, szParameter, 1, szParameter.size() + 1);
  cn90StrUpperCopy(szParameter, szParameter, false);
  if (cn90GetToken(command->args, szComment, 2, szComment.size() + 1)) {
    cn90StripQuotes(szComment);
  } else {
    szComment.Init();
  } // end if

  return cn20_ParamDirectDel( vcontrol,
                             szParameter,
                             szComment,
                             replyData,
                             replyLen,
                             replyLenMax,
                             true);

} /* end cn20ParamDirectDel */

/* PUBLIC FUNCTION cn20ParamStartSession
 * ------------------------------------------------------------------
 * purpose: initialize a XParam-Session
 *
 */
tcn00_Error cn20ParamStartSession
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  DBMSrvPar_Parameters * pXParam     = STATIC_CAST(DBMSrvPar_Parameters *, vcontrol->pXParamSection);
  tcn00_Error         nFuncReturn = cn29ParamStartSession(vcontrol->dbname, pXParam, replyData, replyLen, replyLenMax);

  vcontrol->pXParamSection = STATIC_CAST(void *, pXParam);

  return nFuncReturn;
} /* end cn20ParamStartSession */

/* PUBLIC FUNCTION cn20ParamExtGetAll
 * ------------------------------------------------------------------
 * purpose: get a all parameters with extended information
 *
 */
tcn00_Error cn20ParamExtGetAll
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  return cn29ParamGetAllIntern( vcontrol->dbname, STATIC_CAST(DBMSrvPar_Parameters *, vcontrol->pXParamSection), command->args, replyData, replyLen, replyLenMax, true, false);
} /* end cn20ParamExtGetAll */

/* PUBLIC FUNCTION cn20ParamExtGetAllNext
 * ------------------------------------------------------------------
 * purpose: get a all parameters with extended information
 *
 */
tcn00_Error cn20ParamExtGetAllNext
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  return cn29ParamGetAllIntern( vcontrol->dbname, STATIC_CAST(DBMSrvPar_Parameters *, vcontrol->pXParamSection), command->args, replyData, replyLen, replyLenMax, true, true);
} /* end cn20ParamExtGetAllNext */

/* PUBLIC FUNCTION cn20ParamExtGet
 * ------------------------------------------------------------------
 * purpose: get a named parameter with extended information
 *
 */
tcn00_Error cn20ParamExtGet
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  char szParameter[RTECONF_MAXNAMELENGTH + 1];

  cn90GetToken(command->args, szParameter, 1, RTECONF_MAXNAMELENGTH + 1);
  cn90StrUpperCopy(szParameter, szParameter, false);
  return cn29ParamGetIntern( vcontrol->dbname, STATIC_CAST(DBMSrvPar_Parameters *, vcontrol->pXParamSection), szParameter, replyData, replyLen, replyLenMax, XPRequestExtGet);
} /* end cn20ParamExtGet */

/* PUBLIC FUNCTION cn20ParamGetType
 * ------------------------------------------------------------------
 * purpose: get type of parameter
 */
tcn00_Error cn20ParamGetType
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax )
{
  char szParameter[RTECONF_MAXNAMELENGTH + 1];

  cn90GetToken(command->args, szParameter, 1, RTECONF_MAXNAMELENGTH + 1);
  cn90StrUpperCopy(szParameter, szParameter, false);
  return cn29ParamGetIntern( vcontrol->dbname, STATIC_CAST(DBMSrvPar_Parameters *, vcontrol->pXParamSection), szParameter, replyData, replyLen, replyLenMax, XPRequestType);
} /* end cn20ParamGetType */

/* PUBLIC FUNCTION cn20ParamGetDefault
 * ------------------------------------------------------------------
 * purpose: get default value of parameter
 */
tcn00_Error cn20ParamGetDefault
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax )
{
  char szParameter[RTECONF_MAXNAMELENGTH + 1];

  cn90GetToken(command->args, szParameter, 1, RTECONF_MAXNAMELENGTH + 1);
  cn90StrUpperCopy(szParameter, szParameter, false);
  return cn29ParamGetIntern( vcontrol->dbname, STATIC_CAST(DBMSrvPar_Parameters *, vcontrol->pXParamSection), szParameter, replyData, replyLen, replyLenMax, XPRequestDefault);
} /* end cn20ParamGetDefault */

/* PUBLIC FUNCTION cn20ParamGetValue
 * ------------------------------------------------------------------
 * purpose: get value of parameter
 */
tcn00_Error cn20ParamGetValue
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax )
{
  char szParameter[RTECONF_MAXNAMELENGTH + 1];

  cn90GetToken(command->args, szParameter, 1, RTECONF_MAXNAMELENGTH + 1);
  cn90StrUpperCopy(szParameter, szParameter, false);
  return cn29ParamGetIntern( vcontrol->dbname, STATIC_CAST(DBMSrvPar_Parameters *, vcontrol->pXParamSection), szParameter, replyData, replyLen, replyLenMax, XPRequestValue);
} /* end cn20ParamGetValue */

/* PUBLIC FUNCTION cn20ParamGetProperties
 * ------------------------------------------------------------------
 * purpose: get properties of parameter
 */
tcn00_Error cn20ParamGetProperties
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax )
{
  char szParameter[RTECONF_MAXNAMELENGTH + 1];

  cn90GetToken(command->args, szParameter, 1, RTECONF_MAXNAMELENGTH + 1);
  cn90StrUpperCopy(szParameter, szParameter, false);
  return cn29ParamGetIntern( vcontrol->dbname, STATIC_CAST(DBMSrvPar_Parameters *, vcontrol->pXParamSection), szParameter, replyData, replyLen, replyLenMax, XPRequestProperties);
} /* end cn20ParamGetProperties */

/* PUBLIC FUNCTION cn20ParamGetHelp
 * ------------------------------------------------------------------
 * purpose: get explain section of parameter
 */
tcn00_Error cn20ParamGetHelp
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax )
{
  char szParameter[RTECONF_MAXNAMELENGTH + 1];

  cn90GetToken(command->args, szParameter, 1, RTECONF_MAXNAMELENGTH + 1);
  cn90StrUpperCopy(szParameter, szParameter, false);
  return cn29ParamGetIntern( vcontrol->dbname,STATIC_CAST(DBMSrvPar_Parameters *, vcontrol->pXParamSection), szParameter, replyData, replyLen, replyLenMax, XPRequestHelp);
} /* end cn20ParamGetHelp */

/* PUBLIC FUNCTION cn20ParamGetExplain
 * ------------------------------------------------------------------
 * purpose: get explain section of parameter
 */
tcn00_Error cn20ParamGetExplain
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax )
{
  char szParameter[RTECONF_MAXNAMELENGTH + 1];

  cn90GetToken(command->args, szParameter, 1, RTECONF_MAXNAMELENGTH + 1);
  cn90StrUpperCopy(szParameter, szParameter, false);
  return cn29ParamGetIntern( vcontrol->dbname, STATIC_CAST(DBMSrvPar_Parameters *, vcontrol->pXParamSection), szParameter, replyData, replyLen, replyLenMax, XPRequestExplain);
} /* end cn20ParamGetExplain */

/* PUBLIC FUNCTION cn20ParamGetFull
 * ------------------------------------------------------------------
 * purpose: get all data of parameter
 */
tcn00_Error cn20ParamGetFull
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax )
{
  char szParameter[RTECONF_MAXNAMELENGTH + 1];

  cn90GetToken(command->args, szParameter, 1, RTECONF_MAXNAMELENGTH + 1);
  cn90StrUpperCopy(szParameter, szParameter, false);
  return cn29ParamGetIntern( vcontrol->dbname, STATIC_CAST(DBMSrvPar_Parameters *, vcontrol->pXParamSection), szParameter, replyData, replyLen, replyLenMax, XPRequestFull);
} /* end cn20ParamGetFull */

/* PUBLIC FUNCTION cn20ParamCheckAll
 * ------------------------------------------------------------------
 * purpose: check of all parameters
 */
tcn00_Error cn20ParamCheckAll
      ( tsp00_DbNamec          szDbName,
        DBMSrvPar_Parameters * pXParamData,
        const char           * szGroup,
        char                 * replyData,
        int                  * replyLen,
        int                    replyLenMax)
{
  tcn00_Error         nFuncReturn = OK_CN00;
  DBMSrvPar_Parameter * pParameter  = NULL;
  DBMSrvPar_Parameter::XPPropValue  nGroup = DBMSrvPar_Parameter::XPPropValueUnknown;
  tsp00_Versionc      VersionString;
  bool                bInit = false;

  /* init param management for commandline use or tcontrol or rcontrol */
  if (pXParamData == NULL) {
    nFuncReturn = cn29ParamStartSession( szDbName, pXParamData, replyData, replyLen, replyLenMax);
    bInit = true;
  } /* end if */

  /* look for request mode */
  if (nFuncReturn == OK_CN00) {
    if (_tcslen(szGroup) > 0) {
      if (strcmp(szGroup, XP_PROP_SUPPORT_CN002) == 0) {
        nGroup    = DBMSrvPar_Parameter::XPPropValueSupport;
      } else if (strcmp(szGroup, XP_PROP_EXTENDED_CN002) == 0) {
        nGroup    = DBMSrvPar_Parameter::XPPropValueExtended;
      } else if (strcmp(szGroup, XP_PROP_GENERAL_CN002) == 0) {
        nGroup    = DBMSrvPar_Parameter::XPPropValueGeneral;
      } else {
        nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00);
      } /* end if */
    } /* end if */
  } /* end if */

  /* calculate all parameters */
  if (nFuncReturn == OK_CN00) {

    /* KERNELVERSION   */
    pParameter = pXParamData->Get(PAN_PARAM_KERNELVERSION);
    if (strlen(pParameter->szDefValue) > 0) {
      VersionString.rawAssign(pParameter->szDefValue);
    } else {
      sp100_GetVersionString(COMP_NAME_KERNEL_SP100, s100buildnumber, VersionString);
    } // end if
    cn29SoftPutIntern( pXParamData, PAN_PARAM_KERNELVERSION, VersionString, "", true);

    /* init answer */
    cn90AnswerOK(replyData, replyLen, NULL);

    /* calculate all parameters */
    pParameter = pXParamData->pFirst;
    /* checking while no error */
    while ((pParameter != NULL) && (nFuncReturn == OK_CN00)) {
      nFuncReturn = cn23CalculateParameter(pXParamData, pParameter, replyData, replyLen);
      if (nFuncReturn == OK_CN00) {
        nFuncReturn = cn23GetCalcState(pParameter, replyData, nGroup);
      } /* end if */
      if (nFuncReturn == OK_CN00) {
        pParameter = pParameter->pNext;
      } /* end if */
    } /* end while */

    /* check constraints for all parameters */
    pParameter = pXParamData->pFirst;
    /* checking while no error */
    while ((pParameter != NULL) && (nFuncReturn == OK_CN00)) {
      nFuncReturn = cn23CheckConstraint(pXParamData, pParameter, replyData, replyLen);
      if (nFuncReturn == OK_CN00) {
        nFuncReturn = cn23GetCalcState(pParameter, replyData, nGroup);
      } /* end if */
      if (nFuncReturn == OK_CN00) {
        pParameter = pParameter->pNext;
      } /* end if */
    } /* end while */

  } /* end if */

  if (bInit) {
    if (nFuncReturn == OK_CN00) {
      nFuncReturn = cn20ParamCommitSession(szDbName, pXParamData, replyData, replyLen, replyLenMax); 
    } // end if
    if (pXParamData != NULL) {
      delete pXParamData;
      pXParamData = NULL;
    } /* end if */
  } // end if

  *replyLen = (int)strlen(replyData);

  return nFuncReturn;
} /* end cn20ParamCheckAll */

/* PUBLIC FUNCTION cn20ParamPut
 * ------------------------------------------------------------------
 * purpose: save the new value internal param session structure
 *          - no persitent write to param file
 */
tcn00_Error cn20ParamPut
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error            nFuncReturn = OK_CN00;
  DBMSrvPar_Parameters * pXParamData = (DBMSrvPar_Parameters *) vcontrol->pXParamSection;
  tcn002_XpValueName     szParameter;
  tcn002_XpValueString   szValue;
  tsp00_C80c             szComment;

  bool                   bInit = false;
  DBMSrvPar_Parameter::XPWriteMode oWriteMode  = DBMSrvPar_Parameter::XPWriteModeUnknown;

  /* is there an active param session */
  if (pXParamData == NULL) {
    nFuncReturn = cn29ParamStartSession( vcontrol->dbname, pXParamData, replyData, replyLen, replyLenMax);
    bInit = true;
  } /* end if */

  if (nFuncReturn == OK_CN00) {
    if (pXParamData == NULL) {
      nFuncReturn = ERR_NOXPSESSION_CN00;
    } /* end if */
  } /* end if */

  /* setting the new value */
  if (nFuncReturn == OK_CN00) {
    bool bOption = true;
    char szToken[RTECONF_MAXSTRINGLENGTH +1 ];
    int  nToken = 1;
    while (bOption) {
      cn90GetToken(command->args, szToken, nToken, RTECONF_MAXSTRINGLENGTH);
      if (strnicmp(szToken, PARAM_OPT_ONLINE, strlen(szToken)) == 0) {
        oWriteMode  = (DBMSrvPar_Parameter::XPWriteMode) (oWriteMode | DBMSrvPar_Parameter::XPWriteModeOnline);
        ++nToken;
      } else if (strnicmp(szToken, PARAM_OPT_PERMANENT, strlen(szToken)) == 0) {
        oWriteMode  = (DBMSrvPar_Parameter::XPWriteMode) (oWriteMode | DBMSrvPar_Parameter::XPWriteModePermanent);
        ++nToken;
      } else {
        bOption = false;
      } // end if
    } // end if
    if (!cn90GetToken(command->args, szParameter, nToken, szParameter.size()+1)) {
      nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_TOOFEWPARAM_CN00);
    } else {
      cn90StrUpperCopy(szParameter, szParameter, false);
      if (!cn90GetToken(command->args, szValue,     nToken+1, szValue.size()+1)) {
        nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_TOOFEWPARAM_CN00);
      } else {
        cn90StripQuotes(szValue);
        if (cn90GetToken(command->args, szComment, nToken+2, szComment.size()+1)) {
          cn90StripQuotes(szComment);
        } else {
          szComment.Init();
        } // end if
      } // end if
    } // end if
  } // end if

  if (nFuncReturn == OK_CN00) {
    nFuncReturn = cn29SoftPutIntern(pXParamData, szParameter, szValue, szComment, false, oWriteMode);
  } /* end if */

  if (nFuncReturn == ERR_XPNOTFOUND_CN00) {
    nFuncReturn = cn29AnswerParamNotFound(replyData, replyLen, szParameter, pXParamData->pFirst);
  } else {
    cn90AnswerIError(replyData, replyLen, nFuncReturn);
  } /* end if */

  if (bInit) {
    if (nFuncReturn == OK_CN00) {
      nFuncReturn = cn20ParamCheckAll( vcontrol->dbname, pXParamData, "", replyData, replyLen, replyLenMax);
      if (nFuncReturn == OK_CN00) {
        nFuncReturn = cn20ParamCommitSession(vcontrol->dbname, pXParamData, replyData, replyLen, replyLenMax);
      } // end if
    } // end if

    if (pXParamData != NULL) {
      delete pXParamData;
      pXParamData = NULL;
    } /* end if */
  } // end if

  if (nFuncReturn == OK_CN00) {
    cn90AnswerOK(replyData, replyLen, NULL);
  } // end if

  return nFuncReturn;
} /* end cn20ParamPut */

/* PUBLIC FUNCTION cn20ParamCheckAll
 * ------------------------------------------------------------------
 * purpose: check of all parameters
 */
tcn00_Error cn20ParamCheckAll
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  char               szGroup[XP_STRING_LENGTH_MXCN002 +1 ];

  cn90GetToken(command->args, szGroup, 1, XP_STRING_LENGTH_MXCN002 +1);

  return cn20ParamCheckAll(vcontrol->dbname,
                           STATIC_CAST(DBMSrvPar_Parameters *, vcontrol->pXParamSection),
                           szGroup,
                           replyData,
                           replyLen,
                           replyLenMax);
} // end cn20ParamCheckAll

/* PUBLIC FUNCTION cn20ParamCommitSession
 * ------------------------------------------------------------------
 * purpose: terminate param session _WITH_ writing changes to the param file
 *
 */
tcn00_Error cn20ParamCommitSession
      ( tsp00_DbNamec          szDbName,
        DBMSrvPar_Parameters * pXParamData,
        char                 * replyData,
        int                  * replyLen,
        int                    replyLenMax)
{
  tcn00_Error           nFuncReturn = OK_CN00;
  DBMSrvPar_Parameter * pParameter  = NULL;
  DBMSrvPar_Parameter * pDynHelp    = NULL;
  DBMSrvPar_Parameter * pTemp       = NULL;
  bool                  bDirty      = false;
  bool                  bOnlineDirty= false;
  tcn002_XpValueString  szTemp;

  Tools_DynamicUTF8String szOnlineStatement = "SET PARAMETER ";

  DBMSrvMsg_Error               oMsg;
  DBMSrvPar_RTEConfParameter    oConfigFile(szDbName.asCharp(), 
                                   0, 
                                   RTECONF_MAXNAMELENGTH,
                                   RTECONF_MAXSTRINGLENGTH);

  /* is there an active param session */
  if (nFuncReturn == OK_CN00) {
    if (pXParamData == NULL) {
      nFuncReturn = ERR_NOXPSESSION_CN00;
      cn90AnswerIError(replyData, replyLen, nFuncReturn);
    } /* end if */
  } /* end if */

  /* open param file */
  if (nFuncReturn == OK_CN00) {
    /* open param file */
    if (!oConfigFile.Read(oMsg)) {
		if (oMsg == DBMSrvMsg_Error::DBMSrv_FILEOPEN) {
			oMsg.ClearMessageList();
		} else {
			nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(oMsg);
		} // end if
    } // endif
  } /* end if */

  /* looping around the parameters */
  if (nFuncReturn == OK_CN00) {
    pParameter = pXParamData->pFirst;

    while ((pParameter != NULL) && (nFuncReturn == OK_CN00)) {
      /* could we write the parameter */
      nFuncReturn = cn23GetCalcState(pParameter, replyData, DBMSrvPar_Parameter::XPPropValueNo);
 
      /* ok write it, if there is a value and it is not an internal parameter */
      if ( ( nFuncReturn                                  == OK_CN00                            ) &&
           ( pParameter->xpProperties[XPPropClassDynamic] == DBMSrvPar_Parameter::XPPropValueNo ) &&
           ( pParameter->xpProperties[XPPropClassIntern]  == DBMSrvPar_Parameter::XPPropValueNo )    ) {

        cn90StrUpperCopy (szTemp, pParameter->szNewValue, 
                          (bool   ) (pParameter->xpProperties[XPPropClassCasesensitive] == DBMSrvPar_Parameter::XPPropValueYes));
        cn29StringToParamValue(&(pParameter->xpRecord), szTemp);
        SAPDB_Bool bUpdateable = oConfigFile.UpdateOnlinePossible((const SAPDB_UTF8 *) pParameter->szParameterName.asCharp());
        if ( !pParameter->bParamExist                      || 
             ((pParameter->oWriteMode & DBMSrvPar_Parameter::XPWriteModePermanent) && (strcmp(szTemp, pParameter->szOldValue   ) != 0)) ||
             ((pParameter->oWriteMode & DBMSrvPar_Parameter::XPWriteModeOnline   ) && (strcmp(szTemp, pParameter->szActiveValue) != 0)) ||
             (strlen(pParameter->szComment)          != 0) ||
             (bUpdateable != (pParameter->xpProperties[XPPropClassChange] == DBMSrvPar_Parameter::XPPropValueRunning) ) ) {

          if ((pParameter->oWriteMode & DBMSrvPar_Parameter::XPWriteModePermanent) == DBMSrvPar_Parameter::XPWriteModePermanent) {
            switch (pParameter->xpRecord.xpType) {
              case DBMSrvPar_RTEConfParameter::t_Integer:
                oConfigFile.SetValue ((const SAPDB_UTF8 *) pParameter->szParameterName.asCharp(), 
                                      pParameter->xpRecord.xpValue.xpValueInt, 
                                      (pParameter->xpProperties[XPPropClassChange] == DBMSrvPar_Parameter::XPPropValueRunning), 
                                      true,
                                      oMsg);
                break;
              case DBMSrvPar_RTEConfParameter::t_Real:        
                oConfigFile.SetValue ((const SAPDB_UTF8 *) pParameter->szParameterName.asCharp(), 
                                      pParameter->xpRecord.xpValue.xpValueReal, 
                                      (pParameter->xpProperties[XPPropClassChange] == DBMSrvPar_Parameter::XPPropValueRunning), 
                                      true,
                                      oMsg);
                break;
              case DBMSrvPar_RTEConfParameter::t_String:
                {
                  DBMSrvPar_RTEConfParameter::String pString = (DBMSrvPar_RTEConfParameter::String) pParameter->xpRecord.xpValue.xpValueString.asCharp();
                  oConfigFile.SetValue ((const SAPDB_UTF8 *) pParameter->szParameterName.asCharp(), 
                                        pString, 
                                        (pParameter->xpProperties[XPPropClassChange] == DBMSrvPar_Parameter::XPPropValueRunning), 
                                        true,
                                        oMsg);
                }
                break;
              case DBMSrvPar_RTEConfParameter::t_CryptInfo:  
                oConfigFile.SetValue ((const SAPDB_UTF8 *) pParameter->szParameterName.asCharp(), 
                                      pParameter->xpRecord.xpValue.xpValueCrypt, 
                                      (pParameter->xpProperties[XPPropClassChange] == DBMSrvPar_Parameter::XPPropValueRunning), 
                                      true,
                                      oMsg);
                break;
            } // end switch

            // write history
            if (!oMsg.IsEmpty()) {
              nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(oMsg);
            } else {
              cn29WriteParamHistory(szDbName, pParameter, PAHI_CHANGE_CN002);
            } /* end if */
            bDirty = true;
          } /* end if */

          if ((pParameter->oWriteMode & DBMSrvPar_Parameter::XPWriteModeOnline) == DBMSrvPar_Parameter::XPWriteModeOnline) {
            szOnlineStatement.Append("\"");
            szOnlineStatement.Append(pParameter->szParameterName.asCharp());
            szOnlineStatement.Append("\"");
            szOnlineStatement.Append("=");
            if ((pParameter->xpRecord.xpType == DBMSrvPar_RTEConfParameter::t_String) || (pParameter->xpRecord.xpType == DBMSrvPar_RTEConfParameter::t_CryptInfo)) {
              szOnlineStatement.Append("'");
            } // end if
            if (pParameter->xpRecord.xpType == DBMSrvPar_RTEConfParameter::t_CryptInfo) {
              tsp00_Name    szName;       /* char [18] */
              tsp00_CryptPw cryptPwTemp;  /* char [24] */
              szName.c2p(pParameter->szNewValue.asCharp());
              s02encrypt(szName, cryptPwTemp);
              Tools_HexBuffer oBuffer(&cryptPwTemp[0], sizeof(cryptPwTemp));
              szOnlineStatement.Append(oBuffer.GiveHexBuffer());
            } else {
              szOnlineStatement.Append(pParameter->szNewValue.asCharp());
            } // end if
            if ((pParameter->xpRecord.xpType == DBMSrvPar_RTEConfParameter::t_String) || (pParameter->xpRecord.xpType == DBMSrvPar_RTEConfParameter::t_CryptInfo)) {
              szOnlineStatement.Append("'");
            } // end if
            szOnlineStatement.Append(",");
            bOnlineDirty = true;
          } // end if
        } /* end if */
      } /* end if */
 
      pParameter = pParameter->pNext;
    } /* end while */
  } /* end if */

  /* write paramfile */
  if ((nFuncReturn == OK_CN00) && bDirty) {
    if (!oConfigFile.Write(oMsg)) {
      nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(oMsg);
    } // endif
  } /* end if */

  // change online parameters 
  if ((nFuncReturn == OK_CN00) && bOnlineDirty && (DBMSrvKnl_State::State(szDbName) != DBMSrvKnl_State::StateOffline)) {
    szOnlineStatement.TrimTrailing(",");

    // build execute kernel command
    cn50DBMUser     oUser(szDbName, cn50DBMUser::getColdUser(szDbName));
    DBMSrvMsg_Error oError;
    if (!DBMSrvKnl_Session::Execute (szDbName.asCharp(),
                                     oUser.getUserName().asCharp(),
                                     oUser.getClearMasterPwd().asCharp(),
                                     szOnlineStatement.CharPtr(),
                                     oError)) {
        nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oError);
    } // end if
  } // end if

  if (nFuncReturn == OK_CN00) {
    cn90AnswerOK(replyData, replyLen, NULL);
  } /* end if */

  *replyLen = (int)strlen(replyData);

  return nFuncReturn;
} /* end cn20ParamCommitSession */

/* PUBLIC FUNCTION cn20ParamCommitSession
 * ------------------------------------------------------------------
 * purpose: terminate param session _WITH_ writing changes to the param file
 *
 */
tcn00_Error cn20ParamCommitSession
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error         nFuncReturn = OK_CN00;
  char                szSwitch[XP_STRING_LENGTH_MXCN002 +1 ];
  bool                bClose = true;

  // look for noclose option
  if (cn90GetToken(command->args, szSwitch, 1, XP_STRING_LENGTH_MXCN002 +1)) {
    if (_tcsicmp(PARAM_OPT_NOCLOSE, szSwitch) == 0) {
      bClose = false;
    } else {
      nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00);
    } // end if
  } // end if

  if (nFuncReturn == OK_CN00) {
    nFuncReturn = cn20ParamCommitSession(vcontrol->dbname,
                                         STATIC_CAST(DBMSrvPar_Parameters *, vcontrol->pXParamSection),
                                         replyData,
                                         replyLen,
                                         replyLenMax);
  } // end if

  if (nFuncReturn == OK_CN00 && bClose) {
    /* free XParam section */
    cn20FreeXParamSection (vcontrol);
  } // end if

  return nFuncReturn;
} // end cn20ParamCommitSession

/* PUBLIC FUNCTION cn20ParamAbortSession
 * ------------------------------------------------------------------
 * purpose: terminate param session _WITHOUT_ writing changes to the param file
 *
 */
tcn00_Error cn20ParamAbortSession
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  /* free the XParam section */
  cn20FreeXParamSection (vcontrol);

  /* Answer always ok */
  cn90AnswerOK(replyData, replyLen, NULL);

  return OK_CN00;
} /* end cn20ParamAbortSession */

/* PUBLIC FUNCTION cn20ParamInit
 * ------------------------------------------------------------------
 * purpose: initializes the param file for a new database
 *
 */
tcn00_Error cn20ParamInit
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  FUNCTION_DBG_MCN00_1("cn20ParamInit");

  tcn00_Error         nFuncReturn = OK_CN00;
  DBMSrvPar_Parameters * pXParamData = (DBMSrvPar_Parameters *) vcontrol->pXParamSection;
  DBMSrvPar_Parameter * pParameter  = NULL;
  tcn002_XpValueString szValue;
  tsp00_Versionc      VersionString;
  tsp00_C256          szWrk;
  tsp00_CryptPw         cryptPwd;
  tsp00_Namec           szPwd;
  tcn002_XpValueString szInstance;

  cn90GetToken(command->args, szInstance, 1, szInstance.size()+1);

  /* is there an active param session */
  if (nFuncReturn == OK_CN00) {
    if (pXParamData == NULL) {
      nFuncReturn = ERR_NOXPSESSION_CN00;
    } /* end if */
  } /* end if */

  /* is there an existing param file */
  if (nFuncReturn == OK_CN00) {
    if (pXParamData->bParamFileExist) {
      nFuncReturn = ERR_PARAMEXIST_CN00;
    } /* end if */
  } /* end if */

  // remove parameters from other instance types
  if ((nFuncReturn == OK_CN00) && (_tcslen(szInstance) > 0)) {
      nFuncReturn = cn29SoftPutIntern( pXParamData, PAN_INSTANCE_TYPE, szInstance, "param_init", true);
    if (nFuncReturn == OK_CN00) {
      nFuncReturn = cn29CheckInstance(szInstance, pXParamData);
    } // end if
  } // end if

  /* assign defaults */
  if (nFuncReturn == OK_CN00) {
    /* RUNDIRECTORY    */
    if (nFuncReturn == OK_CN00) {
      cn90GetWrkPath(szWrk);
      sprintf(szValue, "%s%s", (char *) szWrk, (char *) vcontrol->dbname);
      nFuncReturn = cn29SoftPutIntern( pXParamData, PAN_RUNDIR, szValue, "param_init", true);
    } /* end if */
    /* KERNELVERSION   */
    if (nFuncReturn == OK_CN00) {
      pParameter = pXParamData->Get(PAN_PARAM_KERNELVERSION);
      if (strlen(pParameter->szDefValue) > 0) {
        VersionString.rawAssign(pParameter->szDefValue);
      } else {
        sp100_GetVersionString(COMP_NAME_KERNEL_SP100, s100buildnumber, VersionString);
      } // end if
      nFuncReturn = cn29SoftPutIntern( pXParamData, PAN_PARAM_KERNELVERSION, VersionString, "param_init", true);
    } /* end if */

    /* read first dbm operator */
    cn50DBMUser oDBMUser   (vcontrol->dbname, cn50DBMUser::getColdUser(vcontrol->dbname));
    if (!oDBMUser.lastEvent().IsEmpty()) {
        Msg_List aEvent = oDBMUser.lastEvent();
        aEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_USRREAD));
        nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, aEvent);
        return nFuncReturn;
    } // end if

    /* CONTROLUSERID */
    if (nFuncReturn == OK_CN00) {
      nFuncReturn = cn29SoftPutIntern( pXParamData, PAN_CONTROLID, oDBMUser.getUserName(),  "param_init",true);
    } /* end if */
    /* CONTROLUSERPASSWORD */
    if (nFuncReturn == OK_CN00) {
      cryptPwd = oDBMUser.getMasterPwd();
      cn90CalculateUncrypt(cryptPwd, szPwd, false);
      nFuncReturn = cn29SoftPutIntern( pXParamData,  PAN_CONTROLPW, szPwd, "param_init", true);
    } /* end if */

  } /* end if */

  /* generate answer */
  if (nFuncReturn == OK_CN00) {
    cn90AnswerOK(replyData, replyLen, NULL);
  } else {
    cn90AnswerIError(replyData, replyLen, nFuncReturn);
  } /* end if */

  return nFuncReturn;
} /* end cn20ParamInit */

/* PUBLIC FUNCTION cn20ParamCopy
 * ------------------------------------------------------------------
 * purpose: copy parameters from a source-db
 *
 */
tcn00_Error cn20ParamCopy
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error         nFuncReturn = OK_CN00;
  DBMSrvPar_Parameters * pXParamData = (DBMSrvPar_Parameters *) vcontrol->pXParamSection;
  tsp00_DbNamec         szDatabase;
  DBMSrvPar_Parameter * pParameter  = NULL;
  DBMSrvPar_Parameter * pDynHelp    = NULL;
  tcn002_XpValueString  szValue;
  tsp00_Versionc      VersionString;
  tsp00_C256          szWrk;
  tsp00_CryptPw         cryptPwd;
  tsp00_Namec           szPwd;

  /* is there an active param session */
  if (nFuncReturn == OK_CN00) {
    if (pXParamData == NULL) {
      nFuncReturn = ERR_NOXPSESSION_CN00;
      cn90AnswerIError(replyData, replyLen, nFuncReturn);
    } /* end if */
  } /* end if */

  /* is there an existing param file */
  if (nFuncReturn == OK_CN00) {
    if (pXParamData->bParamFileExist) {
      nFuncReturn = ERR_PARAMEXIST_CN00;
      cn90AnswerIError(replyData, replyLen, nFuncReturn);
    } /* end if */
  } /* end if */

  /* get source name */
  if (nFuncReturn == OK_CN00) {
    if (!cn90GetToken(command->args, szDatabase, 1, szDatabase.size()+1)) {
      nFuncReturn = ERR_PARAM_CN00;
      cn90AnswerIError(replyData, replyLen, nFuncReturn);
    } /* end if */
  } /* end if */

  /* open Param file of db */
  DBMSrvMsg_Error               oMsg;
  DBMSrvPar_RTEConfParameter    oConfigFile(szDatabase, 
                                   0, 
                                   RTECONF_MAXNAMELENGTH,
                                   RTECONF_MAXSTRINGLENGTH);

  if ((nFuncReturn == OK_CN00) && (vcontrol->dbname[0] != CHAR_STRINGTERM_CN90)) {
    if(!oConfigFile.Read(oMsg)) {
      nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(oMsg);
    } /* end if */
  } /* end if */

  /* File abarbeiten */
  if (nFuncReturn == OK_CN00) {
    RTEConf_ParameterIterator   oIterator = oConfigFile.GetIterator();
    DBMSrvPar_RTEConfParameter::Name     sName;
    DBMSrvPar_RTEConfParameter::Type     oType;
    SAPDB_Bool                  bEnd       = false;
    SAPDB_Bool                  bOK        = true;
 
    do {
      bOK = oIterator.Next(sName, oType, bEnd, oMsg);
      if (bOK && !bEnd) {

        /* search parameter in List */
        pParameter = pXParamData->Get((char *) sName);

        /* No match -> look for dynamic record */
        if (pParameter == NULL) {
          /* is there a dynamic match */
          pDynHelp = cn29SearchDynamic(pXParamData->pFirst, (char *) sName);
          if (pDynHelp != NULL) {
            if (pDynHelp->xpProperties[XPPropClassClear] == DBMSrvPar_Parameter::XPPropValueNo) {

              /* only none clear-on-copy parameters */
              pParameter = new DBMSrvPar_Parameter;
              if (pParameter == NULL) {
                nFuncReturn = ERR_MEM_CN00;
                cn90AnswerIError(replyData, replyLen, nFuncReturn);
              } else {
                /* copy data from dynamic record */
                nFuncReturn = cn29CopyParameter(pParameter, pDynHelp);
                /* clear the dynamic/intern property */
                pParameter->xpProperties[XPPropClassDynamic] = DBMSrvPar_Parameter::XPPropValueNo;
                pParameter->xpProperties[XPPropClassIntern] = DBMSrvPar_Parameter::XPPropValueNo;

                /* container management */
                pParameter->pNext = pDynHelp->pNext;
                pDynHelp->pNext   = pParameter;
                SAPDB_strcpy(pParameter->szParameterName, (char *) sName);
                pXParamData->Put(pParameter);

              } /* end if */
            } /* end if */
          } /* end if */
        } else {
          /* exclude clear-on-copy parameters */
          pParameter = (pParameter->xpProperties[XPPropClassClear] == DBMSrvPar_Parameter::XPPropValueYes) ? NULL : pParameter;
        } /* end if */

        /* Copy Data */
        if (pParameter != NULL) {
          SAPDB_strcpy(pParameter->szParameterName, (char *) sName);

          switch (oType) {
            case DBMSrvPar_RTEConfParameter::t_Integer:
              oConfigFile.GetValue (sName, pParameter->xpRecord.xpValue.xpValueInt, oMsg);
              break;
            case DBMSrvPar_RTEConfParameter::t_Real:        
              oConfigFile.GetValue (sName, pParameter->xpRecord.xpValue.xpValueReal, oMsg);
              break;
            case DBMSrvPar_RTEConfParameter::t_String:
              oConfigFile.GetValue (sName, (DBMSrvPar_RTEConfParameter::String) pParameter->xpRecord.xpValue.xpValueString.asCharp(), oMsg);
              break;
            case DBMSrvPar_RTEConfParameter::t_CryptInfo:  
              oConfigFile.GetValue (sName, pParameter->xpRecord.xpValue.xpValueCrypt, oMsg);
              break;
          } // end switch

          cn29ParamValueToString(pParameter->szNewValue, &pParameter->xpRecord);
        } /* end if */

      } /* end if */

    }  while ( (bOK) && (!bEnd) && (nFuncReturn == OK_CN00));
  } /* end if */

  /* assign defaults */
  if (nFuncReturn == OK_CN00) {
    /* RUNDIRECTORY    */
    if (nFuncReturn == OK_CN00) {
      cn90GetWrkPath(szWrk);
      sprintf(szValue, "%s%s", (char *) szWrk, (char *) vcontrol->dbname);
      nFuncReturn = cn29SoftPutIntern( pXParamData, PAN_RUNDIR, szValue, "param_copy", true);
    } /* end if */
    /* KERNELVERSION   */
    if (nFuncReturn == OK_CN00) {
      sp100_GetVersionString(COMP_NAME_KERNEL_SP100, s100buildnumber, VersionString);
      nFuncReturn = cn29SoftPutIntern( pXParamData, PAN_PARAM_KERNELVERSION, VersionString, "param_copy", true);
    } /* end if */
    /* CONTROLUSERID */
    if (nFuncReturn == OK_CN00) {
      nFuncReturn = cn29SoftPutIntern( pXParamData, PAN_CONTROLID, vcontrol->pCurrentUser->getUserName(), "param_copy", true);
    } /* end if */
    /* CONTROLUSERPASSWORD */
    if (nFuncReturn == OK_CN00) {
      cryptPwd = vcontrol->pCurrentUser->getMasterPwd();
      cn90CalculateUncrypt(cryptPwd, szPwd, false);
      nFuncReturn = cn29SoftPutIntern( pXParamData, PAN_CONTROLPW, szPwd, "param_copy", true);
    } /* end if */
    if (nFuncReturn == OK_CN00) {
      cn90AnswerIError(replyData, replyLen, nFuncReturn);
    } /* end if */
  } /* end if */

  /* generate answer */
  if (nFuncReturn == OK_CN00) {
    vcontrol->pCurrentUser->setColdUser();
    vcontrol->pCurrentUser->save();
    cn90AnswerOK(replyData, replyLen, NULL);
  } /* end if */

  return nFuncReturn;
} /* end cn20ParamCopy */

/* PUBLIC FUNCTION cn20ParamAddDevSpace
 * ------------------------------------------------------------------
 * purpose: add devspace params
 *
 */
tcn00_Error cn20ParamAddDevSpace
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  long nReturnSize = 0;
  return cn20ParamAddDevSpace(vcontrol, command, replyData, replyLen, replyLenMax, nReturnSize);
} // end cn20ParamAddDevSpace

/* PUBLIC FUNCTION cn20ParamAddDevSpace
 * ------------------------------------------------------------------
 * purpose: add devspace params
 *
 */
tcn00_Error cn20ParamAddDevSpace
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax,
        long          & nReturnSize)
{
  FUNCTION_DBG_MCN00_1("cn20ParamAddDevSpace");

  tcn00_Error            nFuncReturn    = OK_CN00;
  char                   szNumber   [XP_STRING_LENGTH_MXCN002 + 1];
  char                   szMode     [XP_STRING_LENGTH_MXCN002 + 1];
  tcn002_XpValueString   szType;
  tcn002_XpValueString   szName;
  tcn002_XpValueString   szSize;
  tcn002_XpValueString   szVolumeMode;
  tcn002_XpValueName     szParameter;
  char                   szRundir   [PARAMETER_MAXLEN_CN90];
  tcn00_DevspaceKeywords aKeyTable  [] = XP_DEVSPACE_KEYWORDS_CN00;
  tcn00_DevspaceParams   aParamTable[] = XP_DEVSPACE_PARAMS_CN00;
  long                   nKey          = 0;   
  long                   nParam        = 0;   
  int                    nNumber       = 0;
  tsp00_Int4               nCompSize     = 0;
  long                   nSize         = 0;
  DBMSrvKnl_State::StateValue aState;
  tcn002_XpValueString   szChanged;
  tcn002_XpValueName     szChangeName;
  bool                   bDirty        = false;
  tsp00_Bool             bOk;
  tsp00_ErrTextc         szError;
  DBMSrvPar_Parameters    * pXParamData = NULL;
  DBMSrvPar_Parameter    * pParameter  = NULL;

  nReturnSize = 0;

  /* is there an active param session */
  if (nFuncReturn == OK_CN00) {
    if (vcontrol->pXParamSection != NULL) {
      nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_XPSESSION_CN00);
    } /* end if */
  } /* end if */

  // start a temporary param session
  if (nFuncReturn == OK_CN00) {
    nFuncReturn = cn29ParamStartSession( vcontrol->dbname, pXParamData, replyData, replyLen, replyLenMax);
  } // end if

  // check dirty flag
  if (nFuncReturn == OK_CN00) {
    aState = DBMSrvKnl_State::State(vcontrol->dbname);
    if (aState == DBMSrvKnl_State::StateError) {
      nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_STATE_CN00);
    } else if ((aState != DBMSrvKnl_State::StateOffline) && (aState != DBMSrvKnl_State::StateAdmin) && (aState != DBMSrvKnl_State::StateStandby)) {
      szChangeName.rawAssign(XP_PARAM_CHANGED);
      if (cn20XParamGetValue( vcontrol->dbname, szChangeName, szChanged) == OK_CN00) {
        if (atol(szChanged) != 0) {
          bDirty = true;
        } // end if
      } // end if
    } // end if
  } // end if

  /* get parameters and check mode */
  if (nFuncReturn == OK_CN00) {
    cn90GetToken(command->args, szNumber, 1, XP_STRING_LENGTH_MXCN002 + 1);
    cn90GetToken(command->args, szMode, 2, XP_STRING_LENGTH_MXCN002 + 1);
    cn90GetToken(command->args, szName, 3, szName.size());
    cn90StripQuotes(szName);
    cn90GetToken(command->args, szType, 4, szType.size());
    cn90GetToken(command->args, szSize, 5, szSize.size());

    /* search in keywordtable */
    for (nKey = 0; (aKeyTable[nKey].nDevSpace != XPDevSpaceUnknown_ecn00) &&
                   (_tcsicmp(aKeyTable[nKey].szText, szMode) != 0  );    nKey++);

    /* search in paramtable */
    for (nParam = 0; aKeyTable[nKey].nDevSpace != aParamTable[nParam].nDevSpace; nParam++);

    if (aKeyTable[nKey].nDevSpace == XPDevSpaceUnknown_ecn00) {
      nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00);
//PTS 1133030    } else if (bDirty && !aKeyTable[nKey].bMirror) {
//PTS 1133030      nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_NOADDD_CN00);
    } /* end if */
  } /* end if */

  /* check arguments */
  if (nFuncReturn == OK_CN00) {
    if ((strlen(szNumber) == 0) ||
        (strlen(szType)   != 1) ||
        (strlen(szName)   == 0) ||
        ((strlen(szSize)  == 0) && (strlen(aParamTable[nParam].szSize) > 0)) ) {
      nFuncReturn = ERR_PARAM_CN00;
      cn90AnswerIError(replyData, replyLen, nFuncReturn);
    } /* end if */
  } /* end if */

  if (nFuncReturn == OK_CN00) {
    /* set number of devspace */
    nNumber = atoi(szNumber);

    /* set default Data volume mode */
    szVolumeMode.rawAssign("NORMAL");

    /* get the rundir */
    nFuncReturn = cn20XParamGetValue(vcontrol, PAN_RUNDIR, szRundir, replyData);
  } /* end if */

  /* raw check the devspace */
  if (nFuncReturn == OK_CN00) {
    if ((szType[0] == 'R' || szType[0] == 'r') && (strlen(aParamTable[nParam].szSize) > 0)) {
      SqlDevSize( szName, &nCompSize, szError, &bOk );
      if (!bOk) {
          nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(DBMSrvMsg_RTEError(bOk, szError));
      } else if (nCompSize > 0) {
        nSize = atol(szSize);
        nSize = (nSize == 0) ? nCompSize : nSize;
        sprintf(szSize, "%ld", nSize);
      } /* end if */
    } else if (szType[0] == 'F' || szType[0] == 'f') {
      // check existence of path
      tsp00_Pathc szPath;
      int        i;
      szPath.rawAssign(szName.asCharp());
      for (i = szPath.length(); i > 0; --i)  {
        if (szPath[i - 1] == PATH_DELIMITER_CN90) {
          szPath[i] = 0;
          break;
        } // end if
      } // end for
#ifdef _WIN32
      if ((i > 1) && 
          (szPath[i - 1] == PATH_DELIMITER_CN90) &&
          (szPath[i - 2] != ':')) {
          szPath[i - 1] = 0;
      } // end if
#endif
      // exclude relative paths because rundirectory does not exist at this time
      if ((i > 0) && szPath[0] != '.') {
        struct stat oState;
        int nState = stat(szPath, &oState);
        if (nState < 0 || (oState.st_mode & S_IFDIR) == 0) {
#ifdef _WIN32
          if (!((i         >  1                  ) && 
                (szPath[0] == PATH_DELIMITER_CN90) &&
                (szPath[1] == PATH_DELIMITER_CN90)    )) {

            DBMSrvMsg_Error aEvent(SDBMSG_DBMSRV_NOVOLDIR, Msg_Arg(SDBMSGTAG_DBMSRV_NOVOLDIR__DBMSRV_COMP,szPath.asCharp()));
            nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, aEvent);
          } // end if
#else
          DBMSrvMsg_Error aEvent(SDBMSG_DBMSRV_NOVOLDIR, Msg_Arg(SDBMSGTAG_DBMSRV_NOVOLDIR__DBMSRV_COMP,szPath.asCharp()));
          nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, aEvent);
#endif
        } // end if
      } // end if

    } else if (szType[0] == 'L' || szType[0] == 'l') {
      // check existence of link
#ifdef _WIN32
      DBMSrvMsg_Error aEvent(SDBMSG_DBMSRV_NOVOLLINK, Msg_Arg(SDBMSGTAG_DBMSRV_NOVOLLINK__DBMSRV_COMP,szName.asCharp()));
      nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, aEvent);
#else 
      tsp00_Pathc szPath;
      
      if (szName[0] != PATH_DELIMITER_CN90) {
        tcn002_XpValueName   XpKeyC;
        tcn002_XpValueString XpValueC;

        XpValueC.Init();
        XpKeyC.rawAssign(PAN_RUNDIR);
        cn20XParamGetValue (cn00DBMServerData::vcontrol()->dbname, XpKeyC, XpValueC);

        szPath.rawAssign(XpValueC.asCharp());
        strcat(szPath, "/");
        strcat(szPath, szName.asCharp());

      } else {
        szPath.rawAssign(szName.asCharp());
      } // end if

      struct stat oState;
      int nState = lstat(szPath, &oState);
      if (nState < 0 || !S_ISLNK(oState.st_mode) ) {
        DBMSrvMsg_Error aEvent(SDBMSG_DBMSRV_NOVOLLINK, Msg_Arg(SDBMSGTAG_DBMSRV_NOVOLLINK__DBMSRV_COMP,szPath.asCharp()));
        nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, aEvent);
      } // end if
#endif
    } else if (szType[0] == 'A' || szType[0] == 'a') {
      /* set default Data volume mode */
      szVolumeMode.rawAssign("SEQUENTIAL");
    } /* end if */
  } /* end if */

  /* set name of devspace */
  if (nFuncReturn == OK_CN00) {
    sprintf(szParameter, aParamTable[nParam].szName, nNumber);
    nFuncReturn = cn29SoftPutIntern( pXParamData, szParameter, szName, "addvolume", true);
    if (nFuncReturn != OK_CN00) {
      cn90AnswerIError(replyData, replyLen, nFuncReturn);
    } else {
      pParameter = pXParamData->Get(szParameter);

      nFuncReturn = cn23CalculateParameter(pXParamData, pParameter, replyData, replyLen);
      if (nFuncReturn == OK_CN00) {
        nFuncReturn = cn23GetCalcState(pParameter, replyData, DBMSrvPar_Parameter::XPPropValueGeneral);
      } /* end if */
      if (nFuncReturn == OK_CN00) {
        nFuncReturn = cn23CheckConstraint(pXParamData, pParameter, replyData, replyLen);
      } /* end if */
      if (nFuncReturn == OK_CN00) {
        nFuncReturn = cn23GetCalcState(pParameter, replyData, DBMSrvPar_Parameter::XPPropValueGeneral);
      } /* end if */

    } // end if
  } /* end if */
  /* set type of devspace */
  if (nFuncReturn == OK_CN00) {
    sprintf(szParameter, aParamTable[nParam].szType, nNumber);
    nFuncReturn = cn29SoftPutIntern( pXParamData, szParameter, szType, "addvolume", true);
    if (nFuncReturn != OK_CN00) {
      cn90AnswerIError(replyData, replyLen, nFuncReturn);
    } else {
      pParameter = pXParamData->Get(szParameter);

      nFuncReturn = cn23CalculateParameter(pXParamData, pParameter, replyData, replyLen);
      if (nFuncReturn == OK_CN00) {
        nFuncReturn = cn23GetCalcState(pParameter, replyData, DBMSrvPar_Parameter::XPPropValueGeneral);
      } /* end if */
      if (nFuncReturn == OK_CN00) {
        nFuncReturn = cn23CheckConstraint(pXParamData, pParameter, replyData, replyLen);
      } /* end if */
      if (nFuncReturn == OK_CN00) {
        nFuncReturn = cn23GetCalcState(pParameter, replyData, DBMSrvPar_Parameter::XPPropValueGeneral);
      } /* end if */

    } // end if
  } /* end if */
  /* set size of devspace */
  if ((nFuncReturn == OK_CN00) && (strlen(aParamTable[nParam].szSize) > 0)) {
    sprintf(szParameter, aParamTable[nParam].szSize, nNumber);
    nFuncReturn = cn29SoftPutIntern( pXParamData, szParameter, szSize, "addvolume", true);
    if (nFuncReturn != OK_CN00) {
      cn90AnswerIError(replyData, replyLen, nFuncReturn);
    } else {
      pParameter = pXParamData->Get(szParameter);

      nFuncReturn = cn23CalculateParameter(pXParamData, pParameter, replyData, replyLen);
      if (nFuncReturn == OK_CN00) {
        nFuncReturn = cn23GetCalcState(pParameter, replyData, DBMSrvPar_Parameter::XPPropValueGeneral);
      } /* end if */
      if (nFuncReturn == OK_CN00) {
        nFuncReturn = cn23CheckConstraint(pXParamData, pParameter, replyData, replyLen);
      } /* end if */
      if (nFuncReturn == OK_CN00) {
        nFuncReturn = cn23GetCalcState(pParameter, replyData, DBMSrvPar_Parameter::XPPropValueGeneral);
      } /* end if */

    } // end if
  } /* end if */
  /* set mode of devspace */
  if ((nFuncReturn == OK_CN00) && (strlen(aParamTable[nParam].szMode) > 0)) {
    sprintf(szParameter, aParamTable[nParam].szMode, nNumber);
    nFuncReturn = cn29SoftPutIntern( pXParamData, szParameter, szVolumeMode, "addvolume", true);
    if (nFuncReturn != OK_CN00) {
      cn90AnswerIError(replyData, replyLen, nFuncReturn);
    } else {
      pParameter = pXParamData->Get(szParameter);

      nFuncReturn = cn23CalculateParameter(pXParamData, pParameter, replyData, replyLen);
      if (nFuncReturn == OK_CN00) {
        nFuncReturn = cn23GetCalcState(pParameter, replyData, DBMSrvPar_Parameter::XPPropValueGeneral);
      } /* end if */
      if (nFuncReturn == OK_CN00) {
        nFuncReturn = cn23CheckConstraint(pXParamData, pParameter, replyData, replyLen);
      } /* end if */
      if (nFuncReturn == OK_CN00) {
        nFuncReturn = cn23GetCalcState(pParameter, replyData, DBMSrvPar_Parameter::XPPropValueGeneral);
      } /* end if */

    } // end if
  } /* end if */

  if (nFuncReturn == OK_CN00) {
    DBMSrvMsg_Error oError;
    if (!pXParamData->CheckAll(oError)) {
      nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oError);
    } else {
      if (!pXParamData->Commit(oError)) {
        nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oError);
      } // end if
    } // end if
  } // end if

  // free session
  if (pXParamData != NULL) {
    delete pXParamData;
    pXParamData = NULL;
  } // end if

  if (nFuncReturn == OK_CN00) {
    if (aState == DBMSrvKnl_State::StateOffline) {
      command->setArgs("");
      nFuncReturn = cn20CalcDevices(vcontrol, replyData, replyLen, replyLenMax);
    } // end if
  } // end if

  /* generate answer */
  if (nFuncReturn == OK_CN00) {
    nReturnSize = nSize;
    cn90AnswerOK(replyData, replyLen, NULL);
  } /* end if */

  *replyLen = (int)strlen(replyData);

  return nFuncReturn;
} /* end cn20ParamAddDevSpace */

/* PUBLIC FUNCTION cn20ParamDelDevSpace
 * ------------------------------------------------------------------
 * purpose: del devspace params
 *
 */
tcn00_Error cn20ParamDelDevSpace
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error            nFuncReturn    = OK_CN00;
  char                   szNumber   [XP_STRING_LENGTH_MXCN002 + 1];
  char                   szMode     [XP_STRING_LENGTH_MXCN002 + 1];
  char                   szParameter[XP_STRING_LENGTH_MXCN002 + 1];
  tcn00_DevspaceKeywords aKeyTable  [] = XP_DEVSPACE_KEYWORDS_CN00;
  tcn00_DevspaceParams   aParamTable[] = XP_DEVSPACE_PARAMS_CN00;
  long                   nKey          = 0;   
  long                   nParam        = 0;   
  int                    nNumber       = 0;
  DBMSrvKnl_State::StateValue aState;
  tcn002_XpValueString   szChanged;
  tcn002_XpValueName     szChangeName;
  bool                   bDirty        = false;

  /* is there an active param session */
  if (nFuncReturn == OK_CN00) {
    if (vcontrol->pXParamSection != NULL) {
      nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_XPSESSION_CN00);
    } /* end if */
  } /* end if */

  // check dirty flag
  if (nFuncReturn == OK_CN00) {
    aState = DBMSrvKnl_State::State(vcontrol->dbname);
    if (aState == DBMSrvKnl_State::StateError) {
      nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_STATE_CN00);
    } else if (aState != DBMSrvKnl_State::StateOffline) {
      szChangeName.rawAssign(XP_PARAM_CHANGED);
      if (cn20XParamGetValue( vcontrol->dbname, szChangeName, szChanged) == OK_CN00) {
        if (atol(szChanged) != 0) {
          bDirty = true;
        } // end if
      } // end if
    } // end if
  } // end if

  /* get parameters and check mode */
  if (nFuncReturn == OK_CN00) {
    cn90GetToken(command->args, szNumber, 1, XP_STRING_LENGTH_MXCN002 + 1);
    cn90GetToken(command->args, szMode, 2, XP_STRING_LENGTH_MXCN002 + 1);

    /* search in keywordtable */
    for (nKey = 0; (aKeyTable[nKey].nDevSpace != XPDevSpaceUnknown_ecn00) &&
                   (_tcsicmp(aKeyTable[nKey].szText, szMode) != 0  );    nKey++);

    /* search in paramtable */
    for (nParam = 0; aKeyTable[nKey].nDevSpace != aParamTable[nParam].nDevSpace; nParam++);

    if (aKeyTable[nKey].nDevSpace == XPDevSpaceUnknown_ecn00) {
      nFuncReturn = ERR_PARAM_CN00;
      cn90AnswerIError(replyData, replyLen, nFuncReturn);
//PTS 1133030    } else if (bDirty && !aKeyTable[nKey].bMirror) {
//PTS 1133030      nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_XPDIRTY_CN00);
    } else {
      /* set number of devspace */
      nNumber = atoi(szNumber);
    } /* end if */
  } /* end if */

  /* unset name of devspace */
  if (nFuncReturn == OK_CN00) {
    sprintf(szParameter, aParamTable[nParam].szName, nNumber);
    nFuncReturn = cn20_ParamDirectDel( vcontrol, szParameter, "deletevolume", replyData, replyLen, replyLenMax, true); //a volume must have a name
  } /* end if */
  /* unset type of devspace */
  if (nFuncReturn == OK_CN00) {
    sprintf(szParameter, aParamTable[nParam].szType, nNumber);
    nFuncReturn = cn20_ParamDirectDel( vcontrol, szParameter, "deletevolume", replyData, replyLen, replyLenMax, false); //a volumes type is optional
  } /* end if */
  /* unset size of devspace */
  if ((nFuncReturn == OK_CN00) && (strlen(aParamTable[nParam].szSize) > 0)) {
    sprintf(szParameter, aParamTable[nParam].szSize, nNumber);
    nFuncReturn = cn20_ParamDirectDel( vcontrol, szParameter, "deletevolume", replyData, replyLen, replyLenMax, true); //if a volume of this type has a size, the size is mandatory
  } /* end if */
    /* unset mode of devspace */
    if(OK_CN00==nFuncReturn && 0<strlen(aParamTable[nParam].szMode))
    {
        sprintf(szParameter, aParamTable[nParam].szMode, nNumber);
        nFuncReturn=cn20_ParamDirectDel(vcontrol, szParameter, "deletevolume", replyData, replyLen, replyLenMax, false); //a volumes mode is optional
    }

  /* generate answer */
  if (nFuncReturn == OK_CN00) {
    cn90AnswerOK(replyData, replyLen, NULL);
  } /* end if */

  return nFuncReturn;
} // end cn20ParamDelDevSpace

/* PUBLIC FUNCTION cn20ParamGetDevSpace
 * ------------------------------------------------------------------
 * purpose: get devspace params
 *
 */
tcn00_Error cn20ParamGetDevSpace
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error            nFuncReturn   = OK_CN00;
  char                   szNumber   [XP_STRING_LENGTH_MXCN002 + 1];
  char                   szMode     [XP_STRING_LENGTH_MXCN002 + 1];
  tcn00_DevspaceKeywords aKeyTable  [] = XP_DEVSPACE_KEYWORDS_CN00;
  tcn00_DevspaceParams   aParamTable[] = XP_DEVSPACE_PARAMS_CN00;
  long                   nKey          = 0;   
  long                   nParam        = 0;   
  int                    nNumber       = 0;
  tcn002_XpValueString   szName;
  tcn002_XpValueString   szType;
  tcn002_XpValueString   szSize;
  tcn002_XpValueName     szParameter;

  /* get parameters and check mode */
  if (nFuncReturn == OK_CN00) {
    cn90GetToken(command->args, szNumber, 1, XP_STRING_LENGTH_MXCN002 + 1);
    cn90GetToken(command->args, szMode, 2, XP_STRING_LENGTH_MXCN002 + 1);

    /* search in keywordtable */
    for (nKey = 0; (aKeyTable[nKey].nDevSpace != XPDevSpaceUnknown_ecn00) &&
                   (_tcsicmp(aKeyTable[nKey].szText, szMode) != 0  );    nKey++);

    /* search in paramtable */
    for (nParam = 0; aKeyTable[nKey].nDevSpace != aParamTable[nParam].nDevSpace; nParam++);

    if (aKeyTable[nKey].nDevSpace == XPDevSpaceUnknown_ecn00) {
      nFuncReturn = ERR_PARAM_CN00;
      cn90AnswerIError(replyData, replyLen, nFuncReturn);
    } else {
      /* set number of devspace */
      nNumber = atoi(szNumber);
    } /* end if */
  } /* end if */

  szName.Init();
  szType.Init();
  szSize.Init();
  /* get name of devspace */
  if (nFuncReturn == OK_CN00) {
    sprintf(szParameter, aParamTable[nParam].szName, nNumber);
    nFuncReturn = cn20XParamGetValue(vcontrol, szParameter, szName, replyData);
  } /* end if */
  /* get type of devspace */
  if (nFuncReturn == OK_CN00) {
    sprintf(szParameter, aParamTable[nParam].szType, nNumber);
    nFuncReturn = cn20XParamGetValue(vcontrol, szParameter, szType, replyData);
  } /* end if */
  /* get size of devspace */
  if ((nFuncReturn == OK_CN00) && (strlen(aParamTable[nParam].szSize) > 0)) {
    sprintf(szParameter, aParamTable[nParam].szSize, nNumber);
    nFuncReturn = cn20XParamGetValue(vcontrol, szParameter, szSize, replyData);
  } /* end if */

  /* generate answer */
  if (nFuncReturn == OK_CN00) {
    sprintf(replyData,"%s%s%s%s%s%s%s%s",
                      ANSWER_OK_CN00, 
                      LINE_SEPSTRING_CN00,
                      szName.asCharp(),
                      LINE_SEPSTRING_CN00,
                      szType.asCharp(),
                      LINE_SEPSTRING_CN00,
                      szSize.asCharp(),
                      LINE_SEPSTRING_CN00);

  } /* end if */

  *replyLen = (int)strlen(replyData);

  return nFuncReturn;
} // end cn20ParamGetDevSpace

/* PUBLIC FUNCTION cn20ParamGetDevSpaceAll
 * ------------------------------------------------------------------
 * purpose: get devspace params
 *
 */
tcn00_Error cn20ParamGetDevSpaceAll
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error            nFuncReturn   = OK_CN00;
  bool                   bInit         = false;
  DBMSrvPar_Parameters    * pXParamData   = STATIC_CAST( DBMSrvPar_Parameters *, vcontrol->pXParamSection);
  char                   szMode[XP_STRING_LENGTH_MXCN002 + 1] = "";
  DBMSrvPar_Parameter    * pParameter    = NULL;
  DBMSrvPar_Parameter    * pSizeParam    = NULL;
  DBMSrvPar_Parameter    * pTypeParam    = NULL;
  char                 * pCurrent      = replyData;
  tcn00_DevspaceParams   aParamTable[] = XP_DEVSPACE_PARAMS_CN00;
  int                    nIndex        = 0;
  int                    nMIndex       = 0;
  int                    nDevspace     = 0;
  tcn002_XpValueName     szParameter;
  tcn002_XpValueName     szDevspace;
  int                    maxDataVolumes=0;

  if (cn90GetToken(command->args, szMode, 1, XP_STRING_LENGTH_MXCN002 + 1)) {
    if (_tcsicmp(szMode, XP_DEV_DAT_CN00) != 0 && _tcsicmp(szMode, XP_DEV_LOG_CN00) != 0) {
      nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00);
    } // end if
  } // end if

  // init param management for commandline use or tcontrol or rcontrol
  if (nFuncReturn == OK_CN00) {
    if (pXParamData == NULL) {
      nFuncReturn = cn29ParamStartSession( vcontrol->dbname, pXParamData, replyData, replyLen, replyLenMax, true);
      bInit = true;
    } // end if
  } // end if

  if (nFuncReturn == OK_CN00) {
    // init answer
    sprintf(pCurrent, "%s%s", ANSWER_OK_CN00, LINE_SEPSTRING_CN00);
    pCurrent = pCurrent + _tcslen (pCurrent);

    // look for log params
    if (_tcslen(szMode) == 0 || _tcsicmp(szMode, XP_DEV_LOG_CN00) == 0) {
      pParameter = pXParamData->Get(PAN_LOG_MIRRORED);
      sprintf(pCurrent, "%-*s%s%s", szDevspace.size() + 1, PAN_LOG_MIRRORED, (pParameter != NULL) ? pParameter->szNewValue.asCharp() : "", LINE_SEPSTRING_CN00);
      pCurrent = pCurrent + _tcslen(pCurrent);

      pParameter = pXParamData->Get(PAN_MAXARCHIVELOGS);
      sprintf(pCurrent, "%-*s%s%s", szDevspace.size() + 1, PAN_MAXARCHIVELOGS, (pParameter != NULL) ? pParameter->szNewValue.asCharp() : "", LINE_SEPSTRING_CN00);
      pCurrent = pCurrent + _tcslen(pCurrent);
    } // end if

    // look for DATA params
    if (_tcslen(szMode) == 0 || _tcsicmp(szMode, XP_DEV_DAT_CN00) == 0) {
      pParameter = pXParamData->Get(PAN_MAXDATADEVSPACES);
      sprintf(pCurrent, "%-*s%s%s", szDevspace.size() + 1, PAN_MAXDATADEVSPACES, (pParameter != NULL) ? pParameter->szNewValue.asCharp() : "", LINE_SEPSTRING_CN00);
      pCurrent = pCurrent + _tcslen(pCurrent);

      maxDataVolumes=atoi((pParameter)?pParameter->szNewValue.asCharp():""); //-> maxDataVolumes is 0 in case of errors
    } // end if

    // retrieve log devices
    if (_tcslen(szMode) == 0 || _tcsicmp(szMode, XP_DEV_LOG_CN00) == 0) {
      nIndex = 0;
      for (nIndex = 0; aParamTable[nIndex].nDevSpace != XPDevSpaceLog_ecn00; ++nIndex);
      nMIndex = 0;
      for (nMIndex = 0; aParamTable[nMIndex].nDevSpace != XPDevSpaceMLog_ecn00; ++nMIndex);

      nDevspace = 1;
      sprintf(szDevspace, aParamTable[nIndex].szName, nDevspace);
      pParameter = pXParamData->Get(szDevspace);

      while (pParameter != NULL) {
        // name
        sprintf(pCurrent, "%-*s", szDevspace.size() + 1, szDevspace.asCharp());
        pCurrent = pCurrent + _tcslen(pCurrent);

        // size
        sprintf(szParameter, aParamTable[nIndex].szSize, nDevspace);
        pSizeParam = pXParamData->Get(szParameter);
        sprintf(pCurrent, "%-11s", (pSizeParam != NULL) ? pSizeParam->szNewValue.asCharp() : "");
        pCurrent = pCurrent + _tcslen(pCurrent);

        // type
        sprintf(szParameter, aParamTable[nIndex].szType, nDevspace);
        pTypeParam = pXParamData->Get(szParameter);
        sprintf(pCurrent, "%-2s", (pTypeParam != NULL) ? pTypeParam->szNewValue.asCharp() : "");
        pCurrent = pCurrent + _tcslen(pCurrent);

        // path
        sprintf(pCurrent, " %s%s", pParameter->szNewValue.asCharp(), LINE_SEPSTRING_CN00);
        pCurrent = pCurrent + _tcslen(pCurrent);

        // mirror
        sprintf(szDevspace, aParamTable[nMIndex].szName, nDevspace);
        pParameter = pXParamData->Get(szDevspace);
        if (pParameter != NULL) {
          // m-name
          sprintf(pCurrent, "%-*s", szDevspace.size() + 1, szDevspace.asCharp());
          pCurrent = pCurrent + _tcslen(pCurrent);

          // m-size
          sprintf(pCurrent, "%-11s", "");
          pCurrent = pCurrent + _tcslen(pCurrent);

          // m-type
          sprintf(szParameter, aParamTable[nMIndex].szType, nDevspace);
          pTypeParam = pXParamData->Get(szParameter);
          sprintf(pCurrent, "%-2s", (pTypeParam != NULL) ? pTypeParam->szNewValue.asCharp() : "");
          pCurrent = pCurrent + _tcslen(pCurrent);

          // m-path
          sprintf(pCurrent, " %s%s", pParameter->szNewValue.asCharp(), LINE_SEPSTRING_CN00);
          pCurrent = pCurrent + _tcslen(pCurrent);
        } // end if

        ++nDevspace;
        sprintf(szDevspace, aParamTable[nIndex].szName, nDevspace);
        pParameter = pXParamData->Get(szDevspace);
      } // end while

    } // end if

    // retrieve data devices
    if (_tcslen(szMode) == 0 || _tcsicmp(szMode, XP_DEV_DAT_CN00) == 0)
    {
        nIndex = 0;
        for (nIndex = 0; aParamTable[nIndex].nDevSpace != XPDevSpaceDat_ecn00; ++nIndex);

        for(nDevspace=1; nDevspace<=maxDataVolumes; nDevspace++)
        {
            sprintf(szDevspace, aParamTable[nIndex].szName, nDevspace);
            pParameter = pXParamData->Get(szDevspace);

            if(0!=pParameter)
            {
                // parameter name (of the volume name parameter)
                sprintf(pCurrent, "%-*s", szDevspace.size() + 1, szDevspace.asCharp());
                pCurrent = pCurrent + _tcslen(pCurrent);

                // size
                sprintf(szParameter, aParamTable[nIndex].szSize, nDevspace);
                pSizeParam = pXParamData->Get(szParameter);
                sprintf(pCurrent, "%-11s", (pSizeParam != NULL) ? pSizeParam->szNewValue.asCharp() : "");
                pCurrent = pCurrent + _tcslen(pCurrent);

                // type
                sprintf(szParameter, aParamTable[nIndex].szType, nDevspace);
                pTypeParam = pXParamData->Get(szParameter);
                sprintf(pCurrent, "%-2s", (pTypeParam != NULL) ? pTypeParam->szNewValue.asCharp() : "");
                pCurrent = pCurrent + _tcslen(pCurrent);

                // path
                sprintf(pCurrent, " %s%s", pParameter->szNewValue.asCharp(), LINE_SEPSTRING_CN00);
                pCurrent = pCurrent + _tcslen(pCurrent);
            }
        }
    } // end if

  } // end if

  *replyLen = (int)strlen(replyData);

  // if init then stop param management 
  if (bInit && (pXParamData != NULL)) {
    delete pXParamData;
    pXParamData = NULL;
  } /* end if */

  return nFuncReturn;
} // end cn20ParamGetDevSpaceAll

/* PUBLIC FUNCTION cn20ParamCalcDevSpace
 * ------------------------------------------------------------------
 * purpose: recalculate parameters dependent on dev spaces
 *
 */
tcn00_Error cn20ParamCalcDevSpace
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error        nFuncReturn = OK_CN00;
  DBMSrvPar_Parameters * pXParamData = (DBMSrvPar_Parameters *) vcontrol->pXParamSection;
  DBMSrvPar_Parameter * pParameter  = NULL;
  DBMSrvPar_Parameter* pWriteParameter(NULL);
  long               nSize    = 0;
  long               nMaxSize = 0;
  long               nSumSize = 0;
#if defined(SAPDBR73)
  char               szSize[XP_STRING_LENGTH_MXCN002 + 1];
#endif
  char               szNumber[XP_STRING_LENGTH_MXCN002 + 1];
  char               szParameter[XP_STRING_LENGTH_MXCN002 + 1];
  tcn00_DevspaceParams aParamTable[] = XP_DEVSPACE_PARAMS_CN00;
  long               nParam  = 0;   
  int                nNumber = 0;

  /* is there an active param session */
  if (nFuncReturn == OK_CN00) {
    if (pXParamData == NULL) {
      nFuncReturn = ERR_NOXPSESSION_CN00;
      cn90AnswerIError(replyData, replyLen, nFuncReturn);
    } /* end if */
  } /* end if */

  /* get parameters and check mode */
  if (nFuncReturn == OK_CN00) {
    while ((aParamTable[nParam].nDevSpace != XPDevSpaceUnknown_ecn00) && (nFuncReturn == OK_CN00)) {

      /* check and if necessary write maxcount (MAXARCHIVELOGS or MAXDATADEVSPACES) */
      if ((nFuncReturn == OK_CN00) && (strlen(aParamTable[nParam].szMaxCount) > 0)) {
        nNumber = 0;
        /* calculate count */
        do {
          nNumber++;
          sprintf(szParameter, aParamTable[nParam].szName, nNumber);

          pParameter = pXParamData->Get(szParameter);

      } while( (pParameter != NULL) && (strlen(pParameter->szNewValue) != 0) );

        /* write count */
        pWriteParameter = pXParamData->Get(aParamTable[nParam].szMaxCount);
        if (atoi(pWriteParameter->szNewValue) < nNumber) {
          do {
            sprintf(szNumber, "%d", nNumber + ((aParamTable[nParam].nDevSpace == XPDevSpaceDat_ecn00) ? 9 : 0));
            nFuncReturn = cn29SoftPutIntern( pXParamData, aParamTable[nParam].szMaxCount, szNumber, "addvolume");
            --nNumber;
          } while (nFuncReturn == ERR_XPCHECK_CN00);
        } /* end if */
      } /* end if */

      nParam++;
    } /* end while */

  } /* end if */

  /* generate answer */
  if (nFuncReturn == OK_CN00) {
    cn90AnswerOK(replyData, replyLen, NULL);
  } /* end if */

  return nFuncReturn;
} /* end cn20ParamCalcDevSpace */

/* ------------------------------------------------------------------
 * IMPLEMENTATION PUBLIC FUNCTION cn20CalcDevices
 * ------------------------------------------------------------------
 */
#undef  CN_FUNCTION
#define CN_FUNCTION "cn20CalcDevices"
tcn00_Error cn20CalcDevices
      ( VControlDataT * vcontrol,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  CONSOLE_TRACE_ENTER_CN00;

  tcn00_Error   nTmpState = OK_CN00;
  CommandT      oCommand;

  oCommand.setArgs("");

  // calc devspaces
  if (cn20ParamStartSession( vcontrol, 
                             &oCommand, 
                             replyData, 
                             replyLen, 
                             replyLenMax) == OK_CN00) {
    nTmpState = cn20ParamCalcDevSpace( vcontrol, 
                                       &oCommand, 
                                       replyData, 
                                       replyLen, 
                                       replyLenMax);

   
    if (nTmpState == OK_CN00) {
      nTmpState = cn20ParamCheckAll( vcontrol, 
                                     &oCommand, 
                                     replyData, 
                                     replyLen, 
                                     replyLenMax);
    } // end if
    if (nTmpState == OK_CN00) {
      nTmpState = cn20ParamCommitSession( vcontrol, 
                                           &oCommand, 
                                           replyData, 
                                           replyLen, 
                                           replyLenMax);
    } else {
      nTmpState = cn20ParamAbortSession( vcontrol, 
                                         &oCommand, 
                                         replyData, 
                                         replyLen, 
                                         replyLenMax);
    } // end if
  } // end if

  return nTmpState;
} // cn20CalcDevices

/* ------------------------------------------------------------------
 * IMPLEMENTATION PUBLIC FUNCTION cn20CheckedParamPut
 * ------------------------------------------------------------------
 */
#undef  CN_FUNCTION
#define CN_FUNCTION "cn20CheckedParamPut"
tcn00_Error cn20CheckedParamPut
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax,
        int             nCount,
        const char    * szNames[],
        const char    * szValues[])
{
  CONSOLE_TRACE_ENTER_CN00;

  tcn00_Error   nFuncReturn = OK_CN00;

  const char  * pArgs;
  pArgs = command->args;
  command->setArgs("");

  if (nFuncReturn == OK_CN00) {
    if (vcontrol->pXParamSection != NULL) {
      nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_XPSESSION_CN00);
    } // end if
  } // end if

  if (nFuncReturn == OK_CN00) {
    nFuncReturn = cn20ParamStartSession( vcontrol, 
                                         command, 
                                         replyData, 
                                         replyLen, 
                                         replyLenMax);
  } // end if

  if (nFuncReturn == OK_CN00) {
    DBMSrvPar_Parameters * pXParamData = (DBMSrvPar_Parameters *) vcontrol->pXParamSection;
    for (int i = 0; (i < nCount) && (nFuncReturn == OK_CN00); ++i) {
      // put here the parameter
      nFuncReturn = cn29SoftPutIntern( pXParamData, szNames[i], szValues[i], "", true);
    } // end for
  } // end if
  if (nFuncReturn != OK_CN00) {
    cn90AnswerIError(replyData, replyLen, nFuncReturn);
  } // end if

  if (nFuncReturn == OK_CN00) {
    nFuncReturn = cn20ParamCheckAll( vcontrol, 
                                     command, 
                                     replyData, 
                                     replyLen, 
                                     replyLenMax);
  } // end if

  if (nFuncReturn == OK_CN00) {
    nFuncReturn = cn20ParamCommitSession( vcontrol, 
                                          command, 
                                          replyData, 
                                          replyLen, 
                                          replyLenMax);
  } else {
    cn20FreeXParamSection (vcontrol);
  } // end if

  command->setArgs(pArgs);

  return nFuncReturn;
} // cn20CheckedParamPut

/* ------------------------------------------------------------------
 * IMPLEMENTATION PUBLIC FUNCTION cn20_CreateSrvParam
 * ------------------------------------------------------------------
 */
tcn00_Error cn20_CreateSrvParam
      ( tsp00_DbNamec   szDbName,
        tsp00_DbNamec   szMaster,
        const bool      bCheckOnly)
{
  FUNCTION_DBG_MCN00_1(_T("cn20_CreateSrvParam"));

  tcn00_Error         nFuncReturn   = OK_CN00;
  DBMSrvPar_Parameters * pXParamData   = NULL;
  tsp00_Pathc         szRundir;
  tsp00_C256          szWrk;
  tsp00_Versionc      VersionString;
  _TCHAR              replyData[1000];
  int                 replyLen;
  int                 replyLenMax = 1000;
  tsp01_RteError      rteError;

  // start session
  if (!bCheckOnly) {
    sqlXParamRemoveAll(szDbName, &rteError);
  } // end if
  nFuncReturn = cn29ParamStartSession( szDbName, pXParamData, replyData, &replyLen, replyLenMax);
  if (nFuncReturn == OK_CN00) {

    // collect some values
    cn90GetWrkPath(szWrk);
    if (strlen(szWrk) > 0) {
      sprintf(szRundir, "%s%s", szWrk.asCharp(), szDbName.asCharp());
      nFuncReturn = cn90DirCreate(szRundir, &rteError);
      if (nFuncReturn == OK_CN00) {
        sp100_GetVersionString(COMP_NAME_KERNEL_SP100, s100buildnumber, VersionString);
      } else {
        DBMLog_Logger::getInstance().addMessageList(DBMSrvMsg_NewRTEError(rteError));
      } // end if
    } else {
      nFuncReturn = ERR_FILE_CN00;
      Msg_List aEvent ( "DBM", __FILE__, __LINE__, TERR_CN00_1, nFuncReturn, "Empty Wrk Path", 0);
      DBMLog_Logger::getInstance().addMessageList(aEvent);
    } // end if

    // INSTANCE_TYPE
    if (nFuncReturn == OK_CN00) {
      nFuncReturn = cn29SoftPutIntern( pXParamData, PAN_INSTANCE_TYPE               , "OLTP"      , "Service DB", true);
      if (nFuncReturn != OK_CN00) {
        Msg_List aEvent ( "DBM", __FILE__, __LINE__, TERR_CN00_1, nFuncReturn, "Error putting %s", 1, PAN_INSTANCE_TYPE);
        DBMLog_Logger::getInstance().addMessageList(aEvent);
      } // end if
    } // end if

    // RUNDIRECTORY
    if (nFuncReturn == OK_CN00) {
      nFuncReturn = cn29SoftPutIntern( pXParamData, PAN_RUNDIR               , szRundir      , "Service DB", true);
      if (nFuncReturn != OK_CN00) {
        Msg_List aEvent ( "DBM", __FILE__, __LINE__, TERR_CN00_1, nFuncReturn, "Error putting %s", 1, PAN_RUNDIR);
        DBMLog_Logger::getInstance().addMessageList(aEvent);
      } // end if
    } // end if

    // KERNELVERSION 
    if (nFuncReturn == OK_CN00) {
      nFuncReturn = cn29SoftPutIntern( pXParamData, PAN_PARAM_KERNELVERSION  , VersionString , "Service DB", true);
      if (nFuncReturn != OK_CN00) {
        Msg_List aEvent ( "DBM", __FILE__, __LINE__, TERR_CN00_1, nFuncReturn, "Error putting %s", 1, PAN_PARAM_KERNELVERSION);
        DBMLog_Logger::getInstance().addMessageList(aEvent);
      } // end if
    } // end if

    // CONTROLUSERID
    if (nFuncReturn == OK_CN00) {
      nFuncReturn = cn29SoftPutIntern( pXParamData, PAN_CONTROLID            , "SERVICE"     , "Service DB", true);
      if (nFuncReturn != OK_CN00) {
        Msg_List aEvent ( "DBM", __FILE__, __LINE__, TERR_CN00_1, nFuncReturn, "Error putting %s", 1, PAN_CONTROLID);
        DBMLog_Logger::getInstance().addMessageList(aEvent);
      } // end if
    } // end if

    // CONTROLPASSWORD
    if (nFuncReturn == OK_CN00) {
      nFuncReturn = cn29SoftPutIntern( pXParamData, PAN_CONTROLPW            , "SERVICE"     , "Service DB", true);
      if (nFuncReturn != OK_CN00) {
        Msg_List aEvent ( "DBM", __FILE__, __LINE__, TERR_CN00_1, nFuncReturn, "Error putting %s", 1, PAN_CONTROLPW);
        DBMLog_Logger::getInstance().addMessageList(aEvent);
      } // end if
    } // end if

    // MAXUSERTASKS
    if (nFuncReturn == OK_CN00) {
      nFuncReturn = cn29SoftPutIntern( pXParamData, PAN_MAXACTIVE            , "1"           , "Service DB", true);
      if (nFuncReturn != OK_CN00) {
        Msg_List aEvent ( "DBM", __FILE__, __LINE__, TERR_CN00_1, nFuncReturn, "Error putting %s", 1, PAN_MAXACTIVE);
        DBMLog_Logger::getInstance().addMessageList(aEvent);
      } // end if
    } // end if

    // MAXBACKUPDEVS
    if (nFuncReturn == OK_CN00) {
      nFuncReturn = cn29SoftPutIntern( pXParamData, PAN_MAXBACKUPDEVS        , "32"           , "Service DB", true);
      if (nFuncReturn != OK_CN00) {
        Msg_List aEvent ( "DBM", __FILE__, __LINE__, TERR_CN00_1, nFuncReturn, "Error putting %s", 1, PAN_MAXBACKUPDEVS);
        DBMLog_Logger::getInstance().addMessageList(aEvent);
      } // end if
    } // end if

    // DATA_IO_BLOCK_COUNT
    if (nFuncReturn == OK_CN00) {
      nFuncReturn = cn29SoftPutIntern( pXParamData, (SAPDB_Char *) KERNELPARAM_DATA_IO_BLOCK_COUNT  , "128"           , "Service DB", true);
      if (nFuncReturn != OK_CN00) {
        Msg_List aEvent ( "DBM", __FILE__, __LINE__, TERR_CN00_1, nFuncReturn, "Error putting %s", 1, (SAPDB_Char *) KERNELPARAM_DATA_IO_BLOCK_COUNT);
        DBMLog_Logger::getInstance().addMessageList(aEvent);
      } // end if
    } // end if

    // CACHE_SIZE
    if (nFuncReturn == OK_CN00) {
      nFuncReturn = cn29SoftPutIntern( pXParamData, PAN_CACHE_SIZE           , "800"         , "Service DB", true);
      if (nFuncReturn != OK_CN00) {
        Msg_List aEvent ( "DBM", __FILE__, __LINE__, TERR_CN00_1, nFuncReturn, "Error putting %s", 1, PAN_CACHE_SIZE);
        DBMLog_Logger::getInstance().addMessageList(aEvent);
      } // end if
    } // end if

    if ( (nFuncReturn == OK_CN00) && !bCheckOnly) {

      // DATA DEVSPACE
      if (nFuncReturn == OK_CN00) {
        nFuncReturn = cn29SoftPutIntern( pXParamData, PAN_DATADEV_N"0001"     , "CHECKDEV"    , "Service DB", true);
        if (nFuncReturn != OK_CN00) {
          Msg_List aEvent ( "DBM", __FILE__, __LINE__, TERR_CN00_1, nFuncReturn, "Error putting %s", 1, "DATADEV_0001");
          DBMLog_Logger::getInstance().addMessageList(aEvent);
        } // end if
      } // end if
      if (nFuncReturn == OK_CN00) {
        nFuncReturn = cn29SoftPutIntern( pXParamData, PAN_DATADEV_N_TYPE"0001", "F"           , "Service DB", true);
        if (nFuncReturn != OK_CN00) {
          Msg_List aEvent ( "DBM", __FILE__, __LINE__, TERR_CN00_1, nFuncReturn, "Error putting %s", 1, "DATA_TYPE_0001");
          DBMLog_Logger::getInstance().addMessageList(aEvent);
        } // end if
      } // end if
      if (nFuncReturn == OK_CN00) {
        nFuncReturn = cn29SoftPutIntern( pXParamData, PAN_DATADEV_N_SIZE"0001", "3000"        , "Service DB", true);
        if (nFuncReturn != OK_CN00) {
          Msg_List aEvent ( "DBM", __FILE__, __LINE__, TERR_CN00_1, nFuncReturn, "Error putting %s", 1, "DATA_SIZE_0001");
          DBMLog_Logger::getInstance().addMessageList(aEvent);
        } // end if
      } // end if

    } // end if

    if (nFuncReturn == OK_CN00 && szMaster.length() > 0) {
      tcn002_XpValueString sValue;
      nFuncReturn  = cn20XParamGetValue(szMaster, PAN_UNICODE_CATALOG, sValue);
      if (nFuncReturn == OK_CN00) {
        nFuncReturn = cn29SoftPutIntern( pXParamData, PAN_UNICODE_CATALOG, sValue, "Service DB", true);
      } // end if
      if (nFuncReturn != OK_CN00) {
        Msg_List aEvent ( "DBM", __FILE__, __LINE__, TERR_CN00_1, nFuncReturn, "Error putting %s", 1, "DATA_SIZE_0001");
        DBMLog_Logger::getInstance().addMessageList(aEvent);
      } // end if
    } // end if

    // check and save
    if (nFuncReturn == OK_CN00) {
      nFuncReturn = cn20ParamCheckAll( szDbName, pXParamData, "", replyData, &replyLen, replyLenMax);
      if (nFuncReturn == OK_CN00) {
        nFuncReturn = cn20ParamCommitSession(szDbName, pXParamData, replyData, &replyLen, replyLenMax);
        if (nFuncReturn != OK_CN00) {
          Msg_List aEvent ( "DBM", __FILE__, __LINE__, TERR_CN00_1, nFuncReturn, "Error commit param session\n%s", 1, replyData);
          DBMLog_Logger::getInstance().addMessageList(aEvent);
        } // end if
      } else {
        Msg_List aEvent ( "DBM", __FILE__, __LINE__, TERR_CN00_1, nFuncReturn, "Error check params\n%s", 1, replyData);
        DBMLog_Logger::getInstance().addMessageList(aEvent);
      } // end if
    } // endif

    if (pXParamData != NULL) {
      delete pXParamData;
      pXParamData = NULL;
    } /* end if */

  } else {
      Msg_List aEvent ( "DBM", __FILE__, __LINE__, TERR_CN00_1, nFuncReturn, "Error start param session\n%s", 1, replyData);
      DBMLog_Logger::getInstance().addMessageList(aEvent);
  } // end if

  return nFuncReturn;
} // cn20_CreateSrvParam

/* ------------------------------------------------------------------
 * IMPLEMENTATION PUBLIC FUNCTION cn20CreateSrvParam
 * ------------------------------------------------------------------
 */
tcn00_Error cn20CreateSrvParam
      ( tsp00_DbNamec   szDbName )
{
  tsp00_DbNamec szMaster;
  szMaster.Init();
  return cn20_CreateSrvParam(szDbName, szMaster, false);
} // cn20_CreateSrvParam
/* ------------------------------------------------------------------
 * IMPLEMENTATION PUBLIC FUNCTION cn20CreateSrvParam
 * ------------------------------------------------------------------
 */
tcn00_Error cn20CreateSrvParam
      ( tsp00_DbNamec   szDbName,
        tsp00_DbNamec   szMaster )
{
  return cn20_CreateSrvParam(szDbName, szMaster, true);
} // cn20_CreateSrvParam

/* PUBLIC FUNCTION cn20ParamRmFile
 * ------------------------------------------------------------------
 *
 */
tcn00_Error cn20ParamRmFile
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error        nFuncReturn = OK_CN00;

  /* read the parameters */
  if (nFuncReturn == OK_CN00) {
    if (cn90GetToken(command->args, NULL, 1, -1)         ||
       (vcontrol->dbname[0] == CHAR_STRINGTERM_CN90)    ) {
      nFuncReturn = ERR_PARAM_CN00;
      cn90AnswerIError(replyData, replyLen, nFuncReturn);
    } /* end if */
  } /* end if */

  /* check existence of session */
  if (nFuncReturn == OK_CN00) {
    if (vcontrol->pXParamSection != NULL) {
      nFuncReturn = ERR_XPSESSION_CN00;
      cn90AnswerIError(replyData, replyLen, nFuncReturn);
    } /* end if */
  } /* end if */

  /* delete the paramfile */
  if (nFuncReturn == OK_CN00) {
    tsp00_Pathc szType;
    tsp00_Pathc szParamFile;

    szType.rawAssign("PARAM");
    nFuncReturn =cn42GetFileName(vcontrol->dbname, szType, szParamFile);
    if (nFuncReturn == OK_CN00) {
      struct stat oState;
      int         nState = stat(szParamFile, &oState);

      if (nState == 0) {
        tsp05_RteFileError rteFileErr;
        sqlferasec (szParamFile, &rteFileErr);
        if (rteFileErr.sp5fe_result != vf_ok) {
          DBMSrvMsg_RTEError rteError(rteFileErr);
          rteError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILE));
          nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(rteError);
        } // end if
      } else {
        if (errno != ENOENT) {
          nFuncReturn = cn90AnswerErrnoError(replyData, replyLen, ERR_FILE_CN00, errno);
        } // end if
      } // end if
    } else {
      cn90AnswerIError(replyData, replyLen, nFuncReturn);
    } // end if
  } /* end if */

  if (nFuncReturn == OK_CN00) {
    cn90AnswerOK(replyData, replyLen, NULL);
  } /* end if */

  return nFuncReturn;
} /* end cn20ParamRmFile */

/* PUBLIC FUNCTION cn20ParamRestore
 * ------------------------------------------------------------------
 * purpose: Restore old param file 
 */
tcn00_Error cn20ParamRestore
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error        nFuncReturn = OK_CN00;
  char               szVersion[PARAMETER_MAXLEN_CN90];
  char               szSource[PARAMETER_MAXLEN_CN90];
  char               szTarget[PARAMETER_MAXLEN_CN90];
  tsp00_C256         szConfig;

  tsp01_RteError     aRteError;

  /* read the parameters */
  if (nFuncReturn == OK_CN00) {
    if (!cn90GetToken(command->args, szVersion, 1, PARAMETER_MAXLEN_CN90)    ||
        cn90GetToken(command->args, NULL, 2, -1)          ||
       (vcontrol->dbname[0] == CHAR_STRINGTERM_CN90)    ) {
      nFuncReturn = ERR_PARAM_CN00;
      cn90AnswerIError(replyData, replyLen, nFuncReturn);
    } /* end if */
  } /* end if */

  /* check existence of session */
  if (nFuncReturn == OK_CN00) {
    if (vcontrol->pXParamSection != NULL) {
      nFuncReturn = ERR_XPSESSION_CN00;
      cn90AnswerIError(replyData, replyLen, nFuncReturn);
    } /* end if */
  } /* end if */

  // write the history
  if (nFuncReturn == OK_CN00) {
    DBMSrvPar_RTEConfParameter          oConfigVersion (vcontrol->dbname.asCharp(), 
                                               atoi(szVersion),
                                               RTECONF_MAXNAMELENGTH,
                                               RTECONF_MAXSTRINGLENGTH);
    DBMSrvPar_RTEConfParameter          oConfigOriginal(vcontrol->dbname.asCharp(), 
                                               0, 
                                               RTECONF_MAXNAMELENGTH,
                                               RTECONF_MAXSTRINGLENGTH);
    DBMSrvPar_RTEConfParameter::Name    sName;
    DBMSrvPar_Parameter          aParameter;
    DBMSrvMsg_Error            oMsg;
    SAPDB_Bool                 bEnd        = false;

    if (oConfigVersion.Read(oMsg) && 
        oConfigOriginal.Read(oMsg)    ) {

      RTEConf_ParameterIterator  oIteratorVersion  = oConfigVersion.GetIterator();
      RTEConf_ParameterIterator  oIteratorOriginal = oConfigOriginal.GetIterator();

      // search for changed/new parameters
      while (oMsg.IsEmpty()) {
      // iterate through version file
      oIteratorVersion.Next(sName, aParameter.xpRecord.xpType, bEnd, oMsg);

        // read version file
        if (oMsg.IsEmpty() && !bEnd) {
          switch (aParameter.xpRecord.xpType) {
            case DBMSrvPar_RTEConfParameter::t_Integer:
              oConfigVersion.GetValue (sName, aParameter.xpRecord.xpValue.xpValueInt, oMsg);
              break;
            case DBMSrvPar_RTEConfParameter::t_Real:        
              oConfigVersion.GetValue (sName, aParameter.xpRecord.xpValue.xpValueReal, oMsg);
              break;
            case DBMSrvPar_RTEConfParameter::t_String:
              oConfigVersion.GetValue (sName, (DBMSrvPar_RTEConfParameter::String) aParameter.xpRecord.xpValue.xpValueString.asCharp(), oMsg);
              break;
            case DBMSrvPar_RTEConfParameter::t_CryptInfo:  
              oConfigVersion.GetValue (sName, aParameter.xpRecord.xpValue.xpValueCrypt, oMsg);
              break;
          } // end switch
        } // end if

        // read original file
        if (oMsg.IsEmpty() && !bEnd) {
          SAPDB_strcpy(aParameter.szParameterName, (char *) sName);
          cn29ParamValueToString(aParameter.szNewValue, &aParameter.xpRecord);

          switch (aParameter.xpRecord.xpType) {
            case DBMSrvPar_RTEConfParameter::t_Integer:
              oConfigOriginal.GetValue (sName, aParameter.xpRecord.xpValue.xpValueInt, oMsg);
              break;
            case DBMSrvPar_RTEConfParameter::t_Real:        
              oConfigOriginal.GetValue (sName, aParameter.xpRecord.xpValue.xpValueReal, oMsg);
              break;
            case DBMSrvPar_RTEConfParameter::t_String:
              oConfigOriginal.GetValue (sName, (DBMSrvPar_RTEConfParameter::String) aParameter.xpRecord.xpValue.xpValueString.asCharp(), oMsg);
              break;
            case DBMSrvPar_RTEConfParameter::t_CryptInfo:  
              oConfigOriginal.GetValue (sName, aParameter.xpRecord.xpValue.xpValueCrypt, oMsg);
              break;
          } // end switch

          if (oMsg.IsEmpty()) {
            cn29ParamValueToString(aParameter.szOldValue, &aParameter.xpRecord);
          } else {
            SAPDB_strcpy(aParameter.szOldValue, "");
            oMsg.ClearMessageList();
          } // end if
          // write history
          cn29WriteParamHistory(vcontrol->dbname, &aParameter, PAHI_CHANGE_CN002);
        } // end if

      } // end while

      // search for deleted parameters
      while (oMsg.IsEmpty()) {
        // iterate through original file
        oIteratorOriginal.Next(sName, aParameter.xpRecord.xpType, bEnd, oMsg);

        // read original file
        if (oMsg.IsEmpty() && !bEnd) {
          switch (aParameter.xpRecord.xpType) {
            case DBMSrvPar_RTEConfParameter::t_Integer:
              oConfigOriginal.GetValue (sName, aParameter.xpRecord.xpValue.xpValueInt, oMsg);
              break;
            case DBMSrvPar_RTEConfParameter::t_Real:        
              oConfigOriginal.GetValue (sName, aParameter.xpRecord.xpValue.xpValueReal, oMsg);
              break;
            case DBMSrvPar_RTEConfParameter::t_String:
              oConfigOriginal.GetValue (sName, (DBMSrvPar_RTEConfParameter::String) aParameter.xpRecord.xpValue.xpValueString.asCharp(), oMsg);
              break;
            case DBMSrvPar_RTEConfParameter::t_CryptInfo:  
              oConfigOriginal.GetValue (sName, aParameter.xpRecord.xpValue.xpValueCrypt, oMsg);
              break;
          } // end switch
        } // end if

        // read version file
        if (oMsg.IsEmpty() && !bEnd) {
          SAPDB_strcpy(aParameter.szParameterName, (char *) sName);
          cn29ParamValueToString(aParameter.szOldValue, &aParameter.xpRecord);

          switch (aParameter.xpRecord.xpType) {
            case DBMSrvPar_RTEConfParameter::t_Integer:
              oConfigVersion.GetValue (sName, aParameter.xpRecord.xpValue.xpValueInt, oMsg);
              break;
            case DBMSrvPar_RTEConfParameter::t_Real:        
              oConfigVersion.GetValue (sName, aParameter.xpRecord.xpValue.xpValueReal, oMsg);
              break;
            case DBMSrvPar_RTEConfParameter::t_String:
              oConfigVersion.GetValue (sName, (DBMSrvPar_RTEConfParameter::String) aParameter.xpRecord.xpValue.xpValueString.asCharp(), oMsg);
              break;
            case DBMSrvPar_RTEConfParameter::t_CryptInfo:  
              oConfigVersion.GetValue (sName, aParameter.xpRecord.xpValue.xpValueCrypt, oMsg);
              break;
          } // end switch

          if (!oMsg.IsEmpty()) {
            SAPDB_strcpy(aParameter.szNewValue, "");
            cn29WriteParamHistory(vcontrol->dbname, &aParameter, PAHI_DELETE_CN002);
          } // end if
        } // end if
      } // end while

    } // end if
  } // end if

  /* copy the version file */
  if (nFuncReturn == OK_CN00) {

    cn90GetCfgPath(szConfig);

    sprintf(szTarget, "%s%s",  (char *) szConfig, (char *) vcontrol->dbname);
    sprintf(szSource, "%s.%02d", (char *) szTarget, atoi(szVersion));

    if (!sqlfilecopyc( szSource, szTarget, &aRteError )) {
      nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(DBMSrvMsg_NewRTEError(aRteError));
#ifndef _WIN32
    } else {
      chmod(szTarget, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
#endif
    } /* end if */

  } /* end if */

  if (nFuncReturn == OK_CN00) {
    cn90AnswerOK(replyData, replyLen, NULL);
  } /* end if */

  return nFuncReturn;
} /* end  cn20ParamRestore */

/* PUBLIC FUNCTION cn20ParamVersions
 * ------------------------------------------------------------------
 * purpose: delete one named paramter from the param file
 */
tcn00_Error cn20ParamVersions
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error        nFuncReturn = OK_CN00;
  char             * pCurrPos;
  void             * hDirectory  = NULL;  
  bool               bOpen       = false;
  tsp00_C256           szFile;
  tsp00_C256           szConfig;
  tsp05_RteFileError rteFileErr;
  DBMSrvMsg_Error errRTECollector;

  /* read the parameters */
  if (nFuncReturn == OK_CN00) {
    if (cn90GetToken(command->args, NULL, 1, -1)         ||
       (vcontrol->dbname[0] == CHAR_STRINGTERM_CN90)    ) {
      nFuncReturn = ERR_PARAM_CN00;
      cn90AnswerIError(replyData, replyLen, nFuncReturn);
    } /* end if */
  } /* end if */

  /* open config dir */
  if (nFuncReturn == OK_CN00) {
    cn90GetCfgPath(szConfig);

    szConfig[strlen(szConfig) - 1] = CHAR_STRINGTERM_CN90;
    sqlfopendirc ( &szConfig, &hDirectory,  &szFile, &rteFileErr);
    if  ( rteFileErr.sp5fe_result != vf_ok) {
      errRTECollector.Overrule(DBMSrvMsg_RTEError(rteFileErr));
      nFuncReturn = DBMSrvMsg_Error::get_tcn00_Error(DBMSrvMsg_Error::DBMSrv_RTE);
    } else {
      bOpen = true;
    } /* end if */
  } /* end if */

  /* analyze files */
  if (nFuncReturn == OK_CN00) {

    /* Init Output */
    sprintf(replyData, "%s%s", ANSWER_OK_CN00, LINE_SEPSTRING_CN00);
    pCurrPos = replyData + strlen (replyData);

    while ( rteFileErr.sp5fe_result == vf_ok) {

      if ( ( strncmp(szFile, vcontrol->dbname, strlen(vcontrol->dbname)) == 0 ) &&
           ( ( strlen(vcontrol->dbname) == strlen(szFile)           ) ||
             ( (strlen(vcontrol->dbname) + 3) == strlen(szFile) &&
               szFile[strlen(vcontrol->dbname)] == '.'          &&
               isdigit(szFile[strlen(vcontrol->dbname) + 1])    &&
               isdigit(szFile[strlen(vcontrol->dbname) + 2])        )         )    ) {

       sprintf(pCurrPos, "%s%c%s%s", (char *) szConfig, PATH_DELIMITER_CN90, (char *) szFile, LINE_SEPSTRING_CN00);
       pCurrPos = replyData + strlen (replyData);
      } /* end if */

      /* get the next name */
      sqlfreaddirc(hDirectory,  &szFile, &rteFileErr);
    } /* end while */

  } /* end if */

  /* close */
  if (bOpen) {
    sqlfclosedirc (hDirectory, &rteFileErr);
    if (nFuncReturn == OK_CN00 && rteFileErr.sp5fe_result != vf_ok) {
      errRTECollector.Overrule(DBMSrvMsg_RTEError(rteFileErr));
      nFuncReturn = DBMSrvMsg_Error::get_tcn00_Error(DBMSrvMsg_Error::DBMSrv_RTE);
    } /* end if */
  } /* end if */

  /* check result */
  if (nFuncReturn == OK_CN00) {
    *replyLen = (int)strlen(replyData);
  } else if (errRTECollector == DBMSrvMsg_Error::DBMSrv_RTE) {
    DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(errRTECollector);
  } else {
    cn90AnswerIError(replyData, replyLen, nFuncReturn);
  } /* end if */

  return nFuncReturn;
} /* end  cn20ParamVersions */

/* PUBLIC FUNCTION cn20ParamHistory
 * ------------------------------------------------------------------
 */
tcn00_Error cn20ParamHistory
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error              nFuncReturn = OK_CN00;
  DBMSrvMsg_Error          oError;
  _TCHAR                 * pCurrent    = replyData;
  bool                     bOk         = true;
  bool                     bEnd        = false;
  bool                     bBufferLen  = true;
  cn52ParamHistoryRecord * pRecord;

  cn20FreeParamHistory(vcontrol);
  pRecord = new cn52ParamHistoryRecord(vcontrol->dbname);
  vcontrol->pParamHistory = (void *) pRecord;

  tcn52_C1024c szSelection;
  bOk = pRecord->setSelection(szSelection.rawAssign(command->args));

  if (bOk) {

    _stprintf(pCurrent, "%s%s", ANSWER_OK_CN00, LINE_SEPSTRING_CN00);
    pCurrent = pCurrent + _tcslen (pCurrent);
    _stprintf(pCurrent, "%s%s", pRecord->getHeader().asCharp(), LINE_SEPSTRING_CN00);
    pCurrent = pCurrent + _tcslen (pCurrent);

    bOk = pRecord->readFirst(oError, bEnd);

    while (bOk && !bEnd && bBufferLen) {

      _stprintf(pCurrent, "%s%s", pRecord->getRecord().asCharp(), LINE_SEPSTRING_CN00);
      pCurrent = pCurrent + _tcslen (pCurrent);

      bOk = pRecord->readNext(oError, bEnd);
      if (bOk && ((pCurrent - replyData + pRecord->getRecord().length() + 5) > replyLenMax)) {
        bBufferLen = false;
      } // end if

    } // end if
  
    if (!bOk || bEnd) {
        if (!bOk) {
            oError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILE));
            nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(oError);
        } // end if
        cn20FreeParamHistory(vcontrol);
    } else {
      // prepare next command 
      cn00DBMServerData::vcontrol()->szNextCommand.rawAssign("param_gethistorynext");
      cn00DBMServerData::vcontrol()->nNextCommandSkip = 1;
    } // end if
  
  } else {
    nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00);
    cn20FreeParamHistory(vcontrol);
  } // end if

  *replyLen = (int)_tcslen(replyData);

  return nFuncReturn;
} // end cn20ParamHistory

/* PUBLIC FUNCTION cn20ParamHistoryNext
 * ------------------------------------------------------------------
 */
tcn00_Error cn20ParamHistoryNext
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error              nFuncReturn = OK_CN00;
  DBMSrvMsg_Error          oError;
  _TCHAR                 * pCurrent = replyData;
  bool                     bOk         = true;
  bool                     bEnd        = false;
  bool                     bBufferLen  = true;
  cn52ParamHistoryRecord * pRecord = (cn52ParamHistoryRecord *) vcontrol->pParamHistory;
  
  _stprintf(pCurrent, "%s%s", ANSWER_OK_CN00, LINE_SEPSTRING_CN00);
  pCurrent = pCurrent + _tcslen (pCurrent);

  if (pRecord != NULL) {
    _stprintf(pCurrent, "%s%s", pRecord->getHeader().asCharp(), LINE_SEPSTRING_CN00);
    pCurrent = pCurrent + _tcslen (pCurrent);

    while (bOk && !bEnd && bBufferLen) {

      _stprintf(pCurrent, "%s%s", pRecord->getRecord().asCharp(), LINE_SEPSTRING_CN00);
      pCurrent = pCurrent + _tcslen (pCurrent);

      bOk = pRecord->readNext(oError, bEnd);
      if (bOk && ((pCurrent - replyData + pRecord->getRecord().length() + 5) > replyLenMax)) {
        bBufferLen = false;
      } // end if

    } // end if
  
    if (!bOk || bEnd) {
      if (!bOk) {
        oError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILE));
        nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(oError);
      } // end if
      cn20FreeParamHistory(vcontrol);
    } else {
      // prepare next command 
      cn00DBMServerData::vcontrol()->szNextCommand.rawAssign("param_gethistorynext");
      cn00DBMServerData::vcontrol()->nNextCommandSkip = 1;
    } // end if
  } // end if

  *replyLen = (int)_tcslen(replyData);

  return nFuncReturn;
} // end cn20ParamHistoryNext

/* PUBLIC FUNCTION cn20FreeParamHistory
 * ------------------------------------------------------------------
 */
void cn20FreeParamHistory(VControlDataT * vcontrol)
{

  if (vcontrol != NULL) {
    cn52ParamHistoryRecord * pRecord = (cn52ParamHistoryRecord *) vcontrol->pParamHistory;

    if (pRecord != NULL) {
      delete pRecord;
      vcontrol->pParamHistory = NULL;
    } // end if
  } // end if

} // end cn20FreeParamHistory

/* PUBLIC FUNCTION cn20SaveParamsInfoToDB
 * ------------------------------------------------------------------
 * purpose: save param properties to database table
 */

#define DELETE_PARAMSINFO _T("DELETE SYSDBM.DBPARAMSINFO")
#define INSERT_PARAMSINFO _T("INSERT INTO SYSDBM.DBPARAMSINFO VALUES ('%.32s','%.5s','%.8s','%.128s','%.64s') UPDATE DUPLICATES")
//                                                              PARAMID
//                                                                   TYPE
//                                                                        GROUPSPEC
//                                                                             HELP
//                                                                                  DEFVALUE

tcn00_Error cn20SaveParamsInfoToDB
      ( const tsp00_DbNamec & szDbName )
{
  tcn00_Error         nFuncReturn   = OK_CN00;
  DBMSrvPar_Parameters * pXParamData   = NULL;
  _TCHAR              replyData[1000];
  int                 replyLen;
  int                 replyLenMax = 1000;
  // start session
  nFuncReturn = cn29ParamStartSession( szDbName, pXParamData, replyData, &replyLen, replyLenMax);

  if (nFuncReturn == OK_CN00) {
    cn50DBMUser       usrDBM    ( szDbName, cn50DBMUser::getUser(szDbName, FIRSTDBM_CN50));
    DBMSrvKnl_Session oSession;
    
    DBMSrvMsg_Error oError;
    if (!oSession.NormalConnect(szDbName.asCharp(),
                                usrDBM.getUserName().asCharp(),
                                usrDBM.getClearMasterPwd().asCharp(),
                                oError)) {
      nFuncReturn = oError.ID();
    } // end if

    if (nFuncReturn == OK_CN00) {
      DBMSrvPar_Parameter * pParameter    = NULL;
      _TCHAR                szStatement[PARAMETER_MAXLEN_CN90];
      const _TCHAR        * pHelp         = NULL;
      _TCHAR                szHelp[129];
      tcn00_ValueToText     aTypeTable[]  = XP_TYPES_CN002;
      int                   nType         = 0;
      struct {
        DBMSrvPar_Parameter::XPPropValue   nValue;
        const char         * szText;
      }                      aValueTable[] = XP_PROP_VALUES_CN002;
      int                   nValue         = 0;
      int                   i,j;

      oSession.Execute(DELETE_PARAMSINFO, oError);

      pParameter = pXParamData->pFirst;
      while (pParameter != NULL && nFuncReturn == OK_CN00) {

        // save only parameters from file
        if (pParameter->bParamExist) {

          // help
          pHelp = (pParameter->pHelp != NULL) ? pParameter->pHelp->Line() : "";
          for (i = 0,j = 0; i < 128 && pHelp[i] != 0; ++i) {
            szHelp[j] = pHelp[i];
            if (szHelp[j] == _T('\'') ) {
              ++j;
              szHelp[j] = pHelp[i];
            } // end if
            ++j;
          } // end if
          szHelp[j] = 0;

          // type
          nType = 0;
          while ( (_tcslen(aTypeTable[nType].szText) > 0) && (pParameter->xpRecord.xpType != aTypeTable[nType].nValue) ) {
            ++nType;
          } // while
          // group
          nValue = 0;
          while ( (aValueTable[nValue].nValue != DBMSrvPar_Parameter::XPPropValueUnknown                         ) && 
                  (aValueTable[nValue].nValue != pParameter->xpProperties[XPPropClassGroup] )    ) {
            nValue++;
          } // end while 

          _stprintf(szStatement, INSERT_PARAMSINFO, 
                                 pParameter->szParameterName.asCharp(),   // PARAMID
                                 aTypeTable[nType].szText,      // TYPE
                                 (aValueTable[nValue].szText == NULL) ? "" : aValueTable[nValue].szText,    // GROUPSPEC
                                 szHelp,                        // HELP
                                 pParameter->szDefValue.asCharp());       // DEFVALUE

          if (!oSession.ExecuteWithoutCommit(szStatement, oError)) {
            nFuncReturn = oError.ID();
          } // end if
        } // end if

        pParameter = pParameter->pNext;
      } // end while

      oSession.Release(oError);
    } // end if

    if (pXParamData != NULL) {
      delete pXParamData;
      pXParamData = NULL;
    } /* end if */
  } // end if

  return nFuncReturn;
} // end cn20SaveParamsInfoToDB
