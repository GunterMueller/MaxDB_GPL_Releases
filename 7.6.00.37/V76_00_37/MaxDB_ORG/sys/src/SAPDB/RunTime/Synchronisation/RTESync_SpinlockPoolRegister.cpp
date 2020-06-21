/*!
  @file           RTESync_SpinlockPoolRegister.cpp
  @author         RobinW
  @ingroup        Runtime
  @brief          critical regions that support exclusive ("write") and 
                  non-exclusive ("read") access - implementation of the 
                  RTESync_IRWRegion interface


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

#include "RunTime/Synchronisation/RTESync_SpinlockPoolRegister.hpp"
#include "SAPDB/RunTime/Synchronisation/RTESync_RWRegion.hpp"
#include "SAPDB/RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

RTESync_SpinlockPoolRegister & RTESync_SpinlockPoolRegister::Instance( )
{
    SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(RTESync_SpinlockPoolRegister));

    if ( !m_Instance )
    {
#if defined (SAPDB_SLOW)
        m_Instance = new ( Space ) RTESync_SpinlockPoolRegister(true);
#else
        m_Instance = new ( Space ) RTESync_SpinlockPoolRegister(false);
#endif
    }

    return *m_Instance;
}

/*!
    Function: GetStatisticInfo
    Description: Get a snapshot of all statistic informations
    Arguments: pInfoArray[out] Pointer to array of StatisticInfo

    Return value: True if info array fill, false if not not enough memory
    */
SAPDB_Bool RTESync_SpinlockPoolRegister::GetStatisticInfo(StatisticInfo * &pInfoArray, SAPDB_Int4 &itemCount, RTESync_Spinlock::Mode mode, RTESync_SpinlockPoolRegister::AccessSynchronization access )
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
        if ( !FillStatisticInfo(pSnapshotInfoArray, currentCount, itemCount, mode, access ) )
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
/*!
   Function: FreeStatisticInfo
   Description: Free the snapshot of all statistic informations collected by GetStatisticInfo

   Arguments: pInfoArray[in] Pointer to array of StatisticInfo
 */
void RTESync_SpinlockPoolRegister::FreeStatisticInfo(StatisticInfo *pInfoArray)
{
    RTEMem_RteAllocator::Instance().Deallocate(pInfoArray);
}

SAPDB_Bool RTESync_SpinlockPoolRegister:: FillStatisticInfo(StatisticInfo * &pInfoArray, SAPDB_Int4 maxCount, SAPDB_Int4 &itemCount, RTESync_Spinlock::Mode mode, RTESync_SpinlockPoolRegister::AccessSynchronization access )
{
#ifndef SAPDB_CONSOLE
    if( synchronized == access )
        Spinlock().Lock();
#endif

    if ( this->m_Count > maxCount )
    {
#ifndef SAPDB_CONSOLE
    if( synchronized == access )
            Spinlock().Unlock();
#endif
        return false;
    }

    StatisticInfo *pFill = pInfoArray;

    for ( RTE_ItemRegister<RTESync_SpinlockPool>::Info * pInfo = this->m_First; 0 != pInfo; pInfo = pInfo->GetSuccessor() )
    {
        strncpy((char *)pFill->PoolName, (char *)pInfo->GetIdentifier(), RTESync_SpinlockPoolRegister::m_MaxIdentfierLength);
        pFill->pRWRegionRegister = &(pInfo->GetItem().m_RWRegionRegister);

        pFill->NumberOfSpinlocks = pInfo->GetItem().m_numberOfElements;
        pFill->Minimum.locks=SAPDB_MAX_UINT8/2;
        pFill->Maximum.locks=0;
        pFill->Minimum.collisions=SAPDB_MAX_UINT8/2;
        pFill->Maximum.collisions=0;
        pFill->Minimum.totalSpinLoops=SAPDB_MAX_INT4;
        pFill->Maximum.totalSpinLoops=0;
        pFill->Minimum.totalYieldLoops=SAPDB_MAX_INT4;
        pFill->Maximum.totalYieldLoops=0;
        pFill->Minimum.maxSpinLoops=SAPDB_MAX_INT4;
        pFill->Maximum.maxSpinLoops=0;
        pFill->Minimum.maxYieldLoops=SAPDB_MAX_INT4;
        pFill->Maximum.maxYieldLoops=0;
        pFill->Minimum.currentLoops=SAPDB_MAX_INT4;
        pFill->Maximum.currentLoops=0;
        SAPDB_Int8 int8val;
        SAPDB_Int4 int4val;
        SAPDB_Int8 totalLocks=0;
        SAPDB_Int8 totalCollisions=0;
        SAPDB_Int4 totalTotalSpinLoops=0;
        SAPDB_Int4 totalTotalYieldLoops=0;
        SAPDB_Int4 totalMaxSpinLoops=0;
        SAPDB_Int4 totalMaxYieldLoops=0;
        SAPDB_Int4 totalCurrentLoops=0;
        for(int i=0;i < pInfo->GetItem().m_numberOfElements;i++)
        {
            int8val = pInfo->GetItem().m_pool[i].GetStatisticRecord()->locks;
            if(RTESync_Spinlock::reset == mode)
                int8val -= pInfo->GetItem().m_pool[i].GetStatisticRecord(RTESync_Spinlock::reset)->locks;
            if(int8val < pFill->Minimum.locks)
                 pFill->Minimum.locks = int8val;
            if(int8val > pFill->Maximum.locks)
                pFill->Maximum.locks = int8val;
            totalLocks += int8val;

            int8val = pInfo->GetItem().m_pool[i].GetStatisticRecord()->collisions;
            if(RTESync_Spinlock::reset == mode)
                int8val -= pInfo->GetItem().m_pool[i].GetStatisticRecord(RTESync_Spinlock::reset)->collisions;
            if(int8val < pFill->Minimum.collisions)
                 pFill->Minimum.collisions = int8val;
            if(int8val > pFill->Maximum.collisions)
                pFill->Maximum.collisions = int8val;
            totalCollisions += int8val;

            int4val = pInfo->GetItem().m_pool[i].GetStatisticRecord()->totalSpinLoops;
            if(RTESync_Spinlock::reset == mode)
                int4val -= pInfo->GetItem().m_pool[i].GetStatisticRecord(RTESync_Spinlock::reset)->totalSpinLoops;
            if(int4val < pFill->Minimum.totalSpinLoops)
                 pFill->Minimum.totalSpinLoops = int4val;
            if(int4val > pFill->Maximum.totalSpinLoops)
                pFill->Maximum.totalSpinLoops = int4val;
            totalTotalSpinLoops += int4val;

            int4val = pInfo->GetItem().m_pool[i].GetStatisticRecord()->totalYieldLoops;
            if(RTESync_Spinlock::reset == mode)
                int4val -= pInfo->GetItem().m_pool[i].GetStatisticRecord(RTESync_Spinlock::reset)->totalYieldLoops;
            if(int4val < pFill->Minimum.totalYieldLoops)
                 pFill->Minimum.totalYieldLoops = int4val;
            if(int4val > pFill->Maximum.totalYieldLoops)
                pFill->Maximum.totalYieldLoops = int4val;
            totalTotalYieldLoops += int4val;

            int4val = pInfo->GetItem().m_pool[i].GetStatisticRecord()->maxYieldLoops;
            if(RTESync_Spinlock::reset == mode)
                int4val -= pInfo->GetItem().m_pool[i].GetStatisticRecord(RTESync_Spinlock::reset)->maxYieldLoops;
            if(int4val < pFill->Minimum.maxYieldLoops)
                 pFill->Minimum.maxYieldLoops = int4val;
            if(int4val > pFill->Maximum.maxYieldLoops)
                pFill->Maximum.maxYieldLoops = int4val;
            totalMaxYieldLoops += int4val;

            int4val = pInfo->GetItem().m_pool[i].GetStatisticRecord()->maxSpinLoops;
            if(RTESync_Spinlock::reset == mode)
                int4val -= pInfo->GetItem().m_pool[i].GetStatisticRecord(RTESync_Spinlock::reset)->maxSpinLoops;
            if(int4val < pFill->Minimum.maxSpinLoops)
                 pFill->Minimum.maxSpinLoops = int4val;
            if(int4val > pFill->Maximum.maxSpinLoops)
                pFill->Maximum.maxSpinLoops = int4val;
            totalMaxSpinLoops += int4val;

            int4val = pInfo->GetItem().m_pool[i].GetStatisticRecord()->currentLoops;
            if(RTESync_Spinlock::reset == mode)
                int4val -= pInfo->GetItem().m_pool[i].GetStatisticRecord(RTESync_Spinlock::reset)->currentLoops;
            if(int4val < pFill->Minimum.currentLoops)
                 pFill->Minimum.currentLoops = int4val;
            if(int4val > pFill->Maximum.currentLoops)
                pFill->Maximum.currentLoops = int4val;
            totalCurrentLoops += int4val;
        }
        SAPDB_Int4 elementCount         = pInfo->GetItem().m_numberOfElements;
        pFill->Average.locks            = totalLocks / elementCount;
        pFill->Average.collisions       = totalCollisions / elementCount;
        pFill->Average.totalSpinLoops   = totalTotalSpinLoops / elementCount;
        pFill->Average.totalYieldLoops  = totalTotalYieldLoops / elementCount;
        pFill->Average.maxSpinLoops     = totalMaxSpinLoops / elementCount;
        pFill->Average.maxYieldLoops    = totalMaxYieldLoops / elementCount;
        pFill->Average.currentLoops     = totalCurrentLoops / elementCount;

        ++pFill;
    }

    itemCount = this->m_Count;

#ifndef SAPDB_CONSOLE
    if( synchronized == access )
        Spinlock().Unlock();
#endif
    return true;
        
}

void RTESync_SpinlockPoolRegister::ResetAllRegisteredElements(void)
{
    RTESync_LockedScope lock(Spinlock());

    for ( RTE_ItemRegister<RTESync_SpinlockPool>::Info * pInfo = this->m_First; 0 != pInfo; pInfo = pInfo->GetSuccessor() )
    {
        RTESync_SpinlockPool *pCurrent = &(pInfo->GetItem());
        pCurrent->Reset();
        
    }
}
   
RTESync_SpinlockPoolRegister * RTESync_SpinlockPoolRegister::m_Instance = 0;
const SAPDB_Int4 RTESync_SpinlockPoolRegister::m_MaxIdentfierLength = 40;


/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_SpinlockPoolReader::First(RTESync_Spinlock::Mode mode, RTESync_SpinlockPoolRegister::AccessSynchronization access )
{
    if ( RTESync_SpinlockPoolRegister::Instance().GetStatisticInfo(m_pInfoArray, m_Elements, mode, access ) )
    {
        m_Current = -1;
        return (true);
    }
    else
    {
        return (false);
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_SpinlockPoolReader::Next()
{
    m_Current ++;

    if ((!m_pInfoArray) || (m_Current >= m_Elements))
    {
        FreeArray();
        return (false);
    }
    else
    {
        return (true);
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_SpinlockPoolReader::Name (SAPDB_UTF8 name[])
{
    if ((m_pInfoArray) && (m_Current < m_Elements))
    {
        strcpy((SAPDB_Char *)name, (SAPDB_Char *)m_pInfoArray[m_Current].PoolName);
        return (true);
    }
    else
    {
        return (true);
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_SpinlockPoolReader::NumberOfElements (SAPDB_Int4 &numberOfElements)
{
    if ((m_pInfoArray) && (m_Current < m_Elements))
    {
        numberOfElements = m_pInfoArray[m_Current].NumberOfSpinlocks;
        return (true);
    }
    else
    {
        return (true);
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_SpinlockPoolReader::MaxLocks (SAPDB_UInt8 &maxLockCount)
{
    if ((m_pInfoArray) && (m_Current < m_Elements))
    {
        maxLockCount = m_pInfoArray[m_Current].Maximum.locks;
        return (true);
    }
    else
    {
        return (true);
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_SpinlockPoolReader::MinLocks (SAPDB_UInt8 &minLockCount)
{
    if ((m_pInfoArray) && (m_Current < m_Elements))
    {
        minLockCount = m_pInfoArray[m_Current].Minimum.locks;
        return (true);
    }
    else
    {
        return (true);
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_SpinlockPoolReader::AvgLocks (SAPDB_UInt8 &avgLockCount)
{
    if ((m_pInfoArray) && (m_Current < m_Elements))
    {
        avgLockCount = m_pInfoArray[m_Current].Average.locks;
        return (true);
    }
    else
    {
        return (true);
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_SpinlockPoolReader::MaxCollisions (SAPDB_UInt8 &maxCollisions)
{
    if ((m_pInfoArray) && (m_Current < m_Elements))
    {
        maxCollisions = m_pInfoArray[m_Current].Maximum.collisions;
        return (true);
    }
    else
    {
        return (true);
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_SpinlockPoolReader::MinCollisions (SAPDB_UInt8 &minCollisions)
{
    if ((m_pInfoArray) && (m_Current < m_Elements))
    {
        minCollisions = m_pInfoArray[m_Current].Minimum.collisions;
        return (true);
    }
    else
    {
        return (true);
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_SpinlockPoolReader::AvgCollisions (SAPDB_UInt8 &avgCollisions)
{
    if ((m_pInfoArray) && (m_Current < m_Elements))
    {
        avgCollisions = m_pInfoArray[m_Current].Average.collisions;
        return (true);
    }
    else
    {
        return (true);
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_SpinlockPoolReader::MaxTotalSpinLoops (SAPDB_UInt8 &maxTotalSpinLoops)
{
    if ((m_pInfoArray) && (m_Current < m_Elements))
    {
        maxTotalSpinLoops = m_pInfoArray[m_Current].Maximum.totalSpinLoops;
        return (true);
    }
    else
    {
        return (true);
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_SpinlockPoolReader::MinTotalSpinLoops (SAPDB_UInt8 &minTotalSpinLoops)
{
    if ((m_pInfoArray) && (m_Current < m_Elements))
    {
        minTotalSpinLoops = m_pInfoArray[m_Current].Minimum.totalSpinLoops;
        return (true);
    }
    else
    {
        return (true);
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_SpinlockPoolReader::AvgTotalSpinLoops (SAPDB_UInt8 &avgTotalSpinLoops)
{
    if ((m_pInfoArray) && (m_Current < m_Elements))
    {
        avgTotalSpinLoops = m_pInfoArray[m_Current].Average.totalSpinLoops;
        return (true);
    }
    else
    {
        return (true);
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_SpinlockPoolReader::MaxTotalYieldLoops (SAPDB_UInt8 &maxTotalYieldLoops)
{
    if ((m_pInfoArray) && (m_Current < m_Elements))
    {
        maxTotalYieldLoops = m_pInfoArray[m_Current].Maximum.totalYieldLoops;
        return (true);
    }
    else
    {
        return (true);
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_SpinlockPoolReader::MinTotalYieldLoops (SAPDB_UInt8 &minTotalYieldLoops)
{
    if ((m_pInfoArray) && (m_Current < m_Elements))
    {
        minTotalYieldLoops = m_pInfoArray[m_Current].Minimum.totalYieldLoops;
        return (true);
    }
    else
    {
        return (true);
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_SpinlockPoolReader::AvgTotalYieldLoops (SAPDB_UInt8 &avgTotalYieldLoops)
{
    if ((m_pInfoArray) && (m_Current < m_Elements))
    {
        avgTotalYieldLoops = m_pInfoArray[m_Current].Average.totalYieldLoops;
        return (true);
    }
    else
    {
        return (true);
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_SpinlockPoolReader::MaxMaxSpinLoops (SAPDB_UInt8 &maxMaxSpinLoops)
{
    if ((m_pInfoArray) && (m_Current < m_Elements))
    {
        maxMaxSpinLoops = m_pInfoArray[m_Current].Maximum.maxSpinLoops;
        return (true);
    }
    else
    {
        return (true);
    }
}
/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_SpinlockPoolReader::MinMaxSpinLoops (SAPDB_UInt8 &minMaxSpinLoops)
{
    if ((m_pInfoArray) && (m_Current < m_Elements))
    {
        minMaxSpinLoops = m_pInfoArray[m_Current].Minimum.maxSpinLoops;
        return (true);
    }
    else
    {
        return (true);
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_SpinlockPoolReader::AvgMaxSpinLoops (SAPDB_UInt8 &avgMaxSpinLoops)
{
    if ((m_pInfoArray) && (m_Current < m_Elements))
    {
        avgMaxSpinLoops = m_pInfoArray[m_Current].Average.maxSpinLoops;
        return (true);
    }
    else
    {
        return (true);
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_SpinlockPoolReader::MaxMaxYieldLoops (SAPDB_UInt8 &maxMaxYieldLoops)
{
    if ((m_pInfoArray) && (m_Current < m_Elements))
    {
        maxMaxYieldLoops = m_pInfoArray[m_Current].Maximum.maxYieldLoops;
        return (true);
    }
    else
    {
        return (true);
    }
}
/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_SpinlockPoolReader::MinMaxYieldLoops (SAPDB_UInt8 &minMaxYieldLoops)
{
    if ((m_pInfoArray) && (m_Current < m_Elements))
    {
        minMaxYieldLoops = m_pInfoArray[m_Current].Minimum.maxYieldLoops;
        return (true);
    }
    else
    {
        return (true);
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_SpinlockPoolReader::AvgMaxYieldLoops (SAPDB_UInt8 &avgMaxYieldLoops)
{
    if ((m_pInfoArray) && (m_Current < m_Elements))
    {
        avgMaxYieldLoops = m_pInfoArray[m_Current].Average.maxYieldLoops;
        return (true);
    }
    else
    {
        return (true);
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_SpinlockPoolReader::MaxCurrentLoops (SAPDB_UInt8 &maxCurrentLoops)
{
    if ((m_pInfoArray) && (m_Current < m_Elements))
    {
        maxCurrentLoops = m_pInfoArray[m_Current].Maximum.currentLoops;
        return (true);
    }
    else
    {
        return (true);
    }
}
/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_SpinlockPoolReader::MinCurrentLoops (SAPDB_UInt8 &minCurrentLoops)
{
    if ((m_pInfoArray) && (m_Current < m_Elements))
    {
        minCurrentLoops = m_pInfoArray[m_Current].Minimum.currentLoops;
        return (true);
    }
    else
    {
        return (true);
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_SpinlockPoolReader::AvgCurrentLoops (SAPDB_UInt8 &avgCurrentLoops)
{
    if ((m_pInfoArray) && (m_Current < m_Elements))
    {
        avgCurrentLoops = m_pInfoArray[m_Current].Average.currentLoops;
        return (true);
    }
    else
    {
        return (true);
    }
}

SAPDB_Bool RTESync_SpinlockPoolReader::RWRegionRegister(RTESync_RWRegionRegister *&pRWRegionRegister)
{
    if ((m_pInfoArray) && (m_Current < m_Elements))
    {
        pRWRegionRegister = m_pInfoArray[m_Current].pRWRegionRegister;
        return (true);
    }
    else
    {
        return (true);
    }
}

/*---------------------------------------------------------------------------*/

RTESync_SpinlockPoolReader::RTESync_SpinlockPoolReader()
{
    m_pInfoArray  = NULL;
    m_Elements    = 0;
    m_Current     = 0;
}

/*---------------------------------------------------------------------------*/

RTESync_SpinlockPoolReader::~RTESync_SpinlockPoolReader()
{
    FreeArray();
}

/*---------------------------------------------------------------------------*/

void RTESync_SpinlockPoolReader::FreeArray()
{
    if (m_pInfoArray)
    {
        RTESync_SpinlockPoolRegister::Instance().FreeStatisticInfo(m_pInfoArray);
        m_pInfoArray  = NULL;
        m_Elements    = 0;
        m_Current     = 0;
    }
}

/*===========================================================================*
 *  EXPORTED FUNCTIONS, EXPORTED CLASS METHODS (IMPLEMENTATION)              *
 *===========================================================================*/

