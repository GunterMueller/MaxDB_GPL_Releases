/*!**********************************************************************

  module: XMLIdx_Indexing.c

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: XML Index Engine

  description:  XML Indexing

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

/***********************************************************************

	Includes

 ***********************************************************************/

#include "SAPDB/XML/IndexEngine/XMLIdx_Indexing.h"
#include "SAPDB/XML/IndexEngine/XMLIdx_ErrorCodes.h"
#include "SAPDB/XML/IndexEngine/XMLIdx_Workers.h"
#include "SAPDB/XML/IndexEngine/XMLIdx_DocStore.h"

#include "SAPDB/XML/Common/XMLCom_Common.h"
#include "SAPDB/XML/Error/XMLError_Error.h"
#include "SAPDB/XML/DocClass/XMLDC_DocClass.h"
#include "SAPDB/XML/SessionPool/XMLSP_SessionPool.h"

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Defines

 ***********************************************************************/

/***********************************************************************

	Structs & Types

 ***********************************************************************/

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Prototypes

 ***********************************************************************/

XMLXPath_Idx_RetCode Indexing_GetContent( void           *userData,
                                          unsigned char **buffer,
                                          SAPDB_Int      *bufferLength,
                                          SAPDB_Int      *endOfDoc );

SAPDB_Bool Indexing_ReadDoc( XMLIdx_Work      hWork,
                             unsigned char  **buffer,
                             SAPDB_Int       *bufferLength,
                             SAPDB_Int       *endOfDoc,
                             XMLError_Error   hError );
/***********************************************************************

	Variables

 ***********************************************************************/

/***********************************************************************

	Public Functions

 ***********************************************************************/

SAPDB_Bool Indexing_Process( XMLIdx_Work    hWork,
                             XMLError_Error hError )
{

    XMLDC_DocClass  hDocClass = NULL;

    /* Do we already have the doc class list? */
    if (!hWork->hDocClassList) {
        /* todo: Error handling */
    }

    /* Try to find doc class id in doc class list */
    if (!DocClass_FindDocClass(hWork->hDocClassList,
                               hWork->docClassId, &hDocClass, hError)) {
        return SAPDB_FALSE;
    }

    /* Parse content */
    if (XMLXPath_Idx_Parse(hDocClass->hXPath, hWork, Indexing_GetContent)) {
        Error_Set(XMLERROR_POS, hError, XMLIDX_ERR_CODE_XPATH_PARSE_ERROR,
                                        XMLIDX_ERR_TEXT_XPATH_PARSE_ERROR);

        return SAPDB_FALSE;
    }

    /* Set parsing state: FINISHED */
    DocStore_SetState(hWork->hSessionDocStore, hWork->docId,
                      XMLIDX_STATE_DOC_PARSING_HAS_BEEN_FINISHED, hError);
    if (hError->code != XMLIDX_ERR_CODE_UNDEFINED) {
        Error_Set(XMLERROR_POS, hError, XMLIDX_ERR_CODE_COULD_NOT_SET_STATE_FINISHED,
                                        XMLIDX_ERR_TEXT_COULD_NOT_SET_STATE_FINISHED);

        return SAPDB_FALSE;
    }
    
    return SAPDB_TRUE;

}

/*===========================================================================*/

XMLXPath_Idx_RetCode Indexing_SetIndexValue( void             *parserUserData,
                                             void	          *indexUserData,
                                             const SAPDB_UTF8 *indexValue )
{

    struct st_xmlerror_error	error;
    XMLError_Error              hError = &error;
    XMLIdx_Work                 hWork = (XMLIdx_Work)parserUserData;
    XMLDC_Index                 hIndex = (XMLDC_Index)indexUserData;
    XMLLib_Function             Indexing_UserFuncSetIndexValue = NULL;

    /* Inits */
    Error_Init(hError, "USER");

    /* Get user function */
    Indexing_UserFuncSetIndexValue = hWork->funcSetIndexValue;

    /* Call user function */
    Indexing_UserFuncSetIndexValue(hWork->hSessionIndexStore, hWork->docId, hIndex->indexNameSpace, hIndex->indexName, (char*)indexValue, hError);
    if (hError->code != XMLIDX_ERR_CODE_UNDEFINED) {
        Error_Log(hError);

        return XMLXPath_RC_FAILED;
    }

    return XMLXPath_RC_OK;

}

/*===========================================================================*/

/***********************************************************************

	Private Functions

 ***********************************************************************/

XMLXPath_Idx_RetCode Indexing_GetContent( void           *userData,
                                          unsigned char **buffer,
                                          SAPDB_Int      *bufferLength,
                                          SAPDB_Int      *endOfDoc )
{

    struct st_xmlerror_error	error;
    XMLError_Error              hError = &error;
    XMLIdx_Work                 hWork = (XMLIdx_Work)userData;

    /* Inits */
    Error_Init(hError, "USER");

    /* Call user function */
    DocStore_ReadDoc(hWork, buffer, bufferLength, endOfDoc, hError);
    if (hError->code != XMLIDX_ERR_CODE_UNDEFINED) {
        Error_Log(hError);

        return XMLXPath_RC_FAILED;
    }

    return XMLXPath_RC_OK;

}

/***********************************************************************

	End

 ***********************************************************************/
