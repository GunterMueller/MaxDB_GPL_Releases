/*!========================================================================

  @file         RTETask_LegacyQueueElement.h
  @ingroup      Runtime
  @author       RaymondR

  @brief        Old doubly linked queue elelemts

  @since        2005-05-30  17:43
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


#ifndef RTETASK_LEGACYQUEUEELEMENT_H
#define RTETASK_LEGACYQUEUEELEMENT_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    "RunTime/RTE_Types.h"
#include    "RunTime/Tasking/RTETask_CQE_CommunicationReceiveWaitPart.h"
#include    "RunTime/Tasking/RTETask_CQE_ConnectRequest.h"
#include    "RunTime/Tasking/RTETask_CQE_TaskSleep.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


#if defined (_WIN32)

/* +---------------------------------------------------------------+
 * |   QUEUE CONTROL                                               |
 * +---------------------------------------------------------------+
 */
/* --- doubly linked queue control */
typedef struct dlq_timer_req_args_record
  {
  tsp00_Longuint                      ulTimeToWake;
  } DLQ_TIMER_REQ_ARGS_REC, tos00k_DlqTimerReqArgs;

typedef struct dlq_vasynopen_req_args_record
  {
  struct task_ctrl_record             *pReqTaskCtrl;
  LONG                                lAsynciNo;
  PSZ                                 pszLogDeviceName;
  tsp00_Bool                          fOk;
  tsp00_Bool                          fIsDevspace;
  tsp00_Bool                          fForWriting;
  ULONG                               ulBlockSize;
  tsp00_Bool*                         cancelPointer;
  ULONG                               ulMaxBlockCnt;
  ULONG                               ulErrCode;
  tsp00_VfType                         FileType;
  ULONG                               APIRetCode;
  } DLQ_VASYNOPEN_REQ_ARGS_REC, tos00k_DlqVasynopenReqArgs;

typedef struct dlq_vasynio_req_args_record
  {
  struct task_ctrl_record             *pTaskCtrl;
  LONG                                lIO_ID;
  tsp00_Bool                          fFinished;
  ULONG                               ulBlockNo;
  ULONG                               ulBlockCnt;
  PVOID                               pBlockPtr;
  tsp00_Bool                          fOk;
  ULONG                               ulErrCode;
  ULONG                               APIRetCode;
  } DLQ_VASYNIO_REQ_ARGS_REC, tos00k_DlqVasynioReqArgs;

typedef struct dlq_vasynwait_req_args_record
  {
  struct task_ctrl_record             *pReqTaskCtrl;
  LONG                                lIO_ID;
  ULONG                               ulBlockCnt;
  tsp00_Bool                          fOk;
  ULONG                               ulErrCode;
  ULONG                               APIRetCode;
  } DLQ_VASYNWAIT_REQ_ARGS_REC, tos00k_DlqVasynwaitReqArgs;

typedef struct dlq_vasyncntl_req_args_record
  {
  ULONG                               fOk;
  ULONG                               ulErrCode;
  LONG                                iCntlRequest;
  tsp00_VfType                        FileType;
  tsp00_Bool                          fDestructive;
  } DLQ_VASYNCNTL_REQ_ARGS_REC, tos00k_DlqVasyncntlReqArgs;

typedef struct dlq_vasynclose_req_args_record
  {
  LONG                                lAsynciNo;
  tsp00_Bool                          fOk;
  tsp00_Bool                          fRewind;
  tsp00_Bool                          fNxtTapeInAutoLoader;
  tsp00_Bool                          fNxtTapeAvailable;
  ULONG                               ulErrCode;
  ULONG                               APIRetCode;
  } DLQ_VASYNCLOSE_REQ_ARGS_REC, tos00k_DlqVasyncloseReqArgs;

typedef struct dlq_vattach_req_args_record
  {
  LONG                                lDevspaceNo;
  ULONG                               ulDBPageSize;
  PSZ                                 pszDevspaceToAttach;
  tsp2_dev_type_Param                 DevspaceType; 
  RTE_VolumeAccessMode                accessMode;
  bool                                fOk;
  ULONG                               ulErrCode;
  } DLQ_VATTACH_REQ_ARGS_REC, tos00k_DlqVattachReqArgs;

typedef struct dlq_io_req_args_record
  {
  LONG                                lDevspaceNo;
  LONG                                lDevspacePage;
  tsp2_io_op_code_Param               OpCode;
  FILE_SEGMENT_ELEMENT*               aSegmentArray;
  SHORT                               sNumOfPages;
  bool                                fOk;
  ULONG                               ulErrCode;
  } DLQ_IO_REQ_ARGS_REC, tos00k_DlqIoReqArgs;

typedef struct dlq_dev_worker_req_args_record
  {
  ULONG                               ulNumOfBytes;
  LONG                                lDevspaceNo;
  bool                                fOk;
  LONG                                APIRetCode;
  } DLQ_DEV_WORKER_REQ_ARGS_REC, tos00k_DlqDevWorkerReqArgs;

typedef struct dlq_mark_bad_devspace_record
  {
  LONG                                lDevspaceNo;
  LONG                                lBadDevspacePage;
  } DLQ_MARK_BAD_REQ_ARGS_REC, tos00k_DlqMarkBadDevReqArgs;


typedef struct dlq_reint_info_record
  {
  LONG                                lDevspaceNo;
  LONG                                lDevspacePage;
  bool                                fOk;
  ULONG                               ulErrCode;
  } DLQ_REINT_INFO_REQ_ARGS_REC, tos00k_DlqReintInfoReqArgs;

typedef struct dlq_vdetach_req_args_record
  {
  LONG                                lDevspaceNo;
  } DLQ_VDETACH_REQ_ARGS_REC, tos00k_DlqVdetachReqArgs;


typedef struct dlq_connect_req_args_record
  {
  HANDLE                              hCS;         
  struct comm_seg_header_record       *pCSHeader;
  HANDLE                              hPeerFCS;       /* local distrib. only */
  PFLAG_COMM_SEG                      pPeerFCS;       /* local distrib. only */
  HANDLE                              hLocalClientProcess; 
  PID                                 pidLocalClientPID; 
  HEV                                 hClientSem;
  ULONG                               ulRecvSlot;
  } DLQ_CONNECT_REQ_ARGS_REC, tos00k_DlqConnectReqArgs;

typedef struct dlq_new_task_move_meas_req_args_record
  {
  SAPDB_UInt2                         TimeVar;         /* Value 0 .. TIME_VAL_ARRAY_SIZE */
  SAPDB_UInt8                         TaskMoveIntervalStartTime;
  } tos00k_DlqNewTaskMoveMeasReqArgs;

typedef struct dlq_move_task_req_args_record
  {
  SAPDB_UInt8                         TaskMoveIntervalStartTime; /* used to skipp old requests */
  SAPDB_UInt8                         StopWaitForBestFitTaskAt; 
  struct ukt_ctrl_record              *pMoveTaskToUKT;           /* pointer to the UKT to which a Task has to be moved */
  } tos00k_DlqMoveTaskReqArgs;

typedef union dlq_req_args_record
  {
  tos00k_DlqTimerReqArgs                           TimerRequest;
  tos00k_DlqVasynopenReqArgs                       VasynOpenRequest;
  tos00k_DlqVasynioReqArgs                         VasynIORequest;
  tos00k_DlqVasynwaitReqArgs                       VasynWaitRequest;
  tos00k_DlqVasyncntlReqArgs                       VasynCntlRequest;
  tos00k_DlqVasyncloseReqArgs                      VasynCloseRequest;
  tos00k_DlqVattachReqArgs                         VattachRequest;
  tos00k_DlqVdetachReqArgs                         VdetachRequest;
  tos00k_DlqIoReqArgs                              IORequest;
  tos00k_DlqConnectReqArgs                         ConnectRequest;
  tos00k_DlqDevWorkerReqArgs                       DevWorkerRequest;
  tos00k_DlqMarkBadDevReqArgs                      MarkBadDevRequest;
  tos00k_DlqReintInfoReqArgs                       ReintInfoRequest;
  tos00k_DlqNewTaskMoveMeasReqArgs                 NewTaskMoveMeasRequest;
  tos00k_DlqMoveTaskReqArgs                        MoveTaskRequest;

  /* new objects .... */
  struct RTETask_CQE_CommunicationReceiveWaitPart  m_CommunicationReceiveWaitPart;
  struct RTETask_CQE_ConnectRequest                m_ConnectRequest;
  struct RTETask_CQE_TaskSleep                     m_TaskSleep;
  } DLQ_REQ_ARGS_REC, tos00k_DlqReqArgs;

typedef struct dlq_head_record
  {
  /* --- first is as forward, last is as backward used */
  struct  dlq_record                  *pFirstFreeElem;
  struct  dlq_record                  *pLastUsedElem;
  LONG                                lExclusive;  /* used to grant exclusive exccess */
  } DLQ_HEAD_REC, tos00k_DlqHead;

typedef struct dlq_record
  {
  struct  RTETask_CommonQueueElement  *pQueueElementObject;
  struct  dlq_record                  *pForward;
  struct  dlq_record                  *pBackward;
  struct  task_ctrl_record            *pTaskCtrl;
  struct  task_ctrl_record            *pFromTaskCtrl;

  DLQ_REQ_ARGS_REC                    ReqArgs;
  ULONG                               ulReqType;
  ULONG                               ulOrgReqType;
  } DLQ_REC, tos00k_Dlq;

typedef DLQ_HEAD_REC                  *PDLQ_HEAD_REC;
typedef PDLQ_HEAD_REC                 *PPDLQ_HEAD_REC;

typedef DLQ_REC                       *PDLQ_REC;
typedef PDLQ_REC                      *PPDLQ_REC;

#define DLQ_ELEMENT                   struct dlq_record
#define DLQ_ARGS                      DLQ_REQ_ARGS_REC

#else
/*
 *  Specific event parameter field definitions.
 */
struct REQ_PARMS_CONNECT
{
    SAPDB_Int4                            task_index ;
    SAPDB_Int4                            filler;
};

struct REQ_PARMS_VATTACH
{
    SAPDB_Int4                            devno;
    SAPDB_Int4                            ndevi;
    SAPDB_Char                            *volumeName;
    tsp2_dev_type                         devtype;
    RTE_VolumeAccessMode                  accessMode;
    SAPDB_UInt4                           ok;
    SAPDB_UInt4                           errcode;
};

struct REQ_PARMS_VBLOCKIO
{
    SAPDB_Int4                            devno;
    SAPDB_Int4                            devpno;
    SAPDB_Int4                            op_code;
    SAPDB_Int4                            buf_count; /*JH*/
    tsp00_PageAddr                        p;
    SAPDB_UInt4                           ok;
    SAPDB_UInt4                           errcode;
};

struct REQ_PARMS_VDETACH
{
    SAPDB_Int4                            devno;
    SAPDB_Int4                            ndevi;
    struct         DOUBLY_LINKED          *this_req;
    SAPDB_UInt4                           ok;
    SAPDB_UInt4                           errcode;
    SAPDB_Bool                            rewind;
    SAPDB_Bool                            filler[7];
};

struct REQ_PARMS_ASYNOPEN
{
    SAPDB_Int4                            devno;
    SAPDB_Int4                            ndevi;
    SAPDB_Char *                          deviceName;
    SAPDB_Int4                            block_size;
    SAPDB_Int4                            max_block_cnt;
    tsp00_BoolAddr                        cancelPointer;
    SAPDB_UInt4                           is_devspace;
    SAPDB_UInt4                           for_writing;
    SAPDB_UInt4                           ok;
    SAPDB_UInt4                           errcode;
    SAPDB_Int4                            fileType;
    SAPDB_Int4                            filler;
};

struct REQ_PARMS_ASYNIO
{
    SAPDB_Int4                            io_id;
    SAPDB_Int4                            devno;
    SAPDB_Int4                            devpno;
    SAPDB_Int4                            block_siz;
    SAPDB_Int4                            block_cnt;
    SAPDB_UInt4                           finished;
    SAPDB_UInt4                           ok;
    SAPDB_UInt4                           errcode;
    tsp00_Maxiobufaddr                      block_ptr;
    long                                  io_count;
    struct TASK_TYPE                      *initiator ;
    void                                  *aio_op;
    SAPDB_Int4                            aio_errno ;
    SAPDB_Int4                            filler;
};

struct REQ_PARMS_ASYNCNTL
{
    SAPDB_UInt4                           ok;
    SAPDB_UInt4                           errcode;
    SAPDB_Int4                            cntlRequest;
    SAPDB_Bool                            destructive;
    tsp00_VfType                          fileType;
    SAPDB_Bool                            filler[2];
    SAPDB_Int4                            hostfileno;
};

struct REQ_PARMS_VSIGNAL
{
    SAPDB_UInt4                           ok;
    SAPDB_Int4                            filler;
};

struct REQ_PARMS_TIMER
{
    SAPDB_Int4                            time_to_wake;
    SAPDB_Int4                            filler;
};

struct REQ_PARMS_VSUSPEND
{
    SAPDB_Int4                            susp_count;
    SAPDB_Int4                            filler;
};

struct REQ_PARMS_VNSHUTDOWN
{
    SAPDB_Int4                            pathid ;
    SAPDB_Int4                            filler;
    struct TASK_TYPE                      *initiator ;
};

struct REQ_PARMS_VVECTORIO
{
  SAPDB_Int4                            devno;
  SAPDB_Int4                            devpno;
  SAPDB_Int4                            pageCount;
  tsp00_PageAddr                        *pageVector;
  SAPDB_UInt4                           ok;
  SAPDB_UInt4                           errcode;
};

struct REQ_PARMS_MOVE_TASK_MESS_INT
{
  SAPDB_UInt2                           timeVar;         /* Value 0 .. TIME_VAL_ARRAY_SIZE */
  SAPDB_UInt8                           taskMoveIntervalStartTime;
};

struct REQ_PARMS_MOVE_TASK
{
  SAPDB_UInt8                           taskMoveIntervalStartTime; /* used to skip old requests */
  SAPDB_UInt8                           stopWaitForBestFitTaskAt; 
  struct ten50_UKT_Control              *moveTaskToUKT;            /* pointer to the UKT to which a Task has to be moved */
};

union REQ_ARGS
{
    struct REQ_PARMS_VSUSPEND                        vsu_parms;
    struct REQ_PARMS_VATTACH                         vat_parms;
    struct REQ_PARMS_VBLOCKIO                        vbl_parms;
    struct REQ_PARMS_VDETACH                         vde_parms;
    struct REQ_PARMS_ASYNOPEN                        vas_parms;
    struct REQ_PARMS_ASYNIO                          vai_parms;
    struct REQ_PARMS_ASYNCNTL                        vac_parms;
    struct REQ_PARMS_VSIGNAL                         vsig;
    struct REQ_PARMS_TIMER                           timer_auftrag;
    struct REQ_PARMS_CONNECT                         conn_auftrag;
    struct REQ_PARMS_VNSHUTDOWN                      vns_parms;
    struct REQ_PARMS_VVECTORIO                       vve_parms;
    struct REQ_PARMS_MOVE_TASK                       mt_parms;
    struct REQ_PARMS_MOVE_TASK_MESS_INT              mtmi_parms;

     /* new objects .... */
    struct RTETask_CQE_CommunicationReceiveWaitPart  m_CommunicationReceiveWaitPart;
    struct RTETask_CQE_ConnectRequest                m_ConnectRequest;
    struct RTETask_CQE_TaskSleep                     m_TaskSleep;
};
   
struct DOUBLY_LINKED
{
    struct RTETask_CommonQueueElement *pQueueElementObject;
    struct DOUBLY_LINKED              *pForward;
    struct DOUBLY_LINKED              *pBackward;
    struct TASK_TYPE                  *taskid;
    struct TASK_TYPE                  *fromtaskid;

    union  REQ_ARGS                   args;

    SAPDB_UInt4                       req_type;
    SAPDB_UInt4                       req_type_org;
    SAPDB_Int4                        filler;
};

#define DLQ_ELEMENT                   struct DOUBLY_LINKED
#define DLQ_ARGS                      union  REQ_ARGS
#endif


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/



#endif  /* RTETASK_LEGACYQUEUEELEMENT_H */
