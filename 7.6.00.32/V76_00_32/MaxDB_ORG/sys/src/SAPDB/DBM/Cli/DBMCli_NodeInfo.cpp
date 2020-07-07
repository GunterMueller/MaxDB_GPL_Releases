/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_NodeInfo.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager Client Library
  description:  Node Info Class
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
#include "DBM/Cli/DBMCli_Node.hpp"
#include "DBM/Cli/DBMCli_NodeInfo.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define PROP_VERSION   "VERSION"
#define PROP_BUILD     "BUILD"
#define PROP_OS        "OS"
#define PROP_INSTROOT  "INSTROOT"
#define PROP_LOGON     "LOGON"
#define PROP_CODE      "CODE"
#define PROP_SWAP      "SWAP"

/*    
  =============================================================================
  class: DBMCli_NodeInfo
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_NodeInfo :: DBMCli_NodeInfo
  -----------------------------------------------------------------------------
*/
DBMCli_NodeInfo :: DBMCli_NodeInfo ( )
{
  Clear( );
}

/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_NodeInfo :: ~DBMCli_NodeInfo
  -----------------------------------------------------------------------------
*/
DBMCli_NodeInfo :: ~DBMCli_NodeInfo ( )
{
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_NodeInfo :: Refresh
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_NodeInfo :: Refresh ( Msg_List & oMsgList )
{
  DBMCli_Node & oNode = GetNode();

  SAPDB_Bool bConnect = false;
  SAPDB_Bool bRC =  false;

  Clear( );

  if( !oNode.IsConnected( ) ) {
    bRC = oNode.Connect( oMsgList );
    bConnect = bRC;
  } else {
    bRC = true;
  } 

  if( bRC ) {
    bRC = false;

    DBMCli_String sCmd( DBMCLI_CMD_DBMVERSION );

    if( oNode.Execute( sCmd, oMsgList ) ) {
      DBMCli_String sProp;
      DBMCli_String sVal;
      DBMCli_Result & oResult = oNode.GetResult( ); 

      while( oResult.GetPropVal( sProp, sVal ) ) {

        sProp.MakeUpper( );
        if (sProp != PROP_INSTROOT) {
          sVal.MakeUpper( );
        }
        AssignProp( sProp, sVal );
      }

      bRC = true;
    }
  }

  if( bConnect ) {
    oNode.Disconnect( );
  }

  return bRC;
} 

/*
  -----------------------------------------------------------------------------
  public function DBMCli_NodeInfo :: Clear
  -----------------------------------------------------------------------------
*/
void DBMCli_NodeInfo :: Clear ( )
{
  m_oVersion.Clear( );
  m_sBuild.Empty( );
  m_sOS.Empty( );
  m_sInstRoot.Empty( );
  m_bLogon = false;
  m_sCode.Empty( );
  m_sSwap.Empty( );
}

/*
  -----------------------------------------------------------------------------
  private functions...
  -----------------------------------------------------------------------------
*/

void DBMCli_NodeInfo :: AssignProp ( const DBMCli_String & sProp,
                                     const DBMCli_String & sVal )
{
  if( sProp == PROP_VERSION ) {
    m_oVersion.SetName( sVal );
  } else if( sProp == PROP_BUILD ) {
    m_sBuild = sVal;
    m_oVersion.SetBuild( sVal );
  } else if( sProp == PROP_OS ) {
    m_sOS = sVal;
  } else if( sProp == PROP_INSTROOT ) {
    m_sInstRoot = sVal;
  } else if( sProp == PROP_LOGON ) {
    AssignValBool( sVal, &m_bLogon );
  } else if( sProp == PROP_CODE ) {
    m_sCode = sVal;
  } else if( sProp == PROP_SWAP ) {
    m_sSwap = sVal;
  } 
}

void DBMCli_NodeInfo :: AssignValBool ( const DBMCli_String & sVal,
                                              SAPDB_Bool    * pVal )
{
  if( sVal == DBMCLI_VAL_TRUE ) {
    *pVal = true;
  } else if ( sVal == DBMCLI_VAL_YES ) {
    *pVal = true;
  } else if( sVal == DBMCLI_VAL_ON ) {
    *pVal = true;
  } else {
    *pVal = false;
  }
}
