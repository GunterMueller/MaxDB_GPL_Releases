/****************************************************************************/
/*!

  @file        SysView_SpinLockStatistics.cpp

  -------------------------------------------------------------------------

  @author      ElkeZ

  @ingroup     SystemViews
  @brief       This module implements the "SpinLockStatistics" view class.

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
#include "SystemViews/SysView_SpinLockStatistics.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "RunTime/Synchronisation/RTESync_SpinlockReader.hpp"
#include "RunTime/System/RTESys_MicroTime.h"


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

void SysView_SpinLockStatistics::Create(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_SpinLockStatistics::Create", SysView_Trace, 5);

	m_Table->AppendCol (ITOCT_CHAR, SV_NAME, 40);
	m_Table->AppendCol (ITOCT_FIXED, SV_LOCKCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED, SV_COLLISIONCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED, SV_SPINLOOPCOUNT, 10);
	m_Table->AppendCol (ITOCT_FIXED, SV_YIELDLOOPCOUNT, 10);
	m_Table->AppendCol (ITOCT_FIXED, SV_MAXSPINLOOPCOUNT, 10);
	m_Table->AppendCol (ITOCT_FIXED, SV_MAXYIELDLOOPCOUNT, 10);

}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_SpinLockStatistics::GetColCount(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_SpinLockStatistics::GetColCount", SysView_Trace, 5);

	return SV_CC_SPINLOCKSTATISTICS;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_SpinLockStatistics::EstimateRows(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_SpinLockStatistics::EstimateRows", SysView_Trace, 5);

	return SV_ER_SPINLOCKSTATISTICS;
}

/*---------------------------------------------------------------------------*/

void SysView_SpinLockStatistics::Execute(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_SpinLockStatistics::Execute", SysView_Trace, 5);

    RTESync_SpinlockReader  reader;

    SAPDB_UTF8              name[44];
    SAPDB_Int8              int8val;
    SAPDB_Int4              int4val;
    
    m_Table->GetCatalogTable();
    
    if (reader.First())
    {
    
        while(reader.Next() && (m_Context.IsOk()))
        {
            reader.Name (name);
            m_Table->MoveToCol (ITOVT_CHARPTR, name, strlen((SAPDB_Char *)name));
            
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

/*---------------------------------------------------------------------------*/

SAPDB_Bool SysView_SpinLockStatistics::ResetCounter(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_SpinLockStatistics::ResetCounter", SysView_Trace, 5);

    (RTESync_SpinlockRegister::Instance()).ResetAllRegisteredElements();

    m_lastResetTimestamp = RTESys_MicroSecondTime();

    return ( true );
}
   
SAPDB_UInt8 SysView_SpinLockStatistics::m_lastResetTimestamp = 0;

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
