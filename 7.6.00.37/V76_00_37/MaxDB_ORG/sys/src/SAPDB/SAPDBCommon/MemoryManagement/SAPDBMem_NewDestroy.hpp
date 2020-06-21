/*!
  @file           SAPDBMem_NewDestroy.hpp
  @author         ThomasA
  @ingroup        Memory Management
  @brief          Some 'new' and 'destroy' functions for use with SAPDB's
                  memory management.

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


#ifndef SAPDBMem_NewDestroy_HPP
#define SAPDBMem_NewDestroy_HPP

// includes for default placement new and placement delete

#if defined(_WIN32)
#include <windows.h>
#include <new>
#else
#include <new.h>
#endif

#if defined LINUX
#define SAPDBMEM_NOTHROW throw()
#else
#define SAPDBMEM_NOTHROW
#endif

#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"

#include "SAPDBCommon/SAPDB_AutoPtr.hpp"

/*!
   @brief          Memory allocation used by 'placement new' operator.
   @param          ByteCount [in] The number of bytes that should be allocated.
   @param          RawAllocator [in] The heap from which the memory comes.
   @code           
    
    class MyClass
    {
        MyClass(int x) { ... }
    };
    
    void func2()
    {
        // ...
        char Buffer[100];
        MyClass* var = new(Buffer) MyClass(5);
        //...
    }
    
    void func1()
    {
        // ...
        SAPDBMem_IRawAllocator& RawAllocator = SAPDBMem_DefaultRawAllocator::Instance();
        MyClass* var = new(RawAllocator) MyClass(5);
        // ...
    }
    @endcode        

                The memory returned is always suitable aligned for every type.
                Use 'placement new' with explicit pointer to memory, if you 
                want to construct a single object at this memory position.
                No check is done, if the object fits into memory.
                Use 'placement new' with an allocator, if you want to construct
                a single object on the heap of an allocator.

 */

/*
// Placement new with a memory location is ususally predefined.
// This comment just shows, what it does.
inline
void* operator new(size_t ByteCount, void* Memory)
{
    return Memory;
}
*/

inline
void* operator new(size_t ByteCount, SAPDBMem_IRawAllocator& RawAllocator) SAPDBMEM_NOTHROW
{
    return RawAllocator.Allocate(static_cast<SAPDB_ULong>(ByteCount));
}


/*!
   @brief          Free memory.
   @param          Ptr [in] &lt;Ptr> points to the memory that should be freed.
   @param          RawAllocator [in] The raw allocator where the &lt;Ptr> got the memory from.
 
                   Each function 'operator new' used in a 'placement new' 
                   statement must have its corresponding function 'operator delete'.
                   Although this function cannot be called explicitely, the
                   compiler forces the programmer to implement it.
                   If the constructor within the call to 'placement new' throws 
                   an exception, the compiler calls this function to avoid memory 
                   leaks.
*/

inline
void operator delete(void* Ptr, SAPDBMem_IRawAllocator& RawAllocator)
{
    RawAllocator.Deallocate(Ptr);
}


/*!
   @brief          Calls the destructor for an object and frees the memory, 
                   for objects created dynamically.
   @brief          template arguments<class T> The type of the object to destroy.
   @param          Ptr [in/out] The pointer to the object that should be destroyed. 
                   After destruction, the pointer is set to 0.
   @param          Memory [in] The address of the memory position at which the object was created.
*/

template <class T>
void destroy(T* &Ptr, void* Memory)
{
    if ( 0 != Ptr )
    {
        SAPDBERR_ASSERT_ARGUMENT(reinterpret_cast<void*>(Ptr) == Memory);
        Ptr->~T();
        Ptr = 0;
    }
}

/*!
   @brief          Calls the destructor for an object and frees the memory, 
                   for objects created dynamically.
   @brief          template arguments<class T> The type of the object to destroy.
   @param          Ptr [in/out] The pointer to the object that should be destroyed. 
                   After destruction, the pointer is set to 0.
   @param          RawAlloc [in] The raw allocator where the memory for the object came from.
*/

template <class T>
void destroy(T* &Ptr, SAPDBMem_IRawAllocator& RawAlloc)
{
    if ( 0 != Ptr )
    {
        Ptr->~T();
        RawAlloc.Deallocate(Ptr);
        Ptr = 0;
    }
}

/*!
   @brief          Creates an array dynamically.

                   Use 'newarray' with explicit pointer to memory, if you 
                   want to construct an array of objects at this memory position.
                   No check is done, if the array fits into memory or if the 
                   memory is properly aligned.
                   Use 'newarray' with an allocator, if you want to construct
                   an array of objects on the heap of an allocator.
   @brief          template arguments<class T> The type of the objects to create.
   @param          ReturnAddress [out] The pointer in which the new memory position is returned.
                   If the construction of the array fails, zero is returned.
   @param          Count [in] The number of objects of type &lt;T> for which memory should be allocated.
   @param          Memory [in] The memory position at which the object should be constructed.
   @param          RawAllocator [in] The heap from which the array should be allocated.
   @param          Init [in] An initialization value for all vector elements.
   @param          InitArray [in] An initialization vector for the newly constructed vector.
   @returns        T* The return address.
 */


/*!
@brief utility class
*/
template <class T>
class auto_array
{
private:
    T* m_Start; //!< array start address
    T* &m_End;  //!< array end address
public:
    auto_array(T* start, T* &end) : m_Start(start), m_End(end) {} //!< constructor
    ~auto_array() //!< destructor
    {
        if (m_Start != 0) {
            while (m_Start != m_End) {
                m_Start->~T();
                ++m_Start;
            };
        }
    }
    void release() { m_Start = 0; } //!< releases the pointer 
};

/*!
@brief initialises an array of type T at a given address with the 
       default constructor
*/ 

#   if defined(_WIN32) || defined(WIN32)
#   pragma warning(disable : 4345)
#   endif

template <class T>
T* newarray(T* &ReturnAddress, size_t Count, void* Memory)
{
    ReturnAddress = 0;
    T* tmp = reinterpret_cast<T*>(Memory);

    T* Address = tmp;
    T* End = Address + Count;
    // Initialize an temporary auto object in case of copy constructor exceptions.
    auto_array<T> a(tmp, Address);
    // Call the copy constructor <Count> times
    while (Address != End) {
        new(Address) T();
        ++Address;
    };
    // everything worked o.k., a is no longer needed
    a.release();
    return (ReturnAddress = tmp);
}
#   if defined(_WIN32) || defined(WIN32)
#   pragma warning(default : 4345)
#   endif

/*!
@brief initialises an array of type T at a given address with a
       given initial value
*/ 
template <class T>
T* newarray(T* &ReturnAddress, size_t Count, void* Memory, const T& Init)
{
    ReturnAddress = 0;
    T* tmp = reinterpret_cast<T*>(Memory);

    T* Address = tmp;
    T* End = Address + Count;
    // Initialize an temporary auto object in case of copy constructor exceptions.
    auto_array<T> a(tmp, Address);
    // Call the copy constructor <Count> times
    while (Address != End) {
        new(Address) T(Init);
        ++Address;
    };
    // everything worked o.k., a is no longer needed
    a.release();
    return (ReturnAddress = tmp);
}

/*!
@brief initialises an array of type T at a given address with a
       given initial value
*/ 
template <class T>
T* newarray(T* &ReturnAddress, size_t Count, void* Memory, T& Init)
{
    ReturnAddress = 0;
    T* tmp = reinterpret_cast<T*>(Memory);

    T* Address = tmp;
    T* End = Address + Count;
    // Initialize an temporary auto object in case of copy constructor exceptions.
    auto_array<T> a(tmp, Address);
    // Call the copy constructor <Count> times
    while (Address != End) {
        new(Address) T(Init);
        ++Address;
    };
    // everything worked o.k., a is no longer needed
    a.release();
    return (ReturnAddress = tmp);
}

/*!
@brief initialises an array of type T at a given address with a
       given initial array
*/ 

template <class T>
T* newarray(T* &ReturnAddress, size_t Count, void* Memory, const T* InitArray)
{
    ReturnAddress = 0;
    T* tmp = reinterpret_cast<T*>(Memory);
    
    T* Address = tmp;
    T* End = Address + Count;
    // Initialize an temporary auto object in case of copy constructor exceptions
    auto_array<T> a(tmp, Address);
    // Call the copy constructor for each vector element
    while (Address != End) {
        new(Address) T(*InitArray);
        ++InitArray;
        ++Address;
    };
    // everything worked o.k., a is no longer needed
    a.release();
    return (ReturnAddress = tmp);
}

/*!
@brief initialises an array of type T at a given address with a
       given initial array
*/ 

template <class T>
T* newarray(T* &ReturnAddress, size_t Count, void* Memory, T* InitArray)
{
    ReturnAddress = 0;
    T* tmp = reinterpret_cast<T*>(Memory);
    
    T* Address = tmp;
    T* End = Address + Count;
    // Initialize an temporary auto object in case of copy constructor exceptions
    auto_array<T> a(tmp, Address);
    // Call the copy constructor for each vector element
    while (Address != End) {
        new(Address) T(*InitArray);
        ++InitArray;
        ++Address;
    };
    // everything worked o.k., a is no longer needed
    a.release();
    return (ReturnAddress = tmp);
}

/*!
@brief initialises an array of type T at memory provided by an allocator. 
*/ 

template <class T>
T* newarray(T* &ReturnAddress, size_t Count, SAPDBMem_IRawAllocator& RawAllocator)
{
    // Get memory from raw allocator
    void* Memory = RawAllocator.Allocate(Count*sizeof(T));
#ifdef SAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING
    if (Memory == 0) {
        ReturnAddress = 0;
        return 0;
    }
#endif
    // Construct an auto pointer for exception handling
    SAPDB_AutoPtr<T> a(RawAllocator, (T*)Memory);
    // Construct the array elements
    newarray(ReturnAddress, Count, Memory);
    // Everything worked o.k, a is no longer needed
    a.Release();
    return ReturnAddress;
}

/*!
@brief initialises an array of type T at memory provided by an allocator with an
       initial value
*/ 
template <class T>
T* newarray(T* &ReturnAddress, size_t Count, SAPDBMem_IRawAllocator& RawAllocator, const T& Init)
{
    // Get memory from raw allocator
    void* Memory = RawAllocator.Allocate(Count*sizeof(T));
#ifdef SAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING
    if (Memory == 0) {
        ReturnAddress = 0;
        return 0;
    }
#endif
    // Construct an auto pointer for exception handling
    SAPDB_AutoPtr<T> a(RawAllocator, (T*)Memory);
    // Construct the array elements
    newarray(ReturnAddress, Count, Memory, Init);
    // Everything worked o.k, a is no longer needed
    a.Release();
    return ReturnAddress;
}

/*!
@brief initialises an array of type T at memory provided by an allocator with an
       initial array
*/ 
template <class T>
T* newarray(T* &ReturnAddress, size_t Count, SAPDBMem_IRawAllocator& RawAllocator, const T* InitArray)
{
    // Get memory from raw allocator
    void* Memory = RawAllocator.Allocate(Count*sizeof(T));
#ifdef SAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING
    if (Memory == 0) {
        ReturnAddress = 0;
        return 0;
    }
#endif
    // Construct an auto pointer for exception handling
    SAPDB_AutoPtr<T> a(RawAllocator, (T*)Memory);
    // Construct the array elements
    newarray(ReturnAddress, Count, Memory, InitArray);
    // Everything worked o.k, a is no longer needed
    a.Release();
    return ReturnAddress;
}


/*!
   @brief          Calls the destructor for each object of an array and frees 
                   the memory for arrays created dynamically.
   @brief          template arguments<class T> is the type of the objects within the array.
   @param          Array [in/out] The pointer to the array that should be destroyed. After
                   destruction the pointer is set to 0.
   @param          Count [in] The number of objects of type &lt;T> within the array.
   @param          Memory [in] The memory position at which the array was constructed.
 */

template <class T>
void destroyarray(T* &Array, size_t Count, void* Memory)
{
    if ( 0 != Array )
    {
        SAPDBERR_ASSERT_ARGUMENT(reinterpret_cast<void*>(Array) == Memory);
        T* Ptr = Array;
        T* End = Array + Count;
        // Call the destructor <Count> times
        while (Ptr != End) {
            Ptr->~T();
            ++Ptr;
        };
        Array = 0;
    }
}

/*!
   @brief          Calls the destructor for each object of an array and frees 
                   the memory for arrays created dynamically.
   @brief          template arguments<class T> is the type of the objects within the array.
   @param          Array [in/out] The pointer to the array that should be destroyed. After
                   destruction the pointer is set to 0.
   @param          Count [in] The number of objects of type &lt;T> within the array.
   @param          RawAllocator [in] The heap where the memory for the array came from

 */

template <class T>
void destroyarray(T* &Array, size_t Count, SAPDBMem_IRawAllocator& RawAllocator)
{
    // Destruct the array elements
    T* tmp = Array;
    destroyarray(tmp, Count, Array);
    // Free the memory
    RawAllocator.Deallocate(Array);
    Array = 0;
}


#endif // SAPDBMem_NewDestroy_HPP

