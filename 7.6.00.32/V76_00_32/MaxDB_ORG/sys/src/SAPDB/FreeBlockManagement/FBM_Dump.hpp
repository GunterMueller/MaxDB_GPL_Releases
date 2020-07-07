/****************************************************************************

  module      : FBM_Dump.hpp.hpp

  -------------------------------------------------------------------------

  responsible : TorstenS

  auhtor      : AlexanderK

  special area: FreeBlockManagement (FBM)
  description : Layout of FNM dump structures


  last changed: 2001-06-12  18:47
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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


#ifndef FBM_DUMP_HPP
#define FBM_DUMP_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "SAPDBCommon/SAPDB_Types.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/// dump structure of fbm manager
struct FBMManager 
{
    /// active flag
    SAPDB_Bool1     dmpActive;
    /// is savepoint triggered
    SAPDB_Bool1     dmpSvpIsTriggered;
    /// filler for alignment
    SAPDB_UInt2     dmpFiller1;
    /// max number of volumes
    SAPDB_UInt4     dmpMaxNumDev;
    /// current number of volumes
    SAPDB_UInt4     dmpNumDev;
    /// last configured volumes
    SAPDB_UInt4     dmpLastVolume;
    /// total number of blocks
    SAPDB_UInt4     dmpTotalNumBlocks;
    /// total number of used blocks
    SAPDB_UInt4     dmpTotalUsedBlocks;
    /// total number of blocks in state 'free after savepoint'
    SAPDB_UInt4     dmpTotalNumBlocksFreeAfterSVP;
    /// total number of blocks in state 'backup' and 'free after savepoint'
    SAPDB_UInt4     dmpTotalNumBlocksBackupFreeAfterSVP;
    /// total number of free blocks
    SAPDB_UInt4     dmpTotalNumBlocksFree;
    /// preferred volume for next block assignment
    SAPDB_UInt4     dmpNextDevToUseForNewBlock;
};

/// dump structure of one data volume 
struct FBMDataDevice
{
    /// volume number
    SAPDB_UInt4     dmpDevNo;
    /// number of blocks on volume
    SAPDB_UInt4     dmpNumBlocks;
    /// number of used blocks on volume
    SAPDB_UInt4     dmpNumBlocksUsed;
    /// number of blocks marked for backup on volume
    SAPDB_UInt4     dmpNumBlocksMarkedForBackup;
	/// number of clustered blocks marked for backup on volume
	SAPDB_UInt4     dmpNumCLBlocksMarkedForBackup;
	/// number of blocks marked in state 'free after savepoint' on volume
    SAPDB_UInt4     dmpNumBlocksFreeAfterSVP;
    /// number of blocks marked in state 'backup' and 'free after savepoint' on volume
    SAPDB_UInt4     dmpNumBlocksBackupFreeAfterSVP;
    /// ...
    SAPDB_UInt4     dmpNumBlocksToAddUntilOptIsReached;
    /// number of sections on volume
    SAPDB_UInt4     dmpNumberOfSections;
    /// current block number used for backup
    SAPDB_UInt4     dmpActBlockNoForBackup;            
    /// hint for allocating new blocks
    SAPDB_UInt4     dmpStartBlockNoToSearchFreeBlock;
    /// highest assigned block number
    SAPDB_UInt4     dmpLastUsedBlock;
    /// first free block
    SAPDB_UInt4     dmpFirstFreeBlock;
    /// flag: volume is to drop
    SAPDB_Bool1     dmpVolumeToDrop;
    /// filler for alignment
    SAPDB_Bool1     dmpFiller3;
    /// filler for alignment
    SAPDB_UInt2     dmpFiller2;
    
    /// upper limit of unclustered blockNo
    SAPDB_UInt4     dmpMaxUnclusteredBlockNo;
    /// lower limit of clustered blockNo
    SAPDB_UInt4     dmpMinClusteredBlockNo;

    /// highest assigned unclustered blockNo
    SAPDB_UInt4     dmpLastUnclusteredUsedBlockNo;
    /// number of used unclustered blocks
    SAPDB_UInt4     dmpUsedUnclusteredBlocks;
    /// number of used clusters
    SAPDB_UInt4     dmpUsedClusters;
    /// number of reserved clusters
    SAPDB_UInt4     dmpReservedClusters;
};

#endif  /* FBM_DUMP_HPP */
