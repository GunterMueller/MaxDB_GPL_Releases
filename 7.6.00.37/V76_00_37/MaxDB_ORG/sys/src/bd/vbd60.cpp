/*
  @file           vbd61.cpp
  @author         TorstenS
  @ingroup        DataAccess
  @brief          LOB  handling

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

#include "hbd06.h"
#include "hbd13.h"
#include "hbd17.h"
#include "hbd20_9.h"
#include "hbd30.h"
#include "hbd60.h" 
#include "hbd61.h"
#include "hbd998.h"
#include "hgg01_1.h"
#include "gsp03.h"
#include "gsp03_3.h"
#include "hsp20.h"

#include "Converter/Converter_Version.hpp"
#include "Converter/Converter_IVerify.hpp"
#include "DataAccess/Data_BasePage.hpp"
#include "KernelCommon/Kernel_FileIO.hpp"
#include "RunTime/RTE_Message.hpp"
#include "SAPDBCommon/SAPDB_RangeCode.hpp" // Kernel_move_and_fill
#include "SAPDBCommon/Messages/SDBMsg_Data.h"
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
bd60_Fill(
    Trans_Context&      trans,
    tbd_node* const     pNode,
    tsp00_Int4          pos,
    const tsp00_Int4    length,
    const char          fillChar )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd60_Fill", DataTree_Trace, 6 );

    pos = pos + BODY_BEG_BD00;
    SAPDB_RangeFill( __FILE__, 1,
                     sizeof( tbd_node ), pNode,
                     pos, length, fillChar, trans.OldTrans().trError_gg00 );

    if(( pos + length ) > pNode->nd_bottom()){
        pNode->nd_bottom() = pos + length;
    }
}

/*---------------------------------------------------------------------------*/

inline void
bd60_Write(
    Trans_Context&          trans,
    tbd_node* const         pNode,
    tsp00_Int4              pos,
    const tsp00_Int4        length,
    const tsp00_Int4        bufferSize,
    const tsp00_Byte* const pBuffer,
    const tsp00_Int4        bufferPos )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd60_Write", DataTree_Trace, 6 );

    pos = pos + BODY_BEG_BD00;
    SAPDB_RangeMove( __FILE__, 2,
                     bufferSize, sizeof( tbd_node ),
                     pBuffer, bufferPos, pNode, pos,
                     length, trans.OldTrans().trError_gg00 );
    if(( pos + length ) > pNode->nd_bottom()){
        pNode->nd_bottom() = pos + length;
    }
}

/*---------------------------------------------------------------------------*/

void
bd60_Check(
    Trans_Context&          trans,
    tbd_current_tree&       current,
    const tbd_node_ptrs&    pParentNode,
    const bool              bUpdateConverter,
    tsp00_Int4&             numPages,
    const bool              bUpdateLeaves );


/*===========================================================================*
 *  METHODS -lll-                                                            *
 *===========================================================================*/


externC void
bd60ContPutLeaves(
    Trans_Context&          trans,
    tbd_current_tree&       current,
    tbd_node_ptrs&          pNode,
    const tsp00_Int4        pos,
    tsp00_Int4              length,
    const tsp00_Byte* const pBuffer,
    const tsp00_Int4        bufferSize,
    bool&                   bFirstCall,
    const bool              bIsExtended,
    tsp00_Int4&             fileLength )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd60ContPutLeaves", DataTree_Trace, 5 );

    bool            bPrevLastPageExist = false;
    tsp00_Int4      leafPos;
    tbd_node_ptrs   pRoot;

    if( bFirstCall )
    {
        pRoot = pNode;
        bd61SearchLeafForUpdate( trans, current, pNode, pos, leafPos, bPrevLastPageExist );
        bFirstCall = false;
    }
    else
    {
        pRoot.np_ptr()   = 0;
        leafPos          = pos % MAX_BSTR_COVERING_BD00;
    }
    if( ! trans.IsOk())
    {
        if( 0 != pRoot.np_ptr()){
            b13r_release_node( pRoot, current, lru_normal );;
        }
        return;
    }
    bool         bIsReady   = false;
    tsp00_Int4   bufferPos  = 1;
    tsp00_PageNo nextPageNo = NIL_PAGE_NO_GG00;
    do {
        if( nextPageNo != NIL_PAGE_NO_GG00 )
        {
            b13w_release_node( pNode, current );
            bd13GetNode( current, nextPageNo, plmLock_ebd00, nr_for_update, pNode );
        }
        if( ! trans.IsOk()){
            break;
        }
        const tsp00_Int4 partLength = (( leafPos + length ) > MAX_BSTR_COVERING_BD00 )?
                                      ( MAX_BSTR_COVERING_BD00 - leafPos ) : length;

        bd60_Write( trans, pNode.np_ptr(), leafPos, partLength, bufferSize, pBuffer, bufferPos );
        if( ! trans.IsOk()){
            break;
        }
        if( partLength == length ){
            bIsReady = true;
        }else
        {
            length     = length - partLength;
            bufferPos += partLength;
            leafPos    = 0;
            nextPageNo = pNode.np_ptr()->nd_right();
        }
    }
    while( ! bIsReady );

    if( trans.IsOk() && bIsExtended && ( pNode.np_ptr()->nd_right() != NIL_PAGE_NO_GG00 ))
    {
        bd61FreeGarbage( trans, current, pNode.np_ptr()->nd_right(), true );
        if( trans.IsOk()){
            pNode.np_ptr()->nd_right() = NIL_PAGE_NO_GG00;
        }
    }
    if( trans.IsOk())
    {
        if(( pRoot.np_ptr() != 0 ) && bIsExtended )
        {
            pRoot.np_ptr()->ndStrFileSize_bd00() = pos + length;
            fileLength = pRoot.np_ptr()->ndStrFileSize_bd00();
            b13w_release_node( pRoot, current );
        }
        else
            if( bIsExtended )
            {
                bd13GetNode( current, current.curr_tree_id.fileRoot_gg00(),
                             plmLock_ebd00, nr_for_update, pRoot );
                if( trans.IsOk())
                {
                    pRoot.np_ptr()->ndStrFileSize_bd00() = pos + length;
                    fileLength = pRoot.np_ptr()->ndStrFileSize_bd00();
                    b13w_release_node( pRoot, current );
                }
            }
        if( trans.IsOk() && ( 0 != pRoot.np_ptr()))
        {
            if( bIsExtended ){
                b13w_release_node( pRoot, current );
            }
            else{
                b13r_release_node( pRoot, current, lru_normal );
            }
        }
    }
    if( trans.IsOk()){
        return;
    }
    if( 0 != pRoot.np_ptr()){
        b13r_release_node( pRoot, current, lru_normal );
    }
}

/*---------------------------------------------------------------------------*/

externC void
bd60Extend(
    Trans_Context&      trans,
    tbd_current_tree&   current,
    tbd_node_ptrs&      pNode,
    const tsp00_Int4    pos,
    tsp00_Int4          length,
    const bool          bUseBuffer,
    const char          fillChar,
    const tsp00_Int4    bufferSize,
    const tsp00_Byte*   pBuffer,
    tsp00_Int4          bufferPos )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd60Extend", DataTree_Trace, 5 );

    tbd_node_ptrs pCurrRoot;
    tbd_node_ptrs inew_nptr;
    tbd_node_ptrs iinew_nptr;
    tbd_node_ptrs last_nptr;

    pCurrRoot.np_ptr()  = 0;
    inew_nptr.np_ptr()  = 0;
    iinew_nptr.np_ptr() = 0;
    last_nptr.np_ptr()  = 0;

    bool            bChangePageNo      = false;
    bool            bPrevLastPageExist = false;
    tsp00_Int4      leafPos;
    tsp00_Int4      partLength;
    tsp00_Int4      newLength = pos + length;
    tsp00_PageNo    inew_pno  = NIL_PAGE_NO_GG00;
    tsp00_PageNo    iinew_pno = NIL_PAGE_NO_GG00;

    if( pNode.np_ptr()->nd_id() == current.curr_tree_id.fileRoot_gg00())
    {
        pCurrRoot = pNode;
        if(( pos % MAX_BSTR_COVERING_BD00 ) == 0 )
        {
            bd61SearchLeafForUpdate( trans, current, pNode, pos - 1, leafPos,bPrevLastPageExist );
            leafPos = leafPos + 1;
        }
        else{
            bd61SearchLeafForUpdate( trans, current, pNode, pos, leafPos, bPrevLastPageExist );
        }
    }
    else
    {
        leafPos = MAX_BSTR_COVERING_BD00;
        bd13GetNode( current, current.curr_tree_id.fileRoot_gg00(), plmLock_ebd00, nr_for_update, pCurrRoot );
    }
    if(( pNode.np_ptr()->nd_right() != NIL_PAGE_NO_GG00 ) && ( trans.IsOk()))
    {
        if( bPrevLastPageExist ){
            bChangePageNo = true;
        }
        bd13GetNode( current, pNode.np_ptr()->nd_right(), plmLock_ebd00, nr_for_update, inew_nptr );
    }
    else
        b13new_node( LEAF_LEVEL_BD00, inew_nptr, current );

    if( trans.IsOk())
    {
        if( leafPos != MAX_BSTR_COVERING_BD00 )
        {
            partLength = MAX_BSTR_COVERING_BD00 - leafPos;
            if( bUseBuffer ){
                bd60_Write( trans, pNode.np_ptr(), leafPos, partLength, bufferSize, pBuffer, bufferPos );
            }
            else{
                bd60_Fill( trans, pNode.np_ptr(), leafPos, partLength, fillChar );
            }
            bufferPos += partLength;
            length    -= partLength;
        }
        if( trans.IsOk())
        {
            pNode.np_ptr()->nd_right() = inew_nptr.np_ptr()->nd_id();
            leafPos                    = 0;
            if  (length > MAX_BSTR_COVERING_BD00)
            {
                partLength = MAX_BSTR_COVERING_BD00;
                if( inew_nptr.np_ptr()->nd_right() == NIL_PAGE_NO_GG00 ){
                    b13new_node( LEAF_LEVEL_BD00, iinew_nptr, current );
                }
                else{
                    bd13GetNode( current, inew_nptr.np_ptr()->nd_right(),  plmLock_ebd00, nr_for_update, iinew_nptr );
                }
            }
            else
            {
                partLength = length;
                if( inew_nptr.np_ptr()->nd_right() != NIL_PAGE_NO_GG00 )
                {
                    bd61FreeGarbage( trans, current, inew_nptr.np_ptr()->nd_right(), false );
                    if( trans.IsOk()){
                        inew_nptr.np_ptr()->nd_right() = NIL_PAGE_NO_GG00;
                    }
                }
            }
        }
        if( trans.IsOk())
        {
            if ( bUseBuffer ){
                bd60_Write( trans, inew_nptr.np_ptr(), leafPos, partLength, bufferSize, pBuffer, bufferPos );
            }
            else{
                bd60_Fill( trans, inew_nptr.np_ptr(), leafPos, partLength, fillChar );
            }
            if( trans.IsOk())
            {
                inew_nptr.np_ptr()->nd_left() = pNode.np_ptr()->nd_id();
                length -= partLength;
                if( length != 0 )
                {
                    inew_nptr.np_ptr()->nd_right() = iinew_nptr.np_ptr()->nd_id();
                    bufferPos += partLength;
                    if( bUseBuffer ){
                        bd60_Write( trans, iinew_nptr.np_ptr(), leafPos, length, bufferSize, pBuffer, bufferPos );
                    }
                    else{
                        bd60_Fill( trans, iinew_nptr.np_ptr(), leafPos, length, fillChar );
                    }
                    if( trans.IsOk())
                    {
                        iinew_nptr.np_ptr()->nd_left() = inew_nptr.np_ptr()->nd_id();
                        iinew_pno = iinew_nptr.np_ptr()->nd_id();
                        if( bUseBuffer ){
                            b13w_release_node (iinew_nptr, current);
                        }
                        else{
                            last_nptr = iinew_nptr;
                        }
                    }
                }
            }
            if( trans.IsOk())
            {
                inew_pno = inew_nptr.np_ptr()->nd_id();

                if(( last_nptr.np_ptr() != 0 ) || bUseBuffer ){
                    b13w_release_node( inew_nptr, current );
                }
                else{
                    last_nptr = inew_nptr;
                }
                if( trans.IsOk())
                {
                    b13w_release_node( pNode, current );
                    if( trans.IsOk())
                    {
                        pNode = last_nptr;
                        pCurrRoot.np_ptr()->ndStrFileSize_bd00() = newLength;
                        if ( bChangePageNo ){
                            bd61AddIndex( trans, current, pCurrRoot, pCurrRoot.np_ptr()->nd_id(), iinew_pno,
                                          NIL_PAGE_NO_GG00, FIRST_INDEX_LEVEL_BD00 );
                        }
                        else{
                            bd61AddIndex( trans, current, pCurrRoot, pCurrRoot.np_ptr()->nd_id(), inew_pno,
                                          iinew_pno, FIRST_INDEX_LEVEL_BD00 );
                        }
                    }
                }
            }
        }
    }
    if( trans.IsOk())
    {
        if( inew_nptr.np_ptr() != 0 ){
            b13r_release_node( inew_nptr, current, lru_normal );
        }
        if( iinew_nptr.np_ptr() != 0 ){
            b13r_release_node( iinew_nptr, current, lru_normal );
        }
        if( pCurrRoot.np_ptr() != 0 ){
            b13r_release_node( pCurrRoot, current, lru_normal );
        }
        if( last_nptr.np_ptr() != 0 ){
            b13r_release_node( last_nptr, current, lru_normal );
        }
    }
}

/*---------------------------------------------------------------------------*/


externC void
bd60Get(
    Trans_Context&      trans,
    tbd_current_tree&   current,
    tbd_node_ptrs&      pNode,
    tsp00_Int4          pos,
    tsp00_Int4          length,
    tsp00_Int4          bufferSize,
    tsp00_Byte* const   pBuffer,
    tsp00_Int4          bufferPos )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd60Get", DataTree_Trace, 5 );

    tsp00_PageNo nextPageNo = NIL_PAGE_NO_GG00;
    bool         bIsReady = false;

    do {
        if( nextPageNo != NIL_PAGE_NO_GG00 ){
            bd13GetNode( current, nextPageNo, plmLock_ebd00, nr_for_read, pNode );
        }
        if( ! trans.IsOk()){
            return;
        }
        const tsp00_Int4 partLength = (( pos + length ) >= MAX_BSTR_COVERING_BD00 )?
                                      ( MAX_BSTR_COVERING_BD00 - pos ) : length;

        bd60Read( trans, pNode.np_ptr(), pos, partLength,  bufferSize, pBuffer, bufferPos );
        if( ! trans.IsOk()){
            return;
        }
        if( partLength == length ){
            bIsReady = true;
        }
        else
        {
            length    -= partLength;
            bufferPos += partLength;
            pos        = 0;
            nextPageNo = pNode.np_ptr()->nd_right();
            b13r_release_node( pNode, current, lru_normal );
        }
    } while( ! bIsReady );
}

/*---------------------------------------------------------------------------*/

externC void
bd60Read(
    Trans_Context&          trans,
    const tbd_node* const   pNode,
    tsp00_Int4              pos,
    const tsp00_Int4        length,
    const tsp00_Int4        bufferSize,
    tsp00_Byte* const       pBuffer,
    const tsp00_Int4        bufferPos )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd60Read", DataTree_Trace, 5 );

    pos = pos + BODY_BEG_BD00;
    SAPDB_RangeMove( __FILE__, 3,
                     sizeof( tbd_node ), bufferSize, pNode, pos,
                     pBuffer, bufferPos, length, trans.OldTrans().trError_gg00 );
}

/*---------------------------------------------------------------------------*/

externC void
bd60PutIntoLeaves(
    Trans_Context&          trans,
    tbd_current_tree&       current,
    tbd_node_ptrs&          pNode,
    tsp00_Int4              pos,
    tsp00_Int4              length,
    const bool              bUseBuffer,
    const char              fillChar,
    const tsp00_Int4        bufferSize,
    const tsp00_Byte* const pBuffer,
    tsp00_Int4              bufferPos,
    const bool              bIsExtended )

{
    SAPDBTRACE_ROUTINE_DEBUG( "bd60PutIntoLeaves", DataTree_Trace, 5 );

    bool            bPrevLastPageExist = false;
    tsp00_Int4      leafPos;
    tbd_node_ptrs   pRoot = pNode;

    if( bIsExtended ){
        pRoot.np_ptr()->ndStrFileSize_bd00() = pos + length;
    }
    if( pNode.np_ptr()->nd_id() == current.curr_tree_id.fileRoot_gg00()){
        bd61SearchLeafForUpdate( trans, current, pNode, pos, leafPos, bPrevLastPageExist );
    }
    if( ! trans.IsOk())
    {
        if( 0 != pRoot.np_ptr()){
            b13r_release_node( pRoot, current, lru_normal );
        }
        return;
    }
    tsp00_PageNo nextPageNo = NIL_PAGE_NO_GG00;
    bool           bIsReady = false;

    do {
        if( nextPageNo != NIL_PAGE_NO_GG00 )
        {
            b13w_release_node( pNode, current );
            bd13GetNode( current, nextPageNo, plmLock_ebd00, nr_for_update, pNode );
        }
        if( ! trans.IsOk()){
            break;
        }
        const tsp00_Int4 partLength = (( leafPos + length) > MAX_BSTR_COVERING_BD00 )?
                                      ( MAX_BSTR_COVERING_BD00 - leafPos ) : length;
        if( bUseBuffer ){
            bd60_Write( trans, pNode.np_ptr(), leafPos, partLength, bufferSize, pBuffer, bufferPos );
        }
        else{
            bd60_Fill( trans, pNode.np_ptr(), leafPos, partLength, fillChar );
        }
        if( ! trans.IsOk()){
            break;
        }
        if( partLength == length ){
            bIsReady = true;
        }
        else
        {
            length    -= partLength;
            bufferPos += partLength;
            leafPos    = 0;
            nextPageNo = pNode.np_ptr()->nd_right();
        }
    } while( ! bIsReady );

    if(( trans.IsOk()) && bIsExtended && ( pNode.np_ptr()->nd_right() != NIL_PAGE_NO_GG00 ))
    {
        bd61FreeGarbage( trans, current, pNode.np_ptr()->nd_right(), true );
        if( trans.IsOk()){
            pNode.np_ptr()->nd_right() = NIL_PAGE_NO_GG00;
        }
    }
    if( trans.IsOk())
    {
        if( bUseBuffer ){
            b13w_release_node( pNode, current );
        }
        if( bIsExtended && bUseBuffer ){
            b13w_release_node( pRoot, current );
        }
        else{
            b13r_release_node( pRoot, current, lru_normal );
        }
    }
    if((! trans.IsOk()) && ( pRoot.np_ptr() != 0 )){
        b13r_release_node( pRoot, current, lru_normal );
    }
}

/*---------------------------------------------------------------------------*/

externC void
bd60NewLob( tbd_current_tree& current )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd60NewLob", DataTree_Trace, 5 );

    Trans_Context&  trans = Trans_Context::GetContext( *current.curr_trans );

    tbd_node_ptrs pNode;

    current.curr_tree_id.fileType_gg00().clear();
    current.curr_tree_id.fileType_gg00().addElement( ftsByteStr_egg00 );
    current.curr_tree_id.fileType_gg00().addElement( ftsConcurrent_egg00 );
    current.curr_tree_id.fileRoot_gg00() = NIL_PAGE_NO_GG00;

    b13new_root( pNode, current );
    if( ! trans.IsOk()){
        return;
    }
    current.curr_tree_id.fileRoot_gg00() = pNode.np_ptr()->nd_id();
    b13w_release_node( pNode, current );
}

/*---------------------------------------------------------------------------*/

externC void
bd60Drop( 
    Trans_Context&      trans,
    tbd_current_tree&   current )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd60Drop", DataTree_Trace, 5 );

    tgg00_FileId&   fileId = current.curr_tree_id;

    if(
        ( fileId.fileRoot_gg00() == bd17GetBlobFdirRoot())&&
        ( fileId.fileType_gg00().includes( ftsPerm_egg00 )))
    {
        trans.SetErrorCode( e_do_not_drop_fdir );
        return;
    }
    tgg00_BasisError    auxError = e_ok;
    tbd_node_ptrs       pNode;
    pNode = current.currRootNptrs_bd00;

    bd61Trunc( trans, current, pNode, 0, true );
    if( e_bad_datapage == trans.OldTrans().trError_gg00 ){
        auxError = e_bad_datapage;
        trans.ResetErrorCode();
    }
    if( trans.IsOk())
    {
        b13w_release_node( pNode, current );
        pNode = current.currRootNptrs_bd00;
        b13free_node( pNode, current );
    }
    if(( e_ok != auxError )&&( trans.IsOk())){
        trans.SetErrorCode( auxError );
    }
    if( trans.IsOk()){
        return;
    }
    if( 0 != pNode.np_ptr()){
        b13r_release_node( pNode, current, lru_normal );
    }
}

/*---------------------------------------------------------------------------*/

externC void
bd60ExtendRoot(
    Trans_Context&          trans,
    tbd_current_tree&       current,
    tbd_node_ptrs&          pNode,
    tsp00_Int4              pos,
    tsp00_Int4              length,
    const bool              bUseBuffer,
    const char              fillChar,
    const tsp00_Int4        bufferSize,
    const tsp00_Byte* const pBuffer,
    tsp00_Int4              bufferPos )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd60ExtendRoot", DataTree_Trace, 5 );

    tsp00_Int4      leafPos   = ( bUseBuffer )? pos : MAX_BSTR_COVERING_BD00;
    tsp00_Int4      newLength = pos + length;
    tsp00_PageNo    iinew_pno = NIL_PAGE_NO_GG00;

    tbd_node_ptrs inew_nptr;
    tbd_node_ptrs iinew_nptr;
    tbd_node_ptrs pNewRoot;
    tbd_node_ptrs pLast;

    pLast.np_ptr()      = 0;
    inew_nptr.np_ptr()  = 0;
    iinew_nptr.np_ptr() = 0;
    pNewRoot.np_ptr()   = 0;

    b13new_node( LEAF_LEVEL_BD00, pNewRoot, current );
    if( ! trans.IsOk())
    {
        if( pNewRoot.np_ptr() != 0 ){
            b13r_release_node( pNewRoot, current, lru_normal );
        }
        return;
    }

    if( pNode.np_ptr()->nd_right() == NIL_PAGE_NO_GG00 ){
        b13new_node( LEAF_LEVEL_BD00, inew_nptr, current );
    }
    else{
        bd13GetNode( current, pNode.np_ptr()->nd_right(), plmLock_ebd00, nr_for_update, inew_nptr );
    }
    if( trans.IsOk())
    {
        tsp00_Int4 partLength;
        if( leafPos != MAX_BSTR_COVERING_BD00 )
        {
            partLength = MAX_BSTR_COVERING_BD00 - leafPos;
            if( bUseBuffer ){
                bd60_Write( trans, pNode.np_ptr(), leafPos, partLength, bufferSize, pBuffer, bufferPos );
            }
            else{
                bd60_Fill( trans, pNode.np_ptr(), leafPos,  partLength, fillChar );
            }
            bufferPos += partLength;
            length    -= partLength;
        }
        if( trans.IsOk())
        {
            pNode.np_ptr()->nd_right() = inew_nptr.np_ptr()->nd_id();
            leafPos = 0;
            if( length > MAX_BSTR_COVERING_BD00 )
            {
                partLength = MAX_BSTR_COVERING_BD00;
                if( inew_nptr.np_ptr()->nd_right() == NIL_PAGE_NO_GG00 ){
                    b13new_node( LEAF_LEVEL_BD00, iinew_nptr, current );
                }
                else{
                    bd13GetNode( current, inew_nptr.np_ptr()->nd_right(),
                                 plmLock_ebd00, nr_for_update, iinew_nptr );
                }
            }
            else
            {
                partLength = length;
                if( inew_nptr.np_ptr()->nd_right() != NIL_PAGE_NO_GG00 )
                {
                    bd61FreeGarbage( trans, current, inew_nptr.np_ptr()->nd_right(), false );
                    if( trans.IsOk()){
                        inew_nptr.np_ptr()->nd_right() = NIL_PAGE_NO_GG00;
                    }
                }
            }
        }
        if( trans.IsOk())
        {
            if( bUseBuffer ){
                bd60_Write( trans, inew_nptr.np_ptr(), leafPos, partLength, bufferSize, pBuffer, bufferPos );
            }
            else{
                bd60_Fill( trans, inew_nptr.np_ptr(), leafPos, partLength, fillChar );
            }
            length -= partLength;
            if( trans.IsOk())
            {
                if( length != 0 )
                {
                    inew_nptr.np_ptr()->nd_right() = iinew_nptr.np_ptr()->nd_id();
                    bufferPos += partLength;
                    if( bUseBuffer ){
                        bd60_Write( trans, iinew_nptr.np_ptr(), leafPos, length, bufferSize, pBuffer, bufferPos );
                    }
                    else{
                        bd60_Fill( trans, iinew_nptr.np_ptr(), leafPos, length, fillChar );
                    }
                    if( trans.IsOk())
                    {
                        iinew_nptr.np_ptr()->nd_left() = inew_nptr.np_ptr()->nd_id();
                        iinew_pno = iinew_nptr.np_ptr()->nd_id();

                        if( bUseBuffer ){
                            b13w_release_node( iinew_nptr, current );
                        }
                        else{
                            pLast = iinew_nptr;
                        }
                    }
                }
            }
            if( trans.IsOk())
            {
                tsp00_PageNo        pageNo   = NIL_PAGE_NO_GG00;
                const tsp00_PageNo  inew_pno = inew_nptr.np_ptr()->nd_id();
                bd61ChangeContent(  trans, pNode, pNewRoot );
                if( trans.IsOk())
                {
                    inew_nptr.np_ptr()->nd_left() = pNode.np_ptr()->nd_id();
                    pageNo = pNode.np_ptr()->nd_id();

                    if(( pLast.np_ptr() != 0 ) || bUseBuffer ){
                        b13w_release_node (inew_nptr, current);
                    }
                    else{
                        pLast = inew_nptr;
                    }
                }
                if( trans.IsOk())
                {
                    b13w_release_node( pNode, current );
                    if( trans.IsOk())
                    {
                        pNode     = pLast;
                        tsp00_Int4 indexPagePos = BODY_BEG_BD00;
                        s20int4_to_buf( pageNo, pNewRoot.np_ptr(), indexPagePos);
                        indexPagePos += sizeof( tsp00_PageNo );
                        s20int4_to_buf( inew_pno, pNewRoot.np_ptr(), indexPagePos );
                        if( iinew_pno != NIL_PAGE_NO_GG00 )
                        {
                            indexPagePos += sizeof( tsp00_PageNo );
                            s20int4_to_buf( iinew_pno, pNewRoot.np_ptr(), indexPagePos );
                        }
                        pNewRoot.np_ptr()->nd_bottom()          = indexPagePos + sizeof( tsp00_PageNo );
                        pNewRoot.np_ptr()->ndStrFileSize_bd00() = newLength;
                        b13w_release_node( pNewRoot, current );
                    }
                }
            }
        }
    }

    if( trans.IsOk()){
        return;
    }
    if( inew_nptr.np_ptr() != 0 ){
        b13r_release_node( inew_nptr, current, lru_normal );
    }
    if( iinew_nptr.np_ptr() != 0 ){
        b13r_release_node( iinew_nptr, current, lru_normal );
    }
    if( pNewRoot.np_ptr() != 0 ){
        b13r_release_node( pNewRoot, current, lru_normal );
    }
    if( pLast.np_ptr() != 0 ){
        b13r_release_node( pLast, current, lru_normal );
    }
}

/*---------------------------------------------------------------------------*/

externC void
bd60PutIntoRoot(
    Trans_Context&          trans,
    tbd_current_tree&       current,
    tbd_node_ptrs&          pNode,
    tsp00_Int4              pos,
    tsp00_Int4              length,
    const bool              bUseBuffer,
    const char              fillChar,
    const tsp00_Int4        bufferSize,
    const tsp00_Byte* const pBuffer,
    const tsp00_Int4        bufferPos,
    const bool              bExtended )
{

    SAPDBTRACE_ROUTINE_DEBUG( "bd60PutIntoRoot", DataTree_Trace, 5 );

    if( pNode.np_ptr()->nd_right() != NIL_PAGE_NO_GG00 )
    {
        bd61FreeGarbage( trans, current, pNode.np_ptr()->nd_right(), true );
        if( trans.IsOk()){
            pNode.np_ptr()->nd_right() = NIL_PAGE_NO_GG00;
        }
    }
    if( ! trans.IsOk()){
        return;
    }
    tsp00_Int4 leafPos = pos;
    if( bUseBuffer ){
        bd60_Write( trans, pNode.np_ptr(), leafPos, length,  bufferSize, pBuffer, bufferPos );
    }
    else{
        bd60_Fill( trans, pNode.np_ptr(), leafPos, length, fillChar );
    }
    if( bExtended ){
        pNode.np_ptr()->ndStrFileSize_bd00() = pos + length;
    }
    if(( trans.IsOk()) && bUseBuffer ){
        b13w_release_node( pNode, current );
    }
}

/*---------------------------------------------------------------------------*/

externC void
bd60CheckLob(
    tgg00_TransContext&     t,
    tgg00_FileId&           fileId,
    const tgg00_Surrogate&  parentFileNo,
    const bool              bModifyLobCounter,
    const bool              bUpdateLeaves )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd60CheckLob", DataTree_Trace, 5 );

    Trans_Context&  trans = Trans_Context::GetContext( t );

    const bool c_bUpdateConverter = true;
    bool       bFileWasBad        = false;;

    tbd_current_tree    current;
    fileId.fileRoot_gg00() = NIL_PAGE_NO_GG00;

    bd30GetTree( trans.OldTrans(), fileId, current, m_verify, ! LOCK_TREE_EXCL_BD00, ! SYNC_TREE_LOCK_BD00 );
    if( trans.IsOk())
    {
        tsp00_Int4      numPages = 1;
        tbd_node_ptrs   pNode    = current.currRootNptrs_bd00;
        bFileWasBad = (pNode.np_ptr()->nd_file_state().includes( f_bad ));

        if( pNode.np_ptr()->nd_level() > LEAF_LEVEL_BD00 ){
            bd60_Check( trans, current, pNode, ! c_bUpdateConverter, numPages, bUpdateLeaves );
        }
        /* check numPages == current-filesize */
        const tsp00_Int4 flength  = pNode.np_ptr()->ndStrFileSize_bd00();
        const tsp00_Int4 pages    = 1 +  /* root page */
                                    pNode.np_ptr()->nd_level() +
                                    (flength / (PNOS_PER_PAGE_BD00 * MAX_BSTR_COVERING_BD00)) +
                                    (flength / MAX_BSTR_COVERING_BD00 );
        if( bModifyLobCounter )
        {
            const Converter_Version convVersion =
                bd20GetDestinationConverterVersion( trans.Task().ID(), pNode.np_cbptr());

            bd998ModifyLobPageCount( trans.OldTrans(), convVersion, parentFileNo, pages );
        }
    }
    bd30ReleaseTree( current );
    if( trans.IsOk())
    {
        fileId = current.curr_tree_id;
        if( bFileWasBad ){
            bd30ResetBadFile( trans.OldTrans(), fileId );
        }
    }
    else
        if(
            ( trans.OldTrans().trError_gg00 != e_file_not_found      ) &&
            ( trans.OldTrans().trError_gg00 != e_disk_not_accessible ) &&
            ( trans.OldTrans().trError_gg00 != e_cancelled           ) &&
            ( trans.OldTrans().trError_gg00 != e_shutdown           ))
        {
            b06file_opmsg( csp3_bad_long, current.curr_tree_id );
        }
}

/*---------------------------------------------------------------------------*/

externC void
bd60GetNextByteLeafPage(
    tsp00_PageNo&       nextLeafPage,
    tbd_node_ptrs&      pNode,
    tbd_current_tree&   current)

{
    SAPDBTRACE_ROUTINE_DEBUG( "bd60GetNextByteLeafPage", DataTree_Trace, 5 );

    Trans_Context&  trans = Trans_Context::GetContext( *current.curr_trans );

    tsp00_Int4 leafpos;

    if( nextLeafPage == current.curr_tree_id.fileRoot_gg00() )
    {
        bd13GetNode( current, nextLeafPage, plmLock_ebd00, nr_for_read, pNode );
        if( trans.IsOk()){
            bd61SearchLeaf( trans, current, pNode, 1, leafpos );
        }
    }
    else
    {
        if( pNode.np_ptr() != 0 ){
            b13r_release_node( pNode, current, lru_last );
        }
        {
            if( nextLeafPage != NIL_PAGE_NO_GG00 ){
                bd13GetNode( current, nextLeafPage, plmLock_ebd00, nr_for_read, pNode );
            }
        }
    }
    if( pNode.np_ptr() != 0 )
    {
        if( trans.IsOk()){
            nextLeafPage = pNode.np_ptr()->nd_right();
        }
        else{
            b13r_release_node( pNode, current, lru_normal );
        }
    }
}

/*---------------------------------------------------------------------------*/

externC void
bd60VerifyLob (
    tgg00_TransContext& t,
    tgg00_FileId&       fileId,
    const bool          bUpdateConverter,
    tsp00_Int4&         numPages)

{
    SAPDBTRACE_ROUTINE_DEBUG( "bd60Verify", DataTree_Trace, 5 );

    Trans_Context&  trans = Trans_Context::GetContext( t );

    boolean             bFileWasBad = false;
    tbd_current_tree    current;

    numPages     = 1;
    bd30GetTree( trans.OldTrans(), fileId, current, m_verify, ! LOCK_TREE_EXCL_BD00, ! SYNC_TREE_LOCK_BD00 );
    if( trans.IsOk())
    {
        bFileWasBad = current.currRootNptrs_bd00.np_ptr()->nd_file_state().includes( f_bad );
        if( bUpdateConverter ){
            Converter_IVerify::Instance().MarkPageNoAsUsed( trans.Task(), fileId.fileRoot_gg00(), Data_Dynamic );
        }
        if(( trans.IsOk()) && (current.currRootNptrs_bd00.np_ptr()->nd_level() > LEAF_LEVEL_BD00 )){
            bd60_Check( trans, current, current.currRootNptrs_bd00, bUpdateConverter, numPages, false );
        }
    }
    bd30ReleaseTree( current );
    if( trans.IsOk() && bFileWasBad )
    {
        fileId = current.curr_tree_id;
        bd30ResetBadFile( trans.OldTrans(), fileId );
    }
}

/*---------------------------------------------------------------------------*/

externC void
bd60Extract(
    Trans_Context&      trans,
    tbd_current_tree&   current,
    tsp00_VFilename&    hostFileName )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd60Extract", DataTree_Trace, 5 );

    tsp00_PageNo nextPageNo      = current.curr_tree_id.fileRoot_gg00();
    tsp00_PageNo leftMostPageNo  = NIL_PAGE_NO_GG00;

    Kernel_FileIO   hostFile( hostFileName );

    if( hostFile.ErrorOccured()){
        trans.SetErrorCode( e_hostfile_error );
        return;
    }
    Data_BasePage   dumpPage;
    tbd_node_ptrs   pNodes;
    pNodes.np_ptr() = 0;

    while(( trans.IsOk()) && ( nextPageNo != NIL_PAGE_NO_GG00 ))
    {
        bd13GetNode( current, nextPageNo, plmLock_ebd00, nr_for_read, pNodes );
        if( trans.IsOk())
        {
            dumpPage.AssignNodePtr( pNodes.np_ptr());
            if( hostFile.Write( dumpPage )){
                trans.SetErrorCode( e_hostfile_error );
            }
            else
            {
                const tsp00_Int4 level = pNodes.np_ptr()->nd_level();
                nextPageNo             = pNodes.np_ptr()->nd_right();
                if(( level != LEAF_LEVEL_BD00 ) && ( leftMostPageNo == NIL_PAGE_NO_GG00 )){
                    leftMostPageNo = s20buf_to_int4( pNodes.np_ptr(), BODY_BEG_BD00 );
                }
                if(( nextPageNo == NIL_PAGE_NO_GG00 ) && ( leftMostPageNo != NIL_PAGE_NO_GG00 ))
                {
                    nextPageNo     = leftMostPageNo;
                    leftMostPageNo = NIL_PAGE_NO_GG00;
                }
            }
        }
        if( pNodes.np_ptr() != 0 ){
            b13r_release_node( pNodes, current, lru_normal );
        }
    }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void
bd60_Check(
    Trans_Context&          trans,
    tbd_current_tree&       current,
    const tbd_node_ptrs&    pParentNode,
    const bool              bUpdateConverter,
    tsp00_Int4&             numPages,
    const bool              bUpdateLeaves )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd60_Check", DataTree_Trace, 6 );

    const tsp00_Int4 LAST_VALID_PAGENO_POS_BD60 = MAX_BOTTOM_BD00 - 4;

    SAPDBERR_ASSERT_STATE( pParentNode.np_ptr()->nd_level() == FIRST_INDEX_LEVEL_BD00 ||
                           pParentNode.np_ptr()->nd_level() == SECOND_INDEX_LEVEL_BD00 );

    tsp00_Int4          childPageNoPos  = BODY_BEG_BD00;
    const tgg00_PagePos parentBottom    = pParentNode.np_ptr()->nd_bottom();
    const tsp00_Int4    parentLevel     = pParentNode.np_ptr()->nd_level();
    tsp00_PageNo        prevChildPageNo = NIL_PAGE_NO_GG00;

    tbd_node_ptrs pChildNode;
    pChildNode.np_ptr() = 0;

    while(( trans.IsOk()) && ( childPageNoPos < parentBottom ))
    {
        bool  bPageUpdated = false;
        const tsp00_PageNo childPageNo = s20buf_to_int4( pParentNode.np_ptr(), childPageNoPos );
        const tsp00_PageNo nextChildPageNo = ( childPageNoPos + (tsp00_Int4)sizeof( tsp00_PageNo ) < parentBottom ) ?
                                             ( s20buf_to_int4( pParentNode.np_ptr(), childPageNoPos + sizeof( tsp00_PageNo ))) : NIL_PAGE_NO_GG00;

        if(( parentLevel == SECOND_INDEX_LEVEL_BD00 ) && ( nextChildPageNo != NIL_PAGE_NO_GG00 )){
            /* possible corrupted nd_bottom of childPage see PTS 1135692 TS 2005-05-20 */
            bd13GetNode( current, childPageNo, plmLock_ebd00, nr_for_update, pChildNode );
        }
        else{
            bd13GetNode( current, childPageNo, plmLock_ebd00, nr_for_read, pChildNode );
        }
        if( trans.IsOk())
        {
            ++numPages;
            if( bUpdateConverter ){
                Converter_IVerify::Instance().MarkPageNoAsUsed( trans.Task(), childPageNo, Data_Dynamic );
            }
            if( trans.IsOk())
            {
                if( pChildNode.np_ptr()->nd_level() == FIRST_INDEX_LEVEL_BD00 )
                {
                    if(
                        ( pChildNode.np_ptr()->nd_bottom() == LAST_VALID_PAGENO_POS_BD60 ) &&
                        ( nextChildPageNo != NIL_PAGE_NO_GG00                           ))
                    {
                        bPageUpdated                     = true;
                        pChildNode.np_ptr()->nd_bottom() = MAX_BOTTOM_BD00;
                    }
                    if(
                        ( pChildNode.np_ptr()->nd_right() == NIL_PAGE_NO_GG00 ) &&
                        ( nextChildPageNo != NIL_PAGE_NO_GG00                ))
                    {
                        bPageUpdated                    = true;
                        pChildNode.np_ptr()->nd_right() = nextChildPageNo;
                    }
                }
                if( pChildNode.np_ptr()->nd_level() > LEAF_LEVEL_BD00 ){
                    bd60_Check( trans, current, pChildNode, bUpdateConverter, numPages, bUpdateLeaves );
                }
                else
                {
                    if(
                        (( nextChildPageNo != NIL_PAGE_NO_GG00 )&&( nextChildPageNo != pChildNode.np_ptr()->nd_right()))
                        ||
                        (( prevChildPageNo != NIL_PAGE_NO_GG00 )&&( prevChildPageNo != pChildNode.np_ptr()->nd_left()))
                    )
                    {
                        trans.SetErrorCode( e_invalid_index_structure );
                        g01opmsg( sp3p_knldiag, sp3m_error, BD60_INVALID_INDEX_STRUCTURE_SP03,
                            csp3_n_btree, "Parent data pageNo      ", pParentNode.np_ptr()->nd_id());
                        g01opmsg( sp3p_knldiag, sp3m_error, BD60_INVALID_INDEX_STRUCTURE_SP03,
                           csp3_n_btree, "Child data pageNo       ", pChildNode.np_ptr()->nd_id());

                        for( tsp00_Int4 pageCount = 0; pageCount < 2; ++pageCount )
                        {
                            Data_BasePage   dumpPage;
                            if( 0 == pageCount ){
                                dumpPage.AssignNodePtr( pParentNode.np_ptr());
                            }
                            else{
                                dumpPage.AssignNodePtr( pChildNode.np_ptr());
                            }
                            Kernel_FileIO	dumpFile( KERNEL_DATA_FILE,	KERNEL_COR_FILE_EXTENSION,
                                                    SAPDB_ToString( dumpPage.PageNo(), _T_d ));
                            dumpFile.Write(	dumpPage );
                        }
                    }
                }
            }
        }
        if( pChildNode.np_ptr() != 0 )
        {
            if( bPageUpdated || bUpdateLeaves ){
                b13w_release_node( pChildNode, current );
            }
            else{
                b13r_release_node( pChildNode, current, lru_mid );
            }
        }
        childPageNoPos  += sizeof( tsp00_PageNo );
        prevChildPageNo = childPageNo;

        if( ! trans.IsOk()){
            break;
        }
        if( trans.Task().CancelPending())
        {
            trans.SetErrorCode( e_cancelled );
            RTE_Message( Msg_List( Msg_List::Info, SDBMSG_DATA_CHECK_LOB_CANCELED ));
        }
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/

