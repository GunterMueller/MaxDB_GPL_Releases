/****************************************************************************

  module      : RTE_SystemUNIX.cpp

  -------------------------------------------------------------------------

  responsible : JoergM/RobinW

  special area: System Interface
  description : Interface class to hide system dependencys from RTE_Global class

  last changed: 2000-12-19  17:00
  see also    : example.html ...
 first created: 2000-02-28  15:58

  -------------------------------------------------------------------------


  copyright:    Copyright (c) 2000-2005 SAP AG


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

#   FreeBSD portions added by Kai Mosebach,
#   For more informations see : http://www.komadev.de/sapdb
#

*****************************************************************************/
#if !defined(_WIN32)

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_ISystem.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "RunTime/System/RTESys_Spinlock.h"
#include "RunTime/System/RTESys_SystemInfo.h"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "RunTime/MemoryManagement/RTEMem_SystemPageCache.hpp"
#include "RunTime/MemoryManagement/RTEMem_PseudoStackAllocator.hpp"
#if defined(OSF1)
#define _XOPEN_SOURCE_EXTENDED /* needed for usleep */
#endif
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#if defined(SUN) || defined(SOLARIS) || defined (FREEBSD)
#include <fcntl.h>
#endif
#ifdef LINUX
/* PTS 1106738 */
/* With RedHat6.2 the mmap() prototype was fixed. This prevented the kernel from */
/* starting. The message given was 'invalid argument'. This Message was given due */
/* to the last argument of mmap() which was defined as 'off_t' and later 'off64_t' */
/* So on RedHat6.2 the last argument was expected to be a 64bit value, therefore */
/* the extra 32bits are collected from the stack. Such an offset is not PAGESIZE */
/* aligned, so it leads to 'invalid argument'... */
#undef __USE_FILE_OFFSET64
#define __USE_LARGEFILE64
#endif /* LINUX */

#include <sys/mman.h>

#include "Messages/Msg_List.hpp"
#include "SAPDBCommon/Messages/SDBMsg_RTEMem.h"
#include "SAPDBCommon/SAPDB_sprintf.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

class RTE_SystemUNIX : public RTE_ISystem
{
public:

  virtual const SAPDB_ULong GetSystemPageSize() const;

  virtual const SAPDB_ULong RoundUpToSystemPageSize(SAPDB_ULong numberOfBytes) const;

  virtual const SAPDB_ULong GetPhysicalMemorySize() const;

  virtual RTE_SystemRc MemProtect (void *pvAdr,SAPDB_ULong ulSize,SAPDB_Int type);

  virtual const SAPDB_UInt4 GetNumberOfCPU() const;

  virtual void * AllocSystemPages(SAPDB_ULong SizeInBytes, bool DoCommit=true);

  virtual void * AllocSystemPages(SAPDB_ULong SizeInBytes, bool DoCommit, Msg_List &messageList);

  virtual void * UncachedAllocSystemPagesAtFixedAddress(void *FixedAddr, SAPDB_ULong SizeInBytes, bool DoCommit=true);

  virtual void * UncachedAllocSystemPagesAtFixedAddress(void *FixedAddr, SAPDB_ULong SizeInBytes, bool DoCommit, Msg_List &messageList);

  virtual void * ReserveSystemPages(SAPDB_ULong SizeInBytes);

  virtual void * AllocSystemPagesAtFixedAddress(void *FixedAddress, SAPDB_ULong SizeInBytes, bool DoCommit=true);

  virtual void * AllocSystemPagesAtFixedAddress(void *FixedAddress, SAPDB_ULong SizeInBytes, bool DoCommit, Msg_List &messageList);

  virtual void * CommitSystemPages(void *StartPageAddress, SAPDB_ULong SizeInBytes);

  virtual bool   FreeSystemPages(void *StartPageAddress, SAPDB_ULong SizeInBytes);

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

  virtual SAPDB_Bool PseudoAllocStack(SAPDB_ULong stackSize, SAPDB_ULong guardSize);
  virtual void PseudoFreeStack(SAPDB_ULong stackSize, SAPDB_ULong guardSize);

  SAPDB_Bool PseudoAllocMemory(SAPDB_ULong SizeInBytes);

  void PseudoFreeMemory(SAPDB_ULong SizeInBytes);

  static RTE_SystemUNIX * m_Instance; // The singleton instance

private:

  RTE_SystemUNIX(SAPDB_ULong MemoryLimitInBytes);

  /* Called in out of memory situation to show detailed memory usage information.
     If memory statistics was already added with an identical number of successfull
     allocation calls, false is returned */
  void AddMemoryStatistic(Msg_List &messageList);

  void ShowAllocationFailedMessage(Msg_List &messageList);
  SAPDB_ULong m_SystemPageSize;

  SAPDB_UInt4 m_NumberOfCPU;

  SAPDB_ULong m_PhysicalMemorySize;

#ifdef USE_FIXED_MMAP_ADDRESS
  static SAPDB_Char *m_nextMmapAddress;
  RTE_Lock m_nextMmapAddressLock;  // Used for locking mmap start address
#endif /* USE_FIXED_MMAP_ADDRESS */

  SAPDB_ULong m_MemoryLimitInBytes;
  SAPDB_ULong m_MemoryUsedInBytes;
  SAPDB_ULong m_MemoryMaxUsedInBytes;
  SAPDB_UInt8 m_SuccessfullAllocCalls;
  SAPDB_UInt8 m_FailedAllocCalls;
  SAPDB_UInt8 m_CommitCalls;
  SAPDB_UInt8 m_FreeCalls;
  RTE_Lock m_MemoryUsageLock;  // Used for locking memory usage counter
  SAPDB_Char const *m_LocalNodeName;

  void IncrementCallCounter(SAPDB_UInt8 &Counter);
  bool IncrementUsedMemory(SAPDB_ULong SizeInBytes);
  void DecrementUsedMemory(SAPDB_ULong SizeInBytes);

  friend class RTE_IGlobal;
  friend class RTE_ISystem;
};

RTE_SystemUNIX * RTE_SystemUNIX::m_Instance = NULL;
#ifdef USE_FIXED_MMAP_ADDRESS
SAPDB_Char * RTE_SystemUNIX::m_nextMmapAddress = ((SAPDB_Char *)0)+USE_FIXED_MMAP_ADDRESS;
#endif /* USE_FIXED_MMAP_ADDRESS */


const SAPDB_ULong       RTE_ISystem::UnlimitedMemory = 0;
const RTE_SystemRc RTE_ISystem::NoError = 0;

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
  SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(RTE_SystemUNIX));

  if ( !RTE_SystemUNIX::m_Instance )
  {
    RTE_SystemUNIX::m_Instance = new (Space) RTE_SystemUNIX(MemoryLimitInBytes);
  }
}

/*---------------------------------------------------------------------------*/
RTE_ISystem & RTE_ISystem::Instance()
{
  RTE_ISystem::Initialize(RTE_ISystem::UnlimitedMemory);

  SAPDBERR_ASSERT_STATE(RTE_SystemUNIX::m_Instance);

  return *RTE_SystemUNIX::m_Instance;
}
/*---------------------------------------------------------------------------*/

RTE_SystemUNIX::RTE_SystemUNIX(SAPDB_ULong MemoryLimitInBytes)
: m_MemoryLimitInBytes(MemoryLimitInBytes),
  m_MemoryUsedInBytes(0),
  m_MemoryMaxUsedInBytes(0),
  m_SuccessfullAllocCalls(0),
  m_FailedAllocCalls(0),
  m_CommitCalls(0),
  m_FreeCalls(0),
  m_LocalNodeName((SAPDB_Char const *)0)
{
  m_SystemPageSize = RTESys_SystemPageSize();
  m_NumberOfCPU = RTESys_NumberOfCPU();
  m_PhysicalMemorySize = RTESys_PhysicalMemorySize();

  InitLock(m_MemoryUsageLock);
#ifdef USE_FIXED_MMAP_ADDRESS
  InitLock(m_nextMmapAddressLock);
#endif /* USE_FIXED_MMAP_ADDRESS */
}

/*---------------------------------------------------------------------------*/

void   RTE_ISystem::GiveUpTimeSlice()
{
    RTESys_GiveUpTimeSlice();
}

/*---------------------------------------------------------------------------*/

const SAPDB_ULong RTE_SystemUNIX::GetSystemPageSize() const
{
  return m_SystemPageSize;
}

/*---------------------------------------------------------------------------*/

const SAPDB_ULong RTE_SystemUNIX::RoundUpToSystemPageSize(SAPDB_ULong numberOfBytes) const
{
    return ( ( numberOfBytes + m_SystemPageSize - 1 ) & ~( m_SystemPageSize - 1 ) );
}

/*---------------------------------------------------------------------------*/

const SAPDB_ULong RTE_SystemUNIX::GetPhysicalMemorySize() const
{
	return m_PhysicalMemorySize;
}

/*---------------------------------------------------------------------------*/

RTE_SystemRc RTE_SystemUNIX::MemProtect(void *pvAdr,SAPDB_ULong ulSize,SAPDB_Int type)
{
    return RTESys_MemProtect(pvAdr, ulSize, type);
}

/*---------------------------------------------------------------------------*/

const SAPDB_UInt4 RTE_SystemUNIX::GetNumberOfCPU() const
{
  return m_NumberOfCPU;
}

/*---------------------------------------------------------------------------*/

void   RTE_SystemUNIX::IncrementCallCounter(SAPDB_UInt8 &Counter)
{
  while ( RTE_ISystem::TestAndLock(m_MemoryUsageLock) )
  {
    RTE_ISystem::GiveUpTimeSlice();
  }

  Counter++;

  RTE_ISystem::Unlock(m_MemoryUsageLock);
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTE_SystemUNIX::PseudoAllocStack(SAPDB_ULong stackSize, SAPDB_ULong guardSize)
{
    SAPDB_ULong SystemSize = (stackSize + GetSystemPageSize() - 1) & (~(GetSystemPageSize()-1));
    bool canAllocate = IncrementUsedMemory(SystemSize);
    if ( !canAllocate )
    {
        errno = ENOMEM;
        return false;
    }

    IncrementCallCounter(m_SuccessfullAllocCalls);
    RTEMem_PseudoStackAllocator::Instance().AddBaseChunk(SystemSize);
    RTEMem_PseudoStackAllocator::Instance().AddChunk(SystemSize-guardSize);
    return true;
}

/*---------------------------------------------------------------------------*/

void RTE_SystemUNIX::PseudoFreeStack(SAPDB_ULong stackSize, SAPDB_ULong guardSize)
{
    IncrementCallCounter(m_FreeCalls);

    SAPDB_ULong SystemSize = (stackSize + GetSystemPageSize() - 1) & (~(GetSystemPageSize()-1));
    RTEMem_PseudoStackAllocator::Instance().ReleaseChunk(SystemSize-guardSize);
    RTEMem_PseudoStackAllocator::Instance().ReleaseBaseChunk(SystemSize);
    DecrementUsedMemory(SystemSize);
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTE_SystemUNIX::PseudoAllocMemory(SAPDB_ULong SizeInBytes)
{
    SAPDB_ULong SystemSize = (SizeInBytes + GetSystemPageSize() - 1) & (~(GetSystemPageSize()-1));

    bool  DoAllocate = IncrementUsedMemory(SystemSize);

    if ( DoAllocate )
    {
        IncrementCallCounter(m_SuccessfullAllocCalls);
    } 
    else
    {
        IncrementCallCounter(m_FailedAllocCalls);
    }

    return DoAllocate;
}

/*---------------------------------------------------------------------------*/

void RTE_SystemUNIX::PseudoFreeMemory(SAPDB_ULong SizeInBytes)
{
    SAPDB_ULong SystemSize = (SizeInBytes + GetSystemPageSize() - 1) & (~(GetSystemPageSize()-1));

    IncrementCallCounter(m_FreeCalls);

    DecrementUsedMemory(SystemSize);
}

/*---------------------------------------------------------------------------*/

bool   RTE_SystemUNIX::IncrementUsedMemory(SAPDB_ULong SizeInBytes)
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

void   RTE_SystemUNIX::DecrementUsedMemory(SAPDB_ULong SizeInBytes)
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

void * RTE_SystemUNIX::AllocSystemPages(SAPDB_ULong SizeInBytes, bool) // Uncommitted pages unsupported
{
  return AllocSystemPagesAtFixedAddress((void *)0, SizeInBytes);
}

/*---------------------------------------------------------------------------*/

void * RTE_SystemUNIX::AllocSystemPages(SAPDB_ULong SizeInBytes, bool DoCommit, Msg_List &messageList) // Uncommitted pages unsupported
{
  return AllocSystemPagesAtFixedAddress((void *)0, SizeInBytes, DoCommit, messageList);
}

/*---------------------------------------------------------------------------*/

void * RTE_SystemUNIX::ReserveSystemPages(SAPDB_ULong SizeInBytes)
{
#ifdef USE_FIXED_MMAP_ADDRESS
    SAPDB_ULong SystemSize = (SizeInBytes + GetSystemPageSize() - 1) & (~(GetSystemPageSize()-1));
    void *mmapAddress;

    while ( RTE_ISystem::TestAndLock(m_nextMmapAddressLock) )
    {
        RTE_ISystem::GiveUpTimeSlice();
    }

    mmapAddress = m_nextMmapAddress;
    m_nextMmapAddress += SystemSize;
    RTE_ISystem::Unlock(m_nextMmapAddressLock);

    return mmapAddress;
#else
    return (void *)0;
#endif /* USE_FIXED_MMAP_ADDRESS */
}

/*---------------------------------------------------------------------------*/

void * RTE_SystemUNIX::AllocSystemPagesAtFixedAddress(void *FixedAddress, SAPDB_ULong SizeInBytes, bool) // Uncommitted pages unsupported
{
    SAPDB_ULong SystemPages = ((SizeInBytes + GetSystemPageSize() - 1) & (~(GetSystemPageSize()-1)))
                              / GetSystemPageSize();

    return RTEMem_SystemPageCache::Instance().Allocate(FixedAddress, SystemPages, true);
}

/*---------------------------------------------------------------------------*/

void * RTE_SystemUNIX::AllocSystemPagesAtFixedAddress(void *FixedAddress, SAPDB_ULong SizeInBytes, bool, Msg_List &messageList) // Uncommitted pages unsupported
{
    SAPDB_ULong SystemPages = ((SizeInBytes + GetSystemPageSize() - 1) & (~(GetSystemPageSize()-1)))
                              / GetSystemPageSize();

    return RTEMem_SystemPageCache::Instance().Allocate(FixedAddress, SystemPages, true, messageList);
}

/*---------------------------------------------------------------------------*/

void RTE_SystemUNIX::ShowAllocationFailedMessage(Msg_List &messageList)
{
    static SAPDB_UInt8 lastSuccessfullAllocs = 0;

    SAPDB_UInt8 successfullAllocs;

    while ( RTE_ISystem::TestAndLock(m_MemoryUsageLock) )     
    {
        RTE_ISystem::GiveUpTimeSlice();
    }

    successfullAllocs = m_SuccessfullAllocCalls;

    RTE_ISystem::Unlock(m_MemoryUsageLock);

    if ( successfullAllocs > lastSuccessfullAllocs )
    {
        RTE_Message(messageList);
    }
}

//-----

void RTE_SystemUNIX::AddMemoryStatistic(Msg_List &messageList)
{
    SAPDB_ULong MemoryLimitInBytes;
    SAPDB_ULong MemoryCommitedInBytes;
    SAPDB_ULong MemoryUsedInBytes;
    SAPDB_ULong MemoryMaxUsedInBytes;
    SAPDB_UInt8 MemorySuccessfullAllocs;
    SAPDB_UInt8 MemoryFailedAllocs;
    SAPDB_UInt8 MemoryCommits;
    SAPDB_UInt8 MemoryFrees;

    GetMemoryStatistics( MemoryLimitInBytes,
                         MemoryCommitedInBytes,
                         MemoryUsedInBytes,
                         MemoryMaxUsedInBytes,
                         MemorySuccessfullAllocs,
                         MemoryFailedAllocs,
                         MemoryCommits,
                         MemoryFrees );

    if ( MemoryLimitInBytes == RTE_ISystem::UnlimitedMemory )
    {
        messageList.AddDetail( SDBMSG_RTEMEM_SYSTEM_HEAP_MEMORY_STATISTICS,
                               Msg_Arg("BYTES_USED",MemoryUsedInBytes),
                               Msg_Arg("BYTE_SIZE","system imposed limitation"),
                               Msg_Arg("ALLOC_COUNT",MemorySuccessfullAllocs),
	                           Msg_Arg("FREE_COUNT",MemoryFrees),
	                           Msg_Arg("ERROR_COUNT",MemoryFailedAllocs),
	                           Msg_Arg("BYTES_MAX_USED",MemoryMaxUsedInBytes) );
    }
    else
    {
        messageList.AddDetail( SDBMSG_RTEMEM_SYSTEM_HEAP_MEMORY_STATISTICS,
                               Msg_Arg("BYTES_USED",MemoryUsedInBytes),
                               Msg_Arg("BYTE_SIZE",MemoryLimitInBytes),
                               Msg_Arg("ALLOC_COUNT",MemorySuccessfullAllocs),
	                           Msg_Arg("FREE_COUNT",MemoryFrees),
	                           Msg_Arg("ERROR_COUNT",MemoryFailedAllocs),
	                           Msg_Arg("BYTES_MAX_USED",MemoryMaxUsedInBytes) );
    }

#ifdef USE_FIXED_MMAP_ADDRESS
    void *mmapAddress;

    while ( RTE_ISystem::TestAndLock(m_nextMmapAddressLock) )
    {
        RTE_ISystem::GiveUpTimeSlice();
    }

    mmapAddress = m_nextMmapAddress;

    RTE_ISystem::Unlock(m_nextMmapAddressLock);

    messageList.AddDetail( SDBMSG_RTEMEM_NEXT_RESERVED_ADDRESS,
                           Msg_Arg("ADDRESS",mmapAddress) );
#endif /* USE_FIXED_MMAP_ADDRESS */

}

/*---------------------------------------------------------------------------*/

void * RTE_SystemUNIX::UncachedAllocSystemPagesAtFixedAddress(void *FixedAddress, SAPDB_ULong SizeInBytes, bool DoCommit)
{
    Msg_List tmpMessageList;
    void *ptr = UncachedAllocSystemPagesAtFixedAddress(FixedAddress, SizeInBytes, DoCommit, tmpMessageList);
    if ( ptr )
    {
        return ptr;
    }

    // only output if meanwhile an allocation succeeded...
    ShowAllocationFailedMessage( tmpMessageList );

    return 0;
}

/*---------------------------------------------------------------------------*/

void * RTE_SystemUNIX::UncachedAllocSystemPagesAtFixedAddress(void *FixedAddress, SAPDB_ULong SizeInBytes, bool DoCommit, Msg_List &messageList)
{
    // Round up to Systempage size
  SAPDB_ULong SystemSize = (SizeInBytes + GetSystemPageSize() - 1) & (~(GetSystemPageSize()-1));
  void *ChunkAddress = 0;
  bool  DoAllocate = IncrementUsedMemory(SystemSize);

  if ( DoAllocate )
  {
#ifdef USE_VMALLOC_NOT_MMAP

#ifdef HAS_NPTL
        if ( posix_memalign(&ChunkAddress, GetSystemPageSize(), SizeInBytes) )
        {
            ChunkAddress = 0;
        }
#else
        ChunkAddress = valloc(SystemSize);
#endif

        if ( !ChunkAddress )
        {
            SAPDB_Char buffer[256];
            SAPDB_Char const *errorText;
            switch( errno )
            {
            case EINVAL:
                errorText = "EINVAL ? bad alignment";
                break;
            case ENOMEM:
                errorText = "ENOMEM ? out of memory";
                break;
            default:
                SAPDB_sprintf(buffer, sizeof(buffer), "Unexpected error code %d", errno);
                errorText = buffer;
                break;
            }
            DecrementUsedMemory(SystemSize);

            messageList = Msg_List( Msg_List::Error,
	                            SDBMSG_RTEMEM_NO_MORE_SYSTEM_PAGES,
	                            Msg_Arg("BYTES_NEEDED",SizeInBytes),
	                            Msg_Arg("BYTE_SIZE",GetSystemPageSize()),
#ifdef HAS_NPTL
	                            Msg_Arg("DESCRIPTION","posix_memalign"),
#else
	                            Msg_Arg("DESCRIPTION","valloc"),
#endif
                                Msg_Arg("ERRORTEXT",errorText) );

            
            AddMemoryStatistic(messageList);
        }

#else

      SAPDB_Char *mmapAddress = 0; // let system choose

      if ( FixedAddress != 0 )
      {
          mmapAddress = (SAPDB_Char *)FixedAddress;
      }
      else
      {
          mmapAddress = (SAPDB_Char *)ReserveSystemPages(SizeInBytes);
      }

#if defined(SUN) || defined(SOLARIS)
      ChunkAddress = (void *)mmap( (char *)mmapAddress, SystemSize, PROT_READ|PROT_WRITE,
          mmapAddress != 0 ? MAP_FIXED|MAP_PRIVATE|MAP_ANONYMOUS : MAP_PRIVATE|MAP_ANONYMOUS, -1, 0 );
#elif !defined(AIX)
      ChunkAddress = (void *)mmap( mmapAddress, SystemSize, PROT_READ|PROT_WRITE,
          mmapAddress != 0 ? MAP_FIXED|MAP_PRIVATE|MAP_ANONYMOUS : MAP_PRIVATE|MAP_ANONYMOUS, -1, 0 );
#else
      /* AIX is very special... Two physical pages are allocated if MAP_PRIVATE|MAP_ANONYMOUS is used... */
      ChunkAddress = (void *)mmap( (void *)0, SystemSize, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0 );
#endif

      if ( ChunkAddress == (void *) MAP_FAILED )
      {
          SAPDB_Char buffer[256];
          SAPDB_Char const *errorText;
          switch( errno )
          {
          case EINVAL:
            errorText = "EINVAL ? value too large";
            break;
          case EAGAIN:
            errorText = "EAGAIN ? too much memory locked";
            break;
          case ENOMEM:
            errorText = "ENOMEM ? out of memory";
            break;
          default:
            SAPDB_sprintf(buffer, sizeof(buffer), "Unexpected error code %d", errno);
            errorText = buffer;
            break;
          }
          DecrementUsedMemory(SystemSize);

          messageList = Msg_List( Msg_List::Error,
	                            SDBMSG_RTEMEM_NO_MORE_SYSTEM_PAGES,
	                            Msg_Arg("BYTES_NEEDED",SizeInBytes),
	                            Msg_Arg("BYTE_SIZE",GetSystemPageSize()),
	                            Msg_Arg("DESCRIPTION","mmap"),
	                            Msg_Arg("ERRORTEXT",errorText) );
          AddMemoryStatistic(messageList);

          ChunkAddress = (void *)0;
      }

      if ( 0 != ChunkAddress
        && 0 != FixedAddress
        && FixedAddress != ChunkAddress )
      {
        messageList = Msg_List( Msg_List::Error,
                                SDBMSG_RTEMEM_OUT_OF_RESERVABLE_SYSTEM_PAGES,
                                Msg_Arg("BYTES_NEEDED",SizeInBytes),
                                Msg_Arg("ADDRESS",FixedAddress) );

#ifdef USE_FIXED_MMAP_ADDRESS
        void *mmapAddress;

        while ( RTE_ISystem::TestAndLock(m_nextMmapAddressLock) )
        {
            RTE_ISystem::GiveUpTimeSlice();
        }

        mmapAddress = m_nextMmapAddress;

        RTE_ISystem::Unlock(m_nextMmapAddressLock);

        messageList.AddDetail( SDBMSG_RTEMEM_NEXT_RESERVED_ADDRESS,
                               Msg_Arg("ADDRESS",mmapAddress) );
#endif
        // Only wanted address is acceptable...
        UncachedFreeSystemPages(ChunkAddress, SizeInBytes);

        ChunkAddress = 0;
      }

#endif /* USE_VMALLOC_NOT_MMAP */
  }
  else
  {
      SAPDB_ULong MemoryLimitInBytes;

      while ( RTE_ISystem::TestAndLock(m_MemoryUsageLock) )
      {
        RTE_ISystem::GiveUpTimeSlice();
      }

      MemoryLimitInBytes = m_MemoryLimitInBytes;

      RTE_ISystem::Unlock(m_MemoryUsageLock);

      messageList = Msg_List( Msg_List::Error,
                            SDBMSG_RTEMEM_MEMORY_LIMIT_REACHED,
	                        Msg_Arg("ALLOCATOR","Systemheap"),
	                        Msg_Arg("BYTE_SIZE",MemoryLimitInBytes),
	                        Msg_Arg("BYTES_NEEDED",SizeInBytes) );
  }

  if ( ChunkAddress == 0 )
  {
    IncrementCallCounter(m_FailedAllocCalls);
  }
  else
  {
    IncrementCallCounter(m_SuccessfullAllocCalls);
  }

  return ( ChunkAddress );
}

/*---------------------------------------------------------------------------*/

void * RTE_SystemUNIX::CommitSystemPages(void *StartPageAddress, SAPDB_ULong)
{
  IncrementCallCounter(m_CommitCalls);

  // Uncommited memory not supported
  return StartPageAddress;
}

/*---------------------------------------------------------------------------*/

bool   RTE_SystemUNIX::FreeSystemPages(void *FirstPagePointer, SAPDB_ULong SizeInBytes)
{
    SAPDB_ULong SystemPages = ((SizeInBytes + GetSystemPageSize() - 1) & (~(GetSystemPageSize()-1)))
                              / GetSystemPageSize();

    RTEMem_SystemPageCache::Instance().Deallocate(FirstPagePointer, SystemPages);
    return true;
}

/*---------------------------------------------------------------------------*/

bool RTE_SystemUNIX::UncachedFreeSystemPages(void *StartPageAddress, SAPDB_ULong SizeInBytes)
{
  // Round up to Systempage size
  SAPDB_ULong SystemSize = (SizeInBytes + GetSystemPageSize() - 1) & (~(GetSystemPageSize()-1));
  bool IsFreed;

#ifdef USE_VMALLOC_NOT_MMAP

  free(StartPageAddress);
  IsFreed = (StartPageAddress != 0);

#else

#  if defined(SUN) || defined(SOLARIS)
     IsFreed = ( munmap((char *)StartPageAddress, SystemSize) == 0 );
#  else
     IsFreed = ( munmap(StartPageAddress, SystemSize) == 0 );
#  endif

#endif

  IncrementCallCounter(m_FreeCalls);

  if ( IsFreed )
  {
    DecrementUsedMemory(SystemSize);
  }

  return IsFreed;
}

/*---------------------------------------------------------------------------*/

SAPDB_ULong RTE_SystemUNIX::SetMemoryLimitInBytes(SAPDB_ULong NewMemoryLimit, SAPDB_ULong & OldMemoryLimit)
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

void   RTE_SystemUNIX::GetMemoryStatistics( SAPDB_ULong & MemoryLimitInBytes,
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
  MemoryCommitedInBytes = m_MemoryUsedInBytes; // identical to used bytes since no commit supported
  MemoryUsedInBytes = m_MemoryUsedInBytes;
  MemoryMaxUsedInBytes = m_MemoryMaxUsedInBytes;
  MemorySuccessfullAllocs = m_SuccessfullAllocCalls;
  MemoryFailedAllocs = m_FailedAllocCalls;
  MemoryCommits = m_CommitCalls;
  MemoryFrees = m_FreeCalls;

  RTE_ISystem::Unlock(m_MemoryUsageLock);
}

/*---------------------------------------------------------------------------*/

SAPDB_Char const * RTE_SystemUNIX::GetLocalNodeName()
{
    if ( !m_LocalNodeName )
    {
        m_LocalNodeName = RTESys_Hostname();
    }
    return m_LocalNodeName;
}

/*---------------------------------------------------------------------------*/

void RTE_ISystem::InitLock( RTE_Lock & lock )
{
    RTESys_InitLock( &lock );
}

/*---------------------------------------------------------------------------*/

bool RTE_ISystem::TestAndLock( RTE_Lock & lock )
{
    return RTESys_TestAndLock( &lock );
}

/*---------------------------------------------------------------------------*/

void RTE_ISystem::Unlock( RTE_Lock & lock )
{
    RTESys_Unlock( &lock );
}

/*---------------------------------------------------------------------------*/

void RTE_ISystem::GetLocalDateTime( RTE_ISystem::DateTime  &LocalDateTime )
{
  time_t          DateTime;
  struct tm       *pDateTime_tm;

  time ( &DateTime );

#ifdef _REENTRANT
  struct tm       tm_buffer;
  pDateTime_tm = localtime_r ( &DateTime, &tm_buffer );
#else
  pDateTime_tm = localtime ( &DateTime );
#endif

  LocalDateTime.Year         = pDateTime_tm->tm_year + 1900;
  LocalDateTime.Month        = pDateTime_tm->tm_mon + 1;
  LocalDateTime.DayOfWeek    = pDateTime_tm->tm_wday;
  LocalDateTime.Day          = pDateTime_tm->tm_mday;
  LocalDateTime.Hour         = pDateTime_tm->tm_hour;
  LocalDateTime.Minute       = pDateTime_tm->tm_min;
  LocalDateTime.Second       = pDateTime_tm->tm_sec;
  LocalDateTime.Milliseconds = 0;
}

/*---------------------------------------------------------------------------*/

void RTE_ISystem::DoSleep( SAPDB_UInt4 NumberOfMilliseconds )
{
  SAPDB_UInt4 NumberOfSeconds = NumberOfMilliseconds/1000;
  SAPDB_UInt4 RemainingMilliseconds = NumberOfMilliseconds%1000;

  if ( NumberOfSeconds == 0 && NumberOfMilliseconds > 0 )
  {
    /* If less than a second, but not 0 sleep for given number of Microseconds ! */
    usleep(RemainingMilliseconds * 1000);
  }
  else
  {
    sleep( NumberOfSeconds );
    if ( RemainingMilliseconds > 0 )
    {
      usleep(RemainingMilliseconds * 1000);
    }
  }
}

/*---------------------------------------------------------------------------*/

extern "C" SAPDB_Bool RTE_PseudoAllocStack(SAPDB_ULong stackSize, SAPDB_ULong guardSize)
{
   return RTE_SystemUNIX::m_Instance->PseudoAllocStack(stackSize, guardSize);
}

/*---------------------------------------------------------------------------*/

extern "C" void RTE_PseudoFreeStack(SAPDB_ULong stackSize, SAPDB_ULong guardSize)
{
   RTE_SystemUNIX::m_Instance->PseudoFreeStack(stackSize, guardSize);
   return;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTE_PseudoAllocMemory(SAPDB_ULong SizeInBytes)
{
   return RTE_SystemUNIX::m_Instance->PseudoAllocMemory(SizeInBytes);
}

/*---------------------------------------------------------------------------*/

void RTE_PseudoFreeMemory(SAPDB_ULong SizeInBytes)
{
   RTE_SystemUNIX::m_Instance->PseudoFreeMemory(SizeInBytes);
   return;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/

#endif /* !_WIN32 */

