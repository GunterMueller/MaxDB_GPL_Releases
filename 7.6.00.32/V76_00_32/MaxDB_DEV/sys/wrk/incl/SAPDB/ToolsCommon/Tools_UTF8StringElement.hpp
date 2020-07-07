  /*!***************************************************************************

  module:       Tools_UTF8StringElement.hpp

  ------------------------------------------------------------------------------

  responsible:  TillL

  special area: SAPDB Common

  description:  UTF8 string elements of SAPDB_BasicString

                - This module defines the utf-8 string element type to be used 
                  with the SAPDB_DynamicUTF8String and SAPDB_FieldUTF8String class. 
                - Four iterator types are also defined which must be provided
                  by Tools_UTF8StringElement:

                    - Tools_UTF8Iterator
                    - Tools_UTF8ConstIterator
                    - Tools_UTF8ReverseIterator
                    - Tools_UTF8ConstReverseIterator

                - The iterators have nearly identical implementation. However,
                  a template implementation could not be used due to the
                  restriction of not using templates for classes that are to
                  be used with the SAPDB runtime environment.
                
  see also:     

  ------------------------------------------------------------------------------

  copyright:            Copyright (c) 2000-2005 SAP AG




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


#ifndef TOOLS_UTF8STRINGELEMENT_HPP
#define TOOLS_UTF8STRINGELEMENT_HPP

#include <assert.h>

#include "ToolsCommon/Tools_UTF8Basis.hpp"


/*!*****************************************************************************

   class: Tools_UTF8Iterator

   description: This is an utf-8 string element iterator which performs
                multibyte steps from utf8 element to utf8 element

*******************************************************************************/
class Tools_UTF8Iterator
{
public:
    friend class Tools_UTF8ConstIterator;
    friend class Tools_UTF8ReverseIterator;
    friend class Tools_UTF8StringElement;

    typedef SAPDB_UTF8                          ElementType;
    typedef ElementType*                        PointerType;

    /*!---------------------------------------------------------------------
    function:     Tools_UTF8Iterator()
    description:  Default constructor
    arguments:    pos [in]    start position (default = 0)
    return value: none
    ----------------------------------------------------------------------*/
    Tools_UTF8Iterator
        (PointerType                   pos = 0)
        : 
        m_Pos(pos)
    {}

    /*!---------------------------------------------------------------------
    function:     Tools_UTF8Iterator()
    description:  Copy constructor
    arguments:    copy [in]    iterator to be copied
    return value: none
    ----------------------------------------------------------------------*/
    Tools_UTF8Iterator
        (const Tools_UTF8Iterator&      copy)
        : 
        m_Pos(copy.m_Pos)
    {}

    /*!---------------------------------------------------------------------
    function:     operator=()
    description:  Assignment operator
    arguments:    iter [in]   iterator to be assigned
    return value: none
    ----------------------------------------------------------------------*/
    Tools_UTF8Iterator& operator=
        (const Tools_UTF8Iterator&      iter)
    {
        m_Pos = iter.m_Pos;
        return *this;
    }

    /*!---------------------------------------------------------------------
    function:     operator==, !=
    description:  Comparison operators
    arguments:    iter [in]    iterator to be compared
    return value: (bool) 
    prototypes:   2
    ----------------------------------------------------------------------*/
    bool operator==(const Tools_UTF8Iterator& iter) 
    { 
        return m_Pos == iter.m_Pos; 
    }

    bool operator!=(const Tools_UTF8Iterator& iter) 
    { 
        return m_Pos != iter.m_Pos; 
    }

    /*!---------------------------------------------------------------------
    function:     operator*()
    description:  Pointer operator
    arguments:    none
    return value: (ElementType) reference to element
    ----------------------------------------------------------------------*/
    ElementType& operator*()
    {
        return *m_Pos;
    }

    /*!---------------------------------------------------------------------
    function:     operator++()
    description:  Prefix increment; advances iterator to next character
    arguments:    none
    return value: (Tools_UTF8Iterator&) reference to iterator at new position
    ----------------------------------------------------------------------*/
    Tools_UTF8Iterator& operator++()
    {
        return Advance(1);
    }

    /*!---------------------------------------------------------------------
    function:     operator--()
    description:  Prefix decrement; steps back iterator to previous character
    arguments:    none
    return value: (Tools_UTF8Iterator&) reference to iterator at new position
    ----------------------------------------------------------------------*/
    Tools_UTF8Iterator& operator--()
    {
        return StepBack(1);
    }

    /*!---------------------------------------------------------------------
    function:     operator++(int)
    description:  Postfix increment; advances iterator to next character
    arguments:    none
    return value: (Tools_UTF8Iterator) iterator object at old position
    ----------------------------------------------------------------------*/
    Tools_UTF8Iterator operator++(int)
    {
        Tools_UTF8Iterator saveiter(m_Pos);
        operator++();
        return saveiter;
    }

    /*!---------------------------------------------------------------------
    function:     operator--(int)
    description:  Postfix decrement; steps back iterator to previous character
    arguments:    none
    return value: (Tools_UTF8Iterator) iterator object at old position
    ----------------------------------------------------------------------*/
    Tools_UTF8Iterator operator--(int)
    {
        Tools_UTF8Iterator saveiter(m_Pos);
        operator--();
        return saveiter;
    }

    /*!---------------------------------------------------------------------
    function:     operator+()
    description:  Returns an iterator which is positioned 'count' string
                  elements behind the present iterator.

                  - The present iterator remains unchanged.
    arguments:    count [in]   number of string elements
    return value: (Tools_UTF8Iterator) iterator object at new position
    ----------------------------------------------------------------------*/
    Tools_UTF8Iterator operator+
        (SAPDB_UInt                     count) const
    {
        Tools_UTF8Iterator result(*this);
        return result.Advance(count);
    }

    /*!---------------------------------------------------------------------
    function:     operator-()
    description:  Returns an iterator which is positioned 'count' string
                  elements before the present iterator.

                  - The present iterator remains unchanged.
    arguments:    count [in]   number of string elements
    return value: (Tools_UTF8Iterator) iterator object at new position
    ----------------------------------------------------------------------*/
    Tools_UTF8Iterator operator-
        (SAPDB_UInt                     count) const
    {
        Tools_UTF8Iterator result(*this);
        return result.StepBack(count);
    }

    /*!---------------------------------------------------------------------
    function:     operator+=()
    description:  Advances the present iterator by 'count' characters.
    arguments:    count [in]   number of string elements
    return value: (Tools_UTF8Iterator&)  reference to iterator at new position
    ----------------------------------------------------------------------*/
    Tools_UTF8Iterator& operator+=
       (SAPDB_UInt                     count)
    {
        return Advance(count);
    }

    /*!---------------------------------------------------------------------
    function:     operator-=()
    description:  Steps back the present iterator by 'count' characters.
    arguments:    count [in]   number of string elements
    return value: (Tools_UTF8Iterator&)  reference to iterator at new position
    ----------------------------------------------------------------------*/
    Tools_UTF8Iterator& operator-=
       (SAPDB_UInt                     count)
    {
        return StepBack(count);
    }

private:

    /*!---------------------------------------------------------------------
    function:     Advance()
    description:  Advances the present iterator by 'count' string elements
    arguments:    count [in]   number of string elements
    return value: (Tools_UTF8Iterator&) reference to iterator at new position
    ----------------------------------------------------------------------*/
    Tools_UTF8Iterator& Advance
        (SAPDB_UInt                     count)
    {
        assert(IsAssigned());

        while (count-- > 0)
            m_Pos += Tools_UTF8Basis::CharacterSize(m_Pos) == 0 ? 1 : Tools_UTF8Basis::CharacterSize(m_Pos);

        return *this;
    }

    /*!---------------------------------------------------------------------
    function:     StepBack()
    description:  Steps back the present iterator by 'count' string elements
    arguments:    count [in]   number of string elements
    return value: (Tools_UTF8Iterator&) reference to iterator at new position
    ----------------------------------------------------------------------*/
    Tools_UTF8Iterator& StepBack
        (SAPDB_UInt                     count)
    {
        assert(IsAssigned());

        while (count-- > 0)
            m_Pos -= Tools_UTF8Basis::ReverseCharacterSize(m_Pos);

        return *this;
    }

    /*!---------------------------------------------------------------------
    function:     IsAssigned()
    description:  Checks whether iterator has been assigned
    arguments:    none
    return value: (bool) true if iterator points to valid address
    ----------------------------------------------------------------------*/
    bool IsAssigned() const 
    { 
        return m_Pos != 0; 
    }

    PointerType GetElemPointer() const 
    {
        return m_Pos;
    }

    PointerType m_Pos;
};
/*!*****************************************************************************

   endclass: Tools_UTF8Iterator

*******************************************************************************/


/*!*****************************************************************************

   class: Tools_UTF8ConstIterator

   description: This is an utf-8 string element iterator which performs
                multibyte steps from utf8 element to utf8 element

*******************************************************************************/
class Tools_UTF8ConstIterator
{
public:
    friend class Tools_UTF8StringElement;
    friend class Tools_UTF8ConstReverseIterator;

    typedef SAPDB_UTF8                          ElementType;
    typedef const ElementType*                  PointerType;

    /*!---------------------------------------------------------------------
    function:     Tools_UTF8ConstIterator()
    description:  Default constructor
    arguments:    ptr [in]    start position (default = 0)
    return value: none
    ----------------------------------------------------------------------*/
    Tools_UTF8ConstIterator
        (PointerType                   ptr = 0)
        : 
        m_Pos(ptr)
    {}

    /*!---------------------------------------------------------------------
    function:     Tools_UTF8ConstIterator()
    description:  Copy constructor
    arguments:    copy [in]    iterator to be copied
    return value: none
    ----------------------------------------------------------------------*/
    Tools_UTF8ConstIterator
        (const Tools_UTF8ConstIterator& copy)
        : 
        m_Pos(copy.m_Pos)
    {}

    /*!---------------------------------------------------------------------
    function:     Tools_UTF8ConstIterator()
    description:  Constructor
    arguments:    copy [in]    iterator to be copied
    return value: none
    ----------------------------------------------------------------------*/
    Tools_UTF8ConstIterator
        (const Tools_UTF8Iterator&      copy)
        : 
        m_Pos(copy.m_Pos)
    {}

    /*!---------------------------------------------------------------------
    function:     operator=()
    description:  Assignment operator
    arguments:    iter [in]   iterator to be assigned
    return value: none
    ----------------------------------------------------------------------*/
    Tools_UTF8ConstIterator& operator=
        (const Tools_UTF8ConstIterator& iter)
    {
        m_Pos = iter.m_Pos;
        return *this;
    }

    /*!---------------------------------------------------------------------
    function:     operator==, !=
    description:  Comparison operators
    arguments:    iter [in]    iterator to be compared
    return value: (bool) 
    prototypes:   2
    ----------------------------------------------------------------------*/
    bool operator==(const Tools_UTF8ConstIterator& iter) 
    { 
        return m_Pos == iter.m_Pos; 
    }

    bool operator!=(const Tools_UTF8ConstIterator& iter) 
    { 
        return m_Pos != iter.m_Pos; 
    }

    /*!---------------------------------------------------------------------
    function:     operator*()
    description:  Pointer operator
    arguments:    none
    return value: (ElementType) reference to element
    ----------------------------------------------------------------------*/
    const ElementType& operator*() const
    {
        return *m_Pos;
    }

    /*!---------------------------------------------------------------------
    function:     operator++()
    description:  Prefix increment; advances iterator to next character
    arguments:    none
    return value: (Tools_UTF8ConstIterator&) reference to iterator at new position
    ----------------------------------------------------------------------*/
    Tools_UTF8ConstIterator& operator++()
    {
        return Advance(1);
    }

    /*!---------------------------------------------------------------------
    function:     operator--()
    description:  Prefix decrement; steps back iterator to previous character
    arguments:    none
    return value: (Tools_UTF8ConstIterator&) reference to iterator at new position
    ----------------------------------------------------------------------*/
    Tools_UTF8ConstIterator& operator--()
    {
        return StepBack(1);
    }

    /*!---------------------------------------------------------------------
    function:     operator++(int)
    description:  Postfix increment; advances iterator to next character
    arguments:    none
    return value: (Tools_UTF8ConstIterator) iterator object at old position
    ----------------------------------------------------------------------*/
    Tools_UTF8ConstIterator operator++(int)
    {
        Tools_UTF8ConstIterator saveiter(m_Pos);
        operator++();
        return saveiter;
    }

    /*!---------------------------------------------------------------------
    function:     operator--(int)
    description:  Postfix decrement; steps back iterator to previous character
    arguments:    none
    return value: (Tools_UTF8ConstIterator) iterator object at old position
    ----------------------------------------------------------------------*/
    Tools_UTF8ConstIterator operator--(int)
    {
        Tools_UTF8ConstIterator saveiter(m_Pos);
        operator--();
        return saveiter;
    }

    /*!---------------------------------------------------------------------
    function:     operator+()
    description:  Returns an iterator which is positioned 'count' string
                  elements behind the present iterator.

                  - The present iterator remains unchanged.
    arguments:    count [in]   number of string elements
    return value: (Tools_UTF8ConstIterator) iterator object at new position
    ----------------------------------------------------------------------*/
    Tools_UTF8ConstIterator operator+
        (SAPDB_UInt                     count) const
    {
        Tools_UTF8ConstIterator result(*this);
        return result.Advance(count);
    }

    /*!---------------------------------------------------------------------
    function:     operator-()
    description:  Returns an iterator which is positioned 'count' string
                  elements before the present iterator.

                  - The present iterator remains unchanged.
    arguments:    count [in]   number of string elements
    return value: (Tools_UTF8ConstIterator) iterator object at new position
    ----------------------------------------------------------------------*/
    Tools_UTF8ConstIterator operator-
        (SAPDB_UInt                     count) const
    {
        Tools_UTF8ConstIterator result(*this);
        return result.StepBack(count);
    }

    /*!---------------------------------------------------------------------
    function:     operator+=()
    description:  Advances the present iterator by 'count' characters.
    arguments:    count [in]   number of string elements
    return value: (Tools_UTF8ConstIterator&)  reference to iterator at new position
    ----------------------------------------------------------------------*/
    Tools_UTF8ConstIterator& operator+=
       (SAPDB_UInt                     count)
    {
        return Advance(count);
    }

    /*!---------------------------------------------------------------------
    function:     operator-=()
    description:  Steps back the present iterator by 'count' characters.
    arguments:    count [in]   number of string elements
    return value: (Tools_UTF8ConstIterator&)  reference to iterator at new position
    ----------------------------------------------------------------------*/
    Tools_UTF8ConstIterator& operator-=
       (SAPDB_UInt                     count)
    {
        return StepBack(count);
    }

private:

    /*!---------------------------------------------------------------------
    function:     Advance()
    description:  Advances the present iterator by 'count' string elements
    arguments:    count [in]   number of string elements
    return value: (Tools_UTF8ConstIterator&) reference to iterator at new position
    ----------------------------------------------------------------------*/
    Tools_UTF8ConstIterator& Advance
        (SAPDB_UInt                     count)
    {
        assert(IsAssigned());

        while (count-- > 0)
            m_Pos += Tools_UTF8Basis::CharacterSize(m_Pos) == 0 ? 1 : Tools_UTF8Basis::CharacterSize(m_Pos);

        return *this;
    }

    /*!---------------------------------------------------------------------
    function:     StepBack()
    description:  Steps back the present iterator by 'count' string elements
    arguments:    count [in]   number of string elements
    return value: (Tools_UTF8Iterator&) reference to iterator at new position
    ----------------------------------------------------------------------*/
    Tools_UTF8ConstIterator& StepBack
        (SAPDB_UInt                     count)
    {
        assert(IsAssigned());

        while (count-- > 0)
            m_Pos -= Tools_UTF8Basis::ReverseCharacterSize(m_Pos);

        return *this;
    }

    /*!---------------------------------------------------------------------
    function:     IsAssigned()
    description:  Checks whether iterator has been assigned
    arguments:    none
    return value: (bool) true if iterator points to valid address
    ----------------------------------------------------------------------*/
    bool IsAssigned() const 
    { 
        return m_Pos != 0; 
    }

    PointerType GetElemPointer() const 
    {
        return m_Pos;
    }

    PointerType m_Pos;
};
/*!*****************************************************************************

   endclass: Tools_UTF8ConstIterator

*******************************************************************************/


/*!*****************************************************************************

   class: Tools_UTF8ReverseIterator

   description: This is an utf-8 string element iterator which performs
                reverse multibyte steps from utf8 element to utf8 element

*******************************************************************************/
class Tools_UTF8ReverseIterator
{
public:
    friend class Tools_UTF8ConstReverseIterator;
    friend class Tools_UTF8StringElement;

    typedef Tools_UTF8Iterator::ElementType           ElementType;
    typedef Tools_UTF8Iterator::PointerType           PointerType;

    /*!---------------------------------------------------------------------
    function:     Tools_UTF8ReverseIterator()
    description:  Default constructor
    arguments:    pos [in]    start position (default = 0)
    return value: none
    ----------------------------------------------------------------------*/
    Tools_UTF8ReverseIterator
        (PointerType                   pos = 0)
        : 
        m_Iter(pos)
    {}

    /*!---------------------------------------------------------------------
    function:     Tools_UTF8ReverseIterator()
    description:  Copy constructor
    arguments:    copy [in]    iterator to be copied
    return value: none
    ----------------------------------------------------------------------*/
    Tools_UTF8ReverseIterator
        (const Tools_UTF8ReverseIterator& copy)
        : 
        m_Iter(copy.m_Iter)
    {}

    /*!---------------------------------------------------------------------
    function:     operator=()
    description:  Assignment operator
    arguments:    iter [in]   iterator to be assigned
    return value: none
    ----------------------------------------------------------------------*/
    Tools_UTF8ReverseIterator& operator=
        (const Tools_UTF8ReverseIterator& iter)
    {
        m_Iter = iter.m_Iter;
        return *this;
    }

    /*!---------------------------------------------------------------------
    function:     operator==, !=
    description:  Comparison operators
    arguments:    iter [in]    iterator to be compared
    return value: (bool) 
    prototypes:   2
    ----------------------------------------------------------------------*/
    bool operator==(const Tools_UTF8ReverseIterator& iter) 
    { 
        return m_Iter == iter.m_Iter; 
    }
    
    bool operator!=(const Tools_UTF8ReverseIterator& iter) 
    { 
        return m_Iter != iter.m_Iter; 
    }

    /*!---------------------------------------------------------------------
    function:     operator*()
    description:  Pointer operator
    arguments:    none
    return value: (ElementType) reference to previous element
    ----------------------------------------------------------------------*/
    ElementType& operator*()
    {
        return *(m_Iter - 1);
    }

    /*!---------------------------------------------------------------------
    function:     operator++()
    description:  Prefix increment; advances iterator to next character
    arguments:    none
    return value: (Tools_UTF8ReverseIterator&) reference to iterator at new position
    ----------------------------------------------------------------------*/
    Tools_UTF8ReverseIterator& operator++()
    {
        --m_Iter;
        return *this;
    }

    /*!---------------------------------------------------------------------
    function:     operator--()
    description:  Prefix increment; steps back iterator to previous character
    arguments:    none
    return value: (Tools_UTF8ReverseIterator&) reference to iterator at new position
    ----------------------------------------------------------------------*/
    Tools_UTF8ReverseIterator& operator--()
    {
        ++m_Iter;
        return *this;
    }

    /*!---------------------------------------------------------------------
    function:     operator++(int)
    description:  Postfix increment; advances iterator to next character
    arguments:    none
    return value: (Tools_UTF8ReverseIterator) iterator object at old position
    ----------------------------------------------------------------------*/
    Tools_UTF8ReverseIterator operator++(int)
    {
        Tools_UTF8ReverseIterator save = *this;
        --m_Iter;
        return save;
    }

    /*!---------------------------------------------------------------------
    function:     operator--(int)
    description:  Postfix increment; steps back iterator to previous character
    arguments:    none
    return value: (Tools_UTF8ReverseIterator) iterator object at old position
    ----------------------------------------------------------------------*/
    Tools_UTF8ReverseIterator operator--(int)
    {
        Tools_UTF8ReverseIterator save = *this;
        ++m_Iter;
        return save;
    }

    /*!---------------------------------------------------------------------
    function:     operator+()
    description:  Returns an iterator which is positioned 'count' string
                  elements behind the present iterator.

                  - The present iterator remains unchanged.
    arguments:    count [in]   number of string elements
    return value: (Tools_UTF8ReverseIterator) iterator object at new position
    ----------------------------------------------------------------------*/
    Tools_UTF8ReverseIterator operator+
        (SAPDB_UInt                     count) const
    {
        Tools_UTF8ReverseIterator tmp = *this;
        return tmp -= count;
    }

    /*!---------------------------------------------------------------------
    function:     operator-()
    description:  Returns an iterator which is positioned 'count' string
                  elements before the present iterator.

                  - The present iterator remains unchanged.
    arguments:    count [in]   number of string elements
    return value: (Tools_UTF8ReverseIterator) iterator object at new position
    ----------------------------------------------------------------------*/
    Tools_UTF8ReverseIterator operator-
        (SAPDB_UInt                     count) const
    {
        Tools_UTF8ReverseIterator tmp = *this;
        return tmp += count;
    }

    /*!---------------------------------------------------------------------
    function:     operator+=()
    description:  Advances the present iterator by 'count' characters.
    arguments:    count [in]   number of string elements
    return value: (Tools_UTF8ReverseIterator&) reference to iterator at new position
    ----------------------------------------------------------------------*/
    Tools_UTF8ReverseIterator& operator+=
       (SAPDB_UInt                      count)
    {
        m_Iter -= count;
        return *this;
    }

    /*!---------------------------------------------------------------------
    function:     operator-=()
    description:  steps back the present iterator by 'count' characters.
    arguments:    count [in]   number of string elements
    return value: (Tools_UTF8ReverseIterator&) reference to iterator at new position
    ----------------------------------------------------------------------*/
    Tools_UTF8ReverseIterator& operator-=
       (SAPDB_UInt                      count)
    {
        m_Iter += count;
        return *this;
    }

private:

    PointerType GetElemPointer() const 
    {
        return (m_Iter - 1).GetElemPointer();
    }

    Tools_UTF8Iterator m_Iter;
};
/*!*****************************************************************************

   endclass: Tools_UTF8ReverseIterator

*******************************************************************************/


/*!*****************************************************************************

   class: Tools_UTF8ConstReverseIterator

   description: This is an utf-8 string element iterator which performs
                reverse multibyte steps from utf8 element to utf8 element

*******************************************************************************/
class Tools_UTF8ConstReverseIterator
{
public:
    friend class Tools_UTF8StringElement;

    typedef Tools_UTF8ConstIterator::ElementType      ElementType;
    typedef Tools_UTF8ConstIterator::PointerType      PointerType;

    /*!---------------------------------------------------------------------
    function:     Tools_UTF8ConstReverseIterator()
    description:  Default constructor
    arguments:    ptr [in]    start position (default = 0)
    return value: none
    ----------------------------------------------------------------------*/
    Tools_UTF8ConstReverseIterator
        (PointerType                   ptr = 0)
        : 
        m_Iter(ptr)
    {}

    /*!---------------------------------------------------------------------
    function:     Tools_UTF8ConstReverseIterator()
    description:  Copy constructor
    arguments:    copy [in]    iterator to be copied
    return value: none
    ----------------------------------------------------------------------*/
    Tools_UTF8ConstReverseIterator
        (const Tools_UTF8ConstReverseIterator& copy)
        : 
        m_Iter(copy.m_Iter)
    {}

    /*!---------------------------------------------------------------------
    function:     Tools_UTF8ConstReverseIterator()
    description:  Constructor
    arguments:    copy [in]    iterator to be copied
    return value: none
    ----------------------------------------------------------------------*/
    Tools_UTF8ConstReverseIterator
        (const Tools_UTF8ReverseIterator& copy)
        : 
        m_Iter(copy.m_Iter)
    {}

    /*!---------------------------------------------------------------------
    function:     operator=()
    description:  Assignment operator
    arguments:    iter [in]   iterator to be assigned
    return value: none
    ----------------------------------------------------------------------*/
    Tools_UTF8ConstReverseIterator& operator=
        (const Tools_UTF8ConstReverseIterator& iter)
    {
        m_Iter = iter.m_Iter;
        return *this;
    }

    /*!---------------------------------------------------------------------
    function:     operator==, !=
    description:  Comparison operators
    arguments:    iter [in]    iterator to be compared
    return value: (bool) 
    prototypes:   2
    ----------------------------------------------------------------------*/
    bool operator==(const Tools_UTF8ConstReverseIterator& iter) 
    { 
        return m_Iter == iter.m_Iter; 
    }

    bool operator!=(const Tools_UTF8ConstReverseIterator& iter) 
    { 
        return m_Iter != iter.m_Iter; 
    }

    /*!---------------------------------------------------------------------
    function:     operator*()
    description:  Pointer operator
    arguments:    none
    return value: (ElementType) reference to previous element
    ----------------------------------------------------------------------*/
    const ElementType& operator*() const
    {
        return *(m_Iter - 1);
    }

    /*!---------------------------------------------------------------------
    function:     operator++()
    description:  Prefix increment; advances iterator to next character
    arguments:    none
    return value: (Tools_UTF8ConstReverseIterator&) reference to iterator at new position
    ----------------------------------------------------------------------*/
    Tools_UTF8ConstReverseIterator& operator++()
    {
        --m_Iter;
        return *this;
    }

    /*!---------------------------------------------------------------------
    function:     operator--()
    description:  Prefix increment; steps back iterator to previous character
    arguments:    none
    return value: (Tools_UTF8ConstReverseIterator&) reference to iterator at new position
    ----------------------------------------------------------------------*/
    Tools_UTF8ConstReverseIterator& operator--()
    {
        ++m_Iter;
        return *this;
    }

    /*!---------------------------------------------------------------------
    function:     operator++(int)
    description:  Postfix increment; advances iterator to next character
    arguments:    none
    return value: (Tools_UTF8ConstReverseIterator) iterator object at old position
    ----------------------------------------------------------------------*/
    Tools_UTF8ConstReverseIterator operator++(int)
    {
        Tools_UTF8ConstReverseIterator save = *this;
        --m_Iter;
        return save;
    }

    /*!---------------------------------------------------------------------
    function:     operator--(int)
    description:  Postfix increment; steps back iterator to previous character
    arguments:    none
    return value: (Tools_UTF8ConstReverseIterator) iterator object at old position
    ----------------------------------------------------------------------*/
    Tools_UTF8ConstReverseIterator operator--(int)
    {
        Tools_UTF8ConstReverseIterator save = *this;
        ++m_Iter;
        return save;
    }

    /*!---------------------------------------------------------------------
    function:     operator+()
    description:  Returns an iterator which is positioned 'count' string
                  elements behind the present iterator.

                  - The present iterator remains unchanged.
    arguments:    count [in]   number of string elements
    return value: (Tools_UTF8ConstReverseIterator) iterator object at new position
    ----------------------------------------------------------------------*/
    Tools_UTF8ConstReverseIterator operator+
        (SAPDB_UInt                     count) const
    {
        Tools_UTF8ConstReverseIterator tmp = *this;
        return tmp -= count;
    }

    /*!---------------------------------------------------------------------
    function:     operator-()
    description:  Returns an iterator which is positioned 'count' string
                  elements before the present iterator.

                  - The present iterator remains unchanged.
    arguments:    count [in]   number of string elements
    return value: (Tools_UTF8ConstReverseIterator) iterator object at new position
    ----------------------------------------------------------------------*/
    Tools_UTF8ConstReverseIterator operator-
        (SAPDB_UInt                     count) const
    {
        Tools_UTF8ConstReverseIterator tmp = *this;
        return tmp += count;
    }

    /*!---------------------------------------------------------------------
    function:     operator+=()
    description:  Advances the present iterator by 'count' characters.
    arguments:    count [in]   number of string elements
    return value: (Tools_UTF8ConstReverseIterator&) reference to iterator at new position
    ----------------------------------------------------------------------*/
    Tools_UTF8ConstReverseIterator& operator+=
       (SAPDB_UInt                      count)
    {
        m_Iter -= count;
        return *this;
    }

    /*!---------------------------------------------------------------------
    function:     operator-=()
    description:  steps back the present iterator by 'count' characters.
    arguments:    count [in]   number of string elements
    return value: (Tools_UTF8ConstReverseIterator&) reference to iterator at new position
    ----------------------------------------------------------------------*/
    Tools_UTF8ConstReverseIterator& operator-=
       (SAPDB_UInt                      count)
    {
        m_Iter += count;
        return *this;
    }


private:

    PointerType GetElemPointer() const 
    {
        return (m_Iter - 1).GetElemPointer();
    }

    Tools_UTF8ConstIterator m_Iter;
};
/*!*****************************************************************************

   endclass: Tools_UTF8ConstReverseIterator

*******************************************************************************/





/*!*****************************************************************************

    class: Tools_UTF8StringElement

    description: Characteristics of UTF-8 string elements

*******************************************************************************/
class Tools_UTF8StringElement
{
public:
    /*!-------------------------------------------------------------------------
    declaration : BasisElementType 
    description:  Atomic type of the string element. Each utf-8 string element 
                  is physically represented by 1-6 BasisElementType instances.
    --------------------------------------------------------------------------*/
    typedef SAPDB_UTF8                                  BasisElementType;
    /*!-------------------------------------------------------------------------
    declaration : Pointer
    description:  Pointer to element
    --------------------------------------------------------------------------*/
    typedef BasisElementType*                           Pointer;
    /*!-------------------------------------------------------------------------
    declaration : ConstPointer
    description:  Pointer to constant element
    --------------------------------------------------------------------------*/
    typedef const BasisElementType*                     ConstPointer;
    /*!-------------------------------------------------------------------------
    declaration : Iterator
    description:  Iterator referencing element
    --------------------------------------------------------------------------*/
    typedef Tools_UTF8Iterator                          Iterator;
    /*!-------------------------------------------------------------------------
    declaration : ConstIterator
    description:  Iterator referencing constant element
    --------------------------------------------------------------------------*/
    typedef Tools_UTF8ConstIterator                     ConstIterator;
    /*!-------------------------------------------------------------------------
    declaration : ReverseIterator
    description:  Reverse iterator referencing element
    --------------------------------------------------------------------------*/
    typedef Tools_UTF8ReverseIterator                   ReverseIterator;
    /*!-------------------------------------------------------------------------
    declaration : ConstReverseIterator
    description:  Reverse iterator referencing constant element
    --------------------------------------------------------------------------*/
    typedef Tools_UTF8ConstReverseIterator              ConstReverseIterator;


    /*!-------------------------------------------------------------------------
    function:     IsBasic()
    description:  Checks whether string element type is a basic type
    arguments:    none
    return value: false -  this type is not basic!
    --------------------------------------------------------------------------*/
    static bool IsBasic()                       
    { 
        return false; 
    }

    /*!-------------------------------------------------------------------------
    function:     IsValid()
    description:  Checks whether a string element is valid

                  - It is checked whether the string element located at
                    'iter' has a value that is within the domain of the string
                    element type
    arguments:    iter  [in]    iterator at string element
    return value: true if valid
    prototypes:   2
    --------------------------------------------------------------------------*/
    static bool IsValid
        (ConstIterator                  iter)
    { 
        assert(ToPtr(iter) != 0);
        return Tools_UTF8Basis::CharacterSize(ToPtr(iter)) != 0;
    }

    static bool IsValid
        (ConstReverseIterator           iter)
    { 
        assert(ToPtr(iter) != 0);
        return Tools_UTF8Basis::CharacterSize(ToPtr(iter)) != 0;
    }

    /*!-------------------------------------------------------------------------
    function:     Size()
    description:  Returns the size of the next utf8 character

                  - The size in bytes of the utf8 character located
                    at 'iter' is returned. 
    arguments:    iter  [in]    iterator at string element
    return value: size in bytes
    --------------------------------------------------------------------------*/
    static SAPDB_UInt Size
        (ConstIterator                  iter)  
    { 
        assert(ToPtr(iter) != 0);
        return Tools_UTF8Basis::CharacterSize(ToPtr(iter));
    }

    /*!-------------------------------------------------------------------------
    function:     ReverseSize()
    description:  Returns the size of the previous utf8 character

                  - The size in bytes of the utf8 character ending
                    before 'ptr' is returned.
    arguments:    iter  [in]    iterator at string element
    return value: size in bytes
    --------------------------------------------------------------------------*/
    static SAPDB_UInt ReverseSize
        (ConstIterator                  iter)  
    { 
        assert(ToPtr(iter) != 0);
        return Tools_UTF8Basis::ReverseCharacterSize(ToPtr(iter));
    }

    /*!-------------------------------------------------------------------------
    function:     IterDiff()
    description:  Returns the distance of two iterators

                  - The size in basis elements of the distance of iterators
                    'from' and 'to' is returned.
    arguments:    from  [in]    first iterator
                  to    [in]    second iterator
    return value: size in basis elements of iterator distance
    --------------------------------------------------------------------------*/
    static SAPDB_Int IterDiff
        (ConstIterator                  from, 
         ConstIterator                  to) 
    { 
        return (SAPDB_Int) (ToPtr(to) - ToPtr(from)); 
    }

    /*!---------------------------------------------------------------------
    function:     ToPtr()
    description:  Converts iterator to pointer
    arguments:    iter [in]    iterator to be converted
    return value: (ConstPointer/Pointer) 
    prototypes.   4
    ----------------------------------------------------------------------*/
    static ConstPointer ToPtr
        (ConstIterator                  iter)
    {
        return iter.GetElemPointer();
    }

    static Pointer ToPtr
        (Iterator                       iter)
    {
        return iter.GetElemPointer();
    }

    static ConstPointer ToPtr
        (ConstReverseIterator           iter)
    {
        return iter.GetElemPointer();
    }

    static Pointer ToPtr
        (ReverseIterator                iter)
    {
        return iter.GetElemPointer();
    }

    /*!-------------------------------------------------------------------------
    function:     NullTerminator()
    description:  Returns a null terminator for the string element type
    arguments:    none
    return value: (BasisElementType) null terminator
    --------------------------------------------------------------------------*/
    static BasisElementType NullTerminator()           
    { 
        return BasisElementType(0); 
    }

    /*!-------------------------------------------------------------------------
    function:     Compare()
    description:  Compares two memory blocks
    
                  - A lexical comparison of two memory blocks is performed.
                  - The memory blocks are starting at 'lhs' and 'rhs', respectively.
                  - Both memory blocks have the same count of 'size' basis elements.
    arguments:    lhs    [in]    pointer to first memory block
                  rhs    [in]    pointer to second memory block
                  size   [in]    size in basis elements of both memory blocks
    return value: -  0, if memory blocks are equal
                  - -1, if 'lhs' is lexically smaller than 'rhs'
                  - +1, if 'lhs' is lexically greater than 'rhs'
    --------------------------------------------------------------------------*/
    static int Compare
        (ConstPointer                   lhs, 
         ConstPointer                   rhs, 
         SAPDB_UInt                     size)
    {
        return memcmp(lhs, rhs, size * sizeof(BasisElementType));
	}

    /*!-------------------------------------------------------------------------
    function:     Compare()
    description:  Compares two elements
    
                  - A lexical comparison of two elements is performed.
                  - The elements is pointed to by 'lhs' and 'rhs', respectively.
    arguments:    lhs    [in]    pointer to first element
                  rhs    [in]    pointer to second element
    return value: -  0, if elements are equal
                  - -1, if 'lhs' is lexically smaller than 'rhs'
                  - +1, if 'lhs' is lexically greater than 'rhs'
    --------------------------------------------------------------------------*/
    static int Compare
        (ConstPointer                   lhs, 
         ConstPointer                   rhs)
    {
        SAPDB_UInt imax = Size(lhs);
        for (SAPDB_UInt i = 0; i < imax; ++i, ++lhs, ++rhs)
		{
            if (*lhs != *rhs)
                return (*lhs < *rhs) ? -1 : 1;
		}
        return 0; 
	}

};
/*!*****************************************************************************

    endclass: Tools_UTF8StringElement

*******************************************************************************/




#endif // TOOLS_UTF8STRINGELEMENT_HPP


