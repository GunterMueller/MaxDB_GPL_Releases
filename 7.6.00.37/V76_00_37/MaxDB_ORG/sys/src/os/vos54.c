/*!
  @file           vos54.c
  @author         RaymondR
  @brief          Disk I/O
  @see            

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

//#include "gos00.h"
#include "gos41.h"
#include "geo50_0.h"
#include "geo002.h"
#include "heo01.h"
#include "heo00.h"
#include "heo46.h"
#include "heo52.h"
#include "RunTime/System/RTESys_MicroTime.h"
#include "heo54k.h"
#include "heo58.h"
#include "geo54.h"
#include "geo00_1.h"

#include "geo007_1.h"
#include "geo007_2.h"
#include "gos44.h"
#include "gos74.h"

#if defined(_FASTCAP)
# include "fastcap.h"   /* nocheck */
#endif
#include "RunTime/HotStandbyStorage/RTEHSS_PascalInterface.h"
#include "RunTime/Tasking/RTETask_LegacyTaskCtrl.h"

#include <winioctl.h>


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
#define MOD__  "VOS54.C : "
#define MF__   MOD__"UNDEFINED"

#define DEV_OVERLAPPED_IO                  0x1
#define DEV_NO_BUFFERING                   0x2


#define MAX_VDETACH_WAIT_TIME              60        // -- sleep time
#define MAX_VATTACH_WAIT_TIME              60        // -- sleep time


#define MAX_DEVIO_ERRCODE                  28
#define ERRCODE_DEVIO_DEVCREA              0
#define ERRCODE_DEVIO_MAXDEVNO             1
#define ERRCODE_DEVIO_ALRATT               2
#define ERRCODE_DEVIO_DEVICREA             3
#define ERRCODE_DEVIO_WRITE                4
#define ERRCODE_DEVIO_READ                 5
#define ERRCODE_DEVIO_SEEK                 6
#define ERRCODE_DEVIO_INVPNO               7
#define ERRCODE_DEVIO_LOCK                 8
#define ERRCODE_DEVIO_DEV_POS              9
#define ERRCODE_DEVIO_SEEK_POS             10
#define ERRCODE_DEVIO_ERROR_ON_DEVSPACE    11
#define ERRCODE_DEVIO_DEVSPACE_ON_RD_ONLY  12
#define ERRCODE_DEVIO_DEVSPACE_ON_SYS      13
#define ERRCODE_DEVIO_DEVSPACE_ON_DIR      14
#define ERRCODE_DEVIO_GET_FILE_INFO        15
#define ERRCODE_DEVIO_LOCK_DEVSPACE        16
#define ERRCODE_DEVIO_DEVSPACE_LOCKED      17
#define ERRCODE_DEVIO_OPEN_DEVSPACE        18
#define ERRCODE_DEVIO_ACCESS_DENIED        19
#define ERRCODE_DEVIO_WRONG_PATH           20
#define ERRCODE_DEVIO_WRONG_FILE_ATTRIB    21
#define ERRCODE_DEVIO_DEVSPACE_ON_COMP     22
#define ERRCODE_DEVIO_DEVICREA_TIMEOUT     23
#define ERRCODE_DEVIO_EVAL_DEVSPACE        24
#define ERRCODE_DEVIO_CREATE_COMPL_PORT    25
#define ERRCODE_DEVIO_USER_REQ_ERROR       26
#define ERRCODE_DEVIO_ALLOC_MEM            27


typedef enum { IO_NoError = 0,
               IO_Error,
               IO_Continue,
               IO_Pending } IORetCode;


#define MAX_DWORD_VAL                      0xFFFFFFFF
#define NUM_OF_SHARING_VIOLATION_RETRIES   3

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/
#define BUILD_FILE_SEGMENT( _pageVector, _DataLength )                                                                              \
                                         __sql54k_build_file_segment( (void*)ALIGN( (tsp00_Longuint)_alloca(sizeof(FILE_SEGMENT_ELEMENT) *   \
                                                                     ((_DataLength)/kgs.ulSystemPageSize + 3)),                     \
                                                                     sizeof(FILE_SEGMENT_ELEMENT)),                                 \
                                                                     _pageVector, (_DataLength) )


#define REQ_TO_DEVI                      __sql54k_req_to_devi
#define REQ_FROM_DEVx_TO_UKT             __sql54k_req_from_devx_to_ukt
#define REQ_FROM_DEV_WOKER_TO_UKT        __sql54k_req_from_dev_worker_to_ukt
#define REQ_TO_DEV0                      __sql54k_req_to_dev0

#define INFO_TRACE_TO_KNLDIAG           N(4711),  INFO_TYPE,_T("TRACE   ")

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/


/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/


/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/
static ERRORTEXT ErrTab[ MAX_DEVIO_ERRCODE ] =
                        {
                        ERRMSG_DEVIO_DEVCREA ,            // - ERRCODE_DEVIO_DEVCREA
                        ERRMSG_DEVIO_MAXDEVNO ,           // - ERRCODE_DEVIO_MAXDEVNO
                        ERRMSG_DEVIO_ALRATT   ,           // - ERRCODE_DEVIO_ALRATT
                        ERRMSG_DEVIO_DEVICREA ,           // - ERRCODE_DEVIO_DEVICREA
                        ERRMSG_DEVIO_WRITE    ,           // - ERRCODE_DEVIO_WRITE
                        ERRMSG_DEVIO_READ     ,           // - ERRCODE_DEVIO_READ
                        ERRMSG_DEVIO_SEEK     ,           // - ERRCODE_DEVIO_SEEK
                        ERRMSG_DEVIO_INVPNO   ,           // - ERRCODE_DEVIO_INVPNO
                        ERRMSG_DEVIO_LOCK     ,           // - ERRCODE_DEVIO_LOCK
                        ERRMSG_DEVIO_DEV_POS  ,           // - ERRCODE_DEVIO_DEV_POS
                        ERRMSG_DEVIO_SEEK_POS ,           // - ERRCODE_DEVIO_SEEK_POS
                        ERRMSG_DEVIO_ERROR_ON_DEVSPACE,   // - ERRCODE_DEVIO_ERROR_ON_DEVSPACE
                        ERRMSG_DEVIO_DEVSPACE_ON_RD_ONLY, // - ERRCODE_DEVIO_DEVSPACE_ON_RD_ONLY
                        ERRMSG_DEVIO_DEVSPACE_ON_SYS,     // - ERRCODE_DEVIO_DEVSPACE_ON_SYS
                        ERRMSG_DEVIO_DEVSPACE_ON_DIR,     // - ERRCODE_DEVIO_DEVSPACE_ON_DIR
                        ERRMSG_DEVIO_GET_FILE_INFO,       // - ERRCODE_DEVIO_GET_FILE_INFO
                        ERRMSG_DEVIO_LOCK_DEVSPACE,       // - ERRCODE_DEVIO_LOCK_DEVSPACE
                        ERRMSG_DEVIO_DEVSPACE_LOCKED,     // - ERRCODE_DEVIO_DEVSPACE_LOCKED
                        ERRMSG_DEVIO_OPEN_DEVSPACE,       // - ERRCODE_DEVIO_OPEN_DEVSPACE
                        ERRMSG_DEVIO_ACCESS_DENIED,       // - ERRCODE_DEVIO_ACCESS_DENIED
                        ERRMSG_DEVIO_WRONG_PATH,          // - ERRCODE_DEVIO_WRONG_PATH
                        ERRMSG_DEVIO_WRONG_FILE_ATTRIB,   // - ERRCODE_DEVIO_WRONG_FILE_ATTRIB
                        ERRMSG_DEVIO_DEVSPACE_COMPRESSED, // - ERRCODE_DEVIO_DEVSPACE_ON_COMP
                        ERRMSG_DEVIO_DEVICREA_TIMEOUT,    // - ERRCODE_DEVIO_DEVICREA_TIMEOUT
                        ERRMSG_DEVIO_EVAL_DEVSPACE,       // - ERRCODE_DEVIO_EVAL_DEVSPACE
                        ERRMSG_DEVIO_CREATE_COMPL_PORT,   // - ERRCODE_DEVIO_CREATE_COMPL_PORT
                        ERRMSG_DEVIO_USER_REQ_ERROR,      // - ERRCODE_DEVIO_USER_REQ_ERROR
                        ERRMSG_DEVIO_ALLOC_MEM            // - ERRCODE_DEVIO_ALLOC_MEM
                        };


/*===========================================================================*
 *  LOCAL FUNCTIONS (PROTOTYPES)                                             *
 *===========================================================================*/
LONG         os54_FillBlock0     ( HANDLE                    DevHandle,
                                       PSZ                       pszLogDevName,
                                       ULONG                     ulDevPages,
                                       ULONG                     ulMaxDevPages,
                                       ERRORTEXT                 errtext);

LONG         sql54k_format_devspace  ( HANDLE                    DevHandle,
                                       PSZ                       pszLogDevName,
                                       ULONG                     ulCurrFileSizeLow,
                                       ULONG                     ulCurrFileSizeHigh,
                                       ULONG                     ulDevPages,
                                       tsp00_TaskId              pidResponsibleForCall,
                                       char                      *diskImage,
                                       ERRORTEXT                 errtext);

LONG         sql54k_evaluate_block_0 ( HANDLE                    DevHandle,
                                       PULONG                    pulDevPages,
                                       PULONG                    pulMaxDevPages,
                                       ULONG                     ulDBPageSize,
                                       BOOL                      fDevspaceIsDrive,
                                       PSZ                       pszLogDevName );

INT          os54_vdevsize         ( PSZ                       pszPhysDevName,
                                       PSZ                       pszLogDevName,
                                       INT4                      *devcapacity,
                                       tsp00_TaskId              pidResponsibleForCall,
                                       char                      *diskImage,
                                       int                       isSequential,
                                       int                       isForcedReadOnly,
                                       ULONG                     ulCurrFileSizeLow,
                                       ULONG                     ulCurrFileSizeHigh,
                                       ERRORTEXT                 errtext,
                                       BOOLEAN                   *ok );

void          os54_vdevsizeDrive   ( PSZ                       pszDriveName,
                                       INT4                      *devcapacity,
                                       tsp00_TaskId              pidResponsibleForCall,
                                       char                      *diskImage,
                                       int                       isSequential,
                                       int                       isForcedReadOnly,
                                       ERRORTEXT                 errtext,
                                       BOOLEAN                   *ok );

LONG         sql54k_dev_open         ( PSZ                       pszLogDevName,
                                       LONG                      lDevspaceNo,
                                       SAPDB_Bool                isForcedReadOnly,
                                       ULONG                     ulOptions,
                                       HANDLE                   *pDevHandle,
                                       SAPDB_Bool               *pIsLocked,
                                       ERRORTEXT                 pcErrText,
                                       PULONG                    pulErrCode,
                                       LONG                     *lDevspaceType );

LONG         sql54k_dev_close        ( HANDLE                    DevHandle,
                                       SAPDB_Bool               *pIsLocked,
                                       char                     *pszDriveName,
                                       LONG                      lDevspaceType,
                                       ERRORTEXT                 pcErrText);



VOID         sql54k_dev0_vattach     ( DLQ_VATTACH_REQ_ARGS_REC  *pVattachArgs );

VOID         sql54k_devi_kill        ( LONG                      lDevspaceNo );

PDLQ_REC     sql54k_dev_receive      ( PPIO_QUEUE_REC            ppIOWorkQue,
                                       PLONG                     plIOQueLen,
                                       PLONG                     plIOWorkExclusive,
                                       PTHREAD_CTRL_HEADER_REC   pThrdCtrlHeader,
                                       teo52_DevIOStatistic     *pDeviStatistic);

VOID         sql54k_dev0_vdetach     ( DLQ_VDETACH_REQ_ARGS_REC  *pVdetachArgs,
                                       PDLQ_REC                  pRequest );

LONG         sql54k_vmark_bad_dev    ( DLQ_MARK_BAD_REQ_ARGS_REC *pMarkBadDevRequest );

LONG         sql54k_reint_info       ( PDLQ_REC                  pRequest,
                                       PDEVI_CTRL_REC            pDeviCtrl );

VOID _System sql54k_devi             ( PDEVI_CTRL_REC            pDeviCtrl );

LONG         os54_set_volume_lock    ( HANDLE                    FileHandle,
                                       SAPDB_Bool               *pIsLocked,
                                       PSZ                       pszLogDevName );

VOID         sql54k_devi_vdetach     ( PDEVI_CTRL_REC            pDeviCtrl );

BOOL         sql54k_find_device_no ( PLONG                     plDevspaceNo,
                                       PSZ                       pszLogDevName,
                                       PULONG                    pulErrCode );

PDEVI_CTRL_REC sql54k_find_devi      ( LONG                      lDevspaceNo,
                                       PULONG                    pulQueueLength );

LONG         sql54k_single_io        ( PUKT_CTRL_REC              pUKT,
                                       LONG                       lDevspaceNo,
                                       tsp00_PageNo                lDevspacePage,
                                       tsp2_io_op_code_Param      OpCode,
                                       FILE_SEGMENT_ELEMENT       aSegmentArray[],
                                       SHORT                      sNumOfPages,
                                       ERRORTEXT                  pcErrText );

VOID         sql54k_devi_io          ( PDLQ_REC                   pRequest,
                                       PDEVI_CTRL_REC             pDeviCtrl );

LONG         sql54k_devi_write       ( PDEVI_CTRL_REC             pDeviCtrl,
                                       DLQ_IO_REQ_ARGS_REC        *pIOArgs );

LONG         sql54k_devi_read        ( PDEVI_CTRL_REC             pDeviCtrl,
                                       DLQ_IO_REQ_ARGS_REC        *pIOArgs );

VOID         sql54k_wake_up_tasks    ( PPIO_QUEUE_REC             ppIOWorkQue,
                                       PLONG                      plIOWorkExclusive );

LONG         sql54k_lock_drive       ( HANDLE                     FileHandle );

LONG         sql54k_unlock_drive     ( HANDLE                     FileHandle );

static IORetCode sql54k_overlapped_io ( PUKT_CTRL_REC              *ppUKT,
                                        LONG                       lDevspaceNo,
                                        tsp00_PageNo                lDevspacePage,
                                        tsp2_io_op_code_Param      OpCode,
                                        FILE_SEGMENT_ELEMENT       aSegmentArray[],
                                        SHORT                      sNumOfPages,
                                        ERRORTEXT                  pcErrText );

static IORetCode sql54k_overlapped_dual_write_io( PUKT_CTRL_REC                *ppUKT,
                                                  LONG                         lDevspaceNo1,
                                                  LONG                         lDevspaceNo2,
                                                  tsp00_PageNo                  lDevspacePage,
                                                  FILE_SEGMENT_ELEMENT         aSegmentArray[],
                                                  SHORT                        sNumOfPages,
                                                  ERRORTEXT                    pcErrText1,
                                                  ERRORTEXT                    pcErrText2,
                                                  tsp00_Bool    VAR_VALUE_REF ok1,
                                                  tsp00_Bool    VAR_VALUE_REF ok2);

LONG sql54k_uncompress		           ( HANDLE					            FileHandle );


// --- inline functions
_INLINE VOID __sql54k_io_finisched               ( PTASK_CTRL_REC pTaskCtrl );

_INLINE BOOL __sql54k_is_user_req_io_error       ( LONG                  lDevspaceNo,
                                                   tsp2_io_op_code_Param OpCode );

_INLINE VOID __sql54k_req_to_devi                ( PDEVI_CTRL_REC        pDevCtrl,
                                                   PDLQ_REC              pRequest );

_INLINE VOID __sql54k_req_from_devx_to_ukt       ( PDLQ_REC              pRequest );

_INLINE VOID __sql54k_req_to_dev0                ( PDLQ_REC              pRequest,
                                                   PTASK_CTRL_REC        pTaskCtrl );

_INLINE VOID __sql54k_req_from_dev_worker_to_ukt ( BOOL                  fOk,
                                                   ULONG                 ulNumOfBytes,
                                                   LONG                  lDevspaceNo,
                                                   LPOVERLAPPED          pCompletedOverlapped,
                                                   LONG                  rc );
_INLINE LONG __sql54k_io_write                   ( HANDLE                DevHandle,
                                                   FILE_SEGMENT_ELEMENT  aSegmentArray[],
                                                   ULONG                 ulPages,
                                                   LPOVERLAPPED          pOverlapped );

_INLINE LONG __sql54k_io_read_with_retries       ( HANDLE                DevHandle,
                                                   FILE_SEGMENT_ELEMENT  aSegmentArray[],
                                                   SHORT                 sNumOfPages,
                                                   ULONG                 lDevspacePage,
                                                   PSZ                   szDevspace,
                                                   ULONG                 ulRetryCnt,
                                                   BOOL                  fNoErrorOutput );

_INLINE LONG __sql54k_io_read                    ( HANDLE                DevHandle,
                                                   FILE_SEGMENT_ELEMENT  aSegmentArray[],
                                                   ULONG                 ulPages,
                                                   LPOVERLAPPED          pOverlapped );
_INLINE FILE_SEGMENT_ELEMENT*
             __sql54k_build_file_segment         ( void*                 pSegBuffer,
                                                   tsp00_PageAddr        pageVector[],
                                                   ULONG                 DataLength );

_INLINE IORetCode __sql54k_read_write_overlapped ( PUKT_CTRL_REC          pUKT,
                                                   LONG                   lDevspaceNo,
                                                   tsp00_PageNo            lDevspacePage,
                                                   tsp2_io_op_code_Param  OpCode,
                                                   FILE_SEGMENT_ELEMENT   aSegmentArray[],
                                                   SHORT                  sNumOfPages,
                                                   OVERLAPPED            *pOverlapped,
                                                   ERRORTEXT              pcErrText );

_INLINE  IORetCode __sql54k_overlapped_io_wait   ( PUKT_CTRL_REC          *ppUKT,
                                                   LONG                   lDevspaceNo,
                                                   tsp00_PageNo            lDevspacePage,
                                                   tsp2_io_op_code_Param  OpCode,
                                                   SHORT                  sNumOfPages,
                                                   ERRORTEXT              pcErrText );

_INLINE  void __sql54k_overlapped_dual_io_wait (  PUKT_CTRL_REC          *ppUKT,
                                                   LONG                   lDevspaceNo1,
                                                   LONG                   lDevspaceNo2,
                                                   tsp00_PageNo            lDevspacePage,
                                                   SHORT                  sNumOfPages,
                                                   ERRORTEXT              pcErrText1,
                                                   ERRORTEXT              pcErrText2,
                                                   tsp00_Bool    VAR_VALUE_REF ok1,
                                                   tsp00_Bool    VAR_VALUE_REF ok2);

_INLINE HANDLE     __sql54k_create_file         ( LPCTSTR                 lpFileName,
                                                  DWORD                   dwDesiredAccess,
                                                  DWORD                   dwShareMode,
                                                  LPSECURITY_ATTRIBUTES   lpSecurityAttributes,
                                                  DWORD                   dwCreationDisposition,
                                                  DWORD                   dwFlagsAndAttributes,
                                                  HANDLE                  hTemplateFile );


#ifdef DEVELOP_CHECKS
 _INLINE LONG __sql54k_check_filepos             ( HANDLE                FileHandle,
                                                   ULONG                 lDevspaceNo,
                                                   tsp00_PageNo           lDevspacePage,
                                                   SHORT                 sNumOfPages,
                                                   ULONG                 ulDevPages,
                                                   PULONG                pulErrCode,
                                                   ERRORTEXT             pcErrText );
#endif

static void os54_WaitForSelfIOCompletionAndClose( tsp00_Int4 lDevspaceNo );

static void os54_CloseSingleIOFileDesc ( PUKT_CTRL_REC pTmpUKTCtrl,
                                         tsp00_Int4    lDevspaceNo );

/*! @brief check and set HS Master role

    If during log volume initialization a non existing or uninitialized log volume is found
    this routine sets the master role automatically. If not in hot standby configuration,
    not hot standby default master, or set master role fails, the given flag is untouched.

    If read only flag is given already as false, the routine does nothing and reeturns true. 

    @param pIsForcedReadOnly [inout] 
    @return true if log volumes are or are no longer read only
 */
static SAPDB_Bool
ResetForcedReadOnlyIfHSDefaultMaster(SAPDB_Bool *pIsForcedReadOnly);

_INLINE void os54_UpdateCountInIOStatisticRec(
 teo52_IOStatistic * pRec,
 SAPDB_UInt8 pageCount )
{
    pRec->IOStat.TotalTimeStat.ulCount++;
    pRec->TotalPageCount+= pageCount;
    pRec->IOStat.TimeStat.ulCount++;
    pRec->ulPageCount+= pageCount;
}

/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

VOID vcheckopenvolume(
    tsp00_Int4          type_spec_devno,
    tsp2_dev_type_Param devtype,
    BOOLEAN            *ok)
{
    DEV_DESCR            *devsp;
    devsp = sql58k_get_devsp_descr (devtype, --type_spec_devno );

    *ok = (devsp != NULL);
    if ( *ok )
    {
        tsp2_devnamec szLogDevName;
        PATHNAME      szPhysDevName;
        HANDLE        FileHandle;

        strcpy ( szLogDevName , devsp->szDevspaceName ) ;
        // --- substitute logical path name parts
        sql44c_subst_log_parts ( szPhysDevName, szLogDevName );
        FileHandle = CreateFile( szPhysDevName,
                                 GENERIC_READ,
                                 FILE_SHARE_READ | FILE_SHARE_WRITE,
                                 NULL,
                                 OPEN_EXISTING,
                                 0,
                                 NULL);
        *ok = (FileHandle != INVALID_HANDLE_VALUE);
        if ( *ok )
        {
            CloseHandle(FileHandle);
        }
    }
}

VOID vdevsizeWithImage ( tsp00_Int4             type_spec_devno,
                tsp2_dev_type_Param  devtype,
                tsp00_Int4             *devcapacity,
                tsp00_TaskId         pidResponsibleForCall,
                char                 *diskImage,
                tsp00_ErrText          errtext,
                BOOLEAN              *ok )
  {
# undef  MF__
# define MF__  MOD__"vdevsizeWithImage"

  PATHNAME             szPhysDevName;
  tsp2_devnamec        szLogDevName;
  LONG                 lAtt;
  ULONG                ulCurrFileSizeLow  = 0;
  ULONG                ulCurrFileSizeHigh = 0;
  LONG                 rc                 = NO_ERROR;
  DEV_DESCR            *devsp;
  SAPDB_Bool  isForcedReadOnly = ( devtype == sp2dt_log || devtype == sp2dt_mirr_log ) && RTEHSS_IsLogReadOnly();
  DBGIN;

  devsp = sql58k_get_devsp_descr (devtype, --type_spec_devno );

  *ok = (devsp != NULL);

  if ( *ok )
    {
    *ok = true;
    strcpy ( szLogDevName , devsp->szDevspaceName ) ;
    // --- substitute logical path name parts
    sql44c_subst_log_parts ( szPhysDevName, szLogDevName );
    }
  else
    MSGALL(( ERR_INVALID_DEVSP_DESCR,  devtype, type_spec_devno+1 )) ;

  if ( *ok )
    {
      int isSequential;

    *ok = false;

    isSequential = ( devsp->accessMode == RTE_VolumeAccessModeSequential ); 

    switch ( sql44c_get_dev_type_by_filename ( szPhysDevName ) )
      {
      case DT_DRIVE:
        os54_vdevsizeDrive (szPhysDevName, 
                               devcapacity, 
                               pidResponsibleForCall, 
                               diskImage,
                               isSequential,
                               isForcedReadOnly,
                               errtext, 
                               ok);
        break;

      default :
        DBG3 ((MF__, "devspace: '%s'- devcapacity: %d ",
                     szPhysDevName, *devcapacity));

        rc = sql44c_get_file_info ( szPhysDevName, &lAtt,
                                    &ulCurrFileSizeLow, &ulCurrFileSizeHigh );

        if ( rc == ERROR_ACCESS_DENIED )
          {
          MSGALL(( ERR_DEVSPACE_ACCESS_DENIED, szLogDevName ));
          DBG1  (( MF__, "Access on devspace '%s' denied", szLogDevName ));
          sql46c_build_error_string ( errtext, ERRMSG_DEVIO_ACCESS_DENIED, 0 );
          return;
          }
        else if (rc == ERROR_PATH_NOT_FOUND)
          {
          sql46c_build_error_string (errtext, ERRMSG_DEVIO_WRONG_PATH, 0 );
          MSGALL(( ERR_WRONG_PATH, szLogDevName ));
          DBG1  (( MF__, "Wrong path '%s'", szLogDevName ));
          return;
          }
        else if ( rc == ERROR_FILE_NOT_FOUND )
          {
              if ( !ResetForcedReadOnlyIfHSDefaultMaster(&isForcedReadOnly) )
              {
                  sql46c_build_error_string (errtext, ERRMSG_DEVIO_GET_FILE_INFO, rc );
                  MSGALL(( ERR_GET_FILE_INFO, "Missing LogVolumeHotStandby", rc ));
                  return;
              }
              else
              {
                  rc = NO_ERROR;
              }
          }
        else if ( rc != NO_ERROR )
          {
          sql46c_build_error_string (errtext, ERRMSG_DEVIO_GET_FILE_INFO, rc );
          MSGALL(( ERR_GET_FILE_INFO, szLogDevName, rc ));
          DBG1  (( MF__, "Query file info error on '%s', rc = %ld",
                          szLogDevName, rc ));
          return;
          }
        else
        {
            if ( 0 == ulCurrFileSizeHigh )
            {
                if ( XPARAM(ulDBPageSize) >= ulCurrFileSizeLow )
                {
                    if ( !ResetForcedReadOnlyIfHSDefaultMaster(&isForcedReadOnly) )
                    {
                          sql46c_build_error_string (errtext, ERRMSG_DEVIO_GET_FILE_INFO, 0 );
                          MSGALL(( ERR_GET_FILE_INFO, "Empty LogVolumeHotStandby", 0 ));
                          return;
                    }
                }
            }
        }

        if ( lAtt == -1 )
          {
          MSGALL(( ERR_UNKNOWN_FILE_ATTRIBUTE, szLogDevName ));
          DBG1  (( MF__, "'%s' has a unknown file attribute", szLogDevName  ));
          sql46c_build_error_string ( errtext,
                                      ERRMSG_DEVIO_WRONG_FILE_ATTRIB, 0 );
          }
        else if ((lAtt & FILE_DIRECTORY) == FILE_DIRECTORY)
          {
          MSGALL(( ERR_IO_ON_DIR, szLogDevName ));
          DBG1  (( MF__, "No Devspaces on directory '%s' !", szLogDevName ));
          sql46c_build_error_string ( errtext,
                                      ERRMSG_DEVIO_DEVSPACE_ON_DIR, 0 );
          }
        else if ((lAtt & FILE_SYSTEM) == FILE_SYSTEM)
          {
          MSGALL(( ERR_IO_ON_SYS, szLogDevName ));
          DBG1  (( MF__, "No Devspaces on system file '%s' !", szLogDevName ));
          sql46c_build_error_string ( errtext,
                                      ERRMSG_DEVIO_DEVSPACE_ON_SYS, 0 );
          }
        else if ((lAtt & FILE_READONLY) == FILE_READONLY)
          {
          MSGALL(( ERR_DEVSPACE_ON_RD_ONLY, szLogDevName ));
          DBG1  (( MF__, "No Devspaces on read-only file '%s' !",
                          szLogDevName  ));
          sql46c_build_error_string ( errtext,
                                      ERRMSG_DEVIO_DEVSPACE_ON_RD_ONLY, 0 );
          }
        else if (((lAtt & FILE_COMPRESSED)       == FILE_COMPRESSED) &&
                 ( XPARAM(fAllowCompressedDevsp) == false ) &&
		  	   ( ulCurrFileSizeLow || ulCurrFileSizeHigh ))
          {
          MSGALL(( ERR_DEVSPACE_COMPRESSED, szLogDevName ));
          DBG1  (( MF__, "No Devspaces on compressed file '%s' !",
                          szLogDevName  ));
          sql46c_build_error_string ( errtext,
                                      ERRMSG_DEVIO_DEVSPACE_COMPRESSED, 0 );
          }
         else
          {
          os54_vdevsize ( szPhysDevName, szLogDevName, devcapacity, 
                            pidResponsibleForCall,
                            diskImage,
                            isSequential,
                            isForcedReadOnly,
                            ulCurrFileSizeLow, ulCurrFileSizeHigh, errtext, ok );
          }
      }

    DBG3 (( MF__, "devcapacity: %d", *devcapacity ));
    }
  DBGOUT;
  }

/*---------------------------------------------------------------------------*/

VOID vdevsize ( tsp00_Int4             type_spec_devno,
                tsp2_dev_type_Param  devtype,
                tsp00_Int4             *devcapacity,
                tsp00_ErrText          errtext,
                BOOLEAN              *ok )
{
# undef  MF__
# define MF__  MOD__"vdevsize"
    PUKT_CTRL_REC  pUKT  = THIS_UKT_CTRL;
    LONG           rc    = NO_ERROR;

    if ( (*devcapacity != 0)
      && (XPARAM(fFormatDataVolume) == true) )
    {
      // --- set the thread priority
      rc = SET_THREAD_PRIO ( THREAD_PRIORITY_IDLE,
                             pUKT->ThrdCtrlHeader.Tid,
                             pUKT->ThrdCtrlHeader.hThrd );

      if ( rc != NO_ERROR )
      {
         MSGD (( ERR_SETTING_THRD_PRIO, pUKT->ThrdCtrlHeader.Tid));
         DBG1 ((MF__, "Cannot set the thread priority, TID: %u, rc=%d",
                pUKT->ThrdCtrlHeader.Tid, rc ));
         /* This error is not severe, so we do not report it to the caller!!! */
      }

    }

    vdevsizeWithImage(type_spec_devno, 
                      devtype, 
                      devcapacity, 
                      pUKT->pCTask->ulTaskIndex, 
                      (char *)0, 
                      errtext, 
                      ok);

    // --- reset the thread priority
    rc = SET_THREAD_PRIO ( pUKT->ThrdCtrlHeader.lThrdPrio,
                           pUKT->ThrdCtrlHeader.Tid,
                           pUKT->ThrdCtrlHeader.hThrd );

    if ( rc != NO_ERROR )
    {
        MSGD (( ERR_SETTING_THRD_PRIO, pUKT->ThrdCtrlHeader.Tid));
        DBG1 ((MF__, "Cannot set the thread priority, TID: %u, rc=%d",
            pUKT->ThrdCtrlHeader.Tid, rc ));
        /* This error is not severe, so we do not report it to the caller!!! */
    }
}

/*------------------------------*/

VOID vattach ( tsp00_Int4             type_spec_devno,
               tsp2_dev_type_Param  devtype,
               tsp00_Int4             *devno,
               PROCESS_ID           taskId,
               tsp00_ErrText          errtext,
               BOOLEAN              *ok )
  {
# undef  MF__
# define MF__ MOD__"vattach"
  DEV_DESCR                      *devsp ;
  tsp2_devnamec                  szLogDevName;
  PDLQ_REC                       pRequest;
  #ifdef DEVELOP_CHECKS
   PUKT_CTRL_REC                 pUKT      = THIS_UKT_CTRL;
   PTASK_CTRL_REC                pTaskCtrl = pUKT->pCTask;
  #else
   PTASK_CTRL_REC                pTaskCtrl = &kgs.pFirstTaskCtrl[taskId - 1];
   PUKT_CTRL_REC                 pUKT      = pTaskCtrl->pUKT;
  #endif

  DBGIN_T (pTaskCtrl->ulTaskIndex);

  #ifdef DEVELOP_CHECKS
   if ( (ULONG)taskId != pTaskCtrl->ulTaskIndex )
     {
     MSGD (( ERR_VXXXX_WRONG_TASK, "vattach", taskId ));
     ABORT();
     }
  #endif

  if ( kgs.Dev0.ThrdCtrlHeader.ThrdState == KT_INITIALIZED )
    {
    MSGALL(( ERR_THREAD_XXX_NOT_ACTIVE, "DEV0" ));
    sql46c_build_error_string ( errtext, ERRMSG_DEVIO_DEV0_NOT_ACTIVE, 0 );
    *ok = false;
    return;
    }

  pTaskCtrl->TaskState = TSK_VATTACH_EO00;

  devsp = sql58k_get_devsp_descr (devtype, --type_spec_devno ) ;

  if ( devsp != NULL )
     strcpy ( szLogDevName , devsp->szDevspaceName ) ;
  else
    {
    MSGALL(( ERR_INVALID_DEVSP_DESCR,  devtype, type_spec_devno+1 )) ;
    *ok = false;
    return;
    }

  MSGD (( INFO_ATTACHING_DEVSPACE, szLogDevName ));

  //
  //
  // ---  Send an attach-request.
  //
  //
  pRequest                                             = GetTaskDLQElement1( pTaskCtrl );
  pRequest->pTaskCtrl                                  = pTaskCtrl;
  pRequest->ulReqType                                  = REQ_VATTACH;
  pRequest->ReqArgs.VattachRequest.ulDBPageSize        = XPARAM(ulDBPageSize);
  pRequest->ReqArgs.VattachRequest.fOk                 = false;
  pRequest->ReqArgs.VattachRequest.pszDevspaceToAttach = szLogDevName;
  pRequest->ReqArgs.VattachRequest.DevspaceType        = devtype;
  pRequest->ReqArgs.VattachRequest.accessMode          = devsp->accessMode;


  // --- send request to DEV0 thread
  REQ_TO_DEV0( pRequest, pTaskCtrl );

  // --- goto dispatcher
  GOTO_DISP(&pUKT);

  DBG3 (( MF__, "[T:0x%03u] Returned from request pUKT: %x",
          pTaskCtrl->ulTaskIndex, pUKT ));

  // --- wrong reply?
  if ( pUKT->pCurrReq != pRequest )
    {
    MSGALL(( ERR_UNKNOWN_REQUEST ));
    DBG1  (( MF__, "[T:0x%03u] pRequest: 0x%x (SEND) != pUKT->pCurrReq: 0x%x (REPLY)",
            pTaskCtrl->ulTaskIndex, pRequest, pUKT->pCurrReq ));
    ABORT();
    }

  *ok = pRequest->ReqArgs.VattachRequest.fOk;

  if ( *ok == true )
    {
    *devno = pRequest->ReqArgs.VattachRequest.lDevspaceNo;

    DBG3 (( MF__, "[T:0x%03u] Returned OK, pRequest: 0x%x",
            pTaskCtrl->ulTaskIndex, pRequest ));
    }
  else
    {
    //
    // --- replied via UKT queue
    //
    if ( pRequest->ReqArgs.VattachRequest.ulErrCode < MAX_DEVIO_ERRCODE )
      {
      sql46c_build_error_string ( errtext,
                                  ErrTab[ pRequest->ReqArgs.VattachRequest.ulErrCode ], 0 );

      DBG1 (( MF__, "[T:0x%03u] pRequest: 0x%x, %s",
              pTaskCtrl->ulTaskIndex, pRequest,
              ErrTab[ pRequest->ReqArgs.VattachRequest.ulErrCode ] ));
      }
    }

  pTaskCtrl->TaskState = TSK_RUNNING_EO00;

  DBGOUT_T (pTaskCtrl->ulTaskIndex);
  return;
  }

/*------------------------------*/

INT4  vget_data_io_cnt ( VOID )
  {
# undef  MF__
# define MF__ MOD__"vget_data_io_cnt"

  DBGPAS;

  return (INT4)kgs.TotalDataIOCnt;
  }

/*------------------------------*/

VOID  vblockio ( INT4                    devno,
                 tsp00_PageNo             devpno,
                 tsp2_io_op_code_Param   op_code,
                 BUFFERADDRESS           pageAddr,
                 INT2                    pageCount,
                 PROCESS_ID              taskId,
                 ERRORTEXT               errtext,
                 BOOLEAN                 * ok )
  {
# undef  MF__
# define MF__ MOD__"vblockio"
  PDLQ_REC                       pRequest;
  PDEVI_CTRL_REC                 pDeviCtrl;
  ULONG                          ulDummy;
  IORetCode                      IORet;
  FILE_SEGMENT_ELEMENT           aSegmentArray[2];
  #ifdef DEVELOP_CHECKS
   PUKT_CTRL_REC                 pUKT           = THIS_UKT_CTRL;
   PTASK_CTRL_REC                pTaskCtrl      = pUKT->pCTask;
  #else
   PTASK_CTRL_REC                pTaskCtrl      = &kgs.pFirstTaskCtrl[taskId - 1];
   PUKT_CTRL_REC                 pUKT           = pTaskCtrl->pUKT;
  #endif
  BOOLEAN                        fIsReading     = op_code == DEV_READ;
  teo52_TaskDevIOStatistic      *pIOStat;
  PDEV_IO_CTRL_REC               pIOCtrl;

  DBGIN_T (pTaskCtrl->ulTaskIndex);

  #ifdef DEVELOP_CHECKS
   if ( (ULONG)taskId != pTaskCtrl->ulTaskIndex )
     {
     MSGD (( ERR_VXXXX_WRONG_TASK, "vblockio", taskId ));
     ABORT();
     }
  #endif

  DBG3 (( MF__, "[T:0x%03u] devno: %u,  devpno: %u  op_code: %u",
          pTaskCtrl->ulTaskIndex, devno, devpno, op_code ));

  if (devno < 0 || devno >= XPARAM(lMaxDevspaces) )
    {
    MSGALL(( ERR_INVALID_DEVNO ));
    sql46c_build_error_string ( errtext, ERRMSG_DEVIO_INVALID_DEVNO, 0 );
    *ok = false;
    return;
    }

  // --- devspace not attached correctly?
  pIOCtrl = &kgs.Dev0.pIOCtrl[devno] ;

  if ( pIOCtrl->ulAttached      == 0 ||
       pIOCtrl->ulDetachPending != 0 )
    {
    MSGD (( ERR_DEVSPACE_NOT_ATT ));
    sql46c_build_error_string ( errtext, ERRMSG_DEVIO_NOT_ATT, 0 );
    *ok = false;

    pTaskCtrl->TaskState = TSK_RUNNING_EO00;
    DBGOUT_T (pTaskCtrl->ulTaskIndex);
    return;
    }

  if (( pTaskCtrl->TaskType   != TT_DW_EO00 ) &&
      ( pIOCtrl->DevspaceType == sp2dt_data  ))
  {
       if ( ++(kgs.TotalDataIOCnt) >= 0x7fffffff )
          kgs.TotalDataIOCnt = 0 ;
  }

  // -- we have one or more pages in a single buffer
  aSegmentArray[0].Buffer   = pageAddr;
  aSegmentArray[1].Buffer   = NULL;

  pTaskCtrl->TaskState = fIsReading ? TSK_IO_READ_EO00 : TSK_IO_WRITE_EO00;

  #ifdef DEVELOPMENT
   pTaskCtrl->ulWaitTime = kgs.ulCurrTime;
  #endif

  // ---  simulate an I/O error for test porposes ?
  if ( __sql54k_is_user_req_io_error((long )devno, op_code) )
    {
    sql46c_build_error_string ( errtext, ERRMSG_DEVIO_USER_REQ_ERROR, 0 );
    *ok = false;
    MSGD ((ERR_TEST_IO_ERROR,
           fIsReading ? "reading from" : "writing to", devno, devpno ));

    pTaskCtrl->TaskState = TSK_RUNNING_EO00;
    DBGOUT_T (pTaskCtrl->ulTaskIndex);
    return;
    }

  pTaskCtrl->lWaitForDevno     = devno;
  pTaskCtrl->DevNameWaitingFor = pIOCtrl->szDevspace;

  if (( pUKT->ulActiveTasks == 1 ) && ( XPARAM(fAllowSingleIO) ))
    {
    teo52_IOStatistic *pUKT_SelfIOStat;
    teo52_IOStatistic  *pTask_SelfIOStat;
    if ( fIsReading )
    {
      pUKT_SelfIOStat   = &(pUKT->pSingleIO[devno].ReadSelfIo);
      pTask_SelfIOStat = &pTaskCtrl->TaskStateStat.SelfIOReadStat;
    }
    else
    {
      pUKT_SelfIOStat   = &(pUKT->pSingleIO[devno].WriteSelfIo);
      pTask_SelfIOStat = &pTaskCtrl->TaskStateStat.SelfIOWriteStat;
    }

    // --- use single io
    if ( XPARAM(fTimeMeasure) )
    {
         SAPDB_UInt8  BegMicroSeconds, deltaMicroSeconds;

         BegMicroSeconds = RTESys_MicroSecTimer();
         IORet = sql54k_single_io ( pUKT, devno, devpno, op_code,
                                          aSegmentArray, pageCount, errtext );

         deltaMicroSeconds = RTESys_MicroSecTimer() - BegMicroSeconds;
         pTask_SelfIOStat->IOStat.TotalTimeStat.ulAbsMicroSeconds += deltaMicroSeconds;
         pTask_SelfIOStat->IOStat.TotalTimeStat.ulMeasurements++;
         pTask_SelfIOStat->IOStat.TimeStat.ulAbsMicroSeconds += deltaMicroSeconds;
         pTask_SelfIOStat->IOStat.TimeStat.ulMeasurements++;
         pUKT_SelfIOStat->IOStat.TotalTimeStat.ulAbsMicroSeconds += deltaMicroSeconds;
         pUKT_SelfIOStat->IOStat.TotalTimeStat.ulMeasurements++;
         pUKT_SelfIOStat->IOStat.TimeStat.ulAbsMicroSeconds += deltaMicroSeconds;
         pUKT_SelfIOStat->IOStat.TimeStat.ulMeasurements++;
    }
    else
    {
         IORet = sql54k_single_io ( pUKT, devno, devpno, op_code,
                                          aSegmentArray, pageCount, errtext );
    }
    os54_UpdateCountInIOStatisticRec(pTask_SelfIOStat, pageCount);
    os54_UpdateCountInIOStatisticRec(pUKT_SelfIOStat, pageCount);

    if ( IORet != IO_Continue )
      {
      if ( IORet == IO_NoError )
        *ok = true;
      else
        *ok = false;

      __sql54k_io_finisched(pTaskCtrl);
      return;
      }
    //
    // --- could not use single io. continue with DEVi io operation
    //
    DBG3 (( MF__, "[T:0x%03u] Continuing with DEVi io operation",
            pUKT->pCTask->ulTaskIndex ));
    }
  else if ( kgs.ulNumOfDevWorker != 0 )
  {
      pUKT->lWaitForDeviIO++;

      IORet = sql54k_overlapped_io( &pUKT, devno, devpno, op_code,
          aSegmentArray, pageCount, errtext );

      pUKT->lWaitForDeviIO--;

      if ( IORet != IO_Continue )
      {
          teo52_IOStatistic *pUKT_SelfIOStat;

          if ( fIsReading )
          {
            pIOStat = &pTaskCtrl->TaskStateStat.DevIOReadStat;
            pUKT_SelfIOStat   = &(pUKT->pSingleIO[devno].ReadSelfIo);
          }
          else
          {
            pIOStat = &pTaskCtrl->TaskStateStat.DevIOWriteStat;
            pUKT_SelfIOStat   = &(pUKT->pSingleIO[devno].WriteSelfIo);
          }

          pIOStat->IOStat.TotalTimeStat.ulCount++;
          pIOStat->TotalPageCount+= pageCount;
          pIOStat->IOStat.TimeStat.ulCount++;
          pIOStat->ulPageCount+= pageCount;

          os54_UpdateCountInIOStatisticRec(pUKT_SelfIOStat, pageCount);

          if ( pTaskCtrl->TimeCollectionEnabled )
              eo52UpdateTaskStateStatisticRec( &pIOStat->IOStat, &pTaskCtrl->TaskStateStat );

          if ( IORet == IO_NoError )
              *ok = true;
          else
              *ok = false;

          // --- increase io counter
          __sql54k_io_finisched(pTaskCtrl);
          return;
      }
  }

  //
  // --- send a devi request
  //
  pDeviCtrl = sql54k_find_devi ( devno, &ulDummy );
  pRequest  = GetTaskDLQElement1( pTaskCtrl );

  pRequest->pTaskCtrl                        = pTaskCtrl;
  pRequest->ulReqType                        = REQ_VBLOCKIO;
  pRequest->ReqArgs.IORequest.lDevspaceNo    = devno;
  pRequest->ReqArgs.IORequest.lDevspacePage  = devpno;
  pRequest->ReqArgs.IORequest.OpCode         = op_code;
  pRequest->ReqArgs.IORequest.aSegmentArray  = aSegmentArray;
  pRequest->ReqArgs.IORequest.sNumOfPages    = pageCount;
  pRequest->ReqArgs.IORequest.fOk            = false;

  DBG3(( MF__,
          "[T:0x%03u] Send pRequest: 0x%x, using (Devi TID:%u) pIOReqQue: 0x%x",
          pTaskCtrl->ulTaskIndex, pRequest, pDeviCtrl->ThrdCtrlHeader.Tid,
          pDeviCtrl->pIOReqQue ));

  pUKT->lWaitForDeviIO++;
  REQ_TO_DEVI( pDeviCtrl, pRequest );

  if ( fIsReading )
    pIOStat = &pTaskCtrl->TaskStateStat.DevIOReadStat;
  else
    pIOStat = &pTaskCtrl->TaskStateStat.DevIOWriteStat;

  pIOStat->IOStat.TotalTimeStat.ulCount++;
  pIOStat->TotalPageCount += pageCount;
  pIOStat->IOStat.TimeStat.ulCount++;
  pIOStat->ulPageCount += pageCount;

  // --- goto dispatcher
  GOTO_DISP(&pUKT);

  if ( pTaskCtrl->TimeCollectionEnabled )
  {
      eo52UpdateTaskStateStatisticRec( &pIOStat->IOStat, &pTaskCtrl->TaskStateStat );
  }

  pUKT->lWaitForDeviIO--;


  if ( pUKT->pCurrReq != pRequest )
    {
    MSGALL(( ERR_UNKNOWN_REQUEST ));
    DBG1  (( MF__, "[T:0x%03u] pRequest: 0x%x (SEND) != pUKT->pCurrReq: 0x%x (REPLY)",
            pTaskCtrl->ulTaskIndex, pRequest, pUKT->pCurrReq ));
    ABORT();
    }

  *ok = pRequest->ReqArgs.IORequest.fOk;
  DBG3 (( MF__, "[T:0x%03u] Returned from devi_io, devpno: %u",
          pTaskCtrl->ulTaskIndex, devpno));


  if ( !*ok )
    {
    if ( pRequest->ReqArgs.IORequest.ulErrCode < MAX_DEVIO_ERRCODE )
      {
      sql46c_build_error_string ( errtext,
                                  ErrTab[ pRequest->ReqArgs.IORequest.ulErrCode ], 0 );
      MSGD (( ERR_ERROR_ON_DEVSPACE, kgs.Dev0.pIOCtrl[devno].szDevspace, devno));
      DBG1 (( MF__, "[T:0x%03u] pRequest: 0x%x, %s",
              pTaskCtrl->ulTaskIndex, pRequest,
              ErrTab[ pRequest->ReqArgs.IORequest.ulErrCode ] ));
      }
    }
  else
    {
    DBG3 (( MF__, "[T:0x%03u] Returned OK, pRequest: 0x%x",
            pTaskCtrl->ulTaskIndex, pRequest ));
    }

  #ifdef DEVELOPMENT
   pTaskCtrl->ulWaitTime = 0;
  #endif

  // --- increase io counter
  __sql54k_io_finisched(pTaskCtrl);

  DBGOUT_T (pTaskCtrl->ulTaskIndex);

  return;
  }

/*------------------------------*/

void vvectorio ( tsp00_Int4                  devno,
                 tsp00_PageNo                devpno,
                 tsp00_PageAddr              pageVector[],
                 tsp00_Int2                  pageCount,
                 tsp00_TaskId                taskId,
                 tsp00_ErrText VAR_ARRAY_REF errtext,
                 tsp00_Bool    VAR_VALUE_REF ok )
{
# undef  MF__
# define MF__ MOD__"vvectorio"

  tsp2_io_op_code_Param          op_code        = DEV_WRITE; // --- currently the only write is allowed

  PDLQ_REC                       pRequest;
  PDEVI_CTRL_REC                 pDeviCtrl;
  ULONG                          ulDummy;
  IORetCode                      IORet;
  #ifdef DEVELOP_CHECKS
   PUKT_CTRL_REC                 pUKT           = THIS_UKT_CTRL;
   PTASK_CTRL_REC                pTaskCtrl      = pUKT->pCTask;
  #else
   PTASK_CTRL_REC                pTaskCtrl      = &kgs.pFirstTaskCtrl[taskId - 1];
   PUKT_CTRL_REC                 pUKT           = pTaskCtrl->pUKT;
  #endif
  BOOLEAN                        fIsReading     = op_code == DEV_READ;
  teo52_TaskDevIOStatistic      *pIOStat;
  BOOL                           fSingleIO = false;
  PDEV_IO_CTRL_REC               pIOCtrl;
  FILE_SEGMENT_ELEMENT          *pSegmentArray;

  DBGIN_T (pTaskCtrl->ulTaskIndex);

  pTaskCtrl->TaskState = TSK_VVECTORIO_EO00;

  #ifdef DEVELOP_CHECKS
   if ( (ULONG)taskId != pTaskCtrl->ulTaskIndex )
     {
     MSGD (( ERR_VXXXX_WRONG_TASK, "vblockio", taskId ));
     ABORT();
     }
  #endif

  DBG3 (( MF__, "[T:0x%03u] devno: %u,  devpno: %u  op_code: %u",
          pTaskCtrl->ulTaskIndex, devno, devpno, op_code ));

  if ( devno < 0 || devno >= XPARAM(lMaxDevspaces) )
    {
    MSGALL(( ERR_INVALID_DEVNO ));
    sql46c_build_error_string ( errtext, ERRMSG_DEVIO_INVALID_DEVNO, 0 );
    *ok = false;
    return;
    }

  // --- devspace not attached correctly?
  pIOCtrl = &kgs.Dev0.pIOCtrl[devno] ;

  if ( pIOCtrl->ulAttached      == 0 ||
       pIOCtrl->ulDetachPending != 0 )
    {
    MSGD (( ERR_DEVSPACE_NOT_ATT ));
    sql46c_build_error_string ( errtext, ERRMSG_DEVIO_NOT_ATT, 0 );
    *ok = false;

    pTaskCtrl->TaskState = TSK_RUNNING_EO00;
    DBGOUT_T (pTaskCtrl->ulTaskIndex);
    return;
    }

  if (( pTaskCtrl->TaskType   != TT_DW_EO00 ) &&
      ( pIOCtrl->DevspaceType == sp2dt_data ))
    {
       if ( ++(kgs.TotalDataIOCnt) >= 0x7fffffff )
          kgs.TotalDataIOCnt = 0;
    }

  #ifdef DEVELOPMENT
   pTaskCtrl->ulWaitTime = kgs.ulCurrTime;
  #endif

  // ---  simulate an I/O error for test porposes ?
  if ( __sql54k_is_user_req_io_error((long )devno, op_code) )
    {
    sql46c_build_error_string ( errtext, ERRMSG_DEVIO_USER_REQ_ERROR, 0 );
    *ok = false;
    MSGD ((ERR_TEST_IO_ERROR,
           fIsReading ? "reading from" : "writing to", devno, devpno ));

    pTaskCtrl->TaskState = TSK_RUNNING_EO00;
    DBGOUT_T (pTaskCtrl->ulTaskIndex);
    return;
    }

  pSegmentArray                = BUILD_FILE_SEGMENT( pageVector, PAGES_TO_BYTES(pageCount) );
  pTaskCtrl->lWaitForDevno     = devno;
  pTaskCtrl->DevNameWaitingFor = pIOCtrl->szDevspace;

  if ( kgs.ulNumOfDevWorker != 0 )
  {
      pUKT->lWaitForDeviIO++;

      // --- do a overlapped io
      IORet = sql54k_overlapped_io( &pUKT, devno, devpno, op_code,
          pSegmentArray, pageCount, errtext );

      pUKT->lWaitForDeviIO--;

      if ( IORet != IO_Continue )
      {
          teo52_IOStatistic *pUKT_SelfIOStat;

          if ( fIsReading )
          {
            pIOStat = &pTaskCtrl->TaskStateStat.DevIOReadStat;
            pUKT_SelfIOStat   = &(pUKT->pSingleIO[devno].ReadSelfIo);
          }
          else
          {
            pIOStat = &pTaskCtrl->TaskStateStat.DevIOWriteStat;
            pUKT_SelfIOStat   = &(pUKT->pSingleIO[devno].WriteSelfIo);
          }

          pIOStat->IOStat.TotalTimeStat.ulCount++;
          pIOStat->TotalPageCount+= pageCount;
          pIOStat->IOStat.TimeStat.ulCount++;
          pIOStat->ulPageCount+= pageCount;

          os54_UpdateCountInIOStatisticRec(pUKT_SelfIOStat, pageCount);

          if ( pTaskCtrl->TimeCollectionEnabled )
          {
              eo52UpdateTaskStateStatisticRec( &pIOStat->IOStat, &pTaskCtrl->TaskStateStat );
          }

          if ( IORet == IO_NoError )
              *ok = true;
          else
              *ok = false;

          // --- increase io counter
          __sql54k_io_finisched(pTaskCtrl);
          return;
      }
  }

  //
  // --- send a devi request
  //
  pDeviCtrl = sql54k_find_devi ( devno, &ulDummy );
  pRequest  = GetTaskDLQElement1( pTaskCtrl );

  pRequest->pTaskCtrl                        = pTaskCtrl;
  pRequest->ulReqType                        = REQ_VBLOCKIO;
  pRequest->ReqArgs.IORequest.lDevspaceNo    = devno;
  pRequest->ReqArgs.IORequest.lDevspacePage  = devpno;
  pRequest->ReqArgs.IORequest.OpCode         = op_code;
  pRequest->ReqArgs.IORequest.aSegmentArray  = pSegmentArray;
  pRequest->ReqArgs.IORequest.sNumOfPages    = pageCount;
  pRequest->ReqArgs.IORequest.fOk            = false;

  DBG3(( MF__,
              "[T:0x%03u] Send pRequest: 0x%x, using (Devi TID:%u) pIOReqQue: 0x%x",
          pTaskCtrl->ulTaskIndex, pRequest, pDeviCtrl->ThrdCtrlHeader.Tid,
          pDeviCtrl->pIOReqQue ));

  pUKT->lWaitForDeviIO++;
  REQ_TO_DEVI( pDeviCtrl, pRequest );

  if ( fIsReading )
    pIOStat = &pTaskCtrl->TaskStateStat.DevIOReadStat;
  else
    pIOStat = &pTaskCtrl->TaskStateStat.DevIOWriteStat;

  pIOStat->IOStat.TotalTimeStat.ulCount++;
  pIOStat->TotalPageCount += pageCount;
  pIOStat->IOStat.TimeStat.ulCount++;
  pIOStat->ulPageCount += pageCount;

  // --- goto dispatcher
  GOTO_DISP(&pUKT);
  
  if ( pTaskCtrl->TimeCollectionEnabled )
  {
      eo52UpdateTaskStateStatisticRec( &pIOStat->IOStat, &pTaskCtrl->TaskStateStat );
  }

  pUKT->lWaitForDeviIO--;


  if ( pUKT->pCurrReq != pRequest )
    {
    MSGALL(( ERR_UNKNOWN_REQUEST ));
    DBG1  (( MF__, "[T:0x%03u] pRequest: 0x%x (SEND) != pUKT->pCurrReq: 0x%x (REPLY)",
            pTaskCtrl->ulTaskIndex, pRequest, pUKT->pCurrReq ));
    ABORT();
    }

  *ok = pRequest->ReqArgs.IORequest.fOk;
  DBG3 (( MF__, "[T:0x%03u] Returned from devi_io, devpno: %u",
          pTaskCtrl->ulTaskIndex, devpno));


  if ( !*ok )
    {
    if ( pRequest->ReqArgs.IORequest.ulErrCode < MAX_DEVIO_ERRCODE )
      {
      sql46c_build_error_string ( errtext,
                                  ErrTab[ pRequest->ReqArgs.IORequest.ulErrCode ], 0 );
      MSGD (( ERR_ERROR_ON_DEVSPACE, kgs.Dev0.pIOCtrl[devno].szDevspace, devno));
      DBG1 (( MF__, "[T:0x%03u] pRequest: 0x%x, %s",
              pTaskCtrl->ulTaskIndex, pRequest,
              ErrTab[ pRequest->ReqArgs.IORequest.ulErrCode ] ));
      }
    }
  else
    {
    DBG3 (( MF__, "[T:0x%03u] Returned OK, pRequest: 0x%x",
            pTaskCtrl->ulTaskIndex, pRequest ));
    }

  #ifdef DEVELOPMENT
   pTaskCtrl->ulWaitTime = 0;
  #endif

  // --- increase io counter
  __sql54k_io_finisched(pTaskCtrl);

  DBGOUT_T (pTaskCtrl->ulTaskIndex);
  return;
}

/*------------------------------*/

void vdualvectorio ( tsp00_Int4                  devno1,
                     tsp00_Int4                  devno2,
                     tsp00_PageNo                devpno,
                     tsp00_PageAddr              pageVector[],
                     tsp00_Int2                  pageCount,
                     tsp00_TaskId                taskId,
                     tsp00_ErrText VAR_ARRAY_REF errtext1,
                     tsp00_Bool    VAR_VALUE_REF ok1,
                     tsp00_ErrText VAR_ARRAY_REF errtext2,
                     tsp00_Bool    VAR_VALUE_REF ok2 )
{
# undef  MF__
# define MF__ MOD__"vdualvectorio"

  PDLQ_REC                       pRequest1;
  PDLQ_REC                       pRequest2;
  PDEVI_CTRL_REC                 pDeviCtrl1;
  PDEVI_CTRL_REC                 pDeviCtrl2;
  ULONG                          ulDummy;
  IORetCode                      IORet;
  #ifdef DEVELOP_CHECKS
   PUKT_CTRL_REC                 pUKT           = THIS_UKT_CTRL;
   PTASK_CTRL_REC                pTaskCtrl      = pUKT->pCTask;
  #else
   PTASK_CTRL_REC                pTaskCtrl      = &kgs.pFirstTaskCtrl[taskId - 1];
   PUKT_CTRL_REC                 pUKT           = pTaskCtrl->pUKT;
  #endif
  teo52_TaskDevIOStatistic      *pIOStat;
  BOOL                           fSingleIO = false;
  PDEV_IO_CTRL_REC               pIOCtrl;
  FILE_SEGMENT_ELEMENT          *pSegmentArray;

  DBGIN_T (pTaskCtrl->ulTaskIndex);

  pTaskCtrl->TaskState = TSK_VDUALVECTORIO_EO00;

  *ok1 = *ok2 = true;

  #ifdef DEVELOP_CHECKS
   if ( (ULONG)taskId != pTaskCtrl->ulTaskIndex )
     {
     MSGD (( ERR_VXXXX_WRONG_TASK, "vblockio", taskId ));
     ABORT();
     }
  #endif

  DBG3 (( MF__, "[T:0x%03u] devno1: %u, devno2: %u devno devpno: %u  op_code: %u",
          pTaskCtrl->ulTaskIndex, devno1, devno2, devpno, DEV_WRITE ));

  if ( devno1 < 0 || devno1 >= XPARAM(lMaxDevspaces) )
    {
    MSGALL(( ERR_INVALID_DEVNO ));
    sql46c_build_error_string ( errtext1, ERRMSG_DEVIO_INVALID_DEVNO, 0 );
    *ok1 = false;
    }

  if ( devno2 < 0 || devno2 >= XPARAM(lMaxDevspaces) )
    {
    MSGALL(( ERR_INVALID_DEVNO ));
    sql46c_build_error_string ( errtext2, ERRMSG_DEVIO_INVALID_DEVNO, 0 );
    *ok2 = false;
    }

  if ( !*ok1 || !*ok2 )
  {
    pTaskCtrl->TaskState = TSK_RUNNING_EO00;
    DBGOUT_T (pTaskCtrl->ulTaskIndex);
    return;
  }

  // --- devspace not attached correctly?
  pIOCtrl = &kgs.Dev0.pIOCtrl[devno1] ;

  if ( pIOCtrl->ulAttached      == 0 ||
       pIOCtrl->ulDetachPending != 0 )
    {
    MSGD (( ERR_DEVSPACE_NOT_ATT ));
    sql46c_build_error_string ( errtext1, ERRMSG_DEVIO_NOT_ATT, 0 );
    *ok1 = false;
    }
  else if (( pTaskCtrl->TaskType   != TT_DW_EO00 ) &&
      ( pIOCtrl->DevspaceType == sp2dt_data ))
    {
       if ( ++(kgs.TotalDataIOCnt) >= 0x7fffffff )
          kgs.TotalDataIOCnt = 0;
    }


  // --- devspace not attached correctly?
  pIOCtrl = &kgs.Dev0.pIOCtrl[devno2] ;

  if ( pIOCtrl->ulAttached      == 0 ||
       pIOCtrl->ulDetachPending != 0 )
  {
    MSGD (( ERR_DEVSPACE_NOT_ATT ));
    sql46c_build_error_string ( errtext2, ERRMSG_DEVIO_NOT_ATT, 0 );
    *ok2 = false;
  }
  else if (( pTaskCtrl->TaskType   != TT_DW_EO00 ) &&
      ( pIOCtrl->DevspaceType == sp2dt_data ) )
    {
       if ( ++(kgs.TotalDataIOCnt) >= 0x7fffffff )
          kgs.TotalDataIOCnt = 0;
    }


  if ( !*ok1 || !*ok2 )
  {
    pTaskCtrl->TaskState = TSK_RUNNING_EO00;
    DBGOUT_T (pTaskCtrl->ulTaskIndex);
    return;
  }

  #ifdef DEVELOPMENT
   pTaskCtrl->ulWaitTime = kgs.ulCurrTime;
  #endif

  // ---  simulate an I/O error for test porposes ?
  if ( __sql54k_is_user_req_io_error((long )devno1, DEV_WRITE) )
  {
    sql46c_build_error_string ( errtext1, ERRMSG_DEVIO_USER_REQ_ERROR, 0 );
    *ok1 = false;
    MSGD ((ERR_TEST_IO_ERROR,
           "writing to", devno1, devpno ));
  }

  // ---  simulate an I/O error for test porposes ?
  if ( __sql54k_is_user_req_io_error((long )devno2, DEV_WRITE) )
  {
    sql46c_build_error_string ( errtext2, ERRMSG_DEVIO_USER_REQ_ERROR, 0 );
    *ok2 = false;
    MSGD ((ERR_TEST_IO_ERROR,
           "writing to", devno2, devpno ));
  }

  if ( !*ok1 || !*ok2 )
  {
    pTaskCtrl->TaskState = TSK_RUNNING_EO00;
    DBGOUT_T (pTaskCtrl->ulTaskIndex);
    return;
  }

  pSegmentArray                = BUILD_FILE_SEGMENT( pageVector, PAGES_TO_BYTES(pageCount) );
  pTaskCtrl->lWaitForDevno     = devno2;
  pTaskCtrl->DevNameWaitingFor = kgs.Dev0.pIOCtrl[devno2].szDevspace;

  if ( kgs.ulNumOfDevWorker != 0 )
  {
    pUKT->lWaitForDeviIO++;

    // --- do a overlapped io on two devices
    IORet = sql54k_overlapped_dual_write_io( &pUKT, devno1, devno2, devpno,
                                             pSegmentArray, pageCount, errtext1, errtext2, ok1, ok2 );
    pUKT->lWaitForDeviIO--;

    if ( IORet != IO_Continue )
    {
        pIOStat = &pTaskCtrl->TaskStateStat.DevIOWriteStat;

        pIOStat->IOStat.TotalTimeStat.ulCount++;
        pIOStat->TotalPageCount += 2*pageCount ;
        pIOStat->IOStat.TimeStat.ulCount++;
        pIOStat->ulPageCount += 2*pageCount ;

        if ( pTaskCtrl->TimeCollectionEnabled )
        {
            eo52UpdateTaskStateStatisticRec( &pIOStat->IOStat, &pTaskCtrl->TaskStateStat );
        }

      __sql54k_io_finisched(pTaskCtrl);
      return;
    }
  }

  //
  // --- send a devi request
  //
  pDeviCtrl1 = sql54k_find_devi ( devno1, &ulDummy );

  pRequest1  = GetTaskDLQElement1( pTaskCtrl );

  pRequest1->pTaskCtrl                        = pTaskCtrl;
  pRequest1->ulReqType                        = REQ_VBLOCKIO;
  pRequest1->ReqArgs.IORequest.lDevspaceNo    = devno1;
  pRequest1->ReqArgs.IORequest.lDevspacePage  = devpno;
  pRequest1->ReqArgs.IORequest.OpCode         = DEV_WRITE;
  pRequest1->ReqArgs.IORequest.aSegmentArray  = pSegmentArray;
  pRequest1->ReqArgs.IORequest.sNumOfPages    = pageCount;
  pRequest1->ReqArgs.IORequest.fOk            = false;

  DBG3(( MF__,
          "[T:0x%03u] Send pRequest: 0x%x, using (Devi1 TID:%u) pIOReqQue1: 0x%x",
          pTaskCtrl->ulTaskIndex, 
          pRequest1, 
          pDeviCtrl1->ThrdCtrlHeader.Tid,
          pDeviCtrl1->pIOReqQue ));

  pDeviCtrl2 = sql54k_find_devi ( devno2, &ulDummy );

  pRequest2  = GetTaskDLQElement2( pTaskCtrl );

  pRequest2->pTaskCtrl                        = pTaskCtrl;
  pRequest2->ulReqType                        = REQ_VBLOCKIO;
  pRequest2->ReqArgs.IORequest.lDevspaceNo    = devno2;
  pRequest2->ReqArgs.IORequest.lDevspacePage  = devpno;
  pRequest2->ReqArgs.IORequest.OpCode         = DEV_WRITE;
  pRequest2->ReqArgs.IORequest.aSegmentArray  = pSegmentArray;
  pRequest2->ReqArgs.IORequest.sNumOfPages    = pageCount;
  pRequest2->ReqArgs.IORequest.fOk            = false;

  DBG3(( MF__,
          "[T:0x%03u] Send pRequest: 0x%x, using (Devi2 TID:%u) pIOReqQue2: 0x%x",
          pTaskCtrl->ulTaskIndex, 
          pRequest1, 
          pDeviCtrl2->ThrdCtrlHeader.Tid,
          pDeviCtrl2->pIOReqQue ));

  pUKT->lWaitForDeviIO++;

  /* Two simultanous requests are on their way, dispatcher knows to decrement
     this counter and wakeup only if both are completed */
  REQ_TO_DEVI( pDeviCtrl1, pRequest1 );
  REQ_TO_DEVI( pDeviCtrl2, pRequest2 );

  pIOStat = &pTaskCtrl->TaskStateStat.DevIOWriteStat;

  pIOStat->IOStat.TotalTimeStat.ulCount++;
  pIOStat->TotalPageCount += 2*pageCount;
  pIOStat->IOStat.TimeStat.ulCount++;
  pIOStat->ulPageCount += 2*pageCount;

  // --- goto dispatcher
  GOTO_DISP(&pUKT);
  
  if ( pUKT->pCurrReq != pRequest1
    && pUKT->pCurrReq != pRequest2 )
    {
    MSGALL(( ERR_UNKNOWN_REQUEST ));
    DBG1  (( MF__, "[T:0x%03u] pRequest: 0x%x|0x%x (SEND) != pUKT->pCurrReq: 0x%x (REPLY)",
            pTaskCtrl->ulTaskIndex, pRequest1, pRequest2, pUKT->pCurrReq ));
    ABORT();
    }

  GOTO_DISP(&pUKT);

  if ( pUKT->pCurrReq != pRequest1
    && pUKT->pCurrReq != pRequest2 )
    {
    MSGALL(( ERR_UNKNOWN_REQUEST ));
    DBG1  (( MF__, "[T:0x%03u] pRequest: 0x%x|0x%x (SEND) != pUKT->pCurrReq: 0x%x (REPLY)",
            pTaskCtrl->ulTaskIndex, pRequest1, pRequest2, pUKT->pCurrReq ));
    ABORT();
    }

  if ( pTaskCtrl->TimeCollectionEnabled )
  {
      eo52UpdateTaskStateStatisticRec( &pIOStat->IOStat, &pTaskCtrl->TaskStateStat );
  }

  pUKT->lWaitForDeviIO--;

  DBG3 (( MF__, "[T:0x%03u] Returned from devi_io, devpno: %u",
          pTaskCtrl->ulTaskIndex, devpno));

  *ok1 = pRequest1->ReqArgs.IORequest.fOk;
  if ( !*ok1 )
  {
    if ( pRequest1->ReqArgs.IORequest.ulErrCode < MAX_DEVIO_ERRCODE )
      {
      sql46c_build_error_string ( errtext1,
                                  ErrTab[ pRequest1->ReqArgs.IORequest.ulErrCode ], 0 );
      MSGD (( ERR_ERROR_ON_DEVSPACE, kgs.Dev0.pIOCtrl[devno1].szDevspace, devno1));
      DBG1 (( MF__, "[T:0x%03u] pRequest: 0x%x, %s",
              pTaskCtrl->ulTaskIndex, pRequest1,
              ErrTab[ pRequest->ReqArgs.IORequest.ulErrCode ] ));
      }
  }

  *ok2 = pRequest2->ReqArgs.IORequest.fOk;
  if ( !*ok2 )
  {
    if ( pRequest2->ReqArgs.IORequest.ulErrCode < MAX_DEVIO_ERRCODE )
      {
      sql46c_build_error_string ( errtext2,
                                  ErrTab[ pRequest2->ReqArgs.IORequest.ulErrCode ], 0 );
      MSGD (( ERR_ERROR_ON_DEVSPACE, kgs.Dev0.pIOCtrl[devno2].szDevspace, devno2));
      DBG1 (( MF__, "[T:0x%03u] pRequest: 0x%x, %s",
              pTaskCtrl->ulTaskIndex, pRequest2,
              ErrTab[ pRequest->ReqArgs.IORequest.ulErrCode ] ));
      }
  }

  if ( *ok1 && *ok2 )
    {
    DBG3 (( MF__, "[T:0x%03u] Returned OK, pRequest1: 0x%x pRequest2:0x%x",
            pTaskCtrl->ulTaskIndex, pRequest1, pRequest2 ));
    }

  #ifdef DEVELOPMENT
   pTaskCtrl->ulWaitTime = 0;
  #endif

  // --- increase io counter
  __sql54k_io_finisched(pTaskCtrl);

  DBGOUT_T (pTaskCtrl->ulTaskIndex);

  return;
}

/*------------------------------*/

void vcopyvolume ( tsp00_Int4                  devnoSource,
                   tsp00_Int4                  devnoDestination,
                   tsp00_PageNo                firstPage,
                   tsp00_Int4                  pageCount,
                   tsp00_TaskId                taskId,
                   tsp00_ErrText VAR_ARRAY_REF errtext,
                   tsp00_Bool    VAR_VALUE_REF ok )
{
# undef  MF__
# define MF__ MOD__"vcopyvolume"

    tsp00_Int4 ulBuffPages;
    BUFFERADDRESS  pBuffer;
    LONG       rc           = NO_ERROR;
    tsp00_PageNo currentPage = firstPage;
    tsp00_Int4   pagesLeft   = pageCount;

    if ( pagesLeft <= 0 )
    {
        *ok = true;
        DBGOUT;
        return;
    }

    ulBuffPages = (32 KB) / XPARAM(ulDBPageSize);

    if ( ulBuffPages == 0 ) ulBuffPages = 1;

    rc = ALLOC_MEM ((PPVOID)&pBuffer,
                    (XPARAM(ulDBPageSize) * ulBuffPages) +
                    kgs.ulSystemPageSize /* alignment */ );

    if( rc != NO_ERROR )
    {
      MSGD(( ERR_ALLOC_MEMORY__FILE_LINE__, rc, __FILE__, __LINE__ ));
      sql46c_build_error_string ( errtext, ERRMSG_DEVIO_ALLOC_MEM, rc );
      *ok = false;
      DBGOUT;
      return;
    }

    while ( pagesLeft > 0 )
    {
        while ( pagesLeft >= ulBuffPages )
        {
            vblockio( devnoSource,
                      currentPage,
                      DEV_READ,
                      pBuffer,
                      (tsp00_Int2)ulBuffPages,
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
                      DEV_WRITE,
                      pBuffer,
                      (tsp00_Int2)ulBuffPages,
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
                      DEV_READ,
                      pBuffer,
                      (tsp00_Int2)pagesLeft,
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
                      DEV_WRITE,
                      pBuffer,
                      (tsp00_Int2)pagesLeft,
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

    rc = FREE_MEM ( (PVOID)pBuffer );

    if( rc != NO_ERROR )
    {
        MSGD(( ERR_ALLOC_MEMORY__FILE_LINE__, rc, __FILE__, __LINE__ ));
        sql46c_build_error_string ( errtext, ERRMSG_DEVIO_ALLOC_MEM, rc );
        *ok = false;
    }
    DBGOUT;
    return;
}
                 
/*------------------------------*/

VOID vdetach ( INT4        devno,
               PROCESS_ID  taskId )
  {
# undef  MF__
# define MF__ MOD__"vdetach"
  PDLQ_REC                       pRequest;
  tsp2_devnamec                  szLogDevName;
  #ifdef DEVELOP_CHECKS
   PUKT_CTRL_REC                 pUKT      = THIS_UKT_CTRL;
   PTASK_CTRL_REC                pTaskCtrl = pUKT->pCTask;
  #else
   PTASK_CTRL_REC                pTaskCtrl = &kgs.pFirstTaskCtrl[taskId - 1];
   PUKT_CTRL_REC                 pUKT      = pTaskCtrl->pUKT;
  #endif

  DBGIN_T (pTaskCtrl->ulTaskIndex);

  #ifdef DEVELOP_CHECKS
   if ( (ULONG)taskId != pTaskCtrl->ulTaskIndex )
     {
     MSGD (( ERR_VXXXX_WRONG_TASK, "vdetach", taskId ));
     ABORT();
     }
  #endif

  if ( kgs.Dev0.ThrdCtrlHeader.ThrdState == KT_INITIALIZED )
    return;


  strcpy ( szLogDevName, kgs.Dev0.pIOCtrl[devno].szDevspace );

  MSGD (( INFO_DETACHING_DEVSPACE, szLogDevName ));

  if ( devno < 0 || devno > XPARAM(lMaxDevspaces) )
    {
    MSGALL(( ERR_WRONG_DEVSPACE_NO, devno ));
    DBG1  (( MF__, "[T:0x%03u] Wrong devspace number: %d",
             pTaskCtrl->ulTaskIndex, devno ));
    return;
    }

  pTaskCtrl->TaskState = TSK_VDETACH_EO00;

  pRequest                                     = GetTaskDLQElement1( pTaskCtrl );
  pRequest->pTaskCtrl                          = pTaskCtrl;
  pRequest->ulReqType                          = REQ_VDETACH;
  pRequest->ReqArgs.VdetachRequest.lDevspaceNo = devno;

  // --- send request to DEV0 thread
  REQ_TO_DEV0( pRequest, pTaskCtrl );

  // --- goto dispatcher
  GOTO_DISP (&pUKT);

  // --- wrong reply?
  if ( pUKT->pCurrReq != pRequest )
    {
    MSGALL(( ERR_UNKNOWN_REQUEST ));
    DBG1  (( MF__, "[T:0x%03u] pRequest: 0x%x (SEND) != pUKT->pCurrReq: 0x%x (REPLY)",
             pTaskCtrl->ulTaskIndex, pRequest, pUKT->pCurrReq ));
    ABORT();
    }

  pTaskCtrl->TaskState = TSK_RUNNING_EO00;

  DBGOUT_T (pTaskCtrl->ulTaskIndex);
  return;
  }

/*------------------------------*/

VOID _System sql54k_dev0 ( VOID )
  {
# undef  MF__
# define MF__ MOD__"sql54k_dev0"
  LONG                           rc = NO_ERROR;
  PDLQ_REC                       pRequest;
  LONG                           lDummy = 0;

  DBGIN;

  __try
  {
#   if defined(_FASTCAP)
     CAPNameThread ("Dev0");
#   endif

    // --- Create event semaphore (SET) for DEV0
    rc = sql41c_create_event_sem (&kgs.Dev0.ThrdCtrlHeader.hevSem,
                                  NULL,
                                  NULL,
                                  0,
                                  false,
                                  NULL);
    if (rc != NO_ERROR)
      ABORT();

    // --- set the thread priority
    rc = SET_THREAD_PRIO ( kgs.Dev0.ThrdCtrlHeader.lThrdPrio,
                           kgs.Dev0.ThrdCtrlHeader.Tid,
                           kgs.Dev0.ThrdCtrlHeader.hThrd );

    if ( rc != NO_ERROR )
      {
      MSGD (( ERR_SETTING_THRD_PRIO, kgs.Dev0.ThrdCtrlHeader.Tid));
      DBG1 ((MF__, "Cannot set the thread priority, TID: %u",
                    kgs.Dev0.ThrdCtrlHeader.Tid));
      ABORT();
      }

    // --- set thread control info
    kgs.Dev0.ThrdCtrlHeader.ThrdState   = KT_RUNNING;
    kgs.Dev0.ThrdCtrlHeader.ulStartTime = kgs.ulCurrTime;

    // --- Runs forever
    for (;; )
      {
      DBG3 (( MF__, "kgs.Dev0.pIOWorkQue           is 0x%x", kgs.Dev0.pIOWorkQue ));
      DBG3 (( MF__, "kgs.Dev0.pIOReqQue            is 0x%x", kgs.Dev0.pIOReqQue ));
      DBG3 (( MF__, "kgs.Dev0.pIOWorkQue->pRequest is 0x%x", kgs.Dev0.pIOWorkQue->pRequest ));

      /*
       *  Wait for a request to DEV0
       */
      pRequest = sql54k_dev_receive (&kgs.Dev0.pIOWorkQue,
                                     &lDummy,
                                     &kgs.Dev0.lExclusive,
                                     &kgs.Dev0.ThrdCtrlHeader,
                                     0);

      switch ( pRequest->ulReqType )
        {
        case REQ_VATTACH :
          sql54k_dev0_vattach ( &pRequest->ReqArgs.VattachRequest );
          break;

        case REQ_VDETACH :
          sql54k_dev0_vdetach ( &pRequest->ReqArgs.VdetachRequest, pRequest );
          break;

        case REQ_MARK_BAD_DEVSPACE :
          sql54k_vmark_bad_dev ( &pRequest->ReqArgs.MarkBadDevRequest );
          break;

        default          :
          MSGD (( ERR_ILLEGAL_REQUEST, "DEV0", pRequest->ulReqType ));
        }

      // -- send reply back to UKT
      REQ_FROM_DEVx_TO_UKT(pRequest);

      DBG3 (( MF__, "next in loop" ));
      }
  }
  __except( sql50k_excepthandler(GetExceptionInformation()) ) {;}

  DBGOUT;
  EXITTHREAD ( 0, kgs.Dev0.ThrdCtrlHeader.hThrd );

  return;
  }

/*------------------------------*/

VOID _System sql54k_dev_worker ( PDEV_WORKER_CTRL_REC pDevWorkerCtrl )
  {
# undef  MF__
# define MF__ MOD__"sql54k_dev_worker"
  LONG                         rc = NO_ERROR;
  BOOL                         fSuccess;
  ULONG                        ulNumOfBytes;
  LPOVERLAPPED                 pCompletedOverlapped;
  tsp00_Longint                lDevspaceNo;

  DBGIN;

  __try
  {
#   if defined(_FASTCAP)
     CAPNameThread ("DevWorker");
#   endif

    // --- Create event semaphore (SET) for DEV0
    rc = sql41c_create_event_sem (&pDevWorkerCtrl->ThrdCtrlHeader.hevSem,
                                  NULL,
                                  NULL,
                                  0,
                                  false,
                                  NULL);
    if (rc != NO_ERROR)
      ABORT();

    // --- set the thread priority
    rc = SET_THREAD_PRIO ( pDevWorkerCtrl->ThrdCtrlHeader.lThrdPrio,
                           pDevWorkerCtrl->ThrdCtrlHeader.Tid,
                           pDevWorkerCtrl->ThrdCtrlHeader.hThrd );

    if ( rc != NO_ERROR )
      {
      MSGD (( ERR_SETTING_THRD_PRIO, pDevWorkerCtrl->ThrdCtrlHeader.Tid));
      DBG1 ((MF__, "Cannot set the thread priority, TID: %u",
                    pDevWorkerCtrl->ThrdCtrlHeader.Tid));
      ABORT();
      }

    // --- set thread control info
    pDevWorkerCtrl->ThrdCtrlHeader.ThrdState   = KT_RUNNING;
    pDevWorkerCtrl->ThrdCtrlHeader.ulStartTime = kgs.ulCurrTime;

    // --- Runs forever
    for (;;)
      {
      pDevWorkerCtrl->ThrdCtrlHeader.ThrdState = KT_SLEEPING;

      fSuccess = GetQueuedCompletionStatus( kgs.hCompletionPort,
                                            &ulNumOfBytes,
                                            &lDevspaceNo,
                                            &pCompletedOverlapped,
                                            (DWORD)-1);

      pDevWorkerCtrl->ThrdCtrlHeader.ThrdState = KT_RUNNING;

      if (!fSuccess )
        {
        rc = GetLastError();

        if (pCompletedOverlapped == NULL)
          {
          MSGD (( ERR_GET_QUEUED_COMPL_STATUS, GetLastError()));
          DBG1 ((MF__, "GetQueuedCompletionStatus failed, rc = %d",
                 rc ));
          ABORT();
          }
       }
      else
        rc = NO_ERROR;

      pDevWorkerCtrl->ulIOCount++;

      REQ_FROM_DEV_WOKER_TO_UKT ( fSuccess, ulNumOfBytes,
                                  (LONG)lDevspaceNo, pCompletedOverlapped, rc );

      DBG3 (( MF__, "next comleted io" ));
      }
  }
  __except( sql50k_excepthandler(GetExceptionInformation()) ) {;}

  DBGOUT;
  EXITTHREAD ( 0, pDevWorkerCtrl->ThrdCtrlHeader.hThrd );

  return;
  }

/*------------------------------*/

//
// --- This function is called by the exception handler to
//     set free the tasks waiting for DEV0 or DEVi activities
//
VOID sql54k_dev0_devi_crash_cleanup ( TID CrashedTid )
  {
# undef  MF__
# define MF__ MOD__"sql54k_dev0_devi_crash_cleanup"
  PDEVI_CTRL_REC  pCurrDeviCtrl;
  DBGPAS;

  // --- DEV0 thread crashed
  if ( kgs.Dev0.ThrdCtrlHeader.Tid == CrashedTid )
    {
    // --- Set thread state to signal this thread is down
    kgs.Dev0.ThrdCtrlHeader.ThrdState = KT_INITIALIZED;
    sql54k_wake_up_tasks ( &kgs.Dev0.pIOWorkQue,
                           &kgs.Dev0.lExclusive );
    return;
    }

  for (pCurrDeviCtrl =  kgs.pFirstDeviCtrl;
       pCurrDeviCtrl <= kgs.pLastDeviCtrl;
       pCurrDeviCtrl++)
    {
    // --- DEVi thread crashed
    if ( pCurrDeviCtrl->ThrdCtrlHeader.Tid == CrashedTid )
      {
      // --- Set thread state to signal this thread is down
      pCurrDeviCtrl->ThrdCtrlHeader.ThrdState = KT_INITIALIZED;
      sql54k_wake_up_tasks ( &pCurrDeviCtrl->pIOWorkQue,
                             &pCurrDeviCtrl->lExclusive );
      return;
      }
    }
  return;
  }


/*===========================================================================*
 *  LOCAL FUNCTIONS (CODE)                                                   *
 *===========================================================================*/

_INLINE LONG  __sql54k_io_write ( HANDLE                  DevHandle,
                                  FILE_SEGMENT_ELEMENT    aSegmentArray[],
                                  ULONG                   ulPages,
                                  LPOVERLAPPED            pOverlapped )
{
  LONG    rc             = NO_ERROR;
  ULONG   ulLength       = 0;
  ULONG   ulLengthWanted = PAGES_TO_BYTES(ulPages);

  if ( aSegmentArray[1].Buffer == NULL ) // do we have only one page or a block of pages?
    {
    // - We can write this page(s) with a single write operation!
    rc = (WriteFile( DevHandle, aSegmentArray[0].Buffer, ulLengthWanted,
                     &ulLength, pOverlapped ) ? NO_ERROR : GetLastError());
    }
  else if ( pOverlapped == NULL ) // no overlapped io record given?
    {
    FILE_SEGMENT_ELEMENT    *pSegmentArray = aSegmentArray;

    ulLengthWanted = kgs.ulSystemPageSize;

    // - We can write a vector of pages with 'WriteFileGather' only if a overlapped io record was
    //   was given. So we must do single write operations here. This should be done in fallback situations
    //   if no overlapped io is possible.
    do
      {
      rc = (!WriteFile( DevHandle, pSegmentArray->Buffer, ulLengthWanted, &ulLength, NULL ) ?
            GetLastError() : NO_ERROR );

      if (( rc != NO_ERROR ) || ( ulLength != ulLengthWanted ))
        break;

      pSegmentArray++;

      } while (pSegmentArray->Buffer != NULL);

    }
  else // A overlapped io record was given. We can do a 'WriteFileGather'!
    {
    ulLength = ulLengthWanted;

    rc = (!WriteFileGather( DevHandle, aSegmentArray, ulLengthWanted, NULL, pOverlapped ) ?
          GetLastError() : NO_ERROR );
    }

  if (( rc == NO_ERROR ) && ( ulLength != ulLengthWanted ))
    rc = ERROR_BAD_LENGTH;

  return rc;
}

/*------------------------------*/

_INLINE LONG  __sql54k_io_read( HANDLE                  DevHandle,
                                FILE_SEGMENT_ELEMENT    aSegmentArray[],
                                ULONG                   ulPages,
                                LPOVERLAPPED            pOverlapped )
{
  LONG    rc             = NO_ERROR;
  ULONG   ulLength       = 0;
  ULONG   ulLengthWanted = PAGES_TO_BYTES(ulPages);

  if ( aSegmentArray[1].Buffer == NULL ) // do we have only one page or a block of pages?
    {
    // - We can read this page(s) with a single write operation!
    rc = (!ReadFile( DevHandle, aSegmentArray[0].Buffer, ulLengthWanted, &ulLength, pOverlapped ) ?
          GetLastError() : NO_ERROR );
    }
  else if ( pOverlapped == NULL ) // no overlapped io record given?
    {
    FILE_SEGMENT_ELEMENT    *pSegmentArray = aSegmentArray;

    ulLengthWanted = kgs.ulSystemPageSize;

    // - We can read a vector of pages with 'ReadFileScatter' only if a overlapped io record was
    //   was given. So we must do single read operations here. This should be done in fallback situations
    //   if no overlapped io is possible.
    do
      {
      rc = (!ReadFile( DevHandle, pSegmentArray->Buffer, ulLengthWanted, &ulLength, pOverlapped ) ?
            GetLastError() : NO_ERROR );

      if (( rc != NO_ERROR ) || ( ulLength != ulLengthWanted ))
        break;

      pSegmentArray++;

      } while (pSegmentArray->Buffer != NULL);
    }
  else // A overlapped io record was given. We can do a 'ReadFileScatter'!
    {
    ulLength = ulLengthWanted;

    rc = (!ReadFileScatter( DevHandle, aSegmentArray, ulLengthWanted, NULL, pOverlapped ) ?
          GetLastError() : NO_ERROR );
    }

  if (( rc == NO_ERROR ) && ( ulLength != ulLengthWanted ))
    rc = ERROR_BAD_LENGTH;

  return rc;
}

/*------------------------------*/

_INLINE LONG  __sql54k_io_read_with_retries ( HANDLE                  DevHandle,
                                              FILE_SEGMENT_ELEMENT    aSegmentArray[],
                                              SHORT                   sNumOfPages,
                                              ULONG                   lDevspacePage,
                                              PSZ                     szDevspace,
                                              ULONG                   ulRetryCnt,
                                              BOOL                    fNoErrorOutput )
{
  LONG            rc = NO_ERROR;
  ULONG           ulFilePosLow;
  ULONG           ulFilePosHigh;

  do
    {
    rc = __sql54k_io_read( DevHandle, aSegmentArray, sNumOfPages, NULL );

    if (( rc != NO_ERROR ) && ( ulRetryCnt > 1 ))
      {
      LONG Tmp_rc;

      if ( !fNoErrorOutput )
      {
          if ( sNumOfPages > 1 )
              MSGALL(( ERR_CANT_READ_PAGES_FROM_DEVSPACE, lDevspacePage, lDevspacePage + sNumOfPages - 1,  szDevspace, rc ))
          else
              MSGALL(( ERR_CANT_READ_FROM_DEVSPACE, lDevspacePage, szDevspace, rc ));
      }

      SLEEP(1000);
      PAGES_TO_QWORD( ulFilePosLow, ulFilePosHigh, lDevspacePage );

      Tmp_rc = SET_FILE_PTR ( DevHandle, ulFilePosLow, FILE_BEGIN, &ulFilePosLow, &ulFilePosHigh );

      if ( Tmp_rc != NO_ERROR )
        {
        MSGALL(( ERR_CANT_SEEK_ON_DEVSPACE, szDevspace, Tmp_rc ));
        return Tmp_rc;
        }
      }
    }
  while (( rc != NO_ERROR ) && --ulRetryCnt );

  return rc;
}

/*------------------------------*/

static IORetCode sql54k_overlapped_io( PUKT_CTRL_REC          *ppUKT,
                                       LONG                   lDevspaceNo,
                                       tsp00_PageNo            lDevspacePage,
                                       tsp2_io_op_code_Param  OpCode,
                                       FILE_SEGMENT_ELEMENT   aSegmentArray[],
                                       SHORT                  sNumOfPages,
                                       ERRORTEXT              pcErrText )
  {
# undef  MF__
# define MF__ MOD__"sql54k_overlapped_io"

  LONG                    lRetries = NUMBER_OF_IO_RETRIES;
  IORetCode               IORet;
  BOOL                    fRepeat  = TRUE;

  DBGIN_T ((*ppUKT)->pCTask->ulTaskIndex);

  do
    {
     OVERLAPPED *pOverlapped = &((*ppUKT)->pCTask->OvIO[0].Overlapped); // always use first special reqeust element

     IORet = __sql54k_read_write_overlapped ( *ppUKT, lDevspaceNo, lDevspacePage, OpCode,
                                              aSegmentArray, sNumOfPages, pOverlapped, pcErrText );

     if ( IORet == IO_Pending ) // io pending?
     {
      IORet = __sql54k_overlapped_io_wait  ( ppUKT, lDevspaceNo, lDevspacePage,
                                             OpCode, sNumOfPages, pcErrText );
     }

     if ( ( OpCode == DEV_READ ) 
          && ( IORet == IO_Error )
          && !kgs.Dev0.pIOCtrl[lDevspaceNo].ulDetachPending
          && --lRetries )
     {
          SLEEP(1000);
     }
     else
     {
          fRepeat = FALSE;
     }
    } while (fRepeat);

  DBGOUT_T ( (*ppUKT)->pCTask->ulTaskIndex );
  return IORet;
  }


/*------------------------------*/

static IORetCode sql54k_overlapped_dual_write_io( PUKT_CTRL_REC          *ppUKT,
                                                  LONG                   lDevspaceNo1,
                                                  LONG                   lDevspaceNo2,
                                                  tsp00_PageNo            lDevspacePage,
                                                  FILE_SEGMENT_ELEMENT   aSegmentArray[],
                                                  SHORT                  sNumOfPages,
                                                  ERRORTEXT              pcErrText1,
                                                  ERRORTEXT              pcErrText2,
                                                  tsp00_Bool    VAR_VALUE_REF ok1,
                                                  tsp00_Bool    VAR_VALUE_REF ok2)
{
# undef  MF__
# define MF__ MOD__"sql54k_overlapped_dual_write_io"

  IORetCode               IORet1;
  IORetCode               IORet2;

  DBGIN_T ((*ppUKT)->pCTask->ulTaskIndex);

  IORet1 = __sql54k_read_write_overlapped ( *ppUKT, lDevspaceNo1, lDevspacePage, DEV_WRITE,
                                            aSegmentArray, sNumOfPages,
                                            &((*ppUKT)->pCTask->OvIO[0].Overlapped), pcErrText1 );
  if ( IORet1 == IO_Continue )
  {
      return IO_Continue;
  }

  do
  {
      IORet2 = __sql54k_read_write_overlapped ( *ppUKT, lDevspaceNo2, lDevspacePage, DEV_WRITE,
                                                aSegmentArray, sNumOfPages,
                                                &((*ppUKT)->pCTask->OvIO[1].Overlapped), pcErrText2 );

      if ( IO_Continue == IORet2 )
        {
          // if first I/O is pending we are trapped. Second I/O cannot start due to resource problems
          // but first I/O may be holding exactly that resource.... Se we serialze and press thumb, that afterwards
          // the resource is not consumed by someone else in the system. Anyway this would only lead to a second I/O
          // at the same position, but would not collide with the overlapped I/O pending.

          if ( IO_Error == IORet1 )
          {
              *ok1 = false; // if first I/O failed, it makes no sense to retry
              return IO_Error;
          }

          if ( IORet1 == IO_Pending )
          {
              IORet1 = __sql54k_overlapped_io_wait  ( ppUKT, lDevspaceNo1, lDevspacePage,
                                                      DEV_WRITE, sNumOfPages, pcErrText1 );
          }
          else
          {
              return IO_Continue;
          }
      }
  } while ( IO_Continue == IORet2 );
  /* From now on either I/O is pending or an error occured */

  if ( IORet1 == IO_Pending && IORet2 == IO_Pending ) // both io pending -> wait for both to complete
  {
      __sql54k_overlapped_dual_io_wait  ( ppUKT, lDevspaceNo1, lDevspaceNo2, lDevspacePage,
                                          sNumOfPages, pcErrText1, pcErrText2, ok1, ok2 );
  }
  else if ( IO_Pending == IORet1 ) // only first I/O is pending, so wait for only one 
  {
      *ok1 = ( IO_NoError == __sql54k_overlapped_io_wait  ( ppUKT, lDevspaceNo1, lDevspacePage,
                                                            DEV_WRITE, sNumOfPages, pcErrText1 ) );
  }
  else if ( IO_Pending == IORet2 ) // only second I/O is pending, so wait for only one 
  {
      *ok2 = ( IO_NoError == __sql54k_overlapped_io_wait  ( ppUKT, lDevspaceNo2, lDevspacePage,
                                                            DEV_WRITE, sNumOfPages, pcErrText2 ) );
  }

  DBGOUT_T ( (*ppUKT)->pCTask->ulTaskIndex );
  return ( (*ok1 && *ok2) ? IO_NoError : IO_Error) ;
}


/*------------------------------*/

static LONG sql54k_single_io( PUKT_CTRL_REC          pUKT,
                              LONG                   lDevspaceNo,
                              tsp00_PageNo            lDevspacePage,
                              tsp2_io_op_code_Param  OpCode,
                              FILE_SEGMENT_ELEMENT   aSegmentArray[],
                              SHORT                  sNumOfPages,
                              ERRORTEXT              pcErrText )
{
# undef  MF__
# define MF__ MOD__"sql54k_single_io"
    ULONG           ulFilePosLow;
    ULONG           ulFilePosHigh;
    HANDLE          FileHandle;
    ULONG           ulTmp;
    LONG            lDevspaceType;
    LONG            rc = NO_ERROR;
    LONG            ret = IO_NoError;
    UKT_SINGLE_IO_REC *pSingleIOCtrl;

    DBGIN_T (pUKT->pCTask->ulTaskIndex);

    pSingleIOCtrl = &pUKT->pSingleIO[lDevspaceNo];
    DBG3 (( MF__, "[T:0x%03u] lDevspaceNo: %u, using FileHandle: %u",
          pUKT->pCTask->ulTaskIndex, lDevspaceNo,
          pSingleIOCtrl->SingleIOHandle ));

    WAIT_UNTIL_ALONE( &pUKT->lExclusive );
        pUKT->ulSelfIOPending = true;
        if ( pSingleIOCtrl->selfIOStatisticResetPending )
        {
            memset(&pSingleIOCtrl->ReadSelfIo.IOStat.TimeStat, 0, sizeof(pSingleIOCtrl->ReadSelfIo.IOStat.TimeStat) );
            pSingleIOCtrl->ReadSelfIo.ulPageCount = 0;
            memset(&pSingleIOCtrl->WriteSelfIo.IOStat.TimeStat, 0, sizeof(pSingleIOCtrl->WriteSelfIo.IOStat.TimeStat) );
            pSingleIOCtrl->WriteSelfIo.ulPageCount = 0;
            pSingleIOCtrl->selfIOStatisticResetPending = false;
        }
    CLEARLOCK( &pUKT->lExclusive );

    if ( kgs.Dev0.pIOCtrl[lDevspaceNo].ulDetachPending )
    {
        MSGD ((ERR_VBLOCKIO_VDETACH_PENDING, lDevspaceNo ));
        sql46c_build_error_string ( pcErrText,
            ERRMSG_DEVIO_NOT_ATT, 0 );
        ret = IO_Error;
    }
    else while ( IO_NoError == ret )
    {

        // --- file not open?
        if ( pSingleIOCtrl->SingleIOHandle == INVALID_HANDLE_VALUE )
        {
            SAPDB_Bool isForcedReadOnly;
            SAPDB_Bool isLocked = false;

            if ( kgs.fSingleIOOpenLocked == true )
            {
                DBGOUT_T ( pUKT->pCTask->ulTaskIndex );
                ret = IO_Continue;
                break;
            }
            
            if ( kgs.Dev0.pIOCtrl[lDevspaceNo].szDevspace[0] == '\0' )
            {
                MSGALL(( ERR_SINGLEIO_DEVSPACE_UNDEF, lDevspaceNo ));
                sql46c_build_error_string ( pcErrText,
                    ERRMSG_DEVIO_SINGLEIO_DEVSP_UNDEF, 0 );
                DBGOUT_T ( pUKT->pCTask->ulTaskIndex );
                ret = IO_Error;
                break;
            }

            MSGD (( INFO_SINGLEIO_ATTACH, kgs.Dev0.pIOCtrl[lDevspaceNo].szDevspace,
                    pUKT->ulUKTIndex));
            
            isForcedReadOnly = ( kgs.Dev0.pIOCtrl[lDevspaceNo].DevspaceType == sp2dt_log
                   || kgs.Dev0.pIOCtrl[lDevspaceNo].DevspaceType == sp2dt_mirr_log ) 
                   && RTEHSS_IsLogReadOnly();

            rc = sql54k_dev_open ( kgs.Dev0.pIOCtrl[lDevspaceNo].szDevspace,
                    lDevspaceNo,
                    isForcedReadOnly,
                    // PTS 1110743 --- used now on all devspaces
                    DEV_NO_BUFFERING,
                    &FileHandle,
                    &isLocked,
                    pcErrText,
                    &ulTmp,
                    &lDevspaceType);
            
            if ( rc == ERROR_TOO_MANY_OPEN_FILES )
            {
                kgs.fSingleIOOpenLocked = true;
                DBGOUT_T ( pUKT->pCTask->ulTaskIndex );
                ret = IO_Continue;
                break;
            }

            if ( rc != NO_ERROR )
            {
                DBGOUT_T ( pUKT->pCTask->ulTaskIndex );
                ret = IO_Error;
                break;
            }
            
            pSingleIOCtrl->ulOldSingleIOPos = 0;
            pSingleIOCtrl->SingleIOHandle   = FileHandle;
            pSingleIOCtrl->isLocked         = isLocked;
        }
        else /* file already open */
        {
            FileHandle = pSingleIOCtrl->SingleIOHandle;
        }
        
        //
        // --- file-seek necessary?
        //
        if ( pSingleIOCtrl->ulOldSingleIOPos != (ULONG)(lDevspacePage + 1) )
        {
            PAGES_TO_QWORD( ulFilePosLow, ulFilePosHigh, lDevspacePage + 1);
        
            rc = SET_FILE_PTR ( FileHandle, ulFilePosLow, FILE_BEGIN,
                &ulFilePosLow, &ulFilePosHigh );
        
            if (rc != NO_ERROR)
            {
                sql46c_build_error_string ( pcErrText, ERRMSG_DEVIO_SEEK_ON_DEVSPACE, rc );
                MSGALL(( ERR_CANT_SEEK_ON_DEVSPACE, kgs.Dev0.pIOCtrl[lDevspaceNo].szDevspace, rc ));
                DBG1  (( MF__, "[T:0x%03u] Cannot seek on devspace: %s, rc = %d",
                    pUKT->pCTask->ulTaskIndex, kgs.Dev0.pIOCtrl[lDevspaceNo].szDevspace, rc ));
            
                DBGOUT_T ( pUKT->pCTask->ulTaskIndex );
                ret = IO_Error;
                break;
            }
        }
    
#ifdef DEVELOP_CHECKS
        rc = __sql54k_check_filepos ( FileHandle, lDevspaceNo,
            lDevspacePage, sNumOfPages,
            kgs.Dev0.pIOCtrl[lDevspaceNo].ulDevPages,
            NULL, pcErrText );
    
        if ( rc != NO_ERROR )
        {
            DBGOUT_T ( pUKT->pCTask->ulTaskIndex );
            ret = IO_Error;
            break;
        }
#endif
    
        //
        // --- do read/write operation
        //
        if ( OpCode == DEV_READ )
        {
            rc = __sql54k_io_read_with_retries( FileHandle, aSegmentArray, sNumOfPages,
                lDevspacePage + 1, kgs.Dev0.pIOCtrl[lDevspaceNo].szDevspace,
                NUMBER_OF_IO_RETRIES, FALSE );
        
            if ( rc != NO_ERROR )
            {
                if ( sNumOfPages > 1 )
                    MSGALL(( ERR_CANT_READ_PAGES_FROM_DEVSPACE, lDevspacePage + 1, lDevspacePage + sNumOfPages,
                    kgs.Dev0.pIOCtrl[lDevspaceNo].szDevspace, rc ))
                    else
                    MSGALL(( ERR_CANT_READ_FROM_DEVSPACE, lDevspacePage + 1,
                    kgs.Dev0.pIOCtrl[lDevspaceNo].szDevspace, rc ));
                sql46c_build_error_string ( pcErrText, ERRMSG_DEVIO_READ_FROM_DEVSPACE, rc );
            
                DBGOUT_T ( pUKT->pCTask->ulTaskIndex );
                ret = IO_Error;
                break;
            }
        }
        else
        {
            rc = __sql54k_io_write( FileHandle, aSegmentArray, sNumOfPages, NULL );
        
            if ( ( rc != NO_ERROR ))
            {
                if ( sNumOfPages > 1 )
                    MSGALL(( ERR_CANT_WRITE_PAGES_ON_DEVSPACE, lDevspacePage + 1, lDevspacePage + sNumOfPages,
                    kgs.Dev0.pIOCtrl[lDevspaceNo].szDevspace, rc ))
                    else
                    MSGALL(( ERR_CANT_WRITE_ON_DEVSPACE, lDevspacePage + 1,
                    kgs.Dev0.pIOCtrl[lDevspaceNo].szDevspace, rc ));
            
                sql46c_build_error_string ( pcErrText, ERRMSG_DEVIO_WRITE_ON_DEVSPACE, rc );
                DBGOUT_T ( pUKT->pCTask->ulTaskIndex );
                ret = IO_Error;
                break;
            }
        }
    
#ifdef DEVELOP_CHECKS
        rc = __sql54k_check_filepos ( FileHandle, lDevspaceNo,
            lDevspacePage + sNumOfPages, 0,
            kgs.Dev0.pIOCtrl[lDevspaceNo].ulDevPages,
            NULL, pcErrText );
    
        if ( rc != NO_ERROR )
        {
            DBGOUT_T ( pUKT->pCTask->ulTaskIndex );
            ret = IO_Error;
            break;
        }
#endif
    
        // --- save current IO-position
        pSingleIOCtrl->ulOldSingleIOPos = lDevspacePage + sNumOfPages + 1;
    
        DBGOUT_T ( pUKT->pCTask->ulTaskIndex );
        ret = IO_NoError;
        break;

    } /* end while(IO_NoError == ret) */

    WAIT_UNTIL_ALONE( &pUKT->lExclusive );
        pUKT->ulSelfIOPending = false;
    CLEARLOCK( &pUKT->lExclusive );

    return ret;
}

/*------------------------------*/

/*
 * Vdevsize for a device being a file for direct io.
 */

static int os54_vdevsize(   PSZ            pszPhysDevName,
                            PSZ            pszLogDevName,
                            INT4           *devcapacity,
                            tsp00_TaskId   pidResponsibleForCall,
                            char           *diskImage,
                            int            isSequential,
                            int            isForcedReadOnly,
                            ULONG          ulCurrFileSizeLow,
                            ULONG          ulCurrFileSizeHigh,
                            ERRORTEXT      errtext,
                            BOOLEAN        *ok )
{
# undef  MF__
# define MF__  MOD__"os54_vdevsize"

    HANDLE         FileHandle;
    ULONG          ulFilePosLow;
    ULONG          ulFilePosHigh;
    LONG           rc    = NO_ERROR;
    bool           wantFormatDevspace = (diskImage != 0);

    DBGIN;
    DBG3 ((MF__, "devspace: %s", pszLogDevName));

    *ok = false;

    if (*devcapacity == 0 || isForcedReadOnly )
    {
        SAPDB_Int4 foundCapacity;

        DBG3 ((MF__, "Opening devspace: %s", pszLogDevName));

        FileHandle = __sql54k_create_file( pszPhysDevName,
                                           isForcedReadOnly ? (GENERIC_READ) : ( GENERIC_READ | GENERIC_WRITE ),
                                           FILE_SHARE_READ | FILE_SHARE_WRITE,
                                           NULL,
                                           OPEN_EXISTING,
                                           FILE_ATTRIBUTE_NORMAL,
                                           NULL );

        if ( FileHandle == INVALID_HANDLE_VALUE )
        {
            rc = GetLastError();

            if ( rc == ERROR_ACCESS_DENIED )
            {
                MSGALL(( ERR_DEVSPACE_ACCESS_DENIED, pszLogDevName ));
                DBG1  (( MF__, "Access on devspace '%s' denied", pszLogDevName ));
                sql46c_build_error_string ( errtext, ERRMSG_DEVIO_ACCESS_DENIED, 0 );
            }
            else
            {
                MSGALL(( ERR_CANT_OPEN_DEVSPACE, pszLogDevName, rc ));
                DBG1  (( MF__, "Cannot open devspace '%s', rc = %d", pszLogDevName, rc ));
                sql46c_build_error_string ( errtext, ERRMSG_DEVIO_OPEN_DEVSPACE, rc );
            }

            DBGOUT;
            return rc;
        }

        if (HIGH_VALUE_VALID(ulCurrFileSizeHigh) == true)
            foundCapacity = QWORD_TO_PAGES(ulCurrFileSizeLow, ulCurrFileSizeHigh) - 1;
        else
            foundCapacity = MAX_DEVSPACE_PAGES + 1;

        if ( (foundCapacity < 1) || 
             ((ULONG)foundCapacity > MAX_DEVSPACE_PAGES) || 
             (isForcedReadOnly && (*devcapacity != foundCapacity)) )
        {
            foundCapacity += HIGH_VALUE_VALID(ulCurrFileSizeHigh) ? 1 : -1 ;
            MSGALL((ERR_INV_DEVSPACE_SIZE, pszLogDevName, foundCapacity ));
            DBG1  ((MF__, "Invalid devspace size: %u pages", foundCapacity ));
            sql46c_build_error_string ( errtext, ERRMSG_DEVIO_INV_DEVSPACE_SIZE, 0 );

            CloseHandle( FileHandle );
            if ( !isForcedReadOnly )
                *devcapacity = foundCapacity;

            DBGOUT;
            return -1;
        }

        // At least one page?
        if ( ulCurrFileSizeHigh || ulCurrFileSizeLow > XPARAM(ulDBPageSize) )
        {
            ULONG        ulDummy;

            // check file access
            rc = sql54k_evaluate_block_0 ( FileHandle, &ulDummy, &ulDummy, XPARAM(ulDBPageSize), false, pszLogDevName );

            if ( rc != NO_ERROR )
            {
                if (( rc == ERROR_SHARING_VIOLATION ) || ( rc == ERROR_LOCK_VIOLATION ))
                    sql46c_build_error_string ( errtext, ERRMSG_DEVIO_ACCESS_DENIED, 0 );
                else
                    sql46c_build_error_string ( errtext, ERRMSG_DEVIO_CREATE_DEVSPACE, rc );

                CloseHandle( FileHandle );
                DBGOUT;
                return rc;
            }
        }
    }
    else
    {
        if ((*devcapacity < 1) || ((ULONG)*devcapacity > MAX_DEVSPACE_PAGES))
        {
            MSGALL((ERR_INV_DEVSPACE_SIZE, pszLogDevName, *devcapacity ));
            DBG1  ((MF__, "Invalid devspace size: %u pages", *devcapacity ));
            sql46c_build_error_string ( errtext, ERRMSG_DEVIO_INV_DEVSPACE_SIZE, 0 );

            DBGOUT;
            return -1;
        }

        DBG3 (( MF__, "Creating devspace: %s - capacity: %u pages",
            pszLogDevName, *devcapacity ));

        FileHandle = __sql54k_create_file( pszPhysDevName,
                                           GENERIC_READ      | GENERIC_WRITE,
                                           FILE_SHARE_READ   |
                                           FILE_SHARE_WRITE,
                                           NULL,
                                           OPEN_ALWAYS,
                                           FILE_ATTRIBUTE_NORMAL  |
                                           FILE_FLAG_SEQUENTIAL_SCAN,
                                           NULL );

        if (FileHandle == INVALID_HANDLE_VALUE)
        {
            rc = GetLastError();

            if ( rc == ERROR_ACCESS_DENIED )
            {
                MSGALL(( ERR_DEVSPACE_ACCESS_DENIED, pszLogDevName ));
                DBG1  (( MF__, "Access on devspace '%s' denied", pszLogDevName ));
                sql46c_build_error_string ( errtext, ERRMSG_DEVIO_ACCESS_DENIED, 0 );
            }
            else
            {
                MSGALL(( ERR_CANT_CREATE_DEVSPACE, pszLogDevName, rc ));
                DBG1  (( MF__, "Cannot create devspace '%', rc = %d", pszLogDevName,
                    rc ));
                sql46c_build_error_string ( errtext, ERRMSG_DEVIO_CREATE_DEVSPACE, rc );
            }

            DBGOUT;
            return rc;
        }

        // At least one page?
        if ( ulCurrFileSizeHigh || ulCurrFileSizeLow > XPARAM(ulDBPageSize) )
        {
            ULONG        ulDummy;

            // check file access
            rc = sql54k_evaluate_block_0 ( FileHandle, &ulDummy, &ulDummy, XPARAM(ulDBPageSize), false, pszLogDevName );

            if ( rc != NO_ERROR )
            {
                if (( rc == ERROR_SHARING_VIOLATION ) || ( rc == ERROR_LOCK_VIOLATION ))
                    sql46c_build_error_string ( errtext, ERRMSG_DEVIO_ACCESS_DENIED, 0 );
                else
                    sql46c_build_error_string ( errtext, ERRMSG_DEVIO_CREATE_DEVSPACE, rc );

                CloseHandle( FileHandle );
                DBGOUT;
                return rc;
            }
        }

        if ( XPARAM(fAllowCompressedDevsp) == false )
            sql54k_uncompress( FileHandle );

        if ( !wantFormatDevspace )
        {
            if ( XPARAM(fFormatDataVolume) == true )
            {
                wantFormatDevspace = true;
            }
        }
    }

    PAGES_TO_QWORD( ulFilePosLow, ulFilePosHigh, *devcapacity + 1);

    rc = SET_FILE_PTR ( FileHandle, ulFilePosLow, FILE_BEGIN,
                        &ulFilePosLow, &ulFilePosHigh );

    if (rc == ERROR_DISK_FULL)
    {
        MSGALL(( ERR_DISK_FULL, pszLogDevName ));
        DBG1  (( MF__, "Could not create: '%s', not enough space on disk",
            pszLogDevName ));
        sql46c_build_error_string ( errtext, ERRMSG_DEVIO_DISK_FULL, 0 );

        CloseHandle( FileHandle );

        DBGOUT;
        return rc;
    }
    else if (rc != NO_ERROR)
    {
        MSGALL(( ERR_CANT_SEEK_ON_DEVSPACE, pszLogDevName, rc ));
        DBG1  (( MF__, "Cannot seek on devspace: %s, rc = %d", pszLogDevName,
            rc ));
        sql46c_build_error_string ( errtext, ERRMSG_DEVIO_SEEK_ON_DEVSPACE, rc );

        CloseHandle( FileHandle );

        DBGOUT;
        return rc;
    }

    if ( !isForcedReadOnly )
    {
        if ( !isSequential )
        {
            if (SetEndOfFile(FileHandle ) != true)
            {
                rc = GetLastError();

                if (rc == ERROR_DISK_FULL)
                {
                    MSGALL(( ERR_DISK_FULL, pszLogDevName ));
                    DBG1  (( MF__, "Could not create: '%s', not enough space on disk",
                        pszLogDevName ));
                    sql46c_build_error_string ( errtext, ERRMSG_DEVIO_DISK_FULL, 0 );
                }
                else
                {
                    MSGALL(( ERR_CANT_SEEK_ON_DEVSPACE, pszLogDevName, rc ));
                    DBG1  (( MF__, "Cannot seek on devspace: %s, rc = %d", pszLogDevName,
                        rc ));
                    sql46c_build_error_string ( errtext, ERRMSG_DEVIO_SEEK_ON_DEVSPACE, rc );
                }

                CloseHandle( FileHandle );

                DBGOUT;
                return rc;
            }
        }

        if ( !isSequential && wantFormatDevspace )
        {
            rc = sql54k_format_devspace ( FileHandle, pszLogDevName,
                                          ulCurrFileSizeLow, ulCurrFileSizeHigh,
                                          *devcapacity + 1,
                                          pidResponsibleForCall,
                                          diskImage,
                                          errtext );
            if ( rc != NO_ERROR )
            {
                CloseHandle( FileHandle );
                DBGOUT;
                return rc;
            }
        }

        rc = os54_FillBlock0 ( FileHandle, pszLogDevName, *devcapacity + 1, *devcapacity + 1, errtext );

        if ( rc != NO_ERROR )
        {
            CloseHandle( FileHandle );

            DBGOUT;
            return rc;
        }
    }

    if (!CloseHandle( FileHandle ))
    {
        rc = GetLastError();

        MSGALL(( ERR_CANT_CLOSE_DEVSPACE, rc ));
        DBG1  (( MF__, "Cannot close devspace, rc = %d", rc ));
        sql46c_build_error_string ( errtext, ERRMSG_DEVIO_CLOSE_DEVSPACE, rc );

        DBGOUT;
        return rc;
    }

    if ( !isForcedReadOnly )
    {
        rc = sql49c_set_file_security ( pszPhysDevName,
            kgs.AdminSA.lpSecurityDescriptor );

        if ( rc != NO_ERROR )
        {
            rc = GetLastError();
            sql46c_build_error_string ( errtext, ERRMSG_DEVIO_SET_DEVSPACE_OWNER, rc );

            DBGOUT;
            return rc;
        }
    }

    *ok = true;

    DBGOUT;
    return NO_ERROR;
  }

/*------------------------------*/

/*
 * Vdevsize for a device being a drive for direct io.
 */
static void os54_vdevsizeDrive ( PSZ               pszDriveName,
                                   INT4           *devcapacity,
                                   tsp00_TaskId    pidResponsibleForCall,
                                   char           *diskImage,
                                   int             isSequential,
                                   int             isForcedReadOnly,
                                   ERRORTEXT       errtext,
                                   BOOLEAN        *ok )
{
# undef  MF__
# define MF__  MOD__"os54_vdevsizeDrive"

    HANDLE       FileHandle;
    ULONG        ulDummy;
    ULONG        ulMaxPages;
    LONG
    rc = NO_ERROR;                      /* Return code */
    SAPDB_Bool   isLocked = false;
    DBGIN;

    *ok = false;

    DBG3 ((MF__, "Opening devspace: %s", pszDriveName));

    FileHandle = CreateFile( pszDriveName,
                             isForcedReadOnly ? (GENERIC_READ) : ( GENERIC_READ | GENERIC_WRITE ),
                             FILE_SHARE_READ | FILE_SHARE_WRITE,
                             NULL,
                             OPEN_EXISTING,
                             FILE_ATTRIBUTE_NORMAL,
                             NULL );
 
    if (FileHandle == INVALID_HANDLE_VALUE)
        rc = GetLastError();

    if (rc == ERROR_DRIVE_LOCKED)
    {
        MSGALL (( ERR_DEVSPACE_LOCKED, pszDriveName ));
        DBG1   ( (MF__, "Devspace '%s' locked by another process", pszDriveName ));
        sql46c_build_error_string ( errtext, ERRMSG_DEVIO_DEVSPACE_LOCKED, 0 );

        DBGOUT;
        return;
    }

    if ( rc == ERROR_ACCESS_DENIED )
    {
        MSGALL(( ERR_DEVSPACE_ACCESS_DENIED, pszDriveName ));
        DBG1  (( MF__, "Access on devspace '%s' denied", pszDriveName ));
        sql46c_build_error_string ( errtext, ERRMSG_DEVIO_ACCESS_DENIED, 0 );

        DBGOUT;
        return;
    }
    else if ( rc != NO_ERROR )
    {
        MSGALL (( ERR_CANT_OPEN_DEVSPACE, pszDriveName, rc ));
        DBG1   ( (MF__, "Cannot open devspace '%s', rc = %d", pszDriveName, rc ));
        sql46c_build_error_string ( errtext, ERRMSG_DEVIO_OPEN_DEVSPACE, rc );

        DBGOUT;
        return;
    }

    isLocked = false;
    if ( !isForcedReadOnly )
    {
        if ( XPARAM(fSetVolumeLock) )
        {
            rc = sql54k_lock_drive ( FileHandle );
            if (rc == ERROR_DRIVE_LOCKED)
            {
                MSGALL (( ERR_DEVSPACE_LOCKED, pszDriveName ));
                DBG1   ( (MF__, "Devspace '%s' locked by another process", pszDriveName ));
                sql46c_build_error_string ( errtext, ERRMSG_DEVIO_DEVSPACE_LOCKED, 0 );

                DBGOUT;
                return;
            }

            if (rc != 0)
            {
                MSGALL (( ERR_CANT_LOCK_DEVSPACE, pszDriveName, rc ));
                DBG1   ( (MF__, "Cannot lock devspace '%s', rc = %d", pszDriveName, rc ));
                sql46c_build_error_string ( errtext, ERRMSG_DEVIO_LOCK_DEVSPACE, rc );

                DBGOUT;
                return;
            }
            isLocked = true;
        }
    }

    rc = sql54k_evaluate_block_0 ( FileHandle,
                                 &ulDummy,
                                 &ulMaxPages,
                                 XPARAM(ulDBPageSize),
                                 true,
                                 pszDriveName );

    if ( rc == NO_ERROR )
    {
        DBG3  (( MF__, "ulMaxPages: %d", ulMaxPages ));

        if (*devcapacity == 0)
        {
            *devcapacity = ulMaxPages - 1;

            if (*devcapacity < 1)
            {
                MSGALL(( ERR_INV_DEVSPACE_SIZE, pszDriveName, *devcapacity ));
                DBG1  (( MF__, "Devspace '%s', invalid size: %u pages", pszDriveName,
                     *devcapacity ));
                sql46c_build_error_string ( errtext, ERRMSG_DEVIO_INV_DEVSPACE_SIZE, rc );
                DBGOUT;
                return;
            }

            MSGD (( INFO_DEVSPACE_SIZE, pszDriveName, *devcapacity ));
            DBG3 (( MF__, "Devspace '%s', calculated size: %d pages",
                  pszDriveName, *devcapacity ));
        }
        else
        {
            if ((ULONG) *devcapacity > (ulMaxPages - 1))
            {
                MSGD ((INFO_DEV_SIZE_EXC_MAX,
                    pszDriveName, (ulMaxPages - 1) ));
                DBG1 ((MF__, "Devspace '%s', requested size exceeds maximum ( %d ).",
                    pszDriveName, (ulMaxPages - 1) ));

                *devcapacity  = ulMaxPages - 1;
            }
            else if ( (ULONG) *devcapacity < (ulMaxPages - 1) )
            {
                MSGD (( INFO_DEV_SIZE_LOV_MAX,
                    pszDriveName, *devcapacity, (ulMaxPages - 1) ));
                DBG1 (( MF__, "Devspace '%s', requested size ( %d ) is lover than maximum ( %d ).",
                    pszDriveName, *devcapacity, (ulMaxPages - 1) ));
            }
        }

        if ( !isSequential 
            && diskImage != 0 
            && !isForcedReadOnly )
        {
            rc = sql54k_format_devspace ( FileHandle, pszDriveName,
                XPARAM(ulDBPageSize), 0,
                *devcapacity + 1,
                pidResponsibleForCall,
                diskImage,
                errtext );
            if ( rc != NO_ERROR )
            {
                CloseHandle( FileHandle );
                DBGOUT;
                return;
            }
        }

        if ( !isForcedReadOnly )
        {
            rc = os54_FillBlock0 ( FileHandle, pszDriveName, *devcapacity + 1, ulMaxPages, errtext );
        }

        if ( rc == NO_ERROR )
            *ok = true;
        else
            *ok = false;
    }
    else
    {
        sql46c_build_error_string ( errtext, ERRMSG_DEVIO_OPEN_DEVSPACE, 0 );
    }

    if ( sql54k_dev_close ( FileHandle,
                            &isLocked,
                            pszDriveName, 
                            DT_DRIVE,
                            errtext) != NO_ERROR )
    {
        *ok = false;
    }

    DBGOUT;
    return;
}

/*------------------------------*/

/*
 * initialize block 0 of a devspace
 */

static LONG    os54_FillBlock0 ( HANDLE     DevHandle,
                                     PSZ        pszLogDevName,
                                     ULONG      ulDevPages,
                                     ULONG      ulMaxDevPages,
                                     ERRORTEXT  errtext )
  {
# undef  MF__
# define MF__   MOD__"os54_FillBlock0"
  ULONG                     ulFilePosLow;
  PCHAR                     pTmpBuffer;
  ULONG                     ulDBPageSize = XPARAM(ulDBPageSize) ;
  PCHAR                     pBuffer      = NULL;
  LONG                      rc           = NO_ERROR;
  FILE_SEGMENT_ELEMENT      aSegmentArray[2];

  DBGIN;

  rc = ALLOC_MEM ((PPVOID)&pBuffer, ulDBPageSize +
                  kgs.ulSystemPageSize /* alignment */ );

  if( rc != NO_ERROR )
    {
      MSGD(( ERR_ALLOC_MEMORY__FILE_LINE__, rc, __FILE__, __LINE__ ));
    DBGOUT;
    return rc;
    }

  // --- alignment needed for none cached file operations
  pTmpBuffer = (PCHAR)ALIGN( (tsp00_Longuint)pBuffer, kgs.ulSystemPageSize );

  rc = SET_FILE_PTR ( DevHandle, 0, FILE_BEGIN, &ulFilePosLow, NULL );

  if ( rc == ERROR_DISK_FULL )
    {
    MSGALL(( ERR_DISK_FULL, pszLogDevName ));
    DBG1  (( MF__, "Could not create: '%s', not enough space on disk",
             pszLogDevName ));
    sql46c_build_error_string ( errtext, ERRMSG_DEVIO_DISK_FULL, 0 );
    FREE_MEM ( (PVOID)pBuffer );

    DBGOUT;
    return rc;
    }
  else if ( rc != NO_ERROR )
    {
    MSGALL(( ERR_CANT_SEEK_ON_DEVSPACE, pszLogDevName, rc ));
    DBG1  (( MF__, "Cannot seek on devspace: %s, rc = %d", pszLogDevName, rc ));
    sql46c_build_error_string ( errtext, ERRMSG_DEVIO_SEEK_ON_DEVSPACE, rc );
    FREE_MEM ( (PVOID)pBuffer );

    DBGOUT;
    return rc;
    }

  memset (pTmpBuffer, '\0', ulDBPageSize);

  // --- fill buffer
  //
  //     0x12345678 ->  pTmpBuffer[2] = 0x56
  //                    pTmpBuffer[3] = 0x78
  //                    pTmpBuffer[4] = 0x12
  //                    pTmpBuffer[5] = 0x34
  //
  pTmpBuffer[2] = (CHAR) (( ulDBPageSize % KBYTE_64 ) / 256);
  pTmpBuffer[3] = (CHAR) (( ulDBPageSize % KBYTE_64 ) % 256);
  pTmpBuffer[4] = (CHAR) (( ulDBPageSize / KBYTE_64 ) / 256);
  pTmpBuffer[5] = (CHAR) (( ulDBPageSize / KBYTE_64 ) % 256);

  SAPDB_memcpy ( pTmpBuffer + PRODUCT_ID_POS,
           PRODUCT_ID_STRING,
           sizeof (PRODUCT_ID_STRING) - 1);

  SAPDB_memcpy ( pTmpBuffer + DEVSPACE_ID_POS,
           DEVSPACE_ID_STRING,
           sizeof (DEVSPACE_ID_STRING) - 1);

  *(ULONG*)(&pTmpBuffer[ DEVSPACE_MAXSIZE_POS ])    = ulMaxDevPages;
  *(ULONG*)(&pTmpBuffer[ DEVSPACE_SIZE_POS ])       = ulDevPages;
  *(ULONG*)(&pTmpBuffer[ DEVSPACE_BAD_MARK_POS ])   = (ULONG)0;
  *(ULONG*)(&pTmpBuffer[ DEVSPACE_REINT_INFO_POS ]) = (ULONG)UNDEF;

  DBG2 ((MF__, "Block 0 contents", pTmpBuffer, ulDBPageSize));

  aSegmentArray[0].Buffer = pTmpBuffer;
  aSegmentArray[1].Buffer = NULL;

  rc = __sql54k_io_write ( DevHandle, aSegmentArray, 1, NULL );

  if ( rc != NO_ERROR )
    {
    MSGD  (( ERR_CANT_WRITE_ON_DEVSPACE, 0, pszLogDevName, rc ));
    DBG1 ((MF__, "Cannot write on devspace '%s', rc = %d", pszLogDevName, rc ));
    sql46c_build_error_string ( errtext, ERRMSG_DEVIO_WRITE_ON_DEVSPACE, rc );
    FREE_MEM ( (PVOID)pBuffer );

    DBGOUT;
    return rc;
    }

  rc = FREE_MEM ( (PVOID)pBuffer );

  if( rc != NO_ERROR )
    {
    DBGOUT;
    return rc;
    }

  DBGOUT;

  return rc;
  }

/*------------------------------*/

static LONG    sql54k_format_devspace ( HANDLE           DevHandle,
                                        PSZ              pszLogDevName,
                                        ULONG            ulCurrFileSizeLow,
                                        ULONG            ulCurrFileSizeHigh,
                                        ULONG            ulDevPages,
                                        tsp00_TaskId     pidResponsibleForCall,
                                        char             *diskImage,
                                        ERRORTEXT        errtext )
  {
# undef  MF__
# define MF__   MOD__"sql54k_format_devspace"
  ULONG                     ulPageNumber;
  ULONG                     ulFilePosHigh;
  ULONG                     ulFilePosLow;
  PCHAR                     pTmpBuffer;
  ULONG                     ulDBPageSize = XPARAM(ulDBPageSize);
  ULONG                     ulBuffPages;
  PCHAR                     pBuffer      = NULL;
  LONG                      rc           = NO_ERROR;
  FILE_SEGMENT_ELEMENT      aSegmentArray[2];
  tsp00_Int4                blocksWritten;
  PTASK_CTRL_REC            pTaskCtrl = &kgs.pFirstTaskCtrl[pidResponsibleForCall - 1];

  DBGIN;

  if ( diskImage )
  {
      /* If formatting with image is wanted, first page to format is behind RTE page */
      ulPageNumber = 1;
  }
  else
  {
      /* If only expanding of file is wanted, seek behind current last page */
      ulPageNumber = QWORD_TO_PAGES( ulCurrFileSizeLow, ulCurrFileSizeHigh );
  }

  if ( ulPageNumber < ulDevPages )
    {
    ulBuffPages = (32 KB) / ulDBPageSize;

    if ( ulBuffPages == 0 ) ulBuffPages = 1;

    rc = ALLOC_MEM ((PPVOID)&pBuffer,
                    (ulDBPageSize * ulBuffPages) +
                    kgs.ulSystemPageSize /* alignment */ );


    if( rc != NO_ERROR )
    {
      MSGD(( ERR_ALLOC_MEMORY__FILE_LINE__, rc, __FILE__, __LINE__ ));
      DBGOUT;
      return rc;
    }

    // --- alignment needed for none cached file operations
    pTmpBuffer = (PCHAR)ALIGN( (tsp00_Longuint)pBuffer, kgs.ulSystemPageSize );

    if ( diskImage )
    {
        ULONG iPages;
        for ( iPages = 0; iPages < ulBuffPages; iPages++ )
        {
            SAPDB_memcpy ( pTmpBuffer + (iPages * ulDBPageSize), diskImage, ulDBPageSize );
        }
    }
    else
    {
        memset ( pTmpBuffer, '\0', (ulDBPageSize * ulBuffPages) );
    }

    aSegmentArray[0].Buffer = pTmpBuffer;
    aSegmentArray[1].Buffer = NULL;


    PAGES_TO_QWORD( ulFilePosLow, ulFilePosHigh, ulPageNumber );

    rc = SET_FILE_PTR( DevHandle, ulFilePosLow, FILE_BEGIN,
                       &ulFilePosLow, &ulFilePosHigh );

    if ( rc == ERROR_DISK_FULL )
      {
      MSGALL(( ERR_DISK_FULL, pszLogDevName ));
      DBG1  (( MF__, "Could not create: '%s', not enough space on disk",
               pszLogDevName ));
      sql46c_build_error_string ( errtext, ERRMSG_DEVIO_DISK_FULL, 0 );
      FREE_MEM ( (PVOID)pBuffer );

      DBGOUT;
      return rc;
      }
    else if ( rc != NO_ERROR )
      {
      MSGALL(( ERR_CANT_SEEK_ON_DEVSPACE, pszLogDevName, rc ));
      DBG1  (( MF__, "Cannot seek on devspace: %s, rc = %d", pszLogDevName, rc ));
      sql46c_build_error_string ( errtext, ERRMSG_DEVIO_SEEK_ON_DEVSPACE, rc );
      FREE_MEM ( (PVOID)pBuffer );

      DBGOUT;
      return rc;
      }

    for ( blocksWritten = 0; ulPageNumber + ulBuffPages <= ulDevPages;
            ulPageNumber += ulBuffPages )
      {
        rc = __sql54k_io_write ( DevHandle, aSegmentArray, ulBuffPages, NULL );

      if ( rc == ERROR_DISK_FULL )
        {
        MSGALL(( ERR_DISK_FULL, pszLogDevName ));
        DBG1  (( MF__, "Could not create: '%s', not enough space on disk",
                 pszLogDevName ));
        sql46c_build_error_string ( errtext, ERRMSG_DEVIO_DISK_FULL, 0 );
        FREE_MEM ( (PVOID)pBuffer );

        DBGOUT;
        return rc;
        }
      else if ( rc != NO_ERROR )
        {
        MSGD  (( ERR_CANT_WRITE_ON_DEVSPACE, ulPageNumber, pszLogDevName, rc ));
        DBG1 ((MF__, "Cannot write on devspace '%s', rc = %d", pszLogDevName, rc ));
        sql46c_build_error_string ( errtext, ERRMSG_DEVIO_WRITE_ON_DEVSPACE, rc );
        FREE_MEM ( (PVOID)pBuffer );

        DBGOUT;
        return rc;
        }

/*
  To reduce the number of collisions between parallel working formatting threads, the statistic
  update (which is nneded to have at least something monitorable...) is not done for each block
  but after the given number of blocks
 */
        ++blocksWritten;
#define FORMAT_IO_STAT_BLOCKS_WRITTEN_LIMIT_OS54 16
        if ( blocksWritten >= FORMAT_IO_STAT_BLOCKS_WRITTEN_LIMIT_OS54 )
        {
            WAIT_UNTIL_ALONE( &pTaskCtrl->lExclusive );
                pTaskCtrl->TaskStateStat.DevIOWriteStat.IOStat.TotalTimeStat.ulCount += blocksWritten;
				pTaskCtrl->TaskStateStat.DevIOWriteStat.TotalPageCount += (ulBuffPages * blocksWritten);
                pTaskCtrl->TaskStateStat.DevIOWriteStat.IOStat.TimeStat.ulCount += blocksWritten;
				pTaskCtrl->TaskStateStat.DevIOWriteStat.ulPageCount += (ulBuffPages * blocksWritten);
            CLEARLOCK( &pTaskCtrl->lExclusive );
            blocksWritten = 0;
        }
      }

    /* make sure statistic is correct for last block I/O */
    if ( blocksWritten > 0 )
    {
        WAIT_UNTIL_ALONE( &pTaskCtrl->lExclusive );
            pTaskCtrl->TaskStateStat.DevIOWriteStat.IOStat.TotalTimeStat.ulCount += blocksWritten;
            pTaskCtrl->TaskStateStat.DevIOWriteStat.TotalPageCount += (ulBuffPages * blocksWritten);
            pTaskCtrl->TaskStateStat.DevIOWriteStat.IOStat.TimeStat.ulCount += blocksWritten;
            pTaskCtrl->TaskStateStat.DevIOWriteStat.ulPageCount += (ulBuffPages * blocksWritten);
        CLEARLOCK( &pTaskCtrl->lExclusive );
    }

    if ( ulPageNumber < ulDevPages )
      {
         rc = __sql54k_io_write ( DevHandle, aSegmentArray, ulDevPages - ulPageNumber, NULL );

      if ( rc == ERROR_DISK_FULL )
        {
        MSGALL(( ERR_DISK_FULL, pszLogDevName ));
        DBG1  (( MF__, "Could not create: '%s', not enough space on disk",
                 pszLogDevName ));
        sql46c_build_error_string ( errtext, ERRMSG_DEVIO_DISK_FULL, 0 );
        FREE_MEM ( (PVOID)pBuffer );

        DBGOUT;
        return rc;
        }
      else if ( rc != NO_ERROR )
        {
        MSGD  (( ERR_CANT_WRITE_ON_DEVSPACE, ulPageNumber, pszLogDevName, rc ));
        DBG1 ((MF__, "Cannot write on devspace '%s', rc = %d", pszLogDevName, rc ));
        sql46c_build_error_string ( errtext, ERRMSG_DEVIO_WRITE_ON_DEVSPACE, rc );
        FREE_MEM ( (PVOID)pBuffer );

        DBGOUT;
        return rc;
        }

       /* make sure statistic is correct for last I/O */
        WAIT_UNTIL_ALONE( &pTaskCtrl->lExclusive );
            ++pTaskCtrl->TaskStateStat.DevIOWriteStat.IOStat.TotalTimeStat.ulCount;
            pTaskCtrl->TaskStateStat.DevIOWriteStat.TotalPageCount += (ulDevPages - ulPageNumber);
            ++pTaskCtrl->TaskStateStat.DevIOWriteStat.IOStat.TimeStat.ulCount;
            pTaskCtrl->TaskStateStat.DevIOWriteStat.ulPageCount += (ulDevPages - ulPageNumber);
        CLEARLOCK( &pTaskCtrl->lExclusive );
      }

    rc = FREE_MEM ( (PVOID)pBuffer );

    if( rc != NO_ERROR )
      {
      DBGOUT;
      return rc;
      }
    }

  DBGOUT;
  return rc;
  }

/*------------------------------*/

static LONG  sql54k_evaluate_block_0  ( HANDLE        DevHandle,
                                        PULONG        pulDevPages,
                                        PULONG        pulMaxDevPages,
                                        ULONG         ulDBPageSize,
                                        BOOL          fDevspaceIsDrive,
                                        PSZ           pszLogDevName )
  {
# undef  MF__
# define MF__   MOD__"sql54k_evaluate_block_0"
  ULONG                       ulFilePosLow;
  BOOL                        fDevspaceIDOk;
  LONG                        rc      = NO_ERROR;
  PCHAR                       pBuffer = NULL;
  PCHAR                       pTmpBuffer;
  FILE_SEGMENT_ELEMENT        aSegmentArray[2];
  tsp00_PageNo                 BadDevPos;
  ULONG                       ulFileSizeLow;
  ULONG                       ulFileSizeHigh;

  DBGIN;

  *pulDevPages = 0;

  rc = ALLOC_MEM ((PPVOID)&pBuffer,
                  ulDBPageSize +
                  kgs.ulSystemPageSize /* alignment */ );

  if( rc != NO_ERROR )
    {
      MSGD(( ERR_ALLOC_MEMORY__FILE_LINE__, rc, __FILE__, __LINE__ ));
      DBGOUT;
    return rc;
    }

  // --- alignment needed for none cached file operations
  pTmpBuffer = (PCHAR)ALIGN( (tsp00_Longuint)pBuffer, kgs.ulSystemPageSize );

  // --- no devspace file handle defined?
  rc = SET_FILE_PTR ( DevHandle, 0, FILE_BEGIN, &ulFilePosLow, NULL );

  if ( rc != NO_ERROR )
    {
    MSGALL(( ERR_CANT_SEEK_ON_DEVSPACE, pszLogDevName, rc ));
    DBG1  (( MF__, "Cannot seek on devspace: %s, rc = %d", pszLogDevName, rc ));
    FREE_MEM ( (PVOID)pBuffer );
    DBGOUT;
    return rc;
    }

  aSegmentArray[0].Buffer = pTmpBuffer;
  aSegmentArray[1].Buffer = NULL;

  rc = __sql54k_io_read_with_retries( DevHandle, aSegmentArray, 1, 0, pszLogDevName, NUMBER_OF_IO_RETRIES, TRUE );

  if (( rc == ERROR_SHARING_VIOLATION ) || ( rc == ERROR_LOCK_VIOLATION ))
    {
    MSGALL(( ERR_DEVSPACE_IN_USE, pszLogDevName ));
    DBG1  (( MF__, "Devspace '%s' is already used by another process, rc = %u", pszLogDevName, rc ));
    FREE_MEM ( (PVOID)pBuffer );
    DBGOUT;
    return rc;
    }
  else if (rc != NO_ERROR)
    {
    MSGALL(( ERR_CANT_READ_FROM_DEVSPACE, 0, pszLogDevName, rc ));
    DBG1  (( MF__, "Cannot read from devspace: '%s', rc = %d", pszLogDevName, rc ));
    FREE_MEM ( (PVOID)pBuffer );
    DBGOUT;
    return rc;
    }

  DBG2 ((MF__, "Block 0 contents", pTmpBuffer, ulDBPageSize));

  fDevspaceIDOk = !strncmp ( pTmpBuffer + DEVSPACE_ID_POS,
                             DEVSPACE_ID_STRING,
                             sizeof (DEVSPACE_ID_STRING) - 1 );

  if (fDevspaceIDOk == false)
    {
    MSGALL(( ERR_NOT_A_VALID_DEVSPACE, pszLogDevName ));
    DBG1  (( MF__, "It's not a valid devspace: '%s'", pszLogDevName ));
    DBG1  (( MF__, "ulRead: %d - fDevspaceIDOk: %f ", ulRead, fDevspaceIDOk ));
    FREE_MEM ( (PVOID)pBuffer );
    DBGOUT;
    return -1;
    }

  if ( fDevspaceIsDrive )
    {
    *pulDevPages    = *(ULONG*)(&pTmpBuffer[ DEVSPACE_SIZE_POS ]);
    *pulMaxDevPages = *(ULONG*)(&pTmpBuffer[ DEVSPACE_MAXSIZE_POS ]);
    }
  else
    {
    ulFileSizeLow = (ULONG)GetFileSize((HANDLE)DevHandle, &ulFileSizeHigh);

    if ((ulFileSizeLow == MAX_DWORD_VAL) && ((rc = GetLastError()) != NO_ERROR))
      {
      MSGALL(( ERR_DURING_Q_FILE_SIZE, rc ));
      DBG1  (( MF__, "Error during GetFileSize, rc = %d \n", rc ));
      }
    else
      {
      if (HIGH_VALUE_VALID(ulFileSizeHigh) == true)
        {
        *pulDevPages    = QWORD_TO_PAGES( ulFileSizeLow, ulFileSizeHigh );
        *pulMaxDevPages = QWORD_TO_PAGES( ulFileSizeLow, ulFileSizeHigh );
        }
      else
        {
        MSGALL((ERR_INV_DEVSPACE_SIZE, pszLogDevName, MAX_DEVSPACE_PAGES));
        DBG1  ((MF__, "Invalid devspace size: %u pages", MAX_DEVSPACE_PAGES));
        FREE_MEM ( (PVOID)pBuffer );
        DBGOUT;
        return -1;
        }
      }
    }

  SAPDB_memcpy ( &BadDevPos, pTmpBuffer + DEVSPACE_BAD_MARK_POS, sizeof(BadDevPos) );


  if ( BadDevPos != 0 )
    {
    rc = !NO_ERROR ;
    MSGALL(( ERR_DEVSP_MARKED_AS_BAD, pszLogDevName, BadDevPos ));
    FREE_MEM ( (PVOID)pBuffer );
    DBGOUT;
    return rc;
    }

  rc = FREE_MEM ( (PVOID)pBuffer );

  if( rc != NO_ERROR )
    {
    DBGOUT;
    return rc;
    }

  DBGOUT;
  return rc;
  }

/*------------------------------*/

static PDEVI_CTRL_REC sql54k_find_devi ( LONG    lDevspaceNo,
                                         PULONG  pulQueueLength )
  {
# undef  MF__
# define MF__ MOD__"sql54k_find_devi"
  PDEVI_CTRL_REC                    pFirstDeviCtrl;
  PDEVI_CTRL_REC                    pDeviCtrl;
  ULONG                             ulSlot,
                                    ulSlotReminder,
                                    ulMinQueueReminder;
  #ifdef DEBUG_RTE
   PTASK_CTRL_REC                   pTaskCtrl = THIS_UKT_CTRL->pCTask;
  #endif

  DBGIN_T (pTaskCtrl->ulTaskIndex);

  DBG3 (( MF__, "[T:0x%03u] Find slot for despace %d",
          pTaskCtrl->ulTaskIndex, lDevspaceNo ));

  pFirstDeviCtrl = kgs.pFirstDeviCtrl + lDevspaceNo * XPARAM(lDeviPerDevspace);

  // --- is there only one DEVi?
  if (( XPARAM(lDeviPerDevspace)  == 1 ) ||
      ( kgs.Dev0.lTraceDevspaceNo == lDevspaceNo ))
    {
    DBG3 (( MF__, "[T:0x%03u] Threre is only one DEVi, devspace  %d",
            pTaskCtrl->ulTaskIndex, lDevspaceNo ));

    *pulQueueLength = pFirstDeviCtrl->DevIOStat.ulIOQueLen;

    DBGOUT_T (pTaskCtrl->ulTaskIndex);
    return pFirstDeviCtrl;
    }

  /*
   *  Find a sleeping DEVi Process
   */
  ulSlotReminder     = 0;
  ulMinQueueReminder = (ULONG)-1;

  for ( ulSlot = 0, pDeviCtrl = pFirstDeviCtrl;
        ulSlot < (ULONG) XPARAM(lDeviPerDevspace);
        ulSlot++, pDeviCtrl++ )
    {
    if ( pDeviCtrl->DevIOStat.ulIOQueLen <= (ULONG )XPARAM(lDevQueue) )
      {
      DBG3 (( MF__, "[T:0x%03u] Found sleeping DEVi 0x%x, using slot %u",
              pTaskCtrl->ulTaskIndex, pDeviCtrl, ulSlot ));

      *pulQueueLength = pDeviCtrl->DevIOStat.ulIOQueLen;

      DBGOUT_T (pTaskCtrl->ulTaskIndex);
      return pDeviCtrl;
      }

    if (ulMinQueueReminder > (ULONG)pDeviCtrl->DevIOStat.ulIOQueLen)
      {
      // --- set the the reminder to the DEVi with smallest queue length
      ulMinQueueReminder = (ULONG)pDeviCtrl->DevIOStat.ulIOQueLen;
      ulSlotReminder     = ulSlot;
      }
    }

  //
  // --- no sleeping DEVi found. Use the DEVi with the smallest queue length
  //
  *pulQueueLength = ulMinQueueReminder;
  DBG3 (( MF__, "[T:0x%03u] No sleeping DEVi for Devspace No.%u found. Using Slot %u",
                 pTaskCtrl->ulTaskIndex, lDevspaceNo, ulSlotReminder));

  DBGOUT_T (pTaskCtrl->ulTaskIndex);
  return pFirstDeviCtrl + ulSlotReminder;
  }


/*------------------------------*/

/*
 * find a free device slot number
 */
static BOOL sql54k_find_device_no ( PLONG          plDevspaceNo,
                                      PSZ            pszLogDevName,
                                      PULONG         pulErrCode )
  {
# undef  MF__
# define MF__ MOD__"sql54k_find_device_no"
  LONG                             lDevspaceNo      = 0,
                                   lFirstFree       = -1;
  BOOL                             fAlreadyAttached = false,
                                   fOK;
  PDEV_IO_CTRL_REC                 pIOCtrl;

  DBGIN;

  /*
   * Check and search a free DEVi control record
   */
  while ( lDevspaceNo < XPARAM(lMaxDevspaces)  && !fAlreadyAttached)
    {
    pIOCtrl = &kgs.Dev0.pIOCtrl[lDevspaceNo];

    if ( pIOCtrl->ulAttached )
      {
      fAlreadyAttached = !strcmp( pIOCtrl->szDevspace, pszLogDevName );
      }
    else if ( lFirstFree == -1 )
      lFirstFree = lDevspaceNo;

    lDevspaceNo++;
    }

  if ( fAlreadyAttached )
    {
    MSGALL(( ERR_DEVSPACE_ALREADY_ATT, pszLogDevName ));
    DBG1  (( MF__, "Devspace '%s' already attached", pszLogDevName ));
    *pulErrCode = ERRCODE_DEVIO_ALRATT;

    fOK = false;
    *plDevspaceNo = -1;
    }
  else
    {
    if (lFirstFree != -1)
      {
      DBG3 (( MF__, "First free devspace no. %d", lFirstFree ));
      fOK = true;
      *plDevspaceNo = lFirstFree;
      }
    else
      {
      DBG1  (( MF__, "Maximum number of devspaces reached" ));
      MSGALL(( ERR_MAX_DEVSPACES_REACHED ));
      *pulErrCode = ERRCODE_DEVIO_MAXDEVNO;

      fOK = false;
      *plDevspaceNo = -1;
      }
    }

  DBGOUT;
  return fOK;
  }


/*------------------------------*/

/*
 *   Tries to get a request from the DEVx 'IOWork' queue, if no request
 *   is available then this thread should go to sleep until it is waked
 *   by a task/thread making a IO-request to it.
 */

static PDLQ_REC  sql54k_dev_receive ( PPIO_QUEUE_REC          ppIOWorkQue,
                                      PLONG                   plIOQueLen,
                                      PLONG                   plIOWorkExclusive,
                                      PTHREAD_CTRL_HEADER_REC pThrdCtrlHeader,
                                      teo52_DevIOStatistic   *pDeviStatistic)

  {
# undef  MF__
# define MF__ MOD__"sql54k_dev_receive"
  LONG       rc = NO_ERROR;
  PDLQ_REC   pRequest;

  DBGIN;

  for (;;)
    {
    DBG3 (( MF__, "DEV or DEVi will sleep on hevSem %d",
            pThrdCtrlHeader->hevSem ));

    // --- remove element from queue
    WAIT_UNTIL_ALONE( plIOWorkExclusive );

    pRequest = (*ppIOWorkQue)->pRequest;

    if ( pRequest )
      {
      (*ppIOWorkQue)->pRequest = 0;
      (*ppIOWorkQue)           = (*ppIOWorkQue)->pNextElem;
      (*plIOQueLen)--;

      if (( pDeviStatistic ) && ( pDeviStatistic->ResetPending ))
        {
        memset( &pDeviStatistic->DevRead.IOStat.TimeStat, 0, sizeof(teo52_AbsoluteTime));
        pDeviStatistic->DevRead.ulPageCount = 0;
        memset( &pDeviStatistic->DevWrite.IOStat.TimeStat, 0, sizeof(teo52_AbsoluteTime));
        pDeviStatistic->DevWrite.ulPageCount = 0;
        pDeviStatistic->ulMaxIOQueLen = 0;
        pDeviStatistic->ResetPending = false;
        }

      CLEARLOCK( plIOWorkExclusive );
      break;
      }
    CLEARLOCK( plIOWorkExclusive );

    // --- signal that there are no wake up activities
    pThrdCtrlHeader->ThrdState = KT_SLEEPING;

    rc = sql41c_wait_event_sem (pThrdCtrlHeader->hevSem, (ULONG) -1, "DEV or DEVi" );

    if ((rc != NO_ERROR) && (rc != ERROR_TIMEOUT))
        ABORT();

    pThrdCtrlHeader->ThrdState = KT_RUNNING;
    }

  DBG3 (( MF__, "DEV or DEVi Request: 0x%x from TID:%u, [T:0x%03u]",
          pRequest, pRequest->pTaskCtrl->pUKT->ThrdCtrlHeader.Tid,
          pRequest->pTaskCtrl->ulTaskIndex ));

  return pRequest;
  }

/*------------------------------*/

static LONG sql54k_dev_open ( PSZ            pszLogDevName,
                              LONG           lDevspaceNo,
                              SAPDB_Bool     isForcedReadOnly,
                              ULONG          ulOptions,
                              HANDLE        *pDevHandle,
                              SAPDB_Bool    *pIsLocked,
                              ERRORTEXT      pcErrText,
                              PULONG         pulErrCode,
                              LONG          *lDevspaceType )
  {
# undef  MF__
# define MF__ MOD__"sql54k_dev_open"
  DWORD                dwFlagsAndAttributes;
  LONG                 rc        = NO_ERROR;
  ULONG                ulTmp     = 0;
  PATHNAME             szPhysDevName;
  LONG                 lAtt;

  DBGIN;

  *pIsLocked = false;
  // --- substitute logical path name parts
  sql44c_subst_log_parts ( szPhysDevName, pszLogDevName );

  dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL;

  if ( ulOptions & DEV_NO_BUFFERING )
    dwFlagsAndAttributes |= FILE_FLAG_NO_BUFFERING;

  if ( ulOptions & DEV_OVERLAPPED_IO )
    dwFlagsAndAttributes |= FILE_FLAG_OVERLAPPED;

  *lDevspaceType = sql44c_get_dev_type_by_filename ( szPhysDevName ) ;

  switch ( *lDevspaceType )
    {
    case DT_DRIVE :

    *pDevHandle = __sql54k_create_file( szPhysDevName,
                                        isForcedReadOnly ? (GENERIC_READ) : (GENERIC_READ | GENERIC_WRITE ),
                                        FILE_SHARE_READ   |
                                        FILE_SHARE_WRITE,
                                        NULL,
                                        OPEN_EXISTING,
                                        dwFlagsAndAttributes,
                                        NULL );

    if (*pDevHandle == INVALID_HANDLE_VALUE)
      rc = GetLastError();

    if (rc == ERROR_DRIVE_LOCKED)
      {
      *pulErrCode = ERRCODE_DEVIO_DEVSPACE_LOCKED;
      sql46c_build_error_string ( pcErrText, ERRMSG_DEVIO_DEVSPACE_LOCKED, 0 );
      MSGALL (( ERR_DEVSPACE_LOCKED, szPhysDevName ));
      DBG1   ( (MF__, "Devspace '%s' locked by another process", szPhysDevName ));
      *pDevHandle = INVALID_HANDLE_VALUE;

      DBGOUT;
      return rc;
      }
    else if ( rc == ERROR_ACCESS_DENIED )
      {
      *pulErrCode = ERRCODE_DEVIO_ACCESS_DENIED;
      MSGALL(( ERR_DEVSPACE_ACCESS_DENIED, szPhysDevName ));
      DBG1  (( MF__, "Access on devspace '%s' denied", szPhysDevName ));
      sql46c_build_error_string ( pcErrText, ERRMSG_DEVIO_ACCESS_DENIED, 0 );
      *pDevHandle = INVALID_HANDLE_VALUE;
      }
    else if ( rc != NO_ERROR )
      {
      *pulErrCode = ERRCODE_DEVIO_OPEN_DEVSPACE;
      sql46c_build_error_string ( pcErrText, ERRMSG_DEVIO_OPEN_DEVSPACE, rc );
      MSGALL (( ERR_CANT_OPEN_DEVSPACE, szPhysDevName, rc ));
      DBG1   ( (MF__, "Cannot open devspace '%s', rc = %d", szPhysDevName, rc ));
      *pDevHandle = INVALID_HANDLE_VALUE;

      DBGOUT;
      return rc;
      }

    *pIsLocked = false;
    if ( !isForcedReadOnly )
    {
        if ( XPARAM(fSetVolumeLock) )
        {
            rc = sql54k_lock_drive ( *pDevHandle );

            if (rc == ERROR_DRIVE_LOCKED)
            {
              *pulErrCode = ERRCODE_DEVIO_DEVSPACE_LOCKED;
              sql46c_build_error_string ( pcErrText, ERRMSG_DEVIO_DEVSPACE_LOCKED, 0 );
              MSGALL (( ERR_DEVSPACE_LOCKED, szPhysDevName ));
              DBG1   ( (MF__, "Devspace '%s' locked by another process", szPhysDevName ));

              CLOSE_FILE( *pDevHandle );
              *pDevHandle = INVALID_HANDLE_VALUE;

              DBGOUT;
              return rc;
            }
            else if (rc != 0)
            {
              *pulErrCode = ERRCODE_DEVIO_LOCK_DEVSPACE;
              sql46c_build_error_string ( pcErrText, ERRMSG_DEVIO_LOCK_DEVSPACE, rc );
              MSGALL (( ERR_CANT_LOCK_DEVSPACE, szPhysDevName, rc ));
              DBG1   ( (MF__, "Cannot lock devspace '%s', rc = %d", szPhysDevName, rc ));

              CLOSE_FILE( *pDevHandle );
              *pDevHandle = INVALID_HANDLE_VALUE;

              DBGOUT;
              return rc;
            }
            *pIsLocked = true;
        }
    }
    break ;

  default :

    rc = sql44c_get_file_info ( szPhysDevName, &lAtt, &ulTmp, &ulTmp );

    if ( rc == ERROR_ACCESS_DENIED )
      {
      *pulErrCode = ERRCODE_DEVIO_ACCESS_DENIED;
      sql46c_build_error_string ( pcErrText, ERRMSG_DEVIO_ACCESS_DENIED, 0 );
      MSGALL(( ERR_DEVSPACE_ACCESS_DENIED, pszLogDevName ));
      DBG1  (( MF__, "Access on devspace '%s' denied", pszLogDevName ));
      DBGOUT;
      return rc;
      }
    else if (rc == ERROR_PATH_NOT_FOUND)
      {
      *pulErrCode = ERRCODE_DEVIO_WRONG_PATH;
      sql46c_build_error_string (pcErrText, ERRMSG_DEVIO_WRONG_PATH, 0 );
      MSGALL(( ERR_WRONG_PATH, pszLogDevName ));
      DBG1  (( MF__, "Wrong path '%s'", pszLogDevName ));
      DBGOUT;
      return rc;
      }
    else if ( rc == ERROR_FILE_NOT_FOUND )
      {
      rc = NO_ERROR;
      }
    else if ( rc != NO_ERROR )
      {
      *pulErrCode = ERRCODE_DEVIO_GET_FILE_INFO;
      sql46c_build_error_string (pcErrText, ERRMSG_DEVIO_GET_FILE_INFO, rc );
      MSGALL(( ERR_GET_FILE_INFO, pszLogDevName, rc ));
      DBG1  (( MF__, "Query file info error on '%s', rc = %ld",
               pszLogDevName, rc ));
      DBGOUT;
      return rc;
      }

    if ( lAtt == -1 )
      {
      rc          = -1;
      *pulErrCode = ERRCODE_DEVIO_WRONG_FILE_ATTRIB;
      MSGALL(( ERR_UNKNOWN_FILE_ATTRIBUTE, pszLogDevName ));
      DBG1  (( MF__, "'%s' has a unknown file attribute", pszLogDevName  ));
      sql46c_build_error_string ( pcErrText, ERRMSG_DEVIO_WRONG_FILE_ATTRIB, 0 );
      }
    else if ((lAtt & FILE_DIRECTORY) == FILE_DIRECTORY)
      {
      rc          = -1;
      *pulErrCode = ERRCODE_DEVIO_DEVSPACE_ON_DIR;
      MSGALL(( ERR_IO_ON_DIR, pszLogDevName ));
      DBG1  (( MF__, "No Devspaces on directory '%s' !", pszLogDevName ));
      sql46c_build_error_string ( pcErrText, ERRMSG_DEVIO_DEVSPACE_ON_DIR, 0 );
      }
    else if ((lAtt & FILE_SYSTEM) == FILE_SYSTEM)
      {
      rc          = -1;
      *pulErrCode = ERRCODE_DEVIO_DEVSPACE_ON_SYS;
      MSGALL(( ERR_IO_ON_SYS, pszLogDevName ));
      DBG1  (( MF__, "No Devspaces on system file '%s' !", pszLogDevName ));
      sql46c_build_error_string ( pcErrText, ERRMSG_DEVIO_DEVSPACE_ON_SYS, 0 );
      }
    else if ((lAtt & FILE_READONLY) == FILE_READONLY)
      {
      rc          = -1;
      *pulErrCode = ERRCODE_DEVIO_DEVSPACE_ON_RD_ONLY;
      MSGALL(( ERR_DEVSPACE_ON_RD_ONLY, pszLogDevName ));
      DBG1  (( MF__, "No Devspaces on read-only file '%s' !",
                      pszLogDevName  ));
      sql46c_build_error_string (pcErrText, ERRMSG_DEVIO_DEVSPACE_ON_RD_ONLY, 0 );
      }
    else if (((lAtt & FILE_COMPRESSED) == FILE_COMPRESSED) &&
			 (  XPARAM(fAllowCompressedDevsp) == false ))
      {
      rc          = -1;
      *pulErrCode = ERRCODE_DEVIO_DEVSPACE_ON_COMP;
      MSGALL(( ERR_DEVSPACE_COMPRESSED, pszLogDevName ));
      DBG1  (( MF__, "No Devspaces on compressed file '%s' !",
                      pszLogDevName  ));
      sql46c_build_error_string (pcErrText, ERRMSG_DEVIO_DEVSPACE_COMPRESSED, 0 );
      }
    else
      {
      DBG3 (( MF__, "Opening devspace: %s", pszLogDevName ));

      *pDevHandle = __sql54k_create_file( szPhysDevName,
                                          isForcedReadOnly ? (GENERIC_READ) : (GENERIC_READ | GENERIC_WRITE ),
                                          FILE_SHARE_READ   |
                                          FILE_SHARE_WRITE,
                                          NULL,
                                          OPEN_EXISTING,
                                          dwFlagsAndAttributes,
                                          NULL );

      if (*pDevHandle == INVALID_HANDLE_VALUE)
        rc = GetLastError();

      if ( rc == ERROR_ACCESS_DENIED )
        {
        *pulErrCode = ERRCODE_DEVIO_ACCESS_DENIED;
        MSGALL(( ERR_DEVSPACE_ACCESS_DENIED, pszLogDevName ));
        DBG1  (( MF__, "Access on devspace '%s' denied", pszLogDevName ));
        sql46c_build_error_string ( pcErrText, ERRMSG_DEVIO_ACCESS_DENIED, 0 );
        *pDevHandle = INVALID_HANDLE_VALUE;
        }
      else if ( rc != NO_ERROR )
        {
        *pulErrCode = ERRCODE_DEVIO_OPEN_DEVSPACE;
        sql46c_build_error_string ( pcErrText, ERRMSG_DEVIO_OPEN_DEVSPACE, rc );
        MSGALL(( ERR_CANT_OPEN_DEVSPACE, pszLogDevName, rc ));
        DBG1  (( MF__, "Cannot open devspace '%s', rc = %d", pszLogDevName, rc ));
        *pDevHandle = INVALID_HANDLE_VALUE;
        }
      }
    }

  DBGOUT;
  return rc;
  }

/*------------------------------*/

LONG     sql54k_dev_close        ( HANDLE                    DevHandle,
                                   SAPDB_Bool               *pIsLocked,
                                   char                     *pszDriveName,
                                   LONG                      lDevspaceType,
                                   ERRORTEXT                 errtext)
{
# undef  MF__
# define MF__ MOD__"sql54k_dev_close"
  LONG                 rc        = NO_ERROR;

  switch ( lDevspaceType )
    {
      case DT_DRIVE :
           if ( *pIsLocked )
           {
               rc = sql54k_unlock_drive ( DevHandle );
               if (rc != 0)
               {
                   MSGALL (( ERR_CANT_UNLOCK_DEVSPACE, pszDriveName, rc ));
                   DBG1   ( (MF__, "Cannot unlock devspace '%s', rc = %d", pszDriveName, rc ));
                   sql46c_build_error_string ( errtext, ERRMSG_DEVIO_UNLOCK_DEVSPACE, rc );
               }
               else
               {
                   *pIsLocked = false;
                   rc = CLOSE_FILE( DevHandle );
               }
           }
           else
           {
               rc = CLOSE_FILE( DevHandle );
           }
           break ;

      default :
           rc = CLOSE_FILE( DevHandle );
    }

  if ( rc != NO_ERROR )
  {
      MSGALL(( ERR_CANT_CLOSE_DEVSPACE, rc ));
      DBG1  (( MF__, "Cannot close devspace, rc = %d", rc ));
      sql46c_build_error_string ( errtext, ERRMSG_DEVIO_CLOSE_DEVSPACE, rc );
  }

  DBGOUT;
  return rc;
  }

/*------------------------------*/

/*
 * May only be called by the DEV0 thread
 * Assumes that the devspace number is known.
 * Thus only opens the devspace, creates the DEVI thread and updates
 * the 'DEVI_CTRL_REC'.
 */
static VOID sql54k_dev0_vattach ( DLQ_VATTACH_REQ_ARGS_REC *pVattachArgs )
  {
# undef  MF__
# define MF__ MOD__"sql54k_dev0_vattach"
  PDEVI_CTRL_REC                   pDeviCtrl;
  LONG                             rc;
  ULONG                            ulWaitTime;
  LONG                             lNDevi;
  LONG                             lDevspaceNo;
  ULONG                            ulTmp;
  ERRORTEXT                        DummyErrText;
  bool                             fFirstTime       = true;
  bool                             fIsTraceDevspace = false;
  PDEV_IO_CTRL_REC                 pIOCtrl;
  SAPDB_Bool                       isForcedReadOnly;
  SAPDB_Bool                       isLockAllowed;

  DBGIN;

  DBG3 (( MF__, "Find free devspace number for '%s'", pVattachArgs->pszDevspaceToAttach ));

  // --- get free devspace number
  pVattachArgs->fOk = sql54k_find_device_no ( &lDevspaceNo,
                                                pVattachArgs->pszDevspaceToAttach,
                                                &pVattachArgs->ulErrCode );
  if ( pVattachArgs->fOk == true )
  {
    pIOCtrl                   = &kgs.Dev0.pIOCtrl[lDevspaceNo];

    fIsTraceDevspace  = !strcmp( pVattachArgs->pszDevspaceToAttach, XPARAM(szTraceDevspace));

    isForcedReadOnly = ( pIOCtrl->DevspaceType == sp2dt_log
                   || pIOCtrl->DevspaceType == sp2dt_mirr_log ) 
                   && RTEHSS_IsLogReadOnly();

    // Lock is allowed, if SetVolumeLock is true, device is no drive and no trace devspace
    isLockAllowed = XPARAM(fSetVolumeLock)
               && ( pIOCtrl->ulDevType != DT_DRIVE ) 
               && ( !fIsTraceDevspace );

    // Lock is not allowed for any LOG VOLUMES in HotStandby configuration
    if ( KGS->isHotStandbyConfiguration
      && ( pIOCtrl->DevspaceType == sp2dt_log || pIOCtrl->DevspaceType == sp2dt_mirr_log ) )
    {
        isLockAllowed = false;
    }

    pVattachArgs->lDevspaceNo = lDevspaceNo;

    sql70k_init_dev_io ( pIOCtrl );
    strcpy ( pIOCtrl->szDevspace, pVattachArgs->pszDevspaceToAttach );

    //
    //  - Set number of DEVi threads and save its devspace number
    //    If it's the trace devspace use only 1 DEVi thread
    //
    lNDevi            = fIsTraceDevspace ? 1 : XPARAM(lDeviPerDevspace) ;

    DBG3 (( MF__, "Creating %d DEVis for '%s', DevspaceNo: %d",
            lNDevi, pIOCtrl->szDevspace, lDevspaceNo ));

    do
      {
      // --- evaluate the DEVi control record
      pDeviCtrl = kgs.pFirstDeviCtrl                     +
                  lDevspaceNo * XPARAM(lDeviPerDevspace) +
                  lNDevi - 1;

      DBG3 (( MF__, "Using DEVi slot     %d",   lNDevi - 1 ));
      DBG3 (( MF__, "pDeviCtrl is        0x%x", pDeviCtrl ));

      // --- init the devi control record
      sql70k_init_devi_ctrl ( pDeviCtrl, pDeviCtrl->pIOs );

      // --- set devspace related DEVi number and devspace number
      pDeviCtrl->pIOCtrl    = pIOCtrl;
      pIOCtrl->lDevspaceNo  = lDevspaceNo;
      pIOCtrl->ulDBPageSize = pVattachArgs->ulDBPageSize ;

      // --- open the devspace
      rc = sql54k_dev_open ( pIOCtrl->szDevspace,
                             lDevspaceNo,
                             isForcedReadOnly,
                             // PTS 1110743 --- used now on all devspaces
                             DEV_NO_BUFFERING,
                             &pDeviCtrl->DevHandle,
                             &pDeviCtrl->isLocked,
                             DummyErrText,
                             &pVattachArgs->ulErrCode,
                             &pIOCtrl->ulDevType );

      if ( rc != NO_ERROR )
        {
        pVattachArgs->fOk  = false;
        sql70k_init_dev_io    ( pIOCtrl );

        // --- kill all already started DEVi threads
        sql54k_devi_kill (lDevspaceNo);

        DBGOUT;
        return;
        }
      //
      // --- Check block 0, get devspace size and protect file
      //     devspaces against attaches from other databases.
      //
      if ( fFirstTime == true )
        {
        fFirstTime = false;

        rc = sql54k_evaluate_block_0 ( pDeviCtrl->DevHandle,
                                       &pIOCtrl->ulDevPages,
                                       &ulTmp,
                                       pIOCtrl->ulDBPageSize,
                                       ( pVattachArgs->accessMode == RTE_VolumeAccessModeSequential )
                                    || ( pIOCtrl->ulDevType == DT_DRIVE ),
                                       pIOCtrl->szDevspace );
        if ( rc != NO_ERROR )
          {
          ERRORTEXT   errtext ;
          pVattachArgs->fOk                   = false;
          pVattachArgs->ulErrCode             = ERRCODE_DEVIO_EVAL_DEVSPACE;

          sql54k_dev_close ( pDeviCtrl->DevHandle,
                             &pDeviCtrl->isLocked,
                             pIOCtrl->szDevspace,
                             pIOCtrl->ulDevType,
                             errtext) ;

          sql70k_init_devi_ctrl ( pDeviCtrl, pDeviCtrl->pIOs );
          sql70k_init_dev_io    ( pIOCtrl );

          DBGOUT;
          return;
          }
        //
        // --- protect file volumes against attaches from other databases
        //
          if ( isLockAllowed )
          {
              rc = os54_set_volume_lock ( pDeviCtrl->DevHandle,
                                         &pDeviCtrl->isLocked,
                                          pIOCtrl->szDevspace
                                    );
              if ( rc != NO_ERROR )
              {
                  pVattachArgs->fOk       = false;
                  pVattachArgs->ulErrCode = ERRCODE_DEVIO_LOCK;

                  CLOSE_FILE( pDeviCtrl->DevHandle );

                  sql70k_init_devi_ctrl ( pDeviCtrl, pDeviCtrl->pIOs );
                  sql70k_init_dev_io    ( pIOCtrl );

                  DBGOUT;
                  return;
              }
          }
          else
          {
              pDeviCtrl->isLocked = false;
          }
        }

        // --- Create event semaphore (SET) for DEVi
        rc = sql41c_create_event_sem ( &pDeviCtrl->ThrdCtrlHeader.hevSem,
                                       NULL,
                                       NULL,
                                       0,
                                       false,
                                       NULL );
      if (rc == NO_ERROR)
      {
          // --- create DEVi-Thread
          rc = sql81k_create_kernel_thread( "DEVi"
                                          , &pDeviCtrl->ThrdCtrlHeader.Tid
                                          , &pDeviCtrl->ThrdCtrlHeader.hThrd
                                          , sql54k_devi
                                          , pDeviCtrl // argument
                                          , KT_DEVI_STACK_SIZE
                                          , false );
           if (rc != NO_ERROR)
           {
              sql41c_close_event_sem ( pDeviCtrl->ThrdCtrlHeader.hevSem, "DEVi" );
              pDeviCtrl->ThrdCtrlHeader.hevSem = (HEV)INVALID_HANDLE_VALUE;
           }
      }
      else
      {
          pDeviCtrl->ThrdCtrlHeader.hevSem = (HEV)INVALID_HANDLE_VALUE;
      }

      if ( rc == NO_ERROR )
        {
        // --- wait until the DEVi is up and running
        for ( ulWaitTime = MAX_VATTACH_WAIT_TIME * 10;
              (pDeviCtrl->ThrdCtrlHeader.ThrdState == KT_INITIALIZED) && ulWaitTime;
              --ulWaitTime )
          {
          SLEEP( 1000 / 10 ); // -- sleep a moment
          }
        }

      if (( rc != NO_ERROR ) || ( ulWaitTime == 0 ))
        {
        if ( ulWaitTime == 0 )
          pVattachArgs->ulErrCode  = ERRCODE_DEVIO_DEVICREA_TIMEOUT;
        else
          pVattachArgs->ulErrCode  = ERRCODE_DEVIO_DEVICREA;

        pVattachArgs->fOk = false;

        CLOSE_FILE( pDeviCtrl->DevHandle );

        sql70k_init_dev_io    ( pIOCtrl );

        // --- kill all already started DEVi threads
        sql54k_devi_kill (lDevspaceNo);

        DBGOUT;
        return;
        }

      DBG3 (( MF__, "DEVi TID:%d attached", pDeviCtrl->ThrdCtrlHeader.Tid ));
      }
    while ( (--lNDevi) > 0 );

     // --- use overlapped io?
     if ( kgs.ulNumOfDevWorker != 0 )
       {
       // --- open the devspace
       rc = sql54k_dev_open ( pIOCtrl->szDevspace,
                              lDevspaceNo,
                              isForcedReadOnly,
                              // PTS 1110743 --- used now on all devspaces
                              DEV_OVERLAPPED_IO | DEV_NO_BUFFERING, 
                              &pIOCtrl->DevHandle,
                              &pIOCtrl->isLocked,
                              DummyErrText,
                              &pVattachArgs->ulErrCode,
                              &pIOCtrl->ulDevType );

       if ( rc != NO_ERROR )
         {
         pVattachArgs->fOk  = false;
         sql70k_init_dev_io ( pIOCtrl );

         // --- kill all already started DEVi threads
         sql54k_devi_kill (lDevspaceNo);
         DBGOUT;
         return;
         }

       pIOCtrl->hCompletionPort =
                    CreateIoCompletionPort( (HANDLE)pIOCtrl->DevHandle,
                                            kgs.hCompletionPort,
                                            (DWORD)lDevspaceNo, 0);

       if (pIOCtrl->hCompletionPort == NULL)
         {
         pVattachArgs->ulErrCode  = ERRCODE_DEVIO_CREATE_COMPL_PORT;
         pVattachArgs->fOk        = false;
         sql70k_init_dev_io ( pIOCtrl );

         // --- kill all already started DEVi threads
         sql54k_devi_kill (lDevspaceNo);
         DBGOUT;
         return;
         }
       }

    pIOCtrl->ulAttached   = kgs.ulCurrTime;
    pIOCtrl->DevspaceType = pVattachArgs->DevspaceType;
    pIOCtrl->accessMode   = pVattachArgs->accessMode;

    if ( fIsTraceDevspace == true )
      kgs.Dev0.lTraceDevspaceNo = lDevspaceNo;
  }

  DBGOUT;
  return;
}

/*------------------------------*/

static VOID sql54k_devi_kill ( LONG lDevspaceNo )
  {
# undef  MF__
# define MF__ MOD__"sql54k_devi_kill"
  LONG                   lNDevi = XPARAM(lDeviPerDevspace);
  PDEVI_CTRL_REC         pDeviCtrl;
  ERRORTEXT              errtext;

  DBGIN;

  do
    {
    // --- evaluate the DEVi control record
    pDeviCtrl = kgs.pFirstDeviCtrl                     +
                lDevspaceNo * XPARAM(lDeviPerDevspace) +
                lNDevi - 1;

    KILL_THREAD ( pDeviCtrl->ThrdCtrlHeader.Tid,
                  pDeviCtrl->ThrdCtrlHeader.hThrd );

    CloseHandle(pDeviCtrl->ThrdCtrlHeader.hThrd);

    if (pDeviCtrl->ThrdCtrlHeader.ThrdState != KT_INITIALIZED)
      {
      DBG3 (( MF__, "Killing DEVi Tid:%u, pDeviCtrl is 0x%u",
              pDeviCtrl->ThrdCtrlHeader.Tid, pDeviCtrl ));

      if ( pDeviCtrl->DevHandle != INVALID_HANDLE_VALUE )
        sql54k_dev_close ( pDeviCtrl->DevHandle,
                           &pDeviCtrl->isLocked,
                           pDeviCtrl->pIOCtrl->szDevspace,
                           pDeviCtrl->pIOCtrl->ulDevType , errtext) ;

      if ( pDeviCtrl->ThrdCtrlHeader.hevSem != (HEV) INVALID_HANDLE_VALUE )
        {
        sql41c_close_event_sem ( pDeviCtrl->ThrdCtrlHeader.hevSem, "DEVi" );
        {
          SAPDB_ULong SystemSize = ((KT_DEVI_STACK_SIZE + (1024*1024) - 1)&(~((1024*1024) - 1)));
          RTE_PseudoFreeStack(SystemSize, 0);
        }
        }
      }

    sql70k_init_devi_ctrl ( pDeviCtrl, pDeviCtrl->pIOs );
    }
  while ( (--lNDevi) > 0 );

  DBGOUT;
  return;
  }

/*------------------------------*/

static VOID  sql54k_dev0_vdetach  ( DLQ_VDETACH_REQ_ARGS_REC  *pVdetachArgs,
                                    PDLQ_REC                   pRequest)
  {
# undef  MF__
# define MF__ MOD__"sql54k_dev0_vdetach"
  PDEVI_CTRL_REC            pDeviCtrl;
  LONG                      lDevspaceNo;
  LONG                      lNDevi;
  TID                       Tid;
  HANDLE                    hThrd;
  LONG                      rc = NO_ERROR;
  PDEV_IO_CTRL_REC          pIOCtrl;
  ULONG                     ulWaitTime = MAX_VDETACH_WAIT_TIME;
  ERRORTEXT                 errtext;

  lDevspaceNo  = pVdetachArgs->lDevspaceNo;
  lNDevi       = XPARAM(lDeviPerDevspace);
  pIOCtrl      = &kgs.Dev0.pIOCtrl[lDevspaceNo];

  // --- signal that a DETACH is in progress
  pIOCtrl->ulDetachPending = kgs.ulCurrTime;

  if ( kgs.Dev0.lTraceDevspaceNo == lDevspaceNo )
    kgs.Dev0.lTraceDevspaceNo = UNDEF;

  do
    {
    // --- evaluate the DEVi control record
    pDeviCtrl = kgs.pFirstDeviCtrl                     +
                lDevspaceNo * XPARAM(lDeviPerDevspace) +
                lNDevi - 1;

    if (( pDeviCtrl->ThrdCtrlHeader.ThrdState != KT_INITIALIZED ) &&
        ( pDeviCtrl->ThrdCtrlHeader.ThrdState != KT_TERMINATING ))
      {
      DBG3 (( MF__, "Detaching DEVi slot %d",   lNDevi - 1 ));
      DBG3 (( MF__, "pDeviCtrl is        0x%x", pDeviCtrl ));

      //
      // --- wait until the DEVi has stop the activities
      //
      while (( pDeviCtrl->ThrdCtrlHeader.ThrdState != KT_SLEEPING ) ||
             ( pDeviCtrl->pIOWorkQue               != pDeviCtrl->pIOReqQue ))
        {
        DBG3 (( MF__, "ThrdCtrlHeader.ThrdState = %d",   pDeviCtrl->ThrdCtrlHeader.ThrdState ));
        DBG3 (( MF__, "pIOWorkQue               = 0x%x", pDeviCtrl->pIOWorkQue ));
        DBG3 (( MF__, "pIOReqQue                = 0x%x", pDeviCtrl->pIOReqQue ));
        SLEEP( 100 );
        }

      os54_WaitForSelfIOCompletionAndClose(lDevspaceNo);

      //
      // --- awaken devi and wait for its termintion
      //
      DBG3(( MF__,
             "(Dev0) Send pRequest: 0x%x, using (Devi TID:%u) pIOReqQue: 0x%x",
             pRequest, pDeviCtrl->ThrdCtrlHeader.Tid,
             pDeviCtrl->pIOReqQue ));

      Tid   = pDeviCtrl->ThrdCtrlHeader.Tid;
      hThrd = pDeviCtrl->ThrdCtrlHeader.hThrd;

      REQ_TO_DEVI ( pDeviCtrl, pRequest );

      WAIT_THREAD(hThrd, ulWaitTime);
      }
    }
  while ( (--lNDevi) > 0 );

  // --- overlapped io used?
  if ( kgs.ulNumOfDevWorker != 0 )
    rc = sql54k_dev_close ( pIOCtrl->DevHandle,
                            &pIOCtrl->isLocked,
                            pIOCtrl->szDevspace,
                            pIOCtrl->ulDevType , errtext) ;

  if ( kgs.Dev0.lTraceDevspaceNo == lDevspaceNo )
    kgs.Dev0.lTraceDevspaceNo = (LONG)UNDEF;

  sql70k_init_dev_io ( pIOCtrl );
  kgs.fSingleIOOpenLocked  = false;

  DBGOUT;
  return;
  }

/*------------------------------*/

/*
 * The main loop for the DEVI process, a request is
 * always received and always returned
 */
static VOID _System sql54k_devi ( PDEVI_CTRL_REC  pDeviCtrl )
  {
# undef  MF__
# define MF__ MOD__"sql54k_devi"
  LONG                          rc = NO_ERROR;
  BOOL                          fDetached;
  PDLQ_REC                      pRequest;
  HANDLE                        hThrd;
  PDEV_IO_CTRL_REC              pIOCtrl;

  DBGIN;

  __try
  {
#   if defined(_FASTCAP)
     CAPNameThread ("Devi");
#   endif
    // --- save the current thread handle which is used by 'EXITTHREAD'.
    //     Set the Pointer to the IO Control structure.
    hThrd   = pDeviCtrl->ThrdCtrlHeader.hThrd;
    pIOCtrl = pDeviCtrl->pIOCtrl;

    // --- set the thread priority
    rc = SET_THREAD_PRIO ( pDeviCtrl->ThrdCtrlHeader.lThrdPrio,
                           pDeviCtrl->ThrdCtrlHeader.Tid,
                           pDeviCtrl->ThrdCtrlHeader.hThrd );

    if ( rc != NO_ERROR )
      {
      MSGD (( ERR_SETTING_THRD_PRIO, pDeviCtrl->ThrdCtrlHeader.Tid));
      DBG1 ((MF__, "Cannot set the thread priority, TID: %u",
                    kgs.Dev0.ThrdCtrlHeader.Tid));
      ABORT();
      }

    DBG3 (( MF__, "DEVi TID:%d started", pDeviCtrl->ThrdCtrlHeader.Tid ));
    MSGD (( INFO_DEVI_STARTED, pDeviCtrl->pIOCtrl->szDevspace));

    // --- set thread control info
    pDeviCtrl->ThrdCtrlHeader.ThrdState   = KT_RUNNING;
    pDeviCtrl->ThrdCtrlHeader.ulStartTime = kgs.ulCurrTime;

    // --- Runs until detached
    for ( fDetached = false; fDetached == false ; )
      {
      pRequest = sql54k_dev_receive (&pDeviCtrl->pIOWorkQue,
                                     &pDeviCtrl->DevIOStat.ulIOQueLen,
                                     &pDeviCtrl->lExclusive,
                                     &pDeviCtrl->ThrdCtrlHeader,
                                     &pDeviCtrl->DevIOStat);

      switch ( pRequest->ulReqType )
        {
        case REQ_VBLOCKIO:
          sql54k_devi_io ( pRequest, pDeviCtrl );

          // -- send reply back to UKT
          REQ_FROM_DEVx_TO_UKT(pRequest);
          break;

        case REQ_VDETACH :
          pDeviCtrl->ThrdCtrlHeader.ThrdState = KT_TERMINATING;

          sql54k_devi_vdetach  ( pDeviCtrl );

          fDetached = true; // --- exit for loop
          break;

        default          :
          MSGD (( ERR_ILLEGAL_REQUEST, "DEVi", pRequest->ulReqType ));

          // -- send reply back to UKT
          REQ_FROM_DEVx_TO_UKT(pRequest);
          break;
        }

      DBG3 (( MF__, "next in loop"));
      }
  }
  __except( sql50k_excepthandler(GetExceptionInformation()) ) {;}

  DBGOUT;

  {
      SAPDB_ULong SystemSize = ((KT_DEVI_STACK_SIZE + (1024*1024) - 1)&(~((1024*1024) - 1)));
      RTE_PseudoFreeStack(SystemSize, 0);
  }
  EXITTHREAD ( 0, hThrd );

  return;
  }


/*------------------------------*/

static VOID sql54k_devi_vdetach ( PDEVI_CTRL_REC    pDeviCtrl )
  {
# undef  MF__
# define MF__ MOD__"sql54k_devi_vdetach"
  LONG       rc = NO_ERROR;
  ERRORTEXT  errtext;

  DBG3 (( MF__, "Detaching DEVi   TID:%d", pDeviCtrl->ThrdCtrlHeader.Tid ));


  rc = sql54k_dev_close ( pDeviCtrl->DevHandle,
                          &pDeviCtrl->isLocked,
                          pDeviCtrl->pIOCtrl->szDevspace,
                          pDeviCtrl->pIOCtrl->ulDevType , errtext) ;

  sql41c_close_event_sem ( pDeviCtrl->ThrdCtrlHeader.hevSem, "DEVi" );

  MSGD (( INFO_DEVI_STOPPED, pDeviCtrl->pIOCtrl->szDevspace ));

  sql70k_init_devi_ctrl ( pDeviCtrl, pDeviCtrl->pIOs );

  DBGOUT;
  return;
  }

/*------------------------------*/

static LONG os54_set_volume_lock ( HANDLE         FileHandle,
                                   SAPDB_Bool    *pIsLocked,
                                   PSZ            pszLogDevName
                                 )
{
# undef  MF__
# define MF__ MOD__"os54_set_volume_lock"
    ULONG          LockTimeout = 1;
    LONG           rc          = NO_ERROR;
    OVERLAPPED     Overlapped;
    
    DBGPAS;
    
    memset ( &Overlapped, 0, sizeof(Overlapped) );
    
    if ( !LockFileEx( (HANDLE)FileHandle, LOCKFILE_FAIL_IMMEDIATELY | LOCKFILE_EXCLUSIVE_LOCK,
        0, DEVSPACE_ID_POS + sizeof(DEVSPACE_ID_STRING) - 1, 0,
        &Overlapped ))
    {
        rc = GetLastError();
    }
    
    if ( rc == ERROR_LOCK_VIOLATION )
    {
        MSGALL(( ERR_DEVSPACE_IN_USE, pszLogDevName ));
        DBG1  (( MF__, "Devspace '%s' in use", pszLogDevName));
    }
    else if ( rc != NO_ERROR )
    {
        MSGALL(( ERR_CANT_LOCK_UNLOCK_DEVSPACE, pszLogDevName, rc ));
        DBG1  (( MF__, "Cannot lock the devspace '%s', rc = %d",
            pszLogDevName, rc ));
    }
    else
    {
        *pIsLocked = true;
    }
    
    return rc;
}

/*------------------------------*/

/*
 *  May not be called in the DEV0 thread, only in the DEVi thread
 *  This is the mother thread controlling the devspace 'i', all
 *  requests are addressed to it.
 */
static VOID sql54k_devi_io ( PDLQ_REC          pRequest,
                             PDEVI_CTRL_REC    pDeviCtrl )
{
# undef  MF__
# define MF__ MOD__"sql54k_devi_io"
    DLQ_IO_REQ_ARGS_REC  *pIOArgs;

    DBGIN;

  // --- get pointer to the arguments
    pIOArgs = &pRequest->ReqArgs.IORequest;

    DBG3 (( MF__, "'io' for lDevspaceNo: %d, lDevspacePage %d",
          pIOArgs->lDevspaceNo , pIOArgs->lDevspacePage ));
 
    if ( __sql54k_is_user_req_io_error(pIOArgs->lDevspaceNo, pIOArgs->OpCode) )
    {
        /*
         *  Simulate an I/O error for test porposes.
         */
        pIOArgs->fOk            = false ;
        pIOArgs->ulErrCode      = ERRCODE_DEVIO_USER_REQ_ERROR ;

        MSGD ((ERR_TEST_IO_ERROR,
              ( pIOArgs->OpCode == DEV_READ) ? "reading from" : "writing to" ,
                pIOArgs->lDevspaceNo , pIOArgs->lDevspacePage ));
    }
    else
    {
        teo52_IOStatistic *pDevStat;
        if ( pIOArgs->OpCode == DEV_READ )
        {
            pDevStat = & (pDeviCtrl->DevIOStat.DevRead) ;
        }
        else
        {
            pDevStat = & (pDeviCtrl->DevIOStat.DevWrite) ;
        }

        os54_UpdateCountInIOStatisticRec(pDevStat, pIOArgs->sNumOfPages);

        if ( XPARAM(fTimeMeasure) )
        {
            SAPDB_UInt8  BegMicroSeconds, deltaMicroSeconds;

            BegMicroSeconds = RTESys_MicroSecTimer();
            if ( pIOArgs->OpCode == DEV_READ )
            {
                sql54k_devi_read  ( pDeviCtrl, pIOArgs );
            }
            else
            {
                sql54k_devi_write ( pDeviCtrl, pIOArgs );
            }
         
            deltaMicroSeconds = RTESys_MicroSecTimer() - BegMicroSeconds;
            pDevStat->IOStat.TotalTimeStat.ulAbsMicroSeconds += deltaMicroSeconds;
            pDevStat->IOStat.TotalTimeStat.ulMeasurements++;
            pDevStat->IOStat.TimeStat.ulAbsMicroSeconds += deltaMicroSeconds;
            pDevStat->IOStat.TimeStat.ulMeasurements++;
        }
        else
        {
            if ( pIOArgs->OpCode == DEV_READ )
            {
                sql54k_devi_read  ( pDeviCtrl, pIOArgs );
            }
            else
            {
                sql54k_devi_write ( pDeviCtrl, pIOArgs );
            }
        }
    }
    DBG3 (( MF__, "ok = %f", pIOArgs->fOk ));

    DBGOUT;
    return;
}


/*------------------------------*/

static LONG sql54k_uncompress( HANDLE      FileHandle )
  {
# undef  MF__
# define MF__ MOD__"sql54k_uncompress"
  ULONG                 rc = NO_ERROR;
  USHORT				State = 0;
  ULONG					Length;

  DBGIN;

  if (!DeviceIoControl( FileHandle, FSCTL_SET_COMPRESSION, &State,
					    sizeof(USHORT), NULL, 0, &Length, false ))
	{
    rc = GetLastError();
	}

  DBGOUT;
  return rc;
  }

/*------------------------------*/

static LONG sql54k_vmark_bad_dev ( DLQ_MARK_BAD_REQ_ARGS_REC*  pMarkBadDevRequest )
  {
# undef  MF__
# define MF__ MOD__"sql54k_vmark_bad_dev"
  ULONG                           rc,
                                  ulFilePosHigh = 0;
  ULONG                           ulTmp;
  ULONG                           ulLength;
  ERRORTEXT                       pcTmpErrText;
  HANDLE                          DevHandle;
  PDEV_IO_CTRL_REC                pIOCtrl;
  ULONG                           lDevspaceType;
  SAPDB_Bool                      isLocked = false;
  DBGIN;

  pIOCtrl = &kgs.Dev0.pIOCtrl[pMarkBadDevRequest->lDevspaceNo];

  rc = sql54k_dev_open ( pIOCtrl->szDevspace,
                         pIOCtrl->lDevspaceNo,
                         false,
                         0,
                         &DevHandle,
                         &isLocked,
                         pcTmpErrText,
                         &ulTmp,
                         &lDevspaceType );

  if ( rc != NO_ERROR )
    return rc;


  rc = SET_FILE_PTR ( DevHandle, DEVSPACE_BAD_MARK_POS,
                      FILE_BEGIN, &ulFilePosHigh, NULL );

  if ( rc != NO_ERROR )
    {
    MSGD(( ERR_CANT_SEEK_ON_DEVSPACE, pIOCtrl->szDevspace, rc ));
    CLOSE_FILE( DevHandle );
    return rc;
    }

  rc = (WriteFile(DevHandle, &pMarkBadDevRequest->lBadDevspacePage,
                  sizeof(tsp00_PageNo), &ulLength, NULL) ? NO_ERROR : GetLastError());

  if ( ( rc == NO_ERROR ) && ( ulLength != sizeof(tsp00_PageNo)) )
    rc = ERROR_MORE_DATA;

  if ( rc != NO_ERROR )
    MSGD((ERR_CANT_WRITE_ON_DEVSPACE, 0, pIOCtrl->szDevspace, rc ))
  else
    MSGALL (( ERR_MARKING_DEVSP_AS_BAD, pIOCtrl->szDevspace,pMarkBadDevRequest->lBadDevspacePage ))

  CLOSE_FILE( DevHandle );

  DBGOUT;
  return rc;
  }

/*------------------------------*/

/*
 * This function performs read io on the devspace
 */
static LONG  sql54k_devi_read ( PDEVI_CTRL_REC       pDeviCtrl,
                                DLQ_IO_REQ_ARGS_REC  *pIOArgs )
  {
# undef  MF__
# define MF__ MOD__"sql54k_devi_read"

  ULONG           ulFilePosLow;
  ULONG           ulFilePosHigh;
  LONG            rc = NO_ERROR;

  DBGIN;

  if ( (ULONG)(pIOArgs->lDevspacePage + 1) != pDeviCtrl->ulOldIOPos )
    {
    PAGES_TO_QWORD( ulFilePosLow, ulFilePosHigh,
                    pIOArgs->lDevspacePage + 1 );

    rc = SET_FILE_PTR ( pDeviCtrl->DevHandle, ulFilePosLow, FILE_BEGIN,
                        &ulFilePosLow, &ulFilePosHigh );

    if (rc != 0)
      {
      MSGALL(( ERR_CANT_SEEK_ON_DEVSPACE,
               pDeviCtrl->pIOCtrl->szDevspace, rc ));
      DBG1  (( MF__, "Cannot seek on devspace: %s, rc = %d",
               pDeviCtrl->pIOCtrl->szDevspace, rc ));
      pIOArgs->ulErrCode = ERRCODE_DEVIO_SEEK;
      pIOArgs->fOk       = false;

      DBGOUT;
      return rc;
      }
    }

  #ifdef DEVELOP_CHECKS
   if ((ULONG)( pIOArgs->lDevspacePage +
                pIOArgs->sNumOfPages   + 1 ) >
                        pDeviCtrl->pIOCtrl->ulDevPages )
     {
     MSGALL(( ERR_WRNG_DEVSPACE_POS,
              pIOArgs->lDevspacePage,
              pIOArgs->sNumOfPages ));
     DBG1  (( MF__, "Wrong devspace read position: %d - Pages: %d",
              pIOArgs->lDevspacePage,
              pIOArgs->sNumOfPages ));

     pIOArgs->ulErrCode = ERRCODE_DEVIO_DEV_POS;
     pIOArgs->fOk       = false;

     DBGOUT;
     return -1;
     }
  #endif

  #ifdef DEVELOP_CHECKS
   rc = __sql54k_check_filepos ( pDeviCtrl->DevHandle,
                                 pIOArgs->lDevspaceNo,
                                 pIOArgs->lDevspacePage,
                                 pIOArgs->sNumOfPages,
                                 pDeviCtrl->pIOCtrl->ulDevPages,
                                 &pIOArgs->ulErrCode,
                                 NULL );

   if ( rc != NO_ERROR )
     {
     pIOArgs->fOk = false;
     DBGOUT;
     return -1; /* changed '-' to '-1' by UweH 2000-01-03 */
     }
  #endif

  rc = __sql54k_io_read_with_retries( pDeviCtrl->DevHandle, pIOArgs->aSegmentArray, pIOArgs->sNumOfPages,
                                      pIOArgs->lDevspacePage + 1,
                                      pDeviCtrl->pIOCtrl->szDevspace, NUMBER_OF_IO_RETRIES, FALSE );

  if ( rc != NO_ERROR )
    {
    DBG1  (( MF__, "Cannot read from devspace '%s', rc = %d",
             pDeviCtrl->pIOCtrl->szDevspace, rc ));

    if ( pIOArgs->sNumOfPages > 1 )
      MSGALL(( ERR_CANT_READ_PAGES_FROM_DEVSPACE, pIOArgs->lDevspacePage + 1,
               pIOArgs->lDevspacePage + pIOArgs->sNumOfPages,
               pDeviCtrl->pIOCtrl->szDevspace,
               rc ))
    else
      MSGALL(( ERR_CANT_READ_FROM_DEVSPACE,
               pIOArgs->lDevspacePage + 1,
               pDeviCtrl->pIOCtrl->szDevspace,
               rc ));

    pIOArgs->ulErrCode = ERRCODE_DEVIO_READ;
    pIOArgs->fOk       = false;

    DBGOUT;
    return (rc != NO_ERROR) ? rc : -1;
    }

  #ifdef DEVELOP_CHECKS
   rc = __sql54k_check_filepos ( pDeviCtrl->DevHandle,
                                 pIOArgs->lDevspaceNo,
                                 pIOArgs->lDevspacePage +
                                 pIOArgs->sNumOfPages,
                                 0,
                                 pDeviCtrl->pIOCtrl->ulDevPages,
                                 &pIOArgs->ulErrCode,
                                 NULL );

   if ( rc != NO_ERROR )
     {
     pIOArgs->fOk = false;
     DBGOUT;
     return rc;
     }
  #endif

  pDeviCtrl->ulOldIOPos = pIOArgs->lDevspacePage +
                          pIOArgs->sNumOfPages   + 1;
  pIOArgs->fOk    = true;

  DBGOUT;
  return rc;
  }

/*------------------------------*/

/*
 * This function performs read io on the devspace
 */
static LONG  sql54k_devi_write ( PDEVI_CTRL_REC        pDeviCtrl,
                                 DLQ_IO_REQ_ARGS_REC  *pIOArgs )
  {
# undef  MF__
# define MF__ MOD__"sql54k_devi_write"

  ULONG           ulFilePosLow;
  ULONG           ulFilePosHigh;
  LONG            rc = NO_ERROR;

  DBGIN;

  if ( (ULONG)(pIOArgs->lDevspacePage + 1) != pDeviCtrl->ulOldIOPos )
    {
    PAGES_TO_QWORD( ulFilePosLow, ulFilePosHigh,
                    pIOArgs->lDevspacePage + 1 );

    rc = SET_FILE_PTR ( pDeviCtrl->DevHandle, ulFilePosLow, FILE_BEGIN,
                        &ulFilePosLow, &ulFilePosHigh );

    if ( rc !=  NO_ERROR )
      {
      MSGALL(( ERR_CANT_SEEK_ON_DEVSPACE,
               pDeviCtrl->pIOCtrl->szDevspace, rc ));
      DBG1  (( MF__, "Cannot seek on devspace: %s, rc = %d",
               pDeviCtrl->pIOCtrl->szDevspace, rc ));
      pIOArgs->ulErrCode = ERRCODE_DEVIO_SEEK;
      pIOArgs->fOk       = false;

      DBGOUT;
      return rc;
      }
    }

  #ifdef DEVELOP_CHECKS
   rc = __sql54k_check_filepos ( pDeviCtrl->DevHandle,
                                 pIOArgs->lDevspaceNo,
                                 pIOArgs->lDevspacePage,
                                 pIOArgs->sNumOfPages,
                                 pDeviCtrl->pIOCtrl->ulDevPages,
                                 &pIOArgs->ulErrCode,
                                 NULL );

   if ( rc != NO_ERROR )
     {
     pIOArgs->fOk = false;
     DBGOUT;
     return rc;
     }
  #endif

  rc = __sql54k_io_write( pDeviCtrl->DevHandle, pIOArgs->aSegmentArray, pIOArgs->sNumOfPages, NULL );

  if ( rc != NO_ERROR )
    {
    DBG1  (( MF__, "Cannot write on devspace '%s', rc = %d",
             pDeviCtrl->pIOCtrl->szDevspace, rc ));

    if ( pIOArgs->sNumOfPages > 1 )
      MSGALL(( ERR_CANT_WRITE_PAGES_ON_DEVSPACE, pIOArgs->lDevspacePage + 1,
               pIOArgs->lDevspacePage + pIOArgs->sNumOfPages,
               pDeviCtrl->pIOCtrl->szDevspace, rc ))
    else
      MSGALL(( ERR_CANT_WRITE_ON_DEVSPACE,
               pIOArgs->lDevspacePage + 1,
               pDeviCtrl->pIOCtrl->szDevspace, rc ));

    pIOArgs->ulErrCode = ERRCODE_DEVIO_WRITE;
    pIOArgs->fOk       = false;

    DBGOUT;
    return (rc != NO_ERROR) ? rc : -1;
    }

  #ifdef DEVELOP_CHECKS
   rc = __sql54k_check_filepos ( pDeviCtrl->DevHandle,
                                 pIOArgs->lDevspaceNo,
                                 pIOArgs->lDevspacePage +
                                 pIOArgs->sNumOfPages,
                                 0,
                                 pDeviCtrl->pIOCtrl->ulDevPages,
                                 &pIOArgs->ulErrCode,
                                 NULL );

   if ( rc != NO_ERROR )
     {
     pIOArgs->fOk = false;
     DBGOUT;
     return rc;
     }
  #endif

  pDeviCtrl->ulOldIOPos = pIOArgs->lDevspacePage +
                          pIOArgs->sNumOfPages   + 1;
  pIOArgs->fOk    = true;

  DBGOUT;
  return rc;
  }

/*------------------------------*/

static VOID sql54k_wake_up_tasks ( PPIO_QUEUE_REC  ppIOWorkQue,
                                   PLONG           plIOWorkExclusive )
  {
# undef  MF__
# define MF__ MOD__"sql54k_wake_up_tasks"

  DBGPAS;

  while ( (*ppIOWorkQue)->pRequest != (PDLQ_REC) 0 )
    {
    // -- send reply back to UKT ( wake up task )
    REQ_FROM_DEVx_TO_UKT((*ppIOWorkQue)->pRequest);

    (*ppIOWorkQue)->pRequest = 0;
    (*ppIOWorkQue)           = (*ppIOWorkQue)->pNextElem;
    }

  return;

  }

/*------------------------------*/

static LONG sql54k_lock_drive ( HANDLE FileHandle )
  {
# undef  MF__
# define MF__  MOD__"sql54k_lock_drive"
  PVOID   ParmList        = NULL;
  ULONG   ParmLengthMax   = 0;
  ULONG   ParmLengthInOut = 0;
  PUCHAR  DataArea        = NULL;
  ULONG   DataLengthMax   = 0;
  ULONG   DataLengthInOut = 0;
  LONG    rc              = NO_ERROR;

  DBGPAS;

   if (!DeviceIoControl((HANDLE)FileHandle, FSCTL_LOCK_VOLUME,
                        NULL, 0, NULL, 0, &DataLengthInOut, NULL))
       rc = GetLastError();

  return rc;
  }

/*------------------------------*/

static LONG sql54k_unlock_drive ( HANDLE FileHandle )
  {
# undef  MF__
# define MF__  MOD__"sql54k_unlock_drive"
  PVOID   ParmList        = NULL;
  ULONG   ParmLengthMax   = 0;
  ULONG   ParmLengthInOut = 0;
  PUCHAR  DataArea        = NULL;
  ULONG   DataLengthMax   = 0;
  ULONG   DataLengthInOut = 0;
  LONG    rc              = NO_ERROR;

  DBGPAS;

  if (!DeviceIoControl((HANDLE)FileHandle, FSCTL_UNLOCK_VOLUME,
                        NULL, 0, NULL, 0, &DataLengthInOut, NULL))
      rc = GetLastError();

  return rc;
  }

/*------------------------------*/

_INLINE VOID __sql54k_req_to_devi ( PDEVI_CTRL_REC  pDeviCtrl,
                                    PDLQ_REC        pRequest )
  {
# undef  MF__
# define MF__  MOD__"__sql54k_req_to_devi"

  teo52_DevIOStatistic *pDeviStatistic = &pDeviCtrl->DevIOStat;
  tsp00_Uint4 ulIOQueLen;

  DBGPAS;

  WAIT_UNTIL_ALONE( &pDeviCtrl->lExclusive );

  pDeviCtrl->pIOReqQue->pRequest = pRequest;
  pDeviCtrl->pIOReqQue           = pDeviCtrl->pIOReqQue->pNextElem;
  ulIOQueLen = ++(pDeviStatistic->ulIOQueLen);

  if (ulIOQueLen > pDeviStatistic->ulMaxIOQueLen)
  {
      pDeviStatistic->ulMaxIOQueLen = ulIOQueLen;
      if ( ulIOQueLen > pDeviStatistic->TotalMaxIOQueLen )
      {
          pDeviStatistic->TotalMaxIOQueLen = ulIOQueLen;
      }
  }

  CLEARLOCK( &pDeviCtrl->lExclusive );

  sql72k_wake_kt ( &pDeviCtrl->ThrdCtrlHeader );
  }

/*------------------------------*/

_INLINE VOID __sql54k_req_from_dev_worker_to_ukt ( BOOL         fOk,
                                                   ULONG        ulNumOfBytes,
                                                   LONG         lDevspaceNo,
                                                   LPOVERLAPPED pCompletedOverlapped,
                                                   LONG         rc )
  {
# undef  MF__
# define MF__  MOD__"__sql54k_req_from_dev_worker_to_ukt"
  PDLQ_REC                       pRequest;
  //
  // --- OVERLAPPED always is first element of TaskOverlappedIO structure while in task context
  //
  TaskOverlappedIO              *pOvIO = (TaskOverlappedIO *)pCompletedOverlapped;

  DBGPAS;

  pRequest                                        = pOvIO->pRequest;
  pRequest->pTaskCtrl                             = pOvIO->pTaskCtrl;
  pRequest->ulReqType                             = REQ_OVERLAPPED_IO_FINISHED;
  pRequest->ReqArgs.DevWorkerRequest.lDevspaceNo  = lDevspaceNo;
  pRequest->ReqArgs.DevWorkerRequest.ulNumOfBytes = ulNumOfBytes;
  pRequest->ReqArgs.DevWorkerRequest.fOk          = fOk;
  pRequest->ReqArgs.DevWorkerRequest.APIRetCode   = rc;

  DBG3 (( MF__, "Send pRequest: 0x%x to TID:%u, [T:0x%03u]", pRequest,
        pRequest->pTaskCtrl->pUKT->ThrdCtrlHeader.Tid,
        pRequest->pTaskCtrl->ulTaskIndex ));

  sql74k_UKT_enqu_and_wake ( pRequest->pTaskCtrl->pUKT, pRequest->pTaskCtrl, pRequest );
  }

/*------------------------------*/

_INLINE VOID __sql54k_req_from_devx_to_ukt ( PDLQ_REC pRequest )
  {
# undef  MF__
# define MF__  MOD__"__sql54k_req_from_devx_to_ukt"

  DBGPAS;

  DBG3 (( MF__, "Send pRequest: 0x%x to TID:%u, [T:0x%03u]", pRequest,
        pRequest->pTaskCtrl->pUKT->ThrdCtrlHeader.Tid,
        pRequest->pTaskCtrl->ulTaskIndex ));

  sql74k_UKT_enqu_and_wake ( pRequest->pTaskCtrl->pUKT, pRequest->pTaskCtrl, pRequest );
  }


/*------------------------------*/

_INLINE VOID __sql54k_req_to_dev0 ( PDLQ_REC        pRequest,
                                    PTASK_CTRL_REC  pTaskCtrl )
  {
# undef  MF__
# define MF__  MOD__"__sql54k_req_to_dev0"

  DBGPAS;

  DBG3(( MF__, "[T:0x%03u] Send pRequest: 0x%x, using (Dev0) pIOReqQue: 0x%x",
        pTaskCtrl->ulTaskIndex, pRequest, kgs.Dev0.pIOReqQue ));

  WAIT_UNTIL_ALONE( &kgs.Dev0.lExclusive );

  kgs.Dev0.pIOReqQue->pRequest = pRequest;
  kgs.Dev0.pIOReqQue           = kgs.Dev0.pIOReqQue->pNextElem;

  CLEARLOCK( & kgs.Dev0.lExclusive );

  sql72k_wake_kt ( &kgs.Dev0.ThrdCtrlHeader );
  }


/*------------------------------*/


_INLINE BOOL __sql54k_is_user_req_io_error ( LONG                  lDevspaceNo,
                                             tsp2_io_op_code_Param OpCode )

  {
# undef  MF__
# define MF__  MOD__"__sql54k_is_user_req_io_error"

  PDEVI_CTRL_REC             pDeviCtrl;
  BOOL                       fForceError;

  DBGPAS;

  pDeviCtrl = kgs.pFirstDeviCtrl + lDevspaceNo * XPARAM(lDeviPerDevspace) ;

  fForceError =  pDeviCtrl->lForceIOError &&
                (pDeviCtrl->lForceIOError & ~0x100) == OpCode;

  if ( fForceError )
    pDeviCtrl->lForceIOError = false ;

  return fForceError;
  }

/*------------------------------*/

_INLINE VOID __sql54k_io_finisched ( PTASK_CTRL_REC pTaskCtrl )
  {
# undef  MF__
# define MF__ MOD__"__sql54k_io_finisched"

  pTaskCtrl->lWaitForDevno    = UNDEF ;
  pTaskCtrl->DevNameWaitingFor= NULL;
  pTaskCtrl->TaskState        = TSK_RUNNING_EO00;

  return;
  }

/*------------------------------*/

//
// Don't use this function directly, use the BUILD_FILE_SEGMENT macro instead!!!!
//
_INLINE FILE_SEGMENT_ELEMENT*
          __sql54k_build_file_segment ( void*                   pSegBuffer,
                                        tsp00_PageAddr          pageVector[],
                                        ULONG                   DataLength )
  {
  ULONG                   CurrElem = 0;
  ULONG                   Len;
  FILE_SEGMENT_ELEMENT*   aSegmentArray = (FILE_SEGMENT_ELEMENT*)pSegBuffer;

  // --- Split the page vector pages into system pages and put them into the
  //     segment array which is used by ReadFileScatter() and WriteFileGather().
  //     This array must contain one member for each system memory page-sized chunk
  //     of the total number of bytes to write to the file, plus one member for the
  //     final NULL pointer.
  for ( CurrElem = 0, Len = 0; Len < DataLength; Len += kgs.ulSystemPageSize )
    {
    aSegmentArray[CurrElem].Alignment = 0;
    aSegmentArray[CurrElem++].Buffer  = (char*)pageVector[Len/XPARAM(ulDBPageSize)] +
                                               (Len % XPARAM(ulDBPageSize));
    }
  aSegmentArray[CurrElem].Alignment    = 0;
  aSegmentArray[CurrElem].Buffer       = NULL;

  return aSegmentArray;
  }

/*------------------------------*/

_INLINE IORetCode __sql54k_read_write_overlapped( PUKT_CTRL_REC          pUKT,
                                                  LONG                   lDevspaceNo,
                                                  tsp00_PageNo            lDevspacePage,
                                                  tsp2_io_op_code_Param  OpCode,
                                                  FILE_SEGMENT_ELEMENT   aSegmentArray[],
                                                  SHORT                  sNumOfPages,
                                                  OVERLAPPED            *pOverlapped,
                                                  ERRORTEXT              pcErrText )
  {
# undef  MF__
# define MF__ MOD__"__sql54k_read_write_overlapped"
  ULONG                   ulFilePosLow;
  ULONG                   ulFilePosHigh;
  PDEV_IO_CTRL_REC        pIOCtrl;
  IORetCode               IORet;
  LONG                    rc       = NO_ERROR;
  UKT_SINGLE_IO_REC *pSingleIOCtrl = &pUKT->pSingleIO[lDevspaceNo];

  DBGIN_T (pUKT->pCTask->ulTaskIndex);

  pIOCtrl        = &kgs.Dev0.pIOCtrl[lDevspaceNo];
  PAGES_TO_QWORD( ulFilePosLow, ulFilePosHigh, lDevspacePage + 1);

  //
  // --- do read/write operation
  //
  pOverlapped->Offset     = ulFilePosLow;
  pOverlapped->OffsetHigh = ulFilePosHigh;

  if ( OpCode == DEV_READ )
    {
    rc = __sql54k_io_read( pIOCtrl->DevHandle, aSegmentArray, sNumOfPages, pOverlapped );

    // -- are there too many outstanding overlapped I/O requests?
    if (( rc == ERROR_INVALID_USER_BUFFER ) ||
        ( rc == ERROR_NOT_ENOUGH_QUOTA )    ||
        ( rc == ERROR_NOT_ENOUGH_MEMORY ))
      {
      IORet = IO_Continue;
      }
    else if (( rc != NO_ERROR ) && ( rc != ERROR_IO_PENDING ))
      {
          if ( pIOCtrl->ulDetachPending )
          {
               MSGD (( ERR_DEVSPACE_NOT_ATT ));
               sql46c_build_error_string ( pcErrText, ERRMSG_DEVIO_NOT_ATT, 0 );
          }
          else
          {
              if ( sNumOfPages > 1 )
                MSGALL(( ERR_CANT_READ_PAGES_FROM_DEVSPACE, lDevspacePage + 1, lDevspacePage + sNumOfPages,
                         pIOCtrl->szDevspace, rc ))
              else
                MSGALL(( ERR_CANT_READ_FROM_DEVSPACE, lDevspacePage + 1,
                         pIOCtrl->szDevspace, rc ));
              sql46c_build_error_string ( pcErrText, ERRMSG_DEVIO_READ_FROM_DEVSPACE, rc );
          }
          IORet = IO_Error;
      }
    else
      {
          WAIT_UNTIL_ALONE( &pUKT->lExclusive );
          if ( pSingleIOCtrl->selfIOStatisticResetPending )
          {
              memset(&pSingleIOCtrl->ReadSelfIo.IOStat.TimeStat, 0, sizeof(pSingleIOCtrl->ReadSelfIo.IOStat.TimeStat) );
              pSingleIOCtrl->ReadSelfIo.ulPageCount = 0;
              memset(&pSingleIOCtrl->WriteSelfIo.IOStat.TimeStat, 0, sizeof(pSingleIOCtrl->WriteSelfIo.IOStat.TimeStat) );
              pSingleIOCtrl->WriteSelfIo.ulPageCount = 0;
              pSingleIOCtrl->selfIOStatisticResetPending = false;
          }
          CLEARLOCK( &pUKT->lExclusive );

          IORet = IO_Pending;
      }
    }
  else
    {
    rc = __sql54k_io_write( pIOCtrl->DevHandle, aSegmentArray, sNumOfPages, pOverlapped );

    // -- are there too many outstanding overlapped I/O requests?
    if (( rc == ERROR_INVALID_USER_BUFFER ) ||
        ( rc == ERROR_NOT_ENOUGH_QUOTA )    ||
        ( rc == ERROR_NOT_ENOUGH_MEMORY ))
      {
      IORet = IO_Continue;
      }
    else if (( rc != NO_ERROR ) && ( rc != ERROR_IO_PENDING ))
      {
          if ( pIOCtrl->ulDetachPending )
          {
               MSGD (( ERR_DEVSPACE_NOT_ATT ));
               sql46c_build_error_string ( pcErrText, ERRMSG_DEVIO_NOT_ATT, 0 );
          }
          else
          {
              if ( sNumOfPages > 1 )
                MSGALL(( ERR_CANT_WRITE_PAGES_ON_DEVSPACE, lDevspacePage + 1, lDevspacePage + sNumOfPages,
                         pIOCtrl->szDevspace, rc ))
              else
                MSGALL(( ERR_CANT_WRITE_ON_DEVSPACE, lDevspacePage + 1, pIOCtrl->szDevspace, rc ));

              sql46c_build_error_string ( pcErrText, ERRMSG_DEVIO_WRITE_ON_DEVSPACE, rc );
          }
          IORet = IO_Error;
      }
    else
      {
          WAIT_UNTIL_ALONE( &pUKT->lExclusive );
          if ( pSingleIOCtrl->selfIOStatisticResetPending )
          {
              memset(&pSingleIOCtrl->ReadSelfIo.IOStat.TimeStat, 0, sizeof(pSingleIOCtrl->ReadSelfIo.IOStat.TimeStat) );
              pSingleIOCtrl->ReadSelfIo.ulPageCount = 0;
              memset(&pSingleIOCtrl->WriteSelfIo.IOStat.TimeStat, 0, sizeof(pSingleIOCtrl->WriteSelfIo.IOStat.TimeStat) );
              pSingleIOCtrl->WriteSelfIo.ulPageCount = 0;
              pSingleIOCtrl->selfIOStatisticResetPending = false;
          }
          CLEARLOCK( &pUKT->lExclusive );

          IORet = IO_Pending;
      }
    }

  DBGOUT_T ( pUKT->pCTask->ulTaskIndex );
  return IORet;
  }

/*------------------------------*/

_INLINE  IORetCode __sql54k_overlapped_io_wait ( PUKT_CTRL_REC          *ppUKT,
                                                 LONG                   lDevspaceNo,
                                                 tsp00_PageNo            lDevspacePage,
                                                 tsp2_io_op_code_Param  OpCode,
                                                 SHORT                  sNumOfPages,
                                                 ERRORTEXT              pcErrText )
  {
# undef  MF__
# define MF__ MOD__"__sql54k_overlapped_io_wait"
  PDEV_IO_CTRL_REC        pIOCtrl;
  PDLQ_REC                pRequest;
  IORetCode               IORet;
  LONG                    rc       = NO_ERROR;

  DBGIN_T ((*ppUKT)->pCTask->ulTaskIndex);

  GOTO_DISP(ppUKT);

  pRequest = (*ppUKT)->pCurrReq;
  pIOCtrl  = &kgs.Dev0.pIOCtrl[lDevspaceNo];

  if ( ( ( pRequest            != GetTaskDLQElement1( (*ppUKT)->pCTask ) ) && 
            ( pRequest         != GetTaskDLQElement2( (*ppUKT)->pCTask ) ) )
      || ( pRequest->ulReqType != REQ_OVERLAPPED_IO_FINISHED ) )
    {
    MSGALL(( ERR_UNKNOWN_REQUEST ));
    DBG1  (( MF__, "[T:0x%03u] pRequest: 0x%x|0x%x (SEND) != (*ppUKT)->pCurrReq: 0x%x (REPLY)",
            (*ppUKT)->pCTask->ulTaskIndex,
            GetTaskDLQElement1( (*ppUKT)->pCTask ),
            GetTaskDLQElement2( (*ppUKT)->pCTask ),
            pRequest ));
    ABORT();
    }

  if (( pRequest->ReqArgs.DevWorkerRequest.fOk          == false ) ||
      ( pRequest->ReqArgs.DevWorkerRequest.ulNumOfBytes != PAGES_TO_BYTES((ULONG)sNumOfPages) ))
    {
    rc = pRequest->ReqArgs.DevWorkerRequest.APIRetCode;

    if ( OpCode == DEV_READ )
      {
      if ( sNumOfPages > 1 )
        MSGALL(( ERR_CANT_READ_PAGES_FROM_DEVSPACE, lDevspacePage + 1, lDevspacePage + sNumOfPages,
                 pIOCtrl->szDevspace, rc ))
      else
        MSGALL(( ERR_CANT_READ_FROM_DEVSPACE, lDevspacePage + 1, pIOCtrl->szDevspace, rc ));

      sql46c_build_error_string ( pcErrText, ERRMSG_DEVIO_READ_FROM_DEVSPACE, rc );
      IORet = IO_Error;
      }
    else
      {
      if ( sNumOfPages > 1 )
        MSGALL(( ERR_CANT_WRITE_PAGES_ON_DEVSPACE, lDevspacePage + 1, lDevspacePage + sNumOfPages,
                 pIOCtrl->szDevspace, rc ))
      else
        MSGALL(( ERR_CANT_WRITE_ON_DEVSPACE, lDevspacePage + 1, pIOCtrl->szDevspace, rc ));
      sql46c_build_error_string ( pcErrText, ERRMSG_DEVIO_WRITE_ON_DEVSPACE, rc );
      IORet = IO_Error;
      }
    }
  else
    IORet = IO_NoError;

  DBGOUT_T ( (*ppUKT)->pCTask->ulTaskIndex );
  return IORet;
  }

/*------------------------------*/

_INLINE  void __sql54k_overlapped_dual_io_wait (  PUKT_CTRL_REC          *ppUKT,
                                                   LONG                   lDevspaceNo1,
                                                   LONG                   lDevspaceNo2,
                                                   tsp00_PageNo            lDevspacePage,
                                                   SHORT                  sNumOfPages,
                                                   ERRORTEXT              pcErrText1,
                                                   ERRORTEXT              pcErrText2,
                                                   tsp00_Bool    VAR_VALUE_REF ok1,
                                                   tsp00_Bool    VAR_VALUE_REF ok2)
  {
# undef  MF__
# define MF__ MOD__"__sql54k_overlapped_dual_io_wait"
  PDEV_IO_CTRL_REC        pIOCtrl;
  PDLQ_REC                pRequest;
  LONG                    rc       = NO_ERROR;
  int                     outstandingIOCount = 2;
  DBGIN_T ((*ppUKT)->pCTask->ulTaskIndex);

  for ( outstandingIOCount = 2; outstandingIOCount > 0; --outstandingIOCount )
  {
      GOTO_DISP(ppUKT);

      pRequest = (*ppUKT)->pCurrReq;

      if ( ( ( pRequest            != GetTaskDLQElement1( (*ppUKT)->pCTask ) ) && 
                ( pRequest         != GetTaskDLQElement2( (*ppUKT)->pCTask ) ) )
          || ( pRequest->ulReqType != REQ_OVERLAPPED_IO_FINISHED ) )
      {
        MSGALL(( ERR_UNKNOWN_REQUEST ));
        DBG1  (( MF__, "[T:0x%03u] pRequest: 0x%x|0x%x (SEND) != (*ppUKT)->pCurrReq: 0x%x (REPLY)",
                (*ppUKT)->pCTask->ulTaskIndex,
                GetTaskDLQElement1( (*ppUKT)->pCTask ), 
                GetTaskDLQElement2( (*ppUKT)->pCTask ), 
                pRequest ));
        ABORT();
      }

      if ( pRequest == GetTaskDLQElement1( (*ppUKT)->pCTask ) )
      {
          pIOCtrl  = &kgs.Dev0.pIOCtrl[lDevspaceNo1];
      }
      else
      {
          pIOCtrl  = &kgs.Dev0.pIOCtrl[lDevspaceNo2];
      }

      if (( pRequest->ReqArgs.DevWorkerRequest.fOk          == false ) ||
          ( pRequest->ReqArgs.DevWorkerRequest.ulNumOfBytes != PAGES_TO_BYTES((ULONG)sNumOfPages) ))
        {
          rc = pRequest->ReqArgs.DevWorkerRequest.APIRetCode;

          if ( sNumOfPages > 1 )
            MSGALL(( ERR_CANT_WRITE_PAGES_ON_DEVSPACE, lDevspacePage + 1, lDevspacePage + sNumOfPages,
                     pIOCtrl->szDevspace, rc ))
          else
            MSGALL(( ERR_CANT_WRITE_ON_DEVSPACE, lDevspacePage + 1, pIOCtrl->szDevspace, rc ));

          if ( pRequest == GetTaskDLQElement1( (*ppUKT)->pCTask ) )
          {
              sql46c_build_error_string ( pcErrText1, ERRMSG_DEVIO_WRITE_ON_DEVSPACE, rc );
              *ok1 = false;
          }
          else
          {
              sql46c_build_error_string ( pcErrText2, ERRMSG_DEVIO_WRITE_ON_DEVSPACE, rc );
              *ok2 = false;
          }
        }
  }

  DBGOUT_T ( (*ppUKT)->pCTask->ulTaskIndex );

  }

/*------------------------------*/

_INLINE HANDLE __sql54k_create_file( LPCTSTR                 lpFileName,
                                     DWORD                   dwDesiredAccess,
                                     DWORD                   dwShareMode,
                                     LPSECURITY_ATTRIBUTES   lpSecurityAttributes,
                                     DWORD                   dwCreationDisposition,
                                     DWORD                   dwFlagsAndAttributes,
                                     HANDLE                  hTemplateFile )
  {
  #undef  MF__
  #define MF__ MOD__"__sql54k_create_file"

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

/*------------------------------*/

#ifdef DEVELOP_CHECKS
 _INLINE LONG __sql54k_check_filepos ( HANDLE        FileHandle,
                                            ULONG         lDevspaceNo,
                                            tsp00_PageNo   lDevspacePage,
                                            SHORT         sNumOfPages,
                                            ULONG         ulDevPages,
                                            PULONG        pulErrCode,
                                            ERRORTEXT     pcErrText )
{
#  undef  MF__
#  define MF__  MOD__"__sql54k_check_filepos"

   ULONG           ulFilePosLow;
   ULONG           ulFilePosHigh;
   ULONG           ulCurrDevPage = (ULONG)UNDEF;
   LONG            rc            = NO_ERROR;

   if ((ULONG)( lDevspacePage + sNumOfPages + 1 ) > ulDevPages )
     {
     if (pulErrCode)
       *pulErrCode = ERRCODE_DEVIO_DEV_POS;
     if (pcErrText)
       sql46c_build_error_string ( pcErrText, ERRMSG_DEVIO_DEV_POS, 0 );

     MSGALL(( ERR_WRNG_DEVSPACE_POS, lDevspacePage, sNumOfPages ));
     DBG1  (( MF__, "Wrong devspace write position: %u - Pages: %d",
              lDevspacePage, sNumOfPages ));

     DBGOUT;
     return rc;
     }

   ulFilePosHigh = 0;
   rc = SET_FILE_PTR ( FileHandle, 0, FILE_CURRENT,
                       &ulFilePosLow, &ulFilePosHigh );

   if (rc == NO_ERROR)
     {
     if ((HIGH_VALUE_VALID(ulFilePosHigh) == true) &&
         (LOW_VALUE_VALID(ulFilePosLow)   == true))
       ulCurrDevPage = QWORD_TO_PAGES(ulFilePosLow, ulFilePosHigh) - 1;
     else
       ulCurrDevPage = MAX_DEVSPACE_PAGES + 1;
     }
   else
     {
     if (pulErrCode)
       *pulErrCode = ERRCODE_DEVIO_SEEK;
     if (pcErrText)
       sql46c_build_error_string ( pcErrText, ERRMSG_DEVIO_SEEK_ON_DEVSPACE, rc );

     MSGALL(( ERR_CANT_SEEK_ON_DEVSPACE,
              kgs.Dev0.pIOCtrl[lDevspaceNo].szDevspace, rc ));
     DBG1  (( MF__, "Cannot seek on devspace: %s, rc = %d",
              kgs.Dev0.pIOCtrl[lDevspaceNo].szDevspace, rc ));

     DBGOUT;
     return rc;
     }

   if ( ulCurrDevPage != (ULONG)lDevspacePage )
     {
     if (pulErrCode)
       *pulErrCode = ERRCODE_DEVIO_SEEK_POS;
     if (pcErrText)
       sql46c_build_error_string ( pcErrText, ERRMSG_DEVIO_SEEK_POS, rc );

     MSGALL(( ERR_WRNG_DEVSPACE_SEEK_POS,
              ulCurrDevPage, lDevspacePage ));
     DBG1  (( MF__, "Wrong devspace position. current: %u - expected: %u",
              ulCurrDevPage, lDevspacePage ));

     DBGOUT;
     return -1;
     }

   DBGOUT;
   return rc;
   }
#endif

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
 */
 
static void os54_CloseSingleIOFileDesc (
    PUKT_CTRL_REC pTmpUKTCtrl,
    tsp00_Int4    lDevspaceNo )
{
    if (pTmpUKTCtrl->pSingleIO[lDevspaceNo].SingleIOHandle != INVALID_HANDLE_VALUE )
    {
        ERRORTEXT  errtext;
        PDEV_IO_CTRL_REC pIOCtrl = &kgs.Dev0.pIOCtrl[lDevspaceNo];

        DBG3 (( MF__, "Release single io handle of devspace no %d", lDevspaceNo ));

        MSGD (( INFO_SINGLEIO_DETACH, pIOCtrl->szDevspace, pTmpUKTCtrl->ulUKTIndex));

        (void)sql54k_dev_close ( pTmpUKTCtrl->pSingleIO[lDevspaceNo].SingleIOHandle,
                                 &pTmpUKTCtrl->pSingleIO[lDevspaceNo].isLocked,
                                 pIOCtrl->szDevspace,
                                 pIOCtrl->ulDevType ,
                                 errtext) ;

        pTmpUKTCtrl->pSingleIO[lDevspaceNo].ulOldSingleIOPos = 0;
        pTmpUKTCtrl->pSingleIO[lDevspaceNo].SingleIOHandle   = INVALID_HANDLE_VALUE;
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
static void os54_WaitForSelfIOCompletionAndClose(tsp00_Int4 lDevspaceNo)
{
#  undef  MF__
#  define MF__  MOD__"os54_WaitForSelfIOCompletionAndClose"
    PUKT_CTRL_REC             pTmpUKTCtrl;

    tsp00_Int4 yieldStartTime = kgs.ulCurrTime;

    // --- close all single io file of the current UKT
    for ( pTmpUKTCtrl = kgs.pFirstUKTCtrl; 
          pTmpUKTCtrl <= kgs.pLastUKTCtrl;
          pTmpUKTCtrl++ )
    {
        UKT_SINGLE_IO_REC *pSingleIOCtrl = &pTmpUKTCtrl->pSingleIO[lDevspaceNo];
        SAPDB_Int4 ulSelfIOPending;

        do
        {
            WAIT_UNTIL_ALONE( &pTmpUKTCtrl->lExclusive );
                ulSelfIOPending = pTmpUKTCtrl->ulSelfIOPending;
            CLEARLOCK( &pTmpUKTCtrl->lExclusive );

            if ( ulSelfIOPending )
            {
                    tsp00_Int4 now = kgs.ulCurrTime;
                    if ( yieldStartTime >= now
                      && (yieldStartTime + MAX_VDETACH_WAIT_TIME) < now )
                    {
                        SLEEP(100);
                        continue; /* still inside yield time interval... yield again */
                    }
                    else
                    {
                        MSGD(( WRN_VDETACH_IGNORE_SELF_IO, /* "Ignoring pending self io in UKT%d on device %d" */
                               pTmpUKTCtrl->ulUKTIndex, 
                               lDevspaceNo ));
                        ulSelfIOPending = false;
                    }
            }
        } while ( ulSelfIOPending );

        os54_CloseSingleIOFileDesc(pTmpUKTCtrl, lDevspaceNo);

        pSingleIOCtrl->selfIOStatisticResetPending = true;
    }
}

/*------------------------------------*/

static SAPDB_Bool
ResetForcedReadOnlyIfHSDefaultMaster(SAPDB_Bool *pIsForcedReadOnly)
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
