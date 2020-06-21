/*! 
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateParam.hpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  parameter
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

#ifndef _DBMWEB_TEMPLATEPARAM_HPP_
#define _DBMWEB_TEMPLATEPARAM_HPP_

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
  Class:        DBMWeb_TemplateParam
  -----------------------------------------------------------------------------
  description:  An instance of this class represents a HTML-Page with  
                a database parameter.
  -----------------------------------------------------------------------------
*/
class DBMWeb_TemplateParam : public Tools_Template 
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMWeb_TemplateParam
      -------------------------------------------------------------------------
      description:  Constructor
      arguments:    wa [IN]  - webagent instance
      -------------------------------------------------------------------------
     */
     DBMWeb_TemplateParam ( sapdbwa_WebAgent             & wa,
                            DBMCli_Parameter             & oParam,
                            const DBMCli_ParameterStatus   nStatus,
                            const DBMCli_String          & sCompValue,
                            const DBMCli_String          & sBackURL);
 
    /*!
      -------------------------------------------------------------------------
      function:     ~DBMWeb_TemplateParam
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
     */
     ~DBMWeb_TemplateParam ( );

    /*!
      -------------------------------------------------------------------------
      function:     GetParameter
      -------------------------------------------------------------------------
      description:  Get the parameter object.
      return value: Reference to the parameter object.
      -------------------------------------------------------------------------
    */
    DBMCli_Parameter & GetParameter ( ) { return m_oParam; };


  private:
    virtual SAPDB_Int2              askForWriteCount ( const Tools_DynamicUTF8String & szName );
    virtual Tools_DynamicUTF8String askForValue      ( const Tools_DynamicUTF8String & szName );
                                              
  private:
     DBMCli_Parameter & m_oParam;
     DBMCli_ParameterStatus m_nStatus;
     DBMCli_String m_sCompValue;   
     DBMCli_String m_sBackURL;
}; 

/*! EndClass: DBMWeb_TemplateParam */

#endif // _DBMWEB_TEMPLATEPARAM_HPP_

