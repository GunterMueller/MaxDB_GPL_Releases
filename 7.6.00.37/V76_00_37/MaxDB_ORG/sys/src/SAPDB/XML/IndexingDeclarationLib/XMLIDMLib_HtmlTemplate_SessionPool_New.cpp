/****************************************************************************

  module      : XMLIDMLib_HtmlTemplate_SessionPool_New.cpp

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-08-22  14:32
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
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_HtmlTemplate_SessionPool_New.hpp"



	XMLIDMLib_HtmlTemplate_SessionPool_New::XMLIDMLib_HtmlTemplate_SessionPool_New (sapdbwa_WebAgent &wa, sapdbwa_HttpRequest req) 
		                  : Tools_Template ( wa, (const unsigned char*)TEMPLATE_SESSIONPOOL_NEW )
	{ 
		XMLIDMLIB_Char	__value[5001];
		strcpy(_serviceName,wa.GetServiceName());
		// get values from querystring
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
		XMLIDMLIB_GetParameterValue(TEXT_CONSTANT_PARAM_IDXSERVICE_DOCCLASS,req,__value);
		strncpy(_adc,__value,129);
		_adc[129]='\0';

		/* get the Session Pool of the WebDAV service */
		wa.GetSessionPool(_pSessionPool);


	} 


	XMLIDMLib_HtmlTemplate_SessionPool_New::~XMLIDMLib_HtmlTemplate_SessionPool_New() 
	{
	}

	XMLIDMLIB_Int2 XMLIDMLib_HtmlTemplate_SessionPool_New :: askForWriteCount(const Tools_DynamicUTF8String & szName)
	{
		if (szName == TEMPLATE_KEY_SP_NEW_SERVICE)
			return 1;
		else if (szName == TEMPLATE_KEY_SP_NEW_IDXSERVICE_ID) {
			if (strcmp(_idxServiceID,"")==0)
				return 0;
			else
				return 1;
		}
		else if (szName == TEMPLATE_KEY_SP_NEW_IDXSERVICE_NAME) {
			if (strcmp(_idxServiceName,"")==0)
				return 0;
			else
				return 1;
		}
		else if (szName == TEMPLATE_KEY_SP_NEW_IDXSERVICE_DESC) {
			if (strcmp(_idxServiceDesc,"")==0)
				return 0;
			else
				return 1;
		}
		else if (szName == TEMPLATE_KEY_SP_NEW_IDXSERVICE_STORE) {
			if (strcmp(_store,"")==0)
				return 0;
			else
				return 1;
		}
		else if (szName == TEMPLATE_KEY_SP_NEW_IDXSERVICE_DSSP) {
			if (strcmp(_docstore,"")==0)
				return 0;
			else
				return 1;
		}
		else if (szName == TEMPLATE_KEY_SP_NEW_IDXSERVICE_ISSP) {
			if (strcmp(_idxstore,"")==0)
				return 0;
			else
				return 1;
		}
		else if (szName == TEMPLATE_KEY_SP_NEW_IDXSERVICE_DOCCLASS) {
			if (strcmp(_adc,"")==0)
				return 0;
			else
				return 1;
		}
		else if (szName == TEMPLATE_KEY_SP_NEW_DBNODE) {
			return 1;
		}
		else if (szName == TEMPLATE_KEY_SP_NEW_DBNAME) {
			return 1;
		}
		else if (szName == TEMPLATE_KEY_SP_NEW_DBUSER) {
			return 1;
		}
		else if (szName == TEMPLATE_KEY_SP_NEW_DBPWD) {
			return 1;
		}
		else if (szName == TEMPLATE_KEY_SP_NEW_IDXSERVICE_DSSPNEW) {
			if (strcmp(_dsspnew,"")==0)
				return 0;
			else
				return 1;
		}
		else if (szName == TEMPLATE_KEY_SP_NEW_IDXSERVICE_ISSPNEW) {
			if (strcmp(_isspnew,"")==0)
				return 0;
			else
				return 1;
		}
		else if (szName == TEMPLATE_KEY_SP_NEW_HEADING) {
			return 1;
		}
		return 0;
	} 


	Tools_DynamicUTF8String XMLIDMLib_HtmlTemplate_SessionPool_New :: askForValue (const Tools_DynamicUTF8String & szName)
	{
		Tools_DynamicUTF8String __buffer;
		const XMLIDMLIB_Char	*__string;
		XMLIDMLIB_Char			__string2[4096];

		if (szName == TEMPLATE_VALUE_SP_NEW_SERVICE) {
			__buffer.ConvertFromASCII_Latin1(_serviceName,_serviceName+strlen(_serviceName));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_SP_NEW_IDXSERVICE_ID) {
			__buffer.ConvertFromASCII_Latin1(_idxServiceID,_idxServiceID+strlen(_idxServiceID));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_SP_NEW_IDXSERVICE_NAME) {
			XMLIDMLIB_EscapeQuotes(_idxServiceName,strlen(_idxServiceName),__string2);
			__buffer.ConvertFromASCII_Latin1(__string2,__string2+strlen(__string2));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_SP_NEW_IDXSERVICE_DESC) {
			XMLIDMLIB_EscapeQuotes(_idxServiceDesc,strlen(_idxServiceDesc),__string2);
			__buffer.ConvertFromASCII_Latin1(__string2,__string2+strlen(__string2));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_SP_NEW_IDXSERVICE_STORE) {
			__buffer.ConvertFromASCII_Latin1(_store,_store+strlen(_store));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_SP_NEW_IDXSERVICE_DSSP) {
			__buffer.ConvertFromASCII_Latin1(_docstore,_docstore+strlen(_docstore));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_SP_NEW_IDXSERVICE_ISSP) {
			__buffer.ConvertFromASCII_Latin1(_idxstore,_idxstore+strlen(_idxstore));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_SP_NEW_IDXSERVICE_DSSPNEW) {
			__buffer.ConvertFromASCII_Latin1(_dsspnew,_dsspnew+strlen(_dsspnew));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_SP_NEW_IDXSERVICE_ISSPNEW) {
			__buffer.ConvertFromASCII_Latin1(_isspnew,_isspnew+strlen(_isspnew));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_SP_NEW_IDXSERVICE_DOCCLASS) {
			__buffer.ConvertFromASCII_Latin1(_adc,_adc+strlen(_adc));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_SP_NEW_STORE) {
			if (strcmp(_store,"1")==0)
				sp77sprintf(__string2,4096,"Document");
			else
				sp77sprintf(__string2,4096,"Index");
			__buffer.ConvertFromASCII_Latin1(__string2,__string2+strlen(__string2));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_SP_NEW_DBNODE) {
			__string = _pSessionPool->GetServerNode();
			XMLIDMLIB_EscapeQuotes((XMLIDMLIB_Char*)__string,strlen(__string),__string2);
			__buffer.ConvertFromASCII_Latin1(__string2,__string2+strlen(__string2));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_SP_NEW_DBNAME) {
			__string = _pSessionPool->GetServerDb();
			XMLIDMLIB_EscapeQuotes((XMLIDMLIB_Char*)__string,strlen(__string),__string2);
			__buffer.ConvertFromASCII_Latin1(__string2,__string2+strlen(__string2));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_SP_NEW_DBUSER) {
			__string = _pSessionPool->GetUserName();
			XMLIDMLIB_EscapeQuotes((XMLIDMLIB_Char*)__string,strlen(__string),__string2);
			__buffer.ConvertFromASCII_Latin1(__string2,__string2+strlen(__string2));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_SP_NEW_DBPWD) {
			__string = _pSessionPool->GetPassword();
			XMLIDMLIB_EscapeQuotes((XMLIDMLIB_Char*)__string,strlen(__string),__string2);
			__buffer.ConvertFromASCII_Latin1(__string2,__string2+strlen(__string2));
			return __buffer;
		}
		return "";
	}

