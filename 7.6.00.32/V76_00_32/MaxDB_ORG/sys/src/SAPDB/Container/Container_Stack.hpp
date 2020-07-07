/*!
  @file   Container_Stack.hpp
  @author UweH
  @author ContainerMaint
  @brief  Class declaration and definition for a template stack class

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
#ifndef CONTAINER_STACK_HPP
#define CONTAINER_STACK_HPP

#include "Container/Container_Vector.hpp"
/*!
   @class  Container_Stack
   @brief  This class implements a stack class.

    Type of the objects to be pushed onot the stack.
    The class T must provide the operator==.
    bool operator==(const T&amp; t) const;
    T doesn't have to have a default constructor. All constructing methods
    (constructors, Resize(), Reserve()) offer the possibility to pass an default value
    as an argument.

    This implementation uses a vector as the underlying container class.
    Elements can be pushed onto the stack and popped from the stack. 
*/
template <class T>
class Container_Stack : private Container_Vector<T>
{
private:
    /// typedef for abbreviation
    typedef Container_Vector<T> VectorType;
public:
    /// The type of the elements
    typedef typename VectorType::ValueType ValueType;
    /// The range for the number of stack elements
    typedef typename VectorType::SizeType   SizeType;
    /// The range for the index of the stack elements
    typedef typename VectorType::IndexType IndexType;
private:
    /// The index of the next element that will be pushed onto the stack.
    IndexType m_NextIndex;
public:
    /*!
        @brief Construct a stack.
        @param RawAlloc [in] The raw allocator which provides the stack with memory.

        A stack can only be constructed, if an raw allocator is given,
        thus a default constructor is not offered.
     */
    explicit Container_Stack(SAPDBMem_IRawAllocator& RawAlloc);
    /// Destructs the stack and frees the memory of the stack elements.
    ~Container_Stack() 
    {
        Delete();
	}
    /*!
        @brief  Initializes a stack with a certain capacity.
        @param  Capacity [in] The number of stack elements (must be greater or equal zero).
        @return (SAPDB_Bool) true if successful
     */
    SAPDB_Bool Initialize (SizeType Capacity);
    /*!
        @brief  Initializes a stack with a certain capacity.
        @param  InitStack [in] The initial values for the stack elements are copied from here.
        @return (SAPDB_Bool) true if successful
     */
    SAPDB_Bool Initialize (const Container_Stack<T>& InitStack);
    /// Destructs the stack and frees the memory of the stack elements.
    void Delete()
    {
        VectorType::Delete();
	}
    /// Returns the number of elements on the stack.
    SizeType GetSize() const
    {
        return VectorType::GetSize();
	}
    /// Returns true, if there are no elements on the stack.
    bool IsEmpty() const
    {
        return (GetSize() == 0);
	}
    /*!
        @brief Returns the capacity of the stack.

        The capacity is the number of stack elements that can be 
        pushed onto the stack without reallocating memory.
     */
    SizeType GetCapacity() const
    {
        return VectorType::GetCapacity();
	}
    /// Returns the underlying raw allocator
    SAPDBMem_IRawAllocator& GetRawAllocator() const
    {
        return VectorType::GetAllocator();
	}
    /// Compares two stacks and returns true, if the content of both stacks is identical.
    bool operator ==(const Container_Stack<T>& CompStack) const
    {
        return VectorType::operator ==(CompStack);
	}
    /// Compares two stacks and returns true, if the content of both stacks is not identical.
    bool operator !=(const Container_Stack<T>& CompVector) const
    {
        return VectorType::operator !=(CompVector);
	}
    /// Returns a reference to the uppermost element on the stack.
    T& Top();
    /*!
        @brief  Changes the capacity of the stack.
        @param  NewCapacity [in] The new number of vector elements.
        @return (SAPDB_Bool) true if successful

        If the new capacity is greater than the actual size, the 
        memory for the stack is reallocated.
        ATTENTION!! After reallocation all iterators may be invalid!
     */
    SAPDB_Bool Reserve(SizeType NewCapacity)
    {
        return VectorType::Reserve(NewCapacity);
	}
    /*!
        @brief          Pushes an element &lt;Elem> onto the stack.
        @param          Elem [in] 
        @return         (SAPDB_Bool) true if successful

        If the capacity of the stack is not sufficient to hold
        another element, the capacity is enlarged as described
        in method 'Reserve()'.
     */
    SAPDB_Bool Push(const T& Elem);
    /*!
        @brief  Inserts an element at the bottom of the stack.
        @return true, if successful
    */
    SAPDB_Bool InsertBottom(const T& Elem);
    /*!
        @brief  Pops an element &lt;Elem> from the stack.
        @return (SAPDB_Bool) true if successful
     */
    SAPDB_Bool Pop();
    /*!
        @brief  Empties the stack without changing the capacity.
        @return (SAPDB_Bool) true if successful
     */
    SAPDB_Bool Empty();
    /*!
        @brief Returns a reference to the &lt;n>-th stack element.
     */
    inline const T& operator[](IndexType n) const;
};
//-------------------------------------------------------------------------------------------
template <class T> inline Container_Stack<T>::Container_Stack( SAPDBMem_IRawAllocator& RawAlloc)
 : VectorType(RawAlloc),
   m_NextIndex(0)
{}
//-------------------------------------------------------------------------------------------
template <class T> inline SAPDB_Bool Container_Stack<T>::Initialize(SizeType Capacity)
{
    return Reserve(Capacity);
}
//-------------------------------------------------------------------------------------------
template <class T> inline SAPDB_Bool Container_Stack<T>::Initialize(const Container_Stack<T>& InitStack)
{
    return VectorType::Initialize(InitStack);
}
//-------------------------------------------------------------------------------------------
template <class T> inline T& Container_Stack<T>::Top()
{
    SAPDBERR_ASSERT_STATE(!IsEmpty());
    // Return the uppermost element.
    return Container_Vector<T>::operator[](m_NextIndex-1);
}
//-------------------------------------------------------------------------------------------
template <class T> SAPDB_Bool Container_Stack<T>::Push(const T& Elem)
{
    if (!(GetSize() < GetCapacity())) {
        if (GetCapacity() < 4) {
            // Reserve space for at least four stack elements.
            if (!Reserve(4)) return false;
        }
        else {
            // Enlarge the capacity by 50%.
            // This is done to avoid reallocating memory to often.
            if (!Reserve(GetCapacity()+GetCapacity()/2)) return false;
        }
    }
    // Increment the size of the vector by one.
    if (!Resize(m_NextIndex+1)) return false;
    // Insert the new element.
    Container_Vector<T>::operator[](m_NextIndex++) = Elem;
    return true;
}
//-------------------------------------------------------------------------------------------
template <class T> SAPDB_Bool Container_Stack<T>::InsertBottom(const T& Elem)
{
    if (! Reserve( GetSize() + 1) ) return false;
	if ( Insert(0, Elem) ) {
        m_NextIndex++;
        return true;
    }
    return false;
}
//-------------------------------------------------------------------------------------------
template <class T> inline SAPDB_Bool Container_Stack<T>::Pop()
{
    SAPDBERR_ASSERT_STATE(!IsEmpty());
    // Decrement the size of the vector by one.
    --m_NextIndex;
    return Resize(m_NextIndex);
}
//-------------------------------------------------------------------------------------------
template <class T> inline SAPDB_Bool Container_Stack<T>::Empty()
{
    m_NextIndex = 0;
    return this->Resize(0);
}
//-------------------------------------------------------------------------------------------
template <class T>
inline const T& Container_Stack<T>::operator[](IndexType n) const
{
    SAPDBERR_ASSERT_STATE(!IsEmpty());
    SAPDBERR_ASSERT_STATE(n < m_NextIndex);
    return Container_Vector<T>::operator[](n);
}
#endif // CONTAINER_STACK_HPP
