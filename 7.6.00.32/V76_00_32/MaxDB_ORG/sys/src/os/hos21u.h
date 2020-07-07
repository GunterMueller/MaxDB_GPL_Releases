/*!
  @file           hos21u.h
  @author         RaymondR
  @brief          rte internal xuser access
  @see            

\if EMIT_LICENCE

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



\endif
*/




#ifndef HOS21U_H
#define HOS21U_H

// --- Crypt defines ----------------------------------------------------------

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"

typedef tsp00_C18       C18;
typedef tsp00_C20       C20;


#define MAX_CRYPT               6
#define MAXCRYPTNAMELENGTH      6
#define MAXCRYPTPWLENGTH        24
typedef tsp00_CryptName         CRYPTNAME ;


// --- VOS21 - ( CRYPT ) ------------------------------------------------------

void     sql21put_name       (   C18         acToCryptName,
                                 CRYPTNAME   alCryptName     );

void     sql21write_crypt    (   tsp00_Int4    lZahl,
                                 C20         acErgebnis      );

tsp00_Int4 sql21read_crypt     (   C20         acReCryptName   );

#endif
