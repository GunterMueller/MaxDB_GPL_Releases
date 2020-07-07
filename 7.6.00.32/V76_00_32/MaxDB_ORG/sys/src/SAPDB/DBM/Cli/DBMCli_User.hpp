/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_User.hpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager Client Library
  description:  User Class
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

#ifndef _DBMCLI_USER_HPP_
#define _DBMCLI_USER_HPP_

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
class DBMCli_User;
class DBMCli_UserRight;

/*
  -----------------------------------------------------------------------------
  typedefs
  -----------------------------------------------------------------------------
*/
typedef DBMCli_Array< DBMCli_User, DBMCli_User > DBMCli_UserArray;
typedef DBMCli_Array< DBMCli_UserRight, DBMCli_UserRight > DBMCli_UserRightArray;

/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_UserRight
  -----------------------------------------------------------------------------
  description: handle a userright.
  -----------------------------------------------------------------------------
*/
class DBMCli_UserRight
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_UserRight
      -------------------------------------------------------------------------
      description:  Constructor
      prototypes:   2
      -------------------------------------------------------------------------
    */
    DBMCli_UserRight ( );

    DBMCli_UserRight ( const DBMCli_String & sName,
                       const SAPDB_Bool      bValue,
                       const DBMCli_String & sDescription );

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_UserRight
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_UserRight ( );

    /*!  
      Chapter: Properties
    */

    /*!
      -------------------------------------------------------------------------
      function:     Name
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Name ( ) const { return m_sName; };

    /*!
      -------------------------------------------------------------------------
      function:     Value
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Value ( ) const { return m_bValue; };

    /*!
      -------------------------------------------------------------------------
      function:     Right
      -------------------------------------------------------------------------
    */
    const DBMCli_String Right( );

    /*!
      -------------------------------------------------------------------------
      function:     Description
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Description ( ) const { return m_sDescription; };

    /*!  
      EndChapter: Properties
    */

    /*!
      -------------------------------------------------------------------------
      function:     SetValue
      -------------------------------------------------------------------------
    */
    void SetValue ( SAPDB_Bool bValue ) { m_bValue = bValue; };

  private:
    DBMCli_String m_sName;
    SAPDB_Bool    m_bValue;
    DBMCli_String m_sDescription;
}; 
/*! EndClass: DBMCli_UserRight */


/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_User
  -----------------------------------------------------------------------------
  description: Handle a single User.
  -----------------------------------------------------------------------------
*/
class DBMCli_User
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_User
      -------------------------------------------------------------------------
      description:  Constructor
      arguments:    
      prototypes:   3
      -------------------------------------------------------------------------
    */
    DBMCli_User ( );

    DBMCli_User ( const DBMCli_String & sName );

    DBMCli_User ( const DBMCli_String & sName,
                  const DBMCli_String & sServerRights,
                  const DBMCli_String & sGuiRights,
                  const DBMCli_String & sSecondPwd,
                  const DBMCli_String & sDBMUser,
                  const DBMCli_String & sSQLUser,
                  const DBMCli_String & sSQLUserMode,
                  const DBMCli_String & sDisabled,
                  const DBMCli_String & sSystemName,
                  const DBMCli_String & sComment);

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_User
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_User ( );

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
    DBMCli_Database & GetDatabase ( ) const { return *m_pDatabase; };

    /*!
      -------------------------------------------------------------------------
      function:     Clear
      -------------------------------------------------------------------------
      description:  Clear User informations.
      -------------------------------------------------------------------------
    */
    void Clear ( );

    /*!
      -------------------------------------------------------------------------
      function:     Refresh
      -------------------------------------------------------------------------
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Refresh ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Put
      -------------------------------------------------------------------------
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Put ( const DBMCli_String & sDisabled,
                     const DBMCli_String & sComment,
                     const DBMCli_String & sSecondPwd,
                     const SAPDB_Bool      bSeconPwd,
                     Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     PutPassword
      -------------------------------------------------------------------------
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool PutPassword ( const DBMCli_String & sPassword,
                             Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     PutSecondPwd
      -------------------------------------------------------------------------
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool PutSecondPwd ( const DBMCli_String & sSecondPwd,
                              Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Delete
      -------------------------------------------------------------------------
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Delete ( Msg_List & oMsgList );

    /*!  
      Chapter: Properties
    */

    /*!
      -------------------------------------------------------------------------
      function:     Name
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Name ( ) const { return m_sName; };

    /*!
      -------------------------------------------------------------------------
      function:     ServerRights
      -------------------------------------------------------------------------
    */
    const DBMCli_String & ServerRights ( ) const { return m_sServerRights; };

    /*!
      -------------------------------------------------------------------------
      function:     GuiRights
      -------------------------------------------------------------------------
    */
    const DBMCli_String & GuiRights ( ) const { return m_sGuiRights; };

    /*!
      -------------------------------------------------------------------------
      function:     SecondPwd
      -------------------------------------------------------------------------
    */
    const DBMCli_String & SecondPwd ( ) const { return m_sSecondPwd; };

    /*!
      -------------------------------------------------------------------------
      function:     DBMUser
      -------------------------------------------------------------------------
    */
    const DBMCli_String & DBMUser ( ) const { return m_sDBMUser; };

    /*!
      -------------------------------------------------------------------------
      function:     SQLUser
      -------------------------------------------------------------------------
    */
    const DBMCli_String & SQLUser ( ) const { return m_sSQLUser; };

    /*!
      -------------------------------------------------------------------------
      function:     SQLUserMode
      -------------------------------------------------------------------------
    */
    const DBMCli_String & SQLUserMode ( ) const { return m_sSQLUserMode; };

    /*!
      -------------------------------------------------------------------------
      function:     Disabled
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Disabled ( ) const { return m_sDisabled; };

    /*!
      -------------------------------------------------------------------------
      function:     SystemName
      -------------------------------------------------------------------------
    */
    const DBMCli_String & SystemName ( ) const { return m_sSystemName; };

    /*!
      -------------------------------------------------------------------------
      function:     Comment
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Comment ( ) const { return m_sComment; };

    /*!  
      EndChapter: Properties
    */

    /*!
      -------------------------------------------------------------------------
      function:     ServerRightArray
      -------------------------------------------------------------------------
      description:  Get the array of ServerRights.
      return value: Reference to an array of ServerRight objects.
      -------------------------------------------------------------------------
    */
    DBMCli_UserRightArray & ServerRightArray ( ) { return m_aServerRights; }; 

    /*!
      -------------------------------------------------------------------------
      function:     ServerRightString
      -------------------------------------------------------------------------
      description:  Get ServerRights string.
      return value: String of ServerRights.
      -------------------------------------------------------------------------
    */
    const DBMCli_String ServerRightString ( ); 

    /*!
      -------------------------------------------------------------------------
      function:     SetName
      -------------------------------------------------------------------------
    */
    void SetName ( const DBMCli_String & sName ) { m_sName = sName; };


  private:
    void AssignProp ( const DBMCli_String & sProp,
                      const DBMCli_String & sVal );

  private:
    DBMCli_Database * m_pDatabase;

    DBMCli_String m_sName;

    DBMCli_String m_sServerRights;
    DBMCli_UserRightArray m_aServerRights;

    DBMCli_String m_sGuiRights;
    DBMCli_String m_sSecondPwd;
    DBMCli_String m_sDBMUser;
    DBMCli_String m_sSQLUser;
    DBMCli_String m_sSQLUserMode;
    DBMCli_String m_sDisabled;
    DBMCli_String m_sSystemName;
    DBMCli_String m_sComment;
}; 

/*! EndClass: DBMCli_User */

/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_Users
  -----------------------------------------------------------------------------
  description: Handle Users.
  -----------------------------------------------------------------------------
*/
class DBMCli_Users
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_Users
      -------------------------------------------------------------------------
      description:  Constructor
      -------------------------------------------------------------------------
    */
    DBMCli_Users ( );

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_Users
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_Users ( );

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
      description:  Clear the Users object members.
      -------------------------------------------------------------------------
    */
    void Clear ( );

    /*!
      -------------------------------------------------------------------------
      function:     Refresh
      -------------------------------------------------------------------------
      description:  Refresh the Users.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Refresh ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     NewUser
      -------------------------------------------------------------------------
      description:  A new user.
      return value: Reference to a User object.
      -------------------------------------------------------------------------
    */
    DBMCli_User & NewUser( const DBMCli_String & sName );

    /*!
      -------------------------------------------------------------------------
      function:     AddUser
      -------------------------------------------------------------------------
      description:  Add a user.
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool AddUser( const DBMCli_String &  sName,
                        const DBMCli_String &  sPassword,
                        Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     DeleteUser
      -------------------------------------------------------------------------
      description:  Delete a user.
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool DeleteUser( const DBMCli_String &  sName,
                           Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     IndexByName
      -------------------------------------------------------------------------
      description:  Get a index from a User by the name.
      return value: Index of User in array.
      -------------------------------------------------------------------------
    */
    SAPDB_Int IndexByName ( const DBMCli_String & sName );

    /*!
      -------------------------------------------------------------------------
      function:     UserArray
      -------------------------------------------------------------------------
      description:  Get the array of Users.
      return value: Reference to an array of User objects.
      -------------------------------------------------------------------------
    */
    DBMCli_UserArray & UserArray ( ) { return m_aUser; }; 


  private:
    DBMCli_Database * m_pDatabase;

    DBMCli_User m_oNewUser;

    DBMCli_UserArray m_aUser;
}; 

/*! EndClass: DBMCli_Users */

#endif // _DBMCLI_USERS_HPP_

