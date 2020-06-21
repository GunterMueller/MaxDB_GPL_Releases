/*!
  @file           vos65k.cpp
  @author         RaymondR
  @brief          vasyn I/O
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
#include "gos41.h"
#include "heo00.h"
#include "heo46.h"
#include "geo007_1.h"
#include "geo007_2.h"
#include "geo50_0.h"
#include "geo002.h"
#include "heo52.h"
#include "gos44.h"
#include "geo54.h"
#include "geo00_1.h"
#include "gos74.h"
#include "RunTime/Tasking/RTETask_LegacyTaskCtrl.h"
#include "RunTime/Tasking/RTETask_CommonQueueElement.hpp"


#if defined(_FASTCAP)
# include "fastcap.h"   /* nocheck */
#endif

/*
 *  DEFINES
 */
#define MOD__  "VOS65KC : "
#define MF__   MOD__"UNDEFINED"

#define MAX_VASYNOPEN_WAIT_TIME                  60      // -- sleep time
#define MAX_ASYNCLOSE_WAIT_TIME                  240     // -- sleep time

#define NUMBER_OF_IO_RETRIES                     3
#define TAPE_ERROR_RETRY                         5
#define MIN_TAPE_BLOCK_SIZE                      512
#define WATCH_DOG_SLEEP_TIME                     250

#define LOCK_TAPE                                1
#define UNLOCK_TAPE                              2



#define ERRCODE_ASYNCIO_ALRATT                   0
#define ERRCODE_ASYNCIO_MAXDEVICES               1
#define ERRCODE_ASYNCIO_ASYNCI_CREATE            2
#define ERRCODE_ASYNCIO_ASYNC0_NOT_ACTIVE        3
#define ERRCODE_ASYNCIO_ACCESS_DENIED            4
#define ERRCODE_ASYNCIO_WRONG_PATH               5
#define ERRCODE_ASYNCIO_GET_FILE_INFO            6
#define ERRCODE_ASYNCIO_IO_ON_DIR                7
#define ERRCODE_ASYNCIO_IO_ON_SYS                8
#define ERRCODE_ASYNCIO_OPEN_FILE_DEV_PIPE       9
#define ERRCODE_ASYNCIO_WRONG_DEVICE_TYPE        10
#define ERRCODE_ASYNCIO_CLOSE_FILE_DEV_PIPE      11
#define ERRCODE_ASYNCIO_CLOSE_EVENT_SEM          12
#define ERRCODE_ASYNCIO_ASYNCI_REMOVE            13
#define ERRCODE_ASYNCIO_FILE_DEV_PIPE_NOT_OPEN   14
#define ERRCODE_ASYNCIO_WRONG_DEVICENO           15
#define ERRCODE_ASYNCIO_NO_REQ_ACTIVE            16
#define ERRCODE_ASYNCIO_CANT_SEEK_FILE_DEV_PIPE  17
#define ERRCODE_ASYNCIO_CANT_READ_FILE_DEV_PIPE  18
#define ERRCODE_ASYNCIO_CANT_WRITE_FILE_DEV_PIPE 19
#define ERRCODE_ASYNCIO_END_OF_FILE_DEV_PIPE     20
#define ERRCODE_ASYNCIO_ALLOC_MEM                21
#define ERRCODE_ASYNCIO_FREE_MEM                 22
#define ERRCODE_ASYNCIO_WRONG_BLOCK_SIZE         23
#define ERRCODE_ASYNCIO_WRONG_FILE_ATTRIB        24
#define ERRCODE_ASYNCIO_MEDIA_WRITE_PROTECTED    25
#define ERRCODE_ASYNCIO_DEVICE_NOT_READY         26
#define ERRCODE_ASYNCIO_END_OF_MEDIA             27
#define ERRCODE_ASYNCIO_FILE_SET_MARK            28
#define ERRCODE_ASYNCIO_MEDIA_NOT_PATITIONED     29
#define ERRCODE_ASYNCIO_NO_MEDIA_IN_DEVICE       30
#define ERRCODE_ASYNCIO_MEDIA_CHANGED            31
#define ERRCODE_ASYNCIO_BAD_LENGTH_READ_WRITTEN  32
#define ERRCODE_ASYNCIO_GET_TAPE_DRIVE_INFO      33
#define ERRCODE_ASYNCIO_GET_TAPE_MEDIA_INFO      34
#define ERRCODE_ASYNCIO_SET_TAPE_DRIVE_INFO      35
#define ERRCODE_ASYNCIO_SET_TAPE_MEDIA_INFO      36
#define ERRCODE_ASYNCIO_REWIND_TAPE              37
#define ERRCODE_ASYNCIO_LOCK_UNLOCK_TAPE         38
#define ERRCODE_ASYNCIO_WRITE_TAPE_MARK          39
#define ERRCODE_ASYNCIO_CRC_ERROR                40
#define ERRCODE_ASYNCIO_SHARING_VIOLATION        41
#define ERRCODE_ASYNCIO_WRONG_FILE_DEVICE_NAME   42
#define ERRCODE_ASYNCIO_DEVICE_BUSY              43
#define ERRCODE_ASYNCIO_UNRECOGNIZED_MEDIA       44
#define ERRCODE_ASYNCIO_TENSION_TAPE             45
#define ERRCODE_ASYNCIO_DISK_FULL                46
#define ERRCODE_ASYNCIO_LOAD_UNLOAD_TAPE         47
#define ERRCODE_ASYNCIO_ILL_BLOCK_COUNT          48
#define ERRCODE_ASYNCIO_IO_READ_ONLY             49
#define ERRCODE_ASYNCIO_PIPE_CONN_CANCELLED      50
#define ERRCODE_ASYNCIO_NO_DATA_DETECTED         51
#define ERRCODE_ASYNCIO_OPERATION_CANCELED       52
#define ERRCODE_ASYNCIO_INVALID_BLOCKSIZE        53
// !!!!!!!!!! don't forget to alter MAX_ASYNCIO_ERRCODE  !!!!!!!!!!
#define MAX_ASYNCIO_ERRCODE                      54
// !!!!!!!!!! don't forget to alter this value --^  !!!!!!!!!!!!!!!



#define REQ_TO_ASYNCI           __sql65k_req_to_asynci
#define REQ_FROM_ASYNCx_TO_UKT  __sql65k_req_from_asyncx_to_ukt
#define REQ_TO_ASYNC0           __sql65k_req_to_async0

#define NUM_OF_SHARING_VIOLATION_RETRIES   3


// --- ASYNC_IO ------- VA_RETURN ---------------------------------------------

/*
 *  MACROS
 */
#define GET_SHFT_OPERATOR(_Shft,_BlkSze)                                      \
          { INT   _i;                                                         \
            ULONG _ulBlkSze = _BlkSze;                                        \
            for (_i = 0, *_Shft = (ULONG)UNDEF; _i < 32;                      \
                 _i++, _ulBlkSze >>= 1)                                       \
              {                                                               \
              if (_ulBlkSze & 1)                                              \
                {                                                             \
                if(*_Shft != (ULONG)UNDEF)                                    \
                  {                                                           \
                  *_Shft = (ULONG)UNDEF;                                      \
                  break;                                                      \
                  }                                                           \
                else                                                          \
                  *_Shft = _i;                                                \
                }                                                             \
              }                                                               \
          }

/*
 *  LOCAL TYPE AND STRUCT DEFINITIONS
 */


/*
 * EXTERNAL VARIABLES
 */


/*
 *  EXPORTED VARIABLES
 */


/*
 * LOCAL VARIABLES
 */
static const char* ErrTab[ MAX_ASYNCIO_ERRCODE ] =
  {
  ERRMSG_ASYNCIO_ALRATT,                  // - ERRCODE_ASYNCIO_ALRATT
  ERRMSG_ASYNCIO_MAXDEVICES,              // - ERRCODE_ASYNCIO_MAXDEVICES
  ERRMSG_ASYNCIO_ASYNCI_CREATE,           // - ERRCODE_ASYNCIO_ASYNCI_CREATE
  ERRMSG_ASYNCIO_ASYNC0_NOT_ACTIVE,       // - ERRCODE_ASYNCIO_ASYNC0_NOT_ACTIVE
  ERRMSG_ASYNCIO_ACCESS_DENIED,           // - ERRCODE_ASYNCIO_ACCESS_DENIED
  ERRMSG_ASYNCIO_WRONG_PATH,              // - ERRCODE_ASYNCIO_WRONG_PATH
  ERRMSG_ASYNCIO_GET_FILE_INFO,           // - ERRCODE_ASYNCIO_GET_FILE_INFO
  ERRMSG_ASYNCIO_IO_ON_DIR,               // - ERRCODE_ASYNCIO_IO_ON_DIR
  ERRMSG_ASYNCIO_IO_ON_SYS,               // - ERRCODE_ASYNCIO_IO_ON_SYS
  ERRMSG_ASYNCIO_OPEN_FILE_DEV_PIPE,      // - ERRCODE_ASYNCIO_OPEN_FILE_DEV_PIPE
  ERRMSG_ASYNCIO_WRONG_DEVICE_TYPE,       // - ERRCODE_ASYNCIO_WRONG_DEVICE_TYPE
  ERRMSG_ASYNCIO_CLOSE_FILE_DEV_PIPE,     // - ERRCODE_ASYNCIO_CLOSE_FILE_DEVICE
  ERRMSG_ASYNCIO_CLOSE_EVENT_SEM,         // - ERRCODE_ASYNCIO_CLOSE_EVENT_SEM
  ERRMSG_ASYNCIO_ASYNCI_REMOVE,           // - ERRCODE_ASYNCIO_ASYNCI_REMOVE
  ERRMSG_ASYNCIO_FILE_DEV_PIPE_NOT_OPEN,  // - ERRCODE_ASYNCIO_FILE_DEV_PIPE_NOT_OPEN
  ERRMSG_ASYNCIO_WRONG_DEVICENO,          // - ERRCODE_ASYNCIO_WRONG_DEVICENO
  ERRMSG_ASYNCIO_NO_REQ_ACTIVE,           // - ERRCODE_ASYNCIO_NO_REQ_ACTIVE
  ERRMSG_ASYNCIO_CANT_SEEK_FILE_DEV_PIPE, // - ERRCODE_ASYNCIO_CANT_SEEK_FILE_DEV_PIPE
  ERRMSG_ASYNCIO_CANT_READ_FILE_DEV_PIPE, // - ERRCODE_ASYNCIO_CANT_READ_FILE_DEV_PIPE
  ERRMSG_ASYNCIO_CANT_WRITE_FILE_DEV_PIPE,// - ERRCODE_ASYNCIO_CANT_WRITE_FILE_DEV_PIPE
  ERRMSG_ASYNCIO_END_OF_FILE_DEV_PIPE,    // - ERRCODE_ASYNCIO_END_OF_FILE_DEV_PIPE
  ERRMSG_ASYNCIO_ALLOC_MEM,               // - ERRCODE_ASYNCIO_ALLOC_MEM
  ERRMSG_ASYNCIO_FREE_MEM,                // - ERRCODE_ASYNCIO_FREE_MEM
  ERRMSG_ASYNCIO_WRONG_BLOCK_SIZE,        // - ERRCODE_ASYNCIO_WRONG_BLOCK_SIZE
  ERRMSG_ASYNCIO_WRONG_FILE_ATTRIB,       // - ERRCODE_ASYNCIO_WRONG_FILE_ATTRIB
  ERRMSG_ASYNCIO_MEDIA_WRITE_PROTECTED,   // - ERRCODE_ASYNCIO_MEDIA_WRITE_PROTECTED
  ERRMSG_ASYNCIO_DEVICE_NOT_READY,        // - ERRCODE_ASYNCIO_DEVICE_NOT_READY
  ERRMSG_ASYNCIO_END_OF_MEDIA,            // - ERRCODE_ASYNCIO_END_OF_MEDIA
  ERRMSG_ASYNCIO_FILE_SET_MARK,           // - ERRCODE_ASYNCIO_FILE_SET_MARK
  ERRMSG_ASYNCIO_MEDIA_NOT_PATITIONED,    // - ERRCODE_ASYNCIO_MEDIA_NOT_PATITIONED
  ERRMSG_ASYNCIO_NO_MEDIA_IN_DEVICE,      // - ERRCODE_ASYNCIO_NO_MEDIA_IN_DEVICE
  ERRMSG_ASYNCIO_MEDIA_CHANGED,           // - ERRCODE_ASYNCIO_MEDIA_CHANGED
  ERRMSG_ASYNCIO_BAD_LENGTH_READ_WRITTEN, // - ERRCODE_ASYNCIO_BAD_LENGTH_READ_WRITTEN
  ERRMSG_ASYNCIO_GET_TAPE_DRIVE_INFO,     // - ERRCODE_ASYNCIO_GET_TAPE_DRIVE_INFO
  ERRMSG_ASYNCIO_GET_TAPE_MEDIA_INFO,     // - ERRCODE_ASYNCIO_GET_TAPE_MEDIA_INFO
  ERRMSG_ASYNCIO_SET_TAPE_DRIVE_INFO,     // - ERRCODE_ASYNCIO_SET_TAPE_DRIVE_INFO
  ERRMSG_ASYNCIO_SET_TAPE_MEDIA_INFO,     // - ERRCODE_ASYNCIO_SET_TAPE_MEDIA_INFO
  ERRMSG_ASYNCIO_REWIND_TAPE,             // - ERRCODE_ASYNCIO_REWIND_TAPE
  ERRMSG_ASYNCIO_LOCK_UNLOCK_TAPE,        // - ERRCODE_ASYNCIO_LOCK_UNLOCK_TAPE
  ERRMSG_ASYNCIO_WRITE_TAPE_MARK,         // - ERRCODE_ASYNCIO_WRITE_TAPE_MARK
  ERRMSG_ASYNCIO_CRC_ERROR,               // - ERRCODE_ASYNCIO_CRC_ERROR
  ERRMSG_ASYNCIO_SHARING_VIOLATION,       // - ERRCODE_ASYNCIO_SHARING_VIOLATION
  ERRMSG_ASYNCIO_WRONG_FILE_DEVICE_NAME,  // - ERRCODE_ASYNCIO_WRONG_FILE_DEVICE_NAME
  ERRMSG_ASYNCIO_DEVICE_BUSY,             // - ERRCODE_ASYNCIO_DEVICE_BUSY
  ERRMSG_ASYNCIO_UNRECOGNIZED_MEDIA,      // - ERRCODE_ASYNCIO_UNRECOGNIZED_MEDIA
  ERRMSG_ASYNCIO_TENSION_TAPE,            // - ERRCODE_ASYNCIO_TENSION_TAPE
  ERRMSG_ASYNCIO_DISK_FULL,               // - ERRCODE_ASYNCIO_DISK_FULL
  ERRMSG_ASYNCIO_LOAD_UNLOAD_TAPE,        // - ERRCODE_ASYNCIO_LOAD_UNLOAD_TAPE
  ERRMSG_ASYNCIO_ILL_BLOCK_COUNT,         // - ERRCODE_ASYNCIO_ILL_BLOCK_COUNT
  ERRMSG_ASYNCIO_IO_READ_ONLY,            // - ERRCODE_ASYNCIO_IO_READ_ONLY
  ERRMSG_ASYNCIO_PIPE_CONN_CANCELLED,     // - ERRCODE_ASYNCIO_PIPE_CONN_CANCELLED
  ERRMSG_ASYNCIO_NO_DATA_DETECTED,        // - ERRCODE_ASYNCIO_NO_DATA_DETECTED
  ERRMSG_ASYNCIO_OPERATION_CANCELED,      // - ERRCODE_ASYNCIO_OPERATION_CANCELED
  ERRMSG_ASYNCIO_INVALID_BLOCKSIZE        // - ERRCODE_ASYNCIO_INVALID_BLOCKSIZE
  };

static tsp00_VfType_Enum FileType_os2sp[] = {
  vf_t_unknown,         // DT_UNKNOWN
  vf_t_raw,             // DT_CONOUT
  vf_t_raw,             // DT_CONIN
  vf_t_raw,             // DT_PRN
  vf_t_raw,             // DT_LPTx
  vf_t_raw,             // DT_COMx
  vf_t_file,            // DT_OTHER
  vf_t_tape_rew,        // DT_TAPE
  vf_t_raw,             // DT_DRIVE
  vf_t_pipe,            // DT_PIPE
  vf_t_file             // DT_NUL
};

static const char* FileTypeName[] = {
    "unknown",
    "file",
    "pipe",
    "raw",
    "tape norewind",
    "tape rewind"
};

/*
 * LOCAL FUNCTION PROTOTYPES
 */
VOID _System sql65k_asynci        ( PASYNCI_CTRL_REC           pAsynciCtrl );

tsp00_Bool   sql65k_asynci_create ( PDLQ_REC                   pRequest );

tsp00_Bool   sql65k_asynci_remove ( PDLQ_REC                   pRequest );

PDLQ_REC     sql65k_async_receive ( PPIO_QUEUE_REC             ppIOWorkQue,
                                    SAPDB_UInt4 *              plIOQueLen,
                                    PLONG                      plIOWorkExclusive,
                                    PTHREAD_CTRL_HEADER_REC    pThrdCtrlHeader);


// --- inline functions
_INLINE VOID __sql65k_req_to_asynci         ( PASYNCI_CTRL_REC pAsyncCtrl,
                                              PDLQ_REC         pRequest );

_INLINE VOID __sql65k_req_from_asyncx_to_ukt( PDLQ_REC         pRequest );

_INLINE VOID __sql65k_req_to_async0         ( PDLQ_REC        pRequest,
                                             PTASK_CTRL_REC  pTaskCtrl );

VOID       sql65k_tape_open_cleanup( PASYNCI_CTRL_REC            pAsynciCtrl,
                                     DLQ_VASYNOPEN_REQ_ARGS_REC  *pVasynOpenArgs );

tsp00_Bool sql65k_tape_write_mark  ( PASYNCI_CTRL_REC            pAsynciCtrl,
                                     PULONG                      pulErrCode,
                                     PULONG                      pAPIRetCode,
                                     tsp00_Bool                  *pfOk );
tsp00_Bool sql65k_init_dlt_tape    ( PASYNCI_CTRL_REC            pAsynciCtrl );
tsp00_Bool sql65k_tape_loading     ( PASYNCI_CTRL_REC            pAsynciCtrl ,
                                     struct task_ctrl_record     *pReqTaskCtrl,
                                     PULONG                      pulErrCode,
                                     PULONG                      pAPIRetCode,
                                     tsp00_Bool                  *pfOk,
                                     ULONG                       ulOperation );
tsp00_Bool sql65k_tape_locking     ( PASYNCI_CTRL_REC            pAsynciCtrl,
                                     PULONG                      pulErrCode,
                                     PULONG                      pAPIRetCode,
                                     tsp00_Bool                 *pfOk,
                                     ULONG                       ulOperation );
tsp00_Bool sql65k_tape_rewind      ( PASYNCI_CTRL_REC            pAsynciCtrl,
                                     PULONG                      pulErrCode,
                                     PULONG                      pAPIRetCode,
                                     tsp00_Bool                 *pfOk );
tsp00_Bool sql65k_set_drive_info   ( PASYNCI_CTRL_REC            pAsynciCtrl,
                                     PULONG                      pulErrCode,
                                     PULONG                      pAPIRetCode,
                                     tsp00_Bool                 *pfOk );
tsp00_Bool sql65k_set_media_info   ( PASYNCI_CTRL_REC            pAsynciCtrl,
                                     PULONG                      pulErrCode,
                                     PULONG                      pAPIRetCode,
                                     tsp00_Bool                  *pfOk );
tsp00_Bool sql65k_get_drive_info   ( PASYNCI_CTRL_REC            pAsynciCtrl,
                                     PULONG                      pulErrCode,
                                     PULONG                      pAPIRetCode,
                                     tsp00_Bool                 *pfOk );

tsp00_Bool sql65k_get_media_info   ( PASYNCI_CTRL_REC            pAsynciCtrl,
                                     struct task_ctrl_record     *pReqTaskCtrl,
                                     PULONG                      pulErrCode,
                                     PULONG                      pAPIRetCode,
                                     tsp00_Bool                        *pfOk );

tsp00_Bool sql65k_tape_tension     ( PASYNCI_CTRL_REC            pAsynciCtrl,
                                     PULONG                      pulErrCode,
                                     PULONG                      pAPIRetCode,
                                     tsp00_Bool                        *pfOk );

tsp00_Bool sql65k_tape_autorewind  ( PASYNCI_CTRL_REC            pAsynciCtrl,
                                     PULONG                      pulErrCode,
                                     PULONG                      pAPIRetCode,
                                     tsp00_Bool                        *pfOk );

HANDLE sql65k_create_file          ( LPCTSTR                     lpFileName,
                                     DWORD                       dwDesiredAccess,
                                     DWORD                       dwShareMode,
                                     LPSECURITY_ATTRIBUTES       lpSecurityAttributes,
                                     DWORD                       dwCreationDisposition,
                                     DWORD                       dwFlagsAndAttributes,
                                     HANDLE                      hTemplateFile );
                               
tsp00_Bool   sql65k_tape_in_drive  ( PASYNCI_CTRL_REC            pAsynciCtrl );


_INLINE void os65k_UpdateCountInIOStatisticRec( teo52_IOStatistic * pRec, SAPDB_UInt8 pageCount )
{
    pRec->IOStat.TotalTimeStat.ulCount++;
    pRec->TotalPageCount+= pageCount;
    pRec->IOStat.TimeStat.ulCount++;
    pRec->ulPageCount+= pageCount;
}

_INLINE void os65k_UpdateCountInTaskDevIOStatisticRec( teo52_TaskDevIOStatistic * pRec, SAPDB_UInt8 pageCount )
{
    pRec->IOStat.TotalTimeStat.ulCount++;
    pRec->TotalPageCount+= pageCount;
    pRec->IOStat.TimeStat.ulCount++;
    pRec->ulPageCount+= pageCount;
}


_INLINE void os65k_UpdateTimeInTaskDevIOStatisticRec( teo52_TaskDevIOStatistic * pRec,
                                                      SAPDB_UInt8 delta_abs_time,
                                                      SAPDB_UInt8 delta_rel_time )
{
    pRec->IOStat.TotalTimeStat.ulMeasurements++;
    pRec->IOStat.TotalTimeStat.ulAbsMicroSeconds += delta_abs_time;
    pRec->IOStat.TotalTimeStat.ulRelMicroSeconds += delta_rel_time;
    pRec->IOStat.TimeStat.ulMeasurements++;
    pRec->IOStat.TimeStat.ulAbsMicroSeconds += delta_abs_time;
    pRec->IOStat.TimeStat.ulRelMicroSeconds += delta_rel_time;
}

/*
 * ========================== GLOBAL FUNCTIONS ================================
 */

extern "C"  void  vasynopen  ( tsp00_VFilename*   device,
                               tsp00_Bool         is_devspace,
                               tsp00_Bool         for_writing,
                               tsp00_VfType_Param filetype,
                               SAPDB_Int4         block_size,
                               tsp00_BoolAddr     cancel_pointer,
                               SAPDB_Int4      *  max_blockcount,
                               SAPDB_Int4      *  deviceno,
                               tsp00_VaReturn  *  error,
                               tsp00_ErrText     &errtext )
  {
  #undef  MF__
  #define MF__ MOD__"vasynopen"
  PDLQ_REC                       pRequest;
  CHAR                           szLogDeviceName[ sizeof(tsp00_VFilename) + 1 ];
  PUKT_CTRL_REC                  pUKT      = THIS_UKT_CTRL;
  PTASK_CTRL_REC                 pTaskCtrl = pUKT->pCTask;

  DBGIN_T (pTaskCtrl->ulTaskIndex);

  DBG3 (( MF__, "open of device kind %d", filetype));
  if ( kgs.Async0.ThrdCtrlHeader.ThrdState == KT_INITIALIZED )
    {
    MSGCD(( ERR_THREAD_XXX_NOT_ACTIVE, "ASYNC0" ));
    sql46c_build_error_string ( errtext, ERRMSG_ASYNCIO_ASYNC0_NOT_ACTIVE, 0 );
    error->becomes(va_notok);
    return;
    }

  pTaskCtrl->TaskState = TSK_ASYNOPEN_EO00;


  //
  //
  // ---  Send an vasynopen-request.
  //
  //
  eo46PtoC ( szLogDeviceName, (tsp00_Byte*)device, sizeof (tsp00_VFilename) );
  pRequest                                              = GetTaskDLQElement1( pTaskCtrl );
  pRequest->pTaskCtrl                                   = pTaskCtrl;
  pRequest->ulReqType                                   = REQ_ASYNOPEN;
  pRequest->ReqArgs.VasynOpenRequest.pReqTaskCtrl       = pTaskCtrl;
  pRequest->ReqArgs.VasynOpenRequest.fOk                = FALSE;
  pRequest->ReqArgs.VasynOpenRequest.pszLogDeviceName   = szLogDeviceName;
  pRequest->ReqArgs.VasynOpenRequest.fIsDevspace        = is_devspace;
  pRequest->ReqArgs.VasynOpenRequest.fForWriting        = for_writing;
  pRequest->ReqArgs.VasynOpenRequest.FileType.becomes(filetype);
  pRequest->ReqArgs.VasynOpenRequest.ulBlockSize        = block_size;
  pRequest->ReqArgs.VasynOpenRequest.cancelPointer      = ( cancel_pointer ? cancel_pointer : &pTaskCtrl->KernelCtrlRecord.to_cancel );
  pRequest->ReqArgs.VasynOpenRequest.ulMaxBlockCnt      = *max_blockcount;
  pRequest->ReqArgs.VasynOpenRequest.ulErrCode          = 0;
  pRequest->ReqArgs.VasynOpenRequest.APIRetCode         = NO_ERROR;

  // --- send request to ASYNC0 thread
  REQ_TO_ASYNC0( pRequest, pTaskCtrl );

  // --- goto dispatcher
  GOTO_DISP(&pUKT);

  DBG3 (( MF__, "[T:0x%03u] Returned from request pUKT: %x",
          pTaskCtrl->ulTaskIndex, pUKT ));

  // --- wrong reply?
  if ( pUKT->pCurrReq != pRequest )
    {
    MSGCD (( ERR_UNKNOWN_REQUEST ));
    DBG1  (( MF__, "[T:0x%03u] pRequest: 0x%x (SEND) != pUKT->pCurrReq: 0x%x (REPLY)",
            pTaskCtrl->ulTaskIndex, pRequest, pUKT->pCurrReq ));
    ABORT();
    }

  if ( pRequest->ReqArgs.VasynOpenRequest.fOk == TRUE )
    {
    *deviceno       = pRequest->ReqArgs.VasynOpenRequest.lAsynciNo;

    DBG3 (( MF__, "[T:0x%03u] Returned OK, pRequest: 0x%x",
            pTaskCtrl->ulTaskIndex, pRequest ));

    error->becomes(va_ok);
    }
  else
    {
    *deviceno       = UNDEF;
    if ( *max_blockcount != (INT4) pRequest->ReqArgs.VasynOpenRequest.ulMaxBlockCnt )
      *max_blockcount = (INT4 )pRequest->ReqArgs.VasynOpenRequest.ulMaxBlockCnt;
    else
      *max_blockcount = 0;

    if ( pRequest->ReqArgs.VasynOpenRequest.ulErrCode  == ERRCODE_ASYNCIO_NO_DATA_DETECTED )
      {
      error->becomes(va_eof);
      }
    else if ( pRequest->ReqArgs.VasynOpenRequest.ulErrCode < MAX_ASYNCIO_ERRCODE )
      {
      sql46c_build_error_string ( errtext,
                                  ErrTab[ pRequest->ReqArgs.VasynOpenRequest.ulErrCode ],
                                  pRequest->ReqArgs.VasynOpenRequest.APIRetCode );

      DBG1 (( MF__, "[T:0x%03u] pRequest: 0x%x, %s",
              pTaskCtrl->ulTaskIndex, pRequest,
              ErrTab[ pRequest->ReqArgs.VasynOpenRequest.ulErrCode ] ));
      error->becomes(va_notok);
      }
    else
      {
      sql46c_build_error_string ( errtext, ERRMSG_ASYNCIO_SURPRISING_ERROR_CODE,
                                  pRequest->ReqArgs.VasynOpenRequest.APIRetCode );
      error->becomes(va_notok);
      }
    }


  pTaskCtrl->TaskState = TSK_RUNNING_EO00;

  DBGOUT_T (pTaskCtrl->ulTaskIndex);
  return;
  }

/*------------------------------*/

extern "C"  void  vasynclose  ( SAPDB_Int4       deviceno,
                                tsp00_Bool       rewind,
                                tsp00_Bool       nxt_in_autoloader,
                                tsp00_VaReturn*  nxt_tape_available,
                                tsp00_VaReturn*  error,
                                tsp00_ErrText   &errtext )
  {
  #undef  MF__
  #define MF__ MOD__"vasynclose"
  PDLQ_REC                       pRequest;
  PUKT_CTRL_REC                  pUKT      = THIS_UKT_CTRL;
  PTASK_CTRL_REC                 pTaskCtrl = pUKT->pCTask;

  DBGIN_T (pTaskCtrl->ulTaskIndex);

  error->becomes(va_ok);
  nxt_tape_available->becomes(nxt_in_autoloader ? va_ok : va_notok);

  pTaskCtrl->TaskState = TSK_ASYNCLOSE_EO00;

  //
  //
  // ---  Send an vasynclose-request.
  //
  //
  if ( nxt_in_autoloader )
    rewind = TRUE;

  pRequest                                                 = GetTaskDLQElement1( pTaskCtrl );
  pRequest->pTaskCtrl                                      = pTaskCtrl;
  pRequest->ulReqType                                      = REQ_ASYNCLOSE;
  pRequest->ReqArgs.VasynCloseRequest.lAsynciNo            = deviceno;
  pRequest->ReqArgs.VasynCloseRequest.fOk                  = FALSE;
  pRequest->ReqArgs.VasynCloseRequest.ulErrCode            = 0;
  pRequest->ReqArgs.VasynCloseRequest.APIRetCode           = NO_ERROR;
  pRequest->ReqArgs.VasynCloseRequest.fRewind              = rewind;
  pRequest->ReqArgs.VasynCloseRequest.fNxtTapeInAutoLoader = nxt_in_autoloader;
  pRequest->ReqArgs.VasynCloseRequest.fNxtTapeAvailable    = nxt_in_autoloader;

  // --- send request to ASYNC0 thread
  REQ_TO_ASYNC0( pRequest, pTaskCtrl );

  // --- goto dispatcher
  GOTO_DISP(&pUKT);

  DBG3 (( MF__, "[T:0x%03u] Returned from request pUKT: %x",
          pTaskCtrl->ulTaskIndex, pUKT ));

  // --- wrong reply?
  if ( pUKT->pCurrReq != pRequest )
    {
    MSGCD (( ERR_UNKNOWN_REQUEST ));
    DBG1  (( MF__, "[T:0x%03u] pRequest: 0x%x (SEND) != pUKT->pCurrReq: 0x%x (REPLY)",
            pTaskCtrl->ulTaskIndex, pRequest, pUKT->pCurrReq ));
    ABORT();
    }

  if ( pRequest->ReqArgs.VasynCloseRequest.fOk == FALSE )
    error->becomes(va_notok);

  if (( nxt_in_autoloader                                     == TRUE  ) &&
      ( pRequest->ReqArgs.VasynCloseRequest.fNxtTapeAvailable == FALSE ))
    {
    nxt_tape_available->becomes(va_notok);
    }

  if ( *error == va_notok ||
     ( nxt_in_autoloader && *nxt_tape_available == va_notok ) )
    {
    //
    // --- replied via UKT queue
    //
    if ( pRequest->ReqArgs.VasynCloseRequest.ulErrCode < MAX_ASYNCIO_ERRCODE )
      {
      sql46c_build_error_string (
                        errtext,
                        ErrTab[ pRequest->ReqArgs.VasynCloseRequest.ulErrCode ],
                        pRequest->ReqArgs.VasynCloseRequest.APIRetCode );

      DBG1 (( MF__, "[T:0x%03u] pRequest: 0x%x, %s",
              pTaskCtrl->ulTaskIndex, pRequest,
              ErrTab[ pRequest->ReqArgs.VasynCloseRequest.ulErrCode ] ));
      }
    }

  pTaskCtrl->TaskState = TSK_RUNNING_EO00;

  DBGOUT_T (pTaskCtrl->ulTaskIndex);
  return;
  }

/*------------------------------*/

extern "C"  void  vasynio    ( SAPDB_Int4      deviceno,
                               SAPDB_Int4      io_id,
                               SAPDB_Int4      file_pos,
                               SAPDB_Int4      io_block_cnt,
                               tsp00_PageAddr  io_block_ptr,
                               tsp00_VaReturn* error,
                               tsp00_ErrText  &errtext )
  {
  #undef  MF__
  #define MF__ MOD__"vasynio"
  PASYNCI_CTRL_REC                 pAsynciCtrl;
  PDLQ_REC                         pRequest;
  PUKT_CTRL_REC                    pUKT      = THIS_UKT_CTRL;
  PTASK_CTRL_REC                   pTaskCtrl = pUKT->pCTask;
  ULONG                            i;

  DBGIN_T (pTaskCtrl->ulTaskIndex);

  error->becomes(va_ok);

  pTaskCtrl->TaskState = TSK_ASYNIO_EO00;

  if (( deviceno < 0 ) || ( deviceno >= (tsp00_Int4 ) kgs.ulNumOfAsyncis ))
    {
    error->becomes(va_notok);
    pTaskCtrl->TaskState = TSK_RUNNING_EO00;

    MSGCD (( ERR_WRONG_ASYNCI_NO, deviceno ));
    DBG1  (( MF__, "Wrong ASYNCi number: %d", deviceno ));
    sql46c_build_error_string ( errtext, ERRMSG_ASYNCIO_WRONG_DEVICENO, 0 );

    DBGOUT;
    return;
    }

  pAsynciCtrl = &kgs.pFirstAsynciCtrl[deviceno];
  pTaskCtrl->lWaitForDevno   = deviceno ;
  pTaskCtrl->DevNameWaitingFor= pAsynciCtrl->szLogDeviceName ;

  if (( pAsynciCtrl->fAsyncClosePending       == TRUE ) ||
      ( pAsynciCtrl->ThrdCtrlHeader.ThrdState == KT_INITIALIZED ))
    {
    error->becomes(va_notok);
    pTaskCtrl->TaskState = TSK_RUNNING_EO00;

    MSGCD (( ERR_FILE_DEV_PIPE_NOT_OPEN ));
    DBG1  (( MF__, "file/device/pipe not open" ));
    sql46c_build_error_string ( errtext, ERRMSG_ASYNCIO_FILE_DEV_PIPE_NOT_OPEN, 0 );

    DBGOUT;
    return;
    }

  for ( i = 0; i < pAsynciCtrl->ulAsynRequests; i++ )
    {
    pRequest = static_cast<DLQ_ELEMENT*>(pAsynciCtrl->pAsynRequests[i]);

    if ( pRequest->ReqArgs.VasynIORequest.pTaskCtrl )
      continue;

    WAIT_UNTIL_ALONE( &pAsynciCtrl->lExclusive );

    if ( pRequest->ReqArgs.VasynIORequest.pTaskCtrl == NULL )
      {
      pRequest->ReqArgs.VasynIORequest.pTaskCtrl = pTaskCtrl;
      CLEARLOCK( &pAsynciCtrl->lExclusive );
      break;
      }
    CLEARLOCK( &pAsynciCtrl->lExclusive );
    pRequest = 0;
    }

  if ( pRequest == NULL )
    {
    error->becomes(va_wait_first);
    pTaskCtrl->TaskState = TSK_RUNNING_EO00;

    DBG1  (( MF__, "wait first" ));
    sql46c_build_error_string ( errtext, ERRMSG_ASYNCIO_WAIT_FIRST, 0 );

    DBGOUT;
    return;
    }

  // --- increase io counter
  if ( pAsynciCtrl->fForWriting )
  {
      os65k_UpdateCountInTaskDevIOStatisticRec(&pTaskCtrl->TaskStateStat.AsynIOWriteStat, io_block_cnt);
  }
  else
  {
      os65k_UpdateCountInTaskDevIOStatisticRec(&pTaskCtrl->TaskStateStat.AsynIOReadStat, io_block_cnt);
  }

  //
  //
  // ---  Send an vasynio-request.
  //
  //
  pRequest->pTaskCtrl                            = pTaskCtrl;
  pRequest->ulReqType                            = REQ_ASYNIO;
  pRequest->ReqArgs.VasynIORequest.lIO_ID        = io_id;
  pRequest->ReqArgs.VasynIORequest.ulBlockNo     = file_pos;
  pRequest->ReqArgs.VasynIORequest.ulBlockCnt    = io_block_cnt;
  pRequest->ReqArgs.VasynIORequest.pBlockPtr     = io_block_ptr;
  pRequest->ReqArgs.VasynIORequest.fFinished     = FALSE;
  pRequest->ReqArgs.VasynIORequest.fOk           = FALSE;
  pRequest->ReqArgs.VasynIORequest.ulErrCode     = 0;
  pRequest->ReqArgs.VasynIORequest.APIRetCode    = NO_ERROR;

  // --- send request to ASYNCI thread
  REQ_TO_ASYNCI ( pAsynciCtrl, pRequest );

  pTaskCtrl->TaskState = TSK_RUNNING_EO00;

  DBGOUT_T (pTaskCtrl->ulTaskIndex);
  }

/*------------------------------*/

extern "C" void vasynwait  ( SAPDB_Int4       deviceno,
                             SAPDB_Int4*      io_id,
                             SAPDB_Int4*      io_block_cnt,
                             tsp00_VaReturn*  error,
                             tsp00_ErrText   &errtext )
  {
  #undef  MF__
  #define MF__ MOD__"vasynwait"
  PASYNCI_CTRL_REC                 pAsynciCtrl;
  PDLQ_REC                         pRequest;
  PUKT_CTRL_REC                    pUKT        = THIS_UKT_CTRL;
  PTASK_CTRL_REC                   pTaskCtrl   = pUKT->pCTask;
  ULONG                            i;
  DLQ_VASYNIO_REQ_ARGS_REC         *pVasynIORequest;

  DBGIN_T (pTaskCtrl->ulTaskIndex);

  error->becomes(va_ok);

  *io_id        = UNDEF;
  *io_block_cnt = UNDEF;

  if (( deviceno < 0 ) || ( deviceno >= (tsp00_Int4 ) kgs.ulNumOfAsyncis ))
    {
    error->becomes(va_notok);
    pTaskCtrl->TaskState = TSK_RUNNING_EO00;

    MSGCD (( ERR_WRONG_ASYNCI_NO, deviceno ));
    DBG1  (( MF__, "Wrong ASYNCi number: %d", deviceno ));
    sql46c_build_error_string ( errtext, ERRMSG_ASYNCIO_WRONG_DEVICENO, 0 );

    DBGOUT;
    return;
    }

  pAsynciCtrl = &kgs.pFirstAsynciCtrl[deviceno];

  if (( pAsynciCtrl->fAsyncClosePending       == TRUE ) ||
      ( pAsynciCtrl->ThrdCtrlHeader.ThrdState == KT_INITIALIZED ))
    {
    error->becomes(va_notok);
    pTaskCtrl->TaskState = TSK_RUNNING_EO00;

    MSGCD (( ERR_FILE_DEV_PIPE_NOT_OPEN ));
    DBG1  (( MF__, "file/device/pipe not open" ));
    sql46c_build_error_string ( errtext, ERRMSG_ASYNCIO_FILE_DEV_PIPE_NOT_OPEN, 0 );

    DBGOUT;
    return;
    }

  if ( pAsynciCtrl->fForWriting )
    pTaskCtrl->TaskState = TSK_ASYNWAIT_WRITE_EO00;
  else
    pTaskCtrl->TaskState = TSK_ASYNWAIT_READ_EO00;


  for ( i = 0; i < pAsynciCtrl->ulAsynRequests; i++ )
    {
    pRequest        = static_cast<DLQ_ELEMENT*>(pAsynciCtrl->pAsynRequests[i]);
    pVasynIORequest = &pRequest->ReqArgs.VasynIORequest;

    if (( pVasynIORequest->pTaskCtrl == pTaskCtrl ) &&
        ( pVasynIORequest->fFinished ))
      {
      break;
      }
    pRequest = 0;
    }

  // --- finished IO operation found?
  if ( pRequest != NULL )
    {
    if ( pRequest->ReqArgs.VasynIORequest.fOk )
      {
      if ( pRequest->ReqArgs.VasynIORequest.ulBlockCnt == 0 )
        {
        error->becomes(va_eof);
        sql46c_build_error_string (
                           errtext,
                           ErrTab[ pRequest->ReqArgs.VasynIORequest.ulErrCode ],
                           pRequest->ReqArgs.VasynIORequest.APIRetCode );
        }
      else
        {
        *io_id        = pRequest->ReqArgs.VasynIORequest.lIO_ID;
        *io_block_cnt = pRequest->ReqArgs.VasynIORequest.ulBlockCnt;
        }
      }
    else
      {
      error->becomes(va_notok);

      sql46c_build_error_string (
                           errtext,
                           ErrTab[ pRequest->ReqArgs.VasynIORequest.ulErrCode ],
                           pRequest->ReqArgs.VasynIORequest.APIRetCode );

      DBG1 (( MF__, "[T:0x%03u] pRequest: 0x%x, %s",
              pTaskCtrl->ulTaskIndex, pRequest,
              ErrTab[ pRequest->ReqArgs.VasynIORequest.ulErrCode ] ));
      }

    pRequest->ReqArgs.VasynIORequest.fFinished = FALSE;
    pRequest->ReqArgs.VasynIORequest.pTaskCtrl = NULL;
    }
  else
    {
    pUKT->lWaitForAsynciIO++;

    pRequest                                        = GetTaskDLQElement1( pTaskCtrl );
    pRequest->pTaskCtrl                             = pTaskCtrl;
    pRequest->ulReqType                             = REQ_ASYNWAIT;
    pRequest->ReqArgs.VasynWaitRequest.pReqTaskCtrl = pTaskCtrl;
    pRequest->ReqArgs.VasynWaitRequest.fOk          = FALSE;
    pRequest->ReqArgs.VasynWaitRequest.ulErrCode    = 0;
    pRequest->ReqArgs.VasynWaitRequest.APIRetCode   = NO_ERROR;

    // --- send request to ASYNCI thread
    REQ_TO_ASYNCI ( pAsynciCtrl, pRequest );

    // --- goto dispatcher
    if ( pAsynciCtrl->fForWriting )
      {
        GOTO_DISP(&pUKT);
        if ( pTaskCtrl->TimeCollectionEnabled )
        {
          os65k_UpdateTimeInTaskDevIOStatisticRec(
                 &pTaskCtrl->TaskStateStat.AsynIOWriteStat
                , pTaskCtrl->TaskStateStat.dispatcherLeaveTime
                - pTaskCtrl->TaskStateStat.dispatcherEnterTime
                , pTaskCtrl->TaskStateStat.runqueueEnqueueTime
                - pTaskCtrl->TaskStateStat.dispatcherEnterTime);
        }
      }
    else
      {
        GOTO_DISP(&pUKT);
        if ( pTaskCtrl->TimeCollectionEnabled )
        {
          os65k_UpdateTimeInTaskDevIOStatisticRec( 
                 &pTaskCtrl->TaskStateStat.AsynIOReadStat
                , pTaskCtrl->TaskStateStat.dispatcherLeaveTime
                - pTaskCtrl->TaskStateStat.dispatcherEnterTime
                , pTaskCtrl->TaskStateStat.runqueueEnqueueTime
                - pTaskCtrl->TaskStateStat.dispatcherEnterTime);
        }
      }

    // --- wrong reply?
    if ( pUKT->pCurrReq != pRequest )
      {
      MSGCD (( ERR_UNKNOWN_REQUEST ));
      DBG1  (( MF__, "[T:0x%03u] pRequest: 0x%x (SEND) != pUKT->pCurrReq: 0x%x (REPLY)",
              pTaskCtrl->ulTaskIndex, pRequest, pUKT->pCurrReq ));
      ABORT();
      }

    if ( pRequest->ReqArgs.VasynWaitRequest.fOk )
      {
      if ( pRequest->ReqArgs.VasynWaitRequest.ulBlockCnt == 0 )
        {
        error->becomes(va_eof);
        sql46c_build_error_string (
                           errtext,
                           ErrTab[ pRequest->ReqArgs.VasynWaitRequest.ulErrCode ],
                           pRequest->ReqArgs.VasynWaitRequest.APIRetCode );
        }
      else
        {
        *io_id        = pRequest->ReqArgs.VasynWaitRequest.lIO_ID;
        *io_block_cnt = pRequest->ReqArgs.VasynWaitRequest.ulBlockCnt;
        }
      }
    else
      {
      error->becomes(va_notok);

      sql46c_build_error_string (
                         errtext,
                         ErrTab[ pRequest->ReqArgs.VasynWaitRequest.ulErrCode ],
                         pRequest->ReqArgs.VasynWaitRequest.APIRetCode );

      DBG1 (( MF__, "[T:0x%03u] pRequest: 0x%x, %s",
              pTaskCtrl->ulTaskIndex, pRequest,
              ErrTab[ pRequest->ReqArgs.VasynWaitRequest.ulErrCode ] ));
      }

    pUKT->lWaitForAsynciIO--;
    }

  pTaskCtrl->TaskState       = TSK_RUNNING_EO00;
  pTaskCtrl->lWaitForDevno   = undef_ceo00 ;
  pTaskCtrl->DevNameWaitingFor= NULL ;

  DBGOUT_T (pTaskCtrl->ulTaskIndex);
  }

/*------------------------------*/

extern "C" void vasyninit_tape ( SAPDB_Int4       deviceno,
                                 tsp00_Bool       destructive,
                                 tsp00_VfType*    filetype,
                                 tsp00_VaReturn*  error,
                                 tsp00_ErrText   &errtext )
  {
  #undef  MF__
  #define MF__ MOD__"vasyninit_tape"
  PASYNCI_CTRL_REC                 pAsynciCtrl;
  PDLQ_REC                         pRequest;
  PUKT_CTRL_REC                    pUKT      = THIS_UKT_CTRL;
  PTASK_CTRL_REC                   pTaskCtrl = pUKT->pCTask;

  DBGIN_T (pTaskCtrl->ulTaskIndex);

  error->becomes(va_ok);

  pTaskCtrl->TaskState = TSK_ASYNCNTL_EO00;

  if (( deviceno < 0 ) || ( deviceno >= (tsp00_Int4 ) kgs.ulNumOfAsyncis ))
    {
    error->becomes(va_notok);
    pTaskCtrl->TaskState = TSK_RUNNING_EO00;

    MSGCD (( ERR_WRONG_ASYNCI_NO, deviceno ));
    DBG1  (( MF__, "Wrong ASYNCi number: %d", deviceno ));
    sql46c_build_error_string ( errtext, ERRMSG_ASYNCIO_WRONG_DEVICENO, 0 );

    DBGOUT;
    return;
    }

  pAsynciCtrl = &kgs.pFirstAsynciCtrl[deviceno];

  if (( pAsynciCtrl->fAsyncClosePending       == TRUE ) ||
      ( pAsynciCtrl->ThrdCtrlHeader.ThrdState == KT_INITIALIZED ))
    {
    error->becomes(va_notok);
    pTaskCtrl->TaskState = TSK_RUNNING_EO00;

    MSGCD (( ERR_FILE_DEV_PIPE_NOT_OPEN ));
    DBG1  (( MF__, "file/device/pipe not open" ));
    sql46c_build_error_string ( errtext, ERRMSG_ASYNCIO_FILE_DEV_PIPE_NOT_OPEN, 0 );

    DBGOUT;
    return;
    }

  //
  //
  // ---  Send an vasyncntl-request.
  //
  //
  pRequest                                        = GetTaskDLQElement1( pTaskCtrl );
  pRequest->pTaskCtrl                             = pTaskCtrl;
  pRequest->ulReqType                             = REQ_ASYNCNTL;
  pRequest->ReqArgs.VasynCntlRequest.fOk          = TRUE;
  pRequest->ReqArgs.VasynCntlRequest.ulErrCode    = 0;
  pRequest->ReqArgs.VasynCntlRequest.iCntlRequest = ASYNCNTL_FILETYPE;
  pRequest->ReqArgs.VasynCntlRequest.fDestructive = destructive;
  DBG1 ((MF__, "requesting for filetype in %x (%s)",
      &pRequest->ReqArgs.VasynCntlRequest, destructive ? "destructive" : "safe"));

  // --- send request to ASYNCI thread
  REQ_TO_ASYNCI ( pAsynciCtrl, pRequest );

  // --- goto dispatcher
  GOTO_DISP(&pUKT);

  DBG3 (( MF__, "[T:0x%03u] Returned from request pUKT: %x",
          pTaskCtrl->ulTaskIndex, pUKT ));

  // --- wrong reply?
  if ( pUKT->pCurrReq != pRequest )
    {
    MSGCD (( ERR_UNKNOWN_REQUEST ));
    DBG1  (( MF__, "[T:0x%03u] pRequest: 0x%x (SEND) != pUKT->pCurrReq: 0x%x (REPLY)",
            pTaskCtrl->ulTaskIndex, pRequest, pUKT->pCurrReq ));
    ABORT();
    }

  // --- receive request

  DBG1 ((MF__, "reading filetype %d from %x", pRequest->ReqArgs.VasynCntlRequest.FileType,
      &pRequest->ReqArgs.VasynCntlRequest));
  *filetype = pRequest->ReqArgs.VasynCntlRequest.FileType;
  DBGOUT_T (pTaskCtrl->ulTaskIndex);
  }

/*------------------------------*/

extern "C" void sql65k_async0 ( void )
  {
  #undef  MF__
  #define MF__ MOD__"sql65k_async0"
  LONG                          rc = NO_ERROR;
  PDLQ_REC                      pRequest;
  SAPDB_UInt4                   lDummy = 0;

  DBGIN;

  __try 
   {
#   if defined(_FASTCAP)
     CAPNameThread ("Async0");
#   endif

    // --- Create event semaphore (SET) for ASYNC0
    rc = sql41c_create_event_sem (&kgs.Async0.ThrdCtrlHeader.hevSem, NULL,
                                  NULL, 0, FALSE, NULL);
    if (rc != NO_ERROR)
      ABORT();

    // --- set the thread priority
    rc = SET_THREAD_PRIO ( kgs.Async0.ThrdCtrlHeader.lThrdPrio,
                           kgs.Async0.ThrdCtrlHeader.Tid,
                           kgs.Async0.ThrdCtrlHeader.hThrd );

    if ( rc != NO_ERROR )
      {
      MSGCD(( ERR_SETTING_THRD_PRIO, kgs.Async0.ThrdCtrlHeader.Tid));
      DBG1 ((MF__, "Cannot set the thread priority, TID: %u",
                    kgs.Async0.ThrdCtrlHeader.Tid));
      ABORT();
      }

    MSGD (( INFO_KERNEL_THREAD_STARTED, "ASYNC0" ));

    // --- set thread control info
    kgs.Async0.ThrdCtrlHeader.ThrdState   = KT_RUNNING;
    kgs.Async0.ThrdCtrlHeader.ulStartTime = kgs.ulCurrTime;

    // --- Runs forever
    for (;; )
      {
      DBG3 (( MF__, "kgs.Async0.pIOWorkQue           is 0x%x", kgs.Async0.pIOWorkQue ));
      DBG3 (( MF__, "kgs.Async0.pIOReqQue            is 0x%x", kgs.Async0.pIOReqQue ));
      DBG3 (( MF__, "kgs.Async0.pIOWorkQue->pRequest is 0x%x", kgs.Async0.pIOWorkQue->pRequest ));

      /*
       *  Wait for a request to ASYNC0
       */
      pRequest = sql65k_async_receive ( &kgs.Async0.pIOWorkQue,
                                        &lDummy,
                                        &kgs.Async0.lExclusive,
                                        &kgs.Async0.ThrdCtrlHeader );

      switch ( pRequest->ulReqType )
        {

        case REQ_ASYNOPEN :
          sql65k_asynci_create ( pRequest );
          break;

        case REQ_ASYNCLOSE :
          sql65k_asynci_remove ( pRequest );
          break;

        default          :
          MSGCD (( ERR_ILLEGAL_REQUEST, "ASYNC0", pRequest->ulReqType ));
        }


      DBG3 (( MF__, "next in loop" ));
      }
  }
  __except( sql50k_excepthandler(GetExceptionInformation()) ) {;}

  MSGD (( INFO_KERNEL_THREAD_STOPPED, "ASYNC0" ));

  DBGOUT;
  EXITTHREAD ( 0, kgs.Async0.ThrdCtrlHeader.hThrd );

  return;
  }



/*
 * ========================== LOCAL FUNCTIONS =================================
 */

/*
 *   Tries to get a request from the ASYNCx 'IOWork' queue, if no request
 *   is available then this thread should go to sleep until it is waked
 *   by a task/thread making a IO-request to it.
 */

static PDLQ_REC  sql65k_async_receive ( PPIO_QUEUE_REC          ppIOWorkQue,
                                        SAPDB_UInt4 *           plIOQueLen,
                                        PLONG                   plIOWorkExclusive,
                                        PTHREAD_CTRL_HEADER_REC pThrdCtrlHeader)

  {
  #undef  MF__
  #define MF__ MOD__"sql65k_async_receive"
  LONG       rc = NO_ERROR;
  PDLQ_REC   pRequest;

  DBGIN;

  for (;;)
    {
    DBG3 (( MF__, "ASYNC0 ASYNCi will sleep on hevSem %d",
            pThrdCtrlHeader->hevSem ));

    // --- remove element from queue
    WAIT_UNTIL_ALONE( plIOWorkExclusive );

    pRequest = (*ppIOWorkQue)->pRequest;

    if ( pRequest )
      {
      (*ppIOWorkQue)->pRequest = 0;
      (*ppIOWorkQue)           = (*ppIOWorkQue)->pNextElem;
      (*plIOQueLen)--;
      CLEARLOCK( plIOWorkExclusive );
      break;
      }
    CLEARLOCK( plIOWorkExclusive );

    // --- signal that there are no wake up activities
    pThrdCtrlHeader->ThrdState = KT_SLEEPING;

    rc = sql41c_wait_event_sem (pThrdCtrlHeader->hevSem, (ULONG) -1, "ASYNC0 or ASYNCi" );

    if ((rc != NO_ERROR) && (rc != ERROR_TIMEOUT))
        ABORT();

    pThrdCtrlHeader->ThrdState = KT_RUNNING;
    }

  DBG3 (( MF__, "ASYNC0 or ASYNCi Request: 0x%x from TID:%u, [T:0x%03u]",
          pRequest, pRequest->pTaskCtrl->pUKT->ThrdCtrlHeader.Tid,
          pRequest->pTaskCtrl->ulTaskIndex ));


  return ( pRequest );
  }

/*------------------------------*/


static tsp00_Bool sql65k_get_free_asynci ( PASYNCI_CTRL_REC           *ppAsynciCtrl,
                                     DLQ_VASYNOPEN_REQ_ARGS_REC *pVasynOpenArgs )

  {
  #undef  MF__
  #define MF__ MOD__"sql65k_get_free_asynci"
  ULONG                            ulAsynciNo;
  LONG                             lFirstFree  = -1;
  tsp00_Bool                             fFound      = FALSE;
  PASYNCI_CTRL_REC                 pTmpAsynciCtrl;
  ULONG                            ulMaxAsynci;

  DBGIN;

  *ppAsynciCtrl = NULL;

  if ( pVasynOpenArgs->fIsDevspace )
    {
    ulAsynciNo  = 0;
    ulMaxAsynci = XPARAM(lMaxDevspaces);
    }
  else
    {
    ulAsynciNo  = XPARAM(lMaxDevspaces);
    ulMaxAsynci = kgs.ulNumOfAsyncis ;
    }

  /*
   * Check and search a free ASYNCi control record
   */
  while ( ulAsynciNo < ulMaxAsynci  && !fFound)
  {
    pTmpAsynciCtrl = kgs.pFirstAsynciCtrl + ulAsynciNo;

    fFound = pTmpAsynciCtrl->ThrdCtrlHeader.ThrdState != KT_INITIALIZED &&
             !strcmp( pTmpAsynciCtrl->szLogDeviceName,
                      pVasynOpenArgs->pszLogDeviceName );

    if (( pTmpAsynciCtrl->ThrdCtrlHeader.ThrdState == KT_INITIALIZED ) &&
        ( lFirstFree == -1 ))
    {
      lFirstFree = ulAsynciNo;
    }
    ulAsynciNo++;
  }

  if ( fFound )
    {
    MSGALL(( ERR_DEVICE_ALREADY_ATT, pVasynOpenArgs->pszLogDeviceName ));
    DBG1  (( MF__, "Device '%s' already attached",
             pVasynOpenArgs->pszLogDeviceName ));
    pVasynOpenArgs->ulErrCode = ERRCODE_ASYNCIO_ALRATT;
    pVasynOpenArgs->fOk       = FALSE;
    }
  else
    {
    if (lFirstFree != -1)
      {
      DBG3 (( MF__, "First free device no. %d", lFirstFree ));
      pVasynOpenArgs->fOk       = TRUE;
      pVasynOpenArgs->lAsynciNo = lFirstFree;
      *ppAsynciCtrl             = kgs.pFirstAsynciCtrl + lFirstFree;
      }
    else
      {
      DBG1  (( MF__, "Maximum number of attached devices reached" ));
      MSGALL(( ERR_MAX_DEVICES_REACHED ));
      pVasynOpenArgs->ulErrCode = ERRCODE_ASYNCIO_MAXDEVICES;
      pVasynOpenArgs->fOk       = FALSE;
      }
    }

  DBGOUT;
  return ( pVasynOpenArgs->fOk );
  }

/*------------------------------*/

_INLINE VOID __sql65k_req_from_asyncx_to_ukt ( PDLQ_REC pRequest )
  {
  #undef  MF__
  #define MF__  MOD__"__sql65k_req_from_asyncx_to_ukt"

  DBGPAS;

  DBG3 (( MF__, "Send pRequest: 0x%x to TID:%u, [T:0x%03u]", pRequest,
         pRequest->pTaskCtrl->pUKT->ThrdCtrlHeader.Tid,
         pRequest->pTaskCtrl->ulTaskIndex ));

  sql74k_UKT_enqu_and_wake ( pRequest->pTaskCtrl->pUKT, pRequest->pTaskCtrl, pRequest );
  }


/*------------------------------*/

_INLINE VOID __sql65k_req_to_async0 ( PDLQ_REC        pRequest,
                                      PTASK_CTRL_REC  pTaskCtrl )
  {
  #undef  MF__
  #define MF__  MOD__"__sql54k_req_to_async0"

  DBGPAS;

  DBG3(( MF__, "[T:0x%03u] Send pRequest: 0x%x, using (Async0) pIOReqQue: 0x%x",
         pTaskCtrl->ulTaskIndex, pRequest, kgs.Async0.pIOReqQue ));

  WAIT_UNTIL_ALONE( &kgs.Async0.lExclusive );

  kgs.Async0.pIOReqQue->pRequest = pRequest;
  kgs.Async0.pIOReqQue           = kgs.Async0.pIOReqQue->pNextElem;

  CLEARLOCK( & kgs.Async0.lExclusive );

  sql72k_wake_kt ( &kgs.Async0.ThrdCtrlHeader );
  }

/*------------------------------*/

_INLINE VOID __sql65k_req_to_asynci ( PASYNCI_CTRL_REC pAsynciCtrl,
                                      PDLQ_REC         pRequest )
  {
  #undef  MF__
  #define MF__  MOD__"__sql65k_req_to_asynci"

  DBGPAS;

  WAIT_UNTIL_ALONE( &pAsynciCtrl->lExclusive );

  pAsynciCtrl->pIOReqQue->pRequest = pRequest;
  pAsynciCtrl->pIOReqQue           = pAsynciCtrl->pIOReqQue->pNextElem;
  pAsynciCtrl->DevIOStat.ulIOQueLen++;

  CLEARLOCK( &pAsynciCtrl->lExclusive );

  if (pAsynciCtrl->DevIOStat.ulIOQueLen > pAsynciCtrl->DevIOStat.ulMaxIOQueLen)
      pAsynciCtrl->DevIOStat.ulMaxIOQueLen = pAsynciCtrl->DevIOStat.ulIOQueLen;

  sql72k_wake_kt ( &pAsynciCtrl->ThrdCtrlHeader );
  }

/*------------------------------*/

/*
 * May only be called by the ASYNC0 thread
 * Assumes that the Device number is known.
 */

static tsp00_Bool sql65k_asynci_create ( PDLQ_REC   pRequest )
  {
  #undef  MF__
  #define MF__ MOD__"sql65k_asynci_create"
  PASYNCI_CTRL_REC                 pAsynciCtrl;
  LONG                             rc;
  ULONG                            ulWaitTime;
  DLQ_VASYNOPEN_REQ_ARGS_REC       *pVasynOpenArgs;

  DBGIN;

  pVasynOpenArgs = &pRequest->ReqArgs.VasynOpenRequest;

  DBG3 (( MF__, "Find free AsynciCtrl for '%s'",
          pVasynOpenArgs->pszLogDeviceName ));

  // --- get free ASYNCi
  if ( sql65k_get_free_asynci ( &pAsynciCtrl, pVasynOpenArgs ) == FALSE )
    {
    REQ_FROM_ASYNCx_TO_UKT(pRequest); // -- send error back to UKT

    DBGOUT;
    return ( FALSE );
    }

  DBG3 (( MF__, "pAsynciCtrl is        0x%x", pAsynciCtrl ));
  DBG3 (( MF__, "Creating ASYNCi for '%s', ASYNCi No: %d",
          pVasynOpenArgs->pszLogDeviceName, pVasynOpenArgs->lAsynciNo ));

  // --- init the asynci control record
  sql70k_init_asynci_ctrl ( pAsynciCtrl );

  // --- copy device name needed for the ASYNCi startup message
  strncpy ( pAsynciCtrl->szLogDeviceName, pVasynOpenArgs->pszLogDeviceName,
            sizeof(pAsynciCtrl->szLogDeviceName) );
  pAsynciCtrl->szLogDeviceName[sizeof(pAsynciCtrl->szLogDeviceName) - 1] = '\0';

    // --- Create event semaphore (SET) for ASYNCi
    rc = sql41c_create_event_sem ( &pAsynciCtrl->ThrdCtrlHeader.hevSem,
                                   NULL,
                                   NULL,
                                   0,
                                   FALSE,
                                   NULL );
    if ( NO_ERROR == rc )
    {
       // --- create ASYNCi-Thread
       rc = sql81k_create_kernel_thread( "ASYNCi"
                                      , &pAsynciCtrl->ThrdCtrlHeader.Tid
                                      , &pAsynciCtrl->ThrdCtrlHeader.hThrd
                                      , sql65k_asynci
                                      , pAsynciCtrl // argument
                                      , KT_ASYNCi_STACK_SIZE
                                      , false );
       if (rc != NO_ERROR)
       {
          sql41c_close_event_sem ( pAsynciCtrl->ThrdCtrlHeader.hevSem, "ASYNCi" );
          pAsynciCtrl->ThrdCtrlHeader.hevSem = (HEV)INVALID_HANDLE_VALUE;
       }
    }

  if (rc != NO_ERROR)
    {
    pVasynOpenArgs->fOk        = FALSE;
    pVasynOpenArgs->ulErrCode  = ERRCODE_ASYNCIO_ASYNCI_CREATE;
    pVasynOpenArgs->APIRetCode = rc;

    sql70k_init_asynci_ctrl ( pAsynciCtrl );

    REQ_FROM_ASYNCx_TO_UKT(pRequest); // -- send error back to UKT

    DBGOUT;
    return ( FALSE );
    }

  // --- wait until the ASYNCi is up and running
  for ( ulWaitTime = MAX_VASYNOPEN_WAIT_TIME * 10;
        (pAsynciCtrl->ThrdCtrlHeader.ThrdState == KT_INITIALIZED) && ulWaitTime;
        --ulWaitTime )
    {
    SLEEP( 1000 / 10 ); // -- sleep a moment
    }
  
  DBG3 (( MF__, "ASYNCi TID:%d openend", pAsynciCtrl->ThrdCtrlHeader.Tid ));


  // --- send request to ASYNCi
  REQ_TO_ASYNCI ( pAsynciCtrl, pRequest );

  DBGOUT;
  return ( TRUE );
  }

/*------------------------------*/

static tsp00_Bool sql65k_asynci_remove ( PDLQ_REC   pRequest )
  {
  #undef  MF__
  #define MF__ MOD__"sql65k_asynci_remove"
  PASYNCI_CTRL_REC                 pAsynciCtrl;
  DLQ_VASYNCLOSE_REQ_ARGS_REC      *pVasynCloseArgs;
  TID                              Tid;
  HANDLE                           hThrd;
  ULONG                            ulWaitTime = MAX_ASYNCLOSE_WAIT_TIME;

  DBGIN;

  pVasynCloseArgs = &pRequest->ReqArgs.VasynCloseRequest;

  if (( pVasynCloseArgs->lAsynciNo <  0 ) ||
      ( pVasynCloseArgs->lAsynciNo >= (long ) kgs.ulNumOfAsyncis ))
    {
    pVasynCloseArgs->fOk        = FALSE;
    pVasynCloseArgs->ulErrCode  = ERRCODE_ASYNCIO_WRONG_DEVICENO;

    MSGCD (( ERR_WRONG_ASYNCI_NO, pVasynCloseArgs->lAsynciNo ));
    DBG1  (( MF__, "Wrong ASYNCi number: %d", pVasynCloseArgs->lAsynciNo ));

    REQ_FROM_ASYNCx_TO_UKT(pRequest); // -- send error back to UKT

    DBGOUT;
    return ( FALSE );
    }

  pAsynciCtrl = &kgs.pFirstAsynciCtrl[pVasynCloseArgs->lAsynciNo];

  if (( pAsynciCtrl->fAsyncClosePending       == TRUE ) ||
      ( pAsynciCtrl->ThrdCtrlHeader.ThrdState == KT_INITIALIZED ))
    {
    pVasynCloseArgs->fOk        = FALSE;
    pVasynCloseArgs->ulErrCode  = ERRCODE_ASYNCIO_FILE_DEV_PIPE_NOT_OPEN;

    MSGCD (( ERR_FILE_DEV_PIPE_NOT_OPEN ));
    DBG1  (( MF__, "file/device/pipe not open" ));

    REQ_FROM_ASYNCx_TO_UKT(pRequest); // -- send error back to UKT

    DBGOUT;
    return ( FALSE );
    }

  pAsynciCtrl->fAsyncClosePending = TRUE;

  //
  // --- wait until the ASYNCi has stop the activities
  //
  while (( pAsynciCtrl->ThrdCtrlHeader.ThrdState != KT_SLEEPING ) ||
         ( pAsynciCtrl->pIOWorkQue               != pAsynciCtrl->pIOReqQue ))
    {
    DBG3 (( MF__, "ThrdCtrlHeader.ThrdState = %d",   pAsynciCtrl->ThrdCtrlHeader.ThrdState ));
    DBG3 (( MF__, "pIOWorkQue               = 0x%x", pAsynciCtrl->pIOWorkQue ));
    DBG3 (( MF__, "pIOReqQue                = 0x%x", pAsynciCtrl->pIOReqQue ));
    SLEEP( 100 );
    }

  Tid   = pAsynciCtrl->ThrdCtrlHeader.Tid;
  hThrd = pAsynciCtrl->ThrdCtrlHeader.hThrd;

  // --- send request to ASYNCi
  REQ_TO_ASYNCI ( pAsynciCtrl, pRequest );

  WAIT_THREAD( hThrd, ulWaitTime );

  DBGOUT;
  return ( TRUE );
  }

/*------------------------------*/

static tsp00_Bool sql65k_handle_known_errors ( LONG                rc,
                                         PSZ                 pszLogDeviceName,
                                         PULONG              pAPIRetCode,
                                         PULONG              pulErrCode,
                                         tsp00_Bool                  *pfOk )
  {
  #undef  MF__
  #define MF__ MOD__"sql65k_handle_known_errors"

  DBGIN;


  switch ( rc )
    {
    case ERROR_INVALID_BLOCK_LENGTH:
      MSGCD (( ERR_INVALID_TAPE_BLOCK_COUNT, pszLogDeviceName ));
      *pulErrCode  = ERRCODE_ASYNCIO_INVALID_BLOCKSIZE;
      *pfOk        = FALSE;
      *pAPIRetCode = rc;
      DBGOUT;
      return ( TRUE );

    case ERROR_CANCELLED:
      MSGCD (( INFO_TAPE_OPER_USER_CANCEL_REQ, pszLogDeviceName ));
      *pulErrCode  = ERRCODE_ASYNCIO_OPERATION_CANCELED;
      *pfOk        = FALSE;
      *pAPIRetCode = rc;
      DBGOUT;
      return ( TRUE );

    case ERROR_END_OF_MEDIA:
      DBG1  (( MF__, ERRMSG_ASYNCIO_END_OF_MEDIA ));
      MSGCD (( ERR_END_OF_MEDIA, pszLogDeviceName ));
      *pulErrCode  = ERRCODE_ASYNCIO_END_OF_MEDIA;
      *pfOk        = FALSE;
      *pAPIRetCode = rc;
      DBGOUT;
      return ( TRUE );

    case ERROR_FILEMARK_DETECTED:
    case ERROR_SETMARK_DETECTED :
      DBG1  (( MF__, ERRMSG_ASYNCIO_FILE_SET_MARK ));
      MSGCD (( ERR_FILE_SET_MARK, pszLogDeviceName ));
      *pulErrCode  = ERRCODE_ASYNCIO_FILE_SET_MARK;
      *pfOk        = FALSE;
      *pAPIRetCode = rc;
      DBGOUT;
      return ( TRUE );

    case ERROR_DEVICE_NOT_PARTITIONED:
      DBG1  (( MF__, ERRMSG_ASYNCIO_MEDIA_NOT_PATITIONED ));
      MSGALL(( ERR_MEDIA_NOT_PATITIONED, pszLogDeviceName ));
      *pulErrCode  = ERRCODE_ASYNCIO_MEDIA_NOT_PATITIONED;
      *pfOk        = FALSE;
      *pAPIRetCode = rc;
      DBGOUT;
      return ( TRUE );

    case ERROR_NO_MEDIA_IN_DRIVE:
      DBG1  (( MF__, ERRMSG_ASYNCIO_NO_MEDIA_IN_DEVICE ));
      MSGALL(( ERR_NO_MEDIA_IN_DEVICE, pszLogDeviceName ));
      *pulErrCode  = ERRCODE_ASYNCIO_NO_MEDIA_IN_DEVICE;
      *pfOk        = FALSE;
      *pAPIRetCode = rc;
      DBGOUT;
      return ( TRUE );

    case ERROR_MEDIA_CHANGED:
      DBG1  (( MF__,  ERRMSG_ASYNCIO_MEDIA_CHANGED ));
      MSGALL(( ERR_MEDIA_CHANGED, pszLogDeviceName ));
      *pulErrCode  = ERRCODE_ASYNCIO_MEDIA_CHANGED;
      *pfOk        = FALSE;
      *pAPIRetCode = rc;
      DBGOUT;
      return ( TRUE );

    case ERROR_UNRECOGNIZED_MEDIA:
      DBG1  (( MF__, ERRMSG_ASYNCIO_UNRECOGNIZED_MEDIA ));
      MSGALL(( ERR_UNRECOGNIZED_MEDIA, pszLogDeviceName ));
      *pulErrCode  = ERRCODE_ASYNCIO_UNRECOGNIZED_MEDIA;
      *pfOk        = FALSE;
      *pAPIRetCode = rc;
      DBGOUT;
      return ( TRUE );

    case ERROR_CRC:
      DBG1  (( MF__,  ERRMSG_ASYNCIO_CRC_ERROR ));
      MSGALL(( ERR_CRC_ERROR, pszLogDeviceName ));
      *pulErrCode  = ERRCODE_ASYNCIO_CRC_ERROR;
      *pfOk        = FALSE;
      *pAPIRetCode = rc;
      DBGOUT;
      return ( TRUE );

    case ERROR_DISK_FULL:
      DBG1  (( MF__,  ERRMSG_ASYNCIO_DISK_FULL ));
      MSGALL(( ERR_DISK_FULL, pszLogDeviceName ));
      *pulErrCode  = ERRCODE_ASYNCIO_DISK_FULL;
      *pfOk        = FALSE;
      *pAPIRetCode = rc;
      DBGOUT;
      return ( TRUE );

    case ERROR_ACCESS_DENIED:
      DBG1  (( MF__, ERRMSG_ASYNCIO_ACCESS_DENIED ));
      MSGALL(( ERR_ACCESS_DENIED, pszLogDeviceName ));
      *pulErrCode  = ERRCODE_ASYNCIO_ACCESS_DENIED;
      *pfOk        = FALSE;
      *pAPIRetCode = rc;
      DBGOUT;
      return ( TRUE );

    case ERROR_SHARING_VIOLATION:
      DBG1  (( MF__, ERRMSG_ASYNCIO_SHARING_VIOLATION ));
      MSGALL(( ERR_FILE_SHARING_VIOLATION, pszLogDeviceName, rc ));
      *pulErrCode  = ERRCODE_ASYNCIO_SHARING_VIOLATION;
      *pfOk        = FALSE;
      *pAPIRetCode = rc;
      DBGOUT;
      return ( TRUE );

    case ERROR_PATH_NOT_FOUND:
      DBG1  (( MF__, ERRMSG_ASYNCIO_WRONG_PATH  ));
      MSGALL(( ERR_WRONG_PATH, pszLogDeviceName ));
      *pulErrCode  = ERRCODE_ASYNCIO_WRONG_PATH;
      *pfOk        = FALSE;
      *pAPIRetCode = rc;
      DBGOUT;
      return ( TRUE );

    case ERROR_FILE_NOT_FOUND:
      DBG1  (( MF__, ERRMSG_ASYNCIO_WRONG_FILE_DEVICE_NAME ));
      MSGALL(( ERR_WRONG_FILE_DEVICE_NAME, pszLogDeviceName ));
      *pulErrCode  = ERRCODE_ASYNCIO_WRONG_FILE_DEVICE_NAME;
      *pfOk        = FALSE;
      *pAPIRetCode = rc;
      DBGOUT;
      return ( TRUE );

    case ERROR_WRITE_PROTECT:
      DBG1  (( MF__, ERRMSG_ASYNCIO_MEDIA_WRITE_PROTECTED ));
      MSGALL(( ERR_MEDIA_WRITE_PROTECTED, pszLogDeviceName ));
      *pulErrCode  = ERRCODE_ASYNCIO_MEDIA_WRITE_PROTECTED;
      *pfOk        = FALSE;
      *pAPIRetCode = rc;
      DBGOUT;
      return ( TRUE );

    case ERROR_NOT_READY :
      DBG1  (( MF__, ERRMSG_ASYNCIO_DEVICE_NOT_READY  ));
      MSGALL(( ERR_DEVICE_NOT_READY, pszLogDeviceName ));
      *pulErrCode  = ERRCODE_ASYNCIO_DEVICE_NOT_READY;
      *pfOk        = FALSE;
      *pAPIRetCode = rc;
      DBGOUT;
      return ( TRUE );

    case ERROR_BUSY :
      DBG1  (( MF__, ERRMSG_ASYNCIO_DEVICE_BUSY  ));
      MSGALL(( ERR_DEVICE_BUSY, pszLogDeviceName ));
      *pulErrCode  = ERRCODE_ASYNCIO_DEVICE_BUSY;
      *pfOk        = FALSE;
      *pAPIRetCode = rc;
      DBGOUT;
      return ( TRUE );

    case ERROR_NO_DATA_DETECTED :
      DBG1  (( MF__, ERRMSG_ASYNCIO_DEVICE_BUSY  ));
      MSGALL(( INFO_NO_DATA_DETECTED, pszLogDeviceName ));
      *pulErrCode  = ERRCODE_ASYNCIO_NO_DATA_DETECTED;
      *pfOk        = FALSE;
      *pAPIRetCode = rc;
      DBGOUT;
      return ( TRUE );
    }

    
  DBGOUT;
  return ( FALSE );
  }

/*------------------------------*/

static LONG sql65k_io_read ( HANDLE    DevHandle,
                             ULONG     ulDevType,
                             PVOID     pBlockPtr,
                             ULONG     ulBlockNo,
                             ULONG     ulBlockLength,
                             PULONG    pulBytesRead,
                             PSZ       pszLogDeviceName )
  {
  #undef  MF__
  #define MF__ MOD__"sql65k_io_read"
  ULONG           ulLength;
  ULONG           ulLengthWanted;
  ULONG           ulFilePosLow;
  ULONG           ulFilePosHigh;
  LONG            rc        = NO_ERROR;
  PCHAR           pBuffer;
  int             IORetries = NUMBER_OF_IO_RETRIES;
  INT             Retry     = TAPE_ERROR_RETRY;

  DBGIN;

  switch ( ulDevType )
    {
    case DT_PIPE:
      pBuffer        = (PCHAR)pBlockPtr;
      ulLengthWanted = ulBlockLength;
      do
        {
        rc = READ_FILE ( DevHandle, pBuffer,
                         ulLengthWanted, &ulLength );

        ulLengthWanted -= ulLength;
        pBuffer        += ulLength;
        }
      while (( ulLengthWanted > 0 ) && ( rc == NO_ERROR ) && ( ulLength != 0 ));

      *pulBytesRead = ulBlockLength - ulLengthWanted;
      break;

    case DT_TAPE:
       pBuffer        = (PCHAR)pBlockPtr;
       ulLengthWanted = ulBlockLength;
       do
         {
         rc = READ_FILE ( DevHandle, pBuffer,
                          ulLengthWanted, &ulLength );

         ulLengthWanted -= ulLength;
         pBuffer        += ulLength;
         }
       while (( rc == ERROR_BUS_RESET ||
                rc == ERROR_IO_DEVICE ||
                rc == ERROR_CRC    ) &&
              ( ulLengthWanted > 0 ) && --Retry );

       *pulBytesRead = ulBlockLength - ulLengthWanted;

      break;
    default:
      do
      {
        rc = READ_FILE ( DevHandle, pBlockPtr, ulBlockLength, pulBytesRead);
          
        if ( rc != NO_ERROR )
        {
          MSGALL(( ERR_CANT_READ_FROM_DEVSPACE, ulBlockNo + 1, pszLogDeviceName, rc, ulBlockLength ));
          SLEEP(1000); 
          PAGES_TO_QWORD( ulFilePosLow, ulFilePosHigh, ulBlockNo + 1 );
          
          rc = SET_FILE_PTR ( DevHandle, ulFilePosLow, FILE_BEGIN,
                              &ulFilePosLow, (PLONG)&ulFilePosHigh );
          
          if ( rc != NO_ERROR )
          {
            MSGALL(( ERR_CANT_SEEK_ON_DEVSPACE, pszLogDeviceName, rc ));
            return( rc );
          }
        }
      }
      while ( ( rc != NO_ERROR ) && ( --IORetries ));

      break;
    }

  DBGOUT;
  return ( rc );
  }

/*------------------------------*/

static LONG sql65k_io_write ( HANDLE    DevHandle,
                              ULONG     ulDevType,
                              PVOID     pBlockPtr,
                              ULONG     ulBlockNo,
                              ULONG     ulBlockLength,
                              PULONG    pulBytesWritten )
  {
  #undef  MF__
  #define MF__ MOD__"sql65k_io_write"
  LONG            rc       = NO_ERROR;
  ULONG           ulLength;
  ULONG           ulLengthWanted;
  PCHAR           pBuffer;
  INT             Retry    = TAPE_ERROR_RETRY;

  DBGIN;

  switch ( ulDevType )
    {
    case DT_TAPE:
       pBuffer        = (PCHAR)pBlockPtr;
       ulLengthWanted = ulBlockLength;
       do
         {
         rc = WRITE_FILE ( DevHandle, pBuffer, ulLengthWanted, &ulLength );

         ulLengthWanted -= ulLength;
         pBuffer        += ulLength;
         }
       while (( rc == ERROR_BUS_RESET || rc == ERROR_IO_DEVICE ) &&
              ( ulLengthWanted > 0 ) && --Retry );

       *pulBytesWritten = ulBlockLength - ulLengthWanted;
      break;

    default:
      rc = WRITE_FILE ( DevHandle, pBlockPtr, ulBlockLength, pulBytesWritten );
      break;
    }

  DBGOUT;
  return ( rc );
  }

/*------------------------------*/

static LONG sql65k_block_0_io ( PASYNCI_CTRL_REC            pAsynciCtrl,
                                DLQ_VASYNOPEN_REQ_ARGS_REC  *pVasynOpenArgs )
  {
  #undef  MF__
  #define MF__   MOD__"sql65k_block_0_io"
  PCHAR   pTmpBuffer;
  ULONG   ulBlock_0_Size , ulLengthWanted ;
  ULONG   ulWritten;
  ULONG   ulRead;
  PCHAR   pBuffer         = NULL;
  LONG    rc              = NO_ERROR;
  ULONG   ulTapeMarkRetry = 1;
  DBGIN;

  ulLengthWanted = ulBlock_0_Size = pVasynOpenArgs->fIsDevspace
                 ? pVasynOpenArgs->ulBlockSize
                 : pVasynOpenArgs->ulBlockSize *  pVasynOpenArgs->ulMaxBlockCnt;

  DBG1 (( MF__, "ulBlock_0_Size = %d", ulBlock_0_Size ));

  rc = ALLOC_MEM ((PPVOID)&pBuffer,
                  ulBlock_0_Size + kgs.ulSystemPageSize );

  if ( rc != NO_ERROR )
    {
    pVasynOpenArgs->ulErrCode  = ERRCODE_ASYNCIO_ALLOC_MEM;
    pVasynOpenArgs->APIRetCode = rc;
    pVasynOpenArgs->fOk        = FALSE;

    DBGOUT;
    return ( rc );
    }
  else
    {
    // --- alignment needed for none cached file operations
    pTmpBuffer = (PCHAR)ALIGN( (tsp00_Longuint)pBuffer, kgs.ulSystemPageSize );
    }

  if ( pAsynciCtrl->fForWriting )
    {
    memset (pTmpBuffer, '\0', ulBlock_0_Size );

    // --- fill buffer
    //
    //     0x12345678 ->  pTmpBuffer[2] = 0x56
    //                    pTmpBuffer[3] = 0x78
    //                    pTmpBuffer[4] = 0x12
    //                    pTmpBuffer[5] = 0x34
    //
    pTmpBuffer[2] = (CHAR) (( ulBlock_0_Size % KBYTE_64 ) / 256);
    pTmpBuffer[3] = (CHAR) (( ulBlock_0_Size % KBYTE_64 ) % 256);
    pTmpBuffer[4] = (CHAR) (( ulBlock_0_Size / KBYTE_64 ) / 256);
    pTmpBuffer[5] = (CHAR) (( ulBlock_0_Size / KBYTE_64 ) % 256);

    SAPDB_memcpy ( pTmpBuffer + PRODUCT_ID_POS,
             PRODUCT_ID_STRING,
             sizeof (PRODUCT_ID_STRING) - 1);

    DBG2 ((MF__, "Block 0 contents", pTmpBuffer, ulBlock_0_Size ));


    rc = sql65k_io_write ( pAsynciCtrl->DevHandle, pAsynciCtrl->ulDevType,
                           pTmpBuffer, 0, ulBlock_0_Size , &ulWritten );

    if ( rc != NO_ERROR )
      {
      DBG1  (( MF__, "Cannot write to '%s', rc = %d",
               pAsynciCtrl->szLogDeviceName, rc ));

      if ( !sql65k_handle_known_errors ( rc, pAsynciCtrl->szLogDeviceName,
                                         &pVasynOpenArgs->APIRetCode,
                                         &pVasynOpenArgs->ulErrCode,
                                         &pVasynOpenArgs->fOk ) )
        {
        pVasynOpenArgs->ulErrCode  = ERRCODE_ASYNCIO_CANT_WRITE_FILE_DEV_PIPE;
        pVasynOpenArgs->fOk        = FALSE;
        pVasynOpenArgs->APIRetCode = rc;
        MSGALL(( ERR_CANT_WRITE_FILE_DEV_PIPE,
                 pAsynciCtrl->szLogDeviceName, rc ));
        }
      }
    else if ( ulWritten != ulBlock_0_Size )
      {
      DBG1  (( MF__, "Bad length of data %s'", pAsynciCtrl->szLogDeviceName ));
      MSGALL(( ERR_BAD_DATA_LEN_WRIT_READ, pAsynciCtrl->szLogDeviceName ));

      pVasynOpenArgs->ulErrCode  = ERRCODE_ASYNCIO_BAD_LENGTH_READ_WRITTEN;
      pVasynOpenArgs->APIRetCode = 0;
      pVasynOpenArgs->fOk        = FALSE;
      rc                         = ERROR_INVALID_DATA;
      }
    }
  else
    {
    do
      {
      rc = sql65k_io_read ( pAsynciCtrl->DevHandle, pAsynciCtrl->ulDevType,
                            pTmpBuffer, 0, ulBlock_0_Size , &ulRead,
                            pAsynciCtrl->szLogDeviceName );
      }
    while((rc == ERROR_FILEMARK_DETECTED) && !ulRead && ulTapeMarkRetry--);

    if ( rc != ERROR_INVALID_BLOCK_LENGTH )
      {
      if ( rc != NO_ERROR )
        {
        DBG1  (( MF__, "Cannot read from '%s', rc = %d",
                 pAsynciCtrl->szLogDeviceName, rc ));

        if ( !sql65k_handle_known_errors ( rc, pAsynciCtrl->szLogDeviceName,
                                           &pVasynOpenArgs->APIRetCode,
                                           &pVasynOpenArgs->ulErrCode,
                                           &pVasynOpenArgs->fOk ) )
          {
          pVasynOpenArgs->ulErrCode  = ERRCODE_ASYNCIO_CANT_READ_FILE_DEV_PIPE;
          pVasynOpenArgs->fOk        = FALSE;
          pVasynOpenArgs->APIRetCode = rc;
          MSGALL(( ERR_CANT_READ_FILE_DEV_PIPE,
                   pAsynciCtrl->szLogDeviceName, rc ));
          }
        }
      else if ( ulRead != ulBlock_0_Size )
        {
        DBG1  (( MF__, "Bad length of data %s'", pAsynciCtrl->szLogDeviceName ));
        MSGALL(( ERR_BAD_DATA_LEN_WRIT_READ, pAsynciCtrl->szLogDeviceName ));

        pVasynOpenArgs->ulErrCode  = ERRCODE_ASYNCIO_BAD_LENGTH_READ_WRITTEN;
        pVasynOpenArgs->APIRetCode = rc;
        pVasynOpenArgs->fOk        = FALSE;
        rc                         = ERROR_INVALID_DATA;
        }
      }

    if ( rc == NO_ERROR )
      {
      teo54Page0Info *pInfo = (teo54Page0Info *)(pTmpBuffer  + PAGE_0_RECORD_OFFSET_EO54);

      if ( !strcmp(pInfo->magicHeader_eo54, MAGIC_HEADER_EO54) )
        { /* UNIX Page layout */
        ulLengthWanted   = eo54RestoredInt4(pInfo->blockSize_eo54);
        }
      else
        {
        ulLengthWanted = (ULONG ) ( (UCHAR ) pTmpBuffer[4] << 24 ) +
                                  ( (UCHAR ) pTmpBuffer[5] << 16 ) +
                                  ( (UCHAR ) pTmpBuffer[2] <<  8 ) +
                                    (UCHAR ) pTmpBuffer[3] ;
        }

      if ( ulLengthWanted != ulBlock_0_Size )
        {
        MSGCD (( ERR_ASYNCIO_BLOCK_SIZE, ulBlock_0_Size , ulLengthWanted ));
        pVasynOpenArgs->ulMaxBlockCnt = ulLengthWanted /
                                        pVasynOpenArgs->ulBlockSize ;
        pVasynOpenArgs->ulErrCode  = ERRCODE_ASYNCIO_ILL_BLOCK_COUNT;
        pVasynOpenArgs->fOk        = FALSE;
        rc                         = ERROR_INVALID_DATA;
        }
      }
    }

  if ( rc != NO_ERROR )
    FREE_MEM ( (PVOID)pBuffer );
  else
    {
    rc = FREE_MEM ( (PVOID)pBuffer );

    if ( rc != NO_ERROR && pVasynOpenArgs->fOk )
      {
      pVasynOpenArgs->ulErrCode  = ERRCODE_ASYNCIO_FREE_MEM;
      pVasynOpenArgs->APIRetCode = rc;
      pVasynOpenArgs->fOk        = FALSE;
      }
    }


  DBGOUT;
  return ( rc );
  }

/*------------------------------*/


static tsp00_Bool sql65k_file_open ( PASYNCI_CTRL_REC            pAsynciCtrl,
                               DLQ_VASYNOPEN_REQ_ARGS_REC  *pVasynOpenArgs )
  {
  #undef  MF__
  #define MF__ MOD__"sql65k_file_open"
  DWORD                fdwCreate;
  DWORD                fdwAccess;
  DWORD                fdwShare;
  DWORD                fdwFlags;
  LONG                 rc        = NO_ERROR;
  tsp00_Uint           ulTmp     = 0;
  tsp00_Int            lAtt;
  ULONG                ulFilePosLow;

  DBGIN;

  pVasynOpenArgs->fOk = TRUE;

  rc = sql44c_get_file_info ( pAsynciCtrl->szPhysDeviceName, &lAtt,
                              &ulTmp, &ulTmp );

  if (( rc != NO_ERROR ) && ( rc != ERROR_FILE_NOT_FOUND ))
    {
    DBG1  (( MF__, "Query file info error on '%s', rc = %ld",
             pAsynciCtrl->szLogDeviceName, rc ));

    if ( !sql65k_handle_known_errors ( rc, pAsynciCtrl->szLogDeviceName,
                                       &pVasynOpenArgs->APIRetCode,
                                       &pVasynOpenArgs->ulErrCode,
                                       &pVasynOpenArgs->fOk ) )
      {
      pVasynOpenArgs->ulErrCode  = ERRCODE_ASYNCIO_GET_FILE_INFO;
      pVasynOpenArgs->fOk        = FALSE;
      pVasynOpenArgs->APIRetCode = rc;
      MSGALL(( ERR_GET_FILE_INFO,
               pAsynciCtrl->szLogDeviceName, rc ));
      }

    DBGOUT;
    return ( FALSE );
    }

  if ( lAtt == -1 )
    {
    pVasynOpenArgs->fOk       = FALSE;
    pVasynOpenArgs->ulErrCode = ERRCODE_ASYNCIO_WRONG_FILE_ATTRIB;
    MSGALL(( ERR_UNKNOWN_FILE_ATTRIBUTE, pAsynciCtrl->szLogDeviceName ));
    DBG1  (( MF__, "'%s' has a unknown file attribute", pAsynciCtrl->szLogDeviceName));
    DBGOUT;
    return ( FALSE );
    }
  else if ((lAtt & FILE_DIRECTORY) == FILE_DIRECTORY)
    {
    pVasynOpenArgs->fOk       = FALSE;
    pVasynOpenArgs->ulErrCode = ERRCODE_ASYNCIO_IO_ON_DIR;
    MSGALL(( ERR_IO_ON_DIR, pAsynciCtrl->szLogDeviceName ));
    DBG1  (( MF__, "No I/O on directory '%s' !", pAsynciCtrl->szLogDeviceName));
    DBGOUT;
    return ( FALSE );
    }
  else if ((lAtt & FILE_SYSTEM) == FILE_SYSTEM)
    {
    pVasynOpenArgs->fOk       = FALSE;
    pVasynOpenArgs->ulErrCode = ERRCODE_ASYNCIO_IO_ON_SYS;
    MSGALL(( ERR_IO_ON_SYS, pAsynciCtrl->szLogDeviceName ));
    DBG1  (( MF__, "No I/O on system file '%s' !", pAsynciCtrl->szLogDeviceName));
    DBGOUT;
    return ( FALSE );
    }
  else if (((lAtt & FILE_READONLY) == FILE_READONLY) && ( pAsynciCtrl->fForWriting ))
    {
    pVasynOpenArgs->fOk       = FALSE;
    pVasynOpenArgs->ulErrCode = ERRCODE_ASYNCIO_IO_READ_ONLY;
    MSGALL(( ERR_IO_ON_READ_ONLY, pAsynciCtrl->szLogDeviceName ));
    DBG1  (( MF__, "No I/O on read-only file '%s' !", pAsynciCtrl->szLogDeviceName));
    DBGOUT;
    return ( FALSE );
    }


  DBG3 (( MF__, "Opening devspace: %s", pAsynciCtrl->szLogDeviceName ));


  if ( pAsynciCtrl->fForWriting )
    {
    fdwAccess = GENERIC_READ | GENERIC_WRITE;
    fdwFlags  = FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN;

    if (( '\\' != pAsynciCtrl->szPhysDeviceName[0] ) ||
        ( '\\' != pAsynciCtrl->szPhysDeviceName[1] )) 
    {   
        // it's NOT a network share name, no buffering! 
        fdwFlags  |= FILE_FLAG_NO_BUFFERING;   
    }

    if ( pAsynciCtrl->fIsDevspace )
      {
      fdwCreate = OPEN_EXISTING;
      fdwShare  = FILE_SHARE_READ | FILE_SHARE_WRITE;
      }
    else
      {
      fdwCreate = CREATE_ALWAYS;
      fdwShare  = FILE_SHARE_READ;
      }
    }
  else
    {
    fdwFlags  = FILE_ATTRIBUTE_NORMAL  |
                FILE_FLAG_NO_BUFFERING;
    fdwCreate = OPEN_EXISTING;
    fdwAccess = GENERIC_READ;
    fdwShare  = FILE_SHARE_READ | FILE_SHARE_WRITE;

    if ( pAsynciCtrl->fIsDevspace == FALSE )
      fdwFlags |= FILE_FLAG_SEQUENTIAL_SCAN;
    }

  pAsynciCtrl->DevHandle = sql65k_create_file( pAsynciCtrl->szPhysDeviceName,
                                               fdwAccess, fdwShare,
                                               &kgs.AdminSA,
                                               fdwCreate,
                                               fdwFlags,
                                               NULL );

  if (pAsynciCtrl->DevHandle == INVALID_HANDLE_VALUE)
    rc = GetLastError();
  else
    rc = NO_ERROR;

  if ( rc != NO_ERROR )
    {
    DBG1  (( MF__, "Cannot open file '%s', rc = %d",
             pAsynciCtrl->szLogDeviceName, rc ));

    if ( !sql65k_handle_known_errors ( rc, pAsynciCtrl->szLogDeviceName,
                                       &pVasynOpenArgs->APIRetCode,
                                       &pVasynOpenArgs->ulErrCode,
                                       &pVasynOpenArgs->fOk ) )
      {
      pVasynOpenArgs->ulErrCode  = ERRCODE_ASYNCIO_OPEN_FILE_DEV_PIPE;
      pVasynOpenArgs->fOk        = FALSE;
      pVasynOpenArgs->APIRetCode = rc;
      MSGALL(( ERR_CANT_OPEN_FILE,
               pAsynciCtrl->szLogDeviceName, rc ));
      }


    pAsynciCtrl->DevHandle = INVALID_HANDLE_VALUE;

    DBGOUT;
    return ( FALSE );
    }

  if ( pAsynciCtrl->fIsDevspace )
    {
    //
    // --- skip block 0
    //
    ulFilePosLow = XPARAM(ulDBPageSize);

    rc = SET_FILE_PTR ( pAsynciCtrl->DevHandle, 0, FILE_BEGIN,
                        &ulFilePosLow, NULL );

    if ( rc != NO_ERROR )
      {
      DBG1  (( MF__, "Cannot seek on: %s, rc = %d",
               pAsynciCtrl->szLogDeviceName, rc ));

      if ( !sql65k_handle_known_errors ( rc, pAsynciCtrl->szLogDeviceName,
                                         &pVasynOpenArgs->APIRetCode,
                                         &pVasynOpenArgs->ulErrCode,
                                         &pVasynOpenArgs->fOk ) )
        {
        pVasynOpenArgs->ulErrCode  = ERRCODE_ASYNCIO_CANT_SEEK_FILE_DEV_PIPE;
        pVasynOpenArgs->fOk        = FALSE;
        pVasynOpenArgs->APIRetCode = rc;
        MSGALL(( ERR_CANT_SEEK_FILE_DEV_PIPE,
                 pAsynciCtrl->szLogDeviceName, rc ));
        }


      CLOSE_FILE( pAsynciCtrl->DevHandle );
      pAsynciCtrl->DevHandle = INVALID_HANDLE_VALUE;

      DBGOUT;
      return ( FALSE );
      }
    }
  else
    {
    // --- write/read block 0
    if ( sql65k_block_0_io ( pAsynciCtrl, pVasynOpenArgs ) != NO_ERROR )
      {
      CLOSE_FILE( pAsynciCtrl->DevHandle );
      pAsynciCtrl->DevHandle = INVALID_HANDLE_VALUE;

      DBGOUT;
      return ( FALSE );
      }
    }


  DBGOUT;
  return ( pVasynOpenArgs->fOk );
  }
/*------------------------------*/

static VOID _System sql65k_pipe_watchdog ( PASYNCI_CTRL_REC   pAsynciCtrl )
  {
  #undef  MF__
  #define MF__ MOD__"sql65k_pipe_watchdog"
  LONG                          rc = NO_ERROR;
  PTASK_CTRL_REC                pReqTaskCtrl;
  ULONG                         ulMode;
  HANDLE                        DevHandle;
  ULONG                         ulShare;

  DBGIN;

  __try 
   {
#   if defined(_FASTCAP)
     CAPNameThread ("PipeWatchDog");
#   endif

    pReqTaskCtrl = pAsynciCtrl->PipeWatchdogCtrl.pReqTaskCtrl;

    while (( !*(pAsynciCtrl->cancelPointer) ) &&
           ( !pAsynciCtrl->PipeWatchdogCtrl.fStopWatchDog ))
      {
      SLEEP(WATCH_DOG_SLEEP_TIME);
      }

    if ( pAsynciCtrl->PipeWatchdogCtrl.fStopWatchDog == FALSE )
      {
      if ( pAsynciCtrl->fForWriting )
        {
        ulShare = FILE_SHARE_WRITE;
        ulMode  = GENERIC_READ;
        }
      else
        {
        ulShare = FILE_SHARE_READ;
        ulMode  = GENERIC_WRITE;
        }

      DevHandle = CreateFile( pAsynciCtrl->szPhysDeviceName,
                              ulMode,
                              ulShare,
                              &kgs.AdminSA,
                              OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL,
                              NULL );

      if (DevHandle != INVALID_HANDLE_VALUE )
        CLOSE_FILE( DevHandle );
      }
  }
  __except( sql50k_excepthandler(GetExceptionInformation()) ) {;}

  DBGOUT;
  EXITTHREAD ( 0, pAsynciCtrl->PipeWatchdogCtrl.hThrd );

  return;
  }

/*------------------------------*/

static LONG sql65k_create_pipe_watchdog( PASYNCI_CTRL_REC   pAsynciCtrl,
                                         PTASK_CTRL_REC     pReqTaskCtrl )
  {
  #undef  MF__
  #define MF__ MOD__"sql65k_pipe_watchdog"
  LONG   rc = NO_ERROR;


  pAsynciCtrl->PipeWatchdogCtrl.pReqTaskCtrl  = pReqTaskCtrl;
  pAsynciCtrl->PipeWatchdogCtrl.fStopWatchDog = FALSE;

  // --- create Pipe WATCHDOG-Thread
  rc = CREATE_THREAD( (PULONG)&pAsynciCtrl->PipeWatchdogCtrl.Tid,
                      &pAsynciCtrl->PipeWatchdogCtrl.hThrd,
                      sql65k_pipe_watchdog,
                      pAsynciCtrl,       // argument
                      CREATE_THRD_SUSPENDED,
                      4096 );

  if ( rc != NO_ERROR )
    {
    MSGCD((ERR_CREATING_THREAD, "PIPEWATCHDOG", rc));
    DBG1 ((MF__, "Cannot create thread: '%s', rc = %d", "PIPEWATCHDOG", rc))
    }
  else
    {
    rc = RESUME_THREAD(pAsynciCtrl->PipeWatchdogCtrl.Tid,
                       pAsynciCtrl->PipeWatchdogCtrl.hThrd );

    if (rc != NO_ERROR)
      {
      MSGCD ((ERR_RESUMING_THREAD, "PIPEWATCHDOG", rc));
      DBG1 ((MF__, "Cannot resume thread: '%s', rc = %d", "PIPEWATCHDOG", rc))
      }
    }

  DBGOUT;
  return ( rc );
  }

/*------------------------------*/

static tsp00_Bool sql65k_pipe_open ( PASYNCI_CTRL_REC            pAsynciCtrl,
                               DLQ_VASYNOPEN_REQ_ARGS_REC  *pVasynOpenArgs )
  {
  #undef  MF__
  #define MF__ MOD__"sql65k_pipe_open"
  LONG         rc      = NO_ERROR;
  ULONG        ulPipeBuffSize;
  ULONG        ulMode;
  ULONG        ulShare;

  DBGIN;

  pVasynOpenArgs->fOk = TRUE;

  ulPipeBuffSize = pVasynOpenArgs->ulBlockSize *
                   pVasynOpenArgs->ulMaxBlockCnt;

  if ( pAsynciCtrl->fForWriting )
    ulMode = PIPE_ACCESS_OUTBOUND;
  else
    ulMode = PIPE_ACCESS_INBOUND;

  pAsynciCtrl->DevHandle = CreateNamedPipe(pAsynciCtrl->szPhysDeviceName,
                                           ulMode,
                                           PIPE_TYPE_BYTE     |
                                           PIPE_READMODE_BYTE |
                                           PIPE_WAIT,
                                           1,
                                           ulPipeBuffSize,
                                           ulPipeBuffSize,
                                           INFINITE,
                                           &kgs.AdminSA);

  if (pAsynciCtrl->DevHandle == INVALID_HANDLE_VALUE )
    rc = GetLastError();

  if (rc == NO_ERROR)
    {
    sql65k_create_pipe_watchdog( pAsynciCtrl, pVasynOpenArgs->pReqTaskCtrl );

    if (!ConnectNamedPipe((HANDLE)pAsynciCtrl->DevHandle, NULL))
      {
      rc = GetLastError();
      CloseHandle((HANDLE)pAsynciCtrl->DevHandle);
      }

    // --- stop watchdog
    pAsynciCtrl->PipeWatchdogCtrl.fStopWatchDog = TRUE;
    SLEEP(WATCH_DOG_SLEEP_TIME);
    }
  else if ((rc                        == ERROR_PIPE_BUSY )    ||
           ( rc                       == ERROR_INVALID_NAME ) ||
           ( sql02_get_platform_id () != VER_PLATFORM_WIN32_NT ))
    {
    if ( pAsynciCtrl->fForWriting )
      {
      ulShare = FILE_SHARE_READ;
      ulMode  = GENERIC_WRITE;
      }
    else
      {
      ulShare = FILE_SHARE_WRITE;
      ulMode  = GENERIC_READ;
      }

    pAsynciCtrl->DevHandle = CreateFile( pAsynciCtrl->szPhysDeviceName,
                                         ulMode,
                                         ulShare,
                                         &kgs.AdminSA,
                                         OPEN_EXISTING,
                                         FILE_ATTRIBUTE_NORMAL,
                                         NULL );

    if (pAsynciCtrl->DevHandle == INVALID_HANDLE_VALUE )
      rc = GetLastError();
    else
      rc = NO_ERROR;
    }

  if ( rc != NO_ERROR )
    {
    DBG1  (( MF__, "Cannot open device '%s', rc = %d",
             pAsynciCtrl->szLogDeviceName, rc ));

     if ( !sql65k_handle_known_errors ( rc, pAsynciCtrl->szLogDeviceName,
                                        &pVasynOpenArgs->APIRetCode,
                                        &pVasynOpenArgs->ulErrCode,
                                        &pVasynOpenArgs->fOk ) )
      {
      pVasynOpenArgs->ulErrCode  = ERRCODE_ASYNCIO_OPEN_FILE_DEV_PIPE;
      pVasynOpenArgs->fOk        = FALSE;
      pVasynOpenArgs->APIRetCode = rc;
      MSGALL(( ERR_CANT_OPEN_PIPE,
               pAsynciCtrl->szLogDeviceName, rc ));
      }


    CLOSE_FILE( pAsynciCtrl->DevHandle );
    pAsynciCtrl->DevHandle = INVALID_HANDLE_VALUE;

    DBGOUT;
    return ( FALSE );
    }
  else if ( *(pVasynOpenArgs->cancelPointer) )
    {
    MSGCD (( INFO_PIPE_CONNECT_CANCELLED ))
    pVasynOpenArgs->ulErrCode  = ERRCODE_ASYNCIO_PIPE_CONN_CANCELLED;
    pVasynOpenArgs->fOk        = FALSE;
    pVasynOpenArgs->APIRetCode = rc;

    CLOSE_FILE( pAsynciCtrl->DevHandle );
    pAsynciCtrl->DevHandle = INVALID_HANDLE_VALUE;

    DBGOUT;
    return ( FALSE );
    }

  // --- write/read block 0
  if ( sql65k_block_0_io ( pAsynciCtrl, pVasynOpenArgs ) != NO_ERROR )
    {
    CLOSE_FILE( pAsynciCtrl->DevHandle );
    pAsynciCtrl->DevHandle = INVALID_HANDLE_VALUE;

    DBGOUT;
    return ( FALSE );
    }


  DBGOUT;
  return ( pVasynOpenArgs->fOk );
  }

/*------------------------------*/

static tsp00_Bool sql65k_nul_open ( PASYNCI_CTRL_REC            pAsynciCtrl,
                              DLQ_VASYNOPEN_REQ_ARGS_REC  *pVasynOpenArgs )
  {
  #undef  MF__
  #define MF__ MOD__"sql65k_nul_open"
  LONG         rc      = NO_ERROR;

  DBGIN;

  pVasynOpenArgs->fOk = TRUE;

  if ( pAsynciCtrl->fForWriting )
    {
    pAsynciCtrl->DevHandle = CreateFile( pAsynciCtrl->szPhysDeviceName,
                                         GENERIC_WRITE, 0,
                                         &kgs.AdminSA,
                                         OPEN_EXISTING, 0, NULL );

    if (pAsynciCtrl->DevHandle == INVALID_HANDLE_VALUE)
      rc = GetLastError();
    }
  else
    rc = ERROR_INVALID_FUNCTION;

  if ( rc != NO_ERROR )
    {
    DBG1  (( MF__, "Cannot open device '%s', rc = %d",
             pAsynciCtrl->szLogDeviceName, rc ));

    if ( !sql65k_handle_known_errors ( rc, pAsynciCtrl->szLogDeviceName,
                                       &pVasynOpenArgs->APIRetCode,
                                       &pVasynOpenArgs->ulErrCode,
                                       &pVasynOpenArgs->fOk ) )
      {
      pVasynOpenArgs->ulErrCode  = ERRCODE_ASYNCIO_OPEN_FILE_DEV_PIPE;
      pVasynOpenArgs->fOk        = FALSE;
      pVasynOpenArgs->APIRetCode = rc;
      MSGALL(( ERR_CANT_OPEN_FILE,
               pAsynciCtrl->szLogDeviceName, rc ));
      }

    CLOSE_FILE( pAsynciCtrl->DevHandle );
    pAsynciCtrl->DevHandle = INVALID_HANDLE_VALUE;

    DBGOUT;
    return ( FALSE );
    }

  DBGOUT;
  return ( pVasynOpenArgs->fOk );
  }

/*------------------------------*/

static tsp00_Bool sql65k_tape_open ( PASYNCI_CTRL_REC            pAsynciCtrl,
                               DLQ_VASYNOPEN_REQ_ARGS_REC  *pVasynOpenArgs )
  {
  #undef  MF__
  #define MF__ MOD__"sql65k_tape_open"
  LONG                rc = NO_ERROR;
  DWORD               fdwAccess;
  DWORD               dwTapeStatus;
  INT                 i;

  DBGIN;


  pVasynOpenArgs->fOk = TRUE;


  /*
  if ( pAsynciCtrl->fForWriting )
    fdwAccess = GENERIC_READ | GENERIC_WRITE;
  else
    fdwAccess = GENERIC_READ;

  siehe PTS: 1107210-12   
  */
  fdwAccess = GENERIC_READ | GENERIC_WRITE;

  // --- calculate tape block size
  pAsynciCtrl->ulTapeBlkSize = pAsynciCtrl->ulBlockSize *
                               pAsynciCtrl->ulMaxBlockCnt;

  for ( i = -1; pAsynciCtrl->ulTapeBlkSize; i++ )
    pAsynciCtrl->ulTapeBlkSize = pAsynciCtrl->ulTapeBlkSize >> 1;

  pAsynciCtrl->ulTapeBlkSize = 1 << i;

  // --- try write through and no buffering first!
  pAsynciCtrl->DevHandle = sql65k_create_file( pAsynciCtrl->szPhysDeviceName,
                                               fdwAccess, 0,
                                               &kgs.AdminSA,
                                               OPEN_EXISTING,
                                               FILE_FLAG_NO_BUFFERING,
                                               NULL );

  if ( pAsynciCtrl->DevHandle == INVALID_HANDLE_VALUE )
    {
    rc = GetLastError();
 
    MSGD (( INFO_BUFFERED_ASYNCIO,  pAsynciCtrl->szLogDeviceName ));

    pAsynciCtrl->DevHandle = sql65k_create_file( pAsynciCtrl->szPhysDeviceName,
                                                 fdwAccess, 0,
                                                 &kgs.AdminSA,
                                                 OPEN_EXISTING,
                                                 0,
                                                 NULL );

    }

  if (pAsynciCtrl->DevHandle == INVALID_HANDLE_VALUE)
    rc = GetLastError();
 
  if ( rc != NO_ERROR )
    {
    DBG1  (( MF__, "Cannot open device '%s', rc = %d",
             pAsynciCtrl->szLogDeviceName, rc ));

    if ( !sql65k_handle_known_errors ( rc, pAsynciCtrl->szLogDeviceName,
                                       &pVasynOpenArgs->APIRetCode,
                                       &pVasynOpenArgs->ulErrCode,
                                       &pVasynOpenArgs->fOk ) )
      {
      pVasynOpenArgs->ulErrCode  = ERRCODE_ASYNCIO_OPEN_FILE_DEV_PIPE;
      pVasynOpenArgs->fOk        = FALSE;
      pVasynOpenArgs->APIRetCode = rc;
      MSGALL(( ERR_CANT_OPEN_TAPE,
               pAsynciCtrl->szLogDeviceName, rc ));
      }

    CLOSE_FILE( pAsynciCtrl->DevHandle );
    pAsynciCtrl->DevHandle = INVALID_HANDLE_VALUE;

    DBGOUT;
    return ( FALSE );
    }

  // --- check if media was changed
  dwTapeStatus = GetTapeStatus( (HANDLE) pAsynciCtrl->DevHandle );

  if ( dwTapeStatus == ERROR_MEDIA_CHANGED )
    pAsynciCtrl->fMediaChanged = TRUE;
  else
    pAsynciCtrl->fMediaChanged = FALSE;


  sql65k_init_dlt_tape ( pAsynciCtrl );

  if (( sql65k_get_drive_info ( pAsynciCtrl,
                                &pVasynOpenArgs->ulErrCode,
                                &pVasynOpenArgs->APIRetCode,
                                &pVasynOpenArgs->fOk ) == FALSE )   ||
      ( sql65k_tape_loading   ( pAsynciCtrl ,
                                pVasynOpenArgs->pReqTaskCtrl,
                                &pVasynOpenArgs->ulErrCode,
                                &pVasynOpenArgs->APIRetCode,
                                &pVasynOpenArgs->fOk ,
                                TAPE_LOAD) == FALSE )               ||
      ( sql65k_tape_locking   ( pAsynciCtrl,
                                &pVasynOpenArgs->ulErrCode,
                                &pVasynOpenArgs->APIRetCode,
                                &pVasynOpenArgs->fOk,
                                LOCK_TAPE ) == FALSE )              ||
      ( sql65k_get_media_info ( pAsynciCtrl,
                                pVasynOpenArgs->pReqTaskCtrl,
                                &pVasynOpenArgs->ulErrCode,
                                &pVasynOpenArgs->APIRetCode,
                                &pVasynOpenArgs->fOk ) == FALSE )   ||
      ( sql65k_set_drive_info ( pAsynciCtrl,
                                &pVasynOpenArgs->ulErrCode,
                                &pVasynOpenArgs->APIRetCode,
                                &pVasynOpenArgs->fOk ) == FALSE )   ||
      ( sql65k_set_media_info ( pAsynciCtrl,
                                &pVasynOpenArgs->ulErrCode,
                                &pVasynOpenArgs->APIRetCode,
                                &pVasynOpenArgs->fOk ) == FALSE )   ||
 #if defined REWIND_TAPE
      ( sql65k_tape_rewind   (  pAsynciCtrl,
                                &pVasynOpenArgs->ulErrCode,
                                &pVasynOpenArgs->APIRetCode,
                                &pVasynOpenArgs->fOk ) == FALSE )   ||
      ( sql65k_tape_tension  (  pAsynciCtrl,
                                &pVasynOpenArgs->ulErrCode,
                                &pVasynOpenArgs->APIRetCode,
                                &pVasynOpenArgs->fOk ) == FALSE ))
 #else
        
      ( sql65k_tape_autorewind( pAsynciCtrl,
                                &pVasynOpenArgs->ulErrCode,
                                &pVasynOpenArgs->APIRetCode,
                                &pVasynOpenArgs->fOk ) == FALSE ))
 #endif
    {
    sql65k_tape_open_cleanup( pAsynciCtrl, pVasynOpenArgs );

    DBGOUT;
    return ( FALSE );
    }

  // --- write/read block 0
  rc = sql65k_block_0_io ( pAsynciCtrl, pVasynOpenArgs );

  if ( rc != NO_ERROR )
    {
    if (( rc == ERROR_INVALID_BLOCK_LENGTH ) && ( pAsynciCtrl->fForWriting == FALSE ))
      {
      // ---- retry with different block sizes
      //
	  do
	    {
        if ( pAsynciCtrl->ulTapeBlkSize <= MIN_TAPE_BLOCK_SIZE )
          { // - we 've reached the minimum block size
          if ( !sql65k_handle_known_errors ( rc, pAsynciCtrl->szLogDeviceName,
                                             &pVasynOpenArgs->APIRetCode,
                                             &pVasynOpenArgs->ulErrCode,
                                             &pVasynOpenArgs->fOk ) )
            {
            pVasynOpenArgs->ulErrCode  = ERRCODE_ASYNCIO_CANT_READ_FILE_DEV_PIPE;
            pVasynOpenArgs->fOk        = FALSE;
            pVasynOpenArgs->APIRetCode = rc;
            MSGALL(( ERR_CANT_READ_FILE_DEV_PIPE, pAsynciCtrl->szLogDeviceName, rc ));
            break;
            }
          }
        else
          {
          pAsynciCtrl->ulTapeBlkSize = pAsynciCtrl->ulTapeBlkSize >> 1;

          if (( sql65k_set_media_info ( pAsynciCtrl,
                                        &pVasynOpenArgs->ulErrCode,
                                        &pVasynOpenArgs->APIRetCode,
                                        &pVasynOpenArgs->fOk ) == TRUE )   &&
              ( sql65k_tape_autorewind( pAsynciCtrl,
                                        &pVasynOpenArgs->ulErrCode,
                                        &pVasynOpenArgs->APIRetCode,
                                        &pVasynOpenArgs->fOk ) == TRUE ))
            {
            // --- retry reading tape with new tape block size
            rc = sql65k_block_0_io ( pAsynciCtrl, pVasynOpenArgs );

            if ( rc == NO_ERROR )
              break;

            if ( rc == ERROR_INVALID_BLOCK_LENGTH )
              pVasynOpenArgs->fOk = TRUE;
            }
          }
        }
      while ( pVasynOpenArgs->fOk );
      }

    if ( !pVasynOpenArgs->fOk )
      sql65k_tape_open_cleanup( pAsynciCtrl, pVasynOpenArgs );
    }


  DBGOUT;
  return ( pVasynOpenArgs->fOk );
  }

/*------------------------------*/

static VOID sql65k_tape_open_cleanup( PASYNCI_CTRL_REC            pAsynciCtrl,
                                      DLQ_VASYNOPEN_REQ_ARGS_REC  *pVasynOpenArgs )
  {
  #undef  MF__
  #define MF__ MOD__"sql65k_tape_open_cleanup"
  DLQ_REC              TmpRequest;

  DBGIN;

  TmpRequest.ReqArgs.VasynOpenRequest = *pVasynOpenArgs;
  if (pAsynciCtrl->fAutoRewind)
    {
    sql65k_tape_rewind   ( pAsynciCtrl,
                         &TmpRequest.ReqArgs.VasynOpenRequest.ulErrCode,
                         &TmpRequest.ReqArgs.VasynOpenRequest.APIRetCode,
                         &TmpRequest.ReqArgs.VasynOpenRequest.fOk );
    }
  sql65k_tape_locking ( pAsynciCtrl,
                        &TmpRequest.ReqArgs.VasynOpenRequest.ulErrCode,
                        &TmpRequest.ReqArgs.VasynOpenRequest.APIRetCode,
                        &TmpRequest.ReqArgs.VasynOpenRequest.fOk,
                        UNLOCK_TAPE );

  CLOSE_FILE( pAsynciCtrl->DevHandle );
  pAsynciCtrl->DevHandle = INVALID_HANDLE_VALUE;

  DBGOUT;
  return;
  }

/*------------------------------*/


static tsp00_Bool sql65k_asynci_open ( PASYNCI_CTRL_REC            pAsynciCtrl,
                                 DLQ_VASYNOPEN_REQ_ARGS_REC  *pVasynOpenArgs )

  {
  #undef  MF__
  #define MF__ MOD__"sql65k_asynci_open"
  ULONG           ulPageShftOp;
  tsp00_VfType     RequestedFileType;

  DBGIN;

  GET_SHFT_OPERATOR ( &ulPageShftOp, pVasynOpenArgs->ulBlockSize );

  if ( ulPageShftOp == (ULONG)UNDEF )
    {
    MSGALL(( ERR_WRONG_BLOCK_SIZE,
             pVasynOpenArgs->ulBlockSize,
             pAsynciCtrl->szLogDeviceName ));
    DBG1  (( MF__, "Wrong block size %d, specified for '%s'",
             pVasynOpenArgs->ulBlockSize,
             pAsynciCtrl->szLogDeviceName ));

    pVasynOpenArgs->ulErrCode  = ERRCODE_ASYNCIO_WRONG_BLOCK_SIZE;
    pVasynOpenArgs->fOk        = FALSE;

    sql70k_init_asynci_ctrl ( pAsynciCtrl );

    DBGOUT;
    return ( FALSE );
    }

  pVasynOpenArgs->fOk             = TRUE;
  pAsynciCtrl->fForWriting        = pVasynOpenArgs->fForWriting;
  pAsynciCtrl->fAutoRewind        = (pVasynOpenArgs->FileType == vf_t_tape_rew);
  pAsynciCtrl->ulBlockSize        = pVasynOpenArgs->ulBlockSize;
  pAsynciCtrl->ulPageShftOp       = ulPageShftOp;
  pAsynciCtrl->cancelPointer      = pVasynOpenArgs->cancelPointer;
  pAsynciCtrl->ulMaxBlockCnt      = pVasynOpenArgs->ulMaxBlockCnt;
  pAsynciCtrl->fIsDevspace        = pVasynOpenArgs->fIsDevspace;



  // --- substitute logical path name parts
  sql44c_subst_log_parts ( pAsynciCtrl->szPhysDeviceName,
                           pAsynciCtrl->szLogDeviceName );

  pAsynciCtrl->ulDevType = sql44c_get_dev_type_by_filename ( pAsynciCtrl->szPhysDeviceName);

  RequestedFileType = pVasynOpenArgs->FileType;
  if (RequestedFileType == vf_t_tape_norew)
    RequestedFileType.becomes(vf_t_tape_rew);

  if ((pVasynOpenArgs->FileType != vf_t_unknown)
      && (FileType_os2sp[pAsynciCtrl->ulDevType] != RequestedFileType))
    {
    MSGCD(( ERR_WRONG_FTYPE, pVasynOpenArgs->pszLogDeviceName,
        FileTypeName[FileType_os2sp[pAsynciCtrl->ulDevType]],
        FileTypeName[pVasynOpenArgs->FileType]));

    sql70k_init_asynci_ctrl ( pAsynciCtrl );

    DBGOUT;
    return FALSE;
    }

  switch ( pAsynciCtrl->ulDevType )
    {
    case DT_OTHER:
      if ( sql65k_file_open ( pAsynciCtrl, pVasynOpenArgs ) == FALSE )
        {
        sql70k_init_asynci_ctrl ( pAsynciCtrl );

        DBGOUT;
        return ( pVasynOpenArgs->fOk );
        }
      break;

    case DT_NUL:
      if ( sql65k_nul_open ( pAsynciCtrl, pVasynOpenArgs ) == FALSE )
        {
        sql70k_init_asynci_ctrl ( pAsynciCtrl );

        DBGOUT;
        return ( pVasynOpenArgs->fOk );
        }
      break;

    case DT_PIPE:
      if ( sql65k_pipe_open ( pAsynciCtrl, pVasynOpenArgs ) == FALSE )
        {
        sql70k_init_asynci_ctrl ( pAsynciCtrl );
        DBGOUT;
        return ( pVasynOpenArgs->fOk );
        }
      break;

    case DT_TAPE:
      if ( sql65k_tape_open ( pAsynciCtrl, pVasynOpenArgs ) == FALSE )
        {
        sql70k_init_asynci_ctrl ( pAsynciCtrl );

        DBGOUT;
        return ( pVasynOpenArgs->fOk );
        }
      break;

    default:
      pVasynOpenArgs->ulErrCode = ERRCODE_ASYNCIO_WRONG_DEVICE_TYPE;
      pVasynOpenArgs->fOk       = FALSE;

      MSGALL((ERR_WRONG_DEVICE_TYPE, pAsynciCtrl->szLogDeviceName ));
      DBG1 ((MF__, "Wrong device type: '%s'",
             pAsynciCtrl->szLogDeviceName ));

      sql70k_init_asynci_ctrl ( pAsynciCtrl );

      return ( pVasynOpenArgs->fOk );
    }


  DBGOUT;
  return ( pVasynOpenArgs->fOk );
  }

/*------------------------------*/

static VOID sql65k_reply_requests ( PASYNCI_CTRL_REC   pAsynciCtrl )
  {
  #undef  MF__
  #define MF__ MOD__"sql65k_reply_requests"
  PDLQ_REC   pRequest;

  DBGIN;

  pRequest = pAsynciCtrl->pIOWorkQue->pRequest;

  while ( pRequest != (PDLQ_REC) 0 )
    {
    switch ( pRequest->ulReqType )
      {
      case REQ_ASYNIO:
        pRequest->ReqArgs.VasynIORequest.fOk       = FALSE;
        pRequest->ReqArgs.VasynIORequest.ulErrCode =
                                         ERRCODE_ASYNCIO_FILE_DEV_PIPE_NOT_OPEN;

        MSGALL(( ERR_FILE_DEV_PIPE_NOT_OPEN ));
        break;
      default          :
        MSGCD (( ERR_ILLEGAL_REQUEST, "ASYNCi", pRequest->ulReqType ));
        break;
      }

    // -- send reply back to UKT
    REQ_FROM_ASYNCx_TO_UKT( pRequest );

    // --- remove element from queue
    WAIT_UNTIL_ALONE( &pAsynciCtrl->lExclusive );

    pAsynciCtrl->pIOWorkQue->pRequest = 0;
    pAsynciCtrl->pIOWorkQue           = pAsynciCtrl->pIOWorkQue->pNextElem;
    pAsynciCtrl->DevIOStat.ulIOQueLen--;

    CLEARLOCK( &pAsynciCtrl->lExclusive );

    pRequest = pAsynciCtrl->pIOWorkQue->pRequest;
    }

  DBGOUT;
  return;
  }

/*------------------------------*/


static VOID sql65k_asynci_read ( PASYNCI_CTRL_REC         pAsynciCtrl,
                                 DLQ_VASYNIO_REQ_ARGS_REC *pVasynIOArgs )
  {
  #undef  MF__
  #define MF__ MOD__"sql65k_asynci_read"
  ULONG           ulLengthWanted;
  ULONG           ulLength;
  LONG            rc       = NO_ERROR;

  DBGIN;

  pVasynIOArgs->fOk = TRUE;

  if (( pVasynIOArgs->ulBlockNo              != (ULONG)-1 ) &&
      ( (ULONG)(pVasynIOArgs->ulBlockNo + 1) != pAsynciCtrl->ulOldIOPos ))
    {
    ULONG           ulFilePosLow;
    ULONG           ulFilePosHigh;

    BLOCKS_TO_QWORD( ulFilePosLow, ulFilePosHigh,
                     pVasynIOArgs->ulBlockNo + 1, pAsynciCtrl->ulPageShftOp );

    rc = SET_FILE_PTR ( pAsynciCtrl->DevHandle, ulFilePosLow, FILE_BEGIN,
                        &ulFilePosLow, (PLONG)&ulFilePosHigh );

    if (rc != NO_ERROR)
      {
      DBG1  (( MF__, "Cannot seek on: %s, rc = %d",
               pAsynciCtrl->szLogDeviceName, rc ));

      if ( !sql65k_handle_known_errors ( rc, pAsynciCtrl->szLogDeviceName,
                                         &pVasynIOArgs->APIRetCode,
                                         &pVasynIOArgs->ulErrCode,
                                         &pVasynIOArgs->fOk ) )
        {
        pVasynIOArgs->ulErrCode  = ERRCODE_ASYNCIO_CANT_SEEK_FILE_DEV_PIPE;
        pVasynIOArgs->fOk        = FALSE;
        pVasynIOArgs->APIRetCode = rc;
        MSGALL(( ERR_CANT_SEEK_FILE_DEV_PIPE,
                 pAsynciCtrl->szLogDeviceName, rc ));
        }

      DBGOUT;
      return;
      }
    }

  ulLengthWanted = BLOCKS_TO_BYTES( pVasynIOArgs->ulBlockCnt,
                                    pAsynciCtrl->ulPageShftOp );

  rc = sql65k_io_read ( pAsynciCtrl->DevHandle, pAsynciCtrl->ulDevType,
                        pVasynIOArgs->pBlockPtr, pVasynIOArgs->ulBlockNo,
                        ulLengthWanted, &ulLength,
                        pAsynciCtrl->szLogDeviceName );

  if (( rc != NO_ERROR ) || ( ulLength == 0 ))
    {
    if (( rc       == ERROR_HANDLE_EOF        ) ||
        ( rc       == ERROR_FILEMARK_DETECTED ) ||
        ( rc       == ERROR_NO_DATA_DETECTED  ) ||
        ( rc       == ERROR_END_OF_MEDIA      ) ||
        ( rc       == ERROR_BROKEN_PIPE       ) ||
        ( rc       == ERROR_PIPE_NOT_CONNECTED) ||
        (( ulLength == 0 ) && ( rc == NO_ERROR )))
      {
      DBG1  (( MF__, "EOF or disk full: '%s', rc = %d",
               pAsynciCtrl->szLogDeviceName, rc ));

      pVasynIOArgs->ulErrCode  = ERRCODE_ASYNCIO_END_OF_FILE_DEV_PIPE;
      pVasynIOArgs->ulBlockCnt = 0;
      pVasynIOArgs->fOk        = TRUE;

      MSGD (( INFO_END_OF_FILE_OR_MEDIA,
               pAsynciCtrl->szLogDeviceName, rc ));
      }
    else
      {
      DBG1  (( MF__, "Cannot read from '%s', rc = %d",
               pAsynciCtrl->szLogDeviceName, rc ));

      if ( !sql65k_handle_known_errors ( rc, pAsynciCtrl->szLogDeviceName,
                                         &pVasynIOArgs->APIRetCode,
                                         &pVasynIOArgs->ulErrCode,
                                         &pVasynIOArgs->fOk ) )
        {
        pVasynIOArgs->ulErrCode  = ERRCODE_ASYNCIO_CANT_READ_FILE_DEV_PIPE;
        pVasynIOArgs->fOk        = FALSE;
        pVasynIOArgs->APIRetCode = rc;
        MSGALL(( ERR_CANT_READ_FILE_DEV_PIPE,
                 pAsynciCtrl->szLogDeviceName, rc ));
        }

      }

    DBGOUT;
    return;
    }

  pVasynIOArgs->ulBlockCnt = BYTES_TO_BLOCKS ( ulLength,
                                               pAsynciCtrl->ulPageShftOp );
  pAsynciCtrl->ulOldIOPos  = pVasynIOArgs->ulBlockNo  +
                             pVasynIOArgs->ulBlockCnt + 1;
  pVasynIOArgs->fOk        = TRUE;
  os65k_UpdateCountInIOStatisticRec( &pAsynciCtrl->DevIOStat.DevRead, pVasynIOArgs->ulBlockCnt);

  DBGOUT;
  return;
  }

/*------------------------------*/

static VOID sql65k_asynci_write ( PASYNCI_CTRL_REC         pAsynciCtrl,
                                  DLQ_VASYNIO_REQ_ARGS_REC *pVasynIOArgs )
  {
  #undef  MF__
  #define MF__ MOD__"sql65k_asynci_write"
  ULONG           ulFilePosLow;
  ULONG           ulFilePosHigh;
  ULONG           ulLengthWanted;
  ULONG           ulLength;
  LONG            rc = NO_ERROR;

  DBGIN;

  pVasynIOArgs->fOk = TRUE;

  if (( pVasynIOArgs->ulBlockNo              != (ULONG)-1 ) &&
      ( (ULONG)(pVasynIOArgs->ulBlockNo + 1) != pAsynciCtrl->ulOldIOPos ))
    {
    BLOCKS_TO_QWORD( ulFilePosLow, ulFilePosHigh,
                     pVasynIOArgs->ulBlockNo + 1, pAsynciCtrl->ulPageShftOp );

    rc = SET_FILE_PTR ( pAsynciCtrl->DevHandle, ulFilePosLow, FILE_BEGIN,
                        &ulFilePosLow, (PLONG)&ulFilePosHigh );

    if (rc != NO_ERROR)
      {
      DBG1  (( MF__, "Cannot seek on: %s, rc = %d",
               pAsynciCtrl->szLogDeviceName, rc ));

      if ( !sql65k_handle_known_errors ( rc, pAsynciCtrl->szLogDeviceName,
                                         &pVasynIOArgs->APIRetCode,
                                         &pVasynIOArgs->ulErrCode,
                                         &pVasynIOArgs->fOk ) )
        {
        pVasynIOArgs->ulErrCode  = ERRCODE_ASYNCIO_CANT_SEEK_FILE_DEV_PIPE;
        pVasynIOArgs->fOk        = FALSE;
        pVasynIOArgs->APIRetCode = rc;
        MSGALL(( ERR_CANT_SEEK_FILE_DEV_PIPE,
                 pAsynciCtrl->szLogDeviceName, rc ));
        }

      DBGOUT;
      return;
      }
    }

  ulLengthWanted = BLOCKS_TO_BYTES( pVasynIOArgs->ulBlockCnt,
                                    pAsynciCtrl->ulPageShftOp );

  rc = sql65k_io_write ( pAsynciCtrl->DevHandle, pAsynciCtrl->ulDevType,
                         pVasynIOArgs->pBlockPtr, pVasynIOArgs->ulBlockNo,
                         ulLengthWanted, &ulLength );

  if (( rc != NO_ERROR ) || ( ulLength == 0 ))
    {
    if (( rc       == ERROR_HANDLE_DISK_FULL )  ||
        ( rc       == ERROR_END_OF_MEDIA     )  ||
        ( rc       == ERROR_BROKEN_PIPE      )  ||
        ( rc       == ERROR_PIPE_NOT_CONNECTED) ||
        (( ulLength == 0 ) && ( rc == NO_ERROR )))
      {
      DBG1  (( MF__, "EOF or disk full: '%s', rc = %d",
               pAsynciCtrl->szLogDeviceName, rc ));

      pVasynIOArgs->ulErrCode  = ERRCODE_ASYNCIO_END_OF_FILE_DEV_PIPE;
      pVasynIOArgs->ulBlockCnt = 0;
      pVasynIOArgs->fOk        = TRUE;
      pVasynIOArgs->APIRetCode = rc;
      MSGCD (( INFO_END_OF_FILE_DEV_PIPE,
               pAsynciCtrl->szLogDeviceName, rc ));
      }
    else
      {
      DBG1  (( MF__, "Cannot write to '%s', rc = %d",
               pAsynciCtrl->szLogDeviceName, rc ));

      if ( !sql65k_handle_known_errors ( rc, pAsynciCtrl->szLogDeviceName,
                                         &pVasynIOArgs->APIRetCode,
                                         &pVasynIOArgs->ulErrCode,
                                         &pVasynIOArgs->fOk ) )
        {
        pVasynIOArgs->ulErrCode  = ERRCODE_ASYNCIO_CANT_WRITE_FILE_DEV_PIPE;
        pVasynIOArgs->fOk        = FALSE;
        pVasynIOArgs->APIRetCode = rc;
        MSGALL(( ERR_CANT_WRITE_FILE_DEV_PIPE,
                 pAsynciCtrl->szLogDeviceName, rc ));
        }

      }

    DBGOUT;
    return;
    }

  pVasynIOArgs->ulBlockCnt = BYTES_TO_BLOCKS ( ulLength,
                                               pAsynciCtrl->ulPageShftOp );
  pAsynciCtrl->ulOldIOPos  = pVasynIOArgs->ulBlockNo +
                             pVasynIOArgs->ulBlockCnt + 1;
  pVasynIOArgs->fOk        = TRUE;
  os65k_UpdateCountInIOStatisticRec( &pAsynciCtrl->DevIOStat.DevWrite, pVasynIOArgs->ulBlockCnt);

  DBGOUT;
  return;
  }


/*------------------------------*/

static VOID sql65k_asynci_wait ( PASYNCI_CTRL_REC            pAsynciCtrl,
                                 DLQ_VASYNWAIT_REQ_ARGS_REC  *pVasynWaitArgs )
  {
  #undef  MF__
  #define MF__ MOD__"sql65k_asynci_wait"
  PDLQ_REC                         pRequest;
  ULONG                            i;
  DLQ_VASYNIO_REQ_ARGS_REC         *pVasynIORequest;

  for ( i = 0, pRequest = NULL; i < pAsynciCtrl->ulAsynRequests; i++ )
    {
    pRequest        = static_cast<DLQ_ELEMENT*>(pAsynciCtrl->pAsynRequests[i]);
    pVasynIORequest = &pRequest->ReqArgs.VasynIORequest;

    if (( pVasynIORequest->pTaskCtrl == pVasynWaitArgs->pReqTaskCtrl ) &&
        ( pVasynIORequest->fFinished ))
      {
      break;
      }
    pRequest = 0;
    }

  // --- no finished IO operation found?
  if ( pRequest == NULL )
    {
    DBG1  (( MF__, "no I/O requests active" ));
    pVasynWaitArgs->fOk       = FALSE;
    pVasynWaitArgs->ulErrCode = ERRCODE_ASYNCIO_NO_REQ_ACTIVE;
    }
  else
    {
    pVasynWaitArgs->fOk         = pRequest->ReqArgs.VasynIORequest.fOk;
    pVasynWaitArgs->ulErrCode   = pRequest->ReqArgs.VasynIORequest.ulErrCode;
    pVasynWaitArgs->APIRetCode  = pRequest->ReqArgs.VasynIORequest.APIRetCode;
    pVasynWaitArgs->lIO_ID      = pRequest->ReqArgs.VasynIORequest.lIO_ID;
    pVasynWaitArgs->ulBlockCnt  = pRequest->ReqArgs.VasynIORequest.ulBlockCnt;

    pRequest->ReqArgs.VasynIORequest.fFinished = FALSE;
    pRequest->ReqArgs.VasynIORequest.pTaskCtrl = NULL;
    }

  DBGOUT;
  return;
  }

/*------------------------------*/

static tsp00_Bool sql65k_file_close ( PASYNCI_CTRL_REC            pAsynciCtrl,
                                DLQ_VASYNCLOSE_REQ_ARGS_REC *pVasynCloseArgs )
  {
  #undef  MF__
  #define MF__ MOD__"sql65k_file_close"
  LONG                rc  = NO_ERROR;

  rc = CLOSE_FILE( pAsynciCtrl->DevHandle );

  if ( rc != NO_ERROR)
    {
    pVasynCloseArgs->fOk        = FALSE;
    pVasynCloseArgs->ulErrCode  = ERRCODE_ASYNCIO_CLOSE_FILE_DEV_PIPE;
    pVasynCloseArgs->APIRetCode = rc;
    MSGALL(( ERR_CANT_CLOSE_FILE_DEV_PIPE, rc ));
    DBG1  (( MF__, "Cannot close devspace, rc = %d", rc ));
    }


  DBGOUT;
  return ( pVasynCloseArgs->fOk );
  }

/*------------------------------*/

static tsp00_Bool sql65k_pipe_close ( PASYNCI_CTRL_REC            pAsynciCtrl,
                                DLQ_VASYNCLOSE_REQ_ARGS_REC *pVasynCloseArgs )
  {
  #undef  MF__
  #define MF__ MOD__"sql65k_pipe_close"
  LONG                rc  = NO_ERROR;

  FlushFileBuffers( (HANDLE)pAsynciCtrl->DevHandle );

  rc = CLOSE_FILE( pAsynciCtrl->DevHandle );

  if ( rc != NO_ERROR)
    {
    pVasynCloseArgs->fOk        = FALSE;
    pVasynCloseArgs->ulErrCode  = ERRCODE_ASYNCIO_CLOSE_FILE_DEV_PIPE;
    pVasynCloseArgs->APIRetCode = rc;
    MSGALL(( ERR_CANT_CLOSE_FILE_DEV_PIPE, rc ));
    DBG1  (( MF__, "Cannot close devspace, rc = %d", rc ));
    }


  DBGOUT;
  return ( pVasynCloseArgs->fOk );
  }

/*------------------------------*/


static tsp00_Bool sql65k_tape_close ( PASYNCI_CTRL_REC            pAsynciCtrl,
                                DLQ_VASYNCLOSE_REQ_ARGS_REC *pVasynCloseArgs )
  {
  #undef  MF__
  #define MF__ MOD__"sql65k_tape_close"
  DLQ_REC             TmpRequest;
  LONG                rc     = NO_ERROR;

  if ( pAsynciCtrl->fForWriting )
    {
    if ( sql65k_tape_write_mark ( pAsynciCtrl,
                                  &pVasynCloseArgs->ulErrCode,
                                  &pVasynCloseArgs->APIRetCode,
                                  &pVasynCloseArgs->fOk ) == FALSE )
      {
      TmpRequest.ReqArgs.VasynCloseRequest = *pVasynCloseArgs;

      if (pVasynCloseArgs->fRewind || pAsynciCtrl->fAutoRewind)
        {
        sql65k_tape_rewind  ( pAsynciCtrl,
                              &TmpRequest.ReqArgs.VasynCloseRequest.ulErrCode,
                              &TmpRequest.ReqArgs.VasynCloseRequest.APIRetCode,
                              &TmpRequest.ReqArgs.VasynCloseRequest.fOk );
        }
      sql65k_tape_locking ( pAsynciCtrl,
                            &TmpRequest.ReqArgs.VasynCloseRequest.ulErrCode,
                            &TmpRequest.ReqArgs.VasynCloseRequest.APIRetCode,
                            &TmpRequest.ReqArgs.VasynCloseRequest.fOk,
                            UNLOCK_TAPE );

      CLOSE_FILE( pAsynciCtrl->DevHandle );
      pAsynciCtrl->DevHandle = INVALID_HANDLE_VALUE;

      DBGOUT;
      return ( FALSE );
      }
    }

  if ( pVasynCloseArgs->fRewind )
    {
    if ( sql65k_tape_rewind ( pAsynciCtrl,
                                &pVasynCloseArgs->ulErrCode,
                                &pVasynCloseArgs->APIRetCode,
                                &pVasynCloseArgs->fOk ) == FALSE )
      {
      TmpRequest.ReqArgs.VasynCloseRequest = *pVasynCloseArgs;

      sql65k_tape_locking ( pAsynciCtrl,
                              &TmpRequest.ReqArgs.VasynCloseRequest.ulErrCode,
                              &TmpRequest.ReqArgs.VasynCloseRequest.APIRetCode,
                              &TmpRequest.ReqArgs.VasynCloseRequest.fOk,
                              UNLOCK_TAPE );

      CLOSE_FILE( pAsynciCtrl->DevHandle );
      pAsynciCtrl->DevHandle = INVALID_HANDLE_VALUE;

      DBGOUT;
      return ( FALSE );
      }
    }

  if ( sql65k_tape_locking ( pAsynciCtrl,
                             &pVasynCloseArgs->ulErrCode,
                             &pVasynCloseArgs->APIRetCode,
                             &pVasynCloseArgs->fOk,
                             UNLOCK_TAPE ) == FALSE )
    {
    CLOSE_FILE( pAsynciCtrl->DevHandle );
    pAsynciCtrl->DevHandle = INVALID_HANDLE_VALUE;

    DBGOUT;
    return ( FALSE );
    }

  if ( pVasynCloseArgs->fNxtTapeInAutoLoader && pVasynCloseArgs->fOk )
    {
    sql65k_tape_loading  ( pAsynciCtrl ,
                           NULL,
                           &pVasynCloseArgs->ulErrCode,
                           &pVasynCloseArgs->APIRetCode,
                           &pVasynCloseArgs->fNxtTapeAvailable,
                           TAPE_UNLOAD) ;
    /* F.H. what happens, if UNLOAD fails ??? test, test, test !!! */
    }

  rc = CLOSE_FILE( pAsynciCtrl->DevHandle );

  if ( rc != NO_ERROR)
    {
    pVasynCloseArgs->fOk        = FALSE;
    pVasynCloseArgs->ulErrCode  = ERRCODE_ASYNCIO_CLOSE_FILE_DEV_PIPE;
    pVasynCloseArgs->APIRetCode = rc;
    MSGALL(( ERR_CANT_CLOSE_FILE_DEV_PIPE, rc ));
    DBG1  (( MF__, "Cannot close devspace, rc = %d", rc ));
    }

  DBGOUT;
  return ( pVasynCloseArgs->fOk );
  }

/*------------------------------*/

static VOID sql65k_asynci_close ( PASYNCI_CTRL_REC            pAsynciCtrl,
                                  DLQ_VASYNCLOSE_REQ_ARGS_REC *pVasynCloseArgs )
  {
  #undef  MF__
  #define MF__ MOD__"sql65k_asynci_close"
  LONG     rc  = NO_ERROR;

  DBG3 (( MF__, "Closing ASYNCi TID:%d", pAsynciCtrl->ThrdCtrlHeader.Tid ));

  pVasynCloseArgs->fOk                  = TRUE;
  pVasynCloseArgs->fNxtTapeAvailable    = pAsynciCtrl->ulDevType == DT_TAPE &&
                                          pVasynCloseArgs->fNxtTapeInAutoLoader;
  pAsynciCtrl->ThrdCtrlHeader.ThrdState = KT_TERMINATING;

  switch ( pAsynciCtrl->ulDevType )
    {
    case DT_NUL:
    case DT_OTHER:
      sql65k_file_close ( pAsynciCtrl, pVasynCloseArgs );
      break;

    case DT_PIPE:
      sql65k_pipe_close ( pAsynciCtrl, pVasynCloseArgs );
      break;

    case DT_TAPE:
      sql65k_tape_close ( pAsynciCtrl, pVasynCloseArgs );
      break;

    default:
      CLOSE_FILE( pAsynciCtrl->DevHandle );

      pVasynCloseArgs->ulErrCode = ERRCODE_ASYNCIO_WRONG_DEVICE_TYPE;
      pVasynCloseArgs->fOk       = FALSE;

      MSGALL((ERR_WRONG_DEVICE_TYPE, pAsynciCtrl->szLogDeviceName ));
      DBG1  ((MF__, "Wrong device type: '%s'",
             pAsynciCtrl->szLogDeviceName ));

      break;
    }


  // --- reply unhandled requests
  sql65k_reply_requests ( pAsynciCtrl );

  if ( pVasynCloseArgs->fOk == FALSE )
    sql41c_close_event_sem ( pAsynciCtrl->ThrdCtrlHeader.hevSem, "ASYNCi" );
  else
    {
    rc = sql41c_close_event_sem ( pAsynciCtrl->ThrdCtrlHeader.hevSem, "ASYNCi" );

    if ( rc != NO_ERROR)
      {
      pVasynCloseArgs->fOk        = FALSE;
      pVasynCloseArgs->ulErrCode  = ERRCODE_ASYNCIO_CLOSE_EVENT_SEM;
      pVasynCloseArgs->APIRetCode = rc;
      }
    }

  MSGD (( INFO_ASYNCI_STOPPED,  pAsynciCtrl->szLogDeviceName ));

  pAsynciCtrl->ThrdCtrlHeader.ThrdState = KT_INITIALIZED;
  pAsynciCtrl->ThrdCtrlHeader.hThrd     = INVALID_HANDLE_VALUE;
  DBGOUT;
  return;
  }

/*------------------------------*/

static VOID sql65k_asynci_cntl  ( PASYNCI_CTRL_REC            pAsynciCtrl,
                                  DLQ_VASYNCNTL_REQ_ARGS_REC *pVasynCntlArgs )
  {
  #undef  MF__
  #define MF__ MOD__"sql65k_asynci_cntl"
  ULONG         ulErrCode;
  ULONG         apiRetCode;
  tsp00_Bool            ok;
  DLQ_VASYNOPEN_REQ_ARGS_REC openArgs;
  tsp00_Bool            fDoRewind;

  DBGIN;
  DBG1 ((MF__, "request %d in %x",
          pVasynCntlArgs->iCntlRequest, pVasynCntlArgs));
  switch (pVasynCntlArgs->iCntlRequest)
    {
    case ASYNCNTL_FILETYPE:
      if ((pAsynciCtrl->ulDevType == DT_TAPE) && (!pAsynciCtrl->fAutoRewind))
        {
        pVasynCntlArgs->FileType.becomes(vf_t_tape_norew);
        }
      else
        {
        pVasynCntlArgs->FileType.becomes(FileType_os2sp[pAsynciCtrl->ulDevType]);
        }
      DBG1 ((MF__, "setting filetype to %d in %x",
          pVasynCntlArgs->FileType, pVasynCntlArgs));
#if !defined (REWIND_TAPE)
      fDoRewind = FALSE;
      if (pAsynciCtrl->ulDevType == DT_TAPE)
        {
        if (pVasynCntlArgs->fDestructive || pAsynciCtrl->fAutoRewind)
          {
          fDoRewind = TRUE;
          }
        }
      if (fDoRewind)
        {
        DBG1 ((MF__, "rewinding tape"));
        sql65k_tape_rewind (pAsynciCtrl, &ulErrCode, &apiRetCode, &ok);
        memset (&openArgs, '\0', sizeof (openArgs));
        openArgs.fIsDevspace   = FALSE;
        openArgs.ulBlockSize   = pAsynciCtrl->ulBlockSize;
        openArgs.ulMaxBlockCnt = pAsynciCtrl->ulMaxBlockCnt;
        sql65k_block_0_io (pAsynciCtrl, &openArgs);
        }
      else
        {
        DBG1 ((MF__, "%s for device type %d (%s)",
              pVasynCntlArgs->fDestructive ? "destructive" : "safe",
              pAsynciCtrl->ulDevType,
              pAsynciCtrl->fAutoRewind ? "auto rewind" : "norewind"));
        }
#endif
      break;
    default:
      // MSGCD (( ERR_ILLEGAL_REQUEST, "ASYNCi", pRequest->ulReqType ));
      break;
  }

  DBGOUT;
  return;
  }

/*------------------------------*/

static VOID _System  sql65k_asynci ( PASYNCI_CTRL_REC   pAsynciCtrl )
  {
  #undef  MF__
  #define MF__ MOD__"sql65k_asynci"
  LONG                          rc = NO_ERROR;
  tsp00_Bool                          fTerminate;
  PDLQ_REC                      pRequest;
  HANDLE                        hThrd;

  DBGIN;

  __try 
   {
#   if defined(_FASTCAP)
     CAPNameThread ("Asynci");
#   endif
    // --- save the current thread handle which is used by 'EXITTHREAD'.
    hThrd = pAsynciCtrl->ThrdCtrlHeader.hThrd;

    // --- set the thread priority
    rc = SET_THREAD_PRIO ( pAsynciCtrl->ThrdCtrlHeader.lThrdPrio,
                           pAsynciCtrl->ThrdCtrlHeader.Tid,
                           pAsynciCtrl->ThrdCtrlHeader.hThrd );

    if ( rc != NO_ERROR )
      {
      MSGCD (( ERR_SETTING_THRD_PRIO, pAsynciCtrl->ThrdCtrlHeader.Tid));
      DBG1  ((MF__, "Cannot set the thread priority, TID: %u",
                    kgs.Dev0.ThrdCtrlHeader.Tid));
      ABORT();
      }

    DBG3 (( MF__, "ASYNCi TID:%d started", pAsynciCtrl->ThrdCtrlHeader.Tid ));
    MSGD (( INFO_ASYNCI_STARTED, pAsynciCtrl->szLogDeviceName ));

    // --- set thread control info
    pAsynciCtrl->ThrdCtrlHeader.ThrdState   = KT_RUNNING;
    pAsynciCtrl->ThrdCtrlHeader.ulStartTime = kgs.ulCurrTime;

    // --- Runs until closed
    for ( fTerminate = FALSE; fTerminate == FALSE; )
      {
      pRequest = sql65k_async_receive (&pAsynciCtrl->pIOWorkQue,
                                       &pAsynciCtrl->DevIOStat.ulIOQueLen,
                                       &pAsynciCtrl->lExclusive,
                                       &pAsynciCtrl->ThrdCtrlHeader);

      switch ( pRequest->ulReqType )
        {
        case REQ_ASYNOPEN :
          fTerminate = !sql65k_asynci_open (pAsynciCtrl,
                                            &pRequest->ReqArgs.VasynOpenRequest);
          REQ_FROM_ASYNCx_TO_UKT(pRequest); // -- send reply back to UKT
          break;

        case REQ_ASYNIO:
          if ( pAsynciCtrl->fForWriting )
            sql65k_asynci_write( pAsynciCtrl, &pRequest->ReqArgs.VasynIORequest );
          else
            sql65k_asynci_read( pAsynciCtrl, &pRequest->ReqArgs.VasynIORequest );

          pRequest->ReqArgs.VasynIORequest.fFinished = TRUE;
          break;

        case REQ_ASYNWAIT:
          sql65k_asynci_wait ( pAsynciCtrl,
                               &pRequest->ReqArgs.VasynWaitRequest);
          REQ_FROM_ASYNCx_TO_UKT(pRequest); // -- send reply back to UKT
          break;

        case REQ_ASYNCNTL:
          sql65k_asynci_cntl ( pAsynciCtrl, &pRequest->ReqArgs.VasynCntlRequest);
          REQ_FROM_ASYNCx_TO_UKT(pRequest); // -- send reply back to UKT
          break;

        case REQ_ASYNCLOSE :
          sql65k_asynci_close (pAsynciCtrl, &pRequest->ReqArgs.VasynCloseRequest);
          REQ_FROM_ASYNCx_TO_UKT(pRequest); // -- send reply back to UKT
          fTerminate = TRUE;                // --- exit for loop
          break;

        default          :
          MSGCD (( ERR_ILLEGAL_REQUEST, "ASYNCi", pRequest->ulReqType ));
          REQ_FROM_ASYNCx_TO_UKT(pRequest); // -- send reply back to UKT
          break;
        }


      DBG3 (( MF__, "next in loop"));
      }

  }
  __except( sql50k_excepthandler(GetExceptionInformation()) ) {;}

  {
      SAPDB_ULong SystemSize = ((KT_ASYNCi_STACK_SIZE + (1024*1024) - 1)&(~((1024*1024) - 1)));
      RTE_PseudoFreeStack(SystemSize, 0);
  }
  DBGOUT;
  EXITTHREAD ( 0, hThrd );

  return;
  }

/*------------------------------*/

static tsp00_Bool sql65k_get_drive_info ( PASYNCI_CTRL_REC            pAsynciCtrl,
                                    PULONG                      pulErrCode,
                                    PULONG                      pAPIRetCode,
                                    tsp00_Bool                          *pfOk )
  {
  #undef  MF__
  #define MF__ MOD__"sql65k_get_drive_info"

  LONG                       rc         = NO_ERROR;
  INT                        Retry      = TAPE_ERROR_RETRY;
  TAPE_GET_DRIVE_PARAMETERS  DriveInfo;
  DWORD                      dwInfoSize;
  INT                        i;

  DBGIN;

  //
  //  --- get tape device information
  //
  do
    {
    dwInfoSize  = sizeof ( DriveInfo );
    memset ( &DriveInfo, 0, sizeof(DriveInfo));

    rc = GetTapeParameters( (HANDLE)pAsynciCtrl->DevHandle,
                            GET_TAPE_DRIVE_INFORMATION,
                            &dwInfoSize, &DriveInfo );
    }
  while (( rc == ERROR_BUS_RESET          ||
           rc == ERROR_IO_DEVICE          ||
           rc == ERROR_END_OF_MEDIA       ||
           rc == ERROR_BEGINNING_OF_MEDIA ||
           rc == ERROR_MEDIA_CHANGED )    && --Retry);

  if (( rc != NO_ERROR )                 &&
      ( rc != ERROR_MEDIA_CHANGED )      &&
      ( rc != ERROR_END_OF_MEDIA )       &&
      ( rc != ERROR_BEGINNING_OF_MEDIA ) &&
      ( rc != ERROR_INVALID_FUNCTION ))
    {
    DBG1  (( MF__, "Could not get drive info ('%s'), rc = %d",
             pAsynciCtrl->szLogDeviceName, rc ));

    if ( !sql65k_handle_known_errors ( rc, pAsynciCtrl->szLogDeviceName,
                                       pAPIRetCode,
                                       pulErrCode ,
                                       pfOk  ) )
      {
      *pulErrCode   = ERRCODE_ASYNCIO_GET_TAPE_DRIVE_INFO;
      *pfOk         = FALSE;
      *pAPIRetCode  = rc;
      MSGALL(( ERR_CANT_GET_TAPE_DRIVE_INFO, pAsynciCtrl->szLogDeviceName, rc ));
      }

    DBGOUT;
    return ( FALSE );
    }
  else if ( rc != ERROR_INVALID_FUNCTION )
    {
    pAsynciCtrl->ulMaxTapeBlkSize = DriveInfo.MaximumBlockSize;
    pAsynciCtrl->fECC             = DriveInfo.ECC;
    pAsynciCtrl->fCompression     = DriveInfo.Compression;
    pAsynciCtrl->fDataPadding     = DriveInfo.DataPadding;
    pAsynciCtrl->ulTapeFeatures   = 0;

    if ( pAsynciCtrl->ulMaxTapeBlkSize < pAsynciCtrl->ulTapeBlkSize )
      {
      MSGD(( INFO_TAPE_BLOCKSIZE_CORRECTED, 
             pAsynciCtrl->szLogDeviceName,
             pAsynciCtrl->ulTapeBlkSize,
             pAsynciCtrl->ulMaxTapeBlkSize ));
      
      pAsynciCtrl->ulTapeBlkSize = pAsynciCtrl->ulMaxTapeBlkSize;

      for ( i = -1; pAsynciCtrl->ulTapeBlkSize; i++ )
        pAsynciCtrl->ulTapeBlkSize = pAsynciCtrl->ulTapeBlkSize >> 1;

      pAsynciCtrl->ulTapeBlkSize = 1 << i;

      }

    if (( DriveInfo.FeaturesHigh & TAPE_DRIVE_SET_COMPRESSION ) &&
        ( DriveInfo.FeaturesLow  & TAPE_DRIVE_COMPRESSION ))
      pAsynciCtrl->ulTapeFeatures |= TAPE_SET_COMPRESSION;
    if (( DriveInfo.FeaturesHigh & TAPE_DRIVE_SET_ECC ) &&
        ( DriveInfo.FeaturesLow  & TAPE_DRIVE_ECC ))
      pAsynciCtrl->ulTapeFeatures |= TAPE_SET_ECC;
    if ( DriveInfo.FeaturesHigh & TAPE_DRIVE_LOCK_UNLOCK )
      pAsynciCtrl->ulTapeFeatures |= TAPE_LOCK_UNLOCK;
    if ( DriveInfo.FeaturesHigh & TAPE_DRIVE_SET_BLOCK_SIZE )
      pAsynciCtrl->ulTapeFeatures |= TAPE_SET_BLOCK_SIZE;
    if ( DriveInfo.FeaturesHigh & TAPE_DRIVE_REPORT_SMKS )
      pAsynciCtrl->ulTapeFeatures |= TAPE_REPORT_SMKS;
    if ( DriveInfo.FeaturesHigh & TAPE_DRIVE_WRITE_LONG_FMKS )
      pAsynciCtrl->ulTapeFeatures |= TAPE_WRITE_LONG_FMKS;
    if ( DriveInfo.FeaturesHigh & TAPE_DRIVE_WRITE_SHORT_FMKS )
      pAsynciCtrl->ulTapeFeatures |= TAPE_WRITE_SHORT_FMKS;
    if ( DriveInfo.FeaturesHigh & TAPE_DRIVE_WRITE_FILEMARKS )
      pAsynciCtrl->ulTapeFeatures |= TAPE_WRITE_FILEMARKS;
    if ( DriveInfo.FeaturesHigh & TAPE_DRIVE_TENSION )
      pAsynciCtrl->ulTapeFeatures |= TAPE_TENSION_TAPE;
    if ( DriveInfo.FeaturesHigh & TAPE_DRIVE_WRITE_MARK_IMMED )
      pAsynciCtrl->ulTapeFeatures |= TAPE_WRITE_MARK_IMMED;
    if ( DriveInfo.FeaturesHigh & TAPE_DRIVE_LOCK_UNLK_IMMED )
      pAsynciCtrl->ulTapeFeatures |= TAPE_LOCK_UNLOCK_IMMED;
    if ( DriveInfo.FeaturesHigh & TAPE_DRIVE_LOAD_UNLOAD )
      pAsynciCtrl->ulTapeFeatures |= TAPE_LOAD_UNLOAD;
    if ( DriveInfo.FeaturesHigh & TAPE_DRIVE_REWIND_IMMEDIATE )
      pAsynciCtrl->ulTapeFeatures |= TAPE_REWIND_IMMED;
    if ( DriveInfo.FeaturesHigh & TAPE_DRIVE_TENSION_IMMED )
      pAsynciCtrl->ulTapeFeatures |= TAPE_TENSION_TAPE_IMMED;
    }


  DBGOUT;
  return ( TRUE );
  }

/*------------------------------*/

static tsp00_Bool sql65k_get_media_info ( PASYNCI_CTRL_REC            pAsynciCtrl,
                                    struct task_ctrl_record     *pReqTaskCtrl,
                                    PULONG                      pulErrCode,
                                    PULONG                      pAPIRetCode,
                                    tsp00_Bool                          *pfOk )
  {
  #undef  MF__
  #define MF__ MOD__"sql65k_get_media_info"

  LONG                       rc         = NO_ERROR;
  INT                        Retry      = TAPE_ERROR_RETRY;
  TAPE_GET_MEDIA_PARAMETERS  MediaInfo;
  DWORD                      dwInfoSize;

  DBGIN;

  //
  //  --- get media information
  //
  do
    {
    dwInfoSize  = sizeof ( MediaInfo );
    memset ( &MediaInfo, 0, sizeof(MediaInfo));

    rc = GetTapeParameters( (HANDLE)pAsynciCtrl->DevHandle,
                            GET_TAPE_MEDIA_INFORMATION,
                            &dwInfoSize, &MediaInfo );

      if ( rc == ERROR_NO_MEDIA_IN_DRIVE )
        {
        REG_ENTRY_REC              RegistryEntries[1];
        DWORD                      WaitSec;
        PATHNAME                   szSubKey;
        
        RegistryEntries[0].pszValueName = REG_VN_TAPE_LOAD_RETRY_TIME;
        RegistryEntries[0].pValue       = &WaitSec;
        RegistryEntries[0].ulValueSize  = sizeof(DWORD);
        RegistryEntries[0].ulValueType  = REG_DWORD;

        strcpy ( szSubKey, kgs.szServiceName);
        strcat ( szSubKey, "\\"REG_SK_SERVICE_PARAM );

        rc = sql50_reg_get_service_values ( NULL, szSubKey, 1, RegistryEntries );

        if ( rc != NO_ERROR )
          WaitSec = DEFAULT_TAPE_LOAD_RETRY_TIME * 10;

        do 
          {
          SLEEP(1000) ;
          
          dwInfoSize  = sizeof ( MediaInfo );
          memset ( &MediaInfo, 0, sizeof(MediaInfo));

          rc = GetTapeParameters( (HANDLE)pAsynciCtrl->DevHandle,
                                  GET_TAPE_MEDIA_INFORMATION,
                                  &dwInfoSize, &MediaInfo );

          if ( *(pAsynciCtrl->cancelPointer) )
            rc = ERROR_CANCELLED;
          }
        while ( --WaitSec && !sql65k_tape_in_drive(pAsynciCtrl) &&  rc != ERROR_CANCELLED );
        }
    }
  while (( rc == ERROR_BUS_RESET            ||
           rc == ERROR_IO_DEVICE            ||
           rc == ERROR_END_OF_MEDIA         ||
           rc == ERROR_BEGINNING_OF_MEDIA   ||
           rc == ERROR_MEDIA_CHANGED ) && --Retry);

  if (( rc != NO_ERROR )                 &&
      ( rc != ERROR_MEDIA_CHANGED )      &&
      ( rc != ERROR_END_OF_MEDIA )       &&
      ( rc != ERROR_BEGINNING_OF_MEDIA ) &&
      ( rc != ERROR_INVALID_FUNCTION ))
    {
    DBG1  (( MF__, "Could not get media information ('%s'), rc = %d",
             pAsynciCtrl->szLogDeviceName, rc ));

    if ( !sql65k_handle_known_errors ( rc, pAsynciCtrl->szLogDeviceName,
                                       pAPIRetCode,
                                       pulErrCode ,
                                       pfOk  ) )
      {
      *pulErrCode  = ERRCODE_ASYNCIO_GET_TAPE_MEDIA_INFO;
      *pfOk        = FALSE;
      *pAPIRetCode = rc;
      MSGALL(( ERR_CANT_GET_TAPE_MEDIA_INFO, pAsynciCtrl->szLogDeviceName, rc ));
      }

    DBGOUT;
    return ( FALSE );
    }
  else if ( rc != ERROR_INVALID_FUNCTION )
    {
    if ( MediaInfo.PartitionCount > 1)
      {
      MSGD (( INFO_MORE_THAN_ONE_PARTITION,
              pAsynciCtrl->szLogDeviceName,
              MediaInfo.PartitionCount ));
      }
    }

  DBGOUT;
  return ( TRUE );
  }

/*------------------------------*/

static tsp00_Bool sql65k_set_drive_info( PASYNCI_CTRL_REC          pAsynciCtrl,
                                   PULONG                    pulErrCode,
                                   PULONG                    pAPIRetCode,
                                   tsp00_Bool                        *pfOk )
  {
  #undef  MF__
  #define MF__ MOD__"set_drive_info"

  LONG                       rc         = NO_ERROR;
  INT                        Retry      = TAPE_ERROR_RETRY;
  TAPE_SET_DRIVE_PARAMETERS  DriveInfo;
  PSZ                        pszTapeCompressEnabled;
  REG_ENTRY_REC              RegistryEntries[1];
  DWORD                      tapeCompressEnabled = 0;
  PATHNAME                   szSubKey;

  DBGIN;

  //
  //  --- set tape device information
  //
  memset ( &DriveInfo, 0, sizeof(DriveInfo));

  DriveInfo.ECC                = pAsynciCtrl->fECC;
  DriveInfo.Compression        = pAsynciCtrl->fCompression;
  DriveInfo.DataPadding        = pAsynciCtrl->fDataPadding;
  DriveInfo.ReportSetmarks     = FALSE;
  DriveInfo.EOTWarningZoneSize = 0;

  if ( pAsynciCtrl->ulTapeFeatures & TAPE_SET_ECC )
    DriveInfo.ECC  = TRUE;

  if ( pAsynciCtrl->ulTapeFeatures & TAPE_SET_COMPRESSION )
    {
    rc = GETENV ( TAPE_COMPRESS_ENV_VAR, &pszTapeCompressEnabled );

    if (( rc == NO_ERROR ) && ( *pszTapeCompressEnabled ))
      {
      if ( ! strcmp ( "OFF", strupr ( pszTapeCompressEnabled )))
        DriveInfo.Compression  = FALSE;
      else
        DriveInfo.Compression  = TRUE;
      }
    else
      {
      RegistryEntries[0].pszValueName = REG_VN_TAPE_COMPRESS;
      RegistryEntries[0].pValue       = &tapeCompressEnabled;
      RegistryEntries[0].ulValueSize  = sizeof(DWORD);
      RegistryEntries[0].ulValueType  = REG_DWORD;

      strcpy ( szSubKey, kgs.szServiceName);
      strcat ( szSubKey, "\\"REG_SK_SERVICE_PARAM );

      rc = sql50_reg_get_service_values ( NULL, szSubKey, 1, RegistryEntries );

      if (( rc == NO_ERROR ) && ( ! tapeCompressEnabled ))
        DriveInfo.Compression  = FALSE;
      else
        DriveInfo.Compression  = TRUE;  // --- compression enabled by default!!
      }
    }

  MSGD(( INFO_TAPE_PARAMS,
         DriveInfo.Compression ? "ON":"OFF",
         DriveInfo.ECC         ? "ON":"OFF" ));

  do
    {
    rc = SetTapeParameters( (HANDLE)pAsynciCtrl->DevHandle,
                            SET_TAPE_DRIVE_INFORMATION,
                            &DriveInfo );
    }
  while (( rc == ERROR_BUS_RESET            ||
           rc == ERROR_IO_DEVICE            ||
           rc == ERROR_END_OF_MEDIA         ||
           rc == ERROR_BEGINNING_OF_MEDIA   ||
           rc == ERROR_MEDIA_CHANGED ) && --Retry);

  if (( rc != NO_ERROR )                 &&
      ( rc != ERROR_MEDIA_CHANGED )      &&
      ( rc != ERROR_END_OF_MEDIA )       &&
      ( rc != ERROR_BEGINNING_OF_MEDIA ) &&
      ( rc != ERROR_INVALID_FUNCTION ))
    {
    DBG1  (( MF__, "Could not set drive info ('%s'), rc = %d",
             pAsynciCtrl->szLogDeviceName, rc ));

    if ( !sql65k_handle_known_errors ( rc, pAsynciCtrl->szLogDeviceName,
                                       pAPIRetCode,
                                       pulErrCode ,
                                       pfOk  ) )
      {
      *pulErrCode  = ERRCODE_ASYNCIO_SET_TAPE_DRIVE_INFO;
      *pfOk        = FALSE;
      *pAPIRetCode = rc;
      MSGALL(( ERR_CANT_SET_TAPE_DRIVE_INFO, pAsynciCtrl->szLogDeviceName, rc ));
      }

    DBGOUT;
    return ( FALSE );
    }


  DBGOUT;
  return ( TRUE );
  }

/*------------------------------*/

static tsp00_Bool sql65k_set_media_info( PASYNCI_CTRL_REC          pAsynciCtrl,
                                   PULONG                    pulErrCode,
                                   PULONG                    pAPIRetCode,
                                   tsp00_Bool                        *pfOk )
  {
  #undef  MF__
  #define MF__ MOD__"set_media_info"

  LONG                       rc         = NO_ERROR;
  INT                        Retry      = TAPE_ERROR_RETRY;
  TAPE_SET_MEDIA_PARAMETERS  MediaInfo;

  DBGIN;

  //
  //  --- set media information
  //
  if ( pAsynciCtrl->ulTapeFeatures & TAPE_SET_BLOCK_SIZE )
    {
    memset ( &MediaInfo, 0, sizeof(MediaInfo));

    MediaInfo.BlockSize = pAsynciCtrl->ulTapeBlkSize;

    do
      {
      rc = SetTapeParameters( (HANDLE)pAsynciCtrl->DevHandle,
                              SET_TAPE_MEDIA_INFORMATION,
                              &MediaInfo );
      }
    while (( rc == ERROR_BUS_RESET            ||
             rc == ERROR_IO_DEVICE            ||
             rc == ERROR_END_OF_MEDIA         ||
             rc == ERROR_BEGINNING_OF_MEDIA   ||
             rc == ERROR_MEDIA_CHANGED ) && --Retry);

    if (( rc != NO_ERROR )                 &&
        ( rc != ERROR_MEDIA_CHANGED )      &&
        ( rc != ERROR_END_OF_MEDIA )       &&
        ( rc != ERROR_BEGINNING_OF_MEDIA ) &&
        ( rc != ERROR_INVALID_FUNCTION ))
      {
      DBG1  (( MF__, "Could not set media information ('%s'), rc = %d",
               pAsynciCtrl->szLogDeviceName, rc ));

      if ( !sql65k_handle_known_errors ( rc, pAsynciCtrl->szLogDeviceName,
                                         pAPIRetCode,
                                         pulErrCode ,
                                         pfOk  ) )
        {
        *pulErrCode  = ERRCODE_ASYNCIO_SET_TAPE_MEDIA_INFO;
        *pfOk        = FALSE;
        *pAPIRetCode = rc;
        MSGALL(( ERR_CANT_SET_TAPE_MEDIA_INFO, pAsynciCtrl->szLogDeviceName, rc ));
        }

      DBGOUT;
      return ( FALSE );
      }
    }

  MSGD(( INFO_TAPE_MEDIA_INFO, 
         pAsynciCtrl->szLogDeviceName,
         MediaInfo.BlockSize ));
    
  DBGOUT;
  return ( TRUE );
  }

/*------------------------------*/

static tsp00_Bool sql65k_tape_rewind( PASYNCI_CTRL_REC            pAsynciCtrl,
                                PULONG                      pulErrCode,
                                PULONG                      pAPIRetCode,
                                tsp00_Bool                          *pfOk )
  {
  #undef  MF__
  #define MF__ MOD__"sql65k_tape_rewind"

  LONG                       rc         = NO_ERROR;
  INT                        Retry      = TAPE_ERROR_RETRY;

  DBGIN;

  do
    {
    rc = SetTapePosition( (HANDLE)pAsynciCtrl->DevHandle, TAPE_REWIND,
                           0, 0, 0, FALSE);
    }
  while (( rc == ERROR_BUS_RESET          ||
           rc == ERROR_IO_DEVICE          ||
           rc == ERROR_MEDIA_CHANGED ) && --Retry);


  if (( rc != NO_ERROR ) && ( rc != ERROR_MEDIA_CHANGED ))
    {
    DBG1  (( MF__, "Could not rewind tape '%s', rc = %d",
             pAsynciCtrl->szLogDeviceName, rc ));

    if ( !sql65k_handle_known_errors ( rc, pAsynciCtrl->szLogDeviceName,
                                       pAPIRetCode,
                                       pulErrCode ,
                                       pfOk  ) )
      {
      *pulErrCode  = ERRCODE_ASYNCIO_REWIND_TAPE;
      *pfOk        = FALSE;
      *pAPIRetCode = rc;
      MSGALL(( ERR_CANT_REWIND_TAPE, pAsynciCtrl->szLogDeviceName, rc ));
      }

    DBGOUT;
    return ( FALSE );
    }

  DBGOUT;
  return ( TRUE );
  }

/*------------------------------*/

static tsp00_Bool sql65k_tape_locking ( PASYNCI_CTRL_REC            pAsynciCtrl,
                                  PULONG                      pulErrCode,
                                  PULONG                      pAPIRetCode,
                                  tsp00_Bool                          *pfOk,
                                  ULONG                       ulOperation )
  {
  #undef  MF__
  #define MF__ MOD__"sql65k_tape_locking"

  LONG                       rc             = NO_ERROR;
  INT                        Retry          = TAPE_ERROR_RETRY;
  DWORD                      dwOperation;

  DBGIN;

  if ( pAsynciCtrl->ulTapeFeatures & TAPE_LOCK_UNLOCK )
    {
    if ( ulOperation == LOCK_TAPE )
      dwOperation = TAPE_LOCK;
    else
      dwOperation = TAPE_UNLOCK;

    do
      {
      rc = PrepareTape ( (HANDLE)pAsynciCtrl->DevHandle, dwOperation, FALSE);
      }
    while (( rc == ERROR_BUS_RESET            ||
             rc == ERROR_IO_DEVICE            ||
             rc == ERROR_END_OF_MEDIA         ||
             rc == ERROR_BEGINNING_OF_MEDIA   ||
             rc == ERROR_MEDIA_CHANGED ) && --Retry);

    if (( rc != NO_ERROR )                 &&
        ( rc != ERROR_MEDIA_CHANGED )      &&
        ( rc != ERROR_END_OF_MEDIA )       &&
        ( rc != ERROR_BEGINNING_OF_MEDIA ) &&
        ( rc != ERROR_INVALID_FUNCTION ))
      {
      DBG1  (( MF__, "Could not lock/unlock tape '%s', rc = %d",
               pAsynciCtrl->szLogDeviceName, rc ));

      if ( !sql65k_handle_known_errors ( rc, pAsynciCtrl->szLogDeviceName,
                                         pAPIRetCode,
                                         pulErrCode ,
                                         pfOk  ) )
        {
        *pulErrCode  = ERRCODE_ASYNCIO_LOCK_UNLOCK_TAPE;
        *pfOk        = FALSE;
        *pAPIRetCode = rc;
        MSGALL(( ERR_CANT_LOCK_UNLOCK_TAPE, pAsynciCtrl->szLogDeviceName, rc ));
        }

      DBGOUT;
      return ( FALSE );
      }
    }

  DBGOUT;
  return ( TRUE );
  }

/*------------------------------*/

static tsp00_Bool sql65k_init_dlt_tape ( PASYNCI_CTRL_REC            pAsynciCtrl )
{
  #undef  MF__
  #define MF__ MOD__"sql65k_init_dlt_tape"

  LONG                       rc             = NO_ERROR;
  INT                        Retry          = TAPE_ERROR_RETRY;

  DBGIN;

  //
  // --- try to load tape.
  //     it's needed by some tape drives (DLT) as first operation
  //
  do
    {
    rc = PrepareTape ( (HANDLE)pAsynciCtrl->DevHandle, TAPE_UNLOCK , FALSE);
    }
  while (( rc == ERROR_MEDIA_CHANGED        ||
           rc == ERROR_END_OF_MEDIA         ||
           rc == ERROR_BEGINNING_OF_MEDIA   ||
           rc == ERROR_IO_DEVICE            ||
           rc == ERROR_BUS_RESET            ) && --Retry);
  //
  // --- no error handling, ignore all errors
  //

  DBGOUT;
  return ( TRUE );
  }

/*------------------------------*/

static tsp00_Bool sql65k_tape_in_drive (PASYNCI_CTRL_REC            pAsynciCtrl)
  {
  #undef  MF__
  #define MF__ MOD__"sql65k_tape_in_drive"
  LONG                       rc             = NO_ERROR;
  tsp00_Bool                       result;

  DBGIN;
  rc = GetTapeStatus ( (HANDLE)pAsynciCtrl->DevHandle );
#if defined (DEBUG_RTE)
  DBG1 ((MF__, "GetTapeStatus: %d", rc));
#endif
  switch ( rc )
    {
    case ERROR_NOT_READY:
    case ERROR_MEDIA_CHANGED:
    case ERROR_NO_MEDIA_IN_DRIVE:
      result = FALSE;
      break;
    default:
      result = TRUE;
      break;
    }
  DBGOUT;
  return result;
  }

/*------------------------------*/

static tsp00_Bool sql65k_tape_loading ( PASYNCI_CTRL_REC            pAsynciCtrl ,
                                  struct task_ctrl_record     *pReqTaskCtrl,
                                  PULONG                      pulErrCode,
                                  PULONG                      pAPIRetCode,
                                  tsp00_Bool                          *pfOk,
                                  ULONG                       ulOperation )
  {
  #undef  MF__
  #define MF__ MOD__"sql65k_tape_loading"

  LONG                       rc             = NO_ERROR;
  INT                        Retry          = TAPE_ERROR_RETRY;
  REG_ENTRY_REC              RegistryEntries[1];
  PATHNAME                   szSubKey;
  DWORD                      WaitSec;

  DBGIN;

  if ( pAsynciCtrl->ulTapeFeatures & TAPE_LOAD_UNLOAD )
    {
    if (( ulOperation == TAPE_LOAD) && sql65k_tape_in_drive (pAsynciCtrl))
      {
      DBG1 ((MF__, "tape already loaded"));
      return TRUE;
      }
    do
      {
      rc = PrepareTape ( (HANDLE)pAsynciCtrl->DevHandle, ulOperation , FALSE) ;

      if ( ulOperation == TAPE_LOAD && (( rc == ERROR_NO_MEDIA_IN_DRIVE ) || 
                                        ( rc == ERROR_NOT_READY )         ||
                                        ( rc == ERROR_MEDIA_CHANGED )))
        {
        RegistryEntries[0].pszValueName = REG_VN_TAPE_LOAD_RETRY_TIME;
        RegistryEntries[0].pValue       = &WaitSec;
        RegistryEntries[0].ulValueSize  = sizeof(DWORD);
        RegistryEntries[0].ulValueType  = REG_DWORD;

        strcpy ( szSubKey, kgs.szServiceName);
        strcat ( szSubKey, "\\"REG_SK_SERVICE_PARAM );

        rc = sql50_reg_get_service_values ( NULL, szSubKey, 1, RegistryEntries );

        if ( rc != NO_ERROR )
          WaitSec = DEFAULT_TAPE_LOAD_RETRY_TIME;

        do
          {
          SLEEP(1000) ;
          rc = PrepareTape ( (HANDLE)pAsynciCtrl->DevHandle, ulOperation, FALSE);

          if ( *(pAsynciCtrl->cancelPointer) )
            rc = ERROR_CANCELLED;
          }
        while ( --WaitSec && rc != NO_ERROR && rc != ERROR_CANCELLED );
        }
      }
    while (( rc == ERROR_MEDIA_CHANGED        ||
             rc == ERROR_END_OF_MEDIA         ||
             rc == ERROR_BEGINNING_OF_MEDIA   ||
             rc == ERROR_IO_DEVICE            ||
             rc == ERROR_BUS_RESET            ) && --Retry);

     if (( rc != NO_ERROR )                 &&
         ( rc != ERROR_MEDIA_CHANGED )      &&
         ( rc != ERROR_END_OF_MEDIA )       &&
         ( rc != ERROR_BEGINNING_OF_MEDIA ) &&
         ( rc != ERROR_INVALID_FUNCTION ))
       {
       DBG1  (( MF__, "Could not %s tape '%s', rc = %d",
                ulOperation == TAPE_LOAD ? "load" : "unload",
                pAsynciCtrl->szLogDeviceName, rc ));

       if ( !sql65k_handle_known_errors ( rc, pAsynciCtrl->szLogDeviceName,
                                          pAPIRetCode,
                                          pulErrCode ,
                                          pfOk  ) )
         {
         *pulErrCode  = ERRCODE_ASYNCIO_LOAD_UNLOAD_TAPE ;
         *pfOk        = FALSE;
         *pAPIRetCode = rc;
         MSGALL(( ERR_CANT_LOAD_UNLOAD_TAPE, pAsynciCtrl->szLogDeviceName,rc ));
         }

       DBGOUT;
       return ( FALSE );
       }
     }
   else if ( ulOperation == TAPE_UNLOAD )
     {                                // autoloader in use, don't ignore error
     *pulErrCode  = ERRCODE_ASYNCIO_LOAD_UNLOAD_TAPE ;
     *pfOk        = FALSE;
     MSGALL(( ERR_CANT_LOAD_UNLOAD_TAPE, pAsynciCtrl->szLogDeviceName,rc ));
     DBGOUT;
     return ( FALSE );
     }

  DBGOUT;
  return ( TRUE );
  }

/*------------------------------*/

static tsp00_Bool sql65k_tape_tension ( PASYNCI_CTRL_REC            pAsynciCtrl,
                                  PULONG                      pulErrCode,
                                  PULONG                      pAPIRetCode,
                                  tsp00_Bool                          *pfOk )
  {
  #undef  MF__
  #define MF__ MOD__"sql65k_tape_tension"

  LONG                       rc             = NO_ERROR;
  INT                        Retry          = TAPE_ERROR_RETRY;

  DBGIN;

  #if defined ( TENSION_TAPE )

   if (( pAsynciCtrl->ulTapeFeatures & TAPE_TENSION_TAPE ) &&
       ( pAsynciCtrl->fMediaChanged ))
     {
     do
       {
       rc = PrepareTape ( (HANDLE)pAsynciCtrl->DevHandle, TAPE_TENSION, FALSE);
       }
     while (( rc == ERROR_BUS_RESET            ||
              rc == ERROR_IO_DEVICE            ||
              rc == ERROR_END_OF_MEDIA         ||
              rc == ERROR_BEGINNING_OF_MEDIA   ||
              rc == ERROR_MEDIA_CHANGED ) && --Retry);

     if (( rc != NO_ERROR )                 &&
         ( rc != ERROR_MEDIA_CHANGED )      &&
         ( rc != ERROR_END_OF_MEDIA )       &&
         ( rc != ERROR_BEGINNING_OF_MEDIA ) &&
         ( rc != ERROR_INVALID_FUNCTION ))
       {
       DBG1  (( MF__, "Could not tension tape '%s', rc = %d",
                pAsynciCtrl->szLogDeviceName, rc ));

       if ( !sql65k_handle_known_errors ( rc, pAsynciCtrl->szLogDeviceName,
                                          pAPIRetCode,
                                          pulErrCode ,
                                          pfOk  ) )
         {
         *pulErrCode  = ERRCODE_ASYNCIO_TENSION_TAPE;
         *pfOk        = FALSE;
         *pAPIRetCode = rc;
         MSGALL(( ERR_CANT_TENSION_TAPE, pAsynciCtrl->szLogDeviceName, rc ));
         }

       DBGOUT;
       return ( FALSE );
       }
     }

  #endif

  DBGOUT;
  return ( TRUE );
  }

/*------------------------------*/

  static tsp00_Bool sql65k_tape_autorewind ( PASYNCI_CTRL_REC            pAsynciCtrl,
                                       PULONG                      pulErrCode,
                                       PULONG                      pAPIRetCode,
                                       tsp00_Bool                          *pfOk )
  {
  #undef  MF__
  #define MF__ MOD__"sql65k_tape_autorewind"
  if (pAsynciCtrl->fAutoRewind)
    {
    if (!sql65k_tape_rewind (pAsynciCtrl, pulErrCode, pAPIRetCode, pfOk))
      {
      return FALSE;
      }
    if (!sql65k_tape_tension (pAsynciCtrl, pulErrCode, pAPIRetCode, pfOk))
      {
      return FALSE;
      }
    }
  return TRUE;
  }

/*------------------------------*/

static tsp00_Bool sql65k_tape_write_mark( PASYNCI_CTRL_REC            pAsynciCtrl,
                                    PULONG                      pulErrCode,
                                    PULONG                      pAPIRetCode,
                                    tsp00_Bool                        *pfOk )
  {
  #undef  MF__
  #define MF__ MOD__"sql65k_tape_write_mark"

  LONG                       rc             = NO_ERROR;
  INT                        Retry          = TAPE_ERROR_RETRY;
  LONG                       lCount         = 1;
  DWORD                      dwOperation    = 0;
  tsp00_Bool                       fRetImmediate  = TRUE;

  DBGIN;

  if ( pAsynciCtrl->ulTapeFeatures & TAPE_WRITE_MARK_IMMED )
    fRetImmediate  = FALSE;

  if ( pAsynciCtrl->ulTapeFeatures & TAPE_WRITE_LONG_FMKS )
    dwOperation = TAPE_LONG_FILEMARKS;
  else if ( pAsynciCtrl->ulTapeFeatures & TAPE_WRITE_SHORT_FMKS )
    dwOperation = TAPE_SHORT_FILEMARKS;
  else if ( pAsynciCtrl->ulTapeFeatures & TAPE_WRITE_FILEMARKS )
    dwOperation = TAPE_FILEMARKS;
  else
    {
    DBGOUT;
    return ( TRUE );
    }

  do
    {
    rc = WriteTapemark ( (HANDLE)pAsynciCtrl->DevHandle, dwOperation,
                         lCount, fRetImmediate );
    }
  while (( rc == ERROR_BUS_RESET ||
           rc == ERROR_IO_DEVICE ) && --Retry);

  if (( rc != NO_ERROR ) && ( rc != ERROR_INVALID_FUNCTION ))
    {
    DBG1  (( MF__, "Could not write tape mark on '%s', rc = %d",
             pAsynciCtrl->szLogDeviceName, rc ));

    if (( rc       == ERROR_HANDLE_DISK_FULL ) ||
        ( rc       == ERROR_END_OF_MEDIA     ) ||
        ( rc       == ERROR_PIPE_NOT_CONNECTED) ||
        ( rc       == ERROR_BROKEN_PIPE      ))
      {
        DBG1  (( MF__, "Media '%s' full, rc = %d (Ignored)",
                pAsynciCtrl->szLogDeviceName, rc ));
        DBGOUT;
        return ( TRUE );
      }
    else if ( !sql65k_handle_known_errors ( rc, pAsynciCtrl->szLogDeviceName,
                                       pAPIRetCode,
                                       pulErrCode ,
                                       pfOk  ) )
      {
      *pulErrCode  = ERRCODE_ASYNCIO_WRITE_TAPE_MARK;
      *pfOk        = FALSE;
      *pAPIRetCode = rc;
      MSGALL(( ERR_CANT_WRITE_TAPE_MARK, pAsynciCtrl->szLogDeviceName, rc ));
      }

    DBGOUT;
    return ( FALSE );
    }

  DBGOUT;
  return ( TRUE );
  }

/*------------------------------*/

static HANDLE sql65k_create_file( LPCTSTR                 lpFileName,
                                  DWORD                   dwDesiredAccess,
                                  DWORD                   dwShareMode,
                                  LPSECURITY_ATTRIBUTES   lpSecurityAttributes,
                                  DWORD                   dwCreationDisposition,
                                  DWORD                   dwFlagsAndAttributes,
                                  HANDLE                  hTemplateFile )
  {
  #undef  MF__
  #define MF__ MOD__"sql65k_create_file"

  HANDLE Handle;
  INT    Retry  = NUM_OF_SHARING_VIOLATION_RETRIES;
  LONG   rc     = NO_ERROR;

  do
    {
    Handle = CreateFile( lpFileName, dwDesiredAccess, dwShareMode,
                         lpSecurityAttributes, dwCreationDisposition,
                         dwFlagsAndAttributes, hTemplateFile );

    if ( Handle == INVALID_HANDLE_VALUE )
      {
      rc = GetLastError();

      if ( rc == ERROR_SHARING_VIOLATION )
        SLEEP(10000 * ((INT)NUM_OF_SHARING_VIOLATION_RETRIES + 1 - Retry)); // linger on sharing violation
      }
    else 
      rc = NO_ERROR;
    }
  while (( rc == ERROR_SHARING_VIOLATION ) && --Retry );

  return Handle;
  }



/*
 * =============================== END ========================================
 */
