/****************************************************************************

  module      : XMLIDMLib_HtmlTemplate_DocClass_Show.cpp

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-07-13  9:26
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


	/*===================================================================
	// Includes
	//===================================================================*/
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_HtmlTemplate_DocClass_Show.hpp"



	XMLIDMLib_HtmlTemplate_DocClass_Show::XMLIDMLib_HtmlTemplate_DocClass_Show (sapdbwa_WebAgent &wa,
																  sapdbwa_HttpRequest &req,
																  XMLIMAPI_Handle	&xml) 
		                  : _wa(wa), _req(req), _xml(xml), Tools_Template ( wa, (const unsigned char*)TEMPLATE_DOCCLASS_SHOW )
	{ 
		XMLIDMLIB_Char __value[5001];
		
		XMLIDMLIB_GetParameterValue(TEXT_CONSTANT_PARAM_DOCCLASS_ID,req,__value);
		strcpy(_docClass.docClassID,__value);

		XMLIDMLIB_GetParameterValue(TEXT_CONSTANT_PARAM_DOCCLASS_NAME,req,__value);
		strcpy(_docClass.docClassName,__value);

		XMLIDMLIB_GetParameterValue(TEXT_CONSTANT_PARAM_DOCCLASS_DESCRIPTION,req,__value);
		strcpy(_docClass.description,__value);

		XMLIDMLIB_GetParameterValue(TEXT_CONSTANT_PARAM_DOCCLASS_ASSIGNED_XML_INDEXES,req,__value);
		strncpy(_assignedXmlIndexesString,__value,5000);
		_assignedXmlIndexesString[5000]='\0';

		XMLIDMLIB_GetParameterValue(TEXT_CONSTANT_PARAM_DOCCLASS_UNASSIGNED_XML_INDEXES,req,__value);
		strncpy(_unAssignedXmlIndexesString,__value,5000);
		_unAssignedXmlIndexesString[5000]='\0';

		XMLIDMLIB_GetParameterValue(TEXT_CONSTANT_PARAM_DOCCLASS_NEWLY_ADDED,req,__value);
		strncpy(_newlyAdded,__value,5000);
		_newlyAdded[5000]='\0';

		XMLIDMLIB_GetParameterValue(TEXT_CONSTANT_PARAM_IDXSERVICE_ID,req,__value);
		strncpy(_idxServiceID,__value,49);
		_idxServiceID[49] = '\0';
		XMLIDMLIB_GetParameterValue(TEXT_CONSTANT_PARAM_IDXSERVICE_NAME,req,__value);
		strncpy(_idxServiceName,__value,129);
		_idxServiceName[129] = '\0';
		XMLIDMLIB_GetParameterValue(TEXT_CONSTANT_PARAM_IDXSERVICE_DESCRIPTION,req,__value);
		strncpy(_idxServiceDesc,__value,519);
		_idxServiceDesc[519] = '\0';
		XMLIDMLIB_GetParameterValue(TEXT_CONSTANT_PARAM_IDXSERVICE_STORE,req,__value);
		strncpy(_store,__value,9);
		_store[9]='\0';
		XMLIDMLIB_GetParameterValue(TEXT_CONSTANT_PARAM_IDXSERVICE_DOCSTORE,req,__value);
		strncpy(_docstore,__value,129);
		_docstore[129]='\0';
		XMLIDMLIB_GetParameterValue(TEXT_CONSTANT_PARAM_IDXSERVICE_IDXSTORE,req,__value);
		strncpy(_idxstore,__value,129);
		_idxstore[129]='\0';
		XMLIDMLIB_GetParameterValue(TEXT_CONSTANT_PARAM_IDXSERVICE_DSSPNEW,req,__value);
		strncpy(_dsspnew,__value,2);
		_dsspnew[2]='\0';
		XMLIDMLIB_GetParameterValue(TEXT_CONSTANT_PARAM_IDXSERVICE_ISSPNEW,req,__value);
		strncpy(_isspnew,__value,2);
		_isspnew[2]='\0';

		XMLIMAPI_XmlIndexListCreate(&_xmlIndexList);
		_xmlIndexCount = XMLIMAPI_XmlIndexListGetLength(_xmlIndexList);
		_firstXmlIndex = XMLIDMLIB_True;

		XMLIMAPI_XmlIndexListCreate(&_assignedXmlIndices);
		_assignedXmlIndicesCount = XMLIMAPI_XmlIndexListGetLength(_assignedXmlIndices);
		_assignedCount = 0;

		if (strcmp(_docClass.docClassName,"")==0)
			getDocClass();
		XMLIDMLIB_GetParameterValue(TEXT_CONSTANT_PARAM_SUB_COMMAND,req,__value);
		if (strcmp(__value,"DcAddIndex")==0)
			addAssignedXmlIndex();
		else if (strcmp(__value,"DcRemoveAllIndex")==0)
			removeAllAssignedXmlIndices();
		else if (strcmp(__value,"DcRemoveIndex")==0)
			removeAssignedXmlIndex();
		else {
			/* first turn around -> get the assigned indexes from DB */
			getAssignedXmlIndices();
		}
		getXmlIndices();
		strcpy(_serviceName,wa.GetServiceName());
	} 


	XMLIDMLib_HtmlTemplate_DocClass_Show::~XMLIDMLib_HtmlTemplate_DocClass_Show() 
	{
		/* delete all xml indices */
		XMLIMAPI_XmlIndexListDestroy(_xmlIndexList);
		XMLIMAPI_XmlIndexListDestroy(_assignedXmlIndices);
	}

	XMLIDMLIB_Int2 XMLIDMLib_HtmlTemplate_DocClass_Show :: askForWriteCount(const Tools_DynamicUTF8String & szName)
	{
		XMLIDMLIB_UInt4 __nReturn = 0;

		if (szName == TEMPLATE_KEY_DOC_SHOW_DOCCLASSNAME) {
			__nReturn = 1;
		} 
		else if (szName == TEMPLATE_KEY_DOC_SHOW_DOCCLASSID) {
			__nReturn = 1;
		}
		else if (szName == TEMPLATE_KEY_DOC_SHOW_DOCDESCRIPTION) {
			__nReturn = 1;
		}
		else if (szName == TEMPLATE_KEY_DOC_SHOW_ASSIGNED_XML_INDEXES) {
			__nReturn = _assignedXmlIndicesCount;
		}
		else if (szName == TEMPLATE_KEY_DOC_SHOW_ASSIGNED_XML_INDEXES_STR) {
			__nReturn = 1;
		}
		else if (szName == TEMPLATE_KEY_DOC_SHOW_ASSIGNED_XML_INDEXES_CNT) {
			__nReturn = 1;
		}
		else if (szName == TEMPLATE_KEY_DOC_SHOW_NEWLY_ADDED) {
			__nReturn = 1;
		}
		else if (szName == TEMPLATE_KEY_DOC_SHOW_ASSIGNABLE_LISTENTRY) {
			__nReturn = _xmlIndexCount;
		}
		else if (szName == TEMPLATE_KEY_DOC_SHOW_ASSIGNABLE_XML_INDEXES) {
			if (_xmlIndexCount > 0)
				__nReturn = 1;
			else
				__nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_DOC_SHOW_SERVICE) {
			__nReturn = 1;
		}
		else if (szName == TEMPLATE_KEY_DOC_SHOW_IDXSERVICE_ID) {
			if (strcmp(_idxServiceID,"")==0)
				return 0;
			else
				return 1;
		}
		else if (szName == TEMPLATE_KEY_DOC_SHOW_IDXSERVICE_NAME) {
			if (strcmp(_idxServiceName,"")==0)
				return 0;
			else
				return 1;
		}
		else if (szName == TEMPLATE_KEY_DOC_SHOW_IDXSERVICE_DESC) {
			if (strcmp(_idxServiceDesc,"")==0)
				return 0;
			else
				return 1;
		}
		else if (szName == TEMPLATE_KEY_DOC_SHOW_IDXSERVICE_STORE) {
			if (strcmp(_store,"")==0)
				return 0;
			else
				return 1;
		}
		else if (szName == TEMPLATE_KEY_DOC_SHOW_IDXSERVICE_DSSP) {
			if (strcmp(_docstore,"")==0)
				return 0;
			else
				return 1;
		}
		else if (szName == TEMPLATE_KEY_DOC_SHOW_IDXSERVICE_ISSP) {
			if (strcmp(_idxstore,"")==0)
				return 0;
			else
				return 1;
		}
		else if (szName == TEMPLATE_KEY_DOC_SHOW_IDXSERVICE_DSSPNEW) {
			if (strcmp(_dsspnew,"")==0)
				return 0;
			else
				return 1;
		}
		else if (szName == TEMPLATE_KEY_DOC_SHOW_IDXSERVICE_ISSPNEW) {
			if (strcmp(_isspnew,"")==0)
				return 0;
			else
				return 1;
		}
		else if (szName == TEMPLATE_KEY_DOC_SHOW_HEADING) {
			return 1;
		}
		return __nReturn;
	} 


	Tools_DynamicUTF8String XMLIDMLib_HtmlTemplate_DocClass_Show :: askForValue (const Tools_DynamicUTF8String & szName)
	{
		Tools_DynamicUTF8String __buffer;
		XMLIDMLIB_Char			__string[4096];

		if (szName == TEMPLATE_VALUE_DOC_SHOW_DOCCLASSNAME) {
			XMLIDMLIB_EscapeQuotes(_docClass.docClassName,strlen(_docClass.docClassName),__string);
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			return __buffer;
		} 
		else if (szName == TEMPLATE_VALUE_DOC_SHOW_DOCCLASSID) {
			__buffer.ConvertFromASCII_Latin1(_docClass.docClassID,_docClass.docClassID+strlen(_docClass.docClassID));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_DOC_SHOW_DOCDESCRIPTION) {
			XMLIDMLIB_EscapeQuotes(_docClass.description,strlen(_docClass.description),__string);
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_DOC_SHOW_ASSIGNED_XML_INDEXES) {
			__buffer.ConvertFromASCII_Latin1(_assignedXmlIndexesString,_assignedXmlIndexesString+strlen(_assignedXmlIndexesString));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_DOC_SHOW_ASSIGNED_XML_INDEXES_CNT) {
			sp77sprintf(__string,1023,"%d",_assignedXmlIndicesCount);
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_DOC_SHOW_NEWLY_ADDED) {
			__buffer.ConvertFromASCII_Latin1(_newlyAdded,_newlyAdded+strlen(_newlyAdded));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_DOC_SHOW_XML_INDEX_ID) {
			if (_assignedCount == 0) {
				XMLIMAPI_XmlIndexListGetFirst(_assignedXmlIndices,&_currXmlIndex);
			} else {
				XMLIMAPI_XmlIndexListGetNext(_assignedXmlIndices,&_currXmlIndex);
			}
			__buffer.ConvertFromASCII_Latin1(_currXmlIndex->IndexID,_currXmlIndex->IndexID+strlen(_currXmlIndex->IndexID));
			_assignedCount++;
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_DOC_SHOW_XML_INDEX_GENID) {
			sp77sprintf(__string,1023,"%d",_assignedCount);
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_DOC_SHOW_XML_INDEX_NAME) {
			XMLIDMLIB_EscapeQuotes(_currXmlIndex->indexName,strlen(_currXmlIndex->indexName),__string);
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_DOC_SHOW_XML_INDEX_DESC) {
			XMLIDMLIB_EscapeQuotes(_currXmlIndex->description,strlen(_currXmlIndex->description),__string);
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_DOC_SHOW_LISTINDEXID) {
			if (_firstXmlIndex) {
				XMLIMAPI_XmlIndexListGetFirst(_xmlIndexList,&_currXmlIndex);
				_firstXmlIndex = XMLIDMLIB_False;
			} else
				XMLIMAPI_XmlIndexListGetNext(_xmlIndexList,&_currXmlIndex);
			__buffer.ConvertFromASCII_Latin1(_currXmlIndex->IndexID,_currXmlIndex->IndexID+strlen(_currXmlIndex->IndexID));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_DOC_SHOW_LISTINDEXNAME) {
			XMLIDMLIB_EscapeQuotes(_currXmlIndex->indexName,strlen(_currXmlIndex->indexName),__string);
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_DOC_SHOW_LISTINDEXDESC) {
			XMLIDMLIB_EscapeQuotes(_currXmlIndex->description,strlen(_currXmlIndex->description),__string);
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_DOC_SHOW_IDXSERVICE_ID) {
			__buffer.ConvertFromASCII_Latin1(_idxServiceID,_idxServiceID+strlen(_idxServiceID));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_DOC_SHOW_IDXSERVICE_NAME) {
			XMLIDMLIB_EscapeQuotes(_idxServiceName,strlen(_idxServiceName),__string);
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_DOC_SHOW_IDXSERVICE_DESC) {
			XMLIDMLIB_EscapeQuotes(_idxServiceDesc,strlen(_idxServiceDesc),__string);
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_DOC_SHOW_IDXSERVICE_STORE) {
			__buffer.ConvertFromASCII_Latin1(_store,_store+strlen(_store));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_DOC_SHOW_IDXSERVICE_DSSP) {
			__buffer.ConvertFromASCII_Latin1(_docstore,_docstore+strlen(_docstore));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_DOC_SHOW_IDXSERVICE_ISSP) {
			__buffer.ConvertFromASCII_Latin1(_idxstore,_idxstore+strlen(_idxstore));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_DOC_SHOW_IDXSERVICE_DSSPNEW) {
			__buffer.ConvertFromASCII_Latin1(_dsspnew,_dsspnew+strlen(_dsspnew));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_DOC_SHOW_IDXSERVICE_ISSPNEW) {
			__buffer.ConvertFromASCII_Latin1(_isspnew,_isspnew+strlen(_isspnew));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_DOC_SHOW_SERVICE) {
			return (Tools_DynamicUTF8String::ConstPointer)_serviceName;
		}

		return "";
	}

	void XMLIDMLib_HtmlTemplate_DocClass_Show::getDocClass()
	{
		XMLIDMLIB_Char			__value[513];
		XMLIMAPI_Id				__docClassId;
		XMLIMAPI_ErrorItem		__errorItem;
		XMLIMAPI_ErrorType		__errorType;
		XMLIMAPI_ErrorText		__errorMsg;
		XMLIDMLIB_Char			__msg[1024];
		
		// get __values from querystring
		XMLIDMLIB_GetParameterValue(TEXT_CONSTANT_PARAM_DOCCLASS_ID,_req,__value);
		strcpy(_docClass.docClassID,__value);

		XMLIMAPI_IdStringAsId(_docClass.docClassID,__docClassId);
		
		if (XMLIMAPI_DocClassGet(_xml,__docClassId,&_docClass)) {
		} else {
			XMLIMAPI_GetLastError(_xml,&__errorItem);
			XMLIMAPI_GetErrorType(__errorItem,&__errorType);
			XMLIMAPI_GetErrorText(__errorItem,&__errorMsg);
			sp77sprintf(__msg,1023,"[XMLIDMLIB::XMLIDMLib_HtmlTemplate_DocClass_Show::getDocClass] %s\n",__errorMsg);
			_wa.WriteLogMsg(__msg);
		}

	}

	void XMLIDMLib_HtmlTemplate_DocClass_Show::getAssignedXmlIndices()
	{
		XMLIMAPI_XmlIndex		__xmlIndex;
		XMLIMAPI_Id				__docClassId;
		XMLIMAPI_ErrorItem		__errorItem;
		XMLIMAPI_ErrorType		__errorType;
		XMLIMAPI_ErrorText		__errorMsg;
		XMLIDMLIB_Char			__msg[1024];
		XMLIDMLIB_Char			__temp[5001];
		XMLIDMLIB_Bool			__first;
		
		XMLIMAPI_IdStringAsId(_docClass.docClassID,__docClassId);

		if (XMLIMAPI_XmlIndexGetFirstByDocClass(_xml,__docClassId,&__xmlIndex)) {

			strcpy(_assignedXmlIndexesString,"%s%s%s");
			__first = XMLIDMLIB_True;
			strcpy(__temp,_assignedXmlIndexesString);
			do {
				XMLIMAPI_XmlIndexListAddItem(_assignedXmlIndices,&__xmlIndex);
				_assignedXmlIndicesCount++;	
				if (__first) {
					sp77sprintf(_assignedXmlIndexesString,5000,__temp,"",__xmlIndex.IndexID,"%s%s%s");
					__first = XMLIDMLIB_False;
				} else
					sp77sprintf(_assignedXmlIndexesString,5000,__temp,",",__xmlIndex.IndexID,"%s%s%s");
				strcpy(__temp,_assignedXmlIndexesString);
			} while (XMLIMAPI_XmlIndexGetNextByDocClass(_xml,&__xmlIndex));
			sp77sprintf(_assignedXmlIndexesString,5000,__temp,"","","");

			XMLIMAPI_XmlIndexListGetFirst(_assignedXmlIndices,&_currAssignedXmlIndex);

		} else {
			strcpy(_assignedXmlIndexesString,"");
			XMLIMAPI_GetLastError(_xml,&__errorItem);
			XMLIMAPI_GetErrorType(__errorItem,&__errorType);
			XMLIMAPI_GetErrorText(__errorItem,&__errorMsg);
			sp77sprintf(__msg,1023,"[XMLIDMLIB::XMLIDMLib_HtmlTemplate_DocClass_Show::getAssignedXmlIndices] %s\n",__errorMsg);
			_wa.WriteLogMsg(__msg);
		}

	}

	void XMLIDMLib_HtmlTemplate_DocClass_Show::addAssignedXmlIndex()
	{
		XMLIDMLIB_Char				__value[513];
		XMLIDMLIB_Char				__temp[5001];

		// take __values of allready assigned indices from querystring
		XMLIDMLIB_GetParameterValue(TEXT_CONSTANT_PARAM_DOCCLASS_ASSIGNABLE,_req,__value);
		if (strcmp(__value,"")!=0) {
			if (strcmp(_assignedXmlIndexesString,"")!=0) {
				/* add this id to the assignedXmlIndexString */
				sp77sprintf(__temp,5001,"%s,%s",_assignedXmlIndexesString,__value);
				strcpy(_assignedXmlIndexesString,__temp);
			} else {
				/* add this id to the assignedXmlIndexString */
				sp77sprintf(__temp,5001,"%s",__value);
				strcpy(_assignedXmlIndexesString,__temp);
			}
		}
		getAssignedXmlIndexes();
	}

	void XMLIDMLib_HtmlTemplate_DocClass_Show::removeAssignedXmlIndex()
	{
		XMLIMAPI_Id					__xmlIndexId;
		XMLIMAPI_XmlIndex			__xmlIndex;
		XMLIDMLIB_Char				*__temp1,*__temp2;

		/* delete all Xml Indexes, that are no longer assigned to a document class */
		/* this will only delete XML Indexes that are created during the DC Show or DC New dialog 
		   the formerly assigned XML Indexes are deleted at Create or Update */
		if (strcmp(_unAssignedXmlIndexesString,"")!=0) {
			__temp1 = _unAssignedXmlIndexesString;
			while(__temp2 = strchr(__temp1,',')) {
				if (__temp2-__temp1 == 48) {
					strncpy(__xmlIndex.IndexID,__temp1,__temp2-__temp1);
					__xmlIndex.IndexID[48] = '\0';
					XMLIMAPI_IdStringAsId(__xmlIndex.IndexID,__xmlIndexId);
					if (XMLIMAPI_XmlIndexCheckAssign(_xml,__xmlIndexId)) {
						/* no more assignments -> delete the xml index */
						XMLIMAPI_XmlIndexDelete(_xml,__xmlIndexId,XMLIDMLIB_True);
					}
				}
				__temp1 = __temp2+1;
			}
			if (strlen(__temp1)==48) {
				strcpy(__xmlIndex.IndexID,__temp1);
				__xmlIndex.IndexID[48] = '\0';
				XMLIMAPI_IdStringAsId(__xmlIndex.IndexID,__xmlIndexId);
				if (XMLIMAPI_XmlIndexCheckAssign(_xml,__xmlIndexId)) {
					/* no more assignments -> delete the xml index */
					XMLIMAPI_XmlIndexDelete(_xml,__xmlIndexId,XMLIDMLIB_True);
				}
			}
		}
		/* get the remaining Assigned XML Indexes */
		getAssignedXmlIndexes();
	}

	void XMLIDMLib_HtmlTemplate_DocClass_Show::removeAllAssignedXmlIndices()
	{
		XMLIMAPI_Id					__xmlIndexId;
		XMLIMAPI_XmlIndex			__xmlIndex;
		XMLIDMLIB_Char				*__temp1,*__temp2;

		/* delete all Xml Indexes, that are no longer assigned to a document class */
		/* this will only delete XML Indexes that are created during the DC Show or DC New dialog 
		   the formerly assigned XML Indexes are deleted at Create or Update */
		if (strcmp(_unAssignedXmlIndexesString,"")!=0) {
			__temp1 = _unAssignedXmlIndexesString;
			while(__temp2 = strchr(__temp1,',')) {
				if (__temp2-__temp1 == 48) {
					strncpy(__xmlIndex.IndexID,__temp1,__temp2-__temp1);
					__xmlIndex.IndexID[48] = '\0';
					XMLIMAPI_IdStringAsId(__xmlIndex.IndexID,__xmlIndexId);
					if (XMLIMAPI_XmlIndexCheckAssign(_xml,__xmlIndexId)) {
						/* no more assignments -> delete the xml index */
						XMLIMAPI_XmlIndexDelete(_xml,__xmlIndexId,XMLIDMLIB_True);
					}
				}
				__temp1 = __temp2+1;
			}
			if (strlen(__temp1)==48) {
				strcpy(__xmlIndex.IndexID,__temp1);
				__xmlIndex.IndexID[48] = '\0';
				XMLIMAPI_IdStringAsId(__xmlIndex.IndexID,__xmlIndexId);
				if (XMLIMAPI_XmlIndexCheckAssign(_xml,__xmlIndexId)) {
					/* no more assignments -> delete the xml index */
					XMLIMAPI_XmlIndexDelete(_xml,__xmlIndexId,XMLIDMLIB_True);
				}
			}
		}
		// clear the string of assigned xml indexes
		strcpy(_assignedXmlIndexesString,"");
	}

	void XMLIDMLib_HtmlTemplate_DocClass_Show::getXmlIndices()
	{
		XMLIMAPI_XmlIndex			__xmlIndex;
		XMLIMAPI_XmlIndexList		__exclude;
		XMLIMAPI_Id					__docClassId;
		XMLIMAPI_ErrorItem			__errorItem;
		XMLIMAPI_ErrorType			__errorType;
		XMLIMAPI_ErrorText			__errorMsg;
		XMLIDMLIB_Char				__msg[1024];
		XMLIDMLIB_Char				*__temp1,*__temp2;
		
		XMLIMAPI_IdStringAsId(_docClass.docClassID,__docClassId);

		XMLIMAPI_XmlIndexListCreate(&__exclude);
		if (strcmp(_assignedXmlIndexesString,"")!=0) {
			__temp1 = _assignedXmlIndexesString;
			while(__temp2 = strchr(__temp1,',')) {
				if (__temp2-__temp1 == 48) {
					strncpy(__xmlIndex.IndexID,__temp1,__temp2-__temp1);
					__xmlIndex.IndexID[48] = '\0';
					XMLIMAPI_XmlIndexListAddItem(__exclude,&__xmlIndex);
				}
				__temp1 = __temp2+1;
			}
			if (strlen(__temp1)==48) {
				strcpy(__xmlIndex.IndexID,__temp1);
				XMLIMAPI_XmlIndexListAddItem(__exclude,&__xmlIndex);
			}
		}

		if (XMLIMAPI_XmlIndexGetFirstByDocClassInfoExcl(_xml,
														 __docClassId,
														 &__xmlIndex,
														 __exclude)) {

			do {
				XMLIMAPI_XmlIndexListAddItem(_xmlIndexList,&__xmlIndex);
				_xmlIndexCount++;						
			} while (XMLIMAPI_XmlIndexGetNextByDocClassInfoExcl(_xml,&__xmlIndex));

		} else {
			XMLIMAPI_GetLastError(_xml,&__errorItem);
			XMLIMAPI_GetErrorType(__errorItem,&__errorType);
			XMLIMAPI_GetErrorText(__errorItem,&__errorMsg);
			sp77sprintf(__msg,1023,"[XMLIDMLIB::XMLIDMLib_HtmlTemplate_DocClass_Show::getXmlIndices] %s\n",__errorMsg);
			_wa.WriteLogMsg(__msg);
		}

		XMLIMAPI_XmlIndexListGetFirst(_xmlIndexList,&_currXmlIndex);

	}

	void XMLIDMLib_HtmlTemplate_DocClass_Show::getAssignedXmlIndexes()
	{
		XMLIMAPI_XmlIndex			__xmlIndex;
		XMLIMAPI_XmlIndexList		__include;
		XMLIMAPI_ErrorItem			__errorItem;
		XMLIMAPI_ErrorType			__errorType;
		XMLIMAPI_ErrorText			__errorMsg;
		XMLIDMLIB_Char				__msg[1024];
		XMLIDMLIB_Char				*__temp1,*__temp2;

		XMLIMAPI_XmlIndexListCreate(&__include);
		if (strcmp(_assignedXmlIndexesString,"")!=0) {
			__temp1 = _assignedXmlIndexesString;
			while(__temp2 = strchr(__temp1,',')) {
				if (__temp2-__temp1 == 48) {
					strncpy(__xmlIndex.IndexID,__temp1,__temp2-__temp1);
					__xmlIndex.IndexID[48] = '\0';
					XMLIMAPI_XmlIndexListAddItem(__include,&__xmlIndex);
				}
				__temp1 = __temp2+1;
			}
			if (strlen(__temp1)==48) {
				strcpy(__xmlIndex.IndexID,__temp1);
				XMLIMAPI_XmlIndexListAddItem(__include,&__xmlIndex);
			}
		}

		if (XMLIMAPI_XmlIndexGetFirstIncl(_xml,
										  &__xmlIndex,
										  __include)) {
			do {
				XMLIMAPI_XmlIndexListAddItem(_assignedXmlIndices,&__xmlIndex);
				_assignedXmlIndicesCount++;						
			} while (XMLIMAPI_XmlIndexGetNextIncl(_xml,&__xmlIndex));

		} else {
			XMLIMAPI_GetLastError(_xml,&__errorItem);
			XMLIMAPI_GetErrorType(__errorItem,&__errorType);
			XMLIMAPI_GetErrorText(__errorItem,&__errorMsg);
			sp77sprintf(__msg,1023,"[XMLIDMLIB::XMLIDMLib_HtmlTemplate_DocClass_Show::getAssignedXmlIndexes] %s\n",__errorMsg);
			_wa.WriteLogMsg(__msg);
		}

	}

