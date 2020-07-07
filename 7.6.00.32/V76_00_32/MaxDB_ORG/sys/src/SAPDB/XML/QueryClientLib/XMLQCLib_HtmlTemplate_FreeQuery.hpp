#ifndef XMLQCLIB_HTMLTEMPLATE_FREEQUERY_HPP
#define XMLQCLIB_HTMLTEMPLATE_FREEQUERY_HPP
/****************************************************************************

  module      : XMLQCLib_HtmlTemplate_FreeQuery.hpp

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-06-21  17:10
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
	#define TEMPLATE_FREEQUERY								"XMLQCLib_Html_FreeQuery.htm"

	/*===================================================================
	// Template Keys
	//===================================================================*/
	#define TEMPLATE_KEY_QUERY_FREE_SERVICE					"Service*"
	#define TEMPLATE_KEY_QUERY_FREE_DOC_CLASS_LINES			"DcLines*"
	#define TEMPLATE_KEY_QUERY_FREE_DOC_CLASS_IDX			"DocClassIdx*"
	#define TEMPLATE_KEY_QUERY_FREE_DOC_CLASS_NAME			"DocClassName*"
	#define TEMPLATE_KEY_QUERY_FREE_XML_INDEX				"XMLIndex*"
	#define TEMPLATE_KEY_QUERY_FREE_NO_XML_INDEX			"NoXMLIndex*"
	#define TEMPLATE_KEY_QUERY_FREE_INDEX					"Index*"
	#define TEMPLATE_KEY_QUERY_FREE_INDEX_SEL				"IndexSel*"
	#define TEMPLATE_KEY_QUERY_FREE_DC						"Dc*"
	#define TEMPLATE_KEY_QUERY_FREE_DC_SEL					"DcSel*"
	#define TEMPLATE_KEY_QUERY_FREE_DC_SEL_ID				"DcSelId*"
	#define TEMPLATE_KEY_QUERY_FREE_XML_INDEX_VALUE_COUNT	"XMLIndexValueCount*"
	#define TEMPLATE_KEY_QUERY_FREE_XML_INDEX_LINES			"XMLIndexLines*"
	#define TEMPLATE_KEY_QUERY_FREE_XML_INDEX_LOG_OP_1		"CompOp1*"
	#define TEMPLATE_KEY_QUERY_FREE_XML_INDEX_LOG_OP_2		"CompOp2*"
	#define TEMPLATE_KEY_QUERY_FREE_XML_INDEX_LOG_OP_3		"CompOp3*"
	#define TEMPLATE_KEY_QUERY_FREE_XML_INDEX_LOG_OP_4		"CompOp4*"
	#define TEMPLATE_KEY_QUERY_FREE_XML_INDEX_LOG_OP_5		"CompOp5*"
	#define TEMPLATE_KEY_QUERY_FREE_XML_INDEX_INPUT_1		"Input1*"
	#define TEMPLATE_KEY_QUERY_FREE_XML_INDEX_INPUT_2		"Input2*"
	#define TEMPLATE_KEY_QUERY_FREE_XML_INDEX_INPUT_3		"Input3*"
	#define TEMPLATE_KEY_QUERY_FREE_XML_INDEX_INPUT_4		"Input4*"
	#define TEMPLATE_KEY_QUERY_FREE_XML_INDEX_INPUT_5		"Input5*"
	#define TEMPLATE_KEY_QUERY_FREE_XML_INDEX_OR_1			"Or1*"
	#define TEMPLATE_KEY_QUERY_FREE_XML_INDEX_OR_2			"Or2*"
	#define TEMPLATE_KEY_QUERY_FREE_XML_INDEX_OR_3			"Or3*"
	#define TEMPLATE_KEY_QUERY_FREE_XML_INDEX_OR_4			"Or4*"
	#define TEMPLATE_KEY_QUERY_FREE_XML_INDEX_OR_TXT_1		"OrTxt1*"
	#define TEMPLATE_KEY_QUERY_FREE_XML_INDEX_OR_TXT_2		"OrTxt2*"
	#define TEMPLATE_KEY_QUERY_FREE_XML_INDEX_OR_TXT_3		"OrTxt3*"
	#define TEMPLATE_KEY_QUERY_FREE_XML_INDEX_OR_TXT_4		"OrTxt4*"
	#define TEMPLATE_KEY_QUERY_FREE_XML_INDEX_AND			"And*"
	#define TEMPLATE_KEY_QUERY_FREE_XML_INDEX_AND_TXT		"AndTxt*"
	#define TEMPLATE_KEY_QUERY_FREE_XML_INDEX_ORDC			"OrDc*"
	#define TEMPLATE_KEY_QUERY_FREE_XML_INDEX_ORDC_TXT		"OrDcTxt*"
	#define TEMPLATE_KEY_QUERY_FREE_EXPLAIN_DC				"ExplainDc*"
	#define TEMPLATE_KEY_QUERY_FREE_EXPLAIN_NO_DC			"ExplainNoDc*"
	#define TEMPLATE_KEY_QUERY_FREE_NO_CONNECTION			"NoConnection*"
	#define TEMPLATE_KEY_QUERY_FREE_RESULT					"Result*"
	#define TEMPLATE_KEY_QUERY_FREE_RESULT_ROW				"ResultRow*"
	#define TEMPLATE_KEY_QUERY_FREE_NO_RESULT_ROW			"NoResultRow*"
	#define TEMPLATE_KEY_QUERY_FREE_ERROR_ROW				"ErrorRow*"

	/*===================================================================
	// Template Values
	//===================================================================*/
	#define TEMPLATE_VALUE_QUERY_FREE_SERVICE				"Service"
	#define TEMPLATE_VALUE_QUERY_FREE_DOC_CLASS_LINES		"DcLines"
	#define TEMPLATE_VALUE_QUERY_FREE_DOC_CLASS_LINE		"DcLine"
	#define TEMPLATE_VALUE_QUERY_FREE_DOCCLASS_SEL_ID		"DCIDSel"
	#define TEMPLATE_VALUE_QUERY_FREE_DOCCLASS_SEL_NAME		"DCNameSel"
	#define TEMPLATE_VALUE_QUERY_FREE_DOCCLASS_SEL			"DCSel"
	#define TEMPLATE_VALUE_QUERY_FREE_XML_INDEX_GEN_ID		"XMLIndexGenId"
	#define TEMPLATE_VALUE_QUERY_FREE_DOC_CLASS_GEN_ID		"DCGenId"
	#define TEMPLATE_VALUE_QUERY_FREE_XML_INDEX_VALUE_COUNT	"XMLIndexValueCount"
	#define TEMPLATE_VALUE_QUERY_FREE_XML_INDEX_LINES		"XMLIndexLines"
	#define TEMPLATE_VALUE_QUERY_FREE_OR_LINE				"line"
	#define TEMPLATE_VALUE_QUERY_FREE_XML_INDEX_ID			"IDXID"
	#define TEMPLATE_VALUE_QUERY_FREE_DOC_CLASS_ID			"DCID"
	#define TEMPLATE_VALUE_QUERY_FREE_XML_INDEX_SEL			"IDXSel"
	#define TEMPLATE_VALUE_QUERY_FREE_XML_INDEX_NAME		"IDXName"
	#define TEMPLATE_VALUE_QUERY_FREE_DOC_CLASS_NAME		"DCName"
	#define TEMPLATE_VALUE_QUERY_FREE_COMP_OP_SEL_0			"compopsel0"
	#define TEMPLATE_VALUE_QUERY_FREE_COMP_OP_SEL_1			"compopsel1"
	#define TEMPLATE_VALUE_QUERY_FREE_COMP_OP_SEL_2			"compopsel2"
	#define TEMPLATE_VALUE_QUERY_FREE_COMP_OP_SEL_3			"compopsel3"
	#define TEMPLATE_VALUE_QUERY_FREE_COMP_OP_SEL_4			"compopsel4"
	#define TEMPLATE_VALUE_QUERY_FREE_COMP_OP_SEL_5			"compopsel5"
	#define TEMPLATE_VALUE_QUERY_FREE_INPUT_VALUE_1			"valinputvalue1"
	#define TEMPLATE_VALUE_QUERY_FREE_INPUT_VALUE_2			"valinputvalue2"
	#define TEMPLATE_VALUE_QUERY_FREE_INPUT_VALUE_3			"valinputvalue3"
	#define TEMPLATE_VALUE_QUERY_FREE_INPUT_VALUE_4			"valinputvalue4"
	#define TEMPLATE_VALUE_QUERY_FREE_INPUT_VALUE_5			"valinputvalue5"
	#define TEMPLATE_VALUE_QUERY_FREE_RESULT_COUNT			"resultcount"
	#define TEMPLATE_VALUE_QUERY_FREE_RESULT_LINE			"resultline"
	#define TEMPLATE_VALUE_QUERY_FREE_ERROR_LINE			"ErrorRow"

	/*===================================================================
	// Includes
	//===================================================================*/
	#include "SAPDB/XML/QueryClientLib/XMLQCLib_Common_Include.hpp"
	#include "SAPDB/XML/QueryClientLib/XMLQCLib_Common_Utils.hpp"
	#include <stdlib.h>


	/*===================================================================
	// Class XMLQCLib_HtmlTemplate_XmlIndex_New
	//===================================================================*/
	class XMLQCLib_HtmlTemplate_FreeQuery : public Tools_Template 
	{
		public:

			XMLQCLib_HtmlTemplate_FreeQuery(sapdbwa_WebAgent & wa,
										   sapdbwa_HttpRequest &req,
										   XMLQSLib_Handle &qsHandle,
										   XMLIMAPI_Handle &xml);

			~XMLQCLib_HtmlTemplate_FreeQuery();

		private:


			XMLIMAPI_DocClassList	_dcListAll;
			XMLQCLIB_UInt4			_dcCountAll;
			XMLIMAPI_Bool			_firstDcAll;

			XMLQCLIB_Long			_dcLines;
			XMLIMAPI_Bool			_firstDcSel;
			XMLIMAPI_Bool			_firstDcIdx;

			XMLIMAPI_Bool			_firstIdx;

			XMLQCLIB_Char			_string[4096];
			XMLQCLIB_Char			_selDcString[8192];
			XMLQCLIB_Char			_command[4096];
			XMLQCLIB_Char			_serviceName[512];

			XMLQCLIB_UInt4			_compop;

			XMLIMAPI_DocClass		*_docClass;
			XMLIMAPI_XmlIndex		*_xmlIndex;
			sapdbwa_HttpRequest		&_req;
			sapdbwa_WebAgent		&_wa;
			XMLIMAPI_Handle			&_xml;
			XMLQSLib_Handle			&_qsHandle;

			XMLQCLIB_QueryParams		*_queryParams;
			XMLQCLIB_QueryParamsList	_queryParamsList;
			XMLQCLIB_QueryParamsIdx		*_queryParamsIdx;
			XMLQCLIB_QueryParamsIdxList _queryParamsIdxList;
			XMLQCLIB_UInt4				_queryParamCount;
			XMLQCLIB_Bool				_firstQueryParam;
			XMLQCLIB_Bool				_isAddLine;
			XMLQCLIB_Bool				_isSetDc;
			XMLQCLIB_Bool				_isSearch;

			XMLQSLib_WhereClause		_whereClause;
			XMLQCLIB_Bool				_resultError;
			XMLQCLIB_Bool				_firstResultRow;
			XMLQCLIB_UInt4				_resultcount;
			st_xmlerror_error			_error;
			XMLQCLIB_Bool				_noResult;

			XMLQCLIB_Char				_server[1001];
			XMLQCLIB_Char				_docUri[1001];

			void getParameters();
			void getDocClasses();
			void getXmlIndices();

			void getAssignedXmlIndices();
			
			XMLQCLIB_Bool	doSearch();

			XMLQCLIB_UInt4				_outXmlIndexLine;
			XMLQCLIB_UInt4				_outDocClassLine;

/*			XMLQCLIB_Bool	buildWhereClause(XMLIdx_WhereClause	*where);
			XMLQCLIB_Bool	buildWhereRow(XMLIdx_WhereRow	*row,
										  XMLIdx_WhereRow	data);
*/
			SAPDB_Int2 askForWriteCount(const Tools_DynamicUTF8String & szName);

			Tools_DynamicUTF8String askForValue (const Tools_DynamicUTF8String & szName);

			bool askForContinue ( const Tools_DynamicUTF8String & szName );
	}; 


#endif