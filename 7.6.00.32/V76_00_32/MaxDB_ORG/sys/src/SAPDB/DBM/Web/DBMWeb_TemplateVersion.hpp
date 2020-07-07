/*! 
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateVersion.hpp
  -----------------------------------------------------------------------------
  responsible:  BerndV
  special area: Database Manager WebAgent Extension
  description:  Version
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

#ifndef _DBMWEB_TEMPLATEVERSION_HPP_
#define _DBMWEB_TEMPLATEVERSION_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "ToolsCommon/Tools_Template.hpp"

#include "DBM/Cli/DBMCli_Stuff.hpp"

/*!
  -----------------------------------------------------------------------------
  Class:        DBMWeb_TemplateVersion
  -----------------------------------------------------------------------------
  description:  An instance of this class represents a HTML-Page with  
                version information.
  -----------------------------------------------------------------------------
*/
class DBMWeb_TemplateVersion : public Tools_Template 
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMWeb_TemplateVersion
      -------------------------------------------------------------------------
      description:  Constructor
      arguments:    wa [IN]  - webagent instance
      -------------------------------------------------------------------------
     */
     DBMWeb_TemplateVersion ( sapdbwa_WebAgent    & wa,
                              const DBMCli_String & sWebdbm,
                              const DBMCli_String & sDbmsrv,
                              const DBMCli_String & sKernel );
 
    /*!
      -------------------------------------------------------------------------
      function:     ~DBMWeb_TemplateVersion
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
     */
     ~DBMWeb_TemplateVersion ( );


  private:
    virtual SAPDB_Int2              askForWriteCount ( const Tools_DynamicUTF8String & szName );
    virtual Tools_DynamicUTF8String askForValue      ( const Tools_DynamicUTF8String & szName );

    DBMCli_String  m_sWebdbm;
    DBMCli_String  m_sDbmsrv;
    DBMCli_String  m_sKernel;
    DBMCli_String  m_sCurrent;


}; 

/*! EndClass: DBMWeb_TemplateVersion */

#endif // _DBMWEB_TEMPLATEVERSION_HPP_

