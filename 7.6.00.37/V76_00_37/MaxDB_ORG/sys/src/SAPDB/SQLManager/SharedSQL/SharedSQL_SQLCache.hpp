/*!
    @file     SharedSQL_SQLCache.hpp
    @ingroup  SharedSQL
    @author   DirkT
    @brief    SQLCache for calling SharedSQL from SQLManager
    @see            

\if EMIT_LICENCE
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

\endif
*/

#ifndef SharedSQL_SQLCache_HPP
#define SharedSQL_SQLCache_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDB/SQLManager/SharedSQL/SharedSQL_CommandCacheStatistics.hpp"
#include "SAPDB/SQLManager/SharedSQL/SharedSQL_CommandCacheIterator.hpp"
#include "SAPDB/SQLManager/SharedSQL/SharedSQL_ParseIDCache.hpp"
#include "SAPDB/SQLManager/SharedSQL/SharedSQL_Handles.hpp"
#include "SAPDB/SQLManager/SharedSQL/SharedSQL_Types.hpp"

#include "SAPDB/SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

class SAPDBMem_RawAllocator;

/*---------------------------------------------------------------------------*/
/*!
    @class          SharedSQL_SQLCache
    @brief          Used by transactions to handle the sigleton commandcache and a session specific parseid cache
    @copydoc        SAPDBMem_IRawAllocator
 */
class SharedSQL_SQLCache 
{
public:
    /*!
        @brief constructor
        @param Alloc [in] - Allocator to be used by the session specific parseid cache
    */
    SharedSQL_SQLCache( SAPDBMem_IRawAllocator& Alloc );

    /*!
        @brief destructor - does nothing
    */
    ~SharedSQL_SQLCache( void );

    /*!
        @brief  Get an interface to a SharedSQL_PrepareHandle for some SharedSQL_CachedCommand
        @param  Alloc      [in]
        @param  Context    [in]
        @param  Stmt       [in]
        @param  Internal   [in]
		@param  ParseAgain [in]
        @param  MsgList    [out]
        @return SharedSQL_PrepareHandle*
    */
    /*virtual*/ SharedSQL_PrepareHandle* NewPrepareHandle( SAPDBMem_IRawAllocator& Alloc, SharedSQL_SQLContext& Context, SharedSQL_SQLStmt& Stmt, SAPDB_Bool Internal, SAPDB_Bool ParseAgain, Msg_List& MsgList );
    /*!
        @brief  Destroy the Handle created with NewPrepareHandle
        @param  PH [in]
    */
    /*virtual*/ void DestroyPrepareHandle( SharedSQL_PrepareHandle* PH );

    /*!
        @brief  Get an interface to a SharedSQL_ExecuteHandle for some ParseID
        @param  Alloc          [in]
        @param  PID            [in]
        @param  CountExecuting [in]
        @param  Valid          [out]
        @param  M9             [out]
        @param  pModulename    [out]
        @param  MsgList        [out]
        @return SharedSQL_ExecuteHandle*
    */
    /*virtual*/ SharedSQL_ExecuteHandle* NewExecuteHandle( SAPDBMem_IRawAllocator& Alloc, const SharedSQL_ParseID& PID, bool CountExecuting, SAPDB_Bool& Valid, SAPDB_Bool& M9, const SharedSQL_ModuleName* &pModulename, Msg_List& MsgList );
    /*!
        @brief  Destroy the Handle created with NewExecuteHandle
        @param  EH [in]
    */
    /*virtual*/ void DestroyExecuteHandle( SharedSQL_ExecuteHandle* EH );

    /*!
        @brief  Add parse and modulename for some command identified its prepare handle
        @param  PH          [in]
        @param  PID         [in]
        @param  ModuleName  [in]
        @param  M9          [in]
        @param  MsgList     [out]
        @return true, if ok, else false
    */
    /*virtual*/ SAPDB_Bool PutParseID( const SharedSQL_PrepareHandle* PH, const SharedSQL_ParseID& PID, const SharedSQL_ModuleName& ModuleName, const SAPDB_Bool M9, Msg_List& MsgList );
    /*!
        @brief  Drop some parseid previously added with PutParseID
        @param  PID [in]
        @return true, if ok, else false
    */
    /*virtual*/ SAPDB_Bool DropParseID( const SharedSQL_ParseID& PID );
    /*!
        @brief  Get the module name for some command identified by parseid
        @param  PID [in]
        @return const SharedSQL_ModuleName*
    */
    /*virtual*/ const SharedSQL_ModuleName* GetModuleName( const SharedSQL_ParseID& PID );

    /*!
        @brief  Free as much memory as possible by destroying unnecessary elements
    */
    /*virtual*/ void CleanUpAll( void );
    /*!
        @brief  Invalidate all commands within the cache
        @param  NotifyApplication [in]
    */
    /*virtual*/ void Invalidate( bool NotifyApplication );
    /*!
        @brief  Invalidate all commands within the cache that belong to the given schema
        @param  Schema [in]
        @param  NotifyApplication [in]
    */
    /*virtual*/ void Invalidate( SQLMan_Context& Context, SharedSQL_Schema Schema, bool NotifyApplication );
    /*!
        @brief  Invalidate all commands within the cache that belong to the given tables
        @param  Tables [in] 
        @param  NotifyApplication [in]
    */
    /*virtual*/ void Invalidate( SQLMan_Context& Context, SharedSQL_TableTree* Tables, bool NotifyApplication );

    /*!
        @brief  Set statistic data from execution for some command identified by parseid
        @param  PID [in]
        @param  CommandID [out]
        @param  ExecuteTimeSec [in]
        @param  ExecuteTimeMicroSec [in]
        @param  ReadRowCount [in]
        @param  QualifiedRowCount [in]
        @param  VirtualReadCount [in]
        @param  PhysicalReadCount [in]
        @param  FetchRowCount [in]
        @param  SuspendCount [in]
        @param  WaitCount [in]
        @return true if ok, else false
    */
    /*virtual*/ SAPDB_Bool SetExecuteStatisticData( const SharedSQL_ParseID& PID,
                                        SharedSQL_CommandID& CommandID,
                                        SAPDB_UInt8 ExecuteTimeSec, SAPDB_UInt8 ExecuteTimeMicroSec,
                                        SAPDB_Int8 ReadRowCount, SAPDB_Int8 QualifiedRowCount,
                                        SAPDB_Int8 VirtualReadCount, SAPDB_Int8 PhysicalReadCount,
                                        SAPDB_Int8 FetchRowCount, 
                                        SAPDB_Int8 SuspendCount, SAPDB_Int8 WaitCount );

    /*!
        @brief  Get information about the CommandCache
        @param  S [out]
    */
    /*virtual*/ void GetCommandCacheInfo( SharedSQL_CommandCacheInfo& S );
    /*!
        @brief  Get information about the CommandCache (reset version)
        @param  S [out]
    */
    /*virtual*/ void GetCommandCacheInfoReset( SharedSQL_CommandCacheInfo& S );
    /*!
        @brief  Reset statistics information of the cache
    */
    /*virtual*/ void ResetCommandCacheInfo( void );
    /*!
        @brief  Reset statistics information of all stored commands
    */
    /*virtual*/ void ResetCommandInfos( void );
    /*!
        @brief  Get an iterator for the cached commands
    */
    /*virtual*/ SharedSQL_CommandCacheIterator GetCommandCacheIterator( void );

    /*!
        @brief  ...
    */                                       
	void EnterLockForStatisticsIterator( void );
    /*!
        @brief  ...
    */                                       
	void LeaveLockForStatisticsIterator( void );
   
private:
    /// Session specific ParseIDCache
	SharedSQL_ParseIDCache	    mParseIDCache;
    //
};  // SharedSQL_SQLCache

#endif  /* SharedSQL_SQLCache_HPP */

