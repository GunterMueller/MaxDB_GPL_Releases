/****************************************************************************/
/*!

  @file        SysView_SpinLockPoolStatisticsReset.cpp

  -------------------------------------------------------------------------

  @author      Robin

  @ingroup     SystemViews
  @brief       This module implements the "SpinLockPoolStatisticsReset" view class.

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
#include "SystemViews/SysView_SpinLockPoolStatisticsReset.hpp"
#include "RunTime/Synchronisation/RTESync_SpinlockPoolRegister.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "RunTime/RTE_KGS.hpp"
#include "SystemViews/SysView_SpinLockPoolStatistics.hpp"


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

void SysView_SpinLockPoolStatisticsReset::Create(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_SpinLockPoolStatisticsReset::Create", SysView_Trace, 5);

	m_Table->AppendCol (ITOCT_TIMESTAMP, SV_RESETDATE, 0);
	m_Table->AppendCol (ITOCT_CHAR, SV_SPINLOCKPOOLNAME, 40);
    m_Table->AppendCol (ITOCT_FIXED, SV_MAXLOCKCOUNT, 20);
    m_Table->AppendCol (ITOCT_FIXED, SV_MINLOCKCOUNT, 20);
    m_Table->AppendCol (ITOCT_FIXED, SV_AVGLOCKCOUNT, 20);
    m_Table->AppendCol (ITOCT_FIXED, SV_MAXCOLLISIONCOUNT, 20);
    m_Table->AppendCol (ITOCT_FIXED, SV_MINCOLLISIONCOUNT, 20);
    m_Table->AppendCol (ITOCT_FIXED, SV_AVGCOLLISIONCOUNT, 20);
    m_Table->AppendCol (ITOCT_FIXED, SV_MAXTOTALSPINLOOPCOUNT, 20);
    m_Table->AppendCol (ITOCT_FIXED, SV_MINTOTALSPINLOOPCOUNT, 20);
    m_Table->AppendCol (ITOCT_FIXED, SV_AVGTOTALSPINLOOPCOUNT, 20);
    m_Table->AppendCol (ITOCT_FIXED, SV_MAXTOTALYIELDLOOPCOUNT, 20);
    m_Table->AppendCol (ITOCT_FIXED, SV_MINTOTALYIELDLOOPCOUNT, 20);
    m_Table->AppendCol (ITOCT_FIXED, SV_AVGTOTALYIELDLOOPCOUNT, 20);
    m_Table->AppendCol (ITOCT_FIXED, SV_MAXMAXSPINLOOPCOUNT, 20);
    m_Table->AppendCol (ITOCT_FIXED, SV_MINMAXSPINLOOPCOUNT, 20);
    m_Table->AppendCol (ITOCT_FIXED, SV_AVGMAXSPINLOOPCOUNT, 20);
    m_Table->AppendCol (ITOCT_FIXED, SV_MAXMAXYIELDLOOPCOUNT, 20);
    m_Table->AppendCol (ITOCT_FIXED, SV_MINMAXYIELDLOOPCOUNT, 20);
    m_Table->AppendCol (ITOCT_FIXED, SV_AVGMAXYIELDLOOPCOUNT, 20);
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_SpinLockPoolStatisticsReset::GetColCount(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_SpinLockPoolStatisticsReset::GetColCount", SysView_Trace, 5);

	return SV_CC_SPINLOCKPOOLSTATISTICSRESET;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_SpinLockPoolStatisticsReset::EstimateRows(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_SpinLockPoolStatisticsReset::EstimateRows", SysView_Trace, 5);

	return SV_ER_SPINLOCKPOOLSTATISTICSRESET;
}

/*---------------------------------------------------------------------------*/

void SysView_SpinLockPoolStatisticsReset::Execute(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_SpinLockPoolStatisticsReset::Execute", SysView_Trace, 5);

    RTESync_SpinlockPoolReader  reader;

    SAPDB_UTF8              name[44];
    SAPDB_UInt8              int8val;
    
    m_Table->GetCatalogTable();
    
    if( 0 == SysView_SpinLockPoolStatistics::m_lastResetTimestamp )   // reset was never executed 
    {
        SysView_SpinLockPoolStatistics::m_lastResetTimestamp = RTE_KGS::Instance().GetStatistic().timeOfAdminState;
    }
    
    if (reader.First(RTESync_Spinlock::reset))
    {
    
        while(reader.Next() && (m_Context.IsOk()))
        {
            AddColumn( SysView_SpinLockPoolStatistics::m_lastResetTimestamp, AsTimestamp);
        
            reader.Name (name);
            m_Table->MoveToCol (ITOVT_CHARPTR, name, (SAPDB_Int)strlen((SAPDB_Char *)name));
            
            reader.MaxLocks (int8val);
            m_Table->MoveToCol (ITOVT_INT8, &int8val, 0);

            reader.MinLocks (int8val);
            m_Table->MoveToCol (ITOVT_INT8, &int8val, 0);

            reader.AvgLocks (int8val);
            m_Table->MoveToCol (ITOVT_INT8, &int8val, 0);

            reader.MaxCollisions (int8val);
            m_Table->MoveToCol (ITOVT_INT8, &int8val, 0);

            reader.MinCollisions (int8val);
            m_Table->MoveToCol (ITOVT_INT8, &int8val, 0);

            reader.AvgCollisions (int8val);
            m_Table->MoveToCol (ITOVT_INT8, &int8val, 0);

            reader.MaxTotalSpinLoops (int8val);
            m_Table->MoveToCol (ITOVT_INT8, &int8val, 0);

            reader.MinTotalSpinLoops (int8val);
            m_Table->MoveToCol (ITOVT_INT8, &int8val, 0);

            reader.AvgTotalSpinLoops (int8val);
            m_Table->MoveToCol (ITOVT_INT8, &int8val, 0);

            reader.MaxTotalYieldLoops (int8val);
            m_Table->MoveToCol (ITOVT_INT8, &int8val, 0);

            reader.MinTotalYieldLoops (int8val);
            m_Table->MoveToCol (ITOVT_INT8, &int8val, 0);

            reader.AvgTotalYieldLoops (int8val);
            m_Table->MoveToCol (ITOVT_INT8, &int8val, 0);

            reader.MaxMaxSpinLoops (int8val);
            m_Table->MoveToCol (ITOVT_INT8, &int8val, 0);

            reader.MinMaxSpinLoops (int8val);
            m_Table->MoveToCol (ITOVT_INT8, &int8val, 0);

            reader.AvgMaxSpinLoops (int8val);
            m_Table->MoveToCol (ITOVT_INT8, &int8val, 0);

            reader.MaxMaxYieldLoops (int8val);
            m_Table->MoveToCol (ITOVT_INT8, &int8val, 0);

            reader.MinMaxYieldLoops (int8val);
            m_Table->MoveToCol (ITOVT_INT8, &int8val, 0);

            reader.AvgMaxYieldLoops (int8val);
            m_Table->MoveToCol (ITOVT_INT8, &int8val, 0);
        }
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
