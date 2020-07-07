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

  module: StudioWeb_Functions.cpp

  -----------------------------------------------------------------------

  responsible: WolfgangA

  special area: WebQuery

  description:  generic functions for WebQuery

  see also:

  -----------------------------------------------------------------------

  copyright:                Copyright (c) 2000-2005 SAP AG

************************************************************************/

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "SAPDB/SQLStudio/WebQuery/StudioWeb_Functions.hpp"

#include "ToolsCommon/Tools_TemplateMsgBox.hpp"

#include "SAPDB/SQLStudio/OAL/StudioOAL_WCommon.h"

#include "SAPDBCommon/SAPDB_string.h"



//html code fragments
#include "SAPDB/SQLStudio/WebQuery/StudioWeb_HtmlFragments.h"

#define SQL1	     "SELECT TABLENAME FROM DOMAIN.TABLES WHERE OWNER ="
#define SQL2		 " AND TABLENAME ='"
#define SYSDBA		 "SYSDBA"
#define SIMPLEQUOTE	 "'"



void replaceNoCharWithDot(Tools_DynamicUTF8String &sInAndOut)

{

	if (sInAndOut.Size() < 1) return;
    
	
	Tools_DynamicUTF8String::BasisElementIndex Pos;
	Tools_DynamicUTF8String sFind;
	Tools_DynamicUTF8String sReplace = ".";
	
	for(SAPDB_Int i= 0;i < 31;i++){

		sFind = (char)i;
		Pos = sInAndOut.Find(sFind);
		if(Pos != Tools_DynamicUTF8String::NPos){

			while (Pos != Tools_DynamicUTF8String::NPos) {
				
				sInAndOut.Replace(Pos,1,sReplace);
				Pos = sInAndOut.Find(sFind);
			}
		}
	}
}


SAPDB_Bool initSOFromRequest(Studio_StoredFileContent	*poSFC,
			 			     sapdbwa_HttpRequest  &req)
{
	if(poSFC == NULL)  return SAPDB_FALSE;		   

    Tools_DynamicUTF8String sDummy;
    getParameterValue(WQ_SQLQUERYTEXT,req,sDummy);

	poSFC->setSQLData(sDummy);

	//if user is exlusive standard values are used
    getParameterValue(WQ_AUTOCOMMIT_TAG,req,sDummy);
	//!!! these values are used in wqsql.htm !!!
	//On
	//Off
	if(sDummy == "On") 
		poSFC->setAutoCommit(SQL_AUTOCOMMIT_ON);
	else if (sDummy == "Off") 
		poSFC->setAutoCommit(SQL_AUTOCOMMIT_OFF);
	else
		poSFC->setAutoCommit(SQL_AUTOCOMMIT_ON);
	
  	//!!! these values are used in wqsql.htm !!!
	//Internal
	//Oracle
	//DB2
	//Ansi
	/*typedef enum tpr00_SQLModeEnum {
		  cpr_kind_empty         =   0,
		  cpr_kind_internal      =   1,
		  cpr_kind_db2           =   2,
		  cpr_kind_ansi          =   3,
		  cpr_kind_oracle        =   4,
		  cpr_kind_sapr3         =   5
		} tpr00_SQLModeEnum;*/
    getParameterValue(WQ_SQLMODE_TAG,req,sDummy);

	if(sDummy == "Internal")
		poSFC->setSQLMode(cpr_kind_internal);
	else if (sDummy == "Oracle")
		poSFC->setSQLMode(cpr_kind_oracle);
	else if (sDummy == "DB2")
		poSFC->setSQLMode(cpr_kind_db2);
	else if (sDummy == "Ansi")
		poSFC->setSQLMode(cpr_kind_ansi);
	else
		poSFC->setSQLMode(cpr_kind_internal);
		

    getParameterValue(WQ_ISOLATIONLEVEL_TAG,req,sDummy);
	//!!! these values are used in wqsql.htm !!!
	//Uncommitted
	//Committed
	//Repeatable
	//Serializable
	//SQL_TXN_READ_UNCOMMITTED            0x00000001L
	//SQL_TXN_READ_COMMITTED              0x00000002L
	//SQL_TXN_REPEATABLE_READ             0x00000004L
	//SQL_TXN_SERIALIZABLE                0x00000008L
	
	if(sDummy == "Uncommitted")
		poSFC->setIsoLevel(SQL_TXN_READ_UNCOMMITTED);
	else if(sDummy == "Committed")
		poSFC->setIsoLevel(SQL_TXN_READ_COMMITTED);		
	else if(sDummy == "Repeatable")
		poSFC->setIsoLevel(SQL_TXN_REPEATABLE_READ);
	else if(sDummy == "Serializable")
		poSFC->setIsoLevel(SQL_TXN_SERIALIZABLE);
	else 
		poSFC->setIsoLevel(SQL_TXN_READ_COMMITTED);		

	

	return SAPDB_TRUE;
}


SAPDB_Bool checkUserNotExlusive(sapdbwa_DBCP pDbc,
							    const Tools_DynamicUTF8String& sUser)
{

	if (pDbc == NULL) return SAPDB_FALSE;

	StudioOAL_WResult		*pResult = NULL;	
	Tools_DynamicUTF8String	sStatement;
	

	sStatement = "SELECT DOMAIN.USERS.CONNECTMODE FROM DOMAIN.USERS WHERE DOMAIN.USERS.USERNAME = USER";

	pResult = new StudioOAL_WResult(sapdbwa_DBCHandle(pDbc),sapdbwa_DBCEnv(pDbc));
	pResult->setLimitForLong (cnstLimitForLong);
	
 	if (pResult->openResult(sStatement) == SAPDB_FALSE) {

		delete pResult;
		pResult = NULL;
		return SAPDB_FALSE;
	}
    
    if (pResult->fetch() == SAPDB_FALSE) {

		delete pResult;
		pResult = NULL;
		return SAPDB_FALSE;
	}

    if (pResult->noRows()){

		delete pResult;
		pResult = NULL;
		return SAPDB_FALSE;
	}

	Tools_DynamicUTF8String sBuffer;
	if (pResult->getColumnDataAsString(1,sBuffer) == SAPDB_FALSE) {

		delete pResult;
		return SAPDB_FALSE;
	}

	delete pResult;
	
	return (sBuffer != "MULTIPLE") ? SAPDB_FALSE : SAPDB_TRUE;
}


SAPDB_Bool checkForSystemTable(sapdbwa_DBCP pDbc,
							   const Tools_DynamicUTF8String& sOwner,
							   const Tools_DynamicUTF8String& sName)

{	

	if (pDbc == NULL) return SAPDB_FALSE;

	//Variables
	SAPDB_Bool bRet = SAPDB_FALSE;
	StudioOAL_WResult		*pResult = NULL;	
	Tools_DynamicUTF8String	sStatement;

	sStatement = (SAPDB_UTF8*)SQL1;
    if (sOwner != (SAPDB_UTF8*)SYSDBA)  sStatement += (SAPDB_UTF8*)SIMPLEQUOTE;
    sStatement += sOwner;
    if (sOwner != (SAPDB_UTF8*)SYSDBA)  sStatement += (SAPDB_UTF8*)SIMPLEQUOTE;
    sStatement += (SAPDB_UTF8*)SQL2;
	sStatement += sName; 
	sStatement += (SAPDB_UTF8*)SIMPLEQUOTE;	

	pResult = new StudioOAL_WResult(sapdbwa_DBCHandle(pDbc),sapdbwa_DBCEnv(pDbc));
	pResult->setLimitForLong (cnstLimitForLong);
	
 	if (pResult->openResult(sStatement) == SAPDB_FALSE) {

		delete pResult;
		pResult = NULL;
		return SAPDB_FALSE;
	}
    
    if (pResult->fetch() == SAPDB_FALSE) {

		delete pResult;
		pResult = NULL;
		return SAPDB_FALSE;
	}

    bRet = !pResult->noRows();

	delete pResult;
	pResult = NULL;

	return bRet;

}

SAPDB_Bool checkForSystemTable2(sapdbwa_DBCP pDbc,
							   const Tools_DynamicUTF8String& sOwner,
							   const Tools_DynamicUTF8String& sName)

{	

	if (pDbc == NULL) return SAPDB_FALSE;

	//Variables
	SAPDB_Bool bRet = SAPDB_FALSE;
	StudioOAL_WResult		*pResult = NULL;	
	Tools_DynamicUTF8String	sStatement;

	sStatement = (SAPDB_UTF8*)"select * from ";
    sStatement += sOwner;
    sStatement += (SAPDB_UTF8*)".";
	sStatement += sName; 
	
	pResult = new StudioOAL_WResult(sapdbwa_DBCHandle(pDbc),sapdbwa_DBCEnv(pDbc));
	pResult->setLimitForLong (cnstLimitForLong);
	
 	if (pResult->openResult(sStatement) == SAPDB_FALSE) {

		delete pResult;
		pResult = NULL;
		return SAPDB_FALSE;
	}
    
    if (pResult->fetch() == SAPDB_FALSE) {

		delete pResult;
		pResult = NULL;
		return SAPDB_FALSE;
	}

	//if no error occured the function returns true
    bRet = SAPDB_TRUE;

	delete pResult;
	pResult = NULL;

	return bRet;

}


void sendUtf8ErrorMessageBox(sapdbwa_WebAgent &wa,
						sapdbwa_HttpReply &rep,
						Tools_DynamicUTF8String &sMessage)
{
		Tools_TemplateMsgBox oMsgBox(wa);
		oMsgBox.enableLogo(SAPDB_FALSE)
			   .setMessage(sMessage)
			   .writePage(Tools_TemplateWriterWA(rep));
}



void sendErrorMessageBox(sapdbwa_WebAgent &wa,
						sapdbwa_HttpReply &rep,
						const char *pMessage)
{
		Tools_TemplateMsgBox oMsgBox(wa);
		oMsgBox.enableLogo(SAPDB_FALSE)
			   .setMessage(pMessage)
			   .writePage(Tools_TemplateWriterWA(rep));
}

void escapeString(Tools_DynamicUTF8String &sData)
{

	Tools_DynamicUTF8String sNew;	
	Tools_DynamicUTF8String::BasisElementIndex Pos1 = 0;
	
	while(sData.Size() > 0)
	{
		if ((Pos1 = sData.Find((SAPDB_UTF8*)SPACE)) != Tools_DynamicUTF8String::NPos){

			sNew += sData.SubStr(0,Pos1);
			sNew += (SAPDB_UTF8*)SPACETAG;
			sData = sData.SubStr(Pos1+1);
		}
		else{
			sNew += sData;
			break;
		}
	}

	sData = sNew;
}


SAPDB_Bool escapeAsUTF8String(Tools_DynamicUTF8String &sData)
{

	char   sBuf[8];
	Tools_DynamicUTF8String sNew;	

	SAPDB_UCS2* pConvertOut = NULL;
	SAPDB_UCS2	ucs2Buffer[4096];	

	/*-------------------------------------------------------------------------*
     * convert utf8 to ucs2									 			       *
     *-------------------------------------------------------------------------*/
	if (sData.ConvertToUCS2(ucs2Buffer,
						    (ucs2Buffer+(sizeof(ucs2Buffer)/sizeof(SAPDB_UCS2))),
							pConvertOut) != Tools_UTF8Basis::Success ) return SAPDB_FALSE;		

	SAPDB_UCS2 end = (SAPDB_UCS2*)(pConvertOut)-(SAPDB_UCS2*)ucs2Buffer;
	for(SAPDB_UInt k = 0;k < end;k++){

		sNew += "\\u";			
		sprintf(sBuf,"%04x",ucs2Buffer[k]);
		sNew += sBuf;
	}

	sData = sNew;

	return SAPDB_TRUE;
}


void quoteString(Tools_DynamicUTF8String &sData)
{

	Tools_DynamicUTF8String sNew;	
	Tools_DynamicUTF8String::BasisElementIndex Pos1 = 0;
	
	while(sData.Size() > 0)
	{
		if ((Pos1 = sData.Find((SAPDB_UTF8*)QUOTE)) != Tools_DynamicUTF8String::NPos){

			sNew += sData.SubStr(0,Pos1);
			sNew += (SAPDB_UTF8*)QUOTETAG;
			sData = sData.SubStr(Pos1+1);
		}
		else{
			sNew += sData;
			break;
		}
	}

	sData = sNew;
}



void sendHTMLFile(sapdbwa_WebAgent &wa,
				  sapdbwa_HttpReply &rep,
				  const Tools_DynamicUTF8String& sFile)
{

	//NOT UNICODE BECAUSE OF WA
	SAPDB_Char	sAsciiFile[512];
	memset(sAsciiFile,0,512);	
	SAPDB_Char* szpConvert = NULL;

	Tools_DynamicUTF8String sCopyFile = sFile;
	assert(sCopyFile.ConvertToASCII_Latin1((char*)sAsciiFile,sAsciiFile+sizeof (sAsciiFile),szpConvert) == Tools_UTF8Basis::Success);	

	SAPDB_Char   szPath[256] = "";
	wa.GetDocumentRoot(szPath, 200); 
	strcat(szPath,"/");
	strcat(szPath, SAPDBWA_DOCUMENT_ROOT_HTML);
	strcat(szPath,"/");
	strcat(szPath, sAsciiFile);

	sapdbwa_Bool bRet = rep.SendTemplate(szPath);		
	if(bRet == sapdbwa_False) 
	{
		SAPDB_strcpy(sAsciiFile,"Error sending: ");
		strcat(sAsciiFile,szPath);
		wa.WriteLogMsg(sAsciiFile);
	}
	
}


SAPDB_Bool sendUTF8DataAsAscii(sapdbwa_HttpReply &rep,
						 const Tools_DynamicUTF8String& sData)
{

	SAPDB_Char	*psData = new char [sData.Size()+1];
	SAPDB_Char* szpConvert = NULL;

	SAPDB_Bool bRet = SAPDB_FALSE;

	Tools_DynamicUTF8String sCopyData = sData;
	if(sCopyData.ConvertToASCII_Latin1((char*)psData,psData+sCopyData.Size(),szpConvert) == Tools_UTF8Basis::Success){

		psData[sCopyData.Size()] = 0;
		bRet = SAPDB_TRUE;
	}

	rep.SendBody(psData);

	delete[] psData;
	return bRet;
	
}


void sendPageBegin(sapdbwa_HttpReply &rep,const char *pTitle)
{
	sendPageHeader(rep);	

	rep.SendBody(WQ_HTML_BEGIN);
	rep.SendBody(WQ_HTML_HEADER_BEGIN);

	rep.SendBody(WQ_UTF8ENCODING);

	rep.SendBody(WQ_STYLESHEET);

	//second send javascript to display buttons
	rep.SendBody(WQ_JS_BEGIN3);
	rep.SendBody("\"/WARoot/JavaScripts/wqbutton.js\">");
	rep.SendBody(WQ_JS_END);
	
	
	rep.SendBody(WQ_TITLE_BEGIN);
	if (pTitle != NULL)  rep.SendBody(pTitle);
	rep.SendBody(WQ_TITLE_END);

	rep.SendBody(WQ_HTML_HEADER_END);		
}

void sendPageEnd(sapdbwa_HttpReply &rep)
{
	rep.SendBody(WQ_BODYCLOSE);
	rep.SendBody(WQ_HTML_END);
}

void sendPageHeader(sapdbwa_HttpReply &rep)
{
	rep.InitHeader(sapdbwa_Status_Ok, "text/html", NULL, NULL,NULL, NULL );
	rep.SetHeader ("Expires", "Sat, 01 Jan 2000 00:00:00 GMT");
	rep.SetHeader ("charset","UTF-8");
	rep.SendHeader();
}

SAPDB_Bool isDSqlRequest (sapdbwa_HttpRequest  &req)
{
	
	const char		*queryString = req.GetQueryString();

	if (queryString == NULL) return SAPDB_FALSE;	

	//if querystring is just dsql and no params retrun SAPDB_TRUE;
	if (strcmp(queryString,WQ_DSQL_WINDOW) == 0) return SAPDB_TRUE;					
	
	sapdbwa_StringSeq paramNames;	
	//now check params
	if (req.GetParameterNames(paramNames)) {

		if (paramNames[0] == NULL) return SAPDB_FALSE;
		if (strcmp((char *)paramNames[0],(char*)WQ_DSQL_WINDOW) != 0) return SAPDB_FALSE;		
	}

	return SAPDB_TRUE;					
}



SAPDB_Bool isNextStatementRequest (sapdbwa_HttpRequest  &req)
{
	
	const char		*queryString = req.GetQueryString();

	if (queryString == NULL) return SAPDB_FALSE;	

	
	sapdbwa_StringSeq paramNames;	
	//now check params
	if (req.GetParameterNames(paramNames)) {

		if (paramNames[0] == NULL) return SAPDB_FALSE;
		if (strcmp((char *)paramNames[0],(char*)WQ_DSQL_WINDOW) != 0) return SAPDB_FALSE;		

		sapdbwa_StringSeq paramValues;
		if (req.GetParameterValues(WQ_DSQL_WINDOW,paramValues) == SAPDB_FALSE) return SAPDB_FALSE;		


		if (strcmp(paramValues[0],WQ_NEXTSTATEMENT_TAG) != 0) return SAPDB_FALSE;		
	}

	return SAPDB_TRUE;					
}

SAPDB_Bool isDSQLRefreshRequest (sapdbwa_HttpRequest  &req)
{
	
	const char		*queryString = req.GetQueryString();

	if (queryString == NULL) return SAPDB_FALSE;	
	
	sapdbwa_StringSeq paramNames;	
	//now check params
	if (req.GetParameterNames(paramNames)) {

		if (paramNames[0] == NULL) return SAPDB_FALSE;
		if (strcmp((char *)paramNames[0],(char*)WQ_DSQL_WINDOW) != 0) return SAPDB_FALSE;		

		sapdbwa_StringSeq paramValues;
		if (req.GetParameterValues(WQ_DSQL_WINDOW,paramValues) == SAPDB_FALSE) return SAPDB_FALSE;		


		if (strcmp(paramValues[0],WQ_REFRESH_TAG) != 0) return SAPDB_FALSE;		
	}

	return SAPDB_TRUE;					
}

SAPDB_Bool isPrevStatementRequest (sapdbwa_HttpRequest  &req)
{
	
	const char		*queryString = req.GetQueryString();

	if (queryString == NULL) return SAPDB_FALSE;	

	
	sapdbwa_StringSeq paramNames;	
	//now check params
	if (req.GetParameterNames(paramNames)) {

		if (paramNames[0] == NULL) return SAPDB_FALSE;
		if (strcmp((char *)paramNames[0],(char*)WQ_DSQL_WINDOW) != 0) return SAPDB_FALSE;		

		sapdbwa_StringSeq paramValues;
		if (req.GetParameterValues(WQ_DSQL_WINDOW,paramValues) == SAPDB_FALSE) return SAPDB_FALSE;		


		if (strcmp(paramValues[0],WQ_PREVSTATEMENT_TAG) != 0) return SAPDB_FALSE;		
	}

	return SAPDB_TRUE;					
}



SAPDB_Bool isClearStatementRequest (sapdbwa_HttpRequest  &req)
{
	
	const char		*queryString = req.GetQueryString();

	if (queryString == NULL) return SAPDB_FALSE;	
	
	sapdbwa_StringSeq paramNames;	
	//now check params
	if (req.GetParameterNames(paramNames)) {

		if (paramNames[0] == NULL) return SAPDB_FALSE;
		if (strcmp((char *)paramNames[0],(char*)WQ_DSQL_WINDOW) != 0) return SAPDB_FALSE;		

		sapdbwa_StringSeq paramValues;
		if (req.GetParameterValues(WQ_DSQL_WINDOW,paramValues) == SAPDB_FALSE) return SAPDB_FALSE;	


		if (strcmp(paramValues[0],WQ_CLEAR_TAG) != 0) return SAPDB_FALSE;		
	}

	return SAPDB_TRUE;					
}

SAPDB_Bool isDBFSRequest (sapdbwa_HttpRequest  &req)
{
	
	const char		*queryString = req.GetQueryString();

	if (queryString == NULL) return SAPDB_FALSE;		
	if (strcmp(queryString,WQ_CREATENEWFOLDER) == 0) return SAPDB_TRUE;					
	if (strcmp(queryString,WQ_DELETEITEMFOLDER) == 0) return SAPDB_TRUE;					
	if (strcmp(queryString,WQ_DELETEITEMFILE) == 0) return SAPDB_TRUE;
	if (strcmp(queryString,WQ_RENAMEITEM) == 0) return SAPDB_TRUE;
	if (strcmp(queryString,WQ_MOVETOFOLDER) == 0) return SAPDB_TRUE;						
	if (strcmp(queryString,WQ_IMPORTITEMFILE) == 0) return SAPDB_TRUE;						
	if (strcmp(queryString,WQ_IMPORTITEMFOLDER) == 0) return SAPDB_TRUE;						
	if (strcmp(queryString,WQ_EXPORTITEMFILE) == 0) return SAPDB_TRUE;						
	if (strcmp(queryString,WQ_EXPORTITEMFOLDER) == 0) return SAPDB_TRUE;						

	return SAPDB_FALSE;					
}


SAPDB_Bool isCreateFolderRequest (sapdbwa_HttpRequest  &req)
{	
	
	const char		*queryString = req.GetQueryString();

	if (queryString == NULL) return SAPDB_FALSE;		
	if (strcmp(queryString,WQ_CREATENEWFOLDER) == 0) return SAPDB_TRUE;					

	return SAPDB_FALSE;					
}


SAPDB_Bool isDeleteFolderRequest (sapdbwa_HttpRequest  &req)
{
	
	const char		*queryString = req.GetQueryString();

	if (queryString == NULL) return SAPDB_FALSE;		
	if (strcmp(queryString,WQ_DELETEITEMFOLDER) == 0) return SAPDB_TRUE;					

	return SAPDB_FALSE;					
}

SAPDB_Bool isDeleteFileRequest (sapdbwa_HttpRequest  &req)
{
	
	const char		*queryString = req.GetQueryString();

	if (queryString == NULL) return SAPDB_FALSE;		
	if (strcmp(queryString,WQ_DELETEITEMFILE) == 0) return SAPDB_TRUE;					

	return SAPDB_FALSE;					
}

SAPDB_Bool isImportFolderRequest (sapdbwa_HttpRequest  &req)
{
	
	const char		*queryString = req.GetQueryString();

	if (queryString == NULL) return SAPDB_FALSE;		
	if (strcmp(queryString,WQ_IMPORTITEMFOLDER) == 0) return SAPDB_TRUE;					

	return SAPDB_FALSE;					
}

SAPDB_Bool isImportFileRequest (sapdbwa_HttpRequest  &req)
{
	
	const char		*queryString = req.GetQueryString();

	if (queryString == NULL) return SAPDB_FALSE;		
	if (strcmp(queryString,WQ_IMPORTITEMFILE) == 0) return SAPDB_TRUE;					

	return SAPDB_FALSE;					
}

SAPDB_Bool isExportFolderRequest (sapdbwa_HttpRequest  &req)
{
	
	const char		*queryString = req.GetQueryString();

	if (queryString == NULL) return SAPDB_FALSE;		
	if (strcmp(queryString,WQ_EXPORTITEMFOLDER) == 0) return SAPDB_TRUE;					

	return SAPDB_FALSE;					
}

SAPDB_Bool isExportFileRequest (sapdbwa_HttpRequest  &req)
{
	
	const char		*queryString = req.GetQueryString();

	if (queryString == NULL) return SAPDB_FALSE;		
	if (strcmp(queryString,WQ_EXPORTITEMFILE) == 0) return SAPDB_TRUE;					

	return SAPDB_FALSE;					
}

SAPDB_Bool isRenameItemRequest (sapdbwa_HttpRequest  &req)
{
	
	const char		*queryString = req.GetQueryString();

	if (queryString == NULL) return SAPDB_FALSE;	
	if (strcmp(queryString,WQ_RENAMEITEM) == 0) return SAPDB_TRUE;					

	return SAPDB_FALSE;					
}


SAPDB_Bool isMoveToFolderRequest (sapdbwa_HttpRequest  &req)
{
	const char		*queryString = req.GetQueryString();

	if (queryString == NULL) return SAPDB_FALSE;	
	if (strcmp(queryString,WQ_MOVETOFOLDER) == 0) return SAPDB_TRUE;					

	return SAPDB_FALSE;					
}


SAPDB_Bool isQueryStringValid (sapdbwa_HttpRequest  &req)
{

	const char		*queryString = req.GetQueryString();
	if (queryString == NULL) return SAPDB_FALSE;	

	return SAPDB_TRUE;					
}

SAPDB_Bool isHeaderPageRequest (sapdbwa_HttpRequest  &req,SAPDB_Bool &bLogOffButton)
{
	const char		*queryString = req.GetQueryString();
	if (queryString == NULL) return SAPDB_FALSE;	

	
	Tools_DynamicUTF8String sValue;
	getParameterValue(WQ_HEADERPAGE_TAG,req,sValue);

	if (sValue == WQ_LOGOFF_TAG) {

		bLogOffButton = SAPDB_TRUE;
		return SAPDB_TRUE;					
	}
	else if (sValue  == WQ_LOGON_TAG){

		bLogOffButton = SAPDB_FALSE;
		return SAPDB_TRUE;					
	}
	
	return SAPDB_FALSE;					
}

SAPDB_Bool isLogOffRequest (sapdbwa_HttpRequest  &req)
{
	const char		*queryString = req.GetQueryString();
	if (queryString == NULL) return SAPDB_FALSE;	

	if (strcmp(queryString,WQ_LOGOFF_TAG) != 0) return SAPDB_FALSE;						

	return SAPDB_TRUE;					
}

SAPDB_Bool isLogOnRequest (sapdbwa_HttpRequest  &req)
{
	const char		*queryString = req.GetQueryString();
	if (queryString == NULL) return SAPDB_FALSE;	

	//if (strcmp(queryString,WQ_LOGON_TAG) != 0) return SAPDB_FALSE;						
	if (strncmp(queryString,WQ_LOGON_TAG,strlen(WQ_LOGON_TAG)) != 0) return SAPDB_FALSE;						

	return SAPDB_TRUE;					
}

SAPDB_Bool isTreeButtonRequest (sapdbwa_HttpRequest  &req)
{
	const char		*queryString = req.GetQueryString();
	if (queryString == NULL) return SAPDB_FALSE;	

	if (strcmp(queryString,WQ_TREEBUTTON_PAGE) != 0) return SAPDB_FALSE;						

	return SAPDB_TRUE;					
}


SAPDB_Bool isLogonMainRequest (sapdbwa_HttpRequest  &req)
{
	const char		*queryString = req.GetQueryString();
	if (queryString == NULL) return SAPDB_FALSE;	

	if (strcmp(queryString,WQ_WQLOGONMAIN_TAG) != 0) return SAPDB_FALSE;						

	return SAPDB_TRUE;					
}

SAPDB_Bool isFrameRequest (sapdbwa_HttpRequest  &req)
{
	const char		*queryString = req.GetQueryString();
	if (queryString == NULL) return SAPDB_FALSE;	

	if (strcmp(queryString,WQ_FRAME_TAG) != 0) return SAPDB_FALSE;						

	return SAPDB_TRUE;					
}


SAPDB_Bool isTreePageRequest (sapdbwa_HttpRequest  &req)
{
	const char		*queryString = req.GetQueryString();
	if (queryString == NULL) return SAPDB_FALSE;	

	if (strcmp(queryString,WQ_TREEWINDOW_TAG) != 0) return SAPDB_FALSE;						

	return SAPDB_TRUE;					
}

SAPDB_Bool isZoomBackRequest (sapdbwa_HttpRequest  &req)
{
	const char		*queryString = req.GetQueryString();
	if (queryString == NULL) return SAPDB_FALSE;	

	if (strcmp(queryString,WQ_ZOOMBACK_TAG) != 0) return SAPDB_FALSE;						

	return SAPDB_TRUE;					
}

SAPDB_Bool isParamQueryRequest(sapdbwa_HttpRequest  &req){

	const char		*queryString = req.GetQueryString();
	if (queryString == NULL) return SAPDB_FALSE;	

	if (strcmp(queryString,WQ_NEXTPARAMBUTTON_TAG) != 0) return SAPDB_FALSE;						

	return SAPDB_TRUE;					

}

SAPDB_Bool isDSQLExecuteRequest (sapdbwa_HttpRequest  &req)
{
	const char		*queryString = req.GetQueryString();
	if (queryString == NULL) return SAPDB_FALSE;	
	
	Tools_DynamicUTF8String sValue; 
	getParameterValue(WQ_ACTIONTOTAKE,req,sValue); 

	if(sValue.Empty()) return SAPDB_FALSE;
	if(sValue == WQ_DSQLEXECUTE_TAG) return SAPDB_TRUE;					

	return SAPDB_FALSE;					
}

SAPDB_Bool isZoomRequest (sapdbwa_HttpRequest  &req)
{
	const char		*queryString = req.GetQueryString();
	if (queryString == NULL) return SAPDB_FALSE;	
	
	Tools_DynamicUTF8String sValue; 
	getParameterValue(WQ_ZOOM_ROW_TAG,req,sValue); 
	if(sValue.Empty()) return SAPDB_FALSE;

	getParameterValue(WQ_ZOOM_COL_TAG,req,sValue); 
	if(sValue.Empty()) return SAPDB_FALSE;

	return SAPDB_TRUE;					
}

SAPDB_Bool isStoredSQLStudioQueryRequest(sapdbwa_HttpRequest  &req){

	const char		*queryString = req.GetQueryString();
	if (queryString == NULL) return SAPDB_FALSE;	

	Tools_DynamicUTF8String sValue;
	getParameterValue(WQ_STOREDQUERY_TAG,req,sValue); 
	if(!sValue.Empty()) return SAPDB_TRUE;

	return SAPDB_FALSE;					
}

SAPDB_Bool isSelectResultRequest(sapdbwa_HttpRequest  &req){

	const char		*queryString = req.GetQueryString();
	if (queryString == NULL) return SAPDB_FALSE;	

	sapdbwa_StringSeq paramNames;	
	//now check params
	req.GetParameterNames(paramNames);

	Tools_DynamicUTF8String sValue;
	getParameterValue(WQ_RESULTS_TAG,req,sValue); 
	if(!sValue.Empty()) return SAPDB_TRUE;

	return SAPDB_FALSE;					
}

SAPDB_Bool isResultPageRequest (sapdbwa_HttpRequest  &req)
{
	const char		*queryString = req.GetQueryString();
	if (queryString == NULL) return SAPDB_FALSE;	

	if (strcmp(queryString,WQ_RESULTPAGE_TAG) != 0) return SAPDB_FALSE;						

	return SAPDB_TRUE;					
}

SAPDB_Bool isNavigateInResultFirstRequest (sapdbwa_HttpRequest  &req)
{
	const char		*queryString = req.GetQueryString();
	if (queryString == NULL) return SAPDB_FALSE;	

	if (strcmp(queryString,WQ_FIRSTBUTTON_TAG) != 0) return SAPDB_FALSE;						

	return SAPDB_TRUE;					
}

SAPDB_Bool isNavigateInResultNextRequest (sapdbwa_HttpRequest  &req)
{
	const char		*queryString = req.GetQueryString();
	if (queryString == NULL) return SAPDB_FALSE;	

	if (strcmp(queryString,WQ_NEXTBUTTON_TAG) != 0) return SAPDB_FALSE;						

	return SAPDB_TRUE;					
}


SAPDB_Bool isNavigateInResultPrevRequest (sapdbwa_HttpRequest  &req)
{
	const char		*queryString = req.GetQueryString();
	if (queryString == NULL) return SAPDB_FALSE;	

	if (strcmp(queryString,WQ_PREVBUTTON_TAG) != 0) return SAPDB_FALSE;						

	return SAPDB_TRUE;					
}

SAPDB_Bool isNavigateInResultLastRequest (sapdbwa_HttpRequest  &req)
{
	const char		*queryString = req.GetQueryString();
	if (queryString == NULL) return SAPDB_FALSE;	

	if (strcmp(queryString,WQ_LASTBUTTON_TAG) != 0) return SAPDB_FALSE;						

	return SAPDB_TRUE;					
}



SAPDB_Bool isNavigateInResultRequest (sapdbwa_HttpRequest  &req)
{	
	
	const char		*queryString = req.GetQueryString();

	if (queryString == NULL) return SAPDB_FALSE;	

	if (strcmp(queryString,WQ_FIRSTBUTTON_TAG) == 0 ||
		strcmp(queryString,WQ_PREVBUTTON_TAG) == 0 ||
		strcmp(queryString,WQ_NEXTBUTTON_TAG) == 0 ||
		strcmp(queryString,WQ_LASTBUTTON_TAG) == 0 )
		return SAPDB_TRUE;					
	

	return SAPDB_FALSE;					
}

SAPDB_Bool getParameterValue(const char *pParamName,
							 sapdbwa_HttpRequest &req,
							 Tools_DynamicUTF8String &sValue)
{
  if (pParamName == NULL) return SAPDB_FALSE;			
  
  sapdbwa_StringSeq paramValues;

  sValue.Erase();
  
  if (req.GetParameterValues( pParamName, paramValues )) {

	 if (paramValues[0] == NULL) return SAPDB_FALSE;					
	 //request is a utf8 pointer
	 sValue = (Tools_DynamicUTF8String::ConstPointer)paramValues[0];	
  }	

  return SAPDB_TRUE;  
}

SAPDB_Bool sendNewWebSessinID(sapdbwa_WebAgent &wa)
{
	sapdbwa_WebSession    webSession;

	// look for existing session
	if (wa.GetWebSession( webSession ) == sapdbwa_True) {
		
		//check if webSession is valid
		if (webSession.GetWebSession() != NULL){

			webSession.NewSessionId();	
			return SAPDB_TRUE;
		}
		else
			return SAPDB_FALSE;
	}
	return SAPDB_FALSE;
}







