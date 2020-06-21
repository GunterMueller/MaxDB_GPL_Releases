/*!
  @file     IOMan_IDataManager.hpp
  @author   TorstenS
  @ingroup  IOManagement
  @brief    Interface for data volume administration.
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



#ifndef IOMAN_IDATAMANAGER_HPP
#define IOMAN_IDATAMANAGER_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "IOManager/IOMan_Types.hpp"
#include "IOManager/IOMan_ReservedBlockAddress.hpp"

class RTETask_ITask;
class Trans_Context;
class Msg_List;
class Converter_PageNoIterator;

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
   @interface   IOMan_IDataManager
   @brief       This interface is used for data volume administration purpose.
 */
/* -----------------------------------------------------------------------*/


class IOMan_IDataManager
{
public:

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method returns the access to the IOMan_IDataManager interface
                of component IOManager. It is assumed that the IOManager singleton 
                is available! No CreateInstance call will be done.
       @return  Access to the IOMan_IDataManager interface
     */
    /* -----------------------------------------------------------------------*/

    static IOMan_IDataManager& GetInstance();

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to bring all data volumes being in mode
                open in mode close.
       @param   taskId [in] Identification of the calling task.
       @return  none
     */
    /* -----------------------------------------------------------------------*/

    virtual void CloseAllDataVolumes( const tsp00_TaskId  taskId ) = 0;

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to create, format and open all data 
                volumes given within the data base configuration file.
                Additional it is checked whether the sum of all data pages
                within all data volumes fulfils the minimal space requirements 
                of  a sapdb data base. After checking the data base size
                the component FBM is generated and initialized. Precondition
                is that all data volumes are offline.
       @param   taskId [in] Identification of the calling task.
       @return  true means successful creation; else false
     */
    /* -----------------------------------------------------------------------*/

    virtual bool CreateAllDataVolumes( const tsp00_TaskId  taskId ) = 0;

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to bring all data volumes specified
                within the data base configuration file from mode close into
                mode open. It is assumed that all data volumes are generated
                with the method CreateAllDataVolumes. Additional it is checked
                whether all configured data volumes are opened and the configuration
                sequence is right. This is done to validate if the database 
                configuration was changed in a not offical way. If no errors 
                occur the component FBM is generated and restarted. It's no
                precondition that all data volumes are offline. If an error 
                occur the whole data area will be closed.
       @param   taskId [in] Identification of the calling task.
       @return  true means succeful execution; else false
     */
    /* -----------------------------------------------------------------------*/

    virtual bool OpenAllDataVolumes( const tsp00_TaskId   taskId ) = 0;

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to bring one data devspave identified by
                the given DevNo from mode closed into mode open. If the data
                volume is not stored in the data base configuration file or 
                the open method fails an error is returned.
       @param   taskId [in] Identification of the calling task.
       @param   devNo [in] Logical volume identifier
       @return  true means succeful execution; else false
     */
    /* -----------------------------------------------------------------------*/
    
    virtual bool OpenOneDataVolume( 
        const tsp00_TaskId    taskId, 
        const IOMan_DeviceNo  devNo ) = 0;

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to add a new data volume to the data area.
       @param   taskId [in] Identification of the calling task.
       @param   devName [in] Name of the new volume
       @param   devSize [in] Size of the new devsppace in pages
       @param   devNo [in] Logical indentifier of the volume
       @return  true means that add data volume was successful
     */
    /* -----------------------------------------------------------------------*/

    virtual SAPDB_Bool AddDataVolume(
        const tsp00_TaskId      taskId,
        tsp00_VFilename         &devName,
        const IOMan_BlockCount  devSize,
        const IOMan_DeviceNo    devNo ) = 0;

    /*!
       @brief   drops one data volume
                scans the converter for all pages on the given volume, reads
                pages into the data cache und flushes them back to a new position
                moves converter-, filedir- and restart pages to a new position
       @param   trans   [in] trans 
       @param   msgList [out] message list 
       @param   volNo   [in] id of the volume
       @return  bool (true, if successful else false)
     */
    virtual IOMan_ReturnCode DropDataVolume(
        Trans_Context           &trans,
        Msg_List                &msgList,
        const IOMan_DeviceNo    volNo ) = 0;

    /*!
       @brief   removes an empty volume from IOMan data area and the FBM,
                finishes drop volume command
       @param   task     [in] task 
       @param   msgList [out] message list 
       @param   volNo    [in] id of the volume
       @return  bool (true, if successful else false)
     */
    virtual IOMan_ReturnCode RemoveDroppedDataVolume(
        RTETask_ITask           &task,
        Msg_List                &msgList,
        const IOMan_DeviceNo    volNo ) = 0;

    /*!
       @brief          read all pages on one volume into the data cache
       @param          task    [in]       task 
       @param          msgList [in|out] message list
       @param          volno   [in] volume number to be dropped
       @return         bool ( true, if successful; else false)
     */
    virtual bool SrvTaskDropVolume(
        RTETask_ITask       &task,
        Msg_List            &msgList,
        Converter_PageNoIterator &pageIter) = 0;

    /*!
       @brief   get the volNo of the first data volume in the config file
       @return  IOMan_DeviceNo
     */
    virtual IOMan_DeviceNo GetFirstConfiguredDataVolumeNo() const = 0;

    /*!
       @brief   get the address of the restart page
                if the restart page address is unknown, reads the IOMan info page on
                the first configured data volume to get the current address
       @param   taskId [in] Identification of the calling task.
       @return  IOMan_BlockAddress
     */
    virtual const IOMan_BlockAddress& GetRestartPageAddress( const tsp00_TaskId taskId ) = 0;

    /*!
       @brief   checks if the restart page is on the volume to drop
       @return  bool
     */
    virtual bool IsRestartPageOnDropVolume() const = 0;

    /*!
       @brief   This method moves the restart record to another location
                this is needed during drop volume if the current restart
                page is located on the volume to drop
       @param   taskId [in] Identification of the calling task.
       @return  bool
     */
    virtual SAPDB_Bool MoveRestartPage(const tsp00_TaskId  taskId ) = 0;

    /*!
       @brief   This method prepares the volume to prevent new allocation
                of blocks.
       @param   task    [in] Identification of the calling task.
       @param   msgList [in] message list for error messages
       @return  bool

        PrepareVolumeForDrop is called by Log_Savepoint during savepoint

     */
    virtual bool PrepareVolumeForDrop(
        const RTETask_ITask     &task,
        Msg_List                &msgList) = 0;

    /*!
       @brief   This method aborts a drop volume
       @param   taskId  [in] Identification of the calling task.
       @param   msgList [out] Message list for errors
       @param   reason  [in] cause code for aborting the drop volume
       @return  bool
     */
    virtual bool AbortDropDataVolume( RTETask_ITask           &task,
                                      Msg_List &msgList,
                                      const IOMan_ReturnCode reason) = 0;

};


#endif  /* IOMAN_IDATAMANAGER_HPP */
