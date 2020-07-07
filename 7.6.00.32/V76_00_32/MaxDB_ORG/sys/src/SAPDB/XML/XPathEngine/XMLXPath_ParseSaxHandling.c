/*!***************************************************************************

  module      : XMLXPath_ParseSaxHandling.c

  -------------------------------------------------------------------------

  responsible : FerdiF

  special area: XML

  description : code called by SAX handlers to extract index values
 
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
#include "SAPDB/XML/XPathEngine/XMLXPath_ParseSaxHandling.h"
#include "SAPDB/XML/XPathEngine/XMLXPath_Memory.h"
#include "SAPDB/XML/XPathEngine/XMLXPath_Error.h"
#include "SAPDB/XML/XML_UTF8.h"
#include "SAPDBCommon/SAPDB_string.h"

/*------------------------------------------------------------------------------*/
/* prototypes for all only locally defined and used functions                   */
/*------------------------------------------------------------------------------*/
static void
appendIdxActiveList( XMLXPath_ContextPtr  pCtx,
                    XMLXPath_PSH_ItemPtr iwp );

/*------------------------------------------------------------------------------*/
static void
endElemIdx( XMLXPath_ContextPtr   pCtx,
           const SAPDB_UTF8     *elementName,
           XMLXPath_PSH_ItemPtr  index );

/*------------------------------------------------------------------------------*/
static void
charIdx( XMLXPath_ContextPtr   pCtx,
        const SAPDB_UTF8     *s,
        SAPDB_Int             len,
        XMLXPath_PSH_ItemPtr  index );

/*------------------------------------------------------------------------------*/
static XMLXPath_StepPtr
getNextStep( XMLXPath_ContextPtr  pCtx,
            XMLXPath_PSH_ItemPtr index );

/*------------------------------------------------------------------------------*/
static XMLXPath_StepPtr
getPrevStep( XMLXPath_PSH_ItemPtr index );

/*------------------------------------------------------------------------------*/
static void
initIdxWorkItem( XMLXPath_PSH_ItemPtr p );

/*------------------------------------------------------------------------------*/
static void
releaseIdxWorkItemList( XMLXPath_ContextPtr pCtx );

/*------------------------------------------------------------------------------*/
static void
removeIdxActiveList( XMLXPath_ContextPtr  pCtx,
                    XMLXPath_PSH_ItemPtr iwp );
/*------------------------------------------------------------------------------*/
static void
startElemIdx( XMLXPath_ContextPtr      pCtx,
             const SAPDB_UTF8        *elementName,
             const SAPDB_UTF8       **atts,
             XMLXPath_PSH_ItemPtr     index,
             XMLXPath_PSH_ItemPtr    *susp );

/*------------------------------------------------------------------------------*/
static SAPDB_Bool
startElemIdxStep( XMLXPath_ContextPtr  pCtx,
                 const SAPDB_UTF8*    elementName,
                 const SAPDB_UTF8**   atts,
                 XMLXPath_PSH_ItemPtr index,
                 XMLXPath_StepPtr     actualStep );

/*------------------------------------------------------------------------------*/
/* locally defined static functions                                             */
/*------------------------------------------------------------------------------*/
static void
appendIdxActiveList( XMLXPath_ContextPtr pCtx,
                    XMLXPath_PSH_ItemPtr iwp )
{
  if ( NULL == pCtx->idxActiveList )
  {
    pCtx->idxActiveList = iwp;
  }
  else
  {
    XMLXPath_PSH_ItemPtr prv = pCtx->idxActiveList;
    XMLXPath_PSH_ItemPtr nxt = prv->next;
    while ( nxt )
    {
      prv = nxt;
      nxt = nxt->next;
    }
    prv->next = iwp;
    iwp->prev = prv;
  }
}

/*------------------------------------------------------------------------------*/
static void
charIdx( XMLXPath_ContextPtr   pCtx,
        const SAPDB_UTF8     *s,
        SAPDB_Int             len,
        XMLXPath_PSH_ItemPtr  index )
{
  if (( index->foundXPath ) &&
    ( pCtx->actualTreeLevel == index->actualLevel ))
  {
    XMLXPath_StepPtr  actualStep;   
    SAPDB_UTF8   *pwc;
    SAPDB_UTF8   *pbuf = &pCtx->tmpWorkBuf[0];
    
    /* handle '//text' value path */
    if ( 0 == index->actualStepPos )							
      actualStep = index->idxList->itemListValue->firstStep;
    else
      /*XMLXPath_StepPtr*/  actualStep = getPrevStep( index );
      
      if (( XMLXPath_NodeType_Attribute != actualStep->nodeType ) &&
        (!( index->foundFirstValue ) ||
        ( index->idxList->itemListValue->category == XMLXPath_Idx_set )))
      {
        index->idxList->textUsed = SAPDB_TRUE;
        if ( len >= sizeof( pCtx->tmpWorkBuf ) )
          pbuf = XMLXPath_Mem_Malloc( len + 1 );
        SAPDB_memcpy( pbuf, s, len );
        *(pbuf + len) = '\0';
        pwc = XML_UTF8NormalizeValue( pbuf );
        if (( pwc - pbuf ) < len )
        {
          int          insWsFlag = 0;
          SAPDB_UTF8   *pnewbuf;
          if ( index->indexValueStr )
          {
            pnewbuf = XMLXPath_Mem_Malloc( strlen((char*)index->indexValueStr) +
              strlen((char*)pwc) + 2 );
            strcpy( (char*)pnewbuf, (char*)index->indexValueStr );
            XMLXPath_Mem_Free( index->indexValueStr );
            insWsFlag = 1;
          }
          else
          {
            pnewbuf = XMLXPath_Mem_Malloc( strlen((char*)pwc) + 2 );
            *pnewbuf = '\0';
          }
          if ( insWsFlag && ( pwc != pbuf ))
            strcat( (char*)pnewbuf, " " );        /* insert 1 whitespace */
          strcat( (char*)pnewbuf, (char*)pwc );
          index->indexValueStr = pnewbuf;
        }
        if ( pbuf && ( len >= sizeof( pCtx->tmpWorkBuf )))
          XMLXPath_Mem_Free( pbuf );
      };
  }
}

/*------------------------------------------------------------------------------*/
static void endElemIdx( XMLXPath_ContextPtr   pCtx,
                       const SAPDB_UTF8     *elementName,
                       XMLXPath_PSH_ItemPtr  index )
{
  SAPDB_Int         len;
  XMLXPath_StepPtr  actualStep;
  XMLXPath_NodeType nodeType = XMLXPath_NodeType_Element;
  int				  nameNodeType = 0;
  
  if ( pCtx->actualTreeLevel == index->actualLevel )
  {
    /* handle '//text' value path */
    if (( 0 == index->actualStepPos ) && ( 0 == index->idxList->itemListBase->stepsUsed ))							
      actualStep = index->idxList->itemListValue->firstStep;
    else
      actualStep = getPrevStep( index );
    
    /* check if '....//text()' has to be handled */
    if (( XMLXPath_Step_Relative == actualStep->stepType ))		
      nameNodeType = 1;											
    else															
      while (( XMLXPath_NodeType_Attribute == actualStep->nodeType ) ||
        ( XMLXPath_NodeType_Nodename == actualStep->nodeType ) )
      {
        index->actualStepPos--;
        actualStep = getPrevStep( index );
      }
      
      len = strlen( (char*)elementName );
      if ( nameNodeType ||												
        (( len == actualStep->nameLen &&
        !memcmp(actualStep->name, elementName, len)) ||
        (('*' == *( actualStep->name + actualStep->nameLen - 1) ) &&	
        (( actualStep->nameLen == 1 ) ||								
        (( actualStep->nameLen <= len ) &&								
        !memcmp(actualStep->name, elementName, actualStep->nameLen - 1 )))))) 									
      {
        if ( index->foundXPath )
        {
          /* set foundFirstValue Flag for all index descriptions of one XPath query */
          /* if belongs to the same index (index created because of decendent axis )*/
          /* and if valuePath of index is active                                    */
          /* then mark index as 'foundFirstValue'                                   */
          /* Flag is reset, when index switches back to basePath                    */
          XMLXPath_PSH_ItemPtr idxCurr = index;
          while ( idxCurr->prev )                         
            idxCurr = idxCurr->prev;
          while ( idxCurr )
          {
            if ( idxCurr->idxList == index->idxList ) 
            {
              if ( idxCurr->isValueStep ) 
                idxCurr->foundFirstValue = SAPDB_TRUE;
            }
            idxCurr = idxCurr->next;
          }
          
          /* if a value for the element was found, call index handler */
          if ( index->indexValueStr )         
          {
            index->idxList->indexHandler( pCtx->userData, index->idxList->userData,
              index->indexValueStr );
            
            XMLXPath_Mem_Free( index->indexValueStr );
            index->indexValueStr = NULL;
            index->foundXPath = SAPDB_FALSE;
          }
        }
        
        switch ( actualStep->stepType )
        {
        case XMLXPath_Step_Absolute:
          if ( pCtx->actualTreeLevel == index->actualLevel )
          {
            XMLXPath_PSH_ItemPtr idxCurr = index;
            index->actualLevel--;
            index->actualStepPos--;
            if (( 0 == index->actualStepPos ) &&
              ( index->isValueStep ))
            {
              index->isValueStep   = SAPDB_FALSE;
              index->actualStepPos = index->idxList->itemListBase->stepsUsed;
            }
            /* reset foundFirstValue Flag for all index descriptions of one XPath query */
            if (( !index->isValueStep ) && ( index->idxList->itemListBase->stepsUsed > index->actualStepPos ))
              index->foundFirstValue = SAPDB_FALSE;           
          }
          break;
        case XMLXPath_Step_Relative:
          index->actualStepPos--;
          if ( index->actualLevel == index->removeLevel )
            removeIdxActiveList( pCtx, index );
          else
            index->actualLevel--;
          break;
        default:
          break;
        }
      }
  }
}
                       
/*------------------------------------------------------------------------------*/
static  XMLXPath_StepPtr
getNextStep( XMLXPath_ContextPtr   pCtx,
            XMLXPath_PSH_ItemPtr  index )
{
  if ( ! index->isValueStep )
  {
    if ( index->actualStepPos == index->idxList->itemListBase->stepsUsed )
    {
      index->isValueStep    = SAPDB_TRUE;
      index->actualStepPos  = 0;
      return ( index->idxList->itemListValue->firstStep );
    }
    else
      return ( index->idxList->itemListBase->firstStep +
      index->actualStepPos );
  }
  else
  {
    if ( index->actualStepPos < index->idxList->itemListValue->stepsUsed )
      return ( index->idxList->itemListValue->firstStep +
      index->actualStepPos );
    else
      if ( XMLXPath_RC_OK == pCtx->retCode )
      {
        XMLXPath_Err_SetErrorString( pCtx, XMLXPath_NOT_YET_IMPLEMENTED,
          (SAPDB_UTF8*)"leaf element node in XPath expression" );
        pCtx->retCode = XMLXPath_RC_FAILED;
      }
  }
  return NULL;
}

/*------------------------------------------------------------------------------*/
static  XMLXPath_StepPtr
getPrevStep( XMLXPath_PSH_ItemPtr index )
{
  if ( ! index->isValueStep )
    return ( index->idxList->itemListBase->firstStep +
    index->actualStepPos - 1 );
  else
  {
    if ( 0 == index->actualStepPos )
    {
      index->actualStepPos =
        index->idxList->itemListBase->stepsUsed;
      index->isValueStep = SAPDB_FALSE;
      return ( index->idxList->itemListBase->firstStep +
        index->actualStepPos - 1 );
    }
    else
      return ( index->idxList->itemListValue->firstStep +
      index->actualStepPos - 1 );
  }
}

/*------------------------------------------------------------------------------*/
static void
removeIdxActiveList( XMLXPath_ContextPtr pCtx,
                    XMLXPath_PSH_ItemPtr iwp )
{
  if ( iwp->prev )
    iwp->prev->next = iwp->next;
  if ( iwp->next )
    iwp->next->prev = iwp->prev;
  XMLXPath_Mem_Free( iwp );
}

/*------------------------------------------------------------------------------*/
static void
initIdxWorkItem( XMLXPath_PSH_ItemPtr p )
{
  p->actualLevel        = 0;
  p->actualStepPos      = 0;
  p->indexValueStr      = NULL;
  p->idxList            = NULL;
  p->next               = NULL;
  p->prev               = NULL;
  p->isValueStep        = SAPDB_FALSE;
  p->foundXPath         = SAPDB_FALSE;
  p->foundFirstValue    = SAPDB_FALSE;
  p->removeLevel        = 0;
}

/*------------------------------------------------------------------------------*/
static void
releaseIdxWorkItemList( XMLXPath_ContextPtr pCtx )
{
  XMLXPath_Idx_RetCode      rc    = XMLXPath_RC_OK;
  XMLXPath_PSH_ItemPtr      curr  = pCtx->idxActiveList;
  XMLXPath_PSH_ItemPtr      next  = curr->next;
  
  while ( curr )
  {
    XMLXPath_Mem_Free( curr );
    curr = next;
    if ( curr )
      next = curr->next;
  }
  pCtx->idxActiveList = NULL;
}

/*------------------------------------------------------------------------------*/
static  SAPDB_Bool 
checkAttributeNodes( XMLXPath_ContextPtr  pCtx,
                    XMLXPath_PSH_ItemPtr index,
                    XMLXPath_StepPtr     actualStep,
                    const SAPDB_UTF8**   atts )
{
  if ( ! index->idxList->attrUsed )
  {
    int i;
    int starLoop = 0;
    SAPDB_UTF8 *pc;
    
    if ( 1 == actualStep->nameLen && '*' == *actualStep->name )
      starLoop = 1;
    SAPDB_memcpy( &pCtx->tmpWorkBuf[0], actualStep->name, actualStep->nameLen );
    pCtx->tmpWorkBuf[ actualStep->nameLen ] = '\0';
    for ( i=0; atts[i]; i = i+2 )
    {
      if ( ( starLoop &&
        !strstr( (char*)atts[i], "xmlns:" ) ) ||
        !strcmp((char*)&pCtx->tmpWorkBuf[0],(char*)atts[i] ))
      {
        if ( SAPDB_FALSE == index->foundXPath )
        {
          XMLXPath_PSH_ItemPtr idxCurr = index;
          while ( idxCurr->prev )                         
            idxCurr = idxCurr->prev;
          while ( idxCurr )
          {
            if ( idxCurr->idxList == index->idxList ) 
            {
              if ( idxCurr->isValueStep ) 
                idxCurr->foundFirstValue = SAPDB_TRUE;
            }
            idxCurr = idxCurr->next;
          }
          index->idxList->attrUsed  = SAPDB_TRUE;
          index->foundXPath  = SAPDB_TRUE;
          index->actualStepPos++;
        }
        strcpy( (char*)&pCtx->tmpWorkBuf[0], (char*)atts[i+1] );
        pc = XML_UTF8NormalizeValue( &pCtx->tmpWorkBuf[0] );
        index->idxList->indexHandler( pCtx->userData, index->idxList->userData, pc );
        if ( !starLoop  ||
          ( XMLXPath_Idx_value ==
          index->idxList->itemListValue->category ))
          break;
      }
    }
  }
  return ( SAPDB_TRUE == index->foundXPath ? SAPDB_TRUE : SAPDB_FALSE );
}

/*------------------------------------------------------------------------------*/
static SAPDB_Bool
startElemIdxStep( XMLXPath_ContextPtr  pCtx,
                 const SAPDB_UTF8*    elementName,
                 const SAPDB_UTF8**   atts,
                 XMLXPath_PSH_ItemPtr index,
                 XMLXPath_StepPtr     actualStep )
{
  SAPDB_Bool  brc = SAPDB_TRUE;
  
  index->actualLevel = pCtx->actualTreeLevel;
  if ( actualStep->predicate )
  {
    int i;
    SAPDB_memcpy( (char*)&pCtx->tmpWorkBuf[0], (char*)actualStep->predicate->name,
      actualStep->predicate->nameLen );
    pCtx->tmpWorkBuf[ actualStep->predicate->nameLen ] = '\0';
    brc = SAPDB_FALSE;
    for ( i=0; atts[i]; i = i+2 )
    {
      /* 'attribute::*'                                                  */
      /* if ANY of the attribute-values compares to the provided literal */
      /* the predicate is considered to match                            */
      if (( 1 == actualStep->predicate->nameLen &&
        '*' == *actualStep->predicate->name ) ||
        !strcmp((char*)&pCtx->tmpWorkBuf[0],(char*)atts[i] ))
      {
        SAPDB_memcpy( &pCtx->tmpWorkBuf[0], actualStep->predicate->value,
          actualStep->predicate->valueLen );
        pCtx->tmpWorkBuf[ actualStep->predicate->valueLen ] = '\0';
        if ((( XMLXPath_StepOp_EQ == actualStep->predicate->selectOP ) &&
          ( !strcmp((char*)&pCtx->tmpWorkBuf[0],(char*)atts[i+1] ))) ||
          (( XMLXPath_StepOp_NEQ == actualStep->predicate->selectOP ) &&
          ( strcmp((char*)&pCtx->tmpWorkBuf[0],(char*)atts[i+1] ))))
        {
          brc = SAPDB_TRUE;
          break;
        }
      }
    }
  }
  
  index->actualStepPos++;
  actualStep = getNextStep( pCtx, index );
  if (( SAPDB_TRUE == brc ) && ( SAPDB_TRUE == index->isValueStep ) &&
    ( index->actualStepPos == index->idxList->itemListValue->stepsUsed - 1 ))
  {
    if ( XMLXPath_NodeType_Attribute == actualStep->nodeType )
    {
      brc = checkAttributeNodes( pCtx, index, actualStep, atts );
    }
    else if ( XMLXPath_NodeType_Nodename == actualStep->nodeType )
    {
      index->foundXPath  = SAPDB_TRUE;
      index->actualStepPos++;
    }
  }
  return brc;
}

/*------------------------------------------------------------------------------*/
static void
startElemIdx( XMLXPath_ContextPtr    pCtx,
             const SAPDB_UTF8      *elementName,
             const SAPDB_UTF8     **atts,
             XMLXPath_PSH_ItemPtr   index,
             XMLXPath_PSH_ItemPtr  *susp )
{
  int handleNameNotetype = 0;		
  if ( !index->foundFirstValue ||
    index->idxList->itemListValue->category == XMLXPath_Idx_set )
  {
    /* character data is terminated in 'mixed mode' by the beginning of a new */
    /* element therefore pass gathered character data to index handler        */
    if ( index->foundXPath )
    {
      if ( index->indexValueStr )
      {
        XMLXPath_PSH_ItemPtr idxCurr = index;
        while ( idxCurr->prev )
          idxCurr = idxCurr->prev;
        while ( idxCurr )
        {
          if ( idxCurr->idxList == index->idxList )
          {
            idxCurr->foundFirstValue = SAPDB_TRUE;
          }
          idxCurr = idxCurr->next;
        }
        
        index->idxList->indexHandler( pCtx->userData, index->idxList->userData,
          index->indexValueStr );
        XMLXPath_Mem_Free( index->indexValueStr );
        index->indexValueStr = NULL;
      }
    }
    
    if ( !( index->isValueStep &&
      ( index->actualStepPos == index->idxList->itemListValue->stepsUsed )) )
    {
      XMLXPath_StepPtr  actualStep = getNextStep( pCtx, index );
      
      if ( XMLXPath_RC_OK == pCtx->retCode )
      {
        SAPDB_Int         len = strlen( (char*)elementName );
        
        switch ( actualStep->nodeType )
        {
        case XMLXPath_NodeType_Attribute:
          if ((( XMLXPath_Step_Absolute == actualStep->stepType ) &&
            ( pCtx->actualTreeLevel == index->actualLevel + 1 )) ||  
            ( XMLXPath_Step_Relative == actualStep->stepType ))
          {
            SAPDB_Bool  stepAllocated   = SAPDB_FALSE;  /* variable used for desendant axis */
            SAPDB_Int   s_actualLevel   = index->actualLevel;
            SAPDB_Int   s_actualStepPos = index->actualStepPos;
            SAPDB_Bool  s_isValueStep   = index->isValueStep;
            XMLXPath_PSH_ItemPtr index_org = index;   
            XMLXPath_PSH_ItemPtr iwp    = NULL;
            if ( XMLXPath_Step_Relative == actualStep->stepType )
            {
              iwp = XMLXPath_Mem_Malloc( sizeof(XMLXPath_PSH_Item) );
              if ( !iwp )
              {
                XMLXPath_Err_SetError( pCtx, XMLXPath_NO_MORE_MEMORY );
                pCtx->retCode = XMLXPath_RC_NOMEMORY;
              }
              else
              {
                stepAllocated = SAPDB_TRUE;
                *iwp             = *index;
                iwp->removeLevel = pCtx->actualTreeLevel;
                index = iwp;
              }
            }
            
            if ( SAPDB_FALSE == checkAttributeNodes( pCtx, index, actualStep, atts ) )
            {
              if ( XMLXPath_Step_Relative == actualStep->stepType )
                XMLXPath_Mem_Free( iwp );
              else
              {
                index->actualLevel    = s_actualLevel;
                index->actualStepPos  = s_actualStepPos;
                index->isValueStep    = s_isValueStep;
              }
            }
            else
            {
              if ( SAPDB_TRUE == stepAllocated )
              {
                if ( index_org->isValueStep ) 
                  index_org->foundFirstValue = index->foundFirstValue;
                iwp->next       = *susp;          /* chain iwp to supend */
                if ( *susp )
                  (*susp)->prev = iwp;
                *susp           = iwp;
                iwp->prev       = NULL;
              }
            }
          }
          break;
        case XMLXPath_NodeType_Element:
          {
            if (( len == actualStep->nameLen &&
              !( XMLXPath_Step_Absolute == actualStep->stepType &&
              !( pCtx->actualTreeLevel == index->actualLevel + 1 )) &&
              !memcmp(actualStep->name, elementName, len)) ||
              (('*' == *( actualStep->name + actualStep->nameLen - 1) ) &&	
              (( actualStep->nameLen == 1 ) ||								
              (( actualStep->nameLen <= len ) &&								
              !memcmp(actualStep->name, elementName, actualStep->nameLen - 1 ))))) 
            {
              SAPDB_Bool  stepAllocated = SAPDB_FALSE;  /* variable used for desendant axis */
              SAPDB_Int   s_actualLevel   = index->actualLevel;
              SAPDB_Int   s_actualStepPos = index->actualStepPos;
              SAPDB_Bool  s_isValueStep   = index->isValueStep;
              XMLXPath_PSH_ItemPtr index_org = index;   
              XMLXPath_PSH_ItemPtr iwp    = NULL;
              
              if ( XMLXPath_Step_Relative == actualStep->stepType )
              {
                iwp = XMLXPath_Mem_Malloc( sizeof(XMLXPath_PSH_Item) );
                if ( !iwp )
                {
                  XMLXPath_Err_SetError( pCtx, XMLXPath_NO_MORE_MEMORY );
                  pCtx->retCode = XMLXPath_RC_NOMEMORY;
                }
                else
                {
                  stepAllocated = SAPDB_TRUE;
                  *iwp             = *index;
                  iwp->removeLevel = pCtx->actualTreeLevel;
                  index = iwp;
                }
              }
              
              if ( ( XMLXPath_RC_OK == pCtx->retCode ) && ( SAPDB_FALSE ==
                startElemIdxStep( pCtx, elementName, atts, index, actualStep )))
              {
                if ( XMLXPath_Step_Relative == actualStep->stepType )
                  XMLXPath_Mem_Free( iwp );
                else
                {
                  index->actualLevel    = s_actualLevel;
                  index->actualStepPos  = s_actualStepPos;
                  index->isValueStep    = s_isValueStep;
                }
              }
              else
              {
                if ( SAPDB_TRUE == stepAllocated )
                {
                  if ( index_org->isValueStep ) 
                    index_org->foundFirstValue = index->foundFirstValue;
                  iwp->next       = *susp;          /* chain iwp to supend */
                  if ( *susp )
                    (*susp)->prev = iwp;
                  *susp           = iwp;
                  iwp->prev       = NULL;
                }
              }
            }
          }
          break;
        case XMLXPath_NodeType_Nodename:
          index->foundXPath  = SAPDB_TRUE;                      
          if ( actualStep->stepType == XMLXPath_Step_Relative ) 
            handleNameNotetype = 1;		                      
          break;
                }
            }
            else
            {
              XMLXPath_Err_SetErrorString( pCtx, XMLXPath_NOT_YET_IMPLEMENTED,
                (SAPDB_UTF8*)"dependant axis" );
              pCtx->retCode = XMLXPath_RC_FAILED;
            }
        }
        else
        {
          XMLXPath_StepPtr  actualStep = index->idxList->itemListValue->firstStep + index->actualStepPos - 1;
          if ( actualStep->stepType == XMLXPath_Step_Relative )
            handleNameNotetype = 1;
        }
        
        if ( handleNameNotetype )
        {
          /* if valuePath ends with '//text()' a new XMLXPath_PSH_Item is created for any sublevel */
          if ( index->isValueStep )
          {
            XMLXPath_StepPtr  actualStep = index->idxList->itemListValue->firstStep + index->actualStepPos - 1;
            index->foundXPath  = SAPDB_TRUE; 
            
            if ( index->removeLevel < pCtx->actualTreeLevel )
            {
              XMLXPath_PSH_ItemPtr iwp    = NULL;
              iwp = XMLXPath_Mem_Malloc( sizeof(XMLXPath_PSH_Item) );
              if ( !iwp )
              {
                XMLXPath_Err_SetError( pCtx, XMLXPath_NO_MORE_MEMORY );
                pCtx->retCode = XMLXPath_RC_NOMEMORY;
              }
              else
              {
                *iwp             = *index;
                iwp->removeLevel = pCtx->actualTreeLevel;
                iwp->actualLevel = pCtx->actualTreeLevel;
                iwp->next       = *susp;          
                if ( *susp )
                  (*susp)->prev = iwp;
                *susp           = iwp;
                iwp->prev       = NULL;
              }
            }
          }
        }
    }
}

/*------------------------------------------------------------------------------*/
/* globally defined functions                                                   */
/*------------------------------------------------------------------------------*/
XMLXPath_Idx_RetCode
XMLPath_PSH_CreateIdxWorkItemList( XMLXPath_ContextPtr pCtx )
{
  XMLXPath_Idx_RetCode      rc    = XMLXPath_RC_OK;
  XMLXPath_IndexListPtr     curr  = pCtx->idxList;
  
  while ( curr && ( XMLXPath_RC_OK == rc ))
  {
    XMLXPath_PSH_ItemPtr    iwp;
    
    iwp = XMLXPath_Mem_Malloc( sizeof(XMLXPath_PSH_Item) );
    if ( !iwp )
    {
      XMLXPath_Err_SetError( pCtx, XMLXPath_NO_MORE_MEMORY );
      rc = XMLXPath_RC_NOMEMORY;
    }
    else
    {
      initIdxWorkItem( iwp );
      iwp->idxList = curr;
      appendIdxActiveList( pCtx, iwp );
    }
    curr = curr->next;
  }
  
  return rc;
}

/*------------------------------------------------------------------------------*/
void
XMLXPath_PSH_CharacterHandler( XMLXPath_ContextPtr  pCtx,
                              const SAPDB_UTF8    *s,
                              SAPDB_Int            len )
{
  if ( XMLXPath_RC_OK == pCtx->retCode )
  {
    XMLXPath_PSH_ItemPtr   indexToSkip = NULL;
    XMLXPath_PSH_ItemPtr   curr  = pCtx->idxActiveList;
    
    XMLXPath_IndexListPtr    idxCurr  =  pCtx->idxList;
    /* set all indexes to allow found text-values once */
    /* if textUsed is true, the path is skipped        */
    /* because it´s a redundant path to the result     */
    /* textUsed is set to true within                  */
    /* handling in function: charIdx                   */
    while ( idxCurr )
    {
      idxCurr->textUsed = SAPDB_FALSE;
      idxCurr           = idxCurr->next;
    }
    
    while ( curr )
    {
      if ( ! curr->idxList->textUsed )
        charIdx( pCtx, s, len, curr );
      curr = curr->next;
    }
    if ( indexToSkip )
      appendIdxActiveList( pCtx, indexToSkip );
  }
}

/*------------------------------------------------------------------------------*/
void
XMLXPath_PSH_EndElementHandler( XMLXPath_ContextPtr  pCtx,
                               const SAPDB_UTF8    *elementName )
{
  if ( XMLXPath_RC_OK == pCtx->retCode )
  {
    XMLXPath_PSH_ItemPtr            curr  = pCtx->idxActiveList;
    XMLXPath_PSH_ItemPtr            next  = NULL;
    XMLXPath_PSH_ItemPtr            susp  = NULL;
    
    while ( curr )
    {
      next = curr->next;
      endElemIdx( pCtx, elementName, curr );
      curr = next;
    }
  }
}

/*------------------------------------------------------------------------------*/
void
XMLXPath_PSH_ReleaseResources( XMLXPath_ContextPtr pCtx )
{
  releaseIdxWorkItemList( pCtx );
}

/*------------------------------------------------------------------------------*/
void
XMLXPath_PSH_StartElementHandler( XMLXPath_ContextPtr pCtx,
                                 const SAPDB_UTF8*   elementName,
                                 const SAPDB_UTF8**  atts )
{ 
  if ( XMLXPath_RC_OK == pCtx->retCode )
  {
    XMLXPath_IndexListPtr    idxCurr  =  pCtx->idxList;
    XMLXPath_PSH_ItemPtr     curr  = pCtx->idxActiveList;
    XMLXPath_PSH_ItemPtr     susp  = NULL;
    
    /* set all indexes to allow found attributes once */
    /* if attrUsed is true, the path is skipped       */
    /* because it´s a redundant path to the result    */
    /* attrUsed is set to true within attribute       */
    /* handling in function: startElemIdxStep         */
    while ( idxCurr )
    {
      idxCurr->attrUsed = SAPDB_FALSE;
      idxCurr           = idxCurr->next;
    }
    
    /* loop over all active index descriptions */
    while ( curr )
    {
      startElemIdx( pCtx, elementName, atts, curr, &susp );
      curr = curr->next;
    }
    if ( susp )
      appendIdxActiveList( pCtx, susp );
  }
}
