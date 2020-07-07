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

#ifndef HWD27EXCL_H
#define HWD27EXCL_H

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "gwd00.h"
#include "hwd26err.h"

/*!**********************************************************************

  Chapter: twd27ExclP

  description:
  
************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct st_wa_excl	*twd27ExclP;

tsp00_Bool wd27InitMultithreading( twd26ErrP err );

tsp00_Bool wd27InitExcl( twd27ExclP *excl );

tsp00_Bool wd27BegExcl( twd27ExclP  excl );

tsp00_Bool wd27EndExcl( twd27ExclP  excl );

void wd27ExitExcl( twd27ExclP  excl );

#if defined(__cplusplus)
}
#endif

/*!**********************************************************************

  EndChapter: twd27ExclP

************************************************************************/
#endif

