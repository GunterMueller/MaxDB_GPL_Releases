/*!---------------------------------------------------------------------
  @file           RTESys_Time.c
  @author         JoergM
  @brief          System Calls for Time
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




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/System/RTESys_Time.h"
#include "SAPDBCommon/SAPDB_string.h"

#if defined(_WIN32)
#include <windows.h>
/* 1.1.1970 UTC in 100 nanoSecond intervals since 1.1.1601 */
static FILETIME RTESys_epoch = { 3577643008UL, 27111902UL };
#define RTESYS_SEC_TO_100NANO ((SAPDB_UInt8)10000000)
#else
#include <poll.h>
#endif
#include <time.h>

#ifdef AIX
#   include     <sys/time.h>
#endif


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define SAPDB_WEEKDAY_DIFF_BIAS 6
#define SAPDB_ILLEGAL_WEEKDAY_DIFF -2

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  STRUCTURES, TYPES, UNIONS ...                                            *
 *===========================================================================*/

static SAPDB_Int4 RTESys_InCrashSignalHandler = 0;

/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/

/*
  Function: FillProtocolTimeStamp
  Description: Fill the given buffer with local time information
   Format YYYY-MM-DD HH:MM:SS
  Argument: timeBuffer [out] the time buffer to fill
            asLocalTime [in] flag to indicate local time usage or GMT (UTC)
            givenTime [in] given time in seconds since epoch
  Return value: Pointer to first character
 */
static SAPDB_Char *FillProtocolTimeStamp( RTESys_Timestamp timeBuffer,
                                          SAPDB_Int4 asLocalTime,
                                          SAPDB_UInt8 givenTime);
#define WANT_GMT_TIMESTAMP   0
#define WANT_LOCAL_TIMESTAMP 1

/*
  Function: FillDigits
  Description: Fill the given value into buffer

  The given pointer points behind last character to write.
  Number of digits is given (producing lots of leading zeros).
  If separator is not 0 (binary, not ascii...) a separator is
  written after ndigits are written.
  
  Argument: value [in] the value to convert into digits
            ptrBehindLastDigit [in] pointer behind last digit to write
            digitCount [in] number of digits to write
            separator [in] optional separator character
  Return value: Pointer to last character written (most significant digit or separator)
 */
static SAPDB_Char *FillDigits(SAPDB_UInt8 value,
                              SAPDB_Char *ptrBehindLastDigit,
                              SAPDB_Int4 digitCount, 
                              SAPDB_Char separator);

#if !defined(WIN32)
static void EmergencyLocalOrGmtTime(time_t clk, struct tm *buffer, SAPDB_Int4 asLocalTime );
#endif

/*===========================================================================*
 *  GLOBAL FUNCTIONS                                                         *
 *===========================================================================*/

/*
  Function: RTESys_FillProtocolTimeStamp
  Description: Fill the given buffer with local time information
   Format YYYY-MM-DD HH:MM:SS
  Argument: timeBuffer [out] the time buffer to fill
  Return value: Pointer to first character
 */
SAPDB_Char *RTESys_FillProtocolTimeStamp(RTESys_Timestamp timeBuffer)
{
    return FillProtocolTimeStamp( timeBuffer, 
                                  WANT_LOCAL_TIMESTAMP,
                                  (SAPDB_UInt8)0 );
}

/*
  Function: RTESys_FillProtocolGMTTimeStamp
  Description: Fill the given buffer with local time information
   Format YYYY-MM-DD HH:MM:SS
  Argument: timeBuffer [out] the time buffer to fill
  Return value: Pointer to first character
 */
SAPDB_Char *RTESys_FillProtocolGMTTimeStamp(RTESys_Timestamp timeBuffer)
{
    return FillProtocolTimeStamp( timeBuffer,
                                  WANT_GMT_TIMESTAMP,
                                  (SAPDB_UInt8)0 );
}

/*
  Function: RTESys_Time
  Description: Returns seconds since 1.1.1970 0:00 UTC
  Argument: none
  Return value: Seconds since 1.1.1970 0:00 UTC
 */
SAPDB_UInt8 RTESys_Time()
{
    time_t clk ;
	clk = time ( (time_t *) 0 );

    return (SAPDB_UInt8)clk;
}

/*--------------------------------------------------------------------
   @brief    Calculate actual time zone difference in seconds

   To get localtime from gmttime add this difference to the gmtttime returned by RTESys_Time().
   This function is also used by RTESys_BuildTimeStamp().

   @return   the number of seconds (localtime - gmttime)
 */
SAPDB_Int4 RTESys_TimeZoneDelta()
{
    static SAPDB_Int4 timeZoneDelta = 0;

    /* Weekdays may differ due to timezone between localtime and GMT */
    /* Since weekday information [0-6] is a range, rather than an absolute */
    /* value, the difference 0 - 6 and 6 - 0 are special cases. To handle these */
    /* cases, the following array is used. Its index is the difference biased by 6 */
    /* to guarantee a non negativ number */
    static SAPDB_Int4 weekdayDifference[13] = {
         1, /* -6 = 0 - 6 ->  1 localtime sunday, GMT saturday */
         SAPDB_ILLEGAL_WEEKDAY_DIFF, /* -5 = illegal */
         SAPDB_ILLEGAL_WEEKDAY_DIFF, /* -4 = illegal */
         SAPDB_ILLEGAL_WEEKDAY_DIFF, /* -3 = illegal */
         SAPDB_ILLEGAL_WEEKDAY_DIFF, /* -2 = illegal */
        -1, /* -1               localtime one day behind GMT */
         0, /*  0                same day */
         1, /*  1               localtime one day ahead of GMT */
         SAPDB_ILLEGAL_WEEKDAY_DIFF, /*  2 = illegal */
         SAPDB_ILLEGAL_WEEKDAY_DIFF, /*  3 = illegal */
         SAPDB_ILLEGAL_WEEKDAY_DIFF, /*  4 = illegal */
         SAPDB_ILLEGAL_WEEKDAY_DIFF, /*  5 = illegal */
        -1  /*  6 = 6 - 0 -> -1 localtime saturday, GMT sunday */
    };

#if defined(WIN32)

    SYSTEMTIME gmt;
    SYSTEMTIME our;

    GetSystemTime(&gmt);
    GetLocalTime(&our);
    /*
      There could be a jitter in the minute if we have two different calls getting time just 
      when 59 seconds of a minute passed. Therefore the number of seconds reported are checked
      to be greater or equal. Otherwise another minute has begone already in our local time 
      and the gmt info must be adjusted by a minute.
      Adjustment only needs to adjust wMinute,wHour and wDayOfWeek...
     */
    if ( gmt.wSecond > our.wSecond )
    {
        gmt.wMinute++;
        if ( 60 == gmt.wMinute )
        {
           gmt.wHour++;
           if ( 24 == gmt.wHour )
           {
              gmt.wDayOfWeek++;
              if ( 7 == gmt.wDayOfWeek )
              {
                gmt.wDayOfWeek = 0;
              }
           }
        }
    }

    if ( weekdayDifference[SAPDB_WEEKDAY_DIFF_BIAS + our.wDayOfWeek - gmt.wDayOfWeek] != SAPDB_ILLEGAL_WEEKDAY_DIFF )
    {
        /* 
          Time zone difference in seconds
          Weekday difference see above.
          diffMinute is added to gmt.wMinute and module 60 to get the correct value for comparison.
         */
        timeZoneDelta = 60 * ( ( our.wMinute - gmt.wMinute ) 
                               + 60 * ( ( our.wHour - gmt.wHour ) 
                                        + 24 * weekdayDifference[SAPDB_WEEKDAY_DIFF_BIAS + our.wDayOfWeek - gmt.wDayOfWeek]
                                      )
                             );
    }
    else
    {
        timeZoneDelta = 0;
    }
#else

    struct tm gmtBuffer;
    struct tm *gmt = &gmtBuffer;
    struct tm ourBuffer;
    struct tm *our = &ourBuffer;
    time_t now;

    now = time((time_t *)0);
    EmergencyLocalOrGmtTime(now, our, true);
    EmergencyLocalOrGmtTime(now, gmt, false);

    if ( weekdayDifference[SAPDB_WEEKDAY_DIFF_BIAS + our->tm_wday - gmt->tm_wday] != SAPDB_ILLEGAL_WEEKDAY_DIFF )
    {
        /* 
          Return difference (localtime - GMT) in hours:
          Weekday difference see above
         */
        timeZoneDelta = 60 * ( ( our->tm_min  - gmt->tm_min )
                             + 60 * ( ( our->tm_hour - gmt->tm_hour )
                                    + 24 * weekdayDifference[SAPDB_WEEKDAY_DIFF_BIAS + our->tm_wday - gmt->tm_wday]
                                    )
                             );
    }
    else
    {
        timeZoneDelta = 0;
    }
#endif
    
    return timeZoneDelta;
}

/*!--------------------------------------------------------------------
   @brief    Convert seconds since 1.1.1970 0:00 UTC in local time information
   @param    secondsSinceEpoch [in]  Seconds since 1.1.1970 0:00 UTC
   @param    timeBuffer [out] the time buffer to fill
   @return   Pointer to first character
 */
SAPDB_Char *RTESys_BuildTimeStamp( SAPDB_UInt8      secondsSinceEpoch,
                                   RTESys_Timestamp timeBuffer)
{
    return FillProtocolTimeStamp( timeBuffer, 
                                  WANT_LOCAL_TIMESTAMP,
                                  secondsSinceEpoch );
}

/*!--------------------------------------------------------------------
   @brief    Convert seconds since 1.1.1970 0:00 UTC in GMT time information
   @param    secondsSinceEpoch [in]  Seconds since 1.1.1970 0:00 UTC
   @param    timeBuffer [out] the time buffer to fill
   @return   Pointer to first character
 */
SAPDB_Char *RTESys_BuildGMTTimeStamp( SAPDB_UInt8      secondsSinceEpoch,
                                      RTESys_Timestamp timeBuffer)
{
    return FillProtocolTimeStamp( timeBuffer, 
                                  WANT_GMT_TIMESTAMP,
                                  secondsSinceEpoch );
}

/*!--------------------------------------------------------------------
   @brief    Fill the given kernel timestamp buffer with local time information

   Format SAPDB_SQLTIMESTAMP_FORMAT YYYYMMDDHHMMSSmmmuuu

   @param    microSecondsSinceEpoch [in] micro seconds since epoch
   @param    timeBuffer [out] the time buffer to fill with local time
   @return   Pointer to first character
 */
SAPDB_Char *RTESys_BuildSQLTimeStamp( SAPDB_UInt8 microSecondsSinceEpoch,
                                      RTESys_SQLTimestamp VAR_ARRAY_REF timeBuffer)
{
    SAPDB_Char *ptr;
#if defined(WIN32)
    SYSTEMTIME dateTime;
    union {
        FILETIME asFILETIME;
        SAPDB_UInt8 asUInt8;
    } utcTime;

    utcTime.asUInt8 = ( ( microSecondsSinceEpoch * (SAPDB_UInt8)10 )
                      + ( RTESys_TimeZoneDelta() * RTESYS_SEC_TO_100NANO )
                      + (*(SAPDB_UInt8 *)&RTESys_epoch) );
    FileTimeToSystemTime(&utcTime.asFILETIME, &dateTime);

    ptr = &timeBuffer[sizeof(RTESys_SQLTimestamp)];
    /* no microseconds in SYSTEMTIME */
    ptr = FillDigits( (SAPDB_UInt8)0,                      ptr, 3, (SAPDB_Char)0);
    ptr = FillDigits( (SAPDB_UInt8)dateTime.wMilliseconds, ptr, 3, (SAPDB_Char)0);
    ptr = FillDigits( (SAPDB_UInt8)dateTime.wSecond,       ptr, 2, (SAPDB_Char)0);
    ptr = FillDigits( (SAPDB_UInt8)dateTime.wMinute,       ptr, 2, (SAPDB_Char)0);
    ptr = FillDigits( (SAPDB_UInt8)dateTime.wHour,         ptr, 2, (SAPDB_Char)0);
    ptr = FillDigits( (SAPDB_UInt8)dateTime.wDay,          ptr, 2, (SAPDB_Char)0);
    ptr = FillDigits( (SAPDB_UInt8)dateTime.wMonth,        ptr, 2, (SAPDB_Char)0);
    ptr = FillDigits( (SAPDB_UInt8)dateTime.wYear,         ptr, 4, (SAPDB_Char)0);
#else
    time_t utcTime = (time_t)(microSecondsSinceEpoch/1000000);
    struct tm localTimeBuffer;
    struct tm *tim = &localTimeBuffer;

    EmergencyLocalOrGmtTime( utcTime, tim, WANT_LOCAL_TIMESTAMP );

    ptr = (SAPDB_Char *)&timeBuffer[sizeof(RTESys_SQLTimestamp)];
	ptr = FillDigits( microSecondsSinceEpoch%((SAPDB_UInt8)1000000), ptr, 6, (SAPDB_Char)0);
	ptr = FillDigits( (SAPDB_UInt8)tim->tm_sec,                      ptr, 2, (SAPDB_Char)0);
    ptr = FillDigits( (SAPDB_UInt8)tim->tm_min,                      ptr, 2, (SAPDB_Char)0);
    ptr = FillDigits( (SAPDB_UInt8)tim->tm_hour,                     ptr, 2, (SAPDB_Char)0);
    ptr = FillDigits( (SAPDB_UInt8)tim->tm_mday,                     ptr, 2, (SAPDB_Char)0);
    ptr = FillDigits( (SAPDB_UInt8)(tim->tm_mon+1),                  ptr, 2, (SAPDB_Char)0);
    ptr = FillDigits( (SAPDB_UInt8)(tim->tm_year+1900),              ptr, 4, (SAPDB_Char)0);
#endif
    return  (SAPDB_Char *)&timeBuffer[0];
}

/*!--------------------------------------------------------------------
   @brief    Fill the given kernel timestamp buffer with GMT time information
 
   Format SAPDB_SQLTIMESTAMP_FORMAT YYYYMMDDHHMMSSmmmuuu

   @param    microSecondsSinceEpoch [in] micro seconds since epoch
   @param    timeBuffer [out] the time buffer to fill with GMT time
   @return   Pointer to first character
 */
SAPDB_Char *RTESys_BuildSQLGMTTimeStamp( SAPDB_UInt8 microSecondsSinceEpoch,
                                                 RTESys_SQLTimestamp VAR_ARRAY_REF timeBuffer)
{
    SAPDB_Char *ptr;
#if defined(WIN32)
    SYSTEMTIME dateTime;
    union {
        FILETIME asFILETIME;
        SAPDB_UInt8 asUInt8;
    } utcTime;

    utcTime.asUInt8 = ( ( microSecondsSinceEpoch * (SAPDB_UInt8)10 )
                      + (*(SAPDB_UInt8 *)&RTESys_epoch) );
    FileTimeToSystemTime(&utcTime.asFILETIME, &dateTime);

    ptr = &timeBuffer[sizeof(RTESys_SQLTimestamp)];
    /* no microseconds in SYSTEMTIME */
    ptr = FillDigits( (SAPDB_UInt8)0,                      ptr, 3, (SAPDB_Char)0);
    ptr = FillDigits( (SAPDB_UInt8)dateTime.wMilliseconds, ptr, 3, (SAPDB_Char)0);
    ptr = FillDigits( (SAPDB_UInt8)dateTime.wSecond,       ptr, 2, (SAPDB_Char)0);
    ptr = FillDigits( (SAPDB_UInt8)dateTime.wMinute,       ptr, 2, (SAPDB_Char)0);
    ptr = FillDigits( (SAPDB_UInt8)dateTime.wHour,         ptr, 2, (SAPDB_Char)0);
    ptr = FillDigits( (SAPDB_UInt8)dateTime.wDay,          ptr, 2, (SAPDB_Char)0);
    ptr = FillDigits( (SAPDB_UInt8)dateTime.wMonth,        ptr, 2, (SAPDB_Char)0);
    ptr = FillDigits( (SAPDB_UInt8)dateTime.wYear,         ptr, 4, (SAPDB_Char)0);
#else
    time_t utcTime = (time_t)(microSecondsSinceEpoch/1000000);
    struct tm gmtTimeBuffer;
    struct tm *tim = &gmtTimeBuffer;

    EmergencyLocalOrGmtTime( utcTime, tim, WANT_GMT_TIMESTAMP );

    ptr = (SAPDB_Char *)&timeBuffer[sizeof(RTESys_SQLTimestamp)];
	ptr = FillDigits( microSecondsSinceEpoch%((SAPDB_UInt8)1000000), ptr, 6, (SAPDB_Char)0);
	ptr = FillDigits( (SAPDB_UInt8)tim->tm_sec,                      ptr, 2, (SAPDB_Char)0);
    ptr = FillDigits( (SAPDB_UInt8)tim->tm_min,                      ptr, 2, (SAPDB_Char)0);
    ptr = FillDigits( (SAPDB_UInt8)tim->tm_hour,                     ptr, 2, (SAPDB_Char)0);
    ptr = FillDigits( (SAPDB_UInt8)tim->tm_mday,                     ptr, 2, (SAPDB_Char)0);
    ptr = FillDigits( (SAPDB_UInt8)(tim->tm_mon+1),                  ptr, 2, (SAPDB_Char)0);
    ptr = FillDigits( (SAPDB_UInt8)(tim->tm_year+1900),              ptr, 4, (SAPDB_Char)0);
#endif
    return  (SAPDB_Char *)&timeBuffer[0];
}

/*!--------------------------------------------------------------------
   @brief  Get current time in kernel internal time format

   The kernel internally uses a special numeric representation for time stamps.
   Each digit of the localtime YYYYMMDD hhmmss is in the range [0-9]. This allows
   to form the numeric value for date as
    kernelDate = DD + 100 * MM + 10000 * YYYY
   and the numeric value for
    kernelTime = ss + 100 * mm + 10000 * hh
   The advantage is that a decimal representation of this time stamp number is readable!!!!
   .
   @param   kernelDate [out] kernel internal numeric format for date
   @param   kernelTime [out] kernel internal numeric format for time
   @return  0 if timeout wa exceeded, else number of waiting intervals left!
 */
externC void RTESys_FillKernelTimeStamp( SAPDB_UInt4 * kernelDate,
                                         SAPDB_UInt4 * kernelTime)
{
#if defined(_WIN32)
    /*----------------*/
        /* Windows */
    /*----------------*/

    SYSTEMTIME   dateTime;

    GetLocalTime(&dateTime);

    *kernelDate = dateTime.wDay    + 100 * ( dateTime.wMonth  + 100 * dateTime.wYear );
    *kernelTime = dateTime.wSecond + 100 * ( dateTime.wMinute + 100 * dateTime.wHour );
#else
    struct tm localTimeBuffer;
    struct tm *tim = &localTimeBuffer;
        
    EmergencyLocalOrGmtTime( time ( (time_t *) 0 ), tim, WANT_LOCAL_TIMESTAMP );

    *kernelDate = tim->tm_mday + 100 * ( (tim->tm_mon+1) + 100 * (tim->tm_year+1900) );
    *kernelTime = tim->tm_sec  + 100 * ( tim->tm_min     + 100 * tim->tm_hour        );
#endif
}

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
  from the current number of seconds before conversion.

   @param   delayTime  [in]  delay time in seconds
   @param   kernelDate [out] kernel internal numeric format for date
   @param   kernelTime [out] kernel internal numeric format for time
   @return  0 if timeout wa exceeded, else number of waiting intervals left!
 */
externC void RTESys_FillDelayedKernelTimeStamp(SAPDB_Int4       const    delayTime,
                                               SAPDB_UInt4 VAR_VALUE_REF kernelDate,
                                               SAPDB_UInt4 VAR_VALUE_REF kernelTime)
{
#if defined(_WIN32)
    /*----------------*/
        /* Windows */
    /*----------------*/

    SYSTEMTIME   dateTime;

    union {
        FILETIME asFILETIME;
        SAPDB_UInt8 asUInt8;
    } conversionTime;

    GetLocalTime(&dateTime);

    SystemTimeToFileTime(&dateTime, &(conversionTime.asFILETIME));

    conversionTime.asUInt8 = (SAPDB_UInt8)(conversionTime.asUInt8 - (delayTime * RTESYS_SEC_TO_100NANO) );

    FileTimeToSystemTime(&conversionTime.asFILETIME, &dateTime);

    *kernelDate = dateTime.wDay    + 100 * ( dateTime.wMonth  + 100 * dateTime.wYear );
    *kernelTime = dateTime.wSecond + 100 * ( dateTime.wMinute + 100 * dateTime.wHour );
#else
    struct tm localTimeBuffer;
    struct tm *tim = &localTimeBuffer;
        
    EmergencyLocalOrGmtTime( time ( (time_t *) 0 ) - delayTime, tim, WANT_LOCAL_TIMESTAMP );

    *kernelDate = tim->tm_mday + 100 * ( (tim->tm_mon+1) + 100 * (tim->tm_year+1900) );
    *kernelTime = tim->tm_sec  + 100 * ( tim->tm_min     + 100 * tim->tm_hour        );
#endif
}

/*!--------------------------------------------------------------------
   @brief  Convert the UTC time stamp into kernel internal time format

   The kernel internally uses a special numeric representation for time stamps.
   Each digit of the localtime YYYYMMDD hhmmss is in the range [0-9]. This allows
   to form the numeric value for date as
    kernelDate = DD + 100 * MM + 10000 * YYYY
   and the numeric value for
    kernelTime = ss + 100 * mm + 10000 * hh
   The advantage is that a decimal representation of this time stamp number is readable!!!!
   .
   @param   utcTimeStamp [in] number of seconds since epoch
   @param   kernelDate [out] kernel internal numeric format for date
   @param   kernelTime [out] kernel internal numeric format for time
   @return  0 if timeout wa exceeded, else number of waiting intervals left!
 */
externC void RTESys_FillKernelTimeStampFromUTC( SAPDB_UInt8 utcTimeStamp,
                                                SAPDB_UInt4 * kernelDate,
                                                SAPDB_UInt4 * kernelTime)
{
#if defined(_WIN32)
    /*----------------*/
        /* Windows */
    /*----------------*/

    SYSTEMTIME   dateTime;

    /* convert given time in seconds since epoch... */

    union {
        FILETIME asFILETIME;
        SAPDB_UInt8 asUInt8;
    } utcTime;

    utcTime.asUInt8 = ((SAPDB_UInt8)(utcTimeStamp + RTESys_TimeZoneDelta()) * RTESYS_SEC_TO_100NANO) + (*(SAPDB_UInt8 *)&RTESys_epoch) ;

    FileTimeToSystemTime(&utcTime.asFILETIME, &dateTime);

    *kernelDate = dateTime.wDay    + 100 * ( dateTime.wMonth  + 100 * dateTime.wYear );
    *kernelTime = dateTime.wSecond + 100 * ( dateTime.wMinute + 100 * dateTime.wHour );
#else
    /*----------------*/
         /* UNIX */
    /*----------------*/

	/*
	 *  Create date and time stamp
	 */
    struct tm localTimeBuffer;
    struct tm *tim = &localTimeBuffer;
    
    EmergencyLocalOrGmtTime( (time_t)utcTimeStamp, tim, WANT_LOCAL_TIMESTAMP );

    *kernelDate = tim->tm_mday + 100 * ( (tim->tm_mon+1) + 100 * (tim->tm_year+1900) );
    *kernelTime = tim->tm_sec  + 100 * ( tim->tm_min     + 100 * tim->tm_hour        );
#endif
}

/*!--------------------------------------------------------------------
   @brief  Convert the given kernel internal time format into the
           RTESys_SQLTimestamp format "YYYYMMDDHHMMSS000000"

   @param   kernelDate  [in] kernel internal numeric format for date
   @param   kernelTime  [in] kernel internal numeric format for time
   @param   timeBuffer [out] the time buffer to fill
   @return  Pointer to first character
 */

externC SAPDB_Char * 
RTESys_FillSQLTimeStampFromKernelTimeStamp(
                                           const SAPDB_UInt4 kernelDate,
                                           const SAPDB_UInt4 kernelTime,
                                           RTESys_SQLTimestamp VAR_ARRAY_REF timeBuffer)
{
    SAPDB_Char *ptr = (SAPDB_Char *)&timeBuffer[ sizeof(RTESys_SQLTimestamp) ];
    
    ptr = FillDigits(( SAPDB_UInt8)0,          ptr, 6, (SAPDB_Char)0);
    ptr = FillDigits(( SAPDB_UInt8)kernelTime, ptr, 6, (SAPDB_Char)0);
    ptr = FillDigits(( SAPDB_UInt8)kernelDate, ptr, 8, (SAPDB_Char)0);
    
    return  (SAPDB_Char *)&timeBuffer[0];
}

/*------------------------------------------------------------------------------*/
/* Crash on UNIX is a special case, where normal time stamp code cannot be used */
/* Reason is that no libc function is synchronous signal save....               */
/* To handle this situation, the initial action in a crash signal handler is to */
/* setup a flag, that allows to detect this situation                           */
/*------------------------------------------------------------------------------*/

/*
  Function: RTESys_SetInCrashSignalHandler
  Description: Set flag to indicate we are running inside a crash signal handler

  Some of the function we use are not async signal save. To prevent calling them the signal
  handler must call this flag before issuing any of the system calls
 */
void RTESys_SetInCrashSignalHandler()
{
    RTESys_InCrashSignalHandler = 1;
}

/*
  Function: RTESys_ResetInCrashSignalHandler
  Description: Reset flag to indicate we are running outside a crash signal handler

  Some of the function we use are not async signal save. To prevent calling them the signal
  handler must call this global flag before issuing any of the system calls
 */
void RTESys_ResetInCrashSignalHandler()
{
    RTESys_InCrashSignalHandler = 0;
}

/*
  Function: RTESys_IsInCrashSignalHandler
  Description: Check flag to indicate we are running inside a crash signal handler
  Return value: true if flag set, false if flag is reset
 */
SAPDB_Bool RTESys_IsInCrashSignalHandler()
{
    return (SAPDB_Bool)(0 != RTESys_InCrashSignalHandler);
}

/*!--------------------------------------------------------------------
   @brief  Check flag until true or timeout exceeded

   The flag is normally modified by another thread or process.
   The waiting interval time is fixed to 100 milliseconds. If no waiting time
   is given, the call only tests the flag and returns 1 if flag is true, 0 if not!
   @param   pFlag [in] pointer to flag (i.e. in shared memory)
   @param   waitingIntervalsOf100msec [in] number of waiting intervals (minimum 1)
   @return  0 if timeout wa exceeded, else number of waiting intervals left!
 */
SAPDB_UInt4 RTESys_Timeout(bool *pFlag, SAPDB_UInt4 waitingIntervalsOf100msec)
{
    SAPDB_UInt intervalsLeft = ( waitingIntervalsOf100msec > 0 ? waitingIntervalsOf100msec : 1 );

    while ( ( false == *pFlag )
         && ( intervalsLeft > 1 ) )
    {
        --intervalsLeft;
        /* 100 milliseconds polling period */
#if defined(_WIN32)
        Sleep( 100 );
#else
        {
            struct pollfd dummy[1];
            dummy[0].fd = -1;
            dummy[0].events = 0;

            poll(dummy, 1, 100);
        }
#endif
    }

    return intervalsLeft;
}

/*===========================================================================*
 *  STATUC FUNCTIONS                                                         *
 *===========================================================================*/

/*
  Function: FillDigits
  Description: Fill the given buffer with ascii representation of given number of digits


  Argument: value [in] number to convert
            ptrBehindLastDigit [in] pointer behind last digit to write
			digitCount [in] number of digits to write
			separator [in] seprator to add after last digit written
  Return value: Pointer to last written character
 */
static SAPDB_Char *FillDigits(SAPDB_UInt8 value,
                              SAPDB_Char *ptrBehindLastDigit,
                              SAPDB_Int4 digitCount, 
                              SAPDB_Char separator)
{
    SAPDB_Int4 nDigits = digitCount;
    SAPDB_UInt8 tmpValue = value;
    SAPDB_Char *ptr = ptrBehindLastDigit;

    while ( nDigits > 1 )
    {
        *--ptr = '0'+(SAPDB_Char)(value % 10);
        value /= 10;
        --nDigits;
    }
    *--ptr = '0'+(SAPDB_Char)value;
    if ( 0 != separator )
    {
        *--ptr = separator;
    }

    return ptr;
}

/*--------------------------------*/

#if !defined(_WIN32)
static struct tm lastTime_buffer = { 0 }; /* PTS 1112083 */
static time_t    lastClk = 0; 

static void EmergencyLocalOrGmtTime(time_t clk, struct tm *tim, SAPDB_Int4 asLocalTime )
{
    if ( !RTESys_InCrashSignalHandler )
    {
#ifdef _REENTRANT
        if ( asLocalTime )
        {
            (void)localtime_r( &clk, tim ); /* Not async signal save!!! */
        }
        else
        {
            (void)gmtime_r( &clk, tim ); /* Not async signal save!!! */
        }
#else
        struct tm *staticTim;
        if ( asLocalTime )
        {
            staticTim = localtime( &clk ); /* Not async signal save!!! */
        }
        else
        {
            staticTim = gmtime( &clk ); /* Not async signal save!!! */
        }
		SAPDB_memcpy(tim, staticTim, sizeof(struct tm));
#endif
		SAPDB_memcpy(&lastTime_buffer, tim, sizeof(struct tm));
	}
	else
	{
        /*
         * We have no way to detect, that a 'free()' is crashing the database.
         * Since 'localtime_r' uses free() internally, we have to prevent from using
         * it during crash. Therefore we only output 'clk' as seconds since 1.1.1970
         */

		/* PTS 1112083 since stack back trace is needed for support, 'clk' output was visible in knldiag.err */
		/* To make it reasonable the last timestamp produced is used and the time difference is used to update */
		/* A difference in weekdays is not correctly handled, but... */
		long timeDiff = clk - lastClk;
		if ( timeDiff > 0 )
		{
			lastTime_buffer.tm_sec += (timeDiff % 60);
			timeDiff /= 60;
			if ( timeDiff > 0 )
			{
				lastTime_buffer.tm_min += (timeDiff % 60);
                timeDiff /= 60;
				if ( timeDiff > 0 )
				{
					lastTime_buffer.tm_hour += (timeDiff % 24);
                    timeDiff /= 24;
					if ( timeDiff > 0 )
					{
						lastTime_buffer.tm_mday += timeDiff;
					}
				}
			}

            /* work on overflows... */
			if ( lastTime_buffer.tm_sec >= 60 )
			{
			   lastTime_buffer.tm_min += lastTime_buffer.tm_sec/60;
			   lastTime_buffer.tm_sec %= 60;
			}

			if ( lastTime_buffer.tm_min >= 60 )
			{
			   lastTime_buffer.tm_hour += lastTime_buffer.tm_min/60;
			   lastTime_buffer.tm_min %= 60;
			}

			if ( lastTime_buffer.tm_hour >= 60 )
			{
			   lastTime_buffer.tm_mday += lastTime_buffer.tm_hour/24;
			   lastTime_buffer.tm_hour %= 24;
			}

            /* overflow on mday are accepted... */
		}
		SAPDB_memcpy(tim, &lastTime_buffer, sizeof(struct tm));
	}

    lastClk = clk;
}
#endif

/*--------------------------------*/

static SAPDB_Char *FillProtocolTimeStamp( RTESys_Timestamp timeBuffer,
                                          SAPDB_Int4 asLocalTime,
                                          SAPDB_UInt8 givenTime )
{
    
    SAPDB_Char  *ptr = &timeBuffer[sizeof(RTESys_Timestamp)-1];
    /* ptr starts at end of buffer and is used to fill beginning with least significant digits */
    *ptr = 0;
    {
#if defined(_WIN32)
        /*----------------*/
            /* Windows */
        /*----------------*/

        SYSTEMTIME   dateTime;

        if ( 0 == givenTime )
        {
            if ( asLocalTime )
            {
                GetLocalTime(&dateTime);
            }
            else
            {
                GetSystemTime(&dateTime);
            }
        }
        else
        {
            /* convert given time in seconds since epoch... */

            union {
                FILETIME asFILETIME;
                SAPDB_UInt8 asUInt8;
            } utcTime;

            if ( asLocalTime )
            {
                utcTime.asUInt8 = ((givenTime + RTESys_TimeZoneDelta()) * RTESYS_SEC_TO_100NANO) + (*(SAPDB_UInt8 *)&RTESys_epoch);
            }
            else
            {
                utcTime.asUInt8 = (givenTime * RTESYS_SEC_TO_100NANO) + (*(SAPDB_UInt8 *)&RTESys_epoch);
            }

            FileTimeToSystemTime(&utcTime.asFILETIME, &dateTime);
        }

        ptr = FillDigits( (SAPDB_UInt8)dateTime.wSecond, ptr, 2, (SAPDB_Char)':');
        ptr = FillDigits( (SAPDB_UInt8)dateTime.wMinute, ptr, 2, (SAPDB_Char)':');
        ptr = FillDigits( (SAPDB_UInt8)dateTime.wHour,   ptr, 2, (SAPDB_Char)' ');
        ptr = FillDigits( (SAPDB_UInt8)dateTime.wDay,    ptr, 2, (SAPDB_Char)'-');
        ptr = FillDigits( (SAPDB_UInt8)dateTime.wMonth,  ptr, 2, (SAPDB_Char)'-');
        ptr = FillDigits( (SAPDB_UInt8)dateTime.wYear,   ptr, 4, (SAPDB_Char)0  );
#else

        /*----------------*/
             /* UNIX */
        /*----------------*/

	    /*
	     *  Create date and time stamp
	     */
        struct tm timeBuffer;
        struct tm *tim = &timeBuffer;
        
        if ( 0 == givenTime )
        {
            EmergencyLocalOrGmtTime( time ( (time_t *) 0 ), tim, asLocalTime );
        }
        else
        {
            EmergencyLocalOrGmtTime( (time_t)givenTime, tim, asLocalTime );
        }

		ptr = FillDigits( (SAPDB_UInt8)tim->tm_sec,         ptr, 2, (SAPDB_Char)':');
        ptr = FillDigits( (SAPDB_UInt8)tim->tm_min,         ptr, 2, (SAPDB_Char)':');
        ptr = FillDigits( (SAPDB_UInt8)tim->tm_hour,        ptr, 2, (SAPDB_Char)' ');
        ptr = FillDigits( (SAPDB_UInt8)tim->tm_mday,        ptr, 2, (SAPDB_Char)'-');
        ptr = FillDigits( (SAPDB_UInt8)(tim->tm_mon+1),     ptr, 2, (SAPDB_Char)'-');
        ptr = FillDigits( (SAPDB_UInt8)(tim->tm_year+1900), ptr, 4, (SAPDB_Char)0  );
#endif
    }
    return ptr;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/