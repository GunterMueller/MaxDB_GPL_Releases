/*

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

/*!**********************************************************************

  module: StudioWeb_TemplateMainFrame.cpp

  -----------------------------------------------------------------------

  responsible:  WolfgangA

  special area: WebQuery

  description:  sql window template class for WebQuery

************************************************************************/

/*-----------------------------------------------------------------------------
  includes
 -----------------------------------------------------------------------------*/

#include "SAPDB/SQLStudio/WebQuery/StudioWeb_TemplateMainFrame.hpp"



/*-----------------------------------------------------------------------------
  defines
 -----------------------------------------------------------------------------*/
#define TEMPLATE_NAME     "wqlogon.htm"

#define OBJ_USERCONNECTED		"UserConnected*"
#define OBJ_USERNOTCONNECTED    "UserNotConnected"


#define OBJ_SESSIONID    "SessionID*"
#define VAL_SESSIONID    "SessionID"


StudioWeb_TemplateMainFrame::StudioWeb_TemplateMainFrame (sapdbwa_WebAgent & wa,SAPDB_Bool  bUserConnected,Tools_DynamicUTF8String  sSessionID) 
: Tools_Template ( wa, _Tools_UTF8Ptr(TEMPLATE_NAME) )
                      
{ 
	m_bUserConnected = bUserConnected;	
	m_sSessionID = sSessionID;
} 




SAPDB_Int2 StudioWeb_TemplateMainFrame::askForWriteCount(const Tools_DynamicUTF8String &szName)
{
  SAPDB_Int2 nReturn = 0;

  if (szName.Compare(OBJ_USERCONNECTED) == 0) {
    if(m_bUserConnected) nReturn = 1;
  }    
  if (szName.Compare(OBJ_USERNOTCONNECTED) == 0) {
    if(!m_bUserConnected) nReturn = 1;
  }    
  if (szName.Compare(OBJ_SESSIONID) == 0) {
    nReturn = 1;
  }  

  return nReturn;
} 


Tools_DynamicUTF8String StudioWeb_TemplateMainFrame::askForValue (const Tools_DynamicUTF8String &szName)
{  
  if (szName.Compare(VAL_SESSIONID) == 0) {

	  return m_sSessionID;	  
  }   

  return "";
}
