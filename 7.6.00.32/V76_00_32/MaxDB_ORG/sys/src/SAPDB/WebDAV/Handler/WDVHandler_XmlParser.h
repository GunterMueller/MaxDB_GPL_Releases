#ifndef WDVHANDLER_XMLPARSER_H
#define WDVHANDLER_XMLPARSER_H
/****************************************************************************

  module      : WDVHandler_XmlParser.h

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


	#include "SAPDB/WebDAV/Handler/WDVHandler_CommonInclude.h"
	#include "idl/expat/expat.h"

	#define XML_DTD

	/*------------------------------------------------------------------------
	Function	:	xmlParserCheckEmptyTag
	Description	:	check wether an empty tag is written as <emptytag/> or not
	Parameters	:	[in]	p	-	XML parser instance
					[in]	buff-	(part of) document to parse
					[in]	tag	-	tag to check
	Return		:	WDVH_True	-	tag written as <emptytag/>
					WDVH_False	-	tag written as <emptytag></emptytag> or else
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserCheckEmptyTag(XML_Parser p, WDVH_XmlUserData data, WDVH_XmlEmptyTag tag);

	/*------------------------------------------------------------------------
	Function	:	xmlParserCheckNamespaceAbbrev
	Description	:	check wether or not a tag contains a namespace abbrev
	Parameters	:	[in]	tag	-	tag to check
	Return		:	WDVH_True	-	tag contains namespace abbrev
					WDVH_False	-	tag contains no namespace abbrev
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserCheckNamespaceAbbrev(WDVH_Char *tag);

	/*------------------------------------------------------------------------
	Function	:	xmlParserCreateNamespaceList
	Description	:	create doubled linked list
	Parameters	:	[in]	p	-	parser
					[in]	ns	-	namespace entry
					[out]	nslist - namespacelist
	Return		:	WDVH_True	- successfull
					WDVH_False	- error occured
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserCreateNamespaceList(XML_Parser p, 
										   WDVH_XmlNamespaceList *nslist);

	/*------------------------------------------------------------------------
	Function	:	xmlParserDestroyNamespaceList
	Description	:	destroy namespace list
	Parameters	:	[in]	p	-	parser
					[in]	nslist - namespacelist
	Return		:	WDVH_True	- successfull
					WDVH_False	- error occured
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserDestroyNamespaceList(XML_Parser p, 
							 			    WDVH_XmlNamespaceList nslist);

	/*------------------------------------------------------------------------
	Function	:	xmlParserAddNamespaceToList
	Description	:	add namespace to namespace list (at back)
	Parameters	:	[in]	p	-	parser
					[in]	ns	-	namespace entry
	Return		:	WDVH_True	- successfull
					WDVH_False	- 
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserAddNamespaceToList(XML_Parser p, 
									      WDVH_XmlNamespace ns,
										  WDVH_XmlNamespaceList nslist);

	/*------------------------------------------------------------------------
	Function	:	xmlParserRemoveNamespaceFromList
	Description	:	removes namespace from namespace list
	Parameters	:	[in]	p	-	parser
					[in]	ns	-	namespace entry
	Return		:	WDVH_True	- successfull
					WDVH_False	- 
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserRemoveNamespaceFromList(XML_Parser p, 
											   WDVH_XmlNamespace ns,
											   WDVH_XmlNamespaceList nslist);

	/*------------------------------------------------------------------------
	Function	:	xmlParserIsInNamespaceList
	Description	:	check wether an given Namespace is part of the list
	Parameters	:	[in]	p	-	parser
					[in]	ns	-	namespace entry
					[in]	nslist - namespacelist
	Return		:	WDVH_True	- yes
					WDVH_False	- no
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserIsInNamespaceList(XML_Parser p, 
										 WDVH_XmlNamespace ns,
									     WDVH_XmlNamespaceList nslist);

	/*------------------------------------------------------------------------
	Function	:	xmlParserIsPrefixInNamespaceList
	Description	:	check wether an given Namespace prefix is part of the list
	Parameters	:	[in]	p	-	parser
					[in]	ns	-	namespace entry
					[in]	nslist - namespacelist
	Return		:	WDVH_True	- yes
					WDVH_False	- no
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserIsPrefixInNamespaceList(XML_Parser p, 
								  			   WDVH_XmlNamespace ns,
											   WDVH_XmlNamespaceList nslist);

	/*------------------------------------------------------------------------
	Function	:	xmlParserCreateUserData
	Description	:	create user data for communication between handlers
	Parameters	:	[in]	buff   -	buffer with xml doc to parse
					[in]	nslist -	namespace list
					[out]	data   -    user data record
	Return		:	WDVH_True	- yes
					WDVH_False	- no
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserCreateUserData(XML_Parser p,
									  WDVH_XmlBuffer buff,
/*									  WDVH_XmlBufferRef oldbuff,		http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
/*									  WDVH_UInt4 chunks,				http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
							 	      WDVH_XmlNamespaceList nslist,
									  WDVCAPI_WDV wdv,
								      WDVH_XmlUserData *data);

	/*------------------------------------------------------------------------
	Function	:	xmlParserDestroyUserData
	Description	:	destroy user data 
	Parameters	:	[in]	data   -    user data record
	Return		:	WDVH_True	- yes
					WDVH_False	- no
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserDestroyUserData(WDVH_XmlUserData data);

	/*------------------------------------------------------------------------
	Function	:	xmlParserUserDataGetParser
	Description	:	get parser handle from user data record 
	Parameters	:	[in]	data   -    user data record
	Return		:	parser handle for expat parser
	--------------------------------------------------------------------------*/
	XML_Parser xmlParserUserDataGetParser(WDVH_XmlUserData data);

	/*------------------------------------------------------------------------
	Function	:	xmlParserUserDataGetWDV
	Description	:	get wdv handle from user data record 
	Parameters	:	[in]	data   -    user data record
	Return		:	wdv handle for propfind CAPI function
	--------------------------------------------------------------------------*/
	WDVCAPI_WDV xmlParserUserDataGetWDV(WDVH_XmlUserData data);

	/*------------------------------------------------------------------------
	Function	:	xmlParserUserDataSetParseBuffer
	Description	:	set parse buffer and count of buffer chunks 
	Parameters	:	[in]	data   -    user data record

	Return		:	WDVH_True	- yes
					WDVH_False	- no
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserUserDataSetParseBuffer(WDVH_XmlUserData data,
											  WDVH_XmlBufferRef current,
											  WDVH_XmlBufferRef previous,
											  WDVH_UInt4        chunks);

	/*------------------------------------------------------------------------
	Function	:	xmlParserUserDataSetPropfindHandle
	Description	:	set PropfindHandle in user data record 
	Parameters	:	[in]	data   -    user data record
					[in]	propfind - handle for propfind CAPI function
	Return		:	WDVH_True	- yes
					WDVH_False	- no
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserUserDataSetPropfindHandle(WDVH_XmlUserData data,
												 WDVCAPI_PropfindHandle *propfind);

	/*------------------------------------------------------------------------
	Function	:	xmlParserUserDataGetPropfindHandle
	Description	:	get PropfindHandle from user data record 
	Parameters	:	[in]	data   -    user data record
	Return		:	propfind handle for propfind CAPI function
	--------------------------------------------------------------------------*/
	WDVCAPI_PropfindHandle* xmlParserUserDataGetPropfindHandle(WDVH_XmlUserData data);

	/*------------------------------------------------------------------------
		Function:	xmlParserUserDataSetWaHandle
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserUserDataSetWaHandle(WDVH_XmlUserData data,
										   sapdbwa_Handle wa);

	/*------------------------------------------------------------------------
		Function:	xmlParserUserDataGetWaHandle
	--------------------------------------------------------------------------*/
	sapdbwa_Handle xmlParserUserDataGetWaHandle(WDVH_XmlUserData data);

	/*------------------------------------------------------------------------
		Function:	xmlParserUserDataSetDepth
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserUserDataSetDepth(WDVH_XmlUserData data,
										 WDVCAPI_Depth depth);

	/*------------------------------------------------------------------------
		Function:	xmlParserUserDataGetDepth
	--------------------------------------------------------------------------*/
	WDVCAPI_Depth xmlParserUserDataGetDepth(WDVH_XmlUserData data);

	/*------------------------------------------------------------------------
		Function:	xmlParserUserDataSetRequestUri
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserUserDataSetRequestUri(WDVH_XmlUserData data,
											 WDVCAPI_URI uri);

	/*------------------------------------------------------------------------
		Function:	xmlParserUserDataGetRequestUri
	--------------------------------------------------------------------------*/
	WDVCAPI_URI xmlParserUserDataGetRequestUri(WDVH_XmlUserData data);

	/*------------------------------------------------------------------------
		Function:	xmlParserUserDataSetRequest
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserUserDataSetRequest(WDVH_XmlUserData data,
										  sapdbwa_HttpRequestP	request);

	/*------------------------------------------------------------------------
		Function:	xmlParserUserDataGetRequest
	--------------------------------------------------------------------------*/
	sapdbwa_HttpRequestP xmlParserUserDataGetRequest(WDVH_XmlUserData data);

	/*------------------------------------------------------------------------
	Function	:	xmlParserUserDataSetProppatchHandle
	Description	:	set ProppatchHandle in user data record 
	Parameters	:	[in]	data   -    user data record
					[in]	proppatch - handle for proppatch CAPI function
	Return		:	WDVH_True	- yes
					WDVH_False	- no
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserUserDataSetProppatchHandle(WDVH_XmlUserData data,
												 WDVCAPI_ProppatchHandle proppatch);

	/*------------------------------------------------------------------------
	Function	:	xmlParserUserDataGetProppatchHandle
	Description	:	get ProppatchHandle from user data record 
	Parameters	:	[in]	data   -    user data record
	Return		:	proppatch handle for proppatch CAPI function
	--------------------------------------------------------------------------*/
	WDVCAPI_ProppatchHandle xmlParserUserDataGetProppatchHandle(WDVH_XmlUserData data);

	/*------------------------------------------------------------------------
	Function	:	xmlParserUserDataSetParserState
	Description	:	set parser state record  
	Parameters	:	[in]	data   -    user data record
					[in]	parserState -	parser state
	Return		:	WDVH_True	- yes
					WDVH_False	- no
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserUserDataSetParserState(WDVH_XmlUserData data,
											  WDVH_XmlParserState parserState);

	/*------------------------------------------------------------------------
	Function	:	xmlParserUserDataGetPropfindParserState
	Description	:	set parser state record  
	Parameters	:	[in]	data   -    user data record
	Return		:	parser state
	--------------------------------------------------------------------------*/
	WDVH_XmlPropfindParserState xmlParserUserDataGetPropfindParserState(WDVH_XmlUserData data);

	/*------------------------------------------------------------------------
	Function	:	xmlParserUserDataGetProppatchParserState
	Description	:	set parser state record  
	Parameters	:	[in]	data   -    user data record
	Return		:	parser state
	--------------------------------------------------------------------------*/
	WDVH_XmlProppatchParserState xmlParserUserDataGetProppatchParserState(WDVH_XmlUserData data);

	/*------------------------------------------------------------------------
	Function	:	xmlParserUserDataGetLockParserState
	Description	:	set parser state record  
	Parameters	:	[in]	data   -    user data record
	Return		:	parser state
	--------------------------------------------------------------------------*/
	WDVH_XmlLockParserState xmlParserUserDataGetLockParserState(WDVH_XmlUserData data);

	/*------------------------------------------------------------------------
	Function	:	xmlParserCutNamespace
	Description	:	splits a propertyname <namespace:propname> into namespace
					and property name
	Parameters	:	[in]	inTag   - tag found by expat
					[out]	ns		- namespace (long)
					[out]	outTag	- tag without namespace info	
	Return		:	WDVH_True	- yes
					WDVH_False	- no
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserCutNamespace(const WDVH_Char *intag,
									WDVH_Char *ns, 
									WDVH_Char *outtag, 
									WDVH_Char sep);

	/*------------------------------------------------------------------------
	Function	:	xmlParserCreatePropfindParserState
	Description	:	create propfind parser state record and initialize it
	Parameters	:	[out]	parserState -    parser state record
	Return		:	WDVH_True	- yes
					WDVH_False	- no
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserCreatePropfindParserState(WDVH_XmlPropfindParserState *parserState);

	/*------------------------------------------------------------------------
	Function	:	xmlParserDestroyPropfindParserState
	Description	:	destroy parser state record 
	Parameters	:	[in]	parserState   -    parser state record
	Return		:	WDVH_True	- yes
					WDVH_False	- no
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserDestroyPropfindParserState(WDVH_XmlPropfindParserState data);

	/*------------------------------------------------------------------------
	Function	:	xmlParserCreateProppatchParserState
	Description	:	create proppatch parser state record and initialize it
	Parameters	:	[out]	parserState -    parser state record
	Return		:	WDVH_True	- yes
					WDVH_False	- no
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserCreateProppatchParserState(WDVH_XmlProppatchParserState *parserState);

	/*------------------------------------------------------------------------
	Function	:	xmlParserDestroyProppatchParserState
	Description	:	destroy parser state record 
	Parameters	:	[in]	parserState   -    parser state record
	Return		:	WDVH_True	- yes
					WDVH_False	- no
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserDestroyProppatchParserState(WDVH_XmlProppatchParserState data);

	/*------------------------------------------------------------------------
	Function	:	xmlParserCreateLockParserState
	Description	:	create lock parser state record and initialize it
	Parameters	:	[out]	parserState -    parser state record
	Return		:	WDVH_True	- yes
					WDVH_False	- no
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserCreateLockParserState(WDVH_XmlLockParserState *parserState);

	/*------------------------------------------------------------------------
	Function	:	xmlParserDestroyLockParserState
	Description	:	destroy parser state record 
	Parameters	:	[in]	parserState   -    parser state record
	Return		:	WDVH_True	- yes
					WDVH_False	- no
	--------------------------------------------------------------------------*/
	WDVH_Bool xmlParserDestroyLockParserState(WDVH_XmlLockParserState data);

#endif