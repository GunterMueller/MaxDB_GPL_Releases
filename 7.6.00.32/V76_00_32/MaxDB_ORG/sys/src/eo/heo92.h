/*!
  @file           heo92.h
  @author         JoergM
  @special area   x_cons
  @brief          common routines and definitions
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



#ifndef HEO92_H
#define HEO92_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "geo00_2.h"
#include "geo002.h"
#include "geo50_0.h"
#include "RunTime/Tasking/RTETask_SuspendReason.h"

#define SHOW_ALL_EO92        1
#define SHOW_ACTIVE_EO92     2
#define SHOW_RUNNABLE_EO92   3

#define SHOW_TT_ALL_EO92     2


#define IS_TASK_ACTIVE(_TaskType, _TaskState, _TaskStateReason, _OMSCallback, _TypeToShow)                      \
        (                                                                                                       \
            ( _TypeToShow == SHOW_TT_ALL_EO92 || _TaskType == _TypeToShow ) &&                                  \
            (                                                                                                   \
                (1 == _OMSCallback) ||   /* PTS 1116933 */                                                      \
                ( _TaskState != TSK_INACTIVE_EO00 && _TaskState != TSK_CONNECTWAIT_EO00 &&                      \
                  _TaskType != TT_TI_EO00 &&                                                                    \
                  ( _TaskType == TT_US_EO00 || _TaskState != TSK_VSUSPEND_EO00 ||                               \
                        (_TaskState == TSK_VSUSPEND_EO00 && _TaskStateReason != TSK_VSUSPEND_REASON_NO_WORK))&& \
                  ( _TaskState != TSK_VRECEIVE_EO00 ) && ( _TaskState != TSK_STOPPED_EO00 )                     \
                )                                                                                               \
            )                                                                                                   \
        )


#define IS_TASK_RUNNABLE(_TaskType, _TaskState, _InQueue, _TypeToShow) \
      ( _TaskState  == TSK_RUNNING_EO00 || _InQueue == TASK_IS_IN_RUN_QUEUE_EO00 ) && \
      ( _TypeToShow == SHOW_TT_ALL_EO92 || _TypeToShow == _TaskType )


#define IS_TASK_TO_SHOW_EO92(_TaskType, _TaskState, _TaskStateReason, _OMSCallback, _InQueue, _TypeToShow, _StateToShow) \
       ( ( (_StateToShow == SHOW_ALL_EO92 ) &&                                                                  \
           (_TypeToShow  == SHOW_TT_ALL_EO92     || _TypeToShow == _TaskType ) &&                               \
           ( ( _TaskState != TSK_INITIALIZED_EO00                                                               \
		    && _TaskState != TSK_INACTIVE_EO00                                                                  \
		    && _TaskState != TSK_CONNECTWAIT_EO00 ) || _TaskType == TT_UT_EO00 )                                \
         ) ||                                                                                                   \
         ( _StateToShow == SHOW_ACTIVE_EO92 &&                                                                  \
           IS_TASK_ACTIVE ( _TaskType, _TaskState, _TaskStateReason, _OMSCallback, _TypeToShow )                \
         ) ||                                                                                                   \
         ( _StateToShow == SHOW_RUNNABLE_EO92 &&                                                                \
           IS_TASK_RUNNABLE(_TaskType, _TaskState, _InQueue, _TypeToShow )                                      \
         )                                                                                                      \
       )


#define AVG_TIME( State ) State.ulMeasurements == 0 ? 0 : \
            ( (double)(tsp00_Int8)State.ulAbsMicroSeconds/1000000./ (double)State.ulMeasurements )

#define AVG_ABS_TIME( State ) State.ulMeasurements == 0 ? 0 : \
            ( (double)(tsp00_Int8)State.ulAbsMicroSeconds/1000000./ (double)State.ulMeasurements )

#define AVG_REL_TIME( State ) State.ulMeasurements == 0 ? 0 : \
            ( (double)(tsp00_Int8)State.ulRelMicroSeconds/1000000./ (double)State.ulMeasurements )

#define PRI_PG_PER_IO(_StrFormat, _Stat, _StrPg, _StrPgPerIO )            \
      if ( _Stat.IOStat.TimeStat.ulCount )                                                \
        printf ( _StrFormat, _StrPg, _Stat.ulPageCount,                   \
                 _StrPgPerIO,                                             \
                 (float ) _Stat.ulPageCount / (float ) _Stat.IOStat.TimeStat.ulCount ) ;  \

#define PRI_STAT_TM( _StrFormat, _Stat, _StrCount, _StrRelTm, _StrAbsTm ) \
      if ( _Stat.TimeStat.ulMeasurements && _Stat.TimeStat.ulAbsMicroSeconds)              \
        printf(_StrFormat, _StrCount, _Stat.TimeStat.ulMeasurements,              \
               _StrRelTm, AVG_REL_TIME( _Stat.TimeStat )  ,                        \
               _StrAbsTm, AVG_ABS_TIME( _Stat.TimeStat )  ) ;

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/


externC void          e92_print_param          ( RTE_XPARAM_REC *xparam_rec,
                                                 char           *pszParam   );

externC int           e92_set_new_param ( RTE_XPARAM_REC *XParam, 
                                        char           *param_id, 
                                        char           *param_val );

externC tsp00_Int4 
        eo92CreateTaskcluster (  UKT_CTRL           pUKTCtrl,
                                 char               *szCluster,
                                 tsp00_Int4          MaxClusterLen ) ;

/* PTS 1106969 */
externC void eo92PrintTaskOMSStatistics( teo52_TaskOMSStatistic *pTaskOMSStat ) ;

externC void eo92PrintTaskCounts       ( tsp00_Int4                TimeMeasurementActive,
                                         teo52_TaskStateStatistic *pTaskStateStat );
externC bool          e92_strcont              ( char       *pszString1,
                                                 char       *pszString2,
                                                 int         MinLgt );

externC tsp00_Bool    e92_get_tt_to_show       ( char           *param,
                                                 int            *tt_to_show ) ;

externC void          eo92_SecToDDHHMMSS       ( long  Sec, 
                                                 char *Buf,
                                                 int   len ); /* ddd:hh:mm:ss: */

externC char         *eo92GetTaskStateName     ( tsp00_Int     lState,
                                                 tsp00_Int     lStateReason );

externC char         *eo92GetTaskTypeName      ( tsp00_Uint1   TaskType ) ;
externC void          eo92_compress_path       ( char          *pszCompPathName,
                                                 char          *pszPathName,
                                                 tsp00_Uint     ulCompPathLen );

/*!
  @brief return the OMSCallbackMethodName for a given index
  @param MethodIdx [in] to check
  @param space [out] filled in case method index is invalid or points to a undefined entry...
  @return pointer to message text (always valid, never a NULL pointer...)
 */
typedef char eo92UnknownMethodName[40];
externC char const *eo92GetOMSCallbackMethodName( int MethodIdx, 
                                                  eo92UnknownMethodName VAR_ARRAY_REF spaceForUnknownMethodName ) ; /* PTS 1106969 */

externC char *eo92GetConnectStateName(int connState, char *buffer, int len);
externC char *eo92GetConnectServiceName(int connState, char *buffer, int len);
externC char *eo92GetConnectProtocolName(int connProtocol, char *buffer, int len);
externC char *eo92GetSwapTypeName(int swapType, char *buffer, int len);

#endif  /* HEO92_H */
