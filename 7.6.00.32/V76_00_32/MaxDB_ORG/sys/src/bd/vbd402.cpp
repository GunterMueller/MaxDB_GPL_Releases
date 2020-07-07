/*!
  @file           vbd402.cpp
  @author         AlexanderK
  @author         UweH
  @author         TorstenS
  @ingroup        Index
  @brief          Selects

    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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
*/
//   contents of file
#include "gbd600.h"   //   class Node
#include "gbd300.h"  //   Index Current 
#include "gbd450.h"  //   InvListArray - handle
#include "gbd460.h"  //   InvListArrayIterator 
#include "gbd500.h"  //   Tree
#include "gbd510.h"  //   RecordIterator
#include "gsp03_3.h"

#include "hbd02.h"
#include "hbd402.h"
#include "hkb53.h"
#include "hkb71.h"

#include "SAPDBCommon/SAPDB_RangeCode.hpp"
#include "ConsistentRead/CnsRead_Manager.hpp"
#include "DataAccess/Data_Types.hpp"
#include "DataAccess/Data_IOBuffer.hpp"

#if COMPILEMODE_MEO00 >= SLOW_MEO00 
#include "hta99.h"
#endif


#define FILLCHAR      '\0'
#define IGNORE_VWAIT true

/*---------------------------------------------------------------------------*/

inline static void 
bd402LkeyAssign ( 
                 tsp00_Int4        SourceKeyLen,
                 tsp00_KeyPtr      pSourceKey,
                 tgg00_Lkey       &TargetLKey,
                 tgg00_BasisError &BasisError)
{
    TargetLKey.keyLen_gg00() = SourceKeyLen;

    SAPDB_RangeMove( __FILE__,   1,
              sizeof (tsp00_Key), sizeof (TargetLKey.keyVal_gg00 ()),
              pSourceKey,               POS_OFF_DIFF_BD00,
              TargetLKey.keyVal_gg00 (), POS_OFF_DIFF_BD00, 
              SourceKeyLen , BasisError);
}

/*---------------------------------------------------------------------------*/

inline static void
bd402_KeyAssign(
                const tsp00_KeyPtr  pSourceKey,
                const tsp00_Int4    SourceKeyLen,
                const tsp00_KeyPtr  pTargetKey,
                tsp00_Int2          &TargetKeyLen,
                tgg00_BasisError    &BasisError )
{
    TargetKeyLen = SourceKeyLen;
    
    SAPDB_RangeMove( __FILE__, 2,
        SourceKeyLen, sizeof( tsp00_Key ),
        pSourceKey, POS_OFF_DIFF_BD00, pTargetKey, POS_OFF_DIFF_BD00,  
        SourceKeyLen, BasisError );
}

/*---------------------------------------------------------------------------*/

inline static void
bd402_RemoveCommittedReadShareLock( 
    tgg00_TransContext& trans,
    tgg00_FileId&       primFileId,
    const tsp00_KeyPtr  pLockKey,
    tsp00_Int2          lockKeyLen,
    bool&               bDoRemoveLock )
{
    const tgg00_BasisError auxError = trans.trError_gg00;
    trans.trError_gg00              = e_ok;
    #if COMPILEMODE_MEO00 >= SLOW_MEO00      
    t01sname   (bd_inv, "unlock key1:");
    t01moveobj (bd_inv, pLockKey,  1, lockKeyLen);
    #endif              
    k53key_unlock2( trans, lckRowShare_egg00, primFileId.fileTabId_gg00(), pLockKey, lockKeyLen );
    trans.trError_gg00 = auxError;
    bDoRemoveLock      = false;
}
    
/*---------------------------------------------------------------------------*/

inline static tgg00_BasisError
bd402_ReturnCodeHandling(
    const tgg00_MessType2&  messType2,
    const bool              bAscendingKeyOrder,
    const bool              bIsTempIndex )
{
    if ( mm_direct == messType2 )
    {
        if( bIsTempIndex ){
            return e_key_not_found;
        }
        return e_inv_list_not_found;
    }
    if( bAscendingKeyOrder ){
        return e_no_next_record;
    }
    return e_no_prev_record;
}

/*---------------------------------------------------------------------------*/

inline static void
bd402_InterpretMessageTypeForSearch (
    const tgg00_MessType2 &MessType2,
    bool                  &bAscendingKeyOrder,
    bool                  &bIncludeFirstPrimKey)
{
    ROUTINE_DBG_MEO00 ("bd402_InterpretMessageTyp2ForSearch");

    switch (MessType2)
    {
    case mm_direct:
        bAscendingKeyOrder   = true; // in principle this does not matter
        bIncludeFirstPrimKey = true;
        break;

    case mm_first :
        bAscendingKeyOrder   = true;
        bIncludeFirstPrimKey = true;
        break;

    case mm_last:
        bAscendingKeyOrder   = false;
        bIncludeFirstPrimKey = true;
        break;

    case mm_prev:
        bAscendingKeyOrder   = false;
        bIncludeFirstPrimKey = false;
        break;

    case mm_next:
        bAscendingKeyOrder   = true;
        bIncludeFirstPrimKey = false;
        break;
    default:
        break;
    }

#    if COMPILEMODE_MEO00 >= SLOW_MEO00
    t01mess2type (bd_inv, "MessType2   ", MessType2);
    t01bool      (bd_inv, "Ascending   ", bAscendingKeyOrder);
    t01bool      (bd_inv, "IncludeFirst", bIncludeFirstPrimKey);
#    endif
}

/*---------------------------------------------------------------------------*/


static void
bd402_ProcessOneRecordFromIndexFile (
    tgg00_TransContext       &Trans,
    tgg00_FileId             &PrimFileId,
    const tsp00_KeyPtr        pSecKey,
    const tsp00_Int2          SecKeyLen,
    tsp00_KeyPtr              pPrimKey,
    tsp00_Int2                PrimKeyLen,
    tsp00_Int4                RecBufSize,
    tsp00_Int4               &RecBufPos,
    tsp00_MoveObjPtr          pRecBuf,
    tgg00_SelectFieldsParam  &SelectParam,
    tgg00_StackDesc          &StackDesc,
    tgg00_LockReqMode        &GrantedLock,
    tsp00_Int4               &CurResultCnt,
    bool                     &bPrimaryFileErr)
{
    ROUTINE_DBG_MEO00 ("bd402_ProcessOneRecordFromIndexFile");

    tgg00_BasisError &TrError      = Trans.trError_gg00;
    tsp00_Bool        bUnqualified = false;
    tsp00_Int4        CurResLength;


    if ((inv_only == SelectParam.sfp_bd_inv_only()) || (inv_and_primary  == SelectParam.sfp_bd_inv_only()))
    {
#       if COMPILEMODE_MEO00 >= SLOW_MEO00      
        t01sname   (bd_inv, "sec key:    ");
        t01moveobj (bd_inv, pSecKey,  1, SecKeyLen);
        t01sname   (bd_inv, "prim key:   ");
        t01moveobj (bd_inv, pPrimKey,  1, PrimKeyLen);
#       endif              

        k71inv_col_select (Trans, SelectParam, StackDesc, pSecKey, SecKeyLen, pPrimKey, PrimKeyLen, bUnqualified);

        if (bUnqualified && mm_direct != SelectParam.sfp_bd_mess2_type())
            TrError = e_ok;

        if (e_ok != TrError)
            return;

        if  (!bUnqualified && (inv_only == SelectParam.sfp_bd_inv_only()))
        {
            PrimFileId.fileHandling_gg00().addElement(hsNoWaitForInserts_egg00);
            k53row_lock_by_key (Trans, PrimFileId, pPrimKey, PrimKeyLen, SelectParam.sfp_bd_mess_type(),
                                SelectParam.sfp_act_cntresult(), GrantedLock);

            if ((e_lock_collision == TrError) && /* PTS 1108174 */
                    (PrimFileId.fileHandling_gg00().includes(hsIgnoreLock_egg00)))
            {
                TrError = e_ok;
                return;
            }
            if (e_lock_collision_with_inserted_row == TrError)
            {
                // skip current record because it was just inserted
                bUnqualified = true;
                TrError      = e_ok;
            }
            else
            {
                if (e_ok != TrError){
                    return;
                }
                if ( 0 != pRecBuf )
                {
                    if (  RecBufPos + (PrimKeyLen+cgg_rec_key_offset) - 1 > RecBufSize )
                    {
                        TrError = e_buffer_limit;
                        return;
                    }
                    else
                    {
                        tgg00_LkeyPtr pLkey = (tgg00_LkeyPtr)((tsp00_MoveObjPtr) &pRecBuf[RecBufPos-POS_OFF_DIFF_BD00]); 
                        pLkey->keyRecLenSpace_gg00() = PrimKeyLen + cgg_rec_key_offset;
                        pLkey->keyLen_gg00()         = PrimKeyLen;
                        pLkey->keyVarOffSpace_gg00() = 0;
                        pLkey->keyVarCntSpace_gg00() = 0;
    
                        SAPDB_RangeMove( __FILE__, 3,
                            PrimKeyLen, RecBufSize - RecBufPos + 1,
                            pPrimKey, POS_OFF_DIFF_BD00, 
                            pLkey, POS_OFF_DIFF_BD00 + cgg_rec_key_offset,  
                            PrimKeyLen, TrError );
                        RecBufPos += PrimKeyLen + cgg_rec_key_offset;
                        if ( (RecBufPos % 2) == 1 ){
                            ++RecBufPos;
                        }
                    }
                }
                ++CurResultCnt;
    
                if  (( 0 != SelectParam.sfp_m_firstkeys_addr()) && (1 == CurResultCnt))
                {
                    // save key for scrollable cursor
                    bd402LkeyAssign (SecKeyLen,  pSecKey,  SelectParam.sfp_m_firstkeys_addr()->listkey, TrError);
                    bd402LkeyAssign (PrimKeyLen, pPrimKey, SelectParam.sfp_m_firstkeys_addr()->reckey,  TrError);
                }
    
                if  (SelectParam.sfp_resrec_maxlen() < MAX_INT2_SP00){
                    CurResLength = SelectParam.sfp_resrec_maxlen();
                }
                else{
                    CurResLength = SelectParam.sfp_result_length();
                }
                if (m_fetch != SelectParam.sfp_bd_mess_type() && (CurResLength%2) == 1){
                    ++CurResLength;
                }
    #           if COMPILEMODE_MEO00 >= SLOW_MEO00      
                if  (CurResLength > SelectParam.sfp_result_length())
                {
                    SAPDB_RangeFill( __FILE__, 3,
                            SelectParam.sfp_m_result_size(),
                            *SelectParam.sfp_m_result_addr(),
                            SelectParam.sfp_result_length() + 1,
                            CurResLength - SelectParam.sfp_result_length(),
                            FILLCHAR, TrError);
                    if (e_ok != TrError){
                        return;
                    }
                }
    #           endif
    
                SelectParam.sfp_m_result_len()  += CurResLength;
                SelectParam.sfp_m_result_addr()  = SelectParam.sfp_m_result_addr() + CurResLength;
                SelectParam.sfp_m_result_size() -= CurResLength;
                ++SelectParam.sfp_act_cntresult();
            }
        }
    }

    PrimFileId.fileHandling_gg00().delElement(hsNoWaitForInserts_egg00);

    if (bUnqualified)
    {
        if (mm_direct != SelectParam.sfp_bd_mess2_type())
            TrError = e_ok;
    }
    else
    {
        if ((primary_only == SelectParam.sfp_bd_inv_only()) || (inv_and_primary == SelectParam.sfp_bd_inv_only()))
        {
            tgg00_QualKind  QualKindSave;
            tgg00_MessType2 MessType2Save;
            const pasbool   ClusteredIOSave = SelectParam.sfp_cluster_info().clust_bDoClusteredRead_gg00;
            
            QualKindSave.becomes(SelectParam.sfp_bd_inv_only());
            MessType2Save.becomes(SelectParam.sfp_bd_mess2_type());

            SelectParam.sfp_cluster_info().clust_bDoClusteredRead_gg00 = false;

            tsp00_Int4        ResultCntSave     = SelectParam.sfp_m_result_cnt();
			pasbool           FoundOneResult    = true;
            tgg00_TwoKeysPtr  FirstKeysAddrSave = SelectParam.sfp_m_firstkeys_addr();

            SelectParam.sfp_bd_inv_only().becomes(primary_only);
            SelectParam.sfp_bd_mess2_type().becomes(mm_direct); // search prim key from inv list direct in primary file
            SelectParam.sfp_m_result_cnt()     = 1;     // only one record is requested
            SelectParam.sfp_m_firstkeys_addr() = NULL;

            if (0 != pRecBuf)
            {

                b02kb_select_rec (Trans, PrimFileId,
                                  pPrimKey, PrimKeyLen, pPrimKey, PrimKeyLen,
                                  RecBufSize - RecBufPos + 1,
                                  (tsp00_MoveObjPtr) &pRecBuf[RecBufPos-POS_OFF_DIFF_BD00],
                                  IGNORE_VWAIT, SelectParam,
                                  StackDesc, bUnqualified,
                                  GrantedLock);
            }
            else
            {
                b02kb_select_rec (Trans, PrimFileId,
                                  pPrimKey, PrimKeyLen, pPrimKey, PrimKeyLen,
                                  0, NULL, IGNORE_VWAIT, SelectParam,
                                  StackDesc, bUnqualified,
                                  GrantedLock);
            }

            SelectParam.sfp_bd_inv_only().becomes(QualKindSave);
            SelectParam.sfp_bd_mess2_type().becomes(MessType2Save);
			FoundOneResult = (1 == SelectParam.sfp_m_result_cnt());
            SelectParam.sfp_m_result_cnt()     = ResultCntSave;
            SelectParam.sfp_m_firstkeys_addr() = FirstKeysAddrSave;
            SelectParam.sfp_cluster_info().clust_bDoClusteredRead_gg00 = ClusteredIOSave;

            if (e_key_not_found == TrError){
                bUnqualified = true;
            }
            if (bUnqualified && mm_direct != SelectParam.sfp_bd_mess2_type()){
                TrError = e_ok;
            }
            if (e_ok != TrError)
            {
                bPrimaryFileErr = true;
                return;
            }
            if(! bUnqualified )
            {
				if (FoundOneResult) {
					++CurResultCnt;
				}

                if(( 1 == CurResultCnt) && ( NULL != SelectParam.sfp_m_firstkeys_addr() ))
                {
                    // save key for scrollable cursor
                    bd402LkeyAssign (SecKeyLen,  pSecKey,  SelectParam.sfp_m_firstkeys_addr()->listkey, TrError);
                    bd402LkeyAssign (PrimKeyLen, pPrimKey, SelectParam.sfp_m_firstkeys_addr()->reckey,  TrError);
                }
            }
        }
    }
}

/*---------------------------------------------------------------------------*/

static void
bd402_SearchOneInvListForQualifiedRecords (
    cbd300_InvCurrent        &InvCurrent,
    tgg00_FileId             &PrimFileId,
    tsp00_KeyPtr             &pStartPrimKey,          //[out=last prim key before search was interupted]
    tsp00_Int2               &StartPrimKeyLen,
    cbd460_InvListIterator   &KeyIter,
    tsp00_Int4                RecBufSize,
    tsp00_Int4               &RecBufPos,
    tsp00_MoveObjPtr          pRecBuf,
    tgg00_SelectFieldsParam  &SelectParam,
    tgg00_StackDesc          &StackDesc,
    tgg00_LockReqMode        &GrantedLock,
    tsp00_Int4               &CurResultCnt,
    tsp00_Int4                LastResultLen,
    const tsp00_Int4          InitialResultLen,
    bool                     &bPrimaryFileErr,
    bool                     &bDoRemoveLock)
{
    ROUTINE_DBG_MEO00 ("bd402_SearchOneInvListForQualifiedRecords");

    tgg00_BasisError    &TrError = InvCurrent.curr_trans->trError_gg00;
   
    tsp00_KeyPtr    pCurrPrimKey;
    tsp00_Int2      currPrimKeyLen;

    tsp00_KeyPtr    pCurrSecKey;    // PTS 1116651 TS 2002-07-16
    tsp00_Int2      currSecKeyLen;  // PTS 1116651 TS 2002-07-16

    while (e_ok == TrError && CurResultCnt < SelectParam.sfp_m_result_cnt())
    {
        bool hasDeletedFlag;
        KeyIter.bd460Deref (pCurrPrimKey, currPrimKeyLen, hasDeletedFlag);

       if( 0 == pCurrPrimKey )
        {
            if( bDoRemoveLock )
            {   // Remove row share lock of committed read, even if the locked primary key
                // and all succeeding keys of the same inversion list are gone.
                bd402_RemoveCommittedReadShareLock( *(InvCurrent.curr_trans), PrimFileId, 
                    pStartPrimKey, StartPrimKeyLen, bDoRemoveLock );
            }
            return;
        }
        KeyIter.bd460GetSecKey( pCurrSecKey, currSecKeyLen ); // PTS 1116651 TS 2002-07-16

        bd402_ProcessOneRecordFromIndexFile (*InvCurrent.curr_trans, PrimFileId, pCurrSecKey, currSecKeyLen,
            pCurrPrimKey, currPrimKeyLen, RecBufSize, RecBufPos, pRecBuf, SelectParam,
            StackDesc, GrantedLock, CurResultCnt, bPrimaryFileErr);

        if( bDoRemoveLock )
        {
            // Remove row share lock of committed read which was requested by a
            // previous call. pStartPrimKey contains the requested key. Pay attention 
            // to the fact that the lock key could be removed already by the former 
            // owner of the lock.
            bd402_RemoveCommittedReadShareLock( *(InvCurrent.curr_trans), PrimFileId, 
                    pStartPrimKey, StartPrimKeyLen, bDoRemoveLock );
        }
#       if COMPILEMODE_MEO00 >= SLOW_MEO00      
        t01int4 (bd_inv, "CurResultCnt", CurResultCnt);
#       endif

        switch (TrError)
        {
        case e_ok:
            if (CurResultCnt < SelectParam.sfp_m_result_cnt())
            {
                ++ KeyIter; // PTS 1116651 Note that this is an interruptable iterator !!
                LastResultLen = SelectParam.sfp_result_length();
                SelectParam.sfp_result_length() = InitialResultLen;
            }
            else
            {
                if (mm_direct != SelectParam.sfp_bd_mess2_type())
                {
                    bd402_KeyAssign (pCurrPrimKey, currPrimKeyLen, pStartPrimKey, StartPrimKeyLen, TrError);

                    switch (SelectParam.sfp_bd_mess2_type())
                    {
                    case mm_first: SelectParam.sfp_bd_mess2_type().becomes(mm_next);break;
                    case mm_last : SelectParam.sfp_bd_mess2_type().becomes(mm_prev);break;
                    case mm_next : break;
                    case mm_prev : break;
                    default      : TrError = e_not_implemented; break;
                    }
                }
            }
            break;
        case e_wait_for_lock_release:
            TrError = e_ok;
            bd402_KeyAssign (pCurrPrimKey, currPrimKeyLen, pStartPrimKey, StartPrimKeyLen, TrError);

            if (e_ok == TrError){
                TrError = e_wait_for_lock_release;
            }
            // In isolation level 1 (committed read) a row share lock is requested if an 
            // exclusive row lock exist. After the share lock is available and the record 
            // is read the lock must be removed! Do not wait until the end of the transaction! 
            bDoRemoveLock = PrimFileId.fileHandling_gg00().includes(hsCollisionTest_egg00);
            break;
        case e_buffer_limit: // PTS 1120887 TS 2003-02-28
            {
                tgg00_BasisError auxError = e_ok;

                // Save the prim key to guarantee that the next call starts with the correct primKey.
                // Note that the sec key has to be saved by the "caller"
                bd402_KeyAssign (pCurrPrimKey, currPrimKeyLen, pStartPrimKey, StartPrimKeyLen, auxError);

                if(( e_ok == auxError ) && ( 0 < CurResultCnt ))
                {
                    // The given buffer is too small to store all result rows, but
                    // it contains at least one result from a previous secKey resp,
                    // primKey. The requested result count has to be decreased to
                    // the current row count.

                    SelectParam.sfp_m_result_cnt() = CurResultCnt;
                }
                if( e_ok != auxError){
                    TrError = auxError;
                }
                break;
            }
        default:
            break;
        }
    }
}

/*---------------------------------------------------------------------------*/

static void
bd402_ProcessOneRecordFromResultFile (
    tgg00_TransContext       &Trans,
    tgg00_FileId             &PrimFileId,
    tgg00_RecPtr              pRec,
    tsp00_Int4                RecBufSize,
    tsp00_Int4               &RecBufPos,
    tsp00_MoveObjPtr          pRecBuf,
    tgg00_SelectFieldsParam  &SelectParam,
    tgg00_StackDesc          &StackDesc,
    tgg00_LockReqMode        &GrantedLock,
    tsp00_Int4               &CurResultCnt)
{
    ROUTINE_DBG_MEO00 ("bd402_ProcessOneRecordFromResultFile");

    tgg00_BasisError &TrError      = Trans.trError_gg00;
    tsp00_Bool        bUnqualified = false;
    tsp00_KeyPtr      pRecKey      = tsp00_KeyPtr(&pRec->recKey_gg00().keyVal_gg00());
    tsp00_Int2       &RecKeyLen    = pRec->recKey_gg00().keyLen_gg00();

    tgg00_MessType2 MessType2Save;
    MessType2Save.becomes(SelectParam.sfp_bd_mess2_type());
    tsp00_Int4        ResultCntSave     = SelectParam.sfp_m_result_cnt();
    tgg00_TwoKeysPtr  FirstKeysAddrSave = SelectParam.sfp_m_firstkeys_addr();
    const pasbool     ClusteredIOSave   = SelectParam.sfp_cluster_info().clust_bDoClusteredRead_gg00;

    SelectParam.sfp_bd_inv_only().becomes( primary_only );
    SelectParam.sfp_bd_mess2_type().becomes( mm_direct ); // direct access for one record
    SelectParam.sfp_m_result_cnt()     = 1;
    SelectParam.sfp_m_firstkeys_addr() = NULL;
    SelectParam.sfp_cluster_info().clust_bDoClusteredRead_gg00 = false;

    if (0 != pRecBuf)
    {
        b02kb_select_rec (Trans, PrimFileId,
                          pRecKey, RecKeyLen, pRecKey, RecKeyLen,
                          RecBufSize - RecBufPos + 1,
                          (tsp00_MoveObjPtr) &pRecBuf[RecBufPos-POS_OFF_DIFF_BD00],
                          IGNORE_VWAIT, SelectParam,
                          StackDesc, bUnqualified,
                          GrantedLock);
    }
    else
    {
        b02kb_select_rec (Trans, PrimFileId,
                          pRecKey, RecKeyLen, pRecKey, RecKeyLen,
                          0, NULL, IGNORE_VWAIT, SelectParam,
                          StackDesc, bUnqualified,
                          GrantedLock);
    }
    SelectParam.sfp_bd_mess2_type().becomes(MessType2Save);
    SelectParam.sfp_m_result_cnt()                             = ResultCntSave;
    SelectParam.sfp_m_firstkeys_addr()                         = FirstKeysAddrSave;
    SelectParam.sfp_cluster_info().clust_bDoClusteredRead_gg00 = ClusteredIOSave;

    if (e_key_not_found == TrError)
        bUnqualified = true;

    if (bUnqualified && mm_direct != SelectParam.sfp_bd_mess2_type())
        TrError = e_ok;

    if  (!bUnqualified && e_ok == TrError)
    {
        ++CurResultCnt;

        if ((0 != SelectParam.sfp_m_firstkeys_addr()) && (1 == CurResultCnt))
        {   // save key for scrollable cursor
            // unterschiedlich!!!
            bd402LkeyAssign (RecKeyLen, pRecKey, SelectParam.sfp_m_firstkeys_addr()->reckey,  TrError);
        }
    }
}

/*---------------------------------------------------------------------------*/

inline static void
bd402_PrepareNextLoop (tgg00_BasisError         &TrError,
                       tsp00_KeyPtr              pPrimKey,
                       tsp00_Int4                PrimKeyLen,
                       tsp00_KeyPtr              pStartPrimKey,
                       tsp00_Int2               &StartPrimKeyLen,
                       tgg00_SelectFieldsParam  &SelectParam,
                       tsp00_Int4               &CurResultCnt)
{
    /* in case of the error e_buffer_limit this function sets the message type 2    */
    /* and the StartPrimkey so that the function b03select_inv can be called again  */
    /* and continues exactly where it stopped when the error e_buffer_limit occured */

    /* current PrimKey becomes Startkey  */

    ROUTINE_DBG_MEO00 ("bd402_PrepareNextLoop");

    TrError = e_ok;
    bd402_KeyAssign (pPrimKey, PrimKeyLen, pStartPrimKey, StartPrimKeyLen, TrError);

    if (e_ok == TrError)
    {
        /* set mesage type that the current prim key */
        /* is used as startkey in the next loop      */
        if  (CurResultCnt > 0)
        {
            SelectParam.sfp_m_result_cnt() = CurResultCnt;
            switch (SelectParam.sfp_bd_mess2_type())
            {
            case mm_next : SelectParam.sfp_bd_mess2_type().becomes(mm_first); break;
            case mm_prev : SelectParam.sfp_bd_mess2_type().becomes(mm_last); break;
            case mm_first: break;
            case mm_last : break;
            default:
                TrError = e_not_implemented;
                break;
            }
        }
    }
}

/*---------------------------------------------------------------------------*/

externCpp void
bd402NextFromInvTree (cbd300_InvCurrent   &InvCurrent,
                      bool                 bIncludeSecKey,
                      tsp00_KeyPtr         pSecKey,
                      tsp00_Int2          &SecKeyLen)
{
    ROUTINE_DBG_MEO00 ("bd402NextFromInvTree");

    tgg00_BasisError &TrError = InvCurrent.curr_trans->trError_gg00;

    const bool          bAscendingKeyOrder = true;
    cbd600_Node         LeafNode (InvCurrent, tbd_node_request::fromConst(nr_for_read));
    cbd500_Tree Tree (LeafNode);
    bool                bDummy;

    if ( e_ok != TrError ) return;

//    Tree.bd500FindNode (pSecKey, SecKeyLen, LEAF_LEVEL_BD00);
//    if ( e_ok != TrError ) return;

    cbd510_RecordIterator RecIter (Tree, pSecKey, SecKeyLen, NULL, 0,
                                   bAscendingKeyOrder, bIncludeSecKey, bDummy);

    tsp00_KeyPtr  pKey;
    tsp00_Int2    keyLength;
    
    if ( RecIter.bd510GetKey(pKey, keyLength) )
        bd402_KeyAssign (pKey, keyLength, pSecKey, SecKeyLen, TrError);
    else
        TrError = bIncludeSecKey ? e_inv_list_not_found : e_no_next_invkey;
}
/*
  +++++++
     tgg00_RecPtr  pRec = *RecIter;

     if ( NULL == pRec )
        TrError = bIncludeSecKey ? e_inv_list_not_found : e_no_next_invkey;
     else
         bd402_KeyAssign ((tsp00_BytePtr) &pRec->recKey_gg00().keyVal_gg00(),
                        pRec->recKey_gg00().keyLen_gg00(), pSecKey, SecKeyLen, TrError);
*/
/*---------------------------------------------------------------------------*/

externCpp void
bd402SearchIndexForQualifiedRecords (
    cbd300_InvCurrent&          InvCurrent,
    tgg00_FileId&               PrimFileId,
    tsp00_KeyPtr                pCurrSecKey,
    tsp00_Int2&                 currSecKeyLen,
    const tsp00_KeyPtr          pStopSecKey,
    const tsp00_Int4            stopSecKeyLen,
    const tsp00_KeyPtr          pFrozenStartPrimKey,
    const tsp00_Int4            frozenStartPrimKeyLen,
    const tsp00_KeyPtr          pStopPrimKey,
    const tsp00_Int4            stopPrimKeyLen,
    tsp00_KeyPtr                pCurrPrimKey,
    tsp00_Int2&                 currPrimKeyLen,
    const tsp00_Int4            RecBufSize,
    tsp00_MoveObjPtr            pRecBuf,
    tgg00_SelectFieldsParam&    SelectParam,
    tgg00_StackDesc&            StackDesc,
    tgg00_LockReqMode&          GrantedLock,
    bool&                       bPrimaryFileErr)
{
    ROUTINE_DBG_MEO00 ("bd402SearchIndexForQualifiedRecords");

    tgg00_BasisError     &TrError         = InvCurrent.curr_trans->trError_gg00;
    const bool bIncludeFirstSecKey        = true;
    bool       bIncludeFirstPrimKey       = false; // see annotation below!
    bool       bAscendingKeyOrder         = true;
    bool       bContinueWorkOnCurrInvList = true;  
    bool       bDoRemoveLock              = false; // PTS 1117878 UH 2002-10-17
    

    tsp00_Int4 RecBufPos                       = 1;
    tsp00_Int4 CurResultCnt                    = 0;
    tsp00_Int4         LastResultLen           = SelectParam.sfp_result_length();
    const tsp00_Int4   InitialResultLen        = SelectParam.sfp_result_length();
    bool               IndexLeafCachingEnabled; // PTS 1136998 UH 2006-01-23
    tbd_node           pageCache; // PTS 1136998 UH 2006-01-23
    Data_IOBuffer      ioBuffer;
     
    /* the search is stopped after SelectParam.sfp_m_result_cnt() have been found */
    if (SelectParam.sfp_m_result_cnt() <= 0)
        SelectParam.sfp_m_result_cnt() = 1;

    /* decide if the first key is to include and about direction of search */
    bd402_InterpretMessageTypeForSearch (SelectParam.sfp_bd_mess2_type(),
                                         bAscendingKeyOrder, bIncludeFirstPrimKey);

    const bool bClusteredIOWanted = SelectParam.sfp_cluster_info().clust_bDoClusteredRead_gg00
                                    &&
                                    ! InvCurrent.curr_tree_id.fileType_gg00().includes(ftsTemp_egg00)
                                    &&
                                    ioBuffer.Initialize (SelectParam.sfp_cluster_info().clust_BufferSize_gg00,
                                                         (SAPDB_Byte*)SelectParam.sfp_cluster_info().clust_pIOBuffer_gg00);
    
    IndexLeafCachingEnabled = ! bClusteredIOWanted; // PTS 1136998 UH 2006-01-23
    
    if ( IndexLeafCachingEnabled ) // PTS 1136998 UH 2006-01-23
    {
        switch ( Data_IndexLeafCaching )
        {
            case 0:
                IndexLeafCachingEnabled = false;
                break;
            case 1:
                IndexLeafCachingEnabled = true;
                break;
            case 2:
            {
                // todo ++++ 
                // what is select fields parameter telling about how large will the intervall be to read 
                IndexLeafCachingEnabled = false; // = SelectParam ....;
            }
        }
    }
    
    // clust_BufferSize_gg00
    
    /* get iterator on records in the index tree */
    cbd600_Node   IndexLeaf ( InvCurrent,
                              tbd_node_request::fromConst(nr_for_read),
                              tbd00_PageLockMode::fromConst(plmLock_ebd00),
                              IndexLeafCachingEnabled ? &pageCache : 0 ); // PTS 1136998 UH 2006-01-23
    cbd500_Tree   Tree      ( IndexLeaf, true, bClusteredIOWanted ? &ioBuffer : 0 );

    if (e_ok != TrError)
        return;

    cbd510_RecordIterator RecIter ( Tree,
                                    pCurrSecKey,        currSecKeyLen,
                                    pStopSecKey,        stopSecKeyLen, 
                                    bAscendingKeyOrder, bIncludeFirstSecKey,
                                    bContinueWorkOnCurrInvList ); 
    if (e_ok != TrError)
        return;

    tgg00_RecPtr pRec = RecIter.bd510GetRecord();

    /* evaluate search result for start key*/

    if( 0 == pRec )
        TrError = bd402_ReturnCodeHandling( SelectParam.sfp_bd_mess2_type(), bAscendingKeyOrder, false );

    // This value is needed for the search of primary keys in an inversion list. We have to 
    // distinguish between the case where an inversion list is used for the first time and
    // a subsequent all of this method where the work on an inversion list is continued.
    // Case one is difficult to detect, but is could be handled like the second case, because
    // the pCurrPrimKey and the pFrozenStartPrimKey are equal.
    //bContinueWorkOnCurrInvList = true;
    
    const bool bConsistentReadSupport = InvCurrent.bd300ConsistentReadSupport();

    while( e_ok == TrError )
    {
        switch (pRec->recInvListKind_gg00())
        {
        case ilkArray_egg00:
            {  
                if( bContinueWorkOnCurrInvList )  
                {   // continue work on the same inversion list. Note that the work on
                    // the inversion list could be interrupted so that a subsequent call
                    // of this method with the same secondary key and the current primary
                    // is necessary
                    cbd460_InvListArrayIterator keyIter ( pRec,
                                                          pCurrPrimKey,       currPrimKeyLen,
                                                          pStopPrimKey,       stopPrimKeyLen,
                                                          bAscendingKeyOrder, bIncludeFirstPrimKey,
                                                          bConsistentReadSupport );

                    bd402_SearchOneInvListForQualifiedRecords ( InvCurrent, PrimFileId, 
                                                                pCurrPrimKey, currPrimKeyLen,
                                                                keyIter,
                                                                RecBufSize, RecBufPos, pRecBuf, 
                                                                SelectParam, StackDesc,
                                                                GrantedLock, CurResultCnt,
                                                                LastResultLen, 
                                                                InitialResultLen,
                                                                bPrimaryFileErr,
                                                                bDoRemoveLock);
                }
                else
                {   // A new inversion list is reached. The first prim key to be searched in
                    // the new list must be the original prim key and not the last prim key 
                    // found in the last inversion list
                    cbd460_InvListArrayIterator  keyIter ( pRec,
                                                           pFrozenStartPrimKey, frozenStartPrimKeyLen,
                                                           pStopPrimKey,        stopPrimKeyLen,
                                                           bAscendingKeyOrder, bIncludeFirstPrimKey,
                                                           bConsistentReadSupport );

                    bd402_SearchOneInvListForQualifiedRecords ( InvCurrent,   PrimFileId, 
                                                                pCurrPrimKey, currPrimKeyLen,
                                                                keyIter,
                                                                RecBufSize, RecBufPos, pRecBuf, 
                                                                SelectParam, StackDesc,
                                                                GrantedLock, CurResultCnt,
                                                                LastResultLen, 
                                                                InitialResultLen,
                                                                bPrimaryFileErr,
                                                                bDoRemoveLock );
                }
            }
            break;

        case ilkSubTree_egg00:
            {   
                cbd450_InvListRefSubTree  InvListRefSubTree (pRec);

                if( bContinueWorkOnCurrInvList)  // continue work on the same inversion list (see above)
                {
                    cbd460_InvListSubtreeIterator  subtreeIterator (Tree, RecIter, InvListRefSubTree, 
                            pCurrPrimKey,  currPrimKeyLen, pStopPrimKey, stopPrimKeyLen,
                            bAscendingKeyOrder, bIncludeFirstPrimKey, bConsistentReadSupport);

                    bd402_SearchOneInvListForQualifiedRecords (InvCurrent, PrimFileId, 
                            pCurrPrimKey, currPrimKeyLen, subtreeIterator, RecBufSize, RecBufPos, pRecBuf, 
                            SelectParam, StackDesc, GrantedLock, CurResultCnt, LastResultLen,
                            InitialResultLen, bPrimaryFileErr, bDoRemoveLock);
                }
                else // continue work on a new inversion list (see above)
                {
                    cbd460_InvListSubtreeIterator subtreeIterator (Tree, RecIter, InvListRefSubTree, 
                            pFrozenStartPrimKey, frozenStartPrimKeyLen, pStopPrimKey , stopPrimKeyLen,
                            bAscendingKeyOrder, bIncludeFirstPrimKey, bConsistentReadSupport);

                    bd402_SearchOneInvListForQualifiedRecords (InvCurrent, PrimFileId, 
                            pCurrPrimKey, currPrimKeyLen, subtreeIterator, RecBufSize, RecBufPos, pRecBuf, 
                            SelectParam, StackDesc, GrantedLock, CurResultCnt, LastResultLen,
                            InitialResultLen, bPrimaryFileErr, bDoRemoveLock);
                }                
            }
            break;

        default:
            TrError = e_invalid_invlistpos;
            IndexLeaf.bd600Dump (bd402c1IllegalRecord_csp03, "bd402SearchIndexForQuali");
            return;
        }
        /* return if expected number of results is found */
        if (CurResultCnt == SelectParam.sfp_m_result_cnt())
        {
            if (mm_direct == SelectParam.sfp_bd_mess2_type()){
                return;
            }
            pRec = RecIter.bd510GetRecord();  // pRec could be invalidated by interruptable iterator PTS 1116651 TS 2002-07-15

            bd402_KeyAssign (tsp00_KeyPtr(&(pRec->recKey_gg00().keyVal_gg00())), pRec->recKeyLen_gg00(),
                       pCurrSecKey, currSecKeyLen, TrError); // PTS 1116651 TS 2002-07-15

            if(( e_buffer_limit == TrError ) && ( 0 < CurResultCnt )) // PTS 1120887 TS 2003-02-28
            {
                TrError = e_ok;

                switch (SelectParam.sfp_bd_mess2_type())
                {
                case mm_next : SelectParam.sfp_bd_mess2_type().becomes(mm_first);  break;
                case mm_prev : SelectParam.sfp_bd_mess2_type().becomes(mm_last);   break;
                default      : break;
                }
            }
            else
            {
                switch (SelectParam.sfp_bd_mess2_type())
                {
                case mm_first: SelectParam.sfp_bd_mess2_type().becomes(mm_next);  break;
                case mm_last : SelectParam.sfp_bd_mess2_type().becomes(mm_prev);  break;
                default      : break;
                }
            }
            /* tell ak to save the new key */
            InvCurrent.curr_trans->trWarning_gg00.addElement (warn0_exist);
            InvCurrent.curr_trans->trWarning_gg00.addElement (warn14_next_invlist);
            return;
        }
        switch ( TrError )
        {
        case e_wait_for_lock_release:
            TrError = e_ok;
            {  
                tsp00_Key           reEntranceSecKey;
                tsp00_Int2          reEntranceSecKeyLen = 0;
                const tsp00_KeyPtr  pReEntranceSecKey   = ( tsp00_KeyPtr ) &reEntranceSecKey;

                RecIter.bd510Break( pReEntranceSecKey, reEntranceSecKeyLen );

                // wait must be called - also in case of an error -, because an sql lock request already exist
                k53wait (*InvCurrent.curr_trans, SelectParam.sfp_bd_mess_type(), SelectParam.sfp_bd_mess2_type());
                if (e_ok != TrError){
                    break;
                }
                RecIter.bd510Continue( pReEntranceSecKey, reEntranceSecKeyLen ); // PTS 1112883 UH 2001-22-29 not pFirstKey

                if( e_ok == TrError ) 
                {
                    pRec = RecIter.bd510GetRecord();
                    if( 0 == pRec )
                        TrError = bd402_ReturnCodeHandling( SelectParam.sfp_bd_mess2_type(), bAscendingKeyOrder, false );
                    else
                    {
                        tsp00_LcompResult compareResult;
    
                        s30cmp( pReEntranceSecKey, POS_OFF_DIFF_BD00, reEntranceSecKeyLen,
                            &(pRec->recKey_gg00().keyVal_gg00()), POS_OFF_DIFF_BD00, pRec->recKeyLen_gg00(), 
                            compareResult );
                        // If the old inversion list is gone after the sql lock is granted we have to 
                        // work on a new inversion list and therefore the primary key must be
                        // reinitialized with the frozen primary key
                        bContinueWorkOnCurrInvList =( l_equal == compareResult );
                        
                        // Look at all primary keys in the new inversion list, because it is not a new 
                        // call of bd402SearchIndex so there is not reason to skip a given primary key
                        // (pCurrPrimKey) which is may be part of the result buffer filled with the 
                        // previous call of bd402SearchIndex
                        bIncludeFirstPrimKey = true;
                        continue;
                    }
                }
                if( e_ok != TrError && bDoRemoveLock )
                { 
                    // Remove row share lock of commited read, in case of abnormal termination of bd402SearchIndex
                    bd402_RemoveCommittedReadShareLock( *(InvCurrent.curr_trans), PrimFileId, 
                        pCurrPrimKey, currPrimKeyLen, bDoRemoveLock );
                }
            }
            break;
        case e_ok:
           ++RecIter;
            if ( e_ok != TrError )
                break;

            /* tell ak to save the new key */
            InvCurrent.curr_trans->trWarning_gg00.addElement (warn0_exist);
            InvCurrent.curr_trans->trWarning_gg00.addElement (warn14_next_invlist);

            pRec = RecIter.bd510GetRecord();
            if( 0 != pRec )
            {
                // processing of old inversion list completed, a new inversion list is used
                bContinueWorkOnCurrInvList = false; 
                // Look at all primary keys in the new inversion list, because it is not a new 
                // call of bd402SearchIndex so there is not reason to skip a given primary key
                // (pCurrPrimKey) which is may be part of the result buffer filled with the 
                // previous call of bd402SearchIndex
                bIncludeFirstPrimKey  = true;
                continue;
            }
            TrError = bd402_ReturnCodeHandling( SelectParam.sfp_bd_mess2_type(), bAscendingKeyOrder, false );
            break;
        default:
            break;
        }
    } /*endwhile*/

    if (CurResultCnt < SelectParam.sfp_m_result_cnt())
    {
        SelectParam.sfp_m_result_cnt()  = CurResultCnt;
        SelectParam.sfp_result_length() = LastResultLen;
    }
}

/*---------------------------------------------------------------------------*/

externCpp void
bd402SearchResultFileForQualifiedRecords (
        cbd300_InvCurrent        &InvCurrent,
        tgg00_FileId             &PrimFileId,
        tsp00_KeyPtr              pStartKey,
        tsp00_Int2               &StartKeyLen,
        tsp00_Int4                RecBufSize,
        tsp00_MoveObjPtr          pRecBuf,
        tgg00_SelectFieldsParam  &SelectParam,
        tgg00_StackDesc          &StackDesc,
        tgg00_LockReqMode        &GrantedLock)
{
    ROUTINE_DBG_MEO00 ("bd402SearchResultFileForQualifiedRecords");

    tgg00_BasisError  &TrError            = InvCurrent.curr_trans->trError_gg00;
    bool               bIncludeFirstKey   = false;
    bool               bAscendingKeyOrder = true;
    tsp00_Int4         RecBufPos          = 1;
    tsp00_Int4         CurResultCnt       = 0;
    tsp00_Int4         LastResultLen      = SelectParam.sfp_result_length();
    const tsp00_Int4   InitialResultLen   = SelectParam.sfp_result_length();
    bool               bDoRemoveLock      = false; // PTS 1117878 UH 2002-10-17

    /* the search is stopped after SelectParam.sfp_m_result_cnt() have been found */
    if (SelectParam.sfp_m_result_cnt() <= 0){
        SelectParam.sfp_m_result_cnt() = 1;
    }

    /* decide if the first key is to include and about direction of search */
    bd402_InterpretMessageTypeForSearch (SelectParam.sfp_bd_mess2_type(),
                                         bAscendingKeyOrder, bIncludeFirstKey);

    /* get iterator on records in the index tree */
    cbd600_Node   TreeLeaf (InvCurrent, tbd_node_request::fromConst(nr_for_read));
    cbd500_Tree   Tree (TreeLeaf, false ); // private file need not to be interuptable
    if (e_ok != TrError){
        return;
    }
    bool    bDummy;
    cbd510_RecordIterator RecIter (Tree, pStartKey, StartKeyLen, NULL, 0,
                                   bAscendingKeyOrder, bIncludeFirstKey, bDummy);
    if (e_ok != TrError){
        return;
    }
    tgg00_RecPtr  pRec = RecIter.bd510GetRecord();

    /* evaluate search result for start key*/
    if( 0 == pRec ){
        TrError = bd402_ReturnCodeHandling( SelectParam.sfp_bd_mess2_type(), bAscendingKeyOrder, true );
    }

    while( e_ok == TrError )
    {
        tsp00_KeyPtr      pCurrKey   = (tsp00_KeyPtr) &(pRec->recKey_gg00().keyVal_gg00());
        const tsp00_Int2 &currKeyLen = pRec->recKey_gg00().keyLen_gg00();

        bd402_ProcessOneRecordFromResultFile (*InvCurrent.curr_trans, PrimFileId, pRec, RecBufSize, RecBufPos,
                                          pRecBuf, SelectParam,  StackDesc, GrantedLock, CurResultCnt);
        if ( bDoRemoveLock )
        {
            // Remove row share lock of committed read which was requested by a
            // previous call. pCurrKey contains the requested key. 
            bd402_RemoveCommittedReadShareLock( *(InvCurrent.curr_trans), PrimFileId, 
                    pCurrKey, currKeyLen, bDoRemoveLock );
        }
        /* return if expected number of results is reached */
        if (CurResultCnt == SelectParam.sfp_m_result_cnt())
        {
            if (mm_direct == SelectParam.sfp_bd_mess2_type()){
                return;
            }
            bd402_KeyAssign (pCurrKey, currKeyLen, pStartKey, StartKeyLen, TrError);
            switch (SelectParam.sfp_bd_mess2_type())
            {
            case mm_first: SelectParam.sfp_bd_mess2_type().becomes(mm_next); break;
            case mm_last : SelectParam.sfp_bd_mess2_type().becomes(mm_prev); break;
            case mm_next : break;
            case mm_prev : break;
            default      : TrError = e_not_implemented; break;
            }
            return;
        }
        switch (TrError)
        {
        case e_ok:
            ++RecIter;
            if (e_ok != TrError){
                break;
            }
            pRec = RecIter.bd510GetRecord();
            if ( 0 == pRec )
                TrError = bAscendingKeyOrder ? e_no_next_record : e_no_prev_record; 
            else
            {
                LastResultLen = SelectParam.sfp_result_length();
                SelectParam.sfp_result_length() = InitialResultLen;
            }
            break;
        case e_wait_for_lock_release:
           TrError = e_ok;
            {  
                tsp00_Key           reEntranceKey;
                tsp00_Int2          reEntranceKeyLen = 0;
                const tsp00_KeyPtr  pReEntranceKey   = ( tsp00_KeyPtr ) &reEntranceKey;

                RecIter.bd510Break( pReEntranceKey, reEntranceKeyLen );

               // wait must be called - also in case of an error -, because an sql lock request already exist
                k53wait (*InvCurrent.curr_trans, SelectParam.sfp_bd_mess_type(), SelectParam.sfp_bd_mess2_type());

                if (e_ok != TrError){
                    break; 
                }
                bDoRemoveLock = PrimFileId.fileHandling_gg00().includes(hsCollisionTest_egg00);

                RecIter.bd510Continue( pReEntranceKey, reEntranceKeyLen ); 

                if( e_ok == TrError )
                {
                    pRec = RecIter.bd510GetRecord();
                    if ( 0 == pRec ){
                        TrError = bAscendingKeyOrder ? e_no_next_record : e_no_prev_record;
                    }else{
                        continue;
                    }
                }
                if( e_ok != TrError && bDoRemoveLock )
                { 
                    // Remove row share lock of commited read, in case of abnormal termination of bd402SearchResult
                    bd402_RemoveCommittedReadShareLock( *(InvCurrent.curr_trans), PrimFileId, 
                        pReEntranceKey, reEntranceKeyLen, bDoRemoveLock );
                }
            } 
            break;
        case e_buffer_limit:
            bd402_PrepareNextLoop (TrError, pCurrKey, currKeyLen, pStartKey, StartKeyLen,
                                   SelectParam, CurResultCnt);
            break;
        default:
            break;
        }
    } /*endwhile*/
    if (CurResultCnt < SelectParam.sfp_m_result_cnt())
    {
        SelectParam.sfp_m_result_cnt()  = CurResultCnt;
        SelectParam.sfp_result_length() = LastResultLen;
    }
}
