/*!**********************************************************************

  module: StudioWeb_TemplateHeader.cpp

  -----------------------------------------------------------------------

  responsible:  WolfgangA

  special area: WebQuery

  description:  logo header for WebQuery

  see also:		/

  -----------------------------------------------------------------------

  copyright:                Copyright (c) 2000-2005 SAP AG




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




************************************************************************/

/*-----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------*/
#include "SAPDB/SQLStudio/WebQuery/StudioWeb_TemplateHeader.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define TEMPLATE_NAME    "wqlogonmenu.htm"

#define OBJ_LOGOFF       "LogOff"
#define OBJ_SERVER       "Server"
#define OBJ_DATABASE     "Database"
#define OBJ_USER         "User"


/*-----------------------------------------------------------------------------
  public constructor StudioWeb_TemplateHeader::StudioWeb_TemplateHeader
  -----------------------------------------------------------------------------*/
StudioWeb_TemplateHeader::StudioWeb_TemplateHeader(sapdbwa_WebAgent & wa)
                       : Tools_Template( wa, _Tools_UTF8Ptr(TEMPLATE_NAME)),
                         m_bLogOff( SAPDB_FALSE),
                         m_sServer(""),
                         m_sDatabase(""),
                         m_sUser("")
{
} 

StudioWeb_TemplateHeader::StudioWeb_TemplateHeader(sapdbwa_WebAgent & wa,
                                                 SAPDB_Bool         bLogOff,
                                                 Tools_DynamicUTF8String &sServer,
												 Tools_DynamicUTF8String &sDatabase,
												 Tools_DynamicUTF8String &sUser)
                       : Tools_Template( wa, _Tools_UTF8Ptr(TEMPLATE_NAME)),
                         m_bLogOff( bLogOff),
                         m_sServer(sServer),
                         m_sDatabase(sDatabase),
                         m_sUser(sUser)
{
} 
 
/*-----------------------------------------------------------------------------
  public destructor StudioWeb_TemplateHeader::~StudioWeb_TemplateHeader
  -----------------------------------------------------------------------------*/
StudioWeb_TemplateHeader::~StudioWeb_TemplateHeader()
{
} 


/*-----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
  askForWriteCount
  -----------------------------------------------------------------------------*/
SAPDB_Int2 StudioWeb_TemplateHeader::askForWriteCount(const Tools_DynamicUTF8String &szName)
{  
  if (szName.Compare( OBJ_LOGOFF) == 0) {
    return ((m_bLogOff == SAPDB_TRUE) ? 1 : 0);
  }
  else if (szName.Compare( OBJ_SERVER) == 0) {
    if(m_sServer.Compare("") == 0) return 0; 
    return 1;
  }
  else if (szName.Compare( OBJ_DATABASE) == 0) {
    return 1;
  }
  else if (szName.Compare( OBJ_USER) == 0) {
    return 1;
  } 
  
  return 0;
} 

/*-----------------------------------------------------------------------------
  askForValue
  -----------------------------------------------------------------------------*/
Tools_DynamicUTF8String StudioWeb_TemplateHeader::askForValue(const Tools_DynamicUTF8String &szName)
{
  if (szName.Compare( OBJ_SERVER) == 0) {
    return m_sServer;
  } 
  else if (szName.Compare( OBJ_DATABASE) == 0) {
    return  m_sDatabase;
  }
  else if (szName.Compare( OBJ_USER) == 0) {
    return m_sUser;
  } 

  return Tools_DynamicUTF8String();
} 

