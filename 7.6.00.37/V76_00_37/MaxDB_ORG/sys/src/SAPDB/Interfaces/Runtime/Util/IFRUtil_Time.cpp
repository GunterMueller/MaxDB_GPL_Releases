/*!
  @file           IFRUtil_Time.cpp
  @author         D039759
  @ingroup        IFR_Util
  @brief          Time supplementals

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
#include "Interfaces/Runtime/Util/IFRUtil_Time.h"

#ifndef _WIN32
#  if !defined(SVR4) && !defined(SOLARIS)
#    if defined(LINUX) || defined(AIX)
#      include <time.h>
#    endif
#    include <sys/time.h>
#  else
#    ifdef _POSIX_SOURCE
#      undef _POSIX_SOURCE
#      include <sys/time.h>
#      define _POSIX_SOURCE
#    else
#      include <sys/time.h>
#    endif
#  endif
#endif

IFR_BEGIN_NAMESPACE

//----------------------------------------------------------------------
void
IFRUtil_Time::currentTimeStamp(char *dest)
{
    memcpy(dest, "0000-00-00 00:00:00.000000", 27);
#ifdef _WIN32
    SYSTEMTIME systime;
    GetLocalTime(&systime);

    IFR_UInt4  tmp=systime.wYear;
    
    dest[0] += tmp / 1000; tmp %= 1000;
    dest[1] += tmp / 100; tmp %= 100;
    dest[2] += tmp / 10;
    dest[3] += tmp % 10;
    // dest[4] 
    dest[5] += systime.wMonth / 10;
    dest[6] += systime.wMonth % 10;
    // dest[7] 
    dest[8] += systime.wDay / 10;
    dest[9] += systime.wDay % 10;
    // dest[10] 
    dest[11] += systime.wHour / 10;
    dest[12] += systime.wHour % 10;
    // dest[13] 
    dest[14] += systime.wMinute / 10;
    dest[15] += systime.wMinute % 10;
    // dest[16] 
    dest[17] += systime.wSecond / 10;
    dest[18] += systime.wSecond % 10;
    // dest[19] 
    tmp = systime.wMilliseconds;
    dest[20] += tmp / 100; tmp %= 100;
    dest[21] += tmp / 10;
    dest[22] += tmp % 10;
    // dest[23] 
    // dest[24] 
    // dest[25] 
#else
    struct timeval tv;
    gettimeofday(&tv, 0);
    struct tm tmval; 
    localtime_r(&tv.tv_sec,
                &tmval);

    IFR_UInt4  tmp=tmval.tm_year + 1900;
    
    dest[0] += tmp / 1000; tmp %= 1000;
    dest[1] += tmp / 100; tmp %= 100;
    dest[2] += tmp / 10;
    dest[3] += tmp % 10;
    // dest[4] 
    dest[5] += (tmval.tm_mon +1) / 10;
    dest[6] += (tmval.tm_mon +1) % 10;
    // dest[7] 
    dest[8] += tmval.tm_mday / 10;
    dest[9] += tmval.tm_mday % 10;
    // dest[10] 
    dest[11] += tmval.tm_hour / 10;
    dest[12] += tmval.tm_hour % 10;
    // dest[13] 
    dest[14] += tmval.tm_min / 10;
    dest[15] += tmval.tm_min % 10;
    // dest[16] 
    dest[17] += tmval.tm_sec / 10;
    dest[18] += tmval.tm_sec % 10;
    // dest[19] 
    tmp = tv.tv_usec;
    
    dest[20] += tmp / 100000; tmp %= 100000;
    dest[21] += tmp / 10000; tmp %= 10000;
    dest[22] += tmp / 1000; tmp %= 1000;
    dest[23] += tmp / 100; tmp %= 100;
    dest[24] += tmp / 10;
    dest[25] += tmp % 10;
#endif    
}

#ifdef HAVE_RTESYS_MICROTIME
//----------------------------------------------------------------------
IFR_UInt8
IFRUtil_Time::currentMicroSeconds()
{
    return RTESys_MicroSecTimer();
}
#else 
//----------------------------------------------------------------------
#ifdef _WIN32
static IFR_UInt8 time_performancefreq;
#endif

IFR_UInt8
IFRUtil_Time::currentMicroSeconds()
{
#ifdef _WIN32
    if(!time_performancefreq) {
        QueryPerformanceFrequency((LARGE_INTEGER*)&time_performancefreq);
        if(!time_performancefreq) {
            time_performancefreq=1;
        }
    }
    IFR_UInt8 counter=0;
    QueryPerformanceCounter((LARGE_INTEGER*)&counter);
    // Conversion from unsigned 64 bit integer isn't possible
    return (IFR_UInt8) ( ( (IFR_Int8)counter  * (double)1000000.0 ) / (IFR_Int8)time_performancefreq );
#else
    struct timeval tv;
    gettimeofday(&tv, 0);
    struct tm tmval; 
    localtime_r(&tv.tv_sec,
                &tmval);
    
    IFR_Int8 result = tmval.tm_sec + tmval.tm_min * 60 + tmval.tm_hour * 3600;
    result *= 1000000;
    return result + tv.tv_usec;
#endif
}
#endif // HAVE_RTESYS_MICROTIME

IFR_END_NAMESPACE
