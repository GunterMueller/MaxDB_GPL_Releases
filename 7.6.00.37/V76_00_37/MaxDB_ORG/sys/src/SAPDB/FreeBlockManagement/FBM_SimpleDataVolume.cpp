/*!**************************************************************************

  module      : FBM_SimpleDataVolume.cpp

  -------------------------------------------------------------------------

  responsible : TorstenS

  auhtor      : AlexanderK

  special area: FreeBlockManagement (FBM)
  description :


  last changed: 2000-03-10  10:00
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

#include "FreeBlockManagement/FBM_SimpleDataVolume.hpp"
#include "FreeBlockManagement/FBM_Dump.hpp"
#include "FreeBlockManagement/FBM_Exception.hpp"
#include "FreeBlockManagement/FBM_Messages.hpp"

#include "Pager/Pager_Controller.hpp"

#include "IOManager/IOMan_ComplexClusterAddress.hpp"

#include "KernelCommon/Kernel_Dump.hpp"

#include "RunTime/RTE_Crash.hpp"
#include "RunTime/RTE_Message.hpp"


/*===========================================================================*
 *  DEFINES                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASS METHOD DEFINITION                                                  *
 *===========================================================================*/

/*---------------------------------------------------------------------------*
 *  CLASS FBM_SimpleDataVolume                                              *
 *---------------------------------------------------------------------------*/

bool
FBM_SimpleDataVolume::InitVolume (const IOMan_ClusterCount numCluster)
{
    SAPDBTRACE_ROUTINE_DEBUG ("FBM_SimpleDataVolume::InitVolume", FBM_Trace, 5);

    FBM_DataVolume::InitVolume(numCluster);

    m_logClusterSize           = 0;

    // calculare log2 of m_clusterSize
    SAPDB_UInt divisor = m_clusterSize;
    while (divisor != 1)
    {
        divisor /= 2;
        m_logClusterSize++;
    }

    /* set members to their start values */
    m_FirstFreeBlock           = 0;
    m_LastUnclusteredUsedBlock = 0;
    m_usedUnclusteredBlocks    = 0;
    m_maxClusterNo.Invalidate();
	m_NumClusteredBlocksMarkedForBackup = 0;
    
    m_StartBlockNoToSearchFreeBlock = m_FirstFreeBlock;

    /* determine number of sections */
    m_NumberOfSections = ((m_MaxBlockNo + 1) >> m_LogSectionSize);

    if ((IOMan_BlockNo)(m_NumberOfSections << m_LogSectionSize) <= m_MaxBlockNo)
        ++m_NumberOfSections;

    /* create indizees */
    if (! m_NumBlocksFreeInSection.Resize (m_NumberOfSections, 0) ||
            ! m_NumBlocksMarkedForBackUpInSection.Resize (m_NumberOfSections, 0) ||
            ! m_NumBlocksFreeAfterSVPInSection.Resize (m_NumberOfSections, 0))
    {
        FBM_Exception errMsg(__CONTEXT__, FBM_ERR_NO_MORE_MEMORY);
        RTE_Crash(errMsg);
    }

    if (! m_clusterUsageList.Resize ((m_MaxBlockNo + 1) >> m_logClusterSize, 0))
    {
        FBM_Exception errMsg(__CONTEXT__, FBM_ERR_NO_MORE_MEMORY);
        RTE_Crash(errMsg);
    }

    /* initialize the lists of counters which contain the number of free blocks per section, the number */
    /* of blocks marked for backup per section and  number of blocks marked for backup per section      */
    for (int iSection=0; iSection<m_NumberOfSections; iSection++)
    {
        m_NumBlocksFreeInSection [iSection] = m_SectionSize;
        m_NumBlocksMarkedForBackUpInSection [iSection] = 0;
        m_NumBlocksFreeAfterSVPInSection [iSection] = 0;
    }

    m_NumBlocksFreeInSection[m_NumberOfSections-1] = m_MaxBlockNo + 1 - ((m_NumberOfSections-1) << m_LogSectionSize);

	if ((numCluster > (m_MaxBlockNo >> m_logClusterSize)) || !reserveClusterSpace(numCluster))
	{
		FBM_CantReserveClusterSpace errMsg(__CONTEXT__, numCluster, m_MaxBlockNo, m_maxUnclusteredBlockNo);
		RTE_Message(errMsg);
		return false;
	}
	
	/* check consistency of the device */
#   ifdef SAPDB_SLOW
	if( FBM_Check.ChecksLevel( 5 ))
		Verify ();
#   endif

	return true;
}

/*---------------------------------------------------------------------------*/

void
FBM_SimpleDataVolume::RestoreAllBlockStatesMarkedForBackup (IOMan_BlockCount &NumBlocksRestoredToFreeAfterSVP)
{
    SAPDBTRACE_ROUTINE_DEBUG  ("FBM_SimpleDataVolume::RestoreAllBlockStatesMarkedforBackup", FBM_Trace, 5);

    IOMan_BlockNo     BeginOfThisSection;
    IOMan_BlockNo     EndOfThisSection;
    SAPDB_Int4     iSection;
    IOMan_BlockNo     iBlock;
    SAPDB_Bool     bRestoredStateIsFreeAfterSVP;

    NumBlocksRestoredToFreeAfterSVP = 0;

    for (iSection=0; (iSection<m_NumberOfSections) && (m_NumBlocksMarkedForBackup>0); ++iSection)
    {
        if (m_NumBlocksMarkedForBackUpInSection[iSection]>0)
        {
            BeginOfThisSection = SectionBegin(iSection);
            EndOfThisSection   = SectionEnd  (iSection);

            for (iBlock=BeginOfThisSection; iBlock<=EndOfThisSection;  ++iBlock)
            {
                if (GetBlockState (iBlock) == BlockState_BackUp)
                {
                    /* restore the state to the one it had before the backup */
                    RestoreBlockStateMarkedForBackup (iBlock, bRestoredStateIsFreeAfterSVP);
                    if (bRestoredStateIsFreeAfterSVP)
                        ++NumBlocksRestoredToFreeAfterSVP;

                    if (m_NumBlocksMarkedForBackUpInSection[iSection] == 0) break;
                }
            }
            SAPDBERR_ASSERT_STATE (m_NumBlocksMarkedForBackUpInSection[iSection] == 0);
        }
    }
    SAPDBERR_ASSERT_STATE (m_NumBlocksMarkedForBackup == 0);
	SAPDBERR_ASSERT_STATE (m_NumClusteredBlocksMarkedForBackup == 0);
	
    /* reset marker of last block read for back up */
    m_ActBlockNoForBackup.Invalidate();
	m_ActClusterNoForBackup.Invalidate();
	
	// reset cluster counter
	m_NumClusteredBlocksMarkedForBackup = 0;

    /* check consistency of the device */
#   ifdef SAPDB_SLOW
    if( FBM_Check.ChecksLevel( 5 ))
        Verify ();
#   endif
}

/*---------------------------------------------------------------------------*/

IOMan_BlockNo
FBM_SimpleDataVolume::GetMultFreeBlocks  (const IOMan_BlockCount  NumBlocksWanted,
                                          IOMan_BlockCount       &NumBlocksSupplied)
{
    SAPDBTRACE_ROUTINE_DEBUG  ("FBM_SimpleDataVolume::GetMultFreeBlocks", FBM_Trace, 5);

    /* this routine is called to get  a cluster of free blocks.  */
    /* before the routine is called it is already obvious that   */
    /* there must be at least one free block on this device      */

    SAPDB_Bool bFirstFreeBlockWasUsed = false;
    SAPDB_Bool bFreePairFound;
    IOMan_BlockNo FreePairBlockNo;
    IOMan_BlockNo LastSuppliedFreeBlock;
    IOMan_BlockNo FreeBlockNo;
    SAPDB_Int4 CurrSection;
    const IOMan_BlockNo lastBlock = m_maxUnclusteredBlockNo;

    /* find free BlockNo */
    FindFreeBlock (m_StartBlockNoToSearchFreeBlock, FreeBlockNo, CurrSection,  bFirstFreeBlockWasUsed);

    /* check if this free block belongs to a cluster of free blocks. if not the next cluster is searched */
    if (((FreeBlockNo + 1) <= lastBlock) && (GetBlockState (FreeBlockNo + 1) == BlockState_Free ))
        bFreePairFound = true;
    else
    {
        FindPairOfFreeBlocks (FreeBlockNo, bFreePairFound, FreePairBlockNo, m_StartBlockNoToSearchFreeBlock, lastBlock);
        if (bFreePairFound)
        {
            bFirstFreeBlockWasUsed &= (FreeBlockNo == FreePairBlockNo);
            FreeBlockNo = FreePairBlockNo;
            CurrSection   = (FreeBlockNo >> m_LogSectionSize);
            SAPDBERR_ASSERT_STATE ((GetBlockState(FreeBlockNo)     == BlockState_Free) &&
                                   (GetBlockState(FreeBlockNo + 1) == BlockState_Free));
        }
    }

    if (m_BlockStateList.GetBlockState(FreeBlockNo) != BlockState_Free)
    {
        FBM_Exception errMsg( __CONTEXT__, FBM_ERR_FREE_BLOCK_NOT_FREE, SAPDB_ToString (m_VolNo, _T_d),
                                                                        SAPDB_ToString (FreeBlockNo, _T_d));
        RTE_Crash(errMsg);
    }
    if ((FreeBlockNo < 0) || (FreeBlockNo > lastBlock ))
    {
        FBM_Exception errMsg( __CONTEXT__, FBM_ERR_BLOCK_OUT_OF_BOUNDS, SAPDB_ToString (m_VolNo, _T_d),
                                                                        SAPDB_ToString (FreeBlockNo, _T_d),
                                                                        SAPDB_ToString (0, _T_d),
                                                                        SAPDB_ToString (lastBlock, _T_d));
        RTE_Crash(errMsg);
    }

    /* change state of free block to occupied */
    SetBlockStateToOccupied (FreeBlockNo);

    /* reserve all following blocks which are free too until NumBlocksWanted */
    /* free blocks are found or the first block which is not free occurs     */
    for (LastSuppliedFreeBlock=FreeBlockNo, NumBlocksSupplied = 1 ;
            ((NumBlocksSupplied<NumBlocksWanted)  &&
             (LastSuppliedFreeBlock + 1) <= lastBlock) &&
            (GetBlockState (LastSuppliedFreeBlock + 1) == BlockState_Free);
            NumBlocksSupplied++)
    {
        LastSuppliedFreeBlock++;
        SetBlockStateToOccupied (LastSuppliedFreeBlock);
    }

    if (bFreePairFound)
        m_StartBlockNoToSearchFreeBlock = LastSuppliedFreeBlock;

    /* write found block no into vtrace */
    SAPDBTRACE_WRITELN( FBM_Trace, 5, "FreeBlockNo: " << FreeBlockNo
                        << " #WantedFreeB: "  << NumBlocksWanted
                        << " #FoundFreeB: "  << NumBlocksSupplied);

        /* update hint where to start to find the next free block */
    if (bFirstFreeBlockWasUsed || (m_FirstFreeBlock == FreeBlockNo))
        m_FirstFreeBlock = LastSuppliedFreeBlock + 1;

    /* update the counter which controlls the distribution of blocks onto devices */
    m_NumBlocksToAddUntilOptIsReached -=NumBlocksSupplied;

    /* check consistency of the device */
#   ifdef SAPDB_SLOW
    if( FBM_Check.ChecksLevel( 5 ))
        Verify ();
#   endif

    return FreeBlockNo;
}

/*---------------------------------------------------------------------------*/

IOMan_BlockNo
FBM_SimpleDataVolume::GetFreeBlock  ()
{
    SAPDBTRACE_ROUTINE_DEBUG  ("FBM_SimpleDataVolume::GetFreeBlock", FBM_Trace, 5);

    /* this routine is called to get the adress of a free block. */
    /* before the routine is called it is already obvious that   */
    /* there must be at least one free block on this device      */

    SAPDB_Bool bFirstFreeBlockWasUsed = false;
    IOMan_BlockNo FreeBlockNo;
    SAPDB_Int4 CurrSection;
    const IOMan_BlockNo lastBlock = m_maxUnclusteredBlockNo;

    /* find free BlockNo */
    FindFreeBlock (m_StartBlockNoToSearchFreeBlock, FreeBlockNo, CurrSection,  bFirstFreeBlockWasUsed);

    if (m_BlockStateList.GetBlockState(FreeBlockNo) != BlockState_Free)
    {
        FBM_Exception errMsg( __CONTEXT__, FBM_ERR_FREE_BLOCK_NOT_FREE, SAPDB_ToString (m_VolNo, _T_d),
                                                                        SAPDB_ToString (FreeBlockNo, _T_d));
        RTE_Crash(errMsg);
    }
    if ((FreeBlockNo < 0) || (FreeBlockNo > lastBlock ))
    {
        FBM_Exception errMsg( __CONTEXT__, FBM_ERR_BLOCK_OUT_OF_BOUNDS, SAPDB_ToString (m_VolNo, _T_d),
                                                                        SAPDB_ToString (FreeBlockNo, _T_d),
                                                                        SAPDB_ToString (0, _T_d),
                                                                        SAPDB_ToString (lastBlock, _T_d));
        RTE_Crash(errMsg);
    }

    /* change state of free block to occupy */
    SetBlockStateToOccupied(FreeBlockNo);

    /* write found block no into vtrace */
    SAPDBTRACE_WRITELN( FBM_Trace, 5, "FreeSBlockNo: " << FreeBlockNo);

    /* update hint where to start to find the next and the first free block */
    m_StartBlockNoToSearchFreeBlock = FreeBlockNo + 1;
    if (bFirstFreeBlockWasUsed)
    {
        m_FirstFreeBlock = FreeBlockNo + 1;
    }

    /* update the counter which controlls the distribution of blocks onto devices */
    --m_NumBlocksToAddUntilOptIsReached;

    /* check consistency of the device */
#   ifdef SAPDB_SLOW
    if( FBM_Check.ChecksLevel( 5 ))
        Verify ();
#   endif

    return FreeBlockNo;
}

/*---------------------------------------------------------------------------*/

void
FBM_SimpleDataVolume::SetAllBlocksMarkedAsFreeAfterSVPToFree ()
{
    SAPDBTRACE_ROUTINE_DEBUG  ("FBM_SimpleDataVolume::SetAllBlocksMarkedAsFreeAfterSVPToFree", FBM_Trace, 5);

    IOMan_BlockNo     BeginOfThisSection;
    IOMan_BlockNo     EndOfThisSection;
    SAPDB_Int4        iSection;
    IOMan_BlockNo     iBlock;

    /* go through all blocks starting with the largest block */
    for (iSection=m_MaxBlockNo>>m_LogSectionSize; (iSection>=0) && (m_NumBlocksFreeAfterSVP>0); --iSection)
    {
        if (m_NumBlocksFreeAfterSVPInSection[iSection] > 0)
        {
            BeginOfThisSection = SectionBegin(iSection);
            EndOfThisSection   = SectionEnd  (iSection);

            for (iBlock=EndOfThisSection; iBlock>=BeginOfThisSection ;--iBlock)
            {
                if (m_BlockStateList.GetBlockState (iBlock) == BlockState_FreeAfterSVP)
                {
                    SetBlockStateToFreeNoCheck (iBlock);
                    --m_NumBlocksFreeAfterSVP;
                    --m_NumBlocksFreeAfterSVPInSection[iSection];
                    if (m_NumBlocksFreeAfterSVPInSection[iSection] == 0)
                    {
                        break;
                    }
                }
            }
            SAPDBERR_ASSERT_STATE (m_NumBlocksFreeAfterSVPInSection[iSection] == 0);
        }
    }

    /* check that no blocks remain in the state free after save point */
    SAPDBERR_ASSERT_STATE (m_NumBlocksFreeAfterSVP == 0);

    /* check consistency of the device */
#   ifdef SAPDB_SLOW
    if( FBM_Check.ChecksLevel( 5 ))
        Verify ();
#   endif
}

IOMan_ClusterNo FBM_SimpleDataVolume::CalcMaxClusterNo()
{
    IOMan_BlockNo     BeginOfThisSection;
    IOMan_BlockNo     EndOfThisSection;
    SAPDB_Int4        iSection;
    IOMan_BlockNo     iBlock;
    
    /* find the last unclustered used block */
    if (m_NumBlocksUsed == 0)
    {
        m_LastUsedBlock            = 0;
        m_LastUnclusteredUsedBlock = 0;
    }
    else
    {
   
        /* first search first last used section */
        for (iSection = (m_LastUsedBlock >> m_LogSectionSize); iSection >= 0;--iSection)
        {
            BeginOfThisSection = SectionBegin(iSection);
            EndOfThisSection   = SectionEnd  (iSection);
            if (m_NumBlocksFreeInSection [iSection] < (EndOfThisSection-BeginOfThisSection+1))
            {
                break;
            }
        }
   
        /* find last used block in last used section */
        for (iBlock=SAPDB_MIN(EndOfThisSection,m_LastUsedBlock);iBlock>=BeginOfThisSection;--iBlock)
        {
            if (m_BlockStateList.GetBlockState (iBlock) != BlockState_Free)
            {
                m_LastUsedBlock = iBlock;
                break;
            }
        }
        /* first search first last used section */
        for (iSection = (m_LastUnclusteredUsedBlock >> m_LogSectionSize); iSection >= 0;--iSection)
        {
            BeginOfThisSection = SectionBegin(iSection);
            EndOfThisSection   = SectionEnd  (iSection);
            if (m_NumBlocksFreeInSection [iSection] < (EndOfThisSection-BeginOfThisSection+1))
            {
                break;
            }
        }
    
        /* find last used block in last used section */
        for (iBlock=SAPDB_MIN(EndOfThisSection,m_LastUnclusteredUsedBlock); iBlock >= BeginOfThisSection; --iBlock)
        {
            if (m_BlockStateList.GetBlockState (iBlock) != BlockState_Free)
            {
                m_LastUnclusteredUsedBlock = iBlock;
                break;
            }
        }
    }
    
    // find max cluster number
    m_maxClusterNo.Invalidate();
    SAPDB_Int cluster = m_reservedClusters;
    while (cluster >= 0)
    {
        if (m_clusterUsageList[cluster] != 0)
        {
            m_maxClusterNo = cluster;
            break;
        }
        else
        {
            cluster--;
        }
    }
    return m_maxClusterNo;
}

/*---------------------------------------------------------------------------*/

void
FBM_SimpleDataVolume::GetNextBlocksForBackUp (
     const IOMan_BlockCount          maxBlocksWanted,
     const SAPDB_Bool                searchClustered,
     IOMan_ComplexClusterAddress&    cluster )
{
    SAPDBTRACE_ROUTINE_DEBUG  ("FBM_SimpleDataVolume::GetNextBlocksForBackUp", FBM_Trace, 5);

    if (searchClustered)
    {
        // clustered
        if (m_ActClusterNoForBackup.IsInvalid() || (maxBlocksWanted != m_clusterSize))  // nothing todo or clustersize != backupBlockSize
        {
            cluster.Invalidate();
            return;
        }

        /* search next cluster with blocks marked for back up */
        while ((m_ActClusterNoForBackup >= 0) && !hasBlocksForBackup(m_ActClusterNoForBackup))
        {
            m_ActClusterNoForBackup--;
        }
        
        if (m_ActClusterNoForBackup >= 0)  // cluster found for backup
        {
            // mark blocks in cluster
            IOMan_BlockNo backupBlock = getClusterStartAddress(m_ActClusterNoForBackup);
            cluster.SetCluster(IOMan_BlockAddress(m_VolNo, backupBlock), 0);
            for (IOMan_ComplexClusterAddress::BlockIndex index = 0; index < maxBlocksWanted; index++, backupBlock++)
            {
                cluster.Append(GetBlockState (backupBlock) == BlockState_BackUp);
            }
            
            // prepare hint for next cluster
            m_ActClusterNoForBackup--;
            if (m_ActClusterNoForBackup < 0)
            {
                m_ActClusterNoForBackup.Invalidate();
            }
            return;
        }
        else
        {
            cluster.Invalidate();
            return;
        }            
    }
    else
    {
        // unclustered
	
        if (m_ActBlockNoForBackup.IsInvalid())
        {
            cluster.Invalidate();
            return;
        }
 
        /* search next block marked for back up starting from the current backup block no */
        SAPDB_Int4 CurrSection       = m_ActBlockNoForBackup>>m_LogSectionSize;
        IOMan_BlockNo CurrSectionEnd = SectionEnd(CurrSection);
        SAPDB_Bool bBlockFound       = false;
    
        /* go through current section and find the first block which is marked as back up */
        while (m_ActBlockNoForBackup <= CurrSectionEnd)
        {
            if (GetBlockState (m_ActBlockNoForBackup) == BlockState_BackUp)
            {
                /* check if the found block is really marked for backup and not for compression */
                SAPDBERR_ASSERT_STATE ((GetBlockStateAfterBackup (m_ActBlockNoForBackup) == BlockState_Occupied) ||
                                       (GetBlockStateAfterBackup (m_ActBlockNoForBackup) == BlockState_FreeAfterSVP));
                bBlockFound = true;
                break;
            }
            ++m_ActBlockNoForBackup;
        }
    
        /* goto next section which contains blocks marked for backup */
        if (!bBlockFound)
        {
            SAPDB_Bool bSectionFound  = false;
    
            while ((++CurrSection)<m_NumberOfSections)
            {
                if (m_NumBlocksMarkedForBackUpInSection[CurrSection] > 0)
                {
                    bSectionFound = true;
                    break;
                }
            }
    
            if (!bSectionFound)
            {
                m_ActBlockNoForBackup.Invalidate();
            }
            else
            {
                /* go through section and search first block marked for backup */
                m_ActBlockNoForBackup = SectionBegin(CurrSection);
                CurrSectionEnd      = SectionEnd(CurrSection);
                while (m_ActBlockNoForBackup <= CurrSectionEnd)
                {
                    if (GetBlockState (m_ActBlockNoForBackup) == BlockState_BackUp)
                    {
                        bBlockFound = true;
                        break;
                    }
                    ++m_ActBlockNoForBackup;
                }
    
                /* the found section must contain blocks marked for back up */
                SAPDBERR_ASSERT_STATE (bBlockFound);
            }
        }
    
        /* set return value where the next set of neighbouring blocks marked for back up starts */
        IOMan_BlockNo startBlockNO = m_ActBlockNoForBackup;
        IOMan_BlockCount SuppliedNumBlocks = 0;
    
        /* check if there is really a next block which is          */
        /* marked for back up and not yet fetched by an datawriter */
        if (bBlockFound)
        {
            /* check out if the following blocks are also in the state BlockState_BackUp */
            SuppliedNumBlocks = 1;
            ++m_ActBlockNoForBackup;
    
            while ((SuppliedNumBlocks      <  maxBlocksWanted) &&
                    (m_ActBlockNoForBackup  <= m_MaxBlockNo)    &&
                    (GetBlockState (m_ActBlockNoForBackup) == BlockState_BackUp))
            {
    
                /* check if the found block is really marked for backup and not for compression */
                SAPDBERR_ASSERT_STATE ((GetBlockStateAfterBackup (m_ActBlockNoForBackup) == BlockState_Occupied) ||
                                       (GetBlockStateAfterBackup (m_ActBlockNoForBackup) == BlockState_FreeAfterSVP));
                ++ SuppliedNumBlocks;
                ++ m_ActBlockNoForBackup;
            }
    
            /* set marker to display that there are no more blocks for backup */
            if (m_ActBlockNoForBackup > m_MaxBlockNo)
            {
                m_ActBlockNoForBackup.Invalidate();
            }
        }
        
        if (0 == SuppliedNumBlocks)
        {
            cluster.Invalidate();
        }
        else
        {
            cluster.SetCluster(IOMan_BlockAddress(m_VolNo, startBlockNO), SuppliedNumBlocks);
        }
    }
    /* check consistency of the device */
#   ifdef SAPDB_SLOW
    if( FBM_Check.ChecksLevel( 5 ))
        Verify ();
#   endif
}



void
FBM_SimpleDataVolume::SetBlockStateToOccupied (IOMan_BlockNo BlockNo)
{
 
    SAPDBERR_ASSERT_STATE((BlockNo <= m_maxUnclusteredBlockNo) || (BlockNo >= m_minClusteredBlockNo));
         //  do use blocks between m_maxUnclusteredBlockNo and m_minClusteredBlockNo
    
    m_BlockStateList.SetBlockState (BlockNo, BlockState_Occupied);
    ++m_NumBlocksUsed;

    if (m_LastUsedBlock < BlockNo)
    {
         m_LastUsedBlock = BlockNo;
    }
    
    if (BlockNo <= m_maxUnclusteredBlockNo)
    {
        m_usedUnclusteredBlocks++;
        if (BlockNo > m_LastUnclusteredUsedBlock)
        {
            m_LastUnclusteredUsedBlock = BlockNo;
        }
    }
    
    if (BlockNo >= m_minClusteredBlockNo)
    {
        IOMan_ClusterNo clusterNumber = getClusterNumber(BlockNo);
    
        if (0 == m_clusterUsageList[clusterNumber]) // first usage
        {
           SAPDBERR_ASSERT_STATE(m_usedClusters <= m_reservedClusters);
           m_usedClusters++;

           m_maxClusterNo = SAPDB_MAX(m_maxClusterNo, clusterNumber);
        }
        m_clusterUsageList[clusterNumber]++;
       
        SAPDBERR_ASSERT_STATE(m_clusterUsageList[clusterNumber] <= m_clusterSize);
    }
    
    --m_NumBlocksFreeInSection[BlockNo>>m_LogSectionSize];
    
#       ifdef SAPDB_SLOW
    if( FBM_Check.ChecksLevel( 5 ))
        Verify ();
#       endif
}
    
/*---------------------------------------------------------------------------*/

void
FBM_SimpleDataVolume::Verify ()
{
    SAPDBTRACE_ROUTINE_DEBUG  ("FBM_SimpleDataVolume::Verify", FBM_Trace, 5);

    SAPDBERR_ASSERT_STATE ((m_MaxBlockNo + 1) == m_BlockStateList.Capacity());

    IOMan_BlockCount NumBlocksFree        = 0;
    IOMan_BlockCount NumBlockFreeAfterSVP = 0;

    IOMan_BlockNo FirstFreeBlock                = m_MaxBlockNo + 1;
    IOMan_BlockNo LastUsedBlock                 = 0;
    IOMan_BlockCount NumBlocksMarkedForBackUp   = 0;

    /* note: this can be written a better way when exceptions are allowed */
    FBM_CounterList  NumBlocksFreeInSection( m_NumBlocksFreeInSection.GetAllocator() );
    FBM_CounterList  NumBlocksFreeAfterSVPInSection( m_NumBlocksFreeAfterSVPInSection.GetAllocator() );
    FBM_CounterList  NumBlocksMarkedForBackUpInSection( m_NumBlocksMarkedForBackUpInSection.GetAllocator() );

    if (!NumBlocksFreeInSection.Resize(m_NumberOfSections, 0)         ||
            !NumBlocksFreeAfterSVPInSection.Resize(m_NumberOfSections, 0) ||
            !NumBlocksMarkedForBackUpInSection.Resize(m_NumberOfSections, 0))
    {
        FBM_Exception Error(__CONTEXT__, FBM_NO_MEMORY_FOR_VERIFICATION, SAPDB_ToString (m_NumberOfSections, _T_d));
        RTE_Message (Error);
        return;
    }


    /* go through all entries in the blocklist and count the occurence of each state */
    for (IOMan_BlockNo iBlockNo=0; iBlockNo <= m_MaxBlockNo; ++iBlockNo)
    {
        FBM_BlockState BlockStateAfterBackUp = GetBlockStateAfterBackup(iBlockNo);

        switch (GetBlockState(iBlockNo))
        {
        case BlockState_Free:
            ++NumBlocksFree;
            ++NumBlocksFreeInSection [iBlockNo >> m_LogSectionSize];
            if (FirstFreeBlock > iBlockNo)
                FirstFreeBlock = iBlockNo;
            SAPDBERR_ASSERT_STATE (BlockState_Free == BlockStateAfterBackUp);
            break;
        case BlockState_Occupied:
            if (LastUsedBlock < iBlockNo)
                LastUsedBlock = iBlockNo;
            SAPDBERR_ASSERT_STATE (BlockState_Free == BlockStateAfterBackUp);
            break;
        case BlockState_FreeAfterSVP:
            ++NumBlockFreeAfterSVP;
            ++NumBlocksFreeAfterSVPInSection [iBlockNo >> m_LogSectionSize];
            if (LastUsedBlock < iBlockNo)
                LastUsedBlock = iBlockNo;
            SAPDBERR_ASSERT_STATE (BlockState_Free == BlockStateAfterBackUp);
            break;
        case BlockState_BackUp:
            ++ NumBlocksMarkedForBackUp;
            ++ NumBlocksMarkedForBackUpInSection [iBlockNo >> m_LogSectionSize];
            {
                //blocks are marked for back up
                SAPDBERR_ASSERT_STATE ((BlockState_Occupied     == BlockStateAfterBackUp) ||
                                       (BlockState_FreeAfterSVP == BlockStateAfterBackUp));
            }

            if (LastUsedBlock < iBlockNo)
            {
                LastUsedBlock = iBlockNo;
            }
            break;
        default:
            /* write error message into knldiag and vtrace */
            FBM_IllegalState IllegalState(__CONTEXT__, iBlockNo, GetBlockState (iBlockNo), BlockStateAfterBackUp);
            RTE_Crash(IllegalState);
        }
    }

    /* check counters and pointers */
    SAPDBERR_ASSERT_STATE (NumBlocksMarkedForBackUp == m_NumBlocksMarkedForBackup);
    SAPDBERR_ASSERT_STATE (m_NumBlocksUsed == (m_MaxBlockNo + 1 - NumBlocksFree));
    SAPDBERR_ASSERT_STATE (m_NumBlocksFreeAfterSVP == NumBlockFreeAfterSVP);
    SAPDBERR_ASSERT_STATE (FirstFreeBlock >= m_FirstFreeBlock);
    SAPDBERR_ASSERT_STATE (LastUsedBlock <= m_LastUsedBlock);

    /* check content of the counters of free blocks in each section */
    for (int iSection=0; iSection<m_NumberOfSections;iSection++)
    {
        SAPDBERR_ASSERT_STATE (NumBlocksFreeInSection[iSection]            == m_NumBlocksFreeInSection[iSection]);
        SAPDBERR_ASSERT_STATE (NumBlocksFreeAfterSVPInSection[iSection]    == m_NumBlocksFreeAfterSVPInSection[iSection]);
        SAPDBERR_ASSERT_STATE (NumBlocksMarkedForBackUpInSection[iSection] == m_NumBlocksMarkedForBackUpInSection[iSection]);
    }
}

/*---------------------------------------------------------------------------*/

const SAPDB_Int2
FBM_SimpleDataVolume::m_LogSectionSize = 8;

/*---------------------------------------------------------------------------*/

const SAPDB_Int2
FBM_SimpleDataVolume::m_SectionSize    = 1 << (FBM_SimpleDataVolume::m_LogSectionSize);

/*---------------------------------------------------------------------------*/

void
FBM_SimpleDataVolume::FindPairOfFreeBlocks (const IOMan_BlockNo  StartSearchBlockNo,
                                      SAPDB_Bool &bFreePairFound,
                                      IOMan_BlockNo &FreePairBlockNo,
                                      IOMan_BlockNo &StartBlockNoForNextSearch,
                                      const IOMan_BlockNo &lastBlock) const
{
    IOMan_BlockNo SearchBlockNo           = StartSearchBlockNo;
    const IOMan_BlockNo OptLastUsedBlock  = GetOptLastUsedBlock();
    const SAPDB_Int4 MaxBlocksToScan   = 256;

    for (SAPDB_Int4 BlockCount=0; BlockCount < MaxBlocksToScan; ++BlockCount)
    {
        if (GetBlockState(SearchBlockNo) != BlockState_Free || ((SearchBlockNo + 1) >= (lastBlock + 1) ))
            ++SearchBlockNo;
        else
        {
            if (GetBlockState(SearchBlockNo + 1) != BlockState_Free)
                SearchBlockNo += 2;
            else
            {
                bFreePairFound            = true;
                FreePairBlockNo           = SearchBlockNo;
                StartBlockNoForNextSearch = SearchBlockNo;
                return;
            }
        }

        if (SearchBlockNo > OptLastUsedBlock)
            SearchBlockNo = m_FirstFreeBlock;
    }

    bFreePairFound            = false;
    StartBlockNoForNextSearch = SearchBlockNo;
}

/*---------------------------------------------------------------------------*/

void
FBM_SimpleDataVolume::FindFreeBlock (const IOMan_BlockNo  StartSearchBlockNo,
                               IOMan_BlockNo &FreeBlockNo,
                               SAPDB_Int4 &CurrSection,
                               SAPDB_Bool &bFirstFreeBlockWasUsed)
{
    SAPDBERR_ASSERT_STATE ((m_maxUnclusteredBlockNo + 1 - m_usedUnclusteredBlocks) > 0 );

    bool             bFreeBlockFound   = false;

    const IOMan_BlockNo OptLastUsedBlock  = GetOptLastUsedBlock();

    /* get starting position and section to search the first free block */
    IOMan_BlockNo SearchBlockNo = StartSearchBlockNo + 1;
    if (SearchBlockNo > OptLastUsedBlock)
    {
        SearchBlockNo = m_FirstFreeBlock;
        bFirstFreeBlockWasUsed = true;
    }

    CurrSection = (SearchBlockNo >> m_LogSectionSize);

    /* check if there is a free block in the current section behind the start position for search */
    if (m_NumBlocksFreeInSection[CurrSection] > 0)
    {
        const IOMan_BlockNo StopBlockNoInSection = SectionEnd(CurrSection);
        while(SearchBlockNo<=StopBlockNoInSection)
        {
            if (GetBlockState (SearchBlockNo) == BlockState_Free)
            {
            bFreeBlockFound = true;
            break;
            }
            ++SearchBlockNo;
        }
    }

    /* if no free block was found goto next section with free blocks */
    if (!bFreeBlockFound)
    {
        SAPDB_Int4 OptLastUsedSection = (OptLastUsedBlock >> m_LogSectionSize);

        /* find the next section with a free block */
        do
        {
            if (CurrSection < OptLastUsedSection)
                ++CurrSection;
            else
            {
                SAPDBERR_ASSERT_STATE (bFirstFreeBlockWasUsed == false);

                CurrSection = (m_FirstFreeBlock >> m_LogSectionSize);
                bFirstFreeBlockWasUsed = true;
            }
        }
        while (m_NumBlocksFreeInSection[CurrSection] == 0);

        /* find the free block within the section which was found to contain free blocks */
        const IOMan_BlockNo StopBlockNoInSection  = SectionEnd(CurrSection);
        const IOMan_BlockNo StartBlockNoInSection = SectionBegin(CurrSection);
        for (SearchBlockNo=StartBlockNoInSection; SearchBlockNo<=StopBlockNoInSection; ++SearchBlockNo)
        {
            if (GetBlockState (SearchBlockNo) == BlockState_Free) break;
        }
    }
    FreeBlockNo = SearchBlockNo;

    /* check that a free block was found */
    SAPDBERR_ASSERT_STATE (m_BlockStateList.GetBlockState(FreeBlockNo) == BlockState_Free);

}

/*---------------------------------------------------------------------------*/


void
FBM_SimpleDataVolume::RestoreBlockStateMarkedForBackup (IOMan_BlockNo  BlockNo,
        SAPDB_Bool &bRestoredStateIsFreeAfterSVP)
{
    /* check if the state to be restored is correct */
    SAPDBERR_ASSERT_STATE ((GetBlockState             (BlockNo) == BlockState_BackUp)       &&
                           ((GetBlockStateAfterBackup (BlockNo) == BlockState_Occupied) ||
                            (GetBlockStateAfterBackup  (BlockNo) == BlockState_FreeAfterSVP)));

    /* update counter of blocks marked for back up */
    --m_NumBlocksMarkedForBackup;
    --m_NumBlocksMarkedForBackUpInSection[BlockNo>>m_LogSectionSize];
	if (BlockNo >= m_minClusteredBlockNo)
	{
		--m_NumClusteredBlocksMarkedForBackup;
	}

    /* restore the state to the one it had before the backup. If  */
    /* the restored state (=return value of the restore function) */
    /* is FreeAfterSVP the appropriate counter is updated         */
    if (m_BlockStateList.RestoreBlockStateMarkedForBackup (BlockNo) != BlockState_FreeAfterSVP)
    {
        bRestoredStateIsFreeAfterSVP = false;
    }
    else
    {
        ++m_NumBlocksFreeAfterSVP;
        ++m_NumBlocksFreeAfterSVPInSection[BlockNo>>m_LogSectionSize];
        --m_NumBlocksBackupFreeAfterSVP;
        bRestoredStateIsFreeAfterSVP = true;
    }

#   ifdef SAPDB_SLOW
    if( FBM_Check.ChecksLevel( 5 ))
        Verify ();
#   endif
}

bool
FBM_SimpleDataVolume::testAddClusterSpace(IOMan_ClusterCount numCluster)
{
    return testClusterSpace(m_reservedClusters + numCluster);
}

bool
FBM_SimpleDataVolume::reserveAddClusterSpace(IOMan_ClusterCount numCluster)
{
    return reserveClusterSpace(m_reservedClusters + numCluster);
}

bool
FBM_SimpleDataVolume::testClusterSpace(IOMan_ClusterCount numCluster)
{
    SAPDBTRACE_ROUTINE_DEBUG  ("FBM_SimpleDataVolume::testClusterSpace", FBM_Trace, 5);
    SAPDBTRACE_WRITELN( FBM_Trace, 5, "test: " << numCluster << " volume: " << m_VolNo );
    IOMan_BlockNo newBorder = calcClusterBorder(numCluster);

    if (newBorder <= m_LastUnclusteredUsedBlock)
    {
        SAPDBTRACE_WRITELN( FBM_Trace, 5, "test failed: new Border: "<< newBorder << " last unclustered: " << m_LastUnclusteredUsedBlock);
        return false;
    }
    else
    {
        return true;
    }
}


bool
FBM_SimpleDataVolume::reserveClusterSpace(IOMan_ClusterCount numCluster)
{
    SAPDBTRACE_ROUTINE_DEBUG  ("FBM_SimpleDataVolume::reserveClusterSpace", FBM_Trace, 5);
    SAPDBTRACE_WRITELN( FBM_Trace, 5, "reserve: " << numCluster << " volume: " << m_VolNo );

    IOMan_BlockNo newBorder = calcClusterBorder(numCluster);

    if (newBorder <= m_LastUnclusteredUsedBlock)
    {
        SAPDBTRACE_WRITELN( FBM_Trace, 5, "reserve failed: new Border: "<< newBorder << " last unclustered: " << m_LastUnclusteredUsedBlock);
        return false;
    }
    else
    {
//        SAPDB_Int4 lastUnclusteredSection = newBorder >> m_LogSectionSize;
//        IOMan_BlockNo oldSectionEnd = SectionEnd(lastUnclusteredSection);
        
        m_maxUnclusteredBlockNo = newBorder;
		
		if (0 == numCluster)
		{
			m_minClusteredBlockNo = newBorder + 1;
		}
		else
		{
        
//        SAPDBERR_ASSERT_STATE(m_NumBlocksFreeInSection[lastUnclusteredSection] >= (oldSectionEnd - m_maxUnclusteredBlockNo));
//        m_NumBlocksFreeInSection[lastUnclusteredSection] -= oldSectionEnd - m_maxUnclusteredBlockNo;
			m_minClusteredBlockNo  = getClusterStartAddress(numCluster -1);
		}
        m_reservedClusters = numCluster;
        return true;
    }
}

IOMan_BlockNo
FBM_SimpleDataVolume::calcClusterBorder(const IOMan_ClusterCount numCluster) const
{
 	if (0 == numCluster)   // switch to old behavoir
	{ 
		return m_MaxBlockNo;
	}
	else
	{
		IOMan_BlockNo newClusterBorder = getClusterStartAddress(numCluster - 1) - 1;
    
		SAPDB_Int4 newLastSection = ((newClusterBorder + 1 ) >> m_LogSectionSize) - 1;
		return SectionEnd(newLastSection);
	}
}


bool
FBM_SimpleDataVolume::getFreeCluster(const IOMan_BlockCount clusterSize, IOMan_BlockNo &clusterStart)
{
    SAPDBTRACE_ROUTINE_DEBUG  ("FBM_SimpleDataVolume::GetMultCluster", FBM_Trace, 5);

    SAPDBERR_ASSERT_ARGUMENT(clusterSize <= m_clusterSize);
    
    if (!findFreeCluster(clusterSize, clusterStart))
    {
        return false;
    }

    if (m_BlockStateList.GetBlockState(clusterStart) != BlockState_Free)
    {
        FBM_Exception errMsg( __CONTEXT__, FBM_ERR_FREE_BLOCK_NOT_FREE, SAPDB_ToString (m_VolNo, _T_d),
                                                                        SAPDB_ToString (clusterStart, _T_d));
        RTE_Crash(errMsg);
    }

    if ((clusterStart < m_minClusteredBlockNo ) || (clusterStart > (m_MaxBlockNo - m_clusterSize + 1) ))
    {
        FBM_Exception errMsg( __CONTEXT__, FBM_ERR_BLOCK_OUT_OF_BOUNDS, SAPDB_ToString (m_VolNo, _T_d),
                                                                        SAPDB_ToString (clusterStart, _T_d),
                                                                        SAPDB_ToString (m_minClusteredBlockNo, _T_d),
                                                                        SAPDB_ToString (m_MaxBlockNo - m_clusterSize, _T_d));
        RTE_Crash(errMsg);
    }

    SAPDBTRACE_WRITELN( FBM_Trace, 5, "ClusterStart: " << clusterStart
                        << " #clusterSize: "  << clusterSize);

    IOMan_BlockNo loop;
    for (loop = clusterStart; loop < clusterStart+clusterSize; loop++)
    {
        SetBlockStateToOccupied(loop);
    }
    m_lastAssignedCluster = getClusterNumber(clusterStart);

    return true;
}

bool
FBM_SimpleDataVolume::findFreeCluster(const IOMan_BlockCount clusterSize, IOMan_BlockNo &clusterStart) const
{
    clusterStart = IOMan_BlockNo(); // invalidate

    if (m_reservedClusters <= m_usedClusters)
    {
        return false;
    }

    IOMan_ClusterNo searchCluster = m_lastAssignedCluster;
    IOMan_ClusterNo endSearch = searchCluster;
    bool found = false;

    do {
        searchCluster++;
        if (searchCluster >= m_reservedClusters)
        {
            searchCluster = 0;
        }
        if ( 0 == m_clusterUsageList[searchCluster] )
        {
            found = true;
            clusterStart = getClusterStartAddress(searchCluster);
        }
    } while (!found && (searchCluster != endSearch));

    return found;
}

bool
FBM_SimpleDataVolume::hasBlocksForBackup(const IOMan_ClusterNo clusterNo)
{
    SAPDB_Int numBlocks = 0;
    
    if ( 0 == m_clusterUsageList[clusterNo] )
    {
        return false;
    }
    IOMan_BlockNo clusterSearch = getClusterStartAddress(clusterNo);
    IOMan_BlockNo clusterEnd = clusterSearch + m_clusterSize;
    for(;clusterSearch < clusterEnd; clusterSearch++)
    {
        if (GetBlockState (clusterSearch) == BlockState_BackUp)
        {
            numBlocks++;
        }
    }
    if (numBlocks > (m_clusterSize * Pager_Controller::GetInstance().GetClusterWriteThreshold() / 100))
    {
        return true;
    }
    return false;
}
/*---------------------------------------------------------------------------*/

void
FBM_SimpleDataVolume::Dump(
    Kernel_Dump  &dump,
    const IOMan_VolumeNo volNo) const
{
    struct FBMDataDevice   fbmDev;

    fbmDev.dmpDevNo                           = volNo;
    fbmDev.dmpLastUsedBlock                   = m_LastUsedBlock;
    fbmDev.dmpFirstFreeBlock                  = m_FirstFreeBlock;
    fbmDev.dmpNumBlocks                       = m_MaxBlockNo + 1;
    fbmDev.dmpNumBlocksUsed                   = m_NumBlocksUsed;
    fbmDev.dmpNumBlocksMarkedForBackup        = m_NumBlocksMarkedForBackup;
	fbmDev.dmpNumCLBlocksMarkedForBackup      = m_NumClusteredBlocksMarkedForBackup;
    fbmDev.dmpNumBlocksFreeAfterSVP           = m_NumBlocksFreeAfterSVP;
    fbmDev.dmpNumBlocksBackupFreeAfterSVP     = m_NumBlocksBackupFreeAfterSVP;
    fbmDev.dmpNumBlocksToAddUntilOptIsReached = m_NumBlocksToAddUntilOptIsReached;
    fbmDev.dmpNumberOfSections                = m_NumberOfSections;
    fbmDev.dmpActBlockNoForBackup             = m_ActBlockNoForBackup;
    fbmDev.dmpStartBlockNoToSearchFreeBlock   = m_StartBlockNoToSearchFreeBlock;
    fbmDev.dmpVolumeToDrop                    = m_IsToDrop;
    fbmDev.dmpFiller3                         = false;
    fbmDev.dmpFiller2                         = 0;
    fbmDev.dmpMaxUnclusteredBlockNo           = m_maxUnclusteredBlockNo;
    fbmDev.dmpMinClusteredBlockNo             = m_minClusteredBlockNo;
    fbmDev.dmpLastUnclusteredUsedBlockNo      = m_LastUnclusteredUsedBlock;
    fbmDev.dmpUsedUnclusteredBlocks           = m_usedUnclusteredBlocks;
    fbmDev.dmpUsedClusters                    = m_usedClusters;
    fbmDev.dmpReservedClusters                = m_reservedClusters;
    
    dump.InsertEntry( Kernel_Dump::DmpFBMDataDevice,
                      Kernel_DumpPage::Entry( &fbmDev, sizeof( fbmDev )));
}

