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

/*!**********************************************************************

  module: StudioWeb_SQLWindow.cpp

  -----------------------------------------------------------------------

  responsible:  WolfgangA

  special area: WebQuery

  description:  class for a sql window
  
************************************************************************/



#include "SAPDB/SQLStudio/WebQuery/StudioWeb_SQLWindow.hpp"
//some functions
#include "SAPDB/SQLStudio/WebQuery/StudioWeb_Functions.hpp"
//html code fragments
#include "SAPDB/SQLStudio/WebQuery/StudioWeb_HtmlFragments.h"

#include "SAPDB/SQLStudio/WebQuery/StudioWeb_Error.h"



StudioWeb_SQLWindow::StudioWeb_SQLWindow(Studio_DBFSObject  *poDBFS,
										 SAPDB_Bool		   bStoreItemsEnabled)
:m_poDBFS(poDBFS),
m_bStoreItemsEnabled(bStoreItemsEnabled)

{
	//init pointer for statements
	for(SAPDB_Int2 nRun = 0; nRun <= STATEMAXNUMBER - 1;nRun++ ){

		m_pStatements[nRun] = NULL;
	}

	m_lStatementIndex = 0;
	m_lStatementRun = 0;
	m_lStatementMax = 0;

	m_poSFC = new Studio_StoredFileContent;

	m_poSFC->setObjectType(Studio_StoredFileContent::storedObjectTypeStatement);
	m_poSFC->setAutoCommit(SQL_AUTOCOMMIT_ON);
	m_poSFC->setSQLMode(cpr_kind_internal);
	m_poSFC->setIsoLevel(SQL_TXN_READ_COMMITTED);

	m_bCloseExecuteConnection = SAPDB_FALSE;
	
}

StudioWeb_SQLWindow::~StudioWeb_SQLWindow()
{

	for(SAPDB_Int2 nRun = 0; nRun <= STATEMAXNUMBER - 1;nRun++ ){

		if (m_pStatements[nRun] != NULL ){
			
			delete m_pStatements[nRun];
			m_pStatements[nRun] = NULL;
		}		
	}

	delete m_poSFC;
	m_poSFC = NULL;
}


/*===========================================================================*
 *     StudioWeb_SQLWindow::doService										 *
 *===========================================================================*/
SAPDB_Bool StudioWeb_SQLWindow::doService (sapdbwa_WebAgent     &wa,
	  									   sapdbwa_HttpRequest  &req,
										   sapdbwa_HttpReply    &rep,
										   StudioWeb_Result	    *pResult)
{

	if(pResult == NULL) return SAPDB_FALSE;
		 
	//check if request is for dsql
	if (isDSqlRequest(req) == SAPDB_FALSE &&
		isStoredSQLStudioQueryRequest(req) == SAPDB_FALSE){

		sendErrorMessageBox(wa,rep,(const char *)WQ_ERR033);
		return SAPDB_FALSE;
	}	
	
	//new sql window template
	StudioWeb_TemplateDsql *pDSQLTemplate = new StudioWeb_TemplateDsql(wa);		

	pDSQLTemplate->setStoreItemVisible(m_bStoreItemsEnabled);

	if (isStoredSQLStudioQueryRequest(req)){
		
		Tools_DynamicUTF8String sFileName;

		if(getStoredStudioQuery(wa,req,rep,sFileName) == SAPDB_FALSE)
			return SAPDB_FALSE;		

		//if a new stored statement is in auto commit off mode close old
		//force result window to close old execute connection
		if(m_poSFC->getAutoCommit() == SQL_AUTOCOMMIT_OFF)
			m_bCloseExecuteConnection = SAPDB_TRUE;
		else
			m_bCloseExecuteConnection = SAPDB_FALSE;

		m_sLastObjectPath = sFileName;		
	}

	//there is a statement to execute	
	if(isDSQLExecuteRequest(req)){

		//look if there is a request for a new result				
		if (pResult != NULL){

			if(initSOFromRequest (m_poSFC,req) == SAPDB_TRUE){

				pResult->doService(wa,req,rep,m_poSFC,m_bCloseExecuteConnection);
				addStatementToList(m_poSFC->getSQLData());
			}			
			return SAPDB_TRUE;
		}
	}		
	if(isNextStatementRequest(req)){

		m_lStatementRun++;
		if (m_lStatementRun > m_lStatementMax-1) 

			m_poSFC->setSQLData(*m_pStatements[m_lStatementMax-1]);	
		else{
			if (m_lStatementRun < 0)
				m_poSFC->setSQLData(*m_pStatements[0]);
			else
				m_poSFC->setSQLData(*m_pStatements[m_lStatementRun]);
		}
	}
	if(isPrevStatementRequest(req)){

		m_lStatementRun--;
		if (m_lStatementRun > m_lStatementMax-1) 

			m_poSFC->setSQLData(*m_pStatements[m_lStatementMax-1]);	
		else{
			if (m_lStatementRun < 0)
				m_poSFC->setSQLData(*m_pStatements[0]);
			else
				m_poSFC->setSQLData(*m_pStatements[m_lStatementRun]);
		}
	}

	if(isClearStatementRequest(req)) {

		//if a new stored statement is in auto commit off mode close old
		//force result window to close old execute connection
		if(m_poSFC->getAutoCommit() == SQL_AUTOCOMMIT_OFF)
			m_bCloseExecuteConnection = SAPDB_TRUE;
		else
			m_bCloseExecuteConnection = SAPDB_FALSE;

		m_sLastObjectPath.Erase();
		m_poSFC->setSQLData("");
		m_poSFC->setObjectType(Studio_StoredFileContent::storedObjectTypeStatement);
		m_poSFC->setAutoCommit(SQL_AUTOCOMMIT_ON);
		m_poSFC->setSQLMode(cpr_kind_internal);
		m_poSFC->setIsoLevel(SQL_TXN_READ_COMMITTED);
	}

	if(isSelectResultRequest(req)) {

		pResult->doService(wa,req,rep,NULL,m_bCloseExecuteConnection);
		return SAPDB_TRUE;
	}					

	pDSQLTemplate->setStoredObjectPath(m_sLastObjectPath);
	pDSQLTemplate->setSQLStatement(m_poSFC->getSQLData());	

	pDSQLTemplate->setStoredObjectAutoCommit(m_poSFC->getAutoCommit());
	pDSQLTemplate->setSQLMode(m_poSFC->getSQLMode());
	pDSQLTemplate->setIsoLevel(m_poSFC->getIsoLevel());
	pDSQLTemplate->setObjectType(m_poSFC->getObjectType());

	//only stored statements are enable to be saved
	if(m_poSFC->getObjectType() != Studio_StoredFileContent::storedObjectTypeStatement) 
			pDSQLTemplate->enableSave(SAPDB_FALSE);		
	
	
	if (m_lStatementMax > 0){			

		if (m_lStatementRun < m_lStatementMax-1 || (m_lStatementRun == 0 && m_lStatementMax == 1))
			pDSQLTemplate->setNextDisabled(SAPDB_FALSE);			

								   //if there is just one statement in the list enable prev button
		if (m_lStatementRun > 0 || (m_lStatementRun == 1 && m_lStatementMax == 1))  
			pDSQLTemplate->setPrevDisabled(SAPDB_FALSE);

	}

	if (m_lStatementRun < 0) m_lStatementRun = 0;
	if ((m_lStatementRun > m_lStatementMax-1) && (m_lStatementRun != 1 || m_lStatementMax != 1)) m_lStatementRun = m_lStatementMax-1;

	sendPageBegin(rep,WQ_MAINWINDOW_TAG);
	pDSQLTemplate->writePage(Tools_TemplateWriterWA(rep),SAPDB_FALSE);
	rep.SendBody(WQ_HTML_END);
	
	return SAPDB_TRUE;
}


void  StudioWeb_SQLWindow::addStatementToList(const Tools_DynamicUTF8String& sStatement)
{
	if (sStatement.Size() < 1) return;

	//check if statement is alredy in list
	for(SAPDB_Int2 nRun = 0; nRun < m_lStatementMax;nRun++ ){

		if (sStatement == *m_pStatements[nRun]) return;
	}

	if (m_lStatementIndex >= STATEMAXNUMBER){

		m_lStatementIndex = STATEMAXNUMBER - 1;
		if (m_pStatements[m_lStatementIndex] != NULL){

			//if place is already occupied delete first statement old statement
			delete m_pStatements[0];
			m_pStatements[0] = NULL;

			//resort all statements
			for(SAPDB_Int2 nRun = 0; nRun < m_lStatementMax-1;nRun++ )
				m_pStatements[nRun] = m_pStatements[nRun+1];		
		}
	}

	//make a copy of the statement
	m_pStatements[m_lStatementIndex] = new Tools_DynamicUTF8String(sStatement);

	//now increase statement
	++m_lStatementIndex;	

	m_lStatementRun = m_lStatementIndex;
	m_lStatementMax = m_lStatementIndex;
}

SAPDB_Bool StudioWeb_SQLWindow::getStoredStudioQuery(sapdbwa_WebAgent		 &wa,
												     sapdbwa_HttpRequest	 &req,
												     sapdbwa_HttpReply		 &rep,
												     Tools_DynamicUTF8String &sFileName)
{
	
	getParameterValue(WQ_STOREDQUERY_TAG,req,sFileName); 
	if(sFileName.Empty()) return SAPDB_FALSE;
	
	Tools_DynamicUTF8String sBuffer;
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// The class Tools_DynamicUTF8String uses for some functions the type SAPDB_UInt
	// On the supported 32 Bit Platforms the type unsigned int is 4 Bytes.
	// To support bigger stored statements read a file with max 1 MB.
	// Bigger stored statements are not supported
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	SAPDB_Int4 lBytesRead = m_poDBFS->readFile(sFileName,1048576,sBuffer);
	if (lBytesRead < 0){

		sendUtf8ErrorMessageBox(wa,rep,sBuffer);
		return SAPDB_FALSE;
	}
	
	if (m_poSFC->setObjectFromBuffer(sBuffer) == 0){

		sBuffer = (SAPDB_UTF8*)WQ_ERR012;
		sBuffer += sFileName;
		sendUtf8ErrorMessageBox(wa,rep,sBuffer);
		return SAPDB_FALSE;
	}	

		
	return SAPDB_TRUE;
}

