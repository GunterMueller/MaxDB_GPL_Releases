/*!
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateState.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  state of database
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
#include "DBM/Web/DBMWeb_TemplateState.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define TEMPLATE_NAME          "DBMState.htm"

#define OBJ_TITLE              "GroupBoxTitle"

#define OBJ_DBSTATE            "DBState"
#define OBJ_STATEICON          "StateIcon"
#define OBJ_STATEICON_UNKNOWN  "StateIconUnknown"
#define OBJ_STATEICON_OFFLINE  "StateIconOffline"
#define OBJ_STATEICON_COLD     "StateIconCold"
#define OBJ_STATEICON_WARM     "StateIconWarm"
#define OBJ_STATETEXT          "StateText"
#define OBJ_DATABAR            "DataBar"
#define OBJ_DATAPCT            "DataPercent"
#define OBJ_LOGBAR             "LogBar"
#define OBJ_LOGPCT             "LogPercent"
#define OBJ_SESBAR             "SessionsBar"
#define OBJ_SESPCT             "SessionsPercent"
#define OBJ_CONVCACHE          "ConverterCache"
#define OBJ_DATACACHE          "DataCache"
#define OBJ_AUTOLOG            "AutoLog"
#define OBJ_BADIDX             "BadIndexes"
#define OBJ_DBFULL             "DBFull"
#define OBJ_KNLTRACE           "KernelTrace"
#define OBJ_SENDERROR          "SendError"

#define OBJ_BTN_OFFLINE           "ButtonOffline"
#define OBJ_BTN_OFFLINE_DISABLED  "ButtonOfflineDisabled"
#define OBJ_BTN_COLD              "ButtonCold"
#define OBJ_BTN_COLD_DISABLED     "ButtonColdDisabled"
#define OBJ_BTN_WARM              "ButtonWarm"
#define OBJ_BTN_WARM_DISABLED     "ButtonWarmDisabled"

/*
  =============================================================================
  class:        DBMWeb_TemplateState
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMWeb_TemplateState :: DBMWeb_TemplateState
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateState :: DBMWeb_TemplateState ( sapdbwa_WebAgent & wa,
                                               DBMCli_State     & oState,
                                               bool               bSendError)
                        : Tools_Template( wa, _Tools_UTF8Ptr( TEMPLATE_NAME )),
                            m_oState( oState ),
                            m_bSendError( bSendError )
{
} 

/*
  -----------------------------------------------------------------------------
  public destructor DBMWeb_TemplateState :: ~DBMWeb_TemplateState
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateState :: ~DBMWeb_TemplateState (  )
{
} 

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateState :: askForWriteCount
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateState :: askForWriteCount  ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  if (szName.Compare(OBJ_TITLE) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_DBSTATE) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_STATEICON) == 0) {
    nReturn = 0;
  } else if (szName.Compare(OBJ_STATEICON_UNKNOWN) == 0) {
    nReturn = (m_oState.Value( ) == DBMCLI_DBSTATE_UNKNOWN) ? 1 : 0;
  } else if (szName.Compare(OBJ_STATEICON_OFFLINE) == 0) {
    nReturn = (m_oState.Value( ) == DBMCLI_DBSTATE_OFFLINE) ? 1 : 0;
  } else if (szName.Compare(OBJ_STATEICON_COLD) == 0) {
    nReturn = (m_oState.Value( ) == DBMCLI_DBSTATE_COLD) ? 1 : 0;
  } else if (szName.Compare(OBJ_STATEICON_WARM) == 0) {
    nReturn = (m_oState.Value( ) == DBMCLI_DBSTATE_WARM) ? 1 : 0;
  } else if (szName.Compare(OBJ_CONVCACHE) == 0) {
    // VERSIONCHECK
	  if (m_oState.GetDatabase().NodeInfo().Version().GetNumber() < 0x7040000) {
	    nReturn = 1;
    } else {
      nReturn = 0;
    }
  } else if (szName.Compare(OBJ_BTN_OFFLINE) == 0) {
    nReturn = (m_oState.Value( ) == DBMCLI_DBSTATE_COLD ||
               m_oState.Value( ) == DBMCLI_DBSTATE_WARM ||
               m_oState.Value( ) == DBMCLI_DBSTATE_UNKNOWN) ? 1 : 0;
  } else if (szName.Compare(OBJ_BTN_OFFLINE_DISABLED) == 0) {
    nReturn = (m_oState.Value( ) == DBMCLI_DBSTATE_OFFLINE) ? 1 : 0;
  } else if (szName.Compare(OBJ_BTN_COLD) == 0) {
    nReturn = (m_oState.Value( ) == DBMCLI_DBSTATE_OFFLINE ||
               m_oState.Value( ) == DBMCLI_DBSTATE_WARM ||
               m_oState.Value( ) == DBMCLI_DBSTATE_UNKNOWN) ? 1 : 0;
  } else if (szName.Compare(OBJ_BTN_COLD_DISABLED) == 0) {
    nReturn = (m_oState.Value( ) == DBMCLI_DBSTATE_COLD) ? 1 : 0;
  } else if (szName.Compare(OBJ_BTN_WARM) == 0) {
    nReturn = (m_oState.Value( ) == DBMCLI_DBSTATE_OFFLINE ||
               m_oState.Value( ) == DBMCLI_DBSTATE_COLD ||
               m_oState.Value( ) == DBMCLI_DBSTATE_UNKNOWN) ? 1 : 0;
  } else if (szName.Compare(OBJ_BTN_WARM_DISABLED) == 0) {
    nReturn = (m_oState.Value( ) == DBMCLI_DBSTATE_WARM) ? 1 : 0;
  } else if (szName.Compare(OBJ_SENDERROR) == 0) {
    nReturn = m_bSendError ? 1 : 0;
  } else {
    nReturn = 1;
  } 

  return nReturn;
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateState :: askForValue
  -----------------------------------------------------------------------------
*/
Tools_DynamicUTF8String DBMWeb_TemplateState :: askForValue ( const Tools_DynamicUTF8String & szName )
{
  const SAPDB_Char * pReturn = NULL;

  DBMCli_String sValue;
  
  if (szName.Compare(OBJ_TITLE) == 0) {
    sValue = m_oState.GetDatabase().DatabaseOnServer();
    pReturn = sValue;
  } else if (szName.Compare(OBJ_DBSTATE) == 0) {
    sValue.Format("%d", m_oState.Value()); 
    pReturn = sValue;
  } else if (szName.Compare(OBJ_STATEICON) == 0) {
    switch (m_oState.Value()) {
      case 	DBMCLI_DBSTATE_UNKNOWN:
        pReturn = "dboltp1";
        break;
      case 	DBMCLI_DBSTATE_OFFLINE:
        pReturn = "dboltp2";
        break;
      case 	DBMCLI_DBSTATE_COLD:
        pReturn = "dboltp3";
        break;
      case 	DBMCLI_DBSTATE_WARM:
        pReturn = "dboltp4";
        break;
      default:
	      pReturn = "dboltp0";
        break;
    }
  } else if (szName.Compare(OBJ_STATETEXT) == 0) {
    pReturn = m_oState.ValueStr();
  } else if (szName.Compare(OBJ_DATAPCT) == 0) {
    sValue.Format("%d%%", m_oState.DataPercent()); 
    pReturn = sValue;
  } else if (szName.Compare(OBJ_LOGPCT) == 0) {
    sValue.Format("%d%%", m_oState.LogPercent()); 
    pReturn = sValue;
  } else if (szName.Compare(OBJ_SESPCT) == 0) {
    sValue.Format("%d%%", m_oState.SessionsPercent()); 
    pReturn = sValue;
  } else if (szName.Compare(OBJ_CONVCACHE) == 0) {
    if (m_oState.Value() == DBMCLI_DBSTATE_WARM) {
      sValue.Format("%d%%", m_oState.ConverterCacheHitRate()); 
    } else {
      sValue = "&nbsp;";
    }
    pReturn = sValue;
  } else if (szName.Compare(OBJ_DATACACHE) == 0) {
    if (m_oState.Value() == DBMCLI_DBSTATE_WARM) {
      sValue.Format("%d%%", m_oState.DataCacheHitRate()); 
    } else {
      sValue = "&nbsp;";
    }
    pReturn = sValue;
  } else if (szName.Compare(OBJ_AUTOLOG) == 0) {
    if (m_oState.Value() == DBMCLI_DBSTATE_WARM) {
      if( m_oState.AutoLog( ) ) {
        sValue = DBMCLI_VAL_ON;
      } else {
        sValue = DBMCLI_VAL_OFF;
      }
    } else {
      sValue = "&nbsp;";
    }
    pReturn = sValue;
  } else if (szName.Compare(OBJ_BADIDX) == 0) {
    if (m_oState.Value() == DBMCLI_DBSTATE_WARM) {
      if( m_oState.BadIndexes( ) > 0 ) {
        sValue.Format("%d", m_oState.BadIndexes()); 
      } else {
        sValue = DBMCLI_VAL_NO;
      }
    } else {
      sValue = "&nbsp;";
    }
    pReturn = sValue;
  } else if (szName.Compare(OBJ_DBFULL) == 0) {
    if (m_oState.Value() == DBMCLI_DBSTATE_WARM) {
      if( m_oState.DatabaseFull( ) ) {
        sValue = DBMCLI_VAL_YES;
      } else {
        sValue = DBMCLI_VAL_NO;
      }
    } else {
      sValue = "&nbsp;";
    }
    pReturn = sValue;
  } else if (szName.Compare(OBJ_KNLTRACE) == 0) {
    if (m_oState.Value() == DBMCLI_DBSTATE_WARM) {
      if( m_oState.KernelTrace( ) ) {
        sValue = DBMCLI_VAL_ON;
      } else {
        sValue = DBMCLI_VAL_OFF;
      }
    } else {
      sValue = "&nbsp;";
    }
    pReturn = sValue;
  }

  return _Tools_UTF8Ptr(pReturn);
} 

