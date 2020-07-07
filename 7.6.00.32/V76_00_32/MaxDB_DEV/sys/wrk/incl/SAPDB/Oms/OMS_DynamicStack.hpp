/*!
 * \file    OMS_DynamicStack.hpp
 * \author  MarkusSi
 * \brief   implementation of a dynamic growing stack
 */

/*
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

*/


#ifndef __OMS_DYNAMICSTACK_HPP
#define __OMS_DYNAMICSTACK_HPP

/*! This class implements a dynamic growing stack, which means, that an arbitray
**  number of entries can be inserted (Push) and it is only possible to read the  
**  object which was inserted last from the stack (Pop). 
**  All operations besides (Clear) have a constant time complexity.
**  The stack is implemented using a dynamic chain of arrays. Every array can
**  store the specified number of entries and additionally a pointer to the next
**  array. New arrays are allocated on demand and if they are not needed any more,
**  they are chained in a free-list for later reuse. An array is only deallocated
**  if clear is called.
**  \since 1127661
*/
template<class T, class Alloc>
class OMS_DynamicStack
{
public:
  /*! Default constructor */
  OMS_DynamicStack() 
    : m_pAlloc(NULL), 
      m_entriesPerChunk(0), 
      m_pEntries(NULL), 
      m_pFreeList(NULL), 
      m_curr(-1),
      m_chunkSize(0)
  {}

  /*! Constructor */
  OMS_DynamicStack(Alloc *pAlloc, unsigned int entriesPerChunk = 100)
    : m_pAlloc(pAlloc), 
      m_entriesPerChunk(entriesPerChunk), 
      m_pEntries(NULL), 
      m_pFreeList(NULL), 
      m_curr(-1),
      m_chunkSize(0)
  { m_chunkSize = ComputeChunkSize(m_entriesPerChunk); }

  /*! Destructor: All allocated memory is freed */
  ~OMS_DynamicStack()
  { Delete(); }

  /*! This method can be used to specify the allocator which should be
  **  used and furthermore the number of entries, which should be stored
  **  in a single array can be set. 
  **  \param pAlloc Pointer to the allocator
  **  \param entriesPerChunk number of entries, which should be stored in 
  **     an array. The number can only be changes if no array have been 
  **     allocated yet. If entriesPerChunk is set to 0, then the data-structure
  **     is 'inactivated', which meanse that Push and Pop still can be called, 
  **     but Pop will always return NULL, even if a Push has been executed before.
  */
  inline void Advise(Alloc *pAlloc, unsigned int entriesPerChunk = 100); 

  /*! This method removes all entries out of the stack, but keep the allocated
  **  memory. A change of the number of entriesPerChunk is therefore not possible.
  */
  inline void Clear();

  /*! This method releases all allocated memory and reset the data-structure.
  **  After this method has been executed, the number of entriesPerChunk can
  **  be changed again.
  */
  inline void Delete();

  /*! Return whether the data-structure is empty or not.
  **  \return true, if there is at least one entry remaining in the data-structure. 
  */
  inline bool IsEmpty() const;

  /*! Inserts the given entry into the data-structure.
  **  \param entry Entry which should be inserted
  */
  inline void Push(const T &entry);

  /*! Returns a pointer to the entry which was inserted last into the data-structure.
  **  \return Pointer to the last inserted entry. If there is no entry left, then 
  **    Null is returned.
  */
  inline const T* Pop();

private:
  /*! Compute the size of the array which is needed to store entriesPerChunk many entries.
  **  \param entriesPerChunk number of entries which can be stored in a single array
  */
  inline unsigned int ComputeChunkSize(unsigned int entriesPerChunk) const;

  T            *m_pEntries;
  T            *m_pFreeList;
  Alloc        *m_pAlloc;
  int           m_curr;
  unsigned int  m_entriesPerChunk;
  unsigned int  m_chunkSize;
};

//------------------------------------------------------------------------

template<class T, class Alloc>
inline unsigned int OMS_DynamicStack<T,Alloc>::ComputeChunkSize(unsigned int entriesPerChunk) const
{
  if (entriesPerChunk == 0)
    return 0;

  // Size for storing m_entriesPerChunk entries and a pointer to the next array.
  // As the size of the entries might be smaller than a pointer it must be ensured,
  // that the position, where the pointer is stored, is aligned. 
  unsigned int unalignedSize = m_entriesPerChunk*sizeof(T)+sizeof(void*);
  return (unalignedSize + sizeof(void*)-1) & ~(sizeof(void*)-1);
}

//------------------------------------------------------------------------

template<class T, class Alloc>
inline bool OMS_DynamicStack<T,Alloc>::IsEmpty() const 
{
  return (m_curr == -1 || m_entriesPerChunk == 0);
}

//------------------------------------------------------------------------

template<class T, class Alloc>
inline void OMS_DynamicStack<T,Alloc>::Advise(Alloc *pAlloc, unsigned int entriesPerChunk)
{ 
  if (m_pEntries != 0 || m_pFreeList != NULL){
    // Change of chunksize is only possible, if no chunk has been allocated yet
    throw; // TODO
  }

  m_pAlloc          = pAlloc; 
  m_entriesPerChunk = entriesPerChunk;
  m_chunkSize       = ComputeChunkSize(m_entriesPerChunk);
}

//------------------------------------------------------------------------

template<class T, class Alloc>
inline void OMS_DynamicStack<T,Alloc>::Delete()
{
  // Clear entry-list
  T* tmp = m_pEntries;
  while (m_pEntries){
    tmp = *reinterpret_cast<T**>(reinterpret_cast<char*>(m_pEntries)+m_chunkSize-sizeof(void*));
    m_pAlloc->deallocate(m_pEntries);
    m_pEntries = tmp;
  }
  // Clear freelist
  tmp = m_pFreeList;
  while (m_pFreeList){
    tmp = *reinterpret_cast<T**>(reinterpret_cast<char*>(m_pFreeList)+m_chunkSize-sizeof(void*));
    m_pAlloc->deallocate(m_pFreeList);
    m_pFreeList = tmp;
  } 
  m_curr = -1;
}

//------------------------------------------------------------------------

template<class T, class Alloc>
inline void OMS_DynamicStack<T,Alloc>::Clear()
{
  T* tmp = m_pEntries;
  while (m_pEntries){
    // Append entries to freelist
    tmp = *reinterpret_cast<T**>(reinterpret_cast<char*>(m_pEntries)+m_chunkSize-sizeof(void*));
    *reinterpret_cast<T**>(reinterpret_cast<char*>(m_pEntries)+m_chunkSize-sizeof(void*)) = m_pFreeList;
    m_pFreeList = m_pEntries;
    m_pEntries  = tmp;
  }
  m_curr = -1;
}

//------------------------------------------------------------------------

template<class T, class Alloc>
inline void OMS_DynamicStack<T,Alloc>::Push(const T &entry)
{
  if (m_entriesPerChunk == 0){
    // Stack is deactivated
    return;
  }

  ++m_curr;
  if (m_curr == m_entriesPerChunk || m_pEntries == NULL){
    T* pNewChunk = NULL;
    if (m_pFreeList == NULL){
      // Allocate new chunk for storing the oids
      pNewChunk = (T*) m_pAlloc->allocate(m_chunkSize);
    }
    else {
      // Unchain first chunk of freelist
      pNewChunk = m_pFreeList;
      m_pFreeList = *reinterpret_cast<T**>(reinterpret_cast<char*>(m_pFreeList)+m_chunkSize-sizeof(void*));    
    }

    // Insert new chunk at the head position in the list 
    *reinterpret_cast<T**>(reinterpret_cast<char*>(pNewChunk)+m_chunkSize-sizeof(void*))
      = m_pEntries;
    m_pEntries = pNewChunk;

    m_curr     = 0;
  }
  
  m_pEntries[m_curr] = entry;
}

//------------------------------------------------------------------------

template<class T, class Alloc>
inline const T* OMS_DynamicStack<T,Alloc>::Pop()
{
  if (m_pEntries == NULL || m_curr == -1){
    return NULL;
  }

  T** ppNext = reinterpret_cast<T**>(reinterpret_cast<char*>(m_pEntries)+m_chunkSize-sizeof(void*));

  if (m_curr > 0 || (m_curr == 0 && *ppNext == NULL)){
    // m_curr > 0:  Reposition index in the same chunk
    // m_curr == 0: This is the last element in the complete data-structure (there is
    //              no continuation array), therefore set the index to -1 and keep the 
    //              chunk in the m_pEntries-list so that a possible next insert can
    //              be performed quickly.
    return &m_pEntries[m_curr--];
  }
  else {
    // A chunk is completely empty and there is another chunk which contains further
    // entries. Therefore more the empty chunk to the freelist and set the index to
    // the highest entry in the previous chunk.

    // Remember pointer to next chunk
    T *tmp = *ppNext; 
 
    // Insert empty chunk at the head position of the freelist 
    *ppNext     = m_pFreeList;
    m_pFreeList = m_pEntries;

    // Set starting point of the list to the remembered next chunk
    m_pEntries = tmp;

    m_curr = m_entriesPerChunk-1;

    // As chunk has been moved to freelist, return first element 
    // of first chunk in freelist
    return &m_pFreeList[0];  
  }
}

//------------------------------------------------------------------------

#endif    // __OMS_DYNAMICSTACK_HPP
