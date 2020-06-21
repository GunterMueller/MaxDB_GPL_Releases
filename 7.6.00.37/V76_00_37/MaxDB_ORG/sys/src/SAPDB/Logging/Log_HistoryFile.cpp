/*!**************************************************************************

  module      : Log_HistoryFile.cpp
  special area: Logging
  responsible : UweH
  last changed: 2000-09-25  12:00
  copyright:    Copyright (c) 2000-2005 SAP AG
  description : Implementation for class Log_HistoryFile.



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

#include "ggg00.h"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "Logging/Log_Exceptions.hpp"
#include "Logging/Log_UndoFile.hpp"
#include "Logging/Log_HistoryFile.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/
/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/
/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*===========================================================================*
 *  DEFINITION OF INLINE METHODS                                              *
 *===========================================================================*/

/* --------------------------------------------------------------------------- */
Log_HistoryFile::Log_HistoryFile (tgg00_TransContext &Trans,
                                  const Data_PageNo   Root,
                                  const Data_PageNo   Last)
: Data_BaseFile       ( Data_HistoryFile,
                        Data_PageId(Root, Data_PageRecoveryMode(Data_Dynamic,Data_Recoverable)),
                        Data_PageAccessManager(Trans, Data_HistoryFile, 
                        Data_PageRecoveryMode(Data_Dynamic,Data_Recoverable), Root)),
  m_Container         (m_PageAccessManager, m_RootId, Last, sizeof(UndoFileInfo)),
  m_ContainerIterator (m_PageAccessManager),
  m_Iterator          (m_ContainerIterator),
  m_Allocator         (*(reinterpret_cast<SAPDBMem_IRawAllocator*>(Trans.trAllocator_gg00)))
{
    SAPDBTRACE_METHOD_DEBUG ("Log_HistoryFile::Log_HistoryFile", LogHistory_Trace, 5);
}

/* --------------------------------------------------------------------------- */
Log_HistoryFile::Log_HistoryFile (Data_PageAccessManager &Pam,
                                  SAPDBMem_IRawAllocator &Allocator,
                                  const Data_PageNo       Root,
                                  const Data_PageNo       Last)
: Data_BaseFile       ( Data_HistoryFile,
                        Data_PageId(Root, Data_PageRecoveryMode(Data_Dynamic,Data_Recoverable)),
                        Data_PageAccessManager(Pam, Data_HistoryFile, 
                        Data_PageRecoveryMode(Data_Dynamic,Data_Recoverable), Root)),
  m_Container         (m_PageAccessManager, m_RootId, Last, sizeof(UndoFileInfo)),
  m_ContainerIterator (m_PageAccessManager),
  m_Iterator          (m_ContainerIterator),
  m_Allocator         (Allocator)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_HistoryFile::Log_HistoryFile(pam)", LogHistory_Trace, 5);
}

/* --------------------------------------------------------------------------- */
bool Log_HistoryFile::Create (SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_HistoryFile::Create", LogHistory_Trace, 5);

    SAPDBERR_ASSERT_STATE( ! IsCreated() );

    m_PageAccessManager.Invalidate();

    m_RootId.Invalidate();
    
    PageIterator RootPageIter (m_PageAccessManager);

    bool result = m_Container.Create(RootPageIter,errlist); // PTS 1121659 UH 2003-04-30

    result &= m_ContainerIterator.Invalidate(errlist);

    return result;
}

/* --------------------------------------------------------------------------- */
bool Log_HistoryFile::Drop (SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_HistoryFile::Drop", LogHistory_Trace, 5);

    SAPDBERR_ASSERT_STATE( IsCreated() );
    bool result = m_ContainerIterator.Invalidate(errlist);
    result &= m_Container.Drop(errlist);
    m_RootId.Invalidate();
    m_PageAccessManager.Invalidate();
    if(!result)
        AppendContextInfo(__CONTEXT__,errlist);
    return result;
}

/*! ---------------------------------------------------------------------------
function:    Verify
returns:     true, if the file is consistent
*/
bool Log_HistoryFile::Verify (bool                  isCold,
                              SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_HistoryFile::Verify", LogHistory_Trace, 5);
    
    bool result = m_Container.Verify (isCold,errlist);
    
    if ( result )
    {
        if ( LogTrans_Trace.TracesLevel(6) )
            RTE_Message( Log_Exception(__CONTEXT__, LOG_INFO_VERIFY,
                         "HistoryFile", SAPDB_ToString(GetRootId().PageNo()) ) );
    }
    else
    {
        RTE_Message( Log_Exception(__CONTEXT__, LOG_ERROR_VERIFY,
                     "HistoryFile",
                     SAPDB_ToString(GetRootId().PageNo()),
                     SAPDB_ToString(m_PageAccessManager.GetLastError()) ) );
        m_PageAccessManager.ResetLastError();
        return false;
    }
    
    UndoFileInfo *fileinfo;

    // PTS 1116541 TS 2002-07-05
    PageIterator    &pageIter = m_Iterator.GetContainerIter().GetPageIterator();
    
    if(!m_Container.Begin (m_ContainerIterator, Data_ForRead, errlist))
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }

    // Check whether the first page of the history file chain contains
    // a valid undo file info entry

    if( pageIter.IsValid() )
    {
        if( ! m_ContainerIterator.IsValid() )
        {
            // page exists but contains no valid undo file infos
            // therefore the ppageIter is set to the subsequent
            // page. Note that this will update the m_ContainerIterator
            // too, because the pageIter is a member of the m_ContainerIterator.
            if(!pageIter.PlusPlus(errlist))
            {
                AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }
            if( pageIter.IsValid() )
            {
                if(!m_Container.Set ( m_ContainerIterator, ( *pageIter ).PageNo(), 
                    Data_ForRead, true, errlist ))
                {
                    AppendContextInfo(__CONTEXT__,errlist);
                    return false;
                }
            }
        }
    }
    // PTS 1116541

    while ( m_ContainerIterator.IsValid() )
    {
        fileinfo = reinterpret_cast<UndoFileInfo*>(*m_ContainerIterator);
        if ( ! fileinfo->transno.gg90IsNil()  )
        {
            tgg91_TransNo dummyTransNo;
            dummyTransNo.gg90SetNil();
            Log_UndoFile file ( m_PageAccessManager,
                                m_Allocator,
                                dummyTransNo,
                                fileinfo->root,
                                fileinfo->last );
            if ( file.IsCreated() )
            {
                result &= file.Verify(isCold,errlist);
                (void)file.Invalidate(errlist); // prevent drop file
            }
            else
            {
                result = false;
                file.WriteToTrace();
                errlist = Log_Exception( __CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED,
                           "Verify: HistoryFile contains invalid UndoFile reference");
            }
        }
        if(!m_ContainerIterator.PlusPlus(errlist))
        {
            result = false;
            break;
        }
    }
    if(!result)
    	AppendContextInfo(__CONTEXT__,errlist);
    return result;
}

/* --------------------------------------------------------------------------- */
bool Log_HistoryFile::AppendUndoFile (Log_UndoFile         &file,
                                      SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_HistoryFile::AppendUndoFile", LogHistory_Trace, 5);

    SAPDBERR_ASSERT_STATE( IsCreated() );
    
    if(!file.SavePageCountAndRelevanceForGC(errlist))
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    
    if ( ! m_Container.ReserveSpace ( m_ContainerIterator, errlist ) )
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }

    if ( ! m_ContainerIterator.IsValid() )
    {
        m_ContainerIterator.WriteToTrace("not valid after reserve");
        errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"iterator is valid after reserve");
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    
    UndoFileInfo& entry     = *(reinterpret_cast<UndoFileInfo*>(*m_ContainerIterator));
    SAPDB_UInt    pagecount = file.GetPageCount();

    entry.root      = file.GetRootId().PageNo();
    entry.last      = file.LastPageNo();
    entry.transno   = file.GetTransNo();

    if ( pagecount > SAPDB_MAX_UINT2 )
        entry.pagecount = SAPDB_MAX_UINT2;
    else
        entry.pagecount = pagecount;
    
    if(!m_ContainerIterator.Invalidate (errlist))
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    
    // PTS 1127083 UH 2004-01-14 call Unlock() here
    // instead of in ReserveSpace() otherwise the page
	// on which the write operation take place is not save.
    if(!m_Container.UnLock(errlist))
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    return true;
}

/* --------------------------------------------------------------------------- */
bool Log_HistoryFile::RemoveUndoFileInfo (Iterator             &iter,
                                          tgg91_TransNo        &newOldestTransno,
                                          SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_HistoryFile::RemoveUndoFileInfo", LogHistory_Trace, 5);

    SAPDBERR_ASSERT_STATE( IsCreated() );

    UndoFileInfo &info = *iter;
    
    info.transno.gg90SetNil();
    info.root.Invalidate();
    info.last.Invalidate();
    
    if(!m_Container.RemoveSpace (iter.GetContainerIter(), errlist))
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }

    if ( iter.IsValid() )
        newOldestTransno = (*iter).transno;
    else
        newOldestTransno.gg90SetNil(); 
    
    return true;
}                                            

/* --------------------------------------------------------------------------- */
Log_HistoryFile::Iterator& Log_HistoryFile::GetFirstUndoFileInfo (SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_HistoryFile::GetFirstUndoFileInfo", LogHistory_Trace, 5);

    SAPDBERR_ASSERT_STATE( IsCreated() );

    PageIterator &pageiter = m_Iterator.GetContainerIter().GetPageIterator();
    
    if(!m_Container.Begin (m_ContainerIterator, Data_ForUpdate, errlist))
        return m_Iterator;
    
    while ( pageiter.IsValid() )
    {
        if(!m_Container.Set ( m_ContainerIterator, (*pageiter).PageNo(),
                          Data_ForUpdate, true, errlist))
            break;

        if ( m_ContainerIterator.IsValid() )
            break;

        if(!pageiter.PlusPlus(errlist))
            break;
    }
    
    return m_Iterator;
}
