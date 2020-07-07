/*!
  @file           ven72.cpp
  @author         JoergM
  @brief          Kernel RunTime: Wake a Kernel Thread
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

#define MOD__ "ven72.cpp:"
#include "gen00.h"
#include "heo00.h"
#include "geo50_0.h"
#include "gen500.h"
#include "hen50.h"
#include "gen72.h"
#include "hen40.h"

/*
 * ===========================================================================
 */

#undef MF__
#define MF__ MOD__"e72_wake"
extern "C" void e72_wake ( ten50_UKT_Control *kt )
{
  union semun     arg ;
  int  rc ;
  
  if ( kt->fSemAlreadyPosted == FALSE )
  {
    kt->fSemAlreadyPosted = TRUE; /* --- it prevents the semaphore
    to be posted more than once */

    DBG1(( MF__,"Thread T%ld State %ld Semid %d\n", 
      (long)kt->tid, (long)kt->state, kt->semid ));

    arg.val = 1 ;
    do {
      rc = semctl ( kt->semid , 0 , SETVAL , SEMCTL_SETVAL(arg) );
    } while ( rc && errno == EINTR );
    if ( rc )
    {
      if ( errno != ERANGE && KGS->state != SERVER_SHUTDOWNKILL )
        /*JH frickel frickel */
      {
        MSGALL (( ERR_WAKE_SEMSND_ERR, kt->semid , sqlerrs() ));
        vabort ( WRITE_CORE ) ;
      }
    }
  }
  else
  {
    DBG1(( MF__,"Thread T%ld already running\n", (long)kt->tid ));
  }
}

/*
* ===========================================================================
*/

/* PTS 1001075 */

#undef MF__
#define MF__ MOD__"e72_wake_ignore_semop_err"
extern "C" void e72_wake_ignore_semop_err ( ten50_UKT_Control *kt )
{
  union semun     arg ;
  int  rc ;
  
  if ( kt->fSemAlreadyPosted == FALSE )
  {
    kt->fSemAlreadyPosted = TRUE; /* --- it prevents the semaphore
    to be posted more than once */
    DBG1(( MF__,"Thread T%ld State %ld Semid %d\n", 
      (long)kt->tid, (long)kt->state, kt->semid ));
  
    arg.val = 1 ;
    do {
      rc = semctl ( kt->semid , 0 , SETVAL , SEMCTL_SETVAL(arg) );
    } while ( rc && errno == EINTR );
    if ( rc )
    {
      if ( errno != ERANGE && KGS->state != SERVER_SHUTDOWNKILL )
        /*JH frickel frickel */
      {
        MSGCD (( WRN_WAKE_IGNORING_ERROR, kt->semid, sqlerrs() ));
      }
    }
  }
  else
  {
    DBG1(( MF__,"Thread T%ld already running\n", (long)kt->tid ));
  }
}
