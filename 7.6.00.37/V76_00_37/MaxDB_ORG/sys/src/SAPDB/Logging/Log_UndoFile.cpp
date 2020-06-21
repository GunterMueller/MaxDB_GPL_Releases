/*!**************************************************************************

  module      : Log_UndoFile.cpp
  special area: Logging
  responsible : UweH
  last changed: 2000-09-25  12:00
  copyright:    Copyright (c) 2000-2005 SAP AG
  description : Implementation for class Log_UndoFile.



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


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "KernelCommon/Kernel_TraceTopics.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "DataAccess/Data_Types.hpp"
#include "Logging/Log_Exceptions.hpp"
#include "Logging/Log_BeforeImage.hpp"
#include "Logging/Log_UndoFile.hpp"

/* --------------------------------------------------------------------------- */
Log_UndoFile::Log_UndoFile (tgg00_TransContext &Trans,
                            const Data_PageNo   Root,
                            const Data_PageNo   Last)
: Data_BaseFile ( Data_UndoFile,
                  Data_PageId(Root, Data_PageRecoveryMode(Data_Dynamic,Data_Recoverable)),
                  Data_PageAccessManager(Trans, Data_UndoFile, 
                  Data_PageRecoveryMode(Data_Dynamic,Data_Recoverable), Root) ),
  m_Container (m_PageAccessManager, m_RootId, Last),
  m_Iterator  (m_PageAccessManager,*(REINTERPRET_CAST (SAPDBMem_IRawAllocator*, Trans.trAllocator_gg00))),
  m_TransNo   (Trans.trWriteTransId_gg00),
  m_PageCount (0), // PTS 1124083 UH 2003-09-10 added
  m_GarbageCollectionNeeded(true)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_UndoFile::Log_UndoFile", LogTrans_Trace, 5);
}

/* --------------------------------------------------------------------------- */
Log_UndoFile::Log_UndoFile (tgg00_TransContext  &Trans,
                            const tgg91_TransNo &TransNo,
                            const Data_PageNo    Root,
                            const Data_PageNo    Last)
: Data_BaseFile ( Data_UndoFile,
                  Data_PageId(Root, Data_PageRecoveryMode(Data_Dynamic,Data_Recoverable)),
                  Data_PageAccessManager(Trans, Data_UndoFile, 
                  Data_PageRecoveryMode(Data_Dynamic,Data_Recoverable), Root) ),
  m_Container (m_PageAccessManager, m_RootId, Last),
  m_Iterator  (m_PageAccessManager,*(REINTERPRET_CAST (SAPDBMem_IRawAllocator*, Trans.trAllocator_gg00))),
  m_TransNo   (TransNo),
  m_PageCount (0), // PTS 1124083 UH 2003-09-10 added
  m_GarbageCollectionNeeded(true)
{
    if ( Root.IsInvalid() )
        RTE_Crash( Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"root is nil, but file must exist here") );
    SAPDBTRACE_METHOD_DEBUG ("Log_UndoFile::Log_UndoFile(transno)", LogTrans_Trace, 5);
}

/* --------------------------------------------------------------------------- */
Log_UndoFile::Log_UndoFile (Data_PageAccessManager &Pam,
                            SAPDBMem_IRawAllocator &Allocator,
                            const tgg91_TransNo    &TransNo,
                            const Data_PageNo       Root,
                            const Data_PageNo       Last)
: Data_BaseFile ( Data_UndoFile,
                  Data_PageId(Root, Data_PageRecoveryMode(Data_Dynamic,Data_Recoverable)),
                  Data_PageAccessManager(Pam, Data_UndoFile, 
                  Data_PageRecoveryMode(Data_Dynamic,Data_Recoverable), Root) ),
  m_Container (m_PageAccessManager, m_RootId, Last),
  m_Iterator  (m_PageAccessManager, Allocator),
  m_TransNo   (TransNo),
  m_PageCount (0), // PTS 1124083 UH 2003-09-10 added
  m_GarbageCollectionNeeded(true)
{
    if ( Root.IsInvalid() )
        RTE_Crash( Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"root is nil, but file must exist here") );
    SAPDBTRACE_METHOD_DEBUG ("Log_UndoFile::Log_UndoFile(pam)", LogTrans_Trace, 5);
}

/* --------------------------------------------------------------------------- */
bool Log_UndoFile::Create (SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_UndoFile::Create", LogTrans_Trace, 5);

    SAPDBERR_ASSERT_STATE( ! IsCreated() );

    if ( m_TransNo.gg90IsNil() )
    {
        // PTS 1132283 UH 2004-11-22 changed crash to return false
        errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"m_TransNo not nil");
        return false;
    }

    m_PageAccessManager.Invalidate();
    
    m_RootId.Invalidate();
    
    PageIterator RootPageIter (m_PageAccessManager);

    if ( ! m_Container.Create(RootPageIter,errlist) ) // PTS 1121659 UH 2003-04-30
    {
        (void)m_Container.Drop(errlist);
        return false;
    }

    if ( ! m_Iterator.Initialize() )
    {
        (void)m_Container.Drop(errlist);
        return false;
    }

    m_PageCount = 1;

    m_GarbageCollectionNeeded = false;
    
    (*RootPageIter).SetPageCount (m_PageCount);
    (*RootPageIter).SetTransNo   (m_TransNo);
    (*RootPageIter).SaveLastGarbageCollectorPosition (Data_PageNo(), 0); // PTS ? UH 2002-03-15

    return true;
}

/* --------------------------------------------------------------------------- */
bool Log_UndoFile::Drop (SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_UndoFile::Drop", LogTrans_Trace, 5);

    SAPDBERR_ASSERT_STATE( IsCreated() );

    if( ! m_Iterator.Invalidate(errlist)
        ||
        ! m_Iterator.Delete(errlist)
        ||
        ! m_Container.Drop(errlist) )
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    m_RootId.Invalidate();
    m_PageAccessManager.Invalidate();
    return true;
}

/*! ---------------------------------------------------------------------------*/
bool Log_UndoFile::Verify(bool                  isCold,
                          SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_UndoFile::Verify", LogTrans_Trace, 5);
    
    if ( m_Container.Verify (isCold, errlist) )
    {
        if ( LogTrans_Trace.TracesLevel(6) )
            RTE_Message( Log_Exception(__CONTEXT__, LOG_INFO_VERIFY,
                         "UndoFile", SAPDB_ToString(GetRootId().PageNo()) ) );
    }
    else
    {
        WriteToTrace();
        m_PageAccessManager.AppendContextInfo(__CONTEXT__,errlist);
        AppendContextInfo(__CONTEXT__,errlist);
        m_PageAccessManager.ResetLastError();
        return false;
    }
    return true;
}

/*! ---------------------------------------------------------------------------
function:    WriteToTrace
description: This traces some basic information about the file to the trace.
*/
void Log_UndoFile::WriteToTrace (const char * title) const
{
    Data_BaseFile::WriteToTrace(title);
        
    if ( ! IsCreated() )
    {
        Kernel_VTrace() << "LogUndoFile: is no created.";
        return;
    }
    
    Kernel_VTrace() << "TransNo: " << m_TransNo.gg90GetInt4();
    m_Container.WriteToTrace();
}

/* --------------------------------------------------------------------------- */
Log_UndoFile::WriteBeforeImageResult 
    Log_UndoFile::WriteUndoEntry (      Log_BeforeImage &BeforeImage,
                                  const Log_IAction     &Action,
                                  Data_PageNo           &pageno,
                                  Data_PageOffset       &offset,
                                  SAPDBErr_MessageList  &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_UndoFile::WriteUndoEntry", LogTrans_Trace, 5);

    SAPDBERR_ASSERT_STATE( IsCreated() );

    const Data_SplitRecordLength LengthToReserve = 
                                    BeforeImage.GetPersistentLength() 
                                    +
                                    Action.GetPersistentLength();
    const Data_RecordLength      MinLengthToReserve = 
                                    BeforeImage.GetPersistentLength() 
                                    +
                                    Action.GetMinimalPersistentLength();
    
    SAPDBERR_ASSERT_ARGUMENT ( LengthToReserve >= MinLengthToReserve );
    
    pageno.Invalidate();
    offset = 0;

    m_GarbageCollectionNeeded |= Action.GarbageCollectionNeeded();

    SAPDB_UInt addedPages;

    // PTS 1121659 UH 2003-04-30
    if ( ! m_Container.ReserveSpace (LengthToReserve, MinLengthToReserve, m_Iterator, addedPages, errlist) )
    {
        AppendContextInfo(__CONTEXT__,errlist);
        
        (void)m_Iterator.Invalidate (errlist,false);
        return writeBeforeImageNoSpaceAvailable;
    }

    if ( ! m_Iterator.IsValid() )
    {
        BeforeImage.AppendContextInfo(errlist);
        Action.AppendContextInfo(__CONTEXT__,errlist);
        m_Iterator.AppendContextInfo(__CONTEXT__,errlist);
        AppendContextInfo(__CONTEXT__,errlist);
        SAPDBErr_MessageList auxerrlist;
        if(!m_Iterator.Invalidate (auxerrlist,false))
            errlist = errlist + auxerrlist;
        return writeBeforeImageError;
    }

    m_PageCount += addedPages;

    bool        derefResult = true;
    EntrySpace &space       = m_Iterator.Deref(derefResult,errlist);

    if(!derefResult)
    {
        BeforeImage.AppendContextInfo(errlist);
        Action.AppendContextInfo(__CONTEXT__,errlist);
        m_Iterator.AppendContextInfo(__CONTEXT__,errlist);
        AppendContextInfo(__CONTEXT__,errlist);
        m_Container.UndoReserveSpace(space);
        SAPDBErr_MessageList auxerrlist;
        if(!m_Iterator.Invalidate (auxerrlist,false))
            errlist = errlist + auxerrlist;
        return writeBeforeImageError;
    }

    SAPDBERR_ASSERT_STATE ( LengthToReserve == space.Length() );

    Data_SplitSpaceWriter Writer (space);

    if( ! BeforeImage.WritePersistentFormat (Writer, errlist)
        ||
        ! Action.WritePersistentFormat (Writer, errlist) )
    {
        m_Iterator.AppendContextInfo(__CONTEXT__,errlist);
        BeforeImage.AppendContextInfo(errlist);
        Action.AppendContextInfo(__CONTEXT__,errlist);
        AppendContextInfo(__CONTEXT__,errlist);
        // PTS 1117126 UH 2002-08-07
        m_Container.UndoReserveSpace(space);
        SAPDBErr_MessageList auxerrlist;
        if(!m_Iterator.Invalidate (auxerrlist,false))
            errlist = errlist + auxerrlist;
        return writeBeforeImageError;
    }
    
    m_Iterator.GetPosition (pageno, offset);
    
    if ( LogHistory_Check.ChecksLevel(5) // PTS 1117693 UH 2002-09-09 added check 
         &&
         offset != Data_PageSplitSpace::AlignRecordSize(offset) )
    {
        Kernel_VTrace() << "pageref not aligned: " << pageno << "." << offset << NewLine;
        RTE_Crash( Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"pageref not aligned") );
    }
            
    // PTS 1114461 UH 2002-02-26 begin
    if ( pageno == Data_PageNo(BeforeImage.GetPreviousImage().gg91RefPno())
         &&
         offset == Data_PageOffset(BeforeImage.GetPreviousImage().gg91RefPos()) )
	{
        // This is a rare case where before images are linked in cycle which can happen
        // If this might be detected the cycle is explicitly destroyed with the following
        BeforeImage.InvalidateReferenceToPreviousBeforeImage();
	}
    // PTS 1114461 UH 2002-02-26 end
    
    for ( int partno = space.CurrentPart(); partno >= 0; --partno )
    {
        space.GetPage(partno).SetLastEntrySequence  (BeforeImage.GetSequence());
    }
    
    if(!m_Iterator.Invalidate (errlist,true))
    {
        m_Iterator.AppendContextInfo(__CONTEXT__,errlist);
        BeforeImage.AppendContextInfo(errlist);
        Action.AppendContextInfo(__CONTEXT__,errlist);
        AppendContextInfo(__CONTEXT__,errlist);
        m_Container.UndoReserveSpace(space);
        return writeBeforeImageError;
    }
    
    return writeBeforeImageOk;
}

/* --------------------------------------------------------------------------- */
bool Log_UndoFile::GetLastUndoEntry (Data_AccessMode       accessmode,
                                     Iterator             &iter,
                                     SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_UndoFile::GetLastUndoEntry", LogTrans_Trace, 5);
    SAPDBERR_ASSERT_STATE( IsCreated() );
    return m_Container.End(iter, accessmode, errlist);
}

/* --------------------------------------------------------------------------- */
bool Log_UndoFile::GetUndoEntry (Data_PageNo             pageno,
                                 Data_PageOffset         offset,
                                 Data_AccessMode         accessmode,
                                 Data_SplitRecordLength  maxsize,
                                 Iterator               &iter,
                                 SAPDBErr_MessageList   &errlist)
{
    SAPDBTRACE_ROUTINE_DEBUG ("Log_UndoFile::GetUndoEntry", LogTrans_Trace, 5);

    if ( ! iter.SetPosition (pageno, offset, accessmode, errlist) )
    {
        if ( LogTrans_Trace.TracesLevel(6) )
        {
            Kernel_VTrace() << "position not found: " << pageno << "." << offset << " with accessmode: " << accessmode;
            WriteToTrace();
        }
        (void)iter.Invalidate(errlist);
        return true;
    }
    
    if ( ! iter.IsValid() ) // entry begin not found
    {
        return iter.Invalidate(errlist);
    }
    
    if ( maxsize > 0 && iter.GetSize() > maxsize )
    {
        if ( LogTrans_Trace.TracesLevel(6) )
        {
            Kernel_VTrace trace;
            trace << "position: " << pageno << "." << offset
                  << " with accessmode: " << accessmode << FlushLine;
            trace << "size: " << iter.GetSize() << " maxsize: " << maxsize << FlushLine;
            WriteToTrace();
        }
        errlist = Log_Exception(__CONTEXT__,LOG_GET_UNDO_ENTRY_MAXSIZE_MISMATCH,
                                SAPDB_ToString(maxsize),
                                SAPDB_ToString(iter.GetSize()));
        (void)iter.Invalidate(errlist);
        return false;
    }
    // assign the record space
    bool derefResult = true;
    
    (void)iter.Deref(derefResult,errlist);
        
    return derefResult;
}

/* --------------------------------------------------------------------------- */
bool Log_UndoFile::SavePageCountAndRelevanceForGC (SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_UndoFile::SavePageCountAndRelevanceForGC", LogTrans_Trace, 5);

    SAPDBERR_ASSERT_STATE( IsCreated() );

    Log_UndoFile::PageIterator pageiter (m_Container.GetPageAccessManager());
    if ( ! m_Container.Begin (pageiter, Data_ForUpdate, errlist)
         ||
         ! pageiter.IsValid() )
	{
        if(!errlist.IsEmpty())
            errlist = errlist + Log_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED, "root accessable");
        else
            errlist = Log_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED, "root accessable");
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
	}
    (*pageiter).SetPageCount      (m_PageCount);
    (*pageiter).SetIsRelevantForGC(m_GarbageCollectionNeeded);
    return true;
}
    
/* --------------------------------------------------------------------------- */
bool Log_UndoFile::RestorePageCountAndRelevanceForGC (SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_UndoFile::RestorePageCountAndRelevanceForGC", LogTrans_Trace, 5);

    SAPDBERR_ASSERT_STATE( IsCreated() );

    Log_UndoFile::PageIterator pageiter (m_Container.GetPageAccessManager());
    if ( ! m_Container.Begin (pageiter, Data_ForRead, errlist)
         ||
         ! pageiter.IsValid() )
	{
        if(!errlist.IsEmpty())
            errlist = errlist + Log_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED, "root accessable");
        else
            errlist = Log_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED, "root accessable");
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
	}
    m_PageCount               = (*pageiter).GetPageCount();
    m_GarbageCollectionNeeded = (*pageiter).GetIsRelevantForGC();
    return true;
}

/* --------------------------------------------------------------------------- */
bool Log_UndoFile::SaveLastGarbageCollectorPosition (Data_PageNo           pageno,
                                                     Data_PageOffset       offset,
                                                     SAPDBErr_MessageList &errlist)
{
    // PTS ? UH 2002-03-15 new
    SAPDBTRACE_METHOD_DEBUG ("Log_UndoFile::SaveLastGarbageCollectorPosition", LogTrans_Trace, 5);

    SAPDBERR_ASSERT_STATE( IsCreated() );

    Log_UndoFile::PageIterator pageiter (m_Container.GetPageAccessManager());
    if ( ! m_Container.Begin (pageiter, Data_ForUpdate, errlist)
         ||
         ! pageiter.IsValid() )
	{
        if(!errlist.IsEmpty())
            errlist = errlist + Log_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED, "root accessable");
        else
            errlist = Log_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED, "root accessable");
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
	}

    (*pageiter).SaveLastGarbageCollectorPosition(pageno, offset);
    
    return true;
}

/* --------------------------------------------------------------------------- */
bool Log_UndoFile::GetLastGarbageCollectorPosition (Data_PageNo          &pageno,
                                                    Data_PageOffset      &offset,
                                                    SAPDBErr_MessageList &errlist)
{
    // PTS ? UH 2002-03-15 new
    SAPDBTRACE_METHOD_DEBUG ("Log_UndoFile::GetLastGarbageCollectorPosition", LogTrans_Trace, 5);

    SAPDBERR_ASSERT_STATE( IsCreated() );

    Log_UndoFile::PageIterator pageiter (m_Container.GetPageAccessManager());
    if ( ! m_Container.Begin (pageiter, Data_ForRead, errlist)
         ||
         ! pageiter.IsValid() )
	{
        if(!errlist.IsEmpty())
            errlist = errlist + Log_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED, "root accessable");
        else
            errlist = Log_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED, "root accessable");
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
	}

    (*pageiter).GetLastGarbageCollectorPosition(pageno, offset);

    // the same like calling RestorePageCountAndRelevanceForGC()
    m_PageCount               = (*pageiter).GetPageCount();
    m_GarbageCollectionNeeded = (*pageiter).GetIsRelevantForGC();

    return true;
}

/* --------------------------------------------------------------------------- */
bool Log_UndoFile::GetLastEntrySequence (Log_EntrySequence    &LastEntrySequence,
                                         SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_UndoFile::GetLastEntrySequence", LogTrans_Trace, 5);

    LastEntrySequence.Invalidate();
    
    if(!m_Container.End (m_Iterator, Data_ForRead, errlist))
	{
        errlist = Log_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED, "last page accessable");
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
	}

    if ( ! m_Iterator.IsValid() )
        return false;
	
    LastEntrySequence = m_Iterator.GetCurrentPage().GetLastEntrySequence();
	return true;
}

