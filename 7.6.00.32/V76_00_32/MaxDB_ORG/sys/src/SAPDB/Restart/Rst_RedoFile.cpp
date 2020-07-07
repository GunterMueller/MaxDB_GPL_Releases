/*!
    @ingroup        Restart
    @file           Rst_RedoFile.cpp
    @author         UweH
    @brief          Implementation for class Rst_RedoFile.
*/
/*
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

*/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "DataAccess/Data_PageAccessManager.hpp"
#include "DataAccess/Data_Types.hpp"
#include "DataAccess/Data_SplitSpaceReader.hpp"
#include "DataAccess/Data_SplitSpaceWriter.hpp"
#include "Restart/Rst_Exceptions.hpp"
#include "Restart/Rst_RedoFile.hpp"

class SAPDBMem_IRawAllocator;

/* --------------------------------------------------------------------------- */
Rst_RedoFile::Rst_RedoFile (tgg00_TransContext &Trans,
                            const Data_PageNo   Root,
                            const Data_PageNo   Last)
: Data_BaseFile ( Data_RedoFile,
                  Data_PageId(Root, Data_PageRecoveryMode(Data_Dynamic,Data_Recoverable)),
                  Data_PageAccessManager(Trans, Data_RedoFile, 
                  Data_PageRecoveryMode(Data_Dynamic,Data_Recoverable), Root) ),
  m_Container (m_PageAccessManager, m_RootId, Last),
  m_Iterator  (m_PageAccessManager,*(REINTERPRET_CAST (SAPDBMem_IRawAllocator*, Trans.trAllocator_gg00))),
  m_TransNo   (Trans.trWriteTransId_gg00)
{
    SAPDBTRACE_METHOD_DEBUG ("ctor::Rst_RedoFile", LogTrans_Trace, 5);
}

/* --------------------------------------------------------------------------- */
Rst_RedoFile::Rst_RedoFile (Data_PageAccessManager &Pam,
                            SAPDBMem_IRawAllocator &Allocator,
                            const tgg91_TransNo    &TransNo,
                            const Data_PageNo       Root,
                            const Data_PageNo       Last)
: Data_BaseFile ( Data_RedoFile,
                  Data_PageId(Root, Data_PageRecoveryMode(Data_Dynamic,Data_Recoverable)),
                  Data_PageAccessManager(Pam, Data_RedoFile, 
                  Data_PageRecoveryMode(Data_Dynamic, Data_Recoverable), Root)),
  m_Container (m_PageAccessManager, m_RootId, Last),
  m_Iterator  (m_PageAccessManager, Allocator),
  m_TransNo   (TransNo)
{
    SAPDBTRACE_METHOD_DEBUG ("ctor::Rst_RedoFile(pam)", LogTrans_Trace, 5);
}

/* --------------------------------------------------------------------------- */
bool Rst_RedoFile::Create (SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Rst_RedoFile::Create", LogTrans_Trace, 5);

    SAPDBERR_ASSERT_STATE( ! IsCreated() );

    m_PageAccessManager.Invalidate();

    m_RootId.Invalidate();
    
    PageIterator RootPageIter (m_PageAccessManager);

    if ( ! m_Container.Create(RootPageIter,errlist) ) // PTS 1121659 UH 2003-04-30
    {
        AppendContextInfo(__CONTEXT__,errlist);
        (void)m_Container.Drop(errlist);
        return false;
    }

    if ( ! m_Iterator.Initialize(errlist) )
    {
        (void)m_Container.Drop(errlist);
        return false;
    }

    if ( m_TransNo.gg90IsNil() )
    {
        errlist = Restart_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"m_TransNo not nil");
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    (*RootPageIter).SetTransNo (m_TransNo);

    return true;
}

/* --------------------------------------------------------------------------- */
bool Rst_RedoFile::Drop (SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Drop::Rst_RedoFile", LogTrans_Trace, 5);

    SAPDBERR_ASSERT_STATE( IsCreated() );

    if ( ! Delete(errlist)
         ||
         !m_Container.Drop(errlist) )
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    m_RootId.Invalidate();
    m_PageAccessManager.Invalidate();
    return true;
}

/* --------------------------------------------------------------------------- */
bool Rst_RedoFile::Delete (Msg_List &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Delete::Rst_RedoFile", LogTrans_Trace, 5);

    if ( ! m_Iterator.Invalidate(errlist)
         ||
         ! m_Iterator.Delete(errlist))
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    return true;
}

/*!
   @returns        true, if the file is consistent

 */

bool Rst_RedoFile::Verify (bool                  isCold,
                           SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Rst_RedoFile::Verify", LogTrans_Trace, 5);
    if ( m_Container.Verify (isCold,errlist) )
    {
        if ( LogTrans_Check.ChecksLevel(5) )
            RTE_Message( Restart_Exception(__CONTEXT__, RST_INFO_VERIFY,
                         "RedoFile", SAPDB_ToString(GetRootId().PageNo()) ) );
    }
    else
    {
        errlist = errlist +
                 Restart_Exception(__CONTEXT__, RST_ERROR_VERIFY,
                     "RedoFile",
                     SAPDB_ToString(GetRootId().PageNo()),
                     SAPDB_ToString(m_PageAccessManager.GetLastError()) );
        AppendContextInfo(__CONTEXT__,errlist);
        m_PageAccessManager.ResetLastError();
        return false;
    }
    return true;
}

/*!
   @brief          This traces some basic information about the file to the trace.
 */

void Rst_RedoFile::WriteToTrace (const char * title) const
{
    Data_BaseFile::WriteToTrace (title);
        
    if ( ! IsCreated() )
    {
        Kernel_VTrace() << "LogRedoFile: is no created.";
        return;
    }
}

/* --------------------------------------------------------------------------- */
bool Rst_RedoFile::GetEntryInfo (Data_SplitSpaceReader &Reader,
                                 Log_IOSequenceNo      &IOSequence,
                                 Log_EntrySequence     &EntrySequence,
                                 SAPDBErr_MessageList  &errlist)
{
    SAPDBTRACE_ROUTINE_DEBUG ("Rst_RedoFile::GetEntryInfo", LogTrans_Trace, 5);
    
    SAPDB_Byte* pEntryHead;
    if ( Reader.Reserve (sizeof(EntryHead), pEntryHead) != Data_SplitSpaceReader::moreSpaceAvailable
         ||
         0 == pEntryHead )
	{
        Reader.AppendContextInfo(__CONTEXT__,errlist,"GetEntryInfo: Reserve failed");
        return false;
	}
    EntryHead& entryHead = *(REINTERPRET_CAST(EntryHead*,pEntryHead));
    IOSequence    = entryHead.IOSequence;
    EntrySequence = entryHead.Sequence;
    return true;
}

/* --------------------------------------------------------------------------- */



Data_SplitSpaceWriter
Rst_RedoFile::ReReserveSpace (Data_SplitRecordLength RestLength,
                              SAPDBErr_MessageList  &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Rst_RedoFile::ReReserveSpace", LogTrans_Trace, 5);

    SAPDBERR_ASSERT_STATE( IsCreated() );
    
    #ifdef SAPDB_SLOW
    Kernel_VTrace trace;
    #endif
    
    PageIterator pageIter (m_PageAccessManager);
    if(!m_Container.End (pageIter, Data_ForRead, errlist))
    {
        AppendContextInfo(__CONTEXT__,errlist,"Rst_RedoFile::ReReserveSpace no last page found");
        return Data_SplitSpaceWriter();
    }
    
    if ( pageIter.IsValid() )
    {
        const Log_EntrySequence searchedEntrySequence = (*pageIter).GetLastEntrySequence();

        // search start page of the last entry
        while (       pageIter.IsValid() 
                && ((*pageIter).GetFirstEntrySequence() == searchedEntrySequence)
                && ((*pageIter).GetFirstEntrySequence() != 0))
               
        {
            #ifdef SAPDB_SLOW
                if ( LogTrans_Trace.TracesLevel(9) )
                {
                    trace << "Rst_RedoFile searching for redosequence " << searchedEntrySequence 
                          << " Page contains [" << (*pageIter).GetFirstEntrySequence() << ","
                          <<  (*pageIter).GetLastEntrySequence() << "]" << FlushLine;
                }
            #endif
            if (!pageIter.MinusMinus(errlist))
            {
                AppendContextInfo(__CONTEXT__,errlist,"Rst_RedoFile::ReReserveSpace backward search interupted");
                return Data_SplitSpaceWriter();
            }
        }
        #ifdef SAPDB_SLOW
        if ( LogTrans_Trace.TracesLevel(9) )
        {
            trace << "Rst_RedoFile::ReReserve found start for redosequence " << searchedEntrySequence 
                  << " Page contains [" << (*pageIter).GetFirstEntrySequence() << ","
                  <<  (*pageIter).GetLastEntrySequence() << "] or on next page" << FlushLine;
        }
        #endif

        if ( pageIter.IsValid() )
        {
            Data_PageSplitSpaceForwardReadIterator recordIter;

            if ((*pageIter).GetFirstEntrySequence() == searchedEntrySequence)
            // the entry starts at the begin of the current page
            {
                #ifdef SAPDB_SLOW
                    if ( LogTrans_Trace.TracesLevel(9) )
                    {
                        trace << "Rst_RedoFile::ReReserve found entry at start of page" << FlushLine;
                    }
                #endif
                (*pageIter).Begin (recordIter);
            }
            else if (    ((*pageIter).GetLastEntrySequence()    == searchedEntrySequence)
                     || (((*pageIter).GetLastEntrySequence()+1) == searchedEntrySequence))
            // the entry is the last one of the current page or starts at the next page
            // Remark: If the last entry is not complete on this page, LastEntrySequence is not set
            // to his entrySequence
            {
                (*pageIter).Begin (recordIter);

                // the first entry may be incomplete (i.e. the trailer of a previous entry)
                // Thus it has to be skipped. Anyway it cannot be the start of the searched
                // entry, because the FirstEntrySequence of this pages is not equal to the 
                // searched one. This is not true for the first page which should always 
                // start with a valid entry
                if ((*pageIter).GetFirstEntrySequence() != 0)
                {
                    ++recordIter;
                }

                while    ( recordIter.IsValid() && 
                           (reinterpret_cast<EntryHead*>(*recordIter))->Sequence != searchedEntrySequence)
                {
                    #ifdef SAPDB_SLOW
                        if ( LogTrans_Trace.TracesLevel(9) )
                        {
                            trace << "Rst_RedoFile::ReReserve checked entrysequence " << (reinterpret_cast<EntryHead*>(*recordIter))->Sequence 
                                  << FlushLine;
                        }
                    #endif
                    ++recordIter;
                }
                if (! recordIter.IsValid())
                // the entry is not on the current page => search on the next page
                {
                    if (!pageIter.PlusPlus(errlist))
                    {
                        AppendContextInfo(__CONTEXT__,errlist,"Rst_RedoFile::ReReserveSpace forward chain interupted");
                        return Data_SplitSpaceWriter();
                    }
                    (*pageIter).Begin (recordIter);
                    if ((*pageIter).GetFirstEntrySequence() != searchedEntrySequence)
                    {
                        AppendContextInfo(__CONTEXT__,errlist,"Rst_RedoFile::ReReserveSpace entrysequence not found (2) ");
                        return Data_SplitSpaceWriter();
                    }
                    #ifdef SAPDB_SLOW
                        if ( LogTrans_Trace.TracesLevel(9) )
                        {
                            trace << "Rst_RedoFile::ReReserve found entrysequence page " << (*pageIter).ReadPageId()  
                                  << " entrysequence " << (reinterpret_cast<EntryHead*>(*recordIter))->Sequence 
                                  << FlushLine;
                        }
                    #endif
                }
                #ifdef SAPDB_SLOW
                else
                {
                    if ( LogTrans_Trace.TracesLevel(9) )
                    {
                        trace << "Rst_RedoFile::ReReserve found entrysequence " << (reinterpret_cast<EntryHead*>(*recordIter))->Sequence 
                              << FlushLine;
                    }
                }
                #endif
            }
            else
            // the entry is neither on the previous nor on this page => error
            {
                AppendContextInfo(__CONTEXT__,errlist,"Rst_RedoFile::ReReserveSpace entry not found in page");
                WriteToTrace();
                return Data_SplitSpaceWriter();
            }

            const Data_SplitRecordLength  bytesAlreadyWritten =   recordIter.RestLength() 
                                                                - sizeof(Data_SplitRecordLength) 
                                                                - RestLength;
            #ifdef SAPDB_SLOW
            trace   << "RedoFile::ReReserveSpace skipping the first " << bytesAlreadyWritten << " bytes"
                    << " recordIter.RestLength = " <<  recordIter.RestLength() 
                    << " RestLength = " <<  RestLength 
                    << FlushLine;
            #endif
    
            if (pageIter.IsValid())
            {
                Data_PageNo           pno((*pageIter).ReadPageId());
                Data_PageOffset       offset(recordIter.GetPosition());
                if ( ! pageIter.Invalidate(errlist))
                {
                    AppendContextInfo(__CONTEXT__,errlist,"Rst_RedoFile Invalidate pageIter failed");
                    return Data_SplitSpaceWriter();
                }
                

                #ifdef SAPDB_SLOW
                trace << "RedoFile::ReReserveSpace entrypart in RedoFile: page " << pno
                      << " offset " << offset << FlushLine;
                #endif
                if ( ! m_Iterator.SetPosition(pno,
                                              offset,
                                              Data_ForUpdate,
                                              errlist))
                {
                    AppendContextInfo(__CONTEXT__,errlist,"Rst_RedoFile SetPosition failed");
                    return Data_SplitSpaceWriter();
                }
            }

            bool        derefResult = true;
            EntrySpace &space       = m_Iterator.Deref(derefResult,errlist);
            if ( ! derefResult )
            {
                AppendContextInfo(__CONTEXT__,errlist,"Rst_RedoFile Could not deref iterator");
                return Data_SplitSpaceWriter();
            }                    

            Data_SplitSpaceWriter  writer (space);
            
            // skip the already written parts of the logentry
            #ifdef SAPDB_SLOW
            trace << "RedoFile::ReReserveSpace skipping " << bytesAlreadyWritten << " bytes" << FlushLine;
            #endif
            writer.SkipBytes(bytesAlreadyWritten);

            return writer;            
        }
        else
        {
            AppendContextInfo(__CONTEXT__,errlist,"Rst_RedoFile::ReReserveSpace start of partially copied entry not found");
            return Data_SplitSpaceWriter();
        }            
    }
    else
    {
        AppendContextInfo(__CONTEXT__,errlist,"Rst_RedoFile::ReReserveSpace no last page found");
        return Data_SplitSpaceWriter();
    }
}
    
    
    
/* --------------------------------------------------------------------------- */



Data_SplitSpaceWriter
Rst_RedoFile::ReserveSpace (Data_SplitRecordLength Length,
                            Data_RecordLength      MinimalLength,
                            Log_EntrySequence      EntrySequence,
                            Log_IOSequenceNo       IOSequence,
                            SAPDBErr_MessageList  &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Rst_RedoFile::ReserveSpace", LogTrans_Trace, 5);

    SAPDBERR_ASSERT_STATE( IsCreated() );

    SAPDBERR_ASSERT_ARGUMENT ( Length >= MinimalLength );
        
    SAPDB_UInt addedPages;
    
    if ( ! m_Container.ReserveSpace (Length        + sizeof(EntryHead),
                                     MinimalLength + sizeof(EntryHead),
                                     m_Iterator,
                                     addedPages,
                                     errlist) )
    {
        AppendContextInfo(__CONTEXT__,errlist,"Rst_RedoFile::ReserveSpace");
        return Data_SplitSpaceWriter();
    }

    if ( ! m_Iterator.IsValid() )
    {
        errlist = Restart_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED, "iter.IsValid");
        AppendContextInfo(__CONTEXT__,errlist,"Rst_RedoFile::ReserveSpace");
        return Data_SplitSpaceWriter();
    }
	bool        derefResult = true;
	EntrySpace &space       = m_Iterator.Deref(derefResult,errlist);
	if ( ! derefResult )
	{
        AppendContextInfo(__CONTEXT__,errlist,"Rst_RedoFile Could not deref iterator");
        return Data_SplitSpaceWriter();
	}
    if ( Length != space.Length()-sizeof(EntryHead) )
    {
        errlist = Restart_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED, "ReserveSpace() requested and allocated length");
        space.AppendContextInfo(__CONTEXT__,errlist);
        AppendContextInfo(__CONTEXT__,errlist,"Rst_RedoFile::ReserveSpace");
        return Data_SplitSpaceWriter();
    }

	Data_SplitSpaceWriter  writer (space);
    SAPDB_Byte            *SourcePart;

    if ( writer.Reserve (sizeof(EntryHead), SourcePart)
         != Data_SplitSpaceWriter::moreSpaceAvailable )
    {
        errlist = Restart_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED, "Data_SplitSpaceWriter::Reserve returns MoreSpace");
        AppendContextInfo(__CONTEXT__,errlist,"Rst_RedoFile::ReserveSpace");
        return Data_SplitSpaceWriter();
    }

    EntryHead& entryHead = *(REINTERPRET_CAST(EntryHead*,SourcePart));
    entryHead.Sequence   = EntrySequence;
    entryHead.IOSequence = IOSequence;
    m_IOSequence = IOSequence; // PTS 1134170 mb 2005-03-10
    
    if ( space.NewPageWasAppended() )
    {
        SAPDB_UInt partno = space.CurrentPart();
        if ( 0 == partno )
        {
            // mark the first page
            Rst_RedoPage& page = space.GetPage(partno);
            page.SetFirstEntrySequence (EntrySequence);
            page.SetLastEntrySequence  (EntrySequence);
        }
        else
            while ( partno > 0 )
            {
                // mark all pages but not the first
                Rst_RedoPage& page = space.GetPage(partno);
                page.SetFirstEntrySequence (EntrySequence);
                page.SetLastEntrySequence  (EntrySequence);
                --partno;
            }
    }
    else
    {
        Rst_RedoPage &lastpage = space.GetPage(space.CurrentPart());
        lastpage.SetLastEntrySequence (EntrySequence);
    }
    return writer;
}

/* --------------------------------------------------------------------------- */
bool Rst_RedoFile::ReleaseSpace(bool                   undoChanges,
                                SAPDBErr_MessageList  &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Rst_RedoFile::ReleaseSpace", LogTrans_Trace, 5);
    if ( undoChanges )
    {
        bool          derefResult = true;
        EntrySpace   &space = m_Iterator.Deref(derefResult,errlist);

        if(!derefResult)
        {
            AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
        Rst_RedoPage &firstpage = space.GetPage(0);
        // PTS 1127350 UH 2004-01-22 added following line. Otherwise this entry is never redone.
        firstpage.SetLastEntrySequence(firstpage.GetLastEntrySequence()-1);
        
        // PTS 1117126 UH 2002-08-07
        m_Container.UndoReserveSpace(space);
    }

    if ( ! m_Iterator.Invalidate (errlist, ! undoChanges) )
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    return true;
}

/* --------------------------------------------------------------------------- */
bool Rst_RedoFile::Truncate (Log_EntrySequence     stopSequence,
                             SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Rst_RedoFile::Truncate", LogTrans_Trace, 5);
    
    if ( LogTrans_Trace.TracesLevel(6) )
        Kernel_VTrace() << "stopSequence: " << stopSequence;

    if ( stopSequence.IsInvalid() )
    {
        if ( LogTrans_Trace.TracesLevel(6) )
        {
            Kernel_VTrace() << "stopsequence: is invalid: trunc to empty file";
            WriteToTrace();
        }
        if (! m_Container.Truncate ( m_Container.RootId().PageNo(),
                                     Data_Page::MinSpaceOffset(),
                                     errlist ) )
        {
            AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
        return true;
    }
    
    PageIterator pageIter (m_PageAccessManager);
    
    if ( LogTrans_Trace.TracesLevel(6) )
        Kernel_VTrace() << "lastpageno: " << m_Container.LastPageNo();

    if(!m_Container.End (pageIter, Data_ForRead, errlist))
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    
    while ( pageIter.IsValid()
            &&
            (*pageIter).PageNo() != m_Container.RootId().PageNo()
            &&
            (*pageIter).GetFirstEntrySequence() > stopSequence )
    {
        if ( LogTrans_Trace.TracesLevel(6) )
            Kernel_VTrace() << "Pageno: "              << (*pageIter).PageNo()
                            << " FirstSequenceInPage: " << (*pageIter).GetFirstEntrySequence()
                            << " LastSequenceInPage: " << (*pageIter).GetLastEntrySequence();
        if(!pageIter.MinusMinus(errlist))
        {
            AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
    }

    if ( ! pageIter.IsValid() )
    {
        Kernel_VTrace() << "stopsequence: " << stopSequence << NewLine;
        WriteToTrace();
        errlist = Restart_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                                    "Rst_RedoFile::Truncate() file is corrupt (page not found)");
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }

    // truncate to valid position
    
    Data_PageNo                        pnoOfNewLastPage = (*pageIter).PageNo();
    Data_PageSplitSpaceForwardReadIterator recordIter;
    Data_PageOffset                    newFreeOffset = 0; // 0 is invalid => no truncation of offset

    (*pageIter).Begin (recordIter);
    
    if ( ! recordIter.IsValid() )
    {
        // page is empty
        #ifdef SAPDB_QUICK
        if ( ! (*pageIter).IsEmpty() )
        {
            Kernel_VTrace() << "stopsequence: " << stopSequence;
            WriteToTrace();
            RTE_Crash( Restart_Exception(__CONTEXT__,
                       SAPDBERR_ASSERT_STATE_FAILED,
                       "Rst_RedoFile::Truncate() file is corrupt (page not empty)") );
        }
        #endif
        newFreeOffset = Data_Page::MinSpaceOffset();
    }
    else
    {
        // The first logentry in the page has written its sequence to the node header
        // during Reserve().
        // Because the recorditer cannot determine if the current entry part is a restpart
        // of a split logentry also written on the previous page,
        // we can only interpret the second entry on the page and skip the first entry.
		// The root page must always begin with a new logentry, so the first entry is not skipped.

        if ( pnoOfNewLastPage != m_Container.RootId().PageNo() )
			++recordIter;

        Log_EntrySequence sequenceFromPage = (*pageIter).GetFirstEntrySequence();
        Log_EntrySequence sequenceFromEntry;
        
        while ( recordIter.IsValid() )
        {
            sequenceFromEntry = (reinterpret_cast<EntryHead*>(*recordIter))->Sequence;
            
            #ifdef SAPDB_QUICK
            if ( sequenceFromEntry < sequenceFromPage )
            {
                Kernel_VTrace() << "stopsequence: "        << stopSequence
                                << ", sequenceFromPage: "  << sequenceFromPage
                                << ", sequenceFromEntry: " << sequenceFromEntry
                                << NewLine;
                WriteToTrace();
                RTE_Crash( Restart_Exception(__CONTEXT__,
                           SAPDBERR_ASSERT_STATE_FAILED,
                           "Rst_RedoFile::Truncate() invalid entry sequence") );
            }
            #endif
            
            // stopSequence is the last wanted entry, all others are removed
            if ( sequenceFromEntry > stopSequence )
                break;
            ++recordIter;
        }
        
        if ( recordIter.IsValid() )
            newFreeOffset = recordIter.GetPosition();
    }
    
    if(!(*pageIter).Deassign(errlist)) // release for read to get new for update
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    
    if ( LogTrans_Trace.TracesLevel(6) )
        Kernel_VTrace() << "truncate to " << pnoOfNewLastPage << "." << newFreeOffset;
    if(!m_Container.Truncate (pnoOfNewLastPage, newFreeOffset, errlist))
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    return true;
}

/* --------------------------------------------------------------------------- */
bool Rst_RedoFile::GetLastEntrySequence (Log_EntrySequence    &entrySequence,
                                         SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Rst_RedoFile::GetLastEntrySequence", LogTrans_Trace, 5);
    
    entrySequence.Invalidate();
    
    PageIterator pageIter (m_PageAccessManager);

    if(!m_Container.End (pageIter, Data_ForRead, errlist))
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    if ( pageIter.IsValid() )
    {
        entrySequence = (*pageIter).GetLastEntrySequence();
    }
    return true;
}

