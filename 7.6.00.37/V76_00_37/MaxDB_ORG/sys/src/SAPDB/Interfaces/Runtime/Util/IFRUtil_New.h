/*!
  @file           IFRUtil_New.h
  @author         D030044
  @ingroup        IFR_Mem
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
#ifndef IFRUTIL_NEW_H
#define IFRUTIL_NEW_H

#define IFR_MEMORY_DEBUG 0

#if IFR_MEMORY_DEBUG
//======================================================================
// MEMORY DEBUGGING
//======================================================================
#include <new>
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "Interfaces/Runtime/Util/IFRUtil_CopyTraits.h"
#include "Interfaces/Runtime/Util/IFRUtil_StackAllocator.h"

#include <stdlib.h>
#include <stdio.h>
#ifndef WIN32
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#endif

IFR_BEGIN_NAMESPACE

extern "C" void IFR_AllocateCallback(size_t size,
                                     void *pointer,
                                     const char *file,
                                     const int line);

extern "C" void IFR_DeallocateCallback(void *pointer,
                                       const char *file,
                                       const int line);

struct IFRUtil_AllocatorWrapper
{
    inline IFRUtil_AllocatorWrapper(SAPDBMem_IRawAllocator& a,
                                    const char *f,
                                    const int l)
    :allocator(a),
     file(f),
     line(l)
    {}

    inline void *Allocate(size_t sz)
    {
        void *result = allocator.Allocate(sz);
        IFR_AllocateCallback(sz, result, file, line);
        return result;
    }

    inline void Deallocate(void *ptr)
    {
        IFR_DeallocateCallback(ptr, file, line);
        if(ptr) {
            allocator.Deallocate(ptr);
        }
    }

    SAPDBMem_IRawAllocator& allocator;
    const char *            file;
    const int               line;
};

inline IFRUtil_AllocatorWrapper IFRUtil_MakeAllocatorWrapper(SAPDBMem_IRawAllocator& allocator,
                                                             const char *file,
                                                             const int line)
{
    return IFRUtil_AllocatorWrapper(allocator, file, line);
}

inline
void* operator new(size_t sz, IFRUtil_AllocatorWrapper& wrapper) throw()
{
    return wrapper.Allocate(sz);
}


#if (!defined(WIN32)) || (defined(_MSC_VER) && !defined(BIT64) && _MSC_VER >= 1300)
inline 
void *operator new [] (size_t bytecount, IFRUtil_AllocatorWrapper& wrapper) throw()
{
    return wrapper.Allocate(bytecount);
}

inline
void operator delete[] (void *ptr, IFRUtil_AllocatorWrapper& wrapper) throw()
{
    wrapper.Deallocate(ptr);
}
#endif

#define IFR_MEM_FAIL memory_ok=false
#define IFR_MEM_OK memory_ok
#define IFR_MEM_NOT_OK (!memory_ok)

#define IFR_ALLOCATOR(allocator) (IFRUtil_MakeAllocatorWrapper(allocator, __FILE__, __LINE__))

template <class ValueType>
inline void IFRUtil_NewArray(ValueType*& returnaddr, 
                             IFR_size_t size, 
                             IFRUtil_AllocatorWrapper& allocator,
                             IFR_Bool& memory_ok)
{
    //<<< MEMCHECK
    if(!memory_ok) {
      returnaddr = 0;
      return;
    }
    //>>> MEMCHECK
    returnaddr = (ValueType *) allocator.Allocate(sizeof(ValueType) * size);
    if(returnaddr == 0) {
        memory_ok = false;
        return;
    } else {
        ValueType *end=returnaddr + size;
        ValueType *p=returnaddr;
        while(p != end) {
            new (p) ValueType();
            ++p;
        }
        return;
    }
}

template <class ValueType, class InitType>
inline void IFRUtil_NewArray(ValueType*& returnaddr, 
                             IFR_size_t size, 
                             const InitType& init,
                             IFRUtil_AllocatorWrapper& allocator,
                             IFR_Bool& memory_ok)
{
#if defined(HPUX)
    typename IFRUtil_CopyTraits<ValueType>::IsCopyConstructible i;
#else
    typename IFRUtil_CopyTraits<ValueType>::IsCopyConstructible i;
#endif
    i = i;
    IFRUtil_NewArray_1(returnaddr, size, init, allocator, memory_ok, i);
}

template <class ValueType, class InitType>
inline void IFRUtil_NewArray_1(ValueType*& returnaddr, 
                               IFR_size_t size, 
                               const InitType& init,
                               IFRUtil_AllocatorWrapper& allocator,
                               IFR_Bool& memory_ok,
                               IFRUtil_FalseType f)
{
    //<<< MEMCHECK
    if(!memory_ok) {
      returnaddr = 0;
      return;
    }
    //>>> MEMCHECK
    returnaddr = (ValueType *) allocator.Allocate(sizeof(ValueType) * size);
    if(returnaddr == 0) {
        memory_ok = false;
        return;
    } else {
        ValueType *end=returnaddr + size;
        ValueType *p=returnaddr;
        while(p != end) {
            new (p) ValueType(IFRUtil_CopyTraits<ValueType>::init_helper(&(allocator.allocator), init));
            IFRUtil_CopyTraits<ValueType>::copy(*p, init, memory_ok);
            //<<< MEMCHECK
            if(!memory_ok) {
                ValueType *d=returnaddr;
                while(d <= p) {
                    d->~ValueType();
                    ++d;
                }
                allocator.Deallocate(returnaddr);
                returnaddr = 0;
                return;
            }
            //>>> MEMCHECK
            ++p;
        }
        return;
    }
}

template <class ValueType, class InitType>
inline void IFRUtil_NewArray_1(ValueType*& returnaddr, 
                               IFR_size_t size, 
                               const InitType& init,
                               IFRUtil_AllocatorWrapper& allocator,
                               IFR_Bool& memory_ok,
                               IFRUtil_TrueType f)
{
    //<<< MEMCHECK
    if(!memory_ok) {
        returnaddr = 0;
        return;
    }
    //>>> MEMCHECK
    returnaddr = (ValueType *) allocator.Allocate(sizeof(ValueType) * size);
    if(returnaddr == 0) {
        memory_ok = false;
        return;
    } else {
        ValueType *end=returnaddr + size;
        ValueType *p=returnaddr;
        while(p != end) {
            new (p) ValueType(init);
            ++p;
        }
        return;
    }
}

template <class ValueType, class InputIterator>
inline void IFRUtil_NewArray(ValueType*& returnaddr, 
                             InputIterator begin,
                             InputIterator end,
                             IFRUtil_AllocatorWrapper& allocator,
                             IFR_Bool& memory_ok)
{
#if defined(HPUX)
    typename IFRUtil_CopyTraits<ValueType>::IsCopyConstructible i;
#else
    typename IFRUtil_CopyTraits<ValueType>::IsCopyConstructible i;
#endif
    IFRUtil_NewArray_1(returnaddr, begin, end, allocator, memory_ok, i);
}



template <class ValueType, class InputIterator>
inline void IFRUtil_NewArray_1(ValueType*& returnaddr, 
                             InputIterator begin,
                             InputIterator end,
                             IFRUtil_AllocatorWrapper& allocator,
                               IFR_Bool& memory_ok,
                               IFRUtil_TrueType f)
{
    //<<< MEMCHECK
    if(!memory_ok) {
      returnaddr = 0;
      return;
    }
    //>>> MEMCHECK
 
    IFR_size_t count = end - begin;
    if(count == 0) {
        returnaddr = 0;
        return;
    } else {
        returnaddr = (ValueType *) allocator.Allocate(sizeof(ValueType) * count);
        if(returnaddr == 0) {
            memory_ok = false;
            return;
        } else {
            ValueType *p=returnaddr;
            while(begin != end) {
                new (p) ValueType(*begin);
                ++begin;
                ++p;
            }
        }
    }
}

template <class ValueType, class InputIterator>
inline void IFRUtil_NewArray_1(ValueType*& returnaddr, 
                             InputIterator begin,
                             InputIterator end,
                             IFRUtil_AllocatorWrapper& allocator,
                             IFR_Bool& memory_ok,
                             IFRUtil_FalseType f)
{
    //<<< MEMCHECK
    if(!memory_ok) {
      returnaddr = 0;
      return;
    }
    //>>> MEMCHECK
 
    IFR_size_t count = end - begin;
    if(count == 0) {
        returnaddr = 0;
        return;
    } else {
        returnaddr = (ValueType *) allocator.Allocate(sizeof(ValueType) * count);
        if(returnaddr == 0) {
            memory_ok = false;
            return;
        } else {
            ValueType *p=returnaddr;
            while(begin != end) {
                new (p) ValueType(IFRUtil_CopyTraits<ValueType>::init_helper(&(allocator.allocator), *begin));
                IFRUtil_CopyTraits<ValueType>::copy(*p, *begin, memory_ok);
                ++begin;
                ++p;
            }
        }
    } 
}

template <class ValueType>
inline void IFRUtil_DeleteArray(ValueType *&address,
                                IFR_size_t size,
                                IFRUtil_AllocatorWrapper& allocator)
{
    if(address == 0) {
        return;
    } else {
        for(IFR_size_t i=0; i<size; ++i) {
            (address + i)->~ValueType();
        }
        allocator.Deallocate(address);
        address = 0;
    }
}

template <class ValueType>
inline void IFRUtil_DestroyArray(ValueType *begin,
                                 ValueType *end)
{
    while(begin != end) {
        begin->~ValueType();
        ++begin;
    }
    return;
}

template <class ValueType, class InitType>
inline void IFRUtil_Construct(ValueType *p, const InitType& v)
{
    new (p) ValueType(v);
    return;
}

template <class ValueType>
inline void IFRUtil_ConstructArray(ValueType *begin, ValueType *end)
{
    while(begin != end) {
        new (begin) ValueType();
        ++begin;
    }
}

template <class ValueType, class InitType>
inline void IFRUtil_ConstructArray(ValueType *begin, 
                                     ValueType *end, 
                                   const InitType& init,
    IFR_Bool& memory_ok)
{
    // We choose the implementation along with the copy traits
#if defined(HPUX)
    typename IFRUtil_CopyTraits<ValueType>::IsDefaultConstructible i;
#else
    typename IFRUtil_CopyTraits<ValueType>::IsDefaultConstructible i;
#endif

    IFRUtil_ConstructArray_1(begin, end, init, memory_ok, i);
}

template <class ValueType, class InitType>
inline void IFRUtil_ConstructArray_1(ValueType *begin, 
                                     ValueType *end, 
                                     const InitType& init,
                                     IFR_Bool& memory_ok,
                                     IFRUtil_TrueType dummy)
{
    //<<< MEMCHECK
    if(!memory_ok) {
        return;
    }
    //>>> MEMCHECK
    while(begin != end) {
        new (begin) ValueType(init);
        ++begin;
    }
}

template <class ValueType, class InitType>
inline void IFRUtil_ConstructArray_1(ValueType *begin, 
                                     ValueType *end, 
                                     const InitType& init,
                                     IFR_Bool& memory_ok,
                                     IFRUtil_FalseType dummy)
{
    //<<< MEMCHECK
    if(!memory_ok) {
        return;
    }
    //>>> MEMCHECK
    ValueType *start=begin;
    while(begin != end) {
        // Call the helper constructor,
        // and the copy helper afterwards.
        new (begin) ValueType(IFRUtil_CopyTraits<ValueType>::init_helper(0, init));
        IFRUtil_CopyTraits<ValueType>::copy(*begin, init, memory_ok);
        //<<< MEMCHECK
        if(!memory_ok) {
            do {
                start->~ValueType();
                ++start;
            } while(start <= begin);
            return;
        }
        //>>> MEMCHECK
        ++begin;
    }
}

template <class Value>
inline void IFRUtil_Destroy(Value *p)
{
    p->~Value();
    return;
}



#define IFRUtil_Delete(p, allocator) IFRUtil_Delete_Wrapper(p, IFR_ALLOCATOR(allocator))
template <class ValueType>
inline void IFRUtil_Delete_Wrapper(ValueType *&p, IFRUtil_AllocatorWrapper& wrapper)
{
    if(p != 0) {
        p->~ValueType();
        wrapper.Deallocate(p);    
        p=0;
    }
    return;
}

#define IFRUTIL_DELETEOBJECT(p, type, allocator)    \
do {                                                \
    if(p == 0) {                                    \
        break;                                      \
    } else {                                        \
        p->~type();                                 \
        IFR_ALLOCATOR(allocator).Deallocate(p);     \
        p = 0;                                      \
    }                                               \
} while(0)

IFR_END_NAMESPACE

//======================================================================
// END MEMORY DEBUGGING
//======================================================================
#else
//======================================================================
// NORMAL CODE (NO MEMORY DEBUGGING)
//======================================================================

#include <new>
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "Interfaces/Runtime/Util/IFRUtil_CopyTraits.h"
#include "Interfaces/Runtime/Util/IFRUtil_StackAllocator.h"

#include <stdlib.h>
#include <stdio.h>
#ifndef WIN32
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#endif

// HP && AIX have some strange ideas in what name spaces a new operator may reside.
#if !defined(HPUX) && !defined(AIX) && !defined(__INTEL_COMPILER) && !( defined __GNUC__ && __GNUC__ >= 4 )
IFR_BEGIN_NAMESPACE
#endif

// Microsoft Visual C++ 6.0 for Win32 is unable to understand this, but works
// seamlessly without these ...

#if (!defined(WIN32)) || (defined(_MSC_VER) && !defined(BIT64) && _MSC_VER >= 1300)
//----------------------------------------------------------------------
inline 
void *operator new [] (size_t bytecount, SAPDBMem_IRawAllocator& allocator) throw()
{
    return allocator.Allocate(bytecount);
}

//----------------------------------------------------------------------
inline
void operator delete[] (void *ptr, SAPDBMem_IRawAllocator& allocator) throw()
{
    if(ptr) {
        allocator.Deallocate(ptr);
    }
}
#endif

#if defined(HPUX) || defined(AIX) || defined(__INTEL_COMPILER) || ( defined __GNUC__ && __GNUC__ >= 4 )
IFR_BEGIN_NAMESPACE
#endif


#define IFR_MEM_FAIL memory_ok=false
#define IFR_MEM_OK memory_ok
#define IFR_MEM_NOT_OK (!memory_ok)

#define IFR_ALLOCATOR(x) (x)


/**
 * @ingroup IFR_Mem
 * Makes a new array. The default constructor is used to initialize the
 * array elements. 
 * @param returnaddr The return address.
 * @param size       The number of array elements.
 * @param allocator  The allocator to be used for getting the memory.
 */
template <class ValueType>
inline void IFRUtil_NewArray(ValueType*& returnaddr, 
                             IFR_size_t size, 
                             SAPDBMem_IRawAllocator& allocator,
                             IFR_Bool& memory_ok)
{
    //<<< MEMCHECK
    if(!memory_ok) {
      returnaddr = 0;
      return;
    }
    //>>> MEMCHECK
    returnaddr = (ValueType *) allocator.Allocate(sizeof(ValueType) * size);
    if(returnaddr == 0) {
        memory_ok = false;
        return;
    } else {
        ValueType *end=returnaddr + size;
        ValueType *p=returnaddr;
        while(p != end) {
            new (p) ValueType();
            ++p;
        }
        return;
    }
}

/**
 * @ingroup IFR_Mem
 * Makes a new array. The elements are initialized to a provided value.
 * @param returnaddr The return address.
 * @param size       The number of array elements.
 * @param init       The value to be used for initialization.
 * @param allocator  The allocator to be used for getting the memory.
 */
template <class ValueType, class InitType>
inline void IFRUtil_NewArray(ValueType*& returnaddr, 
                             IFR_size_t size, 
                             const InitType& init,
                             SAPDBMem_IRawAllocator& allocator,
                             IFR_Bool& memory_ok)
{
    typename IFRUtil_CopyTraits<ValueType>::IsCopyConstructible i;
    i = i;
    IFRUtil_NewArray_1(returnaddr, size, init, allocator, memory_ok, i);
}

/**
 * @ingroup IFR_Mem
 * Makes a new array. The elements are initialized to a provided value.
 * @param returnaddr The return address.
 * @param size       The number of array elements.
 * @param init       The value to be used for initialization.
 * @param allocator  The allocator to be used for getting the memory.
 * @param memory_ok  Flag to indicate out of memory.
 * @param f          Dummy parameter substituted by @c IFR_CopyTraits<ValueType>.
 */
template <class ValueType, class InitType>
inline void IFRUtil_NewArray_1(ValueType*& returnaddr, 
                               IFR_size_t size, 
                               const InitType& init,
                               SAPDBMem_IRawAllocator& allocator,
                               IFR_Bool& memory_ok,
                               IFRUtil_FalseType f)
{
    //<<< MEMCHECK
    if(!memory_ok) {
      returnaddr = 0;
      return;
    }
    //>>> MEMCHECK
    returnaddr = (ValueType *) allocator.Allocate(sizeof(ValueType) * size);
    if(returnaddr == 0) {
        memory_ok = false;
        return;
    } else {
        ValueType *end=returnaddr + size;
        ValueType *p=returnaddr;
        while(p != end) {
            new (p) ValueType(IFRUtil_CopyTraits<ValueType>::init_helper(&allocator, init));
            IFRUtil_CopyTraits<ValueType>::copy(*p, init, memory_ok);
            //<<< MEMCHECK
            if(!memory_ok) {
                ValueType *d=returnaddr;
                while(d <= p) {
                    d->~ValueType();
                    ++d;
                }
                allocator.Deallocate(returnaddr);
                returnaddr = 0;
                return;
            }
            //>>> MEMCHECK
            ++p;
        }
        return;
    }
}

/**
 * @ingroup IFR_Mem
 * Makes a new array. The elements are initialized to a provided value.
 * @param returnaddr The return address.
 * @param size       The number of array elements.
 * @param init       The value to be used for initialization.
 * @param allocator  The allocator to be used for getting the memory.
 * @param memory_ok  Flag to indicate out of memory.
 * @param f          Dummy parameter substituted by @c IFR_CopyTraits<ValueType>.
 */
template <class ValueType, class InitType>
inline void IFRUtil_NewArray_1(ValueType*& returnaddr, 
                               IFR_size_t size, 
                               const InitType& init,
                               SAPDBMem_IRawAllocator& allocator,
                               IFR_Bool& memory_ok,
                               IFRUtil_TrueType f)
{
    //<<< MEMCHECK
    if(!memory_ok) {
        returnaddr = 0;
        return;
    }
    //>>> MEMCHECK
    returnaddr = (ValueType *) allocator.Allocate(sizeof(ValueType) * size);
    if(returnaddr == 0) {
        memory_ok = false;
        return;
    } else {
        ValueType *end=returnaddr + size;
        ValueType *p=returnaddr;
        while(p != end) {
            new (p) ValueType(init);
            ++p;
        }
        return;
    }
}

template <class ValueType, class InputIterator>
inline void IFRUtil_NewArray(ValueType*& returnaddr, 
                             InputIterator begin,
                             InputIterator end,
                             SAPDBMem_IRawAllocator& allocator,
                             IFR_Bool& memory_ok)
{
#if defined(HPUX)
    typename IFRUtil_CopyTraits<ValueType>::IsCopyConstructible i;
#else
    typename IFRUtil_CopyTraits<ValueType>::IsCopyConstructible i;
#endif
    IFRUtil_NewArray_1(returnaddr, begin, end, allocator, memory_ok, i);
}



/**
 * @ingroup IFR_Mem
 * Makes a new array. The elements are initialized to the elements of the provided 
 * value range <code>[begin, end)</code>.
 * @param returnaddr The return address.
 * @param begin      The input iterator for the initialisation values (begin).
 * @param end        The input iterator for the initialisation values (end).
 * @param allocator  The allocator to be used for getting the memory.
 */
template <class ValueType, class InputIterator>
inline void IFRUtil_NewArray_1(ValueType*& returnaddr, 
                             InputIterator begin,
                             InputIterator end,
                             SAPDBMem_IRawAllocator& allocator,
                               IFR_Bool& memory_ok,
                               IFRUtil_TrueType f)
{
    //<<< MEMCHECK
    if(!memory_ok) {
      returnaddr = 0;
      return;
    }
    //>>> MEMCHECK
 
    IFR_size_t count = end - begin;
    if(count == 0) {
        returnaddr = 0;
        return;
    } else {
        returnaddr = (ValueType *) allocator.Allocate(sizeof(ValueType) * count);
        if(returnaddr == 0) {
            memory_ok = false;
            return;
        } else {
            ValueType *p=returnaddr;
            while(begin != end) {
                new (p) ValueType(*begin);
                ++begin;
                ++p;
            }
        }
    }
}

/**
 * @ingroup IFR_Mem
 * Makes a new array. The elements are initialized to the elements of the provided 
 * value range <code>[begin, end)</code>.
 * @param returnaddr The return address.
 * @param begin      The input iterator for the initialisation values (begin).
 * @param end        The input iterator for the initialisation values (end).
 * @param allocator  The allocator to be used for getting the memory.
 */
template <class ValueType, class InputIterator>
inline void IFRUtil_NewArray_1(ValueType*& returnaddr, 
                             InputIterator begin,
                             InputIterator end,
                             SAPDBMem_IRawAllocator& allocator,
                             IFR_Bool& memory_ok,
                             IFRUtil_FalseType f)
{
    //<<< MEMCHECK
    if(!memory_ok) {
      returnaddr = 0;
      return;
    }
    //>>> MEMCHECK
 
    IFR_size_t count = end - begin;
    if(count == 0) {
        returnaddr = 0;
        return;
    } else {
        returnaddr = (ValueType *) allocator.Allocate(sizeof(ValueType) * count);
        if(returnaddr == 0) {
            memory_ok = false;
            return;
        } else {
            ValueType *p=returnaddr;
            while(begin != end) {
                new (p) ValueType(IFRUtil_CopyTraits<ValueType>::init_helper(&allocator, *begin));
                IFRUtil_CopyTraits<ValueType>::copy(*p, *begin, memory_ok);
                ++begin;
                ++p;
            }
        }
    }
}



/**
 * Destroys an array.
 * @param address   The address where the array is located.
 * @param size      The number of array elements.
 * @param allocator The allocator that is being used to deallocate the memory.
 */
template <class ValueType>
inline void IFRUtil_DeleteArray(ValueType *address,
                                IFR_size_t size,
                                SAPDBMem_IRawAllocator& allocator)
{
    if(address == 0) {
        return;
    } else {
        for(IFR_size_t i=0; i<size; ++i) {
            (address + i)->~ValueType();
        }
        allocator.Deallocate(address);
    }
}

/**
 * Destroys array elements. All elements in range <code>[begin, end)</code> are destroyed.
 * @param begin The begin of the array.
 * @param end   The end of the array.
 */
template <class ValueType>
inline void IFRUtil_DestroyArray(ValueType *begin,
                                 ValueType *end)
{
    while(begin != end) {
        begin->~ValueType();
        ++begin;
    }
    return;
}

/**
 * @ingroup IFR_Mem
 * Creates a new instance of the class at the specified place.
 * @param p The pointer where to create the instance.
 * @param v The value.
 */
template <class ValueType, class InitType>
inline void IFRUtil_Construct(ValueType *p, const InitType& v)
{
    new (p) ValueType(v);
    return;
}

/**
 * @ingroup IFR_Mem
 * Constructs an array at the designated place <code>[begin,
 * end)</code>. The default constructor is used.
 * @param begin The array begin.
 * @param end   The array end.
 */
template <class ValueType>
inline void IFRUtil_ConstructArray(ValueType *begin, ValueType *end)
{
    while(begin != end) {
        new (begin) ValueType();
        ++begin;
    }
}

/**
 * @ingroup IFR_Mem
 * Constructs an array at the designated place <code>[begin,
 * end)</code>. The default constructor is used.
 * @param begin The array begin.
 * @param end   The array end.
 * @param init  The initialization value.
 */
template <class ValueType, class InitType>
inline void IFRUtil_ConstructArray(ValueType *begin, 
                                   ValueType *end, 
                                   const InitType& init,
                                   IFR_Bool& memory_ok)
{
    typename IFRUtil_CopyTraits<ValueType>::IsDefaultConstructible i;
    i=i; // otherwise 'uninitialized' warnings
    IFRUtil_ConstructArray_1(begin, end, init, memory_ok, i);
}

/**
 * Array construction method for constructible types.
 */
template <class ValueType, class InitType>
inline void IFRUtil_ConstructArray_1(ValueType *begin, 
                                     ValueType *end, 
                                     const InitType& init,
                                     IFR_Bool& memory_ok,
                                     IFRUtil_TrueType dummy)
{
    //<<< MEMCHECK
    if(!memory_ok) {
        return;
    }
    //>>> MEMCHECK
    while(begin != end) {
        new (begin) ValueType(init);
        ++begin;
    }
}

/**
 * Array construction method for non-constructible types.
 */
template <class ValueType, class InitType>
inline void IFRUtil_ConstructArray_1(ValueType *begin, 
                                     ValueType *end, 
                                     const InitType& init,
                                     IFR_Bool& memory_ok,
                                     IFRUtil_FalseType dummy)
{
    //<<< MEMCHECK
    if(!memory_ok) {
        return;
    }
    //>>> MEMCHECK
    ValueType *start=begin;
    while(begin != end) {
        // Call the helper constructor,
        // and the copy helper afterwards.
        new (begin) ValueType(IFRUtil_CopyTraits<ValueType>::init_helper(0, init));
        IFRUtil_CopyTraits<ValueType>::copy(*begin, init, memory_ok);
        //<<< MEMCHECK
        if(!memory_ok) {
            do {
                start->~ValueType();
                ++start;
            } while(start <= begin);
            return;
        }
        //>>> MEMCHECK
        ++begin;
    }
}


/**
 * Destroys an instance.
 * @param p The pointer where to create the instance.
 */
template <class Value>
inline void IFRUtil_Destroy(Value *p)
{
    p->~Value();
    return;
}


/**
 * @ingroup IFR_Mem
 * Deletes an instance. This should be used when the placement new operator for
 * the raw allocator was used to create the instance.
 * @param p The pointer where to delete the instance.
 */
template <class ValueType>
inline void IFRUtil_Delete(ValueType *p, SAPDBMem_IRawAllocator& allocator)
{
    if(p) {
        p->~ValueType();
        allocator.Deallocate(p);    
    }
    return;
}

/**
 * @ingroup IFR_Mem
 * Deletes an instance. This should be used when the placement new operator for
 * the raw allocator was used to create the instance. It is used in places where
 * a destructor is inaccessible.
 * @param p The pointer where to delete the instance.
 * @param type The type name.
 * @param allocator The allocator.
 */
#define IFRUTIL_DELETEOBJECT(p, type, allocator)    \
do {                                                \
    if(p == 0) {                                    \
        break;                                      \
    } else {                                        \
        p->~type();                                 \
        allocator.Deallocate(p);                    \
    }                                               \
} while(0)

IFR_END_NAMESPACE

#endif // IFR_MEMORY_DEBUG

#endif // IFRUTIL_NEW_H

