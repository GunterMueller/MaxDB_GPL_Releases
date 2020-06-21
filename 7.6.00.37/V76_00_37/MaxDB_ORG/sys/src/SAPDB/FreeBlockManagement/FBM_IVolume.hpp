/*!
  @file           FBM_IVolume.hpp
  @author         Henrik
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

#ifndef FBM_IVOLUME_H
#define FBM_IVOLUME_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "FreeBlockManagement/FBM_Types.hpp"
#include "IOManager/IOMan_Types.hpp"
#include "KernelCommon/Kernel_Common.hpp"
#include "RunTime/RTE_Types.h"

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/
class Kernel_Dump;


/*!
  @class          FBM_IVolume
  @brief          representation of one data volume
 */
class FBM_IVolume 
{
public:

    /*!
        \brief     initialize volume after allocation
        \param     numCluster [in] number of allocated clusters
        \returns   success state of initVolume
    */
    virtual bool InitVolume(const IOMan_ClusterCount numCluster) = 0;
    
    /*!
        \brief     return the fbm state of a block
        \param     BlockNo [in] number of the block
        \returns   fbm state of the block
    */
    virtual FBM_BlockState GetBlockState(IOMan_BlockNo BlockNo) const = 0;

    /*!
        \brief     set the fbm state of a block to free
        \param     BlockNo [in] number of the block
    */
    virtual void SetBlockStateToFree (IOMan_BlockNo BlockNo) = 0;
    
    /*!
        \brief     set the fbm state of a block to occupied
        \param     BlockNo [in] number of the block
    */
    virtual void SetBlockStateToOccupied (IOMan_BlockNo BlockNo) = 0;
    
    /*!
        \brief     returns the number of free blocks
        \returns   number of free blocks
    */
    virtual IOMan_BlockCount GetNumBlocksFree() const = 0;

    /*!
        \brief     returns the number of free clusters
        \returns   number of free clusters
    */
    virtual IOMan_ClusterCount GetNumClustersFree() const = 0;

    /*!
     *  \brief   reserves space for a number of clusters
     *  \param   numCluster [in] total number of clusters on volume
     *  \returns success
     */
    virtual bool reserveClusterSpace(IOMan_ClusterCount numCluster) = 0;

    /*!
        \brief     returns the number of used blocks
        \returns   number of used blocks
    */
    virtual IOMan_BlockCount GetNumBlocksUsed() const = 0;

    /*!
        \brief     returns the number of blocks marked for backup
        \returns   number of used blocks
    */
    virtual IOMan_BlockCount GetNumBlocksMarkedForBackup() const = 0;

	/*!
		\brief     returns the total number of blocks inside the cluster area of a datavolume 
		\returns   number of blocks in the cluster area
	*/
	virtual IOMan_BlockCount GetTotalClusterAreaSize() const = 0;
	
	/*!
		\brief     returns the number of blocks reserved for clusters, this
		           is the number of occupied clusters * size of one cluster 
		\returns   number of reserved blocks in the cluster area
	*/
	virtual IOMan_BlockCount GetReservedClusterAreaSize() const = 0;

	/*!
		\brief     returns the number of blocks used inside the cluster area
		\returns   number of used blocks in the cluster area
	*/
	virtual IOMan_BlockCount GetUsedClusterAreaSize() const = 0;
	
    /*!
        \brief     returns the capacity of the volume
        \returns   number of addressable blocks
    */
     virtual IOMan_BlockCount GetNumBlocks() const = 0;

    /*!
        \brief     returns the volume mode of this volume
        \returns   volume mode
    */
    virtual RTE_VolumeAccessMode VolMode() const = 0;

    /*! 
        \brief     returns a continous cluster of free blocks
        \param     NumBlocksWanted [in] size of the requested cluster
        \param     NumBlocksSupplied [out] size of the supplied cluster
        \returns   starting block address of the supplied cluster
    */
    virtual IOMan_BlockNo GetMultFreeBlocks (
        const IOMan_BlockCount  NumBlocksWanted,
        IOMan_BlockCount       &NumBlocksSupplied) = 0;

    /*! 
        \brief     returns a continous cluster of free blocks
        \param     clusterSize [in] size of the requested cluster
        \param     clusterStart [out] start address of the cluster
        \returns   success
    */
    virtual bool getFreeCluster(
    const IOMan_BlockCount clusterSize,
    IOMan_BlockNo &clusterStart) = 0;

    /*!
        \brief    returns the block number of a single free block
        \returns  block number of the free block
    */
    virtual IOMan_BlockNo GetFreeBlock () = 0;

    /*!
        \brief   insert all state information of a data volume into the 
                  kernel dump file
        \param   dump [in/out] kernel dump file
        \param   volNo [in] logical identifier for a data volume
    */

    virtual void Dump(Kernel_Dump &dump, const IOMan_VolumeNo volNo) const = 0;

    /*!
        \brief   dump all used blocks to knlmsg file
    */
    virtual void DumpUsedBlocks() const = 0;

        /*!
        \brief  Check if the given blockNo is within the lower and upper bound
                of the volume.
        \param  blockNo [in] offset within volume
        \return (bool) true if blockNo is valid
    */
    virtual bool IsBlockNoValid( const IOMan_BlockNo  blockNo ) const = 0;
};

#endif //FBM_IVOLUME_H
