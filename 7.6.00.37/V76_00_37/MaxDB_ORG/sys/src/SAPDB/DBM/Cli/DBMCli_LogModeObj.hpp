/*!
  -----------------------------------------------------------------------------
  module: DBMCli_LogModeObj.hpp
  -----------------------------------------------------------------------------
  responsible:  Bernd Vorsprach (Bernd.Vorsprach@sap.com)
  special area: Database Manager Client Library
  description:  LogModeObj Class

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

#ifndef _DBMCLI_LOGMODEOBJ_HPP_
#define _DBMCLI_LOGMODEOBJ_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "DBM/Cli/DBMCli_Stuff.hpp"
#include "DBM/Cli/DBMCli_Devspace.hpp"

/*
  -----------------------------------------------------------------------------
  forward declarations
  -----------------------------------------------------------------------------
*/
class DBMCli_Database;
class Msg_List;

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

/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_LogModeValue
  -----------------------------------------------------------------------------
  description: Class to handle the logmode value
  -----------------------------------------------------------------------------
*/
class DBMCli_LogModeValue
{
  public:
    /*!
      -------------------------------------------------------------------------
      Declaration:  Public types
      Description:  Public types used by public member functions.
      -------------------------------------------------------------------------
    */
    typedef enum {
      Demo,
      Single,
      SingleDemo,
      Dual,
      DualDemo,
      Unknown
    } Mode;

    /*! EndDeclaration:  Public types */

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_LogModeValue
      -------------------------------------------------------------------------
      description:  Constructor
      prototypes:   5
      -------------------------------------------------------------------------
    */
    DBMCli_LogModeValue (                                   );
    DBMCli_LogModeValue ( const DBMCli_LogModeValue & oMode );
    DBMCli_LogModeValue ( Mode                        nMode );
    DBMCli_LogModeValue ( const DBMCli_String       & sMode );
    DBMCli_LogModeValue ( DBMCli_LogMode              nMode );

    /*!
      -------------------------------------------------------------------------
      function:     SetValue
      -------------------------------------------------------------------------
      description:  Constructor
      prototypes:   4
      -------------------------------------------------------------------------
    */
    void SetValue ( const DBMCli_LogModeValue & oMode );
    void SetValue ( Mode                        nMode );
    void SetValue ( const DBMCli_String       & sMode );
    void SetValue ( DBMCli_LogMode              nMode );

    /*!
      -------------------------------------------------------------------------
      function:     GetValue
      -------------------------------------------------------------------------
      description:  returns the Log Mode
      return value: the Log Mode
      -------------------------------------------------------------------------
    */
    Mode           GetValue  ( ) const {return m_nMode;}

    /*!
      -------------------------------------------------------------------------
      function:     GetString
      -------------------------------------------------------------------------
      description:  returns the Log Mode Text
      return value: the Log Mode Text
      -------------------------------------------------------------------------
    */
    DBMCli_String  GetString ( ) const;

  private:
    Mode                      m_nMode;

};

/*! EndClass: DBMCli_LogModeValue */

/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_LogModeObj
  -----------------------------------------------------------------------------
  description: Class to handle the logmode of a database
  -----------------------------------------------------------------------------
*/
class DBMCli_LogModeObj
{
  public:
    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_LogModeObj
      -------------------------------------------------------------------------
      description:  Constructor
      -------------------------------------------------------------------------
    */
    DBMCli_LogModeObj ( );


    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_LogModeObj
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_LogModeObj ( );

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
      function:     SAPDB_Bool
      -------------------------------------------------------------------------
      description:  refreshes the object content
      return value: success of operation
      -------------------------------------------------------------------------
    */
    SAPDB_Bool  Refresh ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Value
      -------------------------------------------------------------------------
      description:  returns the Log Value
      return value: the Log Mode
      -------------------------------------------------------------------------
    */
    DBMCli_LogModeValue  Value (  ) const {return m_oValue;}


    /*!
      -------------------------------------------------------------------------
      function:     Change
      -------------------------------------------------------------------------
      description:  change the logmode
      parameters:   oNewMode [IN]  - the new logmode
                    oMsgList [OUT] - error information
      return value: succes of operation
      -------------------------------------------------------------------------
    */
    SAPDB_Bool  Change ( DBMCli_LogModeValue::Mode   nMode,
                         SAPDB_Bool                  bRestart,
                         Msg_List      & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     ReadyForChange
      -------------------------------------------------------------------------
      description:  all devspaces set?
      parameters:   oNewMode [IN]  - the new logmode
      return value: succes of operation
      -------------------------------------------------------------------------
    */
    SAPDB_Bool  ReadyForChange ( DBMCli_LogModeValue::Mode nMode );

  private:

    SAPDB_Bool ChangeToDual   ( SAPDB_Bool                  bRestart,
                                Msg_List      & oMsgList );

    SAPDB_Bool ChangeToSingle ( SAPDB_Bool                  bRestart,
                                Msg_List      & oMsgList );

    SAPDB_Bool ChangeToDemo   ( Msg_List      & oMsgList );

    DBMCli_Database         * m_pDatabase;
    DBMCli_LogModeValue       m_oValue;

};

/*! EndClass: DBMCli_LogModeObj */

#endif // _DBMCLI_LOGMODEOBJ_HPP_
