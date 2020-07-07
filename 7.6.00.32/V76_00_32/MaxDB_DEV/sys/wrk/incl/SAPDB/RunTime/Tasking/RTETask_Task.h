/*!========================================================================

  @file         RTETask_Task.h
  @ingroup      Runtime
  @author       StefanP

  @brief        Task type names (short form)

                This header file holds the short names for the task types which are
                accessed by C functions (formely located in RTE_ConsoleStandardOutput.c)

  @since        2005-01-04  15:39
  @sa           

  ==========================================================================

  \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
 ============================================================================*/


#ifndef RTETASK_TASK_H
#define RTETASK_TASK_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "SAPDBCommon/SAPDB_Types.h"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define RTE_TASK_DISPLAY_SHORT_NAME_LENGTH      7   /* excluding Nullbyte */
#define RTE_TASK_DISPLAY_NAME_LENGTH            32  /* excluding Nullbyte */


#define RTE_TSK_TYPE_SHORTNAME_TI                   "Timer  "
#define RTE_TSK_TYPE_SHORTNAME_AL                   "Logwr  "
#define RTE_TSK_TYPE_SHORTNAME_TW                   "Tracewr"
#define RTE_TSK_TYPE_SHORTNAME_DW                   "Pager  "
#define RTE_TSK_TYPE_SHORTNAME_UT                   "Utility"
#define RTE_TSK_TYPE_SHORTNAME_SV                   "Server "
#define RTE_TSK_TYPE_SHORTNAME_US                   "User   "
#define RTE_TSK_TYPE_SHORTNAME_EV                   "Event  "
#define RTE_TSK_TYPE_SHORTNAME_GC                   "GarbCol"
#define RTE_TSK_TYPE_SHORTNAME_BUP                  "Backup "
#define RTE_TSK_TYPE_SHORTNAME_UNKNOWN              "UNKNOWN"

#define RTE_TSK_TYPE_NAME_TI                        "TIMEOUT         "
#define RTE_TSK_TYPE_NAME_AL                        "LOGWRITER       "
#define RTE_TSK_TYPE_NAME_TW                        "TRACEWRITER     "
#define RTE_TSK_TYPE_NAME_DW                        "PAGER           "
#define RTE_TSK_TYPE_NAME_UT                        "UTILITY         "
#define RTE_TSK_TYPE_NAME_SV                        "SERVER          "
#define RTE_TSK_TYPE_NAME_US                        "USER            "
#define RTE_TSK_TYPE_NAME_EV                        "EVENT           "
#define RTE_TSK_TYPE_NAME_GC                        "GARBAGECOLLECTOR"
#define RTE_TSK_TYPE_NAME_BUP                       "BACKUP          "
#define RTE_TSK_TYPE_NAME_UNKNOWN                   "UNKNOWN         "

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

typedef SAPDB_Char  RTE_TaskDisplayNameShort[RTE_TASK_DISPLAY_SHORT_NAME_LENGTH + 1];
typedef SAPDB_Char  RTE_TaskDisplayName     [RTE_TASK_DISPLAY_NAME_LENGTH + 1];


/*===========================================================================*
 *  PROTOTYPES....                                                           *
 *===========================================================================*/

#if !defined(externC) 
# if defined(__cplusplus)
#  define externC extern "C" 
# else
#  define externC
# endif
#endif

externC void RTETask_TaskMain(void);

#endif  /* RTETASK_TASK_H */
