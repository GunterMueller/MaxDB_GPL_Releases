/*!
  @file           DBMSrvKnl_Session.cpp
  @author         Bernd Vorsprach - bernd.vorsprach@sap.com
  @brief          DBMServer to Kernel Communication - Implementation

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
#include "ToolsCommon/Parsers/Utilities/ToolsParsersUtil_IdentifierCheck.hpp"

#include "heo02.h"
#include "heo03.h"
#include "hsp02.h"

#include "PacketInterface/PIn_Packet.h"
#include "Messages/Msg_List.hpp"
#include "PacketInterface/PIn_SpecialParts.h"

#include "DBM/Srv/KernelAccess/DBMSrvKnl_Session.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_RTEError.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_SQLError.hpp"

#include "RunTime/System/RTESys_Time.h"


#define CMD_CONNECT_P1  "CONNECT \""
#define CMD_CONNECT_P2  "\" IDENTIFIED by :PW TIMEOUT 0"
#define CMD_CONNECT_P3  " NO TEMP FILE"
#define CMD_RELEASE     "COMMIT WORK RELEASE"

// text for each session type
const char* const DBMSrvKnl_Session::m_TextTasktypeUnknown("Unknown");
const char* const DBMSrvKnl_Session::m_TextTasktypeUser("User");
const char* const DBMSrvKnl_Session::m_TextTasktypeEvent("Event");

const char* DBMSrvKnl_Session::getTextForTasktype(TaskType taskType) {
    switch( taskType ) {
        case taskUnknown:
            return m_TextTasktypeUnknown;
            break;
        case taskNormal:
            return m_TextTasktypeUser;
            break;
        case taskEvent:
            return m_TextTasktypeEvent;         
            break;
        default:
            return "illegal";
            break;
    }
}


/*-----------------------------------------------------------------------------*/
/*! @brief constructor */
DBMSrvKnl_Session :: DBMSrvKnl_Session
    ( )
{
  this->Reset();
} // end DBMSrvKnl_Session :: DBMSrvKnl_Session

/*-----------------------------------------------------------------------------*/
/*! @brief destructor */
DBMSrvKnl_Session :: ~DBMSrvKnl_Session
    ( )
{
  DBMSrvMsg_Error oLocalMsgList;
  Release(oLocalMsgList);
} // end DBMSrvKnl_Session :: ~DBMSrvKnl_Session

/*-----------------------------------------------------------------------------*/
/*! @brief public member */
bool DBMSrvKnl_Session :: NormalConnect
    ( const char      * szDB,
      const char      * szUserIn,
      const char      * szPassword,
      DBMSrvMsg_Error & oMsgList,
      bool              bWithNoTempFiles )
{
    bool rc=true;

  // check user and password
  if ((szUserIn == NULL) || (szPassword == NULL) || (strlen(szUserIn) == 0) || (strlen(szPassword) == 0)) {
    oMsgList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_MISSDBUSRPWD));
    oMsgList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_PARAM));
    return false;
  } // end if

  oState.SetDatabase(szDB);

  this->bNoTempFiles = bWithNoTempFiles;

  if (!bWithNoTempFiles) {
    bool         bLogFull       = false;
    bool         bDataFull      = false;

    oState.IsFull(bLogFull, bDataFull);
    if (!oState.DBMMsg().IsEmpty()) {
      oMsgList = oState.DBMMsg();
      return false;
    } else if (bDataFull) {
      // database full error
      oMsgList = DBMSrvMsg_Error(SDBMSG_DBMSRV_DATAFULL);
      return false;
    } // end if
  } // end if
  this->bAdminConnect = (oState.State() != DBMSrvKnl_State::StateOnline);

  if (this->Connect(DBMSrvKnl_Session::taskNormal, szDB, oMsgList )) {
    if(!this->ConnectUser(szUserIn, szPassword, bWithNoTempFiles, oMsgList)) {
      DBMSrvMsg_Error oErrorToIgnore;
      this->Release(oErrorToIgnore);
      rc=false;
    } else {
      m_Connected=true;
      m_TaskType = DBMSrvKnl_Session::taskNormal;
      szDatabase.rawAssign(szDB);
    } // end if
  } // end if
  else
      rc=false;

  return rc;
} // end DBMSrvKnl_Session :: NormalConnect

/*-----------------------------------------------------------------------------*/
/*! @brief public member */
bool DBMSrvKnl_Session :: EventConnect
    ( const char      * szDB,
      DBMSrvMsg_Error & oMsgList )
{
  if (this->Connect(DBMSrvKnl_Session::taskEvent, szDB, oMsgList)) {
    m_Connected=true;
    m_TaskType = DBMSrvKnl_Session::taskEvent;
    szDatabase.rawAssign(szDB);

    return true;
  } // end if
  else
    return false;
} // end DBMSrvKnl_Session :: EventConnect

/*-----------------------------------------------------------------------------*/
/*! @brief public member */
bool DBMSrvKnl_Session :: Release
    ( DBMSrvMsg_Error & oMsgList,
      bool              keepConnectInfo,
      bool              sendReleaseCmdOverConnection)
{
    bool rc=true;

  if (sendReleaseCmdOverConnection && this->isConnected()) {
    if (!this->bAdminConnect && m_TaskType!=DBMSrvKnl_Session::taskEvent ) {
      // send release command
      // create request packet
      PIn_RequestPacket  oPacket(this->pPacket, this->nPacketSize, this->nUnicode);
      // initialise request packet
      oPacket.InitVersion(csp_comp_db_manager, NULL);
      // create segement
      PIn_RequestSegment oSegment = oPacket.AddSegment(sp1m_dbs, sp1sm_session_sqlmode);
      oSegment.SetProducer(sp1pr_internal_cmd);
      // create command part
      PIn_Part oCommandPart = oSegment.AddPart(sp1pk_command);
      // fill command part
      // fill command part
      oCommandPart.AddArgument();
      if (this->nUnicode) {
        oCommandPart.AddDataUTF8ToUCS2(CMD_RELEASE, (int) strlen(CMD_RELEASE));
      } else {
        oCommandPart.AddData(CMD_RELEASE, (int) strlen(CMD_RELEASE));
      } // end if
      // close part
      oSegment.ClosePart(oCommandPart);
      // close segment
      oPacket.CloseSegment(oSegment);
      // send request to kernel

      bool connectionBroken=false;

      if (!Request(oPacket.Length(), connectionBroken, oMsgList) ||
          !Receive(oMsgList))
      {
          if(!connectionBroken)
            rc=false;
      } // end if
    } // end if
  } // end if

  if(m_SessionHandleIsValid)
      sqlarelease (this->nSessionHandle);

  this->Reset(keepConnectInfo);

  return rc;
} // end DBMSrvKnl_Session :: Release

/*-----------------------------------------------------------------------------*/
/*! @brief private member */
bool DBMSrvKnl_Session :: BreakSession
    ( )
{
  this->Cancel();
  sqlarelease (this->nSessionHandle);
  this->Reset(true); //allow automatic reconnect attempts
  return true;
} // end DBMSrvKnl_Session :: BreakSession

/*-----------------------------------------------------------------------------*/

bool DBMSrvKnl_Session :: tryImplicitReconnect(
    DBMSrvMsg_Error & error )
{
    bool rc=false;

    if( implicitReconnectFlagIsSet() &&               //we are allowed to reconnect ...
        (!this->isConnected() ||                      //a previous connection was closed actively (e.g. by db_offline)
         (this->bAdminConnect || this->bAutoCommit))) //a previous connection was open but in mode ADMIN or session using auto commit
    {
        DBMSrvMsg_Error  msgListImplicitConnect;

        if(this->Release(msgListImplicitConnect, true, false)) //keep connection info (for following reconnect) but do not send a release command over the already dead session
        {
            RTESys_Timestamp time;

            RTESys_BuildTimeStamp(RTESys_Time(), time);
            m_ImplicitReconnectIsAllowed[m_ImplicitReconnectType]=false; //this flag is valid for just one request, so this request was the one -> reset the flag

            if(NormalConnect(this->szDatabase, this->szPwd, this->szUser, msgListImplicitConnect, this->bNoTempFiles))
            {
                for(int i=0; i<DBMSrvKnl_Session::noSubsession; i++)
                    if(i!=m_ImplicitReconnectType && !m_ImplicitReconnectEndedSubsession[i]) //update flags of all other subsession types
                    {
                        m_ImplicitReconnectEndedSubsession[i]=true;

                        m_ReconnectMessage[i].Overrule(DBMSrvMsg_Info(
                                                            error,
                                                            SDBMSG_DBMSRV_IMPLICITRECONNECT,
                                                            Msg_Arg(SDBMSGTAG_DBMSRV_IMPLICITRECONNECT__SESSIONTYPE, getTextForTasktype(m_TaskType)),
	                                                        Msg_Arg(SDBMSGTAG_DBMSRV_IMPLICITRECONNECT__DATETIME, time)));
                    }

                return true;
            }
            //else //TODO add information of msgListImplicitConnect as info to error
        }
        //else //TODO add information from Release as info to error
    }

    return rc;
}

/*! @brief public member */
bool DBMSrvKnl_Session :: Request
  ( const char      * szCmd,
    DBMSrvMsg_Error & oMsgList,
    bool              useImplicitReconnectFlag)
{
    bool requestSend=false;
    int  triesLeft=1;
    
    if(implicitReconnectFlagIsSet() &&
       useImplicitReconnectFlag)
    {
        triesLeft=2;
    }

    if (!this->isConnected())
    {
        DBMSrvMsg_Error errorNotConnected(SDBMSG_DBMSRV_NODBSESSION);

        if(!useImplicitReconnectFlag || !tryImplicitReconnect(errorNotConnected))
        {
            oMsgList = errorNotConnected;
            return false;
        }
    }

    while(!requestSend && 0<triesLeft)
    {
        this->bAllowedInDbFull = AllowedInDbFull(szCmd);

        // create request packet
        PIn_RequestPacket  oPacket(this->pPacket, this->nPacketSize, this->nUnicode);
        // initialise request packet
        oPacket.InitVersion(csp_comp_db_manager, NULL);
        // create segement
        PIn_RequestSegment oSegment = oPacket.AddSegment(sp1m_dbs, sp1sm_session_sqlmode);
        oSegment.SetProducer(sp1pr_internal_cmd);

        oSegment.GetRawHeader()->sp1c_with_info          () = true;
        oSegment.GetRawHeader()->sp1c_commit_immediately () = this->bAutoCommit;

        // create command part
        PIn_Part oCommandPart = oSegment.AddPart(sp1pk_command);
        // fill command part
        oCommandPart.AddArgument();

        if (this->nUnicode) {
            oCommandPart.AddDataUTF8ToUCS2(szCmd, (int) strlen(szCmd));
        } else {
            oCommandPart.AddData(szCmd, (int) strlen(szCmd));
        } // end if

        // close part
        oSegment.ClosePart(oCommandPart);

        // mass cmd handling in case of fetch
        if (strncmp("FETCH ", szCmd, 6) == 0) {
            oSegment.GetRawHeader()->sp1c_mass_cmd           () = true;
            // create result count part
            PIn_Part oResultCountPart = oSegment.AddPart(sp1pk_resultcount);

            // add number
            unsigned char oFixed[7] = {0x00,0xC5,0x32,0x76,0x70,0x00,0x00};
            oResultCountPart.AddArgument(&oFixed[0], sizeof(oFixed));

            // close part
            oSegment.ClosePart(oResultCountPart);
        } // end if

        // close segment
        oPacket.CloseSegment(oSegment);

        bool            connectionBroken=false;
        DBMSrvMsg_Error msgListRequest;

        // send request to kernel
        if (!Request(oPacket.Length(), connectionBroken, msgListRequest))
        {
            if(!connectionBroken || 2>triesLeft || !tryImplicitReconnect(msgListRequest))
            {
                oMsgList.Overrule(msgListRequest);
                return false;
            }
        }
        else
        {
            requestSend=true;

            if(m_ImplicitReconnectType<DBMSrvKnl_Session::noSubsession)
                m_ImplicitReconnectIsAllowed[m_ImplicitReconnectType]=false; //this flag is valid for just one request, so this request was the one -> reset the flag
        }

        triesLeft--;
    }

    return true;
} // end DBMSrvKnl_Session :: Request

/*-----------------------------------------------------------------------------*/
/*! @brief public member */
bool DBMSrvKnl_Session :: EventRequest
  ( DBMSrvMsg_Error & oMsgList )
{
  if (!this->isConnected()) {
    oMsgList = DBMSrvMsg_Error(SDBMSG_DBMSRV_NODBSESSION);
    return false;
  } // end if

  // create request packet
  PIn_RequestPacket  oPacket(this->pPacket, this->nPacketSize, this->nUnicode);
  // initialise request packet
  oPacket.InitVersion(csp_comp_db_manager, NULL);
  // create segement
  PIn_RequestSegment oSegment = oPacket.AddSegment(sp1m_wait_for_event, sp1sm_session_sqlmode);

//  oSegment.SetProducer(sp1pr_internal_cmd);

//  oSegment.GetRawHeader()->sp1c_with_info          () = true;
//  oSegment.GetRawHeader()->sp1c_commit_immediately () = this->bAutoCommit;

  // create command part
  PIn_Part oCommandPart = oSegment.AddPart(sp1pk_command);

  // close part
  oSegment.ClosePart(oCommandPart);

  // close segment
  oPacket.CloseSegment(oSegment);

  bool connectionBroken=false;

  // send request to kernel
  if (!Request(oPacket.Length(), connectionBroken, oMsgList)) {
    return false;
  } // end if

  return true;
} // end DBMSrvKnl_Session :: EventRequest

/*-----------------------------------------------------------------------------*/
/*! @brief public member */
bool DBMSrvKnl_Session ::  Receive
  ( ResultType      & oResultType,
    DBMSrvMsg_Error & oMsgList )
{
  if (!this->isConnected()) {
    oMsgList = DBMSrvMsg_Error(SDBMSG_DBMSRV_NODBSESSION);
    return false;
  } // end if

  oResultType = DBMSrvKnl_Session::resultUnknown;

  // receive answer from kernel
  if (!Receive(oMsgList)) {
    return false;
  } // end if

  // create answer packet
  PIn_ReplyPacket  oPacket(this->pReceivePacket);

  // read data
  PIn_ReplySegment oSegment     = oPacket.GetFirstSegment ();

  if (oSegment.ErrorCode() == 0) {

    // decide on answer the next steps
    if (oSegment.GetRawHeader()->sp1s_no_of_parts() != 0) {
      PIn_Part oData        = oSegment.FindPart(sp1pk_data);
      PIn_Part oShortInfo   = oSegment.FindPart(sp1pk_shortinfo);
      PIn_Part oResultCount = oSegment.FindPart(sp1pk_resultcount);
      PIn_Part oResultTable = oSegment.FindPart(sp1pk_resulttablename);
      PIn_Part oMsgListPart = oSegment.FindPart(sp1pk_message_list);

      if (oShortInfo.IsValid() && oResultCount.IsValid() && oResultTable.IsValid()) {
        bool bEnd;
        if (!this->oRecord.getFirstRecord(bEnd, oMsgList)) {
          return false;
        } // end if
        oResultType = DBMSrvKnl_Session::resultSQL;
      } else if (oShortInfo.IsValid() && oData.IsValid()) {
        this->oRecord.RefreshRecordData(DBMSrvKnl_SQLRecord::resetAll);
        oResultType = DBMSrvKnl_Session::resultUtility;
      } else if (oMsgListPart.IsValid()) {
        if (KernelMessage().Type() != Msg_List::Info) {
          oResultType = DBMSrvKnl_Session::resultError;
          this->ReadErrorText(oSegment, oMsgList);
          return false;
        } else {
          oResultType = DBMSrvKnl_Session::resultMessage;
        } // end if
      } else if (oResultCount.IsValid()) {
        // result count only (answer for insert or update)
        oResultType = DBMSrvKnl_Session::resultNothing;
      } // end if

    } else {
      oResultType = DBMSrvKnl_Session::resultNothing;
    } // end if

  } else {
    this->ReadErrorText(oSegment, oMsgList);
    oResultType = DBMSrvKnl_Session::resultError;
    return false;
  } // end if

  return true;
} // end DBMSrvKnl_Session :: Receive

/*-----------------------------------------------------------------------------*/
/*! @brief public member */
bool DBMSrvKnl_Session ::  EventReceive
  ( void * const      pEventBuffer, 
    size_t            nBufferSize,
    DBMSrvMsg_Error & oMsgList)
{
  if (!this->isConnected()) {
    oMsgList = DBMSrvMsg_Error(SDBMSG_DBMSRV_NODBSESSION);
    return false;
  } // end if

  // receive answer from kernel
  if (!Receive(oMsgList)) {
    return false;
  } // end if

  // create answer packet
  PIn_ReplyPacket  oPacket(this->pReceivePacket);

  // read data
  PIn_ReplySegment oSegment     = oPacket.GetFirstSegment ();

  if (oSegment.ErrorCode() == 0) {

    // decide on answer the next steps
    if (oSegment.GetRawHeader()->sp1s_no_of_parts() != 0) {
      PIn_Part oData        = oSegment.FindPart(sp1pk_data);
      if (oData.IsValid()) {
        SAPDB_memcpy(pEventBuffer, oData.GetReadData(1), nBufferSize);
      } // end if
    } // end if

  } else {
    this->ReadErrorText(oSegment, oMsgList);
    return false;
  } // end if

  return true;
} // end DBMSrvKnl_Session :: Receive

/*-----------------------------------------------------------------------------*/
/*! @brief public member */
bool DBMSrvKnl_Session :: Execute
  ( const char      * szCmd, 
    DBMSrvMsg_Error & oMsgList )
{
  ResultType  oResultType;
  return Execute(szCmd, oResultType, oMsgList);
} // end DBMSrvKnl_Session :: Execute

/*-----------------------------------------------------------------------------*/
/*! @brief public member */
bool DBMSrvKnl_Session :: ExecuteWithoutCommit
  ( const char      * szCmd,
    DBMSrvMsg_Error & oMsgList )
{
  bool bResult = false;
  bool bCommit = this->bAutoCommit;
  this->bAutoCommit  = false;
  ResultType  oResultType;
  bResult = Execute(szCmd, oResultType, oMsgList);
  this->bAutoCommit = bCommit;
  return bResult;
} // end DBMSrvKnl_Session :: ExecuteWithoutCommit

/*-----------------------------------------------------------------------------*/
/*! @brief public static member */
bool DBMSrvKnl_Session :: Execute
    ( const char      * szDB,
      const char      * szUser,
      const char      * szPassword,
      const char      * szCommand,
      DBMSrvMsg_Error & oMsgList)
{
  return Execute(szDB, szUser, szPassword, szCommand, oMsgList, false);
} // end DBMSrvKnl_Session :: Execute

/*-----------------------------------------------------------------------------*/
/*! @brief public  static member */
bool DBMSrvKnl_Session :: ExecuteWithNoTempFiles
    ( const char      * szDB,
      const char      * szUser,
      const char      * szPassword,
      const char      * szCommand,
      DBMSrvMsg_Error & oMsgList)
{
  return Execute(szDB, szUser, szPassword, szCommand, oMsgList, true);
} // end DBMSrvKnl_Session :: ExecuteWithNoTempFiles

/*-----------------------------------------------------------------------------*/
/*! @brief public static member */
bool DBMSrvKnl_Session :: Execute
    ( const char      * szDB,
      const char      * szUser,
      const char      * szPassword,
      const char      * szCommand,
      DBMSrvMsg_Error & oMsgList,
      bool              bWithNoTempFiles)
{
  DBMSrvKnl_Session oSession;

  if (oSession.NormalConnect(szDB, szUser, szPassword, oMsgList, bWithNoTempFiles)) {
    ResultType  oResultType;
    if (oSession.Execute(szCommand, oResultType, oMsgList )) {
      if (oResultType == DBMSrvKnl_Session::resultMessage) {
        oMsgList.Overrule(oSession.KernelMessage());
      } // end if
    } else {
      return false;
    } // end if
    DBMSrvMsg_Error oErrorToIgnore;
    oSession.Release(oErrorToIgnore);
  } else {
    return false;
  } // end if

  return true;
} // end DBMSrvKnl_Session :: Execute


/*-----------------------------------------------------------------------------*/
/*! @brief public member */
bool DBMSrvKnl_Session :: Execute
  ( const char      * szCmd,
    ResultType      & oResultType,
    DBMSrvMsg_Error & oMsgList )
{
    // if there is a open result set (created by fetch) so close it
    // ignore the error my be the result set isn't open or was closed
    // or broken before
    ResultType oLocalResultType;
    closeResultSet(oLocalResultType);

    // reset record object because of this member function starts always 
    // a new action like a select, insert, save, restore and so on
    oRecord.RefreshRecordData(DBMSrvKnl_SQLRecord::resetAll);

    // now we have cleand up and can execute the actual action requested by the caller
    return InternExecute(szCmd, oResultType, oMsgList);
} // end DBMSrvKnl_Session :: Execute

/*-----------------------------------------------------------------------------*/
/*! @brief public member */
const Msg_List DBMSrvKnl_Session :: KernelMessage
  ( ) const
{
  Msg_List oKernelMsg;

  PIn_ReplyPacket  oPacket(this->pReceivePacket);
  // check answer packet
  if (oPacket.IsValid()) {
    PIn_ReplySegment oSegment = oPacket.GetFirstSegment ();
    PIn_Part oPart   = oSegment.FindPart(sp1pk_message_list);
    if (oPart.IsValid()) {
      PIn_MessageListPart oMsgListPart(oPart);
      oKernelMsg = oMsgListPart.MessageList();
    } // end if
  } // end if

  return oKernelMsg;
} // end DBMSrvKnl_Session :: KernelMessage

/*-----------------------------------------------------------------------------*/
/*! @brief private member */
bool DBMSrvKnl_Session :: Connect
    ( DBMSrvKnl_Session::TaskType   oTaskTypeIn,
      const char                  * szDB,
      DBMSrvMsg_Error             & oMsgList )
{
  // check user and password
  if ((szDB == NULL) || (strlen(szDB) == 0)) {
    oMsgList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_MISSDBUSRPWD));
    oMsgList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_PARAM));
    return false;
  } // end if

  tsp01_Service       nService;
  tsp00_TaskId        nTaskId;
  tsp00_ErrText       pasErrtext;
  tsp01_CommErr       nCommRc;

  // fill argument nTaskId for sqlaconnect
  sqluid (&nTaskId);

  // fill argument nService for sqlaconnect
  switch (oTaskTypeIn) {
    case DBMSrvKnl_Session::taskNormal:
      nService.becomes(srvUser_esp01);
      break;
    case DBMSrvKnl_Session::taskEvent:
      nService.becomes(srvEvent_esp01);
      break;
    default:
      {
        DBMSrvMsg_Error oMsg(SDBMSG_DBMSRV_UNKNOWNTASK);
        oMsgList.Overrule(oMsg);
        return false;
      }
      break;
  } // end switch

  // connect to kernel
  tsp00_NodeId tspNode;
  tsp00_DbName tspDB;

  sqlaconnect( nTaskId,
               tspNode.c2p(""),
               tspDB.c2p(szDB),
               nService,
               1,
               this->nSessionHandle,
               this->nPacketSize,
               (void **) &this->pPacket,
               pasErrtext,
               nCommRc );

  // fallback to utility if task limit reached
  if ((nCommRc == commErrTasklimit_esp01) && (oTaskTypeIn == DBMSrvKnl_Session::taskNormal)) {
      nService.becomes(srvUtility_esp01);
      sqlaconnect( nTaskId,
                  tspNode.c2p(""),
                  tspDB.c2p(szDB),
                  nService,
                  1,
                  this->nSessionHandle,
                  this->nPacketSize,
                  (void **) &this->pPacket,
                  pasErrtext,
                  nCommRc );
  } // end if

  if (nCommRc != commErrOk_esp01)
    oMsgList.Overrule(DBMSrvMsg_RTEError(nCommRc, pasErrtext));
  else
      m_SessionHandleIsValid=true;

  if( nCommRc == commErrTasklimit_esp01 ) 
    oMsgList.Overrule(DBMSrvMsg_Error(
        SDBMSG_DBMSRV_SESSIONLIMIT,
	    Msg_Arg(SDBMSGTAG_DBMSRV_SESSIONLIMIT__SESSIONTYPE, getTextForTasktype(oTaskTypeIn))));

  return nCommRc == commErrOk_esp01;
} // end DBMSrvKnl_Session :: Connect

/*-----------------------------------------------------------------------------*/
/*! @brief private member */
bool DBMSrvKnl_Session :: InternExecute
  ( const char      * szCmd,
    ResultType      & oResultType,
    DBMSrvMsg_Error & oMsgList,
    bool              useImplicitReconnectFlag)
{
  if (Request(szCmd, oMsgList, useImplicitReconnectFlag)) {
    return Receive(oResultType, oMsgList);
  } // end if
  return false;
} // end DBMSrvKnl_Session :: InternExecute

/*-----------------------------------------------------------------------------*/

bool DBMSrvKnl_Session::closeResultSet(
    ResultType & resultType )
{
    bool rc=true;

    if (this->bIsResultOpen)
    {
        DBMSrvMsg_Error localError;
        rc=InternExecute("CLOSE", resultType, localError, false); //do not try to reconnect for a close, if the session is dead any result sets are closed anyway
        this->bIsResultOpen = false;
    }

    return rc;
}

/*-----------------------------------------------------------------------------*/
/*! @brief private member */
bool DBMSrvKnl_Session :: ConnectUser
    ( const char      * szUsr,
      const char      * szPassword,
      bool              bNoTempFile,
      DBMSrvMsg_Error & oMsgList)
{
  // username
  this->szUser.rawAssign(szUsr);
  if (!ToolsParsersUtil_IdentifierCheck::IsSpecialIdentifier(this->szUser)) {
    // this is not a special identifier
    ToolsParsersUtil_IdentifierCheck::KernelToupper(this->szUser);
  } else {
    // strip doublequotes
    this->szUser.rawAssign(&this->szUser[1]);
    this->szUser[this->szUser.length()-1] = 0;
  } // end if
      
  this->szPwd.rawAssign(szPassword);
  if (!ToolsParsersUtil_IdentifierCheck::IsSpecialIdentifier(this->szPwd)) {
    // this is not a special identifier
    ToolsParsersUtil_IdentifierCheck::KernelToupper(this->szPwd);
  } else {
    // strip doublequotes
    this->szPwd.rawAssign(&this->szPwd[1]);
    this->szPwd[this->szPwd.length()-1] = 0;
  } // end if

  {
    // create request packet
    PIn_RequestPacket  oPacket(this->pPacket, this->nPacketSize, this->nUnicode);
    // initialise request packet
    oPacket.InitVersion(csp_comp_db_manager, NULL);
    // create segement
    PIn_RequestSegment oSegment = oPacket.AddSegment(sp1m_dbs, sp1sm_session_sqlmode);

    oSegment.SetProducer(sp1pr_internal_cmd);

    // create command part
    PIn_Part oCommandPart = oSegment.AddPart(sp1pk_command);
    // fill commadn part
    oCommandPart.AddArgument(CMD_CONNECT_P1, (int) strlen(CMD_CONNECT_P1));
    oCommandPart.AddData    (this->szUser,   (int) strlen(this->szUser));
    oCommandPart.AddData    (CMD_CONNECT_P2, (int) strlen(CMD_CONNECT_P2));
    if (bNoTempFile) {
      oCommandPart.AddData    (CMD_CONNECT_P3, (int) strlen(CMD_CONNECT_P3));
    } // end if
    // close command part
    oSegment.ClosePart(oCommandPart);
    // create data part for crypted password
    PIn_Part oDataPart = oSegment.AddPart(sp1pk_data);

    // fill data part with crypted password
    tsp00_Byte    cByte = csp_defined_byte;
    tsp00_CryptPw cryptPwd;
    tsp00_Name    pasPwd;
    tsp00_TermId  oTermid;

    pasPwd.c2p(this->szPwd);
    s02applencrypt (pasPwd, cryptPwd);

    sqltermid (oTermid);

    oDataPart.AddArgument(&cByte,   sizeof(cByte));
    oDataPart.AddData    (cryptPwd, sizeof(cryptPwd));
    oDataPart.AddData    (&cByte,   sizeof(cByte));
    oDataPart.AddData    (oTermid,  sizeof(oTermid));

    // close part
    oSegment.ClosePart(oDataPart);
    // close segment
    oPacket.CloseSegment(oSegment);

    // send request to kernel
    bool connectionBroken=false;

    if (!Request(oPacket.Length(), connectionBroken, oMsgList)) {
      return false;
    } // end if
  } // end block

  {
    // receive answer from kernel and create answer packet
    PIn_ReplyPacket  oPacket(Receive(oMsgList));
    // check answer packet
    if (!oPacket.IsValid()) {
      return false;
    } // end if

    // read data
    PIn_ReplySegment oSegment     = oPacket.GetFirstSegment ();

    if (oSegment.ErrorCode() == 0) {

      PIn_Part         oPart = oSegment.FindPart(sp1pk_session_info_returned);

      if (oPart.IsValid()) {
        PIn_SessionInfoPart oSessionInfoPart(oPart);
        this->nUnicode = oSessionInfoPart.IsUnicode();
        this->bAutoCommit = true;
      } // end if

      oPart = oSegment.FindPart(sp1pk_user_info_returned);
      if (oPart.IsValid()) {
        PIn_UserInfoPart oUserInfoPart(oPart);
        szSYSDBA.rawAssign(oUserInfoPart.GetSYSDBA());
      } // end if

    } else {
      this->ReadErrorText(oSegment, oMsgList);
      return false;
    } // end if

  } // end block

  return true;
} // DBMSrvKnl_Session :: ConnectUser

/*-----------------------------------------------------------------------------*/
/*! @brief private member */
bool DBMSrvKnl_Session :: FetchRecordBlock
    ( bool            & bEnd,
      bool              bFirst,
      DBMSrvMsg_Error & oMsgList)
{
  Tools_DynamicUTF8String sCommand;
  ResultType              oResultType;
  tsp00_Int4              nParams = this->ParamCount();
  tsp00_Int4              i = 0;
  DBMSrvMsg_Error         msgListFetch;

  bEnd = false;

  sCommand = bFirst ? "FETCH FIRST INTO ?" : "FETCH NEXT INTO ?";

  for (i = 1; i < nParams; ++i) {
    sCommand.Append(", ?");
  } // end for

  if (!this->InternExecute(sCommand.CharPtr(), oResultType, msgListFetch)) {
    if (!bFirst && (msgListFetch.getTagValueAsInteger(SDBMSGTAG_DBMSRV_SQL__RETURNCODE) == 100)) {
        closeResultSet(oResultType);
        bEnd = true;

        return true;
    } // end if

    oMsgList.Overrule(msgListFetch);

    return false;
  } else {
    this->bIsResultOpen = true;
  } // end if

  return true;
} // end DBMSrvKnl_Session :: FetchRecordBlock

/*-----------------------------------------------------------------------------*/
/*! @brief private member */
bool DBMSrvKnl_Session :: Request
    ( SAPDB_Int4        nRequestLength,
      bool            & connectionBroken,
      DBMSrvMsg_Error & oMsgList )
{
    tsp00_ErrText  pasErrtext;
    tsp01_CommErr  nCommRc;

    connectionBroken=false;

    if(this->bRequestPending)
    {
        oMsgList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_ERR));
        return false;
    }
    else
    {
        sqlarequest(
            this->nSessionHandle,
            this->pPacket,
            nRequestLength,
            pasErrtext,
            nCommRc);

        if (nCommRc != commErrOk_esp01) {
            oMsgList.Overrule(DBMSrvMsg_RTEError(nCommRc, pasErrtext));
            connectionBroken=true;
            return false;
        }
        else
        {
            this->bRequestPending = true;
            return true;
        }
    }
} // end DBMSrvKnl_Session :: Request

/*-----------------------------------------------------------------------------*/
/*! @brief public member */
tsp00_Int4 DBMSrvKnl_Session :: ParamCount
  ( )
{
  PIn_ReplyPacket  oPacket(this->pReceivePacket);
  // check answer packet
  if (oPacket.IsValid()) {
    PIn_ReplySegment oSegment = oPacket.GetFirstSegment ();
    PIn_Part oShortInfo   = oSegment.FindPart(sp1pk_shortinfo);
    if (oShortInfo.IsValid()) {
      return oShortInfo.GetRawPart()->sp1p_arg_count();
    } // end if
  } // end if

  return 0;
} // end DBMSrvKnl_Session :: ParamCount

/*-----------------------------------------------------------------------------*/
/*! @brief public member */
tsp1_param_info * DBMSrvKnl_Session :: ParamInfo
  ( )
{
  PIn_ReplyPacket  oPacket(this->pReceivePacket);
  // check answer packet
  if (oPacket.IsValid()) {
    PIn_ReplySegment oSegment = oPacket.GetFirstSegment ();
    PIn_Part oShortInfo = oSegment.FindPart(sp1pk_shortinfo);
    if (oShortInfo.IsValid()) {
      return  (tsp1_param_info *) oShortInfo.GetReadData();
    } // end if
  } // end if

  return NULL;
} // DBMSrvKnl_Session :: ParamInfo

/*-----------------------------------------------------------------------------*/
/*! @brief public member */
tsp00_Int4 DBMSrvKnl_Session :: RecordSize
  ( )
{
  tsp00_Int4        nRecordSize = 0;
  tsp00_Int4        nParamCount = this->ParamCount();
  tsp00_Int4        i           = 0;
  tsp1_param_info * pParamInfo  = this->ParamInfo();

  for (i = 0; i < nParamCount; ++i) {
    nRecordSize = nRecordSize + pParamInfo[i].sp1i_in_out_len;
  } // end for

  return nRecordSize;
} // DBMSrvKnl_Session :: RecordSize

/*-----------------------------------------------------------------------------*/
/*! @brief public member */
tsp00_Int4 DBMSrvKnl_Session :: RecordCount
  ( )
{
  tsp00_Int4        RecordCount = 0;

  PIn_ReplyPacket  oPacket(this->pReceivePacket);
  // check answer packet
  if (oPacket.IsValid()) {
    PIn_ReplySegment oSegment = oPacket.GetFirstSegment ();
    PIn_Part oPart = oSegment.FindPart(sp1pk_data);
    if (oPart.IsValid()) {
      PIn_DataPart oDataPart(oPart);
      RecordCount = oDataPart.GetRawPart()->sp1p_arg_count();
    } // end if
  } // end if

  return RecordCount;
} // DBMSrvKnl_Session :: ResultCount

/*-----------------------------------------------------------------------------*/
/*! @brief public member */
DBMSrvKnl_SQLRecord & DBMSrvKnl_Session :: GetRecordObject
  ( )
{
  this->oRecord.RefreshRecordData(DBMSrvKnl_SQLRecord::refreshWithoutRecordCounter);
  return this->oRecord;
} // end DBMSrvKnl_Session :: GetRecordObject

  /*-----------------------------------------------------------------------------*/
/*! @brief public member */
Tools_DynamicUTF8String DBMSrvKnl_Session :: FieldName
  ( const tsp00_Int4     nField )
{
  Tools_DynamicUTF8String sField;

  PIn_ReplyPacket  oPacket(this->pReceivePacket);

  // check answer packet
  if (oPacket.IsValid()) {
    PIn_ReplySegment oSegment = oPacket.GetFirstSegment ();
    PIn_Part oColumnNames = oSegment.FindPart(sp1pk_columnnames);
    int                     nParamCount = this->ParamCount();

    if (oColumnNames.IsValid()) {
      const tsp00_Byte * pNames      = oColumnNames.GetReadData();
      int                i           = 0;
      int                nLen        = 0;

      // get columns
      for (i = 1; i <= nParamCount; i++) {
        nLen = *pNames++;
        if (i == nField) {
          if (this->nUnicode) {
            sField.ConvertFromUCS2_Unaligned_Swap((Tools_DynamicUTF8String::UCS2Char *) pNames, (Tools_DynamicUTF8String::UCS2Char *) (pNames + nLen));
          } else {
            sField.ConvertFromASCII_Latin1((Tools_DynamicUTF8String::ASCIIConstPointer) pNames, (Tools_DynamicUTF8String::ASCIIConstPointer) (pNames + nLen));
          } // end if
          break;
        } // end if
        pNames += nLen;
      } // end for

    } else {
      int i = 0;
      Tools_DynamicUTF8String sNumber;
      for (i = 1; i <= nParamCount; i++) {
        sNumber.ConvertFromInt(i);
        sField.Assign("Field").Append(sNumber);
      } // end for
    }  // end if
  } // end if

  return sField;
} // end DBMSrvKnl_Session :: FieldName

/*-----------------------------------------------------------------------------*/
/*! @brief public member */
bool DBMSrvKnl_Session :: GetValue
    ( const tsp00_Int4     nField,
      DBMSrvKnl_SQLField    & oField )
{
  PIn_ReplyPacket  oPacket(this->pReceivePacket);
  // check answer packet
  if (oPacket.IsValid()) {
    PIn_ReplySegment oSegment = oPacket.GetFirstSegment ();
    PIn_Part oPart   = oSegment.FindPart(sp1pk_data);

    if (oPart.IsValid()) {
      PIn_DataPart                   oDataPart(oPart);
      PIn_DataPart::ConversionNeeded oConversion = PIn_DataPart::ConvertUnknown;
      const char *                   pValue      = NULL;
      tsp00_Int4                     nLength     = 0;

      switch (oField.oType) {
        case DBMSrvKnl_SQLField::typeInt:
          oField.iValue = 0;
          oField.bNull = oDataPart.IsNull(&(oField.oInfo));
          if (!oField.bNull) {
            if (!oDataPart.GetValueAsInt(&(oField.oInfo), oField.iValue)) {
              return false;
            } // end if
          } // end if
          break;
        case DBMSrvKnl_SQLField::typeString:
          oField.sValue = "";
          oField.bNull = oDataPart.IsNull(&(oField.oInfo));
          if (!oDataPart.GetValueAsChar(&(oField.oInfo), pValue, nLength, oConversion)) {
            return false;
          } // end if

          switch (oSegment.GetRawHeader()->sp1r_function_code()) {
            case csp1_administrator_fc:
            case csp1_diagnose_fc:
              oField.sValue.ConvertFromASCII_Latin1((Tools_DynamicUTF8String::ASCIIConstPointer) pValue, (Tools_DynamicUTF8String::ASCIIConstPointer) (pValue + nLength));
              break;
            default:
              switch (oConversion) {
                case PIn_DataPart::ConvertAlways:
                  oField.sValue.ConvertFromUCS2_Unaligned_Swap((Tools_DynamicUTF8String::UCS2Char *) pValue, (Tools_DynamicUTF8String::UCS2Char *) (pValue + (nLength*2)));
                  break;
                case PIn_DataPart::ConvertNever:
                  oField.sValue.AssignRaw(_Tools_UTF8Ptr(pValue), nLength);
                  break;
                case PIn_DataPart::ConvertSometimes:
                  if (this->nUnicode) {
                    oField.sValue.ConvertFromUCS2_Unaligned_Swap((Tools_DynamicUTF8String::UCS2Char *) pValue, (Tools_DynamicUTF8String::UCS2Char *) (pValue + (nLength*2)));
                  } else {
                    oField.sValue.AssignRaw(_Tools_UTF8Ptr(pValue), nLength);
                  } // end if
                  break;
                default:
                  return false;
                  break;
              } // end switch
              break;
          } // end swicth

          oField.sValue.TrimTrailing();
          break;
        default:
          return false;
          break;
      } // end switch

    } // end if
  } // end if

  return true;
} // end DBMSrvKnl_Session :: GetValue

/*-----------------------------------------------------------------------------*/
/*! @brief private member */
tsp1_packet * DBMSrvKnl_Session :: Receive
    ( DBMSrvMsg_Error & oMsgList )
{
  tsp00_ErrText  pasErrtext;
  tsp01_CommErr  nCommRc;
  SAPDB_Int4     nLength;

  this->pReceivePacket = NULL;

  if(this->bRequestPending)
  {
    // poll here for answer
    if ((!this->bAllowedInDbFull) && (!this->bNoTempFiles) && (m_TaskType == DBMSrvKnl_Session::taskNormal)) {
        SAPDB_UInt4  nMilliSeconds  = 1;
        bool         bLogFull       = false;
        bool         bDataFull      = false;

        while (!this->ReplyAvailable()) {
        RTE_ISystem::DoSleep(nMilliSeconds);
        if (nMilliSeconds > 100) {
            oState.IsFull(bLogFull, bDataFull);
            if (!oState.DBMMsg().IsEmpty()) {
            oMsgList = oState.DBMMsg();
            this->BreakSession();
            return NULL;
            } else if (bDataFull) {
            // database full error
            oMsgList = DBMSrvMsg_Error(SDBMSG_DBMSRV_DATAFULL);
            this->BreakSession();
            return NULL;
            } // end if
        } else {
            nMilliSeconds *= 2;
        } // end if
        } // end while
    } // end if

    sqlareceive (this->nSessionHandle,
                (void**)&this->pReceivePacket,
                nLength,
                pasErrtext,
                nCommRc);
  }
  else
       oMsgList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_ERR));

  this->bRequestPending = false;

  if (nCommRc != commErrOk_esp01) {
    oMsgList.Overrule(DBMSrvMsg_RTEError(nCommRc, pasErrtext));

    if(nCommRc == commErrShutdown_esp01)
      oMsgList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHUTDOWN));

    return NULL;
  } // end if

  return pReceivePacket;
} // end DBMSrvKnl_Session :: Receive

/*-----------------------------------------------------------------------------*/
/*! @brief public member */
bool DBMSrvKnl_Session :: ReplyAvailable
  ( )
{
  tsp00_ErrText  pasErrtext;
  tsp01_CommErr  nCommRc;
  sqlareplyavailable (this->nSessionHandle, pasErrtext, nCommRc);
  return (nCommRc != commErrWouldBlock_esp01);
} // end DBMSrvKnl_Session :: ReplyAvailable

/*-----------------------------------------------------------------------------*/
/*! @brief public member */
void DBMSrvKnl_Session :: Cancel
  ( )
{
  sqlacancel (this->nSessionHandle);
} // end DBMSrvKnl_Session :: Cancel

/*-----------------------------------------------------------------------------*/
/*! @brief private member */
void DBMSrvKnl_Session :: ReadErrorText
  ( PIn_ReplySegment & oSegment, 
    DBMSrvMsg_Error  & oMsgList )
{
    PIn_Part oPart_E  = oSegment.FindPart(sp1pk_errortext);
    PIn_Part oPart_M  = oSegment.FindPart(sp1pk_message_list);

    Tools_DynamicUTF8String sError;

    // look for a error text in "Error Text Part"
    if (oPart_E.IsValid()) {
      PIn_ErrorTextPart oErrorTextPart(oPart_E);
      if (this->nUnicode) {
        sError.ConvertFromUCS2_Unaligned_Swap((Tools_DynamicUTF8String::UCS2Char *) oErrorTextPart.ErrorText(), (Tools_DynamicUTF8String::UCS2Char *) (oErrorTextPart.ErrorText() + oErrorTextPart.ErrorLen()));
      } else {
        sError.ConvertFromASCII_Latin1((Tools_DynamicUTF8String::ASCIIConstPointer) oErrorTextPart.ErrorText(), (Tools_DynamicUTF8String::ASCIIConstPointer) (oErrorTextPart.ErrorText() + oErrorTextPart.ErrorLen()));
      } // end if
    } // end if

    // look for error text in column "TAPE ERRORTEXT"
    if (sError.Length() == 0) {
      // may be it was a backup/recovery without error text part but an errortext
      // inside the answer record in the column "TAPE ERRORTEXT"
      DBMSrvKnl_SQLRecord & oLocalRecord = this->GetRecordObject();
      sError = oRecord.fieldAsChar("TAPE ERRORTEXT");
    } //end if

    // no error text available
    if (sError.Length() == 0) {
      sError = "(no error text available from database kernel)";
    } //end if

    // 1. add the MessageList (if available)
    if (oPart_M.IsValid()) {
      PIn_MessageListPart oMsgListPart(oPart_M);
      oMsgList.Overrule(oMsgListPart.MessageList());
    } // end if

    // 2. at top the well known DBMSrv_SQL error (with kernel data in tags)
    oMsgList.Overrule(DBMSrvMsg_SQLError(oSegment.ErrorCode(), sError.CharPtr()));

} // end DBMSrvKnl_Session :: ReadErrorText

/*-----------------------------------------------------------------------------*/
/*! @brief private member */
bool DBMSrvKnl_Session :: AllowedInDbFull
  ( const char * szCmd )
{
  // check for
  //     add data
  //     save log
  //     auto save
  //     takeover

  const char * szAllowed [] = {"ADD DATA",
                               "ADD LOG",
                               "SAVE LOG",
                               "AUTOSAVE",
                               "TAKEOVER",
                               NULL
                              };

  Tools_DynamicUTF8String szString(_Tools_UTF8Ptr(szCmd));
  Tools_DynamicUTF8String sz1stToken;
  Tools_DynamicUTF8String sz2ndToken;

  szString.ToUpper();
  szString.Trim();

  Tools_DynamicUTF8String::BasisElementIndex nSpace = szString.Find(" ");

  // extract first two tokens
  if (nSpace != Tools_DynamicUTF8String::NPos) {
    sz1stToken = szString.SubStr(0, nSpace);
    szString   = szString.SubStr(nSpace);
    szString.Trim();
    nSpace = szString.Find(" ");
    if (nSpace != Tools_DynamicUTF8String::NPos) {
      sz2ndToken = szString.SubStr(0, nSpace);
    } else {
      sz2ndToken = szString;
    } // end if
  } else {
    sz1stToken = szString;
  } // end if

  // build string
  szString.Assign(sz1stToken).Append(" ").Append(sz2ndToken);

  // compare with allowed list
  int nAllowed = 0;
  while (szAllowed[nAllowed] != NULL) {
    if (szString.Compare(szAllowed[nAllowed]) == 0) {
      return true;
    } // end if
    ++nAllowed;
  } // end while

  return false;
} // end DBMSrvKnl_Session :: AllowedInDbFull

/*-----------------------------------------------------------------------------*/
/*! @brief public member */
bool DBMSrvKnl_Session :: isSYSDBA
    ( const char * szUserIn )
{
  if (szUserIn == NULL) {
    return (strcmp(this->szUser, this->szSYSDBA) == 0);
  } else {
    tsp00_KnlIdentifierc szUserLokal;
    szUserLokal.rawAssign(szUserIn);
    if (!ToolsParsersUtil_IdentifierCheck::IsSpecialIdentifier(szUserLokal)) {
      // this is not a special identifier
      ToolsParsersUtil_IdentifierCheck::KernelToupper(szUserLokal);
    } else {
      // strip doublequotes
      szUserLokal.rawAssign(&szUserLokal[1]);
      szUserLokal[szUserLokal.length()-1] = 0;
    } // end if
    return (strcmp(szUserLokal, this->szSYSDBA) == 0);
  } // end if

} // end DBMSrvKnl_Session :: isSYSDBA

/*-----------------------------------------------------------------------------*/

void DBMSrvKnl_Session::allowReconnectAtNextExecute(
    SubsessionType reconnectType)
{
    if(reconnectType!=DBMSrvKnl_Session::noSubsession &&
       m_ImplicitReconnectType==DBMSrvKnl_Session::noSubsession)
    {
        m_ImplicitReconnectType=reconnectType;
        m_ImplicitReconnectIsAllowed[reconnectType]=true;
        m_ImplicitReconnectEndedSubsession[reconnectType]=false;
        m_ReconnectMessage[reconnectType].ClearMessageList();
    }
}

bool DBMSrvKnl_Session::hasInterruptedSubsession(
    SubsessionType   subsessionType,
    DBMSrvMsg_Info & info) const
{
    if(subsessionType<DBMSrvKnl_Session::noSubsession &&
       m_ImplicitReconnectEndedSubsession[subsessionType])
    {
        info.Overrule(m_ReconnectMessage[subsessionType]);
        return true;
    }

    return false;
}

void DBMSrvKnl_Session::disallowReconnectAtNextExecute()
{
    if(m_ImplicitReconnectType<DBMSrvKnl_Session::noSubsession)
    {
        m_ImplicitReconnectIsAllowed[m_ImplicitReconnectType]=false;
        m_ImplicitReconnectType=DBMSrvKnl_Session::noSubsession;
    }
}

bool DBMSrvKnl_Session::duplicateSessionAndConnect(
        DBMSrvKnl_Session & duplicate,
        DBMSrvMsg_Error   & error) const
{
    if(duplicate.isConnected() &&
       !duplicate.Release(error) )
    {
        return false;
    }

    if(m_TaskType==DBMSrvKnl_Session::taskEvent)
    {
        return duplicate.EventConnect(this->szDatabase.asCharp(), error);
    }
    else
    {
        return duplicate.NormalConnect(
                    this->szDatabase.asCharp(),
                    this->szUser.asCharp(),
                    this->szPwd.asCharp(),
                    error);
    }
}

/*-----------------------------------------------------------------------------*/
/*! @brief private member */
void DBMSrvKnl_Session :: Reset
    ( bool keepConnectInfo )
{
  this->nSessionHandle   = 0;
  this->m_SessionHandleIsValid=false;
  this->nPacketSize      = 0;
  this->pPacket          = NULL;
  this->pReceivePacket   = NULL;
  this->nUnicode         = 0;

  if(!keepConnectInfo)
  {
    this->szUser.Init();
    this->szPwd.Init();
    this->szDatabase.Init();
  }

  this->szSYSDBA.Init();
  m_Connected=false;

  if(!keepConnectInfo)
    m_TaskType= DBMSrvKnl_Session::taskUnknown;

  this->bAutoCommit      = false;
  this->bNoTempFiles     = false;
  this->bAdminConnect    = false;
  this->bAllowedInDbFull = false;
  this->bRequestPending  = false;
  this->oUserType        = DBMSrvKnl_Session::userUnknown;
  this->bIsResultOpen    = false;
  oRecord.SetSession(this);
  
  if(!keepConnectInfo)
  {
      m_ImplicitReconnectType=DBMSrvKnl_Session::noSubsession;

      for(int i=0; i<DBMSrvKnl_Session::noSubsession; i++)
      {
          m_ImplicitReconnectIsAllowed[i]=false;
          m_ImplicitReconnectEndedSubsession[i]=false;
          m_ReconnectMessage[i].ClearMessageList();
      }
  }

} // end DBMSrvKnl_Session :: Reset

/*-----------------------------------------------------------------------------*/
bool DBMSrvKnl_Session::implicitReconnectFlagIsSet()
{
    return (m_ImplicitReconnectType<DBMSrvKnl_Session::noSubsession &&  //we are allowed to reconnect ...
            m_ImplicitReconnectIsAllowed[m_ImplicitReconnectType]);     //... with the current type of sub session       
}
