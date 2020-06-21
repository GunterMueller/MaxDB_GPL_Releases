/*!
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateLogoff.cpp
  -----------------------------------------------------------------------------
  responsible:  
  special area: Database Manager WebAgent Extension
  description:  a class for the Logoff mask of the Web DBM
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
#include "DBM/Web/DBMWeb_TemplateLogoff.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define TEMPLATE_NAME     "DBMLogoff.htm"

/*
  =============================================================================
  class:        DBMWeb_TemplateLogoff
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMWeb_TemplateLogoff :: DBMWeb_TemplateLogoff
  -----------------------------------------------------------------------------
*/

DBMWeb_TemplateLogoff :: DBMWeb_TemplateLogoff ( sapdbwa_WebAgent & wa ) 
                       : Tools_Template( wa, _Tools_UTF8Ptr( TEMPLATE_NAME ))
{ 
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateLogoff :: askForWriteCount
  -----------------------------------------------------------------------------
 */
SAPDB_Int2 DBMWeb_TemplateLogoff :: askForWriteCount  ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  return nReturn;
} // end DBMWeb_TemplateLogoff :: askForWriteCount

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateLogoff :: askForValue
  -----------------------------------------------------------------------------
 */
Tools_DynamicUTF8String DBMWeb_TemplateLogoff :: askForValue  ( const Tools_DynamicUTF8String & szName )
{
  const SAPDB_Char * pReturn = "";

  return _Tools_UTF8Ptr(pReturn);
} // end DBMWeb_TemplateLogoff :: askForValue
