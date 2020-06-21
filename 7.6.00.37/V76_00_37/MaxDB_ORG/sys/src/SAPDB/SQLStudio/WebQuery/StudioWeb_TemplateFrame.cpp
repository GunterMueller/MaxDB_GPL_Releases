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

  module: StudioWeb_TemplateFrame.cpp

  -----------------------------------------------------------------------

  responsible:  WolfgangA

  special area: WebQuery

  description:  inner frame template class for WebQuery

************************************************************************/

/*-----------------------------------------------------------------------------
  includes
 -----------------------------------------------------------------------------*/

#include "SAPDB/SQLStudio/WebQuery/StudioWeb_TemplateFrame.hpp"



/*-----------------------------------------------------------------------------
  defines
 -----------------------------------------------------------------------------*/
#define TEMPLATE_NAME     "wqframe.htm"

#define OBJ_SESSIONID    "SessionID*"
#define OBJ_TREE		 "Tree*"
#define OBJ_NOTREE	     "NoTree*"

#define VAL_SESSIONID    "SessionID"




StudioWeb_TemplateFrame::StudioWeb_TemplateFrame (sapdbwa_WebAgent & wa,Tools_DynamicUTF8String  sSessionID,SAPDB_Bool bWithTree) 
: Tools_Template ( wa, _Tools_UTF8Ptr(TEMPLATE_NAME) )
                      
{ 
	m_sSessionID = sSessionID;
	m_bWithTree = bWithTree;	
} 




SAPDB_Int2 StudioWeb_TemplateFrame::askForWriteCount(const Tools_DynamicUTF8String &szName)
{
  SAPDB_Int2 nReturn = 0;

  if (szName.Compare(OBJ_SESSIONID) == 0) {
    nReturn = 1;
  }  
  else if (szName.Compare( OBJ_TREE) == 0) {
    if (m_bWithTree) nReturn = 1;
  }
  else if (szName.Compare( OBJ_NOTREE) == 0) {
    if (!m_bWithTree) nReturn = 1;
  }
  


  return nReturn;
} 


Tools_DynamicUTF8String StudioWeb_TemplateFrame::askForValue (const Tools_DynamicUTF8String &szName)
{

  if (szName.Compare(VAL_SESSIONID) == 0) {

	  return m_sSessionID;	  
  }   
  return "";
}
