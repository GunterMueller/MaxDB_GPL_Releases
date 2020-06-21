/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_Show.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager Client Library
  description:  Show Class
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
#include "DBM/Cli/DBMCli_Show.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define SHOW_IO         "IO"
#define SHOW_AIO        "AIO"
#define SHOW_STORAGE    "STORAGE"
#define SHOW_TASKS      "TASKS"
#define SHOW_ACTIVE     "ACTIVE"
#define SHOW_RUNNABLE   "RUNNABLE"
#define SHOW_T_C        "T_C"
#define SHOW_VERSIONS   "VERSIONS"
#define SHOW_REGIONS    "REGIONS"
#define SHOW_STATE      "STATE"
#define SHOW_RTE        "RTE"
#define SHOW_QUEUES     "QUEUES"
#define SHOW_SUSPENDS   "SUSPENDS"
#define SHOW_SLEEP      "SLEEP"
#define SHOW_THRD_TIMES "THRD_TIMES"
#define SHOW_PSE_STAT   "PSE_STAT"
#define SHOW_PSE_DATA   "PSE_DATA"
#define SHOW_ALL        "ALL"

#define DESC_IO         "Input/output activities in regular database operation"
#define DESC_AIO        "Input/output activities for data backup"
#define DESC_STORAGE    "Configuration and current status of the various storage areas"
#define DESC_TASKS      "List of tasks"
#define DESC_ACTIVE     "Active tasks for [Datawriter-Task|Server-Task|User-Task]"
#define DESC_RUNNABLE   "Runnable tasks (waiting for CPU)"
#define DESC_T_C        "Task-specific information for [Datawriter_Task|Server-Task|User-Task|Task Nr.x]"
#define DESC_VERSIONS   "Current variant of database kernel and runtime environment"
#define DESC_REGIONS    "Information about the critical sections used in the kernel"
#define DESC_STATE      "Mode of database instance"
#define DESC_RTE        "Runtime environment"
#define DESC_QUEUES     "Queues of runnable tasks (waiting for CPU)"
#define DESC_SUSPENDS   "Information about suspend states anywhere in the overall system"
#define DESC_SLEEP      "The CPU load from user kernel threads measured by the database kernel"
#define DESC_THRD_TIMES "Information from the system about the CPU load from user kernel threads"
#define DESC_PSE_STAT   "PSE statistics"
#define DESC_PSE_DATA   "PSE data page array"
#define DESC_ALL        "All of the information provided by the console"

/*    
  =============================================================================
  class: DBMCli_Show
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_Show :: DBMCli_Show
  -----------------------------------------------------------------------------
*/
DBMCli_Show :: DBMCli_Show ( )
{
} 

DBMCli_Show :: DBMCli_Show ( const DBMCli_String & sName,
                             const DBMCli_String & sDesc )
             : m_sName ( sName ),
               m_sDescription( sDesc )
{
}

/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_Show :: ~DBMCli_Show
  -----------------------------------------------------------------------------
*/
DBMCli_Show :: ~DBMCli_Show ( )
{
} 

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Show :: Open
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Show :: Open ( const DBMCli_String        & sName,
                                       Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();
  DBMCli_Result & oResult = oDB.GetResult( ); 

  DBMCli_String sCmd( DBMCLI_CMD_SHOW );
  sCmd += " ";
  sCmd += sName;

  if( oDB.Execute( sCmd, oMsgList ) ) {
    m_sName = sName;

    DBMCli_String sLine;

    // continue or end 
    oResult.GetLine( sLine ); 
    m_bContinue = ( sLine == DBMCLI_VAL_CONTINUE );

    bRC = true;
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Show :: GetLine
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Show :: GetLine ( DBMCli_String        & sLine,
                                    Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();
  DBMCli_Result & oResult = oDB.GetResult( ); 

  if( oResult.GetFullLine( sLine ) ) {
    bRC = true;
  } else {

    DBMCli_String sRest;
    oResult.GetLine( sRest ); 

    if( m_bContinue ) {
      if ( Next( oMsgList ) ) {
        if( oResult.GetLine( sLine ) ) {
          sLine = sRest + sLine;
          bRC = true;
        }
      }
    }
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Show :: GetPart
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Show :: GetPart ( DBMCli_String        & sPart,
                                    SAPDB_Int              nMaxLen,
                                    Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();
  DBMCli_Result & oResult = oDB.GetResult( ); 

  if( oResult.GetPart( sPart, nMaxLen ) ) {
    bRC = true;
  } else {
    if( m_bContinue ) {
      if ( Next( oMsgList ) ) {
        if( oResult.GetPart( sPart, nMaxLen ) ) {
          bRC = true;
        }
      }
    }
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Show :: Close
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Show :: Close ( Msg_List & oMsgList )
{
  return true;
}

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  private function DBMCli_Show :: Next
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Show :: Next ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();
  DBMCli_Result & oResult = oDB.GetResult( ); 

  DBMCli_String sCmd( DBMCLI_CMD_SHOWNEXT );

  if( oDB.Execute( sCmd, oMsgList ) ) {
    DBMCli_String sLine;

    // continue or end
    oResult.GetLine( sLine ); 
    m_bContinue = ( sLine == DBMCLI_VAL_CONTINUE );

    bRC = true;
  }

  return bRC;
}

/*    
  =============================================================================
  class: DBMCli_Shows
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_Shows :: DBMCli_Shows
  -----------------------------------------------------------------------------
*/
DBMCli_Shows :: DBMCli_Shows ( )
{
} 

/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_Shows :: ~DBMCli_Shows
  -----------------------------------------------------------------------------
*/
DBMCli_Shows :: ~DBMCli_Shows ( )
{
} 

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Shows :: Refresh
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Shows :: Refresh ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  m_aShow.RemoveAll( );

  DBMCli_Database & oDB = GetDatabase();
  DBMCli_Result & oResult = oDB.GetResult( ); 

  DBMCli_String sCmd( DBMCLI_CMD_SHOWLIST );

  if( oDB.Execute( sCmd, oMsgList ) ) {
    DBMCli_String sName;
    DBMCli_String sDesc;

    while( oResult.GetLine( sName ) ) {
      
      if( sName == SHOW_IO ) {
        sDesc = DESC_IO;
      } else if( sName == SHOW_AIO ) {
        sDesc = DESC_AIO;
      } else if( sName == SHOW_STORAGE) {
        sDesc = DESC_STORAGE;
      } else if( sName == SHOW_TASKS) {
        sDesc = DESC_TASKS;
      } else if( sName == SHOW_ACTIVE) {
        sDesc = DESC_ACTIVE;
      } else if( sName == SHOW_RUNNABLE) {
        sDesc = DESC_RUNNABLE;
      } else if( sName == SHOW_T_C) {
        sDesc = DESC_T_C;
      } else if( sName == SHOW_VERSIONS) {
        sDesc = DESC_VERSIONS;
      } else if( sName == SHOW_REGIONS) {
        sDesc = DESC_REGIONS;
      } else if( sName == SHOW_STATE) {
        sDesc = DESC_STATE;
      } else if( sName == SHOW_RTE) {
        sDesc = DESC_RTE;
      } else if( sName == SHOW_QUEUES) {
        sDesc = DESC_QUEUES;
      } else if( sName == SHOW_SUSPENDS) {
        sDesc = DESC_SUSPENDS;
      } else if( sName == SHOW_SLEEP) {
        sDesc = DESC_SLEEP;
      } else if( sName == SHOW_THRD_TIMES) {
        sDesc = DESC_THRD_TIMES;
      } else if( sName == SHOW_PSE_STAT) {
        sDesc = DESC_PSE_STAT;
      } else if( sName == SHOW_PSE_DATA) {
        sDesc = DESC_PSE_DATA;
      } else if( sName == SHOW_ALL) {
        sDesc = DESC_ALL;
      }

      DBMCli_Show oShow( sName, sDesc );
      oShow.SetDatabase( m_pDatabase );

      m_aShow.Add( oShow );

      sName.Empty( ); 
      sDesc.Empty( );
    }

    bRC = true;
  }

  return bRC;
}

