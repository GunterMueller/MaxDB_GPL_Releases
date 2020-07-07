/*!**********************************************************************

  module: StudioWeb_TemplateLogon.cpp

  -----------------------------------------------------------------------


  responsible:  WolfgangA


  special area: WebQuery

  description:  logon mask class for WebQuery

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
#include <string.h>
#include "SAPDB/SQLStudio/WebQuery/StudioWeb_TemplateLogon.hpp"

/*-----------------------------------------------------------------------------
  defines
 -----------------------------------------------------------------------------*/
#define TEMPLATE_NAME     "wqlogonmain.htm"

#define OBJ_SERVER        "Server"
#define OBJ_DATABASE      "Database"
#define OBJ_USER          "User"


StudioWeb_TemplateLogon :: StudioWeb_TemplateLogon ( sapdbwa_WebAgent & wa ) 
                      : Tools_Template ( wa, _Tools_UTF8Ptr(TEMPLATE_NAME )),
                        m_sServer(""),
                        m_sDatabase(""),
                        m_sUser("")
{ 
} 

StudioWeb_TemplateLogon :: StudioWeb_TemplateLogon (sapdbwa_WebAgent & wa,
													Tools_DynamicUTF8String &sServer,
													Tools_DynamicUTF8String &sDatabase,
													Tools_DynamicUTF8String &sUser )
                      : Tools_Template ( wa, _Tools_UTF8Ptr(TEMPLATE_NAME )),
                        m_sServer(sServer),
                        m_sDatabase(sDatabase),
                        m_sUser(sUser)
{ 
} 



SAPDB_Int2 StudioWeb_TemplateLogon :: askForWriteCount(const Tools_DynamicUTF8String &szName)
{
  SAPDB_Int2 nReturn = 0;

  if (szName.Compare( OBJ_SERVER) == 0) {
    nReturn = 1;
  }
  else if (szName.Compare( OBJ_DATABASE) == 0) {
    nReturn = 1;
  }
  else if (szName.Compare( OBJ_USER) == 0) {
    nReturn = 1;
  } // end if

  return nReturn;
} 


Tools_DynamicUTF8String StudioWeb_TemplateLogon :: askForValue (const Tools_DynamicUTF8String &szName)
{

  if (szName.Compare( OBJ_SERVER) == 0) {
    return m_sServer;
  } 
  else if (szName.Compare( OBJ_DATABASE) == 0) {
    return  m_sDatabase;
  }
  else if (szName.Compare( OBJ_USER) == 0) {
    return m_sUser;
  } // end if  

  return "";
}
