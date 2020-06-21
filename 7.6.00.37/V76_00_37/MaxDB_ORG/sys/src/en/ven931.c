/*!
  @file           ven931.c
  @author         JoergM
  @brief          Client RunTime: Clear a kernel
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

#define MOD__ "ven931.c:"

#include "gen00.h"
#include "geo007_1.h"
#include "hen40.h"
#include "gen41.h"

extern	void				sqlinit ( const char *component, char *ptr );

/* local functions */

static  int                 remove_processes ( char *dbname );

/* exported data */

char                      * dbname ;


/*==========================================================================*/

#undef MF__
#define MF__ MOD__"main"
int c_main ( int argc , char *argv[] )
{
int                             rc ;

	DBGIN;
	sqlinit ( "clear_ps_ipc" , (char*)0 );
/*
 *  File permission mode is always specified absolutely
 */
    /* PTS 1105151 */
    (void) umask ( 007 );

	if ( argc != 2 )
    {
		(void) fprintf ( stderr, "USAGE: clear dbname \n", 0 );
		return ( 1 );
    }

	dbname = argv [ 1 ] ;

	rc = remove_processes ( dbname );

	if ( rc == 0 )
	{
		(void) sleep ( 1 );
		rc = sql41_remove_ipc_resources ( dbname );
	}

	DBG1 (( MF__,"returning %d \n", rc ));
	DBGOUT;
	return ( rc );
}


/*==========================================================================*/


static  int     remove_processes ( char * dbname )
{
    int                             rc ;
    tsp00_TaskId                    dbPid;

    if ( en41GetPPIDFromFile(dbname, &dbPid) != 0 )
	{
		MSGCD (( -11987, ERR_TYPE, "CLEAR   ", "Failed to get database watchdog pid %s\n", sqlerrs() ));
        return ( 1 );
    }
    if ( dbPid != 0 )
    {
        do
        {
            rc = kill ( (pid_t) dbPid , 0 ) ;
            if ( 0 == rc )
            {
                rc = kill( (pid_t) dbPid, SIGKILL );
                if ( rc != 0 && errno != EINTR && errno != ESRCH )
                {
            		MSGCD (( -11987, WRN_TYPE, "CLEAR   ", "Failed to kill database watchdog pid %ld error %s\n",
                                (long)dbPid, sqlerrs() ));
                    break;
                }
                sleep(0);
                rc = kill ( (pid_t) dbPid , 0 ) ;
            }
        } while ( rc == 0 || errno != ESRCH );
    }

    if ( en41GetPIDFromFile(dbname, &dbPid) != 0 )
	{
        MSGCD (( -11987, ERR_TYPE, "CLEAR   ", "Failed to get database pid error %s\n", sqlerrs() ));
        return ( 1 );
    }
    if ( dbPid != 0 )
    {
        do
        {
            rc = kill ( (pid_t) dbPid , 0 ) ;
            if ( 0 == rc )
            {
                rc = kill( (pid_t) dbPid, SIGKILL );
                if ( rc != 0 && errno != EINTR && errno != ESRCH )
                {
            		MSGCD (( -11987, WRN_TYPE, "CLEAR   ", "Failed to kill database pid %ld error %s\n",
                                (long)dbPid, sqlerrs() ));
                    break;
                }
                sleep(0);
                rc = kill ( (pid_t) dbPid , 0 ) ;
            }
        } while ( rc == 0 || errno != ESRCH );
    }

	return ( 0 );
}
