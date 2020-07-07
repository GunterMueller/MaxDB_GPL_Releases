/****************************************************************************

  module      : WDVHandler_XmlParser.c

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-03-14  12:59
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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


*****************************************************************************/


	#include "SAPDB/WebDAV/Handler/WDVHandler_XmlParser.h"


	/*------------------------------------------------------------------------
		Function:	xmlParserCheckEmptyTag
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserCheckEmptyTag(XML_Parser p, WDVH_XmlUserData data, WDVH_XmlEmptyTag tag)
	{
		WDVH_Char	*temp1,*temp2,*temp3,*temp4;
		WDVH_Char	emptytag[WDVH_MAX_XML_TAG_LEN+1];
		WDVH_Char   temptag[WDVH_MAX_XML_TAG_LEN+1];
		WDVH_Int4	pos1,pos2,pos3,pos4;

		/* pointer to start of current tag */
		pos1 = XML_GetCurrentByteIndex(p);
		/* if start of current tag is located in the previous handled buffer chunk,
		   the tag has to be build of two chunks one located in previous and one located in
		   current buffer */
		pos1 = pos1 - ((data->bufferChunks - 1) * WDVH_MAX_XML_BUFFER_LEN) + 1;
		if (pos1 < 0) {
			temp1 = strchr(data->currentBuff,'>');
		} else {
			temp1 = strchr(data->currentBuff+pos1,'>');
		}
		if (temp1) {
			if (temp1 != data->currentBuff) {
				return *(temp1-1) == '/';
			} else {
				/* slash is last character of previous Buffer */
				return *(data->previousBuff+WDVH_MAX_XML_BUFFER_LEN-1) == '/';
			}
		} else {
			return WDVH_False;
		}
	}

	/*------------------------------------------------------------------------
		Function:	xmlParserCheckNamespaceAbbrev
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserCheckNamespaceAbbrev(WDVH_Char *tag)
	{
		WDVH_Char	*temp1;
		WDVH_Int4	pos1;

		temp1 = strchr(tag,':');
		pos1 = temp1 - tag + 1;

		if (pos1 < strlen(tag)) 
			return WDVH_True;			
		else
			return WDVH_False;

	}


	/*************************************************************************
	 |	NAMESPACE LIST                                                       |
	 **************************************************************************/

	/*------------------------------------------------------------------------
		Function:	xmlParserCreateNamespaceList
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserCreateNamespaceList(XML_Parser p, 
										   WDVH_XmlNamespaceList *nslist)
	{
		WDVH_Bool              ok = WDVH_False;
		WDVH_XmlNamespaceList  newNameSpaceList = NULL;

		if ( !nslist ) {
			return WDVH_False;
		}
    
		WDVHandler_Allocat( sizeof(struct st_ns_list), (WDVH_UInt1**) &newNameSpaceList, &ok );
		if (ok == WDVH_True) {
			newNameSpaceList->first		= NULL;
			newNameSpaceList->last		= NULL;
			newNameSpaceList->current	= NULL;
			newNameSpaceList->length    = 0;

			*nslist = newNameSpaceList;

			return WDVH_True;
		} else {
			*nslist = NULL;

			return WDVH_False;
		}

	}

	/*------------------------------------------------------------------------
		Function:	xmlParserDestroyNamespaceList
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserDestroyNamespaceList(XML_Parser p, 
							 			    WDVH_XmlNamespaceList nslist)	
	{
		WDVH_XmlNamespaceListItem	temp;

		nslist->current = nslist->first;
		while(nslist->current != NULL) {
			temp = nslist->current;
			nslist->current = nslist->current->next;
			WDVHandler_Free((WDVH_UInt1*)temp);
		}
		if(nslist)
			WDVHandler_Free((WDVH_UInt1*)nslist);

		return WDVH_True;
	}

	/*------------------------------------------------------------------------
		Function:	xmlParserAddNamespaceToList
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserAddNamespaceToList(XML_Parser p, 
									      WDVH_XmlNamespace ns,
										  WDVH_XmlNamespaceList nslist)
	{
		WDVH_Bool					ok = WDVH_False;
		WDVH_XmlNamespaceListItem	newItem = NULL;

		/* check wether there is a namespace with the same prefix in the list allready
		   or not */
		if (xmlParserIsPrefixInNamespaceList(p,ns,nslist))
			return WDVH_False;
	
		WDVHandler_Allocat( sizeof(struct st_ns_list_entry), (WDVH_UInt1**) &newItem, &ok );
		if (ok == WDVH_True) {
			if (ns.prefix)
				strcpy(newItem->ns.prefix,ns.prefix);
			strcpy(newItem->ns.uri,ns.uri);
			newItem->next = NULL;
			newItem->prev = nslist->last;

			if (nslist->length > 0) {
				nslist->last->next = newItem;
				nslist->last       = newItem;
				nslist->length++;
			} else {
				nslist->first = newItem;
				nslist->last  = newItem;
				nslist->length++;
			}

			return WDVH_True;
		} else {

			return WDVH_False;
		}

	}

	/*------------------------------------------------------------------------
		Function:	xmlParserRemoveNamespaceFromList
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserRemoveNamespaceFromList(XML_Parser p, 
											   WDVH_XmlNamespace ns,
											   WDVH_XmlNamespaceList nslist)
	{
		WDVH_XmlNamespaceListItem	temp;

		nslist->current = nslist->last;
		while(nslist->current != NULL) {
			if (strcmp(nslist->current->ns.prefix,ns.prefix)==0) {
				temp = nslist->current;
				if (nslist->length > 1) {
					if (nslist->current->next != NULL)
						nslist->current->next->prev = temp->prev;
					else {
						nslist->current->prev->next = NULL;
						nslist->last = nslist->current->prev;
					}
					if (nslist->current->prev != NULL)
						nslist->current->prev->next = temp->next;
					else {
						nslist->current->next->prev = NULL;
						nslist->first = nslist->current->next;
					}
				} else {
					nslist->first   = NULL;
					nslist->last    = NULL;
					nslist->current = NULL;
				}
				WDVHandler_Free((WDVH_UInt1*)temp);
				nslist->length--;
				return WDVH_True;
			} else
				nslist->current = nslist->current->prev;
		}
		return WDVH_False;
	}

	/*------------------------------------------------------------------------
		Function:	xmlParserIsInNamespaceList
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserIsInNamespaceList(XML_Parser p, 
										 WDVH_XmlNamespace ns,
									     WDVH_XmlNamespaceList nslist)
	{
		nslist->current = nslist->last;
		while(nslist->current != NULL) {
			if ((strcmp(nslist->current->ns.prefix,ns.prefix)==0) && 
				(strcmp(nslist->current->ns.uri,ns.uri)==0))
				return WDVH_True;
			else
				nslist->current = nslist->current->prev;
		}
		return WDVH_False;
	}

	/*------------------------------------------------------------------------
		Function:	xmlParserIsPrefixInNamespaceList
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserIsPrefixInNamespaceList(XML_Parser p, 
											   WDVH_XmlNamespace ns,
									           WDVH_XmlNamespaceList nslist)
	{
		nslist->current = nslist->last;
		while(nslist->current != NULL) {
			if (strcmp(nslist->current->ns.prefix,ns.prefix)==0)
				return WDVH_True;
			else
				nslist->current = nslist->current->prev;
		}
		return WDVH_False;
	}

	/*************************************************************************
	 |	USER DATA                                                            |
	 **************************************************************************/

	/*------------------------------------------------------------------------
		Function:	xmlParserCreateUserData
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserCreateUserData(XML_Parser p,
									  WDVH_XmlBufferRef buff,
/*									  WDVH_XmlBufferRef oldbuff,		http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
/*									  WDVH_UInt4 chunks,				http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
								      WDVH_XmlNamespaceList nslist,
								      WDVCAPI_WDV wdv,
									  WDVH_XmlUserData *data)
	{
		WDVH_Bool         ok = WDVH_False;
		WDVH_XmlUserData  newUserData = NULL;

		if ( !data ) {
			return WDVH_False;
		}
    
		WDVHandler_Allocat( sizeof(struct st_user_data), (WDVH_UInt1**) &newUserData, &ok );
		if (ok == WDVH_True) {
			newUserData->parser = p;
			newUserData->currentBuff	= buff;
/*			newUserData->previousBuff	= oldbuff;					http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
/*			newUserData->bufferChunks	= chunks;					http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
			newUserData->bufferStart = 0;							/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124043 */
			newUserData->bufferEnd = 0;								/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124043 */
			newUserData->nslist	= nslist;
			newUserData->wdv    = wdv;
			newUserData->propertyShortValue = (WDVH_DynamicString)createStrBuffer(1025);	/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
			newUserData->propertyShortValueStarted = WDVH_False;							/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
			newUserData->propertyValueStartPos = 0;											/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
			newUserData->propertyValueEndPos = 0;											/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
			newUserData->lockscope = 0;
			newUserData->locktype = 0;
			newUserData->propfindEmptyPropertyList = WDVH_True;		/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123332 */
			newUserData->error = NULL;								/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
			newUserData->isPropnameRequest = WDVH_False;			/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1130891 */

			*data = newUserData;

			return WDVH_True;
		} else {
			*data = NULL;

			return WDVH_False;
		}

	}

	/*------------------------------------------------------------------------
		Function:	xmlParserDestroyUserData
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserDestroyUserData(WDVH_XmlUserData data)
	{
		if ( !data ) {
			return WDVH_False;
		}
    
		destroyStrBuffer(data->propertyShortValue);
		WDVHandler_Free((WDVH_UInt1*)data);
		return WDVH_True;
	}

	/*------------------------------------------------------------------------
		Function:	xmlParserUserDataGetParser
	--------------------------------------------------------------------------*/
	XML_Parser xmlParserUserDataGetParser(WDVH_XmlUserData data)
	{
		return data->parser;
	}

	/*------------------------------------------------------------------------
		Function:	xmlParserUserDataGetWDV
	--------------------------------------------------------------------------*/
	WDVCAPI_WDV xmlParserUserDataGetWDV(WDVH_XmlUserData data)
	{
		return data->wdv;
	}

	/*------------------------------------------------------------------------
		Function:	xmlParserUserDataSetPropfindHandle
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserUserDataSetPropfindHandle(WDVH_XmlUserData data,
												 WDVCAPI_PropfindHandle *propfind)
	{
		data->propfindHandle = propfind;
		return WDVH_True;
	}

	/*------------------------------------------------------------------------
		Function:	xmlParserUserDataGetPropfindHandle
	--------------------------------------------------------------------------*/
	WDVCAPI_PropfindHandle* xmlParserUserDataGetPropfindHandle(WDVH_XmlUserData data)
	{
		return data->propfindHandle;
	}

	/*------------------------------------------------------------------------
		Function:	xmlParserUserDataSetWaHandle
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserUserDataSetWaHandle(WDVH_XmlUserData data,
										   sapdbwa_Handle wa)
	{
		data->wa = wa;
		return WDVH_True;
	}

	/*------------------------------------------------------------------------
		Function:	xmlParserUserDataGetWaHandle
	--------------------------------------------------------------------------*/
	sapdbwa_Handle xmlParserUserDataGetWaHandle(WDVH_XmlUserData data)
	{
		return data->wa;
	}

	/*------------------------------------------------------------------------
		Function:	xmlParserUserDataSetDepth
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserUserDataSetDepth(WDVH_XmlUserData data,
										 WDVCAPI_Depth depth)
	{
		data->depth = depth;
		return WDVH_True;
	}

	/*------------------------------------------------------------------------
		Function:	xmlParserUserDataGetDepth
	--------------------------------------------------------------------------*/
	WDVCAPI_Depth xmlParserUserDataGetDepth(WDVH_XmlUserData data)
	{
		return data->depth;
	}

	/*------------------------------------------------------------------------
		Function:	xmlParserUserDataSetRequestUri
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserUserDataSetRequestUri(WDVH_XmlUserData data,
											 WDVCAPI_URI uri)
	{
		data->requestUri = uri;
		return WDVH_True;
	}

	/*------------------------------------------------------------------------
		Function:	xmlParserUserDataGetRequestUri
	--------------------------------------------------------------------------*/
	WDVCAPI_URI xmlParserUserDataGetRequestUri(WDVH_XmlUserData data)
	{
		return data->requestUri;
	}

	/*------------------------------------------------------------------------
		Function:	xmlParserUserDataSetRequest
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserUserDataSetRequest(WDVH_XmlUserData data,
										  sapdbwa_HttpRequestP	request)
	{
		data->request = request;
		return WDVH_True;
	}

	/*------------------------------------------------------------------------
		Function:	xmlParserUserDataGetRequest
	--------------------------------------------------------------------------*/
	sapdbwa_HttpRequestP xmlParserUserDataGetRequest(WDVH_XmlUserData data)
	{
		return data->request;
	}

	/*------------------------------------------------------------------------
		Function:	xmlParserUserDataSetProppatchHandle
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserUserDataSetProppatchHandle(WDVH_XmlUserData data,
										 		  WDVCAPI_ProppatchHandle proppatch)
	{
		data->proppatchHandle = proppatch;
		return WDVH_True;
	}

	/*------------------------------------------------------------------------
		Function:	xmlParserUserDataGetProppatchHandle
	--------------------------------------------------------------------------*/
	WDVCAPI_ProppatchHandle xmlParserUserDataGetProppatchHandle(WDVH_XmlUserData data)
	{
		return data->proppatchHandle;
	}

	/*------------------------------------------------------------------------
		Function:	xmlParserUserDataSetParseBuffer
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserUserDataSetParseBuffer(WDVH_XmlUserData data,
											  WDVH_XmlBufferRef current,
											  WDVH_XmlBufferRef previous,
											  WDVH_UInt4        chunks)
	{
		data->currentBuff = current;
		data->previousBuff = previous;
		data->bufferChunks = chunks;
		return WDVH_True;
	}

	/*------------------------------------------------------------------------
		Function:	xmlParserUserDataSetParserState
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserUserDataSetParserState(WDVH_XmlUserData data,
											  WDVH_XmlParserState parserState)
	{
		data->parserState = parserState;
		return WDVH_True;
	}

	/*------------------------------------------------------------------------
		Function:	xmlParserUserDataGetPropfindParserState
	--------------------------------------------------------------------------*/
	WDVH_XmlPropfindParserState xmlParserUserDataGetPropfindParserState(WDVH_XmlUserData data)
	{
		return (WDVH_XmlPropfindParserState)data->parserState;
	}

	/*------------------------------------------------------------------------
		Function:	xmlParserUserDataGetProppatchParserState
	--------------------------------------------------------------------------*/
	WDVH_XmlProppatchParserState xmlParserUserDataGetProppatchParserState(WDVH_XmlUserData data)
	{
		return (WDVH_XmlProppatchParserState)data->parserState;
	}

	/*------------------------------------------------------------------------
		Function:	xmlParserUserDataGetLockParserState
	--------------------------------------------------------------------------*/
	WDVH_XmlLockParserState xmlParserUserDataGetLockParserState(WDVH_XmlUserData data)
	{
		return (WDVH_XmlLockParserState)data->parserState;
	}

	/*------------------------------------------------------------------------
		Function:	xmlParserCutNamespace
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserCutNamespace(const WDVH_Char *intag, 
									WDVH_Char *ns, 
									WDVH_Char *outtag,
									WDVH_Char sep)
	{
		WDVH_Char	*temp;
		WDVH_UInt4	pos;
		
		/* cut namespace info */
		temp = strrchr(intag,sep);
		if (!temp) {
			strcpy(ns,"");
			strcpy(outtag,intag);
		} else {
			pos = temp - intag + 1;
			strncpy(ns,intag,pos-1);
			ns[pos-1] = '\0';
			strcpy(outtag,temp+1);
		}

		return WDVH_True;
	}

	/*------------------------------------------------------------------------
		Function:	xmlParserCreatePropfindParserState
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserCreatePropfindParserState(WDVH_XmlPropfindParserState *parserState)
	{
		WDVH_Bool					 ok = WDVH_False;
		WDVH_XmlPropfindParserState  newPropfindParserState = NULL;

		if ( !parserState ) {
			return WDVH_False;
		}
    
		WDVHandler_Allocat( sizeof(struct st_propfind_parser_state),
					(WDVH_UInt1**) &newPropfindParserState,
					&ok );
		if (ok == WDVH_True) {
			newPropfindParserState->depth = 0;
			newPropfindParserState->state = WDVH_PROPFIND_DFA_STATE_1;
			newPropfindParserState->error = WDVH_XML_PARSER_OK;

			*parserState = newPropfindParserState;

			return WDVH_True;
		} else {
			*parserState = NULL;

			return WDVH_False;
		}

	}

	/*------------------------------------------------------------------------
		Function:	xmlParserDestroyPropfindParserState
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserDestroyPropfindParserState(WDVH_XmlPropfindParserState parserState)
	{
		if ( !parserState ) {
			return WDVH_False;
		}
    
		WDVHandler_Free((WDVH_UInt1*)parserState);
		return WDVH_True;
	}

	/*------------------------------------------------------------------------
		Function:	xmlParserCreateProppatchParserState
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserCreateProppatchParserState(WDVH_XmlProppatchParserState *parserState)
	{
		WDVH_Bool					  ok = WDVH_False;
		WDVH_XmlProppatchParserState  newProppatchParserState = NULL;

		if ( !parserState ) {
			return WDVH_False;
		}
    
		WDVHandler_Allocat( sizeof(struct st_proppatch_parser_state),
					(WDVH_UInt1**) &newProppatchParserState,
					&ok );
		if (ok == WDVH_True) {
			newProppatchParserState->depth = 0;
			newProppatchParserState->state = WDVH_PROPPATCH_DFA_STATE_1;
			newProppatchParserState->error = WDVH_XML_PARSER_OK;

			*parserState = newProppatchParserState;

			return WDVH_True;
		} else {
			*parserState = NULL;

			return WDVH_False;
		}

	}

	/*------------------------------------------------------------------------
		Function:	xmlParserDestroyProppatchParserState
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserDestroyProppatchParserState(WDVH_XmlProppatchParserState parserState)
	{
		if ( !parserState ) {
			return WDVH_False;
		}
    
		WDVHandler_Free((WDVH_UInt1*)parserState);
		return WDVH_True;
	}

	/*------------------------------------------------------------------------
		Function:	xmlParserCreateLockParserState
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserCreateLockParserState(WDVH_XmlLockParserState *parserState)
	{
		WDVH_Bool					  ok = WDVH_False;
		WDVH_XmlLockParserState  newLockParserState = NULL;

		if ( !parserState ) {
			return WDVH_False;
		}
    
		WDVHandler_Allocat( sizeof(struct st_lock_parser_state),
					(WDVH_UInt1**) &newLockParserState,
					&ok );
		if (ok == WDVH_True) {
			newLockParserState->depth = 0;
			newLockParserState->state = WDVH_LOCK_DFA_STATE_1;
			newLockParserState->error = WDVH_XML_PARSER_OK;
			newLockParserState->lockscope = WDVH_False;
			newLockParserState->locktype = WDVH_False;
			newLockParserState->owner = WDVH_False;

			*parserState = newLockParserState;

			return WDVH_True;
		} else {
			*parserState = NULL;

			return WDVH_False;
		}

	}

	/*------------------------------------------------------------------------
		Function:	xmlParserDestroyLockParserState
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserDestroyLockParserState(WDVH_XmlLockParserState parserState)
	{
		if ( !parserState ) {
			return WDVH_False;
		}
    
		WDVHandler_Free((WDVH_UInt1*)parserState);
		return WDVH_True;
	}

