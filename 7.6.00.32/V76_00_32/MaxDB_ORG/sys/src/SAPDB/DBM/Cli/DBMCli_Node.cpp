/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_Node.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager Client Library
  description:  Node Class
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

/*    
  =============================================================================
  class:        DBMCli_Node
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_Node :: DBMCli_Node
  -----------------------------------------------------------------------------
*/
DBMCli_Node :: DBMCli_Node ( )
{
  m_oNodeInfo.SetNode(this);
} 

DBMCli_Node :: DBMCli_Node ( const DBMCli_String        & sServer,
                                   Msg_List & oMsgList,
                             const SAPDB_Bool             bConnect )
             : DBMCli_Session( sServer, oMsgList, bConnect )
{
  m_oNodeInfo.SetNode(this);
} 
/*
  -----------------------------------------------------------------------------
 */
DBMCli_Node :: DBMCli_Node ( const DBMCli_String        & sServer,
                             const DBMCli_String        & sDatabase,
                             const DBMCli_String        & sUser,
                                   Msg_List & oMsgList,
                             const SAPDB_Bool             bConnect )
             : DBMCli_Session (sServer, sDatabase, sUser, oMsgList, bConnect)
{
  m_oNodeInfo.SetNode(this);
} 

/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_Node :: ~DBMCli_Node
  -----------------------------------------------------------------------------
 */
DBMCli_Node :: ~DBMCli_Node ( )
{
} 

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Node :: EnumerateInstallations
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Node :: EnumerateInstallations ( Msg_List & oMsgList )
{
  SAPDB_Bool bConnect = false;
  SAPDB_Bool bRC =  false;

  m_aEnumInstallation.RemoveAll( );

  if( !IsConnected( ) ) {
    bRC = Connect( oMsgList );
    bConnect = bRC;
  } else {
    bRC = true;
  } 

  if( bRC ) {
    bRC = false;
    if( m_oNodeInfo.Refresh( oMsgList ) ) {

      DBMCli_EnumInstallation oEnumInst( m_oNodeInfo.Version( ).GetName( ), m_oNodeInfo.InstRoot( ) );
      m_aEnumInstallation.Add( oEnumInst );

      bRC = true;
    }
  }

  if( bRC ) {
    bRC = false;

    DBMCli_String sCmd( DBMCLI_CMD_INSTENUM );

    if( Execute( sCmd, oMsgList ) ) {
      DBMCli_String sVersion;
      DBMCli_String sInstRoot;
      DBMCli_Result & oResult = GetResult( ); 
      SAPDB_Bool bExists = false;

      while( oResult.GetField( sVersion, " \t\n" ) ) {
        oResult.GetField( sInstRoot, "\t\n" );

        sVersion.Trim( );
        sInstRoot.Trim( );

        bExists = false;

        for( SAPDB_Int nItem = 0; 
             ( nItem < m_aEnumInstallation.GetSize( ) ) && ( bExists == false ); 
             nItem++ ) {

          if( m_aEnumInstallation[nItem].InstRoot( ) == sInstRoot ) {
            bExists = true;
          }
        }

        if( !bExists ) {
          DBMCli_EnumInstallation oEnumInst( sVersion, sInstRoot );
          m_aEnumInstallation.Add( oEnumInst );
        }
      }

      bRC = true;
    }
  }

  if( bConnect ) {
    Disconnect( );
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Node :: EnumerateDatabases
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Node :: EnumerateDatabases ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;
  SAPDB_Bool bConnect = false;

  m_aEnumDatabase.RemoveAll( );

  if( !IsConnected( ) ) {
    bRC = Connect( oMsgList );
    bConnect = bRC;
  } else {
    bRC = true;
  } 

  if( bRC ) {
    bRC = false;

    DBMCli_String sCmd( DBMCLI_CMD_DBENUM );

    if( Execute( sCmd, oMsgList ) ) {
      DBMCli_String sName;
      DBMCli_String sInstRoot;
      DBMCli_String sVersion;
      DBMCli_String sKernel;
      DBMCli_String sState;
      DBMCli_Result & oResult = GetResult( ); 

      DBMCli_Version oVersion;

      while( oResult.GetField( sName, "\t\n" ) ) {
        if( oResult.GetField( sInstRoot, "\t\n" ) )
          if( oResult.GetField( sVersion, "\t\n" ) )
            if( oResult.GetField( sKernel, "\t\n" ) )
              oResult.GetField( sState, "\t\n" );

        oVersion.SetName( sVersion );

        if( oVersion.GetMajor() >= 7 ) {
          DBMCli_EnumDatabase oEnumDB( sName, sInstRoot, sVersion, sKernel, sState );
          m_aEnumDatabase.Add( oEnumDB );
        }
      }

      bRC = true;
    }
  }

  if( bConnect ) {
    Disconnect( );
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Media :: DatabaseIndexByName
  -----------------------------------------------------------------------------
*/
SAPDB_Int DBMCli_Node :: DatabaseIndexByName ( const DBMCli_String & sName )
{
  SAPDB_Bool bFound = false;
  SAPDB_Int nDatabase = 0;

  while( nDatabase < m_aEnumDatabase.GetSize( ) && !bFound ) {
    if( m_aEnumDatabase[nDatabase].Name( ).CompareNoCase( sName ) == 0 ) {
      bFound = true;
    } else {
      nDatabase++;
    }
  }

  if( bFound ) {
    return nDatabase;
  } else {
    return -1;
  }
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Node :: CreateDatabase 
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Node :: CreateDatabase ( const DBMCli_String & sServerName,
                                           const DBMCli_String & sDatabaseName,
                                           const DBMCli_String & sUserName,
                                           const DBMCli_String & sPassword,
                                           const DBMCli_String & sOSUser,
                                           const DBMCli_String & sOSPassword,
                                           const DBMCli_String & sInstRoot,
                                           Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;
  SAPDB_Bool bConnect = false;

  if( IsConnected( ) ) {
    Disconnect( );
  }

  if( !IsConnected( ) ) {
    SetServerName( sServerName );
    SetInstRoot( sInstRoot );
    bRC = Connect( oMsgList );
    bConnect = bRC;
  } else {
    bRC = true;
  } 

  if( bRC ) {
    bRC = false;

    DBMCli_String sCmd( DBMCLI_CMD_DBCREATE );

    sCmd += " ";
    sCmd += sDatabaseName;
    sCmd += " ";
    sCmd += sUserName;
    sCmd += ",";
    sCmd += sPassword;

    if( !sOSUser.IsEmpty( ) ) {
      sCmd += " ";
      sCmd += sOSUser;
      sCmd += ",";
      sCmd += sOSPassword;
    }

    if( Execute( sCmd, oMsgList ) ) {
      bRC = true;
    }
  }

  if( bConnect ) {
    Disconnect( );
    SetServerName( "" );
    SetInstRoot( "" );
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Node :: DropDatabase 
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Node :: DropDatabase ( const DBMCli_String & sDatabaseName,
                                         const DBMCli_String & sUserName,
                                         const DBMCli_String & sPassword,
                                         Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;
  SAPDB_Bool bConnect = false;

  DBMCli_String sUser( sUserName );

  if( !sPassword.IsEmpty( ) ) {
    sUser += ",";
    sUser += sPassword;
  }

  if( IsConnected( ) ) {
    Disconnect( );
  }

  SetDatabaseName( sDatabaseName );
  SetUserName( sUser );

  if( !IsConnected( ) ) {
    bRC = Connect( oMsgList );
    bConnect = bRC;
  } else {
    bRC = true;
  } 

  if( bRC ) {
    bRC = false;

    DBMCli_String sCmd( DBMCLI_CMD_DBDROP );

    if( Execute( sCmd, oMsgList ) ) {
      bRC = true;
    }
  }

  if( bConnect ) {
    Disconnect( );
  }

  SetUserName( "" );
  SetDatabaseName( "" );

  return bRC;
}