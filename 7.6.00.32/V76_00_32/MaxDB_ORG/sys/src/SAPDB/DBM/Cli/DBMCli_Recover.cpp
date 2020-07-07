/*!
  -----------------------------------------------------------------------------
  module: DBMCli_Recover.hpp
  -----------------------------------------------------------------------------
  responsible:
  special area: Database Manager Client Library
  description:  Recover Class
  version:
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
#include "DBM/Cli/DBMCli_Recover.hpp"
#include "DBM/Cli/DBMCli_Database.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define PROPVAL_SEP      "\t"
#define PROP_USED        "Used LOG Page"
#define PROP_FIRST       "First LOG Page"
#define PROP_RESTARTABLE "Restartable"
#define VAL_RESTARTABLE  "1"
#define VAL_INVALID      "********"

/*
  =============================================================================
  class: DBMCli_RecoverItem
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_RecoverItem :: DBMCli_RecoverItem
  -----------------------------------------------------------------------------
*/
DBMCli_RecoverItem :: DBMCli_RecoverItem ( RecoverAction                 aAction,
                                           RecoverType                   aType,
                                           const DBMCli_String         & sLabel,
                                           const DBMCli_String         & sMediumName,
                                           const LocationArray         & oLocations,
                                           const DBMCli_String         & sEBID,
                                           DBMCli_MediumDeviceTypeEnum   aDeviceType)
                 : m_aAction     (aAction),
                      m_aType       (aType),
                      m_sLabel      (sLabel),
                      m_sMediumName (sMediumName),
                      m_oLocations  (oLocations),
                      m_sEBID       (sEBID),
                      m_aDeviceType (aDeviceType)
{
} // end DBMCli_RecoverItem :: DBMCli_RecoverItem
/*
  -----------------------------------------------------------------------------
*/
DBMCli_RecoverItem :: DBMCli_RecoverItem ( )
{
} // end DBMCli_RecoverItem :: DBMCli_RecoverItem

/*
  -------------------------------------------------------------------------
  public function DBMCli_RecoverItem :: GetTypeStr
  -------------------------------------------------------------------------
*/
DBMCli_String DBMCli_RecoverItem :: GetTypeStr () const
{
  DBMCli_String sType;

  switch (m_aType) {
    case TypeData:
      sType = DBMCLI_OPT_RECOVER_DATA;
      break;
    case TypePages:
      sType = DBMCLI_OPT_RECOVER_PAGES;
      break;
    case TypeLog:
      sType = DBMCLI_OPT_RECOVER_LOG;
      break;
  } // end switch

  return sType;
} // end DBMCli_RecoverItem :: GetTypeStr

/*
  -------------------------------------------------------------------------
  public function DBMCli_RecoverItem :: GetLocationName
  -------------------------------------------------------------------------
*/
DBMCli_String DBMCli_RecoverItem :: GetLocationName (SAPDB_Int nRow) const
{
  DBMCli_String sLocation = GetLocation(nRow);
  SAPDB_Int     nLength   = sLocation.GetLength();
  DBMCli_String sLocationName;

  if (GetVersion(nRow).GetLength() > 0) {
    sLocationName = sLocation.Left(nLength - 4);
  } else {
    sLocationName = GetLocation(nRow);
  } // end if

  return sLocationName;
} // end DBMCli_RecoverItem :: GetLocationName

/*!
  -------------------------------------------------------------------------
  public function DBMCli_RecoverItem :: GetVersion
  -------------------------------------------------------------------------
*/
DBMCli_String DBMCli_RecoverItem :: GetVersion (SAPDB_Int nRow) const
{
  DBMCli_String sLocation = GetLocation(nRow);
  SAPDB_Int     nLength   = sLocation.GetLength();
  DBMCli_String sVersion;

  if (nLength > 4) {
    if ((isdigit(sLocation[nLength - 1]) != 0  ) &&
        (isdigit(sLocation[nLength - 2]) != 0  ) &&
        (isdigit(sLocation[nLength - 3]) != 0  ) &&
        (sLocation[nLength - 4]          == '.')    ) {
      sVersion = sLocation.Right (3);
    } // end if
  } // end if

  return sVersion;
} // end DBMCli_RecoverItem :: GetVersion

/*
  =============================================================================
  class: DBMCli_Recover
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_Recover :: DBMCli_Recover
  -----------------------------------------------------------------------------
*/
DBMCli_Recover :: DBMCli_Recover ( )
{

} // end DBMCli_Recover :: DBMCli_Recover

/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_Recover :: ~DBMCli_Recover
  -----------------------------------------------------------------------------
*/
DBMCli_Recover :: ~DBMCli_Recover ( )
{
  m_aItems.RemoveAll();
} // end DBMCli_Recover :: ~DBMCli_Recover

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Recover :: ReplyAvailable
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Recover :: ReplyAvailable ( SAPDB_Bool & bAvailable, Msg_List & oMsgList )
{
  SAPDB_Bool        bRC     = true;

  bAvailable = false;
  if( GetDatabase().Execute( DBMCLI_CMD_RECOVER_REPLY_AVAIL, oMsgList ) ) {
    bAvailable = true;
  } else {
    if (GetDatabase().GetResult().ReturnCode() != DBMCLI_SRV_RC_NOREPLY) {
      bRC = false;
    } else {
      oMsgList.ClearMessageList();
    } // end if
  } // end if

  return bRC;
} // end DBMCli_Recover :: ReplyAvailable

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Recover :: ReplyReceive
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Recover :: ReplyReceive ( Msg_List & oMsgList )
{
  SAPDB_Bool             bRC     = false;
  DBMCli_Database      & oDB     = GetDatabase();
  DBMCli_Result        & oResult = oDB.GetResult( ); 
  DBMCli_String          sCmd( DBMCLI_CMD_RECOVER_REPLY_RECEIVE );
  Msg_List   oRelease;

  m_nLastDBReturn = 0;
  m_sLastDBTxt.Empty();

  if( oDB.Execute( sCmd, oMsgList ) ) {
    m_oResult.SetByResultBuf( oResult ); 
    bRC = true;
    m_RecoverState = RecoverStateAsking;

    // look what was happened
    if (m_aItems[m_nCurrent].GetAction() == DBMCli_RecoverItem::ActionCancel) {
      --m_nCurrent;
      m_RecoverState = RecoverStateCancelled;
    } else if (m_aItems[m_nCurrent].GetAction() == DBMCli_RecoverItem::ActionIgnore) {
      // remove item
      m_aItems.RemoveAt(m_nCurrent);
      --m_nCurrent;
      --m_nIgnoreCount;
    } // end if

    if (!m_oResult.IsFull() && ! m_oResult.IsSkipped()) {
      Release(oDB, m_bCheck, oRelease );

      if (m_aItems[m_nCurrent].GetType()   == DBMCli_RecoverItem::TypeLog) {
        // maybe the database is warm?
        Msg_List oTmpMsgList;
        oDB.GetState().Refresh(oTmpMsgList);
        if (oDB.GetState().Value() == DBMCLI_DBSTATE_WARM) {
          m_RecoverState = RecoverStateRestarted;
        } // end if
      } else {
        oDB.GetRestartInfo().Refresh();
      } // end if

    } else {
      bRC = NextMediumRequired(oMsgList);
    } // end if

  } else {
    m_nLastDBReturn = oResult.ExtReturnCode();
    m_sLastDBTxt   = oResult.ExtErrorText();
    if (! ( (oResult.ReturnCode()     == DBMCLI_SRV_RC_SQL    ) &&
            (oResult.ExtReturnCode()  == DBMCLI_DB_RC_INVFILE  || oDB.GetResult().ExtReturnCode()  == DBMCLI_DB_RC_INVFILE2 )    ) ) {
      Msg_List  oTmpMsgList;
      Release(oDB, m_bCheck, oTmpMsgList );
      m_RecoverState = RecoverStateError;
    } else {
      if ((m_aItems[m_nCurrent].GetAction() == DBMCli_RecoverItem::ActionReplace) ||
          (m_aItems[m_nCurrent].GetAction() == DBMCli_RecoverItem::ActionIgnore )    ) {
        m_RecoverState = RecoverStateReplace;
        m_aItems[m_nCurrent].SetAction(DBMCli_RecoverItem::ActionReplace);
        --m_nCurrent;
        oMsgList.ClearMessageList();
        bRC = true;
      } else {
        Msg_List  oTmpMsgList;
        Release(oDB, m_bCheck, oTmpMsgList );
        m_RecoverState = RecoverStateError;
      } // end if
    } // end if
  } // end if

  if (bRC) {
    ++m_nCurrent;
    if ((m_nCurrent >= m_aItems.GetSize()) && (m_RecoverState != RecoverStateRestarted)) {
      m_RecoverState = RecoverStateClosed;
    } // end if
  } // end if

  return bRC;
} // end DBMCli_Recover :: ReplyReceive

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Recover :: NextMediumRequired
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Recover :: NextMediumRequired( Msg_List & oMsgList)
{
  SAPDB_Bool bRc = true;

  DBMCli_Database & oDB     = GetDatabase();

  if (m_RecoverMode == RecoverModeHistory) {

    if ( ( (m_nCurrent + 1)                     >= m_aItems.GetSize()                ) ||
         ( m_aItems[m_nCurrent + 1].GetAction() != DBMCli_RecoverItem::ActionReplace )    ) {
      // when restoring from history send a ignore if no more items to replace
      if( oDB.Execute( DBMCLI_CMD_RECOVER_IGNORE_REQ, oMsgList ) ) {
        --m_nCurrent;
        m_RecoverState = RecoverStateRunning;
      } else {
        Release(oDB, m_bCheck, oMsgList );
        bRc = false;
      } // end if
    } else if ( ( (m_nCurrent + 1)                         <  m_aItems.GetSize()                ) &&
                ( m_aItems[m_nCurrent + 1].GetAction()     == DBMCli_RecoverItem::ActionReplace ) &&
                ( m_aItems[m_nCurrent + 1].GetDeviceType() != DBMCLI_MEDIUMDEVICETYPE_TAPE      )    ) {
        
       // do a autoreplace
       ++m_nCurrent;
       bRc = RecoverNextRequest(oMsgList);
       --m_nCurrent;
    } else {
      m_RecoverState = RecoverStateReplace;
    } // end if

  } else {

    // copy the last item for a replace action
    CopyLastItem (DBMCli_RecoverItem::ActionReplace);
    m_RecoverState = RecoverStateReplace;

    // try a autoreplace in case of log restore
    if ( ( m_aItems[m_nCurrent].GetType()       == DBMCli_RecoverItem::TypeLog ) &&
         ( m_aItems[m_nCurrent].GetDeviceType() != DBMCLI_MEDIUMDEVICETYPE_TAPE)    ) {
      ++m_nCurrent;
      bRc = RecoverNextRequest(oMsgList);
      --m_nCurrent;
    } // end if
  } // end if

  return bRc;
} // end DBMCli_Recover :: NextMediumRequired

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Recover :: RecoverNextRequest
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Recover :: RecoverNextRequest ( Msg_List & oMsgList )
{
  SAPDB_Bool             bRc = false;
  DBMCli_Database      & oDB = GetDatabase();
  Msg_List   oRelease;
  DBMCli_String          sCmd;

  // recover_start
  if ((m_aItems[m_nCurrent].GetAction() == DBMCli_RecoverItem::ActionStart) || m_oResult.IsSkipped() ||
      ((m_aItems[m_nCurrent].GetType( ) == DBMCli_RecoverItem::TypeLog) && m_bCheck)) {

    m_nIgnoreCount    = m_aItems[m_nCurrent].LocationCount( );

    if (m_bCheck) {
      sCmd = DBMCLI_CMD_RECOVER_CHECK_REQ;
    } else {
      if ( (m_bActivate) && 
           (oDB.NodeInfo().Version().GetNumber() >= 0x7040400) ) {
          sCmd = DBMCLI_CMD_DBACTIVATE_REQ;
          sCmd += " ";
          sCmd += DBMCLI_OPT_DBACTIVATE_RECOVER;
      } else{
          sCmd = DBMCLI_CMD_RECOVER_START_REQ;
      }
    }

    sCmd += " \"";
    sCmd += m_aItems[m_nCurrent].GetMediumName();
    sCmd += "\" ";
    sCmd += m_aItems[m_nCurrent].GetTypeStr();

    // Attach EBID
    if (m_aItems[m_nCurrent].GetEBID().GetLength() > 0) {
      sCmd += " ";
      sCmd += DBMCLI_OPT_RECOVER_EBID;
      sCmd += " ";
      sCmd += m_aItems[m_nCurrent].GetEBID();
    } // end if

    // Attach Version Number
    if (m_aItems[m_nCurrent].GetType( ) == DBMCli_RecoverItem::TypeLog) {
      sCmd += " ";
      sCmd += m_aItems[m_nCurrent].GetVersion(0);
    } // end if

    // Attach Until
    if (!m_oUntil.IsNull() &&
         (m_aItems[m_nCurrent].GetAction( ) == DBMCli_RecoverItem::ActionStart) &&
         (m_aItems[m_nCurrent].GetType( ) == DBMCli_RecoverItem::TypeLog) ) {
      sCmd += " ";
      sCmd += DBMCLI_OPT_RECOVER_UNTIL;
      sCmd += " ";
      sCmd += m_oUntil.Date().Get(DBMCLI_DT_NUM_DATE_FMT);
      sCmd += " ";
      sCmd += m_oUntil.Time().Get(DBMCLI_DT_NUM_TIME_FMT);
    } // end if

    Release(oDB, m_bCheck, oRelease );
    if( Connect(oDB, m_bCheck, oMsgList ) ) {
      if( oDB.Execute( sCmd, oMsgList ) ) {
        m_RecoverState = RecoverStateRunning;
        bRc = true;
      } else {
        Release(oDB, m_bCheck, oMsgList );
      } // end if
    } // end if

  // recover_replace
  } else if (m_aItems[m_nCurrent].GetAction() == DBMCli_RecoverItem::ActionReplace) {

    sCmd = DBMCLI_CMD_RECOVER_REPLACE_REQ;

    // medium name
    sCmd += " \"";
    sCmd += m_aItems[m_nCurrent].GetMediumName();
    sCmd += "\" ";

    // Attach EBID
    if (m_aItems[m_nCurrent].GetEBID().GetLength() > 0) {
      sCmd += " ";
      sCmd += DBMCLI_OPT_RECOVER_EBID;
      sCmd += " ";
      sCmd += m_aItems[m_nCurrent].GetEBID();
    } // end if

    // Attach Location and Version
    if (m_aItems[m_nCurrent].GetType( ) == DBMCli_RecoverItem::TypeLog) {
      sCmd += " \"";
      sCmd += m_aItems[m_nCurrent].GetLocationName(0);
      sCmd += "\" ";
      sCmd += m_aItems[m_nCurrent].GetVersion(0);
    } else {
      sCmd += " \"";
      sCmd += m_aItems[m_nCurrent].GetLocation(0);
      sCmd += "\"";
    } // end if

    if( oDB.Execute( sCmd, oMsgList ) ) {
      m_RecoverState = RecoverStateRunning;
      bRc = true;
    } else {
      Release(oDB, m_bCheck, oRelease );
    } // end if

  // recover_ignore
  } else if (m_aItems[m_nCurrent].GetAction() == DBMCli_RecoverItem::ActionIgnore) {

    sCmd = DBMCLI_CMD_RECOVER_IGNORE_REQ;

    if( oDB.Execute( sCmd, oMsgList ) ) {
      m_RecoverState = RecoverStateRunning;
      bRc = true;
    } else {
      Release(oDB, m_bCheck, oRelease );
    } // end if

  // recover_cancel
  } else if (m_aItems[m_nCurrent].GetAction() == DBMCli_RecoverItem::ActionCancel) {

    Msg_List oTmpMsgList;

    sCmd = DBMCLI_CMD_RECOVER_CANCEL_REQ;

    if( oDB.Execute( sCmd, oMsgList ) ) {
      m_RecoverState = RecoverStateRunning;
      bRc = true;
    } else {
      Release(oDB, m_bCheck, oRelease );
    } // end if

  } // end if

  return bRc;
} // end DBMCli_Recover :: RecoverNextRequest

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Recover :: Restart
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Recover :: Restart (Msg_List & oMsgList)
{
  SAPDB_Bool bRc = false;

  if (!m_oUntil.IsNull()) {
    bRc = GetDatabase().Restart(m_oUntil, oMsgList);
  } else {
    bRc = GetDatabase().Restart(oMsgList);
  } // end if

  if (bRc) {
    m_RecoverState = RecoverStateRestarted;
    bRc = GetDatabase().GetConfig().PutRawValue(DBMCLI_CFGKEY_WEBDBMRECOVERY, "", oMsgList);
  } // end if

  return bRc;
} // end DBMCli_Recover :: Restart

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Recover :: State
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Recover :: State ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();
  DBMCli_Result & oResult = oDB.GetResult( ); 

  DBMCli_String sCmd( DBMCLI_CMD_RECOVER_STATE );

  if ( oDB.Execute( sCmd, oMsgList ) ) {
    DBMCli_BackupResult   oLocalResult;
    oLocalResult.SetByResultBuf( oResult ); 
    if (oLocalResult.TransferredPages() != 0) {
      m_oResult = oLocalResult; 
    } // end if
    bRC = true;
  } // end if

  return bRC;
} // DBMCli_Recover :: State

/*
  -----------------------------------------------------------------------------
  private function DBMCli_Recover :: Connect
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Recover :: Connect     ( DBMCli_Database      & oDB,
                                           SAPDB_Bool             bCheck,
                                           Msg_List & oMsgList) const
{
  SAPDB_Bool bRc = true;

  if (bCheck) {
    bRc = oDB.SRVConnect(oMsgList);
  } else {
    bRc = oDB.UTLConnect(oMsgList);
  } // end if

  return bRc;
} // end DBMCli_Recover :: Connect

/*
  -----------------------------------------------------------------------------
  private function DBMCli_Recover :: Release
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Recover :: Release     ( DBMCli_Database      & oDB,
                                           SAPDB_Bool             bCheck,
                                           Msg_List & oMsgList) const
{
  SAPDB_Bool bRc = true;

  if (bCheck) {
    bRc = oDB.SRVRelease(oMsgList);
  } else {
    bRc = oDB.UTLRelease(oMsgList);
  } // end if

  return bRc;
} // end DBMCli_Recover :: Release

/*
  -----------------------------------------------------------------------------
  private function DBMCli_Recover :: Init
  -----------------------------------------------------------------------------
*/
void DBMCli_Recover :: Init ( )
{
  m_RecoverMode     = RecoverModeUnknown;
  m_RecoverState    = RecoverStateUnknown;
  m_nCurrent           = 0;
  m_nLastDBReturn   = 0;
  m_sLastDBTxt      = "";

  m_aItems.RemoveAll();
  m_oResult.Clear();
} // end DBMCli_Recover :: Init

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Recover :: IgnorePossible
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Recover :: IgnorePossible ( ) const
{

  return ( (m_RecoverState == RecoverStateReplace) && 
           ((m_nIgnoreCount > 1) || ( RecoverItemArray()[m_nCurrent].GetType() == DBMCli_RecoverItem::TypeLog)) && 
           m_oResult.IsFull() &&
           (m_RecoverMode == RecoverModeMedium));

} // end DBMCli_Recover :: IgnorePossible

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Recover :: AskForMediaInfo
  -----------------------------------------------------------------------------
*/
DBMCli_Recover::MediaInfo DBMCli_Recover :: AskForMediaInfo ( ) const
{
  MediaInfo aMediaInfo = MediaInfoNone;

  if (m_RecoverMode == RecoverModeMedium) {
    if ( (m_RecoverState                            == RecoverStatePrepared       ) && 
         ( RecoverItemArray()[m_nCurrent].GetType() == DBMCli_RecoverItem::TypeLog)    ) {
      // ask for first log version
      aMediaInfo = MediaInfoFirstLog;
    } else if ( ( ( LastDBReturn() == DBMCLI_DB_RC_INVFILE  ) ||
                  ( LastDBReturn() == DBMCLI_DB_RC_INVFILE2 )    ) && 
                ( RecoverItemArray()[m_nCurrent].GetType() == DBMCli_RecoverItem::TypeLog) ) {
      // ask for next log location (after error)
      aMediaInfo = MediaInfoErrNextLog;
    } else if ( ( ( LastDBReturn() == DBMCLI_DB_RC_INVFILE  ) ||
                  ( LastDBReturn() == DBMCLI_DB_RC_INVFILE2 )    ) && 
                ( RecoverItemArray()[m_nCurrent].GetType() != DBMCli_RecoverItem::TypeLog) ) {
      // ask for next data location (after error)
      aMediaInfo = MediaInfoErrNextData;
    } else if (   m_pDatabase->GetRecover().Result().IsFull() &&
                ( RecoverItemArray()[m_nCurrent].GetType() != DBMCli_RecoverItem::TypeLog) ) {
      // ask for next data location 
      aMediaInfo = MediaInfoNextData;
    } // end if
  } // end if

  return aMediaInfo;
} // end DBMCli_Recover :: AskForMediaInfo

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Recover :: PrepareRecover
  -----------------------------------------------------------------------------
*/
void DBMCli_Recover :: PrepareRecover 
      ( const DBMCli_HistoryItemArray & oHistoryItems,
        const DBMCli_DateTime         & oUntil, 
        SAPDB_Bool                      bCheck,
        SAPDB_Bool                      bActivate)

{
  // clear members 
  Init();

  // set global information
  m_oUntil        = oUntil;
  m_bCheck        = bCheck;
  m_bActivate     = bActivate;
  m_RecoverMode   = RecoverModeHistory;
  m_RecoverState  = RecoverStatePrepared;

  // analyze the history object
  SAPDB_Int     nHisIndex = 0;
  SAPDB_Int     nMedIndex = 0;
  SAPDB_Int     nRecIndex = 0;

  DBMCli_RecoverItem::LocationArray oLocations;
  DBMCli_RecoverItem::RecoverAction nNextAction = DBMCli_RecoverItem::ActionStart;
  DBMCli_RecoverItem::RecoverAction nAction     = DBMCli_RecoverItem::ActionUnknown;
  DBMCli_RecoverItem::RecoverType   nType       = DBMCli_RecoverItem::TypeUnknown;

  DBMCli_String                     sEBID;
  DBMCli_String                     sKomma;

  DBMCli_MediumDeviceType oDeviceType;

  // loop trough history items
  for (nHisIndex = 0; nHisIndex < oHistoryItems.GetSize(); ++nHisIndex) {

    if (oHistoryItems[nHisIndex].IsActiveForRecovery()) {

      // loop trough media lines of a history item
      nMedIndex = 0;
      while (nMedIndex < oHistoryItems[nHisIndex].MediaCount()) {

        oLocations.RemoveAll();
        sEBID.Empty();
        sKomma.Empty();

        // collect locations
        if (oHistoryItems[nHisIndex].MediumKind(nMedIndex) == HISMED_KIND_MULTI) {

          oLocations.Add(oHistoryItems[nHisIndex].MediumLocationStr(nMedIndex));
          if (oHistoryItems[nHisIndex].ExternalStateValue(nMedIndex) == DBMCli_HistoryItem :: Ext_Available) {
            sEBID = sEBID + sKomma + oHistoryItems[nHisIndex].ExternalID(nMedIndex);
            sKomma = ",";
          } // end if
          ++nMedIndex;

          while (nMedIndex < oHistoryItems[nHisIndex].MediaCount()) {
            if (oHistoryItems[nHisIndex].MediumKind(nMedIndex) == HISMED_KIND_MULTI) {
              oLocations.Add(oHistoryItems[nHisIndex].MediumLocationStr(nMedIndex));
              if (oHistoryItems[nHisIndex].ExternalStateValue(nMedIndex) == DBMCli_HistoryItem :: Ext_Available) {
                sEBID = sEBID + sKomma + oHistoryItems[nHisIndex].ExternalID(nMedIndex);
                sKomma = ",";
              } // end if
              ++nMedIndex;
            } else {
              break;
            } // end if
          } // end while

        } else {
          oLocations.Add(oHistoryItems[nHisIndex].MediumLocationStr(nMedIndex));
          sEBID = sEBID + sKomma + oHistoryItems[nHisIndex].ExternalID(nMedIndex);
          sKomma = ",";
          ++nMedIndex;
        } // end if

        // compute recover action
        switch(oHistoryItems[nHisIndex].MediumKind(nMedIndex - 1)) {
          case HISMED_KIND_REPLACE:
            nAction = DBMCli_RecoverItem::ActionReplace;
            break;
          case HISMED_KIND_SINGLE:
          case HISMED_KIND_MULTI:
          default:
            nAction = nNextAction;
            break;
        } // end switch

        // compute recover type
        switch(oHistoryItems[nHisIndex].Label()) {
          case HISTORY_LABEL_DATA:
            nType = DBMCli_RecoverItem::TypeData;
            break;
          case HISTORY_LABEL_PAGES:
            nType = DBMCli_RecoverItem::TypePages;
            break;
          case HISTORY_LABEL_LOG:
            nType = DBMCli_RecoverItem::TypeLog;
            // after the first log recover only replace action are valid
            nNextAction = DBMCli_RecoverItem::ActionReplace;
            break;
          default:
            nType = DBMCli_RecoverItem::TypeUnknown;
            break;
        } // end switch

        // handle sEBID
        sEBID = (sEBID.GetLength() > 0) ? "\"" + sEBID + "\"" : sEBID;

        // get medium device type
        oDeviceType.SetName(oHistoryItems[nHisIndex].MediumDeviceTypeStr(0));

        // create recover item
        DBMCli_RecoverItem aItem( nAction, nType, oHistoryItems[nHisIndex].LabelStr(),
                                  oHistoryItems[nHisIndex].MediaNameStr(), oLocations, sEBID, 
                                  oDeviceType.Value());

        m_aItems.Add(aItem);
      } // end while
    } // end if
  } // end for

  GetDatabase().GetRestartInfo().Refresh();
} // end DBMCli_Recover :: PrepareRecover
/*
  -----------------------------------------------------------------------------
*/
void DBMCli_Recover :: PrepareRecover 
      ( const DBMCli_Medium   & oSingleMedium, 
        const DBMCli_DateTime & oUntil, 
        SAPDB_Bool            bCheck,
        SAPDB_Bool            bActivate)
{
  // clear members 
  Init();

  // set global information
  m_oUntil        = oUntil;
  m_bCheck        = bCheck;
  m_bActivate     = bActivate;
  m_RecoverMode   = RecoverModeMedium;
  m_RecoverState  = RecoverStatePrepared;

  // analyze the medium object
  DBMCli_BackupType       oBackupType(oSingleMedium.BackupType());
  DBMCli_MediumDeviceType oDeviceType(oSingleMedium.DeviceType());

  DBMCli_RecoverItem::LocationArray oLocations;
  DBMCli_RecoverItem::RecoverType   nType       = DBMCli_RecoverItem::TypeUnknown;
  DBMCli_String                     sEBID;

  // compute recover type
  switch(oBackupType.Value()) {
    case DBMCLI_BACKUPTYPE_DATA:
      nType = DBMCli_RecoverItem::TypeData;
      break;
    case DBMCLI_BACKUPTYPE_PAGES:
      nType = DBMCli_RecoverItem::TypePages;
      break;
    case DBMCLI_BACKUPTYPE_LOG:
    case DBMCLI_BACKUPTYPE_AUTO:
      nType = DBMCli_RecoverItem::TypeLog;
      break;
    default:
      nType = DBMCli_RecoverItem::TypeUnknown;
      break;
  } // end switch

  oLocations.Add(oSingleMedium.Location());

  // create recover item
  DBMCli_RecoverItem aItem( DBMCli_RecoverItem::ActionStart,
                            nType,
                            "MEDIUM",
                            oSingleMedium.Name(),
                            oLocations,
                            sEBID,
                            oDeviceType.Value());

  m_aItems.Add(aItem);

  GetDatabase().GetRestartInfo().Refresh();
} // end DBMCli_Recover :: PrepareRecover
/*
  -----------------------------------------------------------------------------
*/
void DBMCli_Recover :: PrepareRecover 
      ( const DBMCli_MediumParallel   & oParallelMedium, 
        const DBMCli_DateTime         & oUntil, 
        SAPDB_Bool                      bCheck,
        SAPDB_Bool                      bActivate)
{
  // clear members 
  Init();

  // set global information
  m_oUntil        = oUntil;
  m_bCheck        = bCheck;
  m_bActivate     = bActivate;
  m_RecoverMode   = RecoverModeMedium;
  m_RecoverState  = RecoverStatePrepared;

  // analyze the medium object
  SAPDB_Int nMedIndex = 0;

  DBMCli_BackupType       oBackupType(oParallelMedium.BackupType());
  DBMCli_MediumDeviceType oDeviceType(oParallelMedium.DeviceType());

  DBMCli_RecoverItem::LocationArray oLocations;
  DBMCli_RecoverItem::RecoverType   nType       = DBMCli_RecoverItem::TypeUnknown;
  DBMCli_String                     sEBID;

  // compute recover type
  switch(oBackupType.Value()) {
    case DBMCLI_BACKUPTYPE_DATA:
      nType = DBMCli_RecoverItem::TypeData;
      break;
    case DBMCLI_BACKUPTYPE_PAGES:
      nType = DBMCli_RecoverItem::TypePages;
      break;
    case DBMCLI_BACKUPTYPE_LOG:
    case DBMCLI_BACKUPTYPE_AUTO:
      nType = DBMCli_RecoverItem::TypeLog;
      break;
    default:
      nType = DBMCli_RecoverItem::TypeUnknown;
      break;
  } // end switch

  // collect locations
  for (nMedIndex = 0; nMedIndex < oParallelMedium.MediumArray().GetSize(); ++nMedIndex) {
    if (oParallelMedium.MediumArray()[nMedIndex].Location().GetLength() > 0) {
      oLocations.Add(oParallelMedium.MediumArray()[nMedIndex].Location());
    } // end if
  } // end for

  // create recover item
  DBMCli_RecoverItem aItem( DBMCli_RecoverItem::ActionStart,
                            nType,
                            "MEDIAGROUP",
                            oParallelMedium.Name(),
                            oLocations,
                            sEBID,
                            oDeviceType.Value());

  m_aItems.Add(aItem);

  GetDatabase().GetRestartInfo().Refresh();

} // end DBMCli_Recover :: PrepareRecover

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Recover :: CopyLastItem
  -----------------------------------------------------------------------------
*/
void DBMCli_Recover :: CopyLastItem ( DBMCli_RecoverItem::RecoverAction aAction)
{

  SAPDB_Int nLast = m_aItems.GetSize() - 1;

  if (nLast >= 0) {

    DBMCli_RecoverItem::LocationArray oLocations;
    DBMCli_String                     sEBID;

    if (m_oResult.LocationStr().GetLength() > 0) {
      oLocations.Add(m_oResult.LocationStr());
    } else if (nLast >= 0) {
      oLocations.Add(m_aItems[nLast].GetLocation(0));
    } else {
      oLocations.Add("");
    } // end if

    DBMCli_RecoverItem aItem( aAction,
                              m_aItems[nLast].GetType(),
                              "MEDIUM",
                              m_aItems[nLast].GetMediumName(),
                              oLocations,
                              sEBID,
                              m_aItems[nLast].GetDeviceType());

    // increase version number for log items
    if (aItem.GetType() == DBMCli_RecoverItem::TypeLog) {
      DBMCli_String sVersion;
      sVersion.Format("%03d", (atoi(aItem.GetVersion(0)) + 1));
      aItem.SetLocation(aItem.GetLocationName(0) + "." +  sVersion);
    } // end if

    m_aItems.Add(aItem);
  } // end if
} // end DBMCli_Recover :: CopyLastItem

/*
  =============================================================================
  class: DBMCli_RestartInfo
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_RestartInfo :: DBMCli_RestartInfo
  -----------------------------------------------------------------------------
*/
DBMCli_RestartInfo :: DBMCli_RestartInfo ( )
{
  m_nUsedLogPage  = DBMCLI_RESTARTINFO_INVALID;
  m_nFirstLogPage = DBMCLI_RESTARTINFO_INVALID;
  m_bRestartable  = false;
} // end DBMCli_RestartInfo :: DBMCli_RestartInfo

/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_RestartInfo :: ~DBMCli_RestartInfo
  -----------------------------------------------------------------------------
*/
DBMCli_RestartInfo :: ~DBMCli_RestartInfo ( )
{
} // end DBMCli_RestartInfo :: ~DBMCli_RestartInfo

/*
  -----------------------------------------------------------------------------
  public function DBMCli_RestartInfo :: Refresh
  -----------------------------------------------------------------------------
*/
void DBMCli_RestartInfo :: Refresh ( )
{
  Msg_List oMsgList;

  DBMCli_Database & oDB     = GetDatabase();
  DBMCli_Result   & oResult = oDB.GetResult( );
  DBMCli_String     sCmd( DBMCLI_CMD_RESTARTINFO);

  DBMCli_String sProp;
  DBMCli_String sVal;

  m_nUsedLogPage  = DBMCLI_RESTARTINFO_INVALID;
  m_nFirstLogPage = DBMCLI_RESTARTINFO_INVALID;
  m_bRestartable  = false;

  if( oDB.Execute ( sCmd, oMsgList ) ) {
    while( oResult.GetPropVal( sProp, sVal, PROPVAL_SEP ) ) {
      if (sProp == PROP_USED) {
        m_nUsedLogPage  = (sVal == VAL_INVALID) ? DBMCLI_RESTARTINFO_INVALID: atoi(sVal);
      } else if (sProp == PROP_FIRST) {
        m_nFirstLogPage = (sVal == VAL_INVALID) ? DBMCLI_RESTARTINFO_INVALID: atoi(sVal);
      } else if (sProp == PROP_RESTARTABLE) {
        m_bRestartable  = (sVal == VAL_RESTARTABLE);
      } // end if
    } // end while
  } // end if

} // end DBMCli_RestartInfo :: Refresh

