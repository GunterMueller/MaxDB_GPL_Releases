/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_UpdStat.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager Client Library
  description:  Update Statistics Class
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

#ifndef _DBMCLI_UPDSTAT_HPP_
#define _DBMCLI_UPDSTAT_HPP_

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
typedef enum  
{ 
	DBMCLI_UPDSTATESTIMATE_NONE = 0,
	DBMCLI_UPDSTATESTIMATE_ROWS = 1,
	DBMCLI_UPDSTATESTIMATE_PERCENT = 2
} DBMCli_UpdStateEstimate_Type;

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/

/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_UpdStat
  -----------------------------------------------------------------------------
  description: Class to handle update statistics.
  -----------------------------------------------------------------------------
*/
class DBMCli_UpdStat 
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_UpdStat
      -------------------------------------------------------------------------
      description:  Constructor
      -------------------------------------------------------------------------
    */
    DBMCli_UpdStat ( );

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_UpdStat
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_UpdStat ( );

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
      function:     Column
      -------------------------------------------------------------------------
      description:  Update statistics for columns.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      prototypes:   2
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Column ( const DBMCli_String & sItem, 
                        DBMCli_UpdStateEstimate_Type nEstimate,
                        const DBMCli_String & sEstimate,
                        Msg_List & oMsgList );

    SAPDB_Bool Column ( const DBMCli_String & sOwner, 
                        const DBMCli_String & sTable,
                        const DBMCli_String & sColumn,
                        DBMCli_UpdStateEstimate_Type nEstimate,
                        const DBMCli_String & sEstimate,
                        Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Table
      -------------------------------------------------------------------------
      description:  Update statistics for tables.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      prototypes:   2
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Table ( const DBMCli_String & sItem, 
                       DBMCli_UpdStateEstimate_Type nEstimate,
                       const DBMCli_String & sEstimate,
                       Msg_List & oMsgList );

    SAPDB_Bool Table ( const DBMCli_String & sOwner, 
                       const DBMCli_String & sTable,
                       DBMCli_UpdStateEstimate_Type nEstimate,
                       const DBMCli_String & sEstimate,
                       Msg_List & oMsgList );

  private:
    void SplitColumnItem( const DBMCli_String & sItem, 
                          DBMCli_String & sOwner,
                          DBMCli_String & sTable, 
                          DBMCli_String & sColumn );

    void SplitTableItem( const DBMCli_String & sItem, 
                         DBMCli_String & sOwner,
                         DBMCli_String & sTable );

    const DBMCli_String EstimateSpec(const DBMCli_UpdStateEstimate_Type nEstimate,
                                     const DBMCli_String & sEstimate );


  private:
    DBMCli_Database * m_pDatabase;

}; 

/*! EndClass: DBMCli_UpdStat */

#endif // _DBMCLI_UPDSTAT_HPP_
