/*****************************************************************************/
/*!

  @file         FileDir_BaseEntry.cpp
  @author       MartinKi
  @ingroup      FileDirectory_Files

  @brief        Implementation file for class FileDir_BaseEntry.

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

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "FileDirectory/FileDir_Common.hpp"
#include "FileDirectory/FileDir_Types.hpp"

#include "RunTime/System/RTESys_AtomicOperation.hpp"
#include "RunTime/RTE_Crash.hpp"
#include "RunTime/RTE_Types.hpp"

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

#include "FileDirectory/FileDir_BaseEntry.hpp"
#include "ggg91.h"

/**************************************************************************/

FileDir_BaseEntry::FileDir_BaseEntry(SAPDBMem_IRawAllocator& allocator)
    : m_allocator(allocator),
      m_refCounter(0),
//       m_cacheHitCounter(0),
//       m_cacheMissCounter(0),
      m_commentPtr(0)
{}

/**************************************************************************/

FileDir_BaseEntry::~FileDir_BaseEntry()
{
    if ( m_refCounter != 0 ) {
        RTE_Crash(
            SAPDBErr_Exception(
                __CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                "refCounter not null on destroy") );
    }
}

/**************************************************************************/

void FileDir_BaseEntry::incRefCounter()
{
//     if ( this->IsLazyDeleteSet() ) {
//         RTE_Crash(
//             SAPDBErr_Exception(
//                 __CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
//                 "LazyDelete must not be set when increasing ref counter") );
//     }
    RTESys_AtomicModify( m_refCounter, 1 );
}

/**************************************************************************/

SAPDB_Int FileDir_BaseEntry::decRefCounter()
{
    SAPDBERR_ASSERT_STATE( m_refCounter > 0 );

    return RTESys_AtomicModify( m_refCounter, -1 );
}

/**************************************************************************/

SAPDB_Int FileDir_BaseEntry::GetRefCount() const
{
    return m_refCounter;
}

/**************************************************************************/

SAPDBMem_IRawAllocator& FileDir_BaseEntry::GetAllocator()
{
    return m_allocator;
}

/**************************************************************************/

FileDir_Node* FileDir_BaseEntry::GetNodePtr() const
{
    return &m_node;
}

/**************************************************************************/

void FileDir_BaseEntry::IncrementCacheHits()
{
//     RTESys_AtomicModify(
//         m_cacheHitCounter,
//         static_cast<CacheAccessCounter>(1) );
}

/**************************************************************************/

void FileDir_BaseEntry::IncrementCacheMisses()
{
//     RTESys_AtomicModify(
//         m_cacheMissCounter,
//         static_cast<CacheAccessCounter>(1) );
}

/**************************************************************************/

SAPDB_Real8 FileDir_BaseEntry::GetCacheHitrate() const
{
//     SAPDB_Int8 cacheAccesses = m_cacheHitCounter + m_cacheMissCounter;
//     return
//         cacheAccesses > 0
//         ?
//         (static_cast<SAPDB_Real8>(m_cacheHitCounter) / cacheAccesses * 100.0)
//         :
//         100.0;
    return 0;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_BaseEntry::Initialize()
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_BaseEntry::Initialize",
                             FileDir_Trace, 7 );
    return FileDir_Okay;
}

/**************************************************************************/

FileDir_ITable* FileDir_BaseEntry::AsTable()
{
    RTE_Crash(
        SAPDBErr_Exception(
            __CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
            "entry must be of type FileDir_(Temp)TableEntry") );
    return 0;
}

/**************************************************************************/

FileDir_IIndex* FileDir_BaseEntry::AsIndex()
{
    RTE_Crash(
        SAPDBErr_Exception(
            __CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
            "entry must be of type FileDir_(Temp)IndexEntry") );
    return 0;
}

/**************************************************************************/

FileDir_IShortColumn* FileDir_BaseEntry::AsShortColumn()
{
    RTE_Crash(
        SAPDBErr_Exception(
            __CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
            "entry must be of type FileDir_ShortColumnEntry") );
    return 0;
}

/**************************************************************************/

FileDir_IOms*   FileDir_BaseEntry::AsOms()
{
    RTE_Crash(
        SAPDBErr_Exception(
            __CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
            "entry must be of type FileDir_OmsEntry") );
    return 0;
}

/**************************************************************************/

FileDir_IOmsKey* FileDir_BaseEntry::AsOmsKey()
{
    RTE_Crash(
        SAPDBErr_Exception(
            __CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
            "entry must be of type FileDir_OmsKeyEntry") );
    return 0;
}

/**************************************************************************/

FileDir_IOmsKeyPartition* FileDir_BaseEntry::AsOmsKeyPartition()
{
    RTE_Crash(
        SAPDBErr_Exception(
            __CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
            "entry must be of type FileDir_OmsKeyPartitionEntry") );
    return 0;
}

/**************************************************************************/

FileDir_IOmsFixed* FileDir_BaseEntry::AsOmsFixed()
{
    RTE_Crash(
        SAPDBErr_Exception(
            __CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
            "entry must be of type FileDir_OmsFixedEntry") );
    return 0;
}

/**************************************************************************/

FileDir_IOmsVar* FileDir_BaseEntry::AsOmsVar()
{
    RTE_Crash(
        SAPDBErr_Exception(
            __CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
            "entry must be of type FileDir_OmsVarEntry") );
    return 0;
}

/**************************************************************************/

void FileDir_BaseEntry::SetComment(const char* commentPtr)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_BaseEntry::SetComment",
                             FileDir_Trace, 5 );
    m_commentPtr = commentPtr;
}

/**************************************************************************/

const char* FileDir_BaseEntry::GetComment() const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_BaseEntry::GetComment",
                             FileDir_Trace, 5 );
    return m_commentPtr;
}

/**************************************************************************/

void FileDir_BaseEntry::GetFileVersion(tgg91_FileVersion& fileVersion) const
{
    fileVersion.gg91SetDummyVers();
}

/**************************************************************************/
