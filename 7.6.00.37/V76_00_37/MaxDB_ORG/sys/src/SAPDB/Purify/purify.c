/****************************************************************************

  module      : purify.c

  -------------------------------------------------------------------------

  responsible : HolgerB

  special area: purify 
  description : Trace client source info in purify protocol


  last changed: 2002-10-15  10:45
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2002-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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




*****************************************************************************/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDB/Purify/pure.h"
#include "gsp00.h"
#include "hsp77.h"

/*===========================================================================*
 *  PUBLIC FUNCTIONS                                                         *
 *===========================================================================*/

void  AbapInfoToPurify ( tsp00_C40 AbapInfo, tsp00_Int4 InfoLen, tsp00_Int4 lineNo )
{

    char line [100];
    
    sp77sprintf (line, sizeof (line), "abap at: %4d in: %.*s\n", 
        lineNo, InfoLen, AbapInfo);
    PurePrintf("%s", line);
  
    return;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
