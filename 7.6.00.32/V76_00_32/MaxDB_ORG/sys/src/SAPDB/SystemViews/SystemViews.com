# ------------------------------------------------------------------------------
#
# System View Descriptions
#
# ------------------------------------------------------------------------------
#
#
#    ========== licence begin  GPL
#    Copyright (c) 2000-2005 SAP AG
#
#    This program is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public License
#    as published by the Free Software Foundation; either version 2
#    of the License, or (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#    ========== licence end

regex(.*\.cpp$)    -except

# inc=+hta99.h,hta01.h

#
# include dependencies (do not edit)
#
SysView_ActiveConfiguration.cpp inc=ggg00,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp,\
    :SAPDB/SystemViews/SysView_ActiveConfiguration.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp
SysView_ActiveConfiguration.hpp inc=\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_BackTrace.cpp inc=ggg00,:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_BackTrace.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/DBProc/DBProc_Debugger.hpp,hak262.h
SysView_BackTrace.hpp inc=:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_BackupInformation.cpp inc=ggg00,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_BackupInformation.hpp
SysView_BackupInformation.hpp inc=:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_BackupThreads.cpp inc=ggg00,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_BackupThreads.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,geo00_0.h,geo50_0.h,geo002.h,\
    :SAPDB/SAPDBCommon/SAPDB_sprintf.h
SysView_BackupThreads.hpp inc=:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_CacheStatistics.cpp inc=ggg00,hak10.h,hak42.h,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_CacheStatistics.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_SQLCache.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_CommandCacheStatistics.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/SQLMan_ContextSessionMembers.hpp,\
    :SAPDB/SQLManager/SQLMan_SessionIterator.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Cache.hpp
SysView_CacheStatistics.hpp inc=:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_CacheStatisticsReset.cpp inc=ggg00,hak10.h,hak51.h,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_CacheStatisticsReset.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_SQLCache.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_CommandCacheStatistics.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/SQLMan_ContextSessionMembers.hpp,\
    :SAPDB/RunTime/RTE_KGS.hpp
SysView_CacheStatisticsReset.hpp inc=\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_CatalogCacheStatistics.cpp inc=\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_CatalogCacheStatistics.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Cache.hpp
SysView_CatalogCacheStatistics.hpp inc=\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_ClassContainerStatistics.cpp inc=ggg00,hbd04.h,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_ClassContainerStatistics.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/FileDirectory/FileDir_IDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_Iterator.hpp,\
    :SAPDB/FileDirectory/FileDir_Oms.hpp
SysView_ClassContainerStatistics.hpp inc=\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_ClassContainerStatisticsReset.cpp inc=ggg00,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_ClassContainerStatisticsReset.hpp
SysView_ClassContainerStatisticsReset.hpp inc=\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_ClassContainers.cpp inc=ggg00,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_ClassContainers.hpp
SysView_ClassContainers.hpp inc=:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_CommandCacheStatistics.cpp inc=ggg00,hgg01_3.h,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_CommandCacheStatistics.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_SQLCache.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_CommandCacheStatistics.hpp,\
    :SAPDB/SQLManager/SQLMan_ContextSessionMembers.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp
SysView_CommandCacheStatistics.hpp inc=\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_CommandCacheStatisticsReset.cpp inc=ggg00,hgg01_3.h,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_CommandCacheStatisticsReset.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_SQLCache.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_CommandCacheStatistics.hpp,\
    :SAPDB/SQLManager/SQLMan_ContextSessionMembers.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,:SAPDB/RunTime/RTE_KGS.hpp
SysView_CommandCacheStatisticsReset.hpp inc=\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_CommandStatistics.cpp inc=ggg00,hgg01_3.h,hgg03.h,hsp78.h,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_CommandStatistics.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_SQLCache.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_Command.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_Types.hpp,\
    :SAPDB/SQLManager/SQLMan_ContextSessionMembers.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp
SysView_CommandStatistics.hpp inc=:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_CommandStatisticsReset.cpp inc=ggg00,hgg01_3.h,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_CommandStatisticsReset.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_SQLCache.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_Command.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_Types.hpp,\
    :SAPDB/SQLManager/SQLMan_ContextSessionMembers.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,:SAPDB/RunTime/RTE_KGS.hpp
SysView_CommandStatisticsReset.hpp inc=\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_DataCache.cpp inc=ggg00,:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_DataCache.hpp,\
    :SAPDB/FrameControl/FrameCtrl_ICommon.hpp,hbd20_10.h
SysView_DataCache.hpp inc=:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_DataStatistics.cpp inc=ggg00,hbd20_6.h,\
    :SAPDB/Converter/Converter_ICommon.hpp,\
    :SAPDB/FreeBlockManagement/FBM_IManager.hpp,\
    :SAPDB/FrameControl/FrameCtrl_ICommon.hpp,\
    :SAPDB/GarbageCollector/GC_IGCController.hpp,\
    :SAPDB/IOManager/IOMan_IDataInfo.hpp,\
    :SAPDB/KernelCommon/Kernel_DateTime.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_DataStatistics.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp
SysView_DataStatistics.hpp inc=\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_DataStatisticsReset.cpp inc=ggg00,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_DataStatisticsReset.hpp,\
    :SAPDB/Converter/Converter_ICommon.hpp,\
    :SAPDB/FrameControl/FrameCtrl_ICommon.hpp,\
    :SAPDB/IOManager/IOMan_IDataInfo.hpp
SysView_DataStatisticsReset.hpp inc=\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_DataVolumes.cpp inc=gsp00,ggg00,\
    :SAPDB/FrameControl/FrameCtrl_ICommon.hpp,\
    :SAPDB/FreeBlockManagement/FBM_IManager.hpp,\
    :SAPDB/IOManager/IOMan_IDataVolumeInfo.hpp,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_DataVolumes.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp
SysView_DataVolumes.hpp inc=:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_Defines.hpp inc=:SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp
SysView_FileDirectories.cpp inc=:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_FileDirectories.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/FileDirectory/FileDir_ISharedDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_IDirectory.hpp,\
    :SAPDB/Transaction/Trans_Iterator.hpp,\
    :SAPDB/Transaction/Trans_Context.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/Container/Container_List.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,ggg91.h
SysView_FileDirectories.hpp inc=:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/Container/Container_List.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
SysView_Files.cpp inc=ggg00,:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_Files.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/SQLMan_Types.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_TableDescriptor.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_IndexDescriptor.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsKeyPartition.hpp,\
    :SAPDB/FileDirectory/FileDir_ShortColumn.hpp,\
    :SAPDB/FileDirectory/FileDir_Index.hpp,\
    :SAPDB/FileDirectory/FileDir_Table.hpp,\
    :SAPDB/FileDirectory/FileDir_Generic.hpp,\
    :SAPDB/FileDirectory/FileDir_Iterator.hpp,\
    :SAPDB/FileDirectory/FileDir_ISharedDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_IDirectory.hpp,\
    :SAPDB/Transaction/Trans_Iterator.hpp,\
    :SAPDB/Transaction/Trans_Context.hpp,:SAPDB/Messages/Msg_List.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/FrameControl/FrameCtrl_ICommon.hpp,\
    :SAPDB/Container/Container_List.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_string.h,:SAPDB/SAPDBCommon/SAPDB_Types.h,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp
SysView_Files.hpp inc=:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,:SAPDB/Container/Container_List.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,:SAPDB/RunTime/RTE_Types.hpp
SysView_HistoryCleanupStatistics.cpp inc=ggg00,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_HistoryCleanupStatistics.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp
SysView_HistoryCleanupStatistics.hpp inc=\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_HistoryCleanupStatisticsReset.cpp inc=ggg00,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_HistoryCleanupStatisticsReset.hpp
SysView_HistoryCleanupStatisticsReset.hpp inc=\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_HotStandbyComponent.cpp inc=ggg00,\
    :SAPDB/SystemViews/SysView_HotStandbyComponent.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,:SAPDB/RunTime/RTE_KGS.hpp,\
    :SAPDB/RunTime/HotStandbyStorage/RTEHSS_KernelInterface.hpp,\
    :SAPDB/RunTime/System/RTESys_Time.h
SysView_HotStandbyComponent.hpp inc=\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_HotStandbyGroup.cpp inc=ggg00,\
    :SAPDB/SystemViews/SysView_HotStandbyGroup.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/RunTime/HotStandbyStorage/RTEHSS_StandbyNodeList.hpp,\
    :SAPDB/RunTime/System/RTESys_Time.h
SysView_HotStandbyGroup.hpp inc=:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_IOBufferCaches.cpp inc=ggg00,\
    :SAPDB/SystemViews/SysView_IOBufferCaches.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/FrameControl/FrameCtrl_ICommon.hpp
SysView_IOBufferCaches.hpp inc=:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_IOThreadStatistics.cpp inc=ggg00,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_IOThreadStatistics.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,geo00_0.h,geo50_0.h,geo002.h,\
    :SAPDB/SAPDBCommon/SAPDB_sprintf.h,:SAPDB/RunTime/RTE_KGS.hpp
SysView_IOThreadStatistics.hpp inc=\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_IOThreadStatisticsReset.cpp inc=ggg00,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_IOThreadStatisticsReset.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,geo00_0.h,geo50_0.h,geo002.h,\
    :SAPDB/SAPDBCommon/SAPDB_sprintf.h,:SAPDB/RunTime/RTE_KGS.hpp
SysView_IOThreadStatisticsReset.hpp inc=\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_ISysView.hpp inc=
SysView_ITableObj.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SystemViews/SysView_Defines.hpp
SysView_IndexStorageDetails.cpp inc=ggg00,hak40.h,hbd03.h,hak722.h,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_IndexStorageDetails.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_TableDescriptor.hpp,\
    :SAPDB/FileDirectory/FileDir_IDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_Table.hpp,\
    :SAPDB/FileDirectory/FileDir_Index.hpp
SysView_IndexStorageDetails.hpp inc=\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp
SysView_Instance.cpp inc=ggg00,:SAPDB/SystemViews/SysView_Instance.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,:SAPDB/RunTime/RTE_KGS.hpp,\
    :SAPDB/RunTime/System/RTESys_Time.h
SysView_Instance.hpp inc=:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_KernelMessages.cpp inc=ggg00,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_KernelMessages.hpp,\
    :SAPDB/SQLManager/ErrorHandling/SQLManErr_Interface.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_AllocatorInfoReader.hpp
SysView_KernelMessages.hpp inc=:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_LockStatistics.cpp inc=ggg00,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_LockStatistics.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp
SysView_LockStatistics.hpp inc=:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_LockStatisticsReset.cpp inc=ggg00,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_LockStatisticsReset.hpp
SysView_LockStatisticsReset.hpp inc=\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_Locks.cpp inc=ggg00,hgg01_3.h,hak01.h,hak06.h,hak071.h,hak36.h,\
    hak40.h,hak42.h,vkb521.hpp,:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_Locks.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Interface.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp
SysView_Locks.hpp inc=ggg00,:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_LogInformation.cpp inc=ggg00,:SAPDB/SAPDBCommon/SAPDB_string.h,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_LogInformation.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/Logging/Log_Statistics.hpp
SysView_LogInformation.hpp inc=:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_LogQueueStatistics.cpp inc=ggg00,\
    :SAPDB/SAPDBCommon/SAPDB_string.h,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_LogQueueStatistics.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,:SAPDB/Logging/Log_Volume.hpp,\
    :SAPDB/Logging/Log_QueueStatistics.hpp
SysView_LogQueueStatistics.hpp inc=\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_LogQueueStatisticsReset.cpp inc=ggg00,\
    :SAPDB/SAPDBCommon/SAPDB_string.h,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_LogQueueStatisticsReset.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,:SAPDB/Logging/Log_Volume.hpp,\
    :SAPDB/Logging/Log_QueueStatistics.hpp,:SAPDB/RunTime/RTE_KGS.hpp
SysView_LogQueueStatisticsReset.hpp inc=\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_LogStatistics.cpp inc=ggg00,:SAPDB/SAPDBCommon/SAPDB_string.h,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_LogStatistics.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/Logging/Log_Statistics.hpp
SysView_LogStatistics.hpp inc=:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_LogStatisticsReset.cpp inc=ggg00,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_LogStatisticsReset.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/Logging/Log_Statistics.hpp,:SAPDB/Logging/Log_Volume.hpp,\
    :SAPDB/RunTime/RTE_KGS.hpp
SysView_LogStatisticsReset.hpp inc=\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_LogVolumes.cpp inc=:SAPDB/FrameControl/FrameCtrl_ICommon.hpp,\
    :SAPDB/IOManager/IOMan_ILogVolumeInfo.hpp,\
    :SAPDB/IOManager/IOMan_ILogInfo.hpp,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_LogVolumes.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp
SysView_LogVolumes.hpp inc=:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_MachineConfiguration.cpp inc=ggg00,\
    :SAPDB/SystemViews/SysView_MachineConfiguration.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/RunTime/System/RTESys_SystemInfo.h,\
    :SAPDB/RunTime/System/RTESys_MemoryInfo.hpp
SysView_MachineConfiguration.hpp inc=\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_MachineUtilization.cpp inc=ggg00,\
    :SAPDB/SystemViews/SysView_MachineUtilization.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/RunTime/System/RTESys_SystemInfo.h,\
    :SAPDB/RunTime/System/RTESys_MemoryInfo.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_sprintf.h
SysView_MachineUtilization.hpp inc=\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_MemoryAllocatorStatistics.cpp inc=ggg00,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_MemoryAllocatorStatistics.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_AllocatorInfoReader.hpp
SysView_MemoryAllocatorStatistics.hpp inc=\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_MemoryAllocatorStatisticsReset.cpp inc=ggg00,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_MemoryAllocatorStatisticsReset.hpp
SysView_MemoryAllocatorStatisticsReset.hpp inc=\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_MemoryHolders.cpp inc=ggg00,hgg01_3.h,hsp77.h,hak40.h,\
    :SAPDB/liveCache/MemoryManagement/LVCMem_Allocator.hpp,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_MemoryHolders.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp
SysView_MemoryHolders.hpp inc=:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_Miscellaneous.cpp inc=ggg00,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_Miscellaneous.hpp
SysView_Miscellaneous.hpp inc=:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_MultiVersionReadStatistics.cpp inc=ggg00,gsp100.h,hsp100.h,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_MultiVersionReadStatistics.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,hkb51.h
SysView_MultiVersionReadStatistics.hpp inc=\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp
SysView_OMSDiagnose.cpp inc=ggg00,:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_OMSDiagnose.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,hak341.h
SysView_OMSDiagnose.hpp inc=:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_OMSLocks.cpp inc=ggg00,gsp00,vak001,\
    :SAPDB/liveCache/LVC_LibOmsInterface.hpp,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_OMSLocks.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp
SysView_OMSLocks.hpp inc=:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_OMSProcedureStatistics.cpp inc=ggg00,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_OMSProcedureStatistics.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp
SysView_OMSProcedureStatistics.hpp inc=\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_OMSProcedureStatisticsReset.cpp inc=ggg00,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_OMSProcedureStatisticsReset.hpp
SysView_OMSProcedureStatisticsReset.hpp inc=\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_OMSVersions.cpp inc=ggg00,:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_OMSVersions.hpp
SysView_OMSVersions.hpp inc=:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_ObjectLocks.cpp inc=ggg00,hkb52.h,hkb501_1.h,hgg06.h,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_ObjectLocks.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp
SysView_ObjectLocks.hpp inc=:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_OptimizerInformation.cpp inc=vak001,hak47.h,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_OptimizerInformation.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp
SysView_OptimizerInformation.hpp inc=\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_PageLocks.cpp inc=ggg00,:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_PageLocks.hpp
SysView_PageLocks.hpp inc=:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_PascalTableObj.cpp inc=ggg00,hgg01_3.h,ggg91.h,ggg92.h,hgg06.h,\
    gak40,hak40.h,hak42.h,vak001,hsp81.h,hak06.h,hak07.h,gkb05,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_UTF8Basis.hpp,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp,\
    :SAPDB/SystemViews/SysView_PascalTableObj.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/SQLMan_ContextSessionMembers.hpp
SysView_PascalTableObj.hpp inc=ggg00,gak40,vak001,gkb05,\
    :SAPDB/SAPDBCommon/SAPDB_UTF8Basis.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_PseudoLongHandler.cpp inc=\
    :SAPDB/SystemViews/SysView_PseudoLongHandler.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,hak101.h,hgg04.h,hbd02.h
SysView_PseudoLongHandler.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp
SysView_ReaderWriterLockInformation.cpp inc=ggg00,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_ReaderWriterLockInformation.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_RWRegion.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_sprintf.h,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_SpinlockPoolRegister.hpp
SysView_ReaderWriterLockInformation.hpp inc=\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_ReaderWriterLockStatistics.cpp inc=ggg00,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_ReaderWriterLockStatistics.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_RWRegion.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_sprintf.h,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_SpinlockPoolRegister.hpp,\
    :SAPDB/RunTime/System/RTESys_MicroTime.h
SysView_ReaderWriterLockStatistics.hpp inc=\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_ReaderWriterLockStatisticsReset.cpp inc=ggg00,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_ReaderWriterLockStatisticsReset.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_RWRegion.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_sprintf.h,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_SpinlockPoolRegister.hpp,\
    :SAPDB/RunTime/RTE_KGS.hpp,\
    :SAPDB/SystemViews/SysView_ReaderWriterLockStatistics.hpp
SysView_ReaderWriterLockStatisticsReset.hpp inc=\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_ReaderWriterLockWaitingTasks.cpp inc=ggg00,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_ReaderWriterLockWaitingTasks.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_RWRegion.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_sprintf.h,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_SpinlockPoolRegister.hpp
SysView_ReaderWriterLockWaitingTasks.hpp inc=\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_RegionInformation.cpp inc=ggg00,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_RegionInformation.hpp,geo002.h,geo50_0.h
SysView_RegionInformation.hpp inc=:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_RegionLocks.cpp inc=ggg00,:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_RegionLocks.hpp
SysView_RegionLocks.hpp inc=:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_RegionStatistics.cpp inc=ggg00,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_RegionStatistics.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,geo002.h,geo50_0.h,\
    :SAPDB/RunTime/System/RTESys_MicroTime.h
SysView_RegionStatistics.hpp inc=:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_RegionStatisticsReset.cpp inc=ggg00,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_RegionStatisticsReset.hpp,geo002.h,\
    geo50_0.h,:SAPDB/SystemViews/SysView_RegionStatistics.hpp,\
    :SAPDB/RunTime/RTE_KGS.hpp
SysView_RegionStatisticsReset.hpp inc=\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_RestartInformation.cpp inc=ggg00,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_RestartInformation.hpp,\
    :SAPDB/Logging/Log_Savepoint.hpp,:SAPDB/Logging/Log_Volume.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/Transaction/Trans_Context.hpp
SysView_RestartInformation.hpp inc=\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_Schemas.cpp inc=ggg00,:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_Schemas.hpp
SysView_Schemas.hpp inc=:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_Servertasks.cpp inc=ggg00,:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_Servertasks.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/ServerTasks/SrvTasks_ICoordinator.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp
SysView_Servertasks.hpp inc=:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_Sessions.cpp inc=ggg00,hgg01_3.h,hgg03.h,hsp78.h,geo003.h,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_Sessions.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/SQLMan_SessionIterator.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp,\
    :SAPDB/Container/Container_List.hpp,\
    :SAPDB/FileDirectory/FileDir_Generic.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,:SAPDB/Messages/Msg_List.hpp
SysView_Sessions.hpp inc=:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_Snapshots.cpp inc=:SAPDB/Converter/Converter_ICommon.hpp,\
    :SAPDB/FrameControl/FrameCtrl_ICommon.hpp,\
    :SAPDB/KernelCommon/Kernel_DateTime.hpp,:SAPDB/RunTime/RTE_Types.h,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_Snapshots.hpp
SysView_Snapshots.hpp inc=:SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_SpinLockPoolStatistics.cpp inc=ggg00,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_SpinLockPoolStatistics.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_SpinlockPoolRegister.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/RunTime/System/RTESys_MicroTime.h
SysView_SpinLockPoolStatistics.hpp inc=\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_SpinLockPoolStatisticsReset.cpp inc=ggg00,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_SpinLockPoolStatisticsReset.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_SpinlockPoolRegister.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,:SAPDB/RunTime/RTE_KGS.hpp,\
    :SAPDB/SystemViews/SysView_SpinLockPoolStatistics.hpp
SysView_SpinLockPoolStatisticsReset.hpp inc=\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_SpinLockStatistics.cpp inc=ggg00,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_SpinLockStatistics.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_SpinlockReader.hpp,\
    :SAPDB/RunTime/System/RTESys_MicroTime.h
SysView_SpinLockStatistics.hpp inc=\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_SpinLockStatisticsReset.cpp inc=ggg00,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_SpinLockStatisticsReset.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_SpinlockReader.hpp,\
    :SAPDB/SystemViews/SysView_SpinLockStatistics.hpp,\
    :SAPDB/RunTime/RTE_KGS.hpp
SysView_SpinLockStatisticsReset.hpp inc=\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_SysViewBase.cpp inc=ggg00,vak001,gak40,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_PascalTableObj.hpp,\
    :SAPDB/RunTime/System/RTESys_Time.h,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Types.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_DBProcDescriptor.hpp
SysView_SysViewBase.hpp inc=\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SystemViews/SysView_ISysView.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_TableStorageDetails.cpp inc=ggg00,hak40.h,hbd01.h,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_TableStorageDetails.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_TableDescriptor.hpp,\
    :SAPDB/FileDirectory/FileDir_IDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_Table.hpp
SysView_TableStorageDetails.hpp inc=\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp
SysView_TaskGroupStatistics.cpp inc=ggg00,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_TaskGroupStatistics.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,geo50_0.h,heo92.h,\
    :SAPDB/RunTime/System/RTESys_MicroTime.h,\
    :SAPDB/RunTime/Tasking/RTETask_TaskScheduler.hpp
SysView_TaskGroupStatistics.hpp inc=\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_TaskGroupStatisticsReset.cpp inc=ggg00,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_TaskGroupStatisticsReset.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,geo50_0.h,\
    :SAPDB/RunTime/System/RTESys_MicroTime.h,\
    :SAPDB/RunTime/Tasking/RTETask_TaskScheduler.hpp
SysView_TaskGroupStatisticsReset.hpp inc=\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_TaskLoadBalancingInformation.cpp inc=ggg00,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_TaskLoadBalancingInformation.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_LoadBalancingInfo.hpp
SysView_TaskLoadBalancingInformation.hpp inc=\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_TaskLoadBalancingTaskGroupStates.cpp inc=ggg00,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_TaskLoadBalancingTaskGroupStates.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_LoadBalancingInfo.hpp
SysView_TaskLoadBalancingTaskGroupStates.hpp inc=\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_TaskLoadBalancingTaskMoves.cpp inc=ggg00,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_TaskLoadBalancingTaskMoves.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_LoadBalancingInfo.hpp
SysView_TaskLoadBalancingTaskMoves.hpp inc=\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_TaskStatistics.cpp inc=:SAPDB/RunTime/RTE_Types.h,ggg00,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_TaskStatistics.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_sprintf.h,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_Task.hpp
SysView_TaskStatistics.hpp inc=:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_TaskStatisticsReset.cpp inc=ggg00,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_TaskStatisticsReset.hpp
SysView_TaskStatisticsReset.hpp inc=\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_TransactionHistory.cpp inc=ggg00,hak07.h,hgg01.h,hgg01_3.h,\
    ggg91.h,ggg92.h,hgg06.h,:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_TransactionHistory.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/Transaction/Trans_Iterator.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/Transaction/Trans_ExtendedMonitoringIter.hpp
SysView_TransactionHistory.hpp inc=\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_Transactions.cpp inc=ggg00,hak07.h,hgg01.h,ggg91.h,ggg92.h,\
    hgg06.h,:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_Transactions.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/Transaction/Trans_Iterator.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/Logging/Log_Types.hpp,:SAPDB/Logging/Log_Transaction.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_Task.hpp
SysView_Transactions.hpp inc=:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_UnloadedStatements.cpp inc=ggg00,hgg01_3.h,hgg03.h,hsp78.h,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_UnloadedStatements.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_SQLCache.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_Command.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_Types.hpp,\
    :SAPDB/SQLManager/SQLMan_ContextSessionMembers.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp
SysView_UnloadedStatements.hpp inc=\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp
SysView_Version.cpp inc=ggg00,gsp100.h,hsp100.h,\
    :SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp,\
    :SAPDB/SystemViews/SysView_Version.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/RunTime/System/RTESys_SystemInfo.h
SysView_Version.hpp inc=:SAPDB/SystemViews/SysView_Defines.hpp,\
    :SAPDB/SystemViews/SysView_SysViewBase.hpp,\
    :SAPDB/SystemViews/SysView_ITableObj.hpp

