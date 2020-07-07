#ifndef XMLIDMLIB_HTMLTEMPLATE_INDEXINGSERVICE_SHOW_HPP
#define XMLIDMLIB_HTMLTEMPLATE_INDEXINGSERVICE_SHOW_HPP
/****************************************************************************

  module      : XMLIDMLib_HtmlTemplate_IndexingService_Show.hpp

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-07-12  13:45
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
	#define TEMPLATE_IDXSERVICE_SHOW						"XMLIDMLib_Html_IndexingService_Show.htm"

	/*===================================================================
	// Template Keys
	//===================================================================*/
	#define TEMPLATE_KEY_IS_SHOW_SERVICE					"Service*"
	#define TEMPLATE_KEY_IS_SHOW_SERVICE_ID					"ServiceID"
	#define TEMPLATE_KEY_IS_SHOW_SERVICE_NAME				"ServiceName"
	#define TEMPLATE_KEY_IS_SHOW_SERVICE_DESC				"ServiceDesc"
	#define TEMPLATE_KEY_IS_SHOW_SERVICE_SYNC				"ServiceSync"
	#define TEMPLATE_KEY_IS_SHOW_DOCSTORE					"DocStoreSessionPool*"
	#define TEMPLATE_KEY_IS_SHOW_IDXSTORE					"IdxStoreSessionPool*"
	#define TEMPLATE_KEY_IS_SHOW_SPARRAY					"SessionPoolArray*"
	#define TEMPLATE_KEY_IS_SHOW_LAST_DOCSTORE				"LastDocStore"
	#define TEMPLATE_KEY_IS_SHOW_LAST_IDXSTORE				"LastIdxStore"
	#define TEMPLATE_KEY_IS_SHOW_NEW_DOCSTORE				"DSSPNew"
	#define TEMPLATE_KEY_IS_SHOW_NEW_IDXSTORE				"ISSPNew"
	#define TEMPLATE_KEY_IS_SHOW_ASYNC						"Async"
	#define TEMPLATE_KEY_IS_SHOW_DSASYNC					"DSAsync*"
	#define TEMPLATE_KEY_IS_SHOW_DSSYNC						"DSSync*"
	#define TEMPLATE_KEY_IS_SHOW_ASSIGNED_DOCCLASSES		"AssignedDocClasses*"

	/*===================================================================
	// Template Values
	//===================================================================*/
	#define TEMPLATE_VALUE_IS_SHOW_SERVICE					"Service"
	#define TEMPLATE_VALUE_IS_SHOW_SERVICE_ID				"ServiceID"
	#define TEMPLATE_VALUE_IS_SHOW_SERVICE_NAME				"ServiceName"
	#define TEMPLATE_VALUE_IS_SHOW_SERVICE_DESC				"ServiceDesc"
	#define TEMPLATE_VALUE_IS_SHOW_SERVICE_SYNC				"ServiceSync"
	#define TEMPLATE_VALUE_IS_SHOW_DOCSTORE_ID				"DSSPID"
	#define TEMPLATE_VALUE_IS_SHOW_DOCSTORE_SEL				"DSSPSEL"
	#define TEMPLATE_VALUE_IS_SHOW_DOCSTORE_NAME			"DSSPName"
	#define TEMPLATE_VALUE_IS_SHOW_IDXSTORE_ID				"ISSPID"
	#define TEMPLATE_VALUE_IS_SHOW_IDXSTORE_SEL				"ISSPSEL"
	#define TEMPLATE_VALUE_IS_SHOW_IDXSTORE_NAME			"ISSPName"
	#define TEMPLATE_VALUE_IS_SHOW_SP_ARRAY_ID				"SPArrayId"
	#define TEMPLATE_VALUE_IS_SHOW_SP_ARRAY_COUNT			"SPArrayCount"
	#define TEMPLATE_VALUE_IS_SHOW_SP_ARRAY_POS				"SPArrayPos"
	#define TEMPLATE_VALUE_IS_SHOW_LAST_DOCSTORE			"LastDocStore"
	#define TEMPLATE_VALUE_IS_SHOW_LAST_IDXSTORE			"LastIdxStore"
	#define TEMPLATE_VALUE_IS_SHOW_NEW_DOCSTORE				"DSSPNew"
	#define TEMPLATE_VALUE_IS_SHOW_NEW_IDXSTORE				"ISSPNew"
	#define TEMPLATE_VALUE_IS_SHOW_DOCCLASS_ID				"DCID"
	#define TEMPLATE_VALUE_IS_SHOW_DOCCLASS_NAME			"DCName"
	#define TEMPLATE_VALUE_IS_SHOW_DOCCLASS_SEL				"DCSEL"

	/*===================================================================
	// Includes
	//===================================================================*/
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Include.hpp"
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Utils.hpp"


	/*===================================================================
	// Class XMLIDMLib_HtmlTemplate_SessionPool_Show
	//===================================================================*/
	class XMLIDMLib_HtmlTemplate_IndexingService_Show : public Tools_Template 
	{
		public:

			XMLIDMLib_HtmlTemplate_IndexingService_Show(sapdbwa_WebAgent & wa, sapdbwa_HttpRequest &req, XMLIMAPI_Handle &xml);

			~XMLIDMLib_HtmlTemplate_IndexingService_Show();

		private:

			XMLIMAPI_Handle				&_xml;
			sapdbwa_HttpRequest			&_req;
			sapdbwa_WebAgent			&_wa;

			XMLIMAPI_ServiceDesc		_serviceDesc;
			XMLIDMLIB_Char				_serviceName[512];
			XMLIDMLIB_Char				_spName[130];
			XMLIDMLIB_Char				_store[130];
			XMLIDMLIB_Char				_docstore[130];
			XMLIDMLIB_Char				_idxstore[130];
			XMLIDMLIB_Char				_dsspnew[3];
			XMLIDMLIB_Char				_isspnew[3];
			XMLIDMLIB_Char				_adc[130];
			XMLIDMLIB_Char				_docclassid[50];

			XMLIMAPI_SessionPoolList	_spList;
			XMLIDMLIB_UInt4				_spCount;
			XMLIMAPI_SessionPool		*_currSP;
			XMLIDMLIB_UInt4				_count;
			XMLIDMLIB_Bool				_cancel;
			XMLIDMLIB_Bool				_init;

			XMLIMAPI_DocClassList	_docClassList;
			XMLIDMLIB_UInt4			_docClassCount;
			XMLIMAPI_Bool			_firstDocClass;
			XMLIMAPI_DocClass		*_currDocClass;

			XMLIDMLIB_Int2 askForWriteCount(const Tools_DynamicUTF8String & szName);

			Tools_DynamicUTF8String askForValue (const Tools_DynamicUTF8String & szName);

			void getSpList();
			void getService();
			void getDocClasses();

	}; 




#endif