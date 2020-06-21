/****************************************************************************/
/*!

  @file        SysView_TaskGroupStatistics.cpp

  -------------------------------------------------------------------------

  @author      ElkeZ

  @ingroup     SystemViews
  @brief       This module implements the "TaskGroupStatistics" view class.

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
#include "SystemViews/SysView_TaskGroupStatistics.hpp"
#include "SQLManager/SQLMan_Context.hpp"

#define KERNEL_LZU
#include "geo50_0.h"
#include "heo92.h"
#include "RunTime/System/RTESys_MicroTime.h"
#include "RunTime/Tasking/RTETask_TaskScheduler.hpp"



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

void SysView_TaskGroupStatistics::Create()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_TaskGroupStatistics::Create", SysView_Trace, 5);

	m_Table->AppendCol (ITOCT_FIXED ,SV_TASKGROUPID, 10);
	m_Table->AppendCol (ITOCT_CHAR ,SV_TASKCLUSTER, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_TASKCOUNT, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_USEDRUNQUEUELENGTH, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_MAXUSEDRUNQUEUELENGTH, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_IDLECOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_IDLETIME, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_IOWAITCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_IOWAITTIME, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_USEDSYSTEMTIME, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_USEDUSERTIME, 20);

}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_TaskGroupStatistics::GetColCount()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_TaskGroupStatistics::GetColCount", SysView_Trace, 5);

    return SV_CC_TASKGROUPSTATISTICS;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_TaskGroupStatistics::EstimateRows()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_TaskGroupStatistics::EstimateRows", SysView_Trace, 5);

    return SV_ER_TASKGROUPSTATISTICS;
}

/*---------------------------------------------------------------------------*/

void SysView_TaskGroupStatistics::Execute()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_TaskGroupStatistics::Execute", SysView_Trace, 5);

    RTETask_TaskScheduler *         pTaskScheduler = NULL;
    SAPDB_UInt4                     ulTotalTasks;
    SAPDB_Char                      szCluster[20];
#if defined (_WIN32)
    union
    {
        FILETIME        fileTime;
        SAPDB_UInt8     timeCounter;
    }                               systemTime, userTime; // time the thread has spent in kernel/user mode 
    FILETIME                        CreationTime;
    FILETIME                        ExitTime;
#endif    

    m_Table->GetCatalogTable();
    pTaskScheduler = RTE_KGS::Instance().TaskSchedulerList();

    SAPDB_UInt8 actMicroSeconds = RTESys_MicroSecTimer();

    while (m_Context.IsOk() && pTaskScheduler != NULL)
    {
        UKT_CTRL    pUktCtrl = pTaskScheduler->LegacyUKTCtrl();

        AddColumn (pTaskScheduler->ID ());
        ulTotalTasks = eo92CreateTaskcluster (pUktCtrl, szCluster, sizeof (szCluster));
        m_Table->MoveToCol (ITOVT_CHARPTR,  szCluster, strlen(szCluster));
        AddColumn (ulTotalTasks);
#if defined (_WIN32)
        AddColumn ((SAPDB_UInt4)(pUktCtrl->ulRUNQueueLen));
        AddColumn (pUktCtrl->maxRunQueueLen);
        AddColumn (pUktCtrl->UKTRunningStat.TotalTimeStat.ulCount);
        AddColumn (actMicroSeconds 
                   - pUktCtrl->ulUKTStartTimeMicroSeconds
                   - pUktCtrl->UKTRunningStat.TotalTimeStat.ulAbsMicroSeconds
                   - pUktCtrl->UKTIOWaitStat.TotalTimeStat.ulAbsMicroSeconds);
        AddColumn (pUktCtrl->UKTIOWaitStat.TotalTimeStat.ulCount);
        AddColumn (pUktCtrl->UKTIOWaitStat.TotalTimeStat.ulAbsMicroSeconds);

        GetThreadTimes(pUktCtrl->ThrdCtrlHeader.hThrd, &CreationTime, &ExitTime, &systemTime.fileTime, &userTime.fileTime);
        //Convert resulting times for system and user usage to micro seconds
        AddColumn (systemTime.timeCounter/10);
        AddColumn (userTime.timeCounter/10);

#else
        AddColumn (pUktCtrl->RunQueueLen);
        AddColumn (pUktCtrl->maxRunQueueLen);
        AddColumn (pUktCtrl->UKTRunningStat.TotalTimeStat.ulCount);
        AddColumn (actMicroSeconds 
                   - pUktCtrl->UKTStartTimeMicroSeconds
                   - pUktCtrl->UKTRunningStat.TotalTimeStat.ulAbsMicroSeconds
                   - pUktCtrl->UKTIOWaitStat.TotalTimeStat.ulAbsMicroSeconds);
        AddColumn (pUktCtrl->UKTIOWaitStat.TotalTimeStat.ulCount);
        AddColumn (pUktCtrl->UKTIOWaitStat.TotalTimeStat.ulAbsMicroSeconds);

        //User and system time (not yet implemented for UNIX)
        AddColumn ();
        AddColumn ();
#endif

        pTaskScheduler = pTaskScheduler->Next ();
    }

}

/*---------------------------------------------------------------------------*/

SAPDB_Bool SysView_TaskGroupStatistics::ResetCounter(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_TaskGroupStatistics::ResetCounter", SysView_Trace, 5);

    RTETask_TaskScheduler *         pTaskScheduler = NULL;

    RTE_KGS::Instance().NotifyLastTaskGroupStatisticsReset();
    pTaskScheduler = RTE_KGS::Instance().TaskSchedulerList();

    while (m_Context.IsOk() && pTaskScheduler != NULL)
    {
        ((UKT_CTRL)(pTaskScheduler->LegacyUKTCtrl()))->SleepStatRecResetPending = true;
        pTaskScheduler = pTaskScheduler->Next ();
    }
       return ( true );
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
