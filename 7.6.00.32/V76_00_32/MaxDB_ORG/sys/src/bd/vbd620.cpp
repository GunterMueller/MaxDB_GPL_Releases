/****************************************************************************

  module      : vbd620.cpp

  -------------------------------------------------------------------------

  responsible : UweH

  special area: OverflowHandling and UnderflowHandling within a Node
  description : 


  last changed: 1999-09-15  13:44
  see also    : example.html ...

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 1998-2005 SAP AG



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


*****************************************************************************/

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "ggg00.h"
#include "gbd00.h"
#include "gbd500.h"
#include "gbd600.h"
#include "gbd610.h"
#include "hbd102.h"  // PTS 1103980 JA 1999-09-15
#include "hbd06.h"
#include "hgg01_1.h" // g01opmsg
#include "SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp" // Kernel_move_and_fill
#include "Transaction/Trans_Context.hpp"
#include "RunTime/RTE_Message.hpp"
#include "DataAccess/Data_Types.hpp"

#if COMPILEMODE_MEO00 >= SLOW_MEO00 
#include "hta99.h"
#endif
 
#define RIGHT_DISTRIBUTION_BD620    true
#define LEFT_DISTRIBUTION_BD620   ! RIGHT_DISTRIBUTION_BD620

tsp00_Int4   
cbd600_Node::bd620_CalcBlockLenAndSetIndexToNextBlock (const cbd610_RecordList &AuxRecordList,
										               tsp00_Int4  &CurrIndex, 
										               tsp00_Int4   MaxIndex)     
 {
    ROUTINE_DBG_MEO00 ("bd620_CalcBlockLenAndSetIndexToNextBlock");

	/* This function calculates the length of the block starting at the CurrIndex record */
	/* of records which follow each other in the pointerlist as well as in the record part  */
	/* of thge page. Moreover CurrIndex is set to the first record of the next block         */

    if (CurrIndex < MaxIndex)
        return 0;

    const tsp00_Int4 FirstRecOffset = AuxRecordList.GetOffset(CurrIndex);
          tsp00_Int4 BlockLen       = 0;
          tsp00_Int4 NextRecOffset;
		
    while (CurrIndex > MaxIndex)
    {
        BlockLen      += bd102RecAlign(AuxRecordList.GetLength(CurrIndex));
		NextRecOffset  = AuxRecordList.GetOffset(CurrIndex - 1);

        if (FirstRecOffset + BlockLen != NextRecOffset)
            break;
            
        --CurrIndex;
    }
    
	if (CurrIndex == MaxIndex)
        BlockLen += bd102RecAlign(AuxRecordList.GetLength(CurrIndex));
        
    --CurrIndex;
    
    return BlockLen;
}
 
/*---------------------------------------------------------------------------*/

void
cbd600_Node::bd620_DeleteRecordsFromPage (bool		 bRightDistribution,
										  tsp00_Int4 RecIndex)
{
	/* if bRightDistribution = true this routine removes all   */
	/* records from the current page with Indexes <= RecIndex. */
	/* if if bRightDistribution = true 	all records with       */
	/* Indexes >= RecIndex are removed                         */

    ROUTINE_DBG_MEO00 ("bd620_DeleteRecordsFromPage");
    
	tbd_node          Buffer;               // must be aligned for pointers
	cbd600_Node       AuxNode (m_Current, Buffer);
    cbd610_RecordList &AuxRecordList = AuxNode.m_RecordList;
    tsp00_Int4        DestOffset;
    tsp00_Int4        SrcOffset;
    tsp00_Int4        ShiftLen;
    tsp00_Int4        SumShiftLen       = 0;
    tsp00_Int4        MoveLen;
    tsp00_Int4        NumDistribIndexes;
    tsp00_Int4        CurrMoveIndex;
    tsp00_Int4        MaxMoveIndex;
    tsp00_Int4        MinMoveIndex;
    tsp00_Int4        CurrDistIndex;
    tsp00_Int4        MaxDistIndex;

    AuxNode.m_pNode->ndPageVersion_bd00() = m_pNode->ndPageVersion_bd00(); 
    AuxNode.m_pNode->nd_pt2()             = m_pNode->nd_pt2(); 
    AuxNode.m_pNode->nd_id()              = NIL_PAGE_NO_GG00; 
    AuxNode.bd600CopyBody(*this);
    if (e_ok != m_TrError)
        return;

	if (bRightDistribution)
	{
		if ( RecIndex > bd600MaxRecIndex() )
            return; /* nothing to remove */
		
		if (RecIndex > FIRST_REC_INDEX_BD00)
			MinMoveIndex =  RecIndex - 1;
		else 
		{
			/* remove all records from page */
			m_pNode->nd_bottom()     = BODY_BEG_BD00;
			m_pNode->nd_record_cnt() = 0;
			return;
		}
		
		NumDistribIndexes = bd600MaxRecIndex() - RecIndex + 1;
		CurrDistIndex     = bd600MaxRecIndex();
		MaxDistIndex      = RecIndex;
		MaxMoveIndex      = FIRST_REC_INDEX_BD00;
		CurrMoveIndex     = MinMoveIndex;
		
        AuxRecordList.SortForDistribution (MaxDistIndex);
	}
	else
	{
		if (RecIndex < FIRST_REC_INDEX_BD00)
            return; /* nothing to remove */
		
		if (RecIndex < bd600MaxRecIndex())
			MaxMoveIndex  = RecIndex + 1;
		else 
		{
			/* remove all records from page */
			m_pNode->nd_bottom()     = BODY_BEG_BD00;
			m_pNode->nd_record_cnt() = 0;
			return;
		}
		NumDistribIndexes = RecIndex + 1;
		CurrDistIndex     = RecIndex;
		MaxDistIndex      = FIRST_REC_INDEX_BD00;
		MinMoveIndex      = bd600MaxRecIndex();
		CurrMoveIndex     = MinMoveIndex;
		
        AuxRecordList.SortForDistribution (RecIndex + 1);
	}
	
	/* set DestOffset to the first beginning of the first block of connected records to be deleted */
	if ( AuxRecordList.GetOffset(CurrDistIndex) > AuxRecordList.GetOffset(CurrMoveIndex) )
	{
		DestOffset = BODY_BEG_BD00 - POS_OFF_DIFF_BD00
                     +
			         bd620_CalcBlockLenAndSetIndexToNextBlock ( AuxRecordList,
                                                                CurrMoveIndex,
                                                                MaxMoveIndex );
		if (e_ok != m_TrError)
            return;
	}
	else
        DestOffset = BODY_BEG_BD00 - POS_OFF_DIFF_BD00;

    while (CurrDistIndex >= MaxDistIndex)
    {
		/* position of the next block, consisting of records, to move */
		if (CurrMoveIndex >= MaxMoveIndex)
			SrcOffset  = AuxRecordList.GetOffset(CurrMoveIndex);

		/* length of the next block to be deleted */
		ShiftLen = bd620_CalcBlockLenAndSetIndexToNextBlock (AuxRecordList, CurrDistIndex, MaxDistIndex);
		if (e_ok != m_TrError)
            return;

		/* sum up shift lengths */
		SumShiftLen += ShiftLen;

		/* length of the next block to move */
        MoveLen = bd620_CalcBlockLenAndSetIndexToNextBlock (AuxRecordList, CurrMoveIndex, MaxMoveIndex);
		if (e_ok != m_TrError)
            return;
        
		/* stop if there is nothing more to move */
        if (0 == MoveLen)
		{
#           if COMPILEMODE_MEO00 >= SLOW_MEO00 
			if (CurrMoveIndex >= MaxMoveIndex)
			{
				g01opmsg (sp3p_knldiag, sp3m_error, csp03_b31_2_illegal_entrypos,
					csp3_n_btree, "VBD620:rec not moved    ", CurrMoveIndex);
				g01opmsg (sp3p_knldiag, sp3m_error, csp03_b31_2_illegal_entrypos,
					csp3_n_btree, "VBD620:rec not moved    ", MaxMoveIndex);
			}
#           endif
			break;
		}
		
		/* check if the source position is equal to the destination */
		/* plus the sum of the lengths of the record deleted so far */ 
#       if COMPILEMODE_MEO00 >= SLOW_MEO00 
		if (SrcOffset != DestOffset + SumShiftLen)
		{
			g01opmsg (sp3p_knldiag, sp3m_error, csp03_b31_2_illegal_entrypos,
				csp3_n_btree, "VBD620: wrong record pos", DestOffset);
			g01opmsg (sp3p_knldiag, sp3m_error, csp03_b31_2_illegal_entrypos,
				csp3_n_btree, "VBD620:  old  record pos", SrcOffset);
			g01opmsg (sp3p_knldiag, sp3m_error, csp03_b31_2_illegal_entrypos,
				csp3_n_btree, "VBD620:  wanted shiftlen", SumShiftLen);
	        g01opmsg (sp3p_knldiag, sp3m_error, csp03_b31_2_illegal_entrypos,
				csp3_n_btree, "VBD620: DelRecIndex     ", RecIndex);
		}
#       endif
		
		/* move the next block to the position of the deleted block of records */
		SAPDB_RangeOverlappingMove( __FILE__, 1,    
			sizeof (*m_pNode), sizeof(*m_pNode),
			m_pNode, POS_OFF_DIFF_BD00 + SrcOffset, 
			m_pNode, POS_OFF_DIFF_BD00 + DestOffset,
			MoveLen, m_TrError);

		if (e_ok != m_TrError)
            return;

        m_RecordList.UpdatePositionsRange (MaxMoveIndex, MinMoveIndex,
			SrcOffset, SrcOffset + MoveLen, - SumShiftLen);

		if (e_ok != m_TrError)
            return;
		
		DestOffset += MoveLen;
    }

	m_pNode->nd_bottom() -= SumShiftLen;

	if ( bRightDistribution)
		m_pNode->nd_record_cnt() -= NumDistribIndexes;
	else
    {
        if ( ! m_RecordList.DelHighIndexPositions (NumDistribIndexes,
                                                   Trans_Context::GetContext(*(m_Current.curr_trans)).ErrorList()) )
        {
            m_TrError = e_data_page_corrupted;
            #ifdef SAPDB_SLOW
            RTE_Crash
            #else
            RTE_Message
            #endif
            (Trans_Context::GetContext(*(m_Current.curr_trans)).ErrorList());
            if ( 0 == m_pNode->nd_record_cnt() )
                g01opmsg (sp3p_knldiag, sp3m_error, gbd610c5DataPageCorrupted_csp03,
                    csp3_n_btree, "Pointerlist is corrupted", MoveLen);
            else
                g01opmsg (sp3p_knldiag, sp3m_error, gbd610c4DataPageCorrupted_csp03,
                    csp3_n_btree, "Data page is empty  ", NumDistribIndexes);
            b06dump_bad_page (m_Current.curr_trans->trTaskId_gg00, 'd',
                FILE_EXT_COR_BD00, m_pNode->nd_id(), m_pNode, 1);
            b06write_filename_and_root (m_Current.curr_tree_id);
        }
    }

    #ifdef SAPDB_SLOW 
    if ( DataIndex_Trace.TracesLevel(6) )
        m_RecordList.Print();
    #endif
}


/*---------------------------------------------------------------------------*/

void
cbd600_Node::bd620_FindOptimDistribForDelete(cbd600_Node &Neighbor,
											 bool         bRightDistribution,
											 tsp00_Int4   Covering,
											 tsp00_Int4   NeighborCovering,
											 bool        &bDistributionFound,
											 tsp00_Int4  &OptimRecIndex)
{
	/* OptimRecIndex is to move inclusively */

    ROUTINE_DBG_MEO00 ("bd620_FindOptimDistribForDelete");

    const tsp00_Int4 OptimOverflow  = NeighborCovering - ((Covering + NeighborCovering) >> 1);

          tsp00_Int4 StartRecIndex;
          tsp00_Int4 StopRecIndex;
          tsp00_Int4 CurrRecIndex;
          tsp00_Int4 Step;
          tsp00_Int4 Diff;
          tsp00_Int4 OptimDiff      = MAX_INT4_SP00;
          tsp00_Int4 SumOverFlow    = 0;

    bDistributionFound = false;
        
    /* set ranges */
    if (bRightDistribution)
    {
        Step          = 1;
        StartRecIndex = FIRST_REC_INDEX_BD00;
		StopRecIndex  = Neighbor.bd600MaxRecIndex() + 1;
    }
    else
    {
        Step          = -1;
        StartRecIndex = Neighbor.bd600MaxRecIndex();
		StopRecIndex  = FIRST_REC_INDEX_BD00 - 1;
    }
    
    for (CurrRecIndex = StartRecIndex; CurrRecIndex != StopRecIndex; CurrRecIndex += Step)
    {
        SumOverFlow += (bd102RecAlign(Neighbor.m_RecordList.GetLength(CurrRecIndex)) + POINTERSIZE_BD00);
        
        if (Covering + SumOverFlow > FULLCOVERING_BD00) break;
        
        Diff = abs (OptimOverflow - SumOverFlow); 

        if (Diff >= OptimDiff) break;

        OptimRecIndex      = CurrRecIndex;
        OptimDiff          = Diff;
        bDistributionFound = true;
    }
    
	
#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
	t01bool   (bd_oflw, "distr found ", bDistributionFound);
	if (bDistributionFound)
	{
		t01int4   (bd_oflw, "break index ", OptimRecIndex);
		t01p2int4 (bd_oflw, "Overflow    ", SumOverFlow, "New Cov Diff", 2 * OptimDiff);		 
	}
#   endif
}    

/*---------------------------------------------------------------------------*/

void
cbd600_Node::bd620_FindOptimDistribForInsert (bool              bRightDistribution,     //false : load balancing with left neighbor otherwise with the right one 
											  tsp00_Int4        RequiredLen,
											  tsp00_Int4        NeighborCovering,
										      tsp00_Int4        RecIndex,
										      bool             &bDistributionFound,
											  tsp00_Int4       &OptimRecIndex,
											  bool             &bMoveNewRecordIntoNeighbor)
{
    ROUTINE_DBG_MEO00 ("bd620_FindOptimDistribForInsert");

    const tsp00_Int4 Covering      = bd600Covering();
    const tsp00_Int4 OptimOverflow = Covering + RequiredLen - ((Covering + NeighborCovering + RequiredLen) >> 1);
    const tsp00_Int4 SumRecLen     = Covering + RequiredLen;
    bool bOptimDistributionFound   = false;
    bool bNoMoreDistribution       = false;
    tsp00_Int4   StartRecIndex;
    tsp00_Int4   StopRecIndex;
    tsp00_Int4   AuxRecIndex;
    tsp00_Int4   Step;
    tsp00_Int4   Diff;
    tsp00_Int4   OptimDiff         = MAX_INT4_SP00;
    tsp00_Int4   SumOverFlow       = 0;

    bDistributionFound         = false;
    bMoveNewRecordIntoNeighbor = false;
        
    /* set ranges */
    if (bRightDistribution)
    {
        Step          = -1;
        StartRecIndex = bd600MaxRecIndex();
        StopRecIndex  = RecIndex - 1;
    }
    else
    {
        Step          = 1;
        StartRecIndex = FIRST_REC_INDEX_BD00;
        StopRecIndex  = RecIndex;
    }
    
	/* check all records which have to be moved into neighbor before the new record is moved */
    for (AuxRecIndex = StartRecIndex; AuxRecIndex != StopRecIndex; AuxRecIndex += Step)
    {
        SumOverFlow += bd102RecAlign(m_RecordList.GetLength(AuxRecIndex)) + POINTERSIZE_BD00;
        
        if (SumRecLen - SumOverFlow < FULLCOVERING_BD00 )
        {
            if (NeighborCovering + SumOverFlow > FULLCOVERING_BD00)
            {
                bNoMoreDistribution = true;
                break;
            }
            
            Diff = abs (OptimOverflow - SumOverFlow); 
            if(Diff < OptimDiff) 
            {
                OptimRecIndex      = AuxRecIndex;
                OptimDiff          = Diff;
                bDistributionFound = true;
            }
            else
            {
                bOptimDistributionFound = true;
                break;
            }
        }
    }
    
	/* check if the new record has to be moved into neighbor */
    if (!bOptimDistributionFound && !bNoMoreDistribution)
    {
        SumOverFlow += RequiredLen;	
		
		if (NeighborCovering + SumOverFlow > FULLCOVERING_BD00)
			bNoMoreDistribution = true;
		else
		{
			Diff = abs (OptimOverflow - SumOverFlow); 
			if(Diff < OptimDiff) 
			{
				if (bRightDistribution)
					OptimRecIndex          = RecIndex;
				else
					OptimRecIndex          = RecIndex - 1;
				OptimDiff                  = Diff;
				bDistributionFound         = true;
				bMoveNewRecordIntoNeighbor = true;
				
			}
			else
				bOptimDistributionFound = true;
		}
		
		/* check remaining records */
		if (!bOptimDistributionFound && !bNoMoreDistribution)
		{			
			if (bRightDistribution)
			{
				StartRecIndex = StopRecIndex;
				StopRecIndex  = FIRST_REC_INDEX_BD00 - 1;
			}
			else
			{
				StartRecIndex = StopRecIndex;
				StopRecIndex  = bd600MaxRecIndex() + 1;
			}
			
			for (AuxRecIndex = StartRecIndex; AuxRecIndex != StopRecIndex; AuxRecIndex += Step)
			{
				SumOverFlow +=  (bd102RecAlign(m_RecordList.GetLength(AuxRecIndex)) + POINTERSIZE_BD00);
				
				if (NeighborCovering + SumOverFlow > FULLCOVERING_BD00) 
					break;
				
				Diff = abs (OptimOverflow - SumOverFlow); 
				if(Diff < OptimDiff) 
				{
					OptimRecIndex              = AuxRecIndex;
					OptimDiff                  = Diff;
					bDistributionFound         = true;
				}
				else
				{
					bOptimDistributionFound = true;
					break;
				}
			}
		}
    }
#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
	t01bool   (bd_oflw, "distr found ", bDistributionFound);
	if (bDistributionFound)
	{
		t01int4   (bd_oflw, "break index ", OptimRecIndex);
		t01bool   (bd_oflw, "rec to neigh", bMoveNewRecordIntoNeighbor);
		t01p2int4 (bd_oflw, "Overflow    ", SumOverFlow, "New Cov Diff", 2 * OptimDiff);		 
	}
#   endif
}    

/*---------------------------------------------------------------------------*/

void
cbd600_Node::bd620_FindOptimDistribForUpdate (bool              bRightDistribution,     //false : load balancing with left neighbor otherwise with the right one 
											  tsp00_Int4        RequiredLen,
										      tsp00_Int4        NeighborCovering,
											  tsp00_Int4        RecIndex,
										      bool             &bDistributionFound,
											  tsp00_Int4       &OptimRecIndex,
    										  bool             &bMoveRecordIntoNeighbor)
{
    ROUTINE_DBG_MEO00 ("bd620_FindOptimDistribForUpdate");

    const tsp00_Int4          Covering                = bd600Covering();
    const tsp00_Int4          OptimOverflow           = Covering + RequiredLen - ((Covering + NeighborCovering + RequiredLen) >> 1);
    const tsp00_Int4          SumRecLen               = Covering + RequiredLen;
    bool                      bOptimDistributionFound = false;
    bool                      bNoMoreDistribution     = false;
    tsp00_Int4                StartRecIndex;
    tsp00_Int4                StopRecIndex;
    tsp00_Int4                AuxRecIndex;
    tsp00_Int4                Step;
    tsp00_Int4                Diff;
    tsp00_Int4                OptimDiff              = MAX_INT4_SP00;
    tsp00_Int4                SumOverFlow            = 0;

    bDistributionFound      = false;
	bMoveRecordIntoNeighbor = false;
        
    /* set ranges */
	StopRecIndex  = RecIndex;
    if (bRightDistribution)
    {
        Step          = -1;
        StartRecIndex = bd600MaxRecIndex();
    }
    else
    {
        Step          = 1;
        StartRecIndex = FIRST_REC_INDEX_BD00;
    }
    
    for (AuxRecIndex = StartRecIndex; AuxRecIndex != StopRecIndex; AuxRecIndex += Step)
    {
        SumOverFlow +=  (bd102RecAlign(m_RecordList.GetLength(AuxRecIndex)) + POINTERSIZE_BD00);
        
        if (SumRecLen - SumOverFlow < FULLCOVERING_BD00 )
        {
            if (NeighborCovering + SumOverFlow > FULLCOVERING_BD00)
            {
                bNoMoreDistribution = true;
                break;
            }
            
            Diff = abs (OptimOverflow - SumOverFlow); 
            if(Diff < OptimDiff) 
            {
                OptimRecIndex      = AuxRecIndex;
                OptimDiff          = Diff;
                bDistributionFound = true;
            }
            else
            {
                bOptimDistributionFound = true;
                break;
            }
        }
    }
    
    if (!bOptimDistributionFound && !bNoMoreDistribution)
    {
        SumOverFlow += RequiredLen;
        
		if (bRightDistribution)
		{
			StartRecIndex = StopRecIndex;
			StopRecIndex  = FIRST_REC_INDEX_BD00 - 1;
		}
		else
		{
			StartRecIndex = StopRecIndex;
			StopRecIndex  = bd600MaxRecIndex() + 1;
		}
		
        for (AuxRecIndex = StartRecIndex; AuxRecIndex != StopRecIndex; AuxRecIndex += Step)
        {
            SumOverFlow +=  (bd102RecAlign(m_RecordList.GetLength(AuxRecIndex)) + POINTERSIZE_BD00);
            
			if (NeighborCovering + SumOverFlow > FULLCOVERING_BD00) 
				break;
			
			Diff = abs (OptimOverflow - SumOverFlow); 
			if(Diff < OptimDiff) 
			{
				OptimRecIndex           = AuxRecIndex;
				OptimDiff               = Diff;
				bDistributionFound      = true;
				bMoveRecordIntoNeighbor = true;
				
			}
			else
			{
				bOptimDistributionFound = true;
				break;
			}
        }
    }
	
#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
	t01bool   (bd_oflw, "distr found ", bDistributionFound);
	if (bDistributionFound)
	{
		t01int4   (bd_oflw, "break index ", OptimRecIndex);
		t01bool   (bd_oflw, "rec to neigh", bMoveRecordIntoNeighbor);
		t01p2int4 (bd_oflw, "Overflow    ", SumOverFlow, "New Cov Diff", 2 * OptimDiff);		 
	}
#   endif
}    

/*---------------------------------------------------------------------------*/

void
cbd600_Node::bd620_MergeNodes (cbd600_Node &LeftNode)
{
	ROUTINE_DBG_MEO00 ("bd620_MergeNodes");

	const tsp00_Int4  MoveLen     = m_pNode->nd_bottom()   - BODY_BEG_BD00;
	const tsp00_Int4  RecPosDelta = LeftNode.m_pNode->nd_bottom() - BODY_BEG_BD00;
	
#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
	t01p2int4 (bd_oflw, "MoveLen     ", MoveLen, "RecPosDelta ", RecPosDelta);
	if (LeftNode.m_pNode->nd_right() != m_pNode->nd_id())
	{
		g01opmsg (sp3p_knldiag, sp3m_error, csp03_b31_2_illegal_entrypos,
			csp3_n_btree, "VBD620: Wrong Merge Dest", LeftNode.m_pNode->nd_id());
		g01opmsg (sp3p_knldiag, sp3m_error, csp03_b31_2_illegal_entrypos,
			csp3_n_btree, "VBD620: Wrong Merge Src ", m_pNode->nd_id());
		g01opmsg (sp3p_knldiag, sp3m_error, csp03_b31_2_illegal_entrypos,
			csp3_n_btree, "VBD620: Dest->right ", LeftNode.m_pNode->nd_right());
	}
#   endif

	/* Move all records from src node to dest node */
	SAPDB_RangeMove( __FILE__, 2,    
		sizeof (*m_pNode), sizeof (*LeftNode.m_pNode),
		m_pNode,          BODY_BEG_BD00,
		LeftNode.m_pNode, LeftNode.m_pNode->nd_bottom(),
		MoveLen, m_TrError);

	if (e_move_error == m_TrError)
	{
		m_TrError = e_data_page_corrupted;
		LeftNode.bd600Dump (csp3_bd_msg, "bd620_MergeNodes        ");
		return;
	}
	
	LeftNode.m_pNode->nd_bottom() += MoveLen; // MoveLen is aligned
	
	/* rebuild the new PointerList of merged node */
	if ( ! LeftNode.m_RecordList.Merge (m_RecordList, RecPosDelta,
                                        Trans_Context::GetContext(*(m_Current.curr_trans)).ErrorList()) )
    {
        m_TrError = e_move_error;
        #ifdef SAPDB_SLOW
        RTE_Crash
        #else
        RTE_Message
        #endif
        (Trans_Context::GetContext(*(m_Current.curr_trans)).ErrorList());
        LeftNode.bd600Dump (csp3_bd_msg, "bd620_MergeNodes2       ");
        return;
    }

	m_pNode->nd_bottom()        = BODY_BEG_BD00;
	m_pNode->nd_record_cnt()    = 0;

	/* update the LeafCount of the TargetNode and the SourceNode */
	/* PTS 1103885 AK 02-09-1999 */
	if (LEAF_LEVEL_BD00 == LeftNode.m_pNode->nd_level()) 
	{
		LeftNode.m_pNode->ndLeafCount_bd00() += (m_pNode->ndLeafCount_bd00() - 1);
		m_pNode->ndLeafCount_bd00() = 1;
	}
	else
	{
		LeftNode.m_pNode->ndLeafCount_bd00() += m_pNode->ndLeafCount_bd00();
		m_pNode->ndLeafCount_bd00() = 0;
	}
}
