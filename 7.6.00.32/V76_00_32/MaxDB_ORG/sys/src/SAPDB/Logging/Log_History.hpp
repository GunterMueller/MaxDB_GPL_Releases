/*! @defgroup     History History Manager */
/*!
  @file           Log_History.hpp
  @ingroup        History
  @author         UweH
  @brief          The HistoryManager is a singleton.

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
#ifndef Log_HISTORY_H
#define Log_HISTORY_H

#include "gsp00.h" // tsp00_TaskId
#include "ggg00.h"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "Container/Container_Vector.hpp"
#include "DataAccess/Data_Types.hpp"
#include "Logging/Log_HistoryFile.hpp"
#include "Logging/Log_HistoryDirectory.hpp"
#include "GarbageCollector/GC_IGarbageCollector.hpp"
#include "RunTime/Synchronisation/RTESync_Spinlock.hpp"

class Log_UndoFile;
class tgg92_KernelOid;
class SAPDBMem_IRawAllocator;

/*!
	@class  Log_History
    @brief  This is the history manager.
*/

class Log_History
{

public:
    /// This defines the information of one history file within the history directory
    typedef Log_HistoryDirectory::HistoryFileInfo HistoryFileInfo;

    /// Gives the granularity of history to be deleted.
    enum  RemoveOptions
    {
        Normal,        ///< "phase green"  Only easy to remove history is removed.
        Sensitive,     ///< "phase yellow" All history which can be is removed, this is more expensive.
        Truncate,      ///< "phase red"    The oldest history is removed, even if it is needed by views.
        RemoveUnused   ///< for internal use only
    };

public:

	/// This method creates the singleton. Ona any error false is returned.
    static bool CreateInstance (tgg00_TransContext   &trans,
                                Msg_List &errlist);

    /// This deallocates all allocated resources.
    static void DropInstance();

    /// this calls Delete()
    ~Log_History()
    {
        DropInstance();
    }

    /*!
        @param old trans context
        @param new history directory root to be written to the restart record.
        @param errlist
        @brief All data is flushed to disk. This is used by the savepoint.
    */
    bool Flush (tgg00_TransContext   &trans,
                Data_PageNo          &newHistRoot,
                Msg_List &errlist);

    /// returns true, if all files.Verify() return true.
    bool Verify (tgg00_TransContext   &trans,
                 bool                  isCold,
                 Msg_List &errlist);

	/// A transaction calls this, if it produced before images needed by other transactions
    /// to view data consistent.
    bool RegisterUndoFile ( tgg00_TransContext   &trans,
                            Log_UndoFile         &file,
                            Msg_List &errlist );

    /// This is the result code of GetBeforeImage().
    enum GetResult
    {
        getImageFound,
        getInitError,
        getIllegalPageRef,
        getAccessingUndoEntryFailed,
        getImageNotFound,
        getReadError,
        getWrongEntry,
        getBodySizeTooSmall,
        getActionNotPreparedForConsistentRead,
        getImageNotFoundBecauseDeleted,
        getReleasingImageFailed
    };
    /// this offers a before image mathing the given consistent view.
    static GetResult GetBeforeImage (
                                tgg00_TransContext       &Trans,
                                const tgg92_KernelOid    &Oid,
                                const tgg91_PageRef      &WantedPageRef,
                                SAPDB_UInt4               BodySize,
                                SAPDB_Byte               *pObjBody,
                                bool                      bOnlyCheckExistence, // PTS 1113317 UH 2002-01-08
                                SAPDB_UInt4              &BodyLen,
                                tgg00_ObjFrameVers       &FrameVers,
                                Log_ActionType           &ActionType,
                                tgg91_TransNo            &ConsistentView,
                                tgg91_TransNo            &PrevUpdTrans,
                                tgg91_PageRef            &NextPageRef,
                                Msg_List     &errlist);

    static GetResult GetBeforeImage (tgg00_TransContext   &Trans,
                                tgg91_PageRef        &WantedPageRef,
                                tgg91_TransNo        &ConsistentView,
                                tgg00_Rec            &BeforeImage,
                                tgg91_TransNo        &UpdTrans,
                                Msg_List &errlist);
    /// returns true, if the given Id references a valid before image.
    static bool BeforeImageExists (tgg00_TransContext    &Trans,
                                   const tgg92_KernelOid &testoid,
                                   const tgg91_PageRef   &wanted)
    {
        SAPDB_UInt4          BodyLen;
        tgg00_ObjFrameVers   FrameVers;
        Log_ActionType       ActionType = Log_NoOp;
        tgg91_TransNo        ConsistentView;
        tgg91_TransNo        PrevUpdTrans;
        tgg91_PageRef        NextPageRef;
        Msg_List errlist;
        const GetResult      result =
        GetBeforeImage ( Trans, testoid, wanted, 0, NULL, true, // PTS 1113317 UH 2002-01-08
                         BodyLen, FrameVers, ActionType,
                         ConsistentView, PrevUpdTrans, NextPageRef, errlist );

        if ( result != getImageFound || e_ok != Trans.trError_gg00  ) 
        {
            if ( e_illegal_page_no != Trans.trError_gg00 )
                Trans.trError_gg00 = e_ok;
            return false;
        }
        
        return true;
    }

	/// returns true, if is used.
    static bool IsHistoryUsed (tsp00_TaskId        taskid,
                               tgg92_KernelOid    &oid,
                               tgg00_ObjTransInfo &info);

    /// the result of DeleteBeforeImage()
    enum DeleteResult
    {
        delIllegalPageref,
        delInitError,
        delAccessingUndoEntryFailed,
        delImageNotFound,
        delReadError,
        delActionMustBeDelete,
        delWrongEntry,
        delAlreadyDeleted,
        delSuccessfull
    };
    /// A before image is marked as deleted
    static DeleteResult DeleteBeforeImage (tgg00_TransContext    &trans,
                                   const tgg92_KernelOid &oid,
                                   const tgg91_PageRef   &pageref,
                                   Msg_List  &errlist);

	/// The history manager singleton is returned.
	/// It is assumed, that the component is initialized.
    //  +++ THIS IS PUBLIC UNTIL THERE ARE EXISTING DIFFERENT INTERFACES +++
    static Log_History& GetInstance()
    {
        SAPDBERR_ASSERT_STATE (m_Instance != NULL);
        return( *m_Instance );
    }

    /// returns true, if GetInstance() can be used.
    static bool IsInitialized()
    {
        return m_Instance != NULL;
    }

    /// accessor
    SAPDB_UInt GetNumberOfHistoryFiles () const
    {
        return m_Directory.GetFileCount();
    }
                   
    /// accessor
    SAPDB_UInt GetNumberOfMaxUsedHistoryFiles () const
    {
        return m_Directory.GetMaxUsedFileCount();
    }
                   
    /// returns the oldest transo of the given history file
    const tgg91_TransNo & GetOldestTransNo (SAPDB_UInt fileno) const
    {
        return m_Directory.GetOldestTransNo (fileno);
    }
                   
    /// returns the oldest transo of the given history file
	/// The range is between 0 and (GetNumberOfHistoryFiles() - 1)
	/// it's unsynchronized
    void GetInfo ( tsp00_TaskId     taskid,
                   SAPDB_UInt       fileno,
                   HistoryFileInfo &info,
                   SAPDB_UInt      &pagecount ) const
    {
        m_Directory.GetHistoryFileInfo( fileno, info, pagecount );
    }
                   
	/// This call removes history depending on the given options.
    bool RemoveHistory (tgg00_TransContext        &trans,
                        SAPDB_UInt                 fileno,
                        RemoveOptions              option,
                        GC_IGarbageCollector      &IGarbColl,                    //FF 2002-02-12 
                        bool                      &anythingRemoved,
                        Msg_List      &errlist);

    /*!
        @brief This method removes the complete history including the history directory.
       
        The anchor in the restartrecord is deleted, even the memory is deallocated.
        After this method returns the history management must be created new.
        This can be used for migration prupose.
     */
    bool RemoveCompleteHistory (tgg00_TransContext   &trans,
                                Msg_List &errlist);

    /// This is used by transactions to remove their undo file directly
    bool RemoveUndoFileByTransaction (tgg00_TransContext        &trans,             // PTS 1113934 UH 2002-02-01
                                      const tgg91_TransNo       &transno,
                                      Data_PageNo                root,
                                      Data_PageNo                last,
                                      Msg_List                  &errlist);
    /*!
        @brief This method removes history files which are not used if there are any existing.
        History files can be unused if MAXUSERTASKS has been decreased.
     */

    bool CheckAndRemoveUnusedHistory ( tgg00_TransContext   &trans,
                                       GC_IGarbageCollector &IGarbColl,
                                       Msg_List &errlist );

	/// this is used by garbage collector dump bd930DumpHistoryFileDir()
    const void* GetDumpInfo (SAPDB_UInt fileno)
    {
        return m_Directory.GetDumpInfo(fileno);
    }

	/// returns true, if any history directory entry has a valid transno. Dirty Read / Outside region !
    bool AnyHistoryExists (); // PTS 1114877 UH 2002-03-18 new

    /// This is used for migration to get rid of really all history informations
    /// On any error false is returned.
    bool CheckAndStartMigrationIfNeeded (tgg00_TransContext &trans);
    
#ifdef CONSTRUCTOR_MAY_NOT_BE_PRIVATE_AND_NO_FRIEND_DEFINED
protected:
#else
private:
#endif
    /// forbidden to use, because this is a singleton
    Log_History (const Log_History& other);
    /// forbidden to use, because this is a singleton
    Log_History& operator= (const Log_History &Manager);
                     
#ifdef CONSTRUCTOR_MAY_NOT_BE_PRIVATE_AND_NO_FRIEND_DEFINED
private:
#endif

	/// the constructor for the singleton
    Log_History (SAPDBMem_IRawAllocator &allocator,
                 SAPDB_UInt              numUKTs,
                 SAPDB_UInt              maxFiles)
    : m_Directory (allocator, numUKTs, maxFiles)
    {}

	/// This creates Historyfiles and initializes the directory.
    bool CreateHistoryDirectory (tgg00_TransContext   &trans,
                                 Msg_List &errlist);

	/// This creates Historyfiles.
	bool CreateHistoryFiles (tgg00_TransContext   &trans,
							 Log_HistoryDirectory &dirfile,
							 SAPDB_UInt            firstFileNo,
							 SAPDB_UInt            lastFileNo,
                             Msg_List &errlist);
    
	/// This restores the directory.
    bool RestoreHistoryDirectory (tgg00_TransContext   &trans,
                                  Msg_List &errlist);

	/// returns false on any error
    bool UpdateHistoryFileInfo ( tgg00_TransContext   &trans,
                                 SAPDB_UInt            historyFileNo,
                                 HistoryFileInfo      &fileinfo,
                                 SAPDB_UInt           &pagecount,
                                 Msg_List &errlist );
                                 
    /*!
        @return true, if UndoFile was really removed.
    	@brief  this removes an undo file an all objects which are deleted in it.
        isInterrupted can becomes true, if the garbage collection should be aborted.
    */
    // PTS 1113934 UH 2002-02-01
    bool RemoveUndoFile (tgg00_TransContext        &trans,             // PTS 1113934 UH 2002-02-01
                         const tgg91_TransNo       &transno,
                         Data_PageNo                root,
                         Data_PageNo                last,
                         GC_IGarbageCollector      &IGarbColl,         // FF 2002-02-12 
                         SAPDB_UInt                 historyFileNo,
                         Log_HistoryFile           &historyfile,       // PTS 1113934 UH 2002-02-01
                         Log_HistoryFile::Iterator &undoFileInfoIter,
                         bool                      &anythingRemoved,
                         bool                      &dropAuxFilesNeeded, // PTS 1134156 UH 2005-02-25
                         Msg_List                  &errlist);          // PTS 1113934 UH 2002-02-01

    /*!
        @return true, if UndoFile was really removed.
    	@brief  this removes an undo file an all objects which are deleted in it.
        isInterrupted can becomes true, if the garbage collection should be aborted.
    */
    // PTS 1113934 UH 2002-02-01
    bool ProcessUndoFile (tgg00_TransContext        &trans,             // PTS 1113934 UH 2002-02-01
                          const tgg91_TransNo       &transno,
                          Data_PageNo                root,
                          Data_PageNo                last,
                          SAPDB_UInt                 historyFileNo,
                          Log_HistoryFile           *pHistoryFile,
                          Log_HistoryFile::Iterator *pUndoFileInfoIter,
                          GC_IGarbageCollector      &IGarbColl,         // FF 2002-02-12 
                          bool                      &anythingRemoved,
                          bool                      &dropAuxFilesNeeded, // PTS 1134156 UH 2005-02-25
                          Msg_List                  &errlist);          // PTS 1113934 UH 2002-02-01

    /// removal of the oldest transo
    bool TruncateHistory (tgg00_TransContext        &trans,
                          SAPDB_UInt                 fileno,
                          Data_PageNo                root,
                          Data_PageNo                last,
                          GC_IGarbageCollector      &IGarbColl,                            // FF 2002-02-12 
                          bool                      &anythingRemoved,
                          Msg_List      &errlist);
                          
    /// the oldest transno is determines
    bool DetermineOldestTransNo (Log_HistoryFile      &file,
                                 tgg91_TransNo        &oldestTransno,
                                 Msg_List &errlist);

    /// is used to collect informations about the context
    void AppendContextInfo (SAPDB_Char  const * const  filename,
                            SAPDB_UInt4 const          linenumber,
                            Msg_List      &errlist,
                            SAPDB_Char  const * const  title = 0) const
	{
        m_Directory.AppendContextInfo(filename,linenumber,errlist,title);
	}
    
public:

	/// This defines the directory in main memory.
    class HistoryDirectory
    {
    public:
        /// constructor
        HistoryDirectory ( SAPDBMem_IRawAllocator& allocator,
                           SAPDB_UInt              numUKTs,
                           SAPDB_UInt              maxFiles )
        : m_Directory (allocator),
          m_Spinlocks (allocator),
          m_numUKTs   (numUKTs),
          m_maxFiles  (maxFiles)
        {}
        
        /// initializes this history directory
        bool Initialize()
        {
            if ( ! m_Directory.Initialize(GetMaxUsedFileCount()) )
                return false;
            return m_Spinlocks.Initialize(m_numUKTs);
        }
        
        /// delete this history directory
        void Delete()
        {
            m_Directory.Delete();
            m_Spinlocks.Delete();
        }
        
        /// deconstructor
        ~HistoryDirectory()
        {
            Delete();
        }
        
        /// returns the number of files in the history directory
        SAPDB_UInt GetFileCount() const
        {
            return m_Directory.GetSize();
        }

        /// returns the upper limit of this filedirectory
        SAPDB_UInt GetMaxUsedFileCount() const
        {
            return m_maxFiles;
        }

        /// returns the fileNo for the corresponding task
        SAPDB_Int GetFileNo(tsp00_TaskId taskid) const
        {
            return taskid % GetMaxUsedFileCount();
        }

        /// returns the oldest known transaction id
        const tgg91_TransNo & GetOldestTransNo (SAPDB_UInt fileno) const
        {
            return m_Directory[fileno].persistent.oldestTransNo;
        }

        /// sets the values of the ileinfo
        bool SetHistoryFileInfo ( tsp00_TaskId           taskid,
                                  SAPDB_UInt             fileno,
                                  const HistoryFileInfo& info,
                                  SAPDB_UInt             pagecount,
                                  Msg_List  &errlist );

        /// reads the values of the fileinfo unsynchronized
        void GetHistoryFileInfo ( SAPDB_UInt       fileno,
                                  HistoryFileInfo &info,
                                  SAPDB_UInt      &pagecount ) const;

        /// reads the values of the fileinfo synchronized
        void GetHistoryFileInfo ( tsp00_TaskId     taskid,
                                  SAPDB_UInt       fileno,
                                  HistoryFileInfo &info,
                                  SAPDB_UInt      &pagecount );

        /// updates the new undo-information for the specified transNo
        void UpdateInfoAfterRegisterNewUndoFile ( tsp00_TaskId         taskid,
                                                  SAPDB_UInt           fileno,
                                                  const tgg91_TransNo &newtransno,
                                                  SAPDB_UInt           pagecount,
                                                  Data_PageNo          last );

        /// updates removed undo-information for the specified transNo
        void UpdateOldestTransNo ( tsp00_TaskId         taskid,
                                   SAPDB_UInt           fileno,
                                   const tgg91_TransNo &newOldestTransno,
                                   Data_PageNo          last,
                                   SAPDB_UInt           removedPages );
    
        /// dumps the filedirectory for specific fileno
        const void* GetDumpInfo (SAPDB_UInt fileno)
        {
            return &(m_Directory[fileno]);
        }
        

        /*!
            The next fileno of an unused history file is given.
            The history directory entry is marked as used so no other
            requestor will ever get this entry again.
            Usually fileno is set to 0 at the beginning.
            If false is returned there exists no file which is unused.
        */
        bool GetNextUnusedFileNo ( tsp00_TaskId  taskid,
                                   SAPDB_UInt   &fileno );
    
        /// if the size has been decreased, removes unused entries after the restart
        void RemoveUnusedEntries ();

        /// resets the flag for a file, that it is processed by the GarbageCollector
        void ResetInProcess (tsp00_TaskId taskid,
                             SAPDB_UInt   fileno);

        /// set the state of a file to invalide
        void Invalidate (tsp00_TaskId taskid,
                         SAPDB_UInt   fileno);

        /// dumps the file to the trace
        void WriteToTrace (tsp00_TaskId      taskid,
                           const SAPDB_Char *title = NULL) const;

        /// is used to collect informations about the context
        void AppendContextInfo (SAPDB_Char  const * const  filename,
                                SAPDB_UInt4 const          linenumber,
                                Msg_List      &errlist,
                                SAPDB_Char  const * const  title = 0) const;

    public:

        /// one entry in the HistoryDirectory
        struct ExtendedHistoryFileInfo
        {
            HistoryFileInfo persistent;     ///< Information about the history file
            SAPDB_UInt4     pagecount;      ///< Sum of pages in UndoFiles
            SAPDB_Bool1     inProcess;      ///< this entry is already inProcess by the GC
            SAPDB_Bool1     filler1;        ///< filler
            SAPDB_UInt2     filler2;        ///< filler
            ExtendedHistoryFileInfo()
            :pagecount(0),
             inProcess(false),
             filler1(0),
             filler2(0)
            {}
        };
        
    private:
    
        void Lock(SAPDB_UInt fileno)
        {
            m_Spinlocks[fileno%m_numUKTs]->Lock();
        }

        void Unlock(SAPDB_UInt fileno)
        {
            m_Spinlocks[fileno%m_numUKTs]->Unlock();
        }

    private:
    
        typedef Container_Vector<ExtendedHistoryFileInfo> Directory;
        
        class SpinLocks : public Container_Vector<RTESync_Spinlock*>
        {
        public:
            SpinLocks(SAPDBMem_IRawAllocator &allocator)
            : Container_Vector<RTESync_Spinlock*>(allocator)
            {}
            
            bool Initialize(SAPDB_UInt size)
            {
                if ( ! Container_Vector<RTESync_Spinlock*>::Initialize(size) )
                    return false;
                
                for ( Iterator i = Begin(); i != End(); ++i )
                {
                    *i = new(GetAllocator()) RTESync_Spinlock();
                    if ( 0 == *i )
                    {
                        Delete();
                        return false;
                    }
                    (*i)->SetName((SAPDB_UTF8*)"Historylog");
                }
                return true;
            }
            
            void Delete()
            {
                for ( Iterator i = Begin(); i != End(); ++i )
                    destroy(*i,GetAllocator()); 
                Container_Vector<RTESync_Spinlock*>::Delete();
            }

            ~SpinLocks()
            {
                Delete();
            }
        };

        Directory        m_Directory;
        const SAPDB_UInt m_numUKTs;
        const SAPDB_UInt m_maxFiles;
        SpinLocks        m_Spinlocks;

    };
    
    typedef HistoryDirectory::ExtendedHistoryFileInfo ExtendedHistoryFileInfo;     ///< shortcut

    HistoryDirectory m_Directory;           ///< the accesspoint for the directory
    Data_PageNo      m_DirectoryRoot;       ///< the root of the history directory from the savepoint
    
private:

    static Log_History* m_Instance;
};
#endif  /* Log_HISTORY_H */
