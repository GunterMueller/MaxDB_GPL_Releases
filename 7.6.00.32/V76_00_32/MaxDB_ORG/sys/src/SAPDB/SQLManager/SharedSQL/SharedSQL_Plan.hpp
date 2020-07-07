/*!
    @file     SharedSQL_Plan.hpp
    @ingroup  SharedSQL
    @author   DirkT
    @brief    AVLTree to store the execution plan for SQLCommands
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

#ifndef SHAREDSQL_PLAN_HPP
#define SHAREDSQL_PLAN_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SQLManager/SharedSQL/SharedSQL_Types.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/SQLMan_ChangedUserScope.hpp"
#include "SQLManager/Catalog/Catalog_TableDescriptor.hpp"
#include "SQLManager/Catalog/Catalog_Instance.hpp"
#include "SQLManager/Catalog/Catalog_ISharedSQLInterface.hpp"
#include "SAPDBCommon/Messages/SDBMsg_SharedSQL.h"

#include "SAPDB/Container/Container_AVLTree.hpp"
#include "SAPDB/Container/Container_Vector.hpp"

#include "SAPDB/SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"

#include "SAPDB/RunTime/Synchronisation/RTESync_RWRegion.hpp"

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/// PlanComparator
typedef Catalog_ISharedSQLInterface SharedSQL_PlanComparator;

/*---------------------------------------------------------------------------*/
/*!
    @class          SharedSQL_PlanNode
    @brief          Node within the SharedSQL_PlanTree
 */
class SharedSQL_PlanNode : public Container_AVLNode<void*, SharedSQL_PlanComparator>
{
public:
    /*!
        @brief  Constructor
        @param  k [in]
    */
    SharedSQL_PlanNode( void *const& k  ) : 
        Container_AVLNode<void*, SharedSQL_PlanComparator>(k) {};

    /*! @copydoc Container_AVLNode::CleanUp() */
    virtual void CleanUp(SAPDBMem_IRawAllocator& alloc, SharedSQL_PlanComparator& cmp)
    {
        cmp.DestroyPlanObject(alloc, m_Key);
    }
};  // SharedSQL_PlanNode

/*---------------------------------------------------------------------------*/
/*!
    @brief          Container_AVLTree to store SharedSQ_PlanNode 
 */
typedef Container_AVLTree<SharedSQL_PlanNode, void*, SharedSQL_PlanComparator>
        SharedSQL_PlanTree;

/*---------------------------------------------------------------------------*/
/*!
    @class  SharedSQL_Plan
    @brief  Class to handle execution plans
 */
class SharedSQL_Plan
{
public: 

    /*!
        @brief  Destructor
    */
    ~SharedSQL_Plan()
    {
    }

    /*!
        @class  PlanPartIterator
        @brief  Class to iterate over the parts of some plan
    */
	class PlanPartIterator
	{
		friend class SharedSQL_Plan;
		friend class SharedSQL_PrepareHandle;

	protected:

        /// ...
		PlanPartIterator( SharedSQL_PlanTree& PlanTree )
			: Iter(PlanTree.First())
		{
		}

        /// ...
		PlanPartIterator( void )
			: Iter()
		{
		}

	public :
        /*!
           @brief steps to the next plan part
         */
        void operator++()
		{
			Iter++;
		}
        /*!
          @brief true, if the iterator points to a column; otherwise false
         */
        operator bool() const
		{
			return Iter.IsValid();
		}
        /*!
           @brief returns true, if 2 iterators are equal
         */
        bool operator== (const PlanPartIterator& Compare) const
		{
			return ( Iter() == Compare.Iter() );
		}
        /*!
           @brief returns true, if 2 iterators are not equal
         */
        bool operator!= (const PlanPartIterator& Compare) const
		{
			return ( Iter() != Compare.Iter() );
		}
        /*!
          @brief yields a column descriptor for the current column
         */
        const void* operator*() const
		{
			return *Iter()->GetKey();
		}

	private:

		Container_AVLTree<SharedSQL_PlanNode, void*, SharedSQL_PlanComparator>::Iterator Iter;  /// ...
	};

protected:
	friend class SharedSQL_CachedCommand;	// no one but SharedSQL_CachedCommand must access SharedSQL_Plan !!!
	friend class SharedSQL_PrepareHandle;
	friend class SharedSQL_ExecuteHandle;

	/*!
        @brief  Constructor
        @param  Allocator    [in]
    */
    SharedSQL_Plan( SAPDBMem_IRawAllocator& Allocator)
		: mNextPlan(0)
		, mPrevPlan(0)
        , mPlanComparator()
	    , mPlanTree(&mPlanComparator, &Allocator)
        , mPlanSize(0)
        , mMasterParseID()
        , mTables(Allocator)
		, mStatus(New)
		, mParseIDCount(0)
		, mExecuteCount(0)
    {
    }

    /*!
        @brief  Returns the allocator used to store the execution plan
        @return SAPDBMem_IRawAllocator*
    */
    inline SAPDBMem_IRawAllocator* GetAllocator( void )
    {
        return mPlanTree.GetAllocator();
    }

    /*!
        @brief  Returns wheather or not there is a plan
        @return SAPDB_Bool
    */
    inline SAPDB_Bool IsEmpty( void ) const
    {
        return ( mPlanTree.GetSize() == 0);
    }

    /*!
        @brief  Returns the size of the execution plan
        @return SAPDB_Int4
    */
    inline SAPDB_Int4 GetSize( void ) const
    {
        return mPlanSize;
    }

    /// ...
	inline PlanPartIterator GetPlanPartIterator( void )
	{
		return PlanPartIterator(mPlanTree);
	}

    /*!
        @brief  Returns some plan element identified by the key
        @param  Key [in]
        @return const void*
    */
    inline const void* Find( void* Key ) const
    {
        if (SharedSQL_PlanNode* N = mPlanTree.Find(Key))
                return (*(N->GetKey()));
        return 0;
    }

    /*!
        @brief  Insert the execution plan
        @param  MPID [in] - ParseID used during preparation
        @param  P    [in] - Pointer to the execution plan
        @param  Size [in] - Size of the execution plan
        @param  MsgList [out] - Message list
        @return ...
    */
    inline SAPDB_Bool Insert( SharedSQL_ParseID MPID, void *const P, SAPDB_ULong Size, Msg_List& MsgList )
    {
        if ( mMasterParseID == SharedSQL_ParseID() )
            mMasterParseID = MPID;

        if ( MPID == mMasterParseID )
        {
            Container_AVLTreeError rc;
            if (mPlanTree.Insert(P, rc) )
            {
                mPlanSize += Size;
                return SAPDB_TRUE;
            }
            else
            {
                switch (rc)
                {
                case AVLTree_DuplicateKey: 
                    MsgList.AppendNewMessage( Msg_List(Msg_List::Error,
                                              SDBMSG_SHAREDSQL_PUTPLAN_DUPKEY) );
                    break;
                case AVLTree_OutOfMemory:
                    MsgList.AppendNewMessage( Msg_List(Msg_List::Error,
                                              SDBMSG_SHAREDSQL_PUTPLAN_NOMEM) );
                    break;
                }
            }
        }
        else
        {
            MsgList.AppendNewMessage( Msg_List(Msg_List::Error,
                                      SDBMSG_SHAREDSQL_PUTPLAN_INVALIDID) );
			SAPDBERR_ASSERT_STATE(0); // DDT crash always !
        }
        return SAPDB_FALSE;
    }


    /// ...
    inline SAPDB_Bool InsertTableInfo(  const SharedSQL_Table& Table, Msg_List& MsgList )
    {
		SAPDBERR_ASSERT_STATE( !Table.mSchema.IsNull() );
        if ( !mTables.InsertEnd(Table) )
        {
            MsgList.AppendNewMessage( Msg_List(Msg_List::Error,
                                        SDBMSG_SHAREDSQL_PUTTABLES) );
            return false;
        }
        return true;
    }

    /// ...
	inline void ClearTableInfo( void )
	{
		mTables.Clear();
	}


    /*!
        @brief  AffectedByTables
		@param  Context    [in] the (current) sqlman context
		@param  StmtContext[in] the statement context
        @param  Tables     [in] pointer to vector of SharedSQL_Table
        @return true if the plan is affected by one of the given tables 
    */
    inline SAPDB_Bool AffectedByTables( SQLMan_Context& Context, const SharedSQL_SQLContext& StmtContext, SharedSQL_TableTree* Tables ) const
    {
        SAPDBERR_ASSERT_ARGUMENT( Tables );

        SAPDB_Int2 m = mTables.GetSize();
        if ( m>0 && Tables )
        {
            for ( SAPDB_Int2 i=0; i<m; i++ )
            {
                SharedSQL_TableTree::Iterator iter = Tables->First();
                while (iter)
                {
					SAPDBERR_ASSERT_STATE( (*iter()->GetKey()).mWithSpecifiedSchema );

					if (   mTables[i].mSchema == (*iter()->GetKey()).mSchema 
						&& mTables[i].mTable  == (*iter()->GetKey()).mTable )		// it does contain
					{
						return true;
					}

					if (  !mTables[i].mWithSpecifiedSchema 
					    && mTables[i].mSchema != StmtContext.CurrSchema 
        			    && mTables[i].mTable  == (*iter()->GetKey()).mTable )		// it may contain
					{
						if ( WouldFindOtherTableNow(Context, StmtContext, mTables[i]) )
							return true;
					}

					++iter;
                }
            }
            return false;
        }
        return true;
    }

    /*!
        @brief  AffectedBySchema
		@param  Context    [in] the (current) sqlman context
		@param  StmtContext[in] the statement context
        @param  Schema     [in] the schema to search for
        @return true if the plan is affected by the given schema 
    */
    inline SAPDB_Bool AffectedBySchema( SQLMan_Context& Context, const SharedSQL_SQLContext& StmtContext, SharedSQL_Schema Schema ) const
    {
		if (   StmtContext.CurrSchema.IsNull()
			|| Schema.IsNull()  )
			return true;

        SAPDB_Int2 m = mTables.GetSize();
        if ( m > 0 )
        {
            for ( SAPDB_Int2 i=0; i<m; i++ )
            {
	            if ( mTables[i].mSchema == Schema )			// it does contain
	                return true;

				if (  !mTables[i].mWithSpecifiedSchema 
					&& mTables[i].mSchema != StmtContext.CurrSchema )	// it may contain
				{
					if ( WouldFindOtherTableNow(Context, StmtContext, mTables[i]) )
		                return true;
				}
			}
            return false;
        }
        return true;
    }

    /*!
        @brief  WouldFindOtherTableNow
		@param  Context    [in] the (current) sqlman context
		@param  StmtContext[in] the statement context
        @param  Table      [in] the table to check
        @return true if due to ddl operations now another table would be found 
    */
	inline SAPDB_Bool WouldFindOtherTableNow( SQLMan_Context& Context, const SharedSQL_SQLContext& StmtContext, const SharedSQL_Table& Table ) const
	{
		SAPDBERR_ASSERT_STATE( Context.a_returncode == 0 );
		return true;

		// check if we still find the same table
	    SQLMan_ChangedUserScope(Context, StmtContext.UserID, StmtContext.CurrSchema);
		if ( Context.IsOk() )
		{
			Catalog_TableDescriptor TableDescriptor(Context, SQLMan_Identifier(), Table.mTable, StmtContext.UserID, 
													/*ConsiderResultTables=*/ true, /* ConsiderSYSDD */ true);
			if ( !Context.IsOk() || !TableDescriptor )
			{
				Context.a_returncode = 0;
				return true;
			}
			return ( TableDescriptor.GetSchemaId() != Table.mSchema );
		}
		Context.a_returncode = 0;
		return true;
	}

    /*!
        @brief  Returns the master parseid. e.g. the parse id used to prepare the command.
        @return SharedSQL_ParseID&
    */
    inline SharedSQL_ParseID& GetMasterParseID( void )
    {
        return mMasterParseID;
    }

    /// ...
	inline SharedSQL_Plan*& NextPlan( void )
	{
		return mNextPlan;
	}

    /// ...
	inline SharedSQL_Plan*& PrevPlan( void )
	{
		return mPrevPlan;
	}

    /// ...
	inline bool DeletePlanElements( void )
	{
		mPlanTree.DeleteAll();
		mPlanSize = 0;		
		SetStatus(Dropped);
		return true;
	}

    /// ...
	inline void IncExecuteCount( void )
	{
		SAPDBERR_ASSERT_STATE( mParseIDCount >= mExecuteCount );
		++mExecuteCount;
	}

    /// ...
	inline SAPDB_Int4 DecExecuteCount( void )
	{
		--mExecuteCount;
		SAPDBERR_ASSERT_STATE( mParseIDCount >= mExecuteCount );
		return mExecuteCount;
	}

    /// ...
	inline SAPDB_Int4 GetExecuteCount( void )
	{
		SAPDBERR_ASSERT_STATE( mParseIDCount >= mExecuteCount );
		return mExecuteCount;
	}

    /// ...
	inline void IncParseIDCount( void )
	{
		SAPDBERR_ASSERT_STATE( mParseIDCount >= mExecuteCount );
		++mParseIDCount;
	}

    /// ...
	inline SAPDB_Int4 DecParseIDCount( void )
	{
		--mParseIDCount;
		SAPDBERR_ASSERT_STATE( mParseIDCount >= mExecuteCount );
		return mParseIDCount;
	}

    /// ...
	inline SAPDB_Int4 GetParseIDCount( void )
	{
		return mParseIDCount;
	}

    /// ...
	SharedSQL_CommandStatus GetStatus( void )
	{
		return mStatus;
	}

    /// ...
	void SetStatus( SharedSQL_CommandStatus Status )
	{
	    if ( !((mStatus == Invalid) && (Status==Dropped)) )
		    mStatus = Status; 
	}

private:
	/// ...
	SharedSQL_Plan*						mNextPlan;
	/// ...
	SharedSQL_Plan*						mPrevPlan;
    /// ParseID used to create the execution plan
    SharedSQL_ParseID                   mMasterParseID;
	/// ...
	SAPDB_Int4                          mExecuteCount;
	/// ...
	SAPDB_Int4                          mParseIDCount;
    /// AVL Tree that stores the execution plan
    SharedSQL_PlanTree	   			    mPlanTree;
    /// The tables belonging to this command
    Container_Vector<SharedSQL_Table>   mTables;
    /// Comparator to compare and find plan elements
    SharedSQL_PlanComparator            mPlanComparator;
	/// Size of the plan elements stored in the plan tree
    SAPDB_Int4                          mPlanSize;
	/// ...
	SharedSQL_CommandStatus				mStatus;

};

/* ------------------------------------------------------------------------- */

#endif  /* SHAREDSQL_PLAN_HPP */
