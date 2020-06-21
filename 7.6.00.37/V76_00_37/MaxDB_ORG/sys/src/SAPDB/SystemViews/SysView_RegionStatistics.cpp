/****************************************************************************/
/*!

  @file        SysView_RegionStatistics.cpp

  -------------------------------------------------------------------------

  @author      ElkeZ

  @ingroup     SystemViews
  @brief       This module implements the "RegionStatistics" view class.

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
#include "SystemViews/SysView_RegionStatistics.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "geo002.h"
#define KERNEL_LZU
#include "geo50_0.h"
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

void SysView_RegionStatistics::Create()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_RegionStatistics::Create", SysView_Trace, 5);

	m_Table->AppendCol (ITOCT_CHAR ,SV_REGIONNAME, 16);
	m_Table->AppendCol (ITOCT_FIXED ,SV_REGIONID, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_HOLDERTASKID, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_REQUESTORCOUNT, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_ACCESSCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_SPINLOCKCOLLISIONCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_WAITCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_COLLISIONCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_COLLISIONRATE, 10);

}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_RegionStatistics::GetColCount()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_RegionStatistics::GetColCount", SysView_Trace, 5);

    return SV_CC_REGIONSTATISTICS;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_RegionStatistics::EstimateRows()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_RegionStatistics::EstimateRows", SysView_Trace, 5);

    return SV_ER_REGIONSTATISTICS;
}

/*---------------------------------------------------------------------------*/

void SysView_RegionStatistics::Execute()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_RegionStatistics::Execute", SysView_Trace, 5);

    m_Table->GetCatalogTable();
    
    for ( SAPDB_UInt index = 0 ; index < XPARAM(ulNoOfRegions) ; index++ )
    {
        PSEM_CTRL_REC   pSemCtrl = FIRST_REGION + index ;
        TASK_CTRL       pOwnerTaskCtrl = OWNER_TASK(pSemCtrl);

        m_Table->MoveToCol (ITOVT_CHARPTR, REGION_NAME(pSemCtrl),(SAPDB_Int)strlen((SAPDB_Char *)REGION_NAME(pSemCtrl))); // REGIONNAME

        SAPDB_Int4 regionId = index+1;
        m_Table->MoveToCol (ITOVT_INT4, &regionId, 0);    // REGIOIND

        SAPDB_Int4 waiters = 0;
        if( pOwnerTaskCtrl )
        {
            m_Table->MoveToCol (ITOVT_INT4, &TASK_INDEX(pOwnerTaskCtrl), 0);    // HOLDERTASKID
            for ( PSEM_QUEUE_REC pSemQueElem   =  LAST_ELEM(WAIT_LIST(pSemCtrl));
                    pSemQueElem && WAIT_TASK(pSemQueElem) ;
                    pSemQueElem =  NEXT_ELEM(pSemQueElem) )
            {
                waiters++;
            }
        }
        else
        {
            m_Table->MoveToCol(ITOVT_NULL, NULL, 0);    // HOLDERTASKID
        }

        m_Table->MoveToCol (ITOVT_INT4, &waiters, 0);    // REQUESTORCOUNT

        m_Table->MoveToCol (ITOVT_INT4, &REGION_ACCESS(pSemCtrl), 0);    // ACCESSCOUNT

        SAPDB_UInt4 spinlockCollisionCount = REGION_BEGEXCL_TAS_COLL(pSemCtrl) + REGION_ENDEXCL_TAS_COLL(pSemCtrl);
        m_Table->MoveToCol (ITOVT_INT4, &spinlockCollisionCount, 0);    // SPINLOCKCOLLISIONCOUNT

        m_Table->MoveToCol (ITOVT_INT4, &REGION_WAITS(pSemCtrl), 0);    // WAITCOUNT

        m_Table->MoveToCol (ITOVT_INT4, &REGION_COLLISION(pSemCtrl), 0);    // COLLISIONCOUNT

        SAPDB_Int4 Rate = REGION_ACCESS(pSemCtrl) ? (SAPDB_Int4)(100.0 * double(REGION_COLLISION(pSemCtrl)) / double(REGION_ACCESS(pSemCtrl))) : 0;
        m_Table->MoveToCol (ITOVT_INT4, &Rate, 0);    // COLLISIONRATE
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool SysView_RegionStatistics::ResetCounter(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_RegionStatistics::ResetCounter", SysView_Trace, 5);

    for ( SAPDB_UInt index = 0 ; index < XPARAM(ulNoOfRegions) ; index++ )
    {
        PSEM_CTRL_REC   pSemCtrl = FIRST_REGION + index ;

        pSemCtrl->getCountAtReset           = REGION_ACCESS(pSemCtrl);

        pSemCtrl->begExclTasCountAtReset    = REGION_BEGEXCL_TAS_COLL(pSemCtrl);

        pSemCtrl->endExclTasCountAtReset    = REGION_ENDEXCL_TAS_COLL(pSemCtrl);

        pSemCtrl->waitCountAtReset          = REGION_WAITS(pSemCtrl);

        pSemCtrl->collisionCountAtReset     = REGION_COLLISION(pSemCtrl);
    }
    m_lastResetTimestamp = RTESys_MicroSecondTime();

    return ( true );
}

SAPDB_UInt8 SysView_RegionStatistics::m_lastResetTimestamp = 0;
/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
