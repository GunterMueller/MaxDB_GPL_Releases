/*!
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateBadDevspaces.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  BadDevspaces 
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
#include "DBM/Web/DBMWeb_TemplateBadDevspaces.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define ISOBJ(obj)  (szName.Compare(obj) == 0)

#define TEMPLATE_NAME          "DBMBadDevspaces.htm"

#define OBJ_BADDEVS            "BadDevspaces"
#define OBJ_RESULT             "Result"

#define OBJ_NAME               "Name"
#define OBJ_STATUS             "Status"

#define OBJ_ROWPAIR            "RowPair"
#define OBJ_EVENROW            "EvenRow*"
#define OBJ_ODDROW             "OddRow*"

#define OBJ_BTNREINTEGRATE     "BtnReintegrate"
#define OBJ_BTNBAK             "BtnBack"

/*
  =============================================================================
  class:        DBMWeb_TemplateBadDevspaces
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMWeb_TemplateBadDevspaces :: DBMWeb_TemplateBadDevspaces
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateBadDevspaces :: DBMWeb_TemplateBadDevspaces ( sapdbwa_WebAgent & wa,
                                                             DBMCli_Database * pDB,
                                                             DBMWeb_TemplBadDevspaces_Mode nMode )
                        : Tools_Template( wa, _Tools_UTF8Ptr( TEMPLATE_NAME )),
                          m_pDB( pDB ),
                          m_nMode( nMode )
{
  m_nRow = 0;
} 

/*
  -----------------------------------------------------------------------------
  public destructor DBMWeb_TemplateBadDevspaces :: ~DBMWeb_TemplateBadDevspaces
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateBadDevspaces :: ~DBMWeb_TemplateBadDevspaces (  )
{
} 

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateBadDevspaces :: askForWriteCount
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateBadDevspaces :: askForWriteCount  ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;
  Msg_List oMsgList;

  if( ISOBJ( OBJ_BADDEVS ) ) {
    nReturn = (m_nMode == DBMWEB_TEMPLBADDEVSPACES_MODE_BADDEVSPACES) ? 1 : 0;
  } else if( ISOBJ( OBJ_RESULT ) ) {
    nReturn = (m_nMode == DBMWEB_TEMPLBADDEVSPACES_MODE_RESULT) ? 1 : 0;
  } else if( ISOBJ( OBJ_ROWPAIR ) ) {
    nReturn = -1;
  } else if( ISOBJ( OBJ_EVENROW ) || ISOBJ( OBJ_ODDROW ) ) {
    m_oRow.Clear( );
    if (m_pDB->GetInfo( ).GetRow( m_oRow, oMsgList )) {
      m_oRow.GetField( m_sName, "|\n" );
      m_sName.Trim( );
      m_oRow.GetField( m_sStatus, "|\n" );
      m_sStatus.Trim( );
      nReturn = 1;
    }
    m_nRow += 1;
  } else if( ISOBJ( OBJ_BTNREINTEGRATE ) ) {
    nReturn = (m_nMode == DBMWEB_TEMPLBADDEVSPACES_MODE_BADDEVSPACES) ? 1 : 0;
  } else if( ISOBJ( OBJ_BTNBAK ) ) {
    nReturn = (m_nMode == DBMWEB_TEMPLBADDEVSPACES_MODE_RESULT) ? 1 : 0;
  }
  return nReturn;
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateBadDevspaces :: askForValue
  -----------------------------------------------------------------------------
*/
Tools_DynamicUTF8String DBMWeb_TemplateBadDevspaces :: askForValue ( const Tools_DynamicUTF8String & szName )
{
  const SAPDB_Char * pReturn = NULL;

  DBMCli_String sValue;

  if( ISOBJ( OBJ_NAME ) ) {
    pReturn = m_sName;
  } else if( ISOBJ( OBJ_STATUS) ) {
    pReturn = m_sStatus;
  }

  return _Tools_UTF8Ptr(pReturn);
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateInfoTable :: askForContinue
  -----------------------------------------------------------------------------
*/
bool DBMWeb_TemplateBadDevspaces :: askForContinue ( const Tools_DynamicUTF8String & szName )
{
  bool bRC = false;

  if( ISOBJ( OBJ_ROWPAIR ) ) {
    if( !m_pDB->GetInfo( ).EndOfInfo( ) ) {
      bRC = true;
    }
  }

  return bRC;
}
