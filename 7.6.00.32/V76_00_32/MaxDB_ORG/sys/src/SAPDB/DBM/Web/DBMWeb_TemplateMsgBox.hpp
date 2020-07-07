/*! 
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateMsgBox.hpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  MsgBox page
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

#ifndef _DBMWEB_TEMPLATEMSGBOX_HPP_
#define _DBMWEB_TEMPLATEMSGBOX_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "ToolsCommon/Tools_Template.hpp"
#include "Messages/Msg_List.hpp"
#include "DBM/Cli/DBMCli_Stuff.hpp"

/*
  -----------------------------------------------------------------------------
  typedefs
  -----------------------------------------------------------------------------
*/
typedef enum  
{ 
	DBMWEB_TEMPLMSGBOX_DEFAULT  = 0,
	DBMWEB_TEMPLMSGBOX_INFO     = 1,
	DBMWEB_TEMPLMSGBOX_WARNING  = 2,
	DBMWEB_TEMPLMSGBOX_ERROR    = 3,
	DBMWEB_TEMPLMSGBOX_QUESTION = 4,
	DBMWEB_TEMPLMSGBOX_CRITICAL = 5
} DBMWeb_TemplMsgBox_Type;

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/

/*!
  -----------------------------------------------------------------------------
  Class:        DBMWeb_TemplateMsgBox
  -----------------------------------------------------------------------------
  description:  An instance of this class represents the HTML-MsgBox-Page.
  -----------------------------------------------------------------------------
*/
class DBMWeb_TemplateMsgBox : public Tools_Template 
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMWeb_TemplateMsgBox
      -------------------------------------------------------------------------
      description:  Constructor
      arguments:    wa [IN]  - webagent instance
      prototypes:   3
      -------------------------------------------------------------------------
     */
     DBMWeb_TemplateMsgBox ( sapdbwa_WebAgent        & wa,
                             DBMWeb_TemplMsgBox_Type   nType,
                             const DBMCli_String     & sNumber,
                             const DBMCli_String     & sText);

     DBMWeb_TemplateMsgBox ( sapdbwa_WebAgent        & wa,
                             DBMWeb_TemplMsgBox_Type   nType,
                             const DBMCli_String     & sNumber,
                             const DBMCli_String     & sText,
                             const DBMCli_String     & sTitle);

     DBMWeb_TemplateMsgBox ( sapdbwa_WebAgent        & wa,
                             DBMWeb_TemplMsgBox_Type   nType,
                             const Msg_List    & oMsgList,
                             const DBMCli_String     & sTitle);


    /*!
      -------------------------------------------------------------------------
      function:     ~DBMWeb_TemplateMsgBox
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
     */
     ~DBMWeb_TemplateMsgBox ( );

    /*!
      -------------------------------------------------------------------------
      function:     SetButtonText
      -------------------------------------------------------------------------
      description:  sets the text of the Button
      arguments:    sText [IN] - text for button ("OK")
      return value: a reference to this object
      -------------------------------------------------------------------------
     */
     DBMWeb_TemplateMsgBox & SetButtonText ( const DBMCli_String & sText )
     {
       m_sButtonText = sText;
       return *this;
     } 

    /*!
      -------------------------------------------------------------------------
      function:     SetButtonAction
      -------------------------------------------------------------------------
      description:  sets the action of the Button
      arguments:    sText [IN] - action for button ("javascript:history.back()")
      return value: a reference to this object
      -------------------------------------------------------------------------
     */
     DBMWeb_TemplateMsgBox & SetButtonAction ( const DBMCli_String sText )
     {
       m_sButtonAction = sText;
       return *this;
     } 

    /*!
      -------------------------------------------------------------------------
      function:     SetButtonTarget
      -------------------------------------------------------------------------
      description:  sets the target of the Button
      arguments:    sText [IN] - target for button ("_self")
      return value: a reference to this object
      -------------------------------------------------------------------------
     */
     DBMWeb_TemplateMsgBox & SetButtonTarget ( const DBMCli_String sText )
     {
       m_sButtonTarget = sText;
       return *this;
     } 

  private:
    virtual SAPDB_Int2              askForWriteCount ( const Tools_DynamicUTF8String & szName );
    virtual Tools_DynamicUTF8String askForValue      ( const Tools_DynamicUTF8String & szName );
                                              
  private:
    DBMCli_String           m_sTitle;
    DBMWeb_TemplMsgBox_Type m_nType;
    DBMCli_String           m_sNumber;
    DBMCli_String           m_sText;

    Msg_List    m_oMsgList;
    Msg_List *  m_pCurrentMsg;

    DBMCli_String           m_sButtonText;
    DBMCli_String           m_sButtonAction;
    DBMCli_String           m_sButtonTarget;
}; 

/*! EndClass: DBMWeb_TemplateMsgBox */

#endif // _DBMWEB_TEMPLATEMSGBOX_HPP_

