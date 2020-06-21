/****************************************************************************

  module      : XMLIDMLib_HtmlTemplate_IndexingService_Show.cpp

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
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_HtmlTemplate_IndexingService_Show.hpp"


	XMLIDMLib_HtmlTemplate_IndexingService_Show::XMLIDMLib_HtmlTemplate_IndexingService_Show (sapdbwa_WebAgent &wa,
																  sapdbwa_HttpRequest &req,
																  XMLIMAPI_Handle	&xml) 
		                  : _wa(wa), _req(req), _xml(xml), Tools_Template ( wa, (const unsigned char*)TEMPLATE_IDXSERVICE_SHOW )
	{ 
		XMLIDMLIB_Char			__value[5001];
		XMLIMAPI_SessionPoolListCreate(&_spList);
		_spCount = 0;
		_count=0;

		XMLIMAPI_DocClassListCreate(&_docClassList);
		_docClassCount = 0;
		_firstDocClass = XMLIMAPI_True;

		getSpList();
		getService();
		getDocClasses();

		strcpy(_serviceName,wa.GetServiceName());
		XMLIDMLIB_GetParameterValue(TEXT_CONSTANT_PARAM_SESSIONPOOL_NAME,_req,__value);
		strcpy(_spName,__value);
		XMLIDMLIB_GetParameterValue(TEXT_CONSTANT_PARAM_IDXSERVICE_STORE,_req,__value);
		strcpy(_store,__value);
		XMLIDMLIB_GetParameterValue(TEXT_CONSTANT_PARAM_IDXSERVICE_DOCSTORE,_req,__value);
		strcpy(_docstore,__value);
		XMLIDMLIB_GetParameterValue(TEXT_CONSTANT_PARAM_IDXSERVICE_IDXSTORE,_req,__value);
		strcpy(_idxstore,__value);
		XMLIDMLIB_GetParameterValue(TEXT_CONSTANT_PARAM_IDXSERVICE_DSSPNEW,req,__value);
		strcpy(_dsspnew,__value);
		XMLIDMLIB_GetParameterValue(TEXT_CONSTANT_PARAM_IDXSERVICE_ISSPNEW,req,__value);
		strcpy(_isspnew,__value);
		XMLIDMLIB_GetParameterValue(TEXT_CONSTANT_PARAM_IDXSERVICE_DOCCLASS,req,__value);
		strcpy(_adc,__value);
		XMLIDMLIB_GetParameterValue(TEXT_CONSTANT_PARAM_DOCCLASS_ID,req,__value);
		strcpy(_docclassid,__value);
		XMLIDMLIB_GetParameterValue(TEXT_CONSTANT_PARAM_CANCEL,_req,__value);
		if (strcmp(__value,"")==0)
			_cancel = XMLIDMLIB_False;
		else
			_cancel = XMLIDMLIB_True;
		XMLIDMLIB_GetParameterValue(TEXT_CONSTANT_PARAM_IDXSERVICE_INIT,_req,__value);
		if (strcmp(__value,"")==0)
			_init = XMLIDMLIB_False;
		else
			_init = XMLIDMLIB_True;

		strcpy(_serviceName,wa.GetServiceName());
	} 


	XMLIDMLib_HtmlTemplate_IndexingService_Show::~XMLIDMLib_HtmlTemplate_IndexingService_Show() 
	{
		XMLIMAPI_DocClassListDestroy(_docClassList);
		XMLIMAPI_SessionPoolListDestroy(_spList);
	}

	XMLIDMLIB_Int2 XMLIDMLib_HtmlTemplate_IndexingService_Show :: askForWriteCount(const Tools_DynamicUTF8String & szName)
	{
		XMLIDMLIB_UInt4 __nReturn = 0;

		if (szName == TEMPLATE_KEY_IS_SHOW_SERVICE) {
			__nReturn = 1;
		}
		else if (szName == TEMPLATE_KEY_IS_SHOW_SERVICE_ID) {
			__nReturn = 1;
		}
		else if (szName == TEMPLATE_KEY_IS_SHOW_SERVICE_NAME) {
			__nReturn = 1;
		}
		else if (szName == TEMPLATE_KEY_IS_SHOW_SERVICE_DESC) {
			__nReturn = 1;
		}
		else if (szName == TEMPLATE_KEY_IS_SHOW_SERVICE_SYNC) {
			__nReturn = 1;
		}
		else if (szName == TEMPLATE_KEY_IS_SHOW_DOCSTORE) {
			__nReturn = _spCount;
		}
		else if (szName == TEMPLATE_KEY_IS_SHOW_IDXSTORE) {
			__nReturn = _spCount;
		}
		else if (szName == TEMPLATE_KEY_IS_SHOW_SPARRAY) {
			__nReturn = _spCount;
		}
		else if (szName == TEMPLATE_KEY_IS_SHOW_LAST_DOCSTORE) {
			__nReturn = 1;
		}
		else if (szName == TEMPLATE_KEY_IS_SHOW_LAST_IDXSTORE) {
			__nReturn = 1;
		}
		else if (szName == TEMPLATE_KEY_IS_SHOW_NEW_DOCSTORE) {
			__nReturn = 1;
		}
		else if (szName == TEMPLATE_KEY_IS_SHOW_NEW_IDXSTORE) {
			__nReturn = 1;
		}
		else if (szName == TEMPLATE_KEY_IS_SHOW_ASYNC) {
			if (strcmp(_serviceDesc.isSynchronous,"1")!=0)
				__nReturn = 1;
			else
				__nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_IS_SHOW_DSASYNC) {
			if (strcmp(_serviceDesc.isSynchronous,"1")!=0)
				__nReturn = 1;
			else
				__nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_IS_SHOW_DSSYNC) {
			if (strcmp(_serviceDesc.isSynchronous,"1")==0)
				__nReturn = 1;
			else
				__nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_IS_SHOW_ASSIGNED_DOCCLASSES) {
			__nReturn = _docClassCount;
		}
		return __nReturn;
	} 


	Tools_DynamicUTF8String XMLIDMLib_HtmlTemplate_IndexingService_Show :: askForValue (const Tools_DynamicUTF8String & szName)
	{
		Tools_DynamicUTF8String __buffer;
		XMLIDMLIB_Char			__string[9000];
		XMLIDMLIB_Char			__string1[4096];
		XMLIDMLIB_Char			__string2[4096];

		if (szName == TEMPLATE_VALUE_IS_SHOW_DOCSTORE_ID) {
			if (_count==0) 
				XMLIMAPI_SessionPoolListGetFirst(_spList,&_currSP);
			_count++;
			__buffer.ConvertFromASCII_Latin1(_currSP->PoolID,_currSP->PoolID+strlen(_currSP->PoolID));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_IS_SHOW_DOCSTORE_NAME) {
			XMLIDMLIB_EscapeQuotes(_currSP->PoolName,strlen(_currSP->PoolName),__string);
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			XMLIMAPI_SessionPoolListGetNext(_spList,&_currSP);
			if (_count==_spCount)
				_count=0;
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_IS_SHOW_DOCSTORE_SEL) {
			if (_init){
				if (strcmp(_currSP->PoolID,_serviceDesc.DocStoreID)==0)
					return "selected";
				else
					return "";
			} else {
				if (strcmp(_store,"1")!=0  || _cancel) {
					if (strcmp(_currSP->PoolID,_docstore)==0)
						return "selected";
					else
						return "";
				} else if (strcmp(_spName,"")!=0) {
					if (strcmp(_currSP->PoolName,_spName)==0)
						return "selected";
					else
						return "";
				} else {
					if (strcmp(_currSP->PoolID,_serviceDesc.DocStoreID)==0)
						return "selected";
					else
						return "";
				}
			}
		}
		else if (szName == TEMPLATE_VALUE_IS_SHOW_IDXSTORE_ID) {
			if (_count==0) 
				XMLIMAPI_SessionPoolListGetFirst(_spList,&_currSP);
			_count++;
			__buffer.ConvertFromASCII_Latin1(_currSP->PoolID,_currSP->PoolID+strlen(_currSP->PoolID));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_IS_SHOW_IDXSTORE_SEL) {
			if (_init) {
				if (strcmp(_currSP->PoolID,_serviceDesc.IdxStoreID)==0)
					return "selected";
				else
					return "";
			} else {
				if (strcmp(_store,"2")!=0  || _cancel) {
					if (strcmp(_currSP->PoolID,_idxstore)==0)
						return "selected";
					else
						return "";
				} else if (strcmp(_spName,"")!=0) {
					if (strcmp(_currSP->PoolName,_spName)==0)
						return "selected";
					else
						return "";
				} else {
					if (strcmp(_currSP->PoolID,_serviceDesc.IdxStoreID)==0)
						return "selected";
					else
						return "";
				}
			}
		}
		else if (szName == TEMPLATE_VALUE_IS_SHOW_IDXSTORE_NAME) {
			XMLIDMLIB_EscapeQuotes(_currSP->PoolName,strlen(_currSP->PoolName),__string);
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			XMLIMAPI_SessionPoolListGetNext(_spList,&_currSP);
			if (_count==_spCount)
				_count=0;
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_IS_SHOW_SERVICE_ID) {
			__buffer.ConvertFromASCII_Latin1(_serviceDesc.ServiceID,_serviceDesc.ServiceID+strlen(_serviceDesc.ServiceID));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_IS_SHOW_SERVICE_NAME) {
			XMLIDMLIB_EscapeQuotes(_serviceDesc.ServiceName,strlen(_serviceDesc.ServiceName),__string);
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_IS_SHOW_SERVICE_DESC) {
			XMLIDMLIB_EscapeQuotes(_serviceDesc.ServiceDesc,strlen(_serviceDesc.ServiceDesc),__string);
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_IS_SHOW_SERVICE_SYNC) {
			__buffer.ConvertFromASCII_Latin1(_serviceDesc.isSynchronous,_serviceDesc.isSynchronous+strlen(_serviceDesc.isSynchronous));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_IS_SHOW_SERVICE) {
			__buffer.ConvertFromASCII_Latin1(_serviceName,_serviceName+strlen(_serviceName));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_IS_SHOW_LAST_DOCSTORE) {
			if (strcmp(_docstore,"")==0)
				__buffer.ConvertFromASCII_Latin1(_serviceDesc.DocStoreID,_serviceDesc.DocStoreID+strlen(_serviceDesc.DocStoreID));
			else
				__buffer.ConvertFromASCII_Latin1(_docstore,_docstore+strlen(_docstore));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_IS_SHOW_LAST_IDXSTORE) {
			if (strcmp(_idxstore,"")==0)
				__buffer.ConvertFromASCII_Latin1(_serviceDesc.IdxStoreID,_serviceDesc.IdxStoreID+strlen(_serviceDesc.IdxStoreID));	
			else
				__buffer.ConvertFromASCII_Latin1(_idxstore,_idxstore+strlen(_idxstore));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_IS_SHOW_NEW_DOCSTORE) {
			if (strcmp(_dsspnew,"")!=0) {
				__buffer.ConvertFromASCII_Latin1(_dsspnew,_dsspnew+strlen(_dsspnew));
			} else {
				sp77sprintf(__string,1023,"0");
				__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			}
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_IS_SHOW_NEW_IDXSTORE) {
			if (strcmp(_isspnew,"")!=0) {
				__buffer.ConvertFromASCII_Latin1(_isspnew,_isspnew+strlen(_isspnew));
			} else {
				sp77sprintf(__string,1023,"0");
				__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			}
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_IS_SHOW_DOCCLASS_ID) {
			if (_firstDocClass) {
				XMLIMAPI_DocClassListGetFirst(_docClassList,&_currDocClass);
				_firstDocClass = XMLIMAPI_False;
			}
			else
				XMLIMAPI_DocClassListGetNext(_docClassList,&_currDocClass);
			__buffer.ConvertFromASCII_Latin1(_currDocClass->docClassID,_currDocClass->docClassID+strlen(_currDocClass->docClassID));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_IS_SHOW_DOCCLASS_SEL) {
			if (strcmp(_currDocClass->docClassID,_docclassid)==0)
				strcpy(__string,"selected");
			else
				strcpy(__string,"");
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_IS_SHOW_DOCCLASS_NAME) {
			XMLIDMLIB_EscapeQuotes(_currDocClass->docClassName,strlen(_currDocClass->docClassName),__string1);
			XMLIDMLIB_EscapeQuotes(_currDocClass->description,strlen(_currDocClass->description),__string2);
			sp77sprintf(__string,9000,"%s &nbsp;&nbsp;&nbsp;&nbsp;(%s)",__string1,__string2);
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			return __buffer;
		}

		return "";
	}

	void XMLIDMLib_HtmlTemplate_IndexingService_Show::getSpList()
	{
		XMLIMAPI_SessionPool		__sp;
		XMLIMAPI_ErrorItem			__errorItem;
		XMLIMAPI_ErrorType			__errorType;
		XMLIMAPI_ErrorText			__errorMsg;
		XMLIDMLIB_Char				__msg[1024];
		
		if (XMLIMAPI_SessionPoolGetFirst(_xml,&__sp)) {

			do {
				XMLIMAPI_SessionPoolListAddItem(_spList,&__sp);
				_spCount++;						
			} while (XMLIMAPI_SessionPoolGetNext(_xml,&__sp));

			XMLIMAPI_SessionPoolListGetFirst(_spList,&_currSP);

		} else {
			XMLIMAPI_GetLastError(_xml,&__errorItem);
			XMLIMAPI_GetErrorType(__errorItem,&__errorType);
			XMLIMAPI_GetErrorText(__errorItem,&__errorMsg);
			sp77sprintf(__msg,1023,"[XMLIDMLIB::XMLIDMLib_HtmlTemplate_IndexingService_Show::getSpList] %s\n",__errorMsg);
			_wa.WriteLogMsg(__msg);
		}

	}

	void XMLIDMLib_HtmlTemplate_IndexingService_Show::getService()
	{
		XMLIDMLIB_Char			__value[513];
		XMLIMAPI_Id				__serviceId;
		XMLIMAPI_ErrorItem		__errorItem;
		XMLIMAPI_ErrorType		__errorType;
		XMLIMAPI_ErrorText		__errorMsg;
		XMLIDMLIB_Char			__msg[1024];
		
		// get values from querystring
		XMLIDMLIB_GetParameterValue(TEXT_CONSTANT_PARAM_IDXSERVICE_ID,_req,__value);
		strcpy(_serviceDesc.ServiceID,__value);

		XMLIMAPI_IdStringAsId(_serviceDesc.ServiceID,__serviceId);
		
		if (XMLIMAPI_ServiceGet(_xml,__serviceId,&_serviceDesc)) {
		} else {
			XMLIMAPI_GetLastError(_xml,&__errorItem);
			XMLIMAPI_GetErrorType(__errorItem,&__errorType);
			XMLIMAPI_GetErrorText(__errorItem,&__errorMsg);
			sp77sprintf(__msg,1023,"[XMLIDMLIB::XMLIDMLib_HtmlTemplate_IndexingService_Show::getService] %s\n",__errorMsg);
			_wa.WriteLogMsg(__msg);
		}

	}

	void XMLIDMLib_HtmlTemplate_IndexingService_Show::getDocClasses()
	{
		XMLIMAPI_DocClass			__docClass;
		XMLIMAPI_ErrorItem			__errorItem;
		XMLIMAPI_ErrorType			__errorType;
		XMLIMAPI_ErrorText			__errorMsg;
		XMLIDMLIB_Char				__msg[1024];
		
		if (XMLIMAPI_DocClassGetFirst(_xml,&__docClass)) {
			do {
				XMLIMAPI_DocClassListAddItem(_docClassList,&__docClass);
				_docClassCount++;						
			} while (XMLIMAPI_DocClassGetNext(_xml,&__docClass));


		} else {
			XMLIMAPI_GetLastError(_xml,&__errorItem);
			XMLIMAPI_GetErrorType(__errorItem,&__errorType);
			XMLIMAPI_GetErrorText(__errorItem,&__errorMsg);
			sp77sprintf(__msg,1023,"[XMLIDMLIB::XMLIDMLib_HtmlTemplate_DocNavigation::getDocClasses] %s\n",__errorMsg);
			_wa.WriteLogMsg(__msg);
		}

	}

