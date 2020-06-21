/*!**************************************************************************

  module      : Log_OpenTransFile.cpp
  special area: Logging
  responsible : UweH
  last changed: 2000-09-25  12:00
  copyright:    Copyright (c) 2000-2005 SAP AG
  description : Implementation for class Log_OpenTransFile.



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
#include "Logging/Log_Exceptions.hpp"
#include "Logging/Log_Transaction.hpp"
#include "Logging/Log_UndoFile.hpp"
#include "Restart/Rst_RedoFile.hpp"
#include "Logging/Log_OpenTransFile.hpp"

/* --------------------------------------------------------------------------- */
Log_OpenTransFile::Log_OpenTransFile (tgg00_TransContext          &Trans,
                                      const Log_SavepointSequence  SavepointSequence,
                                      bool                         bIsRedoSavepoint)
: Data_BaseFile ( Data_OpenTransFile,
                  Data_PageId(Data_PageNo(), Data_PageRecoveryMode(Data_Dynamic,Data_Recoverable)),
                  Data_PageAccessManager(Trans, Data_OpenTransFile, Data_PageRecoveryMode(), Data_PageNo()) ),
  m_Container         (m_PageAccessManager, m_RootId, Log_Transaction::GetPersistentLength()),
  m_Iterator          (m_PageAccessManager),
  m_SavepointSequence (SavepointSequence),
  m_CreatedDuringRedo (bIsRedoSavepoint),
  m_Allocator         (*(reinterpret_cast<SAPDBMem_IRawAllocator*>(Trans.trAllocator_gg00)))
{
    SAPDBTRACE_METHOD_DEBUG ("Log_OpenTransFile::Log_OpenTransFile_FOR_CREATE", LogTrans_Trace, 5);
}

/* --------------------------------------------------------------------------- */
Log_OpenTransFile::Log_OpenTransFile (tgg00_TransContext &Trans,
                                      const Data_PageNo   Root,
                                      const EntryLayout   entryLayout)
: Data_BaseFile ( Data_OpenTransFile,
                  Data_PageId(Root, Data_PageRecoveryMode(Data_Dynamic,Data_Recoverable)),
                  Data_PageAccessManager(Trans, Data_OpenTransFile, Data_PageRecoveryMode(), Root) ),
  m_Container         (m_PageAccessManager, m_RootId, HasQueueId == entryLayout
                                                      ? Log_Transaction::GetPersistentLength() 
                                                      : Log_Transaction::GetOldPersistentLength() ),
  m_Iterator          (m_PageAccessManager),
  m_CreatedDuringRedo (false),
  m_Allocator         (*(reinterpret_cast<SAPDBMem_IRawAllocator*>(Trans.trAllocator_gg00)))
{
    SAPDBTRACE_METHOD_DEBUG ("Log_OpenTransFile::Log_OpenTransFile_FOR_READ", LogTrans_Trace, 5);
}

/* --------------------------------------------------------------------------- */
Log_OpenTransFile::Log_OpenTransFile (tgg00_TransContext &Trans,
                                      const Data_PageNo   Root)
: Data_BaseFile ( Data_OpenTransFile,
                  Data_PageId(Root, Data_PageRecoveryMode(Data_Dynamic,Data_Recoverable)),
                  Data_PageAccessManager(Trans, Data_OpenTransFile, Data_PageRecoveryMode(), Root) ),
  m_Container         (m_PageAccessManager, m_RootId, Log_Transaction::GetPersistentLength() ),
  m_Iterator          (m_PageAccessManager),
  m_CreatedDuringRedo (false),
  m_Allocator         (*(reinterpret_cast<SAPDBMem_IRawAllocator*>(Trans.trAllocator_gg00)))
{
    SAPDBTRACE_METHOD_DEBUG ("Log_OpenTransFile::Log_OpenTransFile_FOR_READ", LogTrans_Trace, 5);
}

/* --------------------------------------------------------------------------- */
bool Log_OpenTransFile::Create (SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_OpenTransFile::Create", LogTrans_Trace, 5);

    SAPDBERR_ASSERT_STATE( ! IsCreated() );

    m_PageAccessManager.Invalidate();

    m_RootId.Invalidate();
    
    PageIterator RootPageIter (m_PageAccessManager);

    if ( ! m_Container.Create(RootPageIter, errlist) ) // PTS 1121659 UH 2003-04-30
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }

    (*RootPageIter).SetSavepointSequence (m_SavepointSequence);
    (*RootPageIter).SetCreatedDuringRedo (m_CreatedDuringRedo);

    return true;
}

/* --------------------------------------------------------------------------- */
bool Log_OpenTransFile::Drop (SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_OpenTransFile::Drop", LogTrans_Trace, 5);

    SAPDBERR_ASSERT_STATE( IsCreated() );

    if(!m_Iterator.Invalidate(errlist))
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    if(!m_Container.Drop(errlist))
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    m_RootId.Invalidate();
    m_PageAccessManager.Invalidate();
    return true;
}

/* --------------------------------------------------------------------------- */
bool Log_OpenTransFile::Verify (bool                  isCold,
                                SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_OpenTransFile::Verify", LogTrans_Trace, 5);
    
    if ( m_Container.Verify (isCold, errlist) )
    {
        if ( LogTrans_Trace.TracesLevel(6) )
            RTE_Message( Log_Exception(__CONTEXT__, LOG_INFO_VERIFY,
                         "OpenTransFile", SAPDB_ToString(GetRootId().PageNo()) ) );
    }
    else
    {
        if ( ! isCold )
            return true; // In online mode it may happen that during the check a savepoint was written
                         // Then the old file identified by the old previous restartrecord ist not valid anymore
                         // So any error is ignored here.
    
        RTE_Message( Log_Exception(__CONTEXT__, LOG_ERROR_VERIFY,
                     "OpenTransFile",
                     SAPDB_ToString(GetRootId().PageNo()),
                     SAPDB_ToString(m_PageAccessManager.GetLastError()) ) );
        m_PageAccessManager.ResetLastError();
        return false;
    }

    if ( ! isCold )
        return true; // In online mode the transactions cannot be checked.
    
    if(!m_Container.Begin (m_Iterator, Data_ForRead, errlist))
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    
    Log_Transaction::Persistent *transentry;
    bool                         result = true;
    tgg91_TransNo                dummyTransNo;

    dummyTransNo.gg90SetNil();

    while ( m_Iterator.IsValid() )
    {
        transentry = reinterpret_cast<Log_Transaction::Persistent*>(*m_Iterator);
        
        if ( LogTrans_Trace.TracesLevel(6) )
            RTE_Message( Log_Exception(__CONTEXT__, LOG_INFO_VERIFY_OPENTRANS,
                         SAPDB_ToString(transentry->TransNo.gg90GetInt4()) ) );

        if ( Data_PageNo(transentry->UndoFileRootPageNo).IsValid() )
        {
            Log_UndoFile undofile ( m_PageAccessManager, 
                                    m_Allocator,
                                    dummyTransNo,
                                    transentry->UndoFileRootPageNo,
                                    transentry->UndoFileLastPageNo);
            result &= undofile.Verify(isCold,errlist);
            (void)undofile.Invalidate(errlist);
        }
        
        if ( Data_PageNo(transentry->RedoFileRootPageNo).IsValid() )
        {
            Rst_RedoFile redofile ( m_PageAccessManager,
                                    m_Allocator,
                                    dummyTransNo,
                                    transentry->RedoFileRootPageNo,
                                    transentry->RedoFileLastPageNo );
            result &= redofile.Verify(isCold,errlist);
        }
        
        result = m_Iterator.PlusPlus(errlist);
    }
    return result;
}

/* --------------------------------------------------------------------------- */
bool Log_OpenTransFile::WriteOpenTrans (Log_Transaction      &OpenTrans,
                                        SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_OpenTransFile::WriteOpenTrans", LogTrans_Trace, 5);

    SAPDBERR_ASSERT_STATE( IsCreated() );
    SAPDBERR_ASSERT_STATE( m_Container.GetSpaceSize() == Log_Transaction::GetPersistentLength() );

    if ( ! m_Container.ReserveSpace ( m_Iterator, errlist, false /* DO NOT USE FOR STRUCTURE CHANGE */ ) )
    {
        errlist = errlist + Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"ReserveSpace");
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }

    if ( ! m_Iterator.IsValid() )
    {
        errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"m_Iterator.IsValid()");
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    
    OpenTrans.WritePersistentFormat (*m_Iterator, m_Container.GetSpaceSize());

    if(!m_Iterator.Invalidate (errlist))
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    if(!m_Container.UnLock(errlist)) // PTS 1127083 UH 2004-01-14 
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    return true;
}

/* --------------------------------------------------------------------------- */
Log_OpenTransFile::Iterator&
Log_OpenTransFile::GetFirstOpenTrans (Log_SavepointSequence  SavepointSequence,
                                      SAPDBErr_MessageList  &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_OpenTransFile::GetFirstOpenTrans", LogTrans_Trace, 5);
    if ( ! m_Container.Begin (m_Iterator, Data_ForRead, errlist) )
    {
        errlist = errlist + Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"m_Container.Begin");
        AppendContextInfo(__CONTEXT__,errlist);
    }
    else
    {
        if ( m_Iterator.GetPage().GetSavepointSequence() != SavepointSequence )
        {
            errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                       "Log_OpenTransFile::GetFirstOpenTrans: savepoint sequence mismatch");
            AppendContextInfo(__CONTEXT__,errlist);
        }
        m_CreatedDuringRedo = m_Iterator.GetPage().GetCreatedDuringRedo();
    }
    return m_Iterator;
}
/* --------------------------------------------------------------------------- */
bool Log_OpenTransFile::SetOldestKnownIOSequence (Log_IOSequenceNo      oldestEOTSequence,
                                                  SAPDBErr_MessageList &errlist)
{
    // PTS 1124684 UH 2003-10-16 new
    SAPDBTRACE_METHOD_DEBUG ("Log_OpenTransFile::SetOldestKnownIOSequence", LogTrans_Trace, 5);
    
    oldestEOTSequence.Invalidate();
    
    if (!m_Container.Begin (m_Iterator, Data_ForUpdate, errlist))
    {
        errlist = errlist + Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"m_Container.Begin");
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    
    if ( m_Iterator.IsValid() )
    	m_Iterator.GetPage().SetOldestKnownIOSequence(oldestEOTSequence);
        
    return true;
}

/* --------------------------------------------------------------------------- */
Log_IOSequenceNo Log_OpenTransFile::GetOldestKnownIOSequence (SAPDBErr_MessageList &errlist)
{
    // PTS 1124684 UH 2003-10-16 new
    SAPDBTRACE_METHOD_DEBUG ("Log_OpenTransFile::GetOldestKnownIOSequence", LogTrans_Trace, 5);
    errlist.ClearMessageList();
    if ( ! m_Container.Begin (m_Iterator, Data_ForRead, errlist) )
	{
        errlist = errlist + Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"m_Container.Begin");
        AppendContextInfo(__CONTEXT__,errlist);
        return Log_IOSequenceNo();
	}
    
    if ( m_Iterator.GetPage().IsAssigned() )
        return m_Iterator.GetPage().GetOldestKnownIOSequence();
	
    return Log_IOSequenceNo();
}

/* --------------------------------------------------------------------------- */
void Log_OpenTransFile::AppendContextInfo (SAPDB_Char  const * const  filename,
                                           SAPDB_UInt4 const          linenumber,
                                           SAPDBErr_MessageList      &errlist,
                                           SAPDB_Char  const * const  title) const
{
    errlist = errlist + Log_Exception(filename,linenumber,LOG_OPENTRANS_FILE_INFO,
                                      title==0?"Log_OpenTransFile":title,
                                      SAPDB_ToString(m_SavepointSequence));
    Data_BaseFile::AppendContextInfo(filename,linenumber,errlist,"Log_OpenTransFile::Data_BaseFile::Info");
    m_Container.AppendContextInfo(filename,linenumber,errlist,"Log_OpenTransFile::Container::Info");
    m_Iterator.AppendContextInfo(filename,linenumber,errlist,"Log_OpenTransFile::Iterator::Info");
}
