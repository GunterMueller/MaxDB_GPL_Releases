/*!

  @file           FBM_SimpleDataVolume.hpp
  @author         TorstenS
  @author         AlexanderK
  @ingroup        FBM

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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

#ifndef FBM_SIMPLEDATAVOLUME_H
#define FBM_SIMPLEDATAVOLUME_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "FreeBlockManagement/FBM_DataVolume.hpp"

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
 @class          FBM_SimpleDataVolume
 @brief          representation of one data volume
 */
class FBM_SimpleDataVolume : public FBM_DataVolume
{
private:

    /// counter per section
    typedef Container_Vector<SAPDB_UInt2> FBM_CounterList;


    /// first free block on volume (unclustered)
    IOMan_BlockNo m_FirstFreeBlock;

    SAPDB_Int4       m_NumberOfSections;
    FBM_CounterList  m_NumBlocksFreeInSection;
    FBM_CounterList  m_NumBlocksMarkedForBackUpInSection;
    FBM_CounterList  m_NumBlocksFreeAfterSVPInSection;
    IOMan_BlockNo    m_StartBlockNoToSearchFreeBlock;
	IOMan_BlockCount m_NumClusteredBlocksMarkedForBackup;

    static const SAPDB_Int2 m_LogSectionSize;
    static const SAPDB_Int2 m_SectionSize;

    /// last block usable for unclustered data, allways on section border, last block in section!
    IOMan_BlockNo m_maxUnclusteredBlockNo;  // min is 0

    /// first block usable for clustered data
    IOMan_BlockNo m_minClusteredBlockNo;  // max is m_MaxBlockNo

    /// highest used block number in unclustered area
    IOMan_BlockNo m_LastUnclusteredUsedBlock;

    /// number of used unclustered blocks
    IOMan_BlockCount m_usedUnclusteredBlocks;

    /// size of of a cluster in blocks
    const IOMan_BlockCount m_clusterSize;
    SAPDB_UInt2 m_logClusterSize;

    /// number of used clusters
    IOMan_ClusterCount m_usedClusters;
    IOMan_ClusterNo m_lastAssignedCluster;
    IOMan_ClusterNo m_maxClusterNo;

    /// number of Clusters reserved
    IOMan_ClusterCount m_reservedClusters;

    FBM_CounterList m_clusterUsageList;
	
	/// position of the backup process in the cluster area
	IOMan_ClusterNo   m_ActClusterNoForBackup;


    IOMan_BlockNo SectionEnd(SAPDB_UInt4 SectionNo) const
    {
        if (SectionNo == (m_maxUnclusteredBlockNo >> m_LogSectionSize))
        {
            return m_maxUnclusteredBlockNo;
        }
        else
        {
            return SAPDB_MIN ((IOMan_BlockNo)(((SectionNo + 1) << m_LogSectionSize)-1), m_MaxBlockNo);
        }
    }

    IOMan_BlockNo SectionBegin(SAPDB_UInt4 SectionNo) const
    {
        return (SectionNo << m_LogSectionSize);
    }

    virtual void FindPairOfFreeBlocks (
        const IOMan_BlockNo StartSearchBlockNo,
        SAPDB_Bool &bFreePairFound,
        IOMan_BlockNo &FreePairBlockNo,
        IOMan_BlockNo &StartBlockNoForNextSearch,
        const IOMan_BlockNo &lastBlock) const;

    virtual void FindFreeBlock (
        const IOMan_BlockNo StartSearchBlockNo,
        IOMan_BlockNo &FreeBlockNo,
        SAPDB_Int4 &CurrSection,
        SAPDB_Bool &bFirstFreeBlockWasUsed);

    IOMan_BlockNo GetOptLastUsedBlock() const
    {
        return SAPDB_MIN((IOMan_BlockNo)(m_NumBlocksUsed + (m_NumBlocksUsed >> 2)), (m_maxUnclusteredBlockNo));
    }

    void Verify ();

public:

    /*!
     \brief   constructor of FBM_SimpleDataVolume
     \param   Allocator [in] allocator
     \param   VolumeSize [in] number of blocks in volume
     \param   VolMode [in] volume mode
     \param   VolNo   [in] volume number
     \param   ClusterSize [in] size of a cluster in blocks
     */
    FBM_SimpleDataVolume (SAPDBMem_IRawAllocator &Allocator,
        IOMan_BlockCount VolumeSize,
        RTE_VolumeAccessMode VolMode,
        IOMan_VolumeNo VolNo,
        IOMan_BlockCount ClusterSize)
    :
    FBM_DataVolume(Allocator, VolumeSize, VolMode, VolNo),
    m_NumBlocksFreeInSection (Allocator),
    m_NumBlocksMarkedForBackUpInSection (Allocator),
    m_NumBlocksFreeAfterSVPInSection (Allocator),
    m_clusterUsageList (Allocator),
    m_usedClusters (0),
    m_lastAssignedCluster (0),
    m_reservedClusters (0),
    m_usedUnclusteredBlocks(0),
	m_NumClusteredBlocksMarkedForBackup(0),
    m_clusterSize (ClusterSize)
    {
    }


    bool InitVolume(const IOMan_ClusterCount numCluster);

    void RestoreAllBlockStatesMarkedForBackup (IOMan_BlockCount &NumBlocksRestoredToFreeAfterSVP);

    void RestoreBlockStateMarkedForBackup (
        IOMan_BlockNo BlockNo,
        SAPDB_Bool &bRestoredStateIsFreeAfterSVP);

    void SetAllBlocksMarkedAsFreeAfterSVPToFree ();
	
	void InitActBlockNoForBackup() 
	{
		FBM_DataVolume::InitActBlockNoForBackup();
		
		m_ActClusterNoForBackup = m_maxClusterNo;
	}	
	
	SAPDB_Bool IsBackupRunning() const
	{
		return FBM_DataVolume::IsBackupRunning() || m_ActBlockNoForBackup.IsValid();
	}

    void GetNextBlocksForBackUp (
        const IOMan_BlockCount          maxBlocksWanted,
        const SAPDB_Bool                searchClustered,
        IOMan_ComplexClusterAddress&    cluster );
            
    IOMan_BlockNo GetMultFreeBlocks (
        const IOMan_BlockCount NumBlocksWanted,
        IOMan_BlockCount &NumBlocksSupplied);

    IOMan_BlockNo GetFreeBlock ();

    void SetBlockStateToBackUp (IOMan_BlockNo BlockNo)
    {
        FBM_DataVolume::SetBlockStateToBackUp (BlockNo);

        ++ m_NumBlocksMarkedForBackUpInSection[BlockNo>>m_LogSectionSize];
		
		if (BlockNo >= m_minClusteredBlockNo)
		{
			++m_NumClusteredBlocksMarkedForBackup;
		}
        
#       ifdef SAPDB_SLOW
		if( FBM_Check.ChecksLevel( 5 ))
			Verify ();
#       endif
    }

	void SetBlockStateToFree (IOMan_BlockNo BlockNo)
	{
		SetBlockStateToFreeNoCheck(BlockNo);
		
#       ifdef SAPDB_SLOW
		if (FBM_Check.ChecksLevel(5))
			Verify ();
#       endif		
	}
	

    void SetBlockStateToOccupied (IOMan_BlockNo BlockNo);

    void SetBlockStateToFreeAfterSVP (IOMan_BlockNo BlockNo)
    {
        /* set new block state */
        FBM_DataVolume::SetBlockStateToFreeAfterSVP (BlockNo);

        ++m_NumBlocksFreeAfterSVPInSection[BlockNo>>m_LogSectionSize];
#       ifdef SAPDB_SLOW
		if( FBM_Check.ChecksLevel( 5 ))
			Verify ();
#       endif
   }

    /// check if cluster border can be lowered
    bool testAddClusterSpace(IOMan_ClusterCount numCluster);

    /// check if cluster border can be lowered
    bool testClusterSpace(IOMan_ClusterCount numCluster);

    /*!
       @brief  reserve additional space for clusters on the volume
       @param  numCluster [in] additional clusters to reserve
       @return success
    */
    bool reserveAddClusterSpace(IOMan_ClusterCount numCluster);

    /*!
       @brief  reserve space for clusters on the volume
       @param  numCluster [in] total number of clusters to reserve
       @return success
    */
    bool reserveClusterSpace(IOMan_ClusterCount numCluster);

    /// return reserved space for clusters in clusters
    IOMan_ClusterCount getClusterSpace() const
    {
        return m_reservedClusters;
    }

    /// returns max assigned cluster number
    IOMan_ClusterNo CalcMaxClusterNo();

    // returns number of free blocks for unclustered blocks
    IOMan_BlockCount GetNumBlocksFree() const
     {
         if (!m_IsToDrop)
         {
             return m_maxUnclusteredBlockNo + 1 - m_usedUnclusteredBlocks;
         }
         else  // simulate full volume, do not write!
         {
             return 0;
         }
     }

    IOMan_ClusterCount GetNumClustersFree() const
    {
        if (!m_IsToDrop)
        {
            return m_reservedClusters - m_usedClusters;
        }
        else  // simulate full volume, do not write!
        {
            return 0;
        }
    }

    bool getFreeCluster(const IOMan_BlockCount clusterSize, IOMan_BlockNo &clusterStart);

	IOMan_BlockCount GetTotalClusterAreaSize() const
	{
		return m_reservedClusters * m_clusterSize;
	}
	
	IOMan_BlockCount GetReservedClusterAreaSize() const
	{
		return m_usedClusters * m_clusterSize;
	}
	
	IOMan_BlockCount GetUsedClusterAreaSize() const
	{
		SAPDB_Int cluster = m_reservedClusters;
		IOMan_BlockCount numBlocks = 0;
		
		while (cluster >= 0)
		{
			numBlocks += m_clusterUsageList[cluster];
			cluster --;
		}
		return numBlocks;
	}

    IOMan_BlockCount GetNumberOfClusteredBlocksToBackup() const
    {
        return m_NumClusteredBlocksMarkedForBackup;
    }
	
    void Dump(
        Kernel_Dump &dump,
        const IOMan_VolumeNo volNo) const;


private:


    IOMan_BlockNo calcClusterBorder(const IOMan_ClusterCount numCluster) const;

    IOMan_ClusterNo getClusterNumber(const IOMan_BlockNo block) const
    {
        SAPDBERR_ASSERT_ARGUMENT(block >= m_minClusteredBlockNo);

        return (m_MaxBlockNo - block) >> m_logClusterSize;
    }

    IOMan_BlockNo getClusterStartAddress(const IOMan_ClusterNo clusterNum) const
    {
		SAPDBERR_ASSERT_ARGUMENT(clusterNum <= static_cast< IOMan_ClusterNo >((m_MaxBlockNo >> m_logClusterSize)));
		
        return m_MaxBlockNo - ((clusterNum << m_logClusterSize) + m_clusterSize) + 1;
    }

    bool findFreeCluster(const IOMan_BlockCount clusterSize, IOMan_BlockNo &clusterStart) const;

    bool hasBlocksForBackup(const IOMan_ClusterNo clusterNo);
    
	void SetBlockStateToFreeNoCheck (IOMan_BlockNo BlockNo)
	{
		m_BlockStateList.SetBlockState (BlockNo, BlockState_Free);

		--m_NumBlocksUsed;
		if ((BlockNo <= m_maxUnclusteredBlockNo))
		{
			m_usedUnclusteredBlocks--;
		}

		m_NumBlocksFreeInSection[BlockNo>>m_LogSectionSize]++;
		if (m_FirstFreeBlock > BlockNo)
		{
			m_FirstFreeBlock = BlockNo;
		}

		if (BlockNo >= m_minClusteredBlockNo)
		{
			IOMan_ClusterNo clusterNumber = getClusterNumber(BlockNo);

			SAPDBERR_ASSERT_STATE(m_clusterUsageList[clusterNumber] > 0);

			m_clusterUsageList[clusterNumber]--;
			if (0 == m_clusterUsageList[clusterNumber]) // last usage
			{
				SAPDBERR_ASSERT_STATE(m_usedClusters > 0);

				m_usedClusters--;
			}
		}
	}

};

#endif //FBM_SIMPLEDATAVOLUME_H
