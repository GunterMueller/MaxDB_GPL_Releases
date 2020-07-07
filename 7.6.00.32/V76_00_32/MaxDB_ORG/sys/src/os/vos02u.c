/*!
  @file           vos02u.c
  @author         RaymondR
  @brief          Exithandler
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


//
//  DEFINES
//

#define MOD__  "VOS02UC : "
#define MF__   MOD__"UNDEFINED"

#define MAXEXIT 10


//
//  MACROS
//


//
//  LOCAL TYPE AND STRUCT DEFINITIONS
//


//
//  EXPORTED VARIABLES
//
#if defined ( _WIN32 )
 static PFNEXITLIST sql02u_ExitCallBack[MAXEXIT];
#endif


//
//  LOCAL VARIABLES
//


//
//  FUNCTION PROTOTYPES
//


//
//  LOCAL FUNCTION PROTOTYPES
//


//
// ========================== GLOBAL FUNCTIONS ================================
//

APIRET sql02u_add_exit_list ( PFNEXITLIST pExitFunc )
  {
  #undef  MF__
  #define MF__ MOD__"sql02u_add_exit_list"
  APIRET rc = NO_ERROR;
  #if defined ( _WIN32 )
   INT i;
  #endif

  DBGPAS;

  #if defined ( _WIN32 )
   for (i = 0; i < MAXEXIT; ++i)
     {
     if (!sql02u_ExitCallBack[i]) break;
     }

   if (i >= MAXEXIT)
     {
     DBG1((MF__, "Exit callback list overflow!"));
     return (ERROR_OUT_OF_STRUCTURES);
     }

   sql02u_ExitCallBack[i] = pExitFunc;
  #else
    rc = DosExitList ( EXLST_ADD, pExitFunc );
  #endif

  return ( rc );
  }

/*------------------------------*/

APIRET sql02u_next_exit_func ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sql02u_next_exit_func"
  APIRET rc = NO_ERROR;

  DBGPAS;

  #if defined ( _WIN32 )
  #else
    rc = DosExitList ( EXLST_EXIT, NULL );
  #endif

  return ( rc );
  }

/*------------------------------*/

#if defined ( _WIN32 )
 VOID sql02u_run_exit_func ( VOID )
   {
   #undef  MF__
   #define MF__ MOD__"sql02u_run_exit_func"
   int    i;

   for ( i = MAXEXIT-1; i >= 0; --i)
     {
     if (sql02u_ExitCallBack[i])
       (*sql02u_ExitCallBack[i])( 0 );
     }
   }

#endif

//
// ========================== LOCAL FUNCTIONS =================================
//

//
// =============================== END ========================================
//
