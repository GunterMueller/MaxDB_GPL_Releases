/*!
  @file     IOMan_IDataVolumeInfo.hpp
  @author   TorstenS
  @ingroup  IOManagement
  @brief    Interface for data volume informations
  @see            

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



#ifndef IOMAN_IDATA_VOLUME_INFO_HPP
#define IOMAN_IDATA_VOLUME_INFO_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h" // tsp00_VFilename

#include "IOManager/IOMan_Types.hpp"
#include "KernelCommon/Kernel_Common.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/* -----------------------------------------------------------------------*/
/*!
   @class   IOMan_DataVolumeInfo
   @brief   This class is used to get informations about the current
            data volume. Note that a data volume could be in state
            offline and therefore same informations could be not available!
 */
/* -----------------------------------------------------------------------*/

class IOMan_DataVolumeInfo
{
    friend class IOMan_DataVolumeInfoIterator;

public:

    /* -----------------------------------------------------------------------*/
    /*! @name Constructors and initialization */
     /*@{*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   construct a handle for the given data volume.
       @param   volumeNo [in] logical identifier of the data volume
       @return  none
     */
    /* -----------------------------------------------------------------------*/

    IOMan_DataVolumeInfo( const IOMan_DeviceNo volumeNo ):m_VolumeNo( volumeNo )
    {}

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! @name Information access */
     /*@{*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method returns the logical data volume identifier
                of the volume.
       @return  IOMan_DeviceNo
     */
    /* -----------------------------------------------------------------------*/

    IOMan_DeviceNo GetVolumeNo() const
    {
        return( m_VolumeNo );
    }

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method returns whether the data volume is in state open.
       @return  SAPDB_Bool
     */
    /* -----------------------------------------------------------------------*/

    SAPDB_Bool IsVolumeOnline() const;

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method returns the name of the data volume.
       @param   volumeName [out] Name of a data volume
       @return  true if volume name was found
     */
    /* -----------------------------------------------------------------------*/

    SAPDB_Bool GetVolumeName( tsp00_VFilename   &volumeName ) const;

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method provides the configured size in pages of the data volume.
       @return  IOMan_BlockCount
     */
    /* -----------------------------------------------------------------------*/

    IOMan_BlockCount GetVolumeSize() const;

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method provides the usable size in pages of the data volume.
       @return  IOMan_BlockCount
     */
    /* -----------------------------------------------------------------------*/

    IOMan_BlockCount GetUsableVolumeSize() const;

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method provides the used size in pages of the data volume.
       @return  IOMan_BlockCount
     */
    /* -----------------------------------------------------------------------*/

    IOMan_BlockCount GetUsedVolumeSize() const;
	
	/*!
		\brief     returns the total number of blocks inside the cluster area 
		\returns   number of blocks in the cluster area
	*/
	IOMan_BlockCount GetTotalClusterAreaSize() const;
	
	/*!
		\brief     returns the number of blocks reserved for clusters, this
				   is the number of occupied clusters * size of one cluster 
		\returns   number of reserved blocks in the cluster area
	*/
	IOMan_BlockCount GetReservedClusterAreaSize() const;
	
	/*!
		\brief     returns the number of blocks used inside the cluster area
		\returns   number of used blocks in the cluster area
	*/
	IOMan_BlockCount GetUsedClusterAreaSize() const;

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method returns whether the data volume is to drop
       @return  SAPDB_Bool
     */
    /* -----------------------------------------------------------------------*/

    SAPDB_Bool IsVolumeToDrop() const;

private:

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/


    void SetVolumeNo( const IOMan_DeviceNo volumeNo )
    {
        m_VolumeNo = volumeNo;
    }

    // no default constructor available;
    IOMan_DataVolumeInfo();

private:

    /*-------------------------------------------------------------------------
    declaration: m_VolumneNo
    description: logical identifier of the data volume
    --------------------------------------------------------------------------*/

    IOMan_DeviceNo  m_VolumeNo;
};



/* -----------------------------------------------------------------------*/
/*!
   @class   IOMan_DataVolumeInfoIterator
   @brief   This class is used to iterator over all data volumes.
            Note that this iterator wil return volume which are not 
            in state online. Therefore you have to call the method
            IsVolumeOnline first.
 */
/* -----------------------------------------------------------------------*/

class IOMan_DataVolumeInfoIterator
{
public:

    /* -----------------------------------------------------------------------*/
    /*! @name Constructors and initialization */
     /*@{*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   constructor; Sets the iterator to the first data volume.
       @return  none
     */
    /* -----------------------------------------------------------------------*/

    IOMan_DataVolumeInfoIterator();

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/


    /* -----------------------------------------------------------------------*/
    /*! @name Iterator access */
     /*@{*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   Sets the iterator to the first logical data volume.
       @return  none
     */
    /* -----------------------------------------------------------------------*/

    void Begin();

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   Checks wheather the iterator is still valid.
       @return  SAPDB_Bool
     */
    /* -----------------------------------------------------------------------*/

    SAPDB_Bool End() const;

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   Sets the iterator the the logical next data volume. The is
                no check whether the volume is available.
       @return  none
     */
    /* -----------------------------------------------------------------------*/

    void operator++();

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   Returns the current data volume description.
       @return  IOMan_DataVolumeInfo&amp;
     */
    /* -----------------------------------------------------------------------*/

    const IOMan_DataVolumeInfo& operator*() const;

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

private:

    /*-------------------------------------------------------------------------
    declaration: m_VolumeInfo
    description: myText
    --------------------------------------------------------------------------*/

    IOMan_DataVolumeInfo  m_VolumeInfo;
};


#endif  /* IOMAN_IDATA_VOLUME_INFO_HPP */
