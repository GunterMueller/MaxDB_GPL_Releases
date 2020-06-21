/*!
  @file           IFRUtil_Vector.h
  @author         D030044
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
#ifndef IFRUTIL_VECTOR_H
#define IFRUTIL_VECTOR_H

#include <stdio.h>
#include "Interfaces/Runtime/IFR_Types.h"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "Interfaces/Runtime/Util/IFRUtil_Algo.h"
#include "Interfaces/Runtime/Util/IFRUtil_CopyTraits.h"

#ifndef NMP
#  define IFRUTIL_VEC_T_DEFAULT_INIT =T()
#else
#  define IFRUTIL_VEC_T_DEFAULT_INIT
#endif

/**
 * @ingroup IFR_Util
 * @brief This class implements a vector class.
 *
 * The vector uses raw allocators for allocating memory for the vector
 * elements. It is not possible to construct a vector without
 * supplying a raw allocator.  Thus, the default constructor for a
 * vector is not offered.
 * 
 * The type <code>T</code> held in the vector must be default
 * constructible, or featuring a copy constructor.
 * 
 * The following example creates a vector of 10 <code>IFR_Int4</code> values,
 * all creates using a default constructor.
 * <pre>IFRUtil_Vector<IFR_Int4>  myVector(IFRUtil_DefaultRawAllocator::Instance(), 10);</pre>
 */
template <class T>
class IFRUtil_Vector
{
public:
    typedef T                ValueType;     //!< The type of the elements (values).
    typedef IFR_size_t       SizeType;      //!< Range type for index and size information.
    typedef IFR_ptrdiff_t    DifferenceType;//!< Difference type of iterator arithmetic.
    
    typedef ValueType*       Pointer;       //!< Pointer to <code>ValueType</code>.
    typedef const ValueType* ConstPointer;  //!< Const pointer to <code>ValueType</code>.
    typedef ValueType&       Reference;     //!< Reference of <code>ValueType</code>.
    typedef const ValueType& ConstReference;//!< Const reference of <code>ValueType</code>.
    
    typedef Pointer          Iterator;      //!< Iterator on vector elements.
    typedef ConstPointer     ConstIterator; //!< Iterator on constant vector elements.

private:
    SAPDBMem_IRawAllocator* m_rawallocator; //!< The used raw allocator.
    Pointer m_ArrayPtr;                     //!< The pointer to the vector elements.
    SizeType m_Size;                        //!< The number of vector elements
    SizeType m_Capacity;                    //!< The number of vector elements that can be stored without resizing the vector.
    ValueType m_InitVal;                    //!< The initialisation value. 

protected:
    inline void setSize(SizeType size) 
    {
        m_Size = size;
    }

public:
    /**
     * Constructs a vector that will use the provided allocator.
     * @param RawAlloc The raw allocator to be used for this vector.
     */
    IFRUtil_Vector(SAPDBMem_IRawAllocator& RawAlloc)
    :m_rawallocator(&RawAlloc), 
     m_Size(0), 
     m_Capacity(0), 
     m_ArrayPtr(0),
     m_InitVal()
    {}


    /**
     * Constructs a vector that will have the given initial number of
     * elements. The elements are initialized using the default
     * constructor.
     * @param RawAlloc The raw allocator to be used for this vector.
     * @param Size     The number of elements to put into the vector.
    */
    inline IFRUtil_Vector(SAPDBMem_IRawAllocator& RawAlloc,
                   SizeType Size)
    :m_rawallocator(&RawAlloc), 
     m_Size(Size), 
     m_Capacity(Size),
     m_ArrayPtr(0),
     m_InitVal()
    {
        if (Size > 0) {
            IFRUtil_NewArray(m_ArrayPtr, Size, IFR_ALLOCATOR(*m_rawallocator));
        } else {
            m_ArrayPtr=0;
        }
    }


    /**
     * Constructs a vector that will have the given number of elements,
     * all initialized with the provided <code>Init</code> value.
     * @param RawAlloc The raw allocator to be used for this vector.
     * @param Size     The number of elements to put into the vector.
     * @param Init     The value to use as initialization for the elements.
     */
    inline IFRUtil_Vector(SAPDBMem_IRawAllocator& RawAlloc,
                   SizeType Size,
                   ConstReference Init,
                   IFR_Bool& memory_ok)
    :m_rawallocator(&RawAlloc),
     m_Size(Size),
     m_Capacity(Size),
     m_ArrayPtr(0),
     m_InitVal(IFRUtil_CopyTraits<ValueType>::init_helper(&RawAlloc, Init))
    {
        IFRUtil_CopyTraits<ValueType>::copy(m_InitVal, Init, memory_ok);
        if(!memory_ok) {
            m_Size = m_Capacity = 0;
            m_ArrayPtr = 0;
            return;
        }
        if (Size > 0) {
            IFRUtil_NewArray(m_ArrayPtr, Size, Init, 
                             IFR_ALLOCATOR(*m_rawallocator), 
                             memory_ok);
            if(!memory_ok) {
                m_Size = m_Capacity = 0;
                m_ArrayPtr = 0;
                return;
            }
        } else {
            m_ArrayPtr=0;
        }
    }
     
    
    /**
     * Copy constructor.
     * @param InitVector The vector to copy.
     * @param initValue  Optional initialisation value.
     */
    inline IFRUtil_Vector(const IFRUtil_Vector<T>& InitVector, 
                          IFR_Bool &memory_ok,
                          const T& initValue IFRUTIL_VEC_T_DEFAULT_INIT)
    :m_rawallocator(InitVector.m_rawallocator),
     m_Size(InitVector.m_Size),
     m_Capacity(InitVector.m_Capacity),
     m_ArrayPtr(0),
     m_InitVal(initValue)
    {
        if(!memory_ok) {
            m_Size=m_Capacity=0;
        }
        if(m_Size > 0) {
            IFRUtil_NewArray(m_ArrayPtr, 
                             InitVector.m_ArrayPtr, 
                             InitVector.m_ArrayPtr + m_Size, 
                             IFR_ALLOCATOR(*m_rawallocator),
                             memory_ok);
        } else {
            m_ArrayPtr = 0;
        }
    }
     
    
    /**
     * Copy constructor, which changes the allocator.
     * @param RawAlloc The allocator that shall be used.
     * @param InitVector The vector to copy.
     */
    inline IFRUtil_Vector(SAPDBMem_IRawAllocator& RawAlloc,
                          const IFRUtil_Vector<T>& InitVector,
                          const T& initValue,
                          IFR_Bool& memory_ok)
    :m_rawallocator(&RawAlloc),
     m_Size(InitVector.m_Size),
     m_Capacity(InitVector.m_Size),
     m_ArrayPtr(0),
     m_InitVal(IFRUtil_CopyTraits<ValueType>::init_helper(&RawAlloc, initValue))
    {
        if(!memory_ok) {
            m_Size = m_Capacity = 0;
            m_ArrayPtr = 0;
            return;
        }
        if(m_Size > 0) {
            IFRUtil_NewArray(m_ArrayPtr, 
                             InitVector.m_ArrayPtr, 
                             InitVector.m_ArrayPtr + m_Size, 
                             IFR_ALLOCATOR(*m_rawallocator),
                             memory_ok);
        } else {
            m_ArrayPtr = 0;
        }
    }

    /**
     * Destructor.
     */
    inline ~IFRUtil_Vector()
    {
        IFRUtil_DeleteArray(m_ArrayPtr, m_Size, IFR_ALLOCATOR(*m_rawallocator));
    }

    inline SAPDBMem_IRawAllocator& allocator()
    {
        return *m_rawallocator;
    }

    /**
     * Get the data.
     * @return The pointer to the array data.
     */
    inline const T * Data() const
    {
        return (ConstPointer) m_ArrayPtr;
    } 

    /**
     * Get the data.
     * @return The pointer to the array data.
     */
    inline T* Data()
    {
        return (Pointer) m_ArrayPtr;
    } 

    /**
     * Returns the number of vector elements within the vector
     * @return The number of elements in the vector.
     */
    inline SizeType GetSize() const { return m_Size; }
    
    /**
     * Returns <code>true</code> if the vector is empty.
     * @return <code>true</code> if the vector is empty, <code>false</code> if not.
     */
    inline IFR_Bool IsEmpty() const { return (m_Size == 0); }

    /**
     * Gets the capacity of the vector, i.e. the number of element the vector
     * can store without performing a reallocation.
     * @return The current capacity of the vector, which is always less or equal
     *         the size (see <code>GetSize()</code>).
     */
    inline SizeType GetCapacity() const { return m_Capacity; }
    
    /**
     * Gets the raw allocator used by this vector.
     * @return A reference to the allocator object used by this vector.
     */
    inline SAPDBMem_IRawAllocator& GetRawAllocator() const { return ((*m_rawallocator)); }

    /**
     * Compares this vector with the other vector.
     * @param CompVector The vector to compare.
     * @return <code>true</code>, if <code>CompVector</code> has the same
     *         size as this vector, and <code>(*this)[i] == CompVector[i]</code>
     *         is true for all <code>i</code> in <code>[0, GetSize())</code>.
     */
    inline IFR_Bool operator ==(const IFRUtil_Vector<T>& CompVector) const
    {
        if(this == &CompVector) {
            return true;
        }
        if(this->m_Size != &CompVector.m_Size) {
            return false;
        }
        for(SizeType i=0; i<m_Size; ++i) {
            if(!(m_ArrayPtr[i] == CompVector.m_ArrayPtr[i])) {
                return false;
            }
        }
        return true;
    }

    /**
     * Compares this vector with the other vector.
     * @param CompVector The vector to compare.
     * @return <code>true</code>, if <code>CompVector</code> has a different
     *         size as this vector, or <code>(*this)[i] == CompVector[i]</code>
     *         is false for any <code>i</code> in <code>[0, GetSize())</code>.
     */
    inline IFR_Bool operator !=(const IFRUtil_Vector<T>& CompVector) const    
    { 
        return !(this->operator==(CompVector)); 
    }

    /**
     * Gets the index that belongs to an iterator.
     * @param Iter The iterator.
     * @return The index of the value identified by the iterator.
     */
    inline SizeType GetIndex(const Iterator& Iter) const
    {
        return (Iter - m_ArrayPtr);
    }
    
    /**
     * Changes the size of the vector. If the size is decreased, the 
     * removed elements are destroyed. If the size is increased, the
     * default constructor is called for the newly inserted elements.
     * <b>Note that all iterators may be invalidated by this function!</b>
     * @param NewSize The new size of the vector.
     */
    inline void Resize(SizeType NewSize, IFR_Bool& memory_ok)
    {
        if(!memory_ok) {
            return;
        }
        
        if(NewSize < m_Size) {
            IFRUtil_DestroyArray(m_ArrayPtr + NewSize, m_ArrayPtr + m_Size);
        } else if(NewSize > m_Size) {
            Reserve(NewSize, memory_ok);
            if(!memory_ok) {
                return;
            }
            IFRUtil_ConstructArray(m_ArrayPtr + m_Size, m_ArrayPtr + NewSize, m_InitVal, memory_ok);
            if(!memory_ok) {
                return;
            }
        }
        m_Size = NewSize;
    }
    
    /**
     * Changes the size of the vector. If the size is decreased, the 
     * removed elements are destroyed. If the size is increased, the
     * copy constructor is called with <code>Init</code> as argument 
     * for the newly inserted elements.
     * <b>Note that all iterators may be invalidated by this function!</b>
     * @param NewSize The new size of the vector.
     * @param Init    The initialization value for new elements.
     */
    inline void Resize(SizeType NewSize, ConstReference Init, IFR_Bool& memory_ok)
    {
        if(!memory_ok) {
            return;
        }
        if(NewSize < m_Size) {
            IFRUtil_DestroyArray(m_ArrayPtr + NewSize, m_ArrayPtr + m_Size);
        } else if(NewSize > m_Size) {
            Reserve(NewSize, memory_ok);
            if(!memory_ok) {
                return;
            }
            IFRUtil_ConstructArray(m_ArrayPtr + m_Size, m_ArrayPtr + NewSize, Init, memory_ok);
            if(!memory_ok) {
                return;
            }
        }
        m_Size = NewSize;
    }
    
    /**
     * Changes the capacity of the vector.
     * If the requested capacity is larger as the current capacity,
     * new memory is allocated to fulfill the capacity increment. 
     * <b>Note that all iterators may be invalidated by this function!</b>
     * @param NewCapacity The request capacity.
     * @todo Check for allocator reallocation semantics. 
     */
    inline void Reserve(SizeType NewCapacity, IFR_Bool& memory_ok)
    {
        if(!memory_ok) {
            return;
        }
        if(m_Capacity >= NewCapacity) {
            return;
        }
        
        // determine smallest x such that 2^x is >= NewCapacity
        SizeType sz=1;
        while(sz < NewCapacity) 
            sz = sz << 1;
        // Currently, we use simple allocation without hint.
        Pointer new_ArrayPtr = (Pointer) IFR_ALLOCATOR(*m_rawallocator).Allocate(sz * sizeof(ValueType));
        if(new_ArrayPtr == 0) {
            memory_ok=false;
        } else {
            memcpy(new_ArrayPtr, m_ArrayPtr, sizeof(ValueType) * m_Size);
            if(m_ArrayPtr) {
                IFR_ALLOCATOR(*m_rawallocator).Deallocate(m_ArrayPtr);
            }
            m_ArrayPtr=new_ArrayPtr;
            m_Capacity=sz;
        }
        return;
    }

    /**
     * Assignment operator. Note that only the elements are
     * assigned, and not the allocator.
     * @param InitVector The vector to assign to this vector.
     * @return <code>*this</code>.
     */
    inline void assign(const IFRUtil_Vector<T>& InitVector, IFR_Bool& memory_ok)
    {
        
        if(&InitVector == this) {
            return;
        }
        if(!memory_ok) {
            return;
        }
        // First clear, such that less elements are copied on reserve.
        Clear();
        Reserve(InitVector.GetSize(), memory_ok);
        if(!memory_ok) {
            return;
        }
        ConstIterator input=InitVector.Begin();
        ConstIterator input_end=InitVector.End();
        ValueType *p=m_ArrayPtr;
        while(input != input_end) {
            IFRUtil_CopyTraits<ValueType>::copy(*p, *input, memory_ok);
            if(!memory_ok) {
                return;
            }
            ++p;
            ++input;
        }
        m_Size=InitVector.m_Size;
        return;
    }
    /**
     * Exchanges the contents of this vector with another.
     * @param v The vector whose contents shall be swapped with this
     *          one.
     */
    inline void Swap(IFRUtil_Vector<T>& v, IFR_Bool& memory_ok)
    {
        SAPDBMem_IRawAllocator *tmp_allocator=v.m_rawallocator;
        v.m_rawallocator = m_rawallocator;
        m_rawallocator = tmp_allocator;

        Pointer tmp_arrayptr=v.m_ArrayPtr;
        v.m_ArrayPtr = m_ArrayPtr;
        m_ArrayPtr = tmp_arrayptr;
        
        SizeType tmp_Size=v.m_Size;
        v.m_Size = m_Size;
        m_Size = tmp_Size;

        SizeType tmp_Capacity=v.m_Capacity;
        v.m_Capacity = m_Capacity;
        m_Capacity = tmp_Capacity;
        
        ValueType tmp_initval(IFRUtil_CopyTraits<ValueType>::init_helper(m_rawallocator, m_InitVal));
        IFRUtil_CopyTraits<ValueType>::copy(tmp_initval, v.m_InitVal, memory_ok);
        IFRUtil_CopyTraits<ValueType>::copy(v.m_InitVal, m_InitVal, memory_ok);
        IFRUtil_CopyTraits<ValueType>::copy(m_InitVal, tmp_initval, memory_ok);
        
        return;
    }


    /**
     * Retrieve an element by index.
     * @param n The index.
     * @return The n-th element as reference.
     */
    inline Reference operator[](SizeType n)
    {
        return *(m_ArrayPtr + n);
    }

    /**
     * Retrieve an element by index.
     * @param n The index.
     * @return The n-th element as const reference.
     */
    inline ConstReference operator[](SizeType n) const
    {
        return *(m_ArrayPtr + n);
    }
    
    /**
     * Get an iterator pointing to the n-th element.
     * @param n The index. If <code>n</code> is equal to the size, the 
     *          result will be the <code>End()</code> iterator.
     * @return An iterator pointing at the position specified by <code>n</code>.
     */
    inline Iterator Position(SizeType n)
    {
        // An iterator to the n-th element (==m_Size) corresponds to the End() iterator
        if (n < m_Size) return NULL;
        return m_ArrayPtr + n;
    }
    
    /**
     * Get an iterator pointing to the n-th element.
     * @param n The index. If <code>n</code> is equal to the size, the 
     *          result will be the <code>End()</code> iterator.
     * @return A const iterator pointing at the position specified by <code>n</code>.
     */
    inline ConstIterator Position(SizeType n) const
    {
        // An iterator to the n-th element (==m_Size) corresponds to the End() iterator
        if (n < m_Size) return NULL;
        return m_ArrayPtr + n;
    }

    /** 
     * Inserts an element at the beginning of the vector. The other
     * elements are moved one position, thus this
     * method may be rather slow.
     * @param Elem The element to insert
     * @return An iterator pointing at the inserted element.
     */
    inline Iterator InsertFront(ConstReference Elem, IFR_Bool& memory_ok)
    {
        Iterator Iter= Begin();
        return Insert(Iter, Elem, memory_ok);
    }
    
    /** 
     * Inserts an element at the end of the vector. 
     * @param Elem The element to insert
     * @return An iterator pointing at the inserted element.
     */
    inline Iterator InsertEnd(ConstReference Elem, 
                              IFR_Bool& memory_ok)
    {
        Resize(m_Size + 1, Elem, memory_ok);
        if(memory_ok) {
            return Position(m_Size - 1);
        } else {
            return End();
        }
    }

    /**
     * Inserts an element at a specified position.
     * The remaining elements are moved by one position, the
     * size is increased by one.
     * @param Pos The position.
     * @param Elem The value to insert.
     * @return An iterator pointing at the inserted element.
     */
    inline Iterator Insert(Iterator& Pos, ConstReference Elem, IFR_Bool& memory_ok)
    {
        if(Pos == End()) {
            return InsertEnd(Elem, memory_ok);
        }
        // Must be done before the re-allocation !!!
        DifferenceType idx = Pos - Begin();

        Resize(m_Size+1, Elem, memory_ok);
        if(!memory_ok) {
            return Pos;
        }
        for(SizeType i=m_Size-1; i!=idx; --i) {
            m_ArrayPtr[i] = m_ArrayPtr[i-1];
        }
        m_ArrayPtr[idx]=Elem;
        // return a pointer to the current array
        return Position(idx);
    }
    
    /**
     * Inserts an element at a specified position.
     * The remaining elements are moved by one position, the
     * size is increased by one.
     * @param Pos The position.
     * @param Elem The value to insert.
     */
    inline void Insert(SizeType Pos, ConstReference Elem, IFR_Bool& memory_ok)
    {
        Insert(Position(Pos), Elem, memory_ok);
    }

    /**
     * Inserts an element behind a specified position (at <code>Pos + 1</code>.
     * The remaining elements are moved by one position, the
     * size is increased by one.
     * @param Pos The position.
     * @param Elem The value to insert.
     */
    inline Iterator InsertBehind(Iterator& Pos, ConstReference Elem)
    {
        if (Pos != End()) return NULL;
        Iterator posat=Pos+1;
        return Insert(posat, Elem);
    }
            
    /**
     * Removes the first vector element, and decreases the size by one.
     */
    inline void DeleteFront()
    {
        Iterator i=Begin();
        Delete(i);
    }

    /**
     * Removes the last vector element, and decreases the size by one.
     */
    inline void DeleteEnd()
    {
        Iterator i=End();
        --i;
        Delete(i);
    }

    /**
     * Delete one element. The remaining elements are moved by one towards
     * the beginning.
     * @param Pos The position of the element to delete.
     */
    void Delete(const Iterator& Pos)
    {
        Iterator endpos = Pos;
        ++endpos;
        Delete(Pos, endpos);
    }
    
    /**
     * Deletes a range of elements, namely <code>[From, To)</code>.
     * The remaining elements are moved towards the beginning 
     * (by the size of the gap that the removal of the range leaves behind.)
     * @param From Start of the range (inclusive).
     * @param To End of the range (exclusive).
     */
    inline void Delete(const Iterator& From, const Iterator& To)
    {
        Iterator dest = From;
        Iterator src = To;
        while((src!=End())) {
            *dest = *src;
            ++dest;
            ++src;
        }
        Iterator del = End();
        for(IFR_Int i = To-From; i>0 && m_Size; --i) {
            --del;
            IFRUtil_Destroy((ValueType *)del);
            --m_Size;
            
        }
    }

    /**
     * Deletes all elements of the vector, by preserving its capacity.
     */
    inline void Clear()
    {
        IFRUtil_DestroyArray(m_ArrayPtr, m_ArrayPtr + m_Size);
        m_Size=0;
    }

    
    /**
     * Gets an iterator to the first element.
     */
    inline Iterator Begin() { return m_ArrayPtr; }
    
    /**
     * Gets an iterator to the first element.
     */
    inline ConstIterator Begin() const { return m_ArrayPtr; }

    /**
     * Gets an iterator pointing at the position one behind the last element.
     */
    inline Iterator End() { return m_ArrayPtr + m_Size; }

    /**
     * Gets an iterator pointing at the position one behind the last element.
     */
    inline ConstIterator End() const { return m_ArrayPtr + m_Size; }
    
    /**
     * Gets an iterator pointing at the last element.
     */
    inline Iterator Last() { return m_ArrayPtr + m_Size - 1; }

    /**
     * Gets an iterator pointing at the last element.
     */
    inline ConstIterator Last() const { return m_ArrayPtr + m_Size - 1; }
    
private:
    /** Dummy. */
    inline IFRUtil_Vector(const IFRUtil_Vector<T>& c)
    :m_rawallocator(c.m_rawallocator), 
     m_Size(0), 
     m_Capacity(0), 
     m_ArrayPtr(0),
     m_InitVal()
    {
    }
    
    /** Dummy. */
    inline IFRUtil_Vector<T>& operator=(const IFRUtil_Vector<T>&)
    {
        return *this;
    }

};


#endif // IFRUTIL_VECTOR_H

