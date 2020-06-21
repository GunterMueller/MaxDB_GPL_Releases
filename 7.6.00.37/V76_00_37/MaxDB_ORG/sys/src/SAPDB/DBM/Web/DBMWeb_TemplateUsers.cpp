/*!
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateUsers.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  creating list of users
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
#include "DBM/Web/DBMWeb_TemplateUsers.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define ISOBJ(obj)  (szName.Compare(obj) == 0)

#define TEMPLATE_NAME   "DBMUsers.htm"

#define OBJ_TITLE       "GroupBoxTitle"

#define OBJ_ROWPAIR     "RowPair"

#define OBJ_EVENROW     "EvenRow*"
#define OBJ_ODDROW      "OddRow*"

#define OBJ_NAME        "Name"
#define OBJ_SECPWD      "SecondPwd"
#define OBJ_DIS         "Disabled"
#define OBJ_COMMENT     "Comment"

#define VAL_NO          "NO"
#define VAL_YES         "YES"

/*
  =============================================================================
  class:        DBMWeb_TemplateUsers
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMWeb_TemplateUsers :: DBMWeb_TemplateUsers
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateUsers :: DBMWeb_TemplateUsers ( sapdbwa_WebAgent & wa,
                                               DBMCli_Users & oUsers )
                          : Tools_Template( wa, _Tools_UTF8Ptr( TEMPLATE_NAME )), 
                            m_oUsers( oUsers )
{
  m_nCol = 0;
  m_nRow = 0;
} 

/*
  -----------------------------------------------------------------------------
  public destructor DBMWeb_TemplateUsers :: ~DBMWeb_TemplateUsers
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateUsers :: ~DBMWeb_TemplateUsers (  )
{
} 

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateUsers :: askForWriteCount
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateUsers :: askForWriteCount  ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  if( ISOBJ( OBJ_TITLE ) ) {
    nReturn = 1;
  } else if( ISOBJ( OBJ_ROWPAIR ) ) {
    nReturn = (m_oUsers.UserArray( ).GetSize( )  / 2) + 
              (m_oUsers.UserArray( ).GetSize( ) % 2); 
  } else if ( ( ISOBJ( OBJ_EVENROW ) ) || 
              ( ISOBJ( OBJ_ODDROW ) )     ) {
    nReturn = (m_nRow < m_oUsers.UserArray( ).GetSize( )) ? 1 : 0;
    m_nRow++;
  } 

  return nReturn;
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateUsers :: askForValue
  -----------------------------------------------------------------------------
*/
Tools_DynamicUTF8String DBMWeb_TemplateUsers :: askForValue ( const Tools_DynamicUTF8String & szName )
{
  const SAPDB_Char * pReturn = NULL;
  DBMCli_String sValue;
  
  if( ISOBJ( OBJ_TITLE ) ) {
    sValue = m_oUsers.GetDatabase().DatabaseName();  
  } else if( ISOBJ( OBJ_NAME ) ) {
    DBMCli_UserArray & aUsers = m_oUsers.UserArray( );
    sValue = aUsers[m_nRow - 1].Name( );
  } else if( ISOBJ( OBJ_SECPWD ) ) {
    DBMCli_UserArray & aUsers = m_oUsers.UserArray( );
    if( aUsers[m_nRow - 1].SecondPwd( ) == VAL_YES ) {
      sValue = aUsers[m_nRow - 1].SecondPwd( );
    } else {
      sValue = "&nbsp;";
    }
  } else if( ISOBJ( OBJ_DIS ) ) {
    DBMCli_UserArray & aUsers = m_oUsers.UserArray( );
    if( aUsers[m_nRow - 1].Disabled( ) == VAL_YES ) {
      sValue = aUsers[m_nRow - 1].Disabled( );
    } else {
      sValue = "&nbsp;";
    }
  } else if( ISOBJ( OBJ_COMMENT ) ) {
    DBMCli_UserArray & aUsers = m_oUsers.UserArray( );
    sValue = aUsers[m_nRow - 1].Comment( );
    if( sValue.IsEmpty() ) {
      sValue = "&nbsp;";
    }
  }

  pReturn = sValue;

  return _Tools_UTF8Ptr(pReturn);
} 

