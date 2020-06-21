/*! 
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateDevspace.hpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  devspace
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
#ifndef _DBMWEB_TEMPLATEDEVSPACE_HPP_
#define _DBMWEB_TEMPLATEDEVSPACE_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "ToolsCommon/Tools_Template.hpp"

#include "DBM/Cli/DBMCli_Database.hpp"
#include "DBM/Cli/DBMCli_Devspace.hpp"
/*
  -----------------------------------------------------------------------------
  typedefs
  -----------------------------------------------------------------------------
*/
typedef enum  
{ 
	DBMWEB_TEMPLDEVSPACE_NEW  = 0,
	DBMWEB_TEMPLDEVSPACE_EDIT = 1,
	DBMWEB_TEMPLDEVSPACE_INIT = 2
} DBMWeb_TemplDevspace_Mode;


/*!
  -----------------------------------------------------------------------------
  Class:        DBMWeb_TemplateDevspace
  -----------------------------------------------------------------------------
  description:  An instance of this class represents a HTML-Page with  
                a database Devspace.
  -----------------------------------------------------------------------------
*/

class DBMWeb_TemplateDevspace : public Tools_Template
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMWeb_TemplateDevspace
      -------------------------------------------------------------------------
      description:  Constructor
      arguments:    wa [IN]  - webagent instance
      -------------------------------------------------------------------------
    */

    DBMWeb_TemplateDevspace ( sapdbwa_WebAgent & wa,
                              DBMCli_Devspace & oDevspace,
                              DBMWeb_TemplDevspace_Mode nMode,
                              const DBMCli_String & sBackURL,
                              const DBMCli_String & sMessage = "");

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMWeb_TemplateDevspace
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMWeb_TemplateDevspace ( );

    /*!
      -------------------------------------------------------------------------
      function:     GetDevspace
      -------------------------------------------------------------------------
      description:  Get the devspace object.
      return value: Reference to the devspace object.
      -------------------------------------------------------------------------
    */
    DBMCli_Devspace & GetDevspace ( ) { return m_oDevspace; };


  private:
    virtual SAPDB_Int2              askForWriteCount ( const Tools_DynamicUTF8String & szName );
    virtual Tools_DynamicUTF8String askForValue      ( const Tools_DynamicUTF8String & szName );
    void xyz();
                                              
  private:
    DBMCli_Devspace & m_oDevspace;
    DBMWeb_TemplDevspace_Mode m_nMode; 
    DBMCli_String m_sBackURL;
    DBMCli_String m_sMessage;
};

/*! EndClass: DBMWeb_TemplateDevspace */

#endif // _DBMWEB_TEMPLATEDEVSPACE_HPP_

