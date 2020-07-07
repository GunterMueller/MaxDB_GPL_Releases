/*!***************************************************************************

  module      : RTE_KSS.h

  -------------------------------------------------------------------------

  responsible : StefanP

  special area: Kernel Shared Section
  description : Structure definition
  
  last changed: 2001-04-03  16:25
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG


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




*****************************************************************************/
#ifndef RTE_KSS_H
#define RTE_KSS_H


#ifndef _WIN32

/* NT only */

#else

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include    "gsp100.h" 
#include    "RunTime/RTE_Types.h"
#include    "KernelCommon/Kernel_SharedState.h"
/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/
/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

typedef struct kernel_shared_seg
  {
  RTE_Version                    RTEVersion;
  RTE_Version                    KernelVersion;
  tsp100_VersionID2              RTEVersionID;

  BOOLEAN                        fDump;                 // - set by XSTOP
  BOOLEAN                        gracefully;            // - set by XSTOP
  PID                            KernelPid;
  BOOL                           fIsGateway;

  SAPDB_UInt4                    consoleShmSize;
  SAPDB_UInt4                    consoleSemTimeout;

  SAPDB_UInt4                    dBState;         // - Database state

  /* PTS 1129710 */
  SAPDB_UInt4                    kernelStateSize;        // - Size of struct Kernel_SharedState
  SAPDB_ULong                    pendingUpdateCounter;   // - Changed for every pending update on kernelState content
  SAPDB_ULong                    completedUpdateCounter; // - Changed for every completed update on kernelState content
  struct Kernel_SharedState      kernelState;            // - Extended kernel state 
  } KERNEL_SHARED_SEG;

#endif /* _WIN32 */
#endif  /* RTE_KSS_H */
