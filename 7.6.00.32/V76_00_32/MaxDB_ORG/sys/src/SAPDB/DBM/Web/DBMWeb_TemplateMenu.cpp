/*!
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateMenu.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  Menu page
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
#include "DBM/Web/DBMWeb_TemplateMenu.hpp"
#include "DBM/Cli/DBMCli_Stuff.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define TEMPLATE_NAME    "DBMMenu.htm"

#define OBJ_DBSTATE      "DBState"

/*
  =============================================================================
  class:        DBMWeb_TemplateMenu
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMWeb_TemplateMenu :: DBMWeb_TemplateMenu
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateMenu :: DBMWeb_TemplateMenu ( sapdbwa_WebAgent & wa,
                                             SAPDB_Int nDBState )
                        : Tools_Template( wa, _Tools_UTF8Ptr( TEMPLATE_NAME )),
                         m_nDBState( nDBState )
{
} 
 
/*
  -----------------------------------------------------------------------------
  public destructor DBMWeb_TemplateMenu :: ~DBMWeb_TemplateMenu
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateMenu :: ~DBMWeb_TemplateMenu (  )
{
} 


/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateMenu :: askForWriteCount
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateMenu :: askForWriteCount  ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  if (szName.Compare(OBJ_DBSTATE) == 0) {
    nReturn = 1;
  }
  
  return nReturn;
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateMenu :: askForValue
  -----------------------------------------------------------------------------
*/
Tools_DynamicUTF8String DBMWeb_TemplateMenu :: askForValue ( const Tools_DynamicUTF8String & szName )
{
  const SAPDB_Char * pReturn = NULL;

  DBMCli_String sValue;

  if (szName.Compare(OBJ_DBSTATE) == 0) {
    sValue.Format("%d", m_nDBState); 
    pReturn = sValue;
    
  }

  return _Tools_UTF8Ptr(pReturn);
} 

