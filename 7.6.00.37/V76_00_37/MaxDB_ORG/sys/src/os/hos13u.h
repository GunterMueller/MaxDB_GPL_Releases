/*!
  @file           hos13u.h
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


#ifndef HOS13U_H
#define HOS13U_H

#include "gos00.h"
#include "hos21u.h"

#define MAXARGLINELENGTH        132

typedef char                    C4    [  4  ];
typedef char                    C12   [ 12  ];
typedef char                    C40   [ 40  ];
typedef char                    C132C [ 133 ];

typedef char                    argline [ MAXARGLINELENGTH ];


// --- SQLUSER ----------------------------------------------------------------

#define DEFAULT_USER            "DEFAULT           "
#define INFO_XUSER_PAGE         0
#define EOF_XUSER_PAGE          -99
#define MX_XUSER_ENTRIES        32
#define INFO_OK                 0
#define INFO_ILLEGAL_USER       1
#define INFO_ILLEGAL_COMPONENT  2


#define         is_ascii                0
#define         is_ebcdic               1
#define         is_codeneutral          2

// --- VOS13UC - ( Xuser ) ----------------------------------------------------

APIRET sql13u_get_options_char_set_map ( PBOOL               fEnabled,
                                         PSZ                 pszCharSetName,
                                         LONG                lMxCharSetName );

APIRET sql13u_get_options_sqlopt       ( PBOOL               pfEnabled,
                                         PSZ                 pszSQLOpt,
                                         LONG                lMxSQLOpt );
#endif
