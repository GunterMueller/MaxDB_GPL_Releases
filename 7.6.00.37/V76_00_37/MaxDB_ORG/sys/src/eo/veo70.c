/*!
  @file           veo70.c
  @author         JoergM
  @special area   kernel / LZU
  @brief          AnalyzeTaskcluster
  @see            example.html ...

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#define MOD__ "veo70.c:"

#if defined(WIN32)
#ifndef KERNEL
#define KERNEL
#endif /* KERNEL */
#endif
#include "geo70k.h"
#include "geo00_0.h"
#include "geo50_0.h"
#include "geo007_1.h"
#include "heo11.h"
#include "geo001.h"
#include "geo57.h"
#include "heo69.h"

#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.h" 
#include "SAPDBCommon/SAPDB_sprintf.h"
#include "SAPDBCommon/SAPDB_string.h"
/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#ifdef NO_INLINES
# define _INLINE
#else
# define _INLINE	__inline
#endif

#define VAR_TASK_FOUND 11

#ifdef WIN32
#define DEFAULT_UKT_PRIO  DEFAULT_UKT_THRD_PRIO
#else
#define DEFAULT_UKT_PRIO   3 
#endif

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


 
/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/

static tsp00_Longuint eo70_ulSystemPageSize ;

/*===========================================================================*
 *  LOCAL FUNCTIONS (PROTOTYPES)                                             *
 *===========================================================================*/
static void AddTaskStackSizeToUKTStackSize( teo70k_ThrdInfo      *pThrdInfo,
                                            tsp00_Longuint        ulTaskStackSize,
                                            tsp00_Int4            tracePages,
                                            int                   NumberOfTasks) ;

static tsp00_Int4  eo70_CheckTaskTypes   ( teo70k_ClusterInfo    *pCluInfo,
                                     teo70k_VarTaskCntInfo *pVarTasks ) ;

static tsp00_Int4  eo70_CheckPos         ( tsp00_Int4                  *plDest, 
                                     tsp00_Int4                   lValue, 
                                     char                  *pszToken );

static tsp00_Int4  eo70_CheckIfAlreadyDef( tsp00_Int4                  *plDest,
                                     tsp00_Int4                   lValue,
                                     tsp00_Int4                   lCompValue,
                                     char                  *pszToken );

static tsp00_Uint4 eo70_GetNumberOfTasks ( RTE_XPARAM_REC        *XParam );

static void  eo70_InitVarTaskInfo  ( teo70k_VarTaskCntInfo *pVarTasks,
                                     RTE_XPARAM_REC        *XParam );

static void  eo70_SetUKTVarTaskCnt ( teo70k_ThrdInfo       *pThrdInfo,
                                     teo70k_VarTaskCntInfo *pVarTasks,
                                     tsp00_Int4                   lTaskCnt) ;

static void  eo70_CheckCntGTMax    ( teo70k_ThrdInfo       *pThrdInfo,
                                     teo70k_VarTaskCntInfo *pVarTasks ) ;

static tsp00_Int4  eo70_CheckVarTaskCnt  ( char                  *pszToken,
                                     tsp00_Int4                   lCount,
                                     teo70k_ThrdInfo       *pThrdInfo,
                                     teo70k_VarTaskCntInfo *pVarTasks );

static void  eo70_PrepAllInOne     ( teo70k_ClusterInfo    *pCluInfo,
                                     teo70k_VarTaskCntInfo *pVarTasks );

static tsp00_Int4  eo70_PrepOneToOne     ( teo70k_ClusterInfo     *pCluInfo,
                                     teo70k_VarTaskCntInfo  *pVarTasks,
                                     tsp00_Int4                    ulMaxUKTs);

static tsp00_Int4  eo70_CreateVarTaskUKTs( teo70k_ClusterInfo     *pCluInfo,
                                           teo70k_VarTaskCntInfo  *pVarTasks,
                                           tsp00_Int4             *plThrdIdx,
                                           tsp00_Int4              lMaxUKTs,
                                           tsp00_Uint              ulMaxCPU);

static void  eo70_WrnActNotEqMax   ( teo70k_VarTaskCntInfo  *pVarTasks );

static void  eo70_DebCluInfo       ( teo70k_ClusterInfo     *pCluInfo ); 

static bool  eo70_CheckPriority    ( tsp00_Int4                    lPrioToCheck );

static void  eo70_AddExpliciteVarTasksToUKT ( teo70k_ThrdInfo       *pThrdInfo,
                                              teo70k_VarTaskCntInfo *pVarTask );

static void  eo70_AddImpliciteVarTaskToUKT  ( teo70k_ThrdInfo       *pThrdInfo,
                                              teo70k_VarTaskCntInfo *pVarTask,
                                              tsp00_Longuint         ulUKTBaseStack,
                                              tsp00_Uint             ulMaxCPU );

static tsp00_Int4  eo70_create_taskcluster        ( RTE_XPARAM_REC        *XParam,
                                              char                 **TaskCluster );

                                              


/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  *
 *===========================================================================*/


/*------------------------------*/
externC tsp00_Int4  eo70k_AnalyzeTaskcluster    ( RTE_XPARAM_REC      *XParam,
                                                  teo70k_ClusterInfo **ppCluInfo,
                                                  tsp00_Longuint       ulSystemPageSize)
  {
  #undef  MF__
  #define MF__ MOD__"eo70k_AnalyzeTaskcluster"

  teo70k_VarTaskCntInfo       *pVarTasks ;
  tsp00_Int4                   rc = NO_ERROR_EO001;
  teo70k_ClusterInfo          *pCluInfo;
  char                         c;
  tsp00_Int4                   lTCPos;
  char                         szToken [ 256 ];
  tsp00_Int4                   lTokenPos;
  tsp00_Uint4                  ulTasksFound;
  tsp00_Int4                   lCount;
  tsp00_Int4                   lThrdPrio;
  tsp00_Int4                   lThrdIdx;
  bool                         fConStratDefined;
  teo70k_ThrdInfo             *pPrevThrdInfo;
  tsp00_Int4                   i;
  char                        *pszTmp;
  bool                         fAllInOne;
  bool                         fOneToOne;
  tsp00_Uint4                  ulSize;
  tsp00_Uint4                  ulTotTasks;
  tsp00_Uint4                  ulMaxUKTs;
  char                        *pszTaskCluster;
  SAPDB_Int8                   specialTaskStack;
  SAPDB_Char                  *errText;

  DBGIN;

  if ( RTEConf_ParameterAccessGetInteger((SAPDB_UTF8 *)"MAX_SPECIALTASK_STACK"
      , &specialTaskStack
      , &errText ) )
  {
      specialTaskStack *= 1024;
  }
  else
  {
      specialTaskStack = 0;
  }

  eo70_ulSystemPageSize = ulSystemPageSize ;

  rc = eo70_create_taskcluster( XParam , &pszTaskCluster );

  if( rc != NO_ERROR_EO001 )
    {
    DBGOUT;
    return ( rc );
    }

  DBG3 ((MF__, "TaskCluster: '%s'", pszTaskCluster ));
  MSGD (( INFO_TSK_CLST, pszTaskCluster ));

  ulTotTasks = eo70_GetNumberOfTasks (XParam ) ;
  DBG3 ((MF__, "Number of Tasks: %d", ulTotTasks ));

  fConStratDefined = false;
  ulTasksFound     = 0;
  lThrdIdx         = 1;
  lCount           = 0;
  fAllInOne        = false;
  fOneToOne        = false;
  lThrdPrio        = DEFAULT_UKT_PRIO;

  ulMaxUKTs = ulTotTasks ;
  ulSize    = sizeof(teo70k_ClusterInfo) + (sizeof(teo70k_ThrdInfo) * ulMaxUKTs);

  rc = ALLOC_MEM_EO57 ((void **)ppCluInfo, ulSize );

  if( rc != NO_ERROR_EO001 )
    {
    DBGOUT;
    return ( rc );
    }

  SAPDB_memset ( *ppCluInfo , 0 , ulSize );
  pCluInfo            = *ppCluInfo;
  pCluInfo->pThrdInfo = (teo70k_ThrdInfo *)(pCluInfo + 1);

  pVarTasks = pCluInfo->VarTaskCntRec;
  eo70_InitVarTaskInfo ( pVarTasks, XParam ); 

  pCluInfo->ulUKTBaseStackSize = ALIGN_EO00( UKT_BASE_STACK_SIZE, eo70_ulSystemPageSize )
                               + ALIGN_EO00( UKT_TEMP_STACK_SIZE, eo70_ulSystemPageSize );

  pCluInfo->ulNumOfTasks = ulTotTasks;

  DBG3 (( MF__, "Number of Tasks needed %d \n", pCluInfo->ulNumOfTasks ));

  /* -- set some defaults  */

  pCluInfo->lConnectStrategy = STRAT_COMPRESS;

  for ( i = 0; i < (tsp00_Int4)ulMaxUKTs; i ++ )
    {
    pCluInfo->pThrdInfo[i].lThrdPrio = lThrdPrio;
    }


  /* ------------------------------------------------------------------------
   * Begin of task cluster
   * ----------------------------------------------------------------------- */

  for ( lTCPos = 0; pszTaskCluster [lTCPos]; lTCPos ++ )
    {
    /* ---  Collect the next token. */
    while (isspace ( c = pszTaskCluster [lTCPos] )) lTCPos ++;

    for ( lTokenPos = 0; lTokenPos < sizeof(szToken) - 1; lTokenPos ++ )
      {
      if (! isalnum(c))   break;
      if (islower(c))     c = toupper(c);

      szToken [ lTokenPos ] = c;
      c = pszTaskCluster [ ++ lTCPos ];
      }

    szToken [lTokenPos] = '\0';
    if (lTokenPos >= sizeof(szToken) - 1)
      {
      MSGALL(( ERR_TSK_CLST_TOK_TOO_LONG, szToken, lTokenPos ));
      DBG1  (( MF__, "Token too long '%s' - (maximum is %d characters)",
               szToken, lTokenPos ));
      DBGOUT;
      return ( -1 );
      }
    if ( lTokenPos == 0 )
      {
      if ( c == '\0' )
        break;     /* -- end of task cluster analyzation */
      else
        {
        DBG3 ((MF__, "c=%c Prio=%d Tsksf=%d\n", c, lThrdPrio, ulTasksFound))

        if ((c == ';') && (lThrdPrio != DEFAULT_UKT_PRIO))
          {
          if ( lThrdIdx > (tsp00_Int4)ulMaxUKTs )
            {
            MSGALL(( ERR_TSK_CLST_TOO_COMPLEX ));
            DBG1  (( MF__, "TaskCluster too complex" ));
            DBGOUT;
            return ( -1 );
            }
          lThrdIdx ++;
          }
        continue;      /* get next token */
        }
      }
    lThrdPrio = DEFAULT_UKT_PRIO;
    while ( isspace ( c = pszTaskCluster [ lTCPos ] ) ) lTCPos ++;

    DBG3 ((MF__, "Index: %3d  -  Count: %3d  -  Token: %10s  -  Delimiter: %#x",
                 lThrdIdx, lCount, szToken, c ));

    switch ( c )
      {
      case '*' :
        lCount = (tsp00_Int4)strtol ( szToken , &pszTmp, 10 );
        if ( *pszTmp || (lCount < 1) )
          {
          MSGALL(( ERR_TSK_CLST_ILL_NUMBER, szToken ));
          DBG1  (( MF__, "Illegal number '%s' ", szToken ));
          DBGOUT;
          return ( -1 );
          }
        break;
       case '+' :
       case '-' :

          lThrdPrio = (tsp00_Int4) strtol ( szToken , &pszTmp , 10 );

#ifdef WIN32
          if ( c == '-' )
#else
          if ( c == '+' )
#endif
            lThrdPrio *= -1;

#if defined(WIN32)
          DBG3  (( MF__, "new priority %d, range = %d - %d", 
                   lThrdPrio, THREAD_PRIORITY_IDLE, THREAD_PRIORITY_TIME_CRITICAL  ));
#else
          DBG3  (( MF__, "new priority %d", lThrdPrio ));
#endif
          if ( eo70_CheckPriority(lThrdPrio) )
          {
            rc = eo70_CheckIfAlreadyDef ( &pCluInfo->pThrdInfo[lThrdIdx - 1].lThrdPrio ,
                                           lThrdPrio , DEFAULT_UKT_PRIO, szToken );
            if (rc != NO_ERROR_EO001)
            {
              DBGOUT;
              return ( -1 );
            }
          }
        break;

      case ',' :
      case ';' :
      case '\0' :
        ulTasksFound = 0;

        rc = eo70_CheckVarTaskCnt (szToken, lCount,
                                    &pCluInfo->pThrdInfo[lThrdIdx-1], pVarTasks ) ;

        if ( rc == VAR_TASK_FOUND )
        {
          ulTasksFound ++;
          break;
        }
        else
          if (rc != NO_ERROR_EO001)
          {
          DBGOUT;
          return ( -1 );
          }

        if ( ! strcmp ( szToken , TASK_TOKEN_TIMER_EO70K ) )
          {
          if ( eo70_CheckPos ( & pCluInfo->TI_Pos , lThrdIdx , szToken ) )
            {
            DBGOUT;
            return ( -1 );
            }

          pCluInfo->pThrdInfo[lThrdIdx-1].lSpecialTasks ++;
          ulTasksFound ++;
          break;
          }

        if ( ! strcmp ( szToken , TASK_TOKEN_ALOGWR_EO70K ) )
          {
          if ( eo70_CheckPos ( & pCluInfo->AL_Pos , lThrdIdx , szToken ) )
            {
            DBGOUT;
            return ( -1 );
            }
          pCluInfo->pThrdInfo[lThrdIdx-1].lSpecialTasks ++;
          ulTasksFound ++;
          break;
          }

        if ( ! strcmp ( szToken , TASK_TOKEN_TRACEWR_EO70K ) )
          {
          if ( eo70_CheckPos ( & pCluInfo->TW_Pos , lThrdIdx , szToken ) )
            {
            DBGOUT;
            return ( -1 );
            }
          pCluInfo->pThrdInfo[lThrdIdx-1].lSpecialTasks ++;
          ulTasksFound ++;
          break;
          }
        if ( ! strcmp ( szToken , TASK_TOKEN_UTILITY_EO70K ) )
          {
          if ( eo70_CheckPos ( & pCluInfo->UT_Pos , lThrdIdx , szToken ) )
            {
            DBGOUT;
            return ( -1 );
            }
          pCluInfo->pThrdInfo[lThrdIdx-1].lSpecialTasks ++;
          pCluInfo->pThrdInfo[lThrdIdx-1].fUtilityTask  = true;
          ulTasksFound ++;
          break;
          }

        if ( ! strcmp ( szToken , TOKEN_COMPRESS_EO70K ) )
          {
          if ( fConStratDefined == true )
            {
            MSGALL(( ERR_TSK_CLST_CON_ALRDY_SPEC ));
            DBG1  (( MF__, "Connect strategy already specified" ));
            DBGOUT;
            return ( -1 );
            }
          else
            {
            pCluInfo->lConnectStrategy = STRAT_COMPRESS;
            fConStratDefined          = true;
            }
          break;
          }

        if ( ! strcmp ( szToken , TOKEN_EQUALIZE_EO70K ) )
          {
          if ( fConStratDefined == true )
            {
            MSGALL(( ERR_TSK_CLST_CON_ALRDY_SPEC ));
            DBG1  (( MF__, "Connect strategy already specified" ));
            DBGOUT;
            return ( -1 );
            }
          else
            {
            pCluInfo->lConnectStrategy = STRAT_EQUALIZE;
            fConStratDefined          = true;
            }
          break;
          }

        if ( ! strcmp ( szToken , TOKEN_ALLINONE_EO70K ) )
          {
          /* --- if this is specified, it must be allone */
          if ( c != '\0' || lTCPos != 8 )
            {
            MSGALL(( ERR_TSK_CLST_MORE_T_ONE_UKT, TOKEN_ALLINONE_EO70K ));
            DBG1  (( MF__, "More then one UKT specified - '%s' not valid", TOKEN_ALLINONE_EO70K ));
            DBGOUT;
            return ( -1 );
            }
            fAllInOne = true;
            break;
          }

        if ( ! strcmp ( szToken , TOKEN_ONETOONE_EO70K ) )
          {
          /* --- if this is specified, it must be allone */
          if ( c != '\0' || lTCPos != 8 )
            {
            MSGALL(( ERR_TSK_CLST_MORE_T_ONE_UKT, TOKEN_ONETOONE_EO70K ));
            DBG1  (( MF__, "More then one UKT specified - '%s' not valid", TOKEN_ONETOONE_EO70K ));
            DBGOUT;
            return ( -1 );
            }
          fOneToOne = true;
          break;
          }

        MSGALL(( ERR_TSK_CLST_UNKNOWN_TOKEN, szToken ));
        DBG1  (( MF__, "Unknown token '%s'", szToken ));
        DBGOUT;
        return ( -1 );

      default :
        MSGALL(( ERR_TSK_CLST_ILL_CHAR, c ));
        DBG1  (( MF__, "Illegal character '%c'", c ));
        DBGOUT;
        return ( -1 );
      }

    if ( c == '\0' ) break;
    if ( c != '*'  ) lCount = 0;

    if ( c == ';' && ulTasksFound )
      {
      if ( lThrdIdx > (tsp00_Int4)ulMaxUKTs )
        {
        MSGALL(( ERR_TSK_CLST_TOO_COMPLEX ));
        DBG1  (( MF__, "TaskCluster too complex" ));
        DBGOUT;
        return ( -1 );
        }
      lThrdIdx ++;
      }
    lTokenPos = 0;
    }

  /* -----------------------------------------------------------------------
   *  End of task cluster
   * ----------------------------------------------------------------------- */
  
  if ( fAllInOne )
  {
      eo70_PrepAllInOne ( pCluInfo, pVarTasks ) ; 
      pCluInfo->ulNumOfUserUKTs = 1;
  }
  else if ( fOneToOne )
  {
      if ( eo70_PrepOneToOne ( pCluInfo, pVarTasks, ulMaxUKTs ) != NO_ERROR_EO001 )
      {
        DBGOUT;
        return ( -1 );
      }
      pCluInfo->ulNumOfUserUKTs = XParam->ulMaxUser;
  }
  else
  {
      if (eo70_CheckTaskTypes(pCluInfo, pVarTasks) != NO_ERROR_EO001)
      {
        DBGOUT;
        return ( -1 );
      }
      pCluInfo->ulNumOfUserUKTs = XParam->ulMaxCPU;
      if ( pVarTasks[VAR_TASK_US_EO70K].lLastFoundCnt == 1 )
      {
          pCluInfo->ulNumOfUserUKTs = ( 1 == pVarTasks[VAR_TASK_US_EO70K].lLastFoundCnt 
                                            ? XParam->ulMaxUser 
                                            : XParam->ulMaxCPU );
      }
  }

  /* -----------------------------------------------------------------------
   *     Compute the total thread-, user-, and server-counts.
   * ----------------------------------------------------------------------- */

  /* --- Step through all explicitly filled UKTs.  */

  for ( lThrdIdx = 1; lThrdIdx <= (tsp00_Int4)ulMaxUKTs; lThrdIdx ++ )
    {
    pPrevThrdInfo = &pCluInfo->pThrdInfo[lThrdIdx-1];

    /* --- The list of explicitly filled UKTs ends when
     *     a UKT with no tasks is found.                 
     */

    if ( !pPrevThrdInfo->lUserTasks   &&
         !pPrevThrdInfo->lServerTasks &&
         !pPrevThrdInfo->lDWTasks     &&
         !pPrevThrdInfo->lEVTasks     &&
         !pPrevThrdInfo->lGCTasks     &&
         !pPrevThrdInfo->lBUPTasks    &&
         !pPrevThrdInfo->lSpecialTasks )
      {
      DBG3 (( MF__, "Breaking on empty index %d", lThrdIdx ));
      break;
      }

    /* ---  Avoid having more users(servers) than MaxUser(MaxServer). */
    eo70_CheckCntGTMax ( pPrevThrdInfo, pVarTasks ) ;

    /* ---  The above could lead to an empty Thread.
     *      In this case shift the rest of the UKTs one position left. */
    if ( !pPrevThrdInfo->lUserTasks   &&
         !pPrevThrdInfo->lServerTasks &&
         !pPrevThrdInfo->lDWTasks     &&
         !pPrevThrdInfo->lEVTasks     &&
         !pPrevThrdInfo->lGCTasks     &&
         !pPrevThrdInfo->lBUPTasks    &&
         !pPrevThrdInfo->lSpecialTasks )
      {
      DBG3 (( MF__, "Shifting to index %d", lThrdIdx ));

      for ( lTCPos = lThrdIdx; lTCPos < (tsp00_Int4)ulMaxUKTs; lTCPos ++ )
        SAPDB_memcpy ( &pCluInfo->pThrdInfo[lTCPos-1], &pCluInfo->pThrdInfo[lTCPos], sizeof(teo70k_ThrdInfo));

      SAPDB_memset ( &pCluInfo->pThrdInfo[ulMaxUKTs-1], 0, sizeof(teo70k_ThrdInfo));

      if ( pCluInfo->TI_Pos   > lThrdIdx ) pCluInfo->TI_Pos --;
      if ( pCluInfo->AL_Pos   > lThrdIdx ) pCluInfo->AL_Pos --;
      if ( pCluInfo->TW_Pos   > lThrdIdx ) pCluInfo->TW_Pos --;
      if ( pCluInfo->UT_Pos   > lThrdIdx ) pCluInfo->UT_Pos --;
      lThrdIdx --;

      continue;
      }

    /* --- Init the number of tasks */
    pPrevThrdInfo->ulNumOfTasks = pPrevThrdInfo->lSpecialTasks;

  /* -----------------------------------------------------------------------
   *     Compute this thread stack size.
   *     Round each task value up to a multiple of SystemPageSize. It is needed
   *     to build seperate task stacks which are align on SystemPageSize bounds.
   * ----------------------------------------------------------------------- */

    if ( pPrevThrdInfo->ulStackSize == 0 ) 
      pPrevThrdInfo->ulStackSize = pCluInfo->ulUKTBaseStackSize ;

    if ( pCluInfo->TI_Pos == lThrdIdx )
      AddTaskStackSizeToUKTStackSize( pPrevThrdInfo, 
                                      SECURITY_BUFFER_MXEO69 
                  +  (specialTaskStack > 0 ? (tsp00_Longuint)specialTaskStack : FIX_TI_STACK_SIZE_EO70K ), 
                                      XParam->tracePagesTI,
                                      1);
    if ( pCluInfo->AL_Pos == lThrdIdx )
      AddTaskStackSizeToUKTStackSize( pPrevThrdInfo, 
                                      SECURITY_BUFFER_MXEO69
                  +  (specialTaskStack > 0 ? (tsp00_Longuint)specialTaskStack : FIX_AL_STACK_SIZE_EO70K ), 
                                      XParam->tracePagesAL,
                                      1);
    if ( pCluInfo->TW_Pos == lThrdIdx )
      AddTaskStackSizeToUKTStackSize( pPrevThrdInfo, 
                                      SECURITY_BUFFER_MXEO69
                  +  (specialTaskStack > 0 ? (tsp00_Longuint)specialTaskStack : FIX_TW_STACK_SIZE_EO70K ), 
                                      XParam->tracePagesTW,
                                      1);
    if ( pCluInfo->UT_Pos == lThrdIdx )
      AddTaskStackSizeToUKTStackSize( pPrevThrdInfo, 
                                      SECURITY_BUFFER_MXEO69 + XParam->ulMaxUsUtStack,
                                      XParam->tracePagesUT,
                                      1);

    eo70_AddExpliciteVarTasksToUKT ( pPrevThrdInfo, pVarTasks ) ;
    }

  lThrdIdx-- ;

  if ( eo70_CreateVarTaskUKTs ( pCluInfo, pVarTasks, &lThrdIdx, (tsp00_Int4 )ulMaxUKTs, XParam->ulMaxCPU ) != NO_ERROR_EO001 )
  {
    DBGOUT;
    return ( -1 );
  }

  pCluInfo->ulNumOfUKTs = lThrdIdx ;

  DBG3 (( MF__, "Number of UKTs needed %d \n", pCluInfo->ulNumOfUKTs ));

  eo70_WrnActNotEqMax ( pVarTasks ) ;

  eo70_DebCluInfo ( pCluInfo ) ;

  RTEConf_FreeTaskClusterArrayInXParamStructure( XParam );

  {
    SAPDB_Char msgBuffer[256];
    SAPDB_sprintf(msgBuffer, sizeof(msgBuffer), "Total Number of UKT %d", pCluInfo->ulNumOfUKTs);
    MSGD (( INFO_TSK_CLST, msgBuffer ));
    SAPDB_sprintf(msgBuffer, sizeof(msgBuffer), "Number of UKT with User Tasks %d", pCluInfo->ulNumOfUserUKTs);
    MSGD (( INFO_TSK_CLST, msgBuffer ));
  }

  /* WARNING! This is correct but not 'clean'... */
  /* Add regions for log history directory here (HISTn), since dynamically dependend from task cluster */
  /* This MUST be done here, since parameter analysis can only setup the 'constant' number of regions! */
  /* A clean solution would mean to do task cluster analysis during configuration parameter reading... ffs... */
  XParam->ulNoOfRegions += pCluInfo->ulNumOfUserUKTs;

  /* PTS 1133794 mb 2005-02-07 adjusting of ulNoOfRegions by LOG_QUEUE_COUNT */
  /* now in RTEConf_ParameterAccessKernelInterface                           */

  DBGOUT;
  return ( NO_ERROR_EO001 );
  }

/*------------------------------*/
externC tsp00_Int4 *eo70k_GetVarCntFromThrdInfo  ( teo70k_ThrdInfo     *pThrdInfo,
                                             tsp00_Int4                 lVarTaskType ) 
  {
  #undef  MF__
  #define MF__ MOD__"eo70k_GetVarCntFromThrdInfo"

  tsp00_Int4 *plTaskCntToCheck ;

  switch ( lVarTaskType )
  {
    case VAR_TASK_DW_EO70K : plTaskCntToCheck = &pThrdInfo->lDWTasks    ; break ;
    case VAR_TASK_EV_EO70K : plTaskCntToCheck = &pThrdInfo->lEVTasks    ; break ;
    case VAR_TASK_US_EO70K : plTaskCntToCheck = &pThrdInfo->lUserTasks  ; break ;
    case VAR_TASK_SV_EO70K : plTaskCntToCheck = &pThrdInfo->lServerTasks; break ;
    case VAR_TASK_GC_EO70K : plTaskCntToCheck = &pThrdInfo->lGCTasks    ; break ;
    case VAR_TASK_BUP_EO70K: plTaskCntToCheck = &pThrdInfo->lBUPTasks   ; break ;
    default: break ;
  }
  DBGOUT;
  return (plTaskCntToCheck) ;
}


/*===========================================================================*
 *  LOCAL FUNCTIONS (CODE)                                                   *
 *===========================================================================*/
/*------------------------------*/

/* PTS 1106437 guardpages now always added !!! (veo70.c) */
static void AddTaskStackSizeToUKTStackSize( teo70k_ThrdInfo      *pThrdInfo,
                                            tsp00_Longuint        ulTaskStackSize,
                                            tsp00_Int4            tracePages,
                                            int                   NumberOfTasks)
{
  pThrdInfo->ulStackSize += ALIGN_EO00( ulTaskStackSize * NumberOfTasks, eo70_ulSystemPageSize ) +
                            NumberOfTasks * NUM_GUARDPAGES_EO70 * eo70_ulSystemPageSize ;
  pThrdInfo->tracePages += tracePages * NumberOfTasks;
}

/*------------------------------*/

static tsp00_Int4  eo70_create_taskcluster( RTE_XPARAM_REC            *XParam,
                                      char                      **TaskCluster )

{
  #undef  MF__
  #define MF__ MOD__"eo70_create_taskcluster"

  DBGIN;

  if ( XParam->TaskClusterArray != NULL )
    { int i, total_len ;
      for ( total_len = i = 0; XParam->TaskClusterArray[ i ] ; i++ )
        total_len += (tsp00_Int4)strlen ( XParam->TaskClusterArray[ i ] ) ;

      if ( ALLOC_MEM_EO57 ( (void **)TaskCluster,
                     (total_len+1) * sizeof ( char ) ) != NO_ERROR_EO001 )
        { MSGCD (( ERR_MALLOC_FAILED_FOR ,"create TASKCLUSTER" ));
          DBGOUT ;
          return( ERROR_NOT_ENOUGH_MEMORY_EO001 );
        } 

	  /* PTS 1103945 */
      strcpy ( *TaskCluster, XParam->TaskClusterArray[ 0 ] );
	  /* if _TASKCLUSTER_01 == ONETOONE || ALLINONE, ignore the rest */
	  if ( _tcsicmp(*TaskCluster, TOKEN_ALLINONE_EO70K ) != 0 &&
           _tcsicmp(*TaskCluster, TOKEN_ONETOONE_EO70K ) != 0 )
        for ( i = 1; XParam->TaskClusterArray[ i ] ; i++ )
          strcat ( *TaskCluster, XParam->TaskClusterArray[ i ] );

#ifdef FORCE_ALL_IN_ONE
        strcpy ( *TaskCluster, TOKEN_ALLINONE_EO70K );
#endif /* FORCE_ALL_IN_ONE */
#ifdef FORCE_ONE_TO_ONE
        strcpy ( *TaskCluster, TOKEN_ONETOONE_EO70K );
#endif /* FORCE_ONE_TO_ONE */

        DBG3 ((MF__, "whole Taskcluster '%s'", *TaskCluster ));
   }
  DBGOUT;
  return(NO_ERROR_EO001);
}

/*------------------------------*/

void   eo70_deb_taskcluster_array( RTE_XPARAM_REC            *XParam)
{
  #undef  MF__
  #define MF__ MOD__"eo70_deb_taskcluster_array"

  DBGIN;

  if ( XParam->TaskClusterArray != NULL )
  { int i ;
      for ( i=0; XParam->TaskClusterArray[ i ] ; i++ )
        MSGD (( INFO_XPARAM_TASKCLUSTER, i, XParam->TaskClusterArray[i] ));
  }
  DBGOUT;
}



/*------------------------------*/

static tsp00_Int4 eo70_CheckTaskTypes ( teo70k_ClusterInfo    *pCluInfo,
                                   teo70k_VarTaskCntInfo *pVarTasks ) 
  {
  #undef  MF__
  #define MF__ MOD__"eo70k_CheckTaskTypes"
  tsp00_Int4 rc = NO_ERROR_EO001;
  int     iVarTask ;

  DBGPAS;

  if ( pCluInfo->TI_Pos   == 0 )
    {
    MSGALL(( ERR_TSK_CLST_MIS_TASK_TYPE, "ti" ));
    DBG1  (( MF__, "Missing task type '%s'", "ti" ));
    rc = -1;
    }
  if ( pCluInfo->AL_Pos   == 0 )
    {
    MSGALL(( ERR_TSK_CLST_MIS_TASK_TYPE, "al" ));
    DBG1  (( MF__, "Missing task type '%s'", "a2" ));
    rc = -1;
    }
  if ( pCluInfo->TW_Pos   == 0 )
    {
    MSGALL(( ERR_TSK_CLST_MIS_TASK_TYPE, "tw" ));
    DBG1  (( MF__, "Missing task type '%s'", "tw" ));
    rc = -1;
    }
  if ( pCluInfo->UT_Pos   == 0 )
    {
    MSGALL(( ERR_TSK_CLST_MIS_TASK_TYPE, "ut" ));
    DBG1  (( MF__, "Missing task type '%s'", "ut" ));
    rc = -1;
    }

  for ( iVarTask = 0; 
        iVarTask < MX_VAR_TASK_TYPES_EO70K && rc == NO_ERROR_EO001 ; 
        iVarTask ++, pVarTasks++ ) 
    if ( pVarTasks->lLastFoundCnt == 0 && pVarTasks->lMaxTaskCnt != 0 )
    {
      MSGALL(( ERR_TSK_CLST_MIS_TASK_TYPE, pVarTasks->TaskTypeToken ));
      DBG1  (( MF__, "Missing task type '%s'", pVarTasks->TaskTypeToken ));
      rc = -1;
    }

  DBG3 (( MF__, "Task types complete: %f ", (rc == NO_ERROR_EO001)));

  return (rc);
  }

/*------------------------------*/

static tsp00_Int4  eo70_CheckPos ( tsp00_Int4 *plDest, 
                              tsp00_Int4 lValue, 
                              char *pszToken )
  {
  #undef  MF__
  #define MF__ MOD__"eo70k_CheckPos"

  DBGPAS;

  if ( *plDest )
    {
    MSGALL(( ERR_TSK_CLST_MULT_DEFINED, pszToken ));
    DBG1  (( MF__, "Multiply defined '%s' ", pszToken ));
    return ( -1 );
    }
  *plDest = lValue;

  return ( NO_ERROR_EO001 );
  }

/*------------------------------*/

static tsp00_Int4  eo70_CheckIfAlreadyDef ( tsp00_Int4 *plDest,      tsp00_Int4  lValue,
                                       tsp00_Int4  lCompValue,  char *pszToken )
  {
  #undef  MF__
  #define MF__ MOD__"eo70k_CheckIfAlreadyDef"

  DBGPAS;

  if ( *plDest != lCompValue )
    {
    MSGALL(( ERR_TSK_CLST_MULT_DEFINED, pszToken ));
    DBG1  (( MF__, "Multiply defined '%s' ", pszToken ));
    return ( -1 );
    }
  *plDest = lValue;

  return ( NO_ERROR_EO001 );
  }

/*------------------------------*/

static tsp00_Uint4  eo70_GetNumberOfTasks ( RTE_XPARAM_REC         *XParam )
{
  #undef  MF__
  #define MF__ MOD__"eo70k_GetNumberOfTasks"

  tsp00_Uint4 ulMaxTasks = 1           +       /* - TI-Task */
                     1           +       /* - AL-Task */
                     1           +       /* - TW-Task */
                     1;                  /* - UT-Task */
  DBGPAS;

  ulMaxTasks += XParam->ulMaxDataWriter + 
                XParam->ulMaxEventTasks +
                XParam->ulMaxGarbageCollector +
                XParam->ulMaxBackupTasks+
                XParam->ulMaxUser +
                XParam->ulMaxServer ;
;

  return ( ulMaxTasks ) ;
}

/*------------------------------*/

static void  eo70_InitVarTaskInfo ( teo70k_VarTaskCntInfo  *pVarTasks,
                                     RTE_XPARAM_REC         *XParam)
  {
  #undef  MF__
  #define MF__ MOD__"eo70k_InitVarTaskInfo"

  int  iVarTask ;
  SAPDB_Int8 specialTaskStack;
  SAPDB_Int8 serverTaskStack;
  SAPDB_Char *errText;

  DBGPAS;
   
  if ( RTEConf_ParameterAccessGetInteger((SAPDB_UTF8 *)"MAX_SPECIALTASK_STACK"
      , &specialTaskStack
      , &errText ) )
  {
      specialTaskStack *= 1024;
  }
  else
  {
      specialTaskStack = 0;
  }

  if ( RTEConf_ParameterAccessGetInteger((SAPDB_UTF8 *)"MAX_SERVERTASK_STACK"
      , &serverTaskStack
      , &errText ) )
  {
      serverTaskStack *= 1024;
  }
  else
  {
      serverTaskStack = 0;
  }

  SAPDB_memset( pVarTasks, 0, sizeof ( teo70k_VarTaskCntInfo ) * MX_VAR_TASK_TYPES_EO70K ) ;

  for ( iVarTask = 0; iVarTask < MX_VAR_TASK_TYPES_EO70K ; iVarTask ++, pVarTasks++  ) 
  { 
    pVarTasks->lVarTaskType = iVarTask ;
    pVarTasks->lUKTsFilled  = 0;
    switch ( iVarTask )
    {
      case VAR_TASK_DW_EO70K :
                         pVarTasks->TaskTypeToken = TASK_TOKEN_DATAWRITER_EO70K;
                         pVarTasks->lMaxTaskCnt   = XParam->ulMaxDataWriter ; 
                         pVarTasks->lStackSize    = ALIGN_EO00( SECURITY_BUFFER_MXEO69
                             + (specialTaskStack > 0 ? (tsp00_Longuint)specialTaskStack : FIX_DW_STACK_SIZE_EO70K),
                                                           eo70_ulSystemPageSize);
                         pVarTasks->tracePages    = XParam->tracePagesDW;
                         break ;
      case VAR_TASK_EV_EO70K :
                         pVarTasks->TaskTypeToken = TASK_TOKEN_EVENT_EO70K;
                         pVarTasks->lMaxTaskCnt   = XParam->ulMaxEventTasks ; 
                         pVarTasks->lStackSize    = ALIGN_EO00( SECURITY_BUFFER_MXEO69
                             + (specialTaskStack > 0 ? (tsp00_Longuint)specialTaskStack : FIX_EV_STACK_SIZE_EO70K),
                                                           eo70_ulSystemPageSize);
                         pVarTasks->tracePages    = XParam->tracePagesEV;
                         break ;
      case VAR_TASK_GC_EO70K :
                         pVarTasks->TaskTypeToken = TASK_TOKEN_GARBAGEC_EO70K;
                         pVarTasks->lMaxTaskCnt   = XParam->ulMaxGarbageCollector ; 
                         pVarTasks->lStackSize    = ALIGN_EO00( SECURITY_BUFFER_MXEO69
                             + (specialTaskStack > 0 ? (tsp00_Longuint)specialTaskStack : FIX_GC_STACK_SIZE_EO70K),
                                                           eo70_ulSystemPageSize);
                         pVarTasks->tracePages    = XParam->tracePagesGC;
                         break ;
      case VAR_TASK_BUP_EO70K: 
                         pVarTasks->TaskTypeToken = TASK_TOKEN_BACKUP_EO70K;
                         pVarTasks->lMaxTaskCnt   = XParam->ulMaxBackupTasks; 
                         pVarTasks->lStackSize    = ALIGN_EO00( SECURITY_BUFFER_MXEO69
                             + (specialTaskStack > 0 ? (tsp00_Longuint)specialTaskStack : FIX_BUP_STACK_SIZE_EO70K),
                                                           eo70_ulSystemPageSize);
                         pVarTasks->tracePages    = XParam->tracePagesBUP;
                         break ;
      case VAR_TASK_US_EO70K :
                         pVarTasks->TaskTypeToken = TASK_TOKEN_USER_EO70K;
                         pVarTasks->lMaxTaskCnt   = XParam->ulMaxUser ; 
                         pVarTasks->lStackSize    = ALIGN_EO00( SECURITY_BUFFER_MXEO69 + XParam->ulMaxUsUtStack,
                                                           eo70_ulSystemPageSize);
                         pVarTasks->tracePages    = XParam->tracePagesUS;
                         break ;
      case VAR_TASK_SV_EO70K :
                         pVarTasks->TaskTypeToken = TASK_TOKEN_SERVER_EO70K;
                         pVarTasks->lMaxTaskCnt   = XParam->ulMaxServer ; 
                         pVarTasks->lStackSize    = ALIGN_EO00( SECURITY_BUFFER_MXEO69
                             + (serverTaskStack > 0 ? (tsp00_Longuint)serverTaskStack : FIX_SV_STACK_SIZE_EO70K),
                                                           eo70_ulSystemPageSize);
                         pVarTasks->tracePages    = XParam->tracePagesSV;
                         break ;
      default:
                         break ;
    }
  }

  DBGOUT;
  return ;
}

/*------------------------------*/
static void  eo70_SetUKTVarTaskCnt ( teo70k_ThrdInfo       *pThrdInfo,
                                      teo70k_VarTaskCntInfo *pVarTasks,
                                      tsp00_Int4                   lTaskCnt) 
  {
  #undef  MF__
  #define MF__ MOD__"eo70k_SetUKTVarTaskCnt"

  tsp00_Int4 *plTaskCntToSet = eo70k_GetVarCntFromThrdInfo(pThrdInfo, pVarTasks->lVarTaskType) ;

  *plTaskCntToSet = lTaskCnt ;

  DBGOUT;
  return ;
}


/*------------------------------*/
static void  eo70_CheckCntGTMax (teo70k_ThrdInfo        *pThrdInfo,
                                  teo70k_VarTaskCntInfo  *pVarTasks ) 
  {
  #undef  MF__
  #define MF__ MOD__"eo70k_CheckCntGTMax"

  int     iVarTask ;
  tsp00_Int4    *plTaskCntToCheck ;

  for ( iVarTask = 0; iVarTask < MX_VAR_TASK_TYPES_EO70K ; iVarTask ++, pVarTasks++  ) 
  { 
    plTaskCntToCheck = eo70k_GetVarCntFromThrdInfo (pThrdInfo, pVarTasks->lVarTaskType) ;

    if ( ( pVarTasks->lActTaskCnt + *plTaskCntToCheck ) > pVarTasks->lMaxTaskCnt )
      *plTaskCntToCheck = pVarTasks->lMaxTaskCnt - pVarTasks->lActTaskCnt ;
  }

  DBGOUT;
  return ;
}

/*------------------------------*/
static tsp00_Int4  eo70_CheckVarTaskCnt (char                   *pszToken,
                                    tsp00_Int4                    lCount,
                                    teo70k_ThrdInfo        *pThrdInfo,
                                    teo70k_VarTaskCntInfo  *pVarTasks ) 
  {
  #undef  MF__
  #define MF__ MOD__"eo70k_CheckVarTaskCnt"

  int     iVarTask ;
  bool    fFound = false ;
  tsp00_Int4    rc     = NO_ERROR_EO001;

  DBGPAS;
   
  for ( iVarTask = 0; 
        iVarTask < MX_VAR_TASK_TYPES_EO70K && rc == NO_ERROR_EO001 && !fFound; 
        iVarTask ++ ) 
  {
    if ( ! strcmp ( pszToken , pVarTasks[iVarTask].TaskTypeToken ) )
    { 
      fFound = true ;
      if ( lCount == 0 )
        lCount = pVarTasks[iVarTask].lLastFoundCnt;
      
      if ( lCount == 0 )
      {
        MSGALL(( ERR_TSK_CLST_MIS_XX_CNT, pVarTasks[iVarTask].TaskTypeToken ));
        DBG1  (( MF__, "Missing count on first '%s'", pVarTasks[iVarTask].TaskTypeToken ));
        rc = !NO_ERROR_EO001 ;
      }
      else
      {
        tsp00_Int4 *plTaskCntToCheck = eo70k_GetVarCntFromThrdInfo(pThrdInfo, 
                                                             pVarTasks[iVarTask].lVarTaskType) ;
 
        pVarTasks[iVarTask].lLastFoundCnt      = lCount;

        if ( eo70_CheckPos (plTaskCntToCheck, lCount, pszToken) == NO_ERROR_EO001 )
          rc = VAR_TASK_FOUND ;
        else
          rc = !NO_ERROR_EO001 ;
      }
    }
  }

  DBGOUT;
  return ( rc );
}

/*------------------------------*/

static void  eo70_PrepAllInOne ( teo70k_ClusterInfo    *pCluInfo,
                                  teo70k_VarTaskCntInfo *pVarTasks ) 
  {
  #undef  MF__
  #define MF__ MOD__"eo70k_PrepAllInOne"

  int  iVarTask ;

  DBG3 ((MF__, "All tasks go into one UKT" ));

  pCluInfo->pThrdInfo[0].fUtilityTask   = true;

  pCluInfo->TI_Pos                      = 1;
  pCluInfo->AL_Pos                      = 1;
  pCluInfo->TW_Pos                      = 1;
  pCluInfo->UT_Pos                      = 1;
  pCluInfo->pThrdInfo[0].lSpecialTasks  = 4;
  pCluInfo->fOneToOne                   = true;
   
  for ( iVarTask = 0; iVarTask < MX_VAR_TASK_TYPES_EO70K ; iVarTask ++, pVarTasks++ ) 
  {
    eo70_SetUKTVarTaskCnt( &pCluInfo->pThrdInfo[0], pVarTasks, pVarTasks->lMaxTaskCnt ) ;
    pVarTasks->lLastFoundCnt  = pVarTasks->lMaxTaskCnt ;
    pVarTasks->pLastFoundUKT  = &pCluInfo->pThrdInfo[0];
  }

  DBGOUT;
  return;
}

/*------------------------------*/

static tsp00_Int4  eo70_PrepOneToOne ( teo70k_ClusterInfo    *pCluInfo,
                                  teo70k_VarTaskCntInfo *pVarTasks,
                                  tsp00_Int4                 ulMaxUKTs) 
  {
  #undef  MF__
  #define MF__ MOD__"eo70k_PrepOneToOne"

  int  iVarTask ;
  tsp00_Int4 lThrdIdx, lTotTask ;
  tsp00_Int4 rc     = NO_ERROR_EO001;

  DBG3 (( MF__, "Each task has its own UKT"));

  pCluInfo->TI_Pos     = 1;
  pCluInfo->AL_Pos     = 2;
  pCluInfo->TW_Pos     = 3;
  pCluInfo->UT_Pos     = 4;
  lTotTask             = pCluInfo->UT_Pos ;

  pCluInfo->pThrdInfo[pCluInfo->UT_Pos - 1].fUtilityTask   = true;

  for ( lThrdIdx = 1; lThrdIdx <= pCluInfo->UT_Pos; lThrdIdx ++ )
     pCluInfo->pThrdInfo[lThrdIdx-1].lSpecialTasks = 1;

  for ( iVarTask = 0; iVarTask < MX_VAR_TASK_TYPES_EO70K ; iVarTask ++, pVarTasks++ ) 
  {
    pVarTasks->lLastFoundCnt  = 1 ;
    lTotTask += pVarTasks->lMaxTaskCnt ;
    pVarTasks->pLastFoundUKT  = &pCluInfo->pThrdInfo[lThrdIdx + iVarTask ];
  }

  if ( lTotTask > ulMaxUKTs )
  {
    rc = !NO_ERROR_EO001 ;
    MSGALL(( ERR_TSK_CLST_TOO_MANY_UKTS, lTotTask, ulMaxUKTs ));
    DBG1  (( MF__, "Number of UKTs needed: %d, allowed number: %d",lTotTask, ulMaxUKTs ));
  }

  DBGOUT;
  return ( rc );
}

/*------------------------------*/

static void  eo70_AddExpliciteVarTasksToUKT ( teo70k_ThrdInfo       *pThrdInfo,
                                               teo70k_VarTaskCntInfo *pVarTask ) 
{
  #undef  MF__
  #define MF__ MOD__"eo70k_AddExpliciteVarTasksToUKT"

  int     iVarTask ;
  tsp00_Int4    lTasksInUkt ;

  DBGPAS;
   
  for ( iVarTask = 0; iVarTask < MX_VAR_TASK_TYPES_EO70K; iVarTask++, pVarTask++ ) 
  {
    lTasksInUkt = *eo70k_GetVarCntFromThrdInfo(pThrdInfo, pVarTask->lVarTaskType) ;
    if ( lTasksInUkt > 0 )
    {
      DBG3 (( MF__, "adding task '%d*%s' to UKT",lTasksInUkt, pVarTask->TaskTypeToken ));
      AddTaskStackSizeToUKTStackSize( pThrdInfo, pVarTask->lStackSize, pVarTask->tracePages, lTasksInUkt) ;
      pThrdInfo->ulNumOfTasks  += lTasksInUkt ;
      pVarTask->lActTaskCnt    += lTasksInUkt ;
      pVarTask->pLastFoundUKT   = pThrdInfo;
      pVarTask->lUKTsFilled     = 1;
    }
  }

  DBGOUT;
}

/*------------------------------*/

static void eo70_AddImpliciteVarTaskToUKT ( teo70k_ThrdInfo       *pThrdInfo,
                                            teo70k_VarTaskCntInfo *pVarTask,
                                            tsp00_Longuint         ulUKTBaseStackSize,
                                            tsp00_Uint             ulMaxCPU )
{
  #undef  MF__
  #define MF__ MOD__"eo70k_AddImpliciteVarTaskToUKT"

  tsp00_Int4 lTasksInUkt = 0;

  if ( pVarTask->lVarTaskType == VAR_TASK_US_EO70K
    || pVarTask->lVarTaskType == VAR_TASK_SV_EO70K )
  {
      if ( 1 < pVarTask->lLastFoundCnt )
      {
          lTasksInUkt = ( pVarTask->lMaxTaskCnt + ulMaxCPU - 1 - pVarTask->lUKTsFilled ) / ulMaxCPU;
      }
  }

  if ( 0 == lTasksInUkt )
  {
      lTasksInUkt = min (pVarTask->lLastFoundCnt, pVarTask->lMaxTaskCnt - pVarTask->lActTaskCnt );
  }

  if ( lTasksInUkt > 0 )
  {
    DBG3 (( MF__, "adding task '%d*%s' to UKT",lTasksInUkt, pVarTask->TaskTypeToken ));
    
    ++(pVarTask->lUKTsFilled);

    eo70_SetUKTVarTaskCnt( pThrdInfo, pVarTask, lTasksInUkt ) ;
 
    if ( pThrdInfo->ulStackSize == 0 ) /* initialize Thread once */
    {
      pThrdInfo->lThrdPrio          = pVarTask->pLastFoundUKT->lThrdPrio ;
      pThrdInfo->fFixedPriority     = pVarTask->pLastFoundUKT->fFixedPriority ;
      pThrdInfo->fDisablePreemption = pVarTask->pLastFoundUKT->fDisablePreemption ;
      pThrdInfo->ulStackSize        = ulUKTBaseStackSize ;
      pThrdInfo->tracePages         = 0;
    }
    AddTaskStackSizeToUKTStackSize( pThrdInfo, pVarTask->lStackSize, pVarTask->tracePages, lTasksInUkt) ;
    pThrdInfo->ulNumOfTasks  += lTasksInUkt;
    pVarTask->lActTaskCnt    += lTasksInUkt ;
  }

  DBGOUT;
}

/*------------------------------*/

static tsp00_Int4  eo70_CreateVarTaskUKTs ( teo70k_ClusterInfo     *pCluInfo,
                                            teo70k_VarTaskCntInfo  *pVarTasks,
                                            tsp00_Int4             *plThrdIdx,
                                            tsp00_Int4              lMaxUKTs,
                                            tsp00_Uint              ulMaxCPU )
  {
  #undef  MF__
  #define MF__ MOD__"eo70k_CreateVarTaskUKTs"

  int              iActVarTask, iNxtVarTask ;
  tsp00_Int4             lThrdIdx = *plThrdIdx;
  tsp00_Int4             rc       = NO_ERROR_EO001;
  teo70k_ThrdInfo *pThrdInfo ;
  teo70k_VarTaskCntInfo *pActVarTask, *pNxtVarTask ;

  DBGIN;

  for ( iActVarTask = 0;
        iActVarTask < MX_VAR_TASK_TYPES_EO70K && rc == NO_ERROR_EO001 ; 
        iActVarTask++ ) 
  {
    pActVarTask = pVarTasks + iActVarTask ;

    if (pActVarTask->lActTaskCnt < pActVarTask->lMaxTaskCnt )
    { 
      do
      {
        pThrdInfo = &pCluInfo->pThrdInfo[lThrdIdx] ;
        DBG3 (( MF__, "Filling UKT %d with additional (var) tasks", lThrdIdx));
        eo70_AddImpliciteVarTaskToUKT ( pThrdInfo, pActVarTask, pCluInfo->ulUKTBaseStackSize, ulMaxCPU ) ; 
        for ( iNxtVarTask = iActVarTask+1; iNxtVarTask < MX_VAR_TASK_TYPES_EO70K; iNxtVarTask++ )
        {
          pNxtVarTask = pVarTasks + iNxtVarTask ;
          if ( pNxtVarTask->pLastFoundUKT == pActVarTask->pLastFoundUKT && /* add other types ?? */
               pNxtVarTask->lActTaskCnt    < pNxtVarTask->lMaxTaskCnt )
          {
            eo70_AddImpliciteVarTaskToUKT ( pThrdInfo, pNxtVarTask, pCluInfo->ulUKTBaseStackSize, ulMaxCPU ) ; 
          }
        }
        lThrdIdx++ ;
        if ( lThrdIdx > lMaxUKTs )
        { 
          MSGALL(( ERR_TSK_CLST_TOO_COMPLEX ));
          DBG1  (( MF__, "TaskCluster too complex" ));
          rc = !NO_ERROR_EO001 ;
        }
      }
      while ( pActVarTask->lActTaskCnt < pActVarTask->lMaxTaskCnt && rc == NO_ERROR_EO001) ;
    }
  }
  *plThrdIdx = lThrdIdx ;

  DBGOUT;
  return ( rc );
}

/*------------------------------*/

static void  eo70_WrnActNotEqMax ( teo70k_VarTaskCntInfo  *pVarTasks ) 
{
  #undef  MF__
  #define MF__ MOD__"eo70k_WrnActNotEqMax"
  int     iVarTask ;

  DBGPAS;
   
  for ( iVarTask = 0; iVarTask < MX_VAR_TASK_TYPES_EO70K; iVarTask ++, pVarTasks++ ) 
    if ( pVarTasks->lActTaskCnt != pVarTasks->lMaxTaskCnt )
    {
      MSGD (( WRN_TSK_CLST_MAX_XX, 
               pVarTasks->TaskTypeToken ,
               pVarTasks->lActTaskCnt,
               pVarTasks->lMaxTaskCnt ));

      DBG1  (( MF__, "%d server specified - maxserver %d", 
               pVarTasks->TaskTypeToken ,
               pVarTasks->lActTaskCnt,
               pVarTasks->lMaxTaskCnt ));
    }
    else
    {
      MSGD(( INFO_TSK_CLST_NUM_XXX_TSK, 
              pVarTasks->TaskTypeToken, pVarTasks->lActTaskCnt ));
    }

  DBGOUT;
  return ;
}

/*------------------------------*/

static void  eo70_DebCluInfo ( teo70k_ClusterInfo  *pCluInfo ) 
{
  #undef  MF__
  #define MF__ MOD__"eo70k_DebCluInfo"

  tsp00_Uint4                       uldx;
  teo70k_ThrdInfo             *pThrdInfo;

  for ( uldx = 0; uldx < pCluInfo->ulNumOfUKTs ; uldx++ )
  {
    pThrdInfo = &pCluInfo->pThrdInfo[uldx] ;
    DBG3 (( MF__, "%2d: Number of Tasks %d \n", uldx, pThrdInfo->ulNumOfTasks ));
    DBG3 (( MF__, "%2d: Stack needed    %d \n", uldx, pThrdInfo->ulStackSize ));
  }

  DBGOUT;
  return ;
}

/*------------------------------*/

static bool    eo70_CheckPriority ( tsp00_Int4 lPrioToCheck )
{
  #undef  MF__
  #define MF__ MOD__"eo70k_CheckPriority"

  bool    fPrioOK ;

  DBGIN;

#ifdef WIN32 

  fPrioOK = lPrioToCheck == THREAD_PRIORITY_IDLE          ||
            lPrioToCheck == THREAD_PRIORITY_TIME_CRITICAL ||
            ( lPrioToCheck >= THREAD_PRIORITY_LOWEST  &&
              lPrioToCheck <= THREAD_PRIORITY_HIGHEST 
            ) ;

  if ( !fPrioOK )
  {
    DBG1  (( MF__, "Illegal thread priority '%d'", lPrioToCheck ));
    MSGD(( WRN_TSK_CLST_ILL_THRD_PRIO, lPrioToCheck ));
    MSGD(( WRN_TSK_CLST_ALLOWED_THRD_PRIO, THREAD_PRIORITY_IDLE, 
                                            THREAD_PRIORITY_TIME_CRITICAL,
                                            THREAD_PRIORITY_LOWEST,
                                            THREAD_PRIORITY_HIGHEST ));
  }

#else

  fPrioOK = lPrioToCheck >= -20 && lPrioToCheck <= 20 ; /* nice values */
  if ( !fPrioOK )
  {
    DBG1  (( MF__, "Illegal thread priority '%d'", lPrioToCheck ));
    MSGD(( WRN_TSK_CLST_ILL_THRD_PRIO   , lPrioToCheck ));
    MSGD(( WRN_TSK_CLST_ALLOWED_THRD_PRIO, -20, 20 ));
  }

#endif 

  if ( fPrioOK )
  {
    MSGD(( INFO_THRD_PRIO_OK, lPrioToCheck ));
  }

  DBGOUT;
  return ( fPrioOK ) ;
}


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
