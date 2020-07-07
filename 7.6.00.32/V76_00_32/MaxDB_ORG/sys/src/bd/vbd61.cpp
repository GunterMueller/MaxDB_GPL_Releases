/*
  @file           vbd61.cpp
  @author         TorstenS
  @ingroup        DataAccess
  @brief          LOB index handling

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2006 SAP AG

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

#include "hbd61.h" 
#include "hbd13.h"
#include "hbd13_3.h" 
#include "hsp20.h"

#include "Converter/Converter_PageCollector.hpp"
#include "DataAccess/Data_Types.hpp"
#include "DataAccess/Data_BasePage.hpp"
#include "DataAccess/Data_PageNoList.hpp"
#include "DataAccess/Data_PageFrame.hpp"
#include "IOManager/IOMan_ClusterAddress.hpp"
#include "KernelCommon/Kernel_FileIO.hpp"
#include "KernelCommon/Kernel_Common.hpp"
#include "SAPDBCommon/SAPDB_RangeCode.hpp" // Kernel_move_and_fill
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

inline void
bd61_FillPositionList(
    const tsp00_Int4    startLevel,
    const tsp00_Int4    position,
    tsp00_Int4&         leafPosition,
    tbd_position_list&  positionList )
{
    leafPosition        = position % MAX_BSTR_COVERING_BD00;
    tsp00_Int4 result   = position / MAX_BSTR_COVERING_BD00;

    for( tsp00_Int4 level = startLevel; level <= MAX_BSTR_LEVEL_BD00; ++level )
    {
        if( 0 == result ){
            positionList[ level - POS_OFF_DIFF_BD00 ] = 0;
        }
        else
        {
            positionList[ level - POS_OFF_DIFF_BD00 ] = result % PNOS_PER_PAGE_BD00;
            result                                    = result / PNOS_PER_PAGE_BD00;
        }
    }
}

/*---------------------------------------------------------------------------*/

void
bd61_ClusteredRead(
    Trans_Context&              trans,
    tgg00_FileId&               fileId,
    const tbd_node_ptrs&        pIndex,
    Converter_PageCollector&    pageCollection,
    Data_PageFrame&             ioBuffer,
    const tsp00_Int4            maxPagesToRead,
    tsp00_Int4                  pageNoPos );

/*---------------------------------------------------------------------------*/

void
bd61_Free(
    Trans_Context&      trans,
    tbd_current_tree&   current,
    const tsp00_PageNo  pageNo,
    const tsp00_Int4    currLevel );

/*===========================================================================*
 *  METHODS -lll-                                                            *
 *===========================================================================*/

externC void
bd61AddIndex(
    Trans_Context&      trans,
    tbd_current_tree&   current,
    tbd_node_ptrs&      pCurrRoot,
    const tsp00_PageNo  currPageNo,
    const tsp00_PageNo  newPageNo1,
    const tsp00_PageNo  newPageNo2,
    const tsp00_Int2    insertLevel )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd61AddIndex", DataTree_Trace, 5 );

    tbd_node_ptrs   pCurrNode;
    pCurrNode.np_ptr() = 0; // okay

    if( currPageNo != pCurrRoot.np_ptr()->nd_id())
    {
        bd13GetNode( current, currPageNo, plmLock_ebd00, nr_for_update, pCurrNode );
        if( ! trans.IsOk()){
            if( 0 != pCurrNode.np_ptr()){
                b13r_release_node( pCurrNode, current, lru_normal );
            }
            return;
        }
    }
    else{
        pCurrNode = pCurrRoot;
    }
    if( pCurrNode.np_ptr()->nd_level() != insertLevel )
    {
        const tsp00_Int4   indexPos   = pCurrNode.np_ptr()->nd_bottom() - sizeof( tsp00_PageNo );
        const tsp00_PageNo nextPageNo = s20buf_to_int4( pCurrNode.np_ptr(), indexPos );
        if( pCurrNode.np_ptr() != pCurrRoot.np_ptr()){
            b13r_release_node( pCurrNode, current, lru_normal );
        }
        bd61AddIndex( trans, current, pCurrRoot, nextPageNo, newPageNo1, newPageNo2, insertLevel );
        return;
    }
    tsp00_Int4      indexInsertPos = pCurrNode.np_ptr()->nd_bottom();
    tbd_node_ptrs   pIndexNode;

    pIndexNode.np_ptr()   = 0;
    pIndexNode.np_cbptr() = 0;

    if( indexInsertPos <= ( MAX_BOTTOM_BD00 - sizeof( tsp00_PageNo )))
    {
        // at least one leaf pageNo fits into the current index page
        s20int4_to_buf( newPageNo1, pCurrNode.np_ptr(), indexInsertPos );
        indexInsertPos += sizeof( tsp00_PageNo );
        pCurrNode.np_ptr()->nd_bottom() = indexInsertPos;  /* PTS 1135689 TS 2005-05-23 */
        if( newPageNo2 != NIL_PAGE_NO_GG00 )
        {
            if( indexInsertPos <= MAX_BOTTOM_BD00 - sizeof( tsp00_PageNo ))
            {
                // secons leaf pageNo fits into current index page too
                s20int4_to_buf( newPageNo2, pCurrNode.np_ptr(), indexInsertPos );
                indexInsertPos += sizeof ( tsp00_PageNo );
                pCurrNode.np_ptr()->nd_bottom() = indexInsertPos;  /* PTS 1135689 TS 2005-05-23 */
            }
            else  // second leaf pageNo needs new index page
            {
                b13new_node( insertLevel, pIndexNode, current );
                if( trans.IsOk()){
                    s20int4_to_buf( newPageNo2, pIndexNode.np_ptr(), BODY_BEG_BD00 );
                    pIndexNode.np_ptr()->nd_bottom() = BODY_BEG_BD00 + sizeof( tsp00_PageNo );
                }
            }
        }
    }
    else // current index page is completely filled
    {
        b13new_node( insertLevel, pIndexNode, current );
        if( trans.IsOk()){
            s20int4_to_buf( newPageNo1, pIndexNode.np_ptr(), BODY_BEG_BD00 );
            indexInsertPos = BODY_BEG_BD00 + sizeof( tsp00_PageNo );
            if( newPageNo2 != NIL_PAGE_NO_GG00 ){
                s20int4_to_buf( newPageNo2, pIndexNode.np_ptr(), indexInsertPos );
                indexInsertPos += sizeof( tsp00_PageNo );
            }
            pIndexNode.np_ptr()->nd_bottom() = indexInsertPos;
        }
    }
    if( 0 == pIndexNode.np_ptr()) // no new index page needed
    {
        if( pCurrNode.np_ptr()->nd_id() != pCurrRoot.np_ptr()->nd_id()){
            b13w_release_node( pCurrNode, current );
        }
        b13w_release_node( pCurrRoot, current );
        return;
    }
    // integrate new index node into tree

    const tsp00_PageNo indexPageNo = pIndexNode.np_ptr()->nd_id();
    pCurrNode.np_ptr()->nd_right() = indexPageNo;
    b13w_release_node( pIndexNode, current );
    if( trans.IsOk())
    {
        if( pCurrNode.np_ptr()->nd_id() == pCurrRoot.np_ptr()->nd_id())
        {  // split of root page is needed
            tbd_node_ptrs   pNewRoots;
            pNewRoots.np_ptr() = 0;

            b13new_node( insertLevel, pNewRoots, current );
            if( trans.IsOk())
            {
                bd61ChangeContent( trans, pCurrNode, pNewRoots );
                if( trans.IsOk()) // Add both index pageNos to the new root
                {
                    s20int4_to_buf( pCurrNode.np_ptr()->nd_id(), pNewRoots.np_ptr(), BODY_BEG_BD00 );
                    indexInsertPos = BODY_BEG_BD00 + sizeof( tsp00_PageNo );
                    s20int4_to_buf( indexPageNo, pNewRoots.np_ptr(), indexInsertPos );
                    pNewRoots.np_ptr()->nd_bottom() = indexInsertPos + sizeof( tsp00_PageNo );
                    b13w_release_node( pCurrNode, current );
                    b13w_release_node( pNewRoots, current );
                }
            }
            if(( ! trans.IsOk()) && ( 0 != pNewRoots.np_ptr())){
                b13r_release_node( pNewRoots, current, lru_normal );
            }
        }
        else
        {
            b13w_release_node( pCurrNode, current );
            bd61AddIndex( trans, current, pCurrRoot, pCurrRoot.np_ptr()->nd_id(), indexPageNo,
                          NIL_PAGE_NO_GG00, insertLevel + 1 );
        }
    }
    if( trans.IsOk()){
        return;
    }
    if( 0 != pCurrNode.np_ptr()){
        b13r_release_node( pCurrNode, current, lru_normal );
    }
}

/*---------------------------------------------------------------------------*/

externC void
bd61ChangeContent(
    Trans_Context&  trans,
    tbd_node_ptrs&  pNode,
    tbd_node_ptrs&  pRoot )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd61ChangeContent", DataTree_Trace, 5 );

    tbd_node_ptrs pAuxNodes;

    pAuxNodes = pNode;
    pNode    = pRoot;
    pRoot    = pAuxNodes;

    SAPDB_RangeMove( __FILE__, 1,
                     sizeof( tsp00_Page ), sizeof( tsp00_Page ),
                     pRoot.np_ptr(), BODY_BEG_BD00, pNode.np_ptr(), BODY_BEG_BD00,
                     MAX_BSTR_COVERING_BD00, trans.OldTrans().trError_gg00 );

    if( ! trans.IsOk())
    {
        trans.SetErrorCode( e_data_page_corrupted );
        {
            Data_BasePage   dumpPage;
            dumpPage.AssignNodePtr( pRoot.np_ptr());
            Kernel_FileIO	dumpFile( KERNEL_DATA_FILE,	KERNEL_COR_FILE_EXTENSION,
                                    SAPDB_ToString( dumpPage.PageNo(), _T_d ));
            dumpFile.Write(	dumpPage );
        }
        pAuxNodes = pNode;
        pNode    = pRoot;
        pRoot    = pAuxNodes;
        return;
    }
    pNode.np_ptr()->nd_level()  = pRoot.np_ptr()->nd_level();
    pNode.np_ptr()->nd_bottom() = MAX_BOTTOM_BD00;  // set filling to maximum
    pNode.np_ptr()->nd_right()  = pRoot.np_ptr()->nd_right();

    pRoot.np_ptr()->nd_level() = pRoot.np_ptr()->nd_level()+ 1;
    pRoot.np_ptr()->nd_right() = NIL_PAGE_NO_GG00;
}

/*---------------------------------------------------------------------------*/


externC void
bd61FirstLoadStep(
    Trans_Context&      trans,
    tbd_current_tree&   current,
    tbd_node_ptrs&      pLeftNode,
    tbd_node_ptrs&      pRightNode )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd61FirstLoadStep", DataTree_Trace, 5 );

    if( 0 == pRightNode.np_ptr()) // pLeftNodes = root
    {
        pLeftNode.np_ptr()->ndStrFileSize_bd00() = pLeftNode.np_ptr()->nd_bottom() - BODY_BEG_BD00;
        b13w_release_node( pLeftNode, current );
        return;
    }
    // build LOB index
    tbd_node_ptrs pRootNode;
    pRootNode.np_ptr() = 0;
    b13new_node( LEAF_LEVEL_BD00, pRootNode, current );
    if( ! trans.IsOk()){
        if( 0 != pRootNode.np_ptr()){
            b13r_release_node( pRootNode, current, lru_normal );
        }
        return;
    }
    bd61ChangeContent( trans, pLeftNode, pRootNode );
    if( ! trans.IsOk()){
        b13free_node( pRootNode, current );
        return;
    }
    tsp00_Int4 indexInsertPos = BODY_BEG_BD00;
    s20int4_to_buf( pLeftNode.np_ptr()->nd_id(), pRootNode.np_ptr(), indexInsertPos );
    indexInsertPos  += sizeof( tsp00_PageNo );
    s20int4_to_buf( pRightNode.np_ptr()->nd_id(), pRootNode.np_ptr(), indexInsertPos );
    pRootNode.np_ptr()->nd_bottom() = indexInsertPos + sizeof( tsp00_PageNo );

    const tgg00_PagePos saveBottom  = pRightNode.np_ptr()->nd_bottom();

    b13init_default_data_page( current.curr_tree_id, LEAF_LEVEL_BD00,
                               pRightNode.np_ptr()->nd_id(), pRightNode );

    pRightNode.np_ptr()->nd_bottom() = saveBottom;
    pRightNode.np_ptr()->nd_left()   = pLeftNode.np_ptr()->nd_id();
    pRightNode.np_ptr()->ndStrFileSize_bd00() = MAX_BSTR_COVERING_BD00 +
            pRightNode.np_ptr()->nd_bottom() - BODY_BEG_BD00;
    pRootNode.np_ptr()->ndStrFileSize_bd00() = pRightNode.np_ptr()->ndStrFileSize_bd00();

    pLeftNode.np_ptr()->nd_right() = pRightNode.np_ptr()->nd_id();
    b13w_release_node( pLeftNode, current );
    b13w_release_node( pRootNode, current );
    pLeftNode             = pRightNode;
    pRightNode.np_ptr()   = 0;
    pRightNode.np_cbptr() = 0;
}


/*---------------------------------------------------------------------------*/

externC void
bd61FreeGarbage(
    Trans_Context&      trans,
    tbd_current_tree&   current,
    const tsp00_PageNo  pageNo,
    const bool          bTwice )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd61FreeGarbage", DataTree_Trace, 5 );

    tbd_node_ptrs pNode;
    pNode.np_ptr()= 0;

    bd13GetNode( current, pageNo, plmLock_ebd00, nr_for_update, pNode );
    if( trans.IsOk())
    {
        const tsp00_PageNo rightNeighbor = pNode.np_ptr()->nd_right();
        b13free_node( pNode, current );
        if( bTwice && ( rightNeighbor != NIL_PAGE_NO_GG00 ))
        {
            bd13GetNode( current, rightNeighbor, plmLock_ebd00, nr_for_update, pNode );
            if( trans.IsOk()){
                b13free_node( pNode, current );
            }
        }
    }
    if( 0 != pNode.np_ptr() ){
        b13r_release_node( pNode, current, lru_normal );
    }
}
/*---------------------------------------------------------------------------*/

externC void
bd61TruncRoot(
    tbd_current_tree&   current,
    tbd_node_ptrs&      pNode,
    const tsp00_Int4    newFileLength )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd61TruncRoot", DataTree_Trace, 5 );

    pNode.np_ptr()->ndStrFileSize_bd00() = newFileLength;
    pNode.np_ptr()->nd_bottom()          = BODY_BEG_BD00 + newFileLength;
    b13w_release_node( pNode, current );
}

/*---------------------------------------------------------------------------*/

externC void
bd61SearchLeaf (
    Trans_Context&      trans,
    tbd_current_tree&   current,
    tbd_node_ptrs&      pNode,
    const tsp00_Int4    pos,
    tsp00_Int4&         leafPos )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd61SearchLeaf", DataTree_Trace, 5 );

    tsp00_Int4          level = FIRST_INDEX_LEVEL_BD00;
    tbd_position_list   positionList;

    bd61_FillPositionList( level, pos, leafPos, positionList );

    level = pNode.np_ptr()->nd_level();

    while(( level > LEAF_LEVEL_BD00 ) && ( trans.IsOk()))
    {
        const tsp00_Int4   pageNoPos = positionList [level - POS_OFF_DIFF_BD00 ] *
                                       sizeof( tsp00_PageNo ) + BODY_BEG_BD00;
        const tsp00_PageNo subRoot   = s20buf_to_int4( pNode.np_ptr(), pageNoPos );

        b13r_release_node( pNode, current, lru_normal );
        bd13GetNode( current, subRoot, plmLock_ebd00, nr_for_read, pNode );
        --level;
    }
}

/*---------------------------------------------------------------------------*/

externC void
bd61SearchLeafWithClusteredRead(
    Trans_Context&              trans,
    tbd_current_tree&           current,
    tbd_node_ptrs&              pNode,
    Converter_PageCollector&    pageCollection,
    Data_PageFrame&             ioBuffer,
    const tsp00_Int4            maxPagesToRead,
    const tsp00_Int4            pos,
    tsp00_Int4&                 leafPos )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd61SearchLeafWithClusteredRead", DataTree_Trace, 5 );

    tsp00_Int4          level = FIRST_INDEX_LEVEL_BD00;
    tbd_position_list   positionList;

    bd61_FillPositionList( level, pos, leafPos, positionList );

    level = pNode.np_ptr()->nd_level();

    while(( level > LEAF_LEVEL_BD00 )&&( trans.IsOk()))
    {

        const tsp00_Int4   pageNoPos = positionList [level - POS_OFF_DIFF_BD00 ] *
                                       sizeof( tsp00_PageNo ) + BODY_BEG_BD00;
        const tsp00_PageNo subRoot   = s20buf_to_int4( pNode.np_ptr(), pageNoPos );

        if( FIRST_INDEX_LEVEL_BD00 == pNode.np_ptr()->nd_level()){
            bd61_ClusteredRead( trans, current.curr_tree_id, pNode, pageCollection,
                                ioBuffer, maxPagesToRead, pageNoPos );
        }
        b13r_release_node( pNode, current, lru_normal );
        if( trans.IsOk())
        {
            bd13GetNode( current, subRoot, plmLock_ebd00, nr_for_read, pNode );
            --level;
        }
    }
}

/*---------------------------------------------------------------------------*/

externC void
bd61Trunc(
    Trans_Context&      trans,
    tbd_current_tree&   current,
    tbd_node_ptrs&      pNode,
    tsp00_Int4          newFileLength,
    const bool          bDestroy )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd61Trunc", DataTree_Trace, 5 );

    tbd_node_ptrs nnptr;
    nnptr.np_ptr()   = 0;
    nnptr.np_cbptr() = 0;

    pNode.np_ptr()->ndStrFileSize_bd00() = newFileLength;
    const tsp00_Int4   act_root_level    = pNode.np_ptr()->nd_level();
    tsp00_Int4         leafPos;
    tbd_position_list  positionList;

    newFileLength  = newFileLength -1;

    bd61_FillPositionList( FIRST_INDEX_LEVEL_BD00, newFileLength, leafPos, positionList );

    tsp00_Int4       currLevel = pNode.np_ptr()->nd_level();
    tgg00_BasisError auxError  = e_ok;

    while(( currLevel > LEAF_LEVEL_BD00 ) && ( trans.IsOk()))
    {
        tsp00_Int4   pagePos = BODY_BEG_BD00 + positionList[ currLevel - POS_OFF_DIFF_BD00 ] * sizeof( tsp00_PageNo );
        tsp00_PageNo pageNo  = s20buf_to_int4( pNode.np_ptr(), pagePos );

        bd13GetNode( current, pageNo, plmLock_ebd00, nr_for_update, nnptr );
        if( ! trans.IsOk()){
            break;
        }
        pagePos = pagePos + sizeof( tsp00_PageNo );
        while(( pNode.np_ptr()->nd_bottom() > pagePos ))
        {
            pNode.np_ptr()->nd_bottom() = pNode.np_ptr()->nd_bottom() - sizeof( tsp00_PageNo );
            pageNo = s20buf_to_int4( pNode.np_ptr(), pNode.np_ptr()->nd_bottom());
            bd61_Free( trans, current, pageNo, currLevel );
            if( ! trans.IsOk())
            {
                if( e_bad_datapage == trans.OldTrans().trError_gg00 )
                {
                    auxError = e_bad_datapage;
                    trans.ResetErrorCode();
                    continue;
                }
                break; // fatal error
            }
        }
        bool bReduceTree = false;
        if( positionList[ currLevel - POS_OFF_DIFF_BD00 ] == 0 )
        {
            bReduceTree = true;
            tsp00_Int4 auxLevel = currLevel + 1;
            while( bReduceTree && ( auxLevel <= act_root_level ))
            {
                bReduceTree = ( positionList[ auxLevel - POS_OFF_DIFF_BD00 ] == 0 );
                ++auxLevel;
            }
        }
        if( bReduceTree )
        {
            const tsp00_PageNo oldPageNo      = pNode.np_ptr()->nd_id();
            const tsp00_Int4   oldFileSize    = pNode.np_ptr()->ndStrFileSize_bd00();
            const tsp00_Int4   oldLobVersion  = pNode.np_ptr()->nd_str_version();
            const tsp00_Int4   oldConvVersion = pNode.np_ptr()->nd_conv_version();
            const tsp00_Int4   oldLeft        = pNode.np_ptr()->nd_left();
            const tsp00_Int4   oldWriteCnt    = pNode.np_ptr()->nd_write_cnt();

            SAPDB_RangeMove( __FILE__, 2,
                             sizeof( tsp00_Page ), sizeof( tsp00_Page ),
                             nnptr.np_ptr(), POS_OFF_DIFF_BD00, pNode.np_ptr(), POS_OFF_DIFF_BD00,
                             nnptr.np_ptr()->nd_bottom() -1, trans.OldTrans().trError_gg00 );

            if( ! trans.IsOk())
            {
                trans.SetErrorCode( e_data_page_corrupted );
                {
                    Data_BasePage   dumpPage;
                    dumpPage.AssignNodePtr( nnptr.np_ptr());
                    Kernel_FileIO	dumpFile( KERNEL_DATA_FILE,	KERNEL_COR_FILE_EXTENSION,
                                            SAPDB_ToString( dumpPage.PageNo(), _T_d ));
                    dumpFile.Write(	dumpPage );
                }
                break;
            }
            pNode.np_ptr()->nd_id()              = oldPageNo;
            pNode.np_ptr()->ndStrFileSize_bd00() = oldFileSize;
            pNode.np_ptr()->nd_str_version()     = oldLobVersion;
            pNode.np_ptr()->nd_conv_version()    = oldConvVersion;
            pNode.np_ptr()->nd_left()            = oldLeft;
            pNode.np_ptr()->nd_write_cnt()       = oldWriteCnt;
            pNode.np_ptr()->nd_checktype().becomes( chckChecksumData_egg00 );
            pNode.np_ptr()->nd_trailer()         = pNode.np_ptr()->nd_header();

            b13free_node( nnptr, current );
        }
        else // do not reduce tree
        {
            b13w_release_node( pNode, current );
            pNode = nnptr;
        }
        --currLevel;
    }
    if( trans.IsOk()){
        trans.SetErrorCode( auxError );
    }
    if( trans.IsOk())
    {
        pNode.np_ptr()->nd_bottom() = BODY_BEG_BD00 + leafPos + 1;
        pNode.np_ptr()->nd_right()  = NIL_PAGE_NO_GG00;
        if( ! bDestroy ){
            b13w_release_node( pNode, current );
        }
    }
    if(( ! trans.IsOk()) && ( 0 != nnptr.np_ptr() )){
        b13r_release_node( nnptr, current, lru_normal );
    }
}

/*---------------------------------------------------------------------------*/

externC void
bd61SearchLeafForUpdate(
    Trans_Context&      trans,
    tbd_current_tree&   current,
    tbd_node_ptrs&      pNode,
    const tsp00_Int4    pos,
    tsp00_Int4&         leafPos,
    bool&               bPrevLastPageExist )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd61SearchLeafForUpdate", DataTree_Trace, 5 );

    tbd_position_list positionList;
    bd61_FillPositionList( FIRST_INDEX_LEVEL_BD00, pos, leafPos, positionList );

    tsp00_Int4 currLevel = pNode.np_ptr()->nd_level();
    bool bFirst          = true;
    bPrevLastPageExist   = false;
    while(( currLevel > LEAF_LEVEL_BD00 ) && ( trans.IsOk()))
    {
        tsp00_Int4 pagePos = positionList [ currLevel - POS_OFF_DIFF_BD00 ]
                             * sizeof( tsp00_PageNo ) + BODY_BEG_BD00;
        if( pagePos + static_cast<tsp00_Int4>(sizeof( tsp00_PageNo )) < pNode.np_ptr()->nd_bottom()){
            bPrevLastPageExist = true;
        }
        tsp00_PageNo nextPageNo = s20buf_to_int4( pNode.np_ptr(), pagePos );
        if( bFirst ){
            bFirst = false;
        }else{
            b13r_release_node( pNode, current, lru_normal );
        }
        if( currLevel > FIRST_INDEX_LEVEL_BD00 ){
            bd13GetNode( current, nextPageNo, plmLock_ebd00, nr_for_read, pNode );
        }
        else{
            bd13GetNode( current, nextPageNo, plmLock_ebd00, nr_for_update, pNode );
        }
        --currLevel;
    }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void
bd61_ClusteredRead(
    Trans_Context&              trans,
    tgg00_FileId&               fileId,
    const tbd_node_ptrs&        pIndex,
    Converter_PageCollector&    pageCollection,
    Data_PageFrame&             ioBuffer,
    const tsp00_Int4            maxPagesToRead,
    tsp00_Int4                  pageNoPos )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd61_ClusteredRead", DataTree_Trace, 7 );

    SAPDBERR_ASSERT_STATE( FIRST_INDEX_LEVEL_BD00 == pIndex.np_ptr()->nd_level());

    if( ! pageCollection.Initialize()){ // reinitialization to be able to store new pageNo's
        return;
    }
    tsp00_Int4  readPageCount = 0;
    while(( pageNoPos < pIndex.np_ptr()->nd_bottom())&&( readPageCount < maxPagesToRead ))
    {
        const tsp00_PageNo leafPageNo = s20buf_to_int4( pIndex.np_ptr(), pageNoPos );
        if( ! pageCollection.AppendPageNo( leafPageNo )){
            return; // ignore memory exhausted error
        }
        ++readPageCount;
        pageNoPos  += sizeof( tsp00_PageNo );
    }
    pageCollection.DetermineCluster( trans.Task());
    Data_PageNoList         pageNoList( trans.Allocator());
    IOMan_ClusterAddress    cluster;

    if( ! pageNoList.Reserve( maxPagesToRead )){
        return; // ignore memory exhausted error
    }
    for( SAPDB_UInt clusterID = Converter_PageCollector::FirstClusterId();
            clusterID <= pageCollection.GetClusterCount(); clusterID++ )
    {
        pageCollection.GetCluster( clusterID, pageNoList, cluster );
        SAPDBERR_ASSERT_STATE( cluster.GetBlockCount() <= maxPagesToRead );
        bd13LoadNodes( trans, fileId, pageCollection.GetRecoveryMode(),
                       pageNoList, cluster, ioBuffer );
        if( ! trans.IsOk()){
            return;
        }
    }
}

/*---------------------------------------------------------------------------*/

void
bd61_Free(
    Trans_Context&      trans,
    tbd_current_tree&   current,
    const tsp00_PageNo  pageNo,
    const tsp00_Int4    currLevel )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd61_Free", DataTree_Trace, 7 );

    tbd_node_ptrs pNode;
    pNode.np_ptr() = 0;

    if( currLevel == FIRST_INDEX_LEVEL_BD00 )
    {
        bd13GetNode( current, pageNo, plmLock_ebd00, nr_for_update, pNode );
        if( trans.IsOk()){
            b13free_node( pNode, current ); // free leaf page
        }else{
            if( 0 != pNode.np_ptr()){
                b13r_release_node( pNode, current, lru_normal );
            }
        }
        return;
    }
    bd13GetNode( current, pageNo, plmLock_ebd00, nr_for_update, pNode );
    if( trans.IsOk())
    {
        tsp00_Int4          pagePos   = BODY_BEG_BD00;
        const tsp00_Int4    nextLevel = pNode.np_ptr()->nd_level();
        tgg00_BasisError    auxError  = e_ok;
        while(( pagePos < pNode.np_ptr()->nd_bottom())&&( trans.IsOk()))
        {
            const tsp00_PageNo nextPageNo = s20buf_to_int4( pNode.np_ptr(), pagePos );
            bd61_Free( trans, current, nextPageNo, nextLevel );
            pagePos += sizeof( tsp00_PageNo );
            if( e_bad_datapage == trans.OldTrans().trError_gg00 )
            {
                auxError = e_bad_datapage;
                trans.ResetErrorCode();
            }
        }
        if( trans.IsOk()){
            b13free_node( pNode, current );
        }
        if( trans.IsOk()){
            trans.SetErrorCode( auxError );
        }
    }
    if( trans.IsOk()){
        return;
    }
    if( 0 != pNode.np_ptr()){
        b13r_release_node( pNode, current, lru_normal );
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/

