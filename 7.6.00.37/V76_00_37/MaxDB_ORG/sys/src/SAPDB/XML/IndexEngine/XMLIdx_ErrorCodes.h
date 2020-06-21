/*!**********************************************************************

  module: XMLIdx_Error.h

  -----------------------------------------------------------------------

  responsible:	Markus Oezgen

  special area: XML Index Engine

  description:  Error Handling

  see also:

  -----------------------------------------------------------------------

                         Copyright (c) 2001-2005 SAP AG



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





************************************************************************/

#ifndef XMLIDX_ERROR_H
#define XMLIDX_ERROR_H

/***********************************************************************

	Includes

 ***********************************************************************/

#include "SAPDB/XML/Error/XMLError_ErrorCodeRanges.h"

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Defines

 ***********************************************************************/

#define XMLIDX_ERR_TEXT_NO_DOC_TO_INDEX                     "No document found to index"
#define XMLIDX_ERR_TEXT_STOP_WORKER                         "Stop worker"
#define XMLIDX_ERR_TEXT_XPATH_PARSE_ERROR                   "XPath parse error"
#define XMLIDX_ERR_TEXT_CANNOT_INIT_THREADING               "Can not initialize threading"
#define XMLIDX_ERR_TEXT_SESSION_NOT_FOUND                   "Session not found"
#define XMLIDX_ERR_TEXT_CAN_NOT_CONNECT_TO_REG_DB           "Can not connect to registry database"
#define XMLIDX_ERR_TEXT_COULD_NOT_SET_STATE_FINISHED        "Can not set state finished"
#define XMLIDX_ERR_TEXT_BUFFER_IS_TOO_SMALL                 "Buffer is to small"
#define XMLIDX_ERR_TEXT_UNKNOWN_STATE                       "Unknown state"
#define XMLIDX_ERR_TEXT_COULD_NOT_DETERMINE_CONTENT_LENGTH  "Can not determine content length"

/*---------------------------------------------------------------------------*/

/* XML Index engine error codes */
enum en_xmlidx_error_code {
    XMLIDX_ERR_CODE_UNDEFINED = 0,
    XMLIDX_ERR_CODE_NO_DOC_TO_INDEX = XMLIDX_ERR_CODE_RANGE,
    XMLIDX_ERR_CODE_STOP_WORKER,
    XMLIDX_ERR_CODE_XPATH_PARSE_ERROR,
    XMLIDX_ERR_CODE_CANNOT_INIT_THREADING,
    XMLIDX_ERR_CODE_SESSION_NOT_FOUND,
    XMLIDX_ERR_CODE_CAN_NOT_CONNECT_TO_REG_DB,
    XMLIDX_ERR_CODE_COULD_NOT_SET_STATE_FINISHED,
    XMLIDX_ERR_CODE_BUFFER_IS_TOO_SMALL,
    XMLIDX_ERR_CODE_UNKNOWN_STATE,
    XMLIDX_ERR_CODE_COULD_NOT_DETERMINE_CONTENT_LENGTH,
    XMLIDX_ERR_CODE_CAN_NOT_GET_INDEXING_DESCRIPTION
};

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Structs & Types

 ***********************************************************************/

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Prototypes

 ***********************************************************************/

/***********************************************************************

	End

 ***********************************************************************/

#endif
