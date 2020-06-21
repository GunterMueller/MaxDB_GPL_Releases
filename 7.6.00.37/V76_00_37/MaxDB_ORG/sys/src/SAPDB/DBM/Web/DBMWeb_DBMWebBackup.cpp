/*!
  -----------------------------------------------------------------------------
  module: DBMWeb_DBMWebBackup.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  Database backup funtions of the DBMWeb_DBMWeb class
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
#include "DBM/Web/DBMWeb_TemplateRecovery.hpp"
#include "DBM/Web/DBMWeb_TemplateMsgBox.hpp"
#include "DBM/Web/DBMWeb_DBMWeb.hpp"
#include "DBM/Web/DBMWeb_Web.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define PARAM_BACKUPFOR                "BackupFor"

#define PARAM_BACKUPTYPE               "BackupType"
#define PARAM_BACKUPTYPE_VAL_DATA      "DATA"
#define PARAM_BACKUPTYPE_VAL_PAGES     "PAGES"
#define PARAM_BACKUPTYPE_VAL_LOG       "LOG"
#define PARAM_BACKUPTYPE_VAL_AUTO      "AUTO"

#define PARAM_MEDIUMTYPE               "MediumType"
#define PARAM_MEDIUMTYPE_VAL_SINGLE    "SINGLE"
#define PARAM_MEDIUMTYPE_VAL_PARALLEL  "PARALLEL"

#define PARAM_NEWLOC                   "NewLocation"

#define PARAM_ACTION_VAL_SELECT        "SELECT"

#define PARAM_ACTION_VAL_AUTOON        "AUTOON"
#define PARAM_ACTION_VAL_AUTOOFF       "AUTOOFF"
#define PARAM_ACTION_VAL_AUTOCANCEL    "AUTOCANCEL"

#define PARAM_ACTION_VAL_BASTART       "BASTART"
#define PARAM_ACTION_VAL_BAREPLACE     "BAREPLACE"
#define PARAM_ACTION_VAL_BAIGNORE      "BAIGNORE"
#define PARAM_ACTION_VAL_BACANCEL      "BACANCEL"

#define PARAM_ACTION_VAL_BASTATE       "BASTATE"

#define PARAM_BUTTON                   "Button"
#define PARAM_BUTTON_VAL_BACK          "BtnBack"
#define PARAM_BUTTON_VAL_START         "BtnStart"
#define PARAM_BUTTON_VAL_OK            "BtnOK"
#define PARAM_BUTTON_VAL_CONTINUE      "BtnContinue"
#define PARAM_BUTTON_VAL_IGNORE        "BtnIgnore"
#define PARAM_BUTTON_VAL_CANCEL        "BtnCancel"
#define PARAM_BUTTON_VAL_RUN           "BtnRun"
#define PARAM_BUTTON_VAL_RESTART       "BtnRestart"
#define PARAM_BUTTON_VAL_SINGLE        "BtnSingle"
#define PARAM_BUTTON_VAL_PARALLEL      "BtnParallel"
#define PARAM_BUTTON_VAL_REFRESH       "BtnRefresh"
#define PARAM_BUTTON_VAL_STATE         "BtnState"

#define PARAM_RECMODE                  "GlbRecoveryMode"
#define PARAM_CHKRECMODE               "RecoveryMode"
#define PARAM_RECMODE_VAL_RECOVER      "RECOVER"
#define PARAM_RECMODE_VAL_ACTIVATE     "ACTIVATE"
#define PARAM_RECMODE_VAL_INSTALL      "INSTALL"

#define PARAM_RECTYPE                  "GlbRecoveryType"
#define PARAM_MEDTYPE                  "GlbMedType"
#define PARAM_RADRECTYPE               "RecoveryType"
#define PARAM_RECTYPE_VAL_LAST         "LAST"
#define PARAM_RECTYPE_VAL_SPECIFIC     "SPECIFIC"
#define PARAM_RECTYPE_VAL_MEDIUM       "MEDIUM"
#define PARAM_RECTYPE_VAL_CONTINUE     "CONTINUE"

#define PARAM_RECUNTIL                 "GlbUntil"
#define PARAM_CHKRECUNTIL              "RecoveryUntil"
#define PARAM_CHKRECUNTIL_VAL_ON       "ON"

#define PARAM_CHECK                    "GlbCheck"
#define PARAM_CHECK_VAL_CHECKON        "1"

#define PARAM_UNTILDATE                "UntilDate"
#define PARAM_UNTILTIME                "UntilTime"

#define PARAM_KEY                      "Key"

#define PARAM_CHKISACTIVE              "IsActive"

#define PARAM_ACTION_VAL_SELRECTYPE    "SELRECTYPE"
#define PARAM_ACTION_VAL_SELDATASAVE   "SELDATASAVE"
#define PARAM_ACTION_VAL_SELMEDIUM     "SELMEDIUM"
#define PARAM_ACTION_VAL_COMMITLIST    "COMMITLIST"
#define PARAM_ACTION_VAL_RECSTATE      "RECSTATE"

#define PARAM_LOCATION                 "AskForLocation"
#define PARAM_VERSION                  "AskForVersion"

/*
  =============================================================================
  class:        DBMWeb_DBMWeb
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: backupDB
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: backupDB ( sapdbwa_WebAgent    & wa,
                                       sapdbwa_HttpRequest & request,
                                       sapdbwa_HttpReply   & reply )
{
  SAPDB_Bool           bReturn = SAPDB_TRUE;
  SAPDB_Bool           bMedia  = SAPDB_FALSE;
  DBMCli_String        sAction;

  GetParameterValue(PARAM_ACTION, request, sAction);

  // is there a running backup?
  if (( sAction == PARAM_ACTION_VAL_REFRESH ) && m_Database->GetBackup( ).IsRunning ()) {
    sAction = PARAM_ACTION_VAL_BASTATE;
  } // end if

  if( ( sAction == PARAM_ACTION_VAL_REFRESH )   ||
      ( m_Database->GetMedia( ).MediumArray( ).GetSize( ) == 0 )    ) {

    Msg_List oMsgList;

    if( m_Database->GetMedia( ).Refresh( oMsgList ) ) {
      bMedia = SAPDB_TRUE;
    } else {
      sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
    } // end if
  } else {
    bMedia = SAPDB_TRUE;
  }  // end if

  if( bMedia ) {

    DBMCli_String    sBackupType;
    DBMCli_String    sMediumType;
    DBMCli_Backup  & oBackup = m_Database->GetBackup( );
    DBMCli_Media   & oMedia  = m_Database->GetMedia( );

    GetParameterValue(PARAM_BACKUPTYPE, request, sBackupType);
    GetParameterValue(PARAM_MEDIUMTYPE, request, sMediumType);

    DBMCli_BackupType oBackupType( sBackupType );
    if( oBackupType.Value( ) == DBMCLI_BACKUPTYPE_UNKNOWN ) {
      oBackupType.SetValue( DBMCLI_BACKUPTYPE_DATA );
    } // end if
    DBMCli_MediumType oMediumType( sMediumType );
    if( oMediumType.Value( ) == DBMCLI_MEDIUMTYPE_UNKNOWN ) {
      oMediumType.SetValue( DBMCLI_MEDIUMTYPE_SINGLE );
    } // end if

    if( sAction == PARAM_ACTION_VAL_REFRESH ) {
      // view media for backup type
      bReturn = backupDB_View ( wa, request, reply, oBackup, oMedia, oBackupType, oMediumType );
    } else if( sAction == PARAM_ACTION_VAL_SELECT ) {
      // select medium
      bReturn = backupDB_Select ( wa, request, reply, oBackup, oMedia, oBackupType, oMediumType );
    } else if( sAction == PARAM_ACTION_VAL_BASTART ) {
      // start backup
      bReturn = backupDB_Start ( wa, request, reply, oBackup, oMedia, oBackupType, oMediumType );
    } else if( sAction == PARAM_ACTION_VAL_BAREPLACE ) {
      // replace medium
      bReturn = backupDB_Replace ( wa, request, reply, oBackup, oMedia, oBackupType, oMediumType );
    } else if( sAction == PARAM_ACTION_VAL_BAIGNORE ) {
      // ignore medium
      bReturn = backupDB_Ignore ( wa, request, reply, oBackup, oMedia, oBackupType, oMediumType );
    } else if( sAction == PARAM_ACTION_VAL_BACANCEL ) {
      // cancel backup
      bReturn = backupDB_Cancel ( wa, request, reply, oBackup, oMedia, oBackupType, oMediumType );
    } else if( sAction == PARAM_ACTION_VAL_BASTATE ) {
      // show state
      bReturn = backupDB_State ( wa, request, reply, oBackup, oMedia );
    } else if( sAction == PARAM_ACTION_VAL_AUTOON ) {
      // switch autolog on
      bReturn = backupDB_AutoOn ( wa, request, reply, oBackup, oMedia, oBackupType, oMediumType );
    } else if( sAction == PARAM_ACTION_VAL_AUTOOFF ) {
      // switch autolog off
      bReturn = backupDB_AutoOff ( wa, request, reply, oBackup, oMedia, oBackupType, oMediumType );
    } else if( sAction == PARAM_ACTION_VAL_AUTOCANCEL ) {
      // switch autolog off
      bReturn = backupDB_AutoCancel ( wa, request, reply, oBackup, oMedia, oBackupType, oMediumType );
    } else {
      DBMWeb_TemplateMsgBox oMsgBox(wa, DBMWEB_TEMPLMSGBOX_INFO, "", "Web DBM service error (unknown action)!");
      oMsgBox.writePage(Tools_TemplateWriterWA(reply));
    } // end if
  } // end if

  return bReturn;
} // end DBMWeb_DBMWeb :: backupDB

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: backupDB_View
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: backupDB_View       ( sapdbwa_WebAgent    & wa,
                                                  sapdbwa_HttpRequest & request,
                                                  sapdbwa_HttpReply   & reply,
                                                  DBMCli_Backup       & oBackup,
                                                  DBMCli_Media        & oMedia,
                                                  DBMCli_BackupType   & oBackupType,
                                                  DBMCli_MediumType   & oMediumType )


{
  DBMWeb_TemplBackup_Mode nMode = DBMWEB_TEMPLBACKUP_MODE_MEDIA;

  if( oBackupType.Value( ) == DBMCLI_BACKUPTYPE_AUTO ) {
    if( m_Database->GetAutolog( ).State( ) == DBMCLI_AUTOLOGSTATE_ON ) {
      nMode = DBMWEB_TEMPLBACKUP_MODE_AUTOLOGOFF;
    } // end if
  } // end if

  DBMWeb_TemplateBackup oTemplate( wa, oBackup, oMedia, nMode, oBackupType, oMediumType, "");
  oTemplate.writePage( Tools_TemplateWriterWA( reply ) );

  return SAPDB_TRUE;
} // end DBMWeb_DBMWeb :: backupDB_View

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: backupDB_Select
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: backupDB_Select     ( sapdbwa_WebAgent    & wa,
                                                  sapdbwa_HttpRequest & request,
                                                  sapdbwa_HttpReply   & reply,
                                                  DBMCli_Backup       & oBackup,
                                                  DBMCli_Media        & oMedia,
                                                  DBMCli_BackupType   & oBackupType,
                                                  DBMCli_MediumType   & oMediumType )
{
  DBMWeb_TemplBackup_Mode nMode = DBMWEB_TEMPLBACKUP_MODE_SELECT;

  DBMCli_String sMediumName;
  GetParameterValue(PARAM_NAME, request, sMediumName);

  DBMWeb_TemplateBackup oTemplate( wa, oBackup, oMedia, nMode, oBackupType, oMediumType, sMediumName);
  oTemplate.writePage( Tools_TemplateWriterWA( reply ) );

  return SAPDB_TRUE;
} // end DBMWeb_DBMWeb :: backupDB_Select

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: backupDB_State
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: backupDB_State      ( sapdbwa_WebAgent    & wa,
                                                  sapdbwa_HttpRequest & request,
                                                  sapdbwa_HttpReply   & reply,
                                                  DBMCli_Backup       & oBackup,
                                                  DBMCli_Media        & oMedia )
{
  Msg_List    oMsgList;
  SAPDB_Bool              bAvailable  = false;
  SAPDB_Bool              bSuccess    = true;
  DBMCli_String           sMediumName = oBackup.GetMediumName();
  DBMCli_BackupType       oBackupType(oBackup.GetBackupType());
  DBMCli_MediumType       oMediumType(oBackup.GetMediumType());
  
  // check for reply
  if (!oBackup.IsAnswerReceived()) {
    bSuccess = oBackup.ReplyAvailable(bAvailable, oMsgList);
    if (bSuccess) {
      if (bAvailable) {
        // receive reply
        bSuccess = oBackup.ReplyReceive (oMsgList);
        if (bSuccess) {
          // check backup result
          DBMCli_BackupResult & oResult = oBackup.Result( );
          if( oResult.IsReady( ) ) {
            DBMWeb_TemplateBackup oTemplate( wa, oBackup, oMedia, DBMWEB_TEMPLBACKUP_MODE_READY, oBackupType, oMediumType, sMediumName);
            oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
          } else if( oResult.IsFull( ) ) {
            DBMWeb_TemplateBackup oTemplate( wa, oBackup, oMedia, DBMWEB_TEMPLBACKUP_MODE_REPLACE, oBackupType, oMediumType, sMediumName);
            oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
          } // end if
        } // end if
      } else {
        // determine current state
        bSuccess = oBackup.State(oMsgList);
        if (bSuccess) {
          DBMWeb_TemplateBackup oTemplate( wa, oBackup, oMedia, DBMWEB_TEMPLBACKUP_MODE_STATE, oBackupType, oMediumType, sMediumName);
          oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
        } // end if
      } // end if
    } // end if
  } else {
    // check backup result
    DBMCli_BackupResult & oResult = oBackup.Result( );
    if( oResult.IsReady( ) ) {
      DBMWeb_TemplateBackup oTemplate( wa, oBackup, oMedia, DBMWEB_TEMPLBACKUP_MODE_READY, oBackupType, oMediumType, sMediumName);
      oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
    } else if( oResult.IsFull( ) ) {
      DBMWeb_TemplateBackup oTemplate( wa, oBackup, oMedia, DBMWEB_TEMPLBACKUP_MODE_REPLACE, oBackupType, oMediumType, sMediumName);
      oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
    } // end if
  } // end if

  if (!bSuccess) {
    DBMWeb_TemplateMsgBox oMsgBox(wa, DBMWEB_TEMPLMSGBOX_ERROR, oMsgList, "Error - " + m_Database->DatabaseName());
    DBMCli_String sAction;
    sAction = sAction + "javascript:parent.GotoWebDBMURL(this, \\'Event=BACKUP&" +
              PARAM_ACTION + "=" +  PARAM_ACTION_VAL_REFRESH + "&" +
              PARAM_BACKUPTYPE  + "=" +  oBackupType.Name() + "\\')";
    oMsgBox.SetButtonAction(sAction)
           .writePage(Tools_TemplateWriterWA(reply));
  } // end if

  return SAPDB_TRUE;
} // end DBMWeb_DBMWeb :: backupDB_State

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: backupDB_Start
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: backupDB_Start      ( sapdbwa_WebAgent    & wa,
                                                  sapdbwa_HttpRequest & request,
                                                  sapdbwa_HttpReply   & reply,
                                                  DBMCli_Backup       & oBackup,
                                                  DBMCli_Media        & oMedia,
                                                  DBMCli_BackupType   & oBackupType,
                                                  DBMCli_MediumType   & oMediumType )
{
  Msg_List    oMsgList;
  DBMCli_String           sMediumName;
  DBMCli_String           sBackupFor;

  GetParameterValue(PARAM_NAME, request, sMediumName);
  GetParameterValue(PARAM_BACKUPFOR, request, sBackupFor);

  // Send a BackupStart-Reuqest to the DBMServer
  if( oBackup.StartRequest(sMediumName, sBackupFor, oBackupType.Name(), oMediumType.Name(), oMsgList) ) { 
    // activate state page
    DBMWeb_TemplateBackup oTemplate( wa, oBackup, oMedia, DBMWEB_TEMPLBACKUP_MODE_SHOWSTATE, oBackupType, oMediumType, sMediumName);
    oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
  } else {
    DBMWeb_TemplateMsgBox oMsgBox(wa, DBMWEB_TEMPLMSGBOX_ERROR, oMsgList, "Error - " + m_Database->DatabaseName());
    DBMCli_String sAction;
    sAction = sAction + "javascript:parent.GotoWebDBMURL(this, \\'Event=BACKUP&" +
              PARAM_ACTION + "=" +  PARAM_ACTION_VAL_REFRESH + "&" +
              PARAM_BACKUPTYPE  + "=" +  oBackupType.Name() + "\\')";
    oMsgBox.SetButtonAction(sAction)
           .writePage(Tools_TemplateWriterWA(reply));
  } // end if

  return SAPDB_TRUE;
} // end DBMWeb_DBMWeb :: backupDB_Start 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: backupDB_Replace
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: backupDB_Replace ( sapdbwa_WebAgent    & wa,
                                               sapdbwa_HttpRequest & request,
                                               sapdbwa_HttpReply   & reply,
                                               DBMCli_Backup       & oBackup,
                                               DBMCli_Media        & oMedia,
                                               DBMCli_BackupType   & oBackupType,
                                               DBMCli_MediumType   & oMediumType )
{
  Msg_List    oMsgList;
  DBMCli_String           sMediumName;
  DBMCli_String           sReplaceMediumName;
  DBMCli_String           sNewLocation;

  GetParameterValue(PARAM_NAME, request, sMediumName);
  GetParameterValue(PARAM_NEWLOC, request, sNewLocation);

  // handle parallel media
  if (oMediumType.Value() == DBMCLI_MEDIUMTYPE_PARALLEL) {
    SAPDB_Int nGroup = oMedia.IndexByNameParallel(sMediumName);
    if (nGroup != -1) {
      DBMCli_MediumParallel & oParallel = oMedia.MediumParallelArray( ).ElementAt(nGroup);
      SAPDB_Int nIndex = oParallel.IndexByLocation(oBackup.Result().LocationStr());
      oParallel.MediumArray()[nIndex].ChangeLocation(sNewLocation);
      sReplaceMediumName = sMediumName + "\\" + oParallel.MediumArray()[nIndex].Name();
    } // end if
  } else {
    sReplaceMediumName = sMediumName;
  } // end if

  if( oBackup.ReplaceRequest(sReplaceMediumName, sNewLocation, oMsgList) ) {
    // activate state page
    DBMWeb_TemplateBackup oTemplate( wa, oBackup, oMedia, DBMWEB_TEMPLBACKUP_MODE_SHOWSTATE, oBackupType, oMediumType, sMediumName);
    oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
  } else {
    DBMWeb_TemplateMsgBox oMsgBox(wa, DBMWEB_TEMPLMSGBOX_ERROR, oMsgList, "Error - " + m_Database->DatabaseName());
    DBMCli_String sAction;
    sAction = sAction + "javascript:parent.GotoWebDBMURL(this, \\'Event=BACKUP&" +
              PARAM_ACTION + "=" +  PARAM_ACTION_VAL_REFRESH + "&" +
              PARAM_BACKUPTYPE  + "=" +  oBackupType.Name() + "\\')";
    oMsgBox.SetButtonAction(sAction)
           .writePage(Tools_TemplateWriterWA(reply));
  } // end if

  return SAPDB_TRUE;
} // end DBMWeb_DBMWeb :: backupDB_Replace

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: backupDB_Ignore
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: backupDB_Ignore  ( sapdbwa_WebAgent    & wa,
                                               sapdbwa_HttpRequest & request,
                                               sapdbwa_HttpReply   & reply,
                                               DBMCli_Backup       & oBackup,
                                               DBMCli_Media        & oMedia,
                                               DBMCli_BackupType   & oBackupType,
                                               DBMCli_MediumType   & oMediumType )
{
  Msg_List    oMsgList;
  DBMCli_String           sMediumName;

  GetParameterValue(PARAM_NAME, request, sMediumName);

  if( oBackup.IgnoreRequest(oMsgList) ) {
    // activate state page
    DBMWeb_TemplateBackup oTemplate( wa, oBackup, oMedia, DBMWEB_TEMPLBACKUP_MODE_SHOWSTATE, oBackupType, oMediumType, sMediumName);
    oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
  } else {
    DBMWeb_TemplateMsgBox oMsgBox(wa, DBMWEB_TEMPLMSGBOX_ERROR, oMsgList, "Error - " + m_Database->DatabaseName());
    DBMCli_String sAction;
    sAction = sAction + "javascript:parent.GotoWebDBMURL(this, \\'Event=BACKUP&" +
              PARAM_ACTION + "=" +  PARAM_ACTION_VAL_REFRESH + "&" +
              PARAM_BACKUPTYPE  + "=" +  oBackupType.Name() + "\\')";
    oMsgBox.SetButtonAction(sAction)
           .writePage(Tools_TemplateWriterWA(reply));
  } // end if

  return SAPDB_TRUE;
} // end DBMWeb_DBMWeb :: backupDB_Ignore

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: backupDB_Cancel
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: backupDB_Cancel ( sapdbwa_WebAgent    & wa,
                                              sapdbwa_HttpRequest & request,
                                              sapdbwa_HttpReply   & reply,
                                              DBMCli_Backup       & oBackup,
                                              DBMCli_Media        & oMedia,
                                              DBMCli_BackupType   & oBackupType,
                                              DBMCli_MediumType   & oMediumType )
{
  Msg_List    oMsgList;

  if( oBackup.Cancel(oMsgList) ) {
    DBMWeb_TemplateMsgBox oMsgBox(wa, DBMWEB_TEMPLMSGBOX_CRITICAL, "", "Backup canceled!", "Info - " + m_Database->DatabaseName());
    DBMCli_String sAction;
    sAction = sAction + "javascript:parent.GotoWebDBMURL(this, \\'Event=BACKUP&" +
              PARAM_ACTION + "=" +  PARAM_ACTION_VAL_REFRESH + "&" +
              PARAM_BACKUPTYPE  + "=" +  oBackupType.Name() + "&" +
              PARAM_MEDIUMTYPE  + "=" +  oMediumType.Name() + "\\')";
    oMsgBox.SetButtonAction(sAction)
           .writePage(Tools_TemplateWriterWA(reply));
  } else {
    DBMWeb_TemplateMsgBox oMsgBox(wa, DBMWEB_TEMPLMSGBOX_ERROR, oMsgList, "Error - " + m_Database->DatabaseName());
    DBMCli_String sAction;
    sAction = sAction + "javascript:parent.GotoWebDBMURL(this, \\'Event=BACKUP&" +
              PARAM_ACTION + "=" +  PARAM_ACTION_VAL_REFRESH + "&" +
              PARAM_BACKUPTYPE  + "=" +  oBackupType.Name() + "\\')";
    oMsgBox.SetButtonAction(sAction)
           .writePage(Tools_TemplateWriterWA(reply));
  } // end if

  return SAPDB_TRUE;
} // end DBMWeb_DBMWeb :: backupDB_Cancel

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: backupDB_AutoOn
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: backupDB_AutoOn     ( sapdbwa_WebAgent    & wa,
                                                  sapdbwa_HttpRequest & request,
                                                  sapdbwa_HttpReply   & reply,
                                                  DBMCli_Backup       & oBackup,
                                                  DBMCli_Media        & oMedia,
                                                  DBMCli_BackupType   & oBackupType,
                                                  DBMCli_MediumType   & oMediumType )
{
  Msg_List    oMsgList;
  DBMCli_String           sMediumName;

  GetParameterValue(PARAM_NAME, request, sMediumName);

  DBMCli_Autolog & oAutolog = m_Database->GetAutolog( );

  if( oAutolog.On( sMediumName, oMsgList ) ) {
    DBMWeb_TemplateBackup oTemplate( wa, oBackup, oMedia, DBMWEB_TEMPLBACKUP_MODE_AUTOLOGOFF, oBackupType, oMediumType, "");
    oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
  } else {
    sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
  } // end if

  return SAPDB_TRUE;
} // end DBMWeb_DBMWeb :: backupDB_AutoOn

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: backupDB_AutoOff
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: backupDB_AutoOff    ( sapdbwa_WebAgent    & wa,
                                                  sapdbwa_HttpRequest & request,
                                                  sapdbwa_HttpReply   & reply,
                                                  DBMCli_Backup       & oBackup,
                                                  DBMCli_Media        & oMedia,
                                                  DBMCli_BackupType   & oBackupType,
                                                  DBMCli_MediumType   & oMediumType )
{
  Msg_List   oMsgList;
  DBMCli_Autolog       & oAutolog = m_Database->GetAutolog( );

  if( oAutolog.Off( oMsgList ) ) {
    DBMWeb_TemplateBackup oTemplate( wa, oBackup, oMedia, DBMWEB_TEMPLBACKUP_MODE_MEDIA, oBackupType, oMediumType, "");
    oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
  } else {
    sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
  } // end if

  return SAPDB_TRUE;
} // end DBMWeb_DBMWeb :: backupDB_AutoOff

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: backupDB_AutoCancel
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: backupDB_AutoCancel ( sapdbwa_WebAgent    & wa,
                                                  sapdbwa_HttpRequest & request,
                                                  sapdbwa_HttpReply   & reply,
                                                  DBMCli_Backup       & oBackup,
                                                  DBMCli_Media        & oMedia,
                                                  DBMCli_BackupType   & oBackupType,
                                                  DBMCli_MediumType   & oMediumType )
{
  Msg_List   oMsgList;
  DBMCli_Autolog       & oAutolog = m_Database->GetAutolog( );

  if( oAutolog.Cancel( oMsgList ) ) {
    DBMWeb_TemplateBackup oTemplate( wa, oBackup, oMedia, DBMWEB_TEMPLBACKUP_MODE_AUTOLOGOFF, oBackupType, oMediumType, "");
    oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
  } else {
    sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
  } // end if

  return SAPDB_TRUE;
} // end DBMWeb_DBMWeb :: backupDB_AutoCancel

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: recoverDB
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: recoverDB ( sapdbwa_WebAgent    & wa,
                                        sapdbwa_HttpRequest & request,
                                        sapdbwa_HttpReply   & reply )
{
  SAPDB_Bool           bReturn = SAPDB_TRUE;
  DBMCli_String        sAction;

  GetParameterValue(PARAM_ACTION, request, sAction);
  
  if ( m_Database->GetRecover().GetRecoverState() == DBMCli_Recover::RecoverStateRunning) {
    sAction = PARAM_ACTION_VAL_RECSTATE;
  } // end if

  if( sAction == PARAM_ACTION_VAL_REFRESH ) {
    bReturn = recoverDB_Refresh(wa, request, reply);
  } else if( sAction == PARAM_ACTION_VAL_SELRECTYPE ) {
    bReturn = recoverDB_SelectRecType(wa, request, reply);
  } else if( sAction == PARAM_ACTION_VAL_SELDATASAVE ) {
    bReturn = recoverDB_SelectDataSave(wa, request, reply);
  } else if( sAction == PARAM_ACTION_VAL_SELMEDIUM ) {
    bReturn = recoverDB_SelectMedium(wa, request, reply);
  } else if( sAction == PARAM_ACTION_VAL_COMMITLIST ) {
    bReturn = recoverDB_CommitList(wa, request, reply);
  } else if( sAction == PARAM_ACTION_VAL_RECSTATE ) {
    bReturn = recoverDB_State(wa, request, reply);
  } else {
    DBMWeb_TemplateMsgBox oMsgBox(wa, DBMWEB_TEMPLMSGBOX_INFO, "", "Web DBM service error (unknown action)!");
    oMsgBox.writePage(Tools_TemplateWriterWA(reply));
  } // end if

  return bReturn;
} // end DBMWeb_DBMWeb :: recoverDB

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: recoverDB_Refresh
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: recoverDB_Refresh ( sapdbwa_WebAgent    & wa,
                                                sapdbwa_HttpRequest & request,
                                                sapdbwa_HttpReply   & reply )
{
  SAPDB_Bool      bReturn = SAPDB_TRUE;
  DBMCli_String   sRecMode;
  DBMCli_String   sRecType;
  DBMCli_String   sMedType;
  DBMCli_String   sCheck;
  DBMCli_String   sUntil;
  Msg_List  oMsgList;

  GetParameterValue(PARAM_RECMODE, request, sRecMode);
  GetParameterValue(PARAM_RECTYPE, request, sRecType);
  GetParameterValue(PARAM_MEDTYPE, request, sMedType);
  GetParameterValue(PARAM_RECUNTIL, request, sUntil);
  GetParameterValue(PARAM_CHECK, request, sCheck);

  DBMCli_DateTime oUntil(sUntil, DBMCLI_DT_NUM_DATETIME_FMT);

  if (m_Database->NodeInfo().Refresh( oMsgList )) {
    if (m_Database->GetConfig().GetValue(DBMCLI_CFGKEY_WEBDBMRECOVERY, sRecMode, oMsgList)) {
      if( sRecMode.IsEmpty() ) {
        sRecMode = PARAM_RECMODE_VAL_RECOVER;
      }

      bReturn = recoverDB_ViewDialog(wa, request, reply, PARAM_ACTION_VAL_SELRECTYPE, sRecMode, sRecType, sMedType, oUntil, sCheck);
    } else {
      sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
    }
   } else {
    sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
  } 

  return bReturn;
} // end DBMWeb_DBMWeb :: recoverDB_Refresh

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: recoverDB_SelectRecType
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: recoverDB_SelectRecType ( sapdbwa_WebAgent    & wa,
                                                      sapdbwa_HttpRequest & request,
                                                      sapdbwa_HttpReply   & reply )
{
  SAPDB_Bool            bReturn = SAPDB_TRUE;
  Msg_List  oMsgList;
  DBMCli_String         sRecMode;
  DBMCli_String         sRecType;
  DBMCli_String         sMedType;
  DBMCli_String         sCheck;
  DBMCli_String         sUntil;
  DBMCli_String         sUntilDate;
  DBMCli_String         sUntilTime;

  GetParameterValue(PARAM_RECMODE,     request, sRecMode);
  if ( sRecMode != PARAM_RECMODE_VAL_INSTALL ) {
    sRecMode.Empty();
    GetParameterValue(PARAM_CHKRECMODE,  request, sRecMode);
  }
  GetParameterValue(PARAM_RADRECTYPE,  request, sRecType);
  GetParameterValue(PARAM_MEDTYPE,     request, sMedType);
  GetParameterValue(PARAM_CHKRECUNTIL, request, sUntil);
  GetParameterValue(PARAM_UNTILDATE,   request, sUntilDate);
  GetParameterValue(PARAM_UNTILTIME,   request, sUntilTime);
  GetParameterValue(PARAM_CHECK,       request, sCheck);

  DBMCli_DateTime oUntil;
  if (sUntil == PARAM_CHKRECUNTIL_VAL_ON) {
    oUntil.Set(sUntilDate, sUntilTime);
  } // end if

  if (sRecMode.IsEmpty()) {
    sRecMode = PARAM_RECMODE_VAL_RECOVER;
  }

  if (sRecType == PARAM_RECTYPE_VAL_LAST) {

    if (m_Database->GetHistory( ).Refresh(DBMCli_History::Mode_RestLast, oUntil, oMsgList)) {
      // init history items for recovers
      m_Database->GetHistory( ).InitForRecovery();
      // show template
      bReturn = recoverDB_ViewDialog(wa, request, reply, PARAM_ACTION_VAL_COMMITLIST, sRecMode, sRecType, sMedType, oUntil, sCheck);   
    } else {
      // show error
      sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
    } // end if

  } else if (sRecType == PARAM_RECTYPE_VAL_SPECIFIC) {

    if (m_Database->GetHistory( ).Refresh(DBMCli_History::Mode_RestAll, oUntil, oMsgList)) {
      // show template
      bReturn = recoverDB_ViewDialog(wa, request, reply, PARAM_ACTION_VAL_SELDATASAVE, sRecMode, sRecType, sMedType, oUntil, sCheck);
    } else {
      // show error
      sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
    } // end if

  } else if (sRecType == PARAM_RECTYPE_VAL_MEDIUM) {

    if( m_Database->GetMedia( ).Refresh( oMsgList ) ) {
      // show template
      bReturn = recoverDB_ViewDialog(wa, request, reply, PARAM_ACTION_VAL_SELMEDIUM, sRecMode, sRecType, sMedType, oUntil, sCheck);
    } else {
      // show error
      sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
    } // end if

  } else if (sRecType == PARAM_RECTYPE_VAL_CONTINUE) {

    if (m_Database->GetHistory( ).Refresh(DBMCli_History::Mode_RestContinue, oUntil, oMsgList)) {
      // init history items for recovers
      m_Database->GetHistory( ).InitForRecovery();
      // show template
      bReturn = recoverDB_ViewDialog(wa, request, reply, PARAM_ACTION_VAL_COMMITLIST, sRecMode, sRecType, sMedType, oUntil, sCheck);   
    } else {
      // show error
      sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
    } // end if

  } // end if

  return bReturn;
} // end DBMWeb_DBMWeb :: recoverDB_SelectRecType

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: recoverDB_SelectDataSave
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: recoverDB_SelectDataSave ( sapdbwa_WebAgent    & wa,
                                                       sapdbwa_HttpRequest & request,
                                                       sapdbwa_HttpReply   & reply )
{
  SAPDB_Bool            bReturn = SAPDB_TRUE;
  Msg_List  oMsgList;
  DBMCli_String         sButton;
  DBMCli_String         sUntil;
  DBMCli_String         sRecMode;
  DBMCli_String         sRecType;
  DBMCli_String         sMedType;
  DBMCli_String         sCheck;

  GetParameterValue(PARAM_RECMODE, request, sRecMode);
  GetParameterValue(PARAM_BUTTON,   request, sButton);
  GetParameterValue(PARAM_RECUNTIL, request, sUntil);
  GetParameterValue(PARAM_RECTYPE,  request, sRecType);
  GetParameterValue(PARAM_MEDTYPE,  request, sMedType);
  GetParameterValue(PARAM_CHECK,    request, sCheck);

  DBMCli_DateTime oUntil(sUntil, DBMCLI_DT_NUM_DATETIME_FMT);

  if (sButton == PARAM_BUTTON_VAL_OK) {
    DBMCli_String         sKey;

    GetParameterValue(PARAM_KEY, request, sKey);

    if (m_Database->GetHistory( ).Refresh(DBMCli_History::Mode_RestSelected, sKey, oUntil, oMsgList)) {
      // init history items for recovers
      m_Database->GetHistory( ).InitForRecovery();
      // show template
      bReturn = recoverDB_ViewDialog(wa, request, reply, PARAM_ACTION_VAL_COMMITLIST, sRecMode, sRecType, sMedType, oUntil, sCheck);
    } else {
      // show error
      sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
    } // end if

  } else if (sButton == PARAM_BUTTON_VAL_BACK) {

    // show template
    bReturn = recoverDB_ViewDialog(wa, request, reply, PARAM_ACTION_VAL_SELRECTYPE, sRecMode, sRecType, sMedType, oUntil, sCheck);

  } else {
    DBMWeb_TemplateMsgBox oMsgBox(wa, DBMWEB_TEMPLMSGBOX_INFO, "", "Web DBM service error (unknown action)!");
    oMsgBox.writePage(Tools_TemplateWriterWA(reply));
  } // end if

  return bReturn;
} // end DBMWeb_DBMWeb :: recoverDB_SelectDataSave

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: recoverDB_SelectMedium
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: recoverDB_SelectMedium ( sapdbwa_WebAgent    & wa,
                                                     sapdbwa_HttpRequest & request,
                                                     sapdbwa_HttpReply   & reply )
{
  SAPDB_Bool            bReturn = SAPDB_TRUE;
  Msg_List  oMsgList;
  DBMCli_String         sButton;
  DBMCli_String         sUntil;
  DBMCli_String         sRecMode;
  DBMCli_String         sRecType;
  DBMCli_String         sMedType;
  DBMCli_String         sCheck;

  GetParameterValue(PARAM_RECMODE,  request, sRecMode);
  GetParameterValue(PARAM_BUTTON,   request, sButton);
  GetParameterValue(PARAM_RECUNTIL, request, sUntil);
  GetParameterValue(PARAM_RECTYPE,  request, sRecType);
  GetParameterValue(PARAM_MEDTYPE,  request, sMedType);
  GetParameterValue(PARAM_CHECK,    request, sCheck);

  DBMCli_DateTime oUntil(sUntil, DBMCLI_DT_NUM_DATETIME_FMT);

  if (sButton == PARAM_BUTTON_VAL_SINGLE) {

    // show template with the selected 
    bReturn = recoverDB_ViewDialog(wa, request, reply, PARAM_ACTION_VAL_SELMEDIUM, sRecMode, sRecType, PARAM_MEDIUMTYPE_VAL_SINGLE, oUntil, sCheck);

  } else if(sButton == PARAM_BUTTON_VAL_PARALLEL) {

    // show template with the selected 
    bReturn = recoverDB_ViewDialog(wa, request, reply, PARAM_ACTION_VAL_SELMEDIUM, sRecMode, sRecType, PARAM_MEDIUMTYPE_VAL_PARALLEL, oUntil, sCheck);

  } else if (sButton == PARAM_BUTTON_VAL_OK) {

    DBMCli_String    sMedium;
    SAPDB_Int        nMedium;
    DBMCli_Recover & oRecover  = m_Database->GetRecover( );
    SAPDB_Bool       bActivate;

    GetParameterValue(PARAM_KEY, request, sMedium);
    nMedium = atoi(sMedium);

    bActivate = false;
    if ((sRecMode == PARAM_RECMODE_VAL_ACTIVATE) ||    
        (sRecMode == PARAM_RECMODE_VAL_INSTALL)     ) {
          bActivate = true;
    }

    if (sMedType == PARAM_MEDIUMTYPE_VAL_SINGLE) {
      oRecover.PrepareRecover(m_Database->GetMedia().MediumArray()[nMedium],
                              oUntil, 
                              (sCheck == PARAM_CHECK_VAL_CHECKON),
                              bActivate);
    } else if (sMedType == PARAM_MEDIUMTYPE_VAL_PARALLEL) {
      oRecover.PrepareRecover(m_Database->GetMedia().MediumParallelArray()[nMedium],
                              oUntil, 
                              (sCheck == PARAM_CHECK_VAL_CHECKON),
                              bActivate);
    } // end if
    bReturn = recoverDB_ViewDialog(wa, request, reply, PARAM_ACTION_VAL_RECSTATE, sRecMode, sRecType, sMedType, oUntil, sCheck);

  } else if (sButton == PARAM_BUTTON_VAL_BACK) {
    // show template
    bReturn = recoverDB_ViewDialog(wa, request, reply, PARAM_ACTION_VAL_SELRECTYPE, sRecMode, sRecType, sMedType, oUntil, sCheck);
  } else {
    DBMWeb_TemplateMsgBox oMsgBox(wa, DBMWEB_TEMPLMSGBOX_INFO, "", "Web DBM service error (unknown action)!");
    oMsgBox.writePage(Tools_TemplateWriterWA(reply));
  } // end if

  return bReturn;
} // end DBMWeb_DBMWeb :: recoverDB_SelectMedium

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: recoverDB_CommitList
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: recoverDB_CommitList  ( sapdbwa_WebAgent    & wa,
                                                    sapdbwa_HttpRequest & request,
                                                    sapdbwa_HttpReply   & reply )
{
  SAPDB_Bool            bReturn = SAPDB_TRUE;
  Msg_List  oMsgList;
  DBMCli_String         sButton;
  DBMCli_String         sRecMode;
  DBMCli_String         sRecType;
  DBMCli_String         sMedType;
  DBMCli_String         sUntil;
  DBMCli_String         sCheck;

  GetParameterValue(PARAM_RECMODE,  request, sRecMode);
  GetParameterValue(PARAM_BUTTON,   request, sButton);
  GetParameterValue(PARAM_RECUNTIL, request, sUntil);
  GetParameterValue(PARAM_RECTYPE,  request, sRecType);
  GetParameterValue(PARAM_MEDTYPE,  request, sMedType);
  GetParameterValue(PARAM_CHECK,    request, sCheck);

  DBMCli_DateTime oUntil(sUntil, DBMCLI_DT_NUM_DATETIME_FMT);

  if (sButton == PARAM_BUTTON_VAL_OK) {

    DBMCli_History & oHistory  = m_Database->GetHistory( );
    DBMCli_Recover & oRecover  = m_Database->GetRecover( );
    SAPDB_Int      nItem    = 0;
    SAPDB_Int      nParam   = 0;
    DBMCli_String  sActive;
    SAPDB_Bool     bActivate;

    // mark the selected history items
    for (nItem = 0; nItem  < oHistory.HistoryItemArray().GetSize(); ++nItem ) {
      GetParameterValueByIndex(PARAM_CHKISACTIVE, nParam, request, sActive);
      if (sActive == oHistory.HistoryItemArray()[nItem].SysKeyStr()) {
        oHistory.HistoryItemArray()[nItem].ActivateForRecovery(true);
        ++nParam;
      } else {
        oHistory.HistoryItemArray()[nItem].ActivateForRecovery(false);
      } // end if
      sActive.Empty( );
    } // end if

    bActivate = false;
    if ((sRecMode == PARAM_RECMODE_VAL_ACTIVATE) ||    
        (sRecMode == PARAM_RECMODE_VAL_INSTALL)     ) {
          bActivate = true;
    }

    // provide the list for starting backup
    oRecover.PrepareRecover(oHistory.HistoryItemArray(),
                            oUntil, 
                            (sCheck == PARAM_CHECK_VAL_CHECKON),
                            bActivate);

    // view the dialog
    bReturn = recoverDB_ViewDialog(wa, request, reply, PARAM_ACTION_VAL_RECSTATE, sRecMode, sRecType, sMedType, oUntil, sCheck);

  } else if (sButton == PARAM_BUTTON_VAL_BACK) {

    if ((sRecType == PARAM_RECTYPE_VAL_LAST) || (sRecType == PARAM_RECTYPE_VAL_CONTINUE)) {
      // show template
      bReturn = recoverDB_ViewDialog(wa, request, reply, PARAM_ACTION_VAL_SELRECTYPE, sRecMode, sRecType, sMedType, oUntil, sCheck);
    } else if (sRecType == PARAM_RECTYPE_VAL_SPECIFIC) {
      if (m_Database->GetHistory( ).Refresh(DBMCli_History::Mode_RestAll, oUntil, oMsgList)) {
        // show template
        bReturn = recoverDB_ViewDialog(wa, request, reply, PARAM_ACTION_VAL_SELDATASAVE, sRecMode, sRecType, sMedType, oUntil, sCheck);
      } else {
        // show error
        sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
      } // end if
    } else if (sRecType == PARAM_RECTYPE_VAL_CONTINUE) {
      // ???
    } // end if

  } else {
    DBMWeb_TemplateMsgBox oMsgBox(wa, DBMWEB_TEMPLMSGBOX_INFO, "", "Web DBM service error (unknown action)!");
    oMsgBox.writePage(Tools_TemplateWriterWA(reply));
  } // end if

  return bReturn;
} // end DBMWeb_DBMWeb :: recoverDB_CommitList

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: recoverDB_State
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: recoverDB_State  ( sapdbwa_WebAgent    & wa,
                                               sapdbwa_HttpRequest & request,
                                               sapdbwa_HttpReply   & reply )
{
  SAPDB_Bool            bReturn = SAPDB_TRUE;
  Msg_List  oMsgList;
  DBMCli_String         sButton;
  DBMCli_String         sRecMode;
  DBMCli_String         sRecType;
  DBMCli_String         sMedType;
  DBMCli_String         sUntil;
  DBMCli_String         sCheck;
  SAPDB_Bool            bSuccess;
  SAPDB_Bool            bAvailable;

  GetParameterValue(PARAM_RECMODE,  request, sRecMode);
  GetParameterValue(PARAM_BUTTON,   request, sButton);
  GetParameterValue(PARAM_RECUNTIL, request, sUntil);
  GetParameterValue(PARAM_RECTYPE,  request, sRecType);
  GetParameterValue(PARAM_MEDTYPE,  request, sMedType);
  GetParameterValue(PARAM_CHECK,    request, sCheck);

  DBMCli_String sErrTitle = "Error - " + m_Database->DatabaseName();
  DBMCli_String sErrAction = "javascript:parent.GotoWebDBMURL(this, \\'Event=RECOVER_DB";
  sErrAction = sErrAction + "&" + PARAM_ACTION   + "=" + PARAM_ACTION_VAL_REFRESH;
  sErrAction = sErrAction + "&" + PARAM_RECUNTIL + "=" + sUntil;
  sErrAction = sErrAction + "&" + PARAM_RECTYPE  + "=" + sRecType;
  sErrAction = sErrAction + "&" + PARAM_MEDTYPE  + "=" + sMedType;
  sErrAction = sErrAction + "&" + PARAM_CHECK    + "=" + sCheck + "\\')";

  DBMCli_DateTime oUntil(sUntil, DBMCLI_DT_NUM_DATETIME_FMT);

  // get information about state
  DBMCli_Recover               & oRecover   = m_Database->GetRecover( );
  SAPDB_Int                      nCurrent   = oRecover.CurrentItem();
  DBMCli_Recover::RecoverState   nState     = oRecover.GetRecoverState();
  SAPDB_Bool                     bCheck     = oRecover.IsCheck();
  DBMCli_Recover::MediaInfo      nMediaInfo = oRecover.AskForMediaInfo();

  if (nState == DBMCli_Recover::RecoverStateRunning) {
    sButton = PARAM_BUTTON_VAL_STATE;
  } // end if

  if ( (sButton == PARAM_BUTTON_VAL_START   ) ||
       (sButton == PARAM_BUTTON_VAL_CONTINUE)    ) {

    // **** Button START/CONTINUE pressed

    if (nMediaInfo != DBMCli_Recover::MediaInfoNone) {
      // read location/version
      DBMCli_String sLocation;
      DBMCli_String sVersion;

      GetParameterValue(PARAM_LOCATION, request, sLocation);
      GetParameterValue(PARAM_VERSION,  request, sVersion);

      if (oRecover.RecoverItemArray()[nCurrent].GetType() == DBMCli_RecoverItem::TypeLog) {
        if (sVersion.GetLength() > 0) {
          sVersion.Format("%03d", atoi(sVersion));
        } else {
          sVersion.Format("%03d", atoi(oRecover.RecoverItemArray()[nCurrent].GetVersion(0)));
        } // end if
        sLocation = sLocation + "." + sVersion;
      } // end if

      // update medium
      oRecover.RecoverItemArray()[nCurrent].SetLocation(sLocation);
    } // end if

    // start recovery
    if (oRecover.RecoverNextRequest(oMsgList)) {
      bReturn = recoverDB_ViewDialog(wa, request, reply, PARAM_ACTION_VAL_RECSTATE, sRecMode, sRecType, sMedType, oUntil, sCheck);
    } else {
      // show error
      DBMWeb_TemplateMsgBox oMsgBox(wa, DBMWEB_TEMPLMSGBOX_ERROR, oMsgList, sErrTitle);
      oMsgBox.SetButtonAction(sErrAction).writePage(Tools_TemplateWriterWA(reply));
    } // end if

  } else if (sButton == PARAM_BUTTON_VAL_IGNORE) {

    // **** Button IGNORE pressed

    // update action
    oRecover.RecoverItemArray()[nCurrent].SetAction(DBMCli_RecoverItem::ActionIgnore);
    // contiunue recovery
    if (oRecover.RecoverNextRequest(oMsgList)) {
      bReturn = recoverDB_ViewDialog(wa, request, reply, PARAM_ACTION_VAL_RECSTATE, sRecMode, sRecType, sMedType, oUntil, sCheck);
    } else {
      // show error
      DBMWeb_TemplateMsgBox oMsgBox(wa, DBMWEB_TEMPLMSGBOX_ERROR, oMsgList, sErrTitle);
      oMsgBox.SetButtonAction(sErrAction).writePage(Tools_TemplateWriterWA(reply));
    } // end if

  } else if (sButton == PARAM_BUTTON_VAL_CANCEL) {

    // **** Button CANCEL pressed

    // update action
    oRecover.RecoverItemArray()[nCurrent].SetAction(DBMCli_RecoverItem::ActionCancel);
    // contiunue recovery
    if (oRecover.RecoverNextRequest(oMsgList)) {
      bReturn = recoverDB_ViewDialog(wa, request, reply, PARAM_ACTION_VAL_RECSTATE, sRecMode, sRecType, sMedType, oUntil, sCheck);
    } else {
      // show error
      DBMWeb_TemplateMsgBox oMsgBox(wa, DBMWEB_TEMPLMSGBOX_ERROR, oMsgList, sErrTitle);
      oMsgBox.SetButtonAction(sErrAction).writePage(Tools_TemplateWriterWA(reply));
    } // end if

  } else if (sButton == PARAM_BUTTON_VAL_RESTART) {

    // **** Button RESTART pressed

    if (oRecover.Restart(oMsgList)) {
      bReturn = recoverDB_ViewDialog(wa, request, reply, PARAM_ACTION_VAL_RECSTATE, sRecMode, sRecType, sMedType, oUntil, sCheck);
    } else {
      // show error
      DBMWeb_TemplateMsgBox oMsgBox(wa, DBMWEB_TEMPLMSGBOX_ERROR, oMsgList, sErrTitle);
      oMsgBox.SetButtonAction(sErrAction).writePage(Tools_TemplateWriterWA(reply));
    } // end if
  } else if (sButton == PARAM_BUTTON_VAL_STATE) {

    // **** (virtuell) Button STATE pressed
    sCheck = oRecover.IsCheck() ? PARAM_CHECK_VAL_CHECKON : "";
    oUntil = oRecover.GetUntil();

    // check for reply
    bSuccess = oRecover.ReplyAvailable(bAvailable, oMsgList);
    if (bSuccess) {
      if (bAvailable) {
        // receive reply
        bSuccess = oRecover.ReplyReceive (oMsgList);
      } else {
        bSuccess = oRecover.State (oMsgList);
      } // end if
    } // end if

    // send answer
    if (bSuccess) {
      bReturn = recoverDB_ViewDialog(wa, request, reply, PARAM_ACTION_VAL_RECSTATE, sRecMode, sRecType, sMedType, oUntil, sCheck);
    } else {
      // show error
      DBMWeb_TemplateMsgBox oMsgBox(wa, DBMWEB_TEMPLMSGBOX_ERROR, oMsgList, sErrTitle);
      oMsgBox.SetButtonAction(sErrAction).writePage(Tools_TemplateWriterWA(reply));
    } // end if

  } else if (sButton == PARAM_BUTTON_VAL_BACK) {

    if (sRecType == PARAM_RECTYPE_VAL_MEDIUM) {
      bReturn = recoverDB_ViewDialog(wa, request, reply, PARAM_ACTION_VAL_SELMEDIUM, sRecMode, sRecType, sMedType, oUntil, sCheck);
    } else {
      bReturn = recoverDB_ViewDialog(wa, request, reply, PARAM_ACTION_VAL_COMMITLIST, sRecMode, sRecType, sMedType, oUntil, sCheck);
    } // end if

  } else {
    DBMWeb_TemplateMsgBox oMsgBox(wa, DBMWEB_TEMPLMSGBOX_INFO, "", "Web DBM service error (unknown action)!");
    oMsgBox.SetButtonAction("WARoot/HTML/DBMEmpty.htm")
           .writePage(Tools_TemplateWriterWA(reply));
  } // end if
 
  return bReturn;
} // end DBMWeb_DBMWeb :: recoverDB_State

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: recoverDB_ViewDialog
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: recoverDB_ViewDialog  ( sapdbwa_WebAgent      & wa,
                                                    sapdbwa_HttpRequest   & request,
                                                    sapdbwa_HttpReply     & reply,
                                                    const DBMCli_String   & sDialog,
                                                    const DBMCli_String   & sRecMode,
                                                    const DBMCli_String   & sRecType,
                                                    const DBMCli_String   & sMedType,
                                                    const DBMCli_DateTime & oUntil,
                                                    const DBMCli_String   & sCheck)
{

  if( sDialog == PARAM_ACTION_VAL_SELRECTYPE ) {
    // select recovery type
    DBMWeb_TemplateRecovery oTemplate( wa,
                                       DBMWeb_TemplateRecovery::MODE_SELRECTYPE,
                                       PARAM_ACTION_VAL_SELRECTYPE,
                                       m_Database );
    oTemplate.SetRecMode(sRecMode)
             .SetRecType(sRecType)
             .SetMedType(sMedType)
             .SetUntil(oUntil)
             .SetCheck(sCheck)
             .writePage( Tools_TemplateWriterWA( reply ) );

  } else if( sDialog == PARAM_ACTION_VAL_SELDATASAVE ) {
    // select data save from data save list
    DBMWeb_TemplateRecovery oTemplate( wa,
                                       DBMWeb_TemplateRecovery::MODE_SELDATASAVE,
                                       PARAM_ACTION_VAL_SELDATASAVE,
                                       m_Database );
    oTemplate.SetRecMode(sRecMode)
             .SetRecType(sRecType)
             .SetMedType(sMedType)
             .SetUntil(oUntil)
             .SetCheck(sCheck)
             .writePage( Tools_TemplateWriterWA( reply ) );
  
  } else if( sDialog == PARAM_ACTION_VAL_SELMEDIUM ) {
    // select medium from media list
    DBMWeb_TemplateRecovery oTemplate( wa,
                                       DBMWeb_TemplateRecovery::MODE_SELMEDIUM,
                                       PARAM_ACTION_VAL_SELMEDIUM,
                                       m_Database );
    oTemplate.SetRecMode(sRecMode)
             .SetRecType(sRecType)
             .SetMedType(sMedType)
             .SetUntil(oUntil)
             .SetCheck(sCheck)
             .writePage( Tools_TemplateWriterWA( reply ) );
  
  } else if( sDialog == PARAM_ACTION_VAL_COMMITLIST ) {
    // select log or pages for a history recovery
    DBMWeb_TemplateRecovery oTemplate( wa,
                                       DBMWeb_TemplateRecovery::MODE_COMMITLIST,
                                       PARAM_ACTION_VAL_COMMITLIST,
                                       m_Database );

    oTemplate.SetRecMode(sRecMode)
             .SetRecType(sRecType)
             .SetMedType(sMedType)
             .SetUntil(oUntil)
             .SetCheck(sCheck)
             .writePage( Tools_TemplateWriterWA( reply ) );
 
  } else if( sDialog == PARAM_ACTION_VAL_RECSTATE ) {
    // show the current recovery list and ask for action or start automatic
    // action
    DBMWeb_TemplateRecovery oTemplate( wa,
                                       DBMWeb_TemplateRecovery::MODE_RECSTATE,
                                       PARAM_ACTION_VAL_RECSTATE,
                                       m_Database );

    oTemplate.SetRecMode(sRecMode)
             .SetRecType(sRecType)
             .SetMedType(sMedType)
             .SetUntil(oUntil)
             .SetCheck(sCheck)
             .writePage( Tools_TemplateWriterWA( reply ) );
  } // end if

  return SAPDB_TRUE;
} // end DBMWeb_DBMWeb :: recoverDB_ViewDialog

