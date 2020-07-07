/*!
  @file     Converter_Wrapper.cpp
  @author   TorstenS
  @author   AlexanderK
  @ingroup  Converter
  @brief    Wrapper of the Converter
*/

/*
  ========== licence begin  GPL
  Copyright (c) 2001-2004 SAP AG

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
  ========== licence end
*/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg00.h"
#include "gbd00.h"
#include "gkb00.h"
#include "hbd10.h"

#include "Converter/Converter_Converter.hpp"
#include "Converter/Converter_DBSpaceStatistic.hpp"
#include "Converter/Converter_Exception.hpp"
#include "Converter/Converter_Messages.hpp"
#include "IOManager/IOMan_BlockAddress.hpp"
#include "DataAccess/Data_Types.hpp"
#include "KernelCommon/Kernel_DateTime.hpp"
#include "KernelCommon/Kernel_SnapShot.hpp"
#include "Transaction/Trans_Context.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"


/*===========================================================================*
 *  LOCAL FUNCTIONS                                                          *
 *===========================================================================*/

/*===========================================================================*
 *  GLOBAL FUNCTIONS                                                         *
 *===========================================================================*/

/*---------------------------------------------------------------------------*/

/*!
       @brief          Creates converter for restarting an existing database
       @param          TaskId [in] own TaskId
       @param          ConverterVersion [in] converter version from restart record
       @param          NumAllPages [in] maximum number of pages which can be stored in the data base. this
                       corresponds to the sum of size of all devices
       @param          IncrementConvVersion [in] indicates whether version must be
                       incremented after the restart
       @param          LastSaveDataSuccessful [in] indicates whether the last save data finished successfully
       @param          RestartParam [in] aggregation of restart parameters
       @return         none

       - The internal structures of the converter are created.
       - Converter pages are read from devices. The converter root
         page can be found at address 'rootaddr'.
       - The parameters 'MaxDynamicPageNo', 'MaxStaticPageNo' and  'NumAllPages'
         are used to determine the initial size of the  converter.
       - 'ConverterVersion' is the converter version of the database to be
         recovered. If 'bIncConverterVersion' is true, the converter version
         is incremented after a successful restart.
*/
externPascal void b10rc_restart_converter (
    tsp00_TaskId                TaskId,
    tgg00_BasisError            &TrError,
    tsp00_Int4                  ConverterVersion,
    tsp00_Int4                  NumAllPages,
    pasbool                     IncrementConvVersion,
    pasbool                     LastSaveDataSuccessful,
    tkb00_ConfigRestartParam    &RestartParam)
{
    const RTETask_ITask             &task  = *RTETask_ITask::ByTaskID( TaskId );
    const IOMan_PackedBlockAddress  packedRootBlockAddr( RestartParam.crConvRootBlockAddr_kb00 );

    // if a converter snapshot exist the values for the snapShot
    // converter version and the root of the snapshot converter
    // tree is stored within the frozen restart page

    IOMan_PackedBlockAddress  packedSnapShotRootBlockAddr;
    Converter_Version         snapShotVersion;
    Kernel_DateTime           snapShotCreationTimeStamp;
    Kernel_SnapShot           snapShot;

    snapShot.MarkSnapShotRestartPageAsOccupied( task, RestartParam, 
        packedSnapShotRootBlockAddr, snapShotVersion, snapShotCreationTimeStamp );

    const Converter_ReturnCode retCode =
        Converter_IManager::Instance().Restart( task, ConverterVersion, packedRootBlockAddr,
                                                snapShotVersion, packedSnapShotRootBlockAddr,
                                                snapShotCreationTimeStamp, IncrementConvVersion,
                                                LastSaveDataSuccessful,
                                                RestartParam.crMaxDynamicPno_kb00,
                                                RestartParam.crMaxStaticPno_kb00, NumAllPages,
                                                RestartParam.crVolumeBitCount_kb00 );
    if( retCode != Converter_Ok ){
        TrError = e_wrong_database_mode;
    }
}

/*---------------------------------------------------------------------------*/
/*!
   @brief          Restart the converter for restore pages.
   @param          Trans [in] own Trans Context
   @param          ConverterVersion [in] converter version from restart record
   @param          NumAllPages [in] maximum number of pages which can be stored in the data base. this
                   corresponds to the sum of size of all devices
   @param          bLastSaveDataSuccessful [in] indicates whether the last save data finished successfully
   @param          RestartParam [in] aggregation of restart parameters
   @param          OldRestartParam [in] aggregation of old restart parameters
   @return         none

   - The internal structures of the converter are restarted
   - Converter pages are read from devices. The converter root
     page can be found at address 'rootaddr'.
   - The parameters 'MaxDynamicPageNo', 'MaxStaticPageNo' and
     'NumAllPages' are used to determine the initial size of
     the converter.
   - 'ConverterVersion' is the converter version of the database
     to be recovered. If 'bIncConverterVersion' is true, the converter
     version is incremented after a successful restart.
 */
externC void
b10restart_converter_for_restore (
    tgg00_TransContext        &Trans,
    tsp00_Int4                 ConverterVersion,
    tsp00_Int4                 NumAllPages,
    tsp00_Bool                 bLastSaveDataSuccessful,
    tkb00_ConfigRestartParam  &RestartParam,
    tkb00_ConfigRestartParam  &OldRestartParam)
{
    const RTETask_ITask            &task  = *RTETask_ITask::ByTaskID( Trans.trTaskId_gg00 );
    const IOMan_PackedBlockAddress packedRootBlockAddr (OldRestartParam.crConvRootBlockAddr_kb00);

    IOMan_PackedBlockAddress    packedSnapShotRootBlockAddr;
    Converter_Version           snapShotVersion;
    Kernel_DateTime             snapShotCreationTimeStamp;
    Kernel_SnapShot             snapShot;

    snapShot.MarkSnapShotRestartPageAsOccupied( task, OldRestartParam, 
        packedSnapShotRootBlockAddr, snapShotVersion, snapShotCreationTimeStamp );

    const Converter_ReturnCode retCode =
        Converter_IManager::Instance().RestartForRestore( task, ConverterVersion,
                packedRootBlockAddr, snapShotVersion, packedSnapShotRootBlockAddr, snapShotCreationTimeStamp,
                bLastSaveDataSuccessful, SAPDB_MAX (RestartParam.crMaxDynamicPno_kb00, OldRestartParam.crMaxDynamicPno_kb00),
                SAPDB_MAX (RestartParam.crMaxStaticPno_kb00,  OldRestartParam.crMaxStaticPno_kb00),
                NumAllPages, OldRestartParam.crVolumeBitCount_kb00 );

    if( retCode != Converter_Ok ){
        Trans.trError_gg00 = e_wrong_database_mode;
    }

    Converter_IBackUp::Instance().BeginRestoreUsedPageNos();
}

/*---------------------------------------------------------------------------*/

/*!
   @brief          Creates an empty converter for a new data base (needed for activate serverdb)
   @param          TaskId [in] task id
   @param          NumAllPages [in] maximum number of pages which can be stored in the data base.
                   this corresponds to the sum of size of all devices

   - The internal structures of the converter are created.
   - The parameter 'NumAllPages' is used to determine the initial size of the  converter.
*/
externC void
b10create_converter (
    tsp00_TaskId   TaskId,
    tsp00_Int4     NumAllPages )
{
    const RTETask_ITask &task  = *RTETask_ITask::ByTaskID( TaskId );
    Converter_IManager::Instance().Create( task, NumAllPages );
}

/*---------------------------------------------------------------------------*/

/*!
   @brief          Creates an empty converter for restoring existing database
   @param          TaskId [in] task id
   @param          ConverterVersion [in] converter version from restart record
   @param          NumAllPages [in] maximum number of pages which can be stored in the data base. this
                   corresponds to the sum of size of all device
   @param          bLastSaveDataSuccessful [in] indicates whether the last save data finished successfully
   @param          RestartParam [in] aggregation of restart parameters
   @return         none

   - The internal structures of the converter are created and initialized.
   - The parameters 'MaxDynamicPageNo', 'MaxStaticPageNo' and 'NumAllPages'
     are used to determine the initial size of the  converter.
   - 'ConverterVersion' is the converter version of the database to be recovered.
 */
externC void
b10create_converter_for_restore (
    tsp00_TaskId               TaskId,
    tsp00_Int4                 ConverterVersion,
    tsp00_Int4                 NumAllPages,
    tsp00_Bool                 bLastSaveDataSuccessful,
    tkb00_ConfigRestartParam  &RestartParam)
{
    const RTETask_ITask &task  = *RTETask_ITask::ByTaskID( TaskId );

    Converter_IManager::Instance().CreateForRestore( task, ConverterVersion,
        bLastSaveDataSuccessful, RestartParam.crMaxDynamicPno_kb00,
        RestartParam.crMaxStaticPno_kb00, NumAllPages );
}

/*---------------------------------------------------------------------------*/

/*!
   @brief          Finishes converter verification in "cold" database state
   @param          taskId [in] task id
   @param          NumPermPages [out] number of permanent pages used
   @param          NumReleasesBlocks [out] number of released blocks, which were
                   not accessed during verify
*/
externC void
bd10EndColdVerify (tsp00_TaskId taskId,
                   tsp00_Int4   &NumPermPages,
                   tsp00_Int4   &NumReleasesBlocks)
{
    const RTETask_ITask &task = *RTETask_ITask::ByTaskID( taskId );

    Data_PageCount       NumStaticPermDataPages  = 0;
    Data_PageCount       NumDynamicPermDataPages = 0;
    Data_PageCount       NumPermConvPages        = 0;
    Data_PageCount       ReleasedBlocks          = 0;

    Converter_IVerify::Instance().EndCheckData (
        task,
        NumStaticPermDataPages,
        NumDynamicPermDataPages,
        NumPermConvPages,
        ReleasedBlocks);

    NumPermPages = NumStaticPermDataPages + NumDynamicPermDataPages + NumPermConvPages;
    NumReleasesBlocks = ReleasedBlocks;
}

/*---------------------------------------------------------------------------*/

/*!
   @brief          Finishes a failed converter verification
   @param          TaskId [in] task id
   @return         none

   - The converter is restored to the state it had before the verification
     was started by the BeginColdVerify
*/
externC void
bd10EndFailedColdVerify (tsp00_TaskId  TaskId)
{
    const RTETask_ITask &task = *RTETask_ITask::ByTaskID( TaskId );

    Converter_IVerify::Instance().EndFailedCheckData( task );
}

/*---------------------------------------------------------------------------*/

/*!
@brief          Ends the backup of data or pages
@param          TaskId [in] current task id
@param          bBackUpSuccessfullyFinished [in] indicates whether the current back
                up finished successfully
@return         none

- The back up active flag in the converter is reset
- If  the current back up  did not finish successfully all blocks still marked
  as to be written to the back up files are unmarked.
*/
externC void
bd10EndSave (tsp00_TaskId    TaskId,
             tsp00_Bool      bBackUpSuccessfullyFinished)
{
    const RTETask_ITask &task = *RTETask_ITask::ByTaskID( TaskId );

    Converter_IBackUp::Instance().EndSave( task, bBackUpSuccessfullyFinished );
}

/*---------------------------------------------------------------------------*/

/*!
@brief          Returns the number of of pages marked for back up
@return         Number of blocks marked for back up
*/
externC tsp00_Int4
b10bup_data_page_cnt (tsp00_TaskId TaskId)
{
    return Converter_IBackUp::Instance().GetNumPagesForBackUp();
}

/*---------------------------------------------------------------------------*/

/*!
@brief          Returns the number of converter pages required to address all
                static data pages
@return         Number of converter pages required to address all
                data pages which belong to the current backup

- Returns the number of converter pages required to address all
  data pages which belong to the current backup
*/
externC tsp00_Int4
bd10conv_page_cnt ()
{
    return Converter_IBackUp::Instance().GetNumConvPagesForBackUp();
}

/*---------------------------------------------------------------------------*/

/*!
   @brief          Returns the actual converter version
   @return         Converter_Version
 */
externPascal tsp00_Int4
bd10GetConverterVersion ()
{
    return Converter_ICommon::Instance().Version ();
}

/*---------------------------------------------------------------------------*/

/*!
   @brief          Marks a page in the converter as  used
   @param          Trans [in] trans context
   @param          PageNo [in] page number to be marked as used
   @return         none
 */
externC void
b10use_pno (tgg00_TransContext    &Trans,
            tsp00_PageNo           PageNo)
{
    const RTETask_ITask &task = *RTETask_ITask::ByTaskID( Trans.trTaskId_gg00 );

    Converter_IVerify::Instance().MarkPageNoAsUsed( task, PageNo, Data_Dynamic );
}

/*---------------------------------------------------------------------------*/

/*!
   @brief          Saves converter to devspace (sequential writing)
   @param          Trans [in] current trans context
   @param          ConverterVersion [out] new valid converter version
   @param          MaxStaticPageNo [out] maximun used static page number. This number has
                   to be stored in the restart page in order the calculate the
                   right size of the converter (static map) after a restart.
   @param          MaxDynamicPageNo [out] maximun used dynamic page number. This number has
                   to be stored in the restart page in order the calculate the
                   right size of the converter (dynamic map) after a restart.
   @param          PackedConvRootBlockAddress [out] Block address of the converter root
                   page (to be stored in the restart record
   @return         none
*/
externC void
bd10FlushConverterSerial (tgg00_TransContext  &Trans,
                          tsp00_Int4          &ConverterVersion,
                          tsp00_PageNo        &MaxStaticPageNo,
                          tsp00_PageNo        &MaxDynamicPageNo,
                          tsp00_Int4          &PackedConvRootBlockAddress)
{
    const RTETask_ITask &task = *RTETask_ITask::ByTaskID( Trans.trTaskId_gg00 );

    Converter_Version           _ConverterVersion;
    Data_PageNo                 _MaxStaticPageNo;
    Data_PageNo                 _MaxDynamicPageNo;
    IOMan_PackedBlockAddress    _packedRootBlockAddress;

    if( ! Converter_IManager::Instance().FlushSerial (task, _ConverterVersion,
            _MaxStaticPageNo, _MaxDynamicPageNo, _packedRootBlockAddress ))
    {
        Trans.trError_gg00 = e_shutdown;
    }

    ConverterVersion           = _ConverterVersion;
    MaxStaticPageNo            = _MaxStaticPageNo;
    MaxDynamicPageNo           = _MaxDynamicPageNo;
    PackedConvRootBlockAddress = _packedRootBlockAddress;
}

/*---------------------------------------------------------------------------*/

/*!
   @brief          Prepares converter verification in "cold" database state
   @return         none
 */
externC void
bd10BeginColdVerify ( tgg00_TransContext  &trans )
{
    Trans_Context &transContext = Trans_Context::GetContext( trans );
    
    const Converter_ReturnCode retCode =
        Converter_IVerify::Instance().BeginCheckData ( 
        transContext.Task(), transContext.ErrorList());
    
    switch( retCode )
    {
    case Converter_Ok:
        return;
    case Converter_MemoryAllocationFailed:
        trans.trError_gg00 = e_no_more_memory;
        return;
    default:
        trans.trError_gg00 = e_system_error;
    }
}

/*---------------------------------------------------------------------------*/

/*!
   @brief          Shuts the converter down
   @param          TaskId [in,out] task id
   @return         none

   - All resources held by the converter are released and all
     member data are reseted to their initial values
   - All tasks waiting for the converter are resumed.
 */
externC void
bd10ShutdownConverter (tsp00_TaskId  TaskId)
{
    const RTETask_ITask &task = *RTETask_ITask::ByTaskID( TaskId );

    Converter_IManager::Instance().Shutdown( task );
}

/*---------------------------------------------------------------------------*/

/*!
@brief          returns a number of data describing the filling state of the converter
@param          Trans [in] trans context
@param          DbSpaceInfo [out] collection of state data of the converter return value: none
*/
externC void
bd10dbspace_statistic (tgg00_TransContext &Trans,
                       tgg00_DbSpaceInfo  &DbSpaceInfo)
{
    const RTETask_ITask         &task = *RTETask_ITask::ByTaskID( Trans.trTaskId_gg00 );
    Converter_DBSpaceStatistic  dbSpaceStatistic( &DbSpaceInfo );

    Converter_ICommon::Instance().GetDbSpaceStatistics ( task, dbSpaceStatistic );
}

/*---------------------------------------------------------------------------*/

/*!
   @brief
   @param          TaskId [in] own Task ID
   @param          CopyPage [out] container to return copied page to caller
   @param          bNoMorePages [out] pages are available
   @return         none
*/
externC void
bd10GetConverterArchivePage(
    tsp00_TaskId   taskId,
    tsp00_Page     &copyPage,
    tsp00_Bool     &bNoMorePages)
{
    bNoMorePages = false;

    Data_PageFrame      frame( &copyPage, sizeof( tsp00_Page ));
    Converter_LeafPage  leafPage( frame );

    if( ! Converter_IBackUp::Instance().GetStaticArchiveLeafPage( leafPage ))
    {
        if( ! Converter_IBackUp::Instance().GetDynamicArchiveLeafPage( leafPage ))
        {
            bNoMorePages = true;
            return;
        }
    }
    leafPage.PrepareForWrite();
    SAPDBERR_ASSERT_STATE( leafPage.CheckAfterRead());
}

/*---------------------------------------------------------------------------*/

/*!
   @brief
   @param          TaskId [in] own Task ID
   @param          CopyPage [out] container to return copied page to caller
   @param          bNoMorePages [out] pages are available
   @return         none
*/
externC void
bd10GetConverterBitMapPage(
    tsp00_TaskId   taskId,
    tsp00_Page     &copyPage,
    tsp00_Bool     &bNoMorePages)
{
    bNoMorePages = false;

    Data_PageFrame   frame( &copyPage, sizeof( tsp00_Page ));
    Converter_BitMapPage      bitMapPage( frame );

    if( ! Converter_IBackUp::Instance().GetStaticBitMapPage( bitMapPage ))
    {
        if( ! Converter_IBackUp::Instance().GetDynamicBitMapPage( bitMapPage ))
        {
            bNoMorePages = true;
            return;
        }
    }
    bitMapPage.PrepareForWrite();
    SAPDBERR_ASSERT_STATE( bitMapPage.CheckAfterRead());
}

/*---------------------------------------------------------------------------*/

/*!
   @brief          Clears the containers with copied converter leafpages
   @return         none

   - removes all copied converter leafpages which belong to a back up
 */
externC void
bd10RemoveConverterArchivePages ()
{
    Converter_IBackUp::Instance().RemoveArchiveLeafPages();
}

/*---------------------------------------------------------------------------*/

/*!
   @brief          Clears the containers with used static and dynamic page numbers
   @return         none

   - removes all page numbers stored in the containers of
     used dynamic and static page numbers which belong to a back up (save pages)
 */
externC void
bd10RemoveConverterBitMapPages ()
{
    Converter_IBackUp::Instance().RemoveBitMapPages();
}

/*---------------------------------------------------------------------------*/

/*!
@brief          Releases page numbers which do not belong to a restored data base
@param          TaskId [in] task id
@param          pPage [in] page with converter bitmap
@return         none

- for all page number which are not marked as used at the UsedPageNoBitMapPage
  the corresponding converter entry is set to free and the block is released
- for all page numbers which are marked as used in the UsedPageNoBitMapPage but not
  in the converter the correspondung page entry is reserved
- all converter pages which contain any used page number are marked as used
  (see also FreePageNosNotInConverterBitMapPages)
*/
externC void
bd10RestoreUsedPageNos (tsp00_TaskId   TaskId,
                        tsp00_PageAddr pPage)
{
    const RTETask_ITask     &task = *RTETask_ITask::ByTaskID( TaskId );
    Kernel_Page::PageFrame  frame (pPage, sizeof(*pPage));
    Converter_BitMapPage    BitMapPage (frame);

    Converter_IBackUp::Instance().RestoreUsedPageNos ( task, BitMapPage);
}

/*---------------------------------------------------------------------------*/

/*!
@brief         integrate converter pages from backup
@param         taskId [in] own task id
@param         pPage [in] converter page from backup medium
@return        none

- integrate converter entries from backup into the current converter.
*/
externC void
bd10RestoreConverter (tsp00_TaskId   taskId,
                      tsp00_PageAddr pPage)
{
    RTETask_ITask&          task = *RTETask_ITask::ByTaskID( taskId );
    Kernel_Page::PageFrame  frame (pPage, sizeof(*pPage));
    Converter_LeafPage      leafPage (frame);

    Converter_IBackUp::Instance().RestoreConverterPage ( task, leafPage);
}

/*---------------------------------------------------------------------------*/

/*!
@brief          Releases page numbers which do not belong to a restored data base
@param          TaskId [in] task id
@return         none

- after all UsedPageNoBitMapPage of a back up are read this function
  scans the converter and removes all those converter leaf pages (and their entries)
  which are not marked as used. (see also FreeUnusedPageNo)
  This means all those converter entries which were neither marked as used nor as
  free in the UsedPageNoBitMapPage's (means not at all mentioned) are removed
*/
externC void
bd10FinishRestoreUsedPageNos (tsp00_TaskId TaskId)
{
    const RTETask_ITask &task = *RTETask_ITask::ByTaskID( TaskId );
    Converter_IBackUp::Instance().FinishRestoreUsedPageNos ( task);
}

/*---------------------------------------------------------------------------*/

/*!
   @brief    checks ConverterPage for consistency
   @param    Page [in] page to check
   @param    bPageIsDamaged [out] check result
   @return   none
*/
externC void
bd10CheckConverterPage(tsp00_Page     &Page,
                       tsp00_Bool     &bPageIsDamaged)
{
    Kernel_Page::PageFrame frame (&Page, sizeof(Page));
    Converter_LeafPage ConvPage (frame);
    bPageIsDamaged = (! ConvPage.CheckAfterRead());
}

/*---------------------------------------------------------------------------*/

/*!
   @brief    checks ConverterBitMapPage for consistency
   @param    Page [in] page to check
   @param    bPageIsDamaged [out] check result
   @return   none
*/
externC void
bd10CheckConverterBitMapPage(tsp00_Page     &Page,
                             tsp00_Bool     &bPageIsDamaged)
{
    Kernel_Page::PageFrame frame (&Page, sizeof(Page));
    Converter_BitMapPage BitMapPage (frame);
    bPageIsDamaged = (! BitMapPage.CheckAfterRead());
}

/*---------------------------------------------------------------------------*/

/*!
   @brief          Returns the numbers of all currently used converter pages
   @return         number of converter pages (perm and temp) currently used
 */
externC tsp00_Int4
bd10ConverterSize()
{
    return( Converter_ICommon::Instance().UsedConverterPages());
}


/*---------------------------------------------------------------------------*/

/*!
   @brief          Returns true if db is full.
   @param          taskId [in] task id
   @return         (bool)
 */
externC tsp00_Bool
bd10IsDbFull( tsp00_TaskId taskId )
{
    return( Converter_ICommon::Instance().IsDBFull());
}

/*---------------------------------------------------------------------------*/

/*!
   @brief          If the database filling is nearly full and therefore a
                   sensible work is not possible this method returns a true
                   else false.
   @return         true if security is reached and database nearly full else false
 */
externC tsp00_Bool
bd10IsConnectAllowed()
{
    return( ! Converter_ICommon::Instance().SecuritySpaceReached() );
}

/*---------------------------------------------------------------------------*/

/*!
    @brief   Checks if the current back up finished successfully. Checks if
             all pages marked as to be included in the back up were fetched
             by the back up manager
    @param   taskId [in] current task id
    @return  true if the back was successful else false
*/

externC tsp00_Bool
bd10CheckBackup( tsp00_TaskId taskId )
{
    const RTETask_ITask &task = *RTETask_ITask::ByTaskID( taskId );

    return( Converter_IBackUp::Instance().CheckBackUp( task ));
}

/*---------------------------------------------------------------------------*/

/*!
    @brief   returns the value of the configuration parameter VOLUMNO_BIT_COUNT
    @return  tsp00_Uint1
*/

externC tsp00_Uint1
bd10GetVolumeNoBitCount()
{
    Converter_ICommon::Instance().GetVolumeNoBitCount();

    return( Converter_ICommon::Instance().GetVolumeNoBitCount());
}
/*---------------------------------------------------------------------------*/

/*!
   @brief          Checks if the required count of pages is available.
   @note           Note this function does not reserve any number of pages for this tasks, i.e.
                   the information that a particular number is available  is dirty and may be
                   even false, when this task really tries to get wanted number of free pages.
   @param          trans [in/out] transaction context
   @param          numDataPagesRequested [in] number of pages wanted
                   data base to gain free pages
   @return         none

   - If the required count of pages is available this function returns true
*/
externC void
bd999CheckSpace (
    Trans_Context      &context,
    tsp00_Int4          numDataPagesRequested)
{
    const Converter_ReturnCode retCode =
        Converter_IPageNoManager::Instance().HandleDBFull(
            context, numDataPagesRequested );

    switch( retCode )
    {
    case Converter_Ok:
        context.OldTrans().trError_gg00 = e_ok;
        break;
    case Converter_TempSpaceExhausted:
        context.OldTrans().trError_gg00 = e_no_more_temp_space;
        break;
    case Converter_ExecutionCanceled:
        context.OldTrans().trError_gg00 = e_cancelled;
        break;
    default:
        {
            Converter_Exception msg( __CONTEXT__, CONVERTER_WRN_UNHANDLED_RET_CODE,
                                     SAPDB_ToString( retCode ));

            context.ErrorList().Overrule( msg );
            context.OldTrans().trError_gg00 = e_system_error;
        }
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
