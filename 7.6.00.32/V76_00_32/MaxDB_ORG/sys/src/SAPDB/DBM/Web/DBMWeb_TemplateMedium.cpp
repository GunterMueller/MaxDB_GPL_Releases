/*!
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateMedium.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  Medium
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
#include "DBM/Web/DBMWeb_TemplateMedium.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define TEMPLATE_NAME          "DBMMedium.htm"

#define OBJ_BACK               "BackURL"

#define OBJ_TITLE              "GroupBoxTitle"

#define OBJ_NAME               "Name"
#define OBJ_LOCATION           "Location"
#define OBJ_DEVTYPE            "DeviceType"
#define OBJ_BAKTYPE            "BackupType"

#define OBJ_SIZE               "Size"
#define OBJ_OVER               "Overwrite"
#define OBJ_AUTOL              "Autoloader"
#define OBJ_OSCMD              "OSCommand"

#define OBJ_BUTTONOKADD        "ButtonOKAdd"
#define OBJ_BUTTONOKPUT        "ButtonOKPut"
#define OBJ_BUTTONCANCEL       "ButtonCancel"
#define OBJ_BUTTONDELETE       "ButtonDelete"
/*
  =============================================================================
  class:        DBMWeb_TemplateMedium
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMWeb_TemplateMedium :: DBMWeb_TemplateMedium
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateMedium :: DBMWeb_TemplateMedium ( sapdbwa_WebAgent & wa,
                                                 DBMCli_Medium & oMedium,
                                                 DBMWeb_TemplMedium_Mode nMode,
                                                 const DBMCli_String & sBackURL)
                        : Tools_Template( wa, _Tools_UTF8Ptr( TEMPLATE_NAME )),
                            m_oMedium( oMedium ),
                            m_nMode( nMode ),
                            m_sBackURL( sBackURL )
{
} 

/*
  -----------------------------------------------------------------------------
  public destructor DBMWeb_TemplateMedium :: ~DBMWeb_TemplateMedium
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateMedium :: ~DBMWeb_TemplateMedium (  )
{
} 

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateMedium :: askForWriteCount
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateMedium :: askForWriteCount  ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  if (szName.Compare(OBJ_BACK) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_TITLE) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_NAME) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_LOCATION) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_DEVTYPE) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_BAKTYPE) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_SIZE) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_OVER) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_AUTOL) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_OSCMD) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_BUTTONOKADD) == 0) {
    nReturn = ( m_nMode == DBMWEB_TEMPLMEDIUM_NEW ) ? 1 : 0;
  } else if (szName.Compare(OBJ_BUTTONOKPUT) == 0) {
    nReturn = ( m_nMode == DBMWEB_TEMPLMEDIUM_EDIT ) ? 1 : 0;
  } else if (szName.Compare(OBJ_BUTTONCANCEL) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_BUTTONDELETE) == 0) {
    nReturn = ( m_nMode == DBMWEB_TEMPLMEDIUM_EDIT ) ? 1 : 0;
  } 

  return nReturn;
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateMedium :: askForValue
  -----------------------------------------------------------------------------
*/
Tools_DynamicUTF8String DBMWeb_TemplateMedium :: askForValue ( const Tools_DynamicUTF8String & szName )
{
  const SAPDB_Char * pReturn = NULL;

  DBMCli_String sValue;
  
  if (szName.Compare(OBJ_BACK) == 0) {
    pReturn = m_sBackURL;
  } else if (szName.Compare(OBJ_TITLE) == 0) {
    if( m_oMedium.Name( ).IsEmpty( ) ) {
      sValue = "New Medium";
    } else {
      sValue = m_oMedium.Name();
    }
    pReturn = sValue;
  } else if(szName.Compare(OBJ_NAME) == 0) {
    pReturn = m_oMedium.Name( );
  } else if(szName.Compare(OBJ_LOCATION) == 0) {
    pReturn = m_oMedium.Location( );
  } else if(szName.Compare(OBJ_DEVTYPE) == 0) {
    pReturn = m_oMedium.DeviceType( );
  } else if(szName.Compare(OBJ_BAKTYPE) == 0) {
    pReturn = m_oMedium.BackupType( );
  } else if(szName.Compare(OBJ_SIZE) == 0) {
    pReturn = m_oMedium.Pages( );
  } else if(szName.Compare(OBJ_OVER) == 0) {
    pReturn = m_oMedium.Overwrite( );
  } else if(szName.Compare(OBJ_AUTOL) == 0) {
    pReturn = m_oMedium.Autoloader( );
  } else if(szName.Compare(OBJ_OSCMD) == 0) {
    pReturn = m_oMedium.OSCommand( );
  } 

  return _Tools_UTF8Ptr(pReturn);
} 

