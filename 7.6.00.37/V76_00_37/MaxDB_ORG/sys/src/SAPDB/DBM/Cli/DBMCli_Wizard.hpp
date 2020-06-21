/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_Wizard.hpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Wizard Manager Client Library
  description:  Wizard Class
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

#ifndef _DBMCLI_WIZARD_HPP_
#define _DBMCLI_WIZARD_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "DBM/Cli/DBMCli_Common.hpp"
#include "DBM/Cli/DBMCli_Stuff.hpp"
#include "DBM/Cli/DBMCli_Node.hpp"
#include "DBM/Cli/DBMCli_Version.hpp"
#include "DBM/Cli/DBMCli_Database.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define DBMCLI_WIZARD_INITMODE_DEF        "DEFAULT"
#define DBMCLI_WIZARD_INITMODE_CUR        "CURRENT"
#define DBMCLI_WIZARD_INITMODE_CPY        "COPY"
#define DBMCLI_WIZARD_INITMODE_RES        "RESTORE"

#define DBMCLI_WIZARD_INSTANCETYPE_OLTP   "OLTP"
#define DBMCLI_WIZARD_INSTANCETYPE_LC     "LC"
#define DBMCLI_WIZARD_INSTANCETYPE_CS     "CS"
#define DBMCLI_WIZARD_INSTANCETYPE_BW     "BW"

#define DBMCLI_WIZARD_INSTALLMODE_INSTALL "INSTALL"
#define DBMCLI_WIZARD_INSTALLMODE_RESTORE "RESTORE"

/*
  -----------------------------------------------------------------------------
  typedefs
  -----------------------------------------------------------------------------
*/
/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_Wizard
  -----------------------------------------------------------------------------
  description: wizard class. 
  -----------------------------------------------------------------------------
*/
class DBMCli_Wizard
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_Wizard
      -------------------------------------------------------------------------
      description:  Constructor
      -------------------------------------------------------------------------
    */
    DBMCli_Wizard ( );

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_Wizard
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_Wizard ( );

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
      function:     CreateDatabase
      -------------------------------------------------------------------------
      description:  Create a databases instance.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool CreateDatabase ( DBMCli_Database ** ppDatabase, 
                                Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     UseDatabase
      -------------------------------------------------------------------------
      description:  Use a databases instance.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool UseDatabase ( DBMCli_Database ** ppDatabase, 
                             Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     InstallDatabase
      -------------------------------------------------------------------------
      description:  Install a databases instance.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool InstallDatabase ( DBMCli_Database * ppDatabase, 
                                 Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     DropDatabase
      -------------------------------------------------------------------------
      description:  Drop a databases instance.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool DropDatabase ( DBMCli_Database ** ppDatabase,
                              Msg_List & oMsgList );


    DBMCli_Node & GetNode ( ) { return m_oNode; };

    DBMCli_String & GetDatabaseName ( ) { return m_sDatabaseName; };
    DBMCli_String & GetServerName   ( ) { return m_sServerName; };
    DBMCli_String & GetOSUserName   ( ) { return m_sOSUserName; };
    DBMCli_String & GetOSPassword   ( ) { return m_sOSPassword; };

    SAPDB_Bool GetReinstall         ( ) { return m_bReinstall; };

    DBMCli_Version & GetVersion     ( ) { return m_oVersion; };
    DBMCli_String & GetInstRoot     ( ) { return m_sInstRoot; };

    DBMCli_String & GetDBMName      ( ) { return m_sDBMName; };
    DBMCli_String & GetDBMPassword  ( ) { return m_sDBMPwd; };
    DBMCli_String & GetDBAName      ( ) { return m_sDBAName; };
    DBMCli_String & GetDBAPassword  ( ) { return m_sDBAPwd; };

    DBMCli_String & GetInitMode     ( ) { return m_sInitMode; };

    DBMCli_String & GetInstanceType ( ) { return m_sInstanceType; };

    DBMCli_String & GetInstallMode  ( ) { return m_sInstallMode; };

    SAPDB_Bool GetCreatedFlag       ( ) { return m_bCreated; };
    SAPDB_Bool GetConnectedFlag     ( ) { return m_bConnected; };
    SAPDB_Bool GetStartedFlag       ( ) { return m_bStart; };
    SAPDB_Bool GetInitConfigFlag    ( ) { return m_bInitConfig; };
    SAPDB_Bool GetActivatedFlag     ( ) { return m_bActivate; };
    SAPDB_Bool GetLoadSysTabFlag    ( ) { return m_bLoadSysTab; };

    void SetDatabaseName ( const DBMCli_String & sDatabaseName ) { m_sDatabaseName = sDatabaseName; };
    void SetServerName   ( const DBMCli_String & sServerName )   { m_sServerName = sServerName; };
    void SetOSUserName   ( const DBMCli_String & sOSUserName )   { m_sOSUserName = sOSUserName; };
    void SetOSPassword   ( const DBMCli_String & sOSPassword )   { m_sOSPassword = sOSPassword; };

    void SetReinstall    ( const SAPDB_Bool bReinstall )         { m_bReinstall = bReinstall; };

    void SetVersion      ( const DBMCli_String & sVersion )      { m_oVersion.SetName( sVersion ); };
    void SetInstRoot     ( const DBMCli_String & sInstRoot )     { m_sInstRoot= sInstRoot; };

    void SetDBMName      ( const DBMCli_String & sDBMName )      { m_sDBMName = sDBMName; };
    void SetDBMPwd       ( const DBMCli_String & sDBMPwd )       { m_sDBMPwd = sDBMPwd; };
    void SetDBAName      ( const DBMCli_String & sDBAName )      { m_sDBAName = sDBAName; };
    void SetDBAPwd       ( const DBMCli_String & sDBAPwd )       { m_sDBAPwd = sDBAPwd; };

    void SetInitMode     ( const DBMCli_String & sInitMode)      { m_sInitMode = sInitMode; };

    void SetInstanceType ( const DBMCli_String & sInstanceType)  { m_sInstanceType = sInstanceType; };

    void SetInstallMode  ( const DBMCli_String & sInstallMode)   { m_sInstallMode = sInstallMode; };

  private: 

    DBMCli_Node     m_oNode;

    DBMCli_String   m_sDatabaseName;
    DBMCli_String   m_sServerName;
    DBMCli_String   m_sOSUserName;
    DBMCli_String   m_sOSPassword;

    SAPDB_Bool      m_bReinstall;

    DBMCli_Version  m_oVersion;
    DBMCli_String   m_sInstRoot;

    DBMCli_String   m_sDBMName;
    DBMCli_String   m_sDBMPwd;
    DBMCli_String   m_sDBAName;
    DBMCli_String   m_sDBAPwd;

    DBMCli_String   m_sInitMode;

    DBMCli_String   m_sInstanceType;

    SAPDB_Bool      m_bCreated;
    SAPDB_Bool      m_bConnected;

    DBMCli_String   m_sInstallMode;

    SAPDB_Bool      m_bStart;
    SAPDB_Bool      m_bInitConfig;
    SAPDB_Bool      m_bActivate;
    SAPDB_Bool      m_bLoadSysTab; 
}; 

/*! EndClass: DBMCli_Wizard */

#endif // _DBMCLI_WIZARD_HPP_

