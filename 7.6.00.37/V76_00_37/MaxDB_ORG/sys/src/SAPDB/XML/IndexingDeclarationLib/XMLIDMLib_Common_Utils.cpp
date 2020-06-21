/*!**********************************************************************

  module: XMLIDMLib_Common_Utils.c

  -----------------------------------------------------------------------


  responsible:  Daniel Kirmse (D027470 - Daniel.Kirmse@SAP.com)


  special area: WebDAV / XML Indexing

  description:  Helper functions

  see also:		/

  -----------------------------------------------------------------------

  copyright:                Copyright (c) 2000-2005 SAP AG

\if EMIT_LICENCE


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


\endif
************************************************************************/

	/*===================================================================
	// Includes
	//===================================================================*/
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Utils.hpp"

	/*-------------------------------------------------------------------
	// XMLIDMLIB_IsQueryStringValid
	//-------------------------------------------------------------------*/
	XMLIDMLIB_Bool XMLIDMLIB_IsQueryStringValid (sapdbwa_HttpRequestP req)
	{
		const XMLIDMLIB_Char *queryString = sapdbwa_GetQueryString(req);
		if (queryString == NULL) 
			return XMLIDMLIB_False;	
		else
			return XMLIDMLIB_True;					
	}

	void XMLIDMLIB_SendHTMLFile(sapdbwa_Handle    wa,
							    sapdbwa_HttpReplyP   rep,
					  			const Tools_DynamicUTF8String& sFile)
	{
		// send page headers
		sapdbwa_InitHeader(rep,sapdbwa_Status_Ok, "text/html", NULL, NULL,NULL, NULL );
		sapdbwa_SetHeader (rep,"Expires", "Sat, 01 Jan 2000 00:00:00 GMT");
		sapdbwa_SetHeader (rep,"charset","UTF-8");
		sapdbwa_SendHeader(rep);

		//NOT UNICODE BECAUSE OF WA
		XMLIDMLIB_Char	sAsciiFile[512];
		memset(sAsciiFile,0,512);	
		XMLIDMLIB_Char* szpConvert = NULL;

		Tools_DynamicUTF8String sCopyFile = sFile;
		assert(sCopyFile.ConvertToASCII_Latin1((char*)sAsciiFile,sAsciiFile+sizeof (sAsciiFile),szpConvert) == Tools_UTF8Basis::Success);	

		XMLIDMLIB_Char   szPath[256] = "";
		sapdbwa_GetDocumentRoot(wa, szPath, 200); 
		strcat(szPath,"/");
		strcat(szPath, SAPDBWA_DOCUMENT_ROOT_HTML);
		strcat(szPath,"/");
		strcat(szPath, sAsciiFile);

		XMLIDMLIB_Bool bRet = sapdbwa_SendTemplate(rep,szPath);		
		if(bRet == XMLIDMLIB_False) 
		{
			strcpy(sAsciiFile,"Error sending: ");
			strcat(sAsciiFile,szPath);
			sapdbwa_WriteLogMsg(wa,sAsciiFile);
		}
		
	}

	XMLIDMLIB_Bool XMLIDMLIB_IsPageRequest (sapdbwa_HttpRequestP  req, XMLIDMLIB_Char *page)
	{
		const XMLIDMLIB_Char *queryString = sapdbwa_GetQueryString(req);
		if (queryString == NULL) return XMLIDMLIB_False;	

		if (strncmp(queryString,page,strlen(page)) != 0) 
			return XMLIDMLIB_False;						
		else
			return XMLIDMLIB_True;					
	}

	XMLIDMLIB_Bool XMLIDMLIB_IsCommandRequest (sapdbwa_HttpRequestP  req)
	{
		const XMLIDMLIB_Char *queryString = sapdbwa_GetQueryString(req);
		if (queryString == NULL) return XMLIDMLIB_False;	

		if (strncmp(queryString,TEXT_CONSTANT_CMD_PREFIX,strlen(TEXT_CONSTANT_CMD_PREFIX)) != 0) 
			return XMLIDMLIB_False;						
		else
			return XMLIDMLIB_True;					
	}

	XMLIDMLIB_Bool XMLIDMLIB_IsCommandRequestXie (sapdbwa_HttpRequestP  req)
	{
		const XMLIDMLIB_Char *queryString = sapdbwa_GetQueryString(req);
		if (queryString == NULL) return XMLIDMLIB_False;	

		if (strncmp(queryString,TEXT_CONSTANT_CMD_XIE_PREFIX,strlen(TEXT_CONSTANT_CMD_XIE_PREFIX)) != 0) 
			return XMLIDMLIB_False;						
		else
			return XMLIDMLIB_True;					
	}

	XMLIDMLIB_Bool XMLIDMLIB_IsCommandRequestQuery (sapdbwa_HttpRequestP  req)
	{
		const XMLIDMLIB_Char *queryString = sapdbwa_GetQueryString(req);
		if (queryString == NULL) return XMLIDMLIB_False;	

		if (strncmp(queryString,TEXT_CONSTANT_CMD_QUERY_PREFIX,strlen(TEXT_CONSTANT_CMD_QUERY_PREFIX)) != 0) 
			return XMLIDMLIB_False;						
		else
			return XMLIDMLIB_True;					
	}

	XMLIDMLIB_Bool XMLIDMLIB_IsCommand(sapdbwa_HttpRequestP req, XMLIDMLIB_Char *cmdstring)
	{
		XMLIDMLIB_Char		__command[1024];

		XMLIDMLIB_GetParameterValueC("cmd",req,__command);
		if (strcmp(__command,cmdstring)==0) 
			return XMLIDMLIB_True;						
		else
			return XMLIDMLIB_False;					
	}

	XMLIDMLIB_Bool XMLIDMLIB_IsSubCommand(sapdbwa_HttpRequest &req, XMLIDMLIB_Char *cmdstring)
	{
		XMLIDMLIB_Char		__command[1024];

		XMLIDMLIB_GetParameterValue("scmd",req,__command);
		if (strcmp(__command,cmdstring)==0) 
			return XMLIDMLIB_True;						
		else
			return XMLIDMLIB_False;					
	}

	XMLIDMLIB_Bool XMLIDMLIB_GetParameterValue(const XMLIDMLIB_Char *pParamName,
								 	 		   sapdbwa_HttpRequest &req,
								 	 		   XMLIDMLIB_Char *sValue)
	{
		XMLIDMLIB_Bool		bReturn = false;
		sapdbwa_StringSeq	values;

		strcpy(sValue,"");
		if (req.GetParameterValues(pParamName, values) == sapdbwa_True) {
			if (values.GetNumElem() > 0) {
				strcpy(sValue,values[0]);
				bReturn = true;
			} 
		}
		return bReturn;
	}

	XMLIDMLIB_Bool XMLIDMLIB_GetParameterValueC(const XMLIDMLIB_Char *pParamName,
								 	 		    sapdbwa_HttpRequestP req,
								 	 		    XMLIDMLIB_Char *sValue)
	{
		sapdbwa_StringSeqP	__values = sapdbwa_CreateStringSeq();

		strcpy(sValue,"");
		if (sapdbwa_GetParameterValues(req, pParamName, __values) == sapdbwa_True) {
			if (sapdbwa_GetNumElem(__values) > 0) {
				strcpy(sValue,sapdbwa_GetStringByIndex(__values,0));
				return XMLIDMLIB_True;
			} 
		} else {
			return XMLIDMLIB_False;
		}
		return XMLIDMLIB_False;
	}

	XMLIDMLIB_Bool XMLIDMLIB_CreateConnUserData(XMLIDMLIB_ConnUserData *userdata)
	{
		XMLIDMLIB_Bool			__ok = XMLIDMLIB_False;
		XMLIDMLIB_ConnUserData	__newUserData = NULL;

		if ( !userdata ) {
			return XMLIDMLIB_False;
		}
    
		sapdbwa_SQLALLOCAT( sizeof(struct st_conn_user_data), (SAPDB_UInt1**) &__newUserData, &__ok );
		if (__ok == XMLIDMLIB_True) {
			*userdata = __newUserData;
			return XMLIDMLIB_True;
		} else {
			*userdata = NULL;
			return XMLIDMLIB_False;
		}
	}

	XMLIDMLIB_Bool XMLIDMLIB_DestroyConnUserData(void *userdata)
	{
		if ( !userdata ) {
			return XMLIDMLIB_False;
		}
		XMLIMAPI_destroyHandle(((XMLIDMLIB_ConnUserData)userdata)->xmlHandle);
		sapdbwa_SQLFREE((SAPDB_UInt1*)userdata);
		return XMLIDMLIB_True;
	}

	void XMLIDMLIB_SendPageHeaders(sapdbwa_HttpReplyP rep, XMLIDMLIB_Char *mimetype, XMLIDMLIB_Char *expires, XMLIDMLIB_Char *charset) 
	{
		// send page headers
		sapdbwa_InitHeader(rep,sapdbwa_Status_Ok, mimetype , NULL, NULL,NULL, NULL );
		sapdbwa_SetHeader (rep,"Expires", expires);
		sapdbwa_SetHeader (rep,"charset", charset);
		sapdbwa_SendHeader(rep);
	}

	void XMLIDMLIB_strMaxCopy(char    *destination,
		 			char    *source,
					size_t   sizeToCopy )
	{

		strncpy(destination, source, sizeToCopy);
		destination[sizeToCopy-1] = '\0';

	}

	void XMLIDMLIB_EscapeQuotes(XMLIDMLIB_Char *oldstring, XMLIDMLIB_Int4 len, XMLIDMLIB_Char *newstring)
	{
		XMLIDMLIB_Int4 posnew;
		posnew = 0;
		for (int i=0;i<=len;i++) {
			if (oldstring[i]=='"') {
				newstring[posnew++] = '&';
				newstring[posnew++] = 'q';
				newstring[posnew++] = 'u';
				newstring[posnew++] = 'o';
				newstring[posnew++] = 't';
				newstring[posnew++] = ';';
			} else if (oldstring[i]=='&') {
				newstring[posnew++] = '&';
				newstring[posnew++] = 'a';
				newstring[posnew++] = 'm';
				newstring[posnew++] = 'p';
				newstring[posnew++] = ';';
			} else if (oldstring[i]=='°') {
				newstring[posnew++] = '&';
				newstring[posnew++] = '#';
				newstring[posnew++] = '1';
				newstring[posnew++] = '8';
				newstring[posnew++] = '6';
				newstring[posnew++] = ';';
			} else if (oldstring[i]=='²') {
				newstring[posnew++] = '&';
				newstring[posnew++] = 's';
				newstring[posnew++] = 'u';
				newstring[posnew++] = 'p';
				newstring[posnew++] = '2';
				newstring[posnew++] = ';';
			} else if (oldstring[i]=='³') {
				newstring[posnew++] = '&';
				newstring[posnew++] = 's';
				newstring[posnew++] = 'u';
				newstring[posnew++] = 'p';
				newstring[posnew++] = '3';
				newstring[posnew++] = ';';
			} else if (oldstring[i]=='´') {
				newstring[posnew++] = '&';
				newstring[posnew++] = 'a';
				newstring[posnew++] = 'c';
				newstring[posnew++] = 'u';
				newstring[posnew++] = 't';
				newstring[posnew++] = 'e';
				newstring[posnew++] = ';';
			} else if (oldstring[i]=='µ') {
				newstring[posnew++] = '&';
				newstring[posnew++] = 'm';
				newstring[posnew++] = 'i';
				newstring[posnew++] = 'c';
				newstring[posnew++] = 'r';
				newstring[posnew++] = 'o';
				newstring[posnew++] = ';';
			} else if (oldstring[i]=='€') {
				newstring[posnew++] = '&';
				newstring[posnew++] = '#';
				newstring[posnew++] = '8';
				newstring[posnew++] = '3';
				newstring[posnew++] = '6';
				newstring[posnew++] = '4';
				newstring[posnew++] = ';';
			} else if (oldstring[i]=='§') {
				newstring[posnew++] = '&';
				newstring[posnew++] = '#';
				newstring[posnew++] = '1';
				newstring[posnew++] = '6';
				newstring[posnew++] = '7';
				newstring[posnew++] = ';';
/*			} else if (oldstring[i]==' ') {
				newstring[posnew++] = '&';
				newstring[posnew++] = 'n';
				newstring[posnew++] = 'b';
				newstring[posnew++] = 's';
				newstring[posnew++] = 'p';
				newstring[posnew++] = ';';
*/			} else if (oldstring[i]=='ä') {
				newstring[posnew++] = '&';
				newstring[posnew++] = 'a';
				newstring[posnew++] = 'u';
				newstring[posnew++] = 'm';
				newstring[posnew++] = 'l';
				newstring[posnew++] = ';';
			} else if (oldstring[i]=='Ä') {
				newstring[posnew++] = '&';
				newstring[posnew++] = 'A';
				newstring[posnew++] = 'u';
				newstring[posnew++] = 'm';
				newstring[posnew++] = 'l';
				newstring[posnew++] = ';';
			} else if (oldstring[i]=='ö') {
				newstring[posnew++] = '&';
				newstring[posnew++] = 'o';
				newstring[posnew++] = 'u';
				newstring[posnew++] = 'm';
				newstring[posnew++] = 'l';
				newstring[posnew++] = ';';
			} else if (oldstring[i]=='Ö') {
				newstring[posnew++] = '&';
				newstring[posnew++] = 'O';
				newstring[posnew++] = 'u';
				newstring[posnew++] = 'm';
				newstring[posnew++] = 'l';
				newstring[posnew++] = ';';
			} else if (oldstring[i]=='ü') {
				newstring[posnew++] = '&';
				newstring[posnew++] = 'u';
				newstring[posnew++] = 'u';
				newstring[posnew++] = 'm';
				newstring[posnew++] = 'l';
				newstring[posnew++] = ';';
			} else if (oldstring[i]=='Ü') {
				newstring[posnew++] = '&';
				newstring[posnew++] = 'U';
				newstring[posnew++] = 'u';
				newstring[posnew++] = 'm';
				newstring[posnew++] = 'l';
				newstring[posnew++] = ';';
			} else if (oldstring[i]=='ß') {
				newstring[posnew++] = '&';
				newstring[posnew++] = 's';
				newstring[posnew++] = 'z';
				newstring[posnew++] = 'l';
				newstring[posnew++] = 'i';
				newstring[posnew++] = 'g';
				newstring[posnew++] = ';';
			} else {
				newstring[posnew++] = oldstring[i];
			}
		}
		newstring[posnew] = '\0';
	}
	

