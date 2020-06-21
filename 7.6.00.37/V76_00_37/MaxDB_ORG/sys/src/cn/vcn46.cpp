/*!
  @file           vcn46.cpp
  @author         Bernd Vorsprach - bernd.vorsprach@sap.com
  @brief          hot standy functions

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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


\endif
*/

/* ------------------------------------------------------------------
 * includes
 * ------------------------------------------------------------------
 */
#include "hcn46.h"

#include "hsp100.h"

#include "hcn20.h"
#include "hcn40.h"
#include "hcn50.h"
#include "hcn90.h"

#include "SAPDB/DBM/Srv/HSS/DBMSrvHSS_Nodes.hpp"
#include "SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_Session.hpp"

#define HSS_VALUE_DELAY   "delay"
#define HSS_VALUE_PATH    "path"
#define HSS_VALUE_LOGIN   "login"
#define HSS_VALUE_NODE    "node"
#define HSS_VALUE_LIB     "lib"
#define HSS_VALUE_USER    "user"

#define HSS_OPT_YES       "YES"

#define SQL_CHECKROLE         "select STANDBYROLE from hotstandbycomponent"
#define HSS_ROLE_MASTER       "MASTER"

#define SQL_GETRESTARTVERSION "GET RESTART VERSION"
#define SQL_FIELD_MASTER      "MASTER SERVERNODE"

#define HSS_LIB_DEFAULT  "libhsscopy"

/*! @brief cn46AddStandby */
tcn00_Error cn46AddStandby
    ( VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  FUNCTION_DBG_MCN00_1("cn46AddStandby");

  tcn00_Error nFuncReturn = OK_CN00;

  Tools_DynamicUTF8String sNode  = command->oArguments.getValue(1);
  Tools_DynamicUTF8String sLogin = command->oArguments.getValue(HSS_VALUE_LOGIN);
  Tools_DynamicUTF8String sOsUser;
  Tools_DynamicUTF8String sOsPwd;
  Tools_DynamicUTF8String sDelay = command->oArguments.getValue(HSS_VALUE_DELAY);
  Tools_DynamicUTF8String sPath  = command->oArguments.getValue(HSS_VALUE_PATH);
  Tools_DynamicUTF8String sUser  = command->oArguments.getValue(HSS_VALUE_USER);
  SAPDB_Bool              bUser  = sUser.ToUpper().Compare(HSS_OPT_YES) == 0;

  // check parameters
  if (sNode.Empty()) {
    nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00, 0, "missing node name");
  } else if (sLogin.Empty()) {
    nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00, 0, "missing operating system user");
  } else if (vcontrol->pHSSNodes->Exists(sNode)) {
    nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00, 0, "node already exists in cluster");
  } else if (!vcontrol->pHSSNodes->Exists(RTE_ISystem::Instance().GetLocalNodeName())) {
    nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00, 0, "current node is not a hot standby node");
  } // end if

  // analyze os user
  if (nFuncReturn == OK_CN00) {
    Tools_DynamicUTF8String::BasisElementIndex nIndex;

    nIndex = sLogin.Find(',', 0);

    if (nIndex == Tools_DynamicUTF8String::NPos) {
      nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00, 0, "missing operating system user");
    } else {
      sOsUser = sLogin.SubStr(0, nIndex);
      sOsPwd  = sLogin.SubStr(nIndex + 1);
    } // end if

  } // end if  

  // check database state
  if (nFuncReturn == OK_CN00) {
    if (DBMSrvKnl_State::State(vcontrol->dbname) != DBMSrvKnl_State::StateOnline) {
      nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_DBNORUN_CN00);
    } // end if
  } // end if

  // check master state
  if (nFuncReturn == OK_CN00) {
    if (!cn46IsMaster(vcontrol->dbname)) {
      nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_NOTMASTER_CN00);
    } // end if
  } // end if

  if (nFuncReturn == OK_CN00 && sPath.Empty() ) {
    // find matching installation on server
    Msg_List oMsgList;
    DBMCli_Node oNode(sNode, oMsgList, true);
    if (oMsgList.IsEmpty()) {
      DBMCli_Version oVersion;
      DBMCli_String  sInstRoot;

      if (oNode.EnumerateInstallations(oMsgList)) {
        DBMCli_EnumInstallationArray & oInstallations = oNode.EnumInstallationArray();
        int nIndex = 0;

        // get own version
        tsp100_VersionID0   VersionID0;
        sp100_GetVersionID ( VersionIDType0_esp100, s100buildnumberEx, &VersionID0 );

        for (nIndex = 0; nIndex < oInstallations.GetSize(); ++nIndex) {
          DBMCli_Version oVersion = oInstallations[nIndex].Version();

          if (VersionID0.MajorVersion_sp100 == oVersion.GetMajor()    &&
              VersionID0.MinorVersion_sp100 == oVersion.GetMinor()    &&
              VersionID0.CorrLevel_sp100    == oVersion.GetRevision()    ) {
            sPath = (const char *) oInstallations[nIndex].InstRoot();
            break;
          } // end if
        } // end for
      } // end if
    } // end if

    if (!oMsgList.IsEmpty()) {
      oMsgList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_HSS));
      nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(oMsgList);
    } else if (sPath.Empty()) {
      nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_NOVERSIONMATCH, Msg_Arg(SDBMSGTAG_DBMSRV_NOVERSIONMATCH__DBMSRV_COMP,sNode.CharPtr())));
    } // end if

  } else if (nFuncReturn == OK_CN00 && !sPath.Empty() ) {
    // check specified version
    Msg_List oMsgList;
    DBMCli_Node oNode(sNode, oMsgList, false);

    if (oMsgList.IsEmpty()) {
      oNode.SetInstRoot(sPath);
      oNode.Connect(oMsgList);
    } // end if

    if (oMsgList.IsEmpty()) {
      oNode.NodeInfo().Refresh(oMsgList);

      if (oMsgList.IsEmpty()) {
        DBMCli_Version & oVersion = oNode.NodeInfo().Version();

        // get own version
        tsp100_VersionID0   VersionID0;
        sp100_GetVersionID ( VersionIDType0_esp100, s100buildnumberEx, &VersionID0 );

        if (VersionID0.MajorVersion_sp100 != oVersion.GetMajor()    ||
            VersionID0.MinorVersion_sp100 != oVersion.GetMinor()    ||
            VersionID0.CorrLevel_sp100    != oVersion.GetRevision()    ) {
          Tools_DynamicUTF8String sError;
          sError.Assign(sNode).Append(" (").Append(sPath).Append(")");
          nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_NOVERSIONMATCH, Msg_Arg(SDBMSGTAG_DBMSRV_NOVERSIONMATCH__DBMSRV_COMP,sError.CharPtr())));
        } // end if
      } // end if
    } // end if

    if (!oMsgList.IsEmpty()) {
      oMsgList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_HSS));
      nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(oMsgList);
    } // end if
  } // end if

  if (nFuncReturn == OK_CN00) {
    DBMSrvMsg_Error oMsgList;
    if (!vcontrol->pHSSNodes->Create(sNode, sOsUser, sOsPwd, sPath, atoi(sDelay.CharPtr()), bUser, oMsgList)) {
      nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(oMsgList);
    } // end if
  } // end if

  if (nFuncReturn == OK_CN00) {
    cn90AnswerOK(replyData, replyLen, NULL);
  } // end if

  return nFuncReturn;
} // end cn46AddStandby

/*! @brief cn46RemoveStandby */
tcn00_Error cn46RemoveStandby
    ( VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  FUNCTION_DBG_MCN00_1("cn46RemoveStandby");

  tcn00_Error nFuncReturn = OK_CN00;

  Tools_DynamicUTF8String sNode  = command->oArguments.getValue(1);
  DBMSrvHSS_Node *        pNode  = NULL;

  // check parameters
  if (sNode.Empty()) {
    nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00, 0, "missing node name");
  } else if (!vcontrol->pHSSNodes->Exists(sNode)) {
    nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00, 0, "node does not exist in cluster");
  } else if (!vcontrol->pHSSNodes->Exists(RTE_ISystem::Instance().GetLocalNodeName())) {
    nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00, 0, "current node is not a hot standby node");
  } else if (sNode.Compare(RTE_ISystem::Instance().GetLocalNodeName()) == 0) {
    nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00, 0, "can not remove current node");
  } // end if

  DBMSrvMsg_Error oMsg;
  if (!vcontrol->pHSSNodes->Remove(sNode, oMsg)) {
    nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(oMsg);
  } // end if

  if (nFuncReturn == OK_CN00) {
    cn90AnswerOK(replyData, replyLen, NULL);
  } // end if

  return nFuncReturn;
} // end cn46RemoveStandby

/*! @brief cn46GetNodes */
tcn00_Error cn46GetNodes
    ( VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  FUNCTION_DBG_MCN00_1("cn46GetNodes");

  tcn00_Error nFuncReturn = OK_CN00;

  cn90AnswerOK(replyData, replyLen, NULL);

  if (vcontrol->pHSSNodes->Exists(RTE_ISystem::Instance().GetLocalNodeName())) {
    DBMSrvMsg_Error         oMsg;
    Tools_DynamicUTF8String sInfo;
    if (vcontrol->pHSSNodes->ListHSSInfo(sInfo, oMsg)) {
      strcat(replyData, sInfo.CharPtr());
      *replyLen = (int) strlen(replyData);
    } else {
      nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oMsg);
    } // end if
  } // end if

  return nFuncReturn;
} // end cn46GetNodes

/*! @brief cn46DBStandby */
tcn00_Error cn46DBStandby
    ( VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  FUNCTION_DBG_MCN00_1("cn46DBStandby");

  tcn00_Error nFuncReturn = OK_CN00;

  Tools_DynamicUTF8String sNode  = command->oArguments.getValue(1);
  DBMSrvHSS_Node *        pNode  = NULL;

  // check parameters
  if (sNode.Empty()) {
    nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00, 0, "missing node name");
  } else if (!vcontrol->pHSSNodes->Exists(sNode)) {
    nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00, 0, "node does not exist in cluster");
  } else if (!vcontrol->pHSSNodes->Exists(RTE_ISystem::Instance().GetLocalNodeName())) {
    nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00, 0, "current node is not a hot standby node");
  } // end if

  // check database state
  if (nFuncReturn == OK_CN00) {
    if (DBMSrvKnl_State::State(vcontrol->dbname) != DBMSrvKnl_State::StateOnline) {
      nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_DBNORUN_CN00);
    } // end if
  } // end if

  // check master state
  if (nFuncReturn == OK_CN00) {
    DBMSrvKnl_Session             oSession;
    DBMSrvMsg_Error               oError;
    cn50DBMUser                   usrDBM    (vcontrol->dbname, cn50DBMUser::getColdUser(vcontrol->dbname) );
    if (oSession.NormalConnect(vcontrol->dbname.asCharp(), usrDBM.getUserName().asCharp(), usrDBM.getClearMasterPwd().asCharp(), oError)) {
      if (oSession.Execute(SQL_CHECKROLE, oError)) {
        DBMSrvKnl_SQLRecord & oRecord = oSession.GetRecordObject();
        if (oRecord.fieldAsChar(1, 0) != HSS_ROLE_MASTER) {
          nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_NOTMASTER_CN00);
        } // end if
      } else {
        nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oError);
      } // end if
      oSession.Release(oError);
    } else {
      nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oError);
    } // end if
  } // end if

  // Execute
  if (nFuncReturn == OK_CN00) {
    DBMSrvMsg_Error oMsg;
    if (!vcontrol->pHSSNodes->Standby(sNode, oMsg)) {
      nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(oMsg);
    } // end if
  } // end if

  if (nFuncReturn == OK_CN00) {
    cn90AnswerOK(replyData, replyLen, NULL);
  } // end if

  return nFuncReturn;
} // end cn46DBStandby

/*! @brief cn46Enable */
tcn00_Error cn46Enable
    ( VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  FUNCTION_DBG_MCN00_1("cn46Enable");

  tcn00_Error nFuncReturn = OK_CN00;
  bool        bExist      = false;

  Tools_DynamicUTF8String sOfficial  = command->oArguments.getValue(HSS_VALUE_NODE);
  Tools_DynamicUTF8String sLibrary   = command->oArguments.getValue(HSS_VALUE_LIB);

  // check parameters
  if (sOfficial.Empty()) {
    sOfficial = RTE_ISystem::Instance().GetLocalNodeName();
  } // end if
  if (sLibrary.Empty()) {
    sLibrary = HSS_LIB_DEFAULT;
  } // end if

  // check parameters
  if (vcontrol->pHSSNodes->Exists(1)) {
    bExist = true;
  } // end if

  // check database state
  if (nFuncReturn == OK_CN00) {
    if (DBMSrvKnl_State::State(vcontrol->dbname) != DBMSrvKnl_State::StateOffline) {
      nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_DBRUN_CN00);
    } // end if
  } // end if

  // set HSS parameters
  if (nFuncReturn == OK_CN00) {
    const char * szNames[]  = { (const char *) KERNELPARAM_OFFICIAL_NODE, 
                                (const char *) KERNELPARAM_HS_STORAGE_DLL,
                                (const char *) KERNELPARAM_SET_VOLUME_LOCK,
                                (const char *) KERNELPARAM_HS_NODE_"001",
                                (const char *) KERNELPARAM_HS_DELAY_TIME_"001"};
    const char * szValues[] = { sOfficial.CharPtr(),
                                sLibrary.CharPtr(),
                                "NO",
                                RTE_ISystem::Instance().GetLocalNodeName(),
                                "0"};

    command->args = "";
    nFuncReturn = cn20CheckedParamPut ( vcontrol,
                                        command,
                                        replyData,
                                        replyLen,
                                        replyLenMax,
                                        bExist ? 3 :5,
                                        szNames,
                                        szValues);
  } // end if

  if (nFuncReturn == OK_CN00) {
    cn90AnswerOK(replyData, replyLen, NULL);
  } // end if

  return nFuncReturn;
} // end cn46Enable

bool cn46IsMaster
    ( tsp00_DbNamec const szDBName )
{
  bool bMaster     = false;
  bool bTemporary  = true;
  DBMSrvKnl_State::StateValue oState = DBMSrvKnl_State::State(szDBName);

  if ((oState == DBMSrvKnl_State::StateOnline) || (oState == DBMSrvKnl_State::StateAdmin)) {
    cn50DBMUser  usrDBM(szDBName, cn50DBMUser::getColdUser(szDBName));
    if (usrDBM.existsOnDisk() ) {
      DBMSrvKnl_Session oSession;
      DBMSrvMsg_Error   oError;
      if (oSession.NormalConnect(szDBName, usrDBM.getUserName(), usrDBM.getClearMasterPwd(), oError)) {

        if (oState == DBMSrvKnl_State::StateOnline) {
          if (oSession.Execute(SQL_CHECKROLE, oError)) {
            DBMSrvKnl_SQLRecord & oRecord = oSession.GetRecordObject();
            if (oRecord.fieldAsChar(1, 0) == HSS_ROLE_MASTER) {
              bMaster = true;
            } // end if
          } // end if
        } else if (oState == DBMSrvKnl_State::StateAdmin) {
          if (oSession.Execute(SQL_GETRESTARTVERSION, oError)) {
            DBMSrvKnl_SQLRecord & oRecord = oSession.GetRecordObject();
            if (oRecord.fieldAsChar(SQL_FIELD_MASTER, 0) == RTE_ISystem::Instance().GetLocalNodeName()) {
              bMaster = true;
            } // end if
          } // end if
        } // end if
        oSession.Release(oError);
      } // end if
    } // end if
  } // end if

  return bMaster;
} // end cn46IsMaster