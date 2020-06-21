#ifndef TABLE_TEMPHASHTABLE_HPP
#define TABLE_TEMPHASHTABLE_HPP
/*!
    @file     Table_TempHashTable.hpp
    @ingroup  Table
    @author   DirkT
    @brief    Temporary hash table OR temporary single record for aggregation
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

#include "Container/Container_Vector.hpp"

#include "DataAccess/Data_TempFile.hpp"

#include "KernelCommon/ParameterNames/KernelParam_TempHashTable.hpp"
#include "SAPDBCommon/Messages/SDBMsg_Table.h"

#include "SAPDBCommon/SAPDB_Types.hpp"

#include "SAPDBCommon/Algorithms/SAPDBAlgo_FNVHash.hpp"
#include "SAPDBCommon/Algorithms/SAPDBAlgo_Comparator.hpp"
#include "SAPDBCommon/Algorithms/SAPDBAlgo_QuickSort.hpp"
#include "SAPDBCommon/Algorithms/SAPDBAlgo_FunnelMerge.hpp"

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_RawAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"

#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp"

#include "vak001.h"
#include "hak07.h"

// #############################################################################

extern SAPDBDiag_CheckTopic HashedResultSet_Check;

// #############################################################################

struct tgg00_TransContext;

extern SAPDBTrace_Topic Table_Trace;

/// Hashvalue 
typedef SAPDB_UInt4 TempHashTable_HashValue;

// #############################################################################

class Table_TempHashTableAggregate_IDataMerger
{
public:
    virtual void MergeData( const SAPDB_Byte* Source, SAPDB_Byte* Destination, SAPDB_UInt2 Length, SAPDB_Bool Found, SAPDB_Bool SourceIsPartialAggregate ) = 0;
};

// #############################################################################

struct Table_OffsetLengthTuple
{
    Table_OffsetLengthTuple( void ) : Offset(0), Length(0) {};
    Table_OffsetLengthTuple( SAPDB_Int2 O, SAPDB_UInt2 L ) : Offset(O), Length(L) {};
    SAPDB_Int2  Offset;
    SAPDB_UInt2 Length;
};

struct TempFileContainerEntry
{
    Data_TempFile*                  mFile;
    Data_TempFile::RecordIterator*  mIter;
    SAPDB_Byte*                     mData;
};

typedef Container_Vector<Table_OffsetLengthTuple>   Table_FixSizeEntry_FieldDescription;
typedef Container_Vector<SAPDB_UInt2>               Table_FixSizeEntry_KeyNoList;

// #############################################################################

class Table_FixSizeTempAggregate 
{
public:
	virtual SAPDB_Bool CompletelyConstructed( void ) = 0;
    virtual SAPDB_Bool InsertOrReplace( SAPDB_Byte* Data, Msg_List& ErrorList ) = 0;
    virtual SAPDB_Bool First( Msg_List& ErrorList ) = 0;
    virtual SAPDB_Bool Next( void ) = 0;
    virtual SAPDB_Byte* Get( void ) = 0;
    virtual SAPDB_Bool IsValid( void ) = 0;
    virtual SAPDB_Bool IsEmpty( void ) = 0;
    virtual ~Table_FixSizeTempAggregate() {}
};

// #############################################################################

class Table_FixSizeTempSingleRecordAggregate
	: public Table_FixSizeTempAggregate
	, public SAPDBAlgo_IMergeSource<SAPDB_Byte*>
{

public:
    Table_FixSizeTempSingleRecordAggregate( tgg00_TransContext&             TransContext, 
                                Table_TempHashTableAggregate_IDataMerger&	Merger,
                                SAPDB_UInt4									RecordLength,
                                Msg_List&									ErrorList
                                )
		: mSessionAllocator(*( REINTERPRET_CAST(SAPDBMem_IRawAllocator*, TransContext.trAllocator_gg00) ))
        , mMerger(Merger)
		, mRecord(0)
        , mRecordLength(RecordLength)
		, mFirstInsert(true)
        , mValid(false)
	{
        SAPDBTRACE_METHOD_DEBUG ("Table_FixSizeTempSingleRecordAggregate::CompletelyConstructed", Table_Trace, 2);
		mRecord = (SAPDB_Byte*) mSessionAllocator.Allocate(RecordLength);
	}

	virtual ~Table_FixSizeTempSingleRecordAggregate( void )
	{
        SAPDBTRACE_METHOD_DEBUG ("Table_FixSizeTempSingleRecordAggregate::CompletelyConstructed", Table_Trace, 2);
		if ( mRecord )
			destroy(mRecord, mSessionAllocator);
	}

	virtual SAPDB_Bool CompletelyConstructed( void )
    {
        SAPDBTRACE_METHOD_DEBUG ("Table_FixSizeTempSingleRecordAggregate::CompletelyConstructed", Table_Trace, 5);
        return ( mRecord );
    }

    virtual SAPDB_Bool InsertOrReplace( SAPDB_Byte* Data, Msg_List& ErrorList )
    {
        SAPDBTRACE_METHOD_DEBUG ("Table_FixSizeTempSingleRecordAggregate::InsertOrReplace", Table_Trace, 5);
        SAPDBERR_ASSERT_STATE( Data );

	    mMerger.MergeData(Data, mRecord, mRecordLength, !mFirstInsert, /*SourceIsPartialAggregate*/false);
        mFirstInsert = false;
		mValid = false;	// for Next()
		return true;
    }

    SAPDB_Bool First( Msg_List& ErrorList )
    {
        SAPDBTRACE_METHOD_DEBUG ("Table_FixSizeTempSingleRecordAggregate::First", Table_Trace, 5);
		return (mValid=true);
    }

    SAPDB_Bool Next( void )
    {
        SAPDBTRACE_METHOD_DEBUG ("Table_FixSizeTempSingleRecordAggregate::Next", Table_Trace, 5);
        SAPDBERR_ASSERT_STATE( mValid );
        return (mValid=false);
    }

    virtual SAPDB_Byte* Get( void )
    {
        SAPDBTRACE_METHOD_DEBUG ("Table_FixSizeTempSingleRecordAggregate::Get", Table_Trace, 5);
        SAPDBERR_ASSERT_STATE( mValid );
        return mRecord;
    }

    virtual SAPDB_Bool IsValid( void )
    {
        SAPDBTRACE_METHOD_DEBUG ("Table_FixSizeTempSingleRecordAggregate::IsValid", Table_Trace, 5);
        return mValid;
    }

    virtual SAPDB_Bool IsEmpty( void )
    {
        SAPDBTRACE_METHOD_DEBUG ("Table_FixSizeTempSingleRecordAggregate::IsEmpty", Table_Trace, 5);
        return mFirstInsert;
    }

    virtual unsigned int GetListCount( void )
    {
        // dummy method
        return 1;
    }

    virtual SAPDB_Byte*& GetListElement( unsigned int SourceIndex )
    {
        // dummy method
        static SAPDB_Byte* _do_not_use_this_pointer = 0;
        return _do_not_use_this_pointer;
    }

    virtual bool NextListElement( unsigned int SourceIndex )
    {
        // dummy method
        return false;
    }
    
    virtual bool Valid( unsigned int SourceIndex )
    {
        // dummy method
        return false;
    }
    
private:
    SAPDBMem_IRawAllocator&                         mSessionAllocator;
    Table_TempHashTableAggregate_IDataMerger&       mMerger;

	SAPDB_Byte*                                     mRecord;
    SAPDB_UInt2                                     mRecordLength;
	SAPDB_Bool										mFirstInsert;
    SAPDB_Bool                                      mValid;
};

// #############################################################################

//template <class TMerger=Table_TempHashTableAggregate_IDataMerger>  // would be nice, but LINUX can't handle this: internal compiler error
class Table_FixSizeTempHashTableAggregate
	: public Table_FixSizeTempAggregate
	, public SAPDBAlgo_IMergeSource<SAPDB_Byte*>
    , public SAPDBAlgo_IComparator<SAPDB_Byte*>
{
private:

    class TEntry
    {
    public:
        TEntry( void ) : mNext(0)               {};
        TEntry( TEntry* Next ) : mNext(Next)    {};
        TEntry*   mNext;
    };

    // ----------------------------------------------------------------------------------------------

public:

    Table_FixSizeTempHashTableAggregate( tgg00_TransContext&                TransContext, 
                                Table_TempHashTableAggregate_IDataMerger&	Merger,
                                SAPDB_UInt4									RecordLength,
                                Table_FixSizeEntry_FieldDescription&		FieldDescription, 
                                Table_FixSizeEntry_KeyNoList&				KeyNoList,
                                SAPDB_UInt8									ExpectedEntries,
                                Msg_List&									ErrorList
                                )
        : mTransContext(TransContext)
        , mSessionAllocator(*( REINTERPRET_CAST(SAPDBMem_IRawAllocator*, TransContext.trAllocator_gg00) ))
        , mAllocator(0)
        , mFunnelMergeIterator(0)
        , mMerger(Merger)
        , mTempFileContainer(*( REINTERPRET_CAST(SAPDBMem_IRawAllocator*, TransContext.trAllocator_gg00) ))
        , mRecordLength(RecordLength)
        , mFieldDescription(*( REINTERPRET_CAST(SAPDBMem_IRawAllocator*, TransContext.trAllocator_gg00) ))
        , mKeyNoList(*( REINTERPRET_CAST(SAPDBMem_IRawAllocator*, TransContext.trAllocator_gg00) ))
        , mNumberOfSlots(0)
        , mHashTable(0)
        , mSortTable(0)
        , mSortTableEntryCnt(0)
        , mActSortTableEntry(0)
        , mAllocatedEntryCnt(0)
        , mUsedEntryCnt(0)
        , mFetchedRecord(0)
        , mFirstFree(0)
        , mValid(false)
        , mFirstInsert(true)
        , mFlushed(false)
    {
        SAPDBTRACE_METHOD_DEBUG ("Table_FixSizeTempHashTableAggregate::Table_FixSizeTempHashTableAggregate", Table_Trace, 2);

        SAPDBERR_ASSERT_STATE( mTransContext.trError_gg00 == 0 );

        RTEConf_Parameter::Integer ParamSize=0;
        Msg_List       errLst;

        RTEConf_ParameterAccess::Instance()->GetInteger( UTF8( KERNELPARAM_HASHEDRESULTSET_CACHESIZE ), ParamSize, errLst );
        if ( ParamSize < 32*1024 )  // (Minimum according to cserv.pcf)
        {
            ParamSize = 64*1024;    // set to 64MByte 
        }
        SAPDB_UInt4 kBSizeMax = (SAPDB_UInt4) ParamSize;

        mFieldDescription.Initialize(FieldDescription);
        mKeyNoList.Initialize(KeyNoList);
        SAPDBERR_ASSERT_STATE( KeyNoList.GetSize() > 0 );
        SAPDBERR_ASSERT_STATE( FieldDescription.GetSize() >= KeyNoList.GetSize()  );

        SAPDB_UInt4  SizePerEntry    = 2*sizeof(void*) + sizeof(TEntry)+RecordLength;  // assume optimal hashing ...
        mMaxEntryCnt                 = (1024L*kBSizeMax-RecordLength) / SizePerEntry;
#if defined  (SAPDB_SLOW)
        // for test cases
        mMaxEntryCnt                 = 43;
#endif
        mMaxEntryCnt                 = (SAPDBAlgo_MAX_QUICKSORT_ELEMENTS < mMaxEntryCnt ? 
                                        SAPDBAlgo_MAX_QUICKSORT_ELEMENTS : mMaxEntryCnt );

		SAPDB_UInt4 RecordPerPageCnt = (SAPDB_Int4)(Data_TempFile::MaxFreeSpacePerPage()/RecordLength);

		if ( (mMaxEntryCnt > ExpectedEntries) && (mMaxEntryCnt > RecordPerPageCnt) )
			mMaxEntryCnt = (SAPDB_Int4)(ExpectedEntries > RecordPerPageCnt ? ExpectedEntries : RecordPerPageCnt);

        mNumberOfSlots               = mMaxEntryCnt;

        mKeyCount = mKeyNoList.GetSize();

        mAllocator =   new(mSessionAllocator) SAPDBMem_RawAllocator(   (const SAPDB_UTF8 *) "TempHashTableAllocator",
                                                mSessionAllocator,
                                                kBSizeMax*512,             // FirstBlockSize      = kBSize/2
                                                kBSizeMax*256,             // SupplementBlockSize = kBSize/4
                                                SAPDBMem_RawAllocator::FREE_RAW_EXTENDS,
                                                kBSizeMax*1024             // MaxCacheSize
                                            );



        if ( mAllocator)
        {
            mAllocator->DisableOpMessages();
    
            if ( !(mFetchedRecord = (SAPDB_Byte*)mAllocator->Allocate(RecordLength)) )
                ErrorList = ErrorList + Msg_List(Msg_List::Error, SDBMSG_TABLE_TEMPHASH_NOHASHTABLE);

            if ( mHashTable = newarray(mHashTable, mNumberOfSlots, *mAllocator) )
            {
                for ( TempHashTable_HashValue i=0; i<mNumberOfSlots; i++ ) 
                    mHashTable[i] = 0;
            }
            else
            {
                ErrorList = ErrorList + Msg_List(Msg_List::Error, SDBMSG_TABLE_TEMPHASH_NOHASHTABLE);
            }

            mFieldDescription.Initialize(FieldDescription);
            mKeyNoList.Initialize(KeyNoList);
        }
        else
        {
            ErrorList = ErrorList + Msg_List(Msg_List::Error, SDBMSG_TABLE_TEMPHASH_NOALLOCATOR);
        }
    }

    virtual ~Table_FixSizeTempHashTableAggregate( void )
    {
        SAPDBTRACE_METHOD_DEBUG ("Table_FixSizeTempHashTableAggregate::~Table_FixSizeTempHashTableAggregate", Table_Trace, 2);

        SAPDBERR_ASSERT_STATE( mTransContext.trError_gg00 == 0 );

        DestroyHashTableAndAllocator();

        for ( SAPDB_UInt4 i=0; i<mTempFileContainer.GetSize(); i++ )
        {
            if ( mTempFileContainer[i].mIter != 0 )
            {
                mTransContext.trError_gg00 = 0;
                destroy(mTempFileContainer[i].mIter, mSessionAllocator);
            }
            if ( mTempFileContainer[i].mFile != 0 )
            {
                mTransContext.trError_gg00 = 0;
                destroy(mTempFileContainer[i].mFile, mSessionAllocator);
            }
        }
    }

    // ----------------------------------------------------------------------------------------------

	virtual SAPDB_Bool CompletelyConstructed( void )
    {
        SAPDBTRACE_METHOD_DEBUG ("Table_FixSizeTempHashTableAggregate::CompletelyConstructed", Table_Trace, 5);
        return ( mAllocator && mHashTable );
    }

    virtual SAPDB_Bool InsertOrReplace( SAPDB_Byte* Data, Msg_List& ErrorList )
    {
        SAPDBTRACE_METHOD_DEBUG ("Table_FixSizeTempHashTableAggregate::InsertOrReplace", Table_Trace, 5);
        SAPDBERR_ASSERT_ARGUMENT( Data );
        SAPDBERR_ASSERT_STATE( mTransContext.trError_gg00 == 0 );

        mFirstInsert = false;
        TempHashTable_HashValue Hash = CalcHashValueForKeys(Data);
        TempHashTable_HashValue Slot = CalcSlot(Hash);

        SAPDBTRACE_WRITELN( Table_Trace, 5, "hash: " << Hash << " slot: " << Slot );

        if ( TEntry* Act = mHashTable[Slot] )
        {
            do
            {
                SAPDB_Byte* A = (SAPDB_Byte*)&(Act[1]);
                if ( IsEqual(Data, A) )
                {
                    mMerger.MergeData(Data, A, mRecordLength, true, /*SourceIsPartialAggregate*/false);
                    mValid = false;
                    return true;
                }
            }while ( Act = Act->mNext );
        }

        if ( TEntry* NewEntry = GetMemForEntry(ErrorList) )
        {
            NewEntry->mNext = mHashTable[Slot];
            mHashTable[Slot]= NewEntry;

            SAPDB_Byte* N = (SAPDB_Byte*)&(NewEntry[1]);
            mMerger.MergeData(Data, N, mRecordLength, false, /*SourceIsPartialAggregate*/false);
            mValid = false;
            return true;
        }
        else
        {
            return false;
        }
    }

    virtual SAPDB_Bool First( Msg_List& ErrorList )
    {
        SAPDBTRACE_METHOD_DEBUG ("Table_FixSizeTempHashTableAggregate::First", Table_Trace, 2);
        SAPDBERR_ASSERT_STATE( mTransContext.trError_gg00 == 0 );

        if ( !SortEntries(false, ErrorList) )
        {
            ErrorList = ErrorList + Msg_List(Msg_List::Error, SDBMSG_TABLE_TEMPHASH_FIRSTFAILED);
            return false;
        }
        mActSortTableEntry = 0;

        SAPDB_UInt4 FileCnt = mTempFileContainer.GetSize();

        SAPDBTRACE_WRITELN( Table_Trace, 2, "FileCnt: " << FileCnt );

        for ( SAPDB_UInt4 i=0; i<FileCnt; i++ )
        {
            Data_TempFile::RecordIterator* Iter = mTempFileContainer[i].mIter;
            if ( !Iter )
            {
                if ( !(Iter = new(mSessionAllocator) Data_TempFile::RecordIterator(mTempFileContainer[i].mFile->PageAccessManager())) )
                {
                    mTransContext.trError_gg00 = 0;
                    ErrorList = ErrorList + Msg_List(Msg_List::Error, SDBMSG_TABLE_TEMPHASH_NOITERATOR);
                    return 0;
                }
                mTempFileContainer[i].mIter = Iter;
            }
            if(!mTempFileContainer[i].mFile->GetFirst(*Iter,ErrorList))
            {
                mTransContext.trError_gg00 = 0;
                ErrorList = ErrorList + Msg_List(Msg_List::Error, SDBMSG_TABLE_TEMPHASH_NOITERATOR);
                return 0;
            }
            if ( Iter->IsValid() )
            {
                mTempFileContainer[i].mData = *(*(Iter));
            }
            else
            {
                mTransContext.trError_gg00 = 0;
                ErrorList = ErrorList + Msg_List(Msg_List::Error, SDBMSG_TABLE_TEMPHASH_NOITERATOR);
                return 0;
            }
        }

        if ( mFunnelMergeIterator = new(mSessionAllocator) SAPDBAlgo_FunnelMergeIteratorIdx<SAPDB_Byte*>(mSessionAllocator, *this, *this, ErrorList) )
        {
            if ( mFunnelMergeIterator->CompletelyConstructed() )
            {
                mValid = true;
                Collect();
                return mValid;
            }
            else
            {
                ErrorList = ErrorList + Msg_List(Msg_List::Error, SDBMSG_TABLE_TEMPHASH_FIRSTFAILED);
                return false;
            }
        }
        else
        {
            ErrorList = ErrorList + Msg_List(Msg_List::Error, SDBMSG_TABLE_TEMPHASH_NOITERATOR);
            ErrorList = ErrorList + Msg_List(Msg_List::Error, SDBMSG_TABLE_TEMPHASH_FIRSTFAILED);
            return false;
        }
    }

    virtual SAPDB_Bool Next( void )
    {
        SAPDBTRACE_METHOD_DEBUG ("Table_FixSizeTempHashTableAggregate::Next", Table_Trace, 5);
        SAPDBERR_ASSERT_STATE( mValid );
        SAPDBERR_ASSERT_STATE( mTransContext.trError_gg00 == 0 );

        Collect();
        return mValid;
    }

    virtual SAPDB_Byte* Get( void )
    {
        SAPDBTRACE_METHOD_DEBUG ("Table_FixSizeTempHashTableAggregate::Get", Table_Trace, 5);
        SAPDBERR_ASSERT_STATE( mValid );

        if ( mValid )
            return mFetchedRecord;
        else
            return 0;
    }

    virtual SAPDB_Bool IsValid( void )
    {
        SAPDBTRACE_METHOD_DEBUG ("Table_FixSizeTempHashTableAggregate::IsValid", Table_Trace, 5);

        return mValid;
    }

    virtual SAPDB_Bool IsEmpty( void )
    {
        SAPDBTRACE_METHOD_DEBUG ("Table_FixSizeTempHashTableAggregate::IsEmpty", Table_Trace, 5);
        return mFirstInsert;
    }

    //SAPDB_Bool ForceFlushAndFreeMem( Msg_List& ErrorList )
    //{
    //    SAPDBTRACE_METHOD_DEBUG ("Table_FixSizeTempHashTableAggregate::ForceFlushAndFreeMem", Table_Trace, 5);

    //    if ( FlushToPage(ErrorList) )
    //    {
    //        DestroyHashTableAndAllocator();
    //        return true;
    //    }
    //    return false;
    //}

    // ----------------------------------------------------------------------------------------------

public:
    // Methods derived from SAPDBAlgo_IMergeSource<SAPDB_Byte*>
    virtual unsigned int GetListCount( void )
    {
        return mTempFileContainer.GetSize() + 1;
    }

    virtual SAPDB_Byte*& GetListElement( unsigned int SourceIndex )
    {
        SAPDBERR_ASSERT_STATE( SourceIndex < GetListCount() );

        if ( SourceIndex==0 )
        {
            SAPDBERR_ASSERT_STATE( mActSortTableEntry < mSortTableEntryCnt && mSortTableEntryCnt > 0);
            return mSortTable[mActSortTableEntry];
        }
        else
        {
            SAPDBERR_ASSERT_STATE( mTempFileContainer[SourceIndex-1].mIter->IsValid() );
            return mTempFileContainer[SourceIndex-1].mData;  
        }
    }
    virtual bool Valid( unsigned int SourceIndex )
    {
        SAPDBERR_ASSERT_STATE( mAllocator && mHashTable );
        SAPDBERR_ASSERT_STATE( SourceIndex < GetListCount() );

        if ( SourceIndex==0 )
            return ( mActSortTableEntry < mSortTableEntryCnt && mSortTableEntryCnt > 0);
        else
            return mTempFileContainer[SourceIndex-1].mIter->IsValid();
    }

    virtual bool NextListElement( unsigned int SourceIndex )
    {
        SAPDBERR_ASSERT_STATE( SourceIndex < GetListCount() );

        if ( SourceIndex==0 )
        {
            ++mActSortTableEntry;
            return ( mActSortTableEntry < mSortTableEntryCnt && mSortTableEntryCnt > 0);
        }
        else
        {
            bool val;
            SAPDBERR_ASSERT_STATE( mTempFileContainer[SourceIndex-1].mIter->IsValid() )
            TempFileContainerEntry& CE = mTempFileContainer[SourceIndex-1];
            Msg_List errlist;
            if(!(*CE.mIter).PlusPlus(errlist))
            {
                RTE_Message(errlist);
                CE.mData = 0;
            }
            else
            {
                if ( val = CE.mIter->IsValid() )
                    CE.mData = *(*(CE.mIter));
                else
                    CE.mData = 0;
            }
            return val;
        }
    }

    // Methods derived from SAPDBAlgo_IMergeComparator<SAPDB_Byte*>
    virtual bool IsEqual(SAPDB_Byte* const& A, SAPDB_Byte* const& B)
    {
        SAPDBERR_ASSERT_STATE( A && B);

        for ( unsigned int i=0; i<mKeyCount; i++ )
        {
            if ( memcmp( A + mFieldDescription[mKeyNoList[i]].Offset, 
                         B + mFieldDescription[mKeyNoList[i]].Offset,
                        mFieldDescription[mKeyNoList[i]].Length) != 0)
                return false;
        }
        return true;
    }
    virtual bool IsPredecessor(SAPDB_Byte* const& A, SAPDB_Byte* const& B)
    {
        SAPDBERR_ASSERT_STATE( A && B);

        int v;

        for ( unsigned int i=0; i<mKeyCount; i++ )
        {
            v = memcmp( A + mFieldDescription[mKeyNoList[i]].Offset, 
                        B + mFieldDescription[mKeyNoList[i]].Offset,
                        mFieldDescription[mKeyNoList[i]].Length);

            if ( v>0 )
                return false;
            if ( v<0 )
                return true;
        }
        return false;  // all keys are equal -> return false
    }

    // ----------------------------------------------------------------------------------------------

private:

    inline void DestroyHashTableAndAllocator( void )
    {
        SAPDBTRACE_METHOD_DEBUG ("Table_FixSizeTempHashTableAggregate::DestroyHashTableAndAllocator", Table_Trace, 8);

        if ( mSortTable )
        {
            destroyarray(mSortTable, mMaxEntryCnt, mSessionAllocator);
        }
		if ( mFunnelMergeIterator )
		{
			destroy(mFunnelMergeIterator, mSessionAllocator);
		}
        if ( mAllocator )
        {
            // we don't need to deallocate all that stuff as mAllocator itself will be deallocated !
            destroy(mAllocator, mSessionAllocator);
        }
    }

    inline TEntry* TryAllocateNEntries( SAPDB_Int4 n )
    {
        if ( n > (SAPDB_Int4)(mMaxEntryCnt-mAllocatedEntryCnt) )
            n = mMaxEntryCnt-mAllocatedEntryCnt;

        if ( n>0 )
        {
            SAPDB_Int4 Size = sizeof(TEntry)+mRecordLength;
			if ( Size % 8 != 0 )
			{
				Size  = (SAPDB_Int4)(Size/8);
				Size += 1;
				Size *= 8;
			}
            char* p = (char*)mAllocator->Allocate(n*Size);
            if ( p )
            {
                mAllocatedEntryCnt += n;
                for ( SAPDB_Int8 i=0; i<n; i++ )
                {
                    InsertToFreeList((TEntry*)p);
                    p += Size;
                }
                return GetMemOfFirstFree();
            }
        }
        return 0;
    }

    TEntry* GetMemForEntry( Msg_List& ErrorList )
    {
        SAPDBTRACE_METHOD_DEBUG ("Table_FixSizeTempHashTableAggregate::GetMemForEntry", Table_Trace, 8);

        TEntry* NewEntry = 0;

        if ( mFirstFree )
        {
            NewEntry = GetMemOfFirstFree();
        }
        else
        {
            if ( !mFlushed )
            {
                if ( !(NewEntry=TryAllocateNEntries(1000)) )
                    if ( !(NewEntry=TryAllocateNEntries(100)) )
                        if ( !(NewEntry=TryAllocateNEntries(10)) )
                            NewEntry=TryAllocateNEntries(1);
            }
            if ( !NewEntry )
            {
                FlushToPage(ErrorList);
                mFlushed = true;
                mUsedEntryCnt = 0;
                if ( mFirstFree )
                    NewEntry = GetMemOfFirstFree();
                else
                    ErrorList  = ErrorList + Msg_List(Msg_List::Error, SDBMSG_TABLE_TEMPHASH_NOFREEENTRY);
            }
        }
        if ( NewEntry )
            ++mUsedEntryCnt;
        return NewEntry;
    }

    inline TEntry* GetMemOfFirstFree( void )
    {
        SAPDBTRACE_METHOD_DEBUG ("Table_FixSizeTempHashTableAggregate::GetMemOfFirstFree", Table_Trace, 8);
        SAPDBERR_ASSERT_STATE( mFirstFree );

        TEntry* NewEntry = 0;
        NewEntry        = mFirstFree;
        mFirstFree      = mFirstFree->mNext;
        NewEntry->mNext = 0;
        return NewEntry;
    }

    inline void InsertToFreeList( TEntry* Entry )
    {
        SAPDBTRACE_METHOD_DEBUG ("Table_FixSizeTempHashTableAggregate::FreeMemForEntry", Table_Trace, 8);
        SAPDBERR_ASSERT_ARGUMENT( Entry );

        Entry->mNext = mFirstFree;
        mFirstFree = Entry;
    }

    inline TempHashTable_HashValue CalcHashValueForKeys( SAPDB_Byte* Data )
    {
        SAPDBTRACE_METHOD_DEBUG ("Table_FixSizeTempHashTableAggregate::CalcHashValueForKeys", Table_Trace, 8);
        SAPDBERR_ASSERT_ARGUMENT( Data );

        TempHashTable_HashValue Hash = FNV_32_INIT;
        for ( SAPDB_UInt4 i=0; i<mKeyCount; i++ )
        {
            int FieldIndex = mKeyNoList[i];
            FNV32Hash(&((char*)Data)[mFieldDescription[FieldIndex].Offset], 
                                     mFieldDescription[FieldIndex].Length, Hash);
        }
        return Hash;
    }

    inline TempHashTable_HashValue CalcSlot( const TempHashTable_HashValue Hash )
    {
        SAPDBTRACE_METHOD_DEBUG ("Table_FixSizeTempHashTableAggregate::CalcSlot", Table_Trace, 8);
        return Hash%mNumberOfSlots;
    }

    inline SAPDB_Bool SortEntries( bool DetachFromHash, Msg_List& ErrorList )
    {
        SAPDBTRACE_METHOD_DEBUG ("Table_FixSizeTempHashTableAggregate::Sortentries", Table_Trace, 8);
        SAPDBERR_ASSERT_STATE( mAllocator && mHashTable );

        // Create sort table
        if ( !mSortTable )
            mSortTable = newarray(mSortTable, mMaxEntryCnt, mSessionAllocator);

        if ( mSortTable )
        {
            // Add to sort table
            int k = 0;
	        for ( TempHashTable_HashValue i=0; i<mNumberOfSlots; i++ )
            {
		        if ( TEntry* Act = mHashTable[i] )
		        {
			        do 
			        {
    			        TEntry* Next = Act->mNext;
                        mSortTable[k++] = (SAPDB_Byte*)(Act+1);
                        if ( DetachFromHash )
                            InsertToFreeList(Act); // detaches from hash structure only
				        Act = Next;
			        }while( Act );
                    if ( DetachFromHash )
                        mHashTable[i] = 0;
		        }
            }
            mSortTableEntryCnt = k;

            // Sort the sort table

            if ( !SAPDBAlgo_QuickSort<SAPDB_Byte*>(mSortTable, mSortTableEntryCnt, *this) )
            {
                ErrorList = ErrorList + Msg_List(Msg_List::Error, SDBMSG_TABLE_TEMPHASH_CANNOTSORT);
                return false;
            }

            if ( HashedResultSet_Check.ChecksLevel(5) )
            {
                for ( SAPDB_UInt4 k=1; k<mSortTableEntryCnt; k++ )
                {
                    SAPDBERR_ASSERT_STATE( IsPredecessor(mSortTable[k-1],  mSortTable[k]) );
#if !defined (SAPDB_SLOW)
                    if ( !IsPredecessor(mSortTable[k-1],  mSortTable[k]) )
                    {
                        a07_b_put_error( *(tak_all_command_glob*)(mTransContext.trAcvPtr_gg00), e_system_error, 1 );
                        ErrorList = ErrorList + Msg_List(Msg_List::Error, SDBMSG_TABLE_TEMPHASH_WRONGORDER);
                        RTE_Message(ErrorList);
                        return false;
                    }
#endif
                }
            }

            return true;
        }
        else
        {
            ErrorList = ErrorList + Msg_List(Msg_List::Error, SDBMSG_TABLE_TEMPHASH_NOSORTARRAY);
            ErrorList = ErrorList + Msg_List(Msg_List::Info, SDBMSG_TABLE_TEMPHASH_CANNOTSORT);
            return false;
        }
    }

    inline SAPDB_Bool FlushToPage( Msg_List& ErrorList )
    {
        SAPDBTRACE_METHOD_DEBUG ("Table_FixSizeTempHashTableAggregate::FlushToPage", Table_Trace, 2);
        SAPDBERR_ASSERT_STATE( mAllocator && mHashTable );
        SAPDBERR_ASSERT_STATE( mTransContext.trError_gg00 == 0 );

        // Create new TempFile
        TempFileContainerEntry CE;
        if ( CE.mFile = new(mSessionAllocator) Data_TempFile(mTransContext, mRecordLength) )
        {
            if(!CE.mFile->Create(ErrorList))
                return false;
            CE.mIter = 0;
            CE.mData = 0;
            if ( mTempFileContainer.InsertEnd(CE) )
            {
                // Sort the data
                if ( SortEntries(true, ErrorList) )
                {
                    // Write data to temp file
                    SAPDBTRACE_WRITELN( Table_Trace, 2, "Writing " << mSortTableEntryCnt << " records to Tempfile #" << mTempFileContainer.GetSize()-1 );
                    for ( SAPDB_UInt4 i=0; i<mSortTableEntryCnt; i++ )
                    {
                        SAPDBERR_ASSERT_STATE( mSortTable[i] );
                        if ( i>0 )
                        {
                            SAPDBERR_ASSERT_STATE( IsPredecessor(mSortTable[i-1],  mSortTable[i]) );
#if !defined (SAPDB_SLOW)
                            if ( HashedResultSet_Check.ChecksLevel(3) )
                            {
                                if ( !IsPredecessor(mSortTable[i-1],  mSortTable[i]) )
                                {
                                    a07_b_put_error( *(tak_all_command_glob*)(mTransContext.trAcvPtr_gg00), e_system_error, 1 );
                                    ErrorList = ErrorList + Msg_List(Msg_List::Error, SDBMSG_TABLE_TEMPHASH_WRONGORDER);
                                    RTE_Message(ErrorList);
                                    return false;
                                }
                            }
#endif
                        }
                        if ( mSortTable[i] )
                        {
                            if ( !CE.mFile->Append((const SAPDB_Byte *) mSortTable[i], ErrorList) )
                            {
                                mTransContext.trError_gg00 = 0;
                                ErrorList = ErrorList + Msg_List(Msg_List::Error, SDBMSG_TABLE_TEMPHASH_NOORDEREDFLUSH);
                                return false;
                            }
                        }
                        else
                        {
                            ErrorList = ErrorList + Msg_List(Msg_List::Error, SDBMSG_TABLE_TEMPHASH_NOORDEREDFLUSH);
                            return false;
                        }
                    }
                    return true;
                }
                else
                {
                    ErrorList = ErrorList + Msg_List(Msg_List::Error, SDBMSG_TABLE_TEMPHASH_NOORDEREDFLUSH);
                    return false;
                }
            }
            else
            {
                destroy(CE.mFile, mSessionAllocator);
                CE.mFile = 0;
                ErrorList = ErrorList + Msg_List(Msg_List::Error, SDBMSG_TABLE_TEMPHASH_CONATAINERINSERT);
                return false;
            }
        }
        else
        {
            ErrorList = ErrorList + Msg_List(Msg_List::Error, SDBMSG_TABLE_TEMPHASH_NOTEMPFILE);
            return false;
        }
    }

    void Collect( void )
    {
        SAPDBTRACE_METHOD_DEBUG ("Table_FixSizeTempHashTableAggregate::Collect", Table_Trace, 8);
        SAPDBERR_ASSERT_STATE( mTransContext.trError_gg00 == 0 );

        // Merge all Data that have the same key as the current entry 
        // and set the iterator to the first entry having a different key
        if ( mFunnelMergeIterator->IsValid() )
        {
            SAPDB_MemCopyNoCheck(mFetchedRecord, *(*mFunnelMergeIterator), mRecordLength);
//            mFetchedRecord = *(*mFunnelMergeIterator);

            bool SameKey = true;
            do
            {
                ++(*mFunnelMergeIterator);
                if ( mFunnelMergeIterator->IsValid() )
                {
                    SAPDB_Byte* TempData = *(*mFunnelMergeIterator);

                    SAPDBERR_ASSERT_STATE( !IsPredecessor(TempData, mFetchedRecord) );
#if !defined (SAPDB_SLOW)
                    if ( HashedResultSet_Check.ChecksLevel(3) )
                    {
                        if ( IsPredecessor(TempData, mFetchedRecord) )
                        {
                            a07_b_put_error( *(tak_all_command_glob*)(mTransContext.trAcvPtr_gg00), e_system_error, 1 );
                            RTE_Message(Msg_List(Msg_List::Error, SDBMSG_TABLE_TEMPHASH_WRONGORDER));
                            mValid = false;
//                            mFetchedRecord = 0;
                            return;
                        }
                    }
#endif
                    if ( IsEqual(TempData, mFetchedRecord) )
                        mMerger.MergeData(TempData, mFetchedRecord, mRecordLength, true, /*SourceIsPartialAggregate*/true);
                    else
                        SameKey = false;
                }
                else
                {
                    SameKey = false;
                }

            }while ( SameKey );
        }
        else
        {
            mValid = false;
 //           mFetchedRecord = 0;
        }
    };

    // ----------------------------------------------------------------------------------------------

    tgg00_TransContext&                             mTransContext;
    SAPDBMem_IRawAllocator&                         mSessionAllocator;
    SAPDBMem_RawAllocator*                          mAllocator;
    
    SAPDBAlgo_FunnelMergeIteratorIdx<SAPDB_Byte*>*  mFunnelMergeIterator;
    Table_TempHashTableAggregate_IDataMerger&       mMerger;

    TEntry**                                        mHashTable;
    TEntry*                                         mFirstFree;
    SAPDB_Byte**                                    mSortTable;

    SAPDB_UInt4                                     mSortTableEntryCnt;
    SAPDB_UInt4                                     mActSortTableEntry;

    SAPDB_UInt4                                     mMaxEntryCnt;
    SAPDB_UInt4                                     mAllocatedEntryCnt;
    SAPDB_UInt4                                     mUsedEntryCnt;
    TempHashTable_HashValue                         mNumberOfSlots;
    SAPDB_UInt2                                     mKeyCount;

    Table_FixSizeEntry_KeyNoList                    mKeyNoList;
    Table_FixSizeEntry_FieldDescription             mFieldDescription;

    Container_Vector<TempFileContainerEntry>        mTempFileContainer;

    SAPDB_Byte*                                     mFetchedRecord;
    SAPDB_UInt2                                     mRecordLength;
    SAPDB_Bool                                      mValid;
    bool                                            mFirstInsert;
    bool                                            mFlushed;
};

// #############################################################################
#endif
