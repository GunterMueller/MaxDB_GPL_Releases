/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_Database.hpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager Client Library
  description:  Database Class
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

#ifndef _DBMCLI_DATABASE_HPP_
#define _DBMCLI_DATABASE_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "DBM/Cli/DBMCli_Common.hpp"
#include "DBM/Cli/DBMCli_Node.hpp"
#include "DBM/Cli/DBMCli_State.hpp"
#include "DBM/Cli/DBMCli_Info.hpp"
#include "DBM/Cli/DBMCli_History.hpp"
#include "DBM/Cli/DBMCli_File.hpp"
#include "DBM/Cli/DBMCli_Show.hpp"
#include "DBM/Cli/DBMCli_Parameter.hpp"
#include "DBM/Cli/DBMCli_Devspace.hpp"
#include "DBM/Cli/DBMCli_Media.hpp"
#include "DBM/Cli/DBMCli_Backup.hpp"
#include "DBM/Cli/DBMCli_Recover.hpp"
#include "DBM/Cli/DBMCli_Autolog.hpp"
#include "DBM/Cli/DBMCli_KernelTrace.hpp"
#include "DBM/Cli/DBMCli_UpdStat.hpp"
#include "DBM/Cli/DBMCli_Indexes.hpp"
#include "DBM/Cli/DBMCli_LogModeObj.hpp"
#include "DBM/Cli/DBMCli_User.hpp"
#include "DBM/Cli/DBMCli_Config.hpp"

/*
  -----------------------------------------------------------------------------
  typedefs
  -----------------------------------------------------------------------------
*/
typedef enum  
{ 
	DBMCLI_KERNEL_FAST = 0,
	DBMCLI_KERNEL_QUICK = 1,
	DBMCLI_KERNEL_SLOW = 2
} DBMCli_KernelType;

/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_Database
  -----------------------------------------------------------------------------
  description: Class to handle the base database operations of the dbm server. 
  -----------------------------------------------------------------------------
*/
class DBMCli_Database : public DBMCli_Node
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_Database
      -------------------------------------------------------------------------
      description:  Constructor
      arguments:    sServer    [IN] - server node
                    sDatabase  [IN] - database name
                    sUser      [IN] - user name
                    oMsgList  [OUT] - Object for indication of errors
      -------------------------------------------------------------------------
    */
    DBMCli_Database ( const DBMCli_String        & sServer,
                      const DBMCli_String        & sDatabase,
                      const DBMCli_String        & sUser,
                            Msg_List & oMsgList,
                      const SAPDB_Bool             bConnect = true );

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_Database
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_Database ( );

    /*!
      -------------------------------------------------------------------------
      function:     DatabaseOnServer
      -------------------------------------------------------------------------
      description:  Get the full databasename "<db> on <serevr>"
      -------------------------------------------------------------------------
    */
    const DBMCli_String DatabaseOnServer( );

    /*!
      -------------------------------------------------------------------------
      function:     GetState
      -------------------------------------------------------------------------
      description:  Get the state object.
      return value: Reference to a state object.
      -------------------------------------------------------------------------
    */
    DBMCli_State & GetState ( ) { return m_oState; };

    /*!
      -------------------------------------------------------------------------
      function:     GetInfo
      -------------------------------------------------------------------------
      description:  Get the info object.
      return value: Reference to an info object
      -------------------------------------------------------------------------
    */
    DBMCli_Info & GetInfo ( ) { return m_oInfo; };

    /*!
      -------------------------------------------------------------------------
      function:     GetHistory
      -------------------------------------------------------------------------
      description:  Get the history object.
      return value: Reference to a history object
      -------------------------------------------------------------------------
    */
    DBMCli_History & GetHistory ( ) { return m_oHistory; };

    /*!
      -------------------------------------------------------------------------
      function:     GetFile
      -------------------------------------------------------------------------
      description:  Get the file object.
      return value: Reference to a file object.
      -------------------------------------------------------------------------
    */
    DBMCli_File & GetFile ( ) { return m_oFile; };

    /*!
      -------------------------------------------------------------------------
      function:     GetFiles
      -------------------------------------------------------------------------
      description:  Get the files object.
      return value: Reference to a files object.
      -------------------------------------------------------------------------
    */
    DBMCli_Files & GetFiles ( ) { return m_oFiles; };

    /*!
      -------------------------------------------------------------------------
      function:     GetDiagnosises
      -------------------------------------------------------------------------
      description:  Get the diagnosises object.
      return value: Reference to a diagnosises object.
      -------------------------------------------------------------------------
    */
    DBMCli_Diagnosises & GetDiagnosises ( ) { return m_oDiagnosises; };

    /*!
      -------------------------------------------------------------------------
      function:     GetShow
      -------------------------------------------------------------------------
      description:  Get the show object.
      return value: Reference to a show object.
      -------------------------------------------------------------------------
    */
    DBMCli_Show & GetShow ( ) { return m_oShow; };

    /*!
      -------------------------------------------------------------------------
      function:     GetShows
      -------------------------------------------------------------------------
      description:  Get the shows object.
      return value: Reference to a shows object.
      -------------------------------------------------------------------------
    */
    DBMCli_Shows & GetShows ( ) { return m_oShows; };

    /*!
      -------------------------------------------------------------------------
      function:     GetParameter
      -------------------------------------------------------------------------
      description:  Get the parameter object.
      return value: Reference to a parameter object.
      -------------------------------------------------------------------------
    */
    DBMCli_Parameter & GetParameter ( ) { return m_oParameter; };

    /*!
      -------------------------------------------------------------------------
      function:     GetParameters
      -------------------------------------------------------------------------
      description:  Get the parameters object.
      return value: Reference to a parameters object.
      -------------------------------------------------------------------------
    */
    DBMCli_Parameters & GetParameters ( ) { return m_oParameters; };

    /*!
      -------------------------------------------------------------------------
      function:     GetDevspace
      -------------------------------------------------------------------------
      description:  Get the devspace object.
      return value: Reference to a Devspace object.
      -------------------------------------------------------------------------
    */
    DBMCli_Devspace & GetDevspace ( ) { return m_oDevspace; };

    /*!
      -------------------------------------------------------------------------
      function:     GetDevspaces
      -------------------------------------------------------------------------
      description:  Get the devspaces object.
      return value: Reference to a Devspaces object.
      -------------------------------------------------------------------------
    */
    DBMCli_Devspaces & GetDevspaces ( ) { return m_oDevspaces; };

    /*!
      -------------------------------------------------------------------------
      function:     GetMedia
      -------------------------------------------------------------------------
      description:  Get the media object.
      return value: Reference to a Media object.
      -------------------------------------------------------------------------
    */
    DBMCli_Media & GetMedia ( ) { return m_oMedia; };

    /*!
      -------------------------------------------------------------------------
      function:     GetBackup
      -------------------------------------------------------------------------
      description:  Get the backup object.
      return value: Reference to a Backup object.
      -------------------------------------------------------------------------
    */
    DBMCli_Backup & GetBackup ( ) { return m_oBackup; };

    /*!
      -------------------------------------------------------------------------
      function:     GetRecover
      -------------------------------------------------------------------------
      description:  Get the recover object.
      return value: Reference to a Recover object.
      -------------------------------------------------------------------------
    */
    DBMCli_Recover & GetRecover ( ) { return m_oRecover; };

    /*!
      -------------------------------------------------------------------------
      function:     GetRestartInfo
      -------------------------------------------------------------------------
      description:  Get the restart info object.
      return value: Reference to the restart info object.
      -------------------------------------------------------------------------
    */
    DBMCli_RestartInfo & GetRestartInfo ( ) { return m_oRestartInfo; };

    /*!
      -------------------------------------------------------------------------
      function:     GetAutolog
      -------------------------------------------------------------------------
      description:  Get the Autolog object.
      return value: Reference to a Autolog object.
      -------------------------------------------------------------------------
    */
    DBMCli_Autolog & GetAutolog ( ) { return m_oAutolog; };

    /*!
      -------------------------------------------------------------------------
      function:     GetKernelTrace
      -------------------------------------------------------------------------
      description:  Get the KernelTrace object.
      return value: Reference to a KernelTrace object.
      -------------------------------------------------------------------------
    */
    DBMCli_KernelTrace & GetKernelTrace( ) { return m_oKernelTrace; };

    /*!
      -------------------------------------------------------------------------
      function:     UpdStat
      -------------------------------------------------------------------------
      description:  Get the UpdStat object.
      return value: Reference to a UpdStat object.
      -------------------------------------------------------------------------
    */
    DBMCli_UpdStat & GetUpdStat( ) { return m_oUpdStat; };

    /*!
      -------------------------------------------------------------------------
      function:     Indexes
      -------------------------------------------------------------------------
      description:  Get the Indexes object.
      return value: Reference to a Indexes object.
      -------------------------------------------------------------------------
    */
    DBMCli_Indexes & GetIndexes( ) { return m_oIndexes; };

    /*!
      -------------------------------------------------------------------------
      function:     GetLogMode
      -------------------------------------------------------------------------
      description:  Get the LogMode object.
      return value: Reference to a LogMode object.
      -------------------------------------------------------------------------
    */
    DBMCli_LogModeObj & GetLogModeObj( ) { return m_oLogModeObj; };

    /*!
      -------------------------------------------------------------------------
      function:     GetUsers
      -------------------------------------------------------------------------
      description:  Get the Users object.
      return value: Reference to a Users object.
      -------------------------------------------------------------------------
    */
    DBMCli_Users & GetUsers( ) { return m_oUsers; };

    /*!
      -------------------------------------------------------------------------
      function:     GetConfig
      -------------------------------------------------------------------------
      description:  Get the Configuration object.
      return value: Reference to a Configuration object.
      -------------------------------------------------------------------------
    */
    DBMCli_Config & GetConfig( ) { return m_oConfig; };

    /*!
      -------------------------------------------------------------------------
      function:     Start
      -------------------------------------------------------------------------
      description:  Start the database.
      arguments:    nKernel  [IN]  - type of kernel
                    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      prototypes:   2
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Start ( Msg_List & oMsgList );
    SAPDB_Bool Start ( DBMCli_KernelType      nKernel,
                       Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Restart
      -------------------------------------------------------------------------
      description:  Restart the database.
      arguments:    sUntil   [IN]  - restart db until date 
                    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      prototypes:   3
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Restart ( Msg_List & oMsgList );
    SAPDB_Bool Restart ( const DBMCli_String & sUntil,
                         Msg_List & oMsgList );
    SAPDB_Bool Restart ( const DBMCli_DateTime & oUntil,
                         Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Shutdown
      -------------------------------------------------------------------------
      description:  Shutdown the database.
      arguments:    bQuick   [IN]  - indicates a shutdown quick
                    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      prototypes:   2
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Shutdown ( Msg_List & oMsgList );
    SAPDB_Bool Shutdown ( SAPDB_Bool             bQuick,
                          Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Stop
      -------------------------------------------------------------------------
      description:  Stop the database.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Stop ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Offline
      -------------------------------------------------------------------------
      description:  Set the database to offline.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Offline ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Cold
      -------------------------------------------------------------------------
      description:  Set the database to cold.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Cold ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Warm
      -------------------------------------------------------------------------
      description:  Set the database to warm.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Warm ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     UTLIsConnected
      -------------------------------------------------------------------------
      description:  Is utillity session is connected?
      return value: True if utility session is connected, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool UTLIsConnected ( );

    /*!
      -------------------------------------------------------------------------
      function:     UTLConnect
      -------------------------------------------------------------------------
      description:  Open a utility session.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool UTLConnect ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     UTLExecute
      -------------------------------------------------------------------------
      description:  Execute a utility statement. 
      arguments:    sCommand [IN]  - utility statement
                    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool UTLExecute ( const DBMCli_String  & sCommand,
                            Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     UTLRelease
      -------------------------------------------------------------------------
      description:  Close a utility session. 
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool UTLRelease ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     SQLIsConnected
      -------------------------------------------------------------------------
      description:  Is sql session is connected?
      return value: True if sql session is connected, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool SQLIsConnected ( );

    /*!
      -------------------------------------------------------------------------
      function:     SQLConnect
      -------------------------------------------------------------------------
      description:  Open a SQL session.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool SQLConnect ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     SQLExecute
      -------------------------------------------------------------------------
      description:  Execute a sql statement. 
      arguments:    sCommand [IN]  - sql statement
                    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool SQLExecute ( const DBMCli_String  & sCommand,
                            Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     SQLRelease
      -------------------------------------------------------------------------
      description:  Close a SQL session. 
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool SQLRelease ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     InitConfig
      -------------------------------------------------------------------------
      description:  Initialize configuration for database. 
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool InitConfig ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Activate
      -------------------------------------------------------------------------
      description:  Activate database instance. 
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Activate ( const DBMCli_String & sDBAName,
                          const DBMCli_String & sDBAPwd,
                          Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     LoadSysTab
      -------------------------------------------------------------------------
      description:  Load system tables for database instance. 
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool LoadSysTab ( const DBMCli_String & sDBAName,
                            const DBMCli_String & sDBAPwd,
                            const DBMCli_String & sDomainPwd,
                            Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Command
      -------------------------------------------------------------------------
      description:  Execute a DBM-Server command. 
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Command ( const DBMCli_String & sCommand,
                         Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     SRVConnect
      -------------------------------------------------------------------------
      description:  Open a service session.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool SRVConnect ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     SRVRelease
      -------------------------------------------------------------------------
      description:  Close a service session. 
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool SRVRelease ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Verify
      -------------------------------------------------------------------------
      description:  Verify the database. 
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Verify ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Kernel
      -------------------------------------------------------------------------
    */
    DBMCli_KernelType  Kernel( ) { return m_nKernel; };


  private: 
    const DBMCli_String GetKernelOption ( );

  private:
    DBMCli_State        m_oState;
    DBMCli_Info         m_oInfo;
    DBMCli_History      m_oHistory;
    DBMCli_Files        m_oFiles;
    DBMCli_Diagnosises  m_oDiagnosises;
    DBMCli_File         m_oFile;
    DBMCli_Shows        m_oShows;
    DBMCli_Show         m_oShow;
    DBMCli_Parameters   m_oParameters;
    DBMCli_Parameter    m_oParameter;
    DBMCli_Devspaces    m_oDevspaces;
    DBMCli_Devspace     m_oDevspace;
    DBMCli_Media        m_oMedia;
    DBMCli_Backup       m_oBackup;
    DBMCli_Recover      m_oRecover;
    DBMCli_RestartInfo  m_oRestartInfo;
    DBMCli_Autolog      m_oAutolog;
    DBMCli_KernelTrace  m_oKernelTrace;
    DBMCli_UpdStat      m_oUpdStat;
    DBMCli_Indexes      m_oIndexes;
    DBMCli_LogModeObj   m_oLogModeObj;
    DBMCli_Users        m_oUsers;
    DBMCli_Config       m_oConfig;

    DBMCli_KernelType   m_nKernel;

    SAPDB_Bool          m_bUTLConnected;
    SAPDB_Bool          m_bSQLConnected;
    SAPDB_Bool          m_bSRVConnected;
}; 

/*! EndClass: DBMCli_Database */

#endif // _DBMCLI_DATABASE_HPP_

