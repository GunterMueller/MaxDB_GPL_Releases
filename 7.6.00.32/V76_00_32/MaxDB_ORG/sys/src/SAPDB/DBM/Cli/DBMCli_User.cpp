/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_User.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager Client Library
  description:  Media Class
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
#include "DBM/Cli/DBMCli_User.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define PROPVAL_SEP "="
#define FIELD_SEP   "\t\n"

#define PROP_SRVRIG "SERVERRIGHTS"
#define PROP_GUIRIG "GUIRIGHTS"
#define PROP_PASSWD "PASSWORD"
#define PROP_SECPWD "SECONDPASSWORD"
#define PROP_DBMUSR "DBMUSER"
#define PROP_SQLUSR "SQLUSER"
#define PROP_SQLUMO "SQLUSERMODE"
#define PROP_DISABL "DISABLED"
#define PROP_SYSNAM "SYSTEMNAME"
#define PROP_COMMNT "COMMENT"

#define RIGHT_YES   "+"
#define RIGHT_NO    "-"

/*    
  =============================================================================
  class: DBMCli_UserRight
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_UserRight :: DBMCli_UserRight
  -----------------------------------------------------------------------------
*/
DBMCli_UserRight :: DBMCli_UserRight ( )
{
}

DBMCli_UserRight :: DBMCli_UserRight ( const DBMCli_String & sName,
                                       const SAPDB_Bool      bValue,
                                       const DBMCli_String & sDescription )
             : m_sName( sName ),
               m_bValue( bValue ),
               m_sDescription( sDescription )
{
}

/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_UserRight :: ~DBMCli_UserRight
  -----------------------------------------------------------------------------
*/
DBMCli_UserRight :: ~DBMCli_UserRight ( )
{
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_UserRight :: Right
  -----------------------------------------------------------------------------
*/
const DBMCli_String DBMCli_UserRight :: Right( )
{
  DBMCli_String sRight;

  if( m_bValue ) {
    sRight = RIGHT_YES + m_sName;
  } else {
    sRight = RIGHT_NO + m_sName;
  }

  return sRight;
}

/*    
  =============================================================================
  class: DBMCli_User
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_User :: DBMCli_User
  -----------------------------------------------------------------------------
*/
DBMCli_User :: DBMCli_User ( )
{
}

DBMCli_User :: DBMCli_User ( const DBMCli_String & sName )
             : m_sName( sName )
{
  Clear( );
}

DBMCli_User :: DBMCli_User ( const DBMCli_String & sName,
                             const DBMCli_String & sServerRights,
                             const DBMCli_String & sGuiRights,
                             const DBMCli_String & sSecondPwd,
                             const DBMCli_String & sDBMUser,
                             const DBMCli_String & sSQLUser,
                             const DBMCli_String & sSQLUserMode,
                             const DBMCli_String & sDisabled,
                             const DBMCli_String & sSystemName,
                             const DBMCli_String & sComment)
             : m_sName( sName ),
               m_sServerRights( sServerRights),
               m_sGuiRights( sGuiRights ),
               m_sSecondPwd( sSecondPwd ),
               m_sDBMUser( sDBMUser ),
               m_sSQLUser( sSQLUser ),
               m_sSQLUserMode( sSQLUserMode ),
               m_sDisabled( sDisabled ),
               m_sSystemName( sSystemName ),
               m_sComment( sComment )
{
}

/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_User :: ~DBMCli_User
  -----------------------------------------------------------------------------
*/
DBMCli_User :: ~DBMCli_User ( )
{
  m_sName.Empty( );
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_User :: Clear
  -----------------------------------------------------------------------------
*/
void DBMCli_User :: Clear ( )
{
  m_sServerRights.Empty( );
  m_sGuiRights.Empty( );
  m_sSecondPwd.Empty( );
  m_sDBMUser.Empty( );
  m_sSQLUser.Empty( );
  m_sSQLUserMode.Empty( );
  m_sDisabled.Empty( );
  m_sSystemName.Empty( );
  m_sComment.Empty( );

  m_aServerRights.RemoveAll( );
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_User :: Refresh
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_User :: Refresh ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();
  DBMCli_Result & oResult = oDB.GetResult( ); 
  
  DBMCli_String sCmd( DBMCLI_CMD_USER_GET );
  sCmd += " " + m_sName;
  
  Clear( );
  
  if ( oDB.Execute( sCmd, oMsgList ) ) {
    SAPDB_Bool bOK = true;
    DBMCli_String sProp;
    DBMCli_String sVal;

    while( bOK ) {
      bOK = oResult.GetPropVal( sProp, sVal, "=" );
      if( bOK ) {
        AssignProp( sProp, sVal );
      }
    }

    // serverrights

    sCmd.Format( "%s %s %s", DBMCLI_CMD_USER_GETRIGHTS, (const char *)m_sName, PROP_SRVRIG );
    if ( oDB.Execute( sCmd, oMsgList ) ) {
      DBMCli_ResultBuf oLine;

      DBMCli_String sName;
      DBMCli_String sValue;
      DBMCli_String sDescription;
      SAPDB_Bool    bValue;

      while( oResult.GetLine( oLine ) ) {
        if( oLine.GetField( sName, FIELD_SEP ) ) {
          if( oLine.GetField( sValue, FIELD_SEP ) ) {
            sValue.Trim( );
            if( sValue == RIGHT_YES ) {
              bValue = true;
            } else {
              bValue = false;
            }  
            if( oLine.GetField( sDescription, FIELD_SEP ) ) {
              sName.Trim( );
              sDescription.Trim( );
              DBMCli_UserRight oUserRight( sName, bValue, sDescription );
              m_aServerRights.Add( oUserRight );
            }
          }
        }
        oLine.Clear( );
        sName.Empty( );
        sValue.Empty( );
        sDescription.Empty( );
      }

      bRC = true;
    }
  }
   
  return bRC;
} 

/*
  -----------------------------------------------------------------------------
  public function DBMCli_User :: ServerRightString
  -----------------------------------------------------------------------------
*/
const DBMCli_String DBMCli_User :: ServerRightString ( )
{
  DBMCli_String sRightString;
  SAPDB_Int nRight;

  sRightString.Empty( );

  for( nRight = 0; nRight < m_aServerRights.GetSize( ); nRight++) {
    if( !sRightString.IsEmpty( ) ) {
      sRightString += ",";
    }
    sRightString += m_aServerRights[nRight].Right( );
  }

  return sRightString;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_User :: Put
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_User :: Put ( const DBMCli_String & sDisabled,
                                const DBMCli_String & sComment,
                                const DBMCli_String & sSecondPwd,
                                const SAPDB_Bool      bSeconPwd,
                                Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();

  DBMCli_String sCmd( DBMCLI_CMD_USER_PUT );

  sCmd += " "; 
  sCmd += m_sName;
  sCmd += " "; 

  DBMCli_String sServerRights = ServerRightString( );
  
  if( !sServerRights.IsEmpty( ) ) {
    sCmd += PROP_SRVRIG;
    sCmd += "=" + sServerRights; 
    sCmd += " ";
  }

  if( !sDisabled.IsEmpty( ) ) {
    sCmd += PROP_DISABL;
    sCmd += "=" + sDisabled; 
    sCmd += " ";
  }

  sCmd += PROP_COMMNT;
  sCmd += "=" + sComment; 
  sCmd += " ";

  if( bSeconPwd ) {
    sCmd += PROP_SECPWD;
    sCmd += "=" + sSecondPwd; 
    sCmd += " ";
  }

  if( oDB.Execute( sCmd, oMsgList ) ) {
    bRC = Refresh( oMsgList );
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_User :: PutPassword
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_User :: PutPassword ( const DBMCli_String & sPassword,
                                        Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();

  DBMCli_String sCmd( DBMCLI_CMD_USER_PUT );

  sCmd += " "; 
  sCmd += m_sName;
  sCmd += " "; 

  sCmd += PROP_PASSWD;
  sCmd += "=" + sPassword; 
  sCmd += " ";

  if( oDB.Execute( sCmd, oMsgList ) ) {
    bRC = Refresh( oMsgList );
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_User :: PutSecondPwd
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_User :: PutSecondPwd ( const DBMCli_String & sSecondPwd,
                                         Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();

  DBMCli_String sCmd( DBMCLI_CMD_USER_PUT );

  sCmd += " "; 
  sCmd += m_sName;
  sCmd += " "; 

  sCmd += PROP_SECPWD;
  sCmd += "=" + sSecondPwd; 
  sCmd += " ";

  if( oDB.Execute( sCmd, oMsgList ) ) {
    bRC = Refresh( oMsgList );
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_User :: Delete
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_User :: Delete ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();

  DBMCli_String sCmd( DBMCLI_CMD_USER_DELETE );

  sCmd += " "; 
  sCmd += m_sName;

  if( oDB.Execute( sCmd, oMsgList ) ) {
    bRC = true;
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/
void DBMCli_User :: AssignProp ( const DBMCli_String & sProp,
                                 const DBMCli_String & sVal )
{
  if( sProp == PROP_SRVRIG ) {
    m_sServerRights = sVal;
  } else if( sProp == PROP_GUIRIG ) {
    m_sGuiRights = sVal;
  } else if( sProp == PROP_SECPWD ) {
    m_sSecondPwd = sVal;
  } else if( sProp == PROP_DBMUSR ) {
    m_sDBMUser = sVal;
  } else if( sProp == PROP_SQLUSR ) {
    m_sSQLUser = sVal;
  } else if( sProp == PROP_SQLUMO ) {
    m_sSQLUserMode = sVal;
  } else if( sProp == PROP_DISABL ) {
    m_sDisabled = sVal;
  } else if( sProp == PROP_SYSNAM ) {
    m_sSystemName = sVal;
  } else if( sProp == PROP_COMMNT ) {
    m_sComment = sVal;
  }
}


/*    
  =============================================================================
  class: DBMCli_Users
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_Users :: DBMCli_Users
  -----------------------------------------------------------------------------
*/
DBMCli_Users :: DBMCli_Users ( )
{
  Clear( );
}

/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_Users :: ~DBMCli_Users
  -----------------------------------------------------------------------------
*/
DBMCli_Users :: ~DBMCli_Users ( )
{
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Users :: Clear
  -----------------------------------------------------------------------------
*/
void DBMCli_Users :: Clear ( )
{
  m_aUser.RemoveAll( );
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Users :: Refresh
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Users :: Refresh ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  Clear( );

  DBMCli_Database & oDB = GetDatabase();
  DBMCli_Result & oResult = oDB.GetResult( ); 

  DBMCli_String sCmd;

  sCmd = DBMCLI_CMD_USER_GETALL;

  if( oDB.Execute( sCmd, oMsgList ) ) {
    DBMCli_ResultBuf sLine;

    // read away the line containing "OK"
    //oResult.GetLine(sLine);
    sLine.Clear();

    while( oResult.GetLine( sLine ) ) {
      DBMCli_User oUser( sLine );
      oUser.SetDatabase( m_pDatabase );
      m_aUser.Add( oUser );
      sLine.Clear( );
    }

    SAPDB_Int nUser = 0;
    SAPDB_Bool bRefresh = true;

    while( nUser < m_aUser.GetSize( ) && bRefresh ) {
      bRefresh = m_aUser[nUser].Refresh( oMsgList );
      nUser++;
    }

    bRC = bRefresh;
  }

  return bRC;
} 

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Users :: NewUser
  -----------------------------------------------------------------------------
*/
DBMCli_User & DBMCli_Users :: NewUser( const DBMCli_String & sName )
{
  m_oNewUser.Clear( );
  m_oNewUser.SetName( sName );
  m_oNewUser.SetDatabase( m_pDatabase );

  if( m_aUser.GetSize( ) > 0 ) {
    DBMCli_UserRightArray & aRight = m_aUser[0].ServerRightArray( );
    SAPDB_Int nRight;
    for( nRight = 0; nRight < aRight.GetSize( ); nRight++ ) {
      m_oNewUser.ServerRightArray( ).Add( aRight[nRight] );
    }
  }

  return m_oNewUser;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Users :: AddUser
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Users :: AddUser( const DBMCli_String &  sName,
                                    const DBMCli_String &  sPassword,
                                    Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();
  
  DBMCli_String sCmd( DBMCLI_CMD_USER_CREATE );
  sCmd += " " + sName;
  sCmd += "," + sPassword;
  
  if ( oDB.Execute( sCmd, oMsgList ) ) {
    bRC = true;
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Users :: DeleteUser
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Users :: DeleteUser( const DBMCli_String &  sName,
                                       Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();
  
  DBMCli_String sCmd( DBMCLI_CMD_USER_DELETE );
  sCmd += " " + sName;
  
  if ( oDB.Execute( sCmd, oMsgList ) ) {
    bRC = true;
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Users :: IndexByName
  -----------------------------------------------------------------------------
*/
SAPDB_Int DBMCli_Users :: IndexByName ( const DBMCli_String & sName )
{
  SAPDB_Bool bFound = false;
  SAPDB_Int nUser = 0;

  while( nUser < m_aUser.GetSize( ) && !bFound ) {
    if( m_aUser[nUser].Name( ) == sName ) {
      bFound = true;
    } else {
      nUser++;
    }
  }

  if( bFound ) {
    return nUser;
  } else {
    return -1;
  }
}

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

