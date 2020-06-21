/*!
  @file           RTESync_SpinlockPool.cpp
  @author         RobinW
  @ingroup        Runtime
  @brief          realizes an 'array' of named spinlooks which cab be used for striping


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

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/Synchronisation/RTESync_SpinlockPool.hpp"
#include "RunTime/Synchronisation/RTESync_NamedSpinlock.hpp"
#include "RunTime/Synchronisation/RTESync_SpinlockPoolRegister.hpp"
#include "RunTime/RTE_Message.hpp"
#include "RunTime/RTE_MessageList.hpp"
#include "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"
#include "SAPDB/SAPDBCommon/SAPDB_sprintf.h"
#include "SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"


#if defined(ALPHA)
#define SAFETY_OFFSET_ SAPDB_CACHELINE_SIZE
#else
#define SAFETY_OFFSET_ 0
#endif

/*
  @class RTESync_SpinlockPool
 */
RTESync_SpinlockPool::RTESync_SpinlockPool( const SAPDB_UTF8 *poolIdentifier,
                                                    SAPDB_Int4 numberOfElements,
                                                    SAPDBMem_IRawAllocator &allocator)
    : m_numberOfElements(numberOfElements)
    , m_allocator(allocator)
    , m_info(0,0)   // must be done this way, because we do not know "this" at this point
    , m_RWRegionRegister
#if defined(SAPDB_SLOW)
            (true)
#else
            (false)
#endif

{
    strncpy((char *)&m_Name[0], (const char *)poolIdentifier, sizeof(m_Name)-1);
    m_Name[sizeof(m_Name)-1] = 0;

    m_pool = (RTESync_NamedSpinlock *)((SAPDB_Byte *)m_allocator.Allocate(sizeof(RTESync_NamedSpinlock) * m_numberOfElements + 2 * SAFETY_OFFSET_)+SAFETY_OFFSET_);
    if ( !m_pool )
    {
        RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_ALLOCATION_FAILED,"Spinlock Pool" ));
    }

    for( SAPDB_Int4 i=0 ; i<m_numberOfElements ; i++)
    {
        if(0 != poolIdentifier[0])
        {
            SAPDB_UTF8 myName[40+1];
            SAPDB_sprintf((SAPDB_Char *)myName, sizeof(myName), "%s%d", poolIdentifier, i);
            new(&m_pool[i])RTESync_NamedSpinlock(myName);
        }
        else
        {
            new(&m_pool[i])RTESync_Spinlock();
        }
    }
    if(0 != poolIdentifier[0])
    {
        new ( ((void *)&m_info) ) RTE_ItemRegister<RTESync_SpinlockPool>::Info ( (const SAPDB_UTF8 *)&m_Name[0], this );
        RTESync_SpinlockPoolRegister::Instance().Register(m_info);
    }
}

/*---------------------------------------------------------------------------*/

RTESync_SpinlockPool::~RTESync_SpinlockPool()
{
    if(m_Name[0])
    {
        RTESync_SpinlockPoolRegister::Instance().Deregister(m_info);
        for( SAPDB_Int4 i=0 ; i<m_numberOfElements ; i++)
        {
            m_pool[i].~RTESync_NamedSpinlock();
        }
    }
    m_allocator.Deallocate(m_pool);
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_SpinlockPool::Register(RTE_ItemRegister<RTESync_IRWRegion>::Info &itemInfo)
{
    return m_RWRegionRegister.Register(itemInfo);
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_SpinlockPool::Deregister(RTE_ItemRegister<RTESync_IRWRegion>::Info &itemInfo)
{
    return m_RWRegionRegister.Deregister(itemInfo);
}

/*---------------------------------------------------------------------------*/

void RTESync_SpinlockPool::Reset(void)
{
    for( SAPDB_Int4 i=0; i < m_numberOfElements; i++ )
    {
        m_pool[i].Reset();
    }
}

/*---------------------------------------------------------------------------*/
