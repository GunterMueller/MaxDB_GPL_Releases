/*!
 * @addtogroup SQLManager
*/
/*!
 * @defgroup SharedSQL SharedSQL
 * @ingroup SQLManager
*/

/*!
    @file     SharedSQL_Types.hpp
    @ingroup  SharedSQL
    @author   DirkT
    @brief    Certain Types used throughout SharedSQL
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


#ifndef SHAREDSQL_TYPES_HPP
#define SHAREDSQL_TYPES_HPP

//#define SHAREDSQL_PROTECT 1

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include <memory.h>

//#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SQLManager/SQLMan_Types.hpp"
#include "SQLManager/Catalog/Catalog_Types.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"

#include "RunTime/Synchronisation/RTESync_NamedSpinlock.hpp"
#include "RunTime/Synchronisation/RTESync_RWRegion.hpp"

#include "Container/Container_AVLTree.hpp"

#include "vak001.h"

/// Topic to Trace SharedSQL
extern SAPDBTrace_Topic SharedSQL_Trace;

/// Macro to write to the trace even within the fast kernel
#define SHARED_SQL_TRACE(X) \
if (SharedSQL_Trace.TracesLevel (3)) {\
    SAPDBTrace_Stream stream (&SharedSQL_Trace.GetTracePrinter ());\
    stream << X << NewLine;\
}

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/// User ID
typedef tgg00_Surrogate             SharedSQL_UserID;
/// SQL Mode
typedef tsp00_SqlMode_Enum          SharedSQL_SQLMode;
/// CodeType
typedef tsp00_CodeType              SharedSQL_CodeType;
/// Isolation Level
typedef tsp00_Uint1                 SharedSQL_IsolationLevel;
/// Date/Time Format
typedef tgg00_DateTimeFormat_Enum   SharedSQL_DateTimeFormat;
/// a_nls_params.date_format   
typedef tsp00_C50                   SharedSQL_NLSParam_DateFormat;
/// length of SharedSQL_NLSParam_DateFormat
typedef tsp00_Int2                  SharedSQL_NLSParam_DFLength;

/// ID to identify the SharedSQL_CachedCommand
typedef SAPDB_UInt8                 SharedSQL_CommandID;
/// Hashvalue 
typedef SAPDB_UInt4                 SharedSQL_HashValue;

/*---------------------------------------------------------------------------*/

/// ...
typedef Catalog_ObjectId SharedSQL_Schema;
/// ...
typedef Catalog_ObjectId SharedSQL_DBProc;

//class SharedSQL_Schema : public tgg00_Surrogate
//{
//public:
//    //----------------------------------------------------------------------------
//    /*!
//       @brief       default constructor
//    */
//    SharedSQL_Schema( void ) { memset(this, 0x00, sizeof(*this)); };
//
//    //----------------------------------------------------------------------------
//    /*!
//       @brief       operator ==
//       @param       S [in]
//       @return      bool
//    */
//    bool operator==( const SharedSQL_Schema& S ) const
//    {
//        return ( 0 == memcmp(this, &S, sizeof(*this)) );
//    }
//    /*!
//       @brief       operator !=
//       @param       S [in]
//       @return      bool
//    */
//    bool operator!=( const SharedSQL_Schema& S ) const
//    {
//        return ( !(*this==S) );
//    }
//};  // SharedSQL_Schema

/*---------------------------------------------------------------------------*/
/*!
    @class          SharedSQL_Table
    @brief          ...
 */
class SharedSQL_Table
{
public:
    /// ...
    SharedSQL_Table( void ) // default constuctor necessary for Container_Vector<>
        : mSchema()
        , mTable(false)
		, mWithSpecifiedSchema(false)
    {
    }

    /// ...
    SharedSQL_Table( SharedSQL_Schema S, SQLMan_Identifier T, bool SpecifiedSchema )
        : mSchema(S)
        , mTable(T)
		, mWithSpecifiedSchema(SpecifiedSchema)
    {
    }

private:
    /// ...
    bool operator==( const SharedSQL_Table& T ) const
    {
        if ( mTable==T.mTable )
        {
//			if ( mWithSpecifiedSchema==T.mWithSpecifiedSchema )
				return mSchema==T.mSchema;
        }
        return false;
    }

public:
    /// ...
    static int Compare (const SharedSQL_Table& Left, const SharedSQL_Table& Right)
    {
        if (Left.mSchema < Right.mSchema) return -1; 
        if (Left.mSchema > Right.mSchema) return 1;
        if (Left.mTable < Right.mTable) return -1;
        if (Left.mTable > Right.mTable) return 1;
        return 0;
    }

    /// ...
    SharedSQL_Schema        mSchema;    
    /// ...
    SQLMan_Identifier       mTable;    
	/// ...
	bool                    mWithSpecifiedSchema;

};  // SharedSQL_Table

/// ...
typedef Container_AVLNode<SharedSQL_Table,SharedSQL_Table> SharedSQL_TableNode;

/// ...
typedef Container_AVLTree<SharedSQL_TableNode,SharedSQL_Table,SharedSQL_Table> SharedSQL_TableTree;

/*---------------------------------------------------------------------------*/

/*! Status of a SharedSQL_CachedCommand within the SharedSQL_CachedCommand. 
The status may be "New", "Invalid", "Dropped", "Preparing" or "Prepared".
*/
enum SharedSQL_CommandStatus    { New = 0, Invalid = 1, Dropped = 2, Preparing = 3, Prepared = 4 };

/*---------------------------------------------------------------------------*/
/*!
    @class          SharedSQL_LockedScope
    @brief          creates a locked scope either with a Spinlock or with a RWRegion
 */
class SharedSQL_LockedScope
{
public:
    /// Mode of locking (Exclusive or NonExclusive)
    enum SharedSQL_LockMode         { NonExclusive = false, Exclusive = true };
    /// Type of lockobject (RWRegion or Spinlock)
    enum SharedSQL_LockType         { RWRegion = 1, Spinlock = 2 };
    //
    //----------------------------------------------------------------------------
    /*!
       @brief       constructor
       @param       SpinlockPtr [in]
       @param       SpinLoops   [in]
    */
    SharedSQL_LockedScope(RTESync_Spinlock* SpinlockPtr, int SpinLoops=0) : 
        mMode(true)
        ,mLockType(Spinlock)
        ,mSpinlockPtr(SpinlockPtr)
        ,mRWRegionPtr(0)
    {
        mSpinlockPtr->Lock(SpinLoops);
    }

    //----------------------------------------------------------------------------
    /*!
       @brief       constructor
       @param       RWRegionPtr [in]
       @param       Mode        [in]
    */
    SharedSQL_LockedScope(RTESync_IRWRegion* RWRegionPtr, SharedSQL_LockMode Mode) : 
        mMode(Mode==Exclusive) 
        ,mLockType(RWRegion)
        ,mSpinlockPtr(0)
        ,mRWRegionPtr(RWRegionPtr)
    {
        mRWRegionPtr->enter(mMode);
    }

    //----------------------------------------------------------------------------
    /*!
       @brief       destructor
    */
    ~SharedSQL_LockedScope()
    {
        switch (mLockType)
        {
        case Spinlock:
            mSpinlockPtr->Unlock();
            break;
        case RWRegion:
            mRWRegionPtr->leave(mMode);
            break;
        }
    }
    //
private:
    /// Mode
    bool               mMode;
    /// Type of the lock (Spinlock or RWRegion)
    SharedSQL_LockType mLockType;
    //
    /// Pointer to the spinlock
    RTESync_Spinlock* mSpinlockPtr;
    /// Pointer to the RWRegion
    RTESync_IRWRegion* mRWRegionPtr;
    //
};  // SharedSQL_LockedScope

/*---------------------------------------------------------------------------*/
/*!
    @class          SharedSQL_ParseID
    @brief          handles ParseIDs in SharedSQL
 */
class SharedSQL_ParseID : public tak_parsid
{
public:
    //----------------------------------------------------------------------------
    /*!
       @brief       default constructor
    */
    SharedSQL_ParseID( void ) { memset(this, 0x00, sizeof(*this)); };

    ////----------------------------------------------------------------------------
    ///*!
    //   @brief       copy constructor
    //   @param       P [in]
    //*/
    //SharedSQL_ParseID( const SharedSQL_ParseID& P ) 
    //{
    //    SAPDB_MemCopyNoCheck(this, &P, sizeof(*this));
    //}

    ////----------------------------------------------------------------------------
    ///*!
    //   @brief       operator =
    //   @param       P [in]
    //   @return      SharedSQL_ParseID&
    //*/
    //SharedSQL_ParseID& operator=( const SharedSQL_ParseID& P ) 
    //{
    //    SAPDB_MemCopyNoCheck(this, &P, sizeof(*this));
    //    return *this;
    //}

    //----------------------------------------------------------------------------
    /*!
       @brief       operator ==
       @param       P [in]
       @return      bool
    */
    bool operator==( const SharedSQL_ParseID& P ) const
    {
        return ( 0 == memcmp(this, &P, sizeof(*this)-2) );
    }

    //----------------------------------------------------------------------------
    /*!
       @brief       operator !=
       @param       P [in]
       @return      bool
    */
    bool operator!=( const SharedSQL_ParseID& P ) const
    {
        return (! (*this == P) ); 
    }


    //----------------------------------------------------------------------------
    /*!
       @brief       operator >
       @param       P [in]
       @return      bool
    */
    bool operator>( const SharedSQL_ParseID& P ) const
    {
        return ( 0 < memcmp(this, &P, sizeof(*this)-2) );
    }

    //----------------------------------------------------------------------------
    /*!
       @brief       operator <
       @param       P [in]
       @return      bool
    */
    bool operator<( const SharedSQL_ParseID& P ) const
    {
        return ( 0 > memcmp(this, &P, sizeof(*this)-2) );
    }
};  // SharedSQL_ParseID

/*---------------------------------------------------------------------------*/
/*!
    @class          SharedSQL_SQLContext
    @brief          represents the context of a sqlstatement
 */
class SharedSQL_SQLContext
{
public:
    //----------------------------------------------------------------------------
    /*!
       @brief       default constructor
    */
    SharedSQL_SQLContext( void )  
		:SharedRepository(0)
		,UserID() 
        ,DBProc()
        ,CurrSchema()
        ,SQLMode(sqlm_oracle) 
        ,IsolationLevel(0)
        ,DateTimeFormat(dtf_none)
        ,NLSParam_DFLength(0)
        ,CodeType(0)
        ,MassCmd(true)
        ,VariableInput(false)
		,WithInfo(false)
    {
        memset(&NLSParam_DateFormat, 0, sizeof(SharedSQL_NLSParam_DateFormat));
    }

    //----------------------------------------------------------------------------
    /*!
       @brief       constructor
	   @param       SharedRepos[in]
       @param       UID        [in]
       @param       SCH        [in]
       @param       DBP        [in]
       @param       Mode       [in]
       @param       IsoLevel   [in]
       @param       DTFormat   [in]
       @param       DateFormat [in]
       @param       DFLength   [in]
       @param       CodeT      [in]
       @param       MCmd       [in]
       @param       VarIn      [in]
    */
    SharedSQL_SQLContext( SAPDB_Int1 SharedRepos, SharedSQL_UserID UID, SharedSQL_Schema SCH, SharedSQL_DBProc DBP, SharedSQL_SQLMode Mode, SharedSQL_IsolationLevel IsoLevel, 
        SharedSQL_DateTimeFormat DTFormat, SharedSQL_NLSParam_DateFormat DateFormat, SharedSQL_NLSParam_DFLength DFLength, 
        SharedSQL_CodeType CodeT, bool MCmd, bool VarIn, bool WInfo) 
		:UserID(UID)
        ,DBProc(DBP)
        ,CurrSchema(SCH)
        ,SQLMode(Mode)
        ,IsolationLevel(IsoLevel)
        ,DateTimeFormat(DTFormat)
        ,NLSParam_DateFormat(DateFormat)
        ,NLSParam_DFLength(DFLength)
        ,CodeType(CodeT)
		,SharedRepository(SharedRepos)
        ,MassCmd(MCmd) 
        ,VariableInput(VarIn)
		,WithInfo(WInfo)
    {
        memset(((char*)&NLSParam_DateFormat)+DFLength, 0, sizeof(SharedSQL_NLSParam_DateFormat)-DFLength);
    }

    //----------------------------------------------------------------------------
    /*!
       @brief       operator ==
       @param       C [in]
       @return      bool
    */
    bool operator==( const SharedSQL_SQLContext& C ) const
    {
//        return ( 0 == memcmp(this, &C, sizeof(*this)) );  // allignment problem on some plattforms...

        return  (   SharedRepository== C.SharedRepository &&
			        UserID          == C.UserID         &&
                    CurrSchema      == C.CurrSchema     &&
                    DBProc          == C.DBProc         &&
                    CodeType        == C.CodeType       &&
                    SQLMode         == C.SQLMode        &&
                    IsolationLevel  == C.IsolationLevel &&
                    MassCmd         == C.MassCmd        &&
                    VariableInput   == C.VariableInput  &&
					WithInfo        == C.WithInfo       &&
                    DateTimeFormat  == C.DateTimeFormat &&
                    NLSParam_DFLength == C.NLSParam_DFLength &&
                    !memcmp(&NLSParam_DateFormat, &C.NLSParam_DateFormat, NLSParam_DFLength)
                );
    }

    //----------------------------------------------------------------------------
    /*!
       @brief       operator !=
       @param       C [in]
       @return      bool
    */
    bool operator!=( const SharedSQL_SQLContext& C ) const
    {
        return (! (*this == C) ); 
    }

    /// UserID
    SharedSQL_UserID            UserID;
    /// Schema the DBProcedure belongs to
    SharedSQL_Schema            CurrSchema;
    /// DBProcedure
    SharedSQL_DBProc            DBProc;       
    
    /// NLS DateFormat
    SharedSQL_NLSParam_DateFormat NLSParam_DateFormat;
    /// NLS DateFormat length
    SharedSQL_NLSParam_DFLength   NLSParam_DFLength;

    /// SQLMode
    SharedSQL_SQLMode           SQLMode;
    /// Isolation level
    SharedSQL_IsolationLevel    IsolationLevel;
    /// Date/Time format
    SharedSQL_DateTimeFormat    DateTimeFormat;
    /// Unicode 
    SharedSQL_CodeType          CodeType;
	/// SharedRepository
	SAPDB_Int1                  SharedRepository;
    /// MassCommand
    bool                        MassCmd;
    /// Variable Input Length
    bool                        VariableInput;
	/// WithInfo
	bool						WithInfo;
    // + Privileges !?
    //
};  // SharedSQL_SQLContext

/*---------------------------------------------------------------------------*/
/*!
    @class          SharedSQL_SQLStmt
    @brief          represents a sqlstatement
 */
class SharedSQL_SQLStmt
{
public:
    //----------------------------------------------------------------------------
    /*!
       @brief       default constructor
    */
    SharedSQL_SQLStmt( void  ) : mStmt(0), mStmtSize(0), mDescription(0), mDescriptionSize(0)
    {
    }

    //----------------------------------------------------------------------------
    /*!
       @brief       copy constructor. attention: the statement S.mStmt points to will not be duplicated!
    */
    SharedSQL_SQLStmt( const SharedSQL_SQLStmt& S ) : mStmt(S.mStmt), mStmtSize(S.mStmtSize), mDescription(S.mDescription), mDescriptionSize(S.mDescriptionSize)
    {
        SAPDBERR_ASSERT_STATE( (mStmt && mStmtSize>0) || (!mStmt && !mStmtSize) );
        SAPDBERR_ASSERT_STATE( (mDescription && mDescriptionSize>0) || (!mDescription && !mDescriptionSize) );

        if ( !mStmt )  
            mStmtSize = 0;
        if ( !mDescription )
            mDescriptionSize = 0;
    }

    //----------------------------------------------------------------------------
    /*!
        @brief      constructor. attention: the statement Stmt points to will not be duplicated!
        @param      Stmt            [in]
        @param      StmtSize        [in]
        @param      Description     [in]
        @param      DescriptionSize [in]
    */
    SharedSQL_SQLStmt( void* Stmt, SAPDB_Int4 StmtSize, void* Description, SAPDB_Int4 DescriptionSize ) : mStmt(Stmt), mStmtSize(StmtSize), mDescription(Description), mDescriptionSize(DescriptionSize)
    {
        SAPDBERR_ASSERT_STATE( (mStmt && mStmtSize>0) || (!mStmt && !mStmtSize) );
        SAPDBERR_ASSERT_STATE( (mDescription && mDescriptionSize>0) || (!mDescription && !mDescriptionSize) );

        if ( !mStmt )  
            mStmtSize = 0;
        if ( !mDescription )
            mDescriptionSize = 0;
    }

    //----------------------------------------------------------------------------
    /*!
        @brief      destructor. attention: the memory mStmt points to will not be deallocated!
    */
    ~SharedSQL_SQLStmt( void )
    {
        // Attention: The destruktor does not deallocate the memory Stmt points to !!!
    }

    //----------------------------------------------------------------------------
    /*!
        @brief      operator ==
        @param      C [in]
        @return     bool
    */
    bool operator==( SharedSQL_SQLStmt& C )
    {
        if (mStmtSize == C.mStmtSize && mStmt && C.mStmt)
        {
            if (mDescriptionSize == C.mDescriptionSize && mDescription && C.mDescription)
                return ((!memcmp(mStmt,        C.mStmt,        mStmtSize)) && 
                        (!memcmp(mDescription, C.mDescription, mDescriptionSize))); 

            if (!mDescription && !C.mDescription) 
                return (!memcmp(mStmt, C.mStmt, mStmtSize)); 

        }
        return 0;
    }
    //
    /// Pointer to the sqlstatement. attention: the memory mStmt points to will not be allocated/deallocated by SharedSQL_SQLStmt!
	void* mStmt;
    /// Statement size [Byte] 
    SAPDB_Int4 mStmtSize;
    //
    /// Pointer to the despription. attention: the memory mDescription points to will not be allocated/deallocated by SharedSQL_SQLStmt!
    void* mDescription;
    /// Description size [Byte] 
    SAPDB_Int4 mDescriptionSize;
    //
};  // SharedSQL_SQLStmt

/*---------------------------------------------------------------------------*/
/*!
    @class          SharedSQL_ModuleName
    @brief          represents a module name
 */
class SharedSQL_ModuleName
{
public:
    //----------------------------------------------------------------------------
    /*!
       @brief       default constructor
    */
    SharedSQL_ModuleName( void  ) : mName(0), mSize(0)
    {
    }

    //----------------------------------------------------------------------------
    /*!
       @brief       copy constructor. attention: the statement M.mModuleName points to will not be duplicated!
    */
    SharedSQL_ModuleName( const SharedSQL_ModuleName& M ) : mName(M.mName), mSize(M.mSize)
    {
    }

    //----------------------------------------------------------------------------
    /*!
        @brief      constructor. attention: the ModuleName mName points to will not be duplicated!
        @param      Name [in]
        @param      Size [in]
    */
    SharedSQL_ModuleName( void* Name, SAPDB_Int2 Size ) : mName(Name), mSize(Size)
    {
        if ( !mName )  
            mSize = 0;
    }

    //----------------------------------------------------------------------------
    /*!
        @brief      destructor. attention: the memory mName points to will not be deallocated!
    */
    ~SharedSQL_ModuleName( void )
    {
        // Attention: The destruktor does not deallocate the memory Stmt points to !!!
    }

    /// Pointer to the ModuleName. attention: the memory mName points to will not be allocated/deallocated by SharedSQL_SQLStmt!
	void* mName;
    /// Statement size [Byte] 
    SAPDB_Int2 mSize;
    //
};  // SharedSQL_ModuleName

/*---------------------------------------------------------------------------*/

#endif  /* SHAREDSQL_TYPES_HPP */