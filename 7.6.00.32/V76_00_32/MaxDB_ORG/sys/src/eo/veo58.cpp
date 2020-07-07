/*!
  @file           veo58.c
  @author         JoergM
  @special area   kernel / LZU
  @brief          xparamfile access (configuration)
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


#define MOD__ "veo58.c: "

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "geo00_0.h"
#include "geo50_0.h"
#include "geo007_1.h"
#include "heo46.h"
#include "heo52.h"
#include "heo58.h"
#include "vsp0058.h"
#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp"
#include "SAPDBCommon/SAPDB_string.h"
#if !defined(WIN32)
#include "hen50.h" /* nocheck */ /* vabort() */
#endif /* WIN32 */

class Msg_List;

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

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
 *  LOCAL FUNCTIONS (PROTOTYPES)                                             *
 *===========================================================================*/


/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

/* --------------------------------------------------------------------
Create a unique id like : p26202.bea.sap-ag.de:MUT_629_D19980908T174400
                          ------- NODE -------:--DB---_DYYYYMMDDTHHMMSS
                                                       Date     Time
-------------------------------------------------------------------- */
externC void	vget_uniqe_id ( tsp00_Line & uniqe_id )
{
#undef  MF__
#define MF__ MOD__"vget_uniqe_id"

/*********************************************************************/
#if _WIN32 || OS2 
/*********************************************************************/
SQL_DBNAMEC                             *pszServerDB   = &(kgs.szServerDB  );
SQL_NODEIDC                             *pszServerNode = &(kgs.szServerNode);
/*********************************************************************/
#else
/*********************************************************************/
SQL_NODEIDC                             *pszServerNode = & KGS->servernode ;
SQL_DBNAMEC                             *pszServerDB   = & KGS->serverdb ;
/*********************************************************************/
#endif
/*********************************************************************/

  int DBNodeLen     = (int)strlen ( (const char *)*pszServerNode ) ;
  int DBNameLen     = (int)strlen ( (const char *)*pszServerDB ) ;
  tsp00_Time TimeStamp ;
  int TimeLen       = sizeof(tsp00_Time) ;
  int DateLen       = sizeof(tsp00_Date) ;
  tsp00_Byte *PtrUniqueId =  (tsp00_Byte *)&uniqe_id;
  
  if ( DBNodeLen + 1 + DBNameLen + 1 + TimeLen + DateLen 
                        > sizeof (tsp00_Line ) )
  {
    DBNodeLen = sizeof(tsp00_Line) - DateLen - TimeLen - 1 - DBNameLen - 1 ;
  }
  SAPDB_memset ( PtrUniqueId, ' ', sizeof(tsp00_Line) ) ;

  SAPDB_memcpy( PtrUniqueId, *pszServerNode, DBNodeLen ) ;
  PtrUniqueId += DBNodeLen ;
  *PtrUniqueId = ':' ;
  PtrUniqueId++ ;
  SAPDB_memcpy( PtrUniqueId, *pszServerDB, DBNameLen ) ;
  PtrUniqueId += DBNameLen ;
  *PtrUniqueId = '_' ;
  PtrUniqueId++ ;
  vdattime ( *(tsp00_Date *)PtrUniqueId, TimeStamp ) ;
  PtrUniqueId += DateLen ;
  *PtrUniqueId = '_' ;
  PtrUniqueId++ ;
  SAPDB_memcpy( PtrUniqueId, &TimeStamp[2], TimeLen-2 ) ; /* hours only 2 Byte */
}

/*
 * ==================================================================
 */

DEV_DESCR * sql58k_get_devsp_descr ( tsp2_dev_type_Param    dev_type ,
                                     tsp00_Int4               type_spec_devno )
                                     /* first_spec_devno = 0 !!! */
{
#undef  MF__
#define MF__ MOD__"sql58k_get_devsp_descr "

  RTE_XPARAM_REC                          *XParam        = KGS->XParam ;

  DBGPAS;

  if ( dev_type <= sp2dt_trace &&
       XParam->DevspDescr[dev_type].ulNumDevs > (ULONG) type_spec_devno ) 
    return ( &(XParam->DevspDescr[dev_type].Devspaces[type_spec_devno]) ) ;
  else
    return ( NULL ) ;
}

/*
 * ==================================================================
 */

externC void	vgetrteinfo ( tsp0058_RTEInfo &RTEInfo )
{
#undef  MF__
#define MF__ MOD__"vgetrteinfo"

  RTEInfo.NumOfUKTs     = KGS->ulNumOfUKTs;
  RTEInfo.NumOfUserUKTs = KGS->NumOfUserUKTs;

#if defined(WIN32)
  eo46CtoP( (tsp00_Byte *)&RTEInfo.ServerDB,   &(kgs.szServerDB[0]),   sizeof(tsp00_DbName) );
  eo46CtoP( (tsp00_Byte *)&RTEInfo.ServerNode, &(kgs.szServerNode[0]), sizeof(tsp00_NodeId) );
#else
  eo46CtoP( (tsp00_Byte *)&RTEInfo.ServerDB,   &(KGS->serverdb[0]),    sizeof(tsp00_DbName) );
  eo46CtoP( (tsp00_Byte *)&RTEInfo.ServerNode, &(KGS->servernode[0]),  sizeof(tsp00_NodeId) );
#endif
}

/*
 * ==================================================================
 */

externC void	vget_devsize ( tsp2_dev_type_Param dev_type ,
                       tsp00_Int4          type_spec_devno ,
                       tsp00_Int4        & dev_size ,
                       tsp00_Bool        & ok )
{
#undef  MF__
#define MF__ MOD__"vget_devsize"

  DEV_DESCR *devsp ;
  DBGPAS;

  devsp = sql58k_get_devsp_descr (dev_type, --type_spec_devno ) ;

  ok = devsp != NULL && devsp->lDevspaceType == /*(tsp2_dev_type)*/dev_type ;

  if ( ok )
    { dev_size = devsp->lDevspaceSize ;
      DBG3 (( MF__, "type: %d, spec_devno: %d, size: %d\n", 
                            (tsp2_dev_type)dev_type, type_spec_devno, dev_size )) ;
    }
  return ;
}

/*
 * ==================================================================
 */
externC void	vget_devname ( tsp2_dev_type_Param  dev_type ,
                       tsp00_Int4           type_spec_devno ,
                       tsp2_devname        & dev_name ,
                       tsp00_Bool          & ok )
{
#undef  MF__
#define MF__ MOD__"vget_devname"

  DEV_DESCR *devsp ;

  DBGPAS;
  devsp = sql58k_get_devsp_descr (dev_type, --type_spec_devno ) ;

  ok = devsp != NULL && devsp->lDevspaceType == /*(tsp2_dev_type)*/dev_type ;

  if ( ok )
  {
    eo46CtoP ( (tsp00_Byte *)dev_name.asCharp() , devsp->szDevspaceName, sizeof(tsp2_devname) );
    DBG3 (( MF__, "type: %d, spec_dev.no: %d, name: %s\n", 
            /*(tsp2_dev_type)*/dev_type, type_spec_devno, devsp->szDevspaceName));
  }
  return;
}

/* move this with new I/O handling... */
externC tsp00_Bool RTEHSS_UpdateLogVolumeMapping();
externC tsp00_Bool RTEHSS_UpdateDataVolumeMapping();

externC void UpdateHotStandbyMapping(tsp2_dev_type_Param  dev_type)
{
    /* move this with new I/O handling... */
    if ( dev_type == sp2dt_log || dev_type == sp2dt_mirr_log )
    {
        RTEHSS_UpdateLogVolumeMapping();
    }

    if ( dev_type == sp2dt_data )
    {
        RTEHSS_UpdateDataVolumeMapping();
    }
}

/*
 * ==================================================================
 */
externC void	vnew_devspace( tsp2_dev_type_Param  dev_type ,
                       tsp00_Int4             type_spec_devno ,
                       tsp2_devname         & dev_name ,
                       tsp00_Int4             dev_size ,
                       tsp00_Bool           & ok,
                       Msg_List             & err)
{
#undef  MF__
#define MF__ MOD__"vnew_devspace"

  DEV_DESCR *new_devsp ;
  tsp2_devnamec   szDevspaceName;

  DBGPAS;

  eo46PtoC ( szDevspaceName, (tsp00_Byte *)dev_name.asCharp(), sizeof(tsp2_devname));

  ok = RTEConf_CheckDuplicateDevSpaceName ( KGS->XParam->DevspDescr, szDevspaceName, err );

  if ( ok ) 
  {
    new_devsp = sql58k_get_devsp_descr (dev_type, --type_spec_devno );

    ok = new_devsp != NULL && new_devsp->lDevspaceType == DEVSPACE_UNUSED ;

    if ( ok )
    { 
      eo46PtoC ( new_devsp->szDevspaceName, (tsp00_Byte *)dev_name.asCharp(), sizeof(tsp2_devname));
      new_devsp->lDevspaceType = /*(tsp2_dev_type)*/dev_type ;
      new_devsp->lDevspaceSize = dev_size ;
      new_devsp->physicalDevID = RTE_UNDEF_ID;      /* 1109850 */
      new_devsp->accessMode    = RTE_VolumeAccessModeNormal;
      /* XXX ---> elke ??? extension of ADD VOLUME SQL_Command ??? */
      new_devsp->fileSystemAccessType = RTE_FileSystemAccessTypeNormal;

      DBG3 (( MF__, "type: %d, spec_devno: %d, name: %s, size %d\n", (tsp2_dev_type)dev_type, 
              type_spec_devno, new_devsp->szDevspaceName, dev_size));

      /* move this with new I/O handling... */
      UpdateHotStandbyMapping(dev_type);
    }
  }
  return ;
}


/*
 * ==================================================================
 */
externC void	vrevoke_devspace( tsp2_dev_type_Param  dev_type ,
                          tsp00_Int4             type_spec_devno )
{
#undef  MF__
#define MF__ MOD__"vrevoke_devspace"

  int        ok;
  DEV_DESCR *revoke_devsp ;
  DBGPAS;

  revoke_devsp = sql58k_get_devsp_descr (dev_type, --type_spec_devno );

  ok = revoke_devsp != NULL && revoke_devsp->lDevspaceType == /*(tsp2_dev_type)*/dev_type ;

  if ( ok )
    { 
      DBG3 (( MF__, "type: %d, spec_devno: %d, name: %s, size %d\n", 
                     (tsp2_dev_type)dev_type, type_spec_devno, revoke_devsp->szDevspaceName, 
                    revoke_devsp->lDevspaceSize));
      revoke_devsp->lDevspaceType = DEVSPACE_UNUSED ;
      revoke_devsp->lDevspaceSize = 0 ;

      UpdateHotStandbyMapping(dev_type);
    }
  return ;
}


#if defined(_WIN32)
#  define CURR_TASK             (THIS_UKT_CTRL)->pCTask
#else
#  define CURR_TASK             (THIS_UKT_CTRL)->curr_task
#endif

/* PTS CR 1103950 */
/*
 * ==================================================================
 */
externC void	vinit_akdump_ptr( void  *acv_pointer ,        /* CR 1000209 */
                          void (*ak_dump_proc)() )    /* CR 1000209 */

{
#undef  MF__
#define MF__ MOD__"vinit_akdump_ptr"

/*********************************************************************/
#if _WIN32
/*********************************************************************/
  PUKT_CTRL_REC                  pUKT      = THIS_UKT_CTRL;
  PTASK_CTRL_REC                 pCurrTask = pUKT->pCTask;

  DBGIN_T (pCurrTask->ulTaskIndex);

/*********************************************************************/
#endif
/*********************************************************************/
  CURR_TASK->AcvPointer = acv_pointer ;
  CURR_TASK->AkDumpProc = ak_dump_proc ;
}

/*
 * ==================================================================
 */

/* CR 1104345 */
externC void *vGetAcvPtrFromCurrentTask()
{
#undef  MF__
#define MF__ MOD__"vGetAcvPtrFromCurrentTask"

#if defined(_WIN32)
    PUKT_CTRL_REC   this_ukt = THIS_UKT_CTRL;
#else /* UNIX */
    ten50_UKT_Control *this_ukt = THIS_UKT_CTRL;
#endif /* UNIX */

    if ( !this_ukt )
    {
        return (void *)0;
    }

#if defined(_WIN32)
    return this_ukt->pCTask->AcvPointer;
#else /* UNIX */
    return this_ukt->curr_task->AcvPointer;
#endif
}

/* PTS 1108768 */
static tsp00_Bool wantTrace = false;
static tsp00_Bool wantDump = false;
/*!
   @Description    Initialization Function to retrieve Trace and Dump Information
   @param          vtraceName [out] Name of Tracefile (knltrace)
   @param          vdumpName [out] Name of Dumpfile (knldump)

 */

externC void        vInitTraceAndDump ( tsp00_VFilename VAR_ARRAY_REF vtraceName,
                                tsp00_VFilename VAR_ARRAY_REF vdumpName
            					        )
{
  tsp00_Int4 traceDevsize;
  vdiaginit(wantTrace, wantDump, vtraceName, traceDevsize, vdumpName);
}

/*!
   @Description    Request current status of dump wanted flag
   @Return value   true if dump of kernel structures is wanted, false if not

 */

externC tsp00_Bool  vIsDumpWanted  ( )
{
  return wantDump;
}

/*!
   @Description    Request current status of trace wanted flag
   @Return value   true if writing kernel trace is wanted, false if not

 */

externC tsp00_Bool  vIsTraceWanted ( )
{
  return wantTrace;
}

/*------------------------------*/
/* PTS 1110561 */
externC void vpid_alive( tsp00_TaskId pid )
{
#undef MF__
#define MF__ MOD__"vpid_alive"

#if defined(_WIN32)

#ifdef DEVELOP_CHECKS
    PTASK_CTRL_REC                 pTaskCtrl = CURR_TASK;

    if ( (ULONG)pid != pTaskCtrl->ulTaskIndex )
    {
        MSGD (( ERR_VXXXX_WRONG_TASK, "vpid_alive", pid ));
        DBG1 (( MF__, "[T:0x%03u] Wrong pid %u", pTaskCtrl->ulTaskIndex, pid ));
        ABORT();
    }
#else
    PTASK_CTRL_REC                 pTaskCtrl = &KGS->pFirstTaskCtrl[pid - 1];
#endif

#else /* UNIX */
#ifdef DEVELOP_CHECKS
    struct TASK_TYPE   *pTaskCtrl = THIS_UKT_CTRL->curr_task;

    if ( (tsp00_Int4)pid != pTaskCtrl->index )
    {
        MSGD (( ERR_VXXXX_WRONG_TASK , "vpid_alive", (long) pid ));
        vabort(WRITE_CORE);
    }

#else
    struct TASK_TYPE   *pTaskCtrl = &KGS->pFirstTaskCtrl[pid - 1];
#endif

#endif

    pTaskCtrl->TaskAlive++;
}

/*------------------------------*/
/* PTS 1110561 */
externC void vtracewriter_alive( )
{
#undef MF__
#define MF__ MOD__"vtracewriter_alive"

#if defined(_WIN32)
    if (CURR_TASK->ulTaskIndex == KGS->pTW->ulTaskIndex)
    {
        KGS->pTW->TaskAlive++;
    }
#else /* UNIX */
    if (CURR_TASK->index == KGS->tw->index)
    {
        KGS->tw->TaskAlive++;
    }
#endif
}


