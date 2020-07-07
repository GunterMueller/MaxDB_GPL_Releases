/*! 
  -----------------------------------------------------------------------------
  module: DBMWeb_DBMWebCore.cpp
  -----------------------------------------------------------------------------
  responsible:  
  special area: Database Manager WebAgent Extension
  description:  "core" functions of "the" DBMWeb Class
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
#include <stdio.h>

#include "hsp100.h"

#include "DBM/Web/DBMWeb_DBMWeb.hpp"
#include "DBM/Web/DBMWeb_Web.hpp"
#include "DBM/Web/DBMWeb_TemplateMsgBox.hpp"
#include "DBM/Web/DBMWeb_TemplateFrame.hpp"
#include "DBM/Web/DBMWeb_TemplateHeader.hpp"
#include "DBM/Web/DBMWeb_TemplateLogon.hpp"
#include "DBM/Web/DBMWeb_TemplateLogoff.hpp"
#include "DBM/Web/DBMWeb_TemplateDatabases.hpp"
#include "DBM/Web/DBMWeb_TemplateMenu.hpp"
#include "DBM/Web/DBMWeb_TemplateWizMenu.hpp"
#include "DBM/Web/DBMWeb_TemplateSize.hpp"
#include "DBM/Web/DBMWeb_TemplateVersion.hpp"

/*
  -----------------------------------------------------------------------------
  private macros
  -----------------------------------------------------------------------------
*/
// Stuff
#define WEB_PARAM_NODE    "DBNode_"

// Fields
#define PARAM_MODE              "Mode"
#define PARAM_MODE_VAL_MNG      "MANAGER"
#define PARAM_MODE_VAL_WIZ      "WIZARD"

#define PARAM_LOGOFF            "LogOff"
#define PARAM_LOGOFF_VAL_YES    "Y"

#define PARAM_ACTION            "Action"
#define PARAM_ACTION_VAL_VIEW   "VIEW"

#define PARAM_SERVER            "Server"  
#define PARAM_DATABASE          "Database"
#define PARAM_USER              "User"    
#define PARAM_PASSWORD          "Password"    

#define PARAM_URL               "URL"    

#define PARAM_STEP              "Step"
#define PARAM_STATE             "State"
#define PARAM_DBSTATE           "DBState"

#define PARAM_PAGES             "Pages"

/*    
  =============================================================================
  class:        DBMWeb_DBMWeb
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: sendFrame
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: sendFrame ( sapdbwa_WebAgent    & wa,
                                        sapdbwa_HttpRequest & request,
                                        sapdbwa_HttpReply   & reply )
{
  DBMCli_String sAction;
  DBMCli_String sServer;
  DBMCli_String sDatabase;
  DBMCli_String sUser;
  DBMCli_String sPwd;
  DBMCli_String sName;

  GetParameterValue( PARAM_SERVER,   request, sServer );
  GetParameterValue( PARAM_DATABASE, request, sDatabase );
  GetParameterValue( PARAM_USER,     request, sUser );
  GetParameterValue( PARAM_PASSWORD, request, sPwd);

  if (m_Wizard != NULL) {
    // view wizard
    DBMWeb_TemplateFrame oFrame(wa, DBMWEB_TEMPLFRAME_WIZARD, m_sSessionID);
    oFrame.writePage(Tools_TemplateWriterWA(reply));

  } else if (m_Database != NULL) {

    // there is a database session
    if( !sDatabase.IsEmpty() || !sUser.IsEmpty() || !sServer.IsEmpty() ) {

      // check values
      if (m_Database->ServerName()   == sServer   &&
          m_Database->DatabaseName() == sDatabase &&
          m_Database->UserName()     == sUser         ) {
        // match -> refresh database
        DBMWeb_TemplateFrame oFrame(wa, DBMWEB_TEMPLFRAME_WORK, m_sSessionID);
        oFrame.setWorkURL(m_sLastWorkURL)
              .writePage(Tools_TemplateWriterWA(reply));
      } else {
        // no match -> delete database
        delete m_Database;
        m_Database = NULL;

      if( sDatabase.IsEmpty() || sUser.IsEmpty() || sPwd.IsEmpty() ) {
         // view logon box
         sAction = "VIEW";
         DBMWeb_TemplateFrame oFrame(wa, DBMWEB_TEMPLFRAME_LOGON, m_sNullSessionID, sAction, sServer, sDatabase, sUser, sPwd);
         oFrame.writePage( Tools_TemplateWriterWA( reply ) );
       } else {
         // call logon
         dbmLogon(wa, request, reply);
       } // end if
      
      } // end if

    } else {

      // refresh database
      sName = m_Database->DatabaseOnServer();
      DBMWeb_TemplateFrame oFrame(wa, DBMWEB_TEMPLFRAME_WORK, m_sSessionID, sName );
      oFrame.setWorkURL(m_sLastWorkURL)
            .writePage(Tools_TemplateWriterWA(reply));

    } // end if

  } else {

    if( sDatabase.IsEmpty() || sUser.IsEmpty() || sPwd.IsEmpty() ) {
      // view logon box
      sAction = "VIEW";
      DBMWeb_TemplateFrame oFrame(wa, DBMWEB_TEMPLFRAME_LOGON, m_sNullSessionID, sAction, sServer, sDatabase, sUser, sPwd);
      oFrame.writePage( Tools_TemplateWriterWA( reply ) );
    } else {
      // call logon
      dbmLogon(wa, request, reply);
    } // end if

  } // end if

  return SAPDB_TRUE;
} // end DBMWeb_DBMWeb :: sendFrame

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: pageHeader
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: pageHeader ( sapdbwa_WebAgent    & wa,
                                         sapdbwa_HttpRequest & request,
                                         sapdbwa_HttpReply   & reply )
{
  DBMCli_String sMode;
  DBMCli_String sLogOff;
  SAPDB_Bool    bLogOff = SAPDB_FALSE;
  DBMCli_String sName;

  GetParameterValue( PARAM_MODE, request, sMode);

  DBMWeb_TemplHeader_Mode nMode = DBMWEB_TEMPLHEADER_MODE_MANAGER;
  if( sMode == PARAM_MODE_VAL_WIZ )
    nMode = DBMWEB_TEMPLHEADER_MODE_WIZARD;

  if( GetParameterValue( PARAM_LOGOFF, request, sLogOff ) ) {
    bLogOff = ( sLogOff == PARAM_LOGOFF_VAL_YES );
  }

  GetParameterValue(PARAM_NAME, request, sName);

  DBMWeb_TemplateHeader oHeader(wa, nMode, bLogOff, sName);

  oHeader.writePage(Tools_TemplateWriterWA(reply));

  return SAPDB_TRUE;
} // end DBMWeb_DBMWeb :: pageHeader

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: pageMenu
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: pageMenu ( sapdbwa_WebAgent    & wa,
                                       sapdbwa_HttpRequest & request,
                                       sapdbwa_HttpReply   & reply )
{
  DBMCli_String sDBState;
  SAPDB_Int nDBState = 0;

  GetParameterValue(PARAM_DBSTATE, request, sDBState);

  nDBState = atoi( sDBState );

  DBMWeb_TemplateMenu oTemplate( wa, nDBState );

  oTemplate.writePage(Tools_TemplateWriterWA(reply));

  return SAPDB_TRUE;
} // end DBMWeb_DBMWeb :: pageMenu

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: pageWizMenu
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: pageWizMenu ( sapdbwa_WebAgent    & wa,
                                          sapdbwa_HttpRequest & request,
                                          sapdbwa_HttpReply   & reply )
{
  DBMCli_String sStep;
  SAPDB_Int nStep = 1;

  DBMCli_String sState;
  SAPDB_Int nState = 1;

  GetParameterValue(PARAM_STEP, request, sStep);
  GetParameterValue(PARAM_STATE, request, sState);

  nStep = atoi( sStep );
  nState = atoi( sState );

  DBMWeb_TemplateWizMenu oTemplate( wa, nStep, nState );

  oTemplate.writePage(Tools_TemplateWriterWA(reply));

  return SAPDB_TRUE;
} // end DBMWeb_DBMWeb :: pageWizMenu

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: pageSize
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: pageSize ( sapdbwa_WebAgent    & wa,
                                       sapdbwa_HttpRequest & request,
                                       sapdbwa_HttpReply   & reply )
{
  DBMCli_String sPages;
  SAPDB_Int4    nPages;

  GetParameterValue(PARAM_PAGES, request, sPages);

  nPages = atoi( sPages );

  DBMWeb_TemplateSize oTemplate( wa, 8192, nPages );

  oTemplate.writePage(Tools_TemplateWriterWA(reply));

  return SAPDB_TRUE;
} // end DBMWeb_DBMWeb :: pageSize

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: pageVersion
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: pageVersion ( sapdbwa_WebAgent    & wa,
                                          sapdbwa_HttpRequest & request,
                                          sapdbwa_HttpReply   & reply )
{
  tsp00_Versionc       webdbmVersionString;
  DBMCli_String        sWebdbmVersionString;
  DBMCli_String        sDbmsrvVersionString;
  DBMCli_String        sKernelVersionString;
  Msg_List oMsgList;

  // collect version information

  // WebDBM-Version
  sp100_GetVersionString  ( "WebDBM", s100buildnumber, webdbmVersionString );
  sWebdbmVersionString = webdbmVersionString;

  // DBMServer-Version
  if (m_Database != NULL) {
    if (m_Database->NodeInfo().Refresh(oMsgList)) {
      sDbmsrvVersionString = m_Database->NodeInfo().Build();

      // Kernel Version
      if (m_Database->GetState().Value() == DBMCLI_DBSTATE_WARM) {
        if (m_Database->GetInfo().Open(DBMCLI_INFO_VERSIONS, oMsgList)) {
          DBMCli_ResultBuf oRow;
          if (m_Database->GetInfo().GetRow(oRow, oMsgList)) {
            if( oRow.GetField( sKernelVersionString, "=|\n" ) ) {
              if( oRow.GetField( sKernelVersionString, "=|\n" ) ) {
                sKernelVersionString.Trim( );
              } // end if
            } // end if
          } // end if
        } // end if
      } // end if

    } // end if
   } // end if

  DBMWeb_TemplateVersion oTemplate( wa, sWebdbmVersionString, sDbmsrvVersionString, sKernelVersionString);

  oTemplate.writePage(Tools_TemplateWriterWA(reply));

  return SAPDB_TRUE;
} // end DBMWeb_DBMWeb :: pageVersion

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: dbmLogon
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: dbmLogon ( sapdbwa_WebAgent    & wa,
                                       sapdbwa_HttpRequest & request,
                                       sapdbwa_HttpReply   & reply )
{
  DBMCli_String sAction;
  DBMCli_String sServer;
  DBMCli_String sDatabase;
  DBMCli_String sUser;
  DBMCli_String sPwd;
  DBMCli_String sUserPwd;

  GetParameterValue(PARAM_ACTION,   request, sAction);
  GetParameterValue(PARAM_SERVER,   request, sServer);
  GetParameterValue(PARAM_DATABASE, request, sDatabase);
  GetParameterValue(PARAM_USER,     request, sUser);
  GetParameterValue(PARAM_PASSWORD, request, sPwd);

  if (sAction == PARAM_ACTION_VAL_VIEW) {
    // send logon screen
    DBMWeb_TemplateLogon oLogon(wa, sServer, sDatabase, sUser);
    oLogon.writePage(Tools_TemplateWriterWA(reply));

  } else {

    // check data
    if (sDatabase.IsEmpty() || sUser.IsEmpty() || sPwd.IsEmpty()) {

      DBMWeb_TemplateMsgBox oMsgBox(wa, DBMWEB_TEMPLMSGBOX_ERROR, "", "Missing database, user or password!");
      oMsgBox.writePage(Tools_TemplateWriterWA(reply));

    } else {

      // delete old database
      if (m_Database != NULL) {
        delete m_Database;
        m_Database = NULL;
      } // end if

      // create user string
      sUserPwd = sUser;
      sUserPwd += ",";
      sUserPwd += sPwd;
    
      Msg_List oMsgList;
      m_Database = new DBMCli_Database(sServer, sDatabase, sUserPwd, oMsgList);
  
      if (!oMsgList.IsEmpty()) {
        // send logon screen with error message
        DBMWeb_TemplateLogon oLogon(wa, sServer, sDatabase, sUser, oMsgList);
        oLogon.writePage(Tools_TemplateWriterWA(reply));

        oMsgList.ClearMessageList();

        delete m_Database;
        m_Database = NULL;
      } else if (m_Database == NULL) {
        DBMWeb_TemplateMsgBox oMsgBox(wa, DBMWEB_TEMPLMSGBOX_ERROR, "", "Can not create database object!");
        oMsgBox.writePage(Tools_TemplateWriterWA(reply));
      } else {
		// refresh node info (dbm_version)
	    if (m_Database->NodeInfo().Refresh(oMsgList)) {
			// forward to work frame
			m_sLastWorkURL = "";
			DBMCli_String newURL;
			newURL = "/webdbm/" + m_sSessionID;
			MovetoURL(newURL, reply);
		} else {
			DBMWeb_TemplateMsgBox oMsgBox(wa, DBMWEB_TEMPLMSGBOX_ERROR, oMsgList, "Error");
			oMsgBox.writePage(Tools_TemplateWriterWA(reply));

			oMsgList.ClearMessageList();

			delete m_Database;
			m_Database = NULL;
		}
      } // end if
    } // end if
  } // end if

  return SAPDB_TRUE;
} // end DBMWeb_DBMWeb :: dbmLogon

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: dbmLogoff
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: dbmLogoff ( sapdbwa_WebAgent    & wa,
                                        sapdbwa_HttpRequest & request,
                                        sapdbwa_HttpReply   & reply )
{
  if (m_Database != NULL) {
    delete m_Database;
    m_Database = NULL;
  } // end if

  DBMWeb_TemplateLogoff oLogoff(wa);
  oLogoff.writePage(Tools_TemplateWriterWA(reply));

  return SAPDB_TRUE;
} // end DBMWeb_DBMWeb :: dbmLogoff

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: enumDatabases
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: enumDatabases ( sapdbwa_WebAgent    & wa,
                                            sapdbwa_HttpRequest & request,
                                            sapdbwa_HttpReply   & reply )
{
  DBMCli_String sServer;

  GetParameterValue(PARAM_SERVER, request, sServer);

  Msg_List oMsgList;

  DBMCli_Node oNode(sServer, oMsgList);

  if (oMsgList.IsEmpty() && oNode.EnumerateDatabases(oMsgList)) {
    if (oNode.EnumDatabaseArray().GetSize() > 0) {

      DBMWeb_TemplateDatabases oTemplate(wa, oNode);

      oTemplate.writePage( Tools_TemplateWriterWA( reply ) );

    } else {

      DBMCli_String sMessage;

      sMessage = "No databases found on server " + sServer + ".";
      
      DBMWeb_TemplateMsgBox oMsgBox(wa, DBMWEB_TEMPLMSGBOX_INFO, "", sMessage );

      oMsgBox.writePage(Tools_TemplateWriterWA(reply));

    } // end if


  } else {

    DBMWeb_TemplateMsgBox oMsgBox(wa, DBMWEB_TEMPLMSGBOX_INFO, oMsgList, "");

    oMsgBox.writePage(Tools_TemplateWriterWA(reply));

  } // end if
  
  return SAPDB_TRUE;
} // end DBMWeb_DBMWeb :: searchDatabase

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: sendNotImpl
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: sendNotImpl ( sapdbwa_WebAgent    & wa,
                                          sapdbwa_HttpRequest & request,
                                          sapdbwa_HttpReply   & reply )
{

  DBMWeb_TemplateMsgBox oMsgBox(wa, DBMWEB_TEMPLMSGBOX_INFO, "", "Sorry, not implemented!");

  oMsgBox.writePage(Tools_TemplateWriterWA(reply));
 
  return SAPDB_TRUE;

} // end DBMWeb_DBMWeb :: sendNotImpl

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: dbmWizard
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: dbmWizard ( sapdbwa_WebAgent    & wa,
                                        sapdbwa_HttpRequest & request,
                                        sapdbwa_HttpReply   & reply )
{
  DBMCli_String sServer;
  DBMCli_String sDatabase;

  GetParameterValue(PARAM_SERVER, request, sServer);
  GetParameterValue(PARAM_DATABASE, request, sDatabase);

  if (m_Wizard != NULL) {
    delete m_Wizard;
  } // end if

  m_Wizard = new DBMCli_Wizard( );

  if (m_Wizard == NULL) {

    DBMWeb_TemplateMsgBox oMsgBox(wa, DBMWEB_TEMPLMSGBOX_ERROR, "", "Can not create wizard object!");
    oMsgBox.writePage(Tools_TemplateWriterWA(reply));

  } else {

    m_Wizard->SetDatabaseName( sDatabase );
    m_Wizard->SetServerName( sServer );

    // forward to wizard frame
    m_sLastWorkURL = "";
    DBMCli_String newURL;
    newURL = "/webdbm/" + m_sSessionID;
    MovetoURL(newURL, reply);

//    DBMWeb_TemplateFrame oFrame(wa, DBMWEB_TEMPLFRAME_WIZARD, m_sSessionID);
//    oFrame.writePage(Tools_TemplateWriterWA(reply));

  } // end if

  return SAPDB_TRUE;
} // end DBMWeb_DBMWeb :: dbmWizard

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: sendError
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: sendMsgListError ( sapdbwa_WebAgent           & wa,
                                               sapdbwa_HttpRequest        & request,
                                               sapdbwa_HttpReply          & reply,
                                               const Msg_List & oMsgList,
                                               const DBMCli_String        & sDatabase,
                                               const char                 * sAction) 
{
  DBMCli_String sTitle;

  sTitle = "Error - " + sDatabase;

  DBMWeb_TemplateMsgBox oMsgBox(wa, DBMWEB_TEMPLMSGBOX_ERROR, oMsgList, sTitle);
  if (sAction != NULL) {
    oMsgBox.SetButtonAction(sAction);
  } // end if
  
  oMsgBox.writePage(Tools_TemplateWriterWA(reply));
 
  return SAPDB_TRUE;
} // end DBMWeb_DBMWeb :: sendError
