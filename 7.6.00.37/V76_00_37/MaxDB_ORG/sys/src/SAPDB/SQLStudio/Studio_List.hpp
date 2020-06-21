/*!===========================================================================

  module:       Studio_List.cpp

  ----------------------------------------------------------------------------

  responsible:  MarkusS

  special area: Common

  description:  Definition of the doubly linked list class Studio_List.

  see:          

  created:      2000-08-17

  updated:      2000-08-17

  ----------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG
  



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




============================================================================*/

#ifndef Studio_List_H
#define Studio_List_H


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "SAPDBCommon/SAPDB_Types.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
typedef void *       Studio_ListData;


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*!===========================================================================
  class: Studio_ListElement
 
  description: 
    Class for implementing a list element of a singly linked list.
============================================================================*/
class Studio_ListElement
{
public:
  /*!-------------------------------------------------------------------------
    function:     Studio_ListElement
    description:  Constructor of the class.
  --------------------------------------------------------------------------*/
  Studio_ListElement ()
  {
    mData = 0;
    mNext = 0;
    mPrev = 0;
  }
  /*!-------------------------------------------------------------------------
    function:     Studio_ListElement
    description:  Copy constructor of the class.
  --------------------------------------------------------------------------*/
  Studio_ListElement (const Studio_ListElement& element)
  {
    mData = element.mData;
    mNext = element.mNext;
    mPrev = element.mPrev;
  }
  /*!-------------------------------------------------------------------------
    function:     ~Studio_ListElement
    description:  Destructor of the class.
  --------------------------------------------------------------------------*/
  ~Studio_ListElement() {}
  /*!-------------------------------------------------------------------------
    function:     next
    description:  Get the pointer to the next list element.
    arguments:    -
    returns:      Studio_ListElement *
  --------------------------------------------------------------------------*/
  Studio_ListElement * next () { return mNext; }
  /*!-------------------------------------------------------------------------
    function:     prev
    description:  Get the pointer to the previous list element.
    arguments:    -
    returns:      Studio_ListElement *
  --------------------------------------------------------------------------*/
  Studio_ListElement * prev () { return mPrev; }
  /*!-------------------------------------------------------------------------
    function:     data
    description:  Get the pointer to the data of the list element.
    arguments:    -
    returns:      void *
  --------------------------------------------------------------------------*/
  void * data () { return mData; }
  
private:
  void                     *mData;                 // Pointer to the data of the list element
  Studio_ListElement        *mNext;                 // Pointer to the next element in the list
  Studio_ListElement        *mPrev;                 // Pointer to the previous element in the list

  friend class Studio_List;
  
};  /*! endclass: Studio_ListElement */

typedef Studio_ListElement *       Studio_ListPtr;


/*!===========================================================================
  class: Studio_List
 
  description: 
    Class for implementing a doubly linked list.
============================================================================*/
class Studio_List
{
public:
  /*!-------------------------------------------------------------------------
    function:     Studio_List
    description:  Constructor of the class.
  --------------------------------------------------------------------------*/
  Studio_List ();
  /*!-------------------------------------------------------------------------
    function:     Studio_List
    description:  Copy constructor of the class.
  --------------------------------------------------------------------------*/
  Studio_List (const Studio_List& list);
  /*!-------------------------------------------------------------------------
    function:     ~Studio_List
    description:  Destructor of the class.
  --------------------------------------------------------------------------*/
  ~Studio_List();

  
  /*!-------------------------------------------------------------------------
    function:     append
    description:  Append the given element to the end of the list.
    arguments:    data - Element to append to the list
    returns:      -
  --------------------------------------------------------------------------*/
  void append (Studio_ListData data);
  /*!-------------------------------------------------------------------------
    function:     insert
    description:  Append the given element to the end of the list.
    arguments:    data  - Element to insert into the list
                  index - Position of the new element in the list
    returns:      -
  --------------------------------------------------------------------------*/
  void insert (Studio_ListData data, SAPDB_UInt index=0);
  /*!-------------------------------------------------------------------------
    function:     remove
    description:  Remove the current list element from the list.
    arguments:    /
    returns:      Studio_ListData
  --------------------------------------------------------------------------*/
  Studio_ListData remove ();
  /*!-------------------------------------------------------------------------
    function:     remove
    description:  Remove the given number of list elements from the list
                  starting at the given position.
    arguments:    index   - Position of the element to remove
                  number  - Number of elements to remove
                  
    returns:      Studio_ListData
  --------------------------------------------------------------------------*/
  Studio_ListData remove (SAPDB_UInt index, SAPDB_UInt number=1);
  /*!-------------------------------------------------------------------------
    function:     remove
    description:  Search the list for the given list element and remove it 
                  from the list if it was found.
    arguments:    data    - Pointer to the data of the list element to remove
                  
    returns:      Studio_ListData
  --------------------------------------------------------------------------*/
  Studio_ListData remove (Studio_ListData data);
  /*!-------------------------------------------------------------------------
    function:     clear
    description:  Remove all elements from the list.
    arguments:    -
    returns:      -
  --------------------------------------------------------------------------*/
  void clear ();
  /*!-------------------------------------------------------------------------
    function:     twist
    description:  Twist the list element with the given index with its next
                  element.
    arguments:    index - Position of the element to twist
    returns:      -
  --------------------------------------------------------------------------*/
  void twist (SAPDB_UInt index);
  /*!-------------------------------------------------------------------------
    function:     first
    description:  Get the pointer to the first list element.
    arguments:    -
    returns:      Studio_ListData
  --------------------------------------------------------------------------*/
  Studio_ListData first ();
  /*!-------------------------------------------------------------------------
    function:     last
    description:  Get the pointer to the last list element.
    arguments:    -
    returns:      Studio_ListData
  --------------------------------------------------------------------------*/
  Studio_ListData last ();
  /*!-------------------------------------------------------------------------
    function:     next
    description:  Get the pointer to the next list element of the current.
    arguments:    -
    returns:      Studio_ListData
  --------------------------------------------------------------------------*/
  Studio_ListData next ();
  /*!-------------------------------------------------------------------------
    function:     prev
    description:  Get the pointer to the previous list element of the current.
    arguments:    -
    returns:      Studio_ListData
  --------------------------------------------------------------------------*/
  Studio_ListData prev ();
  /*!-------------------------------------------------------------------------
    function:     current
    description:  Get the pointer to the current list element.
    arguments:    -
    returns:      Studio_ListData
  --------------------------------------------------------------------------*/
  Studio_ListData current () { return mCurrent->mData; }
  /*!-------------------------------------------------------------------------
    function:     at
    description:  Get the pointer to the list element at the given position.
    arguments:    index - Position of the requested element
    returns:      Studio_ListData
  --------------------------------------------------------------------------*/
  Studio_ListData at (SAPDB_UInt index);
  /*!-------------------------------------------------------------------------
    function:     size
    description:  Get the number of list elements.
    arguments:    -
    returns:      SAPDB_UInt
  --------------------------------------------------------------------------*/
  SAPDB_UInt size () { return mSize; }
  /*!-------------------------------------------------------------------------
    function:     isEmpty
    description:  Check if the list is empty.
    arguments:    -
    returns:      SAPDB_Bool
  --------------------------------------------------------------------------*/
  SAPDB_Bool isEmpty () { return (mSize == 0); }

  
private:
  Studio_ListPtr             mFirst;                // Pointer to the first list element
  Studio_ListPtr             mLast;                 // Pointer to the last list element
  Studio_ListPtr             mCurrent;              // Pointer to the current list element
  SAPDB_UInt                mSize;                 // Current number of elements in the list
  
};  /*! endclass: Studio_List */

#endif   /* Studio_List_H */
