/*!
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateSysTabs.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  upgrade system tables
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
#include "DBM/Web/DBMWeb_TemplateSysTabs.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define TEMPLATE_NAME          "DBMSysTabs.htm"

#define OBJ_START              "Start"
#define OBJ_WAIT               "Wait*"
#define OBJ_RESULT             "Result"

#define OBJ_DBANAME            "DBAName" 
#define OBJ_DBAPWD             "DBAPwd"
#define OBJ_DOMPWD             "DOMPwd"

/*
  =============================================================================
  class:        DBMWeb_TemplateSysTabs
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMWeb_TemplateSysTabs :: DBMWeb_TemplateSysTabs
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateSysTabs :: DBMWeb_TemplateSysTabs ( sapdbwa_WebAgent & wa,
                                                   const DBMCli_String & sDBAName,
                                                   const DBMCli_String & sDBAPwd,
                                                   const DBMCli_String & sDOMPwd,
                                                   DBMWeb_TemplSysTabs_Mode nMode )
                        : Tools_Template( wa, _Tools_UTF8Ptr( TEMPLATE_NAME )),
                          m_sDBAName( sDBAName ),
                          m_sDBAPwd( sDBAPwd ),
                          m_sDOMPwd( sDOMPwd ),
                          m_nMode( nMode )
{
} 

/*
  -----------------------------------------------------------------------------
  public destructor DBMWeb_TemplateSysTabs :: ~DBMWeb_TemplateSysTabs
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateSysTabs :: ~DBMWeb_TemplateSysTabs (  )
{
} 

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateSysTabs :: askForWriteCount
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateSysTabs :: askForWriteCount  ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  if (szName.Compare(OBJ_START) == 0) {
    nReturn = (m_nMode == DBMWEB_TEMPLSYSTABS_MODE_START) ? 1 : 0;
  } else if (szName.Compare(OBJ_WAIT) == 0) {
    nReturn = (m_nMode == DBMWEB_TEMPLSYSTABS_MODE_WAIT) ? 1 : 0;
  } else if (szName.Compare(OBJ_RESULT) == 0) {
    nReturn = (m_nMode == DBMWEB_TEMPLSYSTABS_MODE_RESULT) ? 1 : 0;
  }
  return nReturn;
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateSysTabs :: askForValue
  -----------------------------------------------------------------------------
*/
Tools_DynamicUTF8String DBMWeb_TemplateSysTabs :: askForValue ( const Tools_DynamicUTF8String & szName )
{
  const SAPDB_Char * pReturn = NULL;

  DBMCli_String sValue;

  sValue = "";

  if (szName.Compare(OBJ_DBANAME) == 0) {
    sValue = m_sDBAName;
  } else if (szName.Compare(OBJ_DBAPWD) == 0) {
    sValue = m_sDBAPwd;
  } else if (szName.Compare(OBJ_DOMPWD) == 0) {
    sValue = m_sDOMPwd;
  }

  pReturn = sValue;

  return _Tools_UTF8Ptr(pReturn);
} 

