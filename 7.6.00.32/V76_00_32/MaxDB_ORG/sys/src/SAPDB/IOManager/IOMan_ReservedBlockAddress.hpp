/*!
  @file     IOMan_ReservedBlockAddress.hpp
  @author   TorstenS
  @ingroup  IOManagement
  @brief    The class IOMan_ReservedBlockAddress is used to store all 
            reserved block addresses on one central place and to make 
            them accessable.
            The class IOMan_ReservedBlockAddressIterator is used to 
            iterate over a given collection of reserved block addresses.

\if EMIT_LICENCE
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

\endif
*/



#ifndef IOMAN_RESERVEDBLOCKADDRESS_HPP
#define IOMAN_RESERVEDBLOCKADDRESS_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "IOManager/IOMan_BlockAddress.hpp"
#include "IOManager/IOMan_Types.hpp"
#include "KernelCommon/Kernel_Common.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/// Defines the block offset of the IOManager info page
#define IOMAN_INFO_BLOCK_NO             0 // first block within each volume

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/* -----------------------------------------------------------------------*/
/*!
   @class   IOMan_ReservedBlockAddress
   @brief   This class is used to store all reserved block addresses on
            one central place and to make them accessable.
 */
/* -----------------------------------------------------------------------*/

class IOMan_ReservedBlockAddress
{
    /// Declare the IOMan_ReservedBlockAddressIterator to be friend
    friend class IOMan_ReservedBlockAddressIterator;

public:

    /* -----------------------------------------------------------------------*/
    /*! @name Constructors and initialization */
     /*@{*/
    /* -----------------------------------------------------------------------*/

     /* -----------------------------------------------------------------------*/
    /*!
       @brief   Default constructor
       @return  none
     */
    /* -----------------------------------------------------------------------*/

    IOMan_ReservedBlockAddress()
    {}

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! @name Access to the reserved block addresses */
     /*@{*/
    /* -----------------------------------------------------------------------*/
    
    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to get the block address of the volume
                specific IOMan_InfoPage. The volume is defined by the devNo.
       @param   devNo [in] Logical volume identifier 
       @return  IOMan_BlockAddress of the volume specific IOMan_InfoPage
     */
    /* -----------------------------------------------------------------------*/

    IOMan_BlockAddress GetInfoPageAddress( const IOMan_DeviceNo devNo ) const
    {
        return( IOMan_BlockAddress( devNo, IOMAN_INFO_BLOCK_NO ));
    }

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to get the block address of the volume
                specific IOMan_InfoPage. The volume is defined by the devNo.
       @param   devNo [in] Logical volume identifier 
       @return  (IOMan_BlockNo) of the volume specific IOMan_InfoPage
     */
    /* -----------------------------------------------------------------------*/

    static IOMan_BlockNo GetInfoPageBlockNo( const IOMan_DeviceNo devNo )
    {
        return( IOMAN_INFO_BLOCK_NO );
    }
    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to check whether a given block address
                specifies the location of a reserved block.
       @param   block [in] Block address to be checked
       @return  (SAPDB_Bool) true if given block address is system used block
     */
    /* -----------------------------------------------------------------------*/

    SAPDB_Bool IsMember( const IOMan_BlockAddress   &block ) const
    {
        if ( ! block.IsValid() )
        {
            return SAPDB_FALSE;
        }

        if ( IOMAN_INFO_BLOCK_NO == block.GetBlockNo() )
        {
            return SAPDB_TRUE;
        }

        return SAPDB_FALSE;
    }

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

private:


};


/* -----------------------------------------------------------------------*/
/*!
   @class   IOMan_ReservedBlockAddressIterator
   @brief   This class is used to iterate over a given collection of 
            reserved block addresses.
 */
/* -----------------------------------------------------------------------*/

class IOMan_ReservedBlockAddressIterator
{
public:

    /* -----------------------------------------------------------------------*/
    /*! @name Constructors and initialization */
     /*@{*/
    /* -----------------------------------------------------------------------*/
    
    /* -----------------------------------------------------------------------*/
    /*!
       @brief   Constructor
       @param   reservedAddr [in] Reference to the block address collection
       @param   devNo        [in] Logical device number specifies the volume
                                  on which the iterator ha sto work.
       @return  none
     */
    /* -----------------------------------------------------------------------*/

    IOMan_ReservedBlockAddressIterator(
        const IOMan_ReservedBlockAddress    &reservedAddr,
        const IOMan_DeviceNo                devNo )
    :
    m_ReservedAddr( reservedAddr ),
    m_DevNo( devNo ),
    m_CurrentAddr( IOMan_InfoPageAddr )
    {}

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! @name Iterator methods */
     /*@{*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to set the iterator to the next reserved
                block address within the specified volume.
       @return  none
     */
    /* -----------------------------------------------------------------------*/

    void operator ++()
    {
        switch( m_CurrentAddr )
        {
        case IOMan_InfoPageAddr:
            m_CurrentAddr = IOMan_EndAddr;
            break;
        default:
            m_CurrentAddr = IOMan_EndAddr;
        }
    }

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method returns the current reserved block address the 
                iterator points to.
       @return  IOMan_BlockAddress
     */
    /* -----------------------------------------------------------------------*/

    IOMan_BlockAddress GetBlockAddress() const
    {
        if( m_CurrentAddr == IOMan_InfoPageAddr )
            return( m_ReservedAddr.GetInfoPageAddress( m_DevNo ));
        else
        {
            return IOMan_BlockAddress();
        }
    }

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to set the iterator to the first reserved
                block address within the volume.
       @return  none
     */
    /* -----------------------------------------------------------------------*/

    void Begin()
    {
        m_CurrentAddr = IOMan_InfoPageAddr;
    }
    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to determine the end of the reserved
                block address collection. I fthe end is reached the method
                returns true else false.
       @return  (SAPDB_Bool) true if the end has reached
     */
    /* -----------------------------------------------------------------------*/

    SAPDB_Bool End() const
    {
        return( IOMan_EndAddr == m_CurrentAddr );
    }

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

private:

    /* -----------------------------------------------------------------------*/
    /*!
        @brief  Enumeration containing all known reserved block addresses.
     */
    /* -----------------------------------------------------------------------*/

    typedef enum
    {
        IOMan_InfoPageAddr = 0,
        IOMan_EndAddr
    }
    IOMan_ReservedBlockAddressEnum;

    /* -----------------------------------------------------------------------*/
    /*!
        @brief  Is a reference to the collection of all reserved.block addresses
                of one volume.
     */
    /* -----------------------------------------------------------------------*/

    const IOMan_ReservedBlockAddress  &m_ReservedAddr;
    
    /* -----------------------------------------------------------------------*/
    /*!
        @brief  Is the logical device number of the current volume on which the
                 iterator works.
     */
    /* -----------------------------------------------------------------------*/

    IOMan_DeviceNo  m_DevNo;

    /* -----------------------------------------------------------------------*/
    /*!
        @brief  Is the current reserved block address the iterator points on.
     */
    /* -----------------------------------------------------------------------*/

    IOMan_ReservedBlockAddressEnum  m_CurrentAddr;
};


#endif  /* IOMAN_RESERVEDBLOCKADDRESS_HPP */
