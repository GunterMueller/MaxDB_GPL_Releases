/*!
  @file           FBM_Manager.hpp
  @author         TorstenS
  @author         AlexanderK
  @ingroup        FBM
  @brief          General header file of the FBM

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



#ifndef FBM_MANAGER_HPP
#define FBM_MANAGER_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "ggg00.h"
#include "heo51.h"
#include "heo55k.h"                                // RTE   : vbegexcl,venexcl 
#include "hgg08.h"                                 // region identifier

#include "IOManager/IOMan_ComplexClusterAddress.hpp"

#include "FreeBlockManagement/FBM_IManager.hpp"
#include "FreeBlockManagement/FBM_DataVolumeArray.hpp"

#include "KernelCommon/Kernel_Common.hpp"
#include "KernelCommon/Kernel_SynchronizedDBFullList.hpp"

#include "RunTime/MemoryManagement/RTEMem_AllocatorWrapper.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/// suspend reason for db full
#define FBM_SUSPEND_HANDLE_DB_FULL                 198

/*===========================================================================*
 *  CLASS DECLARATIONS                                                       *
 *===========================================================================*/

class Kernel_Dump;

/*!
  \class          FBM_Manager
  \brief          FreeBlockManagement / manager for all attached volume

 */


class FBM_Manager : public FBM_IManager
{

public:

    static FBM_Manager& CreateInstance();

    static FBM_Manager& Instance(){
        return *m_Instance;
    }

    FBM_ReturnCode Restart(
        const tsp00_TaskId                  TaskId,
        const IOMan_VolumeNo                MaxNumDev,
        const IOMan_ReservedBlockAddress   &ReservedBlocks,
        const IOMan_BlockCount              clusterSize,
        const SAPDB_Int                     volumeGroups);

    bool RegisterVolume(
        const tsp00_TaskId          TaskId,
        const IOMan_VolumeNo        VolNo,
        const IOMan_BlockCount      VolSize,
        const RTE_VolumeAccessMode  VolMode,
        const IOMan_BlockAddress    &restartPageBlock,
        const SAPDB_UInt4           numCluster );

    bool DeRegisterVolume(
        const RTETask_ITask     &task,
        Msg_List                &msgList,
        const IOMan_VolumeNo    volNo);

    bool PrepareVolumeForDrop(
        const RTETask_ITask     &task,
        Msg_List                &msgList,
        const IOMan_VolumeNo    volNo);

    bool RestoreVolumeForDrop(
        const RTETask_ITask     &task,
        Msg_List                &msgList,
        const IOMan_VolumeNo    volNo);

    bool IsVolumeToDrop( const IOMan_VolumeNo    volNo);

    bool SetBlockStateToBackup(
        const tsp00_TaskId        TaskId,
        const IOMan_BlockAddress &block )
    {
        FBM_SynchObject SynchronizeFBM (TaskId);
        return SetBlockStateToBackupUnsynched ( block );
    }

    bool SetBlockStateToBackup(
        const tsp00_TaskId            TaskId,
        IOMan_IBlockAddressIterator   &BlockIterator )
    {
        SAPDBTRACE_ROUTINE_DEBUG ("FBM_Manager::SetBlockStateToBackup", FBM_Trace, 5);

        FBM_SynchObject SynchronizeFBM (TaskId);

        while (BlockIterator.hasMoreElements())
        {
            IOMan_BlockAddress block = BlockIterator.getNextElement();

            if (! SetBlockStateToBackupUnsynched ( block ))
            {
                return false;
            }
        }
        return true;
    }

    bool SetBlockStateToOccupied(
        const tsp00_TaskId          TaskId,
        IOMan_IBlockAddressIterator &BlockIterator)
    {
        SAPDBTRACE_ROUTINE_DEBUG ("FBM_Manager::SetBlockStateToOccupied", FBM_Trace, 5);

        FBM_SynchObject SynchronizeFBM (TaskId);

        while (BlockIterator.hasMoreElements())
        {
            IOMan_BlockAddress block = BlockIterator.getNextElement();
            if ( ! SetBlockStateToOccupiedUnsynched ( block )){
                return false;
            }
        }
        return true;
    }

    bool SetBlockStateToOccupied(
        const tsp00_TaskId        TaskId,
        const IOMan_BlockAddress &block)
    {
        FBM_SynchObject SynchronizeFBM (TaskId);

        return SetBlockStateToOccupiedUnsynched ( block );
    }

    void SetClusterStateToFree(
        const tsp00_TaskId         TaskId,
        const IOMan_ClusterAddress &ClusterAddress );

    void SetBlockStateToFree(
        const tsp00_TaskId       TaskId,
        const IOMan_BlockAddress &block );

    inline void SetBlockStateToFree( const IOMan_BlockAddress &block ){
        SetBlockStateToFree( GetTaskId(), block );
    }

    void SetBlockStateToFreeAfterSVP(
        const tsp00_TaskId       TaskId,
        const IOMan_BlockAddress &block );

    inline void SetBlockStateToFreeAfterSVP( const IOMan_BlockAddress &block ){
        SetBlockStateToFreeAfterSVP( GetTaskId(), block );
    }

    void Shutdown( const tsp00_TaskId taskId );

    IOMan_ClusterAddress GetMultFreeBlocks(
        const tsp00_TaskId        taskId,
        const IOMan_BlockCount    NumFreeBlocksWanted,
        const bool                bReqSequential );

    bool GetMultFreeBlocks_2(
           RTETask_ITask&            task,
           Msg_List&                 msgList,
           const IOMan_BlockCount&   maxFreeBlocksRequested,
           IOMan_ClusterAddress&     cluster );
        
    bool GetFreeCluster(
        RTETask_ITask&            task,
        Msg_List&                 msgList,
        const IOMan_BlockCount&   clusterSize,
        IOMan_ClusterAddress&     cluster);

    IOMan_BlockAddress GetFreeBlock(
        const tsp00_TaskId  taskId ,
        const bool          bReqSequential );

    void SetAllBlocksMarkedAsFreeAfterSVPToFree( const tsp00_TaskId TaskId );

    void RestoreAllBlockStatesMarkedForBackup( const tsp00_TaskId TaskId );

    void RestoreBlockStateMarkedForBackup(
        RTETask_ITask&                      task,
        const IOMan_ComplexClusterAddress&  cluster );

    IOMan_BlockCount NumBlocksMarkedForBackup( const tsp00_TaskId taskId ) const;

    void BeginReadingBlocksMarkedForBackUp( const tsp00_TaskId TaskId );

    FBM_IManager::GetBackupBlocksRc GetNextBlocksForBackUp(
        RTETask_ITask&                  task,
        Msg_List&                       msgList,
        const IOMan_BlockCount          maxBlocksWanted,
        IOMan_ComplexClusterAddress&    cluster,
        bool&                           bIsClustered );

    void Dump(
        const tsp00_TaskId  taskId,
        Kernel_Dump         &dump ) const;

    IOMan_BlockCount NumBlocksUsed( const IOMan_VolumeNo VolNo ) const
    {
        SAPDBERR_ASSERT_ARGUMENT( m_DataVolumes.IsAvailable( VolNo ));

        return (m_DataVolumes [VolNo].GetNumBlocksUsed());
    }

    IOMan_BlockCount GetNumBlocksMarkedForBackup (const IOMan_VolumeNo volNo ) const
    {
        SAPDBERR_ASSERT_ARGUMENT( m_DataVolumes.IsVolumeNoValid( volNo ));

        if( ! m_DataVolumes.IsRegistered( volNo )) {
            return 0;  // volume is not ready
        }
        else{
            return (m_DataVolumes [volNo].GetNumBlocksMarkedForBackup());
        }
    }

    IOMan_BlockCount GetTotalClusterAreaSize (const IOMan_VolumeNo volNo ) const
    {
        SAPDBERR_ASSERT_ARGUMENT( m_DataVolumes.IsVolumeNoValid( volNo ));

        if( ! m_DataVolumes.IsRegistered( volNo )) {
            return 0;  // volume is not ready
        }
        else{
            return (m_DataVolumes [volNo].GetTotalClusterAreaSize());
        }
    }

    IOMan_BlockCount GetReservedClusterAreaSize (const IOMan_VolumeNo volNo ) const
    {
        SAPDBERR_ASSERT_ARGUMENT( m_DataVolumes.IsVolumeNoValid( volNo ));

        if( ! m_DataVolumes.IsRegistered( volNo )) {
            return 0;  // volume is not ready
        }
        else{
            return (m_DataVolumes [volNo].GetReservedClusterAreaSize());
        }
    }

    IOMan_BlockCount GetUsedClusterAreaSize (const IOMan_VolumeNo volNo ) const
    {
        SAPDBERR_ASSERT_ARGUMENT( m_DataVolumes.IsVolumeNoValid( volNo ));

        if( ! m_DataVolumes.IsRegistered( volNo )) {
            return 0;  // volume is not ready
        }
        else{
            return (m_DataVolumes [volNo].GetUsedClusterAreaSize());
        }
    }

    IOMan_BlockCount GetNumberOfClusteredBlocksToBackup(const tsp00_TaskId    taskId) const;

    void DumpUsedBlocks(const IOMan_VolumeNo volNo) const {
        m_DataVolumes [volNo].DumpUsedBlocks();
    }

    RTE_VolumeAccessMode GetVolMode( const IOMan_VolumeNo volNo ) const
    {
        SAPDBERR_ASSERT_ARGUMENT( m_DataVolumes.IsAvailable( volNo ));

        return (m_DataVolumes [volNo].VolMode());
    }

    IOMan_BlockCount GetNumberOfFreeAfterSVPBlocks() const{
        return m_TotalNumBlocksFreeAfterSVP;
    }

    IOMan_BlockCount GetNumberOfBackupBlocksFreeAfterSVP() const{
        return m_TotalNumBlocksBackupFreeAfterSVP;
    }

    IOMan_BlockCount GetNumberOfFreeBlocks() const{
        return( m_TotalNumBlocksFree );
    }

    IOMan_BlockCount GetNumberOfUsedBlocks() const{
        return( m_TotalNumBlocks - GetNumberOfFreeBlocks() );
    }

    bool IsSpaceAvailable( const IOMan_BlockCount numBlocksRequested ) const{
        return(( numBlocksRequested + GetNumberOfUsedBlocks()) < m_TotalNumBlocks );
    }

    bool GarbageCollectionNeeded( const SAPDB_Int4 numChangedPages ) const
    {
        if( ! m_Active )
            return( false );

        if( m_SavepointIsRequested )
            return( false );

        // data base filling is under 90 percent inclusive the changed pages of the data cache
        if(( GetNumberOfUsedBlocks() + numChangedPages ) < ( m_TotalNumBlocks * 0.9 ))
            return( false );

        return( true );
    }

    bool SavepointNeeded() const
    {
        if( ! m_Active )
            return false;

        if( m_SavepointIsRequested )
            return false;

        // more blocks ready to be free than realy free blocks
        if( m_TotalNumBlocksFree < m_TotalNumBlocksFreeAfterSVP )
            return true;

        return false;
    }

    bool FreeAfterSVPExist() const {
        return 0 < m_TotalNumBlocksFreeAfterSVP;
    }

    bool IsDBFull() const{
        return ! m_DBFullWaitQueue.IsEmpty();
    }


private:

    FBM_Manager(SAPDBMem_IRawAllocator &Allocator);

    IOMan_VolumeNo GetFreeVolumeInGroup(
        const SAPDB_UInt        CurrentVolumeGroup,
        const IOMan_BlockCount  NumRequestedBlocks);

    IOMan_VolumeNo GetDeviceWithFreeBlocks (
        const IOMan_BlockCount  NumRequestedBlocks,
        const bool              bReqSequential);

    IOMan_VolumeNo GetVolumeWithFreeCluster();
    IOMan_VolumeNo SearchVolumeWithFreeCluster();

    bool ReserveAddClusterSpace(const SAPDB_UInt4 numCluster)
    {
        return ReserveClusterSpace(m_reservedClusters + numCluster);
    }

    bool ReserveClusterSpace(const SAPDB_UInt4 numCluster);

    IOMan_BlockCount OptimumNumUsedBlocksPerDevice ();

    void ChangeDeviceUsagePreferences();

    void ResetMembersToNullAndZero();

    bool SetBlockStateToOccupiedUnsynched( const IOMan_BlockAddress &block );

    bool SetBlockStateToBackupUnsynched( const IOMan_BlockAddress &block );

private:

    tsp00_TaskId GetTaskId()
    {
        tsp00_TaskId taskId;
        vgetpid( taskId );
        return( taskId );
    }


    class FBM_SynchObject
    {
    private:
        tsp00_TaskId m_TaskId;
    public:
        FBM_SynchObject(tsp00_TaskId TaskId) :m_TaskId(TaskId)
        {
            vbegexcl (m_TaskId, g08fbm);
        };

        ~FBM_SynchObject()
        {
            vendexcl (m_TaskId, g08fbm);
        };
    };

private:

    RTEMem_AllocatorWrapper         m_Allocator;
    IOMan_BlockCount                m_TotalNumBlocksFreeAfterSVP;
    IOMan_BlockCount                m_TotalNumBlocksBackupFreeAfterSVP;
    IOMan_BlockCount                m_TotalNumBlocksFree;
    IOMan_BlockCount                m_TotalNumBlocks;
    IOMan_BlockCount                m_ClusterSize;
    IOMan_VolumeNo                  m_NumDev;
    IOMan_VolumeNo                  m_NextDevToUseForNewBlock;
    IOMan_VolumeNo                  m_NextDevToUseForFragBlock;
    IOMan_VolumeNo                  m_NextDevToUseForBackup; // for save data or save pages
    IOMan_VolumeNo                  m_NextVolToUseForNewCluster;
    IOMan_VolumeNo                  m_LastArchiveVolumeUsed;
    IOMan_VolumeNo                  m_ParallelWritingArchiveVolumes;

    /// number of Clusters reserved
    IOMan_ClusterCount              m_reservedClusters;

    FBM_DataVolumeArray             m_DataVolumes;
    static FBM_Manager             *m_Instance;
    bool                            m_SavepointIsRequested;
    bool                            m_Active;
    bool                            m_BackupClustered;
    IOMan_ReservedBlockAddress      m_ReservedBlocks;
    Container_Vector<IOMan_VolumeNo>  m_ArchiveVolumes;
    Container_Vector<SAPDB_UInt>    m_ArchiveVolumeGroups;
    SAPDB_UInt                      m_CurArchiveVolumeGroup;
    SAPDB_UInt                      m_NumberVolumeGroups;

    Kernel_SynchronizedDBFullList   m_DBFullWaitQueue;
};


#endif //FBM_MANAGER_HPP
