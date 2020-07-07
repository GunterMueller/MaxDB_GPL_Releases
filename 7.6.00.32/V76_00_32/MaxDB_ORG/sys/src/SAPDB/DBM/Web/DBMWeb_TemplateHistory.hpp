/*! 
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateHistory.hpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  parameters
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

#ifndef _DBMWEB_TEMPLATEHISTORY_HPP_
#define _DBMWEB_TEMPLATEHISTORY_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "ToolsCommon/Tools_Template.hpp"

#include "DBM/Cli/DBMCli_Database.hpp"
#include "DBM/Cli/DBMCli_History.hpp"

/*!
  -----------------------------------------------------------------------------
  Class:        DBMWeb_TemplateHistory
  -----------------------------------------------------------------------------
  description:  An instance of this class represents a HTML-Page with  
                a backup history.
  -----------------------------------------------------------------------------
*/
class DBMWeb_TemplateHistory : public Tools_Template 
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMWeb_TemplateHistory
      -------------------------------------------------------------------------
      description:  Constructor
      arguments:    wa [IN]  - webagent instance
      -------------------------------------------------------------------------
    */
    DBMWeb_TemplateHistory ( sapdbwa_WebAgent & wa,
                             DBMCli_History & oHistory,
                             DBMCli_HistoryItemArray & aItems );

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMWeb_TemplateHistory
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMWeb_TemplateHistory ( );

    /*!
      -------------------------------------------------------------------------
      function:     GetHistory
      -------------------------------------------------------------------------
      description:  Get the history object.
      return value: Reference to the history object.
      -------------------------------------------------------------------------
    */
    DBMCli_History & GetHistory ( ) { return m_oHistory; };


  private:
    virtual SAPDB_Int2              askForWriteCount ( const Tools_DynamicUTF8String & szName );
    virtual Tools_DynamicUTF8String askForValue      ( const Tools_DynamicUTF8String & szName );
    virtual bool                    askForContinue   ( const Tools_DynamicUTF8String & szName );
    virtual bool                  initTemplate ( );

    bool FindNextItem ( );

  private:
    DBMCli_History & m_oHistory;
    DBMCli_HistoryItemArray & m_aItems;

    SAPDB_Int2 m_nItem;
}; 

/*! EndClass: DBMWeb_TemplateHistory */

#endif // _DBMWEB_TEMPLATEHISTORY_HPP_

