/*!**************************************************************************

  module      : Log_History.cpp
  special area: Logging
  responsible : UweH
  created     : 2000-11-07
  Copyright (c) 2000-2005 SAP AG
  description : Implementation for class Log_History.



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
#include "heo55k.h"  // vbegexcl/vendexcl/visexcl
#include "heo56.h"   // vsleep
#include "heo58.h"   // vgetrteinfo
#include "hgg01.h"   // g01maxuser
#include "ggg92.h"   // tgg92_KernelOid for hkb50.h
#include "hkb51.h"   // kb51IsConsistUsedTransId
#include "hkb57_1.h" // k57restartrec
#include "hbd02.h"   // b02DelGarbage
#include "hbd20_11.h"// bd20IsPreventSvpSyncOperationPending
#include "hbd91.h"   // bd91ReleaseUnusedObject
#include "hkb900.h"  // kb900DropAuxFiles
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/SAPDB_RangeCode.hpp"
#include "RunTime/RTE_Message.hpp"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "DataAccess/Data_SplitSpaceReader.hpp"
#include "Logging/Log_Types.hpp"
#include "Logging/Log_Exceptions.hpp"
#include "Logging/Log_ActionObject.hpp"
#include "Logging/Log_ActionCreateDropFile.hpp"
#include "Logging/Log_ActionFile.hpp"
#include "Logging/Log_ActionObjectFile.hpp"
#include "Logging/Log_ActionInsDelRec.hpp"
#include "Logging/Log_ActionUpdateRec.hpp"
#include "Logging/Log_ActionUnqualifiedDelete.hpp"
#include "Logging/Log_UndoFile.hpp"
#include "Logging/Log_BeforeImage.hpp"
#include "Logging/Log_History.hpp"
#include "GarbageCollector/GC_IGCController.hpp" 
#include "KernelCommon/Kernel_IAdminConfig.hpp"
#include "KernelCommon/Kernel_Migration.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "ConsistentRead/CnsRead_Manager.hpp"
#include "Transaction/Trans_Context.hpp"
#include "SAPDBCommon/SAPDB_AutoPtr.hpp"

/* -------------------------------------------------------------------------------- */
class DummyGC : public GC_IGarbageCollector // PTS 1114877 UH 2002-03-18 new
{
public:
    virtual void IncHistCreateObjFileCount() {}
    virtual void IncHistDeleteObjCount() {}
    virtual void IncHistDropObjFileCount() {}
    virtual void IncHistEntryReleaseCount() {}
    virtual void IncHistLockObjCount() {}
    virtual void IncHistInsertObjCount() {}
    virtual void IncHistNewObjCount() {}
    virtual void IncHistUpdateObjCount() {}
    virtual void IncObjReleaseCount() {}
    virtual void IncReleaseEmptyPageCount() {} /* PTS 1115760 FF */
    virtual bool IsInterrrupted () const
    {
        return false;
    }
};
/* -------------------------------------------------------------------------------- */
void UpdateStatistic(GC_IGarbageCollector &gc,
                     Log_ActionType        type)
{
    switch ( type )  // PTS 1114106 FF 13-Feb-2002
    {
    case Log_NewObject:
      gc.IncHistNewObjCount();
      break;
    case Log_InsertObject:
      gc.IncHistInsertObjCount();
      break;
    case Log_UpdateObject:
      gc.IncHistUpdateObjCount();
      break;
    case Log_DeleteObject:
      gc.IncHistDeleteObjCount();
      break;
    case Log_LockObject:
      gc.IncHistLockObjCount();
      break;
    case Log_CreateObjectFile:
      gc.IncHistCreateObjFileCount();
      break;
    case Log_DropObjectFile:
      gc.IncHistDropObjFileCount();
      break;
    default:
      break;
    }
}
/* -------------------------------------------------------------------------------- */
Log_History* Log_History::m_Instance = NULL;
/* -------------------------------------------------------------------------------- */
void Log_History::HistoryDirectory::ResetInProcess (tsp00_TaskId taskid,
                                                    SAPDB_UInt   fileno)
{
    Lock(fileno);
    m_Directory[fileno].inProcess = false;
    Unlock(fileno);
}
/* -------------------------------------------------------------------------------- */
void Log_History::HistoryDirectory::Invalidate (tsp00_TaskId taskid,
                                                SAPDB_UInt   fileno)
{
    Lock(fileno);
    m_Directory[fileno].persistent.Invalidate();
    m_Directory[fileno].pagecount = 0;
    m_Directory[fileno].inProcess = false;
    Unlock(fileno);
}

/* -------------------------------------------------------------------------------- */
void Log_History::HistoryDirectory::RemoveUnusedEntries ()
{
    if ( ! m_Directory.Resize(GetMaxUsedFileCount()) )
        RTE_Crash( Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                   "m_Directory.Resize() failed") );
}

/* -------------------------------------------------------------------------------- */
bool Log_History::HistoryDirectory::GetNextUnusedFileNo ( tsp00_TaskId  taskid,
                                                          SAPDB_UInt   &fileno )
{
    if ( fileno < GetMaxUsedFileCount()
         ||
         fileno >= GetFileCount() )
        fileno = GetMaxUsedFileCount();
    
    while ( fileno < GetFileCount() )
    {
        Lock(fileno);
        if ( m_Directory[fileno].persistent.IsValid()
             &&
             ! m_Directory[fileno].inProcess )
        {
            m_Directory[fileno].inProcess = true;
            Unlock(fileno);
            return true;
        }
        Unlock(fileno);
        ++fileno;
    }
    return false;
}

/* -------------------------------------------------------------------------------- */
bool Log_History::HistoryDirectory::SetHistoryFileInfo
    ( tsp00_TaskId           taskid,
      SAPDB_UInt             fileno,
      const HistoryFileInfo& info,
      SAPDB_UInt             pagecount,
      Msg_List  &errlist )
{
    if ( fileno >= m_Directory.GetCapacity() )
    {
        if ( ! m_Directory.Resize(m_Directory.GetCapacity()+10) )
        {
            // PTS 1117126 UH 2002-08-07 if-clause added
            errlist = Log_Exception(__CONTEXT__, LOG_HISTDIR_RESIZE_FAILED,
                                    SAPDB_ToString(fileno),
                                    SAPDB_ToString(m_Directory.GetCapacity()));
            return false;
        }
    }
    m_Directory[fileno].persistent = info;
    m_Directory[fileno].pagecount = pagecount;
    return true;
}

/* -------------------------------------------------------------------------------- */
void Log_History::HistoryDirectory::GetHistoryFileInfo
    ( SAPDB_UInt       fileno,
      HistoryFileInfo &info,
      SAPDB_UInt      &pagecount ) const
{
    info      = m_Directory[fileno].persistent;
    pagecount = m_Directory[fileno].pagecount;
}

/* -------------------------------------------------------------------------------- */
void Log_History::HistoryDirectory::GetHistoryFileInfo
    ( tsp00_TaskId     taskid,
      SAPDB_UInt       fileno,
      HistoryFileInfo &info,
      SAPDB_UInt      &pagecount )
{
    Lock(fileno);
    info = m_Directory[fileno].persistent;
    pagecount = m_Directory[fileno].pagecount;
    Unlock(fileno);
}

/* -------------------------------------------------------------------------------- */
void Log_History::HistoryDirectory::AppendContextInfo( SAPDB_Char  const * const  filename,
                                                       SAPDB_UInt4 const          linenumber,
                                                       Msg_List      &errlist,
                                                       SAPDB_Char  const * const  title) const
{
    errlist = errlist + Log_Exception(filename,linenumber,LOG_HIST_DIR_INFO,
                        (title!=0?title:"HistDir"),
                        SAPDB_ToString(GetFileCount()),
                        SAPDB_ToString(GetMaxUsedFileCount()));
}

/* -------------------------------------------------------------------------------- */
void Log_History::HistoryDirectory::WriteToTrace (tsp00_TaskId      taskid,
                                                  const SAPDB_Char *title) const
{
    Kernel_VTrace trace;
    
    if ( title != NULL )
        trace << title << NewLine;
    trace << "HistoryDirectory: " << GetFileCount()
          << " entries ("         << GetMaxUsedFileCount()
          << ")"                  << NewLine;

    for ( SAPDB_UInt fileno = 0; fileno < GetFileCount(); ++fileno )
    {
        const HistoryFileInfo &info = m_Directory[fileno].persistent;

        trace << "HistFile "    << fileno
              << ": T"          << info.oldestTransNo.gg90GetInt4()
              << " - T"         << info.youngestTransNo.gg90GetInt4()
              << " page# "      << m_Directory[fileno].pagecount
              << " root: "      << info.root
              << " last: "      << info.last
              << " inProcess: " << m_Directory[fileno].inProcess
              << NewLine;
    }
}

/* -------------------------------------------------------------------------------- */
void Log_History::HistoryDirectory::UpdateInfoAfterRegisterNewUndoFile
    ( tsp00_TaskId        taskid,
     SAPDB_UInt           fileno,
     const tgg91_TransNo &newtransno,
     SAPDB_UInt           pagecount,
     Data_PageNo          last )
{
    Lock(fileno);
    SAPDBERR_ASSERT_ARGUMENT (! newtransno.gg90IsNil());
    // PTS 1114970 UH 2002-03-21 begin
    if ( m_Directory[fileno].persistent.youngestTransNo.gg90IsNil()
         ||
         newtransno > m_Directory[fileno].persistent.youngestTransNo )
        m_Directory[fileno].persistent.youngestTransNo = newtransno;
    if ( m_Directory[fileno].persistent.oldestTransNo.gg90IsNil() )
    {
        m_Directory[fileno].persistent.oldestTransNo = newtransno;
        m_Directory[fileno].pagecount                = pagecount;
    }
    else
    {
        if ( m_Directory[fileno].persistent.oldestTransNo > newtransno )
            m_Directory[fileno].persistent.oldestTransNo = newtransno;
        m_Directory[fileno].pagecount += pagecount;
    }
    m_Directory[fileno].persistent.last = last;
    // PTS 1114970 UH 2002-03-21 end
    Unlock(fileno);
}

/* -------------------------------------------------------------------------------- */
void Log_History::HistoryDirectory::UpdateOldestTransNo
    ( tsp00_TaskId         taskid,
      SAPDB_UInt           fileno,
      const tgg91_TransNo &newOldestTransno,
      Data_PageNo          last,
      SAPDB_UInt           removedPages )
{
    if ( newOldestTransno.gg90IsNil() )
    {
        Lock(fileno);
        m_Directory[fileno].persistent.youngestTransNo.gg90SetNil();
        m_Directory[fileno].persistent.oldestTransNo.gg90SetNil();
        m_Directory[fileno].pagecount = 0;
        Unlock(fileno);
        return;
    }

    Lock(fileno);

    if ( m_Directory[fileno].persistent.oldestTransNo.gg90IsNil()
         ||
         m_Directory[fileno].persistent.oldestTransNo > newOldestTransno )
        m_Directory[fileno].persistent.oldestTransNo = newOldestTransno;
    m_Directory[fileno].persistent.last = last;
    // PTS 1114970 UH 2002-03-21 begin
    if ( m_Directory[fileno].pagecount < removedPages )
        m_Directory[fileno].pagecount = 0;
    else
        m_Directory[fileno].pagecount            -= removedPages;
    // PTS 1114970 UH 2002-03-21 end

    Unlock(fileno);
}
/* -------------------------------------------------------------------------------- */
void Log_History::DropInstance ()
{
    // PTS 1122361 UH 2003-05-27 new
    SAPDBTRACE_ROUTINE_DEBUG( "Log_History::DropInstance", LogHistory_Trace, 5 );
    if ( m_Instance != 0 )
    {
        GC_IGCController::Instance().ClearAll();  
        m_Instance->m_Directory.Delete();
        m_Instance = 0;
    }
}
/* -------------------------------------------------------------------------------- */
bool Log_History::CreateInstance (tgg00_TransContext   &trans,
                                  Msg_List &errlist)
{
    SAPDBTRACE_ROUTINE_DEBUG( "Log_History::CreateInstance", LogHistory_Trace, 5 );

    if ( m_Instance != 0 )
    {
        errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"History not created");
        m_Instance->AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    
    SAPDBMem_IRawAllocator& allocator = RTEMem_Allocator::Instance();

    tsp0058_RTEInfo RTEInfo;
    vgetrteinfo (RTEInfo);

    m_Instance = new(allocator) Log_History ( allocator,
                                              RTEInfo.NumOfUserUKTs,
                                              SAPDB_UInt(g01maxuser()) );
    
    if ( NULL == m_Instance )
    {
        errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"allocate of singleton History");
        return false;
    }

    if ( ! m_Instance->m_Directory.Initialize() ) 
    {
        errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"init of singleton History");
        destroy (m_Instance, allocator);
        m_Instance = NULL;
        return false;
    }

    m_Instance->m_DirectoryRoot = k57restartrec->rstLastSavept_kb00().svpHistoryRoot_kb00;

    if ( m_Instance->m_DirectoryRoot.IsInvalid() )
    {
        if(!m_Instance->CreateHistoryDirectory(trans, errlist))
            return false;
        k57restartrec->rstLastSavept_kb00().svpHistoryRoot_kb00 = m_Instance->m_DirectoryRoot;
    }
    else
        if(!m_Instance->RestoreHistoryDirectory(trans,errlist))
            return false;
    
    return true;
}

/* -------------------------------------------------------------------------------- */
bool Log_History::CreateHistoryDirectory (tgg00_TransContext   &trans,
                                          Msg_List &errlist)
{
    SAPDBTRACE_METHOD_DEBUG( "Log_History::CreateHistoryDirectory", LogHistory_Trace, 5 );

    Log_HistoryDirectory dirfile (trans, Data_PageNo());

    if ( ! dirfile.Create(errlist) )
    {
        AppendContextInfo(__CONTEXT__,errlist,"Log_History::CreateHistoryDirectory() dirfile.Create() failed");
        return false;
    }
    m_DirectoryRoot = dirfile.GetRootId().PageNo();

    if(!CreateHistoryFiles (trans, dirfile, 0, m_Directory.GetFileCount()-1, errlist))
        return false;

    // PTS 1115706 UH 2002-05-13 begin
    RTE_Message( Log_Exception(__CONTEXT__, LOG_EXISTING_HISTORY_FILES,
                               SAPDB_ToString(GetNumberOfMaxUsedHistoryFiles()),
                               SAPDB_ToString(GetNumberOfHistoryFiles())) );
        
    // Only after registration the garbage collector tasks can work on the history files.
    for ( SAPDB_UInt fileno = 0; fileno < GetNumberOfMaxUsedHistoryFiles(); ++fileno )
    {
        GC_IGCController::Instance().RegisterHistoryFile(trans, fileno);  
        if ( trans.trError_gg00 != e_ok )
        {
            AppendContextInfo(__CONTEXT__,errlist,"GC_IGCController::Instance().RegisterHistoryFile failed");
            return false;
        }
    }

    RTE_Message( Log_Exception(__CONTEXT__, LOG_HISTORY_FILE_REGISTERED) );
    // PTS 1115706 UH 2002-05-13 end
    return true;
}
    
/* -------------------------------------------------------------------------------- */

bool Log_History::CreateHistoryFiles (tgg00_TransContext   &trans,
									  Log_HistoryDirectory &dirfile,
									  SAPDB_UInt            firstFileNo,
									  SAPDB_UInt            lastFileNo,
                                      Msg_List &errlist)
{
    SAPDBTRACE_METHOD_DEBUG( "Log_History::CreateHistoryFiles", LogHistory_Trace, 5 );

    HistoryFileInfo newfileinfo;
    
    for ( SAPDB_UInt historyFileNo = firstFileNo; historyFileNo <= lastFileNo; historyFileNo++)
    {
        Log_HistoryFile file (trans, Data_PageNo(), Data_PageNo() /*, historyFileNo*/ );

        if ( ! file.Create(errlist) )
        {
            AppendContextInfo(__CONTEXT__,errlist,"Log_History::CreateHistoryFiles() file.Create() failed");
            return false;
        }
        
        newfileinfo.root = file.GetRootId().PageNo();
        newfileinfo.last = newfileinfo.root;
        
        if(!dirfile.AppendHistoryFile(newfileinfo,errlist))
        {
            AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }

        // no bd91RegisterHistoryFile() here

        if(!m_Directory.SetHistoryFileInfo(trans.trTaskId_gg00, historyFileNo, newfileinfo, 0, errlist))
        {
            AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
    }
    return true;
}

/* -------------------------------------------------------------------------------- */
bool Log_History::RestoreHistoryDirectory (tgg00_TransContext   &trans,
                                           Msg_List &errlist)
{
    SAPDBTRACE_METHOD_DEBUG( "Log_History::RestoreHistoryDirectory", LogHistory_Trace, 5 );

    Log_HistoryDirectory dirfile (trans, m_DirectoryRoot); 
    
    SAPDBERR_ASSERT_STATE ( dirfile.IsCreated() );

    if(!dirfile.MigratePageType(trans,errlist)) // PTS 1115490 UH 2002-04-29 added
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    
    SAPDB_UInt      historyFileNo = 0;
    HistoryFileInfo fileinfo;
    Log_HistoryDirectory::GetResult result;
    Data_PageNo     unknownRoot;
    SAPDB_UInt      pagecount;

    do    
    {
        if ( historyFileNo == 0 )
            result = dirfile.GetFirstHistoryFileInfo (fileinfo,errlist);
        else
            result = dirfile.GetNextHistoryFileInfo (fileinfo,errlist);

        if ( Log_HistoryDirectory::getResultError == result )
        {
            AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }

        if ( LogHistory_Trace.TracesLevel(6) )
            Kernel_VTrace() << "history file #" << historyFileNo << 
                               ": root: "       << fileinfo.root    << 
                               ", last: "       << fileinfo.last    << 
                               ", oldest: "     << fileinfo.oldestTransNo.gg90GetInt4() << 
                               ", youngest: "   << fileinfo.youngestTransNo.gg90GetInt4();

        pagecount = 0;
        
        if ( fileinfo.IsValid() ) // PTS 1122287 UH 2003-05-26
        {
            if(!UpdateHistoryFileInfo (trans, historyFileNo, fileinfo, pagecount, errlist))
                return false;
        }

        if(!m_Directory.SetHistoryFileInfo (trans.trTaskId_gg00, historyFileNo, fileinfo, pagecount, errlist))
        {
            AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }

        // PTS 1114877 UH 2002-03-18 bd91RegisterHistoryFile is now executed at the end.

        ++historyFileNo;
    }
	while ( Log_HistoryDirectory::getResultMoreToRead == result  );

    
    if ( historyFileNo < GetNumberOfMaxUsedHistoryFiles() )
    {
        if ( LogHistory_Trace.TracesLevel(6) )
            Kernel_VTrace() << "MAXUSERTASKS was increased: adding new history files";
        if(!CreateHistoryFiles ( trans, dirfile, historyFileNo,
                             GetNumberOfMaxUsedHistoryFiles()-1, errlist ) )
            return false;
    }

    // PTS 1114877 UH 2002-03-18 begin
    RTE_Message( Log_Exception(__CONTEXT__, LOG_EXISTING_HISTORY_FILES,
                               SAPDB_ToString(GetNumberOfMaxUsedHistoryFiles()),
                               SAPDB_ToString(GetNumberOfHistoryFiles())) );
        
    // Only after registration the garbage collector tasks can work on the history files.
    for ( SAPDB_UInt fileno = 0; fileno < GetNumberOfMaxUsedHistoryFiles(); ++fileno )
    {
        GC_IGCController::Instance().RegisterHistoryFile(trans, fileno);  
        if ( trans.trError_gg00 != e_ok )
        {
            errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"GC_IGCController::Instance().RegisterHistoryFile");
            Kernel_AppendBasisError(trans.trError_gg00,errlist);
            AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
    }

    RTE_Message( Log_Exception(__CONTEXT__, LOG_HISTORY_FILE_REGISTERED) );
    // PTS 1114877 UH 2002-03-18 end
    return true;
}

/* -------------------------------------------------------------------------------- */
bool Log_History::UpdateHistoryFileInfo ( tgg00_TransContext   &trans,
                                          SAPDB_UInt            historyFileNo,
                                          HistoryFileInfo      &fileinfo,
                                          SAPDB_UInt           &pagecount,
                                          Msg_List &errlist )
{
    SAPDBTRACE_METHOD_DEBUG( "Log_History::UpdateHistoryFileInfo", LogHistory_Trace, 5 );
    
    if ( ! fileinfo.IsValid() ) // PTS 1122287 UH 2003-05-26 for security
        return false;

    tgg91_TransNo              oldesttransno;
    tgg91_TransNo              youngesttransno;
    SAPDB_UInt                 entryCount = 0;
    Log_HistoryFile            histfile (trans, fileinfo.root, Data_PageNo());

	errlist.ClearMessageList();
    
    Log_HistoryFile::Iterator& iter = histfile.GetFirstUndoFileInfo(errlist);

    if ( ! errlist.IsEmpty() )
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    
    oldesttransno.gg90SetNil();
    youngesttransno.gg90SetNil();
    
    while ( iter.IsValid() )
    {
        if ( (*iter).transno.gg90IsNil() ) // find the next valid entry
        {
            if(!iter.PlusPlus(errlist))
            {
                AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }
            continue;
        }
        
        ++entryCount;
        
        if ( oldesttransno.gg90IsNil() || oldesttransno > (*iter).transno )
            oldesttransno = (*iter).transno;
        
        if ( youngesttransno.gg90IsNil() || (*iter).transno > youngesttransno  )
            youngesttransno =  (*iter).transno;

        if ( (*iter).pagecount == SAPDB_MAX_UINT2 )
        {
            // look into undo file root.
            Log_UndoFile undofile (trans, (*iter).root, (*iter).last);

            if( ! undofile.RestorePageCountAndRelevanceForGC(errlist) )
            {
                // Ignore this error here until it is clear why historyentries
                // exist and their referenced undofile is missing
                errlist.ClearMessageList();
                trans.trError_gg00 = e_ok;
            }
            else
            {
                pagecount += undofile.GetPageCount();

                if( ! undofile.Invalidate(errlist) ) // prevent Drop() in destructor
                {
                    AppendContextInfo(__CONTEXT__,errlist);
                    return false;
                }
            }
        }
        else
            pagecount += (*iter).pagecount;
            
        if(!iter.PlusPlus(errlist))
        {
            AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
    }

    if ( oldesttransno.gg90IsNil() || youngesttransno.gg90IsNil() )
        return true;
        
    bool fileIsChanged = false;
    
    if ( fileinfo.oldestTransNo != oldesttransno )
    {
        fileinfo.oldestTransNo = oldesttransno;
        fileIsChanged = true;
    }

    if ( fileinfo.youngestTransNo != youngesttransno )
    {
        fileinfo.youngestTransNo = youngesttransno;
        fileIsChanged = true;
    }
    
    if ( fileinfo.last != histfile.LastPageNo() )
    {
        fileinfo.last = histfile.LastPageNo();
        fileIsChanged = true;
    }
    
    if ( fileIsChanged && LogHistory_Trace.TracesLevel(6) )
    {
        Kernel_VTrace() << "updated history file #" << historyFileNo << 
                           ": root: "       << fileinfo.root    << 
                           ", last: "       << fileinfo.last    << 
                           ", oldest: "     << fileinfo.oldestTransNo.gg90GetInt4() << 
                           ", youngest: "   << fileinfo.youngestTransNo.gg90GetInt4()  <<
                           ", pagecount: "  << pagecount;
    }
    return true;
}

/* -------------------------------------------------------------------------------- */
bool Log_History::Flush (tgg00_TransContext   &trans,
                         Data_PageNo          &newHistRoot,
                         Msg_List &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_History::Flush", LogHistory_Trace, 5);

    Log_HistoryDirectory  olddirfile (trans, m_DirectoryRoot);
    Log_HistoryDirectory  newdirfile (trans, Data_PageNo());
    
    if ( LogHistory_Trace.TracesLevel(6) )
        Kernel_VTrace() << "FlushHistory: oldroot: " << m_DirectoryRoot << NewLine;
    
    if ( ! newdirfile.Create(errlist) )
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    
    if ( LogHistory_Trace.TracesLevel(6) )
        Kernel_VTrace() << "FlushHistory: newroot: " << newdirfile.GetRootId().PageNo();
    
    SAPDB_UInt      historyFileCount = GetNumberOfHistoryFiles();
    SAPDB_UInt      fileno;
    SAPDB_UInt      pagecount;
    HistoryFileInfo fileinfo;

    
    if ( GetNumberOfMaxUsedHistoryFiles() < historyFileCount )
    {
        bool removeUnusedHistoryFiles = true;
        for ( fileno = GetNumberOfMaxUsedHistoryFiles();
              fileno < historyFileCount;
              ++fileno )
        {
            if ( ! GetOldestTransNo(fileno).gg90IsNil() )
            {
                if ( LogHistory_Trace.TracesLevel(6) )
                    Kernel_VTrace() << "UNUSED HISTORY: STILL EXISTS";
                removeUnusedHistoryFiles = false;
                break;
            }
        }
        if ( removeUnusedHistoryFiles )
        {
            if ( LogHistory_Trace.TracesLevel(6) )
                Kernel_VTrace() << "UNUSED HISTORY: COMPLETELY REMOVED";
            m_Directory.RemoveUnusedEntries();
            historyFileCount = GetNumberOfMaxUsedHistoryFiles();
            RTE_Message( Log_Exception(__CONTEXT__, LOG_EXISTING_HISTORY_FILES,
                                       SAPDB_ToString(GetNumberOfMaxUsedHistoryFiles()),
                                       SAPDB_ToString(GetNumberOfHistoryFiles())) );
        }
    }

    for ( fileno = 0; fileno < historyFileCount; fileno++)
    {
        m_Directory.GetHistoryFileInfo(trans.trTaskId_gg00, fileno, fileinfo, pagecount);
        if (! newdirfile.AppendHistoryFile(fileinfo, errlist))
        {
            AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
    }
    if ( olddirfile.IsCreated()
         &&
         ! olddirfile.Drop(errlist) )
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    

    m_DirectoryRoot = newdirfile.GetRootId().PageNo();
    newHistRoot     = m_DirectoryRoot;
    return true;
}

/* -------------------------------------------------------------------------------- */
bool Log_History::Verify (tgg00_TransContext   &trans,
                          bool                  isCold,
                          Msg_List &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_History::Verify", LogHistory_Trace, 5);
    Log_HistoryDirectory  dirfile (trans, k57restartrec->rstLastSavept_kb00().svpHistoryRoot_kb00);
    if ( isCold && dirfile.IsCreated() )
        return dirfile.Verify(isCold, errlist);
    else
        return true;
}

/* -------------------------------------------------------------------------------- */
bool Log_History::RegisterUndoFile (tgg00_TransContext   &trans,
                                    Log_UndoFile         &file,
                                    Msg_List &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_History::RegisterUndoFile", LogHistory_Trace, 5);
    
    SAPDB_Int       fileno = m_Directory.GetFileNo(trans.trTaskId_gg00);
    HistoryFileInfo info;
    SAPDB_UInt      dummyPageCount;
    
    m_Directory.GetHistoryFileInfo(fileno, info, dummyPageCount);

	Log_HistoryFile histfile ( trans, info.root, Data_PageNo() );

    if(!histfile.AppendUndoFile (file,errlist))
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }

    m_Directory.UpdateInfoAfterRegisterNewUndoFile
                ( trans.trTaskId_gg00,
                  fileno,
                  file.GetTransNo(),
                  file.GetPageCount(),
                  histfile.LastPageNo() );
	return true;
}
/* -------------------------------------------------------------------------------- */
bool Log_History::RemoveHistory (tgg00_TransContext        &trans,
                                 SAPDB_UInt                 fileno,
                                 RemoveOptions              option,
                                 GC_IGarbageCollector      &IGarbColl,                    // PTS xxxxxxx FF 2002-02-12 
                                 bool                      &anythingRemoved,
                                 Msg_List      &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_History::RemoveHistory", LogHistory_Trace, 5);
    
    if ( fileno >= GetNumberOfHistoryFiles() )
    {
        errlist = Log_Exception(__CONTEXT__,LOG_HISTORY_INVALID_FILENO,
                                SAPDB_ToString(fileno),
                                SAPDB_ToString(GetNumberOfHistoryFiles()) );
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    
    anythingRemoved = false;

    HistoryFileInfo fileinfo;
    tgg91_TransNo   minUsedTransNo;
    pasbool         isTransUsed = 0;
    SAPDB_UInt      pagecount;
    
    // at first read it dirty to check if it's empty
    m_Directory.GetHistoryFileInfo ( fileno, fileinfo, pagecount);
    
    if ( ! fileinfo.IsValid() ) // PTS 1122287 UH 2003-05-26 it may be a continued "remove unused"
    {
        Kernel_VTrace() << "fileinfo " << fileno << " is invalid and ignored" << fileno;
        return true;
    }
        
    if ( fileinfo.oldestTransNo.gg90IsNil()
         &&
         fileinfo.youngestTransNo.gg90IsNil() )
    {
        // file is empty
        if ( option == RemoveUnused )
        {
            Log_HistoryFile file (trans, fileinfo.root, Data_PageNo());
            if ( file.IsCreated() )
            {
                if ( LogHistory_Trace.TracesLevel(6) )
                    Kernel_VTrace() << "UNUSED HISTORY: DROP EMPTY FILE " << fileno;
                if(!file.Drop(errlist))
                {
                    AppendContextInfo(__CONTEXT__,errlist, "Drop of unused history file failed");
                    return false;
                }
            }
        }
        return true;
    }
    
    if ( option != RemoveUnused )
        CheckAndRemoveUnusedHistory (trans, IGarbColl, errlist);

    // second read it synchronized
    m_Directory.GetHistoryFileInfo ( trans.trTaskId_gg00, fileno, fileinfo, pagecount);
    
    kb51IsConsistUsedTransId ( trans.trTaskId_gg00,
                               fileinfo.oldestTransNo,
                               minUsedTransNo,
                               isTransUsed );
                               
    if ( LogHistory_Trace.TracesLevel(6) )
    {
        Kernel_VTrace trace;
        trace << "RemoveHistory() fileno: " << fileno
              << ", oldest trans: "         << fileinfo.oldestTransNo.gg90GetInt4()
              << ", youngest trans: "       << fileinfo.youngestTransNo.gg90GetInt4()
              << NewLine;
        trace << "RemoveHistory() root: "   << fileinfo.root
              << ", last: "                 << fileinfo.last
              << ", pagecount: "            << pagecount
              << NewLine;
        trace << "RemoveHistory() option: " << ( option==Normal      ? "Normal"
                                               :(option==Truncate    ? "Truncate"
                                               :(option==RemoveUnused? "RemoveUnused"
                                               :                       "Sensitive" ) ) )
              << NewLine;
        trace << "RemoveHistory() min used trans: " << minUsedTransNo.gg90GetInt4()
              << ", is used: "      << (isTransUsed==0?"no":"yes")
              << NewLine;
    }

    if ( option == Truncate
         &&
         ! IGarbColl.IsInterrrupted() ) // PTS 1113185 UH 2001-12-18
    {
        if(!TruncateHistory (trans, fileno, fileinfo.root, fileinfo.last,
                             IGarbColl,                 // PTS 1113185 UH 2001-12-18
                             anythingRemoved,
                             errlist))
        {
            return false;
        }
        
        if ( ! anythingRemoved
             &&
             ! IGarbColl.IsInterrrupted()
             &&
             LogHistory_Check.ChecksLevel(5) )
        {
            // The caller at first scans for the oldest trans.
            // Only if the caller gets an oldest trans it calls RemoveHistory for truncation.
            // So if nothing was done, then the HistoryDirectory is inconsistent with the
            // Historyfile.
            errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_ARGUMENT_FAILED,
                                    "TruncateHistory() anything removed");
            AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
        return true;
    }

    if ( isTransUsed == 1 )
    {
        // even the oldest trans in this history file is used
        if ( LogHistory_Trace.TracesLevel(6) )
            Kernel_VTrace() << "RemoveHistory() oldest trans is used => NO ACTION" << NewLine;
        return true;
    }
    
    errlist.ClearMessageList();
    
    Log_HistoryFile            file (trans, fileinfo.root, Data_PageNo() );
    Log_HistoryFile::Iterator &undoFileInfoIter = file.GetFirstUndoFileInfo(errlist);

    if ( ! errlist.IsEmpty() )
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    
    bool          removeHistory;
    bool          result = true;
    SAPDB_UInt    loops = 0;
    Data_PageNo   undoRoot;
    Data_PageNo   undoLast;
    tgg91_TransNo transno;

    while ( undoFileInfoIter.IsValid()
            &&
            ! IGarbColl.IsInterrrupted() ) // PTS 1113185 UH 2001-12-18
    {   
        Log_HistoryFile::UndoFileInfo& ufi = (*undoFileInfoIter);
        transno   = ufi.transno;
        undoRoot  = ufi.root;
        undoLast  = ufi.last;

        if ( transno.gg90IsNil() ) // find the next valid entry
        {
            if(!undoFileInfoIter.PlusPlus(errlist))
            {
                AppendContextInfo(__CONTEXT__,errlist,"Accessing next undofile info failed");
                result = false;
                break;
            }
            continue;
        }
        
        if ( LogHistory_Trace.TracesLevel(6) )
            Kernel_VTrace() << "RemoveHistory() transno: " << transno.gg90GetInt4()
                            << ", undoRoot: " << undoRoot
                            << ", undoLast: " << undoLast
                            << ", pagecount: " << pagecount
                            << NewLine;

#if defined(NOT_USED_ANYMORE) // can't have been changed since "transno = ufi.transno;"
        if ( transno != (*undoFileInfoIter).transno )
        {
            Kernel_VTrace() << "iter.transno: " << (*undoFileInfoIter).transno.gg90GetInt4()
                            << NewLine;
            undoFileInfoIter.WriteToTrace("transno != iter.transno");
            errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                                    "transno == iter.transno, look into vtrace");
            (void)undoFileInfoIter.Invalidate(errlist);
            result = false;
            break;
        }       
#endif

        if ( ! minUsedTransNo.gg90IsNil() )
            // simple check if this history is older then the oldest needed
            removeHistory = minUsedTransNo > transno;
        else
        {
            // at the time this run started, no view was open, no history was needed
            if ( fileinfo.youngestTransNo >= transno  )
                removeHistory = true;
            else
            {
                // local youngest transno is reached
                // there may be more entries in the meantime
                // but they are proofed in a later run
                if(!undoFileInfoIter.Invalidate(errlist))
                {
                    AppendContextInfo(__CONTEXT__,errlist);
                    result = false;
                }
                break;
            }
        }
        
        if ( ! removeHistory && option == Sensitive )
        {
            // expensive check if filling state required "Sensitive"
            kb51IsConsistUsedTransId ( trans.trTaskId_gg00,
                                       transno,
                                       minUsedTransNo,
                                       isTransUsed );

            removeHistory = isTransUsed == 0;
        }
        
        if ( LogHistory_Trace.TracesLevel(6) )
            Kernel_VTrace() << "RemoveHistory() transno: " << transno.gg90GetInt4()
                            << (removeHistory?" is removed now":" is not removed now")
                            << NewLine;

        if ( removeHistory )
        {
            bool dropAuxFilesNeeded = false; // PTS 1134156 UH 2005-02-25
            
            result = RemoveUndoFile (trans,
                                     transno,
                                     undoRoot, undoLast,
                                     IGarbColl,                       // PTS 1113185 UH 2001-12-18
                                     fileno, file, undoFileInfoIter,  // PTS 1113934 UH 2002-02-01
                                     anythingRemoved,
                                     dropAuxFilesNeeded,          // PTS 1134156 UH 2005-02-25
                                     errlist);
            if ( ! result )
                break;

            if ( dropAuxFilesNeeded ) // PTS 1134156 UH 2005-02-25
                kb900DropAuxFiles(trans);
        }
        else
        {
            if(!undoFileInfoIter.PlusPlus(errlist))
            {
                AppendContextInfo(__CONTEXT__,errlist);
                result = false;
                break;
            }
        }

        ++loops;
    }//endwhile
    
    if(!undoFileInfoIter.Invalidate(errlist))
    {
        if(result) AppendContextInfo(__CONTEXT__,errlist);
        result = false;
    }

    if ( option == RemoveUnused
         &&
         result )
    {
        if ( m_Directory.GetOldestTransNo(fileno).gg90IsNil() )
        {
            if ( LogHistory_Trace.TracesLevel(6) )
                Kernel_VTrace() << "UNUSED HISTORY: DROP FILE " << fileno;
            if(!file.Drop(errlist))
            {
                AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }
        }
        return true;
    }

    return result;
}

/* -------------------------------------------------------------------------------- */
bool Log_History::TruncateHistory (tgg00_TransContext        &trans,
                                   SAPDB_UInt                 fileno,
                                   Data_PageNo                root,
                                   Data_PageNo                last,
                                   GC_IGarbageCollector      &IGarbColl,                    //PTS xxxxxxx FF 2002-02-12 
                                   bool                      &anythingRemoved,
                                   Msg_List      &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_History::TruncateHistory", LogHistory_Trace, 5);

    anythingRemoved = false;
    
    errlist.ClearMessageList();

    Log_HistoryFile            file (trans, root, Data_PageNo() );
    Log_HistoryFile::Iterator& undoFileInfoIter = file.GetFirstUndoFileInfo(errlist);

    if ( ! errlist.IsEmpty() )
	{
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
	}

    while ( undoFileInfoIter.IsValid()
            &&
            (*undoFileInfoIter).transno.gg90IsNil() )
	{
        // find the first valid entry
        if ( ! undoFileInfoIter.PlusPlus(errlist) )
        {
            AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
	}
        
    if ( ! undoFileInfoIter.IsValid() )
    {
        // nothing to truncate found
        if(!undoFileInfoIter.Invalidate(errlist))
        {
            AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
        return true;
    }

    if ( LogHistory_Trace.TracesLevel(6) )
        Kernel_VTrace() << "RemoveHistory() transno: " << (*undoFileInfoIter).transno.gg90GetInt4() << " is truncated now"
                        << NewLine;

    Data_PageNo undoRoot  = (*undoFileInfoIter).root;
    Data_PageNo undoLast  = (*undoFileInfoIter).last;
    SAPDB_UInt  pagecount = (*undoFileInfoIter).pagecount;
    
    bool dropAuxFilesNeeded = false; // PTS 1134156 UH 2005-02-25

    if (! RemoveUndoFile (trans, 
                          (*undoFileInfoIter).transno,
                          undoRoot, undoLast,
                          IGarbColl,                   // PTS 1113185 UH 2001-12-18
                          fileno,  
                          file,
                          undoFileInfoIter,
                          anythingRemoved,
                          dropAuxFilesNeeded,          // PTS 1134156 UH 2005-02-25
                          errlist) )
	{
        return false;
	}
    
    if ( dropAuxFilesNeeded ) // PTS 1134156 UH 2005-02-25
        kb900DropAuxFiles(trans);
    
    if ( anythingRemoved )    // PTS 1113934 UH 2002-02-01
    {
        tgg91_TransNo newOldestTransno;
        
        if ( ! undoFileInfoIter.IsValid() )
            newOldestTransno.gg90SetNil();
        else
        {
            if ( ! DetermineOldestTransNo(file, newOldestTransno, errlist) )
                return false;
        }
         
        // The later update is only correct,
        // if the garbage collectors are stopped during savepoint.
        m_Directory.UpdateOldestTransNo ( trans.trTaskId_gg00,
                                          fileno,
                                          newOldestTransno,
                                          file.LastPageNo(),
                                          pagecount );
        return true;
    }
	
    if(!undoFileInfoIter.Invalidate(errlist))
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    
    return true;
}

/* -------------------------------------------------------------------------------- */
bool Log_History::DetermineOldestTransNo (Log_HistoryFile      &file,
                                          tgg91_TransNo        &oldestTransno,
                                          Msg_List &errlist)
{
    oldestTransno.gg90SetNil();
    
    errlist.ClearMessageList();
    
    Log_HistoryFile::Iterator& undoFileInfoIter = file.GetFirstUndoFileInfo(errlist);

    if ( ! errlist.IsEmpty() )
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    
    // find the next valid entry
    while ( undoFileInfoIter.IsValid()
            &&
            (*undoFileInfoIter).transno.gg90IsNil() )
	{
        if(!undoFileInfoIter.PlusPlus(errlist))
        {
            AppendContextInfo(__CONTEXT__,errlist);
            (void)undoFileInfoIter.Invalidate(errlist);
            return false;
        }
	}

    if ( ! undoFileInfoIter.IsValid() )
        oldestTransno.gg90SetNil();
    else
    {
        oldestTransno = (*undoFileInfoIter).transno;
        if(!undoFileInfoIter.Invalidate(errlist))
        {
            AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
    }
    
    return true;
}

/* -------------------------------------------------------------------------------- */
inline bool ContinueIterator(Data_PageNo             pageno,
                             Data_PageOffset         offset,
                             Log_UndoFile           &undofile,
                             Log_UndoFile::Iterator &iter,
                             Msg_List               &errlist)
{
    /* PTS 1140252 FF release history page, if savepoint is active */ 
    /* This is needed, because history pages are accessed in       */
    /* Data_ForStructureChange mode to synchronize GC activity     */
    /* on object containers with isDone flag in history entry      */ 
    if ( bd20IsPreventSvpSyncOperationPending() && iter.IsValid() )
    {
        if ( ! iter.Invalidate(errlist) )
            return false;
    }

    if ( iter.IsValid() )
        return true;

    // PTS ? UH 2002-03-15 now ForUpdate && PTS 1140255 FF now ForStructureChange 
    if(!undofile.GetUndoEntry (pageno, offset, Data_ForStructureChange, 0, iter, errlist)) 
    {
        (void)undofile.Invalidate(errlist); 
        return false;
    }
    
    bool result = true;
    (void)iter.Deref(result, errlist);  // really assign the space at the given position
    if ( ! result )
    {
        (void)undofile.Invalidate(errlist); 
        return false;
    }

    return true;
}

/* -------------------------------------------------------------------------------- */
inline bool ContinueIterator(Data_PageNo             pageno,
                             Data_PageOffset         offset,
                             Log_UndoFile           &undofile,
                             Log_UndoFile::Iterator &iter,
                             Data_SplitSpaceReader  &reader,
                             Log_BeforeImage        &image,
                             Log_ActionType          expectedActionType,
                             Msg_List               &errlist)
{
    if ( iter.IsValid() )
        return true;

    if ( ! ContinueIterator(pageno,offset,undofile,iter,errlist) )
        return false;
    
    reader.Reset();

    if ( ! image.ReadPersistentFormat (reader, errlist) )
    {
        Kernel_VTrace() << "current iter position: " << pageno << "." << offset << NewLine;
        image.WriteToTrace ("RemoveUndoFile2");
        image.AppendContextInfo(errlist);
        iter.AppendContextInfo(__CONTEXT__,errlist);
        (void)undofile.Invalidate(errlist); 
        return false;
    }

    if ( expectedActionType != image.GetActionType() )
    {
        Kernel_VTrace() << "current iter position: " << pageno << "." << offset << NewLine;
        image.WriteToTrace ("RemoveUndoFile2");
        errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"Is expected entry type");
        image.AppendContextInfo(errlist);
        iter.AppendContextInfo(__CONTEXT__,errlist);
        (void)undofile.Invalidate(errlist); 
        return false;
    }
    return true;
}

/* -------------------------------------------------------------------------------- */
inline bool HandleActionUpdateRecord(tgg00_TransContext     &trans,
                                     Log_UndoFile           &undofile,
                                     Data_SplitSpaceReader  &reader,
                                     Log_UndoFile::Iterator &iter,
                                     Log_BeforeImage        &image,
                                     Msg_List               &errlist)
{
    if ( ! CnsRead_Manager::IsConsistentRead_Configurated() )
        return true;

    const tgg91_TransNo updTrans = image.GetPreviousUpdateTransNo();
    const tgg91_PageRef prevRef  = image.GetPreviousImage();
    
    Log_ActionUpdateRec   actionUpdateRec   (updTrans, prevRef);
    Log_ActionImageReader actionImageReader (reader, Trans_Context::GetContext(trans).Allocator());

    SAPDBERR_ASSERT_STATE( actionUpdateRec.GarbageCollectionNeeded() );

    if (!actionUpdateRec.ReadImagePersistent(actionImageReader, errlist)) 
        return false;
    
    Data_PageNo     pageno;
    Data_PageOffset offset;

    iter.GetPosition(pageno, offset);
    
    if ( ! iter.Invalidate(errlist) )
    {
        actionUpdateRec.AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    
    actionUpdateRec.GarbageCollection(trans);
    
    if ((e_file_not_found == trans.trError_gg00) || (e_inv_not_found == trans.trError_gg00))
    {
        trans.trError_gg00 = e_ok;
    }

    if ( trans.trError_gg00 != e_ok )
    {
        Kernel_AppendBasisError(trans.trError_gg00, errlist, "Log_UpdateRecord::GarbageCollection() failed" );
        actionUpdateRec.AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }

    return ContinueIterator (pageno, offset, undofile, iter, errlist);
}
/* -------------------------------------------------------------------------------- */
inline bool HandleActionMarkedDeleted (tgg00_TransContext     &trans,
                                       Log_UndoFile           &undofile,
                                       Data_SplitSpaceReader  &reader,
                                       Log_UndoFile::Iterator &iter,
                                       Log_ActionType          actionType,
                                       Msg_List               &errlist)
{
    if ( ! CnsRead_Manager::IsConsistentRead_Configurated() )
        return true;

    Log_ActionInsDelRec   actionInsDelRec  (actionType, true);
    Log_ActionImageReader actionImageReader(reader, Trans_Context::GetContext(trans).Allocator());

    SAPDBERR_ASSERT_STATE( actionInsDelRec.GarbageCollectionNeeded() );

    if (!actionInsDelRec.ReadImagePersistent (actionImageReader, errlist)) 
        return false;

    Data_PageNo     pageno;
    Data_PageOffset offset;

    iter.GetPosition(pageno, offset);
    
    if ( ! iter.Invalidate(errlist) )
    {
        actionInsDelRec.AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    
    tgg91_PageRef wantedPageref;
    wantedPageref.gg91BuildRef (pageno, offset);

    actionInsDelRec.GarbageDelete (trans, wantedPageref, errlist);

    switch (trans.trError_gg00)
    {
    case e_key_not_found  :
    case e_file_not_found :
        {
            trans.trError_gg00 = e_ok;
        }
    }
    if (trans.trError_gg00 != e_ok)
    {
        Kernel_AppendBasisError(trans.trError_gg00, errlist, "Log_MarkedDeleted::GarbageDelete() failed" );
        actionInsDelRec.AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }

    return ContinueIterator (pageno, offset, undofile, iter, errlist);
}
/* -------------------------------------------------------------------------------- */
inline bool HandleActionDeleteObject (tgg00_TransContext     &trans,
                                      Log_UndoFile           &undofile,
                                      Data_SplitSpaceReader  &reader,
                                      Log_UndoFile::Iterator &iter,
                                      Log_BeforeImage        &image,
                                      GC_IGarbageCollector   &IGarbColl,
                                      Msg_List               &errlist)
{
    Log_ActionObject action;
    
    SAPDBERR_ASSERT_STATE( action.GarbageCollectionNeeded() );

    if( ! action.ReadPersistentFormat (reader, Trans_Context::GetContext(trans).Allocator(), errlist))
        return false;

    Data_PageNo     pageno;
    Data_PageOffset offset;

    iter.GetPosition(pageno, offset);
    
    tgg91_PageRef auxPageref;
    auxPageref.gg91BuildRef (pageno, offset);
    
    bd91ReleaseUnusedObject ( trans,
                              iter,
                              action.GetOid(),
                              action.GetFileNo(),
                              auxPageref,
                              IGarbColl,// PTS 1113774 FF 2002-01-29
                              errlist ); 
    
    bool checkHistoryEntryState = false;
    
    switch (trans.trError_gg00)
    {
    case e_ok:
        break;
    case e_wrong_object_version:
    case e_wrong_class_id:
    case e_page_in_wrong_tree:
        checkHistoryEntryState = true;
        trans.trError_gg00 = e_ok;
        break;
    default:
        Kernel_AppendBasisError(trans.trError_gg00, errlist, "bd91ReleaseUnusedObject failed" );
        action.AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    
    if ( checkHistoryEntryState )
    {
        if ( ! ContinueIterator(pageno,offset,undofile,iter,reader,image,Log_DeleteObject,errlist) )
        {
            action.AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
            
        if ( ! ( image.IsRollbacked(false) || image.IsDeleted(false) ) )
        {
            action.AppendContextInfo(__CONTEXT__,errlist);
            errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED, "is Rollbacked or IsDeleted");
            return false;
        }
    }
    else
    {
        if ( ! ContinueIterator (pageno, offset, undofile, iter, errlist) )
        {
            action.AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
    }

    return true;    
}
/* -------------------------------------------------------------------------------- */
inline bool HandleActionDropObjectFile (tgg00_TransContext     &trans,
                                        Log_UndoFile           &undofile,
                                        Data_SplitSpaceReader  &reader,
                                        Log_UndoFile::Iterator &iter,
                                        Msg_List               &errlist)
{
    Log_ActionObjectFile actionFile;
    
    SAPDBERR_ASSERT_STATE( actionFile.GarbageCollectionNeeded() );

    if ( ! actionFile.ReadPersistentFormat (reader, Trans_Context::GetContext(trans).Allocator(), errlist) )
        return false;

    Data_PageNo     pageno;
    Data_PageOffset offset;

    iter.GetPosition(pageno, offset);
    
    if ( ! iter.Invalidate(errlist) )
    {
        actionFile.AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    
    if ( ! actionFile.RemoveGarbage(trans,errlist) )
    {
        actionFile.AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }

    if ( ! ContinueIterator (pageno, offset, undofile, iter, errlist) )
    {
        actionFile.AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    return true;
}
/* -------------------------------------------------------------------------------- */
inline bool HandleActionMarkFileAsDeleted (tgg00_TransContext     &trans,
                                           Log_UndoFile           &undofile,
                                           Data_SplitSpaceReader  &reader,
                                           Log_UndoFile::Iterator &iter,
                                           Msg_List               &errlist)
{
    Log_ActionFile action (Log_MarkFileAsDeleted);

    SAPDBERR_ASSERT_STATE( action.GarbageCollectionNeeded() );

    if (!action.ReadPersistentFormat (reader, Trans_Context::GetContext(trans).Allocator(), errlist))
    {
        action.AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }

    Data_PageNo     pageno;
    Data_PageOffset offset;

    iter.GetPosition(pageno, offset);
    
    if ( ! iter.Invalidate(errlist) )
    {
        action.AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    
    if ( ! action.RemoveGarbage(trans, errlist) )
    {
        action.AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }

    if ( ! ContinueIterator (pageno, offset, undofile, iter, errlist) )
    {
        action.AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    return true;
}
/* -------------------------------------------------------------------------------- */
inline bool HandleActionUnqualifiedDelete (tgg00_TransContext     &trans,
                                           Log_ActionVersion      version,
                                           Log_UndoFile           &undofile,
                                           Data_SplitSpaceReader  &reader,
                                           Log_UndoFile::Iterator &iter,
                                           Msg_List               &errlist)
{
    Trans_Context& transContext = Trans_Context::GetContext(trans);

    SAPDB_AutoPtr<Log_ActionUnqualifiedDelete> action(
        transContext.Allocator(),
        Log_ActionUnqualifiedDelete::CreateObject(transContext.Allocator(), version));
    
    SAPDBERR_ASSERT_STATE( action->GarbageCollectionNeeded() );

    if ( NULL == action.GetPtr() )
    {
        trans.trError_gg00 = e_no_more_memory;
        return false;
    }

    if (!action->ReadPersistentFormat (reader, Trans_Context::GetContext(trans).Allocator(), errlist))
    {
        action->AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }

    Data_PageNo     pageno;
    Data_PageOffset offset;

    iter.GetPosition(pageno, offset);
    
    if ( ! iter.Invalidate(errlist) )
    {
        action->AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    
    if ( ! action->RemoveGarbage(trans, errlist) )
    {
        action->AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }

    if ( ! ContinueIterator (pageno, offset, undofile, iter, errlist) )
    {
        action->AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    return true;
}

/* -------------------------------------------------------------------------------- */
bool Log_History::RemoveUndoFile (tgg00_TransContext        &trans,
                                  const tgg91_TransNo       &transno,
                                  Data_PageNo                root,
                                  Data_PageNo                last,
                                  GC_IGarbageCollector      &IGarbColl,
                                  SAPDB_UInt                 historyFileNo,
                                  Log_HistoryFile           &historyfile,               // PTS 1113934 UH 2002-02-01
                                  Log_HistoryFile::Iterator &undoFileInfoIter,          // PTS 1113934 UH 2002-02-01
                                  bool                      &anythingRemoved,
                                  bool                      &dropAuxFilesNeeded, // PTS 1134156 UH 2005-02-25
                                  Msg_List                  &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_History::RemoveUndoFile", LogHistory_Trace, 5);

    if(!undoFileInfoIter.Break(errlist)) // PTS 1113934 UH 2002-02-01
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }

    anythingRemoved = false;
    
    return ProcessUndoFile (trans, transno, root, last,
                            historyFileNo, &historyfile, &undoFileInfoIter, IGarbColl,
                            anythingRemoved, dropAuxFilesNeeded, errlist);
}

/* -------------------------------------------------------------------------------- */
    /// This is used by transactions to remove their undo file directly
bool Log_History::RemoveUndoFileByTransaction (tgg00_TransContext        &trans,             // PTS 1113934 UH 2002-02-01
                                               const tgg91_TransNo       &transno,
                                               Data_PageNo                root,
                                               Data_PageNo                last,
                                               Msg_List                  &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_History::RemoveUndoFileByTransaction", LogHistory_Trace, 5);

    bool    dropAuxFilesNeeded = false;
    bool    anythingRemoved    = false;
    DummyGC IGarbColl;
    
    if ( ! ProcessUndoFile (trans, transno, root, last, 0, 0, 0, IGarbColl,
                            anythingRemoved, dropAuxFilesNeeded, errlist) )
        return false;
        
    if ( dropAuxFilesNeeded )
        kb900DropAuxFiles(trans);

    if ( ! anythingRemoved )
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    
    return trans.trError_gg00 == e_ok;
}

/* -------------------------------------------------------------------------------- */
bool Log_History::ProcessUndoFile (tgg00_TransContext        &trans,
                                   const tgg91_TransNo       &transno,
                                   Data_PageNo                root,
                                   Data_PageNo                last,
                                   SAPDB_UInt                 historyFileNo,
                                   Log_HistoryFile           *pHistoryFile,
                                   Log_HistoryFile::Iterator *pUndoFileInfoIter,
                                   GC_IGarbageCollector      &IGarbColl,
                                   bool                      &anythingRemoved,
                                   bool                      &dropAuxFilesNeeded, // PTS 1134156 UH 2005-02-25
                                   Msg_List                  &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_History::ProcessUndoFile", LogHistory_Trace, 5);

    anythingRemoved = false;
    
    SAPDBMem_IRawAllocator &allocator = Trans_Context::GetContext(trans).Allocator();
    Log_UndoFile            undofile (trans, transno, root, last);
    Log_BeforeImage         image;
    bool                    result = true;
    Data_PageNo             pageno;
    Data_PageOffset         offset = 0;                    // UH 2002-03-15
    Log_UndoFile::Iterator &iter = undofile.GetIterator(); // UH 2002-03-15

    // UH 2002-03-15
    // If a previous run was interrupted then start at the last entry which was done at the last time.
    if(!undofile.GetLastGarbageCollectorPosition (pageno, offset, errlist))
    {
        trans.trError_gg00 = e_ok;

        Msg_List dummyErrorList;
        
        if ( pHistoryFile != 0 && pUndoFileInfoIter != 0
             &&
             pUndoFileInfoIter->Continue(dummyErrorList)
             &&
             (**pUndoFileInfoIter).root == root )
        {
            // determine new oldest transno
            tgg91_TransNo newOldestTransno;
            SAPDB_UInt    pagecount = (**pUndoFileInfoIter).pagecount;

            (void) pHistoryFile->RemoveUndoFileInfo (*pUndoFileInfoIter,newOldestTransno,dummyErrorList);

            m_Directory.UpdateOldestTransNo ( trans.trTaskId_gg00,
                                            historyFileNo,
                                            newOldestTransno,
                                            pHistoryFile->LastPageNo(),
                                            pagecount );
        }
        (void)undofile.Invalidate(dummyErrorList); 
        AppendContextInfo(__CONTEXT__,errlist, "INFO: UndoFileRoot not found. Can be ignored.");
        RTE_Message(errlist);
        errlist.ClearMessageList();
        anythingRemoved = true;
        return true; // PTS 1141745 UH Ignoring error that undofile is not accessable
                     //                until the reason is found.
    }

    if ( pageno.IsValid() && offset > 0 )
    {
        // PTS 1140252 FF now ForStructureChange 
        if(!undofile.GetUndoEntry (pageno, offset, Data_ForStructureChange, 0, iter, errlist))
        {
            AppendContextInfo(__CONTEXT__,errlist);
            (void)undofile.Invalidate(errlist); 
            return false;
        }
    }
    else
    {
        // PTS 1140255 FF now ForStructureChange 
        if(!undofile.GetLastUndoEntry (Data_ForStructureChange, iter, errlist))
        {
            AppendContextInfo(__CONTEXT__,errlist);
            (void)undofile.Invalidate(errlist); 
            return false;
        }
    }

    if ( trans.trError_gg00 == e_corrupted_datapage_from_io ) // PTS 1117126 UH 2002-08-07
    {
        (void)undofile.Invalidate(errlist); 
        return false;
    }

    Log_ActionType        currentActionType = Log_NoOp;
    Data_SplitSpaceReader reader (iter.Deref(result,errlist), true); // PTS 1114994 UH 2002-04-17 added releasable
    if( ! result )
    {
        AppendContextInfo(__CONTEXT__,errlist);
        (void)undofile.Invalidate(errlist); 
        return false;
    }
    
    bool allUndoEntriesDone = ! undofile.GarbageCollectionNeeded(); // PTS 1140415 UH 2006-05-04

    if ( allUndoEntriesDone )
    {
        if ( LogHistory_Trace.TracesLevel(6) )
            undofile.WriteToTrace("REMOVED DIRECTLY");
            
        if ( ! iter.Break(errlist) )
        {
            AppendContextInfo(__CONTEXT__,errlist);
            (void)undofile.Invalidate(errlist); 
            return false;
        }
    }
    
    while ( iter.IsValid()
            &&
            ! IGarbColl.IsInterrrupted()
            &&
            ! allUndoEntriesDone )
    {
        iter.GetPosition(pageno, offset);

        reader.Reset();
        if(!image.ReadPersistentFormat (reader, errlist))
        {
            Kernel_VTrace() << "current iter position: " << pageno << "." << offset << NewLine;
            iter.AppendContextInfo(__CONTEXT__,errlist);
            AppendContextInfo(__CONTEXT__,errlist);
            (void)undofile.Invalidate(errlist); 
            return false;
        }

        allUndoEntriesDone = image.GetSequence() == 0; // UH 2002-05-03
        currentActionType  = image.GetActionType();


        if ( ! image.IsDeleted(false) )
            UpdateStatistic(IGarbColl, currentActionType);
            
        if ( ! image.IsDeleted(false) // PTS 1113998 UH 2002-02-05
             &&
             ( ! image.IsRollbacked(false) || Log_DeleteObject == currentActionType) )
        
        {
            // must be set here because iterator may be interrupted during action handling
            image.SetDeleted(true);

            bool result = true;
            
            switch ( currentActionType )
            {
                case Log_UpdateRecord:
                    result = HandleActionUpdateRecord(trans, undofile, reader, iter, image, errlist);
                    break;
                case Log_MarkedDeleted:
                    result = HandleActionMarkedDeleted(trans, undofile, reader, iter, currentActionType, errlist);
                    break;
                case Log_DeleteObject:
                    result = HandleActionDeleteObject(trans, undofile, reader, iter, image, IGarbColl, errlist);
                    break;
                case Log_DropObjectFile:
                    result = HandleActionDropObjectFile(trans, undofile, reader, iter, errlist);
                    break;
                case Log_MarkFileAsDeleted:
                    result = HandleActionMarkFileAsDeleted(trans, undofile, reader, iter, errlist);
                    dropAuxFilesNeeded |= result; // PTS 1134156 UH 2005-02-25
                    break;
                case Log_UnqualifiedDelete:
                    result = HandleActionUnqualifiedDelete(trans, image.GetPersistentVersion(), undofile, reader, iter, errlist);
                    dropAuxFilesNeeded |= result; // PTS 1134156 UH 2005-02-25
                    break;
                default:
                    ; // nothing special to do with this undo entry
            }
            
            if ( ! result )
            {
                Kernel_VTrace() << "current iter position: " << pageno << "." << offset << NewLine;
                image.WriteToTrace      ("RemoveUndoFile");
                image.AppendContextInfo (errlist);
                iter.AppendContextInfo  (__CONTEXT__,errlist);
                (void)undofile.Invalidate(errlist); 
                AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }
        }
        
        if(!iter.MinusMinus(errlist))
        {
            AppendContextInfo(__CONTEXT__,errlist);
            iter.AppendContextInfo(__CONTEXT__,errlist);
            (void)undofile.Invalidate(errlist); 
            return false;
        }
        (void)iter.Deref(result,errlist);
        if(!result) // really assign the space
        {
            AppendContextInfo(__CONTEXT__,errlist);
            iter.AppendContextInfo(__CONTEXT__,errlist);
            (void)undofile.Invalidate(errlist); 
            return false;
        }
    }

    if(!iter.Invalidate(errlist))
    {
        AppendContextInfo(__CONTEXT__,errlist);
        (void)undofile.Invalidate(errlist); 
        return false;
    }

    // PTS 1113934 UH 2002-02-01 begin
    if ( allUndoEntriesDone ) // UH 2002-03-15
    {
        if ( pUndoFileInfoIter != 0 && pHistoryFile != 0 )
        {
            if ( ! pUndoFileInfoIter->Continue(errlist) )
            {
                pUndoFileInfoIter->WriteToTrace ("undoFileInfoIter");
                AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }
    
            if ( (**pUndoFileInfoIter).root != root )
            {
                pUndoFileInfoIter->WriteToTrace ("undoFileInfoIter");
                errlist = Log_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED, "iter points to correct entry");
                AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }
                   
            // determine new oldest transno
            tgg91_TransNo newOldestTransno;
            SAPDB_UInt    pagecount = (**pUndoFileInfoIter).pagecount;
            
            if(!pHistoryFile->RemoveUndoFileInfo (*pUndoFileInfoIter,newOldestTransno,errlist))
            {
                AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }

            // The later update only is correct,
            // if the garbage collectors are stopped during savepoint.
            m_Directory.UpdateOldestTransNo ( trans.trTaskId_gg00,
                                              historyFileNo,
                                              newOldestTransno,
                                              pHistoryFile->LastPageNo(),
                                              pagecount );
        } 

        // 1. remove history entry which references undofile
        // 2. drop undofile

        bool breakIterator;

        if ( pUndoFileInfoIter == 0 )
            breakIterator = false;
        else
        {
            if ( pUndoFileInfoIter->IsValid() )
                breakIterator = true;
            else
            {
                (void)pUndoFileInfoIter->Invalidate(errlist); // really release the history page
                breakIterator = false;
            }
        }

        if ( breakIterator && ! (*pUndoFileInfoIter).Break(errlist) )
        {
            AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }

        if ( ! undofile.Drop(errlist) )
        {
            AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
        
        if ( breakIterator && ! (*pUndoFileInfoIter).Continue(errlist) )
        {
            (*pUndoFileInfoIter).WriteToTrace ("pUndoFileInfoIter");
            AppendContextInfo(__CONTEXT__,errlist);
            (void)undofile.Invalidate(errlist); 
            return false;
        }

        anythingRemoved = true;
    }
    else
    {
        // Save pageno and offset to start there at the next run.
        if(!undofile.SaveLastGarbageCollectorPosition (pageno, offset, errlist)) // UH 2002-03-15
        {
            AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }

        (void)undofile.Invalidate(errlist); // prevent Drop() in destructor
    }
    // PTS 1113934 UH 2002-02-01 end
    return true;
}

/* -------------------------------------------------------------------------------- */
Log_History::GetResult
Log_History::GetBeforeImage
     (tgg00_TransContext       &Trans,
      const tgg92_KernelOid    &TestOid,
      const tgg91_PageRef      &WantedPageRef,
      SAPDB_UInt4               BodySize,
      SAPDB_Byte               *pBody,
      bool                      bOnlyCheckExistence, // PTS 1113317 UH 2002-01-08
      SAPDB_UInt4              &BodyLen,
      tgg00_ObjFrameVers       &FrameVers,
      Log_ActionType           &ActionType,
      tgg91_TransNo            &ConsistentView,
      tgg91_TransNo            &PrevUpdTrans,
      tgg91_PageRef            &NextPageRef,
      Msg_List     &errlist)
{
    SAPDBTRACE_ROUTINE_DEBUG ("Log_History::GetBeforeImage", LogHistory_Trace, 5);

    const bool            bWithBody = (BodySize > 0);
    const Data_PageNo     pageno    = WantedPageRef.gg91RefPno();
    const Data_PageOffset offset    = WantedPageRef.gg91RefPos();

    if ( pageno.IsInvalid()
         ||
         offset != Data_PageSplitSpace::AlignRecordSize(offset) )
         // PTS 1119860 UH 2003-01-08 return with error if offest is not aligned
    {
        errlist = Log_Exception(__CONTEXT__, LOG_ILLEGAL_PAGEREF,
                                SAPDB_ToString(pageno),
                                SAPDB_ToString(offset));
        errlist = errlist + Log_Exception(__CONTEXT__, LOG_CONSISTENT_VIEW_INFO,
                                          SAPDB_ToString(Trans.trConsistView_gg00.gg90GetInt4()));
        errlist = errlist + Log_Exception(__CONTEXT__, LOG_PAGEREF_INFO,
                                          "oid",
                                          SAPDB_ToString(TestOid.gg92GetPno()),
                                          SAPDB_ToString(TestOid.gg92GetPos()));
        GetInstance().AppendContextInfo(__CONTEXT__,errlist);
        return getIllegalPageRef;
    }

    SAPDBMem_IRawAllocator &allocator = *(reinterpret_cast<SAPDBMem_IRawAllocator*>(Trans.trAllocator_gg00));
    Log_UndoFile            file (Trans, Data_PageNo(), Data_PageNo());
    Log_UndoFile::Iterator  iter (file.PageAccessManager(), allocator);
    
    if ( ! iter.Initialize() )
    {
        errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"iter.Initialize()");
        GetInstance().AppendContextInfo(__CONTEXT__,errlist);
        return getInitError;
    }

    // PTS 1117126 UH 2002-08-07 maxAllowedEntrySize check added
    
    const Data_SplitRecordLength maxAllowedEntrySize =
                      3 * ( Log_UndoPage::MaxSpaceOffset()
                      -Log_UndoPage::MinSpaceOffset() );

    if(!file.GetUndoEntry ( pageno, offset, Data_ForRead,
                            maxAllowedEntrySize, iter, errlist ) )
	{
        errlist = errlist + Log_Exception(__CONTEXT__, LOG_CONSISTENT_VIEW_INFO,
                                          SAPDB_ToString(Trans.trConsistView_gg00.gg90GetInt4()));
        errlist = errlist + Log_Exception(__CONTEXT__, LOG_PAGEREF_INFO,
                                          "pageref",
                                          SAPDB_ToString(pageno),
                                          SAPDB_ToString(offset));
        errlist = errlist + Log_Exception(__CONTEXT__, LOG_PAGEREF_INFO,
                                          "oid",
                                          SAPDB_ToString(TestOid.gg92GetPno()),
                                          SAPDB_ToString(TestOid.gg92GetPos()));
        GetInstance().AppendContextInfo(__CONTEXT__,errlist);
        return getAccessingUndoEntryFailed;
	}
                            
    if ( ! iter.IsValid() )
    {
        if ( LogHistory_Trace.TracesLevel(6) )
        {
            Kernel_VTrace() << "GetBeforeImage() history get: " << pageno << "." << offset
                            << " for oid: " << TestOid.gg92GetPno() << "." << TestOid.gg92GetPos()
                            << " with view: " << Trans.trConsistView_gg00.gg90GetInt4()
                            << NewLine;
            Kernel_VTrace() << "GetBeforeImage() entry not found" << NewLine;
        }
        (void)iter.Invalidate(errlist);
        (void)file.Invalidate(errlist);
        return getImageNotFound;
    }
    
    bool                      derefResult = true;
    Log_UndoFile::EntrySpace &space       = iter.Deref(derefResult, errlist);
    
    if ( ! derefResult )
    {
        errlist = errlist + Log_Exception(__CONTEXT__, LOG_CONSISTENT_VIEW_INFO,
                                          SAPDB_ToString(Trans.trConsistView_gg00.gg90GetInt4()));
        errlist = errlist + Log_Exception(__CONTEXT__, LOG_PAGEREF_INFO,
                                          "pageref",
                                          SAPDB_ToString(pageno),
                                          SAPDB_ToString(offset));
        errlist = errlist + Log_Exception(__CONTEXT__, LOG_PAGEREF_INFO,
                                          "oid",
                                          SAPDB_ToString(TestOid.gg92GetPno()),
                                          SAPDB_ToString(TestOid.gg92GetPos()));
        GetInstance().AppendContextInfo(__CONTEXT__,errlist);
        return getAccessingUndoEntryFailed;
    }

    if ( ! space.CheckAlignment() )
    {
        errlist = errlist + Log_Exception(__CONTEXT__, LOG_CONSISTENT_VIEW_INFO,
                                          SAPDB_ToString(Trans.trConsistView_gg00.gg90GetInt4()));
        errlist = errlist + Log_Exception(__CONTEXT__, LOG_PAGEREF_INFO,
                                          "pageref",
                                          SAPDB_ToString(pageno),
                                          SAPDB_ToString(offset));
        errlist = errlist + Log_Exception(__CONTEXT__, LOG_PAGEREF_INFO,
                                          "oid",
                                          SAPDB_ToString(TestOid.gg92GetPno()),
                                          SAPDB_ToString(TestOid.gg92GetPos()));
        GetInstance().AppendContextInfo(__CONTEXT__,errlist);
        (void)iter.Invalidate(errlist);
        (void)file.Invalidate(errlist);
        return getImageNotFound;
    }
    
    Log_BeforeImage       image;
    Data_SplitSpaceReader reader (space);
    
    if (! image.ReadPersistentFormat (reader, errlist) )
    {
        if ( LogHistory_Trace.TracesLevel(6) )
        {
            Kernel_VTrace() << "GetBeforeImage() history get: " << pageno << "." << offset
                            << " for oid: " << TestOid.gg92GetPno() << "." << TestOid.gg92GetPos()
                            << " with view: " << Trans.trConsistView_gg00.gg90GetInt4()
                            << NewLine;
            Kernel_VTrace() << "GetBeforeImage() could not read image" << NewLine;
        }
        errlist = errlist + Log_Exception(__CONTEXT__, LOG_CONSISTENT_VIEW_INFO,
                                          SAPDB_ToString(Trans.trConsistView_gg00.gg90GetInt4()));
        errlist = errlist + Log_Exception(__CONTEXT__, LOG_PAGEREF_INFO,
                                          "pageref",
                                          SAPDB_ToString(pageno),
                                          SAPDB_ToString(offset));
        errlist = errlist + Log_Exception(__CONTEXT__, LOG_PAGEREF_INFO,
                                          "oid",
                                          SAPDB_ToString(TestOid.gg92GetPno()),
                                          SAPDB_ToString(TestOid.gg92GetPos()));
        GetInstance().AppendContextInfo(__CONTEXT__,errlist);
        (void)iter.Invalidate(errlist);
        (void)file.Invalidate(errlist);
        return getReadError;
    }

    ActionType     = image.GetActionType();
    ConsistentView = image.GetConsistentView();
    PrevUpdTrans   = image.GetPreviousUpdateTransNo();
    NextPageRef    = image.GetPreviousImage();
    
    switch ( ActionType )
    {
    case Log_NewObject:
    case Log_InsertObject:
    case Log_UpdateObject:
    case Log_DeleteObject:
    case Log_LockObject:
        {
            Log_ActionObject action;
            
            if(!action.ReadPersistentFormat (reader, allocator, errlist))
            {
                if ( LogHistory_Trace.TracesLevel(6) )
                {
                    Kernel_VTrace() << "GetBeforeImage() history get: " << pageno << "." << offset
                                    << " for oid: " << TestOid.gg92GetPno() << "." << TestOid.gg92GetPos()
                                    << " with view: " << Trans.trConsistView_gg00.gg90GetInt4()
                                    << NewLine;
                    Kernel_VTrace() << "GetBeforeImage() could not read action" << NewLine;
                    image.WriteToTrace("GetBeforeImage()");
                }
                image.AppendContextInfo(errlist);
                errlist = errlist + Log_Exception(__CONTEXT__, LOG_CONSISTENT_VIEW_INFO,
                                                  SAPDB_ToString(Trans.trConsistView_gg00.gg90GetInt4()));
                errlist = errlist + Log_Exception(__CONTEXT__, LOG_PAGEREF_INFO,
                                                  "pageref",
                                                  SAPDB_ToString(pageno),
                                                  SAPDB_ToString(offset));
                errlist = errlist + Log_Exception(__CONTEXT__, LOG_PAGEREF_INFO,
                                                  "oid",
                                                  SAPDB_ToString(TestOid.gg92GetPno()),
                                                  SAPDB_ToString(TestOid.gg92GetPos()));
                GetInstance().AppendContextInfo(__CONTEXT__,errlist);
                (void)iter.Invalidate(errlist);
                (void)file.Invalidate(errlist);
                return getReadError;
            }
            
            if ( ! action.GetOid().gg92IsEqIgnoreFrameVers(TestOid) )
            {
                if ( LogHistory_Trace.TracesLevel(6) )
                {
                    Kernel_VTrace() << "GetBeforeImage() history get: " << pageno << "." << offset
                                    << " for oid: " << TestOid.gg92GetPno() << "." << TestOid.gg92GetPos()
                                    << " with view: " << Trans.trConsistView_gg00.gg90GetInt4() << NewLine;
                    image.WriteToTrace("GetBeforeImage()");
                    action.WriteToTrace("GetBeforeImage()");
                    Kernel_VTrace() << "GetBeforeImage() invalid oid" << NewLine;
                }
                errlist = Log_Exception(__CONTEXT__, LOG_WRONG_HISTORY);
                image.AppendContextInfo(errlist);
                action.AppendContextInfo(__CONTEXT__,errlist);
                errlist = errlist + Log_Exception(__CONTEXT__, LOG_CONSISTENT_VIEW_INFO,
                                                  SAPDB_ToString(Trans.trConsistView_gg00.gg90GetInt4()));
                errlist = errlist + Log_Exception(__CONTEXT__, LOG_PAGEREF_INFO,
                                                  "pageref",
                                                  SAPDB_ToString(pageno),
                                                  SAPDB_ToString(offset));
                errlist = errlist + Log_Exception(__CONTEXT__, LOG_PAGEREF_INFO,
                                                  "expected",
                                                  SAPDB_ToString(TestOid.gg92GetPno()),
                                                  SAPDB_ToString(TestOid.gg92GetPos()));
                errlist = errlist + Log_Exception(__CONTEXT__, LOG_PAGEREF_INFO,
                                                  "found",
                                                  SAPDB_ToString(action.GetOid().gg92GetPno()),
                                                  SAPDB_ToString(action.GetOid().gg92GetPos()));
                GetInstance().AppendContextInfo(__CONTEXT__,errlist);
                (void)iter.Invalidate(errlist);
                (void)file.Invalidate(errlist);
                return getWrongEntry;
            }

            FrameVers = action.GetOid().gg92GetFrameVers();
            BodyLen   = action.GetBodySize();

            if ( pBody == NULL || ActionType == Log_LockObject )
                break; // no body needed or wanted

            if ( BodyLen > BodySize )
            {
                Kernel_VTrace() << "GetBeforeImage() history get: " << pageno << "." << offset
                                << " for oid: " << TestOid.gg92GetPno() << "." << TestOid.gg92GetPos()
                                << " with view: " << Trans.trConsistView_gg00.gg90GetInt4()
                                << NewLine;
                image.WriteToTrace("GetBeforeImage()");
                action.WriteToTrace("GetBeforeImage()");
                Kernel_VTrace() << "GetBeforeImage() BodySize too small: " << BodySize << " < " << BodyLen
                                << NewLine;
                errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"before image fits into bodysize");
                errlist = errlist + Log_Exception(__CONTEXT__, LOG_CONSISTENT_VIEW_INFO,
                                                  SAPDB_ToString(Trans.trConsistView_gg00.gg90GetInt4()));
                errlist = errlist + Log_Exception(__CONTEXT__, LOG_PAGEREF_INFO,
                                                  "pageref",
                                                  SAPDB_ToString(pageno),
                                                  SAPDB_ToString(offset));
                image.AppendContextInfo(errlist);
                action.AppendContextInfo(__CONTEXT__,errlist);
                GetInstance().AppendContextInfo(__CONTEXT__,errlist);
                (void)iter.Invalidate(errlist);
                (void)file.Invalidate(errlist);
                return getBodySizeTooSmall;
            }

            if ( Log_DeleteObject == ActionType || Log_UpdateObject == ActionType )
                SAPDB_MemCopyNoCheck (pBody, action.GetBody(), BodyLen);
            else if ( Log_NewObject == ActionType || Log_InsertObject == ActionType )
                SAPDB_MemFillNoCheck (pBody, 0x00, BodyLen);
        }
        break;
    default:
        if ( LogHistory_Trace.TracesLevel(6) )
        {
            Kernel_VTrace() << "GetBeforeImage() history get: " << pageno << "." << offset
                            << " for oid: " << TestOid.gg92GetPno() << "." << TestOid.gg92GetPos()
                            << " with view: " << Trans.trConsistView_gg00.gg90GetInt4() << NewLine;
            image.WriteToTrace("GetBeforeImage()");
            Kernel_VTrace() << "GetBeforeImage() invalid action type '" << Log_GetActionTypeString(ActionType) // PTS 1134693 UH 2005-04-01
                            << NewLine;
        }
        errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"action expected for consistent read");
        errlist = errlist + Log_Exception(__CONTEXT__, LOG_CONSISTENT_VIEW_INFO,
                                          SAPDB_ToString(Trans.trConsistView_gg00.gg90GetInt4()));
        errlist = errlist + Log_Exception(__CONTEXT__, LOG_PAGEREF_INFO,
                                          "pageref",
                                          SAPDB_ToString(pageno),
                                          SAPDB_ToString(offset));
        errlist = errlist + Log_Exception(__CONTEXT__, LOG_PAGEREF_INFO,
                                          "oid",
                                          SAPDB_ToString(TestOid.gg92GetPno()),
                                          SAPDB_ToString(TestOid.gg92GetPos()));
        image.AppendContextInfo(errlist);
        GetInstance().AppendContextInfo(__CONTEXT__,errlist);
        (void)iter.Invalidate(errlist);
        (void)file.Invalidate(errlist);
        return getActionNotPreparedForConsistentRead;
    }        

    // PTS 1113253 UH 2001-12-27 activate the if-clause
    if ( image.IsDeleted(false) && bOnlyCheckExistence /* PTS 1113317 UH 2002-01-08 */ )
    {
        if ( LogHistory_Trace.TracesLevel(6) )
        {
            Kernel_VTrace() << "GetBeforeImage() history get: " << pageno << "." << offset
                            << " for oid: " << TestOid.gg92GetPno() << "." << TestOid.gg92GetPos()
                            << " with view: " << Trans.trConsistView_gg00.gg90GetInt4() << NewLine;
            image.WriteToTrace("GetBeforeImage()");
            Kernel_VTrace() << "GetBeforeImage() IsDeleted = true && bOnlyCheckExistence" << NewLine;
        }
        return getImageNotFoundBecauseDeleted;
    }

    if(!iter.Invalidate(errlist)||!file.Invalidate(errlist))
    {
        errlist = errlist + Log_Exception(__CONTEXT__, LOG_CONSISTENT_VIEW_INFO,
                                          SAPDB_ToString(Trans.trConsistView_gg00.gg90GetInt4()));
        errlist = errlist + Log_Exception(__CONTEXT__, LOG_PAGEREF_INFO,
                                          "pageref",
                                          SAPDB_ToString(pageno),
                                          SAPDB_ToString(offset));
        errlist = errlist + Log_Exception(__CONTEXT__, LOG_PAGEREF_INFO,
                                          "oid",
                                          SAPDB_ToString(TestOid.gg92GetPno()),
                                          SAPDB_ToString(TestOid.gg92GetPos()));
        GetInstance().AppendContextInfo(__CONTEXT__,errlist);
        return getReleasingImageFailed;
    }
    return getImageFound;
}

/* -------------------------------------------------------------------------------- */
Log_History::GetResult
Log_History::GetBeforeImage (tgg00_TransContext   &Trans,
                             tgg91_PageRef        &WantedPageRef,
                             tgg91_TransNo        &ConsistentView,
                             tgg00_Rec            &BeforeImage,
                             tgg91_TransNo        &UpdTrans,
                             Msg_List &errlist)
{
    SAPDBTRACE_ROUTINE_DEBUG ("Log_History::GetBeforeImage", LogHistory_Trace, 5);

    const Data_PageNo     pageno    = WantedPageRef.gg91RefPno();
    const Data_PageOffset offset    = WantedPageRef.gg91RefPos();

    if ( pageno.IsInvalid() 
         ||
         offset != Data_PageSplitSpace::AlignRecordSize(offset) )
    {
        errlist = Log_Exception(__CONTEXT__, LOG_ILLEGAL_PAGEREF,
                                SAPDB_ToString(pageno),
                                SAPDB_ToString(offset));
        errlist = errlist + Log_Exception(__CONTEXT__, LOG_CONSISTENT_VIEW_INFO,
                                          SAPDB_ToString(Trans.trConsistView_gg00.gg90GetInt4()));
        GetInstance().AppendContextInfo(__CONTEXT__,errlist);
        return getIllegalPageRef;
    }

    SAPDBMem_IRawAllocator &allocator = *(reinterpret_cast<SAPDBMem_IRawAllocator*>(Trans.trAllocator_gg00));
    Log_UndoFile            file (Trans, Data_PageNo(), Data_PageNo());
    Log_UndoFile::Iterator  iter (file.PageAccessManager(), allocator);

    if ( ! iter.Initialize() )
    {
        errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"iter.Initialize()");
        GetInstance().AppendContextInfo(__CONTEXT__,errlist);
        return getInitError;
    }

    // PTS 1117126 UH 2002-08-07 maxAllowedEntrySize check added

    const Data_SplitRecordLength maxAllowedEntrySize =
        3 * ( Log_UndoPage::MaxSpaceOffset()
        -Log_UndoPage::MinSpaceOffset() );

    if (!file.GetUndoEntry ( pageno, offset, Data_ForRead,
        maxAllowedEntrySize, iter, errlist ) )
    {
        errlist = errlist + Log_Exception(__CONTEXT__, LOG_PAGEREF_INFO,
            "pageref",
            SAPDB_ToString(pageno),
            SAPDB_ToString(offset));
        GetInstance().AppendContextInfo(__CONTEXT__,errlist);
        return getAccessingUndoEntryFailed;
    }

    if ( ! iter.IsValid() )
    {
        if ( LogHistory_Trace.TracesLevel(6) )
        {
            Kernel_VTrace() << "history del: " << pageno << "." << offset << NewLine;
            Kernel_VTrace() << "*** Log_History::GetBeforeImage() entry not found" << NewLine;
        }
        (void)iter.Invalidate(errlist);
        (void)file.Invalidate(errlist);
        return getImageNotFound;
    }

    bool                  derefResult = true;
    Log_BeforeImage       image;
    Data_SplitSpaceReader reader (iter.Deref(derefResult, errlist));

    if ( ! derefResult )
    {
        errlist = errlist + Log_Exception(__CONTEXT__, LOG_CONSISTENT_VIEW_INFO,
                                          SAPDB_ToString(Trans.trConsistView_gg00.gg90GetInt4()));
        errlist = errlist + Log_Exception(__CONTEXT__, LOG_PAGEREF_INFO,
                                          "pageref",
                                          SAPDB_ToString(pageno),
                                          SAPDB_ToString(offset));
        GetInstance().AppendContextInfo(__CONTEXT__,errlist);
        return getAccessingUndoEntryFailed;
    }
    if (! image.ReadPersistentFormat (reader, errlist) )
    {
        if ( LogHistory_Trace.TracesLevel(6) )
        {
            Kernel_VTrace() << "GetBeforeImage() history get: " << pageno << "." << offset
                            << " with view: " << Trans.trConsistView_gg00.gg90GetInt4()
                            << NewLine;
            Kernel_VTrace() << "GetBeforeImage() could not read image" << NewLine;
        }
        errlist = errlist + Log_Exception(__CONTEXT__, LOG_CONSISTENT_VIEW_INFO,
                                          SAPDB_ToString(Trans.trConsistView_gg00.gg90GetInt4()));
        errlist = errlist + Log_Exception(__CONTEXT__, LOG_PAGEREF_INFO,
                                          "pageref",
                                          SAPDB_ToString(pageno),
                                          SAPDB_ToString(offset));
        GetInstance().AppendContextInfo(__CONTEXT__,errlist);
        (void)iter.Invalidate(errlist);
        (void)file.Invalidate(errlist);
        return getReadError;
    }

    Log_ActionType ActionType = image.GetActionType();
	switch (ActionType)
	{
    case Log_MarkedDeleted :
        {
            BeforeImage.recVarcolCnt_gg00() -= 0x4000; // remove deleted flag
            /*
            Log_ActionInsDelRec actionInsDelRec (image.GetActionType(), true);
            Log_ActionImageReader actionImageReader(reader, allocator);
            if (!actionInsDelRec.ReadImagePersistent (actionImageReader, errlist)) 
            {
                errlist = errlist + Log_Exception(__CONTEXT__, LOG_CONSISTENT_VIEW_INFO,
                    SAPDB_ToString(Trans.trConsistView_gg00.gg90GetInt4()));
                errlist = errlist + Log_Exception(__CONTEXT__, LOG_PAGEREF_INFO,
                    "pageref",
                    SAPDB_ToString(pageno),
                    SAPDB_ToString(offset));
                GetInstance().AppendContextInfo(__CONTEXT__,errlist);
                return getReadError;
            }
            actionInsDelRec.MaterializeRecord(sizeof(BeforeImage), &BeforeImage, Trans.trError_gg00);
            */
            break;
        }
    case Log_InsertRecord :
        {
            Log_ActionInsDelRec actionInsDelRec (image.GetActionType(), true);
            Log_ActionImageReader actionImageReader(reader, allocator);
            if (!actionInsDelRec.ReadImagePersistent (actionImageReader, errlist)) 
            {
                errlist = errlist + Log_Exception(__CONTEXT__, LOG_CONSISTENT_VIEW_INFO,
                    SAPDB_ToString(Trans.trConsistView_gg00.gg90GetInt4()));
                errlist = errlist + Log_Exception(__CONTEXT__, LOG_PAGEREF_INFO,
                    "pageref",
                    SAPDB_ToString(pageno),
                    SAPDB_ToString(offset));
                GetInstance().AppendContextInfo(__CONTEXT__,errlist);
                return getReadError;
            }
            actionInsDelRec.MaterializeRecord(sizeof(BeforeImage), &BeforeImage, Trans.trError_gg00);
            BeforeImage.recVarcolCnt_gg00() |= 0x4000; // mark deleted
            break;
        }
    case Log_UpdateRecord :
        {
            Log_ActionUpdateRec actionUpdateRec;
            Log_ActionImageReader actionImageReader(reader, allocator);
            if (!actionUpdateRec.ReadImagePersistent (actionImageReader, errlist)) 
            {
                errlist = errlist + Log_Exception(__CONTEXT__, LOG_CONSISTENT_VIEW_INFO,
                    SAPDB_ToString(Trans.trConsistView_gg00.gg90GetInt4()));
                errlist = errlist + Log_Exception(__CONTEXT__, LOG_PAGEREF_INFO,
                    "pageref",
                    SAPDB_ToString(pageno),
                    SAPDB_ToString(offset));
                GetInstance().AppendContextInfo(__CONTEXT__,errlist);
                return getReadError;
            }
            tgg00_Rec auxImage;
            SAPDB_MemCopyNoCheck (&auxImage, &BeforeImage, BeforeImage.recLen_gg00());
            actionUpdateRec.CreateBeforeImage(auxImage, BeforeImage);
            break;
        }
    }
    ConsistentView = image.GetConsistentView();
    UpdTrans       = image.GetPreviousUpdateTransNo();
    WantedPageRef  = image.GetPreviousImage();

    return getImageFound;
}

/* -------------------------------------------------------------------------------- */
Log_History::DeleteResult
Log_History::DeleteBeforeImage (tgg00_TransContext    &trans,
                                const tgg92_KernelOid &oid,
                                const tgg91_PageRef   &pageref,
                                Msg_List  &errlist)
{
    SAPDBTRACE_ROUTINE_DEBUG ("Log_History::DeleteBeforeImage", LogHistory_Trace, 5);

    const Data_PageNo     pageno = pageref.gg91RefPno();
    const Data_PageOffset offset = pageref.gg91RefPos();

    // PTS 1113251 UH 2001-12-27
    // removed all RTE_Crash calls and moved the trace output to the error cases
    
    if ( pageno.IsInvalid()
         ||
         offset != Data_PageSplitSpace::AlignRecordSize(offset) )
    {
        if ( LogHistory_Trace.TracesLevel(6) )
        {
            Kernel_VTrace() << "history del: " << pageno << "." << offset << NewLine;
            Kernel_VTrace() << "*** Log_History::DeleteBeforeImage() pageref is NIL" << NewLine;
        }
        errlist = Log_Exception(__CONTEXT__, LOG_ILLEGAL_PAGEREF,
                                SAPDB_ToString(pageno),
                                SAPDB_ToString(offset));
        errlist = errlist + Log_Exception(__CONTEXT__, LOG_PAGEREF_INFO,
                                          "oid",
                                          SAPDB_ToString(oid.gg92GetPno()),
                                          SAPDB_ToString(oid.gg92GetPos()));
        GetInstance().AppendContextInfo(__CONTEXT__,errlist);
        return delIllegalPageref;
    }

    SAPDBMem_IRawAllocator &allocator = *(reinterpret_cast<SAPDBMem_IRawAllocator*>(trans.trAllocator_gg00));
    Log_UndoFile            file (trans, Data_PageNo(), Data_PageNo());
    Log_UndoFile::Iterator  iter (file.PageAccessManager(), allocator);
    
    if ( ! iter.Initialize() )
    {
        errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"iter.Initialize()");
        GetInstance().AppendContextInfo(__CONTEXT__,errlist);
        return delInitError;
    }

    if(!file.GetUndoEntry (pageno, offset, Data_ForUpdate, 0, iter, errlist))
    {
        errlist = errlist + Log_Exception(__CONTEXT__, LOG_PAGEREF_INFO,
                                          "pageref",
                                          SAPDB_ToString(pageno),
                                          SAPDB_ToString(offset));
        errlist = errlist + Log_Exception(__CONTEXT__, LOG_PAGEREF_INFO,
                                          "oid",
                                          SAPDB_ToString(oid.gg92GetPno()),
                                          SAPDB_ToString(oid.gg92GetPos()));
        GetInstance().AppendContextInfo(__CONTEXT__,errlist);
        return delAccessingUndoEntryFailed;
    }
                            
    if ( ! iter.IsValid() )
    {
        if ( LogHistory_Trace.TracesLevel(6) )
        {
            Kernel_VTrace() << "history del: " << pageno << "." << offset << NewLine;
            Kernel_VTrace() << "*** Log_History::DeleteBeforeImage() entry not found" << NewLine;
        }
        (void)iter.Invalidate(errlist);
        (void)file.Invalidate(errlist);
        return delImageNotFound;
    }
    
    Log_BeforeImage       image;
    bool                  derefResult = true;
    Data_SplitSpaceReader reader (iter.Deref(derefResult,errlist));

    if (! derefResult)
    {
        errlist = errlist + Log_Exception(__CONTEXT__, LOG_PAGEREF_INFO,
                                          "pageref",
                                          SAPDB_ToString(pageno),
                                          SAPDB_ToString(offset));
        errlist = errlist + Log_Exception(__CONTEXT__, LOG_PAGEREF_INFO,
                                          "oid",
                                          SAPDB_ToString(oid.gg92GetPno()),
                                          SAPDB_ToString(oid.gg92GetPos()));
        GetInstance().AppendContextInfo(__CONTEXT__,errlist);
        return delAccessingUndoEntryFailed;
    }
    
    if(!image.ReadPersistentFormat (reader, errlist))
    {
        if ( LogHistory_Trace.TracesLevel(6) )
        {
            Kernel_VTrace() << "history del: " << pageno << "." << offset << NewLine;
            Kernel_VTrace() << "*** Log_History::DeleteBeforeImage() error reading image" << NewLine;
        }
        errlist = errlist + Log_Exception(__CONTEXT__, LOG_PAGEREF_INFO,
                                          "pageref",
                                          SAPDB_ToString(pageno),
                                          SAPDB_ToString(offset));
        errlist = errlist + Log_Exception(__CONTEXT__, LOG_PAGEREF_INFO,
                                          "oid",
                                          SAPDB_ToString(oid.gg92GetPno()),
                                          SAPDB_ToString(oid.gg92GetPos()));
        GetInstance().AppendContextInfo(__CONTEXT__,errlist);
        (void)iter.Invalidate(errlist);
        (void)file.Invalidate(errlist);
        return delReadError;
    }

    if ( image.GetActionType() != Log_DeleteObject )
    {
        if ( LogHistory_Trace.TracesLevel(6) )
        {
            Kernel_VTrace() << "history del: " << pageno << "." << offset << NewLine;
            Kernel_VTrace() << "*** Log_History::DeleteBeforeImage(): invalid action type '"
                            << Log_GetActionTypeString(image.GetActionType()) << NewLine;
            image.WriteToTrace("DeleteBeforeImage");
        }
        errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"action is DeleteObject");
        errlist = errlist + Log_Exception(__CONTEXT__, LOG_PAGEREF_INFO,
                                          "pageref",
                                          SAPDB_ToString(pageno),
                                          SAPDB_ToString(offset));
        errlist = errlist + Log_Exception(__CONTEXT__, LOG_PAGEREF_INFO,
                                          "oid",
                                          SAPDB_ToString(oid.gg92GetPno()),
                                          SAPDB_ToString(oid.gg92GetPos()));
        image.AppendContextInfo(errlist);
        GetInstance().AppendContextInfo(__CONTEXT__,errlist);
        (void)iter.Invalidate(errlist);
        (void)file.Invalidate(errlist);
        return delActionMustBeDelete;
    }

    Log_ActionObject action;

    if(!action.ReadPersistentFormat (reader, allocator, errlist))
    {
        if ( LogHistory_Trace.TracesLevel(6) )
        {
            Kernel_VTrace() << "history del: " << pageno << "." << offset  << NewLine;
            Kernel_VTrace() << "*** Log_History::DeleteBeforeImage() error reading action"  << NewLine;
            image.WriteToTrace("DeleteBeforeImage");
        }
        errlist = errlist + Log_Exception(__CONTEXT__, LOG_PAGEREF_INFO,
                                          "pageref",
                                          SAPDB_ToString(pageno),
                                          SAPDB_ToString(offset));
        errlist = errlist + Log_Exception(__CONTEXT__, LOG_PAGEREF_INFO,
                                          "oid",
                                          SAPDB_ToString(oid.gg92GetPno()),
                                          SAPDB_ToString(oid.gg92GetPos()));
        GetInstance().AppendContextInfo(__CONTEXT__,errlist);
        (void)iter.Invalidate(errlist);
        (void)file.Invalidate(errlist);
        return delReadError;
    }

    if ( ! action.GetOid().gg92IsEqIgnoreFrameVers(oid) )
    {
        if ( LogHistory_Trace.TracesLevel(6) )
        {
            Kernel_VTrace() << "history del: " << pageno << "." << offset  << NewLine;
            Kernel_VTrace() << "*** Log_History::DeleteBeforeImage() invalid oid"  << NewLine;
            image.WriteToTrace("DeleteBeforeImage");
            action.WriteToTrace("DeleteBeforeImage");
        }
        errlist = Log_Exception(__CONTEXT__, LOG_WRONG_HISTORY);
        image.AppendContextInfo(errlist);
        action.AppendContextInfo(__CONTEXT__,errlist);
        errlist = errlist + Log_Exception(__CONTEXT__, LOG_PAGEREF_INFO,
                                          "pageref",
                                          SAPDB_ToString(pageno),
                                          SAPDB_ToString(offset));
        errlist = errlist + Log_Exception(__CONTEXT__, LOG_PAGEREF_INFO,
                                          "expected",
                                          SAPDB_ToString(oid.gg92GetPno()),
                                          SAPDB_ToString(oid.gg92GetPos()));
        errlist = errlist + Log_Exception(__CONTEXT__, LOG_PAGEREF_INFO,
                                          "found",
                                          SAPDB_ToString(action.GetOid().gg92GetPno()),
                                          SAPDB_ToString(action.GetOid().gg92GetPos()));
        GetInstance().AppendContextInfo(__CONTEXT__,errlist);
        (void)iter.Invalidate(errlist);
        (void)file.Invalidate(errlist);
        return delWrongEntry;
    }

    if ( image.IsDeleted(false) )
    {
        if ( LogHistory_Trace.TracesLevel(6) )
        {
            Kernel_VTrace() << "history del: " << pageno << "." << offset  << NewLine;
            Kernel_VTrace() << "*** Log_History::DeleteBeforeImage() entry already deleted"  << NewLine;
            image.WriteToTrace("DeleteBeforeImage");
            action.WriteToTrace("DeleteBeforeImage");
        }
        (void)iter.Invalidate(errlist);
        (void)file.Invalidate(errlist);
        return delAlreadyDeleted;
    }

    // now invalidate the before image if all checks offer positive results ++++
    image.SetDeleted(true);
    
    (void)iter.Invalidate(errlist);
    (void)file.Invalidate(errlist);
    
    return delSuccessfull;
}

/* -------------------------------------------------------------------------------- */
bool Log_History::RemoveCompleteHistory (tgg00_TransContext   &trans,
                                         Msg_List &errlist)
{
    // PTS 1114877 UH 2002-03-18 new
    
    SAPDBTRACE_ROUTINE_DEBUG ("Log_History::RemoveCompleteHistory", LogHistory_Trace, 5);

    bool    anythingRemoved;
    bool    result = true;
    DummyGC dummygc;

    RTE_Message( Log_Exception(__CONTEXT__,LOG_BEGIN_REMOVE_COMPLETE_HISTORY) );

    for ( SAPDB_UInt fileno = 0; fileno < GetNumberOfHistoryFiles(); ++fileno )
    {
        anythingRemoved = false;
        if(!RemoveHistory (trans, fileno, Normal, dummygc, anythingRemoved, errlist))
        {
            RTE_Message(errlist);
            errlist.ClearMessageList();
        	result = false;
        }
    }

    RTE_Message( Log_Exception(__CONTEXT__,LOG_END_REMOVE_COMPLETE_HISTORY) );
    return result;
}

/* -------------------------------------------------------------------------------- */
bool Log_History::AnyHistoryExists ()
{
    // PTS 1114877 UH 2002-03-18 new
    SAPDBTRACE_ROUTINE_DEBUG ("Log_History::AnyHistoryExists", LogHistory_Trace, 5);
    for ( SAPDB_UInt fileno = 0; fileno < GetNumberOfHistoryFiles(); ++fileno )
    {
        // outside region => dirty read
        if ( ! GetOldestTransNo(fileno).gg90IsNil() )
            return true;
    }
    return false;
}

/* -------------------------------------------------------------------------------- */
bool Log_History::CheckAndRemoveUnusedHistory ( tgg00_TransContext   &trans,
                                                GC_IGarbageCollector &IGarbColl,
                                                Msg_List &errlist )
{
    // PTS 1115523 UH 2002-05-06 new
    
    SAPDBTRACE_ROUTINE_DEBUG ("Log_History::CheckAndRemoveUnusedHistory", LogHistory_Trace, 5);

    if ( GetNumberOfHistoryFiles() > GetNumberOfMaxUsedHistoryFiles() )
    {
        bool       anythingRemoved;
        bool       result = true;
        SAPDB_UInt fileno = 0;

        while ( m_Directory.GetNextUnusedFileNo(trans.trTaskId_gg00, fileno) )
        {
            if ( LogHistory_Trace.TracesLevel(6) )
                Kernel_VTrace() << "UNUSED HISTORY: REMOVE " << fileno;
            if(!RemoveHistory (trans, fileno, RemoveUnused, IGarbColl, anythingRemoved, errlist))
            {
                RTE_Message(errlist);
                errlist.ClearMessageList();
                result = false;
            }
            if ( IGarbColl.IsInterrrupted() )
            {
                m_Directory.ResetInProcess(trans.trTaskId_gg00, fileno);
                break;
            }
            m_Directory.Invalidate(trans.trTaskId_gg00, fileno);
        }
        return result;
    }
    return true;
}

/* --------------------------------------------------------------------------- */
bool Log_History::CheckAndStartMigrationIfNeeded (tgg00_TransContext &trans)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_History::CheckAndStartMigrationIfNeeded", LogHistory_Trace, 5);
    // The call of this routine assumes that the caller knows that all history can be removed now.
    // Otherwise it would loop endless here, but the loop can be cancelled by request.

    Kernel_Migration::ChangeInfo &change =
        Kernel_IAdminConfig::Instance().MigrationHandler().SearchForChangeInfo ("HISTORY ROOT ID CHECK");

    /* PTS 1134856 FF */
    Kernel_Migration::ChangeInfo &changeRecreateIndex   =
        Kernel_IAdminConfig::Instance().MigrationHandler().SearchForChangeInfo ("RecreateIndexForOmsKeyedObjects");

    if ( change.IsEnabled() || changeRecreateIndex.IsEnabled() )
    {
        pasbool &isCancelled = trans.trRteCommPtr_gg00->to_cancel;
        
        bd91StartOMSGarbageCollection (trans);
        if ( trans.trError_gg00 != e_ok )
            return false;
        
        RTE_Message( Log_Exception(__CONTEXT__,LOG_WAIT_FOR_REMOVE_HISTORY) );
    
        while ( AnyHistoryExists() && ! isCancelled )
            vsleep (trans.trTaskId_gg00, 20);
    
        if ( isCancelled )
        {
            RTE_Message( Log_Exception( __CONTEXT__, LOG_WAIT_FOR_REMOVE_HISTORY_CANCELLED,
                                        (AnyHistoryExists()?"exists":"does not exist") ) );
            isCancelled = false;
        }
        change.migrationEnabled = false;
        changeRecreateIndex.migrationEnabled = false;
    }
    return true;
}
