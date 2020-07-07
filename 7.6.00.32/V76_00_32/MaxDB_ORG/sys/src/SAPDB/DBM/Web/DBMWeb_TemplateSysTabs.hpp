/*! 
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateSysTabs.hpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  upgrade system tables
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

#ifndef _DBMWEB_TEMPLATESYSTABS_HPP_
#define _DBMWEB_TEMPLATESYSTABS_HPP_

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
	DBMWEB_TEMPLSYSTABS_MODE_START  = 0,
	DBMWEB_TEMPLSYSTABS_MODE_WAIT   = 1,
	DBMWEB_TEMPLSYSTABS_MODE_RESULT = 2
} DBMWeb_TemplSysTabs_Mode;

/*!
  -----------------------------------------------------------------------------
  Class:        DBMWeb_TemplateSysTabs
  -----------------------------------------------------------------------------
  description:  An instance of this class represents a HTML-Page with  
                the upgrade system tables function.
  -----------------------------------------------------------------------------
*/
class DBMWeb_TemplateSysTabs : public Tools_Template 
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMWeb_TemplateSysTabs
      -------------------------------------------------------------------------
      description:  Constructor
      arguments:    wa [IN]  - webagent instance
      -------------------------------------------------------------------------
    */
    DBMWeb_TemplateSysTabs ( sapdbwa_WebAgent & wa,
                             const DBMCli_String & sDBAName,
                             const DBMCli_String & sDBAPwd,
                             const DBMCli_String & sDOMPwd,
                             DBMWeb_TemplSysTabs_Mode nMode );
 
    /*!
      -------------------------------------------------------------------------
      function:     ~DBMWeb_TemplateSysTabs
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMWeb_TemplateSysTabs ( );


  private:
    virtual SAPDB_Int2              askForWriteCount ( const Tools_DynamicUTF8String & szName );
    virtual Tools_DynamicUTF8String askForValue      ( const Tools_DynamicUTF8String & szName );
                                              
  private:
    DBMCli_String m_sDBAName;
    DBMCli_String m_sDBAPwd;
    DBMCli_String m_sDOMPwd;
    
    DBMWeb_TemplSysTabs_Mode m_nMode;
}; 

/*! EndClass: DBMWeb_TemplateSysTabs */

#endif // _DBMWEB_TEMPLATESYSTABS_HPP_

