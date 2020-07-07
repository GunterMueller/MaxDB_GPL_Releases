/*!========================================================================

  @file         RTETask_DoublyLinkedList.hpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Doubly linked list base class.

                This class is used to implement classes
                which are dealing with doubly linked elements
                like a "Ready to Run Queue", free
                list for queue elements, "Connection Queue" etc.

  @since        2005-04-25  16:31
  @sa           

  ==========================================================================

  \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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

  \endif
 ============================================================================*/


#ifndef RTETASK_DOUBLYLINKEDLIST_HPP
#define RTETASK_DOUBLYLINKEDLIST_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_Types.h"
#include "RunTime/Tasking/RTETask_DoublyLinkedListElement.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "RunTime/System/RTESys_MemoryBarrier.h"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

// use this types for RTETask_DoublyLinkedLists with no locking
typedef int DummyLockedScopeDLList;
typedef int DummyLockDLList;

/*!---------------------------------------------------------------------------
  @class   RTETask_DoublyLinkedList
  @brief   Doubly linked list base class.

  template arguments<LockedScopeType,LockType> 
            Type of locked scope might be a dummy type 
            for none locking lists/queues.

            This class is used to implement classes
            which are dealing with doubly linked elements
            like a "Ready to Run Queue", free
            list for queue elements, "Connection Queue" etc.

            The list does not create list elements. All
            elements are created outside of the list.

 ----------------------------------------------------------------------------*/

template <class LockedScopeType, class LockType > class RTETask_DoublyLinkedList
{
public:
    /*!
       @brief          Constructor
     */
    RTETask_DoublyLinkedList()
        : m_Head(0),
          m_Tail(0),
          m_ElemCount(0),
          m_Lock(0) {}

    /*!
       @brief          Checks whether list is empty
       @return         true if empty

     */
    SAPDB_Bool IsEmpty() const
    { 
        // Write memory barrier has been called by LockedScope which are part of 
        // the insert and remove methodes
        RTESys_ReadMemoryBarrier();

        return 0 == m_Head;
    }

    /*!
       @brief          Returns the actual element count of the list
       @return         Element count
     */
    SAPDB_UInt4 ListLen() const
    { 
        // Write memory barrier has been called by LockedScope which are part of 
        // the insert and remove methodes
        RTESys_ReadMemoryBarrier();

        return m_ElemCount; 
    }

    /*!
       @brief          Returns the first element
       @return         First element
     */
    RTETask_DoublyLinkedListElement* FirstElement() const
    { 
        // Write memory barrier has been called by LockedScope which are part of 
        // the insert and remove methodes
        RTESys_ReadMemoryBarrier();

        return m_Head;
    }

    /*!
       @brief          Returns the last element
       @return         Last element
     */
    RTETask_DoublyLinkedListElement* LastElement() const
    { 
        // Write memory barrier has been called by LockedScope which are part of 
        // the insert and remove methodes
        RTESys_ReadMemoryBarrier();

        return m_Tail;
    }

    /*!
       @brief          Inserts a new element at the beginning of the list
       @param          element [in]  - Element to be inserted
       @return         Inserted element;
     */
    inline RTETask_DoublyLinkedListElement* 
                    InsertHead ( RTETask_DoublyLinkedListElement  &element );

    /*!
       @brief          Inserts a new element at the end of the list
       @param          element [in]  - Element to be inserted
       @return         Inserted element;
     */
    inline RTETask_DoublyLinkedListElement* 
                    InsertTail ( RTETask_DoublyLinkedListElement  &element );

    /*!
       @brief          Inserts a new element before the element
       @param          element    [in] - Element to be inserted
       @param          posElement [in] - Insert position; the element 
                                         before the new element will be 
                                         inserted
       @return         Inserted element;
     */
    inline RTETask_DoublyLinkedListElement* 
                    InsertBefore( RTETask_DoublyLinkedListElement &element,
                                  RTETask_DoublyLinkedListElement &posElement );


    /*!
       @brief          Inserts a new element behind the element
       @param          element    [in] - Element to be inserted
       @param          posElement [in] - Insert position; the element 
                                         behind the new element will be 
                                         inserted
       @return         Inserted element;
     */
    inline RTETask_DoublyLinkedListElement*
                    InsertBehind( RTETask_DoublyLinkedListElement &element,
                                  RTETask_DoublyLinkedListElement &posElement );

    /*!
       @brief          Remove element from the beginning of the list
       @return         Pointer to the removed element, 
                       null if no element available
     */
    inline RTETask_DoublyLinkedListElement* RemoveHead();

    /*!
       @brief          Remove element from the end of the list
       @return         Pointer to the removed element, 
                       null if no element available
     */
    inline RTETask_DoublyLinkedListElement* RemoveTail();

    /*!
       @brief          Remove a given element from list
       @param          element [in] - Pointer to the element which
                                      is removed.
       @return         Pointer to the next element after the removed one, if any.
                       Else the last element in list.
                       The methode returns null if no further element is available.
     */
    inline RTETask_DoublyLinkedListElement* Remove( RTETask_DoublyLinkedListElement &element );

private:
    RTETask_DoublyLinkedListElement *   m_Head;
    RTETask_DoublyLinkedListElement *   m_Tail;
    SAPDB_UInt4                         m_ElemCount;  // Actual element count in the list
    LockType                            m_Lock;       // Lock hook
};


/*---------------------------------------------------------------------------*/

template <class LockedScopeType, class LockType>
   inline RTETask_DoublyLinkedListElement* 
      RTETask_DoublyLinkedList<LockedScopeType,LockType>
         ::InsertHead ( RTETask_DoublyLinkedListElement  &element )
{
    SAPDBERR_ASSERT_STATE(element.m_Next == 0);
    SAPDBERR_ASSERT_STATE(element.m_Prev == 0);
    SAPDBERR_ASSERT_STATE(element.m_ListCheck == 0);

#if ASSERT_STATE_OPTION != SAPDBERR_OFF
    element.m_ListCheck = (void*)this;
#endif

    // lock scope if template type is not a dummy LockedScope type like 'interger'
    LockedScopeType Lock( m_Lock );

    if ( 0 == m_Head )
    {
        m_Head          = &element;
        m_Tail          = &element;
    }
    else
    {
        element.m_Next   = m_Head;
        m_Head->m_Prev   = &element;
        m_Head           = &element;
    }

    ++m_ElemCount;

    return m_Head;
}

/*---------------------------------------------------------------------------*/

template <class LockedScopeType, class LockType>
    inline RTETask_DoublyLinkedListElement*
        RTETask_DoublyLinkedList<LockedScopeType,LockType>
            ::RemoveHead ()
{
    RTETask_DoublyLinkedListElement*  element = 0;

    if ( IsEmpty() )
        return 0; // list is empty

    {   // lock scope if template type is not a dummy LockedScope type like 'interger'
        LockedScopeType Lock( m_Lock );

        element = m_Head;

        if ( 1 == m_ElemCount )
        {
            m_Head            = 0;
            m_Tail            = 0;
        }
        else
        {
            m_Head            = m_Head->m_Next;
            m_Head->m_Prev    = 0;
        }

        --m_ElemCount;
    }

    element->m_Next = 0;

#if ASSERT_STATE_OPTION != SAPDBERR_OFF
    element->m_ListCheck = 0;
#endif

    return element;
}

/*---------------------------------------------------------------------------*/

template <class LockedScopeType, class LockType>
    inline RTETask_DoublyLinkedListElement*
        RTETask_DoublyLinkedList<LockedScopeType,LockType>
            ::InsertTail ( RTETask_DoublyLinkedListElement  &element )
{
    SAPDBERR_ASSERT_STATE(element.m_Next == 0);
    SAPDBERR_ASSERT_STATE(element.m_Prev == 0);
    SAPDBERR_ASSERT_STATE(element.m_ListCheck == 0);

#if ASSERT_STATE_OPTION != SAPDBERR_OFF
    element.m_ListCheck = (void*)this;
#endif
    // lock scope if template type is not a dummy LockedScope type like 'interger'
    LockedScopeType Lock( m_Lock );

    if ( 0 == m_Head )
    {
        m_Head           = &element;
        m_Tail           = &element;
    }
    else
    {
        element.m_Prev    = m_Tail;
        m_Tail->m_Next    = &element;
        m_Tail            = &element;
    }

    ++m_ElemCount;

    return m_Tail;
}

/*---------------------------------------------------------------------------*/

template <class LockedScopeType, class LockType>
    inline RTETask_DoublyLinkedListElement*
        RTETask_DoublyLinkedList<LockedScopeType,LockType>
            ::RemoveTail ()
{
    if ( IsEmpty() )
        return 0; // list is empty

    RTETask_DoublyLinkedListElement*  element;

    {   // lock scope if template type is not a dummy LockedScope type like 'interger'
        LockedScopeType Lock( m_Lock );

        element = m_Tail;

        if ( 1 == m_ElemCount )
        {
            m_Head            = 0;
            m_Tail            = 0;
        }
        else
        {
            m_Tail            = m_Tail->m_Prev;
            m_Tail->m_Next    = 0;
        }
        --m_ElemCount;
    }

    element->m_Prev = 0;

#if ASSERT_STATE_OPTION != SAPDBERR_OFF
    element->m_ListCheck = 0;
#endif

    return element;
}

/*---------------------------------------------------------------------------*/

template <class LockedScopeType, class LockType>
    inline RTETask_DoublyLinkedListElement* 
        RTETask_DoublyLinkedList<LockedScopeType,LockType>
            ::InsertBefore( RTETask_DoublyLinkedListElement   &element,
                            RTETask_DoublyLinkedListElement   &posElement )
{
    SAPDBERR_ASSERT_STATE(element.m_Next == 0);
    SAPDBERR_ASSERT_STATE(element.m_Prev == 0);
    SAPDBERR_ASSERT_STATE(element.m_ListCheck == 0);
    SAPDBERR_ASSERT_STATE(posElement.m_ListCheck == (void*)this);

    if ( &posElement == m_Head )
    {
        return InsertHead ( element );
    }

#if ASSERT_STATE_OPTION != SAPDBERR_OFF
    element.m_ListCheck = (void*)this;
#endif

    // lock scope if template type is not a dummy LockedScope type like 'interger'
    LockedScopeType Lock( m_Lock );

    element.m_Next           = &posElement;
    element.m_Prev           = posElement.m_Prev;
    posElement.m_Prev        = &element;
    element.m_Prev->m_Next   = &element;

    ++m_ElemCount;

    return &element;
}

/*---------------------------------------------------------------------------*/

template <class LockedScopeType, class LockType>
    inline RTETask_DoublyLinkedListElement*
        RTETask_DoublyLinkedList<LockedScopeType,LockType>
            ::InsertBehind( RTETask_DoublyLinkedListElement   &element,
                            RTETask_DoublyLinkedListElement   &posElement )
{
    SAPDBERR_ASSERT_STATE(element.m_Next == 0);
    SAPDBERR_ASSERT_STATE(element.m_Prev == 0);
    SAPDBERR_ASSERT_STATE(element.m_ListCheck == 0);
    SAPDBERR_ASSERT_STATE(posElement.m_ListCheck == (void*)this);

    if ( &posElement == m_Tail )
    {
        return InsertTail ( element );
    }

#if ASSERT_STATE_OPTION != SAPDBERR_OFF
    element.m_ListCheck = (void*)this;
#endif

    // lock scope if template type is not a dummy LockedScope type like 'interger'
    LockedScopeType Lock( m_Lock );

    element.m_Next           = posElement.m_Next;
    element.m_Prev           = &posElement;
    posElement.m_Next        = &element;
    element.m_Next->m_Prev   = &element;

    ++m_ElemCount;

    return &element;
}

/*---------------------------------------------------------------------------*/

template <class LockedScopeType, class LockType>
    inline RTETask_DoublyLinkedListElement*
        RTETask_DoublyLinkedList<LockedScopeType,LockType>
            ::Remove ( RTETask_DoublyLinkedListElement   &element )
{
    SAPDBERR_ASSERT_STATE(element.m_ListCheck == (void*)this);

    if ( IsEmpty() )
    {
        return 0; // list is empty or element not found
    }

    if ( &element == m_Head )
    {
        RemoveHead();
        return m_Head;
    }

    if ( &element == m_Tail )
    {
        RemoveTail();
        return m_Tail;
    }


    RTETask_DoublyLinkedListElement*  pElement;

    {   // lock scope if template type is not a dummy LockedScope type like 'interger'
        LockedScopeType Lock( m_Lock );

        pElement = element.m_Next;

        element.m_Prev->m_Next = pElement;
        pElement->m_Prev       = element.m_Prev;
        --m_ElemCount;
    }

    element.m_Prev = 0;
    element.m_Next = 0;

#if ASSERT_STATE_OPTION != SAPDBERR_OFF
    element.m_ListCheck = 0;
#endif

    return &element;
}

/*---------------------------------------------------------------------------*/

#endif  /* RTETASK_DOUBLYLINKEDLIST_HPP */
