/*****************************************************************************/
/*!
 @file    FileDir_RWRegionWrapper.cpp
 @author  MartinKi
 @ingroup FileDirectory_Files

 @brief Implementation of class FileDir_RWRegionWrapper.

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

#include "FileDirectory/FileDir_RWRegionWrapper.hpp"

#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp"
#include "RunTime/Synchronisation/RTESync_RWRegion.hpp"
#include "RunTime/System/RTESys_AtomicOperation.hpp"
#include "SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp"

#include "Messages/Msg_List.hpp"

#include "KernelCommon/ParameterNames/KernelParam_FileDir.hpp"
#include "KernelCommon/Kernel_Trace.hpp"

#include "FileDirectory/FileDir_Common.hpp"

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

/*************************************************************************/

RTESync_SpinlockPool* FileDir_RWRegionWrapper::m_pool = 0;
SAPDB_Int8 FileDir_RWRegionWrapper::m_lockCounter = 0;

enum {
    c_defaultSpinlockPoolSize = 10  ///< Default size for spinlock pool
};

/*************************************************************************/

bool FileDir_RWRegionWrapper::InitSpinlockPool()
{
    if ( m_pool == 0 ) {
        Msg_List msgList;

        RTEConf_Parameter::Integer spinlockPoolSize =
            c_defaultSpinlockPoolSize;
        if ( RTEConf_ParameterAccess::Instance()->GetInteger(
                 KERNELPARAM_FILEDIR_SPINLOCKPOOL_SIZE,
                 spinlockPoolSize,
                 msgList) ) {
            if ( 0 > spinlockPoolSize ) {
                spinlockPoolSize = c_defaultSpinlockPoolSize;
            }
        }

        m_pool = reinterpret_cast<RTESync_SpinlockPool*>(
            (RTEMem_Allocator::Instance())
            .Allocate(sizeof(RTESync_SpinlockPool)) );
        m_pool = new (m_pool) RTESync_SpinlockPool(
            UTF8("FileDir"), static_cast<SAPDB_UInt>(spinlockPoolSize) );
        msgList.ClearMessageList(); //FIXME: add message list to method interface
    }
    return m_pool != 0;
}

/*************************************************************************/

FileDir_RWRegionWrapper::FileDir_RWRegionWrapper()
    : m_plock( RTESync_CreateRWRegion(
                   RTESys_AtomicModify(
                       m_lockCounter,
                       static_cast<SAPDB_Int8>(1)),
                   *m_pool,
                   RTEMem_Allocator::Instance())),
      m_dummy(false)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_RWRegionWrapper::FileDir_RWRegionWrapper",
        FileDir_Trace, 8 );

    SAPDBERR_ASSERT_STATE( m_pool != 0 );

    if ( m_plock != 0 ) {
        m_lockId = m_plock->getId();
    }
}

/*************************************************************************/

FileDir_RWRegionWrapper::FileDir_RWRegionWrapper(SAPDB_Bool dummyOnly)
    : m_plock(0),
      m_dummy(dummyOnly)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_RWRegionWrapper::FileDir_RWRegionWrapper-1",
        FileDir_Trace, 8 );

    if ( !m_dummy ) {
        m_plock =
            RTESync_CreateRWRegion(
                RTESys_AtomicModify(
                    m_lockCounter,
                    static_cast<SAPDB_Int8>(1)),
                *m_pool,
                RTEMem_Allocator::Instance());
        m_dummy = false;

        SAPDBERR_ASSERT_STATE( m_pool != 0 );

        if ( m_plock != 0 ) {
            m_lockId = m_plock->getId();
        }
    }
}

/*************************************************************************/

FileDir_RWRegionWrapper::~FileDir_RWRegionWrapper()
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_RWRegionWrapper::~FileDir_RWRegionWrapper",
        FileDir_Trace, 8 );

    if ( m_plock != 0 ) {
        RTESync_DestroyRWRegion( m_plock, RTEMem_Allocator::Instance() );
    }
}

/*************************************************************************/

void FileDir_RWRegionWrapper::AcquireReadLock( const RTE_TaskId pid )
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_RWRegionWrapper::AcquireReadLock",
        FileDir_Trace, 6 );

    if ( m_dummy ) {
        return;
    }

    SAPDBERR_ASSERT_STATE( m_plock != 0 );
    SAPDBTRACE_WRITELN( FileDir_Trace, 6, "LockId: " << m_lockId << ", "
                        << m_plock->getId() << " " << ToStr(this) );
    SAPDBERR_ASSERT_STATE( m_lockId == m_plock->getId() );

    m_plock->enter( false, pid );
}

/*************************************************************************/

SAPDB_Bool FileDir_RWRegionWrapper::TryAcquireReadLock(
    const RTE_TaskId pid )
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_RWRegionWrapper::TryAcquireReadLock",
        FileDir_Trace, 6 );

    if ( m_dummy ) {
        return true;
    }

    SAPDBERR_ASSERT_STATE( m_plock != 0 );
    SAPDBTRACE_WRITELN( FileDir_Trace, 6, "LockId: " << m_lockId << ", "
                        << m_plock->getId() << " " << ToStr(this) );
    SAPDBERR_ASSERT_STATE( m_lockId == m_plock->getId() );

    return m_plock->tryEnter( false, pid );
}

/*************************************************************************/

void FileDir_RWRegionWrapper::ReleaseReadLock( const RTE_TaskId pid )
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_RWRegionWrapper::ReleaseReadLock",
        FileDir_Trace, 6 );

    if ( m_dummy ) {
        return;
    }

    SAPDBERR_ASSERT_STATE( m_plock != 0 );
    SAPDBTRACE_WRITELN( FileDir_Trace, 6, "LockId: " << m_lockId << ", "
                        << m_plock->getId() << " " << ToStr(this) );
    SAPDBERR_ASSERT_STATE( m_lockId == m_plock->getId() );

    m_plock->leave( false, pid );
}

/*************************************************************************/

void FileDir_RWRegionWrapper::AcquireWriteLock( const RTE_TaskId pid )
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_RWRegionWrapper::AcquireWriteLock",
        FileDir_Trace, 6 );

    if ( m_dummy ) {
        return;
    }

    SAPDBERR_ASSERT_STATE( m_plock != 0 );
    SAPDBTRACE_WRITELN( FileDir_Trace, 6, "LockId: " << m_lockId << ", "
                        << m_plock->getId() << " " << ToStr(this) );
    SAPDBERR_ASSERT_STATE( m_lockId == m_plock->getId() );

    m_plock->enter( true, pid );
}

/*************************************************************************/

SAPDB_Bool FileDir_RWRegionWrapper::TryAcquireWriteLock(
    const RTE_TaskId pid )
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_RWRegionWrapper::TryAcquireWriteLock",
        FileDir_Trace, 6 );

    if ( m_dummy ) {
        return true;
    }

    SAPDBERR_ASSERT_STATE( m_plock != 0 );
    SAPDBTRACE_WRITELN( FileDir_Trace, 6, "LockId: " << m_lockId << ", "
                        << m_plock->getId() << " " << ToStr(this) );
    SAPDBERR_ASSERT_STATE( m_lockId == m_plock->getId() );

    return m_plock->tryEnter( true, pid );
}

/*************************************************************************/

void FileDir_RWRegionWrapper::ReleaseWriteLock( const RTE_TaskId pid )
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_RWRegionWrapper::ReleaseWriteLock",
        FileDir_Trace, 6 );

    if ( m_dummy ) {
        return;
    }

    SAPDBERR_ASSERT_STATE( m_plock != 0 );
    SAPDBTRACE_WRITELN( FileDir_Trace, 6, "LockId: " << m_lockId << ", "
                        << m_plock->getId() << " " << ToStr(this) );
    SAPDBERR_ASSERT_STATE( m_lockId == m_plock->getId() );

    m_plock->leave( true, pid );
}

/*************************************************************************/

SAPDB_Bool FileDir_RWRegionWrapper::Initialize()
{

    if ( m_dummy ) {
        return true;
    }

    return m_plock != 0;
}

/*************************************************************************/

SAPDB_Bool FileDir_RWRegionWrapper::IsDummy() const
{
    return m_dummy;
}

/*************************************************************************/

void FileDir_RWRegionWrapper::AcquireLock(
    const RTE_TaskId pid,
    SAPDB_Bool exclusive )
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_RWRegionWrapper::AcquireLock",
        FileDir_Trace, 6 );

    if ( m_dummy ) {
        return;
    }

    SAPDBERR_ASSERT_STATE( m_plock != 0 );
    SAPDBTRACE_WRITELN( FileDir_Trace, 6, "LockId: " << m_lockId << ", "
                        << m_plock->getId() << " " << ToStr(this) << NewLine
                        << "exclusive: " << exclusive );
    SAPDBERR_ASSERT_STATE( m_lockId == m_plock->getId() );

    m_plock->enter( exclusive, pid );
}

/*************************************************************************/

void FileDir_RWRegionWrapper::ReleaseLock(
    const RTE_TaskId pid,
    SAPDB_Bool exclusive )
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_RWRegionWrapper::ReleaseWriteLock",
        FileDir_Trace, 6 );

    if ( m_dummy ) {
        return;
    }

    SAPDBERR_ASSERT_STATE( m_plock != 0 );
    SAPDBTRACE_WRITELN( FileDir_Trace, 6, "LockId: " << m_lockId << ", "
                        << m_plock->getId() << " " << ToStr(this) << NewLine
                        << "exclusive: " << exclusive );
    SAPDBERR_ASSERT_STATE( m_lockId == m_plock->getId() );

    m_plock->leave( exclusive, pid );
}

/*************************************************************************/
