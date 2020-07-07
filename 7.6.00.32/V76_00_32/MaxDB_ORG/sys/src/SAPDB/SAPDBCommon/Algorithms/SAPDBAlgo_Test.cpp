/*!
  @file           SAPDBAlgo_Test.cpp
  @ingroup		  SAPDBAlgo
  @author         DirkT

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

#include "SAPDBCommon/SAPDB_Types.hpp"

#include "SAPDBCommon/Messages/SDBMsg_SAPDBAlgo.h"


#include "SAPDBCommon/Algorithms/SAPDBAlgo_Comparator.hpp"
#include "SAPDBCommon/Algorithms/SAPDBAlgo_FunnelMerge.hpp"
#include "SAPDBCommon/Algorithms/SAPDBAlgo_QuickSort.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"

#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "Transaction/Trans_Context.hpp"
#include "RunTime/RTE_Message.hpp"

#include "ggg00.h"
#include "ggg200.h"
#include "gsp00.h"

#include "SAPDBCommon/Algorithms/SAPDBAlgo_Test.hpp"

/* --------------------------------------------------------------------------- */

class TestData
{
public:
    TestData( void ) : Key(0), Idx(0) {};
    TestData(SAPDB_Int4 K, SAPDB_UInt4 I) : Key(K), Idx(I) {};
    SAPDB_Int4  Key;
    SAPDB_UInt4 Idx;
    SAPDB_Byte  Filler[504];
};

class TestDataContainer : public SAPDBAlgo_IMergeSource<TestData>
                        , public cgg200_MergeSource<TestData>
{
public:
    TestDataContainer(tgg00_TransContext& TransContext, SAPDB_UInt4 SourceCnt, SAPDB_UInt4 EntryCnt)
        : mAllocator (*( REINTERPRET_CAST(SAPDBMem_IRawAllocator*, TransContext.trAllocator_gg00) ))
        , mResult(0)
        , mPos(0)
        , mSourceCnt(SourceCnt)
        , mEntryCnt(EntryCnt)
    {
        mResult = newarray(mResult, mSourceCnt, mAllocator);
        mPos    = newarray(mPos,    mSourceCnt, mAllocator);
        for ( unsigned int i=0; i<SourceCnt; i++)
            mPos[i] = 0;
    }

    ~TestDataContainer( void )
    {
        destroyarray(mResult, mSourceCnt, mAllocator);
        destroyarray(mPos,    mSourceCnt, mAllocator);
    }

    void Reset( void )
    {
        for ( unsigned int i=0; i<mSourceCnt; i++)
            mPos[i] = 0;
    }

    // derived from SAPDBAlgo_IMergeSource<>
    virtual unsigned int GetListCount( void )
    {
        return mSourceCnt;
    }
    virtual TestData& GetListElement( unsigned int SourceIndex )
    {
        SAPDBERR_ASSERT_STATE( mPos[SourceIndex] < mEntryCnt )

        mResult[SourceIndex].Key = SourceIndex + mPos[SourceIndex];
        mResult[SourceIndex].Idx = SourceIndex;
        return mResult[SourceIndex];
    }

    virtual bool NextListElement( unsigned int SourceIndex )
    {
        return (++(mPos[SourceIndex]) < mEntryCnt);
    }
    virtual bool Valid( unsigned int SourceIndex )
    {
        return (mPos[SourceIndex] < mEntryCnt);
    }

    // derived from cgg200_MergeSource<>
    virtual tsp00_Uint4 gg200GetListCount()
    {
        return GetListCount();
    }
    virtual bool gg200GetCurrentListElement (unsigned int numList, TestData& target)
    {
        if ( mPos[numList] < mEntryCnt )
        {
            target = GetListElement(numList);
            return true;
        }
        return false;
    }
    virtual void gg200GotoNextListElement (unsigned int numList)
    {
        ++(mPos[numList]);
    }

private:
    SAPDBMem_IRawAllocator& mAllocator;
    TestData*               mResult;
    unsigned int*           mPos;
    unsigned int            mSourceCnt;
    unsigned int            mEntryCnt;
};


class TestComparisonOperator : public cgg200ComparisonOperator< TestData >
                             , public SAPDBAlgo_IComparator<TestData> 
{
public:
    TestComparisonOperator( void )
    {
    };

    // derived from cgg200ComparisonOperator<>
    virtual tsp00_LcompResult_Enum gg200Compare (const TestData &elem1, const TestData &elem2 )
    {
        if ( elem1.Key < elem2.Key )
            return l_less;
        if ( elem1.Key == elem2.Key )
        {
            if ( elem1.Idx < elem2.Idx )
                return l_less;
            else
                if ( elem1.Idx > elem2.Idx )
                    return l_greater;
            return l_equal;
        }
        return l_greater;
    }
    // derived from SAPDBAlgo_IComparator<>
    virtual bool IsEqual(const TestData& A, const TestData& B)
    {
        return (A.Key == B.Key);
    }
    virtual bool IsPredecessor(const TestData& A, const TestData& B)
    {
        if ( A.Key < B.Key )                
            return true;
        if ( A.Key == B.Key )
            return (A.Idx < B.Idx);  
        return false;
    }
};

/* --------------------------------------------------------------------------- */

bool SAPDBAlgo_QuickSortTest(tgg00_TransContext& TransContext, Msg_List& errList)
{
    return true;
}

/* --------------------------------------------------------------------------- */

bool SAPDBAlgo_FunnelTest(tgg00_TransContext& TransContext, Msg_List& errList)
{
    SAPDBMem_IRawAllocator& Allocator = *( REINTERPRET_CAST(SAPDBMem_IRawAllocator*, TransContext.trAllocator_gg00) );
    
    SAPDB_UInt4 SourceCount = 17;
    SAPDB_UInt4 EntryCount  = 100000;
    
    TestDataContainer   NDataContainer(TransContext, SourceCount, EntryCount);
    TestDataContainer   NDataContainerIdx(TransContext, SourceCount, EntryCount);
    TestDataContainer   NDataContainerDef(TransContext, SourceCount, EntryCount);
    TestDataContainer   NDataContainerIdxDef(TransContext, SourceCount, EntryCount);
    TestDataContainer   NDataContainerNoTemplate(TransContext, SourceCount, EntryCount);
    TestDataContainer   ODataContainer(TransContext, SourceCount, EntryCount);

    TestComparisonOperator ComparisonOperator;

    TestData Data, PreviousData;
    SAPDB_Bool WrongOrder;
    SAPDB_Bool ok=true;

    // -- 1. test the new algorithm 
    //       version store only index within the queue, all template arguments specified

    NDataContainerIdx.Reset();
    
    SAPDBAlgo_FunnelMergeIteratorIdx<TestData,TestDataContainer,TestComparisonOperator> 
        NewFunnelMergeIteratorIdx(Allocator, NDataContainerIdx, ComparisonOperator, errList);

    Data = PreviousData = TestData();
    WrongOrder = false;
    while (NewFunnelMergeIteratorIdx.IsValid())
    {
        Data = *NewFunnelMergeIteratorIdx;

        if ( !ComparisonOperator.IsPredecessor(PreviousData, Data) &&
             !ComparisonOperator.IsEqual(PreviousData, Data))
             WrongOrder = true;
        PreviousData = Data;

        ++NewFunnelMergeIteratorIdx;
    }

    if ( WrongOrder )
    {
        errList = Msg_List(Msg_List::Error, SDBMSG_SAPDBALGO_COMPONENT_TEST_FAILED, Msg_Arg("ERRORTEXT", "Wrong order in New Algorithm"));
        ok = false;
    }

    // -- 2. test the new algorithm 
    //       version store full data within the queue, all template arguments specified

    NDataContainer.Reset();
    
    SAPDBAlgo_FunnelMergeIterator<TestData,TestDataContainer,TestComparisonOperator> 
        NewFunnelMergeIterator(Allocator, NDataContainer, ComparisonOperator, errList);

    Data = PreviousData = TestData();
    WrongOrder = false;
    while (NewFunnelMergeIterator.IsValid())
    {
        Data = *NewFunnelMergeIterator;

        if ( !ComparisonOperator.IsPredecessor(PreviousData, Data) &&
             !ComparisonOperator.IsEqual(PreviousData, Data))
             WrongOrder = true;
        PreviousData = Data;

        ++NewFunnelMergeIterator;
    }

    if ( WrongOrder )
    {
        errList = Msg_List(Msg_List::Error, SDBMSG_SAPDBALGO_COMPONENT_TEST_FAILED, Msg_Arg("ERRORTEXT", "Wrong order in New Algorithm"));
        ok = false;
    }

    // -- 3. test the new algorithm 
    //       version store only index within the queue, default template arguments

    NDataContainerIdxDef.Reset();
    
    SAPDBAlgo_FunnelMergeIteratorIdx<TestData> 
        NewFunnelMergeIteratorIdxDef(Allocator, NDataContainerIdxDef, ComparisonOperator, errList);

    Data = PreviousData = TestData();
    WrongOrder = false;
    while (NewFunnelMergeIteratorIdxDef.IsValid())
    {
        Data = *NewFunnelMergeIteratorIdxDef;

        if ( !ComparisonOperator.IsPredecessor(PreviousData, Data) &&
             !ComparisonOperator.IsEqual(PreviousData, Data))
             WrongOrder = true;
        PreviousData = Data;

        ++NewFunnelMergeIteratorIdxDef;
    }

    if ( WrongOrder )
    {
        errList = Msg_List(Msg_List::Error, SDBMSG_SAPDBALGO_COMPONENT_TEST_FAILED, Msg_Arg("ERRORTEXT", "Wrong order in New Algorithm"));
        ok = false;
    }

    // -- 4. test the new algorithm 
    //       version store full data within the queue, default template arguments

    NDataContainerDef.Reset();
    
    SAPDBAlgo_FunnelMergeIterator<TestData> 
        NewFunnelMergeIteratorDef(Allocator, NDataContainerDef, ComparisonOperator, errList);

    Data = PreviousData = TestData();
    WrongOrder = false;
    while (NewFunnelMergeIteratorDef.IsValid())
    {
        Data = *NewFunnelMergeIteratorDef;

        if ( !ComparisonOperator.IsPredecessor(PreviousData, Data) &&
             !ComparisonOperator.IsEqual(PreviousData, Data))
             WrongOrder = true;
        PreviousData = Data;

        ++NewFunnelMergeIteratorDef;
    }

    if ( WrongOrder )
    {
        errList = Msg_List(Msg_List::Error, SDBMSG_SAPDBALGO_COMPONENT_TEST_FAILED, Msg_Arg("ERRORTEXT", "Wrong order in New Algorithm"));
        ok = false;
    }

    // -- 5. test the old algorithm

    ODataContainer.Reset();
    
    SAPDB_UInt4 WorkSize = (1+SourceCount)*(sizeof(TestData)+sizeof(int));
    void* WorkSpace = Allocator.Allocate(WorkSize);

    gg200MergeIterator<TestData, TestComparisonOperator>
        OldFunnelMergeIterator(ComparisonOperator);
    OldFunnelMergeIterator.gg200Assign(ODataContainer, WorkSpace, WorkSize);

    Data = PreviousData = TestData();
    WrongOrder = false;
    while (!OldFunnelMergeIterator.IsReady())
    {
        Data = *OldFunnelMergeIterator;

        if ( !ComparisonOperator.IsPredecessor(PreviousData, Data) &&
             !ComparisonOperator.IsEqual(PreviousData, Data))
             WrongOrder = true;
        PreviousData = Data;

        ++OldFunnelMergeIterator;
    }

    if ( WrongOrder )
    {
        errList = Msg_List(Msg_List::Error, SDBMSG_SAPDBALGO_COMPONENT_TEST_FAILED, Msg_Arg("ERRORTEXT", "Wrong order in Old Algorithm"));
        ok = false;
    }

    if (WorkSpace)
        Allocator.Deallocate(WorkSpace);

    // -----------------------------------------

    return ok;
}

/* --------------------------------------------------------------------------- */
externCpp bool SAPDBAlgo_Test (Trans_Context &trans)
{
    // 2005-01-18 UH use session errlist

    bool result = true;

    trans.OldTrans().trError_gg00 = e_ok;
    
    if ( ! SAPDBAlgo_QuickSortTest(trans.OldTrans(), trans.ErrorList()) )
        result = false;

    trans.OldTrans().trError_gg00 = e_ok;

    if ( ! SAPDBAlgo_FunnelTest(trans.OldTrans(), trans.ErrorList()) )
        result = false;

    return result;
}
