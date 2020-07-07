/*!**********************************************************************

  module: hwd29date.h

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  Date conversion functions

  see also:     RFC 822

  -----------------------------------------------------------------------

                         Copyright (c) 1998-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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




************************************************************************/

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include <time.h>
#include <string.h>

#include "hwd29date.h"

#include "SAPDBCommon/SAPDB_string.h"

/*!**********************************************************************

  Chapter: Date conversion functions

************************************************************************/

static char *Month[12] = { "Jan", "Feb", "Mar", "Apr", "May",
                           "Jun", "Jul", "Aug", "Sep", "Oct",
                           "Nov", "Dec" };

static char *DayOfWeek[7] = { "Sun", "Mon", "Tue", "Wed", "Thu",
                              "Fri", "Sat" };

SAPDB_Int4 wd29DiffMonth( char *date1, char *date2 )
{
    char year1[5];
    char month1[3];
    char year2[5];
    char month2[3];
    SAPDB_Int4 index1;
    SAPDB_Int4 index2;
    SAPDB_Int4 diff;
    
    SAPDB_memcpy( year1, date1, 4 );
    year1[4] = '\0';
    SAPDB_memcpy( year2, date2, 4 );
    year2[4] = '\0';
    SAPDB_memcpy( month1, &date1[4], 2 );
    month1[2] = '\0';
    SAPDB_memcpy( month2, &date2[4], 2 );
    month2[2] = '\0';

    index1 = atol( year1 ) * 12 + atol( month1 );
    index2 = atol( year2 ) * 12 + atol( month2 );

    diff = index1-index2;
    if (diff < 0) {
        diff = -diff;
    }; /* if */
    
    return diff;
} /* wd29DiffMonth */


/* format odbc date. buf must be at least MAX_DATE_LEN_WD29 bytes big. */
void wd29FormatDate( SAPDB_Int4  format,
                     const char *odbcDate,
                     const char *odbcTime,
                     char       *buf )
{
    char        day[3];
    char        month[3];
    char        year[5];
    tsp00_Date  actDate;
    tsp00_Time  actTime;
    char        rawDate[9];
    char        hour[3];
    char        minute[3];
    char        second[3];
    
    SAPDB_memcpy( day, &odbcDate[8], 2 );
    day[2] = '\0';
    
    SAPDB_memcpy( month, &odbcDate[5], 2 );
    month[2] = '\0';
    
    SAPDB_memcpy( year, odbcDate, 4 );
    year[4] = '\0';

    SAPDB_memcpy( hour, &odbcTime[0], 2 );
    hour[2] = '\0';

    SAPDB_memcpy( minute, &odbcTime[3], 2 );
    minute[2] = '\0';
    
    SAPDB_memcpy( second, &odbcTime[6], 2 );
    second[2] = '\0';

    if (format == DATE_FORMAT_ISO8601_WD29) {
        sprintf( buf, "%s-%s-%sT%s:%s:%sZ",
                 year, month, day, hour, minute, second );
    } else if (format == DATE_FORMAT_RFC822_WD29) {
        SAPDB_Int4  monthIndex = atol( month )-1;
        SAPDB_Int4  dayIndex = atol( day ) -1;
        
	    sprintf( buf, "%s, %s %s %s %s:%s:%s GMT",
                 DayOfWeek[dayIndex], day, Month[monthIndex],
                 year, hour, minute, second );
    };
} /* wd29FormatDate */


void wd29GetActualDate( char *dateBuf )
{
    tsp00_Date date;
    tsp00_Time time;
    SAPDB_Int4 pos = 0;
    
    sqldattime( date, time );
    dateBuf[pos++] = date[0];
    dateBuf[pos++] = date[1];
    dateBuf[pos++] = date[2];
    dateBuf[pos++] = date[3];
    dateBuf[pos++] = '-';
    dateBuf[pos++] = date[4];
    dateBuf[pos++] = date[5];
    dateBuf[pos++] = '-';    
    dateBuf[pos++] = date[6];
    dateBuf[pos++] = date[7];
    dateBuf[pos++] = '\0';
} /* wd29GetActualDate */


void wd29GetActualTime( char *timeBuf )
{
    tsp00_Date date;
    tsp00_Time time;
    SAPDB_Int4 pos = 0;
    
    sqldattime( date, time );
    timeBuf[pos++] = time[2];
    timeBuf[pos++] = time[3];
    timeBuf[pos++] = ':';
    timeBuf[pos++] = time[4];
    timeBuf[pos++] = time[5];
    timeBuf[pos++] = ':';
    timeBuf[pos++] = time[6];
    timeBuf[pos++] = time[7];    
    timeBuf[pos++] = '\0';
} /* wd29GetActualTime */

 
/* returns zero if error */
SAPDB_UInt4 wd29HttpDateToWADate( const char *http_date,
                                  char       *wa_date )
{
    int     year;
    char    month_name[4];
    int     day;
    int     hour;
    int     minute;
    int     second;

    int     month;

    if(strchr(http_date, '-'))      /* RFC 850(1036) format */
        {
            sscanf( http_date,
                    "%*s %2d %*[-] %3s %*[-] %2d %2d %*[:] %2d %*[:] %2d",
                    &day, 
                    month_name, 
                    &year, 
                    &hour, 
                    &minute, 
                    &second );
            if(year <= 70)          /* this format has a Y2K problem, 
                                       but doesn't this look plausible ? 
                                       (should be revisited before 2070) */
                year +=2000;        
            else
                year +=1900;

        }
    else if(strchr(http_date, ',')) /* RFC 822(1123) format */
        {
            sscanf(http_date,"%*s %2d %3s %4d %2d %*[:] %2d %*[:] %2d", 
                   &day, 
                   month_name, 
                   &year, 
                   &hour, 
                   &minute, 
                   &second);

        }
    else                            /* (probably) ANSI C asctime() format */
        {
            sscanf(http_date,"%*s %3s %2d %2d %*[:] %2d %*[:] %2d %4d", 
                   month_name, 
                   &day, 
                   &hour, 
                   &minute, 
                   &second,
                   &year); 

        }
    for(month = 0; month < 12; month++)
        {
            if(!strcmp(month_name, Month[month]))
                break;
        }

    if( (month < 0 || month > 11) ||
        (day < 1 || day > 31) ||
        (hour < 0 || hour > 23) ||
        (minute < 0 || minute > 59) ||
        (second < 0 || second > 61) )  /* we allow two leap seconds */
        {
            /* WebDbLog("HttpDateToWADate :  invalid HTTP Date"); */
            return 0;
        }

    sprintf(wa_date, 
            "%4d%02d%02d%02d%02d%02d", 
            year,
            month + 1,
            day,
            hour,
            minute,
            second);
    
    return 1;
} /* wd29HttpDateToWADate */


SAPDB_UInt4 wd29WADateToHttpDate( const char *my_date,
                                  char       *http_date )
{
    int i;
    struct tm TM;


    http_date[0] = 0;

    for(i = 0; i < 14; i++)
        if(!isdigit(my_date[i]))
            {
                /*   WebDbLog("WADateToHttpDate_Conv : invalid Date"); */
                return 0;
            } 

    sscanf(my_date, "%4d %2d %2d %2d %2d %2d", 
           &TM.tm_year,
           &TM.tm_mon,
           &TM.tm_mday,
           &TM.tm_hour,
           &TM.tm_min,
           &TM.tm_sec);

    TM.tm_year -= 1900;
    TM.tm_mon--;

    if( (TM.tm_year < 70 || TM.tm_year > 135) ||
        (TM.tm_mon < 0 || TM.tm_mon > 11) ||
        (TM.tm_mday < 1 || TM.tm_mday > 31) ||
        (TM.tm_hour < 0 || TM.tm_hour > 23) ||
        (TM.tm_min < 0 || TM.tm_min > 59) ||
        (TM.tm_sec < 0 || TM.tm_sec > 61) )  /* we allow two leap seconds */
        {
            /* WebDbLog("WADateToHttpDate_Conv :  invalid Date"); */
            return 0;
        }

    TM.tm_isdst = -1;      /* let the system determine wether we have daylight
                              saving time ... */
 

    if( mktime(&TM) < 0 ||
        (TM.tm_mon < 0 || TM.tm_mon > 11) ||
        (TM.tm_mday < 1 || TM.tm_mday > 31) ||
        (TM.tm_hour < 0 || TM.tm_hour > 23) ||
        (TM.tm_min < 0 || TM.tm_min > 59) ||
        (TM.tm_sec < 0 || TM.tm_sec > 61) )  /* we allow two leap seconds */
        {
            /* WebDbLog("WADateToHttpDate_Conv :  invalid Date"); */
            return 0;
        }
    else
        {
            sprintf(http_date, "%s, %02d %s %4d %02d:%02d:%02d GMT",
                    DayOfWeek[TM.tm_wday],
                    TM.tm_mday,
                    Month[TM.tm_mon],
                    1900 + TM.tm_year,
                    TM.tm_hour,
                    TM.tm_min,
                    TM.tm_sec);
        }
    return(strlen(http_date));
} /* wd29WADateToHttpDate */


SAPDB_UInt4 wd29HttpDate( char *Buf )
{
    time_t      tp;
    struct tm   TM, *TMP;

    /* begexcl(HttpDateExcl,0,NULL); */
    time(&tp);
    TMP = gmtime(&tp);
    TM = *TMP;
    /* endexcl(HttpDateExcl,0,NULL); */

    Buf[0] = 0;
    sprintf(Buf, "%s, %02d %s %4d %02d:%02d:%02d GMT",
            DayOfWeek[TM.tm_wday],
            TM.tm_mday,
            Month[TM.tm_mon],
            1900 + TM.tm_year,
            TM.tm_hour,
            TM.tm_min,
            TM.tm_sec);
    return(strlen(Buf));
} /* wd29HttpDate */

/*!**********************************************************************

  EndChapter: Date conversion functions

************************************************************************/
