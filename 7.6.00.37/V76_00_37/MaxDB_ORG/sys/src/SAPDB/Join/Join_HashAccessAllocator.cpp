/*****************************************************************************/
/*!
  @file         Join_HashAccessAllocator.cpp
  @author       MartinKi
  @ingroup      Join

  @brief        Header file for class Join_HashAccessAllocator.

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
/*****************************************************************************/

#include "Join/Join_HashAccessAllocator.hpp"
#include "Join/Join_Trace.hpp"
#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp"
#include "RunTime/MemoryManagement/RTEMem_BlockAllocator.hpp"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "RunTime/RTE_Message.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_SynchronizedRawAllocator.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/SAPDB_Singleton.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "KernelCommon/ParameterNames/KernelParam_Join.hpp"
#include "SAPDBCommon/Messages/SDBMsg_Join.h"

/*************************************************************************/

Join_HashAccessAllocator* Join_HashAccessAllocator::m_instance = 0;

/*************************************************************************/

Join_HashAccessAllocator::Join_HashAccessAllocator(
    SAPDBMem_IRawAllocator& allocator)
    : m_allocator(0),
      m_maxSingleTableSize(0),
      m_maxHashTableMemory(0)
{
    SAPDBTRACE_METHOD_DEBUG( "Join_HashAccessAllocator::ctor", Join_Trace, 1 );

    SAPDBErr_MessageList msgList;

    SAPDB_Int8 paramVal = 0;

    if ( RTEConf_ParameterAccess::Instance()->GetInteger(
             KERNELPARAM_JOIN_MAX_SINGLE_HASHTABLE_SIZE,
             paramVal,
             msgList) ) {
        if ( 0 > paramVal ) {
            paramVal = 0;
        }
        m_maxSingleTableSize = static_cast<SAPDB_UInt>(paramVal) * 1024;
    }

    if ( RTEConf_ParameterAccess::Instance()->GetInteger(
             KERNELPARAM_JOIN_MAX_HASHTABLE_MEMORY,
             paramVal,
             msgList) ) {
        if ( 0 > paramVal ) {
            paramVal = 0;
        }
        m_maxHashTableMemory = static_cast<SAPDB_UInt>(paramVal) * 1024;
    }

    if ( (0 != m_maxSingleTableSize)
         && (0 != m_maxHashTableMemory) ) {
        m_allocator = new (RTEMem_Allocator::Instance())
            SAPDBMem_SynchronizedRawAllocator(
                (const SAPDB_UTF8 *) "Join_HashAccessAllocator",
                RTEMem_BlockAllocator::Instance(),
                m_maxSingleTableSize,
                m_maxSingleTableSize,
                SAPDBMem_RawAllocator::FREE_RAW_EXTENDS,
                m_maxHashTableMemory );
    }

    if ( 0 != m_allocator ) {
        m_allocator->DisableOpMessages();
    } else {
        m_maxSingleTableSize = 0;
        m_maxHashTableMemory = 0;
    }
}

/*************************************************************************/

SAPDB_Bool Join_HashAccessAllocator::Create(
    SAPDBMem_IRawAllocator& allocator)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "Join_HashAccessAllocator::Create",
        Join_Trace, 1 );

    if ( m_instance ) {
        return true;
    }

    m_instance = new (allocator) Join_HashAccessAllocator(allocator);
    return 0 != m_instance;
}

/*************************************************************************/

SAPDB_Bool Join_HashAccessAllocator::SetMaxHashTableMemory(
    const SAPDB_UInt newMaxHashTableMemory)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "Join_HashAccessAllocator::SetMaxHashTableMemory",
        Join_Trace, 1 );

    if ( 0 != m_allocator ) {
        SAPDB_ULong newMemoryLimit = newMaxHashTableMemory * 1024;
        SAPDB_ULong bytesUsed;
        SAPDB_ULong maxBytesUsed;
        SAPDB_ULong bytesControlled;
        m_allocator->CalcStatistics( bytesUsed, maxBytesUsed, bytesControlled );
        // this check is dirty and might fail
        if ( bytesControlled > newMemoryLimit ) {
            // raw allocator is unable to shrink while it has
            // allocated more bytes than the new limit allows
            RTE_Message(
                Msg_List(
                    Msg_List::Warning,
                    SDBMSG_JOIN_MAX_HASHTABLE_MEMORY_REDUCTION_FAILED,
                    Msg_Arg( "UINT4", SAPDB_ToString( newMaxHashTableMemory ) )
                    )
                );
        } else {
            m_allocator->SetLimit( newMemoryLimit );
            m_maxHashTableMemory = newMemoryLimit;
        }
        return true;
    } else {
        return false;
    }
}

/*************************************************************************/

SAPDB_Bool Join_HashAccessAllocator::SetMaxHashTableSize(
    const SAPDB_UInt newMaxHashTableSize)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "Join_HashAccessAllocator::SetMaxHashTableSize",
        Join_Trace, 1 );

    if ( 0 == m_allocator ) {
        return false;
    }
    m_maxSingleTableSize = newMaxHashTableSize;
    return true;
}

/*************************************************************************/
