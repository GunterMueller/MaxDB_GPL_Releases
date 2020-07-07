/*!*****************************************************************************

  module:       SAPDB_AutoPtr.hpp

  ------------------------------------------------------------------------------

  responsible:  Till Luhmann

  special area: SAPDB Common

  description:  Common auto_ptr templates

                - see also: Scott Meyers "Mehr effektiv C++ programmieren", Kap. 8

                The following classes and class templates are included in this file:

                - SAPDB_AutoVoidPtr
                - SAPDB_AutoPtr<T>
                - SAPDB_AutoArrayPtr<T>
                - SAPDB_AutoDestroyPtr<T>

  see also:

  ------------------------------------------------------------------------------

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


*******************************************************************************/


#ifndef SAPDB_AUTO_PTR_HPP
#define SAPDB_AUTO_PTR_HPP


#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"



/*!*****************************************************************************

   class: SAPDB_AutoVoidPtr

   description: autoptr for raw memory blocks 

*******************************************************************************/
class SAPDB_AutoVoidPtr
{
public:

    /*!-------------------------------------------------------------------------
    declaration : Pointer
    description:  pointer to instance of T
    --------------------------------------------------------------------------*/
    typedef void*                                               Pointer;
    /*!-------------------------------------------------------------------------
    declaration : Allocator
    description:  allocator for objects
    --------------------------------------------------------------------------*/
    typedef SAPDBMem_IRawAllocator                              Allocator;


    /*!-------------------------------------------------------------------------
    function:     SAPDB_AutoVoidPtr()
    description:  Constructor
    arguments:    alloc [in]    pointer (default: 0) or reference to allocator 
                                from which memory block has been allocated
                  ptr   [in]    pointer to memory block
                                (default: 0)
    prototypes:   2
    --------------------------------------------------------------------------*/
    explicit SAPDB_AutoVoidPtr
        (Allocator*                     alloc = 0,
         Pointer                        pmem  = 0)
        :
         m_DataPtr(pmem),
         m_Allocator(alloc)
    {
        SAPDBERR_ASSERT_ARGUMENT((pmem == 0) || ((pmem != 0) && (alloc != 0)));
    }

    explicit SAPDB_AutoVoidPtr
        (Allocator&                     alloc,
         Pointer                        pmem  = 0)
        :
         m_DataPtr(pmem),
         m_Allocator(&alloc)
    {}

    /*!-------------------------------------------------------------------------
    function:     SAPDB_AutoVoidPtr()
    description:  Copy constructor

                  - Transfers memory pointer from 'autoptr' to this.
                  - 'autoptr' is released.

    arguments:    autoptr [in]    autoptr to be assigned
    --------------------------------------------------------------------------*/
    SAPDB_AutoVoidPtr
        (SAPDB_AutoVoidPtr&                 autoptr)
    {
        m_Allocator = autoptr.m_Allocator;                 // do not change
        m_DataPtr   = autoptr.Release();                   // this sequence!
    }
  
    /*!-------------------------------------------------------------------------
    function:     ~SAPDB_AutoVoidPtr()
    description:  Destructor

                  - Deallocates memory.
    --------------------------------------------------------------------------*/
    ~SAPDB_AutoVoidPtr()
    {
        DeleteMemBlock();
    }

    /*!-------------------------------------------------------------------------
    function:     operator=()
    description:  Assignment operator

                  - Transfers memory pointer from 'autoptr' to this.
                  - 'autoptr' is released.

    arguments:    autoptr [in]    autoptr to be assigned
    return:       (SAPDB_AutoVoidPtr&) *this
    --------------------------------------------------------------------------*/
    SAPDB_AutoVoidPtr& operator=
        (SAPDB_AutoVoidPtr&                 autoptr)
    {
        if (this != &autoptr)
        {
            if (autoptr.IsAssigned())
            {
                if (m_DataPtr != autoptr.GetPtr())
                    Reset(autoptr.GetAllocator(), autoptr.Release());
                else // both autoptrs point to the same mem block; release 'autoptr'
                    autoptr.Release();
            }
            else
                Release();
        }

        return *this;
    }

    /*!-------------------------------------------------------------------------
    function:     GetPtr()
    description:  Same as operator->()
    return:       (Pointer) pointer to memory block
    --------------------------------------------------------------------------*/
    Pointer GetPtr() const
    {
        SAPDBERR_ASSERT_STATE(IsAssigned());
        return m_DataPtr;
    }

    /*!-------------------------------------------------------------------------
    function:     SetPtr()
    description:  Sets pointer of this autoptr

                  - 'ptr' must point to an object which has been allocated
                    by the same allocator 'm_Allocator'.
                  - Nothing happens with the initially hosted object.
    arguments:    pmem [in] pointer to memory block
    return:       assigned pointer
    --------------------------------------------------------------------------*/
    Pointer SetPtr
        (Pointer                        pmem)
    {
        SAPDBERR_ASSERT_STATE(m_Allocator != 0);
        return m_DataPtr = pmem;
    }

    /*!-------------------------------------------------------------------------
    function:     Release()
    description:  Releases memory pointer

                  - After this operation the actual autoptr is unassigned.
                  - The memory block is not deallocated.

    arguments:    none
    return:       (Pointer) released pointer to memory block
    --------------------------------------------------------------------------*/
    Pointer Release()
    {
        Pointer pobject = m_DataPtr;
        m_DataPtr     = 0;
        m_Allocator     = 0;
        return pobject;
    }

    /*!-------------------------------------------------------------------------
    function:     Reset()
    description:  Resets actual autoptr with another object

                  - The memory block initially pointed to is destroyed.
                  - A new mem block pointer and allocator reference is assigned.

    arguments:    alloc [in]    pointer (default: 0) or reference to allocator 
                                from which memory block has been allocated
                  ptr   [in]    pointer to memory block
                                (default: 0)
    return:       none
    prototypes:   2
    --------------------------------------------------------------------------*/
    void Reset
        (Allocator*                     alloc = 0,
         Pointer                        pobj  = 0)
    {
        SAPDBERR_ASSERT_ARGUMENT((pobj == 0) || ((pobj != 0) && (alloc != 0)));
        DeleteMemBlock();
        m_DataPtr = pobj;
        m_Allocator = alloc;
    }

    void Reset
        (Allocator&                     alloc,
         Pointer                        pobj  = 0)
    {
        DeleteMemBlock();
        m_Allocator = &alloc;
        m_DataPtr = pobj;
    }

    /*!-------------------------------------------------------------------------
    function:     GetAllocator()
    description:  Returns pointer to actual allocator
    return:       (Allocator*) pointer to allocator
    --------------------------------------------------------------------------*/
    Allocator* GetAllocator() const
    {
        return m_Allocator;
    }

    /*!-------------------------------------------------------------------------
    function:     IsAssigned()
    description:  Checks whether this autoptr is assigned (memblock != 0)
    return:       (SAPDB_Bool) true if assigned
    --------------------------------------------------------------------------*/
    SAPDB_Bool IsAssigned() const
    {
        return m_DataPtr != 0;
    }


private:

    /*--------------------------------------------------------------------------
    function:     DeleteMemBlock()
    description:  Deallocates memory block
    return:       none
    --------------------------------------------------------------------------*/
    void DeleteMemBlock()
    {
        if (IsAssigned())
            m_Allocator->Deallocate(m_DataPtr);
    }

    Pointer         m_DataPtr;
    Allocator*      m_Allocator;

};
/*!*****************************************************************************

   endclass: SAPDB_AutoVoidPtr

*******************************************************************************/



/*!*****************************************************************************

   class: SAPDB_AutoPtr

   description: Common auto_ptr template

   template arguments:   
   
   - T       object type

*******************************************************************************/
template <class T>
class SAPDB_AutoPtr
{
public:

    /*!-------------------------------------------------------------------------
    declaration : ValueType
    description:  type of T
    --------------------------------------------------------------------------*/
    typedef T                                                   ValueType;
    /*!-------------------------------------------------------------------------
    declaration : Pointer
    description:  pointer to instance of T
    --------------------------------------------------------------------------*/
    typedef T*                                                  Pointer;
    /*!-------------------------------------------------------------------------
    declaration : Reference
    description:  reference to instance of T
    --------------------------------------------------------------------------*/
    typedef T&                                                  Reference;
    /*!-------------------------------------------------------------------------
    declaration : Allocator
    description:  allocator for objects
    --------------------------------------------------------------------------*/
    typedef SAPDBMem_IRawAllocator                              Allocator;


    /*!-------------------------------------------------------------------------
    function:     SAPDB_AutoPtr()
    description:  Constructor
    arguments:    alloc [in]    pointer (default: 0) or reference to allocator 
                                from which object has been allocated
                  pobj  [in]    pointer to constructed dynamic object 
                                (default: 0)
    prototypes:   2
    --------------------------------------------------------------------------*/
    explicit SAPDB_AutoPtr
        (Allocator*                     alloc = 0,
         Pointer                        pobj  = 0)
        :
         m_ObjectPtr(pobj),
         m_Allocator(alloc)
    {
        SAPDBERR_ASSERT_ARGUMENT((pobj == 0) || ((pobj != 0) && (alloc != 0)));
    }

    explicit SAPDB_AutoPtr
        (Allocator&                     alloc,
         Pointer                        pobj  = 0)
        :
         m_ObjectPtr(pobj),
         m_Allocator(&alloc)
    {}

    /*!-------------------------------------------------------------------------
    function:     SAPDB_AutoPtr()
    description:  Copy constructor

                  - Transfers object handle from 'autoptr' to this.
                  - 'autoptr' is released.

    arguments:    autoptr [in]    autoptr to be assigned
    --------------------------------------------------------------------------*/
    SAPDB_AutoPtr
        (SAPDB_AutoPtr<T>&              autoptr)
    {
        m_Allocator = autoptr.m_Allocator;               // do not change
        m_ObjectPtr = autoptr.Release();                 // this sequence!
    }
  
    /*--------------------------------------------------------------------------
    function:     SAPDB_AutoPtr()
    description:  Copy constructor

                    ** NOT YET SUPPORTED BY COMPILERS **
                    ** WHEN SUPPORTED REPLACE PRECEEDING METHOD WITH THIS METHOD **
                  - Copy constructor as a member template. 
                  - Initializes a new SAPDB_AutoPtr<T> with a compatible
                    SAPDB_AutoPtr<U>.
                  - Transfers object handle from 'autoptr' to this.
                  - 'autoptr' is released.

    arguments:    autoptr [in]    autoptr to be assigned
    --------------------------------------------------------------------------*/
    /*  
    template <class U>
    SAPDB_AutoPtr
        (SAPDB_AutoPtr<U>&              autoptr)
    {
        m_Allocator = autoptr.GetAllocator();                // do not change
        m_ObjectPtr = autoptr.Release();                     // this sequence!
    }
    */

    /*!-------------------------------------------------------------------------
    function:     ~SAPDB_AutoPtr()
    description:  Destructor

                  - Calls the destructor of the hosted object,
                    then deallocates the object.
    --------------------------------------------------------------------------*/
    ~SAPDB_AutoPtr()
    {
        DeleteObject();
    }

    /*!-------------------------------------------------------------------------
    function:     operator=()
    description:  Assignment operator

                  - Transfers object handle from 'autoptr' to this.
                  - 'autoptr' is released.

    arguments:    autoptr [in]    autoptr to be assigned
    return:       (SAPDB_AutoPtr<T>&) *this
    --------------------------------------------------------------------------*/
    SAPDB_AutoPtr<T>& operator=
        (SAPDB_AutoPtr<T>&              autoptr)
    {
        if (this != &autoptr)
        {
            if (autoptr.IsAssigned())
            {
                if (m_ObjectPtr != autoptr.GetPtr())
                    Reset(autoptr.GetAllocator(), autoptr.Release());
                else // both autoptrs are hosting the same object; release 'autoptr'
                    autoptr.Release();
            }
            else
                Release();
        }

        return *this;
    }

    /*--------------------------------------------------------------------------
    function:     operator=()
    description:  Assignment operator for SAPDB_AutoPtr<U>

                    ** NOT YET SUPPORTED BY COMPILERS **
                    ** WHEN SUPPORTED REPLACE PRECEEDING METHOD WITH THIS METHOD **
                  - Assignment operator as a member template. 
                  - Initializes SAPDB_AutoPtr<T> with a compatible
                    SAPDB_AutoPtr<U>.
                  - Transfers object handle from 'autoptr' to this.
                  - 'autoptr' is released.

    arguments:    autoptr [in]    autoptr to be assigned
    return:       (SAPDB_AutoPtr<T>&) *this
    --------------------------------------------------------------------------*/
    /*    
    template <class U>
    SAPDB_AutoPtr<T>& operator=
        (SAPDB_AutoPtr<U>&              autoptr)
    {
        if (this != &autoptr)
        {
            if (autoptr.IsAssigned())
            {
                if (m_ObjectPtr != autoptr.GetPtr())
                    Reset(autoptr.GetAllocator(), autoptr.Release());
                else // both autoptrs are hosting the same object; release 'autoptr'
                    autoptr.Release();
            }
            else
                Release();
        }

        return *this;
    }
    */

    /*!-------------------------------------------------------------------------
    function:     operator*()
    description:  Dereference operator
    return:       (Reference) reference to hosted object
    --------------------------------------------------------------------------*/
    Reference operator*() const
    {
        SAPDBERR_ASSERT_STATE(IsAssigned());
        return *m_ObjectPtr;
    }    

    /*!-------------------------------------------------------------------------
    function:     operator->()
    description:  Pointer operator

                  The definition of operator ->() produces a warning for basic 
                  data types. This warning is disabled for MS Visual C++.
    return:       (Pointer) pointer to hosted object
    --------------------------------------------------------------------------*/
#if defined(_WIN32) || defined(WIN32)
#pragma warning(disable : 4284)
#endif
    Pointer operator->() const
    {
        SAPDBERR_ASSERT_STATE(IsAssigned());
        return m_ObjectPtr;
    }
#if defined(_WIN32) || defined(WIN32)
#pragma warning(default : 4284)
#endif

    /*!-------------------------------------------------------------------------
    function:     GetPtr()
    description:  Same as operator->()
    return:       (Pointer) pointer to hosted object
    --------------------------------------------------------------------------*/
    Pointer GetPtr() const
    {
        SAPDBERR_ASSERT_STATE(IsAssigned());
        return m_ObjectPtr;
    }

    /*!-------------------------------------------------------------------------
    function:     SetPtr()
    description:  Sets pointer of this autoptr

                  - 'pobj' must point to an object which has been allocated
                    by the same allocator 'm_Allocator'.
                  - Nothing happens with the initially hosted object.
    arguments:    ptr [in] pointer to object
    return:       assigned pointer
    --------------------------------------------------------------------------*/
    Pointer SetPtr
        (Pointer                        pobj)
    {
        SAPDBERR_ASSERT_STATE(m_Allocator != 0);
        return m_ObjectPtr = pobj;
    }

    /*!-------------------------------------------------------------------------
    function:     Release()
    description:  Releases object pointer

                  - After this operation the actual autoptr is unassigned.
                  - The hosted object is not destroyed.

    arguments:    none
    return:       (Pointer) released pointer to hosted object
    --------------------------------------------------------------------------*/
    Pointer Release()
    {
        Pointer pobject = m_ObjectPtr;
        m_ObjectPtr     = 0;
        m_Allocator     = 0;
        return pobject;
    }

    /*!-------------------------------------------------------------------------
    function:     Reset()
    description:  Resets actual autoptr with another object

                  - The initially hosted object is destroyed.
                  - A new object pointer and allocator reference is assigned.

    arguments:    alloc [in]    pointer (default: 0) or reference to allocator 
                                from which object has been allocated
                  pobj  [in]    pointer to constructed dynamic object 
                                (default: 0)
    return:       none
    prototypes:   2
    --------------------------------------------------------------------------*/
    void Reset
        (Allocator*                     alloc = 0,
         Pointer                        pobj  = 0)
    {
        SAPDBERR_ASSERT_ARGUMENT((pobj == 0) || ((pobj != 0) && (alloc != 0)));
        DeleteObject();
        m_ObjectPtr = pobj;
        m_Allocator = alloc;
    }

    void Reset
        (Allocator&                     alloc,
         Pointer                        pobj  = 0)
    {
        DeleteObject();
        m_Allocator = &alloc;
        m_ObjectPtr = pobj;
    }


    /*!-------------------------------------------------------------------------
    function:     GetAllocator()
    description:  Returns pointer to allocator of actual object
    return:       (Allocator*) pointer to allocator
    --------------------------------------------------------------------------*/
    Allocator* GetAllocator() const
    {
        return m_Allocator;
    }

    /*!-------------------------------------------------------------------------
    function:     IsAssigned()
    description:  Checks whether this autoptr is assigned (hosts an object)
    return:       (SAPDB_Bool) true if assigned
    --------------------------------------------------------------------------*/
    SAPDB_Bool IsAssigned() const
    {
        return m_ObjectPtr != 0;
    }


protected:

    /*--------------------------------------------------------------------------
    function:     DeleteObject()
    description:  Calls destructor for hosted object; then deletes object
    return:       none
    --------------------------------------------------------------------------*/
    virtual void DeleteObject()
    {
        if (IsAssigned())
        {
            m_ObjectPtr->~T();
            m_Allocator->Deallocate(m_ObjectPtr);
        }
    }

    Pointer         m_ObjectPtr;
    Allocator*      m_Allocator;

//    ** NOT YET SUPPORTED BY COMPILERS **
//    ** UNCOMMENT WHEN SUPPORTED **
//    template <class U> friend class SAPDB_AutoPtr<U>;
};
/*!*****************************************************************************

   endclass: SAPDB_AutoPtr

*******************************************************************************/



/*!*****************************************************************************

    class: SAPDB_AutoDestroyPtr

    description: Common auto_ptr template

    template arguments:   

    - T     object type

            class T must provide the following method:

            void T::Destroy(SAPDBMem_IRawAllocator&);

            This method must be designed to destroy 
            non-autoptr'd objects depending on the existence 
            of the autoptr'd instance of T.

*******************************************************************************/
template <class T>
class SAPDB_AutoDestroyPtr : public SAPDB_AutoPtr<T>
{
    /*!-------------------------------------------------------------------------
    declaration : AutoPtr
    description:  'normal' autoptr
    --------------------------------------------------------------------------*/
    typedef SAPDB_AutoPtr<T>                                    AutoPtr;
    /*!-------------------------------------------------------------------------
    declaration : ValueType
    description:  type of T
    --------------------------------------------------------------------------*/
    typedef T                                                   ValueType;
    /*!-------------------------------------------------------------------------
    declaration : Pointer
    description:  pointer to instance of T
    --------------------------------------------------------------------------*/
    typedef T*                                                  Pointer;
    /*!-------------------------------------------------------------------------
    declaration : Reference
    description:  reference to instance of T
    --------------------------------------------------------------------------*/
    typedef T&                                                  Reference;
    /*!-------------------------------------------------------------------------
    declaration : Allocator
    description:  allocator for objects
    --------------------------------------------------------------------------*/
    typedef SAPDBMem_IRawAllocator                              Allocator;

public:


    /*!-------------------------------------------------------------------------
    function:     SAPDB_AutoDestroyPtr()
    description:  Constructor
    arguments:    alloc [in]    pointer (default: 0) or reference to allocator 
                                from which object has been allocated
                  pobj  [in]    pointer to constructed dynamic object 
                                (default: 0)
    prototypes:   2
    --------------------------------------------------------------------------*/
    explicit SAPDB_AutoDestroyPtr
        (Allocator*                     alloc = 0,
         Pointer                        pobj  = 0)
        :
         AutoPtr(alloc, pobj)
    {}

    explicit SAPDB_AutoDestroyPtr
        (Allocator&                     alloc,
         Pointer                        pobj  = 0)
        :
         AutoPtr(alloc, pobj)
    {}

    /*!-------------------------------------------------------------------------
    function:     SAPDB_AutoDestroyPtr()
    description:  Copy constructor

                  - Transfers object handle from 'autoptr' to this.
                  - 'autoptr' is released.

    arguments:    autoptr [in]    autoptr to be assigned
    --------------------------------------------------------------------------*/
    SAPDB_AutoDestroyPtr
        (AutoPtr&                       autoptr)
        :
         AutoPtr(autoptr)
    {}
  
    /*!-------------------------------------------------------------------------
    function:     operator=()
    description:  Assignment operator

                  - Transfers object handle from 'autoptr' to this.
                  - 'autoptr' is released.

    arguments:    autoptr [in]    autoptr to be assigned
    return:       (SAPDB_AutoDestroyPtr<T>&) *this
    --------------------------------------------------------------------------*/
    SAPDB_AutoDestroyPtr<T>& operator=
        (AutoPtr&                       autoptr)
    {
        AutoPtr::operator=(autoptr);
        return *this;
    }

protected:

    /*--------------------------------------------------------------------------
    function:     DeleteObject()
    description:  Calls destructor for hosted object; then deletes object
    return:       none
    --------------------------------------------------------------------------*/
    virtual void DeleteObject()
    {
        if (this->IsAssigned())
        {
            this->m_ObjectPtr->Destroy(*(this->m_Allocator));
            this->m_Allocator->Deallocate(this->m_ObjectPtr);
        }
    }

};
/*!*****************************************************************************

   endclass: SAPDB_AutoDestroyPtr

*******************************************************************************/



/*!*****************************************************************************

   class: SAPDB_AutoArrayPtr

   description: Common auto_ptr template for arrays of T

   template arguments:   
   
   - T      object type

*******************************************************************************/
template <class T>
class SAPDB_AutoArrayPtr
{
public:

    /*!-------------------------------------------------------------------------
    declaration : ValueType
    description:  type of T
    --------------------------------------------------------------------------*/
    typedef T                                                   ValueType;
    /*!-------------------------------------------------------------------------
    declaration : SizeType
    description:  array size
    --------------------------------------------------------------------------*/
    typedef size_t                                              SizeType;
    /*!-------------------------------------------------------------------------
    declaration : IndexType
    description:  index within array
    --------------------------------------------------------------------------*/
    typedef SizeType                                            IndexType;
    /*!-------------------------------------------------------------------------
    declaration : Pointer
    description:  pointer to instance of T
    --------------------------------------------------------------------------*/
    typedef T*                                                  Pointer;
    /*!-------------------------------------------------------------------------
    declaration : Reference
    description:  reference to instance of T
    --------------------------------------------------------------------------*/
    typedef T&                                                  Reference;
    /*!-------------------------------------------------------------------------
    declaration : Allocator
    description:  allocator for objects
    --------------------------------------------------------------------------*/
    typedef SAPDBMem_IRawAllocator                              Allocator;


    /*!-------------------------------------------------------------------------
    function:     SAPDB_AutoArrayPtr()
    description:  Constructor
    arguments:    alloc [in]    pointer (default: 0) or reference to allocator 
                                from which object has been allocated
                  pobj  [in]    pointer to constructed dynamic array of objects 
                                (default: 0)
                  size  [in]    array size in elements
    prototypes:   2
    --------------------------------------------------------------------------*/
    explicit SAPDB_AutoArrayPtr
        (Allocator*                     alloc = 0,
         Pointer                        pobj  = 0,
         SizeType                       size  = 0)
        :
         m_Allocator(alloc),
         m_ArrayPtr (pobj),
         m_ArraySize(size)
    {
        SAPDBERR_ASSERT_ARGUMENT((pobj == 0) || ((pobj != 0) && (alloc != 0)));
    }

    explicit SAPDB_AutoArrayPtr
        (Allocator&                     alloc,
         Pointer                        pobj  = 0,
         SizeType                       size  = 0)
        :
         m_Allocator(&alloc),
         m_ArrayPtr (pobj),
         m_ArraySize(size)
    {}

    /*!-------------------------------------------------------------------------
    function:     SAPDB_AutoArrayPtr()
    description:  Copy constructor

                  - Transfers object handle from 'autoptr' to this.
                  - 'autoptr' is released.

    arguments:    autoptr [in]    autoptr to be assigned
    --------------------------------------------------------------------------*/
    SAPDB_AutoArrayPtr
        (SAPDB_AutoArrayPtr<T>&         autoptr)
    {
        m_Allocator = autoptr.m_Allocator;                 // do not change
        m_ArraySize = autoptr.m_ArraySize;
        m_ArrayPtr  = autoptr.Release();                   // this sequence!
    }
  
    /*--------------------------------------------------------------------------
    function:     SAPDB_AutoArrayPtr()
    description:  Copy constructor

                    ** NOT YET SUPPORTED BY COMPILERS **
                    ** WHEN SUPPORTED REPLACE PRECEEDING METHOD WITH THIS METHOD **
                  - Copy constructor as a member template. 
                  - Initializes a new SAPDB_AutoArrayPtr<T> with a compatible
                    SAPDB_AutoArrayPtr<U>.
                  - Transfers object array handle from 'autoptr' to this.
                  - 'autoptr' is released.

    arguments:    autoptr [in]    autoptr to be assigned
    --------------------------------------------------------------------------*/
    /*  
    template <class U>
    SAPDB_AutoArrayPtr
        (SAPDB_AutoArrayPtr<U>&         autoptr)
    {
        m_Allocator = autoptr.m_Allocator;                 // do not change
        m_ArraySize = autoptr.m_ArraySize;
        m_ArrayPtr  = autoptr.Release();                   // this sequence!
    }
    */

    /*!-------------------------------------------------------------------------
    function:     ~SAPDB_AutoArrayPtr()
    description:  Destructor

                  - Calls the destructor of the hosted object array,
                    then deallocates the object.
    --------------------------------------------------------------------------*/
    ~SAPDB_AutoArrayPtr()
    {
        DeleteArray();
    }

    /*!-------------------------------------------------------------------------
    function:     operator=()
    description:  Assignment operator

                  - Transfers object array handle from 'autoptr' to this.
                  - 'autoptr' is released.

    arguments:    autoptr [in]    autoptr to be assigned
    return:       (SAPDB_AutoArrayPtr<T>&) *this
    --------------------------------------------------------------------------*/
    SAPDB_AutoArrayPtr<T>& operator=
        (SAPDB_AutoArrayPtr<T>&              autoptr)
    {
        if (this != &autoptr)
        {
            if (autoptr.IsAssigned())
            {
                if (m_ArrayPtr != autoptr.GetPtr())
                {
                    SizeType size = autoptr.GetArraySize();
                    Reset(autoptr.GetAllocator(), autoptr.Release(), size);
                }
                else // both autoptrs are hosting the same object; release 'autoptr'
                    autoptr.Release();
            }
            else
                Release();
        }

        return *this;
    }

    /*--------------------------------------------------------------------------
    function:     operator=()
    description:  Assignment operator for SAPDB_AutoArrayPtr<U>

                    ** NOT YET SUPPORTED BY COMPILERS **
                    ** WHEN SUPPORTED REPLACE PRECEEDING METHOD WITH THIS METHOD **
                  - Assignment operator as a member template. 
                  - Initializes SAPDB_AutoArrayPtr<T> with a compatible
                    SAPDB_AutoArrayPtr<U>.
                  - Transfers object array handle from 'autoptr' to this.
                  - 'autoptr' is released.

    arguments:    autoptr [in]    autoptr to be assigned
    return:       (SAPDB_AutoArrayPtr<T>&) *this
    --------------------------------------------------------------------------*/
    /*    
    template <class U>
    SAPDB_AutoArrayPtr<T>& operator=
        (SAPDB_AutoArrayPtr<U>&         autoptr)
    {
        if (this != &autoptr)
        {
            if (autoptr.IsAssigned())
            {
                if (m_ArrayPtr != autoptr.GetPtr())
                {
                    SizeType size = autoptr.GetArraySize();
                    Reset(autoptr.GetAllocator(), autoptr.Release(), size);
                }
                else // both autoptrs are hosting the same object; release 'autoptr'
                    autoptr.Release();
            }
            else
                Release();
        }

        return *this;
    }
    */

    /*!-------------------------------------------------------------------------
    function:     operator*()
    description:  Dereference operator
    return:       (Reference) reference to first array element
    --------------------------------------------------------------------------*/
    Reference operator*() const
    {
        SAPDBERR_ASSERT_STATE(IsAssigned());
        return *m_ArrayPtr;
    }

    /*!-------------------------------------------------------------------------
    function:     operator[]()
    description:  Direct array access operator (non const)
    return:       (Reference) reference to specified array element
    --------------------------------------------------------------------------*/
    Reference operator[]
        (IndexType                      index)
    {
        SAPDBERR_ASSERT_STATE(IsAssigned());
        SAPDBERR_ASSERT_ARGUMENT(index < m_ArraySize);
        return m_ArrayPtr[index];
    }

    /*!-------------------------------------------------------------------------
    function:     operator[]()
    description:  Direct array access operator (const)
    return:       (ValueType) copy of specified array element
    --------------------------------------------------------------------------*/
    ValueType operator[]
        (IndexType                      index) const
    {
        SAPDBERR_ASSERT_STATE(IsAssigned());
        SAPDBERR_ASSERT_ARGUMENT(index < m_ArraySize);
        return m_ArrayPtr[index];
    }

    /*!-------------------------------------------------------------------------
    function:     GetPtr()
    description:  Same as operator->()
    return:       (Pointer) pointer to array
    --------------------------------------------------------------------------*/
    Pointer GetPtr() const
    {
        SAPDBERR_ASSERT_STATE(IsAssigned());
        return m_ArrayPtr;
    }

    /*!-------------------------------------------------------------------------
    function:     Release()
    description:  Releases object pointer

                  - After this operation the actual autoptr is unassigned.
                  - The hosted object array is not destroyed.

    arguments:    none
    return:       (Pointer) released pointer to array
    --------------------------------------------------------------------------*/
    Pointer Release()
    {
        Pointer pobject = m_ArrayPtr;
        m_Allocator     = 0;
        m_ArrayPtr      = 0;
        m_ArraySize     = 0;
        return pobject;
    }

    /*!-------------------------------------------------------------------------
    function:     Reset()
    description:  Resets actual autoptr with another object

                  - The initially hosted object array is destroyed.
                  - A new object pointer and allocator reference is assigned.

    arguments:    alloc [in]    pointer (default: 0) or reference to allocator 
                                from which object array has been allocated
                  pobj  [in]    pointer to constructed dynamic object array
                                (default: 0)
                  size  [in]    array size in elements
    return:       none
    prototypes:   2
    --------------------------------------------------------------------------*/
    void Reset
        (Allocator*                     alloc = 0,
         Pointer                        pobj  = 0,
         SizeType                       size  = 0)
    {
        SAPDBERR_ASSERT_ARGUMENT((pobj == 0) || ((pobj != 0) && (alloc != 0)));
        DeleteArray();
        m_Allocator = alloc;
        m_ArrayPtr  = pobj;
        m_ArraySize = size;
    }

    void Reset
        (Allocator&                     alloc,
         Pointer                        pobj  = 0,
         SizeType                       size  = 0)
    {
        DeleteArray();
        m_Allocator = &alloc;
        m_ArrayPtr  = pobj;
        m_ArraySize = size;
    }


    /*!-------------------------------------------------------------------------
    function:     GetAllocator()
    description:  Returns pointer to allocator of actual object
    return:       (Allocator*) pointer to allocator
    --------------------------------------------------------------------------*/
    Allocator* GetAllocator() const
    {
        SAPDBERR_ASSERT_STATE(IsAssigned());
        return m_Allocator;
    }

    /*!-------------------------------------------------------------------------
    function:     GetArraySize()
    description:  Returns number of array elements
    return:       (SizeType) element count
    --------------------------------------------------------------------------*/
    SizeType GetArraySize() const
    {
        SAPDBERR_ASSERT_STATE(IsAssigned());
        return m_ArraySize;
    }

    /*!-------------------------------------------------------------------------
    function:     IsAssigned()
    description:  Checks whether this autoptr is assigned (hosts an object)
    return:       (SAPDB_Bool) true if assigned
    --------------------------------------------------------------------------*/
    SAPDB_Bool IsAssigned() const
    {
        return m_ArrayPtr != 0;
    }


private:

    /*--------------------------------------------------------------------------
    function:     DeleteArray()
    description:  Calls destructor for each array element; then deletes array
    return:       none
    --------------------------------------------------------------------------*/
    void DeleteArray()
    {
        if (IsAssigned())
        {
            Pointer iter = m_ArrayPtr;
            Pointer end  = m_ArrayPtr + m_ArraySize;

            while (iter != end) {
              iter->~T();
              ++iter;
            };
            m_Allocator->Deallocate(m_ArrayPtr);
        }
    }

    Allocator*      m_Allocator;
    Pointer         m_ArrayPtr;
    SizeType        m_ArraySize;

//    ** NOT YET SUPPORTED BY COMPILERS **
//    ** UNCOMMENT WHEN SUPPORTED **
//    template <class U> friend class SAPDB_AutoArrayPtr<U>;
};
/*!*****************************************************************************

   endclass: SAPDB_AutoArrayPtr

*******************************************************************************/







#endif // SAPDB_AUTO_PTR_HPP

