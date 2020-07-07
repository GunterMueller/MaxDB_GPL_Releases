/*!
  @file           Container_NodeIterator.hpp
  @author         TillL,UweH
  @brief          Unidirectional iterator template for singly linked list nodes

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
#ifndef CONTAINER_NODEITERATOR_HPP
#define CONTAINER_NODEITERATOR_HPP
/*!
@class          Container_NodeIterator
@brief          Unidirectional iterator template for singly linked list nodes

This iterator may be used to iterate Container_List&lt;T> 
*/
template <class T, class ValueType, class NodePointer>
class Container_NodeIterator
{
private:
    typedef Container_NodeIterator<T, ValueType, NodePointer> Iterator;
public:
    /// Constructor
    Container_NodeIterator()
    : m_Node(0) 
    {}
    /*!
    @brief          Constructor
    @param          Node [in] pointer to a list node
    */
    Container_NodeIterator (NodePointer Node)
    : m_Node(Node) 
    {}
    /*!
    @brief   Dereferences node pointer
    @return  reference to actual node content
    */
    ValueType& operator*() const
    {
        SAPDBERR_ASSERT_STATE(IsValid());
        return m_Node->m_Content;
    }
    /*!
    @brief          Allows access to members of the node content
    @return         (ValueType*) pointer to node content
     */
#   if defined(_WIN32) || defined(WIN32)
#   pragma warning(disable : 4284)
#   endif
    ValueType* operator->() const
    {
        SAPDBERR_ASSERT_STATE(IsValid());
        return &(m_Node->m_Content);
    }
#   if defined(_WIN32) || defined(WIN32)
#   pragma warning(default : 4284)
#   endif
    /*!
    @brief          Prefix increment; advances iterator to next node
    @return         (Iterator&amp;) *this
    */
    Iterator& operator++() 
    {
        SAPDBERR_ASSERT_STATE(IsValid());
        m_Node = m_Node->m_Next;
        return *this;
    }
    /*!
    @brief          Compares two iterators
    @param          iter [in] iterator to be compared with
    @return         (bool) true if both iterators point to same node
    */
    bool operator== (const Iterator &iter) const
    { 
        return m_Node == iter.m_Node; 
    }
    /*!
    @brief          Compares two iterators
    @param          iter [in] iterator to be compared with
    @return         (bool) true if both iterators point to different nodes
    */
    bool operator!= (const Iterator &iter) const
    { 
        return m_Node != iter.m_Node; 
    }
    /// returns true if node is valid
    bool IsValid() const
    {
        return m_Node != 0;
    }
	/// Returns pointer to actual node
    const NodePointer& GetNode() const
    {
        return m_Node;
    }
protected:
	/// pointer to list node
    NodePointer m_Node;
};
#endif // CONTAINER_NODEITERATOR_HPP

