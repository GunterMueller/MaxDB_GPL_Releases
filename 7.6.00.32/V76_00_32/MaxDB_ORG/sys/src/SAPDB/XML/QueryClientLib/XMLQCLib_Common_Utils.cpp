/*!**********************************************************************

  module: XMLQCLib_Common_Utils.c

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
	#include "SAPDB/XML/QueryClientLib/XMLQCLib_Common_Utils.hpp"

	/*-------------------------------------------------------------------
	// XMLQCLIB_IsQueryStringValid
	//-------------------------------------------------------------------*/
	XMLQCLIB_Bool XMLQCLIB_IsQueryStringValid (sapdbwa_HttpRequestP req)
	{
		const XMLQCLIB_Char *queryString = sapdbwa_GetQueryString(req);
		if (queryString == NULL) 
			return XMLQCLIB_False;	
		else
			return XMLQCLIB_True;					
	}

	void XMLQCLIB_SendHTMLFile(sapdbwa_Handle    wa,
							    sapdbwa_HttpReplyP   rep,
					  			const Tools_DynamicUTF8String& sFile)
	{
		// send page headers
		sapdbwa_InitHeader(rep,sapdbwa_Status_Ok, "text/html", NULL, NULL,NULL, NULL );
		sapdbwa_SetHeader (rep,"Expires", "Sat, 01 Jan 2000 00:00:00 GMT");
		sapdbwa_SetHeader (rep,"charset","UTF-8");
		sapdbwa_SendHeader(rep);

		//NOT UNICODE BECAUSE OF WA
		XMLQCLIB_Char	sAsciiFile[512];
		memset(sAsciiFile,0,512);	
		XMLQCLIB_Char* szpConvert = NULL;

		Tools_DynamicUTF8String sCopyFile = sFile;
		assert(sCopyFile.ConvertToASCII_Latin1((char*)sAsciiFile,sAsciiFile+sizeof (sAsciiFile),szpConvert) == Tools_UTF8Basis::Success);	

		XMLQCLIB_Char   szPath[256] = "";
		sapdbwa_GetDocumentRoot(wa, szPath, 200); 
		strcat(szPath,"/");
		strcat(szPath, SAPDBWA_DOCUMENT_ROOT_HTML);
		strcat(szPath,"/");
		strcat(szPath, sAsciiFile);

		XMLQCLIB_Bool bRet = sapdbwa_SendTemplate(rep,szPath);		
		if(bRet == XMLQCLIB_False) 
		{
			strcpy(sAsciiFile,"Error sending: ");
			strcat(sAsciiFile,szPath);
			sapdbwa_WriteLogMsg(wa,sAsciiFile);
		}
		
	}

	XMLQCLIB_Bool XMLQCLIB_IsPageRequest (sapdbwa_HttpRequestP  req, XMLQCLIB_Char *page)
	{
		const XMLQCLIB_Char *queryString = sapdbwa_GetQueryString(req);
		if (queryString == NULL) return XMLQCLIB_False;	

		if (strncmp(queryString,page,strlen(page)) != 0) 
			return XMLQCLIB_False;						
		else
			return XMLQCLIB_True;					
	}

	XMLQCLIB_Bool XMLQCLIB_IsCommandRequest (sapdbwa_HttpRequestP  req)
	{
		const XMLQCLIB_Char *queryString = sapdbwa_GetQueryString(req);
		if (queryString == NULL) return XMLQCLIB_False;	

		if (strncmp(queryString,TEXT_CONSTANT_QCL_CMD_PREFIX,strlen(TEXT_CONSTANT_QCL_CMD_PREFIX)) != 0) 
			return XMLQCLIB_False;						
		else
			return XMLQCLIB_True;					
	}

	XMLQCLIB_Bool XMLQCLIB_IsCommand(sapdbwa_HttpRequestP req, XMLQCLIB_Char *cmdstring)
	{
		XMLQCLIB_Char		__command[1024];

		XMLQCLIB_GetParameterValueC("cmd",req,__command);
		if (strcmp(__command,cmdstring)==0) 
			return XMLQCLIB_True;						
		else
			return XMLQCLIB_False;					
	}

	XMLQCLIB_Bool XMLQCLIB_IsSubCommand(sapdbwa_HttpRequest &req, XMLQCLIB_Char *cmdstring)
	{
		XMLQCLIB_Char		__command[1024];

		XMLQCLIB_GetParameterValue("scmd",req,__command);
		if (strcmp(__command,cmdstring)==0) 
			return XMLQCLIB_True;						
		else
			return XMLQCLIB_False;					
	}

	XMLQCLIB_Bool XMLQCLIB_GetParameterValue(const XMLQCLIB_Char *pParamName,
								 	 		   sapdbwa_HttpRequest &req,
								 	 		   XMLQCLIB_Char *sValue)
	{
		XMLQCLIB_Bool		bReturn = false;
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

	XMLQCLIB_Bool XMLQCLIB_GetParameterValueC(const XMLQCLIB_Char *pParamName,
								 	 		    sapdbwa_HttpRequestP req,
								 	 		    XMLQCLIB_Char *sValue)
	{
		sapdbwa_StringSeqP	__values = sapdbwa_CreateStringSeq();

		strcpy(sValue,"");
		if (sapdbwa_GetParameterValues(req, pParamName, __values) == sapdbwa_True) {
			if (sapdbwa_GetNumElem(__values) > 0) {
				strcpy(sValue,sapdbwa_GetStringByIndex(__values,0));
				return XMLQCLIB_True;
			} 
		} else {
			return XMLQCLIB_False;
		}
		return XMLQCLIB_False;
	}

	XMLQCLIB_Bool XMLQCLIB_CreateConnUserData(XMLQCLIB_ConnUserData *userdata)
	{
		XMLQCLIB_Bool			__ok = XMLQCLIB_False;
		XMLQCLIB_ConnUserData	__newUserData = NULL;

		if ( !userdata ) {
			return XMLQCLIB_False;
		}
    
		sapdbwa_SQLALLOCAT( sizeof(struct st_xmlqclib_conn_user_data), (SAPDB_UInt1**) &__newUserData, &__ok );
		if (__ok == XMLQCLIB_True) {
			*userdata = __newUserData;
			return XMLQCLIB_True;
		} else {
			*userdata = NULL;
			return XMLQCLIB_False;
		}
	}

	XMLQCLIB_Bool XMLQCLIB_DestroyConnUserData(void *userdata)
	{
		if ( !userdata ) {
			return XMLQCLIB_False;
		}
		XMLIMAPI_destroyHandle(((XMLQCLIB_ConnUserData)userdata)->xmlHandle);
		sapdbwa_SQLFREE((SAPDB_UInt1*)userdata);
		return XMLQCLIB_True;
	}

	void XMLQCLIB_SendPageHeaders(sapdbwa_HttpReplyP rep, XMLQCLIB_Char *mimetype, XMLQCLIB_Char *expires, XMLQCLIB_Char *charset) 
	{
		// send page headers
		sapdbwa_InitHeader(rep,sapdbwa_Status_Ok, mimetype , NULL, NULL,NULL, NULL );
		sapdbwa_SetHeader (rep,"Expires", expires);
		sapdbwa_SetHeader (rep,"charset", charset);
		sapdbwa_SendHeader(rep);
	}

	XMLQCLIB_Bool XMLQCLIB_QueryParamsIdxListCreate(XMLQCLIB_QueryParamsIdxList *list)
	{
		XMLQCLIB_Bool				ok = XMLQCLIB_False;
		XMLQCLIB_QueryParamsIdxList	newList = NULL;

		if ( !list ) {
			return XMLQCLIB_False;
		}
    
		sqlallocat( sizeof(struct st_xmlqclib_idx_query_params_list), (XMLQCLIB_UInt1**) &newList, &ok );
		if (ok == XMLQCLIB_True) {

			newList->first = NULL;
			newList->last = NULL;
			newList->curr = NULL;
			*list = newList;

			return XMLQCLIB_True;
		} else {
			*list = NULL;

			return XMLQCLIB_False;
		}

	}

	XMLQCLIB_Bool XMLQCLIB_QueryParamsIdxListDestroy(XMLQCLIB_QueryParamsIdxList list)
	{
		XMLQCLIB_QueryParamsIdxListItem	current,next;
		
		if ( !list ) {
			return XMLQCLIB_False;
		}
    
		/* track down the list to destroy every single item */
		next = list->first;
		while (next) {
			current = next;
			next = current->next;
			sqlfree((XMLQCLIB_UInt1*)current->item);
			sqlfree((XMLQCLIB_UInt1*)current);
		}

		sqlfree((XMLQCLIB_UInt1*)list);
		return XMLQCLIB_True;
	}

	XMLQCLIB_Bool XMLQCLIB_QueryParamsIdxListInit(XMLQCLIB_QueryParamsIdxList list)
	{
		XMLQCLIB_QueryParamsIdxListItem	current,next;
		
		if ( !list ) {
			return XMLQCLIB_False;
		}
    
		/* track down the list to destroy every single item */
		next = list->first;
		while (next) {
			current = next;
			next = current->next;
			sqlfree((XMLQCLIB_UInt1*)current->item);
			sqlfree((XMLQCLIB_UInt1*)current);
		}

		list->first = NULL;
		list->last = NULL;
		list->curr = NULL;

		return XMLQCLIB_True;
	}

	XMLQCLIB_Bool XMLQCLIB_QueryParamsIdxListAddItem(XMLQCLIB_QueryParamsIdxList	list, 
														XMLQCLIB_QueryParamsIdx	*param)
	{
		XMLQCLIB_QueryParamsIdxListItem	newListItem;
		XMLQCLIB_QueryParamsIdx			*newItem;
		XMLQCLIB_Bool						ok = XMLQCLIB_False;
		
		if (!list) {
			return XMLQCLIB_False;
		}
    
		sqlallocat( sizeof(struct st_xmlqclib_idx_query_params_list_item), (XMLQCLIB_UInt1**) &newListItem, &ok );
		if (ok == XMLQCLIB_True) {

			sqlallocat( sizeof(struct st_xmlqclib_idx_query_params), (XMLQCLIB_UInt1**) &newItem, &ok );
			if (ok == XMLQCLIB_True) {

				XMLQCLIB_strMaxCopy(newItem->xmlIndexId,param->xmlIndexId,49);
				XMLQCLIB_strMaxCopy(newItem->xmlIndexName,param->xmlIndexName,512);
				XMLQCLIB_strMaxCopy(newItem->genId,param->genId,49);
				newItem->valueCount = param->valueCount;
				newItem->idxValQueryParams[0].compOperator = param->idxValQueryParams[0].compOperator;
				newItem->idxValQueryParams[1].compOperator = param->idxValQueryParams[1].compOperator;
				newItem->idxValQueryParams[2].compOperator = param->idxValQueryParams[2].compOperator;
				newItem->idxValQueryParams[3].compOperator = param->idxValQueryParams[3].compOperator;
				newItem->idxValQueryParams[4].compOperator = param->idxValQueryParams[4].compOperator;
				XMLQCLIB_strMaxCopy(newItem->idxValQueryParams[0].input,param->idxValQueryParams[0].input,512);
				XMLQCLIB_strMaxCopy(newItem->idxValQueryParams[1].input,param->idxValQueryParams[1].input,512);
				XMLQCLIB_strMaxCopy(newItem->idxValQueryParams[2].input,param->idxValQueryParams[2].input,512);
				XMLQCLIB_strMaxCopy(newItem->idxValQueryParams[3].input,param->idxValQueryParams[3].input,512);
				XMLQCLIB_strMaxCopy(newItem->idxValQueryParams[4].input,param->idxValQueryParams[4].input,512);

				newListItem->item = newItem;
			} else {
				return XMLQCLIB_False;
			}

			newListItem->next = NULL;
			if (!list->first) {
				/* first item */
				list->first = newListItem;
				list->last = newListItem;
			} else {
				/* all other items */
				list->last->next = newListItem;
				list->last = newListItem;
			}

			return XMLQCLIB_True;
		} else {

			return XMLQCLIB_False;
		}

	}

	XMLQCLIB_Bool XMLQCLIB_QueryParamsIdxListGetFirst(XMLQCLIB_QueryParamsIdxList	list, 
														XMLQCLIB_QueryParamsIdx		**param)
	{
		if(!param)
			return XMLQCLIB_False;
		if(!list->first)
			return XMLQCLIB_False;
		*param = list->first->item;
		list->curr = list->first;
		return XMLQCLIB_True;
	}

	XMLQCLIB_Bool XMLQCLIB_QueryParamsIdxListGetNext(XMLQCLIB_QueryParamsIdxList		list, 
													   XMLQCLIB_QueryParamsIdx			**param)
	{
		if(!param)
			return XMLQCLIB_False;
		if(!list->first)
			return XMLQCLIB_False;
		if(!list->curr)
			return XMLQCLIB_False;
		list->curr = list->curr->next;
		if(!list->curr)
			return XMLQCLIB_False;
		*param = list->curr->item;
		return XMLQCLIB_True;

	}

	XMLQCLIB_Bool XMLQCLIB_QueryParamsIdxHasValidLine(XMLQCLIB_QueryParamsIdxList	list)
	{
		XMLQCLIB_QueryParamsIdx	*__params;
		if (XMLQCLIB_QueryParamsIdxListGetFirst(list,&__params)) {
			do {
				if (strcmp(__params->xmlIndexName,"")!=0) {
					for (int i=0;i<5;i++) {
						if (strcmp(__params->idxValQueryParams[i].input,"")!=0)
							return XMLQCLIB_True;
					}
					return XMLQCLIB_False;
				} else {
					return XMLQCLIB_False;
				}
			} while(XMLQCLIB_QueryParamsIdxListGetNext(list,&__params));
		} else {
			return XMLQCLIB_False;
		}
	}

	XMLQCLIB_Bool XMLQCLIB_QueryParamsListCreate(XMLQCLIB_QueryParamsList *list)
	{
		XMLQCLIB_Bool				ok = XMLQCLIB_False;
		XMLQCLIB_QueryParamsList	newList = NULL;

		if ( !list ) {
			return XMLQCLIB_False;
		}
    
		sqlallocat( sizeof(struct st_xmlqclib_query_params_list), (XMLQCLIB_UInt1**) &newList, &ok );
		if (ok == XMLQCLIB_True) {

			newList->first = NULL;
			newList->last = NULL;
			newList->curr = NULL;
			newList->count = 0;
			*list = newList;

			return XMLQCLIB_True;
		} else {
			*list = NULL;

			return XMLQCLIB_False;
		}

	}

	XMLQCLIB_Bool XMLQCLIB_QueryParamsListDestroy(XMLQCLIB_QueryParamsList list)
	{
		XMLQCLIB_QueryParamsListItem	current,next;
		
		if ( !list ) {
			return XMLQCLIB_False;
		}
    
		/* track down the list to destroy every single item */
		next = list->first;
		while (next) {
			current = next;
			next = current->next;
			XMLQCLIB_QueryParamsIdxListDestroy(current->item->idxQueryParams);
			XMLIMAPI_XmlIndexListDestroy(current->item->idxList);
			sqlfree((XMLQCLIB_UInt1*)current);
		}

		sqlfree((XMLQCLIB_UInt1*)list);
		return XMLQCLIB_True;
	}

	XMLQCLIB_Bool XMLQCLIB_QueryParamsListInit(XMLQCLIB_QueryParamsList list)
	{
		XMLQCLIB_QueryParamsListItem	current,next;
		
		if ( !list ) {
			return XMLQCLIB_False;
		}
    
		/* track down the list to destroy every single item */
		next = list->first;
		while (next) {
			current = next;
			next = current->next;
			XMLQCLIB_QueryParamsIdxListDestroy(current->item->idxQueryParams);
			XMLIMAPI_XmlIndexListDestroy(current->item->idxList);
			sqlfree((XMLQCLIB_UInt1*)current);
		}

		list->first = NULL;
		list->last = NULL;
		list->curr = NULL;
		list->count = NULL;

		return XMLQCLIB_True;
	}

	XMLQCLIB_Bool XMLQCLIB_QueryParamsListAddItem(XMLQCLIB_QueryParamsList	list, 
													XMLQCLIB_QueryParams	*param)
	{
		XMLQCLIB_QueryParamsListItem	newListItem;
		XMLQCLIB_QueryParams			*newItem;
		XMLQCLIB_Bool					ok = XMLQCLIB_False;
		
		if (!list) {
			return XMLQCLIB_False;
		}
    
		sqlallocat( sizeof(struct st_xmlqclib_query_params_list_item), (XMLQCLIB_UInt1**) &newListItem, &ok );
		if (ok == XMLQCLIB_True) {

			sqlallocat( sizeof(struct st_xmlqclib_query_params), (XMLQCLIB_UInt1**) &newItem, &ok );
			if (ok == XMLQCLIB_True) {

				XMLQCLIB_strMaxCopy(newItem->docClassId,param->docClassId,49);
				XMLQCLIB_strMaxCopy(newItem->docClassName,param->docClassName,513);
				XMLQCLIB_strMaxCopy(newItem->genId,param->genId,49);
				newItem->idxLines = param->idxLines;
				newItem->idxList = param->idxList;
				newItem->idxQueryParams = param->idxQueryParams;
				newItem->idxListLength = param->idxListLength;

				newListItem->item = newItem;
			} else {
				return XMLQCLIB_False;
			}

			newListItem->next = NULL;
			if (!list->first) {
				/* first item */
				list->first = newListItem;
				list->last = newListItem;
			} else {
				/* all other items */
				list->last->next = newListItem;
				list->last = newListItem;
			}
			list->count++;

			return XMLQCLIB_True;
		} else {

			return XMLQCLIB_False;
		}

	}

	XMLQCLIB_Bool XMLQCLIB_QueryParamsListGetFirst(XMLQCLIB_QueryParamsList	list, 
													 XMLQCLIB_QueryParams		**param)
	{
		if(!param)
			return XMLQCLIB_False;
		if(!list->first)
			return XMLQCLIB_False;
		*param = list->first->item;
		list->curr = list->first;
		return XMLQCLIB_True;
	}

	XMLQCLIB_Bool XMLQCLIB_QueryParamsListGetNext(XMLQCLIB_QueryParamsList		list, 
												    XMLQCLIB_QueryParams			**param)
	{
		if(!param)
			return XMLQCLIB_False;
		if(!list->first)
			return XMLQCLIB_False;
		if(!list->curr)
			return XMLQCLIB_False;
		list->curr = list->curr->next;
		if(!list->curr)
			return XMLQCLIB_False;
		*param = list->curr->item;
		return XMLQCLIB_True;

	}

	XMLQCLIB_UInt4 XMLQCLIB_QueryParamsListGetLength(XMLQCLIB_QueryParamsList	list)
	{
		return list->count;
	}

	void XMLQCLIB_strMaxCopy(char    *destination,
		 			char    *source,
					size_t   sizeToCopy )
	{

		strncpy(destination, source, sizeToCopy);
		destination[sizeToCopy-1] = '\0';

	}

	XMLQCLIB_Bool XMLQCLIB_BuildWhereClause(XMLQCLIB_QueryParamsList params, XMLQSLib_WhereClause *where)
	{
		XMLQCLIB_Bool			__ok;
		XMLQCLIB_QueryParams	*__queryParams;
		XMLQCLIB_QueryParamsIdx	*__queryParamsIdx;
		XMLQSLib_WhereRow		__firstIdxWhereRow;
		XMLQSLib_WhereRow		__lastIdxWhereRow;
		XMLQSLib_WhereRow		__currIdxWhereRow;
		XMLQCLIB_Bool			__firstIdx;
		XMLQCLIB_Bool			__firstVal;
		XMLQSLib_WhereClause	__newWhere=NULL;

		// convert the query parameters list into an where clause
		if (where == NULL)
			return XMLQCLIB_False;

		// create the where clause
		sqlallocat( sizeof(struct st_xmlqslib_where_clause), (XMLQCLIB_UInt1**) &__newWhere, &__ok );
		if (__ok == XMLQCLIB_True) {
			__newWhere->row = NULL;
		} else {
			return XMLQCLIB_False;
		}
		__firstIdxWhereRow = NULL;
		__lastIdxWhereRow = NULL;
		__currIdxWhereRow = NULL;
		if (XMLQCLIB_QueryParamsListGetFirst(params,&__queryParams)) {
			do {
				if (strcmp(__queryParams->docClassName,"")!=0) {
					// create a where clause row for the document class 
					sqlallocat( sizeof(struct st_xmlqslib_where_row), (XMLQCLIB_UInt1**) &__currIdxWhereRow, &__ok );
					if (__ok == XMLQCLIB_True) {
						__currIdxWhereRow->nextRow = NULL;
					} else {
						return XMLQCLIB_False;
					}
					if (__firstIdxWhereRow == NULL) {
						__currIdxWhereRow->logOperator = XMLQSLIB_LOGOPERATOR_NOOP;
						__firstIdxWhereRow = __currIdxWhereRow;
						__newWhere->row = __firstIdxWhereRow;
					} else {
						__currIdxWhereRow->logOperator = XMLQSLIB_LOGOPERATOR_OR;
					}
					__currIdxWhereRow->openParenthesis = 1;
					__currIdxWhereRow->idxObject = XMLQSLIB_IDXOBJECT_DOCCLASS;
					strcpy(__currIdxWhereRow->name,__queryParams->docClassId);
					strcpy(__currIdxWhereRow->value,__queryParams->docClassId);
					__currIdxWhereRow->compareOperator = XMLQSLIB_EQUAL;
					if (XMLQCLIB_QueryParamsIdxHasValidLine(__queryParams->idxQueryParams)) {
						__currIdxWhereRow->closeParenthesis = 0;
					} else {
						__currIdxWhereRow->closeParenthesis = 1;
					}
					if (__lastIdxWhereRow == NULL) {
						__lastIdxWhereRow = __firstIdxWhereRow;
					} else {
						__lastIdxWhereRow->nextRow = __currIdxWhereRow;
						__lastIdxWhereRow = __currIdxWhereRow;
					}
				}
				if (XMLQCLIB_QueryParamsIdxHasValidLine(__queryParams->idxQueryParams)) {
					__firstIdx = XMLQCLIB_True;
					if (XMLQCLIB_QueryParamsIdxListGetFirst(__queryParams->idxQueryParams,&__queryParamsIdx)) {
						do {
							if (strcmp(__queryParamsIdx->xmlIndexName,"")!=0) {
								__firstVal = XMLQCLIB_True;
								for (int i=0;i<5;i++) {
									if (strcmp(__queryParamsIdx->idxValQueryParams[i].input,"")!=0) {
										// create a where clause row for the document class 
										sqlallocat( sizeof(struct st_xmlqslib_where_row), (XMLQCLIB_UInt1**) &__currIdxWhereRow, &__ok );
										if (__ok == XMLQCLIB_True) {
											__currIdxWhereRow->nextRow = NULL;
										} else {
											return XMLQCLIB_False;
										}
										if (__firstIdxWhereRow==NULL) {
											__currIdxWhereRow->logOperator = XMLQSLIB_LOGOPERATOR_NOOP;
											__firstIdxWhereRow = __currIdxWhereRow;
											__newWhere->row = __firstIdxWhereRow;
										} else {
											if (__firstVal) {
												__currIdxWhereRow->logOperator = XMLQSLIB_LOGOPERATOR_AND;
												__firstVal = XMLQCLIB_False;
											} else {
												__currIdxWhereRow->logOperator = XMLQSLIB_LOGOPERATOR_OR;
											}
										}
										if (__firstIdx) {
											__currIdxWhereRow->openParenthesis = 2;
											__firstIdx = XMLQCLIB_False;
										} else {
											if (__firstVal || __currIdxWhereRow->logOperator == XMLQSLIB_LOGOPERATOR_AND)
												__currIdxWhereRow->openParenthesis = 1;
											else 
												__currIdxWhereRow->openParenthesis = 0;
										}
										__currIdxWhereRow->idxObject = XMLQSLIB_IDXOBJECT_XMLINDEX;
										strcpy(__currIdxWhereRow->name,__queryParamsIdx->xmlIndexName);
										strcpy(__currIdxWhereRow->value,__queryParamsIdx->idxValQueryParams[i].input);
										__currIdxWhereRow->compareOperator = __queryParamsIdx->idxValQueryParams[i].compOperator;
										__currIdxWhereRow->closeParenthesis = 0;
										if (__lastIdxWhereRow == NULL) {
											__lastIdxWhereRow = __firstIdxWhereRow;
										} else {
											__lastIdxWhereRow->nextRow = __currIdxWhereRow;
											__lastIdxWhereRow = __currIdxWhereRow;
										}
									}
								}
								if (!__firstVal) {
									__currIdxWhereRow->closeParenthesis = 1;
								}
							}
						} while (XMLQCLIB_QueryParamsIdxListGetNext(__queryParams->idxQueryParams,&__queryParamsIdx));
						if (!__firstIdx) {
							__currIdxWhereRow->closeParenthesis = 3;
						}
					}
				}
			} while(XMLQCLIB_QueryParamsListGetNext(params,&__queryParams));
			*where = __newWhere;
		}
		/* test output */
		/* XMLQSLib_WhereRow		__row = __newWhere->row;
		while (__row) {
			if (__row->logOperator == XMLQSLIB_LOGOPERATOR_NOOP)
				printf("\n\n    ");
			else if (__row->logOperator == XMLQSLIB_LOGOPERATOR_AND)
				printf("\n\nAND ");
			else if (__row->logOperator == XMLQSLIB_LOGOPERATOR_OR)
				printf("\n\nOR  ");
			else
				printf("\n    ");
			for (int i=1;i<=__row->openParenthesis;i++)
				printf("(");
			if (__row->idxObject == XMLQSLIB_IDXOBJECT_DOCCLASS)
				printf(" DOCCLASSNAME ");
			else if (__row->idxObject == XMLQSLIB_IDXOBJECT_XMLINDEX)
				printf(" %s ",__row->name);
			else
				printf(" ");
			if (__row->compareOperator == XMLQSLIB_EQUAL)
				printf("= ");
			else if (__row->compareOperator == XMLQSLIB_LESS)
				printf("< ");
			else if (__row->compareOperator == XMLQSLIB_GREATER)
				printf("> ");
			else if (__row->compareOperator == XMLQSLIB_LESS_EQUAL)
				printf("<=");
			else if (__row->compareOperator == XMLQSLIB_GREATER_EQUAL)
				printf(">=");
			else if (__row->compareOperator == XMLQSLIB_NOT_EQUAL)
				printf("<>");
			else 
				printf("  ");
			printf("%s ",__row->value);
			for (int k=1;k<=__row->closeParenthesis;k++)
				printf(")");
			__row = __row->nextRow;
		}*/
		return XMLQCLIB_False;
	}

	void XMLQCLIB_EscapeQuotes(XMLQCLIB_Char *oldstring, XMLQCLIB_Int4 len, XMLQCLIB_Char *newstring)
	{
		XMLQCLIB_Int4 posnew;
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
	
		/*------------------------------------------------------------------------
		Function:	buildServerStringLong
	--------------------------------------------------------------------------*/
	void XMLQCLIB_BuildServerStringLong(sapdbwa_WebAgent		&wa,
								 	    sapdbwa_HttpRequest &request,
									    XMLQCLIB_Char *server,
										XMLQCLIB_Int2	length) 
	{
		XMLQCLIB_Char				*host = NULL;
		XMLQCLIB_Char				*port = NULL;
		XMLQCLIB_Char				prefix[1001];

	
		host = (char*)request.GetHeader("HTTP_HOST" );
		port = (char*)request.GetHeader("HTTP_PORT" );
		/* set server prefix */
		strcpy(prefix,wa.GetServiceName());

		if (port==NULL) {
			sp77sprintf(server,length,"http://%s/%s",host,prefix);
		} else {
			sp77sprintf(server,length,"http://%s:%s/%s",host,port,prefix);
		}
	}

