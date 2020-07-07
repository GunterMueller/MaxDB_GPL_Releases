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

  module: StudioWeb_Template.hpp

  -----------------------------------------------------------------------

  responsible:  WolfgangA

  special area: WebQuery

  description:  main frame template class for WebQuery

************************************************************************/

#ifndef STUDIOWEB_TEMPLATEMAINFRAME_HPP
#define STUDIOWEB_TEMPLATEMAINFRAME_HPP

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "ToolsCommon/Tools_Template.hpp"
#include "ToolsCommon/Tools_DynamicUTF8String.hpp"



/*!
  -----------------------------------------------------------------------------
  Class:        StudioWeb_TemplateMainFrame
  -----------------------------------------------------------------------------
  description:  An instance of this class represents a HTML-Page with a 
                Info Table.
  -----------------------------------------------------------------------------
*/
class StudioWeb_TemplateMainFrame : public Tools_Template 
{
  public:

     /*!-------------------------------------------------------------------------
		  function:     StudioWeb_WebQuery
		  description:  Constructor
      
		  return value: / 
      
		  arguments:    [in] wa,  webagent instance
      
      -------------------------------------------------------------------------*/
     StudioWeb_TemplateMainFrame (sapdbwa_WebAgent &wa,SAPDB_Bool  bUserConnected,Tools_DynamicUTF8String  sSessionID);
	 
	 
	


  private:
    virtual SAPDB_Int2              askForWriteCount(const Tools_DynamicUTF8String &szName);
    virtual Tools_DynamicUTF8String askForValue (const Tools_DynamicUTF8String &szName);
                                              
                                                  
    SAPDB_Bool m_bUserConnected;	
	Tools_DynamicUTF8String  m_sSessionID;	

}; 



#endif 