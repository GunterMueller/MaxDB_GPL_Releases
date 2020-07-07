/*!
  @file           DBMSrvHSS_Nodes.cpp
  @author         Bernd Vorsprach - bernd.vorsprach@sap.com
  @brief          DBMServer to DBMServer Communication - Implementation

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

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#define XPARAM_NAMES_ONLY
#include "SAPDB/KernelCommon/ParameterNames/KernelParam_RunTime.h"
#include "DBM/Srv/Parameters/DBMSrvPar_RTEConfParameter.hpp"

#include "hcn42.h"

#include "SAPDB/DBM/Srv/HSS/DBMSrvHSS_Nodes.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"

#define DBMSRV_CMD_DbExecute          "db_execute"
#define DBMSRV_CMD_ParamRestore       "param_restore"
#define DBMSRV_CMD_ParamDirectPut     "param_directput"

#define CMD_PARAM_RESTORE      DBMSRV_CMD_ParamRestore" 1"

#define CMD_DB_RESTARTSTANDBY  DBMSRV_CMD_DbExecute" restart standby"
#define CMD_DB_INITSTANDBY     DBMSRV_CMD_DbExecute" init standby"

/*!
  @brief   Constructor
 */
DBMSrvHSS_Nodes :: DBMSrvHSS_Nodes
  ( )
{

} // end DBMSrvHSS_Nodes :: DBMSrvHSS_Nodes

/*!
  @brief   Destructor
 */
DBMSrvHSS_Nodes :: ~DBMSrvHSS_Nodes
  ( )
{
  this->UnloadNodes(this->m_oNodes);
} // end DBMSrvHSS_Nodes :: ~DBMSrvHSS_Nodes

/*!
  @brief   public member function
 */
void DBMSrvHSS_Nodes :: SetConnectionData
  ( const Tools_DynamicUTF8String & sDatabase,
    const Tools_DynamicUTF8String & sUser,
    const Tools_DynamicUTF8String & sPassword )
{
  m_sDatabase = sDatabase;
  m_sUser     = sUser;
  m_sPassword = sPassword;
} // end DBMSrvHSS_Nodes :: SetConnectionData

/*!
  @brief   public member function
 */
bool DBMSrvHSS_Nodes :: Create
  (       Tools_DynamicUTF8String & sNode,
          Tools_DynamicUTF8String & sOsUser,
          Tools_DynamicUTF8String & sOsPwd,
          Tools_DynamicUTF8String & sInstallation,
          SAPDB_Int8                nDelayTime,
          SAPDB_Bool                bUser,
          DBMSrvMsg_Error         & oMessageList )
{
  oMessageList.ClearMessageList();

  DBMCli_Node oNode(sNode, oMessageList, false);

  sNode.ToUpper();

  // create database
  if (!oNode.CreateDatabase(sNode, 
                            this->m_sDatabase,
                            this->m_sUser,
                            this->m_sPassword,
                            sOsUser, 
                            sOsPwd, 
                            sInstallation, 
                            oMessageList)) {
    oMessageList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_HSS));
    return false;
  } // end if

  int nNode = this->m_oNodes.size();
  ++nNode;
  
  DBMSrvPar_RTEConfParameter oConfigFile(m_sDatabase.CharPtr(), 
                                0, 
                                RTECONF_MAXNAMELENGTH,
                                RTECONF_MAXSTRINGLENGTH);
 
  DBMSrvPar_RTEConfParameter::StringElement sName[RTECONF_MAXNAMELENGTH + 1];

  oConfigFile.Read(oMessageList);
  if (oMessageList == DBMSrvMsg_Error::DBMSrv_FILEOPEN) {
	oMessageList.ClearMessageList();
  } // end if

  if (oMessageList.IsEmpty()) {
    sprintf((char *) sName, (char *) KERNELPARAM_HS_NODE_"%03d", nNode);
    oConfigFile.SetValue(sName, (DBMSrvPar_RTEConfParameter::String) sNode.CharPtr(), oMessageList);
    if (oMessageList.IsEmpty()) {
      sprintf((char *) sName, (char *) KERNELPARAM_HS_DELAY_TIME_"%03d", nNode);
      oConfigFile.SetValue(sName, nDelayTime, oMessageList);
      if (oMessageList.IsEmpty()) {
        oConfigFile.Write(oMessageList);
      } // end if
    } // end if
  } // end if

  Tools_DynamicUTF8String  sUserPwd;
  sUserPwd.Assign(this->m_sUser).Append(",").Append(this->m_sPassword);
  DBMSrvHSS_Node *         pNode = new DBMSrvHSS_Node(nNode, sNode, m_sDatabase, sUserPwd, oMessageList );
  if (pNode != NULL) {

    // add os user to service
    if (oMessageList.IsEmpty()) {
      if (bUser) {
        Tools_DynamicUTF8String  sRegister;
        sRegister.Assign("db_reg -u ").Append(sOsUser).Append(",").Append(sOsPwd);
        if (!pNode->DBMCli_Session::Execute(sRegister, oMessageList)) {
          oMessageList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_HSS));
        } // end if
      } // end if
    } // end if


    // copy parameters
    if (oMessageList.IsEmpty()) {
      this->CopyParams(pNode, oMessageList);
    } // end if

    // start database
    if (oMessageList.IsEmpty()) {
      if (!pNode->Start(oMessageList)) {
        oMessageList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_HSS));
      } // endif
    } // endif

    // stop database
    if (oMessageList.IsEmpty()) {
      if (!pNode->Stop(oMessageList)) {
        oMessageList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_HSS));
      } // endif
    } // endif

  } // endif

  if (!oMessageList.IsEmpty()) {

    DBMSrvMsg_Error oDummy;
    if (pNode != NULL) {
      pNode->Stop(oDummy);
      pNode->DBMCli_Session::Execute("db_drop", oDummy);
      delete pNode;
    } // end if
    oConfigFile.Read(oDummy);
    if (oDummy.IsEmpty()) {
      sprintf((char *) sName, (char *) KERNELPARAM_HS_NODE_"%03d", nNode);
      oConfigFile.Delete(sName, oDummy);
      sprintf((char *) sName, (char *) KERNELPARAM_HS_DELAY_TIME_"%03d", nNode);
      oConfigFile.Delete(sName, oDummy);
      oConfigFile.Write(oDummy);
    } // end if

    return false;
  } // end if

  if (pNode != NULL) {
    this->m_oNodes.push_back(pNode);
    pNode->SetDelayTime(nDelayTime);
  } // end if

  return true;
} // end DBMSrvHSS_Nodes :: Create

/*!
  @brief   public member function
 */
bool DBMSrvHSS_Nodes :: Remove
  ( const Tools_DynamicUTF8String & sNode,
          DBMSrvMsg_Error         & oMessageList )
{
  oMessageList.ClearMessageList();

  // get the node
  DBMSrvHSS_Node * pNode = GetNode(sNode, oMessageList);
  if (pNode == NULL) {
    oMessageList = DBMSrvMsg_Error(SDBMSG_DBMSRV_HSSNODEUNKNOWN);
    return false;
  } // end if

  // check state of database
  DBMCli_State & oState = pNode->GetState();
  if (!oState.Refresh(oMessageList)) {

    // database does not exist
    oMessageList.ClearMessageList();

  } else {

    if (oState.Value() == DBMCLI_DBSTATE_WARM) {
      oMessageList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_DBRUN));
      oMessageList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_HSS));
      return false;
    } // end if

    // stop database
    if (!pNode->Stop(oMessageList)) {
      oMessageList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_HSS));
      return false;
    } // endif

    // drop database 
    if (!pNode->DropDatabase(this->m_sDatabase, this->m_sUser, this->m_sPassword, oMessageList)) {
      oMessageList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_HSS));
      return false;
    } // end if

  } // end if

  // remove database from parameters
  DBMSrvPar_RTEConfParameter oConfigFile(m_sDatabase.CharPtr(), 
                                0, 
                                RTECONF_MAXNAMELENGTH,
                                RTECONF_MAXSTRINGLENGTH);
 
  DBMSrvPar_RTEConfParameter::StringElement sName[RTECONF_MAXNAMELENGTH + 1];

  oConfigFile.Read(oMessageList);
  if (oMessageList == DBMSrvMsg_Error::DBMSrv_FILEOPEN) {
	oMessageList.ClearMessageList();
  } // end if
  if (oMessageList.IsEmpty()) {
    sprintf((char *) sName, (char *) KERNELPARAM_HS_NODE_"%03d", pNode->GetNumber());
    oConfigFile.Delete(sName, oMessageList);
    if (oMessageList.IsEmpty()) {
      DBMSrvMsg_Error oDummy;
      sprintf((char *) sName, (char *) KERNELPARAM_HS_DELAY_TIME_"%03d", pNode->GetNumber());
      oConfigFile.Delete(sName, oDummy);

      oConfigFile.Write(oMessageList);
    } // end if
  } // end if
  if (!oMessageList.IsEmpty()) {
    return false;
  } // end if

  // remove node from list
  DelNode(sNode, true, this->m_oNodes);

  return true;
} // end DBMSrvHSS_Nodes :: Remove

/*!
  @brief   public member function
 */
bool DBMSrvHSS_Nodes :: Standby
  ( const Tools_DynamicUTF8String & sNode,
          DBMSrvMsg_Error         & oMessageList )
{
  oMessageList.ClearMessageList();

  // get the node
  DBMSrvHSS_Node * pNode = GetNode(sNode, oMessageList);
  if (pNode == NULL) {
    oMessageList = DBMSrvMsg_Error(SDBMSG_DBMSRV_HSSNODEUNKNOWN);
    return false;
  } // end if

  // check state of database
  DBMCli_State & oState = pNode->GetState();
  if (!oState.Refresh(oMessageList)) {
    return false;
  } // end if
  if (oState.Value() != DBMCLI_DBSTATE_OFFLINE && oState.Value() != DBMCLI_DBSTATE_COLD) {
    oMessageList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_DBRUN));
    oMessageList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_HSS));
    return false;
  } // end if

  // stop database
  if (!pNode->Stop(oMessageList)) {
    oMessageList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_HSS));
    return false;
  } // endif

  // copy parameters
  if (!this->CopyParams(pNode, oMessageList)) {
    return false;
  } // end if

  // copy several files ignore errors (maybe source files does not exist etc.)
  DBMSrvMsg_Error oDummy;
  pNode->CopyFile(FGET_BACKHIST_CN42.asCharp(), oDummy); // backup history
  pNode->CopyFile(FGET_BACKMDF_CN42.asCharp(), oDummy);  // backup media history
  pNode->CopyFile(FGET_BACKEBF_CN42.asCharp(), oDummy);  // external backup history
  pNode->CopyFile(FGET_DBMMDF_CN42.asCharp(), oDummy);   // dbm media file
  pNode->CopyFile(FGET_DBMCFG_CN42.asCharp(), oDummy);   // dbm configuration file
  pNode->CopyFile(FGET_USER_CN42.asCharp(), oDummy);     // user profile container

  // start database
  if (!pNode->Start(oMessageList)) {
    oMessageList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_HSS));
    return false;
  } // endif

  Tools_DynamicUTF8String sAnswer;

  // restart standby
  if (pNode->Execute(CMD_DB_RESTARTSTANDBY, sAnswer, oMessageList)) {
    return true;
  } // end if

  // stop database
  if (!pNode->Stop(oMessageList)) {
    oMessageList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_HSS));
    return false;
  } // endif

  // start database
  if (!pNode->Start(oMessageList)) {
    oMessageList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_HSS));
    return false;
  } // endif

  // init standby
  if (!pNode->Execute(CMD_DB_INITSTANDBY, sAnswer, oMessageList)) {
    oMessageList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_HSS));
    return false;
  } // end if

  // restart standby
  if (!pNode->Execute(CMD_DB_RESTARTSTANDBY, sAnswer, oMessageList)) {
    oMessageList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_HSS));
    return false;
  } // end if

  return true;
} // end DBMSrvHSS_Nodes :: Standby

/*!
  @brief   public member function
 */
bool DBMSrvHSS_Nodes :: Execute
  ( const Tools_DynamicUTF8String & sNode,
    const Tools_DynamicUTF8String & sCommand,
          DBMSrvMsg_Error         & oMessageList )
{
  Tools_DynamicUTF8String sAnswer;

  return this->Execute(sNode, sCommand, sAnswer, oMessageList);
} // end DBMSrvHSS_Nodes :: Execute

/*!
  @brief   public member function
 */
bool DBMSrvHSS_Nodes :: Execute
  (       SAPDB_Int                 nNode,
    const Tools_DynamicUTF8String & sCommand,
          DBMSrvMsg_Error         & oMessageList )
{
  Tools_DynamicUTF8String sAnswer;

  return this->Execute(nNode, sCommand, sAnswer, oMessageList);
} // end DBMSrvHSS_Nodes :: Execute

/*!
  @brief   public member function
 */
bool DBMSrvHSS_Nodes :: Execute
  ( const Tools_DynamicUTF8String & sNode,
    const Tools_DynamicUTF8String & sCommand,
          Tools_DynamicUTF8String & sAnswer,
          DBMSrvMsg_Error         & oMessageList )
{
  if (this->m_oNodes.empty()) {
    if (!this->LoadNodes(oMessageList)) {
      oMessageList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_HSS));
      return false;
    } // end if
  } // end if

  DBMSrvHSS_Node * pNode = this->FindNode(sNode, this->m_oNodes);

  return this->Execute(pNode, sCommand, sAnswer, oMessageList);
} // end DBMSrvHSS_Nodes :: Execute

/*!
  @brief   public member function
 */
bool DBMSrvHSS_Nodes :: Execute
  ( SAPDB_Int                       nNode,
    const Tools_DynamicUTF8String & sCommand,
          Tools_DynamicUTF8String & sAnswer,
          DBMSrvMsg_Error         & oMessageList )
{
  if (this->m_oNodes.empty()) {
    if (!this->LoadNodes(oMessageList)) {
      oMessageList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_HSS));
      return false;
    } // end if
  } // end if

  DBMSrvHSS_Node * pNode = this->FindNode(nNode, this->m_oNodes);

  return this->Execute(pNode, sCommand, sAnswer, oMessageList);
} // end DBMSrvHSS_Nodes :: Execute

bool DBMSrvHSS_Nodes::ExecuteOnAllOtherNodes(const char * command, DBMSrv_Reply & reply)
{
    bool rc=true;
    DBMSrvMsg_Error oMsg;

    Tools_DynamicUTF8String currentNode=RTE_ISystem::Instance().GetLocalNodeName();

    if(Exists(currentNode))    //is this database part of a HSS?
    {
        currentNode.ToUpper();

        for(NodeList::iterator i=m_oNodes.begin(); i!=m_oNodes.end(); i++)
        {
            if(0!=currentNode.Compare((*i)->GetName())) //not current node
            {
                reply.appendStartOfHSSNodeAnswerFor((*i)->GetName());

                oMsg.ClearMessageList();
                if(!(*i)->Execute(command,oMsg))
                {
                    rc=false;
                    reply.appendMessageListForHSSNode(oMsg);
                }
                else
                    reply.appendOKForHSSNode();
            }
        }
    }

    return rc;
}

/*!
  @brief   public member function
 */
bool DBMSrvHSS_Nodes :: Execute
  (       DBMSrvHSS_Node          * pNode,
    const Tools_DynamicUTF8String & sCommand,
          Tools_DynamicUTF8String & sAnswer,
          DBMSrvMsg_Error         & oMessageList )
{
  if (pNode == NULL) {
    oMessageList = DBMSrvMsg_Error(SDBMSG_DBMSRV_HSSNODEUNKNOWN);
    oMessageList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_HSS));
    return false;
  } // end if

  if (!pNode->Execute(sCommand, sAnswer, oMessageList)) {
    oMessageList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_HSS));
    return false;
  } // end if

  return true;
} // end DBMSrvHSS_Nodes :: Execute

/*!
  @brief   private member function
 */
bool DBMSrvHSS_Nodes :: LoadNodes
  ( DBMSrvMsg_Error & oMessageList )
{
  oMessageList.ClearMessageList();

  DBMSrvPar_RTEConfParameter::StringElement sValue[RTECONF_MAXSTRINGLENGTH + 1];
  DBMSrvPar_RTEConfParameter::Integer       nValue;
  DBMSrvPar_RTEConfParameter::Name          sName;
  DBMSrvPar_RTEConfParameter::StringElement sDelayName[RTECONF_MAXNAMELENGTH + 1];
  DBMSrvPar_RTEConfParameter::Type          oType;

  DBMSrvPar_RTEConfParameter oConfigFile(m_sDatabase.CharPtr(), 
                                0, 
                                RTECONF_MAXNAMELENGTH,
                                RTECONF_MAXSTRINGLENGTH);
 
  SAPDB_Bool                  bEnd       = false;
  SAPDB_Int                   nNumber    = 0;
  DBMSrvHSS_Node            * pNode      = NULL;
  Tools_DynamicUTF8String     sUserPwd   = "";
  Tools_DynamicUTF8String     sNameToInt;

  NodeList                    oOldNodes;

  NodeList::iterator oIterator = this->m_oNodes.begin();
  while (oIterator != this->m_oNodes.end()) {
    oOldNodes.push_back(*oIterator);
    ++oIterator;
  } // end while
  this->m_oNodes.clear();

  oConfigFile.Read( oMessageList);
  if (oMessageList == DBMSrvMsg_Error::DBMSrv_FILEOPEN) {
	oMessageList.ClearMessageList();
  } // end if
  if (oMessageList.IsEmpty()) {
    if (oConfigFile.GetValue ((DBMSrvPar_RTEConfParameter::Name) KERNELPARAM_HS_STORAGE_DLL, sValue, oMessageList)) {
      this->m_sStorageDLL = sValue;
      if (oConfigFile.GetValue ((DBMSrvPar_RTEConfParameter::Name) KERNELPARAM_OFFICIAL_NODE, sValue, oMessageList)) {
        this->m_sOfficialNode = sValue;
        RTEConf_ParameterIterator   oIterator  = oConfigFile.GetIterator();
        do {
          if (oIterator.Next(sName, oType, bEnd, oMessageList)) {
            if (STRNCMP_UTF8(sName, KERNELPARAM_HS_NODE_, STRLEN_UTF8(KERNELPARAM_HS_NODE_)) == 0) {
              if (oConfigFile.GetValue (sName, sValue, oMessageList)) {
                sNameToInt = sName;
                nNumber = atoi(sNameToInt.SubStr((Tools_DynamicUTF8String::BasisElementIndex) STRLEN_UTF8(KERNELPARAM_HS_NODE_)).CharPtr());
                // search node
                pNode = FindNode(nNumber, oOldNodes);
                
                // create node
                if (pNode == NULL) {
                  sUserPwd.Assign(this->m_sUser).Append(",").Append(this->m_sPassword);
                  pNode = new DBMSrvHSS_Node(nNumber, sValue, m_sDatabase, sUserPwd, oMessageList );
                } else {
                  DelNode(sValue, false, oOldNodes);
                } // end if
                if (pNode != NULL) {
                  this->m_oNodes.push_back(pNode);
                  // read and set delay time
                  sprintf((char *) sDelayName, (char *) KERNELPARAM_HS_DELAY_TIME_"%03d", nNumber);
                  if (oConfigFile.GetValue (sDelayName, nValue, oMessageList)) {
                    pNode->SetDelayTime(nValue);
                  } else {
                    pNode->SetDelayTime(0);
                    oMessageList.ClearMessageList();
                  } // end if
                } // end if
              } // end if
            } // end if
          } // end if
        }  while ( (oMessageList.IsEmpty()) && !bEnd);
      } // end if
    } // end if
  } // end if

  if (!oMessageList.IsEmpty()) {
    this->UnloadNodes(this->m_oNodes);
  } // end if

  UnloadNodes(oOldNodes);

  return oMessageList.IsEmpty();
} // end DBMSrvHSS_Nodes :: LoadNodes

/*!
  @brief   private member function
 */
void DBMSrvHSS_Nodes :: UnloadNodes
  ( NodeList                & oNodes)
{
  NodeList::iterator  oIterator = oNodes.begin();
  DBMSrvHSS_Node    * pNode     = NULL;

  while (oIterator != oNodes.end()) {
    pNode = (*oIterator);
    ++oIterator;
    if (pNode != NULL) {
      delete pNode;
    } // end if
  } // end while

  oNodes.clear();
} // end DBMSrvHSS_Nodes :: UnloadNodes

/*! @brief   public member function */
DBMSrvHSS_Node * DBMSrvHSS_Nodes :: GetNode
  ( const Tools_DynamicUTF8String & sNode,
    DBMSrvMsg_Error               & oMessageList )
{
  if (this->m_oNodes.empty()) {
    if (!this->LoadNodes(oMessageList)) {
      oMessageList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_HSS));
      return NULL;
    } // end if
  } // end if

  return this->FindNode(sNode, this->m_oNodes);
} // end DBMSrvHSS_Nodes :: GetNode

/*! @brief   public member function */
DBMSrvHSS_Node * DBMSrvHSS_Nodes :: GetNode
  ( SAPDB_Int                       nNode,
    DBMSrvMsg_Error               & oMessageList )
{
  if (this->m_oNodes.empty()) {
    if (!this->LoadNodes(oMessageList)) {
      oMessageList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_HSS));
    } // end if
  } // end if

  return this->FindNode(nNode, this->m_oNodes);
} // end DBMSrvHSS_Nodes :: GetNode

/*!
  @brief   private member function
 */
DBMSrvHSS_Node * DBMSrvHSS_Nodes :: FindNode
  ( SAPDB_Int                 nNumber,
    const NodeList          & oNodes )
{
  NodeList::const_iterator oIterator = oNodes.begin();

  while (oIterator != oNodes.end()) {
    if ((*oIterator)->GetNumber() == nNumber) {
      return *oIterator;
    } // end if
    ++oIterator;
  } // end while

  return NULL;
} // end DBMSrvHSS_Nodes :: FindNode

/*!
  @brief   private member function
 */
DBMSrvHSS_Node * DBMSrvHSS_Nodes :: FindNode
  ( Tools_DynamicUTF8String   sName,
    const NodeList          & oNodes )
{
  NodeList::const_iterator oIterator = oNodes.begin();

  while (oIterator != oNodes.end()) {
    if (sName.ToUpper().Compare((*oIterator)->GetName()) == 0) {
      return *oIterator;
    } // end if
    ++oIterator;
  } // end while


  // maybe a number
  unsigned int nIndex = 0;
  for (nIndex = 0; nIndex < sName.Length(); ++nIndex) {
    if (!isdigit(sName[nIndex])) {
      return NULL;
    } // end if
  } // end for

  return FindNode(atoi(sName.CharPtr()), oNodes);
} // end DBMSrvHSS_Nodes :: FindNode

/*!
  @brief   private member function
 */
void DBMSrvHSS_Nodes :: DelNode
  ( Tools_DynamicUTF8String   sName,
    bool                      bDelete,
    NodeList                & oNodes )
{
  NodeList::iterator oIterator = oNodes.begin();

  while (oIterator != oNodes.end()) {
    if (sName.ToUpper().Compare((*oIterator)->GetName()) == 0) {
      (*oIterator)->Disconnect();
      if (bDelete) {
        delete (*oIterator);
      } // end if
      oNodes.erase(oIterator);
      return;
    } // end if
    ++oIterator;
  } // end while

    // maybe a number
  unsigned int nIndex = 0;
  for (nIndex = 0; nIndex < sName.Length(); ++nIndex) {
    if (!isdigit(sName[nIndex])) {
      return;
    } // end if
  } // end for

  SAPDB_Int nNumber = atoi(sName.CharPtr());
  oIterator = oNodes.begin();

  while (oIterator != oNodes.end()) {
    if ((*oIterator)->GetNumber() == nNumber) {
      (*oIterator)->Disconnect();
      if (bDelete) {
        delete (*oIterator);
      } // end if
      oNodes.erase(oIterator);
      return;
    } // end if
    ++oIterator;
  } // end while

} // end DBMSrvHSS_Nodes :: DelNode

/*! @brief public member function */
bool DBMSrvHSS_Nodes :: ListHSSInfo
    ( Tools_DynamicUTF8String & sInfo,
      DBMSrvMsg_Error         & oMessageList )
{
  Tools_DynamicUTF8String          sDelay;
  DBMSrvPar_RTEConfParameter::StringElement sNodeName[RTECONF_MAXNAMELENGTH + 1];
  SAPDB_Int                        nNumber = 0;

  if (!LoadNodes(oMessageList)) {
    return false;
  } // end if
  
  sInfo.Assign(KERNELPARAM_HS_STORAGE_DLL)
       .Append("\t")
       .Append(this->m_sStorageDLL)
       .Append("\n");

  sInfo.Append(KERNELPARAM_OFFICIAL_NODE)
       .Append("\t")
       .Append(this->m_sOfficialNode)
       .Append("\n");

  sInfo.Append("CURRENT_NODE")
       .Append("\t")
       .Append(RTE_ISystem::Instance().GetLocalNodeName())
       .Append("\n");

  NodeList::iterator oIterator = this->m_oNodes.begin();

  while (oIterator != this->m_oNodes.end()) {
    ++nNumber;
    sprintf((char *) sNodeName, (char *) KERNELPARAM_HS_NODE_"%03d", nNumber);
    sDelay.ConvertFromInt((*oIterator)->GetDelayTime());
    sInfo.Append(sNodeName)
         .Append("\t")
         .Append((*oIterator)->GetName())
         .Append((((40 - (int) strlen((*oIterator)->GetName()))) > 0) ? 41 - (int)strlen((*oIterator)->GetName()) : 1, ' ')
         .Append("\t")
         .Append(sDelay)
         .Append("\n");
    ++oIterator;
  } // end while

  return true;
} // end DBMSrvHSS_Nodes :: ListHSSInfo

/*!
  @brief public member function
 */
bool DBMSrvHSS_Nodes :: CopyParams
  ( DBMSrvHSS_Node  * pNode,
    DBMSrvMsg_Error & oMessageList )
{
  Msg_List     oTmpMsg;
  Tools_DynamicUTF8String  sCommand;
  Tools_DynamicUTF8String  sAnswer;
  DBMSrvMsg_Error          oDummy;

  // load and copy parameters

  // send minimal parameter set
  sCommand.Assign(DBMSRV_CMD_ParamDirectPut)
          .Append(" ")
          .Append(KERNELPARAM_HS_NODE_"001")
          .Append(" ")
          .Append(RTE_ISystem::Instance().GetLocalNodeName());
  if (!pNode->Execute(sCommand, sAnswer, oMessageList)) {
    oMessageList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_HSS));
    pNode->Execute(CMD_PARAM_RESTORE, sAnswer, oDummy);
    return false;
  } // end if

  sCommand.Assign(DBMSRV_CMD_ParamDirectPut)
          .Append(" ")
          .Append(KERNELPARAM_HS_STORAGE_DLL)
          .Append(" ")
          .Append(m_sStorageDLL);
  if (!pNode->Execute(sCommand, sAnswer, oMessageList)) {
    oMessageList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_HSS));
    pNode->Execute(CMD_PARAM_RESTORE, sAnswer, oDummy);
    return false;
  } // end if

  sCommand.Assign(DBMSRV_CMD_ParamDirectPut)
          .Append(" ")
          .Append(KERNELPARAM_OFFICIAL_NODE)
          .Append(" ")
          .Append(m_sOfficialNode);
  if (!pNode->Execute(sCommand, sAnswer, oMessageList)) {
    oMessageList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_HSS));
    pNode->Execute(CMD_PARAM_RESTORE, sAnswer, oDummy);
    return false;
  } // end if

  // send hss_copy file node=1 name=PARAM
  if (!pNode->CopyFile(FGET_PARAM_CN42.asCharp(), oMessageList)) {
    oMessageList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_HSS));
    pNode->Execute(CMD_PARAM_RESTORE, sAnswer, oDummy);
    return false;
  } // end if

  return true;
} // end DBMSrvHSS_Nodes :: CopyParams

/*!
  @brief public member function
 */
bool DBMSrvHSS_Nodes :: AddVolumeToCluster
    ( const Tools_DynamicUTF8String & sAddVolume,
            DBMSrv_Reply            & reply       )
{
  bool                         bReturn      = true;    
  DBMSrvMsg_Error              oMsg;
  NodeList::iterator           oIterator    = m_oNodes.begin();
  Tools_DynamicUTF8String      sCurrentNode = RTE_ISystem::Instance().GetLocalNodeName();
  const Msg_List * pMsg         = NULL;
  Tools_DynamicUTF8String      sNumber;

  while (oIterator != m_oNodes.end()) {
    if (sCurrentNode.ToUpper().Compare((*oIterator)->GetName()) != 0) {
      reply.appendStartOfHSSNodeAnswerFor((*oIterator)->GetName());

      oMsg.ClearMessageList();
      if (!(*oIterator)->AddVolume(sAddVolume, oMsg)) {
        reply.appendMessageListForHSSNode(oMsg);
        
        bReturn = false;
      } else {
          reply.appendOKForHSSNode();
      } // end if
    } // end if
    ++oIterator;
  } // end while

  return bReturn;
} // end DBMSrvHSS_Nodes :: AddVolumeToCluster

bool DBMSrvHSS_Nodes::isHSS()
{
    DBMSrvHSS_Nodes         * nodes=cn00DBMServerData::vcontrol()->pHSSNodes;
    Tools_DynamicUTF8String   currentNode=RTE_ISystem::Instance().GetLocalNodeName();

    if(0==nodes) //use a local object (tempNodes) only if, global one is not there to avoid multiple reading of the parameter file, if possible
    {
        DBMSrvHSS_Nodes tempNodes;
        return tempNodes.Exists(currentNode);    //is this database part of a HSS?
    }
    else
        return nodes->Exists(currentNode);    //is this database part of a HSS?
}
