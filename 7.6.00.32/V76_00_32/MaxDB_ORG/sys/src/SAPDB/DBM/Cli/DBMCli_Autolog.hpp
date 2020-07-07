/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_Autolog.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager Client Library
  description:  Autolog
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

#ifndef _DBMCLI_AUTOLOG_HPP_
#define _DBMCLI_AUTOLOG_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "DBM/Cli/DBMCli_Common.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  typedefs
  -----------------------------------------------------------------------------
*/
typedef enum
{
  DBMCLI_AUTOLOGSTATE_UNKNOWN = 0,
  DBMCLI_AUTOLOGSTATE_ON      = 1,
  DBMCLI_AUTOLOGSTATE_OFF     = 2
} DBMCli_AutologState;

/*
  -----------------------------------------------------------------------------
  froward declarations
  -----------------------------------------------------------------------------
*/
class DBMCli_Database;

/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_Autolog
  -----------------------------------------------------------------------------
  description: Class to handle Autolog.
  -----------------------------------------------------------------------------
*/
class DBMCli_Autolog 
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_Autolog
      -------------------------------------------------------------------------
      description:  Constructor
      -------------------------------------------------------------------------
    */
    DBMCli_Autolog ( );

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_Autolog
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_Autolog ( );

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
      function:     Clear
      -------------------------------------------------------------------------
      description:  Clear the autolog object members.
      -------------------------------------------------------------------------
    */
    void Clear ( );

    /*!
      -------------------------------------------------------------------------
      function:     Refresh
      -------------------------------------------------------------------------
      description:  Refresh the autolog state.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Refresh ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     On
      -------------------------------------------------------------------------
      description:  Turn autosave log on.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool On ( const DBMCli_String & sMedium,
                    Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Off
      -------------------------------------------------------------------------
      description:  Turn autosave log off.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Off ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Cancel
      -------------------------------------------------------------------------
      description:  Cancel a running autosave log action.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Cancel ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     State
      -------------------------------------------------------------------------
    */
    DBMCli_AutologState State ( ) { return m_nState; };

    /*!
      -------------------------------------------------------------------------
      function:     StateStr
      -------------------------------------------------------------------------
    */
    const DBMCli_String & StateStr ( ) { return m_sState; };

    /*!
      -------------------------------------------------------------------------
      function:     SetState
      -------------------------------------------------------------------------
    */
    void  SetState ( DBMCli_AutologState nState );

  private:
    DBMCli_Database * m_pDatabase;

    DBMCli_AutologState m_nState;
    DBMCli_String       m_sState;
}; 

/*! EndClass: DBMCli_Autolog */

#endif // _DBMCLI_AUTOLOG_HPP_

