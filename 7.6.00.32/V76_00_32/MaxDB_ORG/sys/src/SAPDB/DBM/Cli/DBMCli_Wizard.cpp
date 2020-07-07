/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_Wizard.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Wizard Manager Client Library
  description:  Wizard Class
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
#include "DBM/Cli/DBMCli_Wizard.hpp"

/*    
  =============================================================================
  class: DBMCli_Wizard
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_Wizard :: DBMCli_Wizard
  -----------------------------------------------------------------------------
*/
DBMCli_Wizard :: DBMCli_Wizard ( )
{
  Clear( );
}

/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_Wizard :: ~DBMCli_Wizard
  -----------------------------------------------------------------------------
*/
DBMCli_Wizard :: ~DBMCli_Wizard ( )
{
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Wizard :: Clear
  -----------------------------------------------------------------------------
*/
void DBMCli_Wizard :: Clear ( )
{
  m_sDatabaseName.Empty( );
  m_sServerName.Empty( );
  m_sOSUserName.Empty( );
  m_sOSPassword.Empty( );

  m_bReinstall = false;

  m_oVersion.Clear( );
  m_sInstRoot.Empty( );

  m_sDBMName.Empty( );
  m_sDBMPwd.Empty( );
  m_sDBAName.Empty( );
  m_sDBAPwd.Empty( );

  m_sInitMode.Empty( );

  m_sInstanceType.Empty( );

  m_bCreated = false;
  m_bConnected = false;

  m_sInstallMode.Empty( );
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Wizard :: CreateDatabase
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Wizard :: CreateDatabase ( DBMCli_Database ** ppDatabase,
                                             Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;
  
  DBMCli_Node oNode;

  if( *ppDatabase != NULL ) {
    delete *ppDatabase;
    *ppDatabase = NULL;
  }

  m_bCreated = false;
  m_bConnected = false;

  m_sInitMode = DBMCLI_WIZARD_INITMODE_DEF;

  if( oNode.CreateDatabase( m_sServerName,
                            m_sDatabaseName,
                            m_sDBMName,
                            m_sDBMPwd,
                            m_sOSUserName,
                            m_sOSPassword,
                            m_sInstRoot,
                            oMsgList ) ) {

    m_bCreated = true;

    DBMCli_String sUser( m_sDBMName );

    if( !m_sDBMPwd.IsEmpty( ) ) {
      sUser += ",";
      sUser += m_sDBMPwd;
    }

    *ppDatabase = new DBMCli_Database(m_sServerName, m_sDatabaseName, sUser, oMsgList);

    if ( oMsgList.IsEmpty() ) {
      m_bConnected = true;
      bRC = true;
    }
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Wizard :: UseDatabase
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Wizard :: UseDatabase ( DBMCli_Database ** ppDatabase,
                                          Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;
  
  DBMCli_Node oNode;

  if( *ppDatabase != NULL ) {
    delete *ppDatabase;
    *ppDatabase = NULL;
  }

  m_bCreated = false;
  m_bConnected = false;

  m_sInitMode = DBMCLI_WIZARD_INITMODE_CUR;

  DBMCli_String sUser( m_sDBMName );

  if( !m_sDBMPwd.IsEmpty( ) ) {
    sUser += ",";
    sUser += m_sDBMPwd;
  }

  *ppDatabase = new DBMCli_Database(m_sServerName, m_sDatabaseName, sUser, oMsgList);

  if ( oMsgList.IsEmpty() ) {
    m_bConnected = true;
    bRC = true;
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Wizard :: InstallDatabase
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Wizard :: InstallDatabase ( DBMCli_Database * pDatabase,
                                              Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  m_bStart      = false;
  m_bInitConfig = false;
  m_bActivate   = false;
  m_bLoadSysTab = false;

  if( pDatabase != NULL ) {

    // get version info
    if( pDatabase->NodeInfo().Refresh( oMsgList ) ) {

      // start database
      if( pDatabase->Start( oMsgList ) ) {
        m_bStart = true;

        // init configuration
        if( pDatabase->InitConfig( oMsgList ) ) {
          m_bInitConfig = true;

          if( m_sInstallMode == DBMCLI_WIZARD_INSTALLMODE_INSTALL ) {

            // activate 
            if( pDatabase->Activate( m_sDBAName, m_sDBAPwd, oMsgList ) ) {
              m_bActivate = true;

              // load system tables 
              if( pDatabase->LoadSysTab( m_sDBAName, m_sDBAPwd, m_sDBAPwd, oMsgList ) ) {
                m_bLoadSysTab = true;
              
                bRC = true;

              }
            }

          } else {
            // set value in dbm config file to remember to use 'db_activate with recovery' for restore
            if( pDatabase->GetConfig().PutRawValue(DBMCLI_CFGKEY_WEBDBMRECOVERY, DBMCLI_CFGVAL_WEBDBMRECOVERY_INSTALL, oMsgList ) ) {
              bRC = true;
            }
          }

        }
      }
    }
  }

  return bRC;
}


/*
  -----------------------------------------------------------------------------
  public function DBMCli_Wizard :: DropDatabase
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Wizard :: DropDatabase ( DBMCli_Database ** ppDatabase,
                                           Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  if( !( *ppDatabase == NULL ) ) {
    if( (*ppDatabase)->IsConnected( ) ) {
      (*ppDatabase)->Disconnect();
    }
  
    DBMCli_Node oNode(m_sServerName, oMsgList, false);

    if( oNode.DropDatabase( m_sDatabaseName, m_sDBMName, m_sDBMPwd, oMsgList ) ) {
      bRC = true;
    }

    if( *ppDatabase != NULL ) {
      delete *ppDatabase;
      *ppDatabase = NULL;
    }

    m_bCreated = false;
    m_bConnected = false;
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  private function DBMCli_Wizard :: ???
  -----------------------------------------------------------------------------
*/
