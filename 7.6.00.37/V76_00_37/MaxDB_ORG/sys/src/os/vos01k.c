/*!
  @file           vos01k.c
  @author         RaymondR
  @brief          Devlopment_functions
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
//#include "gos00.h"
#include "gos41.h"
#include "heo00.h"
#include "heo46.h"
#include "geo007_1.h"
#include "gos00k.h"


/*
 *  DEFINES
 */
#define MOD__  "VOS01KC : "
#define MF__   MOD__"UNDEFINED"

#define SEM_HALT              "SAP DBTech-HALT-"


/*
 *  MACROS
 */


/*
 *  LOCAL TYPE AND STRUCT DEFINITIONS
 */


/*
 * EXTERNAL VARIABLES
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

/*------------------------------*/

VOID sqlk01_view_stack_page_flags ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sqlk01_view_stack_page_flags"

  return;
  }

/*------------------------------*/

VOID sql01k_halt ( PSZ pszServerDB )
  {
  #undef  MF__
  #define MF__ MOD__"sql01k_halt"
  SECURITY_ATTRIBUTES                   SA;
  CHAR                                  SDBuf[SECURITY_DESCRIPTOR_MIN_LENGTH];
  HEV                                    hevSem;
  APIRET                                 rc;
  PSECURITY_ATTRIBUTES                   pSA = NULL;

  DBGPAS;

  if ( !InitializeSecurityDescriptor( (PSECURITY_DESCRIPTOR) SDBuf,
                                       SECURITY_DESCRIPTOR_REVISION )    ||
       !SetSecurityDescriptorDacl   ( (PSECURITY_DESCRIPTOR) SDBuf,
                                       TRUE, (PACL) NULL, FALSE ) )
  {
    return;
  }

  SA.nLength              = sizeof(SA);
  SA.lpSecurityDescriptor = (PSECURITY_DESCRIPTOR) SDBuf;
  SA.bInheritHandle       = FALSE;
  pSA                     = &SA;

  rc = sql41c_create_event_sem ( &hevSem, SEM_HALT, pszServerDB,
                                 0, FALSE, pSA );

  if (rc != NO_ERROR)
    return;

  sql41c_wait_event_sem ( hevSem, (ULONG)UNDEF, "HALT" );

  sql41c_close_event_sem ( hevSem, "HALT" );

  return;
  }

/*
 * ========================== LOCAL FUNCTIONS =================================
 */


/*
 * =============================== END ========================================
 */
