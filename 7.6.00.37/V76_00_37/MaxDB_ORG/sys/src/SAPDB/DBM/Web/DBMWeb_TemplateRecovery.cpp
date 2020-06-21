/*!
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateRecovery.cpp
  -----------------------------------------------------------------------------
  responsible:  Bernd Vorsprach Bernd.Vorsprach@sap.com
  special area: Database Manager WebAgent Extension
  description:  HTML Template for Recovery
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
#include "DBM/Web/DBMWeb_TemplateRecovery.hpp"
#include "DBM/Cli/DBMCli_DateTime.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define TEMPLATE_NAME          "DBMRecovery.htm"

#define OBJ_GROUPBOXTITLE      "GroupBoxTitle"
#define VAL_GROUPBOXTITLE      OBJ_GROUPBOXTITLE
#define OBJ_REFRESH            "Refresh"

#define OBJ_BTNBACK            "BtnBack"
#define OBJ_BTNSTART           "BtnStart"
#define OBJ_BTNOK              "BtnOK"
#define OBJ_BTNCONTINUE        "BtnContinue"
#define OBJ_BTNIGNORE          "BtnIgnore"
#define OBJ_BTNCANCEL          "BtnCancel"
#define OBJ_BTNRESTART         "BtnRestart"
#define OBJ_BTNCLOSE           "BtnClose"
#define OBJ_BTNADDSINGLE       "BtnAddSingle"
#define OBJ_BTNADDPARALLEL     "BtnAddParallel"

#define OBJ_GLBVARS            "GlobalVars*"
#define VAL_GLBRECOVERYMODE    "GlbRecoveryMode"
#define VAL_GLBRECOVERYTYPE    "GlbRecoveryType"
#define VAL_GLBMEDTYPE         "GlbMedType"
#define VAL_GLBUNTIL           "GlbUntil"
#define VAL_GLBCHECK           "GlbCheck"
#define VAL_ACTION             "Action"

#define OBJ_RECOVERYTYPE       "RecoveryType"

#define OBJ_RECOVERYMODENORM   "RecoveryModeNormal*"
#define OBJ_RECOVERYMODEINST   "RecoveryModeInstall"
#define VAL_RECOVERYMODE       "RecoveryMode" 

#define OBJ_RECOVERYLAST       "RecoveryLast"
#define OBJ_RECOVERYSPECIFIC   "RecoverySpecific"
#define OBJ_RECOVERYMEDIUM     "RecoveryMedium"
#define OBJ_RECOVERYCONT       "RecoveryContinue"

#define OBJ_RECOVERYLAST_CHK     "RecoveryLastChecked"
#define OBJ_RECOVERYSPECIFIC_CHK "RecoverySpecificChecked"
#define OBJ_RECOVERYMEDIUM_CHK   "RecoveryMediumChecked"
#define OBJ_RECOVERYCONT_CHK     "RecoveryContinueChecked"

#define OBJ_CHECKLAST            "CheckLast"
#define OBJ_CHECKSPECIFIC        "CheckSpecific"
#define OBJ_CHECKMEDIUM          "CheckMedium"

#define OBJ_CHECKLAST_CHK        "CheckLastChecked"
#define OBJ_CHECKSPECIFIC_CHK    "CheckSpecificChecked"
#define OBJ_CHECKMEDIUM_CHK      "CheckMediumChecked"

#define VAL_RECOVER            "RECOVER"
#define VAL_ACTIVATE           "ACTIVATE"
#define VAL_INSTALL            "INSTALL"

#define VAL_LAST               "LAST"
#define VAL_SPECIFIC           "SPECIFIC"
#define VAL_MEDIUM             "MEDIUM"
#define VAL_CONTIUNUE          "CONTINUE"

#define VAL_SINGLE             "SINGLE"
#define VAL_PARALLEL           "PARALLEL"

#define VAL_NOCHECK            "0"
#define VAL_CHECK              "1"

#define OBJ_RECOVERYUNTIL      "RecoveryUntil*"
#define VAL_CHECKED            "Checked"
#define VAL_UNTILDATE          "UntilDate"
#define VAL_UNTILTIME          "UntilTime"

#define OBJ_DATASAVELIST       "DataSaveList"
#define OBJ_MEDIALIST          "MediaList"
#define OBJ_RECOVERYLIST       "RecoveryList"
#define OBJ_RECSTATE           "RecoveryState"

#define OBJ_ROWPAIR            "RowPair"
#define OBJ_EVENROW            "EvenRow*"
#define OBJ_ODDROW             "OddRow*"

#define VAL_TYPE               "Type"
#define VAL_NEXTLOG            "NextLog"
#define VAL_INDEX              "Index"
#define VAL_KEY                "Key"
#define VAL_LABEL              "Label"
#define VAL_BEGINNING          "Beginning"
#define VAL_BAACTION           "BaAction"
#define VAL_MEDIANAME          "MediaName"
#define VAL_LOGREQUIRED        "LogRequired"
#define VAL_SIZEPAGES          "SizePages"
#define VAL_VOLUMES            "Volumes"
#define VAL_LOGPAGE            "LogPage"
#define VAL_FROMLOG            "FromLog"
#define VAL_TOLOG              "ToLog"
#define VAL_LASTSAVEPOINT      "LastSavepoint"
#define VAL_FIRSTCOMMIT        "FirstCommit"
#define VAL_LASTCOMMIT         "LastCommit"
#define VAL_IMAGE              "Image"
#define VAL_LOCATION           "Location"
#define VAL_DEVICETYPE         "DeviceType"
#define VAL_BACKUPTYPE         "BackupType"
#define VAL_MODIFIED           "Modified"
#define VAL_OVERWRITE          "Overwrite"
#define VAL_TRANSFERRED        "Transferred"

#define OBJ_ASKFORRESTART      "AskForRestart"
#define OBJ_ASKFORMEDIUM       "AskForMedium"
#define OBJ_ASKFORMEDIUMCHECK  "AskForMediumCheck"
#define OBJ_RECOVERISRUNNING   "RecoverIsRunning"
#define OBJ_CHECKISRUNNING     "CheckIsRunning"
#define OBJ_COMPLETE           "RecoverComplete"
#define OBJ_LOGEMPTY           "LogEmpty"
#define OBJ_RESTARTWITHLOG     "RestartWithLog"
#define OBJ_RESTARTWITHLOGUNT  "RestartWithLogUntil"
#define OBJ_NORESTART          "NoRestart"
#define OBJ_CHECKCOMPLETE      "CheckComplete"
#define OBJ_CHECKFULLCOMPLETE  "CheckFullComplete"
#define OBJ_NEXTDATAMEDIUM     "NextDataMedium"
#define OBJ_NEXTDATAMEDIUMCHECK "NextDataMediumCheck"
#define OBJ_FIRSTLOG           "FirstLog"
#define OBJ_FIRSTLOGCHECK      "FirstLogCheck"
#define OBJ_INVALIDLOGFILE     "InvalidLogFile"
#define OBJ_INVALIDDATAFILE    "InvalidDataFile"
#define OBJ_RECOVERCANCEL      "RecoverCancel"
#define OBJ_CHECKCANCEL        "CheckCancel"

#define OBJ_TABSINGLE          "TabSingle"
#define OBJ_TABPARALLEL        "TabParallel"
#define OBJ_TABLESINGLE        "TableSingle"
#define OBJ_TABLEPARALLEL      "TableParallel"

#define OBJ_MEDIUMDATA         "MediumData"
#define OBJ_ASKFORLOCATION     "AskForLocation*"
#define OBJ_SHOWLOCATION       "ShowLocation*"
#define OBJ_ASKFORVERSION      "AskForVersion*"

#define OBJ_STATEREFRESH       "StateRefresh"

#define VAL_ASKFORLOCATION     "AskForLocation"
#define VAL_SHOWLOCATION       "ShowLocation"
#define VAL_ASKFORVERSION      "AskForVersion"

#define HTML_SPACE             "&nbsp;"
#define HTML_CHECKED           "checked"
#define HTML_IMG_EMPTY         "1x1.gif"
#define HTML_IMG_ARROW         "b_arri.gif"
#define HTML_IMG_OK            "ok.gif"
#define HTML_IMG_CANCEL        "cancel.gif"
#define HTML_IMG_WAIT          "wait.gif"

/*
  =============================================================================
  class:        DBMWeb_TemplateRecovery
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMWeb_TemplateRecovery :: DBMWeb_TemplateRecovery
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateRecovery :: DBMWeb_TemplateRecovery ( sapdbwa_WebAgent & wa,
                                                     Mode               nMode,
                                                     const char       * pAction,
                                                     DBMCli_Database  * pDatabase )
                        : Tools_Template( wa, _Tools_UTF8Ptr( TEMPLATE_NAME )),
                           m_nMode     ( nMode ),
                           m_sAction   ( pAction ),
                           m_pDatabase ( pDatabase )
{
} // end DBMWeb_TemplateRecovery :: DBMWeb_TemplateRecovery

/*
  -----------------------------------------------------------------------------
  public destructor DBMWeb_TemplateRecovery :: ~DBMWeb_TemplateRecovery
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateRecovery :: ~DBMWeb_TemplateRecovery (  )
{
} // end DBMWeb_TemplateRecovery :: ~DBMWeb_TemplateRecovery

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateRecovery :: initTemplate
  -----------------------------------------------------------------------------
*/
bool DBMWeb_TemplateRecovery :: initTemplate ( )
{
  Msg_List oMsgList;

  switch (m_nMode) {
    case MODE_SELRECTYPE:
      // refresh restartinfo
      m_pDatabase->GetRestartInfo().Refresh();
      break;
    case MODE_SELDATASAVE:
      // read count of history items
      m_nCurrentItem = m_pDatabase->GetHistory().HistoryItemArray().GetSize();
      break;
    case MODE_SELMEDIUM:
      m_nCurrentItem = -1;
      break;
    case MODE_COMMITLIST:
      m_nCurrentItem = -1;
      break;
    case MODE_RECSTATE:
      m_nCurrentItem = -1;
      break;
  } // end switch

  if (m_sRecType.Empty()) {
    if ((m_pDatabase->GetRestartInfo().GetFirstLogPage() > m_pDatabase->GetRestartInfo().GetUsedLogPage()) &&
        !m_pDatabase->GetRestartInfo().Restartable()) {
      m_sRecType.Assign(VAL_CONTIUNUE);
    } else {
      m_sRecType.Assign(VAL_LAST);
    } // end if
  } // end if
  if (m_sMedType.Empty()) {
    m_sMedType.Assign(VAL_SINGLE);
  } // end if
  if (m_sCheck.Empty()) {
    m_sCheck.Assign(VAL_NOCHECK);
  } // end if

  return true;
} // end DBMWeb_TemplateRecovery :: initTemplate

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateRecovery :: askForWriteCount
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateRecovery :: askForWriteCount ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  if (szName.Compare(OBJ_GROUPBOXTITLE) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_GLBVARS) == 0) {
    nReturn = 1;
  } else {
    switch (m_nMode) {
      case MODE_SELRECTYPE:
        nReturn = askForWriteCountSelectType(szName);
        break;
      case MODE_SELDATASAVE:
        nReturn = askForWriteCountDataSaveList(szName);
        break;
      case MODE_SELMEDIUM:
        nReturn = askForWriteCountMediaList(szName);
        break;
      case MODE_COMMITLIST:
        nReturn = askForWriteCountRecoveryList(szName);
        break;
      case MODE_RECSTATE:
        nReturn = askForWriteCountRecoverState(szName);
        break;
    } // end switch
  } // end if

  return nReturn;
} // DBMWeb_TemplateRecovery :: askForWriteCount

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateRecovery :: askForWriteCountSelectType
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateRecovery :: askForWriteCountSelectType ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  if (szName.Compare(OBJ_REFRESH) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_BTNOK) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_RECOVERYTYPE) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_RECOVERYMODENORM) == 0) {
    if (m_pDatabase->NodeInfo().Version().GetNumber() >= 0x7040400) {
      if ((m_sRecMode.Compare(VAL_RECOVER) == 0)  ||
          (m_sRecMode.Compare(VAL_ACTIVATE) == 0)    ) {
        nReturn = 1;
      }
    }
  } else if (szName.Compare(OBJ_RECOVERYMODEINST) == 0) {
    if (m_pDatabase->NodeInfo().Version().GetNumber() >= 0x7040400) {
      if (m_sRecMode.Compare(VAL_INSTALL) == 0) {
        nReturn = 1;
      }
    }
  } else if (szName.Compare(OBJ_RECOVERYLAST) == 0) {
    if ((m_sRecType.Compare(VAL_LAST) != 0) && (m_sCheck.Compare(VAL_CHECK) != 0)) {
      nReturn = 1;
    } // end if
  } else if (szName.Compare(OBJ_RECOVERYLAST_CHK) == 0) {
    if ((m_sRecType.Compare(VAL_LAST) == 0) && (m_sCheck.Compare(VAL_CHECK) != 0)) {
      nReturn = 1;
    } // end if
  } else if (szName.Compare(OBJ_RECOVERYSPECIFIC) == 0) {
    if ((m_sRecType.Compare(VAL_SPECIFIC) != 0) && (m_sCheck.Compare(VAL_CHECK) != 0)) {
      nReturn = 1;
    } // end if
  } else if (szName.Compare(OBJ_RECOVERYSPECIFIC_CHK) == 0) {
    if ((m_sRecType.Compare(VAL_SPECIFIC) == 0) && (m_sCheck.Compare(VAL_CHECK) != 0)) {
      nReturn = 1;
    } // end if
  } else if (szName.Compare(OBJ_RECOVERYMEDIUM) == 0) {
    if ((m_sRecType.Compare(VAL_MEDIUM) != 0) && (m_sCheck.Compare(VAL_CHECK) != 0)) {
      nReturn = 1;
    } // end if
  } else if (szName.Compare(OBJ_RECOVERYMEDIUM_CHK) == 0) {
    if ((m_sRecType.Compare(VAL_MEDIUM) == 0) && (m_sCheck.Compare(VAL_CHECK) != 0)) {
      nReturn = 1;
    } // end if
  } else if (szName.Compare(OBJ_RECOVERYCONT) == 0) {
    if ((m_sRecType.Compare(VAL_CONTIUNUE) != 0) && (m_sCheck.Compare(VAL_CHECK) != 0) &&
        (m_pDatabase->GetRestartInfo().GetFirstLogPage() > m_pDatabase->GetRestartInfo().GetUsedLogPage()) &&
        !m_pDatabase->GetRestartInfo().Restartable()) {
      nReturn = 1;
    } // end if
  } else if (szName.Compare(OBJ_RECOVERYCONT_CHK) == 0) {
    if ((m_sRecType.Compare(VAL_CONTIUNUE) == 0) && (m_sCheck.Compare(VAL_CHECK) != 0) &&
        (m_pDatabase->GetRestartInfo().GetFirstLogPage() > m_pDatabase->GetRestartInfo().GetUsedLogPage()) &&
        !m_pDatabase->GetRestartInfo().Restartable()) {
      nReturn = 1;
    } // end if
  } else if (szName.Compare(OBJ_CHECKLAST) == 0) {
    if ((m_sRecType.Compare(VAL_LAST) != 0) && (m_sCheck.Compare(VAL_CHECK) == 0)) {
      nReturn = 1;
    } // end if
  } else if (szName.Compare(OBJ_CHECKLAST_CHK) == 0) {
    if ((m_sRecType.Compare(VAL_LAST) == 0) && (m_sCheck.Compare(VAL_CHECK) == 0)) {
      nReturn = 1;
    } // end if
  } else if (szName.Compare(OBJ_CHECKSPECIFIC) == 0) {
    if ((m_sRecType.Compare(VAL_SPECIFIC) != 0) && (m_sCheck.Compare(VAL_CHECK) == 0)) {
      nReturn = 1;
    } // end if
  } else if (szName.Compare(OBJ_CHECKSPECIFIC_CHK) == 0) {
    if ((m_sRecType.Compare(VAL_SPECIFIC) == 0) && (m_sCheck.Compare(VAL_CHECK) == 0)) {
      nReturn = 1;
    } // end if
  } else if (szName.Compare(OBJ_CHECKMEDIUM) == 0) {
    if ((m_sRecType.Compare(VAL_MEDIUM) != 0) && (m_sCheck.Compare(VAL_CHECK) == 0)) {
      nReturn = 1;
    } // end if
  } else if (szName.Compare(OBJ_CHECKMEDIUM_CHK) == 0) {
    if ((m_sRecType.Compare(VAL_MEDIUM) == 0) && (m_sCheck.Compare(VAL_CHECK) == 0)) {
      nReturn = 1;
    } // end if
  } else if (szName.Compare(OBJ_RECOVERYUNTIL) == 0) {
    nReturn = (m_sCheck.Compare(VAL_CHECK) == 0) ? 0 : 1;
  } // end if

  return nReturn;
} // DBMWeb_TemplateRecovery :: askForWriteCountSelectType

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateRecovery :: askForWriteCountDataSaveList
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateRecovery :: askForWriteCountDataSaveList ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  if (szName.Compare(OBJ_DATASAVELIST) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_BTNBACK) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_ROWPAIR) == 0) {
    nReturn = -1; // -> askForContinue
  } else if (szName.Compare(OBJ_EVENROW) == 0) {
    nReturn = (m_nCurrentItem >= 0) ? 1 : 0;
  } else if (szName.Compare(OBJ_ODDROW)  == 0) {
    // find a valid Item
    --m_nCurrentItem;
    while(m_nCurrentItem >= 0) {
      if (m_pDatabase->GetHistory().HistoryItemArray()[m_nCurrentItem].Action( ) != HISTORY_ACTION_LOST) {
        break;
      } // end if
      --m_nCurrentItem;
    } // end while
    nReturn = (m_nCurrentItem >= 0) ? 1 : 0;
  } // end if

  return nReturn;
} // DBMWeb_TemplateRecovery :: askForWriteCountDataSaveList

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateRecovery :: askForWriteCountMediaList
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateRecovery :: askForWriteCountMediaList ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  if (szName.Compare(OBJ_MEDIALIST) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_BTNBACK) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_BTNADDSINGLE) == 0) {
    nReturn = (m_sMedType.Compare(VAL_SINGLE)) == 0 ? 1 : 0;
  } else if (szName.Compare(OBJ_BTNADDPARALLEL) == 0) {
    nReturn = (m_sMedType.Compare(VAL_PARALLEL) == 0) ? 1 : 0;
  } // end if

  if (m_sMedType.Compare(VAL_SINGLE) == 0) {

    if (szName.Compare(OBJ_TABSINGLE) == 0) {
      nReturn = 1;
    } else if (szName.Compare(OBJ_TABLESINGLE) == 0) {
      nReturn = 1;
    } else if (szName.Compare(OBJ_ROWPAIR) == 0) {
      SAPDB_Int nSize = m_pDatabase->GetMedia( ).MediumArray( ).GetSize( );
      nReturn = (nSize / 2) + (nSize % 2);
    } else if (szName.Compare(OBJ_EVENROW) == 0) {
      ++m_nCurrentItem;
      nReturn = (m_nCurrentItem < m_pDatabase->GetMedia( ).MediumArray( ).GetSize( )) ? 1 : 0;
    } else if (szName.Compare(OBJ_ODDROW)  == 0) {
      ++m_nCurrentItem;
      nReturn = (m_nCurrentItem < m_pDatabase->GetMedia( ).MediumArray( ).GetSize( )) ? 1 : 0;
    } // end if

  } else if (m_sMedType.Compare(VAL_PARALLEL) == 0) {

    if (szName.Compare(OBJ_TABPARALLEL) == 0) {
      nReturn = 1;
    } else if (szName.Compare(OBJ_TABLEPARALLEL) == 0) {
      nReturn = 1;
    } else if (szName.Compare(OBJ_ROWPAIR) == 0) {
      SAPDB_Int nSize = m_pDatabase->GetMedia( ).MediumParallelArray( ).GetSize( );
      nReturn = (nSize / 2) + (nSize % 2);
    } else if (szName.Compare(OBJ_EVENROW) == 0) {
      ++m_nCurrentItem;
      nReturn = (m_nCurrentItem < m_pDatabase->GetMedia( ).MediumParallelArray( ).GetSize( )) ? 1 : 0;
    } else if (szName.Compare(OBJ_ODDROW)  == 0) {
      ++m_nCurrentItem;
      nReturn = (m_nCurrentItem < m_pDatabase->GetMedia( ).MediumParallelArray( ).GetSize( )) ? 1 : 0;
    } // end if

  } // end if

  return nReturn;
} // DBMWeb_TemplateRecovery :: askForWriteCountMediaList

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateRecovery :: askForWriteCountRecoveryList
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateRecovery :: askForWriteCountRecoveryList ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  if (szName.Compare(OBJ_RECOVERYLIST) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_BTNBACK) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_BTNOK) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_ROWPAIR) == 0) {
    nReturn = -1; // -> askForContinue
  } else if (szName.Compare(OBJ_EVENROW) == 0) {
    ++m_nCurrentItem;
    nReturn = (m_nCurrentItem < m_pDatabase->GetHistory().HistoryItemArray().GetSize()) ? 1 : 0;
  } else if (szName.Compare(OBJ_ODDROW)  == 0) {
    ++m_nCurrentItem;
    nReturn = (m_nCurrentItem < m_pDatabase->GetHistory().HistoryItemArray().GetSize()) ? 1 : 0;
  } // end if

  return nReturn;
} // DBMWeb_TemplateRecovery :: askForWriteCountRecoveryList

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateRecovery :: askForWriteCountRecoverState
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateRecovery :: askForWriteCountRecoverState ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  SAPDB_Int                    nCurrent     = m_pDatabase->GetRecover().CurrentItem();
  DBMCli_Recover::RecoverState nState       = m_pDatabase->GetRecover().GetRecoverState();
  SAPDB_Bool                   bCheck       = m_pDatabase->GetRecover().IsCheck();
  DBMCli_Recover::MediaInfo    nMediaInfo   = m_pDatabase->GetRecover().AskForMediaInfo();
  SAPDB_Bool                   bRestartable = m_pDatabase->GetRestartInfo().Restartable() && (m_pDatabase->GetRestartInfo().GetUsedLogPage() > 0);
  SAPDB_Int                    nMax         = m_pDatabase->GetRecover().RecoverItemArray().GetSize();
  SAPDB_Bool                   bIgnore      = m_pDatabase->GetRecover().IgnorePossible();
  DBMCli_DateTime              oUntil       = m_pDatabase->GetRecover().GetUntil();

  // Globals
  if (szName.Compare(OBJ_RECSTATE) == 0) {
    nReturn = 1;
  } else   if (szName.Compare(OBJ_STATEREFRESH) == 0) {
    nReturn = (nState == DBMCli_Recover::RecoverStateRunning) ? 1 : 0;
  } // end if

  // Messages
  if        (szName.Compare(OBJ_ASKFORRESTART      ) == 0) {
    if ( (nState == DBMCli_Recover::RecoverStateAsking) && bRestartable && !bCheck) {
      nReturn = 1;
    } // end if
  } else if (szName.Compare(OBJ_ASKFORMEDIUM       ) == 0) {
    if (((nState == DBMCli_Recover::RecoverStatePrepared) || (nState == DBMCli_Recover::RecoverStateReplace)) && 
          !bCheck && (nMediaInfo == DBMCli_Recover::MediaInfoNone) ) {
      nReturn = 1;
    } // end if
  } else if (szName.Compare(OBJ_ASKFORMEDIUMCHECK  ) == 0) {
    if (((nState == DBMCli_Recover::RecoverStatePrepared) || (nState == DBMCli_Recover::RecoverStateReplace)) && 
          bCheck && (nMediaInfo == DBMCli_Recover::MediaInfoNone) ) {
      nReturn = 1;
    } // end if
  } else if (szName.Compare(OBJ_RECOVERISRUNNING   ) == 0) {   
    if ((nState == DBMCli_Recover::RecoverStateRunning) && !bCheck) {
      nReturn = 1;
    } // end if
  } else if (szName.Compare(OBJ_CHECKISRUNNING     ) == 0) {
    if ((nState == DBMCli_Recover::RecoverStateRunning) && bCheck) {
      nReturn = 1;
    } // end if
  } else if (szName.Compare(OBJ_COMPLETE           ) == 0) {
    nReturn = (nState == DBMCli_Recover::RecoverStateRestarted) ? 1 : 0;
  } else if (szName.Compare(OBJ_LOGEMPTY           ) == 0) {
    if ((nState == DBMCli_Recover::RecoverStatePrepared)   && 
        (nState != DBMCli_Recover::RecoverStateAsking)     && 
         !bCheck && m_pDatabase->GetRestartInfo().IsLogEmpty()) {
      nReturn = 1;
    } // end if
  } else if (szName.Compare(OBJ_RESTARTWITHLOG     ) == 0) {
    if ( (nState == DBMCli_Recover::RecoverStateClosed) && bRestartable && !bCheck && oUntil.IsNull()) {
      nReturn = 1;
    } // end if
  } else if (szName.Compare(OBJ_RESTARTWITHLOGUNT  ) == 0) {
    if ( (nState == DBMCli_Recover::RecoverStateClosed) && bRestartable && !bCheck && !oUntil.IsNull()) {
      nReturn = 1;
    } // end if
  } else if (szName.Compare(OBJ_NORESTART          ) == 0) {
    if ( ((nState == DBMCli_Recover::RecoverStateClosed) || (nState == DBMCli_Recover::RecoverStateAsking)) && 
         !bRestartable && !bCheck) {
      nReturn = 1;
    } // end if
  } else if (szName.Compare(OBJ_CHECKCOMPLETE      ) == 0) {
    if ( (nState == DBMCli_Recover::RecoverStateAsking) && bCheck) {
      nReturn = 1;
    } // end if
  } else if (szName.Compare(OBJ_CHECKFULLCOMPLETE  ) == 0) {
    if ( (nState == DBMCli_Recover::RecoverStateClosed) && bCheck) {
      nReturn = 1;
    } // end if
  } else if (szName.Compare(OBJ_NEXTDATAMEDIUM     ) == 0) {
    if ( (nState     == DBMCli_Recover::RecoverStateReplace) && 
         (nMediaInfo == DBMCli_Recover::MediaInfoNextData ) && !bCheck) {
      nReturn = 1;
    } // end if
  } else if (szName.Compare(OBJ_NEXTDATAMEDIUMCHECK) == 0) {
    if ( (nState     == DBMCli_Recover::RecoverStateReplace) && 
         (nMediaInfo == DBMCli_Recover::MediaInfoNextData ) && bCheck) {
      nReturn = 1;
    } // end if
  } else if (szName.Compare(OBJ_FIRSTLOG           ) == 0) {
    if ( (nState     == DBMCli_Recover::RecoverStatePrepared) && 
         (nMediaInfo == DBMCli_Recover::MediaInfoFirstLog   ) && !bCheck) {
      nReturn = 1;
    } // end if
  } else if (szName.Compare(OBJ_FIRSTLOGCHECK      ) == 0) {
    if ( (nState     == DBMCli_Recover::RecoverStatePrepared) && 
         (nMediaInfo == DBMCli_Recover::MediaInfoFirstLog   ) && bCheck) {
      nReturn = 1;
    } // end if
  } else if (szName.Compare(OBJ_INVALIDLOGFILE     ) == 0) {
    if ( (nState     == DBMCli_Recover::RecoverStateReplace ) && 
         (nMediaInfo == DBMCli_Recover::MediaInfoErrNextLog)    ) {
      nReturn = 1;
    } // end if
  } else if (szName.Compare(OBJ_INVALIDDATAFILE    ) == 0) {
    if ( (nState     == DBMCli_Recover::RecoverStateReplace  ) && 
         (nMediaInfo == DBMCli_Recover::MediaInfoErrNextData)    ) {
      nReturn = 1;
    } // end if
  } else if (szName.Compare(OBJ_RECOVERCANCEL      ) == 0) {
    if ( (nState     == DBMCli_Recover::RecoverStateCancelled) && !bCheck) {
      nReturn = 1;
    } // end if
  } else if (szName.Compare(OBJ_CHECKCANCEL        ) == 0) {
    if ( (nState     == DBMCli_Recover::RecoverStateCancelled) && bCheck) {
      nReturn = 1;
    } // end if
  } // end if

  // Table
  if        (szName.Compare(OBJ_ROWPAIR) == 0) {
    nReturn = -1; // -> askForContinue
  } else if (szName.Compare(OBJ_EVENROW) == 0) {
    ++m_nCurrentItem;
    nReturn = (m_nCurrentItem < nMax) ? 1 : 0;
  } else if (szName.Compare(OBJ_ODDROW )  == 0) {
    ++m_nCurrentItem;
    nReturn = (m_nCurrentItem < nMax) ? 1 : 0;
  } // end if

  // Input
  if        (szName.Compare(OBJ_MEDIUMDATA    )  == 0) {
    if ( ( (nState     == DBMCli_Recover::RecoverStateReplace ) ||
           (nState     == DBMCli_Recover::RecoverStatePrepared)    ) && 
         (nMediaInfo != DBMCli_Recover::MediaInfoNone              )    ) {
      nReturn = 1;
    } // end if
  } else if (szName.Compare(OBJ_ASKFORLOCATION)  == 0) {
    if ( (nMediaInfo == DBMCli_Recover::MediaInfoErrNextLog ) ||
         (nMediaInfo == DBMCli_Recover::MediaInfoNextData   ) ||
         (nMediaInfo == DBMCli_Recover::MediaInfoErrNextData)    ) {
      nReturn = 1;
    } // end if
  } else if (szName.Compare(OBJ_SHOWLOCATION  )  == 0) {
    if (nMediaInfo == DBMCli_Recover::MediaInfoFirstLog) {
      nReturn = 1;
    } // end if
  } else if (szName.Compare(OBJ_ASKFORVERSION )  == 0) {
    if (nMediaInfo == DBMCli_Recover::MediaInfoFirstLog) {
      nReturn = 1;
    } // end if
  } // end if

  // Buttons
  if        (szName.Compare(OBJ_BTNBACK       ) == 0) {
    nReturn = (nState == DBMCli_Recover::RecoverStatePrepared) ? 1 : 0;
  } else if (szName.Compare(OBJ_BTNSTART      ) == 0) {
    nReturn = (nState == DBMCli_Recover::RecoverStatePrepared) ? 1 : 0;
  } else if (szName.Compare(OBJ_BTNOK         ) == 0) {
    // ****
  } else if (szName.Compare(OBJ_BTNCONTINUE   ) == 0) {
    if ( ((nState == DBMCli_Recover::RecoverStateAsking ) || 
          (nState == DBMCli_Recover::RecoverStateReplace)    ) &&
         (nCurrent <  nMax                                   )    ) {
      nReturn = 1;
    } // end if
  } else if (szName.Compare(OBJ_BTNIGNORE     ) == 0) {
    if ( (nState == DBMCli_Recover::RecoverStateReplace ) &&
         (nCurrent <  nMax                              ) &&
         bIgnore                                             ) {
      nReturn = 1;
    } // end if
  } else if (szName.Compare(OBJ_BTNCANCEL     ) == 0) {
    if ( (nState == DBMCli_Recover::RecoverStateReplace ) &&
         (nCurrent <  nMax                              )    ) {
      nReturn = 1;
    } // end if
  } else if (szName.Compare(OBJ_BTNRESTART    ) == 0) {
    if ( ((nState == DBMCli_Recover::RecoverStateAsking) || (nState == DBMCli_Recover::RecoverStateClosed)) && 
          bRestartable  && !bCheck) {
      nReturn = 1;
    } // end if
  } else if (szName.Compare(OBJ_BTNCLOSE      ) == 0) {
    if ( (nState == DBMCli_Recover::RecoverStateRestarted) || 
         (nState == DBMCli_Recover::RecoverStateAsking   ) || 
         (nState == DBMCli_Recover::RecoverStateCancelled) || 
         (nState == DBMCli_Recover::RecoverStateClosed   )    ) {
      nReturn = 1;
    } // end if
  } // end if

  return nReturn;
} // DBMWeb_TemplateRecovery :: askForWriteCountRecoverState

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateRecovery :: askForValue
  -----------------------------------------------------------------------------
*/
Tools_DynamicUTF8String DBMWeb_TemplateRecovery :: askForValue ( const Tools_DynamicUTF8String & szName )
{
  Tools_DynamicUTF8String      sValue;

  if (szName.Compare(VAL_GROUPBOXTITLE) == 0) {
    if (m_sCheck.Compare(VAL_CHECK) == 0) {
      sValue.Assign(" Check - Backup");
    } else {
      sValue.Assign(" Recovery - Database");
    } // end if
  } else if (szName.Compare(VAL_GLBRECOVERYMODE) == 0) {
    sValue.Assign(m_sRecMode);
  } else if (szName.Compare(VAL_GLBRECOVERYTYPE) == 0) {
    sValue.Assign(m_sRecType);
  } else if (szName.Compare(VAL_GLBMEDTYPE) == 0) {
    sValue.Assign(m_sMedType);
  } else if (szName.Compare(VAL_GLBCHECK) == 0) {
    sValue.Assign(m_sCheck);
  } else if (szName.Compare(VAL_GLBUNTIL) == 0) {
    sValue.Assign(m_oUntil.Get(DBMCLI_DT_NUM_DATETIME_FMT));
  } else if (szName.Compare(VAL_ACTION) == 0) {
    sValue.Assign(m_sAction);
  } else {
    switch (m_nMode) {
      case MODE_SELRECTYPE:
        sValue = askForValueSelectType(szName);
        break;
      case MODE_SELDATASAVE:
        sValue = askForValueDataSaveList(szName);
        break;
      case MODE_SELMEDIUM:
        sValue = askForValueMediaList(szName);
        break;
      case MODE_COMMITLIST:
        sValue = askForValueRecoveryList(szName);
        break;
      case MODE_RECSTATE:
        sValue = askForValueRecoverState(szName);
        break;
    } // end switch
  } // end if

   return sValue;
} // end DBMWeb_TemplateRecovery :: askForValue

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateRecovery :: askForValueSelectType
  -----------------------------------------------------------------------------
*/
Tools_DynamicUTF8String DBMWeb_TemplateRecovery :: askForValueSelectType ( const Tools_DynamicUTF8String & szName )
{
  Tools_DynamicUTF8String      sValue;

  if (szName.Compare(VAL_RECOVERYMODE) == 0) {
    if (m_sRecMode.Compare(VAL_ACTIVATE) == 0) {
      sValue.Assign(HTML_CHECKED);
    } 
  } else if (szName.Compare(VAL_UNTILDATE) == 0) {
    if (!m_oUntil.IsNull()) {
      sValue.Assign(m_oUntil.Date().Get(DBMCLI_DT_INT_DATE_FMT));
    } else {
      sValue.Assign(DBMCli_Date::Now().Get(DBMCLI_DT_INT_DATE_FMT));
    } // end if
  } else if (szName.Compare(VAL_UNTILTIME) == 0) {
    if (!m_oUntil.IsNull()) {
      sValue.Assign(m_oUntil.Time().Get(DBMCLI_DT_INT_TIME_FMT));
    } else {
      sValue.Assign(DBMCli_Time::Now().Get(DBMCLI_DT_INT_TIME_FMT));
    } // end if
  } else if (szName.Compare(VAL_CHECKED) == 0) {
    if (!m_oUntil.IsNull()) {
      sValue.Assign(HTML_CHECKED);
    } // end if
  } // end if

   return sValue;
} // end DBMWeb_TemplateRecovery :: askForValueSelectType

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateRecovery :: askForValueDataSaveList
  -----------------------------------------------------------------------------
*/
Tools_DynamicUTF8String DBMWeb_TemplateRecovery :: askForValueDataSaveList ( const Tools_DynamicUTF8String & szName )
{
  Tools_DynamicUTF8String      sValue;

  if (szName.Compare(VAL_KEY) == 0) {
    sValue.Assign(m_pDatabase->GetHistory().HistoryItemArray()[m_nCurrentItem].SysKeyStr( ));
  } else if (szName.Compare(VAL_LABEL) == 0) {
    sValue.Assign(m_pDatabase->GetHistory().HistoryItemArray()[m_nCurrentItem].LabelStr( ));
  } else if (szName.Compare(VAL_BEGINNING) == 0) {
    sValue.Assign(m_pDatabase->GetHistory().HistoryItemArray()[m_nCurrentItem].StartDateStr( ));
  } else if (szName.Compare(VAL_MEDIANAME) == 0) {
    sValue.Assign(m_pDatabase->GetHistory().HistoryItemArray()[m_nCurrentItem].MediaNameStr( ));
  } else if (szName.Compare(VAL_LOGREQUIRED) == 0) {
    sValue.Assign(m_pDatabase->GetHistory().HistoryItemArray()[m_nCurrentItem].LogRequiredStr( ));
  } else if (szName.Compare(VAL_SIZEPAGES) == 0) {
    sValue.Assign(m_pDatabase->GetHistory().HistoryItemArray()[m_nCurrentItem].SizePagesStr( ));
  } else if (szName.Compare(VAL_VOLUMES) == 0) {
    sValue.Assign(m_pDatabase->GetHistory().HistoryItemArray()[m_nCurrentItem].VolumesStr( ));
  } else if (szName.Compare(VAL_LOGPAGE) == 0) {
    sValue.Assign(m_pDatabase->GetHistory().HistoryItemArray()[m_nCurrentItem].LogPageStr( ));
  } else if (szName.Compare(VAL_LASTSAVEPOINT) == 0) {
    sValue.Assign(m_pDatabase->GetHistory().HistoryItemArray()[m_nCurrentItem].LastSavePointStr( ));
  } // end if

   return sValue;
} // end DBMWeb_TemplateRecovery :: askForValueDataSaveList

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateRecovery :: askForValueMediaList
  -----------------------------------------------------------------------------
*/
Tools_DynamicUTF8String DBMWeb_TemplateRecovery :: askForValueMediaList ( const Tools_DynamicUTF8String & szName )
{
  Tools_DynamicUTF8String      sValue;

  if (m_sMedType.Compare(VAL_SINGLE) == 0) {

    if (szName.Compare(VAL_MEDIANAME) == 0) {
      sValue.Assign(m_pDatabase->GetMedia( ).MediumArray( )[m_nCurrentItem].Name());
    } else if (szName.Compare(VAL_INDEX) == 0) {
      sValue.ConvertFromInt(m_nCurrentItem);
    } else if (szName.Compare(VAL_SIZEPAGES) == 0) {
      if ( m_pDatabase->GetMedia( ).MediumArray( )[m_nCurrentItem].Pages() != "0") {
        sValue.Assign(m_pDatabase->GetMedia( ).MediumArray( )[m_nCurrentItem].Pages());
      } // end if
    } else if (szName.Compare(VAL_LOCATION) == 0) {
      sValue.Assign(m_pDatabase->GetMedia( ).MediumArray( )[m_nCurrentItem].Location());
    } else if (szName.Compare(VAL_DEVICETYPE) == 0) {
      sValue.Assign(m_pDatabase->GetMedia( ).MediumArray( )[m_nCurrentItem].DeviceType());
    } else if (szName.Compare(VAL_BACKUPTYPE) == 0) {
      sValue.Assign(m_pDatabase->GetMedia( ).MediumArray( )[m_nCurrentItem].BackupType());
    } else if (szName.Compare(VAL_MODIFIED) == 0) {
      sValue.Assign(m_pDatabase->GetMedia( ).MediumArray( )[m_nCurrentItem].Modified());
    } else if (szName.Compare(VAL_OVERWRITE) == 0) {
      sValue.Assign(m_pDatabase->GetMedia( ).MediumArray( )[m_nCurrentItem].Overwrite());
    } // end if

  } else if (m_sMedType.Compare(VAL_PARALLEL) == 0) {

    if (szName.Compare(VAL_MEDIANAME) == 0) {
      sValue.Assign(m_pDatabase->GetMedia( ).MediumParallelArray( )[m_nCurrentItem].Name());
    } else if (szName.Compare(VAL_INDEX) == 0) {
      sValue.ConvertFromInt(m_nCurrentItem);
    } else if (szName.Compare(VAL_DEVICETYPE) == 0) {
      sValue.Assign(m_pDatabase->GetMedia( ).MediumParallelArray( )[m_nCurrentItem].DeviceType());
    } else if (szName.Compare(VAL_BACKUPTYPE) == 0) {
      sValue.Assign(m_pDatabase->GetMedia( ).MediumParallelArray( )[m_nCurrentItem].BackupType());
    } else if (szName.Compare(VAL_MODIFIED) == 0) {
      sValue.Assign(m_pDatabase->GetMedia( ).MediumParallelArray( )[m_nCurrentItem].Modified());
    } else if (szName.Compare(VAL_OVERWRITE) == 0) {
      sValue.Assign(m_pDatabase->GetMedia( ).MediumParallelArray( )[m_nCurrentItem].Overwrite());
    } // end if

  } // end if

  return sValue;
} // end DBMWeb_TemplateRecovery :: askForValueMediaList

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateRecovery :: askForValueRecoveryList
  -----------------------------------------------------------------------------
*/
Tools_DynamicUTF8String DBMWeb_TemplateRecovery :: askForValueRecoveryList ( const Tools_DynamicUTF8String & szName )
{
  Tools_DynamicUTF8String      sValue;

  if (szName.Compare(VAL_KEY) == 0) {
    sValue.Assign(m_pDatabase->GetHistory().HistoryItemArray()[m_nCurrentItem].SysKeyStr( ));
  } else if (szName.Compare(VAL_INDEX) == 0) {
    sValue.ConvertFromInt(m_nCurrentItem);
  } else if (szName.Compare(VAL_TYPE) == 0) {
    if (m_pDatabase->GetHistory().HistoryItemArray()[m_nCurrentItem].Label() == HISTORY_LABEL_DATA) {
      sValue.Assign(HISTORY_LABEL_DAT_STR);
    } else if (m_pDatabase->GetHistory().HistoryItemArray()[m_nCurrentItem].Label() == HISTORY_LABEL_PAGES) {
      sValue.Assign(HISTORY_LABEL_PAG_STR);
    } else if (m_pDatabase->GetHistory().HistoryItemArray()[m_nCurrentItem].Label() == HISTORY_LABEL_LOG) {
      sValue.Assign(HISTORY_LABEL_LOG_STR);
    } // end if
  } else if (szName.Compare(VAL_NEXTLOG) == 0) {
    if (m_pDatabase->GetHistory().HistoryItemArray()[m_nCurrentItem].Label( ) != HISTORY_LABEL_LOG) {
      sValue.Assign(m_pDatabase->GetHistory().HistoryItemArray()[m_nCurrentItem].LogPageStr( ));
    } else {
      sValue.Assign(m_pDatabase->GetHistory().HistoryItemArray()[m_nCurrentItem].ToPageStr( ));
    } // end if
  } else if (szName.Compare(VAL_CHECKED) == 0) {
    if (m_pDatabase->GetHistory().HistoryItemArray()[m_nCurrentItem].IsActiveForRecovery()) {
      sValue.Assign(HTML_CHECKED);
    } // end if
  } else if (szName.Compare(VAL_LABEL) == 0) {
    sValue.Assign(m_pDatabase->GetHistory().HistoryItemArray()[m_nCurrentItem].LabelStr( ));
  } else if (szName.Compare(VAL_BEGINNING) == 0) {
    sValue.Assign(m_pDatabase->GetHistory().HistoryItemArray()[m_nCurrentItem].StartDateStr( ));
  } else if (szName.Compare(VAL_BAACTION) == 0) {
    sValue.Assign(m_pDatabase->GetHistory().HistoryItemArray()[m_nCurrentItem].ActionStr( ));
  } else if (szName.Compare(VAL_MEDIANAME) == 0) {
    sValue.Assign(m_pDatabase->GetHistory().HistoryItemArray()[m_nCurrentItem].MediaNameStr( ));
  } else if (szName.Compare(VAL_LOGREQUIRED) == 0) {
    if (m_pDatabase->GetHistory().HistoryItemArray()[m_nCurrentItem].Label( ) != HISTORY_LABEL_LOG) {
      sValue.Assign(m_pDatabase->GetHistory().HistoryItemArray()[m_nCurrentItem].LogRequiredStr( ));
    } else {
      sValue.Assign(HTML_SPACE);
    } // end if
  } else if (szName.Compare(VAL_SIZEPAGES) == 0) {
    sValue.Assign(m_pDatabase->GetHistory().HistoryItemArray()[m_nCurrentItem].SizePagesStr( ));
  } else if (szName.Compare(VAL_VOLUMES) == 0) {
    sValue.Assign(m_pDatabase->GetHistory().HistoryItemArray()[m_nCurrentItem].VolumesStr( ));
  } else if (szName.Compare(VAL_LOGPAGE) == 0) {
    if (m_pDatabase->GetHistory().HistoryItemArray()[m_nCurrentItem].Label( ) != HISTORY_LABEL_LOG) {
      sValue.Assign(m_pDatabase->GetHistory().HistoryItemArray()[m_nCurrentItem].LogPageStr( ));
    } else {
      sValue.Assign(HTML_SPACE);
    } // end if
  } else if (szName.Compare(VAL_FROMLOG) == 0) {
    if (m_pDatabase->GetHistory().HistoryItemArray()[m_nCurrentItem].Label( ) == HISTORY_LABEL_LOG) {
      sValue.Assign(m_pDatabase->GetHistory().HistoryItemArray()[m_nCurrentItem].FromPageStr( ));
    } else {
      sValue.Assign(HTML_SPACE);
    } // end if
  } else if (szName.Compare(VAL_TOLOG) == 0) {
    if (m_pDatabase->GetHistory().HistoryItemArray()[m_nCurrentItem].Label( ) == HISTORY_LABEL_LOG) {
      sValue.Assign(m_pDatabase->GetHistory().HistoryItemArray()[m_nCurrentItem].ToPageStr( ));
    } else {
      sValue.Assign(HTML_SPACE);
    } // end if
  } else if (szName.Compare(VAL_LASTSAVEPOINT) == 0) {
    if (m_pDatabase->GetHistory().HistoryItemArray()[m_nCurrentItem].Label( ) != HISTORY_LABEL_LOG) {
      sValue.Assign(m_pDatabase->GetHistory().HistoryItemArray()[m_nCurrentItem].LastSavePointStr( ));
    } else {
      sValue.Assign(HTML_SPACE);
    } // end if
  } else if (szName.Compare(VAL_FIRSTCOMMIT) == 0) {
    if (m_pDatabase->GetHistory().HistoryItemArray()[m_nCurrentItem].Label( ) == HISTORY_LABEL_LOG) {
      sValue.Assign(m_pDatabase->GetHistory().HistoryItemArray()[m_nCurrentItem].FirstCommitStr( ));
    } else {
      sValue.Assign(HTML_SPACE);
    } // end if
  } else if (szName.Compare(VAL_LASTCOMMIT) == 0) {
    if (m_pDatabase->GetHistory().HistoryItemArray()[m_nCurrentItem].Label( ) == HISTORY_LABEL_LOG) {
      sValue.Assign(m_pDatabase->GetHistory().HistoryItemArray()[m_nCurrentItem].LastCommitStr( ));
    } else {
      sValue.Assign(HTML_SPACE);
    } // end if
  } // end if

   return sValue;
} // end DBMWeb_TemplateRecovery :: askForValueRecoveryList

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateRecovery :: askForValueRecoverState
  -----------------------------------------------------------------------------
*/
Tools_DynamicUTF8String DBMWeb_TemplateRecovery :: askForValueRecoverState ( const Tools_DynamicUTF8String & szName )
{
  Tools_DynamicUTF8String      sValue;

  SAPDB_Int                    nCurrent = m_pDatabase->GetRecover().CurrentItem();
  DBMCli_Recover::RecoverState nState   = m_pDatabase->GetRecover().GetRecoverState();
  DBMCli_DateTime              oUntil       = m_pDatabase->GetRecover().GetUntil();

  // table data
  if (szName.Compare(VAL_IMAGE) == 0) {
    if (m_nCurrentItem < nCurrent) {
      sValue.Assign(HTML_IMG_OK);
    } else if (m_nCurrentItem == nCurrent) {
      switch(nState) {
      case DBMCli_Recover::RecoverStatePrepared:
      case DBMCli_Recover::RecoverStateAsking:
      case DBMCli_Recover::RecoverStateReplace:
        sValue.Assign(HTML_IMG_ARROW);
        break;
      case DBMCli_Recover::RecoverStateRunning:
        sValue.Assign(HTML_IMG_WAIT);
        break;
      case DBMCli_Recover::RecoverStateCancelled:
      case DBMCli_Recover::RecoverStateRestarted:
        sValue.Assign(HTML_IMG_CANCEL);
        break;
      default:
        sValue.Assign(HTML_IMG_EMPTY);
      } // end switch
    } else {
      switch(nState) {
      case DBMCli_Recover::RecoverStateCancelled:
      case DBMCli_Recover::RecoverStateRestarted:
        sValue.Assign(HTML_IMG_CANCEL);
        break;
      default:
        sValue.Assign(HTML_IMG_EMPTY);
      } // end switch
    } // end if
  } else if (szName.Compare(VAL_LABEL) == 0) {
    sValue.Assign(m_pDatabase->GetRecover().RecoverItemArray()[m_nCurrentItem].GetLabel());
  } else if (szName.Compare(VAL_MEDIANAME) == 0) {
    sValue.Assign(m_pDatabase->GetRecover().RecoverItemArray()[m_nCurrentItem].GetMediumName());
  } else if (szName.Compare(VAL_LOCATION) == 0) {
    SAPDB_Int nIndex;
    for (nIndex = 0; nIndex < m_pDatabase->GetRecover().RecoverItemArray()[m_nCurrentItem].LocationCount(); ++nIndex) {
      if (nIndex > 0) {
        sValue.Append(" | ");
      } // end if
      sValue.Append(m_pDatabase->GetRecover().RecoverItemArray()[m_nCurrentItem].GetLocation(nIndex));
    } // end if
  } else if (szName.Compare(VAL_TRANSFERRED) == 0) {
    if ((m_nCurrentItem == nCurrent) && (nState == DBMCli_Recover::RecoverStateRunning)) {
      SAPDB_Int4 nTrans = m_pDatabase->GetRecover().Result( ).TransferredPages();
      SAPDB_Int4 nLeft  = m_pDatabase->GetRecover().Result( ).LeftPages();

      if (nLeft == 0  || nTrans == 0) {
        sValue.Assign("");
      } else {
        sValue.ConvertFromInt((nTrans * 100 ) / (nTrans + nLeft));
        sValue.Append(" %");
      } // end if

    } else if (m_nCurrentItem < nCurrent) {
      sValue.ConvertFromInt(100);
      sValue.Append(" %");
    } else {
      sValue.Assign("");
    } // end if
  } // end if

  // input data
  if ( (szName.Compare(VAL_ASKFORLOCATION)  == 0) ||
       (szName.Compare(VAL_SHOWLOCATION  )  == 0)     ) {
    if (nCurrent >= 0 && nCurrent < m_pDatabase->GetRecover().RecoverItemArray().GetSize()) {
      sValue.Assign(m_pDatabase->GetRecover().RecoverItemArray()[nCurrent].GetLocationName(0));
    } // end if
  } else if (szName.Compare(VAL_ASKFORVERSION)  == 0) {
    if (nCurrent >= 0 && nCurrent < m_pDatabase->GetRecover().RecoverItemArray().GetSize()) {
      sValue.Assign(m_pDatabase->GetRecover().RecoverItemArray()[nCurrent].GetVersion(0));
    } // end if
  } // end if

  // error data
  if ( (szName.Compare(OBJ_INVALIDLOGFILE)   == 0) ||
       (szName.Compare(OBJ_INVALIDDATAFILE)  == 0)    ) {
    sValue.ConvertFromInt(m_pDatabase->GetRecover().LastDBReturn());
    sValue.Append(", ")
          .Append(m_pDatabase->GetRecover().LastDBTxt());
  } // end if

  // refresh data
  if (szName.Compare(OBJ_STATEREFRESH) == 0) {

    // refresh rate
    SAPDB_Int               nResult;

    DBMCli_DateTime oBegin(m_pDatabase->GetRecover().Result( ).BeginningStr(), DBMCLI_DT_INT_DATETIME_FMT);
    DBMCli_DateTime oNow = DBMCli_DateTime::Now();

    if (oBegin.IsNull()) {
      sValue.Assign("1");
    } else {
      nResult = (oNow - oBegin) / 12;
      nResult = (nResult <   1) ?   1 : nResult;
      nResult = (nResult > 300) ? 300 : nResult;

      sValue.ConvertFromInt(nResult);
    } // end if

  } // end if

  // until date
  if (szName.Compare(OBJ_RESTARTWITHLOGUNT  ) == 0) {
    sValue.Assign(oUntil.Get(DBMCLI_DT_INT_DATETIME_FMT));
  } // end if

  return sValue;
} // end DBMWeb_TemplateRecovery :: askForValueRecoverState

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateRecovery :: askForContinue
  -----------------------------------------------------------------------------
*/
bool DBMWeb_TemplateRecovery :: askForContinue ( const Tools_DynamicUTF8String & szName )
{
  bool bRC = false;

  switch (m_nMode) {
    case MODE_SELRECTYPE:
      // nothing to do
      break;
    case MODE_SELDATASAVE:
      if( szName.Compare(OBJ_ROWPAIR ) == 0 ) {
        // find a valid Item
        --m_nCurrentItem;
        while(m_nCurrentItem >= 0) {
          if (m_pDatabase->GetHistory().HistoryItemArray()[m_nCurrentItem].Action( ) != HISTORY_ACTION_LOST) {
            break;
          } // end if
          --m_nCurrentItem;
        } // end while
        bRC = (m_nCurrentItem >= 0);
      } // end if
      break;
    case MODE_COMMITLIST:
      bRC = (m_nCurrentItem < m_pDatabase->GetHistory().HistoryItemArray().GetSize());
      break;
    case MODE_RECSTATE:
      bRC = (m_nCurrentItem < m_pDatabase->GetRecover().RecoverItemArray().GetSize());
      break;
  } // end switch

  return bRC;
} // DBMWeb_TemplateRecovery :: askForContinue
