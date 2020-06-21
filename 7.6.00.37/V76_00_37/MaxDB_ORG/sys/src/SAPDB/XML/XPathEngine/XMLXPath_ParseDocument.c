/*!***************************************************************************

  module      : XMLXPath_ParseDocument.c

  -------------------------------------------------------------------------

  responsible : FerdiF

  special area: XML

  description : SAX parser interface
 
  last changed: 2001-05-31  18:24
  
  see also    : 

  -------------------------------------------------------------------------

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




*****************************************************************************/
#include <string.h>
#include "SAPDB/SAPDBCommon/SAPDB_Types.h"
#include "SAPDB/XML/XPathEngine/XMLXPath_ParseDocument.h"
#include "SAPDB/XML/XPathEngine/XMLXPath_Error.h"
#include "SAPDB/XML/XPathEngine/XMLXPath_Memory.h"
#include "SAPDB/XML/XPathEngine/XMLXPath_ParseSaxHandling.h"


/*------------------------------------------------------------------------------*/
static void
CharacterHandler( void           *pContext,
                  const XML_Char *s,
                  int            len )
{
    XMLXPath_ContextPtr  pCtx  = (XMLXPath_ContextPtr)pContext;
    XMLXPath_PSH_CharacterHandler( pCtx,
                                   (const SAPDB_UTF8*) s,
                                   len );
}

/*------------------------------------------------------------------------------*/
static void
StartElementHandler( void           *pContext,
                     const XML_Char *elementName,
                     const XML_Char **atts)
{
    XMLXPath_ContextPtr  pCtx  = (XMLXPath_ContextPtr)pContext;
    pCtx->actualTreeLevel++;
    XMLXPath_PSH_StartElementHandler( pCtx,
                                      (const SAPDB_UTF8*)elementName,
                                      (const SAPDB_UTF8**)atts );
}

/*------------------------------------------------------------------------------*/
static void
EndElementHandler ( void           *pContext,
                    const XML_Char *elementName )
{
    XMLXPath_ContextPtr  pCtx  = (XMLXPath_ContextPtr)pContext;

    XMLXPath_PSH_EndElementHandler( pCtx,
                                    (const SAPDB_UTF8*)elementName );
    pCtx->actualTreeLevel--;
}

/*------------------------------------------------------------------------------*/
static XMLXPath_Idx_RetCode
initParser( XMLXPath_ContextPtr  pCtx )
{
    XML_Memory_Handling_Suite   memhandle;
    XMLXPath_Idx_RetCode        rc;

    rc = XMLPath_PSH_CreateIdxWorkItemList( pCtx );
    if ( XMLXPath_RC_OK == rc )
    {
        memhandle.malloc_fcn  = XMLXPath_Mem_Malloc;
        memhandle.realloc_fcn = XMLXPath_Mem_Realloc;
        memhandle.free_fcn    = XMLXPath_Mem_Free;

        pCtx->theParser =
            (XML_Parser)XML_ParserCreate_MM( NULL,        /* use document encoding declaration            */
                                             &memhandle,  /* using own suite of memory handling functions */
                                             NULL );      /* no namespace handling                        */
        if ( !pCtx->theParser )
        {
            XMLXPath_Err_SetError( pCtx, XMLXPath_CREATE_PARSER_FAILED );
            rc = XMLXPath_RC_FAILED;
        }
        else
        {
            XML_SetElementHandler(pCtx->theParser, StartElementHandler, EndElementHandler);
            XML_SetCharacterDataHandler(pCtx->theParser, CharacterHandler);
            XML_SetUserData( pCtx->theParser, pCtx);
        }
    }
    return rc;
}

/*------------------------------------------------------------------------------*/
XMLXPath_Idx_RetCode
XMLXPath_PD_ParseDocument( void                     *pContext,
                           void                     *userData,
                           XMLXPath_Idx_ReadXmlBuff  readHandler )
{
    unsigned char              *bufPtr;
    SAPDB_Int                   bufLen;
    SAPDB_Int                   dataEnd;
    XMLXPath_Idx_RetCode        rc;
    XMLXPath_ContextPtr         pCtx    = (XMLXPath_ContextPtr)pContext;

    rc = initParser( pCtx );
    if ( XMLXPath_RC_OK == rc )
    {

        /* Store user defined data */
        pCtx->userData = userData;

        do {
            pCtx->retCode = readHandler( userData, &bufPtr, &bufLen, &dataEnd );
            if ( XMLXPath_RC_OK != pCtx->retCode )
            {
                dataEnd = 1;
                XMLXPath_Err_SetError( pCtx, XMLXPath_XML_READ_HANDLER_FAILED );
                rc = pCtx->retCode;
            }
            else
            {
                if (!XML_Parse(pCtx->theParser, (char*)bufPtr, bufLen, dataEnd))
                {
                    int err      = XML_GetErrorCode( pCtx->theParser );
                    int lineno   = XML_GetCurrentLineNumber( pCtx->theParser );
                    int columnno = XML_GetCurrentColumnNumber( pCtx->theParser );
                    long byteIdx = XML_GetCurrentByteIndex( pCtx->theParser );
                    const XML_LChar *errmsg = XML_ErrorString( err );
                    XMLXPath_Err_SetErrorSaxParser( pCtx, err, (SAPDB_UTF8*)errmsg, lineno, columnno, byteIdx );
                    dataEnd = 1;
                    rc = XMLXPath_RC_FAILED;
                }
                else
                {
                    if ( XMLXPath_RC_OK != pCtx->retCode )
                    {
                        dataEnd = 1;
                        rc = pCtx->retCode;
                    }
                }
            }
        } while ( !dataEnd );

        XML_ParserFree(pCtx->theParser);
    }
    XMLXPath_PSH_ReleaseResources( pCtx );
    return rc;
}

/*------------------------------------------------------------------------------*/
XMLXPath_Idx_RetCode
XMLXPath_PD_ParseDocumentBuf( void          *pContext,
                              unsigned char *buf,
                              SAPDB_Int      bufLen,
                              void          *userData,
                              SAPDB_Bool     endOfDoc )
{
    XMLXPath_Idx_RetCode        rc      = XMLXPath_RC_OK;
    XMLXPath_ContextPtr         pCtx    = (XMLXPath_ContextPtr)pContext;

    if ( pCtx->readCallBackFlg )
    {
        rc = initParser( pCtx );
    }
    if ( XMLXPath_RC_OK == rc )
    {
        /* Set user data */
        pCtx->userData = userData;

        pCtx->readCallBackFlg = SAPDB_FALSE;
        if (!XML_Parse(pCtx->theParser, (char*)buf, bufLen, endOfDoc))
        {
            int err      = XML_GetErrorCode( pCtx->theParser );
            int lineno   = XML_GetCurrentLineNumber( pCtx->theParser );
            int columnno = XML_GetCurrentColumnNumber( pCtx->theParser );
            long byteIdx = XML_GetCurrentByteIndex( pCtx->theParser );
            const XML_LChar *errmsg = XML_ErrorString( err );
            XMLXPath_Err_SetErrorSaxParser( pCtx, err, (SAPDB_UTF8*)errmsg, lineno, columnno, byteIdx );
            rc = XMLXPath_RC_FAILED;
        }
    }
    if ( endOfDoc || ( XMLXPath_RC_OK != rc ) )
    {
        if ( pCtx->theParser )
            XML_ParserFree(pCtx->theParser);
        pCtx->readCallBackFlg = SAPDB_TRUE;
        XMLXPath_PSH_ReleaseResources( pCtx );
    }
    return rc;
}
