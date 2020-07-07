/****************************************************************************

  module      : RTEMem_BlockAllocator.cpp

  -------------------------------------------------------------------------

  responsible : JoergM

  special area: Block Allocator
  description : Allocator used to allocate blocks of memory
  
  last changed: 2000-12-14  13:41
  see also    : example.html ...
  first created: 2000-02-28  15:44

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
#include "RunTime/RTE_ISystem.hpp"
#include "RunTime/MemoryManagement/RTEMem_BlockAllocator.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_Exceptions.hpp"
#include "RunTime/MemoryManagement/RTEMem_AllocatorRegister.hpp"

#ifdef SAPDBMem_USE_SYSTEM_ALLOC
#include <stdlib.h>
#endif

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
 *  METHODS                                                                  *
 *===========================================================================*/

/*---------------------------------------------------------------------------*/
// Static member variables
RTEMem_BlockAllocator * RTEMem_BlockAllocator::m_Instance = 0;
RTEMem_AllocatorInfo  * RTEMem_BlockAllocator::m_pAllocatorInfo = NULL;

/*---------------------------------------------------------------------------*/

#ifdef AIX
extern "C" void RTEMem_BlockAllocator_Reset()
{
    RTEMem_BlockAllocator::Reset();
}
#endif

/*---------------------------------------------------------------------------*/

SAPDBMem_IBlockAllocator & RTEMem_BlockAllocator::Instance()
{
  // In this stage we have no allocator, so we use static storage... 
  SAPDBMEM_STATIC_RAW_ALLOCATE( Space, sizeof(RTEMem_BlockAllocator) );

  if ( !m_Instance )
  {
    m_Instance = new (Space) RTEMem_BlockAllocator();
  }
  return *m_Instance;
}

/*---------------------------------------------------------------------------*/

RTEMem_BlockAllocator::RTEMem_BlockAllocator( SAPDB_ULong BlockSize )
: m_BytesControlled(0)
, m_CountAlloc(0)
, m_CountDealloc(0)
, m_AllocatorInfo(NULL,NULL,NULL)
{
  m_AlignmentSize = RTE_ISystem::Instance().GetSystemPageSize();
  SAPDBERR_ASSERT_ARGUMENT(m_AlignmentSize > 0);
  m_BlockSize = ( BlockSize == 0 ? m_AlignmentSize : BlockSize );
  SAPDBERR_ASSERT_ARGUMENT(m_BlockSize > 0);

  if ( m_AlignmentSize <= m_BlockSize )
  {
    SAPDBERR_ASSERT_ARGUMENT(m_BlockSize == m_AlignmentSize * (m_BlockSize/m_AlignmentSize));
    m_AlignmentSize = m_BlockSize/(m_BlockSize/m_AlignmentSize);
  }
  else
  {
    SAPDBERR_ASSERT_ARGUMENT(m_AlignmentSize == m_BlockSize * (m_AlignmentSize/m_BlockSize));
    m_AlignmentSize = m_AlignmentSize/(m_AlignmentSize/m_BlockSize);
  }
  new (&m_AllocatorInfo) RTEMem_AllocatorInfo (this->GetIdentifier(), 
		this, (const SAPDB_UTF8 *)"SystemPageCache");
  m_pAllocatorInfo = (RTEMem_AllocatorInfo *)&m_AllocatorInfo;
  RTEMem_AllocatorRegister::Instance().Register(m_AllocatorInfo);
}

/*---------------------------------------------------------------------------*/

void * RTEMem_BlockAllocator::Allocate( SAPDB_ULong BlockCount ) 
{
#ifdef SAPDBMem_USE_SYSTEM_ALLOC
  return malloc(BlockCount * m_BlockSize);
#else
  void * BlockStartAddress;
 
  m_CountAlloc.Increment();

  BlockStartAddress = RTE_ISystem::Instance().AllocSystemPages(BlockCount * m_BlockSize);

  if ( !BlockStartAddress )
  {
#ifdef SAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING
    return (void *)0;
#else
    throw SAPDBMem_BadAllocException(__CONTEXT__, BlockCount * m_BlockSize);
#endif
  }

  m_BytesControlled.Increment(BlockCount * m_BlockSize);

  return BlockStartAddress;
#endif
}

/*---------------------------------------------------------------------------*/

void * RTEMem_BlockAllocator::Allocate( SAPDB_ULong BlockCount, Msg_List &messageList ) 
{
#ifdef SAPDBMem_USE_SYSTEM_ALLOC
  return malloc(BlockCount * m_BlockSize);
#else
  void * BlockStartAddress;
 
  m_CountAlloc.Increment();

  BlockStartAddress = RTE_ISystem::Instance().AllocSystemPages(BlockCount * m_BlockSize, true, messageList);

  if ( !BlockStartAddress )
  {
#ifdef SAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING
    return (void *)0;
#else
    throw SAPDBMem_BadAllocException(__CONTEXT__, BlockCount * m_BlockSize);
#endif
  }

  m_BytesControlled.Increment(BlockCount * m_BlockSize);

  return BlockStartAddress;
#endif
}

/*---------------------------------------------------------------------------*/

void RTEMem_BlockAllocator::Deallocate( void * BlockStartAddress, SAPDB_ULong BlockCount )
{
#ifdef SAPDBMem_USE_SYSTEM_ALLOC
  free(BlockStartAddress);
#else
  m_CountDealloc.Increment();
  m_BytesControlled.Decrement(BlockCount * m_BlockSize);


  if ( !RTE_ISystem::Instance().FreeSystemPages(BlockStartAddress, BlockCount * m_BlockSize) )
  {
    SAPDBERR_ASSERT_STATE(0);
  }
#endif
}

/*---------------------------------------------------------------------------*/

void RTEMem_BlockAllocator::CalcStatistics( SAPDB_ULong &BytesUsed,
                                            SAPDB_ULong &MaxBytesUsed,
                                            SAPDB_ULong &BytesControlled )
{
	BytesUsed       = m_BytesControlled;
    MaxBytesUsed    = m_BytesControlled; //
    BytesControlled = m_BytesControlled;
}

/*---------------------------------------------------------------------------*/

void RTEMem_BlockAllocator::GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc,
                                  SAPDB_ULong &CountDealloc) const
{
    CountAlloc   = m_CountAlloc;
    CountDealloc = m_CountDealloc; 	
}

/*---------------------------------------------------------------------------*/

void RTEMem_BlockAllocator::GetCallStatistics( SAPDB_ULong &CountAlloc,
                                               SAPDB_ULong &CountDealloc ) const
{
    CountAlloc   = m_CountAlloc;
    CountDealloc = m_CountDealloc; 	
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
