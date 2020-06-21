/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_Version.hpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Version Manager Client Library
  description:  Version Class
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

#ifndef _DBMCLI_VERSION_HPP_
#define _DBMCLI_VERSION_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "DBM/Cli/DBMCli_Stuff.hpp"

/*
  -----------------------------------------------------------------------------
  typedefs
  -----------------------------------------------------------------------------
*/
/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_Version
  -----------------------------------------------------------------------------
  description: Version class. 
  -----------------------------------------------------------------------------
*/
class DBMCli_Version
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_Version
      -------------------------------------------------------------------------
      description:  Constructor
      prototypes:   2
      -------------------------------------------------------------------------
    */
    DBMCli_Version ( );
    DBMCli_Version ( const DBMCli_String & sVersion );

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_Version
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_Version ( );

    /*!
      -------------------------------------------------------------------------
      function:     Clear
      -------------------------------------------------------------------------
      description:  
      -------------------------------------------------------------------------
    */
    void Clear ( );

    /*!
      -------------------------------------------------------------------------
      function:     GetName
      -------------------------------------------------------------------------
      description:  Get the version string.
      -------------------------------------------------------------------------
    */
    DBMCli_String & GetName ( );

    /*!
      -------------------------------------------------------------------------
      function:     SetName
      -------------------------------------------------------------------------
      description:  Set the version.
      -------------------------------------------------------------------------
    */
    void SetName ( const DBMCli_String & sVersion );

    /*!
      -------------------------------------------------------------------------
      function:     SetBuild
      -------------------------------------------------------------------------
      description:  Set the buld number
      -------------------------------------------------------------------------
    */
    void SetBuild ( const DBMCli_String & sBuild );

    /*!
      -------------------------------------------------------------------------
      function:     GetMajor
      -------------------------------------------------------------------------
      description:  Get the major version number.
      -------------------------------------------------------------------------
    */
    SAPDB_Int GetMajor ( ) { return m_nMajor; };

    /*!
      -------------------------------------------------------------------------
      function:     GetMinor
      -------------------------------------------------------------------------
      description:  Get the minor version number.
      -------------------------------------------------------------------------
    */
    SAPDB_Int GetMinor ( ) { return m_nMinor; };

    /*!
      -------------------------------------------------------------------------
      function:     GetRevision
      -------------------------------------------------------------------------
      description:  Get the revision version number.
      -------------------------------------------------------------------------
    */
    SAPDB_Int GetRevision ( ) { return m_nRevision; };

    /*!
      -------------------------------------------------------------------------
      function:     GetPatch
      -------------------------------------------------------------------------
      description:  Get the patch version number.
      -------------------------------------------------------------------------
    */
    SAPDB_Int GetPatch ( ) { return m_nPatch; };

    /*!
      -------------------------------------------------------------------------
      function:     GetNumber
      -------------------------------------------------------------------------
      description:  Get the version as a number.
      -------------------------------------------------------------------------
    */
    SAPDB_Int4 GetNumber ( ) { return m_nNumber; };


  private:
    void AnalyzeVersion ( const DBMCli_String & sVersion, bool bBuild );

  private: 

    DBMCli_String m_sVersion;

    SAPDB_Int     m_nMajor;
    SAPDB_Int     m_nMinor;
    SAPDB_Int     m_nRevision;
    SAPDB_Int     m_nPatch;

    SAPDB_Int4    m_nNumber;
}; 

/*! EndClass: DBMCli_Version */

#endif // _DBMCLI_VERSION_HPP_

