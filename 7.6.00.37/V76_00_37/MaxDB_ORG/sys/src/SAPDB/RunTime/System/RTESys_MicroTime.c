/*!---------------------------------------------------------------------
  @file           RTESys_MicroTime.c
  @author         JoergM
  @ingroup        RunTime
  @brief          Microsecond timer

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


//
//   FreeBSD portions added by Kai Mosebach,
//   For more informations see : http://www.komadev.de/sapdb
//

\endif
---------------------------------------------------------------------*/




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#if defined(LINUX)
#   define _GNU_SOURCE 1
#   include <sched.h>
#endif

#include "RunTime/System/RTESys_MicroTime.h"

#if defined(_WIN32)
#include "RunTime/System/RTESys_NTHighResTimer.h" /* nocheck */
#endif

#include <time.h>

#ifdef AIX
#   include     <sys/time.h>
    /*  AIX 4.1 and newer - use routines 'read_real_time' and 'time_base_to_time' */
#define HAS_READ_REAL_TIME
#endif

#if defined(SUN) || defined(SOLARIS)
#   include     <sys/time.h>
#define HAS_GETHRTIME
#endif

#ifdef HPUX
/* defined in /usr/include/time.h */
#define HAS_GETHRTIME
#endif

#ifdef OSF1
#   include     <sys/time.h>
#endif

#if defined(LINUX)
#  define HAS_GET_CYCLES
#  if defined(HAS_GET_CYCLES)
#    include <asm/param.h>
#    include <asm/types.h>
#    if defined(IA64)
#      include <asm/intrinsics.h>

       typedef unsigned long cycles_t;

       static inline cycles_t
       get_cycles (void)
       {
		   cycles_t ret;
		   ret = ia64_getreg(_IA64_REG_AR_ITC);
		   return ret;
	   }

#    else
#      include <asm/timex.h>
#    endif
#  endif
   typedef SAPDB_UInt8 u64;
#endif

#if defined FREEBSD
#   include     <sys/time.h>
#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define RTESYS_SECONDS_TO_MICROSECONDS(s_) (((SAPDB_UInt8)s_)*1000000U)
#define RTESYS_NANOSECONDS_TO_MICROSECONDS(s_) (((SAPDB_UInt8)s_)/1000U)

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  STRUCTURES, TYPES, UNIONS ...                                            *
 *===========================================================================*/

/*
  Value needed to return number of microseconds since start
  These make the call RTESys_InitMicroSecTimer() not multithread save,
  which is acceptable to prevent the otherwise needed spinlock coding...
 */
static SAPDB_UInt8 databaseStartTimeInMicroseconds = 0;

#if defined(HAS_GET_CYCLES)
static SAPDB_Bool RTESys_forceUseOfGettimeofday = true;
static double RTESys_microsecond_per_cycle = 0.;
#endif

/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/

/*
  Function: CurrentMicroSeconds
  Description: Return the current number of microseconds as returned from high res timer

  The base of the number has nothing to do with database start, but the number is expected to have no overflow...

  Return value: Number of microseconds as returned from high res timer
 */
static SAPDB_UInt8 CurrentMicroSeconds();

/*===========================================================================*
 *  GLOBAL FUNCTIONS                                                         *
 *===========================================================================*/

#if defined(HAS_GET_CYCLES)
/*
   Inspect all CPU timers to be monotonous...
 */
SAPDB_Bool RTESys_LinuxTimerLeapCheck(SAPDB_Bool forceUseOfGettimeofday)
{
    if ( forceUseOfGettimeofday )
    {
        RTESys_forceUseOfGettimeofday = true;
        return false;
    }
    else
    {
        int iRun;
        int iCpu;
        cycles_t firstCycles;
        cycles_t lastTick;
        cycles_t currentTick;
        cpu_set_t cpuset;
        cpu_set_t testset;

		RTESys_forceUseOfGettimeofday = false;
        if ( 0 == sched_getaffinity( 0, sizeof(cpu_set_t), &cpuset) )
        {
            firstCycles = lastTick = get_cycles();
            for ( iRun = 0; iRun < 2; iRun++ ) /* two runs are needed to check last CPU against first CPU */
            {
                for ( iCpu = 0; iCpu < (sizeof(cpu_set_t)*8); ++iCpu )
                {
                    if ( CPU_ISSET(iCpu, &cpuset) )
                    {
                        CPU_ZERO(&testset);
                        CPU_SET(iCpu, &testset);
                        if ( 0 == sched_setaffinity( 0, sizeof(cpu_set_t), &testset) )
                        {
                            currentTick = get_cycles();
                            /* using cycles difference prevents 'overflow' problem in tick registers... */
                            /* without this the very unlikely case of on register overflow could produce */
                            /* a 'false' being returned with a working synchronized timer set */
                            if ( (currentTick - firstCycles) < (lastTick - firstCycles) )
                            {
                                RTESys_forceUseOfGettimeofday = true;
                                break;
                            }
                            else
                            {
                                lastTick = get_cycles();
                            }
						} else {
							RTESys_forceUseOfGettimeofday = true;
							break;
						}
                    }
                }
            }
            sched_setaffinity( 0, sizeof(cpu_set_t), &cpuset);
		} else {
			RTESys_forceUseOfGettimeofday = true;
		}

        return !RTESys_forceUseOfGettimeofday;
    }
}
#endif

/*!--------------------------------------------------------------------
   @description    Initialize timer for microseconds since start of database

 --------------------------------------------------------------------*/

void RTESys_InitMicroSecTimer()
{
#if defined(HAS_GET_CYCLES)
    if ( !RTESys_forceUseOfGettimeofday )
    {
        struct timeval start_time;
        struct timeval end_time;
        SAPDB_UInt8 start_ticks;
        SAPDB_UInt8 end_ticks;

        (void) gettimeofday ( &start_time , (void *)0 );
        start_ticks = get_cycles();
        sleep(5);
        end_ticks   = get_cycles();
        (void) gettimeofday ( &end_time , (void *)0 );

        RTESys_microsecond_per_cycle = ( ( end_time.tv_sec  - start_time.tv_sec  ) * 1000000 )
                                    + end_time.tv_usec - start_time.tv_usec;
        RTESys_microsecond_per_cycle /= (end_ticks - start_ticks);
    }
#endif
    databaseStartTimeInMicroseconds = CurrentMicroSeconds();
}

/*
  Function: RTESys_MicroSecTimer
  Description: Return number of microseconds since start of database

  Return value: Number of microseconds since database start
 */
SAPDB_UInt8 RTESys_MicroSecTimer()
{
    return CurrentMicroSeconds() - databaseStartTimeInMicroseconds;
}

/*===========================================================================*
 *  STATUC FUNCTIONS                                                         *
 *===========================================================================*/

static SAPDB_UInt8 CurrentMicroSeconds()
{
#if defined(_WIN32)

    return RTESys_NTHRTimerMicroSeconds();

#elif defined(HAS_READ_REAL_TIME)

    timebasestruct_t time_now ;

    (void) read_real_time    ( & time_now , TIMEBASE_SZ );
    /* This yields processor-specific units - not usable directly */

    (void) time_base_to_time ( & time_now , TIMEBASE_SZ );
    /* Now 'time_now' contains second / nanosecond */

    return ( RTESYS_SECONDS_TO_MICROSECONDS(time_now.tb_high)
           + RTESYS_NANOSECONDS_TO_MICROSECONDS(time_now.tb_low) );

#elif defined(HAS_GETHRTIME)

    hrtime_t currentTimeInNanoSeconds;
    SAPDB_UInt8 resultTime;

    currentTimeInNanoSeconds = gethrtime();
    resultTime = RTESYS_NANOSECONDS_TO_MICROSECONDS(currentTimeInNanoSeconds);

    return resultTime;

#elif defined(HAS_GET_CYCLES)

    if ( !RTESys_forceUseOfGettimeofday 
      && (RTESys_microsecond_per_cycle != 0) )
    {
        return (SAPDB_UInt8)(get_cycles() * RTESys_microsecond_per_cycle);
    }
    else
    {
        struct timeval	tv ;
        (void) gettimeofday ( &tv , (void *)0 );
        return ( RTESYS_SECONDS_TO_MICROSECONDS(tv.tv_sec) + tv.tv_usec );
    }

#else

    /* Fallback of all other platforms */
    struct timeval	tv ;

    (void) gettimeofday ( &tv , (void *)0 );

    return ( RTESYS_SECONDS_TO_MICROSECONDS(tv.tv_sec) + tv.tv_usec );

#endif
}

/*!--------------------------------------------------------------------
   @brief    Returns microsecond since since 1.1.1970 0:00 UTC

   Used as input for RTESys_BuildSQLTimeStamp to generate SQL time stamps. This
   call is not used to get timestamps for internal timing measurements, so it is
   not called very frequently, but it should be as precise as possible and always
   monotonic ascendend.

   @return   Microseconds since 1.1.1970 0:00 UTC
 */
externC SAPDB_UInt8 RTESys_MicroSecondTime()
{
#if defined(WIN32)
    static FILETIME the_epoch = { 3577643008UL, 27111902UL };

    SYSTEMTIME currentTime;
    union {
        FILETIME asFILETIME;
        SAPDB_UInt8 asUInt8;
    } now;

    GetSystemTime(&currentTime);
    SystemTimeToFileTime(&currentTime, &now.asFILETIME);
    return ( (now.asUInt8 - (*(SAPDB_UInt8 *)&the_epoch)) / 10 );
#else
    struct timeval	tv ;

    (void) gettimeofday ( &tv , (void *)0 );

    return ( RTESYS_SECONDS_TO_MICROSECONDS(tv.tv_sec) + tv.tv_usec );
#endif
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
