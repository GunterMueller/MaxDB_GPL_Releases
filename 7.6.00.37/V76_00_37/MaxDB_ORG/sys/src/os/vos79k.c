/*!
  @file           vos79k.c
  @author         RaymondR
  @brief          read param file
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
#include "geo002.h"
#include "heo11.h"
#include "geo002.h"
#include "gos44.h"

#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.h"

/*
 *  DEFINES
 */
#define MOD__  "VOS79KC : "
#define MF__   MOD__"UNDEFINED"

// --- default values

/*
 *  MACROS
 */


/*
 *  LOCAL TYPE AND STRUCT DEFINITIONS
 */


/*
 * EXPORTED VARIABLES
 */


/*
 * LOCAL VARIABLES
 */

/*
 * LOCAL FUNCTION PROTOTYPES
 */
static  LONG  sql79k_check_params  ( VOID );

/*
 * ========================== GLOBAL FUNCTIONS ================================
 */

LONG  sql79k_read_params (VOID)
  {
  #undef  MF__
  #define MF__ MOD__"sql79k_read_params"

  DBGIN;

  // --- allocate memory for RTE_XPARAM_REC 

  if ( ALLOC_MEM ((PPVOID)&(kgs.XParam), sizeof(RTE_XPARAM_REC) ) != NO_ERROR)
    {
    DBGOUT;
    return ( 1 );
    }

  if ( ! RTEConf_ReadAndAnalyzeParamWithOutput ( kgs.szServerDB, kgs.XParam ) ) 
  {
//    MSGALL (( ERR_PARAMETER_INCONSISTENCY, errText )) ;
    return ( -1 ) ;
  }
  if ( sql79k_check_params() ) return ( -1 );
  
  DBGOUT;
  return ( NO_ERROR );
  }

/*
 * ========================== LOCAL FUNCTIONS =================================
 */

static LONG  sql79k_check_params (void)
  {
  #undef  MF__
  #define MF__ MOD__"sql79k_check_params"

  DBGIN;

&ifdef PE
  if ( XPARAM(lMaxActive) > MAX_ACTIVE )
    {
    MSGALL(( ERR_XP_ENTRYXX_OUT_OF_RANGE,
             PAN_MAXACTIVE, XPARAM(lMaxActive), 0, MAX_ACTIVE ));
    DBG3  (( MF__, "%s (%d) out of range (%d to %d)",
             PAN_MAXACTIVE, XPARAM(lMaxActive), 0, MAX_ACTIVE ));
    return ( -1 );
    }
&endif

  if ((XPARAM(lDeviPerDevspace) > MAX_DEVI_P_DEVSPACE) ||
      (XPARAM(lDeviPerDevspace) < 1))
    {
    MSGALL(( ERR_XP_ENTRYXX_OUT_OF_RANGE,
             PAN_MAXIOTHREADS, XPARAM(lDeviPerDevspace), 1,
             MAX_DEVI_P_DEVSPACE ));
    DBG3 ((  MF__, "%s (%d) out of range (%d to %d)",
             PAN_MAXIOTHREADS, XPARAM(lDeviPerDevspace), 1,
             MAX_DEVI_P_DEVSPACE ));
    return ( -1 );
    }

  if ( XPARAM(ulDiagSize) < MINDIAGFILESIZE )
    {
    MSGALL(( ERR_XP_ENTRYXX_TOO_SMALL,
             PAN_DIAG_SIZE, XPARAM(ulDiagSize), MINDIAGFILESIZE ));
    DBG3  (( MF__, "%s too small (%d), minimum (%d)",
             PAN_DIAG_SIZE, XPARAM(ulDiagSize), MINDIAGFILESIZE ));
    return ( -1 );
    }

  if ( XPARAM(ulMaxUsUtStack) < ( 20 KB ))
    XPARAM(ulMaxUsUtStack) = 100 * 1024;

  if ( XPARAM(lDevQueue) < 0 )
    {
    MSGALL(( ERR_XP_ENTRYXX_TOO_SMALL,
             PAN_DEV_QUEUE, XPARAM(ulDiagSize), 0 ));
    DBG3  (( MF__, "%s too small (%d), minimum (%d)",
             PAN_DEV_QUEUE, XPARAM(ulDiagSize), 0 ));
    return ( -1 );
    }


  if (sql44c_get_dev_type_by_filename (XPARAM(szTraceDevspace)) == DT_DRIVE)
    {
    MSGALL(( ERR_XP_ENTRY_INVALID, PAN_KERNELTRACEFILE, XPARAM(szTraceDevspace) ));
    DBG1  ( (MF__, "XParam file entry '%s' is invalid ( %s ).", PAN_KERNELTRACEFILE, XPARAM(szTraceDevspace) ));
    return ( -1 );
    }

  if ( XPARAM(ulMaxCPU) < 1 )
    {
    MSGALL(( ERR_XP_ENTRYXX_TOO_SMALL,
             PAN_MAXCPU, XPARAM(ulMaxCPU), 1 ));
    DBG3  (( MF__, "%s too small (%d), minimum (%d)",
             PAN_MAXCPU, XPARAM(ulMaxCPU), 1 ));
    return ( -1 );
    }


  if ( XPARAM(ulRegionCollisionLoop) > MAX_REG_COLL_LOOPS)
    {
    MSGALL(( ERR_XP_ENTRYXX_OUT_OF_RANGE,
             PAN_MAXRGN_REQUEST , XPARAM(ulRegionCollisionLoop), 0,
             MAX_REG_COLL_LOOPS));
    DBG3  (( MF__, "%s (%d) out of range (%d to %d)",
             PAN_MAXRGN_REQUEST , XPARAM(ulRegionCollisionLoop), 0,
             MAX_REG_COLL_LOOPS ));
    return ( -1 );
    }

  DBGOUT;

  return (NO_ERROR);
  }

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
