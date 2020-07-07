/*!
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateHeader.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  header page
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
#include <string.h>
#include "DBM/Web/DBMWeb_TemplateHeader.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define TEMPLATE_NAME    "DBMHeader.htm"

#define OBJ_MNG          "Manager"
#define OBJ_WIZ          "Wizard"

#define OBJ_CLOSEWIZ     "CloseWizard"
#define OBJ_LOGOFF       "LogOff"

/*
  =============================================================================
  class:        DBMWeb_TemplateHeader
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMWeb_TemplateHeader :: DBMWeb_TemplateHeader
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateHeader :: DBMWeb_TemplateHeader ( sapdbwa_WebAgent & wa,
                                                 DBMWeb_TemplHeader_Mode nMode )
                        : Tools_Template( wa, _Tools_UTF8Ptr( TEMPLATE_NAME )),
                         m_nMode( nMode ),
                         m_bLogOff( SAPDB_FALSE )
{
} 

DBMWeb_TemplateHeader :: DBMWeb_TemplateHeader ( sapdbwa_WebAgent & wa,
                                                 DBMWeb_TemplHeader_Mode nMode,
                                                 SAPDB_Bool bLogOff,
                                                 DBMCli_String & sName)
                        : Tools_Template( wa, _Tools_UTF8Ptr( TEMPLATE_NAME )),
                         m_nMode( nMode ),
                         m_bLogOff( bLogOff ),
                         m_sName( sName )
{
} 
 
/*
  -----------------------------------------------------------------------------
  public destructor DBMWeb_TemplateHeader :: ~DBMWeb_TemplateHeader
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateHeader :: ~DBMWeb_TemplateHeader (  )
{
} 


/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateHeader :: askForWriteCount
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateHeader :: askForWriteCount  ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  if (szName.Compare(OBJ_LOGOFF) == 0) {
    nReturn = (m_bLogOff == SAPDB_TRUE) ? 1 : 0;
  } else if (szName.Compare(OBJ_MNG) == 0) {
    nReturn = (m_nMode == DBMWEB_TEMPLHEADER_MODE_MANAGER) ? 1 : 0;
  } else if (szName.Compare(OBJ_WIZ) == 0) {
    nReturn = (m_nMode == DBMWEB_TEMPLHEADER_MODE_WIZARD) ? 1 : 0;
  } else if (szName.Compare(OBJ_CLOSEWIZ) == 0) {
    nReturn = (m_nMode == DBMWEB_TEMPLHEADER_MODE_WIZARD) ? 1 : 0;
  }
  
  return nReturn;
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateHeader :: askForValue
  -----------------------------------------------------------------------------
*/
Tools_DynamicUTF8String DBMWeb_TemplateHeader :: askForValue ( const Tools_DynamicUTF8String & szName )
{
  const SAPDB_Char * pReturn = NULL;

  DBMCli_String sValue;

  if (szName.Compare(OBJ_MNG) == 0) {
    if (m_sName.GetLength() > 0) {
      sValue = "&nbsp;-&nbsp;";
      sValue +=  m_sName;
      pReturn = sValue;
    }
  }

  return _Tools_UTF8Ptr(pReturn);
} 

