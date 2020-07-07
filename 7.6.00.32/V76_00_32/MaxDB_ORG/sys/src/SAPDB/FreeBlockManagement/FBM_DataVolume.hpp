/*!

  @file           FBM_DataVolume.hpp
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

#ifndef FBM_DATAVOLUME_H
#define FBM_DATAVOLUME_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "Container/Container_Vector.hpp"
#include "FreeBlockManagement/FBM_IDataVolume.hpp"
#include "FreeBlockManagement/FBM_BlockStateList.hpp"

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
  @class          FBM_DataVolume
  @brief          representation of one data volume.
                  'abstract' base class for simpleDataVolume and clusteredDataVolume
 */
class FBM_DataVolume : public FBM_IDataVolume
{
protected: 


    /// last block on volume / size in blocks of the volume
    const IOMan_BlockNo         m_MaxBlockNo;

    /// volume mode
    const RTE_VolumeAccessMode  m_VolMode;

    /// volume number, used for trace only
    const IOMan_VolumeNo        m_VolNo;

    /// position of the backup process
    IOMan_BlockNo           m_ActBlockNoForBackup;            

    /// delta in blocks to the average filling level of the volumes
    IOMan_BlockCount        m_NumBlocksToAddUntilOptIsReached;

    /// number of blocks marked for backup
    IOMan_BlockCount        m_NumBlocksMarkedForBackup;

    /// number of blocks marked for backup to be freed after savepoint
    IOMan_BlockCount        m_NumBlocksBackupFreeAfterSVP;

    /// number of blocks marked as 'free after savepoint'
    IOMan_BlockCount        m_NumBlocksFreeAfterSVP;

    /// number of used blocks
    IOMan_BlockCount        m_NumBlocksUsed;

    /// highest used block number
    IOMan_BlockNo           m_LastUsedBlock;

    /// internal state list for each block
    FBM_BlockStateList      m_BlockStateList;         

    /// volume is in drop mode -> no free blocks available
    SAPDB_Bool              m_IsToDrop;



    /// verify structure of the fbm
    virtual void Verify () = 0;

public:
    
    /*!
        \brief   constructor of FBM_DataVolume
        \param   Allocator [in] allocator
        \param   VolumeSize [in] number of blocks in volume
        \param   VolMode [in] volume mode
        \param   VolNo   [in] volume number
    */
    FBM_DataVolume (SAPDBMem_IRawAllocator &Allocator,
                    IOMan_BlockCount        VolumeSize,
                    RTE_VolumeAccessMode    VolMode,
                    IOMan_VolumeNo          VolNo)
        :
        FBM_IDataVolume(),
        m_MaxBlockNo                           (VolumeSize - 1),
        m_VolMode                              (VolMode),
        m_BlockStateList                       (Allocator),
        m_NumBlocksMarkedForBackup             (0),
        m_NumBlocksBackupFreeAfterSVP          (0),
        m_NumBlocksFreeAfterSVP                (0),
        m_NumBlocksUsed                        (0),
        m_IsToDrop                             (SAPDB_FALSE),
        m_VolNo                                (VolNo)
    {}

    bool InitVolume(const IOMan_ClusterCount numCluster);
    
    FBM_BlockState GetBlockState(IOMan_BlockNo BlockNo) const
    {
        return m_BlockStateList.GetBlockState(BlockNo); 
    }

    FBM_BlockState GetBlockStateAfterBackup(IOMan_BlockNo BlockNo) const
    {
        return  m_BlockStateList.GetBlockStateAfterBackup(BlockNo);  
    }

    void SetBlockStateAfterBackup(IOMan_BlockNo BlockNo, FBM_BlockState BlockState)
    { 
        m_BlockStateList.SetBlockStateAfterBackup (BlockNo, BlockState);

        if (BlockState_FreeAfterSVP == BlockState)
        {
            m_NumBlocksBackupFreeAfterSVP++;
        }
        
#       ifdef SAPDB_SLOW
        if( FBM_Check.ChecksLevel( 5 ))
            Verify ();
#       endif
    }

    void SetBlockStateToBackUp (IOMan_BlockNo BlockNo)
    {
        m_BlockStateList.SetBlockStateAfterBackup (BlockNo, BlockState_Occupied);
        m_BlockStateList.SetBlockState (BlockNo, BlockState_BackUp);
        ++ m_NumBlocksMarkedForBackup;
    }

    void SetBlockStateToFreeAfterSVP (IOMan_BlockNo BlockNo)
    {
        /* set new block state */
        m_BlockStateList.SetBlockState (BlockNo, BlockState_FreeAfterSVP);
        
        /* update counter of blocks marked as free after savepoint */
        ++m_NumBlocksFreeAfterSVP;
    }

    IOMan_BlockCount GetNumBlocks() const
    {
        return m_MaxBlockNo + 1;
    }

    IOMan_BlockCount GetNumBlocksUsed() const
    {
        return m_NumBlocksUsed;
    }

    IOMan_BlockCount GetNumBlocksFree() const
    {
        if (!m_IsToDrop)
        {
            return m_MaxBlockNo + 1 - m_NumBlocksUsed;
        }
        else  // simulate full volume, do not write!
        {
            return 0;
        }
    }

    void PrepareForDrop()
    {
        m_IsToDrop = SAPDB_TRUE;
    }

    void RestoreForDrop()
    {
        m_IsToDrop = SAPDB_FALSE;
    }

    SAPDB_Bool IsToDrop()
    {
        return m_IsToDrop;
    }

    IOMan_BlockCount GetNumBlocksToAddUntilOptIsReached() const
    {
        return m_NumBlocksToAddUntilOptIsReached;
    }

    void SetNumBlocksToAddUntilOptIsReached(IOMan_BlockCount value) 
    {
        m_NumBlocksToAddUntilOptIsReached = value;
    }

    SAPDB_Bool IsBackupRunning() const
    {
        return m_ActBlockNoForBackup.IsValid();
    }
        
    void InitActBlockNoForBackup() 
    {
        m_ActBlockNoForBackup = 0;
    }
        
    IOMan_BlockCount GetNumBlocksMarkedForBackup() const
    {
        return m_NumBlocksMarkedForBackup;
    }

    IOMan_BlockCount GetNumBlocksFreeAfterSVP() const
    {
        return m_NumBlocksFreeAfterSVP;
    }

    RTE_VolumeAccessMode VolMode() const
    {
        return m_VolMode;
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

    void DumpUsedBlocks() const;

    bool IsBlockNoValid( const IOMan_BlockNo  blockNo ) const
    {
        return( blockNo >= 0 && blockNo <= m_MaxBlockNo );
    }
};

#endif //FBM_DATAVOLUME_H
