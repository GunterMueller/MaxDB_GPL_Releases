/*!
  @file           vbd600.cpp
  @ingroup        Node Handling
  @see            example.html ...
  @author         UweH
  @brief          implementation of methods for cbd600_Node.

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
#include "gbd450.h"
#include "gbd500.h"
#include "gbd520.h"
#include "gbd600.h"

#include "hsp30.h"
#include "hbd102.h"  //PTS 1103980 JA 1999-09-15
#include "hbd06.h"
#include "hbd13.h"
#include "hbd20_2.h" // bd20IsPageRequested
#include "hbd30.h"
#include "hgg01_1.h" // g01opmsg
#include "hgg01_3.h" // g01glob
#include "hgg04.h"
#include "SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp" // Kernel_move_and_fill
#include "Transaction/Trans_Context.hpp"
#include "DataAccess/Data_Types.hpp"
#include "RunTime/RTE_Message.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "Converter/Converter_PageCollector.hpp"

#if COMPILEMODE_MEO00 >= SLOW_MEO00 
#include "hta99.h"
#endif

#define SPACE_FOR_UPDATE_BD600        true
#define INSERT_RECORD_BD600           true
#define ALLOWED_LEAF_COUNT_DIFFERENCE 2 /* PTS 1107959 UH 2000-10-04 */

/// -------------------------------------------------------------------------------------------------------------------

void cbd600_Node::bd600AddRecordSpace (SAPDB_Int4    RequiredLen,
                                       SAPDB_Int4    RecIndex,
                                       SAPDB_Byte*  &pSpace,
                                       bool          withConsistentReadInfo,
                                       bool          withSpaceCheck)
{
    ROUTINE_DBG_MEO00 ("bd600AddRecordSpace");

    if ( withSpaceCheck && ! bd600SpaceIsAvailable (RequiredLen) )
    {
        m_TrError = e_no_more_space;
        return;
    }

    pSpace = reinterpret_cast<SAPDB_Byte*>(m_pNode) + m_pNode->nd_bottom() - POS_OFF_DIFF_BD00;

    if ( ! m_RecordList.AddPosition ( RecIndex,
                                      m_pNode->nd_bottom(),
                                      RequiredLen - POINTERSIZE_BD00,
                                      withConsistentReadInfo,
                                      Trans_Context::GetContext(*(m_Current.curr_trans)).ErrorList()) )
    {
        m_TrError = e_data_page_corrupted;
        RTE_Message(Trans_Context::GetContext(*(m_Current.curr_trans)).ErrorList());
        g01opmsg (sp3p_knldiag, sp3m_error, gbd610c1DataPageCorrupted_csp03,
            csp3_n_btree, "Pointerlist inconsistent", RecIndex);
        b06dump_bad_page (m_Current.curr_trans->trTaskId_gg00, 'd',
            FILE_EXT_COR_BD00, m_pNode->nd_id(), m_pNode, 1);
        b06write_filename_and_root (m_Current.curr_tree_id);
        return;
    }

    m_pNode->nd_sorted() =  false;
    m_pNode->nd_bottom() += bd102RecAlign (RequiredLen - POINTERSIZE_BD00);
}

/*---------------------------------------------------------------------------*/

void
cbd600_Node::bd600UpdRecordSpace (tsp00_Int4   RecSizeDelta,
						          tsp00_Int4   RecIndex,
								  tgg00_RecPtr pRec)
{
    ROUTINE_DBG_MEO00 ("bd600UpdRecordSpace");
    
#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
	t01int4 (bd_inv, "RecSizeDelta", RecSizeDelta);
#   endif 

    /* update space for changed record */

    /* the record is updated after enlarging the space => recLen is the old length */
    /* the record was updated before shrinking the space => recLen is the new length */

    const tsp00_Int4 oldRecordLen        = RecSizeDelta > 0
                                           ? pRec->recLen_gg00()
                                           : pRec->recLen_gg00() - RecSizeDelta;  
    const tsp00_Int4 SrcOffset           = bd600RecOffset(RecIndex) + bd102RecAlign(oldRecordLen);
    const tsp00_Int4 DestOffset          = RecSizeDelta > 0
                                           ? bd600RecOffset(RecIndex) + bd102RecAlign(pRec->recLen_gg00() + RecSizeDelta)
                                           : bd600RecOffset(RecIndex) + bd102RecAlign(pRec->recLen_gg00());
    const tsp00_Int4 MoveLen             = m_pNode->nd_bottom() - (SrcOffset + POS_OFF_DIFF_BD00);
    #ifdef SAPDB_SLOW
    if ( DataIndex_Trace.TracesLevel(6) )
    {
        Kernel_VTrace() << "OldRecLen: " << oldRecordLen
                        << ", RecSizeDelta: " << RecSizeDelta
                        << FlushLine
                        << "SrcOffset: " << SrcOffset
                        << " -> DestOffset: " << DestOffset
                        << " MoveLen=" << MoveLen
                        << " bottom=" << m_pNode->nd_bottom()
                        << FlushLine;
    }
    #endif
                    
    SAPDB_RangeOverlappingMove( __FILE__, 1,    
                                MAX_BOTTOM_BD00, MAX_BOTTOM_BD00,
                                m_pNode, POS_OFF_DIFF_BD00 + SrcOffset,
                                m_pNode, POS_OFF_DIFF_BD00 + DestOffset,
                                MoveLen, 
                                m_TrError );

    if (e_ok == m_TrError)
    {
        m_RecordList.UpdatePositions (SrcOffset + POS_OFF_DIFF_BD00, DestOffset - SrcOffset);
        m_pNode->nd_bottom() += (DestOffset - SrcOffset);
    }

    if  (e_ok != m_TrError) 
    {
        m_TrError = e_data_page_corrupted;
		bd600Dump (bd600c1DataPageCorrupted_csp03, "Data page corrupted     ");
        return;
    }
}

/*---------------------------------------------------------------------------*/

void
cbd600_Node::bd600DelRecordSpace (tsp00_Int4 RecIndex)
{
    ROUTINE_DBG_MEO00 ("bd600DelRecordSpace");

	const tsp00_Int4  RecLen    = m_RecordList.GetLength(RecIndex);
    const tsp00_Int4  ShiftLen  = bd102RecAlign(RecLen);
	const tsp00_Int4  SrcOffset = bd600RecOffset(RecIndex) + ShiftLen;
    const tsp00_Int4  MoveLen   = m_pNode->nd_bottom() - (SrcOffset + POS_OFF_DIFF_BD00);
	
	SAPDB_RangeOverlappingMove( __FILE__, 2,    
		MAX_BOTTOM_BD00 , MAX_BOTTOM_BD00,
		m_pNode, POS_OFF_DIFF_BD00 + SrcOffset,
		m_pNode, POS_OFF_DIFF_BD00 + SrcOffset - ShiftLen,
		MoveLen, m_TrError);
	
    if ( e_ok == m_TrError
         &&
         ! m_RecordList.DelPosition (RecIndex, Trans_Context::GetContext(*(m_Current.curr_trans)).ErrorList()) )
        m_TrError = e_data_page_corrupted;

    if ( e_ok == m_TrError )
        m_RecordList.UpdatePositions (SrcOffset + POS_OFF_DIFF_BD00, - ShiftLen);
    
    if (e_ok != m_TrError) 
    {
        m_TrError = e_data_page_corrupted;
        RTE_Message(Trans_Context::GetContext(*(m_Current.curr_trans)).ErrorList());
        g01opmsg (sp3p_knldiag, sp3m_error,	bd600c2DataPageCorrupted_csp03, 
            csp3_n_btree, "Data page corrupted     ", m_pNode->nd_id());
        b06dump_bad_page (m_Current.curr_trans->trTaskId_gg00, 'd',
            FILE_EXT_COR_BD00, m_pNode->nd_id(), m_pNode, 1);
        b06write_filename_and_root (m_Current.curr_tree_id);
        return;
    }

  	m_pNode->nd_bottom() -= ShiftLen;
}


/*---------------------------------------------------------------------------*/

bool
cbd600_Node::bd600NodeIsRequested   () const
{
	return (TRUE == bd20IsPageRequested (m_NodePtrs.np_cbptr()));
}

/*---------------------------------------------------------------------------*/

void
cbd600_Node::bd600Release (bool IsChanged)
{
    ROUTINE_DBG_MEO00 ("bd600Release");
    
    #ifdef SAPDB_SLOW
    Kernel_VTrace trace;
    
    if ( DataIndex_Trace.TracesLevel(9) )
    {
        trace << "cbd600_Node::bd600Release: "
              << "np_ptr="          << (m_NodePtrs.np_ptr() != 0)
              << ", np_cbptr="      << (m_NodePtrs.np_cbptr() != 0)
              << ", m_RequestKind=" << m_RequestKind
              << ", IsChanged="     << IsChanged
              << ", m_TrError="     << m_TrError
              << FlushLine;
    }
    #endif
    
    if ( NULL != m_NodePtrs.np_ptr() )
    {
        if ( m_NodePtrs.np_cbptr() != 0 )
        {
            if ( IsChanged && (nr_for_update == m_RequestKind)
                &&
                (e_ok == m_TrError) )
            {
                if( g01glob.treecheck )
                    bd600CheckWithErrorHandling(); // PTS 1125096 TS 2003-11-21
                
                b13w_release_node (m_NodePtrs, m_Current);
            }
            else  // m_RequestKind == nr_for_read 
                b13r_release_node (m_NodePtrs, m_Current, lru_normal);
        }
        
        m_NodePtrs.np_ptr() = 0;
    }
    m_RecordList.Deassign();
}
/*---------------------------------------------------------------------------*/

void
cbd600_Node::bd600SearchRecordConsistent (tsp00_KeyPtr pKey,
		                        tsp00_Int4             KeyLen,
                                const tgg91_TransNo*  pWriteTrans,
							    tsp00_Int4            &RecIndex,
							    tbd_searchresult      &SearchResult) const
{
    ROUTINE_DBG_MEO00 ("bd600SearchRecordConsistent");

    tsp00_LcompResult  CompResult (tsp00_LcompResult::fromConst (l_undef));
    tsp00_Int4         Count;
    tsp00_Int4         MaxRecIndex = bd600MaxRecIndex();
    tsp00_Int4         MinRecIndex = FIRST_REC_INDEX_BD00;
    tsp00_KeyPtr       pAuxKey;
    tsp00_Int2         AuxKeyLen;

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
	t01p2int4 (bd_inv, "NodeId:     ", m_pNode->nd_id(), "NodeLevel   ", m_pNode->nd_level());
#   endif 
   
	if (0 == m_pNode->nd_record_cnt())
	{
		SearchResult.becomes (nonefound);
		RecIndex = MinRecIndex;
#       if COMPILEMODE_MEO00 >= SLOW_MEO00 
	    t01p2int4 (bd_inv, "RecIndex    ", RecIndex, "SearchResult", int(SearchResult));
#       endif 
		return;
	}

    tgg91_TransNo writeTrans;
    if (pWriteTrans)
    {
        // build complement
        pWriteTrans->gg91BuildComplement (writeTrans);
        pWriteTrans = &writeTrans;
    }

    /*  MaxRecIndex > MinRecIndex */
    do
    {
        Count = MaxRecIndex - MinRecIndex + 1;
      
        if (Count > 2)
        {
            RecIndex  = MaxRecIndex - (Count >> 1);
            m_RecordList.GetConstKey (RecIndex, AuxKeyLen, pAuxKey);

            bd450IndexKeyCompare (true, pKey,    POS_OFF_DIFF_BD00, KeyLen, &pWriteTrans[0],
                                        pAuxKey, POS_OFF_DIFF_BD00, AuxKeyLen & 0x7F,
                                        CompResult);

            if (l_equal == CompResult)
			{
				SearchResult.becomes(thisfound);
                break; 
			}
            else
                if (l_less == CompResult)
                    MaxRecIndex = RecIndex;
                else
                    MinRecIndex = RecIndex;
        }
        else
        {
            if (Count == 2)
            {
				if (l_less == CompResult)
					MaxRecIndex = MinRecIndex;  
				else
				{					
					RecIndex  = MaxRecIndex;
                    m_RecordList.GetConstKey (RecIndex, AuxKeyLen, pAuxKey);
					
                    bd450IndexKeyCompare (true, pKey,    POS_OFF_DIFF_BD00, KeyLen, &pWriteTrans[0],
                                                pAuxKey, POS_OFF_DIFF_BD00, AuxKeyLen & 0x7F,
                                                CompResult);
					
					if (l_equal == CompResult)
					{
						SearchResult.becomes(thisfound);
						break; 
					}
					else  if (l_less == CompResult)
					{
						if ((bd600MaxRecIndex() - FIRST_REC_INDEX_BD00) > 1)
						{
							SearchResult.becomes(nextfound);
							break;
						}
						else
							MaxRecIndex = MinRecIndex;
					}
					else // l_greater == CompResult
					{
						if (RecIndex == bd600MaxRecIndex ())
						{
							SearchResult.becomes(lastfound);
							break;
						}
						else
						{
							SearchResult.becomes(nextfound);
							++RecIndex;
							break;
						}
					}
				}
            }
            else
            {
				RecIndex  = MaxRecIndex;
                m_RecordList.GetConstKey (RecIndex, AuxKeyLen, pAuxKey);
					
                bd450IndexKeyCompare (true, pKey,    POS_OFF_DIFF_BD00, KeyLen, &pWriteTrans[0],
                                            pAuxKey, POS_OFF_DIFF_BD00, AuxKeyLen & 0x7F,
                                            CompResult);
				
                if (l_equal == CompResult)
				{
					SearchResult.becomes(thisfound);
                    break; // PrimaryKey found
				}
                else if  (l_less == CompResult)
                {
					SearchResult.becomes(nextfound);
                    break;
                }
                else
                {	if (RecIndex == bd600MaxRecIndex ())
					{
						SearchResult.becomes(lastfound);
						break;
					}
					else
					{
						SearchResult.becomes(nextfound);
						++RecIndex;
						break;
					}

                }
            }
        }
    }
    while (e_ok == m_TrError);

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    if (e_ok == m_TrError)
		t01p2int4 (bd_inv, "RecIndex    ", RecIndex,
		                   "SearchResult", int(SearchResult));
#   endif 
}

/*---------------------------------------------------------------------------*/

void
cbd600_Node::bd600SearchRecordNonConsistent (tsp00_KeyPtr      pKey,
		                        tsp00_Int4                     KeyLen,
							    tsp00_Int4                    &RecIndex,
							    tbd_searchresult              &SearchResult) const
{
    ROUTINE_DBG_MEO00 ("bd600SearchRecordNonConsistent");

    tsp00_LcompResult  CompResult (tsp00_LcompResult::fromConst (l_undef));
    tsp00_Int4         Count;
    tsp00_Int4         MaxRecIndex = bd600MaxRecIndex();
    tsp00_Int4         MinRecIndex = FIRST_REC_INDEX_BD00;
    tsp00_KeyPtr       pAuxKey;
    tsp00_Int2         AuxKeyLen;

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
	t01p2int4 (bd_inv, "NodeId:     ", m_pNode->nd_id(), "NodeLevel   ", m_pNode->nd_level());
#   endif 
   
	if (0 == m_pNode->nd_record_cnt())
	{
		SearchResult.becomes (nonefound);
		RecIndex = MinRecIndex;
#       if COMPILEMODE_MEO00 >= SLOW_MEO00 
	    t01p2int4 (bd_inv, "RecIndex    ", RecIndex, "SearchResult", int(SearchResult));
#       endif 
		return;
	}

    /*  MaxRecIndex > MinRecIndex */
    do
    {
        Count = MaxRecIndex - MinRecIndex + 1;
      
        if (Count > 2)
        {
            RecIndex  = MaxRecIndex - (Count >> 1);
            m_RecordList.GetConstKey (RecIndex, AuxKeyLen, pAuxKey);

            s30cmp (pKey,    POS_OFF_DIFF_BD00, KeyLen, 
                    pAuxKey, POS_OFF_DIFF_BD00, AuxKeyLen,
                    CompResult);

            if (l_equal == CompResult)
			{
				SearchResult.becomes(thisfound);
                break; 
			}
            else
                if (l_less == CompResult)
                    MaxRecIndex = RecIndex;
                else
                    MinRecIndex = RecIndex;
        }
        else
        {
            if (Count == 2)
            {
				if (l_less == CompResult)
					MaxRecIndex = MinRecIndex;  
				else
				{					
					RecIndex  = MaxRecIndex;
                    m_RecordList.GetConstKey (RecIndex, AuxKeyLen, pAuxKey);
					
                    s30cmp (pKey,    POS_OFF_DIFF_BD00, KeyLen, 
                            pAuxKey, POS_OFF_DIFF_BD00, AuxKeyLen,
                            CompResult);
					
					if (l_equal == CompResult)
					{
						SearchResult.becomes(thisfound);
						break; 
					}
					else  if (l_less == CompResult)
					{
						if ((bd600MaxRecIndex() - FIRST_REC_INDEX_BD00) > 1)
						{
							SearchResult.becomes(nextfound);
							break;
						}
						else
							MaxRecIndex = MinRecIndex;
					}
					else // l_greater == CompResult
					{
						if (RecIndex == bd600MaxRecIndex ())
						{
							SearchResult.becomes(lastfound);
							break;
						}
						else
						{
							SearchResult.becomes(nextfound);
							++RecIndex;
							break;
						}
					}
				}
            }
            else
            {
				RecIndex  = MaxRecIndex;
                m_RecordList.GetConstKey (RecIndex, AuxKeyLen, pAuxKey);
					
                s30cmp (pKey,    POS_OFF_DIFF_BD00, KeyLen, 
                        pAuxKey, POS_OFF_DIFF_BD00, AuxKeyLen,
                        CompResult);
				
                if (l_equal == CompResult)
				{
					SearchResult.becomes(thisfound);
                    break; // PrimaryKey found
				}
                else if  (l_less == CompResult)
                {
					SearchResult.becomes(nextfound);
                    break;
                }
                else
                {	if (RecIndex == bd600MaxRecIndex ())
					{
						SearchResult.becomes(lastfound);
						break;
					}
					else
					{
						SearchResult.becomes(nextfound);
						++RecIndex;
						break;
					}

                }
            }
        }
    }
    while (e_ok == m_TrError);

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    if (e_ok == m_TrError)
		t01p2int4 (bd_inv, "RecIndex    ", RecIndex,
		                   "SearchResult", int(SearchResult));
#   endif 
}

/*---------------------------------------------------------------------------*/

void
cbd600_Node::bd600GetStatistic (tgg00_SampleInfo  &SampleInfo,
                                tbd_stat_aux_vars &StatAuxVars,
                                bool               bWithSelectivity) const
{
    ROUTINE_DBG_MEO00 ("bd600GetStatistic");

	if ( ptData_egg00 != m_pNode->nd_pt() )
	{
		m_TrError = e_not_implemented;
		return;
	}
	
    if ( LEAF_LEVEL_BD00 != m_pNode->nd_level() )
	{
		if (! bWithSelectivity)
			bd600_GetIndexNodeStatistic (SampleInfo, StatAuxVars);
	}
	else
		/* Leaf Page Statistic */
		switch ( m_pNode->nd_pt2() )
		{
		case pt2Inv_egg00:
			if (bWithSelectivity)
				bd600_GetInvLeafSelectivity (SampleInfo);
			else
				bd600_GetInvLeafStatistic (SampleInfo);
			break;
		case pt2InvSubtree_egg00:
			if (! bWithSelectivity)
				bd600_GetSubTreeLeafStatistic (SampleInfo);
			break;
		default:
			m_TrError = e_not_implemented;
		}
}

/*---------------------------------------------------------------------------*/

void
cbd600_Node::bd600_CheckLeafCount () const
{
    ROUTINE_DBG_MEO00 ("bd600_CheckLeafCount");

	tsp00_Int4 LeafCount;
	
	if ( m_pNode->nd_level() > LEAF_LEVEL_BD00 )
	{
		cbd520_Separator Separator;
		LeafCount = 0;
		for (tsp00_Int4 RecIndex = FIRST_REC_INDEX_BD00;
		     RecIndex <= bd600MaxRecIndex();
			 ++RecIndex)
		{
			Separator.Assign(bd600SepPtr(RecIndex));
			LeafCount += Separator.bd520LeafCount();
		}
	}
	else
	{
		tgg00_RecPtr pRec;

		LeafCount = 1;

		if ( pt2Inv_egg00 == m_pNode->nd_pt2() )
		{
			for (tsp00_Int4 RecIndex = FIRST_REC_INDEX_BD00;
				 RecIndex <= bd600MaxRecIndex();
				 ++RecIndex)
			{
				pRec = bd600RecPtr(RecIndex);
				if ( ilkSubTree_egg00 == pRec->recInvListKind_gg00() )
				{
					cbd450_InvListRefSubTree InvListRefSubTree (pRec);
					LeafCount += InvListRefSubTree.bd450NumLeaves();
				}
			}
		}
	}

    if ( LeafCount != m_pNode->ndLeafCount_bd00() )
    {
        m_TrError = e_data_page_corrupted;
        
        bd600Dump (bd600c1DataPageCorrupted_csp03, "Invalid LeafCount       ");
        g01opmsg  (sp3p_knldiag, sp3m_error,	bd600c1DataPageCorrupted_csp03, 
            csp3_n_btree, "wrong SumLeafCounts     ", LeafCount);
        g01opmsg  (sp3p_knldiag, sp3m_error, csp3_bd_msg, csp3_n_btree,
            "bd600_HandleLeafCount   ", m_pNode->nd_id());
    }

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
	t01int4 (bd_index, "NewLeafCount", m_pNode->ndLeafCount_bd00());
#   endif
}

/*---------------------------------------------------------------------------*/

// PTS 1125096 TS 2003-11-21
void
cbd600_Node::bd600CheckWithErrorHandling() const
{
    bd600Check( TRUE == g01glob.treecheck );
    
    if( e_ok != m_TrError )
    {
        if ( m_drop != m_Current.curr_action // PTS 1125235 UH 2003-11-05
            &&
            m_verify != m_Current.curr_action ) // PTS 1125235 UH 2003-11-05
        {
            g01opmsg (sp3p_knldiag, sp3m_error, csp3_bd_msg, csp3_n_btree,
                "Action Code             ", m_Current.curr_action);
            
            b06write_filename_and_root( m_Current.curr_tree_id );
        }
    }
}


/*---------------------------------------------------------------------------*/

static inline tgg00_RecoveryMode
bd600_GetRecoveryMode( const tgg00_FileId    &fileId )
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

void cbd600_Node::bd600CheckNode( bool bExtendedCheck ) const
{
    ROUTINE_DBG_MEO00 ("bd600CheckNode");

    const tgg00_RecoveryMode    recMode = bd600_GetRecoveryMode( m_Current.curr_tree_id );
    tsp00_TaskId taskId = m_Current.curr_trans->trTaskId_gg00;

    if ( m_pNode->nd_bottom() < BODY_BEG_BD00
         ||
         m_pNode->nd_bottom() > MAX_BOTTOM_BD00 )
    {
        m_TrError = e_data_page_corrupted; // invalid bottom
        g01abort  (csp3_bd_msg, csp3_n_btree, "bd600CheckNode          ", m_pNode->nd_id());
        return;
    }

    const tsp00_Int4 maxRecIndex    = m_pNode->nd_record_cnt()-1;
    tsp00_Int4       totalRecLength = 0;
    tsp00_Int2       prevKeyLength  = 0;
    tsp00_KeyPtr     pPrevKey       = NULL;
    tsp00_Int2       keyLength  = 0;
    tsp00_KeyPtr     pKey       = NULL;
    SAPDB_UInt2      keyLengthMask  = (CnsRead_Manager::IsConsistentRead_Configurated()
                                       &&
                                       pt2InvSubtree_egg00 == m_pNode->nd_pt2())
                                       ? 0x7FFF
                                       : 0xFFFF;
    const bool       isSubTreeLeaf = bd600IsSubTreeLeaf();
    
    for( tsp00_Int4 recIndex = FIRST_REC_INDEX_BD00; recIndex <= maxRecIndex; ++recIndex )
    {
        const tsp00_Int4 recPos = m_RecordList.GetPosition(recIndex);

        if ( recPos < BODY_BEG_BD00
             ||
             recPos >= m_pNode->nd_bottom() )
        {
            m_TrError = e_illegal_entrypos; // invalid record position
            g01abort  (csp3_bd_msg, csp3_n_btree, "bd600CheckNode          ", m_pNode->nd_id());
            return;
        }

        const tsp00_Int4   recLength = m_RecordList.GetLength(recIndex);

        totalRecLength = totalRecLength + bd102RecAlign( recLength );

        if( recPos + bd102RecAlign( recLength ) > m_pNode->nd_bottom() ) 
        {
            m_TrError = e_illegal_entrylength;  // invalid record length
            g01abort  (csp3_bd_msg, csp3_n_btree, "bd600CheckNode          ", m_pNode->nd_id());
            return;
        }

        if ( (isSubTreeLeaf && recLength < 1)
             ||
             (! isSubTreeLeaf && recLength  < MIN_RECORD_LEN_BD00) )  
        {
            m_TrError = e_illegal_entrylength;  // invalid record length
            g01abort  (csp3_bd_msg, csp3_n_btree, "bd600CheckNode          ", m_pNode->nd_id());
            return;
        }

        m_RecordList.GetConstKey(recIndex, keyLength, pKey);

        if (
            (( m_pNode->nd_level() == LEAF_LEVEL_BD00 ) && ( keyLength < 1 )) ||
            ( keyLength > PERM_KEY_MXSP00 ) ||
            ( keyLength > recLength )       ||
            (
                ( m_pNode->nd_level() > LEAF_LEVEL_BD00 ) &&
                ( keyLength + cgg_rec_key_offset + sizeof ( tbd00_SepLeafCount ) != recLength )
            )
        )
        {
            m_TrError = e_illegal_keylength; // invalid key length
            g01abort  (csp3_bd_msg, csp3_n_btree, "bd600CheckNode          ", m_pNode->nd_id());
            return;
        }

        // ADDITIONAL CHECKS ONLY EXECUTED IF REQUESTED

        if( ! bExtendedCheck )
            continue;

        if( NULL != pPrevKey )
        {
            tsp00_LcompResult compResult;
            if ( isSubTreeLeaf && CnsRead_Manager::IsConsistentRead_Configurated())
            {
                s30cmp (pKey, POS_OFF_DIFF_BD00, keyLength - sizeof(tgg91_TransNo),
                        pPrevKey, POS_OFF_DIFF_BD00, prevKeyLength - sizeof(tgg91_TransNo),
                        compResult );
                if (l_equal == compResult)
                {
                    s30cmp (pKey, POS_OFF_DIFF_BD00 + keyLength - sizeof(tgg91_TransNo), sizeof(tgg91_TransNo),
                            pPrevKey, POS_OFF_DIFF_BD00 + prevKeyLength - sizeof(tgg91_TransNo), sizeof(tgg91_TransNo),
                            compResult );
                }
            }
            else
            {
                s30cmp (pKey, POS_OFF_DIFF_BD00, keyLength,
                        pPrevKey, POS_OFF_DIFF_BD00, prevKeyLength,
                        compResult );
            }

            if( l_greater != compResult )
            {
                m_TrError = e_illegal_key; // invalid key order
                bd600Print(true);
                g01abort  (csp3_bd_msg, csp3_n_btree, "bd600CheckNode          ", m_pNode->nd_id());
                return;
            }
        }
        prevKeyLength = keyLength;
        pPrevKey      = pKey;
    }

    if( totalRecLength != m_pNode->nd_bottom() - BODY_BEG_BD00 )
    {
        m_TrError = e_illegal_record; // invalid record length
        g01abort  (csp3_bd_msg, csp3_n_btree, "bd600CheckNode          ", m_pNode->nd_id());
        return;
    }
}

/*---------------------------------------------------------------------------*/

// PTS 1125096 TS 2003-11-21
void
cbd600_Node::bd600Check( const bool bWithExtendedCheck ) const
{
    ROUTINE_DBG_MEO00 ("bd600Check");
    
    if ((e_ok == m_TrError) && (NIL_PAGE_NO_GG00 != m_pNode->nd_id()))
    {
        tsp00_TaskId     &taskId = m_Current.curr_trans->trTaskId_gg00;
        tbd_node_ptrs    nptrs   = m_NodePtrs; // necessary, because of pascal/c++ mix and const method
        
        if ( bd600IsSubTreeLeaf() )
            bd600CheckNode(bWithExtendedCheck);
        else
            bd13CheckNode( taskId, m_TrError, nptrs, m_Current.curr_tree_id, bWithExtendedCheck );
        
        if(
            ( bWithExtendedCheck                            ) &&
            ( e_ok            == m_TrError                  ) &&
            ( LEAF_LEVEL_BD00 == m_pNode->nd_level()        ) &&
            ( pt2Inv_egg00    == m_pNode->nd_pt2()          ) && // beware of pt2InvSubtree_egg00
            ( ! m_pNode->nd_pmode().includes( pmTemp_egg00 ))    // beware of tfnTempInv_egg00
            )
        {
            for( tsp00_Int4 recIndex = FIRST_REC_INDEX_BD00;
            recIndex <= bd600MaxRecIndex(); ++recIndex)
            {
                const tgg00_RecPtr pRec = bd600RecPtr( recIndex );
                
                switch( pRec->recInvListKind_gg00())
                {
                case( ilkArray_egg00 ):
                    {
                        // the destructor checks the record on failure it is aborted 
                        cbd450_InvListArray InvListArrayRec (m_Current.bd300ConsistentReadSupport(), pRec);
                    }
                    break;
                case( ilkSubTree_egg00 ):
                    break;
                default:
                    m_TrError = e_data_page_corrupted;
                    g01opmsg (sp3p_knldiag, sp3m_error, bd600c1WrongKeyOrder_csp03,
                        csp3_n_btree, "Wrong InvListKind       ", recIndex);
                    bd600Dump (csp3_bd_msg, "bd600Check              ");
                    break;
                }
            }
        }
    }
    if(
        ( e_ok == m_TrError   ) && 
        ( ! m_pNode->nd_pmode().includes( pmTemp_egg00 )) &&
        ( bWithExtendedCheck ) 
        )
    {
        bd600_CheckLeafCount(); 
    }
}

/*---------------------------------------------------------------------------*/

void
cbd600_Node::bd600_GetIndexNodeStatistic (tgg00_SampleInfo  &SampleInfo,
										  tbd_stat_aux_vars &StatAuxVars) const
{
    ROUTINE_DBG_MEO00 ("bd600GetIndexNodeStatistic");

	const tsp00_Int4  Covering  = (100 * bd600Covering() + HALF_COVERING_BD00) / FULLCOVERING_BD00;
	tgg00_RecPtr      pRec;
    tsp00_Int4        SepLen;

	++SampleInfo.nodes;
	++SampleInfo.indexnodes;
    if (SampleInfo.min_indexcov > Covering) SampleInfo.min_indexcov = Covering;
    if (SampleInfo.max_indexcov < Covering) SampleInfo.max_indexcov = Covering;
    SampleInfo.treecov  += Covering;
    SampleInfo.indexcov += Covering;

	/* do not collect separator and root statistics for subtree index nodes */
	if ( pt2InvSubtree_egg00 == m_pNode->nd_pt2() ) return;

    if ( m_pNode->nd_id() == m_Current.curr_tree_id.fileRoot_gg00() )
    {
        SampleInfo.rootcov = Covering;
        SampleInfo.hight = m_pNode->nd_level();
	}

	for (tsp00_Int4 CurrRecIndex = FIRST_REC_INDEX_BD00;
	     CurrRecIndex <= bd600MaxRecIndex();
	     ++CurrRecIndex)
	{
		pRec = bd600RecPtr (CurrRecIndex);
		if (e_ok != m_TrError ) return;

        SepLen = pRec->recKeyLen_gg00();

        ++StatAuxVars.sav_separators;
        SampleInfo.ave_sep_length += SepLen;

        if ( (SampleInfo.min_sep_length > SepLen) && (SepLen > 0) )
			SampleInfo.min_sep_length = SepLen;
        if ( SampleInfo.max_sep_length < SepLen )
			SampleInfo.max_sep_length = SepLen;
	}
}

/*---------------------------------------------------------------------------*/

void
cbd600_Node::bd600CountClusteredPages ( cbd500_Tree             &invTree,
                                        Converter_PageCollector &pageCollection,
                                        Data_PageNoList         &pageNoList,
                                        tsp00_Int4              &LeafPageCount,
                                        tsp00_Int4              &ClusteredBlockCount) const
{
    ROUTINE_DBG_MEO00 ("bd600CountClusteredPages");

    /*
    if ( ! m_Current.bd300ArePagesClustered() )
    {
        m_TrError = e_not_implemented;
        return;
    }
    */
    
    if ( bd600IsLeaf() && bd600IsRoot() )
    {
        ++LeafPageCount;
        ++ClusteredBlockCount;
        if ( m_pNode->ndLeafCount_bd00() > 1 )
            bd600CountClusteredPagesOfLeafNode(invTree, LeafPageCount, ClusteredBlockCount);
        return;
    }

    if ( bd600Level() != 1 )
        return;

    if ( ! pageCollection.Initialize() )
    {
        m_TrError = e_no_more_memory;
        return;
    }

    cbd520_Separator sep;
    for ( cbd600_Node::Iterator sepRecIter = bd600GetFirstRecord();
          sepRecIter.IsValid();
          ++sepRecIter )
    {
        sep.Assign(*sepRecIter);
        (void)pageCollection.AppendPageNo(sep.bd520SubNodeId()); // cannot return false because initialized before
        if ( sep.bd520LeafCount() > 1 )
        {
            // The referenced leaf contains one or more subtrees
            cbd600_Node leaf (m_Current, tbd_node_request::fromConst(nr_for_read));

            leaf.bd600GetNode (sep.bd520SubNodeId());
            if ( m_TrError != e_ok )
                return;
            leaf.bd600CountClusteredPagesOfLeafNode(invTree, LeafPageCount, ClusteredBlockCount);
        }
    }

    pageCollection.DetermineCluster( Trans_Context::GetContext(*m_Current.curr_trans).Task() );
              
    ClusteredBlockCount += pageCollection.GetClusterCount();
    LeafPageCount       += bd600NumRecords();
}


/*---------------------------------------------------------------------------*/

void
cbd600_Node::bd600CountClusteredPagesOfLeafNode ( cbd500_Tree &invTree,
                                                 tsp00_Int4  &LeafPageCount,
                                                 tsp00_Int4  &ClusteredBlockCount) const
{
    ROUTINE_DBG_MEO00 ("bd600CountClusteredPagesOfLeafNode");

    for ( cbd600_Node::Iterator recIter = bd600GetFirstRecord();
          recIter.IsValid();
          ++recIter )
    {
        if ( (*recIter)->recInvListKind_gg00() == ilkSubTree_egg00 )
        {
            // Determine cluster statistics of subtree
            cbd450_InvListRefSubTree invSubTreeRecord  (*recIter);
            cbd300_InvSubTreeCurrent InvSubTreeCurrent (m_Current,
                                                        tbd_node_request::fromConst(nr_for_read),
                                                        invSubTreeRecord,
                                                        bd600IsUpdateable());
            bool                     TreeIsRequested   = false;
            cbd601_SubTreeNode       SubTreeNode      (InvSubTreeCurrent, tbd_node_request::fromConst(nr_for_read));
            cbd500_SubTree           SubTree          (invTree, SubTreeNode, &TreeIsRequested);

            if ( m_TrError != e_ok )
                return;

            SubTree.bd500CountClusteredPages(LeafPageCount, ClusteredBlockCount);
        }
    }
}

/*---------------------------------------------------------------------------*/

void
cbd600_Node::bd600_GetInvLeafStatistic (tgg00_SampleInfo  &SampleInfo) const
{
    ROUTINE_DBG_MEO00 ("bd600GetInvLeafStatistic");

	const tsp00_Int4   MaxInt4Div100 = 21474836;
	const tsp00_Int4   Covering      = (100 * bd600Covering() + HALF_COVERING_BD00) / FULLCOVERING_BD00;
	tgg00_RecPtr       pRec;
    tsp00_Int4         SecKeyLen;
	tsp00_Int4         NumPrimKeys;
	tsp00_Int4         iAux;

	++SampleInfo.nodes;
	++SampleInfo.leafnodes;

	if (SampleInfo.min_leafcov > Covering) SampleInfo.min_leafcov = Covering;
	if (SampleInfo.max_leafcov < Covering) SampleInfo.max_leafcov = Covering;

	SampleInfo.treecov += Covering;
	SampleInfo.leafcov += Covering;

	for (tsp00_Int4 CurrRecIndex = FIRST_REC_INDEX_BD00;
	     CurrRecIndex <= bd600MaxRecIndex();
	     ++CurrRecIndex)
	{
		pRec = bd600RecPtr (CurrRecIndex);
		if (e_ok != m_TrError ) return;

		++SampleInfo.sec_key_cnt;
        SecKeyLen = pRec->recKeyLen_gg00();
        SampleInfo.avg_sec_key_len += SecKeyLen;

        if (SecKeyLen < SampleInfo.min_sec_key_len) SampleInfo.min_sec_key_len = SecKeyLen;
        if (SecKeyLen > SampleInfo.max_sec_key_len) SampleInfo.max_sec_key_len = SecKeyLen;

		if (ilkArray_egg00 == pRec->recInvListKind_gg00() )
		{
			NumPrimKeys = pRec->recNumPrimKeys_gg00();
		}
		else if (ilkSubTree_egg00 ==  pRec->recInvListKind_gg00() )
		{
			iAux               = SampleInfo.records;
			SampleInfo.records = 0;

			bd600_GetSubTreeStatistics (pRec, SampleInfo);

			NumPrimKeys        = SampleInfo.records;
			SampleInfo.records = iAux;
		}
		else
		{
			m_TrError = e_not_implemented;
			return;
		}

        SampleInfo.avg_prim_per_list += NumPrimKeys;
        if (NumPrimKeys < SampleInfo.min_prim_per_list) SampleInfo.min_prim_per_list = NumPrimKeys;
        if (NumPrimKeys > SampleInfo.max_prim_per_list) SampleInfo.max_prim_per_list = NumPrimKeys;
	}
}

/*---------------------------------------------------------------------------*/

void
cbd600_Node::bd600_GetInvLeafSelectivity (tgg00_SampleInfo  &SampleInfo) const
{
    ROUTINE_DBG_MEO00 ("bd600_GetInvLeafSelectivity");

	const tsp00_Int4   MaxInt4Div100 = 21474836;
	tgg00_RecPtr       pRec;
	tsp00_Int4         NumPrimKeys;
	tsp00_Int4         iAux;

	for (tsp00_Int4 CurrRecIndex = FIRST_REC_INDEX_BD00;
	     CurrRecIndex <= bd600MaxRecIndex();
	     ++CurrRecIndex)
	{
		pRec = bd600RecPtr (CurrRecIndex);
		if (e_ok != m_TrError ) return;

		if ( ilkArray_egg00 ==  pRec->recInvListKind_gg00() )
		{
			NumPrimKeys = pRec->recNumPrimKeys_gg00();
		}
		else if ( ilkSubTree_egg00 ==  pRec->recInvListKind_gg00() )
		{
			cbd450_InvListRefSubTree InvListRefSubTreeHandle (pRec);
			NumPrimKeys  = InvListRefSubTreeHandle.bd450NumPrimKeys();
		}
		else
		{
			m_TrError = e_not_implemented;
			return;
		}

		if (NumPrimKeys > MaxInt4Div100)
		{
			if ( (MAX_INT4_SP00 - (SampleInfo.avg_prim_per_list / 200)) > NumPrimKeys)
				iAux = (NumPrimKeys + (SampleInfo.avg_prim_per_list / 200)) / (SampleInfo.avg_prim_per_list / 100);
			else
				iAux = NumPrimKeys / (SampleInfo.avg_prim_per_list / 100);
		}
		else
		{
			if ( (MAX_INT4_SP00 - (SampleInfo.avg_prim_per_list >> 1)) > (NumPrimKeys * 100) )
				iAux = (NumPrimKeys * 100 + (SampleInfo.avg_prim_per_list >> 1)) / SampleInfo.avg_prim_per_list;
			else
				iAux = (NumPrimKeys * 100) / SampleInfo.avg_prim_per_list;
		}

		if      (iAux <= 1)  ++SampleInfo.invsel_1;
		else if (iAux <= 5)  ++SampleInfo.invsel_5;
		else if (iAux <= 10) ++SampleInfo.invsel_10;
		else if (iAux <= 25) ++SampleInfo.invsel_25;
		else                 ++SampleInfo.invsel_notsel;
	}
}


/*---------------------------------------------------------------------------*/

void
cbd600_Node::bd600_GetSubTreeLeafStatistic (tgg00_SampleInfo &SampleInfo) const
{
    ROUTINE_DBG_MEO00 ("bd600_GetSubTreeLeafStatistic");

	const tsp00_Int4 Covering = (100 * bd600Covering() + HALF_COVERING_BD00) / FULLCOVERING_BD00;

	++SampleInfo.nodes;
	++SampleInfo.leafnodes;

	if (SampleInfo.min_leafcov > Covering) SampleInfo.min_leafcov = Covering;
	if (SampleInfo.max_leafcov < Covering) SampleInfo.max_leafcov = Covering;

	SampleInfo.treecov += Covering;
	SampleInfo.leafcov += Covering;
	SampleInfo.records += bd600NumRecords();
}

/*---------------------------------------------------------------------------*/

void
cbd600_Node::bd600_GetSubTreeStatistics (tgg00_RecPtr      pRec,
									     tgg00_SampleInfo &SampleInfo) const
{
    ROUTINE_DBG_MEO00 ("bd600_GetSubTreeStatistics");

#   define WITH_SELECTIVITY true

	tbd_stat_aux_vars  StatAuxVars;
	/* The next line is possible, because SubTrees are only found in InvTrees and */
	/* InvTrees are only handled with cbd300_InvCurrent, this implies the current */
	/* node MUST be constructed with a cbd300_InvCurrent                          */
	cbd300_InvCurrent *AuxInvCurrent = REINTERPRET_CAST (cbd300_InvCurrent*, &m_Current);

	cbd450_InvListRefSubTree InvListRefSubTree (pRec);
	cbd300_InvSubTreeCurrent InvSubTreeCurrent (*AuxInvCurrent,
		                                        tbd_node_request::fromConst (nr_for_read),
												InvListRefSubTree,
												bd600IsUpdateable());
	if (e_ok != m_TrError) return;
	cbd600_Node SubTreeLeaf (InvSubTreeCurrent);
	cbd500_Tree Tree        (SubTreeLeaf);

	while ( (e_ok == m_TrError) && SubTreeLeaf.bd600IsAccessable())
	{
		SubTreeLeaf.bd600GetStatistic (SampleInfo, StatAuxVars, ! WITH_SELECTIVITY);
		Tree.bd500NextNode();
	}
}

/*---------------------------------------------------------------------------*/

void
cbd600_Node::bd600Dump (tsp00_Int4 ErrorId, const char *MsgText /*tsp00_C24*/) const
{
	b06write_filename_and_root (m_Current.curr_tree_id);

	if ( NULL == m_pNode )
		return;

    g01opmsg (sp3p_knldiag, sp3m_error,	ErrorId, csp3_n_btree, MsgText, m_pNode->nd_id());
    b06dump_bad_page (m_Current.curr_trans->trTaskId_gg00, 'd', FILE_EXT_COR_BD00,
		m_pNode->nd_id(), m_pNode, 1);
}


/*---------------------------------------------------------------------------*/

void
cbd600_Node::bd600CheckIndex() const
{

    ROUTINE_DBG_MEO00 ("bd600CheckIndex");

	tsp00_TaskId &TaskId        = m_Current.curr_trans->trTaskId_gg00;

    if ( (e_ok == m_TrError)
		 &&
		 (m_pNode->nd_level() > LEAF_LEVEL_BD00) )
	{
		cbd600_Node        SubNode (m_Current, tbd_node_request::fromConst (nr_for_read));
		cbd520_Separator   Separator;
		tsp00_KeyPtr       pFirstKey;
        tsp00_Int2         keyLength;
		tsp00_LcompResult  Lresult;

		for (tsp00_Int4 SepRecIndex = FIRST_REC_INDEX_BD00;
			 SepRecIndex <= bd600MaxRecIndex();
			 ++SepRecIndex)
		{
			Separator.Assign(bd600SepPtr(SepRecIndex));

            if ( bd600IsRoot()
                 &&
                 FIRST_REC_INDEX_BD00 == SepRecIndex
                 &&
                 Separator.bd520KeyLen() != 0 )
            {
				m_TrError = e_invalid_index_structure;
				bd600Dump (csp3_bd_msg, "bd600CheckIndex:Root0Key");
                break;
            }

            SubNode.bd600Move(Separator.bd520SubNodeId());
			if ( e_ok != m_TrError ) break;

			if ( Separator.bd520KeyLen() > 0
                 &&
                 SubNode.bd600NumRecords() > 0  || SubNode.m_pNode->nd_level() > LEAF_LEVEL_BD00 )
			{
				SubNode.bd600GetKey(FIRST_REC_INDEX_BD00, pFirstKey, keyLength);

				s30cmp ( Separator.bd520KeyPtr(), POS_OFF_DIFF_BD00, Separator.bd520KeyLen(),
                         pFirstKey,               POS_OFF_DIFF_BD00, keyLength,
                         Lresult );

				if ( Lresult == l_greater
                     ||
					 Lresult != l_equal && SubNode.m_pNode->nd_level() > LEAF_LEVEL_BD00 )
				{
#					if COMPILEMODE_MEO00 >= SLOW_MEO00 
					Separator.bd520Print();
					t01sname   (bd_index, "FirstKey:   ");
					t01moveobj (bd_index, pFirstKey, POS_OFF_DIFF_BD00, keyLength);
#					endif
					m_TrError = e_invalid_index_structure;
					bd600Dump         (csp3_bd_msg, "bd600CheckIndex:Keys  In");
					SubNode.bd600Dump (csp3_bd_msg, "bd600CheckIndex:Keys  Sn");
				    g01opmsg (sp3p_knldiag, sp3m_error, csp3_bd_msg,
					    csp3_n_btree, "CurrSepRecIndex:        ", SepRecIndex);
					break;
				}
			}

			/* Check LeafCount */
#           if COMPILEMODE_MEO00 >= SLOW_MEO00 
			t01p2int4 (bd_index, "SepLeafCount", Separator.bd520LeafCount(),
					             "NodLeafCount", SubNode.bd600LeafCount() );
			t01p2int4 (bd_index, "IndexNodeId ", m_NodePtrs.np_ptr()->nd_id(),
					             "SubNodeId   ", SubNode.bd600GetId() );
#           endif

            if ( ! SubNode.bd600IsLeaf() )
            {
                if ( Separator.bd520LeafCount() != SubNode.bd600LeafCount() )
			    {
				    m_TrError = e_invalid_index_structure;
				    bd600Dump         (csp3_bd_msg, "bd600CheckIndex:LeafC In");
				    SubNode.bd600Dump (csp3_bd_msg, "bd600CheckIndex:LeafC Sn");
				    g01opmsg (sp3p_knldiag, sp3m_error, csp3_bd_msg,
					    csp3_n_btree, "CurrSepRecIndex:        ", SepRecIndex);
				    break;
			    }
            }

		} // end of loops over records 

        /* check sum of leaf counts */
        if ( e_ok == m_TrError && g01glob.treecheck
             &&
             ! m_pNode->nd_pmode().includes(pmTemp_egg00) )
        {
			 bd600_CheckLeafCount();
        }
	}	
}

/*---------------------------------------------------------------------------*/

tsp00_Int4  
cbd600_Node::bd600LeafCount(const tsp00_Int4 StartRecIndex, 
			                const tsp00_Int4 StopRecIndex) const
{
    ROUTINE_DBG_MEO00 ("bd600LeafCount2");
	/* this function returns the number off all leaves referenced by */
	/* those records on the current node which have indices with     */
	/* StartRecIndex <= index <= StopRecIndex.                       */
    #if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01p2int4 (bd_inv, "Node Id     ", m_pNode->nd_id(),
                       "Node Type2  ", m_pNode->nd_pt2() );
    t01p2int4 (bd_inv, "StrtRecIndex", StartRecIndex,
                       "StopRecIndex", StopRecIndex );
    #endif

    const tsp00_Int4 MaxRecIndex = bd600MaxRecIndex();

    if ( StartRecIndex < 0 || StartRecIndex > MaxRecIndex ||
         StopRecIndex  < 0 || StopRecIndex  > MaxRecIndex )
        return 0;

    tsp00_Int4 SumNumLeaves = 0;
	
    if (LEAF_LEVEL_BD00 == m_pNode->nd_level())
    {
        if ( m_pNode->nd_pt2() != pt2InvSubtree_egg00 )
        {
            tgg00_RecPtr pRec;
    
            for (tsp00_Int4 RecIndex=StartRecIndex; RecIndex <= StopRecIndex; RecIndex++)
            {
                pRec = bd600RecPtr (RecIndex);

                if ( ilkSubTree_egg00 ==  pRec->recInvListKind_gg00() )
                {
                    cbd450_InvListRefSubTree InvListRefSubTreeHandle (pRec);
                    SumNumLeaves += InvListRefSubTreeHandle.bd450NumLeaves();
                }
            }
        }
    }
    else
    {
        for (tsp00_Int4 RecIndex=StartRecIndex; RecIndex <= StopRecIndex; RecIndex++)
        {
            cbd520_Separator Separator;

            Separator.Assign(bd600SepPtr (RecIndex));
            SumNumLeaves += Separator.bd520LeafCount();
        }
    }
    return SumNumLeaves;
}

/*---------------------------------------------------------------------------*/

tsp00_Int4  
cbd600_Node::bd600PrimKeyCount(const tsp00_Int4 StartRecIndex, 
						       const tsp00_Int4 StopRecIndex) const
{
    ROUTINE_DBG_MEO00 ("bd600PrimKeyCount");
	/* this function returns the number off all primary keys referenced */ 
	/* by those records on the current node which have indices with     */
	/* StartRecIndex <= index <= StopRecIndex.                          */

    if (LEAF_LEVEL_BD00 != m_pNode->nd_level())
    {	
        m_TrError = e_data_page_corrupted;
        g01opmsg (sp3p_knldiag, sp3m_error,	csp3_bd_msg, 
            csp3_n_btree, "b600PKeyCnt:NoLeaf  Pno=", m_pNode->nd_id());
        return 0;
    }
	
    tsp00_Int4 PrimKeyCount = 0;
	
	tgg00_RecPtr pRec;
	
	for (tsp00_Int4 RecIndex=StartRecIndex; RecIndex <= StopRecIndex; ++RecIndex)
	{
		pRec = bd600RecPtr (RecIndex);
		if (e_ok != m_TrError ) {return PrimKeyCount;};
		
		switch (pRec->recInvListKind_gg00())
		{
		case (ilkArray_egg00):
			PrimKeyCount += pRec->recNumPrimKeys_gg00();
			break;
			
		case (ilkSubTree_egg00):
			{
				cbd450_InvListRefSubTree InvListRefSubTreeHandle (pRec);
				PrimKeyCount += InvListRefSubTreeHandle.bd450NumPrimKeys();
			}
			break;
			
		default:
			m_TrError = e_data_page_corrupted;
			g01opmsg (sp3p_knldiag, sp3m_error, bd600c1WrongKeyOrder_csp03,
				csp3_n_btree, "b600PKeyCnt:WrongILKind", m_pNode->nd_id());
			g01opmsg (sp3p_knldiag, sp3m_error, bd600c1WrongKeyOrder_csp03,
				csp3_n_btree, "b600PKeyCnt:WrongILKind", RecIndex);
			break;
		}
		
	}
	
	return PrimKeyCount;
}

/*---------------------------------------------------------------------------*/

void cbd600_Node::bd600MigrateSubTreeLeaf()
{
    ROUTINE_DBG_MEO00 ("bd600MigrateSubTreeLeaf");

    tbd_node          auxPage;
    cbd600_Node       auxNode (m_Current, auxPage);
    SAPDB_Byte       *pSpace;
    tsp00_KeyPtr      pKey;
    SAPDB_Int2        keyLength;
    const tsp00_Int4  MaxRecIndex = bd600MaxRecIndex();
    Msg_List         &errlist = Trans_Context::GetContext(*(m_Current.curr_trans)).ErrorList();
    Kernel_VTrace     trace;

    if ( DataIndex_Trace.TracesLevel(1) )
        trace << "bd600MigrateSubTreeLeaf: (pre)  "
              << "Id: "         << m_pNode->nd_id()
              << ", pt2: "      << m_pNode->nd_pt2()
              << ", level: "    << m_pNode->nd_level()
              << ", bottom: "   << m_pNode->nd_bottom()
              << ", recCount: " << m_pNode->nd_record_cnt()
              << FlushLine;

    // 1. make copy to work on

    if ( 0 == SAPDB_MemCopy( &auxPage, m_pNode, sizeof(tbd_node), "VBD600", __CONTEXT__, errlist) )
    {
        m_TrError = e_move_error;
        return;
    }

    auxPage.ndPageVersion_bd00() = 1;
    auxPage.nd_record_cnt()      = 0;
    auxPage.nd_bottom()          = BODY_BEG_BD00;

    // 2. copy all keys from old page to buffer

    for (SAPDB_Int recindex = FIRST_REC_INDEX_BD00; recindex <= MaxRecIndex; ++recindex)
    {
        bd600GetKey(recindex, pKey, keyLength);
        auxNode.bd600AddRecordSpace (keyLength+POINTERSIZE_BD00, recindex, pSpace, true, false);
        if ( e_ok != m_TrError )
            return;
        if ( 0 == SAPDB_MemCopy( pSpace, pKey, keyLength, "VBD600", __CONTEXT__, errlist) )
        {
            m_TrError = e_move_error;
            return;
        }
    }

    // 3. overwrite original with successfully migrated copy

    if ( 0 == SAPDB_MemCopy( m_pNode, &auxPage, sizeof(tbd_node), "VBD600", __CONTEXT__, errlist) )
    {
        m_TrError = e_move_error;
        return;
    }
    if ( DataIndex_Trace.TracesLevel(1) )
        trace << "bd600MigrateSubTreeLeaf: (post) "
              << "Id: "         << m_pNode->nd_id()
              << ", pt2: "      << m_pNode->nd_pt2()
              << ", level: "    << m_pNode->nd_level()
              << ", bottom: "   << m_pNode->nd_bottom()
              << ", recCount: " << m_pNode->nd_record_cnt()
              << FlushLine;
    if ( DataIndex_Check.ChecksLevel(6) )
        bd600Check(true);
}    

/*---------------------------------------------------------------------------*/

void cbd600_Node::bd600Print (bool withBody) const
{
    Kernel_VTrace trace;
    tsp00_KeyPtr  pKey;
    tsp00_Int2    keyLength;
    
    trace << "NodeId: "        << m_pNode->nd_id()
          << ", Type2: "       << m_pNode->nd_pt2()
          << ", PageVersion: " << m_pNode->ndPageVersion_bd00()
          << ", Level: "       << m_pNode->nd_level()
          << ", NodeAddr: "    << SAPDB_ToString(m_pNode) << FlushLine;
    trace << "Bottom: " << m_pNode->nd_bottom() << ", RecordCount: " << m_pNode->nd_record_cnt() << FlushLine;
    trace << ((m_pCachedPage!=0)?"Caching enabled: ":"Caching disabled: ") << FlushLine;

    if ( ! withBody )
        return;

    for (SAPDB_Int4 CurrRecIndex = FIRST_REC_INDEX_BD00;
         CurrRecIndex <= bd600MaxRecIndex();
         CurrRecIndex++)
    {
        m_RecordList.GetConstKey(CurrRecIndex, keyLength, pKey);    

        trace << "RecIndex["     << CurrRecIndex
              << "]: Position: " << m_RecordList.GetPosition(CurrRecIndex)
              << ", Length: "    << m_RecordList.GetLength(CurrRecIndex)
              << ", Key: "       << SAPDB_ToString( pKey, keyLength > sizeof(*m_pNode) ? sizeof(*m_pNode) : keyLength, _T_H )
              << FlushLine;
    }
}

/*---------------------------------------------------------------------------*/

cbd600_Node::~cbd600_Node ()
{
    ROUTINE_DBG_MEO00 ("cbd600_~Node");

#	ifdef SAPDB_SLOW 
    t01int4 (bd_inv, "Node:       ", (m_pNode != NULL)?(m_pNode->nd_id()):(NIL_PAGE_NO_GG00));
#	endif

    if ( e_ok != m_TrError
         &&
         DataIndex_Check.ChecksLevel(9)
         &&
         m_Current.bd300_IsInvError (m_TrError) )
    {
        bd600Dump (csp3_bd_msg, "~cbd600_Node: ERROR     ");
        bd600Print(true);
        g01abort (csp3_bd_msg, csp3_n_btree, "INDEX ERROR CHECKLEVEL 9", m_TrError);
    }

	bd600Release (nr_for_update == m_RequestKind);

	if ( e_page_in_wrong_tree == m_TrError)
    {
        bd600Dump (csp3_bd_msg, "~cbd600_Node: PageInWron");
    }

#	ifdef SAPDB_SLOW 
	if ( e_ok != m_TrError)
		t01basis_error (bd_inv, "TrError     ", m_TrError);
#	endif
}

