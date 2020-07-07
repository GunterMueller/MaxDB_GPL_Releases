/*!
  @file           DBMSrvKnl_State.cpp
  @author         Bernd Vorsprach - bernd.vorsprach@sap.com
  @brief          operational state of database instance - Implementation

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
#include "RunTime/RTE_DBRegister.hpp"

#include "DBM/Srv/KernelAccess/DBMSrvKnl_State.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_RTEError.hpp"
#include "DBM/Logging/DBMLog_Logger.hpp"

#define LOG_IS_NOT_FULL  0
#define DATA_IS_NOT_FULL 0

/*-----------------------------------------------------------------------------*/
/*! @brief constructor */
DBMSrvKnl_State :: DBMSrvKnl_State
    (const char * szDB )
    : m_pActiveDatabase(NULL),
      m_szDBName(szDB)
{
  GetDatabase();
} // end DBMSrvKnl_State :: DBMSrvKnl_State

/*-----------------------------------------------------------------------------*/
/*! @brief constructor */
DBMSrvKnl_State :: DBMSrvKnl_State
    ( )
    : m_pActiveDatabase(NULL),
      m_szDBName("")
{

} // end DBMSrvKnl_State :: DBMSrvKnl_State

/*-----------------------------------------------------------------------------*/
/*! @brief public member SetDatabase */
bool DBMSrvKnl_State :: SetDatabase
    (const char * szDB )
{
  m_szDBName = szDB;
  return GetDatabase();
} // end DBMSrvKnl_State :: DBMSrvKnl_State

/*-----------------------------------------------------------------------------*/
/*! @brief destructor */
DBMSrvKnl_State :: ~DBMSrvKnl_State
    ( )
{
  if (m_pActiveDatabase != NULL)  delete m_pActiveDatabase;
} // end DBMSrvKnl_State :: ~DBMSrvKnl_State

/*-----------------------------------------------------------------------------*/
/*! @brief public member function Speed */
const RTE_SpeedInfo & DBMSrvKnl_State :: Speed
  (  )
{
  if ((m_pActiveDatabase != NULL) || ((m_pActiveDatabase == NULL) && GetDatabase())) {
    return m_pActiveDatabase->GetActiveSpeed();
  } // end if
  return speedNone;
} // end DBMSrvKnl_State :: Speed

/*-----------------------------------------------------------------------------*/
/*! @brief public member function Speed */
const RTE_SpeedInfo &  DBMSrvKnl_State :: Speed
  ( const char              * szDB )
{
  DBMSrvKnl_State oState(szDB);
  if (oState.DBMMsg().IsEmpty()) {
    return oState.Speed();
  } // end if
  return speedNone;
} // end DBMSrvKnl_State :: Speed

/*-----------------------------------------------------------------------------*/
/*! @brief public member function State */
DBMSrvKnl_State :: StateValue DBMSrvKnl_State :: State
  ( Tools_DynamicUTF8String & szState,
    bool                    & bLogIsFull,
    bool                    & bDataIsFull )
{
  StateValue                         oState         = StateUndefined;

  szState.Empty();
  bLogIsFull  = false;
  bDataIsFull = false;

  const RTE_Database::DBStatusInfo * pStatus        = NULL;
  Kernel_SharedState const         * pExtendedState = NULL;
  if (Refresh(pStatus, pExtendedState)) {
    if (pStatus != NULL) {
      oState = (StateValue) pStatus->statusId;
      szState = pStatus->statusName;
    } // end if
    if (pExtendedState != NULL) {
      bDataIsFull = (pExtendedState->databaseFullState  != DATA_IS_NOT_FULL) && (oState != StateOffline);
      bLogIsFull  = (pExtendedState->logvolumeFullState != LOG_IS_NOT_FULL)  && (oState != StateOffline);
    } // end if
  } // end if

  return oState;
} // end DBMSrvKnl_State :: State

/*-----------------------------------------------------------------------------*/
/*! @brief public member function State */
DBMSrvKnl_State :: StateValue DBMSrvKnl_State :: State
  ( )
{
  StateValue                         oState  = StateUndefined;
  
  const RTE_Database::DBStatusInfo * pStatus = NULL;
  if (Refresh(pStatus) && (pStatus != NULL)) {
    oState = (StateValue) pStatus->statusId;
  } // end if
  
  return oState;
} // end DBMSrvKnl_State :: State

/*-----------------------------------------------------------------------------*/
const char * DBMSrvKnl_State::getStateAsString()
{
    return getStateAsString(State());
}

struct StateStringMapEntry
{
    DBMSrvKnl_State::StateValue   m_state;
    const char *                  m_stateString;
};

/*-----------------------------------------------------------------------------*/
const char * DBMSrvKnl_State::getStateAsString(StateValue state)
{
    int i=0;

    StateStringMapEntry map[]=
    {
        {StateUnknown,        "UNKNOWN"},
        {StateOffline,        "OFFLINE"},
        {StateStarting,       "STARTING"},
        {StateAdmin,          "ADMIN"},
        {StateStandby,        "STANDBY"},
        {StateOnline,         "ONLINE"},
        {StateShutdown,       "SHUTDOWN"},
        {StateShutdownreinit, "SHUTDOWNREINIT"},
        {StateShutdownkill,   "SHUTDOWNKILL"},
        {StateStop,           "STOP"},
        {StateKill,           "KILL"},
        {StateAbort,          "ABORT"},
        {StateStopped,        "STOPPED"},
        {StateError,          "ERROR"},
        {StateUndefined,      "UNDEFINED"}
    };

    while(DBMSrvKnl_State::StateUndefined!=map[i].m_state && state!=map[i].m_state)
        i++;

    return map[i].m_stateString;
}

/*-----------------------------------------------------------------------------*/
struct SpeedStringMapEntry
{
    RTE_SpeedInfo                 m_speed;
    const char *                  m_speedString;
};

const char * DBMSrvKnl_State::getSpeedAsString(RTE_SpeedInfo speed)
{
    int i=0;

    SpeedStringMapEntry map[]=
    {
        {speedFast,   "FAST"},
        {speedSlow,   "SLOW"},
        {speedTest,   "TEST"},
        {speedNone,   "UNDEFINED"}
    };

    while(!(speedNone==map[i].m_speed) && !(speed==map[i].m_speed))
        i++;

    return map[i].m_speedString;
}

/*-----------------------------------------------------------------------------*/
/*! @brief public member function State */
DBMSrvKnl_State :: StateValue DBMSrvKnl_State :: State
  ( const char * szDB )
{
  StateValue nState = StateError;

  DBMSrvKnl_State oState(szDB);
  if (oState.DBMMsg().IsEmpty()) {
    nState = oState.State();
    if (!oState.DBMMsg().IsEmpty()) {
      DBMLog_Logger::getInstance().addMessageList(oState.DBMMsg());
    } // end if
  } else {
    DBMLog_Logger::getInstance().addMessageList(oState.DBMMsg());
  } // end if
  return nState;
} // end DBMSrvKnl_State :: State

/*-----------------------------------------------------------------------------*/
/*! @brief public member function IsFull */
bool DBMSrvKnl_State :: IsFull
  (  )
{
  bool bLogIsFull  = false;
  bool bDataIsFull = false;

  return IsFull(bDataIsFull, bLogIsFull);
} // end DBMSrvKnl_State :: IsFull

/*-----------------------------------------------------------------------------*/
/*! @brief public member function IsFull */
bool DBMSrvKnl_State :: IsFull
  ( bool                    & bLogIsFull,
    bool                    & bDataIsFull )
{
  bLogIsFull  = false;
  bDataIsFull = false;

  Kernel_SharedState const         * pExtendedState = NULL;
  if (Refresh(pExtendedState) && (pExtendedState != NULL)) {
    bDataIsFull = (pExtendedState->databaseFullState  != DATA_IS_NOT_FULL);
    bLogIsFull  = (pExtendedState->logvolumeFullState != LOG_IS_NOT_FULL);
  } // end if

  return (bDataIsFull || bLogIsFull);
} // end DBMSrvKnl_State :: IsFull

/*-----------------------------------------------------------------------------*/
/*! @brief private member function Refresh */
bool DBMSrvKnl_State :: Refresh
  ( const RTE_Database::DBStatusInfo * & pStatus )
{  
  Kernel_SharedState const* pExtendedState;
  return Refresh(pStatus, pExtendedState);
} // end DBMSrvKnl_State :: Refresh
/*-----------------------------------------------------------------------------*/
/*! @brief private member function Refresh */
bool DBMSrvKnl_State :: Refresh
  ( Kernel_SharedState const         * & pExtendedState )
{
  const RTE_Database::DBStatusInfo * pStatus;
  return Refresh(pStatus, pExtendedState);
} // end DBMSrvKnl_State :: Refresh
/*-----------------------------------------------------------------------------*/
/*! @brief private member function Refresh */
bool DBMSrvKnl_State :: Refresh
  ( const RTE_Database::DBStatusInfo * & pStatus,
    Kernel_SharedState const         * & pExtendedState )
{
  if ((m_pActiveDatabase != NULL) || ((m_pActiveDatabase == NULL) && GetDatabase())) {
    m_DBMMsg.ClearMessageList();
    Msg_List rteError;
    pStatus = m_pActiveDatabase->GetExtendedDBStatus( pExtendedState, rteError);
    if (pStatus == NULL) {
      if (!rteError.IsEmpty())
        m_DBMMsg.Overrule(DBMSrvMsg_RTEError(rteError));
      else
        m_DBMMsg.Overrule(DBMSrvMsg_RTEError());
      m_DBMMsg.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_STATE));
    }
  } // end if
  return m_DBMMsg.IsEmpty();
} // end DBMSrvKnl_State :: Refresh

/*-----------------------------------------------------------------------------*/
/*! @brief private member function GetDatabase */
bool DBMSrvKnl_State :: GetDatabase
  ( )
{
  m_DBMMsg.ClearMessageList();
  Msg_List rteError;
  m_pActiveDatabase = (RTE_DBRegister::Instance()).GetActiveDatabase( m_szDBName.CharPtr(), rteError);
  if (!rteError.IsEmpty()) {
    m_DBMMsg.Overrule(DBMSrvMsg_RTEError(rteError));
    m_DBMMsg.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_STATE));
  } // end if
  return m_DBMMsg.IsEmpty();
} // end DBMSrvKnl_State :: GetDatabase
