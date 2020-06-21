#ifndef XMLIDMLIB_HTMLTEMPLATE_XMLINDEX_SHOW_HPP
#define XMLIDMLIB_HTMLTEMPLATE_XMLINDEX_SHOW_HPP
/****************************************************************************

  module      : XMLIDMLib_HtmlTemplate_XmlIndex_Show.hpp

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
	#define TEMPLATE_SHOWXMLINDEX							"XMLIDMLib_Html_XmlIndex_Show.htm"

	/*===================================================================
	// Template Keys
	//===================================================================*/
	#define TEMPLATE_KEY_XML_SHOW_SERVICE					"Service*"
	#define TEMPLATE_KEY_XML_SHOW_INDEXID					"xmlIndexId"
	#define TEMPLATE_KEY_XML_SHOW_INDEXPROPID				"xmlIndexPropId"
	#define TEMPLATE_KEY_XML_SHOW_INDEXNAME					"IndexName"
	#define TEMPLATE_KEY_XML_SHOW_BASEPATH 					"BasePath*"
	#define TEMPLATE_KEY_XML_SHOW_VALUEPATH					"ValuePath*"
	#define TEMPLATE_KEY_XML_SHOW_DESCRIPTION				"Description*"
	#define TEMPLATE_KEY_XML_SHOW_DOCCLASSID				"docClassId"
	#define TEMPLATE_KEY_XML_SHOW_DOCCLASSNAME				"docClassName"
	#define TEMPLATE_KEY_XML_SHOW_DOCDESCRIPTION			"DocDescription"
	#define TEMPLATE_KEY_XML_SHOW_ASSIGNED_XMLINDEXES		"AssignedXmlIndexesString"
	#define TEMPLATE_KEY_XML_SHOW_ASSIGNED_XMLINDEXES_CNT	"AssignedXmlIndexesCount"
	#define TEMPLATE_KEY_XML_SHOW_NEWLY_ADDED				"NewlyAdded"
	#define TEMPLATE_KEY_XML_SHOW_IDXSERVICE_ID				"ServiceID"
	#define TEMPLATE_KEY_XML_SHOW_IDXSERVICE_NAME			"ServiceName"
	#define TEMPLATE_KEY_XML_SHOW_IDXSERVICE_DESC			"ServiceDesc"
	#define TEMPLATE_KEY_XML_SHOW_IDXSERVICE_STORE			"DocIdxStore"
	#define TEMPLATE_KEY_XML_SHOW_IDXSERVICE_DSSP			"DocStore"
	#define TEMPLATE_KEY_XML_SHOW_IDXSERVICE_ISSP			"IdxStore"
	#define TEMPLATE_KEY_XML_SHOW_IDXSERVICE_DSSPNEW		"DSSPNew"
	#define TEMPLATE_KEY_XML_SHOW_IDXSERVICE_ISSPNEW		"ISSPNew"
	#define TEMPLATE_KEY_XML_SHOW_HEADING					"Heading*"

	/*===================================================================
	// Template Values
	//===================================================================*/
	#define TEMPLATE_VALUE_XML_SHOW_SERVICE					"Service"
	#define TEMPLATE_VALUE_XML_SHOW_INDEXID					"xmlIndexId"
	#define TEMPLATE_VALUE_XML_SHOW_INDEXPROPID				"xmlIndexPropId"
	#define TEMPLATE_VALUE_XML_SHOW_INDEXNAME				"IndexName"
	#define TEMPLATE_VALUE_XML_SHOW_BASEPATH 				"BasePath"
	#define TEMPLATE_VALUE_XML_SHOW_VALUEPATH				"ValuePath"
	#define TEMPLATE_VALUE_XML_SHOW_DESCRIPTION				"Description"
	#define TEMPLATE_VALUE_XML_SHOW_DOCCLASSID				"docClassId"
	#define TEMPLATE_VALUE_XML_SHOW_DOCCLASSNAME			"docClassName"
	#define TEMPLATE_VALUE_XML_SHOW_DOCDESCRIPTION			"DocDescription"
	#define TEMPLATE_VALUE_XML_SHOW_ASSIGNED_XMLINDEXES		"AssignedXmlIndexesString"
	#define TEMPLATE_VALUE_XML_SHOW_ASSIGNED_XMLINDEXES_CNT	"AssignedXmlIndexesCount"
	#define TEMPLATE_VALUE_XML_SHOW_NEWLY_ADDED				"NewlyAdded"
	#define TEMPLATE_VALUE_XML_SHOW_IDXSERVICE_ID			"ServiceID"
	#define TEMPLATE_VALUE_XML_SHOW_IDXSERVICE_NAME			"ServiceName"
	#define TEMPLATE_VALUE_XML_SHOW_IDXSERVICE_DESC			"ServiceDesc"
	#define TEMPLATE_VALUE_XML_SHOW_IDXSERVICE_STORE		"DocIdxStore"
	#define TEMPLATE_VALUE_XML_SHOW_IDXSERVICE_DSSP			"DocStore"
	#define TEMPLATE_VALUE_XML_SHOW_IDXSERVICE_ISSP			"IdxStore"
	#define TEMPLATE_VALUE_XML_SHOW_IDXSERVICE_DSSPNEW		"DSSPNew"
	#define TEMPLATE_VALUE_XML_SHOW_IDXSERVICE_ISSPNEW		"ISSPNew"

	/*===================================================================
	// Includes
	//===================================================================*/
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Include.hpp"
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Utils.hpp"


	/*===================================================================
	// Class XMLIDMLib_HtmlTemplate_XmlIndex_Show
	//===================================================================*/
	class XMLIDMLib_HtmlTemplate_XmlIndex_Show : public Tools_Template 
	{
		public:

			XMLIDMLib_HtmlTemplate_XmlIndex_Show(sapdbwa_WebAgent & wa,
										   sapdbwa_HttpRequest &req,
										   XMLIMAPI_Handle	&xml);

			~XMLIDMLib_HtmlTemplate_XmlIndex_Show();

		private:

			XMLIMAPI_Handle			&_xml;
			XMLIMAPI_XmlIndex		_xmlIndex;
			XMLIMAPI_Char			_docClass[55];
			sapdbwa_HttpRequest		&_req;
			sapdbwa_WebAgent		&_wa;
			XMLIDMLIB_Char			_serviceName[512];

			XMLIDMLIB_Char				_idxServiceID[50];
			XMLIDMLIB_Char				_idxServiceName[130];
			XMLIDMLIB_Char				_idxServiceDesc[520];
			XMLIDMLIB_Char				_store[10];
			XMLIDMLIB_Char				_docstore[130];
			XMLIDMLIB_Char				_idxstore[130];
			XMLIDMLIB_Char				_dsspnew[3];
			XMLIDMLIB_Char				_isspnew[3];

			XMLIDMLIB_Int2 askForWriteCount(const Tools_DynamicUTF8String & szName);
			Tools_DynamicUTF8String askForValue (const Tools_DynamicUTF8String & szName);
			void getXmlIndex(sapdbwa_HttpRequest &req);
	}; 




#endif