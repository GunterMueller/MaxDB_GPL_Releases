/*! 
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateShows.hpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  creating list of shows
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

#ifndef _DBMWEB_TEMPLATESHOWS_HPP_
#define _DBMWEB_TEMPLATESHOWS_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "ToolsCommon/Tools_Template.hpp"

#include "DBM/Cli/DBMCli_Database.hpp"
#include "DBM/Cli/DBMCli_Show.hpp"

/*!
  -----------------------------------------------------------------------------
  Class:        DBMWeb_TemplateShows
  -----------------------------------------------------------------------------
  description:  An instance of this class represents a HTML-Page with a 
                list of shows.
  -----------------------------------------------------------------------------
*/
class DBMWeb_TemplateShows : public Tools_Template 
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMWeb_TemplateShows
      -------------------------------------------------------------------------
      description:  Constructor
      arguments:    wa [IN]  - webagent instance
      -------------------------------------------------------------------------
     */
     DBMWeb_TemplateShows ( sapdbwa_WebAgent & wa,
                            DBMCli_Shows & oShows );
 
    /*!
      -------------------------------------------------------------------------
      function:     ~DBMWeb_TemplateShows
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
     */
     ~DBMWeb_TemplateShows ( );

    /*!
      -------------------------------------------------------------------------
      function:     GetShows
      -------------------------------------------------------------------------
      description:  Get the shows object.
      return value: Reference to the shows object.
      -------------------------------------------------------------------------
    */
    DBMCli_Shows & GetShows( ) { return m_oShows; };


  private:
    virtual SAPDB_Int2              askForWriteCount ( const Tools_DynamicUTF8String & szName );
    virtual Tools_DynamicUTF8String askForValue      ( const Tools_DynamicUTF8String & szName );
                                              
  private:
     DBMCli_Shows & m_oShows;

     SAPDB_Int2 m_nRow;
     SAPDB_Int2 m_nCol;
}; 

/*! EndClass: DBMWeb_TemplateShows */

#endif // _DBMWEB_TEMPLATESHOWS_HPP_

