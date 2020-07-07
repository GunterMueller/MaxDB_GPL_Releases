/*! 
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateHeader.hpp
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

#ifndef _DBMWEB_TEMPLATEHEADER_HPP_
#define _DBMWEB_TEMPLATEHEADER_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "ToolsCommon/Tools_Template.hpp"
#include "DBM/Cli/DBMCli_Stuff.hpp"

/*
  -----------------------------------------------------------------------------
  typedefs
  -----------------------------------------------------------------------------
*/
typedef enum  
{ 
	DBMWEB_TEMPLHEADER_MODE_MANAGER  = 0,
	DBMWEB_TEMPLHEADER_MODE_WIZARD   = 1
} DBMWeb_TemplHeader_Mode;

/*!
  -----------------------------------------------------------------------------
  Class:        DBMWeb_TemplateHeader
  -----------------------------------------------------------------------------
  description:  An instance of this class represents the HTML-Header-Page.
  -----------------------------------------------------------------------------
*/
class DBMWeb_TemplateHeader : public Tools_Template 
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMWeb_TemplateHeader
      -------------------------------------------------------------------------
      description:  Constructor
      arguments:    wa [IN] - webagent instance
      prototypes:   2
      -------------------------------------------------------------------------
     */
     DBMWeb_TemplateHeader ( sapdbwa_WebAgent & wa,
                             DBMWeb_TemplHeader_Mode nMode );

     DBMWeb_TemplateHeader ( sapdbwa_WebAgent & wa,
                             DBMWeb_TemplHeader_Mode nMode,
                             SAPDB_Bool bLogOff,
                             DBMCli_String & sName );
    /*!
      -------------------------------------------------------------------------
      function:     ~DBMWeb_TemplateHeader
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
     */
     ~DBMWeb_TemplateHeader ( );


  private:
    virtual SAPDB_Int2              askForWriteCount ( const Tools_DynamicUTF8String & szName );
    virtual Tools_DynamicUTF8String askForValue      ( const Tools_DynamicUTF8String & szName );
                                              
  private:
    SAPDB_Bool m_bLogOff;
    DBMCli_String m_sName;

    DBMWeb_TemplHeader_Mode m_nMode;
}; 

/*! EndClass: DBMWeb_TemplateHeader */

#endif // _DBMWEB_TEMPLATEHEADER_HPP_

