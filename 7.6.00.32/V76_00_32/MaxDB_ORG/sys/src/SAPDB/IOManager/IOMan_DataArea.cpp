/*!
    @file       IOMan_DataArea.cpp
    @author     TorstenS
    @ingroup    IOManagement
    @brief      This module is used to manage the data area.

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

#include "hkb57.h"
#include "hgg01_3.h"
#include "hgg04.h"

#include "IOManager/IOMan_DataArea.hpp"
#include "IOManager/IOMan_AsynIOHandle.hpp"
#include "IOManager/IOMan_Messages.hpp"
#include "IOManager/IOMan_MessageList.hpp"
#include "IOManager/IOMan_ComplexClusterAddress.hpp"
#include "Converter/Converter_IManager.hpp"
#include "Converter/Converter_IBackUp.hpp"
#include "DataAccess/Data_PageFrame.hpp"
#include "DataAccess/Data_IOBuffer.hpp"
#include "FreeBlockManagement/FBM_IManager.hpp"
#include "Messages/Msg_IOutput.hpp"
#include "RunTime/RTE_Crash.hpp"
#include "RunTime/RTE_Message.hpp"
#include "RunTime/InputOutput/RTEIO_VolumeInfo.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SAPDBCommon/SAPDB_ToString.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"
#include "SAPDBCommon/Messages/SDBMsg_IOMan.h"
#include "Trace/Trace_DevSpaceIOEntry.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

static void
IOMan_WriteMessageListIntoArchive( Msg_List& msgList )
{
    // only for 7.6 as long as Msg_IOutput did not write into knldiag and knldiag.err
    RTE_Message( msgList );

    Msg_List auxMsgList;
    if( Msg_IOutput::GetDiagnosticOutput().Output( msgList, auxMsgList, Msg_IOutput::alwaysToArchive )){
        return;
    }
    auxMsgList.Clear(); // ignore error
}

/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS -lll-                                                            *
 *===========================================================================*/

SAPDB_Bool
IOMan_DataArea::AddVolume(
    RTETask_ITask&              task,
    const SAPDB_Bool            bAddVolumeOnly,
    tsp00_VFilename             &devName,
    const IOMan_BlockCount      configuredSize,
    const IOMan_DeviceNo        devNo,
    const IOMan_BlockAddress    &restartPageAddress)
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_DataArea::AddVolume", IOMan_Trace, 6 );

    SAPDBTRACE_WRITELN( IOMan_Trace, 6, "Volume: " << devName <<
                        " VolSize: " << configuredSize << " DevNo: " << devNo );

    if( ! IsDataVolumeNoValid( devNo ))
    {
        IOMan_MessageList errMsg( __CONTEXT__, IOMAN_ERR_DATA_DEVNO_OUT_OF_RANGE,
                                  SAPDB_ToString( devNo, _T_d ),
                                  SAPDB_ToString( MaxVolumes(), _T_d ));
        RTE_Message( errMsg );
        return false;
    }

    IOMan_DeviceNo firstConfiguredVolume = GetFirstConfiguredVolumeNo();

    IOMan_DataVolume&   volume      = Get( devNo );
    IOMan_DataVolume*   pPrevVolume = 0;
    IOMan_DataVolume*   pNextVolume = 0;

    // Add data volume to the control structures of the RTE, because at the
    // time of kernel restart this volume was not stored within data base
    // configuration file. The volume was made persistent by the dbmsrv after
    // starting the db kernel, therefore vcold couldn't find the volume!

    if(! volume.Add( devName, configuredSize ))
        return false;

    const SAPDB_Byte *pDBIdentifier = DetermineLinkage( volume, &pPrevVolume, &pNextVolume );

    if( 0 == pDBIdentifier )
        return false;

    IOMan_ReturnCode retCode = volume.Create( task.ID(), pDBIdentifier );  // time to format volume unlocked

    { // locked for linkage checking and modifaction
        LockedScope lock(*m_pRwLock);

        if( IOMan_Okay == retCode )
        {
            DetermineLinkage( volume, &pPrevVolume, &pNextVolume );

            if( IOMan_Okay == volume.Open( task ))
            {
                if( bAddVolumeOnly || (FBM_IManager::Instance().RegisterVolume( task.ID(),
                                       volume.GetLogicalDevNo(), volume.GetInternalSize(),
                                       volume.GetAccessMode(), restartPageAddress, kb57GetClusterReservedSpace())))
                {
                    if( 0 != pPrevVolume )
                        this->CreateLinkage( task, *pPrevVolume, volume );

                    if( 0 != pNextVolume )
                        this->CreateLinkage( task, volume, *pNextVolume );

                    m_TotalDataPages += volume.GetUsableSize();
                    m_UsedVolumes++;
                    m_ConfiguredVolumes++;

                    // At this moment the blocks of the new volume are usable.
                    // If the converter expansion failed (crash) the new volume
                    // is still valid!
                    {
                        IOMan_MessageList msgList( __CONTEXT__, IOMAN_INFO_ADD_VOLUME,
                                                   volume.GetTypeName(),
                                                   SAPDB_ToString( volume.GetLogicalDevNo(), _T_d ),
                                                   SAPDB_ToString( volume.GetConfiguredSize(), _T_d ));
                        RTE_Message( msgList );
                    }
                    // try to expand converter if data base is online to increase
                    // the number of usable pageNo's. Note that it is not fatal if
                    // the expand fails, because the converter will grow during the
                    // next restart.

                    if( ! bAddVolumeOnly ){
                        Converter_IManager::Instance().Expand( task, volume.GetUsableSize());
                    }

                    if ( firstConfiguredVolume > devNo ) // add in front of first volume -> move restart page address to first InfoPage
                    {
                        this->Get( devNo ).SetRestartPageAddress( task, restartPageAddress ); // new
                        this->Get( firstConfiguredVolume ).SetRestartPageAddress( task, IOMan_BlockAddress( 0,0 )); // invalidate
                    }

                    return true; // normal termination
                }
            }
        }
    }
    volume.Close( task.ID());
    volume.Del();
    return false;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
IOMan_DataArea::DropVolume(
    RTETask_ITask&          task,
    const IOMan_DeviceNo    volNo )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_DataArea::DropVolume", IOMan_Trace, 6 );

    SAPDBTRACE_WRITELN( IOMan_Trace, 6, "Volume: " <<  volNo );

    if( ! IsDataVolumeNoValid( volNo ))
    {
        IOMan_MessageList errMsg( __CONTEXT__, IOMAN_ERR_DATA_DEVNO_OUT_OF_RANGE,
                                  SAPDB_ToString( volNo, _T_d ),
                                  SAPDB_ToString( MaxVolumes(), _T_d ));
        RTE_Message( errMsg );
        return false;
    }
    IOMan_DataVolume  &volume = Get( volNo );

    { // locked for linkage checking and modifaction
        LockedScope lock(*m_pRwLock);

        this->DropLinkage( task, volume );

        m_TotalDataPages -= volume.GetUsableSize();
        m_UsedVolumes--;
        m_ConfiguredVolumes--;

        Converter_IManager::Instance().Shrink( task, volume.GetUsableSize());
    }
    volume.Close( task.ID());
    volume.Del();
    return true;
}

/*---------------------------------------------------------------------------*/

void
IOMan_DataArea::CloseAll( RTETask_ITask&    task )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_DataArea::CloseAll", IOMan_Trace, 6 );

    ConstIterator   endIter = End();

    for( Iterator iter = Begin(); iter != endIter; ++iter )
        ( *iter ).Close( task.ID());

    m_TotalDataPages = 0;
    m_UsedVolumes    = 0;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
IOMan_DataArea::CreateAll(
    RTETask_ITask&      task,
    const SAPDB_Byte*   pDBIdentifier )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_DataArea::CreateAll", IOMan_Trace, 6 );

    IOMan_MessageList msgList;

    if( 0 != m_TotalDataPages )
    {
        IOMan_MessageList errMsg( __CONTEXT__, IOMAN_ERR_DATA_AREA_ALREADY_ONLINE );
        RTE_Crash( errMsg );
    }

    if( ! FormatAll( task, msgList ))
    {
        msgList.AppendNewMessage( IOMan_MessageList( __CONTEXT__, IOMAN_ERR_ALL_FORMAT, "data" ));
        RTE_Message( msgList );
        return false;
    }
    { // locked for linkage checking and modifaction
        LockedScope lock(*m_pRwLock);

        Iterator        prevIter = End();
        ConstIterator   endIter  = End();

        for( Iterator iter = Begin(); iter != endIter; ++iter )
        {
            IOMan_ReturnCode retCode = ( *iter ).CreateInfoPage( task.ID(), pDBIdentifier );

            if( IOMan_Okay == retCode )
            {
                if( IOMan_Okay != (( *iter ).Open( task ))){
                    return false;
                }
                else
                {
                    if( prevIter != endIter ){
                        this->CreateLinkage( task, *prevIter, *iter );
                    }
                    prevIter          = iter;
                    m_TotalDataPages += ( *iter ).GetUsableSize();
                    m_UsedVolumes++;
                }
            }
            else if( IOMan_NotConfigured == retCode ){
                continue;
            }
            else{
                return false;
            }
        }
        SAPDBERR_ASSERT_STATE( m_ConfiguredVolumes == m_UsedVolumes );
    }
    return true;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
IOMan_DataArea::GetVolumeNo(
    tsp00_VFilename&    devName,
    IOMan_DeviceNo&     devNo ) const
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_DataArea::GetVolumeNo", IOMan_Trace, 6 );

    tsp00_VFilename     auxName;
    ConstIterator       endIter = End();

    for( ConstIterator iter = Begin(); iter != endIter; ++iter )
    {
        if(( *iter ).GetName( auxName ))
        {
            if( 0 == memcmp( devName, auxName, sizeof( devName )))
            {
                devNo = (*iter).GetLogicalDevNo();
                return true;
            }
        }
    }
    return false;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
IOMan_DataArea::OpenAll( RTETask_ITask& task )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_DataArea::OpenAll", IOMan_Trace, 6 );

    { // locked for linkage checking and modifaction
        LockedScope lock(*m_pRwLock);

        IOMan_BlockCount    totalDataPages = 0;
        SAPDB_UInt          usedVolumes    = 0;
        Iterator            prevIter       = End();
        ConstIterator       endIter        = End();

        for( Iterator iter = Begin(); iter != endIter; ++iter )
        {
            IOMan_ReturnCode retCode = ( *iter ).Open( task );

            if( IOMan_Okay == retCode )
            {
                if( prevIter == endIter )
                {
                    // iter points to the first volume within configuration file
                    if( ( *iter ).IsPrevLinkageValid())
                    {
                        IOMan_MessageList errMsg( __CONTEXT__, IOMAN_ERR_MISSING_PREDECESSOR,
                                                  ( *iter ).GetTypeName(),
                                                  SAPDB_ToString( ( *iter ).GetLogicalDevNo(), _T_d ),
                                                  SAPDB_ToString( ( *iter ).GetPrevLogicalDevNo(), _T_d ));
                        RTE_Message( errMsg );
                        return false;
                    }
                }
                else
                {
                    if( !( *prevIter ).IsLinkageValid( task.ID(), *iter )){
                        return false;
                    }
                    if( !( *prevIter ).IsDBIdentifierValid( task.ID(), *iter )){
                        return false;
                    }
                }
                prevIter        = iter;
                totalDataPages += ( *iter ).GetUsableSize();
                usedVolumes++;
            }
            else if( IOMan_NotConfigured == retCode ){
                continue;
            }
            else{
                return false;
            }
        }
        if( ( *prevIter ).IsNextLinkageValid())
        {
            IOMan_MessageList errMsg( __CONTEXT__, IOMAN_ERR_MISSING_SUCCESSOR,
                                      ( *prevIter ).GetTypeName(),
                                      SAPDB_ToString( ( *prevIter ).GetLogicalDevNo(), _T_d ),
                                      SAPDB_ToString( ( *prevIter ).GetNextLogicalDevNo(), _T_d ));
            RTE_Message( errMsg );
            return false;
        }

        SAPDBERR_ASSERT_STATE( m_ConfiguredVolumes == usedVolumes );

        // Set the data area capacity after opening all volumes
        m_TotalDataPages = totalDataPages;
        m_UsedVolumes    = usedVolumes;

    }
    return true;
}

/*---------------------------------------------------------------------------*/

IOMan_ReturnCode
IOMan_DataArea::MigrateCapacity(
    RTETask_ITask&      task,
    IOMan_MessageList&  msgList )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_DataArea::MigrateCapacity", IOMan_Trace, 6 );

    ConstIterator   endIter = End();

    for( Iterator iter = Begin(); iter != endIter; ++iter )
    {
        const IOMan_ReturnCode retCode = ( *iter ).MigrateCapacity( task.ID(), msgList );

        if(( IOMan_Okay == retCode ) || ( IOMan_NotConfigured == retCode ))
            continue;
        else
            return retCode;
    }
    return IOMan_Okay;
}

/*---------------------------------------------------------------------------*/

bool
IOMan_DataArea::Initialize(
    RTETask_ITask&        task,
    const SAPDB_Int       maxDataVolumes)
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_DataArea::Initialize", IOMan_Trace, 6 );

    if(! m_Volumes.Resize( maxDataVolumes )){
        return false;
    }
    IOMan_DeviceNo  devNo   = IOMan_DataArea::GetFirstDataVolumeNo();
    Iterator        iter    = Begin();
    ConstIterator   endIter = End();

    while( iter != endIter )
    {
        Data_PageFrame frame = m_PageAllocator.New( task.ID());

        if( ! frame.IsAssigned()){
            return false;
        }
        ( *iter ).Initialize( devNo, frame );
        // Determine the number of configured data volumes at this point
        // because same compontents are interested in this information
        // before the corresponding data volumes are started.

        if( ( *iter ).IsConfigured()){
            ++m_ConfiguredVolumes;
        }
        ++iter;
        ++devNo;
    }
    return true;
}

/*---------------------------------------------------------------------------*/

IOMan_DeviceNo
IOMan_DataArea::GetConfiguredVolumeNo( const SAPDB_Int pos ) const
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_DataArea::GetConfiguredVolume", IOMan_Trace, 6 );

    IOMan_DeviceNo  devNo   = IOMan_DataArea::GetFirstDataVolumeNo();
    ConstIterator   iter    = Begin();
    ConstIterator   endIter = End();
    SAPDB_Int       counter = 0;

    while( iter != endIter )
    {
        if( ( *iter ).IsConfigured())
        {
            counter ++;
            if (counter == pos)
            {
                return devNo;
            }
        }
        ++iter;
        ++devNo;
    }
    return IOMan_DeviceNo(); // invalid
}

/*---------------------------------------------------------------------------*/

bool
IOMan_DataArea::ReadPagesForBackup(
    RTETask_ITask&              task,
    Msg_List&                   msgList,
    IOMan_DataPages&            pages,
    const IOMan_AsynIOHandle&   asynIOHandle,
    IOMan_BlockCount&           blocksToBeBackuped,
    const bool                  bDoExtendedCheck,
    bool&                       bIsClustered )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_DataArea::ReadPagesForBackup", IOMan_Trace, 5 );

    FBM_IManager&                fbm             = FBM_IManager::Instance();
    IOMan_BlockCount             maxBlocksWanted = pages.GetPageCount();
    SAPDB_UInt                   startIndex      = 0;
    IOMan_ComplexClusterAddress  cluster;

    blocksToBeBackuped = 0;
    do
    {
        const FBM_IManager::GetBackupBlocksRc fbmRc = fbm.GetNextBlocksForBackUp( task,
                msgList, maxBlocksWanted, cluster, bIsClustered );

        if( FBM_IManager::gbbRcNoMoreBackupBlocks == fbmRc ){
            return true; // no more blocks available
        }
        if( FBM_IManager::gbbRcOkay != fbmRc ){
            break; // system error
        }
        if( ! this->AsynReadPages( task, msgList, pages, startIndex, asynIOHandle, cluster, bDoExtendedCheck )){
            break; // system error
        }
        if( bIsClustered ){
            maxBlocksWanted    = 0; // io buffer is full
            blocksToBeBackuped = pages.GetPageCount(); // complete cluster will be returned
            SAPDBERR_ASSERT_STATE( pages.GetPageCount() == cluster.GetBlockCount());
            for( SAPDB_Int blockIndex = 0; blockIndex < cluster.GetBlockCount(); ++blockIndex )
            {
                Data_BasePage&  page = pages.Get( blockIndex );
                SAPDBERR_ASSERT_STATE( page.IsAssigned());
                if( cluster.IsBlockInUse( blockIndex )){
                    continue;
                }
                // invalidate unused pages by writing 0x00 into page frame. Recovery has to
                // know which page from the backup file is in use and which is only for padding
                SAPDB_MemFillNoCheck( page.GetFramePtr(), 0x00, page.GetSize());
            }
            return true; // io buffer filled completely
        }
        const IOMan_BlockCount usedBlocks = cluster.GetUsedBlocks();
        maxBlocksWanted    -= usedBlocks;
        blocksToBeBackuped += usedBlocks;
        if( usedBlocks != cluster.GetBlockCount()){
            this->RemoveUnusedPages( pages, startIndex, cluster ); // read cluster contains padding blocks
        }
        startIndex = blocksToBeBackuped;
    }while( maxBlocksWanted > 0 );

    if( maxBlocksWanted <= 0 ){ // io buffer filled completely
        return true;
    }
    return false; // system error
}

/*---------------------------------------------------------------------------*/

bool
IOMan_DataArea::WritePagesForRestore(
    RTETask_ITask&              task,
    Msg_List&                   msgList,
    IOMan_DataPages&            pages,
    const IOMan_AsynIOHandle&   asynIOHandle,
    const bool                  bDoExtendedCheck,
    const bool                  bIsClustered )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_DataArea::WritePagesForRestore", IOMan_Trace, 5 );

    FBM_IManager&           fbm        = FBM_IManager::Instance();
    SAPDB_UInt              startIndex = 0;
    IOMan_ClusterAddress    cluster;
    IOMan_BlockCount        blocksToBeRestored  =
        ( bIsClustered ) ? pages.GetPageCount() : this->DetermineUsageCount( pages );

    SAPDBTRACE_WRITELN( IOMan_Trace, 5, "pageCount: " << pages.GetPageCount()
                        <<  " blocksToBeRestored: " << blocksToBeRestored
                        <<  (bIsClustered ? "(clustered)":"( not clustered)"));

    while( blocksToBeRestored > 0 )
    {
        if ( bIsClustered )
        {
            if( ! fbm.GetFreeCluster( task, msgList, blocksToBeRestored, cluster )){
                break; // system error
            }
        }
        else
        {            
            if( ! fbm.GetMultFreeBlocks_2( task, msgList, blocksToBeRestored, cluster )){
                break; // system error
            }
        }
        if( ! this->AsynWritePages( task, msgList, fbm, pages, startIndex, asynIOHandle, cluster,
                                    bIsClustered, bDoExtendedCheck )){
            break; // system error
        }
        blocksToBeRestored -= cluster.GetBlockCount();
        startIndex         += cluster.GetBlockCount();
    }
    if( blocksToBeRestored <= 0 ){ // io buffer completely restored
        return true;
    }
    return false; // system error
}

/*===========================================================================*
 *  PRIVATE METHODS                                                          *
 *===========================================================================*/

bool
IOMan_DataArea::AsynReadPages(
    RTETask_ITask&                      task,
    Msg_List&                           msgList,
    IOMan_DataPages&                    pages,
    const SAPDB_UInt                    startIndex,
    const IOMan_AsynIOHandle&           asynIOHandle,
    const IOMan_ComplexClusterAddress&  cluster,
    const bool                          bDoExtendedCheck ) // for future use
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_DataArea::AsynReadPages", IOMan_Trace, 7 );
    SAPDBERR_ASSERT_STATE( startIndex + SAPDB_UInt( cluster.GetBlockCount()) <= pages.GetPageCount());

    const SAPDB_Int4    asynID = asynIOHandle.GetId( cluster.GetVolumeNo());
    IOMan_DataVolume&   volume = this->Get( cluster.GetVolumeNo());

    IOMan_ClusterAddress::BlockIndex   blockIndex = 0;
    IOMan_DataPagesIterator            iter( pages );
    SAPDB_UInt                         badBlockIndex = 0;
    SAPDB_UInt                         readCount     = 1;
    bool                               bOkay         = false;

    for( readCount = 1; IOMan_Volume::MaxReadRetries() >= readCount; ++readCount )
    {
        const IOMan_DataVolume::AsynReadPagesRc ioRc = volume.AsynReadPages(
                    task, msgList, asynID, pages, startIndex, cluster );

        if( IOMan_DataVolume::arpRcOkay != ioRc )
        {
            if( IOMan_DataVolume::arpRcOffline != ioRc ){
                return false; // fatal error
            }
            msgList = Msg_List( Msg_List::Error, SDBMSG_IOMAN_VOLUME_OFFLINE,
                                Msg_Arg(SDBMSGTAG_IOMAN_VOLUME_OFFLINE__VOLUME_TYPE, volume.GetTypeName()),
                                Msg_Arg(SDBMSGTAG_IOMAN_VOLUME_OFFLINE__VOLUME_ID,   volume.GetLogicalDevNo()));
            msgList.AddDetail( SDBMSG_IOMAN_DATA_AREA_STATE,
                               Msg_Arg(SDBMSGTAG_IOMAN_DATA_AREA_STATE__NUMBER,  ConfiguredVolumes()),
                               Msg_Arg(SDBMSGTAG_IOMAN_DATA_AREA_STATE__NUMBER1, TotalUsableDataPages()),
                               Msg_Arg(SDBMSGTAG_IOMAN_DATA_AREA_STATE__NUMBER2, UsedVolumes())
                             );
            return false; // make offline volume to fatal error
        }
        if( RemoveFromBackUpList( task, msgList, pages, startIndex, cluster, badBlockIndex )){
            bOkay = true;
            break; // pages successfully read and removed from backup pageNo list and FBM
        }
    }
    if( ! bOkay )
    {
        msgList.Overrule( Msg_List( Msg_List::Error, SDBMSG_IOMAN_INCONSISTENT_BACKUP,
                                    Msg_Arg(SDBMSGTAG_IOMAN_INCONSISTENT_BACKUP__VOLUME_ID, cluster.GetVolumeNo()),
                                    Msg_Arg(SDBMSGTAG_IOMAN_INCONSISTENT_BACKUP__BLOCK_NO, cluster.GetBlockNo(badBlockIndex) )));
        return false; // pageNo not in backup pageNo list
    }
    if( readCount > 1 )
    { // it takes more than one attempt to get the right images of the requested pages but NOW the pages are correct
        msgList.Overrule( Msg_List( Msg_List::Warning, SDBMSG_IOMAN_MULTIPLE_FAILED_READS,
                                    Msg_Arg(SDBMSGTAG_IOMAN_MULTIPLE_FAILED_READS__NUMBER,      cluster.GetBlockCount()),
                                    Msg_Arg(SDBMSGTAG_IOMAN_MULTIPLE_FAILED_READS__VOLUME_TYPE, volume.GetTypeName()),
                                    Msg_Arg(SDBMSGTAG_IOMAN_MULTIPLE_FAILED_READS__VOLUME_ID,   volume.GetLogicalDevNo()),
                                    Msg_Arg(SDBMSGTAG_IOMAN_MULTIPLE_FAILED_READS__BLOCK_NO,    cluster.GetBlockNo( 0 )),
                                    Msg_Arg(SDBMSGTAG_IOMAN_MULTIPLE_FAILED_READS__NUMBER1,     readCount ),
                                    Msg_OptArg(SDBMSGTAG_IOMAN_MULTIPLE_FAILED_READS__BLOCK_NO_1, badBlockIndex )));
        IOMan_WriteMessageListIntoArchive( msgList );
        msgList.Clear();
    }
    if( g01vtrace.vtrAll_gg00 || g01vtrace.vtrIoTrace_gg00 )
    {
        SAPDB_UInt  traceNo = ( cluster.GetBlockCount() > 1 ) ? 1:0;
        for( blockIndex = 0, iter.Begin( startIndex ); blockIndex < cluster.GetBlockCount();
                ++iter, ++blockIndex, ++traceNo )
        {
            if( cluster.IsBlockInUse( blockIndex )){
                Trace_ReadDataPageEntry( ( *iter ).ReadPageType2(), ( *iter ).PageNo(),
                                         cluster.GetBlockAddress( blockIndex ), traceNo );
            }
        }
    }
    return true;
}
/*---------------------------------------------------------------------------*/

bool
IOMan_DataArea::AsynWritePages(
    RTETask_ITask&              task,
    Msg_List&                   msgList,
    FBM_IManager&               fbm,
    IOMan_DataPages&            pages,
    const SAPDB_UInt            startIndex,
    const IOMan_AsynIOHandle&   asynIOHandle,
    const IOMan_ClusterAddress& cluster,
    const bool                  bIsClustered,
    const bool                  bDoExtendedCheck ) // for future use
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_DataArea::AsynWritePages", IOMan_Trace, 7 );
    SAPDBERR_ASSERT_STATE( startIndex + SAPDB_UInt( cluster.GetBlockCount()) <= pages.GetPageCount());

    const SAPDB_Int4    asynID = asynIOHandle.GetId( cluster.GetVolumeNo());
    IOMan_DataVolume&   volume = this->Get( cluster.GetVolumeNo());

    IOMan_ComplexClusterAddress        ioCluster;
    IOMan_ClusterAddress::BlockIndex   blockIndex = 0;
    IOMan_DataPagesIterator            iter( pages );

    ioCluster.SetCluster( cluster );
    
    if( bIsClustered ){
        ioCluster.SetBlockCount(0);  // reset block count
        // find padding pages in the given cluster and mark them as unused in the ioCluster
        for(  blockIndex = 0, iter.Begin( startIndex ); blockIndex < cluster.GetBlockCount();
                ++iter, ++blockIndex )
        {
            SAPDBERR_ASSERT_STATE(( *iter ).IsAssigned());
            const bool bBlockIsInUse = (Kernel_Page::Kernel_InvalidPageType != ( *iter ).ReadPageType());
            ioCluster.Append( bBlockIsInUse );
        }
    }
    SAPDBERR_ASSERT_STATE( ioCluster.GetBlockCount() == cluster.GetBlockCount());

    SAPDB_UInt badPageIndex = 0;
    const IOMan_DataVolume::AsynWritePagesRc ioRc = volume.AsynWritePages(
                task, msgList, asynID, pages, startIndex, ioCluster, badPageIndex );

    if( IOMan_DataVolume::awpRcOkay != ioRc )
    {
        if( IOMan_DataVolume::awpRcOffline == ioRc )
        {
            msgList = Msg_List( Msg_List::Error, SDBMSG_IOMAN_VOLUME_OFFLINE,
                                Msg_Arg(SDBMSGTAG_IOMAN_VOLUME_OFFLINE__VOLUME_TYPE, volume.GetTypeName()),
                                Msg_Arg(SDBMSGTAG_IOMAN_VOLUME_OFFLINE__VOLUME_ID,   volume.GetLogicalDevNo()));
            msgList.AddDetail( SDBMSG_IOMAN_DATA_AREA_STATE,
                               Msg_Arg(SDBMSGTAG_IOMAN_DATA_AREA_STATE__NUMBER,  ConfiguredVolumes()),
                               Msg_Arg(SDBMSGTAG_IOMAN_DATA_AREA_STATE__NUMBER1, TotalUsableDataPages()),
                               Msg_Arg(SDBMSGTAG_IOMAN_DATA_AREA_STATE__NUMBER2, UsedVolumes()));
        }
        if( IOMan_DataVolume::awpRcIoCheckFailed == ioRc )
        {
            const Data_BasePage&        page    = pages.Get( badPageIndex );
            const Data_PageRecoveryMode recMode = (gg04IsStaticPage( page.ReadPageType2())?
                                                   Data_PageRecoveryMode( Data_Static, Data_Recoverable ) : Data_PageRecoveryMode());
            msgList.AddDetail( SDBMSG_IOMAN_PAGENO_DETAIL,
                               Msg_Arg(SDBMSGTAG_IOMAN_PAGENO_DETAIL__PAGENO, page.PageNo()),
                               Msg_Arg(SDBMSGTAG_IOMAN_PAGENO_DETAIL__PERSISTENT_TYPE, SAPDB_ToString( recMode )));
        }
        return false;
    }
    Converter_IBackUp& iConv = Converter_IBackUp::Instance();

    for(  blockIndex = 0, iter.Begin( startIndex ); blockIndex < ioCluster.GetBlockCount(); ++iter, ++blockIndex )
    {
        SAPDBERR_ASSERT_STATE(( *iter ).IsAssigned());
        const Data_PageNo          pageNo   = ( *iter ).PageNo();
        const IOMan_BlockAddress   block    = ioCluster.GetBlockAddress( blockIndex );
        
        if (ioCluster.IsBlockInUse(blockIndex))
        {
            // do not use recovery mode from page handle, because this could not be initialized correct before!
            const Data_AddressingMode  addrMode =
                (gg04IsStaticPage((*iter).ReadPageType2()) ? Data_Static : Data_Dynamic );

            if( ! iConv.SetBlockAddressOfRestoredPage( task, msgList, block, pageNo, addrMode )){
                return false;
            }
        }
        else
        {
            fbm.SetBlockStateToFree(task.ID(), block);
        }
    }
    if( g01vtrace.vtrAll_gg00 || g01vtrace.vtrIoTrace_gg00 )
    {
        SAPDB_UInt  traceNo = ( ioCluster.GetBlockCount() > 1 ) ? 1:0;
        for( blockIndex = 0, iter.Begin( startIndex ); blockIndex < ioCluster.GetBlockCount();
                ++iter, ++blockIndex, ++traceNo )
        {
            Trace_ReadDataPageEntry( ( *iter ).ReadPageType2(), ( *iter ).PageNo(),
                                     ioCluster.GetBlockAddress( blockIndex ), traceNo );
        }
    }
    return true;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
IOMan_DataArea::FormatAll(
    RTETask_ITask&      task,
    IOMan_MessageList&  msgList )
{
    RTEIO_VolumeInfoVector  infoVector( m_Allocator );

    if( ! infoVector.Reserve( m_ConfiguredVolumes )){
        return false;
    }
    ConstIterator   iter;
    ConstIterator   endIter = End();

    for( iter = Begin(); iter != endIter; ++iter )
    {
        if(( *iter ).IsConfigured())
        {
            infoVector.InsertEnd( new (m_Allocator) RTEIO_DataVolumeInfo(
                                      (*iter).GetLogicalDevNo(),
                                      (*iter).GetInternalSize()));
        }
    }
    // format all volumes
    if ( ! RTEIO_FormatVolumeVector( task.ID(), infoVector, msgList )){
        return false;
    }
    return true;
}

/*---------------------------------------------------------------------------*/

void
IOMan_DataArea::CreateLinkage(
    RTETask_ITask&      task,
    IOMan_DataVolume&   prevVolume,
    IOMan_DataVolume&   volume )
{
    if( prevVolume.SetNextLinkage( task.ID(), volume ))
        if( volume.SetPrevLinkage( task.ID(), prevVolume ))
            return;

    IOMan_MessageList errMsg( __CONTEXT__, IOMAN_ERR_CREATE_LINKAGE,
                              volume.GetTypeName(),
                              SAPDB_ToString( prevVolume.GetLogicalDevNo(), _T_d ),
                              SAPDB_ToString( volume.GetLogicalDevNo(), _T_d ));
    RTE_Crash( errMsg );
}

/*---------------------------------------------------------------------------*/

void
IOMan_DataArea::DropLinkage(
    RTETask_ITask&      task,
    IOMan_DataVolume&   volume )
{
    IOMan_DataVolume*   pPrevVolume = 0;
    IOMan_DataVolume*   pNextVolume = 0;

    this->DetermineLinkage( volume, &pPrevVolume, &pNextVolume );

    if( 0 != pPrevVolume )
    {
        if( 0 == pNextVolume )
            pPrevVolume->SetNextLinkageToInvalid( task );
        else
            pPrevVolume->SetNextLinkage( task.ID(), *pNextVolume );
    }
    if( 0 != pNextVolume )
    {
        if( 0 == pPrevVolume )
            pNextVolume->SetPrevLinkageToInvalid( task );
        else
            pNextVolume->SetPrevLinkage( task.ID(), *pPrevVolume );
    }
}

/*---------------------------------------------------------------------------*/

const SAPDB_Byte*
IOMan_DataArea::DetermineLinkage(
    const IOMan_DataVolume& volume,
    IOMan_DataVolume**      pPrevVolume,
    IOMan_DataVolume**      pNextVolume )
{
    ConstIterator   endIter = End();

    for( Iterator iter = Begin(); iter != endIter; ++iter )
    {
        if(( *iter ).IsOnline())
        {
            if(( *iter ).GetLogicalDevNo() < volume.GetLogicalDevNo())
                *pPrevVolume = iter;
            else
                if(( *iter ).GetLogicalDevNo() > volume.GetLogicalDevNo())
                {
                    *pNextVolume = iter;
                    break;
                }
        }
    }
    if( 0 != *pPrevVolume )
        return( **pPrevVolume ).GetDBIdentifier();
    else if( 0 != *pNextVolume )
        return( **pNextVolume ).GetDBIdentifier();
    else
        return 0;
}

/*---------------------------------------------------------------------------*/

IOMan_BlockCount
IOMan_DataArea::DetermineUsageCount( const IOMan_DataPages&  pages )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_DataArea::DetermineUsageCount", IOMan_Trace, 7 );

    IOMan_BlockCount pageCount = pages.GetPageCount();

    if( 0 == pageCount ){
        return 0;
    }
    SAPDB_Int  pageIndex = pageCount + ( pages.FirstIndex() -1 ); // set to last page
    for( ; 0 <= pageIndex; --pageIndex, --pageCount ) // remove trailing padding pages
    {
        const Data_BasePage&    page = pages.Get( pageIndex );
        SAPDBERR_ASSERT_STATE( page.IsAssigned());
        if( Kernel_Page::Kernel_InvalidPageType != page.ReadPageType()){
            return pageCount;
        }
    }
    return 0;
}

/*---------------------------------------------------------------------------*/

bool
IOMan_DataArea::RemoveFromBackUpList(
    RTETask_ITask&                      task,
    Msg_List&                           msgList,
    IOMan_DataPages&                    pages,
    const SAPDB_UInt                    startIndex,
    const IOMan_ComplexClusterAddress&  cluster,
    SAPDB_UInt&                         badBlockIndex )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_DataArea::RemoveFromBackUpList", IOMan_Trace, 7 );

    Converter_IBackUp&                 iConv = Converter_IBackUp::Instance();
    IOMan_ClusterAddress::BlockIndex   blockIndex = 0;
    IOMan_DataPagesIterator            iter( pages );

    for(  blockIndex = 0, iter.Begin( startIndex ); blockIndex < cluster.GetBlockCount(); ++iter, ++blockIndex )
    {
        if( ! cluster.IsBlockInUse( blockIndex )){
            continue;
        }
        SAPDBERR_ASSERT_STATE(( *iter ).IsAssigned());
        const Data_PageNo          pageNo   = ( *iter ).PageNo();
        const IOMan_BlockAddress   block    = cluster.GetBlockAddress( blockIndex );
        // do not use recovery mode from page handle, because this could not be initialized correct before!
        const Data_AddressingMode  addrMode =
            (gg04IsStaticPage((*iter).ReadPageType2()) ? Data_Static:Data_Dynamic );

        if( ! iConv.ResetDataPageAfterBackUp( task, msgList, pageNo, addrMode )){
            badBlockIndex = blockIndex;
            return false; // try again
        }
    }
    FBM_IManager::Instance().RestoreBlockStateMarkedForBackup( task, cluster );
    return true;
}

/*---------------------------------------------------------------------------*/

void
IOMan_DataArea::RemoveUnusedPages(
    IOMan_DataPages&                    pages,
    const SAPDB_UInt                    startIndex,
    const IOMan_ComplexClusterAddress&  cluster )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_DataArea::RemoveUnusedPages", IOMan_Trace, 7 );

    SAPDBTRACE_WRITELN( IOMan_Trace, 7, "cluster: " << cluster.GetVolumeNo() << "/"
                        << cluster.GetBlockNo( 0 ) << "(" << cluster.GetBlockCount() << ")"
                        << " pages from idx " << startIndex << " to " << pages.GetPageCount()-1
                        << " padding pages " << cluster.GetUsedBlocks());

    SAPDBERR_ASSERT_STATE( startIndex + SAPDB_UInt( cluster.GetBlockCount()) <= pages.GetPageCount());
    SAPDBERR_ASSERT_STATE( cluster.GetUsedBlocks() != cluster.GetBlockCount());

    SAPDB_UInt  lowerIndex = 0;
    SAPDB_UInt  upperIndex = cluster.GetBlockCount()-1;

    while( lowerIndex < upperIndex )
    {
        // search lower index which points to a padding page
        while( cluster.IsBlockInUse( lowerIndex )){
            ++lowerIndex;
        }
        // search for a upper index which points to a used page which could be copied into padding page
        while( ! cluster.IsBlockInUse( upperIndex )){
            --upperIndex;
        }
        if( lowerIndex < upperIndex )
        {
            Data_BasePage&  src  = pages.Get( upperIndex + startIndex );
            Data_BasePage&  dest = pages.Get( lowerIndex + startIndex );

            SAPDBTRACE_WRITELN( IOMan_Trace, 7, "copy page from idx " << upperIndex + startIndex
                                << " to " << lowerIndex + startIndex );

            SAPDBERR_ASSERT_STATE( src.IsAssigned());
            SAPDBERR_ASSERT_STATE( dest.IsAssigned());

            SAPDB_MemCopyNoCheck( dest.GetFramePtr(), src.GetFramePtr(), src.GetSize());
        }
    }
}

/*===========================================================================*
*  END OF CODE                                                              *
*===========================================================================*/
