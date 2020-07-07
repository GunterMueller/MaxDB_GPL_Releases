/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_Backup.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager Client Library
  description:  Backup
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

#ifndef _DBMCLI_BACKUP_HPP_
#define _DBMCLI_BACKUP_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "DBM/Cli/DBMCli_Common.hpp"
#include "DBM/Cli/DBMCli_Result.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define DBMCLI_BACKUPRC_OK    0
#define DBMCLI_BACKUPRC_FULL -8020
#define DBMCLI_BACKUPRC_SKIP -7075

/*
  -----------------------------------------------------------------------------
  typedefs
  -----------------------------------------------------------------------------
*/
typedef enum
{
  DBMCLI_BACKUPTYPE_UNKNOWN = 0,
  DBMCLI_BACKUPTYPE_DATA    = 1,
  DBMCLI_BACKUPTYPE_PAGES   = 2,
  DBMCLI_BACKUPTYPE_LOG     = 3,
  DBMCLI_BACKUPTYPE_AUTO    = 4
} DBMCli_BackupTypeEnum;

typedef enum
{
  DBMCLI_BACKUPFOR_UNKNOWN   = 0,
  DBMCLI_BACKUPFOR_RECOVERY  = 1,
  DBMCLI_BACKUPFOR_MIGRATION = 2
} DBMCli_BackupForEnum;

/*
  -----------------------------------------------------------------------------
  froward declarations
  -----------------------------------------------------------------------------
*/
class DBMCli_Database;

/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_BackupFor
  -----------------------------------------------------------------------------
  description: 
  -----------------------------------------------------------------------------
*/
class DBMCli_BackupFor
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_BackupFor
      -------------------------------------------------------------------------
      description:  Constructor
      prototypes:   3
      -------------------------------------------------------------------------
    */
    DBMCli_BackupFor ( );
    DBMCli_BackupFor ( const DBMCli_String & sBackupFor );
    DBMCli_BackupFor ( const DBMCli_BackupForEnum nBackupFor );

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_BackupFor
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_BackupFor ( );

    /*!
      -------------------------------------------------------------------------
      function:     Value
      -------------------------------------------------------------------------
    */
    DBMCli_BackupForEnum Value ( ) { return m_nBackupFor; };

    /*!
      -------------------------------------------------------------------------
      function:     SetValue
      -------------------------------------------------------------------------
    */
    void SetValue ( DBMCli_BackupForEnum nBackupFor ) { m_nBackupFor = nBackupFor; };

    /*!
      -------------------------------------------------------------------------
      function:     Name
      -------------------------------------------------------------------------
    */
    const DBMCli_String Name ( );

    /*!
      -------------------------------------------------------------------------
      function:     SetName
      -------------------------------------------------------------------------
    */
    void SetName ( const DBMCli_String & sName );

  private:

    DBMCli_BackupForEnum m_nBackupFor;
};

/*! EndClass: DBMCli_BackupFor */

/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_BackupType
  -----------------------------------------------------------------------------
  description: 
  -----------------------------------------------------------------------------
*/
class DBMCli_BackupType
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_BackupType
      -------------------------------------------------------------------------
      description:  Constructor
      prototypes:   3
      -------------------------------------------------------------------------
    */
    DBMCli_BackupType ( );
    DBMCli_BackupType ( const DBMCli_String & sBackupType );
    DBMCli_BackupType ( const DBMCli_BackupTypeEnum nBackupType );

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_BackupType
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_BackupType ( );

    /*!
      -------------------------------------------------------------------------
      function:     Value
      -------------------------------------------------------------------------
    */
    DBMCli_BackupTypeEnum Value ( ) { return m_nBackupType; };

    /*!
      -------------------------------------------------------------------------
      function:     SetValue
      -------------------------------------------------------------------------
    */
    void SetValue ( DBMCli_BackupTypeEnum nBackupType ) { m_nBackupType = nBackupType; };

    /*!
      -------------------------------------------------------------------------
      function:     Name
      arguments:    nType [IN] - 0 = DATA, PAGES, LOG, AUTO
                                 1 = Complete, Incremental, Log, Auto Log 
                                 2 = Complete Data, Incremental Data, Log, Auto Log
      -------------------------------------------------------------------------
    */
    const DBMCli_String Name ( const SAPDB_Int4 nType = 0 ); 

    /*!
      -------------------------------------------------------------------------
      function:     SetName
      -------------------------------------------------------------------------
    */
    void SetName ( const DBMCli_String & sName );

  private:

    DBMCli_BackupTypeEnum m_nBackupType;
};

/*! EndClass: DBMCli_BackupType */

/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_BackupResult
  -----------------------------------------------------------------------------
  description: Class to handle backup result.
  -----------------------------------------------------------------------------
*/
class DBMCli_BackupResult
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_BackupResult
      -------------------------------------------------------------------------
      description:  Constructor
      -------------------------------------------------------------------------
    */
    DBMCli_BackupResult ( );

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_BackupResult
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_BackupResult ( );

    /*!
      -------------------------------------------------------------------------
      function:     Clear
      -------------------------------------------------------------------------
      description:  Clear the state properties.
      -------------------------------------------------------------------------
    */
    void Clear ( );

    /*!
      -------------------------------------------------------------------------
      function:     SetByResultBuf
      -------------------------------------------------------------------------
      description:  Set the backup result properties by a result buf.
      arguments:    oResultBuf [IN] - object for indication of errors
      -------------------------------------------------------------------------
    */
    void SetByResultBuf ( DBMCli_Result & oResult );

    /*!  
      Chapter: Properties
    */
    
    /*!
      -------------------------------------------------------------------------
      function:     Returncode
      -------------------------------------------------------------------------
    */
    SAPDB_Int4 Returncode ( ) { return m_nReturncode; };

    /*!
      -------------------------------------------------------------------------
      function:     ReturncodeStr
      -------------------------------------------------------------------------
    */
    const DBMCli_String & ReturncodeStr ( ) { return m_sReturncode; };

    /*!
      -------------------------------------------------------------------------
      function:     DateStr
      -------------------------------------------------------------------------
    */
    const DBMCli_String & DateStr ( ) { return m_sDate; };

    /*!
      -------------------------------------------------------------------------
      function:     TimeStr
      -------------------------------------------------------------------------
    */
    const DBMCli_String & TimeStr ( ) { return m_sTime; };

    /*!
      -------------------------------------------------------------------------
      function:     BeginningStr
      -------------------------------------------------------------------------
    */
    const DBMCli_String & BeginningStr ( ) { return m_sBeginning; };

    /*!
      -------------------------------------------------------------------------
      function:     ServerNameStr
      -------------------------------------------------------------------------
    */
    const DBMCli_String & ServerNameStr ( ) { return m_sServerName; };

    /*!
      -------------------------------------------------------------------------
      function:     DatabaseNameStr
      -------------------------------------------------------------------------
    */
    const DBMCli_String & DatabaseNameStr ( ) { return m_sDatabaseName; };

    /*!
      -------------------------------------------------------------------------
      function:     KernelVersionStr
      -------------------------------------------------------------------------
    */
    const DBMCli_String & KernelVersionStr ( ) { return m_sKernelVersion; };

    /*!
      -------------------------------------------------------------------------
      function:     TransferredPages
      -------------------------------------------------------------------------
    */
    SAPDB_Int4 TransferredPages ( ) { return m_nTransferredPages; };

    /*!
      -------------------------------------------------------------------------
      function:     TransferredPagesStr
      -------------------------------------------------------------------------
    */
    const DBMCli_String & TransferredPagesStr ( ) { return m_sTransferredPages; };

    /*!
      -------------------------------------------------------------------------
      function:     LeftPages
      -------------------------------------------------------------------------
    */
    SAPDB_Int4 LeftPages ( ) { return m_nLeftPages; };

    /*!
      -------------------------------------------------------------------------
      function:     LeftPagesStr
      -------------------------------------------------------------------------
    */
    const DBMCli_String & LeftPagesStr ( ) { return m_sLeftPages; };

    /*!
      -------------------------------------------------------------------------
      function:     Volumes
      -------------------------------------------------------------------------
    */
    SAPDB_Int4 Volumes ( ) { return m_nVolumes; };

    /*!
      -------------------------------------------------------------------------
      function:     VolumesStr
      -------------------------------------------------------------------------
    */
    const DBMCli_String & VolumesStr ( ) { return m_sVolumes; };

    /*!
      -------------------------------------------------------------------------
      function:     MediaNameStr
      -------------------------------------------------------------------------
    */
    const DBMCli_String & MediaNameStr ( ) { return m_sMediaName; };

    /*!
      -------------------------------------------------------------------------
      function:     LocationStr
      -------------------------------------------------------------------------
    */
    const DBMCli_String & LocationStr ( ) { return m_sLocation; };

    /*!
      -------------------------------------------------------------------------
      function:     ErrorTextStr
      -------------------------------------------------------------------------
    */
    const DBMCli_String & ErrorTextStr ( ) { return m_sErrorText; };

    /*!
      -------------------------------------------------------------------------
      function:     LabelStr
      -------------------------------------------------------------------------
    */
    const DBMCli_String & LabelStr ( ) { return m_sLabel; };

    /*!
      -------------------------------------------------------------------------
      function:     IsConsistent
      -------------------------------------------------------------------------
    */
    SAPDB_Bool IsConsistent ( ) { return m_bIsConsistent; };

    /*!
      -------------------------------------------------------------------------
      function:     IsConsistentStr
      -------------------------------------------------------------------------
    */
    const DBMCli_String & IsConsistentStr ( ) { return m_sIsConsistent; };

    /*!
      -------------------------------------------------------------------------
      function:     FirstLogPage
      -------------------------------------------------------------------------
    */
    SAPDB_Int4 FirstLogPage ( ) { return m_nFirstLogPage; };

    /*!
      -------------------------------------------------------------------------
      function:     FirstLogPageStr
      -------------------------------------------------------------------------
    */
    const DBMCli_String & FirstLogPageStr ( ) { return m_sFirstLogPage; };

    /*!
      -------------------------------------------------------------------------
      function:     LastLogPage
      -------------------------------------------------------------------------
    */
    SAPDB_Int4 LastLogPage ( ) { return m_nLastLogPage; };

    /*!
      -------------------------------------------------------------------------
      function:     LastLogPageStr
      -------------------------------------------------------------------------
    */
    const DBMCli_String & LastLogPageStr ( ) { return m_sLastLogPage; };

    /*!
      -------------------------------------------------------------------------
      function:     DBStamp1DateStr
      -------------------------------------------------------------------------
    */
    const DBMCli_String & DBStamp1DateStr ( ) { return m_sDBStamp1Date; };

    /*!
      -------------------------------------------------------------------------
      function:     DBStamp1TimeStr
      -------------------------------------------------------------------------
    */
    const DBMCli_String & DBStamp1TimeStr ( ) { return m_sDBStamp1Time; };

    /*!
      -------------------------------------------------------------------------
      function:     DBStamp1Str
      -------------------------------------------------------------------------
    */
    const DBMCli_String & DBStamp1Str ( ) { return m_sDBStamp1; };

    /*!
      -------------------------------------------------------------------------
      function:     DBStamp2DateStr
      -------------------------------------------------------------------------
    */
    const DBMCli_String & DBStamp2DateStr ( ) { return m_sDBStamp2Date; };

    /*!
      -------------------------------------------------------------------------
      function:     DBStamp2TimeStr
      -------------------------------------------------------------------------
    */
    const DBMCli_String & DBStamp2TimeStr ( ) { return m_sDBStamp2Time; };

    /*!
      -------------------------------------------------------------------------
      function:     DBStamp2Str
      -------------------------------------------------------------------------
    */
    const DBMCli_String & DBStamp2Str ( ) { return m_sDBStamp2; };

    /*!
      -------------------------------------------------------------------------
      function:     PageCount
      -------------------------------------------------------------------------
    */
    SAPDB_Int4 PageCount ( ) { return m_nPageCount; };

    /*!
      -------------------------------------------------------------------------
      function:     PageCountStr
      -------------------------------------------------------------------------
    */
    const DBMCli_String & PageCountStr ( ) { return m_sPageCount; };

    /*!
      -------------------------------------------------------------------------
      function:     DevicesUsed
      -------------------------------------------------------------------------
    */
    SAPDB_Int4 DevicesUsed ( ) { return m_nDevicesUsed; };

    /*!
      -------------------------------------------------------------------------
      function:     DevicesUsedStr
      -------------------------------------------------------------------------
    */
    const DBMCli_String & DevicesUsedStr ( ) { return m_sDevicesUsed; };

    /*!
      -------------------------------------------------------------------------
      function:     DatabaseIDStr
      -------------------------------------------------------------------------
    */
    const DBMCli_String & DatabaseIDStr( ) { return m_sDatabaseID; };

    /*!
      -------------------------------------------------------------------------
      function:     MaxUsedDataPage
      -------------------------------------------------------------------------
    */
    SAPDB_Int4 MaxUsedDataPage ( ) { return m_nMaxUsedDataPage; };

    /*!
      -------------------------------------------------------------------------
      function:     MaxUsedDataPageStr
      -------------------------------------------------------------------------
    */
    const DBMCli_String & MaxUsedDataPageStr ( ) { return m_sMaxUsedDataPage; };

    /*!
      -------------------------------------------------------------------------
      function:     RedoTransRead
      -------------------------------------------------------------------------
    */
    SAPDB_Int4 RedoTransRead ( ) { return m_nRedoTransRead; };

    /*!
      -------------------------------------------------------------------------
      function:     RedoTransReadStr
      -------------------------------------------------------------------------
    */
    const DBMCli_String & RedoTransReadStr ( ) { return m_sRedoTransRead; };

    /*!
      -------------------------------------------------------------------------
      function:     RedoTransDone
      -------------------------------------------------------------------------
    */
    SAPDB_Int4 RedoTransDone ( ) { return m_nRedoTransDone; };

    /*!
      -------------------------------------------------------------------------
      function:     RedoTransDoneStr
      -------------------------------------------------------------------------
    */
    const DBMCli_String & RedoTransDoneStr ( ) { return m_sRedoTransDone; };

    /*!
      -------------------------------------------------------------------------
      function:     IsReady
      -------------------------------------------------------------------------
    */
    SAPDB_Bool IsReady ( );

    /*!
      -------------------------------------------------------------------------
      function:     IsFull
      -------------------------------------------------------------------------
    */
    SAPDB_Bool IsFull ( ) const;

    /*!
      -------------------------------------------------------------------------
      function:     IsSkipped
      -------------------------------------------------------------------------
    */
    SAPDB_Bool IsSkipped ( );

    /*!  
      EndChapter: Properties
    */

  private:
    void AssignProp ( const DBMCli_String & sProp,
                      const DBMCli_String & sVal );

    void AssignValInt ( const DBMCli_String & sVal,
                        SAPDB_Int4 * pVal );

    void AssignValBool ( const DBMCli_String & sVal,
                         SAPDB_Bool * pVal );

  private:

    SAPDB_Int4    m_nReturncode;
    DBMCli_String m_sReturncode;
    
    DBMCli_String m_sDate;
    DBMCli_String m_sTime;
    DBMCli_String m_sBeginning;

    DBMCli_String m_sServerName;
    DBMCli_String m_sDatabaseName;
    
    DBMCli_String m_sKernelVersion;
    
    SAPDB_Int4    m_nTransferredPages;
    DBMCli_String m_sTransferredPages;

    SAPDB_Int4    m_nLeftPages;
    DBMCli_String m_sLeftPages;

    SAPDB_Int4    m_nVolumes;
    DBMCli_String m_sVolumes;

    DBMCli_String m_sMediaName;
    DBMCli_String m_sLocation;
    DBMCli_String m_sErrorText;
    DBMCli_String m_sLabel;
    
    SAPDB_Bool    m_bIsConsistent;
    DBMCli_String m_sIsConsistent;
    
    SAPDB_Int4    m_nFirstLogPage;
    DBMCli_String m_sFirstLogPage;
    
    SAPDB_Int4    m_nLastLogPage;
    DBMCli_String m_sLastLogPage;

    DBMCli_String m_sDBStamp1Date;
    DBMCli_String m_sDBStamp1Time;
    DBMCli_String m_sDBStamp1;
    DBMCli_String m_sDBStamp2Date;
    DBMCli_String m_sDBStamp2Time;
    DBMCli_String m_sDBStamp2;

    SAPDB_Int4    m_nPageCount;
    DBMCli_String m_sPageCount;

    SAPDB_Int4    m_nDevicesUsed;
    DBMCli_String m_sDevicesUsed;

    DBMCli_String m_sDatabaseID;

    SAPDB_Int4    m_nMaxUsedDataPage;
    DBMCli_String m_sMaxUsedDataPage;

    SAPDB_Int4    m_nRedoTransRead;
    DBMCli_String m_sRedoTransRead;

    SAPDB_Int4    m_nRedoTransDone;
    DBMCli_String m_sRedoTransDone;
}; 

/*! EndClass: DBMCli_BackupResult */

/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_Backup
  -----------------------------------------------------------------------------
  description: Class to handle backup.
  -----------------------------------------------------------------------------
*/
class DBMCli_Backup 
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_Backup
      -------------------------------------------------------------------------
      description:  Constructor
      -------------------------------------------------------------------------
    */
    DBMCli_Backup ( );

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_Backup
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_Backup ( );

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
      function:     ReplyAvailable
      -------------------------------------------------------------------------
      description:  check for a backup reply
      arguments:    bAvailable [OUT] - available flag
                    oMsgList   [OUT] - object for indication of errors
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool ReplyAvailable ( SAPDB_Bool           & bAvailable,
                                Msg_List & oMsgList );


    /*!
      -------------------------------------------------------------------------
      function:     ReplyReceive
      -------------------------------------------------------------------------
      description:  receives the backup reply
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool ReplyReceive ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     StartRequest
      -------------------------------------------------------------------------
      description:  Start the backup action (request only!)
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool StartRequest ( const DBMCli_String  & sMediumName,
                              const DBMCli_String  & sBackupFor,
                              const DBMCli_String  & sBackupType,
                              const DBMCli_String  & sMediumType,
                              Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     ReplaceRequest
      -------------------------------------------------------------------------
      description:  Medium was replaced continue backup with next medium. (request only!)
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool ReplaceRequest ( const DBMCli_String  & sMedium,
                                const DBMCli_String  & sLocation,
                                Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     IgnoreRequest
      -------------------------------------------------------------------------
      description:  Medium was ignored continue backup. (request only!)
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */

    SAPDB_Bool IgnoreRequest ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     CancelRequest
      -------------------------------------------------------------------------
      description:  Cancel the backup action. (request only!)
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */

    SAPDB_Bool CancelRequest ( Msg_List & oMsgList );
    /*!
      -------------------------------------------------------------------------
      function:     Cancel
      -------------------------------------------------------------------------
      description:  Cancel the backup action.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Cancel ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     State
      -------------------------------------------------------------------------
      description:  retrieves the stateof the backup action.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool State ( Msg_List & oMsgList );
    
    /*!
      -------------------------------------------------------------------------
      function:     Result
      -------------------------------------------------------------------------
      description:  Get the backup result.
      return value: Reference to the backup result object.
      -------------------------------------------------------------------------
    */
    DBMCli_BackupResult & Result ( ) { return m_oResult; };

    /*!
      -------------------------------------------------------------------------
      function:     IgnoreCount
      -------------------------------------------------------------------------
      description:  Get the ignore count
      return value: the count
      -------------------------------------------------------------------------
    */
    SAPDB_Int IgnoreCount ( ) { return m_nIgnoreCount; };

    /*!
      -------------------------------------------------------------------------
      function:     IsRunning
      -------------------------------------------------------------------------
      description:  Get the running flag
      return value: the flad 
      -------------------------------------------------------------------------
    */
    SAPDB_Bool IsRunning ( ) { return m_bRunning; };

    /*!
      -------------------------------------------------------------------------
      function:     IsAnswerReceived
      -------------------------------------------------------------------------
      description:  Get the received flag
      return value: the flad 
      -------------------------------------------------------------------------
    */
    SAPDB_Bool IsAnswerReceived ( ) { return m_bAnswerReceived; };

    /*!
      -------------------------------------------------------------------------
      function:     GetMediumName
      -------------------------------------------------------------------------
      description:  name of medium
      return value: name of medium
      -------------------------------------------------------------------------
    */
    const DBMCli_String & GetMediumName() {return m_sMediumName;}

    /*!
      -------------------------------------------------------------------------
      function:     GetMediumType
      -------------------------------------------------------------------------
      description:  type of medium
      return value: type of medium
      -------------------------------------------------------------------------
    */
    const DBMCli_String & GetMediumType() {return m_sMediumType;}

    /*!
      -------------------------------------------------------------------------
      function:     GetBackupType
      -------------------------------------------------------------------------
      description:  type of backup
      return value: type of backup
      -------------------------------------------------------------------------
    */
    const DBMCli_String & GetBackupType() {return m_sBackupType;}
    
  private:
    DBMCli_Database     * m_pDatabase;
    DBMCli_BackupResult   m_oResult;
    SAPDB_Int4            m_nIgnoreCount;
    SAPDB_Bool            m_bRunning;
    SAPDB_Bool            m_bAnswerReceived;

    // Data for state
    DBMCli_String         m_sMediumName;
    DBMCli_String         m_sMediumType;
    DBMCli_String         m_sBackupType;

}; 

/*! EndClass: DBMCli_Backup */

#endif // _DBMCLI_BACKUP_HPP_

