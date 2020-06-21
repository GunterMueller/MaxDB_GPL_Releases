/*!**********************************************************************

  module: XMLSP_ErrorCodes.h

  -----------------------------------------------------------------------

  responsible:	Markus Oezgen

  special area: XML Session Pool

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

#ifndef XMLSP_ERROR_CODES_H
#define XMLSP_ERROR_CODES_H

/***********************************************************************

	Includes

 ***********************************************************************/

#include "SAPDB/XML/Error/XMLError_ErrorCodeRanges.h"

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Defines

 ***********************************************************************/

#define XMLSP_ERR_TEXT_SESSION_NOT_FOUND                    "Session not found"
#define XMLSP_ERR_TEXT_COULD_NOT_ALLOCAT_ODBC_ENV           "Could not allocat ODBC environment"
#define XMLSP_ERR_TEXT_COULD_NOT_ALLOCAT_ODBC_CONNECT       "Could not allocat ODBC connect"
#define XMLSP_ERR_TEXT_COULD_NOT_CONNECT                    "Could not connect to database"
#define XMLSP_ERR_TEXT_COULD_NOT_SET_AUTOCOMMIT_OFF         "Could not set autocommit off"

/*---------------------------------------------------------------------------*/

/* XML Session Pool error codes */
enum en_xmlsp_error_code {
    XMLSP_ERR_CODE_UNDEFINED = XMLSP_ERR_CODE_RANGE,
    XMLSP_ERR_CODE_SESSION_NOT_FOUND,
    XMLSP_ERR_CODE_COULD_NOT_ALLOCAT_ODBC_ENV,
    XMLSP_ERR_CODE_COULD_NOT_ALLOCAT_ODBC_CONNECT,
    XMLSP_ERR_CODE_COULD_NOT_CONNECT,
    XMLSP_ERR_CODE_COULD_NOT_SET_AUTOCOMMIT_OFF
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
