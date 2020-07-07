/*!
  @file           SAPDBMem_CallDirectory.hpp
  @author         ThomasA
  @ingroup        Memory management
  @brief          directory of call stacks of memory requestors

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


#ifndef SAPDBMEM_CALLDIRECTORY_HPP
#define SAPDBMEM_CALLDIRECTORY_HPP

#include <string.h>
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "RunTime/Synchronisation/RTESync_InterlockedCounter.hpp"
#include "RunTime/Synchronisation/RTESync_Spinlock.hpp"

/*!
  @class     SAPDBMem_CallDirectory
  @brief     this class realizes a singleton, that stores call stacks
             given by an array of pointers. For each call stack a
             unique integer is assigned, which serves a surrogate for
             the call stack. 
             If call stack monitoring of the global allocator is enabled,
             each memory chunk contains the call stack surrogate of the
             memory requestor. Together with an iterator over the allocator
             and over this call stack directory, this allows a better
             analysis of memory leak problems.
             The system table sysdd.heap_content yields the corresponding information

 */


class SAPDBMem_CallDirectory
{
  struct CallStackDescriptor;
public :
  class Iterator;

  friend class SAPDBMem_CallDirectory::Iterator;

  typedef void* VoidPtr; //!< pointer to anything
  /*!
     @class          Iterator
     @brief          defines an iterator over call directoy.

   */

  class Iterator {
    friend class SAPDBMem_CallDirectory;
  public :
    operator bool() const; //!< true if iterator didn't reached the end
    void operator++();     //!< goes to next call directory entry
    void Current(int& callStackId, int& stackLength, void*** pStack) const; //!< returns current 
  private :
    Iterator(); //!< constructor
    SAPDBMem_CallDirectory& m_callStackDirectory; //!< reference to directory singleton
    int                     m_headIndex;          //!< current hash slot 
    CallStackDescriptor*    m_curr;               //!< current   
  };


  /*!
     @brief          returns an iterator over the call stack directory
     @return         call stack iterator

   */

  Iterator Begin() const; 

  /*!
     @brief          returns a reference to the call director instance, If the 
                     does not already exist, the singleton in created impilcitly.
     @return         reference to the call directory instance

   */


  static SAPDBMem_CallDirectory& Instance();

  /*!
     @brief          returns a surrogate (4 Bytes integer) for a given call stack
                     identified by an array of pointers.
                     The method checks, if the call stack is already stored in the
                     directory. If true, the corresponding surrogate is returned.
                     Otherwise a new surrogate is generated and the given call
                     stack is interted into the directory.
     @param          stackLength [in] number of pointers stored in pStack
     @param          pStack [in] pointer array of PC adresses.
     @return         surrogate for the given call stack

   */


  SAPDB_UInt4 GetCallId (int stackLength, VoidPtr* pStack);

  /*!
     @brief          return the maximal call stack surrogate
     @return         the maximal call stack surrogate

   */


  inline SAPDB_UInt4 GetMaxCallStackId() const;
  
private :
  
  enum { HEADER_SIZE = 217 };

  /*!
     @brief description of a call stack
  */

  struct CallStackDescriptor
  {
    CallStackDescriptor* m_next;          //!< pointer to next description
    SAPDB_Int4           m_stackLength;   //!< length of the current call stack
    SAPDB_UInt4          m_callId;        //!< identification of the call stack
    void*                m_stack[1024];   //!< the call stack
    /*!
       @brief constructor
    */
    CallStackDescriptor(SAPDB_UInt4          callId,
                        int                  stackLength, 
                        void*                pStack,
                        CallStackDescriptor* next);

    /*!
       @ brief operator new for CallStackDescriptor
    */

    void* operator new(size_t sz, int stackLength);

    /*!
       @ brief operator new for CallStackDescriptor
    */

    void  operator delete (void* p, int stackLength);
  };

  /*!
     @brief constructor
  */

  SAPDBMem_CallDirectory();

  /*!
     @brief returns the identification of a call stack
  */

  inline SAPDB_UInt4 FindCallId (int hashIndex, int stackLength, void* pStack);

  /*!
     @brief inserts a call stack into the directory
  */
  SAPDB_UInt4 Insert (int hashIndex, SAPDB_UInt4 callId, int stackLength, void* pStack);

  typedef CallStackDescriptor* CallStackDescriptorPtr; //!< defined pointer to a call stack descriptor

  static SAPDBMem_CallDirectory*          m_pInstance;      //!< the directory instance         
  int                                     m_symbOffset;     //!< number of ignored call stack entries 
  RTESync_InterlockedCounter<SAPDB_UInt4> m_callIdGenerator;//!< generator for call stack identifications
  RTESync_Spinlock                        m_slotLock[HEADER_SIZE]; //!< sychronisation of hash slots
  volatile CallStackDescriptorPtr         m_head[HEADER_SIZE];     //!< hash slots
}; 

/*-------------------------------------------------------------------*/

inline SAPDB_UInt4 SAPDBMem_CallDirectory::FindCallId (int hashIndex, int stackLength, void* pStack)
{
  CallStackDescriptor* pCurr = m_head[hashIndex];
  while (NULL != pCurr)
  {
    if (stackLength == pCurr->m_stackLength)
    {
      if (0 == memcmp (pStack, &pCurr->m_stack[0], stackLength * sizeof(void*)))
      {
        return pCurr->m_callId;
      }
    }
    pCurr = pCurr->m_next;
  }
  return 0;
}

/*-------------------------------------------------------------------*/

inline SAPDB_UInt4 SAPDBMem_CallDirectory::GetMaxCallStackId() const
{
  return m_callIdGenerator;
}

#endif
