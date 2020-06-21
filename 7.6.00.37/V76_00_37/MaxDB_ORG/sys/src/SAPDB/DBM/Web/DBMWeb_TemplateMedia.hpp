/*! 
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateMedia.hpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  Media
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

#ifndef _DBMWEB_TEMPLATEMEDIA_HPP_
#define _DBMWEB_TEMPLATEMEDIA_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "ToolsCommon/Tools_Template.hpp"

#include "DBM/Cli/DBMCli_Database.hpp"
#include "DBM/Cli/DBMCli_Media.hpp"

/*
  -----------------------------------------------------------------------------
  typedefs
  -----------------------------------------------------------------------------
*/
typedef enum  
{ 
	DBMWEB_TEMPLMEDIA_SINGLE   = 0,
	DBMWEB_TEMPLMEDIA_PARALLEL = 1
} DBMWeb_TemplMedia_Mode;

/*!
  -----------------------------------------------------------------------------
  Class:        DBMWeb_TemplateMedia
  -----------------------------------------------------------------------------
  description:  An instance of this class represents a HTML-Page with  
                database Media.
  -----------------------------------------------------------------------------
*/
class DBMWeb_TemplateMedia : public Tools_Template 
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMWeb_TemplateMedia
      -------------------------------------------------------------------------
      description:  Constructor
      arguments:    wa [IN]  - webagent instance
      -------------------------------------------------------------------------
     */
     DBMWeb_TemplateMedia ( sapdbwa_WebAgent & wa,
                            DBMCli_Media & oMedia,
                            DBMWeb_TemplMedia_Mode nMode );
 
    /*!
      -------------------------------------------------------------------------
      function:     ~DBMWeb_TemplateMedia
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
     */
     ~DBMWeb_TemplateMedia ( );

    /*!
      -------------------------------------------------------------------------
      function:     GetMedia
      -------------------------------------------------------------------------
      description:  Get the Media object.
      return value: Reference to the Media object.
      -------------------------------------------------------------------------
    */
    DBMCli_Media & GetMedia ( ) { return m_oMedia; };


  private:
    virtual SAPDB_Int2              askForWriteCount ( const Tools_DynamicUTF8String & szName );
    virtual Tools_DynamicUTF8String askForValue      ( const Tools_DynamicUTF8String & szName );

  private:
    DBMCli_Media & m_oMedia;

    DBMWeb_TemplMedia_Mode m_nMode;


    SAPDB_Int2 m_nRow;
}; 

/*! EndClass: DBMWeb_TemplateMedia */

#endif // _DBMWEB_TEMPLATEMEDIA_HPP_

