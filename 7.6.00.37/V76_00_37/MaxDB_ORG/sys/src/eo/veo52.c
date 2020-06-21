/*!
  @file           veo52.c
  @author         JoergM
  @special area   Time and Time Statistic Functions
  @brief          
  @see            

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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

\endif
*/



/*
 * INCLUDE FILES
 */
#include "heo52.h"
#include "geo50_0.h"
#include "RunTime/System/RTESys_MicroTime.h"
#include "RunTime/System/RTESys_Time.h"

/*
 *  DEFINES
 */
#define MOD__  "veo52.c:"
#define EO52_CENTURY_ADJUST 1900
#define EO52_MICROSECONDS_PER_SECOND (1000000U)

/*
 * ========================== GLOBAL FUNCTIONS ================================
 */
externC void vtimestamp (
tsp00_Timestamp stamp )
{
    DBGPAS;
    RTESys_BuildSQLTimeStamp(RTESys_MicroSecondTime(), stamp);
}

/*---------------------*/

externC void vUTCtimestamp (
tsp00_Timestamp stamp )
{
    DBGPAS;
    RTESys_BuildSQLGMTTimeStamp(RTESys_MicroSecondTime(), stamp);
}

/*---------------------*/

void eo52UpdateTaskStateStatisticRec(
 teo52_TaskRelativeAndAbsoluteTimeStatistic *pRec,
 teo52_TaskStateStatistic   *pState )
{
    SAPDB_UInt8 delta_abs_time = pState->dispatcherLeaveTime - pState->dispatcherEnterTime;
    SAPDB_UInt8 delta_rel_time = pState->runqueueEnqueueTime - pState->dispatcherEnterTime;

    pRec->TotalTimeStat.ulMeasurements++;
    pRec->TotalTimeStat.ulAbsMicroSeconds += delta_abs_time;
    pRec->TotalTimeStat.ulRelMicroSeconds += delta_rel_time;
    pRec->TimeStat.ulMeasurements++;
    pRec->TimeStat.ulAbsMicroSeconds += delta_abs_time;
    pRec->TimeStat.ulRelMicroSeconds += delta_rel_time;
}

/*---------------------*/

/*

  This code was used for searching of memory overwrite...

  RTEDiag_SetTrigger(position) was called with pointer to overwritten memory position 
  RTEDiag_CheckTrigger() was added everywhere where a memory overwrite was suspected...
 */

/*
void **RTEDiag_TheTrigger = 0;

void RTEDiag_CheckTrigger()
{
    if ( TheTrigger && !*TheTrigger )
    {
        TheTrigger = 0;
        abort();
    }
}

void RTEDiag_SetTrigger(void **trigger_pointer)
{
    TheTrigger = trigger_pointer;
}

void RTEDiag_ResetTrigger()
{
    TheTrigger = 0;
}
 */

/*---------------------*/

void eo52ResetTaskStatistic( SAPDB_UInt4 taskId )
{
    TASK_CTRL pTaskCtrl = KGS->pFirstTaskCtrl + taskId - 1;

#if defined(_WIN32)
    pTaskCtrl->ulDispatcherCalls = 0;
    pTaskCtrl->ulSelfSuspCount   = 0;
#else
    pTaskCtrl->disp_counter   = 0;
    pTaskCtrl->self_susp_cnt  = 0;
#endif

    memset( &pTaskCtrl->TaskStateStat.SelfIOReadStat.IOStat.TimeStat, 0, sizeof( pTaskCtrl->TaskStateStat.SelfIOReadStat.IOStat.TimeStat ) );
    pTaskCtrl->TaskStateStat.SelfIOReadStat.ulPageCount = 0;
    memset( &pTaskCtrl->TaskStateStat.SelfIOWriteStat.IOStat.TimeStat, 0, sizeof( pTaskCtrl->TaskStateStat.SelfIOWriteStat.IOStat.TimeStat ) );
    pTaskCtrl->TaskStateStat.SelfIOWriteStat.ulPageCount = 0;
    memset( &pTaskCtrl->TaskStateStat.DevIOReadStat.IOStat.TimeStat, 0, sizeof( pTaskCtrl->TaskStateStat.DevIOReadStat.IOStat.TimeStat ) );
    pTaskCtrl->TaskStateStat.DevIOReadStat.ulPageCount = 0;
    memset( &pTaskCtrl->TaskStateStat.DevIOWriteStat.IOStat.TimeStat, 0, sizeof( pTaskCtrl->TaskStateStat.DevIOWriteStat.IOStat.TimeStat ) );
    pTaskCtrl->TaskStateStat.DevIOWriteStat.ulPageCount = 0;
    memset( &pTaskCtrl->TaskStateStat.AsynIOReadStat.IOStat.TimeStat, 0, sizeof( pTaskCtrl->TaskStateStat.AsynIOReadStat.IOStat.TimeStat ) );
    pTaskCtrl->TaskStateStat.AsynIOReadStat.ulPageCount = 0;
    memset( &pTaskCtrl->TaskStateStat.AsynIOWriteStat.IOStat.TimeStat, 0, sizeof( pTaskCtrl->TaskStateStat.AsynIOWriteStat.IOStat.TimeStat ) );
    pTaskCtrl->TaskStateStat.AsynIOWriteStat.ulPageCount = 0;
    memset(&pTaskCtrl->TaskStateStat.ulDevQueueLen[0], 0, sizeof(pTaskCtrl->TaskStateStat.ulDevQueueLen));
    memset( &pTaskCtrl->TaskStateStat.VwaitStat.TimeStat, 0, sizeof( pTaskCtrl->TaskStateStat.VwaitStat.TimeStat ) );
    memset( &pTaskCtrl->TaskStateStat.VsuspStat.TimeStat, 0, sizeof( pTaskCtrl->TaskStateStat.VsuspStat.TimeStat) );
    memset( &pTaskCtrl->TaskStateStat.VsleepStat.TimeStat, 0, sizeof( pTaskCtrl->TaskStateStat.VsleepStat.TimeStat ) );
    memset( &pTaskCtrl->TaskStateStat.RplRcvStat.TimeStat, 0, sizeof( pTaskCtrl->TaskStateStat.RplRcvStat.TimeStat ) );
    memset( &pTaskCtrl->TaskStateStat.RcvRplStat.TimeStat, 0, sizeof( pTaskCtrl->TaskStateStat.RcvRplStat.TimeStat ) );
    pTaskCtrl->TaskStateStat.ulRcvRplToLong = 0;
    memset ( &pTaskCtrl->QueStat.Resetable, 0, sizeof ( pTaskCtrl->QueStat.Resetable ) );
    memset ( &pTaskCtrl->PrioStat.Resetable, 0, sizeof ( pTaskCtrl->PrioStat.Resetable ) );
    memset ( &pTaskCtrl->TaskOMSStat  , 0, sizeof ( teo52_TaskOMSStatistic ) );
}

/*---------------------*/

void eo52ResetIOStatistic( SAPDB_UInt4 taskId )
{
    TASK_CTRL pTaskCtrl = KGS->pFirstTaskCtrl + taskId - 1;

    memset( &pTaskCtrl->TaskStateStat.SelfIOReadStat.IOStat.TimeStat, 0, sizeof( pTaskCtrl->TaskStateStat.SelfIOReadStat.IOStat.TimeStat ) );
    pTaskCtrl->TaskStateStat.SelfIOReadStat.ulPageCount = 0;
    memset( &pTaskCtrl->TaskStateStat.SelfIOWriteStat.IOStat.TimeStat, 0, sizeof( pTaskCtrl->TaskStateStat.SelfIOWriteStat.IOStat.TimeStat ) );
    pTaskCtrl->TaskStateStat.SelfIOWriteStat.ulPageCount = 0;
    memset( &pTaskCtrl->TaskStateStat.DevIOReadStat.IOStat.TimeStat, 0, sizeof( pTaskCtrl->TaskStateStat.DevIOReadStat.IOStat.TimeStat ) );
    pTaskCtrl->TaskStateStat.DevIOReadStat.ulPageCount = 0;
    memset( &pTaskCtrl->TaskStateStat.DevIOWriteStat.IOStat.TimeStat, 0, sizeof( pTaskCtrl->TaskStateStat.DevIOWriteStat.IOStat.TimeStat ) );
    pTaskCtrl->TaskStateStat.DevIOWriteStat.ulPageCount = 0;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
