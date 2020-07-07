/*! 
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateFiles.hpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  creating list of files
  version:      7.3 and higher 
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

#ifndef _DBMWEB_TEMPLATEFILES_HPP_
#define _DBMWEB_TEMPLATEFILES_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "ToolsCommon/Tools_Template.hpp"

#include "DBM/Cli/DBMCli_Database.hpp"
#include "DBM/Cli/DBMCli_File.hpp"

/*!
  -----------------------------------------------------------------------------
  Class:        DBMWeb_TemplateFiles
  -----------------------------------------------------------------------------
  description:  An instance of this class represents a HTML-Page with a 
                list of files.
  -----------------------------------------------------------------------------
*/
class DBMWeb_TemplateFiles : public Tools_Template 
{
  public:

    /*!
      -------------------------------------------------------------------------
      Declaration:     Mode
      -------------------------------------------------------------------------
      description:     the mode values
      -------------------------------------------------------------------------
     */
     typedef enum  
      { 
	      MODE_FILES      = 0,
	      MODE_DIAG       = 1,
	      MODE_DIAGLIST   = 3,
	      MODE_PACK       = 4
      } Mode;
    /*!
      -------------------------------------------------------------------------
      EndDeclaration
      -------------------------------------------------------------------------
     */


    /*!
      -------------------------------------------------------------------------
      function:     DBMWeb_TemplateFiles
      -------------------------------------------------------------------------
      description:  Constructor
      arguments:    wa [IN]  - webagent instance
      -------------------------------------------------------------------------
     */
     DBMWeb_TemplateFiles ( sapdbwa_WebAgent   & wa,
                            Mode                 nMode,
                            DBMCli_Files       & oFiles,
                            DBMCli_Diagnosises & oDiagnosises,
                            SAPDB_Int2           nIndex = 0 );
 
    /*!
      -------------------------------------------------------------------------
      function:     GetFiles
      -------------------------------------------------------------------------
      description:  Get the files object.
      return value: Reference to the ifiles object.
      -------------------------------------------------------------------------
    */
    DBMCli_Files & GetFiles ( ) { return m_oFiles; };


  private:
    virtual SAPDB_Int2              askForWriteCount ( const Tools_DynamicUTF8String & szName );
    virtual Tools_DynamicUTF8String askForValue      ( const Tools_DynamicUTF8String & szName );
                                              
  private:
     DBMCli_Files       & m_oFiles;
     DBMCli_Diagnosises & m_oDiagnosises;

     SAPDB_Int2 m_nIndex;

     SAPDB_Int2 m_nRow;
     SAPDB_Int2 m_nCol;

     Mode       m_nMode;
}; 

/*! EndClass: DBMWeb_TemplateFiles */

#endif // _DBMWEB_TEMPLATEFILES_HPP_

