/*! 
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateCommand.hpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  check command 
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

#ifndef _DBMWEB_TEMPLATECOMMAND_HPP_
#define _DBMWEB_TEMPLATECOMMAND_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "ToolsCommon/Tools_Template.hpp"

#include "DBM/Cli/DBMCli_Database.hpp"

/*
  -----------------------------------------------------------------------------
  typedefs
  -----------------------------------------------------------------------------
*/
typedef enum  
{ 
	DBMWEB_TEMPLCOMMAND_MODE_COMMAND = 0,
	DBMWEB_TEMPLCOMMAND_MODE_RESULT = 1
} DBMWeb_TemplCommand_Mode;

/*!
  -----------------------------------------------------------------------------
  Class:        DBMWeb_TemplateCommand
  -----------------------------------------------------------------------------
  description:  An instance of this class represents a HTML-Page with  
                the check command function.
  -----------------------------------------------------------------------------
*/
class DBMWeb_TemplateCommand : public Tools_Template 
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMWeb_TemplateCommand
      -------------------------------------------------------------------------
      description:  Constructor
      arguments:    wa [IN]  - webagent instance
      -------------------------------------------------------------------------
     */
     DBMWeb_TemplateCommand ( sapdbwa_WebAgent & wa, 
                              DBMWeb_TemplCommand_Mode nMode,
                              const DBMCli_String & sCommand,
                              const DBMCli_String & sResult );
 
    /*!
      -------------------------------------------------------------------------
      function:     ~DBMWeb_TemplateCommand
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
     */
     ~DBMWeb_TemplateCommand ( );

  private:
    virtual SAPDB_Int2              askForWriteCount ( const Tools_DynamicUTF8String & szName );
    virtual Tools_DynamicUTF8String askForValue      ( const Tools_DynamicUTF8String & szName );
                                              
  private:
    DBMWeb_TemplCommand_Mode m_nMode;
    DBMCli_String m_sCommand;
    DBMCli_String m_sResult;
}; 

/*! EndClass: DBMWeb_TemplateCommand */

#endif // _DBMWEB_TEMPLATECOMMAND_HPP_

