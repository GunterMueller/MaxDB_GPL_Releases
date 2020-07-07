/*!**********************************************************************

  module: XMLSem_Semaphore.h

  -----------------------------------------------------------------------

  responsible:	Markus Oezgen

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

#ifndef XMLSEM_SEMAPHORE_H
#define XMLSEM_SEMAPHORE_H

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

typedef struct st_xml_semaphore     *XMLSem_Semaphore;

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Prototypes

 ***********************************************************************/

SAPDB_Bool Semaphore_Create( XMLSem_Semaphore *hSemaphore );

SAPDB_Bool Semaphore_Destroy( XMLSem_Semaphore hSemaphore );

SAPDB_Bool Semaphore_Begin( XMLSem_Semaphore hSemaphore );

SAPDB_Bool Semaphore_End( XMLSem_Semaphore hSemaphore );

/***********************************************************************

	End

 ***********************************************************************/

#endif
