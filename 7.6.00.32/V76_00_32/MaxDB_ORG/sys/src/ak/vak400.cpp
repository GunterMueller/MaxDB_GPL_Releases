
/****************************************************************************

  module      : vak400.cpp

  -------------------------------------------------------------------------

  responsible : UlrichJ

  special area: SystemViews
  description : This module implements the pascal interface functions for the
                new c++ system view classes.

  last changed: 2002-08-13
  see also    :

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2002-2005 SAP AG

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



*****************************************************************************/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "gsp00.h"
#include "ggg00.h"
#include "gak40.h"
#include "hak07.h"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SystemViews/SysView_Defines.hpp"
#include "SystemViews/SysView_ISysView.hpp"
#include "SystemViews/SysView_ActiveConfiguration.hpp"
#include "SystemViews/SysView_BackupInformation.hpp"
#include "SystemViews/SysView_BackTrace.hpp"
#include "SystemViews/SysView_BackupThreads.hpp"
#include "SystemViews/SysView_CacheStatistics.hpp"
#include "SystemViews/SysView_CacheStatisticsReset.hpp"
#include "SystemViews/SysView_CatalogCacheStatistics.hpp"
#include "SystemViews/SysView_ClassContainers.hpp"
#include "SystemViews/SysView_ClassContainerStatistics.hpp"
#include "SystemViews/SysView_ClassContainerStatisticsReset.hpp"
#include "SystemViews/SysView_CommandCacheStatistics.hpp"
#include "SystemViews/SysView_CommandCacheStatisticsReset.hpp"
#include "SystemViews/SysView_CommandStatistics.hpp"
#include "SystemViews/SysView_CommandStatisticsReset.hpp"
#include "SystemViews/SysView_DataCache.hpp"
#include "SystemViews/SysView_DataStatistics.hpp"
#include "SystemViews/SysView_DataStatisticsReset.hpp"
#include "SystemViews/SysView_DataVolumes.hpp"
#include "SystemViews/SysView_FileDirectories.hpp"
#include "SystemViews/SysView_Files.hpp"
#include "SystemViews/SysView_HistoryCleanupStatistics.hpp"
#include "SystemViews/SysView_HistoryCleanupStatisticsReset.hpp"
#include "SystemViews/SysView_HotStandbyComponent.hpp"
#include "SystemViews/SysView_HotStandbyGroup.hpp"
#include "SystemViews/SysView_IndexStorageDetails.hpp"
#include "SystemViews/SysView_Instance.hpp"
#include "SystemViews/SysView_IOBufferCaches.hpp"
#include "SystemViews/SysView_IOThreadStatistics.hpp"
#include "SystemViews/SysView_IOThreadStatisticsReset.hpp"
#include "SystemViews/SysView_KernelMessages.hpp"
#include "SystemViews/SysView_Locks.hpp"
#include "SystemViews/SysView_LockStatistics.hpp"
#include "SystemViews/SysView_LockStatisticsReset.hpp"
#include "SystemViews/SysView_LogInformation.hpp"
#include "SystemViews/SysView_LogQueueStatistics.hpp"
#include "SystemViews/SysView_LogQueueStatisticsReset.hpp"
#include "SystemViews/SysView_LogStatistics.hpp"
#include "SystemViews/SysView_LogStatisticsReset.hpp"
#include "SystemViews/SysView_LogVolumes.hpp"
#include "SystemViews/SysView_MachineConfiguration.hpp"
#include "SystemViews/SysView_MachineUtilization.hpp"
#include "SystemViews/SysView_MemoryAllocatorStatistics.hpp"
#include "SystemViews/SysView_MemoryAllocatorStatisticsReset.hpp"
#include "SystemViews/SysView_MemoryHolders.hpp"
#include "SystemViews/SysView_Miscellaneous.hpp"
#include "SystemViews/SysView_MultiVersionReadStatistics.hpp"
#include "SystemViews/SysView_ObjectLocks.hpp"
#include "SystemViews/SysView_OMSDiagnose.hpp"      // PTS 1127508
#include "SystemViews/SysView_OMSLocks.hpp"
#include "SystemViews/SysView_OMSProcedureStatistics.hpp"
#include "SystemViews/SysView_OMSProcedureStatisticsReset.hpp"
#include "SystemViews/SysView_OMSVersions.hpp"
#include "SystemViews/SysView_OptimizerInformation.hpp"
#include "SystemViews/SysView_PageLocks.hpp"
#include "SystemViews/SysView_PseudoLongHandler.hpp"
#include "SystemViews/SysView_ReaderWriterLockInformation.hpp"
#include "SystemViews/SysView_ReaderWriterLockStatistics.hpp"
#include "SystemViews/SysView_ReaderWriterLockStatisticsReset.hpp"
#include "SystemViews/SysView_ReaderWriterLockWaitingTasks.hpp"
#include "SystemViews/SysView_RegionInformation.hpp"
#include "SystemViews/SysView_RegionStatistics.hpp"
#include "SystemViews/SysView_RegionStatisticsReset.hpp"
#include "SystemViews/SysView_RestartInformation.hpp"
#include "SystemViews/SysView_Schemas.hpp"
#include "SystemViews/SysView_Servertasks.hpp"
#include "SystemViews/SysView_Sessions.hpp"
#include "SystemViews/SysView_Snapshots.hpp"
#include "SystemViews/SysView_SpinLockPoolStatistics.hpp"
#include "SystemViews/SysView_SpinLockPoolStatisticsReset.hpp"
#include "SystemViews/SysView_SpinLockStatistics.hpp"
#include "SystemViews/SysView_SpinLockStatisticsReset.hpp"
#include "SystemViews/SysView_SysViewBase.hpp"
#include "SystemViews/SysView_TableStorageDetails.hpp"
#include "SystemViews/SysView_TaskGroupStatistics.hpp"
#include "SystemViews/SysView_TaskGroupStatisticsReset.hpp"
#include "SystemViews/SysView_TaskLoadBalancingInformation.hpp"
#include "SystemViews/SysView_TaskLoadBalancingTaskGroupStates.hpp"
#include "SystemViews/SysView_TaskLoadBalancingTaskMoves.hpp"
#include "SystemViews/SysView_TaskStatistics.hpp"
#include "SystemViews/SysView_TaskStatisticsReset.hpp"
#include "SystemViews/SysView_Transactions.hpp"
#include "SystemViews/SysView_TransactionHistory.hpp"
#include "SystemViews/SysView_UnloadedStatements.hpp"
#include "SystemViews/SysView_Version.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

SysView_ISysView * SVGetInstanceFromSH ( tak_all_command_glob	&acv,
                                         tak40_show_glob        &a41v)
{
    SAPDBMem_IRawAllocator  *mAlloc;
    SysView_ISysView        *ret; 

    SQLMan_Context& context = SQLMan_Context::AcvToContext(acv);

    
    mAlloc = reinterpret_cast<SAPDBMem_IRawAllocator *> (acv.a_transinf.tri_trans.trAllocator_gg00);
    
    switch (a41v.a4sh_kind)
    {
    case sh_activeconfiguration:
        ret = new(*mAlloc) SysView_ActiveConfiguration(context, a41v);
        break;
    case sh_backtrace :
        ret = new(*mAlloc) SysView_BackTrace(context, a41v);
        break;
	case sh_backupinformation:
		ret = new(*mAlloc) SysView_BackupInformation( context, a41v ); 
		break;
	case sh_backupthreads:
		ret = new(*mAlloc) SysView_BackupThreads( context, a41v );
		break;
	case sh_cachestatistics:
		ret = new(*mAlloc) SysView_CacheStatistics( context, a41v );
		break;
	case sh_cachestatisticsreset:
		ret = new(*mAlloc) SysView_CacheStatisticsReset( context, a41v );
		break;
    case sh_catalogcachestatistics :
        ret = new(*mAlloc) SysView_CatalogCacheStatistics( context, a41v );
		break;
	case sh_classcontainers:
		ret = new(*mAlloc) SysView_ClassContainers( context, a41v );
		break;
	case sh_classcontainerstatistics:
		ret = new(*mAlloc) SysView_ClassContainerStatistics( context, a41v );
		break;
	case sh_classcontainerstatisticsreset:
		ret = new(*mAlloc) SysView_ClassContainerStatisticsReset( context, a41v );
		break;
	case sh_commandcachestatistics:
		ret = new(*mAlloc) SysView_CommandCacheStatistics( context, a41v );
		break;
	case sh_commandcachestatisticsreset:
		ret = new(*mAlloc) SysView_CommandCacheStatisticsReset( context, a41v );
		break;
	case sh_commandstatistics:
		ret = new(*mAlloc) SysView_CommandStatistics( context, a41v );
		break;
	case sh_commandstatisticsreset:
		ret = new(*mAlloc) SysView_CommandStatisticsReset( context, a41v );
		break;
    case sh_datacache:
		ret = new(*mAlloc) SysView_DataCache( context, a41v );
		break;
	case sh_datastatistics:
		ret = new(*mAlloc) SysView_DataStatistics( context, a41v );
		break;
	case sh_datastatisticsreset:
		ret = new(*mAlloc) SysView_DataStatisticsReset( context, a41v );
		break;
	case sh_datavolumes:
		ret = new(*mAlloc) SysView_DataVolumes( context, a41v );
		break;
	case sh_filedirectories:
		ret = new(*mAlloc) SysView_FileDirectories( context, a41v );
		break;
	case sh_files:
		ret = new(*mAlloc) SysView_Files( context, a41v );
		break;
	case sh_historycleanupstatistics:
		ret = new(*mAlloc) SysView_HistoryCleanupStatistics( context, a41v );
		break;
	case sh_historycleanupstatisticsreset:
		ret = new(*mAlloc) SysView_HistoryCleanupStatisticsReset( context, a41v );
		break;
	case sh_hotstandbycomponent:
		ret = new(*mAlloc) SysView_HotStandbyComponent( context, a41v );
		break;
	case sh_hotstandbygroup:
		ret = new(*mAlloc) SysView_HotStandbyGroup( context, a41v );
		break;
	case sh_indexstoragedetails:
		ret = new(*mAlloc) SysView_IndexStorageDetails( context, a41v );
		break;
	case sh_instance:
		ret = new(*mAlloc) SysView_Instance( context, a41v );
		break;
	case sh_iobuffercaches:
		ret = new(*mAlloc) SysView_IOBufferCaches( context, a41v );
		break;
	case sh_iothreadstatistics:
		ret = new(*mAlloc) SysView_IOThreadStatistics( context, a41v );
		break;
	case sh_iothreadstatisticsreset:
		ret = new(*mAlloc) SysView_IOThreadStatisticsReset( context, a41v );
		break;
	case sh_kernelmessages:
		ret = new(*mAlloc) SysView_KernelMessages( context, a41v );
		break;
	case sh_locks:
		ret = new(*mAlloc) SysView_Locks( context, a41v );
		break;
	case sh_lockstatistics:
		ret = new(*mAlloc) SysView_LockStatistics( context, a41v );
		break;
	case sh_lockstatisticsreset:
		ret = new(*mAlloc) SysView_LockStatisticsReset( context, a41v );
		break;
	case sh_loginformation:
		ret = new(*mAlloc) SysView_LogInformation( context, a41v );
		break;
	case sh_logqueuestatistics:
		ret = new(*mAlloc) SysView_LogQueueStatistics( context, a41v );
		break;
	case sh_logqueuestatisticsreset:
		ret = new(*mAlloc) SysView_LogQueueStatisticsReset( context, a41v );
		break;
	case sh_logstatistics:
		ret = new(*mAlloc) SysView_LogStatistics( context, a41v );
		break;
	case sh_logstatisticsreset:
		ret = new(*mAlloc) SysView_LogStatisticsReset( context, a41v );
		break;
	case sh_logvolumes:
		ret = new(*mAlloc) SysView_LogVolumes( context, a41v );
		break;
	case sh_machineconfiguration:
		ret = new(*mAlloc) SysView_MachineConfiguration( context, a41v );
		break;
	case sh_machineutilization:
		ret = new(*mAlloc) SysView_MachineUtilization( context, a41v );
		break;
	case sh_memoryallocatorstatistics:
		ret = new(*mAlloc) SysView_MemoryAllocatorStatistics( context, a41v );
		break;
	case sh_memoryallocatorstatisticsreset:
		ret = new(*mAlloc) SysView_MemoryAllocatorStatisticsReset( context, a41v );
		break;
	case sh_memoryholders:
		ret = new(*mAlloc) SysView_MemoryHolders( context, a41v );
		break;
    case sh_multiVersionReadStats :
       	ret = new(*mAlloc) SysView_MultiVersionReadStatistics( context, a41v );
		break;
	case sh_objectlocks:
		ret = new(*mAlloc) SysView_ObjectLocks( context, a41v );
		break;
    case sh_miscellaneous:
		ret = new(*mAlloc) SysView_Miscellaneous( context, a41v );
		break;
	case sh_omsdiagnose:           // PTS 1127508
		ret = new(*mAlloc) SysView_OMSDiagnose( context, a41v );
		break;
	case sh_omslocks:
		ret = new(*mAlloc) SysView_OMSLocks( context, a41v );
		break;
	case sh_omsprocedurestatistics:
		ret = new(*mAlloc) SysView_OMSProcedureStatistics( context, a41v );
		break;
	case sh_omsprocedurestatisticsreset:
		ret = new(*mAlloc) SysView_OMSProcedureStatisticsReset( context, a41v );
		break;
	case sh_omsversions:
		ret = new(*mAlloc) SysView_OMSVersions( context, a41v );
		break;
	case sh_optimizerinformation:
		ret = new(*mAlloc) SysView_OptimizerInformation( context, a41v );
		break;
	case sh_pagelocks:
		ret = new(*mAlloc) SysView_PageLocks( context, a41v );
		break;
	case sh_readerwriterlockinformation:
        ret = new(*mAlloc) SysView_ReaderWriterLockInformation( context, a41v );
		break;
	case sh_readerwriterlockstatistics:
        ret = new(*mAlloc) SysView_ReaderWriterLockStatistics( context, a41v );
		break;
	case sh_readerwriterlockstatisticsreset:
        ret = new(*mAlloc) SysView_ReaderWriterLockStatisticsReset( context, a41v );
		break;
    case sh_readerwriterlockwaitingtasks:
        ret = new(*mAlloc) SysView_ReaderWriterLockWaitingTasks( context, a41v );
	    break;
	case sh_regioninformation:
		ret = new(*mAlloc) SysView_RegionInformation( context, a41v );
		break;
	case sh_regionstatistics:
		ret = new(*mAlloc) SysView_RegionStatistics( context, a41v );
		break;
	case sh_regionstatisticsreset:
		ret = new(*mAlloc) SysView_RegionStatisticsReset( context, a41v );
		break;
	case sh_restartinformation:
		ret = new(*mAlloc) SysView_RestartInformation( context, a41v );
		break;
	case sh_schemas:
		ret = new(*mAlloc) SysView_Schemas( context, a41v );
		break;
    case sh_servertasks:
        ret = new(*mAlloc) SysView_Servertasks(context, a41v);
        break;
	case sh_sessions:
		ret = new(*mAlloc) SysView_Sessions( context, a41v );
		break;
	case sh_snapshot:
		ret = new(*mAlloc) SysView_Snapshots( context, a41v );
		break;
	case sh_spinlockpoolstatistics:
		ret = new(*mAlloc) SysView_SpinLockPoolStatistics( context, a41v );
		break;
	case sh_spinlockpoolstatisticsreset:
		ret = new(*mAlloc) SysView_SpinLockPoolStatisticsReset( context, a41v );
		break;
	case sh_spinlockstatistics:
		ret = new(*mAlloc) SysView_SpinLockStatistics( context, a41v );
		break;
	case sh_spinlockstatisticsreset:
		ret = new(*mAlloc) SysView_SpinLockStatisticsReset( context, a41v );
		break;
	case sh_tablestoragedetails:
		ret = new(*mAlloc) SysView_TableStorageDetails( context, a41v );
		break;
	case sh_taskgroupstatistics:
		ret = new(*mAlloc) SysView_TaskGroupStatistics( context, a41v );
		break;
	case sh_taskgroupstatisticsreset:
		ret = new(*mAlloc) SysView_TaskGroupStatisticsReset( context, a41v );
		break;
	case sh_taskloadbalancinginformation:
		ret = new(*mAlloc) SysView_TaskLoadBalancingInformation( context, a41v );
		break;
	case sh_taskloadbalancingtaskgroupstates:
		ret = new(*mAlloc) SysView_TaskLoadBalancingTaskGroupStates( context, a41v );
		break;
	case sh_taskloadbalancingtaskmoves:
		ret = new(*mAlloc) SysView_TaskLoadBalancingTaskMoves( context, a41v );
		break;
	case sh_taskstatistics:
		ret = new(*mAlloc) SysView_TaskStatistics( context, a41v );
		break;
	case sh_taskstatisticsreset:
		ret = new(*mAlloc) SysView_TaskStatisticsReset( context, a41v );
		break;
	case sh_transactions:
		ret = new(*mAlloc) SysView_Transactions( context, a41v );
		break;
	case sh_transactionhistory:
		ret = new(*mAlloc) SysView_TransactionHistory( context, a41v );
		break;
	case sh_unloadedstatements:
		ret = new(*mAlloc) SysView_UnloadedStatements( context, a41v );
		break;
	case sh_version:
		ret = new(*mAlloc) SysView_Version( context, a41v );
		break;
    default:
        ret = NULL;
    }

    return (ret);
}

/*------------------------------------------------------------------------------------------*/

void SVReleaseInstance( tak_all_command_glob	&acv,
                        SysView_ISysView *		sv )
{
    SAPDBMem_IRawAllocator *mAlloc;
    
    
    if ( sv != NULL )
    {
        mAlloc = reinterpret_cast<SAPDBMem_IRawAllocator *> (acv.a_transinf.tri_trans.trAllocator_gg00);
 
        destroy(sv, *mAlloc);
    }
    
}
/*------------------------------------------------------------------------------------------*/

externPascal int a400SVGetColCount( tak_all_command_glob	&acv,
                                    tak40_show_glob			&a41v )
{
    
    SysView_ISysView *	sv;
    int					iCols;
    
    sv = SVGetInstanceFromSH ( acv, a41v );
    
    if( sv )
    {
        iCols = sv->GetColCount();
    }
    else
    {
        iCols = -1;
        a07_b_put_error (acv, e_unknown_tablename, 1);
    }
    
    SVReleaseInstance ( acv, sv );
    
    return ( iCols );
}

/*------------------------------------------------------------------------------------------*/

externPascal void a400SVDefSysViewTable( tak_all_command_glob	&acv,
                                         tak40_show_glob		&a41v ) 
{
    SysView_ISysView *	sv;
    
    sv = SVGetInstanceFromSH ( acv, a41v );
    
    if( sv )
    {
        sv->Create();
    }
    else
    {
        a07_b_put_error (acv, e_unknown_tablename, 1);
    }
    
    SVReleaseInstance ( acv, sv );
}

/*------------------------------------------------------------------------------------------*/

externPascal int a400SVEstimateRows( tak_all_command_glob	&acv,
                                     tak40_show_glob		&a41v )
{
    SysView_ISysView *	sv;
    int					iRows;
    
    sv = SVGetInstanceFromSH ( acv, a41v );
    
    if( sv )
    {
        iRows = sv->EstimateRows();
    }
    else
    {
        iRows = -1;
        a07_b_put_error (acv, e_unknown_tablename, 1);
    }
    
    SVReleaseInstance ( acv, sv );
    
    return ( iRows );
}

/*------------------------------------------------------------------------------------------*/

externPascal void a400SVExecute( tak_all_command_glob	&acv, 
                                 tak40_show_glob		&a41v)
{
    SysView_ISysView *	sv;
    
    sv = SVGetInstanceFromSH ( acv, a41v );

    if( sv )
    {
        sv->Execute( );
    }
    else
    {
        a07_b_put_error (acv, e_unknown_tablename, 1);
    }
    
    SVReleaseInstance ( acv, sv );
}

/*------------------------------------------------------------------------------------------*/

externPascal void a400SVReset( tak_all_command_glob	&acv, 
                               tak40_show_glob		&a41v)
{
    SysView_ISysView *	sv;
    
    sv = SVGetInstanceFromSH ( acv, a41v );

    if( sv )
    {
        if (!sv->ResetCounter( ))
            {
                a07_b_put_error (acv, e_unknown_tablename, 1);
            }
    }
    else
    {
        a07_b_put_error (acv, e_unknown_tablename, 2);
    }
    
    SVReleaseInstance ( acv, sv );
}

/*------------------------------------------------------------------------------------------*/

externPascal void a400SVCopyLongValue ( tak_all_command_glob &acv,
                                        tgg00_Surrogate      &longId,
                                        int                  rowCount,
                                        void*                pDest,
                                        tsp00_Int4           destSize,
                                        tsp00_Int4           &length)
{
    SQLMan_Context& context = SQLMan_Context::AcvToContext(acv);
    SysView_PseudoLongHandler::LongIdentificator rowDescriptor;

    SAPDB_MemCopyNoCheck (&rowDescriptor, &longId, sizeof(longId));
    rowDescriptor.m_rowCount = rowCount;
    length = context.GetPseudoLongHandler().ReadLong (context, rowDescriptor, 1, destSize, pDest, destSize);
}

/*------------------------------------------------------------------------------------------*/

externPascal void a400SVDestroyPseudoColumnFile( tak_all_command_glob &acv,
                                                 tsp00_Int4           &fileId)
{
     SQLMan_Context& context = SQLMan_Context::AcvToContext(acv);
     context.GetPseudoLongHandler().DestroyPseudoLongContainer(context, fileId);
}
