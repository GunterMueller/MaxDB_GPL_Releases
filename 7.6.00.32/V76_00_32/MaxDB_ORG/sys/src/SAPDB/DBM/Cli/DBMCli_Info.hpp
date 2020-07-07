/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_Info.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager Client Library
  description:  Info Class
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

#ifndef _DBMCLI_INFO_HPP_
#define _DBMCLI_INFO_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "DBM/Cli/DBMCli_Common.hpp"
#include "DBM/Cli/DBMCli_Stuff.hpp"
#include "DBM/Cli/DBMCli_ResultBuf.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define DBMCLI_INFO_CACHES    "CACHES"
#define DBMCLI_INFO_DATA      "DATA"
#define DBMCLI_INFO_IO        "IO"
#define DBMCLI_INFO_LOG       "LOG"
#define DBMCLI_INFO_LOCKS     "LOCKS"
#define DBMCLI_INFO_USERS     "USERS"
#define DBMCLI_INFO_VERSIONS  "VERSIONS"

#define DBMCLI_TITLE_CACHES    "Caches"
#define DBMCLI_TITLE_DATA      "Data Area"
#define DBMCLI_TITLE_IO        "IO"
#define DBMCLI_TITLE_LOG       "Log Area"
#define DBMCLI_TITLE_LOCKS     "Locks"
#define DBMCLI_TITLE_USERS     "Sessions"
#define DBMCLI_TITLE_VERSIONS  "Versions"

/*
  -----------------------------------------------------------------------------
  typedefs
  -----------------------------------------------------------------------------
*/
typedef enum  
{ 
	DBMCLI_INFOCOLTYPE_UNKNOWN = 0,
	DBMCLI_INFOCOLTYPE_STRING = 1,
	DBMCLI_INFOCOLTYPE_NUM = 2,
	DBMCLI_INFOCOLTYPE_DATE = 3,
	DBMCLI_INFOCOLTYPE_BOOL = 4
} DBMCli_InfoColumnType;

typedef struct {
  DBMCli_String         sName;
  DBMCli_InfoColumnType nType;
} DBMCli_InfoColumn;

typedef DBMCli_Array< DBMCli_InfoColumn, DBMCli_InfoColumn> DBMCli_InfoColumnArray;

/*
  -----------------------------------------------------------------------------
  froward declarations
  -----------------------------------------------------------------------------
*/
class DBMCli_Database;

/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_Info
  -----------------------------------------------------------------------------
  description: Class to handle info access to the dbm server.
  -----------------------------------------------------------------------------
*/
class DBMCli_Info 
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_Info
      -------------------------------------------------------------------------
      description:  Constructor
      -------------------------------------------------------------------------
    */
    DBMCli_Info ( );

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_Info
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_Info ( );

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
      function:     Open
      -------------------------------------------------------------------------
      description:  Open a info.
      arguments:    sName    [IN]  - name of info
                    sWhere   [IN]  - where clause for info select
                    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      prototypes:   2
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Open ( const DBMCli_String        & sName,
                            Msg_List & oMsgList );

    SAPDB_Bool Open ( const DBMCli_String        & sName,
                      const DBMCli_String        & sWhere,
                            Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Close
      -------------------------------------------------------------------------
      description:  Close a info.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Close ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     GetRow
      -------------------------------------------------------------------------
      description:  Get the next info row.
      arguments:    sRow     [OUT] - line from info result
                    oMsgList [OUT] - object for indication of errors
      return value: True if success and the end of info result is not reached, 
                    otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool GetRow ( DBMCli_String        & sRow,
                        Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     EndOfInfo
      -------------------------------------------------------------------------
      -------------------------------------------------------------------------
      description:  Check current pos in buffer.
      return value: If the end of buffer is reached return value is true
                    otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool EndOfInfo ( );

    /*!
      -------------------------------------------------------------------------
      function:     ColumnArray 
      -------------------------------------------------------------------------
      description:  Get a array of columns fromthe open info result.
      return value: Reference to a array of columns.
      -------------------------------------------------------------------------
    */
    DBMCli_InfoColumnArray & ColumnArray ( ) { return m_aInfoColumn; }; 


    /*!
      -------------------------------------------------------------------------
      function:     Name
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Name ( ) { return m_sName; };

    /*!
      -------------------------------------------------------------------------
      function:     Title
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Title ( ) { return m_sTitle; };

  private:
    void Clear ( );

    void GetColumns ( DBMCli_String & sDesc,
                      DBMCli_ResultBuf & sHeader );

    SAPDB_Bool Next ( Msg_List & oMsgList );


  private:
    DBMCli_Database * m_pDatabase;

    DBMCli_String m_sName;
    DBMCli_String m_sTitle;

    SAPDB_Bool m_bContinue;

    DBMCli_InfoColumnArray m_aInfoColumn;
}; 

/*! EndClass: DBMCli_Info */

#endif // _DBMCLI_INFO_HPP_

