/*!**************************************************************************

  module      : vpr09LinkedList.c

  -------------------------------------------------------------------------

  responsible : MarcoP

  special area:
  description : Implements a double linked list.

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
#include "vpr09LinkedList.h"

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
  Function: pr09LinkedListInit

  see also: 

  Description: initializes the double linked list
               
  Arguments:
       tpr09LinkedList     [in|out] pointer to a LinkedList

  returns: void
 */
void pr09LinkedListInit(tpr09LinkedList * LinkedList) {
    LinkedList->head=NULL;
    LinkedList->tail=NULL;
    LinkedList->currentItem=NULL;
    LinkedList->elemCnt=0;
}

/*!
  Function: pr09LinkedListInsert

  see also: 

  Description: Inserts a item before the current position of linked list.  
               
  Arguments:
       tpr09LinkedList     [in|out] pointer to a LinkedList
       tpr09LinkedListItem [in]     pinter to a linked list item

  returns: void 
 */
void pr09LinkedListInsert(tpr09LinkedList * LinkedList, tpr09LinkedListItem *LinkedListItem){
    if (   LinkedList->head == NULL
        && LinkedList->tail == NULL) {
        /*list is empty*/
        LinkedListItem->next_p = NULL;
        LinkedListItem->prev_p = NULL;
        LinkedList->head = LinkedListItem;
        LinkedList->tail = LinkedListItem;
    }        
    else { 
      if (LinkedList->currentItem == LinkedList->head) {
        /*insert into top of list*/
        LinkedListItem->prev_p = NULL;
        LinkedList->head = LinkedListItem;
      }
      else {
        LinkedListItem->prev_p = LinkedList->currentItem->prev_p;
        LinkedListItem->prev_p->next_p = LinkedListItem;
      }   

      LinkedListItem->next_p = LinkedList->currentItem;
      LinkedList->currentItem->prev_p = LinkedListItem;
    }
    LinkedList->currentItem = LinkedListItem;
    LinkedList->elemCnt++;
}

/*!
  Function: pr09LinkedListRemoveCurrent

  see also: 

  Description: Removes current item from linked list. 
               
  Arguments:
       tpr09LinkedList     [in|out] pointer to a LinkedList

  returns: void
 */
void pr09LinkedListRemoveCurrent(tpr09LinkedList * LinkedList) {
    tpr09LinkedListItem *RemoveItem = LinkedList->currentItem;
    if (  RemoveItem == NULL
        ||LinkedList->head == NULL
        ||LinkedList->tail == NULL) {
        return;    
    }        
    
    if (RemoveItem == LinkedList->head) {
        LinkedList->head = LinkedList->head->next_p;
        if (RemoveItem->next_p)
           RemoveItem->next_p->prev_p = NULL;
         else
           LinkedList->tail = NULL; 
    }    
    else if (RemoveItem == LinkedList->tail) {
        LinkedList->tail = LinkedList->tail->prev_p;
        if (RemoveItem->prev_p)
          RemoveItem->prev_p->next_p = NULL;
         else
           LinkedList->head = NULL; 
    }    
    else {
        RemoveItem->prev_p->next_p = RemoveItem->next_p;
        RemoveItem->next_p->prev_p = RemoveItem->prev_p;
    }     
    
    if (LinkedList->tail==NULL || LinkedList->head==NULL) {
        LinkedList->currentItem=NULL;
    }    
    else
      LinkedList->currentItem= (LinkedList->currentItem->next_p)?
                                LinkedList->currentItem->next_p:
                                LinkedList->currentItem->prev_p;     

    RemoveItem->prev_p = NULL;
    RemoveItem->next_p = NULL;
    LinkedList->elemCnt--;
}

/*!
  Function: pr09LinkedListRemoveItem

  see also: 

  Description: Removes given item from linked list. 
               
  Arguments:
       tpr09LinkedList     [in|out] pointer to a LinkedList
       tpr09LinkedListItem [in]     pointer to a LinkedListItem

  returns: void
 */
void pr09LinkedListRemoveItem(tpr09LinkedList * LinkedList, tpr09LinkedListItem * LinkedListItem) {
    tpr09LinkedListItem *oldCurrentItem = LinkedList->currentItem;

    if (LinkedListItem == oldCurrentItem) 
        pr09LinkedListRemoveCurrent(LinkedList); 
    else {
      LinkedList->currentItem = LinkedListItem;
      pr09LinkedListRemoveCurrent(LinkedList); 
      LinkedList->currentItem = oldCurrentItem;  
    }    

}

/*!
  Function: pr09LinkedListGetFirstItem

  see also: 

  Description: returns the first Item from linked list or Null if list is empty
               
  Arguments:
       tpr09LinkedList     [in] pointer to a LinkedList

  returns: tpr09LinkedListItem pointer to next list item
 */
tpr09LinkedListItem * pr09LinkedListGetFirstItem(tpr09LinkedList * LinkedList) {
    LinkedList->currentItem = LinkedList->head;
    return LinkedList->currentItem;
}    

/*!
  Function: pr09LinkedListGetLastItem

  see also: 

  Description: returns the last Item from linked list or Null if list is empty
               
  Arguments:
       tpr09LinkedList     [in] pointer to a LinkedList

  returns: tpr09LinkedListItem pointer to next list item
 */
tpr09LinkedListItem * pr09LinkedListGetLastItem(tpr09LinkedList * LinkedList){
    LinkedList->currentItem = LinkedList->tail;
    return LinkedList->currentItem;
}    

/*!
  Function: pr09LinkedListGetNextItem

  see also: 

  Description: returns the next Item after the current Item from linked list or Null if end of list reached
               
  Arguments:
       tpr09LinkedList     [in] pointer to a LinkedList

  returns: tpr09LinkedListItem pointer to next list item
 */
tpr09LinkedListItem * pr09LinkedListGetNextItem(tpr09LinkedList * LinkedList){
    if (LinkedList->currentItem && LinkedList->currentItem->next_p) {
        LinkedList->currentItem = LinkedList->currentItem->next_p;      
        return LinkedList->currentItem;
    }    
    return NULL;
}    

/*!
  Function: pr09LinkedListGetPreviousItem

  see also: 

  Description: returns the previous Item before the current Item from linked list or Null if end of list reached
               
  Arguments:
       tpr09LinkedList     [in] pointer to a LinkedList

  returns: tpr09LinkedListItem pointer to next list item
 */
tpr09LinkedListItem * pr09LinkedListGetPreviousItem(tpr09LinkedList * LinkedList){
    if (LinkedList->currentItem && LinkedList->currentItem->prev_p) { 
        LinkedList->currentItem = LinkedList->currentItem->prev_p;
        return LinkedList->currentItem;
    }        
    return NULL;
}    

/*!
  Function: pr09LinkedListGetSize

  see also: 

  Description: Returns the current number of elements in the linked list.
  
  Arguments:
       tpr09LinkedList     [in] pointer to a LinkedList

  returns: number of elements
 */
tsp00_Int4 pr09LinkedListGetSize(tpr09LinkedList * LinkedList) {
    return LinkedList->elemCnt;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/