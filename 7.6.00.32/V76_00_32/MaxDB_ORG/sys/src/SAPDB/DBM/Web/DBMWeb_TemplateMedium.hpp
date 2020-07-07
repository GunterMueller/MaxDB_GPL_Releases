/*! 
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateMedium.hpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  Medium
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

#ifndef _DBMWEB_TEMPLATEMEDIUM_HPP_
#define _DBMWEB_TEMPLATEMEDIUM_HPP_

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
	DBMWEB_TEMPLMEDIUM_NEW  = 0,
	DBMWEB_TEMPLMEDIUM_EDIT = 1
} DBMWeb_TemplMedium_Mode;


/*!
  -----------------------------------------------------------------------------
  Class:        DBMWeb_TemplateMedium
  -----------------------------------------------------------------------------
  description:  An instance of this class represents a HTML-Page with  
                a database Medium.
  -----------------------------------------------------------------------------
*/

class DBMWeb_TemplateMedium : public Tools_Template
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMWeb_TemplateMedium
      -------------------------------------------------------------------------
      description:  Constructor
      arguments:    wa [IN]  - webagent instance
      -------------------------------------------------------------------------
    */

    DBMWeb_TemplateMedium ( sapdbwa_WebAgent & wa,
                            DBMCli_Medium & oMedium,
                            DBMWeb_TemplMedium_Mode nMode,
                            const DBMCli_String & sBackURL);
 
    /*!
      -------------------------------------------------------------------------
      function:     ~DBMWeb_TemplateMedium
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMWeb_TemplateMedium ( );

    /*!
      -------------------------------------------------------------------------
      function:     GetMedium
      -------------------------------------------------------------------------
      description:  Get the Medium object.
      return value: Reference to the Medium object.
      -------------------------------------------------------------------------
    */
    DBMCli_Medium & GetMedium ( ) { return m_oMedium; };


  private:
    virtual SAPDB_Int2              askForWriteCount ( const Tools_DynamicUTF8String & szName );
    virtual Tools_DynamicUTF8String askForValue      ( const Tools_DynamicUTF8String & szName );
    void xyz();
                                              
  private:
    DBMCli_Medium & m_oMedium;
    DBMWeb_TemplMedium_Mode m_nMode; 
    DBMCli_String m_sBackURL;
};

/*! EndClass: DBMWeb_TemplateMedium */

#endif // _DBMWEB_TEMPLATEMEDIUM_HPP_

