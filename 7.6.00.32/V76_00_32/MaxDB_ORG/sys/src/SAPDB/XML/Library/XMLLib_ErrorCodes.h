/*!**********************************************************************

  module: XMLLib_ErrorCodes.h

  -----------------------------------------------------------------------

  responsible:	Markus Oezgen

  special area: XML Library Handling

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

#ifndef XMLLIB_ERROR_CODES_H
#define XMLLIB_ERROR_CODES_H

/***********************************************************************

	Includes

 ***********************************************************************/

#include "SAPDB/XML/Error/XMLError_ErrorCodeRanges.h"

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Defines

 ***********************************************************************/

#define XMLLIB_ERR_TEXT_COULD_NOT_FIND_FUNCTION_IN_LIBRARY      "Could not find function in library"
#define XMLLIB_ERR_TEXT_COULD_NOT_CALL_CONSTRUCTOR_FUNCTION     "Could not call constructor function in library"
#define XMLLIB_ERR_TEXT_COULD_NOT_LOAD_LIBRARY                  "Could not load library"
#define XMLLIB_ERR_TEXT_COULD_NOT_UNLOAD_LIBRARY                "Could not unload library"

/*---------------------------------------------------------------------------*/

/* XML library error codes */
enum en_xmllib_error_code {
    XMLLIB_ERR_CODE_UNDEFINED = 0,
    XMLLIB_ERR_CODE_COULD_NOT_FIND_FUNCTION_IN_LIBRARY = XMLLIB_ERR_CODE_RANGE,
    XMLLIB_ERR_CODE_COULD_NOT_CALL_CONSTRUCTOR_FUNCTION,
    XMLLIB_ERR_CODE_COULD_NOT_LOAD_LIBRARY,
    XMLLIB_ERR_CODE_COULD_NOT_UNLOAD_LIBRARY
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
