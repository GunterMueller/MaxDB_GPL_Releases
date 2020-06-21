/*



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





*/
#if defined(_WIN32)

/****************************************************************************

  module      : RTE_SystemNT.cpp

  -------------------------------------------------------------------------

  responsible : JoergM/RobinW

  special area: System Interface
  description : Interface class to hide system dependencys from RTE_Global class

  last changed: 2000-10-23  10:50
  see also    : example.html ...
 first created: 2000-02-28  15:58

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG

*****************************************************************************/

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include <windows.h>
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "RunTime/RTE_ISystem.hpp"
#include "RunTime/System/RTESys_Spinlock.h"
#include "RunTime/System/RTESys_SystemInfo.h"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "RunTime/MemoryManagement/RTEMem_SystemPageCache.hpp"
#include "RunTime/MemoryManagement/RTEMem_PseudoStackAllocator.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

class RTE_SystemNT : public RTE_ISystem
{
public:

  virtual const SAPDB_ULong GetSystemPageSize() const;

  virtual const SAPDB_ULong RoundUpToSystemPageSize(SAPDB_ULong numberOfBytes) const;

  virtual const SAPDB_ULong GetPhysicalMemorySize() const;

  virtual RTE_SystemRc MemProtect(void *pvAdr,SAPDB_ULong ulSize,SAPDB_Int type);

  virtual const SAPDB_UInt4 GetNumberOfCPU() const;

  virtual void * AllocSystemPages(SAPDB_ULong SizeInBytes, bool DoCommit=true);

  virtual void * AllocSystemPages(SAPDB_ULong SizeInBytes, bool DoCommit, Msg_List &messageList);

  virtual void * UncachedAllocSystemPagesAtFixedAddress(void *FixedAddr, SAPDB_ULong SizeInBytes, bool DoCommit=true);

  virtual void * UncachedAllocSystemPagesAtFixedAddress(void *FixedAddr, SAPDB_ULong SizeInBytes, bool DoCommit, Msg_List &messageList);

  virtual void * ReserveSystemPages(SAPDB_ULong SizeInBytes);

  virtual void * AllocSystemPagesAtFixedAddress(void *FixedAddress, SAPDB_ULong SizeInBytes, bool DoCommit=true);

  virtual void * AllocSystemPagesAtFixedAddress(void *FixedAddress, SAPDB_ULong SizeInBytes, bool DoCommit, Msg_List &messageList);

  virtual void * CommitSystemPages(void *FirstPagePointer, SAPDB_ULong NumberOfBytes);

  virtual bool   FreeSystemPages(void *FirstPagePointer, SAPDB_ULong NumberOfBytes);

  virtual bool   UncachedFreeSystemPages(void *FirstPagePointer, SAPDB_ULong NumberOfBytes);

  virtual SAPDB_ULong SetMemoryLimitInBytes(SAPDB_ULong NewMemoryLimit, SAPDB_ULong & OldMemoryLimit);

  virtual void   GetMemoryStatistics( SAPDB_ULong & MemoryLimitInBytes,
                                   SAPDB_ULong & MemoryCommitedInBytes,
                                   SAPDB_ULong & MemoryUsedInBytes,
                                   SAPDB_ULong & MemoryMaxUsedInBytes,
                                   SAPDB_UInt8 & MemorySuccessfullAllocs,
                                   SAPDB_UInt8 & MemoryFailedAllocs,
                                   SAPDB_UInt8 & MemoryCommits,
                                   SAPDB_UInt8 & MemoryFrees);

  virtual SAPDB_Char const * GetLocalNodeName();

  virtual LPVOID AllocStack(LPVOID startAddress, SAPDB_ULong stackSize, SAPDB_ULong guardSize, SAPDB_Bool doCommit);
  virtual SAPDB_Bool FreeStack(LPVOID startAddress, SAPDB_ULong stackSize, SAPDB_ULong guardSize);
  virtual SAPDB_Bool PseudoAllocStack(SAPDB_ULong stackSize, SAPDB_ULong guardSize);
  virtual void PseudoFreeStack(SAPDB_ULong stackSize, SAPDB_ULong guardSize);

  static RTE_SystemNT * m_Instance; // The singleton instance
private:

  RTE_SystemNT(SAPDB_ULong MemoryLimitInBytes);


  SAPDB_ULong m_PhysicalMemorySize;

  SYSTEM_INFO m_TheSystem;     // Cashed system information

  SAPDB_ULong m_MemoryLimitInBytes;
  SAPDB_ULong m_MemoryCommitedInBytes;
  SAPDB_ULong m_MemoryUsedInBytes;
  SAPDB_ULong m_MemoryMaxUsedInBytes;
  SAPDB_UInt8 m_SuccessfullAllocCalls;
  SAPDB_UInt8 m_FailedAllocCalls;
  SAPDB_UInt8 m_CommitCalls;
  SAPDB_UInt8 m_FreeCalls;
  RTE_Lock m_MemoryUsageLock;  // Used for locking memory usage counter
  SAPDB_Char const * m_LocalNodeName;

  void IncrementCallCounter(SAPDB_UInt8 &Counter);
  bool IncrementUsedMemory(SAPDB_ULong SizeInBytes);
  void IncrementCommitedMemory(SAPDB_ULong SizeInBytes);
  void DecrementUsedMemory(SAPDB_ULong SizeInBytes);

  friend class RTE_IGlobal;
  friend class RTE_ISystem;
};

RTE_SystemNT * RTE_SystemNT::m_Instance = NULL;
const SAPDB_ULong RTE_ISystem::UnlimitedMemory = 0;
const RTE_SystemRc RTE_ISystem::NoError = NO_ERROR;

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/


/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/
//
// This is a static member function of RTE_ISystem
// It is implemented here, because it is system dependend
//
void RTE_ISystem::Initialize(SAPDB_ULong MemoryLimitInBytes)
{
  SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(RTE_SystemNT));

  if ( !RTE_SystemNT::m_Instance )
  {
    RTE_SystemNT::m_Instance = new (Space) RTE_SystemNT(MemoryLimitInBytes);
  }
}

/*---------------------------------------------------------------------------*/
RTE_ISystem & RTE_ISystem::Instance()
{
  RTE_ISystem::Initialize(RTE_ISystem::UnlimitedMemory);

  SAPDBERR_ASSERT_STATE(RTE_SystemNT::m_Instance);

  return *RTE_SystemNT::m_Instance;
}

/*---------------------------------------------------------------------------*/
//
// Do all System specific initialization
//
RTE_SystemNT::RTE_SystemNT(SAPDB_ULong MemoryLimitInBytes)
: m_MemoryLimitInBytes(MemoryLimitInBytes),
  m_MemoryCommitedInBytes(0),
  m_MemoryUsedInBytes(0),
  m_MemoryMaxUsedInBytes(0),
  m_SuccessfullAllocCalls(0),
  m_FailedAllocCalls(0),
  m_CommitCalls(0),
  m_FreeCalls(0),
  m_LocalNodeName(0)
{
  GetSystemInfo(&m_TheSystem);  // Cache System Info (i.e. for GetSystemPageSize)
  
  m_PhysicalMemorySize = RTESys_PhysicalMemorySize();

  InitLock(m_MemoryUsageLock);
}

/*---------------------------------------------------------------------------*/

void   RTE_ISystem::GiveUpTimeSlice()
{
    RTESys_GiveUpTimeSlice();
}

/*---------------------------------------------------------------------------*/

const SAPDB_ULong RTE_SystemNT::GetSystemPageSize() const
{
  return (const SAPDB_ULong)m_TheSystem.dwPageSize;
}

/*---------------------------------------------------------------------------*/

const SAPDB_ULong RTE_SystemNT::RoundUpToSystemPageSize(SAPDB_ULong numberOfBytes) const
{
    return ( ( numberOfBytes + m_TheSystem.dwPageSize - 1 ) & ~( (SAPDB_ULong)m_TheSystem.dwPageSize - 1 ) );
}

/*---------------------------------------------------------------------------*/

RTE_SystemRc RTE_SystemNT::MemProtect(void *pvAdr,SAPDB_ULong ulSize,SAPDB_Int type)
{
    return (RTE_SystemRc)RTESys_MemProtect(pvAdr, ulSize, type);
}

/*---------------------------------------------------------------------------*/

const SAPDB_ULong RTE_SystemNT::GetPhysicalMemorySize() const
{
   return m_PhysicalMemorySize;
}

/*---------------------------------------------------------------------------*/

const SAPDB_UInt4 RTE_SystemNT::GetNumberOfCPU() const
{
  return (const SAPDB_UInt4)m_TheSystem.dwNumberOfProcessors;
}

/*---------------------------------------------------------------------------*/

void   RTE_SystemNT::IncrementCallCounter(SAPDB_UInt8 &Counter)
{
  while ( RTE_ISystem::TestAndLock(m_MemoryUsageLock) )
  {
    RTE_ISystem::GiveUpTimeSlice();
  }

  Counter++;

  RTE_ISystem::Unlock(m_MemoryUsageLock);
}

/*---------------------------------------------------------------------------*/

LPVOID RTE_SystemNT::AllocStack(LPVOID startAddress, SAPDB_ULong stackSize, SAPDB_ULong guardSize, SAPDB_Bool doCommit)
{
    LPVOID result = 0;
    SAPDB_ULong SystemSize = (stackSize + GetSystemPageSize() - 1) & (~(GetSystemPageSize()-1));
    bool canAllocate = IncrementUsedMemory(SystemSize);
    if ( !canAllocate )
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return 0;
    }

    result = VirtualAlloc( startAddress
                         , stackSize
                         , doCommit ? MEM_COMMIT : MEM_RESERVE
                         , PAGE_READWRITE );
    if ( !result )
    {
        IncrementCallCounter(m_FailedAllocCalls);
        DecrementUsedMemory(SystemSize);
        return 0;
    }

    IncrementCallCounter(m_SuccessfullAllocCalls);
    RTEMem_PseudoStackAllocator::Instance().AddBaseChunk(SystemSize);
    RTEMem_PseudoStackAllocator::Instance().AddChunk(SystemSize-guardSize);
    return result;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTE_SystemNT::FreeStack(LPVOID startAddress, SAPDB_ULong stackSize, SAPDB_ULong guardSize)
{
    IncrementCallCounter(m_FreeCalls);

    SAPDB_ULong SystemSize = (stackSize + GetSystemPageSize() - 1) & (~(GetSystemPageSize()-1));
    SAPDB_Bool result = (0 != VirtualFree( startAddress, stackSize, MEM_DECOMMIT ));
    if ( result )
    {
        DecrementUsedMemory(SystemSize);
        RTEMem_PseudoStackAllocator::Instance().ReleaseChunk(SystemSize-guardSize);
        RTEMem_PseudoStackAllocator::Instance().ReleaseBaseChunk(SystemSize);
    }

    return result;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTE_SystemNT::PseudoAllocStack(SAPDB_ULong stackSize, SAPDB_ULong guardSize)
{
    SAPDB_ULong SystemSize = (stackSize + GetSystemPageSize() - 1) & (~(GetSystemPageSize()-1));
    bool canAllocate = IncrementUsedMemory(SystemSize);
    if ( !canAllocate )
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return false;
    }

    IncrementCallCounter(m_SuccessfullAllocCalls);
    RTEMem_PseudoStackAllocator::Instance().AddBaseChunk(SystemSize);
    RTEMem_PseudoStackAllocator::Instance().AddChunk(SystemSize-guardSize);
    return true;
}

/*---------------------------------------------------------------------------*/

void RTE_SystemNT::PseudoFreeStack(SAPDB_ULong stackSize, SAPDB_ULong guardSize)
{
    IncrementCallCounter(m_FreeCalls);

    SAPDB_ULong SystemSize = (stackSize + GetSystemPageSize() - 1) & (~(GetSystemPageSize()-1));
    RTEMem_PseudoStackAllocator::Instance().ReleaseChunk(SystemSize-guardSize);
    RTEMem_PseudoStackAllocator::Instance().ReleaseBaseChunk(SystemSize);
    DecrementUsedMemory(SystemSize);
}

/*---------------------------------------------------------------------------*/

bool   RTE_SystemNT::IncrementUsedMemory(SAPDB_ULong SizeInBytes)
{
  bool CanAllocate = true;

  while ( RTE_ISystem::TestAndLock(m_MemoryUsageLock) )
  {
    RTE_ISystem::GiveUpTimeSlice();
  }

  if ( 0 < m_MemoryLimitInBytes )
  {
    if ( (m_MemoryUsedInBytes + SizeInBytes) > m_MemoryLimitInBytes
      || (m_MemoryUsedInBytes + SizeInBytes) < m_MemoryUsedInBytes ) // Overflow check was missing...
    {
      CanAllocate = false;
    }
  }

  if ( CanAllocate )
  {
    m_MemoryUsedInBytes += SizeInBytes;
    if (m_MemoryUsedInBytes > m_MemoryMaxUsedInBytes)
        m_MemoryMaxUsedInBytes = m_MemoryUsedInBytes;
  }

  RTE_ISystem::Unlock(m_MemoryUsageLock);

  return CanAllocate;
}

/*---------------------------------------------------------------------------*/

void RTE_SystemNT::IncrementCommitedMemory(SAPDB_ULong SizeInBytes)
{
  while ( RTE_ISystem::TestAndLock(m_MemoryUsageLock) )
  {
    RTE_ISystem::GiveUpTimeSlice();
  }

  m_MemoryCommitedInBytes += SizeInBytes;

  RTE_ISystem::Unlock(m_MemoryUsageLock);
}

/*---------------------------------------------------------------------------*/

void   RTE_SystemNT::DecrementUsedMemory(SAPDB_ULong SizeInBytes)
{
  while ( RTE_ISystem::TestAndLock(m_MemoryUsageLock) )
  {
    RTE_ISystem::GiveUpTimeSlice();
  }

  SAPDBERR_ASSERT_STATE(SizeInBytes <= m_MemoryUsedInBytes);

  m_MemoryUsedInBytes -= SizeInBytes;

  RTE_ISystem::Unlock(m_MemoryUsageLock);
}

/*---------------------------------------------------------------------------*/

void * RTE_SystemNT::AllocSystemPages(SAPDB_ULong SizeInBytes, bool DoCommit)
{
    return AllocSystemPagesAtFixedAddress((void *)0, SizeInBytes, DoCommit);
}

/*---------------------------------------------------------------------------*/

void * RTE_SystemNT::AllocSystemPages(SAPDB_ULong SizeInBytes, bool DoCommit, Msg_List &messageList)
{
    return AllocSystemPagesAtFixedAddress((void *)0, SizeInBytes, DoCommit, messageList);
}

/*---------------------------------------------------------------------------*/

void * RTE_SystemNT::ReserveSystemPages(SAPDB_ULong SizeInBytes)
{
    return (void *)0; /* not supported */
}

/*---------------------------------------------------------------------------*/

void * RTE_SystemNT::AllocSystemPagesAtFixedAddress(void *FixedAddress, SAPDB_ULong SizeInBytes, bool DoCommit)
{
    SAPDB_ULong SystemPages = ((SizeInBytes + GetSystemPageSize() - 1) & (~(GetSystemPageSize()-1)))
                              / GetSystemPageSize();

    return RTEMem_SystemPageCache::Instance().Allocate(FixedAddress, SystemPages, DoCommit);
}

/*---------------------------------------------------------------------------*/

void * RTE_SystemNT::AllocSystemPagesAtFixedAddress(void *FixedAddress, SAPDB_ULong SizeInBytes, bool DoCommit, Msg_List &messageList)
{
    SAPDB_ULong SystemPages = ((SizeInBytes + GetSystemPageSize() - 1) & (~(GetSystemPageSize()-1)))
                              / GetSystemPageSize();

    return RTEMem_SystemPageCache::Instance().Allocate(FixedAddress, SystemPages, DoCommit, messageList);
}

/*---------------------------------------------------------------------------*/

void * RTE_SystemNT::UncachedAllocSystemPagesAtFixedAddress(void *FixedAddress, SAPDB_ULong SizeInBytes, bool DoCommit, Msg_List &messageList)
{
    return UncachedAllocSystemPagesAtFixedAddress(FixedAddress, SizeInBytes, DoCommit);
}

/*---------------------------------------------------------------------------*/

void * RTE_SystemNT::UncachedAllocSystemPagesAtFixedAddress(void *FixedAddress, SAPDB_ULong SizeInBytes, bool DoCommit)
{
  SAPDB_ULong SystemSize = (SizeInBytes + GetSystemPageSize() - 1) & (~(GetSystemPageSize()-1));
  void *ChunkAddress = NULL;
  bool  DoAllocate   = IncrementUsedMemory(SystemSize);

  if ( DoAllocate )
  {
    // No synchronisation, since direct system call
    ChunkAddress = VirtualAlloc( FixedAddress,
                                 SystemSize,
                               ( DoCommit ? (MEM_COMMIT|MEM_TOP_DOWN) : MEM_TOP_DOWN ),
                                 PAGE_READWRITE );
    if ( !ChunkAddress )
    {
      DecrementUsedMemory(SystemSize);
    }
  }

  if ( 0 != ChunkAddress 
    && 0 != FixedAddress 
    && FixedAddress != ChunkAddress )
  {
      // Only wanted address is acceptable...
    UncachedFreeSystemPages(ChunkAddress, SizeInBytes);
    ChunkAddress = 0;
  }

  if ( ChunkAddress == 0 )
  {
    IncrementCallCounter(m_FailedAllocCalls);
  } 
  else
  {
    IncrementCallCounter(m_SuccessfullAllocCalls);
  }

  return ChunkAddress;
}

/*---------------------------------------------------------------------------*/

void * RTE_SystemNT::CommitSystemPages(void *StartPageAddress, SAPDB_ULong SizeInBytes)
{
  SAPDB_ULong SystemSize = (SizeInBytes + GetSystemPageSize() - 1) & (~(GetSystemPageSize()-1));

  IncrementCallCounter(m_CommitCalls);

  // No synchronisation, since direct system call
  return VirtualAlloc( StartPageAddress,
                       SystemSize,
                       MEM_COMMIT,
                       PAGE_READWRITE );
}

/*---------------------------------------------------------------------------*/

bool   RTE_SystemNT::FreeSystemPages(void *FirstPagePointer, SAPDB_ULong SizeInBytes)
{
    SAPDB_ULong SystemPages = ((SizeInBytes + GetSystemPageSize() - 1) & (~(GetSystemPageSize()-1)))
                              / GetSystemPageSize();

    RTEMem_SystemPageCache::Instance().Deallocate(FirstPagePointer, SystemPages);
    return true;
}

/*---------------------------------------------------------------------------*/

bool   RTE_SystemNT::UncachedFreeSystemPages(void *FirstPagePointer, SAPDB_ULong SizeInBytes)
{
  SAPDB_ULong SystemSize = (SizeInBytes + GetSystemPageSize() - 1) & (~(GetSystemPageSize()-1));

  IncrementCallCounter(m_FreeCalls);

  // No synchronisation, since direct system call
  bool IsFreed = ( VirtualFree( FirstPagePointer,
                                0,
                                MEM_RELEASE) != 0 );
  if ( IsFreed )
  {
    DecrementUsedMemory(SystemSize);
  }

  return IsFreed;
}

/*---------------------------------------------------------------------------*/

SAPDB_ULong RTE_SystemNT::SetMemoryLimitInBytes(SAPDB_ULong NewMemoryLimit, SAPDB_ULong & OldMemoryLimit)
{
  while ( RTE_ISystem::TestAndLock(m_MemoryUsageLock) )
  {
    RTE_ISystem::GiveUpTimeSlice();
  }

  OldMemoryLimit = m_MemoryLimitInBytes;

  if ( NewMemoryLimit != RTE_ISystem::UnlimitedMemory 
    && NewMemoryLimit < m_MemoryUsedInBytes )
  {
    m_MemoryLimitInBytes = m_MemoryUsedInBytes;
  }
  else
  {
    m_MemoryLimitInBytes = NewMemoryLimit;
  }

  SAPDB_ULong SetMemoryLimitInBytes = m_MemoryLimitInBytes;

  RTE_ISystem::Unlock(m_MemoryUsageLock);

  return SetMemoryLimitInBytes;
}

/*---------------------------------------------------------------------------*/

void   RTE_SystemNT::GetMemoryStatistics( SAPDB_ULong & MemoryLimitInBytes,
                                         SAPDB_ULong & MemoryCommitedInBytes,
                                         SAPDB_ULong & MemoryUsedInBytes,
                                         SAPDB_ULong & MemoryMaxUsedInBytes,
                                         SAPDB_UInt8 & MemorySuccessfullAllocs,
                                         SAPDB_UInt8 & MemoryFailedAllocs,
                                         SAPDB_UInt8 & MemoryCommits,
                                         SAPDB_UInt8 & MemoryFrees)
{
  while ( RTE_ISystem::TestAndLock(m_MemoryUsageLock) )
  {
    RTE_ISystem::GiveUpTimeSlice();
  }

  MemoryLimitInBytes = m_MemoryLimitInBytes;
  MemoryCommitedInBytes = m_MemoryCommitedInBytes;
  MemoryUsedInBytes = m_MemoryUsedInBytes;
  MemoryMaxUsedInBytes = m_MemoryMaxUsedInBytes;
  MemorySuccessfullAllocs = m_SuccessfullAllocCalls;
  MemoryFailedAllocs = m_FailedAllocCalls;
  MemoryCommits = m_CommitCalls;
  MemoryFrees = m_FreeCalls;

  RTE_ISystem::Unlock(m_MemoryUsageLock);
}

/*---------------------------------------------------------------------------*/

SAPDB_Char const * RTE_SystemNT::GetLocalNodeName()
{
    if ( !m_LocalNodeName )
    {
        m_LocalNodeName = RTESys_Hostname();
    }
    return m_LocalNodeName;
}

/*---------------------------------------------------------------------------*/

void   RTE_ISystem::InitLock( RTE_Lock &lock )
{
    RTESys_InitLock( &lock );
}

/*---------------------------------------------------------------------------*/

bool   RTE_ISystem::TestAndLock( RTE_Lock &lock )
{
    return RTESys_TestAndLock( &lock );
}

/*---------------------------------------------------------------------------*/

void   RTE_ISystem::Unlock( RTE_Lock &lock )
{
    RTESys_Unlock( &lock );
}

/*---------------------------------------------------------------------------*/

void RTE_ISystem::GetLocalDateTime( RTE_ISystem::DateTime  &LocalDateTime )
{
  SYSTEMTIME      DateTime;

  ::GetLocalTime( &DateTime );
  
  LocalDateTime.Year         = DateTime.wYear;
  LocalDateTime.Month        = DateTime.wMonth;
  LocalDateTime.DayOfWeek    = DateTime.wDayOfWeek;
  LocalDateTime.Day          = DateTime.wDay;
  LocalDateTime.Hour         = DateTime.wHour;
  LocalDateTime.Minute       = DateTime.wMinute;
  LocalDateTime.Second       = DateTime.wSecond;
  LocalDateTime.Milliseconds = DateTime.wMilliseconds;
}

/*---------------------------------------------------------------------------*/

void RTE_ISystem::DoSleep( SAPDB_UInt4 NumberOfMilliseconds )
{
  Sleep( NumberOfMilliseconds );
}

/*---------------------------------------------------------------------------*/

extern "C" LPVOID RTE_AllocStack(LPVOID startAddress, SAPDB_ULong stackSize, SAPDB_ULong guardSize, SAPDB_Bool doCommit)
{
   return RTE_SystemNT::m_Instance->AllocStack(startAddress, stackSize, guardSize, doCommit);
}

/*---------------------------------------------------------------------------*/

extern "C" SAPDB_Bool RTE_FreeStack(LPVOID startAddress, SAPDB_ULong stackSize, SAPDB_ULong guardSize)
{
   return RTE_SystemNT::m_Instance->FreeStack(startAddress, stackSize, guardSize);
}

/*---------------------------------------------------------------------------*/

extern "C" SAPDB_Bool RTE_PseudoAllocStack(SAPDB_ULong stackSize, SAPDB_ULong guardSize)
{
   return RTE_SystemNT::m_Instance->PseudoAllocStack(stackSize, guardSize);
}

/*---------------------------------------------------------------------------*/

extern "C" void RTE_PseudoFreeStack(SAPDB_ULong stackSize, SAPDB_ULong guardSize)
{
   RTE_SystemNT::m_Instance->PseudoFreeStack(stackSize, guardSize);
   return;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/

#endif /* _WIN32 */

