/*!
  @file           vos68k.c
  @author         RaymondR
  @brief          internal console commands
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
#include "gos003.h"


/*
 *  DEFINES
 */
#define MOD__  "VOS68KC : "
#define MF__   MOD__"UNDEFINED"

/*
 *  MACROS
 */


/*
 *  LOCAL TYPE AND STRUCT DEFINITIONS
 */


typedef enum1                           PROCESS_STATE;
#define         PS_SESSION_WAIT         0
#define         PS_COMMAND_WAIT         1
#define         PS_IO_WAIT              2
#define         PS_SYSTEM_RESOURCE_WAIT 3
#define         PS_USER_LOCK_WAIT       4
#define         PS_REGION_WAIT          5
#define         PS_RUNNING              6
#define         PS_SLEEPING             7


typedef enum1                           DATABASE_STATE;
#define         COLD                    0
#define         WARM                    1

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

/*------------------------------*/

VOID xcancel ( PROCESS_ID  pid )
  {
  #undef  MF__
  #define MF__ MOD__"xcancel"
  PTASK_CTRL_REC   pTaskCtrl;

  DBGIN;

  if (( pid < 1) || ( pid > (PROCESS_ID)kgs.ulNumOfTasks ))
    {
    MSGCD (( ERR_XCANCEL_REQ_UNKNOWN_TSK, pid ));
    ABORT();
    }

  pTaskCtrl   = &kgs.pFirstTaskCtrl[pid - 1];

  if (( pTaskCtrl->TaskType == TT_UT_EO00 ) ||
      ( pTaskCtrl->TaskType == TT_EV_EO00 ) ||
      ( pTaskCtrl->TaskType == TT_US_EO00 ))
    {
    sql53k_comm_cancel( pTaskCtrl, commErrOk_esp01 );
    }


  DBGOUT;

  return;
  }

/*------------------------------*/

VOID xkill ( PROCESS_ID  pid )
  {
  #undef  MF__
  #define MF__ MOD__"xkill"
  PTASK_CTRL_REC                 pTaskCtrl;

  DBGIN;

  if (( pid < 1) || ( pid > (PROCESS_ID)kgs.ulNumOfTasks ))
    {
    MSGCD (( ERR_XCANCEL_REQ_UNKNOWN_TSK, pid ));
    ABORT();
    }

  pTaskCtrl   = &kgs.pFirstTaskCtrl[pid - 1];

  if (( pTaskCtrl->TaskType == TT_UT_EO00 ) ||
      ( pTaskCtrl->TaskType == TT_EV_EO00 ) ||
      ( pTaskCtrl->TaskType == TT_US_EO00 ))
    {
    sql53k_comm_cancel( pTaskCtrl, commErrOk_esp01 );
    }


  DBGOUT;

  return;
  }

/*------------------------------*/

VOID xio ( tsp2_devname   devspace,
           INT4           * size,
           INT4           * readcount,
           INT4           * writecount )
  {
  #undef  MF__
  #define MF__ MOD__"xio"

//  DBGMC; // --- Missing Code!!!
  DBGPAS;

  return;
  }

/*------------------------------*/

VOID xstorage ( INT4   * codesize,
                INT4   * datasize,
                INT4   * stacksize,
                INT4   * taskcount,
                INT4   * unused )
  {
  #undef  MF__
  #define MF__ MOD__"xstorage"

//  DBGMC; // --- Missing Code!!!
  DBGPAS;

  return;
  }

/*------------------------------*/

VOID xprocess ( PROCESS_ID        pid,
                PROCESS_ID        * uid,
                tsp2_process_type * ptype,
                PROCESS_STATE     * pstate,
                tsp00_Region      semaname,
                INT4              * timeout,
                INT4              * commandcount,
                INT4              * file_root,
                INT4              * file_record_cnt )
  {
  #undef  MF__
  #define MF__ MOD__"xprocess"

//  DBGMC; // --- Missing Code!!!
  DBGPAS;

  return;
  }

/*------------------------------*/

VOID xversion ( tsp00_Version     kernelversion,
                tsp00_Version     runtimeversion )
  {
  #undef  MF__
  #define MF__ MOD__"xversion"

//  DBGMC; // --- Missing Code!!!
  DBGPAS;

  return;
  }

/*------------------------------*/

VOID xregions ( tsp00_RegionId sid,
                tsp00_Region   semaname,
                INT4           * semcalls,
                INT4           * collisions )
  {
  #undef  MF__
  #define MF__ MOD__"xregions"

//  DBGMC; // --- Missing Code!!!
  DBGPAS;

  return;
  }

/*------------------------------*/

VOID  xstate ( DATABASE_STATE  * dbstate )
  {
  #undef  MF__
  #define MF__ MOD__"xstate"

//  DBGMC; // --- Missing Code!!!
  DBGPAS;

  return;
  }

/*
 * ========================== LOCAL FUNCTIONS =================================
 */



/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
