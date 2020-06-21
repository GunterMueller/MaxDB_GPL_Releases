/*	vmakserv.c	

    ========== licence begin LGPL
    Copyright (C) 2002 SAP AG

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    ========== licence end

*/

#include "devconfig.h"


#include "vmake.h"

#ifdef	WIN32
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <tchar.h>
#include "Win32/service.h"
#endif	/*WIN32*/
#include <stdio.h>
#include <string.h>

static	char * USAGE = "usage: vmakserv [-Dl] [-i] [-k] \n\
\n\
    -Dl     debug level (l = 0 .. 9) \n\
    -i      interactive, vmakserv runs in foreground \n\
    -k      kill a running vmakserv \n\
";
#define ARGS(x)	(x == 0)

#ifdef	unix

static	char  * RUNDIRECTORY    = "/usr/spool/vmake" ;
static	char  * PIDFILE         = "vmakserv.pid" ;
static	char  * PROTFILE        = "vmakserv.prot" ;

#else	/*unix*/

static	char  * RUNDIRECTORY    = "/spool" ;	/* prepended by %windir% */
static	char  * PROTFILE        = "vmakserv.prt" ;

#endif	/*unix*/

#define         UMASK           0007

#ifdef	SOCKETS	/* else server not needed */

#ifdef WIN32
#include <sys/utime.h>
#include <winsock.h>
#define	EADDRINUSE	  WSAEADDRINUSE
#define EADDRNOTAVAIL WSAEADDRNOTAVAIL
#else
#include <utime.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#endif

# ifdef HAVE_DIRENT_H
#  include	<dirent.h>
# else

    typedef	struct
		{
		    long			dir_handle ;
		    int				dir_entryno ;
		    struct _finddata_t		dir_data ;
		    char			dir_name [1];
		}
					DIR ;
    struct dirent
    {
	    long		d_ino;		/* inode number of entry */
	    off_t		d_off;		/* offset of disk dir entry */
	    unsigned short	d_reclen;	/* length of this record */
	    char		d_name[_MAX_FNAME];	/* name of file */
    };

    extern DIR			    *opendir(char*);
    extern struct dirent	*readdir(DIR*);
    extern int			    closedir(DIR*);
	void MyFileError(const char *, const char *, char *, DWORD);
#   define	sleep		    _sleep

# endif /* unix */

struct conn_info
{
	int             ci_sd;
	FILE            * ci_fileptr[ MAXREMOTEFILES ] ;
	int             ci_opened_f [ MAXREMOTEFILES ] ;
	DIR             * ci_dirptr [ MAXREMOTEFILES ] ;
	int             ci_opened_d [ MAXREMOTEFILES ] ;
	struct message  ci_remote_msg ;
};

typedef	struct conn_info	conn_info ;

/****************** imported data ******************/

/**************** exported functions ****************/

#ifdef	WIN32

global	VOID			ServiceStart (DWORD, LPTSTR*);
global	VOID			ServiceStop  ( void );
#define	main			vmakserv_main

#endif	/*WIN32*/

global	int     main (int, char**);
global	char    * serrno (void);

/******************* exported data ******************/

global	char        * vmake_version = VMAKE_VERSION ;
global  int         dbgdbglvl;
global  int         dbgcurlvl;

/****************** local functions *****************/

#ifdef	unix
static	int     	kill_server (void);
#endif	/*unix*/
static	int     	get_server_address (void);
static  int			open_vmake_socket (void);
static  int			server_loop (void);

#ifdef WIN32
static void			serve       (conn_info*);
#else
static  DWORD		serve       (conn_info*);
#endif

static	int			get_time    (conn_info*);
static	int			get_stat    (conn_info*);
static	int			open_file   (conn_info*);
static	int			close_file  (conn_info*);
static	int			read_file   (conn_info*);
static	int			seek_file   (conn_info*);
static	int			open_dir    (conn_info*);
static	int			close_dir   (conn_info*);
static	int			read_dir    (conn_info*);
static	int			incopy_file (conn_info*, int);
global	void	dbgdbg (char    * fmt ,...);

static	int	        test_is_dir (char*);
static  int	        check_dir (char*);

#ifdef	WRITE_ENABLE
static	int			write_file  (conn_info*);
static	int			outcopy_file(conn_info*, int);
static	int			rename_file   (conn_info*);
static	int			unlink_file (conn_info*);
static	int			make_dir    (conn_info*);
#endif

static	int			get_message (conn_info*);
static	int			put_message (conn_info*);
static  int			set_signals (SIG_RTRN (*)());
static  SIG_RTRN	catch_signal(int);
static	int     	wait_for_termination (int);

#ifdef	unix
static	int     	create_pid_file (void);
static	int     	remove_pid_file (void);
#endif	/*unix*/
static  void		server_exit (int);
static	void		close_all   (void);

/*   PTS 1001420  &gar ->	*/
static void close_local_file (conn_info*,char);
static void close_all_files (conn_info*);
static void close_all_dirs (conn_info*);
/*   PTS 1001420  <- &gar 	*/
static	char    	* addrname  (struct sockaddr*);

#ifdef	MKDIR
static	int mkdir (char*, int);
#endif

#ifdef	WIN32
static  int create_task (DWORD (WINAPI*)(LPVOID), LPVOID, pid_t*);
#endif

/******************* local data  *******************/

#ifdef	unix
static	int         opt_kill ;
static	pid_t       master_pid ;
#else	/*unix*/
static	int			stop_requested ;
static  DWORD       ServerLoopID;
#endif	/*unix*/

static	int             interactive;
static	int             lsd ;
static	int             write_timestamp ;
static	struct sockaddr sa ;

/****************************************************/

#endif	/* SOCKETS */


/*==========================================================================*/
#ifdef	WIN32

global	VOID	ServiceStart
(
    DWORD   argc ,
    LPTSTR  * argv
)
{
	int			rc ;

	DBG2 (( "ServiceStart: called \n" ))

    ServerLoopID = GetCurrentThreadId();
	rc = main ( (int) argc , (char**) argv );
	DBG6 (( "ServiceStart: main returned %d \n", rc ))

	/*
	 *  Prevent this function from terminating before
	 *  ServiceStop terminates.
	 */
	(void) sleep ( 2 );

	// report the status to the service control manager.
	//
	(void) ReportStatusToSCMgr(
	    SERVICE_STOPPED,       // service state
	    NO_ERROR,              // exit code
	    0);                    // wait hint
	DBG3 (( "ServiceStart: returning \n", rc ))
}

/*==========================================================================*/

global	VOID	ServiceStop
(
    void
)
{
	DBG2 (( "ServiceStop: called \n" ))

	// report the status to the service control manager.
	//
	(void) ReportStatusToSCMgr(
	    SERVICE_STOP_PENDING,  // service state
	    NO_ERROR,              // exit code
	    30000);                // wait hint

    server_exit ( 0 );

	DBG3 (( "ServiceStop: returning \n" ))
}

#endif	/*WIN32*/
/*============================================================================*/


global	int	main
(
    int     argc ,
    char	** argv
)
{
	int             rc ;
	int             argn ;
	int             protfd ;
	char			* p ;
#ifdef unix
	int             tfd [ 2 ];
	char			* rundir ;
#else
	char			rundir [ 256 ];
#endif

    DBG2 (( "main: called \n" ))

#ifdef	WIN32
	// report the status to the service control manager.
	//
	if (!ReportStatusToSCMgr(
	    SERVICE_START_PENDING, // service state
	    NO_ERROR,              // exit code
	    30000))                // wait hint
	{
	    return ( 2 );
	}
#endif	/*WIN32*/

#ifndef SOCKETS
	/* use arguments to please lint */
	for ( argn = 1 ; argn < argc ; argn ++ ) rc = (int) argv [ argn ] ;

	(void) printf ( "sockets not supported, exiting \n" );
	rc = 1 ;
	return ( rc );
#else
	/* Server input may be piped,               */
	/* so close stdin to free the process(es)   */
	/* that write to the pipe.                  */
	/* Reopen stdin, so file descriptors 0      */
	/* will not be used for network operations. */
	/* The above is important for both the      */
	/* starter and the background server.       */

	/* redirect stdin to the null device */
	(void) close ( 0 );
#if WIN32
	(void) open ( "nul", O_RDONLY , 0666 );
#else /*WIN32*/
	(void) open ( "/dev/null", O_RDONLY , 0666 );
#endif /*WIN32*/

	/*
	 *  Close all files which might eventually be open.
	 */
	close_all ();

	rc = 0 ;
	for ( argn = 1 ; argn < argc ; argn ++ )
	{
	    p = argv [ argn ];
	    if ( * p != '-' ) break ;
	    for ( p ++ ; * p ; p ++ )
	    {
		switch ( * p )
		{
		case 'D' :
			dbgdbglvl = * ( ++ p ) - '0' ;
			break ;
		case 'i' :
			interactive = 1 ;
			break ;
#ifdef	unix
		case 'k' :
			opt_kill = TRUE ;
			break ;
#endif	/*unix*/
		default :
			DBG0 (( "illegal option '%c' \n", * p ))
			rc ++ ;
		}
	    }
	}
	if ( rc || ! ARGS ( argc - argn ) )
	{
	    (void) write ( 2 , vmake_version + 4 ,
			(unsigned) strlen(vmake_version+4) );
	    (void) write ( 2 , "\n" , 1 );
	    (void) write ( 2 , USAGE , (unsigned) strlen(USAGE) );
	    return ( 1 );
	}

	DBG8 (( "starter: starting \n" ))

	(void) umask ( 0 );   /* set file protection mask */

#ifdef	WIN32
	// report the status to the service control manager.
	//
	if (!ReportStatusToSCMgr(
	    SERVICE_START_PENDING, // service state
	    NO_ERROR,              // exit code
	    30000))                // wait hint
	{
	    return ( 2 );
	}
#endif	/*WIN32*/

#if WIN32
	{
	    WSADATA	x;
	    WSAStartup ( 0x101 , &x );
	}
#endif

	rc = open_vmake_socket ( );
	if ( rc ) server_exit ( rc );

#ifdef	WIN32
	// report the status to the service control manager.
	//
	if (!ReportStatusToSCMgr(
	    SERVICE_START_PENDING, // service state
	    NO_ERROR,              // exit code
	    30000))                // wait hint
	{
	    server_exit ( 2 );
	    return ( 2 );
	}
#endif	/*WIN32*/

	if ( interactive )
	{
	    rc = set_signals ( SIG_DFL );
	    if ( rc ) server_exit ( rc );

#ifdef	unix
	    if ( opt_kill )
	    {
		    rc = kill_server ();
		    server_exit ( rc );
		    /*NOTREACHED*/
		    return ( rc );
	    }
#endif	/*unix*/
	}
	else
	{
	    rc = set_signals ( SIG_IGN );
	    if ( rc ) server_exit ( rc );

#ifdef unix
	    rundir = RUNDIRECTORY ;
#else
	    p = getenv ( "windir" );
	    if ( ! p ) p = "" ;
	    (void) sprintf ( rundir , "%s%s" , p , RUNDIRECTORY );
#endif
	    rc = mkdir ( rundir , 0775 );
	    if ( rc == -1 )
	    {
		    if ( geterrno() != EEXIST )
		    {
		        DBG0 (( "cannot create '%s': %s \n", rundir , serrno() ))
		        server_exit ( 2 );
		    }
	    }
	    else DBG1 (( "created directory '%s' \n", rundir ))

	    rc = chdir ( rundir );
	    if ( rc < 0 )
	    {
		    DBG0 (( "cannot chdir to '%s': %s \n", rundir , serrno() ))
		    server_exit ( 2 );
	    }
	    DBG1 (( "changed directory to '%s' \n", rundir ))

#ifdef	unix
	    if ( opt_kill )
	    {
		    rc = kill_server ();
		    server_exit ( rc );
		    /*NOTREACHED*/
		    return ( rc );
	    }

	    /*
	     *  Create a pipe for messages from the background master
	     *  to the foreground starter process.
	     *  If the master's startup sequence is complete,
	     *  it closes the pipe and the starter exits.
	     */
	    rc = pipe ( tfd );
	    if ( rc < 0 )
        {
		    DBG0 (( "cannot create pipe: %s \n", serrno() ))
		    server_exit ( 2 );
        }

	    /* create background server */
	    rc = fork ();
	    if ( rc < 0 )
	    {
		    DBG0 (( "cannot fork: %s \n", serrno() ))
		    server_exit ( 3 );
	    }
	    if ( rc > 0 )
	    {
		    /* starter exits when pipe becomes closed */
		    (void) close ( tfd[1] ); /* don't write to pipe */
		    tfd[1] = -1 ;
		    rc = wait_for_termination ( tfd[0] );
		    server_exit ( rc );
	    }

	    /* background server */

	    (void) close ( tfd[0] ); /* don't read from pipe */

	    /* Server output may be piped,                       */
	    /* so close stdout/stderr to free the process(es)    */
	    /* that read from the pipe(s).                       */
	    /* Reopen stdout/stderr, so file descriptors 1 and 2 */
	    /* will not be used for network operations.          */

	    /* redirect stdout to the pipe */
	    (void) close ( 1 );
	    (void) dup ( tfd[1] );

	    /* redirect stderr to the pipe */
	    (void) close ( 2 );
	    (void) dup ( tfd[1] );

	    master_pid = getpid ();
	    seterrno ( 0 );
#ifdef SETPGRP_VOID
	    rc = (int) setpgrp ();
#else
	    rc = setpgrp ( 0 , master_pid );
#endif
	    DBG2 (( "vmakserv: set process group %ld \n", (long) master_pid ))
	    if ( rc == -1 )
	    {
		    DBG0 (( "cannot set process group: %s \n", serrno() ))
		    DBG0 (( "           master_pid     %ld \n", (long) master_pid ))
		    server_exit ( 3 );
	    }

	    rc = create_pid_file ();
	    if ( rc ) server_exit ( rc );
#endif /*unix*/

	    DBG0 (( "%s   server started \n", vmake_version + 4 ))

	    protfd = open ( PROTFILE , O_WRONLY | O_APPEND | O_CREAT | O_TRUNC ,
								    0664 );
	    if ( protfd < 0 )
	    {
		    DBG0 (( "cannot create protfile '%s': %s \n", PROTFILE , serrno() ))
		    server_exit ( 2 );
	    }

	    /* redirect stdout to the protocol file */
	    (void) close ( 1 );
	    (void) dup ( protfd );

	    /* redirect stderr to the protocol file */
	    (void) close ( 2 );
	    (void) dup ( protfd );

	    /* save the resources of a file descriptor */
	    (void) close ( protfd );

#if unix
	    /*
	     *  Write a null character to the pipe as success indicator.
	     */
	    (void) write ( tfd[1] , "" , 1 );
	    /* closing the pipe lets the starter exit */
	    (void) close ( tfd[1] );
	    tfd[1] = -1 ;
#endif /*unix*/
	}
	/* from now on, debug output will have a time stamp */
	write_timestamp = 1 ;
	DBG0 (( "%s   server started \n", vmake_version + 4 ))

	(void) umask ( UMASK );   /* set file protection mask */

#ifdef	WIN32
	// report the status to the service control manager.
	//
	if (!ReportStatusToSCMgr(
	    SERVICE_RUNNING,       // service state
	    NO_ERROR,              // exit code
	    0))                    // wait hint
	{
	    server_exit ( 2 );
	    return ( 2 );
	}
#endif	/*WIN32*/

	for ( ; ; )
	{
	    (void) server_loop ();

#ifdef	WIN32
	    if ( stop_requested )
	    {
		    DBG0 (( "main: stop requested \n" ))
            closesocket( lsd );
            WSACleanup();
		    break ;
	    }
#endif	/*WIN32*/

	    do
	    {
		    (void) close ( lsd );
		    (void) sleep ( 70 ); /* hoping, socket disappeared after 60s */
	    }
	    while ( open_vmake_socket () );
	}

#ifdef	unix
	/*NOTREACHED*/
#else	/*unix*/
	DBG0 (( "%s   stopped\n", VMAKE_VERSION + 4 ))
	DBG6 (( "main: returning 0 \n" ))
	return ( 0 );
#endif	/*unix*/
#endif  /* SOCKETS */
}


#ifdef  SOCKETS

/*==========================================================================*/

static	int     get_server_address
(
    void
)
{
	short           port ;
	struct servent  * sp ;
	char            * p ;


	DBG2 (( "getservadr: called \n" ))

	/* get port number for remote sql service */

	seterrno ( 0 );
	sp = getservbyname ( SOCKET_SERVICE , SOCKET_PROTOCOL );
	if ( sp == NULL )
	{
	    DBG8 (( "first identification failed \n" ))
	    /* sometimes the first access fails, so try a second time */

	    seterrno ( 0 );
	    sp = getservbyname ( SOCKET_SERVICE , SOCKET_PROTOCOL );
	    if ( sp == NULL )
	    {
		    dbgdbg ( "get service by name error: %s \n", serrno() );
		    dbgdbg ( "service name '%s' \n", SOCKET_SERVICE );
		    return ( 2 );
	    }
	}

	/* host address is implicit, so only address family and */
	/* port number must be specified                        */

	FILL ( & sa , 0 , sizeof(sa) );
    sa.sa_family = AF_INET ;
	p = sa.sa_data ;
	port = sp->s_port ;
	COPY ( p , & port , 2 );
	DBG7 (( "getservadr: socket address '%s' \n", addrname(&sa) ))

	return ( 0 );
}

/*==========================================================================*/

static	int     open_vmake_socket
(
    void
)
{
	int                     rc ;


	DBG2 (( "opnlstsock: called \n" ))

	rc = get_server_address ();
	if ( rc ) return ( rc );

	/* create listen socket */

	lsd = socket ( AF_INET , SOCK_STREAM , 0 );
	if ( lsd < 0 )
    {
	    DBG0 (( "cannot create listen socket: %s \n", serrno() ))
	    return ( 2 );
    }
	DBG7 (( "opnlstsock: opened listen socket %d \n", lsd ))

	/* bind the socket to the server address */

	rc = bind ( lsd , & sa , sizeof(sa) );
        if ( rc < 0 )
        {
	        if ( geterrno() == EADDRINUSE )
	        {
		        DBG0 (( "socket address already in use, " ))
		        DBG0 (( "probably a vmakserv is already running! \n" ))
	        }
	        else
	        {
    		    DBG0 (( "cannot bind listen socket: %s \n", serrno() ))
	        }
	        (void) close ( lsd );
	        return ( 2 );
        }
	DBG7 (( "opnlstsock: bound listen socket '%s' \n", addrname(&sa) ))

	rc = listen ( lsd , 5 );
	if ( rc < 0 )
        {
	        DBG0 (( "cannot switch socket to listen: %s \n", serrno() ))
	        (void) close ( lsd );
	        return ( 2 );
        }
	DBG7 (( "opnlstsock: listening on socket %d \n", lsd ))

	return ( 0 );
}

/*============================================================================*/

static  int     server_loop
(
    void
)
{
	int             rc ;
	int             lgt ;
	int             sd ;
	pid_t           sonpid ;
	int			    errcnt = 0 ;
	conn_info		* condat ;
	struct hostent  * hp ;
	struct sockaddr csa ;


	DBG2 (( "server_loop: called \n" ))
	DBG3 (( "server_loop: MAXFNMLGT %d \n", MAXFNMLGT ))
	DBG3 (( "server_loop: NAMESIZE  %d \n", NAMESIZE  ))
	DBG3 (( "server_loop: FILENMSIZ %d \n", FILENAMESIZE ))

	for ( ; ; )
	{
	    lgt = sizeof(csa);
	    DBG1 (( "master: accept \n" ))
	    sd = accept ( lsd , & csa , (int*) & lgt );
#ifdef	WIN32
	    if ( stop_requested )
	    {
		    DBG0 (( "server_loop: stop requested \n" ))
		    return ( 0 );
	    }
#endif	/*WIN32*/
	    if ( sd < 0 )
	    {
		    if ( geterrno() == EINTR ) continue;
		    errcnt ++ ;
		    DBG0 (( "accept error: %s \n", serrno() ))
		    if ( (errcnt > 9) || (geterrno() == EBADF) )
		        return ( 2 );
		    else
		        continue ;
	    }
	    errcnt = 0 ;
	    DBG3 (( "accepted connection sd %d \n", sd ))
	    DBG7 (( "client address '%s' \n", addrname(&csa) ))

	    lgt = 1 ;
	    rc = setsockopt ( sd , SOL_SOCKET , SO_KEEPALIVE ,
				    (char*) & lgt , sizeof(lgt) );
	    if ( rc < 0 )
	    {
    		DBG0 (( "WARNING: setsockopt (KEEPALIVE) error: %s \n", serrno() ))
	    }

            /* check, whether calling node is known to local node */

	    hp = gethostbyaddr ( csa.sa_data + 2 , 4 , AF_INET );
	    if ( hp == NULL )
	    {
		    DBG8 (( "first identification missed \n" ))
		    /* sometimes the first access fails, so try second time */
		    hp = gethostbyaddr ( csa.sa_data + 2 , 4 , AF_INET );
	    }
	    if ( hp == NULL )
	    {
		    DBG0 (( "unknown host address '%s' \n", addrname(&csa) ))
		    (void) close ( sd );
		    DBG1 (( "closed connection \n" ))
		    continue;
	    }
	    DBG1 (( "accepted connection from '%s' \n", hp->h_name ))

	    condat = (conn_info*) malloc ( sizeof(*condat) );
	    if ( ! condat )
	    {
		    DBG0 (( "insufficient memory for connection data: %s \n",
							    serrno() ))
		    (void) shutdown ( sd , 2 );
		    (void) close ( sd );
		    DBG1 (( "closed connection \n" ))
		    continue;
	    }
	    FILL ( condat , 0 , sizeof(*condat) );
	    condat->ci_sd = sd ;

#ifdef	unix
	    sonpid = fork ();
	    if ( sonpid == (pid_t) -1 )
	    {
    		DBG0 (( "cannot fork: %s \n", serrno() ))
	    }
	    if ( sonpid == 0 )
	    {
		    DBG2 (( "son: starting with pid %ld \n", (long) getpid() ))
		    (void) close ( lsd );

		    /*
		     *  Create an intermediate process, which exits immediately.
		     *  Thus, the son process becomes independent from the master.
		     */
		    sonpid = fork ();
		    if ( sonpid == (pid_t) -1 )
		    {
		        dbgdbg ( "son: cannot fork: %s \n", serrno() );
		        server_exit ( 3 );
		    }
		    if ( sonpid )
		    {
		        DBG2 (( "son: %ld exiting \n", (long) getpid() ))
		        (void) close ( sd );
		        exit ( 0 );
		    }

		    DBG1 (( "serv: starting with pid %ld \n", (long) getpid() ))
		    rc = serve ( condat );
		    DBG1 (( "serv: %ld exiting \n", (long) getpid() ))
		    server_exit ( rc );
	    }
	    if ( sonpid )
	    {
		    DBG4 (( "master: son started with pid %ld \n", (long) sonpid ))
		    (void) wait ( (int*) 0 );
	    }
	    free( (char*) condat);
	    (void) close ( sd );
	    DBG8 (( "master: connection %d closed \n", sd ))
#else	/*unix*/
	
	    rc = _beginthread ( serve , 0, (void*) condat  );
	    if ( rc == -1 )
	    {
    		server_exit ( 3 );
		}
#endif	/*unix*/

	}
	/*NOTREACHED*/

        /* no normal return ( loop forever ) */
}

/*============================================================================*/
#ifdef WIN32
static void serve
#else
static  DWORD	serve
#endif
(
    conn_info   * condat
)
{
	int rc ;
	int lgt ;


	DBG2 (( "serve: called \n" ))

	DBG4 (( "receiving ... \n" ))
	while ( ( lgt = get_message ( condat ) ) >= HEADER_SIZE )
	{
		DBG7 (( "received message (%d bytes) \n", lgt ))
		switch ( condat->ci_remote_msg.type )
		{
		case REM_TIME  :
				(void) get_time ( condat );
				break ;
		case REM_STAT  :
				(void) get_stat ( condat );
				break ;
		case REM_OPEN  :
				(void) open_file ( condat );
				break ;
		case REM_CLOSE :
				(void) close_file ( condat );
				break ;
		case REM_READ  :
				(void) read_file ( condat );
				break ;
		case REM_SEEK  :
				(void) seek_file ( condat );
				break ;
		case REM_DIROPEN  :
				(void) open_dir ( condat );
				break ;
		case REM_DIRCLOSE :
				(void) close_dir ( condat );
				break ;
		case REM_DIRREAD  :
				(void) read_dir ( condat );
				break ;
		case REM_INCOPY :
				(void) incopy_file ( condat , FT_TXT );
				break ;
		case REM_INCOPY_BIN :
				(void) incopy_file ( condat , FT_BIN );
				break ;
#ifdef WRITE_ENABLE
		case REM_OUTCOPY :
				(void) outcopy_file ( condat , FT_TXT );
				break ;
		case REM_OUTCOPY_BIN :
				(void) outcopy_file ( condat , FT_BIN );
				break ;
        case REM_MKDIR :
                (void) make_dir ( condat );
                break;
        case REM_RENAME :
                (void) rename_file ( condat );
                break;
        case REM_UNLINK :
                (void) unlink_file ( condat );
                break;
#endif
		default:
			DBG0 (( "vmakserv: unimplemented request %d \n",
							condat->ci_remote_msg.type ))
			FILL ( & condat->ci_remote_msg , 0 , HEADER_SIZE );
			rc = put_message ( condat );
			if ( rc < HEADER_SIZE )
			{
#ifdef	WIN32
				_endthread();
#else
				return ( rc );
#endif
			}
			break;
		}
		DBG4 (( "receiving ... \n" ))
	}
/*   PTS 1001420 &gar -> */
    close_all_files (condat);
	close_all_dirs (condat);
/*   <- PTS 1001420 */
	 (void) shutdown ( condat->ci_sd , 2 );
	(void) close ( condat->ci_sd );
	DBG1 (( "closed connection \n" ))
	free((char*) condat);
	rc = lgt ;
	DBG3 (( "serve returning %d \n", rc ))
#ifdef	WIN32
	_endthread();
#else
	return ( rc );
#endif
}


/*==========================================================================*/


static	int	get_time
(
    conn_info   * condat
)
{
	int     lgt , rc ;
	time_t  clock ;
	char    * p ;


	DBG6 (( "get_time: called \n" ))

	clock = time ( (time_t *) 0 );

	p = condat->ci_remote_msg.data ;
	p [ 0 ] = clock       & 0xFF ;
	p [ 1 ] = clock >>  8 & 0xFF ;
	p [ 2 ] = clock >> 16 & 0xFF ;
	p [ 3 ] = clock >> 24 & 0xFF ;

	lgt = 4 ;
	p = (char *) condat->ci_remote_msg.length ;
	p [ 0 ] = lgt      & 0xFF ;   /* low  byte */
	p [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

	rc = put_message ( condat );
	DBG6 (( "sent time message (%d bytes) \n", rc ))
	return ( rc );
}


/*==========================================================================*/


static	int	get_stat
(
    conn_info   * condat
)
{
	int         lgt , rc ;
	char        * p ;
	struct stat statbuf ;

	DBG6 (( "getstat: called \n" ))
	DBG7 (( "getstat: file '%s' \n", condat->ci_remote_msg.data ))

	rc = stat ( condat->ci_remote_msg.data , & statbuf );
	if ( rc < 0 )
	{
		rc = geterrno() ;
		DBG7 (( "getstat: stat error %d \n", rc ))

		p = condat->ci_remote_msg.data ;
		p [  0 ] = rc       & 0xFF ;   /* low  byte */
		p [  1 ] = rc >>  8 & 0xFF ;   /* high byte */

		lgt = 2 ;
		p = (char *) condat->ci_remote_msg.length ;
		p [ 0 ] = lgt      & 0xFF ;   /* low  byte */
		p [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

		rc = put_message ( condat );
		DBG6 (( "getstat: sent reply (%d bytes) \n", rc ))
		return ( rc );
	}

	p = condat->ci_remote_msg.data ;
	p [  0 ] = statbuf.st_mode        & 0xFF ;   /* low  byte */
	p [  1 ] = statbuf.st_mode  >>  8 & 0xFF ;   /* high byte */
	p [  2 ] = statbuf.st_ino         & 0xFF ;
	p [  3 ] = statbuf.st_ino   >>  8 & 0xFF ;
	p [  4 ] = statbuf.st_dev         & 0xFF ;
	p [  5 ] = statbuf.st_dev   >>  8 & 0xFF ;
	p [  6 ] = statbuf.st_rdev        & 0xFF ;
	p [  7 ] = statbuf.st_rdev  >>  8 & 0xFF ;
	p [  8 ] = statbuf.st_nlink       & 0xFF ;
	p [  9 ] = statbuf.st_nlink >>  8 & 0xFF ;
	p [ 10 ] = statbuf.st_uid         & 0xFF ;
	p [ 11 ] = statbuf.st_uid   >>  8 & 0xFF ;
	p [ 12 ] = statbuf.st_gid         & 0xFF ;
	p [ 13 ] = statbuf.st_gid   >>  8 & 0xFF ;
	p [ 14 ] = statbuf.st_size        & 0xFF ;
	p [ 15 ] = statbuf.st_size  >>  8 & 0xFF ;
	p [ 16 ] = statbuf.st_size  >> 16 & 0xFF ;
	p [ 17 ] = statbuf.st_size  >> 24 & 0xFF ;
	p [ 18 ] = statbuf.st_atime       & 0xFF ;
	p [ 19 ] = statbuf.st_atime >>  8 & 0xFF ;
	p [ 20 ] = statbuf.st_atime >> 16 & 0xFF ;
	p [ 21 ] = statbuf.st_atime >> 24 & 0xFF ;
	p [ 22 ] = statbuf.st_mtime       & 0xFF ;
	p [ 23 ] = statbuf.st_mtime >>  8 & 0xFF ;
	p [ 24 ] = statbuf.st_mtime >> 16 & 0xFF ;
	p [ 25 ] = statbuf.st_mtime >> 24 & 0xFF ;
	p [ 26 ] = statbuf.st_ctime       & 0xFF ;
	p [ 27 ] = statbuf.st_ctime >>  8 & 0xFF ;
	p [ 28 ] = statbuf.st_ctime >> 16 & 0xFF ;
	p [ 29 ] = statbuf.st_ctime >> 24 & 0xFF ;

	lgt = 30 ;
	p = (char *) condat->ci_remote_msg.length ;
	p [ 0 ] = lgt      & 0xFF ;   /* low  byte */
	p [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

	rc = put_message ( condat );
	DBG6 (( "getstat: sent reply (%d bytes) \n", rc ))
	return ( rc );
}


/*==========================================================================*/


static	int	open_file
(
    conn_info   * condat
)
{
	int     lgt , rc ;
	int     fno ;
	char    * p ;
	FILE    * fp ;

	DBG2 (( "openfile: called \n" ))
	DBG3 (( "openfile: name '%s' \n", condat->ci_remote_msg.data ))

	fno = condat->ci_remote_msg.reference ;
	DBG4 (( "openfile: fno   %d \n", fno ))

	if ( condat->ci_opened_f [ fno ] )
	{
		DBG5 (( "openfile: closing old file \n" ))
		(void) fclose ( condat->ci_fileptr [ fno ] );
		condat->ci_opened_f [ fno ] = 0 ;
	}

	p = condat->ci_remote_msg.data ;
	lgt = strlen ( p ) + 1 ; /* null byte included */
	p += lgt ;
	DBG4 (( "openfile: type '%s' \n", p ))

#ifndef	WRITE_ENABLE
	if ( strcmp(p , "r") )
	{
	    DBG7 (( "openfile: permission denied  '%s' \n", condat->ci_remote_msg.data ))
	    condat->ci_remote_msg.reference = (char) 255 ;
	    lgt = 0 ;
	    p = (char *) condat->ci_remote_msg.length ;
	    p [ 0 ] = lgt      & 0xFF ;   /* low  byte */
	    p [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

	    rc = put_message ( condat );
	    DBG6 (( "openfile: sent reply (%d bytes) \n", rc ))
	    return ( rc );
	}
#endif

	fp = fopen ( condat->ci_remote_msg.data , p );
	if ( fp == NULL )
	{
	    DBG7 (( "openfile: cannot (void) open '%s' \n", condat->ci_remote_msg.data ))
	    condat->ci_remote_msg.reference = (char) 255 ;
	    lgt = 0 ;
	    p = (char *) condat->ci_remote_msg.length ;
	    p [ 0 ] = lgt      & 0xFF ;   /* low  byte */
	    p [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

	    rc = put_message ( condat );
	    DBG6 (( "openfile: sent reply (%d bytes) \n", rc ))
	    return ( rc );
	}
	DBG7 (( "openfile: opened '%s' \n", condat->ci_remote_msg.data ))
	condat->ci_fileptr [ fno ] = fp ;
	condat->ci_opened_f [ fno ] = 1 ;

	for ( lgt = 0 ; lgt < MESSAGESIZE ; lgt += rc )
	{
		rc = fread ( (condat->ci_remote_msg.data + lgt) , 1 ,
					MESSAGESIZE - (CINT) lgt , fp );
		if ( rc == 0 )
		{
			DBG7 (( "openfile: reached eof \n" ))
			condat->ci_remote_msg.type = REM_CLOSE ;
			/*   PTS 1001420  &gar ->	*/
			close_local_file( condat, condat->ci_remote_msg.reference);
			/* <- &gar */
			break ;
		}
	}

	p = (char *) condat->ci_remote_msg.length ;
	p [ 0 ] = lgt      & 0xFF ;   /* low  byte */
	p [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

	rc = put_message ( condat );
	DBG6 (( "openfile: sent reply (%d bytes) \n", rc ))
	return ( rc );
}


/*==========================================================================*/

static	int	close_file
(
    conn_info   * condat
)
{
	int                     rc ;
/*	int                     fno ; */


	DBG2 (( "closefile: called \n" ))

	close_local_file( condat, condat->ci_remote_msg.reference);
/*   PTS 1001420															*/
/* &gar
	fno = condat->ci_remote_msg.reference ;
	DBG3 (( "closefile: fno   %d \n", fno ))

	if ( condat->ci_opened_f [ fno ] ) (void) fclose ( condat->ci_fileptr [ fno ] );
	condat->ci_opened_f [ fno ] = 0 ;
*/
	rc = put_message ( condat );
	DBG6 (( "closefile: sent reply (%d bytes) \n", rc ))
	return ( rc );
}


/* &gar ====================================================================*/
/* close local file and delete entries for concat->ci_fileptr[reference]    */
/* -------------------------------------------------------------------------*/
/*   PTS 1001420															*/
/* =========================================================================*/
static void close_local_file
(
	conn_info   * condat,
	char		reference
)
{
	int                     fno ;

    DBG2 (( "close_local_file: called \n" ))

	fno = reference ;
	DBG3 (( "close_local_file: fno   %d \n", fno ))

	if ( condat->ci_opened_f [ fno ] ) (void) fclose ( condat->ci_fileptr [ fno ] );
	condat->ci_opened_f [ fno ] = 0 ;
}


/* &gar ====================================================================*/
/* close all opened files for this connection                               */
/* -------------------------------------------------------------------------*/
/*   PTS 1001420															*/
/* =========================================================================*/
static void close_all_files
(
	conn_info   * condat
)
{
	char i;
	DBG2 (( "close_all_local_files: called \n" ))
	
	for (i=0; i<MAXREMOTEFILES;i++)
	{
		close_local_file( condat, i);
	}
}

/* &gar ====================================================================*/
/* close all opened directories for this connection                         */
/* -------------------------------------------------------------------------*/
/*   PTS 1001420															*/
/* =========================================================================*/
static void close_all_dirs
(
    conn_info   * condat
)
{
	int i;

	DBG2 (( "close_all_dirs: called \n" ))
	
	for (i=0; i<MAXREMOTEFILES;i++)
		if (condat->ci_dirptr[i])
			(void) closedir ( condat->ci_dirptr[i]);
	
}


/*==========================================================================*/


static	int	read_file
(
    conn_info   * condat
)
{
	int     lgt , rc ;
	int     fno ;
	char    * p ;
	FILE    * fp ;


	DBG2 (( "readfile: called \n" ))

	fno = condat->ci_remote_msg.reference ;
	DBG3 (( "readfile: fno   %d \n", fno ))
	fp = condat->ci_fileptr [ fno ] ;

	for ( rc=0/*lint*/, lgt = 0 ; lgt < MESSAGESIZE ; lgt += rc )
	{
		rc = fread ( (condat->ci_remote_msg.data + lgt) , 1 ,
					MESSAGESIZE - (CINT) lgt , fp );
		if ( rc == 0 )
		{
			DBG7 (( "readfile: reached eof \n" ))
			condat->ci_remote_msg.type = REM_CLOSE ;
			/*   PTS 1001420  &gar ->	*/
			close_local_file( condat, condat->ci_remote_msg.reference);
			/* <- &gar */
			break ;
		}
	}

	p = (char *) condat->ci_remote_msg.length ;
	p [ 0 ] = lgt      & 0xFF ;   /* low  byte */
	p [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

	rc = put_message ( condat );
	DBG6 (( "readfile: sent reply (%d bytes) \n", rc ))
	return ( rc );
}


/*==========================================================================*/

#ifdef	WRITE_ENABLE

static	int	write_file
(
    conn_info   * condat
)
{
	int                     lgt , rc ;
	int                     fno ;
	char                    * p ;


	DBG2 (( "writefile: called \n" ))

	fno = condat->ci_remote_msg.reference ;
	DBG3 (( "writefile: fno   %d \n", fno ))

	p = (char *) condat->ci_remote_msg.length ;
	lgt =   p [  0 ]       & 0x00FF |
		p [  1 ] <<  8 & 0xFF00 ;
	DBG7 (( "writefile: request %d  bytes \n", lgt ))

	lgt = fwrite ( condat->ci_remote_msg.data , 1 ,
					(CINT) lgt , condat->ci_fileptr [ fno ] );
	DBG6 (( "writefile: wrote %d bytes \n", lgt ))

	p = condat->ci_remote_msg.data ;
	p [ 0 ] = lgt      & 0xFF ;   /* low  byte */
	p [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

	lgt = 2 ;
	p = (char *) condat->ci_remote_msg.length ;
	p [ 0 ] = lgt      & 0xFF ;   /* low  byte */
	p [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

	rc = put_message ( condat );
	DBG6 (( "writefile: sent reply (%d bytes) \n", rc ))
	return ( rc );
}

#endif	/*WRITE_ENABLE*/

/*==========================================================================*/

static	int	seek_file
(
    conn_info   * condat
)
{
	int     lgt , rc ;
	int     fno , type ;
	long    offset ;
	char    * p ;
	FILE    * fp ;


	DBG2 (( "seekfile: called \n" ))

	fno = condat->ci_remote_msg.reference ;
	DBG3 (( "seekfile: fno   %d \n", fno ))
	fp = condat->ci_fileptr [ fno ] ;

	p = (char *) condat->ci_remote_msg.length ;
	lgt =   p [  0 ]       & 0x00FF |
		p [  1 ] <<  8 & 0xFF00 ;
	if ( lgt != 5 )
	{
		DBG0 (( "seekfile: illegal request format (%d bytes) \n", lgt ))
		rc = -1 ;
		p = condat->ci_remote_msg.data ;
		p [ 0 ] = rc      & 0xFF ;   /* low  byte */
		p [ 1 ] = rc >> 8 & 0xFF ;   /* high byte */
		lgt = 2 ;
		p = (char *) condat->ci_remote_msg.length ;
		p [ 0 ] = lgt      & 0xFF ;   /* low  byte */
		p [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */
		rc = put_message ( condat );
		DBG6 (( "seekfile: sent reply (%d bytes) \n", rc ))
		return ( rc );
	}

	p = condat->ci_remote_msg.data ;
	offset = p [  0 ]       & 0x000000FF |
		 p [  1 ] <<  8 & 0x0000FF00 |
		 p [  2 ] << 16 & 0x00FF0000 |
		 p [  3 ] << 24 & 0xFF000000 ;
	type =   p [  4 ]       & 0xFF ;

	rc = fseek ( condat->ci_fileptr [ fno ] , offset , type );
	DBG7 (( "seekfile: seek returned %d \n", rc ))

	p = condat->ci_remote_msg.data ;
	p [ 0 ] = rc      & 0xFF ;   /* low  byte */
	p [ 1 ] = rc >> 8 & 0xFF ;   /* high byte */

	for ( lgt = 2 /* rc above */ ; lgt < MESSAGESIZE ; lgt += rc )
	{
		rc = fread ( (condat->ci_remote_msg.data + lgt) , 1 ,
					MESSAGESIZE - (CINT) lgt , fp );
		if ( rc == 0 )
		{
			/*   PTS 1001420  &gar ->	*/
			close_local_file( condat, condat->ci_remote_msg.reference);
			/* <- &gar */
			DBG7 (( "seekfile: reached eof \n" ))
			condat->ci_remote_msg.type = REM_CLOSE ;
			break ;
		}
	}

	p = (char *) condat->ci_remote_msg.length ;
	p [ 0 ] = lgt      & 0xFF ;   /* low  byte */
	p [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

	rc = put_message ( condat );
	DBG6 (( "seekfile: sent reply (%d bytes) \n", rc ))
	return ( rc );
}


/*==========================================================================*/

static	int	open_dir
(
    conn_info   * condat
)
{
	int             idx , fno , lgt , rc ;
	char			chr ;
	char            * p ;
	DIR             * dirp ;
	struct dirent   * dp ;

	DBG2 (( "opendir: called \n" ))
	DBG3 (( "opendir: name '%s' \n", condat->ci_remote_msg.data ))

	fno = condat->ci_remote_msg.reference ;
	DBG3 (( "opendir: fno   %d \n", fno ))

	if ( condat->ci_opened_d [ fno ] )
	{
	    DBG5 (( "opendir: closing old file \n" ))
	    (void) closedir ( condat->ci_dirptr [ fno ] );
	    condat->ci_opened_d [ fno ] = 0 ;
	}

	dirp = opendir ( condat->ci_remote_msg.data );
	if ( ! dirp )
	{
	    DBG7 (( "opendir: cannot (void) open '%s' \n", condat->ci_remote_msg.data ))
	    condat->ci_remote_msg.reference = (char) 255 ;
	    lgt = 0 ;
	    p = (char *) condat->ci_remote_msg.length ;
	    p [ 0 ] = lgt      & 0xFF ;   /* low  byte */
	    p [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

	    rc = put_message ( condat );
	    DBG6 (( "opendir: sent reply (%d bytes) \n", rc ))
	    return ( rc );
	}
	DBG7 (( "opendir: opened '%s' \n", condat->ci_remote_msg.data ))
	condat->ci_dirptr [ fno ] = dirp ;
	condat->ci_opened_d [ fno ] = 1 ;

	FILL ( condat->ci_remote_msg.data , 0 , MESSAGESIZE );

	for ( lgt = 0 ; lgt < MESSAGESIZE - NAMESIZE ; )
	{
	    dp = readdir ( dirp );
	    if ( ! dp )
	    {
		    DBG7 (( "opendir: reached eof \n" ))
		    condat->ci_remote_msg.type = REM_DIRCLOSE ;
		    break ;
	    }
		if ( !( isalnum(dp->d_name[0]) || (dp->d_name[0] == '_') ||
			 ( dp->d_name[0] == '.' && isalnum(dp->d_name[1]) )
			  )
			)
		    continue ;

	    DBG7 (( "opendir: fname '%.14s' \n", dp->d_name ))

	    for ( idx = 0 ; idx < MAXFNMLGT ; idx ++ )
	    {
		    chr = dp->d_name[idx];
		    if ( chr == '\0' ) break ;
		    condat->ci_remote_msg.data[lgt ++] = chr ;
	    }
	    condat->ci_remote_msg.data[lgt ++] = '\0' ;
	}

	p = (char *) condat->ci_remote_msg.length ;
	p [ 0 ] = lgt      & 0xFF ;   /* low  byte */
	p [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

	rc = put_message ( condat );
	DBG6 (( "opendir: sent reply (%d bytes) \n", rc ))
	return ( rc );
}


/*==========================================================================*/

static	int	close_dir
(
    conn_info   * condat
)
{
	int                     rc ;
	int                     fno ;


	DBG2 (( "closedir: called \n" ))

	fno = condat->ci_remote_msg.reference ;
	DBG3 (( "closedir: fno   %d \n", fno ))

	if ( condat->ci_opened_d [ fno ] ) (void) closedir ( condat->ci_dirptr [ fno ] );
	condat->ci_opened_d [ fno ] = 0 ;

	rc = put_message ( condat );
	DBG6 (( "closedir: sent reply (%d bytes) \n", rc ))
	return ( rc );
}


/*==========================================================================*/

static	int	read_dir
(
    conn_info   * condat
)
{
	int                     lgt , idx , fno , rc ;
	char			chr ;
	char                    * p ;
	DIR                     * dirp ;
	struct dirent           * dp ;


	DBG2 (( "read_dir: called \n" ))

	fno = condat->ci_remote_msg.reference ;
	DBG3 (( "read_dir: fno   %d \n", fno ))
	dirp = condat->ci_dirptr [ fno ] ;

	FILL ( condat->ci_remote_msg.data , 0 , MESSAGESIZE );

	for ( lgt = 0 ; lgt < MESSAGESIZE - NAMESIZE ; )
	{
	    dp = readdir ( dirp );
	    if ( ! dp )
	    {
		    DBG7 (( "read_dir: reached eof \n" ))
		    condat->ci_remote_msg.type = REM_DIRCLOSE ;
		    break ;
	    }
		if ( !( isalnum(dp->d_name[0]) || (dp->d_name[0] == '_') ||
			 ( dp->d_name[0] == '.' && isalnum(dp->d_name[1]) )
			  )
			)
		    continue ;

	    DBG7 (( "read_dir: fname '%.14s' \n", dp->d_name ))

	    for ( idx = 0 ; idx < MAXFNMLGT ; idx ++ )
	    {
		    chr = dp->d_name[idx];
		    if ( chr == '\0' ) break ;
		    condat->ci_remote_msg.data[lgt ++] = chr ;
	    }
	    condat->ci_remote_msg.data[lgt ++] = '\0' ;
	}

	p = (char *) condat->ci_remote_msg.length ;
	p [ 0 ] = lgt      & 0xFF ;   /* low  byte */
	p [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

	rc = put_message ( condat );
	DBG6 (( "read_dir: sent reply (%d bytes) \n", rc ))
	return ( rc );
}


/*==========================================================================*/

/*ARGSUSED*/	/* filetype unused in unix */

static	int	incopy_file
(
    conn_info   * condat ,
    int			filetype
)
{
	int                     lgt , rc ;
	char                    * p ;
	FILE                    * fp ;

	DBG2 (( "incopy: called \n" ))
	DBG3 (( "incopy: name '%s' \n", condat->ci_remote_msg.data ))

#if unix
	fp = fopen ( (char *) condat->ci_remote_msg.data , "r" );
#else
	fp = fopen ( (char *) condat->ci_remote_msg.data ,
			(filetype == FT_BIN) ? "rb" : "r" );
#endif
	if ( fp == NULL )
	{
		DBG0 (( "incopy: cannot open '%s' \n", condat->ci_remote_msg.data ))
#ifdef WIN32
		MyFileError ("incopy_file", "cannot opening file", (char *) condat->ci_remote_msg.data, errno);
#endif

		condat->ci_remote_msg.reference = (char) 255 ;
		lgt = 0 ;
		p = (char *) condat->ci_remote_msg.length ;
		p [ 0 ] = lgt      & 0xFF ;   /* low  byte */
		p [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

		rc = put_message ( condat );
		DBG6 (( "incopy: sent reply (%d bytes) \n", rc ))
		return ( rc );
	}
	DBG7 (( "incopy: opened '%s' \n", condat->ci_remote_msg.data ))

	for ( ; ; )
	{
		for ( lgt = 0 ; lgt < MESSAGESIZE ; lgt += rc )
		{
			rc = fread ( (condat->ci_remote_msg.data + lgt) , 1 ,
					MESSAGESIZE - (CINT) lgt , fp );
			if ( rc == 0 )
			{
				DBG7 (( "incopy: reached eof \n" ))
				condat->ci_remote_msg.type = 0 ;
				break ;
			}
		}

		p = (char *) condat->ci_remote_msg.length ;
		p [ 0 ] = lgt      & 0xFF ;   /* low  byte */
		p [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

		rc = put_message ( condat );
		DBG6 (( "incopy: sent reply (%d bytes) \n", rc ))

		if ( condat->ci_remote_msg.type == 0 ) break ;
	}

	(void) fclose ( fp );
	DBG4 (( "incopy: returning %d \n", rc ))
	return ( rc );
}


/*==========================================================================*/

#ifdef	WRITE_ENABLE

static	int	outcopy_file
(
    conn_info   * condat ,
    int			filetype
)
{
	int                     lgt , rc ;
	char                    * p ;
	FILE                    * fp ;
	char                    fname [ FILENAMESIZE ] ;
	char                    tmpfname [ FILENAMESIZE ] ;
    time_t                  file_time;
	struct utimbuf	        utim ;

	DBG2 (( "outcopy_file: called \n" ))
	DBG3 (( "outcopy_file: dst '%s' \n", condat->ci_remote_msg.data ))

	FILL ( fname , 0 , sizeof(fname) );

	p = condat->ci_remote_msg.data ;
	lgt = strlen ( p );
	if ( lgt > sizeof(fname) - 1 )
	{
		DBG0 (( "outcopy_file: filename too long '%s' \n", p ))
		condat->ci_remote_msg.reference = (char) 255 ;
		(void) sprintf ( condat->ci_remote_msg.data , "filename too long" );
		lgt = strlen ( condat->ci_remote_msg.data );
		lgt ++ ; /* include null byte */
		p = (char *) condat->ci_remote_msg.length ;
		p [ 0 ] = lgt      & 0xFF ;   /* low  byte */
		p [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

		rc = put_message ( condat );
		DBG6 (( "outcopy_file: sent reply (%d bytes) \n", rc ))
		return ( rc );
	}
	COPY ( fname , condat->ci_remote_msg.data , lgt );

    (void) check_dir( fname );
	/*
	 *  Create a temporary file name.
	 *  If interrupted, vcopy_file should not leave an
	 *  incomplete file in the source directory.
	 */
    sprintf( tmpfname, "%s.copy", fname );
#if unix
    	fp = fopen ( tmpfname , "w" );
#else
	    fp = fopen ( tmpfname , (filetype == FT_BIN) ? "wb" : "w" );
#endif
	if ( fp == NULL )
	{
		DBG0 (( "outcopy_file: error opening '%s' \n", tmpfname ))
		condat->ci_remote_msg.reference = (char) 255 ;
		(void) sprintf ( condat->ci_remote_msg.data , "error opening file" );
		lgt = strlen ( condat->ci_remote_msg.data );
		lgt ++ ; /* include null byte */
		p = (char *) condat->ci_remote_msg.length ;
		p [ 0 ] = lgt      & 0xFF ;   /* low  byte */
		p [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

		rc = put_message ( condat );
		DBG6 (( "outcopy_file: sent reply (%d bytes) \n", rc ))
		return ( rc );
	}
	DBG7 (( "outcopy: opened '%s' \n", tmpfname ))

	p = condat->ci_remote_msg.data + lgt + 1;
	file_time = p [ 0 ]       & 0xFF     |
                p [ 1 ] <<  8 & 0xFF00   |
		        p [ 2 ] << 16 & 0xFF0000 |
                p [ 3 ] << 24 & 0xFF000000 ;

    lgt = 0 ;
	p = (char *) condat->ci_remote_msg.length ;
	p [ 0 ] = lgt      & 0xFF ;   /* low  byte */
	p [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

	rc = put_message ( condat );
	DBG6 (( "outcopy_file: sent reply (%d bytes) \n", rc ))


	for ( ; ; )
	{
		lgt = get_message ( condat );
		DBG6 (( "outcopy_file: got message (%d bytes) \n", lgt ))

		if ( fp == NULL )
		{
			if ( condat->ci_remote_msg.type == 0 ) break ;
			continue ;
		}
		lgt -= HEADER_SIZE ;

		rc = fwrite ( condat->ci_remote_msg.data , 1 , (CINT) lgt , fp );
		if ( rc != lgt )
		{
			DBG0 (( "outcopy_file: error writing '%s' \n", tmpfname ))
			(void) fclose ( fp );
			fp = NULL ;
			(void) unlink ( tmpfname );
            break;
		}
		if ( condat->ci_remote_msg.type == 0 ) break ;
	}

	if ( fp == NULL )
	{
		condat->ci_remote_msg.reference = (char) 255 ;
		(void) sprintf ( condat->ci_remote_msg.data , "error writing file" );
		lgt = strlen ( condat->ci_remote_msg.data );
		lgt ++ ; /* include null byte */
		p = (char *) condat->ci_remote_msg.length ;
		p [ 0 ] = lgt      & 0xFF ;   /* low  byte */
		p [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

		rc = put_message ( condat );
		DBG6 (( "outcopy_file: sent reply (%d bytes) \n", rc ))
		return ( rc );
	}
	else
	{
		(void) fclose ( fp );
        (void) unlink( fname );
        if ( rename( tmpfname, fname ) )
        {
			DBG0 (( "error renaming '%s' to '%s' ( errno %d ) \n", tmpfname, fname, geterrno() ))
		    condat->ci_remote_msg.reference = (char) 255 ;
		    (void) sprintf ( condat->ci_remote_msg.data , "cannot rename '%s' to 's': %s \n", tmpfname, fname , serrno() );
		    lgt = strlen ( condat->ci_remote_msg.data );
		    lgt ++ ; /* include null byte */
		    p = (char *) condat->ci_remote_msg.length ;
		    p [ 0 ] = lgt      & 0xFF ;   /* low  byte */
		    p [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

		    rc = put_message ( condat );
		    DBG6 (( "outcopy_file: sent reply (%d bytes) \n", rc ))
		    return ( rc );
        }
	    utim.actime = file_time ;
	    utim.modtime = file_time ;
	    rc = utime ( fname , & utim );
	    if ( rc < 0 )
	    {
		    DBG0 (( "cannot utime '%s': %s \n", fname , serrno() ))
		    condat->ci_remote_msg.reference = (char) 255 ;
		    (void) sprintf ( condat->ci_remote_msg.data , "cannot utime '%s': %s \n", fname , serrno() );
		    lgt = strlen ( condat->ci_remote_msg.data );
		    lgt ++ ; /* include null byte */
		    p = (char *) condat->ci_remote_msg.length ;
		    p [ 0 ] = lgt      & 0xFF ;   /* low  byte */
		    p [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

		    rc = put_message ( condat );
		    DBG6 (( "outcopy_file: sent reply (%d bytes) \n", rc ))
		    return ( rc );
	    }

		lgt = 0 ;
		p = (char *) condat->ci_remote_msg.length ;
		p [ 0 ] = lgt      & 0xFF ;   /* low  byte */
		p [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

		rc = put_message ( condat );
		DBG6 (( "outcopy_file: sent reply (%d bytes) \n", rc ))
		return ( rc );
	}
}

/*==========================================================================*/

static	int	make_dir
(
    conn_info   * condat
)
{
	int                     lgt , rc ;
	char                    * p ;
	char                    dname [ FILENAMESIZE ] ;

	DBG2 (( "rem_mkdir: called \n" ))

	FILL ( dname , 0 , sizeof( dname ) );
	p = condat->ci_remote_msg.data ;
	lgt = strlen ( p );
	if ( lgt > sizeof( dname ) - 1 )
	{
		DBG0 (( "rem_mkdir: dirname too long '%s' \n", p ))
		condat->ci_remote_msg.reference = (char) 255 ;
		(void) sprintf ( condat->ci_remote_msg.data , "dirname too long" );
		lgt = strlen ( condat->ci_remote_msg.data );
		lgt ++ ; /* include null byte */
		p = (char *) condat->ci_remote_msg.length ;
		p [ 0 ] = lgt      & 0xFF ;   /* low  byte */
		p [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

		rc = put_message ( condat );
		DBG6 (( "rem_mkdir: sent reply (%d bytes) \n", rc ))
		return ( rc );
	}
	COPY ( dname , condat->ci_remote_msg.data , lgt );

    DBG0 (( "rem_mkdir: dname '%s' \n", dname ))
    rc = mkdir( dname , 0777 );

	if ( rc != 0 )
	{
        DBG0 (( "rem_mkdir: error making dir '%s' \n", dname ))
		rc = geterrno() ;
		DBG0 (( "rem_mkdir: mkdir error %d \n", rc ))
		condat->ci_remote_msg.reference = (char) 255 ;
		(void) sprintf ( condat->ci_remote_msg.data , "mkdir error %d", rc );
		
        lgt = strlen ( condat->ci_remote_msg.data );
		lgt ++ ; /* include null byte */

        p = (char *) condat->ci_remote_msg.length ;
		p [ 0 ] = lgt      & 0xFF ;   /* low  byte */
		p [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

		rc = put_message ( condat );
		DBG6 (( "getstat: sent reply (%d bytes) \n", rc ))
		return ( rc );
	}

	lgt = 0 ;
	p = (char *) condat->ci_remote_msg.length ;
	p [ 0 ] = lgt      & 0xFF ;   /* low  byte */
	p [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

	rc = put_message ( condat );
	DBG6 (( "rem_mkdir: sent reply (%d bytes) \n", rc ))
	return ( rc );

}

/*==========================================================================*/

static	int	rename_file
(
    conn_info   * condat
)
{
	int                     lgt , lto , rc ;
	char                    * p ;
	char                    fname [ NAMESIZE ] ;
	char                    fto   [ NAMESIZE ] ;

	DBG2 (( "rename_file: called \n" ))
	DBG3 (( "rename_file: src '%s' \n", condat->ci_remote_msg.data ))
	DBG3 (( "rename_file: dst '%s' \n", condat->ci_remote_msg.data +
            strlen( condat->ci_remote_msg.data ) + 1 ))

	p = condat->ci_remote_msg.data ;
	lgt = strlen ( p ) + 1 ; /* null byte included */
	p += lgt ;
	lto = strlen ( p ) + 1 ; /* null byte included */

	if ( lgt - 1 > MAXFNMLGT || lto - 1 > MAXFNMLGT )
	{
		DBG0 (( "link: filename too long \n" ))
		condat->ci_remote_msg.reference = (char) 255 ;
		(void) sprintf ( condat->ci_remote_msg.data , "filename too long" );
		lgt = strlen ( condat->ci_remote_msg.data );
		lgt ++ ; /* include null byte */
		p = (char *) condat->ci_remote_msg.length ;
		p [ 0 ] = lgt      & 0xFF ;   /* low  byte */
		p [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

		rc = put_message ( condat );
		DBG6 (( "link: sent reply (%d bytes) \n", rc ))
		return ( rc );
	}

	COPY ( fname , condat->ci_remote_msg.data , lgt ); /* null byte included */
	COPY ( fto   , p               , lto ); /* null byte included */

    rc = check_dir( fto );

    /* DBG0 (( "rename_file: fname %s \n", fname )) */
    /* DBG0 (( "rename_file: fto   %s \n", fto   )) */

    rc += rename ( fname , fto );
	if ( rc )
	{
		rc = geterrno() ;
		DBG4 (( "rename_file: rename error %d \n", rc ))
		condat->ci_remote_msg.reference = (char) 255 ;
		(void) sprintf ( condat->ci_remote_msg.data , "link error %d", rc );
		lgt = strlen ( condat->ci_remote_msg.data );
		lgt ++ ; /* include null byte */
		p = (char *) condat->ci_remote_msg.length ;
		p [ 0 ] = lgt      & 0xFF ;   /* low  byte */
		p [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

		rc = put_message ( condat );
		DBG6 (( "rename_file: sent reply (%d bytes) \n", rc ))
		return ( rc );
	}

	lgt = 0 ;
	p = (char *) condat->ci_remote_msg.length ;
	p [ 0 ] = lgt      & 0xFF ;   /* low  byte */
	p [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

	rc = put_message ( condat );
	DBG6 (( "rename_file: sent reply (%d bytes) \n", rc ))
	return ( rc );
}


/*==========================================================================*/

static	int	unlink_file
(
    conn_info   * condat
)
{
	int                     lgt , rc ;
	char                    * p ;
	char                    fname [ NAMESIZE ] ;

	DBG2 (( "unlink: called \n" ))
	DBG3 (( "unlink: name '%s' \n", condat->ci_remote_msg.data ))

	p = condat->ci_remote_msg.data ;
	lgt = strlen ( p ) + 1 ; /* null byte included */

	if ( lgt - 1 > MAXFNMLGT )
	{
		DBG0 (( "unlink: filename too long \n" ))
		condat->ci_remote_msg.reference = (char) 255 ;
		(void) sprintf ( condat->ci_remote_msg.data , "filename too long" );
		lgt = strlen ( condat->ci_remote_msg.data );
		lgt ++ ; /* include null byte */
		p = (char *) condat->ci_remote_msg.length ;
		p [ 0 ] = lgt      & 0xFF ;   /* low  byte */
		p [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

		rc = put_message ( condat );
		DBG6 (( "unlink: sent reply (%d bytes) \n", rc ))
		return ( rc );
	}
	COPY ( fname , condat->ci_remote_msg.data , lgt ); /* null byte included */
    /* DBG0 (( "unlink fname: %s \n", fname )) */
	rc = unlink ( fname );
	if ( rc )
	{
	    rc = geterrno() ;
	    if ( (rc != ENOENT) && (rc != ENOTDIR) )
	    {
		    DBG0 (( "unlink: unlink error %d \n", rc ))
		    condat->ci_remote_msg.reference = (char) 255 ;
		    (void) sprintf ( condat->ci_remote_msg.data , "unlink error %d", rc );
		    lgt = strlen ( condat->ci_remote_msg.data );
		    lgt ++ ; /* include null byte */
		    p = (char *) condat->ci_remote_msg.length ;
		    p [ 0 ] = lgt      & 0xFF ;   /* low  byte */
		    p [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */
		    rc = put_message ( condat );
		    DBG6 (( "unlink: sent reply (%d bytes) \n", rc ))
		    return ( rc );
	    }
	}

	lgt = 0 ;
	p = (char *) condat->ci_remote_msg.length ;
	p [ 0 ] = lgt      & 0xFF ;   /* low  byte */
	p [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

	rc = put_message ( condat );
	DBG6 (( "unlink: sent reply (%d bytes) \n", rc ))
	return ( rc );
}

#endif	/*WRITE_ENABLE*/

/*==========================================================================*/

static	int     get_message
(
    conn_info   * condat
)
{
	int             msglgt , lgt , rc ;
    static  int     overflowlgt ;
    static  char    overflow [ HEADER_SIZE + MESSAGESIZE ] ;


	DBG2 (( "getmsg: called \n" ))

	lgt = overflowlgt ;
	if ( lgt )
	{
		COPY ( & condat->ci_remote_msg , overflow , lgt );
		overflowlgt = 0 ;
	}

	for ( rc=0/*lint*/ ; lgt < HEADER_SIZE ; lgt += rc )
	{
		rc = recv ( condat->ci_sd , (char *) & condat->ci_remote_msg  + lgt ,
				     sizeof(condat->ci_remote_msg) - lgt , 0 );
		if ( rc <= 0 )
		{
			if ( rc < 0 )
			{
				DBG1 (( "message read error: %s \n", serrno() ))
				return ( rc > 0 ? -rc : -EIO );
			}
			DBG8 (( "connection closed by client \n" ))
			return ( 0 );
		}
		DBG7 (( "getmsg: got %d bytes \n", rc ))
	}
	DBG8 (( "getmsg: header complete (got %d bytes) \n", lgt ))

	msglgt = HEADER_SIZE +
		 (condat->ci_remote_msg.length [ 0 ]      & 0x00FF |
		  condat->ci_remote_msg.length [ 1 ] << 8 & 0xFF00   );
	if ( msglgt < HEADER_SIZE || msglgt > sizeof(condat->ci_remote_msg) )
	{
		DBG0 (( "illegal message length %d \n", msglgt ))
		return ( -EIO );
	}
	DBG8 (( "getmsg: complete message length is %d bytes \n", msglgt ))

	for ( ; lgt < msglgt ; lgt += rc )
	{
		rc = recv ( condat->ci_sd , (char *) & condat->ci_remote_msg  + lgt ,
				     sizeof(condat->ci_remote_msg) - lgt , 0 );
		if ( rc <= 0 )
		{
			if ( rc < 0 )
			{
				DBG0 (( "message read error: %s \n", serrno() ))
				return ( -2 );
			}
			DBG0 (( "connection closed by client \n" ))
			return ( 0 );
		}
		DBG7 (( "getmsg: got %d bytes \n", rc ))
	}
	DBG7 (( "getmsg: message complete (got %d bytes) \n", lgt ))

	if ( lgt > msglgt )
	{
		overflowlgt = lgt - msglgt ;
		COPY ( overflow , ((char*)&condat->ci_remote_msg) + msglgt , overflowlgt );
		lgt -= overflowlgt ;
	}

	DBG3 (( "getmsg: type      %d \n", (int) condat->ci_remote_msg.type ))
	DBG3 (( "getmsg: reference %d \n", (int) condat->ci_remote_msg.reference ))
	DBG4 (( "getmsg: returning %d \n", lgt ))
	return ( lgt );
}


/*==========================================================================*/


static	int     put_message
(
    conn_info   * condat
)
{
	int                     msglgt , rc ;

    DBG2 (( "putmsg: called \n" ))
	DBG3 (( "putmsg: type      %d \n", (int) condat->ci_remote_msg.type ))
	DBG3 (( "putmsg: reference %d \n", (int) condat->ci_remote_msg.reference ))

	msglgt = HEADER_SIZE +
		 (condat->ci_remote_msg.length [ 0 ]      & 0x00FF |
		  condat->ci_remote_msg.length [ 1 ] << 8 & 0xFF00   );
	DBG4 (( "putmsg: length    %d \n", msglgt ))
	if ( msglgt < HEADER_SIZE || msglgt > sizeof(condat->ci_remote_msg) )
	{
		DBG0 (( "illegal message length %d \n", msglgt ))
		return ( -EIO );
	}

	rc = send ( condat->ci_sd , (char *) & condat->ci_remote_msg , (unsigned) msglgt , 0 );
	if ( rc != msglgt )
	{
		if ( rc < 0 )
		{
			DBG0 (( "socket write error: %s \n", serrno() ))
		}
		else
		{
			DBG0 (( "socket write size error %d \n", rc ))
		}
		/* server_exit ( rc ? rc : EIO ); PTS 1104553 */
	}
	DBG7 (( "putmsg: sent message (%d bytes) \n", rc ))

	return ( rc );
}


/*==========================================================================*/

static  int     set_signals
(
    SIG_RTRN    (*func)()
)
{

#ifdef SIGHUP
	(void) signal ( SIGHUP,  (SIG_CAST) func );
#endif
	(void) signal ( SIGINT,  (SIG_CAST) func );
#ifdef SIGPIPE
	(void) signal ( SIGPIPE, SIG_IGN );
#endif
#ifdef SIGALRM
	(void) signal ( SIGALRM, (SIG_CAST) catch_signal );
#endif
	(void) signal ( SIGTERM, (SIG_CAST) catch_signal );
#ifdef SIGCLD
	(void) signal ( SIGCLD,  SIG_IGN );
#endif

	return ( 0 );
}


/*==========================================================================*/

static  SIG_RTRN        catch_signal
(
    int sig
)
{

	(void) signal ( sig, SIG_IGN );

	switch ( sig )
	{
	case SIGINT:
	case SIGTERM:
		    (void) close ( lsd );
		    DBG1 (( "sockets closed \n" ))
#ifdef	unix
		    if ( getpid() == master_pid )
		    {
			DBG0 (( "%s   server stopped \n", vmake_version + 4 ))
			(void) remove_pid_file ();
		    }
#endif	/*unix*/
		    server_exit ( -sig );
		    /*NOTREACHED*/
		    break ;

#ifdef SIGALRM
	case SIGALRM:
		    /* timeout = 1 ; */
		    break;
#endif
	}

	(void) signal ( sig, (SIG_CAST) catch_signal );
}


/*==========================================================================*/

static	int     wait_for_termination
(
    int			fd
)
{
	int		ec ;
	int     lgt ;
	char    line [ 256 ];


	/*
	 *  Assume an error.
	 */
	ec = 1 ;

	for ( ; ; )
	{
	    lgt = read ( fd , line , sizeof(line) );
	    if ( lgt <= 0 ) break ;
	    /*
	     *  On success, the last character transmitted is null.
	     */
	    ec = line [ lgt - 1 ];
	    if ( ec == '\0' ) lgt -- ;
	    (void) write ( 1 , line , (unsigned) lgt );
	}

	(void) sleep ( 1 ); /* allow output queue to drain */
	return ( ec );
}

/*==========================================================================*/

#ifdef	unix
static	int     kill_server
(
    void
)
{
	int     rc = -1 ;
	int     idx ;
	long	lpid ;
	FILE    * fp ;


	DBG4 (( "killserv: called \n" ))

	fp = fopen ( PIDFILE , "r" );
	if ( fp != NULL )
	{
	    DBG1 (( "trying to send termination signal \n" ))
	    (void) fscanf ( fp , "%ld", &lpid );
	    (void) fclose ( fp );

	    if ( lpid > 1 )
	    {
#ifdef	KILL_GROUP
		for ( idx = 0 ; idx < 70 ; idx ++ )
		{
		    rc = kill ( (pid_t) (- lpid) , SIGTERM );
		    if ( (rc < 0) && (geterrno() == ESRCH) )
		    {
		        DBG0 (( "vmakserv stopped \n" ))
		        return ( 0 );
		    }
		    if ( rc < 0 ) break ;
		    DBG1 (( "sent termination signal to process group \n" ))
		    if ( idx < 70 ) (void) sleep ( 1 );
		}
		DBG0 (( "cannot kill process group %ld: %s \n",
						lpid , serrno() ))
#endif	/*KILL_GROUP*/

		for ( idx = 0 ; idx < 70 ; idx ++ )
		{
		    rc = kill ( (pid_t) lpid , SIGTERM );
		    if ( (rc < 0) && (geterrno() == ESRCH) )
		    {
		        DBG0 (( "vmakserv master stopped \n" ))
		        return ( 0 );
		    }
		    if ( rc < 0 ) break ;
		    DBG1 (( "sent termination signal to master process \n" ))
		    if ( idx < 70 ) (void) sleep ( 1 );
		}
		DBG0 (( "cannot kill master process %ld: %s \n",
						lpid , serrno() ))
	    }
	}
	else
	{
	    DBG0 (( "no master pid file '%s', cannot kill \n", PIDFILE ))
	}

	DBG4 (( "killserv: returning %d \n", rc ))
	return ( rc );
}

/*==========================================================================*/

static	int     create_pid_file
(
    void
)
{
	int                     fd , lgt , rc ;
	char                    lin [ 16 ];


	fd = open ( PIDFILE , O_WRONLY|O_CREAT|O_TRUNC , 0644 );
	if ( fd < 0 )
	{
		DBG0 (( "cannot create pid file: %s \n", serrno() ))
		return ( 2 );
	}
	(void) sprintf ( lin , "%ld\n", (long) master_pid );
	lgt = strlen ( lin );
	rc = write ( fd , lin , (unsigned) lgt );
	if ( rc != lgt )
	{
	    if ( rc < 0 )
	    {
		    DBG0 (( "cannot write pid file: %s \n", serrno() ))
		    (void) unlink ( PIDFILE );
		    (void) close ( fd );
		    return ( 2 );
	    }
	    DBG0 (( "pid file write size error %d \n", rc ))
	    DBG0 (( "           bytes to write %d \n", lgt ))
	    (void) unlink ( PIDFILE );
	    (void) close ( fd );
	    return ( 2 );
	}
	(void) close ( fd );
	return ( 0 );
}

/*==========================================================================*/

static	int     remove_pid_file
(
    void
)
{
	int                     rc ;


	rc = unlink ( PIDFILE );
	if ( rc < 0 )
	{
		DBG0 (( "cannot unlink pid file: %s \n", serrno() ))
		return ( 2 );
	}
	return ( 0 );
}
#endif /*unix*/

/*==========================================================================*/

static	void	close_all
(
    void
)
{
#ifdef	unix
	int			fd ;


	for ( fd = 3 ; fd < 100 ; fd ++ )
	{
	    (void) close ( fd );
	}
#endif	/*unix*/
}

/*==========================================================================*/

static  void	server_exit
(
    int exit_value
)
{
#ifdef	WIN32
    int rc;
    stop_requested = TRUE ;
    rc = shutdown( lsd, 0x02 );
    if ( rc )
    {
        rc = closesocket( lsd );
        if ( rc )
        {
            DBG0(( "closesocket failed : %d \n", WSAGetLastError() ))
        }
    }
#else	/*unix*/
	(void) close ( lsd );
	exit ( exit_value );
	/*NOTREACHED*/
#endif	/*unix*/
}


/*==========================================================================*/

/**VARARGS1*/
/*PRINTFLIKE1*/
global	void	dbgdbg
(
    char    * fmt ,
    ...
)
{
	int             lgt,rc ;
    time_t          clock ;
    struct tm       * tim ;
	va_list			args ;
	char            line [ LINESIZE ] ;

#ifdef  lint
	if ( dbgdbglvl < dbgcurlvl ) return ;
#endif

	lgt = 0 ;
	if ( write_timestamp )
	{
	    (void) time ( & clock );
	    tim = localtime ( & clock );

	    (void) sprintf ( line , "%02d-%02d-%02d %02d:%02d:%02d ",
			tim->tm_mday , tim->tm_mon + 1 , tim->tm_year ,
			tim->tm_hour , tim->tm_min , tim->tm_sec );
	    lgt = strlen ( line );
	}

	va_start ( args, fmt );
#ifdef WIN32
	rc = _vsnprintf ( line + lgt , LINESIZE - lgt, fmt , args );
#else
	rc = vsprintf ( line , fmt , args );
#endif
	va_end ( args );
	if ( rc == -1 )
/*	 line to short for args */
			line [ LINESIZE - 1 ] = '\0';
	lgt = strlen ( line );

	(void) write ( 1 , line , (unsigned) lgt );
#ifdef _DEBUG
    _commit( 1 );
#endif
}


/*==========================================================================*/

global	char	*serrno
(
    void
)
{
    int				len ;
    int				err ;
    static	char		line [ 128 ];
    err = geterrno ();
    (void) sprintf ( line , "%d, ", err );
    len = strlen ( line );
    (void) sprintf ( line + len , "%.*s",
			sizeof(line) - len - 1 , strerror(err) );
    return ( line );
}

/*==========================================================================*/

static	char    * addrname
(
    struct sockaddr    * sap
)
{
    static  char                    line [ 80 ] ;

	(void) sprintf ( line , "fam %d, port %d, host %d.%d.%d.%d",
				(int) sap->sa_family ,
				((int) sap->sa_data [ 0 ] & 0xFF) << 8 |
				((int) sap->sa_data [ 1 ] & 0xFF) << 0 ,
				((int) sap->sa_data [ 2 ] & 0xFF) ,
				((int) sap->sa_data [ 3 ] & 0xFF) ,
				((int) sap->sa_data [ 4 ] & 0xFF) ,
				((int) sap->sa_data [ 5 ] & 0xFF) );
	return ( line );
}

#endif  /* SOCKETS */

/*==========================================================================*/

#ifdef	MKDIR
static	int     mkdir
(
    char    * dir ,
    int     mode
)
{
	int                     rc ;
	char                    * p , * cmd ;
	SIG_RTRN                (* oldsigcld)();


	p = "mkdir %s;chmod 0%03o %s";
	cmd = malloc ( strlen(p) + 2 * strlen(dir) );
	if ( ! cmd )
	{
		DBG0 (( "insufficient memory \n" ))
		seterrno(ENOMEM) ;
		return ( -1 );
	}
	(void) sprintf ( cmd , p , dir , mode , dir );

	oldsigcld = signal ( SIGCLD , SIG_DFL ); /* sh needs it */

	rc = system ( line );

	(void) signal ( SIGCLD , (SIG_CAST) oldsigcld );
	free( cmd );

	if ( rc )
	{
	    seterrno ( rc >> 8 );
	    return ( rc );
	}
	return ( 0 );
}
#endif	/*MKDIR*/

/*==========================================================================*/
#ifdef WIN32
static  int     create_task
(
    DWORD   (WINAPI *function)(LPVOID),
    LPVOID  arg ,
    pid_t   * task_id
)
{
//    HANDLE                      hthread ;
    DWORD                       thread_id ;
    LPSECURITY_ATTRIBUTES       lpsa ;
    DWORD                       cbStack ;
    LPTHREAD_START_ROUTINE      lpStartAddr ;
    LPVOID                      lpvThreadParm ;
    DWORD                       fdwCreate ;
    LPDWORD                     lpIDThread ;
    SECURITY_ATTRIBUTES		saAttr ;

	unsigned long hlthread;

    lpsa          = & saAttr ;
    cbStack       = 0 ;
    lpStartAddr   = function ;
    lpvThreadParm = arg ;
    fdwCreate     = 0 ;
    lpIDThread    = &thread_id ;



    /*
     *  Set the bInheritHandle flag so handles are inherited.
     */
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE ;
    saAttr.lpSecurityDescriptor = NULL ;

    /*
	hthread = CreateThread ( lpsa , cbStack , lpStartAddr ,
				lpvThreadParm , fdwCreate , lpIDThread );
		
    if ( ! hthread )
    {
	    DBG0 (( "cannot create thread: %s\n", serrno() ))
	    return ( -1 );
    }
    if ( task_id ) *task_id = thread_id ;

    CloseHandle ( hthread );

	*/
	/*
	hlthread = _beginthreadex ( lpsa , cbStack , lpStartAddr ,
				lpvThreadParm , fdwCreate , lpIDThread );
	*/		
/*	hlthread = _beginthread (lpStartAddr, cbStack, lpvThreadParm  ); */
	/*
	(void*) lpStartAddr , cbStack ,
				lpvThreadParm );
*/
    if ( hlthread < 0 )
    {
	    DBG0 (( "cannot create thread: %s\n", serrno() ))
	    return ( -1 );
    }
    if ( task_id ) *task_id = thread_id ;

    return ( 0 );
}


/*==========================================================================*/

static	struct dirent   dirent ;
global	DIR	*opendir
(
    char    * path
)
{
	unsigned	lgt ;
	char		* ptr ;
	DIR			* dirp ;
	int			ErrorNumber;


	lgt = strlen ( path );
	if ( ! lgt )
	{
	    seterrno ( ENOENT );
	    return ( (DIR*) 0 );
	}
	dirp = (DIR*) malloc ( sizeof(DIR) + lgt + 4 );
	if ( ! dirp )
	{
	    seterrno ( ENOMEM );
	    return ( (DIR*) 0 );
	}
	FILL ( dirp , 0 , sizeof(DIR) + lgt + 4 );
	dirp->dir_handle = -1 ;
	ptr = dirp->dir_name ;
	if ( (lgt > 1) || ((lgt == 1) && (*path != '.')) )
	{
	    for ( ; *path ; ptr ++ , path ++ )
	    {
		if ( *path == '\\' )
		    *ptr = '/' ;
		else
		    *ptr = *path ;
	    }
	    if ( *(ptr - 1) != '/' ) *(ptr ++) = '/' ;
	}
	*(ptr ++) = '*' ;
	*(ptr ++) = '.' ;
	*(ptr ++) = '*' ;
	DBG4 (( "opendir: open '%s' \n", dirp->dir_name ))
	dirp->dir_handle = _findfirst ( dirp->dir_name , &dirp->dir_data );
	if ( dirp->dir_handle == -1 )
	{
		ErrorNumber = geterrno();
	    switch ( ErrorNumber )
	    {
	    case ENOENT :
		    dirp->dir_entryno = -1 ;
		    break ;

	    default :
			MyFileError("opendir","findfirst failed", dirp->dir_name, ErrorNumber);
		    free( (char*) dirp);
		    return ( (DIR*) 0 );
	    }
	}
	else
	{
	    dirp->dir_entryno = 0 ;
	}
	return ( dirp );
}

/*==========================================================================*/

global	struct dirent	* readdir
(
    DIR			* dirp
)
{
	long    lrc ;
	int ErrorNumber;

	if ( ! dirp )
	{
	    seterrno ( EBADF );
	    return ( (struct dirent*) 0 );
	}
	if ( dirp->dir_entryno == -1 )
	{
	    seterrno ( ENOENT );
	    return ( (struct dirent*) 0 );
	}
	if ( dirp->dir_entryno != 0 )
	{
	    lrc = _findnext ( dirp->dir_handle , &dirp->dir_data );
		if ( lrc == -1 )
	    {
			ErrorNumber = geterrno();
			if (ErrorNumber != ENOENT)
			{
				MyFileError ("readdir", "findnext failed", dirp->dir_name, ErrorNumber);
			}
		    dirp->dir_entryno = -1 ;
		    return ( (struct dirent*) 0 );
	    }
	}
    dirp->dir_entryno ++ ;
    DBG4 (( "readdir: dir entry '%s' \n", dirp->dir_data.name ))
    FILL ( &dirent , 0 , sizeof(dirent) );
	dirent.d_reclen = sizeof(dirent);
	dirent.d_off    = sizeof(dirent) * dirp->dir_entryno ;
	(void) strncpy ( dirent.d_name , dirp->dir_data.name ,
        sizeof(dirent.d_name)-1 );
    return ( &dirent );
}

/*==========================================================================*/

global	int	closedir
(
    DIR			* dirp
)
{
	if ( ! dirp )
	{
	    seterrno ( EBADF );
	    return ( -1 );
	}
	if ( dirp->dir_handle != -1 )
	{
	    (void) _findclose ( dirp->dir_handle );
	}
	free( (char*) dirp);
	return ( 0 );
}


void MyFileError(const char * Function, const char * errString, char * filename, DWORD dwError)
{
	TCHAR buf[1024];

	sprintf(buf, "vmakserv file read error in %s: %s (error %li)[%s]",
		Function, errString, dwError, filename);
	DBG0 ((buf))
	AddToMessageLog(buf);
}

#endif /*WIN32*/

/*==========================================================================*/

static int	check_dir /* takes a file name (not a directory) */
(
    char			* file
)
{
	int			    c , rc ;
	char			* p ;
	char			path [ FILENAMESIZE ];
	char			dirbuf [ FILENAMESIZE ];

	(void) strcpy ( path , file );

	(void) strcpy ( path , file );
	p = strrchr ( path , '/' );
	if ( ! p ) return ( 0 ); /* filename only (in current directory) */
	*p = 0 ; /* terminate the directory path */

	rc = test_is_dir ( path );
	if ( rc == 0 ) return ( 0 ); /* exists already */

    (void) strcpy ( dirbuf , path );

	for ( p = dirbuf ; * p ; p ++ )
	{
	    while ( * p == '/' ) p ++ ;
	    while ( (* p != '/') && (* p != '\0') ) p ++ ;
	    c = * p ;
	    * p = '\0' ;

	    rc = test_is_dir ( dirbuf );
	    if ( rc )
	    {
		    rc = mkdir ( dirbuf , 0777 );
		    if ( rc == -1 )
		    {
		        DBG0 (( "cannot create directory '%s': %s \n", dirbuf , serrno() ))
		        return ( 2 );
		    }
	    }

	    if ( c == '\0' ) break ;
	    * p = (char) c ;
	}

	rc = test_is_dir ( path );
	if ( rc )
	{
	    DBG0 (( "failed to create directory '%s' \n", path ))
	    return ( rc );
	}

	return ( 0 );
}

/*==========================================================================*/

static	int	test_is_dir
(
    char			* path
)
{
	int			rc ;
	char			*ptr ;
	struct	stat		statbuf ;
#if WIN32
static	char			drive []= "c:/" ;
#endif


	ptr = path ;
#if WIN32
	/*
	 *  The drive identifier must have a '/' to be
	 *  recognized as a directory.
	 */
	if ( isalpha(ptr[0]) && (ptr[1] == ':') && ! ptr[2] )
	{
	    drive[0] = *ptr ;
	    ptr = drive ;
	}
#endif
	/* rc = stat ( ptr , & statbuf ); */
    rc = stat( ptr , & statbuf );
	if ( rc < 0 )
	{
	    if ( (geterrno() != ENOENT) && (geterrno() != ENOTDIR) )
	    {
		    DBG0 (( "cannot stat '%s': %s \n", ptr , serrno() ))
		    return ( 2 );
	    }
	    DBG4 (( "testisdir: directory does not exist '%s' \n", ptr ))
	    return ( 1 );
	}

	/* path exists */

	if ( ! (statbuf.st_mode & S_IFDIR) )
	{
	    DBG4 (( "'%s' is no directory \n", ptr ))
	    return ( 2 );
	}

	DBG4 (( "testisdir: directory exists '%s' \n", ptr ))
	return ( 0 );
}

/*==========================================================================*/


