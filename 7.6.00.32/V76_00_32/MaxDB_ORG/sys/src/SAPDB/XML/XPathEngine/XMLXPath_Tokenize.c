/*!***************************************************************************

  module      : XMLXPath_Tokenize.c

  -------------------------------------------------------------------------

  responsible : FerdiF

  special area: XML

  description : tokenize XPath expression
 
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
#include "SAPDB/XML/XPathEngine/XMLXPath_Tokenize.h"
#include "SAPDB/XML/XPathEngine/XMLXPath_Error.h"
#include "SAPDB/XML/XPathEngine/XMLXPath_Memory.h"
#include "SAPDB/XML/XML_UTF8.h"
#include "SAPDBCommon/SAPDB_string.h"

/*------------------------------------------------------------------------------*/
/* prototypes of local defined/used functions                                   */
static XMLXPath_Idx_RetCode
allocItems( XMLXPath_TokenListPtr p );

static XMLXPath_Idx_RetCode
checkExprToken( XMLXPath_ContextPtr       pCtx,
                SAPDB_UTF8              **p,
                XMLXPath_TokenEnum        prev,
                XMLXPath_TokenEnum       *tok );

static XMLXPath_Idx_RetCode
getExprToken( XMLXPath_ContextPtr       pCtx,
              SAPDB_UTF8              **xpathStr,
              XMLXPath_ItemPtr          item,
              XMLXPath_TokenEnum        prevToken );

static XMLXPath_Idx_RetCode
getExprTokenItem( XMLXPath_ContextPtr    pCtx,
                  SAPDB_UTF8           **xpathStr,
                  XMLXPath_TokenListPtr   itemList,
                  XMLXPath_TokenEnum     prevToken );

static XMLXPath_Idx_RetCode
getName( XMLXPath_ContextPtr        pCtx,
         XMLXPath_ItemPtr           item,
         SAPDB_UTF8               **p,
         const XMLXPath_TokenEnum   prevToken );

static XMLXPath_ItemPtr
getNextItem( XMLXPath_ContextPtr  pCtx,
             XMLXPath_TokenListPtr itemList );

static XMLXPath_Idx_RetCode
getNumber( XMLXPath_ContextPtr   pCtx,
           SAPDB_UTF8          **p );

static SAPDB_Bool
isNamePrefixToken( XMLXPath_TokenEnum tok );

static SAPDB_Bool
isNodeTest( SAPDB_UTF8 *name, int len );

static XMLXPath_TokenEnum
isOperatorName( SAPDB_UTF8 *name, int len );

/*------------------------------------------------------------------------------*/
/* typedef only used for defining XPathOperatorNames[]                            */
typedef struct T_XPathOperatorNames
{
    SAPDB_UTF8 *op;
    XMLXPath_TokenEnum opEnum;
} T_XPathOperatorNames;

/*------------------------------------------------------------------------------*/
/* list of operators allowed within XPath expressions                           */
static const T_XPathOperatorNames XPathOperatorNames [] =
    {
        { (SAPDB_UTF8 *)"and", XMLXPath_TOK_AND },
        { (SAPDB_UTF8 *)"or",  XMLXPath_TOK_OR },
        { (SAPDB_UTF8 *)"mod", XMLXPath_TOK_MOD },
        { (SAPDB_UTF8 *)"div", XMLXPath_TOK_DIV }
    };
static const unsigned int XPathOperatorNamesDim =
    sizeof(XPathOperatorNames) / sizeof(struct T_XPathOperatorNames);

/*------------------------------------------------------------------------------*/
/* list of NodeTypes allowed within XPath expressions                           */
static const SAPDB_UTF8 *XPathNodeTypeNames[] =
    {
        (SAPDB_UTF8 *)"node",
        (SAPDB_UTF8 *)"text",
        (SAPDB_UTF8 *)"processing-instruction",
        (SAPDB_UTF8 *)"comment"
    };
static const unsigned int XPathNodeTypeNamesDim =
    sizeof(XPathNodeTypeNames) / sizeof(SAPDB_UTF8 *);

/*------------------------------------------------------------------------------*/
/* NamePrefixTokenTable                                                           */
/* a table of tokens which have the effect that the following                   */
/* name is recognized as a NCName (rather than operator name) and * is          */
/* recognized as a wildcard (rather than multiplication operator).              */
static const XMLXPath_TokenEnum NamePrefixTokenTable[] =
    {
        XMLXPath_TOK_ATSIGN,   XMLXPath_TOK_DCOLON,   XMLXPath_TOK_LPAREN,
        XMLXPath_TOK_LBRACKET, XMLXPath_TOK_OR,       XMLXPath_TOK_AND,
        XMLXPath_TOK_EQ,       XMLXPath_TOK_NEQ,      XMLXPath_TOK_LT,
        XMLXPath_TOK_GT,       XMLXPath_TOK_LE,       XMLXPath_TOK_GE,
        XMLXPath_TOK_PLUS,     XMLXPath_TOK_SUBT,     XMLXPath_TOK_MINUS,
        XMLXPath_TOK_MULT,     XMLXPath_TOK_DIV,      XMLXPath_TOK_MOD,
        XMLXPath_TOK_VERT,     XMLXPath_TOK_SLASH,    XMLXPath_TOK_DSLASH,
        XMLXPath_TOK_COMMA
    };
static const unsigned int NamePrefixTokenTableDim =
    sizeof(NamePrefixTokenTable) / sizeof(XMLXPath_TokenEnum);


/*------------------------------------------------------------------------------*/

static XMLXPath_Idx_RetCode
allocItems( XMLXPath_TokenListPtr p )
{
    XMLXPath_Idx_RetCode  rc = XMLXPath_RC_OK;

    p->tokenUsed        = 0;
    p->tokenAllocated   = 10;
    p->firstToken       = XMLXPath_Mem_Malloc(
                              sizeof( XMLXPath_Item ) * p->tokenAllocated );
    if ( !p->firstToken )
    {
        p->tokenAllocated = 0;
        rc = XMLXPath_RC_NOMEMORY;
    }
    return rc;
}

/*------------------------------------------------------------------------------*/
static XMLXPath_Idx_RetCode
checkExprToken( XMLXPath_ContextPtr  pCtx,
                SAPDB_UTF8         **p,
                XMLXPath_TokenEnum   prev,
                XMLXPath_TokenEnum  *tok )
{
    SAPDB_UTF8            utf8CharBuf[8];
    XMLXPath_Idx_RetCode  rc              = XMLXPath_RC_OK;
    SAPDB_Int             utf8CharLen     = XML_UTF8GetChar( &utf8CharBuf[0], *p );

    *tok = XMLXPath_TOK_NONE;
    if (  1 == utf8CharLen )
    {
        switch( utf8CharBuf[0] )
        {
        case '(':
            *tok = XMLXPath_TOK_LPAREN;
            ++(*p);
            break;
        case ')':
            *tok = XMLXPath_TOK_RPAREN;
            ++(*p);
            break;
        case '[':
            *tok = XMLXPath_TOK_LBRACKET;
            ++(*p);
            break;
        case ']':
            *tok = XMLXPath_TOK_RBRACKET;
            ++(*p);
            break;
        case '.':
            if ( ( 1 == XML_UTF8GetChar( &utf8CharBuf[0], ++(*p) )) &&
                    ( '.' == utf8CharBuf[0] ))
            {
                ++(*p);
                *tok = XMLXPath_TOK_DPERIOD;
            }
            else if ( XML_UTF8IsDigit( XML_UTF8CharToCode( *p ) ) )
            {
                rc = getNumber( pCtx, p );
                *tok = XMLXPath_TOK_NUMBER;
            }
            else {
                *tok = XMLXPath_TOK_PERIOD;
            };
            break;
        case '@':
            *tok = XMLXPath_TOK_ATSIGN;
            ++(*p);
            break;
        case ',':
            *tok = XMLXPath_TOK_COMMA;
            ++(*p);
            break;
        case ':':
            if ( ( 1 == XML_UTF8GetChar( &utf8CharBuf[0], (*p)+1 )) &&
                    ( ':' == utf8CharBuf[0] ))
            {
                *p = (*p) + 2;
                *tok = XMLXPath_TOK_DCOLON;
            }
            break;
        case '/':
            *tok = XMLXPath_TOK_SLASH;
            if ( ( 1 == XML_UTF8GetChar( &utf8CharBuf[0], ++(*p) )) &&
                    ( '/' == utf8CharBuf[0] ))
            {
                ++(*p);
                *tok = XMLXPath_TOK_DSLASH;
            }
            break;
        case '|':
            *tok = XMLXPath_TOK_VERT;
            ++(*p);
            break;
        case '+':
            *tok = XMLXPath_TOK_PLUS;
            ++(*p);
            break;
        case '-':
            if ( SAPDB_TRUE == isNamePrefixToken( prev ) )
                *tok = XMLXPath_TOK_MINUS;
            else
                *tok = XMLXPath_TOK_SUBT;
            ++(*p);
            break;
        case '=':
            *tok = XMLXPath_TOK_EQ;
            ++(*p);
            break;
        case '!':
            if ( ( 1 == XML_UTF8GetChar( &utf8CharBuf[0], (*p)+1 )) &&
                    ( '=' == utf8CharBuf[0] ))
            {
                *p = (*p) + 2;
                *tok = XMLXPath_TOK_NEQ;
            }
            break;
        case '<':
            *tok = XMLXPath_TOK_LT;
            if ( ( 1 == XML_UTF8GetChar( &utf8CharBuf[0], ++(*p) )) &&
                    ( '=' == utf8CharBuf[0] ))
            {
                ++(*p);
                *tok = XMLXPath_TOK_LE;
            }
            break;
        case '>':
            *tok = XMLXPath_TOK_GT;
            if ( ( 1 == XML_UTF8GetChar( &utf8CharBuf[0], ++(*p) )) &&
                    ( '=' == utf8CharBuf[0] ))
            {
                ++(*p);
                *tok = XMLXPath_TOK_GE;
            }
            break;
        case '*':
            if ( SAPDB_TRUE == isNamePrefixToken( prev ))
                *tok = XMLXPath_TOK_NAME;
            else
                *tok = XMLXPath_TOK_MULT;
            ++(*p);
            break;
        case '\"':
        case '\'':
            {
                SAPDB_UTF8   *pc;
                if ( pc = (SAPDB_UTF8*)strchr( (char*)++(*p), (char)utf8CharBuf[0] ))
                {
                    *p = ++pc;
                    *tok = XMLXPath_TOK_LITERAL;
                }
                else
                {
                    XMLXPath_Err_SetErrorParse( pCtx, XMLXPath_INFINITE_LITERAL, *p );
                    rc = XMLXPath_RC_SYNTAX;
                };
            }
            break;
        case '&':
            XMLXPath_Err_SetErrorParse( pCtx, XMLXPath_ENTITYREF_NOT_SUPP, *p );
            rc = XMLXPath_RC_SYNTAX;
            break;
        default:
            break;
        };
    };
    return rc;
}

/*------------------------------------------------------------------------------*/
static XMLXPath_Idx_RetCode
getExprToken( XMLXPath_ContextPtr   pCtx,
              SAPDB_UTF8          **xpathStr,
              XMLXPath_ItemPtr      item,
              XMLXPath_TokenEnum    prevToken )
{
    SAPDB_UTF8          utf8CharBuf[8];
    SAPDB_Int           utf8CharLen;
    XML_UnicodeCharCode chrCode;
    XMLXPath_Idx_RetCode    rc    = XMLXPath_RC_OK;

    item->startChar = *xpathStr;
    *xpathStr = XML_UTF8SkipWhiteSpaces( *xpathStr );

    if (!**xpathStr)      /* end-of-UTF-8-string ? */
    {
        item->token = XMLXPath_TOK_NULL;
    }
    else
    {
        item->startChar = *xpathStr;
        rc = checkExprToken( pCtx, xpathStr, prevToken, &item->token );
        if ( ( XMLXPath_RC_OK == rc ) && ( XMLXPath_TOK_NONE == item->token ))
        {
            utf8CharLen = XML_UTF8GetChar( &utf8CharBuf[0], *xpathStr );
            switch ( utf8CharBuf[0] )
            {
            case '$':
                {
                    /* call getName with prevToken=XMLXPath_TOK_NONE                     */
                    /* to ensure that e.g. 'and' in '$and' is treated as a name */
                    ++(*xpathStr);
                    rc = getName( pCtx, item, xpathStr, XMLXPath_TOK_NONE );
                    if ( item->token != XMLXPath_TOK_NONE )
                        item->token = XMLXPath_TOK_VAR;
                    else
                    {
                        XMLXPath_Err_SetErrorParse( pCtx, XMLXPath_BAD_VAR, *xpathStr );
                        return ( XMLXPath_RC_SYNTAX );
                    };
                };
                break;
            default:
                {
                    chrCode = XML_UTF8CharToCode( *xpathStr );
                    if ( XML_UTF8IsDigit( chrCode ))
                    {
                        rc = getNumber( pCtx, xpathStr );
                        item->token = XMLXPath_TOK_NUMBER;
                    }
                    else
                    {
                        if ( XML_UTF8IsLetter( chrCode )
                                || (**xpathStr == '_') || (**xpathStr == ':'))
                        {
                            /* the following call finds XMLXPath_TOK_NAME, XMLXPath_TOK_FUNCNAME, */
                            /* XMLXPath_TOK_AXISNAME,                                 */
                            /* as well as XMLXPath_TOK_AND etc. (based on prev token) */
                            rc = getName( pCtx, item, xpathStr, prevToken );
                        }
                        else
                        {
                            XMLXPath_Err_SetErrorParse( pCtx, XMLXPath_BAD_TOKEN, *xpathStr );
                            return ( XMLXPath_RC_SYNTAX );
                        }
                    }
                };
            };
        };
    };
    return rc;
}

/*------------------------------------------------------------------------------*/
static XMLXPath_Idx_RetCode
getExprTokenItem( XMLXPath_ContextPtr     pCtx,
                  SAPDB_UTF8            **xpathStr,
                  XMLXPath_TokenListPtr   itemList,
                  XMLXPath_TokenEnum      prevToken )
{
    XMLXPath_Idx_RetCode      rc    = XMLXPath_RC_OK;
    XMLXPath_ItemPtr      item  = getNextItem( pCtx, itemList );

    if ( !item ) {
        rc = XMLXPath_RC_NOMEMORY;
    }
    else
    {
        rc = getExprToken( pCtx, xpathStr, item, prevToken );
        if ( XMLXPath_RC_OK == rc )
        {
            item->len = *xpathStr - item->startChar;
            itemList->tokenUsed++;
        }
    }
    return rc;
}

/*------------------------------------------------------------------------------*/
static XMLXPath_Idx_RetCode
getName( XMLXPath_ContextPtr       pCtx,
         XMLXPath_ItemPtr          item,
         SAPDB_UTF8              **p,
         const XMLXPath_TokenEnum  prevToken )
{
    XML_UnicodeCharCode chrCode;
    SAPDB_UTF8         *pLoc;
    XMLXPath_Item         itemLoc;
    SAPDB_UTF8         *startPos = *p;
    XMLXPath_Idx_RetCode      rc       = XMLXPath_RC_OK;
    int                 wasColon = 0;

    chrCode = XML_UTF8CharToCode( *p );
    if (( !XML_UTF8IsLetter( chrCode )) && ( chrCode != '_' ))
    {
        item->token = XMLXPath_TOK_NONE;
        return rc;
    }

    while (( XML_UTF8IsNameChar( chrCode ) || '*' == chrCode ) && ( rc == XMLXPath_RC_OK ))
    {
        if ( chrCode == ':')
        {
            if (wasColon)
            {
                XMLXPath_Err_SetErrorParse( pCtx, XMLXPath_EXTRA_COLON, *p );
                return XMLXPath_RC_SYNTAX;
            }
            else
            {
                switch( *(*p+1) )
                {
                case ':':
                    {
                        item->token = XMLXPath_TOK_AXISNAME;
                        return rc;
                    };
                case '*':
                    {
                        item->token = XMLXPath_TOK_NAME;
                        *p = *p + 2;  /* PTS XXXXXX p += 2;*/
                        return rc;
                    };
                default:
                    wasColon = 1;
                };
            };

        }
        else if ( chrCode == '*')
        {
            if ((*p != startPos) && *(*p - 1) != ':') /* the first condition could be dropped */
            {
                item->token = XMLXPath_TOK_NAME;
                return rc;
            }
        }
        *p += XML_UTF8SingleCharLength( *p );
        chrCode = XML_UTF8CharToCode( *p );
    }

    if ( !wasColon && ( SAPDB_FALSE == isNamePrefixToken( prevToken )))
    {
        item->token = isOperatorName( startPos, (int) (*p - startPos) );
        if ( item->token != XMLXPath_TOK_NONE)
            return rc;
    };

    pLoc = *p;
    rc = getExprToken( pCtx, &pLoc, &itemLoc, XMLXPath_TOK_NAME );
    if ( XMLXPath_RC_OK == rc )
    {
        switch( itemLoc.token )
        {
        case XMLXPath_TOK_DCOLON:
            item->token = XMLXPath_TOK_AXISNAME;
            break;
        case XMLXPath_TOK_LPAREN:
            {
                if ( SAPDB_TRUE == isNodeTest( startPos, (int) (*p - startPos)))
                    item->token = XMLXPath_TOK_NODENAME;
                else
                    item->token = XMLXPath_TOK_FUNCNAME;
            };
            break;
        default:
            item->token = XMLXPath_TOK_NAME;
        };
    };

    return rc;
}

/*------------------------------------------------------------------------------*/
static XMLXPath_ItemPtr
getNextItem( XMLXPath_ContextPtr   pCtx,
             XMLXPath_TokenListPtr itemList )
{
    XMLXPath_ItemPtr       item;

    if ( itemList->tokenUsed == itemList->tokenAllocated )
    {
        item = XMLXPath_Mem_Malloc( sizeof( XMLXPath_Item )
                                    * ( itemList->tokenAllocated + 10 ) );
        if ( !item )
            return NULL;
        itemList->tokenAllocated  = itemList->tokenAllocated + 10;
        SAPDB_memcpy( (char*)item, (char*)itemList->firstToken,
                sizeof( XMLXPath_Item ) * itemList->tokenUsed );
        XMLXPath_Mem_Free( itemList->firstToken );
        itemList->firstToken = item;
    }
    item = itemList->firstToken + itemList->tokenUsed;
    item->len       = 0;
    item->startChar = NULL;
    item->token     = XMLXPath_TOK_NONE;
    return item;
}

/*------------------------------------------------------------------------------*/
static XMLXPath_Idx_RetCode
getNumber( XMLXPath_ContextPtr   pCtx,
           SAPDB_UTF8          **p )
{
    XMLXPath_Idx_RetCode  rc      = XMLXPath_RC_OK;
    int             wasDot  = 0;

    while ( XML_UTF8IsDigit( XML_UTF8CharToCode( *p )) || (**p == '.'))
    {
        if (**p == '.')
            if ( wasDot )
            {
                XMLXPath_Err_SetErrorParse( pCtx, XMLXPath_BAD_NUMBER, *p );
                rc = XMLXPath_RC_SYNTAX;
            }
            else
                wasDot = 1;
        *p += XML_UTF8SingleCharLength( *p );
    };
    return rc;
}

/*------------------------------------------------------------------------------*/
static SAPDB_Bool
isNamePrefixToken( XMLXPath_TokenEnum tok )
{
    unsigned int i;
    if ( tok == XMLXPath_TOK_NONE )
        return SAPDB_TRUE;
    for (i = 0; ( NamePrefixTokenTable[ i ] != tok ) &&
            ( i < NamePrefixTokenTableDim ); i++ );
    return ( ( NamePrefixTokenTable[i] == tok ) ? SAPDB_TRUE : SAPDB_FALSE );
}

/*------------------------------------------------------------------------------*/
static SAPDB_Bool
isNodeTest( SAPDB_UTF8 *name,
            int len )
{
    unsigned int i;
    SAPDB_UTF8 nodeName[32];
    SAPDB_memcpy( (char*)&nodeName[0], (char*)name, len );
    nodeName[ len ] = '\0';
    for ( i=0; i < XPathNodeTypeNamesDim; i++ )
    {
        if ( !strcmp( (char*)XPathNodeTypeNames[i], (char*)nodeName) )
            return SAPDB_TRUE;
    }
    return SAPDB_FALSE;
}

/*------------------------------------------------------------------------------*/
static XMLXPath_TokenEnum
isOperatorName( SAPDB_UTF8 *name,
                int len )
{
    unsigned int i;
    SAPDB_UTF8 opName[32];
    SAPDB_memcpy( (char*)&opName[0], (char*)name, len );
    opName[ len ] = '\0';
    for ( i=0; i < XPathOperatorNamesDim; i++ )
    {
        if ( !strcmp( (char*)XPathOperatorNames[i].op, (char*)opName) )
            return XPathOperatorNames[i].opEnum;
    };
    return XMLXPath_TOK_NONE;
}

/*------------------------------------------------------------------------------*/
XMLXPath_Idx_RetCode
XMLXPath_Tok_Tokenize( XMLXPath_ContextPtr    pCtx,
                       const SAPDB_UTF8      *xpathStrBase,
                       const SAPDB_UTF8      *xpathStrValue )
{
    XMLXPath_Idx_RetCode      rc            = XMLXPath_RC_OK;
    XMLXPath_TokenListPtr  itemListBase  = NULL;
    XMLXPath_TokenListPtr  itemListValue = NULL;

    pCtx->idxListInWork  = XMLXPath_Mem_Malloc(
                               sizeof( XMLXPath_IndexList ) +
                               sizeof( XMLXPath_TokenList ) +
                               strlen( (char*)xpathStrBase ) + 1 +
                               sizeof( XMLXPath_TokenList ) +
                               strlen( (char*)xpathStrValue ) + 1 );
    if ( !pCtx->idxListInWork )
    {
        rc = XMLXPath_RC_NOMEMORY;
    }
    else
    {
        XMLXPath_IndexListPtr  idxList = pCtx->idxListInWork;
        SAPDB_UTF8       *pStr;
        XMLXPath_ItemPtr    item;
        idxList->attrUsed         = SAPDB_FALSE;
        itemListBase              = ( XMLXPath_TokenListPtr )(idxList +1);
        idxList->itemListBase     = itemListBase;
        itemListValue             = itemListBase + 1;
        idxList->itemListValue    = itemListValue;
        idxList->next             = NULL;
        itemListBase->firstToken   = NULL;
        itemListBase->firstStep    = NULL;
        itemListValue->firstToken  = NULL;
        itemListValue->firstStep   = NULL;
        itemListBase->stepsUsed   = 0;
        itemListBase->stepsAllocated  = 0;
        itemListValue->stepsUsed   = 0;
        itemListValue->stepsAllocated  = 0;
        itemListBase->xpathStr    = (SAPDB_UTF8*)( itemListValue + 1 );
        strcpy( (char*)itemListBase->xpathStr, (char*)xpathStrBase );
        itemListValue->xpathStr   = itemListBase->xpathStr +
                                    strlen( (char*)itemListBase->xpathStr ) + 1;
        strcpy( (char*)itemListValue->xpathStr, (char*)xpathStrValue );

        rc = allocItems( itemListBase );
        if ( XMLXPath_RC_OK == rc )
            rc = allocItems( itemListValue );

        if ( XMLXPath_RC_OK == rc )
        {
            pStr = itemListBase->xpathStr;
            rc = getExprTokenItem( pCtx, &pStr, itemListBase, XMLXPath_TOK_NONE );
            item = &itemListBase->firstToken[ itemListBase->tokenUsed - 1 ];
        }
        while ( ( XMLXPath_RC_OK == rc ) &&
                ( item->token != XMLXPath_TOK_NULL ) && ( item->token != XMLXPath_TOK_NONE ) )
        {
            rc = getExprTokenItem( pCtx, &pStr, itemListBase, item->token );
            item = &itemListBase->firstToken[ itemListBase->tokenUsed - 1 ];
        };

        if ( XMLXPath_RC_OK == rc )
        {
            pStr = itemListValue->xpathStr;
            rc = getExprTokenItem( pCtx, &pStr, itemListValue, XMLXPath_TOK_NONE );
            item = &itemListValue->firstToken[ itemListValue->tokenUsed - 1 ];
        }
        while ( ( XMLXPath_RC_OK == rc ) &&
                ( item->token != XMLXPath_TOK_NULL ) && ( item->token != XMLXPath_TOK_NONE ) )
        {
            rc = getExprTokenItem( pCtx, &pStr, itemListValue, item->token );
            item = &itemListValue->firstToken[ itemListValue->tokenUsed - 1 ];
        };
    }
    return rc;
}

