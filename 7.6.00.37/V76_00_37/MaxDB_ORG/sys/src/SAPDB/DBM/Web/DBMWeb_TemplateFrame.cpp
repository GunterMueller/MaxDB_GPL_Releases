/*!
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateFrame.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  Frame page
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
#include "DBM/Web/DBMWeb_TemplateFrame.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define TEMPLATE_NAME    "DBMFrame.htm"

#define OBJ_LOGON        "LogonFrame*"
#define OBJ_WORK         "WorkFrame*"
#define OBJ_WIZARD       "WizardFrame*"
#define OBJ_SESSIONID    "SessionID"

#define VAL_SESSIONID    "SessionID"
#define VAL_WORKURL      "WorkURL"
#define VAL_ACTION       "Action"
#define VAL_SERVER       "Server"
#define VAL_DATABASE     "Database"
#define VAL_USER         "User"
#define VAL_PWD          "Password"
#define VAL_NAME         "Name"

#define DEFAULT_WORK_URL "/WARoot/HTML/DBMEmpty.htm"
/*
  =============================================================================
  class:        DBMWeb_TemplateFrame
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMWeb_TemplateFrame :: DBMWeb_TemplateFrame
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateFrame :: DBMWeb_TemplateFrame ( sapdbwa_WebAgent       & wa,
                                               DBMWeb_TemplFrame_Type   nFrameType,
                                               DBMCli_String          & sSessionID)
                        : Tools_Template( wa, _Tools_UTF8Ptr( TEMPLATE_NAME )),
                        m_nFrameType( nFrameType ),
                        m_sSessionID( sSessionID ),
                        m_sWorkURL(DEFAULT_WORK_URL)
{
}  // end DBMWeb_TemplateFrame :: DBMWeb_TemplateFrame

DBMWeb_TemplateFrame :: DBMWeb_TemplateFrame ( sapdbwa_WebAgent       & wa,
                                               DBMWeb_TemplFrame_Type   nFrameType,
                                               DBMCli_String          & sSessionID,
                                               DBMCli_String          & sName)
                        : Tools_Template( wa, _Tools_UTF8Ptr( TEMPLATE_NAME )),
                        m_nFrameType( nFrameType ),
                        m_sSessionID( sSessionID ),
                        m_sName( sName ),
                        m_sWorkURL(DEFAULT_WORK_URL)
{
}  // end DBMWeb_TemplateFrame :: DBMWeb_TemplateFrame

DBMWeb_TemplateFrame :: DBMWeb_TemplateFrame ( sapdbwa_WebAgent       & wa,
                                               DBMWeb_TemplFrame_Type   nFrameType,
                                               DBMCli_String          & sSessionID,
                                               DBMCli_String          & sAction,
                                               DBMCli_String          & sServer,
                                               DBMCli_String          & sDatabase,
                                               DBMCli_String          & sUser,
                                               DBMCli_String          & sPassword)

                        : Tools_Template( wa, _Tools_UTF8Ptr( TEMPLATE_NAME )),
                        m_nFrameType( nFrameType ),
                        m_sSessionID( sSessionID ),
                        m_sWorkURL(DEFAULT_WORK_URL),
                        m_sAction( sAction ),
                        m_sServer( sServer ),
                        m_sDatabase( sDatabase ),
                        m_sUser( sUser ),
                        m_sPassword( sPassword )
{
}  // end DBMWeb_TemplateFrame :: DBMWeb_TemplateFrame


/*
  -----------------------------------------------------------------------------
  public destructor DBMWeb_TemplateFrame :: ~DBMWeb_TemplateFrame
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateFrame :: ~DBMWeb_TemplateFrame (  )
{
}  // end DBMWeb_TemplateFrame :: ~DBMWeb_TemplateFrame


/*
  -----------------------------------------------------------------------------
  public function DBMWeb_TemplateFrame :: setWorkURL
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateFrame & DBMWeb_TemplateFrame :: setWorkURL(const DBMCli_String & sWorkURL )
{
  if (sWorkURL.IsEmpty()) {
    m_sWorkURL = DEFAULT_WORK_URL;
  } else {
    m_sWorkURL = sWorkURL;
  } // end if
  return *this;
} // end DBMWeb_TemplateFrame :: setWorkURL

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateFrame :: askForWriteCount
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateFrame :: askForWriteCount  ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  if (szName.Compare(OBJ_LOGON) == 0) {
    nReturn = (m_nFrameType == DBMWEB_TEMPLFRAME_LOGON) ? 1 : 0;
  } else if (szName.Compare(OBJ_WORK) == 0) {
    nReturn = (m_nFrameType == DBMWEB_TEMPLFRAME_WORK) ? 1 : 0;
  } else if (szName.Compare(OBJ_WIZARD) == 0) {
    nReturn = (m_nFrameType == DBMWEB_TEMPLFRAME_WIZARD) ? 1 : 0;
  } else if (szName.Compare(OBJ_SESSIONID) == 0) {
    nReturn = 1;
  } // end if

  return nReturn;
} // end DBMWeb_TemplateFrame :: askForWriteCount

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateFrame :: askForValue
  -----------------------------------------------------------------------------
*/
Tools_DynamicUTF8String DBMWeb_TemplateFrame :: askForValue ( const Tools_DynamicUTF8String & szName )
{
  const SAPDB_Char * pReturn = NULL;

  if (szName.Compare(VAL_ACTION) == 0) {
    pReturn = m_sAction;
  } else if (szName.Compare(VAL_SERVER) == 0) {
    pReturn = m_sServer;
  } else if (szName.Compare(VAL_DATABASE) == 0) {
    pReturn = m_sDatabase;
  } else if (szName.Compare(VAL_USER) == 0) {
    pReturn = m_sUser;
  } else if (szName.Compare(VAL_PWD) == 0) {
    pReturn = m_sPassword;
  } else if (szName.Compare(VAL_WORKURL) == 0) {
    pReturn = m_sWorkURL;
  } else if (szName.Compare(VAL_SESSIONID) == 0) {
    pReturn = m_sSessionID;
  } else if (szName.Compare(VAL_NAME) == 0) {
    pReturn = m_sName;
  } // end if

  return _Tools_UTF8Ptr(pReturn);
} // end DBMWeb_TemplateFrame :: askForValue

