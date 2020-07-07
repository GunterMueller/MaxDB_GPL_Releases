/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_State.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager Client Library
  description:  Database State Class
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

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "DBM/Cli/DBMCli_Database.hpp"
#include "DBM/Cli/DBMCli_State.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/

#define PROP_D_KB     "DATA           (KB)"
#define PROP_PD_KB    "PERM DATA      (KB)"
#define PROP_TD_KB    "TEMP DATA      (KB)"
#define PROP_D_PG     "DATA        (PAGES)"
#define PROP_PD_PG    "PERM DATA   (PAGES)"
#define PROP_TD_PG    "TEMP DATA   (PAGES)"
#define PROP_D_PC     "DATA            (%)"
#define PROP_PD_PC    "PERM DATA       (%)"
#define PROP_TD_PC    "TEMP DATA       (%)"
#define PROP_L_KB     "LOG            (KB)"
#define PROP_L_PG     "LOG         (PAGES)"
#define PROP_L_PC     "LOG             (%)"
#define PROP_SES      "SESSIONS"
#define PROP_SES_PC   "SESSIONS        (%)"
#define PROP_DC_PC    "DATA CACHE      (%)"
#define PROP_CC_PC    "CONVERTER CACHE (%)"
#define PROP_DM_KB    "DATA MAX       (KB)"
#define PROP_LM_KB    "LOG MAX        (KB)"
#define PROP_DN_PG    "DATA MAX    (PAGES)"
#define PROP_LM_PG    "LOG MAX     (PAGES)"
#define PROP_SM       "SESSIONS MAX"
#define PROP_DB_FULL  "DATABASE FULL"
#define PROP_CON_POS  "CONNECT POSSIBLE"
#define PROP_CMD_MON  "COMMAND MONITORING"
#define PROP_DB_MON   "DATABASE MONITORING"
#define PROP_KNL_TRC  "KERNEL TRACE"
#define PROP_AUTOLOG  "AUTOSAVE"
#define PROP_BAD_IDX  "BAD INDEXES"
/*    
  =============================================================================
  class: DBMCli_State
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_State :: DBMCli_State
  -----------------------------------------------------------------------------
*/
DBMCli_State :: DBMCli_State ( )
{
  Clear( );
} 

/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_State :: ~DBMCli_State
  -----------------------------------------------------------------------------
*/
DBMCli_State :: ~DBMCli_State ( )
{
} 

/*
  -----------------------------------------------------------------------------
  public function DBMCli_State :: Clear
  -----------------------------------------------------------------------------
*/
void DBMCli_State :: Clear ( )
{
  m_nValue = DBMCLI_DBSTATE_UNKNOWN;
  m_sValue = DBMCLI_DBSTATE_UNKNOWN_STR;

  m_nDataKB = 0;
  m_nPermDataKB = 0;
  m_nTempDataKB = 0;

  m_nDataPages = 0;
  m_nPermDataPages = 0;
  m_nTempDataPages = 0;

  m_nDataPercent = 0;
  m_nPermDataPercent = 0;
  m_nTempDataPercent = 0;

  m_nLogKB = 0;
  m_nLogPages = 0;
  m_nLogPercent = 0;

  m_nSessions = 0;
  m_nSessionsPercent = 0;

  m_nDataCacheHitRate = 0;
  m_nConverterCacheHitRate = 0;

  m_nDataMaxKB = 0;
  m_nLogMaxKB = 0;

  m_nDataMaxPages = 0;
  m_nLogMaxPages = 0;

  m_nSessionsMax = 0;
    
  m_bDatabaseFull = false;
  m_bConnectPossible = false;
  m_bCommandMonitoring = false;
  m_bDatabaseMonitoring = false;
  m_bKernelTrace = false;
  m_bAutoLog = false;

  m_nBadIndexes = 0;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_State :: Refresh
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_State :: Refresh ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();

  DBMCli_String sCmd( DBMCLI_CMD_DBSTATE );

  Clear( );
  
  if ( oDB.Execute( sCmd, oMsgList ) ) {
    DBMCli_Result & oResult = oDB.GetResult( ); 
    DBMCli_String sState;
    if( oResult.GetLine( sState ) ) {
      if( oResult.GetLine( sState ) ) {

        if( sState == DBMCLI_DBSTATE_OFFLINE_STR ) {
          m_nValue = DBMCLI_DBSTATE_OFFLINE;
          m_sValue = DBMCLI_DBSTATE_OFFLINE_STR;
        } else if( sState == DBMCLI_DBSTATE_COLD_STR || sState == DBMCLI_DBSTATE_ADMIN_STR) {
          m_nValue = DBMCLI_DBSTATE_COLD;
          m_sValue = DBMCLI_DBSTATE_ADMIN_STR;
        } else if( sState == DBMCLI_DBSTATE_WARM_STR || sState == DBMCLI_DBSTATE_ONLINE_STR) {
          m_nValue = DBMCLI_DBSTATE_WARM;
          m_sValue = DBMCLI_DBSTATE_ONLINE_STR;
        } else if( sState == DBMCLI_DBSTATE_STANDBY_STR) {
          m_nValue = DBMCLI_DBSTATE_STANDBY;
          m_sValue = DBMCLI_DBSTATE_STANDBY_STR;
        } else {
          m_nValue = DBMCLI_DBSTATE_UNKNOWN;
          m_sValue = DBMCLI_DBSTATE_UNKNOWN_STR;
        }

        bRC = true;
      }
    }
  }
   
  if( m_nValue == DBMCLI_DBSTATE_WARM ) {

    bRC = false;

    if( oDB.SQLConnect( oMsgList ) ) {
    
      DBMCli_String sCmd( DBMCLI_CMD_INFO );
      sCmd += " ";
      sCmd += DBMCLI_OPT_INFO_STATE;

      if( oDB.Execute( sCmd, oMsgList ) ){

        DBMCli_String sProp;
        DBMCli_String sVal;
        DBMCli_Result & oResult = oDB.GetResult( ); 

        while( oResult.GetPropVal( sProp, sVal ) ) {

          sProp.MakeUpper( );
          sVal.MakeUpper( );

          AssignProp( sProp, sVal );
        }

        bRC = true;
      }

      oDB.SQLRelease( oMsgList );
    }

    if( m_bAutoLog ) {
      oDB.GetAutolog( ).SetState( DBMCLI_AUTOLOGSTATE_ON );
    } else {
      oDB.GetAutolog( ).SetState( DBMCLI_AUTOLOGSTATE_OFF );
    }

  } else {
    oDB.GetAutolog( ).SetState( DBMCLI_AUTOLOGSTATE_UNKNOWN );
  }

  return bRC;
} 

void DBMCli_State :: AssignProp ( const DBMCli_String & sProp,
                                  const DBMCli_String & sVal )
{
  if( sProp == PROP_D_KB ) {
    AssignValInt( sVal, &m_nDataKB );
  } else if( sProp == PROP_PD_KB ) {
    AssignValInt( sVal, &m_nPermDataKB );
  } else if( sProp == PROP_TD_KB ) {
    AssignValInt( sVal, &m_nTempDataKB );
  } else if( sProp == PROP_D_PG ) {
    AssignValInt( sVal, &m_nDataPages );
  } else if( sProp == PROP_PD_PG ) {
    AssignValInt( sVal, &m_nPermDataPages );
  } else if( sProp == PROP_TD_PG ) {
    AssignValInt( sVal, &m_nTempDataPages );
  } else if( sProp == PROP_D_PC ) {
    AssignValInt( sVal, &m_nDataPercent );
  } else if( sProp == PROP_PD_PC ) {
    AssignValInt( sVal, &m_nPermDataPercent );
  } else if( sProp == PROP_TD_PC ) {
    AssignValInt( sVal, &m_nTempDataPercent );
  } else if( sProp == PROP_L_KB ) {
    AssignValInt( sVal, &m_nLogKB );
  } else if( sProp == PROP_L_PG ) {
    AssignValInt( sVal, &m_nLogPages );
  } else if( sProp == PROP_L_PC ) {
    AssignValInt( sVal, &m_nLogPercent );
  } else if( sProp == PROP_SES ) {
    AssignValInt( sVal, &m_nSessions );
  } else if( sProp == PROP_SES_PC ) {
    AssignValInt( sVal, &m_nSessionsPercent );
  } else if( sProp == PROP_DC_PC ) {
    AssignValInt( sVal, &m_nDataCacheHitRate );
  } else if( sProp == PROP_CC_PC ) {
    AssignValInt( sVal, &m_nConverterCacheHitRate );
  } else if( sProp == PROP_DM_KB ) {
    AssignValInt( sVal, &m_nDataMaxKB );
  } else if( sProp == PROP_LM_KB ) {
    AssignValInt( sVal, &m_nLogMaxKB );
  } else if( sProp == PROP_DN_PG ) {
    AssignValInt( sVal, &m_nDataMaxPages );
  } else if( sProp == PROP_LM_PG ) {
    AssignValInt( sVal, &m_nLogMaxKB );
  } else if( sProp == PROP_SM ) {
    AssignValInt( sVal, &m_nSessionsMax );
  } else if( sProp == PROP_DB_FULL ) {
    AssignValBool( sVal, &m_bDatabaseFull );
  } else if( sProp == PROP_CON_POS ) {
    AssignValBool( sVal, &m_bConnectPossible );
  } else if( sProp == PROP_CMD_MON ) {
    AssignValBool( sVal, &m_bCommandMonitoring );
  } else if( sProp == PROP_DB_MON ) {
    AssignValBool( sVal, &m_bDatabaseMonitoring );
  } else if( sProp == PROP_KNL_TRC ) {
    AssignValBool( sVal, &m_bKernelTrace );
  } else if( sProp == PROP_AUTOLOG ) {
    AssignValBool( sVal, &m_bAutoLog );
  } else if( sProp == PROP_BAD_IDX ) {
    AssignValInt( sVal, &m_nBadIndexes );
  }
}

void DBMCli_State :: AssignValInt ( const DBMCli_String & sVal,
                                          SAPDB_Int     * pVal )
{
  if( !sVal.IsEmpty( ) ) {
    *pVal = atoi( sVal );
  }
}

void DBMCli_State :: AssignValBool ( const DBMCli_String & sVal,
                                           SAPDB_Bool    * pVal )
{
  if( sVal == DBMCLI_VAL_YES ) {
    *pVal = true;
  } else if( sVal == DBMCLI_VAL_ON ) {
    *pVal = true;
  } else {
    *pVal = false;
  }
}