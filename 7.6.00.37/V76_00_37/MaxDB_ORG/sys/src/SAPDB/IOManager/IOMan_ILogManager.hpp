/*!
  @file     IOMan_ILogManager.hpp
  @author   TorstenS
  @ingroup  IOManagement
  @brief    Interface for archive log administration and log writer registration.

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



#ifndef IOMAN_ILOGMANAGER_HPP
#define IOMAN_ILOGMANAGER_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "IOManager/IOMan_Types.hpp"
#include "IOManager/IOMan_LogPages.hpp"

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
   @interface   IOMan_ILogManager
   @brief       This interface is used for archive log administration purpose.
 */
/* -----------------------------------------------------------------------*/


class IOMan_ILogManager
{
public:

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method returns the access to the IOMan_ILogManager interface
                of component IOManager. It is assumed that the IOManager singleton 
                is available! No CreateInstance call will be done.
       @return  Access to the IOMan_ILogManager interface
     */
    /* -----------------------------------------------------------------------*/

    static IOMan_ILogManager& GetInstance();

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to bring all archive logs being in mode
                open in mode close.
       @param   taskId [in] Identification of the calling task.
       @return  none
     */
    /* -----------------------------------------------------------------------*/

    virtual void CloseAllLogVolumes( const tsp00_TaskId  taskId ) = 0;

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to create, format and open all archive
                logs specified within the data base configuration file.
                Precondition is that all log volumes are offline.
       @param   taskId [in] Identification of the calling task.
       @return  true means successful creation; else false
     */
    /* -----------------------------------------------------------------------*/

    virtual bool CreateAllLogVolumes( const tsp00_TaskId  taskId ) = 0;

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to bring all archive logs specified
                within the data base configuration file from mode close into
                mode open. It is assumed that all archive logs are generated
                with the method CreateAllArchiveLogs. It's no precondition 
                that all archive logs are offline. In case of a HotStandBy 
                scenario it's possible to start the log area for the slave 
                in a ReadOnly mode. This means that subsequent write call
                yields to an emergency shutdown. If an error occur the whole 
                log area will be closed.
       @param   taskId        [in] Identification of the calling task.
       @param   bOpenForWrite [in] true (default) means that the log area will 
                                   be accessible for read and write calls;
                                   false means that only read accesses are 
                                   allowed. This is used by the HotStandBy solution.
       @return  true means successful open; else false
     */
    /* -----------------------------------------------------------------------*/

    virtual bool OpenAllLogVolumes( 
        const tsp00_TaskId  taskId,
        const SAPDB_Bool    bOpenForWrite = SAPDB_TRUE ) = 0;

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to bring one log devspave identified by
                the given DevNo from mode closed into mode open. If the log
                volume is not stored in the data base configuration file or 
                the open method fails an error is returned. The log volume
                will be opened with read and write access.
       @param   taskId [in] Identification of the calling task.
       @param   devNo [in] Logical volume identifier
       @return  true means succeful execution; else false
     */
    /* -----------------------------------------------------------------------*/

    virtual bool OpenOneLogVolume(
        const tsp00_TaskId    taskId,
        const IOMan_DeviceNo  devNo ) = 0;

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to add a new log volume to the given
                archive log. The proper activation of the new volume must 
                be done by the Log_IOManager.
       @param   taskId      [in] Identification of the calling task.
       @param   devName     [in] Name of the new volume
       @param   mirrDevName [in] Name of the new mirrored volume (log mode dual only)
       @param   devSize     [in] Size of the new devsppace in pages
       @param   devNo       [in] Logical indentifier of the volume
       @param   pages       [in] (nil)-Log pages used for volume formating
       @param   newLogSize  [out] New size in pages of the archive log
       @return  true means that add log volume was successful
     */
    /* -----------------------------------------------------------------------*/

    virtual SAPDB_Bool AddLogVolume(
        const tsp00_TaskId          taskId,
        tsp00_VFilename             &devName,
        tsp00_VFilename             &mirrDevName,
        const IOMan_BlockCount      devSize,
        const IOMan_DeviceNo        devNo,
        IOMan_LogPages              &pages,
        IOMan_BlockCount            &newLogSize ) = 0;

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to repair a corrupted log volume by copy
                the intact log volume of the same log device onto the corrupted.
                If the given log volume isn't bad nothing happens. This
                feature is available in log mode dual only.
                (RESTORE LOG VOLUME 'bad volume name')
       @param   taskId [in] Identification of the calling task.
       @param   badDevName [in] Name of the bad log volume to be reintegrated
       @return  true means that reintegrate log volume was successful
     */
    /* -----------------------------------------------------------------------*/

    virtual SAPDB_Bool ReIntegrateBadLogVolume(
        const tsp00_TaskId  taskId,
        tsp00_VFilename     &badDevName ) = 0;
};


#endif  /* IOMAN_ILOGMANAGER_HPP */
