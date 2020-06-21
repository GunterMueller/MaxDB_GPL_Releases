/*!
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateVerify.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  verify database
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
#include "DBM/Web/DBMWeb_TemplateVerify.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define TEMPLATE_NAME          "DBMVerify.htm"

#define OBJ_START              "Start"
#define OBJ_WAIT               "Wait"
#define OBJ_RESULT             "Result"

/*
  =============================================================================
  class:        DBMWeb_TemplateVerify
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMWeb_TemplateVerify :: DBMWeb_TemplateVerify
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateVerify :: DBMWeb_TemplateVerify ( sapdbwa_WebAgent & wa,
                                                 DBMWeb_TemplVerify_Mode nMode )
                        : Tools_Template( wa, _Tools_UTF8Ptr( TEMPLATE_NAME )),
                          m_nMode( nMode )
{
} 

/*
  -----------------------------------------------------------------------------
  public destructor DBMWeb_TemplateVerify :: ~DBMWeb_TemplateVerify
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateVerify :: ~DBMWeb_TemplateVerify (  )
{
} 

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateVerify :: askForWriteCount
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateVerify :: askForWriteCount  ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  if (szName.Compare(OBJ_START) == 0) {
    nReturn = (m_nMode == DBMWEB_TEMPLVERIFY_MODE_START) ? 1 : 0;
  } else if (szName.Compare(OBJ_WAIT) == 0) {
    nReturn = (m_nMode == DBMWEB_TEMPLVERIFY_MODE_WAIT) ? 1 : 0;
  } else if (szName.Compare(OBJ_RESULT) == 0) {
    nReturn = (m_nMode == DBMWEB_TEMPLVERIFY_MODE_RESULT) ? 1 : 0;
  }
  return nReturn;
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateVerify :: askForValue
  -----------------------------------------------------------------------------
*/
Tools_DynamicUTF8String DBMWeb_TemplateVerify :: askForValue ( const Tools_DynamicUTF8String & szName )
{
  const SAPDB_Char * pReturn = NULL;

  DBMCli_String sValue;
  
  return _Tools_UTF8Ptr(pReturn);
} 

