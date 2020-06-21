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





    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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
#include "vpr09DynaDesc.h"
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
void pr09InitDescriptor(tpr09DynamicDescriptor *pDesc, int cbIncCount, int cbSize)
{
  pDesc->cbMaxItems = 0;
  pDesc->cbUsedItems = 0;
  pDesc->cbIncCount = cbIncCount;
  pDesc->cbSize = cbSize;
  pDesc->cbActItem = -1;
  pDesc->pElem = NULL;
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

tpr09DynamicDescriptor *pr09NewDescriptor(int cbIncCount, int cbSize)
{
  tpr09DynamicDescriptor *p = pr09malloc((size_t)sizeof(tpr09DynamicDescriptor), "pr09Desc");
  if (p)
    pr09InitDescriptor(p, cbIncCount, cbSize);
  return p;
}

/*!
  Function: pr09FreeDescriptor 

  description: Gibt alle Elemente des Dynamischen Descriptors frei.

  arguments:
      pDesc        [in]  Zeiger auf einen Dynamischen Descriptor

  return value: none
 */

void pr09FreeDescriptor(tpr09DynamicDescriptor *pDesc)
{
  int i;
  for(i=0; i < pDesc->cbMaxItems;i++) {
    pr09free(pDesc->pElem[i], "pr09Item");
    pDesc->pElem[i] = NULL;
  }  
  pr09free(pDesc->pElem, "pr09Elem");
  pr09InitDescriptor(pDesc, pDesc->cbIncCount, pDesc->cbSize);
}

/*!
  Function: pr09DeleteDescriptor 

  description: Gibt alle Elemente des Dynamischen Descriptors und den Descriptor
               selbst frei.

  arguments:
      pDesc        [in]  Zeiger auf einen Dynamischen Descriptor

  return value: none
 */

void pr09DeleteDescriptor(tpr09DynamicDescriptor *pDesc)
{
  if (pDesc->cbMaxItems > 0)
    pr09FreeDescriptor(pDesc);
  if (pDesc->pElem)
    pr09free(pDesc->pElem, "pr09Elem");
  pr09free(pDesc, "pr09Desc");
}

/*!
  Function: pr09AddItem 

  description: Erweitert die Liste der Elemente des Descriptors um ein Element.
               Das neue Element wird aus dem Pool geholt oder falls keine 
	       Elementen mehr im Pool varhanden werden zuerst neue hinzugefuegt

  arguments:
      pDesc        [in]  Zeiger auf einen Dynamischen Descriptor

  return value: Zeiger auf das der Liste neu hinzugefeugte Element.
 */

void *pr09AddItem(tpr09DynamicDescriptor *pDesc)
{
  int i;
  if (pDesc->cbMaxItems <= pDesc->cbUsedItems ) {
    if (!pDesc->pElem) 
      /* allocate an array of pointers */      
      pDesc->pElem = pr09malloc(sizeof(void*)*pDesc->cbIncCount, "pr09Elem");
    else
      pDesc->pElem = pr09realloc(pDesc->pElem, sizeof(void*)*(pDesc->cbMaxItems+pDesc->cbIncCount), "pr09Elem");

    /* allocate for each pointer on Element */
    for(i=pDesc->cbUsedItems; i < pDesc->cbUsedItems+pDesc->cbIncCount; i++) {
      pDesc->pElem[i] = pr09malloc(pDesc->cbSize, "pr09Item");
    }
    pDesc->cbMaxItems += pDesc->cbIncCount;
  }
  pDesc->cbActItem = pDesc->cbUsedItems++;
  return pDesc->pElem[pDesc->cbActItem];
}

/*!
  Function: pr09SetIndex 

  description: Set den aktuellen Index auf ein Element in der Liste. 

  arguments:
      pDesc        [in]  Zeiger auf einen Dynamischen Descriptor
      cbIndex      [in]  Index des aktuellen Elements der Liste

  return value: Index des letzten aktuellen Zeiger.
 */

int pr09SetIndex(tpr09DynamicDescriptor *pDesc, int cbIndex)
{
  int cbOldIndex = pDesc->cbActItem;
  if (cbIndex < 0) {
    switch (cbIndex) {
    case (Start_epr09) : {
      cbIndex = -1;
      break;
    }
    case (End_epr09) : {
      cbIndex = pDesc->cbUsedItems-1;
      break;
    }
    case (Same_epr09) : {
      cbIndex = pDesc->cbActItem;
      break;
    }
    case (Next_epr09) : {
      cbIndex = pDesc->cbActItem+1;
      break;
    }
    case (Prev_epr09) : {
      cbIndex = pDesc->cbActItem-1;
      break;
    }
    }
  }
  pDesc->cbActItem = cbIndex;
  return cbOldIndex;
}

/*!
  Function: pr09GetIndex 

  description: Liefert den aktuellen Index auf ein Element in der Liste. 

  arguments:
      pDesc        [in]  Zeiger auf einen Dynamischen Descriptor

  return value: Index des aktuellen Zeiger.
 */

int pr09GetIndex(tpr09DynamicDescriptor *pDesc)
{
  return pDesc->cbActItem;
}

/*!
  Function: pr09FindElem 

  description: Liefert den Index auf den in pDesc angegeben Descriptor. 

 */

int pr09ItemFindIndex(tpr09DynamicDescriptor *pDesc, void *pElem)
{
  int i;
  for(i=0; i < pDesc->cbUsedItems; i++) {
    if (pDesc->pElem[i] == pElem)
      return(i);
  }
  return(-1);
}

/*!
  Function: pr09GetItemEx 

  description: Liefert einen Zeiger auf das in cbIndex angegeben Element der 
               Liste. 

  arguments:
      pDesc        [in]  Zeiger auf einen Dynamischen Descriptor
      cbIndex      [in]  Elementindex.

  return value: Zeiger auf das in Index angegeben Element.
 */

void *pr09GetItemEx(tpr09DynamicDescriptor *pDesc, int cbIndex)
{
  if (cbIndex < 0)
    return NULL;
  if (cbIndex >= pDesc->cbUsedItems)
    return NULL;
  return pDesc->pElem[cbIndex];
}

/*!
  Function: pr09GetItem

  description: Liefert einen Zeiger auf aktuelle Element der Liste. 

  arguments:
      pDesc        [in]  Zeiger auf einen Dynamischen Descriptor

  return value: Zeiger auf das aktuelle Element.
 */

void *pr09GetItem(tpr09DynamicDescriptor *pDesc)
{
  return pr09GetItemEx(pDesc, pDesc->cbActItem);
}

/*!
  Function: pr09GetNextItem

  description: Liefert einen Zeiger auf naechste Element der Liste. 

  arguments:
      pDesc        [in]  Zeiger auf einen Dynamischen Descriptor

  return value: Zeiger auf das naechste Element des Descriptors.
 */

void *pr09GetNextItem(tpr09DynamicDescriptor *pDesc)
{
  if (++pDesc->cbActItem < pDesc->cbUsedItems) 
    return pr09GetItemEx(pDesc, pDesc->cbActItem);
  else {
    pDesc->cbActItem--;
    return NULL;
  }
}

/*!
  Function: pr09GetPrevItem

  description: Liefert einen Zeiger auf das vorherige Element der Liste. 

  arguments:
      pDesc        [in]  Zeiger auf einen Dynamischen Descriptor

  return value: Zeiger auf das vorherige Element des Descriptors.
 */

void *pr09GetPrevItem(tpr09DynamicDescriptor *pDesc)
{
  if (--pDesc->cbActItem > 0) 
    return pr09GetItemEx(pDesc, pDesc->cbActItem);
  else {
    pDesc->cbActItem++;
    return NULL;
  }
}

/*!
  Function: pr09GetItemCount

  description: Liefert die Anzahl der im Descriptor verwendeten Elemente. 

  arguments:
      pDesc        [in]  Zeiger auf einen Dynamischen Descriptor

  return value: Anzahl der Elemente des Descriptors.
 */

int pr09GetItemCount(tpr09DynamicDescriptor *pDesc)
{
  return pDesc->cbUsedItems;
}

/*!
  Function: pr09CloseItemEx

  description: Loescht das in Index angegebene Element des Descriptors.
               Nach loeschen des Elements zeigt das aktuelle Element vor
	       das zu geloeschte Element.

  arguments:
      pDesc        [in]  Zeiger auf einen Dynamischen Descriptor
      cbIndex      [in]  Index des zu loeschenden Elements.

  return value: none
 */

void pr09CloseItemEx(tpr09DynamicDescriptor *pDesc, int cbIndex)
{
  int i;
  if (cbIndex >= 0) {
    /* PTS 1104087 */
    if (cbIndex < pDesc->cbUsedItems) {
      void *p=pDesc->pElem[cbIndex];
      for(i=cbIndex; i < pDesc->cbUsedItems-1; i++) {
	pDesc->pElem[i] = pDesc->pElem[i+1];
      }
      if (i > 0) {
	pDesc->pElem[i]=p;
      }
      if (pDesc->cbUsedItems) {
	pDesc->cbUsedItems--;
	pDesc->cbActItem = cbIndex-1;
      }
    }
  }
}

/*!
  Function: pr09CloseItem

  description: Loescht das aktuelle Element des Descriptors. 

  arguments:
      pDesc        [in]  Zeiger auf einen Dynamischen Descriptor

  return value: none
 */

void pr09CloseItem(tpr09DynamicDescriptor *pDesc)
{
  pr09CloseItemEx(pDesc, pDesc->cbActItem);
}

void pr09CloseAllItems(tpr09DynamicDescriptor *pDesc)
{
  while(pr09GetItemCount(pDesc)) {
    /* PTS 1104087 */
    pr09CloseItemEx(pDesc, pDesc->cbUsedItems-1);
  }
}

/*!
  Function: pr09OrderItems

  description: Sortiert die Elemente des Descriptors. Fuer die Sortierung der 
               Elemente wird ein Pointer auf eine Vergleichsfunktion uebergeben.
	       Diese Funktion bekommt jeweils zwei Elemente (elem1 und elem2) 
	       des Descriptors uebergeben und muss < 0 bei elem1 < elem2, 
	       0 bei elem1 = elem2 und > 0 bei elem1 > elem2 liefern. 

  arguments:
      pDesc        [in]  Zeiger auf einen Dynamischen Descriptor
      compare      [in]  Zeiger auf eine Comparefunktion.

  return value: none
 */

void pr09OrderItems(tpr09DynamicDescriptor *pDesc,  int ( *compare )( const void *elem1, const void *elem2 ))
{
  if (pDesc) {
    int cbCount = pDesc->cbUsedItems;
    if (pDesc->cbUsedItems > 0) {
      qsort( pDesc->pElem, cbCount, sizeof(pDesc->pElem), compare );
    }
  }  
}

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
