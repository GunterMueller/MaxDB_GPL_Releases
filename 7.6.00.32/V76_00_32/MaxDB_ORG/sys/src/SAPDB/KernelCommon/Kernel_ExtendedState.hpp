/*!
    @file      Kernel_ExtendedState.hpp
    @ingroup   KernelAdministration
    @author    JörgM
    @brief     This defines class Kernel_ExtendedState

    This class allow a representation of a current kernel administration state snapshot
*/
/*!
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
#ifndef KERNEL_EXTENDEDSTATE_HPP
#define KERNEL_EXTENDEDSTATE_HPP

// This defines the extended kernel state content
#include "KernelCommon/Kernel_SharedState.h"

/*!
    @class Kernel_ExtendedState
    @brief This class collects methods for access to kernel extended state information

    Kernel state extensions examples:

    void Kernel_ExtendedState::UpdateDatabaseFullState(SAPDB_UInt4 const databaseFullState)
    void Kernel_ExtendedState::UpdateLogvolumeFullState(SAPDB_UInt4 const logvolumeFullState)

    Other functions should update state information using the same method. First issue a RTESys_ReadMemoryBarrier()
    then check if an update is needed at all. If its needed do the update of status must be framed 
    in PendingPayloadUpdate() and CompletePayloadUpdate() calls.

    NOTE: Two independend updates may collide. The correct order of updates is not enforced! The only thing
    that is enforced, is that the update is made consistent visible.
 */
class Kernel_ExtendedState
{
public:
    /*!
       @brief Get a reference to the payload structure contained in shared memory
       
       NOTE This call is meaningfull in kernel only. Client or dbmsrv would be aborted if calling this function.

       The kernel shall use this call to 

       @return reference to payload structure contained in shared memory section
     */
    static Kernel_SharedState & GetSharedMemoryPayloadReference()
    {
        return *pSharedState; 
    }

    /*!
       @brief Notify a pending payload update

       After payload has been been checked for necessary modification (use GetSharedMemoryPayload() to get a reference for it)
       the kernel must call this function to allow external tools to synchronize on consistent status data. Update of
       status must be framed in PendingPayloadUpdate() and CompletePayloadUpdate() calls.

       @returns nothing
     */
    static void PendingPayloadUpdate();

    /*!
       @brief Complete a payload update notification

       After payload modification (use GetSharedMemoryPayload() to get a reference for it) has been completed
       the kernel must call this function to allow external tools to synchronize on consistent status data. Update of
       status must be framed in PendingPayloadUpdate() and CompletePayloadUpdate() calls.

       @returns nothing
     */
    static void CompletedPayloadUpdate();

    /*!
       @brief update function for databaseFullState
       @param databaseFullState [in] the new database volume full state

       Convinience function for kernel developers...

       @returns nothing
     */
    static void UpdateDatabaseFullState(SAPDB_UInt4 const databaseFullState);

    /*!
       @brief update function for logvolumeFullState
       @param logvolumeFullState [in] the new log volume full state

       Convinience function for kernel developers...

       @returns nothing
     */
    static void UpdateLogvolumeFullState(SAPDB_UInt4 const logvolumeFullState);

    static SAPDB_ULong          *pPendingUpdateCounter;   ///< pointer to pending update count in shared memory 
    static SAPDB_ULong          *pCompletedUpdateCounter; ///< pointer to completed update count in shared memory 
    static Kernel_SharedState   *pSharedState; ///< pointer to shared memory status information
}; /*! endclass Kernel_ExtendedState */

#endif  /* KERNEL_EXTENDEDSTATE_HPP */
