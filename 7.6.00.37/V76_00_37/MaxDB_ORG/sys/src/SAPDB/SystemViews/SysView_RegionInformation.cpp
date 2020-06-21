/****************************************************************************/
/*!

  @file        SysView_RegionInformation.cpp

  -------------------------------------------------------------------------

  @author      ElkeZ

  @ingroup     SystemViews
  @brief       This module implements the "RegionInformation" view class.

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
#include "SystemViews/SysView_RegionInformation.hpp"
#include "geo002.h"
#define KERNEL_LZU
#include "geo50_0.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define REGION_LOCKED_          "LOCKED"
#define REGION_REQUESTED_       "REQUESTED"


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

void SysView_RegionInformation::Create()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_RegionInformation::Create", SysView_Trace, 5);

	m_Table->AppendCol (ITOCT_CHAR ,SV_REGIONNAME, 16);
	m_Table->AppendCol (ITOCT_CHAR ,SV_STATE, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_TASKID, 10);

}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_RegionInformation::GetColCount()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_RegionInformation::GetColCount", SysView_Trace, 5);

    return SV_CC_REGIONINFORMATION;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_RegionInformation::EstimateRows()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_RegionInformation::EstimateRows", SysView_Trace, 5);

    return SV_ER_REGIONINFORMATION;
}

/*---------------------------------------------------------------------------*/

void SysView_RegionInformation::Execute()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_RegionInformation::Execute", SysView_Trace, 5);

    m_Table->GetCatalogTable();
    
    for ( SAPDB_UInt index = 0 ; index < XPARAM(ulNoOfRegions) ; index++ )
    {
        PSEM_CTRL_REC   pSemCtrl = FIRST_REGION + index ;
        TASK_CTRL       pOwnerTaskCtrl = OWNER_TASK(pSemCtrl);

        if( pOwnerTaskCtrl )
        {
            // first the task that owns the region
            m_Table->MoveToCol (ITOVT_CHARPTR, REGION_NAME(pSemCtrl),(SAPDB_Int)strlen((SAPDB_Char *)REGION_NAME(pSemCtrl))); // REGIONNAME

            m_Table->MoveToCol (ITOVT_CHARPTR, REGION_LOCKED_,(SAPDB_Int)strlen((SAPDB_Char *)REGION_LOCKED_)); // STATE

            m_Table->MoveToCol (ITOVT_INT4, &TASK_INDEX(pOwnerTaskCtrl), 0);

            // and now the ones waiting for the region
            TASK_CTRL pWaitTaskCtrl ;
            for ( PSEM_QUEUE_REC pSemQueElem   =  LAST_ELEM(WAIT_LIST(pSemCtrl));
                    pSemQueElem && (pWaitTaskCtrl = WAIT_TASK(pSemQueElem)) != NULL ;
                    pSemQueElem =  NEXT_ELEM(pSemQueElem) )
            {
                m_Table->MoveToCol (ITOVT_CHARPTR, REGION_NAME(pSemCtrl),(SAPDB_Int)strlen((SAPDB_Char *)REGION_NAME(pSemCtrl))); // REGIONNAME

                m_Table->MoveToCol (ITOVT_CHARPTR, REGION_REQUESTED_,(SAPDB_Int)strlen((SAPDB_Char *)REGION_REQUESTED_)); // STATE

                m_Table->MoveToCol (ITOVT_INT4, &TASK_INDEX(pWaitTaskCtrl), 0);

            }
        }
    }

}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
