/*!**************************************************************************

  module       : RTEMem_PageAllocator.cpp

  -------------------------------------------------------------------------

  responsible : J. Mensing (jrg)

  special area: RTE I/O Page Allocator
  description : Singleton for I/O Page allocation


  first created:2000-11-07  10:09
  last changed: 2000-11-27  11:56

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




*****************************************************************************/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "RunTime/MemoryManagement/RTEMem_PageAllocator.hpp"
#include "RunTime/MemoryManagement/RTEMem_BlockAllocator.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "RunTime/RTE_Message.hpp"

extern SAPDBTrace_Topic Memory_Trace;
#ifndef WIN32                          /* &if $OS not in [ WIN32 ] */
#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp" /* nocheck */
#include "RunTime/RTE_saveUNIXcalls.h" /* nocheck */
#include "RunTime/RTE_SystemUNIX.hpp" /* nocheck */
#endif                                 /* &endif */

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

/*===========================================================================*
 *=====================  END OF DECLARATIONS ================================*
 *===========================================================================*/

/*===========================================================================*
 *  EXPORTED FUNCTIONS, EXPORTED CLASS METHODS (IMPLEMENTATION)              *
 *===========================================================================*/

void RTEMem_PageAllocator::Initialize( SAPDB_Int4 dbPageSize )
{
    SAPDBTRACE_ROUTINE_DEBUG("RTEMem_PageAllocator::Initialize", Memory_Trace, 1);

    SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(RTEMem_PageAllocator));

    m_Instance = new (Space) RTEMem_PageAllocator(dbPageSize);
}

/*---------------------------------------------------------------------------*/

RTEMem_PageAllocator & RTEMem_PageAllocator::Instance()
{
    SAPDBTRACE_ROUTINE_DEBUG("RTEMem_PageAllocator::Instance", Memory_Trace, 5);

    SAPDBERR_ASSERT_STATE(m_Instance != NULL);

    return *m_Instance;
}

RTEMem_PageAllocator * RTEMem_PageAllocator::m_Instance = NULL;

/*---------------------------------------------------------------------------*/
#ifndef WIN32

void *RTE_AllocateSharedMemory(  key_t key
                               , SAPDB_ULong alignedSizeInBytes
                               , SAPDB_Char const * &sysCall
                               , SAPDB_Char const * &errorText)
{
    if ( !RTE_PseudoAllocMemory(alignedSizeInBytes) )
    {
        sysCall = "internal memory limit";
        errorText = "memory limit exceeded";
        return 0;
    }

    int shm_id = shmget(key, alignedSizeInBytes, IPC_CREAT|0600);
    if ( shm_id < 0 )
    {
        sysCall = "shmget";
        switch( errno )
        {
        case EACCES: errorText = "permission denied";
            break;
        case ENOMEM: errorText = "out of memory";
            break;
        case ENOSPC: errorText = "limit of shared memory IDs reached";
            break;
        case ENOENT: errorText = "shared memory region does not exist";
            break;
        case EEXIST: errorText = "shared memory region already exist";
            break;
        case EINVAL: errorText = "invalid size or key";
            break;
        default:
                     errorText = "failed";
            break;
        }
        return 0;
    }
    void *chunk_address = shmat(shm_id, 0, 0);
    if ( (void *)-1 == chunk_address )
    {
        sysCall = "shmat";
        switch (errno )
        {
        case EACCES: errorText = "permission denied";
            break;
        case ENOMEM: errorText = "out of memory";
            break;
        case EMFILE: errorText = "limit of attached shared memory regions per process reached";
            break;
        case EINVAL: errorText = "invalid id or address";
            break;
        default:
                     errorText = "failed";
        }
        (void)shmctl(shm_id, IPC_RMID, 0);
        return 0;
    }

    if ( -1 == shmctl(shm_id, IPC_RMID, 0) )
    {
        sysCall = "shmctl(IPC_RMID)";
        switch (errno )
        {
        case EINVAL: errorText = "invalid id";
            break;
        case EPERM:  errorText = "permission denied";
            break;
        default:
                     errorText = "failed";
        }
        return 0;
    }
    return chunk_address;
}
#endif

#ifdef AIX
extern "C" void RTEMem_PageAllocator_Reset()
{
    RTEMem_PageAllocator::Reset();
}
#endif

/*---------------------------------------------------------------------------*/

/*!
  Function: RTEMem_InitializePageAllocator
  Description: Initialize the RTEMem_PageAllocator

  The database page size is dynamically setup. After it is known this call is used to
  collect memory in chunks of maximum size up to amount of memory wanted to be reserved
  for IO Pages. The allocatation reduces the chunk size each time, an allocation fails
  until either all IO Pages are allocated or a chunk is smaller than a database page.
  Minimum chunk size is system dependend handled via RTEMem_BlockAllocator.
  
  Arguments: dbPageSize [in] Size of a database page in bytes
             totalIOPages [in] total number of IO Pages needed
  Return value: true allocator initialized, false allocator not initialized
 */
extern "C" bool RTEMem_InitializePageAllocator(SAPDB_UInt4 dbPageSize,
                                               SAPDB_UInt4 totalIOPages)
{
    SAPDBTRACE_ROUTINE_DEBUG("RTEMem_InitializePageAllocator", Memory_Trace, 1);
    SAPDBTRACE_WRITELN(Memory_Trace, 5, "dbPageSize " << dbPageSize << " totalIOPages " << totalIOPages );

    SAPDBErr_MessageList messageList;

#ifndef WIN32
    RTEConf_Parameter::String paramValue;
    SAPDB_Bool  useIpcShm;
    if ( RTEConf_ParameterAccess::Instance()->GetString((RTEConf_Parameter::Name)"CACHE_IN_SHARED_MEMORY", paramValue, messageList) )
    {
        useIpcShm = ( strcmp("YES", (SAPDB_Char *)paramValue) == 0 ? 1 : 0 );
    }
    else
    {
        useIpcShm = 0;
    }
#endif

    SAPDB_ULong blockSize;
#ifndef WIN32
    if ( useIpcShm )
    {
#if defined(SSM_SIZE)
        blockSize = SSM_SIZE;
#else
        blockSize = RTE_ISystem::Instance().GetSystemPageSize();
#endif
    }
    else
#endif
    {
        blockSize = RTEMem_BlockAllocator::Instance().GetBlockSize();
    }

    // SAPDB_ULong cast needed to prevent possible overflow during multiplication
    SAPDB_UInt8 totalIOPageSizeInBytes = (SAPDB_UInt8)dbPageSize * (SAPDB_UInt8)totalIOPages; 

    SAPDBERR_ASSERT_STATE( (totalIOPageSizeInBytes/totalIOPages) == dbPageSize );

    SAPDB_UInt4 totalSizeInBlocks;
#ifndef WIN32
    if ( useIpcShm )
    {
        totalSizeInBlocks = ((SAPDB_ULong)totalIOPageSizeInBytes + blockSize - 1)/blockSize;
    }
    else
#endif
    {
        totalSizeInBlocks = RTEMem_BlockAllocator::Instance().CalcBlockCount( (SAPDB_ULong)totalIOPageSizeInBytes );
    }

    SAPDB_UInt4 blocksAdded = 0;
    SAPDB_UInt4 blocksToAdd = totalSizeInBlocks;

    SAPDBTRACE_WRITELN(Memory_Trace, 6, "blockSize " << blockSize << " totalSizeInBlocks " << totalSizeInBlocks );

    RTEMem_PageAllocator::Initialize(dbPageSize);

    if( totalIOPageSizeInBytes != (SAPDB_ULong)totalIOPageSizeInBytes ) // 32-bit system and totalIOPageSizeInBytes > MAXINT4 
    {
        SAPDBTRACE_WRITELN(Memory_Trace, 1, "allocation failed: wanted more than 4GB (" << totalIOPages << "pages à " << dbPageSize << ")on a 32-bit system" );
        return false;
    }

    while ( blocksAdded < totalSizeInBlocks )
    {
        SAPDBTRACE_WRITELN(Memory_Trace, 7, "try to add " << blocksToAdd << " blocks" );

        void *pagePtr;
#ifndef WIN32
        if ( useIpcShm )
        {
            char const *sysCall;
            char const *errorText;
            pagePtr = RTE_AllocateSharedMemory(
                  IPC_PRIVATE
                , (SAPDB_ULong)blocksToAdd * (SAPDB_ULong)blockSize
                , sysCall
                , errorText);
            if ( !pagePtr )
            {
#define RTEERR_RTEMEM_NO_MORE_SYSTEM_PAGES             SAPDBErr_MessageList::Error,20172,"No more system pages for %s bytes available. Pagesize %s bytes, used syscall %s, error %s",4
                messageList = SAPDBErr_MessageList( RTE_CONTEXT,
                                                    RTEERR_RTEMEM_NO_MORE_SYSTEM_PAGES,
	                                                SAPDB_ToString(blockSize * blocksToAdd),
	                                                SAPDB_ToString(blockSize),
	                                                SAPDB_ToString(sysCall),
	                                                SAPDB_ToString(errorText) );
            }
        }
        else
#endif
        {
            pagePtr = RTEMem_BlockAllocator::Instance().Allocate(blocksToAdd, messageList);
        }

        if ( pagePtr != 0 )
        {
#if defined(SAPDB_SLOW)
            // initialize memory with distinct pattern in slow kernel
            memset(pagePtr, 0xba, blockSize * blocksToAdd);
#endif
            RTEMem_PageAllocator::Instance().AddMemoryBlock(  pagePtr, blockSize * blocksToAdd );
            blocksAdded += blocksToAdd;
            SAPDBTRACE_WRITELN(Memory_Trace, 6, "added " << blocksToAdd << " blocks at " << (char *)pagePtr );
            blocksToAdd = totalSizeInBlocks - blocksAdded;
        }
        else
        {
            SAPDBTRACE_WRITELN(Memory_Trace, 7, "split chunk");
            // retry with smaller block
            blocksToAdd /= 2;
            if ( (blocksToAdd * blockSize) < dbPageSize )
            {
                RTE_Message(messageList);
                SAPDBTRACE_WRITELN(Memory_Trace, 1, "allocation failed: got " << blocksAdded << " wanted " << totalSizeInBlocks );
                return false;
            }
        }

    } // while ( blocksAdded < totalSizeInBlocks )

    return true;
}

//-------------------------------------------------------------------------

void* RTEMem_PageAllocator::Allocate(SAPDB_ULong ByteCount)
{
    Page* Ptr = 0;

    // If <ByteCount> is not equal to the page size, don't return memory
    if ( ByteCount != m_PageSize )
    {
        m_freeBlockChainLock.Lock();
            m_CountError++;
        m_freeBlockChainLock.Unlock();
    }
    else
    {
        m_freeBlockChainLock.Lock();
            ++m_CountAlloc;
            // Get the pointer of the first free page in the chain.
            Ptr = m_FirstFreePagePtr;
            if ( Ptr != 0 )
            {
                // Update the chain of free pages.
                m_FirstFreePagePtr = Ptr->NextPage;
                if (m_FirstFreePagePtr != 0) {
                    m_FirstFreePagePtr->PreviousPage = 0;
                }
                // Decrement the number of free pages
                --m_FreePages;
            }
            else
            {
                m_CountError++;
            }
        m_freeBlockChainLock.Unlock();
    }

    if ( Ptr == 0 )
    {
#ifndef SAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING
        throw SAPDBMem_BadAllocException(__CONTEXT__, ByteCount);
#else
        return 0;
#endif
    }

    Ptr->NextPage = 0;
    return Ptr;
}

//-------------------------------------------------------------------------

void RTEMem_PageAllocator::Deallocate(void *Ptr)
{
    Page* PagePtr = reinterpret_cast<Page*>(Ptr);

    m_freeBlockChainLock.Lock();
        ++m_CountDealloc;
        // Chain the free page into the list.
        PagePtr->NextPage = m_FirstFreePagePtr;
        PagePtr->PreviousPage = 0;
        if (m_FirstFreePagePtr != 0)
            m_FirstFreePagePtr->PreviousPage = PagePtr;
        m_FirstFreePagePtr = PagePtr;
        // Increment the number of free pages
        ++m_FreePages;
    m_freeBlockChainLock.Unlock();
}

//-------------------------------------------------------------------------

void RTEMem_PageAllocator::AddMemoryBlock(void *Ptr, SAPDB_ULong Size)
{
    SAPDB_Byte *NextPtr;
    Page *PagePtr, *PrevPtr;

    // Check that <Size> is a multiple of m_PageSize
    SAPDBERR_ASSERT_ARGUMENT((Size % m_PageSize) == 0);

    // Divide the memory block into pages and chain them.
    NextPtr = reinterpret_cast<SAPDB_Byte*>(Ptr);
    PrevPtr = 0;
    for (SAPDB_ULong i=0; i<(Size/m_PageSize); ++i) {
        PagePtr = reinterpret_cast<Page*>(NextPtr);
        NextPtr += m_PageSize;
        PagePtr->NextPage = reinterpret_cast<Page*>(NextPtr);
        PagePtr->PreviousPage = PrevPtr;
        PrevPtr = reinterpret_cast<Page*>(PagePtr);
    }

    SAPDB_ULong addedPages = Size/m_PageSize;

    m_freeBlockChainLock.Lock();
        // Chain the new block into the free list.
        PagePtr->NextPage = m_FirstFreePagePtr;
        if (m_FirstFreePagePtr != 0) {
            m_FirstFreePagePtr->PreviousPage = PagePtr;
        }
        m_FirstFreePagePtr = reinterpret_cast<Page*>(Ptr);
        // Fit the number of free pages.
        m_FreePages += addedPages;
    m_freeBlockChainLock.Unlock();
}

/*===========================================================================*
 *  LOCAL/INLINE FUNCTIONS, LOCAL CLASS METHODS (IMPLEMENTATION)             *
 *===========================================================================*/

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
