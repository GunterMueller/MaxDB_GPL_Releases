/*!
  @file           Container_DoubleNodeIterator.hpp
  @author         StefanB,TillL,UweH
  @brief          Bidirectional iterator and reverse iterator templates for doubly linked list nodes

\if EMIT_LICENCE

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



\endif
*/
#ifndef CONTAINER_DOUBLENODEITERATOR_HPP
#define CONTAINER_DOUBLENODEITERATOR_HPP

#include "Container/Container_NodeIterator.hpp"
/*!
   @class          Container_DoubleNodeIterator
   @brief          Bidirectional iterator template for doubly linked list nodes.
   template arguments &lt;class T>             Type of the elements of the list.
                       &lt;class ValueType>     Type that the dereference operator returns.
                       &lt;class NodePointer>   Type of the node pointer.

   This iterator may be used to iterate Container_DoubleList&lt;T> and
   Container_CircularList&lt;T>.
*/

template <class T, class ValueType, class NodePointer>
class Container_DoubleNodeIterator :
public Container_NodeIterator<T, ValueType, NodePointer>
{
private:
    typedef Container_DoubleNodeIterator<T, ValueType, NodePointer> Iterator;
    typedef Container_NodeIterator<T, ValueType, NodePointer>       Parent;
public:
	/// Constructor
    Container_DoubleNodeIterator()
    {}
    /*!
    @brief          Constructor
    @param          Node [in] pointer to a list node
    */
    Container_DoubleNodeIterator (NodePointer Node)
    : Parent(Node) 
    {}
	/// Prefix increment; advances iterator to next node
    Iterator& operator++() 
    {
        SAPDBERR_ASSERT_STATE(this->IsValid());
        this->m_Node = this->m_Node->m_Next;
        return *this;
    }
	/// Prefix decrement; steps back iterator to previous node
    Iterator& operator--() 
    {
        SAPDBERR_ASSERT_STATE(this->IsValid());
        this->m_Node = this->m_Node->m_Prev;
        return *this;
    } 
};
/*!
@class          Container_DoubleNodeReverseIterator
@brief          Bidirectional reverse iterator template for doubly linked list nodes.
template arguments &lt;class T>             Type of the elements of the list.
                  &lt;class ValueType>     Type that the dereference operator returns.
                   &lt;class NodePointer>   Type of the node pointer.

This iterator may be used to iterate Container_DoubleList&lt;T> and
Container_CircularList&lt;T>.
*/
template <class T, class ValueType, class NodePointer>
class Container_DoubleNodeReverseIterator :
public Container_DoubleNodeIterator<T, ValueType, NodePointer>
{
private:
    typedef Container_DoubleNodeReverseIterator<T, ValueType, NodePointer> ReverseIterator;
    typedef Container_DoubleNodeIterator<T, ValueType, NodePointer>        Parent;
public:
	/// Constructor
    Container_DoubleNodeReverseIterator()
    {}
    /*!
    @brief          Constructor
    @param          Node [in] pointer to a list node
    */
    Container_DoubleNodeReverseIterator  (NodePointer Node)
    : Parent(Node) 
    {}
	/// Prefix increment; advances iterator to next node
    ReverseIterator& operator++() 
    {
        Parent::operator--();
        return *this;
    }
	/// Prefix decrement; steps back iterator to previous node
    ReverseIterator& operator--() 
    {
        Parent::operator++();
        return *this;
    }
};
#endif // CONTAINER_DOUBLENODEITERATOR_HPP
