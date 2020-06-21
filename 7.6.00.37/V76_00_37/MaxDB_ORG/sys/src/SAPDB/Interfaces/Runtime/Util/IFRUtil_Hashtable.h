/*!
  @file           IFRUtil_Hashtable.h
  @author         D039759
  @ingroup        IFR_Util
  @brief          Hashtable, algorithms from STL.

  @see            

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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
#ifndef IFRUTIL_HASHTABLE_H
#define IFRUTIL_HASHTABLE_H
#include <stddef.h>

#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "Interfaces/Runtime/IFR_Types.h"
#include "Interfaces/Runtime/Util/IFRUtil_Vector.h"
#include "Interfaces/Runtime/Util/IFRUtil_Algo.h"
#include "Interfaces/Runtime/Util/IFRUtil_Pair.h"

IFR_BEGIN_NAMESPACE

// Remove 'identifier truncated in debug info' message.
#if defined(_WIN32)
#pragma warning ( disable : 4786 )
#endif

/**
 * A single node in a hashtable bucket.
 */
template <class ValueClass>
struct IFRUtil_HashtableNode
{
    IFRUtil_HashtableNode()
    :m_next(0),
     m_val()
    {}


    IFRUtil_HashtableNode* m_next;
    ValueClass m_val;
};  

// Forward declarations.
template <class ValueClass, class KeyClass, class HashFunctionType,
          class KeyExtract, class KeyEqualsType>
class IFRUtil_Hashtable;

template <class ValueClass, class KeyClass, class HashFunctionType,
          class KeyExtract, class KeyEqualsType>
IFR_Bool operator == (const IFRUtil_Hashtable <ValueClass, KeyClass, HashFunctionType,
                      KeyExtract, KeyEqualsType>& ht1,
                      const IFRUtil_Hashtable<ValueClass, KeyClass, HashFunctionType,
                      KeyExtract, KeyEqualsType>& ht2);

template <class ValueClass, class KeyClass, class HashFunctionType,
          class KeyExtract, class KeyEqualsType>
struct IFRUtil_HashtableIterator;

template <class ValueClass, class KeyClass, class HashFunctionType,
          class KeyExtract, class KeyEqualsType>
struct IFRUtil_HashtableConstIterator;

template <class ValueClass, class KeyClass, class HashFunctionType,
          class KeyExtract, class KeyEqualsType>
struct IFRUtil_HashtableIterator 
{
    typedef IFRUtil_Hashtable<ValueClass,KeyClass,HashFunctionType,KeyExtract,KeyEqualsType> Hashtable;
    typedef IFRUtil_HashtableIterator<ValueClass, KeyClass, HashFunctionType, KeyExtract, KeyEqualsType>  Iterator;
    typedef IFRUtil_HashtableConstIterator<ValueClass, KeyClass, HashFunctionType, KeyExtract, KeyEqualsType>  ConstIterator;
    typedef IFRUtil_HashtableNode<ValueClass> Node;
    typedef ValueClass  ValueType;
    typedef ptrdiff_t   DifferenceType;
    typedef size_t      SizeType;
    typedef ValueClass& Reference;
    typedef ValueClass* Pointer;

    Node* m_cur;
    Hashtable* m_ht;
    
    IFRUtil_HashtableIterator(Node*n, Hashtable* tab) 
    :m_cur(n), 
     m_ht(tab)
    {}
    
    IFRUtil_HashtableIterator() 
    {}
    
    Reference operator*() const 
    { 
        return m_cur->m_val; 
    }
    
    Pointer operator->() const { 
        return &(operator*()); 
    }
    
    Iterator& operator++();
    
    Iterator operator++(int);
    
    IFR_Bool operator==(const Iterator& it) const
    { 
        return m_cur == it.m_cur; 
    }
    
    IFR_Bool operator!=(const Iterator& it) const
    { 
        return m_cur != it.m_cur; 
    }
};


template <class ValueClass, class KeyClass, class HashFunctionType,
          class KeyExtract, class KeyEqualsType>
struct IFRUtil_HashtableConstIterator 
{
    typedef IFRUtil_Hashtable<ValueClass,KeyClass,HashFunctionType,KeyExtract,KeyEqualsType>  Hashtable;
    typedef IFRUtil_HashtableIterator<ValueClass,KeyClass,HashFunctionType, KeyExtract,KeyEqualsType> Iterator;
    typedef IFRUtil_HashtableConstIterator<ValueClass, KeyClass, HashFunctionType, KeyExtract, KeyEqualsType> ConstIterator;
    typedef IFRUtil_HashtableNode<ValueClass> Node;

    typedef ValueClass  ValueType;
    typedef ptrdiff_t   DifferenceType;
    typedef size_t      SizeType;
    typedef const ValueClass& Reference;
    typedef const ValueClass* Pointer;

    const Node* m_cur;
    const Hashtable* m_ht;

    IFRUtil_HashtableConstIterator(const Node* n, const Hashtable* tab)
    :m_cur(n), 
     m_ht(tab)
    {}
    
    IFRUtil_HashtableConstIterator() 
    {}

    IFRUtil_HashtableConstIterator(const Iterator& it) 
    :m_cur(it.m_cur), 
     m_ht(it.m_ht) 
    {}
    
    Reference operator*() const 
    { 
        return m_cur->m_val; 
    }
    
    Pointer operator->() const 
    { 
        return &(operator*()); 
    }
    
    ConstIterator& operator++();
    
    ConstIterator operator++(int);
    
    IFR_Bool operator==(const ConstIterator& it) const 
    { 
        return m_cur == it.m_cur; 
    }
    
    IFR_Bool operator!=(const ConstIterator& it) const 
    { 
        return m_cur != it.m_cur; 
    }
};

//----------------------------------------------------------------------
const IFR_UInt4 IFRUTIL_HT_PRIMES[] = 
{
    53ul,         97ul,         193ul,       389ul,       769ul,
    1543ul,       3079ul,       6151ul,      12289ul,     24593ul,
    49157ul,      98317ul,      196613ul,    393241ul,    786433ul,
    1572869ul,    3145739ul,    6291469ul,   12582917ul,  25165843ul,
    50331653ul,   100663319ul,  201326611ul, 402653189ul, 805306457ul, 
    1610612741ul, 3221225473ul, 4294967291ul
};


inline IFR_size_t IFRUtil_Hashtable_NextPrime(IFR_size_t n)
{
    const IFR_UInt4* first = IFRUTIL_HT_PRIMES;
    const IFR_UInt4* last = IFRUTIL_HT_PRIMES + 28; // number of primes in array
    const IFR_UInt4* pos =  IFRUtil_LowerBound(first, last, n);
    return (IFR_size_t)(pos == last ? *(last - 1) : *pos);
}

// Forward declaration of operator==.

template <class ValueClass, class KeyClass, class HashFunctionType, class KeyExtract, class KeyEqualsType>
class IFRUtil_Hashtable;

template <class ValueClass, class KeyClass, class HashFunctionType, class KeyExtract, class KeyEqualsType>
IFR_Bool operator==(const IFRUtil_Hashtable<ValueClass,KeyClass,HashFunctionType,KeyExtract,KeyEqualsType>& ht1,
                    const IFRUtil_Hashtable<ValueClass,KeyClass,HashFunctionType,KeyExtract,KeyEqualsType>& ht2);

template <class ValueClass, class KeyClass, class HashFunctionType,
          class KeyExtract, class KeyEqualsType>
class IFRUtil_Hashtable 
{
public:
    typedef KeyClass         KeyType;
    typedef ValueClass       ValueType;
    typedef HashFunctionType HashFunction;
    typedef KeyEqualsType    KeyEquals;
    
    typedef size_t           SizeType;
    typedef ptrdiff_t        DifferenceType;
    typedef ValueType*       Pointer;
    typedef const ValueType* ConstPointer;
    typedef ValueType&       Reference;
    typedef const ValueType& ConstReference;

    HashFunction hash_funct() const 
    { 
        return m_hash; 
    }
    
    KeyEquals key_eq() const 
    { 
        return m_equals; 
    }

private:
    typedef IFRUtil_HashtableNode<ValueClass> Node;

public:
    typedef SAPDBMem_IRawAllocator& AllocatorType;
    AllocatorType get_allocator() const { return m_node_allocator; }

private:
    SAPDBMem_IRawAllocator& m_node_allocator;
     
    Node* m_get_node() 
    { 
        Node *np=(Node*)IFR_ALLOCATOR(m_node_allocator).Allocate(sizeof(Node));
        new (np) Node();
        return np;
    }
    
    void m_put_node(Node* p) 
    { 
        IFRUtil_Destroy(p);
        IFR_ALLOCATOR(m_node_allocator).Deallocate(p);
    }

private:
    HashFunction                 m_hash;
    KeyEquals                    m_equals;
    KeyExtract                   m_get_key;
    IFRUtil_Vector<Node*>        m_buckets;
    SizeType                     m_num_elements;

public:
    typedef IFRUtil_HashtableIterator<ValueClass,KeyClass,HashFunctionType,KeyExtract,KeyEqualsType>      Iterator;
    typedef IFRUtil_HashtableConstIterator<ValueClass,KeyClass,HashFunctionType,KeyExtract,KeyEqualsType> ConstIterator;
    
    friend struct IFRUtil_HashtableIterator<ValueClass,KeyClass,HashFunctionType,KeyExtract,KeyEqualsType>;
    friend struct IFRUtil_HashtableConstIterator<ValueClass,KeyClass,HashFunctionType,KeyExtract,KeyEqualsType>;

public:
    IFRUtil_Hashtable(SizeType n,
                      const HashFunctionType& hf,
                      const KeyEqualsType& eql,
                      const KeyExtract& ext,
                      AllocatorType a) 
    :m_node_allocator(a),
     m_hash(hf),
     m_equals(eql),
     m_get_key(ext),
     m_buckets(a),
     m_num_elements(0)
    {
        initialize_buckets(n);
    }

    IFRUtil_Hashtable(SizeType n,
                      const HashFunctionType&    hf,
                      const KeyEqualsType&   eql,
                      AllocatorType a,
                      IFR_Bool& memory_ok)
    :m_node_allocator(a),
     m_hash(hf),
     m_equals(eql),
     m_get_key(KeyExtract()),
     m_buckets(a),
     m_num_elements(0)
    {
        initialize_buckets(n, memory_ok);
    }

//     IFRUtil_Hashtable(const IFRUtil_Hashtable& ht)
//     :m_node_allocator(ht.get_allocator()),
//      m_hash(ht.m_hash),
//      m_equals(ht.m_equals),
//      m_get_key(ht.m_get_key),
//      m_buckets(ht.get_allocator()),
//      m_num_elements(0)
//     {
//         m_copy_from(ht);
//     }


//     IFRUtil_Hashtable& operator= (const IFRUtil_Hashtable& __ht)
//     {
//         if (&__ht != this) {
//             clear();
//             m_hash = __ht.m_hash;
//             m_equals = __ht.m_equals;
//             m_get_key = __ht.m_get_key;
//             m_copy_from(__ht);
//         }
//         return *this;
//     }

    ~IFRUtil_Hashtable() 
    { 
        clear(); 
    }

    SizeType size() const 
    { 
        return m_num_elements; 
    }
    
    IFR_Bool empty() const 
    { 
        return size() == 0; 
    }

    void swap(IFRUtil_Hashtable& ht)
    {
        IFRUtil_Swap(m_hash, ht.m_hash);
        IFRUtil_Swap(m_equals, ht.m_equals);
        IFRUtil_Swap(m_get_key, ht.m_get_key);
        m_buckets.Swap(ht.m_buckets);
        IFRUtil_Swap(m_num_elements, ht.m_num_elements);
    }

    Iterator begin()
    { 
        SizeType bucketlen=m_buckets.GetSize();
        for (SizeType n = 0; n < bucketlen; ++n) {
            if (m_buckets[n]) {
                return Iterator(m_buckets[n], this);
            }
        }
        return end();
    }

    Iterator end() 
    { 
        return Iterator(0, this); 
    }

  ConstIterator begin() const
  {
      SizeType bucketlen=m_buckets.GetSize();
      for (SizeType n = 0; n < bucketlen; ++n)
          if (m_buckets[n])
              return ConstIterator(m_buckets[n], this);
      return end();
  }

    ConstIterator end() const 
    { 
        return ConstIterator(0, this); 
    }

#if !defined(WIN32) && !defined(SVR4) && !defined(SOLARIS)
    friend IFR_Bool operator == <> (const IFRUtil_Hashtable&, 
                                    const IFRUtil_Hashtable &);
#else
    friend IFR_Bool operator == (const IFRUtil_Hashtable&, 
                                 const IFRUtil_Hashtable &);
#endif
    
public:
    SizeType bucket_count() const 
    { 
        return m_buckets.GetSize(); 
    }
    
    SizeType max_bucket_count() const
    { 
        return 4294967291ul;
    } 

    SizeType elems_in_bucket(SizeType bucket) const
    {
        SizeType result = 0;
        for (Node* cur = m_buckets[bucket]; cur; cur = cur->m_next)
            result++;
        return result;
    }

    IFRUtil_Pair<Iterator, IFR_Bool> insert_unique(const ValueType& obj)
    {
        resize(m_num_elements + 1);
        return insert_unique_noresize(obj);
    }

    Iterator insert_equal(const ValueType& obj)
    {
        resize(m_num_elements + 1);
        return insert_equal_noresize(obj);
    }

    IFRUtil_Pair<Iterator, IFR_Bool> insert_unique_noresize(const ValueType& obj);
    Iterator insert_equal_noresize(const ValueType& obj);
 
    void insert_unique(const ValueType* f, const ValueType* l)
    {
        SizeType n = l - f;
        resize(m_num_elements + n);
        for ( ; n > 0; --n, ++f)
            insert_unique_noresize(*f);
    }
    
    void insert_equal(const ValueType* f, const ValueType* l)
    {
        SizeType n = l - f;
        resize(m_num_elements + n);
        for ( ; n > 0; --n, ++f)
            insert_equal_noresize(*f);
    }
    
    void insert_unique(ConstIterator f, ConstIterator l)
    {
        SizeType n = 0;
        IFRUtil_Distance(f, l, n);
        resize(m_num_elements + n);
        for ( ; n > 0; --n, ++f)
            insert_unique_noresize(*f);
    }

    void insert_equal(ConstIterator f, ConstIterator l)
    {
        SizeType n = 0;
        IFRUtil_Distance(f, l, n);
        resize(m_num_elements + n);
        for ( ; n > 0; --n, ++f)
            insert_equal_noresize(*f);
    }
    
    
    Reference find_or_insert(const ValueType& obj, IFR_Bool& memory_ok);

    
    Iterator find(const KeyType& key) 
    {
        SizeType n = m_bkt_num_key(key);
        Node* first;
        for (first = m_buckets[n];
             first && !m_equals(m_get_key(first->m_val), key);
             first = first->m_next)
            ;
        return Iterator(first, this);
    } 

    ConstIterator find(const KeyType& key) const
    {
        SizeType n = m_bkt_num_key(key);
        const Node* first;
        for ( first = m_buckets[n];
              first && !m_equals(m_get_key(first->m_val), key);
              first = first->m_next)
        {}
        return ConstIterator(first, this);
    } 

    SizeType count(const KeyType& key) const
    {
        const SizeType n = m_bkt_num_key(key);
        SizeType result = 0;
        
        for (const Node* cur = m_buckets[n]; cur; cur = cur->m_next)
            if (m_equals(m_get_key(cur->m_val), key))
                ++result;
        return result;
    }

    IFRUtil_Pair<Iterator, Iterator>  equal_range(const KeyType& key);
    IFRUtil_Pair<ConstIterator, ConstIterator> equal_range(const KeyType& key) const;
    SizeType erase(const KeyType& key);
    void erase(const Iterator& it);
    void erase(Iterator first, Iterator last);
    void erase(const ConstIterator& it);
    void erase(ConstIterator first, ConstIterator last);
    void resize(SizeType __num_elements_hint, IFR_Bool& memory_ok);
    void clear();
    
private:
    IFRUtil_Hashtable<ValueClass, KeyClass, HashFunctionType, KeyExtract, KeyEqualsType>&
    operator =(const IFRUtil_Hashtable<ValueClass, KeyClass, HashFunctionType, KeyExtract, KeyEqualsType>&)
    {
        return *this;
    }
    
    IFRUtil_Hashtable(const IFRUtil_Hashtable<ValueClass, KeyClass, HashFunctionType, KeyExtract, KeyEqualsType>&);
private:
    
    

    SizeType m_next_size(SizeType n) const
    { 
        return IFRUtil_Hashtable_NextPrime(n); 
    }

    /**
     * @internal
     * Initializes the table of buckets with <code>n</code>
     * buckets.
     */
    void initialize_buckets(SizeType n, IFR_Bool& memory_ok)
    {
        if(!memory_ok) {
            return;
        }
        const SizeType n_buckets = m_next_size(n);
        m_buckets.Reserve(n_buckets, memory_ok);
        if(!memory_ok) {
            return;
        }
        for(SizeType i=0; i<n_buckets; ++i) {
            m_buckets.InsertEnd((Node*) 0, memory_ok);
            if(!memory_ok) {
                return;
            }
        }
        m_num_elements = 0;
    }

    SizeType m_bkt_num_key(const KeyType& key) const
    {
        return m_bkt_num_key(key, m_buckets.GetSize());
    }
    
    SizeType m_bkt_num(const ValueType& obj) const
    {
        return m_bkt_num_key(m_get_key(obj));
    }
    
    SizeType m_bkt_num_key(const KeyType& key, size_t n) const
    {
        return hash_funct() (key) % n;
    }

    SizeType m_bkt_num(const ValueType& obj, size_t n) const
    {
        return m_bkt_num_key(m_get_key(obj), n);
    }
    
    Node* m_new_node(const ValueType& obj)
    {
        Node* n = m_get_node();
        n->m_next = 0;
        IFRUtil_Construct(&n->m_val, obj);
        return n;
    }
  
    void m_delete_node(Node* n)
    {
        IFRUtil_Destroy(&n->m_val);
        m_put_node(n);
  }
    
    void m_erase_bucket(const SizeType n, Node* first, Node* last);
    void m_erase_bucket(const SizeType n, Node* last);

    void m_copy_from(const IFRUtil_Hashtable& ht);
};


//----------------------------------------------------------------------
// implementation of more complicated members.

template <class ValueClass, class KeyClass, class HashFunctionType, class KeyExtract, class KeyEqualsType>
IFRUtil_HashtableIterator<ValueClass,KeyClass,HashFunctionType,KeyExtract,KeyEqualsType>&
IFRUtil_HashtableIterator<ValueClass,KeyClass,HashFunctionType,KeyExtract,KeyEqualsType>::operator++()
{
    const Node* old = m_cur;
    m_cur = m_cur->m_next;
    if (!m_cur) {
        SizeType bucket = m_ht->m_bkt_num(old->m_val);
        while (!m_cur && ++bucket < m_ht->m_buckets.size())
            m_cur = m_ht->m_buckets[bucket];
    }
    return *this;
}

template <class ValueClass, class KeyClass, class HashFunctionType, class KeyExtract, class KeyEqualsType>
inline IFRUtil_HashtableIterator<ValueClass,KeyClass,HashFunctionType,KeyExtract,KeyEqualsType>
IFRUtil_HashtableIterator<ValueClass,KeyClass,HashFunctionType,KeyExtract,KeyEqualsType>::operator++(int)
{
    Iterator tmp = *this;
    ++*this;
    return tmp;
}

template <class ValueClass, class KeyClass, class HashFunctionType, class KeyExtract, class KeyEqualsType>
IFRUtil_HashtableConstIterator<ValueClass,KeyClass,HashFunctionType,KeyExtract,KeyEqualsType>&
IFRUtil_HashtableConstIterator<ValueClass,KeyClass,HashFunctionType,KeyExtract,KeyEqualsType>::operator++()
{
    const Node* old = m_cur;
    m_cur = m_cur->m_next;
    if (!m_cur) {
        SizeType bucket = m_ht->m_bkt_num(old->m_val);
        while (!m_cur && ++bucket < m_ht->m_buckets.size())
            m_cur = m_ht->m_buckets[bucket];
    }
    return *this;
}

template <class ValueClass, class KeyClass, class HashFunctionType, class KeyExtract, class KeyEqualsType>
inline IFRUtil_HashtableConstIterator<ValueClass,KeyClass,HashFunctionType,KeyExtract,KeyEqualsType>
IFRUtil_HashtableConstIterator<ValueClass,KeyClass,HashFunctionType,KeyExtract,KeyEqualsType>::operator++(int)
{
    ConstIterator tmp = *this;
    ++*this;
    return tmp;
}

template <class ValueClass, class KeyClass, class HashFunctionType, class KeyExtract, class KeyEqualsType>
IFR_Bool operator==(const IFRUtil_Hashtable<ValueClass,KeyClass,HashFunctionType,KeyExtract,KeyEqualsType>& ht1,
                    const IFRUtil_Hashtable<ValueClass,KeyClass,HashFunctionType,KeyExtract,KeyEqualsType>& ht2)
{
    typedef IFR_TYPENAME IFRUtil_Hashtable<ValueClass,KeyClass,HashFunctionType,KeyExtract,KeyEqualsType>::Node Node;
    if (ht1.m_buckets.size() != ht2.m_buckets.size())
        return false;
    for (int n = 0; n < ht1.m_buckets.size(); ++n) {
        Node* cur1 = ht1.m_buckets[n];
        Node* cur2 = ht2.m_buckets[n];
        for ( ; cur1 && cur2 && cur1->m_val == cur2->m_val;
              cur1 = cur1->m_next, cur2 = cur2->m_next)
        {}
        if (cur1 || cur2)
            return false;
    }
    return true;
}  

template <class ValueClass, class KeyClass, class HashFunctionType, class KeyExtract, class KeyEqualsType>
inline IFR_Bool operator!=(const IFRUtil_Hashtable<ValueClass,KeyClass,HashFunctionType,KeyExtract,KeyEqualsType>& ht1,
                           const IFRUtil_Hashtable<ValueClass,KeyClass,HashFunctionType,KeyExtract,KeyEqualsType>& ht2) 
{
    return !(ht1 == ht2);
}


template <class ValueClass, class KeyClass, class HashFunctionType, class KeyExtract, class KeyEqualsType>
IFRUtil_Pair<IFR_TYPENAME IFRUtil_Hashtable<ValueClass,KeyClass,HashFunctionType,KeyExtract,KeyEqualsType>::Iterator, IFR_Bool> 
IFRUtil_Hashtable<ValueClass,KeyClass,HashFunctionType,KeyExtract,KeyEqualsType>
  ::insert_unique_noresize(const ValueType& obj)
{
    const SizeType n = m_bkt_num(obj);
    Node* first = m_buckets[n];
    
    for (Node* cur = first; cur; cur = cur->m_next) 
        if (m_equals(m_get_key(cur->m_val), m_get_key(obj)))
            return IFRUtil_Pair<Iterator, IFR_Bool>(Iterator(cur, this), false);
    
    Node* tmp = m_new_node(obj);
    tmp->m_next = first;
    m_buckets[n] = tmp;
    ++m_num_elements;
    return IFRUtil_Pair<Iterator, IFR_Bool>(Iterator(tmp, this), true);
}

template <class ValueClass, class KeyClass, class HashFunctionType, class KeyExtract, class KeyEqualsType>
IFR_TYPENAME IFRUtil_Hashtable<ValueClass,KeyClass,HashFunctionType,KeyExtract,KeyEqualsType>::Iterator 
IFRUtil_Hashtable<ValueClass,KeyClass,HashFunctionType,KeyExtract,KeyEqualsType>
  ::insert_equal_noresize(const ValueType& obj)
{
    const SizeType n = m_bkt_num(obj);
    Node* first = m_buckets[n];
    
    for (Node* cur = first; cur; cur = cur->m_next) 
        if (m_equals(m_get_key(cur->m_val), m_get_key(obj))) {
            Node* tmp = m_new_node(obj);
            tmp->m_next = cur->m_next;
            cur->m_next = tmp;
            ++m_num_elements;
            return Iterator(tmp, this);
        }
    
    Node* tmp = m_new_node(obj);
    tmp->m_next = first;
    m_buckets[n] = tmp;
    ++m_num_elements;
    return Iterator(tmp, this);
}

template <class ValueClass, class KeyClass, class HashFunctionType, class KeyExtract, class KeyEqualsType>
IFR_TYPENAME IFRUtil_Hashtable<ValueClass,KeyClass,HashFunctionType,KeyExtract,KeyEqualsType>::Reference 
IFRUtil_Hashtable<ValueClass,KeyClass,HashFunctionType,KeyExtract,KeyEqualsType>::find_or_insert(const ValueType& obj, 
                                                                                                 IFR_Bool& memory_ok)
{
    resize(m_num_elements + 1, memory_ok);
    if(!memory_ok) {
        return m_buckets[m_bkt_num(obj)]->m_val;
    }

    SizeType n = m_bkt_num(obj);
    Node* first = m_buckets[n];
    
    for (Node* cur = first; cur; cur = cur->m_next)
        if (m_equals(m_get_key(cur->m_val), m_get_key(obj)))
            return cur->m_val;
    
    Node* tmp = m_new_node(obj);
    tmp->m_next = first;
    m_buckets[n] = tmp;
    ++m_num_elements;
    return tmp->m_val;
}


template <class ValueClass, class KeyClass, class HashFunctionType, class KeyExtract, class KeyEqualsType>
IFRUtil_Pair<IFR_TYPENAME IFRUtil_Hashtable<ValueClass,KeyClass,HashFunctionType,KeyExtract,KeyEqualsType>::Iterator,
     IFR_TYPENAME IFRUtil_Hashtable<ValueClass,KeyClass,HashFunctionType,KeyExtract,KeyEqualsType>::Iterator> 
IFRUtil_Hashtable<ValueClass,KeyClass,HashFunctionType,KeyExtract,KeyEqualsType>::equal_range(const KeyType& key)
{
    const SizeType n = m_bkt_num_key(key);
    
    for (Node* first = m_buckets[n]; first; first = first->m_next)
        if (m_equals(m_get_key(first->m_val), key)) {
            for (Node* cur = first->m_next; cur; cur = cur->m_next)
                if (!m_equals(m_get_key(cur->m_val), key))
                    return IFRUtil_Pair<Iterator, Iterator>(Iterator(first, this), Iterator(cur, this));
            for (SizeType m = n + 1; m < m_buckets.size(); ++m)
                if (m_buckets[m])
                    return IFRUtil_Pair<Iterator, Iterator>(Iterator(first, this),
                                                            Iterator(m_buckets[m], this));
            return IFRUtil_Pair<Iterator, Iterator>(Iterator(first, this), end());
        }
    return IFRUtil_Pair<Iterator, Iterator>(end(), end());
}

template <class ValueClass, class KeyClass, class HashFunctionType, class KeyExtract, class KeyEqualsType>
IFRUtil_Pair<IFR_TYPENAME IFRUtil_Hashtable<ValueClass,KeyClass,HashFunctionType,KeyExtract,KeyEqualsType>::ConstIterator, 
             IFR_TYPENAME IFRUtil_Hashtable<ValueClass,KeyClass,HashFunctionType,KeyExtract,KeyEqualsType>::ConstIterator> 
IFRUtil_Hashtable<ValueClass,KeyClass,HashFunctionType,KeyExtract,KeyEqualsType>
::equal_range(const KeyType& key) const
{
    const SizeType n = m_bkt_num_key(key);
    
    for (const Node* first = m_buckets[n] ;
         first; 
         first = first->m_next) {
        if (m_equals(m_get_key(first->m_val), key)) {
            for (const Node* cur = first->m_next;
                 cur;
                 cur = cur->m_next)
                if (!m_equals(m_get_key(cur->m_val), key))
                    return IFRUtil_Pair<Iterator, Iterator>(ConstIterator(first, this),
                                                            ConstIterator(cur, this));
            for (SizeType m = n + 1; m < m_buckets.size(); ++m)
                if (m_buckets[m])
                    return IFRUtil_Pair<Iterator, Iterator>(ConstIterator(first, this),
                                                            ConstIterator(m_buckets[m], this));
            return IFRUtil_Pair<Iterator, Iterator>(ConstIterator(first, this), end());
        }
    }
    return IFRUtil_Pair<Iterator, Iterator>(end(), end());
}

template <class ValueClass, class KeyClass, class HashFunctionType, class KeyExtract, class KeyEqualsType>
IFR_TYPENAME IFRUtil_Hashtable<ValueClass,KeyClass,HashFunctionType,KeyExtract,KeyEqualsType>::SizeType 
inline IFRUtil_Hashtable<ValueClass,KeyClass,HashFunctionType,KeyExtract,KeyEqualsType>::erase(const KeyType& key)
{
    const SizeType n = m_bkt_num_key(key);
    Node* first = m_buckets[n];
    SizeType erased = 0;
    
    if (first) {
        Node* cur = first;
        Node* next = cur->m_next;
        while (next) {
            if (m_equals(m_get_key(next->m_val), key)) {
                cur->m_next = next->m_next;
                m_delete_node(next);
                next = cur->m_next;
                ++erased;
                --m_num_elements;
            }
            else {
                cur = next;
                next = cur->m_next;
            }
        }
        if (m_equals(m_get_key(first->m_val), key)) {
            m_buckets[n] = first->m_next;
            m_delete_node(first);
            ++erased;
            --m_num_elements;
        }
    }
    return erased;
}

template <class ValueClass, class KeyClass, class HashFunctionType, class KeyExtract, class KeyEqualsType>
void IFRUtil_Hashtable<ValueClass,KeyClass,HashFunctionType,KeyExtract,KeyEqualsType>::erase(const Iterator& it)
{
    Node* p = it.m_cur;
    if (p) {
        const SizeType n = m_bkt_num(p->m_val);
        Node* cur = m_buckets[n];
        
        if (cur == p) {
            m_buckets[n] = cur->m_next;
            m_delete_node(cur);
            --m_num_elements;
        }
        else {
            Node* next = cur->m_next;
            while (next) {
                if (next == p) {
                    cur->m_next = next->m_next;
                    m_delete_node(next);
                    --m_num_elements;
                    break;
                } else {
                    cur = next;
                    next = cur->m_next;
                }
            }
        }
    }
}

template <class ValueClass, class KeyClass, class HashFunctionType, class KeyExtract, class KeyEqualsType>
void IFRUtil_Hashtable<ValueClass,KeyClass,HashFunctionType,KeyExtract,KeyEqualsType>
  ::erase(Iterator first, Iterator last)
{
    SizeType f_bucket = first.m_cur ? 
        m_bkt_num(first.m_cur->m_val) : m_buckets.size();
    SizeType l_bucket = last.m_cur ? 
        m_bkt_num(last.m_cur->m_val) : m_buckets.size();
    
    if (first.m_cur == last.m_cur)
        return;
    else if (f_bucket == l_bucket)
        m_erase_bucket(f_bucket, first.m_cur, last.m_cur);
    else {
        m_erase_bucket(f_bucket, first.m_cur, 0);
        for (SizeType n = f_bucket + 1; n < l_bucket; ++n)
            m_erase_bucket(n, 0);
        if (l_bucket != m_buckets.size())
            m_erase_bucket(l_bucket, last.m_cur);
  }
}

template <class ValueClass, class KeyClass, class HashFunctionType, class KeyExtract, class KeyEqualsType>
inline void
IFRUtil_Hashtable<ValueClass,KeyClass,HashFunctionType,KeyExtract,KeyEqualsType>::erase(ConstIterator first,
                                             ConstIterator last)
{
    erase(Iterator(const_cast<Node*>(first.m_cur),
                   const_cast<IFRUtil_Hashtable*>(first.m_ht)),
          Iterator(const_cast<Node*>(last.m_cur),
                   const_cast<IFRUtil_Hashtable*>(last.m_ht)));
}

template <class ValueClass, class KeyClass, class HashFunctionType, class KeyExtract, class KeyEqualsType>
inline void
IFRUtil_Hashtable<ValueClass,KeyClass,HashFunctionType,KeyExtract,KeyEqualsType>::erase(const ConstIterator& it)
{
    erase(Iterator(const_cast<Node*>(it.m_cur),
                   const_cast<IFRUtil_Hashtable*>(it.m_ht)));
}

template <class ValueClass, class KeyClass, class HashFunctionType, class KeyExtract, class KeyEqualsType>
void IFRUtil_Hashtable<ValueClass,KeyClass,HashFunctionType,KeyExtract,KeyEqualsType>
::resize(SizeType num_elements_hint, IFR_Bool& memory_ok)
{
    if(!memory_ok) {
        return;
    }
    const SizeType old_n = m_buckets.GetSize();
    if (num_elements_hint > old_n) {
        const SizeType n = m_next_size(num_elements_hint);
        if (n > old_n) {
            IFRUtil_Vector<Node*> tmp(m_buckets.GetRawAllocator(), n, (Node*)(0), memory_ok);
            if(!memory_ok) {
                return;
            }
            for (SizeType bucket = 0; bucket < old_n; ++bucket) {
                Node* first = m_buckets[bucket];
                while (first) {
                    SizeType new_bucket = m_bkt_num(first->m_val, n);
                    m_buckets[bucket] = first->m_next;
                    first->m_next = tmp[new_bucket];
                    tmp[new_bucket] = first;
                    first = m_buckets[bucket];          
                }
            }
            m_buckets.Swap(tmp, memory_ok);
        }
    }
}


template <class ValueClass, class KeyClass, class HashFunctionType, class KeyExtract, class KeyEqualsType>
void IFRUtil_Hashtable<ValueClass,KeyClass,HashFunctionType,KeyExtract,KeyEqualsType>
  ::m_erase_bucket(const SizeType n, Node* first, Node* last)
{
    Node* cur = m_buckets[n];
    if (cur == first)
        m_erase_bucket(n, last);
    else {
        Node* next;
        for (next = cur->m_next; 
             next != first; 
             cur = next, next = cur->m_next)
            ;
        while (next != last) {
            cur->m_next = next->m_next;
            m_delete_node(next);
            next = cur->m_next;
            --m_num_elements;
        }
    }
}

template <class ValueClass, class KeyClass, class HashFunctionType, class KeyExtract, class KeyEqualsType>
void IFRUtil_Hashtable<ValueClass,KeyClass,HashFunctionType,KeyExtract,KeyEqualsType>
  ::m_erase_bucket(const SizeType n, Node* last)
{
    Node* cur = m_buckets[n];
    while (cur != last) {
        Node* next = cur->m_next;
        m_delete_node(cur);
        cur = next;
        m_buckets[n] = cur;
        --m_num_elements;
    }
}

template <class ValueClass, class KeyClass, class HashFunctionType, class KeyExtract, class KeyEqualsType>
void IFRUtil_Hashtable<ValueClass,KeyClass,HashFunctionType,KeyExtract,KeyEqualsType>::clear()
{
    SizeType bucket_size=m_buckets.GetSize();
    for (SizeType i = 0; i < bucket_size; ++i) {
        Node* cur = m_buckets[i];
        while (cur != 0) {
            Node* next = cur->m_next;
            m_delete_node(cur);
            cur = next;
        }
        m_buckets[i] = 0;
    }
    m_num_elements = 0;
}

    
template <class ValueClass, class KeyClass, class HashFunctionType, class KeyExtract, class KeyEqualsType>
void IFRUtil_Hashtable<ValueClass,KeyClass,HashFunctionType,KeyExtract,KeyEqualsType>
  ::m_copy_from(const IFRUtil_Hashtable& ht)
{
  m_buckets.clear();
  m_buckets.reserve(ht.m_buckets.size());
  m_buckets.insert(m_buckets.end(), ht.m_buckets.size(), (Node*) 0);
  for (SizeType i = 0; i < ht.m_buckets.size(); ++i) {
      const Node* cur = ht.m_buckets[i];
      if (cur) {
          Node* copy = m_new_node(cur->m_val);
          m_buckets[i] = copy;
          
          for (Node* next = cur->m_next; 
               next; 
               cur = next, next = cur->m_next) {
              copy->m_next = m_new_node(next->m_val);
              copy = copy->m_next;
          }
      }
  }
  m_num_elements = ht.m_num_elements;
}

IFR_END_NAMESPACE

#endif  
