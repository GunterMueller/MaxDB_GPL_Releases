/*!**************************************************************************

  module      : vpr09LinkedList.h

  -------------------------------------------------------------------------

  responsible : MarcoP

  special area:
  description : Implements a double linked list.

  Note that this implementation is not synchronized

  last changed: 2000-01-26
  see also    : :pf/LinkedListTest.c

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


#ifndef VPR09LinkedList_H
#define VPR09LinkedList_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "gpr01.h"

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
  Decleration: tpr09LinkedListItem
  Description: item of linked list

  a liked list item stores the predecessor and successor of the item and
  also a pointer to corresponding data.
*/
typedef struct tpr09LinkedListItem {
   void                         * data_p;   /* pointer to item value*/
   struct tpr09LinkedListItem   * prev_p;	/* pointer to the previous LRU list item  */
   struct tpr09LinkedListItem   * next_p;	/* pointer to the next LRU list item  */
} tpr09LinkedListItem;

/*!
  Decleration: tpr09LinkedList
  Description: basis structure for the double linked list

*/
typedef struct tpr09LinkedList {
  tpr09LinkedListItem *head;        /* pointer to the head of linked list */
  tpr09LinkedListItem *tail;        /* pointer to the tail of linked list */
  tpr09LinkedListItem *currentItem; /* pointer to the current item of linked list (iterator)*/
  tsp00_Int4          elemCnt;      /* number of current elements */
}  tpr09LinkedList;

/*!
  Function: pr09LinkedListInit

  see also:

  Description: initializes the double linked list.

  Sets the head, tail and iterator of list to NULL;

  Arguments:
       tpr09LinkedList     [in|out] pointer to a LinkedList

  returns: void
 */
void pr09LinkedListInit(tpr09LinkedList * LinkedList);

/*!
  Function: pr09LinkedListInsert

  see also:

  Description: Inserts an item into the linked list.

  The given list item will inserted into the double linked list before
  the current iterator position. The iterator will be set to the new
  inserted item.

  Arguments:
       tpr09LinkedList     [in|out] pointer to a LinkedList
       tpr09LinkedListItem [in]     pointer to a linked list item

  returns: void
 */
void pr09LinkedListInsert(tpr09LinkedList * LinkedList, tpr09LinkedListItem *LinkedListItem);

/*!
  Function: pr09LinkedListRemoveCurrent

  see also:

  Description: Removes the current item from linked list.

  Removes the list item which pointed by iterator from linked list and
  sets the iterator to the next list item.

  Arguments:
       tpr09LinkedList     [in|out] pointer to a LinkedList

  returns: void
 */
void pr09LinkedListRemoveCurrent(tpr09LinkedList * LinkedList);

/*!
  Function: pr09LinkedListRemoveItem

  see also:

  Description: Removes given item from linked list.

  Removes the given list item from linked list and sets the iterator
  to the next list item.

  Arguments:
       tpr09LinkedList     [in|out] pointer to a LinkedList
       tpr09LinkedListItem [in]     pointer to a LinkedListItem

  returns: void
 */
void pr09LinkedListRemoveItem(tpr09LinkedList * LinkedList, tpr09LinkedListItem * LinkedListItem);

/*!
  Function: pr09LinkedListGetFirstItem

  see also:

  Description: returns the first item from linked list or Null if list is empty

  The iterator will be set to the first item of list. The first list item
  will be returned. If the list is empty a Null value will returned.

  Arguments:
       tpr09LinkedList     [in] pointer to a LinkedList

  returns: tpr09LinkedListItem pointer to founded list item or NULL
 */
tpr09LinkedListItem * pr09LinkedListGetFirstItem(tpr09LinkedList * LinkedList);

/*!
  Function: pr09LinkedListGetLastItem

  see also:

  Description: Returns the last item from linked list.

  The iterator will be set to the last item of list. The last list item
  will be returned. If the list is empty a Null value will returned.

  Arguments:
       tpr09LinkedList     [in] pointer to a LinkedList

  returns: tpr09LinkedListItem pointer to founded list item or NULL
 */
tpr09LinkedListItem * pr09LinkedListGetLastItem(tpr09LinkedList * LinkedList);


/*!
  Function: pr09LinkedListGetNextItem

  see also:

  Description: Returns the next item from linked list.

  The iterator will be set to the next item of list and this list item
  will returned. If the end of list is reached a Null value will returned.

  Arguments:
       tpr09LinkedList     [in] pointer to a LinkedList

  returns: tpr09LinkedListItem pointer to next list item or NULL
 */
tpr09LinkedListItem * pr09LinkedListGetNextItem(tpr09LinkedList * LinkedList);

/*!
  Function: pr09LinkedListGetPreviousItem

  see also:

  Description: Returns the previous item from linked list.

  The iterator will be set to the previous item of list and this list item
  will returned. If the beginning of list is reached a Null value will returned.

  Arguments:
       tpr09LinkedList     [in] pointer to a LinkedList

  returns: tpr09LinkedListItem pointer to previous list item or NULL
 */
tpr09LinkedListItem * pr09LinkedListGetPreviousItem(tpr09LinkedList * LinkedList);

/*!
  Function: pr09LinkedListGetSize

  see also:

  Description: Returns the current number of elements in the linked list.

  Arguments:
       tpr09LinkedList     [in] pointer to a LinkedList

  returns: number of elements
 */
tsp00_Int4 pr09LinkedListGetSize(tpr09LinkedList * LinkedList);

#endif  /*vpr09LinkedList_H */
