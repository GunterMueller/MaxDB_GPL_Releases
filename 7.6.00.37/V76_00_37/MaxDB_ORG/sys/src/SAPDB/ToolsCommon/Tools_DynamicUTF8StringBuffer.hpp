/*!******************************************************************************

  module:		 Tools_DynamicUTF8StringBuffer.hpp

  ------------------------------------------------------------------------------

  responsible:  TillL

  special area: SAPDBCommon/Fields

  description:  Class for dynamic utf8 string buffers

                - no templates
                - no exceptions
                - no allocators
                - uses ansi new and delete
                - uses ansi assert

  see also:     

  ------------------------------------------------------------------------------

                         Copyright (c) 2000-2005 SAP AG




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


#ifndef TOOLS_DYNAMICUTF8STRINGBUFFER_HPP
#define TOOLS_DYNAMICUTF8STRINGBUFFER_HPP

#include <string.h>
#include <memory.h>
#include <assert.h>
#include "SAPDBCommon/SAPDB_string.h"
#include "ToolsCommon/Tools_UTF8StringElement.hpp"


// allocator block size increment:
#define Tools_DynamicUTF8StringBuffer_ALLOCDELTA      32


/*!*****************************************************************************

   class: Tools_DynamicUTF8StringBuffer

*******************************************************************************/
class Tools_DynamicUTF8StringBuffer
{
public:
    /*!-------------------------------------------------------------------------
    declaration : SizeType
    description:  The range for buffer lengths
    --------------------------------------------------------------------------*/
    typedef SAPDB_UInt                                  SizeType;
    /*!-------------------------------------------------------------------------
    declaration : IndexType
    description:  The range for positions within buffers
    --------------------------------------------------------------------------*/
    typedef SizeType                                    IndexType;
    /*!-------------------------------------------------------------------------
    declaration : BasisType
    description:  Type of data contained in buffer
    --------------------------------------------------------------------------*/
    typedef Tools_UTF8StringElement::BasisElementType   BasisType;
    /*!-------------------------------------------------------------------------
    declaration : Pointer
    description:  Pointer to buffer data
    --------------------------------------------------------------------------*/
    typedef Tools_UTF8StringElement::Pointer            Pointer;
    /*!-------------------------------------------------------------------------
    declaration : Iterator
    description:  Buffer iterator
    --------------------------------------------------------------------------*/
    typedef Pointer                                     Iterator;
    /*!-------------------------------------------------------------------------
    declaration : ConstPointer
    description:  Pointer to constant buffer data
    --------------------------------------------------------------------------*/
    typedef Tools_UTF8StringElement::ConstPointer       ConstPointer;
    /*!-------------------------------------------------------------------------
    declaration : ConstIterator
    description:  Buffer iterator to constant data
    --------------------------------------------------------------------------*/
    typedef ConstPointer                                ConstIterator;

    /*!-------------------------------------------------------------------------
    function:     Tools_DynamicUTF8StringBuffer()
    description:  Constructor
    
    arguments:    none
    return value: none
    --------------------------------------------------------------------------*/
    explicit Tools_DynamicUTF8StringBuffer
        ();

    /*!-------------------------------------------------------------------------
    function:     ~Tools_DynamicUTF8StringBuffer()
    description:  Destructor
    
    arguments:    none
    return value: none
    --------------------------------------------------------------------------*/
    ~Tools_DynamicUTF8StringBuffer
        ();

    /*!-------------------------------------------------------------------------
    function:     Deassign()
    description:  Deassigns the buffer resource

                  - After this operation the buffer is unassigned.
    arguments:    none
    return value: none
    --------------------------------------------------------------------------*/
    void Deassign();

    /*!-------------------------------------------------------------------------
    function:     Append()
    description:  Appends argument to buffer
    
                  - The contents of a given argument is copied to the buffer, 
                    starting at actual fill position.
                  - For given 'src' and 'len', 'len' elements of BasisType are 
                    copied which can be found at 'src'.
                  - A single element 'val' may also be appended to the buffer.
                  - If also 'count' is given, 'count' copies of element 'val' 
                    are appended to the buffer.
    arguments:    src   [in]   pointer to element array
                  len   [in]   count of elements in element array
                  count [in]   number of copies of 'val'
                  val   [in]   source value
    return value: none
    prototypes:   3
    --------------------------------------------------------------------------*/
    void Append
        (const void*                    src, 
         SizeType                       len);

    void Append
        (BasisType                      val);

    void Append
        (SizeType                       count,
         BasisType                      val);

    /*!-------------------------------------------------------------------------
    function:     Insert()
    description:  Inserts argument into buffer
                    
                  - The contents of a given argument is copied to the buffer, 
                    beginning at position 'pos' within the buffer.
                  - For given 'src' and 'len', 'len' elements of BasisType are inserted 
                    which can be found at 'src'.
                  - If 'count' and 'val' are given, 'count' copies of element 'val' 
                    are inserted into the buffer.
    arguments:    pos   [in]   position in buffer at which insert takes place
                  src   [in]   pointer to element array
                  len   [in]   count of elements in element array
                  count [in]   number of copies of 'val' to be inserted
                  val   [in]   source value
    return value: none
    prototypes:   2
    --------------------------------------------------------------------------*/
    void Insert
        (IndexType                      pos,
         const void*                    src, 
         SizeType                       len);

    void Insert
        (IndexType                      pos,
         SizeType                       count,
         BasisType                      val);

    /*!-------------------------------------------------------------------------
    function:     Erase()
    description:  Erases contents of buffer part
    
                  - At most 'len' elements of BasisType are erased from the buffer, 
                    beginning at position 'pos'.
                  - Elements are not removed beyond the actual fill position.
    arguments:    pos   [in]  erase position in buffer
                  len   [in]  count of elements to be removed
    return value: none
    --------------------------------------------------------------------------*/
    void Erase
        (IndexType                      pos,
         SizeType                       len);

    /*!-------------------------------------------------------------------------
    function:     Copy()
    description:  Copies part of buffer contents to specified destination
    
                  - At most 'len' elements of BasisType are copied from the 
                    buffer to 'dest', starting at 'pos' within buffer.
                  - Elements are not copied beyond the actual fill position.
    arguments:    dest [in]   destination address for the copy
                  pos  [in]   position of first element to be copied
                  len  [in]   count of elements to be copied
    return value: none
    --------------------------------------------------------------------------*/
    void Copy
        (void*                          dest, 
         IndexType                      pos,
         SizeType                       len) const;

    /*!-------------------------------------------------------------------------
    function:     Replace()
    description:  Replaces buffer part with given argument
    
                  - At most 'len' elements of BasisType are removed from the buffer, 
                    beginning at position 'pos'.
                  - Elements are not removed beyond the actual fill position.
                  - If 'src' and 'inslen' are given, the removed elements are 
                    replaced with 'replen' elements which can be found at 'src'.
                  - If 'count' and 'val' are given, the removed elements are 
                    replaced by 'count' copies of element 'val'.
                  - Count of removed and inserted elements do not need to match.
    arguments:    pos    [in]  erase position in buffer
                  len    [in]  count of elements to be removed
                  src    [in]  pointer to element array
                  inslen [in]  count of elements to be inserted
                  count  [in]  number of copies of 'val' to be inserted
                  val    [in]  source value
    return value: none
    prototypes:   2
    --------------------------------------------------------------------------*/
    void Replace
        (IndexType                      pos,
         SizeType                       len,
         const void*                    src,
         SizeType                       inslen);

    void Replace
        (IndexType                      pos,
         SizeType                       len,
         SizeType                       count,
         BasisType                      val);

    /*!-------------------------------------------------------------------------
    function:     Clear()
    description:  Clears the contents of the buffer

                  - After this operation the buffer is empty
    arguments:    none
    return value: none
    --------------------------------------------------------------------------*/
    void Clear();

    /*--------------------------------------------------------------------------
    function:     ProvideCapacity
    description:  Reserves resource space for elements

                  - Tries to reserve resource space for at least
                    the requested amount of elements.
                  - On default, all elements are preserved on rellocation.
                  - If 'used' is specified, only the first 'used' elements in the 
                    resource are preserved on reallocation.
    arguments:    requested [in]  requested resource size in elements
                  used      [in]  number of elements to be saved on realloc
    return value: (SAPDB_Bool)
                  success:          true,
                  bad alloc:        false
    prototypes:   2
    --------------------------------------------------------------------------*/
    SAPDB_Bool ProvideCapacity
        (SizeType                       requested);

    SAPDB_Bool ProvideCapacity
        (SizeType                       requested, 
         SizeType                       used);

    /*!-------------------------------------------------------------------------
    function:     Capacity()
    description:  Returns capacity of buffer in elements of BasisType

                  - For dynamic buffer resource, this returns the actual count
                    of elements that can be stored within the buffer without
                    reallocation.
    arguments:    none
    return value: (SizeType) capacity of buffer in elements of BasisType
    --------------------------------------------------------------------------*/
    SizeType Capacity() const;

    /*!-------------------------------------------------------------------------
    function:     MaxCapacity()
    description:  Returns the maximum capacity of the buffer in elements of BasisType

                  - For dynamic buffer resource, this returns the maximum count
                    of elements that can be stored within the buffer w i t h
                    reallocation.
    arguments:    none
    return value: (SizeType) maximum capacity of buffer in elements of BasisType
    --------------------------------------------------------------------------*/
    SizeType MaxCapacity() const;

    /*!-------------------------------------------------------------------------
    function:     ElementCount()
    description:  Returns the actual count of elements in the buffer

    arguments:    none
    return value: (SizeType) actual count of elements in the buffer
    --------------------------------------------------------------------------*/
    SizeType ElementCount() const;

    /*!-------------------------------------------------------------------------
    function:     ElementCount()
    description:  Sets the count of elements in the buffer

                  - 'count' may not exceed the actual buffer capacity 
                  - Uninitialized part of buffer might contain hieroglyphs.
    arguments:    count [in]    count to be set
    return value: none
    --------------------------------------------------------------------------*/
    void SetElementCount(SizeType count);

    /*!-------------------------------------------------------------------------
    function:     UsedByteLength()
    description:  Returns the length in bytes of the used part of the buffer

                  - This is ElementCount() * sizeof(BasisType)
    arguments:    none
    return value: (SAPDB_UInt) length in bytes of the used part of the buffer
    --------------------------------------------------------------------------*/
    SAPDB_UInt UsedByteLength() const;

    /*!-------------------------------------------------------------------------
    function:     IsEmpty()
    description:  Checks whether buffer is empty
    arguments:    none
    return value: (SAPDB_Bool) true if empty
    --------------------------------------------------------------------------*/
    SAPDB_Bool IsEmpty() const;

    /*!-------------------------------------------------------------------------
    function:     IsFull()
    description:  Checks whether buffer is full
    arguments:    none
    return value: (SAPDB_Bool) true if full
    --------------------------------------------------------------------------*/
    SAPDB_Bool IsFull() const;

    /*--------------------------------------------------------------------------
    function:     Begin()
    description:  Returns an iterator which points to the beginning of 
                  the buffer
    arguments:    none
    return value: (ConstIterator / Iterator)
    prototypes:   2
    --------------------------------------------------------------------------*/
    ConstIterator Begin() const;
    Iterator      Begin();

    /*--------------------------------------------------------------------------
    function:     End
    description:  Returns an iterator which points behind the filled 
                  part of the buffer
    arguments:    none
    return value: (ConstIterator / Iterator)
    prototypes:   2
    --------------------------------------------------------------------------*/
    ConstIterator End() const;
    Iterator      End();

    /*!-------------------------------------------------------------------------
    function:     IsAssigned()
    description:  Checks whether buffer is assigned a resource
    arguments:    none
    return value: (SAPDB_Bool) true if assigned
    --------------------------------------------------------------------------*/
    SAPDB_Bool IsAssigned() const;

private:

    /*--------------------------------------------------------------------------
    function:     MemSet()
    description:  Sets 'count' elements after 'at' to 'val'
    
    arguments:    at     [in]  pointer to first element to be set
                  count  [in]  number of copies
                  val    [in]  source value
    return value: none
    --------------------------------------------------------------------------*/
    static void MemSet
        (Pointer                        at, 
         SizeType                       count, 
         BasisType                      val);
    
    /*--------------------------------------------------------------------------
    function:     PtrTo
    description:  Returns pointer to buffer element

                  - Returns a pointer to a specified element within the 
                    associated memory block.
                  - 'pos' may range from 0 to Capacity(). 
                  - 'pos' == Capacity() enables use of iterator End().
    arguments:    pos  [in]    element index [0..Capacity()]
    return value: (Pointer/ConstPointer) pointer to element
    prototypes:   2
    --------------------------------------------------------------------------*/
    Pointer PtrTo
        (IndexType                      pos);

    ConstPointer PtrTo
        (IndexType                      pos) const;

    /*--------------------------------------------------------------------------
    declaration : m_Data
    description:  Pointer to the memory block of the buffer
    --------------------------------------------------------------------------*/
    Pointer   m_Data;

    /*--------------------------------------------------------------------------
    declaration : m_Capacity
    description:  Capacity of the memory block in elements of BasisType
    --------------------------------------------------------------------------*/
    SizeType  m_Capacity;

    /*--------------------------------------------------------------------------
    declaration : m_ElemCount
    description:  Actual count of elements in the buffer (0..Capacity() !!)
    --------------------------------------------------------------------------*/
    IndexType m_ElemCount;

};
/*!*****************************************************************************

   endclass: Tools_DynamicUTF8StringBuffer

*******************************************************************************/


inline Tools_DynamicUTF8StringBuffer::Tools_DynamicUTF8StringBuffer()
    : 
    m_Data(0),
    m_Capacity(0),
    m_ElemCount(0)
    {}

//------------------------------------------------------------------------------

inline Tools_DynamicUTF8StringBuffer::~Tools_DynamicUTF8StringBuffer()
{
    delete m_Data;
}

//------------------------------------------------------------------------------

inline void Tools_DynamicUTF8StringBuffer::Deassign()
{
    delete m_Data;
    m_Data      = 0;
    m_Capacity  = 0;
    m_ElemCount = 0;
}

//------------------------------------------------------------------------------

inline void Tools_DynamicUTF8StringBuffer::Append
    (const void*                    src, 
     SizeType                       len) 
{
    ProvideCapacity(m_ElemCount + len);
    SAPDB_memcpy(End(), src, len * sizeof(BasisType));
    m_ElemCount += len;
}

//------------------------------------------------------------------------------

inline void Tools_DynamicUTF8StringBuffer::Append
    (BasisType                      val)
{
    ProvideCapacity(m_ElemCount + 1);
    *End() = val;
    ++m_ElemCount;
}

//------------------------------------------------------------------------------

inline void Tools_DynamicUTF8StringBuffer::Append
    (SizeType                       count,
     BasisType                      val)
{
    ProvideCapacity(m_ElemCount + count);
    MemSet(End(), count, val);
    m_ElemCount += count;
}

//------------------------------------------------------------------------------

inline void Tools_DynamicUTF8StringBuffer::Insert
    (IndexType                      pos,
     const void*                    src, 
     SizeType                       len) 
{
    assert(pos <= m_ElemCount);
    assert(src != 0);

    ProvideCapacity(m_ElemCount + len);

    SAPDB_memmove(PtrTo(pos + len), PtrTo(pos), (m_ElemCount - pos) * sizeof(BasisType));
    SAPDB_memcpy (PtrTo(pos), src, len * sizeof(BasisType));
    m_ElemCount += len;
}

//------------------------------------------------------------------------------

inline void Tools_DynamicUTF8StringBuffer::Insert
    (IndexType                      pos,
     SizeType                       count,
     BasisType                      val)
{
    assert(pos <= m_ElemCount);

    ProvideCapacity(m_ElemCount + count);
    SAPDB_memmove(PtrTo(pos + count), PtrTo(pos), (m_ElemCount - pos) * sizeof(BasisType));
    MemSet(PtrTo(pos), count, val);
    m_ElemCount += count;
}

//------------------------------------------------------------------------------

inline void Tools_DynamicUTF8StringBuffer::Erase
    (IndexType                      pos,
     SizeType                       len)
{
    assert(pos <= m_ElemCount);

    if (len > m_ElemCount - pos)
        m_ElemCount = pos;
    else
    {
        SAPDB_memmove(PtrTo(pos), PtrTo(pos + len), 
                (m_ElemCount - pos - len) * sizeof(BasisType));
        m_ElemCount -= len;
    }
}

//------------------------------------------------------------------------------

inline void Tools_DynamicUTF8StringBuffer::Copy
    (void*                          dest, 
     IndexType                      pos,
     SizeType                       len) const
{
    assert(dest != 0);
    assert(pos <= m_ElemCount);

    SAPDB_memcpy(dest, PtrTo(pos),
           pos + len > m_ElemCount ? (m_ElemCount - pos - len) * sizeof(BasisType) 
                                   : len * sizeof(BasisType));
}

//------------------------------------------------------------------------------

inline void Tools_DynamicUTF8StringBuffer::Replace
    (IndexType                      pos,
     SizeType                       len,
     const void*                    src,
     SizeType                       inslen)
{
    assert(pos <= m_ElemCount);
    assert(src != 0);

    if (len > m_ElemCount - pos)
        len = m_ElemCount - pos;

    ProvideCapacity((m_ElemCount - len) + inslen);

    if (inslen <= len)
    {
        SAPDB_memcpy(PtrTo(pos), src, inslen * sizeof(BasisType));
        SAPDB_memmove(PtrTo(pos + inslen), PtrTo(pos + len), 
                (m_ElemCount - pos - len) * sizeof(BasisType));
        m_ElemCount -= len - inslen;
    }
    else
    {
        SAPDB_memmove(PtrTo(pos + inslen), PtrTo(pos + len), 
                (m_ElemCount - pos - len) * sizeof(BasisType));
        SAPDB_memcpy(PtrTo(pos), src, inslen * sizeof(BasisType));
        m_ElemCount += inslen - len;
    }
}

//------------------------------------------------------------------------------

inline void Tools_DynamicUTF8StringBuffer::Replace
    (IndexType                      pos,
     SizeType                       len,
     SizeType                       count,
     BasisType                      val)
{
    assert(pos <= m_ElemCount);

    if (len > m_ElemCount - pos)
        len = m_ElemCount - pos;

    ProvideCapacity((m_ElemCount - len) + count);

    if (count <= len)
    {
        MemSet(PtrTo(pos), count, val);
        SAPDB_memmove(PtrTo(pos + count), PtrTo(pos + len), 
                (m_ElemCount - pos - len) * sizeof(BasisType));
        m_ElemCount -= len - count;
    }
    else
    {
        SAPDB_memmove(PtrTo(pos + count), PtrTo(pos + len), 
                (m_ElemCount - pos - len) * sizeof(BasisType));
        MemSet(PtrTo(pos), count, val);
        m_ElemCount += count - len;
    }
}

//------------------------------------------------------------------------------

inline void Tools_DynamicUTF8StringBuffer::Clear
    ()
{
    m_ElemCount = 0;
}

//------------------------------------------------------------------------------

inline SAPDB_Bool Tools_DynamicUTF8StringBuffer::ProvideCapacity
    (SizeType                       requested) 
{
    return ProvideCapacity(requested, m_ElemCount);
}

//------------------------------------------------------------------------------

inline SAPDB_Bool Tools_DynamicUTF8StringBuffer::ProvideCapacity
    (SizeType                       requested, 
     SizeType                       used)
{
    if (requested <= Capacity())                                                // do nothing if resource is big enough
        return true;

    SizeType allocsize = requested | 
                        (Tools_DynamicUTF8StringBuffer_ALLOCDELTA - 1);

    void *p = new BasisType[allocsize];                                         // try to reallocate memory

    if (p == 0) 
        return false;

    if (IsAssigned())
    {
        Pointer save = m_Data;                                                  // save old buffer contents
        SAPDB_memcpy(p, save, used * sizeof(BasisType));
        m_Data       = reinterpret_cast<Pointer>(p);
        m_Capacity   = allocsize;
        delete save;
    }
    else
    {
        m_Data       = reinterpret_cast<Pointer>(p);
        m_Capacity   = allocsize;
    }

    return true;
}

//------------------------------------------------------------------------------

inline Tools_DynamicUTF8StringBuffer::SizeType 
       Tools_DynamicUTF8StringBuffer::Capacity
    () const
{
    return m_Capacity;
}

//------------------------------------------------------------------------------

inline Tools_DynamicUTF8StringBuffer::SizeType 
       Tools_DynamicUTF8StringBuffer::MaxCapacity
    () const
{
    return SAPDB_MAX_UINT4;     // provisorisch
}

//------------------------------------------------------------------------------

inline Tools_DynamicUTF8StringBuffer::SizeType 
       Tools_DynamicUTF8StringBuffer::ElementCount
    () const
{
    assert(IsAssigned());

    return m_ElemCount;
}

//------------------------------------------------------------------------------

inline void Tools_DynamicUTF8StringBuffer::SetElementCount
    (SizeType                       count)
{
	if (count > 0) 
		assert(IsAssigned());
    assert(count <= Capacity());

    m_ElemCount = count;
}

//------------------------------------------------------------------------------

inline SAPDB_UInt Tools_DynamicUTF8StringBuffer::UsedByteLength
    () const
{
    assert(IsAssigned());

    return m_ElemCount * sizeof(BasisType);
}

//------------------------------------------------------------------------------

inline SAPDB_Bool Tools_DynamicUTF8StringBuffer::IsEmpty
    () const
{
    assert(IsAssigned());

    return m_ElemCount == 0;
}

//------------------------------------------------------------------------------

inline SAPDB_Bool Tools_DynamicUTF8StringBuffer::IsFull
    () const
{
    assert(IsAssigned());

    return m_ElemCount == m_Capacity;
}

//------------------------------------------------------------------------------

inline Tools_DynamicUTF8StringBuffer::ConstIterator 
       Tools_DynamicUTF8StringBuffer::Begin
    () const
{
    assert(IsAssigned());

    return m_Data;
}

//------------------------------------------------------------------------------

inline Tools_DynamicUTF8StringBuffer::Iterator 
       Tools_DynamicUTF8StringBuffer::Begin
    ()
{
    assert(IsAssigned());

    return m_Data;
}

//------------------------------------------------------------------------------

inline Tools_DynamicUTF8StringBuffer::ConstIterator 
       Tools_DynamicUTF8StringBuffer::End
    () const
{
    assert(IsAssigned());

    return PtrTo(m_ElemCount);
}

//------------------------------------------------------------------------------

inline Tools_DynamicUTF8StringBuffer::Iterator 
       Tools_DynamicUTF8StringBuffer::End
    ()
{
    assert(IsAssigned());

    return PtrTo(m_ElemCount);
}

//------------------------------------------------------------------------------

inline SAPDB_Bool Tools_DynamicUTF8StringBuffer::IsAssigned() const
{
    return m_Data != 0;
}

//------------------------------------------------------------------------------

inline void Tools_DynamicUTF8StringBuffer::MemSet
    (Pointer                        at, 
     SizeType                       count, 
     BasisType                      val)
{
    while (count > 0)
    {
        *at = val;
        ++at;
        --count;
    }
}

//------------------------------------------------------------------------------

inline Tools_DynamicUTF8StringBuffer::Pointer 
       Tools_DynamicUTF8StringBuffer::PtrTo
    (IndexType                      pos)
{
    assert(IsAssigned());
    assert(pos <= m_Capacity);

    return m_Data + pos;
}

//------------------------------------------------------------------------------

inline Tools_DynamicUTF8StringBuffer::ConstPointer 
       Tools_DynamicUTF8StringBuffer::PtrTo
    (IndexType                      pos) const
{
    assert(IsAssigned());
    assert(pos <= m_Capacity);

    return m_Data + pos;
}

//------------------------------------------------------------------------------


#endif // TOOLS_DYNAMICUTF8STRINGBUFFER_HPP
