/*!**********************************************************************

  module: XMLLock_Lock.c

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: XML Lock

  description:  Lock management

  see also:

  -----------------------------------------------------------------------

                         Copyright (c) 2003-2005 SAP AG



    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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

/****************************************************************************
 ***
 *** Include
 ***
 ****************************************************************************/

#include "heo07.h"

/*---------------------------------------------------------------------------*/

#include "SAPDB/SAPDBCommon/SAPDB_Types.h"

/*---------------------------------------------------------------------------*/

#include "SAPDB/XML/Lock/XMLLock_Lock.h"
#include "SAPDB/XML/Memory/XMLMem_Memory.h"

/*---------------------------------------------------------------------------*/

/****************************************************************************
 ***
 *** Types & Structs
 ***
 ****************************************************************************/

struct st_xmllock_lock {
    teo07_Mutex lock;
};

/****************************************************************************
 ***
 *** Prototypes
 ***
 ****************************************************************************/

/****************************************************************************
 ***
 *** Variables
 ***
 ****************************************************************************/

/***********************************************************************

	Public Functions

 ***********************************************************************/

SAPDB_Bool Lock_Create( XMLLock_Lock *hLock )
{

    tsp00_Bool  bOk;
    XMLLock_Lock hNewLock = NULL;

    XMLMem_Allocat(sizeof(struct st_xmllock_lock), &hNewLock, &bOk);
    if (!bOk) {
        return SAPDB_FALSE;
    }

    memset(hNewLock, 0, sizeof(struct st_xmllock_lock));

    sqlcreatemutex(&hNewLock->lock);
    if (!hNewLock->lock) {
        XMLMem_Free(hNewLock);

        return SAPDB_FALSE;
    }

    *hLock = hNewLock;

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool Lock_Destroy( XMLLock_Lock hLock )
{

    sqldestroymutex(&hLock->lock);
    XMLMem_Free(hLock);

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool Lock_Begin( XMLLock_Lock hLock)
{
    /* Try decrement the semaphore (block if zero, otherwise decrement */
    sqlbeginmutex(&hLock->lock);

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool Lock_End( XMLLock_Lock hLock)
{

    sqlendmutex(&hLock->lock);

    /* Cause Windows cancels the lock, but does not always transfer control */
    sqlyieldthread();

    return SAPDB_TRUE;

}

/***********************************************************************

	Private Functions

 ***********************************************************************/

/*===========================================================================*/

/***********************************************************************

	End

 ***********************************************************************/
