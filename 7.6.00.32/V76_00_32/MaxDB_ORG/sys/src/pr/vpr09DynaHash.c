/*!================================================================

  module:       vpr09DynaDesc.c

 -------------------------------------------------------------------

  responsible:  BurkhardD

  special area: Verwaltung Dynamischer Descriptoren

  description:  
                Ein Dynamischer Descriptor ist Liste von Zeigern auf Elemente  
		einer benutzerdefinierbare Datenstruktur. 
		Die Descriptoren, Zeiger innerhalb einer dynamische vewalteten 
		Liste, koennen mit Hilfe der in diesem Modul implementierten 
		Funktionen verwaltet werden. Wobei neu angeforderte 
		Descriptoren  sich aus einem Descriptorpool versorgen 
		dieser Pool wird automatisch bei der Anforderung eines 
		Descriptor angelegt. Geloeschte Descriptoren wandern wieder 
		in diesen Pool. Ein weiteres Merkmal der Dynamischen 
		Descriptoren ist die schnelle Suche und Sortierbarkeit der 
		Elemente des Descriptors mittels benutzerdefinierbarer 
		Funktionen.

  see also:

  -------------------------------------------------------------------------

  copyright:    Copyright by SAP AG, 1998



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


 ===================================================================*/


/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include <stdlib.h>
#include "gpr00.h"
#include "vpr09DynaHash.h"
#include "gpr03m.h"

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

/*==================================================================*
 *  EXTERNAL PROTOTYPES                                             *
 *==================================================================*/
/*==================================================================*
 *  FUNCTION PROTOTYPES                                             *
 *==================================================================*/

static void *pr09malloc(size_t len, char *szStr);
static void *pr09realloc(void *, size_t len, char *szStr);
static void  pr09free(void *, char *szStr);
static void pr09addtoFreeLst    (void * HItem, tpr09DynamicHashDescriptor * dynaDesc);
static void* pr09getnewHashItem (tpr09DynamicHashDescriptor * dynaDesc);

/*==================================================================*
 *  CODE                                                            *
 *==================================================================*/

/*!
  Function: pr09InitDescriptor 

  description: Initialisiert einen Dynamischen Descriptor

  arguments:
      pDesc        [in]  Zeiger auf einen Dynamischen Descriptor
      cbIncCount   [in]  Anzahl der Descriptoren um die der Descriptor Pool 
                         erweitert wird, wenn keine Descriptoren mehr frei sind.
      cbSize       [in]  Anzahl Bytes eines Elements des Descriptors
 */
static void pr09InitDescriptor(tpr09DynamicHashDescriptor *pDesc, 
                               int cbIncCount, 
                               int cbSize, 
                               tpr09CompareMethod compMeth, 
                               tpr09HashFunc hashFunc, 
                               tpr09ExtractKey KeyFunc,
                               tpr09PrintHashItem PrintFunc)
{
  tpr09_HashTableError myError;
  pDesc->cbIncCount = cbIncCount;
  pDesc->cbSize     = cbSize;
  pDesc->chunkList  = NULL;
  pDesc->freeList   = NULL;
  myError = pr09HTCreateHashTable(&(pDesc->hTab),
                                  1500,
                                  0,
                                  rehash_enable,
                                  compMeth,
                                  hashFunc,
                                  KeyFunc,
                                  PrintFunc);
}

/*!
  Function: pr09NewDescriptor 

  description: Legt einen Dynamischen Descriptor an.

  arguments:
      cbIncCount   [in]  Anzahl der Descriptoren um die der Descriptor Pool 
                         erweitert wird, wenn keine Descriptoren mehr frei sind.
      cbSize       [in]  Anzahl Bytes eines Elements des Descriptors

  return value: Zeiger auf einen Dynamischen Descriptor
 */
tpr09DynamicHashDescriptor *pr09NewHashDescriptor(int cbIncCount, 
                                                  int cbSize, 
                                                  tpr09CompareMethod compMeth, 
                                                  tpr09HashFunc hashFunc, 
                                                  tpr09ExtractKey KeyFunc,
                                                  tpr09PrintHashItem PrintFunc)
{
  tpr09DynamicHashDescriptor *p = pr09malloc((size_t)sizeof(tpr09DynamicHashDescriptor), "pr09Desc");
  if (p)
    pr09InitDescriptor(p, cbIncCount, cbSize, compMeth, hashFunc, KeyFunc, PrintFunc);
  return p;
}


/*!
  Function: pr09DeleteDescriptor 

  description: Gibt alle Elemente des Dynamischen Descriptors und den Descriptor
               selbst frei.

  arguments:
      pDesc        [in]  Zeiger auf einen Dynamischen Descriptor

  return value: none
 */

void pr09DeleteHashDescriptor(tpr09DynamicHashDescriptor *pDesc)
{
  tpr09DynaHashListItem* chunk;
  while (pDesc->chunkList!=NULL){
    chunk = pDesc->chunkList;
    pDesc->chunkList = pDesc->chunkList->next_p;
    pr09free(chunk->data_p, "tpr09DynamicHashDescriptor->Chunk");
    pr09free(chunk,         "tpr09DynamicHashDescriptor->Item");
  }

  pDesc->freeList=NULL;  
  pr09HTDeleteHashTable(&(pDesc->hTab));
  pr09free(pDesc, "pr09Desc");
}

/*!
  Function: pr09GetNewHashItem 

  description: Erweitert die Liste der Elemente des Descriptors um ein Element.
               Das neue Element wird aus dem Pool geholt oder falls keine 
	       Elementen mehr im Pool varhanden werden zuerst neue hinzugefuegt

  arguments:
      pDesc        [in]  Zeiger auf einen Dynamischen Descriptor

  return value: Zeiger auf das der Liste neu hinzugefeugte Element oder NULL im Fehlerfall.
 */
void *pr09GetNewHashItem(tpr09DynamicHashDescriptor *pDesc)
{
  return pr09getnewHashItem (pDesc);
}

tpr09_HashTableError pr09AddHashItemToHash(tpr09DynamicHashDescriptor *pDesc, tpr09HashKey HashKey, tsp00_Uint4 cbHashKeyLen, tpr09HashValue *HashValue)
{
  tpr09HashIndex  hashIndex;
  return pr09HTInsertHashItemByValue (&(pDesc->hTab), HashKey, cbHashKeyLen, HashValue, &hashIndex);
}


/*!
  Function: pr09FindHashItem 

  description: ??? 

 */
void *pr09FindHashItem(tpr09DynamicHashDescriptor *pDesc, tpr09HashKey HashKey, tsp00_Uint4 cbHashKeyLen)
{
  return pr09HTGetHashItemByKey (&(pDesc->hTab), HashKey, cbHashKeyLen);  
}


void pr09DeleteHashItem(tpr09DynamicHashDescriptor *pDesc, tpr09HashKey HashKey, tsp00_Uint4 cbHashKeyLen)
{  
   tpr09HashValue HashValue;
   tpr09HashIndex myHashIndex = pDesc->hTab.HashFunc(HashKey, cbHashKeyLen, pDesc->hTab.cbMaxCapacity);
   if (NULL != (HashValue=pr09HTGetHashItemByIndex (&(pDesc->hTab), myHashIndex, HashKey, cbHashKeyLen))){
     pr09HTRemoveHashItemByIndex  (&(pDesc->hTab), myHashIndex, HashKey, cbHashKeyLen);
     memset(HashValue, 0, pDesc->cbSize);
     pr09addtoFreeLst (HashValue, pDesc);
   }  
}

void pr09DeleteAllHashItems(tpr09DynamicHashDescriptor *pDesc)
{
  tpr09DynaHashListItem* chunk;
  while (pDesc->chunkList!=NULL){
    chunk = pDesc->chunkList;
    pDesc->chunkList = pDesc->chunkList->next_p;
    pr09free(chunk->data_p, "tpr09DynamicHashDescriptor->Chunk");
    pr09free(chunk,         "tpr09DynamicHashDescriptor->Item");
  }

  pDesc->freeList=NULL; 
  pr09HTClearHashTable (&(pDesc->hTab));
}

void pr09ResetHashItemIterator(tpr09DynamicHashDescriptor *pDesc){
  pr09HTResetIterator(&(pDesc->hTab));
}

void * pr09HashItemIteratorNextItem(tpr09DynamicHashDescriptor *pDesc){
  return ((void*) pr09HTIteratorNextItem(&(pDesc->hTab)));
}
/*
void pr09HashDump(tpr09DynamicHashDescriptor *pDesc, char* file){
  int realFile=0;
  FILE* datei = NULL;
  if (!strcmp(file,"stderr")){
    datei = stderr;
  } else if (!strcmp(file,"stdout")) {
    datei = stdout;
  } else { 
    realFile=1;
    datei = fopen(file,"a");    
  }
  pr09HTDump (&(pDesc->hTab), datei );    
  fflush(datei);
  if (realFile) fclose(datei);
}
*/
/*==================================================================*
 *  LOCAL FUNCTIONS                                                 *
 *==================================================================*/

static void *pr09malloc(size_t lSize, char* szStr)
{
#if !defined(USE_MALLOC)
  return pr03mAllocat((tsp00_Int4)lSize, szStr);
#else
  return malloc(lSize);
#endif
}

static void *pr09realloc(void *p, size_t lSize, char* szStr)
{
#if !defined(USE_MALLOC)
  return pr03mReallocat((tsp00_Uint1*)p, (tsp00_Int4)lSize, szStr);
#else
  return realloc(p, lSize);
#endif
}

static void pr09free(void *p, char *szStr)
{
#if !defined(USE_MALLOC)
  pr03mFree(p, szStr);
  return;
#else
  free(p);
  return;
#endif
}

static tsp00_Int4 CompareTPR05_String (const tpr09HashValue Item1, const tpr09HashValue Item2)
{
    tpr05_String* string1=(tpr05_String*)Item1;
    tpr05_String* string2=(tpr05_String*)Item2;

    if (string1->encodingType == string2->encodingType)
    {
        if (string1->cbLen != string2->cbLen)
        {
            return 0;
        }
        return (! memcmp(string1->rawString, string2->rawString, string1->cbLen));
    }
    else
      return (0);
}

static void pr09addtoFreeLst (void * HItem, tpr09DynamicHashDescriptor * dynaDesc)
{
   ((tpr09DynaHashListItem*)HItem)->next_p = (dynaDesc->freeList)?dynaDesc->freeList->next_p:NULL;
   dynaDesc->freeList = (tpr09DynaHashListItem*) HItem ;
}

static void* pr09getnewHashItem (tpr09DynamicHashDescriptor * dynaDesc) {
   void *newItem=NULL;
   if ( dynaDesc->freeList == NULL )
   {
      int i;
      /* the free-list is empty; allocate a new chunk of items
         from the heap and assign them to the free-list  */
      tpr09DynaHashListItem *newChunk=  (tpr09DynaHashListItem *) pr09malloc(sizeof(tpr09DynaHashListItem), "tpr09DynamicHashDescriptor->Chunk");
      if(! newChunk) return NULL; 
      newChunk->next_p    = dynaDesc->chunkList;
      dynaDesc->chunkList = newChunk;
      
      newChunk->data_p = pr03mCallocT(dynaDesc->cbIncCount, dynaDesc->cbSize, "tpr09DynamicHashDescriptor->Item");
      if(! newChunk->data_p) return NULL; 
      memset(newChunk->data_p,0,dynaDesc->cbIncCount*dynaDesc->cbSize); 
      
      /* built the freelist  */
      newItem = newChunk->data_p;
      for(i=0; i<dynaDesc->cbIncCount; i++, newItem= ((char*)newItem) + dynaDesc->cbSize )
      {
         ((tpr09DynaHashListItem*)newItem)->next_p = dynaDesc->freeList;
         dynaDesc->freeList = ((tpr09DynaHashListItem*)newItem); 
      }
   }
   /* return the first item from the free-list */
   newItem = (void*) dynaDesc->freeList;
   dynaDesc->freeList = dynaDesc->freeList->next_p;

   return newItem;

} /* hash_new */
