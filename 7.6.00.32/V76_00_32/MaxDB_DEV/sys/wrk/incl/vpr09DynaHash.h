/*!================================================================

  module:       vpr09DynaDesc.h

  responsible:  BurkhardD

  special area: Verwaltung Dynamischer Descriptoren

  description:  Header fuer Verwaltung Dynamischer Descriptoren

  see also:

 -------------------------------------------------------------------

                         Copyright by SAP AG, 1998



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
#ifndef __VPR09_DYNAHASH_H__
#define __VPR09_DYNAHASH_H__

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/
#include "vpr09HashTable.h"

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/
typedef struct tpr09DynaHashListItem {
   void                         * data_p;   /* pointer to item value*/
   struct tpr09DynaHashListItem * next_p;   /* pointer to the next LRU list item  */
} tpr09DynaHashListItem;

typedef struct {
  int cbIncCount;   /* Anz. der Elemente in einem Stueck angelegt werden */
  int cbSize;       /* Anz. Bytes eines Elements */
  tpr09DynaHashListItem *chunkList;
  tpr09DynaHashListItem *freeList;
  tpr09HashTable hTab;     /* index structure */
} tpr09DynamicHashDescriptor;

/*==================================================================*
 *  PROTOTYPES                                                      *
 *==================================================================*/


tpr09DynamicHashDescriptor *pr09NewHashDescriptor(int cbIncCount,
                                                  int cbSize,
                                                  tpr09CompareMethod compMeth,
                                                  tpr09HashFunc hashFunc,
                                                  tpr09ExtractKey KeyFunc,
                                                  tpr09PrintHashItem PrintFunc);

void pr09DeleteHashDescriptor(tpr09DynamicHashDescriptor *pDesc);

void *pr09GetNewHashItem(tpr09DynamicHashDescriptor *pDesc);

tpr09_HashTableError pr09AddHashItemToHash(tpr09DynamicHashDescriptor *pDesc,
                                           tpr09HashKey HashKey,
                                           tsp00_Uint4 cbHashKeyLen,
                                           tpr09HashValue *HashValue);

void *pr09FindHashItem(tpr09DynamicHashDescriptor *pDesc,
                       tpr09HashKey HashKey,
                       tsp00_Uint4 cbHashKeyLen);

void pr09DeleteHashItem(tpr09DynamicHashDescriptor *pDesc,
                        tpr09HashKey HashKey,
                        tsp00_Uint4 cbHashKeyLen);

void pr09DeleteAllHashItems(tpr09DynamicHashDescriptor *pDesc);

void pr09ResetHashItemIterator(tpr09DynamicHashDescriptor *pDesc);

void * pr09HashItemIteratorNextItem(tpr09DynamicHashDescriptor *pDesc);
/*
void pr09HashDump(tpr09DynamicHashDescriptor *pDesc, char* file);
*/
#endif
