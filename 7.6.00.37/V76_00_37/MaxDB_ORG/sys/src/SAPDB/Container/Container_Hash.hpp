/*!
  @file           Container_Hash.hpp
  @author         Ivan Schreter, ContainerMaint
  @brief          Class declaration and definition for template hash classes

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


#ifndef CONTAINER_HASH_HPP
#define CONTAINER_HASH_HPP

#include <memory.h>

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
//#include "Container/Container_HashNode.hpp"
//#include "Container/Container_HashNodeIterator.hpp"


// forwards for friend declarations

template <class Key, class T, bool AllowDuplicates = false>
class Container_Hash;

template <class Key, class T, class RT, bool AllowDuplicates = false>
class Container_HashNodeIterator;



/*!
   @brief          This template implements default hash function for hashtable. If you need
special hashing, create a specialized implementation for your key class.
   template arguments &lt;class KeyType>     Type of the hash key
   @param          k [in] key to hash
   @return         hash value as unsigned integer

 */

template <class KeyType>
SAPDB_UInt Container_HashFnc(const KeyType &k)
{
    const SAPDB_UInt1 *data = (const SAPDB_UInt1*) &k;
    SAPDB_UInt sum = 0;
    unsigned int shift = 0, count = sizeof(k);
    while (count--) 
    {
        sum ^= *data++ << shift;
        shift += 8;
        if (shift >= sizeof(int) * 8) shift = 0;
    }
    return sum;
}

/*!
   @brief          This template implements default comparison function for hashtable. If you need
special comparison, create a specialized implementation for your key class.
   template arguments &lt;class KeyType>     Type of the hash key
   @param          key1 [in] key to compare with key2
   @param          key2 [in] key to compare with key1
   @return         result of the comparison as bool (true if both are equal, otherwise false)

 */

template <class KeyType>
bool Container_CompareFnc(const KeyType &key1, const KeyType &key2)
{
   return key1==key2;
}



/*!
  @class          Container_HashNode
   @brief          Hash node for hashtable. Used internally by Container_Hash.
   template arguments &lt;class Key>             Type of the hash key
                       &lt;class T>               Type of the elements of the list.

 */

template <class Key, class T>
class Container_HashNode
{
public:
    const Key       key;
    T               value;

    Container_HashNode(const Key &k, const T &v)
      : key(k), value(v)
    {
    }
    ~Container_HashNode()
    {
    }

protected:
    friend class Container_Hash<Key, T, false>;
    friend class Container_Hash<Key, T, true>;
    friend class Container_HashNodeIterator<Key, T, T, false>;
    friend class Container_HashNodeIterator<Key, T, T, true>;
    friend class Container_HashNodeIterator<Key, T, const T, false>;
    friend class Container_HashNodeIterator<Key, T, const T, true>;

    Container_HashNode<Key, T>  *next;
    Container_HashNode<Key, T>  *prev;
};





/*!
   @class          Container_HashNodeIterator
   @brief          Bidirectional iterator template for hashtables
   template arguments &lt;class Key>             Type of the hash key
                       &lt;class T>               Type of the elements of the list.
                       &lt;class RT>              Type that the dereference operator returns.
                       &lt;class AllowDuplicates> true, if allowing duplicate keys


   This iterator may be used to iterate Container_Hash 

 */


template <class Key, class T, class RT, bool AllowDuplicates>
class Container_HashNodeIterator
{
private:
    typedef Container_HashNode<Key, T>                      NodeType;
    typedef Container_HashNode<Key, RT>                     RNodeType;
    typedef SAPDB_UInt                                      SizeType;
    typedef Container_Hash<Key, T, AllowDuplicates>         Hash;

    typedef Container_HashNodeIterator
            <Key, T, RT, AllowDuplicates>                   Iterator;

protected:

    Container_HashNodeIterator(Hash *t, SizeType b, NodeType *n)
      : m_Table(t), m_Bucket(b), m_Node(n)
    {
    }

public:
    Container_HashNodeIterator()
      : m_Table(NULL), m_Bucket(0), m_Node(NULL)
    {
    }

    Container_HashNodeIterator(const Iterator &iter)
      : m_Table(iter.m_Table), m_Bucket(iter.m_Bucket), m_Node(iter.m_Node)
    {
    }

    /*--------------------------------------------------------------------------
    function:     IsValid()
    description:  Checks whether actual node is valid
    arguments:    none
    return value: (bool) true if valid
    --------------------------------------------------------------------------*/
    bool IsValid() const
    {
        return m_Node != NULL;
    }

    /*!
       @brief          Compares two iterators
       @param          iter [in] iterator to be compared with
       @return         (bool) true if both iterators point to same node

     */

    bool operator==
        (const Iterator&                iter) const
    { 
        return m_Node == iter.m_Node; 
    }

    /*!
       @brief          Compares two iterators
       @param          iter [in] iterator to be compared with
       @return         (bool) true if both iterators point to different nodes

     */

    bool operator!=
        (const Iterator&                iter) const
    { 
        return m_Node != iter.m_Node;
    }


    /*!
       @brief          Dereferences node pointer
       @return         (RNodeType&amp;) reference to actual node content

     */

    RNodeType& operator*() const
    {
        SAPDBERR_ASSERT_STATE(IsValid());
        return *((RNodeType*) m_Node);
    }

    /*!
       @brief          Allows access to members of the node content
       @return         (RNodeType*) pointer to node content

     */

#   if defined(_WIN32) || defined(WIN32)
#   pragma warning(disable : 4284)
#   endif
    RNodeType* operator->() const
    {
        SAPDBERR_ASSERT_STATE(IsValid());
        return (RNodeType*) m_Node;
    }
#   if defined(_WIN32) || defined(WIN32)
#   pragma warning(default : 4284)
#   endif

    /*!
       @brief          Prefix increment; advances iterator to next node
       @return         (Iterator&amp;) *this

     */

    Iterator& operator++();

    /*!
       @brief          Postfix increment; advances iterator to next node
       @return         (Iterator&amp;) *this

     */

    Iterator operator++(int) 
    {
        Iterator save(*this);
        ++*this;
        return save;
    }

    /*!
       @brief          Prefix decrement; advances iterator to previous node
       @return         (Iterator&amp;) *this

     */

    Iterator& operator--();

    /*!
       @brief          Postfix decrement; advances iterator to previous node
       @return         (Iterator&amp;) *this

     */

    Iterator operator--(int) 
    {
        Iterator save(*this);
        --*this;
        return save;
    }

    /*!
       @brief          Assignment operator
       @param          iter [in] other iterator
       @return         (Iterator&amp;) *this

     */

    Iterator& operator=(const Iterator &iter) 
    {
        m_Table = iter.m_Table;
        m_Bucket = iter.m_Bucket;
        m_Node = iter.m_Node;
        return *this;
    }



protected:
    friend class Container_Hash<Key, T, AllowDuplicates>;
 
    Hash      *m_Table;
    SizeType  m_Bucket;
    NodeType  *m_Node;
};





/*!
  @class          Container_Hash
   @brief          This class implements a simple hashtable
   template arguments &lt;Key>         type of the key

              Key must provide the following methods:

              - Default constructor Key()
              - SAPDB_Bool operator==(const Key& t) const

          &lt;T>           stored datatype

          &lt;AllowDuplicates> true, if duplicate keys are allowed


   The hashtable uses raw allocators as defined in the SAPDBMem component
   for allocating memory.
   As of release 7.4, no exceptions are thrown.
   Instead, SAPDB_Bool return values or invalid iterators are returned by
   the methods where appropriate.

 */

template <class Key, class T, bool AllowDuplicates>
class Container_Hash
{
private:

   typedef Container_HashNode<Key, T>                       NodeType;
   typedef NodeType*                                        NodePtr;
   typedef Container_Hash<Key, T, AllowDuplicates>          Hash;

public:
    /* -----------------------------------------------------------------------*/
    /*! @name Type definitions */
     /*@{*/
    /*!-----------------------------------------------------------------------*/

    /*!
       @brief          Node key type

     */

    typedef Key                                             KeyType;
    /*!
       @brief          Node content type

     */

    typedef T                                               ValueType;
    /*!
       @brief          Count of nodes (elements) in the hashtable

     */

    typedef SAPDB_UInt                                      SizeType;
    /*!
       @brief          allocator for entries

     */

    typedef SAPDBMem_IRawAllocator                          Allocator;
    /*!
       @brief          table iterator

     */

    typedef Container_HashNodeIterator
            <KeyType, ValueType, ValueType, AllowDuplicates> Iterator;
    /*!
       @brief          table iterator

     */

    typedef Container_HashNodeIterator
            <KeyType, ValueType, const ValueType, AllowDuplicates> ConstIterator;
    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

public:

    /* -----------------------------------------------------------------------*/
    /*! @name Construction and destruction */
     /*@{*/
    /*!-----------------------------------------------------------------------*/

    /*!
       @brief          Constructor
       @param          alloc [in] reference to raw allocator
       @return         none

     */

    Container_Hash(Allocator &alloc)
        : m_Allocator(alloc), m_NodeCount(0),
          m_BucketCount(0), m_Buckets(NULL)
    {
    }

    /*!
       @brief          Constructor
       @param          alloc [in] reference to raw allocator
       @param          table [in] reference to hashtable to copy
       @return         none

     */

    Container_Hash(Allocator &alloc, Hash &table)
        : m_Allocator(alloc), m_NodeCount(0),
          m_BucketCount(0), m_Buckets(NULL)
    {
        AppendTable(table);
    }

    /*!
       @brief          Destructs the hash and deallocates all elements
       @return         none

     */

    ~Container_Hash()
    {
        Delete();
    }

    /*--------------------------------------------------------------------------
    function:     operator=()
    description:  Assigns table

    arguments:    table [in]     table to be assigned
    return value: (Hash&) *this
    --------------------------------------------------------------------------*/
    Hash& operator=
        (const Hash&              table)
    {
        Delete();
        AppendTable(table);
        return *this;
    }

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/


    /* -----------------------------------------------------------------------*/
    /*! @name Status functions */
     /*@{*/
    /*!-----------------------------------------------------------------------*/

    /*!
       @brief          Checks whether hashtable is empty
       @return         (SAPDB_Bool) true if empty

     */

    SAPDB_Bool IsEmpty() const
    { 
        return m_NodeCount == 0;
    }

    /*!
       @brief          Returns the actual node count of the hashtable
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
       @brief          Compares actual hashtable with 'table'
       @param          table [in] hashtable to be compared
       @return         (SAPDB_Bool) true if both tables have the same content

     */

    SAPDB_Bool operator==
        (const Hash&              table) const;

    /*!
       @brief          Compares actual hashtable with 'table'
       @param          table [in] hashtable to be compared
       @return         (SAPDB_Bool) true if both tables have the same content

     */

    SAPDB_Bool operator !=
        (const Hash&              table) const
    { 
        return !(this->operator==(table));
    }
    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/


    /* -----------------------------------------------------------------------*/
    /*! @name Hashtable manipulation */
     /*@{*/
    /*!-----------------------------------------------------------------------*/

    /*!
       @brief          Inserts a new element
       @param          key [in] key
       @param          elem [in] element to be inserted
       @return         (Iterator) iterator pointing to the inserted node;
                         invalid iterator if insert not successful

     */

    Iterator Insert
        (const KeyType&                 key,
         const ValueType&               elem);

    /*!
       @brief          Deletes all elements in the hashtable
       @return         none

     */

    void Delete();

    /*!
       @brief          Deletes the element at the given iterator position
       @param          pos [in/out] iterator delete position
       @return         none


       - After the operation, 'pos' points to the node behind                  the deleted one.

     */

    void Delete
        (Iterator&                      pos);

    /*-----------------------------------------------------------------------
    function:     AppendTable()
    description:  Append elements from other table

                  - Argument 'table' is appended element by element
                    into the actual table.
    arguments:    table  [in] table to be copied
    return:      (SAPDB_Bool) true if successful
    -----------------------------------------------------------------------*/
    SAPDB_Bool AppendTable
        (const Hash&              table);

    /*-----------------------------------------------------------------------
    function:     CopyTable()
    description:  Copy elements from other table

                  - Argument 'table' is copied element by element
                    into the actual table, old contents is lost.
    arguments:    table  [in] table to be copied
    return:      (SAPDB_Bool) true if successful
    -----------------------------------------------------------------------*/
    SAPDB_Bool CopyTable
        (const Hash&              table)
    {
        Delete();
        return AppendTable(table);
    }

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/


    /* -----------------------------------------------------------------------*/
    /*! @name Iterator functions */
     /*@{*/
    /*!-----------------------------------------------------------------------*/

    /*!
       @brief          Returns an iterator to the first node
       @return         (Iterator/ConstIterator)

     */

    Iterator Begin()
    {
        SizeType bucket;
        NodePtr node;
        FindFirstNode(bucket, node);
        return Iterator(this, bucket, node);
    }

    ConstIterator Begin() const
    { 
        SizeType bucket;
        NodePtr node;
        FindFirstNode(bucket, node);
        return ConstIterator(this, bucket, node);
    }

    /*!
       @brief          Returns an iterator behind the last node
       @return         (Iterator/ConstIterator)

     */

    Iterator End()
    { 
        return Iterator(this, 0, NULL);
    }

    ConstIterator End() const
    { 
        return ConstIterator(this, 0, NULL);
    }

    /*!
       @brief          Returns an iterator pointing to the node with given key.
              When there are more items with the same key, any of them
              may be returned.
       @param          k [in] key to look up
       @return         (Iterator/ConstIterator)

     */

    Iterator Find(const KeyType &k)
    {
        SizeType bucket;
        NodePtr node;
        FindNode(bucket, node, k);
        return Iterator(this, bucket, node);
    }

    ConstIterator Find(const KeyType &k) const
    {
        SizeType bucket;
        NodePtr node;
        FindNode(bucket, node, k);
        return ConstIterator(this, bucket, node);
    }

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/


protected:

    friend class Container_HashNodeIterator<KeyType, ValueType, ValueType, AllowDuplicates>;
    friend class Container_HashNodeIterator<KeyType, ValueType, const ValueType, AllowDuplicates>;

    NodePtr FindNextNode(SizeType &bucket) const
    {
        while (++bucket < m_BucketCount)
        {
            if (m_Buckets[bucket])
            {
                return m_Buckets[bucket];
            }
        }
        bucket = 0;
        return NULL;
    }

    NodePtr FindPrevNode(SizeType &bucket) const
    {
        while (--bucket >= 0)
        {
            if (m_Buckets[bucket])
            {
                NodePtr p = m_Buckets[bucket];
                while (p->next) p = p->next;
                return p;
            }
        }
        bucket = 0;
        return NULL;
    }

private:
    /*--------------------------------------------------------------------------
    declaration : m_Allocator
    description:  Reference to underlying raw allocator
    --------------------------------------------------------------------------*/
    Allocator&                                      m_Allocator;
    /*--------------------------------------------------------------------------
    declaration : m_NodeCount
    description:  Actual node count in the table
    --------------------------------------------------------------------------*/
    SizeType                                        m_NodeCount;
    /*--------------------------------------------------------------------------
    declaration : m_BucketCount
    description:  Actual bucket count in the table
    --------------------------------------------------------------------------*/
    SizeType                                        m_BucketCount;
    /*--------------------------------------------------------------------------
    declaration : m_Buckets
    description:  Hashtable buckets
    --------------------------------------------------------------------------*/
    NodePtr                                         *m_Buckets;

    void FindFirstNode(SizeType &bucket, NodePtr &node) const
    {
        SizeType i = 0;
        while (i < m_BucketCount) {
            if (m_Buckets[i]) {
                bucket = i;
                node = m_Buckets[i];
                return;
            }
            ++i;
        }
        bucket = 0;
        node = NULL;
    }

    void FindNode(SizeType &bucket, NodePtr &node, const KeyType &k) const
    {
        if (m_BucketCount > 0) {
            bucket = Container_HashFnc<KeyType>(k) % m_BucketCount;
            NodePtr p = m_Buckets[bucket];
            while (p) 
            {
                if ( Container_CompareFnc<KeyType>(p->key, k) )
                {
                    node = p;
                    return;
                }
                p = p->next;
            }
        }
        bucket = 0;
        node = NULL;
    }

    NodePtr GetNode(KeyType key, ValueType value) const
    {
        return (NodePtr) new (m_Allocator) NodeType(key, value);
    }

    void PutNode(NodePtr p) const
    {
#ifdef LINUX    // egcs 2.91 only likes it this way...
        p->~NodeType();
#else
        p->~Container_HashNode<Key, T>();
#endif
        m_Allocator.Deallocate(p);
    }

    void Resize(SizeType cnt);
};




template <class Key, class T, class RT, bool AllowDuplicates>
Container_HashNodeIterator<Key, T, RT, AllowDuplicates>&
    Container_HashNodeIterator<Key, T, RT, AllowDuplicates>::operator++()
{
    SAPDBERR_ASSERT_STATE(IsValid());
    if (m_Node->next) m_Node = m_Node->next;
    else m_Node = m_Table->FindNextNode(m_Bucket);
    return *this;
}

template <class Key, class T, class RT, bool AllowDuplicates>
Container_HashNodeIterator<Key, T, RT, AllowDuplicates>&
    Container_HashNodeIterator<Key, T, RT, AllowDuplicates>::operator--()
{
    SAPDBERR_ASSERT_STATE(IsValid());
    if (m_Node->prev) m_Node = m_Node->prev;
    else m_Node = m_Table->FindPrevNode(m_Bucket);
    return *this;
}

template <class Key, class T, bool AllowDuplicates>
SAPDB_Bool Container_Hash<Key, T, AllowDuplicates>::operator ==
    (const Hash&              table) const
{
    if (m_NodeCount != table.m_NodeCount)
        return false;
    if (IsEmpty())
        return true;

    // -------------------------------------------------------------------------
    // compare tables elementwise
    // -------------------------------------------------------------------------
    ConstIterator iter1 = Begin();
    ConstIterator iter2 = table.Begin();
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

template <class Key, class T, bool AllowDuplicates>
SAPDB_Bool Container_Hash<Key, T, AllowDuplicates>::AppendTable
    (const Hash&              table)
{
    ConstIterator iter = table.Begin();
    ConstIterator end  = table.End();
    Iterator cend      = End();
    SAPDB_Bool success = true;
    Resize((GetSize() + table.GetSize()) * 2);
    while (iter != end)
    {
        Iterator i = Insert(iter->key, iter->value);
        if (i == cend) success = false;
        ++iter;
    }

    return success;
}

template <class Key, class T, bool AllowDuplicates>
Container_HashNodeIterator<Key, T, T, AllowDuplicates>
Container_Hash<Key, T, AllowDuplicates>::Insert(
    const KeyType &key, const ValueType &value)
{
    if (m_NodeCount >= m_BucketCount)
    {
        Resize(m_NodeCount * 2);
    }

    SizeType bucket = Container_HashFnc<KeyType>(key) % m_BucketCount;

    if (!AllowDuplicates)
    {
        NodePtr p = m_Buckets[bucket];
        while (p) 
        {
            if ( Container_CompareFnc<KeyType>(p->key, key) ) 
            {
                return Iterator(this, 0, NULL);
            }
            p = p->next;
        }
    }

    NodePtr p = GetNode(key, value);
    if (!p)
    {
        return Iterator(this, 0, NULL);
    }
    p->prev = NULL;
    p->next = m_Buckets[bucket];
    if (p->next) p->next->prev = p;
    m_Buckets[bucket] = p;

    ++m_NodeCount;

    return Iterator(this, bucket, p);
}


template <class Key, class T, bool AllowDuplicates>
void Container_Hash<Key, T, AllowDuplicates>::Delete()
{
    if (m_BucketCount == 0) 
        return;

    SizeType i = 0;
    while (i < m_BucketCount) {
        NodePtr node = m_Buckets[i];
        while (node) {
            NodePtr p = node;
            node = node->next;
            PutNode(p);
        }
        ++i;
    }
    if (m_Buckets) m_Allocator.Deallocate(m_Buckets);
    m_BucketCount = 0;
    m_NodeCount = 0;
    m_Buckets = NULL;
}

template <class Key, class T, bool AllowDuplicates>
void Container_Hash<Key, T, AllowDuplicates>::Delete(Iterator &pos)
{
    if (!pos.IsValid())
        return;

    SizeType b = pos.m_Bucket;
    NodePtr p = (pos++).m_Node;

    if (p->prev) p->prev->next = p->next;
    else m_Buckets[b] = p->next;
    if (p->next) p->next->prev = p->prev;
    PutNode(p);

    --m_NodeCount;
}

template <class Key, class T, bool AllowDuplicates>
void Container_Hash<Key, T, AllowDuplicates>::Resize(SizeType n)
{
    if (m_BucketCount >= 333333349 || m_BucketCount > n)
        return;

    // TODO: move primes out of here
    static const SizeType primes[] = { 7, 19, 53, 127, 269, 607, 1307, 2917,
        5939, 12007, 25111, 52021, 130021, 289021, 606037, 1440079,
        2901337, 6111139, 12222241, 25555561, 66666667, 144444457,
        333333349};

    const SizeType *newCount = &primes[0];
    while (*newCount < n && *newCount < 333333349)
        ++newCount;

    SizeType count = *newCount;
    NodePtr *newBuckets = (NodePtr*) m_Allocator.Allocate(sizeof(NodePtr) * count);
    memset(newBuckets, 0, sizeof(NodePtr) * count);

    for (SizeType i = 0; i < m_BucketCount; ++i)
    {
        NodePtr p = m_Buckets[i];
        while (p)
        {
            NodePtr next = p->next;
            SizeType bucket = Container_HashFnc<KeyType>(p->key) % count;

            p->next = newBuckets[bucket];
            p->prev = NULL;
            if (p->next) p->next->prev = p;
            newBuckets[bucket] = p;

            p = next;
        }
    }

    if (m_Buckets) m_Allocator.Deallocate(m_Buckets);
    m_Buckets = newBuckets;
    m_BucketCount = count;
}


#endif  // CONTAINER_HASH_HPP
