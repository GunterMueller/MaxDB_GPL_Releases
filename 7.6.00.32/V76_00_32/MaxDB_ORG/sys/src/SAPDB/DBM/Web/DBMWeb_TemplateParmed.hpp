/*! 
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateParmed.hpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  Parmed
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

#ifndef _DBMWEB_TEMPLATEPARMED_HPP_
#define _DBMWEB_TEMPLATEPARMED_HPP_

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
	DBMWEB_TEMPLPARMED_NEW  = 0,
	DBMWEB_TEMPLPARMED_EDIT = 1
} DBMWeb_TemplParmed_Mode;


/*!
  -----------------------------------------------------------------------------
  Class:        DBMWeb_TemplateParmed
  -----------------------------------------------------------------------------
  description:  An instance of this class represents a HTML-Page with  
                a parallel medium.
  -----------------------------------------------------------------------------
*/

class DBMWeb_TemplateParmed : public Tools_Template
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMWeb_TemplateParmed
      -------------------------------------------------------------------------
      description:  Constructor
      arguments:    wa [IN]  - webagent instance
      -------------------------------------------------------------------------
    */

    DBMWeb_TemplateParmed ( sapdbwa_WebAgent & wa,
                            DBMCli_MediumParallel & oParMed,
                            DBMWeb_TemplParmed_Mode nMode,
                            const DBMCli_String & sBackURL);
 
    /*!
      -------------------------------------------------------------------------
      function:     ~DBMWeb_TemplateParmed
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMWeb_TemplateParmed ( );

    /*!
      -------------------------------------------------------------------------
      function:     GetMediumParallel
      -------------------------------------------------------------------------
      description:  Get the parallel medium object.
      return value: Reference to the parallel medium object.
      -------------------------------------------------------------------------
    */
    DBMCli_MediumParallel & GetMediumParallel ( ) { return m_oParMed; };


  private:
    virtual SAPDB_Int2              askForWriteCount ( const Tools_DynamicUTF8String & szName );
    virtual Tools_DynamicUTF8String askForValue      ( const Tools_DynamicUTF8String & szName );
                                              
  private:
    DBMCli_MediumParallel & m_oParMed;
    DBMWeb_TemplParmed_Mode m_nMode; 
    DBMCli_String m_sBackURL;
    SAPDB_Int m_nMedium;
};

/*! EndClass: DBMWeb_TemplateParmed */

#endif // _DBMWEB_TEMPLATEPARMED_HPP_

