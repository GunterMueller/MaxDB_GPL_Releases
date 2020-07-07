#ifndef XMLQCLIB_COMMONINCLUDE_HPP
#define XMLQCLIB_COMMONINCLUDE_HPP
/****************************************************************************

  module      : XMLQCLib_CommonInclude.hpp

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-06-20  15:53
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

	/*------------------------------------------------------------------------
		Defines
	--------------------------------------------------------------------------*/
	/*------------------------------------------------------------------------
		Includes
	--------------------------------------------------------------------------*/
	#include "SAPDB/XML/QueryClientLib/XMLQCLib_Types.h"
	#include "SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp"
	#include "SAPDB/ToolsCommon/Tools_Template.hpp"
	#include "hsp77.h"
	#include "sapdbwa.h" //nocheck
	#include "hwd07dbc.h"
	#include "hwd26err.h"

	struct st_xmlqclib_idx_value_query_params {
		XMLQSLib_CompareOperator	compOperator;
		XMLQCLIB_Char				input[512];
	};
	typedef st_xmlqclib_idx_value_query_params	XMLQCLIB_QueryParamsIdxVal;

	struct st_xmlqclib_idx_query_params {
		XMLQCLIB_Char					xmlIndexId[49];
		XMLQCLIB_Char					xmlIndexName[513];
		XMLQCLIB_Char					genId[129];
		XMLQCLIB_UInt4					valueCount;
		XMLQCLIB_QueryParamsIdxVal		idxValQueryParams[5];
	};
	typedef st_xmlqclib_idx_query_params	XMLQCLIB_QueryParamsIdx;

	struct st_xmlqclib_idx_query_params_list_item {
		XMLQCLIB_QueryParamsIdx						  *item;
		struct st_xmlqclib_idx_query_params_list_item    *next;
	};
	typedef struct st_xmlqclib_idx_query_params_list_item	*XMLQCLIB_QueryParamsIdxListItem;

	struct st_xmlqclib_idx_query_params_list {
		XMLQCLIB_QueryParamsIdxListItem		first;
		XMLQCLIB_QueryParamsIdxListItem		last;
		XMLQCLIB_QueryParamsIdxListItem		curr;
	};
	typedef struct st_xmlqclib_idx_query_params_list			*XMLQCLIB_QueryParamsIdxList;


	struct st_xmlqclib_query_params {
		XMLQCLIB_Char					genId[129];
		XMLQCLIB_Char					docClassId[49];
		XMLQCLIB_Char					docClassName[513];
		XMLQCLIB_UInt4					idxLines;
		XMLQCLIB_QueryParamsIdxList	idxQueryParams;
		XMLIMAPI_XmlIndexList			idxList;
		XMLIMAPI_UInt4					idxListLength;
	};
	typedef st_xmlqclib_query_params		XMLQCLIB_QueryParams;

	struct st_xmlqclib_query_params_list_item {
		XMLQCLIB_QueryParams						  *item;
		struct st_xmlqclib_query_params_list_item    *next;
	};
	typedef struct st_xmlqclib_query_params_list_item	*XMLQCLIB_QueryParamsListItem;

	struct st_xmlqclib_query_params_list {
		XMLQCLIB_QueryParamsListItem		first;
		XMLQCLIB_QueryParamsListItem		last;
		XMLQCLIB_QueryParamsListItem		curr;
		XMLQCLIB_UInt4						count;
	};
	typedef struct st_xmlqclib_query_params_list			*XMLQCLIB_QueryParamsList;

#endif	
