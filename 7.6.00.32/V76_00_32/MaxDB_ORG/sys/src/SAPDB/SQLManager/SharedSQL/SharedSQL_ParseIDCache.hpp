/*!
    @file     SharedSQL_ParseIDCache.hpp
    @ingroup  SharedSQL
    @author   DirkT
    @brief    Cache for ParseIDs, session specific
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


#ifndef SHAREDSQL_PARSEIDCACHE_HPP
#define SHAREDSQL_PARSEIDCACHE_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SQLManager/SharedSQL/SharedSQL_Command.hpp"
#include "SQLManager/SharedSQL/SharedSQL_Handles.hpp"
#include "SQLManager/SharedSQL/SharedSQL_Types.hpp"

#include "Container/Container_AVLTree.hpp"

#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"

#include "SAPDB/SAPDBCommon/Messages/SDBMsg_SharedSQL.h"

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*---------------------------------------------------------------------------*/
/*!
    @class          SharedSQL_ParseIDData
    @brief          Session specific parse data
 */
class SharedSQL_ParseIDData
{
public:
	/*!
        @brief  Constructor
    */
    SharedSQL_ParseIDData( void ) : mHandle(SharedSQL_ParseIDHandle()), mModuleName(SharedSQL_ModuleName()), mAllocPtr(0)
    {
    };
	/*!
        @brief  Destructor
    */
    ~SharedSQL_ParseIDData(void)
    {
        if (mAllocPtr && mModuleName.mName )
            mAllocPtr->Deallocate((void*)mModuleName.mName);
    };

    /*!
        @brief  Operator =
        @param  D [in]
        @return ...
    */
    inline SharedSQL_ParseIDData& operator=( const SharedSQL_ParseIDData& D ) 
    {
        if (mAllocPtr && mModuleName.mName )
            mAllocPtr->Deallocate((void*)mModuleName.mName);

        if (D.mAllocPtr && D.mModuleName.mName)
        {
            mAllocPtr           = D.mAllocPtr;
            mModuleName.mName   = (void*) mAllocPtr->Allocate(D.mModuleName.mSize);
            if (mModuleName.mName)
            {
                SAPDB_MemCopyNoCheck(mModuleName.mName, D.mModuleName.mName, D.mModuleName.mSize);
                mModuleName.mSize = D.mModuleName.mSize;
            }
        }
        else
        {
            mAllocPtr   = D.mAllocPtr;
            mModuleName.mSize   = 0;
            mModuleName.mName   = 0;
        }

        return *this;
    }

    /// ...
    SharedSQL_ParseIDHandle mHandle;
    /// ...
    SharedSQL_ModuleName    mModuleName;
    /// ...
    SAPDBMem_IRawAllocator* mAllocPtr;    
};  // SharedSQL_ParseIDData

/// ParseIDComparator - comparator to compare tree nodes
typedef Container_AVLNodeComparator<SharedSQL_ParseID> 
        SharedSQL_ParseIDComparator;

/// ParseIDTreeNode - combines the ParseID with the ParseIDData
typedef Container_AVLContentNode<SharedSQL_ParseID, SharedSQL_ParseIDData, SharedSQL_ParseIDComparator> 
        SharedSQL_ParseIDTreeNode;

/// ParseIDTree - AVL Tree to store the ParseID (as Key) and ParseIDData (as Data) in
typedef Container_AVLContentTree<SharedSQL_ParseIDTreeNode, SharedSQL_ParseID, SharedSQL_ParseIDData, SharedSQL_ParseIDComparator>
        SharedSQL_ParseIDTree;

/*---------------------------------------------------------------------------*/
/*!
    @class          SharedSQL_ParseIDCache
    @brief          Session specific cache to store SharedSQL_ParseIDData
 */
class SharedSQL_ParseIDCache 
{
public:
	/*!
        @brief  Constructor
        @param  Alloc [in] - Allocator to be used
    */
    SharedSQL_ParseIDCache( SAPDBMem_IRawAllocator& Alloc )
		: mParseIDComparator()
		, mParseIDTree(&mParseIDComparator, &Alloc)
		, mLocksForCommandCacheIterator(0)
    {
    };
	/*!
        @brief  Destructor
    */
    ~SharedSQL_ParseIDCache( void )
    {
        mParseIDTree.DeleteAll();
    };

    /*!
        @brief  Get an interface to a SharedSQL_ExecuteHandle for some ParseID
        @param  Alloc           [in]
        @param  PID             [in]
        @param  CountExecuting  [in]
        @param  Valid           [out]
        @param  M9              [out]
        @param  pModulename     [out]
        @param  MsgList         [out]
        @return SharedSQL_ExecuteHandle*
    */
    inline SharedSQL_ExecuteHandle* NewExecuteHandle( SAPDBMem_IRawAllocator& Alloc, SharedSQL_ParseID PID, bool CountExecuting, SAPDB_Bool& Valid, SAPDB_Bool& M9, const SharedSQL_ModuleName*& pModulename, Msg_List& MsgList )
    {    
        SAPDBTRACE_METHOD_DEBUG ("SharedSQL_ParseIDCache::GetExecuteHandle", SharedSQL_Trace, 5);

        if (const SharedSQL_ParseIDData* D = mParseIDTree.FindContent(PID))
        {
            SharedSQL_ExecuteHandle* EH = new(Alloc) SharedSQL_ExecuteHandle(&Alloc, D->mHandle, CountExecuting);
            SHARED_SQL_TRACE ("SharedSQL_CommandCache Cmd[" << ToStr(EH->mRef->GetCommandID()) << "]");
            Valid = EH->IsPlanValid(M9);
            pModulename = &(D->mModuleName);
            if (CountExecuting)
            {
                EH->mRef->IncCacheAccessCountExecute();
                if (Valid)
                    EH->mRef->IncCacheSuccessfulAccessCountExecute();
                else
                    EH->mRef->IncCacheUnSuccessfulAccessCountExecute();
            }
            return EH;
        }
        return 0;
    };

    /*!
        @brief  Destroy the Handle created with NewExecuteHandle
        @param  EH [in]
    */
    inline void DestroyExecuteHandle( SharedSQL_ExecuteHandle* EH )
    {
        SharedSQL_ExecuteHandle* Handle = REINTERPRET_CAST(SharedSQL_ExecuteHandle*, EH);
        destroy(Handle, Handle->GetAllocator());
        EH = 0;
    };

    /*!
        @brief  Add parse and modulename for some command identified its prepare handle
        @param  SH          [in]
        @param  PID         [in]
        @param  ModuleName  [in]
        @param  M9          [in]
        @param  MsgList     [out]
        @return true, if ok, else false
    */
    inline SAPDB_Bool PutParseID( const SharedSQL_PrepareHandle* SH, const SharedSQL_ParseID& PID, const SharedSQL_ModuleName& ModuleName, const SAPDB_Bool M9, Msg_List& MsgList )
    {
        SAPDBTRACE_METHOD_DEBUG ("SharedSQL_ParseIDCache::PutParseID", SharedSQL_Trace, 5);
        SAPDBERR_ASSERT_ARGUMENT( SH );

        Container_AVLTreeError rc;
        if (const SharedSQL_ParseIDTreeNode* N = mParseIDTree.Insert(PID, rc))
        {
            const SharedSQL_PrepareHandle* Handle = REINTERPRET_CAST(const SharedSQL_PrepareHandle*, SH);
            SharedSQL_ParseIDData* PD = (SharedSQL_ParseIDData*) N->GetContent();
            PD->mAllocPtr   = mParseIDTree.GetAllocator();
            if (ModuleName.mName)
            {
                PD->mModuleName.mName       = (void*) PD->mAllocPtr->Allocate(ModuleName.mSize);
                if (PD->mModuleName.mName)
                {
                    SAPDB_MemCopyNoCheck(PD->mModuleName.mName, ModuleName.mName, ModuleName.mSize);
                    PD->mModuleName.mSize = ModuleName.mSize;
                }
            }
            PD->mHandle = SharedSQL_ParseIDHandle(0, *Handle, M9);
        }
        else
        {
            MsgList.AppendNewMessage( Msg_List( Msg_List::Error, SDBMSG_SHAREDSQL_PUTPARSEID) );
        }
        return (rc == AVLTree_Ok);
    };

    /*!
        @brief  Drop some parseid previously added with PutParseID
        @param  PID [in]
        @return true, if ok, else false
    */
    inline SAPDB_Bool DropParseID( const SharedSQL_ParseID& PID )
    {
        SAPDBTRACE_METHOD_DEBUG ("SharedSQL_ParseIDCache::DropParseID", SharedSQL_Trace, 5);

        Container_AVLTreeError rc = mParseIDTree.Delete(PID);
        return ( rc != AVLTree_Ok );
    };

    /*!
        @brief  Get the module name for some command identified by parseid
        @param  PID [in]
        @return const SharedSQL_ModuleName*
    */
    inline const SharedSQL_ModuleName* GetModuleName( const SharedSQL_ParseID& PID )
    {
        SAPDBTRACE_METHOD_DEBUG ("SharedSQL_ParseIDCache::GetModuleName", SharedSQL_Trace, 5);

        if (const SharedSQL_ParseIDData* D = mParseIDTree.FindContent(PID))
        {
            return &(D->mModuleName);
        }
        return 0;
    };

    /*!
        @brief  Set statistic data from execution for some command identified by parseid
        @param  PID                 [in]
        @param  CommandID           [out]
        @param  ExecuteTimeSec      [in]
        @param  ExecuteTimeMicroSec [in]
        @param  ReadRowCount        [in]
        @param  QualifiedRowCount   [in]
        @param  VirtualReadCount    [in]
        @param  PhysicalReadCount   [in]
        @param  FetchRowCount       [in]
        @param  SuspendCount        [in]
        @param  WaitCount           [in]
        @return true, if ok, else false
    */
    inline SAPDB_Bool SetExecuteStatisticData( const SharedSQL_ParseID& PID,
                                        SharedSQL_CommandID& CommandID,
                                        SAPDB_UInt8 ExecuteTimeSec, SAPDB_UInt8 ExecuteTimeMicroSec,
                                        SAPDB_Int8 ReadRowCount, SAPDB_Int8 QualifiedRowCount,
                                        SAPDB_Int8 VirtualReadCount, SAPDB_Int8 PhysicalReadCount,
                                        SAPDB_Int8 FetchRowCount, 
                                        SAPDB_Int8 SuspendCount, SAPDB_Int8 WaitCount ) const
    {
        SAPDBTRACE_METHOD_DEBUG ("SharedSQL_ParseIDCache::GetExecuteHandle", SharedSQL_Trace, 5);

        if (const SharedSQL_ParseIDData* D = mParseIDTree.FindContent(PID))
        {
            D->mHandle.GetCommandID(CommandID);
            return D->mHandle.SetExecuteStatisticData(ExecuteTimeSec, ExecuteTimeMicroSec,
                                            ReadRowCount, QualifiedRowCount,
                                            VirtualReadCount, PhysicalReadCount,
                                            FetchRowCount, 
                                            SuspendCount, WaitCount);
        }
        return SAPDB_FALSE;
    };

    /// ...
	SAPDB_UInt2& LocksForCommandCacheIterator( void )
	{
		return mLocksForCommandCacheIterator;
	}
    //
private:
    /// AVL Tree to store the ParseIDData in
    SharedSQL_ParseIDTree               mParseIDTree;
    /// Allocator used to create the ParseIDData
    SharedSQL_ParseIDComparator         mParseIDComparator;
	/// ...
	SAPDB_UInt2							mLocksForCommandCacheIterator;
 }; // SharedSQL_ParseIDCache

/* ------------------------------------------------------------------------- */

#endif  /* SHAREDSQL_PARSEIDCACHE_HPP */

