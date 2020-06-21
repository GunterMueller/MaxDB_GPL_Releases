/*!
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateLogon.cpp
  -----------------------------------------------------------------------------
  responsible:  
  special area: Database Manager WebAgent Extension
  description:  a class for the logon mask of the Web DBM
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
#include <string.h>
#include "DBM/Web/DBMWeb_TemplateLogon.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define TEMPLATE_NAME     "DBMLogon.htm"

#define OBJ_MSG           "Message"   
#define OBJ_MSGNUM        "MsgNumber"   
#define OBJ_MSGTEXT       "MsgText"   
#define OBJ_SERVER        "Server"
#define OBJ_DATABASE      "Database"
#define OBJ_USER          "User"

/*
  =============================================================================
  class:        DBMWeb_TemplateLogon
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMWeb_TemplateLogon :: DBMWeb_TemplateLogon
  -----------------------------------------------------------------------------
*/

DBMWeb_TemplateLogon :: DBMWeb_TemplateLogon ( sapdbwa_WebAgent & wa ) 
                        : Tools_Template( wa, _Tools_UTF8Ptr( TEMPLATE_NAME )),
                        m_sServer(""),
                        m_sDatabase(""),
                        m_sUser("")
{ 
  m_oMsgList.ClearMessageList();
  m_pCurrentMsg = NULL;
} 

DBMWeb_TemplateLogon :: DBMWeb_TemplateLogon ( sapdbwa_WebAgent & wa,
                                               DBMCli_String & sServer,
                                               DBMCli_String & sDatabase,
                                               DBMCli_String & sUser)
                        : Tools_Template( wa, _Tools_UTF8Ptr( TEMPLATE_NAME )),
                        m_sServer(sServer),
                        m_sDatabase(sDatabase),
                        m_sUser(sUser)
{
  m_oMsgList.ClearMessageList();
  m_pCurrentMsg = NULL;
} 

DBMWeb_TemplateLogon :: DBMWeb_TemplateLogon ( sapdbwa_WebAgent & wa,
                                               DBMCli_String & sServer,
                                               DBMCli_String & sDatabase,
                                               DBMCli_String & sUser,
                                               const Msg_List & oMsgList)
                        : Tools_Template( wa, _Tools_UTF8Ptr( TEMPLATE_NAME )),
                        m_sServer(sServer),
                        m_sDatabase(sDatabase),
                        m_sUser(sUser),
                        m_oMsgList(oMsgList)
{
  m_pCurrentMsg = NULL;
} 


/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateLogon :: askForWriteCount
  -----------------------------------------------------------------------------
 */
SAPDB_Int2 DBMWeb_TemplateLogon :: askForWriteCount  ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  if (szName.Compare(OBJ_MSG) == 0) {
    nReturn = (m_oMsgList.IsEmpty()) ? 0 : 1;
    m_pCurrentMsg = &m_oMsgList;
  } else if (szName.Compare(OBJ_MSGNUM) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_MSGTEXT) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_SERVER) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_DATABASE) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_USER) == 0) {
    nReturn = 1;
  } // end if

  return nReturn;
} // end DBMWeb_TemplateLogon :: askForWriteCount

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateLogon :: askForValue
  -----------------------------------------------------------------------------
 */
Tools_DynamicUTF8String DBMWeb_TemplateLogon :: askForValue  ( const Tools_DynamicUTF8String & szName )
{
  const SAPDB_Char * pReturn = NULL;

  DBMCli_String sValue;

  if (szName.Compare(OBJ_MSGNUM) == 0) {
    if (m_pCurrentMsg != NULL) {
      sValue.Format("%d", m_pCurrentMsg->ID());
      pReturn = sValue;
    }
  } else if (szName.Compare(OBJ_MSGTEXT) == 0) {
    if (m_pCurrentMsg != NULL) {
      pReturn = m_pCurrentMsg->Message();
    } 
  } else if (szName.Compare(OBJ_SERVER) == 0) {
    pReturn = m_sServer;
  } else if (szName.Compare(OBJ_DATABASE) == 0) {
    pReturn = m_sDatabase;
  } else if (szName.Compare(OBJ_USER) == 0) {
    pReturn = m_sUser;
  } // end if

  if (pReturn == NULL) {
    pReturn = "";
  } // end if

  return _Tools_UTF8Ptr(pReturn);
} // end DBMWeb_TemplateLogon :: askForValue
