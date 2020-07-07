/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_Autolog.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager Client Library
  description:  Autolog
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
#include "DBM/Cli/DBMCli_Autolog.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define AUTOLOG_ON "AUTOSAVE IS ON"
#define AUTOLOG_OFF "AUTOSAVE IS OFF"

/*    
  =============================================================================
  class: DBMCli_Autolog
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_Autolog :: DBMCli_Autolog
  -----------------------------------------------------------------------------
*/
DBMCli_Autolog :: DBMCli_Autolog ( )
{
  Clear( );
} 

/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_Autolog :: ~DBMCli_Autolog
  -----------------------------------------------------------------------------
*/
DBMCli_Autolog :: ~DBMCli_Autolog ( )
{
} 

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Autolog :: Clear
  -----------------------------------------------------------------------------
*/
void DBMCli_Autolog :: Clear ( )
{
  SetState( DBMCLI_AUTOLOGSTATE_UNKNOWN );
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Autolog :: Refresh
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Autolog :: Refresh ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  Clear( );

  DBMCli_Database & oDB = GetDatabase();
  DBMCli_Result & oResult = oDB.GetResult( ); 

  DBMCli_String sCmd( DBMCLI_CMD_AUTOLOG_SHOW );

  if( oDB.Execute( sCmd, oMsgList ) ) {
    DBMCli_String sState;
    if( oResult.GetLine( sState ) ) {
      if( sState == AUTOLOG_ON ) {
        SetState( DBMCLI_AUTOLOGSTATE_ON );
      } else if( sState == AUTOLOG_OFF ) {
        SetState( DBMCLI_AUTOLOGSTATE_OFF );
      }
    }
    bRC = true;
  }

  return bRC;
} 

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Autolog :: On
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Autolog :: On ( const DBMCli_String & sMedium,
                                  Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();
  DBMCli_Result & oResult = oDB.GetResult( ); 

  DBMCli_String sCmd( DBMCLI_CMD_AUTOLOG_ON );
  sCmd += " \"";
  sCmd += sMedium;
  sCmd += "\"";

  if( oDB.Execute( sCmd, oMsgList ) ) {
    SetState( DBMCLI_AUTOLOGSTATE_ON );
    bRC = true;
  }

  return bRC;
} 

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Autolog :: Off
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Autolog :: Off ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();

  DBMCli_String sCmd( DBMCLI_CMD_AUTOLOG_OFF );

  if( oDB.Execute( sCmd, oMsgList ) ) {
    SetState( DBMCLI_AUTOLOGSTATE_OFF );
    bRC = true;
  }

  return bRC;
} 

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Autolog :: Cancel
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Autolog :: Cancel ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();

  DBMCli_String sCmd( DBMCLI_CMD_AUTOLOG_CANCEL );

  if ( oDB.Execute( sCmd, oMsgList ) ) {
    bRC = true;
  }

  return bRC;
} 

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Autolog :: SetState
  -----------------------------------------------------------------------------
*/
void DBMCli_Autolog :: SetState ( DBMCli_AutologState nState )
{
  m_nState = nState;

  switch( m_nState ) {
    case DBMCLI_AUTOLOGSTATE_ON:
      m_sState = "ON";
      break;
    case DBMCLI_AUTOLOGSTATE_OFF:
      m_sState = "OFF";
      break;
    default:
      m_sState = "UNKNOWN";
      break;
  }
}
