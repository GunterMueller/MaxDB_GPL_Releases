/*!
  @file           geo70k.h
  @author         JoergM
  @special area   RTE
  @brief          Task Cluster analysis
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


#ifndef GEO70K_H
#define GEO70K_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "geo002.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define VAR_TASK_BUP_EO70K	0
#define VAR_TASK_DW_EO70K	1
#define VAR_TASK_US_EO70K	2
#define VAR_TASK_SV_EO70K	3
#define VAR_TASK_EV_EO70K	4
#define VAR_TASK_GC_EO70K	5

#define MX_VAR_TASK_TYPES_EO70K	6

#define TASK_TOKEN_TIMER_EO70K	     "TI"
#define TASK_TOKEN_ALOGWR_EO70K	     "AL"
#define TASK_TOKEN_TRACEWR_EO70K     "TW"
#define TASK_TOKEN_UTILITY_EO70K     "UT"
#define TASK_TOKEN_DATAWRITER_EO70K  "DW"
#define TASK_TOKEN_USER_EO70K	     "US"
#define TASK_TOKEN_SERVER_EO70K	     "SV"
#define TASK_TOKEN_EVENT_EO70K	     "EV"
#define TASK_TOKEN_GARBAGEC_EO70K    "GC"
#define TASK_TOKEN_BACKUP_EO70K      "BUP"

#define TOKEN_COMPRESS_EO70K         "COMPRESS"
#define TOKEN_EQUALIZE_EO70K         "EQUALIZE"
#define TOKEN_ALLINONE_EO70K         "ALLINONE"
#define TOKEN_ONETOONE_EO70K         "ONETOONE"

#if defined(_WIN64)

# define FIX_US_UT_STACK_SIZE_EO70K      (1024 * 1024)  /* -- Utility, User Kernel Task */
# define FIX_AL_STACK_SIZE_EO70K         (300 * 1024)   /* -- Archivelogwriter */
# define FIX_TW_STACK_SIZE_EO70K         (300 * 1024)   /* -- Tracewriter */
# define FIX_TI_STACK_SIZE_EO70K         (300 * 1024)   /* -- Timeout */
# define FIX_DW_STACK_SIZE_EO70K         (300 * 1024)   /* -- Datawriter */
# define FIX_SV_STACK_SIZE_EO70K         (300 * 1024)   /* -- Server */
# define FIX_EV_STACK_SIZE_EO70K         (300 * 1024)   /* -- EventTask */
# define FIX_GC_STACK_SIZE_EO70K         (300 * 1024)   /* -- GarbageCollector */
# define FIX_BUP_STACK_SIZE_EO70K        (300 * 1024)   /* -- BackupTask */

  /* PTS 1106437 guardpages now always added !!! (veo70.c) */
# define NUM_GUARDPAGES_EO70          2

#else /* UNIX or WIN32 */

# if defined(_WIN32)

#  define FIX_US_UT_STACK_SIZE_EO70K      (450 * 1024)  /* -- Utility, User Kernel Task */
   /* PTS 1106437 guardpages now always added !!! (veo70.c) */
#  define NUM_GUARDPAGES_EO70          2

# else

#  define FIX_US_STACK_SIZE_EO70K         (450 * 1024)  /* User Kernel Task */
   /* PTS 1106437 guardpages now always added !!! (veo70.c) */
#  define NUM_GUARDPAGES_EO70          1

# endif

# if defined (HP_IA64) || (defined(LINUX)&&defined(IA64))
#  define FIX_US_UT_STACK_SIZE_EO70K      (1024 * 1024) /* -- Utility, User Kernel Task */
#  define FIX_AL_STACK_SIZE_EO70K         (512  * 1024) /* -- Archivelogwriter */
#  define FIX_TW_STACK_SIZE_EO70K         (512  * 1024) /* -- Tracewriter */
#  define FIX_TI_STACK_SIZE_EO70K         (512  * 1024) /* -- Timeout */
#  define FIX_DW_STACK_SIZE_EO70K         (512  * 1024) /* -- Datawriter */
#  define FIX_SV_STACK_SIZE_EO70K         (512  * 1024) /* -- Server */
#  define FIX_EV_STACK_SIZE_EO70K         (512  * 1024) /* -- EventTask */
#  define FIX_GC_STACK_SIZE_EO70K         (512  * 1024) /* -- GarbageCollector */
#  define FIX_BUP_STACK_SIZE_EO70K        (512  * 1024) /* -- BackupTask */
# else
#  define FIX_AL_STACK_SIZE_EO70K         (100 * 1024)  /* -- Archivelogwriter */
#  define FIX_TW_STACK_SIZE_EO70K         (100 * 1024)  /* -- Tracewriter */
#  define FIX_TI_STACK_SIZE_EO70K         (100 * 1024)  /* -- Timeout */
#  define FIX_DW_STACK_SIZE_EO70K         (100 * 1024)  /* -- Datawriter */
#  define FIX_SV_STACK_SIZE_EO70K         (100 * 1024)  /* -- Server */
#  define FIX_EV_STACK_SIZE_EO70K         (100 * 1024)  /* -- EventTask */
#  define FIX_GC_STACK_SIZE_EO70K         (100 * 1024)  /* -- GarbageCollector */
#  define FIX_BUP_STACK_SIZE_EO70K        (100 * 1024)  /* -- BackupTask */
# endif

#endif  /* UNIX or WIN32 */


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

typedef struct teo70k_ThrdInfo {
  tsp00_Longuint          ulStackSize;
  tsp00_Int4              tracePages;
  tsp00_Int4              lSpecialTasks;
  tsp00_Int4              lUserTasks;
  tsp00_Int4              lServerTasks;
  tsp00_Int4              lDWTasks;
  tsp00_Int4              lEVTasks;
  tsp00_Int4              lGCTasks;
  tsp00_Int4              lBUPTasks;
  tsp00_Uint4             ulNumOfTasks;
  tsp00_Int4              lThrdPrio;
  tsp00_Int4              lBindToCPU ;
  bool                    fUtilityTask;
  bool                    fFixedPriority ;
  bool   	      	      fDisablePreemption ;
} teo70k_ThrdInfo;

typedef struct teo70k_VarTaskCntInfo {
  tsp00_Longuint          lStackSize;
  char                   *TaskTypeToken;
  teo70k_ThrdInfo        *pLastFoundUKT;
  tsp00_Int4              tracePages;
  tsp00_Int4              lVarTaskType;
  tsp00_Int4              lActTaskCnt;
  tsp00_Int4              lMaxTaskCnt;
  tsp00_Int4              lLastFoundCnt;
  tsp00_Int4              lUKTsFilled;
} teo70k_VarTaskCntInfo;

typedef struct teo70k_ClusterInfo {
  tsp00_Longuint                ulUKTBaseStackSize;
  teo70k_ThrdInfo        *pThrdInfo;
  teo70k_VarTaskCntInfo   VarTaskCntRec[MX_VAR_TASK_TYPES_EO70K];
  tsp00_Int4                    TI_Pos;
  tsp00_Int4                    AL_Pos;
  tsp00_Int4                    TW_Pos;
  tsp00_Int4                    UT_Pos;
  tsp00_Int4                    lConnectStrategy;
  bool                          fOneToOne;
  tsp00_Uint4                   ulNumOfUKTs;
  tsp00_Uint4                   ulNumOfUserUKTs;
  tsp00_Uint4                   ulNumOfTasks;
} teo70k_ClusterInfo;

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externC tsp00_Int4  eo70k_AnalyzeTaskcluster    ( RTE_XPARAM_REC      *XParam,
                                            teo70k_ClusterInfo **ppCluInfo,
                                            tsp00_Longuint                ulSystemPageSize);

externC tsp00_Int4 *eo70k_GetVarCntFromThrdInfo  ( teo70k_ThrdInfo     *pThrdInfo,
                                             tsp00_Int4                 lVarTaskType ) ;

#endif  /* GEO70K_H */
