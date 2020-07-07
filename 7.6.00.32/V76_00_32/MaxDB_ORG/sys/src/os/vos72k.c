/*!
  @file           vos72k.c
  @author         RaymondR
  @brief          wake up
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
#define MOD__  "VOS72KC : "
#define MF__   MOD__"UNDEFINED"

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

VOID  sql72k_wake_kt ( PTHREAD_CTRL_HEADER_REC pThrdCtrlHd )
  {
  #undef  MF__
  #define MF__ MOD__"sql72k_wake_kt"
  APIRET  rc = NO_ERROR;

  DBGPAS;


  if ( !SetEvent( (HANDLE)pThrdCtrlHd->hevSem ))
      rc = GetLastError();

  if (( rc                != NO_ERROR )             &&
      ( rc                != ERROR_ALREADY_POSTED ) &&
      ( *kgs.pDBState     < SERVER_SHUTDOWN ))
    {
    MSGD (( ERR_POSTING_KT_SEM, pThrdCtrlHd->Tid, rc ));
    DBG1 (( MF__, "INTERNAL ERROR: on posting semaphore of KT:%u, rc = %d",
            pThrdCtrlHd->Tid, rc ));
    ABORT();
    }
  }

/*------------------------------*/

VOID  sql72k_wake_ukt ( PUKT_CTRL_REC pUKTCtrl )
  {
  #undef  MF__
  #define MF__ MOD__"sql72k_wake_ukt"
  APIRET  rc = NO_ERROR;

  DBGPAS;

  // Since load balancing a NULL pointer is allowed because 'pTaskCtrl->pUKT'
  // is allowed to be NULL. We have nothing to do here!
  if ( pUKTCtrl == NULL )
    return;

  if ( !SetEvent( (HANDLE)pUKTCtrl->ThrdCtrlHeader.hevSem ))
      rc = GetLastError();

  if (( rc              != NO_ERROR )             &&
      ( rc              != ERROR_ALREADY_POSTED ) &&
      ( *kgs.pDBState   != SERVER_SHUTDOWNREINIT) &&
      ( *kgs.pDBState   != SERVER_SHUTDOWNKILL  ))
  {
      MSGD (( ERR_POSTING_UKT_SEM, pUKTCtrl->ulUKTIndex, rc ));
      DBG1 (( MF__, "INTERNAL ERROR: on posting semaphore of UKT:%u, rc = %d",
          pUKTCtrl->ulUKTIndex, rc ));
      ABORT();
  }
  DBG4 (( MF__, "Waked up UKT:%d", pUKTCtrl->ulUKTIndex ));
  }

/*
 * ========================== LOCAL FUNCTIONS =================================
 */


/*
 * =============================== END ========================================
 */
