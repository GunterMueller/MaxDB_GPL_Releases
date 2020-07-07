/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_EnumInstallation.hpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager Client Library
  description:  Enum Installation Class
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

#ifndef _DBMCLI_ENUMINSTALLATION_HPP_
#define _DBMCLI_ENUMINSTALLATION_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "DBM/Cli/DBMCli_Common.hpp"
#include "DBM/Cli/DBMCli_Stuff.hpp"
#include "DBM/Cli/DBMCli_Version.hpp"

/*
  -----------------------------------------------------------------------------
  froward declarations
  -----------------------------------------------------------------------------
*/
class DBMCli_EnumInstallation;

/*
  -----------------------------------------------------------------------------
  typedefs
  -----------------------------------------------------------------------------
*/
typedef DBMCli_Array< DBMCli_EnumInstallation, DBMCli_EnumInstallation > DBMCli_EnumInstallationArray;

/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_EnumInstallation
  -----------------------------------------------------------------------------
  description: Class for a enumerated instalations.
  -----------------------------------------------------------------------------
*/
class DBMCli_EnumInstallation
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_EnumInstallation
      -------------------------------------------------------------------------
      description:  Constructor
      arguments:    sVersion  [IN]  - version string
                    sInstRoot [IN]  - installation root
      prototypes:   2
      -------------------------------------------------------------------------
    */
    DBMCli_EnumInstallation ( );

    DBMCli_EnumInstallation (const DBMCli_String & sVersion,
                             const DBMCli_String & sInstRoot );

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_EnumInstallation
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_EnumInstallation ( );

    /*!  
      Chapter: Properties
    */

    /*!
      -------------------------------------------------------------------------
      function:     Version
      -------------------------------------------------------------------------
    */
    DBMCli_Version & Version ( ) { return m_oVersion; };

    /*!
      -------------------------------------------------------------------------
      function:     InstRoot
      -------------------------------------------------------------------------
    */
    const DBMCli_String & InstRoot ( ) { return m_sInstRoot; };

    /*!  
      EndChapter: Properties
    */


  private:
    DBMCli_Version m_oVersion;
    DBMCli_String  m_sInstRoot;
}; 

/*! EndClass: DBMCli_EnumInstallation */

#endif // _DBMCLI_ENUMINSTALLATION_HPP_

