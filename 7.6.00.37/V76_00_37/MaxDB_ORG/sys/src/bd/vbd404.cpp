/****************************************************************************

  module      : vbd404.cpp

  -------------------------------------------------------------------------

  author      : AlexanderK
  responsible : UweH

  special area: aggregate functions (sum, max, min, avg, count) on inversion trees 
  description : see CRS 1104319


  last changed: 1999-10-05  14:45
  see also    : example.html ...

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 1999-2005 SAP AG



    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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

#include "gbd460.h"
#include "gbd500.h"
#include "gbd510.h"
#include "gbd600.h"

#include "hbd30.h"
#include "hbd404.h"
#include "hkb71.h"
#include "hkb720.h"

#include "SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp" // Kernel_move_and_fill


#if COMPILEMODE_MEO00 >= SLOW_MEO00
#include "hta99.h"
#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


#define TWO_BYTES_BD404         2
#define COUNT_DUPLICATES_BD404  true

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

class cbd404_DistinctRec
{
public: // members
    tsp00_Int2  m_FieldPos;
    bool        m_bDistinct;
    
public: // methods 
    void bd404Init(){m_FieldPos=-1; m_bDistinct=true;};
};

/*---------------------------------------------------------------------------*/

class cbd404_ResultRecord
{
public: // members
    tsp00_Buf           &m_ResultBuffer;
    tgg00_AggrDescrRec  &m_AggrFieldDescr;

public: // methods
    cbd404_ResultRecord                   (tsp00_Buf &ResultBuffer, tgg00_AggrDescrRec &AggrFieldDescr); 
    bool bd404ResultFieldIsNull           (int FieldNo);
    void bd404CopyKeyFieldToResultField   (int FieldNo, tsp00_KeyPtr pKeyFirld, int KeyFieldLen, tgg00_BasisError &TrError);
    void bd404CopyComplementaryKeyToField (int FieldNo, tsp00_KeyPtr pKeyField, int KeyFieldLen, tgg00_BasisError &TrError);
    void bd404CompKeyFieldWithResultField (int FieldNo,
                                           tsp00_KeyPtr        pKey,
                                           int                 KeyLen,          
                                           tsp00_KeyPtr       &pKeyField,      
                                           int                &KeyFieldLen,    
                                           tsp00_Key          &AuxField,        
                                           tsp00_LcompResult  &CompResult);     

private: //methods
    tsp00_Byte &bd404_FirstByteOfResultField (int FieldNo);
    tsp00_Int2  bd404_ResultFieldLen         (int FieldNo);
    tsp00_Int2  bd404_ResultFieldOffset      (int FieldNo);
    void        bd404_FillUpResultField      (int FieldNo, int KeyFieldLen, tgg00_BasisError &TrError);
};

/*===========================================================================*
 *  LOCAL FUNCTIONS (PROTOTYPES)                                             *
 *===========================================================================*/

static void inline
bd404_CountAllPrimKeysInInvList (cbd300_InvCurrent       &InvCurrent,
                                 tgg00_RecPtr             pInvRecord,
                                 tgg00_SelectFieldsParam &SelectParam,
                                 tgg00_StackDesc         &StackDesc,
                                 tsp00_Int4              &NumAllPrimKeys);

/*---------------------------------------------------------------------------*/

static inline void 
bd404_CountQualifiedPrimKeysInInvList (tgg00_TransContext      &Trans,
                                       cbd460_InvListIterator  &InvListIter,
                                       tgg00_SelectFieldsParam &SelectParam,
                                       tgg00_StackDesc         &StackDesc,
                                       tsp00_Int4              &NumQualifiedPrimKeys);

/*---------------------------------------------------------------------------*/

static void
bd404_CountQualifiedPrimKeysInInvList (cbd300_InvCurrent       &InvCurrent,
                                       cbd500_Tree             &InvTree,
                                       tgg00_RecPtr             pInvRecord,
                                       tgg00_SelectFieldsParam &SelectParam,
                                       tgg00_StackDesc         &StackDesc,
                                       tsp00_KeyPtr             pStartPrimKey,
                                       tsp00_Int4               StartPrimKeyLen,
                                       tsp00_KeyPtr             pStopPrimKey,
                                       tsp00_Int4               StopPrimKeyLen,
                                       tsp00_Int4              &NumQualifiedPrimKeys);

/*---------------------------------------------------------------------------*/

static void 
bd404_HandleSecKey (tgg00_TransContext  &Trans,
                    tgg00_Rec           &InvRecord,
                    tgg00_Rec           &AuxRecord, 
                    cbd404_ResultRecord &ResultRecord, 
                    tbd_current_tree    &AuxCurrent,
                    bool                 bSingleIndex, 
                    bool                 bDistinctAccessExists,
                    bool                 bOneListOnly,
                    bool                 bWarnOfNullValues);

/*---------------------------------------------------------------------------*/

static inline bool
bd404_KeyIsNull (tsp00_Key           &Key, 
                 tgg00_FieldDescrRec &FieldDescrRec); 

/*---------------------------------------------------------------------------*/

static void
bd404_LastAggStep (cbd404_ResultRecord &ResultRecord, 
                   tsp00_KeyPtr          pLastSecKey,
                   tsp00_Int2            LastSecKeyLen,
                   bool                  bSingleIndex, 
                   tgg00_BasisError     &TrError);

/*===========================================================================*
 *  CLASS METHODS (CODE)                                                     *
 *===========================================================================*/

inline
cbd404_ResultRecord::cbd404_ResultRecord(tsp00_Buf          &ResultBuffer, 
                                         tgg00_AggrDescrRec &AggrFieldDescr)
:
m_ResultBuffer   (ResultBuffer),
m_AggrFieldDescr (AggrFieldDescr)
{};

/*---------------------------------------------------------------------------*/

inline tsp00_Int2
cbd404_ResultRecord::bd404_ResultFieldLen  (int FieldNo)
{
    return (m_AggrFieldDescr.agg_field_descr [FieldNo].result_out_len);
}

/*---------------------------------------------------------------------------*/

inline tsp00_Int2
cbd404_ResultRecord::bd404_ResultFieldOffset  (int FieldNo)
{
    return (m_AggrFieldDescr.agg_field_descr [FieldNo].result_out_pos - POS_OFF_DIFF_BD00);
}

/*---------------------------------------------------------------------------*/

inline tsp00_Byte &
cbd404_ResultRecord::bd404_FirstByteOfResultField (int FieldNo)
{
    return (m_ResultBuffer [bd404_ResultFieldOffset(FieldNo)]);
}

/*---------------------------------------------------------------------------*/

inline bool
cbd404_ResultRecord::bd404ResultFieldIsNull (int FieldNo)
{
    return (csp_undef_byte == bd404_FirstByteOfResultField (FieldNo));
}

/*---------------------------------------------------------------------------*/

inline void
cbd404_ResultRecord::bd404_FillUpResultField (int               FieldNo, 
                                              int               KeyFieldLen,
                                              tgg00_BasisError &TrError)
{
    if (csp_unicode_def_byte != bd404_FirstByteOfResultField (FieldNo))
    {
        SAPDB_RangeFill( __FILE__,1, sizeof(m_ResultBuffer), m_ResultBuffer,
                bd404_ResultFieldOffset(FieldNo) + POS_OFF_DIFF_BD00 + KeyFieldLen,
                bd404_ResultFieldLen   (FieldNo) - KeyFieldLen, 
                bd404_FirstByteOfResultField (FieldNo), TrError);
    }
    else
    {
        SAPDB_RangeUnicodeFill (__FILE__,2, sizeof(m_ResultBuffer), m_ResultBuffer,
                   bd404_ResultFieldOffset(FieldNo) + POS_OFF_DIFF_BD00 + KeyFieldLen,
                   bd404_ResultFieldLen   (FieldNo) - KeyFieldLen,  
                   csp_unicode_blank, TrError);
    }
}

/*---------------------------------------------------------------------------*/

inline void 
cbd404_ResultRecord::bd404CopyKeyFieldToResultField (int               FieldNo,
                                                     tsp00_KeyPtr      pKeyField,
                                                     int               KeyFieldLen,
                                                     tgg00_BasisError &TrError)
{
    SAPDB_RangeMove( __FILE__,1, sizeof(tsp00_Key), sizeof (m_ResultBuffer), 
        pKeyField,      POS_OFF_DIFF_BD00,
        m_ResultBuffer, POS_OFF_DIFF_BD00 + bd404_ResultFieldOffset (FieldNo),
        KeyFieldLen, TrError);
    
    if ((e_ok == TrError) && (bd404_ResultFieldLen (FieldNo) > KeyFieldLen))
        bd404_FillUpResultField (FieldNo, KeyFieldLen, TrError);

    m_AggrFieldDescr.agg_field_descr [FieldNo].field_count = 1;
}

/*---------------------------------------------------------------------------*/

inline void 
cbd404_ResultRecord::bd404CopyComplementaryKeyToField (int               FieldNo,
                                                       tsp00_KeyPtr      pKeyField,
                                                       int               KeyFieldLen,
                                                       tgg00_BasisError &TrError)
{
    for (int i=0; i<KeyFieldLen; ++i)
       m_ResultBuffer[bd404_ResultFieldOffset (FieldNo) + i] = 255 - pKeyField[i];
    
    if ((e_ok == TrError) && (bd404_ResultFieldLen (FieldNo) > KeyFieldLen))
        bd404_FillUpResultField (FieldNo, KeyFieldLen, TrError);

    m_AggrFieldDescr.agg_field_descr [FieldNo].field_count = 1;
}


/*---------------------------------------------------------------------------*/

inline void 
cbd404_ResultRecord::bd404CompKeyFieldWithResultField (int                 FieldNo,
                                                       tsp00_KeyPtr        pKey,
                                                       int                 KeyLen,
                                                       tsp00_KeyPtr       &pKeyField,
                                                       int                &KeyFieldLen,
                                                       tsp00_Key          &AuxField,
                                                       tsp00_LcompResult  &CompResult)
{
    const tgg00_FieldDescrRec & FieldDescr = m_AggrFieldDescr.agg_field_descr [FieldNo];
    
    if (FieldDescr.field_rec_len > 0)
        KeyFieldLen = FieldDescr.field_rec_len;
    else 
        KeyFieldLen = KeyLen - FieldDescr.field_rec_pos + POS_OFF_DIFF_BD00;
    
    if (FieldDescr.field_is_descen)
    {   /* multiple index, decending, InvKey != NULL */                         
        for (int i=0; i<KeyFieldLen; ++i)
        {   /*create complementary key */
            AuxField[i] = 255 - (pKey [FieldDescr.field_rec_pos - POS_OFF_DIFF_BD00 + i]);
        }
        
        pKeyField = (tsp00_KeyPtr) &AuxField;
    }
    else 
    {   /* multiple index, ascending, InvKey != NULL */ 
        pKeyField = pKey + FieldDescr.field_rec_pos - POS_OFF_DIFF_BD00;
    }
    
    /* insert the new key if it is larger than the key already inserted in the result record */
    s30cmp (pKeyField, POS_OFF_DIFF_BD00, KeyFieldLen, m_ResultBuffer, 
        FieldDescr.result_out_pos, FieldDescr.result_out_len, CompResult);
}

/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

externCpp void                    
bd404AggrFuncOnIndex (cbd300_InvCurrent       &InvCurrent,
                      tbd_current_tree        &AuxCurrent,
                      tsp00_KeyPtr             pStartSecKey, 
                      tsp00_Int2              &StartSecKeyLen, 
                      tsp00_KeyPtr             pStopSecKey, 
                      tsp00_Int2               StopSecKeyLen,
                      tsp00_KeyPtr             pStartPrimKey,
                      tsp00_Int2               StartPrimKeyLen,
                      tsp00_KeyPtr             pStopPrimKey,
                      tsp00_Int2               StopPrimKeyLen,
                      tgg00_AggrDescrRec      &AggrFieldDescr,
                      tgg00_SelectFieldsParam &SelectParam,
                      tgg00_StackDesc         &StackDesc,
                      tgg00_Rec               &AuxRecord, 
                      tsp00_Buf               &ResultBuffer, 
                      bool                     bDistinctAccessExists, 
                      bool                     bOneListOnly,
                      bool                     bWarnOfNullValues)
{
    ROUTINE_DBG_MEO00 ("bd404AggrFuncOnIndex");

    bool                 bStoreLastSecKey   = false;
    bool                 bStartSecKeyFound; 
    bool                 bSingleIndex       = (tfnOmsInv_egg00 == InvCurrent.curr_tree_id.fileTfn_gg00());
    tgg00_BasisError    &TrError            = InvCurrent.curr_trans->trError_gg00;
    tsp00_Key           &LastSecKey         = AggrFieldDescr.agg_last_sec_key.keyVal_gg00();
    tsp00_Int2          &LastSecKeyLen      = AggrFieldDescr.agg_last_sec_key.keyLen_gg00();
    cbd404_ResultRecord  ResultRecord (ResultBuffer, AggrFieldDescr);

    for (int i=0; i<AggrFieldDescr.agg_cnt; ++i) 
    {
        const tgg00_FieldDescrRec & FieldDescr = AggrFieldDescr.agg_field_descr [i];

        bStoreLastSecKey = 
                  (((InvCurrent.curr_tree_id.fileTfn_gg00() ==tfnOmsInv_egg00)  ||
                  (FieldDescr.field_rec_pos == 1))                               &&
                  (((FieldDescr.field_function == op_f_max)                 &&
                  (InvCurrent.curr_tree_id.fileTfn_gg00()== tfnOmsInv_egg00)   &&
                  (!FieldDescr.field_is_descen))                               ||
                  ((FieldDescr.field_function == op_f_min)                  &&
                  (FieldDescr.field_is_descen))));
        
        if (bStoreLastSecKey) break;
    }

// PTS 1134751 UH 2005-04-06 removed check
//#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
//    t01msgcheck ("b40aggr: invalid prim key len ", (StartPrimKeyLen != 0), StartPrimKeyLen);
//#   endif

    /* create iterator for inverted key */
    cbd600_Node           Node     (InvCurrent);
    cbd500_Tree           InvTree  (Node);
    cbd510_RecordIterator InvListRecIter (InvTree, pStartSecKey, StartSecKeyLen,  pStopSecKey, StopSecKeyLen,
        ASCENDING_KEY_ORDER_BD510, INCLUDE_START_KEY_BD510, bStartSecKeyFound);
    
    tgg00_RecPtr pRec = InvListRecIter.bd510GetRecord();
    /* fill the inv list set with inv list */
    while ( e_ok == TrError && pRec != 0 )
    {
#       if COMPILEMODE_MEO00 >= SLOW_MEO00 
        t01sname (bd_inv, "sec key:    ");
        t01lkey  (bd_inv, pRec->recKey_gg00());
#       endif
        
        SelectParam.sfp_m_result_addr() = REINTERPRET_CAST (tsp00_MoveObjPtr, &ResultBuffer);
        SelectParam.sfp_m_result_size() = sizeof (ResultBuffer);
        SelectParam.sfp_m_result_len()  = 0;
        
        /* find number of qualified primary keys and records, respectively */
        if  (AggrFieldDescr.agg_opt_infos.includes (prim_keys))
        { 
            // quailfication is on primary and secondary keys  
            bd404_CountQualifiedPrimKeysInInvList (InvCurrent, InvTree, pRec, 
                SelectParam, StackDesc, pStartPrimKey, StartPrimKeyLen, pStopPrimKey, StopPrimKeyLen, 
                AggrFieldDescr.agg_prim_key_cnt);
        }
        else
        { 
            // qualification is on the secondary key only
            bd404_CountAllPrimKeysInInvList (InvCurrent, pRec, 
                SelectParam, StackDesc, AggrFieldDescr.agg_prim_key_cnt);
        }

        if (e_ok == TrError && (AggrFieldDescr.agg_prim_key_cnt > 0))
        {
            if( bStoreLastSecKey )
            {
                SAPDB_RangeMove( __FILE__, 2,
                    sizeof( tsp00_Key ), sizeof( tsp00_Key ),
                    &(pRec->recKey_gg00().keyVal_gg00()), POS_OFF_DIFF_BD00, 
                    &LastSecKey, POS_OFF_DIFF_BD00,  
                    pRec->recKey_gg00().keyLen_gg00(), TrError );
                
                if ( e_ok != TrError ) 
                    break;
                
                LastSecKeyLen = pRec->recKey_gg00().keyLen_gg00();
            }
            
            bd404_HandleSecKey ( *(InvCurrent.curr_trans), *pRec, AuxRecord, 
                ResultRecord, AuxCurrent, bSingleIndex, bDistinctAccessExists, 
                bOneListOnly, bWarnOfNullValues);

            if  (e_ok != TrError) break;

            if  (bOneListOnly) 
            {
                /* store next sec key as start sec key for next call    */
                /* of bd404AggrFuncOnIndex and stop loop over inversion */
 
                ++ InvListRecIter;
                pRec = InvListRecIter.bd510GetRecord();

                if (NULL == pRec)
                    TrError = e_no_next_record;
                else
                {
                    SAPDB_RangeMove( __FILE__, 3,
                        sizeof( tsp00_Key ), sizeof( tsp00_Key ),
                        &(pRec->recKey_gg00().keyVal_gg00()), POS_OFF_DIFF_BD00, 
                        pStartSecKey, POS_OFF_DIFF_BD00,  
                        pRec->recKey_gg00().keyLen_gg00(), TrError );
                    
                    if( e_ok != TrError ) 
                        break;
                    
                    StartSecKeyLen = pRec->recKey_gg00().keyLen_gg00();
                }
                break;
            }
        }
        
        /* goto next inverted list */
        ++ InvListRecIter;
        pRec = InvListRecIter.bd510GetRecord();
    }
    
    if ((e_ok == TrError) && (LastSecKeyLen > 0))
        bd404_LastAggStep (ResultRecord, (tsp00_KeyPtr) &LastSecKey, LastSecKeyLen, bSingleIndex, TrError);
   
    if ((e_ok == TrError) && bOneListOnly && (NULL == pRec))
        TrError = e_no_next_record;
}
 
/*===========================================================================*
 *  LOCAL FUNCTIONS (CODE)                                                   *
 *===========================================================================*/

static void inline
bd404_CountAllPrimKeysInInvList (cbd300_InvCurrent       &InvCurrent,
                                 tgg00_RecPtr             pInvRecord,
                                 tgg00_SelectFieldsParam &SelectParam,
                                 tgg00_StackDesc         &StackDesc,
                                 tsp00_Int4              &NumAllPrimKeys)
{
    ROUTINE_DBG_MEO00 ("bd404_CountAllPrimKeysInInvList");
    
    /* this function returns via the variable NumAllPrimKeys either the */
    /* number of all primary keys stored in the inversion InvRecord or  */
    /* zero depending on the qualification of the secondary key          */

    tgg00_BasisError   &TrError      = InvCurrent.curr_trans->trError_gg00;
    tsp00_Bool          bUnqualified = false;
    const tsp00_KeyPtr  pSecKey      = (tsp00_KeyPtr) &pInvRecord->recKey_gg00().keyVal_gg00();
    const tsp00_Int2   &SecKeyLen    = pInvRecord->recKeyLen_gg00();
    const tsp00_KeyPtr  pNullPrimKey = NULL;

    /* check if the secondary key is qualified */   
    k71inv_col_select (*InvCurrent.curr_trans, SelectParam, StackDesc, 
        pSecKey, SecKeyLen, pNullPrimKey, 0, bUnqualified);

    if (bUnqualified)
    {
        TrError        = e_ok;
        NumAllPrimKeys = 0;
    }
    else
    {  
        if (e_ok == TrError)
        {           
            switch (pInvRecord->recInvListKind_gg00())
            {
            case ilkArray_egg00:
                NumAllPrimKeys = pInvRecord->recNumPrimKeys_gg00();
                break;
                
            case  ilkSubTree_egg00:
                {
                    cbd450_InvListRefSubTree InvListRefSubTree (pInvRecord);
                    NumAllPrimKeys = InvListRefSubTree.bd450NumPrimKeys ();
                }
                break;
                
            default:
                TrError = e_illegal_record;
            }
        }
    }
}

/*---------------------------------------------------------------------------*/

static inline void 
bd404_CountQualifiedPrimKeysInInvList (tgg00_TransContext      &Trans,
                                       cbd460_InvListIterator  &InvListIter,
                                       tgg00_SelectFieldsParam &SelectParam,
                                       tgg00_StackDesc         &StackDesc,
                                       tsp00_Int4              &NumQualifiedPrimKeys)
{
    ROUTINE_DBG_MEO00 ("bd404_CountQualifiedPrimKeysInInvList_II");
    
    tgg00_BasisError &TrError      = Trans.trError_gg00;
    tsp00_Bool        bUnqualified = false;
    tsp00_KeyPtr      pPrimKey;
    tsp00_KeyPtr      pSecKey;
    tsp00_Int2        PrimKeyLen;
    tsp00_Int2        SecKeyLen;
    
    /* initialize output value */
    NumQualifiedPrimKeys = 0;

    bool hasDeletedFlag;
    InvListIter.bd460Deref (pPrimKey,PrimKeyLen, hasDeletedFlag); // TODO T.A.
    
    /* go through invlist an check qualification of each sec key,prim key pair */
    while ((NULL != pPrimKey) && (e_ok == TrError))
    {
        InvListIter.bd460GetSecKey( pSecKey, SecKeyLen ); // PTS 1116651 TS 2002-07-17

        k71inv_col_select (Trans, SelectParam, StackDesc, 
            pSecKey, SecKeyLen, pPrimKey, PrimKeyLen, bUnqualified);
        
        if (bUnqualified) 
            TrError = e_ok;
        else
        {
            if (e_ok == TrError)
                ++NumQualifiedPrimKeys;
        }
        
        ++InvListIter;
        if (e_ok != TrError) return;
        
        InvListIter.bd460Deref (pPrimKey,PrimKeyLen, hasDeletedFlag); // TODO T.A.
    }
}


/*---------------------------------------------------------------------------*/

static void
bd404_CountQualifiedPrimKeysInInvList (cbd300_InvCurrent       &InvCurrent,
                                       cbd500_Tree             &InvTree,
                                       tgg00_RecPtr             pInvRecord,
                                       tgg00_SelectFieldsParam &SelectParam,
                                       tgg00_StackDesc         &StackDesc,
                                       tsp00_KeyPtr             pStartPrimKey,
                                       tsp00_Int4               StartPrimKeyLen,
                                       tsp00_KeyPtr             pStopPrimKey,
                                       tsp00_Int4               StopPrimKeyLen,
                                       tsp00_Int4              &NumQualifiedPrimKeys)
{
    ROUTINE_DBG_MEO00 ("bd404_CountQualifiedPrimKeysInInvList");
    

    switch (pInvRecord->recInvListKind_gg00()) 
    {
    case ilkArray_egg00:
        {   /* get primkey iterator on arrays */
            cbd460_InvListArrayIterator  InvListIter (pInvRecord, pStartPrimKey, StartPrimKeyLen,
                pStopPrimKey, StopPrimKeyLen, ASCENDING_KEY_ORDER_BD510, INCLUDE_START_KEY_BD510,
                InvCurrent.bd300ConsistentReadSupport());
            
            bd404_CountQualifiedPrimKeysInInvList (*InvCurrent.curr_trans, // PTS 1116651 TS 2002-07-17
                InvListIter, SelectParam, StackDesc, NumQualifiedPrimKeys);
        }
        break;
        
    case ilkSubTree_egg00:
        {   /* get primkey iterator on subtrees */
            cbd450_InvListRefSubTree InvListRefSubTree (pInvRecord);
            
            cbd460_InvListSubtreeIteratorNoBreak InvListIter (InvTree, InvListRefSubTree, // PTS 1116651 TS 2002-07-17
                pStartPrimKey, StartPrimKeyLen, pStopPrimKey , StopPrimKeyLen, 
                ASCENDING_KEY_ORDER_BD510, INCLUDE_START_KEY_BD510);
            
            bd404_CountQualifiedPrimKeysInInvList (*InvCurrent.curr_trans, // PTS 1116651 TS 2002-07-17
                InvListIter, SelectParam, StackDesc, NumQualifiedPrimKeys);
        }
        break;
        
    default:
        InvCurrent.curr_trans->trError_gg00 = e_illegal_record;
    }
}

/*---------------------------------------------------------------------------*/

static void 
bd404_HandleSecKey (tgg00_TransContext  &Trans,
                    tgg00_Rec           &InvRecord,
                    tgg00_Rec           &AuxRecord, 
                    cbd404_ResultRecord &ResultRecord, 
                    tbd_current_tree    &AuxCurrent,
                    bool                 bSingleIndex, 
                    bool                 bDistinctAccessExists,
                    bool                 bOneListOnly,
                    bool                 bWarnOfNullValues)
{
    ROUTINE_DBG_MEO00 ("bd404_HandleSecKey");


    tgg00_BasisError    &TrError = Trans.trError_gg00;
    cbd404_DistinctRec   DistinctRec [MAX_COL_SEQUENCE_GG00];
    bool                 bNullValuesFound    = false;
    bool                 bCallKBForSummation = false;
    bool                 bFound;
    tgg00_FieldDescrRec *pFieldDesc;
    int                  i;
    int                  KeyFieldLen;
    tsp00_KeyPtr         pKeyField;
    tsp00_Key           &AuxKey=AuxRecord.recKey_gg00().keyVal_gg00();
    tsp00_LcompResult    CompResult;

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01int4  (bd_inv, "prim key cnt", ResultRecord.m_AggrFieldDescr.agg_prim_key_cnt);
    t01sname (bd_inv, "sec key:    ");
    t01lkey  (bd_inv, InvRecord.recKey_gg00());
#   endif
    
    /* initialize auxiliary structure for optimized check of the uniqueness of avalue */
    if  (bDistinctAccessExists)
    {
        for(int i=0; i<ResultRecord.m_AggrFieldDescr.agg_cnt; ++i)
            DistinctRec [i].bd404Init();
    }

    for (int iFieldNo=0; (iFieldNo < ResultRecord.m_AggrFieldDescr.agg_cnt) &&  (e_ok == TrError); ++iFieldNo)
    {   
        /* create shortcut to the field description */
        pFieldDesc = &(ResultRecord.m_AggrFieldDescr.agg_field_descr [iFieldNo]);

        pFieldDesc->field_sum_flag = false;

        switch (pFieldDesc->field_function)
        {
        case op_f_max: // ============= MAX ==================

            if (bd404_KeyIsNull (InvRecord.recKey_gg00().keyVal_gg00(), *pFieldDesc))
                bNullValuesFound = true;
            else
            {
                /* optimization: the following case is treated in bd404_LastAggStep */          
                if (!pFieldDesc->field_is_descen && bSingleIndex) break;
                
                if (pFieldDesc->field_is_descen &&  (bSingleIndex || (pFieldDesc->field_rec_pos == 1)))
                {
                    if (ResultRecord.bd404ResultFieldIsNull (iFieldNo))
                    {   
                        if (bSingleIndex || (pFieldDesc->field_rec_len == 0)) /* first and only field in a multiple index */
                            KeyFieldLen = InvRecord.recKeyLen_gg00();
                        else
                            KeyFieldLen = pFieldDesc->field_rec_len;
                        
                        /* put field into result field */
                        ResultRecord.bd404CopyComplementaryKeyToField (iFieldNo,
                            (tsp00_KeyPtr) &InvRecord.recKey_gg00().keyVal_gg00(), KeyFieldLen, TrError);
                    }
                }
                else 
                {                   
                    ResultRecord.bd404CompKeyFieldWithResultField (iFieldNo, 
                        (tsp00_KeyPtr) &InvRecord.recKey_gg00().keyVal_gg00(), InvRecord.recKeyLen_gg00(),
                        pKeyField, KeyFieldLen, AuxKey, CompResult);
                    
                    /* put field into result field */
                    if (l_greater == CompResult)
                        ResultRecord.bd404CopyKeyFieldToResultField (iFieldNo, pKeyField, KeyFieldLen, TrError);
                }   
            }
            break;
            
        case op_f_min: // ================= MIN ======================

            if (bd404_KeyIsNull (InvRecord.recKey_gg00().keyVal_gg00(), *pFieldDesc))
                bNullValuesFound = true;
            else
            {
                /* optimization: the following case is treated in bd404_LastAggStep */
                if (pFieldDesc->field_is_descen && (bSingleIndex || (pFieldDesc->field_rec_pos == 1))) break;

                if (! pFieldDesc->field_is_descen && (bSingleIndex || (pFieldDesc->field_rec_pos == 1)))
                {
                    if (ResultRecord.bd404ResultFieldIsNull (iFieldNo))
                    {
                        if ((bSingleIndex) || (pFieldDesc->field_rec_len == 0)) /* first and only field in a multiple index */
                            KeyFieldLen = InvRecord.recKeyLen_gg00();
                        else
                            KeyFieldLen = pFieldDesc->field_rec_len;
                        
                        pKeyField = (tsp00_KeyPtr) &InvRecord.recKey_gg00().keyVal_gg00();
                        ResultRecord.bd404CopyKeyFieldToResultField (iFieldNo, pKeyField, KeyFieldLen, TrError);
                    }
                }
                else 
                {   
                    ResultRecord.bd404CompKeyFieldWithResultField (iFieldNo, 
                        (tsp00_KeyPtr) &InvRecord.recKey_gg00().keyVal_gg00(), InvRecord.recKeyLen_gg00(),
                        pKeyField, KeyFieldLen, AuxKey, CompResult);
                    
                    if (l_less == CompResult)
                        ResultRecord.bd404CopyKeyFieldToResultField (iFieldNo, pKeyField, KeyFieldLen, TrError);
                }
            }
            break;
            
        case op_f_sum: // =============== SUM and AVG ================
        case op_f_avg: 
            if (bd404_KeyIsNull (InvRecord.recKey_gg00().keyVal_gg00(), *pFieldDesc))
                bNullValuesFound = true;
            else
            {
                pFieldDesc->field_count    += ResultRecord.m_AggrFieldDescr.agg_prim_key_cnt;
                pFieldDesc->field_sum_flag  = true;
            }
            break;
            
        case op_f_dis_sum: // ====== DISTINCT SUM/AVG/COUNT =========
        case op_f_dis_avg:
        case op_f_dis_count:
            
            if (bSingleIndex || bOneListOnly)
            {
                // PTS 1113272  
                if (bd404_KeyIsNull (InvRecord.recKey_gg00().keyVal_gg00(), *pFieldDesc))
                    bNullValuesFound = true;
                else
                {
                    ++ pFieldDesc->field_count;
                    pFieldDesc->field_sum_flag = (op_f_dis_count != pFieldDesc->field_function);
                }
            }
            else
            {
                if (bd404_KeyIsNull (InvRecord.recKey_gg00().keyVal_gg00(), *pFieldDesc))
                    bNullValuesFound = true;
                else
                {
                    /* first go through auxiliary structure and check if this value is distinct */
                    bFound = false;
                    
                    for (i=0; i<iFieldNo; ++i)
                    {
                        if (DistinctRec [i].m_FieldPos == pFieldDesc->field_rec_pos)
                        {
                            bFound = true;
                            if (DistinctRec [i].m_bDistinct)
                            {
                                ++ pFieldDesc->field_count;
                                pFieldDesc->field_sum_flag = (op_f_dis_count != pFieldDesc->field_function);
                            }
                            break;
                        }
                    }
                    
                    if (! bFound)
                    {
                        DistinctRec [iFieldNo].m_FieldPos = pFieldDesc->field_rec_pos;
                        
                        if (pFieldDesc->field_rec_len > 0)
                            KeyFieldLen = pFieldDesc->field_rec_len;
                        else 
                            KeyFieldLen = InvRecord.recKeyLen_gg00() - pFieldDesc->field_rec_pos + POS_OFF_DIFF_BD00;
                        
                        AuxRecord.recKeyLen_gg00() = KeyFieldLen + TWO_BYTES_BD404;
                        AuxRecord.recLen_gg00()    = AuxRecord.recKeyLen_gg00() + cgg_rec_key_offset;
                        
                        /* fill first two bytes of key with field position */
                        tsp00_Int2 *pFieldPosInKey = (tsp00_Int2*) &AuxRecord.recKey_gg00().keyVal_gg00()[0];
                        *pFieldPosInKey = pFieldDesc->field_rec_pos;
                        
                        /* fill the key with the current value */
                        SAPDB_RangeOverlappingMove( __FILE__, 2, sizeof(tsp00_Key), sizeof (tsp00_Key),
                            InvRecord.recKey_gg00().keyVal_gg00(), pFieldDesc->field_rec_pos,
                            AuxRecord.recKey_gg00().keyVal_gg00(), POS_OFF_DIFF_BD00 + TWO_BYTES_BD404,
                            KeyFieldLen, TrError);
                        
                        /* try to insert the record into a temporary tree */
                        bd30AddToTempTree (! COUNT_DUPLICATES_BD404, AuxRecord, AuxCurrent);
                        
                        if (e_ok == TrError)
                        {
                            ++ (pFieldDesc->field_count);
                            pFieldDesc->field_sum_flag = (op_f_dis_count != pFieldDesc->field_function); 
                        }
                        else if (e_duplicate_key)
                        {
                            TrError = e_ok;
                            DistinctRec [i].m_bDistinct = false;
                        }
                    }
                }
            }
            break;

        case op_f_count: // ================ COUNT ==================
            /* count only those keys which are not NULL. note that   */
            /* single index do not contain Null values by definition */
            // PTS 1113272  
            if (! bd404_KeyIsNull (InvRecord.recKey_gg00().keyVal_gg00(), *pFieldDesc)) 
                pFieldDesc->field_count += ResultRecord.m_AggrFieldDescr.agg_prim_key_cnt;
            else
                bNullValuesFound = true;
            break;
            
        case op_f_all_count: // ================ COUNT ALL ======================
            pFieldDesc->field_count += ResultRecord.m_AggrFieldDescr.agg_prim_key_cnt;
            break;
            
        default:
            TrError = e_stack_op_illegal;
        }

        /* set flag in case a summaion has to be peformed */
        bCallKBForSummation = (bCallKBForSummation || pFieldDesc->field_sum_flag); 
    }

    /* send a warning in case null values have been found */
    if (bWarnOfNullValues && bNullValuesFound)
    {       
        Trans.trWarning_gg00.addElement (warn0_exist);
        Trans.trWarning_gg00.addElement (warn2_null_in_builtin_func);      
    }
    
    if (bCallKBForSummation && (e_ok == TrError))
    {
        k720apply_aggr_func (ResultRecord.m_AggrFieldDescr, InvRecord.recKey_gg00(), 
        ResultRecord.m_ResultBuffer, bOneListOnly, TrError);
    }
}

/*---------------------------------------------------------------------------*/

static void
bd404_LastAggStep (cbd404_ResultRecord  &ResultRecord, 
                   tsp00_KeyPtr          pKeyField,
                   tsp00_Int2            KeyFieldLen,
                   bool                  bSingleIndex, 
                   tgg00_BasisError     &TrError)
{
    ROUTINE_DBG_MEO00 ("bd404_LastAggStep");
    
    tgg00_FieldDescrRec *pFieldDesc;

    for (int iFieldNo=0; (iFieldNo < ResultRecord.m_AggrFieldDescr.agg_cnt) &&  (e_ok == TrError); ++iFieldNo)
    {
        /* create shortcut to the field description */
        pFieldDesc = &(ResultRecord.m_AggrFieldDescr.agg_field_descr [iFieldNo]);
        
        switch (pFieldDesc->field_function)
        {
        case op_f_max: // ============= MAX ==================
            
            if (!pFieldDesc->field_is_descen && bSingleIndex)
                ResultRecord.bd404CopyKeyFieldToResultField (iFieldNo, pKeyField, KeyFieldLen, TrError);
            break;
            
        case op_f_min: // ============= MIN ==================
            
            if (pFieldDesc->field_is_descen && (bSingleIndex || (pFieldDesc->field_rec_pos == 1)))
            {
                if (!bSingleIndex)
                    KeyFieldLen = pFieldDesc->field_rec_len;

                ResultRecord.bd404CopyComplementaryKeyToField (iFieldNo, pKeyField, KeyFieldLen, TrError);
            }
            break;
        }
    }
}

/*---------------------------------------------------------------------------*/

static inline bool
bd404_KeyIsNull (tsp00_Key           &Key, 
                 tgg00_FieldDescrRec &FieldDescrRec) 
{
    return ((FieldDescrRec.field_is_descen                                          &&
             csp_defined_byte == Key[FieldDescrRec.field_rec_pos-POS_OFF_DIFF_BD00])    ||
            (!FieldDescrRec.field_is_descen                                         &&
             csp_undef_byte   == Key[FieldDescrRec.field_rec_pos-POS_OFF_DIFF_BD00]));
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
