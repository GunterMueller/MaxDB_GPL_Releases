/*!***************************************************************************
  module:       SAPDBMem_CallDirector.cpp

  responsible:  ThomasA

  special area: Memory management

  description:  directory of call stacks of memory requestors.

  copyright:    Copyright (c) 1999-2005 SAP AG

  created:      Wed Jan 12 14:35:56  2000

  updated:      Tue Feb  8 10:49:05  2000



    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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




*****************************************************************************/

#include <string.h>
#include <stdlib.h>

#include "SAPDBCommon/MemoryManagement/SAPDBMem_CallDirectory.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "RunTime/RTE_CompilerFeatures.h"

#include "heo670.h"

SAPDBMem_CallDirectory* SAPDBMem_CallDirectory::m_pInstance = NULL;

/*-----------------------------------------------------------*/

SAPDBMem_CallDirectory::SAPDBMem_CallDirectory()
: m_symbOffset(-1)
, m_callIdGenerator(1)
{
  for (int slot = 0; slot < HEADER_SIZE; ++slot)
  {
    m_head[slot] = NULL;
  }
}

/*-----------------------------------------------------------*/

SAPDBMem_CallDirectory::Iterator SAPDBMem_CallDirectory::Begin() const
{
  return Iterator();
}

/*-----------------------------------------------------------*/

SAPDB_UInt4 SAPDBMem_CallDirectory::GetCallId (int stackLength, SAPDBMem_CallDirectory::VoidPtr* pStack)
{
#if defined(BIT64)
  SAPDB_UInt8  hashValue = 0;
  SAPDB_UInt8* pInt = REINTERPRET_CAST(SAPDB_UInt8*, pStack);
#else
  SAPDB_UInt4  hashValue = 0;
  SAPDB_UInt4* pInt = REINTERPRET_CAST(SAPDB_UInt4*, pStack);
#endif
  if (-1 == m_symbOffset)
  {
    void* pContext;
    if (eo670_GetAddressInfoProlog(&pContext))
    {
      // first insert into call directory, find out all entries called by 
      // SAPDBMem_BlockAllocatorBasedDLAllocator. These should not be monitored.
      // Store the offset to first occurence of SAPDBMem_BlockAllocatorBasedDLAllocator
      // im m_symbOffset
      VoidPtr*         p        = pStack;
      eo670AddressInfo addressInfo;
      int              offset = 0;
      for (int stackIdx = 0; (stackIdx < stackLength) && (-1 == m_symbOffset); ++stackIdx)
      {
        ++offset;
        if (eo670_GetAddressInfo(*p, &addressInfo, pContext))
        {
          if (NULL != strstr (addressInfo.demangledSymbol, "SAPDBMem_RawAllocator"))
          {
            m_symbOffset = offset; 
          }
          eo670_FreeAddressInfo(&addressInfo);
          ++p;
        }
      }
      eo670_GetAddressInfoEpilog(pContext);
      if (-1 == m_symbOffset)
      {
        m_symbOffset = 0;
      }
    }
  }
  stackLength -= m_symbOffset;
  pStack      += m_symbOffset;
  // find out, if current call stack is already stored in directory
  for (int ix = 0; ix < stackLength; ++ix)
  {
    hashValue += *pInt;
    ++pInt;
  }
  int hashIndex = (int) (hashValue % HEADER_SIZE);
  SAPDB_UInt4 callId = FindCallId (hashIndex, stackLength, pStack);
  if (0 == callId)
  {
    callId = this->Insert (hashIndex, callId, stackLength, pStack);
  }
  return callId;
}

/*-----------------------------------------------------------*/

SAPDB_UInt4 SAPDBMem_CallDirectory::Insert (int hashIndex, SAPDB_UInt4 callId, int stackLength, void* pStack)
{
  RTESync_LockedScope scope(m_slotLock[hashIndex]);
  /* search again, call stack may have been inserted already by concurrent task */ 
  callId = FindCallId (hashIndex, stackLength, pStack);
  if (0 == callId)
  {
    m_callIdGenerator.Increment(1, &callId);
    CallStackDescriptorPtr p = new (stackLength) CallStackDescriptor(callId, stackLength, pStack, m_head[hashIndex]);
    if (NULL != p)
    {
      m_head[hashIndex] = p;
    }
    else
    {
      callId = 0;
    }
  }
  return callId;
}

/*-----------------------------------------------------------*/

SAPDBMem_CallDirectory& SAPDBMem_CallDirectory::Instance()
{
  SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(SAPDBMem_CallDirectory));
  if (NULL == m_pInstance)
  {
    m_pInstance = new(Space) SAPDBMem_CallDirectory();
  }
  return *m_pInstance;
}

/*-----------------------------------------------------------*/

#define OFFSET(T,M) (((char*) &((T*) 0)->M) - (char*) 0)

SAPDBMem_CallDirectory::CallStackDescriptor::CallStackDescriptor
(
 SAPDB_UInt4          callId,
 int                  stackLength, 
 void*                pStack,
 CallStackDescriptor* next
 )
 : m_next(next)
 , m_stackLength(stackLength)
 , m_callId(callId)
{
  memcpy (&m_stack[0], pStack, stackLength * sizeof(void*)); 
}

/*-----------------------------------------------------------*/

void* SAPDBMem_CallDirectory::CallStackDescriptor::operator new (size_t sz, int stackLength)
{
  return malloc(OFFSET(CallStackDescriptor, m_stack) + stackLength * sizeof(void*)); 
}

/*-----------------------------------------------------------*/

void  SAPDBMem_CallDirectory::CallStackDescriptor::operator delete (void* p, int stackLength)
{
  // call stack info is never removed
}

/*-----------------------------------------------------------*/

SAPDBMem_CallDirectory::Iterator::Iterator()
: m_callStackDirectory(SAPDBMem_CallDirectory::Instance())
, m_headIndex(-1)
, m_curr(NULL)

{
  this->operator++();
}

/*-----------------------------------------------------------*/

SAPDBMem_CallDirectory::Iterator::operator bool() const
{
  return (m_curr != NULL);
}

/*-----------------------------------------------------------*/

void SAPDBMem_CallDirectory::Iterator::operator++()
{
  if (NULL != m_curr)
  {
    m_curr = m_curr->m_next;
  }
  if (NULL == m_curr)
  {
    ++m_headIndex;
    while (m_headIndex < HEADER_SIZE)
    {
      if (NULL == m_callStackDirectory.m_head[m_headIndex])
      {
        ++m_headIndex;
      }
      else
      {
        m_curr = m_callStackDirectory.m_head[m_headIndex];
        return;
      }
    }
  }
}

/*-----------------------------------------------------------*/

void SAPDBMem_CallDirectory::Iterator::Current
(
 int&    callStackId,
 int&    stackLength, 
 void*** pStack) const
{
  callStackId = m_curr->m_callId;
  stackLength = m_curr->m_stackLength;
  *pStack     = &m_curr->m_stack[0];
}