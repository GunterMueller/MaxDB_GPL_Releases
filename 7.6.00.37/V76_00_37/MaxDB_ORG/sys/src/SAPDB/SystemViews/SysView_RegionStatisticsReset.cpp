/****************************************************************************/
/*!

  @file        SysView_RegionStatisticsReset.cpp

  -------------------------------------------------------------------------

  @author      ElkeZ

  @ingroup     SystemViews
  @brief       This module implements the "RegionStatisticsReset" view class.

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
#include "SystemViews/SysView_RegionStatisticsReset.hpp"
#include "geo002.h"
#define KERNEL_LZU
#include "geo50_0.h"
#include "SystemViews/SysView_RegionStatistics.hpp"
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

void SysView_RegionStatisticsReset::Create()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_RegionStatisticsReset::Create", SysView_Trace, 5);

	m_Table->AppendCol (ITOCT_TIMESTAMP ,SV_RESETDATE, 0);
	m_Table->AppendCol (ITOCT_CHAR      ,SV_REGIONNAME, 16);
	m_Table->AppendCol (ITOCT_FIXED     ,SV_ACCESSCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED     ,SV_SPINLOCKCOLLISIONCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED     ,SV_WAITCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED     ,SV_COLLISIONCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED     ,SV_COLLISIONRATE, 10);

}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_RegionStatisticsReset::GetColCount()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_RegionStatisticsReset::GetColCount", SysView_Trace, 5);

    return SV_CC_REGIONSTATISTICSRESET;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_RegionStatisticsReset::EstimateRows()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_RegionStatisticsReset::EstimateRows", SysView_Trace, 5);

    return SV_ER_REGIONSTATISTICSRESET;
}

/*---------------------------------------------------------------------------*/

void SysView_RegionStatisticsReset::Execute()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_RegionStatisticsReset::Execute", SysView_Trace, 5);

    m_Table->GetCatalogTable();
    
    if( 0 == SysView_RegionStatistics::m_lastResetTimestamp )   // reset was never executed 
    {
        SysView_RegionStatistics::m_lastResetTimestamp = RTE_KGS::Instance().GetStatistic().timeOfAdminState;
    }

    for ( SAPDB_UInt index = 0 ; index < XPARAM(ulNoOfRegions) ; index++ )
    {
        // Column 0 SV_RESETDATE
        AddColumn( SysView_RegionStatistics::m_lastResetTimestamp, AsTimestamp);
        PSEM_CTRL_REC   pSemCtrl = FIRST_REGION + index ;
        TASK_CTRL       pOwnerTaskCtrl = OWNER_TASK(pSemCtrl);

        m_Table->MoveToCol (ITOVT_CHARPTR, REGION_NAME(pSemCtrl),(SAPDB_Int)strlen((SAPDB_Char *)REGION_NAME(pSemCtrl))); // REGIONNAME

        SAPDB_UInt4 getCount = REGION_ACCESS(pSemCtrl) - pSemCtrl->getCountAtReset;
        m_Table->MoveToCol (ITOVT_INT4, &getCount, 0);    // ACCESSCOUNT

        SAPDB_UInt4 spinlockCollisionCount = REGION_BEGEXCL_TAS_COLL(pSemCtrl) + REGION_ENDEXCL_TAS_COLL(pSemCtrl)
                                           - (pSemCtrl->begExclTasCountAtReset + pSemCtrl->endExclTasCountAtReset);
        m_Table->MoveToCol (ITOVT_INT4, &spinlockCollisionCount, 0);    // SPINLOCKCOLLISIONCOUNT

        SAPDB_UInt4 waitCount = REGION_WAITS(pSemCtrl) - pSemCtrl->waitCountAtReset;
        m_Table->MoveToCol (ITOVT_INT4, &waitCount, 0);    // WAITCOUNT

        SAPDB_UInt4 collisionCount = REGION_COLLISION(pSemCtrl) - pSemCtrl->collisionCountAtReset;
        m_Table->MoveToCol (ITOVT_INT4, &collisionCount, 0);    // COLLISIONCOUNT

        SAPDB_Int4 Rate = getCount ? (SAPDB_Int4)(100.0 * double(collisionCount) / double(getCount)) : 0;
        m_Table->MoveToCol (ITOVT_INT4, &Rate, 0);    // COLLISIONRATE
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
