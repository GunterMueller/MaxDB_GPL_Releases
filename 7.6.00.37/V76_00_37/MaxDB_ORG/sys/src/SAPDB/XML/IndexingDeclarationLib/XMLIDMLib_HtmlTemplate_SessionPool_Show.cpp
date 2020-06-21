/****************************************************************************

  module      : XMLIDMLib_HtmlTemplate_SessionPool_Show.cpp

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
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_HtmlTemplate_SessionPool_Show.hpp"


	XMLIDMLib_HtmlTemplate_SessionPool_Show::XMLIDMLib_HtmlTemplate_SessionPool_Show (sapdbwa_WebAgent &wa,
																  sapdbwa_HttpRequest &req,
																  XMLIMAPI_Handle	&xml) 
		                  : _wa(wa), _req(req), _xml(xml), Tools_Template ( wa, (const unsigned char*)TEMPLATE_SESSIONPOOL_SHOW )
	{ 
		XMLIDMLIB_Char	__value[5001];

		getSessionPool();

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
		XMLIDMLIB_GetParameterValue(TEXT_CONSTANT_PARAM_IDXSERVICE_SYNC,req,__value);
		strncpy(_issync,__value,2);
		_issync[2]='\0';
		XMLIDMLIB_GetParameterValue(TEXT_CONSTANT_PARAM_IDXSERVICE_DSSPNEW,req,__value);
		strncpy(_dsspnew,__value,2);
		_dsspnew[2]='\0';
		XMLIDMLIB_GetParameterValue(TEXT_CONSTANT_PARAM_IDXSERVICE_ISSPNEW,req,__value);
		strncpy(_isspnew,__value,2);
		_isspnew[2]='\0';

		_wa.GetSessionPool(_pool);

	} 


	XMLIDMLib_HtmlTemplate_SessionPool_Show::~XMLIDMLib_HtmlTemplate_SessionPool_Show() 
	{
	}

	XMLIDMLIB_Int2 XMLIDMLib_HtmlTemplate_SessionPool_Show :: askForWriteCount(const Tools_DynamicUTF8String & szName)
	{
		XMLIDMLIB_UInt4 _nReturn = 0;

		if (szName == TEMPLATE_KEY_SP_SHOW_NAME) {
			_nReturn = 1;
		} 
		else if (szName == TEMPLATE_KEY_SP_SHOW_IDXSERVICE_ID) {
			if (strcmp(_idxServiceID,"")==0)
				return 0;
			else
				return 1;
		}
		else if (szName == TEMPLATE_KEY_SP_SHOW_IDXSERVICE_NAME) {
			if (strcmp(_idxServiceName,"")==0)
				return 0;
			else
				return 1;
		}
		else if (szName == TEMPLATE_KEY_SP_SHOW_IDXSERVICE_DESC) {
			if (strcmp(_idxServiceDesc,"")==0)
				return 0;
			else
				return 1;
		}
		else if (szName == TEMPLATE_KEY_SP_SHOW_IDXSERVICE_STORE) {
			if (strcmp(_store,"")==0)
				return 0;
			else
				return 1;
		}
		else if (szName == TEMPLATE_KEY_SP_SHOW_IDXSERVICE_DSSP) {
			if (strcmp(_docstore,"")==0)
				return 0;
			else
				return 1;
		}
		else if (szName == TEMPLATE_KEY_SP_SHOW_IDXSERVICE_ISSP) {
			if (strcmp(_idxstore,"")==0)
				return 0;
			else
				return 1;
		}
		else if (szName == TEMPLATE_KEY_SP_SHOW_IDXSERVICE_DSSPNEW) {
			if (strcmp(_dsspnew,"")==0)
				return 0;
			else
				return 1;
		}
		else if (szName == TEMPLATE_KEY_SP_SHOW_IDXSERVICE_ISSPNEW) {
			if (strcmp(_isspnew,"")==0)
				return 0;
			else
				return 1;
		}
		else if (szName == TEMPLATE_KEY_SP_SHOW_ID) {
			_nReturn = 1;
		}
		else if (szName == TEMPLATE_KEY_SP_SHOW_DESCRIPTION) {
			if (strcmp(_issync,"0")==0)
				_nReturn = 1;
			else
				_nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_SP_SHOW_DBNODE) {
			if (strcmp(_issync,"0")==0)
				_nReturn = 1;
			else
				_nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_SP_SHOW_DBNAME) {
			if (strcmp(_issync,"0")==0)
				_nReturn = 1;
			else
				_nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_SP_SHOW_DBUSER) {
			if (strcmp(_issync,"0")==0)
				_nReturn = 1;
			else
				_nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_SP_SHOW_DBPWD) {
			if (strcmp(_issync,"0")==0)
				_nReturn = 1;
			else
				_nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_SP_SHOW_POOLSIZE) {
			if (strcmp(_issync,"0")==0)
				_nReturn = 1;
			else
				_nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_SP_SHOW_TRACEFILE) {
			if (strcmp(_issync,"0")==0)
				_nReturn = 1;
			else
				_nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_SP_SHOW_DESCRIPTION_SHOW) {
			if (strcmp(_issync,"1")==0)
				_nReturn = 1;
			else
				_nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_SP_SHOW_DBNODE_SHOW) {
			if (strcmp(_issync,"1")==0)
				_nReturn = 1;
			else
				_nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_SP_SHOW_DBNAME_SHOW) {
			if (strcmp(_issync,"1")==0)
				_nReturn = 1;
			else
				_nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_SP_SHOW_DBUSER_SHOW) {
			if (strcmp(_issync,"1")==0)
				_nReturn = 1;
			else
				_nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_SP_SHOW_DBPWD_SHOW) {
			if (strcmp(_issync,"1")==0)
				_nReturn = 1;
			else
				_nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_SP_SHOW_TRACEFILE_SHOW) {
			if (strcmp(_issync,"1")==0)
				_nReturn = 1;
			else
				_nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_SP_SHOW_SHOW) {
			if (strcmp(_issync,"1")==0)
				_nReturn = 1;
			else
				_nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_SP_SHOW_CHANGE) {
			if (strcmp(_issync,"0")==0)
				_nReturn = 1;
			else
				_nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_SP_SHOW_WEBDBM) {
			_nReturn = 1;
		}
		else if (szName == TEMPLATE_KEY_SP_SHOW_SERVICE) {
			_nReturn = 1;
		}
		else if (szName == TEMPLATE_KEY_SP_SHOW_HEADING) {
			if (strcmp(_issync,"0")==0)
				_nReturn = 1;
			else
				_nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_SP_SHOW_HEADING_SHOW) {
			if (strcmp(_issync,"1")==0)
				_nReturn = 1;
			else
				_nReturn = 0;
		}
		return _nReturn;
	} 


	Tools_DynamicUTF8String XMLIDMLib_HtmlTemplate_SessionPool_Show :: askForValue (const Tools_DynamicUTF8String & szName)
	{
		XMLIMAPI_Char	__string[4096];
		Tools_DynamicUTF8String __buffer;

		if (szName == TEMPLATE_VALUE_SP_SHOW_NAME) {
			if (strcmp(_issync,"0")==0) {
				XMLIDMLIB_EscapeQuotes(_sessionPool.PoolName,strlen(_sessionPool.PoolName),__string);
				__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			} else {
				strcpy(__string,"WebDAV Session Pool");
				__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			}
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_SP_SHOW_IDXSERVICE_ID) {
			__buffer.ConvertFromASCII_Latin1(_idxServiceID,_idxServiceID+strlen(_idxServiceID));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_SP_SHOW_IDXSERVICE_NAME) {
			XMLIDMLIB_EscapeQuotes(_idxServiceName,strlen(_idxServiceName),__string);
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_SP_SHOW_IDXSERVICE_DESC) {
			XMLIDMLIB_EscapeQuotes(_idxServiceDesc,strlen(_idxServiceDesc),__string);
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_SP_SHOW_IDXSERVICE_STORE) {
			__buffer.ConvertFromASCII_Latin1(_store,_store+strlen(_store));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_SP_SHOW_IDXSERVICE_DSSP) {
			__buffer.ConvertFromASCII_Latin1(_docstore,_docstore+strlen(_docstore));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_SP_SHOW_IDXSERVICE_ISSP) {
			__buffer.ConvertFromASCII_Latin1(_idxstore,_idxstore+strlen(_idxstore));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_SP_SHOW_IDXSERVICE_DSSPNEW) {
			__buffer.ConvertFromASCII_Latin1(_dsspnew,_dsspnew+strlen(_dsspnew));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_SP_SHOW_IDXSERVICE_ISSPNEW) {
			__buffer.ConvertFromASCII_Latin1(_isspnew,_isspnew+strlen(_isspnew));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_SP_SHOW_STORE) {
			if (strcmp(_store,"1")==0)
				sp77sprintf(__string,4096,"Document");
			else
				sp77sprintf(__string,4096,"Index");
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_SP_SHOW_ID) {
			__buffer.ConvertFromASCII_Latin1(_sessionPool.PoolID,_sessionPool.PoolID+strlen(_sessionPool.PoolID));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_SP_SHOW_DESCRIPTION) {
			if (strcmp(_issync,"0")==0) {
				XMLIDMLIB_EscapeQuotes(_sessionPool.description,strlen(_sessionPool.description),__string);
				__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			} else {
				strcpy(__string,"Session Pool of the WebDAV Service as given in waadmin.wa");
				__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			}
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_SP_SHOW_DBNODE) {
			if (strcmp(_issync,"0")==0) {
				XMLIDMLIB_EscapeQuotes(_sessionPool.DbNode,strlen(_sessionPool.DbNode),__string);
				__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			} else {
				strcpy(__string,_pool->GetServerNode());
				__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			}
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_SP_SHOW_DBNAME) {
			if (strcmp(_issync,"0")==0) {
				XMLIDMLIB_EscapeQuotes(_sessionPool.DbName,strlen(_sessionPool.DbName),__string);
				__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			} else {
				strcpy(__string,_pool->GetServerDb());
				__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			}
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_SP_SHOW_DBUSER) {
			if (strcmp(_issync,"0")==0) {
				XMLIDMLIB_EscapeQuotes(_sessionPool.DbUser,strlen(_sessionPool.DbUser),__string);
				__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			} else {
				strcpy(__string,_pool->GetUserName());
				__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			}
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_SP_SHOW_DBPWD) {
			if (strcmp(_issync,"0")==0) {
				XMLIDMLIB_EscapeQuotes(_sessionPool.DbPassword,strlen(_sessionPool.DbPassword),__string);
				__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			} else {
				strcpy(__string,_pool->GetPassword());
				__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			}
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_SP_SHOW_TRACEFILE) {
			if (strcmp(_issync,"0")==0) {
				XMLIDMLIB_EscapeQuotes(_sessionPool.TraceFile,strlen(_sessionPool.TraceFile),__string);
				__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			} else {
				strcpy(__string,"");
				__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			}
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_SP_SHOW_WEBDBM) {
			if (strcmp(_issync,"0")==0) {
				sp77sprintf(__string,4096,TEXT_CONSTANT_HTML_SESSIONPOOL_WEBDBM,
							_sessionPool.DbNode,
							_sessionPool.DbName);
			} else {
				sp77sprintf(__string,4096,TEXT_CONSTANT_HTML_SESSIONPOOL_WEBDBM,
							_pool->GetServerNode(),
							_pool->GetServerDb());
			}
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_SP_SHOW_SERVICE) {
			__buffer.ConvertFromASCII_Latin1(_serviceName,_serviceName+strlen(_serviceName));
			return __buffer;
		}
		return "";
	}

	void XMLIDMLib_HtmlTemplate_SessionPool_Show::getSessionPool()
	{
		XMLIDMLIB_Char			__value[513];
		XMLIMAPI_Id				__sessionPoolId;
		XMLIMAPI_ErrorItem		__errorItem;
		XMLIMAPI_ErrorType		__errorType;
		XMLIMAPI_ErrorText		__errorMsg;
		XMLIDMLIB_Char			__msg[1024];
		
		// get values from querystring
		XMLIDMLIB_GetParameterValue(TEXT_CONSTANT_PARAM_SESSIONPOOL_ID,_req,__value);
		strcpy(_sessionPool.PoolID,__value);

		XMLIMAPI_IdStringAsId(_sessionPool.PoolID,__sessionPoolId);
		
		if (XMLIMAPI_SessionPoolGet(_xml,__sessionPoolId,&_sessionPool)) {
		} else {
			XMLIMAPI_GetLastError(_xml,&__errorItem);
			XMLIMAPI_GetErrorType(__errorItem,&__errorType);
			XMLIMAPI_GetErrorText(__errorItem,&__errorMsg);
			sp77sprintf(__msg,1023,"[XMLIDMLIB::XMLIDMLib_HtmlTemplate_SessionPool_Show::getSessionPool] %s\n",__errorMsg);
			_wa.WriteLogMsg(__msg);
		}

	}

