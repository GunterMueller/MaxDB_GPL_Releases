/****************************************************************************/
/*!

  @file        SysView_TaskStatisticsReset.cpp

  -------------------------------------------------------------------------

  @author      ElkeZ

  @ingroup     SystemViews
  @brief       This module implements the "TaskStatisticsReset" view class.

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
#include "SystemViews/SysView_TaskStatisticsReset.hpp"


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

void SysView_TaskStatisticsReset::Create()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_TaskStatisticsReset::Create", SysView_Trace, 5);

	m_Table->AppendCol (ITOCT_TIMESTAMP, SV_RESETDATE, 0);
	m_Table->AppendCol (ITOCT_FIXED ,SV_TASKID, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_COMMUNICATIONCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_BYTESRECEIVED, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_BYTESREPLIED, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_DISPATCHCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_TASKRESCEDULECOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_REGIONACCESSCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_SUSPENDCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_SUSPENDRELATIVETIME, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_SUSPENDABSOLUTETIME, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_WAITCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_WAITRELATIVETIME, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_WAITABSOLUTETIME, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_SLEEPCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_SLEEPRELATIVETIME, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_SLEEPABSOLUTETIME, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_STATEMENTEXECUTETIME, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_STATEMENTWAITRELATIVETIME, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_STATEMENTWAITABSOLUTETIME, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_SELFREADCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_SELFREADPAGECOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_SELFREADABSOLUTETIME, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_SELFWRITECOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_SELFWRITEPAGECOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_SELFWRITEABSOLUTETIME, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_READCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_READPAGECOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_READRELATIVETIME, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_READABSOLUTETIME, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_WRITECOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_WRITEPAGECOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_WRITERELATIVETIME, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_WRITEABSOLUTETIME, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_USABLESTACKSIZE, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_MAXUSEDSTACK, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_HOPCOUNT, 20);

}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_TaskStatisticsReset::GetColCount()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_TaskStatisticsReset::GetColCount", SysView_Trace, 5);

    return SV_CC_TASKSTATISTICSRESET;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_TaskStatisticsReset::EstimateRows()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_TaskStatisticsReset::EstimateRows", SysView_Trace, 5);

    return SV_ER_TASKSTATISTICSRESET;
}

/*---------------------------------------------------------------------------*/

void SysView_TaskStatisticsReset::Execute()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_TaskStatisticsReset::Execute", SysView_Trace, 5);

    SysView_SysViewBase::Execute(); // throws e_not_implemented
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
