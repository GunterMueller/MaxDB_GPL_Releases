/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_Indexes.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager Client Library
  description:  Indexes Class
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

#ifndef _DBMCLI_INDEXES_HPP_
#define _DBMCLI_INDEXES_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "DBM/Cli/DBMCli_Common.hpp"

/*
  -----------------------------------------------------------------------------
  froward declarations
  -----------------------------------------------------------------------------
*/
class DBMCli_Database;

/*
  -----------------------------------------------------------------------------
  typedefs
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/

/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_Indexes
  -----------------------------------------------------------------------------
  description: Class to handle indexes.
  -----------------------------------------------------------------------------
*/
class DBMCli_Indexes 
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_Indexes
      -------------------------------------------------------------------------
      description:  Constructor
      -------------------------------------------------------------------------
    */
    DBMCli_Indexes ( );

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_Indexes
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_Indexes ( );

    /*!
      -------------------------------------------------------------------------
      function:     SetDatabase
      -------------------------------------------------------------------------
      description:  Set the database member.
      -------------------------------------------------------------------------
    */
    void SetDatabase ( DBMCli_Database * p ) { m_pDatabase = p; };

    /*!
      -------------------------------------------------------------------------
      function:     GetDatabase
      -------------------------------------------------------------------------
      description:  Get the database member.
      return value: Reference to the database object.
      -------------------------------------------------------------------------
    */
    DBMCli_Database & GetDatabase ( ) { return *m_pDatabase; };

    /*!
      -------------------------------------------------------------------------
      function:     Enable
      -------------------------------------------------------------------------
      description:  Enable a index.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      prototypes:   2
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Enable ( const DBMCli_String & sItem, 
                        Msg_List & oMsgList );

    SAPDB_Bool Enable ( const DBMCli_String & sOwner, 
                        const DBMCli_String & sTable,
                        const DBMCli_String & sIndex,
                        Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Disable
      -------------------------------------------------------------------------
      description:  Disable a index.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      prototypes:   2
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Disable ( const DBMCli_String & sItem, 
                         Msg_List & oMsgList );

    SAPDB_Bool Disable ( const DBMCli_String & sOwner, 
                         const DBMCli_String & sTable,
                         const DBMCli_String & sIndex,
                         Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Delete
      -------------------------------------------------------------------------
      description:  Delete a index.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      prototypes:   2
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Delete ( const DBMCli_String & sItem, 
                        Msg_List & oMsgList );

    SAPDB_Bool Delete ( const DBMCli_String & sOwner, 
                        const DBMCli_String & sTable,
                        const DBMCli_String & sIndex,
                        Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Recreate
      -------------------------------------------------------------------------
      description:  Recreate a index.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      prototypes:   2
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Recreate ( const DBMCli_String & sItem, 
                          Msg_List & oMsgList );

    SAPDB_Bool Recreate ( const DBMCli_String & sOwner, 
                          const DBMCli_String & sTable,
                          const DBMCli_String & sIndex,
                          Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     RecreateAll
      -------------------------------------------------------------------------
      description:  Recreate all bad indexes.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool RecreateAll ( Msg_List & oMsgList );


  private:
    void SplitIndexItem( const DBMCli_String & sItem, 
                         DBMCli_String & sOwner,
                         DBMCli_String & sTable, 
                         DBMCli_String & sIndex );

    const DBMCli_String IndexOnSpec(const DBMCli_String & sOwner,
                                    const DBMCli_String & sTable, 
                                    const DBMCli_String & sIndex );

  private:
    DBMCli_Database * m_pDatabase;

}; 

/*! EndClass: DBMCli_Indexes */

#endif // _DBMCLI_INDEXES_HPP_
