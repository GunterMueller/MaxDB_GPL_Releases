/*
  @file           vbd360.cpp
  @author         TorstenS
  @ingroup        
  @brief          scanning leaves for qualified primary and temporary records

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2005 SAP AG

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

#include "gsp00.h"   // PASCAL: SP_basic_constants_and_types
#include "ggg00.h"   // PASCAL: GG_kernel_constants_and_types
#include "gbd00.h"   // PASCAL: BD_kernel_constants_and_types

#include "hbd13.h"    // PASCAL: Nodehandling
#include "hbd13_3.h"  // PASCAL: Nodehandling
#include "hbd20_2.h"  // PASCAL: DataCache
#include "hbd30.h"    // PASCAL: Treehandling  
#include "hbd31.h"    // PASCAL: Leafhandling
#include "hbd73.h"    // PASCAL: Statistic
#include "hbd360.h"   // own header file  
#include "hbd998.h"   // FileDirWrapper
#include "hsp30.h"    // PASCAL: Buffer handling and comparison routines
#include "hkb53.h"
#include "hkb71.h"

#include "ConsistentRead/CnsRead_CachedRow.hpp"
#include "Converter/Converter_PageCollector.hpp"
#include "DataAccess/Data_BasePage.hpp"
#include "DataAccess/Data_Types.hpp"
#include "DataAccess/Data_Row.hpp"
#include "DataAccess/Data_PageFrame.hpp"
#include "DataAccess/Data_IOBuffer.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "FrameControl/FrameCtrl_ICommon.hpp"
#include "KernelCommon/Kernel_Common.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "KernelCommon/Kernel_FileIO.hpp"
#include "IOManager/IOMan_ClusterAddress.hpp"
#include "IOManager/IOMan_IDataInfo.hpp"
#include "RunTime/RTE_Message.hpp"
#include "SAPDBCommon/Messages/SDBMsg_Data.h"
#include "SAPDBCommon/SAPDB_RangeCode.hpp"
#include "Transaction/Trans_Context.hpp"


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
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

inline bool
bd360_AssignRecKey(
    Trans_Context&          trans,
    tgg00_FileId&           fileId,
    tsp00_KeyPtr            pCurrRecKey,
    SAPDB_Int2&             currRecKeyLen,
    const tbd_node_ptrs&    pLeaf,
    const SAPDB_Int4        recIndex );

/*---------------------------------------------------------------------------*/

void
bd360_ClusteredRead(
    Trans_Context&              trans,
    tbd_current_tree&           current,
    tgg00_SelectFieldsParam&    sel,
    const Data_PageNo           leafPageNo,
    Converter_PageCollector&    pageCollection,
    Data_IOBuffer&              ioBuffer );

/*---------------------------------------------------------------------------*/

void
bd360_CopyRowAndDoQualificationTest(
    Trans_Context&              trans,
    tbd_current_tree&           current,
    tgg00_SelectFieldsParam&    sel,
    tgg00_StackDesc&            stackDesc,
    tbd_node_ptrs&              pLeaf,
    const tgg00_RecPtr          pCopySpace,
    tgg00_RecPtr&               pRec,
    SAPDB_Int4&                 recIndex,
    pasbool&                    bIsNotQualified,
    tbd_searchresult&           searchResult );

/*---------------------------------------------------------------------------*/

void
bd360_MakeQualificationTestWithACopiedRow(
    Trans_Context&              trans,
    tbd_current_tree&           current,
    tgg00_SelectFieldsParam&    sel,
    tgg00_StackDesc&            stackDesc,
    tbd_node_ptrs&              pLeaf,
    const tgg00_RecPtr          pCopySpace,
    tgg00_RecPtr&               pRec,
    SAPDB_Int4&                 recIndex,
    tgg00_LockReqMode&          grantedLock,
    pasbool&                    bIsNotQualified );

/*---------------------------------------------------------------------------*/

inline void
bd360_Monitoring(
    const tgg00_MessType& messType,
    const bool            bIsQualified,
    const bool            bIsTable,
    const bool            bIsTempResult );

/*---------------------------------------------------------------------------*/

inline void
bd360_NextRecord(
    Trans_Context&              trans,
    tbd_current_tree&           current,
    tbd_node_ptrs&              pLeaf,
    SAPDB_Int4&                 recIndex,
    tsp00_PageNo&               unlockedPageNo,
    tgg00_SelectFieldsParam&    sel,
    const bool                  bClusteredRead,
    Converter_PageCollector&    pageCollection,
    Data_IOBuffer&            ioBuffer );

/*---------------------------------------------------------------------------*/

inline void
bd360_PrevRecord(
    Trans_Context&              trans,
    tbd_current_tree&           current,
    tbd_node_ptrs&              pLeaf,
    SAPDB_Int4&                 recIndex,
    tsp00_KeyPtr                pCurrRecKey,
    SAPDB_Int2                  currRecKeyLen,
    tsp00_PageNo&               unlockedPageNo,
    tgg00_SelectFieldsParam&    sel );

/*---------------------------------------------------------------------------*/

inline void
bd360_RemoveCommittedReadShareLock(
    Trans_Context&  trans,
    tgg00_FileId&   fileId,
    tgg00_Lkey&     lockKey,
    pasbool&        bDoRemoveLock );

/*---------------------------------------------------------------------------*/

void
bd360_ReportDamagedDataPage(
    Trans_Context&          trans,
    tgg00_FileId&           fileId,
    const tbd_node_ptrs&    pLeaf,
    const tsp00_Int4        recIndex,
    const tsp00_Int4        recPos );

/*---------------------------------------------------------------------------*/

void
bd360_ResultHandling(
    Trans_Context&              trans,
    tgg00_SelectFieldsParam&    sel,
    SAPDB_Int4&                 currResultCnt,
    SAPDB_Int4&                 lastResultLen,
    SAPDB_Int4&                 recBufferPos,
    const SAPDB_Int4            recBufferSize,
    tsp00_MoveObjPtr            pRecBuffer,
    const tgg00_RecPtr          pRec,
    const SAPDB_Int4            initResultLen,
    const bool                  bWithTransHistoryInfo );

/*---------------------------------------------------------------------------*/

void
bd360_SearchStartKeyForNextScan(
    Trans_Context&              trans,
    tbd_current_tree&           current,
    tsp00_KeyPtr                pCurrRecKey,
    SAPDB_Int2&                 currRecKeyLen,
    tbd_node_ptrs&              pLeaf,
    SAPDB_Int4&                 recIndex,
    tgg00_SelectFieldsParam&    sel );

/*---------------------------------------------------------------------------*/
void
bd360_SearchStartKeyForPrevScan(
    Trans_Context&              trans,
    tbd_current_tree&           current,
    tsp00_KeyPtr                pCurrRecKey,
    SAPDB_Int2&                 currRecKeyLen,
    tbd_node_ptrs&              pLeaf,
    SAPDB_Int4&                 recIndex,
    tgg00_SelectFieldsParam&    sel );

/*---------------------------------------------------------------------------*/

inline bool
bd360_BeyondStopKey(
    Trans_Context&      trans,
    const bool          bIsAscendingOrder,
    const tsp00_KeyPtr  pCurrRecKey,
    const SAPDB_Int2    currRecKeyLen,
    const tsp00_KeyPtr  pStopKey,
    const SAPDB_Int2    stopKeyLen )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd360_BeyondStopKey", DataTree_Trace, 8 );

    tsp00_LcompResult   compResult;
    s30cmp( pCurrRecKey, POS_OFF_DIFF_BD00, currRecKeyLen,
            pStopKey, POS_OFF_DIFF_BD00, stopKeyLen, compResult );
    if( bIsAscendingOrder )
    {
        if( l_greater == compResult )
        {
            trans.SetErrorCode( e_no_next_record );
            return true;
        }
        return false;
    }
    else
    {
        if( l_less == compResult )
        {
            trans.SetErrorCode( e_no_prev_record );
            return true;
        }
        return false;
    }
}

/*---------------------------------------------------------------------------*/

inline bool
bd360_CheckForClusteredRead(
    const bool                  bIsTempTable,
    tgg00_SelectFieldsParam&    sel,
    Converter_PageCollector&    pageCollection,
    Data_IOBuffer&              ioBuffer,
    Kernel_VTrace&              vTrace )
{
    bool bDoClusteredRead = false;
    tgg00_ClusteredIOParam& clustInfo = sel.sfp_cluster_info();
    if(( clustInfo.clust_bDoClusteredRead_gg00 )&&( ! bIsTempTable ))
    {
        if(
            ( ioBuffer.Initialize( clustInfo.clust_BufferSize_gg00,
                                   (SAPDB_Byte*)clustInfo.clust_pIOBuffer_gg00 ))
            &&
            ( pageCollection.Initialize())
        ){
            bDoClusteredRead = true;
        }
    }
    if( DataTree_Trace.TracesLevel( 5 ))
    {
        if( sel.sfp_cluster_info().clust_bDoClusteredRead_gg00 ){
            if( bDoClusteredRead ){
                vTrace << "bd360NextQual: try clustered read" << FlushLine;
            }else{
                vTrace << "bd360NextQual: no clustered read possible" << FlushLine;
            }
        }
    }
    return bDoClusteredRead;
}

/*---------------------------------------------------------------------------*/

inline Data_PageNo
bd360_GetIndexPageNo( const tbd_current_tree& current )
{
    SAPDBERR_ASSERT_STATE( ! current.curr_tree_id.fileType_gg00().includes( ftsTemp_egg00 ));

    if( FIRST_INDEX_LEVEL_BD00 < current.currRootNptrs_bd00.np_ptr()->nd_level()){
        return current.currIndexNptrs_bd00.np_ptr()->nd_id();
    }
    return current.currRootNptrs_bd00.np_ptr()->nd_id();
}

/*---------------------------------------------------------------------------*/

inline const tbd_node_ptrs&
bd360_GetIndexNptrs( const tbd_current_tree& current )
{
    SAPDBERR_ASSERT_STATE( LEAF_LEVEL_BD00 < current.currRootNptrs_bd00.np_ptr()->nd_level());
    SAPDBERR_ASSERT_STATE( ! current.curr_tree_id.fileType_gg00().includes( ftsTemp_egg00 ));

    if( FIRST_INDEX_LEVEL_BD00 < current.currRootNptrs_bd00.np_ptr()->nd_level()){
        return current.currIndexNptrs_bd00;
    }
    return current.currRootNptrs_bd00;
}

/*---------------------------------------------------------------------------*/

inline SAPDB_Int4
bd360_GetRecordPos(
    const tbd_node_ptrs&    pLeaf,
    const SAPDB_Int4        recIndex )
{
    SAPDBERR_ASSERT_STATE( 0 < pLeaf.np_ptr()->nd_record_cnt());
    SAPDBERR_ASSERT_STATE( recIndex < pLeaf.np_ptr()->nd_record_cnt());

    return pLeaf.np_ptr()->nd_pointer_list()[ MAX_POINTERINDEX_BD00 - recIndex - POS_OFF_DIFF_BD00 ];
}

/*---------------------------------------------------------------------------*/

inline tgg00_RecPtr
bd360_GetRecordPtr(
    const tbd_node_ptrs&    pLeaf,
    const SAPDB_Int4        recPos )
{
    SAPDBERR_ASSERT_STATE( recPos > NODE_HEADER_MXBD00 );
    SAPDBERR_ASSERT_STATE( recPos < pLeaf.np_ptr()->nd_bottom());

    return reinterpret_cast< tgg00_RecPtr >
           ( &pLeaf.np_ptr()->nd_body()[ recPos - NODE_HEADER_MXBD00 - POS_OFF_DIFF_BD00 ]);
}

/*---------------------------------------------------------------------------*/

inline SAPDB_Int4
bd360_GetRecordCnt(  const tbd_node_ptrs&    pLeaf )
{
    return pLeaf.np_ptr()->nd_record_cnt();
}
/*---------------------------------------------------------------------------*/

inline static void
bd360_LkeyAssign (
    Trans_Context&          trans,
    const SAPDB_Int4        sourceKeyLen,
    const tsp00_Key* const  pSourceKey,
    tgg00_Lkey&             targetLKey )
{
    SAPDB_RangeMove( __FILE__,   3,
                     sizeof( tsp00_Key ), sizeof( targetLKey.keyVal_gg00()),
                     pSourceKey, POS_OFF_DIFF_BD00,
                     targetLKey.keyVal_gg00(), POS_OFF_DIFF_BD00,
                     sourceKeyLen , trans.OldTrans().trError_gg00 );
    if( e_move_error != trans.OldTrans().trError_gg00 ){
        targetLKey.keyLen_gg00() = sourceKeyLen;
    }
}

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

externPascal void
bd360NextQualifiedRecordFromTree(
    tbd_current_tree&           current,
    tsp00_KeyPtr                pCurrRecKey,
    SAPDB_Int2&                 currRecKeyLen,
    const tsp00_KeyPtr          pStopKey,
    const SAPDB_Int2            stopKeyLen,
    tgg00_Lkey&                 lockKey,
    const SAPDB_Int4            recBufferSize,
    tsp00_MoveObjPtr            pRecBuffer,
    SAPDB_Int4&                 recBufferPos,
    tgg00_SelectFieldsParam&    sel,
    tgg00_StackDesc&            stackDesc,
    SAPDB_Int&                  currResultCnt,
    pasbool&                    bIsNotQualified,
    pasbool&                    bDoRemoveSQLLock,
    tgg00_LockReqMode&          grantedLock,
    tsp00_PageNo&               unlockedPageNo )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd360NextQual", DataTree_Trace, 5 );

    Trans_Context&      trans   = Trans_Context::GetContext( *current.curr_trans );
    tgg00_FileId&       fileId  = current.curr_tree_id;

    SAPDB_Int4      lastResultLen = 0;
    SAPDB_Int4      initResultLen = sel.sfp_result_length();
    SAPDB_Int4      recIndex;
    tbd_node_ptrs   pLeaf;

    pLeaf.np_ptr()  = 0;
    pLeaf.np_cbptr()= 0;
    bIsNotQualified = false;

    tgg00_Rec   auxRecordSpace; // tgg00_Rec used for packed records and UDF

    const bool bDoCopyRow = stackDesc.mstack_state().includes( ssCopyRow_egg00 );
    const bool bIsUpdDel  = ( m_update == sel.sfp_bd_mess_type() || m_delete == sel.sfp_bd_mess_type());

    const bool bIsTable      = ( tfnTable_egg00 == fileId.fileTfn_gg00());
    const bool bIsTempTable  = fileId.fileType_gg00().includes( ftsTemp_egg00 );
    const bool bIsTempResult = ( bIsTempTable && ( bd998GetExtendedTempFileType( trans.OldTrans(), fileId ) <= ttfnTempTable_egg00 ));
    const bool bDoSQLLocking = ( ! bIsTempTable )&&( bIsUpdDel || CnsReadNone == trans.OldTrans().trConsistReadInfo_gg00 );
    const bool bWithTransHistoryInfo    = (( bIsUpdDel )&&( ! bIsTempTable ));
    const Data_PageRecoveryMode recMode = ( bIsTempTable ? Data_PageRecoveryMode( Data_Dynamic,
                                            Data_NotRecoverable ):Data_PageRecoveryMode());
    Kernel_VTrace           vTrace;
    Converter_PageCollector pageCollection( trans.Allocator(), recMode );
    Data_IOBuffer           ioBuffer;

    const bool bClusteredRead = bd360_CheckForClusteredRead( bIsTempTable, sel, pageCollection, ioBuffer, vTrace );

    if(( bDoCopyRow )&&( current.currExtendedFileTypeSet_bd00.includes( eftsRecordsPacked_egg00 )))
    {
        trans.SetErrorCode( e_not_implemented );
        trans.ErrorList() = Msg_List( Msg_List::Warning, SDBMSG_DATA_UDF_NOT_PACKED_TABLES );
        return;
    }
    //  Seek to the first relevant record of the search area
    bd360_SearchStartKeyForNextScan( trans, current, pCurrRecKey, currRecKeyLen, pLeaf, recIndex, sel );

    if(( trans.IsOk())&&( mm_first == sel.sfp_bd_mess2_type())){
        sel.sfp_bd_mess2_type().becomes( mm_next );
    }
    if(( trans.IsOk())&&( bClusteredRead )){
        bd360_ClusteredRead( trans, current, sel, pLeaf.np_ptr()->nd_id(), pageCollection, ioBuffer );
    }
    while( trans.IsOk())
    {
        bIsNotQualified = false;
        if( ! bd360_AssignRecKey( trans, fileId, pCurrRecKey, currRecKeyLen, pLeaf, recIndex )){
            break;
        }
        if(
            ( sel.sfp_bd_use_stopkey())&&
            ( bd360_BeyondStopKey( trans, true, pCurrRecKey, currRecKeyLen, pStopKey, stopKeyLen ))){
            break;
        }
        const SAPDB_Int4 recPos = bd360_GetRecordPos( pLeaf, recIndex );
        tgg00_Rec*       pRec   = bd360_GetRecordPtr( pLeaf, recPos );

        if( Data_Row::ToRow( *pRec ).IsPacked())
        {   // record is stored in compressed layout
            Data_Row::ToRow( *pRec ).Unpack( Data_Row::ToRow( auxRecordSpace ));
            pRec = &auxRecordSpace;
        }
        if( bDoCopyRow )
        {
            // caused by user defined functions it could happen that the current tree
            // will be accessed again and a deadlock could occur therefore it is
            // neccessary to copy the record and release the tree
            const tgg00_RecPtr  pCopySpace = &auxRecordSpace;
            bd360_MakeQualificationTestWithACopiedRow( trans, current, sel, stackDesc, pLeaf,
                    pCopySpace, pRec, recIndex, grantedLock, bIsNotQualified );
        }
        else
        {
            CnsRead_CachedRow* pRow = reinterpret_cast< CnsRead_CachedRow* >( pRec );
            bIsNotQualified = ( ! pRow->IsQualified( trans.OldTrans(), sel, stackDesc ));
        }
#ifdef SAPDB_SLOW 
        if( DataTree_Trace.TracesLevel( 5 ))
        {
            if( bIsNotQualified ){
                vTrace << "bd360NextQual: record is not qualified ";
            }else{
                vTrace <<"bd360NextQual: record is qualified ";
            }
            vTrace << "(" << Kernel_BasisErrorToString( trans.OldTrans().trError_gg00 ) << ")" << FlushLine;
        }
#endif           
        bd360_Monitoring( sel.sfp_bd_mess_type(), ! bIsNotQualified, bIsTable, bIsTempResult );

        if( bIsNotQualified )
        {   // record does not fullfil qualification
            if( mm_direct != sel.sfp_bd_mess2_type()){
                trans.ResetErrorCode(); // go to next record
            }
            else
            {
                if( sel.sfp_bd_return_knf())
                {
                    const SAPDB_Int4 recCount = bd360_GetRecordCnt( pLeaf );
                    if(( 1 < recCount )&&( recIndex < ( recCount -1 )))
                    {
                        const tgg00_BasisError auxError = trans.OldTrans().trError_gg00;
                        trans.ResetErrorCode();
                        recIndex +=1;
                        if( bd360_AssignRecKey( trans, fileId, pCurrRecKey, currRecKeyLen, pLeaf, recIndex )){
                            trans.OldTrans().trError_gg00 = auxError;
                            break; // finish scan access
                        }
                    }
                    sel.sfp_bd_return_knf() = false;
                    break; // finish scan access
                }
            }
        }
        else // record fullfil qualification (bIsNotQualified == false)
        {
            if(( e_buffer_limit == trans.OldTrans().trError_gg00 )&& ( 0 < currResultCnt )) // TS 1120770 2003-02-21
            {
                sel.sfp_m_result_cnt() = currResultCnt;
                break;
            }
            if( ! trans.IsOk()){
                break;
            }
            if( bDoSQLLocking )
            {
                fileId.fileHandling_gg00().addElement(hsNoWaitForInserts_egg00);
                k53row_lock_by_key( trans.OldTrans(), fileId, pCurrRecKey, currRecKeyLen,
                                    sel.sfp_bd_mess_type(), sel.sfp_act_cntresult(), grantedLock );
            }
            if( trans.IsOk()){
                bd360_ResultHandling( trans, sel, currResultCnt, lastResultLen, recBufferPos,
                                      recBufferSize, pRecBuffer, pRec, initResultLen,
                                      bWithTransHistoryInfo );
            }
            if( bDoRemoveSQLLock ){
                bd360_RemoveCommittedReadShareLock( trans, fileId, lockKey, bDoRemoveSQLLock );
            }
            switch( trans.OldTrans().trError_gg00 )
            {
            case e_lock_collision_with_inserted_row:
                // skip current record because it was just inserted
                trans.ResetErrorCode();
                break;
            case e_wait_for_lock_release:
                {
                    if( ! fileId.fileHandling_gg00().includes( hsCollisionTest_egg00 )){
                        break;
                    }
                    // PTS 1117878 UH 2002-10-17
                    // In isolation level 1 (committed read) a row share lock is requested
                    // if an exclusive row lock exist. After the share lock is available
                    // and the record is read the lock must be removed! Do not wait until
                    // the end of the transaction!
                    bDoRemoveSQLLock = true;
                    SAPDB_RangeMove( __FILE__, 2,
                                     currRecKeyLen, sizeof( lockKey.keyVal_gg00()),
                                     pCurrRecKey, POS_OFF_DIFF_BD00,
                                     &lockKey.keyVal_gg00(), POS_OFF_DIFF_BD00, currRecKeyLen,
                                     trans.OldTrans().trError_gg00 );
                    if( e_wait_for_lock_release == trans.OldTrans().trError_gg00 ){
                        lockKey.keyLen_gg00() = currRecKeyLen;
                    }
                    break;
                }
            case e_lock_collision: // PTS 110874 HB 2000-11-01
                {
                    if( ! fileId.fileHandling_gg00().includes( hsIgnoreLock_egg00 )){
                        break;
                    }
                    if( mm_direct != sel.sfp_bd_mess2_type()){
                        trans.ResetErrorCode();
                        break;
                    }
                    trans.SetErrorCode( e_key_not_found );
                    if( ! sel.sfp_bd_return_knf()){
                        break;
                    }
                    if( recIndex == bd360_GetRecordCnt( pLeaf ) -1 ){
                        sel.sfp_bd_return_knf() = false;
                        break;
                    }
                    trans.ResetErrorCode();
                    recIndex +=1;
                    bd360_AssignRecKey( trans, fileId, pCurrRecKey, currRecKeyLen, pLeaf, recIndex );
                    if( trans.IsOk()){
                        trans.SetErrorCode( e_key_not_found );
                    }else{
                        sel.sfp_bd_return_knf() = false;
                    }
                    break;
                }
            default:;
            }
        }
        if( ! trans.IsOk()){
            break;
        }
        if( mm_direct == sel.sfp_bd_mess2_type()){
            break;
        }
        if( currResultCnt >= sel.sfp_m_result_cnt()){
            break;
        }
        bd360_NextRecord( trans, current, pLeaf, recIndex, unlockedPageNo, sel,
                          bClusteredRead, pageCollection, ioBuffer );
    }

    fileId.fileHandling_gg00().delElement(hsNoWaitForInserts_egg00);

    if(( bDoRemoveSQLLock )&&( ! trans.IsOk())&&( e_wait_for_lock_release != trans.OldTrans().trError_gg00 ))
    {   // remove sql lock in case of critical error
        bd360_RemoveCommittedReadShareLock( trans, fileId, lockKey, bDoRemoveSQLLock );
    }
    if( currResultCnt < sel.sfp_m_result_cnt()){
        sel.sfp_result_length() = lastResultLen;
    }
    if( 0 != pLeaf.np_ptr() ){
        b13r_release_node( pLeaf, current, lru_normal );
    }
}

/*---------------------------------------------------------------------------*/

externPascal void
bd360PrevQualifiedRecordFromTree(
    tbd_current_tree&           current,
    tsp00_KeyPtr                pCurrRecKey,
    SAPDB_Int2&                 currRecKeyLen,
    const tsp00_KeyPtr          pStopKey,
    const SAPDB_Int2            stopKeyLen,
    tgg00_Lkey&                 lockKey,
    const SAPDB_Int4            recBufferSize,
    tsp00_MoveObjPtr            pRecBuffer,
    SAPDB_Int4&                 recBufferPos,
    tgg00_SelectFieldsParam&    sel,
    tgg00_StackDesc&            stackDesc,
    SAPDB_Int&                  currResultCnt,
    pasbool&                    bIsNotQualified,
    pasbool&                    bDoRemoveSQLLock,
    tgg00_LockReqMode&          grantedLock,
    tsp00_PageNo&               unlockedPageNo )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd360PrevQual", DataTree_Trace, 5 );

    Trans_Context&      trans   = Trans_Context::GetContext( *current.curr_trans );
    tgg00_FileId&       fileId  = current.curr_tree_id;

    SAPDB_Int4      lastResultLen = 0;
    SAPDB_Int4      initResultLen = sel.sfp_result_length();
    SAPDB_Int4      recIndex;
    tbd_node_ptrs   pLeaf;

    pLeaf.np_ptr()  = 0;
    pLeaf.np_cbptr()= 0;
    bIsNotQualified = false;

    tgg00_Rec   auxRecordSpace; // tgg00_Rec used for packed records and UDF

    const bool bDoCopyRow = stackDesc.mstack_state().includes( ssCopyRow_egg00 );
    const bool bIsUpdDel  = ( m_update == sel.sfp_bd_mess_type() || m_delete == sel.sfp_bd_mess_type());

    const bool bIsTable      = ( tfnTable_egg00 == fileId.fileTfn_gg00());
    const bool bIsTempTable  = fileId.fileType_gg00().includes( ftsTemp_egg00 );
    const bool bIsTempResult = ( bIsTempTable && ( bd998GetExtendedTempFileType( trans.OldTrans(), fileId ) <= ttfnTempTable_egg00 ));
    const bool bDoSQLLocking = ( ! bIsTempTable )&&( bIsUpdDel || CnsReadNone == trans.OldTrans().trConsistReadInfo_gg00 );
    const bool bWithTransHistoryInfo = (( bIsUpdDel )&&( ! bIsTempTable ));
    const Data_PageRecoveryMode recMode = ( bIsTempTable ? Data_PageRecoveryMode( Data_Dynamic,
                                            Data_NotRecoverable ):Data_PageRecoveryMode());

    if(( bDoCopyRow )&&( current.currExtendedFileTypeSet_bd00.includes( eftsRecordsPacked_egg00 )))
    {
        trans.SetErrorCode( e_not_implemented );
        trans.ErrorList() = Msg_List( Msg_List::Warning, SDBMSG_DATA_UDF_NOT_PACKED_TABLES );
        return;
    }
    //  Seek to the first relevant record of the search area
    bd360_SearchStartKeyForPrevScan( trans, current, pCurrRecKey, currRecKeyLen, pLeaf, recIndex, sel );

    if(( trans.IsOk())&&( mm_last == sel.sfp_bd_mess2_type())){
        sel.sfp_bd_mess2_type().becomes( mm_prev );
    }
    while( trans.IsOk())
    {
        bIsNotQualified = false;
        if( ! bd360_AssignRecKey( trans, fileId, pCurrRecKey, currRecKeyLen, pLeaf, recIndex )){
            break;
        }
        if(
            ( sel.sfp_bd_use_stopkey())&&
            ( bd360_BeyondStopKey( trans, false, pCurrRecKey, currRecKeyLen, pStopKey, stopKeyLen ))){
            break;
        }
        const SAPDB_Int4 recPos = bd360_GetRecordPos( pLeaf, recIndex );
        tgg00_Rec*       pRec   = bd360_GetRecordPtr( pLeaf, recPos );

        if( Data_Row::ToRow( *pRec ).IsPacked())
        {   // record is stored in compressed layout
            Data_Row::ToRow( *pRec ).Unpack( Data_Row::ToRow( auxRecordSpace ) );
            pRec = &auxRecordSpace;
        }
        if( bDoCopyRow )
        {
            // caused by user defined functions it could happen that the current tree
            // will be accessed again and a deadlock could occur therefore it is
            // neccessary to copy the record and release the tree
            const tgg00_RecPtr  pCopySpace = &auxRecordSpace;
            bd360_MakeQualificationTestWithACopiedRow( trans, current, sel, stackDesc, pLeaf,
                    pCopySpace, pRec, recIndex, grantedLock, bIsNotQualified );
        }
        else
        {
            CnsRead_CachedRow* pRow = reinterpret_cast< CnsRead_CachedRow* >( pRec );
            bIsNotQualified = ( ! pRow->IsQualified( trans.OldTrans(), sel, stackDesc ));
        }
#ifdef SAPDB_SLOW 
        if( DataTree_Trace.TracesLevel( 5 ))
        {
            Kernel_VTrace vTrace;
            if( bIsNotQualified ){
                vTrace << "bd360PrevQual: record is not qualified ";
            }else{
                vTrace <<"bd360PrevQual: record is qualified ";
            }
            vTrace << "(" << Kernel_BasisErrorToString( trans.OldTrans().trError_gg00 ) << ")" << FlushLine;
        }
#endif        
        bd360_Monitoring( sel.sfp_bd_mess_type(), ! bIsNotQualified, bIsTable, bIsTempResult );

        if( bIsNotQualified ){   // record does not fullfil qualification
            trans.ResetErrorCode();
        }
        else // record fullfil qualification (bIsNotQualified == false)
        {
            if(( e_buffer_limit == trans.OldTrans().trError_gg00 )&& ( 0 < currResultCnt ))
            {
                sel.sfp_m_result_cnt() = currResultCnt;
                break;
            }
            if( ! trans.IsOk()){
                break;
            }
            if( bDoSQLLocking )
            {
                fileId.fileHandling_gg00().addElement(hsNoWaitForInserts_egg00);
                k53row_lock_by_key( trans.OldTrans(), fileId, pCurrRecKey, currRecKeyLen,
                                    sel.sfp_bd_mess_type(), sel.sfp_act_cntresult(), grantedLock );
            }
            if( trans.IsOk()){
                bd360_ResultHandling( trans, sel, currResultCnt, lastResultLen, recBufferPos,
                                      recBufferSize, pRecBuffer, pRec, initResultLen,
                                      bWithTransHistoryInfo );
            }
            if( bDoRemoveSQLLock ){
                bd360_RemoveCommittedReadShareLock( trans, fileId, lockKey, bDoRemoveSQLLock );
            }
            switch( trans.OldTrans().trError_gg00 )
            {
            case e_lock_collision_with_inserted_row:
                // skip current record because it was just inserted
                trans.ResetErrorCode();
                break;
            case e_wait_for_lock_release:
                {
                    if( ! fileId.fileHandling_gg00().includes( hsCollisionTest_egg00 )){
                        break;
                    }
                    // PTS 1117878 UH 2002-10-17
                    // In isolation level 1 (committed read) a row share lock is requested
                    // if an exclusive row lock exist. After the share lock is available
                    // and the record is read the lock must be removed! Do not wait until
                    // the end of the transaction!
                    bDoRemoveSQLLock = true;
                    SAPDB_RangeMove( __FILE__, 3,
                                     currRecKeyLen, sizeof( lockKey.keyVal_gg00()),
                                     pCurrRecKey, POS_OFF_DIFF_BD00,
                                     &lockKey.keyVal_gg00(), POS_OFF_DIFF_BD00, currRecKeyLen,
                                     trans.OldTrans().trError_gg00 );
                    if( e_wait_for_lock_release == trans.OldTrans().trError_gg00 ){
                        lockKey.keyLen_gg00() = currRecKeyLen;
                    }
                    break;
                }
            case e_lock_collision: // PTS 110874 HB 2000-11-01
                {
                    if( ! fileId.fileHandling_gg00().includes( hsIgnoreLock_egg00 )){
                        break;
                    }
                    trans.ResetErrorCode();
                }
            default:;
            }
            if( ! trans.IsOk()){
                break;
            }
            if( currResultCnt >= sel.sfp_m_result_cnt()){
                break;
            }
        }
        bd360_PrevRecord( trans, current, pLeaf, recIndex,
                          pCurrRecKey, currRecKeyLen, unlockedPageNo, sel );
    }

    fileId.fileHandling_gg00().delElement(hsNoWaitForInserts_egg00);

    if(( bDoRemoveSQLLock )&&( ! trans.IsOk())&&( e_wait_for_lock_release != trans.OldTrans().trError_gg00 ))
    {   // remove sql lock in case of critical error
        bd360_RemoveCommittedReadShareLock( trans, fileId, lockKey, bDoRemoveSQLLock );
    }
    if( currResultCnt < sel.sfp_m_result_cnt()){
        sel.sfp_result_length() = lastResultLen;
    }
    if( 0 != pLeaf.np_ptr() ){
        b13r_release_node( pLeaf, current, lru_normal );
    }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

bool
bd360_AssignRecKey(
    Trans_Context&          trans,
    tgg00_FileId&           fileId,
    tsp00_KeyPtr            pCurrRecKey,
    SAPDB_Int2&             currRecKeyLen,
    const tbd_node_ptrs&    pLeaf,
    const SAPDB_Int4         recIndex )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd360_AssignRecKey", DataTree_Trace, 7 );

    SAPDB_Int4 recPos = bd360_GetRecordPos( pLeaf, recIndex );

    if(( recPos < BODY_BEG_BD00 )||( recPos >= pLeaf.np_ptr()->nd_bottom()))
    {
        trans.SetErrorCode( e_system_error );
        bd360_ReportDamagedDataPage( trans, fileId, pLeaf, recIndex, recPos );
        return false;
    }
    const tgg00_RecPtr pRec      = bd360_GetRecordPtr( pLeaf, recPos );
    const SAPDB_Int4   recKeyLen = pRec->recKey_gg00().keyLen_gg00();
    SAPDB_RangeMove( __FILE__, 1,
                     recKeyLen, sizeof( tsp00_Key ),
                     &(pRec->recKey_gg00().keyVal_gg00()), POS_OFF_DIFF_BD00,
                     pCurrRecKey, POS_OFF_DIFF_BD00, recKeyLen,
                     trans.OldTrans().trError_gg00 );
    if( e_move_error != trans.OldTrans().trError_gg00 )
    {
        currRecKeyLen = recKeyLen;
        return true;
    }
    return( false );
}

/*---------------------------------------------------------------------------*/

void
bd360_CopyRowAndDoQualificationTest(
    Trans_Context&              trans,
    tbd_current_tree&           current,
    tgg00_SelectFieldsParam&    sel,
    tgg00_StackDesc&            stackDesc,
    tbd_node_ptrs&              pLeaf,
    tgg00_RecPtr                pCopySpace,
    tgg00_RecPtr&               pRec,
    SAPDB_Int4&                 recIndex,
    pasbool&                    bIsNotQualified,
    tbd_searchresult&           searchResult )
{
    SAPDB_Int4 destPos = 0;
    CnsRead_CachedRow* pOriginalRow = reinterpret_cast< CnsRead_CachedRow* >( pRec );
    pOriginalRow->Copy( false, pRec->recLen_gg00(), pCopySpace, destPos, trans.OldTrans().trError_gg00 );
    if( ! trans.IsOk()){
        return;
    }
    if( 0 != pLeaf.np_ptr()){
        b13r_release_node( pLeaf, current, lru_normal );
    }
    tgg00_FileId auxFileId = current.curr_tree_id; // avoid another call to the file directory

    bd30ReleaseTree (current);

    // do qualification handling without access to the current tree
    CnsRead_CachedRow* pCopiedRow = reinterpret_cast< CnsRead_CachedRow* >( pCopySpace );
    bIsNotQualified = ( ! pCopiedRow->IsQualified( trans.OldTrans(), sel, stackDesc ));
    const tgg00_BasisError auxError = trans.OldTrans().trError_gg00;

    bd30GetTree( trans.OldTrans(), auxFileId, current, sel.sfp_bd_mess_type(),
                 ! LOCK_TREE_EXCL_BD00, ! SYNC_TREE_LOCK_BD00 );
    if( ! trans.IsOk())
    {
        bIsNotQualified = false;
        return;
    }
    tbd_neighbors   neighbors; // not used
    tsp00_KeyPtr    pCurrKey = reinterpret_cast<tsp00_KeyPtr>(&(pCopySpace->recKey_gg00().keyVal_gg00()));
    bd30SearchRecord( current, pCurrKey, pCopySpace->recKeyLen_gg00(),
                      ! LOCK_LEAF_EXCL_BD00, pLeaf, recIndex, neighbors, searchResult );
    if( thisfound == searchResult )
    {
        const SAPDB_Int4 recPos = bd360_GetRecordPos( pLeaf, recIndex );
        pRec   = bd360_GetRecordPtr( pLeaf, recPos );
    }
    if( trans.IsOk()){
        trans.SetErrorCode( auxError );
    }else{
        bIsNotQualified = false;
    }
}

/*---------------------------------------------------------------------------*/

void
bd360_MakeQualificationTestWithACopiedRow(
    Trans_Context&              trans,
    tbd_current_tree&           current,
    tgg00_SelectFieldsParam&    sel,
    tgg00_StackDesc&            stackDesc,
    tbd_node_ptrs&              pLeaf,
    const tgg00_RecPtr          pCopySpace,
    tgg00_RecPtr&               pRec,
    SAPDB_Int4&                 recIndex,
    tgg00_LockReqMode&          grantedLock,
    pasbool&                    bIsNotQualified )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd360_MakeQual", DataTree_Trace, 7 );

    tbd_searchresult    searchResult;

    bd360_CopyRowAndDoQualificationTest( trans, current, sel, stackDesc, pLeaf, pCopySpace,
                                         pRec, recIndex, bIsNotQualified, searchResult );
    if( ! trans.IsOk()){
        return;
    }
    if( thisfound != searchResult ) // record does not exist anymore
    {
        if( nextfound == searchResult )
        {
            if((mm_last != sel.sfp_bd_mess2_type())&&( mm_prev != sel.sfp_bd_mess2_type()))
            {
                // In case of ascending order we have the ensure that the current
                // record which is 'one behind' the record we do the qualification test
                // for will not be skipped with the following call of bd360_NextRecord.
                // Therefore recIndex is decremented by one so that the next main loop
                // will test the qualification on this record
                // In case of descending order no re-position is needed, because the
                // current record was qualified before and the call of bd360_PrevRecord
                // will use the correct one.
                recIndex -=1;
            }
        }
        bIsNotQualified = true;
        return;
    }
    // the record which was copied before the tree was released is still
    // present after the qualification test and the re-entrance of the
    // tree at the same logical position

    if( ! bIsNotQualified ){
        return; // does not fullfil qualification
    }
    //if CnsReadNone != trans.trConsistReadInfo_gg00  // only 7.7
    // return;

    tgg00_FileId&   fileId = current.curr_tree_id;
    if(
        ( ! fileId.fileHandling_gg00().includes( hsWithoutLock_egg00 ))  ||
        ( fileId.fileHandling_gg00().includes( hsOptimisticLock_egg00 ))
    )
    {
        // check if the found record is still the same82
        tsp00_LcompResult   compResult;
        s30cmp( pCopySpace, POS_OFF_DIFF_BD00, pCopySpace->recLen_gg00(),
                pRec, POS_OFF_DIFF_BD00, pRec->recLen_gg00(), compResult );
        if( l_equal != compResult ){
            return;
        }
        {
            tsp00_KeyPtr    pCurrKey = reinterpret_cast<tsp00_KeyPtr>(&(pRec->recKey_gg00().keyVal_gg00()));
            k53row_lock_by_key( trans.OldTrans(), fileId, pCurrKey, pRec->recKeyLen_gg00(),
                                sel.sfp_bd_mess_type(), sel.sfp_act_cntresult(), grantedLock );
            if ( trans.OldTrans().trError_gg00 == e_lock_collision_with_inserted_row )
            {
                bIsNotQualified = true;
                trans.ResetErrorCode();
                return;
            }
        }
        if( ! trans.IsOk()){
            return;
        }
        bd360_CopyRowAndDoQualificationTest( trans, current, sel, stackDesc, pLeaf, pCopySpace,
                                             pRec, recIndex, bIsNotQualified, searchResult );
        {
            tsp00_KeyPtr    pCurrKey = reinterpret_cast<tsp00_KeyPtr>(&(pRec->recKey_gg00().keyVal_gg00()));
            k53key_unlock2( trans.OldTrans(), grantedLock, fileId.fileTabId_gg00(),
                            pCurrKey, pRec->recKeyLen_gg00());
        }
    }
}

/*---------------------------------------------------------------------------*/

void
bd360_Monitoring(
    const tgg00_MessType& messType,
    const bool            bIsQualified,
    const bool            bIsTable,
    const bool            bIsTempResult )
{
    switch( messType )
    {
    case m_update:
        {
            if( ! bIsTable ){
                return;
            }
            b73cmd_count( iupdates_rows_read );
            if( bIsQualified ){
                b73cmd_count( iupdates_rows_upd );
            }
            return;
        }
    case m_delete:
        {
            if( ! bIsTable ){
                return;
            }
            b73cmd_count( ideletes_rows_read );
            if( bIsQualified ){
                b73cmd_count( ideletes_rows_del );
            }
            return;
        }
    default:
        {
            if( bIsTable || bIsTempResult )
            {
                b73cmd_count( iselects_rows_read );
                if( bIsQualified ){
                    b73cmd_count( iselects_rows_qual );
                }
            }
        }
    }
}

/*---------------------------------------------------------------------------*/

void bd360_NextRecord(
    Trans_Context&              trans,
    tbd_current_tree&           current,
    tbd_node_ptrs&              pLeaf,
    SAPDB_Int4&                 recIndex,
    tsp00_PageNo&               unlockedPageNo,
    tgg00_SelectFieldsParam&    sel,
    const bool                  bClusteredRead,
    Converter_PageCollector&    pageCollection,
    Data_IOBuffer&              ioBuffer )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd360_NextRecord", DataTree_Trace, 7 );

    // this may happen if the qualification test has has been made on a copy
    if( 0 == bd360_GetRecordCnt( pLeaf )){
        trans.SetErrorCode( e_no_next_record );
        return;
    }
    if( recIndex < bd360_GetRecordCnt( pLeaf ) -1 ){
        recIndex +=1;
        return;
    }
    const Data_PageNo nextLeafPage = pLeaf.np_ptr()->nd_right();
    // go to right neighbor page
    if( trans.Task().CancelPending())
    {
        trans.SetErrorCode( e_cancelled );
        const FileDir_FileNo fileNo( current.curr_tree_id.fileTabId_gg00());
        RTE_Message( Msg_List( Msg_List::Info, SDBMSG_DATA_TABLE_SCAN_CANCELED,
                               Msg_Arg( SDBMSGTAG_DATA_TABLE_SCAN_CANCELED__FILENO,
                                        FileDir_FileNoToString( fileNo ))));
        return;
    }
    if( bd20IsPageRequested( current.currRootNptrs_bd00.np_cbptr())){
        trans.SetErrorCode( e_bd_leaf_unlocked );
    }
    else
    {
        Data_PageNo oldIndexPageNo;
        if( bClusteredRead ){
            oldIndexPageNo = bd360_GetIndexPageNo( current );
        }
        b31next_search( pLeaf, recIndex, current, lru_mid ); // always a new leaf is reached
        if( trans.IsOk())
        {
            trans.OldTrans().trRteCommPtr_gg00->file_record_cnt +=1;
            if( bClusteredRead )
            {
                const Data_PageNo newIndexPageNo = bd360_GetIndexPageNo( current );
                if(( oldIndexPageNo != newIndexPageNo )){
                    bd360_ClusteredRead( trans, current, sel, pLeaf.np_ptr()->nd_id(), pageCollection, ioBuffer );
                }
            }
        }
    }
    if( e_bd_leaf_unlocked != trans.OldTrans().trError_gg00 ){
        return;
    }
    unlockedPageNo = nextLeafPage;
    sel.sfp_bd_mess2_type().becomes( mm_next );
}

/*---------------------------------------------------------------------------*/

void bd360_PrevRecord(
    Trans_Context&              trans,
    tbd_current_tree&           current,
    tbd_node_ptrs&              pLeaf,
    SAPDB_Int4&                 recIndex,
    tsp00_KeyPtr                pCurrRecKey,
    SAPDB_Int2                  currRecKeyLen,
    tsp00_PageNo&               unlockedPageNo,
    tgg00_SelectFieldsParam&    sel )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd360_PrevRecord", DataTree_Trace, 7 );

    // this may happen if the qualification test has has been made on a copy
    if( 0 == bd360_GetRecordCnt( pLeaf )){
        trans.SetErrorCode( e_no_prev_record );
        return;
    }
    if( FIRST_REC_INDEX_BD00 < recIndex ){
        recIndex -=1;
        return;
    }
    // go to left neighbor page
    if( trans.Task().CancelPending())
    {
        trans.SetErrorCode( e_cancelled );
        const FileDir_FileNo fileNo( current.curr_tree_id.fileTabId_gg00());
        RTE_Message( Msg_List( Msg_List::Info, SDBMSG_DATA_TABLE_SCAN_CANCELED,
                               Msg_Arg( SDBMSGTAG_DATA_TABLE_SCAN_CANCELED__FILENO,
                                        FileDir_FileNoToString( fileNo ))));
        return;
    }
    tsp00_PageNo    prevLeafPage = NIL_PAGE_NO_GG00;
    if( bd20IsPageRequested( current.currRootNptrs_bd00.np_cbptr())){
        trans.SetErrorCode( e_bd_leaf_unlocked );
    }
    else{
        bd31PrevSearch( pCurrRecKey, currRecKeyLen, pLeaf, recIndex, prevLeafPage, current, false );
    }
    if( e_bd_leaf_unlocked != trans.OldTrans().trError_gg00 ){
        return;
    }
    unlockedPageNo = prevLeafPage;
    sel.sfp_bd_mess2_type().becomes( mm_prev );
}

/*---------------------------------------------------------------------------*/

void
bd360_RemoveCommittedReadShareLock(
    Trans_Context&  trans,
    tgg00_FileId&   fileId,
    tgg00_Lkey&     lockKey,
    pasbool&        bDoRemoveLock )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd360_RemoveLock", DataTree_Trace, 5 );

    if(( lockKey.keyLen_gg00() <= 0 ) || ( ! bDoRemoveLock )){
        return;
    }
    tsp00_KeyPtr pLockKey = reinterpret_cast< tsp00_KeyPtr >( &(lockKey.keyVal_gg00()));
    const tgg00_BasisError  auxError = trans.OldTrans().trError_gg00;
    trans.ResetErrorCode();

    k53key_unlock2( trans.OldTrans(), lckRowShare_egg00,
                    fileId.fileTabId_gg00(), pLockKey, lockKey.keyLen_gg00());

    trans.SetErrorCode( auxError );
    lockKey.keyLen_gg00() = 0;
    bDoRemoveLock         = false;
}

/*---------------------------------------------------------------------------*/

void
bd360_ResultHandling(
    Trans_Context&              trans,
    tgg00_SelectFieldsParam&    sel,
    SAPDB_Int4&                 currResultCnt,
    SAPDB_Int4&                 lastResultLen,
    SAPDB_Int4&                 recBufferPos,
    const SAPDB_Int4            recBufferSize,
    tsp00_MoveObjPtr            pRecBuffer,
    const tgg00_RecPtr          pRec,
    const SAPDB_Int4            initResultLen,
    const bool                  bWithTransHistoryInfo )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd360_ResultHandling", DataTree_Trace, 5 );

    if( 0 != pRecBuffer )
    {
        if( recBufferPos + pRec->recLen_gg00() -1 > recBufferSize )
        {
            trans.SetErrorCode( e_buffer_limit );
            sel.sfp_m_result_cnt() = currResultCnt;
            return;
        }
        {
            CnsRead_CachedRow* pRow = reinterpret_cast< CnsRead_CachedRow* >( pRec );
            --recBufferPos;
            pRow->Copy(( 0 != bWithTransHistoryInfo ), recBufferSize, pRecBuffer,
                       recBufferPos, trans.OldTrans().trError_gg00 );
            ++recBufferPos;
        }
        recBufferPos += pRec->recLen_gg00();
        if(( m_fetch != sel.sfp_bd_mess_type())&&( 0 == ( recBufferPos % 2 ))){
            recBufferPos +=1;
        }
    }
    if( ! trans.IsOk()){
        return;
    }
    currResultCnt +=1;
    if(( 1 == currResultCnt )&&( 0 != sel.sfp_m_firstkeys_addr()))
    { // save key for scrollable cursor
        bd360_LkeyAssign( trans, pRec->recKey_gg00().keyLen_gg00(),
                          &(pRec->recKey_gg00().keyVal_gg00()), sel.sfp_m_firstkeys_addr()->reckey );
    }
    SAPDB_Int4 currResultLen;
    if( sel.sfp_resrec_maxlen() < MAX_INT2_SP00 ){
        currResultLen = sel.sfp_resrec_maxlen();
    }else{
        currResultLen = sel.sfp_result_length();
    }
    if( m_fetch != sel.sfp_bd_mess_type()&&( 1 == ( currResultLen %2 ))){
        currResultLen +=1;
    }
    if( currResultLen > sel.sfp_result_length())
    {
        SAPDB_RangeFill( __FILE__, 1,
                         sel.sfp_m_result_size(), *sel.sfp_m_result_addr(),
                         sel.sfp_result_length() + 1,
                         currResultLen - sel.sfp_result_length(),
                         '\0', trans.OldTrans().trError_gg00 );
        if( ! trans.IsOk()){
            return;
        }
    }
    sel.sfp_m_result_len()  += currResultLen;
    sel.sfp_m_result_addr() += currResultLen;
    sel.sfp_m_result_size()-= currResultLen;
    sel.sfp_act_cntresult() +=1;

#ifdef SAPDB_SLOW
    if( DataTree_Trace.TracesLevel( 6 ))
    {
        Kernel_VTrace vTrace;
        vTrace << " m_result_size: " << sel.sfp_m_result_size()
        << " act_cntresul : " << sel.sfp_act_cntresult() << FlushLine;
        vTrace << " m_result_len : " << sel.sfp_m_result_len()
        << " currResultCnt: " << currResultCnt << FlushLine;
    }
#endif    

    if( currResultCnt < sel.sfp_m_result_cnt())
    {
        lastResultLen           = sel.sfp_result_length();
        sel.sfp_result_length() = initResultLen;
    }
}

/*---------------------------------------------------------------------------*/

void
bd360_SearchStartKeyForNextScan(
    Trans_Context&              trans,
    tbd_current_tree&           current,
    tsp00_KeyPtr                pCurrRecKey,
    SAPDB_Int2&                 currRecKeyLen,
    tbd_node_ptrs&              pLeaf,
    SAPDB_Int4&                 recIndex,
    tgg00_SelectFieldsParam&    sel )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd360_SearchForNextScan", DataTree_Trace, 8 );

    tbd_searchresult    searchResult;
    tbd_neighbors       neighbors;

    bd30SearchRecord( current, pCurrRecKey, currRecKeyLen, ! LOCK_LEAF_EXCL_BD00,
                      pLeaf, recIndex, neighbors, searchResult );
    if( ! trans.IsOk())
    {
        sel.sfp_bd_return_knf() = false;
        return;
    }
    if( mm_direct != sel.sfp_bd_mess2_type())
    {
        if(
            (  nonefound == searchResult ) ||
            (( lastfound == searchResult )&&( NIL_PAGE_NO_GG00 == pLeaf.np_ptr()->nd_right())))
        {
            trans.SetErrorCode( e_no_next_record );
            return;
        }
    }
    else // == mm_direct
    {
        if( thisfound != searchResult )
        {
            if( ! sel.sfp_bd_return_knf()){
                trans.SetErrorCode( e_key_not_found );
                return;
            }
            if( nextfound == searchResult )
            {
                if( bd360_AssignRecKey( trans, current.curr_tree_id, pCurrRecKey, currRecKeyLen, pLeaf, recIndex ))
                {
                    trans.SetErrorCode( e_key_not_found );
                }
                return;
            }
            if(( lastfound == searchResult )&&( NIL_PAGE_NO_GG00 == pLeaf.np_ptr()->nd_right())){
                trans.SetErrorCode( e_no_next_record );
            }
            else{
                trans.SetErrorCode( e_key_not_found );
                sel.sfp_bd_return_knf() = false;
            }
            return;
        }
    }
    if(
        (( mm_next == sel.sfp_bd_mess2_type())&&( nextfound != searchResult )) ||
        ((mm_first == sel.sfp_bd_mess2_type())&&( lastfound == searchResult))
    )
    {
        b31next_search( pLeaf, recIndex, current, lru_mid );  // might be b_no_next_record
        if( ! trans.IsOk()){
            return;
        }
        if( FIRST_REC_INDEX_BD00 == recIndex ){
            trans.OldTrans().trRteCommPtr_gg00->file_record_cnt +=1;
        }
    }
}

/*---------------------------------------------------------------------------*/

void
bd360_SearchStartKeyForPrevScan(
    Trans_Context&              trans,
    tbd_current_tree&           current,
    tsp00_KeyPtr                pCurrRecKey,
    SAPDB_Int2&                 currRecKeyLen,
    tbd_node_ptrs&              pLeaf,
    SAPDB_Int4&                 recIndex,
    tgg00_SelectFieldsParam&    sel )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd360_SearchForPrevScan", DataTree_Trace, 8 );

    tbd_searchresult    searchResult;
    tbd_neighbors       neighbors;

    bd30SearchRecord( current, pCurrRecKey, currRecKeyLen, ! LOCK_LEAF_EXCL_BD00,
                      pLeaf, recIndex, neighbors, searchResult );
    if( ! trans.IsOk()){
        return;
    }
    if( nonefound == searchResult ){
        trans.SetErrorCode( e_no_prev_record );
        return;
    }
    if(
        ( nextfound == searchResult ) ||
        (( thisfound == searchResult )&&( mm_prev == sel.sfp_bd_mess2_type())))
    {
        tsp00_PageNo    prevLeafPage = NIL_PAGE_NO_GG00; // not used
        bd31PrevSearch( pCurrRecKey, currRecKeyLen, pLeaf, recIndex, prevLeafPage, current, false);
    }
}

/*---------------------------------------------------------------------------*/

void
bd360_ClusteredRead(
    Trans_Context&              trans,
    tbd_current_tree&           current,
    tgg00_SelectFieldsParam&    sel,
    const Data_PageNo           leafPageNo,
    Converter_PageCollector&    pageCollection,
    Data_IOBuffer&              ioBuffer )
{
    if( LEAF_LEVEL_BD00 == current.currRootNptrs_bd00.np_ptr()->nd_level()){
        return;
    }
    const tbd_node_ptrs& pIndex      = bd360_GetIndexNptrs( current );
    const Data_PageNo    indexPageNo = pIndex.np_ptr()->nd_id();
    if( indexPageNo == sel.sfp_cluster_info().clust_LastUsedIndexPage_gg00 ){
        return;
    }
    // do not read the corresponding leaves of this index node again
    sel.sfp_cluster_info().clust_LastUsedIndexPage_gg00 = indexPageNo;
    if( ! pageCollection.Initialize()){ // reinitialization to be able to store new pageNo's
        return;
    }
    const SAPDB_Int4 recCount = bd360_GetRecordCnt( pIndex );
    bool bIgnore = true;
    for( SAPDB_Int4 recIndex = 0; recIndex < recCount; recIndex++ )
    {
        const SAPDB_Int4    recPos = bd360_GetRecordPos( pIndex, recIndex );
        const tgg00_Rec*    pRec   = bd360_GetRecordPtr( pIndex, recPos );
        const Data_PageNo   childPagePageNo = pRec->recPno_gg00().gg90GetInt4();

        if( childPagePageNo == leafPageNo ){
            bIgnore = false; //put into pageCollection
            continue;
        }
        if( ! bIgnore ){
            if( ! pageCollection.AppendPageNo( childPagePageNo )){
                return; // ignore memory exhausted error
            }
        }
    }
    pageCollection.DetermineCluster( trans.Task());
    Data_PageNoList         pageNoList( trans.Allocator());
    IOMan_ClusterAddress    cluster;

    if( ! pageNoList.Reserve( pageCollection.GetMaxClusterSize())){
        return; // ignore memory exhausted error
    }
    for( SAPDB_UInt clusterID = Converter_PageCollector::FirstClusterId();
            clusterID <= pageCollection.GetClusterCount(); clusterID++ )
    {
        pageCollection.GetCluster( clusterID, pageNoList, cluster );
        bd13LoadNodes( trans, current.curr_tree_id, pageCollection.GetRecoveryMode(),
                       pageNoList, cluster, ioBuffer.GetBuffer());
        if( ! trans.IsOk()){
            return;
        }
    }
}

/*---------------------------------------------------------------------------*/

void
bd360_ReportDamagedDataPage(
    Trans_Context&          trans,
    tgg00_FileId&           fileId,
    const tbd_node_ptrs&    pLeaf,
    const tsp00_Int4        recIndex,
    const tsp00_Int4        recPos )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd360_ReportDamagedDataPage", DataTree_Trace, 5 );

    {
        Data_BasePage   dumpPage;
        dumpPage.AssignNodePtr( pLeaf.np_ptr());
        Kernel_FileIO	dumpFile( KERNEL_DATA_FILE,	KERNEL_COR_FILE_EXTENSION,
                                SAPDB_ToString( dumpPage.PageNo(), _T_d ));
        dumpFile.Write(	dumpPage );
    }
    const Data_PageRecoveryMode recMode  = ( fileId.fileType_gg00().includes( ftsTemp_egg00 ) ?
                                           Data_PageRecoveryMode( Data_Dynamic, Data_NotRecoverable ):
                                           Data_PageRecoveryMode());

    const tsp00_PageNo   pageNo = pLeaf.np_ptr()->nd_id();
    const tsp00_PageNo   root   = fileId.fileRoot_gg00();
    const FileDir_FileNo fileNo( fileId.fileTabId_gg00());

    trans.ErrorList() = Msg_List(
                            Msg_List::Error, SDBMSG_DATA_DAMAGED_DATA_RECORD_FOUND_1,
                            Msg_Arg( SDBMSGTAG_DATA_DAMAGED_DATA_RECORD_FOUND_1__PAGENO, pageNo ),
                            Msg_Arg( SDBMSGTAG_DATA_DAMAGED_DATA_RECORD_FOUND_1__FILENO, FileDir_FileNoToString( fileNo )),
                            Msg_Arg( SDBMSGTAG_DATA_DAMAGED_DATA_RECORD_FOUND_1__ROOT, root ),
                            Msg_Arg( SDBMSGTAG_DATA_DAMAGED_DATA_RECORD_FOUND_1__FILETYPE, "table" ),
                            Msg_Arg( SDBMSGTAG_DATA_DAMAGED_DATA_RECORD_FOUND_1__PERSISTENT_TYPE, SAPDB_ToString( recMode )),
                            Msg_OptArg( SDBMSGTAG_DATA_DAMAGED_DATA_RECORD_FOUND_1__RECORD_INDEX, recIndex ),
                            Msg_OptArg( SDBMSGTAG_DATA_DAMAGED_DATA_RECORD_FOUND_1__RECORD_POSITION, recPos ),
                            Msg_OptArg( SDBMSGTAG_DATA_DAMAGED_DATA_RECORD_FOUND_1__INTERNAL_FILENAME,
                                        SAPDB_ToString( fileId.fileName_gg00(), 0, sizeof( fileId.fileName_gg00()), _T_H )),
                            Msg_OptArg( SDBMSGTAG_DATA_DAMAGED_DATA_RECORD_FOUND_1__KNL_BASE_ERROR,
                                        Kernel_BasisErrorToString( e_illegal_entrypos ))
                        );
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/

