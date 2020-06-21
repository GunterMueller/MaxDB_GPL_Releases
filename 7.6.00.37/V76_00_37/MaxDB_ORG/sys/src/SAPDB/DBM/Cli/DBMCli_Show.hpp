/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_Show.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager Client Library
  description:  Show Class
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

#ifndef _DBMCLI_SHOW_HPP_
#define _DBMCLI_SHOW_HPP_

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
class DBMCli_Show;

/*
  -----------------------------------------------------------------------------
  typedefs
  -----------------------------------------------------------------------------
*/
typedef DBMCli_Array< DBMCli_Show, DBMCli_Show > DBMCli_ShowArray;

/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_Show
  -----------------------------------------------------------------------------
  description: Class to handle show commands to the dbm server.
  -----------------------------------------------------------------------------
*/
class DBMCli_Show 
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_Show
      -------------------------------------------------------------------------
      description:  Constructor
      prototypes:   2
      -------------------------------------------------------------------------
    */
    DBMCli_Show ( );
    DBMCli_Show ( const DBMCli_String & sName,
                  const DBMCli_String & sDesc );

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_Show
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_Show ( );

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
      description:  Open a show command.
      arguments:    sName    [IN]  - name of show command
                    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Open ( const DBMCli_String        & sName,
                            Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Close
      -------------------------------------------------------------------------
      description:  Close a show command.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Close ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     GetLine
      -------------------------------------------------------------------------
      description:  Get the next line from show result.
      arguments:    sLine    [OUT] - line from protocol file
                    oMsgList [OUT] - object for indication of errors
      return value: True if success and the end of result is not reached, 
                    otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool GetLine ( DBMCli_String        & sLine,
                         Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     GetPart
      -------------------------------------------------------------------------
      description:  Get the next part from show result.
      arguments:    sLine    [OUT] - line from protocol file
                    nMaxlen  [IN]  - max len of part
                    oMsgList [OUT] - object for indication of errors
      return value: True if success and the end of result is not reached, 
                    otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool GetPart ( DBMCli_String        & sPart,
                         SAPDB_Int              nMaxLen,
                         Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Name
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Name ( ) { return m_sName; };

    /*!
      -------------------------------------------------------------------------
      function:     Description
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Description ( ) { return m_sDescription; };

  private:
    SAPDB_Bool Next ( Msg_List & oMsgList );


  private:
    DBMCli_Database * m_pDatabase;

    DBMCli_String m_sName;
    DBMCli_String m_sDescription;

    SAPDB_Bool m_bContinue;
}; 

/*! EndClass: DBMCli_Show */

/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_Shows
  -----------------------------------------------------------------------------
  description: Class to handle show commands to the dbm server.
  -----------------------------------------------------------------------------
*/
class DBMCli_Shows 
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_Shows
      -------------------------------------------------------------------------
      description:  Constructor
      -------------------------------------------------------------------------
    */
    DBMCli_Shows ( );

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_Shows
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_Shows ( );

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
      function:     Refresh
      -------------------------------------------------------------------------
      description:  Refresh the array of shows.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Refresh ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     ShowArray 
      -------------------------------------------------------------------------
      description:  Get the array of available shows.
      return value: Reference to a array of available shows.
      -------------------------------------------------------------------------
    */
    DBMCli_ShowArray & ShowArray ( ) { return m_aShow; }; 

  private:
    DBMCli_Database * m_pDatabase;

    DBMCli_ShowArray m_aShow;

}; 

/*! EndClass: DBMCli_Shows */



#endif // _DBMCLI_SHOW_HPP_

