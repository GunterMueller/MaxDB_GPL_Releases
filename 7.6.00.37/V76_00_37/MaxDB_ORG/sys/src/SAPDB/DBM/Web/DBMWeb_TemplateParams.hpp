/*! 
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateParams.hpp
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

#ifndef _DBMWEB_TEMPLATEPARAMS_HPP_
#define _DBMWEB_TEMPLATEPARAMS_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "ToolsCommon/Tools_Template.hpp"

#include "DBM/Cli/DBMCli_Database.hpp"
#include "DBM/Cli/DBMCli_Parameter.hpp"

/*!
  -----------------------------------------------------------------------------
  Class:        DBMWeb_TemplateParams
  -----------------------------------------------------------------------------
  description:  An instance of this class represents a HTML-Page with  
                database parameters.
  -----------------------------------------------------------------------------
*/
class DBMWeb_TemplateParams : public Tools_Template 
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMWeb_TemplateParams
      -------------------------------------------------------------------------
      description:  Constructor
      arguments:    wa [IN]  - webagent instance
      -------------------------------------------------------------------------
     */
     DBMWeb_TemplateParams ( sapdbwa_WebAgent & wa,
                             DBMCli_Parameters & oParams,
                             DBMCli_ParameterGroup nGroup);

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMWeb_TemplateParams
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
     */
     ~DBMWeb_TemplateParams ( );

    /*!
      -------------------------------------------------------------------------
      function:     GetParameters
      -------------------------------------------------------------------------
      description:  Get the parameters object.
      return value: Reference to the parameters object.
      -------------------------------------------------------------------------
    */
    DBMCli_Parameters & GetParameters ( ) { return m_oParams; };


  private:
    virtual SAPDB_Int2              askForWriteCount ( const Tools_DynamicUTF8String & szName );
    virtual Tools_DynamicUTF8String askForValue      ( const Tools_DynamicUTF8String & szName );
    virtual bool                    askForContinue   ( const Tools_DynamicUTF8String & szName );

    bool FindNextParam ( );
     
  private:
    DBMCli_String m_sGroup;
    DBMCli_Parameters & m_oParams;
    DBMCli_ParameterGroup m_nGroup;

    SAPDB_Int2 m_nParam;
}; 

/*! EndClass: DBMWeb_TemplateParams */

#endif // _DBMWEB_TEMPLATEPARAMS_HPP_

