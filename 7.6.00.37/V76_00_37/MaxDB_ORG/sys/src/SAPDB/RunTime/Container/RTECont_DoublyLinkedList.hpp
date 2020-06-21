/*!========================================================================

  @file         RTECont_DoublyLinkedList.hpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Doubly linked list base class.

                This class is used to implement classes
                which are dealing with doubly linked elements
                like a "Request Queue", free list for queue elements  etc.

  @since        2006-01-06  15:27
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


#ifndef RTECONT_DOUBLYLINKEDLIST_HPP
#define RTECONT_DOUBLYLINKEDLIST_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_Types.h"
#include "RunTime/System/RTESys_MemoryBarrier.h"
#include "RunTime/Container/RTECont_DoublyLinkedListElement.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

// use this types for RTECont_DoublyLinkedLists with no locking
typedef int DummyLockedScopeDLList;
typedef int DummyLockDLList;

/*!---------------------------------------------------------------------------
  @class   RTECont_DoublyLinkedList
  @brief   Doubly linked list base class.

  template arguments<LockedScopeType,LockType> 
            Type of locked scope might be a dummy type 
            for none locking lists/queues.

            This class is used to implement classes
            which are dealing with doubly linked elements
            like a "Request Queue", free list for queue elements  etc.

            The list does not create list elements. All
            elements are created outside of the list.

 ----------------------------------------------------------------------------*/

template <class LockedScopeType, class LockType > class RTECont_DoublyLinkedList
{
public:
    /*!
       @brief          Constructor
     */
    RTECont_DoublyLinkedList()
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
        if ( m_Head )  // dirty read!
            return false;

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
    RTECont_DoublyLinkedListElement* FirstElement() const
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
    RTECont_DoublyLinkedListElement* LastElement() const
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
    inline RTECont_DoublyLinkedListElement* 
                    InsertHead ( RTECont_DoublyLinkedListElement  &element );

    /*!
       @brief          Inserts a new element at the end of the list
       @param          element [in]  - Element to be inserted
       @return         Inserted element;
     */
    inline RTECont_DoublyLinkedListElement* 
                    InsertTail ( RTECont_DoublyLinkedListElement  &element );

    /*!
       @brief          Inserts a new element before the element
       @param          element    [in] - Element to be inserted
       @param          posElement [in] - Insert position; the element 
                                         before the new element will be 
                                         inserted
       @return         Inserted element;
     */
    inline RTECont_DoublyLinkedListElement* 
                    InsertBefore( RTECont_DoublyLinkedListElement &element,
                                  RTECont_DoublyLinkedListElement &posElement );


    /*!
       @brief          Inserts a new element behind the element
       @param          element    [in] - Element to be inserted
       @param          posElement [in] - Insert position; the element 
                                         behind the new element will be 
                                         inserted
       @return         Inserted element;
     */
    inline RTECont_DoublyLinkedListElement*
                    InsertBehind( RTECont_DoublyLinkedListElement &element,
                                  RTECont_DoublyLinkedListElement &posElement );

    /*!
       @brief          Remove element from the beginning of the list
       @return         Pointer to the removed element, 
                       null if no element available
     */
    inline RTECont_DoublyLinkedListElement* RemoveHead();

    /*!
       @brief          Remove element from the end of the list
       @return         Pointer to the removed element, 
                       null if no element available
     */
    inline RTECont_DoublyLinkedListElement* RemoveTail();

    /*!
       @brief          Remove a given element from list
       @param          element [in] - Pointer to the element which
                                      is removed.
       @return         Pointer to the next element after the removed one, if any.
                       Else the last element in list.
                       The methode returns null if no further element is available.
     */
    inline RTECont_DoublyLinkedListElement* Remove( RTECont_DoublyLinkedListElement &element );

private:
    RTECont_DoublyLinkedListElement *   m_Head;
    RTECont_DoublyLinkedListElement *   m_Tail;
    SAPDB_UInt4                         m_ElemCount;  // Actual element count in the list
    LockType                            m_Lock;       // Lock hook
};


/*---------------------------------------------------------------------------*/

template <class LockedScopeType, class LockType>
   inline RTECont_DoublyLinkedListElement* 
      RTECont_DoublyLinkedList<LockedScopeType,LockType>
         ::InsertHead ( RTECont_DoublyLinkedListElement  &element )
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
    inline RTECont_DoublyLinkedListElement*
        RTECont_DoublyLinkedList<LockedScopeType,LockType>
            ::RemoveHead ()
{
    RTECont_DoublyLinkedListElement*  element = 0;

    {   // lock scope if template type is not a dummy LockedScope type like 'integer'
        LockedScopeType Lock( m_Lock );

        if ( 0 == m_Head )
            return 0; // list is empty

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
    inline RTECont_DoublyLinkedListElement*
        RTECont_DoublyLinkedList<LockedScopeType,LockType>
            ::InsertTail ( RTECont_DoublyLinkedListElement  &element )
{
    SAPDBERR_ASSERT_STATE(element.m_Next == 0);
    SAPDBERR_ASSERT_STATE(element.m_Prev == 0);
    SAPDBERR_ASSERT_STATE(element.m_ListCheck == 0);

#if ASSERT_STATE_OPTION != SAPDBERR_OFF
    element.m_ListCheck = (void*)this;
#endif
    // lock scope if template type is not a dummy LockedScope type like 'integer'
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
    inline RTECont_DoublyLinkedListElement*
        RTECont_DoublyLinkedList<LockedScopeType,LockType>
            ::RemoveTail ()
{
    RTECont_DoublyLinkedListElement*  element;

    {   // lock scope if template type is not a dummy LockedScope type like 'integer'
        LockedScopeType Lock( m_Lock );

        if ( 0 == m_Head )
            return 0; // list is empty

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
    inline RTECont_DoublyLinkedListElement* 
        RTECont_DoublyLinkedList<LockedScopeType,LockType>
            ::InsertBefore( RTECont_DoublyLinkedListElement   &element,
                            RTECont_DoublyLinkedListElement   &posElement )
{
    SAPDBERR_ASSERT_STATE(element.m_Next == 0);
    SAPDBERR_ASSERT_STATE(element.m_Prev == 0);
    SAPDBERR_ASSERT_STATE(element.m_ListCheck == 0);
    SAPDBERR_ASSERT_STATE(posElement.m_ListCheck == (void*)this);


#if ASSERT_STATE_OPTION != SAPDBERR_OFF
    element.m_ListCheck = (void*)this;
#endif

    // lock scope if template type is not a dummy LockedScope type like 'integer'
    LockedScopeType Lock( m_Lock );

    if ( 0 == m_Head )
    {
        m_Head                   = &element;
        m_Tail                   = &element;
    }
    else if ( &posElement == m_Head )
    {
        element.m_Next           = m_Head;
        m_Head->m_Prev           = &element;
        m_Head                   = &element;
    }
    else
    {
        element.m_Next           = &posElement;
        element.m_Prev           = posElement.m_Prev;
        posElement.m_Prev        = &element;
        element.m_Prev->m_Next   = &element;
    }

    ++m_ElemCount;

    return &element;
}

/*---------------------------------------------------------------------------*/

template <class LockedScopeType, class LockType>
    inline RTECont_DoublyLinkedListElement*
        RTECont_DoublyLinkedList<LockedScopeType,LockType>
            ::InsertBehind( RTECont_DoublyLinkedListElement   &element,
                            RTECont_DoublyLinkedListElement   &posElement )
{
    SAPDBERR_ASSERT_STATE(element.m_Next == 0);
    SAPDBERR_ASSERT_STATE(element.m_Prev == 0);
    SAPDBERR_ASSERT_STATE(element.m_ListCheck == 0);
    SAPDBERR_ASSERT_STATE(posElement.m_ListCheck == (void*)this);

#if ASSERT_STATE_OPTION != SAPDBERR_OFF
    element.m_ListCheck = (void*)this;
#endif

    // lock scope if template type is not a dummy LockedScope type like 'integer'
    LockedScopeType Lock( m_Lock );

    if ( 0 == m_Head )
    {
        m_Head                  = &element;
        m_Tail                  = &element;
    }
    else if ( &posElement == m_Tail )
    {
        element.m_Prev           = m_Tail;
        m_Tail->m_Next           = &element;
        m_Tail                   = &element;
    }
    else
    {
        element.m_Next           = posElement.m_Next;
        element.m_Prev           = &posElement;
        posElement.m_Next        = &element;
        element.m_Next->m_Prev   = &element;
    }

    ++m_ElemCount;

    return &element;
}

/*---------------------------------------------------------------------------*/

template <class LockedScopeType, class LockType>
    inline RTECont_DoublyLinkedListElement*
        RTECont_DoublyLinkedList<LockedScopeType,LockType>
            ::Remove ( RTECont_DoublyLinkedListElement   &element )
{
    SAPDBERR_ASSERT_STATE(element.m_ListCheck == (void*)this);

    RTECont_DoublyLinkedListElement*  pElement;

    {   // lock scope if template type is not a dummy LockedScope type like 'integer'
        LockedScopeType Lock( m_Lock );

        if ( 0 == m_Head )
            return 0; // Oops, list is empty!

        if ( 1 == m_ElemCount )
        {
            m_Head            = 0;
            m_Tail            = 0;
        }
        else if ( &element == m_Tail )
        {
            m_Tail            = m_Tail->m_Prev;
            m_Tail->m_Next    = 0;
        }
        else if ( &element == m_Head )
        {
            m_Head            = m_Head->m_Next;
            m_Head->m_Prev    = 0;
        }
        else
        {
            pElement = element.m_Next;

            element.m_Prev->m_Next = pElement;
            pElement->m_Prev       = element.m_Prev;
        }
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

#endif  /* RTECONT_DOUBLYLINKEDLIST_HPP */
