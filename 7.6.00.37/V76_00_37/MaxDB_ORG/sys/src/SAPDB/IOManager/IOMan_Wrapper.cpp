/*
  @file           IOMan_Wrapper.cpp
  @author         TorstenS
  @ingroup        IOManagement
  @brief          This module is used access c++ code from pascal code

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



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "ggg00.h"   // PASCAL: GG_kernel_constants_and_types
#include "gbd00.h"
#include "gsp03.h"
#include "hgg04.h"
#include "gkb03.h"

#include "DataAccess/Data_PageFrame.hpp"
#include "DataAccess/Data_BasePage.hpp"
#include "DataAccess/Data_Types.hpp"
#include "FrameControl/FrameCtrl_ILog.hpp"
#include "FreeBlockManagement/FBM_IManager.hpp"
#include "IOManager/IOMan_IDataInfo.hpp"
#include "IOManager/IOMan_IDataIO.hpp"
#include "IOManager/IOMan_IDataManager.hpp"
#include "IOManager/IOMan_IDiagnose.hpp"
#include "IOManager/IOMan_ILogInfo.hpp"
#include "IOManager/IOMan_ILogManager.hpp"
#include "IOManager/IOMan_IMigration.hpp"
#include "IOManager/IOMan_AsynIOHandle.hpp"
#include "IOManager/IOMan_KernelPages.hpp"
#include "IOManager/IOMan_Manager.hpp"
#include "IOManager/IOMan_Types.hpp"
#include "IOManager/IOMan_MessageList.hpp"
#include "IOManager/IOMan_IDataVolumeInfo.hpp"
#include "Logging/Log_Page.hpp"
#include "Logging/Log_Types.hpp"
#include "Logging/Log_Volume.hpp"
#include "RunTime/RTE_Message.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "Transaction/Trans_Context.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define ALLOCATE_FAILED_MSG1 "IOManager: bad alloc    "


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/


/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

externC void bd999CheckPageAfterRead(
    tgg00_BasisError   &TrError,
    tbd_nodeptr        &pNode)
{
    if( pNode->nd_pt2() == pt2Checkpt_egg00)
    {
        tkb00_PagePtr   pRestart = REINTERPRET_CAST( tkb00_PagePtr, pNode );

        Kernel_RestartPage  RestartPage( pRestart );
        if( RestartPage.CheckAfterRead() ) return;
    }
    else
    {
        Data_BasePage   DataPage( pNode );
        if( DataPage.CheckAfterRead() ) return;
    }
    TrError = e_bad_datapage;
}

/*---------------------------------------------------------------------------*/

externC void bd999CreateAllLogVolumes(
    const tsp00_TaskId  taskId,
    tgg00_BasisError    &trError )
{
    if(! IOMan_ILogManager::GetInstance().CreateAllLogVolumes( taskId ))
        trError = e_disk_not_accessible;
};

/*---------------------------------------------------------------------------*/

externC void bd999CreateAllDataVolumes(
    const tsp00_TaskId  taskId,
    tgg00_BasisError    &trError )
{

    if(! IOMan_IDataManager::GetInstance().CreateAllDataVolumes(  taskId ))
        trError = e_disk_not_accessible;
};

/*---------------------------------------------------------------------------*/

externC void bd999AttachAllDataVolumes(
    tsp00_TaskId      taskId,
    tgg00_BasisError  &trError )
{
    if(! IOMan_IDataManager::GetInstance().OpenAllDataVolumes( taskId ))
    {
        trError = e_disk_not_accessible;
        return;
    }
};

/*---------------------------------------------------------------------------*/

externC void bd999AttachAllLogVolumes(
    tsp00_TaskId      taskId,
    tgg00_BasisError  &trError )
{
    if( ! Log_Volume::Instance().OpenLogArea( taskId ) )
        trError = e_disk_not_accessible;
}

/*---------------------------------------------------------------------------*/

externC tsp00_Int4 bd999IsDataVolumeConfigured( tsp00_Int2 devNo )
{
    return( IOMan_IDataInfo::GetInstance().IsConfigured( devNo ));
};

/*---------------------------------------------------------------------------*/

externC tsp00_Int4 bd999DataDevSpaceSize( tsp00_Int2 devNo )
{
    return( IOMan_IDataInfo::GetInstance().GetDataVolumeSize( devNo ));
};

/*---------------------------------------------------------------------------*/

externC tsp00_Int4 bd999LogDevSpaceSize( tsp00_Int2 devNo )
{
    return( IOMan_ILogInfo::GetInstance().GetLogVolumeSize( devNo ));
};

/*---------------------------------------------------------------------------*/

externC void bd999DetachAllVolumes( tsp00_TaskId  taskId )
{
    IOMan_IDataManager::GetInstance().CloseAllDataVolumes( taskId );
    Log_Volume::Instance().CloseLogArea( taskId );
};

/*---------------------------------------------------------------------------*/

externC void bd999DetachAllDataVolumes( tsp00_TaskId  taskId )
{
    IOMan_IDataManager::GetInstance().CloseAllDataVolumes( taskId );
}

/*---------------------------------------------------------------------------*/

externC void bd999DetachAllArchiveLogs( tsp00_TaskId  taskId )
{
    Log_Volume::Instance().CloseLogArea( taskId );
}

/*---------------------------------------------------------------------------*/

externC tsp00_Int4 bd999LogIsMirrored()
{
    return( IOMan_ILogInfo::GetInstance().IsLogMirrored() );
}

/*---------------------------------------------------------------------------*/

externC tsp00_Int4 bd999NoOfDataDevices()
{
    tsp00_Int4 NumOfDevSpaces = 0;

    NumOfDevSpaces = IOMan_IDataInfo::GetInstance().UsedDataVolumes();

    if( 0 == NumOfDevSpaces )
        return( IOMan_IDataInfo::GetInstance().ConfiguredDataVolumes());
    else
        return( NumOfDevSpaces );
};

/*---------------------------------------------------------------------------*/

externC tsp00_Int4 bd999MaxConfigurableDataVolumes()
{
    return( IOMan_IDataInfo::GetInstance().MaxConfigurableDataVolumes());
};

/*---------------------------------------------------------------------------*/

externC tsp00_Int4  bd999NoOfLogDevices()
{
    return( IOMan_ILogInfo::GetInstance().ConfiguredLogVolumes() );
}

/*---------------------------------------------------------------------------*/

externC tsp00_Int4 bd999TotalDataPages()
{
    return( IOMan_IDataInfo::GetInstance().TotalUsableDataPages());
};

/*---------------------------------------------------------------------------*/

externC void bd999ReadRestartPage(
    const tsp00_TaskId  taskId,
    const tkb00_PagePtr &pRestart )
{
    Kernel_RestartPage  RestartPage( pRestart );

    IOMan_IDataIO::GetInstance().ReadRestartPage( taskId, RestartPage );
};

/*---------------------------------------------------------------------------*/

externC void bd999WriteRestartPage(
    tsp00_TaskId    TaskId,
    tkb00_PagePtr   &pRestart )
{
    Kernel_RestartPage  RestartPage( pRestart );

    IOMan_IDataIO::GetInstance().WriteRestartPage( TaskId, RestartPage );
};

/*---------------------------------------------------------------------------*/

externC void bd999WriteDataPage(
    tgg00_TransContext  &Trans,
    tbd_nodeptr         &pNode
)
{
    Data_BasePage   Page( pNode,  !gg04IsStaticPage( pNode->nd_pt2() ),
                          pNode->nd_pmode().includes( pmTemp_egg00 ) );

    IOMan_IDataIO::GetInstance().WriteDataPage( Trans.trTaskId_gg00, Page );
};

/*---------------------------------------------------------------------------*/

externC void bd999AsynReadDataPages(
    tgg00_TransContext&     t,
    tsp00_PageAddr          pPageList,
    SAPDB_Int4              maxBlockCount,
    tkb3_save_static_ptr    pAsynIOHandle,
    SAPDB_Int2&             blocksToBeBackupedINT2,
    pasbool                 bDoExtendedCheckPAS,
    pasbool&                bIsClusteredPAS )
{
    Trans_Context&      trans = Trans_Context::GetContext( t );
    IOMan_AsynIOHandle  asynIOHandle( pAsynIOHandle );
    IOMan_DataPages     pages( trans.Allocator(), UTF8( "Pager:ReadDataPages" ));
    if(! pages.Initialize( maxBlockCount ))
    {
        trans.SetErrorCode( e_no_more_memory );
        return;
    }
    for( SAPDB_Int2 Index = 0; Index < maxBlockCount; ++Index )
    {
        tbd_nodeptr pNode = reinterpret_cast<tbd_nodeptr>( pPageList + Index );

        Data_BasePage page( pNode, true, ! true ); // recMode unknown!!!
        pages.Append( page );
    }
    const bool bDoExtendedCheck = (0 != bDoExtendedCheckPAS);
    bool  bIsClustered          = false;
    IOMan_BlockCount  blocksToBeBackuped = 0;
    if( ! IOMan_IDataIO::GetInstance().ReadDataPagesForBackup( trans.Task(),
            trans.ErrorList(), asynIOHandle, pages, blocksToBeBackuped,
            bDoExtendedCheck, bIsClustered ))
    {
        trans.SetErrorCode( e_system_error );
    }
    blocksToBeBackupedINT2 = static_cast<SAPDB_Int2>( blocksToBeBackuped );
    bIsClusteredPAS = (0 != bIsClustered);
}

/*---------------------------------------------------------------------------*/

externC void bd999AsynWriteDataPages(
    tgg00_TransContext&     t,
    tsp00_PageAddr          pPageList,
    SAPDB_Int2              maxBlocksToBeRestored,
    tkb3_save_static_ptr    pAsynIOHandle,
    pasbool                 bDoExtendedCheckPAS,
    pasbool                 bIsClusteredPAS )
{
    Trans_Context&      trans = Trans_Context::GetContext( t );
    IOMan_AsynIOHandle  asynIOHandle( pAsynIOHandle );
    IOMan_DataPages     pages( trans.Allocator(), UTF8( "Pager:WriteDataPages" ));

    if( ! pages.Initialize( maxBlocksToBeRestored ))
    {
        trans.SetErrorCode( e_no_more_memory );
        return;
    }
    for( SAPDB_Int2 index = 0; index < maxBlocksToBeRestored; ++index )
    {
        tbd_nodeptr pNode = reinterpret_cast<tbd_nodeptr>( pPageList + index );

        Data_BasePage page( pNode, true, ! true ); // recMode unknown!!!
        pages.Append( page );
    }
    const bool bDoExtendedCheck = (0 != bDoExtendedCheckPAS);
    const bool bIsClustered     = bIsClusteredPAS;
    if( ! IOMan_IDataIO::GetInstance().WriteDataPagesForRestore( trans.Task(),
            trans.ErrorList(), asynIOHandle, pages, bDoExtendedCheck, bIsClustered ))
    {
        trans.SetErrorCode( e_system_error );
    }
}

/*---------------------------------------------------------------------------*/

externC void bd999GetDataVolumeName(
    SAPDB_Int2      devNo,
    tsp00_VFilename &devName )
{
    IOMan_IDataInfo::GetInstance().GetDataVolumeName( devNo, devName );
}

/*---------------------------------------------------------------------------*/

externC tsp00_Int4 bd999GetDataIOBlockCount()
{
    return IOMan_IDataInfo::GetInstance().GetDataIOBlockCount();
}

/*---------------------------------------------------------------------------*/

externC bool bd999GetNextDataVolumeNameForBackup(
    SAPDB_Int2      &physVolNo,
    tsp00_VFilename &devName,
    SAPDB_Bool      isForWriting)
{
    bool volumeFound;

    do {
        volumeFound = false;
        if ( ++physVolNo  <= IOMan_IDataInfo::GetInstance().MaxConfigurableDataVolumes())
        {
            if ( IOMan_IDataInfo::GetInstance().GetDataVolumeName( physVolNo, devName ) )
            {
                if(IOMan_DataVolumeInfo(physVolNo).IsVolumeOnline())
                {
                    if (isForWriting)  // open allways
                    {
                        volumeFound = true;
                    }
                    else               // open only volumes with backup blocks
                    {
                        volumeFound = (FBM_IManager::Instance().GetNumBlocksMarkedForBackup (physVolNo) > 0);
                    }
                }
            }
        }
    } while (!volumeFound && (physVolNo <= IOMan_IDataInfo::GetInstance().MaxConfigurableDataVolumes()));
    return volumeFound;
}


/*---------------------------------------------------------------------------*/

externC void bd999GetLogVolumeName(
    SAPDB_Int2      devNo,
    tsp00_VFilename &devName )
{
    IOMan_ILogInfo::GetInstance().GetLogVolumeName( devNo, devName );
}

/*---------------------------------------------------------------------------*/

externC void bd999GetMirrLogVolumeName(
    SAPDB_Int2      devNo,
    tsp00_VFilename &devName )
{
    IOMan_ILogInfo::GetInstance().GetMirrLogVolumeName( devNo, devName );
}
/*---------------------------------------------------------------------------*/

externC void bd999MigrateDataBase(
    tsp00_TaskId       taskId,
    tgg00_BasisError   &trError )
{
    RTETask_ITask    &task = *RTETask_ITask::ByTaskID( taskId );
    const IOMan_ReturnCode retCode = IOMan_IMigration::GetInstance().MigrateDataBase( task );

    if( IOMan_NoMoreMemory == retCode )
        trError = e_no_more_memory;
    else if( IOMan_NotPreparedForMigration == retCode )
        trError = e_not_prepared_for_migration;
}

/*---------------------------------------------------------------------------*/

externC bool bd999PrepareDropDataVolume(
    tsp00_TaskId     taskId,
    tgg00_VoidPtr    pErrorList )
{
    SAPDBERR_ASSERT_STATE( 0 != pErrorList );

    RTETask_ITask   &task    = *RTETask_ITask::ByTaskID( taskId );
    Msg_List        &msgList = *( reinterpret_cast<Msg_List*>( pErrorList ));

    return IOMan_IDataManager::GetInstance().PrepareVolumeForDrop( task, msgList );
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
