/*!
  @file           gos94.h
  @author         RaymondR
  @brief          service_entry
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



#ifndef GOS94_H
#define GOS94_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gos00.h"
#include "gsp09.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

APIRET          sql94_service_entry          ( ULONG             ulRequest,
                                               ULONG             ulKernelType,
                                               BOOL              fAutoStart,
                                               PSZ               pszNodeName,
                                               PSZ               pszDBRoot,
                                               PSZ               pszServerDB,
											   PSZ               pszAccountName,
                                               PSZ               pszPassword,
				  							   PSZ               pszParameter,
											   PSZ               pszValue,
                                               PSZ               pszPgmName,
                                               tsp09_rte_xerror *xerror);


/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



#endif  /* GOS94_H */
