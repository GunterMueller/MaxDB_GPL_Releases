/*!**********************************************************************

  module: WAHTTP_Workers.c

  -----------------------------------------------------------------------

  responsible:  Tony Guepin

  special area: SAP DB WWW

  description:  Worker Thread Management

  see also:

  -----------------------------------------------------------------------

                         Copyright (c) 2001-2005 SAP AG



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





************************************************************************/

#define MODULE_NAME "WAHTTP_Workers.c"

/****************************************************************************
 ***
 *** Include
 ***
 ****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
/*
#include <signal.h>
*/
#endif

#include "SAPDB/WebAgent/HTTP/WAHTTP_Misc.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_Log.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_Connect.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_Workers.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_Memory.h"

#include "heo07.h"

static Excl	WorkListExcl;
static Sem	WorkWaitSem;

static struct st_work	*WorkList = NULL, *WorkListLast = NULL;
static teo07_Thread	*WorkerTids = NULL;
static sapdbwa_UInt4	NoOfWorkers = 0;
static int MULTI_THREADED = FALSE;

/****************************************************************************
 ***
 *** InitWorkers:
 ***
 *** Initialize the work queue
 ***
 ****************************************************************************/

int InitWorkers( int MultiThreaded )
{
 char		errtxt[MAX_ERRORTEXT_LG];

 if (!initexcl(&WorkListExcl, sizeof(errtxt), errtxt))
 {
	Log_Error(errtxt, -1);
	return(FALSE);
 }

 if (!initsem(&WorkWaitSem, sizeof(errtxt), errtxt))
 {
	exitexcl(WorkListExcl);
	Log_Error(errtxt, -1);
	return(FALSE);
 }

 WorkList = NULL;
 WorkListLast = NULL;

 MULTI_THREADED = MultiThreaded;

 return(TRUE);

}

/****************************************************************************
 ***
 *** ExitWorkers:
 ***
 *** Free resources of the work queue
 ***
 ****************************************************************************/

int ExitWorkers()
{

	struct st_work	*w, *wnext;

	begexcl(WorkListExcl);

    for (w=WorkList, wnext=w->next; w; w=wnext) {
		DestroyWork(w, FALSE);
    }

	endexcl(WorkListExcl);

	exitexcl(WorkListExcl);
	exitsem(WorkWaitSem);


	WorkList = NULL;
	WorkListLast = NULL;

	return(TRUE);

}

/****************************************************************************
 ***
 *** MakeWorkers:
 ***
 *** 
 ***
 ****************************************************************************/

static  void *(*Worker)(void *);

int MakeWorkers( sapdbwa_UInt4	 amount,
		 void	*(*WorkerFunc)(void *) )
{

 unsigned long		i;
 tsp00_ErrTextc		errtext;
 teo07_ThreadErr		err;
 tsp00_Bool			ok;

 NoOfWorkers = amount;
 Worker = WorkerFunc;

 if (!MULTI_THREADED)
	return(TRUE);

 wahttp_SQLALLOCAT ( NoOfWorkers * sizeof(teo07_Thread), &WorkerTids, &ok );
 if (!ok) {
	/* memory alloc failure	*/
	Log_Error("MakeWorkers: Memory allocation error!", -1);
	return(FALSE);
 };

 if (!WorkerTids)
	return(FALSE);

 for (i=0; i<NoOfWorkers; i++) {
	sqlbeginthread( WORKER_STACK_WAHTTP,
			Worker, 
			0, 
			THR_CREATE_SUSPENDED_EO07, 
			&WorkerTids[i],
			errtext, 
			&err);

	if (err != THR_OK_EO07)
	{
		Log_Error("MakeWorkers: Could not beginthread", -1);
		NoOfWorkers = 0;
		wahttp_SQLFREE( (sapdbwa_UInt1*) WorkerTids );
		return(FALSE);
	}
 }
 return(TRUE);
}

int StartWorkers /* was MakeWorkers */
(
)
{
 unsigned long	i;
 tsp00_ErrTextc		errtext;
 teo07_ThreadErr	err;

 if (!MULTI_THREADED)
	return(TRUE);
/*
 WorkerTids = malloc(NoOfWorkers * sizeof(unsigned long));
*/
 if (!WorkerTids)
	return(FALSE);

 for (i=0; i<NoOfWorkers; i++)
 {
	sqlresumethread(WorkerTids[i], errtext, &err);
	if (err != THR_OK_EO07)
	{
		Log_Error("StartWorkers: Could not sqlresumethread", -1);
		NoOfWorkers = 0;
        wahttp_SQLFREE( (sapdbwa_UInt1*) WorkerTids );
		return(FALSE);
	}
 }

 return(TRUE);
}

/*========================================================================*/

struct st_work *MakeWork( void *(*MakeTcpConnection)( void ),
                          void  (*DestroyTcpConnection)( void *, int ))
{
	struct st_work *work;
	tsp00_Bool      ok;
 
	wahttp_SQLALLOCAT ( sizeof(*work), &work, &ok );
    if (!ok) {
        /* memory alloc failure	*/
		Log_Error("MakeWork: Memory allocation error!", -1);
        return(FALSE);
    };

    if (work) {
        memset(work, 0, sizeof(struct st_work));
    }

    work->ActualWork = MakeTcpConnection();
    if (!work->ActualWork) {
        wahttp_SQLFREE( (sapdbwa_UInt1*) work );
        return(NULL);
    } else {
        work->DestroyActualWork = DestroyTcpConnection;
        return(work);
    }

}

/*========================================================================*/

void DestroyWork( struct st_work    *work,
                  int                persistentConnection )
{

    if (work->DestroyActualWork) {
	    work->DestroyActualWork(work->ActualWork, persistentConnection);
    }

	wahttp_SQLFREE( (sapdbwa_UInt1*) work );

}

/*========================================================================*/

struct st_work *GetWork
(
 struct st_workq	*wq
)
{
 struct st_work *work;

 /* Wait for work */
 begsem(WorkWaitSem);

/* Acquire List controll */
 begexcl(WorkListExcl);

 if (WorkList)
 {
	work = WorkList;
	WorkList = WorkList->next;
	work->next = NULL; 
	if (!WorkList)
		WorkListLast = NULL;

 }
 else
 /*	There wasn't any work, are we shutting down ?
  */
	work = NULL;

/* Give up List controll */
 endexcl(WorkListExcl);

 return(work);
}

int PutWork
(
 struct st_work		*work,
 struct st_workq	*wq
)
{
/* Acquire List controll */
 begexcl(WorkListExcl);

 work->next = NULL;

 if (WorkListLast)
 {
	WorkListLast->next = work;
	WorkListLast = work;
 }
 else
 {
	WorkList = work;
	WorkListLast = work;
 }

 endsem(WorkWaitSem);

/* Give up List controll */
 endexcl(WorkListExcl);

 return(TRUE);
}

