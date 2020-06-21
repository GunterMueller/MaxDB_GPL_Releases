/*****************************************************************************/
/*!
  @file    FileDir_PointerCache.cpp
  @author  MartinKi
  @ingroup FileDirectory

  @brief Implementation of session specific FileDir_Generic smart pointer caching

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

#include "FileDirectory/FileDir_Generic.hpp"
#include "FileDirectory/FileDir_Table.hpp"
#include "FileDirectory/FileDir_Index.hpp"
#include "FileDirectory/FileDir_ShortColumn.hpp"
#include "FileDirectory/FileDir_Oms.hpp"
#include "FileDirectory/FileDir_OmsKey.hpp"
#include "FileDirectory/FileDir_OmsKeyPartition.hpp"
#include "FileDirectory/FileDir_OmsVar.hpp"
#include "FileDirectory/FileDir_OmsFixed.hpp"

#include "FileDirectory/FileDir_PointerCache.hpp"
#include "FileDirectory/FileDir_Common.hpp"

#include "DataAccess/Data_FileTypes.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"

#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"

#include "RunTime/RTE_Crash.hpp"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/
/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/

/*===========================================================================*
 *  PROCEDURES to Create or Destroy FileDir_PointerCache classes ...             *
 *===========================================================================*/

/**************************************************************************/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/**************************************************************************/

FileDir_PointerCache::FileDir_PointerCache(
    SAPDBMem_IRawAllocator& alloc)
    : m_FDirChangeStamp(0),
      m_alloc(alloc),
      m_pointerHash(alloc)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "FileDir_PointerCache::FileDir_PointerCache",
        FileDir_Trace, 5);
}

/**************************************************************************/

FileDir_PointerCache::~FileDir_PointerCache()
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "FileDir_PointerCache::~FileDir_PointerCache",
        FileDir_Trace, 5);

    this->Clear();
}

/*************************************************************************/

FileDir_ReturnCode FileDir_PointerCache::AddEntryToCache(
    FileDir_Generic& p)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "FileDir_PointerCache::AddFileToCache",
        FileDir_Trace, 5);

    if ( ! p.IsAssigned() ) {
        return FileDir_FileNoNotFound;
    }

    FileDir_Generic* filePointer;
    switch ( p.GetFileType() ) {
    case Data_TableFile:
        filePointer = new (m_alloc) FileDir_Table();
        break;
    case Data_IndexFile:
        filePointer = new (m_alloc) FileDir_Index();
        break;
    case Data_ShortColumnFile:
        filePointer = new (m_alloc) FileDir_ShortColumn();
        break;
    case Data_OmsFile:
        filePointer = new (m_alloc) FileDir_Oms();
        break;
    case Data_OmsFixedFile:
        filePointer = new (m_alloc) FileDir_OmsFixed();
        break;
    case Data_OmsKeyFile:
        filePointer = new (m_alloc) FileDir_OmsKey();
        break;
    case Data_OmsKeyPartitionFile:
        filePointer = new (m_alloc) FileDir_OmsKeyPartition();
        break;
    case Data_OmsVarFile:
        filePointer = new (m_alloc) FileDir_OmsVar();
        break;
    default:
        RTE_Crash(
            SAPDBErr_Exception(
                __CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED,
                "each entry type must be listed") );
    }
    if ( filePointer == 0 ) {
        return FileDir_OutOfMemory;
    }

    FilePtr_Hash::Iterator it = m_pointerHash.Insert(
        p.GetFileNo(),
        filePointer );
    if ( ! it.IsValid() ) {
        destroy( filePointer, m_alloc );
        return FileDir_FileNoExists;
    }

    FileDir_Generic::TransferReference( *filePointer, p );
    filePointer->SetAllocator( m_alloc );

    p.AssignSecondary( *filePointer );

    return FileDir_Okay;
}

/**************************************************************************/

void FileDir_PointerCache::RemoveEntryFromCache(
    const FileDir_FileNo& fileNo)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "FileDir_PointerCache::RemoveEntryFromCache", FileDir_Trace, 5);

    FilePtr_Hash::Iterator iter = m_pointerHash.Find( fileNo );
    if ( iter.IsValid() ) {
        FileDir_Generic* p = iter->value;
        m_pointerHash.Delete( iter );
        p->SetSelfDestruct();
    }
}

/**************************************************************************/

void FileDir_PointerCache::Clear()
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "FileDir_PointerCache::Clear",
        FileDir_Trace, 5);

    FilePtr_Hash::Iterator iter = m_pointerHash.Begin();
    FilePtr_Hash::Iterator end  = m_pointerHash.End();
    while ( iter != end )
    {
        FileDir_Generic* p = iter->value;
        m_pointerHash.Delete( iter );
        p->SetSelfDestruct();
    }
}

/*************************************************************************/

void FileDir_PointerCache::CleanUpCache(const SAPDB_Int changeStamp)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "FileDir_PointerCache::CleanUpCache",
        FileDir_Trace, 5);

    if ( FileDir_Trace.TracesLevel( 5 ) ) {
        Kernel_VTrace()
            << "FileDir_PointerCache::CleanUpCache" << NewLine
            << "  changeStamp: " << changeStamp
            << ", own stamp: " << m_FDirChangeStamp;
    }

    if ( m_FDirChangeStamp != changeStamp ) {
        FilePtr_Hash::Iterator iter = m_pointerHash.Begin();
        FilePtr_Hash::Iterator end  = m_pointerHash.End();
        while ( iter.IsValid() && (iter != end) )
        {
            if ( Data_FileDeleted == iter->value->GetFileState() ) {
                FileDir_Generic* p = iter->value;
                m_pointerHash.Delete( iter );
                p->SetSelfDestruct();
            } else {
                ++iter;
            }
        }
        m_FDirChangeStamp = changeStamp;
    }
}

/**************************************************************************/

void FileDir_PointerCache::SetAllLockStates(const SAPDB_Int lockState)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "FileDir_PointerCache::SetAllLockStates",
        FileDir_Trace, 5);

    FilePtr_Hash::Iterator it = m_pointerHash.Begin();
    for ( FilePtr_Hash::Iterator itEnd  = m_pointerHash.End();
          it != itEnd;
          ++it ) {

        it->value->SetLockState( lockState );
    }
}

/**************************************************************************/
