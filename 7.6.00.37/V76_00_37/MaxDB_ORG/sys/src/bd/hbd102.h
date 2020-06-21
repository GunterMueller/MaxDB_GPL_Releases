
/*******************************************************************************

  module:       hbd102.h

  ------------------------------------------------------------------------------

  responsible:  JuergenP

  purpose:      filesysteminterface_1

  lastChanged:  1999-09-15

  generatedBy:  $TOOL/bin/ptocProto.py

  ------------------------------------------------------------------------------

  copyright:    Copyright (c) 1999-2005 SAP AG



    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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


********************************************************************************/

// PTS 1103980 JA 1999-09-15

#ifndef HBD102_H
#define HBD102_H


/*===========================================================================*
 *     RefType: C++                                                          *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\3711\261\226\010\002e\301\321J@Z\334\331\275\327'
 *===========================================================================*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "gbd00.h"

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/


/* !! note: REC_ALIGN_BD00 has to be a power of 2 !! */

# if REC_ALIGN_BD00 == 2

inline tsp00_Int4 bd102RecAlign (const tsp00_Int4 len)
{
    return ((len+1)&~1);
}

#else

inline tsp00_Int4 bd102RecAlign (const tsp00_Int4 len)
{
	if (len)
		return ((((len-1) & ~(REC_ALIGN_BD00-1)) + REC_ALIGN_BD00);
	else
		return 0;
}

#endif

#endif

