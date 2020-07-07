/*!
    @file           IOMan_ILogInfo.hpp
    @author         TorstenS
    @ingroup        IOManagement
    @brief          This module is used  for log area information.

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


#ifndef IOMAN_ILOGINFO_HPP
#define IOMAN_ILOGINFO_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

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

/*!
  @interface    IOMan_ILogInfo
  @brief        This interface is used for log area information.
 */


class IOMan_ILogInfo
{
public:

    /*!
       @brief   This method returns the access to the IOMan_ILogInfo interface
                of component IOManager. It is assumed that the IOManager singleton 
                is available! No CreateInstance call will be done..
       @return  Access to the IOMan_ILogInfo interface
     */
    static IOMan_ILogInfo& GetInstance();
    
    /*!
        @brief  This method returns the io blocking factor in pages for read and write access 
                The value is only available when the IoManager singleton is available
        @return (SAPDB_UInt)
    */
    virtual SAPDB_UInt GetLogIOBlockCount() const = 0;
    
    /*!
        @brief  Returns the io blocking factor too, but this call will read the value from
                the RTE resp. config file. This is not so fast, but it is assured in contrast
                to GetLogIOBlockCount that the value is available.
        @return (SAPDB_UInt)
    */
    static SAPDB_UInt ReadLogIOBlockCount();

    /*!
       @brief   This method provides the number of configured log volumes.
       @return  SAPDB_Int
     */
    virtual SAPDB_Int ConfiguredLogVolumes() const = 0;

    /*!
       @brief   This method provides the maximum number of configurable 
                log volumes.
       @return  SAPDB_Int
     */
    virtual SAPDB_Int MaxConfigurableLogVolumes() const = 0;

    /*!
       @brief   This method provides whether the log area consists of mirrored
                log volumes.
       @return  SAPDB_Int
     */
    virtual SAPDB_Bool IsLogMirrored() const = 0;

    /*!
       @brief   This method is used to determine the name of a log volume
                which is marked as bad resp. not accessable, because it's
                removed. This method is only available in mode admin and
                only if log mode dual is configured.(GET BAD LOG VOLUME)
       @param   taskId [in] Identification of the calling task.
       @param   badDevName [out] Name of the bad log volume
       @return  true means bad log volme found
     */
    virtual SAPDB_Bool GetBadLogVolume(
        const tsp00_TaskId  taskId,
        tsp00_VFilename     &badDevName ) = 0;

    /*!
       @brief   This method returns the logical identifier of for the given
                log volume name. If the volume name doesn't correspond to
                a valid volume the return value is false. In case of a mirrored 
                log, it is returned whether the given log volume name identifies 
                the primary volume or the secondary of the log device.
       @param   devName     [in] Name of a log volume
       @param   devNo      [out] Logical volume identifier
       @param   bIsPrimary [out] Is primary log volume
       @return  true if volume name was found
     */
    virtual bool GetLogVolumeNo(
        tsp00_VFilename     &devName,
        IOMan_DeviceNo      &devNo,
        SAPDB_Bool          &bIsPrimary ) = 0;

    /*!
       @brief   This method returns the name of the log volume identified
                by the given logical identifier.
       @param   devNo   [in ] Logical volume identifier
       @param   devName [out] Name of a data volume
       @return  true if volume name was found
     */
    virtual bool GetLogVolumeName(
        const IOMan_DeviceNo        devNo,
        tsp00_VFilename             &devName ) const = 0;

    /*!
       @brief   This method returns the name of the mirrored log volume 
                identified by the given logical identifier. This method 
                is only available if log mode dual is configured.
       @param   devNo   [in ] Logical volume identifier
       @param   devName [out] Name of a data volume
       @return  true if volume name was found
     */
    virtual bool GetMirrLogVolumeName(
        const IOMan_DeviceNo        devNo,
        tsp00_VFilename             &devName ) const = 0;

    /*!
       @brief   This method provides the configured size in pages of the 
                specified log volume. It is assumed that the volume 
                identifier is valid.
       @param   devNo [in] Logical volume identifier
       @return  IOMan_BlockCount
     */
    virtual IOMan_BlockCount GetLogVolumeSize( const IOMan_DeviceNo devNo ) const = 0;

    /*!
       @brief   This method provides the usable size in pages of the 
                specified log volume. It is assumed that the volume 
                identifier is valid.
       @param   devNo [in] Logical volume identifier
       @return  IOMan_BlockCount
     */
    virtual IOMan_BlockCount GetLogVolumeUsableSize( const IOMan_DeviceNo devNo ) const = 0;

    /*!
       @brief   This method returns the total number of pages storable within
                all log volumes  This number is only available if all log volumes
                are in mode open, else zero is returned.
       @return  SAPDB_Int4
     */

    virtual SAPDB_Int4 TotalUsableLogPages() const = 0;
};




#endif  /* IOMAN_ILOGINFO_HPP */
