/*!
  @file           FBM_SequentialDataVolume.hpp
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

#ifndef FBM_SEQUENTIALDATAVOLUME_H
#define FBM_SEQUENTIALDATAVOLUME_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "FreeBlockManagement/FBM_IVolume.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
  @class          FBM_SequentialDataVolume
  @brief          representation of one data volume
 */
class FBM_SequentialDataVolume : public FBM_IVolume
{
private:
    const IOMan_BlockNo        m_LastBlock;
    const IOMan_VolumeNo       m_VolNo;
    const RTE_VolumeAccessMode m_VolMode;
    IOMan_BlockNo              m_LastUsedBlock;
    SAPDB_Bool                 m_DeviceIsInCompressingMode;

public:

    /*!
        \brief constructor for FBM_SequentialDataVolume 
        \param  Allocator [in] allocator
        \param  VolumeSize [in] number of blocks in volume
        \param  VolMode [in] volume mode 
        \param  VolNo   [in] volume number
    */
    FBM_SequentialDataVolume (SAPDBMem_IRawAllocator &Allocator,
                              IOMan_BlockCount      VolumeSize,
                              RTE_VolumeAccessMode  VolMode,
                              IOMan_VolumeNo        VolNo)
        :
        FBM_IVolume(),
        m_LastBlock                        (VolumeSize-1),
        m_VolMode                          (VolMode),
        m_DeviceIsInCompressingMode        (false),
        m_VolNo                            (VolNo)
    {}

    bool InitVolume (const IOMan_ClusterCount numCluster);

    FBM_BlockState GetBlockState(IOMan_BlockNo BlockNo) const;

    void SetBlockStateToFree (IOMan_BlockNo BlockNo);

    void SetBlockStateToOccupied (IOMan_BlockNo BlockNo);

    IOMan_BlockCount GetNumBlocksUsed() const;

    IOMan_BlockCount GetNumBlocksFree() const;

    IOMan_ClusterCount GetNumClustersFree() const
    {
        return 0;
    }

    IOMan_BlockCount GetNumBlocksMarkedForBackup() const
    {
        return 0;
    }

	IOMan_BlockCount GetTotalClusterAreaSize() const
	{
		return 0;
	}
	
	IOMan_BlockCount GetReservedClusterAreaSize() const
	{
		return 0;
	}
	
	IOMan_BlockCount GetUsedClusterAreaSize() const
	{
		return 0;
	}
	
    IOMan_BlockCount GetNumBlocks() const
    {
        return m_LastBlock + 1;
    }

    RTE_VolumeAccessMode VolMode() const;

    IOMan_BlockNo GetMultFreeBlocks (
        const IOMan_BlockCount  NumBlocksWanted,
        IOMan_BlockCount       &NumBlocksSupplied);

    IOMan_BlockNo GetFreeBlock ();

    bool getFreeCluster(
        const IOMan_BlockCount clusterSize,
        IOMan_BlockNo &clusterStart)
    {
        return false;
    };

    bool reserveClusterSpace(IOMan_ClusterCount numCluster)
    {
        return false;
    }
 
    void Dump( 
        Kernel_Dump             &dump,
        const IOMan_VolumeNo    volNo) const;

    void DumpUsedBlocks() const // dummy
    {}

    bool IsBlockNoValid( const IOMan_BlockNo  blockNo ) const
    {
        return( blockNo >= 0 && blockNo <= m_LastBlock );
    }
};

#endif //FBM_SEQUENTIALDATAVOLUME_H
