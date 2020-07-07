/*!
    @file           IOMan_BlockAddress.hpp
    @author         TorstenS
    @author         TillL
    @ingroup        IOManagement
    @brief          This class is used to offer packed and unpacked block
                    address handler. A block address is needed for volume
                    io. It contains the logical volume id and the position
                    within the volume. The counting of volume id's starts
                    with one and the counting of the offset with zero.
                    The packed format of a block address is needed if the
                    address has to be stored on a page, for example on
                    a converter page. Each packed block address could be 
                    transformed into an unpacked form and vice versa.

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


#ifndef IOMAN_BLOCKADDRESS_HPP
#define IOMAN_BLOCKADDRESS_HPP


#include "IOManager/IOMan_Types.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"

class IOMan_PackedBlockAddress;

/*!
    @class  IOMan_BlockAddress
    @brief  Defines a handle to a logical block address for i/o. This handle
            contains a logical volume number and a block offset in an unpacked
            form.
 */

class IOMan_BlockAddress
{
public:

    /*!
        @brief  Default constructor. Creates an invalid block address.
        @return none
     */

    IOMan_BlockAddress()
    {}

    /*!
        @brief  Constructor; sets block address to a valid address
        @param  devno [in] volume id
        @param  blockpos [in] block position on volume
        @return none
     */

    IOMan_BlockAddress(
        const IOMan_VolumeNo   &devno,
        const IOMan_BlockNo    &blockpos)
    :
    m_VolumeNo(devno),
    m_BlockPos(blockpos)
    {}

    /*!
       @brief   Assigns block address
       @param   devno    [in] volume id
       @param   blockpos [in] block position on volume
       @return  none
     */

    void SetAddress(
        const IOMan_VolumeNo   &devno,
        const IOMan_BlockNo    &blockpos)
    {
        m_VolumeNo = devno;
        m_BlockPos = blockpos;
    }

    /*!
       @brief   Converts block address into packed format
       @param   devnobitcount [in] number of bits used for persistent volume id
                                   representation
       @return  (IOMan_PackedBlockAddress) address in packed format
     */

    IOMan_PackedBlockAddress Pack( const SAPDB_UInt devnobitcount) const;

    /*!
        @brief  Returns volume id of this block address
        @return (IOMan_VolumeNo) volume id
     */

    const IOMan_VolumeNo& GetDeviceNo() const
    {
        SAPDBERR_ASSERT_STATE(IsValid());

        return m_VolumeNo;
    }
    
    /*!
        @brief  Returns volume id of this block address
        @return (IOMan_VolumeNo) volume id
     */

    const IOMan_VolumeNo& GetVolumeNo() const
    {
        SAPDBERR_ASSERT_STATE(IsValid());

        return m_VolumeNo;
    }
    /*!
        @brief  Sets block pos of this block address
        @return none
     */

    void SetBlockNo( const IOMan_BlockNo &BlockPos )
    {
        m_BlockPos = BlockPos;
    }

    /*!
       @brief   Returns block pos of this block address
       @return  IOMan_BlockNo) block position on volume
     */

    const IOMan_BlockNo& GetBlockNo() const
    {
        SAPDBERR_ASSERT_STATE(IsValid());

        return m_BlockPos;
    }

    /*!
        @brief  Assigns an invalid block address
        @return  none
     */

    void Invalidate()
    {
        m_VolumeNo.Invalidate();
    }

    /*!
        @brief  Checks whether actual block address is valid
        @return (bool)
     */

    bool IsValid() const
    {
        return m_VolumeNo.IsValid();
    }

    /*!
       @brief   Compares with another block address
       @param   block [in] other block address
       @return  (bool) true if equal
     */

    bool operator==( const IOMan_BlockAddress &block ) const
    {
        return(
                  ( m_VolumeNo == block.m_VolumeNo ) &&
                  ( m_BlockPos == block.m_BlockPos ));
    }

private:

    /*!
        @brief  Logical volume id. The counting of volumes starts with id one.
     */

    IOMan_VolumeNo  m_VolumeNo;

    /*!
        @brief  Logical block offset on volume. The counting of offsets starts
                with offset zero.
     */

    IOMan_BlockNo   m_BlockPos;
};

/*!
    @class  IOMan_PackedBlockAddress
    @brief  Defines a handle to a logical block address for i/o. This handle
            contains a logical volume number and a block offset in a packed
            form (four byte unsigned integer).
 */

class IOMan_PackedBlockAddress : public SAPDB_Invalidatable<SAPDB_UInt4, SAPDB_MAX_UINT4>
{
    /// Defines abbreviation for invalidatable type
    typedef SAPDB_Invalidatable<SAPDB_UInt4, SAPDB_MAX_UINT4>     Super;

public:

    /*!
       @brief   address value in packed format
     */

    typedef Super::ValueType    PackedValue;

    /*!
       @brief   Default constructor. Creates an invalid block address.
       @return  none
     */

    IOMan_PackedBlockAddress()
    {}

    /*!
       @brief   Constructor; sets address from an unsigned 4-byte integer
       @param   addr [in] packed address to be set
       @return  none
     */

    IOMan_PackedBlockAddress( PackedValue   addr)
    :
    Super(addr)
    {}

    /*!
       @brief   Converts packed format to unpacked format
       @param   devnobitcount [in] number of bits used for persistent volume id
                                   representation.
       @return  (IOMan_BlockAddress) block address in unpacked format
     */

    IOMan_BlockAddress Unpack( const SAPDB_UInt devnobitcount) const;

    /*!
       @brief   Assigns a special value indicating the reserved state
       @return  none
     */

    void SetAsReserved()
    {
        m_Value = ReservedValue();
    }

    /*!
       @brief   Checks whether actual value is in reserved state
       @return  (bool) true if reserved
     */

    bool IsReserved() const
    {
        return m_Value == ReservedValue();
    }

private:

    /*!
       @brief   Returns the reserved value.
       @return  PackedValue
     */

    static PackedValue ReservedValue()
    {
        static PackedValue val = SAPDB_MAX_UINT4 - 1;
        return val;
    }
};

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

inline IOMan_PackedBlockAddress
IOMan_BlockAddress::Pack( const SAPDB_UInt devnobitcount) const
{
    if (! IsValid() )
        return IOMan_PackedBlockAddress();  // returns invalid address

    SAPDBERR_ASSERT_ARGUMENT(SAPDB_UInt2( GetDeviceNo()) < (1UL << devnobitcount));

    SAPDBERR_ASSERT_ARGUMENT(SAPDB_UInt4( GetBlockNo()) < (1UL << (SAPDB_BITS_PER_UINT4
                             - devnobitcount)));

    return ( (GetBlockNo() << devnobitcount) | GetDeviceNo());
}

// --------------------------------------------------------------------------

inline IOMan_BlockAddress
IOMan_PackedBlockAddress::Unpack( const SAPDB_UInt  devnobitcount) const
{
    if (IsInvalid() || IsReserved())
        return IOMan_BlockAddress();        // returns invalid address

    return IOMan_BlockAddress(
               IOMan_VolumeNo(m_Value & ~(SAPDB_MAX_UINT4 << devnobitcount)),
               IOMan_BlockNo((m_Value & (SAPDB_MAX_UINT4 << devnobitcount)) >> devnobitcount)
           );
}

#endif // IOMAN_BLOCKADDRESS_HPP

