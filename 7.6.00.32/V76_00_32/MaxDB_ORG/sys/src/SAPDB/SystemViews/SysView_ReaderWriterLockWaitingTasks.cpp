/****************************************************************************/
/*!

  @file        SysView_ReaderWriterLockWaitingTasks.cpp

  -------------------------------------------------------------------------

  @author      Robin

  @ingroup     SystemViews
  @brief       This module implements the "ReaderWriterLockWaitingTasks" view class.

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
#include "SystemViews/SysView_ReaderWriterLockWaitingTasks.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "RunTime/Synchronisation/RTESync_RWRegion.hpp"
#include "SAPDBCommon/SAPDB_sprintf.h"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "RunTime/Synchronisation/RTESync_SpinlockPoolRegister.hpp"

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

void SysView_ReaderWriterLockWaitingTasks::Create(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_ReaderWriterLockWaitingTasks::Create", SysView_Trace, 5);

	m_Table->AppendCol (ITOCT_CHAR, SV_READERWRITERLOCKADDRESS, 20);

    m_Table->AppendCol (ITOCT_FIXED,  SV_WAITINGTASKID, 10);
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_ReaderWriterLockWaitingTasks::GetColCount(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_ReaderWriterLockWaitingTasks::GetColCount", SysView_Trace, 5);

	return SV_CC_READERWRITERLOCKWAITINGTASKS;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_ReaderWriterLockWaitingTasks::EstimateRows(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_ReaderWriterLockStatistics::EstimateRows", SysView_Trace, 5);

	return SV_ER_READERWRITERLOCKWAITINGTASKS;
}

/*---------------------------------------------------------------------------*/

void SysView_ReaderWriterLockWaitingTasks::Execute(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_ReaderWriterLockWaitingTasks::Execute", SysView_Trace, 5);

    RTESync_IRWRegionReader *pReader;
    RTESync_SpinlockPoolReader  SpinlockPoolReader;
    SAPDB_UInt8             currentRWRegionAddress;

    m_Table->GetCatalogTable();
        
    RTESync_LockedScope lockedScope(RTESync_SpinlockPoolRegister::Instance().Spinlock());   // http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1138984

    if (SpinlockPoolReader.First( RTESync_Spinlock::normal, RTESync_SpinlockPoolRegister::unsynchronized ))   // unsynchronized because we have locked it by ourselves above
    {
        while(SpinlockPoolReader.Next() && (m_Context.IsOk()))
        {
            RTESync_RWRegionRegister *pRWRegionRegister;

            if(SpinlockPoolReader.RWRegionRegister(pRWRegionRegister))  // if the caller supplied us with a spinlock name (to be developed...), it should be compared with the name of the current spinlock here
            {
                pReader = RTESync_GetRWRegionReader(*pRWRegionRegister,RTEMem_Allocator::Instance());

                if (pReader->First())
                {
                    while(pReader->Next() && (m_Context.IsOk()))
                    {
                        pReader->Address(currentRWRegionAddress);
                        SAPDB_ToStringClass address = SAPDB_ToString(currentRWRegionAddress,SAPDB_ToStringClass::hex,SAPDB_ToStringClass::showbase);

                        SAPDB_Int4 waitingTaskId;
                        
                        if(pReader->GetNextWaitingTask(waitingTaskId))
                        {
                            do
                            {
                                m_Table->MoveToCol (ITOVT_CHARPTR, (SAPDB_Char *)address,address.Length());
                                m_Table->MoveToCol (ITOVT_INT4, &waitingTaskId, 0);
                            }
                            while(pReader->GetNextWaitingTask(waitingTaskId));
                        }
                    }
                }
                (RTEMem_Allocator::Instance()).Deallocate(pReader);
            }
        }
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool SysView_ReaderWriterLockWaitingTasks::ResetCounter(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_ReaderWriterLockWaitingTasks::ResetCounter", SysView_Trace, 5);

    m_Context.ThrowError (e_not_implemented, 2);
    return ( true );
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
