/*!
  @file           Container_DoubleNode.hpp
  @author         TillL
  @author         UweH
  @brief          Doubly linked list node template

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
#ifndef CONTAINER_DOUBLENODE_HPP
#define CONTAINER_DOUBLENODE_HPP
/*!
@class Container_DoubleNode
@brief Node of a doubly linked list.

Contains element of type T and two pointers pointing to successor and  predecessor nodes, respectively.
*/
template <class T> class Container_DoubleNode
{
private:
    typedef Container_DoubleNode<T>  NodeType;
public:
    /// previous node in chain
    NodeType* m_Prev;
    /// next node in chain
    NodeType* m_Next;
    /// node content
    T         m_Content;
    /*!
    @brief  Constructor
    @param  content [in] content to be assigned
    Assigns content
    */
    Container_DoubleNode (const T& content)
    : m_Content(content)
    , m_Prev(0)
    , m_Next(0) 
    {}
};
#endif // CONTAINER_DOUBLENODE_HPP


