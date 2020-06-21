/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_EnumDatabase.hpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager Client Library
  description:  Enum Database Class
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

#ifndef _DBMCLI_ENUMDATABASE_HPP_
#define _DBMCLI_ENUMDATABASE_HPP_

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
class DBMCli_EnumDatabase;

/*
  -----------------------------------------------------------------------------
  typedefs
  -----------------------------------------------------------------------------
*/
typedef DBMCli_Array< DBMCli_EnumDatabase, DBMCli_EnumDatabase > DBMCli_EnumDatabaseArray;

/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_EnumDatabase
  -----------------------------------------------------------------------------
  description: Class for a enumerated database.
  -----------------------------------------------------------------------------
*/
class DBMCli_EnumDatabase
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_EnumDatabase
      -------------------------------------------------------------------------
      description:  Constructor
      arguments:    sName     [IN]  - name of database
                    sInstRoot [IN]  - installation root
                    sVersion  [IN]  - version string
                    sKernel   [IN]  - kernel (fast,quick,slow)
                    sState    [IN]  - state (offline,running)
      prototypes:   2
      -------------------------------------------------------------------------
    */
    DBMCli_EnumDatabase ( );

    DBMCli_EnumDatabase ( const DBMCli_String & sName,
                          const DBMCli_String & sInstRoot,
                          const DBMCli_String & sVersion,
                          const DBMCli_String & sKernel,
                          const DBMCli_String & sState);

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_EnumDatabase
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_EnumDatabase ( );

    /*!  
      Chapter: Properties
    */

    /*!
      -------------------------------------------------------------------------
      function:     Name
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Name ( ) { return m_sName; };

    /*!
      -------------------------------------------------------------------------
      function:     InstRoot
      -------------------------------------------------------------------------
    */
    const DBMCli_String & InstRoot ( ) { return m_sInstRoot; };

    /*!
      -------------------------------------------------------------------------
      function:     Version
      -------------------------------------------------------------------------
    */
    DBMCli_Version & Version ( ) { return m_oVersion; };

    /*!
      -------------------------------------------------------------------------
      function:     Kernel
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Kernel ( ) { return m_sKernel; };

    /*!
      -------------------------------------------------------------------------
      function:     State
      -------------------------------------------------------------------------
    */
    const DBMCli_String & State ( ) { return m_sState; };

    /*!  
      EndChapter: Properties
    */


  private:
    DBMCli_String m_sName;
    DBMCli_String m_sInstRoot;
    DBMCli_Version m_oVersion;
    DBMCli_String m_sKernel;
    DBMCli_String m_sState;
}; 

/*! EndClass: DBMCli_EnumDatabase */

#endif // _DBMCLI_ENUMDATABASE_HPP_

