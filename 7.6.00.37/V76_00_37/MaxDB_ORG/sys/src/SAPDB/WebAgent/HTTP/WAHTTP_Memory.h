/*!**********************************************************************

  module: WAHTTP_Memory.h

  -----------------------------------------------------------------------
  
  responsible:  Markus Oezgen

  special area: SAP DB WWW Web Server

  description:  Memory Management

  see also:

  -----------------------------------------------------------------------

  copyright:                Copyright (c) 2000-2005 SAP AG



    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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

#ifndef WAHTTP_MEMORY_H
#define WAHTTP_MEMORY_H

#include "geo102.h"
#include "hwd01wadef.h"

/* To enable memory leak searching */
#define wahttp_MEMCHECK

#ifndef wahttp_MEMCHECK
#define wahttp_SQLMALLOCINIT(p)
#define wahttp_SQLALLOCAT(s, p, ok)	sqlallocat(s, (sapdbwa_UInt1 **)p, ok)
#define wahttp_SQLREALLOCAT(s, p, ok)	*((sapdbwa_UInt1 **)p)?sqlreallocat(s, (sapdbwa_UInt1 **)p, ok):sqlallocat(s, (sapdbwa_UInt1 **)p, ok)
#define wahttp_SQLFREE(p)		sqlfree((sapdbwa_UInt1 *)p)
#else
#define wahttp_SQLMALLOCINIT(p)		wahttp_mallocinit((sapdbwa_UInt1 *)p)
#define wahttp_SQLALLOCAT(s, p, ok)	wahttp_allocat(s, (sapdbwa_UInt1 **)p, ok, __FILE__, __LINE__)
#define wahttp_SQLREALLOCAT(s, p, ok)	wahttp_reallocat(s, (sapdbwa_UInt1 **)p, ok, __FILE__, __LINE__)
#define wahttp_SQLFREE(p)		wahttp_free((sapdbwa_UInt1 *)p, __FILE__, __LINE__)
#endif

void wahttp_mallocinit(tsp00_BoolAddr	pbOk
);

void wahttp_allocat( sapdbwa_Int4	lSize,
		     sapdbwa_UInt1	**ppucObjPtr,
		     tsp00_BoolAddr	pbOk,
		     char		*FILE,
		     long		LINE
);
    
void wahttp_reallocat( sapdbwa_Int4     lSize,
					   sapdbwa_UInt1  **ppucObjPtr,
		     tsp00_BoolAddr	pbOk,
		     char		*FILE,
		     long		LINE
);

void wahttp_free( sapdbwa_UInt1*  pucObjPtr,
		     char		*FILE,
		     long		LINE
);


#endif    
