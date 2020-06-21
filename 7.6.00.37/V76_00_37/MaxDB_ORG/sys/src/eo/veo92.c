/*!
  @file           veo92.c
  @author         JoergM
  @special area   console(x_cons)
  @brief          common x_cons routines
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


//
//   FreeBSD portions added by Kai Mosebach,
//   For more informations see : http://www.komadev.de/sapdb
//

\endif
*/




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "geo00_2.h"
#include "heo00x.h"
#include "geo002.h"
#include "geo003.h"
#include "heo92.h"
#include "heo11.h"
#include "heo47.h"
#include "hsp77.h"
#include "RunTime/RTE_ConsoleStandardOutput.h"
#include "RunTime/Tasking/RTETask_Task.h"

#include "KernelCommon/ParameterNames/KernelParam_IOManager.hpp"
/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#if _WIN32
#define UKT_OF_TASK(_pCurrTask)        _pCurrTask->pUKT
#else

void     e92_print_param ( RTE_XPARAM_REC *XParam,
                           char           *name );
#define UKT_OF_TASK(_pCurrTask)        _pCurrTask->ukt

#endif

#define pri printf

int                             e92_set_new_param () ;
#ifndef FREEBSD
tsp00_Bool                      e92_strcont () ;
#endif

#define TYPE_STR  0
#define TYPE_BOOL 1
#define TYPE_INT4 4

#define SEC_PER_MIN  60
#define SEC_PER_HOUR 3600
#define SEC_PER_DAY  (24*SEC_PER_HOUR)


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

static int set_int_param( int   int_type ,
                          void *act_param,
                          char *new_param,
                          long  lower_limit,
                          long  upper_limit ) ;

static int set_bool_param( tsp00_Bool *act_param,
                           char       *new_param ) ;

static void e92_print_one_param ( char *param_id,
                                  char *param_wanted,
                                  int   wanted_len,
                                  void *value,
                                  char  type ) ;

/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

/* PTS 1106969 */

/* we have to define DEFINE_LVC_METHOD_TXT here, in order to define the array */
/* of method names in gco01_1.h                                 */
#define DEFINE_LVC_METHOD_TXT
#include "liveCache/LVC_MethodNames.hpp"

char const *eo92GetOMSCallbackMethodName( int MethodIdx, eo92UnknownMethodName spaceForUnknownMethodName )
{
	if ( MethodIdx > 0 && MethodIdx < LVC_MAX_LIVECACHE_SINK_METHODS && LiveCacheMethodText[MethodIdx] )
		return ( LiveCacheMethodText[MethodIdx]) ;
	else
	{
		sp77sprintf(spaceForUnknownMethodName, sizeof(eo92UnknownMethodName), "Unknown method: idx=%d", MethodIdx);
		return ( spaceForUnknownMethodName ) ;
	}
}

/*------------------------------*/

char *eo92GetTaskStateName ( tsp00_Int lState,
                             tsp00_Int lStateReason)
  {
  #undef  MF__
  #define MF__ MOD__"eo92GetTaskStateName"

  DBGPAS;

  switch ( lState )
    {
/* --- Values for kernel task states */
    case TSK_INITIALIZED_EO00: return XCONS_TSK_STATE_INITIALIZE;
    case TSK_INACTIVE_EO00: return XCONS_TSK_STATE_INACTIVE;
    case TSK_MARKED_FOR_STARTUP_EO00: return ( XCONS_TSK_STATE_MARKED_FOR_STRT);
    case TSK_RUNNING_EO00: return XCONS_TSK_STATE_RUNNING;
    case TSK_CONNECTWAIT_EO00: return XCONS_TSK_STATE_CONNECTWAIT;
    case TSK_VDEVSIZE_EO00: return XCONS_TSK_STATE_VDEVSIZE;
    case TSK_VATTACH_EO00: return XCONS_TSK_STATE_VATTACH;
    case TSK_VDETACH_EO00: return XCONS_TSK_STATE_VDETACH;
    case TSK_VFOPEN_EO00: return XCONS_TSK_STATE_VFOPEN;
    case TSK_VFWRITE_EO00: return XCONS_TSK_STATE_VFWRITE;
    case TSK_VFCLOSE_EO00: return XCONS_TSK_STATE_VFCLOSE;
    case TSK_VRELEASE_EO00: return XCONS_TSK_STATE_VRELEASE;
    case TSK_VSHUTDOWN_EO00: return XCONS_TSK_STATE_VSHUTDOWN;
    case TSK_VRECEIVE_EO00: return XCONS_TSK_STATE_VRECEIVE;
    case TSK_VREPLY_EO00: return XCONS_TSK_STATE_VREPLY;
    case TSK_VBEGEXCL_EO00: return XCONS_TSK_STATE_VBEGEXCL;
    case TSK_VENDEXCL_EO00: return XCONS_TSK_STATE_VENDEXCL;
    case TSK_VSUSPEND_EO00:
         if ( lStateReason > 0 )
           return (SuspendReasonText[lStateReason].szShortReason) ;
         else
           return ( XCONS_TSK_STATE_VSUSPEND       );
         break ;
    case TSK_VWAIT_EO00: return XCONS_TSK_STATE_VWAIT;
    case TSK_SLEEP_EO00: return XCONS_TSK_STATE_VSLEEP;
    case TSK_VDIAGINIT_EO00: return XCONS_TSK_STATE_VDIAGINIT;
    case TSK_VOPMSG_EO00: return XCONS_TSK_STATE_VOPMSG;
    case TSK_LOCKED_EO00: return XCONS_TSK_STATE_LOCKED;
    case TSK_TERMINATED_EO00: return XCONS_TSK_STATE_TERMINATED;
    case TSK_RUNNABLE_EO00: return XCONS_TSK_STATE_RUNNABLE;
    case TSK_ASYNOPEN_EO00: return XCONS_TSK_STATE_ASYNOPEN;
    case TSK_ASYNCLOSE_EO00: return XCONS_TSK_STATE_ASYNCLOSE;
    case TSK_ASYNIO_EO00: return XCONS_TSK_STATE_ASYNIO;
    case TSK_ASYNWAIT_READ_EO00: return XCONS_TSK_STATE_ASYNWAIT_READ;
    case TSK_ASYNWAIT_WRITE_EO00: return XCONS_TSK_STATE_ASYNWAIT_WRITE;
    case TSK_IO_READ_EO00:            return ( XCONS_TSK_STATE_IO_READ        );
    case TSK_IO_WRITE_EO00:           return ( XCONS_TSK_STATE_IO_WRITE       );
    case TSK_ASYNCNTL_EO00: return XCONS_TSK_STATE_ASYNCNTL;
    case TSK_DCOM_OBJ_CALLED_EO00:    return ( XCONS_TSK_STATE_DCOM_OBJ_CALLED);
    case TSK_RESCHEDULE_MSEC_EO00:    return ( XCONS_TSK_STATE_RESCHEDULE_MSEC);
    case TSK_INSERT_EVENT_EO00:       return ( XCONS_TSK_STATE_INSERT_EVENT );
    case TSK_WAIT_FOR_EVENT_EO00:     return ( XCONS_TSK_STATE_WAIT_FOR_EVENT );
    case TSK_STOPPED_EO00: return XCONS_TSK_STATE_STOPPED;
    case TSK_VVECTORIO_EO00: return XCONS_TSK_STATE_VVECTORIO;
    case TSK_VDUALVECTORIO_EO00: return XCONS_TSK_STATE_VDUALVECTORIO;
    case TSK_ENTERRWREGION_EO00:return ( XCONS_TSK_STATE_VENTERRWREGION );
    case TSK_LEAVERWREGION_EO00:return ( XCONS_TSK_STATE_VLEAVERWREGION );
    case TSK_YIELDING_ON_SPINLOCK_EO00:return ( XCONS_TSK_STATE_YIELDING_ON_SPINLOCK );

    default: break;
  }
  return ( XCONS_TSK_STATE_UNKNOWN );
  }

/*------------------------------*/

char *eo92GetTaskTypeName ( tsp00_Uint1 TaskType )
  {
  #undef  MF__
  #define MF__ MOD__"eo92GetTaskTypeName"

  DBGPAS;

  switch ( TaskType )
    {
    case TT_TI_EO00:     return ( RTE_TSK_TYPE_SHORTNAME_TI );
    case TT_AL_EO00:     return ( RTE_TSK_TYPE_SHORTNAME_AL );
    case TT_TW_EO00:     return ( RTE_TSK_TYPE_SHORTNAME_TW );
    case TT_DW_EO00:     return ( RTE_TSK_TYPE_SHORTNAME_DW );
    case TT_UT_EO00:     return ( RTE_TSK_TYPE_SHORTNAME_UT );
    case TT_SV_EO00:     return ( RTE_TSK_TYPE_SHORTNAME_SV );
    case TT_US_EO00:     return ( RTE_TSK_TYPE_SHORTNAME_US );
    case TT_EV_EO00:     return ( RTE_TSK_TYPE_SHORTNAME_EV );
    case TT_GC_EO00:     return ( RTE_TSK_TYPE_SHORTNAME_GC );
    case TT_BUP_EO00:    return ( RTE_TSK_TYPE_SHORTNAME_BUP );
    }

  return ( RTE_TSK_TYPE_SHORTNAME_UNKNOWN );
  }

/*------------------------------*/

void eo92_SecToDDHHMMSS( long  Sec,
                         char *Buf,
                         int len ) /* ddd:hh:mm:ss: */
{
#undef  MF__
#define MF__ MOD__"eo92_SecToDDHHMMSS"

    int  dd = Sec / SEC_PER_DAY;
    int  hh = ( Sec % SEC_PER_DAY  ) / SEC_PER_HOUR;
    int  mm = ( Sec % SEC_PER_HOUR ) / SEC_PER_MIN;
    int  ss = ( Sec % SEC_PER_MIN );

    if ( len < ((int)strlen("ddd:hh:mm:ss:") + 1) )
    {
        if ( len > 0 )
        {
            memset(Buf, 0, len);
        }
        return;
    }

    if ( dd > 0 )
    {
        sp77sprintf( Buf, len,
                     "%3d"  "%c" "%02d" "%c" "%02d" "%c" "%02d",
                       dd,  ':',    hh, ':',    mm, ':',    ss );
    } else if ( hh > 0 )
    {
        sp77sprintf( Buf, len,
                     "%3s"  "%c" "%02d" "%c" "%02d" "%c" "%02d",
                     "   ", ' ',    hh, ':',    mm, ':',    ss );
    } else if ( mm > 0 )
    {
        sp77sprintf( Buf, len,
                     "%3s"  "%c" "%2s" "%c" "%02d" "%c" "%02d",
                     "   ", ' ', "  ",  ' ',    mm, ':',    ss );
    } else if ( ss > 0 )
    {
        sp77sprintf( Buf, len,
                     "%3s"  "%c" "%2s"  "%c" "%2s"  "%c" "%02d",
                     "   ", ' ', "  ",  ' ', "  ",  ' ',    ss );
    } else
    {
        sp77sprintf( Buf, len,
                     "%3s"  "%c" "%2s"  "%c" "%2s"  "%c" "%2s",
                     "   ", ' ', "  ",  ' ', "  ",  ' ', "  " );
    }
}

/*================================================*/
#ifndef FREEBSD
tsp00_Bool e92_strcont ( char *string1 ,
                         char *string2 ,
                         int   minlgt )
#else
bool e92_strcont ( char *string1 ,
                         char *string2 ,
                         int   minlgt )
#endif
/*================================================*/
{
#undef  MF__
#define MF__ MOD__"e92_strcont"
        int                     i , lgt ;

        /*
         *  Does string1 start with string2 of at least minlgt characters ?
         */
        if ( (int) strlen(string1) < (lgt = (int)strlen(string2)) ) return ( FALSE );
        if ( lgt < minlgt ) return ( FALSE );
        if ( minlgt == 0  ) return ( TRUE );

        for ( i = 0 ; i < lgt ; i ++ )
        {
            if ( string1 [ i ] != string2 [ i ] ) break ;
        }
        if ( i < lgt ) return ( FALSE );
        return ( TRUE );
}


#define SHOW_UNKNOWN    0
/*================================================*/
tsp00_Bool     e92_get_tt_to_show (char *active_param ,
                                   int *s_task_type )
/*================================================*/
{
    int         i;
    tsp00_Bool  rc = true;

    for ( i = 0 ; active_param [ i ] != '\0' ; i ++ )
      if ( islower ( active_param [ i ] ) )
         active_param [ i ] = active_param [ i ] - 'a' + 'A' ;

    if ( e92_strcont ( "DW"   , active_param , 2 ) )
      *s_task_type  = TT_DW_EO00 ;
    else
    if ( e92_strcont ( "SV"   , active_param , 2 ) )
      *s_task_type  = TT_SV_EO00 ;
    else
    if ( e92_strcont ( "US"   , active_param , 2 ) )
      *s_task_type  = TT_US_EO00 ;
    else
    if ( e92_strcont ( "GC"   , active_param , 2 ) ) /* PTS 1107070 */
      *s_task_type  = TT_GC_EO00 ;
    else
      rc = false;

    return (rc) ;
}


/*------------------------------*/

/*===========================================================================*/
#ifdef CONSOLE
/*===========================================================================*/

void eo92PrintTaskOMSStatistics( teo52_TaskOMSStatistic *pTaskOMSStat )
{
	if ( *(pTaskOMSStat->ProcedurName) )
		printf (XCONS_SHOW_TASK_COUNT_OMS_PROCEDURE,
		          XCONS_SHOW_TASK_COUNT_OMS_PROCEDURE_NAME,
		          pTaskOMSStat->ProcedurName);


    if ( pTaskOMSStat->CallbackCount )
    {
        eo92UnknownMethodName spaceForUnknownMethodName;
        SAPDB_Char const * methodName = "";

        if ( pTaskOMSStat->OMSCallActive )
        {
            SAPDB_Int4 methodIdx = pTaskOMSStat->MethodIdx;
            if ( methodIdx > 0 )
            {
                methodName = eo92GetOMSCallbackMethodName(methodIdx, spaceForUnknownMethodName );
            }
        }
        printf (XCONS_SHOW_TASK_COUNT_OMS_CALLBACK_STAT,
		          XCONS_SHOW_TASK_COUNT_OMS_CALLBACK_METHOD,
		          methodName,
                  XCONS_SHOW_TASK_COUNT_OMS_CALLBACK_CNT,
				  pTaskOMSStat->CallbackCount);
    }
}

/*------------------------------*/

void eo92PrintTaskCounts( tsp00_Int4                TimeMeasurementActive,
                          teo52_TaskStateStatistic *pTaskStateStat )
{
  if ( ! TimeMeasurementActive )
    {
      if ( pTaskStateStat->AsynIOReadStat.IOStat.TimeStat.ulCount )
        printf ( XCONS_SHOW_TASK_COUNT_ASYN_CNT_PG ,
                       XCONS_SHOW_TASK_COUNT_KT_ASYN_R_CNT ,
                       pTaskStateStat->AsynIOReadStat.IOStat.TimeStat.ulCount   ,
                       XCONS_SHOW_TASK_COUNT_KT_ASYN_R_PG ,
                       pTaskStateStat->AsynIOReadStat.ulPageCount);

      if ( pTaskStateStat->AsynIOWriteStat.IOStat.TimeStat.ulCount )
        printf ( XCONS_SHOW_TASK_COUNT_ASYN_CNT_PG ,
                       XCONS_SHOW_TASK_COUNT_KT_ASYN_W_CNT ,
                       pTaskStateStat->AsynIOWriteStat.IOStat.TimeStat.ulCount   ,
                       XCONS_SHOW_TASK_COUNT_KT_ASYN_W_PG  ,
                       pTaskStateStat->AsynIOWriteStat.ulPageCount);

      if ( pTaskStateStat->SelfIOReadStat.IOStat.TimeStat.ulCount)
        printf ( XCONS_SHOW_TASK_COUNT_IO_CNT ,
                       XCONS_SHOW_TASK_COUNT_KT_SELF_R_CNT ,
                       pTaskStateStat->SelfIOReadStat.IOStat.TimeStat.ulCount   ,
                       XCONS_SHOW_TASK_COUNT_KT_SELF_R_PG  ,
                       pTaskStateStat->SelfIOReadStat.ulPageCount );

      if ( pTaskStateStat->SelfIOWriteStat.IOStat.TimeStat.ulCount )
        printf ( XCONS_SHOW_TASK_COUNT_IO_CNT ,
                       XCONS_SHOW_TASK_COUNT_KT_SELF_W_CNT ,
                       pTaskStateStat->SelfIOWriteStat.IOStat.TimeStat.ulCount,
                       XCONS_SHOW_TASK_COUNT_KT_SELF_W_PG ,
                       pTaskStateStat->SelfIOWriteStat.ulPageCount);

      if ( pTaskStateStat->DevIOReadStat.IOStat.TimeStat.ulCount )
        printf ( XCONS_SHOW_TASK_COUNT_IO_CNT ,
                       XCONS_SHOW_TASK_COUNT_KT_DEV_R_CNT  ,
                       pTaskStateStat->DevIOReadStat.IOStat.TimeStat.ulCount,
                       XCONS_SHOW_TASK_COUNT_KT_DEV_R_PG ,
                       pTaskStateStat->DevIOReadStat.ulPageCount);

      if ( pTaskStateStat->DevIOWriteStat.IOStat.TimeStat.ulCount )
        printf ( XCONS_SHOW_TASK_COUNT_IO_CNT ,
                       XCONS_SHOW_TASK_COUNT_KT_DEV_W_CNT  ,
                       pTaskStateStat->DevIOWriteStat.IOStat.TimeStat.ulCount,
                       XCONS_SHOW_TASK_COUNT_KT_DEV_W_PG ,
                       pTaskStateStat->DevIOWriteStat.ulPageCount);

      printf ( XCONS_SHOW_TASK_COUNT_WAIT_SLEEP_SUSP_CNT  ,
                   XCONS_SHOW_TASK_COUNT_KT_STATE_VWAIT_CNT  ,
                   pTaskStateStat->VwaitStat.TimeStat.ulCount,
                   XCONS_SHOW_TASK_COUNT_KT_STATE_VSLEEP_CNT ,
                   pTaskStateStat->VsleepStat.TimeStat.ulCount,
                   XCONS_SHOW_TASK_COUNT_KT_STATE_VSUSP_CNT  ,
                   pTaskStateStat->VsuspStat.TimeStat.ulCount ) ;
    }
  else
    {
      PRI_STAT_TM (  XCONS_SHOW_TASK_COUNT_STATE_CNT_TIME,
                     pTaskStateStat->AsynIOReadStat.IOStat,
                     XCONS_SHOW_TASK_COUNT_KT_ASYN_R_CNT  ,
                     XCONS_SHOW_TASK_COUNT_KT_ASYN_R_REL_TIME,
                     XCONS_SHOW_TASK_COUNT_KT_ASYN_R_ABS_TIME ) ;

      PRI_PG_PER_IO( XCONS_SHOW_TASK_COUNT_PG_PER_IO,
                     pTaskStateStat->AsynIOReadStat,
                     XCONS_SHOW_TASK_COUNT_KT_ASYN_R_PG,
                     XCONS_SHOW_TASK_COUNT_KT_PG_PER_IO ) ;

      PRI_STAT_TM  ( XCONS_SHOW_TASK_COUNT_STATE_CNT_TIME,
                     pTaskStateStat->AsynIOWriteStat.IOStat,
                     XCONS_SHOW_TASK_COUNT_KT_ASYN_W_CNT  ,
                     XCONS_SHOW_TASK_COUNT_KT_ASYN_W_REL_TIME,
                     XCONS_SHOW_TASK_COUNT_KT_ASYN_W_ABS_TIME ) ;

      PRI_PG_PER_IO( XCONS_SHOW_TASK_COUNT_PG_PER_IO,
                     pTaskStateStat->AsynIOWriteStat,
                     XCONS_SHOW_TASK_COUNT_KT_ASYN_W_PG,
                     XCONS_SHOW_TASK_COUNT_KT_PG_PER_IO ) ;

      if ( pTaskStateStat->SelfIOReadStat.IOStat.TimeStat.ulMeasurements &&
           pTaskStateStat->SelfIOReadStat.IOStat.TimeStat.ulAbsMicroSeconds )
        printf ( XCONS_SHOW_TASK_COUNT_IO_CNT_PG_TIME ,
                     XCONS_SHOW_TASK_COUNT_KT_SELF_R_CNT ,
                     pTaskStateStat->SelfIOReadStat.IOStat.TimeStat.ulCount ,
                     XCONS_SHOW_TASK_COUNT_KT_SELF_R_PG  ,
                     pTaskStateStat->SelfIOReadStat.ulPageCount   ,
                     XCONS_SHOW_TASK_COUNT_KT_SELF_R_AVG_TIME ,
                     AVG_TIME( pTaskStateStat->SelfIOReadStat.IOStat.TimeStat ) ) ;

      if ( pTaskStateStat->SelfIOWriteStat.IOStat.TimeStat.ulMeasurements &&
           pTaskStateStat->SelfIOWriteStat.IOStat.TimeStat.ulAbsMicroSeconds )
        printf ( XCONS_SHOW_TASK_COUNT_IO_CNT_PG_TIME    ,
                     XCONS_SHOW_TASK_COUNT_KT_SELF_W_CNT ,
                     pTaskStateStat->SelfIOWriteStat.IOStat.TimeStat.ulCount   ,
                     XCONS_SHOW_TASK_COUNT_KT_SELF_W_PG  ,
                     pTaskStateStat->SelfIOWriteStat.ulPageCount   ,
                     XCONS_SHOW_TASK_COUNT_KT_SELF_W_AVG_TIME ,
                     AVG_TIME( pTaskStateStat->SelfIOWriteStat.IOStat.TimeStat ) ) ;

      PRI_STAT_TM  ( XCONS_SHOW_TASK_COUNT_STATE_CNT_TIME,
                     pTaskStateStat->DevIOReadStat.IOStat,
                     XCONS_SHOW_TASK_COUNT_KT_DEV_R_CNT  ,
                     XCONS_SHOW_TASK_COUNT_KT_DEV_R_REL_TIME,
                     XCONS_SHOW_TASK_COUNT_KT_DEV_R_ABS_TIME ) ;

      PRI_PG_PER_IO( XCONS_SHOW_TASK_COUNT_PG_PER_IO,
                     pTaskStateStat->DevIOReadStat,
                     XCONS_SHOW_TASK_COUNT_KT_DEV_R_PG,
                     XCONS_SHOW_TASK_COUNT_KT_PG_PER_IO ) ;

      PRI_STAT_TM  ( XCONS_SHOW_TASK_COUNT_STATE_CNT_TIME,
                     pTaskStateStat->DevIOWriteStat.IOStat,
                     XCONS_SHOW_TASK_COUNT_KT_DEV_W_CNT  ,
                     XCONS_SHOW_TASK_COUNT_KT_DEV_W_REL_TIME,
                     XCONS_SHOW_TASK_COUNT_KT_DEV_W_ABS_TIME ) ;

      PRI_PG_PER_IO( XCONS_SHOW_TASK_COUNT_PG_PER_IO,
                     pTaskStateStat->DevIOWriteStat,
                     XCONS_SHOW_TASK_COUNT_KT_DEV_W_PG,
                     XCONS_SHOW_TASK_COUNT_KT_PG_PER_IO ) ;

      PRI_STAT_TM  ( XCONS_SHOW_TASK_COUNT_STATE_CNT_TIME,
                     pTaskStateStat->VwaitStat,
                     XCONS_SHOW_TASK_COUNT_KT_STATE_VWAIT_CNT  ,
                     XCONS_SHOW_TASK_COUNT_KT_VWAIT_REL_TIME,
                     XCONS_SHOW_TASK_COUNT_KT_VWAIT_ABS_TIME ) ;

      PRI_STAT_TM  ( XCONS_SHOW_TASK_COUNT_STATE_CNT_TIME,
                     pTaskStateStat->VsuspStat,
                     XCONS_SHOW_TASK_COUNT_KT_STATE_VSUSP_CNT  ,
                     XCONS_SHOW_TASK_COUNT_KT_VSUSP_REL_TIME,
                     XCONS_SHOW_TASK_COUNT_KT_VSUSP_ABS_TIME ) ;

      PRI_STAT_TM  ( XCONS_SHOW_TASK_COUNT_STATE_CNT_TIME,
                     pTaskStateStat->VsleepStat,
                     XCONS_SHOW_TASK_COUNT_KT_STATE_VSLEEP_CNT  ,
                     XCONS_SHOW_TASK_COUNT_KT_VSLEEP_REL_TIME,
                     XCONS_SHOW_TASK_COUNT_KT_VSLEEP_ABS_TIME ) ;

    }

  if ( pTaskStateStat->RcvRplStat.TimeStat.ulCount )
    {
      if ( pTaskStateStat->RcvRplStat.TimeStat.ulCount )
        printf ( XCONS_SHOW_TASK_COUNT_RCV_RPL_INFO ,
                     XCONS_SHOW_TASK_COUNT_KT_RCV_RPL_CNT,
                     pTaskStateStat->RcvRplStat.TimeStat.ulCount,
                     XCONS_SHOW_TASK_COUNT_KT_RCV_RPL_TO_LONG,
                     pTaskStateStat->ulRcvRplToLong ,
                     XCONS_SHOW_TASK_COUNT_KT_RCV_RPL_AVG_TIME,
                     AVG_TIME( pTaskStateStat->RcvRplStat.TimeStat )  ) ;

      PRI_STAT_TM  ( XCONS_SHOW_TASK_COUNT_STATE_CNT_TIME,
                     pTaskStateStat->RplRcvStat,
                     XCONS_SHOW_TASK_COUNT_KT_RPL_RCV_CNT  ,
                     XCONS_SHOW_TASK_COUNT_KT_RPL_RCV_REL_TIME,
                     XCONS_SHOW_TASK_COUNT_KT_RPL_RCV_ABS_TIME ) ;
    }
}

/*------------------------------*/

int e92_set_new_param ( RTE_XPARAM_REC *XParam,
                        char           *param_id,
                        char           *param_val )
{
#undef  MF__
#define MF__ MOD__"e92_set_new_param"

int                             set_ok ;

if ( *param_id == 'h' && *param_val == 'h' )
  { set_ok = FALSE ;
    pri ( "\nTunable parameters:\n\n" ) ;
    e92_print_param ( XParam, PAN_DELAY_LW ) ;
    e92_print_param ( XParam, PAN_USE_IOTHREAD_ONLY ) ;
    e92_print_param ( XParam, PAN_MP_RGN_LOOP ) ;
    e92_print_param ( XParam, PAN_MP_RGN_PRIO ) ;
    e92_print_param ( XParam, PAN_MP_DISP_PRIO ) ;
    e92_print_param ( XParam, PAN_MP_RGN_BUSY_WAIT ) ;
    e92_print_param ( XParam, PAN_MP_DISP_LOOPS ) ;
    e92_print_param ( XParam, PAN_PRIO_BASE_U2U ) ;
    e92_print_param ( XParam, PAN_PRIO_BASE_IOC ) ;
    e92_print_param ( XParam, PAN_PRIO_BASE_RAV ) ;
    e92_print_param ( XParam, PAN_PRIO_BASE_REX ) ;
    e92_print_param ( XParam, PAN_PRIO_BASE_COM ) ;
    e92_print_param ( XParam, PAN_PRIO_FACTOR   ) ;
  }
else if ( ! strcmp( param_id , PAN_PRIO_BASE_U2U  ) )
  set_ok = set_int_param ( TYPE_INT4, &XParam->lPrioBaseU2U, param_val, 0, 10000 ) ;

else if ( ! strcmp( param_id , PAN_PRIO_BASE_RAV  ) )
  set_ok = set_int_param ( TYPE_INT4, &XParam->lPrioBaseRAV, param_val, 0, 10000 ) ;

else if ( ! strcmp( param_id , PAN_PRIO_BASE_IOC  ) )

  set_ok = set_int_param ( TYPE_INT4, &XParam->lPrioBaseIOC, param_val, 0, 10000 ) ;

else if ( ! strcmp( param_id , PAN_PRIO_BASE_COM  ) )

  set_ok = set_int_param ( TYPE_INT4, &XParam->lPrioBaseCOM, param_val, 0, 10000 ) ;
else if ( ! strcmp( param_id , PAN_PRIO_BASE_REX  ) )

  set_ok = set_int_param ( TYPE_INT4, &XParam->lPrioBaseREX, param_val, 0, 10000 ) ;
else if ( ! strcmp( param_id , PAN_PRIO_FACTOR  ) )
  set_ok = set_int_param ( TYPE_INT4, &XParam->lPrioFactor, param_val, 0, 10000 ) ;


else if ( ! strcmp( param_id , PAN_DELAY_LW ) )
  set_ok = set_int_param ( TYPE_INT4, &XParam->lDelayLW , param_val, 0, 100 ) ;

else if ( ! strcmp( param_id , PAN_DELAY_COMMIT ) )
  set_ok = set_bool_param ( &XParam->fDelayCommit , param_val ) ;

else if ( ! strcmp( param_id , PAN_USE_IOTHREAD_ONLY ) )
  set_ok = set_bool_param ( &XParam->fAllowSingleIO , param_val ) ;

else if ( ! strcmp( param_id , PAN_MP_RGN_PRIO ) )
  set_ok = set_bool_param ( &XParam->fBegExclQueWhenPrio , param_val ) ;

else if ( ! strcmp( param_id , PAN_MP_DISP_PRIO ) )
  set_ok = set_bool_param ( &XParam->fDynDispQueSrch , param_val ) ;

else if ( ! strcmp( param_id , PAN_MP_RGN_DIRTY_READ ) )
  set_ok = set_bool_param ( &XParam->fBegExclDirtyRead , param_val ) ;

else if ( ! strcmp( param_id , PAN_MP_RGN_BUSY_WAIT ) )
  set_ok = set_bool_param ( &XParam->fEndExclBusyWait , param_val ) ;

else if ( ! strcmp( param_id , PAN_MAXRGN_REQUEST ) )
  set_ok = set_int_param ( TYPE_INT4, &XParam->ulRegLockSlice ,
                                      param_val, 25, 100000 ) ;

else if ( ! strcmp( param_id , PAN_MP_DISP_LOOPS ) )
  set_ok = set_int_param ( TYPE_INT4, &XParam->ulDispLoopsBefSemop ,
                                      param_val, 2, 1000000 ) ;

else if ( ! strcmp( param_id , PAN_MP_RGN_LOOP ) )
  if ( XParam->ulMaxCPU > 1 )
    set_ok = set_int_param ( TYPE_INT4, &XParam->ulRegionCollisionLoop,
                                        param_val, 1, 100000 ) ;
else
  { set_ok = FALSE ;
    pri ( "putparam for parameter '%s' not allowed!\n", param_id ) ;
  }
  return( set_ok ) ;
}

/*------------------------------*/
void     e92_print_param ( RTE_XPARAM_REC *XParam,
                           char           *name )
{
#undef  MF__
#define MF__ MOD__"e92_print_param"

int                             name_len = (int)strlen(name) ;
tsp00_Bool                      fUserIoThreadsOnly;

DBGPAS;

#define PRINT_ONE_PARAM( param_id, param_val, param_type ) \
    e92_print_one_param ( param_id, name , name_len,  param_val , param_type ) ;

PRINT_ONE_PARAM ( PAN_PARAM_KERNELVERSION, XParam->szKernelVersion, TYPE_STR ) ;

PRINT_ONE_PARAM ( PAN_MAXDEVSPACES ,     &XParam->lMaxDevspaces , TYPE_INT4 );

PRINT_ONE_PARAM ( PAN_LOG_MIRRORED     , &XParam->fLogMirrored, TYPE_BOOL );

PRINT_ONE_PARAM ( PAN_MIRR_DATA        , &XParam->fMirrData, TYPE_BOOL);

PRINT_ONE_PARAM ( PAN_MAXBACKUPDEVS    , &XParam->lNoOfBackupDevs , TYPE_INT4 );

PRINT_ONE_PARAM ( PAN_MAXACTIVE ,        &XParam->ulMaxUser , TYPE_INT4 );

PRINT_ONE_PARAM ( PAN_MAXSERVER ,        &XParam->ulMaxServer , TYPE_INT4 );

PRINT_ONE_PARAM ( PAN_DIAG_SIZE ,        &XParam->ulDiagSize , TYPE_INT4 );

PRINT_ONE_PARAM ( PAN_EVENTFILE  ,        XParam->szEventFile, TYPE_STR );

PRINT_ONE_PARAM ( PAN_EVENT_SIZE ,       &XParam->ulEventSize , TYPE_INT4 );

PRINT_ONE_PARAM ( PAN_MAXEVENTS ,        &XParam->ulMaxEvents , TYPE_INT4 );

PRINT_ONE_PARAM ( PAN_RUNDIR ,           XParam->szRunDirectory , TYPE_STR );

PRINT_ONE_PARAM ( PAN_KERNELDUMPFILE  ,  XParam->szDumpFilename , TYPE_STR );

PRINT_ONE_PARAM ( PAN_KERNELTRACEFILE ,  XParam->szTraceDevspace, TYPE_STR );

PRINT_ONE_PARAM ( PAN_KERNELTRACESIZE , &XParam->lTraceDevspaceSize, TYPE_INT4 );

fUserIoThreadsOnly = !XParam->fAllowSingleIO ;
PRINT_ONE_PARAM ( PAN_USE_IOTHREAD_ONLY , &fUserIoThreadsOnly , TYPE_BOOL );

PRINT_ONE_PARAM ( PAN_MAXIOTHREADS ,    &XParam->lDeviPerDevspace , TYPE_INT4 );

PRINT_ONE_PARAM ( PAN_IOTHREADS_FOR_PRIO, &XParam->lPrioDeviPerDevspace, TYPE_INT4 );

PRINT_ONE_PARAM ( PAN_DEV_QUEUE ,         &XParam->lDevQueue , TYPE_INT4 );

PRINT_ONE_PARAM ( PAN_MAXCPU ,            &XParam->ulMaxCPU , TYPE_INT4 );

PRINT_ONE_PARAM ( PAN_PRIO_BASE_U2U ,     &XParam->lPrioBaseU2U , TYPE_INT4 );

PRINT_ONE_PARAM ( PAN_PRIO_BASE_IOC ,     &XParam->lPrioBaseIOC , TYPE_INT4 );

PRINT_ONE_PARAM ( PAN_PRIO_BASE_RAV ,     &XParam->lPrioBaseRAV , TYPE_INT4 );

PRINT_ONE_PARAM ( PAN_PRIO_BASE_COM ,     &XParam->lPrioBaseCOM , TYPE_INT4 );

PRINT_ONE_PARAM ( PAN_PRIO_BASE_REX ,     &XParam->lPrioBaseREX , TYPE_INT4 );

PRINT_ONE_PARAM ( PAN_PRIO_FACTOR   ,     &XParam->lPrioFactor  , TYPE_INT4 );

PRINT_ONE_PARAM ( PAN_MP_RGN_PRIO,        &XParam->fBegExclQueWhenPrio, TYPE_BOOL);

PRINT_ONE_PARAM ( PAN_MP_RGN_QUEUE,       &XParam->fExclQueue, TYPE_BOOL);

PRINT_ONE_PARAM ( PAN_MP_DISP_PRIO ,      &XParam->fDynDispQueSrch ,TYPE_BOOL);

PRINT_ONE_PARAM ( PAN_MP_RGN_DIRTY_READ , &XParam->fBegExclDirtyRead ,TYPE_BOOL);

PRINT_ONE_PARAM ( PAN_MP_RGN_BUSY_WAIT ,  &XParam->fEndExclBusyWait ,TYPE_BOOL);

PRINT_ONE_PARAM ( PAN_MP_DISP_LOOPS ,     &XParam->ulDispLoopsBefSemop,TYPE_INT4);

PRINT_ONE_PARAM ( PAN_MP_RGN_LOOP,        &XParam->ulRegionCollisionLoop,TYPE_INT4);

PRINT_ONE_PARAM ( PAN_MAXRGN_REQUEST ,    &XParam->ulRegLockSlice ,TYPE_INT4);

PRINT_ONE_PARAM ( PAN_DELAY_LW ,          &XParam->lDelayLW , TYPE_INT4 );

PRINT_ONE_PARAM ( PAN_DELAY_COMMIT ,      &XParam->fDelayCommit , TYPE_INT4 );

PRINT_ONE_PARAM ( PAN_MAXUSERSTACK ,      &XParam->ulMaxUsUtStack ,TYPE_INT4);

PRINT_ONE_PARAM ( PAN_MAXDATAWRITER ,     &XParam->ulMaxDataWriter ,TYPE_INT4);

/* PRINT_ONE_PARAM ( PAN_CONTROLID         , XParam->szControlUser     ,TYPE_STR); */

PRINT_ONE_PARAM ( PAN_AK_DUMP_ALLOWED ,  &XParam->fAKDumpAllowed , TYPE_BOOL);

PRINT_ONE_PARAM ( PAN_XPARAM_CHANGED ,   &XParam->ulXparamChanged, TYPE_INT4 );

PRINT_ONE_PARAM ( PAN_MAXEVENTTASKS , &XParam->ulMaxEventTasks , TYPE_INT4 );

PRINT_ONE_PARAM ( PAN_MAX_BACKUP_TASKS , &XParam->ulMaxBackupTasks , TYPE_INT4 );

PRINT_ONE_PARAM ( PAN_MAXGARBAGE_COLLECTOR , &XParam->ulMaxGarbageCollector , TYPE_INT4 );

/******************************/
#if _WIN32 || OS2
/******************************/

PRINT_ONE_PARAM ( PAN_FORMAT_DATAVOLUME   , &XParam->fFormatDataVolume, TYPE_BOOL);

PRINT_ONE_PARAM ( PAN_DYNAMIC_SERVER      , &XParam->fDynamicServerTasks, TYPE_BOOL);

PRINT_ONE_PARAM ( PAN_USE_MEM_ENHANCEMENT,  &XParam->fUseMemEnhancement, TYPE_BOOL);
PRINT_ONE_PARAM ( PAN_MEM_ENHANCEMENT_LIM,  &XParam->lMemEnhancementLimit, TYPE_INT4);

/******************************/
#else
/******************************/
PRINT_ONE_PARAM ( PAN_OPMSG1 , XParam->szOpMsg1, TYPE_STR );

PRINT_ONE_PARAM ( PAN_OPMSG2 , XParam->szOpMsg2, TYPE_STR );

#endif
return;
}

/*------------------------------*/

void eo92_compress_path ( char           *pszCompPathName,
                          char           *pszPathName,
                          tsp00_Uint      ulCompPathLen )
  {
  #undef  MF__
  #define MF__ MOD__"eo92_compress_path"
  tsp00_Uint               lPathLen;
  tsp00_Uint               lSplitPos;
  tsp00_Uint               lRest;

  DBGPAS;

  if ( ulCompPathLen < 4 )
    {
    pszCompPathName[0] = '\0';
    return;
    }

  lPathLen = (tsp00_Uint)strlen ( pszPathName );

  if ( lPathLen > (int)ulCompPathLen )
    {
    lSplitPos = (ulCompPathLen / 2) - 2;
    lRest     = ulCompPathLen - lSplitPos - 4;

    strncpy ( pszCompPathName, pszPathName, lSplitPos );
    pszCompPathName [ lSplitPos ] = '\0';
    strcat ( pszCompPathName, "...." );
    strcat ( pszCompPathName, pszPathName + lPathLen - lRest );
    }
  else
    {
    strcpy ( pszCompPathName, pszPathName );
    }

  return;
  }

/*===========================================================================*
 *  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
 *===========================================================================*/

static void e92_print_one_param ( char *param_id,
                                  char *param_wanted,
                                  int   wanted_len,
                                  void *value,
                                  char  type )
{
  #undef  MF__
  #define MF__ MOD__"e92_print_one_param"

  DBGPAS;

  if ( e92_strcont ( param_id, param_wanted , wanted_len ) )
    switch ( type )
    { case TYPE_STR  : pri ( "%-20s: %s\n", param_id ,
                           value ? (char* )value : "" ) ;
                 break ;
      case TYPE_INT4 : pri ( "%-20s: %d\n", param_id , *(tsp00_Int4 *)value ) ;
                 break ;
      case TYPE_BOOL : pri ( "%-20s: %s\n", param_id , *(teo00_Bool *)value ? "YES" : "NO" ) ;
                 break ;
    }
 return ;
}

/*------------------------------*/

static int set_bool_param( tsp00_Bool *act_param,
                           char *new_param )
{
#undef  MF__
#define MF__ MOD__"set_bool_param"
  int set_ok  = *new_param == 'y' || *new_param == 'Y'
             || *new_param == 'n' || *new_param == 'N' ;

  if ( set_ok )
    *act_param = *new_param == 'y' || *new_param == 'Y' ;
  else
    pri ( "parameter out of range! ( use : y | Y | n | N ) \n" );

  return ( set_ok ) ;
}

/*------------------------------*/

static int set_int_param( int   int_type ,
                          void *act_param,
                          char *new_param,
                          long  lower_limit,
                          long  upper_limit )
{
#undef  MF__
#define MF__ MOD__"set_int_param"

  long new_val = atol ( new_param ) ;
  int set_ok  = /* new_val != *act_param && */
                new_val >= lower_limit && new_val <= upper_limit ;

  if ( set_ok )
     *( (tsp00_Int4 *) act_param ) = (tsp00_Int4 ) new_val ;
  else
    { pri ( "parameter out of range! ( use : %d ... %d ) \n",
                       lower_limit, upper_limit );
    }
  return ( set_ok ) ;
}
/*===========================================================================*/
#endif /* KERNEL */
/*===========================================================================*/

/*------------------------------*/

static void eo92_AddToClusterString(SAPDB_Bool *pMustSeparateByComma,
                                    const char *stringToAdd,
                                    char       *szCluster,
                                    SAPDB_Int4  MaxClusterLen)
{
    SAPDB_Int4 clusterLen = (SAPDB_Int4)strlen(szCluster);

    if ( *pMustSeparateByComma )
    {
        if ( (MaxClusterLen-clusterLen) > 1 )
        {
            szCluster[clusterLen++] = ',';
        }
    }

    /* If not enough space left, zero terminate the entry only */
    if ( (MaxClusterLen-clusterLen-strlen(stringToAdd)) > 1 )
    {
        strcpy(&szCluster[clusterLen], stringToAdd);
        *pMustSeparateByComma = true;
    }
    else
    {
        szCluster[clusterLen] = 0;
    }
}

/*------------------------------*/

static void eo92ExpandMultipleTasks ( SAPDB_Bool *pMustSeparateByComma,
                                      tsp00_Int4       NumTasks,
                                      char            *szTaskToken,
                                      char            *szCluster,
                                      tsp00_Int4      MaxClusterLen )

{
#undef  MF__
#define MF__ MOD__"eo92ExpandMultipleTasks"

    if (NumTasks)
    {
        char  szNumOfXXTasks[20];
        SAPDB_Bool needExtraComma;

        sql47_ltoa( (long )NumTasks, szNumOfXXTasks, sizeof(szNumOfXXTasks) );

        eo92_AddToClusterString( pMustSeparateByComma,
                                 szNumOfXXTasks,
                                 szCluster,
                                 MaxClusterLen );

        needExtraComma = false;  /* just append... */
        eo92_AddToClusterString( &needExtraComma,
                                 szTaskToken,
                                 szCluster,
                                 MaxClusterLen );
    }
}

/*------------------------------*/

externC tsp00_Int4
        eo92CreateTaskcluster (  UKT_CTRL            pUKTCtrl,
                                 char               *szCluster,
                                 tsp00_Int4          MaxClusterLen )
{
#undef  MF__
#define MF__ MOD__"eo92CreateTaskcluster"

    tsp00_Int4 NumOfDWTasks     = 0;
    tsp00_Int4 NumOfGCTasks     = 0;
    tsp00_Int4 NumOfEVTasks     = 0;
    tsp00_Int4 NumOfBackUpTasks = 0;
    tsp00_Int4 NumOfServerTasks = 0;
    tsp00_Int4 NumOfUserTasks   = 0;
    tsp00_Int4 TotalTasks       = 0;
    TASK_CTRL                   pCurrTask;
    SAPDB_Bool mustSeparateByComma = false;

    szCluster[0]      = '\0';

    for ( pCurrTask =  KGS->pFirstTaskCtrl, TotalTasks = 0;
        pCurrTask <= KGS->pLastTaskCtrl;
        pCurrTask++ )
    {
        if ( UKT_OF_TASK(pCurrTask) != pUKTCtrl ) continue;

        TotalTasks++;

        switch (TASK_TYPE(pCurrTask))
        {
            case TT_TI_EO00 : eo92_AddToClusterString( &mustSeparateByComma,
                                                       "TI",
                                                       szCluster,
                                                       MaxClusterLen);
                        break;

            case TT_AL_EO00 : eo92_AddToClusterString( &mustSeparateByComma,
                                                       "AL",
                                                       szCluster,
                                                       MaxClusterLen);
                        break;

            case TT_TW_EO00 : eo92_AddToClusterString( &mustSeparateByComma,
                                                       "TW",
                                                       szCluster,
                                                       MaxClusterLen);
                        break;

            case TT_UT_EO00 : eo92_AddToClusterString( &mustSeparateByComma,
                                                       "UT",
                                                       szCluster,
                                                       MaxClusterLen);
                        break;

            case TT_DW_EO00 : NumOfDWTasks++          ;
                        break;

            case TT_GC_EO00 : NumOfGCTasks++          ;
                        break;

            case TT_EV_EO00 : NumOfEVTasks++          ;
                        break;

            case TT_BUP_EO00: NumOfBackUpTasks++      ;
                        break;

            case TT_SV_EO00 : NumOfServerTasks++      ;
                        break;

            case TT_US_EO00 : NumOfUserTasks++        ;
                        break;

            default :         eo92_AddToClusterString( &mustSeparateByComma,
                                                       "??",
                                                       szCluster,
                                                       MaxClusterLen);
                        break;
        }
    }

    eo92ExpandMultipleTasks ( &mustSeparateByComma,
                              NumOfDWTasks,
                              "*DW",
                              szCluster,
                              MaxClusterLen ) ;

    eo92ExpandMultipleTasks ( &mustSeparateByComma,
                              NumOfGCTasks,
                              "*GC",
                              szCluster,
                              MaxClusterLen ) ;

    eo92ExpandMultipleTasks ( &mustSeparateByComma,
                              NumOfEVTasks,
                              "*EV",
                              szCluster,
                              MaxClusterLen ) ;

    eo92ExpandMultipleTasks ( &mustSeparateByComma,
                              NumOfBackUpTasks,
                              "*BUP",
                              szCluster,
                              MaxClusterLen ) ;

    eo92ExpandMultipleTasks ( &mustSeparateByComma,
                              NumOfServerTasks,
                              "*SV",
                              szCluster,
                              MaxClusterLen ) ;

    eo92ExpandMultipleTasks ( &mustSeparateByComma,
                              NumOfUserTasks,
                              "*US",
                              szCluster,
                              MaxClusterLen ) ;

    return ( TotalTasks ) ;
}
/*------------------------------*/

/* PTS 1114129 */
externC char *eo92GetConnectStateName(int connState, char *buffer, int len)
{
	if ( len < 80 )
	{
		return (char *)"buffer too small";
	}
	switch(connState)
	{
		case CON_UNUSED_EO003:
			sp77sprintf(buffer, len, "unused[%d]", connState);
			break;
		case CON_INITIALIZED_EO003:
			sp77sprintf(buffer, len, "initialized[%d]", connState);
			break;
		case CON_CONNECTING_EO003:
			sp77sprintf(buffer, len, "connecting[%d]", connState);
			break;
		case CON_CONNECTED_EO003:
			sp77sprintf(buffer, len, "connected[%d]", connState);
			break;
		case CON_REQUESTED_EO003:
			sp77sprintf(buffer, len, "requested[%d]", connState);
			break;
		case CON_RECEIVED_EO003:
			sp77sprintf(buffer, len, "received[%d]", connState);
			break;
		case CON_REPLIED_EO003:
			sp77sprintf(buffer, len, "replied[%d]", connState);
			break;
		case CON_CANCELLED_EO003:
			sp77sprintf(buffer, len, "cancelled[%d]", connState);
			break;
		case CON_RELEASED_EO003:
			sp77sprintf(buffer, len, "released[%d]", connState);
			break;
		case CON_KEEP_ALIVE_EO003:
			sp77sprintf(buffer, len, "keep alive[%d]", connState);
			break;
		case CON_TIMEDOUT_EO003:
			sp77sprintf(buffer, len, "timed out[%d]", connState);
			break;
		default:
			sp77sprintf(buffer, len, "UNKNOWN[%d]", connState);
			break;
	}
	return buffer;
}

/*------------------------------*/

externC char *eo92GetConnectServiceName(int connService, char *buffer, int len)
{
	if ( len < 80 )
	{
		return (char *)"buffer too small";
	}

	switch(connService)
	{
        case srvUser_esp01:
			sp77sprintf(buffer, len, "user task[%d]", connService);
			break;
        case srvAsyncUser_esp01:
			sp77sprintf(buffer, len, "asynchron user task[%d]", connService);
			break;
        case srvUtility_esp01:
			sp77sprintf(buffer, len, "utility task[%d]", connService);
			break;
        case srvDistribution_esp01:
			sp77sprintf(buffer, len, "distribution[%d]", connService);
			break;
        case srvControl_esp01:
			sp77sprintf(buffer, len, "data base manager[%d]", connService);
			break;
        case srvEvent_esp01:
			sp77sprintf(buffer, len, "event task[%d]", connService);
			break;
        case srvStream_esp01:
			sp77sprintf(buffer, len, "stream[%d]", connService);
			break;
        case srvPing_esp01:
			sp77sprintf(buffer, len, "ping[%d]", connService);
			break;
        case srvLVCUser_esp01:
			sp77sprintf(buffer, len, "LVC task[%d]", connService);
			break;
        case srvOLTPUser_esp01:
			sp77sprintf(buffer, len, "OLTP task[%d]", connService);
			break;
		default:
			sp77sprintf(buffer, len, "UNKNOWN[%d]", connService);
			break;
	}
	return buffer;
}

/*------------------------------*/

externC char *eo92GetConnectProtocolName(int connProtocol, char *buffer, int len)
{
	if ( len < 80 )
	{
		return (char *)"buffer too small";
	}
	switch(connProtocol)
	{
		case PROT_UNDEF_EO003:
			sp77sprintf(buffer, len, "undefined[%d]", connProtocol);
			break;
		case PROT_SHM_EO003:
			sp77sprintf(buffer, len, "shared memory[%d]", connProtocol);
			break;
		case PROT_BIGSHM_EO003:
			sp77sprintf(buffer, len, "big shared memory[%d]", connProtocol);
			break;
		case PROT_SOCKET_EO003:
			sp77sprintf(buffer, len, "socket[%d]", connProtocol);
			break;
		case PROT_NI_EO003:
			sp77sprintf(buffer, len, "SAP network interface (NI)[%d]", connProtocol);
			break;
		case PROT_LOCAL_CONTROL_EO003:
			sp77sprintf(buffer, len, "local control[%d]", connProtocol);
			break;
		default:
			sp77sprintf(buffer, len, "UNKNOWN[%d]", connProtocol);
			break;
	}
	return buffer;
}

/*------------------------------*/

externC char *eo92GetSwapTypeName(int swapType, char *buffer, int len)
{
    switch ( swapType )
    {
    case 1:
		sp77sprintf(buffer, len, "not swapped[%d]", swapType);
		break;
	case 2:
		sp77sprintf(buffer, len, "swapped[%d]", swapType);
		break;
	case 3:
		sp77sprintf(buffer, len, "half swapped[%d]", swapType);
		break;
    default:
		sp77sprintf(buffer, len, "Illegal swap type[%d]", swapType);
		break;
    }
	return buffer;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
