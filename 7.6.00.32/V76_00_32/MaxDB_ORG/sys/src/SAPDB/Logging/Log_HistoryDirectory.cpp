/*!**************************************************************************

  module      : Log_HistoryDirectory.cpp
  special area: Logging
  responsible : UweH
  last changed: 2000-09-25  12:00
  copyright:    Copyright (c) 2000-2005 SAP AG
  description : Implementation for class Log_HistoryDirectory.



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
#include "DataAccess/Data_PageAccessManager.hpp"
#include "Logging/Log_Types.hpp"
#include "Logging/Log_Exceptions.hpp"
#include "Logging/Log_HistoryFile.hpp"
#include "Logging/Log_HistoryDirectory.hpp"

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
Log_HistoryDirectory::Log_HistoryDirectory (tgg00_TransContext &Trans,
                                            const Data_PageNo         Root)

: Data_BaseFile ( Data_HistoryDirectory,
                  Data_PageId(Root, Data_PageRecoveryMode(Data_Dynamic,Data_Recoverable)),
                  Data_PageAccessManager(Trans, Data_HistoryDirectory, 
                  Data_PageRecoveryMode(Data_Dynamic,Data_Recoverable), Root)),
  m_Container   (m_PageAccessManager, m_RootId, sizeof(HistoryFileInfo)),
  m_Iterator    (m_PageAccessManager),
  m_Allocator   (*(reinterpret_cast<SAPDBMem_IRawAllocator*>(Trans.trAllocator_gg00)))
{
    SAPDBTRACE_METHOD_DEBUG ("Log_HistoryDirectory::Log_HistoryDirectory", LogHistory_Trace, 5);
}

/* --------------------------------------------------------------------------- */
bool Log_HistoryDirectory::Create (SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_HistoryDirectory::Create", LogHistory_Trace, 5);

    SAPDBERR_ASSERT_STATE( ! IsCreated() );

    m_PageAccessManager.Invalidate();

    m_RootId.Invalidate();
    
    PageIterator RootPageIter (m_PageAccessManager);

    if(!m_Container.Create(RootPageIter, errlist)) // PTS 1121659 UH 2003-04-30
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    return true;
}

/* --------------------------------------------------------------------------- */
bool Log_HistoryDirectory::Drop (SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_HistoryDirectory::Drop", LogHistory_Trace, 5);

    SAPDBERR_ASSERT_STATE( IsCreated() );
    bool result = true;
    if(!m_Iterator.Invalidate(errlist))
    {
        AppendContextInfo(__CONTEXT__,errlist);
        RTE_Message(errlist);
        result = false;
    }
    if(!m_Container.Drop(errlist))
    {
        AppendContextInfo(__CONTEXT__,errlist);
        RTE_Message(errlist);
        result = false;
    }
    m_RootId.Invalidate();
    m_PageAccessManager.Invalidate();
    return result;
}

/* --------------------------------------------------------------------------- */
bool Log_HistoryDirectory::Verify (bool                  isCold,
                                   SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_HistoryDirectory::Verify", LogHistory_Trace, 5);

    if ( ! m_Container.Verify (isCold, errlist) )
    {
        errlist = errlist + Log_Exception(__CONTEXT__, LOG_ERROR_VERIFY,
                                          "HistoryDirectoryFile",
                                          SAPDB_ToString(GetRootId().PageNo()),
                                          SAPDB_ToString(m_PageAccessManager.GetLastError()));
        m_PageAccessManager.ResetLastError();
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }

    if ( LogHistory_Trace.TracesLevel(6) )
        RTE_Message( Log_Exception(__CONTEXT__, LOG_INFO_VERIFY,
                     "HistoryDirectoryFile", SAPDB_ToString(GetRootId().PageNo()) ) );

    if(!m_Container.Begin (m_Iterator, Data_ForRead, errlist))
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }

    bool             result = true;
    HistoryFileInfo *fileinfo;

    while ( m_Iterator.IsValid() )
    {
        fileinfo = reinterpret_cast<HistoryFileInfo*>(*m_Iterator);
        Data_PageNo root (fileinfo->root);
        if ( root.IsValid() )
        {
            Log_HistoryFile file (m_PageAccessManager, m_Allocator, root, Data_PageNo());
            result &= file.Verify(isCold, errlist);
        }
        if(!m_Iterator.PlusPlus(errlist))
        {
            result = false;
            break;
        }
    }
    return result;
}

/* --------------------------------------------------------------------------- */
bool Log_HistoryDirectory::AppendHistoryFile (const HistoryFileInfo &info,
                                              SAPDBErr_MessageList  &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_HistoryDirectory::AppendHistoryFile", LogHistory_Trace, 5);

    SAPDBERR_ASSERT_STATE( IsCreated() );

    if ( ! m_Container.ReserveSpace ( m_Iterator, errlist, false /* DO NOT USE FOR STRUCTURE CHANGE */) )
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }

    if ( ! m_Iterator.IsValid() )
    {
        errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"ReserveSpace");
        m_Iterator.AppendContextInfo(__CONTEXT__,errlist);
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    HistoryFileInfo &fileinfo = *(reinterpret_cast<HistoryFileInfo*>(*m_Iterator));
    
    fileinfo = info;    
    
    if(!m_Iterator.Invalidate (errlist))
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    if(!m_Container.UnLock(errlist)) // PTS 1127083 UH 2004-01-14 added
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    return true;
}

/* --------------------------------------------------------------------------- */
Log_HistoryDirectory::GetResult
Log_HistoryDirectory::GetFirstHistoryFileInfo (HistoryFileInfo      &fileinfo,
                                               SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_HistoryDirectory::GetFirstHistoryFileInfo", LogHistory_Trace, 5);

    SAPDBERR_ASSERT_STATE( IsCreated() );

    if(!m_Container.Begin (m_Iterator, Data_ForRead,errlist))
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return getResultError;
    }

    if ( ! m_Iterator.IsValid() )
        return getResultLastRead;

    HistoryFileInfo &dirinfo = *(reinterpret_cast<HistoryFileInfo*>(*m_Iterator));
    
    fileinfo = dirinfo;
    
    if(!m_Iterator.PlusPlus(errlist))
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return getResultError;
    }

	if(! m_Iterator.IsValid())
        return getResultLastRead;
    return getResultMoreToRead;
}                                            

/* --------------------------------------------------------------------------- */
Log_HistoryDirectory::GetResult
Log_HistoryDirectory::GetNextHistoryFileInfo (HistoryFileInfo      &fileinfo,
                                              SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_HistoryDirectory::GetNextHistoryFileInfo", LogHistory_Trace, 5);

    SAPDBERR_ASSERT_STATE( IsCreated() );

    if ( ! m_Iterator.IsValid() )
    {
        errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                   "Log_HistoryDirectory::GetNextHistoryFileInfo: may only be called if previous request returned 'moreToRead'");
        AppendContextInfo(__CONTEXT__,errlist);
        return getResultError;
    }

    HistoryFileInfo &dirinfo = *(reinterpret_cast<HistoryFileInfo*>(*m_Iterator));
    
    fileinfo = dirinfo;
    
    if(!m_Iterator.PlusPlus(errlist))
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return getResultError;
    }

	if(! m_Iterator.IsValid())
        return getResultLastRead;
    return getResultMoreToRead;
}                                            

/* --------------------------------------------------------------------------- */
bool Log_HistoryDirectory::MigratePageType (tgg00_TransContext   &trans,
                                            SAPDBErr_MessageList &errlist)
{
    // PTS 1115490 UH 2002-04-29 new
    
    SAPDBTRACE_METHOD_DEBUG ("Log_HistoryDirectory::MigratePageType", LogHistory_Trace, 5);

    if ( ! IsCreated() )
        return false;

    (void)m_Container.Begin (m_Iterator, Data_ForRead, errlist);
    
    if ( ! m_Iterator.GetPageIterator().IsValid()
         &&
         trans.trError_gg00 == e_inconsistent_nodetype )
    {
        trans.trError_gg00 = e_ok;
        Data_PageAccessManager pam (trans, Data_HistoryFile, Data_PageRecoveryMode(), this->GetRootId().PageNo());
        PageIterator           pageiter (pam);

        if ( ! pageiter.Set ( this->GetRootId().PageNo(),
                              this->GetRootId().PageRecoveryMode(),
                              Data_ForUpdate,
                              errlist ) )
        {
            AppendContextInfo(__CONTEXT__,errlist,"cannot access root page");
            return false;
        }

        while ( pageiter.IsValid() )
        {
            (*pageiter).ResetPageType2(pt2HistoryDirectory_egg00);
            if(!pageiter.PlusPlus(errlist))
            {
                AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }
        }
    }
    
    if(!m_Iterator.Invalidate(errlist))
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
	return true;
}                                            

