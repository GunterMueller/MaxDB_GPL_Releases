/****************************************************************************

  module      : XMLQCLib_HtmlTemplate_FreeQuery.cpp

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-06-21  17:11
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
	#include "SAPDB/XML/QueryClientLib/XMLQCLib_HtmlTemplate_FreeQuery.hpp"



	XMLQCLib_HtmlTemplate_FreeQuery::XMLQCLib_HtmlTemplate_FreeQuery (sapdbwa_WebAgent &wa,
																      sapdbwa_HttpRequest &req,
																	  XMLQSLib_Handle &qsHandle,
																	  XMLIMAPI_Handle &xml) 
		                  : _wa(wa), _req(req), _xml(xml), _qsHandle(qsHandle), Tools_Template ( wa, (const unsigned char*)TEMPLATE_FREEQUERY )
	{ 
		XMLQCLIB_Char	__msg[4096];

		strcpy(_serviceName,wa.GetServiceName());

		XMLIMAPI_DocClassListCreate(&_dcListAll);
		_dcCountAll = 0;
		_firstDcAll = XMLQCLIB_True;
		_resultError = XMLQCLIB_False;
		_firstResultRow = XMLQCLIB_False;
		_isSearch = XMLQCLIB_False;

		/* get server string for error messages */
		XMLQCLIB_BuildServerStringLong(wa,req,_server,1000);

		// get values from querystring
		if (!XMLQCLIB_GetParameterValue(TEXT_CONSTANT_QCL_PARAM_SUB_COMMAND,_req,_command)) {
			/* no sub command -> show initial page */
			getDocClasses();
			_isAddLine = XMLQCLIB_False;
			// get the parameters for the xml index part from the requets
			getParameters();
		} else {
			if (XMLQCLIB_IsSubCommand(_req,(char *)TEXT_CONSTANT_QCL_CMD_QUERY_SET_DC)) {
				/* subcommand: add selected doc class to selection */
				/* get the added doc class */
				getDocClasses();
				_isAddLine = XMLQCLIB_False;
				_isSetDc = XMLQCLIB_True;
				_isSearch = XMLQCLIB_False;
				// get the parameters for the xml index part from the requets
				getParameters();
			} else if (XMLQCLIB_IsSubCommand(_req,(char *)TEXT_CONSTANT_QCL_CMD_QUERY_ADD_OR)) {
				/* subcommand: remove selected doc class from selection */
				getDocClasses();
				_isAddLine = XMLQCLIB_False;
				_isSetDc = XMLQCLIB_False;
				_isSearch = XMLQCLIB_False;
				// get the parameters for the xml index part from the requets
				getParameters();
			} else if (XMLQCLIB_IsSubCommand(_req,(char *)TEXT_CONSTANT_QCL_CMD_QUERY_MORE)) {
				getDocClasses();
				_isAddLine = XMLQCLIB_False;
				_isSetDc = XMLQCLIB_False;
				_isSearch = XMLQCLIB_False;
				// get the parameters for the xml index part from the requets
				getParameters();
			} else if (XMLQCLIB_IsSubCommand(_req,(char *)TEXT_CONSTANT_QCL_CMD_QUERY_ADD_DC_LINE)) {
				/* subcommand: add a new selection block */
				getDocClasses();
				_isAddLine = XMLQCLIB_True;
				_isSetDc = XMLQCLIB_False;
				_isSearch = XMLQCLIB_False;
				// get the parameters for the xml index part from the requets
				getParameters();
			} else if (XMLQCLIB_IsSubCommand(_req,(char *)TEXT_CONSTANT_QCL_CMD_QUERY_SEARCH)) {
				/* subcommand: add a new selection block */
				getDocClasses();
				_isAddLine = XMLQCLIB_False;
				_isSetDc = XMLQCLIB_False;
				_isSearch = XMLQCLIB_True;
				// get the parameters for the xml index part from the requets
				getParameters();
				// prepare search
				XMLQCLIB_BuildWhereClause(_queryParamsList,&_whereClause);
				// perform search
				Error_Clear(&_error);
				if (!XMLQSLIB_FreeSearchEmbedded(qsHandle,_whereClause,&_error)) {
					if (_error.code != XMLERROR_ERR_CODE_UNDEFINED) {
						/* something happend -> write to log */
						sp77sprintf(__msg,4095,"[XMLQSLIB::XMLIDMLib_HtmlTemplate_FreeQuery::Retrieve Documents] %s; [ODBC-Code:%ld][ODBC-State:%s] %s; [Location: %s:%d]\n",_error.text,_error.odbcCode,_error.odbcState,_error.odbcText,_error.file,_error.line);
						wa.WriteLogMsg(__msg);
					}
					_resultError = XMLQCLIB_True;
				} else {
					_resultError = XMLQCLIB_False;
				}
				_resultcount = 1;
				_firstResultRow = XMLQCLIB_True;
			}
		}

		// prepare for output
		_outXmlIndexLine = 0;
		_outDocClassLine = 0;
		_firstQueryParam = XMLQCLIB_True;
		_noResult = XMLQCLIB_True;
	} 


	XMLQCLib_HtmlTemplate_FreeQuery::~XMLQCLib_HtmlTemplate_FreeQuery() 
	{
		/* delete all document classes */
		XMLIMAPI_DocClassListDestroy(_dcListAll);
	}

	bool XMLQCLib_HtmlTemplate_FreeQuery :: askForContinue ( const Tools_DynamicUTF8String & szName )
	{
		if (szName == TEMPLATE_KEY_QUERY_FREE_RESULT_ROW) {
			Error_Clear(&_error);
			strcpy(_docUri,"");
			if (!XMLQSLIB_FreeSearchEmbeddedGetDocs(_qsHandle,_docUri,1000,&_error)) {
				return XMLQCLIB_False;
			} else {
				_noResult = XMLQCLIB_False;
				return XMLQCLIB_True;
			}
		}
		return XMLQCLIB_False;
	}

	SAPDB_Int2 XMLQCLib_HtmlTemplate_FreeQuery :: askForWriteCount(const Tools_DynamicUTF8String & szName)
	{
		XMLQCLIB_UInt4 __nReturn = 0;

		if (szName == TEMPLATE_KEY_QUERY_FREE_SERVICE) {
			__nReturn = 1;
		}
		else if (szName == TEMPLATE_KEY_QUERY_FREE_DOC_CLASS_LINES) {
			__nReturn = 1;
		}
		else if (szName == TEMPLATE_KEY_QUERY_FREE_DOC_CLASS_IDX) {
			if (XMLQCLIB_QueryParamsListGetLength(_queryParamsList) > 0)
				__nReturn = _dcLines;
			else
				__nReturn = 1;
			_firstDcIdx = XMLQCLIB_True;
		}
		else if (szName == TEMPLATE_KEY_QUERY_FREE_DOC_CLASS_NAME) {
			__nReturn = 1;
		}
		else if (szName == TEMPLATE_KEY_QUERY_FREE_XML_INDEX) {
			if (_queryParams->idxListLength > 0) 
				__nReturn = _queryParams->idxLines;
			else
				__nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_QUERY_FREE_NO_XML_INDEX) {
			if (_queryParams->idxListLength > 0) 
				__nReturn = 0;
			else
				__nReturn = 1;
		}
		else if (szName == TEMPLATE_KEY_QUERY_FREE_INDEX) {
			__nReturn = _queryParams->idxListLength;
		}
		else if (szName == TEMPLATE_KEY_QUERY_FREE_INDEX_SEL) {
			__nReturn = 1;
		}
		else if (szName == TEMPLATE_KEY_QUERY_FREE_DC) {
			__nReturn = _dcCountAll;
		}
		else if (szName == TEMPLATE_KEY_QUERY_FREE_DC_SEL_ID) {
			_firstDcSel = XMLQCLIB_True;
			__nReturn = 1;
		}
		else if (szName == TEMPLATE_KEY_QUERY_FREE_DC_SEL) {
			if (_outXmlIndexLine <= 1)
				__nReturn = 1;
			else
				__nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_QUERY_FREE_XML_INDEX_VALUE_COUNT) {
			__nReturn = 1;
		}
		else if (szName == TEMPLATE_KEY_QUERY_FREE_XML_INDEX_LINES) {
			__nReturn = 1;
		}
		else if (szName == TEMPLATE_KEY_QUERY_FREE_XML_INDEX_LOG_OP_1) {
			if (_queryParamsIdx->valueCount >= 1)
				__nReturn = 1;
			else
				__nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_QUERY_FREE_XML_INDEX_LOG_OP_2) {
			if (_queryParamsIdx->valueCount >= 2)
				__nReturn = 1;
			else
				__nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_QUERY_FREE_XML_INDEX_LOG_OP_3) {
			if (_queryParamsIdx->valueCount >= 3)
				__nReturn = 1;
			else
				__nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_QUERY_FREE_XML_INDEX_LOG_OP_4) {
			if (_queryParamsIdx->valueCount >= 4)
				__nReturn = 1;
			else
				__nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_QUERY_FREE_XML_INDEX_LOG_OP_5) {
			if (_queryParamsIdx->valueCount >= 5)
				__nReturn = 1;
			else
				__nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_QUERY_FREE_XML_INDEX_INPUT_1) {
			if (_queryParamsIdx->valueCount >= 1)
				__nReturn = 1;
			else
				__nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_QUERY_FREE_XML_INDEX_INPUT_2) {
			if (_queryParamsIdx->valueCount >= 2)
				__nReturn = 1;
			else
				__nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_QUERY_FREE_XML_INDEX_INPUT_3) {
			if (_queryParamsIdx->valueCount >= 3)
				__nReturn = 1;
			else
				__nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_QUERY_FREE_XML_INDEX_INPUT_4) {
			if (_queryParamsIdx->valueCount >= 4)
				__nReturn = 1;
			else
				__nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_QUERY_FREE_XML_INDEX_INPUT_5) {
			if (_queryParamsIdx->valueCount >= 5)
				__nReturn = 1;
			else
				__nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_QUERY_FREE_XML_INDEX_OR_1) {
			if (_queryParamsIdx->valueCount == 1)
				__nReturn = 1;
			else
				__nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_QUERY_FREE_XML_INDEX_OR_2) {
			if (_queryParamsIdx->valueCount == 2)
				__nReturn = 1;
			else
				__nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_QUERY_FREE_XML_INDEX_OR_3) {
			if (_queryParamsIdx->valueCount == 3)
				__nReturn = 1;
			else
				__nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_QUERY_FREE_XML_INDEX_OR_4) {
			if (_queryParamsIdx->valueCount == 4)
				__nReturn = 1;
			else
				__nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_QUERY_FREE_XML_INDEX_OR_TXT_1) {
			if (_queryParamsIdx->valueCount > 1)
				__nReturn = 1;
			else
				__nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_QUERY_FREE_XML_INDEX_OR_TXT_2) {
			if (_queryParamsIdx->valueCount > 2)
				__nReturn = 1;
			else
				__nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_QUERY_FREE_XML_INDEX_OR_TXT_3) {
			if (_queryParamsIdx->valueCount > 3)
				__nReturn = 1;
			else
				__nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_QUERY_FREE_XML_INDEX_OR_TXT_4) {
			if (_queryParamsIdx->valueCount > 4)
				__nReturn = 1;
			else
				__nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_QUERY_FREE_XML_INDEX_AND) {
			if (_queryParams->idxListLength > 0 && _outXmlIndexLine == _queryParams->idxLines) 
				__nReturn = 1;
			else
				__nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_QUERY_FREE_XML_INDEX_AND_TXT) {
			if (_queryParams->idxListLength > 0 && _outXmlIndexLine < _queryParams->idxLines) 
				__nReturn = 1;
			else
				__nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_QUERY_FREE_XML_INDEX_ORDC) {
			if (_outDocClassLine == XMLQCLIB_QueryParamsListGetLength(_queryParamsList)) 
				__nReturn = 1;
			else
				__nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_QUERY_FREE_XML_INDEX_ORDC_TXT) {
			if (_outDocClassLine < XMLQCLIB_QueryParamsListGetLength(_queryParamsList)) 
				__nReturn = 1;
			else
				__nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_QUERY_FREE_EXPLAIN_NO_DC) {
			if (_outDocClassLine == 1) 
				__nReturn = 1;
			else
				__nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_QUERY_FREE_EXPLAIN_DC) {
			if (_outDocClassLine > 1) 
				__nReturn = 1;
			else
				__nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_QUERY_FREE_RESULT) {
			if (_isSearch) 
				__nReturn = 1;
			else
				__nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_QUERY_FREE_RESULT_ROW) {
			if (_resultError == XMLQCLIB_False)
				if (_firstResultRow == XMLQCLIB_True)
					__nReturn = -1;
				else
					__nReturn = 0;
			else
				__nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_QUERY_FREE_NO_RESULT_ROW) {
			if (_resultError == XMLQCLIB_False && _noResult == XMLQCLIB_True)
				if (_firstResultRow == XMLQCLIB_True)
					__nReturn = 1;
				else
					__nReturn = 0;
			else
				__nReturn = 0;
		}
		else if (szName == TEMPLATE_KEY_QUERY_FREE_ERROR_ROW) {
			if (_resultError == XMLQCLIB_False)
				__nReturn = 0;
			else
				__nReturn = 1;
		}
		return __nReturn;
	} 


	Tools_DynamicUTF8String XMLQCLib_HtmlTemplate_FreeQuery :: askForValue (const Tools_DynamicUTF8String & szName)
	{
		XMLQCLIB_Char			__value[513];
		Tools_DynamicUTF8String __buffer;
		XMLQCLIB_Char			__string[4096];

		if (szName == TEMPLATE_VALUE_QUERY_FREE_SERVICE) {
			__buffer.ConvertFromASCII_Latin1(_serviceName,_serviceName+strlen(_serviceName));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_QUERY_FREE_DOC_CLASS_LINES) {
			sp77sprintf(__value,513,"%d",_dcLines);
			__buffer.ConvertFromASCII_Latin1(__value,__value+strlen(__value));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_QUERY_FREE_DOC_CLASS_LINE) {
			sp77sprintf(__value,513,"%d",_outDocClassLine);
			__buffer.ConvertFromASCII_Latin1(__value,__value+strlen(__value));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_QUERY_FREE_DOCCLASS_SEL_ID) {
			if (_firstDcSel) {
				_firstDcSel = XMLQCLIB_False;
				XMLIMAPI_DocClassListGetFirst(_dcListAll,&_docClass);
			} else {
				XMLIMAPI_DocClassListGetNext(_dcListAll,&_docClass);
			}
			__buffer.ConvertFromASCII_Latin1(_docClass->docClassID,_docClass->docClassID+strlen(_docClass->docClassID));
			return __buffer;
		} 
		else if (szName == TEMPLATE_VALUE_QUERY_FREE_DOCCLASS_SEL_NAME) {
			XMLQCLIB_EscapeQuotes(_docClass->docClassName,strlen(_docClass->docClassName),__string);
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			return __buffer;
		} 
		else if (szName == TEMPLATE_VALUE_QUERY_FREE_DOCCLASS_SEL) {
			if (strcmp(_queryParams->docClassId,_docClass->docClassID)==0)
				sp77sprintf(__value,513,"selected");
			else
				sp77sprintf(__value,513,"");
			__buffer.ConvertFromASCII_Latin1(__value,__value+strlen(__value));
			return __buffer;
		} 
		else if (szName == TEMPLATE_VALUE_QUERY_FREE_XML_INDEX_GEN_ID) {
			sp77sprintf(__value,512,"IDX%d",_outXmlIndexLine);
			__buffer.ConvertFromASCII_Latin1(__value,__value+strlen(__value));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_QUERY_FREE_DOC_CLASS_GEN_ID) {
			__buffer.ConvertFromASCII_Latin1(_queryParams->genId,_queryParams->genId+strlen(_queryParams->genId));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_QUERY_FREE_XML_INDEX_VALUE_COUNT) {
			_compop = 0;
			if (_outXmlIndexLine == 0) {
				XMLQCLIB_QueryParamsIdxListGetFirst(_queryParams->idxQueryParams,&_queryParamsIdx);
				_firstDcSel = XMLQCLIB_True;
			} else {
				XMLQCLIB_QueryParamsIdxListGetNext(_queryParams->idxQueryParams,&_queryParamsIdx);
			}
			_outXmlIndexLine++;
			_firstIdx = XMLQCLIB_True;
			sp77sprintf(__value,512,"%d",_queryParamsIdx->valueCount);
			__buffer.ConvertFromASCII_Latin1(__value,__value+strlen(__value));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_QUERY_FREE_XML_INDEX_LINES) {
			_outXmlIndexLine = 0;
			_outDocClassLine++;
			if (_firstQueryParam) {
				_firstQueryParam = XMLQCLIB_False;
				XMLQCLIB_QueryParamsListGetFirst(_queryParamsList,&_queryParams);
			} else {
				XMLQCLIB_QueryParamsListGetNext(_queryParamsList,&_queryParams);
			}
			// check whether the doc class is still a selected one
/*			if ((strcmp(_queryParams->docClassId,"")!=0) && !isSelectedDocClass(_queryParams->docClassId)) {
				while (XMLQCLIB_QueryParamsListGetNext(_queryParamsList,&_queryParams)) {
					if (isSelectedDocClass(_queryParams->docClassId))
						break;
				}
			}
*/			sp77sprintf(__value,512,"%d",_queryParams->idxLines);
			__buffer.ConvertFromASCII_Latin1(__value,__value+strlen(__value));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_QUERY_FREE_OR_LINE) {
			sp77sprintf(__value,512,"%d",_outXmlIndexLine);
			__buffer.ConvertFromASCII_Latin1(__value,__value+strlen(__value));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_QUERY_FREE_DOC_CLASS_ID) {
			__buffer.ConvertFromASCII_Latin1(_queryParams->docClassId,_queryParams->docClassId+strlen(_queryParams->docClassId));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_QUERY_FREE_XML_INDEX_ID) {
			if (_firstIdx) {
				_firstIdx = XMLQCLIB_False;
				XMLIMAPI_XmlIndexListGetFirst(_queryParams->idxList,&_xmlIndex);
			} else {
				XMLIMAPI_XmlIndexListGetNext(_queryParams->idxList,&_xmlIndex);
			}
			__buffer.ConvertFromASCII_Latin1(_xmlIndex->IndexID,_xmlIndex->IndexID+strlen(_xmlIndex->IndexID));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_QUERY_FREE_XML_INDEX_SEL) {
			if (strcmp(_xmlIndex->IndexID,_queryParamsIdx->xmlIndexId)==0) {
				sp77sprintf(__value,512,"selected");
				__buffer.ConvertFromASCII_Latin1(__value,__value+strlen(__value));
				return __buffer;
			}
		}
		else if (szName == TEMPLATE_VALUE_QUERY_FREE_DOC_CLASS_NAME) {
			XMLQCLIB_EscapeQuotes(_queryParams->docClassName,strlen(_queryParams->docClassName),__string);
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_QUERY_FREE_XML_INDEX_NAME) {
			XMLQCLIB_EscapeQuotes(_xmlIndex->indexName,strlen(_xmlIndex->indexName),__string);
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_QUERY_FREE_INPUT_VALUE_1) {
			XMLQCLIB_EscapeQuotes(_queryParamsIdx->idxValQueryParams[0].input,strlen(_queryParamsIdx->idxValQueryParams[0].input),__string);
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_QUERY_FREE_INPUT_VALUE_2) {
			XMLQCLIB_EscapeQuotes(_queryParamsIdx->idxValQueryParams[1].input,strlen(_queryParamsIdx->idxValQueryParams[1].input),__string);
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_QUERY_FREE_INPUT_VALUE_3) {
			XMLQCLIB_EscapeQuotes(_queryParamsIdx->idxValQueryParams[2].input,strlen(_queryParamsIdx->idxValQueryParams[2].input),__string);
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_QUERY_FREE_INPUT_VALUE_4) {
			XMLQCLIB_EscapeQuotes(_queryParamsIdx->idxValQueryParams[3].input,strlen(_queryParamsIdx->idxValQueryParams[3].input),__string);
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_QUERY_FREE_INPUT_VALUE_5) {
			XMLQCLIB_EscapeQuotes(_queryParamsIdx->idxValQueryParams[4].input,strlen(_queryParamsIdx->idxValQueryParams[4].input),__string);
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_QUERY_FREE_COMP_OP_SEL_0) {
			_compop++;
			if ((_compop == 1 && _queryParamsIdx->idxValQueryParams[0].compOperator == XMLQSLIB_EQUAL) ||
				(_compop == 2 && _queryParamsIdx->idxValQueryParams[1].compOperator == XMLQSLIB_EQUAL) ||
				(_compop == 3 && _queryParamsIdx->idxValQueryParams[2].compOperator == XMLQSLIB_EQUAL) ||
				(_compop == 4 && _queryParamsIdx->idxValQueryParams[3].compOperator == XMLQSLIB_EQUAL) ||
				(_compop == 5 && _queryParamsIdx->idxValQueryParams[4].compOperator == XMLQSLIB_EQUAL)) {
				sp77sprintf(__value,512,"selected");
				__buffer.ConvertFromASCII_Latin1(__value,__value+strlen(__value));
				return __buffer;
			}
		}
		else if (szName == TEMPLATE_VALUE_QUERY_FREE_COMP_OP_SEL_1) {
			if ((_compop == 1 && _queryParamsIdx->idxValQueryParams[0].compOperator == XMLQSLIB_LESS) ||
				(_compop == 2 && _queryParamsIdx->idxValQueryParams[1].compOperator == XMLQSLIB_LESS) ||
				(_compop == 3 && _queryParamsIdx->idxValQueryParams[2].compOperator == XMLQSLIB_LESS) ||
				(_compop == 4 && _queryParamsIdx->idxValQueryParams[3].compOperator == XMLQSLIB_LESS) ||
				(_compop == 5 && _queryParamsIdx->idxValQueryParams[4].compOperator == XMLQSLIB_LESS)) {
				sp77sprintf(__value,512,"selected");
				__buffer.ConvertFromASCII_Latin1(__value,__value+strlen(__value));
				return __buffer;
			}
		}
		else if (szName == TEMPLATE_VALUE_QUERY_FREE_COMP_OP_SEL_2) {
			if ((_compop == 1 && _queryParamsIdx->idxValQueryParams[0].compOperator == XMLQSLIB_GREATER) ||
				(_compop == 2 && _queryParamsIdx->idxValQueryParams[1].compOperator == XMLQSLIB_GREATER) ||
				(_compop == 3 && _queryParamsIdx->idxValQueryParams[2].compOperator == XMLQSLIB_GREATER) ||
				(_compop == 4 && _queryParamsIdx->idxValQueryParams[3].compOperator == XMLQSLIB_GREATER) ||
				(_compop == 5 && _queryParamsIdx->idxValQueryParams[4].compOperator == XMLQSLIB_GREATER)) {
				sp77sprintf(__value,512,"selected");
				__buffer.ConvertFromASCII_Latin1(__value,__value+strlen(__value));
				return __buffer;
			}
		}
		else if (szName == TEMPLATE_VALUE_QUERY_FREE_COMP_OP_SEL_3) {
			if ((_compop == 1 && _queryParamsIdx->idxValQueryParams[0].compOperator == XMLQSLIB_LESS_EQUAL) ||
				(_compop == 2 && _queryParamsIdx->idxValQueryParams[1].compOperator == XMLQSLIB_LESS_EQUAL) ||
				(_compop == 3 && _queryParamsIdx->idxValQueryParams[2].compOperator == XMLQSLIB_LESS_EQUAL) ||
				(_compop == 4 && _queryParamsIdx->idxValQueryParams[3].compOperator == XMLQSLIB_LESS_EQUAL) ||
				(_compop == 5 && _queryParamsIdx->idxValQueryParams[4].compOperator == XMLQSLIB_LESS_EQUAL)) {
				sp77sprintf(__value,512,"selected");
				__buffer.ConvertFromASCII_Latin1(__value,__value+strlen(__value));
				return __buffer;
			}
		}
		else if (szName == TEMPLATE_VALUE_QUERY_FREE_COMP_OP_SEL_4) {
			if ((_compop == 1 && _queryParamsIdx->idxValQueryParams[0].compOperator == XMLQSLIB_GREATER_EQUAL) ||
				(_compop == 2 && _queryParamsIdx->idxValQueryParams[1].compOperator == XMLQSLIB_GREATER_EQUAL) ||
				(_compop == 3 && _queryParamsIdx->idxValQueryParams[2].compOperator == XMLQSLIB_GREATER_EQUAL) ||
				(_compop == 4 && _queryParamsIdx->idxValQueryParams[3].compOperator == XMLQSLIB_GREATER_EQUAL) ||
				(_compop == 5 && _queryParamsIdx->idxValQueryParams[4].compOperator == XMLQSLIB_GREATER_EQUAL)) {
				sp77sprintf(__value,512,"selected");
				__buffer.ConvertFromASCII_Latin1(__value,__value+strlen(__value));
				return __buffer;
			}
		}
		else if (szName == TEMPLATE_VALUE_QUERY_FREE_COMP_OP_SEL_5) {
			if ((_compop == 1 && _queryParamsIdx->idxValQueryParams[0].compOperator == XMLQSLIB_NOT_EQUAL) ||
				(_compop == 2 && _queryParamsIdx->idxValQueryParams[1].compOperator == XMLQSLIB_NOT_EQUAL) ||
				(_compop == 3 && _queryParamsIdx->idxValQueryParams[2].compOperator == XMLQSLIB_NOT_EQUAL) ||
				(_compop == 4 && _queryParamsIdx->idxValQueryParams[3].compOperator == XMLQSLIB_NOT_EQUAL) ||
				(_compop == 5 && _queryParamsIdx->idxValQueryParams[4].compOperator == XMLQSLIB_NOT_EQUAL)) {
				sp77sprintf(__value,512,"selected");
				__buffer.ConvertFromASCII_Latin1(__value,__value+strlen(__value));
				return __buffer;
			}
		}
		else if (szName == TEMPLATE_VALUE_QUERY_FREE_RESULT_COUNT) {
			sp77sprintf(__value,512,"%d",_resultcount++);
			__buffer.ConvertFromASCII_Latin1(__value,__value+strlen(__value));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_QUERY_FREE_RESULT_LINE) {
			sp77sprintf(__value,512,"%s%s",_server,_docUri);
			__buffer.ConvertFromASCII_Latin1(__value,__value+strlen(__value));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_QUERY_FREE_ERROR_LINE) {
			sp77sprintf(__string,4095,"Internal Error: %s; [ODBC-Code:%ld][ODBC-State:%s] %s; [Location: %s:%d]\n",_error.text,_error.odbcCode,_error.odbcState,_error.odbcText,_error.file,_error.line);
			__buffer.ConvertFromASCII_Latin1(__string,__string+strlen(__string));
			return __buffer;
		}
		return "";
	}

	void XMLQCLib_HtmlTemplate_FreeQuery::getDocClasses()
	{
		XMLIMAPI_DocClass			__docClass;
		XMLIMAPI_ErrorItem			__errorItem;
		XMLIMAPI_ErrorType			__errorType;
		XMLIMAPI_ErrorText			__errorMsg;
		XMLQCLIB_Char				__msg[1024];
		
		_dcCountAll = 0;
		if (XMLIMAPI_DocClassGetFirst(_xml,&__docClass)) {
			
			do {
				XMLIMAPI_DocClassListAddItem(_dcListAll,&__docClass);
				_dcCountAll++;						
			} while (XMLIMAPI_DocClassGetNext(_xml,&__docClass));


		} else {
			if (!XMLIMAPI_IsError(_xml,XMLIMAPI_ERR_TYPE_MAPI,XMLIMAPI_ERR_MAPI_CODE_NO_MORE_DOCCLASSES) &&
				!XMLIMAPI_IsError(_xml,XMLIMAPI_ERR_TYPE_MAPI,XMLIMAPI_ERR_MAPI_CODE_NO_DOCCLASS)) {
				XMLIMAPI_GetLastError(_xml,&__errorItem);
				XMLIMAPI_GetErrorType(__errorItem,&__errorType);
				XMLIMAPI_GetErrorText(__errorItem,&__errorMsg);
				sp77sprintf(__msg,1023,"[XMLQCLIB::XMLQCLib_HtmlTemplate_FreeQuery::getDocClasses] %s\n",__errorMsg);
				_wa.WriteLogMsg(__msg);
			}
		}

	}

	void XMLQCLib_HtmlTemplate_FreeQuery::getParameters()
	{
		XMLQCLIB_UInt4					i,j,dc;
		XMLQCLIB_Char					__paramName[513];
		XMLQCLIB_Char					__value[513];
		XMLQCLIB_QueryParams			__queryParams;
		XMLQCLIB_QueryParamsIdx			__queryParamsIdx;
		XMLIMAPI_Id						__docClassId;
		XMLIMAPI_XmlIndex				__xmlIndex;
		XMLIMAPI_XmlIndex				*__pXmlIndex;
		XMLIMAPI_ErrorItem				__errorItem;
		XMLIMAPI_ErrorType				__errorType;
		XMLIMAPI_ErrorText				__errorMsg;
		XMLQCLIB_Char					__msg[1024];
		XMLQCLIB_UInt4					__setDcLine;
		XMLIMAPI_DocClass				*__docClass;

		XMLQCLIB_QueryParamsListCreate(&_queryParamsList);
		// get the amount of document class blocks
		_dcLines = 1;
		if (XMLQCLIB_GetParameterValue("DcLines",_req,__value)) {
			if (strcmp(__value,"")!=0)
				_dcLines = atol(__value);
			else
				_dcLines = 1;
		}
		if (_isSetDc) {
			if (XMLQCLIB_GetParameterValue("line",_req,__value)) {
				if (strcmp(__value,"")!=0)
					__setDcLine = atol(__value);
			}
		}
		if (_isAddLine) 
			_dcLines++;
		
		dc = _dcLines;

		for (j=1;j<=dc;j++) {
			sp77sprintf(__queryParams.genId,128,"DC%d",j);

			sp77sprintf(__paramName,512,"%s_DCID",__queryParams.genId);
			if (!XMLQCLIB_GetParameterValue(__paramName,_req,__queryParams.docClassId)) {
				strcpy(__queryParams.docClassId,"");
			}

			sp77sprintf(__paramName,512,"%s_DCName",__queryParams.genId);
			if (!XMLQCLIB_GetParameterValue(__paramName,_req,__queryParams.docClassName)) {
				strcpy(__queryParams.docClassName,"");
			}

			if (strcmp(__queryParams.docClassId,"")!=0 && strcmp(__queryParams.docClassName,"")==0) {
				/* get the Doc Class name */
				if (XMLIMAPI_DocClassListGetFirst(_dcListAll,&__docClass)) {
					do {
						if (strcmp(__docClass->docClassID,__queryParams.docClassId)==0)
							strcpy(__queryParams.docClassName, __docClass->docClassName);
					} while (XMLIMAPI_DocClassListGetNext(_dcListAll,&__docClass));
				}
			}

			__queryParams.idxLines = 1;
			sp77sprintf(__paramName,512,"%s_IDXLines",__queryParams.genId,__queryParamsIdx.genId);
			if (XMLQCLIB_GetParameterValue(__paramName,_req,__value)) {
				if (strcmp(__value,"")!=0)
					__queryParams.idxLines = atol(__value);
				else
					__queryParams.idxLines = 1;
			}

			// get the xml indexes for this document class
			if (strcmp(__queryParams.docClassId,"")!=0 && strcmp(__queryParams.docClassId,"0")!=0) {
				// ... for a real document class
				XMLIMAPI_IdStringAsId(__queryParams.docClassId,__docClassId);
				if (XMLIMAPI_XmlIndexGetFirstByDocClass(_xml,__docClassId,&__xmlIndex)) {
					XMLIMAPI_XmlIndexListCreate(&__queryParams.idxList);
					__queryParams.idxListLength = 0;
					do {
						XMLIMAPI_XmlIndexListAddItem(__queryParams.idxList,&__xmlIndex);
						__queryParams.idxListLength++;
					} while (XMLIMAPI_XmlIndexGetNextByDocClass(_xml,&__xmlIndex));
				} else {
					XMLIMAPI_XmlIndexListCreate(&__queryParams.idxList);
					__queryParams.idxListLength = 0;
					if (!XMLIMAPI_IsError(_xml,XMLIMAPI_ERR_TYPE_MAPI,XMLIMAPI_ERR_MAPI_CODE_NO_MORE_INDICES) &&
						!XMLIMAPI_IsError(_xml,XMLIMAPI_ERR_TYPE_MAPI,XMLIMAPI_ERR_MAPI_CODE_NO_XMLINDEX)) {
						XMLIMAPI_GetLastError(_xml,&__errorItem);
						XMLIMAPI_GetErrorType(__errorItem,&__errorType);
						XMLIMAPI_GetErrorText(__errorItem,&__errorMsg);
						sp77sprintf(__msg,1023,"[XMLQCLIB::XMLQCLib_HtmlTemplate_FreeQuery::addSelectedDocClassToQueryParams] %s\n",__errorMsg);
						_wa.WriteLogMsg(__msg);
					}
				}
			} else {
				// ... for the list w/o a document class
				if (XMLIMAPI_XmlIndexGetFirst(_xml,&__xmlIndex)) {
					XMLIMAPI_XmlIndexListCreate(&__queryParams.idxList);
					__queryParams.idxListLength = 0;
					do {
						XMLIMAPI_XmlIndexListAddItem(__queryParams.idxList,&__xmlIndex);
						__queryParams.idxListLength++;
					} while (XMLIMAPI_XmlIndexGetNext(_xml,&__xmlIndex));
				} else {
					XMLIMAPI_XmlIndexListCreate(&__queryParams.idxList);
					__queryParams.idxListLength = 0;
					if (!XMLIMAPI_IsError(_xml,XMLIMAPI_ERR_TYPE_MAPI,XMLIMAPI_ERR_MAPI_CODE_NO_MORE_INDICES) &&
						!XMLIMAPI_IsError(_xml,XMLIMAPI_ERR_TYPE_MAPI,XMLIMAPI_ERR_MAPI_CODE_NO_XMLINDEX)) {
						XMLIMAPI_GetLastError(_xml,&__errorItem);
						XMLIMAPI_GetErrorType(__errorItem,&__errorType);
						XMLIMAPI_GetErrorText(__errorItem,&__errorMsg);
						sp77sprintf(__msg,1023,"[XMLQCLIB::XMLQCLib_HtmlTemplate_FreeQuery::addSelectedDocClassToQueryParams] %s\n",__errorMsg);
						_wa.WriteLogMsg(__msg);
					}
				}
			}

			XMLQCLIB_QueryParamsIdxListCreate(&_queryParamsIdxList);
			__queryParams.idxQueryParams = _queryParamsIdxList;

			for (i=1;i<=__queryParams.idxLines;i++) {
				/* get all index lines */
				sp77sprintf(__queryParamsIdx.genId,128,"IDX%d",i);

				if (!(_isSetDc && j==__setDcLine)) {
					sp77sprintf(__paramName,512,"%s_%s_VC",__queryParams.genId,__queryParamsIdx.genId);
					if (XMLQCLIB_GetParameterValue(__paramName,_req,__value)) {
						__queryParamsIdx.valueCount = atol(__value);
					} else {
						__queryParamsIdx.valueCount = 1;
					}
					sp77sprintf(__paramName,512,"%s_%s_IDX",__queryParams.genId,__queryParamsIdx.genId);
					if (!XMLQCLIB_GetParameterValue(__paramName,_req,__queryParamsIdx.xmlIndexId)) {
						strcpy(__queryParamsIdx.xmlIndexId,"");
					}
					// get the name of the index from the index list
					if (XMLIMAPI_XmlIndexListGetFirst(__queryParams.idxList,&__pXmlIndex)) {
						do {
							if (strcmp(__pXmlIndex->IndexID,__queryParamsIdx.xmlIndexId)==0) {
								strcpy(__queryParamsIdx.xmlIndexName,__pXmlIndex->indexName);
								break;
							}
						} while (XMLIMAPI_XmlIndexListGetNext(__queryParams.idxList,&__pXmlIndex));
					} else {
						strcpy(__queryParamsIdx.xmlIndexName,"");
					}
					sp77sprintf(__paramName,512,"%s_%s_COP1",__queryParams.genId,__queryParamsIdx.genId);
					if (XMLQCLIB_GetParameterValue(__paramName,_req,__value)) {
						__queryParamsIdx.idxValQueryParams[0].compOperator = (XMLQSLib_CompareOperator)atoi(__value);
					} else {
						__queryParamsIdx.idxValQueryParams[0].compOperator = XMLQSLIB_EQUAL;
					}
					sp77sprintf(__paramName,512,"%s_%s_COP2",__queryParams.genId,__queryParamsIdx.genId);
					if (XMLQCLIB_GetParameterValue(__paramName,_req,__value)) {
						__queryParamsIdx.idxValQueryParams[1].compOperator = (XMLQSLib_CompareOperator)atoi(__value);
					} else {
						__queryParamsIdx.idxValQueryParams[1].compOperator = XMLQSLIB_EQUAL;
					}
					sp77sprintf(__paramName,512,"%s_%s_COP3",__queryParams.genId,__queryParamsIdx.genId);
					if (XMLQCLIB_GetParameterValue(__paramName,_req,__value)) {
						__queryParamsIdx.idxValQueryParams[2].compOperator = (XMLQSLib_CompareOperator)atoi(__value);
					} else {
						__queryParamsIdx.idxValQueryParams[2].compOperator = XMLQSLIB_EQUAL;
					}
					sp77sprintf(__paramName,512,"%s_%s_COP4",__queryParams.genId,__queryParamsIdx.genId);
					if (XMLQCLIB_GetParameterValue(__paramName,_req,__value)) {
						__queryParamsIdx.idxValQueryParams[3].compOperator = (XMLQSLib_CompareOperator)atoi(__value);
					} else {
						__queryParamsIdx.idxValQueryParams[3].compOperator = XMLQSLIB_EQUAL;
					}
					sp77sprintf(__paramName,512,"%s_%s_COP5",__queryParams.genId,__queryParamsIdx.genId);
					if (XMLQCLIB_GetParameterValue(__paramName,_req,__value)) {
						__queryParamsIdx.idxValQueryParams[4].compOperator = (XMLQSLib_CompareOperator)atoi(__value);
					} else {
						__queryParamsIdx.idxValQueryParams[4].compOperator = XMLQSLIB_EQUAL;
					}
					sp77sprintf(__paramName,512,"%s_%s_IN1",__queryParams.genId,__queryParamsIdx.genId);
					XMLQCLIB_GetParameterValue(__paramName,_req,__queryParamsIdx.idxValQueryParams[0].input);
					sp77sprintf(__paramName,512,"%s_%s_IN2",__queryParams.genId,__queryParamsIdx.genId);
					XMLQCLIB_GetParameterValue(__paramName,_req,__queryParamsIdx.idxValQueryParams[1].input);
					sp77sprintf(__paramName,512,"%s_%s_IN3",__queryParams.genId,__queryParamsIdx.genId);
					XMLQCLIB_GetParameterValue(__paramName,_req,__queryParamsIdx.idxValQueryParams[2].input);
					sp77sprintf(__paramName,512,"%s_%s_IN4",__queryParams.genId,__queryParamsIdx.genId);
					XMLQCLIB_GetParameterValue(__paramName,_req,__queryParamsIdx.idxValQueryParams[3].input);
					sp77sprintf(__paramName,512,"%s_%s_IN5",__queryParams.genId,__queryParamsIdx.genId);
					XMLQCLIB_GetParameterValue(__paramName,_req,__queryParamsIdx.idxValQueryParams[4].input);
					XMLQCLIB_QueryParamsIdxListAddItem(__queryParams.idxQueryParams,&__queryParamsIdx);
				} else {
					__queryParamsIdx.valueCount = 1;
					__queryParams.idxLines = 1;
					strcpy(__queryParamsIdx.xmlIndexId,"");
					__queryParamsIdx.idxValQueryParams[0].compOperator = XMLQSLIB_EQUAL;
					__queryParamsIdx.idxValQueryParams[1].compOperator = XMLQSLIB_EQUAL;
					__queryParamsIdx.idxValQueryParams[2].compOperator = XMLQSLIB_EQUAL;
					__queryParamsIdx.idxValQueryParams[3].compOperator = XMLQSLIB_EQUAL;
					__queryParamsIdx.idxValQueryParams[4].compOperator = XMLQSLIB_EQUAL;
					strcpy(__queryParamsIdx.idxValQueryParams[0].input,"");
					strcpy(__queryParamsIdx.idxValQueryParams[1].input,"");
					strcpy(__queryParamsIdx.idxValQueryParams[2].input,"");
					strcpy(__queryParamsIdx.idxValQueryParams[3].input,"");
					strcpy(__queryParamsIdx.idxValQueryParams[4].input,"");
					XMLQCLIB_QueryParamsIdxListAddItem(__queryParams.idxQueryParams,&__queryParamsIdx);
				}
			}
			// build the new genID
			sp77sprintf(__queryParams.genId,128,"DC%d",j);
			XMLQCLIB_QueryParamsListAddItem(_queryParamsList,&__queryParams);
		}
	}


