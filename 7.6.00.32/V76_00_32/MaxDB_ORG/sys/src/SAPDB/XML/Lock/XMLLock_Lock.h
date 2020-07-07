/*!**********************************************************************

  module: XMLLock_Lock.h

  -----------------------------------------------------------------------

  responsible:	Markus Oezgen

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

#ifndef XMLLOCK_LOCK_H
#define XMLLOCK_LOCK_H

/***********************************************************************

	Includes

 ***********************************************************************/

#include "SAPDB/SAPDBCommon/SAPDB_Types.h"

/***********************************************************************

	Defines

 ***********************************************************************/

/***********************************************************************

	Structs & Types

 ***********************************************************************/

typedef struct st_xmllock_lock      *XMLLock_Lock;

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Prototypes

 ***********************************************************************/

SAPDB_Bool Lock_Create( XMLLock_Lock *hLock );

SAPDB_Bool Lock_Destroy( XMLLock_Lock hLock );

SAPDB_Bool Lock_Begin( XMLLock_Lock hLock);

SAPDB_Bool Lock_End( XMLLock_Lock hLock);

/***********************************************************************

	End

 ***********************************************************************/

#endif
