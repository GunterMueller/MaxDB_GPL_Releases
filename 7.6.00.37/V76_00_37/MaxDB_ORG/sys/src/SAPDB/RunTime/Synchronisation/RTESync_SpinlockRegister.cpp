/*!
  @file           RTESync_SpinlockRegister.cpp
  @author         JoergM
  @ingroup        Runtime
  @brief          Spinlock registration

  This class implements a singleton, that allows to register named spinlocks.
  This allows to collect the spinlock statistic information for all registered
  spinlocks.

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
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/Synchronisation/RTESync_SpinlockRegister.hpp"
#include "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

RTESync_SpinlockRegister * RTESync_SpinlockRegister::m_Instance = 0;
const SAPDB_Int4 RTESync_SpinlockRegister::m_MaxIdentfierLength = 40;

RTESync_SpinlockRegister & RTESync_SpinlockRegister::Instance()
{
    SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(RTESync_SpinlockRegister));

    if ( !m_Instance )
    {
#if defined(SAPDB_SLOW)
        m_Instance = new ( Space ) RTESync_SpinlockRegister(true);
#else
        m_Instance = new ( Space ) RTESync_SpinlockRegister(false);
#endif
    }

    return *m_Instance;
}

  /*!
    @brief Retrieve the statistic record pointer for this spinlock

    NOTE that this routine does not lock the spinlock. This should be done
    from outside if necessary !!!!

    @param  mode [in] select the statistic record to replace
    @return current statistic record pointer ( NULL if currently no statistics )
   */
SAPDB_Bool RTESync_SpinlockRegister::GetStatisticInfo(StatisticInfo * &pInfoArray, SAPDB_Int4 &itemCount, RTESync_Spinlock::Mode mode )
{
    SAPDB_Int4 currentCount;
    StatisticInfo *pSnapshotInfoArray = 0;

    while ( 0 == pSnapshotInfoArray )
    {
        currentCount = m_Count; // dirty read... this leads to the necessity of 'while()' loop. See below
        if ( 0 == currentCount )
        {
            return false;
        }

        pSnapshotInfoArray = reinterpret_cast<StatisticInfo *>(RTEMem_RteAllocator::Instance().Allocate(currentCount * sizeof(StatisticInfo)));
        if ( 0 == pSnapshotInfoArray )
        {
            return false;
        }

        memset( pSnapshotInfoArray, 0, currentCount * sizeof(StatisticInfo));

        /* This is using a locked scope... */
        if ( !FillStatisticInfo(pSnapshotInfoArray, currentCount, itemCount, mode) )
        {
            /* FillInfoArray() only reason to fail, is that the most unlikely case happened and */
            /* in the meantime more allocators were registered. This is handled with forcing a loop here... */
            RTEMem_RteAllocator::Instance().Deallocate(pSnapshotInfoArray);
            pSnapshotInfoArray = 0;
        }
    }

    pInfoArray = pSnapshotInfoArray;

    return true;
}

/*!
   Function: FreeStatisticInfo
   Description: Free the snapshot of all statistic informations collected by GetStatisticInfo

   Arguments: pInfoArray[in] Pointer to array of StatisticInfo
 */
void RTESync_SpinlockRegister::FreeStatisticInfo(StatisticInfo *pInfoArray)
{
    RTEMem_RteAllocator::Instance().Deallocate(pInfoArray);
}

SAPDB_Bool RTESync_SpinlockRegister::FillStatisticInfo(StatisticInfo * &pInfoArray, SAPDB_Int4 maxCount, SAPDB_Int4 &itemCount, RTESync_Spinlock::Mode mode )
{
    RTESync_LockedScope lock(Spinlock());

    if ( this->m_Count > maxCount )
    {
        return false;
    }

    StatisticInfo *pFill = pInfoArray;

    for ( RTESync_SpinlockInfo * pInfo = this->m_First; 0 != pInfo; pInfo = pInfo->GetSuccessor() )
    {
        SAPDB_MemCopyNoCheck(&(pFill->StatisticRecord), pInfo->GetItem().GetStatisticRecord(), sizeof(RTE_SpinlockStatistic));
        if( RTESync_Spinlock::reset == mode )
        {
            RTE_SpinlockStatistic statisticsAtReset;
            SAPDB_MemCopyNoCheck(&statisticsAtReset, pInfo->GetItem().GetStatisticRecord(RTESync_Spinlock::reset), sizeof(RTE_SpinlockStatistic));
            pFill->StatisticRecord.locks            -= statisticsAtReset.locks;           /*!< total number of locks */
            pFill->StatisticRecord.collisions       -= statisticsAtReset.collisions;      /*!< total number of collisions */
            pFill->StatisticRecord.totalSpinLoops   -= statisticsAtReset.totalSpinLoops;  /*!< total number of spin loops needed */
            pFill->StatisticRecord.totalYieldLoops  -= statisticsAtReset.totalYieldLoops; /*!< total number of yield loops needed */
            pFill->StatisticRecord.maxSpinLoops     -= statisticsAtReset.maxSpinLoops;    /*!< maximum number of spin loops needed */
            pFill->StatisticRecord.maxYieldLoops    -= statisticsAtReset.maxYieldLoops;   /*!< maximum number of yield loops needed */
            pFill->StatisticRecord.currentLoops     -= statisticsAtReset.currentLoops;    /*!< current number of loops needed (spin loops or yield loops) */
        }
        strncpy((char *)pFill->SpinlockName, (char *)pInfo->GetIdentifier(), RTESync_SpinlockRegister::m_MaxIdentfierLength);

        ++pFill;
    }

    itemCount = this->m_Count;

    return true;
}

void RTESync_SpinlockRegister::ResetAllRegisteredElements(void)
{
    RTESync_LockedScope lock(Spinlock());

    for ( RTESync_SpinlockInfo * pInfo = this->m_First; 0 != pInfo; pInfo = pInfo->GetSuccessor() )
    {
        RTESync_Spinlock *pCurrent = &(pInfo->GetItem());
        pCurrent->Reset();
        
    }
}

