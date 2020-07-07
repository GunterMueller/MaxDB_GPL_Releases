/*!**********************************************************************

  module: XMLDC_DocClass.h

  -----------------------------------------------------------------------

  responsible:	Markus Oezgen

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

#ifndef XMLDC_DOC_CLASS_H
#define XMLDC_DOC_CLASS_H

/***********************************************************************

	Includes

 ***********************************************************************/

#include "SAPDB/SAPDBCommon/SAPDB_Types.h"

/*---------------------------------------------------------------------------*/

#include "SAPDB/XML/Id/XMLId_Id.h"
#include "SAPDB/XML/Library/XMLLib_Library.h"
#include "SAPDB/XML/SessionPool/XMLSP_SessionPool.h"
#include "SAPDB/XML/Settings/XMLSet_Settings.h"

/*---------------------------------------------------------------------------*/

#include "SAPDB/XML/IndexingMetadataAPI/XMLIMAPI_Definition.h"
#include "SAPDB/XML/XPathEngine/XMLXPath_Indexing.h"

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Defines

 ***********************************************************************/

#define XMLDC_MAX_INDEX_NAME_SPACE_LEN 1023
#define XMLDC_MAX_INDEX_NAME_LEN       1023
#define XMLDC_MAX_BASE_PATH_LEN        1023
#define XMLDC_MAX_VALUE_PATH_LEN       1023

#define XMLDC_MAX_DOC_CLASS_ID_LEN          XMLID_MAX_ID_LEN
#define XMLDC_MAX_DOC_CLASS_ID_STRING_LEN   XMLID_MAX_ID_STRING_LEN

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Structs & Types

 ***********************************************************************/

typedef struct st_xmldc_doc_class   *XMLDC_DocClass;
typedef XMLId_Id                     XMLDC_DocClassId;
typedef XMLId_IdString               XMLDC_DocClassIdString;

typedef void                        *XMLDC_XPath;
typedef struct st_xmldc_index       *XMLDC_Index;

typedef char                         XMLDC_IndexNameSpaceString[XMLDC_MAX_INDEX_NAME_SPACE_LEN+1];
typedef char                        *XMLDC_IndexNameSpace;

typedef char                         XMLDC_IndexNameString[XMLDC_MAX_INDEX_NAME_LEN+1];
typedef char                        *XMLDC_IndexName;

typedef char                         XMLDC_BasePathString[XMLDC_MAX_BASE_PATH_LEN+1];
typedef char                        *XMLDC_BasePath;

typedef char                         XMLDC_ValuePathString[XMLDC_MAX_VALUE_PATH_LEN+1];
typedef char                        *XMLDC_ValuePath;

struct st_xmldc_doc_class {
    XMLDC_DocClassId            id;
    XMLDC_XPath                 hXPath;
    XMLDC_Index                 hIndexList;
    XMLDC_DocClass              hNextDocClass;
    XMLXPath_Idx_IndexHandler   setIndexFunction;
};

struct st_xmldc_index {
    XMLDC_IndexNameSpaceString indexNameSpace;
    XMLDC_IndexNameString      indexName;
    XMLDC_BasePathString       basePath;
    XMLDC_ValuePathString      valuePath;

    XMLDC_Index                hNextIndex;
};

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Prototypes

 ***********************************************************************/

SAPDB_Bool DocClass_Init();

SAPDB_Bool DocClass_Exit();

SAPDB_Bool DocClass_CreateDocClassList( XMLDC_DocClass              *hDocClassList,
                                        XMLIMAPI_Handle              hIMApi,
                                        XMLXPath_Idx_IndexHandler    setIndexFunction,
                                        XMLError_Error               hError );

SAPDB_Bool DocClass_DestroyDocClass( XMLDC_DocClass hDocClass );

SAPDB_Bool DocClass_FindDocClass( XMLDC_DocClass     hDocClasslist,
                                  XMLDC_DocClassId   docClassId,
                                  XMLDC_DocClass    *hDocClass,
                                  XMLError_Error     hError );

/***********************************************************************

	End

 ***********************************************************************/

#endif
