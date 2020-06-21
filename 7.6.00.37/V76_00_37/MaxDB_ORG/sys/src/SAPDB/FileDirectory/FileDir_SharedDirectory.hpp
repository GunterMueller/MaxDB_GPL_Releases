/*****************************************************************************/
/*!

  @file         FileDir_SharedDirectory.hpp
  @author       MartinKi
  @ingroup      FileDirectory_Files

  @brief        Header file for class FileDirectory.

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
/*****************************************************************************/

#ifndef FILEDIR_SHAREDDIRECTORY_HPP
#define FILEDIR_SHAREDDIRECTORY_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "FileDirectory/FileDir_Generic.hpp"
#include "FileDirectory/FileDir_SysKeyCache.hpp"
#include "FileDirectory/FileDir_GenericDirectory.hpp"
#include "FileDirectory/FileDir_ISharedDirectory.hpp"
#include "FileDirectory/FileDir_Vector.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"

#include "DataAccess/Data_Types.hpp"

#include "Converter/Converter_Version.hpp"

#include "RunTime/System/RTESys_AtomicOperation.hpp"
#include "RunTime/RTE_Types.hpp"

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

#include "ggg00.h"
#include "gsp00.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

struct tgg00_TransContext;

class FileDir_Entry;
class FileDir_PageManager;
class FileDir_RWRegionWrapper;
class FileDir_Iterator;
class FileDir_SingleIndexEntry;
class FileDir_Table;
class Kernel_Dump;
class Msg_List;
class SAPDBMem_IRawAllocator;
class Trans_Context;

/*!
 * @class FileDir_SharedDirectory
 * @ingroup FileDirectory_Classes
 * @author MartinKi
 *
 * @brief This class manages the permanent file directory.
 *
 * Regular users of the file directory should not include this file but use
 * FileDir_IDirectory.hpp instead.
 * @sa FileDir_IDirectory, FileDir_ISharedDirectory
 */
class FileDir_SharedDirectory : public FileDir_GenericDirectory,
                                public FileDir_ISharedDirectory
{
public:
    /// List of possible update actions for an entry in
    /// m_needUpdateList once the savepoint has finished.
    enum EntryUpdateAction {
        euaUpdateEntry,
        euaRemoveEntry
    };

private:
    static FileDir_SharedDirectory* m_fdir;

    // Allow iterator access to private members.
    friend class FileDir_Iterator;
    friend class FileDir_Directory;
    friend class FileDir_Test;

    friend class FileDir_Table; // needs access to syskey methods

    SAPDBMem_IRawAllocator& m_allocator;

    bool m_initialized;
    bool m_crashAfterFlush;    ///< Used for file directory tests.
    bool m_savepointActive;

    /// True during migration from old file directory.
    SAPDB_Bool m_migrationRunning;

    /// Number of entries that needed to have deltas applied since restart.
    SAPDB_UInt m_statisticsDeltaCount;

    /// Cache for current sysKey counter values for tables.
    FileDir_SysKeyCache m_sysKeyCache;

    Converter_Version m_converterVersion;

    FileDir_FileNo m_maxFileNo;
    FileDir_FileNo m_maxTempFileNo;
    FileDir_FileNo m_maxTempSharedFileNo;
    FileDir_FileNo m_maxTempSharedFileNoAllowed;

    FileDir_PageManager* m_pageManager;

    FileDir_RWRegionWrapper* m_lock;

    /// This lock is used to synchronise access to the list of entries
    /// that need to be updated once the savepoint has finished. In
    /// addition it is also used to protect set and clear the
    /// m_savepointActive flag.
    FileDir_RWRegionWrapper* m_updateListLock;

    /// Counts the number of files deleted since file directory
    /// instantiation.
    SAPDB_UInt m_delOpCounter;

    struct EntryUpdateInfo {
        FileDir_Generic   fileEntry;
        EntryUpdateAction action;

        EntryUpdateInfo(
            const FileDir_Generic&  fileEntry,
            const EntryUpdateAction action)
            : fileEntry( fileEntry ),
              action( action )
        {}
    };

    typedef FileDir_Vector<EntryUpdateInfo> UpdateInfoList;
    UpdateInfoList m_needUpdateList;

    typedef FileDir_Vector<FileDir_SingleIndexEntry*> SingleIndexList;
    SingleIndexList m_singleIndexList;

    SAPDB_UInt m_badIndexCount;

/***************************** Methods *****************************/

    void UpdatePersistentEntryParts(const RTE_TaskId taskId);

    void DetermineValidEntryVersion(
        FileDir_Entry&           pageEntry,
        const Converter_Version& converterVersion);

    /*!
     * @brief Recreates a file entry from its persistent data.
     *
     * @param pageEntry [in] a file directory entry whose persistent
     *                       data points to the address of the file
     *                       directory entry to be recreated
     */
    void RecreateFileEntry(
        const RTE_TaskId     taskId,
        FileDir_Entry&       pageEntry,
        SAPDB_Bool           bMigrate,
        FileDir_EntryLayouts migrationStartLayout);

private:

     /*!
     * @brief Constructs a file directory object
     */
    FileDir_SharedDirectory(SAPDBMem_IRawAllocator& alloc);

     /*!
     * @brief Initializes this object.
     *
     * @param initialCapacity [in] the initial capacity of the file
     *                             directory; if more entries will be
     *                             entered an automatic resize will take place
     */
    SAPDB_Bool InitializeSharedFDir(SAPDB_Bool isCreateFDir = false);

    // Copy and assignment operator are not supported

    FileDir_SharedDirectory& operator=( const FileDir_SharedDirectory &fdir );

    FileDir_SharedDirectory( const FileDir_SharedDirectory &fdir );

    /// Used by FileDir_Test.
    void CrashAfterFlush();

    /*!
     * @brief Returns the cache for table syskeys.
     */
    FileDir_SysKeyCache& GetSysKeyCache();

public:
    /// Creates and initializes the file directory singleton.
    static void CreateInstance();

    /// Returns a reference to file directory singleton.
    static FileDir_SharedDirectory& Instance();

    /*!
     * @name Startup/Shutdown
     */
    /*@{*/

    /*!
     * @brief Creates a new file directory starting with rootPageNo.
     *
     * @return true on success
     */
    FileDir_ReturnCode Create(RTE_TaskId  taskId);

    /*!
     * @brief Restarts the file directory by loading all file
     *        directory pages into memory and creating all file
     *        directory entries valid for converterVersion.
     *
     * This method also checks if a migration of the persistent file
     * directory layout is required and initiates it if necessary.
     *
     * @param trans            [in] trans context
     * @param rootPageNo       [in] page number of file directory root page
     * @param converterVersion [in] converter version of file directory
     *                              once restart has finished
     * @param restartConverterVersion [in] highest converter version valid
     *                              entries in file directory can have;
     *                              entries with a higher converter
     *                              version are either discarded or an
     *                              older version that was valid for
     *                              converterVersion is used
     * @param cleanupFileDirPages [in] if true, then all free space on
     * file directory pages will be compacted by moving all entries
     * next to each other in one block; if false the file directory
     * entries on the page will not be moved (useful for diagnose)
     * @param msgList [out] message list holding possible error messages
     *
     * @return
     * - FileDir_Okay,             if restart was successful
     * - FileDir_AlreadyRestarted, if file directory has already been
     *                             restarted (and not been shutdown in
     *                             the meantime)
     * - FileDir_InitFailed        if initialisation of file directory failed
     */
    FileDir_ReturnCode RestartFDir(
        tgg00_TransContext&      trans,
        const Data_PageNo&       rootPageNo,
        const Converter_Version& converterVersion,
        const Converter_Version& restartConverterVersion,
        const SAPDB_Bool         cleanupFileDirPages,
        Msg_List&                msgList);

    /*!
     * @brief Shut down the file directory. All in-memory structures
     *        for this FileDir-object and its entries are freed. No
     *        references to any file directory entries must exist when
     *        calling this method.
     *
     * @param taskId [in] Id of the calling task
     */
    FileDir_ReturnCode ShutdownSharedFDir(RTE_TaskId taskId);

    /*!
     * @brief Return the rootPageNo of the file directory
     *
     * @param taskId           [in]  task id of running task, or
     *                               RTE_UnknownTaskId if not available
     * @return rootPageNo
     */
    Data_PageNo GetRootPageNo(RTE_TaskId taskId = RTE_UnknownTaskId);

    FileDir_ReturnCode CheckDataWithUpdate(RTE_TaskId taskId);

    FileDir_ReturnCode CheckData(RTE_TaskId taskId);
    /*@}*/

    /*!
     * @name Adding new entries/deleting entries
     */
    /*@{*/

    FileDir_ReturnCode AddFileEntry(
        const RTE_TaskId         taskId,
        const FileDir_FileNo&    fileNo,
        FileDir_BaseEntry*       entry,
        Msg_List&                msgList,
        const LockingBehaviour   lockType = FileDir_UseLocking);

    /*!
     * @brief Return a new unused fileNo. This function does not
     *        recycle entries or check for duplicate fileNos.
     */
    FileDir_FileNo GetNewFileNo();

    /*!
     * @brief Return a new unused fileNo for a temporary object. This
     *        function does not recycle entries or check for duplicate
     *        fileNos.
     */
    FileDir_FileNo GetNewTempFileNo();

    /*!
     * @brief Return a new unused fileNo for a temporary object that
     *        is shared among all sessions. This function does not
     *        recycle entries or check for duplicate fileNos.
     */
    FileDir_FileNo GetNewTempSharedFileNo();

    /*!
     * @brief Return a new unused fileNo during migration from old
     *        file directory.
     */
    FileDir_FileNo MigrationGetNewFileNo();

    /*!
     * @brief Increase the highest internal file no counter used for
     * GetNewFile and MigrationGetNewFileNo by a constant value.
     *
     * This is necessary during migraton to create a hole in the
     * fileNo distribution between the tables and other entries
     * because these fileNos might be used during roll forward of log
     * (this safety margin might still be too small to accomodate all
     * fileNos that are required during roll forward though).
     */
    void AddFileNoSafetyMargin();

    /*!
     * @brief Adds a short column file whose table is not present in
     *        the file directory. Necessary to be able to
     *        roll back/roll forward log from earlier releases.
     */
    FileDir_ReturnCode AddLooseShortColumnFile(
        const RTE_TaskId         taskId,
        const FileDir_FileNo&    fileNo,
        const FileDir_FileNo&    parentFileNo,
        const Data_PageNo        rootPage,
        const tgg00_FiletypeSet& fileTypeSet,
        const tgg91_FileVersion& fileVersion,
        Msg_List&                msgList);
    /*@}*/


    /*!
     * @brief Adds an idnex file whose table is not present in
     *        the file directory. Necessary to be able to
     *        roll back/roll forward log from earlier releases.
     */
    FileDir_ReturnCode AddLooseIndexFile(
        const RTE_TaskId         taskId,
        const FileDir_FileNo&    fileNo,
        const FileDir_FileNo&    parentFileNo,
        const SAPDB_UInt2        indexId,
        const Data_PageNo        rootPage,
        const tgg00_FiletypeSet& fileTypeSet,
        const tgg91_FileVersion& fileVersion,
        Msg_List&                msgList);


    /*!
     * @name Migration from 7.3
     */
    /*@{*/

    /*!
     * @brief Adds a file directory entry for a 7.3 single index file.
     *
     * The entry created will not be part of the regular file
     * directory hash and can not be accessed via the usual
     * Get...File()-Methods. However, the entry is stored persistently
     * and will be recreated during each following restart until the
     * entry has been converted to a regular index entry via the
     * MigrationSwitchSingleIndexToMulti method.
     */
    FileDir_ReturnCode MigrationAddSingleIndexFile(
        const RTE_TaskId         taskId,
        const FileDir_FileNo&    fileNo,
        const FileDir_FileNo&    parentFileNo,
        const SAPDB_UInt2        indexId,
        const Data_PageNo        rootPage,
        const tgg00_FiletypeSet& fileTypeSet,
        const tgg91_FileVersion& fileVersion,
        Msg_List&                msgList);

    /*!
     * @brief Changes the file type of a file directory single index
     * entry to Data_IndexEntry.
     *
     * If successful, the entry will be entered into the regular file
     * directory hash.
     */
    FileDir_ReturnCode MigrationSwitchSingleIndexToMulti(
        const RTE_TaskId      taskId,
        const FileDir_FileNo& tableFileNo,
        const SAPDB_Int       oldSingleIndexId,
        const SAPDB_Int       newMultiIndexId,
        Msg_List&             msgList);

    /*!
     * @brief Deletes any leftover single index entries from the file
     * directory.
     *
     * For every entry removed a warning message will be written to
     * knldiag. The index file itself will be scheduled for deletion
     * by a server task.
     */
    FileDir_ReturnCode MigrationCleanupSingleIndexes(
        Trans_Context& transContext,
        Msg_List&      msgList);

    /*@}*/


    /*!
     * @brief Returns true if the fileNo given belongs to a temporary file.
     */
    SAPDB_Bool IsTempFile(FileDir_FileNo fileNo);

    /*!
     * @name Savepoint related
     */
    /*@{*/

    /*!
     * @brief Tells file directory that a savepoint is in progress.
     *
     * @param taskId           [in] task id of running task, or
     *                              RTE_UnknownTaskId if not available
     */
    void BeginSavepoint(RTE_TaskId taskId = RTE_UnknownTaskId);

    /*!
     * @brief Tells file directory to flush all dirty pages to disk.
     *
     * @param taskId           [in] task id of running task, or
     *                              RTE_UnknownTaskId if not available
     */
    void Flush(RTE_TaskId taskId = RTE_UnknownTaskId);

    /*!
     * @brief Tells file directory that savepoint has completed. All
     *        entries will have their persistent data parts updated
     *        with any deltas that might have accumulated during the
     *        savepoint.
     *
     * @param taskId           [in] task id of running task, or
     *                              RTE_UnknownTaskId if not available
     * @param newConverterVersion [in] new converter version for the
     *                                 file directory
     */
    void EndSavepoint(
        RTE_TaskId taskId,
        const Converter_Version& newConverterVersion );

    /*!
     * @brief Returns true if a savepoint is in progress.
     */
    SAPDB_Bool IsSavepointInProgress();

    void WriteLockNeedUpdateList(const RTE_TaskId taskId);
    void WriteUnlockNeedUpdateList(const RTE_TaskId taskId);
    void ReadLockNeedUpdateList(const RTE_TaskId taskId);
    void ReadUnlockNeedUpdateList(const RTE_TaskId taskId);

    /*!
     * @brief Adds fileNo to a list of entries that need updating once
     *        the savepoint has finished.
     */
    void AddToNeedUpdateList(
        const RTE_TaskId        taskId,
        const FileDir_FileNo    fileNo,
        const EntryUpdateAction action);

    /*!
     * @brief Returns the current converter version of the file directory.
     */
    Converter_Version GetConverterVersion();

    /*@}*/

    /// Clean up file directory pages. <em>No-op at this time</em>
    void DefragPages() const;

    /*!
     * @brief Returns the highest file id that has been handed out by
     *        the file directory.
     */
    FileDir_FileNo GetMaxFileNo() const;

    /*!
     * @brief Returns the highest file id for temp-files that has been
     *        handed out by the file directory.
     */
    FileDir_FileNo GetMaxTempFileNo() const;

    /*!
     * @brief Set the highest file id of the file directory. If
     *        maxFileNo is less than the internal file id counter,
     *        the internal file id counter will not be
     *        changed. <em>Can only be called during restart!</em>
     */
    void Restart_SetFileNoHighWaterMark(FileDir_FileNo maxFileNo);

    /*!
     * @brief Returns an iterator over all file directory entries.
     */
    SAPDB_Bool GetFileDirIterator(
        FileDir_Iterator&   iterator,
        tgg00_TransContext* transContext = 0);

    SAPDB_Bool GetPrivateFileDirIterator(
        FileDir_Iterator&   iterator,
        tgg00_TransContext* transContext = 0);

    SAPDB_Bool GetSharedFileDirIterator(
        FileDir_Iterator&   iterator,
        tgg00_TransContext* transContext = 0);

    /*!
     * @brief Removes a file directory entry from the file directory
     *        and destroys it. This method should only be called by
     *        the garbage collector.
     *
     * @copydoc FileDir_GenericDirectory::DelFile
     */
    FileDir_ReturnCode DelFile(
        RTE_TaskId            taskId,
        const FileDir_FileNo& fileNo,
        Msg_List&             msgList);

    SAPDB_UInt GetDeleteOpCounter() const;

    /*!
     * @name Bad Indices
     */
    /*@{*/

    /*!
     * @brief Returns the number of bad indices in the shared file directory.
     */
    SAPDB_UInt4 GetBadIndexCount() const;

    /*!
     * @brief Increments the number of bad indices in the shared file
     *        directory.
     */
     void IncrementBadIndexCount();

    /*!
     * @brief Increments the number of bad indices in the shared file
     *        directory.
     */
     void DecrementBadIndexCount();

    /*@}*/

/**************************************************************************/

    /*!
     * @brief Returns the number of pages on disk used by the file
     *        directory.
     */
    SAPDB_UInt GetPageCount() const;

    /*!
     * @brief Marks the given page to be flushed to disk during the
     *        next savepoint.
     *
     * @param taskId   [in] task id of calling task
     * @param pageNo   [in] page number of file directory page to be
     *                      marked dirty
     *
     * @return
     *   - true, on success
     *   - false, if pageNo is not part of the file directory
     */
    SAPDB_Bool MarkPageForWrite(
        const RTE_TaskId   taskId,
        const Data_PageNo& pageNo);

    /*!
     * @brief Write contents of shared directory to kernel dump.
     *
     * @param taskId task id of calling task
     * @param dump   dump to write contents to
     */
    void Dump(const RTE_TaskId taskId, Kernel_Dump& dump);

    /*!
     * @brief Write contents of shared file directory pages to file.
     *
     * @param taskId   [in] task id of calling task
     * @param fileName [in] fileName of file to write pages to
     */
    void Extract(
        const RTE_TaskId       taskId,
        const tsp00_VFilename& fileName);

    /*******************************************************************/

    /*!
     * @brief Returns the number of entries in the directory.
     */
    SAPDB_UInt GetEntryCount() const;

    /*!
     * @brief Returns the number of entries in the directory.
     */
    SAPDB_UInt GetHashTableSlotCount() const;

    /*!
     * @brief Returns the number of entries in the directory.
     */
    SAPDB_UInt GetUsedHashTableSlotCount() const;

    /*!
     * @brief Returns the number of entries in the directory.
     */
    SAPDB_UInt GetMaxHashTableChainEntryCount() const;

    /*!
     * @brief Returns the number of entries in the directory.
     */
    SAPDB_UInt GetResizeHashTableCount() const;

    /*!
     * @brief Returns the number of entries in the directory.
     */
    SAPDB_UInt GetStatisticsDeltaCount() const;

    /*!
     * @brief Returns the number of entries in the directory.
     */
    SAPDB_UInt GetCurrentStatisticsDeltaCount() const;

    /*!
     * @brief Removes the list of entries whose file id would be
     *        remapped during the first savepoint. Needed during file
     *        directory migration.
     */
    void SetMigrationRunning(SAPDB_Bool running);

/*************************************************************************/
/*************************************************************************/
/*************************************************************************/
    /// Necessary for old style file directory entries; Do not use!
    FileDir_GenericDirectory* GetBaseClass();
};

/**********************************************************************/

inline FileDir_SharedDirectory& FileDir_SharedDirectory::Instance()
{
    SAPDBERR_ASSERT_STATE( m_fdir != 0 );
    return *m_fdir;
}

/**********************************************************************/

inline Converter_Version FileDir_SharedDirectory::GetConverterVersion()
{
    return RTESys_AtomicRead(
        *reinterpret_cast<SAPDB_UInt4*>(&m_converterVersion) );
}

/**********************************************************************/

inline FileDir_GenericDirectory* FileDir_SharedDirectory::GetBaseClass()
{
    return this;
}

/**********************************************************************/

inline FileDir_SysKeyCache&
FileDir_SharedDirectory::GetSysKeyCache()
{
    return m_sysKeyCache;
}

/**********************************************************************/

#endif // FILEDIR_SHAREDDIRECTORY_HPP
