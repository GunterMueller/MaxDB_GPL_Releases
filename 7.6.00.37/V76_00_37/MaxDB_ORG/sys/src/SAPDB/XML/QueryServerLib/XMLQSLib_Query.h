/*


    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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



*/
#ifndef XMLQSLIB_QUERY_HPP
#define XMLQSLIB_QUERY_HPP

/*=============================================================================
 *
 *  INTERFACE FOR WEBDAV XML INDEXING SEARCH LIBRARIES
 *
 *  This file defines the interface to implement if a library shall be used as
 *  Search Library for the WebDAV XML Indexing Query Server or for the embedded
 *  WebDAV XML Indexing Query HTML GUI.
 *
 *=============================================================================*/

	/*------------------------------------------------------------------------
		Includes
	--------------------------------------------------------------------------*/
	#include "SAPDB/XML/SessionPool/XMLSP_SessionPool.h"
	#include "SAPDB/XML/Library/XMLLib_Library.h"
	#include "SAPDB/XML/Error/XMLError_Error.h"
	#include "SAPDB/XML/QueryServerLib/XMLQSLib_Types.h"

	/* QueryServerLib Handle */
	struct st_xmlqslib_handle {
		SQLHDBC						hdbc;
		SQLHENV						henv;
		XMLLib_Library				hLibrary;
		XMLLib_Function				retrieveDocs;
		XMLLib_Function				getRetrievedDocs;
		void						*userData;
        XMLLib_Function				funcDestroyUserData;
		XMLLib_LibraryNameString	libraryName;
	};
	typedef struct st_xmlqslib_handle				*XMLQSLib_Handle;

	/* Searching (WHERE clause) */
	enum en_xmlqslib_logOperator {
		XMLQSLIB_LOGOPERATOR_NOOP,
		XMLQSLIB_LOGOPERATOR_OR,
		XMLQSLIB_LOGOPERATOR_AND
	};
	typedef enum en_xmlqslib_logOperator			XMLQSLib_LogOperator;

	enum en_xmlqslib_idxObject {
		XMLQSLIB_IDXOBJECT_NOOBJ,
		XMLQSLIB_IDXOBJECT_DOCCLASS,
		XMLQSLIB_IDXOBJECT_XMLINDEX
	};
	typedef enum en_xmlqslib_idxObject				XMLQSLib_IdxObject;

	enum en_xmlqslib_compare {
		XMLQSLIB_EQUAL,
		XMLQSLIB_LESS,
		XMLQSLIB_GREATER,
		XMLQSLIB_LESS_EQUAL,
		XMLQSLIB_GREATER_EQUAL,
		XMLQSLIB_NOT_EQUAL
	};
	typedef enum en_xmlqslib_compare				XMLQSLib_CompareOperator;

	struct st_xmlqslib_where_row {
		XMLQSLib_LogOperator			logOperator;
		XMLQSLIB_UInt2					openParenthesis;
		XMLQSLib_IdxObject				idxObject;
		XMLQSLIB_Char					name[513];
		XMLQSLib_CompareOperator		compareOperator;
		XMLQSLIB_Char					value[513];
		XMLQSLIB_UInt2					closeParenthesis;
		struct st_xmlqslib_where_row	*nextRow;
	};
	typedef struct st_xmlqslib_where_row			*XMLQSLib_WhereRow;

	struct st_xmlqslib_where_clause {
		XMLQSLib_WhereRow				row;
	};
	typedef struct st_xmlqslib_where_clause			*XMLQSLib_WhereClause;

	typedef char									*XMLQSLib_StringValue;


#endif