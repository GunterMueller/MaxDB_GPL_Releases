/*!
  @file           RTEDiag_SymbolResolutionLock.cpp
  @author         JoergM
  @ingroup        RunTime
  @brief          Symbol resolution lock interface

  The symbol resolution lock is a well known lock used to synchronize symbol resolution
  between different threads. It is special in such a way it must be resettable by a signal
  handler to allow to 'force' usage in a crash situation...

  This module is used in KernelRuntime only...

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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
/*===========================================================================*
 *  PROTOTYPES ...                                                           *
 *===========================================================================*/

#define KERNEL_LZU
#include "RunTime/Diagnose/RTEDiag_SymbolResolutionLock.h"
#include "RunTime/Synchronisation/RTESync_NamedSpinlock.hpp"
#include "RunTime/System/RTESys_Spinlock.h"
#include "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "geo002.h"
#include "geo50_0.h"

/* Spinlock used to interlock stack backtrace requests */
static RTE_Lock RTEDiag_LockForSymbolResolution;
static RTESync_NamedSpinlock *RTEDiag_PointerToNamedLockForSymbolResolution = 0;
static SAPDB_UInt8 RTEDiag_CounterForSymbolResolutionLock = 0;
static SAPDB_UInt8 *RTEDiag_PointerToCounterForSymbolResolutionLock = &RTEDiag_CounterForSymbolResolutionLock;

/*!
  @brief call to check if symbol resolution is switched off
  @return true if switched off, false if symbol resolution switch on
 */
extern "C" SAPDB_Bool RTEDiag_SymbolResolutionAllowed()
{
    SAPDB_Bool allowSymbolResolution = true;

#if defined(_WIN32)
    if ( kgs.XParam )
    {
        allowSymbolResolution = ( false != kgs.XParam->allowSymbolResolution );
    }
#else
    if ( kgs && kgs->XParam )
    {
        allowSymbolResolution = ( false != kgs->XParam->allowSymbolResolution );
    }
#endif

    return allowSymbolResolution;
}

/*!
  @brief return number of symbol resolution lock calls up to now

  This information is used by the x_cons show total command. For more statistic information
  you should use the system view SPINLOCKSTATISTICS for spinlock named "SYMBOL_RESOLUTION".

  @return number of symbol resolution lock calls
 */
SAPDB_UInt8 RTEDiag_SymbolResolutionLockCalls()
{
    return (*RTEDiag_PointerToCounterForSymbolResolutionLock);
}

SAPDB_Bool RTEDiag_CreatedRegisteredSymbolResolutionLock()
{
    RTEDiag_PointerToNamedLockForSymbolResolution = new ( RTEMem_RteAllocator::Instance() ) 
        RTESync_NamedSpinlock( (const SAPDB_UTF8 *)"SYMBOL_RESOLUTION"
                             , &RTEDiag_LockForSymbolResolution
                             , false
                             , false );

    KGS->symbolResolutionLockCount = RTEDiag_CounterForSymbolResolutionLock;
    RTEDiag_PointerToCounterForSymbolResolutionLock = &(KGS->symbolResolutionLockCount);

    return 0 != RTEDiag_PointerToNamedLockForSymbolResolution;
}

extern "C" SAPDB_Bool RTEDiag_SymbolResolutionInit(volatile int *pEverCalled, void **pAnchor)
{
    if ( !*pEverCalled )
    {
        RTESys_InitLock(&RTEDiag_LockForSymbolResolution);
        *pEverCalled = 1;
    }
    else
    {
        /* since all calls of eo670_CTraceStackInit are protected via eo670_TraceEverCalled,
           this must wanted reinit... */
        if ( RTESys_TestAndLock(&RTEDiag_LockForSymbolResolution) )
        {
            *pAnchor = 0; /* reset module list anchor too */
            RTESys_InitLock(&RTEDiag_LockForSymbolResolution);
            return false;
        }
        else
        {
            RTESys_Unlock(&RTEDiag_LockForSymbolResolution);
        }
    }
    return true;
}

/*!
  @brief lock the symbol resolution lock
 */
extern "C" void RTEDiag_SymbolResolutionLock()
{
    if ( RTEDiag_PointerToNamedLockForSymbolResolution )
    {
        RTEDiag_PointerToNamedLockForSymbolResolution->Lock();
    }
    else
    {
        if ( RTESys_TestAndLock(&RTEDiag_LockForSymbolResolution) )
        {
            while ( RTESys_TestAndLock(&RTEDiag_LockForSymbolResolution) )
            {
                RTESys_GiveUpTimeSlice();
            }
        }
    }
    ++(*RTEDiag_PointerToCounterForSymbolResolutionLock);
}

/*!
  @brief unlock the symbol resolution lock
 */
extern "C" void RTEDiag_SymbolResolutionUnlock()
{
    if ( RTEDiag_PointerToNamedLockForSymbolResolution )
    {
        RTEDiag_PointerToNamedLockForSymbolResolution->Unlock();
    }
    else
    {
        RTESys_Unlock(&RTEDiag_LockForSymbolResolution);
    }
}
