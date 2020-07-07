#ifndef XMLIDMLIB_TEMPLATEMESSAGE_HPP
#define XMLIDMLIB_TEMPLATEMESSAGE_HPP
/****************************************************************************

  module      : XMLIDMLib_HtmlTemplate_Message.hpp

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
	// Defines
	//===================================================================*/
	#define TEMPLATE_MESSAGE								"XMLIDMLib_Html_Message.htm"

	/*===================================================================
	// Template Keys
	//===================================================================*/
	#define TEMPLATE_KEY_MSG_SERVICE						"Service*"
	#define TEMPLATE_KEY_MSG_MESSAGE						"Message"
	#define TEMPLATE_KEY_MSG_MESSAGETYPE					"MessageType"
	#define TEMPLATE_KEY_MSG_KEEPDATA						"KeepData"
	#define TEMPLATE_KEY_MSG_REFRESH_XMLINDEX				"RefreshXmlIndex"
	#define TEMPLATE_KEY_MSG_REFRESH_DOCCLASS				"RefreshDocClass"
	#define TEMPLATE_KEY_MSG_REFRESH_SERVICE				"RefreshService"
	#define TEMPLATE_KEY_MSG_SESSIONPOOL_NAME				"PoolName"
	#define TEMPLATE_KEY_MSG_IDXSERVICE_STORE				"DocIdxStore"
	#define TEMPLATE_KEY_MSG_IDXSERVICE_DOCSTORE			"DocStore"
	#define TEMPLATE_KEY_MSG_IDXSERVICE_IDXSTORE			"IdxStore"
	#define TEMPLATE_KEY_MSG_IDXSERVICE_DOCSTORENEW			"DocStoreNew"
	#define TEMPLATE_KEY_MSG_IDXSERVICE_IDXSTORENEW			"IdxStoreNew"
	#define TEMPLATE_KEY_MSG_IDXSERVICE_ID					"ServiceID"
	#define TEMPLATE_KEY_MSG_IDXSERVICE_NAME				"ServiceName"
	#define TEMPLATE_KEY_MSG_IDXSERVICE_DESC				"ServiceDesc"
	#define TEMPLATE_KEY_MSG_IDXSERVICE_DOCCLASS			"ADC"
	#define TEMPLATE_KEY_MSG_DOCCLASS_ID					"docClassId"
	#define TEMPLATE_KEY_MSG_DOCCLASS_NAME					"docClassName"
	#define TEMPLATE_KEY_MSG_DOCCLASS_DESCRIPTION			"DocDescription"
	#define TEMPLATE_KEY_MSG_DOCCLASS_ASSIGNED_XMLINDEXES	"AssignedXmlIndexesString"
	#define TEMPLATE_KEY_MSG_DOCCLASS_ASSIGNED_XMLINDEXES_COUNT	"AssignedXmlIndexesCount"
	#define TEMPLATE_KEY_MSG_DOCCLASS_NEWLY_ADDED			"NewlyAdded"
	#define TEMPLATE_KEY_MSG_CANCEL							"Cancel"
	#define TEMPLATE_KEY_MSG_LEVEL							"Level"

	/*===================================================================
	// Template Values
	//===================================================================*/
	#define TEMPLATE_VALUE_MSG_SERVICE						"Service"
	#define TEMPLATE_VALUE_MSG_MESSAGE						"Message"
	#define TEMPLATE_VALUE_MSG_MESSAGETYPE					"MessageType"
	#define TEMPLATE_VALUE_MSG_KEEPDATA						"KeepData"
	#define TEMPLATE_VALUE_MSG_REFRESH_XMLINDEX				"RefreshXmlIndex"
	#define TEMPLATE_VALUE_MSG_REFRESH_DOCCLASS				"RefreshDocClass"
	#define TEMPLATE_VALUE_MSG_REFRESH_SERVICE				"RefreshService"
	#define TEMPLATE_VALUE_MSG_SESSIONPOOL_NAME				"PoolName"
	#define TEMPLATE_VALUE_MSG_IDXSERVICE_STORE				"DocIdxStore"
	#define TEMPLATE_VALUE_MSG_IDXSERVICE_DOCSTORE			"DocStore"
	#define TEMPLATE_VALUE_MSG_IDXSERVICE_IDXSTORE			"IdxStore"
	#define TEMPLATE_VALUE_MSG_IDXSERVICE_DOCSTORENEW		"DocStoreNew"
	#define TEMPLATE_VALUE_MSG_IDXSERVICE_IDXSTORENEW		"IdxStoreNew"
	#define TEMPLATE_VALUE_MSG_IDXSERVICE_ID				"ServiceID"
	#define TEMPLATE_VALUE_MSG_IDXSERVICE_NAME				"ServiceName"
	#define TEMPLATE_VALUE_MSG_IDXSERVICE_DESC				"ServiceDesc"
	#define TEMPLATE_VALUE_MSG_IDXSERVICE_DOCCLASS			"ADC"
	#define TEMPLATE_VALUE_MSG_DOCCLASS_ID					"docClassId"
	#define TEMPLATE_VALUE_MSG_DOCCLASS_NAME				"docClassName"
	#define TEMPLATE_VALUE_MSG_DOCCLASS_DESCRIPTION			"DocDescription"
	#define TEMPLATE_VALUE_MSG_DOCCLASS_ASSIGNED_XMLINDEXES	"AssignedXmlIndexesString"
	#define TEMPLATE_VALUE_MSG_DOCCLASS_ASSIGNED_XMLINDEXES_COUNT	"AssignedXmlIndexesCount"
	#define TEMPLATE_VALUE_MSG_DOCCLASS_NEWLY_ADDED			"NewlyAdded"
	#define TEMPLATE_VALUE_MSG_LEVEL						"Level"

	/*===================================================================
	// Includes
	//===================================================================*/
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Include.hpp"
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Utils.hpp"


	/*===================================================================
	// Class XMLIDMLib_HtmlTemplate_Message
	//===================================================================*/
	class XMLIDMLib_HtmlTemplate_Message : public Tools_Template 
	{
		public:

			XMLIDMLib_HtmlTemplate_Message(sapdbwa_WebAgent & wa,
										   sapdbwa_HttpRequest &req);
			
			~XMLIDMLib_HtmlTemplate_Message();

			void setMessage(XMLIDMLIB_Int2 msgty, 
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
							XMLIDMLIB_Char *level);

		private:

			XMLIDMLIB_Int2 askForWriteCount(const Tools_DynamicUTF8String & szName);

			Tools_DynamicUTF8String askForValue (const Tools_DynamicUTF8String & szName);

			Tools_DynamicUTF8String 	_message;

			XMLIDMLIB_Int2				_messagetype;

			XMLIDMLIB_Bool				_keepdata;
			XMLIDMLIB_Bool				_refreshXmlIndex;
			XMLIDMLIB_Bool				_refreshDocClass;
			XMLIDMLIB_Bool				_refreshService;
			XMLIDMLIB_Char				_idxServiceID[50];
			XMLIDMLIB_Char				_idxServiceName[130];
			XMLIDMLIB_Char				_idxServiceDesc[520];
			XMLIDMLIB_Char				_spName[130];
			XMLIDMLIB_Char				_store[130];
			XMLIDMLIB_Char				_docstore[130];
			XMLIDMLIB_Char				_idxstore[130];
			XMLIDMLIB_Char				_docstorenew[3];
			XMLIDMLIB_Char				_idxstorenew[3];
			XMLIDMLIB_Char				_adc[130];
			XMLIDMLIB_Char				_docclassid[50];
			XMLIDMLIB_Char				_docclassname[129];
			XMLIDMLIB_Char				_docclassdesc[513];
			XMLIDMLIB_Char				_assignedXmlIndexes[5001];
			XMLIDMLIB_Char				_assignedXmlIndexesCount[5];
			XMLIDMLIB_Char				_newlyAdded[5001];
			XMLIDMLIB_Bool				_cancel;
			XMLIDMLIB_Char				_level[3];

			XMLIDMLIB_Char				_string[4096];
			XMLIDMLIB_Char				_serviceName[512];
			sapdbwa_HttpRequest			&_req;

	}; 




#endif