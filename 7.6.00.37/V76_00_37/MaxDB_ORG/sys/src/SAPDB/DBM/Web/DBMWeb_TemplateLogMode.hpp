/*! 
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateLogMode.hpp
  -----------------------------------------------------------------------------
  responsible:  
  special area: Database Manager WebAgent Extension
  description:  HTML Template for Config Log Mode
  -----------------------------------------------------------------------------
  Copyright (c) 2000-2005 SAP AG


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

#ifndef _DBMWEB_TEMPLATELOGMODE_HPP_
#define _DBMWEB_TEMPLATELOGMODE_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "ToolsCommon/Tools_Template.hpp"
#include "DBM/Cli/DBMCli_Database.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/

/*!
  -----------------------------------------------------------------------------
  Class:        DBMWeb_TemplateLogMode
  -----------------------------------------------------------------------------
  description:  An instance of this class represents a HTML-Page for  
                Config Log Mode.
  -----------------------------------------------------------------------------
*/
class DBMWeb_TemplateLogMode : public Tools_Template 
{
  public:

    /*!
      -------------------------------------------------------------------------
      Declaration:  Public types
      Description:  Public types used by public member functions.
      -------------------------------------------------------------------------
    */
    typedef enum {
      View,
      Edit
    } Mode;

    /*! EndDeclaration:  Public types */

    /*!
      -------------------------------------------------------------------------
      function:     DBMWeb_TemplateLogMode
      -------------------------------------------------------------------------
      description:  Constructor
      arguments:    wa [IN]  - webagent instance
      -------------------------------------------------------------------------
    */
    DBMWeb_TemplateLogMode ( sapdbwa_WebAgent          & wa,
                             Mode                        nMode,
                             DBMCli_LogModeValue::Mode   nNewLogMode,
                             SAPDB_Bool                  bRestart,
                             DBMCli_Database           * pDatabase );

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMWeb_TemplateLogMode
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMWeb_TemplateLogMode ( );

    /*!
      -------------------------------------------------------------------------
      function:     SetCurrentDevspace
      -------------------------------------------------------------------------
      description:  sets the selected devspace for edit
      -------------------------------------------------------------------------
    */
    DBMWeb_TemplateLogMode & SetCurrentDevspace(const DBMCli_String & sDevspace);

    /*!
      -------------------------------------------------------------------------
      function:     SetRefreshState
      -------------------------------------------------------------------------
      description:  activate the refreshing of the database state frame
      -------------------------------------------------------------------------
    */
    DBMWeb_TemplateLogMode & SetRefreshState(SAPDB_Bool bRefresh = true) {m_bRefreshState = bRefresh; return *this;}

  private:
    virtual bool initTemplate                     ( );

    virtual SAPDB_Int2              askForWriteCount ( const Tools_DynamicUTF8String & szName );
    virtual Tools_DynamicUTF8String askForValue      ( const Tools_DynamicUTF8String & szName );
    virtual bool                    askForContinue   ( const Tools_DynamicUTF8String & szName );

  private:
    DBMCli_Database         * m_pDatabase;
    Mode                      m_nMode;
    DBMCli_LogModeValue::Mode m_nLogMode;
    DBMCli_LogModeValue::Mode m_nNewLogMode;
    DBMCli_DBState_Constants  m_nState;
    SAPDB_Int                 m_nIndex;
    DBMCli_Devspaces        & m_oDevspaces;
    SAPDB_Int                 m_nCurrent;
    SAPDB_Bool                m_bRestart;
    SAPDB_Bool                m_bRefreshState;
}; 

/*! EndClass: DBMWeb_TemplateLogMode */

#endif // _DBMWEB_TEMPLATELOGMODE_HPP_

