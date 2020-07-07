/*!
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateUser.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  User
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
#include "DBM/Web/DBMWeb_TemplateUser.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define ISOBJ(obj)  (szName.Compare(obj) == 0)

#define TEMPLATE_NAME          "DBMUser.htm"

#define OBJ_BACK               "BackURL"

#define OBJ_TITLE              "GroupBoxTitle"

#define OBJ_NAME               "Name"
#define OBJ_NAMEEDIT           "NameEdit"
#define OBJ_PWD                "Password"
#define OBJ_PWDCON             "PwdConfirm"
#define OBJ_DIS                "Disabled"
#define OBJ_COM                "Comment"
#define OBJ_PWD2               "Password2"
#define OBJ_PWD2CON            "Pwd2Confirm"

#define OBJ_PWDCHANGE          "PwdChange"
#define OBJ_PWD2CHANGE         "Pwd2Change"

#define OBJ_SRVRIGLST          "SrvRightList"
#define OBJ_SRVRIG             "SrvRight"
#define OBJ_SRVRIGVALS         "SrvRightVals*"
#define OBJ_SRVRIGNAME         "SrvRightName"
#define OBJ_SRVRIGVAL          "SrvRightValue"
#define OBJ_SRVRIGDESC         "SrvRightDesc"

#define OBJ_BUTTONOKADD        "ButtonOKAdd"
#define OBJ_BUTTONOKPUT        "ButtonOKPut"
#define OBJ_BUTTONCANCEL       "ButtonCancel"
#define OBJ_BUTTONDELETE       "ButtonDelete"
#define OBJ_BUTTONCHGPWD       "ButtonChangePwd"
#define OBJ_BUTTONCHGPWD2      "ButtonChangePwd2"

#define OBJ_CHANGEPWD          "ChangePwd"
#define OBJ_CHANGEPWD2         "ChangePwd2"

/*
  =============================================================================
  class:        DBMWeb_TemplateUser
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMWeb_TemplateUser :: DBMWeb_TemplateUser
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateUser :: DBMWeb_TemplateUser ( sapdbwa_WebAgent & wa,
                                             DBMCli_User & oUser,
                                             DBMWeb_TemplUser_Mode nMode,
                                             const DBMCli_String & sBackURL )
                     : Tools_Template( wa, _Tools_UTF8Ptr( TEMPLATE_NAME )),
                       m_oUser( oUser ),
                       m_nMode( nMode ),
                       m_sBackURL( sBackURL )
{
  m_nRight = 0;
} 

/*
  -----------------------------------------------------------------------------
  public destructor DBMWeb_TemplateUser :: ~DBMWeb_TemplateUser
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateUser :: ~DBMWeb_TemplateUser (  )
{
} 

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateUser :: askForWriteCount
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateUser :: askForWriteCount  ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  if( ISOBJ( OBJ_BACK ) ) {
    nReturn = 1;
  } else if( ISOBJ( OBJ_TITLE ) ) {
    nReturn = 1;
  } else if( ISOBJ( OBJ_CHANGEPWD ) ) {
    nReturn = ( m_nMode == DBMWEB_TEMPLUSER_CHANGEPWD ) ? 1 : 0;
  } else if( ISOBJ( OBJ_CHANGEPWD2 ) ) {
    nReturn = ( m_nMode == DBMWEB_TEMPLUSER_CHANGEPWD2 ) ? 1 : 0;
  } else if( ISOBJ( OBJ_NAME ) ) {
    nReturn = ( ( m_nMode == DBMWEB_TEMPLUSER_EDIT ) || 
                ( m_nMode == DBMWEB_TEMPLUSER_CHANGEPWD ) || 
                ( m_nMode == DBMWEB_TEMPLUSER_CHANGEPWD2 ) ) ? 1 : 0;
  } else if( ISOBJ( OBJ_NAMEEDIT ) ) {
    nReturn = ( m_nMode == DBMWEB_TEMPLUSER_NEW ) ? 1 : 0;
  } else if( ISOBJ( OBJ_PWD ) ) {
    nReturn = ( ( m_nMode == DBMWEB_TEMPLUSER_NEW ) || 
                ( m_nMode == DBMWEB_TEMPLUSER_CHANGEPWD ) ) ? 1 : 0;
  } else if( ISOBJ( OBJ_PWDCON ) ) {
    nReturn = ( ( m_nMode == DBMWEB_TEMPLUSER_NEW ) || 
                ( m_nMode == DBMWEB_TEMPLUSER_CHANGEPWD ) ) ? 1 : 0;
  } else if( ISOBJ( OBJ_PWDCHANGE ) ) {
    nReturn = ( m_nMode == DBMWEB_TEMPLUSER_EDIT ) ? 1 : 0;
  } else if( ISOBJ( OBJ_DIS ) ) {
    nReturn = ( ( m_nMode == DBMWEB_TEMPLUSER_NEW )  || 
                ( m_nMode == DBMWEB_TEMPLUSER_EDIT )    ) ? 1 : 0;
  } else if( ISOBJ( OBJ_COM ) ) {
    nReturn = ( ( m_nMode == DBMWEB_TEMPLUSER_NEW )  || 
                ( m_nMode == DBMWEB_TEMPLUSER_EDIT )    ) ? 1 : 0;
  } else if( ISOBJ( OBJ_PWD2 ) ) {
    nReturn = ( ( m_nMode == DBMWEB_TEMPLUSER_NEW ) ||
                ( m_nMode == DBMWEB_TEMPLUSER_CHANGEPWD2 ) ||
                ( m_nMode == DBMWEB_TEMPLUSER_EDIT && m_oUser.SecondPwd( ) == "NO" ) ) ? 1 : 0;
  } else if( ISOBJ( OBJ_PWD2CON ) ) {
    nReturn = ( ( m_nMode == DBMWEB_TEMPLUSER_NEW ) ||
                ( m_nMode == DBMWEB_TEMPLUSER_CHANGEPWD2 ) ||
                ( m_nMode == DBMWEB_TEMPLUSER_EDIT && m_oUser.SecondPwd( ) == "NO" ) ) ? 1 : 0;
  } else if( ISOBJ( OBJ_PWD2CHANGE ) ) {
    if( ( m_nMode == DBMWEB_TEMPLUSER_NEW ) || 
        ( m_nMode == DBMWEB_TEMPLUSER_EDIT ) ) {
      nReturn = ( m_oUser.SecondPwd( ) == "YES" ) ? 1 : 0;
    } else {
      nReturn = 0;
    }
  } else if( ISOBJ( OBJ_SRVRIGLST ) ) {
    nReturn = ( ( m_nMode == DBMWEB_TEMPLUSER_NEW )  || 
                ( m_nMode == DBMWEB_TEMPLUSER_EDIT )    ) ? 1 : 0;
  } else if( ISOBJ( OBJ_SRVRIG ) ) {
    if( ( m_nMode == DBMWEB_TEMPLUSER_NEW ) || 
        ( m_nMode == DBMWEB_TEMPLUSER_EDIT ) ) {
      nReturn = m_oUser.ServerRightArray( ).GetSize( );
    } else {
      nReturn = 0;
    }
  } else if( ISOBJ( OBJ_SRVRIGVALS ) ) {
    nReturn = 1;
  } else if( ISOBJ( OBJ_BUTTONOKADD ) ) {
    nReturn = ( m_nMode == DBMWEB_TEMPLUSER_NEW ) ? 1 : 0;
  } else if( ISOBJ( OBJ_BUTTONOKPUT ) ) {
    nReturn = ( m_nMode == DBMWEB_TEMPLUSER_EDIT ) ? 1 : 0;
  } else if( ISOBJ( OBJ_BUTTONCHGPWD ) ) {
    nReturn = ( m_nMode == DBMWEB_TEMPLUSER_CHANGEPWD ) ? 1 : 0;
  } else if( ISOBJ( OBJ_BUTTONCHGPWD2 ) ) {
    nReturn = ( m_nMode == DBMWEB_TEMPLUSER_CHANGEPWD2 ) ? 1 : 0;
  } else if( ISOBJ( OBJ_BUTTONCANCEL ) ) {
    nReturn = 1;
  } else if( ISOBJ( OBJ_BUTTONDELETE ) ) {
    nReturn = ( m_nMode == DBMWEB_TEMPLUSER_EDIT ) ? 1 : 0;
  } 

  return nReturn;
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateUser :: askForValue
  -----------------------------------------------------------------------------
*/
Tools_DynamicUTF8String DBMWeb_TemplateUser :: askForValue ( const Tools_DynamicUTF8String & szName )
{
  const SAPDB_Char * pReturn = NULL;

  DBMCli_String sValue;
  
  if( ISOBJ( OBJ_BACK ) ) {
    sValue = m_sBackURL;
  } else if( ISOBJ( OBJ_TITLE ) ) {
    if( m_oUser.Name( ).IsEmpty( ) ) {
      sValue = "New User";
    } else {
      sValue = m_oUser.Name();
    }
  } else if( ISOBJ( OBJ_NAME ) ) {
    sValue = m_oUser.Name( );
  } else if( ISOBJ( OBJ_NAMEEDIT ) ) {
    sValue = m_oUser.Name( );
  } else if( ISOBJ( OBJ_DIS ) ) {
    sValue = m_oUser.Disabled( );
  } else if( ISOBJ( OBJ_COM ) ) {
    sValue = m_oUser.Comment( );
  } else if( ISOBJ( OBJ_SRVRIGNAME ) ) {
    DBMCli_UserRightArray & aRight = m_oUser.ServerRightArray( );
    sValue = aRight[m_nRight].Name( );    
  } else if( ISOBJ( OBJ_SRVRIGVAL ) ) {
    DBMCli_UserRightArray & aRight = m_oUser.ServerRightArray( );
    if( aRight[m_nRight].Value( ) == SAPDB_TRUE ) {
      sValue = "1";  
    } else {
      sValue = "0";  
    }
  } else if( ISOBJ( OBJ_SRVRIGDESC ) ) {
    DBMCli_UserRightArray & aRight = m_oUser.ServerRightArray( );
    sValue = aRight[m_nRight].Description( );    
    m_nRight += 1;
  }

  pReturn = sValue;

    return _Tools_UTF8Ptr(pReturn);
} 

