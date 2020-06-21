/*!**********************************************************************

  module: XMLDC_DocClass.c

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: XML Document Class

  description:  Document Class Management

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
#include "SAPDBCommon/SAPDB_string.h"

/*---------------------------------------------------------------------------*/

#include "SAPDB/XML/DocClass/XMLDC_DocClass.h"
#include "SAPDB/XML/DocClass/XMLDC_ErrorCodes.h"

#include "SAPDB/XML/Lock/XMLLock_Lock.h"
#include "SAPDB/XML/Settings/XMLSet_Settings.h"
#include "SAPDB/XML/Memory/XMLMem_Memory.h"

/*---------------------------------------------------------------------------*/

#include "SAPDB/XML/XPathEngine/XMLXPath_Interface.h"
#include "SAPDB/XML/XPathEngine/XMLXPath_Indexing.h"

/*---------------------------------------------------------------------------*/

/****************************************************************************
 ***
 *** Defines
 ***
 ****************************************************************************/

#define XMLDC_SAPDB_DEF_NAME_SPACE  "SAPDB"

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

SAPDB_Bool DocClass_GetIndexList( XMLIMAPI_Handle     hXmlModellingApi,
                                  XMLDC_DocClass      hDocClass,
                                  XMLError_Error      hError );

SAPDB_Bool DocClass_CreateDocClass( XMLDC_DocClass              *hNewDocClass,
                                    XMLDC_DocClassId             docClassId,
                                    XMLXPath_Idx_IndexHandler    setIndexFunction,
                                    XMLError_Error               hError );

SAPDB_Bool DocClass_InitDocClass( XMLDC_DocClass            hDocClass,
                                  XMLDC_DocClassId          docClassId,
                                  XMLXPath_Idx_IndexHandler setIndexFunction,
                                  XMLError_Error            hError );

SAPDB_Bool DocClass_CreateIndex( XMLDC_Index          *hNewIndex,
                                 XMLDC_IndexName       indexName,
                                 XMLDC_BasePath        basePath,
                                 XMLDC_ValuePath       valuePath,
                                 XMLError_Error        hError );

SAPDB_Bool DocClass_InitIndex( XMLDC_Index           hIndex,
                               XMLDC_IndexName       indexName,
                               XMLDC_BasePath        basePath,
                               XMLDC_ValuePath       valuePath,
                               XMLError_Error        hError );

SAPDB_Bool DocClass_DestroyIndex( XMLDC_Index hIndex );

SAPDB_Bool DocClass_DestroyDocClass( XMLDC_DocClass hDocClass );

/****************************************************************************
 ***
 *** Variables
 ***
 ****************************************************************************/

static XMLLock_Lock  hLockCreateDocList = NULL;

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Public Functions

 ***********************************************************************/

SAPDB_Bool DocClass_CreateDocClassList( XMLDC_DocClass              *hDocClassList,
                                        XMLIMAPI_Handle              hIMApi,
                                        XMLXPath_Idx_IndexHandler    setIndexFunction,
                                        XMLError_Error               hError )
{

    XMLIMAPI_DocClass  docClass;
    XMLDC_DocClassId   docClassId;
    XMLDC_DocClass     hLastDocClass = NULL;
    SAPDB_Bool         noMoreDocClasses;

    /* Lock creation of doc class list because we have only one XML modelling api handle */
    Lock_Begin(hLockCreateDocList);

    /* Get first doc classes from our XML modelling storage */
    if (!XMLIMAPI_DocClassGetFirst(hIMApi, &docClass)) {
        /* Error handling */
        Error_Set(XMLERROR_POS, hError, XMLDC_ERR_CODE_CAN_NOT_GET_DOC_CLASS_LIST, XMLDC_ERR_TEXT_CAN_NOT_GET_DOC_CLASS_LIST);

        Lock_End(hLockCreateDocList);

        return SAPDB_FALSE;
    }

    /* Create new doc class handle */
    Id_StringAsId(docClass.docClassID, docClassId);
    if (!DocClass_CreateDocClass(hDocClassList, docClassId, setIndexFunction, hError)) {
        /* Error handling */
        Lock_End(hLockCreateDocList);

        return SAPDB_FALSE;
    }

    /* Get index list for this new doc class */
    if (!DocClass_GetIndexList(hIMApi, *hDocClassList, hError)) {
        /* Error handling */
        DocClass_DestroyDocClass(*hDocClassList);
        *hDocClassList = NULL;

        Lock_End(hLockCreateDocList);

        return SAPDB_FALSE;
    }

    /* Get the next doc classes from our XML modelling storage */
    hLastDocClass = *hDocClassList;
    noMoreDocClasses = SAPDB_FALSE;
    do {
        /* Get the next doc classes from our XML modelling storage */
        if (!XMLIMAPI_DocClassGetNext(hIMApi, &docClass)) {
            if (!XMLIMAPI_IsError(hIMApi,
                                   XMLIMAPI_ERR_TYPE_MAPI,
                                   XMLIMAPI_ERR_MAPI_CODE_NO_MORE_DOCCLASSES)) {
                /* Error handling */
                DocClass_DestroyDocClass(*hDocClassList);
                *hDocClassList = NULL;

                Lock_End(hLockCreateDocList);

                return SAPDB_FALSE;
            }

            noMoreDocClasses = SAPDB_TRUE;
        } else {
            /* Create new doc class handle */
            Id_StringAsId(docClass.docClassID, docClassId);
            if (!DocClass_CreateDocClass(&hLastDocClass->hNextDocClass, docClassId, setIndexFunction, hError)) {
                /* Error handling */
                DocClass_DestroyDocClass(*hDocClassList);
                *hDocClassList = NULL;

                Lock_End(hLockCreateDocList);

                return SAPDB_FALSE;
            }

            /* Get index list for this new doc class */
            if (!DocClass_GetIndexList(hIMApi, hLastDocClass->hNextDocClass, hError)) {
                /* Error handling */
                DocClass_DestroyDocClass(*hDocClassList);
                *hDocClassList = NULL;

                Lock_End(hLockCreateDocList);

                return SAPDB_FALSE;
            }

            /* Set last doc class pointer */
            hLastDocClass = hLastDocClass->hNextDocClass;
        }
    } while (noMoreDocClasses == SAPDB_FALSE);

    Lock_End(hLockCreateDocList);

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool DocClass_DestroyDocClass( XMLDC_DocClass hDocClass )
{

    XMLDC_DocClass hNextDocClass = NULL;

    /* Free doc class list */
    while(hDocClass) {
        hNextDocClass = hDocClass->hNextDocClass;

        /* Terminate XPath */
        if (hDocClass->hXPath) {
    		XMLXPath_Idx_Terminate(hDocClass->hXPath);
        }

        /* Free index list */
        if (hDocClass->hIndexList) {
            DocClass_DestroyIndex(hDocClass->hIndexList);
        }

        /* Free doc class itself */
        XMLMem_Free((SAPDB_UInt1*) hDocClass);

        hDocClass = hNextDocClass;
    }

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool DocClass_Init()
{

    return Lock_Create(&hLockCreateDocList);

}

/*===========================================================================*/

SAPDB_Bool DocClass_Exit()
{

    return Lock_Destroy(hLockCreateDocList);

}

/***********************************************************************

	Private Functions

 ***********************************************************************/

SAPDB_Bool DocClass_GetIndexList( XMLIMAPI_Handle     hIMApi,
                                  XMLDC_DocClass      hDocClass,
                                  XMLError_Error      hError )
{

    XMLIMAPI_XmlIndex       xmlIndex;
    XMLDC_Index             hLastIndex = NULL;
    XMLXPath_Idx_RetCode    xmlXPathRC = XMLXPath_RC_OK;
    SAPDB_Bool              noMoreIndexes = SAPDB_FALSE;

    /* Get first index of doc class */
    if (!XMLIMAPI_XmlIndexGetFirstByDocClass(hIMApi, hDocClass->id, &xmlIndex)) {
        /* Error handling */
        Error_Set(XMLERROR_POS, hError, XMLDC_ERR_CODE_COULD_NOT_GET_INDEX_FOR_DOC_CLASS, XMLDC_ERR_TEXT_COULD_NOT_GET_INDEX_FOR_DOC_CLASS);

        return SAPDB_FALSE;
    }
    
/* TEST DATA */
    /*
strcpy(xmlModellingApiIndex.indexNameSpace, "DAV:");
strcpy(xmlModellingApiIndex.indexName, "AUTHOR");
strcpy(xmlModellingApiIndex.xpathBasePath, "//author");
strcpy(xmlModellingApiIndex.xpathValuePath, "* /text()");
strcpy(xmlModellingApiIndex.DbNode, "P48126");
strcpy(xmlModellingApiIndex.DbName, "WEBDAV");
strcpy(xmlModellingApiIndex.DbUser, "WDV");
strcpy(xmlModellingApiIndex.DbPassword, "WDV");
strcpy(xmlModellingApiIndex.serviceDll, "D:\\V74\\USR\\PGM\\WDVXIS");
strcpy(xmlModellingApiIndex.serviceInitCallBack, "WDVXIS_Init");
strcpy(xmlModellingApiIndex.serviceExitCallBack, "WDVXIS_Exit");
strcpy(xmlModellingApiIndex.serviceWriteCallBack, "WDVXIS_PutIndexValue");
    */

    /* Construct XML index object */
    if (!DocClass_CreateIndex(&hDocClass->hIndexList,
                               xmlIndex.indexName,
                               xmlIndex.xpathBasePath,
                               xmlIndex.xpathValuePath,
                               hError)) {
        /* Error handling */
        Error_Log(hError);

        return SAPDB_FALSE;
    }

    /* Define XPath indexes */
    hLastIndex = hDocClass->hIndexList;
    do {
        /* Set current index */
		xmlXPathRC = XMLXPath_Idx_DefineIndex(hDocClass->hXPath,
                                              (void*)hLastIndex, hDocClass->setIndexFunction,
                                              (SAPDB_UTF8*)hLastIndex->basePath,
                                              (SAPDB_UTF8*)hLastIndex->valuePath, XMLXPath_Idx_set);
        if (xmlXPathRC) {
            /* Error handling */
            if (xmlXPathRC == XMLXPath_RC_SYNTAX) {
                Error_Set(XMLERROR_POS, hError, XMLDC_ERR_CODE_XPATH_SYNTAX_ERROR,
                                                XMLDC_ERR_TEXT_XPATH_SYNTAX_ERROR);
            } else {
                Error_Set(XMLERROR_POS, hError, XMLDC_ERR_CODE_ACCESS_TO_XPATH_ENGINE_FAILED,
                                                XMLDC_ERR_TEXT_ACCESS_TO_XPATH_ENGINE_FAILED);
            }
            Error_Log(hError);

            DocClass_DestroyIndex(hDocClass->hIndexList);
            hDocClass->hIndexList = NULL;

            return SAPDB_FALSE;
        }

        /* Get next index of doc class */
        if (!XMLIMAPI_XmlIndexGetNextByDocClass(hIMApi, &xmlIndex)) {
            /* What kind of error occured? */
            if (!XMLIMAPI_IsError(hIMApi,
                                   XMLIMAPI_ERR_TYPE_MAPI,
                                   XMLIMAPI_ERR_MAPI_CODE_NO_MORE_INDICES)) {
                /* Error handling */
                Error_Set(XMLERROR_POS, hError, XMLDC_ERR_CODE_DOC_CLASS_NOT_FOUND,
                                                XMLDC_ERR_TEXT_DOC_CLASS_NOT_FOUND);
                Error_Log(hError);

                DocClass_DestroyIndex(hDocClass->hIndexList);
                hDocClass->hIndexList = NULL;

                return SAPDB_FALSE;
            }

            noMoreIndexes = SAPDB_TRUE;
        } else {
            /* Construct XML index object */
            if (!DocClass_CreateIndex(&hLastIndex->hNextIndex,
                                       xmlIndex.indexName,
                                       xmlIndex.xpathBasePath,
                                       xmlIndex.xpathValuePath,
                                       hError)) {
                /* Error handling */
                Error_Log(hError);
                DocClass_DestroyIndex(hDocClass->hIndexList);
                hDocClass->hIndexList = NULL;

                return SAPDB_FALSE;
            } else {
                hLastIndex = hLastIndex->hNextIndex;
            }
        }
    /* Check error */
    } while (noMoreIndexes == SAPDB_FALSE);

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool DocClass_CreateDocClass( XMLDC_DocClass              *hNewDocClass,
                                    XMLDC_DocClassId             docClassId,
                                    XMLXPath_Idx_IndexHandler    setIndexFunction,
                                    XMLError_Error               hError )
{

    SAPDB_Bool ok;

    /* Construct new doc class */
	XMLMem_Allocat(sizeof(struct st_xmldc_doc_class), (SAPDB_UInt1**) hNewDocClass, &ok );
	if (!ok) {
        *hNewDocClass = NULL;

        Error_Set(XMLERROR_POS, hError, XMLERROR_ERR_CODE_NO_MORE_MEMORY, XMLERROR_ERR_TEXT_NO_MORE_MEMORY);
        Error_Set(XMLERROR_POS, hError, XMLDC_ERR_CODE_CAN_NOT_CREATE_DOC_CLASS_ITEM, XMLDC_ERR_TEXT_CAN_NOT_CREATE_DOC_CLASS_ITEM);

        return SAPDB_FALSE;
	}

	/* Init handle */
    if (!DocClass_InitDocClass(*hNewDocClass, docClassId, setIndexFunction, hError)) {
        XMLMem_Free(*hNewDocClass);
        *hNewDocClass = NULL;

        return SAPDB_FALSE;
    }

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool DocClass_InitDocClass( XMLDC_DocClass            hDocClass,
                                  XMLDC_DocClassId          docClassId,
                                  XMLXPath_Idx_IndexHandler setIndexFunction,
                                  XMLError_Error            hError )
{


    /* Set doc class id */
    SAPDB_memcpy(hDocClass->id, docClassId, XMLDC_MAX_DOC_CLASS_ID_LEN);

    /* Initialize XPath handle */
    hDocClass->hXPath = NULL;
	if (XMLXPath_Idx_Initialize(&hDocClass->hXPath)) {
        Error_Set(XMLERROR_POS, hError, XMLDC_ERR_CODE_COULD_NOT_SET_XPATH,
                                        XMLDC_ERR_TEXT_COULD_NOT_SET_XPATH);

        return SAPDB_FALSE;
	}

    hDocClass->hIndexList = NULL;
    hDocClass->hNextDocClass = NULL;
    hDocClass->setIndexFunction = setIndexFunction;

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool DocClass_CreateIndex( XMLDC_Index          *hNewIndex,
                                 XMLDC_IndexName       indexName,
                                 XMLDC_BasePath        basePath,
                                 XMLDC_ValuePath       valuePath,
                                 XMLError_Error        hError )
{

    SAPDB_Bool ok;

    /* Construct new index */
	XMLMem_Allocat(sizeof(struct st_xmldc_index), (SAPDB_UInt1**) hNewIndex, &ok );
	if (!ok) {
        *hNewIndex = NULL;

        Error_Set(XMLERROR_POS, hError, XMLERROR_ERR_CODE_NO_MORE_MEMORY, XMLERROR_ERR_TEXT_NO_MORE_MEMORY);

        return SAPDB_FALSE;
	}

	/* Init handle */
    if (!DocClass_InitIndex(*hNewIndex, indexName, basePath, valuePath, hError)) {

        XMLMem_Free(*hNewIndex);
        *hNewIndex = NULL;

        return SAPDB_FALSE;
    }

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool DocClass_InitIndex( XMLDC_Index           hIndex,
                               XMLDC_IndexName       indexName,
                               XMLDC_BasePath        basePath,
                               XMLDC_ValuePath       valuePath,
                               XMLError_Error        hError )
{

    /* Set index properties */
    Com_StrMaxCopy(hIndex->indexNameSpace, XMLDC_SAPDB_DEF_NAME_SPACE, XMLDC_MAX_INDEX_NAME_LEN);
    Com_StrMaxCopy(hIndex->indexName, indexName, XMLDC_MAX_INDEX_NAME_LEN);
    Com_StrMaxCopy(hIndex->basePath, basePath, XMLDC_MAX_BASE_PATH_LEN);
    Com_StrMaxCopy(hIndex->valuePath, valuePath, XMLDC_MAX_VALUE_PATH_LEN);

    hIndex->hNextIndex = NULL;

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool DocClass_DestroyIndex( XMLDC_Index hIndex )
{

    XMLDC_Index    hNextIndex = NULL;

    /* Free index list */
    while(hIndex) {
        hNextIndex = hIndex->hNextIndex;

        /* Free index handle itself */
        XMLMem_Free((SAPDB_UInt1*) hIndex);

        hIndex = hNextIndex;
    }

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool DocClass_FindDocClass( XMLDC_DocClass     hDocClassList,
                                  XMLDC_DocClassId   docClassId,
                                  XMLDC_DocClass    *hDocClass,
                                  XMLError_Error     hError )
{

    XMLDC_DocClass hCurrentDocClass = NULL;

    /* Inits */
    *hDocClass = NULL;

    /* Checks */
    if (!hDocClassList || !hDocClass) {
        Error_Set(XMLERROR_POS, hError, XMLERROR_ERR_CODE_INTERNAL_ERROR,
                                        XMLERROR_ERR_TEXT_INTERNAL_ERROR);

        return SAPDB_FALSE;
    }

    /* Get first doc class from doc class list */
    hCurrentDocClass = hDocClassList;

    /* Search doc class list */
    while (hCurrentDocClass) {
        /* Doc class found? */
        if (Id_IsIdentical(hCurrentDocClass->id, docClassId)) {
            *hDocClass = hCurrentDocClass;

            return SAPDB_TRUE;
        }

        /* Get next doc class */
        hCurrentDocClass = hCurrentDocClass->hNextDocClass;
    }

    /* Doc class not found */
    Error_Set(XMLERROR_POS, hError, XMLDC_ERR_CODE_DOC_CLASS_NOT_FOUND,
                                    XMLDC_ERR_TEXT_DOC_CLASS_NOT_FOUND);

    return SAPDB_FALSE;

}

/*===========================================================================*/

/***********************************************************************

	End

 ***********************************************************************/
