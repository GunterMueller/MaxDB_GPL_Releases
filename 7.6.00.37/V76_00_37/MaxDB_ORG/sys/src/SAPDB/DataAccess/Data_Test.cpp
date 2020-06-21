/*!***************************************************************************

  module      : Data_Test
  responsible : UweH
  special area: DataAccess
  created     : 2000-11-22
  last changed: 2000-11-22 12:00
  Copyright (c) 2000-2005 SAP AG
  description : entrypoint for component tests



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


*****************************************************************************/

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/SAPDB_RangeCode.hpp"
#include "DataAccess/Data_PageAccessManager.hpp"
#include "DataAccess/Data_Chain.hpp"
#include "DataAccess/Data_PageSplitSpace.hpp"
#include "DataAccess/Data_SplitSpaceWriter.hpp"
#include "DataAccess/Data_SplitSpaceReader.hpp"
#include "DataAccess/Data_ChainSplitSpaceBackwardRead.hpp"
#include "DataAccess/Data_ChainSplitSpaceForwardRead.hpp"
#include "DataAccess/Data_ChainFixSizeSpace.hpp"
#include "DataAccess/Data_PrimaryFile.hpp"
#include "DataAccess/Data_TempFile.hpp"
#include "DataAccess/Data_PageBasic.hpp"
#include "DataAccess/Data_Test.hpp"
#include "KernelCommon/Kernel_IAdminInfo.hpp"
#include "KernelCommon/Kernel_OpMsg.hpp"
#include "Container/Container_Vector.hpp"
#include "RunTime/System/RTESys_MicroTime.h"
#include "Transaction/Trans_Context.hpp"
#include "hbd01.h"   // b01fcreate_file, b01pdestroy_perm_file
#include "hbd01_1.h" // b01niltree_id
#include "hbd02.h"   // b02add_record

/* --------------------------------------------------------------------------- */
typedef Data_ChainSplitSpaceBackwardRead<Data_PageSplitSpaceBackwardRead>
    BackwardChainType;
typedef Data_ChainSplitSpaceForwardRead<Data_PageSplitSpaceForwardRead>
    ForwardChainType;
typedef Data_ChainFixSizeSpace<Data_PageFixSizeSpace>
    FixSizeChain;
    
/* --------------------------------------------------------------------------- */

inline void Check (const char * Message,
                   bool         CheckValue)
{
    if ( ! CheckValue )
        RTE_Crash( Data_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,Message) );
}

/* --------------------------------------------------------------------------- */
/*
 * Create a chain, insert a second page after the root page.
 *
 */
inline bool ChainTest (tgg00_TransContext   &Trans,
                       SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_ROUTINE_DEBUG ("Data_Test::ChainTest", DataChain_Trace, 5);
    
    // An empty Chainhandler is declared.

    Data_PageId InvalidRootId;

    Data_PageAccessManager PAM (Trans, Data_UnknownFile, 
        Data_PageRecoveryMode(Data_Dynamic,Data_Recoverable), Data_PageNo());
    
    Data_Chain<Data_ChainPage> Chain (PAM, InvalidRootId);

    // Everything is declared now to create a new page chain.

    Data_ChainIterator<Data_ChainPage> ChainIter (PAM);

    // The create method returns an iterator pointing to the root page.

    if ( ! Chain.Create (ChainIter, errlist) )
        return false;

    // If the Iterator can be dereferenced - it should, test if the root pno of the new chain can be get.

    Check ("iter points to root", ChainIter.IsValid() );

    // First,  the Iterator is dereferenced and gives the ChainPage it points to.
    // Second, the Method from base class Page is used to get the PageId of the root ChainPage.

    Data_PageNo AuxPageNo;

    AuxPageNo = (*ChainIter).PageNo();

    // The Iterator should now move to the next page in chain.

    if ( ! ChainIter.PlusPlus(errlist) )
        return false;

    Check ("there is no next page", ! ChainIter.IsValid() );

    // Set to root again.

    if ( ! Chain.Begin (ChainIter, Data_ForStructureChange, errlist) )
        return false;

    Check ("the root must be accessable", ChainIter.IsValid() );

    // A new page is inserted into the chain.

    if ( ! Chain.Insert (ChainIter, errlist) )
        return false;

    // The new page just inserted is removed now.

    if ( ! Chain.Begin  (ChainIter, Data_ForStructureChange, errlist) ) // ROOT
        return false;
    if ( ! Chain.Delete (ChainIter, errlist) )
        return false;

    // The Iterator should now move to the next page in chain.

    if ( ! ChainIter.PlusPlus(errlist) )
        return false;

    Check ("There is no next page", ! ChainIter.IsValid() );

    // Insert three pages

    if ( ! Chain.End    (ChainIter, Data_ForStructureChange, errlist) )
        return false;
    if ( ! Chain.Insert (ChainIter, errlist) ) // PAGE1
        return false;
    if ( ! ChainIter.Invalidate(errlist) )
        return false;

    if ( ! Chain.End    (ChainIter, Data_ForStructureChange, errlist) )
        return false;
    if ( ! Chain.Insert (ChainIter, errlist) ) // PAGE2
        return false;
    if ( ! ChainIter.Invalidate(errlist) ) 
        return false;

    if ( ! Chain.End    (ChainIter, Data_ForStructureChange, errlist) )
        return false;
    if ( ! Chain.Insert (ChainIter, errlist) ) // PAGE3
        return false;
    if ( ! ChainIter.Invalidate(errlist) ) 
        return false;

    // Now: ROOT->PAGE1->PAGE2->PAGE3
    //      LAST=PAGE3

    if ( ! Chain.Begin  (ChainIter, Data_ForRead, errlist) )
        return false;
    if ( ! ChainIter.PlusPlus(errlist) )
        return false;
    if ( ! ChainIter.PlusPlus(errlist) )
        return false;
    if ( ! ChainIter.PlusPlus(errlist) )
        return false;

    Check ("The Iterator points to PAGE1", ChainIter.IsValid() );

    if(!Chain.FindLastPageNo(errlist))
        return false;
        
    Check ("iter points to last page", Chain.LastPageNo() == (*ChainIter).PageNo() );

    // release internal members (the page pointing to the root)

    if ( ! ChainIter.Invalidate(errlist) )
        return false;

    // Store the LastPageNo - be sure not to collide with myself

    if ( ! Chain.StoreLastPageNo(errlist) )
        return false;
    
    // Drop Chain

    if ( ! Chain.Drop(errlist) )
        return false;
    
    return true;
}

/* --------------------------------------------------------------------------- */

inline bool TestBackwardChainSplitSpace (BackwardChainType           &Chain,
                                         BackwardChainType::Iterator &Iter,
                                         SAPDB_UInt                   NumberOfWrites,
                                         SAPDBErr_MessageList        &errlist)
{
    SAPDBTRACE_ROUTINE_DEBUG ("Data_Test::TestBackwardChainSplitSpace", DataChain_Trace, 5);

    SAPDB_UInt             i,j,k;
    SAPDB_Byte             Block [256];
    const SAPDB_UInt       BlockCount = 10;
    SAPDB_UInt             AddedPages;
    Data_SplitRecordLength WantedSize = BlockCount * sizeof(Block);

    for (i=0;i<sizeof(Block);++i) Block [i] = i;
    
    Data_SplitSpaceWriter::Result WriteResult;
    
    for (i=0;i<NumberOfWrites;++i)
    {
        if ( ! Chain.ReserveSpace (WantedSize, 10, Iter, AddedPages, errlist) )
        {
            errlist = errlist + Data_Exception(__CONTEXT__,DATA_COMPONENT_TEST_FAILED,"ReserveSpace");
            return false;
        }
        bool result;
        Data_SplitSpaceWriter Writer (Iter.Deref(result, errlist));
        if ( ! result )
            return false;
        
        for (j=0;j<BlockCount;++j)
        {
            WriteResult = Writer.Write (Block, sizeof(Block));
            if ( WriteResult != Data_SplitSpaceWriter::moreSpaceAvailable
                 &&
                 WriteResult != Data_SplitSpaceWriter::ok )
            {
                errlist = Data_Exception(__CONTEXT__,DATA_COMPONENT_TEST_FAILED,"TestReserveWriteRead::Write1");
                return false;
            }
        }
        if ( WriteResult != Data_SplitSpaceWriter::ok )
        {
            errlist = Data_Exception(__CONTEXT__,DATA_COMPONENT_TEST_FAILED,"TestReserveWriteRead::Write2");
            return false;
        }
    }

    if ( ! Chain.End (Iter, Data_ForRead, errlist) )
        return false;

    Data_SplitSpaceReader::Result ReadResult;

    for (i=0;i<NumberOfWrites;++i)
    {
        bool result;
        Data_SplitSpaceReader Reader (Iter.Deref(result,errlist));
        if (!result)
            return false;

        for (j=0;j<BlockCount;++j)
        {
            SAPDB_MemFillNoCheck (Block, 0, sizeof(Block));
            
            ReadResult = Reader.Read (Block, sizeof(Block));
            
            if ( ReadResult != Data_SplitSpaceReader::moreSpaceAvailable
                 &&
                 ReadResult != Data_SplitSpaceReader::ok )
            {
                errlist = Data_Exception(__CONTEXT__,DATA_COMPONENT_TEST_FAILED,"TestReserveWriteRead::Read1");
                return false;
            }
                    
            for (k=0;k<sizeof(Block);++k)
                if ( Block [k] != k )
                {
                    errlist = Data_Exception(__CONTEXT__,DATA_COMPONENT_TEST_FAILED,"TestReserveWriteRead::ReadContents");
                    return false;
                }
        }
        if ( ReadResult != Data_SplitSpaceWriter::ok )
        {
            errlist = Data_Exception(__CONTEXT__,DATA_COMPONENT_TEST_FAILED,"TestReserveWriteRead::Read2");
        }
        if ( ! Iter.MinusMinus(errlist) )
            return false;
    }
    return true;
}


/* --------------------------------------------------------------------------- */

inline bool TestBackwardChainSplitSpaceNegative
                (BackwardChainType           &Chain,
                 BackwardChainType::Iterator &Iter,
                 SAPDB_UInt                   NumberOfWrites,
                 SAPDBErr_MessageList        &errlist)
{
    // PTS 1117126 UH 2002-08-07 new
    SAPDBTRACE_ROUTINE_DEBUG ("Data_Test::TestBackwardChainSplitSpaceNegative", DataChain_Trace, 5);

    SAPDB_Byte                    Block [65536-1];
    Data_SplitSpaceWriter::Result WriteResult;

    Iter.WriteToTrace ("TestBackwardChainSplitSpaceNegative1");

    SAPDB_MemFillNoCheck (Block, 'X', sizeof(Block));

    SAPDB_UInt  AddedPages;

    if (! Chain.FindLastPageNo(errlist) )
        return false;

    Data_PageNo testPageNo = Chain.LastPageNo();

    if ( ! Chain.ReserveSpace (sizeof(Block), 10, Iter, AddedPages, errlist) )
    {
        errlist = errlist + Data_Exception(__CONTEXT__,DATA_COMPONENT_TEST_FAILED,"Allocation failed");
        return false;
    }

    bool                            result;
    BackwardChainType::RecordSpace &space       = Iter.Deref(result,errlist);
    if ( !result)
        return false;
    Data_SplitSpaceWriter           Writer (space);

    WriteResult = Writer.Write (Block, sizeof(Block));
    
    Iter.WriteToTrace ("TestBackwardChainSplitSpaceNegative2");

    if ( WriteResult != Data_SplitSpaceWriter::ok )
    {
        errlist = Data_Exception(__CONTEXT__,DATA_COMPONENT_TEST_FAILED,
                                   "TestReserveWriteReadNegative::Write");
        return false;
    }

    Chain.UndoReserveSpace(space);
	if(!Iter.Invalidate(errlist,false))
        return false;

    Iter.WriteToTrace ("TestBackwardChainSplitSpaceNegative3");

    if ( Chain.LastPageNo() != testPageNo )
    {
        errlist = Data_Exception(__CONTEXT__,DATA_COMPONENT_TEST_FAILED,
                                 "TestReserveWriteReadNegative::Chain.LastPageNo() != testPageNo");
        return false;
    }
    return true;
}


/* --------------------------------------------------------------------------- */

inline bool TestForwardChainSplitSpace (ForwardChainType           &Chain,
                                        ForwardChainType::Iterator &Iter,
                                        SAPDB_UInt                  NumberOfWrites,
                                        SAPDBErr_MessageList       &errlist)
{
    SAPDBTRACE_ROUTINE_DEBUG ("Data_Test::TestForwardChainSplitSpace", DataChain_Trace, 5);

    SAPDB_UInt             i,j,k;
    SAPDB_Byte             Block [256];
    const SAPDB_UInt       BlockCount = 10;
    Data_SplitRecordLength WantedSize = BlockCount * sizeof(Block);
    SAPDB_UInt             AddedPages;
    
    for (i=0;i<sizeof(Block);++i) Block [i] = i;
    
    Data_SplitSpaceWriter::Result WriteResult;
    
    for (i=0;i<NumberOfWrites;++i)
    {
        if ( ! Chain.ReserveSpace (WantedSize, 10, Iter, AddedPages, errlist) )
            return false;

        bool result;
        Data_SplitSpaceWriter Writer (Iter.Deref(result,errlist));
        if ( !result)
            return false;

        for (j=0;j<BlockCount;++j)
        {
            WriteResult = Writer.Write (Block, sizeof(Block));
            Check ( "TestReserveWriteRead::Write1",
                    WriteResult == Data_SplitSpaceWriter::moreSpaceAvailable
                    ||
                    WriteResult == Data_SplitSpaceWriter::ok );
        }
        Check ( "TestReserveWriteRead::Write2",
                WriteResult == Data_SplitSpaceWriter::ok );
    }
    
    if(!Chain.Begin (Iter, Data_ForRead, errlist))
        return false;

    Data_SplitSpaceReader::Result ReadResult;

    for (i=0;i<NumberOfWrites;++i)
    {
        bool result;
        Data_SplitSpaceReader Reader (Iter.Deref(result,errlist));
        if ( !result)
            return false;

        for (j=0;j<BlockCount;++j)
        {
            SAPDB_MemFillNoCheck (Block, 0, sizeof(Block));
            
            ReadResult = Reader.Read (Block, sizeof(Block));
            
            Check ( "TestReserveWriteRead::Read1",
                    ReadResult == Data_SplitSpaceReader::moreSpaceAvailable
                    ||
                    ReadResult == Data_SplitSpaceReader::ok );
                    
            for (k=0;k<sizeof(Block);++k)
                Check ( "TestReserveWriteRead::ReadContents", Block [k] == k );
        }
        Check ( "TestReserveWriteRead::Read2",
                ReadResult == Data_SplitSpaceReader::ok );
        if(!Iter.PlusPlus(errlist))
            return false;
    }
    return true;
}


/* --------------------------------------------------------------------------- */

inline bool TestFixSizeChain (FixSizeChain           &Chain,
                              FixSizeChain::Iterator &Iter,
                              SAPDB_UInt              NumberOfRecords,
                              SAPDBErr_MessageList   &errlist)
{
    SAPDBTRACE_ROUTINE_DEBUG ("Data_Test::TestFixSizeChain", DataChain_Trace, 5);

    for (SAPDB_UInt i = 0; i < NumberOfRecords; ++i)
    {
        if(!Chain.ReserveSpace (Iter,errlist))
            return false;
        Check ("Chain::ReserveSpace", Iter.IsValid());
        Check ("Iter::IsValid", Iter.IsValid() );
        if(!Chain.UnLock(errlist))
            return false;
    }
    return true;
}


/* --------------------------------------------------------------------------- */
inline bool StartTestBackwardChainSplitSpace (tgg00_TransContext     &Trans,
                                              SAPDBMem_IRawAllocator &Allocator,
                                              SAPDBErr_MessageList   &errlist)
{
    SAPDBTRACE_ROUTINE_DEBUG ("Data_Test::StartTestBackwardChainSplitSpace", DataChain_Trace, 5);

    Data_PageId                 InvalidRootId;
    Data_PageAccessManager      PAM   (Trans, Data_UndoFile, 
                                       Data_PageRecoveryMode(Data_Dynamic,Data_Recoverable), Data_PageNo());
    BackwardChainType           Chain (PAM, InvalidRootId);
    BackwardChainType::Iterator Iter  (PAM, Allocator);
    
    Check ("Iter.Initialize()", Iter.Initialize() );
    if ( ! Chain.Create (errlist) )
        return false;

    bool result;
    result = TestBackwardChainSplitSpace (Chain, Iter, 10, errlist);
    result = result && TestBackwardChainSplitSpaceNegative (Chain, Iter, 10, errlist);

    result = result && Iter.Invalidate(errlist);
    result = result && Chain.Drop(errlist);
	result = result && Iter.Delete(errlist);

    return result;
}

/* --------------------------------------------------------------------------- */
inline bool StartTestForwardChainSplitSpace (tgg00_TransContext     &Trans,
                                             SAPDBMem_IRawAllocator &Allocator,
                                             SAPDBErr_MessageList   &errlist)
{
    SAPDBTRACE_ROUTINE_DEBUG ("Data_Test::StartTestForwardChainSplitSpace", DataChain_Trace, 5);

    Data_PageId                InvalidRootId;
    Data_PageAccessManager     PAM   (Trans, Data_RedoFile, 
                                      Data_PageRecoveryMode(Data_Dynamic,Data_Recoverable), Data_PageNo());
    ForwardChainType           Chain (PAM, InvalidRootId);
    ForwardChainType::Iterator Iter  (PAM, Allocator);
    
    if ( ! Iter.Initialize(errlist) )
        return false;
    if ( ! Chain.Create (errlist) )
        return false;

    bool result = TestForwardChainSplitSpace (Chain, Iter, 10, errlist);

    result = result && Iter.Invalidate(errlist);
    result = result && Chain.Drop(errlist);
    result = result && Iter.Delete(errlist);
    return result;
}

/* --------------------------------------------------------------------------- */
inline bool StartTestFixSizeChain (tgg00_TransContext   &Trans,
                                   SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_ROUTINE_DEBUG ("Data_Test::StartTestFixSizeChain", DataChain_Trace, 5);

    Data_PageId                InvalidRootId;
    Data_PageAccessManager     PAM    (Trans, Data_OpenTransFile, 
                                       Data_PageRecoveryMode(Data_Dynamic,Data_Recoverable), Data_PageNo());
    FixSizeChain               Chain  (PAM, InvalidRootId, 2000);
    FixSizeChain::Iterator     Iter   (PAM);
    
    if (! Chain.Create(errlist) )
        return false;

    bool result = TestFixSizeChain(Chain, Iter, 10, errlist);
	result = result && Iter.Invalidate(errlist);
    result = result && Chain.Drop(errlist);
    return result;
}


/* --------------------------------------------------------------------------- */
inline bool TestTempFile (tgg00_TransContext     &trans,
                          SAPDBMem_IRawAllocator &allocator,
                          SAPDBErr_MessageList   &errlist)
{
    SAPDB_UInt4   record       = 4711;
    SAPDB_Byte   *recordSpace  = reinterpret_cast<SAPDB_Byte*>(&record);
    bool          result       = true;
    Data_TempFile                 file1 (trans, sizeof(record) );
    Data_TempFile                 file2 (trans, sizeof(record) );
    Data_TempFile::RecordIterator iter1 (file1.PageAccessManager());
    Data_TempFile::RecordIterator iter2 (file2.PageAccessManager());

    if (!file1.Create(errlist))
        return false;
    if (!file2.Create(errlist))
        return false;

    if (!file1.Append (recordSpace, errlist))
        return false;

    record = 4712;
    
    if (!file2.Append (recordSpace, errlist))
        return false;

    if (!file1.GetFirst (iter1,errlist))
        return false;
    if (!iter1.IsValid())
    {
        errlist = Data_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"iter1.IsValid(");
        return false;
    }
    iter1.WriteToTrace("iter1 is ok");
    if (*(reinterpret_cast<SAPDB_UInt4*>(*iter1)) != 4711)
    {
        errlist = Data_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"*iter1 == 4711");
        return false;
    }

    if (!file2.GetFirst (iter2,errlist))
        return false;
    if (!iter2.IsValid())
    {
        errlist = Data_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"iter2.IsValid(");
        return false;
    }
    iter2.WriteToTrace("iter2 is ok");
    if (*(reinterpret_cast<SAPDB_UInt4*>(*iter2)) != 4712)
    {
        errlist = Data_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"*iter2 == 4712");
        return false;
    }

    if (!iter1.Invalidate(errlist))
        return false;
    if (!iter2.Invalidate(errlist))
        return false;

    if(!file1.Verify(Kernel_IAdminInfo::Instance().KernelStateIsAdmin(),errlist))
        return false;
    if (!file2.Verify(Kernel_IAdminInfo::Instance().KernelStateIsAdmin(),errlist))
        return false;
    
    if (! file1.Drop(errlist) && ! file1.IsCreated())
    {
        errlist = errlist + Data_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"! file1.IsCreated()");
        return false;
    }
    if ( ! file2.Drop(errlist) && ! file2.IsCreated())
    {
        errlist = errlist + Data_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"! file2.IsCreated()");
        return false;
    }

    Data_TempFile copiedFile (file1);

    Container_Vector<Data_TempFile>                 files     (allocator);
    Container_Vector<Data_TempFile::RecordIterator> iterators (allocator);

    files.InsertEnd(file1);
    iterators.InsertEnd(iter1);
    return true;
}

/* --------------------------------------------------------------------------- */
inline bool TestChainSpeed (tgg00_TransContext     &trans,
                            SAPDBMem_IRawAllocator &allocator,
                            SAPDBErr_MessageList   &errlist)
{
    Data_PageAccessManager        pam   (trans, Data_UndoFile, 
                                         Data_PageRecoveryMode(Data_Dynamic,Data_Recoverable), 
                                         Data_PageNo());
    Data_PageId                   rootid;
    BackwardChainType             chain (pam, rootid);
    BackwardChainType::Iterator   iter  (pam, allocator);
    
    const SAPDB_UInt              MAXLOOPS  = 10000;
    SAPDB_UInt                    loop;

    const SAPDB_UInt              BLOCKSIZE = 256;
    SAPDB_Byte                    block [BLOCKSIZE];
    SAPDB_UInt4                  *pKey = reinterpret_cast<SAPDB_UInt4*>(block);
    
    SAPDB_UInt                    addedpages;
    bool                          derefResult;
	Data_SplitSpaceWriter         writer (iter.Deref(derefResult,errlist));

    if (!derefResult)
        return false;
        
    SAPDB_UInt8                   timeBegin;
    SAPDB_UInt8                   timeEnd;
    
    SAPDB_MemFillNoCheck (block, 0xAA, BLOCKSIZE);

    if ( ! iter.Initialize() )
    {
        errlist = Data_Exception(__CONTEXT__,DATA_COMPONENT_TEST_FAILED,"TestChainSpeed/iter.Initialize");
        return false;
    }
    if ( ! chain.Create(errlist) )
    {
        errlist = errlist + Data_Exception(__CONTEXT__,DATA_COMPONENT_TEST_FAILED,"TestChainSpeed/chain.Create");
        return false;
    }
    
    timeBegin = RTESys_MicroSecTimer();
    for (loop=0;loop<MAXLOOPS;++loop)
    {
        *pKey = loop;
        if ( ! chain.ReserveSpace (BLOCKSIZE, BLOCKSIZE, iter, addedpages, errlist) )
        {
            errlist = errlist + Data_Exception(__CONTEXT__,DATA_COMPONENT_TEST_FAILED,"TestChainSpeed/ReserveSpace");
            return false;
        }
        writer.Reset();
        if ( writer.Write(block,BLOCKSIZE) != Data_SplitSpaceWriter::ok )
        {
            errlist = Data_Exception(__CONTEXT__,DATA_COMPONENT_TEST_FAILED,"TestChainSpeed/Write");
            writer.AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
        if (!iter.Invalidate(errlist))
            return false;
    }
    timeEnd   = RTESys_MicroSecTimer();

    if (!chain.Drop(errlist))
        return false;
    
    Kernel_VTrace() << "STATISTICS: Chain Write: " << timeEnd-timeBegin << " [ms]";

    tgg00_FileId fileid;
    tgg00_Rec    record;
    
    pKey = reinterpret_cast<SAPDB_UInt4*>(&(record.recBody_gg00()));
    record.recLen_gg00()    = BLOCKSIZE;
    record.recKeyLen_gg00() = sizeof(SAPDB_UInt4);

    SAPDB_MemFillNoCheck (&(record.recBody_gg00()), 0xAA, BLOCKSIZE);

    fileid = b01niltree_id;
    
    // fileid.fileName_gg00() = Filename;
    fileid.fileType_gg00().clear();
    fileid.fileType_gg00().addElement (ftsConcurrent_egg00);
    fileid.fileType_gg00().addElement (ftsPerm_egg00);
	SAPDB_MemCopyNoCheck( &fileid.fileTabId_gg00(), "0xAA0xAA0xAA0xAA0xAA0xAA0xAA0xAA", 8);
    tgg00_ExtendedFiletypeSet eFiletypeSet;
    eFiletypeSet.clear();

    bd01CreatePermTableFile(trans, fileid, eFiletypeSet, 0);

    if ( trans.trError_gg00 != e_ok )
    {
        errlist = Data_Exception(__CONTEXT__,DATA_COMPONENT_TEST_FAILED,"TestChainSpeed/b01fcreate_file");
        return false;
    }
    
    timeBegin = RTESys_MicroSecTimer();
    for (loop=0;loop<MAXLOOPS;++loop)
    {
        *pKey = loop;
        b02add_record (trans, fileid, record);
        if ( trans.trError_gg00 != e_ok )
        {
            errlist = Data_Exception(__CONTEXT__,DATA_COMPONENT_TEST_FAILED,"TestChainSpeed/b02add_record");
            return false;
        }
    }
    timeEnd   = RTESys_MicroSecTimer();
    
    b01pdestroy_perm_file(trans, fileid);
    
    Kernel_VTrace() << "STATISTICS: b02add_record: " << timeEnd-timeBegin << " [ms]";
    return true;
}
/* --------------------------------------------------------------------------- */
inline bool TestNewPages (tgg00_TransContext     &trans,
                          SAPDBMem_IRawAllocator &allocator,
                          SAPDBErr_MessageList   &errlist)
{
    union FrameType
    {
        SAPDB_UInt4 AlignmentPrefix;
        SAPDB_Byte  RawFrame [8192];
    };
    
    FrameType      frame;
    Data_PageFrame pagefame  (frame.RawFrame,8192);
    Data_PageBasic newPage   (pagefame, Data_PageBasic::Id());
    Data_PageBasic emptyPage (pagefame);

    if ( ! newPage.PrepareForWrite() )
    {
        errlist = Data_Exception(__CONTEXT__,DATA_COMPONENT_TEST_FAILED,"newPage.PrepareForWrite() failed");
        return false;
    }
    if ( ! newPage.CheckAfterRead() )
    {
        errlist = Data_Exception(__CONTEXT__,DATA_COMPONENT_TEST_FAILED,"newPage.CheckAfterRead() failed");
        return false;
    }

    return true;
}
/* --------------------------------------------------------------------------- */
externPascal void bd03TestTreeInit   (tgg00_FileId &InvFileId);
externPascal void bd03TestTreeDrop   (tgg00_TransContext &trans, tgg00_FileId &InvFileId);
externPascal void bd03TestTreeCreate (tgg00_TransContext &trans, tgg00_FileId &InvFileId);
externPascal void bd03TestTreeFill   (tgg00_TransContext &trans, tgg00_FileId &InvFileId);
externPascal void bd03TestTreeCheck  (tgg00_TransContext &trans, tgg00_FileId &InvFileId);
externPascal void bd03TestReadPrev (tgg00_TransContext &trans,tgg00_FileId &InvFileId);
static tgg00_FileId Data_Test_InvFileId;
/* --------------------------------------------------------------------------- */
externCpp bool Data_Test (Trans_Context    &trans,
                          SAPDB_UTF8 const *utf8StringArgument,
                          SAPDB_Int4        utf8StringArgumentByteLength)
{
    SAPDBTRACE_ROUTINE_DEBUG ("Data_Test", DataChain_Trace, 5);

    tgg00_TransContext     &oldtrans  = trans.OldTrans();
    SAPDBErr_MessageList   &errlist   = trans.ErrorList();
    SAPDBMem_IRawAllocator &Allocator = trans.Allocator();
    
    if ( ! Kernel_IAdminInfo::Instance().KernelStateIsOnline() )
    {
        errlist = errlist + Data_Exception(__CONTEXT__,DATA_COMPONENT_TEST_FAILED,"MUST BE IN ONLINE MODE");
        oldtrans.trError_gg00 = e_dbm_command_not_possible;
        return false;
	}

    if ( ! TestNewPages(oldtrans, Allocator, errlist) )
    {
        errlist = errlist + Data_Exception(__CONTEXT__,DATA_COMPONENT_TEST_FAILED,"TestNewPages");
        return false;
    }

    if ( ! memcmp(utf8StringArgument,"init",utf8StringArgumentByteLength) )
    {
        bd03TestTreeInit(Data_Test_InvFileId);
        return true;
    }
    if ( ! memcmp(utf8StringArgument,"create",utf8StringArgumentByteLength) )
    {
        bd03TestTreeCreate(oldtrans,Data_Test_InvFileId);
        if ( oldtrans.trError_gg00 != e_ok )
        {
            errlist = Data_Exception(__CONTEXT__,DATA_COMPONENT_TEST_FAILED,"bd03TestTreeCreate failed");
            return false;
        }
        Kernel_OpInfo (csp3_n_btree) << "Data_Test: InvFile created with root " << Data_Test_InvFileId.fileRoot_gg00();
        return true;
    }
    if ( ! memcmp(utf8StringArgument,"fill",utf8StringArgumentByteLength) )
    {
        bd03TestTreeFill(oldtrans,Data_Test_InvFileId);
        if ( oldtrans.trError_gg00 != e_ok )
        {
            errlist = Data_Exception(__CONTEXT__,DATA_COMPONENT_TEST_FAILED,"bd03TestTreeFill failed");
            return false;
        }
        return true;
    }
    if ( ! memcmp(utf8StringArgument,"check",utf8StringArgumentByteLength) )
    {
        bd03TestTreeCheck(oldtrans,Data_Test_InvFileId);
        if ( oldtrans.trError_gg00 != e_ok )
        {
            errlist = Data_Exception(__CONTEXT__,DATA_COMPONENT_TEST_FAILED,"bd03TestTreeCheck failed");
            return false;
        }
        return true;
    }
    if ( ! memcmp(utf8StringArgument,"readPrev",utf8StringArgumentByteLength) )
    {
        bd03TestReadPrev(oldtrans,Data_Test_InvFileId);
        if ( oldtrans.trError_gg00 != e_ok )
        {
            errlist = Data_Exception(__CONTEXT__,DATA_COMPONENT_TEST_FAILED,"bd03TestReadPrev failed");
            return false;
        }
        return true;
    }
    if ( ! memcmp(utf8StringArgument,"drop",utf8StringArgumentByteLength) )
    {
        bd03TestTreeDrop(oldtrans,Data_Test_InvFileId);
        if ( oldtrans.trError_gg00 != e_ok )
        {
            errlist = Data_Exception(__CONTEXT__,DATA_COMPONENT_TEST_FAILED,"bd03TestTreeDrop failed");
            return false;
        }
        return true;
    }

    if ( ! ChainTest(oldtrans, errlist) )
    {
        errlist = errlist + Data_Exception(__CONTEXT__,DATA_COMPONENT_TEST_FAILED,"ChainTest");
        return false;
    }
    if ( ! StartTestBackwardChainSplitSpace(oldtrans, Allocator, errlist) )
    {
        errlist = errlist + Data_Exception(__CONTEXT__,DATA_COMPONENT_TEST_FAILED,"StartTestBackwardChainSplitSpace" );
        return false;
    }
    if ( ! StartTestForwardChainSplitSpace(oldtrans, Allocator, errlist) )
    {
        errlist = errlist + Data_Exception(__CONTEXT__,DATA_COMPONENT_TEST_FAILED,"StartTestForwardChainSplitSpace" );
        return false;
    }
    if ( ! StartTestFixSizeChain(oldtrans, errlist) )
    {
        errlist = errlist + Data_Exception(__CONTEXT__,DATA_COMPONENT_TEST_FAILED,"StartTestFixSizeChain" );
        return false;
    }
    if ( ! TestTempFile (oldtrans, Allocator, errlist) )
    {
        errlist = errlist + Data_Exception(__CONTEXT__,DATA_COMPONENT_TEST_FAILED,"TestTempFile" );
        return false;
    }
    if ( ! TestChainSpeed(oldtrans, Allocator, errlist) )
    {
        errlist = errlist + Data_Exception(__CONTEXT__,DATA_COMPONENT_TEST_FAILED,"TestChainSpeed" );
        return false;
    }
    if ( oldtrans.trError_gg00 != e_ok )
    {
        errlist = errlist + Data_Exception(__CONTEXT__,DATA_COMPONENT_TEST_FAILED,"BasisError is not OK" );
        return false;
    }
    // fast compiler and linker test of new primary file implementation
    Data_PrimaryFile file (oldtrans);
    return true;
}
