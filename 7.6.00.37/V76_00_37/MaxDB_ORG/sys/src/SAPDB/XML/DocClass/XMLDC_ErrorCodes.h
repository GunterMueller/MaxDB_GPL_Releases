/*!**********************************************************************

  module: XMLDC_ErrorCodes.h

  -----------------------------------------------------------------------

  responsible:	Markus Oezgen

  special area: XML Document Class Handling

  description:  Error Codes

  see also:

  -----------------------------------------------------------------------

                         Copyright (c) 2003-2005 SAP AG



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





************************************************************************/

#ifndef XMLDC_ERROR_CODES_H
#define XMLDC_ERROR_CODES_H

/***********************************************************************

	Includes

 ***********************************************************************/

#include "SAPDB/XML/Error/XMLError_ErrorCodeRanges.h"

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Defines

 ***********************************************************************/

#define XMLDC_ERR_TEXT_DOC_CLASS_NOT_FOUND                  "Document class not found"
#define XMLDC_ERR_TEXT_XPATH_SYNTAX_ERROR                   "XPath syntax error"
#define XMLDC_ERR_TEXT_ACCESS_TO_XPATH_ENGINE_FAILED        "Access to XPath engine failed"
#define XMLDC_ERR_TEXT_CAN_NOT_GET_DOC_CLASS_LIST           "Can not get doc class list"
#define XMLDC_ERR_TEXT_CAN_NOT_CREATE_DOC_CLASS_ITEM        "Can not create doc class item"
#define XMLDC_ERR_TEXT_COULD_NOT_SET_XPATH                  "Could not set XPath"
#define XMLDC_ERR_TEXT_COULD_NOT_GET_INDEX_FOR_DOC_CLASS    "Could not get indexes for a doc class"

/*---------------------------------------------------------------------------*/

/* XML doc class error codes */
enum en_xmldc_error_code {
    XMLDC_ERR_CODE_UNDEFINED = 0,
    XMLDC_ERR_CODE_DOC_CLASS_NOT_FOUND = XMLDC_ERR_CODE_RANGE,
    XMLDC_ERR_CODE_XPATH_SYNTAX_ERROR,
    XMLDC_ERR_CODE_ACCESS_TO_XPATH_ENGINE_FAILED,
    XMLDC_ERR_CODE_CAN_NOT_GET_DOC_CLASS_LIST,
    XMLDC_ERR_CODE_CAN_NOT_CREATE_DOC_CLASS_ITEM,
    XMLDC_ERR_CODE_COULD_NOT_SET_XPATH,
    XMLDC_ERR_CODE_COULD_NOT_GET_INDEX_FOR_DOC_CLASS
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
