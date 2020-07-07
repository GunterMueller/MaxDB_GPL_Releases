/*!**************************************************************************

  module      : Log_ClusterAddress.hpp

  -------------------------------------------------------------------------

  author      : TorstenS
  responsible : UweH

  special area: Logging
  description : This class is used to define a log address for read and write
                operations of log pages on an archive log.

  last changed: 2001-03-12  9:17
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


#ifndef LOG_CLUSTERADDRESS_HPP
#define LOG_CLUSTERADDRESS_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "IOManager/IOMan_Types.hpp"
#include "Logging/Log_Types.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


/*!*****************************************************************************

   class: Log_ClusterAddress

   description: This class is used to define a log address for read and write
                operations of log pages on an archive log.
                 
*******************************************************************************/

class Log_ClusterAddress
{
public:

    /* -----------------------------------------------------------------------*/
    /*! chapter: Constructors and initialization                              */
    /* -----------------------------------------------------------------------*/

    /*!-------------------------------------------------------------------------
    function:     Log_ClusterAddress()
    description:  Constructor. 
    arguments:    Offset       [in] Logical offset within the specified archive log
                  BlockCount   [in] Number of blocks to read or write
    return value: none
    --------------------------------------------------------------------------*/

    Log_ClusterAddress(
        Log_RawDeviceOffset     Offset,
        Log_DeviceBlockCount    BlockCount )
        :
        m_Offset( Offset ),
        m_BlockCount( BlockCount ){}
    
    /* -----------------------------------------------------------------------*/
    /*! endchapter: Constructors and initialization                           */
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! chapter: Access                                                       */
    /* -----------------------------------------------------------------------*/

    /*!-------------------------------------------------------------------------
    function:     GetBlockCount()
    description:  This method returns the number of blocks to be read or write
                  by the next io operation.
    arguments:    none
    return value: Log_DeviceBlockCount
    --------------------------------------------------------------------------*/

    Log_DeviceBlockCount GetBlockCount() const
    {
        return( m_BlockCount );
    }

    /*!-------------------------------------------------------------------------
    function:     GetOffset()
    description:  This method returns the offset within the archive log for
                  the next read or write operation.
    arguments:    none
    return value: Log_RawDeviceOffset
    --------------------------------------------------------------------------*/

    Log_RawDeviceOffset GetOffset() const
    {
        return( m_Offset );
    }

    /* -----------------------------------------------------------------------*/
    /*! endchapter: Access                                                    */
    /* -----------------------------------------------------------------------*/

    /*!-------------------------------------------------------------------------
    function:     SetOffset()
    description:  This method sets the offset
    arguments:    offset [in]
    return value: none
    --------------------------------------------------------------------------*/

    void SetOffset (Log_RawDeviceOffset offset)
    {
        m_Offset = offset;
    }

private:

    /*-------------------------------------------------------------------------
    declaration: m_BlockCount
    description: Number of blocks involved in read or write operations on 
                 an archive log.
    --------------------------------------------------------------------------*/

    Log_DeviceBlockCount    m_BlockCount;

    /*-------------------------------------------------------------------------
    declaration: m_Offset
    description: Block offset on an archive log.
    --------------------------------------------------------------------------*/

    Log_RawDeviceOffset m_Offset;
};

/*!*****************************************************************************

   endclass: Log_ClusterAddress

*******************************************************************************/


#endif  /* LOG_CLUSTERADDRESS_HPP */
