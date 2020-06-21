/*!
  @file           vos17u.c
  @author         RaymondR
  @brief          User Utility Functions
  @see            

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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




//
//  INCLUDE FILES
//
#include "gos00.h"
#include "heo46.h"
#include "gos003.h"


//
//  DEFINES
//

#define MOD__  "VOS17UC : "
#define MF__   MOD__"UNDEFINED"


//
//  MACROS
//


//
//  LOCAL TYPE AND STRUCT DEFINITIONS
//


//
//  EXTERNAL VARIABLES
//


//
//  EXPORTED VARIABLES
//


//
//  LOCAL VARIABLES
//
static  CHAR                      acNodeSeparatorList [] = NODESEPARATORLIST;


//
//  LOCAL FUNCTION PROTOTYPES
//


//
// ========================== GLOBAL FUNCTIONS ================================
//

//
// --- For compatibility with Release 2
//
VOID sql17u_split_dbname   ( SQL_DBNAMEC acServerDB,
                             SQL_NODEIDC acServerNode )
  {
  #undef  MF__
  #define MF__ MOD__"sql17u_split_dbname"

  INT     nIndex,
          nNodeLength;
  PCHAR   pcNodeSeparator;

  DBGPAS;

  DBG3 (( MF__, "acServerDB '%s'", acServerDB ));

  pcNodeSeparator = NULL;

  for ( nIndex = 0; acNodeSeparatorList [ nIndex ]; nIndex++ )
    {
    pcNodeSeparator = strchr ( acServerDB, acNodeSeparatorList [ nIndex ] );

    if ( pcNodeSeparator != NULL )
      break;
    }

  if ( pcNodeSeparator != NULL )
    {
    DBG3 (( MF__, "Node separator '%c'", *pcNodeSeparator ));

    nNodeLength = (INT)(pcNodeSeparator - acServerDB);
    SAPDB_memcpy ( acServerNode, acServerDB, nNodeLength );
    acServerNode [ nNodeLength ] = '\0';

    for ( pcNodeSeparator++; *pcNodeSeparator; pcNodeSeparator++ )
        *( acServerDB++ ) = *pcNodeSeparator;


    *acServerDB = '\0';
    }

  return;
  }

//
// ========================== LOCAL FUNCTIONS =================================
//


//
// =============================== END ========================================
//
