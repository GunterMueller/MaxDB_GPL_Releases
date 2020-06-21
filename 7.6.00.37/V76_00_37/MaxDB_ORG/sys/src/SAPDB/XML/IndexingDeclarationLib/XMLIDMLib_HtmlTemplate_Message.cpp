/****************************************************************************

  module      : XMLIDMLib_HtmlTemplate_Message.cpp

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-06-26  16:19
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
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_HtmlTemplate_Message.hpp"



	XMLIDMLib_HtmlTemplate_Message::XMLIDMLib_HtmlTemplate_Message (sapdbwa_WebAgent &wa,
																    sapdbwa_HttpRequest &req) 
		                  : _req(req), Tools_Template ( wa, (const unsigned char*)TEMPLATE_MESSAGE )
	{ 
		_keepdata = XMLIDMLIB_True;
		_refreshXmlIndex = XMLIDMLIB_False;
		_refreshDocClass = XMLIDMLIB_False;
		_refreshService = XMLIDMLIB_False;
		strcpy(_serviceName,wa.GetServiceName());
		strcpy(_idxServiceID,"");
		strcpy(_spName,"");
		strcpy(_store,"");
		strcpy(_docstore,"");
		strcpy(_idxstore,"");
		strcpy(_docstorenew,"");
		strcpy(_idxstorenew,"");
		strcpy(_adc,"");
		strcpy(_docclassid,"");
		strcpy(_docclassname,"");
		strcpy(_docclassdesc,"");
		strcpy(_assignedXmlIndexes,"");
		strcpy(_assignedXmlIndexesCount,"");
		strcpy(_newlyAdded,"");
	} 


	XMLIDMLib_HtmlTemplate_Message::~XMLIDMLib_HtmlTemplate_Message() 
	{
	}

	XMLIDMLIB_Int2 XMLIDMLib_HtmlTemplate_Message :: askForWriteCount(const Tools_DynamicUTF8String & szName)
	{
		XMLIDMLIB_UInt4 __nReturn = 0;

		if (szName == TEMPLATE_KEY_MSG_MESSAGE) {
			__nReturn = 1;
		} else if (szName == TEMPLATE_KEY_MSG_MESSAGETYPE) {
			__nReturn = 1;
		} else if (szName == TEMPLATE_KEY_MSG_KEEPDATA) {
			__nReturn = 1;
		} else if (szName == TEMPLATE_KEY_MSG_REFRESH_XMLINDEX) {
			__nReturn = 1;
		} else if (szName == TEMPLATE_KEY_MSG_REFRESH_DOCCLASS) {
			__nReturn = 1;
		} else if (szName == TEMPLATE_KEY_MSG_REFRESH_SERVICE) {
			__nReturn = 1;
		} else if (szName == TEMPLATE_KEY_MSG_SERVICE) {
			__nReturn = 1;
		} else if (szName == TEMPLATE_KEY_MSG_IDXSERVICE_ID) {
			__nReturn = 1;
		} else if (szName == TEMPLATE_KEY_MSG_IDXSERVICE_NAME) {
			__nReturn = 1;
		} else if (szName == TEMPLATE_KEY_MSG_IDXSERVICE_DESC) {
			__nReturn = 1;
		} else if (szName == TEMPLATE_KEY_MSG_SESSIONPOOL_NAME) {
			__nReturn = 1;
		} else if (szName == TEMPLATE_KEY_MSG_IDXSERVICE_STORE) {
			__nReturn = 1;
		} else if (szName == TEMPLATE_KEY_MSG_IDXSERVICE_DOCSTORE) {
			__nReturn = 1;
		} else if (szName == TEMPLATE_KEY_MSG_IDXSERVICE_IDXSTORE) {
			__nReturn = 1;
		} else if (szName == TEMPLATE_KEY_MSG_IDXSERVICE_DOCSTORENEW) {
			__nReturn = 1;
		} else if (szName == TEMPLATE_KEY_MSG_IDXSERVICE_IDXSTORENEW) {
			__nReturn = 1;
		} else if (szName == TEMPLATE_KEY_MSG_IDXSERVICE_DOCCLASS) {
			__nReturn = 1;
		} else if (szName == TEMPLATE_KEY_MSG_DOCCLASS_ID) {
			__nReturn = 1;
		} else if (szName == TEMPLATE_KEY_MSG_DOCCLASS_NAME) {
			__nReturn = 1;
		} else if (szName == TEMPLATE_KEY_MSG_DOCCLASS_DESCRIPTION) {
			__nReturn = 1;
		} else if (szName == TEMPLATE_KEY_MSG_DOCCLASS_ASSIGNED_XMLINDEXES) {
			__nReturn = 1;
		} else if (szName == TEMPLATE_KEY_MSG_DOCCLASS_ASSIGNED_XMLINDEXES_COUNT) {
			__nReturn = 1;
		} else if (szName == TEMPLATE_KEY_MSG_DOCCLASS_NEWLY_ADDED) {
			__nReturn = 1;
		} else if (szName == TEMPLATE_KEY_MSG_LEVEL) {
			__nReturn = 1;
		} else if (szName == TEMPLATE_KEY_MSG_CANCEL) {
			if (_cancel)
				__nReturn = 1;
			else
				__nReturn = 0;
		}
		return __nReturn;
	} 


	Tools_DynamicUTF8String XMLIDMLib_HtmlTemplate_Message :: askForValue (const Tools_DynamicUTF8String & szName)
	{
		Tools_DynamicUTF8String __buffer;
		XMLIDMLIB_Char			__string[4096];

		if (szName == TEMPLATE_VALUE_MSG_MESSAGE) {
			return _message;
		} else if (szName == TEMPLATE_VALUE_MSG_MESSAGETYPE) {
			switch(_messagetype) {
				case XMLIDMLIB_MSGTY_ERROR:
					return (Tools_DynamicUTF8String::ConstPointer)TEXT_CONSTANT_HTML_MSGTY_ERROR_GIF;
				case XMLIDMLIB_MSGTY_WARNING:
					return (Tools_DynamicUTF8String::ConstPointer)TEXT_CONSTANT_HTML_MSGTY_WARNING_GIF;
				case XMLIDMLIB_MSGTY_INFO:
					return (Tools_DynamicUTF8String::ConstPointer)TEXT_CONSTANT_HTML_MSGTY_INFO_GIF;
				case XMLIDMLIB_MSGTY_REQUEST:
					return (Tools_DynamicUTF8String::ConstPointer)TEXT_CONSTANT_HTML_MSGTY_REQUEST_GIF;
				case XMLIDMLIB_MSGTY_OK:
					return (Tools_DynamicUTF8String::ConstPointer)TEXT_CONSTANT_HTML_MSGTY_OK_GIF;
				case XMLIDMLIB_MSGTY_CLEAR:
					return (Tools_DynamicUTF8String::ConstPointer)TEXT_CONSTANT_HTML_MSGTY_CLEAR_GIF;
				default:
					return (Tools_DynamicUTF8String::ConstPointer)TEXT_CONSTANT_HTML_MSGTY_CLEAR_GIF;
			}
			return _message;
		} else if (szName == TEMPLATE_VALUE_MSG_KEEPDATA) {
			if (_keepdata)
				return (Tools_DynamicUTF8String::ConstPointer)"true";
			else
				return (Tools_DynamicUTF8String::ConstPointer)"false";
		} else if (szName == TEMPLATE_VALUE_MSG_REFRESH_XMLINDEX) {
			if (_refreshXmlIndex)
				return (Tools_DynamicUTF8String::ConstPointer)"true";
			else
				return (Tools_DynamicUTF8String::ConstPointer)"false";
		} else if (szName == TEMPLATE_VALUE_MSG_REFRESH_DOCCLASS) {
			if (_refreshDocClass)
				return (Tools_DynamicUTF8String::ConstPointer)"true";
			else
				return (Tools_DynamicUTF8String::ConstPointer)"false";
		} else if (szName == TEMPLATE_VALUE_MSG_REFRESH_SERVICE) {
			if (_refreshService)
				return (Tools_DynamicUTF8String::ConstPointer)"true";
			else
				return (Tools_DynamicUTF8String::ConstPointer)"false";
		} else if (szName == TEMPLATE_VALUE_MSG_SERVICE) {
			return (Tools_DynamicUTF8String::ConstPointer)_serviceName;
		} else if (szName == TEMPLATE_VALUE_MSG_IDXSERVICE_ID) {
			__buffer.ConvertFromASCII_Latin1(_idxServiceID,_idxServiceID+strlen(_idxServiceID));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_MSG_IDXSERVICE_NAME) {
			XMLIDMLIB_EscapeQuotes(_idxServiceName,strlen(_idxServiceName),__string);
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_MSG_IDXSERVICE_DESC) {
			XMLIDMLIB_EscapeQuotes(_idxServiceDesc,strlen(_idxServiceDesc),__string);
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_MSG_SESSIONPOOL_NAME) {
			XMLIDMLIB_EscapeQuotes(_spName,strlen(_spName),__string);
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_MSG_IDXSERVICE_STORE) {
			__buffer.ConvertFromASCII_Latin1(_store,_store+strlen(_store));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_MSG_IDXSERVICE_DOCSTORE) {
			__buffer.ConvertFromASCII_Latin1(_docstore,_docstore+strlen(_docstore));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_MSG_IDXSERVICE_IDXSTORE) {
			__buffer.ConvertFromASCII_Latin1(_idxstore,_idxstore+strlen(_idxstore));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_MSG_IDXSERVICE_DOCSTORENEW) {
			__buffer.ConvertFromASCII_Latin1(_docstorenew,_docstorenew+strlen(_docstorenew));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_MSG_IDXSERVICE_IDXSTORENEW) {
			__buffer.ConvertFromASCII_Latin1(_idxstorenew,_idxstorenew+strlen(_idxstorenew));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_MSG_IDXSERVICE_DOCCLASS) {
			__buffer.ConvertFromASCII_Latin1(_adc,_adc+strlen(_adc));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_MSG_DOCCLASS_ID) {
			__buffer.ConvertFromASCII_Latin1(_docclassid,_docclassid+strlen(_docclassid));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_MSG_DOCCLASS_NAME) {
			XMLIDMLIB_EscapeQuotes(_docclassname,strlen(_docclassname),__string);
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_MSG_DOCCLASS_DESCRIPTION) {
			XMLIDMLIB_EscapeQuotes(_docclassdesc,strlen(_docclassdesc),__string);
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_MSG_DOCCLASS_ASSIGNED_XMLINDEXES) {
			__buffer.ConvertFromASCII_Latin1(_assignedXmlIndexes,_assignedXmlIndexes+strlen(_assignedXmlIndexes));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_MSG_DOCCLASS_ASSIGNED_XMLINDEXES_COUNT) {
			__buffer.ConvertFromASCII_Latin1(_assignedXmlIndexesCount,_assignedXmlIndexesCount+strlen(_assignedXmlIndexesCount));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_MSG_DOCCLASS_NEWLY_ADDED) {
			__buffer.ConvertFromASCII_Latin1(_newlyAdded,_newlyAdded+strlen(_newlyAdded));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_MSG_LEVEL) {
			__buffer.ConvertFromASCII_Latin1(_level,_level+strlen(_level));
			return __buffer;
		}
		return (Tools_DynamicUTF8String::ConstPointer)"";
	}

	void XMLIDMLib_HtmlTemplate_Message :: setMessage(XMLIDMLIB_Int2 msgty, 
												  const Tools_DynamicUTF8String & msg,
												  XMLIDMLIB_Bool keepdata,
												  XMLIDMLIB_Bool refreshXmlIndex,
												  XMLIDMLIB_Bool refreshDocClass,
												  XMLIDMLIB_Bool refreshService,
												  XMLIDMLIB_Char *idxServiceID,
												  XMLIDMLIB_Char *idxServiceName,
												  XMLIDMLIB_Char *idxServiceDesc,
												  XMLIDMLIB_Char *spName,
												  XMLIDMLIB_Char *store,
												  XMLIDMLIB_Char *docstore,
												  XMLIDMLIB_Char *idxstore,
												  XMLIDMLIB_Char *docstorenew,
												  XMLIDMLIB_Char *idxstorenew,
												  XMLIDMLIB_Char *adc,
												  XMLIDMLIB_Char *docclassid,
												  XMLIDMLIB_Char *docclassname,
											 	  XMLIDMLIB_Char *docclassdesc,
												  XMLIDMLIB_Char *assignedXmlIndexes,
												  XMLIDMLIB_Char *assignedXmlIndexesCount,
												  XMLIDMLIB_Char *newlyadded,
												  XMLIDMLIB_Bool cancel,
												  XMLIDMLIB_Char *level)
	{
		_message = msg;
		_messagetype = msgty;
		_keepdata = keepdata;
		_refreshXmlIndex = refreshXmlIndex;
		_refreshDocClass = refreshDocClass;
		_refreshService = refreshService;
		strcpy(_idxServiceID,idxServiceID);
		strcpy(_idxServiceName,idxServiceName);
		strcpy(_idxServiceDesc,idxServiceDesc);
		strcpy(_spName,spName);
		strcpy(_store,store);
		strcpy(_docstore,docstore);
		strcpy(_idxstore,idxstore);
		strcpy(_docstorenew,docstorenew);
		strcpy(_idxstorenew,idxstorenew);
		strcpy(_adc,adc);
		strcpy(_docclassid,docclassid);
		strcpy(_docclassname,docclassname);
		strcpy(_docclassdesc,docclassdesc);
		strcpy(_assignedXmlIndexes,assignedXmlIndexes);
		strcpy(_assignedXmlIndexesCount,assignedXmlIndexesCount);
		strcpy(_newlyAdded,newlyadded);
		_cancel = cancel;
		strcpy(_level,level);
	}

