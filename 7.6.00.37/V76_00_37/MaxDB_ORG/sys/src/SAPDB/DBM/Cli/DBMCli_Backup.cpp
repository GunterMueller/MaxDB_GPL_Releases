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

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "DBM/Cli/DBMCli_Database.hpp"
#include "DBM/Cli/DBMCli_Backup.hpp"
#include "DBM/Cli/DBMCli_DateTime.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define BACKUPFORNAME_UNKNOWN   "UNKNOWN"
#define BACKUPFORNAME_RECOVERY  "RECOVERY"
#define BACKUPFORNAME_MIGRATION "MIGRATION"

#define BACKUPTYPENAME_UNKNOWN  "UNKNOWN"
#define BACKUPTYPENAME_DATA     "DATA"
#define BACKUPTYPENAME_PAGES    "PAGES"
#define BACKUPTYPENAME_LOG      "LOG"
#define BACKUPTYPENAME_AUTO     "AUTO"

#define BACKUPTYPENAME1_UNKNOWN  "Unknown"
#define BACKUPTYPENAME1_DATA     "Complete"
#define BACKUPTYPENAME1_PAGES    "Incremental"
#define BACKUPTYPENAME1_LOG      "Log"
#define BACKUPTYPENAME1_AUTO     "Auto Log"

#define BACKUPTYPENAME2_UNKNOWN  "Unknown"
#define BACKUPTYPENAME2_DATA     "Complete Data"
#define BACKUPTYPENAME2_PAGES    "Incremental Data"
#define BACKUPTYPENAME2_LOG      "Log"
#define BACKUPTYPENAME2_AUTO     "Auto Log"

#define PROPVAL_SEP "\t"

#define PROP_RC     "RETURNCODE"
#define PROP_DATE   "DATE"
#define PROP_TIME   "TIME"
#define PROP_SRV    "SERVER"
#define PROP_DB     "DATABASE"
#define PROP_VER    "KERNEL VERSION"
#define PROP_TRANS  "PAGES TRANSFERRED"
#define PROP_LEFT   "PAGES LEFT"
#define PROP_VOLS   "VOLUMES"
#define PROP_MEDIA  "MEDIANAME"
#define PROP_LOC    "LOCATION"                
#define PROP_ERR    "ERRORTEXT"
#define PROP_LBL    "LABEL"                   
#define PROP_CONS   "IS CONSISTENT"
#define PROP_FIRST  "FIRST LOG PAGE"          
#define PROP_LAST   "LAST LOG PAGE"
#define PROP_DBS1D  "DB STAMP 1 DATE"         
#define PROP_DBS1T  "DB STAMP 1 TIME"         
#define PROP_DBS2D  "DB STAMP 2 DATE"
#define PROP_DBS2T  "DB STAMP 2 TIME"
#define PROP_PCNT   "PAGE COUNT"              
#define PROP_DEVS   "DEVICES USED"            
#define PROP_DBID   "DATABASE ID"             
#define PROP_MUDP   "MAX USED DATA PAGE"      
#define PROP_REDOR  "REDO TRANSACTIONS READ"      
#define PROP_REDOD  "REDO TRANSACTIONS DONE"      

/*    
  =============================================================================
  class: DBMCli_BackupFor
  =============================================================================
*/
/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_BackupFor :: DBMCli_BackupFor
  -----------------------------------------------------------------------------
*/
DBMCli_BackupFor :: DBMCli_BackupFor ( )
{
  m_nBackupFor = DBMCLI_BACKUPFOR_UNKNOWN;
}

DBMCli_BackupFor :: DBMCli_BackupFor ( const DBMCli_String & sBackupFor )
{
  SetName( sBackupFor );
}

DBMCli_BackupFor :: DBMCli_BackupFor ( const DBMCli_BackupForEnum nBackupFor )
                  : m_nBackupFor( nBackupFor )
{
}

/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_BackupFor :: ~DBMCli_BackupFor
  -----------------------------------------------------------------------------
*/
DBMCli_BackupFor :: ~DBMCli_BackupFor ( )
{
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_BackupFor :: Name
  -----------------------------------------------------------------------------
*/
const DBMCli_String DBMCli_BackupFor :: Name ( )
{
  DBMCli_String sName;

  switch( m_nBackupFor ) {
    case DBMCLI_BACKUPFOR_RECOVERY:
      sName = BACKUPFORNAME_RECOVERY;
      break;
    case DBMCLI_BACKUPFOR_MIGRATION:
      sName = BACKUPFORNAME_MIGRATION;
      break;
    default:
      sName = BACKUPFORNAME_UNKNOWN;
      break;
  }

  return sName;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_BackupFor :: SetName
  -----------------------------------------------------------------------------
*/
void DBMCli_BackupFor :: SetName ( const DBMCli_String & sName )
{
  if( sName == BACKUPFORNAME_RECOVERY ) {
    m_nBackupFor = DBMCLI_BACKUPFOR_RECOVERY;
  } else if( sName == BACKUPFORNAME_MIGRATION ) {
    m_nBackupFor = DBMCLI_BACKUPFOR_MIGRATION;
  } else {
    m_nBackupFor = DBMCLI_BACKUPFOR_UNKNOWN;
  }
}

/*    
  =============================================================================
  class: DBMCli_BackupType
  =============================================================================
*/
/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_BackupType :: DBMCli_BackupType
  -----------------------------------------------------------------------------
*/
DBMCli_BackupType :: DBMCli_BackupType ( )
{
  m_nBackupType = DBMCLI_BACKUPTYPE_UNKNOWN;
}

DBMCli_BackupType :: DBMCli_BackupType ( const DBMCli_String & sBackupType )
{
  SetName( sBackupType );
}

DBMCli_BackupType :: DBMCli_BackupType ( const DBMCli_BackupTypeEnum nBackupType )
                   : m_nBackupType( nBackupType )
{
}

/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_BackupType :: ~DBMCli_BackupType
  -----------------------------------------------------------------------------
*/
DBMCli_BackupType :: ~DBMCli_BackupType ( )
{
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_BackupType :: Name
  -----------------------------------------------------------------------------
*/
const DBMCli_String DBMCli_BackupType :: Name ( const SAPDB_Int4 nType )
{
  DBMCli_String sName;

  switch( m_nBackupType ) {
    case DBMCLI_BACKUPTYPE_DATA: 
      {
        switch( nType ) {
          case 0:
            sName = BACKUPTYPENAME_DATA;
            break;
          case 1:
            sName = BACKUPTYPENAME1_DATA;
            break;
          case 2:
            sName = BACKUPTYPENAME2_DATA;
            break;
          default:
            sName = BACKUPTYPENAME_DATA;
            break;
          }
      }
      break;
    case DBMCLI_BACKUPTYPE_PAGES:
      {
        switch( nType ) {
          case 0:
            sName = BACKUPTYPENAME_PAGES;
            break;
          case 1:
            sName = BACKUPTYPENAME1_PAGES;
            break;
          case 2:
            sName = BACKUPTYPENAME2_PAGES;
            break;
          default:
            sName = BACKUPTYPENAME_PAGES;
            break;
        }
      }
      break;
    case DBMCLI_BACKUPTYPE_LOG:
      {
        switch( nType ) {
          case 0:
            sName = BACKUPTYPENAME_LOG;
            break;
          case 1:
            sName = BACKUPTYPENAME1_LOG;
            break;
          case 2:
            sName = BACKUPTYPENAME2_LOG;
            break;
          default:
            sName = BACKUPTYPENAME_LOG;
            break;
        }
      }
      break;
    case DBMCLI_BACKUPTYPE_AUTO:
      {
        switch( nType ) {
          case 0:
            sName = BACKUPTYPENAME_AUTO;
            break;
          case 1:
            sName = BACKUPTYPENAME1_AUTO;
            break;
          case 2:
            sName = BACKUPTYPENAME2_AUTO;
            break;
          default:
            sName = BACKUPTYPENAME_AUTO;
            break;
        }
      }
      break;
    default:
      {
        switch( nType ) {
          case 0:
            sName = BACKUPTYPENAME_UNKNOWN;
            break;
          case 1:
            sName = BACKUPTYPENAME1_UNKNOWN;
            break;
          case 2:
            sName = BACKUPTYPENAME2_UNKNOWN;
            break;
          default:
            sName = BACKUPTYPENAME_UNKNOWN;
            break;
        }
      }
      break;
  }

  return sName;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_BackupType :: SetName
  -----------------------------------------------------------------------------
*/
void DBMCli_BackupType :: SetName ( const DBMCli_String & sName )
{
  if( sName == BACKUPTYPENAME_DATA ) {
    m_nBackupType = DBMCLI_BACKUPTYPE_DATA;
  } else if( sName == BACKUPTYPENAME_PAGES ) {
    m_nBackupType = DBMCLI_BACKUPTYPE_PAGES;
  } else if( sName == BACKUPTYPENAME_LOG ) {
    m_nBackupType = DBMCLI_BACKUPTYPE_LOG;
  } else if( sName == BACKUPTYPENAME_AUTO ) {
    m_nBackupType = DBMCLI_BACKUPTYPE_AUTO;
  } else {
    m_nBackupType = DBMCLI_BACKUPTYPE_UNKNOWN;
  }
}

/*    
  =============================================================================
  class: DBMCli_BackupResult
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_BackupResult :: DBMCli_BackupResult
  -----------------------------------------------------------------------------
*/
DBMCli_BackupResult :: DBMCli_BackupResult ( )
{
} 

/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_BackupResult :: ~DBMCli_BackupResult
  -----------------------------------------------------------------------------
*/
DBMCli_BackupResult :: ~DBMCli_BackupResult ( )
{
} 

/*
  -----------------------------------------------------------------------------
  public function DBMCli_BackupResult :: Clear
  -----------------------------------------------------------------------------
*/
void DBMCli_BackupResult :: Clear ( )
{
  m_nReturncode = 0;
  m_sReturncode.Empty( );
  m_sDate.Empty( );
  m_sTime.Empty( );
  m_sBeginning.Empty( );
  m_sServerName.Empty( );
  m_sDatabaseName.Empty( );
  m_sKernelVersion.Empty( );
  m_nTransferredPages = 0;
  m_sTransferredPages.Empty( );
  m_nLeftPages = 0;
  m_sLeftPages.Empty( );
  m_nVolumes = 0;
  m_sVolumes.Empty( );
  m_sMediaName.Empty( );
  m_sLocation.Empty( );
  m_sErrorText.Empty( );
  m_sLabel.Empty( );
  m_bIsConsistent = false;
  m_sIsConsistent.Empty( );
  m_nFirstLogPage = 0;
  m_sFirstLogPage.Empty( );
  m_nLastLogPage = 0;
  m_sLastLogPage.Empty( );
  m_sDBStamp1Date.Empty( );
  m_sDBStamp1Time.Empty( );
  m_sDBStamp1.Empty( );
  m_sDBStamp2Date.Empty( );
  m_sDBStamp2Time.Empty( );
  m_sDBStamp2.Empty( );
  m_nPageCount = 0;
  m_sPageCount.Empty( );
  m_nDevicesUsed = 0;
  m_sDevicesUsed.Empty( );
  m_sDatabaseID.Empty( );
  m_nMaxUsedDataPage = 0;
  m_sMaxUsedDataPage.Empty( );
  m_nRedoTransRead = 0;
  m_sRedoTransRead.Empty( );
  m_nRedoTransDone = 0;
  m_sRedoTransDone.Empty( );
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_BackupResult :: SetByResultBuf
  -----------------------------------------------------------------------------
*/
void DBMCli_BackupResult :: SetByResultBuf ( DBMCli_Result & oResult )
{
  Clear( );

  DBMCli_String sProp;
  DBMCli_String sVal;

  while( oResult.GetPropVal( sProp, sVal, PROPVAL_SEP ) ) {
    sProp.MakeUpper( );
    AssignProp( sProp, sVal );
  }

  DBMCli_DateTime oDT;

  oDT.Set( m_sDate, DBMCLI_DT_NUM_DATE_FMT,
           m_sTime, DBMCLI_DT_NUM_TIME_FMT );
  
  m_sBeginning = oDT.Get( DBMCLI_DT_INT_DATETIME_FMT );

  oDT.Set( m_sDBStamp1Date, DBMCLI_DT_NUM_DATE_FMT,
           m_sDBStamp1Time, DBMCLI_DT_NUM_TIME_FMT );

  m_sDBStamp1 = oDT.Get( DBMCLI_DT_INT_DATETIME_FMT );

  oDT.Set( m_sDBStamp2Date, DBMCLI_DT_NUM_DATE_FMT,
           m_sDBStamp2Time, DBMCLI_DT_NUM_TIME_FMT );

  m_sDBStamp2 = oDT.Get( DBMCLI_DT_INT_DATETIME_FMT );
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_BackupResult :: IsReady
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_BackupResult :: IsReady ( )
{
  SAPDB_Bool bRC = false;

  if( m_nReturncode == DBMCLI_BACKUPRC_OK ) {
    bRC = true;
  } 

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_BackupResult :: IsFull
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_BackupResult :: IsFull ( ) const
{
  SAPDB_Bool bRC = false;

  if( m_nReturncode == DBMCLI_BACKUPRC_FULL ) {
    bRC = true;
  } 

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_BackupResult :: IsSkipped
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_BackupResult :: IsSkipped ( )
{
  SAPDB_Bool bRC = false;

  if( m_nReturncode == DBMCLI_BACKUPRC_SKIP ) {
    bRC = true;
  } 

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

void DBMCli_BackupResult :: AssignProp ( const DBMCli_String & sProp,
                                         const DBMCli_String & sVal )
{
  if( sProp == PROP_RC ) {
    m_sReturncode =  sVal;
    AssignValInt( sVal, &m_nReturncode);
  } else if( sProp == PROP_DATE ) {
    m_sDate = sVal;
  } else if( sProp == PROP_TIME ) {
    m_sTime = sVal;
  } else if( sProp == PROP_SRV ) {
    m_sServerName = sVal;
  } else if( sProp == PROP_DB ) {
    m_sDatabaseName = sVal;
  } else if( sProp == PROP_VER ) {
    m_sKernelVersion = sVal;
  } else if( sProp == PROP_TRANS ) {
    m_sTransferredPages = sVal;
    AssignValInt( sVal, &m_nTransferredPages );
  } else if( sProp == PROP_LEFT ) {
    m_sLeftPages = sVal;
    AssignValInt( sVal, &m_nLeftPages );
  } else if( sProp == PROP_VOLS ) {
    m_sVolumes = sVal;
    AssignValInt( sVal, &m_nVolumes );
  } else if( sProp == PROP_MEDIA ) {
    m_sMediaName = sVal;
  } else if( sProp == PROP_LOC ) {
    m_sLocation = sVal;
  } else if( sProp == PROP_ERR ) {
    m_sErrorText= sVal;
  } else if( sProp == PROP_LBL ) {
    m_sLabel= sVal;
  } else if( sProp == PROP_CONS ) {
    m_sIsConsistent = sVal,
    AssignValBool( sVal, &m_bIsConsistent );
  } else if( sProp == PROP_FIRST ) {
    m_sFirstLogPage = sVal;
    AssignValInt( sVal, &m_nFirstLogPage );
  } else if( sProp == PROP_LAST ) {
    m_sLastLogPage = sVal;
    AssignValInt( sVal, &m_nLastLogPage );
  } else if( sProp == PROP_DBS1D ) {
    m_sDBStamp1Date = sVal;
  } else if( sProp == PROP_DBS1T ) {
    m_sDBStamp1Time = sVal;
  } else if( sProp == PROP_DBS2D ) {
    m_sDBStamp2Date = sVal;
  } else if( sProp == PROP_DBS2T ) {
    m_sDBStamp2Time = sVal;
  } else if( sProp == PROP_PCNT ) {
    m_sPageCount = sVal;
    AssignValInt( sVal, &m_nPageCount );
  } else if( sProp == PROP_DEVS ) {
    m_sDevicesUsed = sVal;
    AssignValInt( sVal, &m_nDevicesUsed );
  } else if( sProp == PROP_DBID ) {
    m_sDatabaseID = sVal;
  } else if( sProp == PROP_MUDP ) {
    m_sMaxUsedDataPage = sVal;
    AssignValInt( sVal, &m_nMaxUsedDataPage );
  } else if( sProp == PROP_REDOR ) {
     m_sRedoTransRead = sVal;
     AssignValInt( sVal, &m_nRedoTransRead );
  } else if( sProp == PROP_REDOD ) {
    m_sRedoTransDone = sVal;
    AssignValInt( sVal, &m_nRedoTransDone );
  }
}

void DBMCli_BackupResult :: AssignValInt ( const DBMCli_String & sVal,
                                           SAPDB_Int4 * pVal )
{
  if( !sVal.IsEmpty( ) ) {
    *pVal = atoi( sVal );
  }
}

void DBMCli_BackupResult :: AssignValBool ( const DBMCli_String & sVal,
                                            SAPDB_Bool * pVal )
{
  DBMCli_String sValUpper = sVal;

  sValUpper.MakeUpper( ); 

  if( sValUpper == DBMCLI_VAL_TRUE ) {
    *pVal = true;
  } else {
    *pVal = false;
  }
}

/*    
  =============================================================================
  class: DBMCli_Backup
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_Backup :: DBMCli_Backup
  -----------------------------------------------------------------------------
*/
DBMCli_Backup :: DBMCli_Backup ( )
{
  m_nIgnoreCount    = 0;
  m_bRunning        = false;
  m_bAnswerReceived = false;
} 

/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_Backup :: ~DBMCli_Backup
  -----------------------------------------------------------------------------
*/
DBMCli_Backup :: ~DBMCli_Backup ( )
{
} 

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Backup :: ReplyAvailable
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Backup :: ReplyAvailable ( SAPDB_Bool & bAvailable, Msg_List & oMsgList )
{
  SAPDB_Bool        bRC     = true;
  DBMCli_Database & oDB     = GetDatabase();
  DBMCli_Result   & oResult = oDB.GetResult( ); 
  DBMCli_String     sCmd( DBMCLI_CMD_BACKUP_REPLY_AVAIL );

  bAvailable = false;
  if( oDB.Execute( sCmd, oMsgList ) ) {
    bAvailable = true;
  } else {
    if (oResult.ReturnCode() != DBMCLI_SRV_RC_NOREPLY) {
      bRC = false;
    } else {
      oMsgList.ClearMessageList();
    } // end if
  } // end if

  return bRC;
} // end DBMCli_Backup :: ReplyAvailable

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Backup :: ReplyReceive
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Backup :: ReplyReceive ( Msg_List & oMsgList )
{
  SAPDB_Bool             bRC     = false;
  DBMCli_Database      & oDB     = GetDatabase();
  DBMCli_Result        & oResult = oDB.GetResult( ); 
  DBMCli_String          sCmd( DBMCLI_CMD_BACKUP_REPLY_RECEIVE );
  Msg_List   oRelease;

  if( oDB.Execute( sCmd, oMsgList ) ) {
    m_oResult.SetByResultBuf( oResult ); 
    bRC = true;
    if (!m_oResult.IsFull()) {
      oDB.UTLRelease( oRelease ); 
      m_bRunning = false;
    } // end if
    m_bAnswerReceived = true;
  } else {
    oDB.UTLRelease( oRelease ); 
    m_bRunning = false;
  } // end if
  return bRC;
} // end DBMCli_Backup :: ReplyReceive

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Backup :: StartRequest
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Backup :: StartRequest ( const DBMCli_String  & sMediumName,
                                           const DBMCli_String  & sBackupFor,
                                           const DBMCli_String  & sBackupType,
                                           const DBMCli_String  & sMediumType,
                                           Msg_List & oMsgList )
{
  SAPDB_Bool             bRC = false;
  DBMCli_Database      & oDB = GetDatabase();
  Msg_List   oRelease;

  // save backup data
  m_sMediumName = sMediumName;
  m_sMediumType = sMediumType;
  m_sBackupType = sBackupType;

  DBMCli_String sCmd( DBMCLI_CMD_BACKUP_START_REQ );
  sCmd += " ";
  sCmd += sMediumName;
  sCmd += " ";
  sCmd += sBackupFor;
  sCmd += " ";
  sCmd += sBackupType;
  sCmd += " ";

  m_oResult.Clear();

  oDB.UTLRelease( oRelease );
  if( oDB.UTLConnect( oMsgList ) ) {
    if( oDB.Execute( sCmd, oMsgList ) ) {
      bRC        = true;
      m_bRunning = true;
      m_bAnswerReceived = false;
    } else {
      oDB.UTLRelease( oRelease ); 
      m_bRunning = false;
    } // end if
  } // end if

  return bRC;
} // end DBMCli_Backup :: StartRequest

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Backup :: ReplaceRequest
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Backup :: ReplaceRequest ( const DBMCli_String & sMedium,
                                             const DBMCli_String & sLocation,
                                             Msg_List & oMsgList )
{
  SAPDB_Bool             bRC = false;
  DBMCli_Database      & oDB = GetDatabase();
  Msg_List   oRelease;

  DBMCli_String sCmd( DBMCLI_CMD_BACKUP_REPLACE_REQ );
  sCmd += " \"";
  sCmd += sMedium;
  sCmd += "\" \"";
  sCmd += sLocation;
  sCmd += "\"";

  if ( oDB.Execute( sCmd, oMsgList ) ) {
    bRC = true;
    m_bAnswerReceived = false;
  } else {
    oDB.UTLRelease( oRelease ); 
    m_bRunning = false;
  } // end if

  return bRC;
} // end DBMCli_Backup :: ReplaceRequest

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Backup :: IgnoreRequest
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Backup :: IgnoreRequest ( Msg_List & oMsgList )
{
  SAPDB_Bool             bRC = false;
  DBMCli_Database      & oDB = GetDatabase();
  Msg_List   oRelease;

  DBMCli_String sCmd( DBMCLI_CMD_BACKUP_IGNORE_REQ );

  if ( oDB.Execute( sCmd, oMsgList ) ) {
    bRC = true;
    m_bAnswerReceived = false;
    ++m_nIgnoreCount;
  } else {
    oDB.UTLRelease( oRelease ); 
    m_bRunning = false;
  } // end if

  return bRC;
} // end DBMCli_Backup :: IgnoreRequest

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Backup :: CancelRequest
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Backup :: CancelRequest ( Msg_List & oMsgList )
{
  SAPDB_Bool             bRC = false;
  DBMCli_Database      & oDB = GetDatabase();
  Msg_List   oRelease;

  DBMCli_String sCmd( DBMCLI_CMD_BACKUP_CANCEL_REQ );

  if ( oDB.Execute( sCmd, oMsgList ) ) {
    bRC = true;
    m_bAnswerReceived = false;
  } else {
    oDB.UTLRelease( oRelease ); 
    m_bRunning = false;
  } // end if

  return bRC;
} // DBMCli_Backup :: CancelRequest

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Backup :: Cancel
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Backup :: Cancel ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = true;

  bRC = CancelRequest(oMsgList);
  if (bRC) {
    bRC = ReplyReceive(oMsgList);
  } // end if

  return bRC;
} // DBMCli_Backup :: Cancel

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Backup :: State
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Backup :: State ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();
  DBMCli_Result & oResult = oDB.GetResult( ); 

  DBMCli_String sCmd( DBMCLI_CMD_BACKUP_STATE );

  if ( oDB.Execute( sCmd, oMsgList ) ) {
    DBMCli_BackupResult   oLocalResult;
    oLocalResult.SetByResultBuf( oResult ); 
    if (oLocalResult.TransferredPages() != 0) {
      m_oResult = oLocalResult; 
    } // end if
    bRC = true;
  } // end if

  return bRC;
} // DBMCli_Backup :: State
