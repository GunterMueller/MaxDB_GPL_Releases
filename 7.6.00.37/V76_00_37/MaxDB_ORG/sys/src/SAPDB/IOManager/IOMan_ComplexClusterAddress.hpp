/*!
    @file       IOMan_ComplexClusterAddress.hpp
    @author     TorstenS
    @ingroup    IOManagement
    @brief      This class is an extension of the IOMan_ClusterAddress. In contrast 
                to the cluster address the complex cluster address offers the 
                possibility to define a cluster including blocks which must not be
                treated as valid resp. occupied. i.e. the complex cluster address
                may be contain invalid blocks. Per default all blocks in the cluster
                are marked as valid. (see also IOMan_ClusterAddress.hpp and
                IOMan_BlockAddress.hpp)

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2000-2006 SAP AG

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

#ifndef IOMAN_COMPLEXCLUSTERADDRESS_HPP
#define IOMAN_COMPLEXCLUSTERADDRESS_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "IOManager/IOMan_ClusterAddress.hpp"
#include "IOManager/IOMan_Types.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"
#include "gbd00.h"

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
    @class  IOMan_ComplexClusterAddress
    @brief  This class is an extension of the IOMan_ClusterAddress. In contrast 
            to the cluster address the complex cluster address offers the 
            possibility to define a cluster including blocks which must not be
            treated as valid resp. occupied. i.e. the complex cluster address
            may be contain invalid blocks. Per default all blocks in the cluster
            are marked as valid.
 */

class IOMan_ComplexClusterAddress : public IOMan_ClusterAddress
{
public:

    /// Block index number within cluster
    typedef IOMan_ClusterAddress::BlockIndex BlockIndex;

    /// Default constructor
    IOMan_ComplexClusterAddress()
    :
    IOMan_ClusterAddress(),
    m_UsageCount( 0 )
    {
        this->InitializeUsageList( 0 );
    }

    /*!
       @brief   Constructor. All blocks in the cluster will be
                marked as valid resp. occupied.
       @param   startBlock [in] block address of first block
       @param   blockCount [in] number of blocks in cluster
     */
    IOMan_ComplexClusterAddress(
        const IOMan_BlockAddress&   startBlock,
        const IOMan_BlockCount      blockCount )
    :
    IOMan_ClusterAddress( startBlock, blockCount ),
    m_UsageCount( 0 )
    {
        this->InitializeUsageList( blockCount );
    }

    /*!
       @brief   Assign a new cluster. All blocks in the cluster will be
                marked as valid resp. occupied.
       @param   startBlock [in] block address of first block
       @param   blockCount [in] number of blocks in cluster
    */
    void SetCluster(
        const IOMan_BlockAddress&   startBlock,
        const IOMan_BlockCount      blockCount )
    {
        IOMan_ClusterAddress::SetCluster( startBlock, blockCount );
        m_UsageCount = 0;
        this->InitializeUsageList( blockCount );
    }
    
    /*!
       @brief   Assign a new cluster. All blocks in the cluster will be
                marked as valid resp. occupied.
       @param   cluster [in] cluster to be assigned
    */
    void SetCluster( const IOMan_ClusterAddress& cluster ){
        this->SetCluster( cluster.GetBlockAddress( 0 ), cluster.GetBlockCount());
    }

    /// Set complex cluster to invalid
    void Invalidate()
    {
        IOMan_ClusterAddress::Invalidate();
        m_UsageCount = 0;
        SAPDB_MemFillNoCheck( m_UsageList, 0x00, sizeof( m_UsageList ));
    }

    /*!
       @brief   Increase block count by one. The given value bBlockIsInUse
                specifies if the new block in the cluster is valid or not.

                Example: cluster is 2/4022(5) After cluster.Append( true ) 
                the cluster looks like this 2/4022(6), e.g. cluster is on 
                volume 2, starts at blockNo 4022 and 6 blocks belongs to  
                him whereby the last block 2/4027 is in use.
       @param   bBlockIsInUse [in] new block is valid
    */
    void Append( const bool bBlockIsInUse )
    {
        const IOMan_BlockCount blockCount = GetBlockCount();
        IOMan_ClusterAddress::SetBlockCount( blockCount + 1 );
        if(  bBlockIsInUse ){
            this->MarkAsUsed( blockCount );
        }else{
            this->MarkAsUnused( blockCount );
        }
    }
    /*!
        @brief   Set block count. If the given block count is greater
                 than the old one all 'new' blocks will be treated as
                 used and the usage count will be increased. If the 
                 given block count is smaller than old one all blocks
                 all block which become invalid will be set to unused
                 if the where used before and the usage count will be
                 adjusted. If new block count is equal to the old one
                 nothing happens. The block count must not be greater
                 than 128 blocks!
        @param   newBlockCount [in] new block count
    */
    void SetBlockCount( const IOMan_BlockCount newBlockCount )
    {
        SAPDBERR_ASSERT_STATE( newBlockCount >= 0 );
        const IOMan_BlockCount oldBlockCount = GetBlockCount();

        if( oldBlockCount == newBlockCount ){
            return;
        }
        if( oldBlockCount < newBlockCount ) // cluster is growing
        {
            for( BlockIndex index = oldBlockCount; index < newBlockCount; ++index ){
                this->MarkAsUsed( index );
            }
        }
        else // cluster is shrinking
        {
            for( BlockIndex index = newBlockCount; index < oldBlockCount; ++index ){
                this->MarkAsUnused( index );
            }
        }
        IOMan_ClusterAddress::SetBlockCount( newBlockCount );
        SAPDBERR_ASSERT_STATE( this->GetUsedBlocks() <= GetBlockCount());
        SAPDBERR_ASSERT_STATE( this->GetUsedBlocks() >= 0 );
    }

    /*!
        @brief   Ask for the block state (used resp. unused) specified 
                 by the given block index. First block index is zero.
        @param   index [in] block address of first block; start with 0
        @return  (bool) true if block is in use; else false
    */
    bool IsBlockInUse( const BlockIndex index ) const
    {
        SAPDBERR_ASSERT_STATE( index <= IOMan_ClusterAddress::GetBlockCount());
        const SAPDB_UInt4 slotNo = this->GetSlotNo( index );
        const SAPDB_UInt4 bitNo  = this->GetBitNo( index );
        SAPDBERR_ASSERT_STATE( slotNo <  sizeof( m_UsageList ));
        return( 0 != ( m_UsageList[ slotNo ] & ( 0x80 >> bitNo )));
    }

    /*!
        @brief   Returns the number of used resp. valid blocks in the cluster
                 If the cluster is invalid no blocks are used.
        @return  (IOMan_BlockCount) number of used blocks
    */
    IOMan_BlockCount GetUsedBlocks() const{
        return m_UsageCount;
    }

private:

    SAPDB_UInt4 GetSlotNo( const BlockIndex index ) const{
        return index / SAPDB_BITS_PER_BYTE;
    }

    SAPDB_UInt4 GetBitNo( const BlockIndex index ) const{
        return index % SAPDB_BITS_PER_BYTE;
    }

    void InitializeUsageList( const IOMan_BlockCount usageCount )
    {

        SAPDB_MemFillNoCheck( m_UsageList, 0x00, sizeof( m_UsageList ));
        for( BlockIndex index = 0; index < usageCount; ++index ){
            this->MarkAsUsed( index );
        }
    }

    void MarkAsUnused( const BlockIndex index )
    {
        SAPDBERR_ASSERT_STATE( index <= IOMan_ClusterAddress::GetBlockCount());
        const SAPDB_UInt4 slotNo = this->GetSlotNo( index );
        const SAPDB_UInt4 bitNo  = this->GetBitNo( index );
        SAPDBERR_ASSERT_STATE( slotNo <  sizeof( m_UsageList ));
        if( 0 == ( m_UsageList[ slotNo ] & ( 0x80 >> bitNo ))){
            return; // already marked as not used
        }
        m_UsageList[ slotNo ] &= ~( 0x80 >> bitNo );
        --m_UsageCount;
        SAPDBERR_ASSERT_STATE( 0 <= m_UsageCount );
    }

    void MarkAsUsed( const BlockIndex index )
    {
        SAPDBERR_ASSERT_STATE( index <= IOMan_ClusterAddress::GetBlockCount());
        const SAPDB_UInt4 slotNo = this->GetSlotNo( index );
        const SAPDB_UInt4 bitNo  = this->GetBitNo( index );
        SAPDBERR_ASSERT_STATE( slotNo <  sizeof( m_UsageList ));
        if( 0 != ( m_UsageList[ slotNo ] & ( 0x80 >> bitNo ))){
            return; // already marked as used
        }
        m_UsageList[ slotNo ] |= ( 0x80 >> bitNo );
        ++m_UsageCount;
        SAPDBERR_ASSERT_STATE( m_UsageCount <= IOMan_ClusterAddress::GetBlockCount());
    }

private:

    // current number of used blocks in a cluster
    IOMan_BlockCount m_UsageCount;

    // to manage 8 blocks one byte is needed. Totaly 16 bytes are needed 
    // to handle the maximum cluster capacity of 128 blocks
    SAPDB_Byte m_UsageList[ MAX_VF_BUFFERS_BD00 / SAPDB_BITS_PER_BYTE ]; 

};

/*===========================================================================*
 *  END CODE                                                                 *
 *===========================================================================*/

#endif // IOMAN_COMPLEXCLUSTERADDRESS_HPP

