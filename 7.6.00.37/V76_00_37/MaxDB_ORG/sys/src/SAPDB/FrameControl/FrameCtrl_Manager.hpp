/*!
  @file     FrameCtrl_Manager.hpp
  @author   TorstenS
  @ingroup  FrameControl
  @brief    This class is used to handle i/o capable memory request

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



#ifndef FRAMECTRL_MANAGER_HPP
#define FRAMECTRL_MANAGER_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

// Interfaces
#include "FrameControl/FrameCtrl_ICommon.hpp"
#include "FrameControl/FrameCtrl_IConverter.hpp"
#include "FrameControl/FrameCtrl_IIOManager.hpp"
#include "FrameControl/FrameCtrl_ILog.hpp"
#include "FrameControl/FrameCtrl_IDataCache.hpp"
#include "FrameControl/FrameCtrl_IFileDirectory.hpp"
#include "FrameControl/FrameCtrl_IRestartPage.hpp"
#include "FrameControl/FrameCtrl_IBlockAllocator.hpp"
#include "FrameControl/FrameCtrl_ICreateIndex.hpp"

#include "FrameControl/FrameCtrl_Types.hpp"
#include "KernelCommon/Kernel_Common.hpp"
#include "DataAccess/Data_PageFrame.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
    @class    FrameCtrl_Manager
    @brief    This class is used to handle i/o capable memory request
 */

class FrameCtrl_Manager :
            public FrameCtrl_ICommon,
            public FrameCtrl_IConverter,
            public FrameCtrl_IDataCache,
            public FrameCtrl_IFileDirectory,
            public FrameCtrl_IIOManager,
            public FrameCtrl_ILog,
            public FrameCtrl_IRestartPage,
            public FrameCtrl_IBlockAllocator,
            public FrameCtrl_ICreateIndex
{
    friend class FrameCtrl_ICommon;
    friend class FrameCtrl_IConverter;
    friend class FrameCtrl_IDataCache;
    friend class FrameCtrl_IFileDirectory;
    friend class FrameCtrl_IIOManager;
    friend class FrameCtrl_ILog;
    friend class FrameCtrl_IRestartPage;
    friend class FrameCtrl_IBlockAllocator;
    friend class FrameCtrl_ICreateIndex;

public:

    /*!
       @brief   This method creates the singleton frame control.It is assumed 
                that this method will be callled only one time by the timeout 
                task and therefore no synchonization mechanism is used.
       @return  true means instance could be created; else false
     */

    static bool CreateInstance();

    SAPDB_UInt GetFreeFrameCount() const;

    SAPDB_UInt GetPageSize() const{
        return m_PageSize ;
    }

    SAPDB_UInt GetCacheSize() const{
        return m_Capacity ;
    }

    SAPDB_UInt GetConverterSize() const{
        return m_UsedByConverter;
    }

    SAPDB_UInt GetDataCacheSize() const{
        return m_UsedByDataCache;
    }

    SAPDB_UInt GetLogQueueSize() const{
        return m_UsedByLogQueue;
    }

    SAPDB_UInt GetIOManagementSize() const{
        return m_UsedByIOManagement;
    }

    SAPDB_UInt GetFileDirSize() const{
        return m_UsedByFileDirectory;
    }

    SAPDB_UInt GetRestartRecordSize() const{
        return m_UsedByRestartRecord;
    }

    SAPDB_UInt GetBlockAllocatorSize() const{
        return m_UsedByBlockAllocator;
    }

    SAPDB_UInt GetCreateIndexSize() const{
        return m_UsedByCreateIndex;
    }

    Data_PageFrame NewConvFrame(
        const tsp00_TaskId	taskId,
        const SAPDB_Bool	bStopIfMemoryExhausted = true )
    {
        return( NewFrame( FrameCtrl_ConverterKey, taskId, bStopIfMemoryExhausted ));
    }

    void FreeConvFrame(
        const tsp00_TaskId      taskId,
        Data_PageFrame          &frame,
        const SAPDB_Bool        bIsShutdown = true )
    {
        Free( FrameCtrl_ConverterKey, taskId, bIsShutdown, frame );
    }

    Data_PageFrame NewIOManFrame(
        const tsp00_TaskId  taskId,
        const SAPDB_Bool    bStopIfMemoryExhausted = true )
    {
        return( NewFrame( FrameCtrl_IOManagementKey,
                          taskId, bStopIfMemoryExhausted ));
    }

    void FreeIOManFrame(
        const tsp00_TaskId      taskId,
        Data_PageFrame          &frame,
        const SAPDB_Bool        bIsShutdown = true )
    {
        Free( FrameCtrl_IOManagementKey, taskId, bIsShutdown, frame );
    }

    Data_PageFrame NewLogFrame(
        const tsp00_TaskId  taskId,
        const SAPDB_Bool    bStopIfMemoryExhausted = true )
    {
        return( NewFrame( FrameCtrl_LogKey, taskId, bStopIfMemoryExhausted ));
    }

    void FreeLogFrame(
        const tsp00_TaskId      taskId,
        Data_PageFrame          &frame,
        const SAPDB_Bool        bIsShutdown = true )
    {
        Free( FrameCtrl_LogKey, taskId, bIsShutdown, frame );
    }

    Data_PageFrame NewDataFrame( const tsp00_TaskId taskId )
    {
        return( NewFrame( FrameCtrl_DataCacheKey, taskId, true ));
    }

    void FreeDataFrame(
        const tsp00_TaskId  taskId,
        Data_PageFrame      &frame )
    {
        Free( FrameCtrl_DataCacheKey, taskId, false, frame );
    }

    Data_PageFrame NewFDirFrame(
        const tsp00_TaskId  taskId,
        const SAPDB_Bool    bStopIfMemoryExhausted = true )
    {
        return( NewFrame( FrameCtrl_FileDirectoryKey, taskId, bStopIfMemoryExhausted ));
    }

    void FreeFDirFrame(
        const tsp00_TaskId      taskId,
        Data_PageFrame          &frame,
        const SAPDB_Bool        bIsShutdown = true )
    {
        Free( FrameCtrl_FileDirectoryKey, taskId, bIsShutdown, frame );
    }

    Data_PageFrame NewRestartPageFrame(
        const tsp00_TaskId      taskId,
        const SAPDB_Bool        bStopIfMemoryExhausted = true )
    {
        return( NewFrame( FrameCtrl_RestartPageKey, taskId,
                          bStopIfMemoryExhausted ));
    }

    void FreeRestartPageFrame(
        const tsp00_TaskId      taskId,
        Data_PageFrame          &frame,
        const SAPDB_Bool        bIsShutdown = true )
    {
        Free( FrameCtrl_RestartPageKey, taskId, bIsShutdown, frame );
    }

    Data_PageFrame NewBlockAllocatorFrame(
        const tsp00_TaskId      taskId,
        const SAPDB_Bool        bStopIfMemoryExhausted = true )
    {
        return( NewFrame( FrameCtrl_BlockAllocatorKey, taskId,
                          bStopIfMemoryExhausted ));
    }

    void FreeBlockAllocatorFrame(
        const tsp00_TaskId      taskId,
        Data_PageFrame          &frame,
        const SAPDB_Bool        bIsShutdown = true )
    {
        Free( FrameCtrl_BlockAllocatorKey, taskId, bIsShutdown, frame );
    }

    Data_PageFrame NewIndexFrame( RTETask_ITask&  task ){
        return( this->NewFrame( FrameCtrl_CreateIndexKey, task.ID(), false ));
    }

    void FreeIndexFrame(
        RTETask_ITask&  task,
        Data_PageFrame& frame )
    {
        this->Free( FrameCtrl_CreateIndexKey, task.ID(), false, frame );
    }

    // methods of SAPDBMem_IBlockAllocator

    SAPDB_ULong GetBlockSize() const{
        return m_PageSize;
    }

    SAPDB_ULong GetAlignmentSize() const{
        return 8;
    }

    void* Allocate(SAPDB_ULong BlockCount);

    void Deallocate(
        void*       pFrame,
        SAPDB_ULong BlockCount );

    // methods of SAPDBMem_IAllocatorInfo

    void GetBaseAllocatorCallStatistics(
        SAPDB_ULong &CountAlloc,
        SAPDB_ULong &CountDealloc) const
    {
        CountAlloc   = 0;
        CountDealloc = 0;
    }

    void GetCallStatistics(
        SAPDB_ULong &CountAlloc,
        SAPDB_ULong &CountDealloc ) const
    {
        CountAlloc   = 0;
        CountDealloc = 0;
    }

    const SAPDB_UTF8 *GetIdentifier() const{
        return (SAPDB_UTF8*) "FrameCtrlBlockAllocator";
    }

protected:

    /// protected and not private to avoid warnings on linux!

    FrameCtrl_Manager();

    /*!
       @brief   This method is used to initialize the component, in particular
                the number of i/o capable memory frames is requested from the RTE.
       @return  none
     */

    void Initialize( )
    {
        m_Capacity = GetFreeFrameCount();
    }

private:

    // Copy and assignment operator are not supported

    FrameCtrl_Manager& operator=( const FrameCtrl_Manager &manager );

    FrameCtrl_Manager( const FrameCtrl_Manager &manager );

    static FrameCtrl_Manager& GetInstance(){ return( *m_Instance ); }

    void MaintainInternalCounter(
        const FrameCtrl_RegistrationKey registrationKey,
        const SAPDB_Bool                bIncrementCounter = true );

    Data_PageFrame NewFrame(
        const FrameCtrl_RegistrationKey   registrationKey,
        const tsp00_TaskId                taskId,
        const SAPDB_Bool                  bStopIfMemoryExhausted )
    {
        Data_PageFrame  frame;
        this->New( registrationKey, taskId, bStopIfMemoryExhausted, frame );
        return( frame );
    }

    void New(
        const FrameCtrl_RegistrationKey   registrationKey,
        const tsp00_TaskId                taskId,
        const SAPDB_Bool                  bStopIfMemoryExhausted,
        Data_PageFrame                    &frame );

    void Free(
        const FrameCtrl_RegistrationKey   registrationKey,
        const tsp00_TaskId                taskId,
        const SAPDB_Bool                  bIsShutdown,
        Data_PageFrame                    &frame );

private:


    // Reference to the proper FrameControl Manager
    static FrameCtrl_Manager    *m_Instance;

    // Size in bytes of the pages managed by the page allocator
    const SAPDB_UInt m_PageSize;


    // Initial number of frames stored within the page allocator
    SAPDB_UInt m_Capacity;

    // Number of page frames checked out by the component converter
    SAPDB_UInt m_UsedByConverter;

    // Number of page frames checked out by the component data cache
    SAPDB_UInt m_UsedByDataCache;

    // Number of page frames checked out by the component log queue
    SAPDB_UInt m_UsedByLogQueue;

    // Number of page frames checked out by the component I/O Management
    SAPDB_UInt m_UsedByIOManagement;

    // Number of page frames checked out by the component file directory
    SAPDB_UInt m_UsedByFileDirectory;

    // Number of page frames checked out by the component restart
    SAPDB_UInt m_UsedByRestartRecord;

    // Number of page frames checked out by the block allocator
    SAPDB_UInt m_UsedByBlockAllocator;

    // Number of page frames checked out by the parallel index creation
    SAPDB_UInt m_UsedByCreateIndex;
};


#endif  /* FRAMECTRL_MANAGER_HPP */
