/*!
  @file           IFRUtil_HashMap.h
  @author         D039759
  @ingroup        IFR_Util
  @brief          

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
#ifndef IFRUTIL_HASHMAP_H
#define IFRUTIL_HASHMAP_H

#include "Interfaces/Runtime/Util/IFRUtil_Hashtable.h"
#include "Interfaces/Runtime/Util/IFRUtil_Pair.h"

IFR_BEGIN_NAMESPACE

template <class KeyTypeClass, class MappedTypeClass, class HashFunctionClass, class KeyEqualsClass>
class IFRUtil_HashMap
{
private:
    typedef IFRUtil_Hashtable<IFRUtil_Pair<const KeyTypeClass, MappedTypeClass>,
                              KeyTypeClass,
                              HashFunctionClass,
                              IFRUtil_Select1st<IFRUtil_Pair<const KeyTypeClass, MappedTypeClass> >,
                              KeyEqualsClass> Hashtable;
    
    Hashtable m_ht;
public:
    typedef typename Hashtable::KeyType KeyType;
    typedef MappedTypeClass DataType;
    typedef MappedTypeClass MappedType;
    typedef typename Hashtable::ValueType ValueType;
    typedef typename Hashtable::HashFunction HashFunction;
    typedef typename Hashtable::KeyEquals KeyEquals;
  
    typedef typename Hashtable::SizeType SizeType;
    typedef typename Hashtable::DifferenceType DifferenceType;
    typedef typename Hashtable::Pointer Pointer;
    typedef typename Hashtable::ConstPointer ConstPointer;
    typedef typename Hashtable::Reference Reference;
    typedef typename Hashtable::ConstReference ConstReference;

    typedef typename Hashtable::Iterator Iterator;
    typedef typename Hashtable::ConstIterator ConstIterator;
    typedef typename Hashtable::AllocatorType AllocatorType;

    HashFunction hash_funct() const { return m_ht.hash_funct(); }
    KeyEquals key_eq() const { return m_ht.key_eq(); }
    AllocatorType get_allocator() const { return m_ht.get_allocator(); }

public:
    IFRUtil_HashMap(AllocatorType allocator, IFR_Bool& memory_ok) 
    :m_ht(100, HashFunction(), KeyEquals(), allocator, memory_ok) 
    {
    }
    
    explicit IFRUtil_HashMap(SizeType n, AllocatorType allocator, IFR_Bool& memory_ok)
    :m_ht(n, HashFunction(), KeyEquals(), allocator, memory_ok) 
    {}
    
    IFRUtil_HashMap(SizeType n, const HashFunction& hf, AllocatorType allocator, IFR_Bool& memory_ok)
    :m_ht(n, hf, KeyEquals(), allocator, memory_ok) 
    {}
    
    IFRUtil_HashMap(SizeType n, 
                    const HashFunction& hf, 
                    const KeyEquals& eql,
                    AllocatorType a,
                    IFR_Bool& memory_ok)
    : m_ht(n, hf, eql, a) 
    {}

    IFRUtil_HashMap(const ValueType* f, const ValueType* l)
    : m_ht(100, HashFunction(), KeyEquals())
    { 
        m_ht.insert_unique(f, l); 
    }
    
    IFRUtil_HashMap(const ValueType* f, const ValueType* l, SizeType n)
    :m_ht(n, HashFunction(), KeyEquals())
    { 
        m_ht.insert_unique(f, l); 
    }
    
    IFRUtil_HashMap(const ValueType* f, const ValueType* l, SizeType n,
                    const HashFunction& hf)
    :m_ht(n, hf, KeyEquals())
    { 
        m_ht.insert_unique(f, l);
    }
    IFRUtil_HashMap(const ValueType* f, const ValueType* l, SizeType n,
                    const HashFunction& hf, const KeyEquals& eql,
                    AllocatorType a)
    :m_ht(n, hf, eql, a)
    { 
        m_ht.insert_unique(f, l); 
    }

    IFRUtil_HashMap(ConstIterator f, ConstIterator l)
    :m_ht(100, HashFunction(), KeyEquals())
    { 
        m_ht.insert_unique(f, l); 
    }
    
    IFRUtil_HashMap(ConstIterator f, ConstIterator l, SizeType n)
    :m_ht(n, HashFunction(), KeyEquals())
    { 
        m_ht.insert_unique(f, l); 
    }
    
    IFRUtil_HashMap(ConstIterator f, ConstIterator l, SizeType n,
                    const HashFunction& hf)
    :m_ht(n, hf, KeyEquals())
    { 
        m_ht.insert_unique(f, l); 
    }
    
    IFRUtil_HashMap(ConstIterator f, ConstIterator l, SizeType n,
                    const HashFunction& hf, const KeyEquals& eql,
                    AllocatorType a)
    :m_ht(n, hf, eql, a)
    { 
        m_ht.insert_unique(f, l); 
    }
    
public:
    SizeType size() const { return m_ht.size(); }
    IFR_Bool empty() const { return m_ht.empty(); }
    void swap(IFRUtil_HashMap& hs) 
    { 
        m_ht.swap(hs.m_ht); 
    }
   

    Iterator begin() { return m_ht.begin(); }
    Iterator end() { return m_ht.end(); }
    ConstIterator begin() const { return m_ht.begin(); }
    ConstIterator end() const { return m_ht.end(); }
public:
    IFRUtil_Pair<Iterator,IFR_Bool> insert(const ValueType& obj)
    { 
        return m_ht.insert_unique(obj); 
    }
    void insert(const ValueType* f, const ValueType* l) 
    {
        m_ht.insert_unique(f,l);
    }

    void insert(ConstIterator f, ConstIterator l)
    { 
        m_ht.insert_unique(f, l); 
    }
    
    IFRUtil_Pair<Iterator,IFR_Bool> insert_noresize(const ValueType& obj)
    { 
        return m_ht.insert_unique_noresize(obj); 
    }    

    Iterator find(const KeyType& key) 
    { 
        return m_ht.find(key); 
    }
    
    ConstIterator find(const KeyType& key) const 
    { 
        return m_ht.find(key); 
    }

    MappedTypeClass& get(const KeyType& key, IFR_Bool& memory_ok) 
    {
        return m_ht.find_or_insert(ValueType(key, MappedTypeClass()),memory_ok).second;
    }

    SizeType count(const KeyType& key) const 
    { 
        return m_ht.count(key); 
    }
  
    IFRUtil_Pair<Iterator, Iterator> equal_range(const KeyType& key)
    { 
        return m_ht.equal_range(key); 
    }
    
    IFRUtil_Pair<ConstIterator, ConstIterator>  equal_range(const KeyType& key) const
    { 
        return m_ht.equal_range(key); 
    }

    SizeType erase(const KeyType& key) 
    {
        return m_ht.erase(key); 
    }
    
    void erase(Iterator it) 
    { 
        m_ht.erase(it); 
    }
    
    void erase(Iterator f, Iterator l) 
    { 
        m_ht.erase(f, l); 
    }
    
    void clear() 
    { 
        m_ht.clear(); 
    }
    
    void resize(SizeType hint) 
    { 
        m_ht.resize(hint); 
    }
    
    SizeType bucket_count() const 
    { 
        return m_ht.bucket_count(); 
    }
    
    SizeType max_bucket_count() const 
    { 
        return m_ht.max_bucket_count(); 
    }
    
    SizeType elems_in_bucket(SizeType n) const
    { 
        return m_ht.elems_in_bucket(n); 
    }
};

IFR_END_NAMESPACE

#endif  
