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

  module: Tools_TemplateMsgBox.cpp

  -----------------------------------------------------------------------------

  responsible:  BerndV

  special area: universal stuff for web access

  description:  a class for creating simple "Messageboxes" in SAP MiniApps style

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
#include <string.h>
#include "ToolsCommon/Tools_TemplateMsgBox.hpp"

/*
  -----------------------------------------------------------------------------
  private macros
  -----------------------------------------------------------------------------
 */
#define OBJ_LOGO          "Logo"
#define OBJ_TITLE         "Title"
#define OBJ_MESSAGE       "Message"
#define OBJ_BUTTON        "Button"
#define OBJ_MSGLIST       "MessageList"
#define OBJ_MSGNO         "MessageNumber"
#define OBJ_MSGTXT        "MessageText"

/*
  =============================================================================
  class:        Tools_TemplateMsgBox
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  private function Tools_TemplateMsgBox :: askForWriteCount
  -----------------------------------------------------------------------------
 */
SAPDB_Int2 Tools_TemplateMsgBox :: askForWriteCount  ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  if (szName.Compare(OBJ_LOGO) == 0) {
    nReturn = (m_bLogo) ? 1 : 0;
  } else if (szName.Compare(OBJ_TITLE) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_MESSAGE) == 0) {
    nReturn = m_strMessage.Empty() ? 0 : 1;
  } else if (szName.Compare(OBJ_MSGLIST) == 0) { 
    nReturn = m_oMsgList.IsEmpty() ? 0 : m_oMsgList.NumOfMessages();
    m_pCurrentMsg = &m_oMsgList;
  } else if (szName.Compare(OBJ_MSGNO) == 0) {   
    nReturn = 1;
  } else if (szName.Compare(OBJ_MSGTXT) == 0) {  
    nReturn = 1;
  } else if (szName.Compare(OBJ_BUTTON) == 0) {
    nReturn = (m_strBtnText.Empty()) ? 0 : 1;
  } // end if

  return nReturn;
} // end Tools_TemplateMsgBox :: askForWriteCount

/*
  -----------------------------------------------------------------------------
  private function Tools_TemplateMsgBox :: askForValue
  -----------------------------------------------------------------------------
 */
Tools_DynamicUTF8String Tools_TemplateMsgBox :: askForValue  ( const Tools_DynamicUTF8String & szName )
{
  Tools_DynamicUTF8String strReturn;

  if (szName.Compare(OBJ_TITLE) == 0) {
    strReturn.Assign(m_strTitle);
  } else if (szName.Compare(OBJ_MESSAGE) == 0) {
    return m_strMessage;
  } else if (szName.Compare(OBJ_MSGNO) == 0) {   
    while (m_pCurrentMsg != NULL && m_pCurrentMsg->IsEmpty()) {
      m_pCurrentMsg = m_pCurrentMsg->NextMessage();
    } // end while
    if (m_pCurrentMsg != NULL) {
      strReturn.ConvertFromInt(m_pCurrentMsg->ID());
    } // end if
  } else if (szName.Compare(OBJ_MSGTXT) == 0) {  
    if (m_pCurrentMsg != NULL) {
      strReturn.Assign(m_pCurrentMsg->Message());
    } // end if
    m_pCurrentMsg = m_pCurrentMsg->NextMessage();
  } else if (szName.Compare(OBJ_BUTTON) == 0) {
    m_strButton.Assign("'")
               .Append(m_strBtnText)
               .Append("','")
               .Append(m_strBtnAction)
			         .Append("','")
			         .Append(m_strBtnTarget)
               .Append("'");
	
    strReturn.Assign(m_strButton);
  } // end if

  return strReturn;
} // end Tools_TemplateMsgBox :: askForValue
