/*!***************************************************************************

  module      : XMLXPath_ParseExpr.h

  -------------------------------------------------------------------------

  responsible : FerdiF

  special area: XML

  description : convert token list into execution steps
 
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
#include "SAPDBCommon/SAPDB_string.h"

#include "SAPDB/XML/XPathEngine/XMLXPath_ParseExpr.h"
#include "SAPDB/XML/XPathEngine/XMLXPath_Memory.h"
#include "SAPDB/XML/XPathEngine/XMLXPath_Error.h"
#include "SAPDB/XML/XPathEngine/XMLXPath_Tokenize.h"

/*------------------------------------------------------------------------------*/
/* prototypes for all only locally defined and used functions                   */
/*------------------------------------------------------------------------------*/
static XMLXPath_Idx_RetCode
allocExecDesc( XMLXPath_TokenListPtr p );

/*------------------------------------------------------------------------------*/
static XMLXPath_PredicatePtr
allocPredicateItem(  XMLXPath_ContextPtr pCtx );

/*------------------------------------------------------------------------------*/
static SAPDB_Bool
hasPathExpr( XMLXPath_TokenListPtr p );

/*------------------------------------------------------------------------------*/
static SAPDB_Bool
isAxisnameSup( XMLXPath_ContextPtr pCtx,
               XMLXPath_ItemPtr item,
               XMLXPath_TokenEnum prev );

/*------------------------------------------------------------------------------*/
static SAPDB_Bool
isDColonSup( XMLXPath_ContextPtr pCtx,
             XMLXPath_ItemPtr item,
             XMLXPath_TokenEnum prev );

/*------------------------------------------------------------------------------*/
static SAPDB_Bool
isLBracketSup( XMLXPath_ContextPtr pCtx,
               XMLXPath_ItemPtr item,
               XMLXPath_TokenEnum prev );

/*------------------------------------------------------------------------------*/
static SAPDB_Bool
isLiteralSup( XMLXPath_ContextPtr pCtx,
              XMLXPath_ItemPtr item,
              XMLXPath_TokenEnum prev );

/*------------------------------------------------------------------------------*/
static SAPDB_Bool
isLParenSup( XMLXPath_ContextPtr pCtx,
             XMLXPath_ItemPtr item,
             XMLXPath_TokenEnum prev );

/*------------------------------------------------------------------------------*/
static SAPDB_Bool
isNameSup( XMLXPath_ContextPtr pCtx,
           XMLXPath_ItemPtr item,
           XMLXPath_TokenEnum prev );

/*------------------------------------------------------------------------------*/
static SAPDB_Bool
isNodenameSup( XMLXPath_ContextPtr pCtx,
               XMLXPath_ItemPtr item,
               XMLXPath_TokenEnum prev );

/*------------------------------------------------------------------------------*/
static SAPDB_Bool
isNullSup( XMLXPath_ContextPtr pCtx,
           XMLXPath_ItemPtr item,
           XMLXPath_TokenEnum prev );

/*------------------------------------------------------------------------------*/
static SAPDB_Bool
isOperandSup( XMLXPath_ContextPtr pCtx,
              XMLXPath_ItemPtr item,
              XMLXPath_TokenEnum prev );

/*------------------------------------------------------------------------------*/
static SAPDB_Bool
isRBracketSup( XMLXPath_ContextPtr pCtx,
               XMLXPath_ItemPtr item,
               XMLXPath_TokenEnum prev );

/*------------------------------------------------------------------------------*/
static SAPDB_Bool
isRParenSup( XMLXPath_ContextPtr pCtx,
             XMLXPath_ItemPtr item,
             XMLXPath_TokenEnum prev );

/*------------------------------------------------------------------------------*/
static SAPDB_Bool
isSlashSup( XMLXPath_ContextPtr pCtx,
            XMLXPath_ItemPtr item,
            XMLXPath_TokenEnum prev );

/*------------------------------------------------------------------------------*/
static XMLXPath_Idx_RetCode
incrStepsUsed( XMLXPath_ContextPtr pCtx,
               XMLXPath_TokenListPtr p,
               XMLXPath_StepPtr *step );

/*------------------------------------------------------------------------------*/
static XMLXPath_Idx_RetCode
parseExpr( XMLXPath_ContextPtr pCtx,
           XMLXPath_TokenListPtr p );

/*------------------------------------------------------------------------------*/
/* definition of static module local variables                                  */
/*------------------------------------------------------------------------------*/
static SAPDB_UTF8* starChar = (SAPDB_UTF8*)"*";

/*------------------------------------------------------------------------------*/
/* local defined functions                                                      */
/*------------------------------------------------------------------------------*/
static XMLXPath_Idx_RetCode
allocExecDesc( XMLXPath_TokenListPtr p )
{
    XMLXPath_Idx_RetCode      rc   = XMLXPath_RC_OK;

    if ( SAPDB_TRUE == hasPathExpr( p ) )
    {
        p->stepsAllocated  += 10;
        p->firstStep        = XMLXPath_Mem_Malloc(
                              sizeof( XMLXPath_Step ) * p->stepsAllocated );
        if ( !p->firstStep )
        {
            p->stepsAllocated   -= 10;
            rc = XMLXPath_RC_NOMEMORY;
        }
    }
    return rc;
}

/*------------------------------------------------------------------------------*/
static XMLXPath_PredicatePtr
allocPredicateItem(  XMLXPath_ContextPtr pCtx )
{
  XMLXPath_PredicatePtr   p;
  
  p = ( XMLXPath_Mem_Malloc( sizeof( XMLXPath_Predicate )));
    if ( p )
    {
        p->next        = NULL;
        p->name        = NULL;
        p->nameLen     = 0;
        p->value       = NULL;
        p->valueLen    = 0;
        p->selectOP    = XMLXPath_StepOp_NONE;
        p->nodeType    = XMLXPath_NodeType_Element;
    }
    else
        XMLXPath_Err_SetError( pCtx, XMLXPath_NO_MORE_MEMORY );

    return p;
}

/*------------------------------------------------------------------------------*/
static SAPDB_Bool
hasPathExpr( XMLXPath_TokenListPtr p )
{
    if ( 1 < p->tokenUsed )
        return SAPDB_TRUE;
    else
        return SAPDB_FALSE;
}

/*------------------------------------------------------------------------------*/
static SAPDB_Bool
isAxisnameSup( XMLXPath_ContextPtr pCtx,
               XMLXPath_ItemPtr    item,
               XMLXPath_TokenEnum  prev )
{
    SAPDB_Bool  rc;

    switch ( prev )
    {
    case XMLXPath_TOK_LBRACKET:
    case XMLXPath_TOK_DSLASH:
    case XMLXPath_TOK_NONE:
    case XMLXPath_TOK_SLASH:
        {
            SAPDB_UTF8 axisName[32];
            SAPDB_memcpy( &axisName[0], item->startChar, item->len );
            axisName[ item->len ] = '\0';

            if ( strcmp( "child", (char*)axisName ) &&
                    strcmp( "@", (char*)axisName ) &&
                    strcmp( "attribute", (char*)axisName ) &&
                    strcmp( "descendant", (char*)axisName ) )
            {
                rc = SAPDB_FALSE;
                XMLXPath_Err_SetErrorParse( pCtx, XMLXPath_NOT_SUPPORTED,
                                            item->startChar );
            }
            else
                rc = SAPDB_TRUE;
        }
        break;
    default:
        XMLXPath_Err_SetErrorParse( pCtx, XMLXPath_BAD_TOKEN, item->startChar );
        rc = SAPDB_FALSE;
    }
    return rc;
}

/*------------------------------------------------------------------------------*/
static SAPDB_Bool
isDColonSup( XMLXPath_ContextPtr pCtx,
             XMLXPath_ItemPtr    item,
             XMLXPath_TokenEnum  prev )
{
    SAPDB_Bool  rc;
    switch ( prev )
    {
    case XMLXPath_TOK_ATSIGN:
    case XMLXPath_TOK_AXISNAME:
        rc = SAPDB_TRUE;
        break;
    default:
        XMLXPath_Err_SetErrorParse( pCtx, XMLXPath_BAD_TOKEN, item->startChar );
        rc = SAPDB_FALSE;
    }
    return rc;
}

/*------------------------------------------------------------------------------*/
static SAPDB_Bool
isLBracketSup( XMLXPath_ContextPtr pCtx,
               XMLXPath_ItemPtr    item,
               XMLXPath_TokenEnum  prev )
{
    SAPDB_Bool  rc;
    switch ( prev )
    {
    case XMLXPath_TOK_NAME:
        rc = SAPDB_TRUE;
        break;
    default:
        XMLXPath_Err_SetErrorParse( pCtx, XMLXPath_BAD_TOKEN, item->startChar );
        rc = SAPDB_FALSE;
    }
    return rc;
}

/*------------------------------------------------------------------------------*/
static SAPDB_Bool
isLiteralSup( XMLXPath_ContextPtr pCtx,
              XMLXPath_ItemPtr    item,
              XMLXPath_TokenEnum  prev )
{
    SAPDB_Bool  rc;
    switch ( prev )
    {
    case XMLXPath_TOK_EQ:
    case XMLXPath_TOK_NEQ:
        rc = SAPDB_TRUE;
        break;
    default:
        XMLXPath_Err_SetErrorParse( pCtx, XMLXPath_BAD_TOKEN, item->startChar );
        rc = SAPDB_FALSE;
    }
    return rc;
}

/*------------------------------------------------------------------------------*/
static SAPDB_Bool
isLParenSup( XMLXPath_ContextPtr pCtx,
             XMLXPath_ItemPtr    item,
             XMLXPath_TokenEnum  prev )
{
    SAPDB_Bool  rc;
    switch ( prev )
    {
    case XMLXPath_TOK_NODENAME:
        rc = SAPDB_TRUE;
        break;
    default:
        XMLXPath_Err_SetErrorParse( pCtx, XMLXPath_BAD_TOKEN, item->startChar );
        rc = SAPDB_FALSE;
    }
    return rc;
}

/*------------------------------------------------------------------------------*/
static SAPDB_Bool
isNameSup( XMLXPath_ContextPtr pCtx,
           XMLXPath_ItemPtr    item,
           XMLXPath_TokenEnum  prev )
{
    SAPDB_Bool  rc;
    switch ( prev )
    {
    case XMLXPath_TOK_ATSIGN:
    case XMLXPath_TOK_AXISNAME:
    case XMLXPath_TOK_DCOLON:
    case XMLXPath_TOK_NONE:
    case XMLXPath_TOK_SLASH:
    case XMLXPath_TOK_DSLASH:
        rc = SAPDB_TRUE;
        break;
    default:
        XMLXPath_Err_SetErrorParse( pCtx, XMLXPath_NOT_SUPPORTED, item->startChar );
        rc = SAPDB_FALSE;
    }
    return rc;
}

/*------------------------------------------------------------------------------*/
static SAPDB_Bool
isNodenameSup( XMLXPath_ContextPtr pCtx,
               XMLXPath_ItemPtr    item,
               XMLXPath_TokenEnum  prev )
{
    SAPDB_Bool  rc = isNameSup( pCtx, item, prev );
    if ( SAPDB_TRUE == rc )
    {
        SAPDB_UTF8 nodeName[32];
        SAPDB_memcpy( (char*)&nodeName[0], (char*)item->startChar, item->len );
        nodeName[ item->len ] = '\0';
        if ( !strcmp( "text", (char*)nodeName) )
            return SAPDB_TRUE;
        else {
            rc = SAPDB_FALSE;
            XMLXPath_Err_SetErrorParse( pCtx, XMLXPath_NOT_SUPPORTED, item->startChar );
        }
    }
    return rc;
}

/*------------------------------------------------------------------------------*/
static SAPDB_Bool
isNullSup( XMLXPath_ContextPtr pCtx,
           XMLXPath_ItemPtr    item,
           XMLXPath_TokenEnum  prev )
{
    SAPDB_Bool  rc;

    switch ( prev )
    {
    case XMLXPath_TOK_NAME:
    case XMLXPath_TOK_RBRACKET:
    case XMLXPath_TOK_RPAREN:
        rc = SAPDB_TRUE;
        break;
    default:
        XMLXPath_Err_SetErrorParse( pCtx, XMLXPath_EXPR_NOT_TERMINATED,
                                    item->startChar );
        rc = SAPDB_FALSE;
    }
    return rc;
}

/*------------------------------------------------------------------------------*/
static SAPDB_Bool
isOperandSup( XMLXPath_ContextPtr pCtx,
              XMLXPath_ItemPtr    item,
              XMLXPath_TokenEnum  prev )
{
    SAPDB_Bool  rc;
    switch ( prev )
    {
    case XMLXPath_TOK_NAME:
        rc = SAPDB_TRUE;
        break;
    default:
        XMLXPath_Err_SetErrorParse( pCtx, XMLXPath_BAD_TOKEN, item->startChar );
        rc = SAPDB_FALSE;
    }
    return rc;
}

/*------------------------------------------------------------------------------*/
static SAPDB_Bool
isRBracketSup( XMLXPath_ContextPtr pCtx,
               XMLXPath_ItemPtr    item,
               XMLXPath_TokenEnum  prev )
{
    SAPDB_Bool  rc;
    switch ( prev )
    {
    case XMLXPath_TOK_LITERAL:
        rc = SAPDB_TRUE;
        break;
    default:
        XMLXPath_Err_SetErrorParse( pCtx, XMLXPath_BAD_TOKEN, item->startChar );
        rc = SAPDB_FALSE;
    }
    return rc;
}

/*------------------------------------------------------------------------------*/
static SAPDB_Bool
isRParenSup( XMLXPath_ContextPtr pCtx,
             XMLXPath_ItemPtr    item,
             XMLXPath_TokenEnum  prev )
{
    SAPDB_Bool  rc;
    switch ( prev )
    {
    case XMLXPath_TOK_LPAREN:
        rc = SAPDB_TRUE;
        break;
    default:
        XMLXPath_Err_SetErrorParse( pCtx, XMLXPath_NOT_SUPPORTED, item->startChar );
        rc = SAPDB_FALSE;
    }
    return rc;
}

/*------------------------------------------------------------------------------*/
static SAPDB_Bool
isSlashSup( XMLXPath_ContextPtr pCtx,
            XMLXPath_ItemPtr    item,
            XMLXPath_TokenEnum  prev )
{
    SAPDB_Bool  rc;
    switch ( prev )
    {
    case XMLXPath_TOK_RBRACKET:
    case XMLXPath_TOK_NAME:
    case XMLXPath_TOK_NONE:
        rc = SAPDB_TRUE;
        break;
    default:
        XMLXPath_Err_SetErrorParse( pCtx, XMLXPath_NOT_SUPPORTED, item->startChar );
        rc = SAPDB_FALSE;
    }
    return rc;
}


/*------------------------------------------------------------------------------*/
static XMLXPath_Idx_RetCode
incrStepsUsed(XMLXPath_ContextPtr    pCtx,
              XMLXPath_TokenListPtr  p,
              XMLXPath_StepPtr      *step )
{
    XMLXPath_Idx_RetCode      rc   = XMLXPath_RC_OK;

    *step = NULL;
    if ( p->stepsUsed == p->stepsAllocated )
    {
        XMLXPath_StepPtr firstStepOld = p->firstStep;
        rc = allocExecDesc( p );
        SAPDB_memcpy( p->firstStep, firstStepOld,
                sizeof( XMLXPath_Step ) * p->stepsAllocated );
        XMLXPath_Mem_Free( firstStepOld );
    };
    if ( XMLXPath_RC_OK == rc )
    {
        *step = p->firstStep + p->stepsUsed;
        (*step)->nodeType    = XMLXPath_NodeType_Element;
        (*step)->predicate   = NULL;
        (*step)->name        = NULL;
        (*step)->nameLen     = 0;
        (*step)->stepType    = XMLXPath_Step_Absolute;
        p->stepsUsed++;
        if ( p->stepsUsed > 1 )
        {
            XMLXPath_StepPtr pPrev = p->firstStep + p->stepsUsed - 2;
            if ( !pPrev->name || ( 0 == pPrev->nameLen ))
            {
                /* Error */;
            }
        }
    }
    return rc;
}

/*------------------------------------------------------------------------------*/
static XMLXPath_Idx_RetCode
parseExpr( XMLXPath_ContextPtr   pCtx,
           XMLXPath_TokenListPtr p )
{
    XMLXPath_ItemPtr          item;
    XMLXPath_StepPtr          step;
    XMLXPath_PredicatePtr     predicate;
    XMLXPath_Idx_RetCode      rc;
    int                       i;
    XMLXPath_TokenEnum        prevTok = XMLXPath_TOK_NONE;
    SAPDB_Bool                isPredicate = SAPDB_FALSE;

    rc = XMLXPath_RC_OK;
    for ( i=0; ( XMLXPath_RC_OK == rc ) && ( i < p->tokenUsed ); i++ )
    {
        item = p->firstToken + i;
        switch( item->token )
        {
        case XMLXPath_TOK_NULL:
            if (( SAPDB_FALSE == isNullSup( pCtx, item, prevTok )) ||
                    ( SAPDB_TRUE == isPredicate ))
                rc = XMLXPath_RC_SYNTAX;
            break;
        case XMLXPath_TOK_NAME:
            if ( SAPDB_TRUE == isNameSup( pCtx, item, prevTok ))
            {
                if ( isPredicate )
                {
                    if ( ( XMLXPath_NodeType_Attribute != predicate->nodeType ))
                    {
                        XMLXPath_Err_SetErrorString( pCtx, XMLXPath_NOT_YET_IMPLEMENTED,
                                                     (SAPDB_UTF8*)"predicate using nodetype other than attribute" );
                        rc = XMLXPath_RC_SYNTAX;
                    }
                    else
                    {
                        predicate->name    = item->startChar;
                        predicate->nameLen = item->len;
                    }
                }
                else
                {
                    if ( XMLXPath_TOK_NONE == prevTok )
                        rc = incrStepsUsed( pCtx, p, &step );
                    step->name    = item->startChar;
                    step->nameLen = item->len;
                }
            }
            else
                rc = XMLXPath_RC_SYNTAX;
            break;
        case XMLXPath_TOK_AXISNAME:
            if ( SAPDB_TRUE == isAxisnameSup( pCtx, item, prevTok ))
            {
                SAPDB_UTF8 axisName[32];
                SAPDB_memcpy( &axisName[0], item->startChar, item->len );
                axisName[ item->len ] = '\0';
                if ( isPredicate )
                {
                    if ( strcmp( "@", (char*)axisName ) &&
                            strcmp( "attribute", (char*)axisName ) )
                    {
                        XMLXPath_Err_SetErrorParse( pCtx, XMLXPath_NOT_SUPPORTED, item->startChar );
                        rc = XMLXPath_RC_SYNTAX;
                    }
                    else
                    {
                        predicate->nodeType = XMLXPath_NodeType_Attribute;
                    }
                }
                else
                {
                    if ( XMLXPath_TOK_NONE == prevTok )
                    {
                        rc = incrStepsUsed( pCtx, p, &step );
                    }

                    if ( !strcmp( "@", (char*)axisName ) ||
                            !strcmp( "attribute", (char*)axisName ) )
                        step->nodeType = XMLXPath_NodeType_Attribute;
                    if ( !strcmp( "descendant", (char*)axisName ) )
                        step->stepType = XMLXPath_Step_Relative;
                }
            }
            else
                rc = XMLXPath_RC_SYNTAX;
            break;
        case XMLXPath_TOK_NODENAME:
            if ( ! isPredicate )
            {
                if ( SAPDB_TRUE == isNodenameSup( pCtx, item, prevTok ))
                {
                    if ( XMLXPath_TOK_NONE == prevTok )
                    {
                        rc = incrStepsUsed( pCtx, p, &step );
                    }
                    step->name     = item->startChar;
                    step->nameLen  = item->len;
                    step->nodeType = XMLXPath_NodeType_Nodename;
                }
                else
                    rc = XMLXPath_RC_SYNTAX;
            }
            else
            {
                XMLXPath_Err_SetErrorParse( pCtx, XMLXPath_NOT_SUPPORTED,
                                            item->startChar );
                rc = XMLXPath_RC_SYNTAX;
            }
            break;
        case XMLXPath_TOK_LPAREN:
            if (!SAPDB_TRUE == isLParenSup( pCtx, item, prevTok ))
                rc = XMLXPath_RC_SYNTAX;
            break;
        case XMLXPath_TOK_RPAREN:
            if (!SAPDB_TRUE == isRParenSup( pCtx, item, prevTok ))
                rc = XMLXPath_RC_SYNTAX;
            break;
        case XMLXPath_TOK_LBRACKET:
            if ( ! isPredicate )
            {
                if ( SAPDB_TRUE == isLBracketSup( pCtx, item, prevTok ))
                {
                    predicate = allocPredicateItem( pCtx );
                    if ( ! predicate )
                        rc = XMLXPath_RC_NOMEMORY;
                    else
                    {
                        /* chain to end of predicate list */
                        XMLXPath_PredicatePtr prv;
                        XMLXPath_PredicatePtr nxt;
                        isPredicate = SAPDB_TRUE;
                        if ( ! step->predicate )
                            step->predicate = predicate;
                        else
                        {
                            prv = step->predicate;
                            nxt = prv->next;
                            while ( nxt )
                            {
                                prv = nxt;
                                nxt = prv->next;
                            }
                            prv->next = predicate;
                        }
                    }
                }
                else
                    rc = XMLXPath_RC_SYNTAX;
            }
            else
            {
                XMLXPath_Err_SetErrorParse( pCtx, XMLXPath_NOT_SUPPORTED,
                                            item->startChar );
                rc = XMLXPath_RC_SYNTAX;
            }
            break;
        case XMLXPath_TOK_RBRACKET:
            if ( isPredicate )
            {
                if ( isRBracketSup( pCtx, item, prevTok ))
                {
                    isPredicate = SAPDB_FALSE;
                }
                else
                    rc = XMLXPath_RC_SYNTAX;
            }
            else
            {
                XMLXPath_Err_SetErrorParse( pCtx, XMLXPath_NOT_SUPPORTED,
                                            item->startChar );
                rc = XMLXPath_RC_SYNTAX;
            }
            break;
        case XMLXPath_TOK_ATSIGN:
            if ( SAPDB_TRUE == isAxisnameSup( pCtx, item, prevTok ))
            {
                if ( isPredicate )
                    predicate->nodeType = XMLXPath_NodeType_Attribute;
                else
                {
                    if ( XMLXPath_TOK_NONE == prevTok )
                    {
                        rc = incrStepsUsed( pCtx, p, &step );
                    }
                    step->nodeType = XMLXPath_NodeType_Attribute;
                }
            }
            else
                rc = XMLXPath_RC_SYNTAX;
            break;
        case XMLXPath_TOK_DCOLON:
            if ( SAPDB_FALSE == isDColonSup( pCtx, item, prevTok ))
                rc = XMLXPath_RC_SYNTAX;
            break;
        case XMLXPath_TOK_DSLASH:
            if ( ! isPredicate )
            {
                if ( SAPDB_TRUE == isSlashSup( pCtx, item, prevTok ))
                {
                    rc = incrStepsUsed( pCtx, p, &step );
                    step->stepType = XMLXPath_Step_Relative;
                }
                else
                    rc = XMLXPath_RC_SYNTAX;
            }
            else
            {
                XMLXPath_Err_SetErrorParse( pCtx, XMLXPath_NOT_SUPPORTED,
                                            item->startChar );
                rc = XMLXPath_RC_SYNTAX;
            }
            break;
        case XMLXPath_TOK_SLASH:
            if ( ! isPredicate )
            {
                if ( SAPDB_TRUE == isSlashSup( pCtx, item, prevTok ))
                    rc = incrStepsUsed( pCtx, p, &step );
                else
                    rc = XMLXPath_RC_SYNTAX;
            }
            else
            {
                XMLXPath_Err_SetErrorParse( pCtx, XMLXPath_NOT_SUPPORTED,
                                            item->startChar );
                rc = XMLXPath_RC_SYNTAX;
            }
            break;
        case XMLXPath_TOK_LITERAL:
            if ( isPredicate && isLiteralSup( pCtx, item, prevTok ))
            {
                /* store without surrounding quotes */
                predicate->value     = item->startChar + 1;
                predicate->valueLen  = item->len - 2;
            }
            else
            {
                XMLXPath_Err_SetErrorParse( pCtx, XMLXPath_NOT_SUPPORTED,
                                            item->startChar );
                rc = XMLXPath_RC_SYNTAX;
            }
            break;
        case XMLXPath_TOK_EQ:
            if ( isPredicate && isOperandSup( pCtx, item, prevTok ))
            {
                predicate->selectOP = XMLXPath_StepOp_EQ;
            }
            else
            {
                XMLXPath_Err_SetErrorParse( pCtx, XMLXPath_NOT_SUPPORTED,
                                            item->startChar );
                rc = XMLXPath_RC_SYNTAX;
            }
            break;
        case XMLXPath_TOK_NEQ:
            if ( isPredicate && isOperandSup( pCtx, item, prevTok ))
            {
                predicate->selectOP = XMLXPath_StepOp_NEQ;
            }
            else
            {
                XMLXPath_Err_SetErrorParse( pCtx, XMLXPath_NOT_SUPPORTED,
                                            item->startChar );
                rc = XMLXPath_RC_SYNTAX;
            }
            break;
        case XMLXPath_TOK_NUMBER:
        case XMLXPath_TOK_COMMA:
        case XMLXPath_TOK_VAR:
        case XMLXPath_TOK_OR:
        case XMLXPath_TOK_AND:
        case XMLXPath_TOK_LT:
        case XMLXPath_TOK_GT:
        case XMLXPath_TOK_LE:
        case XMLXPath_TOK_GE:
        case XMLXPath_TOK_PLUS:
        case XMLXPath_TOK_SUBT:
        case XMLXPath_TOK_MULT:
        case XMLXPath_TOK_MOD:
        case XMLXPath_TOK_DIV:
        case XMLXPath_TOK_MINUS:
        case XMLXPath_TOK_VERT:
        case XMLXPath_TOK_STAR:
        case XMLXPath_TOK_PERIOD:
        case XMLXPath_TOK_DPERIOD:
        case XMLXPath_TOK_FUNCNAME:
        case XMLXPath_TOK_NONE:
            XMLXPath_Err_SetErrorParse( pCtx, XMLXPath_NOT_SUPPORTED,
                                        item->startChar );
            rc = XMLXPath_RC_SYNTAX;
            break;
        default:
            ;
        }
        prevTok = item->token;
    }
    return rc;
}

/*------------------------------------------------------------------------------*/
XMLXPath_Idx_RetCode
XMLXPath_PE_ParseExpr( XMLXPath_ContextPtr     pCtx )
{
    XMLXPath_TokenListPtr      pb;
    XMLXPath_TokenListPtr      pv;
    XMLXPath_Idx_RetCode      rc = XMLXPath_RC_OK;

    pb = pCtx->idxListInWork->itemListBase;
    if ( SAPDB_TRUE == hasPathExpr( pb ) )
    {
        rc = allocExecDesc( pb );
        if ( XMLXPath_RC_OK == rc )
            rc = parseExpr( pCtx, pb );
    }

    pv = pCtx->idxListInWork->itemListValue;
    if (( XMLXPath_RC_OK == rc ) && ( SAPDB_TRUE == hasPathExpr( pv )))
    {
        rc = allocExecDesc( pv );
        if ( XMLXPath_RC_OK == rc )
            rc = parseExpr( pCtx, pv );
        /* **********************************************
        if ( XMLXPath_RC_OK == rc  && SAPDB_TRUE == hasPathExpr( pb ) &&
                pv->firstToken->token == XMLXPath_TOK_SLASH &&
                pv->firstStep->stepType != XMLXPath_Step_Relative )
        {
            XMLXPath_Err_SetErrorParse( pCtx, XMLXPath_NO_ABSOLUTE_PATH_ALLOWED,
                                        pv->firstToken->startChar );
            rc = XMLXPath_RC_SYNTAX;
        }
        *********************************************** */
    }

    if (( XMLXPath_RC_OK == rc ) &&
            ! hasPathExpr( pCtx->idxListInWork->itemListValue ))
    {
        XMLXPath_Err_SetErrorParse( pCtx, XMLXPath_NO_VALUE_PATHEXPR, NULL );
        rc = XMLXPath_RC_SYNTAX;
    }
    return rc;
}
