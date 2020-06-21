/*!
  @file           Container_Vector.hpp
  @author         ContainerMaint
  @brief          Class declaration and definition for a template vector class
  @author         Stefan Baier, Till Luhmann

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


#ifndef CONTAINER_VECTOR_HPP
#define CONTAINER_VECTOR_HPP

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"

#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"


/*!
  @class          Container_Vector
   @brief          This class implements a vector class.
   template arguments<class T> 
                 Type of the objects to be held in vector.
                 The class T must provide the operator==
                     SAPDB_Bool operator==(const T& t) const;
                 T doesn't have to have a default constructor. All constructing 
                 methods (constructors, Resize(), Reserve()) offer the 
                 possibility to pass an default value as an argument.
                 
   @code           
    This example constructs an vector of type 'int' by using
             a 'SAPDBMem_DefaultRawAllocator'.
    
             void f()
             {
                 // ...
    
                 // Construct an vector of type 'int' with ten elements using 
                 // a default raw allocator.
                 Container_Vector<int>  MyVector(SAPDBMem_DefaultRawAllocator::Instance(), 10);
    
                 // ...
             }
    @endcode        


                The vector uses raw allocators as defined in the SAPDBMem component
                for allocating memory for the vector elements. It is not possible 
                to construct a vector without supplying a raw allocator.
                Thus, the default constructor for a vector is not offered.

 */


template <class T>
class Container_Vector
{

public:

    typedef T            ValueType;     ///< The type of the elements
    typedef SAPDB_UInt   SizeType;      ///< The range for the number of vector elements
    typedef SAPDB_UInt   IndexType;     ///< The range for the index of the vector elements
    typedef T*           Iterator;      ///< Iterator over vector elements
    typedef const T*     ConstIterator; ///< Constant Iterator
    typedef SAPDBMem_IRawAllocator Allocator; ///< Allocator type used for vectors

    

private:
    /// no assignment allowed
    Container_Vector& operator= (const Container_Vector&);

    /// The underlying raw allocator.
    Allocator& m_Allocator;
    /// The number of vector elements that can be stored without resizing the vector
    SizeType m_Capacity;

protected:	// used in ..sortableVector
    /// The number of vector elements
    SizeType m_Size;
    /// The pointer to the vector elements
    T* m_ArrayPtr;

public:
    /*!
       @brief          Constructs a vector
       @param          RawAlloc [in] The raw allocator providing the vector with memory.
        A vector can only be constructed, if a raw allocator is given.
        Thus, a default constructor is not offered.
     */
    Container_Vector(Allocator& RawAlloc);

    /*!
       @brief          Initializes a vector with a certain number of elements
       @param          Size [in] The number of vector elements (must be greater or equal zero).
       @returns        (SAPDB_Bool) true if successful
     */
    SAPDB_Bool Initialize
        (SizeType Size);

    /*!
       @brief          Initializes a vector with a certain number of elements
       @param          Init [in] The initial value of all vector elements. By default,
             the default constructor of &lt;T> is used as the init value.
       @returns        (SAPDB_Bool) true if successful
     */
    SAPDB_Bool Initialize
        (SizeType Size,
         const T& Init);

    /*!
     * @brief Copy-constructor.
     *
     * @warning copy-construction is only allowed for a
     * Container_Vector which does not have any memory allocated! In a
     * slowknl any other use will trigger an assertion!
     */
#if  defined(SAPDB_SLOW) || defined(SAPDB_QUICK)
    Container_Vector(const Container_Vector& vector)
        : m_Allocator(vector.m_Allocator),
          m_Capacity(0),
          m_Size(0),
          m_ArrayPtr(0)
    {
        SAPDBERR_ASSERT_STATE( vector.m_ArrayPtr == 0 );
    }
#endif

    /*!
       @brief          Initializes a vector with a certain number of elements
       @param          InitVector [in] The initial values for the vector elements are copied from here.
       @returns        (SAPDB_Bool) true if successful
     */
    SAPDB_Bool Initialize
        (const Container_Vector<T>& InitVector);

	/// Destructs the vector elements. Preserves the capacity.
    void Clear();

	/// Destructs the vector and frees the memory of the vector 
    void Delete();

	/// Destructs the vector and frees the memory of the vector elements.
    ~Container_Vector();

	/// Returns the number of vector elements within the vector
    SizeType GetSize() const
        { return m_Size; }

    /// Returns true, if the number of vector elements is zero
    SAPDB_Bool IsEmpty() const
        { return (m_Size == 0); }

	/// Returns true, if the number of vector elements is equal to the actual capacity
    SAPDB_Bool IsFull() const
        { return (m_Size == m_Capacity); }

    /*!
       @brief          Returns the capacity of the vector.

        The capacity is the number of vector elements that can be 
        given as an argument to 'Resize()' without reallocating memory.
     */
    SizeType GetCapacity() const
        { return m_Capacity; }

	/// Returns the underlying raw allocator
    Allocator& GetAllocator() const
        { return m_Allocator; }

	/// Compares two vectors and returns true, if the content of both vectors is identical.
    SAPDB_Bool operator ==(const Container_Vector<T>& CompVector) const;

	/// Compares two vectors and returns true, if the content of both vectors is not identical.
    SAPDB_Bool operator !=(const Container_Vector<T>& CompVector) const
        { return !(this->operator==(CompVector)); }

    /*!
       @brief          Changes the number of vector elements.
       @param          NewSize [in] The new number of vector elements.
       @returns        (SAPDB_Bool) true if successful

        If the capacity of the vector is not large enough, the 
        memory for the vector elements is reallocated.
        ATTENTION!! After reallocation all iterators may be invalid!
     */
    SAPDB_Bool Resize(SizeType NewSize);

    /*!
       @brief          Changes the number of vector elements.
       @param          NewSize [in] The new number of vector elements.
       @param          Init [in] An initialization value for the new vector elements.
             If &lt;Init> is not given, the default constructor of &lt;T> is
             used as initialization value.
       @returns        (SAPDB_Bool) true if successful


        If the capacity of the vector is not large enough, the 
        memory for the vector elements is reallocated.
        ATTENTION!! After reallocation all iterators may be invalid!
     */
    SAPDB_Bool Resize(SizeType NewSize, const T& Init);

    /*!
       @brief          Changes the capacity of the vector.
       @param          NewCapacity [in] The new number of vector elements.
       @returns        (SAPDB_Bool) true if successful

        If the new capacity is greater than the actual capacity, memory for
        the vector elements is reallocated.
        ATTENTION!! After reallocation all iterators may be invalid!
        The newly reserved space is not initialized.
     */

    SAPDB_Bool Reserve(SizeType NewCapacity);

	/// Returns a reference to the &lt;n>-th vector element.
    T& operator[](IndexType n)
    {
        SAPDBERR_ASSERT_ARGUMENT(n < m_Size);
        return *(m_ArrayPtr + n);
    }

    const T& operator[](IndexType n) const
    {
        SAPDBERR_ASSERT_ARGUMENT(n < m_Size);
        return *(m_ArrayPtr + n);
    }

	/// Returns an iterator to the &lt;n>-th vector element.
    Iterator Position(IndexType n)
    {
        SAPDBERR_ASSERT_ARGUMENT(n < m_Size);
        return m_ArrayPtr + n;
    }

    ConstIterator Position(IndexType n) const
    {
        SAPDBERR_ASSERT_ARGUMENT(n < m_Size);
        return m_ArrayPtr + n;
    }

    /*!
       @brief          Inserts an element &lt;Elem> at the beginning of the vector.
       @param          Elem [in] The element to insert.
                    The size of the vector grows by one.
                    All vector elements change their index by +1.
                    Thus, all iterators are invalid after a call to 'InsertFront'.
                    This method is rather slow, because all vector elements are
                    moved by one position.
       @returns        true if the possibly necessary reallocation was successfull.
     */
    SAPDB_Bool InsertFront(const T& Elem);

    /*!
       @brief          Inserts an element &lt;Elem> at the end of the vector.
       @param          Elem [in] The element to insert.
       @returns        true if the possibly necessary reallocation was successfull.

        The size of the vector grows by one.
        There must be enough capacity to do this.
        All iterators remain valid after a call to 'InsertEnd'.
     */
    SAPDB_Bool InsertEnd(const T& Elem);

    /*!
       @brief          Inserts a new element at the iterator position &lt;Pos>.
       @param          PosIndex [in] IndexPosition
       @param          Elem [in] The element to insert.

                    The size of the vector grows by one.
                    There must be enough capacity to do this.
                    All vector elements after &lt;PosIndex> change their index by +1.
                    Some iterators are invalid after a call to 'Insert'.
                    This method is rather slow, because all vector elements after
                    &lt;Pos> are moved by one position.
       @returns        true if the possibly necessary reallocation was successfull.
   */
	SAPDB_Bool Insert(const IndexType PosIndex, const T& Elem);

    /*!
       @brief          Inserts a new element at the iterator position &lt;Pos>.
       @param          Pos [in] Iterator position
       @param          Elem [in] The element to insert.

                    The size of the vector grows by one.
                    There must be enough capacity to do this.
                    All vector elements after &lt;Pos> change their index by +1.
                    Iterators may be invalid after a call to 'Insert' ig memory must be reallocated.
                    This method is rather slow, because all vector elements after
                    &lt;Pos> are moved by one position.
       @returns        true if the possibly necessary reallocation was successfull.
   */
	SAPDB_Bool Insert(Iterator& Pos, const T& Elem);

    /*!
       @brief          Deletes the first element of the vector.
        The vector must not be empty.
     */
    void DeleteFront();

    /*!
       @brief          Deletes one vector element or a range of vector elements.
       @param          Pos [in] Iterator to one element.
       @return         Iterator The iterator points to the vector element behind the last one deleted.

        The vector must not be empty.
     */
    void Delete(const Iterator& Pos);
	void Delete(const IndexType PosIndex);
	
    /*!
       @brief          Deletes one vector element or a range of vector elements.
       @param          From [in] Iterator to the begin of the range.
       @param          To [in] Iterator to the end of the range.
       @return         Iterator The iterator points to the vector element behind the last one deleted.

        The vector must not be empty.
        If only one iterator is given as an argument, this element is 
        deleted. If a range is given, the range is deleted.
     */
    void Delete(const Iterator& From, const Iterator& To);
	void Delete(const IndexType FromIndex, const IndexType ToIndex);

	/// Returns an iterator to the first vector element.
    Iterator Begin()
    { 
        SAPDBERR_ASSERT_STATE(m_ArrayPtr != 0 || (m_Capacity == 0));
        return m_ArrayPtr; 
    }

    ConstIterator Begin() const
    { 
        SAPDBERR_ASSERT_STATE(m_ArrayPtr != 0 || (m_Capacity == 0));
        return m_ArrayPtr; 
    }

	/// Returns an iterator behind the last vector element.
    Iterator End()
    { 
        SAPDBERR_ASSERT_STATE((m_ArrayPtr != 0) || (m_Capacity == 0));
        return m_ArrayPtr + m_Size; 
    }

    ConstIterator End() const
    { 
        SAPDBERR_ASSERT_STATE((m_ArrayPtr != 0) || (m_Capacity == 0));
        return m_ArrayPtr + m_Size; 
    }


    /*!
       @brief          Returns the index that belongs to an iterator
       @return         IndexType
     */
    IndexType GetIndex(const Iterator& Iter) const
    {
        SAPDBERR_ASSERT_STATE(m_ArrayPtr != 0);
        // current impl. assumes that iter is a pointer
        SAPDBERR_ASSERT_ARGUMENT(Iter >= m_ArrayPtr);
        SAPDBERR_ASSERT_ARGUMENT(Iter <= m_ArrayPtr + m_Size);
        return (IndexType)(Iter - m_ArrayPtr);
    }
    
    IndexType GetIndex( ConstIterator& Iter ) const
    {
        SAPDBERR_ASSERT_STATE(m_ArrayPtr != 0);
        // current impl. assumes that iter is a pointer
        SAPDBERR_ASSERT_ARGUMENT(Iter >= m_ArrayPtr);
        SAPDBERR_ASSERT_ARGUMENT(Iter <= m_ArrayPtr + m_Size);
        return (IndexType)(Iter - m_ArrayPtr);
    }
};



// -----------------------------------------------------------------------------

template <class T>
inline
Container_Vector<T>::Container_Vector(
    Allocator& RawAlloc
)
 : m_Allocator(RawAlloc), m_Size(0), m_Capacity(0), m_ArrayPtr(0)
{}

// -----------------------------------------------------------------------------

template <class T>
SAPDB_Bool Container_Vector<T>::Initialize(SizeType Size)
{
    SAPDBERR_ASSERT_ARGUMENT(Size > 0);

    T* NewArrayPtr = 0; 
    if (Size > 0)
    {
        // allocate and initialize new vector:
        newarray(NewArrayPtr, Size, m_Allocator);
        if (NewArrayPtr == 0) 
            return false;
    }

    // delete old vector:
    Delete();

    // initialize vector:
    m_ArrayPtr = NewArrayPtr;
    m_Capacity = Size;
    m_Size     = Size;
    return true;
}

// -----------------------------------------------------------------------------

template <class T>
SAPDB_Bool Container_Vector<T>::Initialize(
    SizeType Size,
    const T& Init
)
{
    SAPDBERR_ASSERT_ARGUMENT(Size > 0);

    T* NewArrayPtr = 0; 
    if (Size > 0)
    {
        // allocate and initialize new vector:
        newarray(NewArrayPtr, Size, m_Allocator, Init);
        if (NewArrayPtr == 0) 
            return false;
    }

    // delete old vector:
    Delete();

    // initialize vector:
    m_ArrayPtr = NewArrayPtr;
    m_Capacity = Size;
    m_Size     = Size;
    return true;
}

// -----------------------------------------------------------------------------

template <class T>
inline
SAPDB_Bool Container_Vector<T>::Initialize(
    const Container_Vector<T>& InitVector
)
{
    SizeType Capacity = InitVector.GetCapacity();
    SizeType Size     = InitVector.GetSize();

    T* NewArrayPtr = 0; 
    if (Capacity > 0) 
    {
        // Construct the new vector elements from InitVector
        newarray(NewArrayPtr, Capacity, m_Allocator, InitVector.m_ArrayPtr);
        if (NewArrayPtr == 0) 
            return false;
    }

    // delete old vector:
    Delete();

    // Everything worked ok.
    m_ArrayPtr = NewArrayPtr;
    m_Size     = Size;
    m_Capacity = Capacity;
    return true;
}

// -----------------------------------------------------------------------------

template <class T>
inline
void Container_Vector<T>::Clear()
{
    if (m_Size > 0) 
    {
        // Destruct all used array elements.
        T* tmp = m_ArrayPtr;
        destroyarray(tmp, m_Size, m_ArrayPtr);
    }
    m_Size     = 0;
}

// -----------------------------------------------------------------------------

template <class T>
inline
void Container_Vector<T>::Delete()
{
    Clear();
    if (m_ArrayPtr != 0) 
    {
        // Deallocate the memory for the reserved capacity
        m_Allocator.Deallocate(m_ArrayPtr);
    }
    m_ArrayPtr = 0;
    m_Capacity = 0;
}

// -----------------------------------------------------------------------------

template <class T>
inline
Container_Vector<T>::~Container_Vector()
{
    Delete();
}

// -----------------------------------------------------------------------------

template <class T>
SAPDB_Bool Container_Vector<T>::operator ==(const Container_Vector<T>& CompVector) const
{
    SAPDB_Bool Result = false;

    // Compare the sizes of both vectors
    if (m_Size == CompVector.GetSize()) {
        // If the sizes match, the result is true even if the size is zero
        Result = true;
        if (m_Size > 0) {
            // if the size is greater than zero, compare the vectors element by element
            ConstIterator Iter1, Iter2;
            Iter2 = CompVector.Begin();
            for (Iter1 = Begin(); Iter1 != End(); ++Iter1) {
                if (!(*Iter1 == *Iter2)) {
                    // If two elements are not equal, the result is false.
                    // Then break the loop.
                    Result = false;
                    break;
                }
                ++Iter2;
            }
        }
    }
    return Result;
}

// -----------------------------------------------------------------------------

template <class T>
SAPDB_Bool Container_Vector<T>::Resize(SizeType NewSize)
{
    T* tmp;

    if (NewSize < m_Size) {
        // Destroy the unused part of the vector.
        tmp = m_ArrayPtr + NewSize;
        destroyarray(tmp, m_Size-NewSize, tmp);
    }
    else {
        if (NewSize > m_Capacity) {
            if (!Reserve(NewSize))
                return false;
        }
        // Initialize the new part of the vector.
        if ( NewSize > m_Size ) // PTS 1117895 UH 2002-10-18
            newarray(tmp, NewSize-m_Size, m_ArrayPtr+m_Size);
    }
    m_Size = NewSize;
    return true;
}

// -----------------------------------------------------------------------------

template <class T>
SAPDB_Bool Container_Vector<T>::Resize(SizeType NewSize, const T& Init)
{
    T* tmp;

    if (NewSize < m_Size) {
        // Destroy the unused part of the vector.
        tmp = m_ArrayPtr + NewSize;
        destroyarray(tmp, m_Size-NewSize, tmp);
    }
    else {
        if (NewSize > m_Capacity) {
            if (!Reserve(NewSize))
                return false;
        }
        // Initialize the new part of the vector.
        if ( NewSize > m_Size ) // PTS 1117895 UH 2002-10-18
            newarray(tmp, NewSize-m_Size, m_ArrayPtr+m_Size, Init);
    }
    m_Size = NewSize;
    return true;
}

// -----------------------------------------------------------------------------

template <class T>
SAPDB_Bool Container_Vector<T>::Reserve(SizeType NewCapacity)
{
    // If the reserved capacity is sufficient, do nothing.
    // The capacity never becomes smaller.
    if (NewCapacity <= m_Capacity) {
        return true;
    }
    // Give m_ArrayPtr as a hint to try to reallocate memory at the hint position.
    T* NewArrayPtr = reinterpret_cast<T*>(m_Allocator.Allocate(NewCapacity * sizeof(T), m_ArrayPtr));

    if (NewArrayPtr == 0) 
        { return false; }

    if (NewArrayPtr != m_ArrayPtr) 
    {
        if( m_ArrayPtr != 0 ) // && m_Size != 0)  // PTS 1139232 TS 2005-12-08 
        {
            if( 0 != m_Size ) // PTS 1139232 TS 2005-12-08 
            {
                // If the reallocation at the hint position didn't work...
                // ...initialize the new vector elements with the old ones,
                newarray(NewArrayPtr, m_Size, NewArrayPtr, m_ArrayPtr);
            }
            // ...and destroy the old vector, i.e. return memory to the give allocator
            destroyarray(m_ArrayPtr, m_Size, m_Allocator); // PTS 1139232 TS 2005-12-08 
        }
        // Assign the new member.
        m_ArrayPtr = NewArrayPtr;
    }
    m_Capacity = NewCapacity;

    return true;
}

// -----------------------------------------------------------------------------

template <class T>
inline
SAPDB_Bool Container_Vector<T>::InsertFront(const T& Elem)
{
    //Iterator Iter = Begin();
    //return Insert(Iter, Elem);
	return Insert(0, Elem);
}

// -----------------------------------------------------------------------------

template <class T>
inline
SAPDB_Bool Container_Vector<T>::InsertEnd(const T& Elem)
{
    return Resize(m_Size+1, Elem);
}

// -----------------------------------------------------------------------------

template <class T>
SAPDB_Bool Container_Vector<T>::Insert(const IndexType PosIndex, const T& Elem)
{
    // Let the size grow by one
    if ( Resize(m_Size+1, Elem) )
    {
        // Shift all vector elements from <Pos> on one position to the end.
        for (IndexType i=m_Size-1; i>PosIndex; --i) 
		{
            operator[](i) = operator[](i-1);
        }
        // Insert the new element
        operator[](PosIndex) = Elem;
        // Set the new iterator position.
        return true;
    }
    return false;
}

template <class T>
SAPDB_Bool Container_Vector<T>::Insert(Iterator& Pos, const T& Elem)
{
    // Calculate the index of iterator <Pos>
    IndexType PosIndex = GetIndex(Pos);
	return Insert(PosIndex, Elem);
}

// -----------------------------------------------------------------------------

template <class T>
inline
void Container_Vector<T>::DeleteFront()
{
    Delete(Begin());
}

// -----------------------------------------------------------------------------

template <class T>
void Container_Vector<T>::Delete(const IndexType PosIndex)
{
    SAPDBERR_ASSERT_ARGUMENT(!IsEmpty());
    SAPDBERR_ASSERT_ARGUMENT( PosIndex >=0 && PosIndex<m_Size );

	// Shift all vector elements behind <Pos> one position.
    for (IndexType i=PosIndex+1; i<m_Size; ++i) {
        operator[](i-1) = operator[](i);
    }
    // Let the size shrink by one.
    // *Begin() is necessary to work with classes that don't provide a default constructor
    Resize(m_Size-1, *Begin());
}

template <class T>
void Container_Vector<T>::Delete(const Iterator& Pos)
{
    // Calculate the index of iterator <Pos>
    IndexType PosIndex = GetIndex(Pos);
	
	Delete(PosIndex);
}

// -----------------------------------------------------------------------------

template <class T>
void Container_Vector<T>::Delete(const IndexType FromIndex, const IndexType ToIndex)
{
    SAPDBERR_ASSERT_ARGUMENT(!IsEmpty());
    SAPDBERR_ASSERT_ARGUMENT(FromIndex <= ToIndex);
    SAPDBERR_ASSERT_ARGUMENT(FromIndex >= 0);
    SAPDBERR_ASSERT_ARGUMENT(ToIndex < m_Size);

	// Shift all vector elements behind <To> to <From>
    for (IndexType i=ToIndex+1; i<m_Size; ++i) {
        operator[](FromIndex+i-(ToIndex+1)) = operator[](i);
    }
    // Let the size shrink.
    // *Begin() is necessary to work with classes that don't provide a default constructor
    Resize(m_Size-(ToIndex-FromIndex+1), *Begin());
}


template <class T>
void Container_Vector<T>::Delete(const Iterator& From, const Iterator& To)
{
    // Calculate the index of iterator positions.
    IndexType FromIndex = GetIndex(From);
    IndexType ToIndex = GetIndex(To);

	Delete(FromIndex, ToIndex);
}

// -----------------------------------------------------------------------------



#endif // CONTAINER_VECTOR_HPP
