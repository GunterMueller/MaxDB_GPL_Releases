/*!========================================================================

  @file         RTETask_DoublyLinkedListElement.hpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Doubly linked list elements base class
 
                A element contains two pointers pointing to successor and 
                predecessor elements, respectively. 

  @since        2003-10-17  16:42
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


#ifndef RTETASK_DOUBLYLINKEDLISTELEMENT_HPP
#define RTETASK_DOUBLYLINKEDLISTELEMENT_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

template <class LockedScopeType, class LockType > class RTETask_DoublyLinkedList;

/*!---------------------------------------------------------------------------
  @class   RTETask_DoublyLinkedListElement
  @brief   Element of a doubly linked list

           Base class from which all task related doubly linked list 
           element classes should derive.

           A element contains two pointers pointing to successor and 
           predecessor elements, respectively. 
           calls. 
 ----------------------------------------------------------------------------*/

class RTETask_DoublyLinkedListElement
{
public:
    /*!
       @brief          Constructor
     */
    RTETask_DoublyLinkedListElement()
                    : m_Prev(0),
                      m_Next(0),
                      m_ListCheck(0) {}

    /*!
       @brief          Returns the previous element
       @return         Previous element
     */
    RTETask_DoublyLinkedListElement * PreviousElement() const
    {
        return m_Prev;
    }

    /*!
       @brief          Returns the next element
       @return         Next element
     */
    RTETask_DoublyLinkedListElement * NextElement() const
    {
        return m_Next;
    }

private:
    template <class LockedScopeType, class LockType > friend class RTETask_DoublyLinkedList;

    // 'RTETask_DoublyLinkedList' don't use methodes to access this 
    // members because we use them in locked scopes of list and 
    // iterator classes. This prevents unintended side effects
    // caused by using a locked scope spanned over methode calls.
    RTETask_DoublyLinkedListElement*   m_Prev;
    RTETask_DoublyLinkedListElement*   m_Next;

    void*                              m_ListCheck;  // used for checks
};

#endif  /* RTETASK_DOUBLYLINKEDLISTELEMENT_HPP */
