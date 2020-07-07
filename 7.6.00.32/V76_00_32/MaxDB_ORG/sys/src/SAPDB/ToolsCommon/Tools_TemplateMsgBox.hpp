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
 
  module: Tools_TemplateMsgBox.hpp
 
  -----------------------------------------------------------------------------
 
  responsible:  BerndV
 
  special area: universal stuff for web access
   
  description:  a class for creating simple "Messageboxes" in SAP MiniApps style

  version:      7.3 and higher 
  -----------------------------------------------------------------------------
 
                          Copyright (c) 2000-2005 SAP AG
 
  -----------------------------------------------------------------------------
*/

#ifndef _Tools_TemplateMsgBox_HPP_
#define _Tools_TemplateMsgBox_HPP_

#include "ToolsCommon/Tools_Template.hpp"
#include "Messages/Msg_List.hpp"

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */

/*!
  -----------------------------------------------------------------------------
  Class:        Tools_TemplateMsgBox
  -----------------------------------------------------------------------------

  description:  Tools 

                An instance of this class represents a HTML-Page with a Table
  -----------------------------------------------------------------------------
 */
class Tools_TemplateMsgBox : public Tools_Template 
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     Tools_TemplateMsgBox
      -------------------------------------------------------------------------
      description:  Constructor

      arguments:    wa [IN]  - webagent instance
      -------------------------------------------------------------------------
     */
     Tools_TemplateMsgBox  ( sapdbwa_WebAgent & wa ) : Tools_Template ( wa, _Tools_UTF8Ptr("SAPDBMsgBox.htm") ) 
        {
          m_bLogo         = false;
          m_strTitle      = "Information";
          m_strMessage    = "";
          m_strBtnText    = "Back";
          m_strBtnAction  = "javascript:history.back()";
          m_strButton     = "";
          m_oMsgList.ClearMessageList();
          m_pCurrentMsg   = NULL;
          m_strBtnTarget  = "_self";
        } 
 
    /*!
      -------------------------------------------------------------------------
      function:     ~Tools_TemplateMsgBox
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
     */
     ~Tools_TemplateMsgBox ( ) {}

    /*!
      -------------------------------------------------------------------------
      function:     enableLogo
      -------------------------------------------------------------------------
      description:  enable the Logo

      arguments:    bEnable   [IN] - enable or disable the logo (default: disabled)

      return value: a reference to this object
      -------------------------------------------------------------------------
     */
     Tools_TemplateMsgBox & enableLogo ( bool bEnable = true )
        {
          m_bLogo = bEnable;
          return *this;
        } 

    /*!
      -------------------------------------------------------------------------
      function:     setTitle
      -------------------------------------------------------------------------
      description:  sets the title message box

      arguments:    szText   [IN] - the title (default: "Information")

      return value: a reference to this object
      -------------------------------------------------------------------------
     */
     Tools_TemplateMsgBox & setTitle ( const SAPDB_UTF8 * szText )
        {
          m_strTitle = szText;
          return *this;
        } 

    /*!
      -------------------------------------------------------------------------
      function:     setMessage
      -------------------------------------------------------------------------
      description:  sets the text of the message

      arguments:    szText  [IN] - the message text

      return value: a reference to this object
      -------------------------------------------------------------------------
     */
     Tools_TemplateMsgBox & setMessage ( const SAPDB_UTF8 * szText )
        {
          m_strMessage = szText;
          return *this;
        } 

    /*!
      -------------------------------------------------------------------------
      function:     setMessage
      -------------------------------------------------------------------------
      description:  sets the text of the message

      arguments:    szText  [IN] - the message text

      return value: a reference to this object
      -------------------------------------------------------------------------
     */
     Tools_TemplateMsgBox & setMessage ( const Tools_DynamicUTF8String&  szText )
        {
          m_strMessage = szText;
          return *this;
        } 

    /*!

      -------------------------------------------------------------------------
      function:     setMessage
      -------------------------------------------------------------------------
      description:  sets the text of the message

      arguments:    szText  [IN] - the message text

      return value: a reference to this object
      -------------------------------------------------------------------------
     */
     Tools_TemplateMsgBox & setMessage ( Msg_List & oMsgList )
        {
          m_oMsgList = oMsgList;
          return *this;
        } 
                     
    /*!
      -------------------------------------------------------------------------
      function:     setButtonText
      -------------------------------------------------------------------------
      description:  sets the text of the Button

      arguments:    szText  [IN] - text for button (Default: "back")

      return value: a reference to this object
      -------------------------------------------------------------------------
     */
     Tools_TemplateMsgBox & setButtonText ( const SAPDB_UTF8 * szText )
        {
          m_strBtnText = szText;
          return *this;
        } 

    /*!
      -------------------------------------------------------------------------
      function:     setButtonAction
      -------------------------------------------------------------------------
      description:  sets the action of the Button

      arguments:    szText  [IN] - action for button 
                                     (Default: "javascript:history.back()")

      return value: a reference to this object
      -------------------------------------------------------------------------
     */
     Tools_TemplateMsgBox & setButtonAction ( const SAPDB_UTF8 * szText )
        {
          m_strBtnAction = szText;
          return *this;
        } 

       /*!
      -------------------------------------------------------------------------
      function:     setButtonTarget
      -------------------------------------------------------------------------
      description:  sets the target of the Button

      arguments:    szText  [IN] - target for button 
                                     (Default: "javascript:history.back()")

      return value: a reference to this object
      -------------------------------------------------------------------------
     */
     Tools_TemplateMsgBox & setButtonTarget ( const SAPDB_UTF8 * szText )
        {
          m_strBtnTarget = szText;
          return *this;
        } 

  private:
     virtual SAPDB_Int2 askForWriteCount  ( const Tools_DynamicUTF8String & szName );

     virtual Tools_DynamicUTF8String askForValue ( const Tools_DynamicUTF8String & szName );

     bool                    m_bLogo;
     Tools_DynamicUTF8String m_strTitle;
     Tools_DynamicUTF8String m_strMessage;
     Tools_DynamicUTF8String m_strBtnText;
     Tools_DynamicUTF8String m_strBtnAction;
     Tools_DynamicUTF8String m_strButton;
     Tools_DynamicUTF8String m_strBtnTarget;
     Msg_List    m_oMsgList;
     Msg_List *  m_pCurrentMsg;
}; 

/*! EndClass: Tools_TemplateMsgBox */

#endif // _Tools_TemplateMsgBox_HPP_

