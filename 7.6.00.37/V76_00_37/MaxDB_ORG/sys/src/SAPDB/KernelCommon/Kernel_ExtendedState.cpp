/*!
    @file      Kernel_ExtendedState.cpp
    @ingroup   KernelAdministration
    @author    JörgM
    @brief     This defines class Kernel_ExtendedState

    This class allow a representation of a current kernel administration state snapshot

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

// This defines the extended kernel state content
#include "KernelCommon/Kernel_ExtendedState.hpp"
#include "RunTime/System/RTESys_AtomicOperation.hpp"
#include "RunTime/System/RTESys_MemoryBarrier.h"

void Kernel_ExtendedState::PendingPayloadUpdate()
{
    RTESys_AtomicModify(*pPendingUpdateCounter, 1L);
    RTESys_WriteMemoryBarrier();
}

//---

void Kernel_ExtendedState::CompletedPayloadUpdate()
{
    RTESys_WriteMemoryBarrier();
    RTESys_AtomicModify(*pCompletedUpdateCounter, 1L);
}

//---

void Kernel_ExtendedState::UpdateDatabaseFullState(SAPDB_UInt4 const databaseFullState)
{
    Kernel_SharedState & kernelSharedState = GetSharedMemoryPayloadReference();

    RTESys_ReadMemoryBarrier();

    if ( databaseFullState != kernelSharedState.databaseFullState )
    {
        PendingPayloadUpdate();
        {
            kernelSharedState.databaseFullState = databaseFullState;
        }
        CompletedPayloadUpdate();
    }
}

//---

void Kernel_ExtendedState::UpdateLogvolumeFullState(SAPDB_UInt4 const logvolumeFullState)
{
    Kernel_SharedState & kernelSharedState = GetSharedMemoryPayloadReference();

    RTESys_ReadMemoryBarrier();

    if ( logvolumeFullState != kernelSharedState.logvolumeFullState )
    {
        PendingPayloadUpdate();
        {
            kernelSharedState.logvolumeFullState = logvolumeFullState;
        }
        CompletedPayloadUpdate();
    }
}

SAPDB_ULong          *Kernel_ExtendedState::pPendingUpdateCounter;
SAPDB_ULong          *Kernel_ExtendedState::pCompletedUpdateCounter;
Kernel_SharedState   *Kernel_ExtendedState::pSharedState;
