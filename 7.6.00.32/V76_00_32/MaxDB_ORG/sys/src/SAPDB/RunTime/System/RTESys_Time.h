/*!---------------------------------------------------------------------
  @file           RTESys_Time.h
  @author         JoergM
  @brief          Time system call wrapper
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
---------------------------------------------------------------------*/

#ifndef RTESYS_TIME_H
#define RTESYS_TIME_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_Types.h"
#include "RunTime/RTE_CompilerFeatures.h" /* for externC and VAR_ARRAY_REF ... */

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*! SAPDB_TIMESTAMP_FORMAT          "YYYY-MM-DD HH:MM:SS" zero terminated */
#define SAPDB_TIMESTAMP_FORMAT "%04d-%2d-%2d %2d:%2d:%2d"
/*! Offset to year in SAPDB_TIMESTAMP */
#define SAPDB_TIMESTAMP_YEAR_OFFSET 0
/*! Offset to month in SAPDB_TIMESTAMP */
#define SAPDB_TIMESTAMP_MONTH_OFFSET 5
/*! Offset to day in SAPDB_TIMESTAMP */
#define SAPDB_TIMESTAMP_DAY_OFFSET 8
/*! Offset to hour in SAPDB_TIMESTAMP */
#define SAPDB_TIMESTAMP_HOUR_OFFSET 11
/*! Offset to minute in SAPDB_TIMESTAMP */
#define SAPDB_TIMESTAMP_MINUTE_OFFSET 14
/*! Offset to second in SAPDB_TIMESTAMP */
#define SAPDB_TIMESTAMP_SECOND_OFFSET 17

/*! SAPDB_SQLTIMESTAMP_FORMAT     "YYYYMMDDHHMMSSmmmuuu" not zero terminated */
#define SAPDB_SQLTIMESTAMP_FORMAT "%04d%2d%2d%2d%2d%2d%3d%3d"
/*! Offset to year in SAPDB_KERNELTIMESTAMP */
#define SAPDB_SQLTIMESTAMP_YEAR_OFFSET 0
/*! Offset to month in SAPDB_KERNELTIMESTAMP */
#define SAPDB_SQLTIMESTAMP_MONTH_OFFSET 4
/*! Offset to day in SAPDB_KERNELTIMESTAMP */
#define SAPDB_SQLTIMESTAMP_DAY_OFFSET 6
/*! Offset to hour in SAPDB_KERNELTIMESTAMP */
#define SAPDB_SQLTIMESTAMP_HOUR_OFFSET 8
/*! Offset to minute in SAPDB_KERNELTIMESTAMP */
#define SAPDB_SQLTIMESTAMP_MINUTE_OFFSET 10
/*! Offset to second in SAPDB_KERNELTIMESTAMP */
#define SAPDB_SQLTIMESTAMP_SECOND_OFFSET 12
/*! Offset to millisecond in SAPDB_KERNELTIMESTAMP */
#define SAPDB_SQLTIMESTAMP_MILISECOND_OFFSET 14
/*! Offset to millisecond in SAPDB_KERNELTIMESTAMP */
#define SAPDB_SQLTIMESTAMP_MICROSECOND_OFFSET 17

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*! Character array for SAPDB_TIMESTAMP_FORMAT "YYYY-MM-DD HH:MM:SS" zero terminated */
typedef SAPDB_Char RTESys_Timestamp            [5   +3 +3 +3 +3 +3  ];

/*! Character array for Kernel Timestamp "YYYYMMDDHHMMSSmmmuuu" not zero terminated! */
typedef SAPDB_UTF8 RTESys_SQLTimestamp   [   4+2+2+2+2+2+3 +3 ];

/*!--------------------------------------------------------------------
   @brief    Fill the given timestamp buffer with local time information

   Format SAPDB_TIMESTAMP_FORMAT YYYY-MM-DD HH:MM:SS

   @param    timeBuffer [out] the time buffer to fill
   @return   Pointer to first character
 */
externC SAPDB_Char *RTESys_FillProtocolTimeStamp(RTESys_Timestamp VAR_ARRAY_REF timeBuffer);

/*!--------------------------------------------------------------------
   @brief    Fill the given timestamp buffer with GMT time information
 
   Format SAPDB_TIMESTAMP_FORMAT YYYY-MM-DD HH:MM:SS

   @param    timeBuffer [out] the time buffer to fill
   @return   Pointer to first character
 */
externC SAPDB_Char *RTESys_FillProtocolGMTTimeStamp(RTESys_Timestamp VAR_ARRAY_REF timeBuffer);

/*!--------------------------------------------------------------------
   @brief    Returns seconds since 1.1.1970 0:00 UTC

   Used as input for RTESys_BuildTimeStamp to generate kernel internal time stamps. This
   call is not used to get timestamps for intenal timing measurements, so it is
   not called very frequently, but it should be as precise as possible and always
   monotonic ascendend. It is somewhat faster than RTESys_MicroSecondTime().

   @return   Seconds since 1.1.1970 0:00 UTC
 */
externC SAPDB_UInt8 RTESys_Time();

/*!--------------------------------------------------------------------
   @brief    Calculate actual time zone difference in seconds

   To get localtime from gmttime add this difference to the gmtttime returned by RTESys_Time().
   This function is also used by RTESys_BuildTimeStamp().

   @return   the number of seconds (localtime - gmttime)
 */
externC SAPDB_Int4 RTESys_TimeZoneDelta();
/*!--------------------------------------------------------------------
   @brief    Convert seconds since 1.1.1970 0:00 UTC in local time information
   @param    secondsSinceEpoch [in]  Seconds since 1.1.1970 0:00 UTC
   @param    timeBuffer [out] the time buffer to fill
   @return   Pointer to first character
 */
externC SAPDB_Char *RTESys_BuildTimeStamp( SAPDB_UInt8 secondsSinceEpoch,
                                           RTESys_Timestamp VAR_ARRAY_REF timeBuffer);

/*!--------------------------------------------------------------------
   @brief    Convert seconds since 1.1.1970 0:00 UTC in GMT time information
   @param    secondsSinceEpoch [in]  Seconds since 1.1.1970 0:00 UTC
   @param    timeBuffer [out] the time buffer to fill
   @return   Pointer to first character
 */
externC SAPDB_Char *RTESys_BuildGMTTimeStamp( SAPDB_UInt8 secondsSinceEpoch,
                                              RTESys_Timestamp VAR_ARRAY_REF timeBuffer);

/*!--------------------------------------------------------------------
   @brief    Fill the given kernel timestamp buffer with local time information

   Format SAPDB_SQLTIMESTAMP_FORMAT YYYYMMDDHHMMSSmmmuuu

   @param    microSecondsSinceEpoch [in] micro seconds since epoch
   @param    timeBuffer [out] the time buffer to fill
   @return   Pointer to first character
 */
externC SAPDB_Char *RTESys_BuildSQLTimeStamp( SAPDB_UInt8 microSecondsSinceEpoch,
                                              RTESys_SQLTimestamp VAR_ARRAY_REF timeBuffer);

/*!--------------------------------------------------------------------
   @brief    Fill the given kernel timestamp buffer with GMT time information
 
   Format SAPDB_SQLTIMESTAMP_FORMAT YYYYMMDDHHMMSSmmmuuu

   @param    microSecondsSinceEpoch [in] micro seconds since epoch
   @param    timeBuffer [out] the time buffer to fill
   @return   Pointer to first character
 */
externC SAPDB_Char *RTESys_BuildSQLGMTTimeStamp( SAPDB_UInt8 microSecondsSinceEpoch,
                                                 RTESys_SQLTimestamp VAR_ARRAY_REF timeBuffer);

/*!--------------------------------------------------------------------
   @brief  Get current time in kernel internal time format

   The kernel internally uses a special numeric representation for time stamps.
   Each digit of the localtime YYYYMMDD hhmmss is in the range [0-9]. This allows
   to form the numeric value for date as
    kernelDate = DD + 100 * MM + 10000 * YYYY
   and the numeric value for
    kernelTime = ss + 100 * mm + 10000 * hh
  The advantage is that a decimal representation of this time stamp number is readable.

   @param   kernelDate [out] kernel internal numeric format for date
   @param   kernelTime [out] kernel internal numeric format for time
   @return  0 if timeout wa exceeded, else number of waiting intervals left!
 */
externC void RTESys_FillKernelTimeStamp( SAPDB_UInt4 VAR_VALUE_REF kernelDate,
                                         SAPDB_UInt4 VAR_VALUE_REF kernelTime);

/*!--------------------------------------------------------------------
   @brief  Get current time minus specified delay time in kernel internal time format

   The kernel internally uses a special numeric representation for time stamps.
   Each digit of the localtime YYYYMMDD hhmmss is in the range [0-9]. This allows
   to form the numeric value for date as
    kernelDate = DD + 100 * MM + 10000 * YYYY
   and the numeric value for
    kernelTime = ss + 100 * mm + 10000 * hh
  The advantage is that a decimal representation of this time stamp number is readable.

  This call is identical with RTESys_FillKernelTimeStamp() but the delay_time is substructed
  from the current number of seconds before consion.

   @param   delayTime  [in]  delay time in seconds
   @param   kernelDate [out] kernel internal numeric format for date
   @param   kernelTime [out] kernel internal numeric format for time
   @return  0 if timeout wa exceeded, else number of waiting intervals left!
 */
externC void RTESys_FillDelayedKernelTimeStamp(SAPDB_Int4       const    delayTime,
                                               SAPDB_UInt4 VAR_VALUE_REF kernelDate,
                                               SAPDB_UInt4 VAR_VALUE_REF kernelTime);

/*!--------------------------------------------------------------------
   @brief  Convert the UTC time stamp into kernel internal time format

   The kernel internally uses a special numeric representation for time stamps.
   Each digit of the localtime YYYYMMDD hhmmss is in the range [0-9]. This allows
   to form the numeric value for date as
    kernelDate = DD + 100 * MM + 10000 * YYYY
   and the numeric value for
    kernelTime = ss + 100 * mm + 10000 * hh
   The advantage is that a decimal representation of this time stamp number is readable.

   @param   utcTimeStamp [in] number of seconds since epoch
   @param   kernelDate [out] kernel internal numeric format for date
   @param   kernelTime [out] kernel internal numeric format for time
   @return  0 if timeout wa exceeded, else number of waiting intervals left!
 */
externC void RTESys_FillKernelTimeStampFromUTC( SAPDB_UInt8 utcTimeStamp,
                                                SAPDB_UInt4 VAR_VALUE_REF kernelDate,
                                                SAPDB_UInt4 VAR_VALUE_REF kernelTime);

/*!--------------------------------------------------------------------
   @brief  Convert the given kernel internal time format into the
           RTESys_SQLTimestamp format "YYYYMMDDHHMMSS000000"

   @param   kernelDate  [in] kernel internal numeric format for date
   @param   kernelTime  [in] kernel internal numeric format for time
   @param   timeBuffer [out] the time buffer to fill
   @return  Pointer to first character
 */

externC SAPDB_Char * RTESys_FillSQLTimeStampFromKernelTimeStamp(
                                                        const SAPDB_UInt4 kernelDate,
                                                        const SAPDB_UInt4 kernelTime,
                                                        RTESys_SQLTimestamp VAR_ARRAY_REF timeBuffer);

/*------------------------------------------------------------------------------*/
/* Crash on UNIX is a special case, where normal time stamp code cannot be used */
/* Reason is that no libc function is synchronous signal save....               */
/* To handle this situation, the initial action in a crash signal handler is to */
/* setup a flag, that allows to detect this situation                           */
/*------------------------------------------------------------------------------*/

/*!--------------------------------------------------------------------
   @brief  Set flag to indicate we are running inside a crash signal handler

   Some of the function we use are not async signal save. To prevent calling them the signal
   handler must call this flag before issuing any of the system calls
 */
externC void RTESys_SetInCrashSignalHandler();

/*!--------------------------------------------------------------------
   @brief  Reset flag to indicate we are running outside a crash signal handler

   Some of the function we use are not async signal save. To prevent calling them the signal
   handler must call this global flag before issuing any of the system calls
 */
externC void RTESys_ResetInCrashSignalHandler();

/*!--------------------------------------------------------------------
   @brief  Check flag to indicate we are running inside a crash signal handler
   @return true if flag set, false if flag is reset
 */
externC SAPDB_Bool RTESys_IsInCrashSignalHandler();

/*!--------------------------------------------------------------------
   @brief  Check flag until true or timeout exceeded

   The flag is normally modified by another thread or process.
   The waiting interval time is fixed to 100 milliseconds. If no waiting time
   is given, the call only tests the flag and returns 1 if flag is true, 0 if not!
   @param   pFlag [in] pointer to flag (i.e. in shared memory)
   @param   waitingIntervalsOf100msec [in] number of waiting intervals (minimum 1)
   @return  0 if timeout wa exceeded, else number of waiting intervals left!
 */
externC SAPDB_UInt4 RTESys_Timeout(SAPDB_Bool *pFlag, SAPDB_UInt4 waitingIntervalsOf100msec);

#endif  /* RTESYS_TIME_H */
