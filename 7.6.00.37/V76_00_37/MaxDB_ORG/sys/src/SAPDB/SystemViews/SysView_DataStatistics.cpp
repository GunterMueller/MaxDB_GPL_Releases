/*!
  @file     SysView_DataStatistics.cpp
  @author   TorstenS
  @ingroup  SystemViews
  @brief    This contains a class object representing the datastatistics view.
  @see            
*/
/*

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



*/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg00.h"
#include "hbd20_6.h"  // bd20GetNumberOfChangedPages

#include "Converter/Converter_ICommon.hpp"
#include "FreeBlockManagement/FBM_IManager.hpp"
#include "FrameControl/FrameCtrl_ICommon.hpp"
#include "GarbageCollector/GC_IGCController.hpp" 
#include "IOManager/IOMan_IDataInfo.hpp"
#include "KernelCommon/Kernel_DateTime.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "SystemViews/SysView_Defines.hpp"
#include "SystemViews/SysView_ITableObj.hpp"
#include "SystemViews/SysView_DataStatistics.hpp"
#include "SQLManager/SQLMan_Context.hpp"

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
 *  STATIC/INLINE FUNCTIONS  (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

void SysView_DataStatistics::Create()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_DataStatistics::Create", SysView_Trace, 5);

    m_Table->AppendCol (ITOCT_FIXED ,       SV_USABLESIZE, 20);
    m_Table->AppendCol (ITOCT_FIXED ,       SV_USEDSIZE, 20);
    m_Table->AppendCol (ITOCT_FIXED ,       SV_USEDSIZEPERCENTAGE, 3);
    m_Table->AppendCol (ITOCT_CHAR  ,       SV_DATAFULL, 3);
    m_Table->AppendCol (ITOCT_FIXED ,       SV_USEDSIZEONVOLUMES, 20);
    m_Table->AppendCol (ITOCT_FIXED ,       SV_USEDSIZENOTONVOLUMES, 20);
    m_Table->AppendCol (ITOCT_FIXED ,       SV_USEDPERMANENTSIZE, 20);
    m_Table->AppendCol (ITOCT_FIXED ,       SV_USEDTEMPORARYSIZE, 20);
    m_Table->AppendCol (ITOCT_FIXED ,       SV_MAXUSEDSIZE, 20);
    m_Table->AppendCol (ITOCT_FIXED ,       SV_MAXUSEDPERMANENTSIZE, 20);
    m_Table->AppendCol (ITOCT_FIXED ,       SV_MAXUSEDTEMPORARYSIZE, 20);
    m_Table->AppendCol (ITOCT_FIXED ,       SV_INCREMENTALBACKUPSIZE, 20);
    m_Table->AppendCol (ITOCT_TIMESTAMP,    SV_LASTEMERGENCYHISTORYCLEANUP, 0);
    m_Table->AppendCol (ITOCT_FIXED ,       SV_CONVERTERVERSION, 10);
    m_Table->AppendCol (ITOCT_FIXED ,       SV_MAXTEMPORARYDATAPAGENUMBER, 10);
    m_Table->AppendCol (ITOCT_FIXED ,       SV_MAXPERMANENTDATAPAGENUMBER, 10);
    m_Table->AppendCol (ITOCT_FIXED ,       SV_MAXPERMANENTSTATICDATAPAGENUMBER, 10);
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_DataStatistics::GetColCount()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_DataStatistics::GetColCount", SysView_Trace, 5);

    return SV_CC_DATASTATISTICS;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_DataStatistics::EstimateRows()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_DataStatistics::EstimateRows", SysView_Trace, 5);

    return SV_ER_DATASTATISTICS;
}

/*---------------------------------------------------------------------------*/

void SysView_DataStatistics::Execute()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_DataStatistics::Execute", SysView_Trace, 5);

    Converter_ICommon   &iConvCom = Converter_ICommon::Instance();
    const RTETask_ITask &task     = *RTETask_ITask::ByTaskID( m_Context.TransContext().trTaskId_gg00 );
    const SAPDB_UInt4   pageSize  = FrameCtrl_ICommon::GetInstance().GetPageSize() / 1024;

    m_Table->GetCatalogTable();

    // USABLE_SIZE

    SAPDB_UInt usableSize = IOMan_IDataInfo::GetInstance().TotalUsableDataPages() * pageSize;

    m_Table->MoveToCol( ITOVT_UINT4, &usableSize, 0 );

    // USED_SIZE:
    // determine the number of changed permanent data and converter pages. No temp pages!

    SAPDB_UInt changedPages = (bd20GetNumberOfChangedPages() +
                               iConvCom.GetNumberOfChangedConvPages()) * pageSize;
    SAPDB_UInt usedPages    = iConvCom.GetNumberOfUsedPages() * pageSize;
    SAPDB_UInt usedBlocks   = FBM_IManager::Instance().GetNumberOfUsedBlocks() * pageSize;
    SAPDB_UInt usedSize     = SAPDB_MAX( usedPages, usedBlocks );

    m_Table->MoveToCol( ITOVT_UINT4, &usedSize, 0 );

    // USED_SIZE_PERCENTAGE

    SAPDB_UInt usedSizePercent = ( 0 >= usableSize ) ? 0 : ((usedSize * 100) / usableSize );

    m_Table->MoveToCol( ITOVT_UINT4, &usedSizePercent, 0 );

    // DATA_FULL

    SAPDB_Bool bIsFull = iConvCom.IsDBFull();

    m_Table->MoveToCol( ITOVT_YESNO, &bIsFull, 0 );

    // USED_SIZE_ON_VOLUMES

    m_Table->MoveToCol( ITOVT_UINT4, &usedBlocks, 0 );

    // USED_SIZE_NOT_ON_VOLUMES

    m_Table->MoveToCol( ITOVT_UINT4, &changedPages, 0 );

    // USED_PERMANENT_SIZE

    SAPDB_UInt permPages = iConvCom.GetNumberOfPermPages() * pageSize;

    m_Table->MoveToCol( ITOVT_UINT4, &permPages, 0 );

    // USED_TEMPORARY_SIZE

    SAPDB_UInt tempPages = iConvCom.GetNumberOfTempPages() * pageSize;

    m_Table->MoveToCol( ITOVT_UINT4, &tempPages, 0 );

    // MAX_USED_SIZE

    SAPDB_UInt permPeak     = iConvCom.GetPeakForPermPages() * pageSize;
    SAPDB_UInt tempPeak     = iConvCom.GetPeakForTempPages() * pageSize;
    SAPDB_UInt maxPagesUsed = SAPDB_MAX( permPeak, tempPeak );

    m_Table->MoveToCol( ITOVT_UINT4, &maxPagesUsed, 0 );

    // MAX_USED_PERMANENT_SIZE

    m_Table->MoveToCol( ITOVT_UINT4, &permPeak, 0 );


    // MAX_USED_TEMPORARY_SIZE

    m_Table->MoveToCol( ITOVT_UINT4, &tempPeak, 0 );

    // INCREMENTAL_BACKUP_SIZE

    SAPDB_UInt updatedPages = iConvCom.UpdatedPagesSinceLastSaveData( task ) * pageSize;

    m_Table->MoveToCol( ITOVT_UINT4, &updatedPages, 0 );

    // LAST_EMERGENCY_HISTORY_CLEANUP

    RTESys_SQLTimestamp timeStamp;
    Kernel_DateTime     dateTime( GC_IGCController::Instance().GetGCLastTimeStarted());

    dateTime.GetDateTimeAsSQLTimeStamp( timeStamp );

    if( dateTime.IsValid())
    {
        dateTime.GetDateTimeAsSQLTimeStamp( timeStamp );

        m_Table->MoveToCol( ITOVT_CHARPTR, &timeStamp, sizeof( RTESys_SQLTimestamp ));
    }
    else
        m_Table->MoveToCol( ITOVT_NULL, NULL, 0 );

    // CONVERTER_VERSION

    SAPDB_UInt convVersion = iConvCom.Version();

    m_Table->MoveToCol( ITOVT_UINT4, &convVersion, 0 );

    // MAX_TEMPORARY_DATA_PAGE_NUMMER

    SAPDB_UInt maxTempPageNo = iConvCom.MaxTempPageNo();

    m_Table->MoveToCol( ITOVT_UINT4, &maxTempPageNo, 0 );

    // MAX_PERMERMANENT_DATA_PAGE_NUMMER

    SAPDB_UInt maxPermPageNo = iConvCom.MaxPermPageNo();

    m_Table->MoveToCol( ITOVT_UINT4, &maxPermPageNo, 0 );

    // MAX_PERM_PERMANENT_STATIC_DATA_PAGE_NUMMER

    SAPDB_UInt maxPermStaticPageNo = iConvCom.MaxStaticPageNo();

    m_Table->MoveToCol( ITOVT_UINT4, &maxPermStaticPageNo, 0 );
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool SysView_DataStatistics::ResetCounter(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_DataStatistics::ResetCounter", SysView_Trace, 5);

    Converter_ICommon::Instance().ResetSystemViewDataStatistics();
    return ( true );
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
