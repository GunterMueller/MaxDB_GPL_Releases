/*!
  @file           geo52.h
  @author         JoergM
  @special area   LZU Statistics
  @brief          description ...
  @see            example.html ...

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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



#ifndef GEO52_H
#define GEO52_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "geo00.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define MAX_DEVQUEUELEN_STAT_EO52 3

/* There is no task statistic reset pending */
#define EO52_NO_RESET_PENDING           0

/* This sort of reset is set, if 'x_cons RESET TASKS' was executed. */
/* It is handled in disaptcher entry code while console code sets the flag only */
#define EO52_RESET_TASK_FULL_PENDING    1

/* This sort of reset is set, if 'x_cons TIME ENABLE' was executed (while fTimeMeasure was false) */
/* It is handled in disaptcher entry code while console code sets the flag only */
#define EO52_RESET_TASK_PARTIAL_PENDING 2

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*
// +---------------------------------------------------------------+
// |   STATE STATISTIC                                             |
// +---------------------------------------------------------------+
*/

 typedef struct teo52_AbsoluteTimeRec
   {
   SAPDB_UInt8                             ulAbsMicroSeconds;
   SAPDB_UInt8                             ulMeasurements;
   SAPDB_UInt8                             ulCount;
   } teo52_AbsoluteTime;

  typedef struct teo52_TaskSchedulerTimeRec
   {
   SAPDB_UInt8                             ulAbsMicroSeconds;
   SAPDB_UInt8                             ulCount;
   } teo52_TaskSchedulerTime;

 typedef struct teo52_RelativeAndAbsoluteTimeRec
   {
   SAPDB_UInt8                             ulAbsMicroSeconds;
   SAPDB_UInt8                             ulRelMicroSeconds;
   SAPDB_UInt8                             ulMeasurements;
   SAPDB_UInt8                             ulCount;
   } teo52_RelativeAndAbsoluteTime;

 typedef struct teo52_TaskAbsoluteTimeStatisticRec
   {
   teo52_AbsoluteTime                        TimeStat;
   teo52_AbsoluteTime                        TotalTimeStat;
   } teo52_TaskAbsoluteTimeStatistic;

 typedef struct teo52_TaskSchedulerTimeStatisticRec
   {
   teo52_TaskSchedulerTime                   TimeStat;
   teo52_TaskSchedulerTime                   TotalTimeStat;
   } teo52_TaskSchedulerTimeStatistic;

 typedef struct teo52_IOStatisticRec
   {
   teo52_TaskAbsoluteTimeStatistic            IOStat;
   SAPDB_UInt8                                ulPageCount;
   SAPDB_UInt8                                TotalPageCount;
   } teo52_IOStatistic;

 typedef struct teo52_TaskRelativeAndAbsoluteTimeStatisticRec
   {
   teo52_RelativeAndAbsoluteTime              TimeStat;
   teo52_RelativeAndAbsoluteTime              TotalTimeStat;
   } teo52_TaskRelativeAndAbsoluteTimeStatistic;

 typedef struct teo52_TaskDevIOStatisticRec
   {
   teo52_TaskRelativeAndAbsoluteTimeStatistic IOStat;
   SAPDB_UInt8                                ulPageCount;
   SAPDB_UInt8                                TotalPageCount;
   } teo52_TaskDevIOStatistic;

 /* PTS 1106969 */
 typedef struct teo52_TaskOMSStatisticRec
   {
   tsp00_KnlIdentifierc                    ProcedurName;
   SAPDB_UInt4                             CallbackCount;
   SAPDB_Int4                              MethodIdx;
   SAPDB_Int4                              OMSCallActive;
   } teo52_TaskOMSStatistic;
/*
// +---------------------------------------------------------------+
// |   TASK STATE STATISTIC                                        |
// +---------------------------------------------------------------+
*/

typedef struct teo52_TaskStateStatisticRec
  {
  teo52_IOStatistic               SelfIOReadStat, SelfIOWriteStat ;
  teo52_TaskDevIOStatistic        DevIOReadStat , DevIOWriteStat ;
  teo52_TaskDevIOStatistic        AsynIOReadStat, AsynIOWriteStat ;
  SAPDB_UInt4                     ulDevQueueLen[MAX_DEVQUEUELEN_STAT_EO52] ;
  teo52_TaskRelativeAndAbsoluteTimeStatistic   VwaitStat ;
  teo52_TaskRelativeAndAbsoluteTimeStatistic   VsuspStat ;
  teo52_TaskRelativeAndAbsoluteTimeStatistic   VsleepStat ;
  teo52_TaskRelativeAndAbsoluteTimeStatistic   RplRcvStat ;
  teo52_TaskAbsoluteTimeStatistic              RcvRplStat ;
  SAPDB_UInt8                     vreceiveORvreplyLeaveTime; /* in microseconds */
  SAPDB_UInt8                     dispatcherEnterTime;       /* in microseconds */
  SAPDB_UInt8                     dispatcherLeaveTime;       /* in microseconds */
  SAPDB_UInt8                     runqueueEnqueueTime;       /* in microseconds */
#define EO52_TOO_LONG_LIMIT_IN_MICROSECONDS (1000000U)
  SAPDB_UInt4                     ulRcvRplToLong ;   /* counter for commands that took longer that 1 second */
  SAPDB_UInt4                     ulReschedMsecCount;
  } teo52_TaskStateStatistic;


typedef struct teo52_TaskQueueStatisticRec
{
    struct
    {
      SAPDB_UInt4                              ulUKTQueCount;
      SAPDB_UInt4                              ulUToUQueCount;
      SAPDB_UInt4                              ulCOMQueCount;
    } Resetable;
    struct
    {
      SAPDB_UInt4                              ulUKTQueCount;
      SAPDB_UInt4                              ulUToUQueCount;
      SAPDB_UInt4                              ulCOMQueCount;
    } Total;
} teo52_TaskQueueStatistic;

/*
// +---------------------------------------------------------------+
// |   TASK PRIO STATISTIC                                         |
// +---------------------------------------------------------------+
*/
typedef struct teo52_TaskPrioStatisticRec
  {
    struct
    {
      SAPDB_UInt4                              ulTotalCount;
      SAPDB_UInt4                              ulOtherUkt;
    } Resetable;
    struct
    {
      SAPDB_UInt4                              ulTotalCount;
      SAPDB_UInt4                              ulOtherUkt;
    } Total;
  } teo52_TaskPrioStatistic;

/*
// +---------------------------------------------------------------+
// |   DEV / ASDEV I/O STATISTIC                                         |
// +---------------------------------------------------------------+
*/
typedef struct teo52_DevIOStatisticRec
  {
  teo52_IOStatistic            DevRead;          /* resetable */
  teo52_IOStatistic            DevWrite;         /* resetable */
  SAPDB_UInt4                  ulMaxIOQueLen;    /* resetable */
  SAPDB_UInt4                  TotalMaxIOQueLen; /* not resetable */
  SAPDB_UInt4                  ulIOQueLen;       /* This element will not be resetted!!! */
  SAPDB_Bool                   ResetPending;
  } teo52_DevIOStatistic ;

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/


/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/


#endif  /* GEO52_H */
