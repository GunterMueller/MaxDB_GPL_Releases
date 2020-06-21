/*!**********************************************************************

  module: XMLId_Id.h

  -----------------------------------------------------------------------

  responsible:	Markus Oezgen

  special area: XML Id Handling

  description:  Id

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

#ifndef XMLID_ID_H
#define XMLID_ID_H

/***********************************************************************

	Includes

 ***********************************************************************/

#include "SAPDB/SAPDBCommon/SAPDB_Types.h"

/***********************************************************************

	Defines

 ***********************************************************************/

#define XMLID_MAX_ID_LEN                24
#define XMLID_MAX_ID_STRING_LEN         2*XMLID_MAX_ID_LEN
#define XMLID_MAX_ID_DEBUG_STRING_LEN   52  /* See also RTE_UniqueIdBuffer in RTE_UniqueId.h */

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Structs & Types

 ***********************************************************************/

typedef char	XMLId_Id[XMLID_MAX_ID_LEN];
typedef char	XMLId_IdString[XMLID_MAX_ID_STRING_LEN+1];

typedef char	XMLId_IdDebugString[XMLID_MAX_ID_DEBUG_STRING_LEN+1];

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Prototypes

 ***********************************************************************/

SAPDB_Bool Id_Init();

void Id_StringAsId( XMLId_IdString idString,
                    XMLId_Id       id );

SAPDB_Bool Id_IsIdentical( XMLId_Id id1,
                           XMLId_Id id2 );

/***********************************************************************

	End

 ***********************************************************************/

#endif
