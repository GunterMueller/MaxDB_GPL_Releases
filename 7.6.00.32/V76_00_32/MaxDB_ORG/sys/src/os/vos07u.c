/*!
  @file           vos07u.c
  @author         RaymondR
  @brief          User-DLL exit
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
#include "geo572.h"


//
//  DEFINES
//

#define MOD__  "VOS07UC : "
#define MF__   MOD__"UNDEFINED"



//
//  MACROS
//


//
//  LOCAL TYPE AND STRUCT DEFINITIONS
//


//
//  EXPORTED VARIABLES
//


//
//  LOCAL VARIABLES
//


//
//  FUNCTION PROTOTYPES
//
#if defined ( _WIN32 )
 BOOL WINAPI _CRT_INIT ( HINSTANCE  hInstDll,
                         DWORD      fdwReason,
                         LPVOID     lpvReserved );
#endif

//
//  LOCAL FUNCTION PROTOTYPES
//


//
// ========================== GLOBAL FUNCTIONS ================================
//


#if defined ( _WIN32 )

 BOOL WINAPI sql07u_DLLExit ( HINSTANCE  hInstDll,
                              DWORD      fdwReason,
                              LPVOID     lpvReserved )
   {
   #undef  MF__
   #define MF__ MOD__"sql07u_DLLExit"

   DBGPAS;
   DBG3((MF__, "Reason = %d", fdwReason));

   // --- Initialize the C run_time first
   if ((fdwReason == DLL_PROCESS_ATTACH) || (fdwReason == DLL_THREAD_ATTACH))
     {
     sqlSetModuleHandle (hInstDll);

     if ( !_CRT_INIT(hInstDll, fdwReason, lpvReserved ) )
       return ( FALSE );
     }

   if (fdwReason == DLL_PROCESS_DETACH)
     {
     sql02u_run_exit_func ();
     sql40c_free_dll();
     sql02_release_resources();
     sql60_release_resources();
     eo572_DestroyAllocPool();
     }

   // --- Terminate the C run_time after all is done
   if ((fdwReason == DLL_PROCESS_DETACH) || (fdwReason == DLL_THREAD_DETACH))
     {
     if ( !_CRT_INIT(hInstDll, fdwReason, lpvReserved ) )
       return ( FALSE );
     }

   return (TRUE);
   }

#endif

//
// ========================== LOCAL FUNCTIONS =================================
//

//
// =============================== END ========================================
//
