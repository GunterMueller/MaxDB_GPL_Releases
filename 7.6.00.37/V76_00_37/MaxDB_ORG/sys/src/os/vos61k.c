/*!
  @file           vos61k.c
  @author         RaymondR
  @brief          X_START/X_STOP posting
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




/*
 * INCLUDE FILES
 */
#include "gos00.h"
#include "heo00.h"
#include "heo46.h"
#include "geo007_1.h"
#include "gos00k.h"


/*
 *  DEFINES
 */
#define MOD__  "VOS61KC : "
#define MF__   MOD__"UNDEFINED"


/*
 *  MACROS
 */


/*
 *  LOCAL TYPE AND STRUCT DEFINITIONS
 */


/*
 *  EXPORTED VARIABLES
 */


/*
 * LOCAL VARIABLES
 */


/*
 * LOCAL FUNCTION PROTOTYPES
 */


/*
 * ========================== GLOBAL FUNCTIONS ================================
 */

VOID sql61k_post_xstart_sem ( PSZ pszServerDB, BOOL fCrash )
  {
  #undef  MF__
  #define MF__ MOD__"sql61k_post_xstart_sem"
  HEV         hevStartSem  = 0;
  HEV         hevChrashSem  = 0;
  PATHNAME    szSemName;

  DBGPAS;

  #if defined(_WIN32)
   if ( fCrash )
     {
     strcpy (szSemName, SEM_XSTART_CRASH );
     strcat (szSemName, pszServerDB);

     hevChrashSem = OpenEvent(EVENT_ALL_ACCESS, FALSE, szSemName);

     if ( hevChrashSem )
       {
       SetEvent    ( hevChrashSem );
       CloseHandle ( hevChrashSem );
       }
     }

   strcpy (szSemName, SEM_XSTART );
   strcat (szSemName, pszServerDB);

   hevStartSem = OpenEvent(EVENT_ALL_ACCESS, FALSE, szSemName);

   if ( hevStartSem )
     {
     SetEvent    ( hevStartSem );
     CloseHandle ( hevStartSem );
     }

  #else
   if ( fCrash )
     {
     strcpy (szSemName, SEM_XSTART_CRASH );
     strcat (szSemName, pszServerDB);

     if ( DosOpenEventSem(szSemName, &hevChrashSem ) == NO_ERROR )
       {
       DosPostEventSem(hevChrashSem);
       DosCloseEventSem(hevChrashSem);
       }
     }

   strcpy (szSemName, SEM_XSTART );
   strcat (szSemName, pszServerDB);

   if ( DosOpenEventSem(szSemName, &hevStartSem ) == NO_ERROR )
     {
     DosPostEventSem(hevStartSem);
     DosCloseEventSem(hevStartSem);
     }
  #endif

  return;
  }


/*------------------------------*/

VOID sql61k_post_xstop_sem ( PSZ pszServerDB )
  {
  #undef  MF__
  #define MF__ MOD__"sql61k_post_xstop_sem"
  HEV         hevStopSem  = 0;
  PATHNAME    szSemName;

  DBGPAS;

  strcpy (szSemName, SEM_XSTOP );
  strcat (szSemName, pszServerDB);

  #if defined(_WIN32)
   hevStopSem = OpenEvent(EVENT_ALL_ACCESS, FALSE, szSemName);

   if ( hevStopSem )
     {
     SetEvent    ( hevStopSem );
     CloseHandle ( hevStopSem );
     }
  #else
   if ( DosOpenEventSem(szSemName, &hevStopSem ) == NO_ERROR )
     {
     DosPostEventSem(hevStopSem);
     DosCloseEventSem(hevStopSem);
     }
  #endif

  return;
  }


/*
 * ========================== LOCAL FUNCTIONS =================================
 */

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
