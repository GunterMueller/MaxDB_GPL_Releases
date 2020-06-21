/*
    Copyright (c) 2000-2005 SAP AG

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA



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

/*!**********************************************************************

  module: StudioWeb_WebQuery.cpp

  -----------------------------------------------------------------------

  mail to:  sapdb-general@sap.com

  special area: WebQuery

  description:  main class for WebQuery

  see also:		/

  -----------------------------------------------------------------------

  copyright:                Copyright (c) 2000-2005 SAP AG

************************************************************************/


#include "SAPDB/SQLStudio/WebQuery/StudioWeb_WebQuery.hpp"

//error messages
#include "SAPDB/SQLStudio/WebQuery/StudioWeb_Error.h"

//some functions
#include "SAPDB/SQLStudio/WebQuery/StudioWeb_Functions.hpp"

#include "ToolsCommon/Tools_TemplateMsgBox.hpp"

#include "SAPDB/SQLStudio/WebQuery/StudioWeb_TemplateLogon.hpp"

#include "SAPDB/SQLStudio/WebQuery/StudioWeb_TemplateHeader.hpp"

#include "SAPDB/SQLStudio/WebQuery/StudioWeb_TemplateFrame.hpp"

#include "SAPDB/SQLStudio/WebQuery/StudioWeb_TemplateMainFrame.hpp"


const SAPDB_UInt2 cnstSQLStudioFilePrefix = 7;


StudioWeb_WebQuery::StudioWeb_WebQuery()
{

	m_bUserLoggedOn = SAPDB_FALSE;

	m_pQueryTree = NULL;
	m_pSQLWindow = NULL;
	m_pResult = NULL;

	m_poDBFS = NULL;

	m_poConnection = NULL;

	m_sSessionID = "0";	
    m_sServer    = "";
    m_sDatabase  = "";
    m_sUser      = "";


}

StudioWeb_WebQuery::~StudioWeb_WebQuery()
{	
	if (m_poDBFS != NULL){

		delete m_poDBFS;
		m_poDBFS = NULL;
	}	

	//clear connect opbject
	if (m_poConnection != NULL){

		Tools_DynamicUTF8String	sError;
		m_poConnection->dbLogOff(sError);
		delete m_poConnection;
		m_poConnection = NULL;
	}		

	//clear SQL Window
	if (m_pSQLWindow != NULL){

		delete m_pSQLWindow;
		m_pSQLWindow = NULL;
	}	
	//clear web result object
	if (m_pResult != NULL){

		delete m_pResult;
		m_pResult = NULL;
	}	
}



SAPDB_Bool StudioWeb_WebQuery::doService (sapdbwa_WebAgent     &wa,
	   									  sapdbwa_HttpRequest  &req,
										  sapdbwa_HttpReply    &rep,
										  SAPDB_Bool		   &bCloseSession)
{	
	bCloseSession = SAPDB_FALSE;
	
	//no user is logged on
	if (!m_bUserLoggedOn) 

		checkRequestUserNotConnected(wa,req,rep);		
	else
		checkRequestUserConnected(wa,req,rep,bCloseSession);			

	return SAPDB_TRUE;
}

SAPDB_Bool StudioWeb_WebQuery::checkRequestUserConnected(sapdbwa_WebAgent	  &wa,
		  	   							  		   sapdbwa_HttpRequest &req,
												   sapdbwa_HttpReply   &rep,
												   SAPDB_Bool			&bCloseSession)
{
	SAPDB_Bool bLogOffButton = FALSE;

	if (!isQueryStringValid(req)) {

		//querystring can be NULL for e.g. refresh
		sendLogonFrame(wa,rep);
		return SAPDB_TRUE;
	}
	//user is connected,request was provoked  by sendLogonFrame in line above
	else if (isHeaderPageRequest(req,bLogOffButton)){

		sendHeaderPage(wa,rep,bLogOffButton);
		return SAPDB_TRUE;
	}	
	//request for logon frame window,request was provoked by sendLogonFrame in line above
	else if (isLogonMainRequest(req) || isFrameRequest(req)) {

		//send main frame to refresh all frame windows
		sendMainFrame(wa,rep);
		return SAPDB_TRUE;
	}
	else if(isTreeButtonRequest(req)){

		sendPageBegin(rep,WQ_MAINWINDOW_TAG);
		sendHTMLFile(wa,rep,(SAPDB_UTF8*)WQ_TREEBUTTONS_PATH);		
		rep.SendBody(WQ_HTML_END);

	}
	else if (isTreePageRequest(req) ||
			 isDBFSRequest(req)){				

		if (m_pQueryTree != NULL){
			m_pQueryTree->doService(wa,req,rep);
			return SAPDB_TRUE;
		}
		else{
			sendErrorMessageBox(wa,rep,(const char *)WQ_ERR029);
			return SAPDB_FALSE;
		}
	}		
	else if (isZoomRequest(req) ||
			 isZoomBackRequest(req)){

		m_pResult->doService(wa,req,rep,NULL,SAPDB_FALSE);
		return SAPDB_TRUE;
	}
	else if (isNavigateInResultRequest(req)){

		m_pResult->doService(wa,req,rep,NULL,SAPDB_FALSE);
		return SAPDB_TRUE;
	}
	else if (isDSqlRequest(req)					||
			 isStoredSQLStudioQueryRequest(req)){
		
		if (m_pSQLWindow != NULL){
			return (m_pSQLWindow->doService(wa,req,rep,m_pResult));			
		}
		else{
			sendErrorMessageBox(wa,rep,(const char *)WQ_ERR031);
			return SAPDB_FALSE;
		}
	}		
	else if (isResultPageRequest(req)			||			 
			 isParamQueryRequest(req)){

		if (m_pResult != NULL){

			m_pResult->doService(wa,req,rep,NULL,SAPDB_FALSE);
			return SAPDB_TRUE;
		}
		else{
			sendErrorMessageBox(wa,rep,(const char *)WQ_ERR032);
			return SAPDB_FALSE;
		}
	}			
	else if (isLogOffRequest(req)){

		dbLogOff(wa,rep);
		bCloseSession = true;	
	}	
	else{
		sendPageBegin(rep,WQ_MAINWINDOW_TAG);		
		rep.SendBody(WQ_BODYOPEN2);
		rep.SendBody(WQ_BG_COLOR);
		rep.SendBody(WQ_CLOSETAG);
		sendPageEnd (rep);	
	}	
	
	

	return SAPDB_FALSE;
}


SAPDB_Bool StudioWeb_WebQuery::checkRequestUserNotConnected(sapdbwa_WebAgent &wa,
		  	   												sapdbwa_HttpRequest &req,
															sapdbwa_HttpReply &rep)
{
	//if user is not logged on and there is no querystring
	//avoid reposting of data and send logon screen
	if (!isQueryStringValid(req)) {
			
		sendLogonFrame(wa,rep);
		return SAPDB_TRUE;
	}

	//if there user is not logged on and querystring is logout, send logon window
	if (isLogOffRequest(req)){	
		
		sendLogOffTemplate(wa,rep);
		return SAPDB_TRUE;
	}		

	if (isLogOnRequest(req)) {
	
		m_bUserLoggedOn = dbLogon(wa,req,rep);
#ifdef QUICK
		wa.WriteLogMsg("dbLogon ok !");
#endif
		if (m_bUserLoggedOn == SAPDB_TRUE) {

			if (m_pQueryTree != NULL){

				delete m_pQueryTree;
				m_pQueryTree = NULL;
			}

			//create a dbfs file object
			m_poDBFS = new Studio_DBFSObject(m_poConnection,
											 cnstSQLStudioFilePrefix);

			if (m_poDBFS != NULL){

				m_pQueryTree = new StudioWeb_StoredQueryTree(m_poDBFS);

				if (m_pQueryTree != NULL)
					m_bFrameWithTree = m_pQueryTree->checkForQueryTree();									
				else
					m_bFrameWithTree = false;				
			}
			//get sql window object
			if (m_pSQLWindow != NULL){

				delete m_pSQLWindow;
				m_pSQLWindow = NULL;
			}

			m_pSQLWindow = new StudioWeb_SQLWindow(m_poDBFS,
												   m_bFrameWithTree);				

			//clear web result object
			if (m_pResult != NULL){

				delete m_pResult;
				m_pResult = NULL;
			}	
			m_pResult = new StudioWeb_Result(m_poConnection);
			
			reloadFrame(rep,m_sSessionID);
		}						
	}	
	else
		sendLogonWindows(wa,req,rep);

	return SAPDB_FALSE;
}

void StudioWeb_WebQuery::reloadFrame(sapdbwa_HttpReply rep,Tools_DynamicUTF8String sSessionID)
{

	sendPageHeader(rep);
	rep.SendBody(WQ_HTML_BEGIN);
	rep.SendBody(WQ_HTML_HEADER_BEGIN);			
	rep.SendBody(WQ_JS_BEGIN);
	Tools_DynamicUTF8String sBuffer="top.location.href = \"/websql";

	if(sSessionID.Size() > 0){

		sBuffer +=	"/";
		sBuffer +=	sSessionID;
	}
	sBuffer += "\"";
	rep.SendBody(sBuffer.StrPtr());
	rep.SendBody(WQ_JS_END);
	rep.SendBody(WQ_HTML_HEADER_END);
	rep.SendBody(WQ_HTML_END);
}

void StudioWeb_WebQuery::sendLogonFrame(sapdbwa_WebAgent &wa, sapdbwa_HttpReply rep)
{    
	StudioWeb_TemplateMainFrame oFrame(wa, m_bUserLoggedOn,m_sSessionID);
	oFrame.writePage( Tools_TemplateWriterWA( rep) );
}


void StudioWeb_WebQuery::sendMainFrame(sapdbwa_WebAgent &wa,
									   sapdbwa_HttpReply &rep)
{	
	StudioWeb_TemplateFrame oFrame(wa, m_sSessionID,m_bFrameWithTree);
	oFrame.writePage( Tools_TemplateWriterWA( rep) );
	
}



void StudioWeb_WebQuery::sendLogonWindows(sapdbwa_WebAgent &wa,
										  sapdbwa_HttpRequest &req,
										  sapdbwa_HttpReply &rep)
{    

	SAPDB_Bool bLogOffButton = FALSE;	

	//request for logon menu window
	if (isHeaderPageRequest(req,bLogOffButton)){

		sendHeaderPage(wa,rep,bLogOffButton);
	}	
	//request for logon frame window
	if (isLogonMainRequest(req)) {

		sendPageBegin(rep,WQ_MAINWINDOW_TAG);	

		Tools_DynamicUTF8String sEmpty;
		StudioWeb_TemplateLogon oLogon(wa,sEmpty,sEmpty,sEmpty);
		oLogon.writePage(Tools_TemplateWriterWA(rep),SAPDB_FALSE);	

		sendPageEnd(rep);	
	}	
}




SAPDB_Bool StudioWeb_WebQuery::dbLogOff(sapdbwa_WebAgent &wa,
									    sapdbwa_HttpReply &rep)
{

	sapdbwa_WebSession webSession;

	// look for existing Session
	if (wa.GetWebSession( webSession ) == sapdbwa_False) {

		sendErrorMessageBox(wa,rep,WQ_ERR016);
		return SAPDB_FALSE;
	}

	Tools_DynamicUTF8String	sError;
	m_poConnection->dbLogOff(sError);
	
	//reinit member
	m_bUserLoggedOn = SAPDB_FALSE;
    m_sServer       = "";
    m_sDatabase     = "";
    m_sUser         = "";
	
	//check if webSession is valid
	if (webSession.GetWebSession() != NULL)

		sendLogOffTemplate(wa,rep);
	else
		sendErrorMessageBox(wa,rep,WQ_ERR016);		

	return SAPDB_TRUE;
	
}	

void StudioWeb_WebQuery::sendLogOffTemplate(sapdbwa_WebAgent &wa,
										    sapdbwa_HttpReply &rep)
{
	/*send java script to relocate mainframe*/
	reloadFrame(rep,"");
}

SAPDB_Bool StudioWeb_WebQuery::dbLogon(sapdbwa_WebAgent &wa,
									   sapdbwa_HttpRequest &req,
									   sapdbwa_HttpReply &rep)
{	

	if(m_poConnection != NULL) delete m_poConnection;

#if defined(WIN32)
	if (m_sODBCDriver.Length() < 1) {
		sendErrorMessageBox(wa,rep,(const char *)WQ_ERR002);
		return SAPDB_FALSE;
	}
#endif

	m_poConnection = new Studio_Connection(m_pDbcPool,sapdbwa_DBC_EXCL_AUTOCOMMIT_OFF);
	if(m_poConnection == NULL){

		//throw error
		sendErrorMessageBox(wa,rep,(const char *)WQ_ERR009);
		return SAPDB_FALSE;
	}
	m_poConnection->setODBCDriver(m_sODBCDriver);

	Tools_DynamicUTF8String		sDBServer;
	Tools_DynamicUTF8String		sDBName;
	Tools_DynamicUTF8String		sDBUser;
	Tools_DynamicUTF8String		sPassWord;		

	const char*		method = req.GetMethod();
	//check if method is valid
	if (method == NULL) return SAPDB_FALSE;
	if (strcmp(method,"POST") == 0) {
	
		#ifdef QUICK
		sapdbwa_StringSeq paramValues;
		if (req.GetParameterValues( WQ_DBSERVER_TAG, paramValues )) {

			wa.WriteLogMsg(WQ_DBSERVER_TAG);
			if (paramValues[0] == NULL) 
				wa.WriteLogMsg("null");
			else
				wa.WriteLogMsg(paramValues[0]);
			wa.WriteLogMsg("\n");
		}	
		if (req.GetParameterValues( WQ_DBNAME_TAG, paramValues )) {

			wa.WriteLogMsg(WQ_DBNAME_TAG);
			if (paramValues[0] == NULL) 
				wa.WriteLogMsg("null");
			else
				wa.WriteLogMsg(paramValues[0]);
			wa.WriteLogMsg("\n");
		}	
		if (req.GetParameterValues( WQ_DBUSER_TAG, paramValues )) {

			wa.WriteLogMsg(WQ_DBUSER_TAG);
			if (paramValues[0] == NULL) 
				wa.WriteLogMsg("null");
			else
				wa.WriteLogMsg(paramValues[0]);
			wa.WriteLogMsg("\n");
		}			
		if (req.GetParameterValues(WQ_LOGOGN_TAG, paramValues )) {

			wa.WriteLogMsg(WQ_LOGOGN_TAG);
			if (paramValues[0] == NULL) 
				wa.WriteLogMsg("null");
			else
				wa.WriteLogMsg(paramValues[0]);
			wa.WriteLogMsg("\n");
		}			
		#endif

		getParameterValue(WQ_DBSERVER_TAG,req,sDBServer);
		getParameterValue(WQ_DBNAME_TAG,req,sDBName);
		getParameterValue(WQ_DBUSER_TAG,req,sDBUser);
		getParameterValue(WQ_PASSWORD_TAG,req,sPassWord);
		
		// check data
		if (sDBName.Empty() || sDBUser.Empty() || sPassWord.Empty()) {
			
			sendErrorMessageBox(wa,rep,(const char *)WQ_ERR024);
			return SAPDB_FALSE;
		}

		
		/*	 wa.WriteLogMsg(WQ_DBSERVER_TAG);		 
			wa.WriteLogMsg((const char *)sDBServer.StrPtr());
			wa.WriteLogMsg("\n");

			wa.WriteLogMsg(WQ_DBNAME_TAG);		 
			wa.WriteLogMsg((const char *)sDBName.StrPtr());
			wa.WriteLogMsg("\n");

			wa.WriteLogMsg(WQ_DBUSER_TAG);
			wa.WriteLogMsg((const char *)sDBUser.StrPtr());
			wa.WriteLogMsg("\n");

			wa.WriteLogMsg(WQ_PASSWORD_TAG);
			wa.WriteLogMsg((const char *)sPassWord.StrPtr());
			wa.WriteLogMsg("\n");
		*/
		
		/*
		http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123275
		*/
		Tools_DynamicUTF8String tmp = sDBUser.SubStrElem(0,1);
		if(!tmp.Empty()){
			if(tmp != (SAPDB_Char*)"\""){
				sDBUser = sDBUser.ToUpper();
			}else{
				sDBUser = sDBUser.SubStrElem(1,sDBUser.Size()-2);			
			}
		}	
		m_poConnection->setDBUser(sDBUser);

		tmp = sPassWord.SubStrElem(0,1);
		if(!tmp.Empty()){
			if(tmp != (SAPDB_Char*)"\""){
				sPassWord = sPassWord.ToUpper();
			}else{
				sPassWord = sPassWord.SubStrElem(1,sPassWord.Size()-2);			
			}
		}
		m_poConnection->setPassWord(sPassWord);
		m_poConnection->setDBServer(sDBServer);
		m_poConnection->setDBName(sDBName);		

	}else{
		sendErrorMessageBox(wa,rep,(const char *)WQ_ERR021);
		return SAPDB_FALSE;			
	}
   

	if (m_pDbcPool != NULL) {
		
		Tools_DynamicUTF8String sError;
		if(m_poConnection->dbLogOn(sError) == SAPDB_FALSE){

			sendUtf8ErrorMessageBox(wa,rep,sError);
			return SAPDB_FALSE;
		}
	}	
	else{
		sendErrorMessageBox(wa,rep,(const char *)WQ_ERR013);
		return SAPDB_FALSE;
	}

    m_sServer       = sDBServer;
    m_sDatabase     = sDBName;
    m_sUser         = sDBUser;

	return SAPDB_TRUE;
}


void StudioWeb_WebQuery::sendHeaderPage(sapdbwa_WebAgent &wa,
										sapdbwa_HttpReply &rep,
										SAPDB_Bool bLogOff)
{
	sendPageBegin(rep,WQ_MAINWINDOW_TAG);

	StudioWeb_TemplateHeader oHeader(wa, bLogOff, m_sServer, m_sDatabase, m_sUser);
	oHeader.writePage(Tools_TemplateWriterWA(rep),SAPDB_FALSE);

	sendPageEnd (rep);	
}
