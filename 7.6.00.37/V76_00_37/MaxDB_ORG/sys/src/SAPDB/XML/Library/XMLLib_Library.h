/*!**********************************************************************

  module: XMLLib_Library.h

  -----------------------------------------------------------------------

  responsible:	Markus Oezgen

  special area: XML Library Handling

  description:  Library

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

#ifndef XMLLIB_LIBRARY_H
#define XMLLIB_LIBRARY_H

/***********************************************************************

	Includes

 ***********************************************************************/

#include "heo02x.h" /* sqlLoadLibrary */

/*---------------------------------------------------------------------------*/

#include "SAPDB/SAPDBCommon/SAPDB_Types.h"

/*---------------------------------------------------------------------------*/

#include "SAPDB/XML/Library/XMLLib_ErrorCodes.h"

#include "SAPDB/XML/Error/XMLError_Error.h"

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Defines

 ***********************************************************************/

#define XMLLIB_MAX_LIBRARY_NAME_LEN     255
#define XMLLIB_MAX_FUNCTION_NAME_LEN    255

/***********************************************************************

	Structs & Types

 ***********************************************************************/

typedef struct st_xmllib_library    *XMLLib_Library;
typedef char                         XMLLib_LibraryNameString[XMLLIB_MAX_LIBRARY_NAME_LEN+1];
typedef char                        *XMLLib_LibraryName;

typedef sqlPROC		                 XMLLib_Function;
typedef char                         XMLLib_FunctionNameString[XMLLIB_MAX_FUNCTION_NAME_LEN+1];
typedef char                        *XMLLib_FunctionName;

struct st_xmllib_library {
    HANDLE                      library;
    XMLLib_LibraryNameString    libraryName;
    XMLLib_Function             funcConstructUserData;
    XMLLib_Function             funcDestructUserData;
    void*                       userData;
    XMLLib_Library              hNextLibrary;
};

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Prototypes

 ***********************************************************************/

SAPDB_Bool Library_Load( XMLLib_Library       *hLibrary,
                         XMLLib_LibraryName    libraryName,
                         XMLLib_FunctionName   funcConstructUserData,
                         XMLLib_FunctionName   funcDestructUserData,
                         XMLError_Error        hError );

SAPDB_Bool Library_Unload( XMLLib_Library hLibrary );

SAPDB_Bool Library_GetFunction( XMLLib_Library         hLibrary,
                                XMLLib_FunctionName    functionName,
                                XMLLib_Function       *function,
                                XMLError_Error         hError );

/***********************************************************************

	End

 ***********************************************************************/

#endif
