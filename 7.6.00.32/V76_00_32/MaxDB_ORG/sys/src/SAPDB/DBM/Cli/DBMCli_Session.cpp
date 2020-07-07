/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_Session.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager Client Library
  description:  DBM-Server Session Class
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
#include "hcn14.h"
#include "DBM/Cli/DBMCli_Session.hpp"

/*    
  =============================================================================
  class: DBMCli_Session
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_Session :: DBMCli_Session
  -----------------------------------------------------------------------------
*/
DBMCli_Session :: DBMCli_Session ( )
{
  m_pSession   = NULL;
  m_pResult    = NULL;

  m_sServer.Empty( );
  m_sDatabase.Empty( );
  m_sUser.Empty( );
  m_sInstRoot.Empty( );
}

DBMCli_Session :: DBMCli_Session ( const DBMCli_String        & sServer,
                                         Msg_List & oMsgList,
                                   const SAPDB_Bool             bConnect )
{
  m_pSession   = NULL;
  m_pResult    = NULL;

  m_sServer = sServer;
  m_sDatabase.Empty( );
  m_sUser.Empty( );
  m_sInstRoot.Empty( );

  oMsgList.ClearMessageList( );

  if( bConnect ) {
    Connect( oMsgList );
  } 
} 

DBMCli_Session :: DBMCli_Session ( const DBMCli_String        & sServer,
                                   const DBMCli_String        & sDatabase,
                                         Msg_List & oMsgList,
                                   const SAPDB_Bool             bConnect )
{
  m_pSession   = NULL;
  m_pResult    = NULL;

  m_sServer = sServer;
  m_sDatabase = sDatabase;
  m_sUser.Empty( );
  m_sInstRoot.Empty( );

  oMsgList.ClearMessageList( );

  if( bConnect ) {
    Connect( oMsgList );
  } 
}

DBMCli_Session :: DBMCli_Session ( const DBMCli_String        & sServer,
                                   const DBMCli_String        & sDatabase,
                                   const DBMCli_String        & sUser,
                                         Msg_List & oMsgList,
                                   const SAPDB_Bool             bConnect )
{
  m_pSession   = NULL;
  m_pResult    = NULL;

  m_sServer   = sServer;
  m_sDatabase = sDatabase;
  m_sUser     = sUser;
  m_sInstRoot.Empty( );

  oMsgList.ClearMessageList( );

  if( bConnect ) {
    Connect( oMsgList );
  } 
}
 
DBMCli_Session :: DBMCli_Session ( const DBMCli_String        & sServer,
                                   const DBMCli_String        & sDatabase,
                                   const DBMCli_String        & sUser,
                                   const DBMCli_String        & sInstRoot,
                                         Msg_List & oMsgList,
                                   const SAPDB_Bool             bConnect )
{
  m_pSession   = NULL;
  m_pResult    = NULL;

  m_sServer   = sServer;
  m_sDatabase = sDatabase;
  m_sUser     = sUser;
  m_sInstRoot = sInstRoot;

  oMsgList.ClearMessageList( );

  if( bConnect ) {
    Connect( oMsgList );
  } 
}

/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_Session :: ~DBMCli_Session
  -----------------------------------------------------------------------------
*/
DBMCli_Session :: ~DBMCli_Session ( )
{
  DBMDisconnect( );
} 

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Session :: Connect
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Session :: Connect ( Msg_List & oMsgList )
{
  if( m_pSession != NULL ) {
    DBMDisconnect( );
  } 
  return DBMConnect( m_sServer, m_sDatabase, m_sUser, m_sInstRoot, oMsgList );
} 

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Session :: Disconnect
  -----------------------------------------------------------------------------
*/
void DBMCli_Session :: Disconnect ( )
{
  DBMDisconnect( );
} 

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Session :: IsConnected
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Session :: IsConnected ( )
{
  return( m_pSession != NULL );
} 

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Session :: GetResult
  -----------------------------------------------------------------------------
*/
DBMCli_Result & DBMCli_Session :: GetResult ( )
{
  return *m_pResult;
} 

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Session :: Execute
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Session :: Execute ( const DBMCli_String        & sCommand,
                                             Msg_List & oMsgList )
{
  SAPDB_Bool bRC = true;

  oMsgList.ClearMessageList();

  if( !IsConnected( ) ) {
    bRC = Connect( oMsgList );
  } 

  if( oMsgList.IsEmpty( ) ) {
    SAPDB_Int nRC = DBMAPI_OK_CN14;
    SAPDB_Int nOut = m_nPacketSize;
    tsp00_ErrTextc oError;

    oMsgList.ClearMessageList();

    m_pResult->Clear( ); 

    oError.Init();

    char * pBuffer = m_pResult->GetBuffer(nOut);

    nRC = cn14cmdExecute( m_pSession, 
                          sCommand, sCommand.GetLength( ), 
                          pBuffer, &nOut,
                          oError);
    
    pBuffer[nOut] = '\0';

    m_pResult->ReleaseBuffer(nOut); 

    if( nRC == DBMAPI_OK_CN14 ) {

      bRC = m_pResult->Check( oMsgList );
    
    } else {
      Msg_List oMsg( DBMCLI_COMPID, __FILE__, __LINE__,  
                                 Msg_List::Error, nRC, 
                                 oError, 0 );

      oMsgList.Overrule( oMsg );

      bRC = false;
    } 
  } 

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  private function DBMCli_Session :: DBMConnect
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Session :: DBMConnect ( const DBMCli_String        & sServer,
                                          const DBMCli_String        & sDatabase,
                                          const DBMCli_String        & sUser,
                                          const DBMCli_String        & sInstRoot,
                                                Msg_List & oMsgList )
{
  SAPDB_Int4        nRc      = DBMAPI_OK_CN14;
  tsp00_ErrTextc    aErrText;
  SAPDB_Bool        bUserLogon = false;
  SAPDB_Bool        bRC = true;

  m_pSession    = NULL;

  aErrText.Init();

  oMsgList.ClearMessageList();

  if (!sUser.IsEmpty()) {
    nRc = cn14connectDBM(sServer, sDatabase, sInstRoot, &m_pSession, aErrText);
    if (nRc == DBMAPI_OK_CN14) {
      bUserLogon = true;
    }
  } else {
    nRc = cn14connectDBM(sServer, sDatabase, sInstRoot, &m_pSession, aErrText);
  } // end if

  if (nRc == DBMAPI_OK_CN14) {
    SAPDB_Int4 nPacketLen;
    SAPDB_Int4 nPacketPos;

    cn14packetstats(m_pSession, &m_nPacketSize, &nPacketLen, &nPacketPos);
    m_pResult = new DBMCli_Result(m_nPacketSize + 200);

  } else {
    Msg_List lMsg(DBMCLI_COMPID, __FILE__, __LINE__, Msg_List::Error, nRc, aErrText, 0);
    oMsgList = lMsg;
  } 

  if (bUserLogon) {
    DBMCli_String sCmd("user_logon ");
    sCmd += sUser;

    bRC = Execute(sCmd, oMsgList);

    if (bRC) {
      nRc = DBMAPI_OK_CN14;
    } else {
      DBMDisconnect();
      nRc = DBMAPI_USR_FALSE_CN14;
    }
  }
  
  return (nRc == DBMAPI_OK_CN14);
} // end DBMCli_Session :: DBMConnect
 
/*
  -----------------------------------------------------------------------------
  private function DBMCli_Session :: DBMDisconnect
  -----------------------------------------------------------------------------
*/
void DBMCli_Session :: DBMDisconnect ( )
{
  if (m_pSession != NULL) {
    cn14release(&m_pSession);
    m_pSession = NULL;
  } 

  if (m_pResult != NULL) {
    delete m_pResult;
    m_pResult= NULL;
  } 
} //  end DBMCli_Session :: DBMDisconnect

