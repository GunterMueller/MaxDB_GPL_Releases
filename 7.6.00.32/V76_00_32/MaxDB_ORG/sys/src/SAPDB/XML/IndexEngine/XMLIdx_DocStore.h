/*!**********************************************************************

  module: XMLIdx_DocStore.h

  -----------------------------------------------------------------------

  responsible:	Markus Oezgen

  special area: XML Index Engine

  description:  Doc Store Handling

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

#ifndef XMLIDX_DOC_STORE_H
#define XMLIDX_DOC_STORE_H

/***********************************************************************

	Includes

 ***********************************************************************/

#include "SAPDB/SAPDBCommon/SAPDB_Types.h"

#include "SAPDB/XML/IndexEngine/XMLIdx_Workers.h"

#include "SAPDB/XML/Id/XMLId_Id.h"
#include "SAPDB/XML/Error/XMLError_Error.h"
#include "SAPDB/XML/SessionPool/XMLSP_SessionPool.h"

/***********************************************************************

	Defines

 ***********************************************************************/

#define DOCSTORE_MAX_CONTENT_BUFFER                 8192
#define DOCSTORE_MAX_PROPERTY_SHORT_VALUE_LEN       900

/***********************************************************************

	Structs & Types

 ***********************************************************************/

/* XML Parsing states */
enum en_xmlidx_parsing_state {
    XMLIDX_STATE_DOC_HAS_TO_BE_PARSED,
    XMLIDX_STATE_DOC_PARSING_IN_PROCESS,
    XMLIDX_STATE_DOC_PARSING_HAS_BEEN_FINISHED
};

typedef enum en_xmlidx_parsing_state    XMLIdx_IndexingState;

/*---------------------------------------------------------------------------*/

typedef struct st_xmlidx_doc_store_data         *XMLIdx_DocStoreData;

struct st_xmlidx_doc_store_data {
    SQLHSTMT                hStmtSelectNextDoc;
    SQLHSTMT                hStmtSelectDocClassId;
    SQLHSTMT                hStmtUpdateDocumentState;

    XMLId_Id                cId;
    SQLLEN                  cIdIndicator;

    XMLId_Id                docClassId;
    XMLId_IdString          docClassIdString;
    SQLLEN                  docClassIdStringIndicator;

    /* Get content */
    SQLHSTMT                hStmtGetContent;
    SQLHSTMT                hStmtGetContentLength;
    SAPDB_Bool              firstBlockDone;
    XMLId_Id                getContentResourceId;
    SQLLEN                  getContentResourceIdIndicator;
    unsigned char           contentBuffer[DOCSTORE_MAX_CONTENT_BUFFER];
    char                    propertyContentLength[DOCSTORE_MAX_PROPERTY_SHORT_VALUE_LEN];
    SQLLEN                  propertyContentLengthIndicator;
    SAPDB_ULong             numberContentLength;

    /* Set state */
    SQLHSTMT                hStmtSetState;
    char                    state[DOCSTORE_MAX_PROPERTY_SHORT_VALUE_LEN];
    SQLLEN                  stateIndicator;
    XMLId_Id                stateResourceId;
    SQLLEN                  stateResourceIdIndicator;
};

/***********************************************************************

	Prototypes

 ***********************************************************************/

SAPDB_Bool  DocStore_SelectNextDoc( XMLSP_Session     hSessionDocStore,
                                    XMLId_Id          docId,
                                    XMLId_Id          docClassId,
                                    XMLError_Error    hError );

SAPDB_Bool DocStore_ReadDoc( XMLIdx_Work      hWork,
                             unsigned char  **buffer,
                             SAPDB_Int       *bufferLength,
                             SAPDB_Int       *endOfDoc,
                             XMLError_Error   hError );

SAPDB_Bool DocStore_SetState( XMLSP_Session         hSessionDocStore,
                              XMLId_Id              docId,
                              XMLIdx_IndexingState  state,
                              XMLError_Error        hError );

/***********************************************************************

	End

 ***********************************************************************/

#endif
