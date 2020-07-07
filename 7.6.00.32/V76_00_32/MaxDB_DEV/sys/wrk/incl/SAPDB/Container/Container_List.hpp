/*!
  @file    Container_List.hpp
  @author  StefanB, TillL, UweH
  @brief   Class declaration and definition for template list classes
          (singly linked lists)

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
#ifndef CONTAINER_LIST_HPP
#define CONTAINER_LIST_HPP

#include <memory.h>

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "Container/Container_Node.hpp"
#include "Container/Container_NodeIterator.hpp"
/*!
@class          Container_List
@brief          This class implements a doubly linked list

template arguments<T>  type of the elements to be held in the list

T must provide the following methods:

- Default constructor T()
- bool operator==(const T& t) const

The list uses raw allocators as defined in the SAPDBMem component
for allocating memory for the list elements and the link pointers.
As of release 7.4, no exceptions are thrown.
Instead, bool return values or invalid iterators are returned by
the methods where appropriate.
*/
template <class T> class Container_List
{
private:
   typedef Container_Node<T>                                                     NodeType;
   typedef Container_List<T>                                                     List;
public:
	/// Node content type
    typedef T                                                                    ValueType;
	/// Pointer to node
    typedef NodeType*                                                            NodePointer;
	/// List iterator
    typedef Container_NodeIterator<ValueType, ValueType, NodeType*>              Iterator;
	/// Constant list iterator
    typedef Container_NodeIterator<ValueType, const ValueType, const NodeType*>  ConstIterator;
public:
	/*!
    @brief Constructor
    @param alloc [in] reference to raw allocator
    */
    Container_List (SAPDBMem_IRawAllocator &alloc)
    : m_Allocator(alloc)
    , m_Head     (0)
    , m_Last     (0)
    , m_NodeCount(0)
    {}
    /*!
    @brief          Initializes a list
    @param          list [in] list to be copied
    @return         (bool) true if successful

    - The former list content is deleted.
    - All elements from 'list' are copied.
    */
    bool Initialize (const List &list)
    {
        return CopyList(list);
    }
	/// Deletes the whole list
    void Delete();
	/// Destructs the list and deallocates all elements
    ~Container_List()
    {
        Delete();
    }
	/// Checks whether list is empty, returns true if empty
    bool IsEmpty() const
    { 
        return m_Head == 0; 
    }
    /*!
    @brief          Returns the actual node count of the list
    @return         node count
    */
    SAPDB_UInt GetSize() const
    { 
        return m_NodeCount; 
    }
    /*!
    @brief          Returns the associated allocator
    @return         reference to allocator
    */
    SAPDBMem_IRawAllocator& GetAllocator() const
    { 
        return m_Allocator; 
    }
    /*!
    @brief          Compares actual list with 'list'
    @param          list [in] list to be compared
    @return         (bool) true if both lists have the same content
    */
    bool operator== (const List &list) const;
    /*!
    @brief          Compares actual list with 'list'
    @param          list [in] list to be compared
    @return         (bool) true if both lists have different content
    */
    bool operator != (const List &list) const
    { 
        return !(this->operator==(list)); 
    }
    /*!
    @brief          Inserts a new element at the beginning of the list
    @param          elem [in] element to be inserted
    @return         (Iterator) iterator pointing to inserted node;
    invalid iterator if insert not successful
    */
    Iterator InsertFront (const ValueType &elem);
    /*!
    @brief          Inserts a new element at the end of the list
    @param          elem [in] element to be inserted
    @return         (Iterator) iterator pointing to inserted node;
    invalid iterator if insert not successful
     */
    Iterator InsertEnd (const ValueType &elem);
    /*!
    @brief          Inserts a new element before a given iterator position
    @param          pos [in/out] iterator insert position
    @param          elem [in] element to be inserted
    @return         (Iterator) iterator pointing to inserted node;
	invalid iterator if insert not successful
    - After the operation, 'pos' points to the inserted node.
    - This method is rather slow, because in order to insert a list element
      at a certain position, the predecessor of that position has to be found.
    */
    Iterator Insert (Iterator        &pos, 
                     const ValueType &elem);
    /*!
    @brief          Inserts a new element behind a given iterator position
    @param          pos [in/out] iterator insert position
    @param          elem [in] element to be inserted
    @return         (Iterator) iterator pointing to inserted node;
    invalid iterator if insert not successful
	After the operation, 'pos' points to the inserted node.
    */
    Iterator InsertBehind  (Iterator        &pos, 
                            const ValueType &elem);
    /*!
    @brief Deletes the first element of the list
    The list must not be empty.
    */
    void DeleteFront();
    /*!
    @brief          Deletes the element at a given iterator position
    @param          pos [in/out] iterator delete position

    - This method is rather slow, because in order to delete a list element at a certain position, the predecessor of that                position has to be found.
    - After the operation, 'pos' points to the node behind the deleted one.
     */
    void Delete (Iterator &pos);
    /*!
    @brief          Returns an iterator to the first list node
    @return         (Iterator/ConstIterator)
    */
    Iterator Begin()
    { 
        return Iterator(m_Head); 
    }
	/// return const iter to the list begin
    ConstIterator Begin() const
    { 
        return ConstIterator(m_Head); 
    }
    /*!
    @brief          Returns an iterator to the last list node
    @return         (Iterator/ConstIterator)
    */
    Iterator Last()
    { 
        return Iterator(m_Last); 
    }
    /*!
    @brief          Returns an iterator to the last list node
    @return         (Iterator/ConstIterator)
    */
    ConstIterator Last() const
    { 
        return ConstIterator(m_Last); 
    }
    /*!
    @brief          Returns an iterator behind the last list node
    @return         (Iterator/ConstIterator)
    */
    Iterator End()
    { 
        return Iterator(0); 
    }
    /*!
    @brief          Returns an iterator behind the last list node
    @return         (Iterator/ConstIterator)
    */
    ConstIterator End() const
    { 
        return ConstIterator(0); 
    }
    /*!
    @brief          Checks whether actual node count is the same as m_NodeCount
    @return         (bool) true if consistent
    - For test usage only.
    */
    bool NodeCountConsistent() const;

private:
	/// Copy constructor
    Container_List (const List &list)
    { 
        // do nothing in 7.4
    }
	/*!
    @brief Assigns list (does not work in 7.4)
    @param   list [in]     list to be assigned
    @return (DoubleList&) *this
    - The content of 'list' is copied (duplicated).
    */
    List& operator= (const List &list)
    {
        // do nothing in 7.4
        return *this;
    }
    /*!
    @brief  Performs an elementwise list copy
    @param    list  [in] list to be copied
    @return  (bool) true if successful
                  - The former content of the actual list is deleted.
                  - Argument list 'list' is copied element by element
                    into the actual list.
    */
    bool CopyList (const List &list);
    /*!
    @brief  Dynamically creates a new node
    @param  elem [in]     content of the new node
    @return (NodeType*) pointer to node (0 if not successful)
    */
    // !!! Attention: Please do not change the following lines
    // Otherwise a compiler error on linux is generated.
    NodeType* CreateNode(const ValueType&               elem)
    {
      return new(m_Allocator) NodeType(elem);
    }
    // !!! Attention: Please do not change the lines above.
    /// Reference to underlying raw allocator
    SAPDBMem_IRawAllocator &m_Allocator;
	/// Pointer to first node in the list
    NodePointer             m_Head;
	/// Pointer to last node in the list
    NodePointer             m_Last;
	/// Actual node count in the list
    SAPDB_UInt                m_NodeCount;
};


// -----------------------------------------------------------------------------

template <class T>
inline bool Container_List<T>::operator == (const List& list) const
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
    ConstIterator iter1 = Begin();
    ConstIterator iter2 = list.Begin();
    ConstIterator end   = End();
    while (iter1 != end) 
    {
        if (!(*iter1 == *iter2)) 
            return false;
        ++iter1;
        ++iter2;
    }

    return true;
}

// -----------------------------------------------------------------------------

template <class T>
inline typename Container_List<T>::Iterator Container_List<T>::InsertFront (const ValueType &elem)
{
    // -------------------------------------------------------------------------
    // create a new node
    // -------------------------------------------------------------------------
    NodeType *node     = CreateNode(elem);
    if (node == 0)
        return Iterator();

    // -------------------------------------------------------------------------
    // chain in the node at the beginning of the list
    // -------------------------------------------------------------------------
    node->m_Next   = m_Head;
    if (IsEmpty ()) //List is empty
    {
        m_Last = node;
    }

    m_Head         = node;

    ++m_NodeCount;

    return Iterator(node);
}

// -----------------------------------------------------------------------------

template <class T>
inline typename Container_List<T>::Iterator Container_List<T>::InsertEnd ( const ValueType &elem)
{
    // -------------------------------------------------------------------------
    // create a new node
    // -------------------------------------------------------------------------
    NodeType *node     = CreateNode(elem);
    if (node == 0)
        return Iterator();

    // -------------------------------------------------------------------------
    // chain in the node at the end of the list
    // -------------------------------------------------------------------------
    if (IsEmpty())
    {
        m_Head = node;
    }
    else
    {
        m_Last->m_Next = node;
    }

    m_Last          = node;
    m_Last->m_Next  = 0;

    ++m_NodeCount;

    return Iterator(node);
}

// -----------------------------------------------------------------------------

template <class T>
inline typename Container_List<T>::Iterator Container_List<T>::Insert (Iterator        &pos, 
                                                                       const ValueType &elem)
{
    SAPDBERR_ASSERT_ARGUMENT(pos.IsValid());

    // -------------------------------------------------------------------------
    // check whether InsertFront() can be used
    // -------------------------------------------------------------------------
    if (pos == Begin()) 
    {
        // insert the new element at the beginning of the list
        if (!InsertFront(elem).IsValid())
            return Iterator();
        else
        {
            // set iterator position to new begin
            pos = Begin();
            return pos;
        }
    }
    // -------------------------------------------------------------------------
    // insert element in the middle of the list
    // -------------------------------------------------------------------------
    else
    {
        // -------------------------------------------------------------------------
        // Find the iterator position before 'pos'
        // -------------------------------------------------------------------------
        Iterator prev  = Begin();
        Iterator next = ++Begin();
        while (next != pos) 
        {
            SAPDBERR_ASSERT_STATE(next != End());      // error: 'pos' is not in list
            ++prev;
            ++next;
        }

        // -------------------------------------------------------------------------
        // create a new node
        // -------------------------------------------------------------------------
        NodeType *node = CreateNode(elem);
        if (node == 0)
            return Iterator();

        // -------------------------------------------------------------------------
        // chain the new node behind 'prev'
        // -------------------------------------------------------------------------
        node->m_Next           = prev.GetNode()->m_Next;
        prev.GetNode()->m_Next = node;

        if (pos == End ())
        {
            m_Last = node;   
        }

        // Set the iterator position.
        pos            = Iterator(node);

        ++m_NodeCount;
        return pos;
    }
}

// -----------------------------------------------------------------------------

template <class T>
inline typename Container_List<T>::Iterator Container_List<T>::InsertBehind (Iterator        &pos, 
                                                                             const ValueType &elem)
{
    SAPDBERR_ASSERT_ARGUMENT(pos.IsValid());
    SAPDBERR_ASSERT_ARGUMENT(pos != End());
    SAPDBERR_ASSERT_STATE(!IsEmpty());

    // -------------------------------------------------------------------------
    // create a new node
    // -------------------------------------------------------------------------
    NodeType *node  = CreateNode(elem);
    if (node == 0)
        return Iterator();

    Iterator prev   = pos;

    // -------------------------------------------------------------------------
    // chain the new node behind 'pos'
    // -------------------------------------------------------------------------
    node->m_Next           = prev.GetNode()->m_Next;
    prev.GetNode()->m_Next = node;
    if (pos == Last ())  // Insertion takes place at the end 
    {   
       m_Last = node;
    }

    pos                    = Iterator(node);

    ++m_NodeCount;
    return pos;
}

// -----------------------------------------------------------------------------

template <class T>
inline void Container_List<T>::DeleteFront()
{
    SAPDBERR_ASSERT_STATE(!IsEmpty());

    NodeType* node = m_Head;
    m_Head         = m_Head->m_Next;
    if (IsEmpty ())    // Last node in the list was deleted
    {
        m_Last = m_Head;
    }

    --m_NodeCount;
    destroy(node, m_Allocator);
}

// -----------------------------------------------------------------------------

template <class T>
inline void Container_List<T>::Delete (Iterator &pos)
{
    SAPDBERR_ASSERT_ARGUMENT(pos.IsValid());
    SAPDBERR_ASSERT_ARGUMENT(pos != End());
    SAPDBERR_ASSERT_STATE(!IsEmpty());

    // -------------------------------------------------------------------------
    // check whether DeleteFront() can be used
    // -------------------------------------------------------------------------
    if (pos == Begin()) 
    {
        // Delete the first list element
        DeleteFront();
        // Set the iterator position.
        pos = Begin();
    }
    // -------------------------------------------------------------------------
    // delete element from the middle of the list
    // -------------------------------------------------------------------------
    else
    {
        // -------------------------------------------------------------------------
        // Find the iterator position before 'pos'
        // -------------------------------------------------------------------------
        Iterator prev = Begin();
        Iterator next = ++Begin();
        while (next != pos) 
        {
            SAPDBERR_ASSERT_STATE(next != End());      // error: 'pos' is not in list
            ++prev;
            ++next;
        }

        // -------------------------------------------------------------------------
        // chain the node out of the list
        // -------------------------------------------------------------------------
        prev.GetNode()->m_Next = pos.GetNode()->m_Next;
        if (pos == Last())
        {
            m_Last = prev.GetNode();
        }

        NodePointer p = pos.GetNode();
        destroy(p, m_Allocator);
        pos = prev.GetNode()->m_Next;
        --m_NodeCount;
    }
}

// -----------------------------------------------------------------------------

template <class T>
inline void Container_List<T>::Delete()
{
   while (!IsEmpty()) 
   {
      DeleteFront();
   }
}

// -----------------------------------------------------------------------------

template <class T>
inline bool Container_List<T>::NodeCountConsistent() const
{
     ConstIterator iter    = Begin();
     ConstIterator enditer = End();
     SAPDB_UInt nodecount    = 0;
     
     while (iter != enditer)
     {
          ++iter;
          ++nodecount;
     }
     return (nodecount == m_NodeCount);
}


// -----------------------------------------------------------------------------

template <class T>
inline bool Container_List<T>::CopyList (const List&                    list)
{
    // -------------------------------------------------------------------------
    // save actual list contents:
    // -------------------------------------------------------------------------
    NodeType* head      = m_Head;
    NodeType* last      = m_Last;
    SAPDB_UInt  nodecount = m_NodeCount;

    // -------------------------------------------------------------------------
    // copy list elementwise:
    // -------------------------------------------------------------------------
    m_Head              = 0;
    m_Last              = 0;
    m_NodeCount         = 0;
    ConstIterator iter  = list.Begin();
    ConstIterator end   = list.End();
    while (iter != end)
    {
        if (!(InsertEnd(*iter).IsValid()))
        {
            Delete();
            m_Head      = head;
            m_Last      = last;
            m_NodeCount = nodecount;
            return false;
        }
        ++iter;
    }

    // -------------------------------------------------------------------------
    // delete old list contents:
    // -------------------------------------------------------------------------
    List del(m_Allocator);
    del.m_Head      = head;
    del.m_Last      = last;
    del.m_NodeCount = nodecount;
    del.Delete();
    return true;
}
#endif // CONTAINER_LIST_HPP

