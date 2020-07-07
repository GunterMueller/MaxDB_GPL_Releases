/*!
  @file           heo52.h
  @author         JoergM
  @special area   time
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



#ifndef HEO52_H
#define HEO52_H

#include <stdio.h>
/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "gsp00.h"
#include "geo52.h"

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

/*!
   @brief Return seconds and microseconds since start of database
   @param Sec [out] seconds since start
   @param MicroSec [out] microseconds of last second (0-999999)
 */
externC void
vclock            ( teo00_Int4 * Sec,
                    teo00_Int4 * MicroSec);

/*!
   @brief Return current time as timestamp

      Format of timestamp is  YYYYMMDDhhmmssuuuuuu
      Y Year digits
      M month digits
      D day digits
      h hour digits
      m minute digits
      s second digits
      u microsecond digits
   @param          pStamp [out] timestamp to fill
 */

externC void
vtimestamp        ( tsp00_Timestamp VAR_ARRAY_REF pStamp);

/*!
   @brief Return current UTC time as timestamp

      Format of timestamp is  YYYYMMDDhhmmssuuuuuu
      Y Year digits
      M month digits
      D day digits
      h hour digits
      m minute digits
      s second digits
      u microsecond digits
   @param          pStamp [out] timestamp to fill
 */

externC void
vUTCtimestamp        ( tsp00_Timestamp VAR_ARRAY_REF pStamp);

/*!
   @brief Return current date and time

      Format of date is YYYYMMDD. Format of time is hhmmss
      Y Year digits
      M month digits
      D day digits
      h hour digits
      m minute digits
      s second digits
   @param          pDate [out] date to fill
   @param          pTime [out] time to fill
 */
externC void
vdattime          ( tsp00_Date VAR_ARRAY_REF pDate,
                    tsp00_Time VAR_ARRAY_REF pTime );

/*
  @brief Return difference between local time and GMT in hours

  Berlin winter time result 1.

  Berlin summer time result 2.

  New Zealand Chatham Islands result 12,75 !!!!

  NOTE: If weekday difference between localtime and systemtime is larger
  than a day, 0. is returned and a warning message is issued in knldiag.

  @return Difference in hours as floating point number
 */
externC tsp00_Longreal vTimeZoneDelta();

/*

  This code was used for searching of memory overwrite...

  RTEDiag_SetTrigger(position) was called with pointer to overwritten memory position 
  RTEDiag_CheckTrigger() was added everywhere where a memory overwrite was suspected...
 */
/*
externC void RTEDiag_CheckTrigger();
externC void RTEDiag_SetTrigger(void **trigger_pointer);
externC void RTEDiag_ResetTrigger();
 */

/*!
   @brief    Update relative and absolute time record

    The difference (leaveDispatcher - enterDispatcher) is added to the AbsoluteTime record.
    The difference (enterRunqueue - enterDispatcher) is added to the RelativeTime record.
   @param          pRec [inout] record to update
   @param          pState [in] record containing dispatcher timestamps
 */

#if !defined(__cplusplus)

externC void eo52UpdateTaskStateStatisticRec(
     teo52_TaskRelativeAndAbsoluteTimeStatistic *pRec,
     teo52_TaskStateStatistic   *pState );

externC void eo52ResetTaskStatistic( SAPDB_UInt4 taskId );
externC void eo52ResetIOStatistic( SAPDB_UInt4 taskId );
#else

/*!
   @brief    Update relative and absolute time record

    The difference (leaveDispatcher - enterDispatcher) is added to the AbsoluteTime record.
    The difference (enterRunqueue - enterDispatcher) is added to the RelativeTime record.
   @param          rec [inout] record to update
   @param          state [in] record containing dispatcher timestamps
 */
inline void eo52UpdateTimeInStatisticRec(
     teo52_TaskRelativeAndAbsoluteTimeStatistic &rec,
     teo52_TaskStateStatistic & state )
{
    SAPDB_UInt8 delta_abs_time = state.dispatcherLeaveTime - state.dispatcherEnterTime;
    SAPDB_UInt8 delta_rel_time = state.runqueueEnqueueTime - state.dispatcherEnterTime;

    rec.TotalTimeStat.ulMeasurements++;
    rec.TotalTimeStat.ulAbsMicroSeconds += delta_abs_time;
    rec.TotalTimeStat.ulRelMicroSeconds += delta_rel_time;
    rec.TimeStat.ulMeasurements++;
    rec.TimeStat.ulAbsMicroSeconds += delta_abs_time;
    rec.TimeStat.ulRelMicroSeconds += delta_rel_time;
}

/*!
   @brief    Update count information in teo52_TaskAbsoluteTimeStatistic record
   @param rec [in] the record to update
 */
inline void eo52UpdateCountInStatisticRec( teo52_TaskAbsoluteTimeStatistic & rec )
{
    rec.TotalTimeStat.ulCount++;
    rec.TimeStat.ulCount++;
}

/*!
   @brief    Update count information in teo52_TaskAbsoluteTimeStatistic record
   @param rec [in] the record to update
 */
inline void eo52UpdateCountInStatisticRec( teo52_TaskRelativeAndAbsoluteTimeStatistic & rec )
{
    rec.TotalTimeStat.ulCount++;
    rec.TimeStat.ulCount++;
}

/*!
   @brief    Update count information in teo52_TaskDevIOStatistic record
   @param rec [in] the record to update
   @param pageCount [in]the page count to add
 */
inline void eo52UpdateCountInIOStatisticRec(
 teo52_TaskDevIOStatistic & rec,
 SAPDB_UInt8 pageCount )
{
    rec.IOStat.TotalTimeStat.ulCount++;
    rec.TotalPageCount+= pageCount;
    rec.IOStat.TimeStat.ulCount++;
    rec.ulPageCount+= pageCount;
}

/*!
   @brief    Update count information in teo52_IOStatistic record
   @param rec [in] the record to update
   @param pageCount [in]the page count to add
 */
inline void eo52UpdateCountInIOStatisticRec(
 teo52_IOStatistic & rec,
 SAPDB_UInt8 pageCount )
{
    rec.IOStat.TotalTimeStat.ulCount++;
    rec.TotalPageCount+= pageCount;
    rec.IOStat.TimeStat.ulCount++;
    rec.ulPageCount+= pageCount;
}

/*!
   @brief    Update time information in teo52_TaskAbsoluteTimeStatistic record
   @param rec [in] the record to update
   @param deltaTime [in] the time to add
 */
inline void eo52UpdateTimeInStatisticRec(
 teo52_TaskAbsoluteTimeStatistic & rec,
 SAPDB_UInt8 deltaTime )
{
    rec.TotalTimeStat.ulMeasurements++;
    rec.TotalTimeStat.ulAbsMicroSeconds += deltaTime;
    rec.TimeStat.ulMeasurements++;
    rec.TimeStat.ulAbsMicroSeconds += deltaTime;
}

/*!
   @brief    Update time information in teo52_TaskAbsoluteTimeStatistic record
   @param rec [in] the record to update
   @param delta_abs_time [in] the time to add to absolute time
   @param delta_rel_time [in] the time to add to relative time
 */
inline void eo52UpdateTimeInStatisticRec(
 teo52_TaskRelativeAndAbsoluteTimeStatistic & rec,
 SAPDB_UInt8 delta_abs_time, 
 SAPDB_UInt8 delta_rel_time )
{
    rec.TotalTimeStat.ulMeasurements++;
    rec.TotalTimeStat.ulAbsMicroSeconds += delta_abs_time;
    rec.TotalTimeStat.ulRelMicroSeconds += delta_rel_time;
    rec.TimeStat.ulMeasurements++;
    rec.TimeStat.ulAbsMicroSeconds += delta_abs_time;
    rec.TimeStat.ulRelMicroSeconds += delta_rel_time;
}

/*!
   @brief    Reset resetable information in teo52_TaskDevIOStatistic record
   @param rec [in] the record to reset
 */
inline void eo52ResetStatisticRecord( teo52_TaskAbsoluteTimeStatistic &rec )
{
    rec.TimeStat.ulAbsMicroSeconds = 0;
    rec.TimeStat.ulMeasurements = 0;
    rec.TimeStat.ulCount = 0;
}

/*!
   @brief    Reset resetable information in teo52_TaskDevIOStatistic record
   @param rec [in] the record to reset
 */
inline void eo52ResetStatisticRecord( teo52_TaskRelativeAndAbsoluteTimeStatistic &rec )
{
    rec.TimeStat.ulAbsMicroSeconds = 0;
    rec.TimeStat.ulRelMicroSeconds = 0;
    rec.TimeStat.ulMeasurements = 0;
    rec.TimeStat.ulCount = 0;
}

/*!
   @brief    Reset resetable information in teo52_TaskDevIOStatistic record
   @param rec [in] the record to reset
 */
inline void eo52ResetStatisticRecord( teo52_TaskDevIOStatistic &rec )
{
    eo52ResetStatisticRecord(rec.IOStat);
    rec.ulPageCount = 0;
}

/*!
   @brief    Reset resetable information in teo52_TaskDevIOStatistic record
   @param rec [in] the record to reset
 */
inline void eo52ResetStatisticRecord( teo52_IOStatistic &rec )
{
    eo52ResetStatisticRecord(rec.IOStat);
    rec.ulPageCount = 0;
}

/*!
   @brief    Reset resetable information in teo52_TaskStateStatistic record
   @param rec [in] the record to reset
 */
inline void eo52ResetStatisticRecord( teo52_TaskStateStatistic &rec )
{
    eo52ResetStatisticRecord(rec.SelfIOReadStat);
    eo52ResetStatisticRecord(rec.SelfIOWriteStat);
    eo52ResetStatisticRecord(rec.DevIOReadStat);
    eo52ResetStatisticRecord(rec.DevIOWriteStat);
    eo52ResetStatisticRecord(rec.AsynIOReadStat);
    eo52ResetStatisticRecord(rec.AsynIOWriteStat);
    memset(&rec.ulDevQueueLen[0], 0, sizeof(rec.ulDevQueueLen));
    eo52ResetStatisticRecord(rec.VwaitStat);
    eo52ResetStatisticRecord(rec.VsuspStat);
    eo52ResetStatisticRecord(rec.VsleepStat);
    eo52ResetStatisticRecord(rec.RplRcvStat);
    eo52ResetStatisticRecord(rec.RcvRplStat);
    rec.ulRcvRplToLong = 0;
}

/*!
   @brief    Reset resetable information in teo52_TaskQueueStatistic record
   @param rec [in] the record to reset
 */
inline void eo52ResetStatisticRecord( teo52_TaskQueueStatistic &rec )
{
    rec.Resetable.ulUKTQueCount = 0;
    rec.Resetable.ulUToUQueCount = 0;
    rec.Resetable.ulCOMQueCount = 0;
}

/*!
   @brief    Reset resetable information in teo52_TaskPrioStatistic record
   @param rec [in] the record to reset
 */
inline void eo52ResetStatisticRecord( teo52_TaskPrioStatistic &rec )
{
    rec.Resetable.ulTotalCount = 0;
    rec.Resetable.ulOtherUkt = 0;
}

/*!
   @brief    Reset resetable information in teo52_TaskDevIOStatistic record
   @param rec [in] the record to reset
 */
inline void eo52ResetStatisticRecord( teo52_DevIOStatistic &rec )
{
    eo52ResetStatisticRecord(rec.DevRead);
    eo52ResetStatisticRecord(rec.DevWrite);
    rec.ulMaxIOQueLen = 0;
    rec.ResetPending = 0;
}
#endif /* __cplusplus */

#endif  /* HEO52_H */
