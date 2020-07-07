#ifndef XMLIDMLIB_HTMLTEMPLATE_DOCCLASS_SHOW_HPP
#define XMLIDMLIB_HTMLTEMPLATE_DOCCLASS_SHOW_HPP
/****************************************************************************

  module      : XMLIDMLib_HtmlTemplate_DocClass_Show.hpp

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
	#define TEMPLATE_DOCCLASS_SHOW							"XMLIDMLib_Html_DocClass_Show.htm"

	/*===================================================================
	// Template Keys
	//===================================================================*/
	#define TEMPLATE_KEY_DOC_SHOW_SERVICE					"Service*"
	#define TEMPLATE_KEY_DOC_SHOW_DOCCLASSID				"docClassId"
	#define TEMPLATE_KEY_DOC_SHOW_DOCCLASSNAME				"docClassName"
	#define TEMPLATE_KEY_DOC_SHOW_DOCDESCRIPTION			"DocDescription"
	#define TEMPLATE_KEY_DOC_SHOW_ASSIGNED_XML_INDEXES_STR	"AssignedXmlIndexesString"
	#define TEMPLATE_KEY_DOC_SHOW_ASSIGNED_XML_INDEXES_CNT	"AssignedXmlIndexesCount"
	#define TEMPLATE_KEY_DOC_SHOW_ASSIGNED_XML_INDEXES		"AssignedXmlIndexes*"
	#define TEMPLATE_KEY_DOC_SHOW_ASSIGNABLE_XML_INDEXES	"AssignableXmlIndexes*"
	#define TEMPLATE_KEY_DOC_SHOW_ASSIGNABLE_LISTENTRY		"AssignableListEntry*"
	#define TEMPLATE_KEY_DOC_SHOW_NEWLY_ADDED				"NewlyAdded"
	#define TEMPLATE_KEY_DOC_SHOW_IDXSERVICE_ID				"ServiceID"
	#define TEMPLATE_KEY_DOC_SHOW_IDXSERVICE_NAME			"ServiceName"
	#define TEMPLATE_KEY_DOC_SHOW_IDXSERVICE_DESC			"ServiceDesc"
	#define TEMPLATE_KEY_DOC_SHOW_IDXSERVICE_STORE			"DocIdxStore"
	#define TEMPLATE_KEY_DOC_SHOW_IDXSERVICE_DSSP			"DocStore"
	#define TEMPLATE_KEY_DOC_SHOW_IDXSERVICE_ISSP			"IdxStore"
	#define TEMPLATE_KEY_DOC_SHOW_IDXSERVICE_DSSPNEW		"DSSPNew"
	#define TEMPLATE_KEY_DOC_SHOW_IDXSERVICE_ISSPNEW		"ISSPNew"
	#define TEMPLATE_KEY_DOC_SHOW_HEADING					"Heading*"


	/*===================================================================
	// Template Values
	//===================================================================*/
	#define TEMPLATE_VALUE_DOC_SHOW_SERVICE					"Service"
	#define TEMPLATE_VALUE_DOC_SHOW_DOCCLASSID				"docClassId"
	#define TEMPLATE_VALUE_DOC_SHOW_DOCCLASSNAME			"docClassName"
	#define TEMPLATE_VALUE_DOC_SHOW_DOCDESCRIPTION			"DocDescription"
	#define TEMPLATE_VALUE_DOC_SHOW_ASSIGNED_XML_INDEXES	"AssignedXmlIndexesString"
	#define TEMPLATE_VALUE_DOC_SHOW_ASSIGNED_XML_INDEXES_CNT	"AssignedXmlIndexesCount"
	#define TEMPLATE_VALUE_DOC_SHOW_NEWLY_ADDED				"NewlyAdded"
	#define TEMPLATE_VALUE_DOC_SHOW_XML_INDEX_GENID			"XmlIndexGenId"
	#define TEMPLATE_VALUE_DOC_SHOW_XML_INDEX_ID			"XmlIndexId"
	#define TEMPLATE_VALUE_DOC_SHOW_XML_INDEX_NAME			"XmlIndexName"
	#define TEMPLATE_VALUE_DOC_SHOW_XML_INDEX_DESC			"XmlIndexDesc"
	#define TEMPLATE_VALUE_DOC_SHOW_LISTINDEXNAME			"ListIndexName"
	#define TEMPLATE_VALUE_DOC_SHOW_LISTINDEXID				"ListIndexID"
	#define TEMPLATE_VALUE_DOC_SHOW_LISTINDEXNAME			"ListIndexName"
	#define TEMPLATE_VALUE_DOC_SHOW_LISTINDEXDESC			"ListIndexDesc"
	#define TEMPLATE_VALUE_DOC_SHOW_IDXSERVICE_ID			"ServiceID"
	#define TEMPLATE_VALUE_DOC_SHOW_IDXSERVICE_NAME			"ServiceName"
	#define TEMPLATE_VALUE_DOC_SHOW_IDXSERVICE_DESC			"ServiceDesc"
	#define TEMPLATE_VALUE_DOC_SHOW_IDXSERVICE_STORE		"DocIdxStore"
	#define TEMPLATE_VALUE_DOC_SHOW_IDXSERVICE_DSSP			"DocStore"
	#define TEMPLATE_VALUE_DOC_SHOW_IDXSERVICE_ISSP			"IdxStore"
	#define TEMPLATE_VALUE_DOC_SHOW_IDXSERVICE_DSSPNEW		"DSSPNew"
	#define TEMPLATE_VALUE_DOC_SHOW_IDXSERVICE_ISSPNEW		"ISSPNew"

	/*===================================================================
	// Includes
	//===================================================================*/
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Include.hpp"
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Utils.hpp"


	/*===================================================================
	// Class XMLIDMLib_HtmlTemplate_DocClass_Show
	//===================================================================*/
	class XMLIDMLib_HtmlTemplate_DocClass_Show : public Tools_Template 
	{
		public:

			XMLIDMLib_HtmlTemplate_DocClass_Show(sapdbwa_WebAgent & wa,
										   sapdbwa_HttpRequest &req,
										   XMLIMAPI_Handle	&xml);

			~XMLIDMLib_HtmlTemplate_DocClass_Show();

		private:

			sapdbwa_HttpRequest		&_req;
			sapdbwa_WebAgent		&_wa;
			XMLIMAPI_Handle			&_xml;
			XMLIMAPI_DocClass		_docClass;

			XMLIMAPI_XmlIndexList	_assignedXmlIndices;
			XMLIDMLIB_UInt4			_assignedXmlIndicesCount;
			XMLIMAPI_XmlIndex		*_currAssignedXmlIndex;
			XMLIDMLIB_Char			_assignedXmlIndexesString[5001];
			XMLIDMLIB_UInt4			_assignedCount;
			XMLIDMLIB_Char			_unAssignedXmlIndexesString[5001];
			XMLIDMLIB_Char			_newlyAdded[5001];

			XMLIMAPI_XmlIndexList	_xmlIndexList;
			XMLIDMLIB_UInt4			_xmlIndexCount;
			XMLIMAPI_XmlIndex		*_currXmlIndex;
			XMLIDMLIB_Bool			_firstXmlIndex;
			
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

			void getDocClass();
			void getAssignedXmlIndices();
			void getXmlIndices();
			void addAssignedXmlIndex();
			void removeAssignedXmlIndex();
			void removeAllAssignedXmlIndices();
			void getAssignedXmlIndexes();

			XMLIDMLIB_Char			_serviceName[512];

	}; 




#endif