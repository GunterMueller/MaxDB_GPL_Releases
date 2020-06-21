/*!
  @file           ven02.c
  @author         JoergM
  @brief          Application Runtime date_time
  @see            


  Defines sql02_init_time, sqlclock, sqldattime

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


#include <sys/time.h>

/* exported functions */

#include "gen00.h"

void			sql02_init_time ();
void			sqlclock (tsp00_Int4 *sec, tsp00_Int4 *usec);
void			sqldattime (tsp00_Date d, tsp00_Time t);

/* local functions */

static void     sql02_sec_usec ( time_t *sec , long *usec );

/* COMMON data */

void            ( * sql01_init_time ) () = sql02_init_time ;

/* local data */

static  time_t  sql02_start_time ;

/*
 * ===========================================================================
 */

void	sql02_init_time ( )
{
    time_t			sec ;
    long			usec ;
    /*
     *  Get start_time.
     */
    sql02_sec_usec ( &sec , &usec );
    sql02_start_time = sec ;
}

/*
 * ===========================================================================
 */

static void sql02_sec_usec ( time_t *sec , long *usec )
{
    struct timeval		tv ;
    struct timezone		tz ; /* obsolete on newer SVR4 systems */

# if _IBMR2
#  if _LONG_LONG
/*  AIX 4.1 and newer - use routines 'read_real_time' and 'time_base_to_time' */
    timebasestruct_t            time_now ;
    (void) read_real_time    ( & time_now , TIMEBASE_SZ );
           /* This yields processor-specific units - not usable directly */

    (void) time_base_to_time ( & time_now , TIMEBASE_SZ );
           /* Now 'time_now' contains second / nanosecond */

    tv.tv_sec  =   time_now . tb_high ;
    tv.tv_usec = ( time_now . tb_low ) / 1000 ; /* from nano to micro */

#  else
/*  AIX 3.2 - use 'mfspr' assembler instructions */
    /* Nanoseconds are only valid, if no jump of the second's value occured. */
    do
    {
	tv.tv_sec  = pfaix_sec  (); /* get the current second's value */
	tv.tv_usec = pfaix_nsec (); /* get the current nanosecond's value */
    }
    while ( tv.tv_sec != pfaix_sec() ); /* check the second's value */
    tv.tv_usec /= 1000 ; /* transform nanoseconds to microseconds */
#  endif /* AIX 4.1 ff. vs. AIX 3.2 */
# else
    (void) gettimeofday ( &tv , &tz );
# endif
    *sec  = tv.tv_sec ;
    *usec = tv.tv_usec ;
}

/*
 * ===========================================================================
 */

#if REL30
void    sqlclock ( tsp00_Int4 *sec , tsp00_Int4 *usec )
{
    time_t unix_sec;
    long   unix_usec;
    sql02_sec_usec ( &unix_sec , &unix_usec );

    /*
     *  Return the time since the start of the database.
     */
    *sec = unix_sec - sql02_start_time ;
    *usec = unix_usec;

    DBG1 (( MF__,"sqlclock: sec %ld usec %ld \n", (long) *sec , (long) *usec ))
}
#endif	/* REL30 */

/*
 * ===========================================================================
 */

void    sqldattime ( tsp00_Date PasDate , tsp00_Time PasTime )
{

    char			*dat = (char *)&PasDate[0] ;
    char			*tim = (char *)&PasTime[0] ;
    time_t          time_sec;
    time_t   	    sec ;
    long         	usec ;
#ifdef _REENTRANT
    struct tm	    tm_buffer ;
#endif
    struct tm	   *tmbuf ;
    char		   *ptr ;


    sql02_sec_usec ( &sec , &usec );
    /*
     *  The adjustment to local time is done by 'localtime'.
     */
    time_sec = sec;
#ifdef _REENTRANT
    tmbuf = localtime_r ( &time_sec , &tm_buffer );
#else
    tmbuf = localtime ( &time_sec );
#endif

    /*
     * ASCII  '0'
     */
    (void) SAPDB_memset ( dat , '0' , sizeof(tsp00_Date) );
    (void) SAPDB_memset ( tim , '0' , sizeof(tsp00_Time) );

    /*
     *  WARNING: Using "usec" as a temporary variable.
     *           Its value is no longer needed in this function.
     */
    ptr = dat + sizeof(tsp00_Date) ;
		 usec = tmbuf->tm_mday ;
    *(-- ptr) += usec %  10 ;
		 usec /= 10 ;
    *(-- ptr) += usec %  10 ;
		 usec = tmbuf->tm_mon + 1 ;
    *(-- ptr) += usec %  10 ;
		 usec /= 10 ;
    *(-- ptr) += usec %  10 ;
		 usec = tmbuf->tm_year + 1900 ;
    *(-- ptr) += usec %  10 ;
		 usec /= 10 ;
    *(-- ptr) += usec %  10 ;
		 usec /= 10 ;
    *(-- ptr) += usec %  10 ;
		 usec /= 10 ;
    *(-- ptr) += usec %  10 ;

    ptr = tim + sizeof(tsp00_Time) ;
		 usec = tmbuf->tm_sec ;
    *(-- ptr) += usec %  10 ;
		 usec /= 10 ;
    *(-- ptr) += usec %  10 ;
		 usec = tmbuf->tm_min ;
    *(-- ptr) += usec %  10 ;
		 usec /= 10 ;
    *(-- ptr) += usec %  10 ;
		 usec = tmbuf->tm_hour ;
    *(-- ptr) += usec %  10 ;
		 usec /= 10 ;
    *(-- ptr) += usec %  10 ;

    DBG1 (( MF__,"sqldattime: %.8s %.8s \n", dat , tim ))
}
