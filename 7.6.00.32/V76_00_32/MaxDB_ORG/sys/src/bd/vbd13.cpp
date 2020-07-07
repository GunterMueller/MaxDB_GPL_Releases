/****************************************************************************

  module      : vbd13.cpp

  -------------------------------------------------------------------------

  responsible : TorstenS

  special area: nodehandling
  description : Implements the node handling inclusive waiting conditions

  last changed: 2000-10-05  9:03
  see also    :

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG



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


*****************************************************************************/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"    // All Types
#include "gsp03_3.h"

#include "SQLManager/Catalog/Catalog_Interface.hpp"

#include "Converter/Converter_IPageNoManager.hpp"
#include "Converter/Converter_ICommon.hpp"
#include "Converter/Converter_Version.hpp"

#include "DataAccess/Data_BasePage.hpp"
#include "DataAccess/Data_Exceptions.hpp"
#include "DataAccess/Data_Messages.hpp"
#include "DataAccess/Data_Types.hpp"
#include "DataAccess/Data_PageNoList.hpp"
#include "DataAccess/Data_PageFrame.hpp"

#include "FileDirectory/FileDir_FileNo.hpp"

#include "IOManager/IOMan_Types.hpp"
#include "IOManager/IOMan_BlockAddress.hpp"
#include "IOManager/IOMan_ClusterAddress.hpp"
#include "IOManager/IOMan_ComplexClusterAddress.hpp"
#include "IOManager/IOMan_IDataIO.hpp"
#include "IOManager/IOMan_DataPages.hpp"

#include "KernelCommon/Kernel_Common.hpp"
#include "KernelCommon/Kernel_FileIO.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"

#include "RunTime/RTE_Message.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "RunTime/RTE_Crash.hpp"

#include "Trace/Trace_PageNoEntry.hpp"

#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SAPDBCommon/SAPDB_ToString.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Hex.hpp"

#include "Transaction/Trans_Context.hpp"

#include "ggg00.h"    // Kernel Types
#include "ggg01.h"    // OMS errors
#include "gbd00.h"    // BD Types
#include "gbd02.h"    // Datacache Types
#include "ggg91.h"    // Fileversion
#include "gbd490.h"  

#include "hbd01.h"
#include "hbd01_1.h"
#include "hbd06.h"
#include "hbd13.h" 
#include "hbd13_2.h"
#include "hbd17.h"
#include "hbd20.h"
#include "hbd30.h"
#include "hbd73.h"
#include "hbd102.h"
#include "hgg01_1.h" 
#include "hgg01_3.h" 
#include "hgg04.h" 
#include "hgg17.h"
#include "ggg92.h"    // used by hgg06.h
#include "hgg06.h"
#include "hsp30.h"
#include "heo55k.h"   // excl
#include "heo56.h"    // vsuspend
#include "heo670.h"   // eo670_CTraceStack

#if COMPILEMODE_MEO00 >= SLOW_MEO00
#include "hta99.h"
#include "hta01.h"
#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


#define LOCK_PAGE_EXCLUSIVE_BD13   true

#define WAIT_FOR_SVP_PREP   "B13WAIT FOR SVP PREPARE "
#define WAIT_FOR_READ       "B13WAIT FOR READ        "
#define WAIT_FOR_WRITE      "B13WAIT FOR WRITE       "
#define WAIT_FOR_BD_LOCK    "B13WAIT FOR BD LOCK     "
#define WAIT_FOR_SVP_END    "B13WAIT FOR SVP END     "

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/


#ifdef SAPDB_SLOW
class cbd13_CheckNodeTrace
{
public:

    cbd13_CheckNodeTrace(
        tgg00_BasisError &trError,
        tsp00_PageNo      root,
        tsp00_PageNo      nodeRoot ) : m_TrError( trError )
    {
        t01p2int4( bd_io, "NodeRoot    ", nodeRoot, "CurrRoot    ", root );
    }

    ~cbd13_CheckNodeTrace()
    {
        t01basis_error( bi, "bd13chk Node", m_TrError );
    }

private:
    tgg00_BasisError    &m_TrError;
};
#endif

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

static inline void
bd13_RReleaseNode(
    const tsp00_TaskId          taskId,
    tgg00_FileId                &fileId,
    tbd_node_ptrs               &pNodes,
    tsp00_Int2                  &exclFileLockCnt,
    tsp00_Int2                  &refCounter,
    const tbd_lru_info_Param    lruInfo,
    const tsp00_Bool            bReduceSyncCnt );

/*---------------------------------------------------------------------------*/

static inline tgg00_RecoveryMode
bd13_GetRecoveryMode( const tgg00_FileId    &fileId )
{
    if( gg04IsPermStaticTfn( fileId.fileTfn_gg00()))
    {
        SAPDBERR_ASSERT_STATE( fileId.fileType_gg00().includes( ftsObject_egg00 ));

        return( tgg00_RecoveryMode::fromConst( rmStatic_egg00 ));
    }
    else if( fileId.fileType_gg00().includes( ftsTemp_egg00 )) // PTS 1123689 TS 2003-08-20
    {
        return( tgg00_RecoveryMode::fromConst( rmTemp_egg00 ));
    }
    else
    {
        SAPDBERR_ASSERT_STATE( fileId.fileType_gg00().includes( ftsPerm_egg00 ) ||
                               fileId.fileType_gg00().includes( ftsByteStr_egg00 ));

        return( tgg00_RecoveryMode::fromConst( rmPerm_egg00 ));
    }
}

/*---------------------------------------------------------------------------*/

static inline tgg00_RecoveryMode
bd13_GetRecovMode( const Data_PageRecoveryMode& recMode )
{
    const Data_AddressingMode addrMode = recMode.GetAddressingMode();
    const Data_Recoverability recov    = recMode.GetRecoverability();

    if(( Data_Dynamic == addrMode )&&( Data_Recoverable == recov)){
        return tgg00_RecoveryMode::fromConst( rmPerm_egg00 );
    }
    if(( Data_Static == addrMode )&&( Data_Recoverable == recov )){
        return tgg00_RecoveryMode::fromConst( rmStatic_egg00 );
    }
    if(( Data_Dynamic == addrMode )&&( Data_NotRecoverable == recov )){
        return tgg00_RecoveryMode::fromConst( rmTemp_egg00 );
    }
    RTE_Crash( SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                                  "bd13GetRecovMode: Invalid recovery mode"));
    return tgg00_RecoveryMode::fromConst( rmNone_egg00 );
}

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL FUNCTIONS (PROTOTYPES)                                             *
 *===========================================================================*/

static inline void
bd13_CheckNodeHeader(
    const tgg00_FileId    &FileId,
    const tsp00_PageNo    PageNo,
    const tbd_nodeptr     Nptr,
    tgg00_BasisError      &TrError );

/*---------------------------------------------------------------------------*/

static inline void
bd13_CheckRootNodeHeader(
    const tgg00_FileId& FileId,
    const tbd_nodeptr   Nptr,
    tgg00_BasisError&   TrError );

/*---------------------------------------------------------------------------*/

static inline void
bd13_CheckNodeErrorHandling(
    const tsp00_TaskId          taskId,
    const tgg00_BasisError      trError,
    const tbd_nodeptr           &pNode,
    const tgg00_RecoveryMode    recMode,
    const tsp00_Int4            value );

/*---------------------------------------------------------------------------*/

static inline void
bd13_FrameForNewNode(
    tgg00_TransContext         &trans,
    tgg00_FileId               &fileId,
    tsp00_Int2                 &refCounter,
    const tsp00_Int2           level,
    const tsp00_PageNo         newPagePno,
    const tgg00_RecoveryMode   recMode,
    const tgg00_MessType       actionType,
    tbd_node_ptrs              &pNodes );

/*---------------------------------------------------------------------------*/

static inline void
bd13_FreeNode(
    tgg00_TransContext          &trans,
    tgg00_FileId                &fileId,
    const tsp00_PageNo          pageNo,
    const tgg00_RecoveryMode    recMode,
    const tgg00_MessType        actionType,
    const Converter_Version     &pageConverterVersion,
    tsp00_Int2                  &refCounter );

/*---------------------------------------------------------------------------*/

static inline void
bd13_WriteOldOccupant(
    const tsp00_TaskId          taskId,
    tbd_nodeptr                 pNode,
    tbd_nodeptr                 pCblock,
    const tsp00_PageNo          PageNo,
    const tgg00_RecoveryMode    recMode,
    tbd02_CachePageState       &nodeState,
    const tsp00_Bool            bLockPageExclusive );

/*---------------------------------------------------------------------------*/

static inline void
bd13_BadDataPageHandling(
    tgg00_BasisError    &baseError,
    const tgg00_FileId  &fileId,
    const tsp00_PageNo  pageNo );

/*---------------------------------------------------------------------------*/

static inline void
bd13_IllegalPageNoHandling(
    tgg00_BasisError            &baseError,
    const tgg00_FileId          &fileId,
    const tsp00_PageNo          pageNo,
    const tgg00_RecoveryMode    recMode );

/*---------------------------------------------------------------------------*/

static inline void
bd13_MsgAndSuspend(
    const tsp00_TaskId              taskId,
    const tsp00_PageNo              pageNo,
    const tgg00_VtraceType_Param    traceType,
    const char                      *msgText, // tsp00_C24
    const tsp00_Int4                suspendReason );

/*---------------------------------------------------------------------------*/

static inline void
bd13_MsgAndSuspendForPageLockReq(
    const tsp00_TaskId              taskId,
    tgg00_FileId                    &fileId,
    const tsp00_PageNo              pageNo,
    const tbd_node_request_Param    nodeReq,
    const SAPDB_Bool                bIsRoot );

/*---------------------------------------------------------------------------*/

static inline void
bd13_InitPageState(
    tbd02_CachePageState            &nodeState,
    const tbd00_PageLockMode_Param  pageLockMode,
    bool                            &bIsFirstInit );

/*---------------------------------------------------------------------------*/

static inline void
bd13_LoadOneNode(
    Trans_Context&      trans,
    tgg00_FileId&       fileId,
    Data_PageNo         pageNo,
    tgg00_RecoveryMode  recMode,
    tbd_node_ptrs&      pNodes,
    bool&               bDoReadIO );

/*---------------------------------------------------------------------------*/

static inline void
bd13_OverflowReaction (
    const tsp00_TaskId      taskId,
    const tsp00_PageNo      pageNo,
    const tgg00_MessType    actionType,
    const tsp00_Int4        suspendReason,
    tgg00_BasisError        &trError,
    tsp00_Int               &retryCount );

/*---------------------------------------------------------------------------*/

#ifdef SAPDB_SLOW
static inline void
bd13_GetNodeTraceInfo(
    const tsp00_PageNo              pageNo,
    const tgg00_RecoveryMode        recMode,
    const SAPDB_Bool                bIsRoot,
    const tbd_node_request_Param    nodeRequest );
#endif

/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  * 
 *===========================================================================*/

externC void
b13r_release_node(
    tbd_node_ptrs         &Nptrs,
    tbd_current_tree      &Current,
    tbd_lru_info_Param     LruInfo )
{
    ROUTINE_DBG_MEO00 ("b13r_release_node");

    const tbd_nodeptr   pRoot  = Current.currRootNptrs_bd00.np_ptr();
    const tbd_nodeptr   pIndex = Current.currIndexNptrs_bd00.np_ptr();
    const tsp00_PageNo  pageNo = Nptrs.np_ptr()->nd_id();

    if(
        (( NULL != pRoot  ) && ( pageNo == pRoot->nd_id())) ||
        (( NULL != pIndex ) && ( pageNo == pIndex->nd_id()))
    )
    {
        // if currNptrs_bd00 is set, then tree-locks are realized by
        // holding the node within current. It is forbidden to release
        // the node until the tree-lock is already necessary.

        Nptrs.np_ptr()   = NULL;
        Nptrs.np_cbptr() = NULL;
        return;
    }
    bd13_RReleaseNode( Current.curr_trans->trTaskId_gg00, Current.curr_tree_id, Nptrs,
                       Current.curr_trans->trBdExclFileLocks_gg00, Current.currCounter_bd00,
                       LruInfo, false );
}

/*---------------------------------------------------------------------------*/

externC void
bd13RReleaseRoot( tbd_current_tree  &current )
{
    ROUTINE_DBG_MEO00 ("bd13RReleaseRoot");

    SAPDBERR_ASSERT_STATE( plmLockWithSVPSyncNoWait_ebd00 != current.currRootPageLockMode_bd00 );
    SAPDBERR_ASSERT_STATE( plmLockWithoutReschedule_ebd00 != current.currRootPageLockMode_bd00 );
    SAPDBERR_ASSERT_STATE( current.curr_tree_id.fileRoot_gg00() == current.currRootNptrs_bd00.np_ptr()->nd_id());

    // currRootNptrs_bd00 will be set to NULL in bd20RReleasePage

    bd13_RReleaseNode( current.curr_trans->trTaskId_gg00, current.curr_tree_id, current.currRootNptrs_bd00,
                       current.curr_trans->trBdExclFileLocks_gg00, current.currCounter_bd00, lru_normal,
                       plmLockWithSVPSync_ebd00 == current.currRootPageLockMode_bd00 );
}

/*---------------------------------------------------------------------------*/

externC void
bd13WReleaseRoot( tbd_current_tree &current )
{
    ROUTINE_DBG_MEO00 ("bd13WReleaseRoot");

    SAPDBERR_ASSERT_STATE( plmLockWithSVPSyncNoWait_ebd00 != current.currRootPageLockMode_bd00 );
    SAPDBERR_ASSERT_STATE( plmLockWithoutReschedule_ebd00 != current.currRootPageLockMode_bd00 );
    SAPDBERR_ASSERT_STATE( current.curr_tree_id.fileRoot_gg00() == current.currRootNptrs_bd00.np_ptr()->nd_id());

    SAPDBERR_ASSERT_STATE( current.currRootUpdated_bd00 );

    tbd_node_ptrs auxRootNptrs = current.currRootNptrs_bd00;

    current.currRootNptrs_bd00.np_ptr()   = NULL;
    current.currRootNptrs_bd00.np_cbptr() = NULL;

    b13w_release_node( auxRootNptrs, current );
    current.currRootUpdated_bd00 = false;
}

/*---------------------------------------------------------------------------*/

externC void
bd13RReleaseSubRoot( tbd_current_tree &current )
{
    ROUTINE_DBG_MEO00 ("bd13RReleaseSubRoot");

    SAPDBERR_ASSERT_STATE( plmLockWithSVPSyncNoWait_ebd00 != current.currIndexPageLockMode_bd00 );
    SAPDBERR_ASSERT_STATE( plmLockWithoutReschedule_ebd00 != current.currIndexPageLockMode_bd00 );
    SAPDBERR_ASSERT_STATE( current.curr_tree_id.fileRoot_gg00() == current.currRootNptrs_bd00.np_ptr()->nd_id());


    // currIndexNptrs_bd00 will be set to NULL in bd20RReleasePage

    bd13_RReleaseNode( current.curr_trans->trTaskId_gg00, current.curr_tree_id, current.currIndexNptrs_bd00,
                       current.curr_trans->trBdExclFileLocks_gg00, current.currCounter_bd00, lru_normal,
                       plmLockWithSVPSync_ebd00 == current.currIndexPageLockMode_bd00);
}

/*---------------------------------------------------------------------------*/

externC void
bd13WReleaseSubRoot( tbd_current_tree &current )
{
    ROUTINE_DBG_MEO00 ("bd13WReleaseSubRoot");

    SAPDBERR_ASSERT_STATE( plmLockWithSVPSyncNoWait_ebd00 != current.currIndexPageLockMode_bd00 );
    SAPDBERR_ASSERT_STATE( plmLockWithoutReschedule_ebd00 != current.currIndexPageLockMode_bd00 );
    SAPDBERR_ASSERT_STATE( current.curr_tree_id.fileRoot_gg00() == current.currRootNptrs_bd00.np_ptr()->nd_id());
    SAPDBERR_ASSERT_STATE( current.currIndexUpdated_bd00 );

    tbd_node_ptrs auxSubRootNptrs = current.currIndexNptrs_bd00;

    current.currIndexNptrs_bd00.np_ptr()   = NULL;
    current.currIndexNptrs_bd00.np_cbptr() = NULL;

    b13w_release_node( auxSubRootNptrs, current );
    current.currIndexUpdated_bd00 = false;
}

/*---------------------------------------------------------------------------*/

externC void
bd13GetNode (tbd_current_tree         &Current,
             tsp00_PageNo              PageNo,
             tbd00_PageLockMode_Param  PageLockMode,
             tbd_node_request_Param    NodeReq,
             tbd_node_ptrs            &Nptrs)

{
    ROUTINE_DBG_MEO00 ("bd13GetNode");

    bool                        IsRoot       = (PageNo == Current.curr_tree_id.fileRoot_gg00());
    bool                        bPageRead    = false;
    bool                        bUnlockDone  = false;
    bool                        bIsFirstInit = true;
    tsp00_Int                   RetryCount   = 0;
    tbd02_CachePageState        nodeState;
    tgg00_BasisError            &TrError     = Current.curr_trans->trError_gg00;
    tgg00_TransContext          &currTrans   = *Current.curr_trans;
    tgg00_FileId                &currFileId  = Current.curr_tree_id;
    const tsp00_TaskId          &taskId      = currTrans.trTaskId_gg00;
    const tgg00_RecoveryMode    recMode      = bd13_GetRecoveryMode( currFileId );
    tgg00_WaitContext           waitContext;

    if  ((IsRoot) && (NULL != Current.currRootNptrs_bd00.np_ptr()))
    {
        Nptrs = Current.currRootNptrs_bd00;
        bd20CheckRootNptrs( taskId, TrError, PageNo, Nptrs.np_ptr()->nd_id(), Nptrs.np_cbptr() );
        return;
    }

    if( NULL != Current.currIndexNptrs_bd00.np_ptr() &&
            PageNo == Current.currIndexNptrs_bd00.np_ptr()->nd_id())
    {
        Nptrs = Current.currIndexNptrs_bd00;
        return;
    }

    Nptrs.np_cbptr() = NULL;
    Nptrs.np_ptr()   = NULL;

#   ifdef SAPDB_SLOW
    bd13_GetNodeTraceInfo( PageNo, recMode, IsRoot, NodeReq );
    t01int4( bd_io, "CurrCounter ", Current.currCounter_bd00 );
#   endif

    if ( plmLockWithSVPSync_ebd00 == PageLockMode || plmLockWithSVPSyncNoWait_ebd00 == PageLockMode) /* PTS 1108703 UH 2000-12-12 */
        nodeState.cpsSyncWithSavepoint_bd02.becomes( synCheckPreventSync_ebd02 );
    else
        nodeState.cpsSyncWithSavepoint_bd02.becomes( synNone_ebd02 );

    while (e_ok == TrError)
    {
        bd13_InitPageState (nodeState, PageLockMode, bIsFirstInit);

        bd20GetPage( taskId, waitContext, // UH 2003-02-26
                     TrError, currTrans.trBdExclFileLocks_gg00, currFileId, PageNo,
                     recMode, NodeReq, Nptrs.np_ptr(), Nptrs.np_cbptr(), nodeState );

        if  (e_ok != TrError)
        {
            // data cache overflow handling
            if(( e_sysbuffer_overflow == TrError ) || ( e_no_more_temp_space == TrError ))
            {
                bd13_OverflowReaction( taskId, PageNo, Current.curr_action, 43, TrError, RetryCount );
                continue;
            }
            break; // fatal error has occured
        }

        // data cache synchronisation with savepoint prepare phase
        if( synWait_ebd02 == nodeState.cpsSyncWithSavepoint_bd02 )
        {
            bd13_MsgAndSuspend( taskId, PageNo, b_await_r, WAIT_FOR_SVP_PREP, 230 );
            nodeState.cpsSyncWithSavepoint_bd02.becomes(synCheckPreventSync_ebd02);
            continue;
        }

        // data cache contains requested page
        if  (nodeState.cpsFound_bd02)
        {
            // requested page is still in read IO by another task
            if  (nodeState.cpsDirty_bd02)
            {
                bd13_MsgAndSuspend( taskId , PageNo, b_await_r, WAIT_FOR_READ, 41 );
                continue;
            }

            // requested page is locked by another task
            if  (nodeState.cpsHaveToWaitForLock_bd02)
            {
                bd13_MsgAndSuspendForPageLockReq( taskId, currFileId, PageNo, NodeReq, IsRoot );
                continue;
            }

            // requested page is available
            if  (currTrans.trRteCommPtr_gg00->virt_reads < MAX_INT2_SP00)
            {
                ++currTrans.trRteCommPtr_gg00->virt_reads;
            }
            if( NULL != Nptrs.np_ptr())
            {
                bd73DataIOStatisticForVirtualRead( Nptrs.np_ptr()->nd_level(),
                                                   rmTemp_egg00 != recMode,
                                                   currFileId.fileType_gg00().includes( ftsByteStr_egg00 ),
                                                   Nptrs.np_ptr()->nd_pmode().includes( pmCatalog_egg00 ));
            }
            break;
        }

        // data cache doesn't contains requested page

        // requested page is still in write IO by another task
        if  (nodeState.cpsWaitForWrite_bd02)
        {
            bd13_MsgAndSuspend( taskId, PageNo, b_await_w, WAIT_FOR_WRITE, 42);
            continue;
        }

        // in special cases release bd lock on requested page during page will be read by own task
        if
        (
            ( Current.currUnlockAllowed_bd00                                ) &&
            ( nr_for_read == NodeReq                                        ) &&
            ( NULL != Current.currIndexNptrs_bd00.np_ptr()                  ) && // for leaves only
            ( bd20IsPageShareLocked( Current.currIndexNptrs_bd00.np_cbptr())) && //  ----- " ------
            (
                (tfnTable_egg00     == currFileId.fileTfn_gg00()) ||
                (tfnShortScol_egg00 == currFileId.fileTfn_gg00())
            )
        )
        {
            bUnlockDone = true; // indicate that the bd lock is released
            bd30ReleaseTree( Current );
        }

        // before reading the request page into the data cache the assigned
        // data cache frame has to be written onto disk, because the page on
        // the frame is changed.
        if( nodeState.cpsDirty_bd02 )
        {
            bd13_WriteOldOccupant( taskId, Nptrs.np_ptr(), Nptrs.np_cbptr(),
                                   PageNo, recMode, nodeState, ! LOCK_PAGE_EXCLUSIVE_BD13 );
        }
        {
            // perform the read IO for the requested page
            bPageRead = true; // needed to handle the assigned data cache frame
            Data_BasePage Page( Nptrs.np_ptr(), rmStatic_egg00 != recMode,
                                rmTemp_egg00 == recMode, Nptrs.np_cbptr() );

            const IOMan_ReturnCode retCode =
                IOMan_IDataIO::GetInstance().ReadDataPage( taskId, Page, PageNo );

            switch( retCode )  // PTS 1127038 TS 2004-02-06
            {
            case IOMan_NoConverterEntry:
                {
                    TrError = e_no_converter_entry;
                    break;
                }
            case IOMan_PageNoOutOfRange:
                {
                    bd13_IllegalPageNoHandling( TrError, currFileId, PageNo, recMode );
                    break; // could be a fatal error
                }
            case IOMan_BadDataPage:
                {
                    bd13_BadDataPageHandling( TrError, currFileId, PageNo );
                    break; // fatal error has occured
                }
            default:
                ;
            }
        }
        break;
    };
    // end data cache overflow loop

    if(( NULL != Nptrs.np_ptr()) && ( e_ok == TrError ))
    {
        bd13_CheckNodeHeader( currFileId, PageNo, Nptrs.np_ptr(), TrError );
    }

    // handle assigned data cache frame in case of user triggered read IO
    if( bPageRead )
    {
        if( e_ok == TrError )
        {
            if( bUnlockDone ){
                TrError = e_bd_leaf_unlocked;
            }
            const tsp00_Bool bLockPageExclusive = ( nr_for_update == NodeReq );
            bd20UsePage( taskId, TrError, PageNo, recMode,  Nptrs.np_cbptr(), Nptrs.np_ptr(), bLockPageExclusive );
        }
        else
        {
            tgg00_BasisError AuxError = TrError;
            TrError                   = e_ok;

            bd20FreePage( taskId, PageNo, recMode, TrError, currTrans.trBdExclFileLocks_gg00 );

            if( e_ok == TrError )
                TrError = AuxError;

            if  (
                (( e_no_converter_entry == TrError ) || ( e_page_in_wrong_tree == TrError ))
                &&
                ( rmPerm_egg00 == recMode )
                &&
                ( IsRoot )
                &&
                ( m_verify != Current.curr_action )
            )
            {
                if ( g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdPrim_gg00 )
                {
                    Kernel_VTrace()
                    << "bd13 fileversion:"
                    << " fileNo: " << FileDir_FileNoToString(currFileId.fileTabId_gg00())
                    << ", fi_vers: " << SAPDBTrace_Hex(&currFileId.fileVersion_gg00(),
                                                       sizeof(currFileId.fileVersion_gg00()))
                    << ", root: " << currFileId.fileRoot_gg00()
                    << ", trError: " << TrError;
                }
                TrError = e_old_fileversion;
            }
            Nptrs.np_ptr()   = NULL;
            Nptrs.np_cbptr() = NULL;
        }
    }
    ++currTrans.trIoCount_gg00;

    if( Nptrs.np_ptr() != NULL )
        ++Current.currCounter_bd00;
};

/*---------------------------------------------------------------------------*/

externC void
bd13GetNewSpecifiedNode (
    tgg00_TransContext         &Trans,
    tgg00_FileId               &FileId,
    tsp00_PageNo               WantedPageNo,
    tbd_node_ptrs              &Nptrs,
    tsp00_Bool                 &bPageGenerated)
{
    ROUTINE_DBG_MEO00( "bd13GetNewSpecifiedNode" );


#   ifdef SAPDB_SLOW
    t01int4( bd_io, "GetStaticPno", WantedPageNo );
#   endif

    SAPDBERR_ASSERT_STATE( rmStatic_egg00 == bd13_GetRecoveryMode( FileId ));

    bool                        bPageRead    = false;
    bool                        bIsFirstInit = true;
    tsp00_Int                   RetryCount   = 0;
    tbd02_CachePageState        nodeState;
    tgg00_BasisError            &TrError     = Trans.trError_gg00;
    const tsp00_TaskId          &taskId      = Trans.trTaskId_gg00;
    const tgg00_RecoveryMode    recMode      = tgg00_RecoveryMode::fromConst( rmStatic_egg00 );
    tgg00_WaitContext           waitContext;

    Nptrs.np_cbptr() = NULL;
    Nptrs.np_ptr()   = NULL;
    bPageGenerated   = false;
    nodeState.cpsSyncWithSavepoint_bd02.becomes(synCheckPreventSync_ebd02);


    while( e_ok == TrError )
    {
        bd13_InitPageState( nodeState, plmLockWithSVPSync_ebd00, bIsFirstInit );

        bd20GetPage( taskId, waitContext, // UH 2003-02-26
                     TrError, Trans.trBdExclFileLocks_gg00, FileId, WantedPageNo,
                     rmStatic_egg00, nr_for_update, Nptrs.np_ptr(), Nptrs.np_cbptr(),
                     nodeState );

        if( e_ok != TrError )
        {
            // data cache overflow handling
            if(( e_sysbuffer_overflow == TrError ) || ( e_no_more_temp_space == TrError ))
            {
                bd13_OverflowReaction( taskId, WantedPageNo,  tgg00_MessType::fromConst(m_nil),
                                       43, TrError, RetryCount );
                continue;
            }
            break; // fatal error has occured
        }

        // data cache synchronisation with savepoint prepare phase
        if( synWait_ebd02 == nodeState.cpsSyncWithSavepoint_bd02 )
        {
            bd13_MsgAndSuspend( taskId, WantedPageNo, b_await_r, WAIT_FOR_SVP_PREP, 230 );
            nodeState.cpsSyncWithSavepoint_bd02.becomes( synCheckPreventSync_ebd02 );
            continue;
        }
        // ==================================
        // data cache contains requested page
        // ==================================
        if( nodeState.cpsFound_bd02 )
        {
            // requested page is still in read IO by another task
            if( nodeState.cpsDirty_bd02 )
            {
                bd13_MsgAndSuspend( taskId, WantedPageNo, b_await_r, WAIT_FOR_READ, 41 );
                continue;
            }

            // requested page is locked by another task
            if( nodeState.cpsHaveToWaitForLock_bd02 )
            {
                bd13_MsgAndSuspendForPageLockReq( taskId, FileId, WantedPageNo, nr_for_update, false );
                continue;
            }

            // requested page is available
            if( Trans.trRteCommPtr_gg00->virt_reads < MAX_INT2_SP00 )
            {
                ++Trans.trRteCommPtr_gg00->virt_reads;
            }
            if( Nptrs.np_ptr() != NULL )
            {
                bd73DataIOStatisticForVirtualRead( LEAF_LEVEL_BD00,
                                                   SAPDB_TRUE, SAPDB_FALSE, SAPDB_FALSE );
            }
            break;
        }
        // ==========================================
        // data cache doesn't contains requested page
        // ==========================================

        // requested page is still in write IO by another task
        if( nodeState.cpsWaitForWrite_bd02 )
        {
            bd13_MsgAndSuspend( taskId, WantedPageNo, b_await_w, WAIT_FOR_WRITE, 42 );
            continue;
        }

        // before reading the request page into the data cache the assigned
        // data cache frame has to be written onto disk, because the page on
        // the frame is changed.
        if( nodeState.cpsDirty_bd02 )
        {
            bd13_WriteOldOccupant( taskId, Nptrs.np_ptr(), Nptrs.np_cbptr(), WantedPageNo,
                                   recMode, nodeState, ! LOCK_PAGE_EXCLUSIVE_BD13 );
        }
        {
            // perform the read IO for the requested page
            bPageRead = true; // needed to handle the assigned data cache frame
            Data_BasePage Page( Nptrs.np_ptr(), rmStatic_egg00 != recMode,
                                rmTemp_egg00 == recMode, Nptrs.np_cbptr() );

            const RTETask_ITask     &task   = *RTETask_ITask::ByTaskID( taskId );
            const IOMan_ReturnCode  retCode =
                IOMan_IDataIO::GetInstance().ReadDataPage( taskId, Page, WantedPageNo );

            switch( retCode )  // PTS 1127038 TS 2004-02-06
            {
            case IOMan_BadDataPage:
                {
                    TrError = e_bad_datapage;
                    break;
                }
            case IOMan_NoConverterEntry:
                {
                    // The page is neither in the data cache nor on a data volume
                    // and therefore is has to be generated with the given page no!

                    TrError        = e_ok;
                    bPageGenerated = true; // Mark corresponding converter entry as used

                    if( ! Converter_IPageNoManager::Instance().RequestSpecificiedNewStaticPageNo(
                                task, WantedPageNo ))
                        TrError = e_page_no_in_use;
                    break;
                }
            case IOMan_PageNoOutOfRange:
                {
                    TrError = e_invalid_oid;
                    break;
                }
            default:
                ;
            }
        }

        if( e_ok != TrError )
            break; // fatal error has occured

        if( bPageGenerated )
            b13init_default_data_page( FileId, LEAF_LEVEL_BD00, WantedPageNo, Nptrs );

        break;
    };
    // end data cache overflow loop

    // Make only same primitive checks, because of the interaction with the garbage
    // collector it is possible that requested page is still in another object file.
    // It is only warranted that this file is an object file.
    if(( NULL != Nptrs.np_ptr()) && ( e_ok == TrError ))
    {
        if( WantedPageNo != Nptrs.np_ptr()->nd_id() )
            TrError = e_illegal_page_no;
        else
            if(
                ( pt2Object_egg00    != Nptrs.np_ptr()->nd_pt2() ) &&
                ( pt2VarObject_egg00 != Nptrs.np_ptr()->nd_pt2() )
            )
                TrError = e_inconsistent_nodetype;
    }
    // handle assigned data cache frame in case of user triggered read IO
    if( bPageRead )
    {
        if( e_ok == TrError )
            bd20UsePage( taskId, TrError, WantedPageNo, rmStatic_egg00, Nptrs.np_cbptr(),
                         Nptrs.np_ptr(), LOCK_PAGE_EXCLUSIVE_BD13 );
        else
        {
            tgg00_BasisError AuxError = TrError;
            TrError                   = e_ok;

            bd20FreePage( taskId, WantedPageNo, rmStatic_egg00, TrError, Trans.trBdExclFileLocks_gg00 );

            if( e_ok == TrError)
                TrError = AuxError;

            Nptrs.np_ptr()   = NULL;
            Nptrs.np_cbptr() = NULL;
        }
    }
}

/*---------------------------------------------------------------------------*/

externC void
bd13LoadNode (
    const tsp00_TaskId          taskId,
    tgg00_BasisError&           trError,
    tgg00_FileId&               fileId,
    tgg00_PageType&             pageType,
    const tsp00_PageNo          pageNo,
    const tgg00_RecoveryMode&   recMode,
    const tsp00_Bool            bSetToChanged,
    const tsp00_Bool            bIsClustered )

{
    ROUTINE_DBG_MEO00( "bd13LoadNode" );

    bool                    bPageRead    = false;
    bool                    bIsFirstInit = true;
    tsp00_Int               retryCount   = 0;
    tbd02_CachePageState    nodeState;
    tbd_node_ptrs           pNodes;

    switch( recMode )
    {
    case rmTemp_egg00 :
        break;
    case rmPerm_egg00:
        break;
    case rmStatic_egg00:
        break;
    default:
        RTE_Crash( SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                                      "bd13LoadNode: Invalid rec mode") );
    }

    pNodes.np_cbptr() = NULL;
    pNodes.np_ptr()   = NULL;

    nodeState.cpsSyncWithSavepoint_bd02.becomes( synNone_ebd02 );

    while( e_ok == trError )
    {
        bd13_InitPageState( nodeState, plmLock_ebd00, bIsFirstInit );

        bd20LoadPage( taskId, pageNo, recMode, bSetToChanged, bIsClustered, pageType, trError,
                      fileId, pNodes.np_ptr(), pNodes.np_cbptr(), nodeState );

        if( e_ok != trError )
        {
            // data cache overflow handling
            if(( e_sysbuffer_overflow == trError ) || ( e_no_more_temp_space == trError ))
            {
                bd13_OverflowReaction( taskId, pageNo, tgg00_MessType::fromConst( m_nil ),
                                       43, trError, retryCount );
                continue;
            }
            break; // fatal error has occured
        }

        // data cache contains requested page
        if( nodeState.cpsFound_bd02 )
        {
            // requested page is still in read IO by another task
            if( nodeState.cpsDirty_bd02 )
            {
                bd13_MsgAndSuspend( taskId, pageNo, b_await_r, WAIT_FOR_READ, 41 );
                continue;
            }
            // requested page is available
            // because pNodes is not set level and catalog affiliation is not
            // determinable therefore  most probable values are taken
            bd73DataIOStatisticForVirtualRead( LEAF_LEVEL_BD00, rmTemp_egg00 != recMode,
                                               fileId.fileType_gg00().includes( ftsByteStr_egg00 ),
                                               SAPDB_FALSE );
            break;
        }

        // data cache doesn't contains requested page

        // requested page is still in write IO by another task
        if( nodeState.cpsWaitForWrite_bd02 )
        {
            bd13_MsgAndSuspend( taskId, pageNo, b_await_w, WAIT_FOR_WRITE, 42 );
            continue;
        }

        // before reading the request page into the data cache the assigned
        // data cache frame has to be written to disk, because the page on
        // the frame is changed.
        if( nodeState.cpsDirty_bd02 )
        {
            bd13_WriteOldOccupant( taskId, pNodes.np_ptr(), pNodes.np_cbptr(),
                                   pageNo, recMode, nodeState, ! LOCK_PAGE_EXCLUSIVE_BD13 );
        }
        {
            // execute read operation for the requested page
            bPageRead = true; // needed to handle the assigned data cache frame

            Data_BasePage dataPage( pNodes.np_ptr(), rmStatic_egg00 != recMode,
                                    rmTemp_egg00 == recMode, pNodes.np_cbptr() );

            const IOMan_ReturnCode retCode =
                IOMan_IDataIO::GetInstance().ReadDataPage( taskId, dataPage, pageNo );

            switch( retCode )
            {
            case IOMan_NoConverterEntry:
                {
                    trError = e_no_converter_entry;
                    break;
                }
            case IOMan_PageNoOutOfRange:
                {
                    bd13_IllegalPageNoHandling( trError, fileId, pageNo, recMode );
                    break; // could be a fatal error
                }
            case IOMan_BadDataPage:
                {
                    bd13_BadDataPageHandling( trError, fileId, pageNo );
                    break; // fatal error has occured
                }
            default:
                ;
            }
        }
        break;
    };
    // end data cache overflow loop

    if( ! bPageRead )
        return; // if the page was still in the datacache pNodes is NULL and no lock exist!

    // if the page was read then pNodes points to a 'locked' page!
    if( e_ok == trError ){
        pageType = pNodes.np_ptr()->nd_pt();
    }

    // make sure that new file directory pages won't be stored
    // in the data cache. They have to be treated by the file
    // directory himself as changed
    const bool bDataCacheRelevant = ( pageType != ptFileDir_egg00 );

    // handle assigned data cache frame in case of user requested read operation
    if(( e_ok == trError ) && ( bDataCacheRelevant ))
    {
        bd20UsePageAfterLoad( taskId, pageNo, recMode, bSetToChanged, bIsClustered,
                              pNodes.np_cbptr(), fileId, trError );
        return;
    }
    // error handling; throw read page out of the data cache
    tsp00_Int2 exclFileLockCnt = 0; // dummy, because bd20load didn't increase this value
    tgg00_BasisError auxError  = trError;
    trError                    = e_ok;

    bd20FreePage( taskId, pageNo, recMode, trError, exclFileLockCnt );

    if( e_ok == trError ){
        trError = auxError;
    }
};

/*---------------------------------------------------------------------------*/

externC void
bd13LoadNodes(
    Trans_Context&               trans,
    tgg00_FileId&                fileId,
    const Data_PageRecoveryMode& recMode,
    const Data_PageNoList&       pageNoList,
    const IOMan_ClusterAddress&  cluster,
    Data_PageFrame&              ioBuffer )
{
    ROUTINE_DBG_MEO00( "bd13LoadNodes" );

    SAPDBERR_ASSERT_STATE( pageNoList.GetSize() == cluster.GetBlockCount());

    IOMan_ClusteredDataPages    pages( trans.Allocator(), ioBuffer ) ;

    if( ! pages.Initialize( pageNoList.GetSize())){
        trans.SetErrorCode( e_no_more_memory );
        return;
    }
    const tgg00_RecoveryMode    _recMode         = bd13_GetRecovMode( recMode );
    SAPDB_UInt                  readPageCount    = 0;
    SAPDB_UInt                  startPageNoIndex = 0;
    SAPDB_UInt                  lastPageNoIndex  = 0;
    IOMan_ComplexClusterAddress ioCluster;

    Data_PageNoList::ConstIterator  iter    = pageNoList.Begin();
    Data_PageNoList::ConstIterator  endIter = pageNoList.End();
    for( ; iter != endIter; ++iter )
    {
        tbd_node_ptrs     pNodes;
        bool              bDoReadPage = false;
        const Data_PageNo pageNo      = *iter;
        bd13_LoadOneNode( trans, fileId, pageNo, _recMode, pNodes, bDoReadPage );
        if( ! trans.IsOk()){
            break;
        }
        Data_BasePage page;
        page.SetRecoveryMode( recMode );
        page.SetHint( pNodes.np_cbptr());
        page.AssignNodePtr( pNodes.np_ptr());
        pages.AppendPage( pageNo, page, bDoReadPage );
        if( bDoReadPage )
        {
            ++readPageCount;
            if( 1 == readPageCount )
            {   // set cluster to the first page which is not in the data cache
                startPageNoIndex = pageNoList.GetIndex( iter );
                lastPageNoIndex  = startPageNoIndex;
                ioCluster.SetCluster( cluster.GetBlockAddress( startPageNoIndex ), 1 );
                continue;
            }else{
                lastPageNoIndex = pageNoList.GetIndex( iter );
            }
        }
        if( 0 < readPageCount ){ // at least one block in the cluster exist
            ioCluster.Append( bDoReadPage );
        }
    }
    if( trans.IsOk())
    {
        if( 1 > readPageCount ){ // all requested pages are in the data cache
            return;
        }
        ioCluster.SetBlockCount( lastPageNoIndex - startPageNoIndex + 1 );
        Data_PageNo badDataPageNo;
        const IOMan_ReturnCode retCode = IOMan_IDataIO::GetInstance().ReadDataPages(
                                             trans.Task(), pageNoList, pages, ioCluster,
                                             startPageNoIndex, badDataPageNo );
        if( IOMan_Okay != retCode ){
            bd13_BadDataPageHandling( trans.OldTrans().trError_gg00, fileId, badDataPageNo );
        }
        else
        {
            const pasbool bSetToChanged = false;
            const pasbool bIsClustered  = true;
            pages.CopyFrames(); // from continuous memory chunk into data cache
            IOMan_DataPagesIterator pageIter( pages );
            for( pageIter.Begin(); ! pageIter.End(); ++pageIter )
            {
                if( ! ( *pageIter ).IsAssigned()){
                    continue; // clustered read with 'empty' pages for better performance
                }
                const Data_PageNo pageNo  = ( *pageIter ).PageNo();
                tbd_nodeptr pCBlock = static_cast< tbd_nodeptr >(( *pageIter ).GetHint());

                bd20UsePageAfterLoad( trans.Task().ID(), pageNo, _recMode, bSetToChanged, bIsClustered,
                                      pCBlock, fileId, trans.OldTrans().trError_gg00 );
            }
        }
    }
    if( trans.IsOk()){
        return;
    }
    // a fatal error occur during read; throw read page out of the data cache
    const tgg00_BasisError  auxError = trans.OldTrans().trError_gg00;
    Msg_List                auxList  = trans.ErrorList();

    IOMan_DataPagesIterator pageIter( pages );
    for( pageIter.Begin(); ! pageIter.End(); ++pageIter )
    {
        if( ! ( *pageIter ).IsAssigned()){
            continue; // ignore pages without page frame pointer
        }
        const Data_PageNo pageNo  = ( *pageIter ).PageNo();
        tsp00_Int2 exclFileLockCnt    = 0; // dummy, because bd20load didn't increase this value
        trans.ResetErrorCode();

        bd20FreePage( trans.Task().ID(), pageNo, _recMode, trans.OldTrans().trError_gg00, exclFileLockCnt );
    }
    if(( e_sysbuffer_overflow == auxError )||( e_no_more_temp_space == auxError )){
        trans.ResetErrorCode();
    }else{
        trans.SetErrorCode( auxError );
        trans.ErrorList() = auxList;
        auxList.Clear();
    }
};

/*---------------------------------------------------------------------------*/

externC void
b13new_node(
    tsp00_Int2         level,
    tbd_node_ptrs     &Nptrs,
    tbd_current_tree  &Current)

{
    ROUTINE_DBG_MEO00 ("b13new_node");

    Data_PageNo                 newPageNo;
    tgg00_FileId                &fileId = Current.curr_tree_id;
    tgg00_TransContext          &trans  = *Current.curr_trans;
    const tgg00_RecoveryMode    recMode = bd13_GetRecoveryMode( fileId );
    const RTETask_ITask         &task   = *RTETask_ITask::ByTaskID( trans.trTaskId_gg00 );


    SAPDBERR_ASSERT_STATE( rmStatic_egg00 != recMode );

    if( fileId.fileType_gg00().includes( ftsTemp_egg00 ))
    {
        Converter_IPageNoManager::Instance().RequestNewTempPageNo( task, newPageNo );

        if( ! fileId.fileType_gg00().includes( ftsShared_egg00 )){
            ++trans.trTempCount_gg00;
        }
    }
    else
    {
        const bool bIgnoreSecureLimit =
            trans.trState_gg00.includes( tsIgnoreDBSecureLimit_egg00 ) ? true : false;

        Converter_IPageNoManager::Instance().RequestNewPermPageNo(
            task, bIgnoreSecureLimit, newPageNo );
    };

    if( LEAF_LEVEL_BD00 == level )
        ++Current.currLeafPageCountDelta_bd00;
    else
        ++Current.currIndexPageCountDelta_bd00;

    bd13_FrameForNewNode( trans, fileId, Current.currCounter_bd00, level,
                          newPageNo, recMode, Current.curr_action, Nptrs);
}

/*---------------------------------------------------------------------------*/

externC void
b13new_object_node(
    tbd_node_ptrs     &Nptrs,
    tbd_current_tree  &Current)
{
    ROUTINE_DBG_MEO00 ("b13new_object_node");

    Data_PageNo                 newPageNo;
    tgg00_FileId                &fileId = Current.curr_tree_id;
    tgg00_TransContext          &trans  = *Current.curr_trans;
    const tgg00_RecoveryMode    recMode = bd13_GetRecoveryMode( fileId );
    const RTETask_ITask         &task   = *RTETask_ITask::ByTaskID( trans.trTaskId_gg00 );

    switch(  fileId.fileTfn_gg00() )
    {
    case tfnObj_egg00:
        {
            SAPDBERR_ASSERT_STATE( rmStatic_egg00 == recMode );

            Converter_IPageNoManager::Instance().RequestNewStaticPageNo( task, newPageNo );

            bd13_FrameForNewNode( trans, fileId, Current.currCounter_bd00,
                                  LEAF_LEVEL_BD00, newPageNo, recMode,
                                  Current.curr_action, Nptrs );

            break;
        }
    case tfnContObj_egg00:
        {
            SAPDBERR_ASSERT_STATE( rmPerm_egg00 == recMode );

            Converter_IPageNoManager::Instance().RequestNewPermPageNo(
                task, false, newPageNo );

            bd13_FrameForNewNode (trans, fileId, Current.currCounter_bd00,
                                  LEAF_LEVEL_BD00, newPageNo, recMode,
                                  Current.curr_action, Nptrs );
            break;
        }
    default:
        {
            trans.trError_gg00 = e_invalid_filetype;
            g01opmsg (sp3p_knldiag, sp3m_error, csp03_b13_illegal_obj_file_type,
                      csp3_n_obj, "Illegal type for objfile", fileId.fileTfn_gg00());
            return;
        }
    }
    if( e_ok != trans.trError_gg00 )
    {
        tgg00_BasisError AuxError = trans.trError_gg00;
        trans.trError_gg00        = e_ok;

        b13free_node( Nptrs, Current );
        if( e_ok == trans.trError_gg00 )
        {
            trans.trError_gg00 = AuxError;
        }
    }
}

/*---------------------------------------------------------------------------*/

externC void
b13new_root(
    tbd_node_ptrs     &Nptrs,
    tbd_current_tree  &Current)

{
    ROUTINE_DBG_MEO00 ("b13new_root");

    b13new_node (LEAF_LEVEL_BD00, Nptrs, Current );

#   ifdef SAPDB_SLOW
    if( e_ok == Current.curr_trans->trError_gg00 )
    {
        t01int4 (bd_io, "new root pno", Nptrs.np_ptr()->nd_id());
    }
#   endif

};

/*---------------------------------------------------------------------------*/

externC void
b13init_default_data_page (
    tgg00_FileId         &FileId,
    tsp00_Int2            Level,
    tsp00_PageNo          NewPno,
    tbd_node_ptrs        &Nptrs)

{
    ROUTINE_DBG_MEO00 ("b13init_default_data_page");

    const tgg00_RecoveryMode recMode = bd13_GetRecoveryMode( FileId );
    const SAPDB_Bool bIsRoot = (NIL_PAGE_NO_GG00 == FileId.fileRoot_gg00()) && (LEAF_LEVEL_BD00 == Level);

    Nptrs.np_ptr()->nd_id()               = NewPno;
    Nptrs.np_ptr()->nd_pt().becomes (ptData_egg00);
    Nptrs.np_ptr()->nd_checktype().becomes (chckChecksumData_egg00);
    Nptrs.np_ptr()->nd_pmode().clear();
    /* */
    Nptrs.np_ptr()->nd_bottom()           = BODY_BEG_BD00;
    Nptrs.np_ptr()->nd_record_cnt()       = 0;
    /* */
    Nptrs.np_ptr()->nd_level()            = Level;
    Nptrs.np_ptr()->nd_file_state().clear();
    Nptrs.np_ptr()->nd_sorted()           = false;
    /* */
    Nptrs.np_ptr()->nd_right()            = NIL_PAGE_NO_GG00;
    Nptrs.np_ptr()->nd_left()             = NIL_PAGE_NO_GG00;
    /* */
    Nptrs.np_ptr()->nd_last()             = NIL_PAGE_NO_GG00;
    Nptrs.np_ptr()->nd_filler2()          = NIL_PAGE_NO_GG00;
    /* */
    Nptrs.np_ptr()->nd_conv_version()     = Converter_Version::GetNilVersion();
    Nptrs.np_ptr()->ndStrVersion_bd00()   = NIL_STR_VERSION_GG00;
    /* */
    Nptrs.np_ptr()->nd_history_info().clear();
    Nptrs.np_ptr()->ndPageVersion_bd00()  = 0;
    Nptrs.np_ptr()->ndInvUsageCnt_bd00()  = cgg_nil_transindex;
    /* */
    if( FileId.fileType_gg00().includes ( ftsByteStr_egg00 ))
    {
        Nptrs.np_ptr()->ndStrFileSize_bd00() = 0;  // from byte 57 to byte 60
    }
    else
    {
        if( LEAF_LEVEL_BD00 == Level )
            Nptrs.np_ptr()->ndLeafCount_bd00() = 1; // from byte 57 to byte 60
        else
            Nptrs.np_ptr()->ndLeafCount_bd00() = 0;
    }
    /* */
    if ( bIsRoot && (tfnTable_egg00 == FileId.fileTfn_gg00())
            && FileId.fileType_gg00().includes ( ftsPerm_egg00 ) ) {
        Nptrs.np_ptr()->ndTreeLeaves_bd00() = FileId.fileLeafNodes_gg00();
    } else {
        Nptrs.np_ptr()->ndTreeLeaves_bd00() = cgg_nil_leafnodes;
    }
    /* */
    Nptrs.np_ptr()->nd_trans_id().gg90SetNil();
    Nptrs.np_ptr()->nd_filler4()          = 0;
    /* */
    Nptrs.np_ptr()->ndInvRoot_bd00()      = NIL_PAGE_NO_GG00; // PTS 1120695 UH 2003-10-27

    Nptrs.np_ptr()->nd_write_cnt()        = 0;
    /* */
    if( bIsRoot )
    {
        Nptrs.np_ptr()->nd_root()         = NewPno;
        Nptrs.np_ptr()->nd_file_version() = FileId.fileVersion_gg00();
    }
    else
    {
        Nptrs.np_ptr()->nd_root() = FileId.fileRoot_gg00();
        Nptrs.np_ptr()->nd_file_version().gg90SetNil();
    }
    /* */
    /* */
    /* */
    switch (FileId.fileTfn_gg00())
    {
    case tfnOmsInv_egg00 :
    case tfnMulti_egg00 :
    case tfnTempMulti_egg00 :
        Nptrs.np_ptr()->nd_pt2().becomes(pt2Inv_egg00);
        break;
    case tfnInvSubtree_egg00 :
        Nptrs.np_ptr()->nd_pt2().becomes(pt2InvSubtree_egg00);
        break;
    case tfnLog_egg00 :
    case tfnObjBefore_egg00 :
        Nptrs.np_ptr()->nd_pt2().becomes(pt2Log_egg00);
        if  (tfnLog_egg00 == FileId.fileTfn_gg00())
        {
            Nptrs.np_ptr()->nd_trans_id() = FileId.fileTransId_gg00();
        }
        else
        {
            // pos of last checked and unused history entry in the page
            Nptrs.np_ptr()->nd_left() = sizeof(Nptrs.np_ptr()->nd_full_header()) + 1;
        }

        Nptrs.np_ptr()->nd_file_version().gg90SetNil();

        if  (NIL_PAGE_NO_GG00 == FileId.fileRoot_gg00())
        {
            Nptrs.np_ptr()->nd_root() = NewPno;
            Nptrs.np_ptr()->nd_last() = NewPno;
        }
        else
        {
            Nptrs.np_ptr()->nd_root() = FileId.fileRoot_gg00();
            Nptrs.np_ptr()->nd_last() = NIL_PAGE_NO_GG00;
        }
        break;
    case tfnObj_egg00 :
        Nptrs.np_ptr()->nd_pt2().becomes(pt2Object_egg00);
        break;
    case tfnContObj_egg00 :
        Nptrs.np_ptr()->nd_pt2().becomes(pt2ContObject_egg00);
        break;
    case tfnUndoLog_egg00 :
        Nptrs.np_ptr()->nd_pt2().becomes(pt2UndoLog_egg00);
        break;
    case tfnRedoLog_egg00 :
        Nptrs.np_ptr()->nd_pt2().becomes(pt2RedoLog_egg00);
        break;
    case tfnHistory_egg00 :
        Nptrs.np_ptr()->nd_pt2().becomes(pt2History_egg00);
        break;
    case tfnHistoryDirectory_egg00 :
        Nptrs.np_ptr()->nd_pt2().becomes(pt2HistoryDirectory_egg00);
        break;
    case tfnOpenTrans_egg00 :
        Nptrs.np_ptr()->nd_pt2().becomes(pt2OpenTrans_egg00);
        break;
    default :
        if (FileId.fileType_gg00().includes (ftsByteStr_egg00))
        {
            Nptrs.np_ptr()->nd_pt2().becomes(pt2String_egg00);
        }
        else
        {
            const tsp00_PageNo pageRoot = Nptrs.np_ptr()->nd_root();
            Nptrs.np_ptr()->nd_pt2().becomes (pt2Tab_egg00);

            if(
                ( recMode == rmPerm_egg00 ) &&
                (
                    ( pageRoot == Catalog_Interface::GetInstance().GetSys1CatalogTreeId().fileRoot_gg00()) ||
                    ( pageRoot == Catalog_Interface::GetInstance().GetSys2CatalogTreeId().fileRoot_gg00())
                )
            )
            {
                Nptrs.np_ptr()->nd_pmode().addElement( pmCatalog_egg00 );
            }
            else
            {
                if  (
                    ( pageRoot == bd17GetBlobFdirRoot() && (recMode == rmPerm_egg00))
                )
                {
                    Nptrs.np_ptr()->nd_pmode().addElement(pmFdir_egg00);
                }
                else
                    if( ! bIsRoot && FileId.fileType_gg00().includes( ftsArchive_egg00 ))
                    {
                        Nptrs.np_ptr()->nd_pmode().addElement( pmArchive_egg00 );
                    }
            }
        }
    }
    /* */
    if  (FileId.fileType_gg00().includes (ftsTemp_egg00))
    {
        Nptrs.np_ptr()->nd_pmode().addElement(pmTemp_egg00);
    }
    Nptrs.np_ptr()->nd_checksum() = 0;
    Nptrs.np_ptr()->nd_trailer()  = Nptrs.np_ptr()->nd_header();
};

/*---------------------------------------------------------------------------*/

void
bd13_RReleaseNode(
    const tsp00_TaskId          taskId,
    tgg00_FileId                &fileId,
    tbd_node_ptrs               &pNodes,
    tsp00_Int2                  &exclFileLockCnt,
    tsp00_Int2                  &refCounter,
    const tbd_lru_info_Param    lruInfo,
    const tsp00_Bool            bReduceSyncCnt )
{
    const tgg00_RecoveryMode    recMode = bd13_GetRecoveryMode( fileId );
    const tsp00_PageNo          pageNo  = pNodes.np_ptr()->nd_id();

#   ifdef SAPDB_SLOW
    switch( recMode )
    {
    case rmTemp_egg00:
        t01int4 (bd_io, "Rrel tmp pno", pageNo);
        break;
    case rmPerm_egg00:
        t01int4 (bd_io, "Rrel per pno", pageNo);
        break;
    case rmStatic_egg00:
        t01int4 (bd_io, "Rrel sta pno", pageNo);
        break;
    default:
        ;
    }
#   endif

    bd20RReleasePage( taskId, exclFileLockCnt, fileId, pNodes.np_ptr(),
                      REFCAST_MEO00 (tbd02_pDataCBlock) &pNodes.np_cbptr(),
                      pageNo, recMode, lruInfo, bReduceSyncCnt );

    --refCounter;

#   ifdef SAPDB_SLOW
    t01int4( bd_io, "CurrCounter ", refCounter );
#   endif
};

/*---------------------------------------------------------------------------*/

externC void
b13w_release_node(
    tbd_node_ptrs         &Nptrs,
    tbd_current_tree      &Current)

{
    ROUTINE_DBG_MEO00 ("b13w_release_node");


    tgg00_TransContext          &currTrans  = *Current.curr_trans;
    tgg00_FileId                &currFileId = Current.curr_tree_id;;
    const tgg00_RecoveryMode    recMode     = bd13_GetRecoveryMode( currFileId );
    const tsp00_TaskId          &taskId     = currTrans.trTaskId_gg00;
    const tsp00_PageNo          pageNo      = Nptrs.np_ptr()->nd_id();
    const tbd_nodeptr           pRoot       = Current.currRootNptrs_bd00.np_ptr();
    const tbd_nodeptr           pIndex      = Current.currIndexNptrs_bd00.np_ptr();

#   ifdef SAPDB_SLOW
    switch( recMode )
    {
    case rmTemp_egg00:
        t01int4 (bd_io, "Wrel tmp pno", pageNo);
        break;
    case rmPerm_egg00:
        t01int4 (bd_io, "Wrel per pno", pageNo);
        break;
    case rmStatic_egg00:
        t01int4 (bd_io, "Wrel sta pno", pageNo);

        break;
    default:
        ;
    }
#   endif

    if(
        ( g01glob.treecheck ) && ( rmTemp_egg00 != recMode ) &&
        (
            ( tfnSys_egg00       == currFileId.fileTfn_gg00()) ||
            ( tfnTable_egg00     == currFileId.fileTfn_gg00()) ||
            ( tfnShortScol_egg00 == currFileId.fileTfn_gg00())
        )
    )
    {
        tgg00_BasisError    auxError = e_ok;

        bd13CheckNode( taskId, auxError, Nptrs, currFileId, true );
        if( e_ok != auxError )
        {
            b06write_filename_and_root( currFileId );
            g01abort( csp3_bd_msg, csp3_n_btree, "bd13CheckNode           ", 0);
        };
    }

    if(
        ((NULL != pRoot)  && (pageNo == pRoot->nd_id())) ||
        ((NULL != pIndex) && (pageNo == pIndex->nd_id()))
    )
    {

        // if currNptrs_bd00 is set, then tree-locks are realized by
        // holding the node within current. It is forbidden to release
        // the node until the tree-lock is already necessary.

        if( pageNo == currFileId.fileRoot_gg00() )
            Current.currRootUpdated_bd00 = true;
        else
            Current.currIndexUpdated_bd00 = true;

        Nptrs.np_ptr()   = NULL;
        Nptrs.np_cbptr() = NULL;
        return;
    }

    tbd02_SwapState  ioState;
    const tsp00_Int  level        = Nptrs.np_ptr()->nd_level();
    const pasbool    bIsCatalog   = Nptrs.np_ptr()->nd_pmode().includes( pmCatalog_egg00 );
    const pasbool    bIsClustered = Current.currExtendedFileTypeSet_bd00.includes( eftsClustered_egg00 ) ||
                                    (currFileId.fileType_gg00().includes( ftsByteStr_egg00 ) && gg01_clustered_lobs);
    ioState.becomes( swsTestIo_ebd02 );
    bd20WReleasePage( taskId, currTrans.trBdExclFileLocks_gg00, currFileId, Nptrs.np_ptr(),
                      REFCAST_MEO00 (tbd02_pDataCBlock) &Nptrs.np_cbptr(),
                      pageNo, recMode, bIsClustered, ioState);

    if( swsDoIo_ebd02 == ioState )
    {
        Data_BasePage   page( Nptrs.np_ptr(), rmStatic_egg00 != recMode,
                              rmTemp_egg00 == recMode, Nptrs.np_cbptr() );

        IOMan_IDataIO::GetInstance().WriteDataPage( taskId, page );

        bd20ForcedIODone( Nptrs.np_cbptr(), taskId );
        ioState.becomes( swsIoDone_ebd02 );

        bd20WReleasePage( taskId, currTrans.trBdExclFileLocks_gg00, currFileId, Nptrs.np_ptr(),
                          REFCAST_MEO00 (tbd02_pDataCBlock) &Nptrs.np_cbptr(),
                          pageNo, recMode, bIsClustered, ioState );
    }

    bd73DataIOStatisticForVirtualWrite( level, rmTemp_egg00 != recMode,
                                        currFileId.fileType_gg00().includes( ftsByteStr_egg00 ),
                                        bIsCatalog );

    --Current.currCounter_bd00;
    // monitoring of physical writes must be done in VBD20 when
    // clearing pages or executing savepoints
#       ifdef SAPDB_SLOW
    t01int4( bd_io, "CurrCounter ", Current.currCounter_bd00 );
#       endif
};

/*---------------------------------------------------------------------------*/

externC void
bd13FreePageNo(
    tsp00_PageNo            pageNo,
    tgg00_ConverterVersion  pageConverterVersion,
    tsp00_Int2              level,
    tbd_current_tree        &current )

{
    ROUTINE_DBG_MEO00 ("bd13FreePageNo");

    tgg00_FileId    &currFileId = current.curr_tree_id;
    tbd00_FreeQueue &freeQueue  = current.currFreeQueue_bd00;

    if(
        ( pageNo == currFileId.fileRoot_gg00()       ) &&
        ( NULL != current.currRootNptrs_bd00.np_ptr())
    )
    {
        current.currRootNptrs_bd00.np_ptr() = NULL;
    }

    if(
        ( NULL != current.currRootNptrs_bd00.np_ptr()                       ) &&
        ( bd20IsPageExclusiveLocked( current.currRootNptrs_bd00.np_cbptr( ))) &&
        ( freeQueue.fqTop_bd00 <= MAX_FREE_QUEUE_BD00                       ) &&
        ( ! currFileId.fileBdUse_gg00().includes( bd_release_acc )          )
    )
    {
        const tsp00_Int4 queuePos = freeQueue.fqTop_bd00 - POS_OFF_DIFF_BD00;

        freeQueue.fqPnos_bd00[ queuePos ]        = pageNo;
        freeQueue.fqPageVersion_bd00[ queuePos ] = pageConverterVersion;

        ++freeQueue.fqTop_bd00;
    }
    else
    {
        const tgg00_RecoveryMode recMode = bd13_GetRecoveryMode( currFileId );

        bd13_FreeNode( *current.curr_trans, currFileId, pageNo, recMode,
                       current.curr_action, pageConverterVersion, current.currCounter_bd00 );

    }

    if( LEAF_LEVEL_BD00 == level )
        --current.currLeafPageCountDelta_bd00;
    else
        --current.currIndexPageCountDelta_bd00;
};

/*---------------------------------------------------------------------------*/

externC void
bd13FreeNewRoot( tbd_current_tree  &current )
{
    Converter_Version   pageConverterVersion;

    bd13FreePageNo( current.curr_tree_id.fileRoot_gg00(),
                    pageConverterVersion, LEAF_LEVEL_BD00, current );
};

/*---------------------------------------------------------------------------*/

externC void
bd13FreeTempPageNo(
    tsp00_PageNo      pageNo,
    tsp00_Int2        level,
    tbd_current_tree  &current )
{
    SAPDBERR_ASSERT_STATE( current.curr_tree_id.fileType_gg00().includes( ftsTemp_egg00 ));

    Converter_Version   pageConverterVersion;

    bd13FreePageNo( pageNo, pageConverterVersion, level, current );
};

/*---------------------------------------------------------------------------*/

externC void
b13free_node (
    tbd_node_ptrs    &Nptrs,
    tbd_current_tree &Current)

{
    const tsp00_Int2        level  = Nptrs.np_ptr()->nd_level();
    const tsp00_PageNo      pageNo = Nptrs.np_ptr()->nd_id();
    const Converter_Version pageConverterVersion = Nptrs.np_ptr()->nd_conv_version();

    if(
        ( pageNo == Current.curr_tree_id.fileRoot_gg00()                ) &&
        ( Current.curr_tree_id.fileType_gg00().includes( ftsTemp_egg00 ))
    )
    {
        Nptrs.np_ptr()->nd_root() = NIL_PAGE_NO_GG00;
    }

    bd13FreePageNo( pageNo, pageConverterVersion, level, Current );

    if(
        ( e_ok       != Current.curr_trans->trError_gg00 ) &&
        ( e_shutdown != Current.curr_trans->trError_gg00 )
    )
    {
        g01abort( csp3_bd_msg, csp3_n_btree, "Free page failed        ",
                  Current.curr_trans->trError_gg00 );
    }

    Nptrs.np_ptr()   = NULL;
    Nptrs.np_cbptr() = NULL;
};

/*---------------------------------------------------------------------------*/

externC void
bd13FreePnosAfterUnlock( tbd_current_tree &Current )

{
    ROUTINE_DBG_MEO00 ("bd13FreePnosAfterUnlock");

    tgg00_TransContext  &currTrans = *Current.curr_trans;

    if( e_shutdown == currTrans.trError_gg00 ) // PTS 1123793 TS 2003-08-26
        return;

    const tgg00_BasisError auxError  = currTrans.trError_gg00; // PTS 1123793 TS 2003-08-26
    currTrans.trError_gg00           = e_ok;

    const tgg00_RecoveryMode recMode    = bd13_GetRecoveryMode( Current.curr_tree_id );
    tbd00_FreeQueue          &freeQueue = Current.currFreeQueue_bd00;

    while ((e_ok == currTrans.trError_gg00) && (1 < freeQueue.fqTop_bd00))
    {
        --freeQueue.fqTop_bd00;

        const tsp00_Int4        queuePos    = freeQueue.fqTop_bd00 - POS_OFF_DIFF_BD00;
        const tsp00_PageNo      pageNo      = freeQueue.fqPnos_bd00[ queuePos ];
        const Converter_Version pageVersion = freeQueue.fqPageVersion_bd00[ queuePos ];

        bd13_FreeNode( currTrans, Current.curr_tree_id, pageNo, recMode,
                       Current.curr_action, pageVersion, Current.currCounter_bd00 );
    }
    currTrans.trError_gg00 = auxError;
};

/*---------------------------------------------------------------------------*/

externC void
bd13WriteExclusiveLockedRoot(
    tgg00_TransContext    &trans,
    tgg00_FileId          &fileId,
    tbd_node_ptrs         &rootNptrs )
{
    ROUTINE_DBG_MEO00( "bd13WriteExclusiveLockedRoot" );


    const tbd_nodeptr           &pNode   = rootNptrs.np_ptr();
    const tbd_nodeptr           &pCblock = rootNptrs.np_cbptr();
    const tsp00_PageNo          pageNo   = pNode->nd_id();
    const tgg00_RecoveryMode    recMode  = bd13_GetRecoveryMode( fileId );


    tbd02_CachePageState    nodeState;
    SAPDB_Bool              bIsFirstInit = true;
    SAPDB_Bool1             bWritePage   = false;

    bd13_InitPageState( nodeState, plmLock_ebd00, bIsFirstInit );
    nodeState.cpsSyncWithSavepoint_bd02.becomes( synNone_ebd02 );

    while( e_ok == trans.trError_gg00 )
    {
        bd20CheckExclusiveLockedPageForIO( trans.trTaskId_gg00, fileId,
                                           pageNo, recMode, pCblock, bWritePage, nodeState );

        if( nodeState.cpsWaitForWrite_bd02 )
        {
            // at this time the data cache contains more than one image of the current
            // root page and therefore we have to wait with the i/o until the older
            // images of the root page are written to to the data area.

            bd13_MsgAndSuspend( trans.trTaskId_gg00, pageNo, b_await_w, WAIT_FOR_WRITE, 40 );
            nodeState.cpsWaitForWrite_bd02 = false;
            continue;
        }

        if( nodeState.cpsWaitForSvpEnd_bd02 )
        {
            // the root page is changed but not relevant for the running savepoint
            // therefore we have to wait with the i/o until the savepoint is finished.
            // otherwise the converter version materialized with the running savepoint
            // will be inconsistent

            bd13_MsgAndSuspend (trans.trTaskId_gg00, pageNo, b_await_w, WAIT_FOR_SVP_END, 28 );
            nodeState.cpsWaitForSvpEnd_bd02 = false;
            continue;
        }

        if( bWritePage )
        {
            // the root page has to be written into the data area. Note that the
            // state of the control block is blocked and not userIo! Synchronization
            // of i/o with a concurrent running (pager) task will be done by the
            // usageCounter and the blocked state of the control block.

            nodeState.cpsFound_bd02 = true; // guarantee that subsequent proc will not call bd20ReplaceOldOccupan
            bd13_WriteOldOccupant( trans.trTaskId_gg00, pNode, pCblock, pageNo, recMode, nodeState, ! LOCK_PAGE_EXCLUSIVE_BD13);
        }
        break; // normal termination condition
    }
};

/*---------------------------------------------------------------------------*/

externC void
bd13CheckNode(
    tsp00_TaskId          taskId,
    tgg00_BasisError      &trError,
    tbd_node_ptrs         &pNodes,
    tgg00_FileId          &fileId,
    tsp00_Bool            bExtendedCheck )
{
    ROUTINE_DBG_MEO00 ("bd13CheckNode");

    const tbd_nodeptr           &pNode  = pNodes.np_ptr();
    const tgg00_RecoveryMode    recMode = bd13_GetRecoveryMode( fileId );


    if(( pNode->nd_bottom() < BODY_BEG_BD00 ) || ( pNode->nd_bottom() > MAX_BOTTOM_BD00 )){
        trError = e_data_page_corrupted; // invalid bottom
        bd13_CheckNodeErrorHandling( taskId, trError, pNode, recMode, pNode->nd_bottom());
        return;
    }

    const Data_PageNo maxPageNo = ( ! bExtendedCheck ) ? (Data_PageNo) NIL_PAGE_NO_GG00 :
                                  (( rmTemp_egg00 == recMode ) ?
                                   Converter_ICommon::Instance().MaxTempPageNo() :
                                   Converter_ICommon::Instance().MaxPermPageNo());

    const tsp00_Int4 maxRecIndex    = pNode->nd_record_cnt()-1;
    tsp00_Int4       totalRecLength = 0;
    tsp00_Int4       prevKeyLength  = 0;
    tgg00_RecPtr     pPrevRec       = NULL;

    for( tsp00_Int4 recIndex = FIRST_REC_INDEX_BD00; recIndex <= maxRecIndex; ++recIndex )
    {
        const tsp00_Int4 recPos =
            pNode->nd_pointer_list()[MAX_POINTERINDEX_BD00 - recIndex - POS_OFF_DIFF_BD00];

        if(( recPos < BODY_BEG_BD00) || ( recPos >= pNode->nd_bottom()))
        {
            trError = e_illegal_entrypos; // invalid record position
            bd13_CheckNodeErrorHandling( taskId, trError, pNode, recMode, recPos );
            return;
        }

        const tgg00_RecPtr pRec =
            reinterpret_cast<tgg00_RecPtr>( &pNode->nd_body()[recPos - POS_OFF_DIFF_BD00 - NODE_HEADER_MXBD00]);
        const tsp00_Int4   recLength = pRec->recLen_gg00();

        totalRecLength = totalRecLength + bd102RecAlign( recLength );

        if(
            ( recLength  < MIN_RECORD_LEN_BD00 ) ||
            ( recPos + bd102RecAlign( recLength ) > pNode->nd_bottom())
        )
        {
            trError = e_illegal_entrylength;  // invalid record length
            bd13_CheckNodeErrorHandling( taskId, trError, pNode, recMode, recPos );
            return;
        }

        const tsp00_Int4   keyLength = pRec->recKeyLen_gg00();

        if (
            (( pNode->nd_level() == LEAF_LEVEL_BD00 ) && ( keyLength < 1 )) ||
            ( keyLength > PERM_KEY_MXSP00 ) ||
            ( keyLength > recLength )       ||
            (
                ( pNode->nd_level() > LEAF_LEVEL_BD00 ) &&
                ( keyLength + cgg_rec_key_offset + sizeof ( tbd00_SepLeafCount ) != recLength )
            )
        )
        {
            trError = e_illegal_keylength; // invalid key length
            bd13_CheckNodeErrorHandling( taskId, trError, pNode, recMode, recPos );
            return;
        }

        // ADDITIONAL CHECKS ONLY EXECUTED IF REQUESTED

        if( ! bExtendedCheck )
            continue;

        if( NULL != pPrevRec )
        {
            tsp00_LcompResult compResult;

            s30cmp (pRec->recKey_gg00().keyVal_gg00(), POS_OFF_DIFF_BD00, keyLength,
                    pPrevRec->recKey_gg00().keyVal_gg00(), POS_OFF_DIFF_BD00, prevKeyLength,
                    compResult );
            if( l_greater != compResult )
            {
                trError = e_illegal_key; // invalid key order
                bd13_CheckNodeErrorHandling( taskId, trError, pNode, recMode, recPos );
                return;
            }
        }
        prevKeyLength = keyLength;
        pPrevRec      = pRec;

        if( pNode->nd_level() > LEAF_LEVEL_BD00 )
        {
            const tsp00_PageNo pageNo = gg06PnoGet( pRec->recPno_gg00());

            if(( pageNo < 0) || ( pageNo > maxPageNo ))
            {
                trError = e_illegal_page_no; // invalid separator pageNo
                bd13_CheckNodeErrorHandling( taskId, trError, pNode, recMode, recPos );
                return;
            }

            for( tsp00_Int4 auxRecIndex = FIRST_REC_INDEX_BD00; auxRecIndex < recIndex; ++auxRecIndex )
            {
                const tsp00_Int4   auxRecPos =
                    pNode->nd_pointer_list()[MAX_POINTERINDEX_BD00 - auxRecIndex - POS_OFF_DIFF_BD00];

                const tgg00_RecPtr pAuxRec   =
                    reinterpret_cast<tgg00_RecPtr> (&pNode->nd_body()[ auxRecPos - POS_OFF_DIFF_BD00
                                                    - NODE_HEADER_MXBD00]);

                if( pageNo == gg06PnoGet( pAuxRec->recPno_gg00()))
                {
                    trError = e_illegal_page_no; // duplicate separator pageNo
                    bd13_CheckNodeErrorHandling( taskId, trError, pNode, recMode, recPos );
                    return;
                }
            }
        }
    }

    if( totalRecLength != pNode->nd_bottom() - BODY_BEG_BD00 )
    {
        trError = e_illegal_record; // invalid record length
        bd13_CheckNodeErrorHandling( taskId, trError, pNode, recMode, totalRecLength );
        return;
    }
}

/*===========================================================================*
 *  LOCAL FUNCTIONS (CODE)                                                   *
 *===========================================================================*/

inline void
bd13_MsgAndSuspend(
    const tsp00_TaskId           taskId,
    const tsp00_PageNo           pageNo,
    const tgg00_VtraceType_Param traceType,
    const char                   *msgText, //tsp00_C24
    const tsp00_Int4             suspendReason )
{
    if( g01vtrace.vtrAll_gg00 || g01vtrace.vtrIoTrace_gg00 )
    {
        Trace_PageNoEntry( pageNo, traceType );
    }
    vsuspend( taskId, suspendReason );
}

/*---------------------------------------------------------------------------*/

inline void
bd13_MsgAndSuspendForPageLockReq(
    const tsp00_TaskId              taskId,
    tgg00_FileId                    &fileId,
    const tsp00_PageNo              pageNo,
    const tbd_node_request_Param    nodeReq,
    const SAPDB_Bool                bIsRoot )
{
    const SAPDB_Bool bForUpdate = ( nr_for_update == nodeReq );
    tsp00_Int4 suspendReason;

    if( fileId.fileType_gg00().includes( ftsTemp_egg00 ))
    {   // TempRootExcl, TempRootShare, TempNodeExcl, TempNodeShare
        suspendReason = bIsRoot ? (( bForUpdate ) ? 20 : 21 ) : (( bForUpdate ) ? 22 : 23 );
    }
    else if( g04inv_tfn( fileId.fileTfn_gg00() ))
    {
        // InvRootExcl, InvRootShare, InvNodeExcl, InvNodeShare
        suspendReason = bIsRoot ? (( bForUpdate ) ? 16 : 17 ) : (( bForUpdate ) ? 18 : 19 );
    }
    else if( fileId.fileType_gg00().includes( ftsObject_egg00 ))
    {
        // ObjRootExcl, ObjRootShare, ObjNodeExcl, ObjNodeShare
        suspendReason = bIsRoot ? (( bForUpdate ) ? 24 : 25 ) : (( bForUpdate ) ? 26 : 27 );
    }
    else if( tfnUndoLog_egg00 == fileId.fileTfn_gg00() )
    {
        // UndoRootExcl, UndoRootShare, UndoNodeExcl, UndoNodeShare
        suspendReason = bIsRoot ? (( bForUpdate ) ? 29 : 30 ) : (( bForUpdate ) ? 31 : 32 );
    }
    else if( tfnHistory_egg00 == fileId.fileTfn_gg00() )
    {
        // HistRootExcl, HistRootShare, HistNodeExcl, HistNodeShare
        suspendReason = bIsRoot ? (( bForUpdate ) ? 33 : 34 ) : (( bForUpdate ) ? 35 : 36 );
    }
    else
    {   // TableRootExcl, TableRootShare, TableNodeExcl, TableNodeShare
        suspendReason = bIsRoot ? (( bForUpdate ) ? 10 : 11 ) : (( bForUpdate ) ? 12 : 13 );
    }

    if( g01vtrace.vtrAll_gg00 || g01vtrace.vtrIoTrace_gg00 )
    {
        const tgg00_VtraceType_Param traceType = bForUpdate ? b13exclusive_wait : b13share_wait;

        Trace_PageNoEntry( pageNo, traceType );
    }

    vSuspendForPageLock( taskId, suspendReason, pageNo );
}

/*---------------------------------------------------------------------------*/

inline void
bd13_InitPageState( tbd02_CachePageState            &nodeState,
                    const tbd00_PageLockMode_Param  pageLockMode,
                    bool                            &bIsFirstInit )
{
    nodeState.cpsFound_bd02             = false;
    nodeState.cpsDirty_bd02             = false;
    nodeState.cpsWaitForWrite_bd02      = false;
    nodeState.cpsWaitForSvpEnd_bd02     = false;
    nodeState.cpsIoDone_bd02            = false;
    nodeState.cpsHaveToWaitForLock_bd02 = false;

    if( plmLockWithSVPSyncNoWait_ebd00 == pageLockMode )
        nodeState.cpsIntendToWaitForLock_bd02 = false;
    else
        nodeState.cpsIntendToWaitForLock_bd02 = true;

    if( bIsFirstInit )
    {
        if( plmLockWithoutReschedule_ebd00 == pageLockMode ) // PTS 1115176 TS 2002-04-09
            nodeState.cpsTaskRescheduled_bd02 = true; // do not reschedule
        else
            nodeState.cpsTaskRescheduled_bd02 = false;
        bIsFirstInit = false;
    }
}

/*---------------------------------------------------------------------------*/

void
bd13_CheckNodeHeader (
    const tgg00_FileId    &FileId,
    const tsp00_PageNo    PageNo,
    const tbd_nodeptr     Nptr,
    tgg00_BasisError      &TrError)
{
    ROUTINE_DBG_MEO00 ("bd13_CheckNodeHeader");


#   ifdef SAPDB_SLOW
    cbd13_CheckNodeTrace (TrError, FileId.fileRoot_gg00(), Nptr->nd_root());
#   endif

    if( PageNo != Nptr->nd_id() )
    {
        TrError = e_illegal_page_no;
        return;
    }

    if  ((NIL_PAGE_NO_GG00 != FileId.fileRoot_gg00()) &&
            ( Nptr->nd_root()  != FileId.fileRoot_gg00()))
    {
        if  (NIL_PAGE_NO_GG00 != Nptr->nd_root())
        {
            if ( FileId.fileTfn_gg00() == tfnMulti_egg00 )
            {
                if ( g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdPrim_gg00 )
                {
                    Kernel_VTrace()
                    << " bd13 invroot has changed:"
                    << " fileNo: " << FileDir_FileNoToString(FileId.fileTabId_gg00())
                    << ", fi_vers: " << SAPDBTrace_Hex(&FileId.fileVersion_gg00(),
                                                       sizeof(FileId.fileVersion_gg00()))
                    << ", root: " << FileId.fileRoot_gg00();
                }
                TrError = e_old_fileversion; // PTS 1128771 UH 2004-03-26
            }
            else
                TrError = e_page_in_wrong_tree;
        }
        else
            TrError = e_invalid_root;
        return;
    }

    switch( FileId.fileTfn_gg00() )
    {
    case tfnObjBefore_egg00 :
        if  ((ptData_egg00 != Nptr->nd_pt()) ||
                ( pt2Log_egg00 != Nptr->nd_pt2()))
        {
            TrError = e_inconsistent_nodetype;
            return;
        }
        break;
    case tfnObj_egg00 :
        SAPDBERR_ASSERT_STATE(FileId.fileContObjFileNo_gg00() == 0);

        if  (oftFixLenObjFile_egg00 == FileId.fileObjFileType_gg00())
        {
            if  (pt2Object_egg00 != Nptr->nd_pt2())
            {
                TrError = e_inconsistent_nodetype;
                return;
            }
        }
        else if  (oftVarLenObjFile_egg00 == FileId.fileObjFileType_gg00())
        {
            if  (pt2VarObject_egg00 != Nptr->nd_pt2())
            {
                TrError = e_inconsistent_nodetype;
                return;
            }
        }
        else if
        (
            ( pt2Object_egg00     != Nptr->nd_pt2() ) &&
            ( pt2VarObject_egg00  != Nptr->nd_pt2() )
        )
        {
            TrError = e_inconsistent_nodetype;
            return;
        }
        break;
    case tfnContObj_egg00 :
        SAPDBERR_ASSERT_STATE(FileId.fileContObjFileNo_gg00() > 0);
        if  (pt2ContObject_egg00 != Nptr->nd_pt2())
        {
            TrError = e_inconsistent_nodetype;
            return;
        }
        break;
    case tfnUndoLog_egg00 :
        if  ((ptData_egg00 != Nptr->nd_pt()) ||
                ( pt2UndoLog_egg00 != Nptr->nd_pt2()))
        {
            TrError = e_inconsistent_nodetype;
            return;
        }
        break;
    case tfnRedoLog_egg00 :
        if  ((ptData_egg00 != Nptr->nd_pt()) ||
                ( pt2RedoLog_egg00 != Nptr->nd_pt2()))
        {
            TrError = e_inconsistent_nodetype;
            return;
        }
        break;
    case tfnHistory_egg00 :
        if  ((ptData_egg00 != Nptr->nd_pt()) ||
                ( pt2History_egg00 != Nptr->nd_pt2()))
        {
            TrError = e_inconsistent_nodetype;
            return;
        }
        break;
    case tfnHistoryDirectory_egg00 :
        if  ((ptData_egg00 != Nptr->nd_pt()) ||
                ( pt2HistoryDirectory_egg00 != Nptr->nd_pt2()))
        {
            TrError = e_inconsistent_nodetype;
            return;
        }
        break;
    case tfnOpenTrans_egg00 :
        if  ((ptData_egg00 != Nptr->nd_pt()) ||
                ( pt2OpenTrans_egg00 != Nptr->nd_pt2()))
        {
            TrError = e_inconsistent_nodetype;
            return;
        }
        break;
    default:
        ;
    };

    if( Nptr->nd_id() == FileId.fileRoot_gg00() ){
        bd13_CheckRootNodeHeader( FileId, Nptr, TrError );
    }
};

/*---------------------------------------------------------------------------*/

void
bd13_CheckRootNodeHeader (
    const tgg00_FileId& FileId,
    const tbd_nodeptr   Nptr,
    tgg00_BasisError&   TrError)

{
    ROUTINE_DBG_MEO00 ("bd13_CheckRootNodeHeader");


    switch( FileId.fileTfn_gg00() )
    {
    case tfnInvSubtree_egg00 :
        if  (pt2InvSubtree_egg00 != Nptr->nd_pt2())
        {
            TrError = e_inconsistent_nodetype;
            return;
        }
        break;
    case tfnOmsInv_egg00 :
    case tfnMulti_egg00 :
        if  (pt2Inv_egg00 != Nptr->nd_pt2())
        {
            TrError = e_inconsistent_nodetype;
            return;
        }
        break;
    case tfnTempMulti_egg00 :
        if  (
            (
                (pt2InvSubtree_egg00 != Nptr->nd_pt2()) &&
                (pt2Inv_egg00        != Nptr->nd_pt2())
            )
            ||
            (!Nptr->nd_pmode().includes (pmTemp_egg00))
        )
        {
            TrError = e_inconsistent_nodetype;
            return;
        }
        break;
    case tfnColumn_egg00 :
        if  (pt2String_egg00 != Nptr->nd_pt2())
        {
            TrError = e_inconsistent_nodetype;
            return;
        }
        break;
    case tfnAux_egg00 :
        if  (g04inv_tfn (FileId.fileOldTfn_gg00()))
        {
            if  ((pt2InvSubtree_egg00 != Nptr->nd_pt2()) &&
                    ( pt2Inv_egg00        != Nptr->nd_pt2()))
            {
                TrError = e_inconsistent_nodetype;
                return;
            }
        }
        else
            TrError = e_ok;
        break;
    case tfnUndoLog_egg00 :
        if  (pt2UndoLog_egg00 != Nptr->nd_pt2())
        {
            TrError = e_inconsistent_nodetype;
            return;
        }
        break;
    case tfnRedoLog_egg00 :
        if  (pt2RedoLog_egg00 != Nptr->nd_pt2())
        {
            TrError = e_inconsistent_nodetype;
            return;
        }
        break;
    case tfnOpenTrans_egg00 :
        if  (pt2OpenTrans_egg00 != Nptr->nd_pt2())
        {
            TrError = e_inconsistent_nodetype;
            return;
        }
        break;
    case tfnHistory_egg00 :
        if  (pt2History_egg00 != Nptr->nd_pt2())
        {
            TrError = e_inconsistent_nodetype;
            return;
        }
        break;
    case tfnHistoryDirectory_egg00 :
        if  (pt2HistoryDirectory_egg00 != Nptr->nd_pt2())
        {
            TrError = e_inconsistent_nodetype;
            return;
        }
        break;
    default :
        if  ((pt2InvSubtree_egg00 == Nptr->nd_pt2()) ||
                ( pt2Inv_egg00        == Nptr->nd_pt2()) ||
                ( pt2String_egg00     == Nptr->nd_pt2()) )
        {
            TrError = e_inconsistent_nodetype;
            return;
        }
    };

    if  (FileId.fileType_gg00().includes(ftsTemp_egg00))
    {
        if  (!Nptr->nd_pmode().includes (pmTemp_egg00))
        {
            TrError = e_inconsistent_nodetype;
        }
        return;
    }

    // no ftsTemp_egg00 included
    if  (Nptr->nd_pmode().includes (pmTemp_egg00))
    {
        TrError = e_inconsistent_nodetype;
        return;
    }

    if  ((Nptr->nd_file_version() != FileId.fileVersion_gg00()) &&
            (!FileId.fileVersion_gg00().gg91IsDummyVers())          &&
            (tfnAux_egg00 != FileId.fileTfn_gg00()))
    {
        if ( g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdPrim_gg00 )
        {
            Kernel_VTrace()
            << "bd13 fileversion mismatch:"
            << "fileNo: " << FileDir_FileNoToString(FileId.fileTabId_gg00())
            << ", fileId fi_vers: " << SAPDBTrace_Hex(&FileId.fileVersion_gg00(),
                    sizeof(FileId.fileVersion_gg00()))
            << ", tree fi_vers: "
            << SAPDBTrace_Hex(&Nptr->nd_file_version(),
                              sizeof(Nptr->nd_file_version()));
        }
        TrError = e_old_fileversion;
        return;
    }
    else
    {
        if  ((Nptr->nd_file_state().includes(f_not_accessible)     )    &&
                (!FileId.fileBdUse_gg00().includes(bd_release_acc)))
        {
            // PTS 1118737 UH 2002-12-10 added messages
            tgg00_FileId auxFileId = FileId;
            b06write_filename_and_root(auxFileId);
            g01opmsg (sp3p_knldiag, sp3m_error, csp3_bd_msg, csp3_n_btree,
                      "File not accessible, pt2", Nptr->nd_pt2());
            TrError = e_file_not_accessible;
            return;
        }
    }

    if (FileId.fileBdUse_gg00().includes(bd_write_acc))
    {
        if  (Nptr->nd_file_state().includes(f_bad))
        {
            TrError = e_bad_file;
            Data_Exception errMsg( __CONTEXT__, DATA_WRN_BAD_FILE,
                                   SAPDB_ToString( FileId.fileRoot_gg00(), _T_d ));
            RTE_Message( errMsg );
            return;
        }
        if
        (
            (Nptr->nd_file_state().includes(f_read_only))       &&
            (!FileId.fileBdUse_gg00().includes(bd_load_nolog))  &&
            (!FileId.fileHandling_gg00().includes(hsNoLog_egg00))
        )
        {
            TrError = e_file_read_only;
            return;
        }
        if  (
            (FileId.fileHandling_gg00().includes(hsNoLog_egg00)) &&
            (! FileId.fileType_gg00().includes( ftsTemp_egg00 )) &&
            (! Nptr->nd_file_state().includes(f_read_only     )) &&
            (pt2Inv_egg00        != Nptr->nd_pt2()             ) &&
            (pt2InvSubtree_egg00 != Nptr->nd_pt2()             )
        )
        {
            TrError = e_file_must_be_read_only;
            return;
        }
    };

    if  ((tfnTable_egg00 == FileId.fileTfn_gg00()                     ) &&
            (cgg_nil_leafnodes != FileId.fileLeafNodes_gg00()         ) &&
            (Nptr->ndTreeLeaves_bd00() != FileId.fileLeafNodes_gg00()))
    {
#   ifdef SAPDB_SLOW
        t01int4 (bd, "bd13che tree", FileId.fileLeafNodes_gg00());
        t01int4 (bd, "bd13che root", Nptr->ndTreeLeaves_bd00());
#   endif
        TrError = e_old_leafnodes_number;
    }
};

/*---------------------------------------------------------------------------*/

void
bd13_CheckNodeErrorHandling(
    const tsp00_TaskId          taskId,
    const tgg00_BasisError      trError,
    const tbd_nodeptr           &pNode,
    const tgg00_RecoveryMode    recMode,
    const tsp00_Int4            value )
{
    switch( trError )
    {
    case e_data_page_corrupted:
        g01opmsg( sp3p_knldiag, sp3m_error, bd13x1IllegalBottom_csp03,
                  csp3_n_btree, "Invalid bottom          ", value);
        break;
    case e_illegal_entrypos:
        g01opmsg( sp3p_knldiag, sp3m_error, bd13x1IllegalEntry_csp03,
                  csp3_n_btree, "Illegal entry position  ", value );
        break;
    case e_illegal_entrylength:
        g01opmsg( sp3p_knldiag, sp3m_error, bd13x1IllegalEntry_csp03,
                  csp3_n_btree, "Illegal record length   ", value );
        break;
    case e_illegal_keylength:
        g01opmsg( sp3p_knldiag, sp3m_error, bd13x1IllegalEntry_csp03,
                  csp3_n_btree, "Illegal key len length  ", value );
        break;
    case e_illegal_key:
        g01opmsg( sp3p_knldiag, sp3m_error, bd13x1IllegalEntry_csp03,
                  csp3_n_btree, "Wrong key order         ", value );
        break;
    case e_illegal_record:
        g01opmsg( sp3p_knldiag, sp3m_error, bd13x1IllegalEntry_csp03,
                  csp3_n_btree, "Sum record len <> bottom", value );
        break;
    case e_illegal_page_no:
        g01opmsg( sp3p_knldiag, sp3m_error, bd13x1IllegalPageNo_csp03,
                  csp3_n_btree, "Illegal separator pageNo", value );
        break;
    default :
        g01opmsg( sp3p_knldiag, sp3m_error, bd13x1IllegalEntry_csp03,
                  csp3_n_btree, "unexpected base error   ", trError );
    }

    const tsp00_PageNo  pageNo = pNode->nd_id();

    g01opmsg( sp3p_knldiag, sp3m_error, bd13x1IllegalEntry_csp03,
              csp3_n_btree, "Corrupted data page     ", pageNo );

    Data_BasePage   page( pNode, rmStatic_egg00 != recMode, rmTemp_egg00 == recMode );
    Kernel_FileIO   dumpFile( KERNEL_DATA_FILE, KERNEL_COR_FILE_EXTENSION, SAPDB_ToString( pageNo, _T_d ));
    dumpFile.Write( page );
    dumpFile.Close();
};

/*---------------------------------------------------------------------------*/

void
bd13_BadDataPageHandling(
    tgg00_BasisError    &baseError,
    const tgg00_FileId  &fileId,
    const tsp00_PageNo  pageNo)
{
    tsp00_Line tfnText;
    tsp00_Int4 len = 0;
    g17tfntype_to_line( fileId.fileTfn_gg00(), len, tfnText );
    tfnText [len] = '\0';

    switch( fileId.fileTfn_gg00() )
    {
    case tfnUndoLog_egg00:
    case tfnRedoLog_egg00:
    case tfnHistory_egg00:
    case tfnOpenTrans_egg00:
    case tfnHistoryDirectory_egg00:
    case tfnObj_egg00:
    case tfnContObj_egg00:
        {
            // ReSet error code. The new error code yields to an emergency shutdown,
            // because no 'online recovery' with SQL technics is possible

            baseError = e_corrupted_datapage_from_io;

            Data_Exception errMsg( __CONTEXT__, DATA_ERR_CORRUPTED_PAGE,
                                   SAPDB_ToString( pageNo, _T_d ),
                                   SAPDB_ToString( fileId.fileRoot_gg00(), _T_d ), tfnText );
            RTE_Message( errMsg );
        }
        break;
    default:
        {
            // e_bad_datapage is handled by bd30ReleaseTree or bd300SetFileNotAccessible

            baseError = e_bad_datapage;

            Data_Exception errMsg( __CONTEXT__, DATA_ERR_BAD_PAGE,
                                   SAPDB_ToString( pageNo, _T_d ),
                                   SAPDB_ToString( fileId.fileRoot_gg00(), _T_d ), tfnText );
            RTE_Message( errMsg );
        }
    }
}

/*---------------------------------------------------------------------------*/
// PTS 1127038 TS 2004-02-06
void
bd13_IllegalPageNoHandling(
    tgg00_BasisError            &baseError,
    const tgg00_FileId          &fileId,
    const tsp00_PageNo          pageNo,
    const tgg00_RecoveryMode    recMode )
{
    ROUTINE_DBG_MEO00 ("bd13_IllegalPageNoHandling");

    if( rmStatic_egg00 == recMode )
    {
        baseError = e_invalid_oid;
        return;
    }

    // In case of a non liveCache instance this is a FATAL error
    // Kernel should be stopped by the calling task, after appending
    // some additional informations.

    baseError = e_illegal_page_no;

    const Data_PageNo maxPageNo = ( rmPerm_egg00 == recMode ) ?
                                  Converter_ICommon::Instance().MaxPermPageNo() :
                                  Converter_ICommon::Instance().MaxTempPageNo();

    const SAPDB_Char *recModeString = ( rmPerm_egg00 == recMode ) ?
                                      "permanent" : "temporary";

    Data_Exception errMsg( __CONTEXT__, DATA_ERR_PAGENO_OUT_OF_RANGE,
                           recModeString, SAPDB_ToString( pageNo, _T_d ),
                           SAPDB_ToString( fileId.fileRoot_gg00(), _T_d ),
                           SAPDB_ToString( maxPageNo, _T_d ));
    RTE_Message( errMsg );

    eo670_CTraceStack();
}

/*---------------------------------------------------------------------------*/

void
bd13_WriteOldOccupant(
    const tsp00_TaskId          taskId,
    tbd_nodeptr                 pNode,
    tbd_nodeptr                 pCblock,
    const tsp00_PageNo          PageNo,
    const tgg00_RecoveryMode    recMode, // recovery mode of the new page
    tbd02_CachePageState       &nodeState,
    const tsp00_Bool            bLockPageExclusive)

{
    ROUTINE_DBG_MEO00 ("bd13_WriteOldOccupant");

    Data_BasePage   Page( pNode, !gg04IsStaticPage( pNode->nd_pt2()),
                          pNode->nd_pmode().includes( pmTemp_egg00 ), pCblock );

    IOMan_IDataIO::GetInstance().WriteDataPage( taskId, Page );

    if( nodeState.cpsFound_bd02 )
    {
        bd20ForcedIODone( pCblock, taskId );
        nodeState.cpsIoDone_bd02 = true;
    }
    else
        bd20ReplaceOldOccupant( taskId, PageNo, recMode, pCblock, bLockPageExclusive );
}

/*---------------------------------------------------------------------------*/

inline void
bd13_OverflowReaction (
    const tsp00_TaskId      taskId,
    const tsp00_PageNo      pageNo,
    const tgg00_MessType    actionType,
    const tsp00_Int4        suspendReason,
    tgg00_BasisError        &trError,
    tsp00_Int               &retryCount )
{
    const tsp00_Int4 defaultRetries = 11;
    const tsp00_Int4 maxRetries     = 61;

    if( e_sysbuffer_overflow == trError )
    {
        vsleep( taskId, 1 );
        ++retryCount;

        if( 0 == ( retryCount % 10 ))
        {
            g01opmsg( sp3p_knldiag, sp3m_warning, csp3_b20_lru_overflow_1,
                      csp3_n_datacache, "DATA CACHE OVERFLOW     ", taskId );
        }

        if( retryCount <= defaultRetries )
        {
            trError = e_ok;
            return;
        }
        // be more soft, if parallel index creation is active
        // and calling task is not the index builder
        if( cbd490_CreateIndexParallel::IsActive() && ( actionType != m_create_index_parallel ))
        {
            if( retryCount <= maxRetries )
            {
                trError = e_ok;
                return;
            }
        }
        //  page acces will fail with e_sysbuffer_overflow
        g01opmsg( sp3p_knldiag, sp3m_warning, csp3_b20_lru_overflow_2,
                  csp3_n_datacache, "DATA CACHE OVERFLOW     ", taskId );
        return;
    }
    if( e_no_more_temp_space == trError )
    {
        vsuspend( taskId, suspendReason );
        trError = e_ok;
    }
}

/*---------------------------------------------------------------------------*/

void
bd13_FrameForNewNode(
    tgg00_TransContext         &trans,
    tgg00_FileId               &fileId,
    tsp00_Int2                 &refCounter,
    const tsp00_Int2           level,
    const tsp00_PageNo         newPagePno,
    const tgg00_RecoveryMode   recMode,
    const tgg00_MessType       actionType,
    tbd_node_ptrs              &pNodes )

{
    ROUTINE_DBG_MEO00 ("bd13_FrameForNewNode");


    bool                 bIsFirstCall = true;
    bool                 bIsFirstInit = true;
    tsp00_Int            RetryCount   = 0;
    tbd02_CachePageState nodeState;

    tgg00_BasisError    &trError = trans.trError_gg00;

#   ifdef SAPDB_SLOW
    t01int4 (bd_io, "CurrCounter   ", refCounter);
#   endif

    trError           = e_ok;
    pNodes.np_cbptr() = NULL;

    bd13_InitPageState( nodeState, plmLock_ebd00, bIsFirstInit );
    nodeState.cpsSyncWithSavepoint_bd02.becomes( synNone_ebd02 );

    while (e_ok == trError)
    {
        bd20NewPage (trans.trTaskId_gg00, trError, fileId, newPagePno, recMode,
                     pNodes.np_ptr(), REFCAST_MEO00 (tbd02_pDataCBlock) &pNodes.np_cbptr(), nodeState);

        if  (e_ok != trError)
        {
            // data cache overflow handling
            if ((e_sysbuffer_overflow == trError) || (e_no_more_temp_space == trError))
            {
                bd13_OverflowReaction( trans.trTaskId_gg00, newPagePno,
                                       actionType, 46, trError, RetryCount );
                continue;
            }
            break; // fatal error has occured
        }

        // caused by the synchronization between data cache and converter it
        // is possible that a new pageno is temporary in the data cache, although
        // the corresponding converter slot indicates that the pageno is free!
        if( nodeState.cpsWaitForWrite_bd02 )
        {
            bd13_MsgAndSuspend (trans.trTaskId_gg00, newPagePno, b_await_w, WAIT_FOR_WRITE, 56);
            nodeState.cpsWaitForWrite_bd02 = false;
            continue;
        }

        // before reading the request page into the data cache the assigned
        // data cache frame has to be written onto disk, because the page on
        // the frame is changed.
        if( nodeState.cpsDirty_bd02 )
        {
            bd13_WriteOldOccupant( trans.trTaskId_gg00, pNodes.np_ptr(), pNodes.np_cbptr(),
                                   newPagePno, recMode, nodeState, LOCK_PAGE_EXCLUSIVE_BD13 );
        }
        b13init_default_data_page( fileId, level, newPagePno, pNodes );
        break;
    }
    ++trans.trIoCount_gg00;

    if( pNodes.np_ptr() != NULL )
        ++refCounter;

#   ifdef SAPDB_SLOW
    if  (e_ok == trError)
    {
        t01int4 (bd_io, "new pno     ", pNodes.np_ptr()->nd_id());
    }
#   endif
};

/*---------------------------------------------------------------------------*/

void
bd13_FreeNode (
    tgg00_TransContext          &trans,
    tgg00_FileId                &fileId,
    const tsp00_PageNo          pageNo,
    const tgg00_RecoveryMode    recMode,
    const tgg00_MessType         actionType,
    const Converter_Version     &pageConverterVersion,
    tsp00_Int2                  &refCounter )
{
    ROUTINE_DBG_MEO00 ("bd13_FreeNode");

    bool                 bIsFirstInit      = true;
    tsp00_Bool           bFirstCall        = true;
    tsp00_Bool           bPageLockedToFree = false;
    tsp00_Int            retryCount        = 0;
    tbd02_CachePageState nodeState;
    tbd_node_ptrs        pNodes;

    tgg00_BasisError     &trError = trans.trError_gg00;
    const RTETask_ITask  &task    = *RTETask_ITask::ByTaskID( trans.trTaskId_gg00 );


#   ifdef SAPDB_SLOW
    switch( recMode )
    {
    case rmTemp_egg00:
        t01int4( bd_io, "free tmp pno", pageNo );
        break;
    case rmPerm_egg00:
        t01int4( bd_io, "free per pno", pageNo );
        break;
    case rmStatic_egg00:
        t01int4( bd_io, "free sta pno", pageNo );
        break;
    default:
        ;
    }
#   endif

    bd13_InitPageState( nodeState, plmLock_ebd00, bIsFirstInit );
    nodeState.cpsSyncWithSavepoint_bd02.becomes( synNone_ebd02 );

    while(( e_ok == trError ) && ( !bPageLockedToFree ))
    {
        bd20LockPageForFree( task.ID(), trError, fileId, pageNo, recMode,
                             bFirstCall, bPageLockedToFree, nodeState, pNodes.np_ptr(),
                             REFCAST_MEO00 (tbd02_pDataCBlock) &pNodes.np_cbptr() );

        if( e_ok != trError )
        {
            if(( e_sysbuffer_overflow == trError ) || ( e_no_more_temp_space == trError ))
            {
                bd13_OverflowReaction( task.ID(), pageNo, actionType, 240, trError, retryCount );
                continue;
            }
            break; //fatal error has occured
        }

        bFirstCall = false;
        if( nodeState.cpsWaitForWrite_bd02 )
        {
            bd13_MsgAndSuspend( task.ID(), pageNo, b_await_w, WAIT_FOR_WRITE, 44 );
            nodeState.cpsWaitForWrite_bd02 = false;
            continue;
        }
        if( nodeState.cpsDirty_bd02 )
        {
            bd13_WriteOldOccupant( task.ID(), pNodes.np_ptr(), pNodes.np_cbptr(),
                                   pageNo, recMode, nodeState, ! LOCK_PAGE_EXCLUSIVE_BD13 );
            continue;
        }
    }

    if( e_ok != trError )
        return;

    // pNodes.np_cbptr() is available but pNodes.np_ptr() do not have to point to the valid page

    switch( recMode )
    {
    case rmTemp_egg00:
        {
            SAPDBERR_ASSERT_STATE( fileId.fileType_gg00().includes( ftsTemp_egg00 ));

            Converter_IPageNoManager::Instance().FreeTempPageNo( task, pageNo );

            if( ! fileId.fileType_gg00().includes( ftsShared_egg00 )){
                --trans.trTempCount_gg00;
            }
        }
        break;
    case rmPerm_egg00:
        Converter_IPageNoManager::Instance().FreePermPageNo(
            task, pageNo, pageConverterVersion );
        break;
    case rmStatic_egg00:
        Converter_IPageNoManager::Instance().FreeStaticPageNo(
            task, pageNo, pageConverterVersion );
        break;
    default:
        ;
    }

    if( 0 < bd20GetPageUsageCount( pNodes.np_cbptr() ))
    {
        // Caused by the different possibilities to free a page it's difficult to decide
        // whether the page reference count should be decremented or not. For example:
        // bd13GetNode -> bd13RRelease -> bd13FreePageNo(bd13_FreeNode) => no decrement
        // bd13GetNode -> bd13FreePageNo(bd13_FreeNode) => decrement
        // bd13FreePageNo(bd13_FreeNode) without reading the temporary page before => no decrement

        --refCounter;
    }

    bd20FreePage( task.ID(), pageNo, recMode, trError, trans.trBdExclFileLocks_gg00 );

#   ifdef SAPDB_SLOW
    t01int4( bd_io, "CurrCounter ", refCounter );
#   endif

};

/*---------------------------------------------------------------------------*/

#ifdef SAPDB_SLOW
inline void
bd13_GetNodeTraceInfo(
    const tsp00_PageNo              pageNo,
    const tgg00_RecoveryMode        recMode,
    const SAPDB_Bool                bIsRoot,
    const tbd_node_request_Param    nodeRequest )
{
    switch( recMode )
    {
    case rmTemp_egg00 :
        if( bIsRoot ){
            t01int4( bd_io, "GetTempRoot ", pageNo );
        }
        else
            t01int4( bd_io, "GetTempPno  ", pageNo );
        break;
    case rmPerm_egg00:
        if( bIsRoot ){
            t01int4( bd_io, "GetPermRoot ", pageNo );
        }
        else
            t01int4( bd_io, "GetPermPno  ", pageNo );
        break;
    case rmStatic_egg00:
        t01int4( bd_io, "GetStaticPno", pageNo );
        break;
    default:
        ;
    }

    switch( nodeRequest )
    {
    case nr_for_read:
        t01sname( bd_io, "Read Request" );
        break;
    case nr_for_update:
        t01sname( bd_io, "Upd Request " );
        break;
    default:
        t01sname( bd_io, "NoOp Request" );
    }
};
#endif

/*---------------------------------------------------------------------------*/

void
bd13_LoadOneNode(
    Trans_Context&      trans,
    tgg00_FileId&       fileId,
    Data_PageNo         pageNo,
    tgg00_RecoveryMode  recMode,
    tbd_node_ptrs&      pNodes,
    bool&               bDoReadIO )
{
    const pasbool   bSetToChanged = false;
    const pasbool   bIsClustered  = true;
    bool            bPageRead     = false;
    bool            bIsFirstInit  = true;
    tgg00_PageType  pageType;   // dummy
    tsp00_Int       retryCount = 0;

    tbd02_CachePageState    nodeState;
    nodeState.cpsSyncWithSavepoint_bd02.becomes( synNone_ebd02 );

    pNodes.np_cbptr() = 0;
    pNodes.np_ptr()   = 0;
    bDoReadIO         = false;

    const RTE_TaskId&   taskId = trans.Task().ID();
    while( e_ok == trans.OldTrans().trError_gg00 )
    {
        bd13_InitPageState( nodeState, plmLock_ebd00, bIsFirstInit );

        bd20LoadPage( taskId, pageNo, recMode, bSetToChanged, bIsClustered, pageType,
                      trans.OldTrans().trError_gg00, fileId, pNodes.np_ptr(), pNodes.np_cbptr(), nodeState );

        if( e_ok != trans.OldTrans().trError_gg00 )
        {
            // data cache overflow handling
            if(
                ( e_sysbuffer_overflow == trans.OldTrans().trError_gg00 ) ||
                ( e_no_more_temp_space == trans.OldTrans().trError_gg00 ))
            {
                bd13_OverflowReaction( taskId, pageNo, tgg00_MessType::fromConst( m_nil ),
                                       43, trans.OldTrans().trError_gg00, retryCount );
                continue;
            }
            break; // fatal error has occured
        }
        // ============= data cache contains requested page =============
        if( nodeState.cpsFound_bd02 )
        {   // requested page is still in read IO by another task
            if( nodeState.cpsDirty_bd02 )
            {
                bd13_MsgAndSuspend( taskId, pageNo, b_await_r, WAIT_FOR_READ, 41 );
                continue;
            }
            break;
        }
        // ============= data cache doesn't contains requested page =============
        if( nodeState.cpsWaitForWrite_bd02 )
        {  // requested page is still in write IO by another task
            bd13_MsgAndSuspend( taskId, pageNo, b_await_w, WAIT_FOR_WRITE, 42 );
            continue;
        }
        // before reading the request page into the data cache the assigned
        // data cache frame has to be written to disk, because the page on
        // the frame is changed.
        if( nodeState.cpsDirty_bd02 )
        {
            bd13_WriteOldOccupant( taskId, pNodes.np_ptr(), pNodes.np_cbptr(),
                                   pageNo, recMode, nodeState, ! LOCK_PAGE_EXCLUSIVE_BD13 );
        }
        // execute read operation for the requested page
        bDoReadIO = true;
        break; // data cache is reserved; read execution will be done outside
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
