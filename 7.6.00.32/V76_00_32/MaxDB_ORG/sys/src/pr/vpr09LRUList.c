/*!**************************************************************************

  module      : vpr09LRUList.c

  -------------------------------------------------------------------------

  responsible : MarcoP

  special area:
  description : Implements a LRU List.

  last changed: 2000-01-20  17:17
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



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "vpr09LRUList.h"
#include "gpr01.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/
/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/
/*!
  Function: pr09LRUListInit

  see also: 

  Description: initializes the LRU list
               
  Arguments:
       tpr09LinkedList     [in|out] pointer to a LRUList

  returns: void
 */
void pr09LRUListInit(tpr09LinkedList * LRUList) {
     pr09LinkedListInit(LRUList);    
}

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
void pr09LRUListInsert(tpr09LinkedList * LRUList, tpr09LinkedListItem *LRUListItem)
{
    if (LRUListItem->next_p!=NULL
        ||LRUListItem->prev_p!=NULL){
        pr09LinkedListRemoveItem(LRUList, LRUListItem); 
    }
    pr09LinkedListGetFirstItem(LRUList);
    pr09LinkedListInsert(LRUList, LRUListItem);
}

/*!
  Function: pr09LRUListRemove

  see also: 

  Description: returns the least recently used item from LRUList and removes it from the list
               
  Arguments:
       tpr09LinkedList     [in|out] pointer to a LRUList

  returns: tpr09LinkedListItem pointer to item which was removed from LRU list
 */
tpr09LinkedListItem * pr09LRUListRemove(tpr09LinkedList * LRUList)
{
    tpr09LinkedListItem *erg = pr09LinkedListGetLastItem(LRUList);
    pr09LinkedListRemoveCurrent(LRUList);
    return erg; 
}

/*!
  Function: pr09LRUListRemoveItem

  see also: 

  Description: removes the given LRUListItem from LRUList
               
  Arguments:
       tpr09LinkedList     [in|out] pointer to a LRUList

  returns: void
 */
void pr09LRUListRemoveItem(tpr09LinkedList * LRUList, tpr09LinkedListItem *LRUListItem){
    pr09LinkedListRemoveItem(LRUList, LRUListItem);
    pr09LinkedListGetLastItem(LRUList);    
}    

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/