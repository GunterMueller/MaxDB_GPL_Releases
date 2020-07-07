/*



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

/*!
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateMsgBox.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  MsgBox page
  version:      7.3 and higher
  -----------------------------------------------------------------------------
                          Copyright (c) 2000-2005 SAP AG
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "DBM/Web/DBMWeb_TemplateMsgBox.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define TEMPLATE_NAME   "DBMMsgBox.htm"

#define OBJ_TITLE       "GroupBoxTitle"

#define OBJ_MSG         "Message"   
#define OBJ_MSGLST      "MessageList"   
#define OBJ_MSGROW      "MessageRow"   
#define OBJ_INFO        "Info"   
#define OBJ_WARN        "Warning"   
#define OBJ_ERRO        "Error"   
#define OBJ_QUES        "Question"   
#define OBJ_CRIT        "Critical"   
#define OBJ_NUM         "Number"   
#define OBJ_TEXT        "Text"   
#define OBJ_BUTTON      "Button"   

#define DEF_BTN_TEXT    "OK"
#define DEF_BTN_ACTION  "javascript:history.back()"
#define DEF_BTN_TARGET  "_self"
/*
  =============================================================================
  class:        DBMWeb_TemplateMsgBox
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMWeb_TemplateMsgBox :: DBMWeb_TemplateMsgBox
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateMsgBox :: DBMWeb_TemplateMsgBox ( sapdbwa_WebAgent        & wa,
                                                 DBMWeb_TemplMsgBox_Type   nType,
                                                 const DBMCli_String     & sNumber,
                                                 const DBMCli_String     & sText,
                                                 const DBMCli_String     & sTitle)
                        : Tools_Template( wa, _Tools_UTF8Ptr( TEMPLATE_NAME )),
                        m_nType( nType ),
                        m_sNumber( sNumber ),
                        m_sText( sText ),
                        m_sTitle( sTitle )
{
  m_oMsgList.ClearMessageList();
  m_pCurrentMsg = NULL;

  m_sButtonText   = DEF_BTN_TEXT;
  m_sButtonAction = DEF_BTN_ACTION;
  m_sButtonTarget = DEF_BTN_TARGET;
} 

DBMWeb_TemplateMsgBox :: DBMWeb_TemplateMsgBox ( sapdbwa_WebAgent        & wa,
                                                 DBMWeb_TemplMsgBox_Type   nType,
                                                 const DBMCli_String     & sNumber,
                                                 const DBMCli_String     & sText)
                        : Tools_Template( wa, _Tools_UTF8Ptr( TEMPLATE_NAME )),
                        m_nType( nType ),
                        m_sNumber( sNumber ),
                        m_sText( sText )
{
  switch (nType) {
    case DBMWEB_TEMPLMSGBOX_INFO:
      m_sTitle = "Information";
      break;
    case DBMWEB_TEMPLMSGBOX_WARNING:
      m_sTitle = "Warning";
      break;
    case DBMWEB_TEMPLMSGBOX_ERROR:
      m_sTitle = "Error";
      break;
    case DBMWEB_TEMPLMSGBOX_QUESTION:
      m_sTitle = "Question";
      break;
    case DBMWEB_TEMPLMSGBOX_CRITICAL:
      m_sTitle = "Critical Error";
      break;
    default:
      m_sTitle = "Message";
      break;
  }

  m_oMsgList.ClearMessageList();
  m_pCurrentMsg = NULL;

  m_sButtonText   = DEF_BTN_TEXT;
  m_sButtonAction = DEF_BTN_ACTION;
} 

DBMWeb_TemplateMsgBox :: DBMWeb_TemplateMsgBox ( sapdbwa_WebAgent        & wa,
                                                 DBMWeb_TemplMsgBox_Type   nType,
                                                 const Msg_List    & oMsgList,
                                                 const DBMCli_String     & sTitle)
                        : Tools_Template( wa, _Tools_UTF8Ptr( TEMPLATE_NAME )),
                        m_nType( nType ),
                        m_oMsgList( oMsgList ),
                        m_sTitle( sTitle )
{
  m_sNumber.Empty();
  m_sText.Empty();
  m_pCurrentMsg = NULL;

  m_sButtonText   = DEF_BTN_TEXT;
  m_sButtonAction = DEF_BTN_ACTION;
} 

/*
  -----------------------------------------------------------------------------
  public destructor DBMWeb_TemplateMsgBox :: ~DBMWeb_TemplateMsgBox
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateMsgBox :: ~DBMWeb_TemplateMsgBox (  )
{
} 


/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateMsgBox :: askForWriteCount
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateMsgBox :: askForWriteCount  ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  if (szName.Compare(OBJ_TITLE) == 0) {

    nReturn = 1;

  } else if (szName.Compare(OBJ_MSG) == 0) {

    nReturn = (m_sText.IsEmpty()) ? 0 : 1;

  } else if (szName.Compare(OBJ_MSGLST) == 0) {

    nReturn = (m_oMsgList.IsEmpty()) ? 0 : 1;

  } else if (szName.Compare(OBJ_MSGROW) == 0) {

    nReturn = (m_oMsgList.IsEmpty()) ? 0 : m_oMsgList.NumOfMessages();
    m_pCurrentMsg = &m_oMsgList;

  } else if (szName.Compare(OBJ_INFO) == 0) {

    nReturn = (m_nType == DBMWEB_TEMPLMSGBOX_INFO) ? 1 : 0;

  } else if (szName.Compare(OBJ_WARN) == 0) {

    nReturn = (m_nType == DBMWEB_TEMPLMSGBOX_WARNING) ? 1 : 0;

  } else if (szName.Compare(OBJ_ERRO) == 0) {

    nReturn = (m_nType == DBMWEB_TEMPLMSGBOX_ERROR) ? 1 : 0;

  } else if (szName.Compare(OBJ_QUES) == 0) {

    nReturn = (m_nType == DBMWEB_TEMPLMSGBOX_QUESTION) ? 1 : 0;

  } else if (szName.Compare(OBJ_CRIT) == 0) {

    nReturn = (m_nType == DBMWEB_TEMPLMSGBOX_CRITICAL) ? 1 : 0;

  } else if (szName.Compare(OBJ_NUM) == 0) {

    if( m_oMsgList.IsEmpty() ) {
      nReturn = (m_sNumber.IsEmpty()) ? 0 : 1;
    } else {
      nReturn = 1;
    }

  } else if (szName.Compare(OBJ_TEXT) == 0) {

    nReturn = 1;

  } else if (szName.Compare(OBJ_BUTTON) == 0) {

    nReturn = 1;

  } 

  return nReturn;
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateMsgBox :: askForValue
  -----------------------------------------------------------------------------
*/
Tools_DynamicUTF8String DBMWeb_TemplateMsgBox :: askForValue ( const Tools_DynamicUTF8String & szName )
{
  const SAPDB_Char * pReturn = NULL;

  DBMCli_String sValue;

  if (szName.Compare(OBJ_TITLE) == 0) {

    pReturn = m_sTitle;  

  } else if (szName.Compare(OBJ_NUM) == 0) {

    if (m_oMsgList.IsEmpty()) {
      pReturn = m_sNumber;
    } else {
      while (m_pCurrentMsg != NULL && m_pCurrentMsg->IsEmpty()) {
        m_pCurrentMsg = m_pCurrentMsg->NextMessage();
      } 
      if (m_pCurrentMsg != NULL) {
        sValue.Format("%d", m_pCurrentMsg->ID());
        pReturn = sValue;
      }
    }

  } else if (szName.Compare(OBJ_TEXT) == 0) {

    if (m_oMsgList.IsEmpty()) {
      pReturn = m_sText;
    } else {
      if (m_pCurrentMsg != NULL) {
        pReturn = m_pCurrentMsg->Message();
      } 
      m_pCurrentMsg = m_pCurrentMsg->NextMessage();
    }

  } else if (szName.Compare(OBJ_BUTTON) == 0) {

    sValue = "'" + m_sButtonTarget + "','" + m_sButtonText + "','" + m_sButtonAction + "'";
    pReturn = sValue;

  }

  return _Tools_UTF8Ptr(pReturn);
} 

