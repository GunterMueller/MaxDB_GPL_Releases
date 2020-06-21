/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_Database.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager Client Library
  description:  Database Class
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

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define UTL_CMD_INITCONFIG    "init config"
#define UTL_CMD_VERIFY        "verify"

/*    
  =============================================================================
  class: DBMCli_Database
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_Database :: DBMCli_Database
  -----------------------------------------------------------------------------
*/
DBMCli_Database :: DBMCli_Database ( const DBMCli_String        & sServer,
                                     const DBMCli_String        & sDatabase,
                                     const DBMCli_String        & sUser,
                                           Msg_List & oMsgList,
                                     const SAPDB_Bool             bConnect )
                 : DBMCli_Node (sServer, sDatabase, sUser, oMsgList, bConnect)
{
  m_oState.SetDatabase( this );
  m_oInfo.SetDatabase( this );
  m_oHistory.SetDatabase( this );
  m_oFiles.SetDatabase( this );
  m_oDiagnosises.SetDatabase( this );
  m_oFile.SetDatabase( this );
  m_oShows.SetDatabase( this );
  m_oShow.SetDatabase( this );
  m_oParameters.SetDatabase( this );
  m_oParameter.SetDatabase( this );
  m_oDevspaces.SetDatabase( this );
  m_oMedia.SetDatabase( this );
  m_oBackup.SetDatabase( this );
  m_oRecover.SetDatabase( this );
  m_oRestartInfo.SetDatabase( this );
  m_oAutolog.SetDatabase( this );
  m_oKernelTrace.SetDatabase( this );
  m_oUpdStat.SetDatabase( this );
  m_oIndexes.SetDatabase( this );
  m_oLogModeObj.SetDatabase( this );
  m_oUsers.SetDatabase( this );
  m_oConfig.SetDatabase( this );

  m_nKernel = DBMCLI_KERNEL_FAST;

  m_bUTLConnected = false;
  m_bSQLConnected = false;
  m_bSRVConnected = false;
}

/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_Database :: ~DBMCli_Database
  -----------------------------------------------------------------------------
*/
DBMCli_Database :: ~DBMCli_Database ( )
{
  Msg_List oMsgList;

  if( m_bSQLConnected )
    SQLRelease( oMsgList );

  if( m_bUTLConnected )
    UTLRelease( oMsgList );
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Database :: Start
  -----------------------------------------------------------------------------
*/
const DBMCli_String DBMCli_Database :: DatabaseOnServer( )
{
    DBMCli_String sServer = ServerName( );
    DBMCli_String sName = DatabaseName( );

    if( !sServer.IsEmpty( ) ) {
      sName += " on ";
      sName += sServer;
    }

    return sName;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Database :: Start
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Database :: Start ( Msg_List & oMsgList )
{
  return Start( DBMCLI_KERNEL_FAST, oMsgList );
}

SAPDB_Bool DBMCli_Database :: Start ( DBMCli_KernelType      nKernel,
                                      Msg_List & oMsgList )
{
  m_nKernel  = nKernel;

  DBMCli_String sCmd( DBMCLI_CMD_DBSTART );
  sCmd += " ";
  sCmd += GetKernelOption( );

  return Execute( sCmd, oMsgList);
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Database :: Restart
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Database :: Restart ( Msg_List & oMsgList )
{
  return Execute( DBMCli_String( DBMCLI_CMD_DBWARM ), oMsgList);
}
/*
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Database :: Restart ( const DBMCli_String & sUntil,
                                  Msg_List & oMsgList )
{
  DBMCli_String sCmd( DBMCLI_CMD_DBWARM );
  sCmd += " ";
  sCmd += DBMCLI_OPT_RESTART_UNTIL;
  sCmd += " ";
  sCmd += sUntil;

  return Execute( sCmd, oMsgList);
}
/*
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Database :: Restart ( const DBMCli_DateTime & oUntil,
                                        Msg_List & oMsgList )
{
  DBMCli_String sCmd( DBMCLI_CMD_DBWARM );
  sCmd += " ";
  sCmd += DBMCLI_OPT_RESTART_UNTIL;
  sCmd += " ";
  sCmd += oUntil.Date().Get(DBMCLI_DT_NUM_DATE_FMT);
  sCmd += " ";
  sCmd += oUntil.Time().Get(DBMCLI_DT_NUM_TIME_FMT);

  return Execute( sCmd, oMsgList);
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Database :: Shutdown
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Database :: Shutdown ( Msg_List & oMsgList )
{
  return Shutdown( false, oMsgList );
}

SAPDB_Bool DBMCli_Database :: Shutdown ( SAPDB_Bool                   bQuick,
                                   Msg_List & oMsgList )
{
  DBMCli_String sCmd( DBMCLI_CMD_DBCOLD );
  
  if( bQuick )
  {
    sCmd += " ";
    sCmd += DBMCLI_OPT_SHUTDOWN_QUICK;
  }

  return Execute( sCmd, oMsgList);
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Database :: Stop
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Database :: Stop ( Msg_List & oMsgList )
{
  return Execute( DBMCli_String( DBMCLI_CMD_DBSTOP ), oMsgList);
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Database :: Offline
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Database :: Offline ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_State & oState = GetState( );

  switch( oState.Value( ) ) {

    case DBMCLI_DBSTATE_OFFLINE:
      bRC = true;
      break;

    case DBMCLI_DBSTATE_COLD:
      bRC = Stop( oMsgList );
      break;

    case DBMCLI_DBSTATE_WARM:
      if( Shutdown( oMsgList ) ) {
        if( Stop( oMsgList ) ) {
          bRC = true;
        }
      }
      break;

    default:
      bRC = Execute( DBMCLI_CMD_DBOFFLINE, oMsgList);
      break;
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Database :: Cold
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Database :: Cold ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_State & oState = GetState( );

  switch( oState.Value( ) ) {

    case DBMCLI_DBSTATE_OFFLINE:
      bRC = Start( oMsgList );
      break;

    case DBMCLI_DBSTATE_COLD:
      bRC = true;
      break;

    case DBMCLI_DBSTATE_WARM:
      bRC = Shutdown( oMsgList );
      break;

    default:
      bRC = Execute( DBMCLI_CMD_DBCOLD, oMsgList);
      break;
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Database :: Warm
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Database :: Warm ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_State & oState = GetState( );

  switch( oState.Value( ) ) {

    case DBMCLI_DBSTATE_OFFLINE:
      if( Start( oMsgList ) ) {
        if( Restart( oMsgList ) ) {
          bRC = true;
        }
      }
      break;

    case DBMCLI_DBSTATE_COLD:
      bRC = Restart( oMsgList );
      break;

    case DBMCLI_DBSTATE_WARM:
      bRC = true;
      break;

    default:
      bRC = Execute( DBMCLI_CMD_DBWARM, oMsgList);
      break;
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Database :: UTLIsConnected
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Database :: UTLIsConnected ( )
{
  return m_bUTLConnected ;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Database :: UTLConnect
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Database :: UTLConnect ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  if( !m_bUTLConnected ) {
    if( Execute( DBMCli_String( DBMCLI_CMD_UTLCONNECT ), oMsgList) ) {
      m_bUTLConnected = true;
      bRC = true;
    }
  } else {
    bRC = true;
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Database :: UTLExecute
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Database :: UTLExecute ( const DBMCli_String        & sCommand,
                                                 Msg_List & oMsgList )
{
  DBMCli_String sCmd( DBMCLI_CMD_UTLEXECUTE );

  sCmd += " ";
  sCmd += sCommand;

  return Execute( sCmd, oMsgList );
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Database :: UTLRelease
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Database :: UTLRelease ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  if( Execute( DBMCli_String( DBMCLI_CMD_UTLRELEASE ), oMsgList) ) {
    bRC = true;
  }
   
  m_bUTLConnected = false;

  return bRC;
} 

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Database :: SQLIsConnected
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Database :: SQLIsConnected ( )
{
  return m_bSQLConnected ;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Database :: SQLConnect
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Database :: SQLConnect ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  if( !m_bSQLConnected ) {
    if( Execute( DBMCli_String( DBMCLI_CMD_SQLCONNECT ), oMsgList) ) {
      m_bSQLConnected = true;
      bRC = true;
    }
  } else {
    bRC = true;
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Database :: SQLExecute
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Database :: SQLExecute ( const DBMCli_String        & sCommand,
                                                 Msg_List & oMsgList )
{
  DBMCli_String sCmd( DBMCLI_CMD_SQLEXECUTE );

  sCmd += " ";
  sCmd += sCommand;

  return Execute( sCmd, oMsgList );
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Database :: SQLRelease
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Database :: SQLRelease ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  if( Execute( DBMCli_String( DBMCLI_CMD_SQLRELEASE ), oMsgList) ) {
    bRC = true;
  }
  
  m_bSQLConnected = false;
  
  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Database :: Activate
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Database :: Activate ( const DBMCli_String & sDBAName,
                                         const DBMCli_String & sDBAPwd,
                                         Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;
  SAPDB_Bool bConnect = true;

  if( NodeInfo().Version().GetNumber() < 0x7040400 ) {
    if( !m_bUTLConnected ) {
      if( UTLConnect( oMsgList ) ) {
        bConnect = true;
        bRC = true;
      }
    }

    if( bRC ) {
      bRC = false;

      DBMCli_String sCmd( DBMCLI_CMD_UTILACTIVATE );
      sCmd += " ";
      sCmd += sDBAName;
      sCmd += ",";
      sCmd += sDBAPwd;

      if( Execute( sCmd, oMsgList ) ) {
        bRC = true;
      }
    }

    if( bConnect ) {
      UTLRelease( oMsgList );
    }
  } else {
    DBMCli_String sCmd( DBMCLI_CMD_DBACTIVATE );
    sCmd += " ";
    sCmd += sDBAName;
    sCmd += ",";
    sCmd += sDBAPwd;

    if( Execute( sCmd, oMsgList ) ) {
      bRC = true;
    }
  }
  
  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Database :: InitConfig
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Database :: InitConfig ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;
  SAPDB_Bool bConnect = true;

  if( NodeInfo().Version().GetNumber() < 0x7040400 ) {
    if( !m_bUTLConnected ) {
      if( UTLConnect( oMsgList ) ) {
        bConnect = true;
        bRC = true;
      }
    }

    if( bRC ) {
      bRC = false;
      if( UTLExecute( UTL_CMD_INITCONFIG, oMsgList ) ) {
        bRC = true;
      }
    }

    if( bConnect ) {
      UTLRelease( oMsgList );
    }
  } else {
    bRC = true;
  }
  
  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Database :: LoadSysTab
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Database :: LoadSysTab ( const DBMCli_String & sDBAName,
                                           const DBMCli_String & sDBAPwd,
                                           const DBMCli_String & sDomainPwd,
                                           Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_String sCmd( DBMCLI_CMD_LOADSYSTAB );
  if( !sDBAName.IsEmpty( ) ) {
    sCmd += " -u ";
    sCmd += sDBAName;
    sCmd += ",";
    sCmd += sDBAPwd;
    if( !sDomainPwd.IsEmpty( ) ) {
      sCmd += " -ud ";
      sCmd += sDomainPwd;
    }
  }

  if( Execute( sCmd, oMsgList) ) {
    bRC = true;
  }
  
  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Database :: Command
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Database :: Command ( const DBMCli_String & sCommand,
                                        Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  if( Execute( sCommand, oMsgList) ) {
    bRC = true;
  }
  
  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Database :: SRVConnect
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Database :: SRVConnect ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  if( !m_bSRVConnected ) {
    if( Execute( DBMCli_String( DBMCLI_CMD_SRVCONNECT ), oMsgList) ) {
      m_bSRVConnected = true;
      bRC = true;
    }
  } else {
    bRC = true;
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Database :: SRVRelease 
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Database :: SRVRelease ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  if( Execute( DBMCli_String( DBMCLI_CMD_SRVRELEASE ), oMsgList) ) {
    bRC = true;
  }
  
  m_bSRVConnected = false;
  
  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Database :: Verify
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Database :: Verify ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;
  SAPDB_Bool bConnect = true;

  if( !m_bUTLConnected ) {
    if( UTLConnect( oMsgList ) ) {
      bConnect = true;
      bRC = true;
    }
  }

  if( bRC ) {
    bRC = false;
    if( UTLExecute( UTL_CMD_VERIFY, oMsgList ) ) {
      bRC = true;
    }
  }

  if( bConnect ) {
    UTLRelease( oMsgList );
  }
  
  return bRC;
}

/*
  -----------------------------------------------------------------------------
  private function DBMCli_Database :: GetKernelOption
  -----------------------------------------------------------------------------
*/
const DBMCli_String DBMCli_Database :: GetKernelOption ( )
{

  switch( m_nKernel )
  {
    case DBMCLI_KERNEL_FAST: 
      return DBMCLI_OPT_KERNEL_FAST;
    case DBMCLI_KERNEL_QUICK: 
      return DBMCLI_OPT_KERNEL_QUICK;
    case DBMCLI_KERNEL_SLOW: 
      return DBMCLI_OPT_KERNEL_SLOW;
    default:
      return "";
  }
}
