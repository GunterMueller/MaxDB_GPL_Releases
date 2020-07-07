/*!**********************************************************************

  module: XMLSet_ErrorCodes.h

  -----------------------------------------------------------------------

  responsible:	Markus Oezgen

  special area: XML Settings Handling

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

#ifndef XMLSET_ERROR_CODES_H
#define XMLSET_ERROR_CODES_H

/***********************************************************************

	Includes

 ***********************************************************************/

#include "SAPDB/XML/Error/XMLError_ErrorCodeRanges.h"

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Defines

 ***********************************************************************/

#define XMLSET_ERR_TEXT_CANNOT_OPEN_CONF_FILE       "Cannot open conf file"
#define XMLSET_ERR_TEXT_COULD_NOT_OPEN_REGISTRY     "Could not open registry"
#define XMLSET_ERR_TEXT_COULD_NOT_READ_REGISTRY     "Could not read registry"
#define XMLSET_ERR_TEXT_COULD_NOT_UPDATE_REGISTRY   "Could not update registry"

/*---------------------------------------------------------------------------*/

/* XML settings error codes */
enum en_xmlset_error_code {
    XMLSET_ERR_CODE_UNDEFINED = XMLSET_ERR_CODE_RANGE,
    XMLSET_ERR_CODE_CANNOT_OPEN_CONF_FILE,
    XMLSET_ERR_CODE_COULD_NOT_OPEN_REGISTRY,
    XMLSET_ERR_CODE_COULD_NOT_READ_REGISTRY,
    XMLSET_ERR_CODE_COULD_NOT_UPDATE_REGISTRY
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
