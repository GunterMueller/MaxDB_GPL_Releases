/*!
  @file           ven70.cpp
  @author         JoergM
  @brief          SAPDB Kernel: Initialization of shared memory
  

first created  2000-10-17  14:10

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

#include "gen00.h"
#include "geo00_1.h"
#include "geo001.h"
#include "geo002.h"
#include "geo50_0.h"
#include "gen55.h"
#include "geo70k.h"
#include "gen71.h"


#ifdef OSF1
#include	<sys/param.h>
#else
#include	<sys/param.h>
#include	<sys/sysmacros.h>
#endif

#include        <ctype.h>

#include <sys/mman.h>
#ifndef MAP_FAILED
#  define MAP_FAILED -1
#endif

#include "heo07.h"
#include "gen500.h"
#include "gen70.h"
#include "gen41.h"
#include "geo57.h"
#include "gen73.h"
#include "gen74.h"
#include "gen38.h"
#include "hen40.h"
#include "gen54.h"
#include "heo15.h"
#include "gen81.h"
#include "hen50.h"
#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.h"
#include "RunTime/Tasking/RTETask_Task.hpp"
#include "RunTime/Tasking/RTETask_TaskScheduler.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "RunTime/Tasking/RTETask_CommonQueueElement.hpp"
#include "RunTime/Tasking/RTETask_CommonQueue.hpp"
#include "RunTime/ClientKernelCommunication/RTECKC_KernelCommunication.hpp"
#include "gen84.h"
#include "KernelCommon/Kernel_ExtendedState.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#undef MOD__
#define MOD__ "ven70.c:"

#define DBPAGESIZE ((unsigned long)XParam->ulDBPageSize)

#define MAX_REQ_PER_TASK                2
#if defined(LINUX)
#define SHARED_MEMORY_ALIGNMENT getpagesize()
#else
#define SHARED_MEMORY_ALIGNMENT SHMLBA
#endif

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

static  int     en70_InsertTask ( struct  TASK_TYPE * tcb ,
				  int                 task_type ,
				  int                 stack_size ,
				  ten50_UKT_Control * pUktCtrl ,
				  int                 count );
static  void    en70_InitProc (   ten50_KT_Control * kp );
static  void    en70_RemoveKgs (  int * shmids );
static  void *  en70_AllocSystemPages(size_t SizeInBytes);

/*===========================================================================*
 *=====================  END OF DECLARATIONS ================================*
 *===========================================================================*/

/*===========================================================================*
 *  EXPORTED FUNCTIONS, EXPORTED CLASS METHODS (IMPLEMENTATION)              *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL/INLINE FUNCTIONS, LOCAL CLASS METHODS (IMPLEMENTATION)             *
 *===========================================================================*/
/* exported data */

/* imported data */

extern	int				e60_dbgdbglvl ;

extern  tsp00_DbNamec   en81_dbname;
extern	int		en81_usable_fds ;

/*------------------------------*/

static void SearchUnreleasedConnectionsCallback(bool atLeastOneFound)
{
    if ( !atLeastOneFound )
    {
        // --- sleep a moment and retry find free task
        sleep ( 3 );
    }

    en84_remove_unrel_connnections();
}

/*---------------------------------------------------------------------------
  Function: e70_init_kgs
  Description: Initialize the kernel global section

  Arguments: XParam [in] the parameter structure
  Return value: 0 everything worked fine
                != 0 initialization failed, you should giveup...
 */
#undef MF__
#define MF__ MOD__"e70_init_kgs"
int e70_init_kgs ( RTE_XPARAM_REC *XParam )
{
  int           i;
  int           idx;

  long          kgs_number_of_tasks ;
  long          kgs_ukt_count ;
  tsp00_Int4    kgs_number_of_user_ukts;

  unsigned long tot_size ;
  unsigned long tot_offset ;
  unsigned long shm_start ;
  unsigned long kgs_start ;
  unsigned long kgs_shmids ;

  unsigned long kgs_first_kp ;
  unsigned long kgs_mem_for_run_queues ;
  unsigned long kgs_mem_for_queues ;
  unsigned long kgs_mem_xxx_q_head ;
  unsigned long kgs_mem_xxx_queues ;
  unsigned long kgs_mem_for_ios_q  ;
  unsigned long kgs_mem_for_dev_info ;
  unsigned long kgs_mem_for_ukp_io ;
  unsigned long kgs_mem_for_regions ;
  unsigned long kgs_mem_for_reglist ;
  unsigned long kgs_mem_for_reg_stat_cnt ;
  unsigned long kgs_mem_for_reg_stat_coll;
  unsigned long kgsMemoryForMessageBuffer;
    /* PTS 1108976 */
  void *        memoryForTraceBuffer;
  tsp00_Int4    kgsTotalTraceBufferSizeInPages;
  char *        nextUktTraceBuffer;

  unsigned long kgs_last_kp ;
  unsigned long kgs_first_task ;
  unsigned long kgs_last_task ;
  unsigned long kgs_devsp_descr;
  unsigned long kgs_first_asdev ;
  unsigned long kgs_first_dev ;
  unsigned long kgs_last_dev ;
  unsigned long kgs_first_xxx ;
  unsigned long kgs_last_xxx ;
  unsigned long kgs_rte_xp_size;
  key_t         kgs_ipcBaseKey = (key_t)0;

  unsigned long kgs_first_task_object ;
  char *        task_object_buffer;

  unsigned long kgs_first_task_scheduler_object ;
  char *        task_scheduler_object_buffer;

  unsigned long kgs_common_queue_element_object;
  char *        common_queue_element_object_buffer;

  unsigned long kgs_first_asyn_req_object;
  char *        asyn_req_buffer;

  struct RTETask_CommonQueueElement*  pCommonQueueElements;

  unsigned long kgs_synchronized_common_queue_free_list;
  char *        synchronized_common_queue_buffer;

  unsigned long kgs_kernel_communication;
  char *        kernel_communication_buffer;

  int           rc ;
  int	        count ;
  int           ukt_index ;
  int           task_index ;

  int		max_io_devs ;
  int		max_ukpio_devs ;
  unsigned long number_of_freelist ;
  struct  SHARED_SEGMENT          * seg_ptr = 0;
  struct  TASK_TYPE               * tcb ;
  ten50_UKT_Control               * lkp ;
  struct  XXX_QUEUE_HEAD          * l_xxx ;
  struct  DOUBLY_LINKED           * quu ;
  struct  DEV_PCB                 * devpcb ;
  unsigned long                     kgs_connection ;
  unsigned long                     LastKgsPartSize ;
  int                               kgs_shmid = 0;
  tsp00_Int2                        maxdevspaces = XParam->lMaxDevspaces  ;
  tsp00_Int2                        num_dw ;
  tsp00_Int4                        num_asynio_tasks = XParam->ulMaxServer + 1 + XParam->ulMaxUser;/*1=ut*/
  tsp00_Int4                        devpsets  = XParam->lDeviPerDevspace + 
                                                XParam->lReaderDeviPerDevspace +
                                                XParam->lPrioDeviPerDevspace;
  tsp00_Int4                        MaxBackupDevs = XParam->lNoOfBackupDevs + 1;
  unsigned long                     kgs_aioc_queue_elem;
  unsigned long	                    kgs_mem_for_sys_asyn_struc;
  unsigned long                     SizeOfSysAsynIOStruct = e54a_get_aio_struct_size();
  teo70k_ClusterInfo *              pCluInfo = NULL;

  DBGIN;

  if ( (int) strlen((char *) en81_dbname) >= (int) sizeof(tsp00_DbNamec) )
  {
	  MSGCD (( ERR_INITSHM_DBNAME_TO_LONG, sizeof(KGS->serverdb)-1 ));
	  return ( 2 ) ;
  }

  num_dw = XParam->ulMaxDataWriter ? XParam->ulMaxDataWriter : 1 ;

/*
 * ================================================================
 *  Analyze the task cluster.
 *  The results of the parsing are stored in 'cluster' and 'proc'.
 * ================================================================
 */

  if ( eo70k_AnalyzeTaskcluster ( XParam, &pCluInfo, en81_MemPageSize ) != 0 )
  {
    MSGCD (( ERR_INITSHM_BAD_TC , &XParam->TaskClusterArray[0] ))
    return ( 2 );
  }

  if ( pCluInfo->fOneToOne && XParam->uLoadBalancingCheck == 0 )
  {
      // disable load balancing permanently
      XParam->fForbidLoadBalancing = true;
  }

  kgs_number_of_tasks     = pCluInfo->ulNumOfTasks ;
  kgs_ukt_count           = pCluInfo->ulNumOfUKTs ;
  kgs_number_of_user_ukts = pCluInfo->ulNumOfUserUKTs;

/*
 * ====================================================================
 *  Calculate 'tot_size' the size of the kernel shared memory section.
 * ====================================================================
 */

/* PTS 1001132 */

  DBG1 (( MF__,"size kgs          %d \n",
				  sizeof(struct KERNEL_GLOBAL_SECTION) ));
  DBG1 (( MF__,"size rte_pcb      %d \n",
				  sizeof(ten50_KT_Control) ));
  DBG1 (( MF__,"size dev_pcb      %d \n",
				  sizeof(struct DEV_PCB) ));
  DBG1 (( MF__,"size ten50_UKT_Ctrl %d \n",
				  sizeof(ten50_UKT_Control) ));
  DBG1 (( MF__,"size tcb          %d \n",
				  sizeof(struct TASK_TYPE) ));
  DBG1 (( MF__,"size xxx          %d \n",
				  sizeof(struct XXX_QUEUE_HEAD) ));
  DBG1 (( MF__,"size quu          %d \n",
				  sizeof(struct DOUBLY_LINKED) ));
  DBG1 (( MF__,"size page         %ld \n",
				  XParam->ulDBPageSize ));
  DBG1 (( MF__,"MAX_QUU_PER_TASK  %d \n",
				  MAX_QUU_PER_TASK ));
  DBG1 (( MF__,"SHARED_MEMORY_ALIGNMENT 0x%08x \n", SHARED_MEMORY_ALIGNMENT     ));

  tot_size = 0 ;

/*
 *  Shared Segment Header with pointer to kgs (12 bytes)
 */
  tot_size += sizeof(struct SHARED_SEGMENT);
  DBG1 (( MF__,"starting with   0x%08lx \n", 0L ));

/*
 *  Allocate space for the KGS structure.
 */
/* pointer alignment */
  tot_size  = ( (tot_size - 1) / sizeof(void *) + 1 ) * sizeof(void *) ;
  kgs_start = tot_size ;
  tot_size += sizeof(struct KERNEL_GLOBAL_SECTION);
  DBG1 (( MF__,"kgs offset      0x%08lx \n", kgs_start ));

  kgs_shmids = tot_size ;
  tot_size   += sizeof (int) ;
  tot_size   = ( (tot_size - 1) / sizeof(void *) + 1 ) * sizeof(void *) ;

/*
 *  free list
 */
  number_of_freelist = kgs_number_of_tasks * MAX_QUU_PER_TASK * 2 + kgs_ukt_count ;
  DBG1 (( MF__,"free events     %d \n", number_of_freelist ));

  kgs_common_queue_element_object  = tot_size;
  tot_size                        += sizeof(RTETask_CommonQueueElement) * number_of_freelist;

  kgs_synchronized_common_queue_free_list = tot_size;
  tot_size                               += sizeof(RTETask_SynchronizedCommonQueue);

/*
 *  unconnected task pool
 */
  kgs_kernel_communication   = tot_size;
  tot_size                  += sizeof(RTECKC_KernelCommunication);

/*
 *  kernel processes
 */
  kgs_first_kp  = tot_size;
  tot_size     += sizeof(ten50_UKT_Control) * kgs_ukt_count ;
  kgs_last_kp   = tot_size - sizeof(ten50_UKT_Control);
  DBG1 (( MF__,"first_kp        0x%08lx \n", kgs_first_kp   ));
  DBG1 (( MF__,"last_kp         0x%08lx \n", kgs_last_kp    ));


  kgs_aioc_queue_elem =  XParam->fUseAsyncIO ? 
					   kgs_number_of_tasks : kgs_ukt_count  ; /* 1 per UKT */

  kgs_mem_for_queues = tot_size;
  tot_size += kgs_aioc_queue_elem * sizeof ( struct IOR_QUEUE ) /* aiocs */
      + kgs_number_of_tasks  * kgs_ukt_count * sizeof ( struct IOC_QUEUE ) /* iocs */
	      + (maxdevspaces + MaxBackupDevs) * MAX_ASYN_REQUESTS *
			  kgs_ukt_count * sizeof ( struct IOC_QUEUE ); /* iocs */
		  /*
		   *  Each UKT may contain the task which is allowed to issue
		   *  MAX_ASYN_REQUESTS per devspace and per tape.
		   */

  DBG1 (( MF__,"IOR Elements %d\n", kgs_aioc_queue_elem ));
  DBG1 (( MF__,"IOC Elements %d\n", kgs_number_of_tasks ));
  DBG1 (( MF__,"IOC for async request per UKT %d\n", (maxdevspaces + MaxBackupDevs) * MAX_ASYN_REQUESTS ));
  DBG1 (( MF__,"mem_for_queues  0x%08lx \n", kgs_mem_for_queues ));

  kgs_mem_for_run_queues = tot_size;
  tot_size += MAX_REQ_PER_TASK * kgs_number_of_tasks * kgs_ukt_count *
		    sizeof ( struct RUN_QUEUE ) ;
  DBG1 (( MF__,"mem_for_run_queues  0x%08lx \n", kgs_mem_for_run_queues ));

/*
 *  task scheduler objects
 */
  kgs_first_task_scheduler_object = tot_size ;
  tot_size                       += sizeof(RTETask_TaskScheduler) * kgs_ukt_count;

/*
 *  tasks
 */
  kgs_first_task  = tot_size ;
  tot_size       += sizeof(struct TASK_TYPE) * kgs_number_of_tasks ;
  kgs_last_task   = tot_size - sizeof(struct TASK_TYPE);
  DBG1 (( MF__,"pFirstTaskCtrl      0x%08lx \n", kgs_first_task ));
  DBG1 (( MF__,"pLastTaskCtrl       0x%08lx \n", kgs_last_task  ));

/*
 *  task objects
 */
  kgs_first_task_object  = tot_size ;
  tot_size              += sizeof(RTETask_Task) * kgs_number_of_tasks ;

/*
 *  tasks ( async_io ) 
 */
  kgs_mem_for_sys_asyn_struc =  tot_size ;
  tot_size                   += SizeOfSysAsynIOStruct 
                              * (  kgs_number_of_tasks    /* vblockio */
                                 + num_asynio_tasks  * MAX_ASYN_REQUESTS ) ; /* vasynio */
  DBG1 (( MF__,"total size for aio_struct %d\n", 
    SizeOfSysAsynIOStruct * kgs_number_of_tasks ));

/*
 *  devspace_descriptions
 */
  kgs_rte_xp_size = tot_size ;
  tot_size       += sizeof(RTE_XPARAM_REC) ;

  kgs_devsp_descr = tot_size ;
  tot_size       += sizeof(DEV_DESCR) * maxdevspaces ;
  DBG1 (( MF__,"devsp_descr_size 0x%lx \n", 
			      sizeof(DEV_DESCR) * maxdevspaces ));

/*
 *  devprocs
 */
  max_io_devs = maxdevspaces * 2 + MaxBackupDevs ;
  max_ukpio_devs  = ( XParam->fUseAsyncIO ) ? max_io_devs : maxdevspaces ;
  kgs_first_dev   = tot_size ;
  tot_size       += sizeof(struct DEV_PCB) * ( devpsets * maxdevspaces ) ;
  kgs_first_asdev   = tot_size ;
  tot_size       += sizeof(struct DEV_PCB) * ( maxdevspaces + MaxBackupDevs );
  kgs_last_dev    = tot_size - sizeof(struct DEV_PCB);
  DBG1 (( MF__,"first_dev       0x%08lx \n", kgs_first_dev  ));
  DBG1 (( MF__,"last_dev        0x%08lx \n", kgs_last_dev   ));

  kgs_mem_for_ios_q  = tot_size ;

  /* DEV0 */
  tot_size += sizeof(struct IOC_QUEUE) * max_io_devs ;

  /* DEVIs */
  tot_size += sizeof(struct IOC_QUEUE) * kgs_number_of_tasks * 
		    devpsets * maxdevspaces ;          
  /* ASDEVs */
  tot_size += sizeof(struct IOC_QUEUE) * num_asynio_tasks * MAX_ASYN_REQUESTS 
					     * ( maxdevspaces + MaxBackupDevs );

  DBG1 (( MF__,"mem_for_ios_queue  0x%08lx \n", kgs_mem_for_ios_q ));

  /* ASYN REQUESTS */
  kgs_first_asyn_req_object = tot_size ;
  tot_size += sizeof(RTETask_CommonQueueElement) * num_asynio_tasks * MAX_ASYN_REQUESTS;
  DBG1 (( MF__,"mem_for_asyn_req   0x%08lx \n", kgs_first_asyn_req_object ));

  kgs_mem_for_dev_info = tot_size ;
  tot_size += sizeof(struct dev_info) * max_io_devs ;
  DBG1 (( MF__,"mem_for_dev_info   0x%08lx \n", kgs_mem_for_dev_info ));

  kgs_mem_xxx_q_head = tot_size ;
  tot_size += sizeof(struct XXX_QUEUE_HEAD *) * kgs_ukt_count * kgs_ukt_count;
  DBG1 (( MF__,"mem_for_xxx_q_head 0x%08lx \n", kgs_mem_xxx_q_head ));

  kgs_mem_for_ukp_io =tot_size ; 
  tot_size += sizeof(struct UKP_IO_STRUCT) * max_ukpio_devs * kgs_ukt_count ;
  DBG1 (( MF__,"mem_for_ukp_io     0x%08lx \n", kgs_mem_for_ukp_io));

/*
 *  Connection information blocks for tasks and processes.
 */
  kgs_connection = tot_size ;
  tot_size += e74_comdata_size ( XParam , kgs_ukt_count );
  DBG1 (( MF__,"connection    0x%08lx \n", kgs_connection ));

/*
 *  ukp_to_ukp queues
 */
  kgs_first_xxx  = tot_size ;
  tot_size      += sizeof(struct XXX_QUEUE_HEAD) *
		   kgs_ukt_count * ( kgs_ukt_count - 1 );
  kgs_last_xxx   = tot_size - sizeof(struct XXX_QUEUE_HEAD);
		   /* if only one ukp, this is lower than first_xxx */
  DBG1 (( MF__,"first_xxx       0x%08lx \n", kgs_first_xxx  ));
  DBG1 (( MF__,"last_xxx        0x%08lx \n", kgs_last_xxx   ));

  {
    int idx, tot_xxx_elem;
    kgs_mem_xxx_queues = tot_size;
    if ( XParam->fForbidLoadBalancing )
    {
        for ( tot_xxx_elem = 0, idx=0; idx< kgs_ukt_count; idx++ )
	        tot_xxx_elem += ( pCluInfo->pThrdInfo[idx].ulNumOfTasks + 1 ) 
					        * (kgs_ukt_count -1) ;
    }
    else
    {
        for ( tot_xxx_elem = 0, idx=0; idx< kgs_ukt_count; idx++ )
	        tot_xxx_elem += ( kgs_number_of_tasks + 1 ) 
					        * (kgs_ukt_count -1) ;
    }
    DBG1 (( MF__,"init %d tot_xxx_elem for all ukps \n", tot_xxx_elem ));
    tot_size += tot_xxx_elem * sizeof ( struct XXX_QUEUE ) ;  
    DBG1 (( MF__,"total_size for xxx_elements %d at offs %x \n", 
			     tot_xxx_elem * sizeof ( struct XXX_QUEUE ), 
			     kgs_mem_xxx_queues ));
  }

/*
 * DDB4_SEMA
 */
  kgs_mem_for_regions = tot_size ;
  tot_size += XParam->ulNoOfRegions * sizeof( struct DDB4_SEMA );

  kgs_mem_for_reglist = tot_size ;
  tot_size += kgs_number_of_tasks * sizeof(struct N_QUEUE) * XParam->ulNoOfRegions;

  kgs_mem_for_reg_stat_cnt = tot_size ;
  tot_size += kgs_number_of_tasks * XParam->ulNoOfRegions  * sizeof( tsp00_Int4 );

  kgs_mem_for_reg_stat_coll= tot_size ;
  tot_size += kgs_number_of_tasks * XParam->ulNoOfRegions  * sizeof( tsp00_Int4 );

/* PTS 1103824 Space for TraceBuffers used i.e. by COM procedures */
  kgsMemoryForMessageBuffer = tot_size;
  tot_size += XParam->ulMaxMessageFiles * sizeof(teo15MessageBufferSpace);

/*
 * =================================================================
 *  END OF KGS !
 * =================================================================
 */

  DBG1 (( MF__,"tot_size (hex)  0x%08x \n", tot_size ));
  DBG1 (( MF__,"tot_size (dec)  %d \n", tot_size ));

/*
 * ==========================
 *  Create KGS shared memory
 * ==========================
 */

  tot_size = ( ( tot_size - 1 ) / SHARED_MEMORY_ALIGNMENT + 1 ) * SHARED_MEMORY_ALIGNMENT;

  DBG1 (( MF__,"tot_size = %ld\n", tot_size )) ;

  if ( !en41CreateServerKeyAndFirstSegment( en81_dbname,
                                            tot_size,
                                            &kgs_ipcBaseKey,
                                            &kgs_shmid ) )
  {
      MSGD(( ERR_IPC_SHMS_SIZE, tot_size));
      MSGD(( ERR_IPC_SHMC_SHMGET, "Check OS Parameter for IPC SHM "));
      MSGD ((ERR_IPC_SHMC_SHMGET, "and please check also your critical database parameters" )) ;
      MSGD ((ERR_IPC_SHMC_SHMGET, "These are especially MAXUSERTASKS, MAXSERVERTASKS and CACHE_SIZE" ));
      return(2);
  }

  DBG1 (( MF__,"trying to attach at system supplied address\n" ));
#if defined(LINUX) && defined(I386)
  seg_ptr = (struct SHARED_SEGMENT *)sql41AttachShm ( kgs_shmid, (char*) 0x50000000, 0 );
#else
  seg_ptr = (struct SHARED_SEGMENT *)sql41AttachShm ( kgs_shmid, (char*) 0, 0 );
#endif
  if ( seg_ptr == 0 )
  {
    MSGD ((ERR_IPC_SHMC_SHMGET, "Cannot attach the main shared memory segment" )) ;
    MSGD(( ERR_IPC_SHMS_SIZE, tot_size));
    MSGD(( ERR_IPC_SHMC_SHMGET, "Check OS Parameter for IPC SHM "));
    MSGD ((ERR_IPC_SHMC_SHMGET, "and please check also your critical database parameters" )) ;
    MSGD ((ERR_IPC_SHMC_SHMGET, "These are especially MAXUSERTASKS, MAXSERVERTASKS and CACHE_SIZE" ));
    en70_RemoveKgs ( &kgs_shmid ) ;
    return ( 2 );
  }

  /* All segments have been attached at the system supplied address */
  shm_start = (unsigned long) seg_ptr ;

  MSGD(( INFO_INITSHM_ATTACHED_AT, "shared section", shm_start ));
  MSGD(( INFO_INITSHM_ENDS_AT    , "shared section", shm_start+tot_size ));

/*
 *  set global pointer to kernel global section
 */
  kgs_start += shm_start ;
  kgs = (struct KERNEL_GLOBAL_SECTION *) kgs_start ;

/*
 *      the shared memory segment has been established
 */

  SAPDB_memset ( seg_ptr , 0 , tot_size );
  seg_ptr->this_seg = seg_ptr ;
  seg_ptr->seg_size = tot_size ;
  seg_ptr->kgs      = (struct KERNEL_GLOBAL_SECTION *) kgs_start ;
  DBG1 (( MF__,"KGS                0x%08lx", kgs_start ));
  DBG1 (( MF__,"segment end        0x%08lx", shm_start + tot_size ));

/*
 *  insert the computed values into the shared section
 */
  RTE_KGS::Instance().SetKGS(KGS);
  KGS->this_kgs         = KGS;
  KGS->seg              = seg_ptr ;
  KGS->seg_size         = tot_size ;
  KGS->ipcBaseKey       = kgs_ipcBaseKey ;
  KGS->kgsshmid         = (int *) (kgs_shmids + shm_start) ;
  *(KGS->kgsshmid)      = kgs_shmid;
  KGS->uid              = getuid();
  KGS->pid              = getpid();
  KGS->debuglevel       = e60_dbgdbglvl ;
  KGS->ulNumOfTasks     = kgs_number_of_tasks ;
  KGS->NumOfUserUKTs    = kgs_number_of_user_ukts;
  KGS->ulNumOfUKTs      = kgs_ukt_count ;
  KGS->first_kp         = (ten50_UKT_Control *) (kgs_first_kp + shm_start);
  KGS->last_kp          = (ten50_UKT_Control *) (kgs_last_kp + shm_start);
  KGS->pFirstTaskCtrl       = (struct TASK_TYPE *) (kgs_first_task + shm_start);
  KGS->pLastTaskCtrl        = (struct TASK_TYPE *) (kgs_last_task + shm_start);
  KGS->first_dev        = (struct DEV_PCB *) (kgs_first_dev + shm_start);
  KGS->first_asdev      = (struct DEV_PCB *) (kgs_first_asdev + shm_start);
  KGS->last_dev         = (struct DEV_PCB *) (kgs_last_dev + shm_start);
  KGS->connection       = (struct connection_info*) (kgs_connection + shm_start);
  KGS->first_xxx        = (struct XXX_QUEUE_HEAD *) (kgs_first_xxx + shm_start);
  KGS->last_xxx         = (struct XXX_QUEUE_HEAD *) (kgs_last_xxx + shm_start);
  INIT_LOCK(KGS->ExclusiveVallocat);
  INIT_LOCK(KGS->ExclusiveVnewbuf);
  INIT_LOCK(KGS->lastTaskMoves.exclusive);

  PAGESIZE_TO_SHIFTVALUE( XParam->ulDBPageSize, &KGS->ulDBPageShift ); 

/*
 *  setup shared kernel state access
 */
  kgs->kernelStateSize = sizeof(Kernel_SharedState);
  Kernel_ExtendedState::pPendingUpdateCounter   = &kgs->pendingUpdateCounter;
  Kernel_ExtendedState::pCompletedUpdateCounter = &kgs->completedUpdateCounter;
  Kernel_ExtendedState::pSharedState            = &kgs->kernelState;

/*
 *  Copy database name
 *  (was in ven81c - needed here for ven41c IPC management routines
 *  which may be called indirectly via ALONE way down below. 91-11-25 J.B.)
 */

  (void) strcpy ( (char *) KGS->serverdb , (char *) en81_dbname );

/*FSt*/
  KGS->maxtapes            = MaxBackupDevs ;
  KGS->max_asyn_request    = MAX_ASYN_REQUESTS ;
  KGS->maxdevspaces        = maxdevspaces ;
  KGS->max_ukpio_devs      = max_ukpio_devs ;
  KGS->devpsets            = devpsets;
  KGS->dev_queue           = XParam->lDevQueue ;
/*
 *  Initialise RTE Process states and info
 */
  en70_InitProc ( & KGS->console );
  en70_InitProc ( & KGS->timer );
  en70_InitProc ( & KGS->requestor );
  en70_InitProc ( reinterpret_cast<ten50_KT_Control *>(& KGS->dev0) );
  
  KGS->coord.state = KT_RUNNING ;
  KGS->coord.semid = UNDEF ;

  e73_dl_init ( & KGS->timer.work, (char*)"TIMER" );

/*
 * Initialize DEV0
 */

  DBG1 (( MF__,"devpcb is  0x%08x \n", & KGS->dev0 ));
  KGS->dev0.ios = (struct IOC_QUEUE *) (kgs_mem_for_ios_q + shm_start);
  kgs_mem_for_ios_q  += max_io_devs * sizeof ( struct IOC_QUEUE ) ;
  e73_dev_ioc_queue_init ( KGS->dev0.ios , max_io_devs );

  KGS->dev0.io_work          = KGS->dev0.ios;
  KGS->dev0.io_request       = KGS->dev0.ios;
  KGS->dev0.cpu_id           = -1 ;
  KGS->dev0.nice             = 0 ;
  KGS->dev0.fix_pri          = 0 ;
  KGS->dev0.dis_pre          = 0 ;
  KGS->dev0.dev              = (struct dev_info*) (kgs_mem_for_dev_info +
							  shm_start);
  INIT_LOCK( KGS->dev0.alone);

/*
 * all RTE processes initialized
 */

/*
 * Initialize DEVICE drivers
 */

  for ( devpcb = KGS->first_dev ; devpcb <= KGS->last_dev ; devpcb++ )
  {
    int num_elem = devpcb < KGS->first_asdev 
			     ? kgs_number_of_tasks 
			     : num_asynio_tasks * MAX_ASYN_REQUESTS ;
    devpcb->thread           = NULL;
    devpcb->tid              = UNDEF;
    devpcb->semid            = UNDEF;
    devpcb->state            = KT_INITIALIZED;
    devpcb->cpu_id           = 0;
    devpcb->start_time       = (time_t)0;
    devpcb->fSemAlreadyPosted = 0;
    devpcb->ukt              = (ten50_UKT_Control *)0;
    devpcb->fd               = -1;
    devpcb->dev_type         = UNDEF;
    devpcb->request          = NULL;
    devpcb->sfd              = UNDEF;
    devpcb->set_io_error     = UNDEF;
    devpcb->devcap           = UNDEF;
    devpcb->ios  =  (struct IOC_QUEUE *) (kgs_mem_for_ios_q + shm_start);
    kgs_mem_for_ios_q  += num_elem * sizeof ( struct IOC_QUEUE ) ;
    e73_dev_ioc_queue_init ( devpcb->ios , num_elem );
    devpcb->io_work          = devpcb->ios;
    devpcb->io_request       = devpcb->ios;
    INIT_LOCK( devpcb->alone);
    DBG1 (( MF__,"dev 0x%08lx 0x%08lx \n",
		    (long) devpcb , (long) devpcb->ios ));
  }

  common_queue_element_object_buffer = (char*) (kgs_common_queue_element_object + shm_start);
  synchronized_common_queue_buffer   = (char*) (kgs_synchronized_common_queue_free_list + shm_start);
  asyn_req_buffer                    = (char*) (kgs_first_asyn_req_object + shm_start);

/*
 * Initialize Free_list
 */

  pCommonQueueElements = newarray ( pCommonQueueElements, number_of_freelist, common_queue_element_object_buffer );

  RTETask_SynchronizedCommonQueue* pFreeList;

  pFreeList = new (synchronized_common_queue_buffer) RTETask_SynchronizedCommonQueue(pCommonQueueElements, number_of_freelist, "GlobalCommonQueueFreeList");

  RTE_KGS::Instance().SetGlobalFreeList( *pFreeList );

  /*
   * create unconnected task pool
   */ 
  RTETask_UnconnectedTaskPool::Strategy strategy =
                                          pCluInfo->lConnectStrategy == STRAT_EQUALIZE ? 
                                          RTETask_UnconnectedTaskPool::Equalize : 
                                          RTETask_UnconnectedTaskPool::Compress;

  kernel_communication_buffer = (char*) (kgs_kernel_communication + shm_start);

  RTECKC_KernelCommunication* 
      pKernelCommunication = new (kernel_communication_buffer) RTECKC_KernelCommunication (strategy, SearchUnreleasedConnectionsCallback);

  RTE_KGS::Instance().SetKernelCommunication( *pKernelCommunication );

/*
 * Initialize all tasks
 */
  DBG1 (( MF__,"pFirstTaskCtrl  0x%08x \n", KGS->pFirstTaskCtrl ));
  DBG1 (( MF__,"pLastTaskCtrl   0x%08x \n", KGS->pLastTaskCtrl ));
  task_index = 1 ;
  for ( tcb = KGS->pFirstTaskCtrl ; tcb <= KGS->pLastTaskCtrl ; tcb ++ )
  {
    tcb->this_tcb          = tcb ;
    tcb->next_task         = 0 ;
    tcb->prev_task         = 0 ;
    tcb->state             = TSK_INITIALIZED_EO00 ;
    tcb->index             = task_index ++;
    tcb->StackChunkStart   = (UCHAR *)0 ;
    tcb->StackChunkEnd     = (UCHAR *)0 ;
    tcb->ukt               = 0 ;
	tcb->taskThreadId      = UNDEF; /* PTS 1111201 */
    tcb->type              = 0 ;
    /* PTS 1104926 */
    INIT_LOCK(tcb->exclusive);
    tcb->resume_count = 0 ;
    tcb->signal_count      = 0 ;
    tcb->vsignal_ok        = 0 ;
    tcb->prio_flag         = FALSE ;
    tcb->aio_op            = (void *)(shm_start + kgs_mem_for_sys_asyn_struc) ;
    kgs_mem_for_sys_asyn_struc += SizeOfSysAsynIOStruct ;
    tcb->aio_in_progress   = 0;
    tcb->reg_excl_cnt = (int *) (shm_start + kgs_mem_for_reg_stat_cnt ) ;
    kgs_mem_for_reg_stat_cnt += XParam->ulNoOfRegions * sizeof ( tsp00_Int4 ) ;
    tcb->reg_coll_cnt = (int *) (shm_start + kgs_mem_for_reg_stat_coll ) ;
    kgs_mem_for_reg_stat_coll+= XParam->ulNoOfRegions * sizeof ( tsp00_Int4 ) ;
    tcb->lWaitForDevno        = undef_ceo00 ;
    tcb->lWaitForRoot         = undef_ceo00 ;
    tcb->lWaitForTask         = undef_ceo00 ;
    tcb->DevNameWaitingFor    = NULL ;
    
    tcb->pCurrStackPointer    = 0; /* PTS 1110829 */

    tcb->maxStackPointer      = 0; /* PTS 1110953 */
    tcb->commandMaxStack      = 0;
    tcb->totalMaxStack        = 0;
    tcb->lockedPageNo         = MAX_INT4_SP00; /* PTS 1115172 */

    SAPDB_memset ( &tcb->runnableMicroSeconds,    0, sizeof ( tcb->runnableMicroSeconds ) );
    SAPDB_memset ( &tcb->maxRunnableMicroSeconds, 0, sizeof ( tcb->maxRunnableMicroSeconds ) );
  }

/*
 * Initialize Kernel Processes
 */

  DBG1 (( MF__,"first_kp    0x%08x \n", KGS->first_kp ));
  DBG1 (( MF__,"last_kp     0x%08x \n", KGS->last_kp  ));
  ukt_index = 1 ;

    /* PTS 1108768 Space for Tracebuffer used for InMemory tracing */
    /* PTS 1108957 Allokation verlagert */
    /* PTS 1108975 Individual trace sizes */
    kgsTotalTraceBufferSizeInPages = XParam->lTraceDevspaceSize;
    memoryForTraceBuffer = en70_AllocSystemPages(DBPAGESIZE * kgsTotalTraceBufferSizeInPages);
    if ( !memoryForTraceBuffer )
    {
        MSGD ((IERR_VMALLOC_MALLOC_SYS_ERR, "Failed to allocate TRACE space. Please check the critical database parameters" )) ;
        MSGD ((IERR_VMALLOC_MALLOC_SYS_ERR, "These are MAXUSERTASKS, MAXSERVERTASKS, CACHE_SIZE and TRACE_PAGES_..." ));
  	    en70_RemoveKgs ( &kgs_shmid ) ;
        return ( 2 ) ;
    }

    nextUktTraceBuffer  = (char *) (memoryForTraceBuffer);
    nextUktTraceBuffer += DBPAGESIZE; /* First page is infopage */

  for (lkp = KGS->first_kp; lkp <= KGS->last_kp; lkp++ )
  {
    DBG1 (( MF__,"UKT        0x%08x \n", lkp ));

    lkp->index          = ukt_index ++ ;
    lkp->thread         = NULL;
    lkp->tid            = UNDEF;
    lkp->semid          = UNDEF ;
    lkp->state          = KT_INITIALIZED ;
    lkp->fSemAlreadyPosted = 0;
    lkp->stacksize      = pCluInfo->pThrdInfo[THREAD_INDEX(lkp)-1].ulStackSize ;
    lkp->pFirstTaskCtrl     = 0 ;
    lkp->ukp_timeout    = MAXTIMEVALUE ;
    lkp->taskSwitches   = 0;

    lkp->cpu_id         = pCluInfo->pThrdInfo[THREAD_INDEX(lkp)-1].lBindToCPU ;
    lkp->nice           = pCluInfo->pThrdInfo[THREAD_INDEX(lkp)-1].lThrdPrio ;
    lkp->dis_pre        = pCluInfo->pThrdInfo[THREAD_INDEX(lkp)-1].fDisablePreemption ;
    lkp->fix_pri        = pCluInfo->pThrdInfo[THREAD_INDEX(lkp)-1].fFixedPriority ;
    DBG1 (( MF__,"ukt %d nice %d \n", THREAD_INDEX(lkp), lkp->nice ));
    DBG1 (( MF__,"    cpu_id %d \n", lkp->cpu_id ));
    DBG1 (( MF__,"   fix_pri %d \n", lkp->fix_pri ));
    DBG1 (( MF__,"   dis_pre %d \n", lkp->dis_pre ));
    DBG1 (( MF__,"ioc  head is 0x%08x \n", &lkp->ioc_queue ));

    {
      int tot_tasks = kgs_number_of_tasks;
      int tot_aioc_tasks =  XParam->fUseAsyncIO ? tot_tasks : 1 ; 

      DBG1 (( MF__,"UKT %d, tot_tasks %d\n", THREAD_INDEX(lkp), tot_tasks ));
      DBG1 (( MF__,"UKT %d, tot_aioc_tasks %d\n", THREAD_INDEX(lkp), tot_aioc_tasks ));

      lkp->RunQueue = (struct RUN_QUEUE *) (kgs_mem_for_run_queues + shm_start);
      kgs_mem_for_run_queues += MAX_REQ_PER_TASK * tot_tasks * 
						        sizeof ( struct RUN_QUEUE ) ;

      lkp->aiocs = (struct IOR_QUEUE *) (kgs_mem_for_queues + shm_start);
      kgs_mem_for_queues += tot_aioc_tasks * sizeof ( struct IOR_QUEUE ) ;
      e73_ior_queue_init ( &lkp->aioc_queue, lkp->aiocs, (int)tot_aioc_tasks );

      /*
       *  Each UKT may contain the task which is allowed to issue
       *  MAX_ASYN_REQUESTS per devspace and per tape.
       */
      lkp->iocs = (struct IOC_QUEUE *) (kgs_mem_for_queues + shm_start);
      kgs_mem_for_queues += tot_tasks * sizeof ( struct IOC_QUEUE );
      kgs_mem_for_queues += (maxdevspaces + MaxBackupDevs ) *
		        MAX_ASYN_REQUESTS * sizeof ( struct IOC_QUEUE );
      e73_ioc_queue_init ( &lkp->ioc_queue, lkp->iocs, (int)tot_tasks +
		      (maxdevspaces + MaxBackupDevs ) 
					       * MAX_ASYN_REQUESTS );

      lkp->xxx = (struct XXX_QUEUE_HEAD **) ( kgs_mem_xxx_q_head + shm_start );
      kgs_mem_xxx_q_head += sizeof(struct XXX_QUEUE_HEAD *) * kgs_ukt_count;

      lkp->pSingleIO = (PUKT_SINGLE_IO_REC) (kgs_mem_for_ukp_io + shm_start);
      kgs_mem_for_ukp_io += sizeof(struct UKP_IO_STRUCT) * max_ukpio_devs ;
    }
    INIT_LOCK(lkp->exclusive);

    /*
     * Initialize local-I/O variables
     */

    lkp->allow_single_io    = XParam->fAllowSingleIO ;
    lkp->wait_for_ioproc_io = 0 ;

    SAPDB_memset ( &lkp->aio, 0 , sizeof ( lkp->aio ) );

    for ( count = 0 ; count < KGS->max_ukpio_devs ; count ++ )
    lkp->pSingleIO[count].ukp_attached_fd = UNDEF ;

    /* PTS 1108976 */
    lkp->traceBufferSizeInPages = pCluInfo->pThrdInfo[THREAD_INDEX(lkp)-1].tracePages ;
    lkp->traceBuffer            = (tsp00_PageAddr)nextUktTraceBuffer;
    nextUktTraceBuffer         += lkp->traceBufferSizeInPages * DBPAGESIZE;
  }
/* initialize UKTs */

  tcb  = KGS->pFirstTaskCtrl ;
  DBG1 (( MF__,"-------------------------- \n" ));

#define INSERT_TASK(_pos, _type, _stack, _glob) \
	lkp = KGS->first_kp + pCluInfo->_pos - 1 ; \
	rc = en70_InsertTask ( tcb , _type, _stack, lkp , 1 ); \
	if ( rc != 0 ) \
	  return ( rc ); \
	_glob = tcb

/*
 *      TIMEOUT
 */
  /* No own entry for task stack . Reuse entry for data writer task... */
  INSERT_TASK(TI_Pos, TT_TI_EO00, pCluInfo->VarTaskCntRec[VAR_TASK_DW_EO70K].lStackSize, KGS->ti);
  tcb ++;
/*
 *      ARCHIVELOGWRITER
 */
  /* No own entry for task stack ... Reuse entry for data writer task... */
  INSERT_TASK(AL_Pos, TT_AL_EO00, pCluInfo->VarTaskCntRec[VAR_TASK_DW_EO70K].lStackSize, KGS->al);
  tcb ++;
/*
 *      TRACEWRITER
 */
  /* No own entry for task stack ... Reuse entry for data writer task... */
  INSERT_TASK(TW_Pos, TT_TW_EO00, pCluInfo->VarTaskCntRec[VAR_TASK_DW_EO70K].lStackSize, KGS->tw);
  tcb ++;
/*
 *      DATAWRITER
 */
  KGS->first_datawriter = tcb ;
  for ( lkp = KGS->first_kp ; lkp <= KGS->last_kp ; lkp ++ )
  {
    count = pCluInfo->pThrdInfo[THREAD_INDEX(lkp)-1].lDWTasks ;
    if ( count > 0 )
    {
      rc = en70_InsertTask ( tcb , TT_DW_EO00, pCluInfo->VarTaskCntRec[VAR_TASK_DW_EO70K].lStackSize, lkp , count );
      tcb += count ;
    }
  }

/*
 *      EVENT_TASKS
 */
  KGS->first_event_task = tcb ;
  for ( lkp = KGS->first_kp ; lkp <= KGS->last_kp ; lkp ++ )
  {
    count = pCluInfo->pThrdInfo[THREAD_INDEX(lkp)-1].lEVTasks ;
    if ( count > 0 )
    {
      rc = en70_InsertTask ( tcb , TT_EV_EO00, pCluInfo->VarTaskCntRec[VAR_TASK_DW_EO70K].lStackSize, lkp , count );
      tcb += count ;
    }
  }

/*
 *      BACKUP_TASK
 */
  KGS->first_backup_task = tcb ;
  for ( lkp = KGS->first_kp ; lkp <= KGS->last_kp ; lkp ++ )
  {
    count = pCluInfo->pThrdInfo[THREAD_INDEX(lkp)-1].lBUPTasks ;
    if ( count > 0 )
    {
      rc = en70_InsertTask ( tcb , TT_BUP_EO00, pCluInfo->VarTaskCntRec[VAR_TASK_BUP_EO70K].lStackSize, lkp , count );
      tcb += count ;
    }
  }

/*
 *      GARBAGE_COLLECTOR
 */
  KGS->first_garbage_collector = tcb ;
  for ( lkp = KGS->first_kp ; lkp <= KGS->last_kp ; lkp ++ )
  {
    count = pCluInfo->pThrdInfo[THREAD_INDEX(lkp)-1].lGCTasks ;
    if ( count > 0 )
    {
      rc = en70_InsertTask ( tcb , TT_GC_EO00, pCluInfo->VarTaskCntRec[VAR_TASK_GC_EO70K].lStackSize, lkp , count );
      tcb += count ;
    }
  }

/*
 * Server tasks
 */
  DBG1 (( MF__,"Creating %d server tasks \n", XParam->ulMaxServer  ));
  DBG1 (( MF__,"-------------------------- \n" ));
  KGS->first_server =  XParam->ulMaxServer != 0 ? tcb : 0 ;

  for ( lkp = KGS->first_kp ; lkp <= KGS->last_kp ; lkp ++ )
  {
    count = pCluInfo->pThrdInfo[THREAD_INDEX(lkp)-1].lServerTasks ;
    if ( count > 0 )
    {
      rc = en70_InsertTask ( tcb , TT_SV_EO00, pCluInfo->VarTaskCntRec[VAR_TASK_SV_EO70K].lStackSize, lkp , count );
      tcb += count ;
    }
  }

  for ( tcb = KGS->first_server, i=0; i < XParam->ulMaxServer; i++, tcb++ )
  {
    int ii; 

    tcb->asyn_requests = newarray ( tcb->asyn_requests, MAX_ASYN_REQUESTS, asyn_req_buffer );
    asyn_req_buffer += sizeof(RTETask_CommonQueueElement) * MAX_ASYN_REQUESTS;

    for ( ii=0; ii < KGS->max_asyn_request; ii++ )
    {
      DLQ_ELEMENT*  element  = static_cast<DLQ_ELEMENT*>(tcb->asyn_requests[ii]);  
      element->args.vai_parms.aio_op = (void *)(shm_start + kgs_mem_for_sys_asyn_struc) ;
      kgs_mem_for_sys_asyn_struc += SizeOfSysAsynIOStruct  ; 
    }
  }

/*
 *      UTILITY
 */
  INSERT_TASK(UT_Pos, TT_UT_EO00, XParam->ulMaxUsUtStack, KGS->ut); /* PTS 1106765 */
  tcb->asyn_requests = newarray ( tcb->asyn_requests, MAX_ASYN_REQUESTS, asyn_req_buffer );
  asyn_req_buffer += sizeof(RTETask_CommonQueueElement) * MAX_ASYN_REQUESTS;

  for ( i=0; i < KGS->max_asyn_request; i++ )
  {
    DLQ_ELEMENT*  element = static_cast<DLQ_ELEMENT*>( tcb->asyn_requests[i]);
    element->args.vai_parms.aio_op = (void *)(shm_start + kgs_mem_for_sys_asyn_struc) ;
    kgs_mem_for_sys_asyn_struc += SizeOfSysAsynIOStruct  ; 
  }
  tcb ++ ;

/*
 * User tasks
 */
  DBG1 (( MF__,"Creating %d user tasks \n", XParam->ulMaxUser  ));
  DBG1 (( MF__,"-------------------------- \n" ));
  KGS->first_user = tcb ;

  for ( lkp = KGS->first_kp ; lkp <= KGS->last_kp ; lkp ++ )
  {
    count = pCluInfo->pThrdInfo[THREAD_INDEX(lkp)-1].lUserTasks ;
    if ( count > 0 )
    {
      rc = en70_InsertTask ( tcb , TT_US_EO00, XParam->ulMaxUsUtStack, lkp , count );
      tcb += count ;
    }
  }

  tcb--;
  if ( tcb != KGS->pLastTaskCtrl )
  {
    MSGCD (( IERR_INITSHM_TSK_NOT_LSTTSK, tcb  ))
    MSGCD (( IERR_INITSHM_LSTTSK_INFO, KGS->pLastTaskCtrl ))
    en70_RemoveKgs ( &kgs_shmid ) ;
    return ( 2 );
  }

  for ( tcb = KGS->first_user; tcb <= KGS->pLastTaskCtrl; tcb++ )
  {
    int ii; 
    tcb->asyn_requests = newarray ( tcb->asyn_requests, MAX_ASYN_REQUESTS, asyn_req_buffer );
    asyn_req_buffer += sizeof(RTETask_CommonQueueElement) * MAX_ASYN_REQUESTS;

    for ( ii=0; ii < KGS->max_asyn_request; ii++ )
    {
      DLQ_ELEMENT*  element  = static_cast<DLQ_ELEMENT*>(tcb->asyn_requests[ii]);  
      element->args.vai_parms.aio_op = (void *)(shm_start + kgs_mem_for_sys_asyn_struc) ;
      kgs_mem_for_sys_asyn_struc += SizeOfSysAsynIOStruct  ; 
    }
  }

/*
 *  Initialize each UKTs xxx
 */
  l_xxx = KGS->first_xxx ;
  for ( lkp = KGS->first_kp ; lkp <= KGS->last_kp ; lkp ++ )
  {
    int tot_tasks;
    if ( XParam->fForbidLoadBalancing )
    {
        tot_tasks = pCluInfo->pThrdInfo[THREAD_INDEX(lkp)-1].ulNumOfTasks + 1;
    }
    else
    {
        tot_tasks = kgs_number_of_tasks + 1;
    }
    DBG1 (( MF__,"xxx pUKTCtrl 0x%08x \n", lkp ));
    DBG1 (( MF__,"xxx ukt P%d \n", THREAD_INDEX(lkp) ));
    for ( count = 0 ; count < KGS->ulNumOfUKTs ; count++ )
    {
      if ( count == THREAD_INDEX(lkp) - 1 ) continue ;
      DBG1 (( MF__,"xxx dst P%d \n", count + 1 ));
      DBG1 (( MF__,"xxx     0x%08x \n", l_xxx ));
      l_xxx->element = (struct XXX_QUEUE *) (kgs_mem_xxx_queues + shm_start);
      kgs_mem_xxx_queues += tot_tasks * sizeof ( struct XXX_QUEUE ) ;
      DBG1 (( MF__,"xxx[%d][%d]->elem = %X \n", 
		      THREAD_INDEX(lkp), count + 1, l_xxx->element ));
      e73_xxx_queue_init ( l_xxx, tot_tasks );
      lkp->xxx [ count ] = l_xxx ;
      l_xxx ++ ;
    }
  }

 /*
  *  Create a task scheduler object per UKT
  */
  task_scheduler_object_buffer = (char*) (kgs_first_task_scheduler_object + shm_start);

  RTETask_TaskScheduler* pLastTaskScheduler = 0;

  // we step backward through the UKT list to get a correct TaskScheduler next-chain
  for ( task_scheduler_object_buffer += (sizeof(RTETask_TaskScheduler) * (kgs_ukt_count-1)),
        lkp = KGS->last_kp; 
        lkp >= KGS->first_kp; 
        lkp --,
        task_scheduler_object_buffer -= sizeof(RTETask_TaskScheduler) )
  {
    lkp->pTaskSchedulerObject = new (task_scheduler_object_buffer) RTETask_TaskScheduler( lkp, pLastTaskScheduler );
    pLastTaskScheduler = lkp->pTaskSchedulerObject;
    
  }
  // the last will be the first...
  RTE_KGS::Instance().SetTaskSchedulerList( *pLastTaskScheduler );

 /*
  *  Create a task object per task
  */
  task_object_buffer = (char*) (kgs_first_task_object + shm_start);

  for ( tcb = KGS->pFirstTaskCtrl; tcb <= KGS->pLastTaskCtrl ; tcb ++ )
  {
      tcb->pTaskObject = new (task_object_buffer) RTETask_Task( tcb->index, static_cast<RTETask_Task::TaskType>(tcb->type), tcb );
  
    task_object_buffer += sizeof(RTETask_Task);
  }

  {
    int idx;
    KGS->semaphore = ( struct DDB4_SEMA *)
					   ( kgs_mem_for_regions + shm_start ) ;
    for (idx = 0 ; idx < XParam->ulNoOfRegions; idx++ )
    { KGS->semaphore[idx].semaliste.sem = 
		       ( struct N_QUEUE *) ( kgs_mem_for_reglist + shm_start ) ;
      kgs_mem_for_reglist += kgs_number_of_tasks * sizeof( struct N_QUEUE ) ;
    }
  }

  e74_init_comdata ( XParam , 0, pCluInfo  );

  if ( pCluInfo )
  {
    FREE_MEM_EO57( pCluInfo );
  }

#ifdef DEBUG_RTE
/*
 *  Now, list what we have created.
 */
  DBG1 (( MF__,"--------------------- \n" ));
  DBG1 (( MF__,"TASK LIST \n" ));
  for ( tcb = KGS->pFirstTaskCtrl ; tcb <= KGS->pLastTaskCtrl ; tcb ++ )
  {
    DBG1 (( MF__,"--------------------- \n" ));
    DBG1 (( MF__,"task tcb   0x%08x \n", tcb            ));
    DBG1 (( MF__,"task index T%d    \n", tcb->index     ));
    DBG1 (( MF__,"task ukt   P%d    \n", THREAD_INDEX(tcb->ukt) ));
    DBG1 (( MF__,"task type  %d     \n", tcb->type      ));
    DBG1 (( MF__,"task next  0x%08x \n", tcb->next_task ));
    DBG1 (( MF__,"task prev  0x%08x \n", tcb->prev_task ));
  }
  DBG1 (( MF__,"--------------------- \n" ));
  DBG1 (( MF__,"UKT LIST \n" ));
  for ( lkp = KGS->first_kp ; lkp <= KGS->last_kp ; lkp ++ )
  {
    DBG1 (( MF__,"--------------------- \n" ));
    DBG1 (( MF__,"UKT%d   pUktCtrl 0x%08x \n", 
		    THREAD_INDEX(lkp) , lkp ));
    for ( tcb = lkp->pFirstTaskCtrl ; tcb ; tcb = tcb->next_task )
    DBG1 (( MF__,"UKT task  T%d    \n", tcb->index     ));
  }
  DBG1 (( MF__,"--------------------- \n" ));
#endif  /*DEBUG_RTE*/

  KGS->XParam = (struct rte_xparam_record*) (shm_start + kgs_rte_xp_size ) ;

  SAPDB_memcpy ( KGS->XParam , XParam, sizeof ( RTE_XPARAM_REC) )  ;

  for ( idx = 0; idx < 8; idx++ )
  { 
    ULONG NumDevs      = XParam->DevspDescr[idx].ulNumDevs ;
    if ( NumDevs > 0 )
    {
      ULONG DevDescrSize = NumDevs * sizeof ( DEV_DESCR ) ;
      maxdevspaces -= NumDevs ;
      if ( maxdevspaces < 0 ) 
      {
        MSGCD (( IERR_INITSHM_TO_MANY_DEVSP )) ;
        en70_RemoveKgs ( &kgs_shmid ) ;
        return ( 2 );
      }
      XPARAM(DevspDescr)[idx].Devspaces = 
			 (DEV_DESCR *) (shm_start + kgs_devsp_descr) ;
      DBG1 (( MF__,"DevspDescr[%d] = %8X (%d Devsp)\n", idx,
					XParam->DevspDescr[idx].Devspaces , NumDevs )) ;
      SAPDB_memcpy ( XPARAM(DevspDescr)[idx].Devspaces ,
		       XParam->DevspDescr[idx].Devspaces, DevDescrSize ) ; 
      RTEConf_FreeDevDescrInXParamStructure(XParam,idx);
      kgs_devsp_descr += DevDescrSize ;
    }
  }

  e55_init_vbegexcl_func_ptr( kgs_ukt_count ) ;

  if ( XParam->fUseAsyncIO )
  {
    MSGD(( INFO_INITSHM_ASYNIO_SUPP ));
    e54a_init_sys_aio_struc () ;
  }

  /*
   * Allocate Tracebuffer in Shared Memory Segment...
   */
  if ( XParam->ulMaxMessageFiles > 0 )
  {
    KGS->MessageBufferSpace = (void *) (kgsMemoryForMessageBuffer + shm_start);
  }
  else
  {
    KGS->MessageBufferSpace = NULL;
  }

    /* PTS 1108976 */
    KGS->traceBufferSpace = (tsp00_PageAddr) memoryForTraceBuffer;
    KGS->totalTraceBufferSizeInPages = kgsTotalTraceBufferSizeInPages;
    KGS->traceNeedsSaving = 0;

    /* PTS 1115180 */
    KGS->dumpDiagFiles = false;       

  DBGOUT;
  return ( 0 );
}

/*---------------------------------------------------------------------------
  Function: e70_is_kgsshmid
  Description: Test given shared memory identify against the segements allocated

  Arguments: shmid [in]the shared memory identifier to test
  Return value: true if found in array of identifiers
                false if not
 */
#undef MF__
#define MF__ MOD__"e70_is_kgsshmid"
int e70_is_kgsshmid ( int shmid )
{
  return ( KGS->kgsshmid && *(KGS->kgsshmid) == shmid ) ;
}

/*
 * ======================================================================
 * LOCAL FUNCTIONS
 * ======================================================================
 */
/*---------------------------------------------------------------------------
  Function: en70_InsertTask
  Description: Insert tasks of given type in chain of tasks in given ukt
  Arguments: tcb [in] The task to insert
             task_type [in] The task type
             stack_size [in] Stacksize for task
             pUktCtrl [in] The UKT control block
             count [in] Number of tasks to insert
  Return value: 0 if successfull
                3 if tcb out of range (kgs removed)
 */
#undef MF__
#define MF__ MOD__"en70_InsertTask"
static  int     en70_InsertTask ( 
struct  TASK_TYPE   * tcb ,
int                 task_type ,
int                 stack_size ,
ten50_UKT_Control   * pUktCtrl ,
int                 count )
{
  int                             idx ;
  int                             j ;
  struct  TASK_TYPE            ** stcb ;
  struct  TASK_TYPE            *  prev_tcb ;

  if ( tcb > KGS->pLastTaskCtrl)
  {
    MSGCD (( IERR_INITSHM_TCB_AFTER_LST, tcb ))
    MSGCD (( IERR_INITSHM_TCB_AFTER_LST_I , KGS->pLastTaskCtrl ))
    en70_RemoveKgs ( KGS->kgsshmid ) ;
    return ( 3 );
  }

/*
 *  find end of task queue
 */
  DBG1 (( MF__,"searching for end of task in P%d\n", pUktCtrl->index ));
  for ( stcb = & pUktCtrl->pFirstTaskCtrl, prev_tcb = NULL, idx = 0, j = 0;
	    * stcb; 
        prev_tcb = *stcb, stcb = & (*stcb)->next_task ) ;

  DBG1 (( MF__,"end of task is T%d\n", tcb->index ));
  DBG1 (( MF__,"try to insert %d Tasks\n", count ));
  for ( idx = 0 ; idx < count ; prev_tcb = tcb++, idx++ , stcb = & (*stcb)->next_task ) 
  { 
    tcb->ukt                   = pUktCtrl ;
    tcb->bigComSegBelongsToUKT = pUktCtrl ; /* we need this because of task load balancing */
    tcb->type                  = task_type;
    tcb->stack_size            = stack_size ;
    tcb->yieldRequestAtPos     = -1;

    switch ( tcb->type )
    {
    case TT_US_EO00:
    case TT_UT_EO00:
    case TT_EV_EO00:
      /*  mark as inactive */
      tcb->state       = TSK_INACTIVE_EO00;
      tcb->connectable = true;
      break;
    default:
      tcb->connectable = false;
      break;
    }

    tcb->moveLock.val = 0;  // reset lock

    tcb->moveLock.type.longTerm = TASK_IS_INACTIVE; /* Set move lock. Will be removed
                                                        by the first 'vreceive' */
    if ( tcb->type != TT_US_EO00 )
        tcb->moveLock.type.longTerm |= NEVER_MOVE_TASK;
    else
        pUktCtrl->tasksAllowedToMoveTo = SAPDB_TRUE;

    en71AlterTaskCnt( tcb, ADD_TOTAL_COUNTER );

    tcb->prev_task = prev_tcb;  /* prev. pointer */
    *stcb          = tcb;       /* next pointer  */

#ifdef DEBUG_RTE
    {
      char *task_name = (char *) e38_type_name ( task_type ) ;
      DBG1 (( MF__,"%s tcb        0x%08x \n", task_name, tcb        ));
      DBG1 (( MF__,"%s index      %d \n", task_name, tcb->index ));
      DBG1 (( MF__,"%s kp index   %d \n", task_name, THREAD_INDEX(tcb->ukt) ));
      DBG1 (( MF__,"%s type       %d \n", task_name, tcb->type  ));
      DBG1 (( MF__,"-------------------------- \n"));
    }
#endif
  }

  return ( 0 );
}

/*---------------------------------------------------------------------------
  Function: en70_InitProc
  Description: Initialize the kernel threads common values

  Arguments: kernelThread [in] kernel thread to initialize
 */
#undef MF__
#define MF__ MOD__"en70_InitProc"
static  void    en70_InitProc ( 
ten50_KT_Control       * kernelThread )
{
  kernelThread->state = KT_INITIALIZED ;
  kernelThread->semid = UNDEF ;
  kernelThread->thread = NULL;
  kernelThread->tid   = UNDEF;
}

/*---------------------------------------------------------------------------
  Function: en70_RemoveKgs
  Description: Remove the kernel global section

  Removes the shared memory section used for kernel global information
  Arguments: shmids [in] array of shared memory ids
 */
#undef MF__
#define MF__ MOD__"en70_RemoveKgs"
static  void    en70_RemoveKgs ( int *shmids )
{
    DBGIN;

    (void) sql41_remove_shm ( shmids , (char*)"db" , en81_dbname );

  DBGOUT;

  return ;
}

/*---------------------------------------------------------------------------*/

/* PTS 1108885 Linux has problem with mmap ... */
#ifdef LINUX
#define HAS_NON_WORKING_MMAP_SHARED
#else
#undef HAS_NON_WORKING_MMAP_SHARED
#endif

static void * en70_AllocSystemPages(size_t SizeInBytes)
{
  size_t SystemSize = (SizeInBytes + en81_MemPageSize - 1) & (~(en81_MemPageSize-1));
  void *ChunkAddress = NULL;

  /* PTS 1108885 MAP_PRIVATE replaced by MAP_SHARED */
  if ( SystemSize > 0 )
  {
#ifdef HAS_NON_WORKING_MMAP_SHARED

    int traceShmid = sql41_create_shm ( IPC_PRIVATE,
        0 , 0 , SystemSize, 0600 , (int)getuid() , "db" , en81_dbname );

    ChunkAddress = (char *) sql41AttachShm ( traceShmid, 0, 0 );

#else

    ChunkAddress = (void *)mmap( 0, SystemSize, PROT_READ|PROT_WRITE, 
                                 MAP_SHARED|MAP_ANONYMOUS, -1, 0 ); /* PTS 1108885 */
    if ( ChunkAddress == (void *) MAP_FAILED )
    {
      ChunkAddress = (void *)0;
    }

#endif
  }

  return ( ChunkAddress );
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
