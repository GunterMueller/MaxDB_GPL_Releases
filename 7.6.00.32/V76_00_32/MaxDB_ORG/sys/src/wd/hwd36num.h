/*!**********************************************************************

  module: hwd36num.h

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  Get "exclusive" numbers.

  see also:

  -----------------------------------------------------------------------

  copyright:              Copyright (c) 1999-2005 SAP AG



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




************************************************************************/

#ifndef HWD36NUM_H
#define HWD36NUM_H

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "gwd00.h"

/*!**********************************************************************

  Chapter: twd36NumPoolP

  description:
  
************************************************************************/

typedef struct st_wa_num_pool *twd36NumPoolP;

twd36NumPoolP wd36CreateNumPool( sapdbwa_Int4	maxNum );

void wd36DestroyNumPool( twd36NumPoolP	pool );

sapdbwa_Bool wd36GetNumber( twd36NumPoolP	 pool,
							sapdbwa_Int4	*id		);

sapdbwa_Bool wd36PutNumber( twd36NumPoolP	pool,
							sapdbwa_Int4	id		);

/*!**********************************************************************

  EndChapter: twd36NumPoolP

************************************************************************/
#endif
