/****************************************************************************/
/*!

  @file        SysView_ReaderWriterLockStatisticsReset.cpp

  -------------------------------------------------------------------------

  @author      Robin

  @ingroup     SystemViews
  @brief       This module implements the "ReaderWriterLockStatisticsReset" view class.

  @see        

*/
/*-------------------------------------------------------------------------

  copyright:    Copyright (c) 2002-2005 SAP AG


    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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




*****************************************************************************/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg00.h"
#include "SystemViews/SysView_Defines.hpp"
#include "SystemViews/SysView_ITableObj.hpp"
#include "SystemViews/SysView_ReaderWriterLockStatisticsReset.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "RunTime/Synchronisation/RTESync_RWRegion.hpp"
#include "SAPDBCommon/SAPDB_sprintf.h"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "RunTime/Synchronisation/RTESync_SpinlockPoolRegister.hpp"
#include "RunTime/RTE_KGS.hpp"
#include "SystemViews/SysView_ReaderWriterLockStatistics.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS  (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

void SysView_ReaderWriterLockStatisticsReset::Create(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_ReaderWriterLockStatisticsReset::Create", SysView_Trace, 5);

	m_Table->AppendCol (ITOCT_TIMESTAMP, SV_RESETDATE, 0);
	m_Table->AppendCol (ITOCT_CHAR,  SV_READERWRITERLOCKADDRESS, 20);
    m_Table->AppendCol (ITOCT_FIXED, SV_EXCLUSIVEACCESSCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED, SV_NONEXCLUSIVEACCESSCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED, SV_COLLISIONCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED, SV_WAITCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED, SV_SPINLOCKCOLLISIONCOUNT, 20);
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_ReaderWriterLockStatisticsReset::GetColCount(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_ReaderWriterLockStatisticsReset::GetColCount", SysView_Trace, 5);

	return SV_CC_READERWRITERLOCKSTATISTICSRESET;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_ReaderWriterLockStatisticsReset::EstimateRows(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_ReaderWriterLockStatisticsReset::EstimateRows", SysView_Trace, 5);

	return SV_ER_READERWRITERLOCKSTATISTICSRESET;
}

/*---------------------------------------------------------------------------*/

void SysView_ReaderWriterLockStatisticsReset::Execute(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_RWRegionStatisticsReset::Execute", SysView_Trace, 5);

    RTESync_IRWRegionReader *pReader;
    RTESync_SpinlockPoolReader  SpinlockPoolReader;

    SAPDB_UInt8             uint8val;

    m_Table->GetCatalogTable();

    if( 0 == SysView_ReaderWriterLockStatistics::m_lastResetTimestamp )   // reset was never executed 
    {
        SysView_ReaderWriterLockStatistics::m_lastResetTimestamp = RTE_KGS::Instance().GetStatistic().timeOfAdminState;
    }

    RTESync_LockedScope lockedScope(RTESync_SpinlockPoolRegister::Instance().Spinlock());   // http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1138984

    if (SpinlockPoolReader.First( RTESync_Spinlock::normal, RTESync_SpinlockPoolRegister::unsynchronized ))   // unsynchronized because we have locked it by ourselves above
    {
        while(SpinlockPoolReader.Next() && (m_Context.IsOk()))
        {
            RTESync_RWRegionRegister *pRWRegionRegister;

            if(SpinlockPoolReader.RWRegionRegister(pRWRegionRegister))  // if the caller supplied us with a spinlock name (to be developed...), it should be compared with the name of the current spinlock here
            {
                pReader = RTESync_GetRWRegionReader(*pRWRegionRegister,RTEMem_Allocator::Instance());

                if (pReader->First(RTESync_IRWRegionReader::reset))
                {
                    while(pReader->Next() && (m_Context.IsOk()))
                    {
                        AddColumn( SysView_ReaderWriterLockStatistics::m_lastResetTimestamp, AsTimestamp);
        
                        pReader->Address(uint8val);
                        SAPDB_ToStringClass address = SAPDB_ToString(uint8val,SAPDB_ToStringClass::hex,SAPDB_ToStringClass::showbase);
                        m_Table->MoveToCol (ITOVT_CHARPTR, (SAPDB_Char *)address,address.Length());

                        pReader->ExclusiveAccessCount(uint8val);
                        m_Table->MoveToCol (ITOVT_INT8, &uint8val, 0);
                
                        pReader->NonExclusiveAccessCount(uint8val);
                        m_Table->MoveToCol (ITOVT_INT8, &uint8val, 0);

                        pReader->CollisionCount(uint8val);
                        m_Table->MoveToCol (ITOVT_INT8, &uint8val, 0);

                        pReader->WaitCount(uint8val);
                        m_Table->MoveToCol (ITOVT_INT8, &uint8val, 0);

                        pReader->SpinLockCollisionCount(uint8val);
                        m_Table->MoveToCol (ITOVT_INT8, &uint8val, 0);
                    }
                }
                (RTEMem_Allocator::Instance()).Deallocate(pReader);
            }
        }
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
