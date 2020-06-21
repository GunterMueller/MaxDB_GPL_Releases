/*!**********************************************************************

  module: hwd27excl.h

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  Semaphores

  see also:

  -----------------------------------------------------------------------

                         Copyright (c) 1998-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "heo07.h"
#include "hwd27excl.h"

/*!**********************************************************************

  Chapter: twd27ExclP

************************************************************************/

struct st_wa_excl
{
    teo07_Mutex hMutex;
}; /* struct st_wa_excl */


sapdbwa_Bool wd27InitMultithreading( twd26ErrP err )
{
#ifndef CGI_ONLY
    teo07_Thread    thisThread;
    tsp00_ErrText   errText;
    teo07_ThreadErr ok = true;
    
    sqlinit_multithreading( &thisThread, (char*) errText, &ok );
    if (!ok) {
        wd26SetErr( err, ERR_INIT_MT_WD26, (char*) errText, NULL );
    }; /* if */
    return ok;
#else
    return true;
#endif
} /* wd27InitMultithreading */


/* Initialize exclusive access for threads to a resource.
 * Count: Number of threads that may access the resource before blocking.
 *    If 0 (zero), timeout is INFINITE.
 * Timeout: Millisecs the thread will be blocked when blocking applies.
 */
sapdbwa_Bool wd27InitExcl( twd27ExclP *excl )
{
#ifndef CGI_ONLY
    tsp00_Bool allocatOK;
    sapdbwa_SQLALLOCAT( sizeof(struct st_wa_excl), (sapdbwa_UInt1**) excl, &allocatOK );
    if (!allocatOK)
        {
            return(false);
        }
    memset(*excl, 0, sizeof(**excl));

    sqlcreatemutex(&(*excl)->hMutex);

    if (!(*excl)->hMutex)
        {
            free((*excl));
            return(false);
        }
    return(true);
#else
    return(true);
#endif
} /* wd27InitExcl */


/* Try decrement the semaphore (block if zero, otherwise decrement */
sapdbwa_Bool wd27BegExcl( twd27ExclP excl )
{
#ifndef CGI_ONLY
    sqlbeginmutex(&excl->hMutex);
    return(true);
#else
    return(true);
#endif
} /* wd27BegExcl */


sapdbwa_Bool wd27EndExcl( twd27ExclP excl )
{
#ifndef CGI_ONLY
    sqlendmutex(&excl->hMutex);
    sqlyieldthread();   /* cause Bill cancels the lock, but does not
                         * always transfer control */
    return(true);
#else
    return(true);
#endif
} /* wd27EndExcl */


void wd27ExitExcl( twd27ExclP excl )
{
#ifndef CGI_ONLY
    sqldestroymutex(&excl->hMutex);
    sapdbwa_SQLFREE((sapdbwa_UInt1*) excl);
#else
    return;
#endif
} /* wd27ExitExcl */

/*!**********************************************************************

  EndChapter: twd27ExclP

************************************************************************/
