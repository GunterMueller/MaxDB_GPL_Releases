/****************************************************************************/
/*!

  @file        SysView_SpinLockStatisticsReset.cpp

  -------------------------------------------------------------------------

  @author      ElkeZ

  @ingroup     SystemViews
  @brief       This module implements the "SpinLockStatisticsReset" view class.

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
#include "SystemViews/SysView_SpinLockStatisticsReset.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "RunTime/Synchronisation/RTESync_SpinlockReader.hpp"
#include "SystemViews/SysView_SpinLockStatistics.hpp"
#include "RunTime/RTE_KGS.hpp"

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

void SysView_SpinLockStatisticsReset::Create()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_SpinLockStatisticsReset::Create", SysView_Trace, 5);

	m_Table->AppendCol (ITOCT_TIMESTAMP, SV_RESETDATE, 0);
	m_Table->AppendCol (ITOCT_CHAR  ,SV_NAME, 40);
	m_Table->AppendCol (ITOCT_FIXED ,SV_LOCKCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_COLLISIONCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_SPINLOOPCOUNT, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_YIELDLOOPCOUNT, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_MAXSPINLOOPCOUNT, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_MAXYIELDLOOPCOUNT, 10);

}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_SpinLockStatisticsReset::GetColCount()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_SpinLockStatisticsReset::GetColCount", SysView_Trace, 5);

    return SV_CC_SPINLOCKSTATISTICSRESET;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_SpinLockStatisticsReset::EstimateRows()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_SpinLockStatisticsReset::EstimateRows", SysView_Trace, 5);

    return SV_ER_SPINLOCKSTATISTICSRESET;
}

/*---------------------------------------------------------------------------*/

void SysView_SpinLockStatisticsReset::Execute()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_SpinLockStatisticsReset::Execute", SysView_Trace, 5);
    RTESync_SpinlockReader  reader;

    SAPDB_UTF8              name[44];
    SAPDB_Int8              int8val;
    SAPDB_Int4              int4val;
    
    m_Table->GetCatalogTable();

    if( 0 == SysView_SpinLockStatistics::m_lastResetTimestamp )   // reset was never executed 
    {
        SysView_SpinLockStatistics::m_lastResetTimestamp = RTE_KGS::Instance().GetStatistic().timeOfAdminState;
    }
    
    if (reader.First(RTESync_Spinlock::reset))
    {
    
        while(reader.Next() && (m_Context.IsOk()))
        {
            AddColumn( SysView_SpinLockStatistics::m_lastResetTimestamp, AsTimestamp);

            reader.Name (name);
            m_Table->MoveToCol (ITOVT_CHARPTR, name, (SAPDB_Int)strlen((SAPDB_Char *)name));
            
            reader.LockCount (int8val);
            m_Table->MoveToCol (ITOVT_INT8, &int8val, 0);
 
            reader.CollisionCount (int8val);
            m_Table->MoveToCol (ITOVT_INT8, &int8val, 0);

            reader.SpinLoopCount (int4val);
            m_Table->MoveToCol (ITOVT_INT4, &int4val, 0);

            reader.YieldLoopCount (int4val);
            m_Table->MoveToCol (ITOVT_INT4, &int4val, 0);

            reader.MaxSpinLoopCount (int4val);
            m_Table->MoveToCol (ITOVT_INT4, &int4val, 0);

            reader.MaxYieldLoopCount (int4val);
            m_Table->MoveToCol (ITOVT_INT4, &int4val, 0);
        }
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
