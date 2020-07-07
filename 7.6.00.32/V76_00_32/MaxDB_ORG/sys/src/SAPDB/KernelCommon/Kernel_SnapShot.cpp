/*!

  @file         Kernel_SnapShot.cpp
  @author       TorstenS
  @ingroup      Kernel_Common
  @brief        This class is used to create, drop and read the snapshot
                restart record which is used when a converter snapshot
                is defined.
*/
/*
    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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

*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "Converter/Converter_ICommon.hpp"
#include "DataAccess/Data_Types.hpp"
#include "FrameControl/FrameCtrl_IRestartPage.hpp"
#include "FreeBlockManagement/FBM_IManager.hpp"
#include "IOManager/IOMan_BlockAddress.hpp"
#include "IOManager/IOMan_IDataIO.hpp"
#include "KernelCommon/Kernel_DateTime.hpp"
#include "KernelCommon/Kernel_RestartPage.hpp"
#include "KernelCommon/Kernel_Exception.hpp"
#include "KernelCommon/Kernel_Messages.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "RunTime/RTE_Crash.hpp"

#include "KernelCommon/Kernel_SnapShot.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/
// datadrea is online

void
Kernel_SnapShot::MarkSnapShotRestartPageAsOccupied(
    const RTETask_ITask             &task,
    const tkb00_ConfigRestartParam  &rstConfig,
    IOMan_PackedBlockAddress        &packedSnapShotRootBlock,
    Converter_Version               &snapShotVersion,
    Kernel_DateTime                 &snapShotCreationTimeStamp )
{

    if( 0 == rstConfig.crSnapShotRestartRecordAddr_kb00 )
        return; // no snapshot defined

    const IOMan_PackedBlockAddress  packedSnapShotRestartAddr( rstConfig.crSnapShotRestartRecordAddr_kb00 );
    const SAPDB_Int           volNoBitCount        = Converter_ICommon::Instance().GetVolumeNoBitCount();
    const IOMan_BlockAddress  snapShotRestartBlock = packedSnapShotRestartAddr.Unpack( volNoBitCount );

    FrameCtrl_IRestartPage  &frameControl = FrameCtrl_IRestartPage::GetInstance();
    Data_PageFrame          pageFrame     = frameControl.NewRestartPageFrame( task.ID() );
    Kernel_RestartPage      snapShotRstPage( pageFrame );

    IOMan_IDataIO::GetInstance().ReadSnapShotRestartPage( task.ID(), snapShotRstPage, snapShotRestartBlock );

    // make sure that the block of the frozen restart record
    // will not be used by another page

    if( ! FBM_IManager::Instance().SetBlockStateToOccupied( task.ID(), snapShotRestartBlock ))
    {
        Kernel_Exception errMsg( __CONTEXT__, KERNEL_ERR_SET_TO_OCCUPIED );
        RTE_Crash( errMsg );
    }
    const tkb00_PagePtr   pPage = reinterpret_cast<tkb00_PagePtr>(snapShotRstPage.GetFramePtr());

    snapShotVersion         = pPage->rstConverterVersion_kb00();
    packedSnapShotRootBlock = pPage->rstConfigParam_kb00().crConvRootBlockAddr_kb00;

    Kernel_DateTime auxTime( pPage->rstLastSavept_kb00().svpDate_kb00, pPage->rstLastSavept_kb00().svpTime_kb00);

    snapShotCreationTimeStamp = auxTime;

    frameControl.FreeRestartPageFrame( task.ID(), pageFrame );
}

/* --------------------------------------------------------------------------- */

// dataarea is online/ restart record available
void
Kernel_SnapShot::Create(
    const RTETask_ITask  &task,
    tkb00_PagePtr        pRstPage )
{
    SAPDBERR_ASSERT_STATE( NULL != pRstPage );

    IOMan_BlockAddress  rstPageBlock;
    Kernel_RestartPage  rstPage( pRstPage ); // prepare copy restart page

    k57restartrec->rstConfigParam_kb00().crSnapShotRestartRecordAddr_kb00 = 0;

    IOMan_IDataIO::GetInstance().WriteSnapShotRestartPage( task.ID(), rstPage, rstPageBlock );

    const SAPDB_Int                 volNoBitCount      = Converter_ICommon::Instance().GetVolumeNoBitCount();
    const IOMan_PackedBlockAddress  packedRstPageBlock = rstPageBlock.Pack( volNoBitCount );

    pRstPage->rstConfigParam_kb00().crSnapShotRestartRecordAddr_kb00 = packedRstPageBlock;

    IOMan_IDataIO::GetInstance().WriteRestartPage( task.ID(), rstPage );
}

/* --------------------------------------------------------------------------- */

// dataarea is online/ restart record available
void
Kernel_SnapShot::Drop(
    const RTETask_ITask  &task,
    tkb00_PagePtr        pRstPage )
{
    SAPDBERR_ASSERT_STATE( NULL != k57restartrec );

    pRstPage->rstConfigParam_kb00().crSnapShotRestartRecordAddr_kb00 = 0;
}

/* --------------------------------------------------------------------------- */

void
Kernel_SnapShot::Activate( const RTETask_ITask  &task )
{
    // TODO
}


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
