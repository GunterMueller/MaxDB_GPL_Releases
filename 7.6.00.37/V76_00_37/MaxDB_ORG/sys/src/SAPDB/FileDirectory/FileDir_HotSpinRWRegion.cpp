/*****************************************************************************/
/*!
  @file         FileDir_HotSpinRWRegion.cpp
  @author       MartinKi
  @ingroup      FileDirectory_Files

  @brief        Header file for class .

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

#include "FileDirectory/FileDir_HotSpinRWRegion.hpp"


#include "RunTime/System/RTESys_MemoryBarrier.h"
#include "RunTime/System/RTESys_AtomicOperation.hpp"
#include "RunTime/RTE_Types.hpp"

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*************************************************************************/

FileDir_HotSpinRWRegion::FileDir_HotSpinRWRegion()
    : m_state(0)
{}

/*************************************************************************/

FileDir_HotSpinRWRegion::FileDir_HotSpinRWRegion(SAPDB_Bool dummyOnly)
    : m_state(0)
{}

/*************************************************************************/

SAPDB_Bool FileDir_HotSpinRWRegion::Initialize()
{
    return SAPDB_TRUE;
}

/*************************************************************************/

SAPDB_Bool FileDir_HotSpinRWRegion::IsDummy() const
{
    return SAPDB_FALSE;
}

/*************************************************************************/

void FileDir_HotSpinRWRegion::AcquireReadLock(const RTE_TaskId)
{
    bool gotLock = false;
    do {
        int state = RTESys_AtomicRead( m_state );
        if ( state >= 0 ) {
            gotLock = RTESys_CompareAndExchange(
                m_state, state, state+1, state);
        }
    } while ( !gotLock );
    RTESys_ReadMemoryBarrier();
}

/*************************************************************************/

void FileDir_HotSpinRWRegion::ReleaseReadLock(const RTE_TaskId)
{
    bool done = false;
    do {
        SAPDBERR_ASSERT_STATE( m_state > 0 );
        int state = RTESys_AtomicRead( m_state );
        done = RTESys_CompareAndExchange( m_state, state, state-1, state);
    } while ( !done );
}

/*************************************************************************/

void FileDir_HotSpinRWRegion::AcquireWriteLock(const RTE_TaskId)
{
    bool gotLock = false;
    do {
        int state = RTESys_AtomicRead( m_state );
        if ( state == 0 ) {
            gotLock = RTESys_CompareAndExchange(
                m_state, state, state-1, state);
        }
    } while ( !gotLock );
    RTESys_ReadMemoryBarrier();
}

/*************************************************************************/

void FileDir_HotSpinRWRegion::ReleaseWriteLock(const RTE_TaskId)
{
    bool done = false;
    do {
        SAPDBERR_ASSERT_STATE( m_state == -1 );
        int state = RTESys_AtomicRead( m_state );
        done = RTESys_CompareAndExchange( m_state, state, state+1, state);
    } while ( !done );
    RTESys_WriteMemoryBarrier();
}

/*************************************************************************/
