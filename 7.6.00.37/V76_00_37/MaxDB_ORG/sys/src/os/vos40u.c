/*!
  @file           vos40u.c
  @author         RaymondR
  @brief          Abort and Exit Functions
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
#include "geo007_1.h"
#include "geo007_2.h"


//
//  DEFINES
//

#define MOD__  "VOS40UC : "
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


//
//  LOCAL FUNCTION PROTOTYPES
//


//
// ========================== GLOBAL FUNCTIONS ================================
//


VOID    sqlabort (  VOID    )
{
    #undef  MF__
    #define MF__ MOD__"sqlabort"
    PID     pid;


    DBGPAS;

#if defined(WINTERM)
    win07p_abort ();
#endif
    GETPROCESSID(&pid);

    MSGD(( ERR_SQLABORT, pid ));

    EXITPROCESS( -1 );
}


//
// ========================== LOCAL FUNCTIONS =================================
//


//
// =============================== END ========================================
//
