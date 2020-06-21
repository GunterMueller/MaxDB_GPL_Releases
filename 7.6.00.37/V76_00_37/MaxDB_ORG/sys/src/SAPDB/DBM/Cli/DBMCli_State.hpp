/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_State.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager Client Library
  description:  Database State
  version:      7.3 and higher 
  -----------------------------------------------------------------------------
                          Copyright (c) 2000-2005 SAP AG
  -----------------------------------------------------------------------------



    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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

#ifndef _DBMCLI_STATE_HPP_
#define _DBMCLI_STATE_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "DBM/Cli/DBMCli_Common.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define DBMCLI_DBSTATE_UNKNOWN_STR "UNKNOWN"
#define DBMCLI_DBSTATE_OFFLINE_STR "OFFLINE"
#define DBMCLI_DBSTATE_COLD_STR    "COLD"
#define DBMCLI_DBSTATE_ADMIN_STR   "ADMIN"
#define DBMCLI_DBSTATE_WARM_STR    "WARM"
#define DBMCLI_DBSTATE_ONLINE_STR  "ONLINE"
#define DBMCLI_DBSTATE_STANDBY_STR "STANDBY"

/*
  -----------------------------------------------------------------------------
  typedefs
  -----------------------------------------------------------------------------
*/
typedef enum 
{ 
	DBMCLI_DBSTATE_UNKNOWN = 0,
	DBMCLI_DBSTATE_OFFLINE = 1,
	DBMCLI_DBSTATE_COLD = 2,
	DBMCLI_DBSTATE_WARM = 4,
	DBMCLI_DBSTATE_STANDBY = 5
} DBMCli_DBState_Constants;

/*
  -----------------------------------------------------------------------------
  froward declarations
  -----------------------------------------------------------------------------
*/
class DBMCli_Database;

/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_State
  -----------------------------------------------------------------------------
  description: Class to handle the general database state.
  -----------------------------------------------------------------------------
*/
class DBMCli_State 
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_State
      -------------------------------------------------------------------------
      description:  Constructor
      -------------------------------------------------------------------------
    */
    DBMCli_State ( );

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_State
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_State ( );

    /*!
      -------------------------------------------------------------------------
      function:     SetDatabase
      -------------------------------------------------------------------------
      description:  Set the database member.
      -------------------------------------------------------------------------
    */
    void SetDatabase ( DBMCli_Database * p ) { m_pDatabase = p; };

    /*!
      -------------------------------------------------------------------------
      function:     GetDatabase
      -------------------------------------------------------------------------
      description:  Get the database member.
      return value: Reference to the database object.
      -------------------------------------------------------------------------
    */
    DBMCli_Database & GetDatabase ( ) { return *m_pDatabase; };

    /*!
      -------------------------------------------------------------------------
      function:     Clear
      -------------------------------------------------------------------------
      description:  Clear the state properties.
      -------------------------------------------------------------------------
    */
    void Clear ( );

    /*!
      -------------------------------------------------------------------------
      function:     Refresh
      -------------------------------------------------------------------------
      description:  Refresh the state properties.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Refresh ( Msg_List & oMsgList );

    /*!  
      Chapter: Properties
    */
    
    /*!
      -------------------------------------------------------------------------
      function:     Value
      -------------------------------------------------------------------------
    */
    DBMCli_DBState_Constants Value ( ) { return m_nValue; };

    /*!
      -------------------------------------------------------------------------
      function:     ValueStr
      -------------------------------------------------------------------------
    */
    const DBMCli_String & ValueStr ( ) { return m_sValue; };

    /*!
      -------------------------------------------------------------------------
      function:     DataKB
      -------------------------------------------------------------------------
    */
    SAPDB_Int DataKB ( ) { return m_nDataKB; };

    /*!
      -------------------------------------------------------------------------
      function:     PermDataKB
      -------------------------------------------------------------------------
    */
    SAPDB_Int PermDataKB ( ) { return m_nPermDataKB; };

    /*!
      -------------------------------------------------------------------------
      function:     TempDataKB
      -------------------------------------------------------------------------
    */
    SAPDB_Int TempDataKB ( ) { return m_nTempDataKB; };
    
    /*!
      -------------------------------------------------------------------------
      function:     DataPages
      -------------------------------------------------------------------------
    */
    SAPDB_Int DataPages ( ) { return m_nDataPages; };

    /*!
      -------------------------------------------------------------------------
      function:     PermDataPages
      -------------------------------------------------------------------------
    */
    SAPDB_Int PermDataPages ( ) { return m_nPermDataPages; };

    /*!
      -------------------------------------------------------------------------
      function:     TempDataPages
      -------------------------------------------------------------------------
    */
    SAPDB_Int TempDataPages ( ) { return m_nTempDataPages; };
    
    /*!
      -------------------------------------------------------------------------
      function:     DataPercent
      -------------------------------------------------------------------------
    */
    SAPDB_Int DataPercent ( ) { return m_nDataPercent; };

    /*!
      -------------------------------------------------------------------------
      function:     PermDataPercent
      -------------------------------------------------------------------------
    */
    SAPDB_Int PermDataPercent ( ) { return m_nPermDataPercent; };

    /*!
      -------------------------------------------------------------------------
      function:     TempDataPercent
      -------------------------------------------------------------------------
    */
    SAPDB_Int TempDataPercent ( ) { return m_nTempDataPercent; };

    /*!
      -------------------------------------------------------------------------
      function:     LogKB
      -------------------------------------------------------------------------
    */
    SAPDB_Int LogKB ( ) { return m_nLogKB; };

    /*!
      -------------------------------------------------------------------------
      function:     LogPages
      -------------------------------------------------------------------------
    */
    SAPDB_Int LogPages ( ) { return m_nLogPages; };

    /*!
      -------------------------------------------------------------------------
      function:     LogPercent
      -------------------------------------------------------------------------
    */
    SAPDB_Int LogPercent ( ) { return m_nLogPercent; };

    /*!
      -------------------------------------------------------------------------
      function:     Sessions
      -------------------------------------------------------------------------
    */
    SAPDB_Int Sessions ( ) { return m_nSessions; };

    /*!
      -------------------------------------------------------------------------
      function:     SessionsPercent
      -------------------------------------------------------------------------
    */
    SAPDB_Int SessionsPercent ( ) { return m_nSessionsPercent; };

    /*!
      -------------------------------------------------------------------------
      function:     DataCacheHitRate
      -------------------------------------------------------------------------
    */
    SAPDB_Int DataCacheHitRate ( ) { return m_nDataCacheHitRate; };

    /*!
      -------------------------------------------------------------------------
      function:     ConverterCacheHitRate
      -------------------------------------------------------------------------
    */
    SAPDB_Int ConverterCacheHitRate ( ) { return m_nConverterCacheHitRate; };

    /*!
      -------------------------------------------------------------------------
      function:     DataMaxKB
      -------------------------------------------------------------------------
    */
    SAPDB_Int DataMaxKB ( ) { return m_nDataMaxKB; };

    /*!
      -------------------------------------------------------------------------
      function:     LogMaxKB
      -------------------------------------------------------------------------
    */
    SAPDB_Int LogMaxKB ( ) { return m_nLogMaxKB; };

    /*!
      -------------------------------------------------------------------------
      function:     DataMaxPages
      -------------------------------------------------------------------------
    */
    SAPDB_Int DataMaxPages ( ) { return m_nDataMaxPages; };

    /*!
      -------------------------------------------------------------------------
      function:     LogMaxPages
      -------------------------------------------------------------------------
    */
    SAPDB_Int LogMaxPages ( ) { return m_nLogMaxPages; };

    /*!
      -------------------------------------------------------------------------
      function:     SessionsMax
      -------------------------------------------------------------------------
    */
    SAPDB_Int SessionsMax ( ) { return m_nSessionsMax; };

    /*!
      -------------------------------------------------------------------------
      function:     DatabaseFull
      -------------------------------------------------------------------------
    */
    SAPDB_Bool DatabaseFull ( ) { return m_bDatabaseFull; };

    /*!
      -------------------------------------------------------------------------
      function:     ConnectPossible
      -------------------------------------------------------------------------
    */
    SAPDB_Bool ConnectPossible ( ) { return m_bConnectPossible; };

    /*!
      -------------------------------------------------------------------------
      function:     CommandMonitoring
      -------------------------------------------------------------------------
    */
    SAPDB_Bool CommandMonitoring ( ) { return m_bCommandMonitoring; };

    /*!
      -------------------------------------------------------------------------
      function:     DatabaseMonitoring
      -------------------------------------------------------------------------
    */
    SAPDB_Bool DatabaseMonitoring ( ) { return m_bDatabaseMonitoring; };

    /*!
      -------------------------------------------------------------------------
      function:     KernelTrace
      -------------------------------------------------------------------------
    */
    SAPDB_Bool KernelTrace ( ) { return m_bKernelTrace; };

    /*!
      -------------------------------------------------------------------------
      function:     AutoLog
      -------------------------------------------------------------------------
    */
    SAPDB_Bool AutoLog ( ) { return m_bAutoLog; };

    /*!
      -------------------------------------------------------------------------
      function:     BadIndexes
      -------------------------------------------------------------------------
    */
    SAPDB_Int  BadIndexes ( ) { return m_nBadIndexes; };

    /*!  
      EndChapter: Properties
    */

  private:
    void AssignProp ( const DBMCli_String & sProp,
                      const DBMCli_String & sVal );

    void AssignValInt ( const DBMCli_String & sVal,
                              SAPDB_Int       * pVal );

    void AssignValBool ( const DBMCli_String & sVal,
                               SAPDB_Bool    * pVal );


  private:
    DBMCli_Database * m_pDatabase;

    DBMCli_DBState_Constants m_nValue;
    DBMCli_String m_sValue;

    SAPDB_Int m_nDataKB;
    SAPDB_Int m_nPermDataKB;
    SAPDB_Int m_nTempDataKB;

    SAPDB_Int m_nDataPages;
    SAPDB_Int m_nPermDataPages;
    SAPDB_Int m_nTempDataPages;

    SAPDB_Int m_nDataPercent;
    SAPDB_Int m_nPermDataPercent;
    SAPDB_Int m_nTempDataPercent;

    SAPDB_Int m_nLogKB;
    SAPDB_Int m_nLogPages;
    SAPDB_Int m_nLogPercent;

    SAPDB_Int m_nSessions;
    SAPDB_Int m_nSessionsPercent;

    SAPDB_Int m_nDataCacheHitRate;
    SAPDB_Int m_nConverterCacheHitRate;

    SAPDB_Int m_nDataMaxKB;
    SAPDB_Int m_nLogMaxKB;

    SAPDB_Int m_nDataMaxPages;
    SAPDB_Int m_nLogMaxPages;

    SAPDB_Int m_nSessionsMax;
    
    SAPDB_Bool m_bDatabaseFull;
    SAPDB_Bool m_bConnectPossible;
    SAPDB_Bool m_bCommandMonitoring;
    SAPDB_Bool m_bDatabaseMonitoring;
    SAPDB_Bool m_bKernelTrace;
    SAPDB_Bool m_bAutoLog;

    SAPDB_Int m_nBadIndexes;
}; 

/*! EndClass: DBMCli_State */

#endif // _DBMCLI_STATE_HPP_

