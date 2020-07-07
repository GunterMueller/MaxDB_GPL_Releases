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

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "DBM/Cli/DBMCli_LogModeObj.hpp"
#include "DBM/Cli/DBMCli_Database.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define LOGMODE_TXT_DEMO            "DEMO"
#define LOGMODE_TXT_SINGLE          "SINGLE"
#define LOGMODE_TXT_SINGLEDEMODB    "SINGLE /DEMO"
#define LOGMODE_TXT_SINGLEDEMOVIEW  "SINGLE/DEMO"
#define LOGMODE_TXT_SINGLEDEMOPARAM "SINGLEDEMO"
#define LOGMODE_TXT_DUAL            "DUAL"
#define LOGMODE_TXT_DUALDEMODB      "DUAL   /DEMO"
#define LOGMODE_TXT_DUALDEMOVIEW    "DUAL/DEMO"
#define LOGMODE_TXT_DUALDEMOPARAM   "DUALDEMO"
#define LOGMODE_TXT_UNKNOWN         "UNKNOWN"

#define LOG_MODE_FIELD         "Log Mode"

/*
  =============================================================================
  class: DBMCli_LogModeValue
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_LogModeValue :: DBMCli_LogModeValue
  -----------------------------------------------------------------------------
*/
DBMCli_LogModeValue :: DBMCli_LogModeValue ()
{
  SetValue(Unknown);
} // end DBMCli_LogModeValue :: DBMCli_LogModeValue
/*
  -----------------------------------------------------------------------------
*/
DBMCli_LogModeValue :: DBMCli_LogModeValue (const DBMCli_LogModeValue & oMode)
{
  SetValue(oMode);
} // end DBMCli_LogModeValue :: DBMCli_LogModeValue
/*
  -----------------------------------------------------------------------------
*/
DBMCli_LogModeValue :: DBMCli_LogModeValue (Mode nMode)
{
  SetValue(nMode);
} // end DBMCli_LogModeValue :: DBMCli_LogModeValue
/*
  -----------------------------------------------------------------------------
*/
DBMCli_LogModeValue :: DBMCli_LogModeValue (const DBMCli_String & sMode)
{
  SetValue(sMode);
} // end DBMCli_LogModeValue :: DBMCli_LogModeValue
/*
  -----------------------------------------------------------------------------
*/
DBMCli_LogModeValue :: DBMCli_LogModeValue (DBMCli_LogMode nMode)
{
  SetValue(nMode);
} // end DBMCli_LogModeValue :: DBMCli_LogModeValue

/*
  -----------------------------------------------------------------------------
  public function DBMCli_LogModeValue :: SetValue
  -----------------------------------------------------------------------------
*/
void DBMCli_LogModeValue :: SetValue (const DBMCli_LogModeValue & oMode)
{
  m_nMode = oMode.GetValue();
} // end DBMCli_LogModeValue :: SetValue
/*
  -----------------------------------------------------------------------------
*/
void DBMCli_LogModeValue :: SetValue (Mode nMode)
{
  m_nMode = nMode;
} // end DBMCli_LogModeValue :: SetValue
/*
  -----------------------------------------------------------------------------
*/
void DBMCli_LogModeValue :: SetValue (const DBMCli_String & sMode)
{
  if (sMode == LOGMODE_TXT_DEMO) {
    m_nMode = Demo;
  } else if (sMode == LOGMODE_TXT_SINGLE) {
    m_nMode = Single;
  } else if ((sMode == LOGMODE_TXT_SINGLEDEMODB   ) ||
             (sMode == LOGMODE_TXT_SINGLEDEMOVIEW ) ||
             (sMode == LOGMODE_TXT_SINGLEDEMOPARAM)    ) {
    m_nMode = SingleDemo;
  } else if (sMode == LOGMODE_TXT_DUAL) {
    m_nMode = Dual;
  } else if ((sMode == LOGMODE_TXT_DUALDEMODB   ) ||
             (sMode == LOGMODE_TXT_DUALDEMOVIEW ) ||
             (sMode == LOGMODE_TXT_DUALDEMOPARAM)    ) {
    m_nMode = DualDemo;
  } else {
    m_nMode = Unknown;
  } // end if

} // end DBMCli_LogModeValue :: SetValue
/*
  -----------------------------------------------------------------------------
*/
void DBMCli_LogModeValue :: SetValue (DBMCli_LogMode nMode)
{
  switch (nMode) {
    case DBMCLI_LOGMODE_SINGLE:
      m_nMode = Single;
      break;
    case DBMCLI_LOGMODE_DUAL:
      m_nMode = Dual;
      break;
    case DBMCLI_LOGMODE_DEMO:
      m_nMode = Demo;
      break;
    default:
      m_nMode = Unknown;
      break;
  } // end switch
} // end DBMCli_LogModeValue :: SetValue

/*
  -----------------------------------------------------------------------------
  public function DBMCli_LogModeValue :: GetString
  -----------------------------------------------------------------------------
*/
DBMCli_String DBMCli_LogModeValue :: GetString () const
{
  DBMCli_String sMode;
  switch (m_nMode) {
  case Demo:
    sMode = LOGMODE_TXT_DEMO;
    break;
  case Single:
    sMode = LOGMODE_TXT_SINGLE;
    break;
  case SingleDemo:
    sMode = LOGMODE_TXT_SINGLEDEMOVIEW;
    break;
  case Dual:
    sMode = LOGMODE_TXT_DUAL;
    break;
  case DualDemo:
    sMode = LOGMODE_TXT_DUALDEMOVIEW;
    break;
  default:
    sMode = LOGMODE_TXT_UNKNOWN;
    break;
  } // end switch

  return sMode;
} // end DBMCli_LogModeValue :: GetString

/*
  =============================================================================
  class: DBMCli_LogModeObj
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_LogModeObj :: DBMCli_LogModeObj
  -----------------------------------------------------------------------------
*/
DBMCli_LogModeObj :: DBMCli_LogModeObj ( )
{
} // end DBMCli_LogModeObj :: DBMCli_LogModeObj

/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_LogModeObj :: ~DBMCli_LogModeObj
  -----------------------------------------------------------------------------
*/
DBMCli_LogModeObj :: ~DBMCli_LogModeObj ( )
{

} // end DBMCli_LogModeObj :: ~DBMCli_LogModeObj

/*
  -----------------------------------------------------------------------------
  public function DBMCli_LogModeObj :: Refresh
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_LogModeObj :: Refresh ( Msg_List & oMsgList )
{
  SAPDB_Bool           bRc = true;
  Msg_List oTmpMsgList;
  DBMCli_ResultBuf     oRow;
  DBMCli_String        sField;
  SAPDB_Bool           bInfoOpen = false;

  bRc = GetDatabase().GetState().Refresh(oMsgList);
  if (bRc) {
    GetDatabase().GetDevspaces().Refresh(oMsgList);
  } // en dif
  if (bRc && (GetDatabase().GetState().Value() == DBMCLI_DBSTATE_WARM)) {
    bRc = GetDatabase().GetInfo().Open(DBMCLI_INFO_LOG, oMsgList);
    if (bRc) {
      bInfoOpen = true;
    } // end if
  } // end if

  if (bRc) {
    m_oValue.SetValue(GetDatabase().GetDevspaces().LogMode());
    if (GetDatabase().GetState().Value() == DBMCLI_DBSTATE_WARM) {
      while (GetDatabase().GetInfo().GetRow(oRow, oTmpMsgList)) {
        if( oRow.GetField( sField, "=|\n" ) ) {
          sField.Trim( );
          if (sField == LOG_MODE_FIELD) {
            if( oRow.GetField( sField, "=|\n" ) ) {
              sField.Trim( );
              m_oValue.SetValue(sField);
            } // end if
            break;
          } // end if
        } // end if
      } // end while
    } // end if

  } // end if

  if (bInfoOpen) {
    GetDatabase().GetInfo().Close(oTmpMsgList);
  } // end if

  return bRc;
} // end DBMCli_LogModeObj :: Refresh

/*
  -----------------------------------------------------------------------------
  public function DBMCli_LogModeObj :: Change
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_LogModeObj :: Change ( DBMCli_LogModeValue::Mode   nMode,
                                         SAPDB_Bool                  bRestart,
                                         Msg_List      & oMsgList )
{
  SAPDB_Bool           bRc = true;

  SAPDB_Int          nIndex = 0;
  DBMCli_Devspaces & oDevspaces = GetDatabase().GetDevspaces();

  switch (nMode) {
    case DBMCli_LogModeValue::Dual:
      bRc = ChangeToDual(bRestart, oMsgList);
      break;

    case DBMCli_LogModeValue::Single:
      bRc = ChangeToSingle(bRestart, oMsgList);
      break;

    case DBMCli_LogModeValue::DualDemo:
    case DBMCli_LogModeValue::SingleDemo:
      bRc = ChangeToDemo(oMsgList);
      break;
    } // end if

  return bRc;
} // end DBMCli_LogModeObj :: Change

/*
  -----------------------------------------------------------------------------
  public function DBMCli_LogModeObj :: ReadyForChange
  -----------------------------------------------------------------------------
*/
SAPDB_Bool  DBMCli_LogModeObj :: ReadyForChange ( DBMCli_LogModeValue::Mode nMode )
{
  SAPDB_Bool         bRc    = false;
  SAPDB_Int          nIndex = 0;
  DBMCli_Devspaces & oDevspaces = GetDatabase().GetDevspaces();

  switch (nMode) {
    case DBMCli_LogModeValue::Dual:
      // check prepared mirrors
      bRc = true;
      while ((nIndex < oDevspaces.DevspaceArray().GetSize()) && bRc) {
        if (oDevspaces.DevspaceArray()[nIndex].Class() == DBMCLI_DEVSPACECLASS_LOG) {
          bRc = bRc && 
                (oDevspaces.DevspaceArray()[nIndex].PreparedMirroredType() != DBMCLI_DEVSPACETYPE_UNKNOWN) &&
                (oDevspaces.DevspaceArray()[nIndex].PreparedMirroredLocation().GetLength() != 0);
        } // end if
        ++nIndex;
      } // end while

      break;

    case DBMCli_LogModeValue::Single:
    case DBMCli_LogModeValue::DualDemo:
    case DBMCli_LogModeValue::SingleDemo:
      // check nothing
      bRc = true;
      break;
    } // end if


  return bRc;
} // end DBMCli_LogModeObj :: ReadyForChange

/*
  -----------------------------------------------------------------------------
  private function DBMCli_LogModeObj :: ChangeToDual
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_LogModeObj :: ChangeToDual ( SAPDB_Bool                  bRestart,
                                               Msg_List      & oMsgList )
{
  SAPDB_Bool             bRc           = true;
  SAPDB_Bool             bParamChanged = false;
  SAPDB_Int              nDevspace     = 0;
  Msg_List   oTmpList;
  DBMCli_Devspaces     & oDevspaces    = GetDatabase().GetDevspaces();
  DBMCli_Parameters    & oParameters   = GetDatabase().GetParameters();

  // add mirrors
  if (bRc) {
    nDevspace = 0;
    while ((nDevspace < oDevspaces.DevspaceArray().GetSize()) && bRc) {
      if (oDevspaces.DevspaceArray()[nDevspace].Class() == DBMCLI_DEVSPACECLASS_LOG) {
        bRc = oDevspaces.DevspaceArray()[nDevspace].PutPreparedMirror(oMsgList);
        if (bRc) {
          bParamChanged = true;
        } // end if
      } // end if
      ++nDevspace;
    } // end while
  } // end if

  // alter LOG_MODE
  if (bRc) {
    bRc = oParameters.Refresh(oMsgList);
    if (bRc) {
      bRc = oParameters.Open(oMsgList);
      if (bRc) {
        SAPDB_Int nLogMode = oParameters.IndexByName(PARAM_LOG_MODE);
        bRc = oParameters.ParameterArray()[nLogMode].Put(LOGMODE_TXT_DUAL, oMsgList);
        if (bRc) {
          bRc = oParameters.Check(oMsgList);
          if (bRc) {
            bRc = oParameters.Close(true, oMsgList);
          } // end if
        } // end if
        if (!bRc) {
          bRc = oParameters.Close(false, oTmpList);
        } // end if
      } // end if
    } // end if
  } // end if

  // on error param_restore
  if (bParamChanged && ! bRc) {
    oParameters.Restore(1, oTmpList);
  } // end if

  // Restart
  if (bRc) {
    bRc = GetDatabase().Execute( DBMCLI_CMD_DBRESTART, oMsgList);
  } // end if

  // refresh devspaces
  if (bRc) {
    bRc = oDevspaces.Refresh (oMsgList);
  } // end if


  // reintegrate
  if (bRc) {
    nDevspace = 0;
    while ((nDevspace < oDevspaces.DevspaceArray().GetSize()) && bRc) {
      if (oDevspaces.DevspaceArray()[nDevspace].Class() == DBMCLI_DEVSPACECLASS_LOG) {
        bRc = oDevspaces.DevspaceArray()[nDevspace].Reintegrate(oMsgList);
      } // end if
      ++nDevspace;
    } // end while
  } // end if

  return bRc;
} // end DBMCli_LogModeObj :: ChangeToDual

/*
  -----------------------------------------------------------------------------
  private function DBMCli_LogModeObj :: ChangeToSingle
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_LogModeObj :: ChangeToSingle ( SAPDB_Bool                  bRestart,
                                                 Msg_List      & oMsgList )
{
  SAPDB_Bool             bRc           = true;
  SAPDB_Bool             bParamChanged = false;
  SAPDB_Int              nDevspace     = 0;
  Msg_List   oTmpList;
  DBMCli_Devspaces     & oDevspaces    = GetDatabase().GetDevspaces();
  DBMCli_Parameters    & oParameters   = GetDatabase().GetParameters();

  // add mirrors
  if (bRc) {
    nDevspace = 0;
    while ((nDevspace < oDevspaces.DevspaceArray().GetSize()) && bRc) {
      if (oDevspaces.DevspaceArray()[nDevspace].Class() == DBMCLI_DEVSPACECLASS_LOG) {
        bRc = oDevspaces.DevspaceArray()[nDevspace].DeleteMirror(oMsgList);
        if (bRc) {
          bParamChanged = true;
        } // end if
      } // end if
      ++nDevspace;
    } // end while
  } // end if

  // alter LOG_MODE
  if (bRc) {
    bRc = oParameters.Refresh(oMsgList);
    if (bRc) {
      bRc = oParameters.Open(oMsgList);
      if (bRc) {
        SAPDB_Int nLogMode = oParameters.IndexByName(PARAM_LOG_MODE);
        bRc = oParameters.ParameterArray()[nLogMode].Put(LOGMODE_TXT_SINGLE, oMsgList);
        if (bRc) {
          bRc = oParameters.Check(oMsgList);
          if (bRc) {
            bRc = oParameters.Close(true, oMsgList);
          } // end if
        } // end if
        if (!bRc) {
          bRc = oParameters.Close(false, oTmpList);
        } // end if
      } // end if
    } // end if
  } // end if

  // on error param_restore
  if (bParamChanged && ! bRc) {
    oParameters.Restore(1, oTmpList);
  } // end if

  // Restart
  if (bRc) {
    bRc = GetDatabase().Execute( DBMCLI_CMD_DBRESTART, oMsgList);
  } // end if

  // refresh devspaces
  if (bRc) {
    bRc = oDevspaces.Refresh (oMsgList);
  } // end if

  return bRc;
} // end DBMCli_LogModeObj :: ChangeToSingle

/*
  -----------------------------------------------------------------------------
  private function DBMCli_LogModeObj :: ChangeToDemo
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_LogModeObj :: ChangeToDemo ( Msg_List      & oMsgList )
{
  DBMCli_String          sCmd;

  sCmd = DBMCLI_CMD_UTLEXECUTE;
  sCmd += " SET LOG MODE DEMO";

  return GetDatabase().Execute(sCmd, oMsgList);
} // end DBMCli_LogModeObj :: ChangeToDemo
