/*!***************************************************************************

  module      : XMLXPath_Error.c

  -------------------------------------------------------------------------

  responsible : FerdiF

  special area: XML

  description : get/set last error description
 
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
 #include <stdio.h>
 #include "SAPDBCommon/SAPDB_string.h"

 #include "SAPDB/SAPDBCommon/SAPDB_Types.h"
 #include "SAPDB/XML/XPathEngine/XMLXPath_Error.h"
 #include "SAPDB/XML/XPathEngine/XMLXPath_Types.h"

/*------------------------------------------------------------------------------*/
/* definition of structs only defined and used locally                          */
/*------------------------------------------------------------------------------*/
typedef struct
{
    XMLXPath_ErrorNo   no;
    SAPDB_UTF8        *msg;
} XMLXPath_ErrorMsg;

/*------------------------------------------------------------------------------*/
/* locally defined constants                                                    */
/*------------------------------------------------------------------------------*/
static const XMLXPath_ErrorMsg ErrorMsg[] = 
{
  { XMLXPath_NOERR,                    (SAPDB_UTF8*)"" } ,
  { XMLXPath_BAD_NUMBER,               (SAPDB_UTF8*)"bad number - pos:%d xpath:'%s'" } ,
  { XMLXPath_BAD_TOKEN,                (SAPDB_UTF8*)"bad token - pos:%d xpath:'%s'" } ,
  { XMLXPath_INFINITE_LITERAL,         (SAPDB_UTF8*)"not ending literal starting at pos:%d xpath:'%s'" } ,
  { XMLXPath_BAD_VAR,                  (SAPDB_UTF8*)"bad variable - pos:%d xpath:'%s'" } ,
  { XMLXPath_EXTRA_COLON,              (SAPDB_UTF8*)"extra colon not allowed - pos:%d xpath:'%s'" } ,
  { XMLXPath_ENTITYREF_NOT_SUPP,       (SAPDB_UTF8*)"entity reference not supported - pos:%d xpath:'%s'" } ,
  { XMLXPath_NO_VALUE_PATHEXPR,        (SAPDB_UTF8*)"empty valuepath not allowed" } ,
  { XMLXPath_NOT_SUPPORTED,            (SAPDB_UTF8*)"unsupported token - pos:%d xpath:'%s'" } ,
  { XMLXPath_EXPR_NOT_TERMINATED,      (SAPDB_UTF8*)"expression not correctly terminated - pos:%d xpath:'%s'" } ,
  { XMLXPath_NO_ABSOLUTE_PATH_ALLOWED, (SAPDB_UTF8*)"no absolute valuepath allowed, if basepath is defined - pos:%d xpath:'%s'" } ,
  { XMLXPath_NO_MORE_MEMORY,           (SAPDB_UTF8*)"no more memory available" } ,
  { XMLXPath_CREATE_PARSER_FAILED,     (SAPDB_UTF8*)"parser instance couldn't be initialized" } ,
  { XMLXPath_XML_READ_HANDLER_FAILED,  (SAPDB_UTF8*)"read xml buffer handler failed" } ,
  { XMLXPath_XML_SAX_PARSER_ERROR,     (SAPDB_UTF8*)"SAX-Parser error:%d line-no:%d column-no:%d byte-index:%d %.200s" } ,
  { XMLXPath_NOT_YET_IMPLEMENTED,      (SAPDB_UTF8*)"%s not supported" } ,
  { XMLXPath_MIX_SYNC_ASYNC_READ,      (SAPDB_UTF8*)"you can't mix XMLXPath_Idx_ParseBuf calls with a call to XMLXPath_Idx_Parse" }
};

/*------------------------------------------------------------------------------*/
/* globally defined functions                                                   */
/*------------------------------------------------------------------------------*/
XMLXPath_Idx_RetCode XMLXPath_Err_GetError( void        *ptr,
                                            int         *ErrNo,
                                            SAPDB_UTF8  *ErrMsg, 
                                            int maxMsgLen ) 
{
  XMLXPath_ContextPtr p = (XMLXPath_ContextPtr)ptr;
  if ( p ) {
    int len = strlen( (char*)p->error.msg );
    *ErrNo  = p->error.rc;
    if ( maxMsgLen  < ( len - 1 ) )
      len = maxMsgLen - 1;
    SAPDB_memcpy( ErrMsg, p->error.msg, len );
    ErrMsg[len] = '\0';
  }
  else
  {
    *ErrNo = -9999;
    strcpy( (char*)ErrMsg, "context pointer is NULL" );
  }
  return XMLXPath_RC_OK;
}


/*------------------------------------------------------------------------------*/
XMLXPath_Idx_RetCode XMLXPath_Err_SetError( void *ptr,
                                            int   ErrNo )
{
  XMLXPath_ContextPtr p         = ptr;
  int i;
  
  for ( i = 0; i < ( sizeof( ErrorMsg ) / sizeof( XMLXPath_ErrorMsg )); i++ )
    if ( ErrorMsg[i].no == ErrNo ) 
      break;
    if ( i < ( sizeof( ErrorMsg ) / sizeof( XMLXPath_ErrorMsg )))
    {
      sprintf( (char*)p->error.msg, (char*)ErrorMsg[i].msg );
    }
    else
      sprintf( (char*)p->error.msg, "Unknown Error %d", ErrNo );
    return XMLXPath_RC_OK;
}

/*------------------------------------------------------------------------------*/
XMLXPath_Idx_RetCode XMLXPath_Err_SetErrorParse( void           *ptr,
                                                 int             ErrNo,
                                           const SAPDB_UTF8     *ErrPos )
{
  XMLXPath_ContextPtr p         = ptr;
  SAPDB_UTF8          *xpathStr = NULL;
  SAPDB_UTF8          *errorStr = NULL;
  int                  i;

  p->error.rc   = ErrNo;
  
  if ( ErrPos )
  {
    if ( p->idxListInWork )
    {
      if (( p->idxListInWork->itemListBase->xpathStr <= ErrPos ) && 
        ( p->idxListInWork->itemListBase->xpathStr + 
        strlen((char*)p->idxListInWork->itemListBase->xpathStr) >= ErrPos ))
      {
        xpathStr = p->idxListInWork->itemListBase->xpathStr;
      }
      if (( p->idxListInWork->itemListValue->xpathStr <= ErrPos ) && 
        ( p->idxListInWork->itemListValue->xpathStr + 
        strlen((char*)p->idxListInWork->itemListValue->xpathStr) >= ErrPos ))
      {
        xpathStr = p->idxListInWork->itemListValue->xpathStr;
      }
    }
  }

  for ( i = 0; i < ( sizeof( ErrorMsg ) / sizeof( XMLXPath_ErrorMsg )); i++ )
    if ( ErrorMsg[i].no == ErrNo ) 
      break;
  if ( i < ( sizeof( ErrorMsg ) / sizeof( XMLXPath_ErrorMsg )))
  {
    if ( ErrPos )
      sprintf( (char*)p->error.msg, (char*)ErrorMsg[i].msg, ErrPos - xpathStr + 1, xpathStr );
    else
      sprintf( (char*)p->error.msg, (char*)ErrorMsg[i].msg );
  }
  else
    sprintf( (char*)p->error.msg, "Unknown Error %d", ErrNo );
  return XMLXPath_RC_OK;
}

/*------------------------------------------------------------------------------*/
XMLXPath_Idx_RetCode
XMLXPath_Err_SetErrorSaxParser( void    *pCtx,
                            SAPDB_Int   errCode,
                            const SAPDB_UTF8       *errMsg,
                            SAPDB_Int   lineno,
                            SAPDB_Int   columnno,
                            SAPDB_Int   byteIdx )
{
  XMLXPath_ContextPtr p         = pCtx;
  int                  i;

    p->error.rc   = XMLXPath_XML_SAX_PARSER_ERROR;
    for ( i = 0; i < ( sizeof( ErrorMsg ) / sizeof( XMLXPath_ErrorMsg )); i++ )
    if ( ErrorMsg[i].no == XMLXPath_XML_SAX_PARSER_ERROR ) 
      break;
    if ( i < ( sizeof( ErrorMsg ) / sizeof( XMLXPath_ErrorMsg )))
    {
      sprintf( (char*)p->error.msg, (char*)ErrorMsg[i].msg, errCode, lineno, columnno, byteIdx, errMsg );
    }
    else
      sprintf( (char*)p->error.msg, "Unknown Error %d", XMLXPath_XML_SAX_PARSER_ERROR );
    return XMLXPath_RC_OK;
}

/*------------------------------------------------------------------------------*/
XMLXPath_Idx_RetCode
XMLXPath_Err_SetErrorString( void        *pCtx,
                             SAPDB_Int   ErrNo,
                       const SAPDB_UTF8 *str )
{
  XMLXPath_ContextPtr p         = pCtx;
  int                  i;

    p->error.rc   = ErrNo;
    for ( i = 0; i < ( sizeof( ErrorMsg ) / sizeof( XMLXPath_ErrorMsg )); i++ )
    if ( ErrorMsg[i].no == ErrNo ) 
      break;
    if ( i < ( sizeof( ErrorMsg ) / sizeof( XMLXPath_ErrorMsg )))
    {
      sprintf( (char*)p->error.msg, (char*)ErrorMsg[i].msg, str );
    }
    else
      sprintf( (char*)p->error.msg, "Unknown Error %d", XMLXPath_XML_SAX_PARSER_ERROR );
    return XMLXPath_RC_OK;

}
