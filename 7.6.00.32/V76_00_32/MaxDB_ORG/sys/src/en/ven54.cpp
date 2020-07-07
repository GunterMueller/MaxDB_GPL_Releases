/*!
  @file           ven54.cpp
  @author         JoergM
  @brief          Disk IO: Kernel IO Routines and Threads
  
    Directly translated from ven54.c...

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


//
//   FreeBSD portions added by Kai Mosebach,
//   For more informations see : http://www.komadev.de/sapdb
//

\endif
*/

#undef MOD__
#define MOD__  "ven54.c:"

#if defined(LINUX)
/* this enables uses of system define for O_DIRECT on Linux by indirectly definition of __USE_GNU in <features.h>... */
#define _GNU_SOURCE
#endif /* LINUX */

#if SVR4 || SOLARIS
#include        <wait.h>
#endif

#ifdef  DEBUG_RTE
#include        <ctype.h>
#endif

#if defined(LINUX)
#include <bits/xopen_lim.h>
#endif /* LINUX */

#define        NO_PRIO                  0
#define        OK                       0
#define        IO_LIMIT                10
#define        VB_NOTOK                 0
#define        VB_OK                    1
#define        VB_NO_MORE_FILES         2
#define        SAPDB_IOV_LIMIT         16
#define MAX_VDETACH_WAIT_TIME              30        /* -- sleep time */

/* PTS 1105707 */
#if !defined(BIT64) && defined(O_LARGEFILE)
#define DEFAULT_OPEN_MODE_EN54 ( O_LARGEFILE )
#else
#define DEFAULT_OPEN_MODE_EN54 ( 0 )
#endif

#define NO_REPLY_WANTED_EN54 (-1)

#define  WRN_TEST_WILDCARD                            11809,2,"TEST    ","%s"
#define  INFO_TEST_WILDCARD                           11809,3,"TEST    ","%s"

#define IS_TRACE_DEV(_devno)         (_devno == 0 )
#define IS_TAPE(_hostfileno) ( _hostfileno >= KGS->maxdevspaces * 2 )

#include "gen00.h"
#include "heo00.h"
#include "hen50.h" /* vabort() */
#include "heo52.h"
#include "geo50_0.h"
#include "gen500.h"
#include "geo54.h"
#include "gen54.h"
#include "heo54k.h"
#include "geo00_2.h"
#include "gen41.h"
#include "gen541.h"
#include "geo57.h"
#include "heo58.h"
#include "gen71.h"
#include "gen72.h"
#include "gen75.h"
#include "heo07.h"
#include "gen81.h"
#include "gen88.h"
#include "hen40.h"
#include "geo001.h"
#include "RunTime/InputOutput/RTEIO_VolumeInfo.h" /* 1109850 */
#include "RunTime/System/RTESys_MicroTime.h"
#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.h"
#include "RunTime/RTE_saveUNIXcalls.h" /* PTS 1118797 */
#include "RunTime/HotStandbyStorage/RTEHSS_PascalInterface.h"
#include "RunTime/Tasking/RTETask_LegacyTaskCtrl.h"
#ifdef HAVE_DAFS_SUPPORT
#  include "RunTime/InputOutput/RTEIO_DAFS.h" /* nocheck */
#endif
#include "hsp77.h"
#include "heo51.h"
#include "RunTime/RTE_KGS.hpp"
#include "RunTime/Tasking/RTETask_CommonQueueElement.hpp"

extern "C" tsp00_Bool RTE_CSetDatabasePathOwnership( SAPDB_Char const *volume );

#define EN54_NO_PIPE_CHECK_CODE_TESTED  0
#define EN54_FSTAT_PIPE_CHECK_CODE_OK   1
#define EN54_POLL_PIPE_CHECK_CODE_OK    2
#define EN54_NO_PIPE_CHECK_CODE_WORKING 3

#define EN54_TEST_PIPE_NAME             "/pipecheck_"
#define EN54_TEST_DONE_FILE             "/ckeckdone_"
#define EN54_TEST_PIPE_CONTENT          "Testing pipe code"
static SAPDB_Int en54_pipeCheckAlgorithm = EN54_NO_PIPE_CHECK_CODE_TESTED;
#define  FILE_SYSTEM_FULL  "I am sorry, but no space left on device"
#define  BEYOND_END_OF_VOLUME "Cannot read behind end of volume"

#ifdef WANT_DATAWRITE_TIME_INFO
#define INFO_TIME_START 11525, INFO_TYPE, "IOTIMING", "Task T%3d Timing %s Start %ld"
#define INFO_TIME_STOP  11525, INFO_TYPE, "IOTIMING", "Task T%3d Timing %s Start %ld End %ld Delta %ld"
#endif /* WANT_DATAWRITE_TIME_INFO */

#define EN54_SMALL_MESSAGE_BUFFER_SIZE 256

#if defined(LINUX)
#ifndef O_DIRECT
#error O_DIRECT not defined
#endif
#endif /* LINUX */

#if defined(O_DIRECT)
# define SAPDB_DIRECTIO O_DIRECT
#elif defined(O_DIRECTIO)
# define SAPDB_DIRECTIO O_DIRECTIO
#else
# undef SAPDB_DIRECTIO
#endif

#if defined(O_DSYNC)
# define SAPDB_SYNC_DATA_TO_DISK O_DSYNC
#else
# define SAPDB_SYNC_DATA_TO_DISK O_SYNC
#endif

/* local types */

struct en54_open_arg
{
    struct DOUBLY_LINKED      *request;
    void                      *thread;
    volatile SAPDB_Int               done;
};

struct ALIGNED_MEM
{
    UCHAR* baseAddr;
    UCHAR* dataAddr;
};

struct DEV_CNTL_INFO
{
    SAPDB_Int                         filedes;
    SAPDB_Int4                  hostfileno;
    struct stat                 statInfo;
    SAPDB_Int4                  block_size ;
    SAPDB_Int4                  max_block_cnt ;
    char *                      deviceName ;
    SAPDB_UInt4                 asyn_write ;
    tsp00_Bool                  destructiveCheck;
};

/* local functions */

static  SAPDB_Int                     en54_ResetNonBlockingFlag(SAPDB_Int fd);

static  SAPDB_Int                     en54_IsExistingPipe(const char *devname);

static  SAPDB_Int                     en54_OpenAsyncDevice( ten50_UKT_Control         * this_ukt ,
                                                      struct DOUBLY_LINKED      * request,
                                                      struct REQ_PARMS_ASYNOPEN * parms,
                                                      char                      * devname );

static  tsp00_Uint4             en54_find_device_no (
                                        struct DOUBLY_LINKED  *request,
                                        char                  *devname,
                                        tsp00_Uint4           *pErrCode);

static  tsp00_Bool              en54_Dev0Vattach (
                                        struct DOUBLY_LINKED   *request);

static  tsp00_Bool              en54_Dev0Vasynopen (
                                        struct DOUBLY_LINKED   *request);

static  tsp00_Bool              en54_DeviVasynopen (
                                        struct DEV_PCB         *devi_pcb);

static  tsp00_Bool              en54_DeviVattach (
                                        struct DEV_PCB         *devi_pcb);

static  void                    en54_DeviVblockio (
                                        struct DOUBLY_LINKED   *request,
                                        struct DEV_PCB         *devi_pcb,
                                        INTFUNC_PTR            *io_f,
                                        SAPDB_Int                     fd);

static  void                    en54_DeviVVectorio (
                                        struct DOUBLY_LINKED   *request,
                                        struct DEV_PCB         *devi_pcb,
                                        SAPDB_Int                     fd);

static  void                    en54_DeviVmarkBadDev  (
                                        struct DOUBLY_LINKED   *request,
                                        struct DEV_PCB         *devi_pcb,
                                        SAPDB_Int                     fd);

static  SAPDB_Int                     en54_DeviAsynioReadBlock(
                                        struct DEV_PCB          *devi_pcb,
                                        SAPDB_Int                      fd,
                                        struct REQ_PARMS_ASYNIO *parms,
                                        long                     length,
                                        long                     pno) ;

static SAPDB_Int                      en54_ReadWithRetry (SAPDB_Int                     fd,
                                               off_t                   offset,
                                               char                   *block,
                                               unsigned                block_len);
static  SAPDB_Int                     en54_ReadBlock (
                                        SAPDB_Int                     fd,
                                        tsp00_PageAddr          block,
                                        long                    block_len);

static  void                    en54_Dev0Vdetach (
                                        struct DOUBLY_LINKED   *request);

static  void                    en54_DeviVdetach (
                                        struct DOUBLY_LINKED   *request,
                                        struct DEV_PCB         *pcb);

static  void                    en54_Dev0Main (void);

extern "C" void                *en54_Mdevi_main( void *arg );

static  void                    en54_DeviMain (
                                        struct DEV_PCB         *pcb);

static  SAPDB_Int                     en54_Page0IO (
                                        SAPDB_Int                     fd,
                                        tsp2_io_op_code_Param         op_code,
                                        SAPDB_Int4               *page_0_size,
                                        SAPDB_Int4                capacity,
                                        tsp00_Bool              is_devspace,
                                        char                   *f_label);

static  SAPDB_Int                     en54_WrongCall (
                                        struct DEV_PCB         *devi_pcb,
                                        SAPDB_Int                     fd,
                                        char                   *buf,
                                        long                    length,
                                        long                    pno,
                                        SAPDB_UInt4            *errcode);

static  SAPDB_Int                     en54_DiskIORead (
                                        struct DEV_PCB         *devi_pcb,
                                        SAPDB_Int                     fd,
                                        char                   *buf,
                                        long                    length,
                                        long                    pno,
                                        SAPDB_UInt4            *errcode);

static  SAPDB_Int                     en54_DiskIOWrite (
                                        struct DEV_PCB         *devi_pcb,
                                        SAPDB_Int                     fd,
                                        char                   *buf,
                                        long                    length,
                                        long                    pno,
                                        SAPDB_UInt4            *errcode);

static  SAPDB_Int                     en54_DiskIOWriteVector (
                                        struct DEV_PCB         *devi_pcb,
                                        SAPDB_Int                     fd,
                                        tsp00_PageAddr          pageVector[],
                                        SAPDB_Int4              pageCount,
                                        SAPDB_Int4              pno,
                                        SAPDB_UInt4            *errcode);

static  SAPDB_Int                     en54_SimulateDiskIOWriteVector (
                                        struct DEV_PCB         *devi_pcb,
                                        int                     fd,
                                        tsp00_PageAddr          pageVector[],
                                        SAPDB_Int4              pageCount,
                                        SAPDB_Int4              pno,
                                        SAPDB_UInt4            *errcode);

static  SAPDB_Int                     en54_DevOpen (
                                        char                   *fname,
                                        tsp00_Bool              forceReadOnly,
                                        INTFUNC_PTR            *io_f,
                                        SAPDB_Int4               *fsize,
                                        SAPDB_Int                    *type);

static  struct DOUBLY_LINKED    *en54_dev0Receive (void);

static  struct DEV_PCB          *en54_FindDevpcb (
                                        ten50_UKT_Control    * this_ukt ,
                                        SAPDB_Int4                devno,
                                        tsp00_Bool                 prio,
                                        SAPDB_Bool              reader);

static  struct DEV_PCB          *en54_FindAsyncDevpcb ( SAPDB_Int4       devno );


static  void                    en54_ValidateReadOnlyVolumeSize( tsp2_devnamec volume,
                                                                 SAPDB_Int4    givenBlockSize,
                                                                 tsp00_Bool   *pForceReadOnly );

static  void                    en54_RegularFileVdevsize (
                                        tsp2_devnamec           volume,
                                        SAPDB_Int4                givenBlockSize,
                                        SAPDB_Int4               *devcapacity,
                                        tsp00_TaskId              taskResponsibleForCall,
                                        char                     *diskImage,
                                        SAPDB_Int                 isSequential,
                                        tsp00_Bool                forceReadOnly,
                                        tsp00_ErrText           & errtext,
                                        tsp00_Bool                *ok);

static  void                    en54_RawDiskVdevsize (
                                        tsp2_devnamec           volume,
                                        SAPDB_Int4                givenBlockSize,
                                        SAPDB_Int4               *devcapacity,
                                        tsp00_TaskId              taskResponsibleForCall,
                                        char                     *diskImage,
                                        SAPDB_Int                 isSequential,
                                        tsp00_Bool                forceReadOnly,
                                        tsp00_ErrText           & errtext,
                                        tsp00_Bool                *ok);

static  void                    en54_CloseSingleFilescs (
                                        ten50_UKT_Control     * this_ukt );

static  SAPDB_Int                     en54_SingleBlockIO (
                                        ten50_UKT_Control     * this_ukt,
                                        SAPDB_Int4                devno,
                                        tsp00_PageNo             devpno,
                                        tsp2_io_op_code_Param          op_code,
                                        tsp00_PageAddr           bufadr,
                                        SAPDB_Int2               buf_count,
                                        tsp00_ErrText          & errtext,
                                        tsp00_Bool                *ok);

static  SAPDB_Int                     en54_SingleVectorIO (
                                        ten50_UKT_Control     * this_ukt,
                                        SAPDB_Int4              devno,
                                        tsp00_PageNo             devpno,
                                        tsp00_PageAddr          pageVector[],
                                        SAPDB_Int2              pageCountcount,
                                        tsp00_ErrText         & errtext,
                                        tsp00_Bool                *ok);

static  SAPDB_Int                     en54_SimulateSingleVectorIO (
                                        ten50_UKT_Control     * this_ukt,
                                        SAPDB_Int4              devno,
                                        tsp00_PageNo             devpno,
                                        tsp00_PageAddr          pageVector[],
                                        SAPDB_Int2              pageCountcount,
                                        tsp00_ErrText         & errtext,
                                        tsp00_Bool                *ok);

#ifdef HAS_ASYNC_IO_SUPPORT
extern "C" SAPDB_Int                     e54_asyn_vblockio (
                                        struct DOUBLY_LINKED   *request,
                                        tsp00_ErrText         & errtext) ;

extern "C" SAPDB_Int                     e54_asyn_vasynio  (
                                        struct DOUBLY_LINKED   *request,
                                        tsp00_ErrText         & errtext) ;
#endif

static  void                    en54_NewDevsize (
                                        SAPDB_Int4                fd,
                                        tsp00_Bool                 devsp_is_file,
                                        SAPDB_Int4                devcapacity,
                                        SAPDB_Int4                givenBlockSize,
                                        tsp00_ErrText           & errtext,
                                        tsp00_Bool                *ok);

static  void                    en54_FormatDevspace (
                                        SAPDB_Int4                fd,
                                        SAPDB_Int4                devcapacity,
                                        SAPDB_Int4                givenBlockSize,
                                        tsp00_TaskId              taskResponsibleForCall,
                                        char                      *diskImage,
                                        tsp00_ErrText           & errtext,
                                        tsp00_Bool                *ok);

static  void                    en54_RequestToDev0 (
                                        struct DOUBLY_LINKED   *request);

static  void                    en54_RequestToDevi (
                                        struct DEV_PCB         *devi,
                                        struct DOUBLY_LINKED   *request );

static  void                    en54_RequestToUKT (
                                        struct DEV_PCB         *devi,
                                        struct DOUBLY_LINKED   *request );

static  UCHAR*                  en54_AllocAligned (
                                        struct ALIGNED_MEM     *memInfo,
                                        SAPDB_Int                     blockSize,
                                        SAPDB_Int                    *retcod);

static  void                    en54_FreeAligned (struct ALIGNED_MEM     *memInfo);

static  tsp00_Bool              en54_ValidHostfile (
                                        SAPDB_Int4               hostfileno,
                                        char const             * routineName,
                                        tsp00_VaReturn         * returncode,
                                        tsp00_ErrText          & errtext);

static  tsp00_Bool              en54_Rewind (
                                        SAPDB_Int                 filedes,
                                        SAPDB_Int4                hostfileno,
                                        tsp00_Bool                forClose,
                                        SAPDB_UInt4              *errcode);

static  tsp00_VfType             en54_StatFileType (
                                        struct  DEV_CNTL_INFO  *devCntlInfo,
                                        SAPDB_UInt4                  *returncode);

static  tsp00_Bool              en54_TouchVolumeIfNotExists ( char          * volume,
                                                              tsp00_Bool    * pForceReadOnly,
                                                              tsp00_ErrText & errtext ) ;

#if DUMP
static  void                    en54_Dump (
                                        char                    mode,
                                        SAPDB_Int4               dno,
                                        tsp00_PageNo             pno,
                                        tsp2_io_op_code_Param    op,
                                        tsp00_PageAddr           buf,
                                        SAPDB_Int2               cnt,
                                        tsp00_TaskId             taskId);
#endif

static void *en54_OpenWatchdog(void *pArg);

static SAPDB_Int en54_AddSynchronousOpen();

static void en54_CloseSingleIOFileDesc ( ten50_UKT_Control * pTmpUKTCtrl,
                                         SAPDB_Int4          lDevspaceNo );

static void en54_WaitForSelfIOCompletionAndClose(SAPDB_Int4 lDevspaceNo);

static tsp00_Bool ResetForcedReadOnlyIfHSDefaultMaster(tsp00_Bool *pIsForcedReadOnly);

static void en54_InitializeDevspace( SAPDB_Int fd,
                                     SAPDB_Int4 devcapacity,
                                     SAPDB_Int4 givenBlockSize,
                                     tsp00_Bool wantFormatDevspace,
                                     tsp00_TaskId taskResponsibleForCall,
                                     char      *diskImage,
                                     tsp00_ErrText & errtext,
                                     tsp00_Bool   *ok);

static off_t en54_SignalSave_lseek(int fd, off_t where, int whence);
static long en54_SignalSave_read(int fd, void *buffer, size_t size);
static long en54_SignalSave_read_partial(int fd, void *buffer, size_t size);
static long en54_SignalSave_write(int fd, void *buffer, size_t size);
tsp00_Bool RTE_CheckForSimulateVectorIo(tsp00_Bool isRawDevice );

/* local data */

static char const * ftype_text[] = {
    "unknown",
    "file",
    "pipe",
    "raw",
    "tape norewind",
    "tape rewind"
};

/* Error codes for error texts */
#define         DEVIO_ERR_OPEN          0
#define         DEVIO_ERR_DEVICREA      1
#define         DEVIO_ERR_MAXDEVNO      2
#define         DEVIO_ERR_ALRATT        3
#define         DEVIO_ERR_DEVIJCREA     4
#define         DEVIO_ERR_SEWR          5
#define         DEVIO_ERR_SERE          6
#define         DEVIO_ERR_INVPNO        7
#define         DEVIO_ERR_LOCK          8
#define         DEVIO_ERR_USER_REQ      9
#define         DEVIO_ERR_PAGE_0        10
#define         DEVIO_ERR_FILETYPE      11
#define         DEVIO_ERR_FILESTAT      12
#define         DEVIO_ERR_DEVCONTROL    13
#define         DEVIO_ERR_DEVSUPPORT    14
#define         DEVIO_ERR_MALLOC        15
#define         DEVIO_ERR_CLOSE         16
#define         DEVIO_ERR_INVALID_DEVNO 17
#define         DEVIO_MAX_ERR_NR        18

volatile SAPDB_Int neverStop_en54 = 1;

static char const *errtab[ DEVIO_MAX_ERR_NR ]=
{
    "Could not open volume",
    "Could not create volume",
    "No more devices",
    "Device already open",
    "Could not create DEVI process",
    "Error during seek or write",
    "Error during seek or read",
    "Invalid pageno, no io performed",
    "Could not lock volume",
    "I/O error requested by user",
    "I/O error for headerpage",
    "Wrong file type",
    "Error during fstat",
    "Device control failed",
    "Not supported by device",
    "Memory allocation failed",
    "Error during close device",
    "Invalid volume identifier"
};

/* PTS 1105660 */
#define SET_DEVSP_WAITING_FOR(_Task, _Devno) \
    _Task->lWaitForDevno     = _Devno; \
    _Task->DevNameWaitingFor = _Devno >= 0 ? KGS->dev0.dev[_Devno].devspace : NULL;

/* ======================================= MF__ MOD__ (exports) */

extern "C" void vcheckopenvolume(
                    SAPDB_Int4          type_spec_devno,
                    tsp2_dev_type_Param devtype,
                    tsp00_Bool        & ok)
{
    DEV_DESCR *devsp;

    --type_spec_devno;

    devsp = sql58k_get_devsp_descr (devtype, type_spec_devno ) ;
    ok = (devsp != NULL);
    if ( ok )
    {
        int        fd;

#ifdef HAVE_DAFS_SUPPORT
        if ( RTE_FileSystemAccessTypeDAFS == devsp->fileSystemAccessType )
        {
            RTEIO_DAFS_vcheckopenvolume(type_spec_devno, devtype, &ok);
            return;
        }
#endif

        fd = RTE_save_open(devsp->szDevspaceName, DEFAULT_OPEN_MODE_EN54 | O_RDONLY );
        ok = ( fd >= 0 );
        if ( ok )
        {
            RTE_save_close(fd);
        }
    }
}

extern "C" void vdevsizeWithImage (
                    SAPDB_Int4          type_spec_devno,
                    tsp2_dev_type_Param devtype,
                    SAPDB_Int4        & devcapacity,
                    tsp00_TaskId        taskResponsibleForCall,
                    char               *diskImage,
                    tsp00_ErrText     & errtext,
                    tsp00_Bool        & ok)
{
    #undef MF__
    #define MF__ MOD__"vdevsizeWithImage"

    SAPDB_Int4  givenBlockSize = XPARAM(ulDBPageSize) ;
    SAPDB_Int4  tmp;
    DEV_DESCR  *devsp;
    char       *volume;
    SAPDB_Int   isSequential;

    tsp00_Bool  forceReadOnly = ( devtype == sp2dt_log || devtype == sp2dt_mirr_log ) && RTEHSS_IsLogReadOnly();

    --type_spec_devno;

    devsp = sql58k_get_devsp_descr (devtype, type_spec_devno ) ;
    if ( ( ok = (devsp != NULL) ) )
    {
        volume = devsp->szDevspaceName;
        DBG1 (( MF__,"vdevsize: '%s', %ld requested \n",
                          volume , (long) devcapacity ));
    }
    else
    {
        MSGD ((ERR_VDEVSIZE_INVALID_DEVSPACE, devtype, type_spec_devno+1 )) ;
        SAPDB_Char tmpErrText[sizeof(errtext)+1];
        sp77sprintf(&tmpErrText[0], sizeof(tmpErrText), "invalid volume type %d devno %d\n", devtype, type_spec_devno+1 );
        errtext.c2p(&tmpErrText[0]);
        return;
    }

    isSequential = ( devsp->accessMode == RTE_VolumeAccessModeSequential );

    /* PTS CR 1103898 */
    ok = en54_TouchVolumeIfNotExists (volume, &forceReadOnly, errtext) ;
    if ( ok )
      {
        switch ( e541_get_nodinfo ((char *)volume, &tmp) )
        {
        case MOD_ORDIFILE:
            if ( forceReadOnly )
            {
                en54_ValidateReadOnlyVolumeSize( volume,
                                                 givenBlockSize,
                                                &forceReadOnly );
            }

            en54_RegularFileVdevsize ( volume,
                                       givenBlockSize,
                                       &devcapacity,
                                       taskResponsibleForCall,
                                       diskImage,
                                       isSequential,
                                       forceReadOnly,
                                       errtext,
                                       &ok );
            break;
        case MOD_BLCKSPEC:
        case MOD_CHARSPEC:
            en54_RawDiskVdevsize ( volume,
                                   givenBlockSize,
                                   &devcapacity,
                                   taskResponsibleForCall,
                                   diskImage,
                                   isSequential,
                                   forceReadOnly,
                                   errtext,
                                   &ok );
            break;
        case MOD_FIFOSPEC:
            MSGD ((ERR_VDEVSIZE_NO_FIFO, volume ));
            errtext.c2p("No volumes on fifo");
            ok = false ;
            break;
        case MOD_DIR:
            MSGD ((ERR_VDEVSIZE_NO_DIR, volume ));
            errtext.c2p( "No volumes on directory");
            ok = false ;
            break;
        case MOD_UNKNOWN:
        default:
            MSGD ((ERR_VDEVSIZE_ERR_STAT, volume ));
            errtext.c2p("Cannot stat volume");
            ok = false ;
            break;
        }
      }

    if ( ok )
    {
        MSGD ((INFO_VDEVSIZE_SUCCEEDED, volume , (long) devcapacity ));
    }
    else
    {
        tsp00_ErrTextc outputError;
        outputError.p2c(errtext);
        MSGD ((INFO_VDEVSIZE_FAILED, volume , (long) devcapacity, outputError.asCharp() ));
    }
}

/*-----------------------------------
  Function: vdevsize
  Description:
  Arguments: type_spec_devno [in] Type specific logical volume number
             devtype         [in] Type of volume
             devcapacity     [inout] Capacity in pages (not including RTE Page) (updated if 0 given)
             errtext         [out] error if error occured
             ok
 */
extern "C" void vdevsize (
                    SAPDB_Int4          type_spec_devno,
                    tsp2_dev_type_Param devtype,
                    SAPDB_Int4        & devcapacity,
                    tsp00_ErrText     & errtext,
                    tsp00_Bool        & ok)
{
    #undef MF__
    #define MF__ MOD__"vdevsize"
    tsp00_TaskId taskResponsibleForCall;

    vgetpid( taskResponsibleForCall );

    vdevsizeWithImage(type_spec_devno, devtype, devcapacity, taskResponsibleForCall, (char *)0, errtext, ok);
}
/*vdevsize*/

/*
 * ======================================================================
 */

#undef MF__
#define MF__ MOD__"vattach"
/*ARGSUSED*/
extern "C" void vattach (
                        SAPDB_Int4           type_spec_devno,
                        tsp2_dev_type_Param  devtype,
                        SAPDB_Int4         & devno,
                        tsp00_TaskId         taskId,
                        tsp00_ErrText      & errtext,
                        tsp00_Bool         & ok)

{
    struct TASK_TYPE                * tcb;
    ten50_UKT_Control               * this_ukt;
    SAPDB_Int4                        givenBlockSize = XPARAM(ulDBPageSize) ;
    struct DOUBLY_LINKED            * request;
    DEV_DESCR                       * devsp ;
    char                            * volume;

    devsp = sql58k_get_devsp_descr (devtype, (type_spec_devno-1) );
    if ( devsp == NULL )
    {
        MSGD ((IERR_DEV0_VATTACH_INVALID_DEVSPACE, devtype, type_spec_devno )) ;
        ok = false;
        return;
    }

#ifdef HAVE_DAFS_SUPPORT
    if ( RTE_FileSystemAccessTypeDAFS == devsp->fileSystemAccessType )
    {
        RTEIO_DAFS_vattach(type_spec_devno, devtype, &devno, taskId, errtext, &ok);
        return;
    }
#endif

    volume = devsp->szDevspaceName;

    PID_TCB(taskId, tcb);
    this_ukt = tcb->ukt;

    if (KGS->dev_blocksize == 0 )
    {
        KGS->dev_blocksize = givenBlockSize ;
    }
    else
    if ( KGS->dev_blocksize != givenBlockSize )
      {  MSGD ((IERR_DEV0_VATTACH_BLOCKSIZE_ERROR));
         MSGD ((IERR_DEV0_VATTACH_BLOCKSIZE_ERROR_CONT, KGS->dev_blocksize ,
              givenBlockSize ));
         ok = false ;
         vabort ( WRITE_CORE );
      }

    tcb->state = TSK_VATTACH_EO00 ;

    DBG1 (( MF__,"attach '%s' \n", volume ));

    if ( !( ( sp2dt_log == devtype
           || sp2dt_mirr_log == devtype )
           && RTEHSS_IsLogReadOnly() ) )
    {
        if ( !RTE_CSetDatabasePathOwnership( volume ) )
        {
            errtext.c2p( "Cannot set support group for volume" );
            MSGD ((WRN_ATTACH_DEVSPACE, volume, "Cannot set support group for volume" ));
        }
    }

    /*
     * Send an attach-request.
     */

    request                    = GetTaskDLQElement1( tcb );
    request->taskid            = tcb;
    request->req_type          = REQ_VATTACH_EO00 ;
    request->args.vat_parms.volumeName = volume;
    request->args.vat_parms.ndevi      = /* If trace, only 1 devi process ! J.B. 91-11-27 */
        (sp2dt_trace == devtype) ? 1 : KGS->devpsets ;
    request->args.vat_parms.devno = -1;
    request->args.vat_parms.devtype.becomes(devtype);
    request->args.vat_parms.accessMode = devsp->accessMode;
    request->args.vat_parms.ok = false;
    request->args.vat_parms.errcode = 0;

    en54_RequestToDev0 ( request );

    GOTO_DISP (&this_ukt);

    DBG1 (( MF__,"vattach: Return from req 0x%p \n", tcb ));

    if ( this_ukt->curr_quu != request )
    {
        MSGCD ((IERR_DEV0_VATTACH_REQUEST_MISMATCH, volume ));;
        DBG1 (( MF__,"vattach: ABEND curr      0x%p \n", this_ukt->curr_quu ));
        DBG1 (( MF__,"vattach: ABEND request   0x%p \n", request ));
        vabort( WRITE_CORE );
    }

    DBG1 (( MF__,"vattach: Received answer\n" ));

    if ( request->args.vat_parms.ok )
    {
        DBG1 (( MF__,"vattach: returned OK, request = 0x%p \n", request ));
        devno = request->args.vat_parms.devno;
        ok = true ;
        /* determing the physical device ID for this volume (PTS 1109850) */
        if (KGS->XParam->ulFormattingMode == RTE_FORMATTING_MODE_AUTO)
        {
            RTEIO_UpdatePhysicalVolInfoForVolume (devsp);
        }
    }
    else
    {
        /*
         * reply is via ioc queue
         */

        MSGD ((ERR_DEV0_VATTACH_DEV0_ERROR ));
        errtext.c2p( errtab[ request->args.vat_parms.errcode ] );

        /*
         *  also remember to clear the fileno table !!
         */

        KGS->dev0.dev[request->args.vat_parms.devno].devspace[0] = '\0';
        ok = false ;
    }

    if ( ok )
    {
        MSGD (( INFO_VATTACH_SUCCEEDED, volume, devno, taskId ));
    }
    else
    {
        MSGD (( INFO_VATTACH_FAILED, volume, taskId ));
    }
    tcb->state = TSK_RUNNING_EO00 ;
}
/* vattach */

/*
 * ==================================================================
 */

/*ARGSUSED*/
extern "C" void vdetach (
                        SAPDB_Int4        devno,
                        tsp00_TaskId      taskId)
{
#undef MF__
#define MF__ MOD__"vdetach"
  struct TASK_TYPE          * tcb;
  ten50_UKT_Control         * this_ukt;
  struct DOUBLY_LINKED      * request ;
  ten50_UKT_Control         * l_kp ;
  struct dev_info           * pIOCtrl;

    if ( (devno < 0) || (devno > KGS->maxdevspaces) )
    {
        MSGD ((IERR_VDETACH_INVALID_DEVNO, devno ));
        return;
    }

    MSGD (( INFO_VDETACH_CALLED,
            &KGS->dev0.dev[devno].devspace[0] , devno , taskId ));

#ifdef HAVE_DAFS_SUPPORT
    if ( RTE_FileSystemAccessTypeDAFS == KGS->dev0.dev[devno].fileSystemAccessType )
    {
        RTEIO_DAFS_vdetach(devno, taskId);
        return;
    }
#endif

    PID_TCB(taskId, tcb);
    this_ukt = tcb->ukt;

    tcb->state = TSK_VDETACH_EO00 ;
    pIOCtrl = &KGS->dev0.dev[devno];

    if ( 0 == pIOCtrl->devspace[0] )
    {
        MSGD ((IERR_VDETACH_NOT_ATTACHED, devno ));
        tcb->state = TSK_RUNNING_EO00;
        return;
    }

    pIOCtrl->ulDetachPending = 1;

    request                          = GetTaskDLQElement1( tcb );
    request->taskid                  = tcb;
    request->req_type                = REQ_VDETACH_EO00 ;
    request->args.vde_parms.devno    = devno;
    request->args.vde_parms.this_req = request;
    request->args.vde_parms.ndevi    = devno ? KGS->devpsets : 1;
    request->args.vde_parms.rewind   = false;
    request->args.vde_parms.ok       = true;
    request->args.vde_parms.errcode  = (SAPDB_UInt4)UNDEF;
    /* See "vattach" - only 1 devi proc for volume 0 == trace */

    en54_RequestToDev0 (request);

    GOTO_DISP (&this_ukt);

    if ( this_ukt->curr_quu != request )
    {
        MSGD ((IERR_VDETACH_WRONG_EVENT, (long) this_ukt->curr_quu ));
        MSGD ((IERR_VDETACH_WRONG_EVENT_CONT, (long) request ));
        vabort(WRITE_CORE);
    }

    pIOCtrl->ulDetachPending = 0;
    tcb->state = TSK_RUNNING_EO00 ;

    DBGOUT;
}
/* vdetach */

/*
 * ==================================================================
 */

extern "C" SAPDB_Int4 vget_data_io_cnt ()
{
  #undef  MF__
  #define MF__ MOD__"vget_data_io_cnt"

  DBGPAS;

  return ( (SAPDB_Int4 ) KGS->TotalDataIOCnt ) ;
}


/*
 * ==================================================================
 */

extern "C" void vblockio (
  SAPDB_Int4        devno,
  tsp00_PageNo      devpno,
  tsp2_io_op_code_Param op_code,
  tsp00_PageAddr    bufadr,
  SAPDB_Int2        buf_count,
  tsp00_TaskId      taskId,
  tsp00_ErrText   & errtext,
  tsp00_Bool      & ok)
{
#undef MF__
#define MF__ MOD__"vblockio"
  struct TASK_TYPE                * tcb;
  ten50_UKT_Control               * this_ukt;
  REGISTER  struct DOUBLY_LINKED  * request;
  struct DEV_PCB                  * dev_pcb;
  tsp00_Bool                        is_reading = op_code == sp2ioc_read ;
  tsp00_Bool                        io_finished = false ;
  SAPDB_Int                         rc ;
  teo52_TaskDevIOStatistic        * pIOStat ;
  struct dev_info                 * pIOCtrl;

    if ( buf_count <= 0 )
    {
        char buffer[EN54_SMALL_MESSAGE_BUFFER_SIZE];
        sp77sprintf(&buffer[0], sizeof(buffer), "vblockio without pages (%d)", buf_count );
        MSGD(( WRN_TEST_WILDCARD, &buffer[0] ));
        return;
    }

/*----------------- check devno ----------------------- */
    if ( (devno < 0) || (devno >= KGS->maxdevspaces) )
    {
        MSGD ((IERR_VBLOCKIO_INVALID_DEVNO ));
        ok = false ;
        return ;
    }

#ifdef HAVE_DAFS_SUPPORT
    if ( RTE_FileSystemAccessTypeDAFS == KGS->dev0.dev[devno].fileSystemAccessType )
    {
        RTEIO_DAFS_vblockio(devno,
                            op_code,
                            bufadr,
                            buf_count,
                            taskId,
                            errtext,
                            &ok);
        return;
    }
#endif

    PID_TCB(taskId, tcb);
    this_ukt = tcb->ukt;

    DBG1 (( MF__,"T%d devno %d pno %d op %d\n", taskId, devno , devpno , op_code ));

    pIOCtrl = &KGS->dev0.dev[devno];

    if ( pIOCtrl->ulDetachPending )
    {
        MSGD ((IERR_VBLOCKIO_NOT_ATTACHED ));
        errtext.c2p( "Not attached" );
        ok = false;
        return;
    }

/*----------------- IO statistics --------------------- */

    if ( tcb->type != TT_DW_EO00  &&         /* PTS 1001193 */
        ( pIOCtrl->devtype == sp2dt_data ) )
      if ( ++(KGS->TotalDataIOCnt) >= 0x7fffffff )
        KGS->TotalDataIOCnt = 0 ;

    SET_DEVSP_WAITING_FOR(tcb, devno) ;

    tcb->state = is_reading ? TSK_IO_READ_EO00 : TSK_IO_WRITE_EO00;

/*------------------- try single_io ------------------- */

    if ( this_ukt->allow_single_io &&  this_ukt->activeTasks == 1 )
    {
        SAPDB_UInt8 startTime = 0;
        struct UKP_IO_STRUCT *pSingleIOCtrl = &this_ukt->pSingleIO[devno];
        teo52_IOStatistic *pUKT_SelfIOStat;
        teo52_IOStatistic *pTask_SelfIOStat;
        if ( is_reading )
        {
           pUKT_SelfIOStat   = &(pSingleIOCtrl->ReadSelfIo);
           pTask_SelfIOStat = &(tcb->TaskStateStat.SelfIOReadStat);
        }
        else
        {
            pUKT_SelfIOStat   = &(pSingleIOCtrl->WriteSelfIo);
            pTask_SelfIOStat = &(tcb->TaskStateStat.SelfIOWriteStat);
        }

        eo52UpdateCountInIOStatisticRec(*pUKT_SelfIOStat, buf_count);
        eo52UpdateCountInIOStatisticRec(*pTask_SelfIOStat, buf_count);

        if ( XPARAM(fTimeMeasure) )
        {
            startTime = RTESys_MicroSecTimer();
#ifdef WANT_DATAWRITE_TIME_INFO
            if ( tcb->type == TT_DW_EO00 )
            {
                MSGD(( INFO_TIME_START, tcb->index, "vblockio(singleio)", startTime ));
            }
#endif
            rc = en54_SingleBlockIO ( this_ukt, devno, devpno, op_code, bufadr,
                                                   buf_count, errtext, &ok );

            SAPDB_UInt8 deltaTime = RTESys_MicroSecTimer() - startTime;

            eo52UpdateTimeInStatisticRec(pUKT_SelfIOStat->IOStat, deltaTime);
            eo52UpdateTimeInStatisticRec(pTask_SelfIOStat->IOStat, deltaTime);
#ifdef WANT_DATAWRITE_TIME_INFO
            if ( tcb->type == TT_DW_EO00 )
            {
                MSGD(( INFO_TIME_STOP, tcb->index, "vblockio(singleio)", startTime, startTime + deltaTime, deltaTime ));
            }
#endif
        }
        else
        {
            rc = en54_SingleBlockIO ( this_ukt, devno, devpno, op_code, bufadr,
                                                   buf_count, errtext, &ok );
        }

        io_finished = (rc != VB_NO_MORE_FILES);
#if DUMP
        if ( io_finished )
        {
            en54_Dump ( 'S', devno, devpno, op_code, bufadr, buf_count, taskId );
        }
#endif
    }

/*----------------- try system supplied asyn I/O ------ */

  if ( !io_finished )
    {
    pIOStat = is_reading ? &(tcb->TaskStateStat.DevIOReadStat)
                        : &(tcb->TaskStateStat.DevIOWriteStat) ;

    eo52UpdateCountInIOStatisticRec(*pIOStat, buf_count);
    
    request                           = GetTaskDLQElement1( tcb );
    request->taskid                   = tcb;
    request->req_type                 = REQ_VBLOCKIO_EO00 ;
    request->args.vbl_parms.devno     = devno;
    request->args.vbl_parms.devpno    = devpno;
    request->args.vbl_parms.op_code   = SAPDB_Int4(op_code);
    request->args.vbl_parms.p         = bufadr;
    request->args.vbl_parms.buf_count = buf_count;
    request->args.vbl_parms.ok        = false;

    /* 15.4.2002 jrg: currently completely disabled */
#ifdef HAS_ASYNC_IO_SUPPORT
    if ( XPARAM(fUseAsyncIO)  && !IS_TRACE_DEV(devno) )
      {
/********************************************************************/
        rc = e54_asyn_vblockio ( request, errtext );
        if ( tcb->TimeCollectionEnabled )
        {
            eo52UpdateTimeInStatisticRec( pIOStat->IOStat, tcb->TaskStateStat );
        }
/********************************************************************/

        DBG1 (( MF__,"T%d single_block_io rc %d, ok %d\n",
                       tcb->index, rc, ok )) ;
        if ( (io_finished = rc == VB_OK) )  /* if asynio failed, try via DEVi */
        {
          ok = true ;
#if DUMP
          en54_Dump ( 'A' , devno , devpno , op_code , bufadr , buf_count , taskId );
#endif
        }
      }
#endif

    }

/*----------------- try asyn I/O via DEVi ------------- */

  if ( !io_finished )
    {
    if (( KGS->devpsets == 1 ) || IS_TRACE_DEV(devno) )
                                            /* 1 devi proc for trace */
        dev_pcb = KGS->first_dev + devno;
    else
        dev_pcb = en54_FindDevpcb ( this_ukt
                                , devno
                                , (tsp00_Bool) tcb->prio_flag
                                , is_reading != 0 );

    if ( dev_pcb->state != KT_INITIALIZED )
      {
#ifdef WANT_DATAWRITE_TIME_INFO
        SAPDB_UInt8 checkStartTime = 0;
        SAPDB_UInt8 checkEndTime = 0;
#endif
        DBG1 (( MF__,"send req 0x%x to devi\n", request ));

        this_ukt->wait_for_ioproc_io++;
#ifdef WANT_DATAWRITE_TIME_INFO
        if ( tcb->type == TT_DW_EO00 )
        {
            checkStartTime = RTESys_MicroSecTimer();
            MSGD(( INFO_TIME_START, tcb->index, "vblockio(devio)", checkStartTime ));
        }
#endif
        en54_RequestToDevi ( dev_pcb , request );
/********************************************************************/
        GOTO_DISP (&this_ukt);
        if ( tcb->TimeCollectionEnabled )
        {
            eo52UpdateTimeInStatisticRec( pIOStat->IOStat, tcb->TaskStateStat );
        }
/********************************************************************/
#ifdef WANT_DATAWRITE_TIME_INFO
        if ( tcb->type == TT_DW_EO00 )
        {
           checkEndTime = RTESys_MicroSecTimer();
           MSGD(( INFO_TIME_STOP, tcb->index, "vblockio(devio)", checkStartTime, checkEndTime, checkEndTime - checkStartTime ));
        }
#endif

        this_ukt->wait_for_ioproc_io--;

        if ( this_ukt->curr_quu != request )
        {
            MSGD ((IERR_VBLOCKIO_WRONG_EVENT, (long) this_ukt->curr_quu ));
            MSGD ((IERR_VBLOCKIO_WRONG_EVENT_CONT, (long) request ));
            vabort(WRITE_CORE);
        }

        DBG1 (( MF__,"returned from en54_DeviVblockio, devpno = %d \n",
            devpno));

        if ( ! (ok = request->args.vbl_parms.ok) )
        {
            errtext.c2p( errtab[request->args.vbl_parms.errcode] );
            if ( ! KGS->dumpflag )
            {
                MSGD ((ERR_VBLOCKIO_SYSTEMERR, devno));
            }
        }
        else
        {
            DBG1 (( MF__,"ok for T%d\n", tcb->index ));
        }
        /*
         * increase io counter
         */
        tcb->dirio++;
      }
    else
      {
        MSGD ((IERR_VBLOCKIO_NOT_ATTACHED ));
        errtext.c2p( "Not attached" );
        ok = false;
      }
    }
#if DUMP
    en54_Dump ( 'D' , devno , devpno , op_code , bufadr , buf_count , taskId );
#endif
    DBG1 (( MF__,"returning, T%d \n", tcb->index ));
    SET_DEVSP_WAITING_FOR(tcb, undef_ceo00) ;
    tcb->state = TSK_RUNNING_EO00;
    return;
}/* vblockio */

/*==========================================================================*/

extern "C" void vasynopen (
  tsp00_VFilename & hostfile,
#ifdef HAS_FSTYPE_IN_VASYNOPEN
  RTE_FileSystemAccessType fsType, /* ---> interface change needed for 'fsType'... */
#endif
  tsp00_Bool       is_devspace,
  tsp00_Bool       for_writing,
  tsp00_VfType_Param filetype,
  SAPDB_Int4       givenBlockSize,
  tsp00_BoolAddr   cancel_pointer,
  SAPDB_Int4     & max_block_cnt,
  SAPDB_Int4     & hostfileno,
  tsp00_VaReturn & returncode,
  tsp00_ErrText  & errtext )
{
    #undef MF__
    #define MF__ MOD__"vasynopen"
    ten50_UKT_Control           *this_ukt;
    tsp00_VFilenamec             hostfileC;
    struct DOUBLY_LINKED        *request ;
    tsp_vf_label                 f_label ;

#ifndef HAS_FSTYPE_IN_VASYNOPEN
    RTE_FileSystemAccessType     fsType; /* ---> interface change needed for 'fsType'... */

    if ( !is_devspace
      || !RTEConf_GetVolumeType(hostfile, fsType) )
    {
        fsType = RTE_FileSystemAccessTypeNormal;
    }
#endif

#ifdef HAVE_DAFS_SUPPORT
    if ( RTE_FileSystemAccessTypeDAFS == fsType )
    {
        RTEIO_DAFS_vasynopen( hostfile,
                              is_devspace,
                              for_writing,
                              filetype,
                              givenBlockSize,
                              cancel_pointer,
                              &max_block_cnt,
                              &hostfileno,
                              &returncode,
                              errtext );
    }
#endif

    SAPDB_memset( f_label, '_', sizeof ( tsp_vf_label ) );

    this_ukt = THIS_UKT_CTRL;
    this_ukt->curr_task->state = TSK_ASYNOPEN_EO00 ;

    hostfileC.p2c( hostfile );

    /*
     * Send an attach-request.
     */
    request                               = GetTaskDLQElement1( this_ukt->curr_task );
    request->taskid                       = this_ukt->curr_task ;
    request->req_type                     = REQ_ASYNOPEN_EO00 ;
    request->args.vas_parms.ndevi         = 1 ;
    request->args.vas_parms.is_devspace   = is_devspace ;
    request->args.vas_parms.for_writing   = for_writing ;
    request->args.vas_parms.block_size    = givenBlockSize ;
    request->args.vas_parms.cancelPointer = ( cancel_pointer ? cancel_pointer : &this_ukt->curr_task->rte_comm.to_cancel );
    request->args.vas_parms.max_block_cnt = max_block_cnt ;
    request->args.vas_parms.fileType      = filetype ;
    request->args.vas_parms.deviceName    = (char *)hostfileC;
    request->args.vas_parms.ok = false ;

    en54_RequestToDev0 ( request );

    GOTO_DISP (&this_ukt);
    DBG1 (( MF__,"asynopen: return from dispatcher \n" ));

    if ( this_ukt->curr_quu != request )
    {
        MSGD ((IERR_ASYNOPEN_WRONG_EVENT, this_ukt->curr_quu ));
        MSGD ((IERR_ASYNOPEN_WRONG_EVENT_CONT, request ));
        MSGD ((IERR_ASYNOPEN_WRONG_EVENT_CONT2, this_ukt->curr_quu->req_type ));
        MSGD ((IERR_ASYNOPEN_WRONG_EVENT_CONT3, request->req_type ));
        vabort(WRITE_CORE);
        /*NOTREACHED*/
    }

    if ( request->args.vas_parms.ok )
    {
        DBG1 (( MF__,"asynopen: succeeded \n" ));
        hostfileno = request->args.vas_parms.devno;
        KGS->dev0.dev[hostfileno].IsDevspace    = is_devspace ; /* PTS 1106176 */
        KGS->dev0.dev[hostfileno].asyn_write    = for_writing ;
        KGS->dev0.dev[hostfileno].block_size    = givenBlockSize ;
        KGS->dev0.dev[hostfileno].max_block_cnt = max_block_cnt ;
        KGS->dev0.dev[hostfileno].fileSystemAccessType = RTE_FileSystemAccessTypeNormal;
        errtext.SetBlank();
        returncode.becomes(va_ok);
    }
    else
    {
        max_block_cnt = request->args.vas_parms.max_block_cnt ;
        /* Clear the file table */
        KGS->dev0.dev[request->args.vas_parms.devno].devspace[0] = '\0';
        errtext.c2p( errtab[request->args.vas_parms.errcode] );
        returncode.becomes(va_notok);
    }

    if ( returncode == va_ok )
    {
        MSGD (( INFO_VASYNOPEN_SUCCEEDED,
                  hostfileC.asCharp() , hostfileno, this_ukt->curr_task->index ));
    }
    else
    {
        MSGD (( INFO_VASYNOPEN_FAILED,
                    hostfileC.asCharp() , this_ukt->curr_task->index ));
    }
    this_ukt->curr_task->state = TSK_RUNNING_EO00 ;
}
/* vasynopen */

/*==========================================================================*/

/*ARGSUSED*/
extern "C" void vasynclose (
  SAPDB_Int4       hostfileno ,
  tsp00_Bool       rewind,
  tsp00_Bool       nxt_in_autoloader,
  tsp00_VaReturn & nxt_tape_available,
  tsp00_VaReturn & returncode ,
  tsp00_ErrText  & errtext )
{
#undef MF__
#define MF__ MOD__"vasynclose"
  ten50_UKT_Control               * this_ukt = THIS_UKT_CTRL;
  struct DOUBLY_LINKED            * request;

    if ( (hostfileno < KGS->maxdevspaces) ||
         (hostfileno >= KGS->maxdevspaces * 2 + KGS->maxtapes) )
    {
        MSGD ((IERR_ASYNCLOSE_INVALID_FILENO, (long) hostfileno ));
        errtext.c2p( "illegal hostfileno" );
        returncode.becomes(va_notok);
        return;
    }

    MSGD (( INFO_VASYNCLOSE_CALLED,
          &KGS->dev0.dev[hostfileno].devspace[0], hostfileno ,
          this_ukt->curr_task->index ));

#ifdef HAVE_DAFS_SUPPORT
    if ( RTE_FileSystemAccessTypeDAFS == KGS->dev0.dev[hostfileno].fileSystemAccessType )
    {
        RTEIO_DAFS_vasynclose ( hostfileno,
                                rewind,
                                nxt_in_autoloader,
                               &nxt_tape_available,
                               &returncode ,
                                errtext );
        return;
    }
#endif

    if ( ! KGS->dev0.dev[hostfileno].devspace[0] )
    {
        MSGD ((IERR_ASYNCLOSE_NOT_OPEN, (long) hostfileno ));
        errtext.c2p( "device not open" );
        returncode.becomes(va_notok);
        return;
    }
    DBG1 (( MF__,"asynclose %d '%s' \n",
                hostfileno , KGS->dev0.dev[hostfileno].devspace ));

    this_ukt->curr_task->state = TSK_ASYNCLOSE_EO00 ;

    request                          = GetTaskDLQElement1( this_ukt->curr_task );
    request->taskid                  = this_ukt->curr_task ;
    request->req_type                = REQ_ASYNCLOSE_EO00 ;
    request->args.vde_parms.devno    = hostfileno ;
    request->args.vde_parms.this_req = request ;
    request->args.vde_parms.ndevi    = 1 ;
    request->args.vde_parms.rewind   = rewind;
    request->args.vde_parms.ok       = true;
    request->args.vde_parms.errcode  = (SAPDB_UInt4)UNDEF;

    en54_RequestToDev0 ( request );

    GOTO_DISP (&this_ukt);

    if ( this_ukt->curr_quu != request )
    {
        MSGD ((IERR_ASYNCLOSE_WRON_EVENT, this_ukt->curr_quu ));
        MSGD ((IERR_ASYNCLOSE_WRON_EVENT_CONT, request ));
        MSGD ((IERR_ASYNCLOSE_WRON_EVENT_CONT2, this_ukt->curr_quu->req_type ));
        MSGD ((IERR_ASYNCLOSE_WRON_EVENT_CONT3, request->req_type ));
        vabort(WRITE_CORE);
    }
    if (request->args.vde_parms.ok) {
		errtext.SetBlank();
        returncode.becomes(va_ok);
    }
    else {
        errtext.c2p( errtab[ request->args.vde_parms.errcode ] );
        returncode.becomes(va_notok);
    }

    nxt_tape_available.becomes(va_notok);
    this_ukt->curr_task->state = TSK_RUNNING_EO00 ;
}
/* vasynclose */

/*==========================================================================*/
extern "C" void vasynclose_old (
  SAPDB_Int4       hostfileno,
  tsp00_Bool       nxt_in_autoloader,    /* dummy, for NT only */
  tsp00_VaReturn & nxt_tape_available,   /* dummy, for NT only */
  tsp00_VaReturn & returncode,
  tsp00_ErrText  & errtext)
{
    #undef MF__
    #define MF__ MOD__"vasynclose_old"
    vasynclose (hostfileno, false, nxt_in_autoloader, nxt_tape_available,
        returncode, errtext);
}
/* vasynclose_old */

/*==========================================================================*/

extern "C" void vasynio (
  SAPDB_Int4          hostfileno,
  SAPDB_Int4          io_id,
  SAPDB_Int4          file_pos,
  SAPDB_Int4          io_block_cnt,
  tsp00_PageAddr      io_block_ptr,
  tsp00_VaReturn    & returncode,
  tsp00_ErrText     & errtext)

{
#undef MF__
#define MF__ MOD__"vasynio"
    ten50_UKT_Control           *this_ukt;
    SAPDB_Int                    idx ;
    struct DOUBLY_LINKED        *request ;
    struct REQ_PARMS_ASYNIO     *parms ;
    struct DEV_PCB              *dev_pcb ;
    teo52_TaskDevIOStatistic    *pIOStat ;
    teo52_IOStatistic           *pDevStat ;

    if ( (hostfileno < KGS->maxdevspaces) ||
         (hostfileno >= KGS->maxdevspaces * 2 + KGS->maxtapes) )
    {
      MSGD ((IERR_ASYNIO_INVALID_FILENO, (long) hostfileno ));
      errtext.c2p( "illegal hostfileno" );
      returncode.becomes(va_notok);
    }

#ifdef HAVE_DAFS_SUPPORT
    if ( RTE_FileSystemAccessTypeDAFS == KGS->dev0.dev[hostfileno].fileSystemAccessType )
    {
        RTEIO_DAFS_vasynio ( hostfileno,
                             io_id,
                             file_pos,
                             io_block_cnt,
                             io_block_ptr,
                            &returncode,
                             errtext );
        return;
    }
#endif

    this_ukt = THIS_UKT_CTRL;

    DBG1 (( MF__,"asynio: T%d dev %ld pos %ld len %ld id %ld \n",
            this_ukt->curr_task->index , (long) hostfileno , (long) file_pos ,
            (long) io_block_cnt , (long) io_id ));


    returncode.becomes(va_ok);
    dev_pcb = en54_FindAsyncDevpcb ( hostfileno ) ;
    if ( dev_pcb->state == KT_INITIALIZED )
    {
        MSGD ((IERR_ASYNIO_NOT_OPEN, (long) hostfileno ));
        errtext.c2p( "device not open" );
        returncode.becomes(va_notok);
    }

    if ( returncode == va_ok )
    {
        struct RTETask_CommonQueueElement *  pQueueElement = this_ukt->curr_task->asyn_requests;

        for ( idx = 0 ; idx < KGS->max_asyn_request ;
              idx ++ )
          {
            request = static_cast<DLQ_ELEMENT*>(*pQueueElement);

            if ( ! request->args.vai_parms.initiator )
              {
                KGS->dev0.dev[hostfileno].asyn_io_count ++ ;
                parms = &request->args.vai_parms ;
                parms->initiator = this_ukt->curr_task ;
                parms->io_count   = KGS->dev0.dev[hostfileno].asyn_io_count ;
                break ;
              }
            pQueueElement++;
          }

        if ( idx >= KGS->max_asyn_request )
          {
            errtext.c2p( "wait first" );
            returncode.becomes(va_wait_first);
          }
      }

    if ( returncode == va_ok )
      { this_ukt->curr_task->state = TSK_ASYNIO_EO00 ;

        request->taskid   = this_ukt->curr_task ;
        request->req_type = REQ_ASYNIO_EO00 ;
        parms->io_id      = io_id ;
        parms->devno      = hostfileno ;
        parms->devpno     = file_pos ;
        parms->block_siz  = KGS->dev0.dev[hostfileno].block_size ;
        parms->block_cnt  = io_block_cnt ;
        parms->block_ptr  = reinterpret_cast<tsp00_Maxiobufaddr>(io_block_ptr);
        parms->finished   = false ;
        parms->ok         = false ;
        DBG1 (( MF__,"asynio: T%d req 0x%lx \n",
               this_ukt->curr_task->index , (long) request ));
        DBG1 (( MF__,"asynio: id %ld dev %ld cnt %ld \n",
                (long) parms->io_id , (long) parms->devno ,
                (long) parms->io_count ));
        DBG1 (( MF__,"asynio: addr 0x%lx len %ld pos %ld \n",
                (long) parms->block_ptr , (long) parms->block_cnt ,
                (long) parms->devpno ));

        if ( KGS->dev0.dev[hostfileno].asyn_write )
          {
            pDevStat = & (dev_pcb->DevIOStat.DevWrite) ;
            pIOStat =  & (this_ukt->curr_task->TaskStateStat.AsynIOWriteStat);
          }
        else
          {
            pDevStat = & (dev_pcb->DevIOStat.DevRead) ;
            pIOStat =  & (this_ukt->curr_task->TaskStateStat.AsynIOReadStat);
          }

        eo52UpdateCountInIOStatisticRec(*pIOStat, io_block_cnt);
        eo52UpdateCountInIOStatisticRec(*pDevStat, io_block_cnt);

#ifdef HAS_ASYNC_IO_SUPPORT
        if ( XPARAM(fUseAsyncIO) && ! IS_TAPE(hostfileno) )
          {
            if ( e54_asyn_vasynio  ( request, errtext) )
              { returncode.becomes(va_ok);
                parms->ok = true ;
              }
           else
              { returncode.becomes(va_notok);
                parms->ok = false ;
              }
          }
        else
#endif
          en54_RequestToDevi ( dev_pcb , request );

        errtext.SetBlank();
      }

    this_ukt->curr_task->state     = TSK_RUNNING_EO00 ;
    DBG1 (( MF__,"asynio: returning, T%d \n", this_ukt->curr_task->index ));
}
/* vasynio */

/*==========================================================================*/

extern "C" void vasynwait (
  SAPDB_Int4        hostfileno,
  SAPDB_Int4      & io_id,
  SAPDB_Int4      & io_block_cnt,
  tsp00_VaReturn  & returncode,
  tsp00_ErrText   & errtext)

{
    #undef MF__
    #define MF__ MOD__"vasynwait"
    ten50_UKT_Control           *this_ukt;
    SAPDB_Int                         idx ;
    SAPDB_Int                         active_cnt ;
    struct DOUBLY_LINKED        *request ;
    struct REQ_PARMS_ASYNIO     *parms ;

    io_id        = -1 ;
    io_block_cnt = -1 ;

    if ( (hostfileno < KGS->maxdevspaces) ||
         (hostfileno >= KGS->maxdevspaces * 2 + KGS->maxtapes) )
    {
        MSGD ((IERR_ASYNWAIT_INVALID_FILENO, (long) hostfileno ));
        errtext.c2p( "illegal hostfileno" );
        returncode.becomes(va_notok);
        return;
    }

#ifdef HAVE_DAFS_SUPPORT
    if ( RTE_FileSystemAccessTypeDAFS == KGS->dev0.dev[hostfileno].fileSystemAccessType )
    {
        RTEIO_DAFS_vasynwait ( hostfileno,
                              &io_id,
                              &io_block_cnt,
                              &returncode,
                               errtext );
        return;
    }
#endif

    this_ukt = THIS_UKT_CTRL;

     DBG1 (( MF__,"asynwait: T%d devno %ld \n",
            this_ukt->curr_task->index , (long) hostfileno ));

    /*
     *  Search for a 'finished' I/O.
     *  After the I/O completion event is received by the dispatcher,
     *  the dispatcher declares the I/O as finished.
     */
    SET_DEVSP_WAITING_FOR(this_ukt->curr_task, hostfileno) ;
    active_cnt = 0 ;
    struct RTETask_CommonQueueElement *  pQueueElement = this_ukt->curr_task->asyn_requests;

    for ( idx = 0 ; idx < KGS->max_asyn_request ; idx ++ )
    {
        request = static_cast<DLQ_ELEMENT*>(*pQueueElement);
        parms   = &request->args.vai_parms ;

        if ( parms->initiator )
            active_cnt ++ ;
        if ( parms->finished && parms->initiator == this_ukt->curr_task )
            break ;
        pQueueElement++;
    }

    if ( idx >= KGS->max_asyn_request )
    {
        /*
         *  I/O completion event not yet received.
         */
        if ( active_cnt == 0 )
        {
            MSGD ((IERR_ASYNWAIT_NO_REQUEST_ACTIVE ));
            errtext.c2p( "no I/O requests active" );
            io_block_cnt = 0 ;
            returncode.becomes(va_notok);
            return;
        }

        do
        {
            DBG1 (( MF__,"asynwait: goto_disp T%d \n", this_ukt->curr_task->index ));

            if ( KGS->dev0.dev[hostfileno].asyn_write )
            {
                this_ukt->curr_task->state = TSK_ASYNWAIT_WRITE_EO00 ;
/********************************************************************/
                GOTO_DISP (&this_ukt);
                if ( this_ukt->curr_task->TimeCollectionEnabled )
                {
                    eo52UpdateTimeInStatisticRec( this_ukt->curr_task->TaskStateStat.AsynIOWriteStat.IOStat, this_ukt->curr_task->TaskStateStat );
                }
/********************************************************************/
            }
            else
            {
                this_ukt->curr_task->state = TSK_ASYNWAIT_READ_EO00 ;
/********************************************************************/
                GOTO_DISP (&this_ukt);
                if ( this_ukt->curr_task->TimeCollectionEnabled )
                {
                    eo52UpdateTimeInStatisticRec( this_ukt->curr_task->TaskStateStat.AsynIOReadStat.IOStat, this_ukt->curr_task->TaskStateStat );
                }
/********************************************************************/
            }

            request = this_ukt->curr_quu ;
            if ( request->req_type != REQ_ASYNIO_EO00 )
            {
                MSGD ((IERR_ASYNWAIT_WRONG_REQUEST, request->req_type ));
                MSGD ((IERR_ASYNWAIT_WRONG_REQUEST_CONT, REQ_ASYNIO_EO00 ));
                vabort(WRITE_CORE);
            }

            parms = &request->args.vai_parms ;
            DBG1 (( MF__,"asynwait: T%d req 0x%lx for T%d \n",
                    this_ukt->curr_task->index , (long) request ,
                    request->taskid->index ));
            DBG1 (( MF__,"asynwait: id %ld dev %ld cnt %ld ok %d\n",
                    (long) parms->io_id , (long) parms->devno ,
                    (long) parms->io_count , parms->ok ));
            DBG1 (( MF__,"asynwait: addr 0x%lx len %ld pos %ld \n",
                    (long) parms->block_ptr , (long) parms->block_cnt ,
                    (long) parms->devpno ));
        }
        while ( parms->devno != hostfileno && parms->initiator != this_ukt->curr_task);
        /*
         *  MUST NOT handle I/O completion from other device.
         */
    }

#ifdef HAS_ASYNC_IO_SUPPORT
    if ( XPARAM(fUseAsyncIO) && ! IS_TAPE(hostfileno) )
       e54_asyn_vasynio_result ( request, errtext ) ;
#endif

    io_id        = parms->io_id ;
    io_block_cnt = parms->block_cnt ;
    if ( parms->ok )
    {
        errtext.SetBlank();
        returncode.becomes(va_ok);
    }
    else
    {
        if ( parms->block_cnt < 0 )
        {
            errtext.c2p( "I/O Error" );
            returncode.becomes(va_notok);
        }
        else
        {
            errtext.c2p( "end of file" );
            returncode.becomes(va_eof);
        }
    }
    parms->finished = false ;
    parms->initiator = NULL ;
    SET_DEVSP_WAITING_FOR(this_ukt->curr_task, undef_ceo00) ;
    this_ukt->curr_task->state = TSK_RUNNING_EO00 ;
    DBG1 (( MF__,"asynwait: returning, T%d \n", this_ukt->curr_task->index ));
}
/* vasynwait */


extern "C" void vasyninit_tape (
        SAPDB_Int4        hostfileno,
        tsp00_Bool        destructive,
        tsp00_VfType    & filetype,
        tsp00_VaReturn  & returncode,
        tsp00_ErrText   & errtext)

{
    #undef MF__
    #define MF__ MOD__"vasyninit_tape"
ten50_UKT_Control               * this_ukt = THIS_UKT_CTRL;
    struct DOUBLY_LINKED        *request ;
    struct DEV_PCB              *dev_pcb ;

    DBG1 (( MF__,"asyninit_tape: T%d devno %ld \n",
                this_ukt->curr_task->index , (long) hostfileno ));

    if (!en54_ValidHostfile (hostfileno, "vasyninit_tape", &returncode, errtext) )
        return;
    DBG1 (( MF__,"asyninit_tape %d '%s' %s\n",
                hostfileno , KGS->dev0.dev[hostfileno].devspace,
                destructive ? "destructive" : "nondestructive" ));
    DBG1 (( MF__,"asyninit_tape: page0 size %d bytes\n",
        KGS->dev0.dev[hostfileno].block_size
        * KGS->dev0.dev[hostfileno].max_block_cnt ));
    /*
     * Send an device control request (subkind filetype)
     */
    this_ukt->curr_task->state = TSK_ASYNCNTL_EO00 ;
    request                               = GetTaskDLQElement1( this_ukt->curr_task );
    request->taskid                       = this_ukt->curr_task ;
    request->req_type                     = REQ_ASYNCNTL_EO00 ;
    request->args.vac_parms.cntlRequest   = ASYNCNTL_FILETYPE ;
    request->args.vac_parms.destructive   = destructive;
    request->args.vac_parms.hostfileno    = hostfileno ;

    dev_pcb = en54_FindAsyncDevpcb ( hostfileno );
    if ( dev_pcb->state == KT_INITIALIZED )
    {
        MSGD ((IERR_ASYNINIT_NOT_OPEN, (long) hostfileno ));
        errtext.c2p( "device not open" );
        returncode.becomes(va_notok);
        this_ukt->curr_task->state = TSK_RUNNING_EO00 ;
        return;
    }

    en54_RequestToDevi ( dev_pcb, request );

    GOTO_DISP (&this_ukt);

    if ( this_ukt->curr_quu != request )
    {
        MSGD ((IERR_ASYNINIT_WRONG_EVENT, this_ukt->curr_quu ));
        MSGD ((IERR_ASYNINIT_WRONG_EVENT_CONT, request ));
        MSGD ((IERR_ASYNINIT_WRONG_EVENT_CONT2, this_ukt->curr_quu->req_type ));
        MSGD ((IERR_ASYNINIT_WRONG_EVENT_CONT3, request->req_type ));
        vabort(WRITE_CORE);
    }

    this_ukt->curr_task->state = TSK_RUNNING_EO00 ;
    if (request->args.vac_parms.ok) {
        filetype = request->args.vac_parms.fileType;
        returncode.becomes(va_ok);
    }
    else {
        MSGD ((IERR_ASYNINIT_FAILED, (long) returncode ));
        errtext.c2p( errtab[request->args.vac_parms.errcode] );
        returncode.becomes(va_notok);
    }
}
/* vasyninit_tape */

/* ======================================= MF__ MOD__ (devprocesses) */

/*
 * Main program for the dev0 process.
 */
    #undef MF__
    #define MF__ MOD__"en54_Mdevio"
/*ARGSUSED*/
extern "C" void *en54_Mdevio (void *arg)
{
    teo07_ThreadErr err;
    SAPDB_Int rc;

    KGS->dev0.start_time = KGS->current_time;
    KGS->dev0.state = KT_RUNNING;

    MSGCD (( INFO_PROCESS_STARTED, "DEV0" ));

    DBG1 (( MF__,"call dev_main \n" ));

    en54_Dev0Main ();

    MSGD (( INFO_PROCESS_STOPPED, "DEV0" ));

    while(neverStop_en54)
    {
        sqlyieldthread();
    }
    /*NOTREACHED*/
    return NULL;
}
/* e54_Mdevio */

/*
 * ==================================================================
 */

static  tsp00_Uint4    en54_find_device_no (
  struct DOUBLY_LINKED  * request,
  char                  * devname,
  tsp00_Uint4           * pErrCode)
{
#undef MF__
#define MF__ MOD__"en54_find_device_no"
  SAPDB_Int                         devpsets;
  SAPDB_Int                         idx ;
  SAPDB_Int                         edx ;
  SAPDB_Int                         first_free ;
  SAPDB_Int                         found ;
  char const                  *msg ;
  SAPDB_Int4                   devno ;
  SAPDB_UInt4                  errcode ;
  tsp00_Uint4                  ok ;
  struct DEV_PCB              *devp ;

    /*
     *  Vattach may occupy the first 'KGS->maxdevspaces' entries
     *  of the 'KGS->dev0.dev' array.
     *  Vasynopen for volumes may occupy the same amount
     *  of entries behind them.
     *  Vasynopen for tape devices may occupy the last 'KGS->maxtapes'
     *  entries.
     */
    if ( request->req_type == REQ_VATTACH_EO00 )
    {
        devpsets = KGS->devpsets ;
        devp     = KGS->first_dev ;
        idx      = 0 ;
        edx      = KGS->maxdevspaces ;
        msg      = "VATTACH";
    }
    else
    {
        devpsets = 1;
        if ( request->args.vas_parms.is_devspace )
        {   devp = KGS->first_asdev ;
            idx = KGS->maxdevspaces ;
            edx = KGS->maxdevspaces * 2 ;
            msg = "ASYNOPEN (disk)";
        }
        else
        {   devp = KGS->first_asdev + KGS->maxdevspaces ;
            idx  = KGS->maxdevspaces * 2 ;
            edx  = KGS->maxdevspaces * 2 + KGS->maxtapes ;
            msg  = "ASYNOPEN (tape)";
        }
    }
    DBG1 (( MF__,"%s search from %d to %d \n", msg , idx , edx-1 ));

    devno      = -1 ;
    ok         = true ;
    errcode    = 0 ;
    found      = false ;
    first_free = -1 ;

    while ( (idx < edx)  && ! found )
    {
        found = (devp->state != KT_INITIALIZED) &&
                ! strcmp((const char *)KGS->dev0.dev[idx].devspace,
                         (const char *)devname );
        if ( devp->state == KT_INITIALIZED && first_free == -1 )
            first_free = idx ;
        DBG1 (( MF__,"en54_find_device_no: dev[%d].state %d \n", idx, devp->state )) ;
        idx ++ ;
        devp += devpsets ;
    }

    if ( ! found )
    {
        if ( first_free != -1 )
        {
            devno = first_free ;
            strcpy((char *)KGS->dev0.dev[devno].devspace,
                    (const char *)devname );
            DBG1 (( MF__,"en54_find_device_no: first_free %d \n", first_free ));
        }
        else
        {
            MSGD ((ERR_FIND_DEVNO_INSUFFICIENT_DEVICES, msg ));
            ok      = false ;
            errcode = DEVIO_ERR_MAXDEVNO ;
        }
    }
    else
    {
        MSGD ((IERR_FIND_DEVNO_ALREADY_ATTACHED, devname ));
        ok      = false ;
        errcode = DEVIO_ERR_ALRATT ;
    }

    if ( request->req_type == REQ_VATTACH_EO00 )
    {
        request->args.vat_parms.devno   = devno ;
        *pErrCode                       = errcode ;
    }
    else
    {
        request->args.vas_parms.devno   = devno ;
        *pErrCode                       = errcode ;
    }
    return ok;
}
/* en54_find_device_no */

/*
 * ==================================================================
 */

static tsp00_Bool en54_Dev0Vattach ( struct DOUBLY_LINKED   *request )
/*
 * May only be called by the DEV0 process
 * Thus only the DEVI thread.
 * The child,DEVi does the necessary initialization and calls the
 * main_procedure to perform its function.
 */
{
    #undef MF__
    #define MF__ MOD__"en54_Dev0Vattach"
    SAPDB_Int                   rc;
    SAPDB_Int                   devno;
    SAPDB_Int                   ndevi;
    struct DEV_PCB             *devi_pcb;
    struct REQ_PARMS_VATTACH   *parms;
    struct IOC_QUEUE           *iox, *first;
    char                       *devname ;
    tsp00_ErrTextc             threadErrtext;
    teo07_ThreadErr            err;

    DBGIN;

    parms = &request->args.vat_parms;

    if ( -1 == parms->devno ) /* Only once per volume */
    {
        parms->ok = en54_find_device_no ( request,
                                          parms->volumeName,
                                          &parms->errcode );
        if (! parms->ok )
        {
            MSGD ((IERR_DEV0_VATTACH_FAILED ));
            return false;
        }
    }

    devno = parms->devno ; /* current volume worker group index */
    ndevi = parms->ndevi ; /* current volume worker process index */

    devname = KGS->dev0.dev[devno].devspace ;
    KGS->dev0.dev[devno].devtype = parms->devtype,
    KGS->dev0.dev[devno].accessMode = parms->accessMode;
    KGS->dev0.dev[devno].fileSystemAccessType = RTE_FileSystemAccessTypeNormal;
    DBG1 (( MF__,"devno %d \n", devno ));
    DBG1 (( MF__,"index %d name '%s'\n", ndevi-1 , devname ));

    devi_pcb = KGS->first_dev + devno * KGS->devpsets + ndevi - 1;
    devi_pcb->state  = KT_RUNNING ;

    /* PTS 1107963 Do no allow vattach while in SERVER_STOP/KILL */
    if ( KGS->state == SERVER_STOP || KGS->state == SERVER_KILL )
    {
        MSGD ((ERR_DEV0_VATTACH_CANNOT_OPEN, "database shutdown" ));
        MSGD ((ERR_DEV0_VATTACH_CANNOT_OPEN_CONT, devname ));
        parms->ok       = false;
        parms->errcode  = DEVIO_ERR_OPEN;
        devi_pcb->state  = KT_INITIALIZED ;
        return false;
    }

    /*
     * ensure that pointers are correct
     */
    first = &devi_pcb->ios[0];
    DBG1 (( MF__,"init devi_pcb, first 0x%lx\n", (long) first ));
    first->request = 0 ;
    for ( iox = first->next ; iox != first ; iox = iox->next )
        iox->request = 0 ;
    devi_pcb->io_work    = iox->next ;
    devi_pcb->io_request = iox->next ;
    devi_pcb->request = request;

    if ( XPARAM(preallocateIOWorker) )
    {
		sqlresumethread( devi_pcb->thread, threadErrtext, &err);
        if ( err != THR_OK_EO07 )
        {
            MSGD (( ERR_RESUME_IOTHREAD, devname, &threadErrtext[0] ));
            rc = -1;
        }
        else
        {
            rc = 0;
        }
    }
    else
    {
        rc = sql41_create_sem ( 0, 0, 0, 0660, 0, (char *)"db", (char *) KGS->serverdb );
	    if ( -1 == rc )
        {
            MSGD (( ERR_DEV0_VATTACH_ERR_FORK, "Out of IPC semaphores for thread synchronization..." ));
        }
        else
        {
            devi_pcb->semid = rc;

#if defined(LINUX) && !defined(HAS_NPTL)
            sqlbeginthread( 0, /* PTS 1106187 */
#else
            sqlbeginthread( KT_DEVI_STACK_SIZE,
#endif
                            en54_Mdevi_main,
                            devi_pcb,
                            THR_CREATE_SUSPENDED_EO07,
                            & devi_pcb->thread,
                            threadErrtext,
                            &err );
            if ( err == THR_OK_EO07 )
            {
                devi_pcb->tid = sqlthreadid( devi_pcb->thread );
                sqlresumethread( devi_pcb->thread, threadErrtext, &err);
                if ( err != THR_OK_EO07 )
                {
                    MSGD (( ERR_RESUME_IOTHREAD, devname, threadErrtext.asCharp() ));
                    sql41_remove_sem(&(devi_pcb->semid), (char *)"db", (char *)KGS->serverdb);
                    rc = -1;
                }
            }
            else
            {
                MSGD ((ERR_DEV0_VATTACH_ERR_FORK, threadErrtext.asCharp() ));
                sql41_remove_sem( &(devi_pcb->semid), (char *)"db", (char *)KGS->serverdb );
                rc = -1;
            }
        }
    }

    if ( -1 == rc )
    {
        parms->ok       = false;
        parms->errcode  = DEVIO_ERR_DEVICREA;
        devi_pcb->fd    = -1;
        devi_pcb->state  = KT_INITIALIZED ;
        return false;
	}

    /*
     *  the DEVI process replies to DEV0
     */
    DBGOUT;
    return true;
}

static tsp00_Bool en54_DeviVattach ( struct DEV_PCB *devi_pcb )
/*
 * May only be called by the DEV0 process
 * Assumes that the dsab is already reserved and that the devno is known.
 * Thus only opens the volume, spawn the DEVI thread and enters
 * that taskId in the dsab.
 * The child,DEVi does the necessary initialization and calls the
 * main_procedure to perform its function.
 */

{
#undef MF__
#define MF__ MOD__"en54_DeviVattach"
    struct DOUBLY_LINKED      *request;
    SAPDB_Int                  tmp_fd;
    SAPDB_Int			       rc;
    SAPDB_Int                  devno;
    SAPDB_Int                  ndevi;
    struct REQ_PARMS_VATTACH   *parms;
    struct flock               lockf;
    char                       *devname ;
    tsp00_Bool                 forceReadOnly;

	DBGIN;

    request = devi_pcb->request;
    parms = reinterpret_cast<struct REQ_PARMS_VATTACH *>(&request->args);
    devno = parms->devno ; /* current volume worker group index */
    ndevi = parms->ndevi ; /* current volume worker process index */

    devname = KGS->dev0.dev[devno].devspace ;
    KGS->dev0.dev[devno].devtype = parms->devtype,
    KGS->dev0.dev[devno].accessMode = parms->accessMode;
    DBG1 (( MF__,"devno %d \n", devno ));
    DBG1 (( MF__,"index %d name '%s'\n", ndevi-1 , devname ));

    forceReadOnly = ( sp2dt_log == KGS->dev0.dev[devno].devtype
                   || sp2dt_mirr_log == KGS->dev0.dev[devno].devtype )
                  && RTEHSS_IsLogReadOnly();

    if ( (tmp_fd = en54_DevOpen( (char *)devname,
                                 forceReadOnly,
                                 devi_pcb->io_f,
                                 &devi_pcb->devcap,
                                 & devi_pcb->dev_type)) < 0 )
    {
        MSGD ((ERR_DEV0_VATTACH_CANNOT_OPEN, sqlerrs() ));
        MSGD ((ERR_DEV0_VATTACH_CANNOT_OPEN_CONT, devname ));
        parms->ok       = false;
        parms->errcode  = DEVIO_ERR_OPEN;
        return false;
    }

    MSGD (( INFO_OPEN_FD, devname, tmp_fd ));

    /*
     * protect the volumes against attaches from other databases,
     * don't lock vtrace (devno 0, may be in NFS -> uuhhh )
     */

    devi_pcb->isLocked = false;

    if ( (ndevi == KGS->devpsets)
      && XPARAM(fSetVolumeLock)
      && ! IS_TRACE_DEV(parms->devno)
#if ( SUN && SUNOS ) || OSF1 || defined (FREEBSD) || defined(LINUX)
      && devi_pcb->dev_type == MOD_ORDIFILE /* cannot lock raw-devices on Solaris or OSF/1 */
#endif
      && !forceReadOnly )
    {
        lockf.l_type   = F_WRLCK;
        lockf.l_whence = 0;
        lockf.l_start  = 0;
        lockf.l_len    = 0;
        lockf.l_pid    = -1;

        if ( fcntl ( tmp_fd, F_SETLK, & lockf ) == -1 )
        {
            MSGD ((ERR_DEV0_VATTACH_ERR_FCNTL, sqlerrs() ));
            if ( ( errno == EACCES ) || ( errno == EAGAIN ) )
            {
                 MSGD ((ERR_DEV0_VATTACH_DEVSPACE_LOCKED, devname ));
            }
            else
            {
                 MSGD ((ERR_DEV0_VATTACH_DEVSPACE_NOT_LOCKABLE, devname ));
            }
            parms->ok       = false;
            parms->errcode  = DEVIO_ERR_LOCK;
            (void) RTE_save_close(tmp_fd);
            return false;
        }
        else
        {
            devi_pcb->isLocked = true;
        }

        DBG1 (( MF__,"lock successful, %d\n", tmp_fd ));
    }

    rc = en54_Page0IO ( tmp_fd , sp2ioc_read ,  & (KGS->dev_blocksize) , 0,
                     true /*is_devspace*/, (char*)0 /* label */ );
    if ( rc != OK )
    {
        parms->ok       = false ;
        parms->errcode  = DEVIO_ERR_OPEN ;
        return false;
    }

	devi_pcb->fd      = tmp_fd;

    parms->ok = true;
	parms->ndevi--;

    if ( 0 == parms->ndevi )
	{
		/*
		 * Wake task
		 */
		DBG1 (( MF__,"son DEVi, en71_ioc to T%d\n", request->taskid->index ));
		en71_iocEnquAndWake( request->taskid->ukt, request->taskid, request );
	}
	else
	{
        /*
         * Reissue request to DEV0
         */
		DBG1 (( MF__,"son DEVi, en54_RequestToDev0 \n"));
		en54_RequestToDev0 ( request );
	}

	DBGOUT;
	return true;
} /* en54_DeviVattach */

/*--------------------------------------------*/

static void en54_KillAllOtherDevi( struct REQ_PARMS_VATTACH *parms )
{
    int ndevi;

    if ( IS_TRACE_DEV(parms->devno) )
    {
        return; /* nothing to do... The first devi already failed */
    }

    for ( ndevi = parms->ndevi; ndevi < KGS->devpsets; ++ndevi )
    {
	    tsp00_Int4                   exit_status;
        tsp00_ErrTextc               errtext;
        teo07_ThreadErr              err;
        struct DEV_PCB*              killPcb;
        RTETask_CommonQueueElement&  queueElement = *RTE_KGS::Instance().GlobalFreeList()->Dequeue();
        DLQ_ELEMENT*                 killRequest  = static_cast<DLQ_ELEMENT*>(queueElement);

        killPcb = KGS->first_dev + parms->devno * KGS->devpsets + ndevi - 1 + 1;
        /* The devi to kill uses the 'next' slot !!! */

        killRequest->taskid                  = 0;
        killRequest->req_type                = REQ_VDETACH_EO00 ;
        killRequest->args.vde_parms.devno    = parms->devno;
        killRequest->args.vde_parms.this_req = killRequest;
        killRequest->args.vde_parms.ndevi    = NO_REPLY_WANTED_EN54; /* this force vdetach not to send the reply... */
        killRequest->args.vde_parms.rewind   = false;
        killRequest->args.vde_parms.ok       = true;
        killRequest->args.vde_parms.errcode  = (SAPDB_UInt4)UNDEF;

        en54_RequestToDevi ( killPcb , killRequest );

        DBG1 (( MF__,"waiting for termination of TID %ld \n", (long) killPcb->tid ));

        if ( !XPARAM(preallocateIOWorker) )
        {
	        sqljointhread( killPcb->thread, &exit_status, errtext, &err );
            if ( err != THR_OK_EO07 )
            {
                DBG1 (( MF__,"Thread join error: %s\n", errtext ));
            }

            queueElement.Release();
	        killPcb->thread = NULL;
	        killPcb->tid    = 0;
        }
    }
}

/* ------------------------------ */

extern "C" void *en54_Mdevi_main( void *arg )
{
#undef MF__
#define MF__ MOD__"en54_Mdevi_main"
    struct DEV_PCB *pcb = (struct DEV_PCB *)arg;
    struct DOUBLY_LINKED *request;
    tsp00_Bool attachOrOpenFailed;

    DBGIN;

    do {
        attachOrOpenFailed = false;
        request = pcb->request;

        SAPDB_Int                  devno;

        /* find devno from parameters */
        if ( request->req_type == REQ_VATTACH_EO00 )
	    {
            devno = request->args.vat_parms.devno ; /* current volume worker group index */
        }
        else
        {
            devno = request->args.vas_parms.devno ; /* current volume worker group index */
        }

        /* Reset statistics */
        SAPDB_memset(&(pcb->DevIOStat), 0, sizeof(teo52_DevIOStatistic) ) ;

	    /*
	     * this will also reply to DEV0 that next may proceed
	     */
	    if ( request->req_type == REQ_VATTACH_EO00 )
	    {
            if ( !en54_DeviVattach( pcb ) )
            {
                en54_KillAllOtherDevi( &request->args.vat_parms );
                attachOrOpenFailed = true;
            }
	    }
	    else
	    {
            if ( !en54_DeviVasynopen( pcb ) )
            {
                attachOrOpenFailed = true;
            }
	    }

        if ( attachOrOpenFailed )
        {
            if ( !XPARAM(preallocateIOWorker) )
            {
                sqlforgetthread(pcb->thread);
    	        pcb->thread = NULL;
	            pcb->tid    = 0;
                sql41_remove_sem(&(pcb->semid), (char *)"db", (char *)KGS->serverdb);
            }
    	    pcb->state = KT_INITIALIZED;
            en71_iocEnquAndWake( request->taskid->ukt, request->taskid, request );   /* send reply back */
        }
        else
        {
            en54_DeviMain ( pcb );
        }

        if ( XPARAM(preallocateIOWorker) )
        {
            tsp00_ErrTextc              threadErrtext;
            teo07_ThreadErr             err;
            sqlsuspendthread(pcb->thread, threadErrtext, &err);
            if ( err != THR_OK_EO07 )
            {
                MSGD (( ERR_SUSPEND_IOTHREAD, "DEVi", &threadErrtext[0] ));
                vabort(WRITE_CORE);
            }
        }

    } while ( XPARAM(preallocateIOWorker) );

    DBG1 (( MF__,"DEV removed, tid %ld \n", (long) pcb->tid ));
    MSGD (( INFO_PROCESS_STOPPED, "DEVi" ));

    return NULL;
} /* en54_Mdevi_main */

/*
 * ==================================================================
 */

static tsp00_Bool en54_Dev0Vasynopen( struct DOUBLY_LINKED   *request)
/*
 * May only be called by the DEV0 process
 * Assumes that the dsab is already reserved and that the devno is known.
 * Thus only opens the volume, spawns the DEVI thread and enters
 * that taskId in the dsab, then closes the volume again.
 * The child,DEVi does the necessary initialization and calls the
 * main_procedure to perform its function.
 */
{
    #undef MF__
    #define MF__ MOD__"en54_Dev0Vasynopen"
    struct REQ_PARMS_ASYNOPEN * parms;
    struct DEV_PCB            * devi_pcb;
    struct IOC_QUEUE          * iox, *first;
    SAPDB_Int                   rc;
    SAPDB_Int                   devno;
    tsp00_ErrTextc              threadErrtext;
    teo07_ThreadErr             err;

    DBGIN;

    parms = &request->args.vas_parms;

    parms->ok = en54_find_device_no( request,
                                     parms->deviceName,
                                    &parms->errcode);
    if ( !parms->ok )
    {
        MSGD ((ERR_DEV0_VATTACH_CANNOT_OPEN, "database shutdown" ));
        MSGD ((ERR_DEV0_VATTACH_CANNOT_OPEN_CONT, parms->deviceName ));
        return false;
    }

    devno = parms->devno ;
    if ( !parms->is_devspace && parms->for_writing )
    {
		SAPDB_memset( KGS->dev0.dev[devno].devspace_label, '_', sizeof ( tsp_vf_label ) );
    }

    DBG1 (( MF__,"To create devi for >%s< \n", parms->deviceName ));
    DBG1 (( MF__,"devno              %d\n", parms->devno ));
    DBG1 (( MF__,"index %d, name '%s' \n", parms->ndevi-1 , parms->deviceName ));

    devi_pcb = en54_FindAsyncDevpcb ( devno ) ;

    DBG1 (( MF__,"using devi slot    %d\n",   parms->ndevi - 1 ));
    DBG1 (( MF__,"devi_pcb is        0x%x\n", devi_pcb ));

    devi_pcb->state  = KT_RUNNING;

        /* PTS 1107963 Do no allow vasynopen while in SERVER_STOP/KILL */
    if ( KGS->state == SERVER_STOP || KGS->state == SERVER_KILL )
    {
        MSGD ((ERR_DEV0_VATTACH_CANNOT_OPEN, "database shutdown" ));
        MSGD ((ERR_DEV0_VATTACH_CANNOT_OPEN_CONT, parms->deviceName ));
        parms->ok       = false;
        parms->errcode  = DEVIO_ERR_OPEN;
        devi_pcb->state  = KT_INITIALIZED ;
        return false;
    }

    /*
     * ensure that pointers are correct
     */
    first = &devi_pcb->ios[0];
    DBG1 (( MF__,"init devi_pc: first = 0x%x\n", first ));
    first->request = 0;
    for ( iox = first->next; iox != first; iox = iox->next )
        iox->request = 0;
    devi_pcb->io_work    = iox->next;
    devi_pcb->io_request = iox->next;
    devi_pcb->request = request;

    if ( XPARAM(preallocateIOWorker) )
    {
		sqlresumethread( devi_pcb->thread, threadErrtext, &err);
        if ( err != THR_OK_EO07 )
        {
            MSGD (( ERR_RESUME_IOTHREAD, parms->deviceName, &threadErrtext[0] ));
            rc = -1;
        }
        else
        {
            rc = 0;
        }
    }
    else
    {
        /* PTS 1125036 do no longer vabort() if semaphore not available */
	    rc = sql41_create_sem ( 0, 0, 0, 0660, 0, (char *)"db", (char *) KGS->serverdb );
	    if ( -1 == rc )
	    {
		    MSGD (( ERR_DEV0_ASYNOPEN_ERR_SEMAPHORE, "resource problem" ));
        }
        else
        {
		    devi_pcb->semid = rc;

#if defined(LINUX) && !defined(HAS_NPTL)
            sqlbeginthread( 0, /* PTS 1106187 */
#else
            sqlbeginthread( KT_DEVI_STACK_SIZE,
#endif
                            en54_Mdevi_main,
                            devi_pcb,
                            THR_CREATE_SUSPENDED_EO07,
                            & devi_pcb->thread,
                            threadErrtext,
                            &err );
            if ( err == THR_OK_EO07 )
	        {
                tsp00_ErrTextc threadErrorText;
		        devi_pcb->tid = sqlthreadid( devi_pcb->thread );
		        sqlresumethread( devi_pcb->thread, threadErrorText, &err);
	        }
	        else
	        {
		        MSGD (( ERR_DEV0_ASYNOPEN_ERR_THREAD_CREATE, threadErrtext.asCharp() ));

                sql41_remove_sem( &(devi_pcb->semid), (char *)"db", (char *)KGS->serverdb );
                rc = -1;
	        }
        }
    }

	if ( -1 == rc )
	{
        parms->ok       = false;
        parms->errcode  = DEVIO_ERR_DEVICREA;
        devi_pcb->fd    = -1;
        devi_pcb->state  = KT_INITIALIZED ;
        return false;
    }

    /*
     *  the DEVI process replies to DEV0
     */
    DBGOUT;
    return true;
} /* en54_Dev0Vasynopen */

/*
 * ==================================================================
 */
/*
 * May only be called by a DEVi process
 * It opens the devspace and informs the task that is waiting.
 * This call is allowed to block until open is complete.
 */
static tsp00_Bool en54_DeviVasynopen ( struct DEV_PCB * devi_pcb )
{

    #undef MF__
    #define MF__ MOD__"en54_DeviVasynopen"

    struct DOUBLY_LINKED      * request;
    struct REQ_PARMS_ASYNOPEN * parms;
    char                      * devname ;
    int                         devno;
    int                         tmp_fd;
    int			                rc;
    tsp00_Int4                  Page_0_Size ;

    DBGIN;

    request = devi_pcb->request;
    parms = &request->args.vas_parms;

    devno = parms->devno ;
    devname = parms->deviceName;

    /* PTS 1107963 Do no allow vasynopen while in SERVER_STOP/KILL */
    if ( KGS->state == SERVER_STOP || KGS->state == SERVER_KILL )
    {
        MSGD ((ERR_DEV0_VATTACH_CANNOT_OPEN, "database shutdown" ));
        MSGD ((ERR_DEV0_VATTACH_CANNOT_OPEN_CONT, devname ));
        parms->ok       = false;
        parms->errcode  = DEVIO_ERR_OPEN;
        return false;
    }

    tmp_fd = en54_OpenAsyncDevice( reinterpret_cast<ten50_UKT_Control *>(& KGS->dev0),
                                     request, parms, (char *)devname);
    if ( tmp_fd < 0 )
    {
        parms->ok       = false;
        parms->errcode  = DEVIO_ERR_OPEN;
        return false;
    }

    MSGD (( INFO_OPEN_FD, devname, tmp_fd ));

    /*
     *  Create/Skip the magic record.
     *  IST 1994-02-03:
     *  Creation should not be done by asynopen:
     *  - if is_devspace then already written by vdevsize.
     *  - if ! is_devspace (e.g. tape) no header should be required.
     *  BUT, this will not be included in 6.1.1!
     */
    Page_0_Size = parms->is_devspace ? parms->block_size
                                     : parms->block_size * parms->max_block_cnt;
    DBG1 (( MF__,"size of page 0 , %d\n", Page_0_Size ));

    if ( parms->for_writing )
    {
        rc = en54_Page0IO ( tmp_fd , sp2ioc_write , &Page_0_Size , 0,
                         (tsp00_Bool) parms->is_devspace, KGS->dev0.dev[devno].devspace_label );
    }
    else
    {
        SAPDB_Int4 PageSizeWanted = Page_0_Size ;
        rc = en54_Page0IO ( tmp_fd , sp2ioc_read , &PageSizeWanted , 0,
                         (tsp00_Bool) parms->is_devspace, KGS->dev0.dev[devno].devspace_label );
        if ( rc != OK && PageSizeWanted != Page_0_Size )
        {
             parms->max_block_cnt = PageSizeWanted /  parms->block_size ;
             MSGD ((IERR_DEV0_ASYNOPEN_MAXBLOCK_CHANGED, parms->max_block_cnt ));
        }
    }

    if ( rc != OK )
    {
        (void) RTE_save_close ( tmp_fd ) ;
        parms->ok       = false;
        parms->errcode  = DEVIO_ERR_PAGE_0;
        return false;
    }

    if (parms->fileType != vf_t_unknown) {
        /* test if device kind matches client expectation */
        struct DEV_CNTL_INFO    devCntlInfo;
        tsp00_VfType             realFileType;
        SAPDB_UInt4                   returncode;

        devCntlInfo.filedes       = tmp_fd;
        devCntlInfo.hostfileno    = parms->devno;
        devCntlInfo.block_size    = parms->block_size;
        devCntlInfo.max_block_cnt = parms->max_block_cnt;
        devCntlInfo.deviceName    = devname;
        devCntlInfo.asyn_write    = parms->for_writing;
        devCntlInfo.destructiveCheck = false;
        realFileType = en54_StatFileType (&devCntlInfo, &returncode);
        if ((returncode != (SAPDB_UInt4)UNDEF) || (realFileType != parms->fileType)) {
            (void) RTE_save_close ( tmp_fd ) ;
            parms->ok       = false;
            if (returncode == (SAPDB_UInt4)UNDEF) {
                parms->errcode  = DEVIO_ERR_FILETYPE;
                MSGD ((ERR_DEV0_ASYNOPEN_WRONG_FTYPE, ftype_text[realFileType],
                    ftype_text[parms->fileType]));
            }
            else {
                parms->errcode  = returncode;
                MSGD ((ERR_DEV0_ASYNOPEN_ERR_STAT, errtab[returncode]));
            }
            return false;
        }
    }

	devi_pcb->fd      = tmp_fd;

    parms->ok = true;
	parms->ndevi--;

    if ( 0 == parms->ndevi )
	{
		/*
		 * Wake task
		 */
		DBG1 (( MF__,"son DEVi, en71_ioc to T%d\n", request->taskid->index ));
		en71_iocEnquAndWake( request->taskid->ukt, request->taskid, request );
	}
	else
	{
        /*
         * Reissue request to DEV0
         */
		DBG1 (( MF__,"son DEVi, en54_RequestToDev0 \n"));
		en54_RequestToDev0 ( request );
	}

    DBGOUT;
	return true;
} /* en54_DeviVasynopen */
/*
 * ==================================================================
 */

static SAPDB_Int en54_ResetNonBlockingFlag(SAPDB_Int fd)
{
#undef MF__
#define MF__ MOD__"en54_ResetNonBlockingFlag"
    /* Reset NON_BLOCKING flag */
    SAPDB_Int rc = 0;
    long flags;

    flags = fcntl(fd, F_GETFL);
    flags &= ~O_NONBLOCK;
    if ( fcntl(fd, F_SETFL, flags) < 0 )
    {
        MSGD ((ERR_DEV0_ASYNOPEN_ERR_OPEN, sqlerrs() ));
        rc = -1;
    }
    return rc;
} /* en54_ResetNonBlockingFlag */

/*
 * ==================================================================
 */

static SAPDB_Int en54_IsExistingPipe(const char *devname)
{
#undef MF__
#define MF__ MOD__"en54_IsExistingPipe"
    SAPDB_Int rc;
    struct stat statBuf;
    rc = RTE_save_stat(devname, &statBuf);

    if ( rc == 0 )
    {
        rc = S_ISFIFO(statBuf.st_mode);
    }
    else
    {
        rc = 0;
    }
    return rc;
} /* en54_IsExistingPipe */

/*
 * ==================================================================
 */

/* PTS 1113528 */
static SAPDB_Int en54_OpenAsyncDevice(
ten50_UKT_Control         * this_ukt ,
struct DOUBLY_LINKED      * request,
struct REQ_PARMS_ASYNOPEN * parms,
char                      * devname )
{
#undef MF__
#define MF__ MOD__"en54_OpenAsyncDevice"
    SAPDB_Int   rc;
    SAPDB_Int   isExistingPipe;
    SAPDB_Int   tmp_fd;
    struct stat statBuf;
    SAPDB_Int   openFlags;

    DBGIN;

    isExistingPipe = en54_IsExistingPipe(devname);

    if ( isExistingPipe && parms->is_devspace )
    {
        MSGD ((ERR_DEV0_ASYNOPEN_WRONG_FTYPE, ftype_text[vf_t_pipe],
            "raw or regular file"));
        return -1;
    }

    if ( *(parms->cancelPointer) )
    {
        MSGD ((ERR_DEV0_ASYNOPEN_ERR_OPEN, "task canceled" ));
        /* PTS 1110557 */
        return -1;
    }

    /* Open flags on pipes get NON_BLOCK mode added */
    if ( parms->for_writing )
        if ( parms->is_devspace )
#ifdef PAGE_0_PRESERVE
            openFlags = DEFAULT_OPEN_MODE_EN54 | en54_AddSynchronousOpen() | O_RDWR;
#else
            openFlags = DEFAULT_OPEN_MODE_EN54 | en54_AddSynchronousOpen() | O_WRONLY;
#endif
        else
            openFlags = DEFAULT_OPEN_MODE_EN54 |O_WRONLY|O_TRUNC|O_CREAT;
    else
        openFlags = DEFAULT_OPEN_MODE_EN54 | O_RDONLY;

    if ( isExistingPipe )
    {
        switch( en54_pipeCheckAlgorithm ) /* CR 1115397 */
        {
        case EN54_FSTAT_PIPE_CHECK_CODE_OK:
            openFlags |= O_NONBLOCK;
            break;
        case EN54_POLL_PIPE_CHECK_CODE_OK:
            openFlags |= O_NONBLOCK;
            break;
        default:
            if ( parms->for_writing ) /* open for read must be done blocked, since input detection not available */
            {
                openFlags |= O_NONBLOCK;
            }
            break;
        }
    }


    do {
        if ( parms->for_writing )
            if ( parms->is_devspace )
                tmp_fd = RTE_save_open ( devname, openFlags );
            else
                tmp_fd = RTE_save_openCreate ( devname , openFlags, 0660 );
        else
            tmp_fd = RTE_save_open ( devname, openFlags );

        if ( tmp_fd < 0 )
        {
            if ( !parms->for_writing
              || !isExistingPipe
              || ENXIO != errno )
            {
                MSGD ((ERR_DEV0_ASYNOPEN_ERR_OPEN, sqlerrs() ));
                MSGD ((ERR_DEV0_ASYNOPEN_ERR_OPEN_CONT, devname ));
                return -1;
            }
            else
            {
                sqlyieldthread(); /* Polling on pipe opened for write */
            }
        }
    } while ( tmp_fd < 0 && !*(parms->cancelPointer) );

    if ( *(parms->cancelPointer) )
    {
        if ( tmp_fd >= 0 )
        {
            MSGD ((ERR_DEV0_ASYNOPEN_ERR_OPEN, "task canceled" ));
            RTE_save_close(tmp_fd);
            tmp_fd = -1;
        }
    }

    if ( isExistingPipe && (tmp_fd >= 0) ) /* CR 1115397 */
    {
        /* Pipe was opened non blocking, but blocking calls are wanted later on */

        if ( parms->for_writing )
        {
            if ( en54_ResetNonBlockingFlag(tmp_fd) )
            {
                MSGD ((ERR_DEV0_ASYNOPEN_ERR_OPEN, "reset NON_BLOCK flag on pipe failed" ));
                RTE_save_close(tmp_fd);
                tmp_fd = -1;
            }
        }
        else
        {
            SAPDB_Int mustResetNonBlocking = 0;

            switch( en54_pipeCheckAlgorithm ) /* CR 1115397 */
            {
            case EN54_FSTAT_PIPE_CHECK_CODE_OK:
                mustResetNonBlocking = 1;
                break;
            case EN54_POLL_PIPE_CHECK_CODE_OK:
                mustResetNonBlocking = 1;
                break;
            default:
                break;
            }
            if ( mustResetNonBlocking )
            {
                if ( en54_ResetNonBlockingFlag(tmp_fd) )
                {
                    MSGD ((ERR_DEV0_ASYNOPEN_ERR_OPEN, "reset NON_BLOCK flag on pipe failed" ));
                    RTE_save_close(tmp_fd);
                    tmp_fd = -1;
                }
                else
                {
                    if ( EN54_FSTAT_PIPE_CHECK_CODE_OK == en54_pipeCheckAlgorithm ) /* CR 1115397 */
                    {
                        struct stat statBuf;
                        do {
                            rc = RTE_save_fstat(tmp_fd, &statBuf);
                            if ( rc < 0 )
                            {
                                MSGD ((ERR_DEV0_ASYNOPEN_ERR_OPEN, "stat on pipe failed" ));
                                RTE_save_close(tmp_fd);
                                tmp_fd = -1;
                                break;
                            }
                            if ( statBuf.st_size == 0 )
                            {
                                sqlyieldthread(); /* Polling on pipe opened for read */
                            }
                        } while ( statBuf.st_size == 0 && !*(parms->cancelPointer) );
                    }
                    else if (  EN54_POLL_PIPE_CHECK_CODE_OK == en54_pipeCheckAlgorithm )
                    {
                        struct pollfd pipeIn[1];

                        do {
                            pipeIn[0].fd = tmp_fd;
                            pipeIn[0].events = POLLIN;
                            pipeIn[0].revents = 0;

                            rc = RTE_save_poll(&pipeIn[0], 1, 0);
                            if ( rc < 0 )
                            {
                                MSGD ((ERR_DEV0_ASYNOPEN_ERR_OPEN, "poll on pipe failed" ));
                                RTE_save_close(tmp_fd);
                                tmp_fd = -1;
                                break;
                            }

                            if ( (1 == rc) && ((pipeIn[0].revents & POLLIN) != 0) )
                            {
                                break; /* input on pipe available */
                            }

                            if ( 0 == rc )
                            {
                                sqlyieldthread(); /* Polling on pipe opened for read */
                            }
                        } while ( rc >= 0 && !*(parms->cancelPointer) );
                    }
                }
            }
        }
    }

    if ( *(parms->cancelPointer) )
    {
        if ( tmp_fd >= 0 )
        {
            MSGD ((ERR_DEV0_ASYNOPEN_ERR_OPEN, "task canceled" ));
            RTE_save_close(tmp_fd);
            tmp_fd = -1;
        }
    }

    DBGOUT;

    return tmp_fd;
} /* en54_OpenAsyncDevice */

/*
 * ==================================================================
 */

static  void    en54_DeviVblockio      (
                        struct DOUBLY_LINKED   *request,
                        struct DEV_PCB         *devi_pcb,
                        INTFUNC_PTR            *io_f,
                        SAPDB_Int                     fd)
/*
 *  May not be called in the DEV0 process, only in the DEVi process
 *  This is the mother process controlling the volume 'i', all
 *  requests are addressed to it.
 *
 *  The io-function used to perform io on the volume is taken from
 *  the PCB it is the correct function for the appropriate device, This
 *  field is set during vattach.
 */
{
    #undef MF__
    #define MF__ MOD__"en54_DeviVblockio"
    SAPDB_Int                         slot ;
    struct DEV_PCB              *devpcb ;
    struct REQ_PARMS_VBLOCKIO   *parms ;


    parms = reinterpret_cast<struct REQ_PARMS_VBLOCKIO*>(&request->args);
    DBG1 (( MF__,"called,    for devno  %d \n", parms->devno ));
    DBG1 (( MF__,"called,    for devpno %d \n", parms->devpno ));

    /*
     *  Simulate an I/O error for test porposes.
     */
    devpcb = KGS->first_dev + parms->devno * KGS->devpsets ;
    if ( devpcb->set_io_error &&
         ((devpcb->set_io_error & ~0x100) == parms->op_code) )
    {
        devpcb->set_io_error = 0 ;
        parms->ok = false ;
        parms->errcode = DEVIO_ERR_USER_REQ ;
         MSGD ((IERR_EEVI_VBLOCKIO_TEST_IO_ERROR,
                (parms->op_code == sp2ioc_read) ? "reading from" : "writing to" ,
                (long) parms->devno , (long) parms->devpno ));
    }
    else
    {
        teo52_IOStatistic *pDevStat ;

        if ( parms->op_code == sp2ioc_read )
        {
          pDevStat = & (devi_pcb->DevIOStat.DevRead) ;
        }
        else
        {
          pDevStat = & (devi_pcb->DevIOStat.DevWrite) ;
        }

        eo52UpdateCountInIOStatisticRec(*pDevStat, parms->buf_count);

        if ( XPARAM(fTimeMeasure) )
        {
            SAPDB_UInt8 startTime = RTESys_MicroSecTimer();
#ifdef WANT_DATAWRITE_TIME_INFO
            if ( request->taskid->type == TT_DW_EO00 )
            {
                MSGD(( INFO_TIME_START, request->taskid->index, "deviVblockio", startTime ));
            }
#endif
            parms->ok = ( (*io_f[parms->op_code]) (devi_pcb, fd, (char *)parms->p,
                                 (long)(KGS->dev_blocksize*parms->buf_count),
                                 parms->devpno, &parms->errcode) > 0 ) ;
            SAPDB_UInt8 deltaTime = RTESys_MicroSecTimer() - startTime;
#ifdef WANT_DATAWRITE_TIME_INFO
            if ( request->taskid->type == TT_DW_EO00 )
            {
                MSGD(( INFO_TIME_STOP, request->taskid->index, "deviVblockio", startTime, startTime + deltaTime, deltaTime ));
            }
#endif
            eo52UpdateTimeInStatisticRec(pDevStat->IOStat, deltaTime);
        }
        else
        {
            parms->ok = ( (*io_f[parms->op_code]) (devi_pcb, fd, (char *)parms->p,
                                 (long)(KGS->dev_blocksize*parms->buf_count),
                                 parms->devpno, &parms->errcode) > 0 ) ;
        }
    }

    DBG1 (( MF__,"parms->ok = %s \n", parms->ok ? "true" : "false" ));
    DBG1 (( MF__,"Return for devpno %d \n", parms->devpno ));

    en54_RequestToUKT ( devi_pcb , request );

    return;

}/* en54_DeviVblockio */


/*
 * ==================================================================
 */

static  void    en54_DeviVVectorio      (
                        struct DOUBLY_LINKED   *request,
                        struct DEV_PCB         *devi_pcb,
                        SAPDB_Int                     fd)
/*
 *  May not be called in the DEV0 process, only in the DEVi process
 *  This is the mother process controlling the volume 'i', all
 *  requests are addressed to it.
 *
 *  The io-function used to perform io on the volume is taken from
 *  the PCB it is the correct function for the appropriate device, This
 *  field is set during vattach.
 */
{
#undef MF__
#define MF__ MOD__"en54_DeviVVectorio"
    SAPDB_Int                         slot ;
    struct DEV_PCB              *devpcb ;
    struct REQ_PARMS_VVECTORIO  *parms ;
    teo52_IOStatistic           *pDevStat ;

    parms = reinterpret_cast<struct REQ_PARMS_VVECTORIO*>(&request->args);
    DBG1 (( MF__,"called,    for devno  %d \n", parms->devno ));
    DBG1 (( MF__,"called,    for devpno %d \n", parms->devpno ));

    devpcb = KGS->first_dev + parms->devno * KGS->devpsets ;

    pDevStat = & (devi_pcb->DevIOStat.DevWrite) ;

    eo52UpdateCountInIOStatisticRec(*pDevStat, parms->pageCount);

    if ( XPARAM(fTimeMeasure) )
    {
        SAPDB_UInt8 startTime = RTESys_MicroSecTimer();
        if ( RTE_CheckForSimulateVectorIo( devpcb->dev_type == MOD_BLCKSPEC ) )
        {
            parms->ok = ( en54_SimulateDiskIOWriteVector(devi_pcb, fd,
                              parms->pageVector, parms->pageCount,
                              parms->devpno, &parms->errcode) > 0 ) ;
        }
        else
        {
            parms->ok = ( en54_DiskIOWriteVector(devi_pcb, fd,
                              parms->pageVector, parms->pageCount,
                              parms->devpno, &parms->errcode) > 0 ) ;
        }
        eo52UpdateTimeInStatisticRec(pDevStat->IOStat, RTESys_MicroSecTimer() - startTime);
    }
    else
    {
        if ( RTE_CheckForSimulateVectorIo( devpcb->dev_type == MOD_BLCKSPEC ) )
        {
            parms->ok = ( en54_SimulateDiskIOWriteVector(devi_pcb, fd,
                              parms->pageVector, parms->pageCount,
                              parms->devpno, &parms->errcode) > 0 ) ;
        }
        else
        {
            parms->ok = ( en54_DiskIOWriteVector(devi_pcb, fd,
                              parms->pageVector, parms->pageCount,
                              parms->devpno, &parms->errcode) > 0 ) ;
        }
    }

    DBG1 (( MF__,"parms->ok = %s \n", parms->ok ? "true" : "false" ));
    DBG1 (( MF__,"Return for devpno %d \n", parms->devpno ));

    en54_RequestToUKT ( devi_pcb , request );

    return;
}/* en54_DeviVVectorio */


/*
 * ==================================================================
 */
static  void    en54_DeviVmarkBadDev  (
                        struct DOUBLY_LINKED   *request,
                        struct DEV_PCB         *devi_pcb,
                        SAPDB_Int                     fd )
{
    #undef MF__
    #define MF__ MOD__"en54_DeviVmarkBadDev"
    struct REQ_PARMS_VBLOCKIO   *parms ;
    struct ALIGNED_MEM bufMem;
    UCHAR           *buf ;
    SAPDB_Int4 givenBlockSize = XPARAM(ulDBPageSize);

    parms = reinterpret_cast<struct REQ_PARMS_VBLOCKIO*>(&request->args);
    devi_pcb = KGS->first_dev + parms->devno * KGS->devpsets ;
    fd       = devi_pcb->fd ;
    parms->ok = false ;
    DBG1 (( MF__,"called,    for devno  %d \n", parms->devno ));
    DBG1 (( MF__,"called,    for devpno %d \n", parms->devpno ));

    if ( (buf = en54_AllocAligned( &bufMem, givenBlockSize, NULL)) == NULL )
    {
        MSGCD (( ERR_VMARK_BAD_DEV_NO_MEMORY, givenBlockSize,  sqlerrs() ));
    }
    else
    {
        if ( en54_SignalSave_lseek ( fd , NO_OFFSET , 0 )    == 0 )
        {
            off_t readBytes = en54_SignalSave_read  ( fd , buf , givenBlockSize );

            if ( readBytes == givenBlockSize )
            {
                teo54Page0Info *pInfo = (teo54Page0Info *)(buf + PAGE_0_RECORD_OFFSET_EO54);

                if ( !strcmp(pInfo->magicHeader_eo54, MAGIC_HEADER_EO54) )
                {
                    pInfo->badMarkedPage_eo54 = eo54SavablePageNo(parms->devpno);
                }
                else
                {
                    ((teo54OldUnixPage0Info *)buf)->badMarkedPage_Ueo54 = parms->devpno;
                }

                if ( en54_SignalSave_lseek ( fd , NO_OFFSET , 0 ) == 0 )
                {
                    off_t writtenBytes = en54_SignalSave_write ( fd , buf , givenBlockSize );
                    if ( writtenBytes == givenBlockSize )
                    {
                        MSGD (( ERR_DEVSP_MARKED_AS_BAD , parms->devpno));
                        parms->ok = true ;
                    }
                }
            }
        }
        devi_pcb->old_io_endpos = -10 ;
        en54_FreeAligned( &bufMem );
    }

    DBG1 (( MF__,"parms->ok = %s \n", parms->ok ? "true" : "false" ));
    DBG1 (( MF__,"Return for devpno %d \n", parms->devpno ));

    en54_RequestToUKT ( devi_pcb , request );

    return;
}

/*
 * ==================================================================
 */

static  void    devi_asynio (
                        struct DOUBLY_LINKED   *request,
                        struct DEV_PCB         *devi_pcb,
                        SAPDB_Int                    fd)

/*
 *  May not be called in the DEV0 process, only in the DEVi process
 *  This is the mother process controlling the volume 'i', all
 *  requests are addressed to it.
 *
 *  The io-function used to perform io on the volume is taken from
 *  the PCB it is the correct function for the appropriate device, This
 *  field is set during vattach.
 */
{
    #undef MF__
    #define MF__ MOD__"devi_asynio"
    long                        length ;
    long                        offset ;
    struct REQ_PARMS_ASYNIO     *parms ;


    parms = &request->args.vai_parms ;
    DBG1 (( MF__,"req 0x%lx for T%d \n",
            (long) request , request->taskid->index ));
    DBG1 (( MF__,"id %ld dev %ld cnt %ld ok %d\n",
            (long) parms->io_id , (long) parms->devno ,
            (long) parms->io_count , parms->ok ));
    DBG1 (( MF__,"addr 0x%lx len %ld pos %ld \n",
            (long) parms->block_ptr , (long) parms->block_cnt ,
            (long) parms->devpno ));

    length = (long) parms->block_siz * (long) parms->block_cnt ;
    offset = (long) parms->devpno ;
    DBG1 (( MF__,"offset %ld length %ld \n", offset , length ));

    if ( XPARAM(fTimeMeasure) )
    {
        SAPDB_UInt8 startTime = RTESys_MicroSecTimer();
        if ( KGS->dev0.dev[parms->devno].asyn_write )
        {
            offset = en54_DiskIOWrite ( devi_pcb, fd , (char *)parms->block_ptr ,
                            length , offset , &parms->errcode );
            eo52UpdateTimeInStatisticRec(devi_pcb->DevIOStat.DevWrite.IOStat, RTESys_MicroSecTimer() - startTime);
        }
        else
        {
            offset = en54_DeviAsynioReadBlock ( devi_pcb, fd , parms, length , offset );
            eo52UpdateTimeInStatisticRec(devi_pcb->DevIOStat.DevRead.IOStat, RTESys_MicroSecTimer() - startTime);
        }
    }
    else
    {
        if ( KGS->dev0.dev[parms->devno].asyn_write )
        {
            offset = en54_DiskIOWrite ( devi_pcb, fd , (char *)parms->block_ptr ,
                            length , offset , &parms->errcode );
        }
        else
        {
            offset = en54_DeviAsynioReadBlock ( devi_pcb, fd , parms, length , offset );
        }
    }
    parms->block_cnt = offset / parms->block_siz ;
    parms->ok = offset > 0 ;

    DBG1 (( MF__,"req 0x%lx for T%d \n",
            (long) request , request->taskid->index ));
    DBG1 (( MF__,"id %ld dev %ld cnt %ld ok %d\n",
            (long) parms->io_id , (long) parms->devno ,
            (long) parms->io_count , parms->ok ));
    DBG1 (( MF__,"addr 0x%lx len %ld pos %ld \n",
            (long) parms->block_ptr , (long) parms->block_cnt ,
            (long) parms->devpno ));

    en54_RequestToUKT ( devi_pcb , request );
}
/* devi_asynio */

/*
 * ==================================================================
 */

static  void    devi_asyncntl (
                        struct DOUBLY_LINKED   *request,
                        struct DEV_PCB         *devi_pcb,
                        SAPDB_Int                    fd)

{
    #undef MF__
    #define MF__ MOD__"devi_asyncntl"
    struct  DEV_CNTL_INFO    devCntlInfo ;
    struct dev_info         *devInfo;

    DBG1 (( MF__,"with subrequest %d \n", request->args.vac_parms.cntlRequest));
    switch (request->args.vac_parms.cntlRequest) {
        case ASYNCNTL_FILETYPE:
            devCntlInfo.filedes = fd;
            devCntlInfo.hostfileno = request->args.vac_parms.hostfileno;
            devInfo = &KGS->dev0.dev[devCntlInfo.hostfileno];
            devCntlInfo.block_size    = devInfo->block_size;
            devCntlInfo.max_block_cnt = devInfo->max_block_cnt;
            devCntlInfo.deviceName    = devInfo->devspace;
            devCntlInfo.asyn_write    = devInfo->asyn_write;
            DBG1 (( MF__,"%s check\n", request->args.vac_parms.destructive ? "destructive" : "nondestructive"));
            devCntlInfo.destructiveCheck = request->args.vac_parms.destructive;
            request->args.vac_parms.fileType = en54_StatFileType (
                &devCntlInfo, &request->args.vac_parms.errcode);
            break;
        default:
             MSGD ((IERR_DEVI_ASYNCNTL_INVALID_REQUEST,
                                        request->args.vac_parms.cntlRequest ));
            vabort(WRITE_CORE);
            break;
    }
    if (request->args.vac_parms.errcode != (SAPDB_UInt4)UNDEF)
        request->args.vac_parms.ok = false;
    else
        request->args.vac_parms.ok = true;
    en54_RequestToUKT ( devi_pcb , request );
}

/*
 * ==================================================================
 */

    #undef MF__
    #define MF__ MOD__"en54_Dev0Vdetach"
static  void    en54_Dev0Vdetach (
         struct DOUBLY_LINKED   *request)
{
    struct REQ_PARMS_VDETACH    *parms ;
    SAPDB_Int                            devno ;
    SAPDB_Int                            ndevi ;
    struct DEV_PCB              *pcb;
    SAPDB_Int4                   exit_status;
    tsp00_ErrTextc               errtext;
    teo07_ThreadErr              err;
    SAPDB_Int                          rc;

    /*
     *  may only be called in the DEV0 process
     */
    parms = reinterpret_cast<struct REQ_PARMS_VDETACH*>(&request->args);
    devno = parms->devno ;
    ndevi = parms->ndevi ;
    DBG1 (( MF__,"devno %d ndevi %d\n", devno , ndevi ));

    /*
     * awaken each thread and wait for its confirmation
     */
    pcb = request->req_type == REQ_ASYNCLOSE_EO00
          ? en54_FindAsyncDevpcb ( devno )
          : KGS->first_dev + devno * KGS->devpsets + ndevi - 1 ;

    DBG1 (( MF__,"detach dev slot %d \n", ndevi - 1 ));
    DBG1 (( MF__,"detach pcb      0x%lx T%d\n", (long) pcb, pcb->tid ));

    while ( (pcb->state != KT_SLEEPING) || (pcb->io_work != pcb->io_request) )
    {
        sqlyieldthread();
        DBG1 (( MF__,"pcb->state %d io_work 0x%p io_request 0x%p",
            pcb->state, pcb->io_work, pcb->io_request ));
    }

    if ( request->req_type != REQ_ASYNCLOSE_EO00 )
    {
        en54_WaitForSelfIOCompletionAndClose(devno);
    }

    en54_RequestToDevi ( pcb , request );

    DBG1 (( MF__,"waiting for termination of TID %ld \n", (long) pcb->tid ));

    if ( !XPARAM(preallocateIOWorker) )
    {
        sqljointhread( pcb->thread, &exit_status, errtext, &err );
        if ( err != THR_OK_EO07 )
        {
            DBG1 (( MF__,"Thread join error: %s\n", errtext ));
        }

        pcb->thread = NULL;
        pcb->tid    = 0;
    }
    pcb->state = KT_INITIALIZED ;
}
/* en54_Dev0Vdetach */

/*
 * ==================================================================
 */

static  void    en54_DeviVdetach (
struct DOUBLY_LINKED   *request,
struct DEV_PCB              *pcb)
{
    #undef MF__
    #define MF__ MOD__"en54_DeviVdetach"
    struct REQ_PARMS_VDETACH    *parms ;
    int fdToClose;
    /*
     * may only be called in the DEVi process
     */
    DBGIN;

    parms      = reinterpret_cast<struct REQ_PARMS_VDETACH*>( &request->args );

    /* PTS 1103806 */
    fdToClose = pcb->fd;
    pcb->fd = -1;
    if ( RTE_save_close( fdToClose ) == -1 )
    {
        int lastErrno = errno;
        char badFileDescriptor[80];
        sp77sprintf(badFileDescriptor, 80, "Bad file descriptor %d!", fdToClose);

        MSGD ((IERR_DEVI_VDETACH_ERR_CLOSE, badFileDescriptor ));
        errno = lastErrno;
        MSGD ((IERR_DEVI_VDETACH_ERR_CLOSE, sqlerrs() ));

        request->args.vde_parms.ok       = false;
        request->args.vde_parms.errcode  = DEVIO_ERR_CLOSE;
    }
    else
    if ( request->req_type             == REQ_ASYNCLOSE_EO00 &&
         request->args.vde_parms.ndevi == 1             &&
         request->args.vde_parms.rewind)
    {
      SAPDB_Int RewindFd = RTE_save_open(KGS->dev0.dev[request->args.vde_parms.devno].devspace, O_RDWR) ;
      DBG1 (( MF__,"en54_DeviVdetach: DEVi try to rewind fd: %d \n", RewindFd ));
      if ( RewindFd >= 0 )
      {
        en54_Rewind (RewindFd, UNDEF, true, &request->args.vde_parms.errcode);
        RTE_save_close( RewindFd );
      }
      else
      {
        MSGD ((IERR_ASYNCLOSE_ERR_OPEN_FOR_REWIND,
               &KGS->dev0.dev[request->args.vde_parms.devno].devspace[0],
               sqlerrs() ));
      }
    }

    pcb->state = KT_INITIALIZED ;
    if ( !XPARAM(preallocateIOWorker) )
    {
        (void) sql41_remove_sem ( & pcb->semid, (char *)"db", (char *)KGS->serverdb );
    }

    /*
       No reply wanted... Used to terminated devi by en54_KillAllOtherDevi() if initialization of other devi for device failed
     */
    if ( NO_REPLY_WANTED_EN54 == parms->ndevi )
    {
        return;
    }

    /*
     *  Reply to DEV0 that next may proceed
     *  or to UKP that the last has gone.
     */
    parms->ndevi -- ;
    if ( parms->ndevi > 0 ) /* Still a DEV-Process alive, detach it */
    {
        en54_RequestToDev0 ( request );
    }
    else                /* all DEV Processes detached, reply to UKP */
    {
        en54_RequestToUKT ( pcb , request );              /* send reply back */
    }
}
/* en54_DeviVdetach */

/*
 * ==================================================================
 */

/*ARGSUSED*/
static  SAPDB_Int     en54_WrongCall (
struct DEV_PCB         *devi_pcb,
SAPDB_Int                     fd,
char                   *buf,
long                    length,
long                    pno,
SAPDB_UInt4                  *errcode)
{
    #undef MF__
    #define MF__ MOD__"en54_WrongCall"
     MSGD ((IERR_WRONGCALL ));
    return DEVIO_NOREPLY;
}/* en54_WrongCall */

/*
 * ==================================================================
 */
static  void    en54_Dev0Main (void)
{
    #undef MF__
    #define MF__ MOD__"en54_Dev0Main"
    struct DOUBLY_LINKED        *request ;


    /*
     *  The main loop for the DEV0 process. A request is
     *  always received but never answered by myself.
     *  Attach, detach
     */
    for ( ; ; )
    {
        DBG1 (( MF__,"KGS->dev0.io_work          0x%lx\n",
                                            (long) KGS->dev0.io_work ));
        DBG1 (( MF__,"KGS->dev0.io_request       0x%lx\n",
                                            (long) KGS->dev0.io_request ));
        DBG1 (( MF__,"KGS->dev0.io_work->request 0x%lx\n",
                                            (long) KGS->dev0.io_work->request ));

        /*
         *  Wait for a request to DEV0
         */
        request = en54_dev0Receive ();
        if ( !request )
        {
            /* PTS 1107963 Cannot happen... */
            MSGD ((IERR_DEV0_MAIN_INVALID_REQUEST, -1 ));
            break;
        }

        DBG1 (( MF__,"got request %d \n", request->req_type ));

        switch ( request->req_type )
        {
        case REQ_VATTACH_EO00 :
            if ( !en54_Dev0Vattach ( request ) )
            {
                en71_iocEnquAndWake ( request->taskid->ukt, request->taskid, request );   /* send reply back */
                en54_KillAllOtherDevi( &request->args.vat_parms );
            }
            break;

        case REQ_VDETACH_EO00 :
            en54_Dev0Vdetach ( request );
            break;

        case REQ_ASYNOPEN_EO00 :
            if ( !en54_Dev0Vasynopen ( request ) )
            {
                en71_iocEnquAndWake ( request->taskid->ukt, request->taskid, request );   /* send reply back */
            }
            break;

        case REQ_ASYNCLOSE_EO00 :
            en54_Dev0Vdetach ( request );
            break;

        default :
            MSGD ((IERR_DEV0_MAIN_INVALID_REQUEST, request->req_type ));
            en71_iocEnquAndWake ( request->taskid->ukt, request->taskid , request );
            break;
        }
        DBG1 (( MF__,"next in loop \n" ));
    } /* PTS 1107963 Do no longer react on SERVER_STOP/KILL */
    /*NOTREACHED*/
}
/* en54_Dev0Main */

/*
 * ==================================================================
 */
    #undef MF__
    #define MF__ MOD__"en54_DeviMain"
static  void   en54_DeviMain ( struct DEV_PCB *pcb )
{
    struct DOUBLY_LINKED                *request ;
    SAPDB_Int                                 rc ;
    struct sembuf                       own_semrcv ;
    SAPDB_Int                                 own_semid ;
    INTFUNC_PTR                         io_f [ 2 ];
    SAPDB_Int                                 fd ;
    SAPDB_Int                                 detached = false;
    struct teo52_DevIOStatisticRec *pDeviStatistic = &pcb->DevIOStat;
    /*
     *  The main loop for the DEVI process, a request is
     *  always received and always returned
     */

    SAPDB_memcpy ( &own_semrcv , &semrcv , sizeof(own_semrcv) );
    SAPDB_memcpy ( io_f , &pcb->io_f[0] , sizeof(io_f) );
    own_semid             = pcb->semid ;
    pcb->old_io_endpos    = -10 ;
    fd                    = pcb->fd ;
    DBG1 (( MF__," DEVi semid %d \n", own_semid ));

    MSGD (( INFO_PROCESS_STARTED, "DEVi" ));
    while ( !detached )
    {
/* PTS 1105262 / PTS 1105263 */
        do {
            WAIT_UNTIL_ALONE ( pcb->alone );
                request = pcb->io_work->request ;
                if ( pDeviStatistic->ResetPending )
                {
                    eo52ResetStatisticRecord(*pDeviStatistic);
                    pDeviStatistic->ResetPending = false;
                }
            CLEARLOCK ( pcb->alone );
        /* pcb->io_work is updated in en54_RequestToUKT() */

        if ( request )
        {
              DBG1 (( MF__,"got request 0x%0lx type %d T%d \n",
                      (long) request , request->req_type, request->taskid->index ));
          switch ( request->req_type )
          {
          case REQ_VBLOCKIO_EO00:
              en54_DeviVblockio ( request, pcb, io_f, fd );
              break;

          case REQ_VVECTORIO_EO00:
              en54_DeviVVectorio ( request, pcb, fd );
              break;

          case REQ_VMARK_BAD_DEV_EO00:
               en54_DeviVmarkBadDev ( request , pcb, fd );
               break;

          case REQ_ASYNIO_EO00:
              devi_asynio ( request, pcb, fd );
              break;

          case REQ_ASYNCNTL_EO00:
              devi_asyncntl (request, pcb, fd);
              break;

          case REQ_ASYNCLOSE_EO00 :
          /* PTS 1103806 */
              en54_DeviVdetach ( request, pcb );
              detached = true;
              break;

          case REQ_VDETACH_EO00 :
              en54_DeviVdetach ( request, pcb );
              detached = true;
              break;

          default :
               MSGD ((IERR_DEVI_MAIN_INVALID_REQUEST,
                      request->req_type ));
               vabort(WRITE_CORE);
                       break;
          }
        }
        DBG1 (( MF__," next in loop \n" ));
/* PTS 1105262 / PTS 1105263 */
        } while ( request != NULL && !detached );

        if ( !detached )
        {
          DBG1 (( MF__,"DEVi will sleep on semid %d \n", own_semid ));
          if ( !pcb->fSemAlreadyPosted )
          {
            pcb->state  = KT_SLEEPING ;
            rc = RTE_save_semop ( own_semid, &own_semrcv, 1 ); /* wait until wakened */
            if ( rc == -1 )
            {
                MSGD ((IERR_DEVI_MAIN_ERR_SEMOP, sqlerrs() ));
                vabort(WRITE_CORE);
            }
            pcb->state  = KT_RUNNING ;
          }
          pcb->fSemAlreadyPosted = 0;
        }
    } /* PTS 1107963 Do no longer react on SERVER_STOP/KILL */
    return;
}
/* en54_DeviMain */

/*
 * ==================================================================
 */
static  SAPDB_Int     en54_Page0IO (
                        SAPDB_Int                 fd,
                        tsp2_io_op_code_Param     op_code,
                        SAPDB_Int4               *page_0_size,
                        SAPDB_Int4                capacity,
                        tsp00_Bool                is_devspace,
                        char                     *f_label)

{
    #undef MF__
    #define MF__ MOD__"en54_Page0IO"
    SAPDB_Int                         rc ;
    SAPDB_Int                         retcod = OK;
    struct ALIGNED_MEM          bufmem;
    UCHAR                      *buf ;
    SAPDB_Int4                  givenBlockSize = *page_0_size ;

    DBG1 (( MF__," called on %d for %s (%d)\n", fd,
        (op_code == sp2ioc_read) ? "read" : "write", op_code ));

    if ( (buf = en54_AllocAligned ( &bufmem, givenBlockSize, NULL)) == NULL )
    {
        MSGD (( ERR_PAGE_0_IO_NO_MEMORY, givenBlockSize, sqlerrs() ));
        retcod = UNDEF ;
    }
    else
    {
        teo54Page0Info *pInfo = (teo54Page0Info *)(buf + PAGE_0_RECORD_OFFSET_EO54);

        if ( op_code == sp2ioc_write )
        {
            SAPDB_memset ( buf, 0, givenBlockSize ); /* clear complete buffer */
#ifdef PAGE_0_PRESERVE
            if ( is_devspace )
            {
                if ( 0 != en54_SignalSave_lseek ( fd , NO_OFFSET , 0 ) ) /* works on empty files too!!! */
                {
                    rc = 0;
                }
                else
                {
                    rc = RTE_save_read ( fd , buf , givenBlockSize );

                    if (              0 != rc /* empty file is ok too!!! */
                      && givenBlockSize != rc )
                    {
                        rc = 0;
                    }
                    else if ( 0 != en54_SignalSave_lseek ( fd , NO_OFFSET , 0 ) )
                    {
                        rc = 0;
                    }
                    else
                    {
                        rc = 1;
                    }
                }
            }
            else
            {
                rc = 1;
            }

            if ( 1 == rc )
#endif /* PAGE_0_PRESERVE */
            {
                if ( capacity == 0 )
                {
                    capacity = pInfo->maxSize_eo54; /* reuse found capacity */
                }
                strcpy(pInfo->magicHeader_eo54, MAGIC_HEADER_EO54);
                pInfo->blockSize_eo54 = eo54SavableInt4(givenBlockSize);
                pInfo->maxSize_eo54 = eo54SavableInt4(capacity);
                pInfo->actualSize_eo54 = eo54SavableInt4(0); /* NYI */
                pInfo->badMarkedPage_eo54 = eo54SavablePageNo(NO_BAD_PAGE_MARKED_EO54);

                if ( is_devspace )
                {
                    SAPDB_memcpy( pInfo->label_eo54, DEVSPACE_LABEL_EO54,
                            strlen(DEVSPACE_LABEL_EO54) + 1);
                }
                else
                {
                    if ( f_label )
                    {
                        if ( strlen(f_label) <= SIZEOF_TSP_VF_LABEL_EO54 )
                        {
                            SAPDB_memcpy( pInfo->label_eo54, f_label, strlen(f_label)+1);
                        }
                        else
                        {
                            SAPDB_memcpy( pInfo->label_eo54, f_label, SIZEOF_TSP_VF_LABEL_EO54 );
                        }
                        pInfo->label_eo54[SIZEOF_TSP_VF_LABEL_EO54] = '\0';
                    }
                }
                rc = en54_SignalSave_write ( fd , buf , givenBlockSize );
            }
        }
        else
        {
            SAPDB_Int4 expectedBlockSize;
            SAPDB_Int4 blockSizeFound;
            tsp00_PageNo badMarkedPageFound;
            char       * labelFound;

            rc = is_devspace ? en54_ReadWithRetry( fd , 0, (char *)buf , givenBlockSize ) /* PTS 1106176 */
                             : en54_ReadBlock   ( fd , (tsp00_PageAddr) buf , (long) givenBlockSize );

            if ( rc == (SAPDB_Int) givenBlockSize )
            {
                if ( !strcmp(pInfo->magicHeader_eo54, MAGIC_HEADER_EO54) )
                {
                /*
                 * New page 0 Layout ( beginning with release 7.02.00 )
                 */
                    pInfo->blockSize_eo54 = eo54RestoredInt4(pInfo->blockSize_eo54);
                    pInfo->maxSize_eo54 = eo54RestoredInt4(pInfo->maxSize_eo54);
                    pInfo->actualSize_eo54 = eo54RestoredInt4(pInfo->actualSize_eo54);
                    pInfo->badMarkedPage_eo54 = eo54RestoredInt4(pInfo->badMarkedPage_eo54);

                    expectedBlockSize  = givenBlockSize;
                    blockSizeFound     = pInfo->blockSize_eo54;
                    badMarkedPageFound = pInfo->badMarkedPage_eo54;
                    labelFound         = pInfo->label_eo54;
                }
                else
                {
                /*
                 * Old page 0 Layout ( until release 7.02.00 )
                 */
                    expectedBlockSize  = (givenBlockSize & 0xFFFF);
                    blockSizeFound     =
                  ((unsigned char)(((teo54OldUnixPage0Info *)buf)->blockSizeByte1_Ueo54) << 8)
                 | (unsigned char)(((teo54OldUnixPage0Info *)buf)->blockSizeByte0_Ueo54);
                    badMarkedPageFound = ((teo54OldUnixPage0Info *)buf)->badMarkedPage_Ueo54;
                    labelFound         = ((teo54OldUnixPage0Info *)buf)->label_Ueo54;
                }

                if ( is_devspace && rc == (SAPDB_Int) givenBlockSize )
                {
                    if ( badMarkedPageFound != NO_BAD_PAGE_MARKED_EO54 )
                    {
                        MSGD ((ERR_PAGE0IO_DEVSP_MARKED_AS_BAD, badMarkedPageFound ));
                        retcod = UNDEF ;
                    }
                }

                if ( retcod == OK )
                {
                    if ( rc == (SAPDB_Int) givenBlockSize )
                    {
#ifdef DEBUG_RTE
                        char label_buf[SIZEOF_TSP_VF_LABEL_EO54+1];
                        if ( labelFound[0] != '\0' )
                        {
                            for ( rc = 0; rc <= SIZEOF_TSP_VF_LABEL_EO54; rc++ )
                            {
                                label_buf[rc] = labelFound[rc];
                                if ( ! isprint(labelFound[rc]) )
                                    label_buf[rc] = '.' ;
                                if ( !label_buf[rc] ) break;
                            }
                            label_buf[SIZEOF_TSP_VF_LABEL_EO54] = '\0';
                            DBG1 (( MF__,"label = %s\n", label_buf )) ;
                        }
                        else
                        {
                            DBG1 (( MF__,"no file_label found !! \n"));
                        }
#endif /* DEBUG_RTE */
                        rc = (memcmp ( labelFound, DEVSPACE_LABEL_EO54,
                                        strlen(DEVSPACE_LABEL_EO54)+1 ) == 0);

                        if ( !is_devspace && f_label )
                        {
                            labelFound[SIZEOF_TSP_VF_LABEL_EO54] = '\0';
                            SAPDB_memcpy( f_label, labelFound, strlen(labelFound)+1 );
                        }

                        if (   expectedBlockSize != blockSizeFound
                            || ( is_devspace && !rc )
                            || ( !is_devspace && rc ) )
                        {
                            if ( !is_devspace )
                            {
                                *page_0_size = blockSizeFound;
                            }
                            retcod = UNDEF ;

                            MSGD (( IERR_PAGE0IO_INVALID_FORMAT ));
                            MSGD (( IERR_PAGE0IO_INVALID_FORMAT_CONT,
                                givenBlockSize, blockSizeFound ));
                        }
                        /*  IST 1994-12-05
                         *  From 3K to 4K is reserved for write-tests by R/3 (Achim).
                         */
                        rc = (SAPDB_Int) givenBlockSize ;
                    }
                }
            }
        }

        if ( rc != (SAPDB_Int) givenBlockSize )
        {
            retcod = UNDEF ;
        }

        en54_FreeAligned( &bufmem );
    }

    return ( retcod );
}
/* en54_Page0IO */

/*
 * ==================================================================
 */

/* PTS 1106175 */
static SAPDB_Int  en54_ReadWithRetry (SAPDB_Int                     fd,
                           off_t                   offset,
                           char                   *block,
                           unsigned                block_len)
{
  SAPDB_Int BytesRead ;
  SAPDB_Int Retries = 0 ;

  while ( (BytesRead = en54_SignalSave_read_partial ( fd , block , block_len ) ) != block_len && ++Retries < NUMBER_OF_IO_RETRIES )
  {
    MSGD ((ERR_IO_READBLOCK_ERR_READ_SIZE_TRY_AGAIN, BytesRead, block_len, sqlerrs() ));
    sleep(1);
    if ( en54_SignalSave_lseek(fd, offset, 0) == (off_t)-1 )
    {
        break;
    }
  }

  return ( BytesRead ) ;
}

/*
 * ==================================================================
 */

/*
 * This function performs read on a file/tape/pipe of one block
 * if the specified file is a pipe, it can be neccessarry to make
 * multiple reads to get the whole block
 */

static SAPDB_Int  en54_ReadBlock (
                        SAPDB_Int                     fd,
                        tsp00_PageAddr          block,
                        long                    block_len)

{
#undef MF__
#define MF__ MOD__"en54_ReadBlock"
   long          rc ;
   char         *blockPointer;
   long          bytesAlreadyRead = 0;

   DBG1 (( MF__,"addr 0x%lx len %ld \n", (long) block , block_len ));

   do
   {
       blockPointer = ((char *)block) + bytesAlreadyRead;
       rc = en54_SignalSave_read_partial ( fd , blockPointer, block_len-bytesAlreadyRead );
       if ( rc > 0 )
       {
            bytesAlreadyRead += rc;
       }
   } while ( (rc > 0)
          && (bytesAlreadyRead < block_len) );

   return (int)( rc < 0 ? rc : bytesAlreadyRead );
}

/*
 * ==================================================================
 */

/*
 * This function performs read on file ( used by devi_asynio ) !!! PIPE !!!
 */

static  SAPDB_Int     en54_DeviAsynioReadBlock (
                        struct DEV_PCB           *devi_pcb,
                        SAPDB_Int                       fd,
                        struct REQ_PARMS_ASYNIO  *parms,
                        long                      length,
                        long                      pno )

{
    #undef MF__
    #define MF__ MOD__"en54_DeviAsynioReadBlock"
    SAPDB_Int                     rc;
    off_t                   offset;
    char                   *buf     = (char *)parms->block_ptr ;
    SAPDB_UInt4            *errcode = &parms->errcode ;

    offset = ((off_t) pno + (off_t) 1) * (off_t) KGS->dev_blocksize ;
    DBG1 (( MF__," addr 0x%lx len %ld off %ld \n",
        buf , length , offset ));

    if ( (offset == devi_pcb->old_io_endpos) || (pno == -1) )
    {
        rc = KGS->dev0.dev[parms->devno].IsDevspace
                  ? en54_ReadWithRetry ( fd , offset, buf , (unsigned) length ) /* PTS 1106176 */
                  : en54_ReadBlock    ( fd , (tsp00_PageAddr) buf , (long) length );

        if ( rc != length )
        {
            devi_pcb->old_io_endpos = -7;
            if ( rc == -1 )
            {
                 MSGD ((ERR_AIO_READBLOCK_ERR_READ, sqlerrs() ));
                *errcode = DEVIO_ERR_SEWR;
            }
            else
            {
                 MSGD ((ERR_AIO_READBLOCK_ERR_READ_SIZE, (unsigned) rc , length ));
                *errcode = DEVIO_ERR_SEWR;
            }
            /*
             *  ASYNIO needs the number of bytes read.
             *  BLOCKIO needs a true/false return.
             */
            if ( pno != -1 )
                rc = false ;
        }
        else
        {
            devi_pcb->old_io_endpos = offset + (off_t) length;
            /*
             *  ASYNIO needs the number of bytes read.
             *  BLOCKIO needs a true/false return.
             */
            if ( pno != -1 )
                rc = true;
        }
    }
    else
    {
        /* PTS 1106176 */
        rc = offset == en54_SignalSave_lseek(fd, offset, 0) ;  /* F.H. pipe ?, tape ? */

        if ( rc )
        {
          SAPDB_Int LengthRead = KGS->dev0.dev[parms->devno].IsDevspace
                             ? en54_ReadWithRetry ( fd , offset, buf , (unsigned) length ) /* PTS 1106176 */
                             : en54_ReadBlock    ( fd , (tsp00_PageAddr)buf , (long) length );

          if ( (rc = length == LengthRead) )
            devi_pcb->old_io_endpos = offset + (off_t) length;
        }

        if ( rc == false )
        {
            devi_pcb->old_io_endpos = -7;
             MSGD ((ERR_AIO_READBLOCK_ERR_READ2, sqlerrs() ));
            *errcode = DEVIO_ERR_SEWR;
            rc = false;
        }
    }

    return ( rc );
}
/* en54_DeviAsynioReadBlock */

/*
 * ==================================================================
 */

/*
 * This function performs io on the volume defined as a file thus
 * unix-direct io.
 */

static  SAPDB_Int     en54_DiskIORead (
                        struct DEV_PCB         *devi_pcb,
                        SAPDB_Int                     fd,
                        char                   *buf,
                        long                    length,
                        long                    pno,
                        SAPDB_UInt4            *errcode)
{
    #undef MF__
    #define MF__ MOD__"en54_DiskIORead"
    SAPDB_Int                                rc;
    off_t                              offset;


    offset = ((off_t) pno + (off_t) 1) * (off_t) KGS->dev_blocksize ;
    DBG1 (( MF__," addr 0x%lx len %ld off %ld \n",
        buf , length , offset ));

    if ( (offset == devi_pcb->old_io_endpos) || (pno == -1) )
    {
        /* PTS 1106176 */
        rc = en54_ReadWithRetry ( fd , offset, (char *)buf , length );
        if ( rc != length )
        {
            devi_pcb->old_io_endpos = -6;
            *errcode = DEVIO_ERR_SEWR;
            /*
             *  ASYNIO needs the number of bytes read.
             *  BLOCKIO needs a true/false return.
             */
            if ( pno != -1 )
                rc = false ;
        }
        else
        {
            devi_pcb->old_io_endpos = offset + (off_t) length;
            /*
             *  ASYNIO needs the number of bytes read.
             *  BLOCKIO needs a true/false return.
             */
            if ( pno != -1 )
                rc = true;
        }
    }
    else
    {
        if ((offset == en54_SignalSave_lseek(fd, offset, 0)
           && ( length == en54_ReadWithRetry(fd, offset, (char *)buf, (unsigned) length )))) /* PTS 1106176 */
        {
            devi_pcb->old_io_endpos = offset + (off_t) length;
            rc = true;
        }
        else
        {
            devi_pcb->old_io_endpos = -6;
            *errcode = DEVIO_ERR_SEWR;
            rc = false;
        }
    }

    return ( rc );
}
/* en54_DiskIORead */

/*
 * ==================================================================
 */

/*
 * This function performs io on the volume defined as a file thus
 * unix-direct io.
 */

static  SAPDB_Int     en54_DiskIOWrite (
                        struct DEV_PCB         *devi_pcb,
                        SAPDB_Int                     fd,
                        char                   *buf,
                        long                    length,
                        long                    pno,
                        SAPDB_UInt4            *errcode)
{
    #undef MF__
    #define MF__ MOD__"en54_DiskIOWrite"
    SAPDB_Int                                 rc ;
    off_t                               offset;


    offset = ((off_t) pno + (off_t) 1) * (off_t) KGS->dev_blocksize ;
    DBG1 (( MF__," addr 0x%lx len %ld off %ld \n",
        buf , length , offset ));

    if ( (offset == devi_pcb->old_io_endpos) || (pno == -1) )
    {
        rc = en54_SignalSave_write ( fd , buf , (unsigned) length );
        if ( rc != length )
        {
            devi_pcb->old_io_endpos = -5;
            *errcode = DEVIO_ERR_SEWR;
            /*
             *  ASYNIO needs the number of bytes read.
             *  BLOCKIO needs a true/false return.
             */
            if ( pno != -1 )
                rc = false ;
        }
        else
        {
            devi_pcb->old_io_endpos = offset + (off_t) length;
            /*
             *  ASYNIO needs the number of bytes read.
             *  BLOCKIO needs a true/false return.
             */
            if ( pno != -1 )
                rc = true;
        }
    }
    else
    {
        if ( offset == en54_SignalSave_lseek( fd, offset, 0 ) &&
            length == en54_SignalSave_write( fd, buf, (unsigned) length ) )
        {
            devi_pcb->old_io_endpos = offset + (off_t) length;
            rc = true ;
        }
        else
        {
            devi_pcb->old_io_endpos = -5;
            *errcode = DEVIO_ERR_SEWR;
            rc = false ;
        }
    }

    return ( rc );
}
/* en54_DiskIOWrite */

/*
 * ==================================================================
 */

static  SAPDB_Int     en54_DiskIOWriteVector (
                        struct DEV_PCB         *devi_pcb,
                        SAPDB_Int                     fd,
                        tsp00_PageAddr          pageVector[],
                        SAPDB_Int4              pageCount,
                        SAPDB_Int4              pno,
                        SAPDB_UInt4            *errcode)
{
#undef MF__
#define MF__ MOD__"en54_DiskIOWriteVector"
    SAPDB_Int                                 rc = true;
    off_t                               offset;

    offset = ((off_t) pno + (off_t) 1) * (off_t) KGS->dev_blocksize ;

    /* Seek always if pno == -1 (RTE internal page0...) */
    if ( (offset != devi_pcb->old_io_endpos) || (-1 == pno) )
    {
      if ( offset != en54_SignalSave_lseek( fd, offset, 0 ) )
      {
        char buffer[EN54_SMALL_MESSAGE_BUFFER_SIZE];
        sp77sprintf(buffer, sizeof(buffer), "PNO %d : %s", pno, sqlerrs() );

        MSGD ((ERR_DEVIO_ERR_SEEK_FD, fd, &buffer[0] ));
        *errcode = DEVIO_ERR_SEWR;
        rc = false ;
      }
    }

    if ( rc )
    {
      long            ret_code;
      SAPDB_Int             i;
      struct iovec    ioVector[SAPDB_IOV_LIMIT]; /* OS dependend found in limits.h */
      SAPDB_Int iovLimit = (SAPDB_IOV_LIMIT < IOV_MAX ? SAPDB_IOV_LIMIT : IOV_MAX);
      SAPDB_Int iovMax;

      SAPDB_memset(ioVector, 0, sizeof(struct iovec) * iovLimit); /* is stack is too small, this at least will force an overflow */

      for ( i = 0; rc && i < pageCount; i += iovMax )
      {
        SAPDB_Int j;

        /* Last io Action must not use all ioVector elements */
        if ( (i + iovLimit) > pageCount )
        {
          iovMax = pageCount - i; /* calculate remaining I/O pages */
        }
        else
        {
          iovMax = iovLimit; /* use maximum offered by OS */
        }
            SAPDB_Int retry = 0;
            SAPDB_Int iovWrite; 

        /* All io Action must update buffer addresses */
        for ( j = 0; j < iovMax; j++ )
        {
          ioVector[j].iov_base = (char *)pageVector[i+j];
        }

        /* All io Action must update buffer addresses */
        for ( j = 0; j < iovMax; j++ )
        {
            ioVector[j].iov_base = (char *)pageVector[i+j];
            ioVector[j].iov_len  = KGS->dev_blocksize;
        }

        for (; j < iovLimit; j++ )
        {
            ioVector[j].iov_base = 0;
            ioVector[j].iov_len  = 0;
        }

        iovWrite = iovMax;
        do
        {
            ret_code = RTE_save_writev ( fd, ioVector, iovWrite );

            if ( ret_code <= 0 )
            {
                char buffer[EN54_SMALL_MESSAGE_BUFFER_SIZE];
                sp77sprintf(buffer, sizeof(buffer), "(v) PNO %d Size %d : %s", pno + i, iovWrite, sqlerrs() );
                MSGD (( ERR_DEVIO_ERR_WRITE_FD, fd, &buffer[0] ));
                *errcode = DEVIO_ERR_SEWR;
                rc = false;
            } 
            else if ( ret_code != iovWrite * KGS->dev_blocksize )
            {
                if ( ret_code % KGS->dev_blocksize ) /* Oops, unexpected length! */
                {
                    char buffer[EN54_SMALL_MESSAGE_BUFFER_SIZE];
                    sp77sprintf(buffer, sizeof(buffer), "(v) PNO %d Size %d : Result %ld (%s)"
                        , pno + i, iovWrite, (long)ret_code, sqlerrs() );
                    MSGD (( ERR_DEVIO_ERR_WRITE_FD, fd, &buffer[0] ));
                    *errcode = DEVIO_ERR_SEWR;
                    rc = false;
                }
                else
                {
                    if ( 0 != ret_code )
                    {
                        SAPDB_Int k = ret_code / KGS->dev_blocksize;
                        retry = 0;

                        for ( j = 0; k < iovWrite; j++, k++ ) /* Shift buffer addresses */
                            ioVector[j].iov_base = ioVector[k].iov_base;

                        for ( ; j < iovWrite; j++ )
                        {
                            ioVector[j].iov_base = 0;
                            ioVector[j].iov_len  = 0;
                        }
                        iovWrite -= ret_code / KGS->dev_blocksize;
                    }
                    else
                    {
                        if ( NUMBER_OF_IO_RETRIES == retry++ )
                        {
                            char buffer[EN54_SMALL_MESSAGE_BUFFER_SIZE];
                            sp77sprintf(buffer, sizeof(buffer), "(v) PNO %d Size %d : Result %ld (%s)"
                                , pno + i, iovWrite, (long)ret_code, sqlerrs() );
                            MSGD (( ERR_DEVIO_ERR_WRITE_FD, fd, &buffer[0] ));
                            *errcode = DEVIO_ERR_SEWR;
                            rc = false;
                        }
                    }
                }
            } 
            else
            {
                break; /* all data written */
            }
        }
        while ( rc );
      }
    }

    if ( rc )
    {
        devi_pcb->old_io_endpos = offset + (off_t) pageCount * (off_t)KGS->dev_blocksize;
    }
    else
    {
        devi_pcb->old_io_endpos = -8;
    }

    return ( rc );
}
/* en54_DiskIOWriteVector */

/*
 * ==================================================================
 */

static  SAPDB_Int     en54_SimulateDiskIOWriteVector (
                        struct DEV_PCB         *devi_pcb,
                        SAPDB_Int                     fd,
                        tsp00_PageAddr          pageVector[],
                        SAPDB_Int4              pageCount,
                        SAPDB_Int4              pno,
                        SAPDB_UInt4            *errcode)
{
#undef MF__
#define MF__ MOD__"en54_SimulateDiskIOWriteVector"
    SAPDB_Int rc = true;
    off_t     offset;

    offset = ((off_t) pno + (off_t) 1) * (off_t) KGS->dev_blocksize ;

    /* Seek always if pno == -1 (RTE internal page0...) */
    if ( (offset != devi_pcb->old_io_endpos) || (-1 == pno) )
    {
        if ( offset != en54_SignalSave_lseek( fd, offset, 0 ) )
        {
            char buffer[EN54_SMALL_MESSAGE_BUFFER_SIZE];
            sp77sprintf(buffer, sizeof(buffer), "PNO %d : %s", pno, sqlerrs() );

            MSGD ((ERR_DEVIO_ERR_SEEK_FD, fd, &buffer[0] ));
            *errcode = DEVIO_ERR_SEWR;
            rc = false ;
        }
    }

    if ( rc )
    {
        SAPDB_Int     pagesWritten;
        long          ret_code;
        SAPDB_Int     i;

        pagesWritten = 0;
        for ( i = 0; rc && i < pageCount; i = pagesWritten )
        {
            SAPDB_Int j;
            SAPDB_Byte *firstPage;
            SAPDB_Long  bytesToWrite;
            
            firstPage = (SAPDB_Byte *)pageVector[i];
            bytesToWrite = KGS->dev_blocksize;

            /* All adjacent pages can be written in a single call */
            for ( j = 1; (i+j) < pageCount; j++ )
            {
#ifdef RTEDEBUG_SIMULATE_VECTORIO
                {
                    char buffer2[EN54_SMALL_MESSAGE_BUFFER_SIZE];
                    sp77sprintf(buffer2, sizeof(buffer2), "DEBUG...(Sim) PageVector[i+j (%d)] = %p",
                        i+j, (SAPDB_Byte *)pageVector[i+j] );
                    MSGD (( ERR_DEVIO_ERR_WRITE_FD, fd, &buffer2[0] ));
                }
#endif /* RTEDEBUG_SIMULATE_VECTORIO */
                if ( (firstPage + bytesToWrite) != (SAPDB_Byte *)pageVector[i+j] )
                {
                    break;
                }
                bytesToWrite += KGS->dev_blocksize;
            }

#ifdef RTEDEBUG_SIMULATE_VECTORIO
            {
                char buffer2[EN54_SMALL_MESSAGE_BUFFER_SIZE];
                sp77sprintf(buffer2, sizeof(buffer2), "DEBUG...(Sim) PNO %d Size %d ( [%d-%d[ of %d )"
                    , pno + i, bytesToWrite, i, i+j, pageCount );
                MSGD (( ERR_DEVIO_ERR_WRITE_FD, fd, &buffer2[0] ));
            }
#endif /* RTEDEBUG_SIMULATE_VECTORIO */

            ret_code = RTE_save_write ( fd, firstPage, bytesToWrite );
            if ( ret_code == bytesToWrite )
            {
                pagesWritten += j; /* Add all pages sucessfully written */
            }
            else if ( ret_code <= 0 )
            {
                char buffer[EN54_SMALL_MESSAGE_BUFFER_SIZE];
                sp77sprintf(buffer, sizeof(buffer), "(vSim) PNO %d Size %d : %s", pno + i, bytesToWrite, sqlerrs() );
                MSGD (( ERR_DEVIO_ERR_WRITE_FD, fd, &buffer[0] ));
                *errcode = DEVIO_ERR_SEWR;
                rc = false;
            }
            else if ( ret_code != bytesToWrite )
            {
                char buffer[EN54_SMALL_MESSAGE_BUFFER_SIZE];
                sp77sprintf(buffer, sizeof(buffer), "(vSim) PNO %d Size %d : Result %ld (%s)"
                       , pno + i, bytesToWrite, ret_code, sqlerrs() );
                MSGD (( ERR_DEVIO_ERR_WRITE_FD, fd, &buffer[0] ));
                *errcode = DEVIO_ERR_SEWR;
                rc = false;
            }
        }
    }

    if ( rc )
    {
        devi_pcb->old_io_endpos = offset + (off_t) pageCount * (off_t)KGS->dev_blocksize;
    }
    else
    {
        devi_pcb->old_io_endpos = -9;
    }

    return ( rc );
}
/* en54_SimulateDiskIOWriteVector */

/*
 * ==================================================================
 */

/*ARGSUSED*/
static SAPDB_Int en54_DevOpen (
                        char                   *fname,
                        tsp00_Bool              forceReadOnly,
                        INTFUNC_PTR            *io_f,
                        SAPDB_Int4             *fsize,
                        SAPDB_Int              *type)
/*
 *   Function to open any volume, now only implemented for direct-io
 *   since io on a file seems to use the same io-functions as io on an
 *   /dev/rdsk device.
 *   The parameter io_f is the address of an array of length 2, where
 *   the io-functions read and write for the device opened are returned.
 *   Fsize returns the size of the device that is to be opened.
 */
{
    #undef MF__
    #define MF__ MOD__"en54_DevOpen"
    SAPDB_Int fd;

    fd = RTE_save_open(fname, DEFAULT_OPEN_MODE_EN54 | en54_AddSynchronousOpen() | (forceReadOnly ? O_RDONLY : O_RDWR) );
    if ( fd < 0 )
    {
        * type = MOD_UNKNOWN;
    }
    else
    {
        switch ( e541_get_nodinfo(fname, fsize) )
        {
        case MOD_ORDIFILE:
            io_f[sp2ioc_read] =en54_DiskIORead;
            io_f[sp2ioc_write]=en54_DiskIOWrite;
            DBG1 (( MF__," ordinary file \n" ));
            * type = MOD_ORDIFILE;
            break;
        case MOD_BLCKSPEC:
        case MOD_CHARSPEC:
            io_f[sp2ioc_read] =en54_DiskIORead; /* Is the same as dio */
            io_f[sp2ioc_write]=en54_DiskIOWrite;/* at the moment      */
            DBG1 (( MF__," c/b special file \n" ));
            * type = MOD_BLCKSPEC;
            break;
        case MOD_UNKNOWN :
            io_f[sp2ioc_read]  = en54_WrongCall;
            io_f[sp2ioc_write] = en54_WrongCall;
             MSGD ((IERR_DEVOPEN_UNKNOWN_MODE, fname ));
            (void) RTE_save_close(fd);
            fd = -1;
            * type = MOD_UNKNOWN;
            break;
        default:
            io_f[sp2ioc_read]  = en54_WrongCall;
            io_f[sp2ioc_write] = en54_WrongCall;
             MSGD ((IERR_DEVOPEN_UNKNOWN_MODE, fname ));
            (void) RTE_save_close(fd);
            fd = -1;
            * type = MOD_UNKNOWN;
            break;
        }
    }
    DBGOUT;
    return ( fd );
}/* en54_DevOpen */

/*
 * ====================================================================
 */

static  struct DOUBLY_LINKED    *en54_dev0Receive (void)
{
    #undef MF__
    #define MF__ MOD__"en54_dev0Receive"
    SAPDB_Int                                    rc;
    struct             DOUBLY_LINKED       *t_req;

    do {
      /*
       *   Tries to get a request from the queue of the pcb, if no request
       *   is available then this proc should go to sleep until it is waked
       *   by a task/proc making a IO-request to it.
       */

      DBG1 (( MF__,"immed. ALONE \n" ));

/* PTS 1105262 / PTS 1105263 */
      WAIT_UNTIL_ALONE ( KGS->dev0.alone );

      t_req = KGS->dev0.io_work->request;

      DBG1 (( MF__,"found request 0x%lx \n", (long) t_req ));

      if ( t_req != NULL )
      {
        KGS->dev0.io_work->request = 0;
        KGS->dev0.io_work          = KGS->dev0.io_work->next;
      }

      CLEARLOCK ( KGS->dev0.alone );

      /*
       * If a request was found we can work on it
       */
      if ( t_req != NULL )
      {
        DBG1 (( MF__,"immed. return 0x%lx from T%d\n", t_req, t_req->taskid->index ));
        KGS->dev0.state            = KT_RUNNING;
      }
      else
      {
        /* PTS 1107963 Do no longer react on SERVER_STOP/KILL */
        KGS->dev0.state = KT_SLEEPING ;

        DBG1 (( MF__,"DEV0 will sleep on semid %d \n", KGS->dev0.semid ));

        /*
         *  If another process at this point does a e72_wake,
         *  then this process now has state KT_RUNNING, and will return
         *  immediately from its sleep
         */
        if ( !KGS->dev0.fSemAlreadyPosted )
        {
           struct sembuf own_semrcv ;
           SAPDB_memcpy( &own_semrcv , &semrcv , sizeof(own_semrcv) );
           rc = RTE_save_semop ( KGS->dev0.semid, & own_semrcv, 1 ); /* wait until wakened */
           if ( rc == -1 )
           {
               MSGD ((IERR_DEV0_RECEIVE_ERR_SEMOP, sqlerrs() ));
               vabort(WRITE_CORE);
           }
        }

        KGS->dev0.fSemAlreadyPosted = 0;
      }

    } while ( t_req == NULL );

    DBG1 (( MF__,"returning req 0x%lx from T%d\n",
                (long) t_req , t_req->taskid->index ));

    return ( t_req );
} /*en54_dev0Receive*/

/* =======================================================================
  Function: en54_FindDevpcb
  Description: Find volume control block for non-asynchron devices

  Three parameter defines the number of devi per volume

  XPARAM(lDeviPerDevspace) _IOPROC_PER_DEV define the base number of parallel worker threads
  XPARAM(lPrioDeviPerDevspace) _IOPROCS_FOR_PRIO define an additional number used only by
    tasks with priority
  XPARAM(lReaderDeviPerDevspace) _IOPROCS_FOR_READER define an additional number used only by
    task for read I/O or task with priority

  If a task has priority, it firsts looks through the normal devi, then through the reader devi
  and finally through the priority devi's. If none of these has no entry, it uses the least filled slot.

  If a task wants to read, it looks through the normal and the reader devi for the least filled slot.
  If the least filled slot is empty .
  If the least filled slot 
  

  Arguments: this_ukt   [in] task control block
             devno      [in] the volume number
             tsp00_Bool [in] prio
  Return value: pointer to volume decriptor
 */
static struct DEV_PCB * en54_FindDevpcb (
                        ten50_UKT_Control    * this_ukt ,
                        SAPDB_Int4             devno,
                        tsp00_Bool             prio,
                        SAPDB_Bool             reader)
{
    #undef MF__
    #define MF__ MOD__"en54_FindDevpcb"
    REGISTER  struct DEV_PCB  *d;
    REGISTER  struct DEV_PCB  *d0;
    REGISTER  SAPDB_Int             slot;
    SAPDB_Int                       ActDevQueLen;
    SAPDB_Int                       min_queue_len;
    SAPDB_Int                       min_filled_slot;
    SAPDB_Int                       max_queu_len ;
    SAPDB_Int                       last_slot;

    /*
     *  Find the least filled DEVi thread
     */

    if ( devno == 0 )
    {
        min_filled_slot = 0;
        d0 = KGS->first_dev;
    }
    else
    {
        d0 = KGS->first_dev + devno * KGS->devpsets;

        if ( prio ) // task has priority and will look for an empty slot or the least filled
        {
            min_filled_slot = 0;
            min_queue_len = d0->DevIOStat.ulIOQueLen ;

            if ( 0 != min_queue_len ) // if first slot is empty, nothing to do...
            {
                // search for an empty slot
                last_slot = KGS->devpsets;
                for ( slot = 1; slot < last_slot ; ++slot )
                {
                    d = d0 + slot;
                    if ( 0 == d->DevIOStat.ulIOQueLen )
                    {
                        DBG1 (( MF__,"found sleeping devp 0x%p", d ));
                        min_filled_slot = slot ;
                        break;
                    }
                    else
                    {
                        if ( d->DevIOStat.ulIOQueLen < min_queue_len )
                        {
                            min_filled_slot = slot;
                            min_queue_len = d->DevIOStat.ulIOQueLen ;
                        }
                    }
                }
            }
        }

        else // non priorized IO request handling
        {

            if ( reader && (XPARAM(lReaderDeviPerDevspace) > 0) ) // read request with special slots enabled */
            {
                SAPDB_Int foundSlot = false ;

                min_queue_len = (d0+XPARAM(lDeviPerDevspace))->DevIOStat.ulIOQueLen ;
                min_filled_slot = XPARAM(lDeviPerDevspace);
                max_queu_len  = KGS->dev_queue ;

                if ( min_queue_len <= max_queu_len )
                {
                    foundSlot = true ;
                }
                else
                {
                    // search for first reader slot with less than dev_queue elements
                    last_slot = (XPARAM(lDeviPerDevspace) + XPARAM(lReaderDeviPerDevspace));

                    for ( slot = XPARAM(lDeviPerDevspace) + 1; slot < last_slot ; ++slot )
                    {
                        d = d0 + slot;
                        if ( d->DevIOStat.ulIOQueLen <= max_queu_len )
                        {
                            foundSlot = true ;
                            min_filled_slot = slot;
                            break;
                        }

                        if ( d->DevIOStat.ulIOQueLen < min_queue_len )
                        {
                            min_filled_slot = slot;
                            min_queue_len = d->DevIOStat.ulIOQueLen ;
                        }
                    }
                }

                if ( !foundSlot ) // found no read slot
                {
                    // only search through 'normal' slots in reverse order if reader slots are all filled beyond 'dev_queue'
                    // reason: in writer slots large blockio (vector I/O happens, which is slower than single page I/O...
                    last_slot = XPARAM(lDeviPerDevspace);
                    for ( slot = last_slot - 1; slot >= 0; --slot ) // prefer additional I/O worker...
                    {
                        d = d0 + slot;
                        if ( d->DevIOStat.ulIOQueLen <= max_queu_len )
                        {
                            min_filled_slot = slot;
                            break;
                        }
                        // min_queu_len not taken into account! writer_slots can have 'large I/O jobs...
                    }
                }
            }
            else // non priorized writer handling or no special read requests
            {
                min_queue_len = d0->DevIOStat.ulIOQueLen ;
                min_filled_slot = 0;
                max_queu_len  = KGS->dev_queue ;
                if ( min_queue_len > max_queu_len )
                {
                    // search for a 'normal' slot with less than dev_queue elements
                    last_slot = XPARAM(lDeviPerDevspace);
                    for ( slot = 1; slot < last_slot ; ++slot )
                    {
                        d = d0 + slot;
                        if ( d->DevIOStat.ulIOQueLen <= max_queu_len )
                        {
                            min_filled_slot = slot ;
                            break;
                        }
                        else
                        {
                            if ( d->DevIOStat.ulIOQueLen < min_queue_len )
                            {
                                min_queue_len = d->DevIOStat.ulIOQueLen ;
                                min_filled_slot = slot;
                            }
                        }
                    }
                }
            }
        }
    }

    d = d0 + min_filled_slot;
    DBG1 (( MF__,"using slot          %d", min_filled_slot ));
    ActDevQueLen = d->DevIOStat.ulIOQueLen ;
    (this_ukt->curr_task->TaskStateStat.ulDevQueueLen
                [ ActDevQueLen>=MAX_DEVQUEUELEN_STAT_EO52 ?
                  MAX_DEVQUEUELEN_STAT_EO52-1 : ActDevQueLen ]) ++ ;
    return (d);
}

/* =======================================================================
  Function: en54_FindAsyncDevpcb
  Description: Find volume control block for asynchron devices

  Arguments: devno [in] the volume number
  Return value: pointer to volume decriptor
 */
static struct DEV_PCB * en54_FindAsyncDevpcb ( SAPDB_Int4                devno)
{
    #undef MF__
    #define MF__ MOD__"en54_FindAsyncDevpcb"
    return ( KGS->first_asdev + ( devno - KGS->maxdevspaces ) ) ;
}

/* =======================================================================
  @brief Write Page0 and format volume if wanted

  @param volume [in] the volume name
  @param givenBlockSize [in] data page size in bytes
  @param devcapacity [inout] volume capacity in pages (updated if 0)
  @param wantFormatDevspace [in] flag indicating format devspace is wanted
  @param taskResponsibleForCall [in] task id of initiator
  @param diskImage [in] data used for formatting volume if non NULL
  @param errtext [out] error text if error occured
  @param ok [out]set to true on success, false on error
 */

static void en54_InitializeDevspace( SAPDB_Int fd,
                                     SAPDB_Int4 devcapacity,
                                     SAPDB_Int4 givenBlockSize,
                                     tsp00_Bool wantFormatDevspace,
                                     tsp00_TaskId taskResponsibleForCall,
                                     char      *diskImage,
                                     tsp00_ErrText & errtext,
                                     tsp00_Bool    *ok)
{
#undef MF__
#define MF__ MOD__"en54_InitializeDevspace"

    /* if wanted format volume now */
    if ( wantFormatDevspace )
    {
        en54_FormatDevspace ( fd, devcapacity+1, givenBlockSize, taskResponsibleForCall, diskImage, errtext, ok);
    }

    if ( *ok == true ) /* PTS 1111950 */
    {
        /* Fill first page with RTE specific information */
        if (( NO_OFFSET == en54_SignalSave_lseek(fd, NO_OFFSET, ABS_OFFSET)) &&
            ( OK == en54_Page0IO (fd, sp2ioc_write, &givenBlockSize, devcapacity, true, (char*)0) ) )
        {
            *ok = true;
        }
        else
        {
            *ok = false;
            errtext.c2p( "Cannot seek/fill block 0 of volume" );
            MSGD(( ERR_VDEVSIZE_IOERROR, "Cannot seek/fill block 0 of volume", sqlerrs() ));
            DBG1 (( MF__,"Could not seek/write, %s\n", sqlerrs() ));
        }
    }
}

/* =======================================================================
  @brief If file should be opened read only, validate it is already initialized

  If file size is to small to hold a page0, the read only flag is reset if neccessary

  @param volume [in] the volume name
  @param givenBlockSize [in] data page size in bytes
  @param pForceReadOnly [inout] pointer to flag indicating read only access is wanted
 */
static void en54_ValidateReadOnlyVolumeSize( tsp2_devnamec volume,
                                             SAPDB_Int4    givenBlockSize,
                                             tsp00_Bool   *pForceReadOnly )
{
#undef MF__
#define MF__ MOD__"en54_ValidateReadOnlyVolumeSize"
    struct stat stat_buf ;

    if ( ( -1 != RTE_save_stat(volume,&stat_buf) )
      && ( (long) stat_buf.st_size <= (long) givenBlockSize ) )
    {
        ResetForcedReadOnlyIfHSDefaultMaster(pForceReadOnly);
    }
    return;
}

/* =======================================================================
  @brief Calculate volume capacity for regular file

  @param volume [in] the volume name
  @param givenBlockSize [in] data page size in bytes
  @param devcapacity [inout] volume capacity in pages (updated if 0)
  @param taskResponsibleForCall [in] task id of initiator
  @param diskImage [in] data used for formatting volume if non NULL
  @param isSequential [inout] flag indicating sequential access is wanted
  @param forceReadOnly [inout] flag indicating read only access is wanted
  @param errtext [out] error text if error occured
  @param ok [out]set to true on success, false on error
 */
static  void    en54_RegularFileVdevsize (
                        tsp2_devnamec           volume,
                        SAPDB_Int4                givenBlockSize,
                        SAPDB_Int4               *devcapacity,
                        tsp00_TaskId              taskResponsibleForCall,
                        char                     *diskImage,
                        SAPDB_Int                 isSequential,
                        tsp00_Bool                forceReadOnly,
                        tsp00_ErrText           & errtext,
                        tsp00_Bool                *ok)
/*
 * Vdevsize for a device being a unix file for direct io.
 */
{
    #undef MF__
    #define MF__ MOD__"en54_RegularFileVdevsize"
    SAPDB_Int                         rc ;
    SAPDB_Int                         fd ;
    off_t                       devcap ;
    struct stat                 stat_buf ;
    bool                        wantFormatDevspace = (diskImage != 0);

    DBG1 (( MF__,"called for volume '%s' \n", volume ));
    DBG1 (( MF__,"called with givenBlockSize %d, capacity = %d\n",
                          givenBlockSize , *devcapacity ));

    if ( 0 != *devcapacity )
    {
        off_t DevCapOverflowCheck ;

        DBG1 (( MF__,"create '%s' \n", volume ));
        devcap = ((off_t) *devcapacity + (off_t) 1) * (off_t) givenBlockSize ;
        DevCapOverflowCheck = devcap / (off_t) givenBlockSize ;
        if ( (SAPDB_Int4 )DevCapOverflowCheck != *devcapacity + 1 ) /* PTS 1105902 */
        {
            MSGD ((ERR_DIOSIZE_ERR_SIZE2, *devcapacity ));
            errtext.c2p( "Illegal device size" );
            *ok = false ;
            return ;
        }
    }

/* PTS 1106785 */
    if (*devcapacity == 0 || forceReadOnly )
    {
        fd = RTE_save_open((const char *)volume,DEFAULT_OPEN_MODE_EN54 | (forceReadOnly ? O_RDONLY : O_RDWR) );
    }
    else
    {
#if !defined(BIT64) && defined(O_LARGEFILE)
        fd = RTE_save_openCreate ( (const char *)volume, O_LARGEFILE | O_RDWR | O_CREAT, 0660 );
#else
        fd = RTE_save_openCreate ( (const char *)volume, O_RDWR | O_CREAT, 0660 );
#endif
    }
    if (fd == -1)
    {
        MSGD ((ERR_DIOSIZE_ERR_OPEN, sqlerrs() ));
        errtext.c2p( "Cannot create volume" );
        DBG1 (( MF__,"Error during create, %s \n", sqlerrs() ));
        *ok = false;
    }

    if (*devcapacity == 0 || forceReadOnly )
    {
        if ( RTE_save_fstat(fd,&stat_buf) == -1 )
        {
            *ok = false;
            errtext.c2p( "Error during fstat" );
            MSGD ((ERR_DIOSIZE_ERR_FSTAT, sqlerrs() ));
        }
        else if ( (long) stat_buf.st_size <= (long) givenBlockSize )
        {
            *ok = false;
            errtext.c2p( "Illegal volume size" );
            MSGD ((ERR_DIOSIZE_ERR_SIZE, (long) stat_buf.st_size ));
        }
        else
        {
            if ( forceReadOnly )
            {
                if ( (*devcapacity) != (stat_buf.st_size/givenBlockSize - 1) )
                {
                    *ok = false;
                    errtext.c2p( "Wrong size for read only volume" );
                    MSGD ((ERR_DIOSIZE_ERR_SIZE, (long) stat_buf.st_size ));
                }
            }
            else
            {
                *ok = true;
                *devcapacity = stat_buf.st_size/givenBlockSize - 1;
            }
        }
    }
    else
    {
        *ok = true;
        if ( !isSequential )
        {
            off_t endOffset = en54_SignalSave_lseek ( fd , (off_t) 0, SEEK_END );
            if ( endOffset > devcap )
            {
                rc = ftruncate( fd, devcap );
                if ( 0 != rc )
                {
                    MSGD (( ERR_NEWDEVSIZE_ERR_TRUNCATE, sqlerrs()));
                    errtext.c2p( "Cannot truncate volume" );
                    *ok = false;
                }
            }

            if ( true == *ok && !wantFormatDevspace )
            {
                /* en54_NewDevsize is only needed if no formatting of volume is enforced */
                if ( XPARAM(fFormatDataVolume) )
                {
                    wantFormatDevspace = true;
                    *ok = true;
                }
                else
                {
                    en54_NewDevsize ( fd, true, *devcapacity, givenBlockSize, errtext, ok);
                    if ( false == *ok )
                    {
                        MSGD(( ERR_VDEVSIZE_IOERROR, (const char *)volume, &errtext[0]));
                    }
                }
            }
        }
    }

    if ( (*ok == true) && !forceReadOnly )
    {
        en54_InitializeDevspace(fd,
                                *devcapacity,
                                givenBlockSize,
                                ((!isSequential) && wantFormatDevspace),
                                taskResponsibleForCall,
                                diskImage,
                                errtext,
                                ok);
        if ( *ok == false )
        {
            MSGD(( ERR_VDEVSIZE_IOERROR, (const char *)volume, "Intialization failed"));
            RTE_save_ftruncate(fd, 0); /* free space on filesystem */
        }
        (void) RTE_save_close(fd);
    }
    else if ( fd != -1 )
    {
        (void) RTE_save_close(fd);
    }
    return;
} /* en54_RegularFileVdevsize */

/* =======================================================================
  Function: en54_RawDiskVdevsize
  Description: Calculate volume capacity for raw disk

  Arguments: volume [in] the volume name
             givenBlockSize [in] data page size in bytes
             devcapacity [inout] volume capacity in pages (updated if 0)
             taskResponsibleForCall [in] task id of initiator
             diskImage [in] data used for formatting volume if non NULL
             errtext [out] error text if error occured
             ok [out]set to true on success, false on error
 */
static  void    en54_RawDiskVdevsize (
                        tsp2_devnamec             volume,
                        SAPDB_Int4                givenBlockSize,
                        SAPDB_Int4               *devcapacity,
                        tsp00_TaskId              taskResponsibleForCall,
                        char                     *diskImage,
                        SAPDB_Int                 isSequential,
                        tsp00_Bool                forceReadOnly,
                        tsp00_ErrText           & errtext,
                        tsp00_Bool                *ok)
/*
 * Is called if volume is a raw device, if devcapacity
 * is 0 the capacity of the raw device will be returned, if devcapacity
 * is larger than the capacity of the raw device then ok = false.
 */
{
    #undef MF__
    #define MF__ MOD__"en54_RawDiskVdevsize"
    SAPDB_Int         rc ;
    SAPDB_Int         fd ;
    struct stat stat_buf ;
    bool        wantFormatDevspace = (( diskImage != 0 ) && !forceReadOnly);

    DBG1 (( MF__,"called for volume '%s' \n", volume ));
    DBG1 (( MF__,"called with givenBlockSize %d, capacity = %d\n",
                          givenBlockSize , *devcapacity ));
    *ok = false;

    fd = RTE_save_open( (const char *)volume, DEFAULT_OPEN_MODE_EN54 | (forceReadOnly ? O_RDONLY : O_RDWR) );
    if ( fd == -1 )
    {
        errtext.c2p( "Cannot open volume" );
        MSGD ((ERR_RIOSIZE_ERR_OPEN, &volume[0], sqlerrs() ));
    }
    else
    {
        if ( rc = RTE_save_fstat ( fd, &stat_buf ), rc != -1)
        {
            if ( *devcapacity == 0 )
            {
                if ( ( *devcapacity =
                       e541_get_devsize0 ( fd, givenBlockSize ) - 1 ) <= 1 )
                {
                    errtext.c2p( "illegal devsize on volume" );
                    MSGD ((ERR_RIOSIZE_ERR_SIZE, *devcapacity ));
                }
                else
                {
                     MSGD ((INFO_RIOSIZE_CALCULATED_SIZE, *devcapacity ));
                    *ok = true;
                }
            }
            else if ( !isSequential )
            {
                if ( !wantFormatDevspace )
                {
                    en54_NewDevsize ( fd, false, *devcapacity, givenBlockSize, errtext, ok );
                    if ( false == *ok )
                    {
                        MSGD(( ERR_VDEVSIZE_IOERROR, (const char *)volume, &errtext[0]));
                    }
                }
                else
                {
                    *ok = true;
                }
            }
            else
            {
                *ok = true;
            }
        }
        else
        {
            errtext.c2p( "Error during fstat" );
            MSGD ((ERR_RIOSIZE_ERR_FSTAT, sqlerrs() ));
        }
    }

    if ( (*ok == true) && !forceReadOnly )
    {
        en54_InitializeDevspace( fd,
                                 *devcapacity,
                                 givenBlockSize,
                                 ((!isSequential) && wantFormatDevspace),
                                 taskResponsibleForCall,
                                 diskImage,
                                 errtext,
                                 ok);
    }

    (void) RTE_save_close(fd);
    DBG1 (( MF__,"returning ok = %d\n", *ok ));
    return;
}/* en54_RawDiskVdevsize */

/* =======================================================================
  Function: en54_NewDevsize
  Description: Verify new volume size

  Verification is done by a read/write cycle on the last page in the volume

  Arguments: fd [in] the already opended volume
             devsp_is_file [in] flag to indicate volume is file
             devcapacity [in] volume capacity in pages
             givenBlockSize [in] data page size in bytes
             errtext [out] error text if error occured
             ok [out]set to true on success, false on error
 */
static void en54_NewDevsize (
                        SAPDB_Int4                fd,
                        tsp00_Bool                devsp_is_file,
                        SAPDB_Int4                devcapacity,
                        SAPDB_Int4                givenBlockSize,
                        tsp00_ErrText           & errtext,
                        tsp00_Bool                *ok)
{
    #undef MF__
    #define MF__ MOD__"en54_NewDevsize"
    off_t                       offseek ;
    off_t                       offreturn ;

     /*
     *  The device capacity needs to be one givenBlockSize larger than
     *  devcapacity for RTE purposes.
     *  We don't add the block here since we want to seek to the
     *  beginning of the last block, not to the end of the device.
     */
    offseek = (off_t) devcapacity * (off_t) givenBlockSize ;
    *ok = true ;

    /*
     *  off_t might be:
     *        signed long
     *      unsigned long
     *        signed long long
     *      unsigned long long
     *  The formula "offseek=devcapacity*givenBlockSize" may overrun
     *  what off_t can hold. If that happens, the result is either negative,
     *  or one or more bits are lost. Computing the formula reversely will
     *  reveal the overrun in any case. A negative result will stay negative
     *  and lost bits cannot be restored.
     */
    if ( offseek/(off_t)givenBlockSize != (off_t)devcapacity )
    {
        MSGD ((ERR_NEWDEVSIZE_INVALID_SIZE, (long) devcapacity ));
        errtext.c2p( "Illegal device size" );
        *ok = false ;
        return ;
    }

    offreturn = en54_SignalSave_lseek ( fd , offseek , 0 );
    if ( offreturn != (off_t) -1 )
    {
        SAPDB_Int             rc ;
        struct ALIGNED_MEM bufMem;
        unsigned char *buf;

        if ( (buf = en54_AllocAligned( &bufMem, givenBlockSize, NULL )) == NULL )
        {
            MSGD ((ERR_NEWDEVSIZE_ERR_MALLOC, sqlerrs() ));
            vabort ( WRITE_CORE );
        }
        DBG1 (( MF__,"try to read last page to addr 0x%x\n",buf ));
        rc = RTE_save_read ( fd, buf, (unsigned) givenBlockSize );
        DBG1 (( MF__,"read returned %d \n", rc ));
        if ( rc == -1 )
        {
            errtext.c2p( "Could not read from volume" );
            *ok = false;
        }
        else
        {
            if ( devsp_is_file || rc == (SAPDB_Int) givenBlockSize )
            {
                if ( en54_SignalSave_lseek(fd, offseek , 0) == offseek )
                {
                    rc = en54_SignalSave_write( fd, buf, (unsigned) givenBlockSize );
                    if ( rc != (SAPDB_Int) givenBlockSize )
                    {
                        if ( rc >= 0 )
                        {
                            errtext.c2p( "File system full" );
                        }
                        else
                        {
                            errtext.c2p( "Cannot write on volume" );
                        }
                        *ok = false;
                    }
                }
                else
                {
                    errtext.c2p( "Could not  seek" );
                    *ok = false;
                }
            }
            else
            {
                errtext.c2p( "Volume too small" );
                *ok = false ;
            }
        }
        en54_FreeAligned( &bufMem );
    }
    else
    {
        errtext.c2p( "Error during seek/write on volume" );
        *ok = false ;
    }
}

/* =======================================================================
  Function: en54_FormatDevspace
  Description: Format a volume

  The given diskImage (or a page of zerobytes) is replicated in a buffer of
  Data_IO_Block_Count. If diskImage is not given, a seek to end of file is done
  (only expansion of file, not defined pattern needed...) otherwise formatting starts
  at first page of file (skipping RTE Page_0).

  Arguments: fd [in] Open file handle for volume
             devcapacity [in] Capacity of volume in pages
             givenBlockSize [in] size of data page
             diskImage [in] pointer to image (may be NULL if only expansion is needed)
             errtext [out] filled with error message on error
             ok [out] set to false on error, true if succeeded
 */
static void en54_FormatDevspace (
                        SAPDB_Int4                fd,
                        SAPDB_Int4                devcapacity,
                        SAPDB_Int4                givenBlockSize,
                        tsp00_TaskId              taskResponsibleForCall,
                        char                      *diskImage,
                        tsp00_ErrText           & errtext,
                        tsp00_Bool                *ok)
{
#undef MF__
#define MF__ MOD__"en54_FormatDevspace"
    off_t              offreturn ;
    unsigned char     *buf;
    struct ALIGNED_MEM bufMem;
    SAPDB_Int                ActBlockCount = XPARAM(ulDataIOBlockCount); // now DATA_IO_BLOCK_COUNT !

    if ( 0 == ActBlockCount ) ActBlockCount = 1;

    SAPDB_Int                BufSize = givenBlockSize * ActBlockCount;
    struct TASK_TYPE  *tcb;

    DBGIN ;

    /* PID_TCB macro not usefull here, since formatting used inside worker thread too... */
#if (COMPILEMODE_MEO00 >= SLOW_MEO00) || defined(DEVELOP_CHECKS)
    if ( taskResponsibleForCall < 1 || taskResponsibleForCall > kgs->ulNumOfTasks )
    {
      MSGD (( ERR_KERN_PARAM_PID_OUT_OF_RANGE, taskResponsibleForCall ));
      vabort ( WRITE_CORE );
    }
#endif
    tcb = kgs->pFirstTaskCtrl + taskResponsibleForCall - 1;

    /* Allocate Buffer for I/O Operation (properly system page aligned) */
    if ( (buf = en54_AllocAligned( &bufMem, BufSize, NULL)) == NULL )
    {
        MSGD ((ERR_NEWDEVSIZE_ERR_MALLOC, sqlerrs() ));
        errtext.c2p( "Error during seek on volume" );
        *ok = false ;
        return;
    }

    /* Prepare diskImage and seek to start position */
    if ( diskImage )
    {
        SAPDB_Int iBlock;
        for ( iBlock = 0; iBlock < ActBlockCount; iBlock++ )
        {
            SAPDB_memcpy ( buf + (iBlock * givenBlockSize), diskImage, givenBlockSize ) ;
        }
        offreturn = en54_SignalSave_lseek ( fd , (off_t) givenBlockSize, SEEK_SET );
    }
    else
    {
        SAPDB_memset ( buf, 0, BufSize ) ;
        offreturn = en54_SignalSave_lseek ( fd , (off_t) 0, SEEK_END );
    }

    DBG3 (( MF__, "lseek return %ld \n", (long) offreturn ));

    if ( offreturn != (off_t) -1 )
    {
        SAPDB_Int4      blocksWritten = 0;
        SAPDB_Int       BytesWritten = 0;
        SAPDB_Int       ActPage = offreturn / givenBlockSize ;
        time_t          startTime = 0;
        int             ioCountDown = 0;
        DBG3 (( MF__,"starting with page %ld \n", ActPage ));

        if ( devcapacity > ActPage )
        {
            MSGD ((INFO_FORMATTING_VOLUME_START, (long)devcapacity - ActPage ));
        }
        else
        {
            MSGD ((INFO_FORMATTING_VOLUME_START, (long)0 ));
        }

        /* Formatting loop */
        for ( *ok = true, ActPage = offreturn / givenBlockSize ;
              *ok && (ActPage + ActBlockCount) <= devcapacity;
              ActPage += ActBlockCount )
        {
            if ( ioCountDown == 0 )
            {
                time_t  currentTime = KGS->current_time;
                if ( startTime == 0 || (currentTime - startTime) >= 5 )
                {
                    startTime = currentTime;
                    ioCountDown = 100;
                    MSGD ((INFO_FORMATTED_PAGES_LEFT, (long)devcapacity - (long)ActPage ));
                }
                else
                {
                    ioCountDown = 10;
                }
            }
            else
            {
                --ioCountDown;
            }

            BytesWritten = en54_SignalSave_write( fd, buf, (unsigned) BufSize ) ;
            if ( BytesWritten != BufSize )
            {
                if ( BytesWritten >= 0 )
                {
                    errtext.c2p( "File system full" );
                }
                else
                {
                    errtext.c2p( "Cannot write on volume" );
                }
                *ok = false; /* PTS 1122960 */
                break; /* PTS 1111950 */
            }
            DBG3 (( MF__,"write returned %d", BytesWritten ));
/*
  To reduce the number of collisions between parallel working formatting threads, the statistic
  update (which is needed to have at least something monitorable...) is not done for each block
  but after the given number of blocks
 */
            ++blocksWritten;
#define FORMAT_IO_STAT_BLOCKS_WRITTEN_LIMIT_EN54 16
            if ( blocksWritten >= FORMAT_IO_STAT_BLOCKS_WRITTEN_LIMIT_EN54 )
            {
                WAIT_UNTIL_ALONE( tcb->exclusive );
                    tcb->TaskStateStat.DevIOWriteStat.IOStat.TotalTimeStat.ulCount += blocksWritten;
                    tcb->TaskStateStat.DevIOWriteStat.TotalPageCount += (ActBlockCount * blocksWritten);
                    tcb->TaskStateStat.DevIOWriteStat.IOStat.TimeStat.ulCount += blocksWritten;
                    tcb->TaskStateStat.DevIOWriteStat.ulPageCount += (ActBlockCount * blocksWritten);
                CLEARLOCK( tcb->exclusive );
                blocksWritten = 0;
            }
        }

        if ( blocksWritten > 0 )
        {
            WAIT_UNTIL_ALONE( tcb->exclusive );
                tcb->TaskStateStat.DevIOWriteStat.IOStat.TotalTimeStat.ulCount += blocksWritten;
                tcb->TaskStateStat.DevIOWriteStat.TotalPageCount += (ActBlockCount * blocksWritten);
                tcb->TaskStateStat.DevIOWriteStat.IOStat.TimeStat.ulCount += blocksWritten;
                tcb->TaskStateStat.DevIOWriteStat.ulPageCount += (ActBlockCount * blocksWritten);
            CLEARLOCK( tcb->exclusive );
        }

        if ( ActPage < devcapacity
          && ((ActPage + ActBlockCount) > devcapacity) )
        {
            BufSize       = (devcapacity   - ActPage) * givenBlockSize ;
            BytesWritten = en54_SignalSave_write( fd, buf, (unsigned) BufSize ) ;
            if ( BytesWritten != BufSize )
            {
                MSGD (( ERR_NEWDEVSIZE_ERR_WRITE, sqlerrs() ));
                errtext.c2p( "Cannot write on volume" );
                *ok = false;
            }
            DBG3 (( MF__,"write returned %d", BytesWritten ));

            WAIT_UNTIL_ALONE( tcb->exclusive );
                eo52UpdateCountInIOStatisticRec( tcb->TaskStateStat.DevIOWriteStat,
                                                 devcapacity - ActPage );
            CLEARLOCK( tcb->exclusive );
        }

        if ( *ok != false )
        {
            MSGD ((INFO_FORMATTING_VOLUME_DONE, (long)devcapacity ));
        }
    }
    else
    {
        MSGD ((ERR_NEWDEVSIZE_ERR_SEEK, sqlerrs() ));
        errtext.c2p( "Error during seek on volume" );
        *ok = false ;
    }

    en54_FreeAligned( &bufMem );

    DBGOUT ;
}

/*
 * ==================================================================
 */

static  void    en54_CloseSingleFilescs (
ten50_UKT_Control               * this_ukt )
{
    #undef MF__
    #define MF__ MOD__"en54_CloseSingleFilescs"
    SAPDB_Int                         ix ;
    SAPDB_Int                         ex ;

    DBGIN;

    ex = KGS->maxdevspaces ;
    for ( ix = 0 ; ix < ex ; ix ++ )
    {
        en54_CloseSingleIOFileDesc(this_ukt, ix);
    }
    DBGOUT;
}
/*
 * ==================================================================
 */

    #undef MF__
    #define MF__ MOD__"e54_init_self_io"
/*ARGSUSED*/
extern "C" SAPDB_Int e54_init_self_io (
                                   ten50_UKT_Control      * this_ukt,
                                   SAPDB_Int4               devno,
                                   tsp00_PageNo             devpno,
                                   tsp2_io_op_code_Param    op_code,
                                   SAPDB_Int              & fd,
                                   tsp00_ErrText          & errtext )
{
    struct DEV_PCB  *devpcb ;

    DBGIN ;

    DBG3 (( MF__, "this_ukt->pSingleIO[%d].ukp_attached_fd %d\n",
                   devno, this_ukt->pSingleIO[devno].ukp_attached_fd ));

    if ( this_ukt->pSingleIO[devno].ukp_attached_fd == -1 )
    {
        tsp00_Bool forceReadOnly;

        if ( KGS->dev0.dev[devno].devspace[0] == '\0' )
        {
            MSGD ((IERR_SINGLEIO_INVALID_DEVNO, devno ));
            errtext.c2p( "Invalid devno" );
            DBGOUT ;
            return ( VB_NOTOK );
        }
        DBG3 ((MF__, "single I/O attach '%s'",
                        KGS->dev0.dev[devno].devspace ));

        forceReadOnly = ( sp2dt_log == KGS->dev0.dev[devno].devtype
                        || sp2dt_mirr_log == KGS->dev0.dev[devno].devtype )
                        && RTEHSS_IsLogReadOnly();

        if ( ( this_ukt->pSingleIO[devno].ukp_attached_fd =
                  RTE_save_open( (const char *)KGS->dev0.dev[devno].devspace,
                                 DEFAULT_OPEN_MODE_EN54
                                  | en54_AddSynchronousOpen()
                                  | (forceReadOnly ? O_RDONLY : O_RDWR) )
                ) == -1 )
        {
            if ( errno == ENFILE || errno == EMFILE )
            {
                MSGD ((ERR_SINGLEIO_FILE_LIMIT, sqlerrs() ));
                en54_CloseSingleFilescs ( this_ukt );
                this_ukt->allow_single_io = false;
                DBGOUT ;
                return ( VB_NO_MORE_FILES );
            }

            MSGD ((ERR_SINGLEIO_ERR_OPEN, devno , sqlerrs() ));
            errtext.c2p( sqlerrs() );
            DBGOUT ;
            return ( VB_NOTOK );
        }
        DBG3 (( MF__, "attached to fd %d",
                       this_ukt->pSingleIO[devno].ukp_attached_fd ));
    }

    /*
     *  Simulate an I/O error for test porposes.
     */
    devpcb = KGS->first_dev + devno * KGS->devpsets ;
    if ( devpcb->set_io_error &&
         ((devpcb->set_io_error & ~0x100) == op_code) )
    {
        devpcb->set_io_error = 0 ;
        MSGD ((IERR_SINGLEIO_TEST_IO_ERROR,
                (op_code == sp2ioc_read) ? "reading from" : "writing to" ,
                (long) devno , (long) devpno ));
        errtext.c2p( errtab[DEVIO_ERR_USER_REQ] );
        DBGOUT ;
        return ( VB_NOTOK );
    }

  fd = this_ukt->pSingleIO[devno].ukp_attached_fd ;
  DBGOUT ;
  return ( VB_OK ) ;
}

/*
 * ==================================================================
 */

static SAPDB_Int en54_SingleBlockIO (ten50_UKT_Control       * this_ukt,
                                     SAPDB_Int4                devno,
                                     tsp00_PageNo              devpno,
                                     tsp2_io_op_code_Param     op_code,
                                     tsp00_PageAddr            bufadr,
                                     SAPDB_Int2                buf_count,
                                     tsp00_ErrText           & errtext,
                                     tsp00_Bool                *ok)
{
    #undef MF__
    #define MF__ MOD__"en54_SingleBlockIO"
    SAPDB_Int             fd;
    off_t           offset;
    long            ret_code;
    SAPDB_Int             i, ret ;
    unsigned        length_wanted = KGS->dev_blocksize * buf_count ;
    struct UKP_IO_STRUCT *pSingleIOCtrl;

    DBGIN ;

    pSingleIOCtrl = &this_ukt->pSingleIO[devno];
    *ok = false ;

    WAIT_UNTIL_ALONE( this_ukt->exclusive );
        this_ukt->ulSelfIOPending = true;
        if ( pSingleIOCtrl->selfIOStatisticResetPending )
        {   // must reset all resettable counter
            eo52ResetStatisticRecord(pSingleIOCtrl->ReadSelfIo);
            eo52ResetStatisticRecord(pSingleIOCtrl->WriteSelfIo);
            pSingleIOCtrl->selfIOStatisticResetPending = false;
        }
    CLEARLOCK( this_ukt->exclusive );

    ret = e54_init_self_io ( this_ukt, devno, devpno, op_code, fd, errtext ) ;

    if ( ret == VB_OK )
    {
        offset = (off_t) KGS->dev_blocksize * ( (off_t) devpno + (off_t) 1 );

        for ( i = 0, ret_code = 0;
            i < IO_LIMIT && ret_code != (long) length_wanted && ret == VB_OK;
            i++ )
        {
            if ( en54_SignalSave_lseek ( fd, offset, 0 ) == -1 )
            {
                int lastSeekErrno = errno;
                MSGD ((ERR_SINGLEIO_ERR_SEEK_CONT, devno ));
                errno = lastSeekErrno;
                errtext.c2p( sqlerrs() );
                ret = VB_NOTOK ;
            }
            else
            if ( op_code == sp2ioc_read )
                ret_code = en54_ReadWithRetry ( fd, offset, (char *) bufadr, length_wanted ); /* PTS 1106176 */
            else
                ret_code = en54_SignalSave_write ( fd, (char *) bufadr, length_wanted );
        }

        if ( ret_code != (long) length_wanted )
        {
            if ( ret_code != -1 )
            {
                if ( op_code == sp2ioc_read )
                {
                    errtext.c2p( "End of file" );
                }
                else
                {
                    errtext.c2p( "File system full" );
                }
            }
            else
            {
                errtext.c2p( sqlerrs() );
            }
            ret = VB_NOTOK ;
        }

        if ( ret == VB_OK )
            *ok = true;
    }

    WAIT_UNTIL_ALONE( this_ukt->exclusive );
        this_ukt->ulSelfIOPending = false;
    CLEARLOCK( this_ukt->exclusive );

    DBGOUT ;
    return (ret);
}

/*
 * ==================================================================
 */

static SAPDB_Int en54_SingleVectorIO (
                                ten50_UKT_Control     * this_ukt,
                                SAPDB_Int4              devno,
                                tsp00_PageNo            devpno,
                                tsp00_PageAddr          pageVector[],
                                SAPDB_Int2              pageCount,
                                tsp00_ErrText         & errtext,
                                tsp00_Bool            * ok)
{
    #undef MF__
    #define MF__ MOD__"en54_SingleVectorIO"
    SAPDB_Int             fd;
    off_t           offset;
    long            ret_code;
    SAPDB_Int             i, ret ;
    struct iovec    ioVector[SAPDB_IOV_LIMIT]; /* OS dependend found in limits.h */
    SAPDB_Int iovLimit = (SAPDB_IOV_LIMIT < IOV_MAX ? SAPDB_IOV_LIMIT : IOV_MAX);
    SAPDB_Int iovMax; /* If I/O is to be splitted in several calls, */
                /* this counts the iov elements in use */
    struct UKP_IO_STRUCT *pSingleIOCtrl;

    DBGIN ;

    pSingleIOCtrl = &this_ukt->pSingleIO[devno];

    WAIT_UNTIL_ALONE( this_ukt->exclusive );
        this_ukt->ulSelfIOPending = true;
    CLEARLOCK( this_ukt->exclusive );

    SAPDB_memset(ioVector, 0, sizeof(struct iovec) * iovLimit); /* is stack is too small, this at least will force an overflow */

    ret = e54_init_self_io ( this_ukt, devno, devpno, sp2ioc_write, fd, errtext ) ;
    if ( ret == VB_OK )
    {
      offset = (off_t) KGS->dev_blocksize * ( (off_t) devpno + (off_t) 1 );

      if ( en54_SignalSave_lseek ( fd, offset, 0 ) == -1 )
      {
        char buffer[EN54_SMALL_MESSAGE_BUFFER_SIZE];
        sp77sprintf(buffer, sizeof(buffer), "DEVNO %d PNO %d : %s", devno, devpno, sqlerrs() );

        MSGD ((ERR_DEVIO_ERR_SEEK_FD, fd, &buffer[0] ));
        ret = VB_NOTOK;
      }
    }

    for ( i = 0; ret == VB_OK && i < pageCount; i += iovMax )
    {
      SAPDB_Int j;
      SAPDB_Int iovWrite; 
      SAPDB_Int retry = 0;

      /* Last io Action must not use all ioVector elements */
      if ( (i + iovLimit) > pageCount )
      {
        iovMax = pageCount - i; /* calculate remaining I/O pages */
      }
      else
      {
        iovMax = iovLimit; /* use maximum offered by OS */
      }

      /* All io Action must update buffer addresses */
      for ( j = 0; j < iovMax; j++ )
      {
        ioVector[j].iov_base = (char *)pageVector[i+j];
        ioVector[j].iov_len  = KGS->dev_blocksize;
      }
      for (; j < iovLimit; j++ )
      {
          ioVector[j].iov_base = 0;
          ioVector[j].iov_len  = 0;
      }

      iovWrite = iovMax;
      do
      {
          ret_code = RTE_save_writev ( fd, ioVector, iovWrite );

          if ( ret_code <= 0 )
          {
              char buffer[EN54_SMALL_MESSAGE_BUFFER_SIZE];
              sp77sprintf(buffer, sizeof(buffer), "(v) DEVNO %d PNO %d Size %d : %s", devno, devpno + i, iovWrite, sqlerrs() );
              MSGD (( ERR_DEVIO_ERR_WRITE_FD, fd, &buffer[0] ));
              ret = VB_NOTOK ;
          } 
          else if ( ret_code != iovWrite * KGS->dev_blocksize )
          {
              if ( ret_code % KGS->dev_blocksize ) /* Oops, unexpected length! */
              {
                  char buffer[EN54_SMALL_MESSAGE_BUFFER_SIZE];
                  sp77sprintf(buffer, sizeof(buffer), "(v) DEVNO %d PNO %d Size %d : Result %ld (%s)"
                      , devno, devpno + i, iovWrite, (long)ret_code, sqlerrs() );
                  MSGD (( ERR_DEVIO_ERR_WRITE_FD, fd, &buffer[0] ));
                  ret = VB_NOTOK ;
              }
              else
              {
                  if ( 0 != ret_code )
                  {
                      SAPDB_Int k = ret_code / KGS->dev_blocksize;
                      retry = 0;

                      for ( j = 0; k < iovWrite; j++, k++ ) /* Shift buffer addresses */
                          ioVector[j].iov_base = ioVector[k].iov_base;

                      for ( ; j < iovWrite; j++ )
                      {
                          ioVector[j].iov_base = 0;
                          ioVector[j].iov_len  = 0;
                      }
                      iovWrite -= ret_code / KGS->dev_blocksize;
                  }
                  else
                  {
                      if ( NUMBER_OF_IO_RETRIES == retry++ )
                      {
                          char buffer[EN54_SMALL_MESSAGE_BUFFER_SIZE];
                          sp77sprintf(buffer, sizeof(buffer), "(v) DEVNO %d PNO %d Size %d : Result %ld (%s)"
                                     , devno, devpno + i, iovWrite, (long)ret_code, sqlerrs() );
                          MSGD (( ERR_DEVIO_ERR_WRITE_FD, fd, &buffer[0] ));
                          ret = VB_NOTOK ;
                      }
                  }
              }
          }
          else
          {
              break; /* all data written */
          }
      } 
      while ( ret != VB_NOTOK );
    }

    WAIT_UNTIL_ALONE( this_ukt->exclusive );
        this_ukt->ulSelfIOPending = false;
    CLEARLOCK( this_ukt->exclusive );

    *ok = ( ret == VB_OK );

    DBGOUT ;

    return (ret);
}

/*
 * ==================================================================
 */

static SAPDB_Int en54_SimulateSingleVectorIO (
                                ten50_UKT_Control     * this_ukt,
                                SAPDB_Int4              devno,
                                tsp00_PageNo            devpno,
                                tsp00_PageAddr          pageVector[],
                                SAPDB_Int2              pageCount,
                                tsp00_ErrText         & errtext,
                                tsp00_Bool            * ok)
{
    #undef MF__
    #define MF__ MOD__"en54_SimulateSingleVectorIO"
    SAPDB_Int             fd;
    off_t                 offset;
    long                  ret_code;
    SAPDB_Int             i;
    SAPDB_Int             ret;
    SAPDB_Int             pagesWritten;
    struct UKP_IO_STRUCT *pSingleIOCtrl;

    DBGIN ;

    pSingleIOCtrl = &this_ukt->pSingleIO[devno];

    WAIT_UNTIL_ALONE( this_ukt->exclusive );
        this_ukt->ulSelfIOPending = true;
    CLEARLOCK( this_ukt->exclusive );

    ret = e54_init_self_io ( this_ukt, devno, devpno, sp2ioc_write, fd, errtext ) ;
    if ( ret == VB_OK )
    {
      offset = (off_t) KGS->dev_blocksize * ( (off_t) devpno + (off_t) 1 );

      if ( en54_SignalSave_lseek ( fd, offset, 0 ) == -1 )
      {
        char buffer[EN54_SMALL_MESSAGE_BUFFER_SIZE];
        sp77sprintf(buffer, sizeof(buffer), "(vSim) DEVNO %d PNO %d : %s", devno, devpno, sqlerrs() );

        MSGD ((ERR_DEVIO_ERR_SEEK_FD, fd, &buffer[0] ));
        errtext.c2p( "Seek failed" );
        ret = VB_NOTOK;
      }
    }

    pagesWritten = 0;
    for ( i = 0; ret == VB_OK && i < pageCount; i = pagesWritten )
    {
        SAPDB_Int   j;
        SAPDB_Byte *firstPage = (SAPDB_Byte *)pageVector[i];
        SAPDB_Long  bytesToWrite = KGS->dev_blocksize;

        /* All adjacent pages can be written in a single call */
        for ( j = 1; (i+j) < pageCount; j++ )
        {
            if ( (firstPage + bytesToWrite) != (SAPDB_Byte *)pageVector[i+j] )
            {
                break;
            }
            bytesToWrite += KGS->dev_blocksize;
        }

#ifdef RTEDEBUG_SIMULATE_VECTORIO
        {
            char buffer2[EN54_SMALL_MESSAGE_BUFFER_SIZE];
            sp77sprintf(buffer2, sizeof(buffer2), "DEBUG...(SimSelfIo) PNO %d Size %d ( [%d-%d[ of %d )"
                , devpno + i, bytesToWrite, i, i+j, pageCount );
            MSGD (( ERR_DEVIO_ERR_WRITE_FD, fd, &buffer2[0] ));
        }
#endif /* RTEDEBUG_SIMULATE_VECTORIO */

        ret_code = RTE_save_write ( fd, firstPage, bytesToWrite );
        if ( ret_code == bytesToWrite )
        {
            pagesWritten += j; /* Add all pages sucessfully written */
        }
        else if ( ret_code <= 0 )
        {
            char buffer[EN54_SMALL_MESSAGE_BUFFER_SIZE];
            sp77sprintf(buffer, sizeof(buffer), "(vSim) DEVNO %d PNO %d Size %d : %s", devno, devpno + i, bytesToWrite, sqlerrs() );
            MSGD (( ERR_DEVIO_ERR_WRITE_FD, fd, &buffer[0] ));
            if ( 0 == ret_code )
            {
                errtext.c2p( "File system full" );
            }
            else
            {
                errtext.c2p( "Write I/O error" );
            }
            ret = VB_NOTOK ;
        }
        else if ( ret_code != bytesToWrite )
        {
            char buffer[EN54_SMALL_MESSAGE_BUFFER_SIZE];
            sp77sprintf(buffer, sizeof(buffer), "(vSim) DEVNO %d PNO %d Size %d : Result %ld (%s)"
                   , devno, devpno + i, bytesToWrite, ret_code, sqlerrs() );
            MSGD (( ERR_DEVIO_ERR_WRITE_FD, fd, &buffer[0] ));
            errtext.c2p( "File system full" );
            ret = VB_NOTOK ;
        }
    }

    WAIT_UNTIL_ALONE( this_ukt->exclusive );
        this_ukt->ulSelfIOPending = false;
    CLEARLOCK( this_ukt->exclusive );

    *ok = ( ret == VB_OK );

    DBGOUT ;

    return (ret);
}

/* ======================================= MF__ MOD__ (utilities) */
/*
 * ==================================================================
 */

global  void    e54_int_to_text ( SAPDB_Int       io_err,
                                  tsp00_ErrText & errtext)
{
    #undef MF__
    #define MF__ MOD__"e54_int_to_text"
    char      buffer[EN54_SMALL_MESSAGE_BUFFER_SIZE];
    SAPDB_Int len;

    if ( io_err )
    {
#if defined(LINUX) || defined(FREEBSD) || defined(AIX)
        /* LINUX + AIX define strerror() being not thread save... but strerror_r is... */
        (void) sp77sprintf ( buffer, sizeof(buffer), "UNIX errno %-2d (", io_err);
        len = strlen(buffer);
        if ( 0 == strerror_r(io_err, buffer + len, sizeof(buffer) - len) )
        {
            len = strlen(buffer);
            if ( strlen(buffer) < (sizeof(buffer)-1) )
            {
                len = strlen(buffer);
            }
        }
        buffer[len++] = ')';
        buffer[len] = 0;
#elif defined(SUN) || defined(SOLARIS) || defined(HPUX)
        /* SUN + SOLARIS + HPUX define strerror() as thread save. strerror_r does not exist on Solaris and is obsolete on HPUX */
        (void) sp77sprintf ( buffer, sizeof(buffer), "UNIX errno %-2d (%s)\0",
            io_err, strerror(io_err) );
#else
        /* AIX does not support a multithread save version of strerror and strerror_r is n ot document (yet...) */
        (void) sp77sprintf ( buffer, sizeof(buffer), "UNIX errno %-2d (%s)\0",
            io_err, sys_errlist[io_err] );
#endif
        if ( ( len = strlen ( buffer ) ) < MAXERRORTEXT )
        {
            errtext.c2p( &buffer[0] );
        }
        else
        {
            errtext.c2p( &buffer[0] );
        }
    }
    else
    {
        errtext.SetBlank();
    }
}

/*
 * ==================================================================
 */

/*
 * en54_RequestToDev0 need no sync, since utility does nothing
 * until reply on attach
 */

static  void    en54_RequestToDev0 (
                        struct DOUBLY_LINKED   *request)

{
    #undef MF__
    #define MF__ MOD__"en54_RequestToDev0"
    DBG1 (( MF__,"request 0x%lx from T%d\n",
            (long) request , request->taskid->index ));

    WAIT_UNTIL_ALONE ( KGS->dev0.alone );
    DBG1 (( MF__,"ALONE now \n" ));

    if ( KGS->dev0.io_request->request )
    {
        MSGD ((IERR_TODEV0_REQUEST_NOT_FREE ));
        vabort(WRITE_CORE);
    }
    DBG1 (( MF__,"      setting io_requ addr 0x%lx\n",
                    (long) KGS->dev0.io_request ));

    KGS->dev0.io_request->request = request ;
    KGS->dev0.io_request          = KGS->dev0.io_request->next ;

    CLEARLOCK ( KGS->dev0.alone );

    e72_wake ( reinterpret_cast<ten50_UKT_Control *>(&KGS->dev0) );
    DBG1 (( MF__,"WAKE is sent \n" ));
}

/*
 * ==================================================================
 */

static  void    en54_RequestToDevi (
                        struct DEV_PCB         *devi,
                        struct DOUBLY_LINKED   *request )
{
    #undef MF__
    #define MF__ MOD__"en54_RequestToDevi"

    DBG1 (( MF__,"request 0x%lx from T%d\n",
            (long) request , request->taskid->index ));

    WAIT_UNTIL_ALONE ( devi->alone );
#ifdef DEBUG_ALONE
    DBG1 (( MF__,"ALONE now \n" ));
#endif

    if ( devi->io_request->request )
    {
        MSGD ((IERR_TODEVI_REQUEST_NOT_FREE ));
        vabort(WRITE_CORE);
    }

    devi->io_request->request = request ;
    devi->io_request          = devi->io_request->next ;
    devi->DevIOStat.ulIOQueLen ++ ;

    CLEARLOCK ( devi->alone );

    if ( devi->DevIOStat.ulIOQueLen > devi->DevIOStat.ulMaxIOQueLen )
        devi->DevIOStat.ulMaxIOQueLen = devi->DevIOStat.ulIOQueLen;

    /* PTS 1001075 */
    if ( request->req_type == REQ_ASYNCLOSE_EO00 ||
         request->req_type == REQ_VDETACH_EO00 )
      e72_wake_ignore_semop_err( reinterpret_cast<ten50_UKT_Control *>(devi) );
    else
      e72_wake ( reinterpret_cast<ten50_UKT_Control *>(devi) );

    DBG1 (( MF__,"WAKE is sent \n" ));
}

/*
 * ==================================================================
 */

static  void    en54_RequestToUKT (
                        struct DEV_PCB         *devi,
                        struct DOUBLY_LINKED   *request )
{
    #undef MF__
    #define MF__ MOD__"en54_RequestToUKT"

    DBG1 (( MF__,"request 0x%lx from T%d\n",
                (long) request , request->taskid->index ));

    WAIT_UNTIL_ALONE ( devi->alone );

#ifdef DEBUG_ALONE
    DBG1 (( MF__,"ALONE now \n" ));
#endif

    devi->io_work->request = 0 ;
    devi->io_work          = devi->io_work->next ;
    devi->DevIOStat.ulIOQueLen -- ;

    CLEARLOCK ( devi->alone );

    en71_iocEnquAndWake ( request->taskid->ukt, request->taskid, request );

    DBG1 (( MF__,"WAKE is sent \n" ));
}

static tsp00_Bool en54_ValidHostfile (
                        SAPDB_Int4               hostfileno,
                        char const             * routineName,
                        tsp00_VaReturn         * returncode,
                        tsp00_ErrText          & errtext)

{
    #undef MF__
    #define MF__ MOD__"en54_ValidHostfile"
    if ( (hostfileno < KGS->maxdevspaces) ||
         (hostfileno >= KGS->maxdevspaces * 2 + KGS->maxtapes) )
    {
        MSGD ((IERR_VALIDFILE_INVALID_FILENO, routineName, (long) hostfileno ));
        errtext.c2p( "Illegal hostfileno" );
        returncode->becomes(va_notok);
        return false;
    }
    if ( ! KGS->dev0.dev[hostfileno].devspace[0] )
    {
        MSGD ((IERR_VALIDFILE_NOT_OPEN, routineName, (long) hostfileno ));
        errtext.c2p( "Device not open" );
        returncode->becomes(va_notok);
        return false;
    }
    return true;
}

static UCHAR* en54_AllocAligned (
                        struct ALIGNED_MEM     *memInfo,
                        SAPDB_Int                     blockSize,
                        SAPDB_Int                    *retcod)
{
    #undef MF__
    #define MF__ MOD__"en54_AllocAligned"
    unsigned long rawAlign = XPARAM(ulDBPageSize);

    memInfo->baseAddr = NULL;
    if ( ALLOC_MEM_EO57( (void **)&(memInfo->baseAddr), blockSize + rawAlign )
        != NO_ERROR_EO001 )
    {
        MSGD ((ERR_AMALLOC_ERR_MALLOC,
                        blockSize + rawAlign ,  sqlerrs() ));
        memInfo->dataAddr = NULL;
        if ( retcod )
        {
            *retcod = UNDEF ;
        }
    }
    else
    {
        memInfo->dataAddr =
            (UCHAR*) ( ((unsigned long) (memInfo->baseAddr + rawAlign - 1)
                            / rawAlign) * rawAlign );
        SAPDB_memset( memInfo->dataAddr, 0, blockSize );
    }
    return memInfo->dataAddr;
}

static void en54_FreeAligned (
struct ALIGNED_MEM      *memInfo)

{
    if (memInfo->baseAddr != NULL)
        FREE_MEM_EO57 (memInfo->baseAddr);
}

/* ======================================= MF__ MOD__ (tape handling) */


static void en54_FillPattern (
                        UCHAR*                  pBuf,
                        SAPDB_Int                     pBufSize)

{
    SAPDB_Int iCol;
    SAPDB_Int iRow;
    SAPDB_Int rowCount;
#define COLSIZE 64

    rowCount = pBufSize / COLSIZE;
    SAPDB_memset (pBuf, ' ', pBufSize);
    for (iRow = 0; iRow < rowCount; ++iRow)
    {
        iCol = iRow % COLSIZE;
        pBuf[iRow * COLSIZE + iCol] = '*';
    }
}

/* define which include file for tapes */

#define TAPE_MTIO_H     1
#define TAPE_ATT        3
#define TAPE_AIX        4

#if defined (SVR4) && defined (I386)
#define TAPE_VARIANT TAPE_ATT
#include <sys/tape.h>

#elif defined (_IBMR2)
#define TAPE_VARIANT TAPE_AIX
#include <sys/tape.h>

#elif defined (HPUX) || defined (OSF1) || defined (SVR4) || defined (SOLARIS) || defined(LINUX) || defined(FREEBSD)
#define TAPE_VARIANT TAPE_MTIO_H
#include <sys/ioctl.h>
#include <sys/mtio.h>

#else
#error "Must define something to include tape header"
#endif

#if defined (HPUX)
#define         NOREWIND_BITS           MT_NO_REWIND_MASK
#elif defined (OSF1)
#define         NOREWIND_BITS           NO_REWIND
#elif defined (SVR4) && defined (SUN) || defined (SOLARIS)
#define         NOREWIND_BITS           MT_NOREWIND
#elif defined (FREEBSD)
#define T_NOREWIND      0x04	     /* This was found in 4.2BSD */
#define         NOREWIND_BITS           T_NOREWIND
#elif defined (LINUX)
/* PTS 1103770 */
/* undocumented but found in source of "drivers/scsi/st.c" */
#define         NOREWIND_BITS           0x80
#endif

#if !defined (getminor)
#define getminor(dev)   (dev % 256)
#endif

#if TAPE_VARIANT == TAPE_MTIO_H
static tsp00_Bool e54_rewind_mtio (
                        SAPDB_Int                     filedes,
                        SAPDB_UInt4                  *errcode)

{
    #undef MF__
    #define MF__ MOD__"e54_rewind_mtio"
    SAPDB_Int         ioctlRC;
    struct mtop tapeOP;
    tapeOP.mt_op = MTREW;
    tapeOP.mt_count = 1;
    ioctlRC = ioctl (filedes, MTIOCTOP, &tapeOP);
    if (ioctlRC != -1)
        *errcode = (SAPDB_UInt4)UNDEF;
    else {
        if (errno == EINVAL)
            /* rewind not supported is OK */
            *errcode = (SAPDB_UInt4)UNDEF;
        else
            *errcode = DEVIO_ERR_DEVCONTROL;
    }
    return (ioctlRC != -1 ? true : false);
}

#define E54_REWIND_PRIMITIVE(filedes, errcode) e54_rewind_mtio (filedes, errcode)

#if defined (MTIOCGET)

static tsp00_Bool e54_isTape_hpux (
                        struct  DEV_CNTL_INFO  *devCntlInfo)

{
    #undef MF__
    #define MF__ MOD__"e54_isTape_hpux"
    SAPDB_Int                 ioctlRC;
    struct mtget        tapeStatus;
    tsp00_Bool             result;

    ioctlRC = ioctl (devCntlInfo->filedes, MTIOCGET, &tapeStatus);
    if (ioctlRC == -1) {
        /* not supported by driver */
        result = false;
    }
    else {
        result = true;
    }
    return result;
}

#define E54_ISTAPE(devCntlInfo)    e54_isTape_hpux (devCntlInfo)

#elif defined (OSF1)

#include <sys/devio.h>

static tsp00_Bool e54_isTape_osf1 (
                        struct  DEV_CNTL_INFO  *devCntlInfo)

{
    #undef MF__
    #define MF__ MOD__"e54_isTape_osf1"
    SAPDB_Int                 ioctlRC;
    struct devget       devStatus;
    tsp00_Bool             result;

    ioctlRC = ioctl (devCntlInfo->filedes, DEVIOCGET, &devStatus);
    if (ioctlRC == -1) {
        /* not supported by driver */
        result = false;
    }
    else {
        result = (devStatus.category == DEV_TAPE);
    }
    return result;
}

#define E54_ISTAPE(devCntlInfo)    e54_isTape_osf1 (devCntlInfo)

#else

#define E54_ISTAPE(devCntlInfo)    false

#endif

static tsp00_VfType e54_detectByDriver_mtio (struct  DEV_CNTL_INFO  *devCntlInfo)
{
    #undef MF__
    #define MF__ MOD__"e54_detectByDriver_mtio"
    tsp00_VfType         result;
    tsp00_Bool           isTape;
    SAPDB_UInt4          errcode;

    if (!E54_ISTAPE (devCntlInfo)) {
        result.becomes(vf_t_raw);
    }
    else {

#if defined (NOREWIND_BITS)
        if (getminor (devCntlInfo->statInfo.st_rdev) & NOREWIND_BITS)
            result.becomes(vf_t_tape_norew);
        else
            result.becomes(vf_t_tape_rew);
        /* e54_rewind_mtio (devCntlInfo->filedes, &errcode); */
#else
        result.becomes(vf_t_unknown);
#endif
    }
    return result;
}

#define E54_DETECT_BY_DRIVER(devCntlInfo) e54_detectByDriver_mtio (devCntlInfo)

#endif

#if TAPE_VARIANT == TAPE_ATT
static tsp00_Bool e54_rewind_att (
                        SAPDB_Int                     filedes,
                        SAPDB_UInt4                  *errcode)

{
    #undef MF__
    #define MF__ MOD__"e54_rewind_att"
    SAPDB_Int ioctlRC;

    ioctlRC = ioctl (filedes, T_RWD);
    if (ioctlRC != -1)
        *errcode = (SAPDB_UInt4)UNDEF;
    else
        *errcode = DEVIO_ERR_DEVCONTROL;
    return (ioctlRC != -1);
}

#define E54_REWIND_PRIMITIVE(filedes, errcode) e54_rewind_att (filedes, errcode)

#define E54_DETECT_BY_DRIVER(devCntlInfo) vf_t_unknown

#endif

#if TAPE_VARIANT == TAPE_AIX

#include <sys/ioctl.h>
#include <sys/devinfo.h>
#include <sys/tape.h>
#include <sys/scsi.h>
#include <sys/watchdog.h>
#ifndef AIX_52_HEADERFILES_NO_LONGER_CORRUPT
/*
  We have a problem with our new machine is0141... Waiting for APAR from IBM...
 */
#define TAPE_NOREW_ON_CLOSE  0x01       /* bit check for no rewind   */
#define TAPE_RETEN_ON_OPEN   0x02       /* bit check for retention   */
#else
#include <sys/tapedd.h>
#endif

#include <sys/sysmacros.h>

static tsp00_Bool e54_rewind_aix (
                        SAPDB_Int                     filedes,
                        SAPDB_UInt4                  *errcode)

{
    #undef MF__
    #define MF__ MOD__"e54_rewind_aix"
    SAPDB_Int ioctlRC;
    struct stop         stop ;

    stop.st_op = STREW ;
    stop.st_count = 1 ;
    ioctlRC = ioctl (filedes, STIOCTOP , &stop );
    if (ioctlRC != -1)
        *errcode = (SAPDB_UInt4)UNDEF;
    else
        *errcode = DEVIO_ERR_DEVCONTROL;
    return (ioctlRC != -1);
}

#define E54_REWIND_PRIMITIVE(filedes, errcode) e54_rewind_aix (filedes, errcode)

/* ADIS 1000969 Start */

static SAPDB_Int e54_isTape_aix (SAPDB_Int fhandle)
{
    #undef MF__
    #define MF__ MOD__"e54_isTape_aix"
    SAPDB_Int osRC;
    struct devinfo devInfo;
    SAPDB_Int result;

    osRC = ioctl (fhandle, IOCINFO, &devInfo);
    if (osRC == -1)
    {
        return false;
    }
    if ((devInfo.devtype == DD_TAPE)
            || (devInfo.devtype == DD_SCTAPE) )
    {
        result = true;
    }
    else
    {
        result = false;
    }
    return result;
}

static tsp00_VfType e54_detectByDriver_aix (
                        struct  DEV_CNTL_INFO  *devCntlInfo)

{
    #undef MF__
    #define MF__ MOD__"e54_detectByDriver_aix"
    tsp00_VfType         result;
    SAPDB_Int                 minorKind;
    SAPDB_UInt4               errcode;

    if (!e54_isTape_aix (devCntlInfo->filedes)) {
        result.becomes(vf_t_raw);
    }
    else {
        minorKind = minor (devCntlInfo->statInfo.st_rdev);
        if ((minorKind & TAPE_NOREW_ON_CLOSE)
                && !(minorKind & TAPE_RETEN_ON_OPEN)) {
            result.becomes(vf_t_tape_norew);
        }
        else {
            result.becomes(vf_t_tape_rew);
        }
    }
    return result;
}

#define E54_DETECT_BY_DRIVER(devCntlInfo) e54_detectByDriver_aix (devCntlInfo)

/* ADIS 1000969 End */

#endif

static tsp00_Bool en54_Rewind (
                        SAPDB_Int                     filedes,
                        SAPDB_Int4                hostfileno,
                        tsp00_Bool                 forClose,
                        SAPDB_UInt4                  *errcode)

{
    #undef MF__
    #define MF__ MOD__"en54_Rewind"
    struct dev_info             *devInfo;
    tsp_vf_label                devspace_label ;
    tsp00_Bool                  result;
    tsp2_io_op_code             op_code;
    SAPDB_Int4                  page0Size;

    DBG1 (( MF__," entry %s\n", forClose ? "for close" : "for reuse" ));
    result = E54_REWIND_PRIMITIVE (filedes, errcode);
    if (result == true && !forClose) {
        devInfo = &KGS->dev0.dev[hostfileno];
        if (devInfo->asyn_write) {
            op_code.becomes(sp2ioc_write);
            SAPDB_memset (devspace_label, '_', sizeof (devspace_label));
        }
        else {
            op_code.becomes(sp2ioc_read);
        }
        page0Size = devInfo->block_size * devInfo->max_block_cnt;
        en54_Page0IO (filedes, op_code, &page0Size, 0, false, devspace_label);
    }
    DBG1 (( MF__," exit\n" ));
    return result;
}


static tsp00_VfType_Enum e54_genericNorewindTest (
                        struct  DEV_CNTL_INFO  *devCntlInfo,
                        SAPDB_UInt4                  *errcode)

{
    #undef MF__
    #define MF__ MOD__"e54_genericNorewindTest"
    tsp00_VfType                 result;
    SAPDB_Int                         ioCount;
    struct ALIGNED_MEM          patternPage;
    struct ALIGNED_MEM          ioPage;
    tsp_vf_label                devspace_label;
    SAPDB_Int                         blockSize;
    SAPDB_Int4                    dummyBlockSize;
    SAPDB_Int                         page0_OK;

    blockSize = devCntlInfo->block_size * devCntlInfo->max_block_cnt;
    dummyBlockSize = blockSize;
    DBG1 (( MF__,"blockSize: %d\n", blockSize));
    DBG1 (( MF__,"%s Check\n", devCntlInfo->destructiveCheck ? "destructive" : "nondestructive"));
    if (!devCntlInfo->destructiveCheck || !devCntlInfo->asyn_write) {
        /* generic test requires writing to tape */
        DBG1 (( MF__,"Cannot overwrite tape contents\n"));
        return vf_t_unknown;
    }
    /* assumes that asyn files are opened with O_RDWR | en54_AddSynchronousOpen()   */
    /* otherwise: O_RDONLY requires close and open with O_WRONLY */
    en54_AllocAligned (&patternPage, blockSize, NULL);
    en54_AllocAligned (&ioPage, blockSize, NULL);
    if ((patternPage.dataAddr == NULL) || (ioPage.dataAddr == NULL)) {
        en54_FreeAligned (&patternPage);
        en54_FreeAligned (&ioPage);
        *errcode = DEVIO_ERR_MALLOC;
        return vf_t_unknown;
    }
    /*
     *   write two blocks so that a read after the last write
     *       would find something different from the test pattern
     */
    en54_Rewind (devCntlInfo->filedes, devCntlInfo->hostfileno,
        false, errcode);
    SAPDB_memset (patternPage.dataAddr, '\0', blockSize);
    ioCount = RTE_save_write (devCntlInfo->filedes, patternPage.dataAddr,
        blockSize);
    ioCount += RTE_save_write (devCntlInfo->filedes, patternPage.dataAddr,
        blockSize);
    en54_Rewind (devCntlInfo->filedes, devCntlInfo->hostfileno, false, errcode);
    /*
     * write test pattern to tape
     */
    en54_FillPattern (patternPage.dataAddr, blockSize);
    ioCount = RTE_save_write (devCntlInfo->filedes, patternPage.dataAddr,
        blockSize);
    DBG1 (( MF__,"wrote %d bytes to tape", ioCount));
    if (ioCount != blockSize)
    {
        /* error exit */
        en54_FreeAligned (&patternPage);
        en54_FreeAligned (&ioPage);
        DBG1 (( MF__," returns error (write failed)"));
        *errcode = DEVIO_ERR_SEWR;
        return vf_t_unknown;
    }
    RTE_save_close(devCntlInfo->filedes);
    devCntlInfo->filedes = RTE_save_open ( (const char *)devCntlInfo->deviceName, DEFAULT_OPEN_MODE_EN54 | en54_AddSynchronousOpen() | O_RDWR );
    DBG1 (( MF__," opens tape as %d\n", devCntlInfo->filedes));
    if (devCntlInfo->filedes == -1)
    {
        /* error exit */
        en54_FreeAligned (&patternPage);
        en54_FreeAligned (&ioPage);
        DBG1 (( MF__," returns error (open failed)\n"));
        *errcode = DEVIO_ERR_OPEN;
        return vf_t_unknown;
    }
    page0_OK = en54_Page0IO (devCntlInfo->filedes, sp2ioc_read, &dummyBlockSize, 0,
                            false, devspace_label);
    if (page0_OK == OK) {
        /* do read */
        SAPDB_memset (ioPage.dataAddr, 0, blockSize);
        ioCount = RTE_save_read (devCntlInfo->filedes, ioPage.dataAddr, blockSize);
        DBG1 (( MF__,"read %d bytes from tape\n", ioCount));
    }
    else {
        DBG1 (( MF__,"en54_Page0IO failed\n"));
    }

    if ((page0_OK != OK) || (ioCount != blockSize))
    {
        /* probably at end, no rewind */
        en54_FreeAligned (&patternPage);
        en54_FreeAligned (&ioPage);
        result.becomes(vf_t_tape_norew);
        en54_Rewind (devCntlInfo->filedes, devCntlInfo->hostfileno,
            true, errcode);
        RTE_save_close(devCntlInfo->filedes);
        devCntlInfo->filedes = RTE_save_open ( (const char *)devCntlInfo->deviceName, DEFAULT_OPEN_MODE_EN54 | en54_AddSynchronousOpen() | O_RDWR );
        if (devCntlInfo->filedes > 0) {
            SAPDB_memset (devspace_label, '_', sizeof ( tsp_vf_label ) );
            dummyBlockSize = blockSize;
            en54_Page0IO (devCntlInfo->filedes, sp2ioc_write, &dummyBlockSize, 0,
                false, devspace_label);
        }
        DBG1 (( MF__," returns %s (read failed)\n", ftype_text[result]));
        return result;
    }

    if (memcmp (patternPage.dataAddr, ioPage.dataAddr, blockSize) == 0)
    {
        result.becomes(vf_t_tape_rew);
        RTE_save_close(devCntlInfo->filedes);
        devCntlInfo->filedes = RTE_save_open ( (const char *)devCntlInfo->deviceName, DEFAULT_OPEN_MODE_EN54 | en54_AddSynchronousOpen() | O_RDWR );
        if (devCntlInfo->filedes > 0) {
            SAPDB_memset (devspace_label, '_', sizeof ( tsp_vf_label ) );
            dummyBlockSize = blockSize;
            en54_Page0IO (devCntlInfo->filedes, sp2ioc_write, &dummyBlockSize, 0,
                false, devspace_label);
        }
    }
    else
    {
        result.becomes(vf_t_tape_norew);
        en54_Rewind (devCntlInfo->filedes, devCntlInfo->hostfileno, false, errcode);
    }

    en54_FreeAligned (&patternPage);
    en54_FreeAligned (&ioPage);
    DBG1 (( MF__," returns %s (from data compare)\n", ftype_text[result]));

    return result;
}

static tsp00_VfType e54_checkForTape (
                        struct  DEV_CNTL_INFO  *devCntlInfo,
                        SAPDB_UInt4                  *errcode)

{
    #undef MF__
    #define MF__ MOD__"e54_checkForTape"
    tsp00_VfType result;
    SAPDB_Int isTape;

    *errcode = (SAPDB_UInt4)UNDEF;
    DBG1 (( MF__,"entry \n"));
    result = E54_DETECT_BY_DRIVER (devCntlInfo);
    if (result != vf_t_unknown)
    {
        isTape = (result == vf_t_tape_norew) || (result == vf_t_tape_rew);
        if (isTape && devCntlInfo->destructiveCheck) {
            en54_Rewind (devCntlInfo->filedes, devCntlInfo->hostfileno,
                false, errcode);
        }
        /* normal exit, value known */
        DBG1 (( MF__," returns %s (from driver)\n",
            ftype_text[result]));
    }
    else {
        result.becomes(e54_genericNorewindTest (devCntlInfo, errcode));
        DBG1 (( MF__," returns %s (generic test)\n",
            ftype_text[result]));
    }
    return result;
}

static tsp00_VfType en54_StatFileType (
                        struct  DEV_CNTL_INFO  *devCntlInfo,
                        SAPDB_UInt4            *returncode)
{
    #undef MF__
    #define MF__ MOD__"en54_StatFileType"
    tsp00_VfType result;
    mode_t fileKind;

    DBGIN;
    *returncode = (SAPDB_UInt4)UNDEF;
    if (RTE_save_fstat (devCntlInfo->filedes, &devCntlInfo->statInfo) == -1)
    {
        DBG1 (( MF__," fstat failed with %ld \n",
            (long) errno));
        result.becomes(vf_t_unknown);
        *returncode = DEVIO_ERR_FILESTAT;
    }
    else
    {
        fileKind = devCntlInfo->statInfo.st_mode;
        DBG1 (( MF__," fstat (%d) gives fileKind %o \n",
            devCntlInfo->filedes, fileKind));
        if ( S_ISBLK (fileKind) )
        {
            DBG1 (( MF__," S_ISBLK: %o =? %o \n",
                fileKind & S_IFBLK, S_IFBLK));
            result.becomes(vf_t_raw);
        }
        else if ( S_ISCHR (fileKind) )
        {
            DBG1 (( MF__," S_ISCHR: %o =? %o \n",
                fileKind & S_IFCHR, S_IFCHR));
            result = e54_checkForTape (devCntlInfo, returncode);
        }
        else if ( S_ISFIFO (fileKind) )
        {
            DBG1 (( MF__," S_ISFIFO: %o =? %o \n",
                fileKind & S_IFIFO, S_IFIFO));
            result.becomes(vf_t_pipe);
        }
        else if ( S_ISREG (fileKind) )
        {
            DBG1 (( MF__," S_ISREG: %o =? %o \n",
                fileKind & S_IFREG, S_IFREG));
            result.becomes(vf_t_file);
        }
        else
        {
            DBG1 (( MF__," none found\n"));
            result.becomes(vf_t_unknown);
        }
    }
    DBG1 (( MF__," returns %s \n",
        ftype_text[result]));
    return result;
}

/* ======================================= MF__ MOD__ (debugging) */
/*
 * ==================================================================
 */

#if DUMP
static  void    en54_Dump (
                        char                    mode,
                        SAPDB_Int4                dno,
                        tsp00_PageNo             pno,
                        tsp2_io_op_code_Param          op,
                        tsp00_PageAddr           buf,
                        SAPDB_Int2                cnt,
                        tsp00_TaskId              taskId)

{
    #undef MF__
    #define MF__ MOD__"en54_Dump"
    SAPDB_Int                         idx ;
    unsigned char               *ptr ;


    ptr = (unsigned char*) buf ;
    DBG1 (( MF__,"T%-2ld %c %s dev %2d pno %5ld buf %02x %02x %02x %02x \n",
                    (long) taskId , mode , (op==sp2ioc_read)?"RD":"WR" , (SAPDB_Int) dno ,
                    (long) pno , ptr[0] , ptr[1] , ptr[2] , ptr[3] ));
    for ( idx = 1 ; idx < cnt ; idx ++ )
    {
        ptr = (unsigned char*) (buf + idx);
        DBG1 (( MF__,"                               %02x %02x %02x %02x \n",
                                    ptr[0] , ptr[1] , ptr[2] , ptr[3] ));
    }
}
#endif /*DUMP*/

/*
 * ==================================================================
 */
static void en54_RteDumpFile( tsp00_Pathc &RteDumpFile,
                              tsp00_Pathc &RteDumpPath )
{
    #undef MF__
    #define MF__ MOD__"en54RteDumpFile"
  char *lastSeperator;
  if ( *(XPARAM(szRTEDumpFilename).asCharp()) != '/' )
  {
    strcpy( (char *)RteDumpPath, XPARAM(szRunDirectory).asCharp() );
    strcat( (char *)RteDumpPath, "/" );
    strcat( (char *)RteDumpPath, XPARAM(szRTEDumpFilename).asCharp() ) ;
    strcat( (char *)RteDumpPath, "_dir" );
    strcpy( (char *)RteDumpFile, XPARAM(szRTEDumpFilename).asCharp() ) ;
  }
  else
  {
    strcpy( (char *)RteDumpPath, XPARAM(szRTEDumpFilename).asCharp() ) ;
    lastSeperator = strrchr( RteDumpPath.asCharp(), '/' );
    if ( lastSeperator != NULL )
    {
        *lastSeperator = '\0';
        strcpy( RteDumpFile.asCharp(), lastSeperator + 1);
    }
  }
}

/*
 * ==================================================================
 */
static void en54_RteDumpFinalFile( tsp00_Pathc &RteDumpFinalPath )
{
    #undef MF__
    #define MF__ MOD__"en54RteDumpFile"
    if ( (strlen(XPARAM(szRunDirectory)) + strlen(XPARAM(szRTEDumpFilename)) + 2) < sizeof(tsp00_Pathc) )
    {
        strcpy( (char *)RteDumpFinalPath, XPARAM(szRunDirectory).asCharp() );
        strcat( (char *)RteDumpFinalPath, "/");
        strcat( (char *)RteDumpFinalPath, XPARAM(szRTEDumpFilename).asCharp() ) ;
    }
    else
    {
        *(char *)RteDumpFinalPath = 0;
    }
}

/*
 * ==================================================================
 */
void vdump_rte(void)

{
    #undef MF__
    #define MF__ MOD__"vdump_rte"

  tsp00_Pathc rteDumpPath;
  tsp00_Pathc rteDumpFile ;
  SAPDB_Int dumpStreamFd;
  SAPDB_Int bytes_written ;
  static int ever_called = 0;
  DBGIN;

  if ( ever_called )
  {
      return;
  }
  ever_called = 1;

  en54_RteDumpFile( rteDumpFile, rteDumpPath );
  {
    char szCmdBuf[100 + 3 * sizeof(tsp00_Pathc)];

    KGS->debuglevel=3 ;
    if ( getenv("NO_RTE_DUMP_WANTED") != NULL )
    {
        MSGD ((WRN_DUMP_RTE_FAILED, "NO_RTE_DUMP_WANTED" )) ;
    }
    else
    {
        sp77sprintf(szCmdBuf, sizeof(szCmdBuf), "mkdir -p %s", rteDumpPath.asCharp());
        (void)system(szCmdBuf);
        /* PTS 1109343 DBROOT is needed from environment of kernel */
        /* since x_cons is no longer automatically in path */
        /* PTS 1112519 $DBROOT/bin/x_cons no longer found, but $DBROOT/pgm/console... */

        /* PTS 1118383 console renamed to cons */
        sp77sprintf(szCmdBuf, sizeof(szCmdBuf), "cd %s; %s/pgm/cons %s sh all > %s",
            rteDumpPath.asCharp(), getenv("DBROOT"), KGS->serverdb.asCharp(), rteDumpFile.asCharp());
        if ( system(szCmdBuf) == 0 )
        {
            MSGD (( INFO_DUMP_RTE_DONE, XPARAM(szRTEDumpFilename).asCharp() )) ;
            if ( *(XPARAM(szRTEDumpFilename).asCharp()) != '/' )
            {
                sp77sprintf(szCmdBuf, sizeof(szCmdBuf), "cd %s; mv %s ..", rteDumpPath.asCharp(), rteDumpFile.asCharp());
                (void)system(szCmdBuf);
            }
        }
        else
        {
            MSGD ((WRN_DUMP_RTE_FAILED, &szCmdBuf[0] )) ;
        }
    }
  }
  DBGOUT;
}

/*
 * ==================================================================
 */
/*
 * Dump an RteDump, but only if no such exists already
 */
void en54DumpNewRteDumpOnly(void)
{
    #undef MF__
    #define MF__ MOD__"en81DumpNewRteDumpOnly"

  tsp00_Pathc rteDumpPath;
  struct stat buf;

  en54_RteDumpFinalFile( rteDumpPath );
  if ( 0 == RTE_save_stat((char *)rteDumpPath, &buf) )
  {
      if ( buf.st_ctime > KGS->start_time )
      {
          MSGD (( INFO_DUMP_RTE_DONE, "...by running kernel already" )) ;
          return;
      }
  }
  vdump_rte();
}

/*
 * ==================================================================
 */
tsp00_Bool RTE_CheckForSimulateVectorIo(tsp00_Bool isRawDevice )
{
    tsp00_Bool simulateVectorIo;

    switch ( XPARAM(simulateVectorIo) )
    {
    case RTE_SimVioAlways:
        simulateVectorIo = true;
        break;
    case RTE_SimVioIfOpenDirect:
        simulateVectorIo = true; /* if possible devspaces are always opened for direct I/O */
        break;
    case RTE_SimVioIfOpenDirectOrRawDevice:
        simulateVectorIo = true; /* if possible devspaces are always opened for direct I/O */
        break;
    case RTE_SimVioIfRawDevice:
        simulateVectorIo = isRawDevice;
        break;
    default: /* RTE_SimVioNever */
        simulateVectorIo = false;
        break;
    }

    return simulateVectorIo;
}
/*
 * ==================================================================
 */
externC void vvectorio ( SAPDB_Int4      devno,
                         tsp00_PageNo    devpno,
                         tsp00_PageAddr  pageVector[],
                         SAPDB_Int2      pageCount,
                         tsp00_TaskId    taskId,
                         tsp00_ErrText & errtext,
                         tsp00_Bool    & ok )
{
#undef MF__
#define MF__ MOD__"vvectorio"
    struct TASK_TYPE                * tcb;
    ten50_UKT_Control               * this_ukt;
    REGISTER  struct DOUBLY_LINKED  * request;
    struct DEV_PCB                  * dev_pcb;
    tsp00_Bool                        io_finished = false ;
    SAPDB_Int                         rc ;
    teo52_TaskDevIOStatistic        * pIOStat ;
    struct dev_info                 * pIOCtrl;

    DBGIN;

    if ( pageCount == 1 )
    {
        vblockio( devno
                , devpno
                , sp2ioc_write
                , pageVector[0]
                , 1
                , taskId
                , errtext
                , ok );
        return;
    }

    DBG1 (( MF__,"T%d devno %d pno %d pageCount %d\n",
        taskId, devno , devpno , pageCount ));

    if ( pageCount <= 0 )
    {
        char buffer[EN54_SMALL_MESSAGE_BUFFER_SIZE];
        sp77sprintf(buffer, sizeof(buffer), "vvectorio without pages (%d)", pageCount );
        MSGD(( WRN_TEST_WILDCARD, &buffer[0] ));
        return;
    }

    /*----------------- check devno ----------------------- */
    if ( (devno < 0) || (devno >= KGS->maxdevspaces) )
    {
        MSGD ((IERR_VVECTORIO_INVALID_DEVNO ));
        ok = false ;
        return ;
    }

#ifdef HAVE_DAFS_SUPPORT
    if ( RTE_FileSystemAccessTypeDAFS == KGS->dev0.dev[devno].fileSystemAccessType )
    {
        RTEIO_DAFS_vvectorio ( devno,
                               devpno,
                               pageVector,
                               pageCount,
                               taskId,
                               errtext,
                              &ok );
        return;
    }
#endif

    PID_TCB(taskId, tcb);
    this_ukt = tcb->ukt;

    pIOCtrl = &KGS->dev0.dev[devno];

    if ( pIOCtrl->ulDetachPending )
    {
        MSGD ((IERR_VVECTORIO_NOT_ATTACHED ));
        errtext.c2p( "Not attached" );
        ok = false;
        return;
    }

    /*----------------- check volume -------------------- */
    if (( KGS->devpsets == 1 ) || IS_TRACE_DEV(devno) )
        /* 1 devi proc for trace */
        dev_pcb = KGS->first_dev + devno;
    else
        dev_pcb = en54_FindDevpcb ( this_ukt
                                    , devno
                                    , (tsp00_Bool) tcb->prio_flag
                                    , false );

    if ( dev_pcb->state == KT_INITIALIZED )
    {
        MSGD ((IERR_VVECTORIO_NOT_ATTACHED ));
        ok = false;
        return;
    }

    /*----------------- IO statistics --------------------- */
    if ( tcb->type != TT_DW_EO00  &&         /* PTS 1001193 */
        ( pIOCtrl->devtype == sp2dt_data) )
    {
        if ( ++(KGS->TotalDataIOCnt) >= 0x7fffffff )
        {
            KGS->TotalDataIOCnt = 0 ;
        }
    }

    SET_DEVSP_WAITING_FOR(tcb, devno) ;

    tcb->state = TSK_VVECTORIO_EO00;

    /*------------------- try single_io ------------------- */

    if ( this_ukt->allow_single_io &&  this_ukt->activeTasks == 1 )
    {
        SAPDB_UInt8 startTime = 0;
        struct UKP_IO_STRUCT *pSingleIOCtrl = &this_ukt->pSingleIO[devno];
        teo52_IOStatistic *pUKT_SelfIOStat;
        teo52_IOStatistic *pTask_SelfIOStat;
/*
        if ( is_reading )
        {
           pUKT_SelfIOStat   = &(pSingleIOCtrl->ReadSelfIo);
           pTask_SelfIOStat = &(tcb->TaskStateStat.SelfIOReadStat);
        }
        else
 */
        {
            pUKT_SelfIOStat   = &(pSingleIOCtrl->WriteSelfIo);
            pTask_SelfIOStat = &(tcb->TaskStateStat.SelfIOWriteStat);
        }

        eo52UpdateCountInIOStatisticRec(*pUKT_SelfIOStat, pageCount);
        eo52UpdateCountInIOStatisticRec(*pTask_SelfIOStat, pageCount);

        if ( XPARAM(fTimeMeasure) )
        {
            startTime = RTESys_MicroSecTimer();
#ifdef WANT_DATAWRITE_TIME_INFO
            if ( tcb->type == TT_DW_EO00 )
            {
                MSGD(( INFO_TIME_START, tcb->index, "vvectorio(singleio)", startTime ));
            }
#endif
            if ( RTE_CheckForSimulateVectorIo( dev_pcb->dev_type == MOD_BLCKSPEC ) )
            {
                rc = en54_SimulateSingleVectorIO ( this_ukt, devno, devpno, pageVector,
                    pageCount, errtext, &ok );
            }
            else
            {
                rc = en54_SingleVectorIO ( this_ukt, devno, devpno, pageVector,
                    pageCount, errtext, &ok );
            }

            SAPDB_UInt8 deltaTime = RTESys_MicroSecTimer() - startTime;

            eo52UpdateTimeInStatisticRec(pUKT_SelfIOStat->IOStat, deltaTime);
            eo52UpdateTimeInStatisticRec(pTask_SelfIOStat->IOStat, deltaTime);
#ifdef WANT_DATAWRITE_TIME_INFO
            if ( tcb->type == TT_DW_EO00 )
            {
                MSGD(( INFO_TIME_STOP, tcb->index, "vvectorio(singleio)", startTime, startTime + deltaTime, deltaTime ));
            }
#endif
        }
        else
        {
            if ( RTE_CheckForSimulateVectorIo( dev_pcb->dev_type == MOD_BLCKSPEC ) )
            {
                rc = en54_SimulateSingleVectorIO ( this_ukt, devno, devpno, pageVector,
                    pageCount, errtext, &ok );
            }
            else
            {
                rc = en54_SingleVectorIO ( this_ukt, devno, devpno, pageVector,
                    pageCount, errtext, &ok );
            }
        }

        io_finished = ( rc != VB_NO_MORE_FILES );
    }

    /*----------------- try asyn I/O via DEVi ------------- */

    /* UNIX does not support asynchronous I/O using Scatter/Gather transfer... */

    if ( !io_finished )
    {
        pIOStat = &(tcb->TaskStateStat.DevIOWriteStat) ;

        eo52UpdateCountInIOStatisticRec(*pIOStat, pageCount);

        request = GetTaskDLQElement1( tcb );

        request->taskid                    = tcb;
        request->req_type                  = REQ_VVECTORIO_EO00;
        request->args.vve_parms.devno      = devno;
        request->args.vve_parms.devpno     = devpno;
        request->args.vve_parms.pageVector = pageVector;
        request->args.vve_parms.pageCount  = pageCount;
        request->args.vve_parms.ok         = false;

        if (( KGS->devpsets == 1 ) || IS_TRACE_DEV(devno) )
            /* 1 devi proc for trace */
            dev_pcb = KGS->first_dev + devno;
        else
            dev_pcb = en54_FindDevpcb ( this_ukt, devno, (tsp00_Bool) tcb->prio_flag, false );

        if ( dev_pcb->state != KT_INITIALIZED )
        {
#ifdef WANT_DATAWRITE_TIME_INFO
            SAPDB_UInt8 checkStartTime = 0;
            SAPDB_UInt8 checkEndTime = 0;
            if ( tcb->type == TT_DW_EO00 )
            {
                checkStartTime = RTESys_MicroSecTimer();
                MSGD(( INFO_TIME_START, tcb->index, "vvectorio(devio)", checkStartTime ));
            }
#endif
            DBG1 (( MF__,"send req 0x%x to devi\n", request ));

            this_ukt->wait_for_ioproc_io++;
            en54_RequestToDevi ( dev_pcb , request );

            /*******************************************************/
            /* Wait in dispatcher until devi Thread sends response */
            /********************************************************************/
            GOTO_DISP (&this_ukt);
            if ( tcb->TimeCollectionEnabled )
            {
                eo52UpdateTimeInStatisticRec( pIOStat->IOStat, tcb->TaskStateStat );
            }
            /********************************************************************/
#ifdef WANT_DATAWRITE_TIME_INFO
            if ( tcb->type == TT_DW_EO00 )
            {
                checkEndTime = RTESys_MicroSecTimer();
                MSGD(( INFO_TIME_STOP, tcb->index, "vvectorio(devio)", checkStartTime, checkEndTime, checkEndTime - checkStartTime ));
            }
#endif

            this_ukt->wait_for_ioproc_io--;

            if ( this_ukt->curr_quu != request )
            {
                MSGD ((IERR_VVECTORIO_WRONG_EVENT, (long) this_ukt->curr_quu ));
                MSGD ((IERR_VVECTORIO_WRONG_EVENT_CONT, (long) request ));
                vabort(WRITE_CORE);
            }

            DBG1 (( MF__,"returned from en54_DeviVVectorio, devpno = %d \n", devpno));

            if ( ! (ok = request->args.vve_parms.ok) )
            {
                errtext.c2p( errtab[request->args.vve_parms.errcode] );
                if ( ! KGS->dumpflag )
                {
                    MSGD ((ERR_VVECTORIO_SYSTEMERR, devno ));
                }
            }
            else
            {
                DBG1 (( MF__,"ok for T%d\n", taskId ));
            }
            /*
            * increase io counter
            */
            tcb->dirio++;
        }
    }

    DBG1 (( MF__,"returning, T%d \n", taskId ));

    SET_DEVSP_WAITING_FOR(tcb, undef_ceo00) ;

    tcb->state = TSK_RUNNING_EO00;

    return;
}/* vvectorio */

/*------------------------------*/

void vcopyvolume ( SAPDB_Int4      devnoSource,
                   SAPDB_Int4      devnoDestination,
                   tsp00_PageNo    firstPage,
                   SAPDB_Int4      pageCount,
                   tsp00_TaskId    taskId,
                   tsp00_ErrText & errtext,
                   tsp00_Bool    & ok )
{
# undef  MF__
# define MF__ MOD__"vcopyvolume"

    SAPDB_Int4 ulBuffPages;
    struct ALIGNED_MEM bufMem;
    tsp00_PageAddr  pBuffer;
    LONG       rc           = NO_ERROR_EO001;
    tsp00_PageNo currentPage = firstPage;
    SAPDB_Int4   pagesLeft   = pageCount;

    if ( pagesLeft <= 0 )
    {
        ok = true;
        DBGOUT;
        return;
    }

    ulBuffPages = XPARAM(ulDataIOBlockCount); // now DATA_IO_BLOCK_COUNT !

    if ( 0 == ulBuffPages ) ulBuffPages = 1;

    if ( (pBuffer = (tsp00_PageAddr)en54_AllocAligned( &bufMem, ulBuffPages * XPARAM(ulDBPageSize), NULL )) == NULL )
    {
        errtext.c2p( errtab[DEVIO_ERR_MALLOC] );
        ok = false;
        DBGOUT;
        return;
    }

    while ( pagesLeft > 0 )
    {
        while ( pagesLeft >= ulBuffPages )
        {
            vblockio( devnoSource,
                      currentPage,
                      sp2ioc_read,
                      pBuffer,
                      (SAPDB_Int2)ulBuffPages,
                      taskId,
                      errtext,
                      ok );
            if ( !ok )
            {
                pagesLeft = 0;
                break;
            }

            vblockio( devnoDestination,
                      currentPage,
                      sp2ioc_write,
                      pBuffer,
                      (SAPDB_Int2)ulBuffPages,
                      taskId,
                      errtext,
                      ok );
            if ( !ok )
            {
                pagesLeft = 0;
                break;
            }

            pagesLeft -= ulBuffPages;
            currentPage += ulBuffPages;
        }

        if ( pagesLeft > 0 )
        {
            vblockio( devnoSource,
                      currentPage,
                      sp2ioc_read,
                      pBuffer,
                      (SAPDB_Int2)pagesLeft,
                      taskId,
                      errtext,
                      ok );
            if ( !ok )
            {
                pagesLeft = 0;
                break;
            }

            vblockio( devnoDestination,
                      currentPage,
                      sp2ioc_write,
                      pBuffer,
                      (SAPDB_Int2)pagesLeft,
                      taskId,
                      errtext,
                      ok );
            if ( !ok )
            {
                pagesLeft = 0;
                break;
            }
            pagesLeft = 0;
        }
    }

    en54_FreeAligned( &bufMem );

    DBGOUT;
    return;
} /* vcopyvolume */

/* ================================================================== */
void vdualvectorio ( SAPDB_Int4      devno1,
                     SAPDB_Int4      devno2,
                     tsp00_PageNo    devpno,
                     tsp00_PageAddr  pageVector[],
                     SAPDB_Int2      pageCount,
                     tsp00_TaskId    taskId,
                     tsp00_ErrText & errtext1,
                     tsp00_Bool    & ok1,
                     tsp00_ErrText & errtext2,
                     tsp00_Bool    & ok2 )
{
#undef MF__
#define MF__ MOD__"vdualvectorio"
    struct TASK_TYPE                * tcb;
    ten50_UKT_Control               * this_ukt;
    REGISTER  struct DOUBLY_LINKED  * request1;
    REGISTER  struct DOUBLY_LINKED  * request2;
    struct DEV_PCB                  * dev_pcb1;
    struct DEV_PCB                  * dev_pcb2;
    tsp00_Bool                        io_finished = false ;
    SAPDB_Int                               rc ;
    teo52_TaskDevIOStatistic        * pIOStat ;
    SAPDB_Int                               io_counts = 2;

    DBGIN;

    PID_TCB(taskId, tcb);

    tcb->state = TSK_VDUALVECTORIO_EO00;

    this_ukt = tcb->ukt;

    DBG1 (( MF__,"T%d devno1 %d devno2 %d pno %d pageCount %d\n",
        taskId, devno1 , devno2, devpno , pageCount ));

    ok1 = true;
    ok2 = true;

    if ( pageCount <= 0 )
    {
        char buffer[EN54_SMALL_MESSAGE_BUFFER_SIZE];
        sp77sprintf(buffer, sizeof(buffer), "vdualvectorio without pages (%d)", pageCount );
        MSGD(( WRN_TEST_WILDCARD, &buffer[0] ));
        return;
    }

    /*----------------- check devno ----------------------- */
    if ( (devno1 < 0) || (devno1 >= KGS->maxdevspaces) )
    {
        MSGD ((IERR_VVECTORIO_INVALID_DEVNO ));
        errtext1.c2p( errtab[DEVIO_ERR_INVALID_DEVNO] );
        ok1 = false ;
    }
    else if ( devno2 < 0 || devno2 >= KGS->maxdevspaces )
    {
        MSGD ((IERR_VVECTORIO_INVALID_DEVNO ));
        errtext2.c2p( errtab[DEVIO_ERR_INVALID_DEVNO] );
        ok2 = false;
    }
    else if ( devno1 == devno2 )
    {
        MSGD ((IERR_VVECTORIO_INVALID_DEVNO ));
        errtext1.c2p( "Identical volume identifier" );
        ok1 = false ;
    }
    else if ( KGS->dev0.dev[devno1].fileSystemAccessType != KGS->dev0.dev[devno2].fileSystemAccessType )
    {
        MSGD ((IERR_VVECTORIO_INVALID_DEVNO ));
        errtext1.c2p( "Different file system access types" );
        ok1 = false ;
    }
    else if ( !ok1 || !ok2 )
    {
        DBGOUT_T (tcb->index);
        return;
    }

#ifdef HAVE_DAFS_SUPPORT
    if ( RTE_FileSystemAccessTypeDAFS == KGS->dev0.dev[devno1].fileSystemAccessType )
    {
        RTEIO_DAFS_vdualvectorio ( devno1,
                                   devno2,
                                   devpno,
                                   pageVector,
                                   pageCount,
                                   taskId,
                                   errtext1,
                                  &ok1,
                                   errtext2,
                                  &ok2 );
        return;
    }
#endif

    /*----------------- check volume -------------------- */
    dev_pcb1 = en54_FindDevpcb ( this_ukt, devno1, NO_PRIO, false );

    if ( KT_INITIALIZED == dev_pcb1->state )
    {
        errtext1.c2p( "Not attached" );
        MSGD ((IERR_VVECTORIO_NOT_ATTACHED1 ));
        ok1 = false;
    }

    dev_pcb2 = en54_FindDevpcb ( this_ukt, devno2, NO_PRIO, false );

    if ( KT_INITIALIZED == dev_pcb2->state )
    {
        errtext2.c2p( "Not attached" );
        MSGD ((IERR_VVECTORIO_NOT_ATTACHED2 ));
        ok2 = false;
    }

    /*----------------- IO statistics --------------------- */
    if ( tcb->type != TT_DW_EO00  &&         /* PTS 1001193 */
       ( KGS->dev0.dev[devno1].devtype == sp2dt_data ) )
    {
        if ( ++(KGS->TotalDataIOCnt) >= 0x7fffffff )
        {
            KGS->TotalDataIOCnt = 0 ;
        }
    }

/*----------------- try asyn I/O via DEVi ------------- */

    /* UNIX does not support asynchronous I/O using Scatter/Gather transfer... */

    request1                            = GetTaskDLQElement1( tcb );
    request1->taskid                    = tcb;
    request1->req_type                  = REQ_VVECTORIO_EO00;
    request1->args.vve_parms.devno      = devno1;
    request1->args.vve_parms.devpno     = devpno;
    request1->args.vve_parms.pageVector = pageVector;
    request1->args.vve_parms.pageCount  = pageCount;
    request1->args.vve_parms.ok         = false;

    tcb->dirio +=2;

    en54_RequestToDevi ( dev_pcb1 , request1 );

    request2                            = GetTaskDLQElement2( tcb );
    request2->taskid                    = tcb;
    request2->req_type                  = REQ_VVECTORIO_EO00;
    request2->args.vve_parms.devno      = devno2;
    request2->args.vve_parms.devpno     = devpno;
    request2->args.vve_parms.pageVector = pageVector;
    request2->args.vve_parms.pageCount  = pageCount;
    request2->args.vve_parms.ok         = false;

    en54_RequestToDevi ( dev_pcb2 , request2 );

    SET_DEVSP_WAITING_FOR(tcb, request1->args.vve_parms.devno) ;

    for ( ; io_counts; io_counts--)   /* receive and process the two replies */
    {
        eo52UpdateCountInIOStatisticRec(tcb->TaskStateStat.DevIOWriteStat, pageCount);
/********************************************************************/
        GOTO_DISP (&this_ukt);

        if ( tcb->TimeCollectionEnabled )
        {
            eo52UpdateTimeInStatisticRec( tcb->TaskStateStat.DevIOWriteStat.IOStat,
                                          tcb->TaskStateStat );
        }
/********************************************************************/

        if ( ! this_ukt->curr_quu->args.vve_parms.ok )
        {
            if (this_ukt->curr_quu == request1)
            {
                errtext1.c2p( errtab[this_ukt->curr_quu->args.vve_parms.errcode] );
                ok1 = false;
                MSGD ((ERR_VVECTORIO_SYSTEMERR, devno1));
            }
            else if (this_ukt->curr_quu == request2)
            {
                errtext1.c2p( errtab[this_ukt->curr_quu->args.vve_parms.errcode] );
                ok2 = false;
               MSGD ((ERR_VVECTORIO_SYSTEMERR, devno2));
            }
            else
            {
                MSGD ((IERR_VVECTORIO_WRONG_EVENT, (long) this_ukt->curr_quu ));
                MSGD ((IERR_VVECTORIO_WRONG_EVENT_CONT, (long) request1 ));
                MSGD ((IERR_VVECTORIO_WRONG_EVENT_CONT2, (long) request2 ));
                vabort(WRITE_CORE);
            }
        }
    }

    SET_DEVSP_WAITING_FOR(tcb, undef_ceo00) ;

    tcb->state = TSK_RUNNING_EO00;
    return;
} /* vdualvectorio */


/*
 * ==================================================================
 */
/* PTS CR 1103898 */
static  tsp00_Bool en54_TouchVolumeIfNotExists ( char           * volume,
                                                  tsp00_Bool    * pForceReadOnly,
                                                  tsp00_ErrText & errtext )
{
    struct stat buf;

    if ( -1 != RTE_save_stat((char *)volume, &buf) )
    {
        return true;
    }

    if ( ENOENT == errno) /* volume does not exists ? */
    {
        if ( !ResetForcedReadOnlyIfHSDefaultMaster(pForceReadOnly) )
        {
            errtext.c2p( "Cannot touch readonly volume" );
            MSGD ((ERR_TOUCH_DEVSPACE, volume, "Missing LogVolumeHotStandby" ));
            return (false);
        }
        else
        {
            SAPDB_Int tmp_fd = RTE_save_openCreate ( (char *)volume,DEFAULT_OPEN_MODE_EN54|O_RDWR|O_CREAT , 0660 );
            if ( tmp_fd >= 0 )
            {
                if ( 0 == RTE_save_close(tmp_fd) )
                {
                    if ( !RTE_CSetDatabasePathOwnership( volume ) )
                    {
                        errtext.c2p( "Cannot set support group for volume" );
                        MSGD ((WRN_TOUCH_DEVSPACE, volume, "Cannot set support group for volume" ));
                    }
                    return true;
                }
            }
        }
    }

    errtext.c2p( "Error during touch" );
    MSGD ((ERR_TOUCH_DEVSPACE, volume, sqlerrs() ));
    return(false);
}

/*
 * ==================================================================
 */
/* CR 1115397 */
/*
  Function: WriteIntoPipe
  Description: This is the routine a child process executes during testing pipe algorithms

  The test pipe path is given as well as a test file path.
  The pipe is opened for write and a single testmessage is written (short enaugh to be written in a
  single piece. Now it is waiting until the test file is unlinked by the master process.

  Arguments: fifoPath [in] path to pipe
             testPath [in] path to test file
 */
void WriteIntoPipe(tsp00_Pathc &fifoPath, tsp00_Pathc &testPath)
{
    SAPDB_Int fifoFd;
    long bytesWritten;

    /* Block until opened for read from other side */
    fifoFd = RTE_save_open(fifoPath, O_WRONLY);
    if ( fifoFd < 0 )
    {
        MSGD (( ERR_PIPECHECK_OPEN, fifoPath.asCharp(), sqlerrs() ));
    }
    else
    {
        SAPDB_Int testFile;

        bytesWritten = en54_SignalSave_write(fifoFd, (char *)EN54_TEST_PIPE_CONTENT, strlen(EN54_TEST_PIPE_CONTENT));
        if ( bytesWritten != strlen(EN54_TEST_PIPE_CONTENT) )
        {
            MSGD (( ERR_PIPECHECK_WRITE, fifoPath.asCharp() ));
        }

        KGS->pipeCheckWriteDone = true;

        /* Wait until master unlinks testfile */
        do
        {
            testFile = RTE_save_open(testPath, O_RDONLY);
            if ( testFile >= 0 )
            {
                RTE_save_close(testFile);
            }
            else
            {
                sleep(0);
            }
        } while ( testFile >= 0 );

        RTE_save_close(fifoFd);
    }
}

/*-------------------------------*/
/* PTS 1115397 */
/*
  Function: en54FindPipeCheckAlgorithm
  Description: Test well known methods to check for input on pipes without blocking

  The code is needed since operating system releases may change internal behaviour. This happened with LINUX 2.2.x -> 2.4.x

  The test pipe is written to by a client process, which is terminated using a test file. Both path names are created unique
  using the database name.

  fstat expects to have st_size element containing the number of bytes stored in pipe

  poll expects to report input pending in pipe

  Arguments: dbname [in] database name used for unique file names
 */
void en54FindPipeCheckAlgorithm(char *dbname)
{
#undef MF__
#define MF__ MOD__"en54FindPipeCheckAlgorithm"
    tsp00_Pathc fifoPath;
    tsp00_Pathc testPath;

    DBGIN;

    KGS->pipeCheckWriteDone = false;

    sql41_get_fifo_dir( fifoPath );
    sql41_get_fifo_dir( testPath );

    if ( (strlen(fifoPath)+strlen(EN54_TEST_PIPE_NAME)+strlen(dbname)) > sizeof(tsp00_Path) )
    {
        MSGD (( ERR_PIPECHECK_PATH, fifoPath.asCharp(), EN54_TEST_PIPE_NAME ));
    }
    else
    {
        SAPDB_Int testFile;

        strcat(fifoPath, EN54_TEST_PIPE_NAME);
        strcat(fifoPath, dbname);

        strcat(testPath, EN54_TEST_DONE_FILE);
        strcat(testPath, dbname);

        testFile = RTE_save_openCreate(testPath, O_CREAT|O_RDWR, 0666);
        if ( testFile < 0 )
        {
            MSGD (( ERR_PIPECHECK_CREATE, testPath.asCharp(), sqlerrs() ));
        }
        else
        {
            RTE_save_close(testFile);
        }

        if ( testFile >= 0 )
        {
            if ( sql41_create_fifo(fifoPath, 0666) )
            {
                MSGD (( ERR_PIPECHECK_CREATE, fifoPath.asCharp(), sqlerrs() ));
            }
            else
            {
                SAPDB_Int4 childPid = RTE_save_fork();
                if ( childPid == 0 )
                {
                    WriteIntoPipe(fifoPath, testPath);
                    exit(0);
                }
                else if ( childPid == -1 )
                {
                    MSGD(( ERR_FORK_ERR, sqlerrs() ));
                }
                else
                {
                    SAPDB_Int rc;
                    SAPDB_Int fifoFd;
                    struct stat statBuf;
                    char inputByte;
                    SAPDB_Int supportFstat = 0;
                    SAPDB_Int supportPoll = 0;
#define MAX_PIPE_CHECK_WAIT_TIME_EN54 30
                    SAPDB_Int maxRetries = MAX_PIPE_CHECK_WAIT_TIME_EN54;

                    fifoFd = RTE_save_open(fifoPath, O_NONBLOCK|O_RDONLY);
                    if ( fifoFd < 0 )
                    {
                        MSGD (( ERR_PIPECHECK_REOPEN, fifoPath.asCharp(), sqlerrs() ));
                    }

                    /* PTS 1117651 */
                    while ( --maxRetries > 0
                         && !KGS->pipeCheckWriteDone )
                    {
                        /* give write some time to write... */
                        sleep(1);
                    }

                    if ( maxRetries > 0 )
                    {
                        rc = RTE_save_fstat(fifoFd, &statBuf);
                        supportFstat = ( (0 == rc) && (statBuf.st_size == strlen(EN54_TEST_PIPE_CONTENT)) );
#ifdef DEBUG_PIPE_CODE
                        if ( !supportFstat )
                        {
                            if ( 0 != rc )
                            {
                                DBG1 (( MF__, "First fstat failed\n" ));
                            }
                            else
                            {
                                DBG1 (( MF__, "First fstat reported no input\n" ));
                            }
                        }
#endif

                        {
                            struct pollfd pipeIn[1];

                            pipeIn[0].fd = fifoFd;
                            pipeIn[0].events = POLLIN;
                            pipeIn[0].revents = 0;

                            rc = RTE_save_poll(&pipeIn[0], 1, 0);
                            supportPoll = ( ( 1 == rc ) && ((pipeIn[0].revents & POLLIN) != 0) );
                        }
#ifdef DEBUG_PIPE_CODE
                        if ( !supportPoll )
                        {
                            if ( 1 != rc )
                            {
                                DBG1 (( MF__, "First poll failed\n" ));
                            }
                            else
                            {
                                DBG1 (( MF__, "First poll reported no input\n" ));
                            }
                        }
#endif

                        /* empty pipe */
                        while(RTE_save_nonblocking_read(fifoFd, &inputByte, 1) == 1 )
                            ;

                        if ( supportFstat )
                        {
                            rc = RTE_save_fstat(fifoFd, &statBuf);
                            supportFstat = ( (0 == rc) && (statBuf.st_size == 0) );
#ifdef DEBUG_PIPE_CODE
                            if ( !supportFstat )
                            {
                                if ( 0 != rc )
                                {
                                    DBG1 (( MF__, "Second fstat failed\n" ));
                                }
                                else
                                {
                                    DBG1 (( MF__, "Second fstat reported input\n" ));
                                }
                            }
#endif
                        }

                        if ( supportPoll )
                        {
                            struct pollfd pipeIn[1];

                            pipeIn[0].fd = fifoFd;
                            pipeIn[0].events = POLLIN;
                            pipeIn[0].revents = 0;

                            rc = RTE_save_poll(&pipeIn[0], 1, 0);

                            supportPoll =     (0 == rc)
                                         || ( (1 == rc) && ((pipeIn[0].revents & POLLIN) == 0) );
#ifdef DEBUG_PIPE_CODE
                            if ( !supportPoll )
                            {
                                if ( 1 != rc )
                                {
                                    DBG1 (( MF__, "Second poll failed\n" ));
                                }
                                else
                                {
                                    DBG1 (( MF__, "Second poll reported input\n" ));
                                }
                            }
#endif
                        }

                        if ( supportFstat )
                        {
                            MSGD (( INFO_PIPECHECK_FSTAT_CODE_SUPPORTED ));
                        }
                        else
                        {
                            MSGD (( INFO_PIPECHECK_FSTAT_CODE_NOT_SUPPORTED ));
                        }

                    }
                    else
                    {
                        MSGD (( INFO_PIPECHECK_TIMEOUT, MAX_PIPE_CHECK_WAIT_TIME_EN54 ));
                    }

                    if ( supportPoll )
                    {
                        MSGD (( INFO_PIPECHECK_POLL_CODE_SUPPORTED ));
                    }
                    else
                    {
                        MSGD (( INFO_PIPECHECK_POLL_CODE_NOT_SUPPORTED ));
                    }

                    if ( supportFstat )
                    {
                        en54_pipeCheckAlgorithm = EN54_FSTAT_PIPE_CHECK_CODE_OK;
                    } else if ( supportPoll )
                    {
                        en54_pipeCheckAlgorithm = EN54_POLL_PIPE_CHECK_CODE_OK;
                    } else
                    {
                        en54_pipeCheckAlgorithm = EN54_NO_PIPE_CHECK_CODE_WORKING;
                        MSGD (( WRN_PIPECHECK_INPUT_CHECK_NOT_SUPPORTED ));
                    }

                    /* signalize write process allowed to terminate */
                    if ( RTE_save_unlink(testPath) != 0 )
                    {
                        /* if unlink fails, we kill child process, otherwise later on waitpid would block... */
                        RTE_save_kill(childPid, SIGKILL);
                    }

                    RTE_save_close(fifoFd);

                    /* wait for child, to prevent zombie */
                    while ( RTE_save_waitpid(childPid, (SAPDB_Int *)0, 0) != childPid )
                        ;
                }
            }
            sql41_remove_fifo(fifoPath);
        }
    }

    DBGOUT;
}

/*-----------------------------------------*/
static SAPDB_Int en54_AddSynchronousOpen()
{
    /* If O_DIRECT shows up to be a performance win, it can be added. */
    static SAPDB_Int useOpenDirect = -1;
    static SAPDB_Int useSync = 1;
    if ( useOpenDirect < 0 ) /* only executed once... */
    {
        SAPDB_Char *paramValue;
        SAPDB_Char *errText;
        if ( RTEConf_ParameterAccessGetString((SAPDB_UTF8 *)"NO_SYNC_TO_DISK_WANTED", &paramValue, &errText) )
        {
            useSync = ( strcmp("YES", paramValue) == 0 ? 0 : 1 );
        }
#if defined(SAPDB_DIRECTIO)
        if ( RTEConf_ParameterAccessGetString((SAPDB_UTF8 *)PAN_USE_OPEN_DIRECT, &paramValue, &errText) )
        {
            useOpenDirect = ( strcmp("YES", paramValue) == 0 ? 1 : 0 );
        }
        else
        {
            useOpenDirect = 0;
        }
#else
        useOpenDirect = 0;
#endif

#if defined(SAPDB_DIRECTIO)
        if ( 0 == useOpenDirect )
        {
#endif
            switch( XPARAM(simulateVectorIo) )
            {
            case RTE_SimVioIfOpenDirect:
                SAPDB_strcpy(XPARAM(szVectorIoMode), "NEVER");
                XPARAM(simulateVectorIo) = RTE_SimVioNever;
                break;
            case RTE_SimVioIfOpenDirectOrRawDevice:
                SAPDB_strcpy(XPARAM(szVectorIoMode), "IF_RAW_DEVICE");
                XPARAM(simulateVectorIo) = RTE_SimVioIfRawDevice;
                break;
            default:
                break;
            }
#if defined(SAPDB_DIRECTIO)
        }
#endif
    }

#if defined(SAPDB_DIRECTIO)
    if ( 1 == useOpenDirect )
    {
        if ( useSync )
        {
            return SAPDB_DIRECTIO | SAPDB_SYNC_DATA_TO_DISK;
        }
        else
        {
            return SAPDB_DIRECTIO;
        }
    }
    else
#endif
    {
        if ( useSync )
        {
            return SAPDB_SYNC_DATA_TO_DISK;
        }
        else
        {
            return 0;
        }
    }
}


/* ==================================================================

  It took me some time to understand what happens here, so it is good to comment it ;-) jrg 21.2.2003

  With each vdetach the KGS->kgs_detach_count is incremented (spinlock protected). In the same
  locked code, the device path name is reset (KGS->dev0.dev[devno].devspace[0] == 0).
  But... the open file descriptors of previous single_io ARE NOT CLOSED!!!! A 'vdetach' on
  UNIX is therefore incomplete...

  The next call of e54_init_self_io() now compares the 'this_ukt->ukp_detach_count' with the KGS
  counter 'KGS->kgs_detach_count' and closes ALL filedescriptors for single_io if a mismatch exists.
  After that it copies the value of the counter...

  This code works only somewhat performant, because there is almost no dynamic attach/detach activity
  in the system (better do not think about backups...)

  The NT RunTime approach is completely different. The vdetach sets a 'ulDetachPending', which is used
  by subsequent vblockio() to inform 'NOT ATTACHED' (with a small chance to pass over this check, before
  the I/O is on its way...) and closes all open file handles. Since there is no synchronization a small
  unprotected code path still exist, which has a smaöll likelihood to produce 'I/O error due to invalid
  handles...

  ---> Changes

  NT behaviour can be modified, to contain no time critical code path. The vdetach already synchronizes
  for all other I/O requests, to be completed. In addition to that, the single_io activity will be
  synchronized to indicate its completion via a reference counter. This reference counter is monitored
  and vdetach is delayed until it reaches zero. This way no I/O error can occure due to an invalid handle.

  Why this additional synchronization? The HotStandby code uses vdetach() followed by vattach() to do
  the transition from STANDBY mode to ONLINE mode (becoming master...). This is needded, since the STANDBY
  role implied readonly access to the log volumes, while the MASTER role needs read and write access...

  The UNIX coding is modified, to use the same conventions as the NT code. This means to really close the open
  files if they exist and to add the same synchronization means...

  ukp_detach_count and kgs_detach_count removed...
 */

static void en54_CloseSingleIOFileDesc (
    ten50_UKT_Control * pTmpUKTCtrl,
    SAPDB_Int4          lDevspaceNo )
{
    int attachedFd = pTmpUKTCtrl->pSingleIO[lDevspaceNo].ukp_attached_fd;
    pTmpUKTCtrl->pSingleIO[lDevspaceNo].ukp_attached_fd = -1;

    if ( -1 != attachedFd && (RTE_save_close(attachedFd) == -1) )
    {
        int lastErrno = errno;
        char badFileDescriptor[80];
        sp77sprintf(badFileDescriptor, 80, "Bad file descriptor %d!", attachedFd);

        MSGD ((ERR_CLOSESINGLE_ERR_CLOSE, &badFileDescriptor[0] ));
        errno = lastErrno;
        MSGD ((ERR_CLOSESINGLE_ERR_CLOSE, sqlerrs() ));
    }
}


 /*!
  @brief This call synchronizes with pending self I/O activity on devno
  The UKT owns a device specific information structure which includes the
  file descriptor, the self I/O statistic and the self I/O pending flag.

  The routine does not block forever, but waits only for maximum of
  SAPDB_MAXIMUM_SELF_IO_YIELD_WAIT seconds for self i/o to complete. If it
  did not complete in time, a warning message is issued, and the self I/O
  file descriptor is closed anyhow.

  The routine returns after all UKTs have been parsed and all self I/O activity
  is known to be completed. After the routine returned, all file handles for self
  I/O on the given device are closed.

  The selfIOPending flag must be seen in combination with the 'vdetach' pending flag.
  The self I/O code checks the

  @param devno [in] the device number to check
 */
static void en54_WaitForSelfIOCompletionAndClose(SAPDB_Int4 lDevspaceNo)
{
#  undef  MF__
#  define MF__  MOD__"en54_WaitForSelfIOCompletionAndClose"
    ten50_UKT_Control         * pTmpUKTCtrl;

    time_t yieldStartTime = KGS->current_time;
    
    /* --- close all single io file of the current UKT */
    for ( pTmpUKTCtrl = KGS->first_kp;
          pTmpUKTCtrl <= KGS->last_kp;
          pTmpUKTCtrl++ )
    {
        struct UKP_IO_STRUCT *pSingleIOCtrl = &pTmpUKTCtrl->pSingleIO[lDevspaceNo];
        SAPDB_Int4 ulSelfIOPending;

        do
        {
            WAIT_UNTIL_ALONE( pTmpUKTCtrl->exclusive );
                ulSelfIOPending = pTmpUKTCtrl->ulSelfIOPending;
            CLEARLOCK( pTmpUKTCtrl->exclusive );

            if ( ulSelfIOPending )
            {
                    time_t now = KGS->current_time;
                    if ( yieldStartTime <= now
                      && (yieldStartTime + MAX_VDETACH_WAIT_TIME) < now )
                    {
                        sqlyieldthread();
                        continue; /* still inside yield time interval... yield again */
                    }
                    else
                    {
                        MSGD(( WRN_VDETACH_IGNORE_SELF_IO, /* "Ignoring pending self io in UKT%d on device %d" */
                               pTmpUKTCtrl->index,
                               lDevspaceNo ));
                        ulSelfIOPending = false;
                    }
            }
        } while ( ulSelfIOPending );

        pSingleIOCtrl->selfIOStatisticResetPending = true;
        en54_CloseSingleIOFileDesc(pTmpUKTCtrl, lDevspaceNo);
    }
}

/*------------------------------------*/

static tsp00_Bool
ResetForcedReadOnlyIfHSDefaultMaster(tsp00_Bool *pIsForcedReadOnly)
{
    if ( *pIsForcedReadOnly )
    {
        if ( KGS->isHotStandbyConfiguration )
        {
            if ( RTEHSS_IsDefaultMaster() )
            {
                if ( RTEHSS_SetRoleToMaster() )
                {
                    *pIsForcedReadOnly = false;
                }
            }
        }
    }
    return !(*pIsForcedReadOnly); /* if it is or no longer is read only, return true */
}

/*------------------------------------*/

static off_t en54_SignalSave_lseek(int fd, off_t where, int whence)
{
    off_t result;
    result = RTE_save_lseek(fd, where, whence);
    if ( result == (off_t)-1 )
    {
        int lastErrno = errno;
        MSGD(( ERR_DEVIO_ERR_SEEK_FD, fd, sqlerrs() ));
        errno = lastErrno;
    }
    return result;
}

/*------------------------------------*/

static long en54_SignalSave_read(int fd, void *buffer, size_t size)
{
    long result;
    result = RTE_save_read(fd, buffer, size);
    if ( result == -1 )
    {
        int lastErrno = errno;
        MSGD(( ERR_DEVIO_ERR_READ_FD, fd, sqlerrs() ));
        errno = lastErrno;
    }
    else if ( result != size )
    {
        MSGD(( ERR_DEVIO_ERR_READ_FD_EOF, fd, result, size ));
    }
    return result;
}

/*------------------------------------*/

static long en54_SignalSave_read_partial(int fd, void *buffer, size_t size)
{
    long result;
    result = RTE_save_read(fd, buffer, size);
    if ( result == -1 )
    {
        int lastErrno = errno;
        MSGD(( ERR_DEVIO_ERR_READ_FD, fd, sqlerrs() ));
        errno = lastErrno;
    }
    return result;
}

/*------------------------------------*/

static long en54_SignalSave_write(int fd, void *buffer, size_t size)
{
    char *partialBuffer = (char *)buffer;
    size_t bytesWritten = 0;
    int lastErrno = errno;
    long result;
    errno = 0;
    do
    {
        result = RTE_save_write(fd, partialBuffer + bytesWritten, size - bytesWritten);
        if ( result > 0 )
        {
            bytesWritten += result;
        }
        else
        {
            lastErrno = errno;
            if ( result < 0 )
            {
                MSGD(( ERR_DEVIO_ERR_WRITE_FD, fd, sqlerrs() ));
                errno = lastErrno;
            }
            else
            {
                MSGD(( ERR_DEVIO_ERR_WRITE_FD_DISK_FULL, fd, result, size ));
                errno = lastErrno;
            }
            return result;
        }
    } while ( bytesWritten < size );

    return bytesWritten;
}

/*----------------------------------------*/

static SAPDB_Bool en54_CreateSingleIOWorker(struct DEV_PCB *devi_pcb)
{
    int rc;
    tsp00_ErrTextc             threadErrtext;
    teo07_ThreadErr            err;

    rc = sql41_create_sem ( 0, 0, 0, 0660, 0, (char *)"db", (char *) KGS->serverdb );
	if ( -1 == rc )
    {
        MSGD (( ERR_DEV0_VATTACH_ERR_FORK, "Out of IPC semaphores for thread synchronization..." ));
        return false;
    }
    else
    {
        devi_pcb->semid = rc;

#if defined(LINUX) && !defined(HAS_NPTL)
        sqlbeginthread( 0, /* PTS 1106187 */
#else
    	sqlbeginthread( KT_DEVI_STACK_SIZE,
#endif
                        en54_Mdevi_main,
			            devi_pcb,
			            THR_CREATE_SUSPENDED_EO07,
			            & devi_pcb->thread,
			            threadErrtext,
			            &err );
	    if ( err == THR_OK_EO07 )
	    {
		    devi_pcb->tid = sqlthreadid( devi_pcb->thread );
	    }
	    else
	    {
	    	MSGD ((ERR_DEV0_VATTACH_ERR_FORK, &threadErrtext[0] ));
            return false;
        }
	}

    return true;
}

/*------------------------------------*/

externC int en54CreateIOWorkerThreads()
{
    tsp00_ErrTextc             threadErrtext;
    teo07_ThreadErr            err;
    /*
     *  Create DEV0  process
     */
    sqlbeginthread( KT_DEV0_STACK_SIZE,
                    en54_Mdevio,
                    NULL,
					THR_CREATE_SUSPENDED_EO07,
                    & KGS->dev0.thread,
                    threadErrtext,
                    &err );
    if ( err == THR_OK_EO07 )
    {
        KGS->dev0.tid = sqlthreadid(KGS->dev0.thread);
    }
    else
    {
        MSGCD (( ERR_CREATING_IOTHREAD , "DEV0", threadErrtext.asCharp() )) ;
        return ( 3 );
    }
	sqlresumethread( KGS->dev0.thread, threadErrtext, &err );
    if ( err != THR_OK_EO07 )
    {
        MSGCD (( ERR_RESUME_IOTHREAD, "DEV0", threadErrtext.asCharp() ));
    }

    /* Create worker thread for tasks */

    if ( XPARAM(preallocateIOWorker) )
    {
        char msgBuffer[512];
        struct DEV_PCB *devi_pcb;
        int iDev;

        sprintf(msgBuffer, "Creating resources for (%d volumes * %d worker per volume + %d backup volumes + %d backup device(s) = %d) IO worker threads", 
                KGS->maxdevspaces, KGS->devpsets, KGS->maxdevspaces, KGS->maxtapes,
                ( KGS->maxdevspaces * KGS->devpsets ) + (KGS->maxdevspaces + KGS->maxtapes));

        MSGD (( INFO_PROCESS_STARTED, msgBuffer ));

        for ( iDev = 0; iDev < (KGS->maxdevspaces * KGS->devpsets) ; iDev++ )
        {
            devi_pcb = KGS->first_dev + iDev;
            if ( !en54_CreateSingleIOWorker(devi_pcb) )
            {

                sprintf(msgBuffer, "Created %d IO worker, configured are %d worker!", 
                    iDev, (KGS->maxdevspaces * KGS->devpsets) + (KGS->maxdevspaces + KGS->maxtapes));

                MSGD (( ERR_DEV0_VATTACH_ERR_FORK, "Out of volume IO worker thread resources." ));
                MSGD (( ERR_DEV0_VATTACH_ERR_FORK, msgBuffer ));
                MSGD (( ERR_DEV0_VATTACH_ERR_FORK, "Reduce MAXLOGVOLUMES,MAXDATAVOLUMES,_IOPROCS_PER_DEV or CACHE_SIZE." ));
                return 3;
            }
        }

        for ( iDev = 0; iDev < (KGS->maxdevspaces + KGS->maxtapes) ; iDev++ )
        {
            devi_pcb = KGS->first_asdev + iDev;
            if ( !en54_CreateSingleIOWorker(devi_pcb) )
            {
                sprintf(msgBuffer, "Created %d IO worker, configured are %d worker!", 
                    (KGS->maxdevspaces * KGS->devpsets) + iDev, (KGS->maxdevspaces * KGS->devpsets) + (KGS->maxdevspaces + KGS->maxtapes));

                MSGD (( ERR_DEV0_VATTACH_ERR_FORK, "Out of backup IO worker thread resources." ));
                MSGD (( ERR_DEV0_VATTACH_ERR_FORK, msgBuffer ));
                MSGD (( ERR_DEV0_VATTACH_ERR_FORK, "Reduce MAXLOGVOLUMES,MAXDATAVOLUMES,_IOPROCS_PER_DEV,MAXBACKUPDEVS or CACHE_SIZE." ));
                return 3;
            }
        }

    }

    return 0;
}
