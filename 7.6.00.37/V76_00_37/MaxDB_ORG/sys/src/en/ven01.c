/*!
  @file           ven01.c
  @author         JoergM
  @brief          Client RunTime Initialization Functions
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

#define MOD__ "ven01.c:"

#include "gen00.h"
					/* type of sqlinit(component) */
#define		COMPSIZ			(sizeof(tsp00_C64))

#include	<pwd.h>

#include "heo46.h"
#include "geo001.h"
#include "geo57.h"
#include "gen22.h"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageOutput.h"
#include "RunTime/Security/Crypto/RTECrypto_EntropyPool.h"
#include "RunTime/RTE_UNIXAuthenticate.h"

#undef	spri
#define	spri				(void) sprintf

/* exported functions */

void    en01CheckForDBUmask ();  /* PTS 1106388 */
void    sqlinit ( tsp00_C64	component , tsp00_Bool *canceladdr );
void	sqlfinish ( tsp00_Bool	terminate );
void	sqlos ( UINT1 * os );
void	sqluid ( PROCESS_ID *aid );
void    sqlresult ( UINT1 result );

/* local functions */

/* also used in ven03.c */
      void en01terminateSelfBySignal(int sig);
      void en01replace_signal_handler(int sig, void (*sigHandler)(int), struct sigaction *pOldsa);
SAPDB_Bool en01restore_old_signal_handler(int sig, struct sigaction *pOldsa);

static struct sigaction sql01_oldSIGINT  = { 0 };
static  SIGNALFUNCTYPE sql01_catch_signal ();
static void en01assignStdFiledescriptors();

/* COMMON data */

	void                            ( * sql01_init_time ) () ;
	void                            ( * sql01_init_com ) () ;
	void                            ( * sql01_finish_com ) () ;

/* exported data */

int                  sql01_vt_clear = 0 ;
char				*sql01_dblang = NULL;
char				*sql01_dbname = NULL;
char				*sql01_username = NULL;
char				*sql01_opt_string = NULL;
char				sql01_component [ ALIGN_4BYTE_EO00(COMPSIZ+1) ] = { 0 };

/* local data */

/* PTS 1113410
  Initialization is protected by reference count en01_sqlinitCallCount
  and some initializazion must be done only in the very first sqlinit call.
  This is protected by the en01_sqlinitAlreadyCalled flag.
 */
static  int                             en01_sqlinitCallCount = 0;
static  int                             en01_isRunTimeComponent = 0;
static  int                             en01_sqlinitAlreadyCalled = 0;
static  UINT1                           sql01_return_value = 0;
static  SIGNALFUNCTYPE                  (* sql01_fetched_sigint)() = SIG_DFL ;
static  tsp00_Bool                       * sql01_cancel_address = NULL;

/*
 * ===========================================================================
 */
int sqlIsRunTimeComponent()
{
    return en01_isRunTimeComponent;
}

/*
 * ===========================================================================
 */
void sqlMarkAsRunTimeComponent()
{
    en01_isRunTimeComponent = 1;
}

/*
 * ===========================================================================
 */
void sqlMarkAsNonRunTimeComponent()
{
    en01_isRunTimeComponent = 0;
}

/*
 * ===========================================================================
 */

#undef MF__
#define MF__ MOD__"sqlinit"
void    sqlinit ( 
tsp00_C64	 	 component ,
tsp00_Bool 		*canceladdr )
{
    uid_t			uid ;
    struct passwd		*pwdp ;
    SIGNALFUNCTYPE              (*oldsigint)();
    char			msg [ 64 ];

    DBGIN;

    /* PTS 1113410 */
    ++en01_sqlinitCallCount;
    if ( en01_sqlinitCallCount > 1 )
    {
        DBGOUT;
	    return;
    }

    if ( 'R' == component[0] )
    {
        sqlMarkAsRunTimeComponent();
    }

    SAPDBErr_SetMessageOutputCall( en22MessageOutputCall );

    en01assignStdFiledescriptors();

    en01CheckForDBUmask ();
    eo46PtoC ( sql01_component , component , COMPSIZ );
    DBG1 (( MF__,"canceladdr 0x%08lx component '%s' \n",
			(long) canceladdr , sql01_component ))

    sql01_dblang = getenv ( "DBLOCALE" );
    if ( sql01_dblang == NULL )
      sql01_dblang = getenv ( "DBLANG" );
    
    DBG1 (( MF__,"DBLANG=%s\n", sql01_dblang ))
    sql01_dbname = getenv ( "SERVERDB" );
    DBG1 (( MF__,"SERVERDB=%s\n", sql01_dbname ))

    uid = geteuid ();
    if ( sql01_username ) FREE_MEM_EO57 ( sql01_username );

    {
        SAPDB_Char nameBuffer[256];
        SAPDB_UInt4 neededSize;

        if ( !RTE_GetUsernameFromPasswdById( (SAPDB_Int4)uid
                                          , nameBuffer
                                          , sizeof(nameBuffer)
                                          , &neededSize) )
        {
	        spri ( msg , MF__": cannot find user name for uid %ld !!!\r\n",
								        (long) uid );
	        (void) write ( 2 , msg , (unsigned) strlen(msg) );
	        exit ( 1 );
        }

        if ( ALLOC_MEM_EO57( (void **)&sql01_username, neededSize ) 
            != NO_ERROR_EO001 )
        {
	        spri ( msg , MF__"insufficient memory for user name !!!\r\n" );
	        (void) write ( 2 , msg , (unsigned) strlen(msg) );
	        exit ( 1 );
        }
        (void) strcpy ( sql01_username , nameBuffer );
        DBG1 (( MF__,"user name '%s'\n", sql01_username ))
    }

	initSystemEntropyPool( );

    sql01_cancel_address = canceladdr ;
    if ( sql01_cancel_address )
    {
        en01replace_signal_handler( SIGINT, sql01_catch_signal, &sql01_oldSIGINT );
    }

    sql01_return_value = 0 ;

    if ( 0 == en01_sqlinitAlreadyCalled )
    {
        en01_sqlinitAlreadyCalled = 1;
        if ( sql01_init_time ) sql01_init_time (); /* pointer to init function */
        if ( sql01_init_com  ) sql01_init_com  (); /* pointer to init function */
    }

    DBGOUT
} /* sqlinit */

/*
 * ===========================================================================
 */
#undef MF__
#define MF__ MOD__"sqlfinish"
void    sqlfinish ( 
tsp00_Bool	terminate )
{
    DBGIN

    /* PTS 1113981 */
    if ( en01_sqlinitCallCount <= 0 )
    {
        en01_sqlinitCallCount = 0;
        DBGOUT;
        return;
    }

    /* PTS 1113410 */
    --en01_sqlinitCallCount;
    if ( en01_sqlinitCallCount > 0 )
    {
        DBGOUT;
        return;
    }

    DBG1 (( MF__,"terminate %d \n", (int) terminate ))

    if ( sql01_finish_com ) sql01_finish_com (); /* pointer to finish funct */

    if ( sql01_cancel_address )
    {
        en01restore_old_signal_handler( SIGINT , &sql01_oldSIGINT );
    }

    if ( terminate )
    {
	DBG1 (( MF__,"EXITS  %d \n", sql01_return_value ));
	exit ( (int) sql01_return_value );
    }

    if ( sql01_opt_string ) 
    {
	free ( sql01_opt_string );
    }
    sql01_opt_string = 0 ;
    if ( sql01_username )
    {
    	FREE_MEM_EO57( sql01_username );
    }
    sql01_username = 0 ;

    DBGOUT
} /* sqlfinish */

/*
 * ===========================================================================
 */
#undef MF__
#define MF__ MOD__"sqlos"
void    sqlos ( 
UINT1                         * os )
{
    *os = os_unix ;
    DBG1 (( MF__,"sqlos: os %d \n", (int) *os ))
}

/*
 * ==================================================================
 */

#undef MF__
#define MF__ MOD__"sqluid"
void    sqluid ( 
PROCESS_ID                    *aid )
{
    DBGIN
    *aid = (PROCESS_ID) getpid ();
    DBG1 (( MF__,"aid %ld \n", (long) *aid ));
    DBGOUT
}

/*
 * ==================================================================
 */
#undef MF__
#define MF__ MOD__"sqlresult"
void    sqlresult ( 
UINT1			result )
{
    DBGIN
    DBG1 (( MF__,"result %d \n", (int) result ))
    sql01_return_value = result ;
    DBGOUT
}

/*
 * ==================================================================
 */
#undef MF__
#define MF__ MOD__"sqlsleep"
void    sqlsleep ( 
tsp00_Int2 limit )
{
    DBGIN
    DBG1 (( MF__,"sleep %u seconds \n", (unsigned) limit ));
    (void) sleep ( (unsigned) limit );
    DBGOUT 
}

/*==========================================================================*/

/* also used in ven03.c */
void en01terminateSelfBySignal(int sig)
{
   struct sigaction sa;

   sa.sa_handler = SIG_DFL;
   sigemptyset( &sa.sa_mask );
   sa.sa_flags = 0;
   sigaction(sig, &sa, 0);
   kill(getpid(), sig);
}

/* also used in ven03.c */
SAPDB_Bool en01restore_old_signal_handler(int sig, struct sigaction *pOldsa)
{
    if ( pOldsa->sa_handler
      && SIG_ERR != pOldsa->sa_handler
      && SIG_DFL != pOldsa->sa_handler )
    {
        sigaction(sig, pOldsa, NULL);
        pOldsa->sa_handler = SIG_ERR;
        return true;
    }
    return false;
}

/* also used in ven03.c */
void en01replace_signal_handler(int sig, void (*sigHandler)(int), struct sigaction *pOldsa)
{
    int rc;
    struct sigaction sa;
    struct sigaction saFound;

    sa.sa_handler = sigHandler;
    sigemptyset( &sa.sa_mask );
    sa.sa_flags = 0;
    rc = sigaction(sig, &sa, &saFound);

    if (  !pOldsa->sa_handler
       || ( saFound.sa_handler != SIG_DFL 
         && saFound.sa_handler != sigHandler ) )
    {
        if ( rc != 0 )
        {
            pOldsa->sa_handler = SIG_ERR;
        }
        else
        {
            SAPDB_memcpy(pOldsa, &saFound, sizeof(struct sigaction));
        }
    }
}

/*==========================================================================*/
 
#undef MF__
#define MF__ MOD__"sql01_catch_signal"
static  SIGNALFUNCTYPE  sql01_catch_signal ( int sig )
{
    DBGIN

    DBG1 (( MF__,"caught signal %d \n" , sig ))

    switch ( sig )
    {
    case SIGINT:
	    /*
	     *  If canceladdr was supplied with sqlinit,
	     *  set it now and don't exit.
	     */
	    if ( sql01_cancel_address )
	    {
	        *sql01_cancel_address = TRUE ;
	    }

        /*
         *  If the user supplied an interrupt handler,
         *  call it now and don't exit.
         */
        if ( sql01_oldSIGINT.sa_handler
          && sql01_oldSIGINT.sa_handler  != SIG_ERR
          && sql01_oldSIGINT.sa_handler  != SIG_DFL
          && sql01_oldSIGINT.sa_handler  != SIG_IGN
          && sql01_oldSIGINT.sa_handler  != sql01_catch_signal )
        {
            sql01_oldSIGINT.sa_handler ( sig );
        }
        else if ( !sql01_cancel_address )
        {
            en01terminateSelfBySignal(sig);
        }
    	break ;

    default:
	    break ;
    }

    DBGOUT 
} /* sql01_catch_signal */
 

/* PTS 1106388 */
#define DBUMASK "DBUMASK"
/*-----------------------------------------------------------*/
void en01CheckForDBUmask ()
/*-----------------------------------------------------------*/
{
   char *db_umask = getenv ( DBUMASK ) ;
   if ( db_umask != NULL ) 
   {
     int rc, new_umask ;
     sscanf(db_umask, "%o", &new_umask ) ;
     rc = umask ( new_umask ) ;
     DBG1 (( "\ncall_control : umask set to %o, old umask was %o\n", new_umask, rc ));
   }
  return ;
}

/*---------------------------------------------------------------------------*/
/* PTS 1112534 */
static void en01assignStdFiledescriptors()
{
  int fd;

  /* setup all standard file descriptor with '/dev/null'... */
  do {
    fd = open("/dev/null", O_RDWR);
  } while ( fd < 2 && fd >= 0 );

  /* if all had been defined before, we have an extra filedescriptor to close now */
  if ( fd > 2 )
  {
    close(fd);
  }
}
