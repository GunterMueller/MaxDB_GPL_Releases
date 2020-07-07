/****************************************************************************/
/*!

  @file        SysView_HistoryCleanupStatistics.cpp

  -------------------------------------------------------------------------

  @author      ElkeZ

  @ingroup     SystemViews
  @brief       This module implements the "HistoryCleanupStatistics" view class.

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
#include "SystemViews/SysView_HistoryCleanupStatistics.hpp"
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

void SysView_HistoryCleanupStatistics::Create()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_HistoryCleanupStatistics::Create", SysView_Trace, 5);

	m_Table->AppendCol (ITOCT_FIXED ,SV_TASKID, 10);
	m_Table->AppendCol (ITOCT_CHAR ,SV_ACTIVE, 3);
	m_Table->AppendCol (ITOCT_FIXED ,SV_RESUMETASKCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_ALLOBJECTCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_CLEANUPCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_EMERGENCYCLEANUPCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_ALLOCATEOBJECTCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_CREATEOBJECTCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_UPDATEOBJECTCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_DELETEOBJECTCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_LOCKOBJECTCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_RELEASEOBJECTPAGECOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_RELEASECONTAINERPAGECOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_DROPCONTAINERCOUNT, 20);
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_HistoryCleanupStatistics::GetColCount()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_HistoryCleanupStatistics::GetColCount", SysView_Trace, 5);

    return SV_CC_HISTORYCLEANUPSTATISTICS;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_HistoryCleanupStatistics::EstimateRows()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_HistoryCleanupStatistics::EstimateRows", SysView_Trace, 5);

    return SV_ER_HISTORYCLEANUPSTATISTICS;
}

/*---------------------------------------------------------------------------*/

void SysView_HistoryCleanupStatistics::Execute()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_HistoryCleanupStatistics::Execute", SysView_Trace, 5);

    SysView_SysViewBase::Execute(); // throws e_not_implemented
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool SysView_HistoryCleanupStatistics::ResetCounter(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_HistoryCleanupStatistics::ResetCounter", SysView_Trace, 5);

    m_Context.ThrowError (e_not_implemented, 2);
    return ( true );
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
