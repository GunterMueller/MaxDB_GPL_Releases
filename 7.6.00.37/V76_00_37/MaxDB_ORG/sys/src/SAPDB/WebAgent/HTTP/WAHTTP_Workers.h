/*!**********************************************************************

  module: WAHTTP_Workers.h

  -----------------------------------------------------------------------

  responsible:	Tony Guepin

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

#ifndef WAHTTP_WORKERS_H
#define WAHTTP_WORKERS_H

#include "hwd01wadef.h"

typedef struct st_work *wahttp_worker_dummy1;

struct st_work {
    struct st_work  *next;
    int              stop;
    void            *ActualWork;
    void             (*DestroyActualWork)( void *, int );
};

int InitWorkers
(
 int MultiThreaded
);

int ExitWorkers
(
);

int MakeWorkers
(
 sapdbwa_UInt4	amount,
 void	*(*WorkerFunc)(void *)
);

int StartWorkers
(
);

typedef struct st_workq	*http_workers_dummy1;

int PutWork
(
 struct st_work		*work,
 struct st_workq	*wq
);

struct st_work *GetWork
(
 struct st_workq	*wq
);

struct st_work *MakeWork( void *(*MakeTcpConnection)( void ),
                          void  (*DestroyTcpConnection)( void *, int ));

void DestroyWork( struct st_work    *work,
                  int                persistentConnection );

#endif