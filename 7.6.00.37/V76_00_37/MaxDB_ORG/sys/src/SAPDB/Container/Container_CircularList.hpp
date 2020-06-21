/*!
  @file           Container_CircularList.hpp
  @author         TillL, UweH
  @brief          Class declaration and definition for template list classes
                  (doubly linked circular lists)
  @see            Container_DoubleNode.html
                  Container_DoubleNodeIterator.html

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


#ifndef CONTAINER_CIRCULARLIST_HPP
#define CONTAINER_CIRCULARLIST_HPP

#include <memory.h>

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "Container/Container_DoubleNode.hpp"
#include "Container/Container_DoubleNodeIterator.hpp"


/*!
  @class          Container_CircularList
  @brief          This class implements a doubly linked circular list

  template arguments <T> type of the elements to be held in the list

	T must provide the following methods:
    - Default constructor T()
    - SAPDB_Bool operator==(const T& t) const

   The list uses raw allocators as defined in the SAPDBMem component
   for allocating memory for the list elements and the link pointers.
   As of release 7.4, no exceptions are thrown.
   Instead, SAPDB_Bool return values or invalid iterators are returned by
   the methods where appropriate.
 */

template <class T>
class Container_CircularList
{
private:

   typedef Container_DoubleNode<T>   NodeType;
   typedef Container_CircularList<T> CircularList;

public:
	/// Node content type
    typedef T                                               ValueType;
	/// Pointer to node
    typedef NodeType*                                       NodePointer;
	/// List iterator
    typedef Container_DoubleNodeIterator
            <ValueType, ValueType, NodeType* >              Iterator;
	/// Constant list iterator
    typedef Container_DoubleNodeIterator
            <ValueType, const ValueType, const NodeType* >  ConstIterator;
	/// List reverse iterator
    typedef Container_DoubleNodeReverseIterator
            <ValueType, ValueType, NodeType* >              ReverseIterator;
	/// Constant list reverse iterator
    typedef Container_DoubleNodeReverseIterator
            <ValueType, const ValueType, const NodeType* >  ConstReverseIterator;
	/// Count of nodes (elements) in a list
    typedef SAPDB_UInt                                      SizeType;
	/// allocator for queue entries
    typedef SAPDBMem_IRawAllocator                          Allocator;

public:
    /*!
       @brief Constructor
       @param alloc [in] reference to raw allocator
     */
    Container_CircularList
        (Allocator&                     alloc)
        : 
         m_Allocator (alloc), 
         m_Anchor    (0),
         m_NodeCount (0)
    {}
    /*!
       @brief  Copy constructor
       @param  list [in] list to be copied
       In 7.4, this copy constructor may be called only if the list to be copied
       has not yet performed any memory allocation.
     */
    Container_CircularList
        (const CircularList&            list)
        :
         m_Allocator (list.m_Allocator), 
         m_Anchor    (list.m_Anchor),
         m_NodeCount (list.m_NodeCount)
        
    { 
        SAPDBERR_ASSERT_ARGUMENT(list.IsEmpty());
        if (!list.IsEmpty())
        {
            m_Anchor    = 0;
            m_NodeCount = 0;
        }
    }

    /*!
       @brief          Initializes a list
       @param          list [in] list to be copied
       @return         (SAPDB_Bool) true if successful
       - The former list content is deleted.
       - All elements from 'list' are copied.
     */
    SAPDB_Bool Initialize
        (const CircularList&              list)
    {
        return CopyList(list);
    }

	/// Destructs the list and deallocates all elements
    ~Container_CircularList()
    {
        Delete();
    }

    /*!
       @brief          Checks whether list is empty
       @return         (SAPDB_Bool) true if empty
     */
    SAPDB_Bool IsEmpty() const
    { 
        return m_Anchor == 0; 
    }

    /*!
       @brief          Returns the actual node count of the list
       @return         (SizeType) node count
     */
    SizeType GetSize() const
    { 
        return m_NodeCount; 
    }

    /*!
       @brief          Returns the associated allocator
       @return         (Allocator&amp;) reference to allocator
     */
    Allocator& GetAllocator() const
    { 
        return m_Allocator; 
    }

    /*!
       @brief          Compares actual list with 'list'
       @param          list [in] list to be compared
       @return         (SAPDB_Bool) true if both lists have the same content
     */
    SAPDB_Bool operator==
        (const CircularList&            list) const;

    /*!
       @brief          Compares actual list with 'list'
       @param          list [in] list to be compared
       @return         (SAPDB_Bool) true if both lists have different content
     */
    SAPDB_Bool operator !=
        (const CircularList&            list) const
    { 
        return !(this->operator==(list)); 
    }

    /*!
       @brief          Inserts a new element behind the list anchor
       @param          elem [in] element to be inserted
       @return         (Iterator) iterator pointing to inserted node;
                         invalid iterator if insert not successful
     */
    Iterator Insert
        (const ValueType&               elem);

    /*!
       @brief          Inserts a new element before a given iterator position
       @param          pos [in/out] iterator insert position
       @param          elem [in] element to be inserted
       @return         (Iterator) iterator pointing to inserted node; invalid iterator if insert not successful
       - After the operation, 'pos' points to the inserted node.
     */
    Iterator Insert     
        (Iterator&                      pos, 
         const ValueType&               elem);

    /*!
       @brief          Inserts a new element behind a given iterator position
       @param          pos [in/out] iterator insert position
       @param          elem [in] element to be inserted
       @return         (Iterator) iterator pointing to inserted node;
                         invalid iterator if insert not successful
       - After the operation, 'pos' points to the inserted node.
     */
    Iterator InsertBehind
        (Iterator&                      pos, 
         const ValueType&               elem);

    /*!
       @brief          Deletes the element at a given iterator position
       @param          pos [in/out] iterator delete position

       - After the operation, 'pos' points to the node behind                  the deleted one.
     */
    void Delete
        (Iterator&                      pos);

	/// Deletes the whole list
    void Delete();

    /*!
       @brief          Returns an iterator to the anchor node
       @return         (Iterator)
     */
    Iterator Anchor()
    {
        return Iterator(m_Anchor);
    }

    /*!
       @brief          Returns an constant iterator to the anchor node
       @return         (ConstIterator)
     */
    ConstIterator Anchor() const
    {
        return ConstIterator(m_Anchor);
    }

    /*!
       @brief          Returns a reverse iterator to the anchor node
       @return         (ReverseIterator)
     */
    ReverseIterator RAnchor()
    {
        return ReverseIterator(m_Anchor);
    } 

    /*!
       @brief          Returns a reverse iterator to the anchor node
       @return         (ConstReverseIterator)
     */
    ConstReverseIterator RAnchor() const
    {
        return ConstReverseIterator(m_Anchor);
    }

    /*!
       @brief          Checks whether actual node count is the same as m_NodeCount
       @return         (bool) true if consistent
       - For test usage only.
     */
    bool NodeCountConsistent() const;

private:

    /*!
        @param list [in] list to be assigned
        @return (CircularList&) *this

        In 7.4, this assignment operator may be called only if the list to be copied has not
        yet performed any memory allocation.
    */
    CircularList& operator=
        (const CircularList&            list)
    {
        return *this;
    }

    /*!
        @brief Performs an elementwise list copy
        @param list  [in] list to be copied
        @return (SAPDB_Bool) true if successful

          - The former content of the actual list is deleted.
          - Argument list 'list' is copied element by element
            into the actual list.
    */
    SAPDB_Bool CopyList
        (const CircularList&            list);

    /*!
        @brief   Dynamically creates a new node
        @param   elem [in]     content of the new node
        @return (NodeType*) pointer to node
    */
    // !!! Attention: Please do not change the following lines
    // Otherwise a compiler error on linux is generated.
    NodeType* CreateNode
        (const ValueType&                       elem)
    {
      return new(m_Allocator) NodeType(elem);
    }
    // !!! Attention: Please do not change the lines above.


	/// Reference to underlying raw allocator
    Allocator&                                      m_Allocator;
	/// Anchor node to the list
    NodePointer                                     m_Anchor;
	/// Actual node count in the list
    SizeType                                        m_NodeCount;
};

// -----------------------------------------------------------------------------
template <class T>
SAPDB_Bool Container_CircularList<T>::operator ==
    (const CircularList&              list) const
{
    // -------------------------------------------------------------------------
    // check whether lists have the same node count
    // -------------------------------------------------------------------------
    if (m_NodeCount != list.m_NodeCount)
        return false;

    // -------------------------------------------------------------------------
    // check whether both lists are empty (lists have the same node count)
    // -------------------------------------------------------------------------
    if (IsEmpty())
        return true;

    // -------------------------------------------------------------------------
    // compare lists elementwise (lists have the same node count)
    // -------------------------------------------------------------------------
    ConstIterator iter1 = Anchor();
    ConstIterator iter2 = list.Anchor();
    SizeType      count = 0;
    while (count < m_NodeCount) 
    {
        if (!(*iter1 == *iter2)) 
            return false;
        ++iter1;
        ++iter2;
        ++count;
    }

    return true;
}

// -----------------------------------------------------------------------------

template <class T>
typename Container_CircularList<T>::Iterator 
Container_CircularList<T>::Insert
    (const ValueType&               elem)
{
    if (IsEmpty())
    {
        // -------------------------------------------------------------------------
        // create a new node
        // -------------------------------------------------------------------------
        NodeType *node  = CreateNode(elem);
        if (node == 0)
            return Iterator();

        node->m_Next = node;
        node->m_Prev = node;
        m_Anchor     = node;
        ++m_NodeCount;
        return Iterator(node);
    }
    else
    {
        Iterator iter = Anchor();
        return InsertBehind(iter, elem);
    }
}

// -----------------------------------------------------------------------------

template <class T>
typename Container_CircularList<T>::Iterator 
Container_CircularList<T>::Insert
    (Iterator&                      pos, 
     const ValueType&               elem)
{
    SAPDBERR_ASSERT_ARGUMENT(pos.IsValid());

    if (pos == Anchor())
        return Insert(elem);

    // -------------------------------------------------------------------------
    // create a new node
    // -------------------------------------------------------------------------
    NodeType *node  = CreateNode(elem);
    if (node == 0)
        return Iterator();

    // -------------------------------------------------------------------------
    // get the iterator positions before and behind the node to be inserted
    // -------------------------------------------------------------------------
    Iterator prev   = --Iterator(pos);
    Iterator next   = pos;

    // -------------------------------------------------------------------------
    // chain the new node between 'prev' and 'next'
    // -------------------------------------------------------------------------
    node->m_Next           = prev.GetNode()->m_Next;
    node->m_Prev           = next.GetNode()->m_Prev;
    prev.GetNode()->m_Next = node;
    next.GetNode()->m_Prev = node;
    pos                    = Iterator(node);

    ++m_NodeCount;
    return pos;
}

// -----------------------------------------------------------------------------

template <class T>
typename Container_CircularList<T>::Iterator 
Container_CircularList<T>::InsertBehind
    (Iterator&                      pos, 
     const ValueType&               elem)
{
    SAPDBERR_ASSERT_STATE(!IsEmpty());
    SAPDBERR_ASSERT_ARGUMENT(pos.IsValid());

    // -------------------------------------------------------------------------
    // create a new node
    // -------------------------------------------------------------------------
    NodeType *node  = CreateNode(elem);
    if (node == 0)
        return Iterator();

    // -------------------------------------------------------------------------
    // get the iterator positions before and behind the node to be inserted
    // -------------------------------------------------------------------------
    Iterator prev   = pos;
    Iterator next   = ++Iterator(pos);

    // -------------------------------------------------------------------------
    // chain the new node between 'prev' and 'next'
    // -------------------------------------------------------------------------
    node->m_Next           = prev.GetNode()->m_Next;
    node->m_Prev           = next.GetNode()->m_Prev;
    prev.GetNode()->m_Next = node;
    next.GetNode()->m_Prev = node;
    pos                    = Iterator(node);

    ++m_NodeCount;
    return pos;
} 

// -----------------------------------------------------------------------------

template <class T>
void Container_CircularList<T>::Delete
    (Iterator&                      pos)
{
    SAPDBERR_ASSERT_STATE(!IsEmpty());
    SAPDBERR_ASSERT_ARGUMENT(pos.IsValid());

    // at this point, we are sure that there is a precedessor as well as
    // a successor to 'pos' in the list
    // -------------------------------------------------------------------------
    // set anchor if its node is to be deleted
    // -------------------------------------------------------------------------
    if (pos == Anchor())
        m_Anchor = pos.GetNode()->m_Next;
    // -------------------------------------------------------------------------
    // get the iterator positions before and behind the node to be deleted
    // -------------------------------------------------------------------------
    Iterator prev = --Iterator(pos);
    Iterator next = ++Iterator(pos);

    // -------------------------------------------------------------------------
    // chain the node out of the list
    // -------------------------------------------------------------------------
    prev.GetNode()->m_Next = pos.GetNode()->m_Next;
    next.GetNode()->m_Prev = pos.GetNode()->m_Prev;

    NodeType* p = pos.GetNode();
    destroy(p, m_Allocator);
    pos = Iterator(next.GetNode());

    if (--m_NodeCount == 0)
        m_Anchor = 0;

}

// -----------------------------------------------------------------------------

template <class T>
inline
void Container_CircularList<T>::Delete()
{
    Iterator pos = Anchor();
    while (!IsEmpty()) 
    {   
        Delete(pos);    
    }
}

// -----------------------------------------------------------------------------

template <class T>
SAPDB_Bool Container_CircularList<T>::NodeCountConsistent() const
{
    SizeType nodecount = 0;
    
    if (m_Anchor != 0)
    {
        ConstIterator iter = Anchor();
        ConstIterator end  = Anchor();
        do
        {
            ++iter;
            ++nodecount;
        }
        while (iter != end);
    }

    return (nodecount == m_NodeCount);
}

// -----------------------------------------------------------------------------

template <class T>
SAPDB_Bool Container_CircularList<T>::CopyList
    (const CircularList&              list)
{
    if (list.IsEmpty())
    {
        Delete();
        return true;
    }

    // -------------------------------------------------------------------------
    // save actual list contents:
    // -------------------------------------------------------------------------
    NodeType* anchor    = m_Anchor;
    SizeType  nodecount = m_NodeCount;
    m_Anchor            = 0;
    m_NodeCount         = 0;

    // -------------------------------------------------------------------------
    // insert first element:
    // -------------------------------------------------------------------------
    ConstIterator iter = list.Anchor();
    Iterator      pos  = Insert(*iter);
    if (!pos.IsValid())
    {
        Delete();     
        return false;
    }

    // -------------------------------------------------------------------------
    // copy list elementwise:
    // -------------------------------------------------------------------------
    ConstIterator end  = iter;
    ++iter;
    while (iter != end)
    {
        if (!(InsertBehind(pos, *iter).IsValid()))
        {
            Delete();
            m_Anchor    = anchor;
            m_NodeCount = nodecount;
            return false;
        }
        ++iter;
    }

    // -------------------------------------------------------------------------
    // delete old list contents:
    // -------------------------------------------------------------------------
    CircularList del(m_Allocator);
    del.m_Anchor = anchor;
    del.m_NodeCount = nodecount;
    del.Delete();
    return true;
}

// -----------------------------------------------------------------------------

  

#endif // CONTAINER_CIRCULARLIST_HPP

