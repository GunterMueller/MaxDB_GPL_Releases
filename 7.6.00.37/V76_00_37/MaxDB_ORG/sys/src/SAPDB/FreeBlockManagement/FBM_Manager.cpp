/*!**************************************************************************

  module      : FBM_Manager.cpp

  -------------------------------------------------------------------------

  responsible : TorstenS

  auhtor      : AlexanderK

  special area: FreeBlockManagement (DDBM)
  description : 


  last changed: 2000-03-10  11:00
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG



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


*****************************************************************************/

/*===========================================================================*
*  INCLUDES                                                                 *
*===========================================================================*/

#include <new.h>                                  // SYSTEM: required for placement new
#include "hgg08.h"                                // region identifier
#include "hgg01.h"                                // g01is_archive
#include "hkb57.h"                                // kb57[GS]etClusterReservedSpace
#include "Container/Container_Vector.hpp"
#include "FreeBlockManagement/FBM_Manager.hpp"
#include "FreeBlockManagement/FBM_Dump.hpp"
#include "FreeBlockManagement/FBM_Types.hpp"
#include "FreeBlockManagement/FBM_Exception.hpp"
#include "FreeBlockManagement/FBM_SimpleDataVolume.hpp"
#include "FreeBlockManagement/FBM_SequentialDataVolume.hpp"
#include "FreeBlockManagement/FBM_DataVolumeIterator.hpp"
#include "KernelCommon/Kernel_Dump.hpp"
#include "RunTime/RTE_Crash.hpp"
#include "RunTime/RTE_Message.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"

/*===========================================================================*
 *  DEFINES                                                                   *
 *===========================================================================*/

#define FBM_MEM_INITIAL_SIZE            (8192 * 8)
#define FBM_MEM_SUPPLEMENT_SIZE         (8192 * 8)

/*===========================================================================*
 *  PUBLIC METHODS                                                           *
 *===========================================================================*/

FBM_Manager* FBM_Manager::m_Instance = NULL;

/*---------------------------------------------------------------------------*/

FBM_Manager&
FBM_Manager::CreateInstance()
{
    SAPDBTRACE_ROUTINE_DEBUG ("FBM_Manager::CreateInstance", FBM_Trace, 5);

    if (m_Instance == NULL)
    {
        m_Instance = new (RTEMem_Allocator::Instance())
                     FBM_Manager(RTEMem_Allocator::Instance());

        if( NULL == m_Instance )
        {
            FBM_Exception errMsg( __CONTEXT__, FBM_ERR_NO_MORE_MEMORY_TO_BUILD_FBM );
            RTE_Crash( errMsg );
        }
    }
    return *m_Instance;
}

/*---------------------------------------------------------------------------*/

FBM_Manager::FBM_Manager(SAPDBMem_IRawAllocator &Allocator)
        :
        m_Allocator( UTF8( "FBM_Manager" ), Allocator ),
        m_DataVolumes( m_Allocator ),
        m_ArchiveVolumes( m_Allocator ),
        m_ArchiveVolumeGroups( m_Allocator ),
        m_DBFullWaitQueue( FBM_SUSPEND_HANDLE_DB_FULL )
{
    ResetMembersToNullAndZero();
}

/*---------------------------------------------------------------------------*/

FBM_IManager&
FBM_IManager::Instance ()
{
    return FBM_Manager::Instance();
}

/*---------------------------------------------------------------------------*/

FBM_ReturnCode
FBM_Manager::Restart(
    const tsp00_TaskId                TaskId,
    const IOMan_VolumeNo              MaxNumDev,
    const IOMan_ReservedBlockAddress  &ReservedBlocks,
    const IOMan_BlockCount            clusterSize,
    const SAPDB_Int                   volumeGroups)
{
    SAPDBTRACE_ROUTINE_DEBUG ("FBM_Manager::Restart", FBM_Trace, 5);

    FBM_SynchObject SynchronizeFBM (TaskId);

    if( m_Active )
    {
        if(( m_DataVolumes.GetMaxVolumeNo() != MaxNumDev ))
        {
            FBM_Exception errMsg( __CONTEXT__, FBM_ERR_VOLUME_COUNT_INCOMPATIBLE );
            RTE_Message( errMsg );

            return( FBM_IncompatibleVolumeCount );
        }
        return( FBM_AlreadyStarted );
    }

    m_ReservedBlocks = ReservedBlocks;
    m_ClusterSize    = clusterSize;

    if (g01is_archive())
    {
        m_ArchiveVolumes.Reserve(MaxNumDev);

        m_NumberVolumeGroups = volumeGroups;
        m_ArchiveVolumeGroups.Initialize(m_NumberVolumeGroups, 0);
    }

    /* construct array containing the number of free pages for each section */
    SAPDBTRACE_WRITELN( FBM_Trace, 5, "MaxNumDev: " << MaxNumDev);

    m_DataVolumes.Initialize( MaxNumDev );
    m_Active = true;

    return( FBM_Okay );
}

/*---------------------------------------------------------------------------*/

void
FBM_Manager::Shutdown( const tsp00_TaskId taskId )
{
    SAPDBTRACE_ROUTINE_DEBUG ("FBM_Manager::Shutdown", FBM_Trace, 5);

    /* this method releases all memory resources */

    {
        FBM_SynchObject SynchronizeFBM (taskId);

        /* release memory allocated by the FBM_Manager and its member */
        m_DataVolumes.Shutdown();

        /* reset all member variables, usage of the FBM_Manager leads to errors now */
        ResetMembersToNullAndZero();
    }
    m_DBFullWaitQueue.ResumeAllWaitingTasks( taskId );
}

/*---------------------------------------------------------------------------*/

bool
FBM_Manager::RegisterVolume(
    const tsp00_TaskId         TaskId,
    const IOMan_VolumeNo       VolNo,
    const IOMan_BlockCount     VolSize,
    const RTE_VolumeAccessMode VolMode,
    const IOMan_BlockAddress   &restartPageBlock,
    const SAPDB_UInt4          numCluster )
{
    SAPDBTRACE_ROUTINE_DEBUG ("FBM_Manager::RegisterDevice", FBM_Trace, 5);

    SAPDBTRACE_WRITELN( FBM_Trace, 5, "VolNo: " << VolNo << " VolSize: "  << VolSize);

    /* check validity of the device number */
    if( ! m_DataVolumes.IsVolumeNoValid( VolNo ))
    {
        FBM_VolumeNoOutOfRange errMsg(__CONTEXT__, VolNo, m_DataVolumes.GetMinVolumeNo(),
                                      m_DataVolumes.GetMaxVolumeNo());
        RTE_Message (errMsg);
        return false;
    }
    {
        FBM_SynchObject SynchronizeFBM (TaskId);

        if (RTE_VolumeAccessModeSequential == VolMode)
        {
            m_DataVolumes.Register(VolNo, new (m_Allocator) FBM_SequentialDataVolume(m_Allocator, VolSize, VolMode, VolNo));
        }
        else
        {
            m_DataVolumes.Register(VolNo, new (m_Allocator) FBM_SimpleDataVolume(m_Allocator, VolSize, VolMode, VolNo, m_ClusterSize));
        }

        /* initialize device description and fill it with start values */
        if (!m_DataVolumes [VolNo].InitVolume(numCluster))
        {
            m_DataVolumes.UnRegister(VolNo);
            return false;
        }



        // update archiveVolumes
        if (RTE_VolumeAccessModeSequential == VolMode)
        {
            SAPDBERR_ASSERT_STATE (g01is_archive());

            m_ArchiveVolumes.InsertEnd(VolNo);
            // recalc VolumeGroup startOffsets
            for (SAPDB_UInt loop=0; loop < m_NumberVolumeGroups; loop++)
            {
                m_ArchiveVolumeGroups[loop] = (m_ArchiveVolumes.GetSize() / m_NumberVolumeGroups) * loop;
            }
        }

        /* update global block counters */
        m_NumDev++;
        m_TotalNumBlocks     += VolSize;
        m_TotalNumBlocksFree += VolSize;

        /* reserve blocks on the device for example for the restart record */
        IOMan_ReservedBlockAddressIterator Iter( m_ReservedBlocks, VolNo );

        for( Iter.Begin(); !Iter.End(); ++Iter )
        {
            m_TotalNumBlocksFree --;
            m_DataVolumes[VolNo].SetBlockStateToOccupied( Iter.GetBlockAddress().GetBlockNo());
        }
        if( VolNo == restartPageBlock.GetVolumeNo())
        {
            m_DataVolumes[VolNo].SetBlockStateToOccupied( restartPageBlock.GetBlockNo());
        }

        // After marking the reserved blocks the volume is usable
        m_NextDevToUseForNewBlock = VolNo;
        m_NextVolToUseForNewCluster = VolNo;
    }
    m_DBFullWaitQueue.ResumeAllWaitingTasks( TaskId );

    return true;
}

/*---------------------------------------------------------------------------*/

bool
FBM_Manager::DeRegisterVolume(
    const RTETask_ITask     &task,
    Msg_List                &msgList,
    const IOMan_VolumeNo    volNo)
{
    SAPDBTRACE_ROUTINE_DEBUG ("FBM_Manager::DeRegisterVolume", FBM_Trace, 5);

    SAPDBTRACE_WRITELN( FBM_Trace, 5, "VolNo: " << volNo);

    FBM_SynchObject SynchronizeFBM (task.ID());

    if( ! m_DataVolumes.IsVolumeNoValid( volNo )) // Check given given volNo!
    {
        msgList.AppendNewMessage( FBM_VolumeNoOutOfRange (__CONTEXT__, volNo,
                                  m_DataVolumes.GetMinVolumeNo(),
                                  m_DataVolumes.GetMaxVolumeNo()));
        return false;
    }
    if( ! m_DataVolumes.IsRegistered( volNo )) // Check given given volNo!
    {
        msgList.AppendNewMessage( FBM_VolumeNotRegistered(__CONTEXT__, volNo ));
        return false;
    }
    FBM_IVolume * volPtr = &m_DataVolumes[volNo];
    destroy(volPtr, m_Allocator);
    m_DataVolumes.UnRegister( volNo );
    return true;
}

/*---------------------------------------------------------------------------*/

bool
FBM_Manager::PrepareVolumeForDrop(
    const RTETask_ITask     &task,
    Msg_List                &msgList,
    const IOMan_VolumeNo    volNo)
{
    FBM_SynchObject SynchronizeFBM (task.ID());

    if( ! m_DataVolumes.IsVolumeNoValid( volNo )) // Check given given volNo!
    {
        msgList.AppendNewMessage( FBM_VolumeNoOutOfRange(__CONTEXT__, volNo,
                                  m_DataVolumes.GetMinVolumeNo(),
                                  m_DataVolumes.GetMaxVolumeNo()));
        return false;
    }
    if( ! m_DataVolumes.IsRegistered( volNo )) // Check given given volNo!
    {
        msgList.AppendNewMessage( FBM_VolumeNotRegistered(__CONTEXT__, volNo ));
        return false;
    }
    FBM_IVolume& IVolume = m_DataVolumes [volNo];

    if (RTE_VolumeAccessModeNormal != IVolume.VolMode())
    {
        msgList.AppendNewMessage( FBM_Exception( __CONTEXT__,
                                  FBM_ERR_NO_CLASSIC_VOLUME, SAPDB_ToString( volNo, _T_d )));
        return false;
    }

    FBM_IDataVolume &Volume = (FBM_IDataVolume&) IVolume;

    Volume.PrepareForDrop();
    IOMan_BlockCount numBlocksUsed = Volume.GetNumBlocksUsed();
    IOMan_BlockCount numBlocks     = Volume.GetNumBlocks();

    /* update global block counters */
    m_NumDev             --;
    m_TotalNumBlocks     -= numBlocks;
    m_TotalNumBlocksFree -= numBlocks - numBlocksUsed;

    return true;
}

/*---------------------------------------------------------------------------*/

bool
FBM_Manager::RestoreVolumeForDrop(
    const RTETask_ITask     &task,
    Msg_List                &msgList,
    const IOMan_VolumeNo    volNo)
{
    FBM_SynchObject SynchronizeFBM (task.ID());

    if( ! m_DataVolumes.IsVolumeNoValid( volNo )) // Check given given volNo!
    {
        msgList.AppendNewMessage( FBM_VolumeNoOutOfRange(__CONTEXT__, volNo,
                                  m_DataVolumes.GetMinVolumeNo(),
                                  m_DataVolumes.GetMaxVolumeNo()));
        return false;
    }
    if( ! m_DataVolumes.IsRegistered( volNo )) // Check given given volNo!
    {
        msgList.AppendNewMessage( FBM_VolumeNotRegistered(__CONTEXT__, volNo ));
        return false;
    }
    FBM_IVolume & IVolume = m_DataVolumes [volNo];

    if (RTE_VolumeAccessModeNormal != IVolume.VolMode())
    {
        msgList.AppendNewMessage( FBM_Exception( __CONTEXT__,
                                  FBM_ERR_NO_CLASSIC_VOLUME, SAPDB_ToString( volNo, _T_d )));
        return false;
    }

    FBM_IDataVolume &Volume = (FBM_IDataVolume&) IVolume;

    Volume.RestoreForDrop();

    IOMan_BlockCount numBlocksUsed = Volume.GetNumBlocksUsed();
    IOMan_BlockCount numBlocks     = Volume.GetNumBlocks();

    /* update global block counters */
    m_NumDev             ++;
    m_TotalNumBlocks     += numBlocks;
    m_TotalNumBlocksFree += numBlocks - numBlocksUsed;

    if((0 < GetNumberOfFreeBlocks()) && ( ! m_DBFullWaitQueue.IsEmpty())){
        m_DBFullWaitQueue.ResumeAllWaitingTasks( task.ID() );
    }

    return true;
}

/*---------------------------------------------------------------------------*/

bool
FBM_Manager::IsVolumeToDrop(
    const IOMan_VolumeNo    volNo)
{

    /* check device number and get a reference to the current device */
    if ( ! m_DataVolumes.IsRegistered( volNo))
    {
        return false;
    }

    FBM_IVolume & IVolume = m_DataVolumes [volNo];

    if (RTE_VolumeAccessModeNormal != IVolume.VolMode())
    {
        return false;
    }

    FBM_IDataVolume &Volume = (FBM_IDataVolume&) IVolume;

    return Volume.IsToDrop();
}

/*---------------------------------------------------------------------------*/

void
FBM_Manager::SetClusterStateToFree(
    const tsp00_TaskId         TaskId,
    const IOMan_ClusterAddress &ClusterAddress )
{
    SAPDBTRACE_ROUTINE_DEBUG ("FBM_Manager::SetClusterStateToFree", FBM_Trace, 5);

    IOMan_BlockCount pageCnt = ClusterAddress.GetBlockCount();

    for (IOMan_ClusterAddress::BlockIndex loop=0; loop < pageCnt; loop++){
        SetBlockStateToFree(TaskId,ClusterAddress.GetBlockAddress(loop));
    }
}

/*---------------------------------------------------------------------------*/

void
FBM_Manager::SetBlockStateToFree(
    const tsp00_TaskId       TaskId,
    const IOMan_BlockAddress &block )
{
    SAPDBTRACE_ROUTINE_DEBUG ("FBM_Manager::SetBlockStateToFree", FBM_Trace, 5);

    const IOMan_VolumeNo  volNo   = block.GetVolumeNo();
    const IOMan_BlockNo   blockNo = block.GetBlockNo();

    SAPDBTRACE_WRITELN( FBM_Trace, 5, "SetFree: volNo: " << volNo << " blockNo: " << blockNo);

    /* check that the block containing the restart record is never set to free */
    SAPDBERR_ASSERT_STATE ( ! m_ReservedBlocks.IsMember( block ));

    {
        FBM_SynchObject SynchronizeFBM (TaskId);

        /* check device number and get a reference to the current device */
        FBM_IVolume & IVolume = m_DataVolumes [volNo];

        if (RTE_VolumeAccessModeNormal != IVolume.VolMode())
            return;

        FBM_IDataVolume &Volume = (FBM_IDataVolume&) IVolume;

        /* check that the current state is  occupied */
        switch (Volume.GetBlockState (blockNo))
        {
        case BlockState_Occupied:
            /* set block state to free and update all auxilliary structures */
            if (!Volume.IsToDrop())
            {
                m_TotalNumBlocksFree ++;
            }
            Volume.SetBlockStateToFree (blockNo);
            break;
            // fatal error use default action
        default:
            {
                /* if none of the cases above was fulfilled an error occured */
                /* write error message into knldiag and vtrace               */
                FBM_IllegalTransition ErrorMessage (__CONTEXT__, volNo, blockNo,
                                                    Volume.GetBlockState (blockNo),
                                                    Volume.GetBlockStateAfterBackup (blockNo),
                                                    BlockState_Free,
                                                    BlockState_Free);
                RTE_Crash(ErrorMessage);

            }
        }
    }
    if((0 < GetNumberOfFreeBlocks()) && ( ! m_DBFullWaitQueue.IsEmpty())){
        m_DBFullWaitQueue.ResumeAllWaitingTasks( TaskId );
    }
}

/*---------------------------------------------------------------------------*/

bool
FBM_Manager::SetBlockStateToOccupiedUnsynched( const IOMan_BlockAddress &block )
{
    SAPDBTRACE_ROUTINE_DEBUG ("FBM_Manager::SetBlockStateToOccupied", FBM_Trace, 5);

    const IOMan_VolumeNo  volNo   = block.GetVolumeNo();
    const IOMan_BlockNo   blockNo = block.GetBlockNo();

    SAPDBTRACE_WRITELN( FBM_Trace, 5, "SetOcc: VolNo: " << volNo << " blockNo: " << blockNo );

    if( ! m_DataVolumes.IsVolumeNoValid( volNo )) // Check given given volNo!
    {
        FBM_VolumeNoOutOfRange errMsg(__CONTEXT__, volNo, m_DataVolumes.GetMinVolumeNo(),
                                      m_DataVolumes.GetMaxVolumeNo());
        RTE_Message( errMsg );
        return false;
    }
    if( ! m_DataVolumes.IsRegistered( volNo )) // Check given given volNo!
    {
        FBM_VolumeNotRegistered errMsg(__CONTEXT__, volNo );
        RTE_Message( errMsg );
        return false;
    }

    FBM_IVolume & IVolume = m_DataVolumes [volNo];

    if( ! IVolume.IsBlockNoValid( blockNo ))  // Check given given blockNo!
    {
        FBM_VolumeTooSmall errMsg(__CONTEXT__, volNo, blockNo, IVolume.GetNumBlocks());
        RTE_Message( errMsg );
        return false;
    }
    /* check that the current state is free */
    if (IVolume.GetBlockState (blockNo) == BlockState_Free)
    {
        if(RTE_VolumeAccessModeNormal == IVolume.VolMode()) {
            SAPDBERR_ASSERT_STATE ((((FBM_IDataVolume&)IVolume).GetBlockStateAfterBackup (blockNo) == BlockState_Free))
        }
        m_TotalNumBlocksFree --;

        /* set block state to occupied and update all auxilliary structures */
        IVolume.SetBlockStateToOccupied (blockNo);
        return( true );
    }

    if(RTE_VolumeAccessModeSequential == IVolume.VolMode()) {
        return( true );
    }

    /* if none of the cases above was fulfiled an error occured */
    /* write error message into knldiag and vtrace              */

    FBM_BlockState backupState;
    if(RTE_VolumeAccessModeNormal == IVolume.VolMode()){
        backupState = ((FBM_IDataVolume&)IVolume).GetBlockStateAfterBackup (blockNo);
    }
    else{
        backupState = BlockState_Free;
    }

    FBM_IllegalTransition errMsg (__CONTEXT__, volNo, blockNo,
                                  IVolume.GetBlockState (blockNo), backupState,
                                  BlockState_Occupied, BlockState_Free);
    RTE_Message( errMsg );
    return( false );
}

/*---------------------------------------------------------------------------*/


bool
FBM_Manager::SetBlockStateToBackupUnsynched( const IOMan_BlockAddress &block )
{
    SAPDBTRACE_ROUTINE_DEBUG ("FBM_Manager::SetBlockStateToBackup", FBM_Trace, 5);

    const IOMan_VolumeNo  volNo   = block.GetVolumeNo();
    const IOMan_BlockNo   blockNo = block.GetBlockNo();

    SAPDBTRACE_WRITELN( FBM_Trace, 5, "SetBackU volNo " << volNo << " blockNo: " << blockNo );

    /* check that the block containing the restart record is never set to backup */
    SAPDBERR_ASSERT_STATE ( ! m_ReservedBlocks.IsMember( block ));

    /* check device number and get a reference to the current device */
    FBM_IVolume & IVolume = m_DataVolumes [volNo];

    if (RTE_VolumeAccessModeNormal != IVolume.VolMode())
        return( true );

    FBM_IDataVolume &Volume = (FBM_IDataVolume&) IVolume;

    /* check that no more blocks are marked after starting the backup read process */
    SAPDBERR_ASSERT_STATE (!Volume.IsBackupRunning());

    /* check that the current state is occupied */
    if (BlockState_Occupied == Volume.GetBlockState (blockNo))
    {
        Volume.SetBlockStateToBackUp(blockNo);
        return( true );
    }

    /* if none of the cases above was fulfilled an error occured */
    /* write error message into knldiag and vtrace */
    FBM_IllegalTransition errMsg (__CONTEXT__, volNo, blockNo,
                                  Volume.GetBlockState (blockNo),
                                  Volume.GetBlockStateAfterBackup (blockNo),
                                  BlockState_BackUp,
                                  BlockState_Occupied);
    RTE_Message( errMsg );
    return( false );
}

/*---------------------------------------------------------------------------*/

void
FBM_Manager::SetBlockStateToFreeAfterSVP(
    const tsp00_TaskId       TaskId,
    const IOMan_BlockAddress &block )
{
    SAPDBTRACE_ROUTINE_DEBUG ("FBM_Manager::SetBlockStateToFreeAfterSVP", FBM_Trace, 5);

    const IOMan_VolumeNo  volNo   = block.GetVolumeNo();
    const IOMan_BlockNo   blockNo = block.GetBlockNo();

    SAPDBTRACE_WRITELN( FBM_Trace, 5, "SetFSVP: volNo: " << volNo << " blockNo: " << blockNo );

    FBM_SynchObject SynchronizeFBM (TaskId);

    /* check that the block containing the restart record is never set to fsvp */
    SAPDBERR_ASSERT_STATE ( ! m_ReservedBlocks.IsMember( block ));

    /* check volume number and get a reference to the current volume */
    FBM_IVolume & IVolume = m_DataVolumes [volNo];

    if (RTE_VolumeAccessModeNormal != IVolume.VolMode())
        return;

    FBM_IDataVolume &Volume = (FBM_IDataVolume&) IVolume;

    /* check that the current state is  occupied */
    switch (Volume.GetBlockState (blockNo))
    {
    case BlockState_Occupied:
        Volume.SetBlockStateToFreeAfterSVP (blockNo);
        ++m_TotalNumBlocksFreeAfterSVP;
        return;
    case BlockState_BackUp:
        if (BlockState_Occupied == Volume.GetBlockStateAfterBackup (blockNo))
        {
            Volume.SetBlockStateAfterBackup (blockNo, BlockState_FreeAfterSVP);
            ++m_TotalNumBlocksBackupFreeAfterSVP;
        }
        return;
    default:
        /* if none of the cases above was fulfilled an error occured */
        /* write error message into knldiag and vtrace */
        FBM_IllegalTransitionToFreeAfterSVP ErrorMessage (__CONTEXT__, volNo, blockNo,
                Volume.GetBlockState (blockNo),
                Volume.GetBlockStateAfterBackup (blockNo));
        RTE_Crash(ErrorMessage);
        break;

    }
}

/*---------------------------------------------------------------------------*/

IOMan_ClusterAddress
FBM_Manager::GetMultFreeBlocks(
    const tsp00_TaskId        taskId,
    const IOMan_BlockCount    NumFreeBlocksWanted,
    const bool                bReqSequential )
{
    SAPDBTRACE_ROUTINE_DEBUG ("FBM_Manager::GetMultFreeBlocks", FBM_Trace, 5);

    IOMan_BlockAddress      block;
    IOMan_ClusterAddress    cluster;
    IOMan_BlockCount        NumFreeBlocksSupplied = 0;
    
    if (0 == NumFreeBlocksWanted)
    {
        return IOMan_ClusterAddress();
    }

    /* This following block is needed to leave the FBM region before entering the     */
    /* log region in the block below. This is done to avoid a dead lock. The FBM      */
    /* region is left when the FBM_SynchObject is destructed at the end of this block */

    while( m_Active )
    {
        {
            FBM_SynchObject SynchronizeFBM( taskId );

            /* get number of a device containing free blocks */
            const IOMan_VolumeNo volNo = GetDeviceWithFreeBlocks (NumFreeBlocksWanted, bReqSequential);

            if (volNo.IsInvalid())
            {
                FBM_Exception NoMoreSpace( __CONTEXT__, FBM_NO_MORE_FREE_BLOCKS );
                RTE_Message( NoMoreSpace );
            }
            else
            {
                block.SetAddress( volNo, m_DataVolumes [volNo].GetMultFreeBlocks( NumFreeBlocksWanted,
                                  NumFreeBlocksSupplied ));
                cluster.SetCluster( block, NumFreeBlocksSupplied );

                SAPDBTRACE_WRITELN( FBM_Trace, 5, "Free volNo  " << cluster.GetVolumeNo()
                                    << " Free blockNo: " << cluster.GetBlockNo( 0 ));

                SAPDBTRACE_WRITELN( FBM_Trace, 5, "Wanted Num: " << NumFreeBlocksWanted
                                    << " Suppl.Num: " << NumFreeBlocksSupplied );

                m_TotalNumBlocksFree -= NumFreeBlocksSupplied;

                break; // usable cluster found => okay
            }
        }
        m_DBFullWaitQueue.InsertTaskAndWait( taskId );
    }
    return cluster;
}

bool
FBM_Manager::GetMultFreeBlocks_2(
       RTETask_ITask&            task,
       Msg_List&                 msgList,
       const IOMan_BlockCount&   maxFreeBlocksRequested,
       IOMan_ClusterAddress&     cluster )
{
    cluster = GetMultFreeBlocks( task.ID(), maxFreeBlocksRequested, false );
    return cluster.IsValid();
}

/*---------------------------------------------------------------------------*/



bool
FBM_Manager::GetFreeCluster(
    RTETask_ITask&            task,
    Msg_List&                 msgList,
    const IOMan_BlockCount&   clusterSize,
    IOMan_ClusterAddress&     cluster)
{
    SAPDBTRACE_ROUTINE_DEBUG ("FBM_Manager::GetFreeCluster", FBM_Trace, 5);

    IOMan_BlockAddress      block;
    cluster.Invalidate();
     
    if( m_Active )
    {
        {
            FBM_SynchObject SynchronizeFBM( task.ID() );

            /* get number of a device containing free cluster */
            const IOMan_VolumeNo volNo = GetVolumeWithFreeCluster ();
            if (volNo.IsValid())
            {
                IOMan_BlockNo blockNo;
                if (m_DataVolumes [volNo].getFreeCluster(clusterSize, blockNo))
                {
                    block.SetAddress( volNo, blockNo);
                    cluster.SetCluster( block, clusterSize );

                    SAPDBTRACE_WRITELN( FBM_Trace, 5, "Free volNo  " << cluster.GetVolumeNo()
                                        << " Free blockNo: " << cluster.GetBlockNo( 0 ));

                    SAPDBTRACE_WRITELN( FBM_Trace, 5, "Wanted Num: " << clusterSize
                                        << " Suppl.Num: " << clusterSize );

                    m_TotalNumBlocksFree -= clusterSize;
                }
            }
        }
        if (!cluster.IsValid())
        {
            // no cluster available -> get multiple blocks for unclustered area
            GetMultFreeBlocks_2(task, msgList, clusterSize, cluster);
        }
    }
    return cluster.IsValid();
}
/*---------------------------------------------------------------------------*/

IOMan_BlockAddress
FBM_Manager::GetFreeBlock(
    const tsp00_TaskId  taskId,
    const bool          bReqSequential)
{
    SAPDBTRACE_ROUTINE_DEBUG ("FBM_Manager::GetFreeBlock", FBM_Trace, 5);

    IOMan_BlockAddress block;

    /* This following block is needed to leave the FBM region before entering the     */
    /* log region in the block below. This is done to avoid a dead lock. The FBM      */
    /* region is left when the FBM_SynchObject is destructed at the end of this block */

    while( m_Active )
    {
        {
            FBM_SynchObject SynchronizeFBM( taskId );

            /* get number of a device containing free blocks */
            const IOMan_BlockCount cNumFreeBlocksWanted = 1;
            const IOMan_VolumeNo volNo = GetDeviceWithFreeBlocks (cNumFreeBlocksWanted, bReqSequential);

            if (volNo.IsInvalid())
            {
                FBM_Exception NoMoreSpace( __CONTEXT__, FBM_NO_MORE_FREE_BLOCKS );
                RTE_Message( NoMoreSpace );
            }
            else
            {
                block.SetAddress (volNo, m_DataVolumes[volNo].GetFreeBlock());

                SAPDBTRACE_WRITELN( FBM_Trace, 5, "Free volNo: " << block.GetVolumeNo()
                                    << " Free blockNo: " << block.GetBlockNo() );

                -- m_TotalNumBlocksFree;

                break; // usable cluster found => okay
            }
        }
        m_DBFullWaitQueue.InsertTaskAndWait( taskId );
    }
    return block;
}

/*---------------------------------------------------------------------------*/

IOMan_VolumeNo
FBM_Manager::GetFreeVolumeInGroup(
    const SAPDB_UInt       CurrentVolumeGroup,
    const IOMan_BlockCount NumRequestedBlocks )
{
    SAPDB_UInt     VolumesPerGroup;
    IOMan_VolumeNo VolWithFreeBlocks;
    SAPDB_UInt     VolIndex;

    SAPDBERR_ASSERT_STATE (g01is_archive());

    if (0 == m_ArchiveVolumes.GetSize() % m_NumberVolumeGroups)
    {
        VolumesPerGroup = m_ArchiveVolumes.GetSize() / m_NumberVolumeGroups;
    }
    else
    {
        VolumesPerGroup = m_ArchiveVolumes.GetSize() / m_NumberVolumeGroups + 1;
    }

    if (CurrentVolumeGroup * VolumesPerGroup >= m_ArchiveVolumes.GetSize())
    {
        VolWithFreeBlocks.Invalidate();
        return VolWithFreeBlocks;
    }

    VolIndex = m_ArchiveVolumeGroups[CurrentVolumeGroup];

    do
    {
        SAPDBERR_ASSERT_STATE (RTE_VolumeAccessModeSequential == m_DataVolumes[m_ArchiveVolumes[VolIndex]].VolMode())

        if (m_DataVolumes[m_ArchiveVolumes[VolIndex]].GetNumBlocksFree() < NumRequestedBlocks)
        {
            SAPDBTRACE_WRITELN( FBM_Trace, 5, "Archive Volume Full: " << m_ArchiveVolumes[VolIndex] );
            VolIndex++;
            if ((VolIndex >= ((CurrentVolumeGroup+1)*VolumesPerGroup)) || (VolIndex >=m_ArchiveVolumes.GetSize()))
            {
                VolIndex = CurrentVolumeGroup*VolumesPerGroup;
            }
        }
        else
        {
            VolWithFreeBlocks = m_ArchiveVolumes[VolIndex];
        }
    } while ((VolWithFreeBlocks.IsInvalid()) && (VolIndex != m_ArchiveVolumeGroups[CurrentVolumeGroup]));

    m_ArchiveVolumeGroups[CurrentVolumeGroup] = VolIndex;

    return VolWithFreeBlocks;
}

/*---------------------------------------------------------------------------*/

IOMan_VolumeNo
FBM_Manager::GetDeviceWithFreeBlocks(
    const IOMan_BlockCount NumRequestedBlocks,
    const bool             bReqSequential )
{
    SAPDBTRACE_ROUTINE_DEBUG ("FBM_Manager::GetDeviceWithFreeBlocks", FBM_Trace, 5);

    IOMan_VolumeNo  iVolNo;
    IOMan_VolumeNo  VolWithFreeBlocks;
    bool            VolumeFound = false;

    if (bReqSequential)// archive Volume
    {
        SAPDB_UInt StartVolumeGroup = m_CurArchiveVolumeGroup;

        do
        {
            m_CurArchiveVolumeGroup++;
            if (m_CurArchiveVolumeGroup >= m_NumberVolumeGroups)
            {
                m_CurArchiveVolumeGroup = 0;
            }

            VolWithFreeBlocks = GetFreeVolumeInGroup(m_CurArchiveVolumeGroup, NumRequestedBlocks);

            if (VolWithFreeBlocks.IsValid())
            {
                VolumeFound = true;
            }
        }while(!VolumeFound && (m_CurArchiveVolumeGroup != StartVolumeGroup));
    }
    else
    {
        /* search a data device which has still NumRequestedBlocks free blocks */
        IOMan_VolumeNo  SearchBegDevNo = m_NextDevToUseForNewBlock;
        iVolNo                         = SearchBegDevNo;

        SAPDBTRACE_WRITELN( FBM_Trace, 5, "FirstVolNo: " << m_DataVolumes.GetFirstVolumeNo()
                            << " LastVolNo: " << m_DataVolumes.GetLastVolumeNo() );

        SAPDBTRACE_WRITELN( FBM_Trace, 5, "SearchBegDev: " << SearchBegDevNo
                            << " BlocksWanted: " << NumRequestedBlocks );

        /* loop over all devices until a device with NumRequestedBlocks free blocks is found */
        do
        {
            if(  m_DataVolumes.IsRegistered( iVolNo ))
            {
                if (RTE_VolumeAccessModeNormal == m_DataVolumes[iVolNo].VolMode())
                {
                    FBM_IDataVolume & Volume = (FBM_IDataVolume&) m_DataVolumes[iVolNo];
                    if (Volume.GetNumBlocksFree() < NumRequestedBlocks)
                    {
                        SAPDBTRACE_WRITELN( FBM_Trace, 5, "Device Full: " << iVolNo);
                    }
                    else
                    {
                        VolumeFound       = true;
                        VolWithFreeBlocks = iVolNo;

                        /* if this device contains too few used blocks this device is choosen imediately.   */
                        /* Otherwise this device is skipped this time and the search is continued. However, */
                        /* m_NumBlocksToAddUntilOptIsReached is changed so that this device is choosen      */
                        /* next time it is looked at                                                        */
                        if (Volume.GetNumBlocksToAddUntilOptIsReached() > 0)
                            break;
                        else
                            Volume.SetNumBlocksToAddUntilOptIsReached(1);
                    }

                }
            }
            /* goto next volume */
            if ((++iVolNo) > m_DataVolumes.GetLastVolumeNo())
            {
                iVolNo = m_DataVolumes.GetFirstVolumeNo();
            }
        }
        while (iVolNo != SearchBegDevNo);

        /* return device or throw exception if there is no device with enough free blocks */
        if (VolumeFound)
        {
            /* set device where to start the next search */
            if (VolWithFreeBlocks == m_DataVolumes.GetLastVolumeNo())
            {
                m_NextDevToUseForNewBlock = m_DataVolumes.GetFirstVolumeNo();
            }
            else
            {
                m_NextDevToUseForNewBlock = VolWithFreeBlocks+1;
            }
        }
    }
    if (!VolumeFound)
    {
        VolWithFreeBlocks.Invalidate();
    }

    SAPDBTRACE_WRITELN( FBM_Trace, 5, "Free_Device: " << VolWithFreeBlocks);

    return (VolWithFreeBlocks) ;
}

/*---------------------------------------------------------------------------*/

IOMan_VolumeNo
FBM_Manager::GetVolumeWithFreeCluster()
{
    IOMan_VolumeNo searchVolume = SearchVolumeWithFreeCluster();

    if (searchVolume.IsInvalid())
    {
        ReserveAddClusterSpace(5);
        searchVolume = SearchVolumeWithFreeCluster();
    }

    return searchVolume;
}

/*---------------------------------------------------------------------------*/

IOMan_VolumeNo
FBM_Manager::SearchVolumeWithFreeCluster()
{
    SAPDBTRACE_ROUTINE_DEBUG ("FBM_Manager::SearchVolumeWithFreeCluster", FBM_Trace, 5);

    IOMan_VolumeNo  iVolNo;
    IOMan_VolumeNo  VolWithFreeCluster; // result
    bool            VolumeFound = false;

    {
        /* search a data volume which has a free cluster */
        IOMan_VolumeNo SearchBegVolume = m_NextVolToUseForNewCluster;
        if (SearchBegVolume < m_DataVolumes.GetFirstVolumeNo())
        {
            SearchBegVolume = m_DataVolumes.GetFirstVolumeNo();
        }
        if (SearchBegVolume > m_DataVolumes.GetLastVolumeNo())
        {
            SearchBegVolume = m_DataVolumes.GetLastVolumeNo();
        }
        iVolNo                         = SearchBegVolume;

        SAPDBTRACE_WRITELN( FBM_Trace, 5, "FirstVolNo: " << m_DataVolumes.GetFirstVolumeNo()
                            << " LastVolNo: " << m_DataVolumes.GetLastVolumeNo() );

        SAPDBTRACE_WRITELN( FBM_Trace, 5, "SearchBegVol: " << SearchBegVolume);

        /* loop over all volumes until a volume with NumRequestedBlocks free blocks is found */
        do
        {
            if( m_DataVolumes.IsRegistered( iVolNo ))
            {
                if (RTE_VolumeAccessModeNormal == m_DataVolumes[iVolNo].VolMode())
                {
                    FBM_IDataVolume & Volume = (FBM_IDataVolume&) m_DataVolumes[iVolNo];
                    if (Volume.GetNumClustersFree() < 1)
                    {
                        SAPDBTRACE_WRITELN( FBM_Trace, 5, "Volume Full: " << iVolNo);
                    }
                    else
                    {
                        VolumeFound       = true;
                        VolWithFreeCluster = iVolNo;
                        break;
                    }
                }
            }
            /* goto next volume */
            if ((++iVolNo) > m_DataVolumes.GetLastVolumeNo())
            {
                iVolNo = m_DataVolumes.GetFirstVolumeNo();
            }
        }
        while (iVolNo != SearchBegVolume);

        /* return volume or throw exception if there is no volume with enough free blocks */
        if (VolumeFound)
        {
            /* set device where to start the next search */
            if (VolWithFreeCluster == m_DataVolumes.GetLastVolumeNo())
            {
                m_NextVolToUseForNewCluster = m_DataVolumes.GetFirstVolumeNo();
            }
            else
            {
                m_NextVolToUseForNewCluster = VolWithFreeCluster+1;
            }
        }
    }
    if (!VolumeFound)
    {
        VolWithFreeCluster.Invalidate();
    }

    SAPDBTRACE_WRITELN( FBM_Trace, 5, "Free_Device: " << VolWithFreeCluster);

    return (VolWithFreeCluster) ;
}


IOMan_BlockCount
FBM_Manager::NumBlocksMarkedForBackup( const tsp00_TaskId taskId ) const
{
    SAPDBTRACE_ROUTINE_DEBUG ("FBM_Manager::NumBlocksMarkedForBackup", FBM_Trace, 5);

    IOMan_BlockCount NumAllBlocksMarkedForBackUp = 0;

    FBM_SynchObject SynchronizeFBM( taskId );

    FBM_DataVolumeIterator volIter = FBM_DataVolumeIterator(m_DataVolumes);

    while(*volIter)
    {
#       ifdef SAPDB_SLOW 
        if ((*volIter)->GetNumBlocksMarkedForBackup() > 0)
            SAPDBTRACE_WRITELN( FBM_Trace, 5, "bup blocks  " <<  (*volIter)->GetNumBlocksMarkedForBackup());
#       endif

        NumAllBlocksMarkedForBackUp += (*volIter)->GetNumBlocksMarkedForBackup();
        ++volIter;
    }
    return NumAllBlocksMarkedForBackUp;
}

/*---------------------------------------------------------------------------*/

void
FBM_Manager::RestoreAllBlockStatesMarkedForBackup( const tsp00_TaskId TaskId )
{
    SAPDBTRACE_ROUTINE_DEBUG ("FBM_Manager::RestoreAllBlockStatesMarkedForBackup", FBM_Trace, 5);

    IOMan_VolumeNo      iVolNo;
    IOMan_BlockCount    NumBlocksRestoredToFreeAfterSVP;

    FBM_SynchObject SynchronizeFBM (TaskId);

    FBM_DataVolumeIterator volIter = FBM_DataVolumeIterator(m_DataVolumes);

    while(*volIter)
    {
        (*volIter)->RestoreAllBlockStatesMarkedForBackup(NumBlocksRestoredToFreeAfterSVP);
        m_TotalNumBlocksFreeAfterSVP += NumBlocksRestoredToFreeAfterSVP;
        m_TotalNumBlocksBackupFreeAfterSVP -= NumBlocksRestoredToFreeAfterSVP;
        ++volIter;
    }
}

/*---------------------------------------------------------------------------*/

void
FBM_Manager::RestoreBlockStateMarkedForBackup(
    RTETask_ITask&                      task,
    const IOMan_ComplexClusterAddress&  cluster )
{
    SAPDBTRACE_ROUTINE_DEBUG ("FBM_Manager::RestoreBlockStateMarkedForBackup", FBM_Trace, 5);

    const IOMan_VolumeNo volNo = cluster.GetVolumeNo();
    bool  bRestoredStateIsFreeAfterSVP = false;

    SAPDBTRACE_WRITELN( FBM_Trace, 5, "RestoreBackupBlocks: " << volNo << "/"
                        << cluster.GetBlockNo ( 0 ) << "(" << cluster.GetBlockCount() << ")" );

    FBM_SynchObject SynchronizeFBM( task.ID());

    /* check device number and get a reference to the current device */
    FBM_IVolume& IVolume = m_DataVolumes [volNo];

    if (RTE_VolumeAccessModeNormal != IVolume.VolMode()){
        return;
    }
    FBM_IDataVolume& Volume = (FBM_IDataVolume&) IVolume;

    for(  IOMan_ClusterAddress::BlockIndex blockIndex = 0; blockIndex < cluster.GetBlockCount(); ++blockIndex )
    {
        if (cluster.IsBlockInUse(blockIndex))
        {
            const IOMan_BlockNo  blockNo = cluster.GetBlockNo( blockIndex );
            if( BlockState_BackUp != Volume.GetBlockState( blockNo ))
            {
                /* if none of the cases above was fulfilled an error occured */
                /* write error message into knldiag and vtrace               */
                FBM_IllegalRestoreAfterBackup ErrorMesage (__CONTEXT__, volNo, blockNo,
                        Volume.GetBlockState( blockNo ),
                        Volume.GetBlockStateAfterBackup( blockNo ));
                RTE_Crash( ErrorMesage );
            }
            Volume.RestoreBlockStateMarkedForBackup( blockNo, bRestoredStateIsFreeAfterSVP );

            if( bRestoredStateIsFreeAfterSVP )
            {
                ++m_TotalNumBlocksFreeAfterSVP;
                --m_TotalNumBlocksBackupFreeAfterSVP;
            }
        }
    }
}

/*---------------------------------------------------------------------------*/

void
FBM_Manager::SetAllBlocksMarkedAsFreeAfterSVPToFree( const tsp00_TaskId TaskId )
{
    /* go through all registered devices an set the states of all */
    /* blocks which are marked as free after savepoint to free    */

    SAPDBTRACE_ROUTINE_DEBUG ("FBM_Manager::SetAllBlocksMarkedAsFreeAfterSVPToFree", FBM_Trace, 5);

    {
        FBM_SynchObject SynchronizeFBM (TaskId);

        FBM_DataVolumeIterator volIter = FBM_DataVolumeIterator(m_DataVolumes);
        IOMan_ClusterNo maxClusterNo;
        IOMan_ClusterNo totalMaxClusterNo;

        while(*volIter)
        {
            if ((*volIter)->GetNumBlocksFreeAfterSVP() > 0)
            {
                /* update global counters of all blocks free and free after save point */
                m_TotalNumBlocksFreeAfterSVP -= (*volIter)->GetNumBlocksFreeAfterSVP();
                if (!(*volIter)->IsToDrop())
                {
                    m_TotalNumBlocksFree         += (*volIter)->GetNumBlocksFreeAfterSVP();
                }

                (*volIter)->SetAllBlocksMarkedAsFreeAfterSVPToFree();
            }
            maxClusterNo = ((FBM_SimpleDataVolume*)(*volIter))->CalcMaxClusterNo();
            if (totalMaxClusterNo.IsInvalid() && maxClusterNo.IsValid())
            {
                totalMaxClusterNo = maxClusterNo;
            }
            else
            {
                totalMaxClusterNo = SAPDB_MAX(totalMaxClusterNo, maxClusterNo);
            }
            ++volIter;
        }

        /* update counters which control which devices are prefered when free blocks are   */
        /* requested  (getfreeblock) depending on the number of used blocks on each device */
        ChangeDeviceUsagePreferences ();

        /* resize Cluster area */
        if (totalMaxClusterNo.IsInvalid())
        {
            ReserveClusterSpace(0);
        }
        else
        {
            ReserveClusterSpace(totalMaxClusterNo+1);
        }


        /* reset the trigger flag,i.e. from now the routine GetFreeBlock */
        /* can trigger again a savepoint if there are too few blocks     */
        m_SavepointIsRequested = false;

        /* check that the global counter of all blocks which are free are svp is zero now */
        SAPDBERR_ASSERT_STATE (m_TotalNumBlocksFreeAfterSVP == 0)
    }
    if( 0 < GetNumberOfFreeBlocks() && ! m_DBFullWaitQueue.IsEmpty()){
        m_DBFullWaitQueue.ResumeAllWaitingTasks( TaskId );
    }
}

/*---------------------------------------------------------------------------*/

FBM_IManager::GetBackupBlocksRc
FBM_Manager::GetNextBlocksForBackUp(
    RTETask_ITask&                  task,
    Msg_List&                       msgList,
    const IOMan_BlockCount          maxBlocksWanted,
    IOMan_ComplexClusterAddress&    cluster,
    bool&                           bIsClustered )
{
    SAPDBTRACE_ROUTINE_DEBUG ("FBM_Manager::GetNextBlocksForBackUp", FBM_Trace, 5);

    if( ! m_Active )
    {
        // fill msgList - FIXME!
        return gbbRcSystemError;
    }

    FBM_SynchObject SynchronizeFBM( task.ID() );

    // current search mode: clustered or not
    bool searchClustered;
    
    // current volume to inspect
    IOMan_VolumeNo searchVol;

    cluster.Invalidate();
    
    do  // loop if cluster mode changes -> try again unclustered
    {
        searchVol   = m_NextDevToUseForBackup;   // start at last hint position
        searchClustered = m_BackupClustered;     // initialize with current cluster mode

        do  // loop until volume with blocks is found
        {
            /* check volume number and get a reference to the current volume */
            if(  m_DataVolumes.IsRegistered( searchVol ))
            {
                FBM_IVolume & iVolume = m_DataVolumes [searchVol];

                if (RTE_VolumeAccessModeNormal == iVolume.VolMode())  // we don't backup sequential volumes
                {
                    FBM_IDataVolume &volume = (FBM_IDataVolume&) iVolume;  // assign to correct type

                    /* get blocks marked for backup */
                    volume.GetNextBlocksForBackUp (maxBlocksWanted, searchClustered, cluster);
                }
            }
            
            // go to next volume
            if ((++searchVol) > m_DataVolumes.GetLastVolumeNo())
            {
                searchVol = m_DataVolumes.GetFirstVolumeNo();
            }
        } while ((searchVol != m_NextDevToUseForBackup) && (cluster.GetUsedBlocks() == 0));  
        
        if ((0 == cluster.GetUsedBlocks()) && (searchClustered == true))  // switch from clustered backup to non clustered
        {
            m_BackupClustered = false;
        }
    } while (searchClustered != m_BackupClustered);  // cluster mode has changed?
    m_NextDevToUseForBackup = searchVol;             // update volume hint

    bIsClustered = m_BackupClustered;

    if( 0 == cluster.GetUsedBlocks() ){
        return gbbRcNoMoreBackupBlocks;
    }

    SAPDBTRACE_WRITELN( FBM_Trace, 5, "num_found: " << cluster.GetUsedBlocks()
                        << " VolNo: " << cluster.GetDeviceNo()
                        << " blockNo: " << cluster.GetBlockNo(0) );
    return gbbRcOkay;
}

/*---------------------------------------------------------------------------*/

void
FBM_Manager::BeginReadingBlocksMarkedForBackUp( const tsp00_TaskId TaskId )
{
    SAPDBTRACE_ROUTINE_DEBUG ("FBM_Manager::BeginReadingBlocksMarkedForBackUp", FBM_Trace, 5);

    /* this routine initializes the  position (iterator)                              */
    /* from where to start to look for blocks marked for backup.                      */
    /* All blocks marked for backup can than be fetched                               */
    /* by calling GetNextBlocksForBackUp until this function returns no more blocks   */

    FBM_SynchObject SynchronizeFBM (TaskId);
    m_NextDevToUseForBackup.Invalidate();
    m_BackupClustered = true;      // start with clustered blocks

    FBM_DataVolumeIterator volIter = FBM_DataVolumeIterator(m_DataVolumes);

    while(*volIter)
    {
        if (m_NextDevToUseForBackup.IsInvalid()) // set to first valid volume
        {
            m_NextDevToUseForBackup = volIter.GetPosition();
        }

        SAPDBERR_ASSERT_STATE (!(*volIter)->IsBackupRunning());

        if ((*volIter)->GetNumBlocksMarkedForBackup() > 0)
        {
            (*volIter)->InitActBlockNoForBackup();
        }
        ++volIter;
    }
}

/*---------------------------------------------------------------------------*/

bool
FBM_Manager::ReserveClusterSpace(const SAPDB_UInt4 numCluster)
{
    SAPDBTRACE_ROUTINE_DEBUG ("FBM_Manager::ReserveClusterSpace", FBM_Trace, 5);
    SAPDBTRACE_WRITELN( FBM_Trace, 5, "numCluster: " << numCluster);

    FBM_DataVolumeIterator testVolIter = FBM_DataVolumeIterator(m_DataVolumes);

    bool spaceAvailable = true;

    while(*testVolIter && spaceAvailable)
    {
        spaceAvailable &= ((FBM_SimpleDataVolume*)(*testVolIter))->testClusterSpace(numCluster);
        ++testVolIter;
    }

    if (spaceAvailable)
    {
        FBM_DataVolumeIterator setVolIter = FBM_DataVolumeIterator(m_DataVolumes);

        while(*setVolIter)
        {
            ((FBM_SimpleDataVolume*)(*setVolIter))->reserveClusterSpace(numCluster);
            ++setVolIter;
        }
        kb57SetClusterReservedSpace(numCluster);
        m_reservedClusters = numCluster;
    }

    return spaceAvailable;
}
/*---------------------------------------------------------------------------*/

IOMan_BlockCount
FBM_Manager::GetNumberOfClusteredBlocksToBackup(const tsp00_TaskId    taskId) const
{
    FBM_SynchObject SynchronizeFBM( taskId );

    FBM_DataVolumeIterator volIter   = FBM_DataVolumeIterator(m_DataVolumes);
    IOMan_BlockCount clusteredBlocks = 0;

    while(*volIter)
    {
        clusteredBlocks += (*volIter)->GetNumberOfClusteredBlocksToBackup();
        ++volIter;
    }

    return clusteredBlocks;
}

/*---------------------------------------------------------------------------*/

void
FBM_Manager::Dump(
    const tsp00_TaskId    taskId,
    Kernel_Dump           &dump ) const
{
    struct FBMManager   fbmMan;

    FBM_SynchObject SynchronizeFBM( taskId );

    fbmMan.dmpActive                     = m_Active;
    fbmMan.dmpSvpIsTriggered             = m_SavepointIsRequested;
    fbmMan.dmpFiller1                    = 0;
    fbmMan.dmpMaxNumDev                  = m_DataVolumes.GetMaxVolumeNo();
    fbmMan.dmpNumDev                     = m_NumDev;
    fbmMan.dmpLastVolume                 = m_DataVolumes.GetLastVolumeNo();
    fbmMan.dmpTotalNumBlocksFreeAfterSVP = m_TotalNumBlocksFreeAfterSVP;
    fbmMan.dmpTotalNumBlocksBackupFreeAfterSVP = m_TotalNumBlocksBackupFreeAfterSVP;
    fbmMan.dmpTotalNumBlocksFree         = m_TotalNumBlocksFree;
    fbmMan.dmpTotalNumBlocks             = m_TotalNumBlocks;
    fbmMan.dmpTotalUsedBlocks            = m_TotalNumBlocks - m_TotalNumBlocksFree
                                           - m_TotalNumBlocksFreeAfterSVP - m_TotalNumBlocksBackupFreeAfterSVP;
    fbmMan.dmpNextDevToUseForNewBlock    = m_NextDevToUseForNewBlock;

    dump.InsertEntry( Kernel_Dump::DmpFBMManager,
                      Kernel_DumpPage::Entry( &fbmMan, sizeof( fbmMan )));

    if( m_Active )
    {
        for( IOMan_VolumeNo volNo = m_DataVolumes.GetMinVolumeNo();
                volNo <= m_DataVolumes.GetMaxVolumeNo(); ++volNo )
        {
            if(  m_DataVolumes.IsRegistered( volNo ))
            {
                m_DataVolumes[ volNo ].Dump( dump, volNo );
            }
        }
    }
}

/*===========================================================================*
 *  PRIVATE METHODS                                                          *
 *===========================================================================*/

void
FBM_Manager::ChangeDeviceUsagePreferences ()
{
    SAPDBTRACE_ROUTINE_DEBUG ("FBM_Manager::ChangeDeviceUsagePreferences", FBM_Trace, 5);

    /* update counters which control which devices are prefered when free blocks are   */
    /* requested  (getfreeblock) depending on the number of used blocks on each device */
    /* the prefrences are calculated to get optimum parallel read accesses that means  */
    /* all devices should accomodate the same number of blocks                         */
    IOMan_BlockCount MaxNumBlocksUsed = 0;

    FBM_DataVolumeIterator volIter = FBM_DataVolumeIterator(m_DataVolumes);

    while(*volIter)
    {
        if (MaxNumBlocksUsed < (*volIter)->GetNumBlocksUsed()){
            MaxNumBlocksUsed = (*volIter)->GetNumBlocksUsed();
        }
        ++volIter;
    }

    /* calculate the number of used blocks to add to each */
    /* device that all devices have the same occupation   */

    FBM_DataVolumeIterator volIter2 = FBM_DataVolumeIterator(m_DataVolumes);

    while(*volIter2)
    {
        (*volIter2)->SetNumBlocksToAddUntilOptIsReached((MaxNumBlocksUsed - (*volIter2)->GetNumBlocksUsed()));

        SAPDBTRACE_WRITELN( FBM_Trace, 5, "volNo: " << volIter2.GetPosition()
                            << " OptAddNumBlo: " << (*volIter2)->GetNumBlocksToAddUntilOptIsReached() );
        ++volIter2;
    }
}

/*---------------------------------------------------------------------------*/

void
FBM_Manager::ResetMembersToNullAndZero()
{
    m_Active                     = false;
    m_BackupClustered            = true;
    m_TotalNumBlocksFreeAfterSVP = 0;
    m_TotalNumBlocksBackupFreeAfterSVP = 0;
    m_TotalNumBlocksFree         = 0;
    m_TotalNumBlocks             = 0;
    m_ClusterSize                = 0;
    m_NumDev                     = 0;
    m_SavepointIsRequested       = false;
    m_NextDevToUseForNewBlock.Invalidate();
    m_NextDevToUseForBackup.Invalidate();
    m_NextVolToUseForNewCluster.Invalidate();
    m_LastArchiveVolumeUsed.Invalidate();
    m_CurArchiveVolumeGroup      = 0;
    m_NumberVolumeGroups         = 0;

    m_ArchiveVolumes.Clear();
    m_ArchiveVolumeGroups.Clear();
}


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
