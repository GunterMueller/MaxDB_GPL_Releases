/*!
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateDevspace.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  devspace
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
#include "DBM/Web/DBMWeb_TemplateDevspace.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define ISOBJ(obj)  (szName.Compare(obj) == 0)

#define TEMPLATE_NAME          "DBMDevspace.htm"

#define OBJ_BACK               "BackURL"

#define OBJ_MSG                "Message"

#define OBJ_TITLE              "GroupBoxTitle"

#define OBJ_NAME               "Name"
#define OBJ_SIZE               "Size"
#define OBJ_SIZEEDIT           "SizeEdit"
#define OBJ_LOCATION           "Location"
#define OBJ_TYPEFILE           "TypeFile"
#define OBJ_TYPELINK           "TypeLink"
#define OBJ_TYPERAW            "TypeRaw"

#define OBJ_MLOCATION          "MLocation"
#define OBJ_MTYPEFILE          "MTypeFile"
#define OBJ_MTYPELINK          "MTypeLink"
#define OBJ_MTYPERAW           "MTypeRaw"

#define OBJ_INFO               "Info"

#define OBJ_BUTTONOKADD        "ButtonOKAdd"
#define OBJ_BUTTONOKPUT        "ButtonOKPut"
#define OBJ_BUTTONCANCEL       "ButtonCancel"
/*
  =============================================================================
  class:        DBMWeb_TemplateDevspace
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMWeb_TemplateDevspace :: DBMWeb_TemplateDevspace
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateDevspace :: DBMWeb_TemplateDevspace ( sapdbwa_WebAgent & wa,
                                                     DBMCli_Devspace & oDevspace,
                                                     DBMWeb_TemplDevspace_Mode nMode,
                                                     const DBMCli_String & sBackURL,
                                                     const DBMCli_String & sMessage )
                        : Tools_Template( wa, _Tools_UTF8Ptr( TEMPLATE_NAME )),
                           m_oDevspace( oDevspace ),
                           m_nMode( nMode ),
                           m_sBackURL( sBackURL ),
                           m_sMessage( sMessage )
{
} 

/*
  -----------------------------------------------------------------------------
  public destructor DBMWeb_TemplateDevspace :: ~DBMWeb_TemplateDevspace
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateDevspace :: ~DBMWeb_TemplateDevspace (  )
{
} 

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateDevspace :: askForWriteCount
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateDevspace :: askForWriteCount  ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  if( ISOBJ( OBJ_BACK ) ) {
    nReturn = 1;
  } else if( ISOBJ( OBJ_TITLE ) ) {
    nReturn = 1;
  } else if( ISOBJ( OBJ_MSG ) ) {
    nReturn = m_sMessage.IsEmpty( ) ? 0 : 1; 
  } else if( ISOBJ( OBJ_NAME ) ) {
    nReturn = 1;
  } else if( ISOBJ( OBJ_SIZE ) ) {
    nReturn = ( ( m_nMode == DBMWEB_TEMPLDEVSPACE_EDIT )             || 
                ( m_oDevspace.Class( ) == DBMCLI_DEVSPACECLASS_SYS )    ) ? 1 : 0;
  } else if( ISOBJ( OBJ_SIZEEDIT ) ) {
    nReturn = ( ( ( m_nMode == DBMWEB_TEMPLDEVSPACE_NEW )  ||
                  ( m_nMode == DBMWEB_TEMPLDEVSPACE_INIT )   
                ) && ( m_oDevspace.Class( ) != DBMCLI_DEVSPACECLASS_SYS ) ) ? 1 : 0;
  } else if( ISOBJ( OBJ_LOCATION ) ) {
    nReturn = 1;
  } else if( ISOBJ( OBJ_TYPEFILE ) ) {
    nReturn = ( m_oDevspace.Type( ) ==  DBMCLI_DEVSPACETYPE_FILE ) ? 1 : 0;
  } else if( ISOBJ( OBJ_TYPELINK ) ) {
    nReturn = ( m_oDevspace.Type( ) ==  DBMCLI_DEVSPACETYPE_LINK ) ? 1 : 0;
  } else if( ISOBJ( OBJ_TYPERAW ) ) {
    nReturn = ( m_oDevspace.Type( ) ==  DBMCLI_DEVSPACETYPE_RAW ) ? 1 : 0;
  } else if( ISOBJ( OBJ_MLOCATION ) ) {
    nReturn = ( m_oDevspace.Mirrored( ) ) ? 1 : 0;
  } else if( ISOBJ( OBJ_MTYPEFILE ) ) {
    nReturn = ( ( m_oDevspace.Mirrored( ) ) &&
                ( m_oDevspace.MirroredType( ) == DBMCLI_DEVSPACETYPE_FILE ) ) ? 1 : 0;
  } else if( ISOBJ( OBJ_MTYPELINK ) ) {
    nReturn = ( ( m_oDevspace.Mirrored( ) ) &&
                ( m_oDevspace.MirroredType( ) == DBMCLI_DEVSPACETYPE_LINK ) ) ? 1 : 0;
  } else if( ISOBJ( OBJ_MTYPERAW ) ) {
    nReturn = ( ( m_oDevspace.Mirrored( ) ) &&
                ( m_oDevspace.MirroredType( ) == DBMCLI_DEVSPACETYPE_RAW ) ) ? 1 : 0;
  } else if( ISOBJ( OBJ_INFO ) ) {
    nReturn = 1;
  } else if( ISOBJ( OBJ_BUTTONOKADD ) ) {
    nReturn = ( m_nMode == DBMWEB_TEMPLDEVSPACE_NEW ) ? 1 : 0;
  } else if( ISOBJ( OBJ_BUTTONOKPUT ) ) {
    nReturn = ( ( m_nMode == DBMWEB_TEMPLDEVSPACE_EDIT ) ||
                ( m_nMode == DBMWEB_TEMPLDEVSPACE_INIT )    ) ? 1 : 0;
  } else if( ISOBJ( OBJ_BUTTONCANCEL ) ) {
    nReturn = 1;
  } 

  return nReturn;
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateDevspace :: askForValue
  -----------------------------------------------------------------------------
*/
Tools_DynamicUTF8String DBMWeb_TemplateDevspace :: askForValue ( const Tools_DynamicUTF8String & szName )
{
  const SAPDB_Char * pReturn = NULL;

  DBMCli_String sValue;
  
  if( ISOBJ( OBJ_BACK ) ) {

    pReturn = m_sBackURL;

  } else if( ISOBJ( OBJ_MSG ) ) {

    pReturn = m_sMessage; 

  } else if( ISOBJ( OBJ_TITLE ) ) {

    sValue = m_oDevspace.Name();
    pReturn = sValue;

  } else if( ISOBJ( OBJ_NAME ) ) {

    pReturn = m_oDevspace.Name( );

  } else if( ISOBJ( OBJ_SIZE ) ) {

    sValue.Format( "%d", m_oDevspace.Pages( ) );
    pReturn = sValue;

  } else if( ISOBJ( OBJ_SIZEEDIT ) ) {

    sValue.Format( "%d", m_oDevspace.Pages( ) );
    pReturn = sValue;

  } else if( ISOBJ( OBJ_LOCATION ) ) {

    pReturn = m_oDevspace.Location( );

  } else if( ISOBJ( OBJ_MLOCATION ) ) {

    pReturn = m_oDevspace.MirroredLocation( );

  } else if( ISOBJ( OBJ_INFO ) ) {

    pReturn = "&nbsp;";

  } else if( ISOBJ( OBJ_BUTTONOKADD ) ) {

    sValue = "'" + m_oDevspace.Name( ) + "','" + m_oDevspace.ClassName( ) + "'";
    pReturn = sValue;

  } else if( ISOBJ( OBJ_BUTTONOKPUT ) ) {

    sValue = "'" + m_oDevspace.Name( ) + "','" + m_oDevspace.ClassName( ) + "'";
    pReturn = sValue;

  } else if( ISOBJ( OBJ_BUTTONCANCEL ) ) {

    sValue = "'" + m_oDevspace.ClassName( ) + "'";
    pReturn = sValue;

  } 

  return _Tools_UTF8Ptr(pReturn);
} 

