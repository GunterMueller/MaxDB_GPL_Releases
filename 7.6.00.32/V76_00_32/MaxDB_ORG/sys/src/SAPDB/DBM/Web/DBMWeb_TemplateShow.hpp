/*! 
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateShow.hpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  view show
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

#ifndef _DBMWEB_TEMPLATESHOW_HPP_
#define _DBMWEB_TEMPLATESHOW_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "ToolsCommon/Tools_Template.hpp"

#include "DBM/Cli/DBMCli_Database.hpp"
#include "DBM/Cli/DBMCli_File.hpp"

/*!
  -----------------------------------------------------------------------------
  Class:        DBMWeb_TemplateShow
  -----------------------------------------------------------------------------
  description:  An instance of this class represents a HTML-Page with the 
                content of a show.
  -----------------------------------------------------------------------------
*/
class DBMWeb_TemplateShow : public Tools_Template 
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMWeb_TemplateShow
      -------------------------------------------------------------------------
      description:  Constructor
      arguments:    wa [IN]  - webagent instance
      -------------------------------------------------------------------------
     */
     DBMWeb_TemplateShow ( sapdbwa_WebAgent & wa,
                           DBMCli_Show & oShow );
 
    /*!
      -------------------------------------------------------------------------
      function:     ~DBMWeb_TemplateShow
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
     */
     ~DBMWeb_TemplateShow ( );

    /*!
      -------------------------------------------------------------------------
      function:     GetShow
      -------------------------------------------------------------------------
      description:  Get the Show object.
      return value: Reference to the Show object.
      -------------------------------------------------------------------------
    */
    DBMCli_Show & GetShow ( ) { return m_oShow; };


  private:
    virtual SAPDB_Int2              askForWriteCount ( const Tools_DynamicUTF8String & szName );
    virtual Tools_DynamicUTF8String askForValue      ( const Tools_DynamicUTF8String & szName );
    virtual bool                    askForContinue   ( const Tools_DynamicUTF8String & szName );
                                              
  private:
     DBMCli_Show & m_oShow;

     DBMCli_String m_sLine;
}; 

/*! EndClass: DBMWeb_TemplateShow */

#endif // _DBMWEB_TEMPLATESHOW_HPP_

