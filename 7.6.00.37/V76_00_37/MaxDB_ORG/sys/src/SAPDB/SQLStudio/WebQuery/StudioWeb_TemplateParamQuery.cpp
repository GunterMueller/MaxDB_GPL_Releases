/*!**********************************************************************w

  module: StudioWeb_Template.cpp

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
#include <stdio.h>
#include "SQLStudio/WebQuery/StudioWeb_TemplateParamQuery.hpp"

/*-----------------------------------------------------------------------------
  defines
 -----------------------------------------------------------------------------*/
#define TEMPLATE_NAME     "wqparamquery.htm"

#define OBJ_PARAMSTATEMENT  "ParamStatement"
#define OBJ_ONEPARAM		"OneParam"
#define OBJ_ONEPARAMTEXT    "OneParamText"
#define OBJ_ONEPARAMINPUTNAME	"OneParamInputName"



StudioWeb_TemplateParamQuery::StudioWeb_TemplateParamQuery (sapdbwa_WebAgent & wa) 
: Tools_Template (wa,_Tools_UTF8Ptr(TEMPLATE_NAME))
                      
{ 
	m_nParams = 0;
	m_nCurrentParam = 0;
	m_nCurrentParamText = 0;
	m_nCurrentParamInputName = 0;
	
} 

void StudioWeb_TemplateParamQuery::setParam(const Tools_DynamicUTF8String &sParamText)
{
	++m_nParams;	
	
	m_ParamText[m_nParams] = sParamText;	

	m_ParamInputName[m_nParams] = (SAPDB_UTF8*)"T";
	//sBuffer contains a number, which can be represented in ascii7 => cast to (SAPDB_UTF8*) is valid
	char	sBuffer[64];
	sprintf(sBuffer,"%d",m_nParams);				
	m_ParamInputName[m_nParams].Append((SAPDB_UTF8*)sBuffer);	

	
}



SAPDB_Int2 StudioWeb_TemplateParamQuery::askForWriteCount(const Tools_DynamicUTF8String &szName)
{
  SAPDB_Int2 nReturn = 0;

  if (szName.Compare( OBJ_ONEPARAM) == 0) {
    //nReturn = (m_nCurrentParam < m_nParams) ? 1 : 0;
	  nReturn = m_nParams;
    ++m_nCurrentParam;
  }
  else if (szName.Compare( OBJ_ONEPARAMTEXT) == 0) {
    nReturn = 1;
  }
  else if (szName.Compare( OBJ_ONEPARAMINPUTNAME) == 0) {
    nReturn = 1;
  } // end if
  else if (szName.Compare( OBJ_PARAMSTATEMENT) == 0) {
    nReturn = 1;
  } // end if
  

  return nReturn;
} 


Tools_DynamicUTF8String StudioWeb_TemplateParamQuery::askForValue (const Tools_DynamicUTF8String &szName)
{
  
  if (szName.Compare( OBJ_ONEPARAM) == 0) {

	  return Tools_DynamicUTF8String();	  
  } 
  else if (szName.Compare( OBJ_ONEPARAMTEXT) == 0) {

	  ++m_nCurrentParamText;
	  return  m_ParamText[m_nCurrentParamText];	  
  } 
  else if (szName.Compare( OBJ_ONEPARAMINPUTNAME) == 0) {

	  ++m_nCurrentParamInputName;	
	  return m_ParamInputName[m_nCurrentParamInputName];
  }
  else if (szName.Compare( OBJ_PARAMSTATEMENT) == 0) {

    return m_sStatement;
  } 	

  return Tools_DynamicUTF8String();
}

void StudioWeb_TemplateParamQuery::resetStartValues ()
{
	m_nCurrentParam = 0;
	m_nCurrentParamText = 0;
	m_nCurrentParamInputName = 0;
}