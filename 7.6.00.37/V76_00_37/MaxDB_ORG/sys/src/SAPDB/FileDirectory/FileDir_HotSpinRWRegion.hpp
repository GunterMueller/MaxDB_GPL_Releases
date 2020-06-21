/*****************************************************************************/
/*!
  @file         FileDir_HotSpinRWRegion.hpp
  @author       MartinKi
  @ingroup      FileDirectory_Files

  @brief        Header file for class FileDir_HotSpinRWRegion.

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

#ifndef FILEDIR_HOTSPINRWREGION_HPP
#define FILEDIR_HOTSPINRWREGION_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_Types.hpp"

#include "SAPDBCommon/SAPDB_Types.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/
/*!
 * @class FileDir_HotSpinRWRegion
 * @author MartinKi
 *
 * Implementation of reader/writer locks for which blocked
 * reader/writer do not sleep but hot spin.
 */
class FileDir_HotSpinRWRegion
{
    int m_state;

public:
    /// Constructor.
    FileDir_HotSpinRWRegion();

    /// Constructs a dummy lock that does not do anything.
    FileDir_HotSpinRWRegion(SAPDB_Bool dummyOnly);

    /// Returns true if lock was initialized successfully.
    SAPDB_Bool Initialize();

    /// Returns true if lock is a dummy and does not do any locking.
    SAPDB_Bool IsDummy() const;

    /// Acquires a read lock.
    void AcquireReadLock(const RTE_TaskId = 0);

    /// Releases a read lock.
    void ReleaseReadLock(const RTE_TaskId = 0);

    /// Acquires a write lock.
    void AcquireWriteLock(const RTE_TaskId = 0);

    /// Releases a write lock.
    void ReleaseWriteLock(const RTE_TaskId = 0);
};

#endif // FILEDIR_HOTSPINRWREGION_HPP
