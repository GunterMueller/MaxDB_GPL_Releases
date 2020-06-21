/*!
  @file           ven45.c
  @author         JoergM
  @brief          RunTime: kernel_kill_and_info
  @see            


  Kill Kernel Processes and Remove Ipc Resources
                Kill Kernel Tasks
                Fetch Communication Info

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

#define MOD__ "ven45.c:"
#include "gen00.h"
#include "heo00.h"
#ifdef OUTSIDE_KERNEL
#include "geo002.h" /* nocheck */
#endif /* OUTSIDE_KERNEL */
#include "gen50.h"
#include "vsp001c.h"
#include "gen003.h"
#include "gen41.h"
#include "gen45.h"
#ifndef OUTSIDE_KERNEL
#include "gen500.h" /* nocheck */
#endif /* !OUTSIDE_KERNEL */
#include "heo07.h"
#include "gen41.h"
#include "hsp77.h"

/* local functions */

/*
 * ===========================================================================
 */
#undef MF__
#define MF__ MOD__"sql45_killall"
externC void sql45_killall ( 
struct KERNEL_GLOBAL_SECTION    * lkgs )
{
	pid_t           		mypid ;
	tsp00_Pathc             path;
    tsp00_DbNamec           dbname ;

	DBGIN;

	if ( lkgs->state != SERVER_STOP )
	{
	    lkgs->state = SERVER_KILL ;
	}

    SAPDB_memcpy(dbname, lkgs->serverdb, sizeof(tsp00_DbNamec));

	/*
	 *  Kill some rte processes.
	 */
	mypid = getpid ();
	if ( mypid != lkgs->pid )
    {
        if ( mypid != lkgs->parentPid )
        {
            if ( kill ( lkgs->parentPid, 0 ) == 0 )
            {
                kill( lkgs->pid, SIGKILL );
          		/*
	        	 *  allow the parent process to do the cleanup
		         */
                while ( kill ( lkgs->parentPid, 0 ) == 0 )
                {
        	    	(void) sleep ( 1 );
                }
                DBG1 (( MF__,"Cleanup done by kernel himself\n" ));
        	    DBGOUT;
                return;
            }
        }
        else
        {
            kill( lkgs->pid, SIGKILL );
        }
	}

    sql41_remove_ipc_resources(dbname);

    DBG1 (( MF__,"All resources removed \n" ));
	DBGOUT;
}

/*
 * ===========================================================================
 */
/*
 * ===========================================================================
 */

externC	void	sql45_kill_task ( 
struct TASK_TYPE	*tcb )
{
    int  rc ;
    union semun     arg ;

	if ( tcb->connection )
    {
        tcb->connection->ci_state = CON_ABORTED ;
    }
    tcb->rte_comm.to_cancel = 1 ;
   /* PTS 1113593 Inform UKT about change in communication state */
    arg.val = 1 ;
    do {
      rc = semctl ( tcb->ukt->semid , 0 , SETVAL , SEMCTL_SETVAL(arg) );
    } while ( rc && errno == EINTR );
}

/*
 * ===========================================================================
 */

externC	int	sql45_has_remote_pid ( 
struct TASK_TYPE	*tcb )
{
    if ( tcb->connection 
      && *(tcb->connection->ci_remote_pid) )
    	return ( TRUE );
    else
	    return ( FALSE );
}

/*
 * ===========================================================================
 */

externC	char	*sql45_peer_pid ( 
struct TASK_TYPE	*tcb ,
char                *peer_pid_buffer)
{
    char			*ptr ;

    if ( ! tcb->connection )
    {
        return ( "-1" ) ;
    }

    if ( *(tcb->connection->ci_remote_pid) )
    {
    	ptr = (char *)tcb->connection->ci_remote_pid ;
    }
    else
    {
    	ptr = peer_pid_buffer;
	    (void) sp77sprintf ( ptr , 32, "%ld" , (long)
		    	tcb->connection->ci_peer_pid );
    }

    return ( ptr );
}

/*
 * ===========================================================================
 */

externC	char *sql45_peer_node ( 
struct TASK_TYPE	*tcb )
{
    if ( tcb->connection )
      return ( (char *)tcb->connection->ci_peer_node );
    else
      return ( "" ) ;
}
