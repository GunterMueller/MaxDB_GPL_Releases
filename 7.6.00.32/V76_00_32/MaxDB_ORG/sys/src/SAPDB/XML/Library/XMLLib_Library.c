/*!**********************************************************************

  module: XMLLib_Library.c

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

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

/****************************************************************************
 ***
 *** Include
 ***
 ****************************************************************************/

#include "SAPDB/SAPDBCommon/SAPDB_Types.h"

/*---------------------------------------------------------------------------*/

#include "SAPDB/XML/Library/XMLLib_Library.h"
#include "SAPDB/XML/Memory/XMLMem_Memory.h"

/*---------------------------------------------------------------------------*/

/****************************************************************************
 ***
 *** Defines
 ***
 ****************************************************************************/

/****************************************************************************
 ***
 *** Types & Structs
 ***
 ****************************************************************************/

/****************************************************************************
 ***
 *** Prototypes
 ***
 ****************************************************************************/

SAPDB_Bool Library_Create( XMLLib_Library       *hLibrary,
                           XMLLib_LibraryName    libraryName,
                           XMLLib_FunctionName   funcNameConstructUserData,
                           XMLLib_FunctionName   funcNameDestructUserData,
                           XMLError_Error        hError );

SAPDB_Bool Library_Init( XMLLib_Library       hLibrary,
                         XMLLib_LibraryName   libraryName,
                         XMLLib_FunctionName  funcNameConstructUserData,
                         XMLLib_FunctionName  funcNameDestructUserData,
                         XMLError_Error       hError );

SAPDB_Bool Library_Destroy( XMLLib_Library hLibrary );

SAPDB_Bool Library_FindLibrary( XMLLib_Library      hLibraryList,
                                XMLLib_LibraryName  libraryName,
                                XMLLib_Library     *hLibrary,
                                XMLError_Error      hError );

/****************************************************************************
 ***
 *** Variables
 ***
 ****************************************************************************/

static XMLLib_Library   hLibraryList = NULL;

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Public Functions

 ***********************************************************************/

SAPDB_Bool Library_Load( XMLLib_Library       *hLibrary,
                         XMLLib_LibraryName    libraryName,
                         XMLLib_FunctionName   funcConstructUserData,
                         XMLLib_FunctionName   funcDestructUserData,
                         XMLError_Error        hError )
{

    /* Try to find library in library list                          */
    /* We don't have to "mutex" this list, because it is accessed   */
    /* only by the listener thread                                  */
    if (!Library_FindLibrary(hLibraryList, libraryName, hLibrary, hError)) {
        return SAPDB_FALSE;
    }
    if (*hLibrary) {
        return SAPDB_TRUE;
    }

    /* Add new library */
    if (!Library_Create(hLibrary, libraryName, funcConstructUserData, funcDestructUserData, hError)) {
        return SAPDB_FALSE;
    }

    if (hLibraryList) {
        (*hLibrary)->hNextLibrary = hLibraryList;
    }

    hLibraryList = *hLibrary;

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool Library_Unload( XMLLib_Library hLibrary )
{

    XMLLib_Library  hLastLibrary = hLibraryList;
    XMLLib_Library  hCurrentLibrary = hLibraryList;

    if (!hLibrary) {
        return SAPDB_FALSE;
    }

    while (hCurrentLibrary) {
        if (memcmp(hCurrentLibrary->library, hLibrary->library, sizeof(XMLLib_Library)) == 0) {
            /* Manage list */
            if (hLastLibrary == hCurrentLibrary) {
                /* The first item in list is to delete */
                hLibraryList = hLibrary->hNextLibrary;
            } else {
                /* Exclude library to delet from list */
                hLastLibrary->hNextLibrary = hLibrary->hNextLibrary;
            }

            /* Unload and destroy library */
            return Library_Destroy(hLibrary);
        }

        /* Check next item in list */
        hLastLibrary = hCurrentLibrary;
        hCurrentLibrary = hCurrentLibrary->hNextLibrary;
    }

    return SAPDB_FALSE;

}

/*===========================================================================*/

SAPDB_Bool Library_GetFunction( XMLLib_Library         hLibrary,
                                XMLLib_FunctionName    functionName,
                                XMLLib_Function       *function,
                                XMLError_Error         hError )
{

    XMLError_ErrorTextString  errorText = "";

    if (!hLibrary || !functionName || !function) {
        Error_Set(XMLERROR_POS, hError, XMLERROR_ERR_CODE_INTERNAL_ERROR, XMLERROR_ERR_TEXT_INTERNAL_ERROR);

        return SAPDB_FALSE;
    }
    
    /* Get function pointer */
    *function = sqlGetProcAddress( hLibrary->library, functionName, errorText, XMLERROR_MAX_ERROR_TEXT_LEN);
    if (*function == NULL) {
        /* Error handling */
        Error_Set(XMLERROR_POS, hError, XMLLIB_ERR_CODE_COULD_NOT_FIND_FUNCTION_IN_LIBRARY,
                                        XMLLIB_ERR_TEXT_COULD_NOT_FIND_FUNCTION_IN_LIBRARY);

        return SAPDB_FALSE;
    };

    return SAPDB_TRUE;

}

/*===========================================================================*/

/***********************************************************************

	Private Functions

 ***********************************************************************/

SAPDB_Bool Library_Create( XMLLib_Library       *hLibrary,
                           XMLLib_LibraryName    libraryName,
                           XMLLib_FunctionName   funcNameConstructUserData,
                           XMLLib_FunctionName   funcNameDestructUserData,
                           XMLError_Error        hError )
{

    SAPDB_Bool ok;

    /* Construct */
	XMLMem_Allocat(sizeof(struct st_xmllib_library), (SAPDB_UInt1**) hLibrary, &ok );
	if (!ok) {
        *hLibrary = NULL;

        return SAPDB_FALSE;
	}

	/* Init */
    if (!Library_Init(*hLibrary, libraryName, funcNameConstructUserData, funcNameDestructUserData, hError )) {
        XMLMem_Free(*hLibrary);
        *hLibrary = NULL;

        return SAPDB_FALSE;
    }

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool Library_Init( XMLLib_Library       hLibrary,
                         XMLLib_LibraryName   libraryName,
                         XMLLib_FunctionName  funcNameConstructUserData,
                         XMLLib_FunctionName  funcNameDestructUserData,
                         XMLError_Error       hError )
{

    XMLError_ErrorTextString  errorText = "";

    /* Inits */
    memset(hLibrary, 0, sizeof(XMLLib_Library));

    /* Load library */
    hLibrary->library = sqlLoadLibrary(libraryName, errorText, XMLERROR_MAX_ERROR_TEXT_LEN);
    if (hLibrary->library == NULL) {
        /* Error handling */
        Error_SetFormatted(XMLERROR_POS,
                           hError,
                           XMLLIB_ERR_CODE_COULD_NOT_LOAD_LIBRARY,
                           "%s: %s",
                           XMLLIB_ERR_TEXT_COULD_NOT_LOAD_LIBRARY, libraryName);

        return SAPDB_FALSE;
    }

    /* Set properties */
    Com_StrMaxCopy(hLibrary->libraryName, libraryName, XMLLIB_MAX_LIBRARY_NAME_LEN);

    hLibrary->hNextLibrary = NULL;
    hLibrary->userData = NULL;

    /* Get user function to construct usr data */
    hLibrary->funcConstructUserData = NULL;
    if (!Library_GetFunction(hLibrary, funcNameConstructUserData, &hLibrary->funcConstructUserData, hError)) {
        sqlFreeLibrary(hLibrary->library, errorText, XMLERROR_MAX_ERROR_TEXT_LEN);

        Error_SetFormatted(XMLERROR_POS, hError,
                           XMLLIB_ERR_CODE_COULD_NOT_FIND_FUNCTION_IN_LIBRARY,
                           "%s: %s",
                           XMLLIB_ERR_TEXT_COULD_NOT_FIND_FUNCTION_IN_LIBRARY, funcNameConstructUserData);

        return SAPDB_FALSE;
    }

    /* Get user function to destruct user data */
    hLibrary->funcDestructUserData = NULL;
    if (!Library_GetFunction(hLibrary, funcNameDestructUserData, &hLibrary->funcDestructUserData, hError)) {
        /* Error handling */
        sqlFreeLibrary(hLibrary->library, errorText, XMLERROR_MAX_ERROR_TEXT_LEN);

        Error_SetFormatted(XMLERROR_POS, hError,
                           XMLLIB_ERR_CODE_COULD_NOT_FIND_FUNCTION_IN_LIBRARY,
                           "%s: %s",
                           XMLLIB_ERR_TEXT_COULD_NOT_FIND_FUNCTION_IN_LIBRARY, funcNameDestructUserData);

        return SAPDB_FALSE;
    }

    /* Call user function to construct user data */
    if (hLibrary->funcConstructUserData) {
        hLibrary->funcConstructUserData(&hLibrary->userData, hError);
        if (hError->code != XMLLIB_ERR_CODE_UNDEFINED) {
            /* Error handling */
            sqlFreeLibrary(hLibrary->library, errorText, XMLERROR_MAX_ERROR_TEXT_LEN);

            Error_SetFormatted(XMLERROR_POS, hError,
                               XMLLIB_ERR_CODE_COULD_NOT_CALL_CONSTRUCTOR_FUNCTION,
                               "%s: %s->%s",
                               XMLLIB_ERR_TEXT_COULD_NOT_CALL_CONSTRUCTOR_FUNCTION, libraryName, funcNameConstructUserData);

            return SAPDB_FALSE;
        }
    }

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool Library_Destroy( XMLLib_Library hLibrary )
{

    XMLError_ErrorTextString  errorText = "";

    /* Free library */
    if (hLibrary) {
        /* Call user function to destruct user data */
        if (hLibrary->funcDestructUserData) {
            hLibrary->funcDestructUserData(hLibrary->userData);
        }

        /* Unload library */
        sqlFreeLibrary(hLibrary->library, errorText, XMLERROR_MAX_ERROR_TEXT_LEN);

        /* Free handle itself */
        XMLMem_Free((SAPDB_UInt1*) hLibrary);
    }

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool Library_FindLibrary( XMLLib_Library      hLibraryList,
                                XMLLib_LibraryName  libraryName,
                                XMLLib_Library     *hLibrary,
                                XMLError_Error      hError )
{

    XMLLib_Library  hCurrentLibrary = hLibraryList;

    while (hCurrentLibrary) {
        if (strcmp(hCurrentLibrary->libraryName, libraryName) == 0) {
            *hLibrary = hCurrentLibrary;

            return SAPDB_TRUE;
        }

        /* Check next item in list */
        hCurrentLibrary = hCurrentLibrary->hNextLibrary;
    }

    *hLibrary = NULL;

    return SAPDB_TRUE; /* No error occured */

}

/*===========================================================================*/

/***********************************************************************

	End

 ***********************************************************************/
