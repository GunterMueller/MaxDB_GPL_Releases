/* @lastChanged: "2004-13-12  14:12"
 * @filename:    heo131x.h
 * @purpose:     "C-Types/Prototypes for XUSER File C++ Module"
 * @release:     7.6.0.4
 * @see:         ""
 *
 * @Copyright (c) 1998-2005 SAP AG"


\if EMIT_LICENCE

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



\endif

 */


#ifndef HEO131X_H
#define HEO131X_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "vsp004.h"
#include "SAPDBCommon/SAPDB_string.h"

/*===========================================================================*
 *  TYPES                                                      *
 *===========================================================================*/

#ifdef __cplusplus
    #define externC      extern "C"
#else
    #define externC
#endif


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/
externC void buildServernodeURI( struct tsp4_xuser_record *paramUserRecord,
                                SAPDB_Char *    EncryptionType);


#endif  /* HEO131X_H */
