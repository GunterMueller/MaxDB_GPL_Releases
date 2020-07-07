/*!***************************************************************************

  module:       XMLXPath_Indexing.c

  ------------------------------------------------------------------------------

  responsible:  FerdiF

  special area: XML UTF-8

  description:  Select values within XML documents which can be used to
                build indexes on XML documents:
                The values to be selected are specified via ( a subset of )
                XPATH expressions.
                The evaluation is done using one SAX parse thru the document.
  see also:     

  ------------------------------------------------------------------------------

  copyright:            Copyright (c) 2000-2005 SAP AG




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
#include "SAPDB/XML/XPathEngine/XMLXPath_Indexing.h"
#include "SAPDB/XML/XPathEngine/XMLXPath_Error.h"
#include "SAPDB/XML/XPathEngine/XMLXPath_Memory.h"
#include "SAPDB/XML/XPathEngine/XMLXPath_ParseExpr.h"
#include "SAPDB/XML/XPathEngine/XMLXPath_Tokenize.h"
#include "SAPDB/XML/XPathEngine/XMLXPath_Types.h"
#include "SAPDB/XML/XPathEngine/XMLXPath_ParseDocument.h"

/*------------------------------------------------------------------------------*/
/* prototypes for all only locally defined and used functions                   */
/*------------------------------------------------------------------------------*/
static void
deleteIdxList( XMLXPath_IndexListPtr p );

/*------------------------------------------------------------------------------*/
/* locally defined static functions                                             */
/*------------------------------------------------------------------------------*/
static void deleteIdxList( XMLXPath_IndexListPtr p )
{
    short i;
    if ( p->itemListBase && p->itemListBase->firstToken )
        XMLXPath_Mem_Free( p->itemListBase->firstToken );
    if ( p->itemListValue && p->itemListValue->firstToken )
        XMLXPath_Mem_Free( p->itemListValue->firstToken );
    if ( p->itemListBase && p->itemListBase->firstStep )
    {
        XMLXPath_StepPtr pStp = p->itemListBase->firstStep;
        for ( i=0; i < p->itemListBase->stepsUsed; ++i, ++pStp )
        {
            XMLXPath_PredicatePtr     curr = pStp->predicate;
            XMLXPath_PredicatePtr     next;
            while ( curr )
            {
                next = curr->next;
                XMLXPath_Mem_Free( curr );
                curr = next;
            }
        }
        XMLXPath_Mem_Free( p->itemListBase->firstStep );
    }
    if ( p->itemListValue && p->itemListValue->firstStep )
    {
        XMLXPath_StepPtr pStp = p->itemListValue->firstStep;
        for ( i=0; i < p->itemListValue->stepsUsed; ++i, ++pStp )
        {
            XMLXPath_PredicatePtr     curr = pStp->predicate;
            XMLXPath_PredicatePtr     next;
            while ( curr )
            {
                next = curr->next;
                XMLXPath_Mem_Free( curr );
                curr = next;
            }
        }
        XMLXPath_Mem_Free( p->itemListValue->firstStep );
    }
    XMLXPath_Mem_Free( p );
}


/*------------------------------------------------------------------------------*/
/* globally defined functions                                                   */
/*------------------------------------------------------------------------------*/
XMLXPath_Idx_RetCode
XMLXPath_Idx_DefineIndex( void                      *p,
                          void                      *idxUserData,
                          XMLXPath_Idx_IndexHandler  idxHandler,
                          SAPDB_UTF8                *basePath,
                          SAPDB_UTF8                *valuePath,
                          XMLXPath_Idx_Category      valueCategory )
{
    XMLXPath_Idx_RetCode      rc;
    XMLXPath_ContextPtr       pCtx     = (XMLXPath_ContextPtr)p;

    XMLXPath_Err_SetError( pCtx, XMLXPath_NOERR );
    rc = XMLXPath_Tok_Tokenize( pCtx, basePath, valuePath );
    if ( XMLXPath_RC_OK == rc )
    {
        rc = XMLXPath_PE_ParseExpr( pCtx );
        if ( XMLXPath_RC_OK == rc )
        {
            XMLXPath_IndexListPtr *idx = &pCtx->idxList;
            while ( *idx )
                idx = &(*idx)->next;
            pCtx->idxListInWork->itemListValue->category = valueCategory;
            pCtx->idxListInWork->indexHandler = idxHandler;
            pCtx->idxListInWork->userData     = idxUserData;
            *idx = pCtx->idxListInWork;

            /* release memory (Token description arrays) not needed anymore */
            if ( pCtx->idxListInWork->itemListBase->firstToken )
                XMLXPath_Mem_Free( pCtx->idxListInWork->itemListBase->firstToken );
            if ( pCtx->idxListInWork->itemListValue->firstToken )
                XMLXPath_Mem_Free( pCtx->idxListInWork->itemListValue->firstToken );
            pCtx->idxListInWork->itemListBase->firstToken = NULL;
            pCtx->idxListInWork->itemListBase->tokenUsed = 0;
            pCtx->idxListInWork->itemListBase->tokenAllocated = 0;
            pCtx->idxListInWork->itemListValue->firstToken = NULL;
            pCtx->idxListInWork->itemListValue->tokenUsed = 0;
            pCtx->idxListInWork->itemListValue->tokenAllocated = 0;
        }
        else
        {
            deleteIdxList( pCtx->idxListInWork );
        };
        pCtx->idxListInWork = NULL;
    }
    return rc;
}

/*------------------------------------------------------------------------------*/
XMLXPath_Idx_RetCode
XMLXPath_Idx_Initialize( void **p )
{
    XMLXPath_Idx_RetCode    rc;
    XMLXPath_ContextPtr     pCtx;

    pCtx = XMLXPath_Mem_Malloc( sizeof(XMLXPath_Context) );
    if ( !pCtx )
    {
        *p = NULL;
        rc = XMLXPath_RC_NOMEMORY;
    }
    else
    {
        *p                    = pCtx;
        pCtx->theParser       = NULL;
        pCtx->idxList         = NULL;
        pCtx->idxActiveList   = NULL;
        pCtx->actualTreeLevel = 0;
        pCtx->readCallBackFlg = SAPDB_TRUE;
        XMLXPath_Err_SetError( pCtx, XMLXPath_NOERR );
        pCtx->retCode         = XMLXPath_RC_OK;
        rc                    = XMLXPath_RC_OK;
    }
    return rc;
}

/*------------------------------------------------------------------------------*/
XMLXPath_Idx_RetCode
XMLXPath_Idx_Parse( void                     *p,
                    void                     *userData,
                    XMLXPath_Idx_ReadXmlBuff  readHandler )
{
    XMLXPath_Idx_RetCode rc;
    XMLXPath_ContextPtr   pCtx  = (XMLXPath_ContextPtr)p;
    XMLXPath_Err_SetError( pCtx, XMLXPath_NOERR );
    if ( pCtx->readCallBackFlg )
        rc = XMLXPath_PD_ParseDocument( pCtx, userData, readHandler );
    else
    {
        XMLXPath_Err_SetError( pCtx, XMLXPath_MIX_SYNC_ASYNC_READ );
        rc = XMLXPath_RC_FAILED;
    }
    return rc;
}

/*------------------------------------------------------------------------------*/
XMLXPath_Idx_RetCode
XMLXPath_Idx_ParseBuf( void          *p,
                       unsigned char *buf,
                       SAPDB_Int      bufLen,
                       void          *userData,
                       SAPDB_Bool     endOfDoc )
{
    XMLXPath_ContextPtr   pCtx  = (XMLXPath_ContextPtr)p;
    XMLXPath_Err_SetError( pCtx, XMLXPath_NOERR );
    return XMLXPath_PD_ParseDocumentBuf( p, buf, bufLen, userData, endOfDoc );
}

/*------------------------------------------------------------------------------*/
XMLXPath_Idx_RetCode
XMLXPath_Idx_Terminate( void *p )
{
    XMLXPath_ContextPtr   pCtx  = (XMLXPath_ContextPtr)p;
    if ( pCtx && pCtx->idxList )
    {
        XMLXPath_IndexListPtr  curr = pCtx->idxList;
        XMLXPath_IndexListPtr  next = curr->next;

        while ( curr )
        {
            deleteIdxList( curr );
            curr = next;
            if ( next ) next = next->next;
        }
    }
    XMLXPath_Mem_Free( pCtx );
    return XMLXPath_RC_OK;
}

