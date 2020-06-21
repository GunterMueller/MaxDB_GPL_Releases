/*!**********************************************************************

  module: WDVCAPI_XMLIndexing.c

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: WebDAV

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

#include <WDVCAPI_Definition.h>

#include "SAPDB/XML/XPathEngine/XMLXPath_Interface.h"
#include "SAPDB/XML/XPathEngine/XMLXPath_Indexing.h"

#include "SAPDB/WebDAV/CAPI/WDVCAPI_WDV.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Memory.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Property.h"
#include "SAPDBCommon/SAPDB_string.h"

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

WDV_Bool XMLIndexing_CreateXmlIndex( WDVCAPI_WDV       wdv,
                                     WDVCAPI_XmlIndex *hXmlIndex );

WDV_Bool XMLIndexing_DestroyXmlIndex( WDVCAPI_XmlIndex hXmlIndex );

WDV_Bool XMLIndexing_InitXmlDocClass( WDVCAPI_WDV         wdv,
                                      WDVCAPI_XmlDocClass hXmlDocClass );

WDV_Bool XMLIndexing_InitXmlIndex( WDVCAPI_WDV      wdv,
                                   WDVCAPI_XmlIndex hXmlIndex );

WDV_Bool XMLIndexing_FindDocClass( WDVCAPI_WDV              wdv,
                                   WDVCAPI_XmlDocClassId    xmlDocClassId,
                                   WDVCAPI_XmlDocClass     *phXmlDocClass );

/*===========================================================================*/

/***********************************************************************

	API Functions

 ***********************************************************************/

/*===========================================================================*/

/***********************************************************************

	API Internal Functions

 ***********************************************************************/

WDV_Bool XMLIndexing_CreateXmlDocClass( WDVCAPI_WDV          wdv,
                                        WDVCAPI_XmlDocClass *hDocClass )
{

	WDV_Bool             ok = WDV_False;
    WDVCAPI_XmlDocClass  hNewDocClass = NULL;

	if ( !wdv ) {
		return WDV_False;
	}
    
	WDVCAPI_Allocat( sizeof(struct st_wdvcapi_xml_doc_class), (WDV_UInt1**) &hNewDocClass, &ok );
	if (ok == WDV_False) {
		return WDV_False;
	}

	/* Init handle */
    if (!XMLIndexing_InitXmlDocClass(wdv, hNewDocClass)) {
        WDVCAPI_Free(hNewDocClass);
        *hDocClass = NULL;

        return WDV_False;
    }

    /* Set handle */
    *hDocClass = hNewDocClass;

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool XMLIndexing_DestroyXmlDocClass( WDVCAPI_XmlDocClass hDocClass )
{

    WDVCAPI_XmlDocClass hDocClassToDelete = hDocClass;
    WDVCAPI_XmlDocClass hNextDocClass = NULL;

    /* Free doc class list */
    while(hDocClassToDelete) {
        hNextDocClass = hDocClassToDelete->hNextDocClass;

        /* Free XML index list */
        XMLIndexing_DestroyXmlIndex(hDocClassToDelete->hIndexList);

        /* Free XPath engine handle */
        if (XMLXPath_Idx_Terminate(hDocClassToDelete->hXPath)) {
            return WDV_False;
        }

        /* Free doc class list handle itself */
        WDVCAPI_Free((WDV_UInt1*) hDocClassToDelete);

        hDocClassToDelete = hNextDocClass;
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool XMLIndexing_GetXmlDocClass( WDVCAPI_WDV            wdv,
                                     WDVCAPI_XmlDocClassId  xmlDocClassId,
                                     WDVCAPI_XmlDocClass   *xmlDocClass )
{

    WDVCAPI_XmlDocClass     hCurrentXmlDocClass = NULL;
    WDVCAPI_XmlIndex        hCurrentXmlIndex = NULL;
    WDVCAPI_XmlIndex        hLastXmlIndex = NULL;
    XMLIMAPI_ErrorItem      xmlErrorItem;
    XMLXPath_Idx_RetCode    xmlXPathRC = XMLXPath_RC_OK;

    /* Try to find doc class id in doc class list */
    if (!XMLIndexing_FindDocClass(wdv, xmlDocClassId, xmlDocClass)) {
        return WDV_False;
    }

    if (!*xmlDocClass) {
        /* Try to find doc class in our XML modelling storage */
        if (!wdv->hXmlModelling) {
            Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                    WDVCAPI_ERR_CODE_NO_ACCESS_TO_XML_MODELLING_MANAGEMENT,
                                    WDVCAPI_ERR_TEXT_NO_ACCESS_TO_XML_MODELLING_MANAGEMENT);

            return WDV_False;
        }

        /* Construct a new doc class object */
        if (!XMLIndexing_CreateXmlDocClass(wdv, xmlDocClass)) {
            Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                    WDVCAPI_ERR_CODE_DOC_CLASS_NOT_FOUND,
                                    WDVCAPI_ERR_TEXT_DOC_CLASS_NOT_FOUND);

            return WDV_False;
        }

        hCurrentXmlDocClass = *xmlDocClass;
        SAPDB_memcpy(hCurrentXmlDocClass->docClassId, xmlDocClassId, WDVCAPI_MAX_DOC_CLASS_ID_LEN);

        /* Construct XML index object */
        if (!XMLIndexing_CreateXmlIndex(wdv, &hCurrentXmlDocClass->hIndexList)) {
            XMLIndexing_DestroyXmlDocClass(hCurrentXmlDocClass);

            return WDV_False;
        }

        hCurrentXmlIndex = hCurrentXmlDocClass->hIndexList;

        /* Get first index of doc class */
        if (!XMLIMAPI_XmlIndexGetFirstByDocClass(wdv->hXmlModelling, xmlDocClassId, &hCurrentXmlIndex->index)) {
            Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                    WDVCAPI_ERR_CODE_DOC_CLASS_NOT_FOUND,
                                    WDVCAPI_ERR_TEXT_DOC_CLASS_NOT_FOUND);

            XMLIndexing_DestroyXmlIndex(hCurrentXmlIndex);
            XMLIndexing_DestroyXmlDocClass(hCurrentXmlDocClass);

            return WDV_False;
        }

	    if (XMLXPath_Idx_Initialize(&hCurrentXmlDocClass->hXPath)) {
            Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                    WDVCAPI_ERR_CODE_DOC_CLASS_NOT_FOUND,
                                    WDVCAPI_ERR_TEXT_DOC_CLASS_NOT_FOUND);

            XMLIndexing_DestroyXmlIndex(hCurrentXmlIndex);
            XMLIndexing_DestroyXmlDocClass(hCurrentXmlDocClass);

            return WDV_False;
	    }

        /* Define XPath indexes */
        do {
            /* Set current index */
            hCurrentXmlIndex->hWDV = wdv;
		    xmlXPathRC = XMLXPath_Idx_DefineIndex(hCurrentXmlDocClass->hXPath,
                                                  (void*)hCurrentXmlIndex, Property_SetIndex,
                                                  (SAPDB_UTF8*) hCurrentXmlIndex->index.xpathBasePath,
                                                  (SAPDB_UTF8*) hCurrentXmlIndex->index.xpathValuePath,
                                                  XMLXPath_Idx_set);
            if (xmlXPathRC) {
                if (xmlXPathRC == XMLXPath_RC_SYNTAX) {
                    Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                            WDVCAPI_ERR_CODE_XPATH_SYNTAX_ERROR,
                                            WDVCAPI_ERR_TEXT_XPATH_SYNTAX_ERROR);
                } else {
                    Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                            WDVCAPI_ERR_CODE_ACCESS_TO_XPATH_ENGINE_FAILED,
                                            WDVCAPI_ERR_TEXT_ACCESS_TO_XPATH_ENGINE_FAILED);
                }

                XMLIndexing_DestroyXmlIndex(hCurrentXmlDocClass->hIndexList);
                XMLIndexing_DestroyXmlDocClass(hCurrentXmlDocClass);

                return WDV_False;
            }

            /* Construct XML index object */
            if (!XMLIndexing_CreateXmlIndex(wdv, &hCurrentXmlIndex->hNextIndex)) {
                Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                        WDVCAPI_ERR_CODE_DOC_CLASS_NOT_FOUND,
                                        WDVCAPI_ERR_TEXT_DOC_CLASS_NOT_FOUND);

                XMLIndexing_DestroyXmlIndex(hCurrentXmlDocClass->hIndexList);
                XMLIndexing_DestroyXmlDocClass(hCurrentXmlDocClass);

                return WDV_False;
            }

            hLastXmlIndex = hCurrentXmlIndex;
            hCurrentXmlIndex = hCurrentXmlIndex->hNextIndex;


            /* Get next index */
            if (!XMLIMAPI_XmlIndexGetNextByDocClass(wdv->hXmlModelling, &hCurrentXmlIndex->index)) {
                /* There is no more need for the last constructed XML index */
                XMLIndexing_DestroyXmlIndex(hCurrentXmlIndex);
                hLastXmlIndex->hNextIndex = NULL;

                /* What kind of error occured? */
                if (!XMLIMAPI_IsError(wdv->hXmlModelling,
                                       XMLIMAPI_ERR_TYPE_MAPI,
                                       XMLIMAPI_ERR_MAPI_CODE_NO_MORE_INDICES)) {
                    Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                            WDVCAPI_ERR_CODE_DOC_CLASS_NOT_FOUND,
                                            WDVCAPI_ERR_TEXT_DOC_CLASS_NOT_FOUND);

                    XMLIndexing_DestroyXmlIndex(hCurrentXmlDocClass->hIndexList);
                    XMLIndexing_DestroyXmlDocClass(hCurrentXmlDocClass);

                    return WDV_False;
                }
            }
        /* Check error */
        } while (!XMLIMAPI_IsError(wdv->hXmlModelling,
                                    XMLIMAPI_ERR_TYPE_MAPI,
                                    XMLIMAPI_ERR_MAPI_CODE_NO_MORE_INDICES));

        /* Insert new doc class to the doc class list */
        hCurrentXmlDocClass->hNextDocClass = wdv->hXmlDocClassList;
        wdv->hXmlDocClassList = hCurrentXmlDocClass;
    }

    return WDV_True;

}

/***********************************************************************

	Module Functions

 ***********************************************************************/

WDV_Bool XMLIndexing_CreateXmlIndex( WDVCAPI_WDV       wdv,
                                     WDVCAPI_XmlIndex *hXmlIndex )
{

	WDV_Bool         ok = WDV_False;
    WDVCAPI_XmlIndex hNewXmlIndex = NULL;

	if ( !wdv ) {
		return WDV_False;
	}
    
	WDVCAPI_Allocat( sizeof(struct st_wdvcapi_xml_index), (WDV_UInt1**) &hNewXmlIndex, &ok );
	if (ok == WDV_False) {
		return WDV_False;
	}

	/* Init handle */
    if (!XMLIndexing_InitXmlIndex(wdv, hNewXmlIndex)) {
        WDVCAPI_Free(hNewXmlIndex);
        *hXmlIndex = NULL;

        return WDV_False;
    }

    /* Set handle */
    *hXmlIndex = hNewXmlIndex;

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool XMLIndexing_DestroyXmlIndex( WDVCAPI_XmlIndex hXmlIndex )
{

    WDVCAPI_XmlIndex hNextXmlIndex = NULL;

    /* Free XML index list */
    while(hXmlIndex) {
        hNextXmlIndex = hXmlIndex->hNextIndex;

        /* Free doc class list handle itself */
        WDVCAPI_Free((WDV_UInt1*) hXmlIndex);

        hXmlIndex = hNextXmlIndex;
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool XMLIndexing_InitXmlDocClass( WDVCAPI_WDV         wdv,
                                      WDVCAPI_XmlDocClass hXmlDocClass )
{


    memset(hXmlDocClass->docClassId, 0, WDVCAPI_MAX_ID_LEN);

    hXmlDocClass->hXPath = NULL;
    hXmlDocClass->hIndexList = NULL;
    hXmlDocClass->hNextDocClass = NULL;

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool XMLIndexing_InitXmlIndex( WDVCAPI_WDV      wdv,
                                   WDVCAPI_XmlIndex hXmlIndex )
{

    hXmlIndex->hWDV = NULL;
    hXmlIndex->hNextIndex = NULL;

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool XMLIndexing_FindDocClass( WDVCAPI_WDV              wdv,
                                   WDVCAPI_XmlDocClassId    xmlDocClassId,
                                   WDVCAPI_XmlDocClass     *phXmlDocClass )
{

    WDVCAPI_XmlDocClass hCurrentDocClass = NULL;

    /* Inits */
    *phXmlDocClass = NULL;

    /* Checks */
    if (!wdv || !phXmlDocClass) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                                WDVCAPI_ERR_TEXT_INTERNAL_ERROR);

        return WDV_False;
    }

    /* Get first doc class from doc class list */
    hCurrentDocClass = wdv->hXmlDocClassList;

    /* Search doc class list */
    while (hCurrentDocClass) {
        /* Doc class found? */
        if (WDVCAPI_IdsAreIdentical(hCurrentDocClass->docClassId, xmlDocClassId)) {
            *phXmlDocClass = hCurrentDocClass;
            return WDV_True;
        }

        /* Get next doc class */
        hCurrentDocClass = hCurrentDocClass->hNextDocClass;
    }

    return WDV_True;

}

/***********************************************************************

	End

 ***********************************************************************/
