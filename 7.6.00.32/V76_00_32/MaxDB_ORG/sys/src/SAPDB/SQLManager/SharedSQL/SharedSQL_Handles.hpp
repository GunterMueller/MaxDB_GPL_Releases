/*!
    @file     SharedSQL_Handles.hpp
    @ingroup  SharedSQL
    @author   DirkT
    @brief    Handles for SharedSQL_Commands
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

#ifndef SHAREDSQL_HANDLES_HPP
#define SHAREDSQL_HANDLES_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SQLManager/SharedSQL/SharedSQL_Types.hpp"
#include "SQLManager/SharedSQL/SharedSQL_Plan.hpp"

#include "SAPDBCommon/SAPDB_Types.hpp"

class SharedSQL_CachedCommand;
//class SharedSQL_Plan;

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


/*---------------------------------------------------------------------------*/
/*!
    @class          SharedSQL_PrepareHandle
    @brief          Handle for the SharedSQL_CachedCommand used during parsing
 */
class SharedSQL_PrepareHandle
{
public:
	/*!
        @brief  Constructor
        @param  Alloc [in] - Pointer to the allocator that was used to create the handle
        @param  R     [in] - Pointer to the command the handle refers to
        @param  MustPrepare [in] 
    */
	SharedSQL_PrepareHandle( SAPDBMem_IRawAllocator* Alloc, SharedSQL_CachedCommand* R, bool& MustPrepare );
    /*!
        @brief  Destructor
    */
    ~SharedSQL_PrepareHandle( void );

    /*!
        @brief  Returns wheather or not the command must be prepared
        @param  Status [out] - The (old) status before the handle was created. It may be NEW, INVALID or DROPPED if the command has to be prepared or PREPARED if not.
        @return SAPDB_Bool
    */
    SAPDB_Bool MustPrepare( SharedSQL_CommandStatus& Status ) const;
    /*!
        @brief  Returns the master parse id. e.g. the parse id used to prepare the command.
        @return const SharedSQL_ParseID*
    */
    const SharedSQL_ParseID* GetMasterParseID( void ) const;
    /*!
        @brief  Store the execution plan
        @param  MPID [in] - Parse ID used to prepare the plan. Will be stored as MasterParseID within the command
        @param  P    [in] - Pointer to the execution plan
        @param  Size [in] - Size of the execution plan
        @param  MsgList [out] - MessageList
        @return true/false
    */
	bool PutPlan( SharedSQL_ParseID MPID, void *const P, SAPDB_ULong Size, Msg_List& MsgList );

    /*!
        @brief  Add information about a table that is referd by the command
        @param  Table   [in]
        @param  MsgList [out]
        @return true/false
    */
    bool PutTable( const SharedSQL_Table& Table, Msg_List& MsgList ) const;
	
    /*!
        @brief  Clears the information about tables refered by the command
    */
    void ClearTables( void ) const;
    /*!
        @brief  Returns a pointer to the execution plan
        @param  Key [in]
        @return const void*
    */
	const void*	GetPlan( void* Key ) const;
    /*!
        @brief  Returns a pointer to the SharedSQL_Plan object
        @return const void*
    */
	const void*	GetPlanObjectPtr( void ) const;

	/// ...
	bool HasPlan( void ) const { return mpPlan!=0; }

    /// ...
	const SharedSQL_Plan::PlanPartIterator GetPlanPartIterator( void ) const;

    /*!
        @brief  To be called when an error during preparation occurs. Will set the status to INVALID.
    */
	void ErrorWhilePreparing( void ) const;
    /*!
        @brief  Release the Handle. Will set the status to PREPARED if a plan was stored otherwise to INVALID.
        @param  ParseTimeSec      [in] - Time needed for preparation (sec)
        @param  ParseTimeMicroSec [in] - Time needed for preparation (microsec)
		@param  QRWDone           [in] - QueryRewrite that has been done
    */
    void ReleaseHandle( SAPDB_UInt8 ParseTimeSec, SAPDB_UInt8 ParseTimeMicroSec, SQLMan_QueryRewriteMode QRWDone );


    /*!
        @brief  Operaror=
        @param  H [in]
    */
    SharedSQL_PrepareHandle& operator=( const SharedSQL_PrepareHandle& H );

    /*!
        @brief  Returns the allocator that was used to create the handle
        @return SAPDBMem_IRawAllocator&
    */
    SAPDBMem_IRawAllocator& GetAllocator(void);
	//
	friend class SharedSQL_ParseIDHandle;

private:
	/// Pointer to the allocator that was used to create the handle
    SAPDBMem_IRawAllocator*     mAllocator;
	/// Pointer to the command the PrepareHandle refers to
	SharedSQL_CachedCommand*	mRef;
	/// The plan that corresponds to this handle
	SharedSQL_Plan*				mpPlan;
	/// The commands status before this handle was created by the command cache. The actual status will be PREPARING if mFlgMustPrepare is TRUE or PREPARED if its FALSE. The old status may be NEW, INVALID or DROPPED if FlgPrepare is TRUE or PREPARED if its FALSE.
    SharedSQL_CommandStatus     mOldStatus;
	/// Flag to indicate that the session that holds this handle has to prepare the command
	SAPDB_Bool		            mFlgMustPrepare;

};  // SharedSQL_PrepareHandle

/*---------------------------------------------------------------------------*/
/*!
    @class          SharedSQL_ParseIDHandle
    @brief          Handle for the SharedSQL_CachedCommand used within the SharedSQL_ParseIDCache
 */
class SharedSQL_ParseIDHandle
{
public:
	/*!
        @brief  Constructor
    */
    SharedSQL_ParseIDHandle( void );

    /*!
        @brief  Constructor
        @param  R [in]
    */
    SharedSQL_ParseIDHandle( SharedSQL_CachedCommand* R );

    /*!
        @brief  Constructor
        @param  Alloc [in] - Pointer to the allocator that was used to create the handle
        @param  H     [in] - PrepareHandle to create the ParseIDHandle from
        @param  M9    [in] - Flag to indicate wheather -8 or -9 should be returned to the application
    */
    SharedSQL_ParseIDHandle( SAPDBMem_IRawAllocator* Alloc, const SharedSQL_PrepareHandle& H, const SAPDB_Bool M9 );
    
    /*!
        @brief  Operaror =
        @param  H [in]
    */
    SharedSQL_ParseIDHandle& operator=( const SharedSQL_ParseIDHandle& H ); 

    /*!
        @brief  Destructor
    */
    ~SharedSQL_ParseIDHandle( void );

    /*!
        @brief  Returns the allocator that was used to create the handle
        @return SAPDBMem_IRawAllocator&
    */
    SAPDBMem_IRawAllocator& GetAllocator(void);

    /*!
        @brief  SetExecuteStatisticData
        @param  ExecuteTimeSec      [in]
        @param  ExecuteTimeMicroSec [in]
        @param  ReadRowCount        [in]
        @param  QualifiedRowCount   [in]
        @param  VirtualReadCount    [in]
        @param  PhysicalReadCount   [in]
        @param  FetchRowCount       [in]
        @param  SuspendCount        [in]
        @param  WaitCount           [in]
        @return TRUE if ok, else FALSE
    */
    SAPDB_Bool SetExecuteStatisticData(SAPDB_UInt8 ExecuteTimeSec, SAPDB_UInt8 ExecuteTimeMicroSec,
                                        SAPDB_Int8 ReadRowCount, SAPDB_Int8 QualifiedRowCount,
                                        SAPDB_Int8 VirtualReadCount, SAPDB_Int8 PhysicalReadCount,
                                        SAPDB_Int8 FetchRowCount, 
                                        SAPDB_Int8 SuspendCount, SAPDB_Int8 WaitCount ) const;

    /*!
        @brief  Returns the CommandID
        @param  ID [out]
    */
    void GetCommandID( SharedSQL_CommandID& ID ) const;

	friend class SharedSQL_ExecuteHandle;

private:
	/// Pointer to the allocator that was used to create the handle
    SAPDBMem_IRawAllocator*     mAllocator;
	/// Pointer to the command the ParseIDHandle refers to
	SharedSQL_CachedCommand*	mRef;
	/// The plan that corresponds to this handle
	SharedSQL_Plan*				mpPlan;
	/// Flag to indicate wheather -8 or -9 should be returned to the application
    SAPDB_Bool                  mMinus9;

};  // SharedSQL_ParseIDHandle

/*---------------------------------------------------------------------------*/
/*!
    @class          SharedSQL_ExecuteHandle
    @brief          Handle for the SharedSQL_CachedCommand used during execution
 */
class SharedSQL_ExecuteHandle
{
public:
    /*!
        @brief  Constructor
        @param  Alloc          [in] - Pointer to the allocator that was used to create the handle
        @param  H              [in] - ParseIDHandle to create the ExecuteHandle from
        @param  CountExecuting [in] - Flag to indicate if the Execution counter shall be increased
    */
	SharedSQL_ExecuteHandle( SAPDBMem_IRawAllocator* Alloc, const SharedSQL_ParseIDHandle& H, bool CountExecuting );

    /*!
        @brief  Destructor
    */
	~SharedSQL_ExecuteHandle( void ); 

    /*!
        @brief  Returns the master parse id. e.g. the parse id used to prepare the command.
        @return const SharedSQL_ParseID*
    */
    const SharedSQL_ParseID* GetMasterParseID( void ) const;
    /*!
        @brief  Returns a pointer to the execution plan
        @param  Key [in]
        @return const void*
    */
	const void*	GetPlan( void* Key ) const;
    /*!
        @brief  Returns a pointer to the SharedSQL_Plan object
        @return const void*
    */
	const void*	GetPlanObjectPtr( void ) const;
    /*!
        @brief  Sets the execution plan to INVALID if an error during excution occurs.
    */
	void SetPlanInvalid( void ) const;
    /*!
        @brief  Sets the execution plan to DROPPED if an error during excution occurs.
    */
	void SetSilentReparse( void ) const;
    /*!
        @brief  Returns if the execution plan is valid
        @param  M9 [out] - Flag to indicate wheather -8 or -9 should be returned to the application
        @return SAPDB_Bool
    */
	SAPDB_Bool IsPlanValid( SAPDB_Bool& M9 ) const;
    /*!
        @brief  Returns the sql statement and its context
        @param  Context [out]
        @return const SharedSQL_SQLStmt
    */
	const SharedSQL_SQLStmt GetSQLStmt( const SharedSQL_SQLContext* &Context ) const;
    /*!
        @brief  Release the ExecutionHandle 
    */
    void ReleaseHandle( void );

    /*!
        @brief  Operaror =
        @param  H [in]
    */
    SharedSQL_ExecuteHandle& operator=( const SharedSQL_ExecuteHandle& H );

    /*!
        @brief  Returns the allocator that was used to create the handle
        @return SAPDBMem_IRawAllocator&
    */
    SAPDBMem_IRawAllocator& GetAllocator(void);

    /// ...
	friend class SharedSQL_ParseIDCache;

private:
	/// Pointer to the allocator that was used to create the handle
    SAPDBMem_IRawAllocator*     mAllocator;
	/// Pointer to the command the ExecuteHandle refers to
	SharedSQL_CachedCommand*	mRef;
	/// The plan that corresponds to this handle
	SharedSQL_Plan*				mpPlan;
	/// Flag to indicate wheather -8 or -9 should be returned to the application
    SAPDB_Bool                  mMinus9;
    //
}; // SharedSQL_ExecuteHandle

#endif  /* SHAREDSQL_HANDLES_HPP */
