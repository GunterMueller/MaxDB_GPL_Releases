/*!
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateLogMode.cpp
  -----------------------------------------------------------------------------
  responsible:  Bernd Vorsprach Bernd.Vorsprach@sap.com
  special area: Database Manager WebAgent Extension
  description:  HTML Template for Recovery
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
#include "DBM/Web/DBMWeb_TemplateLogMode.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define TEMPLATE_NAME              "DBMLogMode.htm"

#define OBJ_GROUPBOXTITLE          "GroupBoxTitle"
#define VAL_GROUPBOXTITLE          OBJ_GROUPBOXTITLE

#define OBJ_CURRENTTXT             "CurrentTxt"
#define VAL_CURRENTTXT             OBJ_CURRENTTXT

#define OBJ_LOGMODE73              "LogMode73" 
#define OBJ_LOGMODE74              "LogMode74"

#define OBJ_IMGDEMO                "ImgDemo"
#define OBJ_IMGSINGLE              "ImgSingle"
#define OBJ_IMGSINGLEDEMO          "ImgSingleDemo"
#define OBJ_IMGDUAL                "ImgDual"
#define OBJ_IMGDUALDEMO            "ImgDualDemo"
#define OBJ_IMGUNKNOWN             "ImgUnknown"

#define OBJ_BTNCHANGELOGMODE       "BtnChangeLogMode"
#define OBJ_BTNCHANGELOGMODEDIS    "BtnChangeLogModeDisabled"

#define OBJ_GLBVARS                "GlobalVars*"
#define VAL_LOGMODE                "Logmode"
#define VAL_DEVSPACE               "Devspace"

#define OBJ_NEWMODESINGLE          "NewModeSingle"
#define OBJ_NEWMODEDUAL            "NewModeDual"
#define OBJ_NEWMODESINGLEDEMO      "NewModeSingleDemo"
#define OBJ_NEWMODEDUALDEMO        "NewModeDualDemo"
#define OBJ_NEWMODESINGLECHK       "NewModeSingleChk"
#define OBJ_NEWMODEDUALCHK         "NewModeDualChk"
#define OBJ_NEWMODESINGLEDEMOCHK   "NewModeSingleDemoChk"
#define OBJ_NEWMODEDUALDEMOCHK     "NewModeDualDemoChk"

#define OBJ_RESTART                "Restart"
#define OBJ_RESTARTCHECKED         "RestartChecked"
#define OBJ_RESTARTNOTCHECKED      "RestartNotChecked"
#define OBJ_RESTARTIMPLICIT        "RestartImplicit"
#define OBJ_REINTEGRATEIMPLICIT    "ReintegrateImplicit"

#define OBJ_AREA_VIEWDEVSPACES     "ViewDevspaces"
#define OBJ_AREA_EDITDEVSPACE      "EditDevspace"

#define OBJ_ROWPAIREDIT            "RowPairEdit"
#define OBJ_ROWPAIRVIEW            "RowPairView"
#define OBJ_EVENROW                "EvenRow*"
#define OBJ_ODDROW                 "OddRow*"
#define VAL_NAME                   "Name"
#define VAL_SIZE                   "Size"
#define VAL_TYPE                   "Type"
#define VAL_LOCATION               "Location"
#define VAL_MTYPE                  "MType"
#define VAL_MLOCATION              "MLocation"

#define OBJ_TXTTARGETSINGLE         "TxtTargetSingle"
#define OBJ_TXTTARGETDUAL           "TxtTargetDual"
#define OBJ_TXTTARGETSINGLEDEMO     "TxtTargetSingleDemo"
#define OBJ_TXTTARGETDUALDEMO       "TxtTargetDualDemo"
#define OBJ_TXTTARGETSINGLEFROMDEMO "TxtTargetSingleFromDemo"
#define OBJ_TXTTARGETDUALFROMDEMO   "TxtTargetDualFromDemo"

#define HTML_SPACE                  "&nbsp;"
#define REMOVE_STRING               "&lt;remove&gt;"
#define QUESTION_STRING             "?"

#define OBJ_EDITNAME                "EditName"
#define OBJ_EDITSIZE                "EditSize"
#define OBJ_EDITLOCATION            "EditLocation"
#define OBJ_EDITTYPEFILE            "EditTypeFile"
#define OBJ_EDITTYPELINK            "EditTypeLink"
#define OBJ_EDITTYPERAW             "EditTypeRaw"
#define OBJ_EDITMLOCATION           "EditMLocation"
#define OBJ_EDITMTYPEFILE           "EditMTypeFile"
#define OBJ_EDITMTYPELINK           "EditMTypeLink"
#define OBJ_EDITMTYPERAW            "EditMTypeRaw"

#define OBJ_REFRESHSTATE            "RefreshState"

/*
  =============================================================================
  class:        DBMWeb_TemplateLogMode
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMWeb_TemplateLogMode :: DBMWeb_TemplateLogMode
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateLogMode :: DBMWeb_TemplateLogMode ( sapdbwa_WebAgent          & wa,
                                                   Mode                        nMode,
                                                   DBMCli_LogModeValue::Mode   nNewLogMode,
                                                   SAPDB_Bool                  bRestart,
                                                   DBMCli_Database           * pDatabase )
                        : Tools_Template( wa, _Tools_UTF8Ptr( TEMPLATE_NAME )),
                           m_nMode         ( nMode ),
                           m_nNewLogMode   ( nNewLogMode ),
                           m_bRestart      ( bRestart ),
                           m_pDatabase     ( pDatabase ),
                           m_oDevspaces    ( pDatabase->GetDevspaces()),
                           m_bRefreshState ( false )
{
} // end DBMWeb_TemplateLogMode :: DBMWeb_TemplateLogMode

/*
  -----------------------------------------------------------------------------
  public destructor DBMWeb_TemplateLogMode :: ~DBMWeb_TemplateLogMode
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateLogMode :: ~DBMWeb_TemplateLogMode (  )
{
} // end DBMWeb_TemplateLogMode :: ~DBMWeb_TemplateLogMode

/*
  -----------------------------------------------------------------------------
  public function DBMWeb_TemplateLogMode :: SetCurrentDevspace
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateLogMode & DBMWeb_TemplateLogMode :: SetCurrentDevspace (const DBMCli_String & sDevspace)
{
  m_nCurrent = m_oDevspaces.IndexByName(sDevspace);
  return *this;
} // end DBMWeb_TemplateLogMode :: SetCurrentDevspace

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateLogMode :: initTemplate
  -----------------------------------------------------------------------------
*/
bool DBMWeb_TemplateLogMode :: initTemplate ( )
{
  m_nLogMode    = m_pDatabase->GetLogModeObj().Value().GetValue();
  m_nState      = m_pDatabase->GetState().Value();
  m_nIndex   = -1;
  return true;
} // end DBMWeb_TemplateLogMode :: initTemplate

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateLogMode :: askForWriteCount
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateLogMode :: askForWriteCount ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  // globals
  if (szName.Compare(OBJ_GROUPBOXTITLE) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_CURRENTTXT) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_GLBVARS) == 0) {
    nReturn = 1;
  
  } else if (szName.Compare(OBJ_LOGMODE73) == 0) {
    // VERSIONCHECK
    if (m_pDatabase->NodeInfo().Version().GetNumber() < 0x7040000) {
      nReturn = 1;
    } else {
      nReturn = 0;
    }
  } else if (szName.Compare(OBJ_LOGMODE74) == 0) {
    // VERSIONCHECK
    if (m_pDatabase->NodeInfo().Version().GetNumber() >= 0x7040000) {
      nReturn = 1;
    } else {
      nReturn = 0;
    }

  // left area
  
  // icon of current mode
  } else if (szName.Compare(OBJ_IMGDEMO) == 0) {
    nReturn = (  m_nLogMode == DBMCli_LogModeValue::Demo) ? 1 : 0;
  } else if (szName.Compare(OBJ_IMGSINGLE) == 0) {
    nReturn = ( m_nLogMode == DBMCli_LogModeValue::Single) ? 1 : 0;
  } else if (szName.Compare(OBJ_IMGSINGLEDEMO) == 0) {
    nReturn = ( m_nLogMode == DBMCli_LogModeValue::SingleDemo) ? 1 : 0;
  } else if (szName.Compare(OBJ_IMGDUAL) == 0) {
    nReturn = ( m_nLogMode == DBMCli_LogModeValue::Dual) ? 1 : 0;
  } else if (szName.Compare(OBJ_IMGDUALDEMO) == 0) {
    nReturn = ( m_nLogMode == DBMCli_LogModeValue::DualDemo) ? 1 : 0;
  } else if (szName.Compare(OBJ_IMGUNKNOWN) == 0) {
    nReturn = ( m_nLogMode == DBMCli_LogModeValue::Unknown) ? 1 : 0;

  // text
  } else if (szName.Compare(OBJ_TXTTARGETDUAL) == 0) {
    nReturn = (m_nNewLogMode == DBMCli_LogModeValue::Dual) && (m_nLogMode != DBMCli_LogModeValue::DualDemo) ? 1 : 0;
  } else if (szName.Compare(OBJ_TXTTARGETSINGLE) == 0) {
    nReturn = (m_nNewLogMode == DBMCli_LogModeValue::Single) && (m_nLogMode != DBMCli_LogModeValue::SingleDemo) ? 1 : 0;
  } else if (szName.Compare(OBJ_TXTTARGETDUALFROMDEMO) == 0) {
    nReturn = (m_nNewLogMode == DBMCli_LogModeValue::Dual) && (m_nLogMode == DBMCli_LogModeValue::DualDemo) ? 1 : 0;
  } else if (szName.Compare(OBJ_TXTTARGETSINGLEFROMDEMO) == 0) {
    nReturn = (m_nNewLogMode == DBMCli_LogModeValue::Single) && (m_nLogMode == DBMCli_LogModeValue::SingleDemo) ? 1 : 0;
  } else if (szName.Compare(OBJ_TXTTARGETSINGLEDEMO) == 0) {
    nReturn = (m_nNewLogMode == DBMCli_LogModeValue::SingleDemo) ? 1 : 0;
  } else if (szName.Compare(OBJ_TXTTARGETDUALDEMO) == 0) {
    nReturn = (m_nNewLogMode == DBMCli_LogModeValue::DualDemo) ? 1 : 0;

  // upper right area

  // radiobuttons unchecked
  } else if (szName.Compare(OBJ_NEWMODESINGLE) == 0) {
    nReturn = ((m_nLogMode == DBMCli_LogModeValue::Demo) || 
               (m_nLogMode == DBMCli_LogModeValue::SingleDemo) || 
               (m_nLogMode == DBMCli_LogModeValue::Dual))    && 
              (m_nNewLogMode != DBMCli_LogModeValue::Single)? 1 : 0;
  } else if (szName.Compare(OBJ_NEWMODEDUAL) == 0) {
    nReturn = ((m_nLogMode == DBMCli_LogModeValue::Demo) || 
               (m_nLogMode == DBMCli_LogModeValue::DualDemo) || 
               (m_nLogMode == DBMCli_LogModeValue::Single))  && 
              (m_nNewLogMode != DBMCli_LogModeValue::Dual)? 1 : 0;
  } else if (szName.Compare(OBJ_NEWMODESINGLEDEMO) == 0) {
    nReturn = ((m_nLogMode    == DBMCli_LogModeValue::Single) && 
               (m_nState      == DBMCLI_DBSTATE_WARM        ) &&
               (m_nNewLogMode != DBMCli_LogModeValue::SingleDemo)    ) ? 1 : 0;
  } else if (szName.Compare(OBJ_NEWMODEDUALDEMO) == 0) {
    nReturn = ((m_nLogMode    == DBMCli_LogModeValue::Dual) && 
               (m_nState      == DBMCLI_DBSTATE_WARM        ) &&
               (m_nNewLogMode != DBMCli_LogModeValue::DualDemo)    ) ? 1 : 0;

  // radiobuttons checked
  } else if (szName.Compare(OBJ_NEWMODESINGLECHK) == 0) {
    nReturn = ((m_nLogMode == DBMCli_LogModeValue::Demo) || 
               (m_nLogMode == DBMCli_LogModeValue::SingleDemo) || 
               (m_nLogMode == DBMCli_LogModeValue::Dual))    && 
              (m_nNewLogMode == DBMCli_LogModeValue::Single)? 1 : 0;
  } else if (szName.Compare(OBJ_NEWMODEDUALCHK) == 0) {
    nReturn = ((m_nLogMode == DBMCli_LogModeValue::Demo) || 
               (m_nLogMode == DBMCli_LogModeValue::DualDemo) || 
               (m_nLogMode == DBMCli_LogModeValue::Single))  && 
              (m_nNewLogMode == DBMCli_LogModeValue::Dual)? 1 : 0;
  } else if (szName.Compare(OBJ_NEWMODESINGLEDEMOCHK) == 0) {
    nReturn = ((m_nLogMode    == DBMCli_LogModeValue::Single) && 
               (m_nState      == DBMCLI_DBSTATE_WARM        ) &&
               (m_nNewLogMode == DBMCli_LogModeValue::SingleDemo)    ) ? 1 : 0;
  } else if (szName.Compare(OBJ_NEWMODEDUALDEMOCHK) == 0) {
    nReturn = ((m_nLogMode    == DBMCli_LogModeValue::Dual) && 
               (m_nState      == DBMCLI_DBSTATE_WARM        ) &&
               (m_nNewLogMode == DBMCli_LogModeValue::DualDemo)    ) ? 1 : 0;
  
  // restart
  } else if (szName.Compare(OBJ_RESTART) == 0) {
    nReturn = ((m_nNewLogMode == DBMCli_LogModeValue::Dual     ) || (m_nNewLogMode == DBMCli_LogModeValue::Single    )) &&
              ((m_nLogMode    != DBMCli_LogModeValue::DualDemo ) && (m_nLogMode    != DBMCli_LogModeValue::SingleDemo))     ? 1 : 0;
  } else if (szName.Compare(OBJ_RESTARTCHECKED) == 0) {
    nReturn = m_bRestart ? 1 : 0;
  } else if (szName.Compare(OBJ_RESTARTNOTCHECKED) == 0) {
    nReturn = m_bRestart ? 0 : 1;
  } else if (szName.Compare(OBJ_RESTARTIMPLICIT) == 0) {
    nReturn = m_nNewLogMode == DBMCli_LogModeValue::Single ? 1 : 0;
  } else if (szName.Compare(OBJ_REINTEGRATEIMPLICIT) == 0) {
    nReturn = m_nNewLogMode == DBMCli_LogModeValue::Dual ? 1 : 0;

  // lower right area
  } else if (szName.Compare(OBJ_AREA_VIEWDEVSPACES) == 0) {
    nReturn = m_nMode == View ? 1 : 0;
  } else if (szName.Compare(OBJ_AREA_EDITDEVSPACE) == 0) {
    nReturn = m_nMode == Edit ? 1 : 0;
    
  // devspace table
  } else if (szName.Compare(OBJ_ROWPAIRVIEW) == 0) {
    nReturn = (m_nNewLogMode == DBMCli_LogModeValue::Dual) && (m_nLogMode != DBMCli_LogModeValue::DualDemo) ?  0 : -1;
  } else if (szName.Compare(OBJ_ROWPAIREDIT) == 0) {
    nReturn = (m_nNewLogMode == DBMCli_LogModeValue::Dual) && (m_nLogMode != DBMCli_LogModeValue::DualDemo) ? -1 :  0;
  } else if (szName.Compare(OBJ_EVENROW) == 0) {
    nReturn = 1; // always because ROWPAIR 
  } else if (szName.Compare(OBJ_ODDROW) == 0) {
    // search next logdevspace
    ++m_nIndex;
    while (m_nIndex < m_oDevspaces.DevspaceArray().GetSize()) {
      if (m_oDevspaces.DevspaceArray()[m_nIndex].Class() == DBMCLI_DEVSPACECLASS_LOG) {
        nReturn = 1;
        break;
      } // end if
      ++m_nIndex;
    } // end while

  // devspace edit
  } else if (szName.Compare(OBJ_EDITNAME) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_EDITSIZE) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_EDITLOCATION) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_EDITTYPEFILE) == 0) {
    nReturn = m_oDevspaces.DevspaceArray()[m_nCurrent].Type() == DBMCLI_DEVSPACETYPE_FILE ? 1 : 0;
  } else if (szName.Compare(OBJ_EDITTYPELINK) == 0) {
    nReturn = m_oDevspaces.DevspaceArray()[m_nCurrent].Type() == DBMCLI_DEVSPACETYPE_LINK ? 1 : 0;
  } else if (szName.Compare(OBJ_EDITTYPERAW) == 0) {
    nReturn = m_oDevspaces.DevspaceArray()[m_nCurrent].Type() == DBMCLI_DEVSPACETYPE_RAW ? 1 : 0;
  } else if (szName.Compare(OBJ_EDITMLOCATION) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_EDITMTYPEFILE) == 0) {
    if (m_oDevspaces.DevspaceArray()[m_nCurrent].PreparedMirroredType() != DBMCLI_DEVSPACETYPE_UNKNOWN) {
      nReturn = m_oDevspaces.DevspaceArray()[m_nCurrent].PreparedMirroredType() == DBMCLI_DEVSPACETYPE_FILE ? 1 : 0;
    } else {
      nReturn = m_oDevspaces.DevspaceArray()[m_nCurrent].Type() == DBMCLI_DEVSPACETYPE_FILE ? 1 : 0;
    } // end if
  } else if (szName.Compare(OBJ_EDITMTYPELINK) == 0) {
    if (m_oDevspaces.DevspaceArray()[m_nCurrent].PreparedMirroredType() != DBMCLI_DEVSPACETYPE_UNKNOWN) {
      nReturn = m_oDevspaces.DevspaceArray()[m_nCurrent].PreparedMirroredType() == DBMCLI_DEVSPACETYPE_LINK ? 1 : 0;
    } else {
      nReturn = m_oDevspaces.DevspaceArray()[m_nCurrent].Type() == DBMCLI_DEVSPACETYPE_LINK ? 1 : 0;
    } // end if
  } else if (szName.Compare(OBJ_EDITMTYPERAW) == 0) {
    if (m_oDevspaces.DevspaceArray()[m_nCurrent].PreparedMirroredType() != DBMCLI_DEVSPACETYPE_UNKNOWN) {
      nReturn = m_oDevspaces.DevspaceArray()[m_nCurrent].PreparedMirroredType() == DBMCLI_DEVSPACETYPE_RAW ? 1 : 0;
    } else {
      nReturn = m_oDevspaces.DevspaceArray()[m_nCurrent].Type() == DBMCLI_DEVSPACETYPE_RAW ? 1 : 0;
    } // end if
  
  // bottom area
    
  // button
  } else if (szName.Compare(OBJ_BTNCHANGELOGMODE) == 0) {
    nReturn = ((m_nLogMode == DBMCli_LogModeValue::Dual  ) && (m_nNewLogMode == DBMCli_LogModeValue::DualDemo   )) ||
              ((m_nLogMode == DBMCli_LogModeValue::Dual  ) && (m_nNewLogMode == DBMCli_LogModeValue::Single     )) ||
              ((m_nLogMode == DBMCli_LogModeValue::Single) && (m_nNewLogMode == DBMCli_LogModeValue::SingleDemo )) ||
              ((m_nLogMode == DBMCli_LogModeValue::Single) && (m_nNewLogMode == DBMCli_LogModeValue::Dual       ) && m_pDatabase->GetLogModeObj().ReadyForChange(m_nNewLogMode) ) ||
              ((m_nLogMode == DBMCli_LogModeValue::Demo  ) && (m_nNewLogMode == DBMCli_LogModeValue::Single     )) ||
              ((m_nLogMode == DBMCli_LogModeValue::Demo  ) && (m_nNewLogMode == DBMCli_LogModeValue::Dual       ) && m_pDatabase->GetLogModeObj().ReadyForChange(m_nNewLogMode) ) ? 1 : 0;
  } else if (szName.Compare(OBJ_BTNCHANGELOGMODEDIS) == 0) {
    nReturn = ((m_nLogMode == DBMCli_LogModeValue::Dual  ) && (m_nNewLogMode == DBMCli_LogModeValue::DualDemo   )) ||
              ((m_nLogMode == DBMCli_LogModeValue::Dual  ) && (m_nNewLogMode == DBMCli_LogModeValue::Single     )) ||
              ((m_nLogMode == DBMCli_LogModeValue::Single) && (m_nNewLogMode == DBMCli_LogModeValue::SingleDemo )) ||
              ((m_nLogMode == DBMCli_LogModeValue::Single) && (m_nNewLogMode == DBMCli_LogModeValue::Dual       ) && m_pDatabase->GetLogModeObj().ReadyForChange(m_nNewLogMode) ) ||
              ((m_nLogMode == DBMCli_LogModeValue::Demo  ) && (m_nNewLogMode == DBMCli_LogModeValue::Single     )) ||
              ((m_nLogMode == DBMCli_LogModeValue::Demo  ) && (m_nNewLogMode == DBMCli_LogModeValue::Dual       ) && m_pDatabase->GetLogModeObj().ReadyForChange(m_nNewLogMode) ) ? 0 : 1;
  } else if (szName.Compare(OBJ_REFRESHSTATE) == 0) {
    nReturn = m_bRefreshState ? 1 : 0;
  } // end if

  return nReturn;
} // DBMWeb_TemplateLogMode :: askForWriteCount

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateLogMode :: askForValue
  -----------------------------------------------------------------------------
*/
Tools_DynamicUTF8String DBMWeb_TemplateLogMode :: askForValue ( const Tools_DynamicUTF8String & szName )
{
  Tools_DynamicUTF8String      sValue;

  if (szName.Compare(VAL_GROUPBOXTITLE) == 0) {
    sValue.Assign(m_pDatabase->DatabaseName());
  } else if (szName.Compare(VAL_LOGMODE) == 0) {
    DBMCli_LogModeValue oNewMode(m_nNewLogMode);
    sValue.Assign(oNewMode.GetString());
  } else if (szName.Compare(VAL_DEVSPACE) == 0) {
    if (m_nMode == Edit) {
      sValue.Assign(m_oDevspaces.DevspaceArray()[m_nCurrent].Name());
    } // end if
  } else if (szName.Compare(VAL_CURRENTTXT) == 0) {
    sValue.Assign(m_pDatabase->GetLogModeObj().Value().GetString());

  } else if (szName.Compare(VAL_NAME) == 0) {
    sValue.Assign(m_oDevspaces.DevspaceArray()[m_nIndex].Name());
  } else if (szName.Compare(VAL_SIZE) == 0) {   
    sValue.ConvertFromInt(m_oDevspaces.DevspaceArray()[m_nIndex].Pages());
  } else if (szName.Compare(VAL_TYPE) == 0) {   
    sValue.Assign(m_oDevspaces.DevspaceArray()[m_nIndex].TypeName());
  } else if (szName.Compare(VAL_LOCATION) == 0) {   
    sValue.Assign(m_oDevspaces.DevspaceArray()[m_nIndex].Location());
  } else if (szName.Compare(VAL_MTYPE) == 0) { 
    if (((m_nLogMode == DBMCli_LogModeValue::Dual) || (m_nLogMode == DBMCli_LogModeValue::DualDemo))
        && (m_nNewLogMode != DBMCli_LogModeValue::Single)) {
      sValue.Assign(m_oDevspaces.DevspaceArray()[m_nIndex].MirroredTypeName());
    } else if (((m_nLogMode == DBMCli_LogModeValue::Dual) || (m_nLogMode == DBMCli_LogModeValue::DualDemo))
        && (m_nNewLogMode == DBMCli_LogModeValue::Single)) {
      sValue.Assign(REMOVE_STRING);
    } else if (((m_nLogMode == DBMCli_LogModeValue::Single) || (m_nLogMode == DBMCli_LogModeValue::Demo)) &&
               (m_nNewLogMode == DBMCli_LogModeValue::Dual)) {
      sValue.Assign(m_oDevspaces.DevspaceArray()[m_nIndex].PreparedMirroredTypeName());
      if (sValue.Length() == 0) {
        sValue.Assign(QUESTION_STRING);
      } // end if
    } else {
      sValue.Assign(HTML_SPACE);
    } // end if
  } else if (szName.Compare(VAL_MLOCATION) == 0) {  
    if (((m_nLogMode == DBMCli_LogModeValue::Dual) || (m_nLogMode == DBMCli_LogModeValue::DualDemo))
        && (m_nNewLogMode != DBMCli_LogModeValue::Single)) {
      sValue.Assign(m_oDevspaces.DevspaceArray()[m_nIndex].MirroredLocation());
    } else if (((m_nLogMode == DBMCli_LogModeValue::Dual) || (m_nLogMode == DBMCli_LogModeValue::DualDemo))
        && (m_nNewLogMode == DBMCli_LogModeValue::Single)) {
      sValue.Assign(REMOVE_STRING);
    } else if (((m_nLogMode == DBMCli_LogModeValue::Single) || (m_nLogMode == DBMCli_LogModeValue::Demo)) &&
               (m_nNewLogMode == DBMCli_LogModeValue::Dual)) {
      sValue.Assign(m_oDevspaces.DevspaceArray()[m_nIndex].PreparedMirroredLocation());
      if (sValue.Length() == 0) {
        sValue.Assign(QUESTION_STRING);
      } // end if
    } else {
      sValue.Assign(HTML_SPACE);
    } // end if

  // devspace edit
  } else if (szName.Compare(OBJ_EDITNAME) == 0) {
    sValue.Assign(m_oDevspaces.DevspaceArray()[m_nCurrent].Name());
  } else if (szName.Compare(OBJ_EDITSIZE) == 0) {
    sValue.ConvertFromInt(m_oDevspaces.DevspaceArray()[m_nCurrent].Pages());
  } else if (szName.Compare(OBJ_EDITLOCATION) == 0) {
    sValue.Assign(m_oDevspaces.DevspaceArray()[m_nCurrent].Location());
  } else if (szName.Compare(OBJ_EDITMLOCATION) == 0) {
    sValue.Assign(m_oDevspaces.DevspaceArray()[m_nCurrent].PreparedMirroredLocation());
  } // end if

   return sValue;
} // end DBMWeb_TemplateLogMode :: askForValue

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateLogMode :: askForContinue
  -----------------------------------------------------------------------------
*/
bool DBMWeb_TemplateLogMode :: askForContinue ( const Tools_DynamicUTF8String & szName )
{
  bool bRC = false;

  if ((szName.Compare(OBJ_ROWPAIRVIEW) == 0) || (szName.Compare(OBJ_ROWPAIREDIT) == 0)) {
    // search next logdevspace
    ++m_nIndex;
    while (m_nIndex < m_oDevspaces.DevspaceArray().GetSize()) {
      if (m_oDevspaces.DevspaceArray()[m_nIndex].Class() == DBMCLI_DEVSPACECLASS_LOG) {
        bRC = true;
        break;
      } // end if
      ++m_nIndex;
    } // end while
  } // end if

  return bRC;
} // DBMWeb_TemplateLogMode :: askForContinue
