/*****************************************************************************/
/*!
  @file         SQLMan_SessionList.cpp
  @author       MartinKi
  @ingroup

  @brief        Implementation file for class SQLMan_SessionList.

\if EMIT_LICENCE
  ========== licence begin  GPL
  Copyright (C) 2005 SAP AG

  This program is free software; you can redisytribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
  ========== licence end
\endif
*/
/*****************************************************************************/

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SQLManager/SQLMan_SessionList.hpp"

#include "SQLManager/SQLMan_Tracing.hpp"
#include "SAPDBCommon/Messages/SDBMsg_SQLMan.h"
#include "Messages/Msg_List.hpp"
#include "Container/Container_List.hpp"
#include "RunTime/Synchronisation/RTESync_SpinlockPool.hpp"
#include "RunTime/Synchronisation/RTESync_RWRegion.hpp"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"

#include "RunTime/RTE_Crash.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

SQLMan_SessionList* SQLMan_SessionList::m_instance = 0;

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/**************************************************************************/

SQLMan_SessionList& SQLMan_SessionList::CreateInstance()
{
    // verify that instance does not exist in locked scope
    static RTESync_Spinlock lock;
    RTESync_LockedScope scope(lock);

    if ( m_instance == 0 )
    {
        m_instance = new(RTEMem_Allocator::Instance()) SQLMan_SessionList;
    }

    return *m_instance;
}

/**************************************************************************/

SQLMan_SessionList::SQLMan_SessionList()
    : m_sessionList(RTEMem_Allocator::Instance()),
      m_spinlockPool(UTF8("SQL_SessionList"), 1),
      m_pRwLock(0)
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_SessionList::SQLMan_SessionList", SQLMan_Trace, 1 );

    m_pRwLock = RTESync_CreateRWRegion(
        1, m_spinlockPool, RTEMem_Allocator::Instance());

    if ( m_pRwLock == 0 ) {
        Msg_List errMsg = Msg_List(
            Msg_List::Error,
            SDBMSG_SQLMAN_NO_MEMORY );
        RTE_Crash( errMsg );
    };
}

/**************************************************************************/

SQLMan_SessionList::~SQLMan_SessionList()
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_SessionList::~SQLMan_SessionList", SQLMan_Trace, 1 );

    if ( m_pRwLock ) {
        RTESync_DestroyRWRegion(
            m_pRwLock, RTEMem_Allocator::Instance());
    }
}

/**************************************************************************/

SQLMan_SessionList::AddReturnCode SQLMan_SessionList::Add(
    SQLMan_Context& sessionContext,
    Msg_List&       errMsg)
{
    SAPDBTRACE_METHOD_DEBUG( "SQLMan_SessionList::Add", SQLMan_Trace, 1 );

    RTESync_RWLockedScope lockedScope( *m_pRwLock, true );

    SessionList::Iterator it = this->Find( sessionContext );
    if ( it.IsValid() ) {
        SAPDBERR_ASSERT_STATE( ! it.IsValid() );
        return arcDuplicate;
    }

    SAPDB_Bool ok = m_sessionList.InsertFront( &sessionContext ).IsValid();
    if ( ! ok ) {
        return arcOutOfMemory;
    }

    return arcOk;
}

/**************************************************************************/

SAPDB_Bool SQLMan_SessionList::Remove(
    SQLMan_Context& sessionContext,
    Msg_List&       errMsg)
{
    SAPDBTRACE_METHOD_DEBUG( "SQLMan_SessionList::Remove", SQLMan_Trace, 1 );

    RTESync_RWLockedScope lockedScope( *m_pRwLock, true );

    SessionList::Iterator it = this->Find( sessionContext );
    if ( ! it.IsValid() ) {
        return false;
    }

    m_sessionList.Delete( it );
    return true;
}

/**************************************************************************/

SQLMan_SessionList::SessionList::Iterator
SQLMan_SessionList::Find(SQLMan_Context& sessionContext)
{
    SAPDBTRACE_METHOD_DEBUG( "SQLMan_SessionList::Find", SQLMan_Trace, 1 );

    SessionList::Iterator it = m_sessionList.Begin();
    for ( SessionList::Iterator itEnd = m_sessionList.End();
          it != itEnd;
          ++it ) {

        if ( *it == &sessionContext ) {
            break;
        }
    }
    return it;
}

/**************************************************************************/

SQLMan_SessionList::Iterator::Iterator(SQLMan_SessionList& parent)
    : m_pParent(&parent)
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_SessionList::Iterator::Iterator", SQLMan_Trace, 1 );

    m_pParent->m_pRwLock->enter( false );
    m_iterator  = m_pParent->m_sessionList.Begin();
    m_advanceIt = false;
}

/**************************************************************************/

SQLMan_SessionList::Iterator::~Iterator()
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_SessionList::Iterator::~Iterator", SQLMan_Trace, 1 );

    if ( m_pParent != 0 ) {
        m_pParent->m_pRwLock->leave( false );
    }
}

/**************************************************************************/

SAPDB_Bool SQLMan_SessionList::Iterator::Next(SQLMan_Context*& sessionContext)
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_SessionList::Iterator::Next", SQLMan_Trace, 1 );

    sessionContext = 0;
    if ( m_advanceIt && m_iterator.IsValid() ) {
        ++m_iterator;
    } else {
        // either first call of Next or we are already beyond the end
        // of the list; in this case setting m_advanceIt does not hurt
        m_advanceIt = true;
    }

    if ( ! m_iterator.IsValid() ) {
        if ( m_pParent != 0 ) {
            // free lock now, don't wait for destructor
            m_pParent->m_pRwLock->leave( false );
            m_pParent = 0;
        }
        return false;
    }

    sessionContext = *m_iterator;

    return true;
}

/**************************************************************************/
