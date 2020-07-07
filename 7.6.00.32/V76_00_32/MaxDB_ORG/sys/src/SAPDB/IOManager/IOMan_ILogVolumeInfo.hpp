/*!
  @file     IOMan_ILogVolumeInfo.hpp
  @author   TorstenS
  @ingroup  IOManagement
  @brief    Interface for log volume informations
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



#ifndef IOMAN_ILOG_VOLUME_INFO_HPP
#define IOMAN_ILOG_VOLUME_INFO_HPP


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
   @class   IOMan_LogVolumeInfo
   @brief   This class is used to get informations about the current
            log volume. Note that a log volume could be in state
            offline and therefore same informations could be not available!
 */
/* -----------------------------------------------------------------------*/

class IOMan_LogVolumeInfo
{
    friend class IOMan_LogVolumeInfoIterator;

public:

    /* -----------------------------------------------------------------------*/
    /*! @name Constructors and initialization */
     /*@{*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*!
       @brief          construct a handle for the given log volume.
       @param          volumeNo [in] logical identifier of the log volume
       @return         none
     */
    /* -----------------------------------------------------------------------*/

    IOMan_LogVolumeInfo( const IOMan_DeviceNo volumeNo ):m_VolumeNo( volumeNo )
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
       @brief   This method returns the logical log volume identifier
                of the volume.
       @return  IOMan_DeviceNo
     */

    IOMan_DeviceNo GetVolumeNo() const
    {
        return( m_VolumeNo );
    }

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method returns whether the log volume is in state open.
       @return  SAPDB_Bool
     */
    /* -----------------------------------------------------------------------*/

    SAPDB_Bool IsVolumeOnline() const;

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method returns the name of the mirrored log volume.
                This method is only available if log mode dual is configured.
       @param   mirrVolumeName [out] Name of a mirrored log volume
       @return  true if mirrored volume name was found
     */
    /* -----------------------------------------------------------------------*/

    SAPDB_Bool GetMirrVolumeName( tsp00_VFilename   &mirrVolumeName ) const;
    
    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method returns the name of the log volume.
       @param   volumeName [out] Name of a log volume
       @return  true if volume name was found
     */
    /* -----------------------------------------------------------------------*/

    SAPDB_Bool GetVolumeName( tsp00_VFilename   &volumeName ) const;

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method returns the name of the log volume.
       @param   volumeName [out] Name of a log volume
       @return  true if volume name was found
     */
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method provides the configured size in pages of the log volume.
       @return  IOMan_BlockCount
     */
    /* -----------------------------------------------------------------------*/

    IOMan_BlockCount GetVolumeSize() const;

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method provides the usable size in pages of the log volume.
       @return  IOMan_BlockCount
     */
    /* -----------------------------------------------------------------------*/


    IOMan_BlockCount GetUsableVolumeSize() const;

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

private:

    void SetVolumeNo( const IOMan_DeviceNo volumeNo )
    {
        m_VolumeNo = volumeNo;
    }

    // no default constructor available;
    IOMan_LogVolumeInfo();

private:

    /*-------------------------------------------------------------------------
    declaration: m_VolumneNo
    description: logical identifier of the log volume
    --------------------------------------------------------------------------*/

    IOMan_DeviceNo  m_VolumeNo;
};


/* -----------------------------------------------------------------------*/
/*!
   @class   IOMan_LogVolumeInfoIterator
   @brief   This class is used to iterator over all log volumes.
            Note that this iterator wil return volume which are not 
            in state online. Therefore you have to call the method
            IsVolumeOnline first.
 */
/* -----------------------------------------------------------------------*/

class IOMan_LogVolumeInfoIterator
{
public:

    /* -----------------------------------------------------------------------*/
    /*! @name Constructors and initialization */
     /*@{*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   constructor; Sets the iterator to the first log volume.
       @return  none
     */
    /* -----------------------------------------------------------------------*/

    IOMan_LogVolumeInfoIterator();

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/


    /* -----------------------------------------------------------------------*/
    /*! @name Iterator access */
     /*@{*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   Sets the iterator to the first logical log volume.
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
       @brief   Sets the iterator the the logical next log volume. The is
                no check whether the volume is available.
       @return  none
     */
    /* -----------------------------------------------------------------------*/

    void operator++();

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   Returns the current log volume description.
       @return  IOMan_LogVolumeInfo&amp;
     */
    /* -----------------------------------------------------------------------*/

    const IOMan_LogVolumeInfo& operator*() const;

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

private:

    /*-------------------------------------------------------------------------
    declaration: m_VolumeInfo
    description: myText
    --------------------------------------------------------------------------*/

    IOMan_LogVolumeInfo  m_VolumeInfo;
};

#endif  /* IOMAN_ILOG_VOLUME_INFO_HPP */
