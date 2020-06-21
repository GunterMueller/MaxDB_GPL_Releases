/*!
    @file           IOMan_ClusterAddress.hpp
    @author         TorstenS
    @ingroup        IOManagement
    @brief          This class is used to offer a handler for a
                    collection of block addresses callled cluster.
                    A cluster consists of a number of related block 
                    addresses. A start block address defines the 
                    beginning of the cluster and a block count the 
                    end, i.e. all blocks of the cluster belong to the 
                    same volume and the offsets are contiguous. 
                    (see also IOMan_BlockAddress.hpp)

\if EMIT_LICENCE

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



\endif
*/

#ifndef IOMAN_CLUSTERADDRESS_HPP
#define IOMAN_CLUSTERADDRESS_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "IOManager/IOMan_BlockAddress.hpp"
#include "IOManager/IOMan_Types.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  FORWARD DECLARATION                                                      *
 *===========================================================================*/


/*===========================================================================*
 *  DECLARATION OF CLASSES, STRUCTURES, TYPES, UNIONS ...                    *
 *===========================================================================*/

/*!
    @class  IOMan_ClusterAddress
    @brief  Defines a handle to a cluster of logical block addresses for i/o. 
            This handle contains a logical volume number, a start block 
            offset and a block count. All blocks of a cluster are related.
            The begin of the clustes is defined by the start address and 
            the end could be calculated by the start address and the block
            count.
 */

class IOMan_ClusterAddress
{
public:

    /*!
       @brief   Block index number within cluster
     */

    typedef SAPDB_UInt2 BlockIndex;

    /*!
       @brief   Default constructor
       @return  none
     */

    IOMan_ClusterAddress()
    : m_BlockCount(0)
    {}

    /*!
       @brief   Constructor
       @param   startBlock [in] block address of first block
       @param   blockCount [in] count of blocks in cluster
       @return  none
     */

    IOMan_ClusterAddress(
        const IOMan_BlockAddress&   startBlock,
        const IOMan_BlockCount      blockCount )
    :m_StartBlock( startBlock ),
    m_BlockCount( blockCount )
    {}

    /*!
        @brief   Assigns cluster
        @param   startBlock  [in] block address of first block
        @param   blockCount [in] count of blocks in cluster
        @return  none
     */

    void SetCluster(
        const IOMan_BlockAddress&   startBlock,
        const IOMan_BlockCount      blockCount )
    {
        m_StartBlock  = startBlock;
        m_BlockCount = blockCount;
    }

    /// Set cluster to invalid
    void Invalidate(){
        m_StartBlock.Invalidate();
        m_BlockCount = 0;
    }

    /*!
        @brief  Checks if actual cluster address is valid.
        @return (bool) true if valid
    */
    bool IsValid() const{
        return m_StartBlock.IsValid();
    }
    /*!
        @brief  Returns count of blocks in the cluster
        @return (IOMan_BlockCount) block count
     */

    IOMan_BlockCount GetBlockCount() const{
        return m_BlockCount;
    }

    /*!
        @brief  Returns block address of requested block in cluster
        @param  index [in] block number within cluster (first block = 0)
        @return (IOMan_BlockAddress) block address
     */

    IOMan_BlockAddress GetBlockAddress( const BlockIndex index ) const
    {
        SAPDBERR_ASSERT_ARGUMENT( index < m_BlockCount );

        return IOMan_BlockAddress(
                   m_StartBlock.GetVolumeNo(),
                   m_StartBlock.GetBlockNo() + index );
    }

    /*!
        @brief  Returns device number of actual cluster address
        @return (IOMan_DeviceNo) device number
     */

    const IOMan_DeviceNo& GetDeviceNo() const{
        return m_StartBlock.GetDeviceNo();
    }

    /*!
        @brief  Returns volume number of actual cluster address
        @return (IOMan_VolumeNo) volume number
     */

    const IOMan_VolumeNo& GetVolumeNo() const{
        return m_StartBlock.GetVolumeNo();
    }

    /*!
        @brief  Returns block pos of requested block in cluster
        @param  index [in] block number within cluster (first block = 0)
        @return (IOMan_BlockNo) block position on device
     */

    IOMan_BlockNo GetBlockNo( const BlockIndex index ) const
    {
        SAPDBERR_ASSERT_ARGUMENT( index < m_BlockCount );

        return IOMan_BlockNo
               ( m_StartBlock.GetBlockNo() + index );
    }

    /*!
        @brief  sets count of blocks in the cluster
        @param  blockCount [in] new number of blocks
     */

    void SetBlockCount( const IOMan_BlockCount blockCount ){
        m_BlockCount = blockCount;
    }
private:

    //block address of first block in cluster
    IOMan_BlockAddress  m_StartBlock;

    //count of blocks in sequence
    IOMan_BlockCount m_BlockCount;
};


/*===========================================================================*
 *  DEFINITION OF METHODS                                                    *
 *===========================================================================*/


#endif // IOMAN_CLUSTERADDRESS_HPP

