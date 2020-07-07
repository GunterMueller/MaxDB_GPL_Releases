/*!
  @file           vos18u.c
  @author         RaymondR
  @brief          character set mapping
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
// INCLUDE FILES
//
#include "gos00.h"
#include "heo46.h"
#include "vsp004.h"
#include "geo13.h"
#include "hos13u.h"


//
//  DEFINES
//

#define MOD__  "VOS18UC : "
#define MF__   MOD__"UNDEFINED"


//
//  MACROS
//


//
//  LOCAL TYPE AND STRUCT DEFINITIONS
//


//
// EXTERNAL VARIABLES
//


//
//  EXPORTED VARIABLES
//


//
// LOCAL VARIABLES
//


//
// LOCAL FUNCTION PROTOTYPES
//

//
// ========================== GLOBAL FUNCTIONS ================================
//


VOID sqlcharsetname ( tsp00_KnlIdentifier acCharSetName )
  {
  #undef  MF__
  #define MF__ MOD__"sqlcharsetname"
  PSZ             pszCharSetName;
  #if defined ( _WIN32 )
   CHAR           szCharSetName[sizeof(tsp00_KnlIdentifier) + 1];
   BOOL           fEnabled;
   APIRET         rc;
  #endif

  DBGIN;

  memset( acCharSetName, ' ', sizeof (tsp00_KnlIdentifier));

  if( ( pszCharSetName = getenv ( CHAR_SET_ENV_VAR )) != NULL )
    {
    DBG3 (( MF__, "%s = '%s' ", CHAR_SET_ENV_VAR, pszCharSetName));
    eo46CtoP ( acCharSetName, pszCharSetName, sizeof(tsp00_KnlIdentifier) );
    DBGOUT;
    return;
    }

  #if defined ( _WIN32 )
   rc = sql13u_get_options_char_set_map ( &fEnabled, szCharSetName,
                                          sizeof(tsp00_KnlIdentifier) );
   if (( rc == NO_ERROR ) && ( fEnabled ))
     {
     eo46CtoP ( acCharSetName, szCharSetName, sizeof(tsp00_KnlIdentifier) );
     DBGOUT;
     return;
     }
  #endif


  DBGOUT;
  return;
  }

//
// ========================== LOCAL FUNCTIONS =================================
//


//
// =============================== END ========================================
//
