/****************************************************************************

  module      : XMLIDMLib_HtmlTemplate_XmlIndex_Show.cpp

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
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_HtmlTemplate_XmlIndex_Show.hpp"



	XMLIDMLib_HtmlTemplate_XmlIndex_Show::XMLIDMLib_HtmlTemplate_XmlIndex_Show (sapdbwa_WebAgent &wa,
																  sapdbwa_HttpRequest &req,
																  XMLIMAPI_Handle	&xml) 
		                  : _wa(wa), _req(req), _xml(xml), Tools_Template ( wa, (const unsigned char*)TEMPLATE_SHOWXMLINDEX )
	{ 
		XMLIMAPI_Char		__value[5001];

		// get values from querystring
		XMLIDMLIB_GetParameterValue(TEXT_CONSTANT_PARAM_DOCCLASS_ID,_req,__value);
  		strcpy(_docClass,__value);

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

		getXmlIndex(_req);

		strcpy(_serviceName,wa.GetServiceName());

	} 


	XMLIDMLib_HtmlTemplate_XmlIndex_Show::~XMLIDMLib_HtmlTemplate_XmlIndex_Show() 
	{
	}

	XMLIDMLIB_Int2 XMLIDMLib_HtmlTemplate_XmlIndex_Show :: askForWriteCount(const Tools_DynamicUTF8String & szName)
	{
		XMLIDMLIB_UInt4 __nReturn = 0;

		if (szName == TEMPLATE_KEY_XML_SHOW_INDEXNAME) {
			__nReturn = 1;
		} 
		else if (szName == TEMPLATE_KEY_XML_SHOW_INDEXID) {
			__nReturn = 1;
		}
		else if (szName == TEMPLATE_KEY_XML_SHOW_INDEXPROPID) {
			__nReturn = 1;
		}
		else if (szName == TEMPLATE_KEY_XML_SHOW_BASEPATH) {
			__nReturn = 1;
		}
		else if (szName == TEMPLATE_KEY_XML_SHOW_VALUEPATH) {
			__nReturn = 1;
		}
		else if (szName == TEMPLATE_KEY_XML_SHOW_DESCRIPTION) {
			__nReturn = 1;
		}
		else if (szName == TEMPLATE_KEY_XML_SHOW_SERVICE) {
			__nReturn = 1;
		}
		else if (szName == TEMPLATE_KEY_XML_SHOW_DOCCLASSNAME) {
			__nReturn = 1;
		} 
		else if (szName == TEMPLATE_KEY_XML_SHOW_DOCCLASSID) {
			__nReturn = 1;
		}
		else if (szName == TEMPLATE_KEY_XML_SHOW_DOCDESCRIPTION) {
			__nReturn = 1;
		}
		else if (szName == TEMPLATE_KEY_XML_SHOW_ASSIGNED_XMLINDEXES) {
			__nReturn = 1;
		}
		else if (szName == TEMPLATE_KEY_XML_SHOW_ASSIGNED_XMLINDEXES_CNT) {
			__nReturn = 1;
		}
		else if (szName == TEMPLATE_KEY_XML_SHOW_NEWLY_ADDED) {
			__nReturn = 1;
		}
		else if (szName == TEMPLATE_KEY_XML_SHOW_IDXSERVICE_ID) {
			if (strcmp(_idxServiceID,"")==0)
				return 0;
			else
				return 1;
		}
		else if (szName == TEMPLATE_KEY_XML_SHOW_IDXSERVICE_NAME) {
			if (strcmp(_idxServiceName,"")==0)
				return 0;
			else
				return 1;
		}
		else if (szName == TEMPLATE_KEY_XML_SHOW_IDXSERVICE_DESC) {
			if (strcmp(_idxServiceDesc,"")==0)
				return 0;
			else
				return 1;
		}
		else if (szName == TEMPLATE_KEY_XML_SHOW_IDXSERVICE_STORE) {
			if (strcmp(_store,"")==0)
				return 0;
			else
				return 1;
		}
		else if (szName == TEMPLATE_KEY_XML_SHOW_IDXSERVICE_DSSP) {
			if (strcmp(_docstore,"")==0)
				return 0;
			else
				return 1;
		}
		else if (szName == TEMPLATE_KEY_XML_SHOW_IDXSERVICE_ISSP) {
			if (strcmp(_idxstore,"")==0)
				return 0;
			else
				return 1;
		}
		else if (szName == TEMPLATE_KEY_XML_SHOW_IDXSERVICE_DSSPNEW) {
			if (strcmp(_dsspnew,"")==0)
				return 0;
			else
				return 1;
		}
		else if (szName == TEMPLATE_KEY_XML_SHOW_IDXSERVICE_ISSPNEW) {
			if (strcmp(_isspnew,"")==0)
				return 0;
			else
				return 1;
		}
		else if (szName == TEMPLATE_KEY_XML_SHOW_HEADING) {
			return 1;
		}

		return __nReturn;
	} 


	Tools_DynamicUTF8String XMLIDMLib_HtmlTemplate_XmlIndex_Show :: askForValue (const Tools_DynamicUTF8String & szName)
	{
		XMLIMAPI_Char	__value[5001];
		Tools_DynamicUTF8String __buffer;
		XMLIDMLIB_Char			__string[4096];

		if (szName == TEMPLATE_VALUE_XML_SHOW_INDEXNAME) {
			XMLIDMLIB_EscapeQuotes(_xmlIndex.indexName,strlen(_xmlIndex.indexName),__string);
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			return __buffer;
		} 
		else if (szName == TEMPLATE_VALUE_XML_SHOW_INDEXID) {
			__buffer.ConvertFromASCII_Latin1(_xmlIndex.IndexID,_xmlIndex.IndexID+strlen(_xmlIndex.IndexID));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_XML_SHOW_BASEPATH) {
			XMLIDMLIB_EscapeQuotes(_xmlIndex.xpathBasePath,strlen(_xmlIndex.xpathBasePath),__string);
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_XML_SHOW_VALUEPATH) {
			XMLIDMLIB_EscapeQuotes(_xmlIndex.xpathValuePath,strlen(_xmlIndex.xpathValuePath),__string);
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_XML_SHOW_DESCRIPTION) {
			XMLIDMLIB_EscapeQuotes(_xmlIndex.description,strlen(_xmlIndex.description),__string);
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_XML_SHOW_DOCCLASSID) {
			__buffer.ConvertFromASCII_Latin1(_docClass,_docClass+strlen(_docClass));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_XML_SHOW_DOCCLASSNAME) {
			XMLIDMLIB_GetParameterValue(TEXT_CONSTANT_PARAM_DOCCLASS_NAME,_req,__value);
			if (strcmp(__value,"")!=0)
				XMLIDMLIB_EscapeQuotes(__value,strlen(__value),__string);
			else
				strcpy(__string,"New Document Class");
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_XML_SHOW_DOCDESCRIPTION) {
			XMLIDMLIB_GetParameterValue(TEXT_CONSTANT_PARAM_DOCCLASS_DESCRIPTION,_req,__value);
			XMLIDMLIB_EscapeQuotes(__value,strlen(__value),__string);
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_XML_SHOW_ASSIGNED_XMLINDEXES) {
			XMLIDMLIB_GetParameterValue(TEXT_CONSTANT_PARAM_DOCCLASS_ASSIGNED_XML_INDEXES,_req,__value);
			XMLIDMLIB_EscapeQuotes(__value,strlen(__value),__string);
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_XML_SHOW_ASSIGNED_XMLINDEXES_CNT) {
			XMLIDMLIB_GetParameterValue(TEXT_CONSTANT_PARAM_DOCCLASS_ASSIGNED_XML_INDEXES_CNT,_req,__value);
			XMLIDMLIB_EscapeQuotes(__value,strlen(__value),__string);
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_XML_SHOW_NEWLY_ADDED) {
			XMLIDMLIB_GetParameterValue(TEXT_CONSTANT_PARAM_DOCCLASS_NEWLY_ADDED,_req,__value);
			XMLIDMLIB_EscapeQuotes(__value,strlen(__value),__string);
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_XML_SHOW_SERVICE) {
			return _serviceName;
		} else if (szName == TEMPLATE_VALUE_XML_SHOW_IDXSERVICE_ID) {
			XMLIDMLIB_EscapeQuotes(_idxServiceID,strlen(_idxServiceID),__string);
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_XML_SHOW_IDXSERVICE_NAME) {
			XMLIDMLIB_EscapeQuotes(_idxServiceName,strlen(_idxServiceName),__string);
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_XML_SHOW_IDXSERVICE_DESC) {
			XMLIDMLIB_EscapeQuotes(_idxServiceDesc,strlen(_idxServiceDesc),__string);
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_XML_SHOW_IDXSERVICE_STORE) {
			__buffer.ConvertFromASCII_Latin1(_store,_store+strlen(_store));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_XML_SHOW_IDXSERVICE_DSSP) {
			__buffer.ConvertFromASCII_Latin1(_docstore,_docstore+strlen(_docstore));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_XML_SHOW_IDXSERVICE_ISSP) {
			__buffer.ConvertFromASCII_Latin1(_idxstore,_idxstore+strlen(_idxstore));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_XML_SHOW_IDXSERVICE_DSSPNEW) {
			__buffer.ConvertFromASCII_Latin1(_dsspnew,_dsspnew+strlen(_dsspnew));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_XML_SHOW_IDXSERVICE_ISSPNEW) {
			__buffer.ConvertFromASCII_Latin1(_isspnew,_isspnew+strlen(_isspnew));
			return __buffer;
		}
		return "";
	}

	void XMLIDMLib_HtmlTemplate_XmlIndex_Show::getXmlIndex(sapdbwa_HttpRequest &req)
	{
		XMLIDMLIB_Char			__value[513];
		XMLIMAPI_Id				__xmlIndexId;
		XMLIMAPI_ErrorItem		__errorItem;
		XMLIMAPI_ErrorType		__errorType;
		XMLIMAPI_ErrorText		__errorMsg;
		XMLIDMLIB_Char			__msg[1024];
		
		// get values from querystring
		XMLIDMLIB_GetParameterValue(TEXT_CONSTANT_PARAM_XMLINDEX_NAME,_req,__value);
  		strcpy(_xmlIndex.indexName,__value);

		// get values from querystring
		XMLIDMLIB_GetParameterValue(TEXT_CONSTANT_PARAM_XMLINDEX_ID,_req,__value);
  		strcpy(_xmlIndex.IndexID,__value);

		XMLIMAPI_IdStringAsId(_xmlIndex.IndexID,__xmlIndexId);
		
		if (!XMLIMAPI_XmlIndexGet(_xml,__xmlIndexId,&_xmlIndex)) {
			XMLIMAPI_GetLastError(_xml,&__errorItem);
			XMLIMAPI_GetErrorType(__errorItem,&__errorType);
			XMLIMAPI_GetErrorText(__errorItem,&__errorMsg);
			sp77sprintf(__msg,1023,"[XMLIDMLIB::XMLIDMLib_HtmlTemplate_XmlIndex_Show::getXmlIndex] %s\n",__errorMsg);
			_wa.WriteLogMsg(__msg);
		}

	}
