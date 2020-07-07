/*!========================================================================

  @file         RTECont_DoublyLinkedListElement.hpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Doubly linked list elements base class
 
                A element contains two pointers pointing to successor and 
                predecessor elements, respectively. 

  @since        2004-12-16  12:23
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


#ifndef RTECont_DoublyLinkedListELEMENT_HPP
#define RTECont_DoublyLinkedListELEMENT_HPP

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

template <class LockedScopeType, class LockType > class RTECont_DoublyLinkedList;

/*!---------------------------------------------------------------------------
  @class   RTECont_DoublyLinkedListElement
  @brief   Element of a doubly linked list

           A element contains two pointers pointing to successor and 
           predecessor elements, respectively. 
           calls. 
 ----------------------------------------------------------------------------*/

class RTECont_DoublyLinkedListElement
{
public:
    /*!
       @brief          Constructor
     */
    RTECont_DoublyLinkedListElement()
                    : m_Prev(0),
                      m_Next(0),
                      m_ListCheck(0) {}

    /*!
       @brief          Returns the previous element
       @return         Previous element
     */
    RTECont_DoublyLinkedListElement * PreviousElement() const
    {
        return m_Prev;
    }

    /*!
       @brief          Returns the next element
       @return         Next element
     */
    RTECont_DoublyLinkedListElement * NextElement() const
    {
        return m_Next;
    }

private:
    template <class LockedScopeType, class LockType > friend class RTECont_DoublyLinkedList;

    // 'RTECont_DoublyLinkedList' don't use methodes to access this 
    // members because we use them in locked scopes of list and 
    // iterator classes. This prevents unintended side effects
    // caused by using a locked scope spanned over methode calls.
    RTECont_DoublyLinkedListElement*   m_Prev;
    RTECont_DoublyLinkedListElement*   m_Next;

    void*                              m_ListCheck;  // used for checks
};

#endif  /* RTECont_DoublyLinkedListELEMENT_HPP */
