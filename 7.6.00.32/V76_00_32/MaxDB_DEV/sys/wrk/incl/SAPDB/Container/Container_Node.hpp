/*!
  @file           Container_Node.hpp
  @author         StefanB, TillL, UweH
  @brief          Singly linked list node template

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
#ifndef CONTAINER_NODE_HPP
#define CONTAINER_NODE_HPP
/*!
@class          Container_Node
@brief          Node of a singly linked list.

template arguments<T>            node content type
Contains element of type T and a pointer pointing to successor node. 
*/
template <class T> class Container_Node
{
private:
    typedef Container_Node<T>  NodeType;
public:
    /// pointer to next node
    NodeType* m_Next;
    /// content of current node
    T         m_Content;
    /*!
    @brief          Constructor
    @param          content [in] content to be assigned
    */
    Container_Node (const T &content)
    : m_Content(content)
    , m_Next(0) 
    {}
};
#endif // CONTAINER_NODE_HPP


