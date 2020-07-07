/*!================================================================

  module:       vpr09DynaDesc.h

  responsible:  BurkhardD

  special area: Verwaltung Dynamischer Descriptoren

  description:  Header fuer Verwaltung Dynamischer Descriptoren

  see also:

 -------------------------------------------------------------------





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
#ifndef __VPR09_DYNADESC_H__
#define __VPR09_DYNADESC_H__

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/
typedef enum {
  Start_epr09 = -1,
  Same_epr09 = -2,
  Next_epr09 = -3,
  Prev_epr09 = -4,
  End_epr09 = -5
} DynaDescSetPos_epr09;

typedef struct {
  int cbIncCount;   /* Anz. der Elemente in einem Stueck angelegt werden */
  int cbSize;       /* Anz. Bytes eines Elements */
  int cbMaxItems;   /* Anz. der Elemente die allociert sind */
  int cbUsedItems;   /* Anz. der Elemente die belegt sind */
  int cbActItem;    /* Anzahl belegter Elemente */
  void **pElem;     /* Zeiger auf eine Array von Elementen */
} tpr09DynamicDescriptor;

/*==================================================================*
 *  PROTOTYPES                                                      *
 *==================================================================*/

void pr09InitDescriptor(tpr09DynamicDescriptor *pDesc, int cbIncCount, int cbSize);

tpr09DynamicDescriptor *pr09NewDescriptor(int cbIncCount, int cbSize);

void pr09FreeDescriptor(tpr09DynamicDescriptor *pDesc);

void pr09DeleteDescriptor(tpr09DynamicDescriptor *pDesc);

void *pr09AddItem(tpr09DynamicDescriptor *pDesc);

int pr09SetIndex(tpr09DynamicDescriptor *pDesc, int cbIndex);

int pr09GetIndex(tpr09DynamicDescriptor *pDesc);

void *pr09GetItemEx(tpr09DynamicDescriptor *pDesc, int cbIndex);

void *pr09GetItem(tpr09DynamicDescriptor *pDesc);

void *pr09GetNextItem(tpr09DynamicDescriptor *pDesc);

void *pr09GetPrevItem(tpr09DynamicDescriptor *pDesc);

int pr09ItemFindIndex(tpr09DynamicDescriptor *pDesc, void *pElem);

int pr09GetItemCount(tpr09DynamicDescriptor *pDesc);

void pr09CloseItemEx(tpr09DynamicDescriptor *pDesc, int cbIndex);

void pr09CloseItem(tpr09DynamicDescriptor *pDesc);

void pr09CloseAllItems(tpr09DynamicDescriptor *pDesc);

void pr09OrderItems(tpr09DynamicDescriptor *pDesc,  int ( *compare )( const void *elem1, const void *elem2 ));

#endif
