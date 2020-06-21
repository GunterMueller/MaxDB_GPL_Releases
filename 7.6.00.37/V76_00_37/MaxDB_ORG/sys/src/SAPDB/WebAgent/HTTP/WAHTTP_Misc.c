/*



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





*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>
#include <time.h>

#ifndef WIN32
#include <errno.h>
#endif

#include "gwd00.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_Misc.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_Log.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_Memory.h"

#include "heo07.h"

#define LOG_FORMAT	"%s: %s\n"
#define TRC_FORMAT	"%s: "
#define MALLOC_ERROR	"Malloc error"

/* Options	*/
#define OPTERRCOLON (1)
#define OPTERRNF (2)
#define OPTERRARG (3)

int optind = 1;
int opterr = 1;
int optopt;

/* Semaphoring */

struct st_excl
{
 teo07_Mutex hMutex;
};

int initexcl
(
 Excl		*excl,
 unsigned short	ErrorTextSize,
 char		ErrorText[]
)
/* Initialize exclusive access for threads to a resource.
 * Count: Number of threads that may access the resource before blocking.
 *	  If 0 (zero), timeout is INFINITE.
 * Timeout: Millisecs the thread will be blocked when blocking applies.
 */
{
 tsp00_Bool		bOk;

 wahttp_SQLALLOCAT( sizeof(struct st_excl),
		  &(*excl),
		  &bOk);
 if (!bOk)
 {
 	strcpymax(ErrorTextSize, ErrorText, MALLOC_ERROR);
	return(FALSE);
 }
 memset(*excl, 0, sizeof(struct st_excl));

 sqlcreatemutex(&(*excl)->hMutex);

 if (!(*excl)->hMutex)
 {
	strcpymax(ErrorTextSize, ErrorText, "error sqlcreatemutex");
	wahttp_SQLFREE((*excl));
	return(FALSE);
 }
 return(TRUE);
}

void begexcl
(
 Excl		excl
)
 /* Try decrement the semaphore (block if zero, otherwise decrement */
{
 sqlbeginmutex(&excl->hMutex);
}

void endexcl
(
 Excl		excl
)
{
 sqlendmutex(&excl->hMutex);
 sqlyieldthread();	/* cause Bill cancels the lock, but does not always transfer control */
}

void exitexcl
(
 Excl		excl
)
{
 sqldestroymutex(&excl->hMutex);
 wahttp_SQLFREE(excl);
}

struct st_sem
{
 teo07_ThreadSemaphore	hSem;
};

int initsem
(
 Sem		*sem,
 unsigned short	ErrorTextSize,
 char		ErrorText[]
)
{
 tsp00_ErrTextc		errtext;
 teo07_ThreadErr	err;
 tsp00_Bool		bOk;

 wahttp_SQLALLOCAT( sizeof(struct st_sem),
		  &(*sem),
		  &bOk);
 if (!bOk)
 {
 	strcpymax(ErrorTextSize, ErrorText, MALLOC_ERROR);
	return(FALSE);
 }
 memset(*sem, 0, sizeof(struct st_sem));

 sqlcreatesem(&(*sem)->hSem, 0, errtext, &err);

 if (err != THR_OK_EO07)
 {
	strcpymax(ErrorTextSize, ErrorText, "error sqlcreatesem");
	wahttp_SQLFREE((*sem));
	return(FALSE);
 }
 return(TRUE);
}

void begsem
(
 Sem		sem
)
{
 sqlwaitsem(sem->hSem);
}

void endsem
(
 Sem		sem
)
{
 sqlsignalsem(sem->hSem);
}

void exitsem
(
 Sem		sem
)
{
 sqldestroysem(sem->hSem);
 wahttp_SQLFREE(sem);
}

char *strcpymax
(
 size_t		m,
 char		*dst,
 char		*src
)
/* Copy to dst with a maximum size of m.
 * dst will always NULL terminated or NULL.
 * Return dst.
 */ 
{
 size_t	l = m > 0 ? WAHTTP_MIN(m-1, strlen(src)) : 0;

 if (l > 0)
 {
	strncpy(dst, src, l);
	dst[l] = '\0';
 }
 return(dst);
}

char *strcatmax
(
 size_t		m,
 char		*dst,
 char		*src
)
/* Copy to dst with a maximum size of m.
 * dst will always NULL terminated or NULL.
 * Return dst.
 */ 
{
 size_t	ldst = dst ? strlen(dst) : 0;
 size_t	lsrc = src ? strlen(src) : 0;
 size_t	l = m > 0 ? m-1 - ldst : 0;

 if (l > 0)
 {
	strncpy(dst+ldst, src, l);
	dst[ldst+l] = '\0';
 }
 return(dst);
}

char *strnstrcase
(
 char	*string,
 size_t	stringlg,
 char	*search
)
{
 size_t	searchlg = strlen(search);

 for (; stringlg > 0; string++, stringlg-- )
 {
	if (stringlg < searchlg)
		return(NULL);
	else
		if (!strncasecmp(string, search, searchlg))
			return(string);
 }
 return(NULL);
}

char *trim
(
 char		Str[]
)
/* Remove leading and trailing (ascii) white spaces.
 */
{
 size_t	j = 0, i = 0;

 if (!Str) return(Str);


 /* remove leading */
 for (i=0; Str[i]; i++)
	if (!isspace(Str[i]))
		break;

 j = 0;
 while (i > 0 && Str[i])
 {
	Str[j] = Str[i];
	i = i + 1;
	j = j + 1;
 }
 Str[j] = Str[i];

 /* remove trailing */
 for (i=strlen(Str); i>0 && isspace(Str[i-1]); i--);
 Str[i] = '\0';

 return(Str);
}

#define DYNSTR_MALLOC_CHUNCK 1024

DynStr DSinit
(
 void
)
{
 DynStr			ds;
 tsp00_Bool		bOk;

 wahttp_SQLALLOCAT( sizeof(struct st_dynstring),
		  &ds,
		  &bOk);
 if (!bOk)
	ds = NULL;
 else
	memset(ds, 0, sizeof(struct st_dynstring));
 return(ds);
}

DynStr DSextent
(
 DynStr	*ds,
 size_t	sz
)
{
 char			*tmp = NULL;
 tsp00_Bool		bOk;

 do
	if ((*ds)->lg + sz > (*ds)->sz)
	{
		tmp = (*ds)->str;
		wahttp_SQLREALLOCAT( (*ds)->sz + DYNSTR_MALLOC_CHUNCK,
					  &tmp,
					  &bOk);
		if (!bOk)
		{
			Log("Internal DYNSTR_MALLOC_CHUNCK too small !!!", DYNSTR_MALLOC_CHUNCK);
			DSdelete(*ds);
		}
		(*ds)->str = tmp;
		(*ds)->sz += DYNSTR_MALLOC_CHUNCK;
	}
 while ((*ds)->lg + sz > (*ds)->sz);

 return(*ds);
}

void DSdelete
(
 DynStr	ds
)
{
 if (ds->sz > 0) wahttp_SQLFREE(ds->str);
 wahttp_SQLFREE(ds);
}

void Basename
(
 char	Basename[],
 char	Path[],
 char	PathSeparator
)
/* Copy the string after the last 'PathSeparator' in 'Path' into 'Basename'.
 * If there is no 'PathSeparator' then copy 'Path'.
*/
{
 char	*tmp = strrchr(Path, PathSeparator);

 if (tmp)
	strcpy(Basename, ++tmp);
 else
	strcpy(Basename, Path);
}

struct st_data *CreateData
(
 void
)
{
 struct st_data		*rtc;
 tsp00_Bool		bOk;

 wahttp_SQLALLOCAT( sizeof(struct st_data),
		  &rtc,
		  &bOk);
 if (!bOk)
	memset(rtc, 0, sizeof(struct st_data));
 else
	rtc = NULL;

 return(rtc);
}

void DeleteData
(
 struct st_data *data
)
{
 wahttp_SQLFREE(data);
}

/****************************************************************************
 ***
 *** OptIsErr
 ***
 ****************************************************************************/

static int OptIsErr( int           argc, 
					 char * const *argv,
					 int           oint,
					 const char   *optstr,
                     int           optchr,
					 int           err    )
{

    if (opterr) {
        fprintf(stderr, "Error in argument %d, char %d: ", oint, optchr + 1);

        switch (err) {
        case OPTERRCOLON:
            fprintf(stderr, ": in flags\n");
            break;
        case OPTERRNF:
            fprintf(stderr, "option not found %c\n", argv[oint][optchr]);
            break;
        case OPTERRARG:
            fprintf(stderr, "no argument for option %c\n", argv[oint][optchr]);
            break;
        default:
            fprintf(stderr, "unknown\n");
            break;
        }
    }

    optopt = argv[oint][optchr];
    return('?');

}

/****************************************************************************
 ***
 *** GetOpt
 ***
 ****************************************************************************/
    
int GetOpt( int           argc,
		    char* const  *argv,
			const char   *optstr,
			char        **optarg )
{

    static int optchr = 0;
    static int dash = 0; /* have already seen the - */

    char *cp;

    if (optind >= argc)
        return(EOF);

    if (!dash && (argv[optind][0] !=  '-'))
        return(EOF);

    if (!dash && (argv[optind][0] ==  '-') && !argv[optind][1]) {
        /*
         * use to specify stdin. Need to let pgm process this and
         * the following args
         */
        return(EOF);
    }

    if ((argv[optind][0] == '-') && (argv[optind][1] == '-')) {
        /* -- indicates end of args */
        optind++;
        return(EOF);
    }

    if (!dash) {
        assert((argv[optind][0] == '-') && argv[optind][1]);
        dash = 1;
        optchr = 1;
    }

    /* Check if the guy tries to do a -: kind of flag */
    assert(dash);
    if (argv[optind][optchr] == ':') {
        dash = 0;
        optind++;
        return(OptIsErr(argc, argv, optind-1, optstr, optchr, OPTERRCOLON));
    }

    if (!(cp = strchr(optstr, argv[optind][optchr]))) {
        int errind = optind;
        int errchr = optchr;

        if (!argv[optind][optchr+1]) {
            dash = 0;
            optind++;
        }
        else
            optchr++;
        return(OptIsErr(argc, argv, errind, optstr, errchr, OPTERRNF));
    }

    if (cp[1] == ':') {
        dash = 0;
        optind++;

        if(optind == argc)
            return(OptIsErr(argc, argv, optind-1, optstr, optchr, OPTERRARG));
        *optarg = argv[optind++];
        return(*cp);
    } else {
        if (!argv[optind][optchr+1]) {
            dash = 0;
            optind++;
        }
        else
            optchr++;
        return(*cp);
    }
/* Statement will be never reached MaO
    assert(0);
    return(0);
*/
}

#ifdef notyet
static char ap_month_snames[12][4] =
{
    "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
static char ap_day_snames[7][4] =
{
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

size_t our_timestr_822(char *s, time_t sec)
{
    struct tm *tms;

    tms = gmtime(&sec);

    /* RFC date format; as strftime '%a, %d %b %Y %T GMT' */
    return sprintf(s,
		"%s, %.2d %s %d %.2d:%.2d:%.2d GMT", ap_day_snames[tms->tm_wday],
		tms->tm_mday, ap_month_snames[tms->tm_mon], tms->tm_year + 1900,
		tms->tm_hour, tms->tm_min, tms->tm_sec);
}
#endif