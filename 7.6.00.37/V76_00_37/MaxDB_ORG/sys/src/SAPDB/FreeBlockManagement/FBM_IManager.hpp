/*!
  @file           FBM_IManager.hpp
  @author         TorstenS
  @ingroup        FBM
  @brief          Interface definition of the FBM

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



#ifndef FBM_IMANAGER_HPP
#define FBM_IMANAGER_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_Types.h"
#include "gsp00.h"
#include "FreeBlockManagement/FBM_Types.hpp"
#include "IOManager/IOMan_BlockAddress.hpp"
#include "IOManager/IOMan_IBlockAddressIterator.hpp"
#include "IOManager/IOMan_ReservedBlockAddress.hpp"
#include "IOManager/IOMan_ClusterAddress.hpp"

// forward
class IOMan_ComplexClusterAddress;
class Kernel_Dump;
class Msg_List;
class RTETask_ITask;

/*!
  @class          FBM_IManager
  \brief          interface for the free block management manager
 */

class FBM_IManager
{

public:

    /*!
       @brief          returns the reference to the sigelton instance of FBM_Manager
       @return         (FBM_Manager&amp;) reference to the instance of FBM_Manager
     */
    static FBM_IManager& Instance ();

    /*!
       @brief   Sets the state of block to 'occupied'. If the block is already
                marked as occupied a error message is written.
       @param   TaskId        [in] task id
       @param   BlockIterator [in] iterator of I/O adresses with the blocks to change
       @return  (bool) true if operation was executed successful

       - If the state could not be changed successfully the kernel aborts if
      the given parameter bAbortIfErrorOccured is set to true.
     */
    virtual bool SetBlockStateToOccupied(
        const tsp00_TaskId                 TaskId,
        IOMan_IBlockAddressIterator &BlockIterator )= 0;
    /*!
       @brief   Sets the state of block to 'occupied'. If the block is already
                marked as occupied a error message is written.
       @param   TaskId       [in] task id
       @param   block [in] I/O adress of the block to change
       @return  (bool) true if operation was executed successful

       - If the state could not be changed successfully the kernel aborts if
      the given parameter bAbortIfErrorOccured is set to true.
     */
    virtual bool SetBlockStateToOccupied (
        const tsp00_TaskId        TaskId,
        const IOMan_BlockAddress &block ) =0;

    /*!
       @brief   add a new volume to the FBM
       @param   TaskId           [in] task id
       @param   VolNo            [in] device number
       @param   VolSize          [in] number of pages the device can accomodate
       @param   VolMode          [in] access mode of the volume 
       @param   restartPageBlock [in] block address of restart record
       @param   numCluster       [in] number of clusters used on this volume
       @return  true if successful executed else false
     */
    virtual bool RegisterVolume(
        const tsp00_TaskId          TaskId,
        const IOMan_VolumeNo        VolNo,
        const IOMan_BlockCount      VolSize,
        const RTE_VolumeAccessMode  VolMode,
        const IOMan_BlockAddress    &restartPageBlock,
        const SAPDB_UInt4           numCluster  ) = 0;

    /*!
        \brief   remove a volume from the fbm
        \param   task [in] current task
        \param   msgList [out] message list for errors
        \param   volNo [in] volume number of the volume to remove
        \return  true if the volume was found and successfully removed
    */
    virtual bool DeRegisterVolume(
        const RTETask_ITask     &task,
        Msg_List                &msgList,
        const IOMan_VolumeNo    volNo) = 0;

    /*!
       \brief   prepares dropping of a volume 
       \param   task [in] current task
       \param   msgList [out] message list for errors
       \param   volNo [in] volume number of the volume to drop
       \return  true if the volume was found and successfully prepared

                a volume in drop mode prevents writing of new pages to this volume
     */
    virtual bool PrepareVolumeForDrop(
        const RTETask_ITask     &task,
        Msg_List                &msgList,
        const IOMan_VolumeNo    volNo) = 0;

    /*!
       \brief   restores normal operation of a volume after an aborted drop volume
       \param   task [in] current task
       \param   msgList [out] message list for errors
       \param   volNo [in] volume number of the volume to restore
       \return  true if the volume was found and successfully restored

           a volume in drop mode prevents writing of new pages to this volume
     */
    virtual bool RestoreVolumeForDrop(
        const RTETask_ITask     &task,
        Msg_List                &msgList,
        const IOMan_VolumeNo    volNo) = 0;

    /*!
       \brief   checks the 'drop state' of the volume
       \param   volNo [in] volume number of the volume to restore
       \returns        'drop state' of the volume

            a volume in drop mode prevents writing of new pages to this volume
     */
    virtual bool IsVolumeToDrop(
        const IOMan_VolumeNo    volNo) = 0;

    /*!
       @brief          The iterator supplying the blocks marked for back up is set to the first block
       @param          TaskId [in] task id
       @return         none

       - This function has to be called prior to the call of the function
      GetNextBlocksForBackUp which returns each time it is executed another
    block marked for backup. The function BeginReadingBlocksMarkedForBackUp
    initializes this iterator and sets it onto the very first block marked for backup
     */
    virtual void BeginReadingBlocksMarkedForBackUp( const tsp00_TaskId TaskId ) = 0;


    enum GetBackupBlocksRc{
        gbbRcOkay               = FBM_Okay,
        gbbRcSystemError        = FBM_SystemError,
        gbbRcNoMoreBackupBlocks = FBM_NoMoreBackupBlocks
    };
    
    /*!
       @brief        returns a number of blocks marked for backup
       @param        task [in] current task
       @param        msgList [out] message list for errors
       @param        maxBlocksWanted [in] maximum number of blocks to return
       @param        cluster [out] resulting complex cluster
       @param        bIsClustered [out] resulting cluster is from the cluster area and may contain empty blocks in between
       @return       GetBackupBlocksRc success state
    */
    virtual GetBackupBlocksRc GetNextBlocksForBackUp(
        RTETask_ITask&                  task,
        Msg_List&                       msgList,
        const IOMan_BlockCount          maxBlocksWanted,
        IOMan_ComplexClusterAddress&    cluster,
        bool&                           bIsClustered ) = 0;

    /*!
       @brief          Removes for all blocks the flags indicating that this block must be saved
       @param          TaskId [in] task id
       @return         none

       - The original state of the blocks is restored.
     */
    virtual void RestoreAllBlockStatesMarkedForBackup( const tsp00_TaskId TaskId ) = 0;

    /*!
       @brief          Returns the number of all blocks marked for back up
       @param          TaskId [in] task id
       @return         number of all blocks marked for back up
     */
    virtual IOMan_BlockCount NumBlocksMarkedForBackup( const tsp00_TaskId TaskId ) const = 0;

    /*!
       @brief          Checks if the given number of blocks is storable within the
                       data volumes.
       @param          numBlocksRequested [in] number of blocks to be written into
                       the data volumes.
       @return         true is given number of blocks is storable within data
     */
    virtual bool IsSpaceAvailable( const IOMan_BlockCount numBlocksRequested ) const = 0;

    /*!
       @brief   Checks whether a task is suspended because of db full
       @return  true is given if task is suspended
     */
    virtual bool IsDBFull() const = 0;

    /*!
        @brief  Returns true if the number of blocks in state free after savepoint is greater than zero.
        @return (bool) 
     */
    virtual bool FreeAfterSVPExist() const = 0;

    /*!
       @brief          Returns the number of all blocks in state freeAfterSavepoint
       @return         (IOMan_BlockCount) Number of pending free blocks
     */

    virtual IOMan_BlockCount GetNumberOfFreeAfterSVPBlocks() const = 0;

    /*!
       @brief          Returns the number of all blocks to backup in state freeAfterSavepoint
                       which will be deleted after the blocks are written.
       @return         (IOMan_BlockCount) Number of pending free blocks still used for backup
     */
    virtual IOMan_BlockCount GetNumberOfBackupBlocksFreeAfterSVP() const = 0;

    /*!
       @brief          Returns the number of blocks in state free. Note that the
                       blocks in state free after savepoint are treated as blocks
                       in state occupied, because the are not available at this 
                       moment.
       @return         (IOMan_BlockCount) Number of free blocks
     */
    virtual IOMan_BlockCount GetNumberOfFreeBlocks() const = 0;

    /*!
       @brief          Returns the number of all used blocks. Used means all blocks
                       not in state free. Note that blocks in state free after savepoint 
                       are handled as occupied blocks because there are not available
                       at this moment.
       @return         (IOMan_BlockCount) Number of used blocks
     */
    virtual IOMan_BlockCount GetNumberOfUsedBlocks() const = 0;


    /*!
       \brief          dump all used blocks to knldiag
       \param          volNo [in] volume number
       \return         none

                       used for diagnostics in drop volume
     */
    virtual void DumpUsedBlocks( const IOMan_VolumeNo volNo) const  = 0;

    /*!
       @brief          If the data volume filling is about 90 percent and the fbm
                       is in online mode, this method returns true to signalize that 
                       garbage collector support is needed.
       @param          numChangedPages [in] number of changed pages within data cache
                       and converter
       @return         true if garbage collector support is needed; else false
     */
    virtual bool GarbageCollectionNeeded( const SAPDB_Int4 numChangedPages ) const = 0;

    /*!
       @brief          If the number of blocks in state free after savepoint is greater
                       than the number of blocks in state free a savepoint is needed.
       @return         true if a savepoint is needed; else false
     */
    virtual bool SavepointNeeded() const = 0;

    /*!
       @brief          Supplies a set of free and neighbouring blocks
       @param          taskId [in] task id
       @param          NumFreeBlocksWanted [in] wanted number of blocks
       @param          bReqSequential [in] request access to sequential volume
       @return         (IOMan_ClusterAddress) address of the supplied cluster

       - Supplies a set of free and neighbouring blocks and sets these blocks
      to the state 'occupied'
     */

    virtual IOMan_ClusterAddress GetMultFreeBlocks(
        const tsp00_TaskId  taskId,
        const IOMan_BlockCount    NumFreeBlocksWanted,
        const bool    bReqSequential) = 0;
    
    virtual bool GetMultFreeBlocks_2(
        RTETask_ITask&            task,
        Msg_List&                 msgList,
        const IOMan_BlockCount&   maxFreeBlocksRequested,
        IOMan_ClusterAddress&     cluster ) = 0;        

    /*!
       @brief  supplies a free cluster
       @param          task [in] current task
       @param          msgList [out] message list for errors
       @param          clusterSize [in] size of the cluster in blocks
       @return         (IOMan_ClusterAddress) address of the supplied cluster
    */
    virtual bool GetFreeCluster(
        RTETask_ITask&            task,
        Msg_List&                 msgList,
        const IOMan_BlockCount&   clusterSize,
        IOMan_ClusterAddress&     cluster) = 0;

    /*!
        \brief   reserves space for a number of clusters on all volumes
        \param   numCluster [in] total number of clusters on volume
        \returns success
    */
    virtual bool ReserveClusterSpace(const SAPDB_UInt4 numCluster) = 0;

    /*!
       @brief          Supplies a free block
       @param          taskId [in] task id
       @param          bReqSequential [in] request access to sequential volume
       @return         (IOMan_BlockAddress) block address of the supplied block

       - Supplies a single free block and marks this block as 'occupied'
      if no free block could be found than an emergency shutdown will
    be executed.
     */
    virtual IOMan_BlockAddress GetFreeBlock(
        const tsp00_TaskId  taskId ,
        const bool    bReqSequential) = 0;

    /*!
       @brief   Removes the flags indicating that a block must be saved
       @param   task    [in] task 
       @param   cluster [in] address of the block to restore
       - The original state of the block is restored.
       - If the state could not be restored successfully since the block to be restored
      was not at all marked for backup the kernel aborts
     */
    virtual void RestoreBlockStateMarkedForBackup(
        RTETask_ITask&                      task,
        const IOMan_ComplexClusterAddress&  cluster ) = 0;

    /*!
       @brief   Sets the state of block to 'free after completion of the next save point'
       @param   block [in] I/O adress of the block to change
       @return  none

       - If the state could not be changed successfully the kernel aborts
    */

    /*!
       @brief          Sets the state of block to 'free'
       @param          TaskId [in] task id
       @param          block [in] I/O adress of the block to change
       @return         none

       - If the state could not be changed successfully the kernel aborts
     */
    virtual void SetBlockStateToFree(
        const tsp00_TaskId       TaskId,
        const IOMan_BlockAddress &block ) = 0;

    /*!
       @brief          Sets the state of cluster to 'free'
       @param          TaskId [in] task id
       @param          ClusterAddress [in] I/O adress and length of the cluster to change
       @return         none

       - If the state could not be changed successfully the kernel aborts
     */
    virtual void SetClusterStateToFree(
        const tsp00_TaskId         TaskId,
        const IOMan_ClusterAddress &ClusterAddress ) = 0;


    /*!
       @brief   Sets the state of block to 'free'
       @param   block [in] I/O adress of the block to change
       @return  none

       - If the state could not be changed successfully the kernel aborts
     */

    virtual inline void SetBlockStateToFree( const IOMan_BlockAddress &block ) = 0;

    /*!
       @brief          Sets the state of block to 'free after completion of the next save point'
       @param          TaskId [in] task id
       @param          block [in] I/O adress of the block to change
       @return         none

       - If the state could not be changed successfully the kernel aborts
     */

    virtual void SetBlockStateToFreeAfterSVP(
        const tsp00_TaskId       TaskId,
        const IOMan_BlockAddress &block ) = 0;


    /*!
       @brief          Sets the state of block to 'free after completion of the next save point'
       @param          block [in] I/O adress of the block to change
       @return         none

       - If the state could not be changed successfully the kernel aborts
     */
    virtual inline void SetBlockStateToFreeAfterSVP( const IOMan_BlockAddress &block ) = 0;

    /*!
       @brief          All Blocks marked as 'free after savepoint' are released
       @param          TaskId [in] task id
       @return         none
     */
    virtual void SetAllBlocksMarkedAsFreeAfterSVPToFree( const tsp00_TaskId TaskId ) = 0;

    /*!
       @brief          Sets the state of block to 'marked for backup'
       @param          TaskId [in] task id
       @param          block [in] I/O adress of the block to change
       @return         (bool) true if operation was executed successfully

       - If the state could not be changed successfully the kernel aborts, if
      the given parameter bAbortIfErrorOccured is set to true.
     */
    virtual bool SetBlockStateToBackup(
        const tsp00_TaskId        TaskId,
        const IOMan_BlockAddress &block ) = 0;

    /*!
       @brief          Sets the state of multiple block to 'marked for backup'
       @param          TaskId [in] task id
       @param          BlockIterator [in] I/O adresses of the blocks to change
       @return         (bool) true if operation was executed successfully

       - If the state could not be changed successfully the kernel aborts, if
      the given parameter bAbortIfErrorOccured is set to true.
     */
    virtual bool SetBlockStateToBackup(
        const tsp00_TaskId        TaskId,
        IOMan_IBlockAddressIterator &BlockIterator ) = 0;

    /*!
       @brief          Returns the number of all used blocks on a device
       @param          VolNo [in] number of the device for which the number of used blocks is requested
       @return         number of all blocks used

       - Returns the number of all used blocks on a device. A block is considered to be used if it is not in the state free.
     */
    virtual IOMan_BlockCount NumBlocksUsed( const IOMan_VolumeNo VolNo ) const = 0;

    /*!
       @brief          Returns the number of blocks marked for backup on a volume
       @param          VolNo [in] number of the volume for which the number of backup blocks is requested
       @return         number of backup blocks 

       - Returns the number of all blocks marked for backup a volume.
     */

    virtual IOMan_BlockCount GetNumBlocksMarkedForBackup ( const IOMan_VolumeNo VolNo ) const = 0;

    /*!
    	\brief     returns the total number of blocks inside the cluster area 
    	\returns   number of blocks in the cluster area
    */
    virtual IOMan_BlockCount GetTotalClusterAreaSize ( const IOMan_VolumeNo VolNo ) const = 0;

    /*!
    	\brief     returns the number of blocks reserved for clusters, this
    			   is the number of occupied clusters * size of one cluster 
    	\returns   number of reserved blocks in the cluster area
    */
    virtual IOMan_BlockCount GetReservedClusterAreaSize ( const IOMan_VolumeNo VolNo ) const = 0;

    /*!
    	\brief     returns the number of blocks used inside the cluster area
    	\returns   number of used blocks in the cluster area
    */
    virtual IOMan_BlockCount GetUsedClusterAreaSize ( const IOMan_VolumeNo VolNo ) const = 0;

    /*!
       @brief          Returns the volume mode of a device
       @param          VolNo [in] number of the volume for which the volume mode is requested
       @return         volume mode

       - Returns the volume mode of a device.
     */
    virtual RTE_VolumeAccessMode GetVolMode( const IOMan_VolumeNo VolNo ) const = 0;

    /*!
       @brief          all memory resources are released and members set to their initial values
       @param          taskId [in] task id
       @return         one
     */
    virtual void Shutdown( const tsp00_TaskId taskId ) = 0;

    /*!
       @brief   restarts the FBM
       @param   TaskId    [in] task id
       @param   MaxNumDev [in] maximum number of devices which can be handled 
                               by the FBM_Manager
       @param   ReservedBlocks [in] collection of reserved block address
       @param   clusterSize    [in] block size of a cluster
       @param   volumeGroups   [in] number of volume groups for archive volumes
       @return  (FBM_ReturnCode)
     */
    virtual FBM_ReturnCode Restart(
        const tsp00_TaskId                  TaskId,
        const IOMan_VolumeNo                MaxNumDev,
        const IOMan_ReservedBlockAddress    &ReservedBlocks,
        const IOMan_BlockCount              clusterSize,
        const SAPDB_Int                     volumeGroups) = 0;


    /*!
       @brief          inserts all important memory structures of the FBM into the 
                       kernel dump file
       @param          taskId [in] identification of the calling task
       @param          dump [in/out] kernel dump file
       @return         none
     */
    virtual void Dump(
        const tsp00_TaskId  taskId,
        Kernel_Dump         &dump ) const  = 0;

};


#endif //FBM_IMANAGER_HPP
