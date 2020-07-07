/*!
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateBackup.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
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
#include "DBM/Web/DBMWeb_TemplateBackup.hpp"
#include "DBM/Cli/DBMCli_DateTime.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define TEMPLATE_NAME          "DBMBackup.htm"

#define OBJ_GLBVARS            "GlobalVars*"
#define OBJ_GBAKTYPE           "GlbBackupType"
#define OBJ_GMEDTYPE           "GlbMediumType"
#define OBJ_GMEDNAME           "GlbMediumName"

#define OBJ_TITLE              "GroupBoxTitle"

#define OBJ_REFRESH            "Refresh"

//// template mode
#define OBJ_MEDIA              "Media"
#define OBJ_AUTOLOGOFF         "AutologOff"
#define OBJ_SELECT             "Select"
#define OBJ_REPLACE            "Replace"
#define OBJ_READY              "Ready"

#define OBJ_TABSNG             "TabSingle"
#define OBJ_TABPAR             "TabParallel"
#define OBJ_TABSNGLOG          "TabSingleLog"

#define OBJ_TABLESNG           "TableSingle"
#define OBJ_TABLEPAR           "TableParallel"

#define OBJ_ROWPAIR            "RowPair"

#define OBJ_EVENROW            "EvenRow*"
#define OBJ_ODDROW             "OddRow*"

#define OBJ_NAME               "Name"
#define OBJ_LOC                "Location"
#define OBJ_DEVTYPE            "DeviceType"
#define OBJ_BAKTYPE            "BackupType"
#define OBJ_MOD                "Modified"
#define OBJ_SIZE               "Size"
#define OBJ_OVW                "Overwrite"

#define OBJ_BTNADDSNG          "ButtonAddSingle"
#define OBJ_BTNADDPAR          "ButtonAddParallel"

#define OBJ_MEDIUM             "Medium*"

#define OBJ_RESULT             "Result*"
#define OBJ_LBL                "Label"
#define OBJ_BEG                "Beginning"
#define OBJ_MED                "MediaName"
#define OBJ_ISC                "IsConsistent"
#define OBJ_PGS                "Pages"
#define OBJ_VOL                "Volumes"
#define OBJ_LPG                "LogPage"
#define OBJ_LSP                "LastSavepoint"

#define OBJ_NEWLOC             "NewLocation"
#define OBJ_TRANS              "Transferred"
#define OBJ_LEFT               "Left"

#define OBJ_MEDNORMAL          "MediaNormal"
#define OBJ_MEDAUTOLOG         "MediaAutolog"

#define OBJ_SELNORMAL          "SelectNormal"
#define OBJ_SELNORMAL74        "SelectNormal74"
#define OBJ_SELAUTOLOG         "SelectAutolog"

#define OBJ_SELBTNSTART        "SelectBtnStart"
#define OBJ_SELBTNON           "SelectBtnAutologOn"

#define OBJ_STATEREFRESH       "StateRefresh"
#define OBJ_STATE              "State*"
#define OBJ_PROGRESSVALUE      "ProgressValue"
#define OBJ_PROGRESSBAR        "ProgressBar"

#define OBJ_BTNIGNORE          "BtnIgnore"

#define OBJ_OBJRUNNING         "Running"

#define BACKUPTYPETITLE_DATA     "Complete Data"
#define BACKUPTYPETITLE_PAGES    "Incremental Data"
#define BACKUPTYPETITLE_LOG      "Log"
#define BACKUPTYPETITLE_AUTO     "Auto Log"

/*
  =============================================================================
  class:        DBMWeb_TemplateBackup
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMWeb_TemplateBackup:: DBMWeb_TemplateBackup
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateBackup :: DBMWeb_TemplateBackup ( sapdbwa_WebAgent & wa,
                                                 DBMCli_Backup & oBackup,
                                                 DBMCli_Media & oMedia,
                                                 DBMWeb_TemplBackup_Mode nMode,
                                                 DBMCli_BackupType & oBackupType,
                                                 DBMCli_MediumType & oMediumType,
                                                 const DBMCli_String & sMediumName )
                        : Tools_Template( wa, _Tools_UTF8Ptr( TEMPLATE_NAME )),
                            m_oBackup( oBackup ),
                            m_oMedia( oMedia ),
                            m_nMode( nMode ),
                            m_oBackupType( oBackupType ),
                            m_oMediumType( oMediumType ),
                            m_sMediumName( sMediumName )
{
  m_nMedium = 0;
  m_nMediumIndex = -1;
} 

/*
  -----------------------------------------------------------------------------
  public destructor DBMWeb_TemplateBackup :: ~DBMWeb_TemplateBackup
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateBackup :: ~DBMWeb_TemplateBackup (  )
{
} 

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateBackup :: initTemplate
  -----------------------------------------------------------------------------
*/
bool DBMWeb_TemplateBackup :: initTemplate ( )
{
  m_nMediumIndex = -1;
  if( !m_sMediumName.IsEmpty( ) ) {
    if( m_oMediumType.Value( ) == DBMCLI_MEDIUMTYPE_SINGLE ) {
      m_nMediumIndex = m_oMedia.IndexByName( m_sMediumName );
    } else {
      m_nMediumIndex = m_oMedia.IndexByNameParallel( m_sMediumName );
    }
  }
  return true;
}

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateBackup :: askForWriteCount
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateBackup :: askForWriteCount ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  if (szName.Compare(OBJ_TITLE) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_GLBVARS) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_OBJRUNNING) == 0) {
    nReturn = (m_nMode == DBMWEB_TEMPLBACKUP_MODE_SHOWSTATE) ? 1 : 0;
  } else {
    switch( m_nMode ) {
      case DBMWEB_TEMPLBACKUP_MODE_MEDIA:
        nReturn = askForWriteCountMedia( szName );
        break;
      case DBMWEB_TEMPLBACKUP_MODE_AUTOLOGOFF:
        nReturn = askForWriteCountAutologOff( szName );
        break;
      case DBMWEB_TEMPLBACKUP_MODE_SELECT:
        nReturn = askForWriteCountSelect( szName );
        break;
      case DBMWEB_TEMPLBACKUP_MODE_REPLACE:
        nReturn = askForWriteCountReplace( szName );
        break;
      case DBMWEB_TEMPLBACKUP_MODE_READY:
        nReturn = askForWriteCountReady( szName );
        break;
      case DBMWEB_TEMPLBACKUP_MODE_STATE:
        nReturn = askForWriteCountState( szName );
        break;
    } // end switch
  } // end if
  return nReturn;
} // DBMWeb_TemplateBackup :: askForWriteCount
    
/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateBackup :: askForWriteCountMedia
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateBackup :: askForWriteCountMedia ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  if (szName.Compare(OBJ_REFRESH) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_MEDIA) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_MEDNORMAL) == 0) {
    nReturn = (m_oBackupType.Value( ) != DBMCLI_BACKUPTYPE_AUTO) ? 1 : 0;  
  } else if (szName.Compare(OBJ_MEDAUTOLOG) == 0) {
    nReturn = (m_oBackupType.Value( ) == DBMCLI_BACKUPTYPE_AUTO) ? 1 : 0;  
  } else if (szName.Compare(OBJ_TABSNG) == 0) {
    nReturn = (m_oMediumType.Value( ) == DBMCLI_MEDIUMTYPE_SINGLE      && 
               !( m_oBackupType.Value( ) == DBMCLI_BACKUPTYPE_LOG  ||
                  m_oBackupType.Value( ) == DBMCLI_BACKUPTYPE_AUTO    )    ) ? 1 : 0;
  } else if (szName.Compare(OBJ_TABPAR) == 0) {
    nReturn = (m_oMediumType.Value( ) == DBMCLI_MEDIUMTYPE_PARALLEL) ? 1 : 0;
  } else if (szName.Compare(OBJ_TABSNGLOG) == 0) {
    nReturn = (m_oMediumType.Value( ) == DBMCLI_MEDIUMTYPE_SINGLE      && 
               ( m_oBackupType.Value( ) == DBMCLI_BACKUPTYPE_LOG  ||
                 m_oBackupType.Value( ) == DBMCLI_BACKUPTYPE_AUTO    )    ) ? 1 : 0;
  } else if (szName.Compare(OBJ_TABLESNG) == 0) {
    nReturn = (m_oMediumType.Value( ) == DBMCLI_MEDIUMTYPE_SINGLE) ? 1 : 0;
  } else if (szName.Compare(OBJ_TABLEPAR) == 0) {
    nReturn = (m_oMediumType.Value( ) == DBMCLI_MEDIUMTYPE_PARALLEL) ? 1 : 0;
  } else if (szName.Compare(OBJ_ROWPAIR) == 0) {
    if (m_oMediumType.Value( ) == DBMCLI_MEDIUMTYPE_SINGLE) {
      nReturn = (m_nMedium < m_oMedia.MediumArray( ).GetSize( )) ? -1 : 0; 
    } else {
      nReturn = (m_nMedium < m_oMedia.MediumParallelArray( ).GetSize( )) ? -1 : 0; 
    }
  } else if ( (szName.Compare(OBJ_EVENROW) == 0) || 
              (szName.Compare(OBJ_ODDROW) == 0)     ) {

    nReturn = (FindNextMedium( ) == true) ? 1 : 0;
  } else if (szName.Compare(OBJ_BTNADDSNG) == 0) {
    nReturn = (m_oMediumType.Value( ) == DBMCLI_MEDIUMTYPE_SINGLE) ? 1 : 0;
  } else if (szName.Compare(OBJ_BTNADDPAR) == 0) {
    nReturn = (m_oMediumType.Value( ) == DBMCLI_MEDIUMTYPE_PARALLEL) ? 1 : 0;
  }
  return nReturn;
} // end DBMWeb_TemplateBackup :: askForWriteCountMedia 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateBackup :: askForWriteCountAutologOff
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateBackup :: askForWriteCountAutologOff ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  if (szName.Compare(OBJ_REFRESH) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_AUTOLOGOFF) == 0) {
    nReturn = 1;
  }
  return nReturn;
}  // end DBMWeb_TemplateBackup :: askForWriteCountAutologOff

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateBackup :: askForWriteCountSelect
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateBackup :: askForWriteCountSelect ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;
    
  if (szName.Compare(OBJ_SELECT) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_MEDIUM) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_SELNORMAL) == 0) {
    // VERSIONCHECK
    if ((m_oBackupType.Value( ) != DBMCLI_BACKUPTYPE_AUTO) &&
        (m_oBackup.GetDatabase().NodeInfo().Version().GetNumber() < 0x7040000)) {
      nReturn = 1;
    } else {
      nReturn = 0;
    }
  } else if (szName.Compare(OBJ_SELNORMAL74) == 0) {
    // VERSIONCHECK
    if ((m_oBackupType.Value( ) != DBMCLI_BACKUPTYPE_AUTO) &&
        (m_oBackup.GetDatabase().NodeInfo().Version().GetNumber() >= 0x7040000)) {
      nReturn = 1;
    } else {
      nReturn = 0;
    }
  } else if (szName.Compare(OBJ_SELAUTOLOG) == 0) {
    nReturn = ( m_oBackupType.Value( ) == DBMCLI_BACKUPTYPE_AUTO ) ? 1 : 0;
  } else if (szName.Compare(OBJ_SELBTNSTART) == 0) {
    nReturn = ( m_oBackupType.Value( ) != DBMCLI_BACKUPTYPE_AUTO ) ? 1 : 0;
  } else if (szName.Compare(OBJ_SELBTNON) == 0) {
    nReturn = ( m_oBackupType.Value( ) == DBMCLI_BACKUPTYPE_AUTO ) ? 1 : 0;
  }
  return nReturn;
}  // end DBMWeb_TemplateBackup :: askForWriteCountSelect

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateBackup :: askForWriteCountReplace
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateBackup :: askForWriteCountReplace ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;
    
  if (szName.Compare(OBJ_REPLACE) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_MEDIUM) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_NEWLOC) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_BTNIGNORE) == 0) {
    if ((m_oMediumType.Value() == DBMCLI_MEDIUMTYPE_PARALLEL) && (m_nMediumIndex >= 0)) {
      DBMCli_MediumParallel oMedium = m_oMedia.MediumParallelArray( ).GetAt( m_nMediumIndex);
      // find medium
      if (m_oBackup.IgnoreCount() < (oMedium.MediaCount() - 1)) {
        nReturn = 1;
      } // end if
    } // end if
  } // end if
  return nReturn;
}  // end DBMWeb_TemplateBackup :: askForWriteCountReplace

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateBackup :: askForWriteCountReady
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateBackup :: askForWriteCountReady ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;
    
  if (szName.Compare(OBJ_READY) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_RESULT) == 0) {
    nReturn = 1;
  }
  return nReturn;
} // end DBMWeb_TemplateBackup :: askForWriteCountReady

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateBackup :: askForWriteCountState
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateBackup :: askForWriteCountState ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;
    
  if (szName.Compare(OBJ_STATE) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_STATEREFRESH) == 0) {
    nReturn = 1;
  } // end if

  return nReturn;
} // end DBMWeb_TemplateBackup :: askForWriteCountState

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateBackup :: askForValue
  -----------------------------------------------------------------------------
*/
Tools_DynamicUTF8String DBMWeb_TemplateBackup :: askForValue ( const Tools_DynamicUTF8String & szName )
{
  const SAPDB_Char * pReturn = NULL;
  DBMCli_String      sValue;

  if (szName.Compare(OBJ_TITLE) == 0) {
    switch( m_oBackupType.Value( ) ) {
      case DBMCLI_BACKUPTYPE_DATA:
        sValue = BACKUPTYPETITLE_DATA;
        break;
      case DBMCLI_BACKUPTYPE_PAGES:
        sValue = BACKUPTYPETITLE_PAGES;
        break;
      case DBMCLI_BACKUPTYPE_LOG:
        sValue = BACKUPTYPETITLE_LOG;
        break;
      case DBMCLI_BACKUPTYPE_AUTO:
        sValue = BACKUPTYPETITLE_AUTO;
        break;
      default:
        sValue = "";
        break;
    }
  } else if (szName.Compare(OBJ_GBAKTYPE) == 0) {
    sValue = m_oBackupType.Name( );
  } else if (szName.Compare(OBJ_GMEDTYPE) == 0) {
    sValue = m_oMediumType.Name( );
  } else if (szName.Compare(OBJ_GMEDNAME) == 0) {
    sValue = m_sMediumName;
  } else {
    switch( m_nMode ) {
      case DBMWEB_TEMPLBACKUP_MODE_MEDIA:
        askForValueMedia( szName, sValue );
        break;
      case DBMWEB_TEMPLBACKUP_MODE_SELECT:
        askForValueSelect( szName, sValue );
        break;
      case DBMWEB_TEMPLBACKUP_MODE_REPLACE:
        askForValueReplace( szName, sValue );
        break;
      case DBMWEB_TEMPLBACKUP_MODE_READY:
        askForValueReady( szName, sValue );
        break;
      case DBMWEB_TEMPLBACKUP_MODE_STATE:
        askForValueState( szName, sValue );
        break;
    } // end switch
  } // end if

  pReturn = sValue;

  return _Tools_UTF8Ptr(pReturn);
} // end DBMWeb_TemplateBackup :: askForValue

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateBackup :: askForValueMedia
  -----------------------------------------------------------------------------
*/
void DBMWeb_TemplateBackup :: askForValueMedia ( const Tools_DynamicUTF8String & szName,
                                                 DBMCli_String    & sValue )
{
  sValue.Empty( );
  if (m_oMediumType.Value( ) == DBMCLI_MEDIUMTYPE_SINGLE) {
    DBMCli_MediumArray & aMedium = m_oMedia.MediumArray( );
    if (szName.Compare(OBJ_NAME) == 0) {
      sValue = aMedium[m_nMedium].Name( );
    } else if (szName.Compare(OBJ_LOC) == 0) {
      sValue = aMedium[m_nMedium].Location( );
    } else if (szName.Compare(OBJ_DEVTYPE) == 0) {
      sValue = aMedium[m_nMedium].DeviceType( );
    } else if (szName.Compare(OBJ_BAKTYPE) == 0) {
      sValue = aMedium[m_nMedium].BackupType( );
    } else if (szName.Compare(OBJ_MOD) == 0) {
      sValue = aMedium[m_nMedium].Modified( );
    } else if (szName.Compare(OBJ_SIZE) == 0) {
      if(aMedium[m_nMedium].Pages( ) == "0") {
        sValue = "&nbsp;";
      } else {
        sValue = aMedium[m_nMedium].Pages( );
      }
    } else if (szName.Compare(OBJ_OVW) == 0) {
      sValue = aMedium[m_nMedium].Overwrite( );
      m_nMedium++;
    }
  } else {
    DBMCli_MediumParallelArray & aMediumParallel = m_oMedia.MediumParallelArray( );
    if (szName.Compare(OBJ_NAME) == 0) {
      sValue = aMediumParallel[m_nMedium].Name( );
    } else if (szName.Compare(OBJ_DEVTYPE) == 0) {
      sValue = aMediumParallel[m_nMedium].DeviceType( );
    } else if (szName.Compare(OBJ_BAKTYPE) == 0) {
      sValue = aMediumParallel[m_nMedium].BackupType( );
    } else if (szName.Compare(OBJ_MOD) == 0) {
      sValue = aMediumParallel[m_nMedium].Modified( );
    } else if (szName.Compare(OBJ_OVW) == 0) {
      sValue = aMediumParallel[m_nMedium].Overwrite( );
      m_nMedium++;
    }
  }
} // end DBMWeb_TemplateBackup :: askForValueMedia

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateBackup :: askForValueSelect
  -----------------------------------------------------------------------------
*/
void DBMWeb_TemplateBackup :: askForValueSelect ( const Tools_DynamicUTF8String & szName,
                                                  DBMCli_String    & sValue )
{
  sValue.Empty( );
  if (m_oMediumType.Value( ) == DBMCLI_MEDIUMTYPE_SINGLE) {
    if( m_nMediumIndex >= 0 ) {
      DBMCli_Medium oMedium = m_oMedia.MediumArray( ).GetAt( m_nMediumIndex );

      if (szName.Compare(OBJ_NAME) == 0) {
        sValue = oMedium.Name( );
      } else if (szName.Compare(OBJ_LOC) == 0) {
        sValue = oMedium.Location( );
      } else if (szName.Compare(OBJ_DEVTYPE) == 0) {
        sValue = oMedium.DeviceType( );
      } else if (szName.Compare(OBJ_BAKTYPE) == 0) {
        sValue = oMedium.BackupType( );
      } else if (szName.Compare(OBJ_MOD) == 0) {
        sValue = oMedium.Modified( );
      } else if (szName.Compare(OBJ_SIZE) == 0) {
        sValue = oMedium.Pages( );
      } else if (szName.Compare(OBJ_OVW) == 0) {
        sValue = oMedium.Overwrite( );
      }

    } else {
      if (szName.Compare(OBJ_NAME) == 0) {
        sValue = m_sMediumName;
      }
    }
  } else {
    if( m_nMediumIndex >= 0 ) {
      DBMCli_MediumParallel oMedium = m_oMedia.MediumParallelArray( ).GetAt( m_nMediumIndex );
      if (szName.Compare(OBJ_NAME) == 0) {
        sValue = oMedium.Name( );
      } else if (szName.Compare(OBJ_DEVTYPE) == 0) {
        sValue = oMedium.DeviceType( );
      } else if (szName.Compare(OBJ_BAKTYPE) == 0) {
        sValue = oMedium.BackupType( );
      } if (szName.Compare(OBJ_MOD) == 0) {
        sValue = oMedium.Modified( );
      } else if (szName.Compare(OBJ_OVW) == 0) {
        sValue = oMedium.Overwrite( );
      }
    } else {
      if (szName.Compare(OBJ_NAME) == 0) {
        sValue = m_sMediumName;
      }
    }
  }
} // end DBMWeb_TemplateBackup :: askForValueSelect

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateBackup :: askForValueReplace
  -----------------------------------------------------------------------------
*/
void DBMWeb_TemplateBackup :: askForValueReplace ( const Tools_DynamicUTF8String & szName,
                                                   DBMCli_String    & sValue )
{
  DBMCli_BackupResult & oResult = m_oBackup.Result( );

  sValue.Empty( );

  if (szName.Compare(OBJ_NAME) == 0) {
    if ((m_oMediumType.Value() == DBMCLI_MEDIUMTYPE_PARALLEL) && (m_nMediumIndex >= 0)) {
      DBMCli_MediumParallel oParallel = m_oMedia.MediumParallelArray( ).GetAt( m_nMediumIndex);
      SAPDB_Int             nIndex    = oParallel.IndexByLocation(oResult.LocationStr( ));
      sValue = oResult.MediaNameStr( );
      sValue = sValue + "\\";
      sValue = sValue + oParallel.MediumArray()[nIndex].Name();
    } else {
      sValue = oResult.MediaNameStr( );
    } // end if
  } else if (szName.Compare(OBJ_LOC) == 0) {
    sValue = oResult.LocationStr( );
  } else if (szName.Compare(OBJ_TRANS) == 0) {
    sValue = oResult.TransferredPagesStr( );
  } else if (szName.Compare(OBJ_LEFT) == 0) {
    sValue = oResult.LeftPagesStr( );
  } else if (szName.Compare(OBJ_NEWLOC) == 0) {
    sValue = oResult.LocationStr( );
  }
} // end DBMWeb_TemplateBackup :: askForValueReplace

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateBackup :: askForValueReady
  -----------------------------------------------------------------------------
*/
void DBMWeb_TemplateBackup :: askForValueReady ( const Tools_DynamicUTF8String & szName,
                                                 DBMCli_String    & sValue )
{
  DBMCli_BackupResult & oResult = m_oBackup.Result( );
  
  sValue.Empty( );

  if (szName.Compare(OBJ_LBL) == 0) {
    sValue = oResult.LabelStr( );
  } else if (szName.Compare(OBJ_BEG) == 0) {
    sValue = oResult.BeginningStr( );
  } else if (szName.Compare(OBJ_MED) == 0) {
    sValue = oResult.MediaNameStr( );
  } else if (szName.Compare(OBJ_ISC) == 0) {
    sValue = oResult.IsConsistentStr( );
  } else if (szName.Compare(OBJ_PGS) == 0) {
    sValue = oResult.TransferredPagesStr( );
  } else if (szName.Compare(OBJ_VOL) == 0) {
    sValue = oResult.VolumesStr( );
  } else if (szName.Compare(OBJ_LPG) == 0) {
    sValue = oResult.FirstLogPageStr( );
  } else if (szName.Compare(OBJ_LSP) == 0) {
    sValue = oResult.DBStamp1Str( );
  }
} // end DBMWeb_TemplateBackup :: askForValueReady

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateBackup :: askForValueState
  -----------------------------------------------------------------------------
*/
void DBMWeb_TemplateBackup :: askForValueState ( const Tools_DynamicUTF8String & szName,
                                                 DBMCli_String    & sValue )
{
 
  sValue.Empty( );
  if (szName.Compare(OBJ_STATEREFRESH) == 0) {

    // refresh rate
    Tools_DynamicUTF8String strResult;
    SAPDB_Int               nResult;

    DBMCli_DateTime oBegin(m_oBackup.Result( ).BeginningStr(), DBMCLI_DT_INT_DATETIME_FMT);
    DBMCli_DateTime oNow = DBMCli_DateTime::Now();

    if (oBegin.IsNull()) {
      sValue = "1";
    } else {
      nResult = (oNow - oBegin) / 12;
      nResult = (nResult <   1) ?   1 : nResult;
      nResult = (nResult > 300) ? 300 : nResult;

      strResult.ConvertFromInt(nResult);
      sValue    = strResult.StrPtr();
    } // end if

  } else if ( (szName.Compare(OBJ_PROGRESSVALUE) == 0) ||
              (szName.Compare(OBJ_PROGRESSBAR)   == 0)     ){

    SAPDB_Int4 nTrans = m_oBackup.Result( ).TransferredPages();
    SAPDB_Int4 nLeft  = m_oBackup.Result( ).LeftPages();

    Tools_DynamicUTF8String strResult;
    
    if (nLeft == 0  || nTrans == 0) {
      sValue = "0";
    } else {
      strResult.ConvertFromInt((nTrans * 100 ) / (nTrans + nLeft));
      sValue = strResult.StrPtr();
    } // end if

  } // end if

} // end DBMWeb_TemplateBackup :: askForValueState

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateBackup :: askForContinue
  -----------------------------------------------------------------------------
*/
bool DBMWeb_TemplateBackup :: askForContinue ( const Tools_DynamicUTF8String & szName )
{
  bool bRC = false;

  switch( m_nMode ) {
    case DBMWEB_TEMPLBACKUP_MODE_MEDIA:
      bRC = askForContinueMedia( szName );
      break;
    default:
      bRC = false;
      break;
  }

  return bRC;
} // end DBMWeb_TemplateBackup :: askForContinue

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateBackup :: askForContinueMedia
  -----------------------------------------------------------------------------
*/
bool DBMWeb_TemplateBackup :: askForContinueMedia ( const Tools_DynamicUTF8String & szName )
{
  bool bRC = false;
  
  if (m_oMediumType.Value( ) == DBMCLI_MEDIUMTYPE_SINGLE) {
    DBMCli_MediumArray & aMedium = m_oMedia.MediumArray( );

    if( szName.Compare(OBJ_ROWPAIR ) == 0 ) {

      bRC = ( m_nMedium < aMedium.GetSize( ) );
    }
  } else {
    DBMCli_MediumParallelArray & aMedium = m_oMedia.MediumParallelArray( );

    if( szName.Compare(OBJ_ROWPAIR ) == 0 ) {

      bRC = ( m_nMedium < aMedium.GetSize( ) );
    }
  }

  return bRC;
} // end DBMWeb_TemplateBackup :: askForContinueMedia

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateBackup :: FindNextMedium
  -----------------------------------------------------------------------------
*/
bool DBMWeb_TemplateBackup :: FindNextMedium ( )
{
  bool bFound = false;

  if (m_oMediumType.Value( ) == DBMCLI_MEDIUMTYPE_SINGLE) {
    DBMCli_MediumArray & aMedium = m_oMedia.MediumArray( );

    DBMCli_BackupType oBTLog( DBMCLI_BACKUPTYPE_LOG );
    DBMCli_BackupType oBTAuto( DBMCLI_BACKUPTYPE_AUTO );

    while( ( m_nMedium < aMedium.GetSize( ) ) && ( bFound == false ) ) {
      if( m_oBackupType.Value( ) != DBMCLI_BACKUPTYPE_AUTO ) {
        if( aMedium[m_nMedium].BackupType( ) == m_oBackupType.Name( ) ) {
          bFound = true;
        } else {
          m_nMedium++;
        }
      } else {
        if( aMedium[m_nMedium].BackupType( ) == oBTLog.Name( )  ||
            aMedium[m_nMedium].BackupType( ) == oBTAuto.Name( )    ) {
          bFound = true;
        } else {
          m_nMedium++;
        }
      }
    }
  } else {
    DBMCli_MediumParallelArray & aMedium = m_oMedia.MediumParallelArray( );

    while( ( m_nMedium < aMedium.GetSize( ) ) && ( bFound == false ) ) {
      if( aMedium[m_nMedium].BackupType( ) == m_oBackupType.Name( ) ) {
        bFound = true;
      } else {
        m_nMedium++;
      }
    }
  }

  return bFound;
} // end DBMWeb_TemplateBackup :: FindNextMedium