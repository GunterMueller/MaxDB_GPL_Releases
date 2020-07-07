/*!
  @file           DBMSrvKnl_State.hpp
  @author         Bernd Vorsprach - bernd.vorsprach@sap.com
  @brief          operational state of database instance - Specification

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

#ifndef _DBMSrvKnl_State_HPP_
#define _DBMSrvKnl_State_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include "geo00_2.h"
#include "SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp"
#include "RunTime/RTE_ActiveDatabase.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"

/*!
  @brief  state object

  This object determines the operational state of the database instance.
  Note, that this means, that the object determines the operational state of
  the database instance every time it is asked to deliver the state
  information. If you need the state of exactly one point in time use the enum
  StateValue.
 */
class DBMSrvKnl_State {

public:
  /*! @brief  operational state
      
      If you insert any new state into this enum update the map, that
      is mapping states to strings in the static function
      getStateAsString(...) */
  typedef enum {
    StateUnknown        = SERVER_UNKOWN_EO00,
    StateOffline        = SERVER_OFFLINE_EO00,
    StateStarting       = SERVER_STARTING_EO00,
    StateAdmin          = SERVER_ADMIN_EO00,
    StateStandby        = SERVER_STANDBY_EO00,
    StateOnline         = SERVER_ONLINE_EO00,
    StateShutdown       = SERVER_SHUTDOWN_EO00,
    StateShutdownreinit = SERVER_SHUTDOWNREINIT_EO00,
    StateShutdownkill   = SERVER_SHUTDOWNKILL_EO00,
    StateStop           = SERVER_STOP_EO00,
    StateKill           = SERVER_KILL_EO00,
    StateAbort          = SERVER_ABORT_EO00,
    StateStopped        = SERVER_STOPPED_EO00,
    StateError,
    StateUndefined      = SERVER_UNDEFINED_EO00
  } StateValue;

  /*! @brief   Constructor 
      Creates a database state object
      @param  szDB [IN] - null terminated name of database instance
   */
  DBMSrvKnl_State 
    ( const char * szDB );

  /*! @brief   Constructor 
      Creates a database state object
   */
  DBMSrvKnl_State 
    (  );

  /*! @brief   Destructor */
  ~DBMSrvKnl_State 
    (  );

  /*! @brief   set database instance name
      @param  szDB [IN] - null terminated name of database instance
   */
  bool SetDatabase
      (const char * szDB );

  /*! @brief   Speed
      Returns the database speed.
      @return  A reference to the RTE speed object
   */
  const RTE_SpeedInfo & Speed
    ( );

  /*! @brief   Speed
      Returns the database speed.
      @param  szDB    [IN]  - null terminated name of database instance
      @return  A reference to the RTE speed object.
   */
  static const RTE_SpeedInfo & Speed
    ( const char              * szDB );

  /*! @brief   State
      Returns the database state.
      @return The binary state value. In case of errors it returns the value StateUndefined
              and the function DBMMsg() returns more information.
   */
  StateValue State
    ( );

  /*! \brief returns a string for current database state

      Note, that this function is not const, as it determines the
      state of the database each time it is called.

      \return state as zero terminated string */
  const char * getStateAsString();

  /*! \brief returns a string for the database state

      \param state [IN] state as enum
      \return state as zero terminated string */
  static const char * getStateAsString(StateValue state);

  /*! \brief returns a string for current database speed

      Note, that this function is not const, as it determines the
      speed of the database each time it is called.

      \return speed as zero terminated string */
  const char * getSpeedAsString();

  /*! \brief returns a string for the database state

      \param speed [IN] speed as value
      \return speed as zero terminated string */
  static const char * getSpeedAsString(RTE_SpeedInfo speed);

  /*! @brief   State
      Returns the database state.
      @param  szDB [IN] - null terminated name of database instance
      @return The binary state value. In case of errors it returns the value StateUndefined
              and the function DBMMsg() returns more information.
   */
  static StateValue State
    ( const char * szDB );

  /*! @brief   State
      Returns the verbose database state.
      @param  szState     [OUT] - object to contain the text representation of the state
      @param  bLogIsFull  [OUT] - flag is true if the log volume is full
      @param  bDataIsFull [OUT] - flag is true if the data volume is full
      @return The binary state value. In case of errors it returns the value StateUndefined
              and the function DBMMsg() returns more information.
   */
  StateValue State
    ( Tools_DynamicUTF8String & szState,
      bool                    & bLogIsFull,
      bool                    & bDataIsFull );

  /*! @brief   IsFull
      Returns the complete full state
      @param  bLogIsFull  [OUT] - flag is true if the log volume is full
      @param  bDataIsFull [OUT] - flag is true if the data volume is full
      @return true if log volume or data volme is full
   */
  bool IsFull
    ( bool                    & bLogIsFull,
      bool                    & bDataIsFull );

  /*! @brief   IsFull
      Returns the full state
      @return true if log volume or data volme is full
   */
  bool IsFull
    ( );

  /*! @brief   last DBM message
      Returns the last message list in case of errors
      @return reference to message object
   */
  const DBMSrvMsg_Error & DBMMsg() {return m_DBMMsg;}

  private:
    bool Refresh
      ( const RTE_Database::DBStatusInfo * & pStatus,
        Kernel_SharedState const         * & pExtendedState );
    bool Refresh
      ( const RTE_Database::DBStatusInfo * & pStatus );
    bool Refresh
      ( Kernel_SharedState const         * & pExtendedState );
    bool GetDatabase
      ( );

    RTE_ActiveDatabase      * m_pActiveDatabase;
    DBMSrvMsg_Error           m_DBMMsg;
    Tools_DynamicUTF8String   m_szDBName;
}; // end class DBMSrvKnl_State

#endif /* _DBMSrvKnl_State_HPP_ */
