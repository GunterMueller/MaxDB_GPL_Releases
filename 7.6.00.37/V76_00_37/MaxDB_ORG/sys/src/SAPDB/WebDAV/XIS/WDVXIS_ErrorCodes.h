/*!**********************************************************************

  module: WDVXIS_ErrorCodes.h

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: WebDAV XML Indexing Service

  description:  Error handling

  see also:

  -----------------------------------------------------------------------

                         Copyright (c) 2000-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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




************************************************************************/

#ifndef WDVXIS_ERROR_H
#define WDVXIS_ERROR_H

/***********************************************************************

	Includes

 ***********************************************************************/

#include "SAPDB/XML/Error/XMLError_ErrorCodeRanges.h"

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Defines

 ***********************************************************************/

#define WDVXIS_ERR_TEXT_INTERNAL_ERROR                      "Internal error"
#define WDVXIS_ERR_TEXT_COULD_NOT_DETERMINE_CONTENT_LENGTH  "Could not determine content length"
#define WDVXIS_ERR_TEXT_BUFFER_IS_TOO_SMALL                 "Buffer too small"
#define WDVXIS_ERR_TEXT_INVALID_ODBC_HANDLE                 "Invalid ODBC handle"
#define WDVXIS_ERR_TEXT_NO_DATA                             "No data"
#define WDVXIS_ERR_TEXT_SUCCESS                             ""
#define WDVXIS_ERR_TEXT_UNKNOWN_ODBC_RETURN_CODE            "Unknown ODBC return code"
#define WDVXIS_ERR_TEXT_COULD_NOT_SET_INDEX_VALUE           "Could not set XML index value"
#define WDVXIS_ERR_TEXT_UNKNOWN_NAME_SPACE                  "Unknown name space"
#define WDVXIS_ERR_TEXT_COULD_NOT_GET_NEXT_UNIQUE_ID        "Could not get next unique id"
#define WDVXIS_ERR_TEXT_UNKNOWN_STATE                       "Unknown state"
#define WDVXIS_ERR_TEXT_ODBC                                "ODBC error"
#define WDVXIS_ERR_TEXT_RETRIEVE_DOCS_FAILED                "Retrieving of documents failed"
#define WDVXIS_ERR_TEXT_NO_DOCS_FOUND                       "No document found"
#define WDVXIS_ERR_TEXT_CAN_NOT_GET_DOC_PATH                "Could not get document path"
#define WDVXIS_ERR_TEXT_NO_MORE_PARENTS                     "No more parent resource"

#define WDVXIS_ERR_ODBC_STATE_NO_ROWS_UPDATED_OR_DELETED    "01S03" /* No rows updated or deleted   */

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Structs & Types

 ***********************************************************************/

/* WebDAV XML indexing service error codes */
enum en_wdvxis_error_code {
    WDVXIS_ERR_CODE_UNDEFINDE = XMLWIS_ERR_CODE_RANGE,
    WDVXIS_ERR_CODE_INTERNAL_ERROR,
    WDVXIS_ERR_CODE_COULD_NOT_DETERMINE_CONTENT_LENGTH,
    WDVXIS_ERR_CODE_BUFFER_IS_TOO_SMALL,
    WDVXIS_ERR_CODE_INVALID_ODBC_HANDLE,
    WDVXIS_ERR_CODE_NO_DATA,
    WDVXIS_ERR_CODE_UNKNOWN_ODBC_RETURN_CODE,
    WDVXIS_ERR_CODE_COULD_NOT_SET_INDEX_VALUE,
    WDVXIS_ERR_CODE_UNKNOWN_NAME_SPACE,
    WDVXIS_ERR_CODE_COULD_NOT_GET_NEXT_UNIQUE_ID,
    WDVXIS_ERR_CODE_UNKNOWN_STATE,
    WDVXIS_ERR_CODE_ODBC,
    WDVXIS_ERR_CODE_RETRIEVE_DOCS_FAILED,
    WDVXIS_ERR_CODE_NO_DOCS_FOUND,
    WDVXIS_ERR_CODE_CAN_NOT_GET_DOC_PATH,
    WDVXIS_ERR_CODE_NO_MORE_PARENTS
};

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Prototypes

 ***********************************************************************/

/***********************************************************************

	End

 ***********************************************************************/

#endif
