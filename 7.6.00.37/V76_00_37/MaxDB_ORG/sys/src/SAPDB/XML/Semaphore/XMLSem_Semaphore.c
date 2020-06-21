/*!**********************************************************************

  module: XMLSem_Semaphore.c

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: XML Semaphore

  description:  Semaphore management

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

#include "SAPDB/XML/Memory/XMLMem_Memory.h"
#include "SAPDB/XML/Semaphore/XMLSem_Semaphore.h"

/*---------------------------------------------------------------------------*/

/****************************************************************************
 ***
 *** Types & Structs
 ***
 ****************************************************************************/

struct st_xml_semaphore {
    teo07_ThreadSemaphore	semaphore;
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

SAPDB_Bool Semaphore_Create( XMLSem_Semaphore *hSemaphore )
{

    tsp00_ErrTextc	   errorText;
    teo07_ThreadErr	   error;
    tsp00_Bool		   bOk;
    XMLSem_Semaphore   hNewSemaphore = NULL;

    XMLMem_Allocat(sizeof(struct st_xml_semaphore), &hNewSemaphore, &bOk);
    if (!bOk) {
        return SAPDB_FALSE;
    }

    memset(hNewSemaphore, 0, sizeof(struct st_xml_semaphore));

    sqlcreatesem(&hNewSemaphore->semaphore, 0, errorText, &error);
    if (error != THR_OK_EO07) {
        XMLMem_Free(hNewSemaphore);

        return SAPDB_FALSE;
    }

    *hSemaphore = hNewSemaphore;

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool Semaphore_Destroy( XMLSem_Semaphore hSemaphore )
{

    sqldestroysem(hSemaphore->semaphore);

    XMLMem_Free(hSemaphore);

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool Semaphore_Begin( XMLSem_Semaphore hSemaphore )
{

    sqlwaitsem(hSemaphore->semaphore);

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool Semaphore_End( XMLSem_Semaphore hSemaphore )
{

    sqlsignalsem(hSemaphore->semaphore);

    return SAPDB_TRUE;

}

/***********************************************************************

	Private Functions

 ***********************************************************************/

/*===========================================================================*/

/***********************************************************************

	End

 ***********************************************************************/
