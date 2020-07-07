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

  module: StudioWeb_Result.cpp

  -----------------------------------------------------------------------

  responsible:  WolfgangA

  special area: WebQuery

  description:  class for keeping and showing db result 

************************************************************************/



#include "SAPDB/SQLStudio/WebQuery/StudioWeb_Result.hpp"

//html code fragments
#include "SAPDB/SQLStudio/WebQuery/StudioWeb_HtmlFragments.h"

//error messages
#include "SAPDB/SQLStudio/WebQuery/StudioWeb_Error.h"

#include "SAPDB/SQLStudio/Studio_Statements.hpp"

const SAPDB_Int4 cnstResultSize = 50;

const SAPDB_Int4 cnstSmallLong = 32;
const SAPDB_Int4 cnstBigLong = 8192;




StudioWeb_Result::StudioWeb_Result(Studio_Connection* poConnection)
:m_poConnection (poConnection)
{	

	m_poExecuteConnection = NULL;
	m_poSFC = NULL;
	m_bCloseExecuteConnection = SAPDB_TRUE;
	m_poParamTemplate = NULL;
	m_poZoomTemplate = NULL;

	m_CurrentResult = 0;
}

StudioWeb_Result::~StudioWeb_Result()
{
	//clear result collection 
	clearResultCollection();	
	
	//delete zoom template
	if(m_poZoomTemplate != NULL){

		delete m_poZoomTemplate;
		m_poZoomTemplate = NULL;
	}

	//delete param template
	if(m_poParamTemplate != NULL){

		delete m_poParamTemplate;
		m_poParamTemplate = NULL;
	}
	
	//close last open execute connection
	if(m_poExecuteConnection != NULL){

		//if connection is type autocommit off, do rollback on connection before closing
		if(m_poExecuteConnection->getAutoCommit() == SQL_AUTOCOMMIT_OFF)
		  rollBack(m_poExecuteConnection);

		//logoff from database
		Tools_DynamicUTF8String	sError;
		m_poExecuteConnection->dbLogOff(sError);

		//delete connection
		delete m_poExecuteConnection;
		m_poExecuteConnection = NULL;
	}
}


SAPDB_Bool StudioWeb_Result::doService (sapdbwa_WebAgent     & wa,
	  									sapdbwa_HttpRequest  &req,
										sapdbwa_HttpReply    &rep,
										Studio_StoredFileContent	*poSFC,
										SAPDB_Bool bCloseExecuteConnection)
{	
	sendPageBegin(rep,WQ_MAINWINDOW_TAG);
	rep.SendBody(WQ_REFRESHFRAMESCTIPT);
	
	rep.SendBody(WQ_BODYOPEN_RESULT);
	rep.SendBody(WQ_BG_COLOR);
	//don't activate refresh of dsql window if request is navigation or select result
	if (!isNavigateInResultRequest(req) &&
		!isSelectResultRequest(req)     &&
		!isZoomRequest(req)				&&
		!isZoomBackRequest(req)) {

		rep.SendBody(WQ_REFRESHDSQLWINDOW);		
	}
	rep.SendBody(WQ_CLOSETAG);	

	////////////////////////////////////////////////////////////////////
	//init members		
	//set SO Object just if valid
	if(poSFC) m_poSFC = poSFC;
	m_bCloseExecuteConnection = bCloseExecuteConnection;
	////////////////////////////////////////////////////////////////////

	if (isNavigateInResultRequest(req)){

		//direction to fetch the result
		StudioWeb_FetchDirection fDir = StudioWeb_FetchDirNone;	

		//check again the tag of the request
		if (isNavigateInResultFirstRequest(req))
			fDir = StudioWeb_FetchDirFirst;
		else if (isNavigateInResultPrevRequest(req))
			fDir = StudioWeb_FetchDirPrevious;
		else if (isNavigateInResultNextRequest(req))  
			fDir = StudioWeb_FetchDirNext;					     		
		else if (isNavigateInResultLastRequest(req))
			fDir = StudioWeb_FetchDirLast;			
		else
			fDir = StudioWeb_FetchDirNone;

		if (fDir != StudioWeb_FetchDirNone)	{

			drawResultNo(m_CurrentResult,fDir,wa,rep);			
		}
	}
	else if(isParamQueryRequest(req)){

		Tools_DynamicUTF8String sNewStatement;
		Tools_DynamicUTF8String sOldStatement;
		if(getStatementOfParamPage(req,sOldStatement,sNewStatement)){

			if(m_poSFC != NULL){

				m_poSFC->setSQLData(sNewStatement);
				handleExecuteRequest(wa,rep);	
				//set param query as sqldata for dsql window to show
				m_poSFC->setSQLData(sOldStatement);
			}
		}
	}
	else if(isSelectResultRequest(req)){

		m_CurrentResult = getResultNo(req);		
		drawResultNo(m_CurrentResult,StudioWeb_FetchDirNone,wa,rep);
	}
	else if(isResultPageRequest(req)){

		if(m_poParamTemplate){

			m_poParamTemplate->resetStartValues();
			m_poParamTemplate->writePage(Tools_TemplateWriterWA(rep),SAPDB_FALSE);    		
		}
		else if(m_poZoomTemplate){

			m_poZoomTemplate->resetStartValues();
			m_poZoomTemplate->writePage(Tools_TemplateWriterWA(rep),SAPDB_FALSE);    				}
		else
			drawResultNo(m_CurrentResult,StudioWeb_FetchDirNone,wa,rep);		
	}	
	else if(isZoomRequest(req)){
		doZoom(m_CurrentResult,wa,req,rep);
	}
	else if(isZoomBackRequest(req)){

		//first delete zoom template
		if(m_poZoomTemplate != NULL){

			delete m_poZoomTemplate;
			m_poZoomTemplate = NULL;
		}		
		drawResultNo(m_CurrentResult,StudioWeb_FetchDirNone,wa,rep);		
	}	
	else if(isDSQLExecuteRequest(req)){
		if(m_poSFC != NULL){

		   // reinit current result count
		   m_CurrentResult = 0;
		   handleExecuteRequest(wa,rep);			
		}
	}

	sendPageEnd (rep);			
	return SAPDB_TRUE;
}


void StudioWeb_Result::handleExecuteRequest(sapdbwa_WebAgent     &wa,
											sapdbwa_HttpReply    &rep)

{		
	if(m_poSFC == NULL)  return;
	if(m_poSFC->getSQLData().Empty())  return;

	//if there is a param page delete it
	if(m_poParamTemplate != NULL){

		delete m_poParamTemplate;
		m_poParamTemplate = NULL;
	}

	if(m_poZoomTemplate != NULL){

		delete m_poZoomTemplate;
		m_poZoomTemplate = NULL;
	}	
				
	if(!sendGetParamPage(wa,rep,m_poSFC->getSQLData())){

		if(executeStatements(wa,rep) == SAPDB_FALSE){

			//clear result collection, if execute of one statement failed
			//clearResultCollection();	
			return;		
		}

		StudioWeb_ResultCollection *pResultCollection = (StudioWeb_ResultCollection *)m_ResultList.at(0);
		if (pResultCollection != NULL){

			//get valid template
			if(pResultCollection->pTable == NULL)
				pResultCollection->pTable = new StudioWeb_ResultTemplate(wa);		
			
			sendResultPage(wa,rep,pResultCollection,StudioWeb_FetchDirFirst);
		}
		//if there is no result collection, but executeStatements returned true 
		//a sql statement without result was executed successfully
		else{
			rep.SendBody(WQ_ERR008);	
			return;
		}
	}	
}

SAPDB_Bool  StudioWeb_Result::executeStatements(sapdbwa_WebAgent    &wa,
												sapdbwa_HttpReply   &rep)
{

	if(m_poSFC == NULL) return SAPDB_FALSE;
	if(m_poConnection == NULL) return SAPDB_FALSE;
	
	SAPDB_Bool bCloseExecuteConnection = SAPDB_TRUE;

	Tools_DynamicUTF8String	sError;
	//close last open execute connection
	if(m_poExecuteConnection != NULL){

		//check if new connection required for stored sql statement has different parameters
		//then the current connection
		bCloseExecuteConnection =  m_bCloseExecuteConnection ||
								   (m_poExecuteConnection->getSQLMode() != m_poSFC->getSQLMode()) ||
								   (m_poExecuteConnection->getIsoLevel() != m_poSFC->getIsoLevel()) ||
								   (m_poExecuteConnection->getAutoCommit() != m_poSFC->getAutoCommit());

		if(bCloseExecuteConnection)
		{
			if(m_poExecuteConnection->getAutoCommit() == SQL_AUTOCOMMIT_OFF)
			   rollBack(m_poExecuteConnection);			

			if (m_poExecuteConnection->dbLogOff(sError) == SAPDB_FALSE){

				sendUtf8ErrorMessageBox(wa,rep,sError);
				return SAPDB_FALSE;
			}
			delete m_poExecuteConnection;
			m_poExecuteConnection = NULL;
		}
	}
	if(m_poExecuteConnection == NULL){
		//get new temp connection object
		m_poExecuteConnection = new Studio_Connection(*m_poConnection);
		m_poExecuteConnection->setSQLMode(m_poSFC->getSQLMode());
		m_poExecuteConnection->setIsoLevel(m_poSFC->getIsoLevel());
		m_poExecuteConnection->setAutoCommit(m_poSFC->getAutoCommit());

		if (m_poExecuteConnection->dbLogOn(sError) == SAPDB_FALSE){

			sendUtf8ErrorMessageBox(wa,rep,sError);
			return SAPDB_FALSE;
		}
	}	
	
	//clear all list for the results
	clearResultCollection();
	StudioWeb_ResultCollection *pResultCollection = NULL;	
	
	Studio_Statements *pStatements = new Studio_Statements(m_poSFC->getSQLData());
	Tools_DynamicUTF8String sCurrentStatement = pStatements->getCurrentStatement();

	//check if statement is valid
	if(sCurrentStatement.Size() < 1) return SAPDB_FALSE;
	//if there is nothing to execute return false
	if(pStatements->endOfStatements() == SAPDB_TRUE) return SAPDB_FALSE;

	//dummy result for some executes
	StudioOAL_WResult				  *pDummyResult = NULL;
	Studio_Statements::statementType  emStatmentTyp;

	while (pStatements->endOfStatements() == SAPDB_FALSE)
	{	 		 
		emStatmentTyp = pStatements->getCurrentStatementType();

		//get new result collection
		pResultCollection = getNewResultCollection();
		if (pResultCollection == NULL) return SAPDB_FALSE;
		//add result collection to list
		m_ResultList.append(pResultCollection);	
		pResultCollection->sStatmentHeader = sCurrentStatement;		 
			
		switch (emStatmentTyp)
		{			
			case Studio_Statements::statementShow:
			case Studio_Statements::statementExplain:				
				
				pDummyResult = new StudioOAL_WResult(sapdbwa_DBCHandle(m_poExecuteConnection->getDBCHandle()),
													 sapdbwa_DBCEnv(m_poExecuteConnection->getDBCHandle()));
				/*set the sql mode for the result object PTS:1118901*/
				pDummyResult->setSqlMode(m_poExecuteConnection->getSQLMode());

				if (!(pDummyResult->fullExecStmt(sCurrentStatement))){
				
					//if there is an error delete result object of 
					//pResultCollection and put dummy result as result object					
					delete pResultCollection->pResult;
					pResultCollection->pResult = pDummyResult;					
				}		
				else{                                        
					if (!(pResultCollection->pResult->openResult("SELECT * FROM SHOW"))){
					
						//if there is no show table,
						//delete old result and put new result object
						//to force 
						if(pResultCollection->pResult->getNativeError() == -4004){

							delete pResultCollection->pResult;
							pResultCollection->pResult = new StudioOAL_WResult(sapdbwa_DBCHandle(m_poExecuteConnection->getDBCHandle()),
																			   sapdbwa_DBCEnv(m_poExecuteConnection->getDBCHandle()));							
							/*set the sql mode for the result object PTS:1118901*/
							pResultCollection->pResult->setSqlMode(m_poExecuteConnection->getSQLMode());
						}
						else
							delete pDummyResult;				
					}
				}
				
				break;
			case Studio_Statements::statementSelect:				
			
				pResultCollection->pResult->openResult(sCurrentStatement);				
				break;
			case Studio_Statements::statementCallProcedure:
        
				pResultCollection->pResult->openParamResult(sCurrentStatement);
				break;

			default:				
				//create, alter ...				
				pResultCollection->pResult->fullExecStmt(sCurrentStatement);
				
		}		
		sCurrentStatement = pStatements->nextStatement();
	}

	delete pStatements;
	pStatements = NULL;

	return SAPDB_TRUE;

}

SAPDB_Bool  StudioWeb_Result::sendResultPage(sapdbwa_WebAgent    &wa,
											 sapdbwa_HttpReply   &rep,
											 StudioWeb_ResultCollection *pResultCollection,
											 StudioWeb_FetchDirection emFetchDirNow)
{
	
	if(m_poConnection == NULL) return SAPDB_FALSE;
	if(m_poConnection->getDBCHandle() == NULL) return SAPDB_FALSE;		
	
	if (pResultCollection == NULL) return SAPDB_FALSE;
	if (pResultCollection->pResult == NULL) return SAPDB_FALSE;

	//check for valid pointer
	if(pResultCollection->pTable == NULL) return SAPDB_FALSE;

	//variables
	Tools_DynamicUTF8String		sBuffer;	
	getFormatedStatement(pResultCollection,64,sBuffer);	
	
	//reset the template
	pResultCollection->pTable->resetTemplate();
	pResultCollection->pTable->setTableTitle(sBuffer);	

	//if there are no rows, this could be e.g. a create statement,or a select with no rows =>
	//show message "Statements successfully executed, no result !"
	//but if there was a fetch before and sqlmode is not internal
	//there are no more rows, show "No more rows, cursor is closed !"	
	if(m_poExecuteConnection->getSQLMode() != cpr_kind_internal){
		
		//if there is error text show error text
		if(!pResultCollection->pResult->isError()){

			if(pResultCollection->pResult->noRows()){
				if(emFetchDirNow == StudioWeb_FetchDirNext){
					//No more rows, cursor is closed !
					pResultCollection->pTable->setCell(WQ_ERR003,1,1);
				}
				else{
					//"Statements successfully executed, no result !"					
					pResultCollection->pTable->setCell(WQ_ERR008,1,1);
				}

				fillResultList(pResultCollection);
				pResultCollection->pTable->writePage(Tools_TemplateWriterWA(rep),SAPDB_FALSE);
				return SAPDB_TRUE;
			}
		}
		else{
			pResultCollection->pTable->setCell(pResultCollection->pResult->getErrorText(),1,1);
			fillResultList(pResultCollection);
			pResultCollection->pTable->writePage(Tools_TemplateWriterWA(rep),SAPDB_FALSE);
			return SAPDB_TRUE;
		}		
	}
	else{
		//if there is error text show error text
		if(!pResultCollection->pResult->isError()){

			if(pResultCollection->pResult->noRows() 
				&& emFetchDirNow == StudioWeb_FetchDirFirst
				&& pResultCollection->lastAction == StudioWeb_FetchDirNone){

				pResultCollection->pTable->setCell(WQ_ERR008,1,1);
				fillResultList(pResultCollection);
				pResultCollection->pTable->writePage(Tools_TemplateWriterWA(rep),SAPDB_FALSE);
				return SAPDB_TRUE;
			}
		}
		else{
			pResultCollection->pTable->setCell(pResultCollection->pResult->getErrorText(),1,1);
			fillResultList(pResultCollection);
			pResultCollection->pTable->writePage(Tools_TemplateWriterWA(rep),SAPDB_FALSE);
			return SAPDB_TRUE;
		}	
	}				
	
	for (SAPDB_UInt2 nRun = 1 ; nRun <= pResultCollection->pResult->getColCount();nRun++){

		if (pResultCollection->pResult->getColumnName(nRun,sBuffer))
			
			pResultCollection->pTable->setColumnTitle(sBuffer,nRun);		
		else
			break;			
	} 	 	 

	if(pResultCollection->pResult->isDBProcedureResult())

		buildParamResultData(pResultCollection);	
	else
		buildResultData(rep,pResultCollection,emFetchDirNow);
	
	
	fillResultList(pResultCollection);
	pResultCollection->pTable->writePage(Tools_TemplateWriterWA(rep),SAPDB_FALSE);

	return SAPDB_TRUE;
}

void StudioWeb_Result::buildParamResultData(StudioWeb_ResultCollection *pResultCollection)
{

	Tools_DynamicUTF8String		sBuffer;	
	for (SAPDB_Int2 nRun = 1 ; nRun <= pResultCollection->pResult->getColCount();nRun++){

		if (pResultCollection->pResult->getParamValue(nRun-1,sBuffer))
			
			pResultCollection->pTable->setCell(sBuffer,1,nRun);				
		else
			break;
	}

	// in param result there is no fetch direction
	pResultCollection->lastAction = StudioWeb_FetchDirNone;
}

void StudioWeb_Result::buildResultData(sapdbwa_HttpReply   &rep,
									   StudioWeb_ResultCollection *pResultCollection,
									   StudioWeb_FetchDirection emFetchDirNow)
{

	//variables
	Tools_DynamicUTF8String		sBuffer;	
	SAPDB_UInt2					nRowCount = 0;
	SAPDB_Bool					bThereIsMore = SAPDB_FALSE;	
	SAPDB_ULong					noRowsFetched = 0;
	SAPDB_UInt2					nRowStatus = 0;
	StudioWeb_DisableButton		emDisableButton = StudioWeb_DisableNone;		

	SAPDB_UInt2 lSQLParam = SQL_FETCH_ABSOLUTE;
	SAPDB_UInt2 lSQLParam2 = SQL_FETCH_NEXT;	

	//get the fetch direction from last result page
	StudioWeb_FetchDirection emLastAction = pResultCollection->lastAction;
	//remember the last fetch action
	pResultCollection->lastAction = emFetchDirNow;
	//determine sqlparams for extended fetch
	SAPDB_Int4	nStartRow = 0;
	if (getFetchParams(pResultCollection,
					   emFetchDirNow,
					   emLastAction,
					   lSQLParam,
					   lSQLParam2,
					   nStartRow) == SAPDB_FALSE) return;	
	
	//just fetch for first or last position
	if ( (emFetchDirNow == StudioWeb_FetchDirFirst || emFetchDirNow == StudioWeb_FetchDirLast)	
		||
		 (emFetchDirNow == StudioWeb_FetchDirNext && emLastAction == StudioWeb_FetchDirPrevious)		
		||
		(emFetchDirNow == StudioWeb_FetchDirPrevious && emLastAction == StudioWeb_FetchDirNext)){

		SAPDB_Bool bOk = SAPDB_FALSE;
		//if result is executed in sapdb mode use extended fetch
		if(m_poExecuteConnection->getSQLMode() == cpr_kind_internal) 
			 bOk = pResultCollection->pResult->extendedFetch(lSQLParam,nStartRow,&noRowsFetched,&nRowStatus);
		else
			bOk  = pResultCollection->pResult->fetch();
		
		if(!bOk){

			handleError(pResultCollection->pResult,rep);			
			return;
		}
		else{
			if (!pResultCollection->pResult->noRows()) {
				
				if (emFetchDirNow == StudioWeb_FetchDirLast || emFetchDirNow == StudioWeb_FetchDirPrevious)
					buildOneRow(pResultCollection,cnstResultSize-nRowCount);
				else
					buildOneRow(pResultCollection,nRowCount+1);
				
				++nRowCount;
			}
		}		
	}
	//get the rows
	while (!pResultCollection->pResult->noRows() && nRowCount < cnstResultSize){		

		//if result is executed in sapdb mode use extended fetch
		if(m_poExecuteConnection->getSQLMode() == cpr_kind_internal) {

			if (!pResultCollection->pResult->extendedFetch( lSQLParam2,0,&noRowsFetched,&nRowStatus))	break;
		}
		else{
			if (!pResultCollection->pResult->fetch())	break;		
		}

		if (!pResultCollection->pResult->noRows()) {		
			
			if (emFetchDirNow == StudioWeb_FetchDirLast || emFetchDirNow == StudioWeb_FetchDirPrevious)
				buildOneRow(pResultCollection,cnstResultSize-nRowCount);
			else
				buildOneRow(pResultCollection,nRowCount+1);

			++nRowCount;			
		}
	}

	//remember the fetched rows 
	pResultCollection->lastResultSize = nRowCount;

	if (nRowCount < cnstResultSize){		

		//if we fetch  back, and there are less rows than start row
		//set current row of template to cnstResultSize-nRowCount
		if (emFetchDirNow == StudioWeb_FetchDirLast || emFetchDirNow == StudioWeb_FetchDirPrevious)
			pResultCollection->pTable->setStartRow(cnstResultSize-pResultCollection->lastResultSize);

		pResultCollection->bBorderOfResult = SAPDB_TRUE;		   
	}
	else{
		////////////////////////////////////////////////////////////////////////////////
		// if nRowCount matches the wanted fetch size,
		// => one more fetch to see if there is another row
		// this is only possible in sap db internal mode
		////////////////////////////////////////////////////////////////////////////////		
		if(m_poExecuteConnection->getSQLMode() == cpr_kind_internal) 
		{
			if (nRowCount == cnstResultSize){

				pResultCollection->pResult->extendedFetch(lSQLParam2,0,&noRowsFetched,&nRowStatus);
				
				if (lSQLParam2 == SQL_FETCH_NEXT){

					lSQLParam2 = SQL_FETCH_PRIOR;			
					if (noRowsFetched > 0) bThereIsMore = SAPDB_TRUE;				
				}
				else if (lSQLParam2 == SQL_FETCH_PRIOR){

					lSQLParam2 = SQL_FETCH_NEXT;
					if (noRowsFetched > 0) bThereIsMore = SAPDB_TRUE;
				}
				//fetch back
				pResultCollection->pResult->extendedFetch(lSQLParam2,0,&noRowsFetched,&nRowStatus);
			}
		}	
		else{
			if (nRowCount == cnstResultSize) bThereIsMore = SAPDB_TRUE;
		}

		pResultCollection->bBorderOfResult = !bThereIsMore;
	}	
	// there are more rows or more the the first page is fetched
	// => show navigate buttons
	if (emFetchDirNow != StudioWeb_FetchDirFirst || bThereIsMore){			
		
		if (emFetchDirNow == StudioWeb_FetchDirFirst) emDisableButton = StudioWeb_DisablePrevious;			

		if (emFetchDirNow == StudioWeb_FetchDirLast) emDisableButton = StudioWeb_DisableNext;			

		//if next fetch with no more rows => disable next button
		if (emFetchDirNow == StudioWeb_FetchDirNext && bThereIsMore == SAPDB_FALSE)
			emDisableButton = StudioWeb_DisableNextLast;					

		if (emFetchDirNow == StudioWeb_FetchDirPrevious && (bThereIsMore == SAPDB_FALSE)) 
			emDisableButton = StudioWeb_DisableFirstPrev;			

		if(m_poExecuteConnection->getSQLMode() == cpr_kind_internal) 
			//build a button bar for navigation
			buildInternalModeButtonForm (pResultCollection->pTable,emDisableButton);											
		else{
			//if sql mode is not internal, build just a next button			
			sBuffer.Append(WQ_CREATEWEBSQLURL);
			sBuffer.Append(WQ_NEXTBUTTON_TAG);
			sBuffer.Append("\")");
			
			pResultCollection->pTable->addButton(WQ_NEXTBUTTONNAME, sBuffer,WQ_TARGETSELF,SAPDB_FALSE);
		}
	} 
}

void StudioWeb_Result::buildInternalModeButtonForm(StudioWeb_ResultTemplate* poTable,
												   StudioWeb_DisableButton emDisableButton)
{
	//variables
	Tools_DynamicUTF8String	sBuffer;
	Tools_DynamicUTF8String	sBuffer2;
	SAPDB_Bool bDisable = SAPDB_FALSE;	
		
	sBuffer.Append(WQ_CREATEWEBSQLURL);			

	sBuffer2 = sBuffer;
	sBuffer2.Append(WQ_FIRSTBUTTON_TAG);
	sBuffer2.Append("\")");

	poTable->addButton(WQ_FIRSTBUTTONNAME, sBuffer2,WQ_TARGETSELF,SAPDB_FALSE);

	//prev button
	if (emDisableButton == StudioWeb_DisablePrevious || emDisableButton == StudioWeb_DisableFirstPrev) 
		bDisable = SAPDB_TRUE;	
	else
		bDisable = SAPDB_FALSE;	

	sBuffer2  = sBuffer;
	sBuffer2.Append(WQ_PREVBUTTON_TAG);
	sBuffer2.Append("\")");

	poTable->addButton(WQ_PREVBUTTONNAME, sBuffer2,WQ_TARGETSELF,bDisable);

	//next button
	if (emDisableButton == StudioWeb_DisableNext || emDisableButton == StudioWeb_DisableNextLast) 
		bDisable = SAPDB_TRUE;	
	else
		bDisable = SAPDB_FALSE;	

	sBuffer2  = sBuffer;
	sBuffer2.Append(WQ_NEXTBUTTON_TAG);
	sBuffer2.Append("\")");

	poTable->addButton(WQ_NEXTBUTTONNAME, sBuffer2,WQ_TARGETSELF,bDisable);
	
	sBuffer2 = sBuffer;
	sBuffer2.Append(WQ_LASTBUTTON_TAG);
	sBuffer2.Append("\")");

	poTable->addButton(WQ_LASTBUTTONNAME,sBuffer2,WQ_TARGETSELF,SAPDB_FALSE);

	
}

void StudioWeb_Result::handleError(StudioOAL_WResult		 *pResult,
								   sapdbwa_HttpReply		 &rep)
{	
	Tools_DynamicUTF8String	 sError;
	getError(pResult,sError);
	rep.SendBody(sError.StrPtr());	
	
	return;
}


void StudioWeb_Result::getError(StudioOAL_WResult		 *pResult,
								Tools_DynamicUTF8String	 &sError)
{
	//init output
	sError.Erase();
		
	//check some pointers
	if (m_poConnection->getDBCHandle() == NULL) return;
	if (pResult == NULL) return;
	
	//init bool value
	sapdbwa_Bool connectionDown = SAPDB_TRUE;	
	
	Tools_DynamicUTF8String sBuffer = pResult->getSQLState();	
	//check if connection was down
	if (sapdbwa_CheckDBC(m_poConnection->getDBCHandle(),
						 pResult->getNativeError(),
					     (const char*)sBuffer.StrPtr(),
					     &connectionDown )){

		if (connectionDown == SAPDB_FALSE)

			sError = pResult->getErrorText();					
		else
			sError = WQ_ERR014;		
	}
	else
		sError = WQ_ERR015;						
	
	return;

}

SAPDB_Bool StudioWeb_Result::buildOneRow(StudioWeb_ResultCollection *pResultCollection,SAPDB_Int2 nTemplateRow)
{
	
	SAPDB_Bool						bColNull;
	Tools_DynamicUTF8String			sBuffer;
	Tools_DynamicUTF8String			sBuffer2;
	Tools_DynamicUTF8String			sXMPOpen = "";
	Tools_DynamicUTF8String			sXMPClose= "";
	SQLSMALLINT						nColType = 0;

	for (SAPDB_Int2 nRun = 1 ; nRun <= pResultCollection->pResult->getColCount();nRun++){						
		
		if (pResultCollection->pResult->getColumnDataAsString(nRun,sBuffer)){
			
			pResultCollection->pResult->getODBCColType(nRun,nColType);
			if(nColType == SQL_LONGVARBINARY ||
			   nColType == SQL_LONGVARCHAR ||
			   nColType == SQL_WLONGVARCHAR){

				replaceNoCharWithDot(sBuffer);
				//if sqlmode is internal and long data is not null 
				//build hyperlink for hex zoom

				pResultCollection->pResult->isColNull(nRun,bColNull);
				if(m_poExecuteConnection->getSQLMode() == cpr_kind_internal 
					&&
				   !bColNull){
					
					char sBuf[8];
					sprintf(sBuf,"%d",nTemplateRow);
					/*sBuffer2 = "<a href=\"";
					/sBuffer2.Append("/");
					sBuffer2.Append(WQ_SERVICEDLL);
					sBuffer2.Append("/");*/

					//!!!!!!!!!!!!!!!!!!!!!!!
					sBuffer2 = "<a href=\"";
					sBuffer2.Append("javascript:");
					sBuffer2.Append("Zoom('");

					//!!!!!!!!!!!!!!!!!!!!!!!
					//sBuffer2.Append(WQ_RESULTPAGE_TAG);
					//sBuffer2.Append("?");
					sBuffer2.Append(WQ_ZOOM_ROW_TAG);
					sBuffer2.Append("=");
					sBuffer2.Append(sBuf);
					sBuffer2.Append("&");	
					sBuffer2.Append(WQ_ZOOM_COL_TAG);
					sBuffer2.Append("=");
					sprintf(sBuf,"%d",nRun);
					sBuffer2.Append(sBuf);

					//!!!!!!!!!!!!!!!!!!!!!!!
					sBuffer2.Append("')\">");
					//sBuffer2.Append("\">");	
					//!!!!!!!!!!!!!!!!!!!!!!!

					sBuffer2.Append(sXMPOpen);
					replaceNoCharWithDot(sBuffer);
					sBuffer2.Append(sBuffer);
					sBuffer2.Append(sXMPClose);
					sBuffer2.Append("</a>");

					pResultCollection->pTable->setCell(sBuffer2,nTemplateRow,nRun);
				}
				else{
					if(sBuffer.Empty())
						sBuffer2 = WQ_NPSP;
					else{
					   sBuffer2 = sXMPOpen;
					   sBuffer2.Append(sBuffer);
					   sBuffer2.Append(sXMPClose);
					}
					pResultCollection->pTable->setCell(sBuffer2,nTemplateRow,nRun);
				}
		   }
		   else{
				if(sBuffer.Empty()) 
					sBuffer2 = WQ_NPSP;
				else{
				   sBuffer2 = sXMPOpen;
				   sBuffer2.Append(sBuffer);
				   sBuffer2.Append(sXMPClose);
				}
			    pResultCollection->pTable->setCell(sBuffer2,nTemplateRow,nRun);		   		   
		   }
		}
		else{
			getError(pResultCollection->pResult,sBuffer);			
			//if an error ocurred set error into cell, but continue fetching columns
			pResultCollection->pTable->setCell(sBuffer,nTemplateRow,nRun);		   		   			
		}
			
	}
	return SAPDB_TRUE;
}


SAPDB_Bool StudioWeb_Result::sendGetParamPage(sapdbwa_WebAgent    &wa,
											  sapdbwa_HttpReply   &rep,
											  const Tools_DynamicUTF8String &sStatement)
{
 
	Tools_DynamicUTF8String::BasisElementIndex     nStartPos;
    Tools_DynamicUTF8String::BasisElementIndex     nEndPos;

	//first check for [ , ]
	nStartPos = sStatement.Find((SAPDB_UTF8*)"[");
	if (nStartPos == Tools_DynamicUTF8String::NPos) return SAPDB_FALSE;	
	nStartPos = sStatement.Find((SAPDB_UTF8*)"]");
	if (nStartPos == Tools_DynamicUTF8String::NPos) return SAPDB_FALSE;	

	//now other variables
	SAPDB_UInt				nStatementLen = sStatement.Size();
    SAPDB_UInt				nParamCount = 0;
	SAPDB_Bool				bQuoted = SAPDB_FALSE;
	Tools_DynamicUTF8String::BasisElementIndex     nStartPosNew = 0;
	Tools_DynamicUTF8String::BasisElementIndex     nPosQuote1 = 0;
	Tools_DynamicUTF8String::BasisElementIndex     nPosQuote2 = 0;    

	if(m_poParamTemplate == NULL)
		m_poParamTemplate = new StudioWeb_TemplateParamQuery(wa);    	        

	//use sSubStr as buffer
	Tools_DynamicUTF8String sSubStr = sStatement;	
	quoteString(sSubStr);
	m_poParamTemplate->setSQLStatement(sSubStr);
	
	while (nStartPosNew <= nStatementLen && nStartPos != Tools_DynamicUTF8String::NPos){		
    
		//reinit
		sSubStr.Erase();
		//find the open tag
		nStartPos = sStatement.Find((SAPDB_UTF8*)"[",nStartPosNew);

		nPosQuote1  = sStatement.Find((SAPDB_UTF8*)"\"",nStartPosNew);        
        if (nPosQuote1 == Tools_DynamicUTF8String::NPos)
			nPosQuote1  = sStatement.Find((SAPDB_UTF8*)"'",nStartPosNew);        

		nEndPos = sStatement.Find((SAPDB_UTF8*)"]",nStartPosNew);

        nPosQuote2  = sStatement.Find((SAPDB_UTF8*)"\"",nEndPos);        
        if (nPosQuote2 == Tools_DynamicUTF8String::NPos)
			nPosQuote2  = sStatement.Find((SAPDB_UTF8*)"'",nEndPos);        
		
        if (nStartPos != Tools_DynamicUTF8String::NPos){        
            
			if (nPosQuote1 != Tools_DynamicUTF8String::NPos &&
				nPosQuote2 != Tools_DynamicUTF8String::NPos &&
				nStartPos > nPosQuote1						&&
				nEndPos < nPosQuote2){
            
                bQuoted = SAPDB_TRUE;
                nStartPos = nPosQuote1;
            }

			if (nEndPos > nStartPos && nEndPos != Tools_DynamicUTF8String::NPos){            

				if (nPosQuote1 != Tools_DynamicUTF8String::NPos && bQuoted)
                    bQuoted = SAPDB_TRUE;
                else
                    bQuoted = SAPDB_FALSE;
					
				if (bQuoted == SAPDB_FALSE){

					sSubStr = sStatement.SubStrBasis(nStartPos + 1, nEndPos);	
					m_poParamTemplate->setParam(sSubStr);
					nStartPosNew = nEndPos + 1;
					++nParamCount;
				}
				else
					nStartPosNew = nPosQuote2 + 1;
			}
			else
                return SAPDB_FALSE;
		}
        else
            nStartPosNew = nStatementLen + 1;        		
    }	
    if (nParamCount < 1) return SAPDB_FALSE;

	m_poParamTemplate->writePage(Tools_TemplateWriterWA(rep),SAPDB_FALSE);    

	return SAPDB_TRUE;
    
}

SAPDB_Bool StudioWeb_Result::getStatementOfParamPage(sapdbwa_HttpRequest  &req,
													 Tools_DynamicUTF8String &sOldStatement,
													 Tools_DynamicUTF8String &sNewStatement)
{

	Tools_DynamicUTF8String sStatement;
	
	if(getParameterValue(WQ_NEXTPARAMBUTTON_TAG,req,sOldStatement) == SAPDB_FALSE) return SAPDB_FALSE;

	Tools_DynamicUTF8String::BasisElementIndex     nStartPos;
    Tools_DynamicUTF8String::BasisElementIndex     nEndPos;

	//first check for [ , ]
	nStartPos = sOldStatement.Find((SAPDB_UTF8*)"[");
	if (nStartPos == Tools_DynamicUTF8String::NPos) return SAPDB_FALSE;	
	nStartPos = sOldStatement.Find((SAPDB_UTF8*)"]");
	if (nStartPos == Tools_DynamicUTF8String::NPos) return SAPDB_FALSE;	

	//now other variables
	SAPDB_UInt				nStatementLen = sOldStatement.Size();
    SAPDB_UInt				nParamCount = 0;
	SAPDB_Bool				bQuoted = SAPDB_FALSE;
	Tools_DynamicUTF8String::BasisElementIndex     nStartPosNew = 0;
	Tools_DynamicUTF8String::BasisElementIndex     nPosQuote1 = 0;
	Tools_DynamicUTF8String::BasisElementIndex     nPosQuote2 = 0;    
	Tools_DynamicUTF8String::BasisElementIndex     nTailPos = Tools_DynamicUTF8String::NPos;    

	char sParamName[8];
    Tools_DynamicUTF8String sParamValue;				
	
	while (nStartPosNew <= nStatementLen && nStartPos != Tools_DynamicUTF8String::NPos){		
    
		nStartPos = sOldStatement.Find((SAPDB_UTF8*)"[",nStartPosNew);

		nPosQuote1  = sOldStatement.Find((SAPDB_UTF8*)"\"",nStartPosNew);        
        if (nPosQuote1 == Tools_DynamicUTF8String::NPos)
			nPosQuote1  = sOldStatement.Find((SAPDB_UTF8*)"'",nStartPosNew);        

		nEndPos = sOldStatement.Find((SAPDB_UTF8*)"]",nStartPosNew);

		//remember last valid end pos
		if (nEndPos != Tools_DynamicUTF8String::NPos) nTailPos = nEndPos + 1;
        nPosQuote2  = sOldStatement.Find((SAPDB_UTF8*)"\"",nEndPos);
        
        if (nPosQuote2 == Tools_DynamicUTF8String::NPos)
			nPosQuote2  = sOldStatement.Find((SAPDB_UTF8*)"'",nEndPos);        
		
        if (nStartPos != Tools_DynamicUTF8String::NPos){        
            
			if (nPosQuote1 != Tools_DynamicUTF8String::NPos &&
				nPosQuote2 != Tools_DynamicUTF8String::NPos &&
				nStartPos > nPosQuote1						&&
				nEndPos < nPosQuote2){
            
                bQuoted = SAPDB_TRUE;
                nStartPos = nPosQuote1;
            }

			if (nEndPos > nStartPos && 
				nEndPos != Tools_DynamicUTF8String::NPos &&
				nStartPos > nStartPosNew){

				if (nPosQuote1 != Tools_DynamicUTF8String::NPos && bQuoted)
                    bQuoted = SAPDB_TRUE;
                else
                    bQuoted = SAPDB_FALSE;
					
				if (bQuoted == SAPDB_FALSE){

					sNewStatement.Append(sOldStatement.SubStrBasis(nStartPosNew,nStartPos));	

					nStartPosNew = nEndPos + 1;
					++nParamCount;
					sprintf(sParamName,"T%d",nParamCount);				
					if(getParameterValue(sParamName,req,sParamValue))

						sNewStatement.Append(sParamValue);
					else
						return SAPDB_FALSE;    
				}
				else{					
					sNewStatement.Append(sOldStatement.SubStrBasis(nStartPosNew,nPosQuote2 + 1));						
					nStartPosNew = nPosQuote2 + 1;
				}
			}
			else
                nStartPosNew = nStatementLen + 1;            
		}
        else{

            nStartPosNew = nStatementLen + 1;        
		}
    }	
    if (nParamCount < 1) return SAPDB_FALSE;    

	if (nTailPos  != Tools_DynamicUTF8String::NPos) 
		sNewStatement.Append(sOldStatement.SubStrBasis(nTailPos ,sOldStatement.BasisSize()));						

	return SAPDB_TRUE;
    
}


void StudioWeb_Result::getFormatedStatement(StudioWeb_ResultCollection *pResultCollection,
											SAPDB_UInt lLength,
											Tools_DynamicUTF8String& sStatement)
{
	//format statement string to 64 chars
	Tools_DynamicUTF8String sBuffer = pResultCollection->sStatmentHeader;
	if (sBuffer.Size() > lLength){

		sBuffer = sBuffer.SubStr (0,lLength);
		sStatement = sBuffer;
		sStatement += (SAPDB_UTF8*) " ...";
	}
	else
		sStatement = sBuffer;
}




SAPDB_Bool StudioWeb_Result::getFetchParams(StudioWeb_ResultCollection *pResultCollection,
											StudioWeb_FetchDirection emFetchDir,
											StudioWeb_FetchDirection emLastAction,
											SAPDB_UInt2 &lSQLParam,
											SAPDB_UInt2 &lSQLParam2,
											SAPDB_Int4 &nStartRow)
{

	nStartRow = 1;
	if (emFetchDir == StudioWeb_FetchDirFirst){

		lSQLParam = SQL_FETCH_FIRST;
		lSQLParam2 = SQL_FETCH_NEXT;				
	}	
	else if (emFetchDir == StudioWeb_FetchDirPrevious){

		lSQLParam = SQL_FETCH_RELATIVE;
		lSQLParam2 = SQL_FETCH_PREV;		
		if (emLastAction == StudioWeb_FetchDirNext){
			
			nStartRow = -1 * (pResultCollection->lastResultSize);			
			//if the border of cursor was reached increase start row
			if (pResultCollection->bBorderOfResult &&
				pResultCollection->lastResultSize < cnstResultSize) --nStartRow;
		}
	}

	else if (emFetchDir == StudioWeb_FetchDirNext){

		lSQLParam = SQL_FETCH_RELATIVE;
		lSQLParam2 = SQL_FETCH_NEXT;
		if (emLastAction == StudioWeb_FetchDirPrevious) 
			nStartRow =  pResultCollection->lastResultSize;		

		//if the border of cursor was reached increase start row
		if (pResultCollection->bBorderOfResult &&
			pResultCollection->lastResultSize < cnstResultSize) ++nStartRow;
	}

	else if (emFetchDir == StudioWeb_FetchDirLast){

		lSQLParam = SQL_FETCH_LAST;
		lSQLParam2 = SQL_FETCH_PREV;		
	}
	else
		return SAPDB_FALSE;
	
	return SAPDB_TRUE;

}


void StudioWeb_Result::rollBack(Studio_Connection	*poConnection)
{

	if(poConnection == NULL) return;

	StudioOAL_WResult				*pDummyResult;
	pDummyResult = new StudioOAL_WResult(sapdbwa_DBCHandle(poConnection->getDBCHandle()),
										 sapdbwa_DBCEnv(poConnection->getDBCHandle()));
	/*set the sql mode for the result object PTS:1118901*/
	pDummyResult->setSqlMode(poConnection->getSQLMode());

	pDummyResult->fullExecStmt("rollback");	
	delete pDummyResult;
}
void StudioWeb_Result::fillResultList(StudioWeb_ResultCollection  *pResultCollection) 
{
	StudioWeb_ResultCollection  *poCurrentResultCol = (StudioWeb_ResultCollection*)m_ResultList.first();
	SAPDB_Int2 nRun = 1;
	char sBuf[8];
	Tools_DynamicUTF8String		sBuffer;	
	Tools_DynamicUTF8String		sBuffer2;		

    while (poCurrentResultCol != NULL){		
	
		//variables
		sBuffer2 = poCurrentResultCol->sStatmentHeader;
		getFormatedStatement(poCurrentResultCol,36,sBuffer2);	
		sprintf(sBuf,"%d",nRun);
		sBuffer = "(";
		sBuffer += sBuf;
		sBuffer += ") ";
		sBuffer += sBuffer2;
		
		if(pResultCollection == poCurrentResultCol)
			pResultCollection->pTable->setSelectedResult(nRun);
		
		pResultCollection->pTable->setResults(sBuffer,nRun);		
		poCurrentResultCol = (StudioWeb_ResultCollection*)m_ResultList.next();

		++nRun;
	} 
}



void StudioWeb_Result::clearResultCollection() 
{    
	
	StudioWeb_ResultCollection  *poCurrentResultCol = (StudioWeb_ResultCollection*)m_ResultList.first();
    while (poCurrentResultCol != NULL){		

		//delete all result objects
		if(poCurrentResultCol->pResult != NULL){

			delete poCurrentResultCol->pResult;
			poCurrentResultCol->pResult = NULL;
		}
		//delete all table templates
		if(poCurrentResultCol->pTable != NULL){

			delete poCurrentResultCol->pTable;
			poCurrentResultCol->pTable = NULL;
		}
		//delete collection struct
		delete poCurrentResultCol;

		poCurrentResultCol = (StudioWeb_ResultCollection*)m_ResultList.next();
	} 

	//clear list
	m_ResultList.clear();
		
    return;
}



StudioWeb_Result::StudioWeb_ResultCollection* StudioWeb_Result::getNewResultCollection() 
{
	StudioWeb_ResultCollection* pResultCollection = new  StudioWeb_ResultCollection;

	if (pResultCollection == NULL)  return NULL;	
	pResultCollection->pResult = new StudioOAL_WResult(sapdbwa_DBCHandle(m_poExecuteConnection->getDBCHandle()),
													   sapdbwa_DBCEnv(m_poExecuteConnection->getDBCHandle()));
	/*set the sql mode for the result object PTS:1118901*/
	pResultCollection->pResult->setSqlMode(m_poExecuteConnection->getSQLMode());
	//check for valid result
	if (pResultCollection->pResult == NULL)
	{
		delete pResultCollection;
		return NULL;
	}
	pResultCollection->pResult->setLimitForLong (cnstSmallLong);		

	//if result is executed in sapdb mode set special cursor for scrolling
	if(m_poExecuteConnection->getSQLMode() == cpr_kind_internal) 
		pResultCollection->pResult->setResultCursorType(StudioOAL_WResult::stoalOptimizeCursor);	
	else
		pResultCollection->pResult->setResultCursorType(StudioOAL_WResult::stoalCursorForwardOnly);	


	pResultCollection->pTable = NULL;
	pResultCollection->lastResultSize = 0;
	pResultCollection->lastAction = StudioWeb_FetchDirNone;

	return pResultCollection;

}

SAPDB_Int2 StudioWeb_Result::getResultNo(sapdbwa_HttpRequest  &req)
{

	Tools_DynamicUTF8String sValue;
	getParameterValue(WQ_RESULTS_TAG,req,sValue); 
	if(sValue.Empty()) return 0;	

	Tools_DynamicUTF8String::BasisElementIndex pos = 0;
	pos   = sValue.Find (")");
	if(pos == Tools_DynamicUTF8String::NPos) return 0;

    Tools_DynamicUTF8String sBuffer;
	sBuffer = sValue.SubStrBasis (1, pos);	

	char* szpConvert = NULL;
	char* pValue = new char[sBuffer.Size()+1];
	memset(pValue,0,sBuffer.Size()+1);
	if(sBuffer.ConvertToASCII_Latin1((char*)pValue,pValue+sBuffer.Size(),szpConvert) 
		!= Tools_UTF8Basis::Success){
		return 0;
	}
	if(pValue == NULL) return 0;

	return (atoi(pValue) -1);
}


void StudioWeb_Result::drawResultNo(SAPDB_Int2	nResultNo,
									StudioWeb_FetchDirection fDir,
									sapdbwa_WebAgent     &wa,
									sapdbwa_HttpReply    &rep)
{

	StudioWeb_ResultCollection *pResultCollection = (StudioWeb_ResultCollection *)m_ResultList.at(nResultNo);
	if (pResultCollection != NULL)
	{
		//get valid template
		if(pResultCollection->pTable != NULL)
		{
			//if there is no fetch direction  redraw old table
			if(fDir == StudioWeb_FetchDirNone){

				pResultCollection->pTable->resetStartValues();
				//if we fetched  back, and there are less rows than start row
				//set current row of template to cnstResultSize-nRowCount
				if ((pResultCollection->lastAction == StudioWeb_FetchDirLast || 
					 pResultCollection->lastAction == StudioWeb_FetchDirPrevious)
					&&
					(pResultCollection->lastResultSize < cnstResultSize))
					pResultCollection->pTable->setStartRow(cnstResultSize-pResultCollection->lastResultSize);
				
				pResultCollection->pTable->writePage(Tools_TemplateWriterWA(rep),SAPDB_FALSE);
			}
			else
				sendResultPage(wa,rep,pResultCollection,fDir);			
		}
		else{			
			//if there was no result before fetch direction is StudioWeb_FetchDirFirst
			fDir = StudioWeb_FetchDirFirst;
			pResultCollection->pTable = new StudioWeb_ResultTemplate(wa);					

			sendResultPage(wa,rep,pResultCollection,fDir);			
		}
	}	
}

SAPDB_Bool StudioWeb_Result::doZoom (SAPDB_Int2	nResultNo,
									 sapdbwa_WebAgent     & wa,
	  								 sapdbwa_HttpRequest  &req,
									 sapdbwa_HttpReply    &rep)
{
	//zooming is just possible with internal sql mode
	if(m_poExecuteConnection->getSQLMode() != cpr_kind_internal) return SAPDB_FALSE;

	StudioWeb_ResultCollection *pResultCollection = (StudioWeb_ResultCollection *)m_ResultList.at(nResultNo);
	if (pResultCollection == NULL)  return SAPDB_FALSE;
	if (pResultCollection->pResult == NULL)  return SAPDB_FALSE;
	
	if(pResultCollection->lastAction == StudioWeb_FetchDirNone){

		rep.SendBody(WQ_ERR036);
		return SAPDB_FALSE;
	}

	//get a valid template for zooming
	if(m_poZoomTemplate == NULL)
		m_poZoomTemplate = new StudioWeb_ResultTemplate(wa);  
	else
		m_poZoomTemplate->resetTemplate();

	//local variables//////////////////
	Tools_DynamicUTF8String sValue; 		
	char sAsciiBuffer[16];
	SAPDB_Char* szpConvert = NULL;
	////////////////////////////////////

	//get the zoom row number from the request
	getParameterValue(WQ_ZOOM_ROW_TAG,req,sValue); 
	memset(sAsciiBuffer,0,16);
	sValue.ConvertToASCII_Latin1(sAsciiBuffer,sAsciiBuffer+sizeof(sAsciiBuffer),szpConvert);
	SAPDB_Int2 nRow = atoi((char*)sAsciiBuffer) ;

	//get the zoom col number from the request
	getParameterValue(WQ_ZOOM_COL_TAG,req,sValue); 
	memset(sAsciiBuffer,0,16);
	sValue.ConvertToASCII_Latin1(sAsciiBuffer,sAsciiBuffer+sizeof(sAsciiBuffer),szpConvert);	
	SAPDB_Int2 nCol = atoi((char*)sAsciiBuffer) ;

	//set the odbc cursor to the selected zoom position
	if(setZoomPos(pResultCollection,nRow,nCol) == SAPDB_FALSE){

		handleError(pResultCollection->pResult,rep);			
		return SAPDB_FALSE;
	}
	
	//local variables//////////////////
	SAPDB_ULong					noRowsFetched = 0;
	SAPDB_UInt2					nRowStatus = 0;
	SAPDB_Long					lSize = 0;
	SAPDB_Bool					bDataTrunc = SAPDB_FALSE;
	SAPDB_Bool					bOk = SAPDB_FALSE;
	Tools_DynamicUTF8String		sBuffer;
	char						sHex[8];
	////////////////////////////////////

	//set new long limit value
	SAPDB_Byte *pValue = new SAPDB_Byte [cnstBigLong];
	if (pValue == NULL) return SAPDB_FALSE;			

	if (pResultCollection->pResult->getLongData(nCol,pValue,cnstBigLong,&lSize,&bDataTrunc)){	

		//build hex view of data		
		SAPDB_Int2  nY = 1;
		SAPDB_Int2  nX = 1;		
		SAPDB_Long i = 0;
				
		//for ascii char show ascii chars
		SQLSMALLINT	nColType = 0;
		pResultCollection->pResult->getODBCColType(nCol,nColType);
		if(nColType == SQL_LONGVARCHAR ||
		   nColType == SQL_WLONGVARCHAR){
			
			bOk = pResultCollection->pResult->extendedFetch(SQL_FETCH_RELATIVE,0,&noRowsFetched,&nRowStatus);	
			if(!bOk){

				handleError(pResultCollection->pResult,rep);			
				return SAPDB_FALSE;
			}	

			Tools_DynamicUTF8String sZoomText;

			SAPDB_ULong old_lLimitForLong = pResultCollection->pResult->getLimitForLong();
			pResultCollection->pResult->setLimitForLong(cnstBigLong);

			if (pResultCollection->pResult->getColumnDataAsString(nCol,sZoomText))		
				m_poZoomTemplate->setZoomText(sZoomText);

			pResultCollection->pResult->setLimitForLong(old_lLimitForLong);
			
		
		}		
		for(i = 0; i < lSize; i++){
			
			sprintf (sHex, "%.2X ", (int)pValue[i]);
			m_poZoomTemplate->setCell(sHex,nX,nY);
			++nY;
			if (nY > 32) {

				nY = 1;
				++nX;
			}
		}	
	}

	//delete buffer provided by getLongData
	delete[] pValue;

	//set cursor back
	bOk = pResultCollection->pResult->extendedFetch(SQL_FETCH_RELATIVE,-nRow,&noRowsFetched,&nRowStatus);
	if(!bOk){

		handleError(pResultCollection->pResult,rep);			
		return SAPDB_FALSE;	
	}	
	
	//build table title
	sBuffer = WQ_HEXVIEW;
	if(bDataTrunc) {

			//append warning if buffer for long was too small
			sBuffer.Append(WQ_ZOOMWARN1);
			//use sHex again as biffer
			sprintf (sHex, "%d", cnstBigLong);
			sBuffer.Append(sHex);
			sBuffer.Append(WQ_ZOOMWARN2);
	}
	m_poZoomTemplate->setTableTitle(sBuffer);	

	//build href for back button
	sBuffer = WQ_CREATEWEBSQLURL;
	sBuffer.Append(WQ_ZOOMBACK_TAG);
	sBuffer.Append("\")");
	
	
	m_poZoomTemplate->addButton(WQ_ZOOMBACKTEXT_TAG, sBuffer,WQ_TARGETSELF,SAPDB_FALSE);
	m_poZoomTemplate->writePage(Tools_TemplateWriterWA(rep),SAPDB_FALSE);    			

	return SAPDB_TRUE;
}


SAPDB_Bool StudioWeb_Result::setZoomPos (StudioWeb_ResultCollection* pResultCollection,
										 SAPDB_Int2 &nRow,
										 SAPDB_Int2 &nCol)
{
	
	SAPDB_Bool		bOk;
	SAPDB_ULong		noRowsFetched = 0;
	SAPDB_UInt2		nRowStatus = 0;
	SAPDB_Int4		nLastPos = 0;

	//get the last position of the odbc cursor
	pResultCollection->pResult->getCursorPos(nLastPos);		

	//fetch to zoom position
	//if last action was a fetch first, 
	//fetch relative to the desired row minus the last result size if last cursor pos is negative
	if (pResultCollection->lastAction == StudioWeb_FetchDirFirst) {
				
		if(nLastPos > 0) 
			nRow -= cnstResultSize;
		else
			nRow -= (pResultCollection->lastResultSize + 1);		
			
	}	
	//if last action was a fetch last,
	//fetch relative depends whether last pos is positive or negative
	//if last pos is negative the end of the cursor was reached
	//fetch relative to the desired row minus the last result size 
	//plus one, to get into a valid cursor pos
	if (pResultCollection->lastAction == StudioWeb_FetchDirNext){

		if(nLastPos > 0) 
			nRow -= cnstResultSize;
		else
			nRow -= (pResultCollection->lastResultSize + 1);		
	}
	if (pResultCollection->lastAction == StudioWeb_FetchDirPrevious){

		if(pResultCollection->bBorderOfResult){

			nRow = nRow + pResultCollection->lastResultSize - cnstResultSize;
			if(pResultCollection->lastResultSize == cnstResultSize) --nRow;
		}
		else
			nRow -= 1;	
	}
	if (pResultCollection->lastAction == StudioWeb_FetchDirLast) nRow -=1;	

	//set cursor pos with a relative fetch from actually version
	bOk = pResultCollection->pResult->extendedFetch(SQL_FETCH_RELATIVE,nRow,&noRowsFetched,&nRowStatus);	
	if(!bOk)		
		return SAPDB_FALSE;	

	return SAPDB_TRUE;
}