/*!
  @file           SAPDBMem_PseudoAllocator.cpp
  @author         JoergM
  @ingroup        Memory Management
  @brief          Pseudo allocator for providing statistic information about memory consumption

  This implements the contructors for SAPDBMem_PseudoAllocator and SAPDBMem_SynchronizedPseudoAllocator

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

#include "SAPDBCommon/MemoryManagement/SAPDBMem_PseudoAllocator.hpp"
#include "RunTime/MemoryManagement/RTEMem_AllocatorRegister.hpp"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "SAPDBCommon/SAPDB_string.h"

class SAPDBMem_AllocatorInfo
{
public: 
    SAPDBMem_AllocatorInfo( SAPDB_UTF8 const *identifier, SAPDBMem_IRawAllocator *pThis, SAPDB_UTF8 const *baseIdentifier)
        : m_AllocatorInfo(identifier, pThis, baseIdentifier)
    {}
    RTEMem_AllocatorInfo m_AllocatorInfo;
};

SAPDBMem_PseudoAllocator::SAPDBMem_PseudoAllocator( SAPDB_UTF8 const *identifier
                                                  , SAPDB_UTF8 const *baseIdentifier)
: m_pAllocatorInfo(0)
, m_BytesUsed(0)
, m_MaxBytesUsed(0)
, m_BytesControlled(0)
, m_CountAlloc(0)
, m_CountDealloc(0)
, m_CountBaseAlloc(0)
, m_CountBaseDealloc(0)
{
    SAPDB_strncpy_zero_terminated( (SAPDB_Char *)&m_Name[0]
        , (SAPDB_Char const *)(identifier ? identifier : (SAPDB_UTF8 const *)"")
        , sizeof(m_Name) );
    SAPDB_strncpy_zero_terminated( (SAPDB_Char *)&m_BaseName[0]
        , (SAPDB_Char const *)(baseIdentifier ? baseIdentifier : (SAPDB_UTF8 const *)"")
        , sizeof(m_BaseName) );

    m_pAllocatorInfo = new ( RTEMem_Allocator::Instance() ) SAPDBMem_AllocatorInfo(&m_Name[0], this, &m_BaseName[0]);

    RTEMem_AllocatorRegister::Instance().Register(m_pAllocatorInfo->m_AllocatorInfo);
}

SAPDBMem_SynchronizedPseudoAllocator::SAPDBMem_SynchronizedPseudoAllocator( SAPDB_UTF8 const *identifier
                                                                          , SAPDB_UTF8 const *baseIdentifier)
: SAPDBMem_PseudoAllocator(identifier, baseIdentifier)
, m_Lock(identifier)
{
}
