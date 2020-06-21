/*!
  @file           ven05.c
  @author         JoergM
  @brief          Client RunTime: Virtual Printer (using standard spooler "lp")
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

#define MOD__ "ven05.c:"

#include "gen00.h"
#include "geo007_1.h"
#include "heo46.h"
#include "hen50.h"
#include "hen40.h"

#if SVR4 || SOLARIS 
#include	<wait.h>
#endif
#if SCO
#include	<sys/wait.h>
#endif

#ifdef  DEBUG_RTE
#define ERRTEXT(v,t)    { DBG1 (( MF__,"ERR %s\n", t));\
			  eo46CtoP(v,t,MAXERRORTEXT); }
#else
#define ERRTEXT(v,t)    { eo46CtoP(v,t,MAXERRORTEXT); }
#endif  


#define LP                             "lp"

#define DEST                           "-d"
#define STD_IN                         0
#define STD_OUT                        1
#define STD_ERR                        2
#define MAX_PID                        8
#define MAX_DR_ANZ                     3
#define SPL_NAME                       "/tmp/spl"

static struct
    {
    char                               dest[2];
    PRINTERNAMEC                       printer;
    } destination = { 0 };

static  pid_t                   sql05_apc_pid = (pid_t)0;
static  tsp00_Bool              sql05_vp_initial_printer = (tsp00_Bool)TRUE ;
static  tsp00_VFilenamec        sql05_spl_filename ;
static  tsp00_VFilenamec        sql05_pr_file ;
static  tsp00_VFilenamec        sql05_lp ;
static  tsp00_Int4              sql05_pr_number = 0;
static  char                    sql05_pr_lf = 0;
static  char                    sql05_pr_ff = 0;
static  char                    sql05_pr_cr = 0;
static  int                     sql05_spool_fd ;
static  char                    * sql05_pr_argv [ 3 ] = { NULL, NULL, NULL };
static  tsp00_Bool                 has_printed = 0;
static  int                     fildes [ 2 ] = { 0, 0 };

/*  IST unused * /
/ * IST * / static	char			blankprinter [] =
/ * IST * /     "                                                                ";
/ * IST */

#ifdef  REL30
void                        sqlpon ( tsp00_PrtName  printer,
tsp00_Bool                                *vpok);
void                        sqlprint ( tsp00_PrtLine line,
tsp00_Int2                               length,
tsp00_VpLinefeeds                       lfeeds,
tsp00_ErrText                              errtext,
tsp00_Bool                                *vpok);
void                        sqlpoff ( tsp00_Bool print,
tsp00_ErrText                            errtext,
tsp00_Bool                                *printok);
#else
void                        vpon ( tsp00_PrtName  printer,
tsp00_Bool                                *vpok);
void   						vprint ( tsp00_PrtLine line,
tsp00_Int2                               length,
tsp00_VpLinefeeds                       lfeeds,
tsp00_ErrText                            errtext,
tsp00_Bool                                *vpok);
void    				    vpoff ( tsp00_Bool print);
#endif

static	void			newdest ();
static	void			init_printer ();
static	void			spoolname ();
static	void			dup_in ();
static	void			dup_stdout ();
static	void			dup_stderr ();

/*==========================================================================*/

static			void newdest ()
{
int             len ;

DBG1 (( MF__,"newdest: called \n" ));

/*  FST * / if ( strncmp ( destination.printer, blankprinter, sizeof (tsp00_PrtName) ) )
/ * FST **/
len = strlen ( destination.printer ) ;
DBG1 (( MF__,"newdest         : len                            = %d\n", len ))
if ( len )
  {
  /* destination is not blank, use it      */
  (void) strncpy (destination.dest, DEST, 2);
  sql05_pr_argv [ 1 ] = (char *) & destination ;
  sql05_pr_argv [ 2 ] = (char *) NULL ;
  }
else
  {
  /* destination is blank, use default */
  sql05_pr_argv [ 1 ] = (char *) NULL ;
  }

DBG1 (( MF__,"newdest: returning \n" ));
}


/*------------------------------*/


static void init_printer ()
{

DBG1 (( MF__,"init_prin: called \n" ));

sql05_apc_pid   = getpid ();
sql05_pr_number = 1;
sql05_pr_lf     = '\n';
sql05_pr_ff     = '\f';
sql05_pr_cr     = '\r';
(void) strcpy ( sql05_lp , LP);
(void) strcpy ( sql05_pr_file , LP );
sql05_pr_argv [ 0 ] = sql05_lp ;
sql05_pr_argv [ 1 ] = (char *) NULL ;
sql05_pr_argv [ 2 ] = (char *) NULL ;

DBG1 (( MF__,"init_prin: returning \n" ));
}


/*------------------------------*/


/*
 * spoolname
 *
 * ermittelt aus Prozess_id und Anzahl der Druckjobs den Namen fuer die
 * Druckausgabedatei.
 */

static void spoolname (
pid_t			pid ,
tsp00_Int4		dr_anz )
{
char                                   pid_str[MAX_PID+1];
char                                   dr_str[MAX_DR_ANZ+1];
char                                   *cp;

DBG1 (( MF__,"spoolname: called \n" ));

for (*((cp = pid_str + MAX_PID)) = '\0', cp--; cp >= pid_str; cp--)
    {
    *cp     = pid % 10 + '0';
    pid    /= 10;
    }

for (*((cp = dr_str + MAX_DR_ANZ)) = '\0', cp--; cp >= dr_str; cp--)
    {
    *cp     = dr_anz % 10 + '0';
    dr_anz /= 10;
    }

(void) strcpy ( sql05_spl_filename , SPL_NAME);
(void) strcat ( sql05_spl_filename , pid_str);
(void) strcat ( sql05_spl_filename , dr_str);

DBG1 (( MF__,"spoolname: returning \n" ));
}


/*------------------------------*/


static void dup_in (
tsp00_Bool                                *notok)
{

DBG1 (( MF__,"dup_in: called \n" ));

*notok = TRUE;
if ( lseek ( sql05_spool_fd , 0 , 0 ) == ( long ) UNDEF )
    {
    MSGD (( -11301 , ERR_TYPE, "VPRINTER", "lseek error, %s\n", sqlerrs() ));
    }
else
    {
    if (close (STD_IN) == UNDEF)
	{
	MSGD (( -11302 , ERR_TYPE, "VPRINTER", "close stdin error, %s\n", sqlerrs() ));
	}
    else
	{
	if (dup ( sql05_spool_fd ) != STD_IN)
	    {
	    MSGD (( -11300 , ERR_TYPE, "VPRINTER", "error, %s\n", sqlerrs() ));
	    }
        else
	    {
	    if (close ( sql05_spool_fd ) == UNDEF)
		{
		MSGD (( -11303 , ERR_TYPE, "VPRINTER", "close error, %s\n", sqlerrs() ));
		}
	    else
		{
		*notok = FALSE;
		}
	    }
	}
    }

DBG1 (( MF__,"dup_in: returning \n" ));
}


/*==========================================================================*/


static void dup_stdout ( 
tsp00_Bool                         *ok)
{
    int                         fd_devnull;
    tsp00_VFilenamec               dev_null;

    DBG1 (( MF__,"********** dup_stdout called **************************\n" ))

    *ok = FALSE;
    (void) strcpy (dev_null, "/dev/null");
    fd_devnull = open ( dev_null , O_WRONLY , 0 );

    DBG1 (( MF__,"dup_stdout      : fd_devnull                     = %d\n",
			     fd_devnull ))

    if (fd_devnull == UNDEF)
    {
	MSGD (( -11115 , ERR_TYPE, "VPRINTER", "open /dev/null error, %s\n", sqlerrs() ));
    }
    else
    {
	if ( close ( STD_OUT ) == UNDEF )
	{
	    MSGD (( -11302 , ERR_TYPE, "VPRINTER", "close stdout error, %s\n", sqlerrs() ));
	}
	else
	{
	    if (dup (fd_devnull) == UNDEF)
	    {
		MSGD (( -11304 , ERR_TYPE, "VPRINTER", "dup stdout error, %s\n", sqlerrs() ));
	    }
	    else
	    {
		if (close (fd_devnull) == UNDEF)
		{
		    MSGD (( -11302 , ERR_TYPE, "VPRINTER", "close /dev/null error, %s\n", sqlerrs() ));
		}
		else
		{
		    *ok = TRUE;
		}
	    }
	}
    }

    DBG1 (( MF__,"dup_stdout : returning \n" ));
}

/*==========================================================================*/


static void dup_stderr (
tsp00_Bool                         *ok)
{
/* IST unused* /
/ *IST* /    int                         fd_devnull;
/ *IST* /    tsp00_VFilenamec               dev_null;
/ *IST*/

    DBG1 (( MF__,"********** dup_stderr called **************************\n" ))

    *ok = FALSE;

    if ( close ( STD_ERR ) == UNDEF )
    {
	MSGD (( -11302 , ERR_TYPE, "VPRINTER", "close stderr error, %s\n", sqlerrs() ));
    }
    else
    {
	if (dup ( fildes [ 1 ] ) == UNDEF)
	{
	    MSGD (( -11305 , ERR_TYPE, "VPRINTER", "dup fildes [1] error, %s\n", sqlerrs() ));
	}
	else
	{
	    if (close ( fildes [ 1 ] ) == UNDEF)
	    {
		MSGD (( -11306 , ERR_TYPE, "VPRINTER", "close fildes [1] error, %s\n", sqlerrs() ));
	    }
	    else
	    {
		*ok = TRUE;
	    }
	}
    }

    DBG1 (( MF__,"dup_stderr : returning \n" ));
}

/*------------------------------*/

#ifdef  REL30
void                                   sqlpon (
#else
void                                   vpon (
#endif

tsp00_PrtName                            printer,
tsp00_Bool                                *vpok)
{
    eo46PtoC (destination.printer, printer, sizeof(tsp00_PrtName));

    DBG1 (( MF__,"sqlpon: called,    to print on %s\n", destination.printer ));

    has_printed = FALSE ;
    * vpok = FALSE ;

    if (sql05_vp_initial_printer)
    {
	init_printer ();
	sql05_vp_initial_printer = FALSE;
    }

    spoolname ( sql05_apc_pid , sql05_pr_number );
    DBG1 (( MF__,"sqlpon     : printer        >%s<\n", destination.printer));

    newdest ();

    sql05_spool_fd = open ( sql05_spl_filename ,
					   O_RDWR | O_CREAT | O_TRUNC , 0666);
    if ( sql05_spool_fd == UNDEF)
    {
	MSGD (( -11307, ERR_TYPE, "VPRINTER", "open spoolfile error, %s\n", sqlerrs()));
    }
    else
    {
	if (unlink ( sql05_spl_filename ) == UNDEF)
	{
	    *vpok = FALSE;
	    MSGD (( -11308, ERR_TYPE, "VPRINTER", "open unlink error, %s\n", sqlerrs()));
	}
	else
	{
	    ++ sql05_pr_number ;
	    * vpok = TRUE ;
	}
    }

    DBG1 (( MF__,"sqlpon: returning \n" ));
}


/*------------------------------*/


#ifdef  REL30
void	sqlprint (
tsp00_PrtLine                            line,
tsp00_Int2                               length,
tsp00_VpLinefeeds                       lfeeds,
tsp00_ErrText                              errtext,
tsp00_Bool                                *vpok)
#else
void	vprint (
tsp00_PrtLine                            line,
tsp00_Int2                               length,
tsp00_VpLinefeeds                       lfeeds,
tsp00_ErrText                            errtext,
tsp00_Bool                                *vpok)
#endif
{
char                                   prstring[ MX_PRLINE + 4 ];
tsp00_Int2                               len;
tsp00_Int4                               retvalue;
 
DBG1 (( MF__,"sqlprint: called \n" ));


if ( lfeeds < VP_PAGE)
    {
    SAPDB_memset ( prstring , sql05_pr_lf , lfeeds );
    len =  lfeeds;
    }
else
    {
    * prstring = sql05_pr_ff ;
    len       = 1;
    }

SAPDB_memcpy (( char * ) ( prstring + len ) , line , length );
len           +=  length;
prstring[len]  = sql05_pr_cr ;

retvalue = write ( sql05_spool_fd , prstring , ( unsigned ) ++ len );
if (retvalue < len)
    {
    * vpok = FALSE;
    ERRTEXT (errtext, "write error");
    DBG1 (( MF__,"sqlprint   : write error, %s\n", sqlerrs() ));
    }
else
    {
    has_printed = TRUE ;
    * vpok = TRUE;
    }

DBG1 (( MF__,"sqlprint: returning \n" ));
}


/*------------------------------*/


#ifdef  REL30
void	sqlpoff (
tsp00_Bool                                print,
tsp00_ErrText                            errtext,
tsp00_Bool                                *printok)
#else
void	vpoff (
tsp00_Bool                                print)
#endif
{
    pid_t                               son_pid;
    pid_t                               wait_pid;
    int                                 status;
    tsp00_Int4                            retvalue;
    tsp00_Bool                             ok;
    tsp00_Bool                             notok;
    int                                 cnt ;
    char                                son_err_buf [ 121 ] ;
    SIGNALFUNCTYPE                      (* sig_old)();
#ifdef  REL24
    tsp00_Bool                             printok_buf, *printok = &printok_buf;
#endif

    DBG1 (( MF__,"sqlpoff: called \n" ));

    * printok = TRUE ;

    if ( print && has_printed )
    {

	/* ***** set signal to SIG_DFL ***** */

	sig_old = signal (SIGCLD, SIG_DFL);


	/* ***** create unnamed pipe for error handling ***** */

	if ( pipe ( fildes ) < 0 )
	{
	    * printok = FALSE ;
	    MSGD (( -11309, ERR_TYPE, "VPRINTER", "sqlpoff: creating pipe error, %s\n", sqlerrs() ))
	    DBG1 (( MF__,"sqlpoff         : * printok (pipe)               = %d\n",
				     * printok ))
	    ERRTEXT (errtext, "cannot create pipe to lp");
	    return ;
	}


	/* ***** fork your SON ***** */

	son_pid = fork ();

	if (son_pid == 0)
	{
	    /*****              S O N                 *****/

	    /* ***** close fd for reading ***** */
	    /* ***** from pipe            ***** */

	    (void ) close ( fildes [ 0 ] ) ;


	    /* ***** dup printer file to STDIN ***** */

	    dup_in (&notok);
	    if (notok)
	    {
		* printok = FALSE ;
		MSGD (( -11310 , ERR_TYPE, "VPRINTER", "dup_in error, %s\n", sqlerrs() ));
		ERRTEXT (errtext, "cannot redirect stdin to lp");
	    }
	    else
	    {

		/* ***** dup STDOUT to /dev/null ***** */

		dup_stdout (&ok);
		if (! ok)
		{
		    * printok = FALSE ;
		    MSGD (( -11310 , ERR_TYPE, "VPRINTER", "dup_stdout error, %s\n", sqlerrs() ));
		    ERRTEXT (errtext, "cannot redirect stdout from lp");
		}
		else
		{
		/* ***** dup STDERR to pipe ( to father ) ***** */

		    dup_stderr (&ok);
		    if (! ok)
		    {
			* printok = FALSE ;
			MSGD (( -11310 , ERR_TYPE, "VPRINTER", "dup_stderr error, %s\n", sqlerrs() ));
			ERRTEXT (errtext, "cannot redirect stderr from lp");
		    }
		    else
		    {
			DBG1 (( MF__,"sqlpoff         : sql05_pr_argv [ 0 ]            = >%s<\n",
						 sql05_pr_argv [ 0 ] ))
			DBG1 (( MF__,"sqlpoff         : sql05_pr_argv [ 1 ]            = >%s<\n",
						 sql05_pr_argv [ 1 ] ))
			DBG1 (( MF__,"sqlpoff         : sql05_pr_argv [ 1 ]            = %d\n",
						 sql05_pr_argv [ 1 ] ))
			DBG1 (( MF__,"sqlpoff         : sql05_pr_argv [ 2 ]            = %d\n",
						 sql05_pr_argv [ 2 ] ))

			retvalue = execvp ( sql05_pr_file , sql05_pr_argv );

			if (retvalue < 0)
			{
			    (void) fprintf ( stderr ,
				"cannot execute '%s': ", sql05_pr_file );
			    perror ( "" );
			    exit ( -1 );
			}
		    }
		}
	    }
	}
	else
	{
	    /*****              F A T H E R           *****/
	    if (son_pid < 0)
	    {
		* printok = FALSE ;
		MSGD (( -11311 , ERR_TYPE, "VPRINTER", "fork error, %s\n", sqlerrs() ));
		ERRTEXT (errtext, "cannot fork for lp");
	    }
	    else
	    {
		DBG1 (( MF__,"sqlpoff    : Waiting \n" ));
		wait_pid = wait (&status);
		if (wait_pid == UNDEF)
		{
		    * printok = FALSE ;
		    MSGD (( -11312 , ERR_TYPE, "VPRINTER", "wait error, %s\n", sqlerrs() ));
		    ERRTEXT (errtext, "cannot get status from lp");
		}
		else
		{
		    if (wait_pid != son_pid)
		    {
			* printok = FALSE ;
			MSGD (( -11313 , ERR_TYPE, "VPRINTER", "son_pid error, %s\n", sqlerrs() ));
			ERRTEXT (errtext, "got status from wrong process");
		    }
		    else if ( status )
		    {
			* printok = FALSE ;
			MSGD (( -11313 , ERR_TYPE, "VPRINTER", "lp returned 0x%04x \n", status ));
			ERRTEXT (errtext, "lp returned error status");
		    }
		}
	    }

	    /* ***** install old signal handling ***** */

	    (void) signal (SIGCLD, sig_old);


	    /* ***** reading from pipe ***** */

	    SAPDB_memset ( son_err_buf , '\0' , 120 ) ;
	    ( void ) close ( fildes [ 1 ] ) ;
	    cnt = read ( fildes [ 0 ] , son_err_buf , 120 ) ;
	    if ( cnt < 0 )
	    {
		* printok = FALSE ;
		MSGD (( -11314, ERR_TYPE, "VPRINTER", "sqlpoff: read from pipe error, %s\n",
							sqlerrs() ))
		ERRTEXT (errtext, "cannot read error message from lp");
	    }
	    (void) close ( fildes[0] ); /* IST 26.05.94 */
	    (void) close ( sql05_spool_fd ); /* IST 13.06.94 */
	    sql05_spool_fd = -1 ; /* IST 13.06.94 */

	    if ( cnt > 0 )
	    {
		* printok = FALSE ;
		MSGD (( -11315, ERR_TYPE, "VPRINTER", "%s" , son_err_buf ))
		(void) sleep ( 4 ); /* lets the MSG stand on screen */
#               ifdef REL24
		sqle04c_cup ( 2 ) ;
		sqle04c_refresh () ;
		write ( STD_ERR , son_err_buf , (unsigned) cnt ) ;
#               endif
		ERRTEXT (errtext, son_err_buf);
	    }
	}
    }
    else
    {
	DBG1 (( MF__,"sqlpoff    : Clear up           \n" ));
    }

    DBG1 (( MF__,"sqlpoff         : * printok ( returning )        = %d\n",
			     * printok ))
}
