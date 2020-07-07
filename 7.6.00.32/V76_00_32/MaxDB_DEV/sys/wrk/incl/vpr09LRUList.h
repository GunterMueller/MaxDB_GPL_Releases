/*!**************************************************************************

  module      : vpr09LRUList.h

  -------------------------------------------------------------------------

  responsible : MarcoP

  special area:
  description : Implements a LRU List.


  last changed: 2000-01-26
  see also    :

  -------------------------------------------------------------------------



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


#ifndef VPR09LRUList_H
#define VPR09LRUList_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "vpr09LinkedList.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
  Function: pr09LRUListInit

  see also:

  Description: initializes the LRU list

  Arguments:
       tpr09LinkedList     [in|out] pointer to a LRUList

  returns: void
 */
void pr09LRUListInit(tpr09LinkedList * LRUList);

/*!
  Function: pr09LRUListInsert

  see also:

  Description: Inserts or removes a item to the end of LRU list.

  The LRUListItem will inserted to the end of LRUList. If the item already
  exists it will be removed to the end of the list.

  Arguments:
       tpr09LinkedList     [in|out] pointer to a LRUList
       tpr09LinkedListItem [in]     pinter to a LRU list item

  returns: void
 */
void pr09LRUListInsert(tpr09LinkedList * LRUList, tpr09LinkedListItem *LRUListItem);

/*!
  Function: pr09LRUListRemove

  see also:

  Description: returns the least recently used item from LRUList and removes it from the list

  Arguments:
       tpr09LinkedList     [in|out] pointer to a LRUList

  returns: tpr09LinkedListItem pointer to item which was removed from LRU list
 */
tpr09LinkedListItem * pr09LRUListRemove(tpr09LinkedList * LRUList);

/*!
  Function: pr09LRUListRemoveItem

  see also:

  Description: removes the given LRUListItem from LRUList

  Arguments:
       tpr09LinkedList     [in|out] pointer to a LRUList

  returns: void
 */
void pr09LRUListRemoveItem(tpr09LinkedList * LRUList, tpr09LinkedListItem *LRUListItem);

#endif  /*VPR09LRUList_H */
