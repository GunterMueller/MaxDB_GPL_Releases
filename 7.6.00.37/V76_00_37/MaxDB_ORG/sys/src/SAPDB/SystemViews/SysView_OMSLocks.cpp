/****************************************************************************/
/*!

  @file        SysView_OMSLocks.cpp

  -------------------------------------------------------------------------

  @author      ElkeZ

  @ingroup     SystemViews
  @brief       This module implements the "OMSLocks" view class.

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
#include "gsp00.h"
#include "vak001.h"
#include "liveCache/LVC_LibOmsInterface.hpp"
#include "SystemViews/SysView_Defines.hpp"
#include "SystemViews/SysView_ITableObj.hpp"
#include "SystemViews/SysView_OMSLocks.hpp"
#include "SQLManager/SQLMan_Context.hpp"

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

void SysView_OMSLocks::Create()
{

	m_Table->AppendCol (ITOCT_CHARBYTE, SV_ID, 8);
	m_Table->AppendCol (ITOCT_FIXED, SV_TASKID, 10);
	m_Table->AppendCol (ITOCT_FIXED, SV_LOCKREQUESTTIMEOUT, 10);
	m_Table->AppendCol (ITOCT_CHAR, SV_LOCKMODE, 18);
	m_Table->AppendCol (ITOCT_CHAR, SV_REQUESTMODE, 18);

}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_OMSLocks::GetColCount()
{
	return SV_CC_OMSLOCKS;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_OMSLocks::EstimateRows()
{
	return SV_ER_OMSLOCKS;
}

/*---------------------------------------------------------------------------*/

void SysView_OMSLocks::Execute()
{

    //OMS_LibOmsInterface     *pLibOmsInterface;
    void                    *pHandle;
    tgg01_OmsLockInfo       LockInfo;
    SAPDB_Bool              bExit;

    m_Table->GetCatalogTable();

    if (m_Context.IsOk() && (m_Context.a_ex_kind != only_parsing))
    {
        pHandle = NULL;
        bExit = false;

        while ((!bExit) && (LVC_LibOmsInterface::Instance()->NextOmsLockObjInfo(&pHandle, LockInfo)))
        {
            m_Table->MoveToCol (ITOVT_CHARPTR, LockInfo.oli_handle.asCharp(), sizeof(LockInfo.oli_handle));
            m_Table->MoveToCol (ITOVT_INT4, &LockInfo.oli_taskid, 0);
            if (LockInfo.oli_timeout < 0)
            {
                m_Table->MoveToCol (ITOVT_NULL, NULL, 0);
            }
            else
            {
                m_Table->MoveToCol (ITOVT_INT4, &LockInfo.oli_timeout, 0);
            }
            m_Table->MoveToCol (ITOVT_CHARPTR, LockInfo.oli_lockmode.asCharp(), LockInfo.oli_lockmode.length());
            m_Table->MoveToCol (ITOVT_CHARPTR, LockInfo.oli_requestmode.asCharp(), LockInfo.oli_requestmode.length());

            if (!pHandle)
            {
                bExit = true;
            }
        }
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
